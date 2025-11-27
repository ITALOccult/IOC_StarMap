/**
 * @file leo_constellation.cpp
 * @brief Esempio: Mappa della Costellazione del Leone
 * 
 * Genera una mappa dettagliata della costellazione del Leone con:
 * - Centro su Regolo (α Leonis), la stella più brillante
 * - Campo di vista 30° × 30° per includere tutta la costellazione
 * - Stelle fino a magnitudine 12
 * - Numeri SAO per le stelle principali
 * - Griglia equatoriale
 * - Rendering con colori spettrali
 */

#include <starmap/StarMap.h>
#include <iostream>
#include <iomanip>

using namespace starmap;

void printStarInfo(const core::Star& star) {
    std::cout << "  • ";
    
    if (star.getSAONumber() > 0) {
        std::cout << "SAO " << star.getSAONumber() << " / ";
    }
    
    std::cout << "GAIA DR3 " << star.getGaiaId()
              << " | Mag: " << std::fixed << std::setprecision(2) 
              << star.getMagnitude()
              << " | RA: " << std::setprecision(3)
              << star.getCoordinates().getRightAscension() << "°"
              << " Dec: " << star.getCoordinates().getDeclination() << "°";
    
    if (star.getColorIndex() > -10.0) {
        std::cout << " | B-V: " << std::setprecision(2) << star.getColorIndex();
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║        Mappa Costellazione del Leone (Leo)                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << std::endl;
    
    // ==================================================================
    // 1. CONFIGURAZIONE MAPPA
    // ==================================================================
    
    map::MapConfiguration config;
    
    // Centro su Regolo (α Leonis) - Stella più brillante del Leone
    // RA: 10h 08m 22s = 152.0958° | Dec: +11° 58' 02" = +11.9672°
    config.center = core::EquatorialCoordinates(152.0958, 11.9672);
    
    // Campo di vista: 30° × 30° per includere tutta la costellazione
    config.fieldOfViewWidth = 30.0;
    config.fieldOfViewHeight = 30.0;
    
    // Dimensioni immagine: 4K per alta qualità
    config.imageWidth = 3840;
    config.imageHeight = 2160;
    
    // Limite magnitudine: 12 per vedere anche stelle deboli
    config.limitingMagnitude = 12.0;
    
    // Proiezione stereografica (standard per mappe celesti)
    config.projectionType = map::ProjectionType::Stereographic;
    
    // Configurazione griglia
    config.showGrid = true;
    config.gridRASpacing = 1.0;   // Intervallo 1h in RA
    config.gridDecSpacing = 10.0; // Intervallo 10° in Dec
    config.gridColor = {80, 80, 100, 255}; // Grigio-blu tenue
    config.gridLineWidth = 1.0;
    
    // Linee speciali
    config.showEquator = true;
    config.equatorColor = {100, 150, 200, 255}; // Azzurro
    config.showEcliptic = true;
    config.eclipticColor = {200, 150, 100, 255}; // Arancione
    
    // Configurazione stelle
    config.showStars = true;
    config.starScaleFactor = 1.2;  // Stelle leggermente più grandi
    config.starColorBySpectralType = true;
    config.showSAONumbers = true;  // Mostra numeri SAO
    config.saoNumberThreshold = 6.0; // Solo per stelle mag ≤ 6
    
    // Etichette
    config.showLabels = true;
    config.labelFont = "Arial";
    config.labelSize = 12;
    config.labelColor = {255, 255, 200, 255}; // Giallo chiaro
    
    // Sfondo
    config.backgroundColor = {5, 5, 15, 255}; // Blu scuro notturno
    
    std::cout << "📐 Configurazione Mappa:\n";
    std::cout << "   Centro: Regolo (α Leonis)\n";
    std::cout << "   RA: " << config.center.toHMSString() << "\n";
    std::cout << "   Dec: " << config.center.toDMSString() << "\n";
    std::cout << "   Campo: " << config.fieldOfViewWidth << "° × " 
              << config.fieldOfViewHeight << "°\n";
    std::cout << "   Risoluzione: " << config.imageWidth << " × " 
              << config.imageHeight << " px\n";
    std::cout << "   Mag limite: " << config.limitingMagnitude << "\n";
    std::cout << std::endl;
    
    // ==================================================================
    // 2. QUERY CATALOGO GAIA
    // ==================================================================
    
    std::cout << "🌟 Query GAIA DR3 per Leone...\n";
    
    catalog::CatalogManager catalogMgr;
    
    // Verifica disponibilità catalogo locale
    if (catalogMgr.getGaiaClient().isLocalCatalogAvailable()) {
        std::cout << "   ✓ Usando catalogo locale Mag18 V2 (query veloce)\n";
    } else {
        std::cout << "   ⚠ Catalogo locale non disponibile, usando query online\n";
        std::cout << "   (Download consigliato: ~/catalogs/gaia_mag18_v2.cat)\n";
    }
    
    catalog::GaiaQueryParameters queryParams;
    queryParams.center = config.center;
    queryParams.radiusDegrees = 15.0; // Raggio per coprire 30° × 30°
    queryParams.maxMagnitude = config.limitingMagnitude;
    queryParams.maxResults = 100000;
    queryParams.includeProperMotion = true;
    queryParams.includeParallax = true;
    
    auto stars = catalogMgr.queryStars(queryParams, true);
    
    std::cout << "   ✓ Trovate " << stars.size() << " stelle\n";
    
    // Statistiche catalogo
    auto stats = catalogMgr.getGaiaClient().getStatistics();
    if (!stats.isOnline) {
        std::cout << "   📊 Cache: " << stats.cacheHitsMisses.first << " hits, "
                  << stats.cacheHitsMisses.second << " misses\n";
    }
    std::cout << std::endl;
    
    // ==================================================================
    // 3. STELLE PRINCIPALI DEL LEONE
    // ==================================================================
    
    std::cout << "⭐ Stelle Principali del Leone:\n\n";
    
    // Stelle luminose del Leone con nomi
    struct NamedStar {
        std::string name;
        double ra;
        double dec;
        double mag;
    };
    
    std::vector<NamedStar> brightStars = {
        {"Regolo (α Leo)", 152.0958, 11.9672, 1.35},    // La più brillante
        {"Denebola (β Leo)", 177.2647, 14.5721, 2.14},  // Coda del leone
        {"Algieba (γ Leo)", 154.9926, 19.8415, 2.01},   // Sistema doppio
        {"Zosma (δ Leo)", 168.5269, 20.5236, 2.56},     // Dorso
        {"Chort (θ Leo)", 168.5605, 15.4294, 3.34},     // Fianco
        {"Al Minliar (κ Leo)", 148.1903, 26.1825, 4.46} // Testa
    };
    
    for (const auto& named : brightStars) {
        // Cerca la stella nel dataset
        std::shared_ptr<core::Star> found = nullptr;
        double minDist = 1.0; // 1 grado di tolleranza
        
        for (const auto& star : stars) {
            double dist = config.center.angularDistance(
                core::EquatorialCoordinates(named.ra, named.dec),
                star->getCoordinates()
            );
            
            if (dist < minDist && std::abs(star->getMagnitude() - named.mag) < 0.5) {
                minDist = dist;
                found = star;
            }
        }
        
        std::cout << "  🌟 " << named.name << "\n";
        if (found) {
            printStarInfo(*found);
        } else {
            std::cout << "     (Stimata: RA " << named.ra << "°, Dec " 
                      << named.dec << "°, Mag " << named.mag << ")\n";
        }
        std::cout << std::endl;
    }
    
    // ==================================================================
    // 4. GENERAZIONE MAPPA
    // ==================================================================
    
    std::cout << "🎨 Generazione mappa...\n";
    
    map::MapRenderer renderer(config);
    
    // Rendering
    auto imageBuffer = renderer.render(stars);
    
    std::cout << "   ✓ Mappa renderizzata (" << imageBuffer.width << " × " 
              << imageBuffer.height << " px)\n";
    std::cout << std::endl;
    
    // ==================================================================
    // 5. SALVATAGGIO
    // ==================================================================
    
    std::string filename = "leo_constellation_map.png";
    
    std::cout << "💾 Salvataggio mappa...\n";
    
    if (imageBuffer.saveAsPNG(filename)) {
        std::cout << "   ✓ Mappa salvata: " << filename << "\n";
    } else {
        std::cout << "   ⚠ Salvataggio PNG richiede stb_image_write\n";
        std::cout << "   💡 Buffer RGBA disponibile in memoria per export custom\n";
    }
    
    std::cout << std::endl;
    
    // ==================================================================
    // 6. INFORMAZIONI COSTELLAZIONE
    // ==================================================================
    
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                 Info Costellazione del Leone                ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "📖 Nome: Leo (Leone)\n";
    std::cout << "📏 Dimensione: 947 gradi quadrati (12ª più grande)\n";
    std::cout << "🌟 Stelle principali: 9 (pattern a \"falce\" e \"trapezio\")\n";
    std::cout << "✨ Stelle visibili (mag < 6): ~95\n";
    std::cout << "🔭 Oggetti deep-sky: M65, M66, M95, M96, M105 (galassie)\n";
    std::cout << "📅 Migliore visibilità: Marzo-Aprile (primavera boreale)\n";
    std::cout << "🌍 Visibile: Latitudini +90° a -65°\n";
    std::cout << "\n";
    std::cout << "⭐ Asterismo principale:\n";
    std::cout << "   - La \"Falce\": Regolo, η, γ, ζ, μ, ε Leo (testa e criniera)\n";
    std::cout << "   - Il \"Trapezio\": Denebola, β, δ, θ Leo (corpo e coda)\n";
    std::cout << "\n";
    std::cout << "🪐 Regolo (α Leo):\n";
    std::cout << "   - Magnitudine: 1.35 (21ª stella più luminosa)\n";
    std::cout << "   - Tipo spettrale: B8 IVn (gigante blu-bianca)\n";
    std::cout << "   - Distanza: ~79 anni luce\n";
    std::cout << "   - Temperatura: ~12,000 K\n";
    std::cout << "   - Rotazione rapida: ~316 km/s (quasi al limite di rottura)\n";
    std::cout << "   - Sistema multiplo: 4 componenti\n";
    std::cout << "   - Posizione: Molto vicina all'eclittica (spesso occultata)\n";
    std::cout << "\n";
    std::cout << "✨ Completato!\n";
    std::cout << std::endl;
    
    return 0;
}
