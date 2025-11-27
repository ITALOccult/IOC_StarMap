#ifndef STARMAP_MAP_RENDERER_H
#define STARMAP_MAP_RENDERER_H

#include "MapConfiguration.h"
#include "Projection.h"
#include "GridRenderer.h"
#include "starmap/core/CelestialObject.h"
#include <vector>
#include <memory>
#include <string>

namespace starmap {
namespace map {

/**
 * @brief Pixel buffer per immagini
 */
struct ImageBuffer {
    std::vector<uint8_t> data; // RGBA format
    int width;
    int height;
    
    ImageBuffer(int w, int h) : width(w), height(h) {
        data.resize(w * h * 4, 0);
    }
    
    void setPixel(int x, int y, uint32_t color);
    uint32_t getPixel(int x, int y) const;
    
    bool saveAsPNG(const std::string& filename) const;
    bool saveAsJPEG(const std::string& filename, int quality = 95) const;
};

/**
 * @brief Renderer principale per mappe celesti
 */
class MapRenderer {
public:
    explicit MapRenderer(const MapConfiguration& config);
    ~MapRenderer();

    /**
     * @brief Renderizza una mappa completa con stelle
     * @param stars Lista di stelle da renderizzare
     * @return Buffer immagine
     */
    ImageBuffer render(const std::vector<std::shared_ptr<core::Star>>& stars);

    /**
     * @brief Renderizza solo lo sfondo e la griglia
     */
    ImageBuffer renderBackground();

    /**
     * @brief Aggiorna configurazione
     */
    void setConfiguration(const MapConfiguration& config);
    
    /**
     * @brief Ottieni configurazione corrente
     */
    const MapConfiguration& getConfiguration() const { return config_; }

private:
    MapConfiguration config_;
    std::unique_ptr<Projection> projection_;
    std::unique_ptr<GridRenderer> gridRenderer_;
    
    // Helper per rendering
    void drawBackground(ImageBuffer& buffer);
    void drawGrid(ImageBuffer& buffer);
    void drawStars(ImageBuffer& buffer, 
                   const std::vector<std::shared_ptr<core::Star>>& stars);
    void drawStar(ImageBuffer& buffer, 
                  const core::CartesianCoordinates& pos,
                  const core::Star& star);
    void drawLine(ImageBuffer& buffer, 
                  const MapLine& line);
    void drawLabel(ImageBuffer& buffer, 
                   const MapLabel& label);
    void drawBorder(ImageBuffer& buffer);
    void drawTitle(ImageBuffer& buffer);
    
    // Conversione coordinate normalizzate -> pixel
    void normalizedToPixel(const core::CartesianCoordinates& normalized,
                          int& x, int& y) const;
    
    // Calcola dimensione simbolo stella basata su magnitudine
    float calculateStarSize(double magnitude) const;
    
    // Calcola colore stella basato su indice colore o tipo spettrale
    uint32_t calculateStarColor(const core::Star& star) const;
    
    // Antialiasing per cerchi
    void drawCircleAA(ImageBuffer& buffer, int cx, int cy, 
                     float radius, uint32_t color);
};

} // namespace map
} // namespace starmap

#endif // STARMAP_MAP_RENDERER_H
