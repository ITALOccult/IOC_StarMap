#include "starmap/map/MapRenderer.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace starmap {
namespace map {

// ============================================================================
// ImageBuffer
// ============================================================================

void ImageBuffer::setPixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    
    int idx = (y * width + x) * 4;
    data[idx + 0] = (color >> 24) & 0xFF; // R
    data[idx + 1] = (color >> 16) & 0xFF; // G
    data[idx + 2] = (color >> 8) & 0xFF;  // B
    data[idx + 3] = color & 0xFF;          // A
}

uint32_t ImageBuffer::getPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return 0;
    
    int idx = (y * width + x) * 4;
    return (data[idx + 0] << 24) | (data[idx + 1] << 16) | 
           (data[idx + 2] << 8) | data[idx + 3];
}

bool ImageBuffer::saveAsPNG(const std::string& filename) const {
    // TODO: Implementare usando libreria stb_image_write o libpng
    // Per ora ritorna false
    return false;
}

bool ImageBuffer::saveAsJPEG(const std::string& filename, int quality) const {
    // TODO: Implementare usando libreria stb_image_write o libjpeg
    return false;
}

// ============================================================================
// MapRenderer
// ============================================================================

MapRenderer::MapRenderer(const MapConfiguration& config) 
    : config_(config) {
    
    projection_ = ProjectionFactory::create(
        config_.projection,
        config_.center,
        config_.fieldOfViewWidth,
        config_.fieldOfViewHeight
    );
    
    gridRenderer_ = std::make_unique<GridRenderer>(config_, *projection_);
}

MapRenderer::~MapRenderer() = default;

void MapRenderer::setConfiguration(const MapConfiguration& config) {
    config_ = config;
    
    projection_ = ProjectionFactory::create(
        config_.projection,
        config_.center,
        config_.fieldOfViewWidth,
        config_.fieldOfViewHeight
    );
    
    gridRenderer_ = std::make_unique<GridRenderer>(config_, *projection_);
}

ImageBuffer MapRenderer::renderBackground() {
    ImageBuffer buffer(config_.imageWidth, config_.imageHeight);
    
    drawBackground(buffer);
    drawGrid(buffer);
    
    if (config_.showBorder) {
        drawBorder(buffer);
    }
    
    if (config_.showTitle && !config_.title.empty()) {
        drawTitle(buffer);
    }
    
    return buffer;
}

ImageBuffer MapRenderer::render(
    const std::vector<std::shared_ptr<core::Star>>& stars) {
    
    ImageBuffer buffer = renderBackground();
    drawStars(buffer, stars);
    
    return buffer;
}

void MapRenderer::drawBackground(ImageBuffer& buffer) {
    uint32_t bgColor = config_.backgroundColor;
    
    for (int y = 0; y < buffer.height; ++y) {
        for (int x = 0; x < buffer.width; ++x) {
            buffer.setPixel(x, y, bgColor);
        }
    }
}

void MapRenderer::drawGrid(ImageBuffer& buffer) {
    if (!config_.gridStyle.enabled) {
        return;
    }
    
    // Disegna griglia RA/Dec
    auto gridLines = gridRenderer_->generateRADecGrid();
    for (const auto& line : gridLines) {
        drawLine(buffer, line);
    }
    
    // Disegna etichette
    auto labels = gridRenderer_->generateCoordinateLabels();
    for (const auto& label : labels) {
        drawLabel(buffer, label);
    }
    
    // Disegna equatore celeste
    if (config_.showEquator) {
        auto equator = gridRenderer_->generateCelestialEquator();
        drawLine(buffer, equator);
    }
    
    // Disegna eclittica
    if (config_.showEcliptic) {
        auto ecliptic = gridRenderer_->generateEcliptic();
        drawLine(buffer, ecliptic);
    }
    
    // Disegna bussola
    if (config_.showCompass) {
        auto compass = gridRenderer_->generateCompass();
        for (const auto& line : compass) {
            drawLine(buffer, line);
        }
    }
}

