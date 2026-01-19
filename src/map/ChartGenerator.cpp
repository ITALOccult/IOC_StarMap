/**
 * @file ChartGenerator.cpp
 * @brief Implementazione del generatore di carte stellari
 */

#include "starmap/map/ChartGenerator.h"
#include "starmap/map/ConstellationData.h"
#include "starmap/catalog/GaiaClient.h"
#include "starmap/catalog/SAOCatalog.h"
#include "starmap/config/LibraryConfig.h"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <regex>
#include <optional>

namespace starmap {
namespace map {

// ============================================================================
// ChartGenerator Implementation
// ============================================================================

ChartGenerator::ChartGenerator() = default;
ChartGenerator::~ChartGenerator() = default;

bool ChartGenerator::loadConfig(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        lastError_ = "Cannot open file: " + jsonPath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return loadConfigFromString(buffer.str());
}

bool ChartGenerator::loadConfigFromString(const std::string& jsonString) {
    return parseJSON(jsonString);
}

void ChartGenerator::setConfig(const ChartConfig& config) {
    config_ = config;
}

bool ChartGenerator::generate() {
    return generate(config_);
}

bool ChartGenerator::generate(const ChartConfig& config) {
    config_ = config;
    
    // Carica stelle
    if (!loadStars()) {
        return false;
    }
    
    // Genera SVG
    std::string svgPath = config_.outputPath + ".svg";
    if (!generateSVG(svgPath)) {
        return false;
    }
    
    // Converti se necessario
    if (config_.outputFormat == "png") {
        std::string pngPath = config_.outputPath + ".png";
        if (!convertToPNG(svgPath, pngPath)) {
            lastError_ = "Failed to convert to PNG (ImageMagick required)";
            // Non è un errore fatale, SVG è stato generato
        }
        outputPath_ = pngPath;
    } else if (config_.outputFormat == "jpg" || config_.outputFormat == "jpeg") {
        std::string jpgPath = config_.outputPath + ".jpg";
        if (!convertToJPG(svgPath, jpgPath)) {
            lastError_ = "Failed to convert to JPG (ImageMagick required)";
        }
        outputPath_ = jpgPath;
    } else {
        outputPath_ = svgPath;
    }
    
    return true;
}

void ChartGenerator::loadBrightStarsFromDatabase() {
    // Carica stelle luminose (mag < 6) dal database stellar_crossref
    // per coprire quelle che mancano in Gaia DR3
    sqlite3* db = nullptr;
    std::string dbPath = config::LibraryConfig::getInstance().getGaiaSaoDbPath();
    
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        return; // Database non disponibile, continua senza
    }
    
    const char* query = R"(
        SELECT gaia_dr3, ra_deg, dec_deg, magnitude, sao, proper_name, bayer, flamsteed
        FROM stars
        WHERE ra_deg BETWEEN ? AND ?
          AND dec_deg BETWEEN ? AND ?
          AND magnitude < 6.0
          AND (gaia_dr3 IS NULL OR gaia_dr3 = 0)
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }
    
    // Calcola bounding box
    double raWidth = config_.fieldRadius / std::cos(config_.centerDec * M_PI / 180.0);
    double raMin = config_.centerRA - raWidth;
    double raMax = config_.centerRA + raWidth;
    double decMin = config_.centerDec - config_.fieldRadius;
    double decMax = config_.centerDec + config_.fieldRadius;
    
    sqlite3_bind_double(stmt, 1, raMin);
    sqlite3_bind_double(stmt, 2, raMax);
    sqlite3_bind_double(stmt, 3, decMin);
    sqlite3_bind_double(stmt, 4, decMax);
    
    int addedCount = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        double ra = sqlite3_column_double(stmt, 1);
        double dec = sqlite3_column_double(stmt, 2);
        double mag = sqlite3_column_double(stmt, 3);
        
        auto star = std::make_shared<core::Star>();
        star->setCoordinates(core::EquatorialCoordinates(ra, dec));
        star->setMagnitude(mag);
        
        // Aggiungi SAO se disponibile
        if (sqlite3_column_type(stmt, 4) == SQLITE_INTEGER) {
            star->setSAONumber(sqlite3_column_int(stmt, 4));
        }
        
        // Aggiungi nome proprio se disponibile
        if (sqlite3_column_type(stmt, 5) == SQLITE_TEXT) {
            const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            if (name && strlen(name) > 0) {
                star->setName(name);
            }
        }
        
        // Se non ha nome proprio, usa Bayer o Flamsteed
        if (star->getName().empty()) {
            if (sqlite3_column_type(stmt, 6) == SQLITE_TEXT) {
                const char* bayer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                if (bayer && strlen(bayer) > 0) {
                    star->setName(bayer);
                }
            } else if (sqlite3_column_type(stmt, 7) == SQLITE_INTEGER) {
                int flamsteed = sqlite3_column_int(stmt, 7);
                if (flamsteed > 0) {
                    star->setName(std::to_string(flamsteed));
                }
            }
        }
        
        stars_.push_back(star);
        addedCount++;
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    if (addedCount > 0) {
        std::cout << "  Aggiunte " << addedCount << " stelle luminose dal database\n";
    }
}

