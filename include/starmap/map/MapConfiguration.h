#ifndef STARMAP_MAP_CONFIGURATION_H
#define STARMAP_MAP_CONFIGURATION_H

#include "starmap/core/Coordinates.h"
#include <string>
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
    
    // Orientamento
    double rotationAngle = 0.0;  // Rotazione in gradi
    bool northUp = true;         // Nord in alto
    bool eastLeft = true;        // Est a sinistra (standard astronomico)
    
    // Stili
    GridStyle gridStyle;
    StarStyle starStyle;
    
    // Background
    uint32_t backgroundColor = 0x000000FF; // Nero
    
    // Bordi e titoli
    bool showBorder = true;
    bool showTitle = true;
    std::string title;
    bool showScale = true;
    bool showLegend = false;
    bool showCompass = true;
    
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
