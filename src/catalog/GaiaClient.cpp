#include "starmap/catalog/GaiaClient.h"
#include "starmap/utils/HttpClient.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace starmap {
namespace catalog {

// URL del servizio TAP di GAIA
const std::string GAIA_TAP_URL = "https://gea.esac.esa.int/tap-server/tap";

class GaiaClient::Impl {
public:
    Impl() : tapUrl_(GAIA_TAP_URL), timeout_(60) {}

    std::string tapUrl_;
    int timeout_;
    utils::HttpClient httpClient_;
};

GaiaClient::GaiaClient() : pImpl_(std::make_unique<Impl>()) {}

GaiaClient::~GaiaClient() = default;

std::string GaiaClient::buildADQLQuery(const GaiaQueryParameters& params) const {
    std::ostringstream query;
    
    query << "SELECT TOP " << params.maxResults << " "
          << "source_id, ra, dec, phot_g_mean_mag";
    
    if (params.includeParallax) {
        query << ", parallax";
    }
    
    if (params.includeProperMotion) {
        query << ", pmra, pmdec";
    }
    
    // Color index (BP-RP)
    query << ", bp_rp";
    
    query << " FROM gaiadr3.gaia_source "
          << "WHERE 1=CONTAINS("
          << "POINT('ICRS', ra, dec), "
          << "CIRCLE('ICRS', "
          << std::fixed << std::setprecision(6)
          << params.center.getRightAscension() << ", "
          << params.center.getDeclination() << ", "
          << params.radiusDegrees << "))";
    
    if (params.maxMagnitude < 30.0) {
        query << " AND phot_g_mean_mag < " << params.maxMagnitude;
    }
    
    query << " ORDER BY phot_g_mean_mag ASC";
    
    return query.str();
}

std::vector<std::shared_ptr<core::Star>> GaiaClient::parseVOTable(
    const std::string& votable) const {
    
    std::vector<std::shared_ptr<core::Star>> stars;
    
    // Parser semplificato per VOTable
    // In produzione si userebbe una libreria XML (es. libxml2 o pugixml)
    
    size_t pos = 0;
    while ((pos = votable.find("<TR>", pos)) != std::string::npos) {
        size_t endPos = votable.find("</TR>", pos);
        if (endPos == std::string::npos) break;
        
        std::string row = votable.substr(pos, endPos - pos);
        
        auto star = std::make_shared<core::Star>();
        
        // Estrai i valori TD
        std::vector<std::string> values;
        size_t tdPos = 0;
        while ((tdPos = row.find("<TD>", tdPos)) != std::string::npos) {
            tdPos += 4;
            size_t endTd = row.find("</TD>", tdPos);
            if (endTd != std::string::npos) {
                values.push_back(row.substr(tdPos, endTd - tdPos));
                tdPos = endTd;
            }
        }
        
        if (values.size() >= 4) {
            try {
                // source_id, ra, dec, mag
                star->setGaiaId(std::stoll(values[0]));
                
                double ra = std::stod(values[1]);
                double dec = std::stod(values[2]);
                star->setCoordinates(core::EquatorialCoordinates(ra, dec));
                
                star->setMagnitude(std::stod(values[3]));
                
                // Parallax se presente
                if (values.size() > 4 && !values[4].empty() && values[4] != "NaN") {
                    star->setParallax(std::stod(values[4]));
                }
                
                // Proper motion
                if (values.size() > 6) {
                    if (!values[5].empty() && values[5] != "NaN") {
                        star->setProperMotionRA(std::stod(values[5]));
                    }
                    if (!values[6].empty() && values[6] != "NaN") {
                        star->setProperMotionDec(std::stod(values[6]));
                    }
                }
                
                // Color index
                if (values.size() > 7 && !values[7].empty() && values[7] != "NaN") {
                    star->setColorIndex(std::stod(values[7]));
                }
                
                stars.push_back(star);
            } catch (const std::exception&) {
                // Ignora righe malformate
            }
        }
        
        pos = endPos;
    }
    
    return stars;
}

std::vector<std::shared_ptr<core::Star>> GaiaClient::queryRegion(
    const GaiaQueryParameters& params) {
    
    std::string adqlQuery = buildADQLQuery(params);
    
    // Prepara la richiesta TAP
    std::ostringstream requestUrl;
    requestUrl << pImpl_->tapUrl_ << "/sync?REQUEST=doQuery&LANG=ADQL&FORMAT=votable&QUERY=";
    
    // URL-encode della query
    std::string encodedQuery;
    for (char c : adqlQuery) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encodedQuery += c;
        } else if (c == ' ') {
            encodedQuery += '+';
        } else {
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02X", (unsigned char)c);
            encodedQuery += hex;
        }
    }
    
    requestUrl << encodedQuery;
    
    pImpl_->httpClient_.setTimeout(pImpl_->timeout_);
    
    try {
        std::string response = pImpl_->httpClient_.get(requestUrl.str());
        return parseVOTable(response);
    } catch (const std::exception& e) {
        // Log error
        return std::vector<std::shared_ptr<core::Star>>();
    }
}

std::shared_ptr<core::Star> GaiaClient::queryById(long long gaiaId) {
    std::ostringstream query;
    query << "SELECT source_id, ra, dec, phot_g_mean_mag, parallax, "
          << "pmra, pmdec, bp_rp FROM gaiadr3.gaia_source "
          << "WHERE source_id = " << gaiaId;
    
    std::ostringstream requestUrl;
    requestUrl << pImpl_->tapUrl_ << "/sync?REQUEST=doQuery&LANG=ADQL&FORMAT=votable&QUERY=";
    
    std::string encodedQuery;
    for (char c : query.str()) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encodedQuery += c;
        } else if (c == ' ') {
            encodedQuery += '+';
        } else {
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02X", (unsigned char)c);
            encodedQuery += hex;
        }
    }
    
    requestUrl << encodedQuery;
    
    try {
        std::string response = pImpl_->httpClient_.get(requestUrl.str());
        auto stars = parseVOTable(response);
        if (!stars.empty()) {
            return stars[0];
        }
    } catch (const std::exception&) {
        // Gestisci errore
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<core::Star>> GaiaClient::queryBox(
    const core::EquatorialCoordinates& center,
    double widthDeg,
    double heightDeg,
    double maxMagnitude) {
    
    // Converte box in query circolare approssimativa
    double radius = std::sqrt(widthDeg * widthDeg + heightDeg * heightDeg) / 2.0;
    
    GaiaQueryParameters params;
    params.center = center;
    params.radiusDegrees = radius;
    params.maxMagnitude = maxMagnitude;
    
    auto allStars = queryRegion(params);
    
    // Filtra per box rettangolare
    std::vector<std::shared_ptr<core::Star>> boxStars;
    double raMin = center.getRightAscension() - widthDeg / 2.0;
    double raMax = center.getRightAscension() + widthDeg / 2.0;
    double decMin = center.getDeclination() - heightDeg / 2.0;
    double decMax = center.getDeclination() + heightDeg / 2.0;
    
    for (const auto& star : allStars) {
        double ra = star->getCoordinates().getRightAscension();
        double dec = star->getCoordinates().getDeclination();
        
        if (ra >= raMin && ra <= raMax && dec >= decMin && dec <= decMax) {
            boxStars.push_back(star);
        }
    }
    
    return boxStars;
}

void GaiaClient::setTapServiceUrl(const std::string& url) {
    pImpl_->tapUrl_ = url;
}

void GaiaClient::setTimeout(int seconds) {
    pImpl_->timeout_ = seconds;
}

} // namespace catalog
} // namespace starmap