bool ChartGenerator::loadStars() {
    catalog::GaiaClient gaia;
    
    if (!gaia.isAvailable()) {
        lastError_ = "Gaia catalog not available";
        return false;
    }
    
    // Calcola le dimensioni del campo rettangolare in gradi
    // fieldRadius è il semi-asse maggiore (metà larghezza/altezza in gradi)
    double aspectRatio = static_cast<double>(config_.width) / config_.height;
    double fieldW, fieldH;  // Semi-assi in gradi
    
    if (aspectRatio >= 1.0) {
        // Landscape o quadrato: fieldRadius = semi-larghezza
        fieldW = config_.fieldRadius;
        fieldH = config_.fieldRadius / aspectRatio;
    } else {
        // Portrait: fieldRadius = semi-altezza
        fieldH = config_.fieldRadius;
        fieldW = config_.fieldRadius * aspectRatio;
    }
    
    // Query con raggio = diagonale del rettangolo per coprire gli angoli
    double diagonalRadius = std::sqrt(fieldW * fieldW + fieldH * fieldH);
    
    catalog::GaiaQueryParameters params;
    params.center = core::EquatorialCoordinates(config_.centerRA, config_.centerDec);
    params.radiusDegrees = diagonalRadius;
    params.maxMagnitude = config_.maxMagnitude;
    params.maxResults = 20000; // Increased to avoid truncation in wide fields
    
    // Calcola automaticamente il limite ottimale per evitare overflow di memoria
    params.calculateOptimalMaxResults();
    
    std::cout << "  Query Gaia con limite dinamico: " << params.maxResults << " stelle max\n";
    
    auto allStars = gaia.queryRegion(params);
    
    std::cout << "  Query Gaia: " << allStars.size() << " stelle trovate (raggio " << diagonalRadius << "°)\n";
    
    // Filtra per il rettangolo effettivo (non il cerchio)
    double cosCenter = std::cos(config_.centerDec * M_PI / 180.0);
    
    stars_.clear();
    stars_.reserve(std::min(allStars.size(), static_cast<size_t>(50000)));
    
    for (const auto& star : allStars) {
        double ra = star->getCoordinates().getRightAscension();
        double dec = star->getCoordinates().getDeclination();
        
        // Calcola offset dal centro
        double dra = (ra - config_.centerRA) * cosCenter;
        double ddec = dec - config_.centerDec;
        
        // Gestisci wrap-around RA (0-360)
        if (dra > 180 * cosCenter) dra -= 360 * cosCenter;
        if (dra < -180 * cosCenter) dra += 360 * cosCenter;
        
        // Controlla se dentro il rettangolo
        if (std::abs(dra) <= fieldW && std::abs(ddec) <= fieldH) {
            stars_.push_back(star);
        }
        
        // Limita numero stelle per prevenire consumo eccessivo di memoria
        if (stars_.size() >= static_cast<size_t>(50000)) {
            std::cout << "  ATTENZIONE: Raggiunto limite di 50000 stelle, troncamento necessario\n";
            std::cout << "  Suggerimento: riduci limitingMagnitude o fieldOfView per vedere tutte le stelle\n";
            break;
        }
    }
    
    std::cout << "  Stelle nel rettangolo: " << stars_.size() << " (dopo filtro geometrico)\n";
    
    // Filtra per magnitudine minima
    if (config_.minMagnitude > -10) {
        stars_.erase(
            std::remove_if(stars_.begin(), stars_.end(),
                [this](const auto& s) { return s->getMagnitude() < config_.minMagnitude; }),
            stars_.end()
        );
    }
    
    // Arricchisci stelle con numeri SAO se richiesto
    if (config_.showSAONumbers) {
        catalog::SAOCatalog saoCatalog;
        int enrichedCount = 0;
        for (auto& star : stars_) {
            if (star->getMagnitude() <= config_.saoMagnitudeLimit) {
                if (saoCatalog.enrichWithSAO(star)) {
                    enrichedCount++;
                }
            }
        }
        // Debug: mostra quante stelle hanno SAO
        if (enrichedCount > 0) {
            std::cout << "  Arricchite " << enrichedCount << " stelle con numeri SAO\n";
        }
    }
    
    // Aggiungi stelle luminose dal database che mancano in Gaia (mag < 6)
    // Molte stelle luminose non sono in Gaia DR3
    int starsBeforeDB = stars_.size();
    loadBrightStarsFromDatabase();
    std::cout << "  Totale stelle dopo integrazione database: " << stars_.size() << "\n";
    
    return true;
}

std::pair<double, double> ChartGenerator::projectToSVG(double ra, double dec) const {
    double scale = config_.width / (2.0 * config_.fieldRadius);
    double dra = (ra - config_.centerRA) * std::cos(config_.centerDec * M_PI / 180.0);
    double ddec = dec - config_.centerDec;
    
    double x = config_.width / 2.0 - dra * scale;  // RA increases left
    double y = config_.height / 2.0 - ddec * scale;
    
    return {x, y};
}

std::string ChartGenerator::getStarColor(double colorIndex) const {
    if (!config_.style.useStarColors) {
        return config_.style.starColor;
    }
    
    if (colorIndex < -0.1) return config_.style.colorBlue;
    if (colorIndex < 0.3) return config_.style.colorWhite;
    if (colorIndex < 0.8) return config_.style.colorYellow;
    if (colorIndex < 1.4) return config_.style.colorOrange;
    return config_.style.colorRed;
}

double ChartGenerator::getStarRadius(double magnitude) const {
    // Aumentata la differenza tra le magnitudini (da 0.8 a 1.8)
    double r = (config_.style.maxStarSize - magnitude * 1.8) * config_.style.starSizeMultiplier;
    return std::max(config_.style.minStarSize, std::min(config_.style.maxStarSize, r));
}

