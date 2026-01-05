#ifndef STARMAP_MAP_CONFIGURATION_H
#define STARMAP_MAP_CONFIGURATION_H

#include "starmap/core/Coordinates.h"
#include <string>
#include <vector>
#include <cstdint>

namespace starmap {
namespace map {

/**
 * @brief Tipo di proiezione per la mappa
 */
enum class ProjectionType {
    STEREOGRAPHIC,      // Proiezione stereografica (standard per mappe celesti)
    GNOMONIC,          // Proiezione gnomica (tangenziale)
    ORTHOGRAPHIC,      // Proiezione ortografica
    MERCATOR,          // Proiezione di Mercatore
    AZIMUTHAL_EQUIDISTANT  // Proiezione azimutale equidistante
};

/**
 * @brief Sistema di coordinate per la mappa
 */
enum class CoordinateSystem {
    EQUATORIAL,        // RA/Dec
    GALACTIC,          // l/b
    ECLIPTIC,          // Coordinate eclittiche
    HORIZONTAL         // Alt/Az
};

/**
 * @brief Stile della griglia di coordinate
 */
struct GridStyle {
    bool enabled = true;
    double raStepDegrees = 15.0;      // Step in RA (1h = 15°)
    double decStepDegrees = 10.0;     // Step in Dec
    uint32_t color = 0x404040FF;      // Grigio scuro, formato RGBA
    float lineWidth = 0.5f;
    bool showLabels = true;
    uint32_t labelColor = 0xFFFFFFFF; // Bianco
    float labelFontSize = 10.0f;
};

/**
 * @brief Stile per il rendering delle stelle
 */
struct StarStyle {
    // Mapping magnitudine -> dimensione simbolo
    float minSymbolSize = 0.5f;    // Per stelle deboli
    float maxSymbolSize = 8.0f;    // Per stelle luminose
    float magnitudeRange = 10.0f;  // Range di magnitudine
    
    // Colori basati su temperatura/spettro
    bool useSpectralColors = true;
    uint32_t defaultColor = 0xFFFFFFFF; // Bianco default
    
    // Simboli speciali
    bool useCircles = true;
    bool useCrosses = false;       // Per stelle variabili
    bool useDiamonds = false;      // Per doppie
    
    // Labels
    bool showNames = true;
    bool showSAONumbers = true;
    bool showMagnitudes = false;
    float minMagnitudeForLabel = 4.0f; // Mostra label solo per mag < 4
    uint32_t labelColor = 0xFFFFFFFF;
    float labelFontSize = 8.0f;
};

/**
 * @brief Posizione della scala magnitudini sulla mappa
 */
enum class LegendPosition {
    NONE,           // Non mostrare
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    CUSTOM          // Usa coordinate custom
};

/**
 * @brief Configurazione scala magnitudini
 */
struct MagnitudeLegendStyle {
    bool enabled = false;
    LegendPosition position = LegendPosition::BOTTOM_RIGHT;
    float customX = 0.0f;  // Posizione custom (0-1, normalizzato)
    float customY = 0.0f;  // Posizione custom (0-1, normalizzato)
    float fontSize = 10.0f;
    uint32_t textColor = 0xFFFFFFFF;
    uint32_t backgroundColor = 0x000000CC; // Semi-trasparente
    bool showBackground = true;
};

/**
 * @brief Stile del titolo
 */
struct TitleStyle {
    bool enabled = true;
    std::string text;
    LegendPosition position = LegendPosition::TOP_LEFT;
    float customX = 0.0f;  // Posizione custom (0-1, normalizzato)
    float customY = 0.0f;  // Posizione custom (0-1, normalizzato)
    float fontSize = 18.0f;
    uint32_t textColor = 0xFFFFFFFF;
    uint32_t backgroundColor = 0x00000000; // Trasparente di default
    bool showBackground = false;
    float paddingX = 10.0f;  // Padding orizzontale
    float paddingY = 10.0f;  // Padding verticale
};

/**
 * @brief Stile del bordo
 */
struct BorderStyle {
    bool enabled = true;
    uint32_t color = 0xFFFFFFFF;  // Bianco
    float width = 2.0f;            // Spessore in pixel
    float margin = 0.0f;           // Margine interno
};

/**
 * @brief Stile della bussola
 */
struct CompassStyle {
    bool enabled = true;
    LegendPosition position = LegendPosition::TOP_RIGHT;
    float customX = 0.0f;  // Posizione custom (0-1, normalizzato)
    float customY = 0.0f;  // Posizione custom (0-1, normalizzato)
    float size = 80.0f;    // Dimensione in pixel
    uint32_t northColor = 0xFF0000FF;  // Rosso per Nord
    uint32_t eastColor = 0x00FF00FF;   // Verde per Est
    uint32_t lineColor = 0xFFFFFFFF;   // Bianco per le linee
    float lineWidth = 2.0f;
    bool showLabels = true;
    float labelFontSize = 10.0f;
};

/**
 * @brief Stile della scala
 */
struct ScaleStyle {
    bool enabled = true;
    LegendPosition position = LegendPosition::BOTTOM_LEFT;
    float customX = 0.0f;  // Posizione custom (0-1, normalizzato)
    float customY = 0.0f;  // Posizione custom (0-1, normalizzato)
    float length = 100.0f; // Lunghezza in pixel
    uint32_t color = 0xFFFFFFFF;
    float lineWidth = 2.0f;
    float fontSize = 10.0f;
    bool showText = true;  // Mostra il valore in gradi/arcmin
};

/**
 * @brief Rettangolo da evidenziare sulla mappa
 */
struct OverlayRectangle {
    bool enabled = false;
    double centerRA = 0.0;      // Centro in ascensione retta (gradi)
    double centerDec = 0.0;     // Centro in declinazione (gradi)
    double widthRA = 0.0;       // Larghezza in AR (gradi)
    double heightDec = 0.0;     // Altezza in Dec (gradi)
    uint32_t color = 0xFF0000FF; // Rosso default
    float lineWidth = 2.0f;
    bool filled = false;
    uint32_t fillColor = 0xFF000033; // Rosso semi-trasparente
    std::string label;          // Etichetta opzionale
};

/**
 * @brief Punto di un percorso sulla mappa
 */
struct PathPoint {
    double ra = 0.0;   // Ascensione retta (gradi)
    double dec = 0.0;  // Declinazione (gradi)
    std::string label; // Etichetta opzionale per questo punto
};

/**
 * @brief Percorso/traccia da visualizzare sulla mappa
 */
struct OverlayPath {
    bool enabled = false;
    std::vector<PathPoint> points;
    uint32_t color = 0x00FF00FF; // Verde default
    float lineWidth = 2.0f;
    bool showPoints = true;      // Mostra i punti lungo il percorso
    float pointSize = 3.0f;
    bool showLabels = false;     // Mostra le etichette dei punti
    bool showDirection = false;  // Mostra frecce di direzione
    std::string label;           // Etichetta generale del percorso
};

/**
 * @brief Configurazione completa per una mappa celeste
 */
class MapConfiguration {
public:
    MapConfiguration();
    ~MapConfiguration() = default;

