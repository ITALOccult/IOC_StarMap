#include "starmap/map/MapRenderer.h"
#include <algorithm>
#include <cmath>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "starmap/utils/stb_image_write.h"

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
    if (data.empty() || width <= 0 || height <= 0) return false;
    
    // stbi_write_png(filename, w, h, comp, data, stride)
    // format is RGBA, so comp = 4
    int result = stbi_write_png(filename.c_str(), width, height, 4, data.data(), width * 4);
    return result != 0;
}

bool ImageBuffer::saveAsJPEG(const std::string& filename, int quality) const {
    if (data.empty() || width <= 0 || height <= 0) return false;
    
    // stbi_write_jpg(filename, w, h, comp, data, quality)
    int result = stbi_write_jpg(filename.c_str(), width, height, 4, data.data(), quality);
    return result != 0;
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
    
    // Se troppe stelle, usa rendering in batch
    if (stars.size() > static_cast<size_t>(config_.starBatchSize)) {
        renderStarsBatched(buffer, stars);
    } else {
        drawStars(buffer, stars);
    }
    
    // Disegna overlay personalizzati
    drawOverlayRectangles(buffer);
    drawOverlayPaths(buffer);
    drawMagnitudeLegend(buffer);
    
    return buffer;
}

void MapRenderer::renderStarsBatched(ImageBuffer& buffer,
                                     const std::vector<std::shared_ptr<core::Star>>& stars,
                                     int batchSize) {
    // Usa dimensione batch dalla config se non specificato
    if (batchSize <= 0) {
        batchSize = config_.starBatchSize;
    }
    
    // Processa le stelle in batch per ridurre il consumo di memoria
    size_t totalStars = stars.size();
    for (size_t i = 0; i < totalStars; i += batchSize) {
        size_t end = std::min(i + batchSize, totalStars);
        
        // Crea un vettore temporaneo per questo batch
        std::vector<std::shared_ptr<core::Star>> batch(
            stars.begin() + i, 
            stars.begin() + end
        );
        
        // Renderizza questo batch
        drawStars(buffer, batch);
        
        // Il batch viene distrutto qui, liberando memoria
    }
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
    
    // Scala astronomica: la brillantezza raddoppia circa ogni 0.75 magnitudini.
    // Usiamo una scala di potenza per un effetto più realistico.
    // mag brillanti (0 o neg) -> r grande
    // mag deboli (10+) -> r piccolo
    
    // Normalizziamo la magnitudine: 0.0 per magnitudine limite, 1.0 per magnitudine 0 o inferiore
    float normalizedMag = (config_.limitingMagnitude - magnitude) / config_.limitingMagnitude;
    normalizedMag = std::max(0.0f, normalizedMag);
    
    // Usiamo una curva di potenza per dare più peso alle stelle luminose
    float powerScale = std::pow(normalizedMag, 1.5f);
    
    float size = style.minSymbolSize + powerScale * (style.maxSymbolSize - style.minSymbolSize);
    return std::min(style.maxSymbolSize, size);
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
    // Aumentiamo il limite di magnitudine per le label (molte stelle di occultazione sono mag 8-9)
    float labelLimit = std::max(config_.starStyle.minMagnitudeForLabel, 10.0f);
    
    if (config_.starStyle.showNames && !star.getName().empty() &&
        star.getMagnitude() < labelLimit) {
        
        MapLabel label;
        label.position = pos;
        label.text = star.getName();
        label.color = config_.starStyle.labelColor;
        label.fontSize = config_.starStyle.labelFontSize;
        drawLabel(buffer, label);
    }
    
    // Numero SAO se disponibile
    if (config_.starStyle.showSAONumbers && star.getSAONumber().has_value() &&
        star.getMagnitude() < labelLimit) {
        
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
    // TODO: Implementare rendering testo vero
    // Per ora disegniamo un piccolo rettangolo per rendere visibile la presenza di testo
    
    int px, py;
    normalizedToPixel(label.position, px, py);
    
    // Disegna un piccolo segnaposto (box 3x3) per la label
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            buffer.setPixel(px + dx, py + dy, label.color);
        }
    }
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

void MapRenderer::drawMagnitudeLegend(ImageBuffer& buffer) {
    if (!config_.magnitudeLegend.enabled) return;
    
    // Determina posizione
    int x = 0, y = 0;
    int legendWidth = 150;
    int legendHeight = 200;
    int margin = 20;
    
    switch (config_.magnitudeLegend.position) {
        case map::LegendPosition::TOP_LEFT:
            x = margin;
            y = margin;
            break;
        case map::LegendPosition::TOP_RIGHT:
            x = buffer.width - legendWidth - margin;
            y = margin;
            break;
        case map::LegendPosition::BOTTOM_LEFT:
            x = margin;
            y = buffer.height - legendHeight - margin;
            break;
        case map::LegendPosition::BOTTOM_RIGHT:
            x = buffer.width - legendWidth - margin;
            y = buffer.height - legendHeight - margin;
            break;
        case map::LegendPosition::CUSTOM:
            x = static_cast<int>(config_.magnitudeLegend.customX * buffer.width);
            y = static_cast<int>(config_.magnitudeLegend.customY * buffer.height);
            break;
        case map::LegendPosition::NONE:
            return;
    }
    
    // Disegna sfondo se richiesto
    if (config_.magnitudeLegend.showBackground) {
        uint32_t bgColor = config_.magnitudeLegend.backgroundColor;
        for (int dy = 0; dy < legendHeight; ++dy) {
            for (int dx = 0; dx < legendWidth; ++dx) {
                int px = x + dx;
                int py = y + dy;
                if (px >= 0 && px < buffer.width && py >= 0 && py < buffer.height) {
                    buffer.setPixel(px, py, bgColor);
                }
            }
        }
    }
    
    // TODO: Disegna simboli stelle e magnitudini con FreeType
    // Per ora disegna solo cerchi di dimensioni diverse
    int numSamples = 5;
    for (int i = 0; i < numSamples; ++i) {
        double mag = i * 2.0; // mag 0, 2, 4, 6, 8
        float size = calculateStarSize(mag);
        int cy = y + 30 + i * 35;
        int cx = x + 30;
        
        drawCircleAA(buffer, cx, cy, size, config_.magnitudeLegend.textColor);
    }
}