bool ChartGenerator::generateSVG(const std::string& path) {
    std::ofstream svg(path);
    if (!svg.is_open()) {
        lastError_ = "Cannot create file: " + path;
        return false;
    }
    
    const auto& s = config_.style;
    
    // Header SVG
    svg << R"(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" width=")" << config_.width 
        << R"(" height=")" << config_.height << R"(">
  <defs>
    <radialGradient id="starGlow" cx="50%" cy="50%" r="50%">
      <stop offset="0%" style="stop-color:)" << (s.printable ? "black" : "white") << R"(;stop-opacity:1"/>
      <stop offset="100%" style="stop-color:)" << (s.printable ? "black" : "white") << R"(;stop-opacity:0"/>
    </radialGradient>
  </defs>
)";
    
    // Sfondo
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"" << s.backgroundColor << "\"/>\n";
    
    // Margini per area di disegno (lascia spazio per etichette coordinate)
    int margin = 60;
    int chartX = margin;
    int chartY = margin;
    int chartW = config_.width - 2 * margin;
    int chartH = config_.height - 2 * margin;
    
    // Bordo quadrato con clip path
    svg << "\n  <!-- Chart area border -->\n";
    svg << "  <defs>\n";
    svg << "    <clipPath id=\"chartArea\">\n";
    svg << "      <rect x=\"" << chartX << "\" y=\"" << chartY << "\" width=\"" << chartW << "\" height=\"" << chartH << "\"/>\n";
    svg << "    </clipPath>\n";
    svg << "  </defs>\n";
    
    if (config_.showBorder) {
        svg << "  <rect x=\"" << chartX << "\" y=\"" << chartY << "\" width=\"" << chartW 
            << "\" height=\"" << chartH << "\" fill=\"none\" stroke=\"" 
            << s.borderColor << "\" stroke-width=\"" << s.borderWidth << "\"/>\n";
    }
    
    // Titolo
    if (!config_.title.empty()) {
        svg << "  <text x=\"" << config_.width / 2 << "\" y=\"25\" text-anchor=\"middle\" "
            << "font-family=\"" << s.fontFamily << "\" font-size=\"" << s.titleFontSize 
            << "\" fill=\"" << s.titleColor << "\" font-weight=\"bold\">" << config_.title << "</text>\n";
    }
    
    if (!config_.subtitle.empty()) {
        svg << "  <text x=\"" << config_.width / 2 << "\" y=\"45\" text-anchor=\"middle\" "
            << "font-family=\"" << s.fontFamily << "\" font-size=\"" << s.subtitleFontSize 
            << "\" fill=\"" << (s.printable ? "#666666" : "#aaaaaa") << "\">" 
            << config_.subtitle << "</text>\n";
    }
    
    // Helper per conversione RA gradi -> ore:minuti
    auto raToHMS = [](double raDeg) -> std::string {
        double hours = raDeg / 15.0;
        int h = static_cast<int>(hours);
        int m = static_cast<int>((hours - h) * 60);
        std::ostringstream oss;
        oss << h << "h" << std::setw(2) << std::setfill('0') << m << "m";
        return oss.str();
    };
    
    // Scala per proiezione nell'area del chart
    double scale = chartW / (2.0 * config_.fieldRadius);
    
    // Lambda per proiezione nel chart area
    auto projectToChart = [&](double ra, double dec) -> std::pair<double, double> {
        double dra = (ra - config_.centerRA) * std::cos(config_.centerDec * M_PI / 180.0);
        double ddec = dec - config_.centerDec;
        double x = chartX + chartW / 2.0 - dra * scale;
        double y = chartY + chartH / 2.0 - ddec * scale;
        return {x, y};
    };
    
    // Griglia e coordinate sugli assi
    // Calcola intervalli griglia per RA e Dec
    double decStart = std::floor((config_.centerDec - config_.fieldRadius) / config_.gridInterval) * config_.gridInterval;
    
    // Calcola intervallo RA per formare quadrati, basato su declinazione media
    double raInterval = config_.gridIntervalRA;
    if (raInterval <= 0.0) {
        // Auto: compensa per proiezione a questa declinazione
        double cosDec = std::cos(config_.centerDec * M_PI / 180.0);
        if (cosDec > 0.1) {
            raInterval = config_.gridInterval / cosDec;
        } else {
            raInterval = config_.gridInterval;
        }
        // Arrotonda a multiplo di 5 o 10
        if (raInterval >= 10.0) {
            raInterval = std::round(raInterval / 10.0) * 10.0;
        } else {
            raInterval = std::round(raInterval / 5.0) * 5.0;
        }
        if (raInterval < 5.0) raInterval = 5.0;
    }
    double raStart = std::floor((config_.centerRA - config_.fieldRadius) / raInterval) * raInterval;
    
    if (config_.showGrid) {
        svg << "\n  <!-- Grid and axis labels -->\n";
        svg << "  <g clip-path=\"url(#chartArea)\">\n";
        svg << "    <g stroke=\"" << s.gridColor << "\" stroke-width=\"" << s.gridLineWidth 
            << "\" fill=\"none\" opacity=\"" << s.gridOpacity << "\">\n";
        
        // Linee declinazione orizzontali
        for (double dec = decStart; dec <= config_.centerDec + config_.fieldRadius + 0.1; dec += config_.gridInterval) {
            auto [x1, y1] = projectToChart(config_.centerRA - config_.fieldRadius * 1.5, dec);
            auto [x2, y2] = projectToChart(config_.centerRA + config_.fieldRadius * 1.5, dec);
            svg << "      <line x1=\"" << x1 << "\" y1=\"" << y1 
                << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
        }
        
        // Linee ascensione retta verticali
        for (double ra = raStart; ra <= config_.centerRA + config_.fieldRadius + 0.1; ra += raInterval) {
            auto [x1, y1] = projectToChart(ra, config_.centerDec - config_.fieldRadius * 1.5);
            auto [x2, y2] = projectToChart(ra, config_.centerDec + config_.fieldRadius * 1.5);
            svg << "      <line x1=\"" << x1 << "\" y1=\"" << y1 
                << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
        }
        svg << "    </g>\n";
        svg << "  </g>\n";
        
        // Etichette coordinate sugli assi (fuori dal clip)
        svg << "  <g font-family=\"" << s.fontFamily << "\" font-size=\"9\" fill=\"" << s.titleColor << "\">\n";
        
        // Etichette Dec sul bordo sinistro
        for (double dec = decStart; dec <= config_.centerDec + config_.fieldRadius + 0.1; dec += config_.gridInterval) {
            auto [x, y] = projectToChart(config_.centerRA, dec);
            if (y >= chartY && y <= chartY + chartH) {
                // Tacca
                svg << "    <line x1=\"" << chartX - 5 << "\" y1=\"" << y 
                    << "\" x2=\"" << chartX << "\" y2=\"" << y 
                    << "\" stroke=\"" << s.borderColor << "\" stroke-width=\"1\"/>\n";
                // Etichetta
                svg << "    <text x=\"" << chartX - 8 << "\" y=\"" << y + 3 
                    << "\" text-anchor=\"end\">" << (dec >= 0 ? "+" : "") 
                    << static_cast<int>(dec) << "°</text>\n";
            }
        }
        
        // Etichette RA sul bordo inferiore (in ore)
        for (double ra = raStart; ra <= config_.centerRA + config_.fieldRadius + 0.1; ra += raInterval) {
            auto [x, y] = projectToChart(ra, config_.centerDec);
            if (x >= chartX && x <= chartX + chartW) {
                // Tacca
                svg << "    <line x1=\"" << x << "\" y1=\"" << chartY + chartH 
                    << "\" x2=\"" << x << "\" y2=\"" << chartY + chartH + 5 
                    << "\" stroke=\"" << s.borderColor << "\" stroke-width=\"1\"/>\n";
                // Etichetta in ore
                svg << "    <text x=\"" << x << "\" y=\"" << chartY + chartH + 18 
                    << "\" text-anchor=\"middle\">" << raToHMS(ra) << "</text>\n";
            }
        }
        
        // Label assi
        svg << "    <text x=\"" << chartX - 45 << "\" y=\"" << chartY + chartH/2 
            << "\" text-anchor=\"middle\" transform=\"rotate(-90 " << chartX - 45 << " " << chartY + chartH/2 
            << ")\" font-size=\"11\" font-weight=\"bold\">Declination</text>\n";
        svg << "    <text x=\"" << chartX + chartW/2 << "\" y=\"" << chartY + chartH + 35 
            << "\" text-anchor=\"middle\" font-size=\"11\" font-weight=\"bold\">Right Ascension</text>\n";
        
        svg << "  </g>\n";
    }
    
    // Riquadro esterno
    if (config_.showBorder) {
        svg << "\n  <!-- Border frame -->\n";
        svg << "  <rect x=\"" << chartX << "\" y=\"" << chartY 
            << "\" width=\"" << chartW << "\" height=\"" << chartH 
            << "\" fill=\"none\" stroke=\"" << s.borderColor 
            << "\" stroke-width=\"" << s.borderWidth << "\"/>\n";
    }
    
    // Confini costellazioni (tratteggiati)
    if (config_.showConstellationBoundaries) {
        svg << "\n  <!-- Constellation boundaries -->\n";
        svg << "  <g clip-path=\"url(#chartArea)\" stroke=\"" << s.constellationBoundaryColor << "\" stroke-width=\"" 
            << s.constellationBoundaryWidth << "\" fill=\"none\" opacity=\"" 
            << s.constellationBoundaryOpacity << "\" stroke-dasharray=\"5,3\">\n";
        
        // Itera su tutti i confini disponibili
        for (const auto& boundary : CONSTELLATION_BOUNDARIES) {
            // Controlla se almeno un segmento è nel campo visivo
            for (const auto& seg : boundary.segments) {
                auto [x1, y1] = projectToChart(seg.ra1, seg.dec1);
                auto [x2, y2] = projectToChart(seg.ra2, seg.dec2);
                
                // Disegna se almeno un estremo è nel campo
                bool inField1 = (x1 >= chartX && x1 <= chartX + chartW && y1 >= chartY && y1 <= chartY + chartH);
                bool inField2 = (x2 >= chartX && x2 <= chartX + chartW && y2 >= chartY && y2 <= chartY + chartH);
                if (inField1 || inField2) {
                    svg << "    <line x1=\"" << x1 << "\" y1=\"" << y1 
                        << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
                }
            }
        }
        svg << "  </g>\n";
    }
    
    // Linee costellazione - TUTTE le costellazioni nel campo visivo
    if (config_.showConstellationLines) {
        svg << "\n  <!-- Constellation lines -->\n";
        svg << "  <g clip-path=\"url(#chartArea)\" stroke=\"" << s.constellationLineColor << "\" stroke-width=\"" 
            << s.constellationLineWidth << "\" fill=\"none\" opacity=\"" 
            << s.constellationLineOpacity << "\">\n";
        
        // Itera su tutte le costellazioni disponibili
        for (const auto& constName : getAvailableConstellations()) {
            auto constData = getConstellationData(constName);
            if (!constData.has_value()) continue;
            
            // Controlla se la costellazione è nel campo visivo
            double dra = std::abs(constData->centerRA - config_.centerRA);
            if (dra > 180) dra = 360 - dra;
            double ddec = std::abs(constData->centerDec - config_.centerDec);
            
            // Skip se troppo lontana (con margine generoso)
            if (dra > config_.fieldRadius * 2 || ddec > config_.fieldRadius * 2) continue;
            
            // Disegna le linee
            for (const auto& line : constData->lines) {
                auto [x1, y1] = projectToChart(line.ra1, line.dec1);
                auto [x2, y2] = projectToChart(line.ra2, line.dec2);
                svg << "    <line x1=\"" << x1 << "\" y1=\"" << y1 
                    << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
            }
        }
        svg << "  </g>\n";
        
        // Nomi costellazioni
        svg << "  <g font-family=\"" << s.fontFamily << "\" font-size=\"11\" fill=\"" 
            << s.constellationLineColor << "\" font-style=\"italic\">\n";
        for (const auto& constName : getAvailableConstellations()) {
            auto constData = getConstellationData(constName);
            if (!constData.has_value()) continue;
            
            auto [cx, cy] = projectToChart(constData->centerRA, constData->centerDec);
            if (cx >= chartX && cx <= chartX + chartW && cy >= chartY && cy <= chartY + chartH) {
                svg << "    <text x=\"" << cx << "\" y=\"" << cy << "\" text-anchor=\"middle\">"
                    << constData->abbreviation << "</text>\n";
            }
        }
        svg << "  </g>\n";
    }
    
    // Stelle
    svg << "\n  <!-- Stars (" << stars_.size() << " total) -->\n";
    svg << "  <g clip-path=\"url(#chartArea)\">\n";
    
    // Ordina per magnitudine (più deboli prima)
    std::vector<std::shared_ptr<core::Star>> sortedStars = stars_;
    std::sort(sortedStars.begin(), sortedStars.end(),
              [](const auto& a, const auto& b) {
                  return a->getMagnitude() > b->getMagnitude();
              });
    
    int starCount = 0;
    for (const auto& star : sortedStars) {
        double ra = star->getCoordinates().getRightAscension();
        double dec = star->getCoordinates().getDeclination();
        double mag = star->getMagnitude();
        
        auto [x, y] = projectToChart(ra, dec);
        
        // Salta stelle fuori campo
        if (x < chartX - 10 || x > chartX + chartW + 10 || 
            y < chartY - 10 || y > chartY + chartH + 10) continue;
        
        double r = getStarRadius(mag);
        starCount++;
        
        // Colore stella
        std::string color = s.starColor;
        if (s.useStarColors) {
            auto ciOpt = star->getColorIndex();
            if (ciOpt.has_value()) {
                color = getStarColor(ciOpt.value());
            }
        }
        
        // Alone per stelle luminose (solo in modalità non stampabile)
        if (!s.printable && mag < 3.0) {
            svg << "    <circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r * 2.5 
                << "\" fill=\"url(#starGlow)\"/>\n";
        }
        
        svg << "    <circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r 
            << "\" fill=\"" << color << "\" opacity=\"" << s.starOpacity << "\"/>\n";
    }
    svg << "  </g>\n";
    
    // Etichette stelle (nomi comuni e designazioni Bayer/Flamsteed)
    if (config_.showStarLabels) {
        svg << "\n  <!-- Star labels (common names, Flamsteed/Bayer) -->\n";
        svg << "  <g font-family=\"" << s.fontFamily << "\" font-size=\"" << s.labelFontSize 
            << "\" fill=\"" << s.labelColor << "\" font-weight=\"bold\">\n";
        
        int labelCount = 0;
        for (const auto& star : sortedStars) {
            double mag = star->getMagnitude();
            if (mag > config_.labelMagnitudeLimit) continue;
            
            double ra = star->getCoordinates().getRightAscension();
            double dec = star->getCoordinates().getDeclination();
            auto [x, y] = projectToChart(ra, dec);
            
            if (x < chartX + 20 || x > chartX + chartW - 20 || 
                y < chartY + 20 || y > chartY + chartH - 20) continue;
            
            std::string starName = star->getName();
            if (starName.empty()) continue;
            
            // Filtra: mostra solo nomi comuni (non numeri Gaia/HD/HIP)
            if (starName.find("Gaia") == 0 || 
                starName.find("HD ") == 0 || 
                starName.find("HIP ") == 0 ||
                starName.find("TYC ") == 0) {
                continue;
            }
            
            double r = getStarRadius(mag);
            svg << "    <text x=\"" << x + r + 3 << "\" y=\"" << y - 2 << "\">" 
                << starName << "</text>\n";
            
            if (++labelCount > 50) break;
        }
        svg << "  </g>\n";
    }
    
    // Numeri SAO (solo per stelle SENZA nome proprio)
    if (config_.showSAONumbers) {
        svg << "\n  <!-- SAO numbers (only for unnamed stars) -->\n";
        svg << "  <g font-family=\"" << s.fontFamily << "\" font-size=\"" << s.saoFontSize 
            << "\" fill=\"" << s.labelColor << "\" opacity=\"0.7\">\n";
        
        int saoCount = 0;
        for (const auto& star : sortedStars) {
            double mag = star->getMagnitude();
            if (mag > config_.saoMagnitudeLimit) continue;
            
            // Salta se ha un nome proprio
            std::string starName = star->getName();
            bool hasProperName = !starName.empty() && 
                                starName.find("Gaia") != 0 && 
                                starName.find("HD ") != 0 && 
                                starName.find("HIP ") != 0 &&
                                starName.find("TYC ") != 0;
            if (hasProperName) continue;
            
            // Verifica se ha numero SAO
            auto saoOpt = star->getSAONumber();
            if (!saoOpt.has_value()) continue;
            
            double ra = star->getCoordinates().getRightAscension();
            double dec = star->getCoordinates().getDeclination();
            auto [x, y] = projectToChart(ra, dec);
            
            if (x < chartX + 20 || x > chartX + chartW - 20 || 
                y < chartY + 20 || y > chartY + chartH - 20) continue;
            
            double r = getStarRadius(mag);
            svg << "    <text x=\"" << x + r + 3 << "\" y=\"" << y - 2 << "\">SAO " 
                << saoOpt.value() << "</text>\n";
            
            if (++saoCount > 100) break;
        }
        svg << "  </g>\n";
    }
    
    // Etichette stelle nominate (da dati costellazioni hardcoded)
    if (config_.showStarLabels) {
        svg << "\n  <!-- Named star labels -->\n";
        svg << "  <g font-family=\"" << s.fontFamily << "\" font-size=\"" << s.labelFontSize 
            << "\" fill=\"" << s.labelColor << "\" font-weight=\"bold\">\n";
        
        // Mostra nomi per tutte le costellazioni nel campo
        for (const auto& constName : getAvailableConstellations()) {
            auto constData = getConstellationData(constName);
            if (!constData.has_value()) continue;
            
            for (const auto& [name, coords] : constData->namedStars) {
                auto [x, y] = projectToChart(coords.first, coords.second);
                if (x >= chartX + 20 && x <= chartX + chartW - 40 && 
                    y >= chartY + 20 && y <= chartY + chartH - 20) {
                    svg << "    <text x=\"" << x + 8 << "\" y=\"" << y + 4 << "\">"
                        << name << "</text>\n";
                }
            }
        }
        svg << "  </g>\n";
    }
    
    // Scala angolare
    if (config_.showScaleBar) {
        svg << "\n  <!-- Scale bar -->\n";
        double scaleLength = config_.fieldRadius / 3.0;  // 1/3 del campo
        double scalePixels = scaleLength * (config_.width / (2.0 * config_.fieldRadius));
        int scaleY = config_.height - 55;
        int scaleX = 30;
        
        svg << "  <line x1=\"" << scaleX << "\" y1=\"" << scaleY 
            << "\" x2=\"" << scaleX + scalePixels << "\" y2=\"" << scaleY 
            << "\" stroke=\"" << s.titleColor << "\" stroke-width=\"2\"/>\n";
        svg << "  <line x1=\"" << scaleX << "\" y1=\"" << scaleY - 5 
            << "\" x2=\"" << scaleX << "\" y2=\"" << scaleY + 5 
            << "\" stroke=\"" << s.titleColor << "\" stroke-width=\"2\"/>\n";
        svg << "  <line x1=\"" << scaleX + scalePixels << "\" y1=\"" << scaleY - 5 
            << "\" x2=\"" << scaleX + scalePixels << "\" y2=\"" << scaleY + 5 
            << "\" stroke=\"" << s.titleColor << "\" stroke-width=\"2\"/>\n";
        
        std::ostringstream scaleLbl;
        scaleLbl << std::fixed << std::setprecision(1) << scaleLength << "°";
        svg << "  <text x=\"" << scaleX + scalePixels / 2 << "\" y=\"" << scaleY - 8 
            << "\" text-anchor=\"middle\" font-family=\"" << s.fontFamily 
            << "\" font-size=\"10\" fill=\"" << s.titleColor << "\">" 
            << scaleLbl.str() << "</text>\n";
    }
    
    // Freccia Nord
    if (config_.showNorthArrow) {
        svg << "\n  <!-- North arrow -->\n";
        int arrowX = config_.width - 50;
        int arrowY = config_.height - 60;
        
        svg << "  <polygon points=\"" << arrowX << "," << arrowY - 20 
            << " " << arrowX - 8 << "," << arrowY 
            << " " << arrowX + 8 << "," << arrowY 
            << "\" fill=\"" << s.titleColor << "\"/>\n";
        svg << "  <text x=\"" << arrowX << "\" y=\"" << arrowY + 15 
            << "\" text-anchor=\"middle\" font-family=\"" << s.fontFamily 
            << "\" font-size=\"12\" font-weight=\"bold\" fill=\"" 
            << s.titleColor << "\">N</text>\n";
    }
    
    // Generic Target Box Indicator
    if (config_.targetBoxSize > 0.0) {
        svg << "\n  <!-- Target Box Indicator -->\n";
        
        // Use configured size
        double boxSizeDegrees = config_.targetBoxSize;
        double halfBox = boxSizeDegrees / 2.0;

        // Calcola dimensioni in pixel
        double pixelsPerDegree = chartW / (2.0 * config_.fieldRadius);
        double boxPixelW = config_.targetBoxSize * pixelsPerDegree;
        double boxPixelH = config_.targetBoxSize * pixelsPerDegree;

        // Centro del rettangolo (centro del chart)
        double centerX = chartX + chartW / 2.0;
        double centerY = chartY + chartH / 2.0;
        
        // Coordinate per il rettangolo
        double x1 = centerX - boxPixelW/2;
        double y1 = centerY - boxPixelH/2;
        double x2 = centerX + boxPixelW/2;
        double y2 = centerY + boxPixelH/2;
        
        std::string boxColor = "rgb(204,0,0)";
        
        // Draw 4 lines for the box
        svg << "  <line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y1 
            << "\" stroke=\"" << boxColor << "\" stroke-width=\"2\" stroke-dasharray=\"5,3\"/>\n";
        svg << "  <line x1=\"" << x2 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 
            << "\" stroke=\"" << boxColor << "\" stroke-width=\"2\" stroke-dasharray=\"5,3\"/>\n";
        svg << "  <line x1=\"" << x2 << "\" y1=\"" << y2 << "\" x2=\"" << x1 << "\" y2=\"" << y2 
            << "\" stroke=\"" << boxColor << "\" stroke-width=\"2\" stroke-dasharray=\"5,3\"/>\n";
        svg << "  <line x1=\"" << x1 << "\" y1=\"" << y2 << "\" x2=\"" << x1 << "\" y2=\"" << y1 
            << "\" stroke=\"" << boxColor << "\" stroke-width=\"2\" stroke-dasharray=\"5,3\"/>\n";
        
        // Optional label logic could go here, but omitted to keep it clean/generic
    }
    
    // Legenda
    if (config_.showLegend) {
        int legendY = config_.height - 35;
        int legendX = config_.showScaleBar ? 150 : 20;
        
        svg << "\n  <!-- Legend -->\n";
        svg << "  <g transform=\"translate(" << legendX << ", " << legendY << ")\">\n";
        
        std::string legendStarColor = s.printable ? "#000000" : "#ffffff";
        std::string legendTextColor = s.printable ? "#333333" : "#aaaaaa";
        
        svg << "    <circle cx=\"10\" cy=\"0\" r=\"4\" fill=\"" << legendStarColor << "\"/>\n";
        svg << "    <text x=\"20\" y=\"4\" font-family=\"" << s.fontFamily 
            << "\" font-size=\"9\" fill=\"" << legendTextColor << "\">Mag 1-2</text>\n";
        
        svg << "    <circle cx=\"80\" cy=\"0\" r=\"2.5\" fill=\"" << legendStarColor << "\"/>\n";
        svg << "    <text x=\"90\" y=\"4\" font-family=\"" << s.fontFamily 
            << "\" font-size=\"9\" fill=\"" << legendTextColor << "\">Mag 3-4</text>\n";
        
        svg << "    <circle cx=\"150\" cy=\"0\" r=\"1.5\" fill=\"" << legendStarColor << "\"/>\n";
        svg << "    <text x=\"160\" y=\"4\" font-family=\"" << s.fontFamily 
            << "\" font-size=\"9\" fill=\"" << legendTextColor << "\">Mag 5-6</text>\n";
        
        if (config_.maxMagnitude > 8) {
            svg << "    <circle cx=\"220\" cy=\"0\" r=\"0.8\" fill=\"" << legendStarColor << "\"/>\n";
            svg << "    <text x=\"230\" y=\"4\" font-family=\"" << s.fontFamily 
                << "\" font-size=\"9\" fill=\"" << legendTextColor << "\">Mag >8</text>\n";
        }
        
        svg << "  </g>\n";
    }
    
    // Scala magnitudini (sul lato destro)
    svg << "\n  <!-- Magnitude scale -->\n";
    svg << "  <g transform=\"translate(" << config_.width - 80 << ", " << chartY + 30 << ")\" font-family=\"" << s.fontFamily << "\" font-size=\"9\">\n";
    
    std::string scaleTextColor = s.printable ? "#333333" : "#aaaaaa";
    std::string scaleStarColor = s.printable ? "#000000" : "#ffffff";
    
    svg << "    <text x=\"30\" y=\"-10\" text-anchor=\"middle\" font-weight=\"bold\" fill=\"" << scaleTextColor << "\">Magnitude</text>\n";
    
    // Disegna stelle di esempio per diverse magnitudini
    for (int mag = 1; mag <= 7; mag++) {
        double yPos = (mag - 1) * 25;
        double r = getStarRadius(static_cast<double>(mag));
        
        // Cerchio stella
        svg << "    <circle cx=\"15\" cy=\"" << yPos << "\" r=\"" << r << "\" fill=\"" << scaleStarColor << "\" opacity=\"" << s.starOpacity << "\"/>\n";
        
        // Label magnitudine
        svg << "    <text x=\"30\" y=\"" << yPos + 4 << "\" fill=\"" << scaleTextColor << "\">" << mag << ".0</text>\n";
    }
    
    svg << "  </g>\n";
    
    // Info (stelle totali)
    svg << "\n  <!-- Info -->\n";
    svg << "  <text x=\"" << config_.width - 10 << "\" y=\"" << config_.height - 10 
        << "\" text-anchor=\"end\" font-family=\"" << s.fontFamily 
        << "\" font-size=\"8\" fill=\"" << (s.printable ? "#666666" : "#555555") 
        << "\">" << starCount << " stars | RA " << std::fixed << std::setprecision(2)
        << config_.centerRA << "° Dec " << (config_.centerDec >= 0 ? "+" : "") 
        << config_.centerDec << "° | FOV " << config_.fieldRadius << "°</text>\n";
    
    // Footer
    svg << "</svg>\n";
    svg.close();
    
    return true;
}

