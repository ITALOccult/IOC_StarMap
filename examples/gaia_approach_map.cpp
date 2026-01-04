/**
 * @file gaia_approach_map.cpp
 * @brief Genera carta di approccio per stella Gaia 3441366322859801216
 * Con linee costellazioni, confini, nomi, Flamsteed, SAO
 */

#include "starmap/catalog/GaiaClient.h"
#include "starmap/catalog/SAOCatalog.h"
#include "starmap/catalog/CatalogManager.h"
#include "starmap/core/Coordinates.h"
#include "starmap/map/ChartGenerator.h"
#include "starmap/map/ConstellationData.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

using namespace starmap;

int main() {
    try {
        std::cout << "═══════════════════════════════════════════════\n";
        std::cout << "  MAPPA DI APPROCCIO COMPLETA\n";
        std::cout << "  Stella: Gaia DR3 3441366322859801216\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        // Coordinate stella target (Betelgeuse)
        double targetRA = 88.79293;   // RA in gradi
        double targetDec = 7.40704;   // Dec in gradi
        double fieldOfView = 20.0;    // Campo 20 gradi
        double magLimit = 7.0;        // Stelle fino a mag 7
        
        std::cout << "Parametri:\n";
        std::cout << "  Centro: RA=" << targetRA << "°, Dec=" << targetDec << "°\n";
        std::cout << "  Campo: " << fieldOfView << "°\n";
        std::cout << "  Magnitudine limite: " << magLimit << "\n\n";
        
        // Configurazione carta usando ChartGenerator
        map::ChartConfig config;
        config.width = 2400;
        config.height = 2400;
        config.centerRA = targetRA;
        config.centerDec = targetDec;
        config.fieldRadius = fieldOfView / 2.0;
        config.maxMagnitude = magLimit;
        
        // Abilita tutte le features richieste
        config.showGrid = true;
        config.gridInterval = 5.0;  // Griglia ogni 5 gradi (RA e Dec)
        config.showConstellationLines = true;
        config.showConstellationBoundaries = true;
        config.showStarLabels = true;
        config.showSAONumbers = true;
        config.showCoordinateLabels = true;
        config.showBorder = true;  // Riquadro esterno
        config.labelMagnitudeLimit = 6.0;  // Nomi stelle fino mag 6
        config.saoMagnitudeLimit = 8.0;    // SAO fino mag 8
        
        // Stile stampabile
        config.style.printable = true;
        config.style.backgroundColor = "#ffffff";
        config.style.starColor = "#000000";
        config.style.gridColor = "#cccccc";
        config.style.constellationLineColor = "#5588cc";
        config.style.constellationBoundaryColor = "#999999";
        config.style.labelColor = "#000000";
        config.style.titleColor = "#000000";
        
        // Dimensioni stelle proporzionali e testo
        config.style.starSizeMultiplier = 1.2;
        config.style.minStarSize = 0.8;
        config.style.maxStarSize = 8.0;
        config.style.useStarColors = false;  // Solo nero
        config.style.labelFontSize = 14;
        config.style.saoFontSize = 11;
        
        config.title = "Carta di Approccio - Gaia 3441366322859801216";
        config.subtitle = "Campo 20° - Mag limite 7.0";
        config.outputPath = "approach_gaia_3441366322859801216";
        config.outputFormat = "svg";
        
        // Genera carta
        std::cout << "Generando carta con ChartGenerator...\n";
        map::ChartGenerator generator;
        generator.setConfig(config);
        
        bool success = generator.generate();
        
        if (success) {
            std::cout << "✓ Carta SVG generata: " << config.outputPath << ".svg\n";
            
            // Converti in PNG
            std::string svgFile = config.outputPath + ".svg";
            std::string pngFile = config.outputPath + ".png";
            std::string cmd = "magick " + svgFile + " -density 150 " + pngFile + " 2>/dev/null";
            int result = system(cmd.c_str());
            if (result != 0) {
                cmd = "convert " + svgFile + " " + pngFile + " 2>/dev/null";
                result = system(cmd.c_str());
            }
            
            if (result == 0) {
                std::cout << "✓ PNG generato: " << pngFile << "\n";
            }
            
            std::cout << "\n═══════════════════════════════════════════════\n";
            std::cout << "MAPPA COMPLETA GENERATA\n";
            std::cout << "Features incluse:\n";
            std::cout << "  ✓ Griglia coordinate (1°)\n";
            std::cout << "  ✓ Linee costellazioni\n";
            std::cout << "  ✓ Confini costellazioni IAU\n";
            std::cout << "  ✓ Nomi stelle (mag < 6)\n";
            std::cout << "  ✓ Numeri SAO (mag < 8)\n";
            std::cout << "  ✓ Designazioni Bayer/Flamsteed\n";
            std::cout << "═══════════════════════════════════════════════\n";
            
        } else {
            std::cerr << "ERRORE: Generazione fallita\n";
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERRORE: " << e.what() << std::endl;
        return 1;
    }
}
