#include "starmap/catalog/CatalogManager.h"
#include <algorithm>
#include <thread>
#include <future>

namespace starmap {
namespace catalog {

CatalogManager::CatalogManager() 
    : cacheEnabled_(true), parallelEnrichment_(false) {
}

CatalogManager::~CatalogManager() = default;

std::vector<std::shared_ptr<core::Star>> CatalogManager::queryStars(
    const GaiaQueryParameters& params,
    bool enrichWithSAO) {
    
    // Query GAIA
    auto stars = gaiaClient_.queryRegion(params);
    
    if (!enrichWithSAO || stars.empty()) {
        return stars;
    }
    
    // Arricchisci con numeri SAO
    if (parallelEnrichment_ && stars.size() > 10) {
        // Arricchimento parallelo per grandi dataset
        std::vector<std::future<void>> futures;
        
        for (auto& star : stars) {
            futures.push_back(std::async(std::launch::async, [this, star]() {
                // Solo per stelle luminose (SAO tipicamente ha mag < 9)
                if (star->getMagnitude() < 9.0) {
                    saoCatalog_.enrichWithSAO(star);
                }
            }));
        }
        
        // Attendi completamento
        for (auto& future : futures) {
            future.get();
        }
    } else {
        // Arricchimento sequenziale
        for (auto& star : stars) {
            if (star->getMagnitude() < 9.0) {
                saoCatalog_.enrichWithSAO(star);
            }
        }
    }
    
    return stars;
}

std::vector<std::shared_ptr<core::Star>> CatalogManager::queryRectangularRegion(
    const core::EquatorialCoordinates& center,
    double widthDeg,
    double heightDeg,
    double maxMagnitude,
    bool enrichWithSAO) {
    
    auto stars = gaiaClient_.queryBox(center, widthDeg, heightDeg, maxMagnitude);
    
    if (enrichWithSAO && !stars.empty()) {
        for (auto& star : stars) {
            if (star->getMagnitude() < 9.0) {
                saoCatalog_.enrichWithSAO(star);
            }
        }
    }
    
    return stars;
}

void CatalogManager::setCacheEnabled(bool enabled) {
    cacheEnabled_ = enabled;
}

void CatalogManager::setParallelEnrichment(bool enabled) {
    parallelEnrichment_ = enabled;
}

} // namespace catalog
} // namespace starmap
