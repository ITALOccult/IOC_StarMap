/**
 * @file leo_quick.cpp
 * @brief Query veloce Leone con catalogo V1 (caricamento ottimizzato)
 */

#include <starmap/catalog/CatalogManager.h>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║   Query Leone (Gaia Mag18 V1)          ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";
    
    std::cout << "⏳ Inizializzazione catalogo...\n";
    std::cout << "   (primo caricamento richiede ~30-60 secondi)\n\n";
    
    starmap::catalog::CatalogManager catalogMgr;
    
    std::cout << "✓ Catalogo pronto\n\n";
    
    // Query piccola e veloce
    starmap::catalog::GaiaQueryParameters params;
    params.center = starmap::core::EquatorialCoordinates(152.0958, 11.9672); // Regolo
    params.radiusDegrees = 2.0;  // Solo 2° per essere veloci
    params.maxMagnitude = 5.0;   // Solo stelle molto luminose
    params.maxResults = 20;
    
    std::cout << "🌟 Query stelle luminose vicino a Regolo\n";
    std::cout << "   Raggio: 2° | Mag limite: 5.0\n\n";
    
    auto stars = catalogMgr.queryStars(params, false);
    
    std::cout << "✓ Trovate " << stars.size() << " stelle\n\n";
    
    if (!stars.empty()) {
        // Ordina per magnitudine
        std::sort(stars.begin(), stars.end(),
            [](const auto& a, const auto& b) {
                return a->getMagnitude() < b->getMagnitude();
            });
        
        std::cout << "⭐ Stelle più luminose:\n";
        std::cout << std::string(65, '-') << "\n";
        std::cout << std::left
                  << std::setw(4) << "#"
                  << std::setw(20) << "GAIA DR3 ID"
                  << std::setw(6) << "Mag"
                  << std::setw(12) << "RA"
                  << "Dec\n";
        std::cout << std::string(65, '-') << "\n";
        
        for (size_t i = 0; i < stars.size(); ++i) {
            const auto& star = stars[i];
            std::cout << std::left
                      << std::setw(4) << (i + 1)
                      << std::setw(20) << star->getGaiaId()
                      << std::fixed << std::setprecision(2)
                      << std::setw(6) << star->getMagnitude()
                      << std::setprecision(3)
                      << std::setw(12) << star->getCoordinates().getRightAscension()
                      << star->getCoordinates().getDeclination()
                      << "\n";
        }
        std::cout << std::string(65, '-') << "\n";
    }
    
    std::cout << "\n✨ Query completata!\n\n";
    return 0;
}