bool ChartGenerator::convertToPNG(const std::string& svgPath, const std::string& pngPath) {
    // Prima prova rsvg-convert (migliore qualità SVG)
    std::string cmd = "rsvg-convert \"" + svgPath + "\" -o \"" + pngPath + "\" 2>/dev/null";
    int result = system(cmd.c_str());
    
    if (result != 0) {
        // Fallback a ImageMagick
        cmd = "magick \"" + svgPath + "\" -density " + std::to_string(config_.pngDensity) 
            + " \"" + pngPath + "\" 2>/dev/null";
        result = system(cmd.c_str());
    }
    
    if (result != 0) {
        // Prova vecchia sintassi ImageMagick
        cmd = "convert \"" + svgPath + "\" -density " + std::to_string(config_.pngDensity) 
            + " \"" + pngPath + "\" 2>/dev/null";
        result = system(cmd.c_str());
    }
    
    return result == 0;
}

bool ChartGenerator::convertToJPG(const std::string& svgPath, const std::string& jpgPath) {
    std::string cmd = "magick \"" + svgPath + "\" -density " + std::to_string(config_.pngDensity) 
                    + " -quality 90 \"" + jpgPath + "\" 2>/dev/null";
    int result = system(cmd.c_str());
    
    if (result != 0) {
        cmd = "convert \"" + svgPath + "\" -density " + std::to_string(config_.pngDensity) 
            + " -quality 90 \"" + jpgPath + "\" 2>/dev/null";
        result = system(cmd.c_str());
    }
    
    return result == 0;
}

