// Test magnitudes from GaiaClient
#include <iostream>
#include "starmap/catalog/GaiaClient.h"
#include "starmap/core/Coordinates.h"

int main() {
    std::cout << "=== Testing GaiaClient Magnitudes ===\n\n";
    
    starmap::catalog::GaiaClient client;
    
    if (!client.isAvailable()) {
        std::cerr << "ERROR: GaiaClient not available!\n";
        return 1;
    }
    
    std::cout << "GaiaClient initialized successfully\n\n";
    
    // Test region query
    starmap::catalog::GaiaQueryParameters params;
    params.center = starmap::core::EquatorialCoordinates(0.1, 0.1);
    params.radiusDegrees = 1.0;
    params.maxMagnitude = 15.0;
    params.maxResults = 10;
    
    auto stars = client.queryRegion(params);
    
    std::cout << "Found " << stars.size() << " stars\n\n";
    
    for (size_t i = 0; i < stars.size(); ++i) {
        const auto& star = stars[i];
        std::cout << (i+1) << ". Gaia " << star->getGaiaId() 
                  << " | RA=" << star->getCoordinates().getRightAscension()
                  << " Dec=" << star->getCoordinates().getDeclination()
                  << " | Mag=" << star->getMagnitude() << "\n";
    }
    
    return 0;
}
