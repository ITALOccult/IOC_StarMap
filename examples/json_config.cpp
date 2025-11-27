#include <starmap/StarMap.h>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "StarMap Library - JSON Configuration Example\n\n";

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config.json>\n";
        std::cout << "Example: " << argv[0] << " config_examples/orion.json\n";
        return 1;
    }

    std::string configFile = argv[1];
    
    try {
        std::cout << "Caricamento configurazione da: " << configFile << "\n";
        
        // Usa il builder per caricare da file JSON
        starmap::StarMapBuilder builder;
        builder.setCatalogTimeout(60);
        builder.setCacheEnabled(true);
        
        auto image = builder.generateFromFile(configFile, true);
        
        std::cout << "✓ Mappa generata: " << image.width << "x" << image.height 
                  << " pixels\n";
        
        // Salva con nome basato sul file di configurazione
        std::string outputFile = configFile;
        size_t lastDot = outputFile.find_last_of('.');
        if (lastDot != std::string::npos) {
            outputFile = outputFile.substr(0, lastDot);
        }
        outputFile += "_output.png";
        
        std::cout << "Salvataggio in: " << outputFile << "\n";
        
        if (image.saveAsPNG(outputFile)) {
            std::cout << "✓ Completato!\n";
        } else {
            std::cout << "⚠ Nota: Implementare libreria PNG per salvare\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
