#include "starmap/map/GridRenderer.h"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace starmap {
namespace map {

GridRenderer::GridRenderer(const MapConfiguration& config,
                          const Projection& projection)
    : config_(config), projection_(projection) {
}

GridRenderer::~GridRenderer() = default;

std::vector<core::CartesianCoordinates> GridRenderer::generateSmoothCurve(
    const std::vector<core::EquatorialCoordinates>& celestialPoints) {
    
    std::vector<core::CartesianCoordinates> cartesianPoints;
    
    for (const auto& point : celestialPoints) {
        if (projection_.isVisible(point)) {
            cartesianPoints.push_back(projection_.project(point));
        }
    }
    
    return cartesianPoints;
}

std::vector<MapLine> GridRenderer::generateRADecGrid() {
    std::vector<MapLine> lines;
    
    if (!config_.gridStyle.enabled) {
        return lines;
    }
    
    double centerRA = config_.center.getRightAscension();
    double centerDec = config_.center.getDeclination();
    double fovRA = config_.fieldOfViewWidth;
    double fovDec = config_.fieldOfViewHeight;
    
    // Linee di RA costante (meridiani)
    double raStep = config_.gridStyle.raStepDegrees;
    double raStart = std::floor((centerRA - fovRA / 2.0) / raStep) * raStep;
    double raEnd = std::ceil((centerRA + fovRA / 2.0) / raStep) * raStep;
    
    for (double ra = raStart; ra <= raEnd; ra += raStep) {
        MapLine line;
        line.color = config_.gridStyle.color;
        line.width = config_.gridStyle.lineWidth;
        
        std::vector<core::EquatorialCoordinates> points;
        
        // Genera punti lungo il meridiano
        double decStart = std::max(-90.0, centerDec - fovDec / 2.0 - 5.0);
        double decEnd = std::min(90.0, centerDec + fovDec / 2.0 + 5.0);
        
        for (double dec = decStart; dec <= decEnd; dec += 0.5) {
            points.push_back(core::EquatorialCoordinates(ra, dec));
        }
        
        line.points = generateSmoothCurve(points);
        
        if (!line.points.empty()) {
            lines.push_back(line);
        }
    }
    
    // Linee di Dec costante (paralleli)
    double decStep = config_.gridStyle.decStepDegrees;
    double decStart = std::floor((centerDec - fovDec / 2.0) / decStep) * decStep;
    double decEnd = std::ceil((centerDec + fovDec / 2.0) / decStep) * decStep;
    
    for (double dec = decStart; dec <= decEnd; dec += decStep) {
        if (dec < -90.0 || dec > 90.0) continue;
        
        MapLine line;
        line.color = config_.gridStyle.color;
        line.width = config_.gridStyle.lineWidth;
        
        std::vector<core::EquatorialCoordinates> points;
        
        // Genera punti lungo il parallelo
        double raStartLine = centerRA - fovRA / 2.0 - 5.0;
        double raEndLine = centerRA + fovRA / 2.0 + 5.0;
        
        for (double ra = raStartLine; ra <= raEndLine; ra += 0.5) {
            double normalizedRA = ra;
            while (normalizedRA < 0.0) normalizedRA += 360.0;
            while (normalizedRA >= 360.0) normalizedRA -= 360.0;
            
            points.push_back(core::EquatorialCoordinates(normalizedRA, dec));
        }
        
        line.points = generateSmoothCurve(points);
        
        if (!line.points.empty()) {
            lines.push_back(line);
        }
    }
    
    return lines;
}

std::vector<MapLabel> GridRenderer::generateCoordinateLabels() {
    std::vector<MapLabel> labels;
    
    if (!config_.gridStyle.showLabels) {
        return labels;
    }
    
    double centerRA = config_.center.getRightAscension();
    double centerDec = config_.center.getDeclination();
    double fovRA = config_.fieldOfViewWidth;
    double fovDec = config_.fieldOfViewHeight;
    
    // Labels per RA (lungo il bordo superiore o inferiore)
    double raStep = config_.gridStyle.raStepDegrees;
    double raStart = std::floor((centerRA - fovRA / 2.0) / raStep) * raStep;
    double raEnd = std::ceil((centerRA + fovRA / 2.0) / raStep) * raStep;
    
    double labelDec = centerDec + fovDec / 2.0 - 1.0; // Vicino al bordo superiore
    
    for (double ra = raStart; ra <= raEnd; ra += raStep) {
        core::EquatorialCoordinates coord(ra, labelDec);
        
        if (projection_.isVisible(coord)) {
            MapLabel label;
            label.position = projection_.project(coord);
            label.color = config_.gridStyle.labelColor;
            label.fontSize = config_.gridStyle.labelFontSize;
            
            // Formatta RA come ore
            int hours = static_cast<int>(ra / 15.0);
            int minutes = static_cast<int>((ra / 15.0 - hours) * 60.0);
            
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << hours << "h"
                << std::setw(2) << minutes << "m";
            label.text = oss.str();
            
            labels.push_back(label);
        }
    }
    
    // Labels per Dec (lungo il bordo sinistro)
    double decStep = config_.gridStyle.decStepDegrees;
    double decStart = std::floor((centerDec - fovDec / 2.0) / decStep) * decStep;
    double decEnd = std::ceil((centerDec + fovDec / 2.0) / decStep) * decStep;
    
    double labelRA = centerRA - fovRA / 2.0 + 1.0;
    
    for (double dec = decStart; dec <= decEnd; dec += decStep) {
        if (dec < -90.0 || dec > 90.0) continue;
        
        core::EquatorialCoordinates coord(labelRA, dec);
        
        if (projection_.isVisible(coord)) {
            MapLabel label;
            label.position = projection_.project(coord);
            label.color = config_.gridStyle.labelColor;
            label.fontSize = config_.gridStyle.labelFontSize;
            
            std::ostringstream oss;
            oss << (dec >= 0 ? "+" : "") << static_cast<int>(dec) << "°";
            label.text = oss.str();
            
            labels.push_back(label);
        }
    }
    
    return labels;
}

MapLine GridRenderer::generateCelestialEquator() {
    MapLine line;
    line.color = 0xFF0000FF; // Rosso
    line.width = 1.0f;
    
    std::vector<core::EquatorialCoordinates> points;
    
    double centerRA = config_.center.getRightAscension();
    double fovRA = config_.fieldOfViewWidth;
    
    for (double ra = centerRA - fovRA / 2.0 - 5.0; 
         ra <= centerRA + fovRA / 2.0 + 5.0; 
         ra += 0.5) {
        double normalizedRA = ra;
        while (normalizedRA < 0.0) normalizedRA += 360.0;
        while (normalizedRA >= 360.0) normalizedRA -= 360.0;
        
        points.push_back(core::EquatorialCoordinates(normalizedRA, 0.0));
    }
    
    line.points = generateSmoothCurve(points);
    return line;
}

MapLine GridRenderer::generateEcliptic() {
    MapLine line;
    line.color = 0xFFFF00FF; // Giallo
    line.width = 1.0f;
    
    // L'eclittica è inclinata di ~23.44° rispetto all'equatore celeste
    const double obliquity = 23.44;
    
    std::vector<core::EquatorialCoordinates> points;
    
    // Genera punti lungo l'eclittica
    for (double lambda = 0.0; lambda < 360.0; lambda += 1.0) {
        // Converti coordinate eclittiche in equatoriali
        double lambdaRad = lambda * M_PI / 180.0;
        double oblRad = obliquity * M_PI / 180.0;
        
        double ra = std::atan2(std::sin(lambdaRad) * std::cos(oblRad), 
                              std::cos(lambdaRad)) * 180.0 / M_PI;
        double dec = std::asin(std::sin(lambdaRad) * std::sin(oblRad)) * 180.0 / M_PI;
        
        if (ra < 0.0) ra += 360.0;
        
        points.push_back(core::EquatorialCoordinates(ra, dec));
    }
    
    line.points = generateSmoothCurve(points);
    return line;
}

std::vector<MapLine> GridRenderer::generateCompass() {
    std::vector<MapLine> lines;
    
    if (!config_.showCompass) {
        return lines;
    }
    
    // Posizione della rosa dei venti (angolo superiore destro)
    double compassSize = 0.8; // In unità normalizzate
    core::CartesianCoordinates compassCenter(0.85, 0.85);
    
    // Freccia Nord
    MapLine northArrow;
    northArrow.color = 0xFF0000FF; // Rosso
    northArrow.width = 2.0f;
    northArrow.points.push_back(compassCenter);
    northArrow.points.push_back(core::CartesianCoordinates(
        compassCenter.getX(), 
        compassCenter.getY() + compassSize * 0.1));
    lines.push_back(northArrow);
    
    // Etichetta N
    // (Questo richiederebbe aggiungere testo, gestito separatamente)
    
    // Freccia Est
    MapLine eastArrow;
    eastArrow.color = 0x00FF00FF; // Verde
    eastArrow.width = 1.5f;
    eastArrow.points.push_back(compassCenter);
    eastArrow.points.push_back(core::CartesianCoordinates(
        compassCenter.getX() - compassSize * 0.1,  // Est a sinistra in astronomia
        compassCenter.getY()));
    lines.push_back(eastArrow);
    
    return lines;
}

std::vector<MapLine> GridRenderer::generateScale() {
    std::vector<MapLine> lines;
    
    if (!config_.showScale) {
        return lines;
    }
    
    // Barra di scala in basso
    // TODO: Implementare scala graduata
    
    return lines;
}

} // namespace map
} // namespace starmap
