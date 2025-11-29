/**
 * @file generate_chart.cpp
 * @brief Tool da linea di comando per generare carte stellari da configurazione JSON
 * 
 * Uso:
 *   generate_chart config.json
 *   generate_chart --constellation Leo --output leo_map
 *   generate_chart --help
 */

#include "starmap/map/ChartGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace starmap::map;

void printUsage() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════╗
║                    STARMAP CHART GENERATOR                        ║
║              Genera carte stellari da catalogo Gaia               ║
╚═══════════════════════════════════════════════════════════════════╝

USO:
    generate_chart <config.json>
    generate_chart [opzioni]

PRESET TELESCOPIO:
    --finder <ra> <dec>          Carta puntamento (25°, mag 8, stampabile)
    --detail <ra> <dec>          Carta dettaglio (2°, mag 16, stampabile)
    --print                      Stile stampabile (stelle nere, sfondo bianco)

OPZIONI:
    -c, --constellation <name>   Costellazione (Leo, Orion, UrsaMajor, etc.)
    -o, --output <path>          Percorso output (senza estensione)
    -f, --format <fmt>           Formato output: svg, png, jpg (default: png)
    -r, --radius <deg>           Raggio campo visivo in gradi (default: 25)
    -m, --magnitude <mag>        Magnitudine limite (default: 6.5)
    --ra <deg>                   Ascensione retta centro (gradi)
    --dec <deg>                  Declinazione centro (gradi)
    --width <px>                 Larghezza immagine (default: 800)
    --height <px>                Altezza immagine (default: 800)
    --no-grid                    Nascondi griglia
    --no-lines                   Nascondi linee costellazione
    --no-labels                  Nascondi etichette stelle
    --no-legend                  Nascondi legenda
    --sao                        Mostra numeri SAO
    --boundaries                 Mostra confini costellazioni
    --list                       Lista costellazioni disponibili
    -h, --help                   Mostra questo messaggio

ESEMPI:
    # Genera carta del Leone in PNG
    generate_chart -c Leo -o leone -f png

    # Carta di puntamento per M42 (Orion Nebula)
    generate_chart --finder 83.82 -5.39 -o m42_finder

    # Carta di dettaglio per M42 a mag 14
    generate_chart --detail 83.82 -5.39 -m 14 -r 1.5 -o m42_detail

    # Genera da file JSON
    generate_chart my_chart.json

    # Carta stampabile custom
    generate_chart --ra 88.79 --dec 7.41 -r 15 --print -o betelgeuse

FILE JSON ESEMPIO:
    {
        "title": "Finder Chart",
        "preset": "finder",
        "centerRA": 83.82,
        "centerDec": -5.39,
        "fieldRadius": 25,
        "maxMagnitude": 8.0,
        "showSAONumbers": true,
        "showConstellationBoundaries": true,
        "outputFormat": "png"
    }

)";
}

