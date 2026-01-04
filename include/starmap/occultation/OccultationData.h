#ifndef STARMAP_OCCULTATION_DATA_H
#define STARMAP_OCCULTATION_DATA_H

#include "starmap/core/Coordinates.h"
#include <string>
#include <vector>
#include <optional>
#include <ctime>

namespace starmap {
namespace occultation {

/**
 * @brief Dati di una stella target per occultazione
 */
struct TargetStar {
    std::string catalogId;           // ID catalogo (es. "Gaia DR3 123456789")
    core::EquatorialCoordinates coordinates;
    double magnitude = 0.0;
    std::optional<double> bvColorIndex;
    std::optional<std::string> spectralType;
    std::optional<std::string> properName;
    std::optional<int> saoNumber;
};

/**
 * @brief Elementi orbitali dell'asteroide
 */
struct AsteroidOrbitalElements {
    double semiMajorAxis = 0.0;      // a (UA)
    double eccentricity = 0.0;       // e
    double inclination = 0.0;        // i (gradi)
    double longitudeAscendingNode = 0.0; // Ω (gradi)
    double argumentPerihelion = 0.0; // ω (gradi)
    double meanAnomaly = 0.0;        // M (gradi)
    double epoch = 0.0;              // JD
};

/**
 * @brief Dati dell'asteroide
 */
struct AsteroidData {
    std::string name;                // Nome/designazione (es. "(433) Eros")
    int number = 0;                  // Numero MPC (0 se non numerato)
    std::string designation;         // Designazione provvisoria
    
    // Posizione al momento dell'occultazione
    core::EquatorialCoordinates position;
    double magnitude = 0.0;
    double diameter = 0.0;           // km
    double velocity = 0.0;           // arcsec/hour
    double positionAngle = 0.0;      // gradi (direzione del moto)
    
    // Elementi orbitali (opzionali)
    std::optional<AsteroidOrbitalElements> orbitalElements;
};

/**
 * @brief Circostanze dell'occultazione
 */
struct OccultationCircumstances {
    std::string eventTime;           // ISO 8601 (es. "2025-12-15T22:34:12Z")
    double duration = 0.0;           // secondi (durata massima)
    double shadowWidth = 0.0;        // km (larghezza dell'ombra)
    double shadowSpeed = 0.0;        // km/s (velocità dell'ombra)
    double positionAngle = 0.0;      // gradi (direzione dell'ombra)
    
    // Incertezza
    double uncertaintyAlongTrack = 0.0;  // km
    double uncertaintyCrossTrack = 0.0;  // km
    
    // Drop di magnitudine atteso
    double magnitudeDrop = 0.0;
    
    // Probabilità
    double probability = 1.0;        // 0.0-1.0
};

/**
 * @brief Coordinate geografiche di un punto sulla traccia
 */
struct GeographicPoint {
    double latitude = 0.0;           // gradi
    double longitude = 0.0;          // gradi
    std::string locationName;        // Nome località (opzionale)
};

/**
 * @brief Traccia dell'ombra sulla Terra
 */
struct ShadowPath {
    std::vector<GeographicPoint> centerLine;    // Linea centrale
    std::vector<GeographicPoint> northernLimit; // Limite nord (1-sigma)
    std::vector<GeographicPoint> southernLimit; // Limite sud (1-sigma)
};

/**
 * @brief Dati di osservazione per una specifica località
 */
struct ObservationSite {
    std::string name;
    GeographicPoint location;
    double altitude = 0.0;           // metri s.l.m.
    
    // Coordinate locali al momento dell'evento
    double azimuth = 0.0;            // gradi
    double elevation = 0.0;          // gradi
    double airMass = 1.0;
    
    // Timing locale
    std::string localEventTime;      // Ora locale
    double moonDistance = 0.0;       // Distanza dalla Luna (gradi)
    double sunAltitude = 0.0;        // Altitudine del Sole (per crepuscolo)
};

/**
 * @brief Dati completi di un evento di occultazione asteroidale
 */
struct OccultationEvent {
    // Identificativo evento
    std::string eventId;             // ID univoco (es. "2025-12-15-433-Eros")
    std::string source;              // Fonte dati (es. "IOTA", "Occult", "JPL")
    std::string dataVersion;         // Versione previsione
    
    // Stella target
    TargetStar targetStar;
    
    // Asteroide occultante
    AsteroidData asteroid;
    
    // Circostanze
    OccultationCircumstances circumstances;
    
    // Traccia geografica
    std::optional<ShadowPath> shadowPath;
    
    // Sito di osservazione (opzionale, per carte personalizzate)
    std::optional<ObservationSite> observationSite;
    
    // Note e metadati
    std::string notes;
    std::vector<std::string> references; // URL o riferimenti
    
    // Validazione
    bool validate() const;
};

/**
 * @brief Tipo di carta da generare
 */
enum class ChartType {
    APPROACH,              // Carta di avvicinamento (campo largo)
    DETAIL,                // Carta dettagliata (campo stretto)
    FINDER,                // Carta cercatrice (campo medio)
    WIDE_FIELD,            // Campo molto ampio (overview)
    CUSTOM                 // Configurazione personalizzata
};

/**
 * @brief Configurazione per carta di occultazione
 */
struct OccultationChartConfig {
    ChartType chartType = ChartType::DETAIL;
    
    // Override campo visivo (0 = usa default per il tipo)
    double fieldOfViewWidth = 0.0;   // gradi (0 = auto)
    double fieldOfViewHeight = 0.0;  // gradi (0 = auto)
    
    // Dimensioni immagine
    int imageWidth = 3840;           // Default 4K
    int imageHeight = 2160;
    
    // Orientamento
    bool northUp = true;
    bool eastLeft = true;            // Standard astronomico
    double rotation = 0.0;           // Rotazione custom (gradi)
    
    // Visualizzazione
    double limitingMagnitude = 15.0; // Magnitudine limite
    bool showGrid = true;
    double gridInterval = 1.0;
    bool showConstellationLines = false;
    bool showConstellationBoundaries = false;
    bool showSAONumbers = false;
    bool showAsteroidPath = true;    // Mostra la traccia dell'asteroide
    bool showAsteroidPosition = true;// Evidenzia la posizione
    bool showTargetStar = true;      // Evidenzia la stella target
    bool showEventTime = true;       // Mostra data/ora evento
    bool showCircumstances = true;   // Mostra dettagli evento
    bool showScale = true;
    bool showCompass = true;
    
    // Traccia asteroide
    double pathDurationHours = 2.0;  // Ore prima/dopo evento
    int pathSteps = 60;              // Numero di punti sulla traccia
    
    // Stile
    uint32_t asteroidPathColor = 0xFF4444FF;   // Rosso
    uint32_t targetStarColor = 0x44FF44FF;     // Verde
    uint32_t gridColor = 0x404040FF;           // Grigio
    
    // Output
    std::string outputFilename;      // Se vuoto, genera automaticamente
    std::string format = "png";      // "png", "svg", "pdf"
    int quality = 95;                // Per formati con compressione
    
    // Titolo personalizzato
    std::string customTitle;         // Se vuoto, genera automaticamente
    std::string customSubtitle;
    
    // Ottenere i default per un tipo di carta
    static OccultationChartConfig getDefaultForType(ChartType type);
};

} // namespace occultation
} // namespace starmap

#endif // STARMAP_OCCULTATION_DATA_H
