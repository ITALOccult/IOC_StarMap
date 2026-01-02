/**
 * @file occultation_chart.cpp
 * @brief Esempio di utilizzo delle API per carte di occultazione asteroidale
 * 
 * Questo esempio mostra come:
 * 1. Caricare dati di occultazione da JSON
 * 2. Generare carte di avvicinamento e dettagliate
 * 3. Personalizzare la configurazione delle carte
 */

#include "starmap/occultation/OccultationChartBuilder.h"
#include "starmap/occultation/OccultationData.h"
#include <iostream>
#include <iomanip>

using namespace starmap;
using namespace starmap::occultation;

void printEventInfo(const OccultationEvent& event) {
    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              EVENTO DI OCCULTAZIONE ASTEROIDALE                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "ID Evento:     " << event.eventId << "\n";
    std::cout << "Sorgente dati: " << event.source << "\n\n";
    
    std::cout << "STELLA TARGET:\n";
    std::cout << "  Catalogo:    " << event.targetStar.catalogId << "\n";
    std::cout << "  RA:          " << std::fixed << std::setprecision(4) 
              << event.targetStar.coordinates.getRightAscension() << "°\n";
    std::cout << "  Dec:         " << event.targetStar.coordinates.getDeclination() << "°\n";
    std::cout << "  Magnitudine: " << std::setprecision(2) << event.targetStar.magnitude << "\n";
    
    if (event.targetStar.saoNumber.has_value()) {
        std::cout << "  SAO:         " << event.targetStar.saoNumber.value() << "\n";
    }
    
    std::cout << "\nASTEROIDE:\n";
    std::cout << "  Nome:        " << event.asteroid.name << "\n";
    if (event.asteroid.number > 0) {
        std::cout << "  Numero:      (" << event.asteroid.number << ")\n";
    }
    std::cout << "  Magnitudine: " << std::setprecision(2) << event.asteroid.magnitude << "\n";
    std::cout << "  Diametro:    " << std::setprecision(1) << event.asteroid.diameter << " km\n";
    std::cout << "  Velocità:    " << event.asteroid.velocity << " arcsec/h\n";
    
    std::cout << "\nCIRCOSTANZE:\n";
    std::cout << "  Data/Ora:    " << event.circumstances.eventTime << "\n";
    std::cout << "  Durata max:  " << std::setprecision(2) 
              << event.circumstances.duration << " s\n";
    std::cout << "  Largh. ombra:" << std::setprecision(1) 
              << event.circumstances.shadowWidth << " km\n";
    std::cout << "  Drop mag:    " << std::setprecision(2) 
              << event.circumstances.magnitudeDrop << "\n";
    std::cout << "  Probabilità: " << std::setprecision(0) 
              << (event.circumstances.probability * 100.0) << "%\n";
    
    if (event.observationSite.has_value()) {
        const auto& site = event.observationSite.value();
        std::cout << "\nSITO OSSERVAZIONE:\n";
        std::cout << "  Nome:        " << site.name << "\n";
        std::cout << "  Lat/Lon:     " << std::setprecision(4) 
                  << site.location.latitude << "°, " 
                  << site.location.longitude << "°\n";
        std::cout << "  Altitudine:  " << std::setprecision(0) 
                  << site.altitude << " m\n";
        std::cout << "  Elevazione:  " << std::setprecision(1) 
                  << site.elevation << "°\n";
    }
    
    std::cout << "\n════════════════════════════════════════════════════════════════\n\n";
}