std::optional<ConstellationData> ChartGenerator::getConstellationData(const std::string& name) {
    // Cerca case-insensitive
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    for (const auto& [key, data] : ALL_CONSTELLATIONS) {
        std::string keyLower = key;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
        if (keyLower == lower) {
            return data;
        }
    }
    
    return std::nullopt;
}

std::vector<std::string> ChartGenerator::getAvailableConstellations() {
    std::vector<std::string> names;
    for (const auto& [key, data] : ALL_CONSTELLATIONS) {
        names.push_back(key);
    }
    return names;
}

std::vector<std::string> ChartGenerator::getAvailablePresets() {
    return {"default", "visual", "finder", "detail", "print"};
}

void ChartGenerator::applyPreset(ChartPreset preset) {
    switch (preset) {
        case ChartPreset::Visual:
            // Visuale: colori temperatura, cielo scuro
            config_.style.backgroundColor = "#0a0a20";
            config_.style.useStarColors = true;
            config_.style.starOpacity = 0.95;
            config_.showConstellationLines = true;
            config_.showGrid = true;
            config_.style.gridOpacity = 0.3;
            break;
            
        case ChartPreset::FinderChart:
            // Carta puntamento: stampabile, campo largo
            config_.fieldRadius = 25.0;
            config_.maxMagnitude = 8.0;
            config_.width = 1200;
            config_.height = 1200;
            config_.pngDensity = 200;
            
            // Stile stampabile
            config_.style.printable = true;
            config_.style.backgroundColor = "#ffffff";
            config_.style.starColor = "#000000";
            config_.style.gridColor = "#aaaaaa";
            config_.style.constellationLineColor = "#444444";
            config_.style.constellationBoundaryColor = "#666666";  // Più scuro per visibilità
            config_.style.labelColor = "#333333";
            config_.style.titleColor = "#000000";
            config_.style.borderColor = "#000000";
            config_.style.useStarColors = false;
            config_.style.gridOpacity = 0.6;
            config_.style.constellationLineOpacity = 0.8;
            config_.style.constellationBoundaryOpacity = 0.7;  // Più opaco per visibilità
            config_.style.constellationBoundaryWidth = 1.0;    // Linee più spesse
            config_.style.borderWidth = 2.0;
            
            // Elementi
            config_.showGrid = true;
            config_.showConstellationLines = true;
            config_.showConstellationBoundaries = true;
            config_.showBorder = true;
            config_.showScaleBar = true;
            config_.showNorthArrow = true;
            config_.showSAONumbers = true;
            config_.saoMagnitudeLimit = 7.0;
            config_.showStarLabels = true;
            config_.labelMagnitudeLimit = 5.0;
            config_.showLegend = true;
            config_.gridInterval = 5.0;
            break;
            
        case ChartPreset::DetailChart:
            // Carta dettaglio: stampabile, campo stretto, magnitudine profonda
            config_.fieldRadius = 2.0;
            config_.maxMagnitude = 16.0;
            config_.width = 1200;
            config_.height = 1200;
            config_.pngDensity = 200;
            
            // Stile stampabile
            config_.style.printable = true;
            config_.style.backgroundColor = "#ffffff";
            config_.style.starColor = "#000000";
            config_.style.gridColor = "#cccccc";
            config_.style.labelColor = "#444444";
            config_.style.titleColor = "#000000";
            config_.style.borderColor = "#000000";
            config_.style.useStarColors = false;
            config_.style.gridOpacity = 0.5;
            config_.style.borderWidth = 1.5;
            
            // Stelle più piccole per il campo denso
            config_.style.starSizeMultiplier = 0.6;
            config_.style.minStarSize = 0.3;
            config_.style.maxStarSize = 4.0;
            
            // Elementi
            config_.showGrid = true;
            config_.showConstellationLines = false;  // Non serve in campo stretto
            config_.showConstellationBoundaries = false;
            config_.showBorder = true;
            config_.showScaleBar = true;
            config_.showNorthArrow = true;
            config_.showSAONumbers = false;  // Troppo denso
            config_.showStarLabels = false;  // Troppo denso
            config_.showLegend = true;
            config_.gridInterval = 0.5;  // Griglia fine
            break;
            
        case ChartPreset::PrintBlack:
        case ChartPreset::PrintInverse:
            // Stampabile generico
            config_.style.printable = true;
            config_.style.backgroundColor = "#ffffff";
            config_.style.starColor = "#000000";
            config_.style.gridColor = "#aaaaaa";
            config_.style.constellationLineColor = "#555555";
            config_.style.labelColor = "#333333";
            config_.style.titleColor = "#000000";
            config_.style.useStarColors = false;
            config_.showBorder = true;
            break;
            
        case ChartPreset::Default:
        default:
            // Default: cielo scuro standard
            config_.style.backgroundColor = "#0a0a20";
            config_.style.starColor = "#ffffff";
            config_.style.useStarColors = true;
            break;
    }
    
    config_.preset = preset;
}