void MapRenderer::drawOverlayRectangles(ImageBuffer& buffer) {
    for (const auto& rect : config_.overlayRectangles) {
        if (rect.enabled) {
            drawRectangle(buffer, rect);
        }
    }
}

void MapRenderer::drawOverlayPaths(ImageBuffer& buffer) {
    for (const auto& path : config_.overlayPaths) {
        if (path.enabled) {
            drawPath(buffer, path);
        }
    }
}

void MapRenderer::drawRectangle(ImageBuffer& buffer, const OverlayRectangle& rect) {
    // Converti coordinate celesti in coordinate schermo
    core::EquatorialCoordinates center(rect.centerRA, rect.centerDec);
    
    if (!projection_->isVisible(center)) return;
    
    // Calcola i 4 angoli del rettangolo
    double halfWidthRA = rect.widthRA / 2.0;
    double halfHeightDec = rect.heightDec / 2.0;
    
    // Corregge per la curvatura del cielo se necessario
    double cosCenter = std::cos(rect.centerDec * M_PI / 180.0);
    double actualHalfWidthRA = halfWidthRA / (cosCenter > 0.01 ? cosCenter : 0.01);
    
    core::EquatorialCoordinates corners[4] = {
        core::EquatorialCoordinates(rect.centerRA - actualHalfWidthRA, rect.centerDec - halfHeightDec),
        core::EquatorialCoordinates(rect.centerRA + actualHalfWidthRA, rect.centerDec - halfHeightDec),
        core::EquatorialCoordinates(rect.centerRA + actualHalfWidthRA, rect.centerDec + halfHeightDec),
        core::EquatorialCoordinates(rect.centerRA - actualHalfWidthRA, rect.centerDec + halfHeightDec)
    };
    
    // Converti in pixel
    int px[4], py[4];
    for (int i = 0; i < 4; ++i) {
        if (projection_->isVisible(corners[i])) {
            auto projected = projection_->project(corners[i]);
            normalizedToPixel(projected, px[i], py[i]);
        } else {
            return; // Rettangolo fuori vista
        }
    }
    
    // Disegna riempimento se richiesto
    if (rect.filled) {
        // TODO: Implementare fill del rettangolo
    }
    
    // Disegna bordi
    for (int i = 0; i < 4; ++i) {
        int next = (i + 1) % 4;
        
        // Bresenham line drawing
        int x0 = px[i], y0 = py[i];
        int x1 = px[next], y1 = py[next];
        
        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;
        
        while (true) {
            if (x0 >= 0 && x0 < buffer.width && y0 >= 0 && y0 < buffer.height) {
                // Disegna linea più spessa
                for (int w = -static_cast<int>(rect.lineWidth/2); w <= static_cast<int>(rect.lineWidth/2); ++w) {
                    if (x0 + w >= 0 && x0 + w < buffer.width) {
                        buffer.setPixel(x0 + w, y0, rect.color);
                    }
                    if (y0 + w >= 0 && y0 + w < buffer.height) {
                        buffer.setPixel(x0, y0 + w, rect.color);
                    }
                }
            }
            
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

void MapRenderer::drawPath(ImageBuffer& buffer, const OverlayPath& path) {
    if (path.points.size() < 2) return;
    
    // Disegna linee tra i punti
    for (size_t i = 0; i < path.points.size() - 1; ++i) {
        const auto& p1 = path.points[i];
        const auto& p2 = path.points[i + 1];
        
        core::EquatorialCoordinates coord1(p1.ra, p1.dec);
        core::EquatorialCoordinates coord2(p2.ra, p2.dec);
        
        if (!projection_->isVisible(coord1) || !projection_->isVisible(coord2)) continue;
        
        auto proj1 = projection_->project(coord1);
        auto proj2 = projection_->project(coord2);
        
        int x0, y0, x1, y1;
        normalizedToPixel(proj1, x0, y0);
        normalizedToPixel(proj2, x1, y1);
        
        // Bresenham line drawing
        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;
        
        while (true) {
            if (x0 >= 0 && x0 < buffer.width && y0 >= 0 && y0 < buffer.height) {
                // Disegna linea più spessa
                for (int w = -static_cast<int>(path.lineWidth/2); w <= static_cast<int>(path.lineWidth/2); ++w) {
                    if (x0 + w >= 0 && x0 + w < buffer.width) {
                        buffer.setPixel(x0 + w, y0, path.color);
                    }
                    if (y0 + w >= 0 && y0 + w < buffer.height) {
                        buffer.setPixel(x0, y0 + w, path.color);
                    }
                }
            }
            
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
    
    // Disegna i punti se richiesto
    if (path.showPoints) {
        for (const auto& point : path.points) {
            core::EquatorialCoordinates coord(point.ra, point.dec);
            if (!projection_->isVisible(coord)) continue;
            
            auto projected = projection_->project(coord);
            int px, py;
            normalizedToPixel(projected, px, py);
            
            drawCircleAA(buffer, px, py, path.pointSize, path.color);
        }
    }
}

} // namespace map
} // namespace starmap