int main(int argc, char* argv[]) {
    // Banner
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  IOC STARMAP - Generatore Carte per Occultazioni Asteroidali    ║\n";
    std::cout << "║  International Occultation Timing Association (IOTA)            ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    // Parse command line
    std::string jsonFile;
    
    if (argc < 2) {
        std::cerr << "\nUso: " << argv[0] << " <file_json_occultazione>\n\n";
        std::cerr << "Esempio:\n";
        std::cerr << "  " << argv[0] << " event_2025_12_15.json\n\n";
        return 1;
    }
    
    jsonFile = argv[1];
    
    std::cout << "\n🔄 Caricamento dati occultazione da: " << jsonFile << "\n";
    
    // Crea il builder
    OccultationChartBuilder builder;
    
    // Carica dati da JSON
    if (!builder.fromJSONFile(jsonFile)) {
        std::cerr << "\n❌ ERRORE: Impossibile caricare il file JSON!\n\n";
        
        auto messages = builder.getValidationMessages();
        if (!messages.empty()) {
            std::cerr << "Messaggi di errore:\n";
            for (const auto& msg : messages) {
                std::cerr << "  • " << msg << "\n";
            }
        }
        return 1;
    }
    
    std::cout << "✅ Dati caricati con successo!\n";
    
    // Valida i dati
    if (!builder.validate()) {
        std::cerr << "\n⚠️  WARNING: I dati potrebbero essere incompleti\n";
        
        auto messages = builder.getValidationMessages();
        for (const auto& msg : messages) {
            std::cerr << "  • " << msg << "\n";
        }
        std::cerr << "\nContinuo comunque...\n";
    }
    
    // Mostra informazioni sull'evento
    const auto& event = builder.getEvent();
    printEventInfo(event);
    
    // Configura cataloghi
    std::cout << "🔧 Configurazione:\n";
    std::cout << "  - Timeout cataloghi: 30s\n";
    std::cout << "  - Cache abilitata: sì\n";
    std::cout << "  - Log level: info\n\n";
    
    builder.setCatalogTimeout(30);
    builder.setCacheEnabled(true);
    builder.setLogLevel(3);
    
    // ========================================================================
    // Genera CARTA DI AVVICINAMENTO (campo largo)
    // ========================================================================
    
    std::cout << "📊 Generazione CARTA DI AVVICINAMENTO (campo largo)...\n";
    std::cout << "  - Campo visivo: 10° × 10°\n";
    std::cout << "  - Magnitudine limite: 12.0\n";
    std::cout << "  - Traccia asteroide: ±3 ore\n";
    
    std::string approachFile = "occultation_approach.png";
    
    try {
        if (builder.generateAndSaveApproachChart(approachFile)) {
            std::cout << "✅ Carta di avvicinamento salvata: " << approachFile << "\n\n";
        } else {
            std::cout << "❌ Errore generazione carta di avvicinamento\n\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Eccezione: " << e.what() << "\n\n";
    }
    
    // ========================================================================
    // Genera CARTA DETTAGLIATA (campo stretto 3° x 1.5°)
    // ========================================================================
    
    std::cout << "📊 Generazione CARTA DETTAGLIATA (campo stretto)...\n";
    std::cout << "  - Campo visivo: 3° × 1.5°\n";
    std::cout << "  - Magnitudine limite: 15.0\n";
    std::cout << "  - Traccia asteroide: ±1 ora\n";
    
    std::string detailFile = "occultation_detail.png";
    
    try {
        if (builder.generateAndSaveDetailChart(detailFile)) {
            std::cout << "✅ Carta dettagliata salvata: " << detailFile << "\n\n";
        } else {
            std::cout << "❌ Errore generazione carta dettagliata\n\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Eccezione: " << e.what() << "\n\n";
    }
    
    // ========================================================================
    // Genera CARTA PERSONALIZZATA
    // ========================================================================
    
    std::cout << "📊 Generazione CARTA PERSONALIZZATA...\n";
    
    // Configurazione custom
    OccultationChartConfig customConfig;
    customConfig.chartType = ChartType::CUSTOM;
    customConfig.fieldOfViewWidth = 5.0;
    customConfig.fieldOfViewHeight = 3.0;
    customConfig.imageWidth = 2560;
    customConfig.imageHeight = 1440;
    customConfig.limitingMagnitude = 14.0;
    customConfig.showGrid = true;
    customConfig.showAsteroidPath = true;
    customConfig.showTargetStar = true;
    customConfig.showCircumstances = true;
    customConfig.pathDurationHours = 1.5;
    customConfig.customTitle = "Occultazione Asteroidale - Carta Personalizzata";
    
    std::cout << "  - Campo visivo: " << customConfig.fieldOfViewWidth 
              << "° × " << customConfig.fieldOfViewHeight << "°\n";
    std::cout << "  - Risoluzione: " << customConfig.imageWidth 
              << " × " << customConfig.imageHeight << "\n";
    std::cout << "  - Magnitudine limite: " << customConfig.limitingMagnitude << "\n";
    
    std::string customFile = "occultation_custom.png";
    
    try {
        auto buffer = builder.generateCustomChart(customConfig);
        if (builder.saveChart(buffer, customFile)) {
            std::cout << "✅ Carta personalizzata salvata: " << customFile << "\n\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Eccezione: " << e.what() << "\n\n";
    }
    
    // ========================================================================
    // Genera TUTTE LE CARTE STANDARD
    // ========================================================================
    
    std::cout << "📊 Generazione SET COMPLETO di carte...\n";
    
    std::string baseName = "event_" + event.eventId;
    int count = builder.generateAllCharts(baseName);
    
    std::cout << "✅ Generate " << count << " carte su 3\n\n";
    
    // ========================================================================
    // Riepilogo
    // ========================================================================
    
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    GENERAZIONE COMPLETATA                        ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "File generati:\n";
    std::cout << "  📄 " << approachFile << "  (campo largo, avvicinamento)\n";
    std::cout << "  📄 " << detailFile << "    (campo stretto, dettaglio)\n";
    std::cout << "  📄 " << customFile << "    (configurazione personalizzata)\n";
    std::cout << "  📄 " << baseName << "_approach.png\n";
    std::cout << "  📄 " << baseName << "_finder.png\n";
    std::cout << "  📄 " << baseName << "_detail.png\n\n";
    
    std::cout << "💡 Suggerimenti:\n";
    std::cout << "  • Usa la carta di avvicinamento per localizzare la regione\n";
    std::cout << "  • Usa la carta dettagliata per osservazione al telescopio\n";
    std::cout << "  • Verifica l'ora locale e le condizioni meteo\n";
    std::cout << "  • Registra i tempi con precisione al centesimo di secondo\n\n";
    
    return 0;
}
