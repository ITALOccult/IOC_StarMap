#ifndef STARMAP_OCCULTATION_CHART_BUILDER_H
#define STARMAP_OCCULTATION_CHART_BUILDER_H

#include "starmap/occultation/OccultationData.h"
#include "starmap/map/MapConfiguration.h"
#include "starmap/map/MapRenderer.h"
#include <memory>
#include <string>

namespace starmap {
namespace occultation {

/**
 * @brief Builder per la creazione di carte celesti per occultazioni asteroidali
 * 
 * Questa classe fornisce API ad alto livello per generare:
 * - Carte di avvicinamento (campo largo, per localizzare la regione)
 * - Carte dettagliate (campo stretto, per osservazione al telescopio)
 * - Carte personalizzate con configurazione specifica
 * 
 * Esempio d'uso:
 * @code
 * OccultationChartBuilder builder;
 * builder.fromJSON("event.json");
 * auto imageDetail = builder.generateDetailChart();
 * auto imageApproach = builder.generateApproachChart();
 * @endcode
 */
class OccultationChartBuilder {
public:
    OccultationChartBuilder();
    ~OccultationChartBuilder();

    /**
     * @brief Carica dati occultazione da JSON
     * @param jsonData Stringa JSON con i dati dell'evento
     * @return true se il parsing è riuscito
     */
    bool fromJSON(const std::string& jsonData);
    
    /**
     * @brief Carica dati occultazione da file JSON
     * @param jsonFilePath Path al file JSON
     * @return true se il caricamento è riuscito
     */
    bool fromJSONFile(const std::string& jsonFilePath);
    
    /**
     * @brief Imposta direttamente i dati dell'evento
     * @param event Dati dell'occultazione
     */
    void setEvent(const OccultationEvent& event);
    
    /**
     * @brief Ottiene i dati dell'evento corrente
     * @return Riferimento ai dati dell'evento
     */
    const OccultationEvent& getEvent() const;

    /**
     * @brief Genera carta di avvicinamento (campo largo)
     * 
     * Campo tipico: 10° x 10° o maggiore
     * Mostra il contesto generale e la posizione approssimativa
     * Ideale per localizzare la regione con binocolo o cercatore
     * 
     * @param config Configurazione (opzionale, usa default se omesso)
     * @return Buffer immagine generato
     */
    map::ImageBuffer generateApproachChart(
        const OccultationChartConfig* config = nullptr);

    /**
     * @brief Genera carta dettagliata (campo stretto)
     * 
     * Campo default: 3° x 1.5°
     * Mostra la regione in dettaglio con stelle fino a mag 15
     * Ideale per osservazione al telescopio
     * 
     * @param config Configurazione (opzionale, usa default se omesso)
     * @return Buffer immagine generato
     */
    map::ImageBuffer generateDetailChart(
        const OccultationChartConfig* config = nullptr);

    /**
     * @brief Genera carta cercatrice (campo medio)
     * 
     * Campo tipico: 5° x 5°
     * Via di mezzo tra approccio e dettaglio
     * 
     * @param config Configurazione (opzionale, usa default se omesso)
     * @return Buffer immagine generato
     */
    map::ImageBuffer generateFinderChart(
        const OccultationChartConfig* config = nullptr);

    /**
     * @brief Genera carta con configurazione personalizzata
     * @param config Configurazione completa
     * @return Buffer immagine generato
     */
    map::ImageBuffer generateCustomChart(
        const OccultationChartConfig& config);

    /**
     * @brief Salva carta su file
     * @param imageBuffer Buffer immagine da salvare
     * @param filename Nome file output
     * @param format Formato ("png", "svg", "pdf")
     * @return true se il salvataggio è riuscito
     */
    bool saveChart(const map::ImageBuffer& imageBuffer,
                   const std::string& filename,
                   const std::string& format = "png");

    /**
     * @brief Genera e salva carta di avvicinamento
     * @param filename Nome file output
     * @param config Configurazione (opzionale)
     * @return true se la generazione è riuscita
     */
    bool generateAndSaveApproachChart(
        const std::string& filename,
        const OccultationChartConfig* config = nullptr);

