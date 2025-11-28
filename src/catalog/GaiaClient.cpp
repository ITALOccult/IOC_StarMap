#include "starmap/catalog/GaiaClient.h"
#include <ioc_gaialib/gaia_mag18_catalog_v2.h>
#include <ioc_gaialib/gaia_local_catalog.h>
#include <ioc_gaialib/gaia_client.h>
#include <ioc_gaialib/types.h>
#include <stdexcept>
#include <cmath>
#include <fstream>
#include <filesystem>

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
        
        // Inizializza catalogo offline se disponibile
        initializeLocalCatalogs(mag18Path);
    }
    
    void initializeLocalCatalogs(const std::string& customPath) {
        try {
            const char* home = getenv("HOME");
            if (!home) return;
            
            std::string homePath(home);
            std::string grappaPath = homePath + "/catalogs/GRAPPA3E";
            std::string v2Path = homePath + "/catalogs/gaia_mag18_v2.mag18v2";
            std::string v1Path = homePath + "/catalogs/gaia_mag18.cat.gz";
            
            // Priorità: percorso custom > GRAPPA3E full > Mag18 V2 > Mag18 V1
            
            // 1. Prova percorso custom se fornito
            if (!customPath.empty() && std::filesystem::exists(customPath)) {
                if (initializeMag18V2(customPath)) {
                    localCatalogAvailable_ = true;
                    return;
                }
            }
            
            // 2. Prova GRAPPA3E (offline completo con 1.8B stelle)
            if (std::filesystem::exists(grappaPath)) {
                try {
                    gaiaLocalCatalog_ = std::make_unique<ioc_gaialib::GaiaLocalCatalog>(
                        grappaPath
                    );
                    if (gaiaLocalCatalog_->isLoaded()) {
                        localCatalogType_ = LocalCatalogType::GRAPPA3E;
                        localCatalogAvailable_ = true;
                        return;
                    }
                } catch (const std::exception&) {
                    gaiaLocalCatalog_.reset();
                }
            }
            
            // 3. Prova Mag18 V2 (offline con ~100M stelle, mag ≤ 18)
            if (std::filesystem::exists(v2Path)) {
                if (initializeMag18V2(v2Path)) {
                    localCatalogAvailable_ = true;
                    return;
                }
            }
            
            // 4. Fallback a Mag18 V1 (offline legacy, mag ≤ 18)
            if (std::filesystem::exists(v1Path)) {
                if (initializeMag18V2(v1Path)) {
                    localCatalogAvailable_ = true;
                    return;
                }
            }
            
        } catch (const std::exception&) {
            // Errore durante inizializzazione cataloghi locali
            localCatalogAvailable_ = false;
        }
    }
    
    bool initializeMag18V2(const std::string& path) {
        try {
            mag18Catalog_ = std::make_unique<ioc::gaia::Mag18CatalogV2>();
            if (mag18Catalog_->open(path)) {
                // Abilita parallelizzazione per query veloci
                mag18Catalog_->setParallelProcessing(true, 0);  // 0 = auto-detect threads
                localCatalogType_ = LocalCatalogType::MAG18V2;
                return true;
            }
        } catch (const std::exception&) {
            mag18Catalog_.reset();
        }
        return false;
    }
    
    enum class LocalCatalogType {
        NONE,      ///< No local catalog
        GRAPPA3E,  ///< Full GRAPPA3E (~1.8B stars)
        MAG18V2    ///< Mag18 V2 (~100M stars, mag ≤ 18)
    };
    
    bool useMag18_;
    bool localCatalogAvailable_ = false;
    LocalCatalogType localCatalogType_ = LocalCatalogType::NONE;
    int timeout_;
    int rateLimit_;
    
    ioc::gaia::GaiaClient onlineClient_;
    std::unique_ptr<ioc::gaia::Mag18CatalogV2> mag18Catalog_;
    std::unique_ptr<ioc_gaialib::GaiaLocalCatalog> gaiaLocalCatalog_;
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
        // Usa catalogo locale se disponibile
        if (pImpl_->localCatalogAvailable_) {
            
            // GRAPPA3E: full offline catalog (1.8B stars)
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::GRAPPA3E &&
                pImpl_->gaiaLocalCatalog_) {
                
                auto gaiaStars = pImpl_->gaiaLocalCatalog_->queryConeWithMagnitude(
                    params.center.getRightAscension(),
                    params.center.getDeclination(),
                    params.radiusDegrees,
                    -10.0,  // mag_min: very bright
                    params.maxMagnitude,
                    params.maxResults > 0 ? params.maxResults : 0
                );
                
                for (const auto& gaiaStar : gaiaStars) {
                    auto star = convertGaiaStar(&gaiaStar);
                    if (star) stars.push_back(star);
                }
                
                return stars;
            }
            
            // Mag18 V2/V1: optimized queries with new API
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::MAG18V2 &&
                pImpl_->mag18Catalog_ && 
                params.maxMagnitude <= 18.0) {
                
                // Usa nuovo metodo ottimizzato con filtro magnitudine
                auto gaiaStars = pImpl_->mag18Catalog_->queryConeWithMagnitude(
                    params.center.getRightAscension(),
                    params.center.getDeclination(),
                    params.radiusDegrees,
                    -10.0,  // mag_min
                    params.maxMagnitude,
                    params.maxResults > 0 ? params.maxResults : 0
                );
                
                for (const auto& gaiaStar : gaiaStars) {
                    auto star = convertGaiaStar(&gaiaStar);
                    if (star) stars.push_back(star);
                }
                
                return stars;
            }
        }
        
        // Fallback: query online via GaiaClient (TAP ADQL)
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
        
        // Limita risultati
        if (stars.size() > static_cast<size_t>(params.maxResults)) {
            stars.resize(params.maxResults);
        }
        
    } catch (const std::exception& e) {
        // Log errore ma ritorna risultati parziali se disponibili
        if (stars.empty()) {
            stars.clear();
        }
    }
    
    return stars;
}