ChartConfig ChartGenerator::createFinderChart(double ra, double dec, double fieldDegrees) {
    ChartConfig config;
    config.centerRA = ra;
    config.centerDec = dec;
    config.fieldRadius = fieldDegrees;
    config.preset = ChartPreset::FinderChart;
    
    // Applica stile finder
    config.maxMagnitude = 8.0;
    config.width = 1200;
    config.height = 1200;
    config.pngDensity = 200;
    
    // Stile stampabile
    config.style.printable = true;
    config.style.backgroundColor = "#ffffff";
    config.style.starColor = "#000000";
    config.style.gridColor = "#aaaaaa";
    config.style.constellationLineColor = "#444444";
    config.style.constellationBoundaryColor = "#666666";  // Più scuro per visibilità
    config.style.labelColor = "#333333";
    config.style.titleColor = "#000000";
    config.style.borderColor = "#000000";
    config.style.useStarColors = false;
    config.style.gridOpacity = 0.6;
    config.style.constellationBoundaryOpacity = 0.7;  // Più opaco
    config.style.constellationBoundaryWidth = 1.0;    // Più spesso
    config.style.borderWidth = 2.0;
    
    config.showGrid = true;
    config.showConstellationLines = true;
    config.showConstellationBoundaries = true;
    config.showBorder = true;
    config.showScaleBar = true;
    config.showNorthArrow = true;
    config.showSAONumbers = true;
    config.saoMagnitudeLimit = 7.0;
    config.showStarLabels = true;
    config.labelMagnitudeLimit = 5.0;
    config.showLegend = true;
    config.gridInterval = 5.0;
    
    config.outputFormat = "png";
    
    // Titolo automatico
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Finder Chart - RA " << ra << "° Dec " << (dec >= 0 ? "+" : "") << dec << "°";
    config.title = oss.str();
    config.subtitle = "Field: " + std::to_string((int)fieldDegrees) + "° - Mag ≤ 8.0";
    
    return config;
}

