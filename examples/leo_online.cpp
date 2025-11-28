/**
 * @file leo_online.cpp
 * @brief Query rapida online del Leone (senza catalogo locale)
 */

#include <ioc_gaialib/gaia_client.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  Query Online Leone (Gaia TAP - veloce)          ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    // Query online diretta (niente caricamento catalogo)
    ioc::gaia::GaiaClient client(ioc::gaia::GaiaRelease::DR3);
    
    std::cout << "📡 Modalità: Query online Gaia Archive\n";
    std::cout << "⏱️  Tempo stimato: 10-20 secondi\n\n";
    
    std::cout << "🌟 Query Leone (15° intorno a Regolo)\n";
    std::cout << "   Magnitudine limite: 6.0 (stelle visibili)\n";
    std::cout << "   Query in corso...\n\n";
    
    auto stars = client.queryCone(152.0958, 11.9672, 15.0, 6.0);
    
    std::cout << "✓ Trovate " << stars.size() << " stelle\n\n";
    
    if (!stars.empty()) {
        // Ordina per magnitudine
        std::sort(stars.begin(), stars.end(),
            [](const auto& a, const auto& b) {
                return a.phot_g_mean_mag < b.phot_g_mean_mag;
            });
        
        std::cout << "⭐ Le 15 stelle più luminose:\n";
        std::cout << std::string(75, '-') << "\n";
        std::cout << std::left
                  << std::setw(4) << "#"
                  << std::setw(20) << "GAIA DR3 ID"
                  << std::setw(6) << "Mag"
                  << std::setw(12) << "RA"
                  << std::setw(12) << "Dec"
                  << "BP-RP\n";
        std::cout << std::string(75, '-') << "\n";
        
        for (size_t i = 0; i < std::min(size_t(15), stars.size()); ++i) {
            const auto& star = stars[i];
            std::cout << std::left
                      << std::setw(4) << (i + 1)
                      << std::setw(20) << star.source_id
                      << std::fixed << std::setprecision(2)
                      << std::setw(6) << star.phot_g_mean_mag
                      << std::setprecision(4)
                      << std::setw(12) << star.ra
                      << std::setw(12) << star.dec
                      << std::setprecision(3) << star.getBpRpColor()
                      << "\n";
        }
        std::cout << std::string(75, '-') << "\n";
    }
    
    std::cout << "\n✨ Query completata!\n\n";
    return 0;
}
