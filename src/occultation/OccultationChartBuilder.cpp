#include "starmap/occultation/OccultationChartBuilder.h"
#include "starmap/catalog/CatalogManager.h"
#include "starmap/map/ChartGenerator.h"
#include "starmap/core/CelestialObject.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace starmap {
namespace occultation {

// ============================================================================
// Implementation class
// ============================================================================

class OccultationChartBuilder::Impl {
public:
    OccultationEvent event;
    catalog::CatalogManager catalogManager;
    int logLevel = 2; // warnings
    std::vector<std::string> validationMessages;
    
    bool hasEvent = false;
};

// ============================================================================
// Constructor / Destructor
// ============================================================================

OccultationChartBuilder::OccultationChartBuilder()
    : pImpl_(std::make_unique<Impl>()) {
}

OccultationChartBuilder::~OccultationChartBuilder() = default;

// ============================================================================
// Data loading
// ============================================================================

bool OccultationChartBuilder::fromJSON(const std::string& jsonData) {
    try {
        pImpl_->event = utils::parseIOCJSON(jsonData);
        pImpl_->hasEvent = true;
        return validate();
    } catch (const std::exception& e) {
        if (pImpl_->logLevel >= 1) {
            pImpl_->validationMessages.push_back(
                std::string("JSON parsing error: ") + e.what());
        }
        return false;
    }
}

bool OccultationChartBuilder::fromJSONFile(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        if (pImpl_->logLevel >= 1) {
            pImpl_->validationMessages.push_back(
                "Cannot open file: " + jsonFilePath);
        }
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return fromJSON(buffer.str());
}

void OccultationChartBuilder::setEvent(const OccultationEvent& event) {
    pImpl_->event = event;
    pImpl_->hasEvent = true;
}

const OccultationEvent& OccultationChartBuilder::getEvent() const {
    return pImpl_->event;
}

// ============================================================================
// Chart generation
// ============================================================================

map::ImageBuffer OccultationChartBuilder::generateApproachChart(
    const OccultationChartConfig* config) {
    
    OccultationChartConfig defaultConfig = 
        OccultationChartConfig::getDefaultForType(ChartType::APPROACH);
    
    const OccultationChartConfig& finalConfig = 
        config ? *config : defaultConfig;
    
    return generateCustomChart(finalConfig);
}

map::ImageBuffer OccultationChartBuilder::generateDetailChart(
    const OccultationChartConfig* config) {
    
    OccultationChartConfig defaultConfig = 
        OccultationChartConfig::getDefaultForType(ChartType::DETAIL);
    
    const OccultationChartConfig& finalConfig = 
        config ? *config : defaultConfig;
    
    return generateCustomChart(finalConfig);
}

map::ImageBuffer OccultationChartBuilder::generateFinderChart(
    const OccultationChartConfig* config) {
    
    OccultationChartConfig defaultConfig = 
        OccultationChartConfig::getDefaultForType(ChartType::FINDER);
    
    const OccultationChartConfig& finalConfig = 
        config ? *config : defaultConfig;
    
    return generateCustomChart(finalConfig);
}

map::ImageBuffer OccultationChartBuilder::generateCustomChart(
    const OccultationChartConfig& chartConfig) {
    
    if (!pImpl_->hasEvent) {
        throw std::runtime_error("No occultation event data loaded");
    }
    
    // Crea configurazione mappa
    map::MapConfiguration mapConfig = createMapConfig(chartConfig);
    
    // Crea renderer
    map::MapRenderer renderer(mapConfig);
    
    // Query stelle dal catalogo
    catalog::GaiaQueryParameters params;
    params.center = pImpl_->event.targetStar.coordinates;
    params.radiusDegrees = std::max(chartConfig.fieldOfViewWidth, 
                                     chartConfig.fieldOfViewHeight) * 0.6;
    params.maxMagnitude = chartConfig.limitingMagnitude;
    
    auto stars = pImpl_->catalogManager.queryGaia(params);
    
    // Aggiungi stelle al renderer
    for (const auto& star : stars) {
        renderer.addStar(star);
    }
    
    // Aggiungi traccia asteroide
    if (chartConfig.showAsteroidPath) {
        addAsteroidPath(renderer, chartConfig);
    }
    
    // Aggiungi marker per stella target
    if (chartConfig.showTargetStar) {
        addTargetMarker(renderer, chartConfig);
    }
    
    // Aggiungi overlay con informazioni
    if (chartConfig.showCircumstances) {
        addInfoOverlay(renderer, chartConfig);
    }
    
    // Render finale
    return renderer.render();
}

// ============================================================================
// Save operations
// ============================================================================

bool OccultationChartBuilder::saveChart(
    const map::ImageBuffer& imageBuffer,
    const std::string& filename,
    const std::string& format) {
    
    // TODO: Implementare salvataggio basato sul formato
    // Per ora salva sempre come PNG
    return imageBuffer.saveToPNG(filename);
}

bool OccultationChartBuilder::generateAndSaveApproachChart(
    const std::string& filename,
    const OccultationChartConfig* config) {
    
    try {
        auto buffer = generateApproachChart(config);
        return saveChart(buffer, filename);
    } catch (const std::exception& e) {
        if (pImpl_->logLevel >= 1) {
            pImpl_->validationMessages.push_back(
                std::string("Error generating approach chart: ") + e.what());
        }
        return false;
    }
}

bool OccultationChartBuilder::generateAndSaveDetailChart(
    const std::string& filename,
    const OccultationChartConfig* config) {
    
    try {
        auto buffer = generateDetailChart(config);
        return saveChart(buffer, filename);
    } catch (const std::exception& e) {
        if (pImpl_->logLevel >= 1) {
            pImpl_->validationMessages.push_back(
                std::string("Error generating detail chart: ") + e.what());
        }
        return false;
    }
}

int OccultationChartBuilder::generateAllCharts(
    const std::string& baseFilename,
    const std::string& format) {
    
    int successCount = 0;
    
    // Carta di avvicinamento
    std::string approachFile = baseFilename + "_approach." + format;
    if (generateAndSaveApproachChart(approachFile)) {
        successCount++;
    }
    
    // Carta cercatrice
    std::string finderFile = baseFilename + "_finder." + format;
    if (generateAndSaveFinderChart(finderFile)) {
        successCount++;
    }
    
    // Carta dettagliata
    std::string detailFile = baseFilename + "_detail." + format;
    if (generateAndSaveDetailChart(detailFile)) {
        successCount++;
    }
    
    return successCount;
}

// ============================================================================
// Configuration
// ============================================================================

void OccultationChartBuilder::setCatalogTimeout(int seconds) {
    pImpl_->catalogManager.setTimeout(seconds);
}

void OccultationChartBuilder::setCacheEnabled(bool enabled) {
    pImpl_->catalogManager.setCacheEnabled(enabled);
}

void OccultationChartBuilder::setLogLevel(int level) {
    pImpl_->logLevel = level;
}

// ============================================================================
// Validation
// ============================================================================

bool OccultationChartBuilder::validate() const {
    if (!pImpl_->hasEvent) {
        return false;
    }
    return pImpl_->event.validate();
}

std::vector<std::string> OccultationChartBuilder::getValidationMessages() const {
    return pImpl_->validationMessages;
}

// ============================================================================
// Private helper methods
// ============================================================================

map::MapConfiguration OccultationChartBuilder::createMapConfig(
    const OccultationChartConfig& chartConfig) {
    
    map::MapConfiguration mapConfig;
    
    // Centro sulla stella target
    mapConfig.center = pImpl_->event.targetStar.coordinates;
    
    // Campo visivo
    mapConfig.fieldOfViewWidth = chartConfig.fieldOfViewWidth;
    mapConfig.fieldOfViewHeight = chartConfig.fieldOfViewHeight;
    
    // Dimensioni immagine
    mapConfig.imageWidth = chartConfig.imageWidth;
    mapConfig.imageHeight = chartConfig.imageHeight;
    
    // Magnitudine limite
    mapConfig.limitingMagnitude = chartConfig.limitingMagnitude;
    
    // Orientamento
    mapConfig.northUp = chartConfig.northUp;
    mapConfig.eastLeft = chartConfig.eastLeft;
    mapConfig.rotationAngle = chartConfig.rotation;
    
    // Griglia
    mapConfig.gridStyle.enabled = chartConfig.showGrid;
    mapConfig.gridStyle.color = chartConfig.gridColor;
    
    // Titolo
    if (!chartConfig.customTitle.empty()) {
        mapConfig.title = chartConfig.customTitle;
    } else {
        mapConfig.title = generateTitle(chartConfig.chartType);
    }
    
    // Altri elementi
    mapConfig.showScale = chartConfig.showScale;
    mapConfig.showCompass = chartConfig.showCompass;
    
    return mapConfig;
}

void OccultationChartBuilder::addAsteroidPath(
    map::MapRenderer& renderer,
    const OccultationChartConfig& chartConfig) {
    
    // Calcola posizioni dell'asteroide lungo la traccia
    double duration = chartConfig.pathDurationHours;
    int steps = chartConfig.pathSteps;
    
    std::vector<core::EquatorialCoordinates> pathPoints;
    
    for (int i = 0; i < steps; i++) {
        double t = -duration + (2.0 * duration * i) / (steps - 1);
        auto pos = utils::calculateAsteroidPosition(pImpl_->event, t);
        pathPoints.push_back(pos);
    }
    
    // Disegna la traccia
    // TODO: Implementare renderer.addPath() o simile
    // renderer.addPath(pathPoints, chartConfig.asteroidPathColor);
}

void OccultationChartBuilder::addTargetMarker(
    map::MapRenderer& renderer,
    const OccultationChartConfig& chartConfig) {
    
    // Evidenzia la stella target
    const auto& star = pImpl_->event.targetStar;
    
    // TODO: Implementare renderer.addMarker() o simile
    // renderer.addMarker(star.coordinates, chartConfig.targetStarColor, "TARGET");
}

void OccultationChartBuilder::addInfoOverlay(
    map::MapRenderer& renderer,
    const OccultationChartConfig& chartConfig) {
    
    // Crea overlay con informazioni sull'evento
    std::ostringstream info;
    
    info << "Evento: " << pImpl_->event.eventId << "\n";
    info << "Asteroide: " << pImpl_->event.asteroid.name << "\n";
    info << "Stella: " << pImpl_->event.targetStar.catalogId << "\n";
    info << "Data: " << pImpl_->event.circumstances.eventTime << "\n";
    info << "Durata: " << std::fixed << std::setprecision(1) 
         << pImpl_->event.circumstances.duration << " s\n";
    info << "Drop mag: " << std::setprecision(2)
         << pImpl_->event.circumstances.magnitudeDrop;
    
    // TODO: Implementare renderer.addTextOverlay() o simile
    // renderer.addTextOverlay(info.str(), position, style);
}

std::string OccultationChartBuilder::generateAutoFilename(ChartType type) const {
    return utils::generateFilename(pImpl_->event, type);
}

std::string OccultationChartBuilder::generateTitle(ChartType type) const {
    std::ostringstream title;
    
    title << "Occultazione ";
    
    switch (type) {
        case ChartType::APPROACH:
            title << "- Carta di Avvicinamento";
            break;
        case ChartType::DETAIL:
            title << "- Carta Dettagliata";
            break;
        case ChartType::FINDER:
            title << "- Carta Cercatrice";
            break;
        case ChartType::WIDE_FIELD:
            title << "- Campo Ampio";
            break;
        case ChartType::CUSTOM:
            title << "- Configurazione Custom";
            break;
    }
    
    title << "\n" << pImpl_->event.asteroid.name 
          << " occulta " << pImpl_->event.targetStar.catalogId;
    
    return title.str();
}

// ============================================================================
// Utility functions
// ============================================================================

namespace utils {

OccultationEvent parseIOCJSON(const std::string& jsonStr) {
    json j = json::parse(jsonStr);
    
    OccultationEvent event;
    
    // Parse basic info
    event.eventId = j.value("event_id", "");
    event.source = j.value("source", "IOC");
    
    // Parse target star
    if (j.contains("target_star")) {
        auto& star = j["target_star"];
        event.targetStar.catalogId = star.value("catalog_id", "");
        
        if (star.contains("ra") && star.contains("dec")) {
            event.targetStar.coordinates = core::EquatorialCoordinates(
                star["ra"].get<double>(),
                star["dec"].get<double>()
            );
        }
        
        event.targetStar.magnitude = star.value("magnitude", 0.0);
        
        if (star.contains("bv_color")) {
            event.targetStar.bvColorIndex = star["bv_color"].get<double>();
        }
        if (star.contains("sao_number")) {
            event.targetStar.saoNumber = star["sao_number"].get<int>();
        }
    }
    
    // Parse asteroid
    if (j.contains("asteroid")) {
        auto& ast = j["asteroid"];
        event.asteroid.name = ast.value("name", "");
        event.asteroid.number = ast.value("number", 0);
        event.asteroid.magnitude = ast.value("magnitude", 0.0);
        event.asteroid.diameter = ast.value("diameter_km", 0.0);
        
        if (ast.contains("ra") && ast.contains("dec")) {
            event.asteroid.position = core::EquatorialCoordinates(
                ast["ra"].get<double>(),
                ast["dec"].get<double>()
            );
        }
        
        event.asteroid.velocity = ast.value("velocity_arcsec_per_hour", 0.0);
        event.asteroid.positionAngle = ast.value("position_angle", 0.0);
    }
    
    // Parse circumstances
    if (j.contains("circumstances")) {
        auto& circ = j["circumstances"];
        event.circumstances.eventTime = circ.value("event_time", "");
        event.circumstances.duration = circ.value("duration_seconds", 0.0);
        event.circumstances.shadowWidth = circ.value("shadow_width_km", 0.0);
        event.circumstances.magnitudeDrop = circ.value("magnitude_drop", 0.0);
        event.circumstances.probability = circ.value("probability", 1.0);
    }
    
    // Parse observation site (if present)
    if (j.contains("observation_site")) {
        ObservationSite site;
        auto& siteData = j["observation_site"];
        
        site.name = siteData.value("name", "");
        site.location.latitude = siteData.value("latitude", 0.0);
        site.location.longitude = siteData.value("longitude", 0.0);
        site.altitude = siteData.value("altitude_m", 0.0);
        
        event.observationSite = site;
    }
    
    return event;
}

OccultationEvent parseIOTAJSON(const std::string& jsonStr) {
    // TODO: Implementare parsing formato IOTA
    // Per ora usa lo stesso parser IOC
    return parseIOCJSON(jsonStr);
}

core::EquatorialCoordinates calculateAsteroidPosition(
    const OccultationEvent& event,
    double timeOffsetHours) {
    
    // Calcolo semplificato: moto lineare uniforme
    // Per calcoli più precisi si potrebbero usare elementi orbitali
    
    double ra = event.asteroid.position.getRightAscension();
    double dec = event.asteroid.position.getDeclination();
    
    // Velocità in gradi/ora
    double velocityDegPerHour = event.asteroid.velocity / 3600.0;
    double pa = event.asteroid.positionAngle * M_PI / 180.0;
    
    // Componenti del moto
    double dRA = velocityDegPerHour * timeOffsetHours * sin(pa);
    double dDec = velocityDegPerHour * timeOffsetHours * cos(pa);
    
    // Correzione per la declinazione (RA varia più velocemente vicino ai poli)
    dRA /= cos(dec * M_PI / 180.0);
    
    return core::EquatorialCoordinates(ra + dRA, dec + dDec);
}

std::pair<double, double> calculateOptimalFieldOfView(
    const OccultationEvent& event,
    ChartType type) {
    
    auto config = OccultationChartConfig::getDefaultForType(type);
    return {config.fieldOfViewWidth, config.fieldOfViewHeight};
}

std::string generateFilename(
    const OccultationEvent& event,
    ChartType type,
    const std::string& format) {
    
    std::ostringstream filename;
    
    // Estrai data dall'event time (formato ISO)
    std::string dateStr = event.circumstances.eventTime.substr(0, 10);
    std::replace(dateStr.begin(), dateStr.end(), '-', '_');
    
    filename << dateStr << "_";
    filename << event.asteroid.name << "_";
    
    switch (type) {
        case ChartType::APPROACH:
            filename << "approach";
            break;
        case ChartType::DETAIL:
            filename << "detail";
            break;
        case ChartType::FINDER:
            filename << "finder";
            break;
        case ChartType::WIDE_FIELD:
            filename << "wide";
            break;
        case ChartType::CUSTOM:
            filename << "custom";
            break;
    }
    
    filename << "." << format;
    
    return filename.str();
}

} // namespace utils

} // namespace occultation
} // namespace starmap