void MapRenderer::normalizedToPixel(
    const core::CartesianCoordinates& normalized,
    int& x, int& y) const {
    
    // Converti da coordinate normalizzate [-1, 1] a pixel
    double aspectRatio = static_cast<double>(config_.imageWidth) / config_.imageHeight;
    
    x = static_cast<int>((normalized.getX() / aspectRatio + 1.0) * 0.5 * config_.imageWidth);
    y = static_cast<int>((1.0 - normalized.getY()) * 0.5 * config_.imageHeight);
}

float MapRenderer::calculateStarSize(double magnitude) const {
    const auto& style = config_.starStyle;
    
    // Mappa magnitudine su dimensione simbolo
    // mag brillanti (< 0) -> simbolo grande
    // mag deboli (> 10) -> simbolo piccolo
    
    float normalizedMag = (style.magnitudeRange - magnitude) / style.magnitudeRange;
    normalizedMag = std::max(0.0f, std::min(1.0f, normalizedMag));
    
    return style.minSymbolSize + normalizedMag * (style.maxSymbolSize - style.minSymbolSize);
}

uint32_t MapRenderer::calculateStarColor(const core::Star& star) const {
    if (!config_.starStyle.useSpectralColors) {
        return config_.starStyle.defaultColor;
    }
    
    // Usa color index (B-V) se disponibile
    auto colorIndex = star.getColorIndex();
    
    if (colorIndex.has_value()) {
        double bv = colorIndex.value();
        
        // Mappa B-V su colore RGB
        // B-V: -0.4 (blu) a +2.0 (rosso)
        uint8_t r, g, b;
        
        if (bv < 0.0) {
            // Stelle blu-bianche (O, B)
            r = 155 + static_cast<uint8_t>(100 * (1.0 + bv / 0.4));
            g = 200 + static_cast<uint8_t>(55 * (1.0 + bv / 0.4));
            b = 255;
        } else if (bv < 0.6) {
            // Stelle bianche (A, F)
            r = 255;
            g = 255 - static_cast<uint8_t>(55 * bv / 0.6);
            b = 255 - static_cast<uint8_t>(155 * bv / 0.6);
        } else if (bv < 1.2) {
            // Stelle gialle (G, K)
            r = 255;
            g = 200 - static_cast<uint8_t>(100 * (bv - 0.6) / 0.6);
            b = 100 - static_cast<uint8_t>(100 * (bv - 0.6) / 0.6);
        } else {
            // Stelle rosse (M)
            r = 255;
            g = static_cast<uint8_t>(std::max(0.0, 100.0 - 50.0 * (bv - 1.2)));
            b = 0;
        }
        
        return (r << 24) | (g << 16) | (b << 8) | 0xFF;
    }
    
    // Default: bianco
    return config_.starStyle.defaultColor;
}

void MapRenderer::drawCircleAA(ImageBuffer& buffer, int cx, int cy, 
                              float radius, uint32_t color) {
    
    int minX = std::max(0, static_cast<int>(cx - radius - 1));
    int maxX = std::min(buffer.width - 1, static_cast<int>(cx + radius + 1));
    int minY = std::max(0, static_cast<int>(cy - radius - 1));
    int maxY = std::min(buffer.height - 1, static_cast<int>(cy + radius + 1));
    
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;
    
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float dx = x - cx;
            float dy = y - cy;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist <= radius + 0.5f) {
                float alpha = 1.0f;
                if (dist > radius - 0.5f) {
                    alpha = radius + 0.5f - dist;
                }
                
                uint8_t finalAlpha = static_cast<uint8_t>(a * alpha);
                uint32_t pixelColor = (r << 24) | (g << 16) | (b << 8) | finalAlpha;
                
                // Alpha blending
                uint32_t bgColor = buffer.getPixel(x, y);
                uint8_t bgR = (bgColor >> 24) & 0xFF;
                uint8_t bgG = (bgColor >> 16) & 0xFF;
                uint8_t bgB = (bgColor >> 8) & 0xFF;
                
                float blendAlpha = finalAlpha / 255.0f;
                uint8_t newR = static_cast<uint8_t>(r * blendAlpha + bgR * (1.0f - blendAlpha));
                uint8_t newG = static_cast<uint8_t>(g * blendAlpha + bgG * (1.0f - blendAlpha));
                uint8_t newB = static_cast<uint8_t>(b * blendAlpha + bgB * (1.0f - blendAlpha));
                
                buffer.setPixel(x, y, (newR << 24) | (newG << 16) | (newB << 8) | 0xFF);
            }
        }
    }
}