std::shared_ptr<core::Star> GaiaClient::queryById(long long gaiaId) {
    try {
        uint64_t sourceId = static_cast<uint64_t>(gaiaId);
        
        // Prova prima catalogo locale
        if (pImpl_->localCatalogAvailable_) {
            
            // GRAPPA3E full catalog
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::GRAPPA3E &&
                pImpl_->gaiaLocalCatalog_) {
                
                auto gaiaStarOpt = pImpl_->gaiaLocalCatalog_->queryBySourceId(sourceId);
                if (gaiaStarOpt.has_value()) {
                    return convertGaiaStar(&gaiaStarOpt.value());
                }
            }
            
            // Mag18 V2/V1
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::MAG18V2 &&
                pImpl_->mag18Catalog_) {
                
                auto gaiaStarOpt = pImpl_->mag18Catalog_->queryBySourceId(sourceId);
                if (gaiaStarOpt.has_value()) {
                    return convertGaiaStar(&gaiaStarOpt.value());
                }
            }
        }
        
        // Fallback a query online
        std::vector<int64_t> ids = {static_cast<int64_t>(sourceId)};
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
        // Usa catalogo locale se disponibile
        if (pImpl_->localCatalogAvailable_) {
            
            // GRAPPA3E: full offline catalog
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::GRAPPA3E &&
                pImpl_->gaiaLocalCatalog_) {
                
                // Usa queryBox diretto (nuova API)
                auto gaiaStars = pImpl_->gaiaLocalCatalog_->queryBox(
                    center.getRightAscension() - widthDeg / 2.0,
                    center.getRightAscension() + widthDeg / 2.0,
                    center.getDeclination() - heightDeg / 2.0,
                    center.getDeclination() + heightDeg / 2.0,
                    0  // max_results = 0 (no limit)
                );
                
                std::vector<std::shared_ptr<core::Star>> boxStars;
                for (const auto& gaiaStar : gaiaStars) {
                    if (gaiaStar.phot_g_mean_mag <= maxMagnitude) {
                        auto star = convertGaiaStar(&gaiaStar);
                        if (star) boxStars.push_back(star);
                    }
                }
                return boxStars;
            }
            
            // Mag18 V2/V1: simula queryBox con cone search
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::MAG18V2 &&
                pImpl_->mag18Catalog_ && 
                maxMagnitude <= 18.0) {
                
                // Calcola raggio approssimativo del box
                double radius = std::sqrt(widthDeg * widthDeg + heightDeg * heightDeg) / 2.0;
                
                auto gaiaStars = pImpl_->mag18Catalog_->queryConeWithMagnitude(
                    center.getRightAscension(),
                    center.getDeclination(),
                    radius,
                    -10.0,  // mag_min
                    maxMagnitude,
                    0  // max_results
                );
                
                double raMin = center.getRightAscension() - widthDeg / 2.0;
                double raMax = center.getRightAscension() + widthDeg / 2.0;
                double decMin = center.getDeclination() - heightDeg / 2.0;
                double decMax = center.getDeclination() + heightDeg / 2.0;
                
                std::vector<std::shared_ptr<core::Star>> boxStars;
                for (const auto& gaiaStar : gaiaStars) {
                    // Filtro box preciso
                    if (gaiaStar.ra >= raMin && gaiaStar.ra <= raMax &&
                        gaiaStar.dec >= decMin && gaiaStar.dec <= decMax) {
                        
                        auto star = convertGaiaStar(&gaiaStar);
                        if (star) boxStars.push_back(star);
                    }
                }
                return boxStars;
            }
        }
        
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
    stats.isOnline = true;  // default
    
    try {
        if (pImpl_->localCatalogAvailable_) {
            
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::GRAPPA3E &&
                pImpl_->gaiaLocalCatalog_) {
                
                auto localStats = pImpl_->gaiaLocalCatalog_->getStatistics();
                stats.totalStars = localStats.total_sources;
                stats.magLimit = 20.0;  // GRAPPA3E è più profondo di Mag18
                stats.isOnline = false;
                return stats;
            }
            
            if (pImpl_->localCatalogType_ == Impl::LocalCatalogType::MAG18V2 &&
                pImpl_->mag18Catalog_) {
                
                stats.totalStars = pImpl_->mag18Catalog_->getTotalStars();
                stats.magLimit = pImpl_->mag18Catalog_->getMagLimit();
                stats.isOnline = false;
                return stats;
            }
        }
    } catch (const std::exception&) {
        // Fallback a online
    }
    
    return stats;
}

} // namespace catalog
} // namespace starmap
