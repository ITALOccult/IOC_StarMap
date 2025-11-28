/**
 * @file test_mag18_direct.cpp
 * @brief Test diretto del catalogo Mag18 con IOC_GaiaLib
 */

#include <ioc_gaialib/gaia_mag18_catalog.h>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║     Test Diretto Catalogo Mag18 V1                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";
    
    const char* home = getenv("HOME");
    if (!home) {
        std::cerr << "❌ Impossibile determinare HOME directory\n";
        return 1;
    }
    
    std::string catalogPath = std::string(home) + "/catalogs/gaia_mag18.cat.gz";
    
    std::cout << "📁 Percorso catalogo: " << catalogPath << "\n";
    std::cout << "🔄 Caricamento in corso...\n\n";
    
    try {
        // Carica catalogo Mag18 V1
        ioc_gaialib::GaiaMag18Catalog catalog(catalogPath);
        
        auto stats = catalog.getStatistics();
        
        std::cout << "✅ Catalogo caricato con successo!\n\n";
        std::cout << "📊 Statistiche:\n";
        std::cout << "   • Stelle totali: " << stats.total_stars << "\n";
        std::cout << "   • Limite magnitudine: " << std::fixed << std::setprecision(1) 
                  << stats.mag_limit << "\n";
        std::cout << "   • Dimensione file: " << std::fixed << std::setprecision(2)
                  << (stats.file_size / (1024.0 * 1024.0 * 1024.0)) << " GB\n";
        std::cout << "   • Rapporto compressione: " << std::setprecision(1)
                  << stats.compression_ratio << "x\n";
        std::cout << "\n";
        
        // Test query Regolo
        std::cout << "🧪 Test Query: Regolo (α Leonis)\n";
        std::cout << "   RA: 152.0958° | Dec: +11.9672°\n";
        std::cout << "   Raggio: 15° | Mag limite: 8.0\n\n";
        
        auto stars = catalog.queryCone(152.0958, 11.9672, 15.0);
        
        // Filtra per magnitudine
        std::vector<ioc::gaia::GaiaStar> brightStars;
        for (const auto& star : stars) {
            if (star.phot_g_mean_mag <= 8.0) {
                brightStars.push_back(star);
            }
        }
        
        std::cout << "✓ Trovate " << brightStars.size() << " stelle (mag ≤ 8.0)\n";
        std::cout << "   Totale nel raggio: " << stars.size() << " stelle\n\n";
        
        if (!brightStars.empty()) {
            // Ordina per magnitudine
            std::sort(brightStars.begin(), brightStars.end(),
                [](const auto& a, const auto& b) {
                    return a.phot_g_mean_mag < b.phot_g_mean_mag;
                });
            
            std::cout << "⭐ Prime 10 stelle più luminose:\n";
            std::cout << std::string(70, '-') << "\n";
            std::cout << std::left
                      << std::setw(4) << "#"
                      << std::setw(20) << "GAIA DR3 ID"
                      << std::setw(6) << "Mag"
                      << std::setw(12) << "RA"
                      << std::setw(12) << "Dec"
                      << "\n";
            std::cout << std::string(70, '-') << "\n";
            
            for (size_t i = 0; i < std::min(size_t(10), brightStars.size()); ++i) {
                const auto& star = brightStars[i];
                std::cout << std::left
                          << std::setw(4) << (i + 1)
                          << std::setw(20) << star.source_id
                          << std::fixed << std::setprecision(2)
                          << std::setw(6) << star.phot_g_mean_mag
                          << std::setprecision(4)
                          << std::setw(12) << star.ra
                          << std::setw(12) << star.dec
                          << "\n";
            }
            std::cout << std::string(70, '-') << "\n";
        }
        
        std::cout << "\n✨ Test completato!\n\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Errore: " << e.what() << "\n\n";
        return 1;
    }
}
