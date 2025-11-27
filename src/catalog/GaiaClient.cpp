#include "starmap/catalog/GaiaClient.h"
#include <ioc_gaialib/gaia_catalog.h>
#include <ioc_gaialib/gaia_client.h>
#include <ioc_gaialib/types.h>
#include <stdexcept>
#include <cmath>

namespace starmap {
namespace catalog {

class GaiaClient::Impl {
public:
    explicit Impl(bool useMag18, const std::string& mag18Path) 
        : useMag18_(useMag18)
        , onlineClient_(ioc::gaia::GaiaRelease::DR3)
        , timeout_(60)
        , rateLimit_(10) {
        
        onlineClient_.setRateLimit(rateLimit_);
        onlineClient_.setTimeout(timeout_);
        
        // Inizializza catalogo locale se richiesto
        if (useMag18_) {
            try {
                ioc_gaialib::GaiaCatalogConfig config;
                if (!mag18Path.empty()) {
                    config.mag18_catalog_path = mag18Path;
                } else {
                    // Prova percorsi standard
                    const char* home = getenv("HOME");
                    if (home) {
                        config.mag18_catalog_path = std::string(home) + 
                            "/catalogs/gaia_mag18_v2.cat";
                    }
                }
                
                if (!config.mag18_catalog_path.empty()) {
                    unifiedCatalog_ = std::make_unique<ioc_gaialib::GaiaCatalog>(config);
                    localCatalogAvailable_ = true;
                }
            } catch (const std::exception& e) {
                // Fallback a query online
                localCatalogAvailable_ = false;
            }
        }
    }
    
    bool useMag18_;
    bool localCatalogAvailable_ = false;
    int timeout_;
    int rateLimit_;
    
