/**
 * @file test_sao_database.cpp
 * @brief Test del database locale Gaia-SAO cross-match
 */

#include <starmap/StarMap.h>
#include <iostream>
#include <iomanip>

using namespace starmap;

int main(int argc, char* argv[]) {
    std::cout << "=== Test Database Gaia-SAO Cross-Match ===\n\n";
    
    // 1. Test diretto del database
    std::cout << "1. Test accesso diretto al database\n";
    std::cout << "------------------------------------\n";
    
    catalog::GaiaSAODatabase db("gaia_sao_xmatch.db");
    
    if (db.isAvailable()) {
        std::cout << "✓ Database caricato con successo!\n\n";
        std::cout << db.getStatistics() << "\n";
    } else {
        std::cout << "✗ Database non disponibile\n";
        std::cout << "Genera il database con:\n";
        std::cout << "  python scripts/build_gaia_sao_database.py\n\n";
        return 1;
    }
    
    // 2. Test lookup per Gaia ID
    std::cout << "\n2. Test lookup per Gaia ID\n";
    std::cout << "------------------------------------\n";
    
    // Sirio: Gaia DR3 2947050466531873024
    long long sirius_gaia_id = 2947050466531873024LL;
    auto sirius_sao = db.findSAOByGaiaId(sirius_gaia_id);
    
    if (sirius_sao) {
        std::cout << "Sirius: Gaia " << sirius_gaia_id 
                  << " -> SAO " << *sirius_sao << "\n";
    } else {
        std::cout << "Sirius non trovato nel database\n";
    }
    
    // Vega: Gaia DR3 2115669670928909440
    long long vega_gaia_id = 2115669670928909440LL;
    auto vega_sao = db.findSAOByGaiaId(vega_gaia_id);
    
    if (vega_sao) {
        std::cout << "Vega: Gaia " << vega_gaia_id 
                  << " -> SAO " << *vega_sao << "\n";
    }
    
    // 3. Test cone search
    std::cout << "\n3. Test cone search (Regione Orione)\n";
    std::cout << "------------------------------------\n";
    
    // Betelgeuse area
    core::EquatorialCoordinates betelgeuse_coords(88.8, 7.4);
    auto entries = db.coneSearch(betelgeuse_coords, 2.0); // 2 gradi
    
    std::cout << "Trovate " << entries.size() << " stelle nel cono di 2°\n";
    std::cout << "\nPrime 10 stelle:\n";
    std::cout << std::setw(15) << "Gaia ID" 
              << std::setw(10) << "SAO" 
              << std::setw(10) << "RA"
              << std::setw(10) << "Dec"
              << std::setw(10) << "Mag" << "\n";
    std::cout << std::string(55, '-') << "\n";
    
    for (size_t i = 0; i < std::min(size_t(10), entries.size()); ++i) {
        const auto& entry = entries[i];
        std::cout << std::setw(15) << entry.gaiaSourceId
                  << std::setw(10) << entry.saoNumber
                  << std::setw(10) << std::fixed << std::setprecision(2) << entry.ra
                  << std::setw(10) << std::fixed << std::setprecision(2) << entry.dec
                  << std::setw(10) << std::fixed << std::setprecision(2) << entry.magnitude
                  << "\n";
    }
    
    // 4. Test con CatalogManager
    std::cout << "\n4. Test integrazione con CatalogManager\n";
    std::cout << "------------------------------------\n";
    
    catalog::CatalogManager manager;
    
    if (manager.getSAOCatalog().hasLocalDatabase()) {
        std::cout << "✓ CatalogManager usa database locale\n\n";
    } else {
        std::cout << "⚠ CatalogManager userà query online\n\n";
    }
    
    // Query stelle in una regione
    catalog::GaiaQueryParameters params;
    params.center = betelgeuse_coords;
    params.radiusDegrees = 1.0;
    params.maxMagnitude = 7.0;
    
    std::cout << "Query stelle in regione Orione (mag < 7.0)...\n";
    auto stars = manager.queryStars(params, true); // enrichWithSAO = true
    
    std::cout << "Trovate " << stars.size() << " stelle\n";
    
    // Conta stelle con SAO
    int with_sao = 0;
    for (const auto& star : stars) {
        if (star->getSAONumber()) {
            with_sao++;
        }
    }
    
    std::cout << "Stelle con numero SAO: " << with_sao << "/" << stars.size()
              << " (" << std::fixed << std::setprecision(1) 
              << (100.0 * with_sao / stars.size()) << "%)\n";
    
    // Mostra alcune stelle
    std::cout << "\nEsempi di stelle con SAO:\n";
    int count = 0;
    for (const auto& star : stars) {
        if (star->getSAONumber() && count < 5) {
            std::cout << "  ";
            if (!star->getName().empty()) {
                std::cout << std::setw(15) << star->getName();
            } else {
                std::cout << std::setw(15) << ("Gaia " + std::to_string(star->getGaiaId()).substr(0, 10) + "...");
            }
            std::cout << " -> SAO " << *star->getSAONumber()
                      << " (mag " << std::fixed << std::setprecision(2) 
                      << star->getMagnitude() << ")\n";
            count++;
        }
    }
    
    // 5. Performance test
    std::cout << "\n5. Performance test\n";
    std::cout << "------------------------------------\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    const int NUM_QUERIES = 1000;
    int found = 0;
    
    for (int i = 0; i < NUM_QUERIES; ++i) {
        // Test con ID casuali (in pratica molti non esistono, ma testiamo la velocità)
        long long test_id = 1000000000000000000LL + (i * 1000000000);
        if (db.findSAOByGaiaId(test_id)) {
            found++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Eseguite " << NUM_QUERIES << " query in " 
              << (duration.count() / 1000.0) << " ms\n";
    std::cout << "Tempo medio per query: " 
              << std::fixed << std::setprecision(3)
              << (duration.count() / (double)NUM_QUERIES / 1000.0) << " ms\n";
    std::cout << "Query al secondo: " 
              << std::fixed << std::setprecision(0)
              << (NUM_QUERIES * 1000000.0 / duration.count()) << "\n";
    
    std::cout << "\n=== Test completato con successo! ===\n";
    
    return 0;
}
