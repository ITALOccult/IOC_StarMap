#include "starmap/catalog/GaiaClient.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== Test GaiaClient con catalogo multifile ===" << std::endl;
    
    starmap::catalog::GaiaClient client;
    
    if (!client.isAvailable()) {
        std::cerr << "Catalogo non disponibile!" << std::endl;
        return 1;
    }
    
    std::cout << "Catalogo disponibile!" << std::endl;
    
    // Query regione Leone
    starmap::catalog::GaiaQueryParameters params;
    params.center = starmap::core::EquatorialCoordinates(168.5, 12.0);
    params.radiusDegrees = 10.0;
    params.maxMagnitude = 6.0;
    
    std::cout << "\nQuery regione Leone (RA=168.5, Dec=12, r=10°, mag<6)..." << std::endl;
    
    auto stars = client.queryRegion(params);
    
    std::cout << "Trovate " << stars.size() << " stelle" << std::endl;
    
    // Mostra le prime 10
    std::cout << "\nPrime 10 stelle:" << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    
    int count = 0;
    for (const auto& star : stars) {
        if (count++ >= 10) break;
        std::cout << "  Gaia " << star->getGaiaId() 
                  << " RA=" << star->getCoordinates().getRightAscension()
                  << " Dec=" << star->getCoordinates().getDeclination()
                  << " Mag=" << star->getMagnitude() 
                  << std::endl;
    }
    
    std::cout << "\n=== Test completato ===" << std::endl;
    return 0;
}