void listConstellations() {
    std::cout << "\nCostellazioni disponibili:\n";
    std::cout << "─────────────────────────\n";
    for (const auto& name : ChartGenerator::getAvailableConstellations()) {
        auto data = ChartGenerator::getConstellationData(name);
        if (data.has_value()) {
            std::cout << "  • " << name << " (" << data->abbreviation << ") - "
                      << "RA " << data->centerRA << "° Dec " << data->centerDec << "°\n";
        }
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    ChartConfig config;
    std::string jsonFile;
    std::string userOutputPath;  // Output specificato dall'utente
    bool hasCenter = false;
    bool hasRadius = false;      // L'utente ha specificato -r
    bool hasMagnitude = false;   // L'utente ha specificato -m
    bool useFinderPreset = false;
    bool useDetailPreset = false;
    bool usePrintStyle = false;
    double presetRA = 0, presetDec = 0;
    double userRadius = 0;       // Raggio specificato dall'utente
    double userMagnitude = 0;    // Magnitudine specificata dall'utente
    
    // Parse argomenti
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        }
        else if (arg == "--list") {
            listConstellations();
            return 0;
        }
        else if (arg == "--finder" && i + 2 < argc) {
            useFinderPreset = true;
            presetRA = std::stod(argv[++i]);
            presetDec = std::stod(argv[++i]);
        }
        else if (arg == "--detail" && i + 2 < argc) {
            useDetailPreset = true;
            presetRA = std::stod(argv[++i]);
            presetDec = std::stod(argv[++i]);
        }
        else if (arg == "--print") {
            usePrintStyle = true;
        }
        else if (arg == "--sao") {
            config.showSAONumbers = true;
        }
        else if (arg == "--boundaries") {
            config.showConstellationBoundaries = true;
        }
        else if ((arg == "-c" || arg == "--constellation") && i + 1 < argc) {
            config.constellation = argv[++i];
        }
        else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            userOutputPath = argv[++i];
            config.outputPath = userOutputPath;
        }
        else if ((arg == "-f" || arg == "--format") && i + 1 < argc) {
            config.outputFormat = argv[++i];
        }
        else if ((arg == "-r" || arg == "--radius") && i + 1 < argc) {
            userRadius = std::stod(argv[++i]);
            hasRadius = true;
            config.fieldRadius = userRadius;
        }
        else if ((arg == "-m" || arg == "--magnitude") && i + 1 < argc) {
            userMagnitude = std::stod(argv[++i]);
            hasMagnitude = true;
            config.maxMagnitude = userMagnitude;
        }
        else if (arg == "--ra" && i + 1 < argc) {
            config.centerRA = std::stod(argv[++i]);
            hasCenter = true;
        }
        else if (arg == "--dec" && i + 1 < argc) {
            config.centerDec = std::stod(argv[++i]);
            hasCenter = true;
        }
        else if (arg == "--width" && i + 1 < argc) {
            config.width = std::stoi(argv[++i]);
        }
        else if (arg == "--height" && i + 1 < argc) {
            config.height = std::stoi(argv[++i]);
        }
        else if (arg == "--no-grid") {
            config.showGrid = false;
        }
        else if (arg == "--no-lines") {
            config.showConstellationLines = false;
        }
        else if (arg == "--no-labels") {
            config.showStarLabels = false;
        }
        else if (arg == "--no-legend") {
            config.showLegend = false;
        }
        else if (arg[0] != '-' && jsonFile.empty()) {
            // Potrebbe essere un file JSON
            jsonFile = arg;
        }
        else {
            std::cerr << "Opzione sconosciuta: " << arg << "\n";
            return 1;
        }
    }
    
    ChartGenerator generator;
    
    // Gestione preset
    if (useFinderPreset) {
        // Finder: default 20°, mag 8
        double radius = hasRadius ? userRadius : 20.0;
        config = ChartGenerator::createFinderChart(presetRA, presetDec, radius);
        if (!userOutputPath.empty()) {
            config.outputPath = userOutputPath;
        } else {
            std::ostringstream oss;
            oss << "finder_" << static_cast<int>(presetRA) << "_" << static_cast<int>(presetDec);
            config.outputPath = oss.str();
        }
    }
    else if (useDetailPreset) {
        // Detail: default 2°, mag 16
        double radius = hasRadius ? userRadius : 2.0;
        double mag = hasMagnitude ? userMagnitude : 16.0;
        config = ChartGenerator::createDetailChart(presetRA, presetDec, radius, mag);
        if (!userOutputPath.empty()) {
            config.outputPath = userOutputPath;
        } else {
            std::ostringstream oss;
            oss << "detail_" << static_cast<int>(presetRA) << "_" << static_cast<int>(presetDec);
            config.outputPath = oss.str();
        }
    }
    else if (!jsonFile.empty()) {
        // Se c'è un file JSON, caricalo
        std::cout << "Caricamento configurazione: " << jsonFile << "\n";
        if (!generator.loadConfig(jsonFile)) {
            std::cerr << "Errore: " << generator.getLastError() << "\n";
            return 1;
        }
    } else {
        // Configurazione manuale
        if (usePrintStyle) {
            generator.applyPreset(ChartPreset::PrintBlack);
            config = generator.getConfig();
        }
        
        // Se c'è una costellazione, imposta i default
        if (!config.constellation.empty()) {
            auto constData = ChartGenerator::getConstellationData(config.constellation);
            if (constData.has_value()) {
                if (!hasCenter) {
                    config.centerRA = constData->centerRA;
                    config.centerDec = constData->centerDec;
                }
                if (config.title.empty()) {
                    config.title = constData->name;
                }
                if (config.subtitle.empty()) {
                    config.subtitle = "Gaia DR3 Catalog";
                }
            } else {
                std::cerr << "Costellazione non trovata: " << config.constellation << "\n";
                std::cerr << "Usa --list per vedere le costellazioni disponibili\n";
                return 1;
            }
        }
        
        // Output path default
        if (config.outputPath.empty()) {
            config.outputPath = config.constellation.empty() ? "star_chart" : config.constellation + "_chart";
        }
        
        generator.setConfig(config);
    }
    
    // Applica override da linea di comando se usiamo preset
    if (useFinderPreset || useDetailPreset) {
        generator.setConfig(config);
    }
    
    // Mostra configurazione
    const auto& cfg = generator.getConfig();
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════╗\n";
    std::cout << "║           GENERAZIONE CARTA STELLARE          ║\n";
    std::cout << "╚═══════════════════════════════════════════════╝\n\n";
    
    std::cout << "Configurazione:\n";
    std::cout << "  Centro:        RA " << cfg.centerRA << "° Dec " 
              << (cfg.centerDec >= 0 ? "+" : "") << cfg.centerDec << "°\n";
    std::cout << "  Raggio:        " << cfg.fieldRadius << "°\n";
    std::cout << "  Magnitudine:   ≤ " << cfg.maxMagnitude << "\n";
    std::cout << "  Dimensioni:    " << cfg.width << "x" << cfg.height << " px\n";
    std::cout << "  Formato:       " << cfg.outputFormat << "\n";
    std::cout << "  Stile:         " << (cfg.style.printable ? "STAMPABILE" : "schermo") << "\n";
    if (!cfg.constellation.empty()) {
        std::cout << "  Costellazione: " << cfg.constellation << "\n";
    }
    if (cfg.showSAONumbers) {
        std::cout << "  Numeri SAO:    sì (mag ≤ " << cfg.saoMagnitudeLimit << ")\n";
    }
    std::cout << "\n";
    
    // Genera
    std::cout << "Generazione in corso...\n";
    
    if (!generator.generate()) {
        std::cerr << "Errore: " << generator.getLastError() << "\n";
        return 1;
    }
    
    std::cout << "\n✓ Carta generata: " << generator.getOutputPath() << "\n\n";
    
    return 0;
}