ChartConfig ChartGenerator::createDetailChart(double ra, double dec, 
                                               double fieldDegrees, double maxMag) {
    ChartConfig config;
    config.centerRA = ra;
    config.centerDec = dec;
    config.fieldRadius = fieldDegrees;
    config.maxMagnitude = maxMag;
    config.preset = ChartPreset::DetailChart;
    
    config.width = 1200;
    config.height = 1200;
    config.pngDensity = 200;
    
    // Stile stampabile
    config.style.printable = true;
    config.style.backgroundColor = "#ffffff";
    config.style.starColor = "#000000";
    config.style.gridColor = "#cccccc";
    config.style.labelColor = "#444444";
    config.style.titleColor = "#000000";
    config.style.borderColor = "#000000";
    config.style.useStarColors = false;
    config.style.gridOpacity = 0.5;
    config.style.borderWidth = 1.5;
    
    // Stelle piccole
    config.style.starSizeMultiplier = 0.6;
    config.style.minStarSize = 0.3;
    config.style.maxStarSize = 4.0;
    
    config.showGrid = true;
    config.showConstellationLines = false;
    config.showConstellationBoundaries = false;
    config.showBorder = true;
    config.showScaleBar = true;
    config.showNorthArrow = true;
    config.showSAONumbers = false;
    config.showStarLabels = false;
    config.showLegend = true;
    config.gridInterval = std::max(0.25, fieldDegrees / 8.0);  // Griglia adattiva
    
    config.outputFormat = "png";
    
    // Titolo automatico
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Detail Chart - RA " << ra << "° Dec " << (dec >= 0 ? "+" : "") << dec << "°";
    config.title = oss.str();
    
    std::ostringstream sub;
    sub << std::fixed << std::setprecision(1);
    sub << "Field: " << fieldDegrees << "° - Mag ≤ " << maxMag;
    config.subtitle = sub.str();
    
    return config;
}

