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
    
    // Performance e memoria
    j["performance"]["max_stars"] = config.maxStars;
    j["performance"]["star_batch_size"] = config.starBatchSize;
    
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
    
    // Display (backward compatibility)
    j["display"]["show_border"] = config.showBorder;
    j["display"]["show_title"] = config.showTitle;
    j["display"]["title"] = config.title;
    j["display"]["show_scale"] = config.showScale;
    j["display"]["show_legend"] = config.showLegend;
    j["display"]["show_compass"] = config.showCompass;
    
    // Stile titolo
    j["title"]["enabled"] = config.titleStyle.enabled;
    j["title"]["text"] = config.titleStyle.text;
    j["title"]["position"] = static_cast<int>(config.titleStyle.position);
    j["title"]["custom_x"] = config.titleStyle.customX;
    j["title"]["custom_y"] = config.titleStyle.customY;
    j["title"]["font_size"] = config.titleStyle.fontSize;
    j["title"]["text_color"] = config.titleStyle.textColor;
    j["title"]["background_color"] = config.titleStyle.backgroundColor;
    j["title"]["show_background"] = config.titleStyle.showBackground;
    j["title"]["padding_x"] = config.titleStyle.paddingX;
    j["title"]["padding_y"] = config.titleStyle.paddingY;
    
    // Stile bordo
    j["border"]["enabled"] = config.borderStyle.enabled;
    j["border"]["color"] = config.borderStyle.color;
    j["border"]["width"] = config.borderStyle.width;
    j["border"]["margin"] = config.borderStyle.margin;
    
    // Stile bussola
    j["compass"]["enabled"] = config.compassStyle.enabled;
    j["compass"]["position"] = static_cast<int>(config.compassStyle.position);
    j["compass"]["custom_x"] = config.compassStyle.customX;
    j["compass"]["custom_y"] = config.compassStyle.customY;
    j["compass"]["size"] = config.compassStyle.size;
    j["compass"]["north_color"] = config.compassStyle.northColor;
    j["compass"]["east_color"] = config.compassStyle.eastColor;
    j["compass"]["line_color"] = config.compassStyle.lineColor;
    j["compass"]["line_width"] = config.compassStyle.lineWidth;
    j["compass"]["show_labels"] = config.compassStyle.showLabels;
    j["compass"]["label_font_size"] = config.compassStyle.labelFontSize;
    
    // Stile scala
    j["scale"]["enabled"] = config.scaleStyle.enabled;
    j["scale"]["position"] = static_cast<int>(config.scaleStyle.position);
    j["scale"]["custom_x"] = config.scaleStyle.customX;
    j["scale"]["custom_y"] = config.scaleStyle.customY;
    j["scale"]["length"] = config.scaleStyle.length;
    j["scale"]["color"] = config.scaleStyle.color;
    j["scale"]["line_width"] = config.scaleStyle.lineWidth;
    j["scale"]["font_size"] = config.scaleStyle.fontSize;
    j["scale"]["show_text"] = config.scaleStyle.showText;
    
    // Overlays
    j["overlays"]["constellation_lines"] = config.showConstellationLines;
    j["overlays"]["constellation_boundaries"] = config.showConstellationBoundaries;
    j["overlays"]["constellation_names"] = config.showConstellationNames;
    j["overlays"]["milky_way"] = config.showMilkyWay;
    j["overlays"]["ecliptic"] = config.showEcliptic;
    j["overlays"]["equator"] = config.showEquator;
    
    // Scala magnitudini
    j["magnitude_legend"]["enabled"] = config.magnitudeLegend.enabled;
    j["magnitude_legend"]["position"] = static_cast<int>(config.magnitudeLegend.position);
    j["magnitude_legend"]["custom_x"] = config.magnitudeLegend.customX;
    j["magnitude_legend"]["custom_y"] = config.magnitudeLegend.customY;
    j["magnitude_legend"]["font_size"] = config.magnitudeLegend.fontSize;
    j["magnitude_legend"]["text_color"] = config.magnitudeLegend.textColor;
    j["magnitude_legend"]["background_color"] = config.magnitudeLegend.backgroundColor;
    j["magnitude_legend"]["show_background"] = config.magnitudeLegend.showBackground;
    
    // Rettangoli overlay
    j["overlay_rectangles"] = json::array();
    for (const auto& rect : config.overlayRectangles) {
        json r;
        r["enabled"] = rect.enabled;
        r["center_ra"] = rect.centerRA;
        r["center_dec"] = rect.centerDec;
        r["width_ra"] = rect.widthRA;
        r["height_dec"] = rect.heightDec;
        r["color"] = rect.color;
        r["line_width"] = rect.lineWidth;
        r["filled"] = rect.filled;
        r["fill_color"] = rect.fillColor;
        r["label"] = rect.label;
        j["overlay_rectangles"].push_back(r);
    }
    
    // Path overlay
    j["overlay_paths"] = json::array();
    for (const auto& path : config.overlayPaths) {
        json p;
        p["enabled"] = path.enabled;
        p["color"] = path.color;
        p["line_width"] = path.lineWidth;
        p["show_points"] = path.showPoints;
        p["point_size"] = path.pointSize;
        p["show_labels"] = path.showLabels;
        p["show_direction"] = path.showDirection;
        p["label"] = path.label;
        
        p["points"] = json::array();
        for (const auto& point : path.points) {
            json pt;
            pt["ra"] = point.ra;
            pt["dec"] = point.dec;
            pt["label"] = point.label;
            p["points"].push_back(pt);
        }
        
        j["overlay_paths"].push_back(p);
    }
    
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
    
    // Performance e memoria
    if (j.contains("performance")) {
        config.maxStars = j["performance"].value("max_stars", 50000);
        config.starBatchSize = j["performance"].value("star_batch_size", 5000);
    }
    
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
    
    // Display (backward compatibility)
    if (j.contains("display")) {
        config.showBorder = j["display"].value("show_border", true);
        config.showTitle = j["display"].value("show_title", true);
        config.title = j["display"].value("title", "");
        config.showScale = j["display"].value("show_scale", true);
        config.showLegend = j["display"].value("show_legend", false);
        config.showCompass = j["display"].value("show_compass", true);
    }
    
    // Stile titolo
    if (j.contains("title")) {
        config.titleStyle.enabled = j["title"].value("enabled", true);
        config.titleStyle.text = j["title"].value("text", "");
        int pos = j["title"].value("position", 1); // TOP_LEFT = 1
        config.titleStyle.position = static_cast<map::LegendPosition>(pos);
        config.titleStyle.customX = j["title"].value("custom_x", 0.0f);
        config.titleStyle.customY = j["title"].value("custom_y", 0.0f);
        config.titleStyle.fontSize = j["title"].value("font_size", 18.0f);
        config.titleStyle.textColor = j["title"].value("text_color", 0xFFFFFFFFu);
        config.titleStyle.backgroundColor = j["title"].value("background_color", 0x00000000u);
        config.titleStyle.showBackground = j["title"].value("show_background", false);
        config.titleStyle.paddingX = j["title"].value("padding_x", 10.0f);
        config.titleStyle.paddingY = j["title"].value("padding_y", 10.0f);
        
        // Backward compatibility
        if (config.titleStyle.text.empty() && !config.title.empty()) {
            config.titleStyle.text = config.title;
        }
        config.titleStyle.enabled = config.titleStyle.enabled && config.showTitle;
    } else if (!config.title.empty()) {
        config.titleStyle.enabled = config.showTitle;
        config.titleStyle.text = config.title;
    }
    
    // Stile bordo
    if (j.contains("border")) {
        config.borderStyle.enabled = j["border"].value("enabled", true);
        config.borderStyle.color = j["border"].value("color", 0xFFFFFFFFu);
        config.borderStyle.width = j["border"].value("width", 2.0f);
        config.borderStyle.margin = j["border"].value("margin", 0.0f);
    } else {
        config.borderStyle.enabled = config.showBorder;
    }
    
    // Stile bussola
    if (j.contains("compass")) {
        config.compassStyle.enabled = j["compass"].value("enabled", true);
        int pos = j["compass"].value("position", 2); // TOP_RIGHT = 2
        config.compassStyle.position = static_cast<map::LegendPosition>(pos);
        config.compassStyle.customX = j["compass"].value("custom_x", 0.0f);
        config.compassStyle.customY = j["compass"].value("custom_y", 0.0f);
        config.compassStyle.size = j["compass"].value("size", 80.0f);
        config.compassStyle.northColor = j["compass"].value("north_color", 0xFF0000FFu);
        config.compassStyle.eastColor = j["compass"].value("east_color", 0x00FF00FFu);
        config.compassStyle.lineColor = j["compass"].value("line_color", 0xFFFFFFFFu);
        config.compassStyle.lineWidth = j["compass"].value("line_width", 2.0f);
        config.compassStyle.showLabels = j["compass"].value("show_labels", true);
        config.compassStyle.labelFontSize = j["compass"].value("label_font_size", 10.0f);
    } else {
        config.compassStyle.enabled = config.showCompass;
    }
    
    // Stile scala
    if (j.contains("scale")) {
        config.scaleStyle.enabled = j["scale"].value("enabled", true);
        int pos = j["scale"].value("position", 3); // BOTTOM_LEFT = 3
        config.scaleStyle.position = static_cast<map::LegendPosition>(pos);
        config.scaleStyle.customX = j["scale"].value("custom_x", 0.0f);
        config.scaleStyle.customY = j["scale"].value("custom_y", 0.0f);
        config.scaleStyle.length = j["scale"].value("length", 100.0f);
        config.scaleStyle.color = j["scale"].value("color", 0xFFFFFFFFu);
        config.scaleStyle.lineWidth = j["scale"].value("line_width", 2.0f);
        config.scaleStyle.fontSize = j["scale"].value("font_size", 10.0f);
        config.scaleStyle.showText = j["scale"].value("show_text", true);
    } else {
        config.scaleStyle.enabled = config.showScale;
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
    
    // Scala magnitudini
    if (j.contains("magnitude_legend")) {
        config.magnitudeLegend.enabled = j["magnitude_legend"].value("enabled", false);
        int pos = j["magnitude_legend"].value("position", 4); // BOTTOM_RIGHT = 4
        config.magnitudeLegend.position = static_cast<map::LegendPosition>(pos);
        config.magnitudeLegend.customX = j["magnitude_legend"].value("custom_x", 0.0f);
        config.magnitudeLegend.customY = j["magnitude_legend"].value("custom_y", 0.0f);
        config.magnitudeLegend.fontSize = j["magnitude_legend"].value("font_size", 10.0f);
        config.magnitudeLegend.textColor = j["magnitude_legend"].value("text_color", 0xFFFFFFFFu);
        config.magnitudeLegend.backgroundColor = j["magnitude_legend"].value("background_color", 0x000000CCu);
        config.magnitudeLegend.showBackground = j["magnitude_legend"].value("show_background", true);
    }
    
    // Rettangoli overlay
    if (j.contains("overlay_rectangles") && j["overlay_rectangles"].is_array()) {
        for (const auto& r : j["overlay_rectangles"]) {
            map::OverlayRectangle rect;
            rect.enabled = r.value("enabled", false);
            rect.centerRA = r.value("center_ra", 0.0);
            rect.centerDec = r.value("center_dec", 0.0);
            rect.widthRA = r.value("width_ra", 0.0);
            rect.heightDec = r.value("height_dec", 0.0);
            rect.color = r.value("color", 0xFF0000FFu);
            rect.lineWidth = r.value("line_width", 2.0f);
            rect.filled = r.value("filled", false);
            rect.fillColor = r.value("fill_color", 0xFF000033u);
            rect.label = r.value("label", "");
            config.overlayRectangles.push_back(rect);
        }
    }
    
    // Path overlay
    if (j.contains("overlay_paths") && j["overlay_paths"].is_array()) {
        for (const auto& p : j["overlay_paths"]) {
            map::OverlayPath path;
            path.enabled = p.value("enabled", false);
            path.color = p.value("color", 0x00FF00FFu);
            path.lineWidth = p.value("line_width", 2.0f);
            path.showPoints = p.value("show_points", true);
            path.pointSize = p.value("point_size", 3.0f);
            path.showLabels = p.value("show_labels", false);
            path.showDirection = p.value("show_direction", false);
            path.label = p.value("label", "");
            
            if (p.contains("points") && p["points"].is_array()) {
                for (const auto& pt : p["points"]) {
                    map::PathPoint point;
                    point.ra = pt.value("ra", 0.0);
                    point.dec = pt.value("dec", 0.0);
                    point.label = pt.value("label", "");
                    path.points.push_back(point);
                }
            }
            
            config.overlayPaths.push_back(path);
        }
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