    /**
     * @brief Genera e salva carta dettagliata
     * @param filename Nome file output
     * @param config Configurazione (opzionale)
     * @return true se la generazione è riuscita
     */
    bool generateAndSaveDetailChart(
        const std::string& filename,
        const OccultationChartConfig* config = nullptr);

    /**
     * @brief Genera e salva carta cercatrice
     * @param filename Nome file output
     * @param config Configurazione (opzionale)
     * @return true se la generazione è riuscita
     */
    bool generateAndSaveFinderChart(
        const std::string& filename,
        const OccultationChartConfig* config = nullptr);

    /**
     * @brief Genera tutte le carte standard per un evento
     * 
     * Genera:
     * - Carta di avvicinamento (wide field)
     * - Carta cercatrice (finder)
     * - Carta dettagliata (detail)
     * 
     * @param baseFilename Nome base per i file (aggiungerà _approach, _finder, _detail)
     * @param format Formato file ("png", "svg", "pdf")
     * @return Numero di carte generate con successo
     */
    int generateAllCharts(const std::string& baseFilename,
                          const std::string& format = "png");

    /**
     * @brief Configura timeout per query ai cataloghi
     */
    void setCatalogTimeout(int seconds);
    
    /**
     * @brief Abilita/disabilita cache per i cataloghi
     */
    void setCacheEnabled(bool enabled);

    /**
     * @brief Imposta il livello di logging
     * @param level 0=none, 1=errors, 2=warnings, 3=info, 4=debug
     */
    void setLogLevel(int level);

    /**
     * @brief Valida i dati dell'evento
     * @return true se i dati sono validi e completi
     */
    bool validate() const;

    /**
     * @brief Ottiene messaggi di errore/warning dalla validazione
     * @return Lista di messaggi
     */
    std::vector<std::string> getValidationMessages() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Helper methods
    map::MapConfiguration createMapConfig(const OccultationChartConfig& chartConfig);
    void addAsteroidPath(map::MapRenderer& renderer, 
                         const OccultationChartConfig& chartConfig);
    void addTargetMarker(map::MapRenderer& renderer,
                         const OccultationChartConfig& chartConfig);
    void addInfoOverlay(map::MapRenderer& renderer,
                        const OccultationChartConfig& chartConfig);
    std::string generateAutoFilename(ChartType type) const;
    std::string generateTitle(ChartType type) const;
};

/**
 * @brief Funzioni di utilità per occultazioni
 */
namespace utils {

/**
 * @brief Converte formato JSON IOC in OccultationEvent
 * @param json Stringa JSON
 * @return Evento di occultazione
 * @throws std::runtime_error se il parsing fallisce
 */
OccultationEvent parseIOCJSON(const std::string& json);

/**
 * @brief Converte formato JSON IOTA in OccultationEvent
 * @param json Stringa JSON
 * @return Evento di occultazione
 */
OccultationEvent parseIOTAJSON(const std::string& json);

/**
 * @brief Calcola la posizione dell'asteroide a un dato tempo
 * @param event Evento occultazione
 * @param timeOffsetHours Offset in ore dal tempo dell'evento
 * @return Coordinate equatoriali
 */
core::EquatorialCoordinates calculateAsteroidPosition(
    const OccultationEvent& event,
    double timeOffsetHours);

/**
 * @brief Calcola il campo visivo ottimale per una carta
 * @param event Evento occultazione
 * @param type Tipo di carta
 * @return Coppia (larghezza, altezza) in gradi
 */
std::pair<double, double> calculateOptimalFieldOfView(
    const OccultationEvent& event,
    ChartType type);

/**
 * @brief Genera nome file automatico per l'evento
 * @param event Evento occultazione
 * @param type Tipo di carta
 * @param format Formato file
 * @return Nome file suggerito
 */
std::string generateFilename(
    const OccultationEvent& event,
    ChartType type,
    const std::string& format = "png");

} // namespace utils

} // namespace occultation
} // namespace starmap

#endif // STARMAP_OCCULTATION_CHART_BUILDER_H