    ioc::gaia::GaiaClient onlineClient_;
    std::unique_ptr<ioc_gaialib::GaiaCatalog> unifiedCatalog_;
};

GaiaClient::GaiaClient(bool useMag18, const std::string& mag18Path)
    : pImpl_(std::make_unique<Impl>(useMag18, mag18Path)) {}

GaiaClient::~GaiaClient() = default;

std::shared_ptr<core::Star> GaiaClient::convertGaiaStar(const void* gaiaStarPtr) const {
    if (!gaiaStarPtr) return nullptr;
    
    const auto& gaiaStar = *static_cast<const ioc::gaia::GaiaStar*>(gaiaStarPtr);
    
    auto star = std::make_shared<core::Star>();
    
    star->setGaiaId(gaiaStar.source_id);
    star->setCoordinates(core::EquatorialCoordinates(gaiaStar.ra, gaiaStar.dec));
    star->setMagnitude(gaiaStar.phot_g_mean_mag);
    
    if (gaiaStar.parallax > 0) {
        star->setParallax(gaiaStar.parallax);
    }
    
    star->setProperMotionRA(gaiaStar.pmra);
    star->setProperMotionDec(gaiaStar.pmdec);
    
    // Color index BP-RP
    double bpRp = gaiaStar.getBpRpColor();
    if (!std::isnan(bpRp)) {
        star->setColorIndex(bpRp);
    }
    
    return star;
}

std::vector<std::shared_ptr<core::Star>> GaiaClient::queryRegion(
    const GaiaQueryParameters& params) {
    
    std::vector<std::shared_ptr<core::Star>> stars;
    
    try {
        // Usa catalogo locale se disponibile e magnitudine compatibile
        if (pImpl_->localCatalogAvailable_ && 
            pImpl_->unifiedCatalog_ && 
            params.maxMagnitude <= 18.0) {
            
            auto gaiaStars = pImpl_->unifiedCatalog_->queryCone(
                params.center.getRightAscension(),
                params.center.getDeclination(),
                params.radiusDegrees,
                params.maxResults
            );
            
            for (const auto& gaiaStar : gaiaStars) {
                if (gaiaStar.phot_g_mean_mag <= params.maxMagnitude) {
                    auto star = convertGaiaStar(&gaiaStar);
                    if (star) stars.push_back(star);
                }
            }
            
        } else {
            // Fallback a query online
            auto gaiaStars = pImpl_->onlineClient_.queryCone(
                params.center.getRightAscension(),
                params.center.getDeclination(),
                params.radiusDegrees,
                params.maxMagnitude
            );
            
            for (const auto& gaiaStar : gaiaStars) {
                auto star = convertGaiaStar(&gaiaStar);
                if (star) stars.push_back(star);
            }
        }
        
        // Limita risultati
        if (stars.size() > static_cast<size_t>(params.maxResults)) {
            stars.resize(params.maxResults);
        }
        
    } catch (const std::exception& e) {
        // Log errore e ritorna vettore vuoto
        stars.clear();
    }
    
    return stars;
}

std::shared_ptr<core::Star> GaiaClient::queryById(long long gaiaId) {
    try {
        // Prova prima catalogo locale
        if (pImpl_->localCatalogAvailable_ && pImpl_->unifiedCatalog_) {
            auto gaiaStarOpt = pImpl_->unifiedCatalog_->queryStar(
                static_cast<uint64_t>(gaiaId)
            );
            
            if (gaiaStarOpt.has_value()) {
                return convertGaiaStar(&gaiaStarOpt.value());
            }
        }
        
        // Fallback a query online
        std::vector<uint64_t> ids = {static_cast<uint64_t>(gaiaId)};
        auto gaiaStars = pImpl_->onlineClient_.queryBySourceIds(ids);
        
        if (!gaiaStars.empty()) {
            return convertGaiaStar(&gaiaStars[0]);
        }
        
    } catch (const std::exception&) {
        // Ritorna nullptr in caso di errore
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<core::Star>> GaiaClient::queryBox(
    const core::EquatorialCoordinates& center,
    double widthDeg,
    double heightDeg,
    double maxMagnitude) {
    
    try {
        // Usa query box di IOC_GaiaLib se disponibile
        if (pImpl_->localCatalogAvailable_ && 
            pImpl_->unifiedCatalog_ && 
            maxMagnitude <= 18.0) {
            
            double raMin = center.getRightAscension() - widthDeg / 2.0;
            double raMax = center.getRightAscension() + widthDeg / 2.0;
            double decMin = center.getDeclination() - heightDeg / 2.0;
            double decMax = center.getDeclination() + heightDeg / 2.0;
            
            // Approssima con cone search
            double radius = std::sqrt(widthDeg * widthDeg + heightDeg * heightDeg) / 2.0;
            auto gaiaStars = pImpl_->unifiedCatalog_->queryCone(
                center.getRightAscension(),
                center.getDeclination(),
                radius,
                10000
            );
            
            std::vector<std::shared_ptr<core::Star>> boxStars;
            for (const auto& gaiaStar : gaiaStars) {
                if (gaiaStar.ra >= raMin && gaiaStar.ra <= raMax &&
                    gaiaStar.dec >= decMin && gaiaStar.dec <= decMax &&
                    gaiaStar.phot_g_mean_mag <= maxMagnitude) {
                    
                    auto star = convertGaiaStar(&gaiaStar);
                    if (star) boxStars.push_back(star);
                }
            }
            
            return boxStars;
            
        } else {
            // Fallback a query online
            auto gaiaStars = pImpl_->onlineClient_.queryBox(
                center.getRightAscension() - widthDeg / 2.0,
                center.getRightAscension() + widthDeg / 2.0,
                center.getDeclination() - heightDeg / 2.0,
                center.getDeclination() + heightDeg / 2.0,
                maxMagnitude
            );
            
            std::vector<std::shared_ptr<core::Star>> stars;
            for (const auto& gaiaStar : gaiaStars) {
                auto star = convertGaiaStar(&gaiaStar);
                if (star) stars.push_back(star);
            }
            
            return stars;
        }
        
    } catch (const std::exception&) {
        return std::vector<std::shared_ptr<core::Star>>();
    }
}

void GaiaClient::setTapServiceUrl(const std::string& url) {
    // IOC_GaiaLib usa URL fisso per GAIA ESA
    // Questa funzione è mantenuta per compatibilità ma non ha effetto
}

void GaiaClient::setTimeout(int seconds) {
    pImpl_->timeout_ = seconds;
    pImpl_->onlineClient_.setTimeout(seconds);
}

void GaiaClient::setRateLimit(int queriesPerMinute) {
    pImpl_->rateLimit_ = queriesPerMinute;
    pImpl_->onlineClient_.setRateLimit(queriesPerMinute);
}

bool GaiaClient::isLocalCatalogAvailable() const {
    return pImpl_->localCatalogAvailable_;
}

GaiaClient::CatalogStats GaiaClient::getStatistics() const {
    CatalogStats stats;
    
    if (pImpl_->unifiedCatalog_) {
        auto catalogStats = pImpl_->unifiedCatalog_->getStatistics();
        stats.totalStars = catalogStats.total_stars;
        stats.magLimit = catalogStats.mag_limit;
        stats.isOnline = false;
        stats.cacheHitsMisses = catalogStats.cache_hits_misses;
    } else {
        stats.isOnline = true;
    }
    
    return stats;
}

} // namespace catalog
} // namespace starmap
