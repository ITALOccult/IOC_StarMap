/**
 * @file ChartGenerator.h
 * @brief Generatore di carte stellari configurabile via JSON
 */

#pragma once

#include "starmap/core/Coordinates.h"
#include "starmap/core/CelestialObject.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>

namespace starmap {
namespace map {

/**
 * @brief Tipi di preset disponibili
 */
enum class ChartPreset {
    Default,      // Vista normale (cielo scuro)
    Visual,       // Per osservazione visuale (cielo scuro, colori)
    FinderChart,  // Carta di puntamento telescopio (stampabile, 20-30°)
    DetailChart,  // Carta di dettaglio telescopio (stampabile, 1-3°, fino mag 16)
    PrintBlack,   // Stampabile (stelle nere su sfondo bianco)
    PrintInverse  // Inversione colori per stampa
};

/**
 * @brief Configurazione stile della carta
 */
struct ChartStyle {
    // Colori (formato #RRGGBB)
    std::string backgroundColor = "#0a0a20";
    std::string gridColor = "#333355";
    std::string constellationLineColor = "#4488cc";
    std::string constellationBoundaryColor = "#555555";  // Confini costellazioni
    std::string starColor = "#ffffff";
    std::string labelColor = "#88aacc";
    std::string titleColor = "#ffffff";
    std::string borderColor = "#444444";
    
    // Font
    std::string fontFamily = "Arial";
    int titleFontSize = 26;
    int subtitleFontSize = 16;
    int labelFontSize = 12;
    int saoFontSize = 9;  // Dimensione numeri SAO/HD
    
    // Opacità (0.0 - 1.0)
    double gridOpacity = 0.5;
    double constellationLineOpacity = 0.6;
    double constellationBoundaryOpacity = 0.4;
    double starOpacity = 0.9;
    
    // Spessori linee
    double gridLineWidth = 0.5;
    double constellationLineWidth = 1.5;
    double constellationBoundaryWidth = 0.8;
    double borderWidth = 2.0;
    
    // Scala stelle
    double starSizeMultiplier = 1.0;
    double minStarSize = 0.5;
    double maxStarSize = 8.0;
    
    // Colori stelle per temperatura (B-V color index)
    bool useStarColors = true;
    std::string colorBlue = "#aaccff";
    std::string colorWhite = "#ffffff";
    std::string colorYellow = "#ffeecc";
    std::string colorOrange = "#ffaa77";
    std::string colorRed = "#ff6644";
    
    // Stile stampabile
    bool printable = false;  // Inverti colori per stampa
};

/**
 * @brief Configurazione della carta
 */
struct ChartConfig {
    // Dimensioni output
    int width = 800;
    int height = 800;
    
    // Centro e campo visivo
    double centerRA = 0.0;      // Ascensione retta (gradi)
    double centerDec = 0.0;     // Declinazione (gradi)
    double fieldRadius = 15.0;  // Raggio campo visivo (gradi)
    
    // Filtri
    double maxMagnitude = 6.5;
    double minMagnitude = -2.0;
    
    // Titoli
    std::string title;
    std::string subtitle;
    
    // Elementi da mostrare
    bool showGrid = true;
    bool showConstellationLines = true;
    bool showConstellationBoundaries = false;  // Confini IAU
    bool showStarLabels = true;
    bool showSAONumbers = false;   // Numeri SAO per stelle luminose
    bool showHDNumbers = false;    // Numeri HD
    bool showLegend = true;
    bool showCoordinateLabels = true;
    bool showBorder = false;       // Bordo carta
    bool showScaleBar = false;     // Scala angolare
    bool showNorthArrow = false;   // Freccia nord
    
    double labelMagnitudeLimit = 4.0;  // Etichette solo per stelle più luminose
    double saoMagnitudeLimit = 6.0;    // SAO/HD fino a questa magnitudine
    
    // Target Box
    double targetBoxSize = 0.0;        // Dimensione del riquadro target in gradi (0.0 = disabilitato)
    
    // Intervallo griglia (gradi)
    double gridInterval = 5.0;      // Intervallo Declinazione
    double gridIntervalRA = 0.0;    // Intervallo RA (0 = auto, basato su gridInterval)
    
    // Proiezione
    std::string projection = "stereographic";  // stereographic, gnomonic, orthographic
    
