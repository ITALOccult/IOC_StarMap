#include <starmap/StarMap.h>
#include <iostream>

/**
 * Esempio di query diretta al catalogo GAIA
 * senza generare una mappa completa
 */

int main() {
    std::cout << "StarMap Library - GAIA Query Example\n\n";

    try {
        // Crea client GAIA
        starmap::catalog::GaiaClient gaiaClient;
        gaiaClient.setTimeout(60);
        
        // Query per regione del cielo
        starmap::catalog::GaiaQueryParameters params;
        
        // Centro su Pleiadi (M45)
        params.center = starmap::core::EquatorialCoordinates(56.75, 24.12);
        params.radiusDegrees = 2.0;  // 2° di raggio
        params.maxMagnitude = 10.0;
        params.maxResults = 5000;
        params.includeProperMotion = true;
        params.includeParallax = true;
        
        std::cout << "Query GAIA DR3:\n";
        std::cout << "  Centro: RA=" << params.center.getRightAscension() 
                  << "°, Dec=" << params.center.getDeclination() << "°\n";
        std::cout << "  Raggio: " << params.radiusDegrees << "°\n";
        std::cout << "  Magnitudine max: " << params.maxMagnitude << "\n";
        std::cout << "  Max risultati: " << params.maxResults << "\n\n";
        
        std::cout << "Interrogazione catalogo GAIA...\n";
        
        auto stars = gaiaClient.queryRegion(params);
        
        std::cout << "✓ Trovate " << stars.size() << " stelle\n\n";
        
        // Mostra prime 10 stelle
        std::cout << "Prime 10 stelle più luminose:\n";
        std::cout << "----------------------------------------\n";
        
        // Ordina per magnitudine
        std::sort(stars.begin(), stars.end(), 
            [](const auto& a, const auto& b) {
                return a->getMagnitude() < b->getMagnitude();
            });
        
        for (size_t i = 0; i < std::min(size_t(10), stars.size()); ++i) {
            const auto& star = stars[i];
            
            std::cout << (i + 1) << ". GAIA DR3 " << star->getGaiaId() << "\n";
            std::cout << "   RA/Dec: " << star->getCoordinates().toHMSString() 
                      << " / " << star->getCoordinates().toDMSString() << "\n";
            std::cout << "   Mag: " << std::fixed << std::setprecision(2) 
                      << star->getMagnitude();
            
            if (star->getParallax().has_value()) {
                std::cout << ", Parallax: " << star->getParallax().value() << " mas";
                
                auto dist = star->getDistance();
                if (dist.has_value()) {
                    std::cout << " (~" << std::fixed << std::setprecision(1) 
                              << dist.value() << " pc)";
                }
            }
            
            if (star->getProperMotionRA().has_value()) {
                std::cout << ", PM: (" << std::fixed << std::setprecision(2)
                          << star->getProperMotionRA().value() << ", "
                          << star->getProperMotionDec().value() << ") mas/yr";
            }
            
            std::cout << "\n\n";
        }
        
        // Arricchisci con numeri SAO
        std::cout << "Arricchimento con numeri SAO...\n";
        
        starmap::catalog::SAOCatalog saoCatalog;
        int saoCount = 0;
        
        for (auto& star : stars) {
            if (star->getMagnitude() < 8.0) { // Solo stelle luminose
                if (saoCatalog.enrichWithSAO(star)) {
                    saoCount++;
                }
            }
        }
        
        std::cout << "✓ " << saoCount << " stelle arricchite con numeri SAO\n\n";
        
        // Mostra stelle con SAO
        std::cout << "Stelle con numero SAO:\n";
        std::cout << "----------------------------------------\n";
        
        int displayCount = 0;
        for (const auto& star : stars) {
            if (star->getSAONumber().has_value()) {
                std::cout << "SAO " << star->getSAONumber().value() 
                          << " (GAIA " << star->getGaiaId() << ")\n";
                std::cout << "  Mag: " << std::fixed << std::setprecision(2) 
                          << star->getMagnitude() << "\n\n";
                
                displayCount++;
                if (displayCount >= 5) break;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