void MapRenderer::drawStars(ImageBuffer& buffer, 
                           const std::vector<std::shared_ptr<core::Star>>& stars) {
    
    for (const auto& star : stars) {
        if (!star) continue;
        
        const auto& coords = star->getCoordinates();
        
        // Verifica se la stella è visibile
        if (!projection_->isVisible(coords)) {
            continue;
        }
        
        // Proietta coordinate
        auto projected = projection_->project(coords);
        
        drawStar(buffer, projected, *star);
    }
}

void MapRenderer::drawStar(ImageBuffer& buffer, 
                          const core::CartesianCoordinates& pos,
                          const core::Star& star) {
    
    int px, py;
    normalizedToPixel(pos, px, py);
    
    // Calcola dimensione e colore
    float size = calculateStarSize(star.getMagnitude());
    uint32_t color = calculateStarColor(star);
    
    // Disegna cerchio con antialiasing
    drawCircleAA(buffer, px, py, size, color);
    
    // Etichetta se necessario
    if (config_.starStyle.showNames && !star.getName().empty() &&
        star.getMagnitude() < config_.starStyle.minMagnitudeForLabel) {
        
        MapLabel label;
        label.position = pos;
        label.text = star.getName();
        label.color = config_.starStyle.labelColor;
        label.fontSize = config_.starStyle.labelFontSize;
        drawLabel(buffer, label);
    }
    
    // Numero SAO se disponibile
    if (config_.starStyle.showSAONumbers && star.getSAONumber().has_value() &&
        star.getMagnitude() < config_.starStyle.minMagnitudeForLabel) {
        
        MapLabel label;
        label.position = core::CartesianCoordinates(pos.getX(), pos.getY() - 0.02);
        label.text = "SAO " + std::to_string(star.getSAONumber().value());
        label.color = config_.starStyle.labelColor;
        label.fontSize = config_.starStyle.labelFontSize * 0.8f;
        drawLabel(buffer, label);
    }
}

void MapRenderer::drawLine(ImageBuffer& buffer, const MapLine& line) {
    if (line.points.size() < 2) return;
    
    for (size_t i = 0; i < line.points.size() - 1; ++i) {
        int x0, y0, x1, y1;
        normalizedToPixel(line.points[i], x0, y0);
        normalizedToPixel(line.points[i + 1], x1, y1);
        
        // Algoritmo di Bresenham per linee
        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;
        
        while (true) {
            buffer.setPixel(x0, y0, line.color);
            
            if (x0 == x1 && y0 == y1) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }
}

void MapRenderer::drawLabel(ImageBuffer& buffer, const MapLabel& label) {
    // TODO: Implementare rendering testo usando libreria come FreeType
    // Per ora segna solo la posizione
    
    int px, py;
    normalizedToPixel(label.position, px, py);
    
    // Disegna piccolo marker
    buffer.setPixel(px, py, label.color);
}

void MapRenderer::drawBorder(ImageBuffer& buffer) {
    uint32_t borderColor = 0xFFFFFFFF; // Bianco
    
    // Bordo superiore
    for (int x = 0; x < buffer.width; ++x) {
        buffer.setPixel(x, 0, borderColor);
        buffer.setPixel(x, 1, borderColor);
    }
    
    // Bordo inferiore
    for (int x = 0; x < buffer.width; ++x) {
        buffer.setPixel(x, buffer.height - 1, borderColor);
        buffer.setPixel(x, buffer.height - 2, borderColor);
    }
    
    // Bordo sinistro
    for (int y = 0; y < buffer.height; ++y) {
        buffer.setPixel(0, y, borderColor);
        buffer.setPixel(1, y, borderColor);
    }
    
    // Bordo destro
    for (int y = 0; y < buffer.height; ++y) {
        buffer.setPixel(buffer.width - 1, y, borderColor);
        buffer.setPixel(buffer.width - 2, y, borderColor);
    }
}

void MapRenderer::drawTitle(ImageBuffer& buffer) {
    // TODO: Implementare rendering titolo con FreeType
}

} // namespace map
} // namespace starmap