    // Output
    std::string outputFormat = "svg";  // svg, png, jpg, pdf
    std::string outputPath = "star_chart";
    int pngDensity = 150;  // DPI per PNG/JPG/PDF
    
    // Stile
    ChartStyle style;
    
    // Costellazione (opzionale)
    std::string constellation;  // Nome costellazione (es. "Leo", "Orion")
    
    // Preset
    ChartPreset preset = ChartPreset::Default;
};

/**
 * @brief Linea di costellazione
 */
struct ConstellationLine {
    double ra1, dec1;
    double ra2, dec2;
};

/**
 * @brief Dati costellazione
 */
struct ConstellationData {
    std::string name;
    std::string abbreviation;
    double centerRA;
    double centerDec;
    std::vector<ConstellationLine> lines;
    std::map<std::string, std::pair<double, double>> namedStars;  // nome -> (ra, dec)
};

/**
 * @brief Confine di una costellazione (segmenti IAU)
 */
struct ConstellationBoundary {
    std::string abbreviation;
    std::vector<ConstellationLine> segments;
};

/**
 * @brief Generatore di carte stellari
 */
class ChartGenerator {
public:
    ChartGenerator();
    ~ChartGenerator();
    
    /**
     * @brief Carica configurazione da file JSON
     * @param jsonPath Percorso file JSON
     * @return true se caricato con successo
     */
    bool loadConfig(const std::string& jsonPath);
    
    /**
     * @brief Carica configurazione da stringa JSON
     * @param jsonString Stringa JSON
     * @return true se caricato con successo
     */
    bool loadConfigFromString(const std::string& jsonString);
    
    /**
     * @brief Imposta configurazione programmaticamente
     */
    void setConfig(const ChartConfig& config);
    
    /**
     * @brief Ottiene la configurazione corrente
     */
    const ChartConfig& getConfig() const { return config_; }
    
    /**
     * @brief Genera la carta
     * @return true se generata con successo
     */
    bool generate();
    
    /**
     * @brief Genera carta con configurazione specifica
     */
    bool generate(const ChartConfig& config);
    
    /**
     * @brief Ottiene l'ultimo errore
     */
    const std::string& getLastError() const { return lastError_; }
    
    /**
     * @brief Ottiene il percorso del file generato
     */
    const std::string& getOutputPath() const { return outputPath_; }
    
    /**
     * @brief Carica dati costellazione predefiniti
     */
    static std::optional<ConstellationData> getConstellationData(const std::string& name);
    
    /**
     * @brief Lista delle costellazioni disponibili
     */
    static std::vector<std::string> getAvailableConstellations();
    
    /**
     * @brief Applica un preset alla configurazione
     * @param preset Tipo di preset
     */
    void applyPreset(ChartPreset preset);
    
    /**
     * @brief Crea configurazione per carta di puntamento (finder chart)
     * @param ra Ascensione retta centro (gradi)
     * @param dec Declinazione centro (gradi)
     * @param fieldDegrees Campo visivo (default 25°)
     * @return Configurazione pronta
     */
    static ChartConfig createFinderChart(double ra, double dec, double fieldDegrees = 25.0);
    
    /**
     * @brief Crea configurazione per carta di dettaglio (deep field)
     * @param ra Ascensione retta centro (gradi)
     * @param dec Declinazione centro (gradi)
     * @param fieldDegrees Campo visivo (default 2°)
     * @param maxMag Magnitudine limite (default 16)
     * @return Configurazione pronta
     */
    static ChartConfig createDetailChart(double ra, double dec, 
                                          double fieldDegrees = 2.0, 
                                          double maxMag = 16.0);
    
    /**
     * @brief Lista dei preset disponibili
     */
    static std::vector<std::string> getAvailablePresets();

private:
    ChartConfig config_;
    std::string lastError_;
    std::string outputPath_;
    
    // Stelle caricate
    std::vector<std::shared_ptr<core::Star>> stars_;
    
    // Metodi interni
    bool loadStars();
    void loadBrightStarsFromDatabase();
    bool generateSVG(const std::string& path);
    bool convertToPNG(const std::string& svgPath, const std::string& pngPath);
    bool convertToJPG(const std::string& svgPath, const std::string& jpgPath);
    
    std::pair<double, double> projectToSVG(double ra, double dec) const;
    std::string getStarColor(double colorIndex) const;
    double getStarRadius(double magnitude) const;
    
    bool parseJSON(const std::string& json);
};

} // namespace map
} // namespace starmap