// ============================================================================
// JSON Parser (semplice, senza dipendenze esterne)
// ============================================================================

bool ChartGenerator::parseJSON(const std::string& json) {
    try {
        // Helper per estrarre valori
        auto getString = [&](const std::string& key) -> std::string {
            std::regex re("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
            std::smatch match;
            if (std::regex_search(json, match, re)) {
                return match[1].str();
            }
            return "";
        };
        
        auto getNumber = [&](const std::string& key) -> std::optional<double> {
            std::regex re("\"" + key + "\"\\s*:\\s*([\\d.\\-]+)");
            std::smatch match;
            if (std::regex_search(json, match, re)) {
                return std::stod(match[1].str());
            }
            return std::nullopt;
        };
        
        auto getBool = [&](const std::string& key) -> std::optional<bool> {
            std::regex re("\"" + key + "\"\\s*:\\s*(true|false)");
            std::smatch match;
            if (std::regex_search(json, match, re)) {
                return match[1].str() == "true";
            }
            return std::nullopt;
        };
        
        // Se c'è un eventuale preset, applicalo per primi (verrà sovrascritto dai campi successivi se presenti)
        auto presetStr = getString("preset");
        if (!presetStr.empty()) {
            std::string p = presetStr;
            std::transform(p.begin(), p.end(), p.begin(), ::tolower);
            if (p == "finder") applyPreset(ChartPreset::FinderChart);
            else if (p == "detail") applyPreset(ChartPreset::DetailChart);
            else if (p == "print" || p == "printable") applyPreset(ChartPreset::PrintBlack);
            else if (p == "visual") applyPreset(ChartPreset::Visual);
        }

        // Parse campi principali
        if (auto v = getNumber("width")) config_.width = static_cast<int>(*v);
        if (auto v = getNumber("height")) config_.height = static_cast<int>(*v);
        if (auto v = getNumber("centerRA")) config_.centerRA = *v;
        if (auto v = getNumber("centerDec")) config_.centerDec = *v;
        if (auto v = getNumber("fieldRadius")) config_.fieldRadius = *v;
        if (auto v = getNumber("maxMagnitude")) config_.maxMagnitude = *v;
        if (auto v = getNumber("minMagnitude")) config_.minMagnitude = *v;
        if (auto v = getNumber("gridInterval")) config_.gridInterval = *v;
        if (auto v = getNumber("labelMagnitudeLimit")) config_.labelMagnitudeLimit = *v;
        if (auto v = getNumber("pngDensity")) config_.pngDensity = static_cast<int>(*v);
        
        if (auto v = getBool("showGrid")) config_.showGrid = *v;
        if (auto v = getBool("showConstellationLines")) config_.showConstellationLines = *v;
        if (auto v = getBool("showStarLabels")) config_.showStarLabels = *v;
        if (auto v = getBool("showLegend")) config_.showLegend = *v;
        if (auto v = getBool("showCoordinateLabels")) config_.showCoordinateLabels = *v;
        
        auto title = getString("title");
        if (!title.empty()) config_.title = title;
        
        auto subtitle = getString("subtitle");
        if (!subtitle.empty()) config_.subtitle = subtitle;
        
        auto constellation = getString("constellation");
        if (!constellation.empty()) config_.constellation = constellation;
        
        auto outputFormat = getString("outputFormat");
        if (!outputFormat.empty()) config_.outputFormat = outputFormat;
        
        auto outputPath = getString("outputPath");
        if (!outputPath.empty()) config_.outputPath = outputPath;
        
        auto projection = getString("projection");
        if (!projection.empty()) config_.projection = projection;
        
        // Parse stile
        auto bgColor = getString("backgroundColor");
        if (!bgColor.empty()) config_.style.backgroundColor = bgColor;
        
        auto gridColor = getString("gridColor");
        if (!gridColor.empty()) config_.style.gridColor = gridColor;
        
        auto starColor = getString("starColor");
        if (!starColor.empty()) config_.style.starColor = starColor;
        
        auto labelColor = getString("labelColor");
        if (!labelColor.empty()) config_.style.labelColor = labelColor;
        
        auto constellationLineColor = getString("constellationLineColor");
        if (!constellationLineColor.empty()) config_.style.constellationLineColor = constellationLineColor;
        
        if (auto v = getNumber("starSizeMultiplier")) config_.style.starSizeMultiplier = *v;
        if (auto v = getNumber("gridOpacity")) config_.style.gridOpacity = *v;
        if (auto v = getBool("useStarColors")) config_.style.useStarColors = *v;
        
        // Se c'è una costellazione, usa i suoi default per centro
        if (!config_.constellation.empty()) {
            auto constData = getConstellationData(config_.constellation);
            if (constData.has_value()) {
                // Usa centro costellazione se non specificato
                if (!getNumber("centerRA").has_value()) {
                    config_.centerRA = constData->centerRA;
                }
                if (!getNumber("centerDec").has_value()) {
                    config_.centerDec = constData->centerDec;
                }
                // Titolo default
                if (config_.title.empty()) {
                    config_.title = constData->name + " Constellation";
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        lastError_ = std::string("JSON parse error: ") + e.what();
        return false;
    }
}

} // namespace map
} // namespace starmap
