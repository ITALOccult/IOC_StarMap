/**
 * @file leo_simple.cpp
 * @brief Esempio semplificato: Query stelle della costellazione del Leone
 * 
 * Dimostra l'uso di IOC_GaiaLib tramite StarMap per interrogare
 * le stelle della costellazione del Leone.
 */

#include <starmap/catalog/CatalogManager.h>
#include <starmap/core/Coordinates.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace starmap;

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║    Query Costellazione del Leone con IOC_GaiaLib           ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << std::endl;
    
    // ==================================================================
    // 1. CONFIGURAZIONE QUERY
    // ==================================================================
    
    // Centro su Regolo (α Leonis) - Stella più brillante del Leone
    // RA: 10h 08m 22s = 152.0958° | Dec: +11° 58' 02" = +11.9672°
    core::EquatorialCoordinates regulus(152.0958, 11.9672);
    
    std::cout << "📐 Centro: Regolo (α Leonis)\n";
    std::cout << "   RA: " << regulus.toHMSString() << "\n";
    std::cout << "   Dec: " << regulus.toDMSString() << "\n";
    std::cout << std::endl;
    
    // ==================================================================
    // 2. INIZIALIZZA CATALOG MANAGER (SOLO DATI LOCALI)
    // ==================================================================
    
    std::cout << "🔧 Inizializzazione CatalogManager...\n";
    
    // Configura per usare SOLO catalogo locale
    catalog::GaiaClient gaiaClient(true, "");  // useMag18=true
    catalog::CatalogManager catalogMgr;
    
    // Verifica disponibilità catalogo locale
    if (!gaiaClient.isLocalCatalogAvailable()) {
        std::cout << "   ✗ ERRORE: Catalogo locale non trovato!\n";
        std::cout << "\n";
        std::cout << "   Per usare StarMap con dati locali, scarica il catalogo:\n";
        std::cout << "\n";
        std::cout << "   wget https://www.ioccult.it/ioc_gaialib/downloads/gaia_mag18_v2.cat \\\n";
        std::cout << "     -O ~/catalogs/gaia_mag18_v2.cat\n";
        std::cout << "\n";
        std::cout << "   Dimensione: ~14 GB\n";
        std::cout << "   Leggi: ~/catalogs/README.md per maggiori informazioni\n";
        std::cout << "\n";
        return 1;
    }
    
    std::cout << "   ✓ Catalogo locale Mag18 V2 disponibile\n";
    
    auto stats = catalogMgr.getGaiaClient().getStatistics();
    if (!stats.isOnline) {
        std::cout << "   📊 Limite magnitudine catalogo: " << stats.magLimit << "\n";
    }
    std::cout << std::endl;
    
    // ==================================================================
    // 3. QUERY STELLE DEL LEONE
    // ==================================================================
    
    std::cout << "🌟 Query GAIA DR3 per Leone...\n";
    std::cout << "   Campo: 30° × 30° centrato su Regolo\n";
    std::cout << "   Magnitudine limite: 8.0 (stelle visibili ad occhio nudo)\n";
    std::cout << std::endl;
    
    catalog::GaiaQueryParameters queryParams;
    queryParams.center = regulus;
    queryParams.radiusDegrees = 15.0; // Raggio per coprire 30° × 30°
    queryParams.maxMagnitude = 8.0;    // Solo stelle luminose
    queryParams.maxResults = 500;
    queryParams.includeProperMotion = true;
    queryParams.includeParallax = true;
    
    auto stars = catalogMgr.queryStars(queryParams, true);
    
    std::cout << "✓ Trovate " << stars.size() << " stelle\n";
    std::cout << std::endl;
    
    // ==================================================================
    // 4. ORDINA PER MAGNITUDINE
    // ==================================================================
    
    std::sort(stars.begin(), stars.end(), 
        [](const auto& a, const auto& b) {
            return a->getMagnitude() < b->getMagnitude();
        });
    
    // ==================================================================
    // 5. MOSTRA STELLE PIÙ LUMINOSE
    // ==================================================================
    
    std::cout << "⭐ Le 15 stelle più luminose del Leone:\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::left 
              << std::setw(4) << "#"
              << std::setw(20) << "GAIA DR3 ID"
              << std::setw(6) << "Mag"
              << std::setw(14) << "RA"
              << std::setw(14) << "Dec"
              << std::setw(8) << "SAO"
              << "B-V\n";
    std::cout << std::string(80, '-') << "\n";
    
    for (size_t i = 0; i < std::min(size_t(15), stars.size()); ++i) {
        const auto& star = stars[i];
        
        std::cout << std::left
                  << std::setw(4) << (i + 1)
                  << std::setw(20) << star->getGaiaId()
                  << std::fixed << std::setprecision(2)
                  << std::setw(6) << star->getMagnitude()
                  << std::setprecision(3)
                  << std::setw(14) << star->getCoordinates().getRightAscension()
                  << std::setw(14) << star->getCoordinates().getDeclination();
        
        // Numero SAO se disponibile
        auto saoNum = star->getSAONumber();
        if (saoNum.has_value() && saoNum.value() > 0) {
            std::cout << std::setw(8) << saoNum.value();
        } else {
            std::cout << std::setw(8) << "-";
        }
        
        // Indice colore se disponibile
        auto colorIdx = star->getColorIndex();
        if (colorIdx.has_value() && colorIdx.value() > -10.0) {
            std::cout << std::setprecision(2) << colorIdx.value();
        } else {
            std::cout << "-";
        }
        
        std::cout << "\n";
    }
    
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::endl;
    
    // ==================================================================
    // 6. IDENTIFICA STELLE PRINCIPALI
    // ==================================================================
    
    std::cout << "🌟 Stelle Principali del Leone:\n";
    std::cout << std::endl;
    
    struct NamedStar {
        std::string name;
        double ra;
        double dec;
        double mag;
    };
    
    std::vector<NamedStar> brightStars = {
        {"Regolo (α Leo)", 152.0958, 11.9672, 1.35},    
        {"Denebola (β Leo)", 177.2647, 14.5721, 2.14},  
        {"Algieba (γ Leo)", 154.9926, 19.8415, 2.01},   
        {"Zosma (δ Leo)", 168.5269, 20.5236, 2.56},     
        {"Chort (θ Leo)", 168.5605, 15.4294, 3.34}
    };
    
    for (const auto& named : brightStars) {
        // Cerca la stella nel dataset
        std::shared_ptr<core::Star> found = nullptr;
        double minDist = 0.5; // 0.5 gradi di tolleranza
        
        core::EquatorialCoordinates targetCoord(named.ra, named.dec);
        
        for (const auto& star : stars) {
            double dist = targetCoord.angularDistance(star->getCoordinates());
            
            if (dist < minDist && std::abs(star->getMagnitude() - named.mag) < 0.5) {
                minDist = dist;
                found = star;
            }
        }
        
        std::cout << "  • " << std::setw(20) << std::left << named.name;
        
        if (found) {
            std::cout << " ✓ Trovata: GAIA DR3 " << found->getGaiaId()
                      << " | Mag: " << std::fixed << std::setprecision(2) 
                      << found->getMagnitude();
            
            auto saoNum = found->getSAONumber();
            if (saoNum.has_value() && saoNum.value() > 0) {
                std::cout << " | SAO " << saoNum.value();
            }
        } else {
            std::cout << " ✗ Non trovata (potrebbe essere mag > " 
                      << queryParams.maxMagnitude << ")";
        }
        
        std::cout << "\n";
    }
    
    std::cout << std::endl;
    
    // ==================================================================
    // 7. STATISTICHE FINALI
    // ==================================================================
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                      Statistiche Query                      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "📊 Risultati:\n";
    std::cout << "   • Stelle trovate: " << stars.size() << "\n";
    std::cout << "   • Magnitudine media: " << std::fixed << std::setprecision(2);
    
    if (!stars.empty()) {
        double avgMag = 0.0;
        for (const auto& star : stars) {
            avgMag += star->getMagnitude();
        }
        avgMag /= stars.size();
        std::cout << avgMag << "\n";
    } else {
        std::cout << "N/A\n";
    }
    
    std::cout << "   • Magnitudine minima: ";
    if (!stars.empty()) {
        std::cout << stars.front()->getMagnitude() << " (stella più luminosa)\n";
    } else {
        std::cout << "N/A\n";
    }
    
    std::cout << "   • Magnitudine massima: ";
    if (!stars.empty()) {
        std::cout << stars.back()->getMagnitude() << " (stella più debole)\n";
    } else {
        std::cout << "N/A\n";
    }
    
    // Conta stelle con SAO
    size_t withSAO = 0;
    for (const auto& star : stars) {
        auto sao = star->getSAONumber();
        if (sao.has_value() && sao.value() > 0) {
            withSAO++;
        }
    }
    
    std::cout << "   • Stelle con numero SAO: " << withSAO;
    if (!stars.empty()) {
        std::cout << " (" << std::fixed << std::setprecision(1) 
                  << (100.0 * withSAO / stars.size()) << "%)";
    }
    std::cout << "\n";
    
    std::cout << "\n";
    std::cout << "✨ Query completata con successo!\n";
    std::cout << std::endl;
    
    return 0;
}
