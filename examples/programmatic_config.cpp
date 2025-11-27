#include <starmap/StarMap.h>
#include <iostream>

/**
 * Esempio di configurazione programmatica completa
 * con tutti i parametri disponibili
 */

int main() {
    std::cout << "StarMap Library - Programmatic Configuration Example\n\n";

    try {
        // Crea configurazione dettagliata
        starmap::map::MapConfiguration config;
        
        // ===== CENTRO E CAMPO DI VISTA =====
        // Andromeda Galaxy (M31)
        config.center = starmap::core::EquatorialCoordinates(10.68, 41.27);
        config.fieldOfViewWidth = 5.0;
        config.fieldOfViewHeight = 5.0;
        
        // ===== DIMENSIONI OUTPUT =====
        config.imageWidth = 2400;
        config.imageHeight = 1800;
        
        // ===== PROIEZIONE =====
        config.projection = starmap::map::ProjectionType::STEREOGRAPHIC;
        config.coordinateSystem = starmap::map::CoordinateSystem::EQUATORIAL;
        
        // ===== ORIENTAMENTO =====
        config.northUp = true;
        config.eastLeft = true;
        config.rotationAngle = 0.0;
        
        // ===== MAGNITUDINE =====
        config.limitingMagnitude = 12.0;
        
        // ===== GRIGLIA =====
        config.gridStyle.enabled = true;
        config.gridStyle.raStepDegrees = 1.0;   // 4 minuti in RA
        config.gridStyle.decStepDegrees = 1.0;
        config.gridStyle.color = 0x404040FF;     // Grigio scuro
        config.gridStyle.lineWidth = 0.5f;
        config.gridStyle.showLabels = true;
        config.gridStyle.labelColor = 0xC0C0C0FF; // Grigio chiaro
        config.gridStyle.labelFontSize = 10.0f;
        
        // ===== STELLE =====
        config.starStyle.minSymbolSize = 0.8f;
        config.starStyle.maxSymbolSize = 10.0f;
        config.starStyle.magnitudeRange = 12.0f;
        config.starStyle.useSpectralColors = true;
        config.starStyle.useCircles = true;
        config.starStyle.showNames = true;
        config.starStyle.showSAONumbers = true;
        config.starStyle.showMagnitudes = false;
        config.starStyle.minMagnitudeForLabel = 6.0f;
        config.starStyle.labelColor = 0xFFFFFFFF;
        config.starStyle.labelFontSize = 9.0f;
        
        // ===== COLORI =====
        config.backgroundColor = 0x000000FF;  // Nero
        
        // ===== DISPLAY =====
        config.showBorder = true;
        config.showTitle = true;
        config.title = "Andromeda Region (M31)";
        config.showScale = true;
        config.showCompass = true;
        config.showLegend = false;
        
        // ===== OVERLAY =====
        config.showEquator = true;
        config.showEcliptic = false;
        config.showConstellationLines = false;
        config.showConstellationBoundaries = false;
        config.showMilkyWay = false;
        
        // Valida configurazione
        if (!config.validate()) {
            std::cerr << "Errore: Configurazione non valida!\n";
            return 1;
        }
        
        std::cout << "Configurazione validata.\n";
        std::cout << "Target: " << config.title << "\n";
        std::cout << "Centro: RA=" << config.center.toHMSString() 
                  << ", Dec=" << config.center.toDMSString() << "\n";
        std::cout << "FOV: " << config.fieldOfViewWidth << "° x " 
                  << config.fieldOfViewHeight << "°\n";
        std::cout << "Risoluzione: " << config.imageWidth << "x" 
                  << config.imageHeight << "\n";
        std::cout << "Magnitudine limite: " << config.limitingMagnitude << "\n\n";
        
        // Opzionale: Salva configurazione come JSON per riutilizzo
        starmap::config::JSONConfigLoader jsonLoader;
        std::string jsonConfig = jsonLoader.saveToString(config);
        
        std::cout << "Configurazione JSON:\n";
        std::cout << jsonConfig << "\n\n";
        
        // Genera mappa
        std::cout << "Generazione mappa in corso...\n";
        
        starmap::StarMapBuilder builder;
        builder.setCatalogTimeout(90); // Timeout più lungo per regione complessa
        
        auto image = builder.generate(config, true);
        
        std::cout << "✓ Mappa generata con successo!\n";
        std::cout << "  Dimensioni: " << image.width << "x" << image.height << "\n";
        std::cout << "  Dati: " << image.data.size() << " bytes\n";
        
        // Salva
        std::string outputFile = "andromeda_programmatic.png";
        if (image.saveAsPNG(outputFile)) {
            std::cout << "✓ Salvata in: " << outputFile << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
