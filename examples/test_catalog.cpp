/**
 * @file test_catalog.cpp
 * @brief Test diagnostico per verificare stato catalogo locale
 */

#include <ioc_gaialib/gaia_client.h>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "\n╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║      Diagnostica Catalogo IOC_GaiaLib                  ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n\n";
    
    // Inizializza client per query online (Gaia TAP service)
    ioc::gaia::GaiaClient client(ioc::gaia::GaiaRelease::DR3);
    
    std::cout << "📡 Modalità: Query online via TAP/ADQL (Gaia Archive)\n";
    std::cout << "   ⚠️  Limite: 10 query/minuto\n";
    std::cout << "   💡 Per query veloci, scaricare catalogo locale da:\n";
    std::cout << "      https://grappa.difa.unibo.it/gaia/\n";
    std::cout << "\n";
    
    // 3. Test query semplice - Regolo
    std::cout << "🧪 Test Query: Regolo (α Leonis)\n";
    std::cout << "   RA: 152.0958° | Dec: +11.9672°\n";
    std::cout << "   Raggio: 0.1° | Mag limite: 2.0\n";
    std::cout << "   Query in corso (può richiedere 10-30 secondi)...\n\n";
    
    double ra = 152.0958;
    double dec = 11.9672;
    double radius = 0.1;  // Piccolo raggio
    double magLimit = 2.0; // Solo Regolo
    
    try {
        auto results = client.queryCone(ra, dec, radius, magLimit);
        
        std::cout << "✓ Query completata: " << results.size() << " stelle trovate\n";
        
        if (results.empty()) {
            std::cout << "\n⚠️  Nessuna stella trovata!\n";
            std::cout << "   Possibili cause:\n";
            std::cout << "   - Catalogo vuoto o corrotto\n";
            std::cout << "   - File catalogo incompleto\n";
            std::cout << "   - Coordinate fuori range\n\n";
        } else {
            std::cout << "\n🌟 Stelle trovate:\n";
            std::cout << "--------------------------------------------------------------------------------\n";
            std::cout << std::left
                      << std::setw(20) << "GAIA DR3 ID"
                      << std::setw(6) << "Mag"
                      << std::setw(12) << "RA"
                      << std::setw(12) << "Dec"
                      << "\n";
            std::cout << "--------------------------------------------------------------------------------\n";
            
            for (const auto& star : results) {
                std::cout << std::left
                          << std::setw(20) << star.source_id
                          << std::fixed << std::setprecision(2)
                          << std::setw(6) << star.phot_g_mean_mag
                          << std::setprecision(4)
                          << std::setw(12) << star.ra
                          << std::setw(12) << star.dec
                          << "\n";
            }
            
            std::cout << "--------------------------------------------------------------------------------\n";
        }
        
    } catch (const std::exception& e) {
        std::cout << "✗ Errore durante la query: " << e.what() << "\n";
        return 1;
    }
    
    // 4. Test query più ampia
    std::cout << "\n🧪 Test Query Estesa: Leone centrale\n";
    std::cout << "   Centro: RA=165° | Dec=+15°\n";
    std::cout << "   Raggio: 10° | Mag limite: 6.0\n";
    std::cout << "   Query in corso (può richiedere 30-60 secondi)...\n\n";
    
    try {
        auto results = client.queryCone(165.0, 15.0, 10.0, 6.0);
        
        std::cout << "✓ Query completata: " << results.size() << " stelle trovate\n";
        
        if (results.size() > 0) {
            // Conta per range magnitudine
            int mag1_2 = 0, mag2_3 = 0, mag3_4 = 0, mag4_5 = 0, mag5_6 = 0;
            
            for (const auto& star : results) {
                double mag = star.phot_g_mean_mag;
                if (mag < 2.0) mag1_2++;
                else if (mag < 3.0) mag2_3++;
                else if (mag < 4.0) mag3_4++;
                else if (mag < 5.0) mag4_5++;
                else mag5_6++;
            }
            
            std::cout << "\n   Distribuzione per magnitudine:\n";
            std::cout << "   • Mag < 2.0: " << mag1_2 << "\n";
            std::cout << "   • Mag 2-3: " << mag2_3 << "\n";
            std::cout << "   • Mag 3-4: " << mag3_4 << "\n";
            std::cout << "   • Mag 4-5: " << mag4_5 << "\n";
            std::cout << "   • Mag 5-6: " << mag5_6 << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cout << "✗ Errore durante la query: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "\n✨ Diagnostica completata!\n\n";
    
    return 0;
}