    // Centro della mappa
    core::EquatorialCoordinates center;
    
    // Dimensioni del campo di vista
    double fieldOfViewWidth = 10.0;   // In gradi
    double fieldOfViewHeight = 10.0;  // In gradi
    
    // Dimensioni output (immagine)
    int imageWidth = 1920;
    int imageHeight = 1080;
    
    // Proiezione
    ProjectionType projection = ProjectionType::STEREOGRAPHIC;
    CoordinateSystem coordinateSystem = CoordinateSystem::EQUATORIAL;
    
    // Magnitudine limite
    double limitingMagnitude = 12.0;
    
    // Limite stelle per ottimizzare memoria
    int maxStars = 50000;  // Numero massimo di stelle da caricare
    int starBatchSize = 5000;  // Dimensione batch per rendering progressivo
    
    // Orientamento
    double rotationAngle = 0.0;  // Rotazione in gradi
    bool northUp = true;         // Nord in alto
    bool eastLeft = true;        // Est a sinistra (standard astronomico)
    
    // Stili
    GridStyle gridStyle;
    StarStyle starStyle;
    MagnitudeLegendStyle magnitudeLegend;
    TitleStyle titleStyle;
    BorderStyle borderStyle;
    CompassStyle compassStyle;
    ScaleStyle scaleStyle;
    
    // Overlay personalizzati
    std::vector<OverlayRectangle> overlayRectangles;
    std::vector<OverlayPath> overlayPaths;
    
    // Background
    uint32_t backgroundColor = 0x000000FF; // Nero
    
    // Deprecated: usa invece gli Style objects sopra
    bool showBorder = true;  // Usa borderStyle.enabled
    bool showTitle = true;   // Usa titleStyle.enabled
    std::string title;       // Usa titleStyle.text
    bool showScale = true;   // Usa scaleStyle.enabled
    bool showLegend = false; // Usa magnitudeLegend.enabled
    bool showCompass = true; // Usa compassStyle.enabled
    
    // Overlay aggiuntivi
    bool showConstellationLines = false;
    bool showConstellationBoundaries = false;
    bool showConstellationNames = false;
    bool showMilkyWay = false;
    bool showEcliptic = false;
    bool showEquator = false;
    
    // Data e ora (per posizioni dipendenti dal tempo)
    bool useObservationTime = false;
    std::string observationTime; // ISO 8601 format
    
    // Observer location (per coordinate orizzontali)
    double observerLatitude = 0.0;
    double observerLongitude = 0.0;
    
    // Validazione
    bool validate() const;
    
    // Clona configurazione
    MapConfiguration clone() const;
};

} // namespace map
} // namespace starmap

#endif // STARMAP_MAP_CONFIGURATION_H
