#ifndef STARMAP_GRID_RENDERER_H
#define STARMAP_GRID_RENDERER_H

#include "MapConfiguration.h"
#include "Projection.h"
#include <vector>
#include <memory>

namespace starmap {
namespace map {

/**
 * @brief Struttura per rappresentare una linea nella mappa
 */
struct MapLine {
    std::vector<core::CartesianCoordinates> points;
    uint32_t color;
    float width;
};

/**
 * @brief Struttura per rappresentare un'etichetta
 */
struct MapLabel {
    core::CartesianCoordinates position;
    std::string text;
    uint32_t color;
    float fontSize;
};

/**
 * @brief Renderer per griglia di coordinate e overlay
 */
class GridRenderer {
public:
    GridRenderer(const MapConfiguration& config,
                 const Projection& projection);
    ~GridRenderer();

    /**
     * @brief Genera linee della griglia RA/Dec
     */
    std::vector<MapLine> generateRADecGrid();

    /**
     * @brief Genera etichette per coordinate
     */
    std::vector<MapLabel> generateCoordinateLabels();

    /**
     * @brief Genera linea dell'equatore celeste
     */
    MapLine generateCelestialEquator();

    /**
     * @brief Genera linea dell'eclittica
     */
    MapLine generateEcliptic();

    /**
     * @brief Genera rosa dei venti (indicatore direzioni)
     */
    std::vector<MapLine> generateCompass();

    /**
     * @brief Genera scala della mappa
     */
    std::vector<MapLine> generateScale();

private:
    const MapConfiguration& config_;
    const Projection& projection_;
    
    // Helper per generare curve smooth
    std::vector<core::CartesianCoordinates> generateSmoothCurve(
        const std::vector<core::EquatorialCoordinates>& celestialPoints);
};

} // namespace map
} // namespace starmap

#endif // STARMAP_GRID_RENDERER_H
