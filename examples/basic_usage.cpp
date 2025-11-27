#include <starmap/StarMap.h>
#include <iostream>

int main() {
    std::cout << "StarMap Library - Basic Usage Example\n";
    std::cout << "Version: " << starmap::VERSION << "\n\n";

    try {
        // Crea configurazione di base
        starmap::map::MapConfiguration config;
        
        // Imposta centro su Orione (circa RA=5h35m, Dec=+5°)
        config.center = starmap::core::EquatorialCoordinates(83.75, 5.0);
        
        // Campo di vista: 10° x 10°
        config.fieldOfViewWidth = 10.0;
        config.fieldOfViewHeight = 10.0;
        
        // Dimensioni immagine
        config.imageWidth = 1920;
        config.imageHeight = 1080;
        
        // Magnitudine limite
        config.limitingMagnitude = 10.0;
        
        // Abilita griglia e elementi
        config.gridStyle.enabled = true;
        config.showEquator = true;
        config.showCompass = true;
        config.showBorder = true;
        config.title = "Orion Region";
        config.showTitle = true;
        
        // Stile stelle
        config.starStyle.useSpectralColors = true;
        config.starStyle.showSAONumbers = true;
        config.starStyle.minMagnitudeForLabel = 5.0;
        
        std::cout << "Configurazione creata.\n";
        std::cout << "Centro: RA=" << config.center.getRightAscension() 
                  << "°, Dec=" << config.center.getDeclination() << "°\n";
        std::cout << "FOV: " << config.fieldOfViewWidth << "° x " 
                  << config.fieldOfViewHeight << "°\n\n";
        
        // Genera mappa
        std::cout << "Generazione mappa in corso...\n";
        
        starmap::StarMapBuilder builder;
        builder.setCatalogTimeout(60);
        
        auto image = builder.generate(config, true);
        
        std::cout << "Mappa generata: " << image.width << "x" << image.height 
                  << " pixels\n";
        
        // Salva immagine
        std::string outputFile = "orion_map.png";
        std::cout << "Salvataggio in '" << outputFile << "'...\n";
        
        if (image.saveAsPNG(outputFile)) {
            std::cout << "✓ Mappa salvata con successo!\n";
        } else {
            std::cout << "⚠ Nota: Il salvataggio richiede una libreria PNG\n";
            std::cout << "  I dati della mappa sono comunque disponibili in memoria.\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
