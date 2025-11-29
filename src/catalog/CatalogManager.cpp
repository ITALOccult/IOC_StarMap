#include "starmap/catalog/CatalogManager.h"
#include <algorithm>

namespace starmap {
namespace catalog {

CatalogManager::CatalogManager() 
    : cacheEnabled_(true)
    , parallelEnrichment_(false) {
}

CatalogManager::~CatalogManager() = default;

std::vector<std::shared_ptr<core::Star>> CatalogManager::queryStars(
    const GaiaQueryParameters& params,
    bool enrichWithSAO) {
    
    auto stars = gaiaClient_.queryRegion(params);
    
    if (!enrichWithSAO || stars.empty()) {
        return stars;
    }
    
    for (auto& star : stars) {
        if (star->getMagnitude() < 9.0) {
            saoCatalog_.enrichWithSAO(star);
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
    
    // Usa query circolare con raggio che contiene il rettangolo
    double radius = std::sqrt(widthDeg * widthDeg + heightDeg * heightDeg) / 2.0;
    
    GaiaQueryParameters params;
    params.center = center;
    params.radiusDegrees = radius;
    params.maxMagnitude = maxMagnitude;
    
    return queryStars(params, enrichWithSAO);
}

void CatalogManager::setCacheEnabled(bool enabled) {
    cacheEnabled_ = enabled;
}

void CatalogManager::setParallelEnrichment(bool enabled) {
    parallelEnrichment_ = enabled;
}

} // namespace catalog
} // namespace starmap
