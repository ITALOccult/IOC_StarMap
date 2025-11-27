#include "starmap/config/JSONConfigLoader.h"
#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace starmap {
namespace config {

std::string JSONConfigLoader::projectionTypeToString(map::ProjectionType type) {
    switch (type) {
        case map::ProjectionType::STEREOGRAPHIC: return "stereographic";
        case map::ProjectionType::GNOMONIC: return "gnomonic";
        case map::ProjectionType::ORTHOGRAPHIC: return "orthographic";
        case map::ProjectionType::MERCATOR: return "mercator";
        case map::ProjectionType::AZIMUTHAL_EQUIDISTANT: return "azimuthal_equidistant";
        default: return "stereographic";
    }
}

map::ProjectionType JSONConfigLoader::stringToProjectionType(const std::string& str) {
    if (str == "gnomonic") return map::ProjectionType::GNOMONIC;
    if (str == "orthographic") return map::ProjectionType::ORTHOGRAPHIC;
    if (str == "mercator") return map::ProjectionType::MERCATOR;
    if (str == "azimuthal_equidistant") return map::ProjectionType::AZIMUTHAL_EQUIDISTANT;
    return map::ProjectionType::STEREOGRAPHIC;
}

std::string JSONConfigLoader::coordinateSystemToString(map::CoordinateSystem sys) {
    switch (sys) {
        case map::CoordinateSystem::EQUATORIAL: return "equatorial";
        case map::CoordinateSystem::GALACTIC: return "galactic";
        case map::CoordinateSystem::ECLIPTIC: return "ecliptic";
        case map::CoordinateSystem::HORIZONTAL: return "horizontal";
        default: return "equatorial";
    }
}

map::CoordinateSystem JSONConfigLoader::stringToCoordinateSystem(const std::string& str) {
    if (str == "galactic") return map::CoordinateSystem::GALACTIC;
    if (str == "ecliptic") return map::CoordinateSystem::ECLIPTIC;
    if (str == "horizontal") return map::CoordinateSystem::HORIZONTAL;
    return map::CoordinateSystem::EQUATORIAL;
}

json JSONConfigLoader::configToJson(const map::MapConfiguration& config) {
    json j;
    
    // Centro
    j["center"]["ra"] = config.center.getRightAscension();
    j["center"]["dec"] = config.center.getDeclination();
    
    // Campo di vista
    j["field_of_view"]["width"] = config.fieldOfViewWidth;
    j["field_of_view"]["height"] = config.fieldOfViewHeight;
    
    // Dimensioni immagine
    j["image"]["width"] = config.imageWidth;
    j["image"]["height"] = config.imageHeight;
    
    // Proiezione
    j["projection"]["type"] = projectionTypeToString(config.projection);
    j["projection"]["coordinate_system"] = coordinateSystemToString(config.coordinateSystem);
    
    // Magnitudine
    j["limiting_magnitude"] = config.limitingMagnitude;
    
    // Orientamento
    j["orientation"]["rotation_angle"] = config.rotationAngle;
    j["orientation"]["north_up"] = config.northUp;
    j["orientation"]["east_left"] = config.eastLeft;
    
    // Griglia
    j["grid"]["enabled"] = config.gridStyle.enabled;
    j["grid"]["ra_step"] = config.gridStyle.raStepDegrees;
    j["grid"]["dec_step"] = config.gridStyle.decStepDegrees;
    j["grid"]["color"] = config.gridStyle.color;
    j["grid"]["line_width"] = config.gridStyle.lineWidth;
    j["grid"]["show_labels"] = config.gridStyle.showLabels;
    j["grid"]["label_color"] = config.gridStyle.labelColor;
    j["grid"]["label_font_size"] = config.gridStyle.labelFontSize;
    
    // Stelle
    j["stars"]["min_symbol_size"] = config.starStyle.minSymbolSize;
    j["stars"]["max_symbol_size"] = config.starStyle.maxSymbolSize;
    j["stars"]["magnitude_range"] = config.starStyle.magnitudeRange;
    j["stars"]["use_spectral_colors"] = config.starStyle.useSpectralColors;
    j["stars"]["default_color"] = config.starStyle.defaultColor;
    j["stars"]["show_names"] = config.starStyle.showNames;
    j["stars"]["show_sao_numbers"] = config.starStyle.showSAONumbers;
    j["stars"]["show_magnitudes"] = config.starStyle.showMagnitudes;
    j["stars"]["min_magnitude_for_label"] = config.starStyle.minMagnitudeForLabel;
    j["stars"]["label_color"] = config.starStyle.labelColor;
    j["stars"]["label_font_size"] = config.starStyle.labelFontSize;
    
    // Background
    j["background_color"] = config.backgroundColor;
    
    // Display
    j["display"]["show_border"] = config.showBorder;
    j["display"]["show_title"] = config.showTitle;
    j["display"]["title"] = config.title;
    j["display"]["show_scale"] = config.showScale;
    j["display"]["show_legend"] = config.showLegend;
    j["display"]["show_compass"] = config.showCompass;
    
    // Overlays
    j["overlays"]["constellation_lines"] = config.showConstellationLines;
    j["overlays"]["constellation_boundaries"] = config.showConstellationBoundaries;
    j["overlays"]["constellation_names"] = config.showConstellationNames;
    j["overlays"]["milky_way"] = config.showMilkyWay;
    j["overlays"]["ecliptic"] = config.showEcliptic;
    j["overlays"]["equator"] = config.showEquator;
    
    // Observer
    if (config.useObservationTime) {
        j["observation"]["time"] = config.observationTime;
    }
    j["observation"]["latitude"] = config.observerLatitude;
    j["observation"]["longitude"] = config.observerLongitude;
    
    return j;
}

map::MapConfiguration JSONConfigLoader::jsonToConfig(const json& j) {
    map::MapConfiguration config;
    
    // Centro
    if (j.contains("center")) {
        double ra = j["center"].value("ra", 0.0);
        double dec = j["center"].value("dec", 0.0);
        config.center = core::EquatorialCoordinates(ra, dec);
    }
    
    // Campo di vista
    if (j.contains("field_of_view")) {
        config.fieldOfViewWidth = j["field_of_view"].value("width", 10.0);
        config.fieldOfViewHeight = j["field_of_view"].value("height", 10.0);
    }
    
    // Dimensioni immagine
    if (j.contains("image")) {
        config.imageWidth = j["image"].value("width", 1920);
        config.imageHeight = j["image"].value("height", 1080);
    }
    
    // Proiezione
    if (j.contains("projection")) {
        std::string projType = j["projection"].value("type", "stereographic");
        config.projection = stringToProjectionType(projType);
        
        std::string coordSys = j["projection"].value("coordinate_system", "equatorial");
        config.coordinateSystem = stringToCoordinateSystem(coordSys);
    }
    
    // Magnitudine
    config.limitingMagnitude = j.value("limiting_magnitude", 12.0);
    
    // Orientamento
    if (j.contains("orientation")) {
        config.rotationAngle = j["orientation"].value("rotation_angle", 0.0);
        config.northUp = j["orientation"].value("north_up", true);
        config.eastLeft = j["orientation"].value("east_left", true);
    }
    
    // Griglia
    if (j.contains("grid")) {
        config.gridStyle.enabled = j["grid"].value("enabled", true);
        config.gridStyle.raStepDegrees = j["grid"].value("ra_step", 15.0);
        config.gridStyle.decStepDegrees = j["grid"].value("dec_step", 10.0);
        config.gridStyle.color = j["grid"].value("color", 0x404040FFu);
        config.gridStyle.lineWidth = j["grid"].value("line_width", 0.5f);
        config.gridStyle.showLabels = j["grid"].value("show_labels", true);
        config.gridStyle.labelColor = j["grid"].value("label_color", 0xFFFFFFFFu);
        config.gridStyle.labelFontSize = j["grid"].value("label_font_size", 10.0f);
    }
    
    // Stelle
    if (j.contains("stars")) {
        config.starStyle.minSymbolSize = j["stars"].value("min_symbol_size", 0.5f);
        config.starStyle.maxSymbolSize = j["stars"].value("max_symbol_size", 8.0f);
        config.starStyle.magnitudeRange = j["stars"].value("magnitude_range", 10.0f);
        config.starStyle.useSpectralColors = j["stars"].value("use_spectral_colors", true);
        config.starStyle.defaultColor = j["stars"].value("default_color", 0xFFFFFFFFu);
        config.starStyle.showNames = j["stars"].value("show_names", true);
        config.starStyle.showSAONumbers = j["stars"].value("show_sao_numbers", true);
        config.starStyle.showMagnitudes = j["stars"].value("show_magnitudes", false);
        config.starStyle.minMagnitudeForLabel = j["stars"].value("min_magnitude_for_label", 4.0f);
        config.starStyle.labelColor = j["stars"].value("label_color", 0xFFFFFFFFu);
        config.starStyle.labelFontSize = j["stars"].value("label_font_size", 8.0f);
    }
    
    // Background
    config.backgroundColor = j.value("background_color", 0x000000FFu);
    
    // Display
    if (j.contains("display")) {
        config.showBorder = j["display"].value("show_border", true);
        config.showTitle = j["display"].value("show_title", true);
        config.title = j["display"].value("title", "");
        config.showScale = j["display"].value("show_scale", true);
        config.showLegend = j["display"].value("show_legend", false);
        config.showCompass = j["display"].value("show_compass", true);
    }
    
    // Overlays
    if (j.contains("overlays")) {
        config.showConstellationLines = j["overlays"].value("constellation_lines", false);
        config.showConstellationBoundaries = j["overlays"].value("constellation_boundaries", false);
        config.showConstellationNames = j["overlays"].value("constellation_names", false);
        config.showMilkyWay = j["overlays"].value("milky_way", false);
        config.showEcliptic = j["overlays"].value("ecliptic", false);
        config.showEquator = j["overlays"].value("equator", false);
    }
    
    // Observer
    if (j.contains("observation")) {
        if (j["observation"].contains("time")) {
            config.useObservationTime = true;
            config.observationTime = j["observation"]["time"];
        }
        config.observerLatitude = j["observation"].value("latitude", 0.0);
        config.observerLongitude = j["observation"].value("longitude", 0.0);
    }
    
    return config;
}

map::MapConfiguration JSONConfigLoader::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    json j;
    file >> j;
    
    return jsonToConfig(j);
}

bool JSONConfigLoader::save(const map::MapConfiguration& config, 
                           const std::string& filename) {
    try {
        json j = configToJson(config);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2); // Pretty print con indentazione 2
        return true;
    } catch (...) {
        return false;
    }
}

map::MapConfiguration JSONConfigLoader::loadFromString(const std::string& data) {
    json j = json::parse(data);
    return jsonToConfig(j);
}

std::string JSONConfigLoader::saveToString(const map::MapConfiguration& config) {
    json j = configToJson(config);
    return j.dump(2);
}

} // namespace config
} // namespace starmap
