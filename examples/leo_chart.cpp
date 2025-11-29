/**
 * @file leo_chart.cpp
 * @brief Genera una carta celeste della costellazione del Leone usando il catalogo Gaia
 */

#include "starmap/catalog/GaiaClient.h"
#include "starmap/map/MapRenderer.h"
#include "starmap/map/MapConfiguration.h"
#include "starmap/map/Projection.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace starmap;

// Stelle principali del Leone con coordinate e nomi
struct LeoStar {
    std::string name;
    double ra;      // gradi
    double dec;     // gradi
    double mag;
};

// Le stelle principali della costellazione del Leone
const std::vector<LeoStar> LEO_MAIN_STARS = {
    {"Regulus (α Leo)",     152.093, 11.967,  1.40},
    {"Denebola (β Leo)",    177.265, 14.572,  2.14},
    {"Algieba (γ Leo)",     154.993, 19.842,  2.08},
    {"Zosma (δ Leo)",       168.527, 20.524,  2.56},
    {"Chertan (θ Leo)",     168.560, 15.430,  3.34},
    {"Adhafera (ζ Leo)",    154.173, 23.417,  3.44},
    {"Rasalas (μ Leo)",     148.191, 26.007,  3.88},
    {"Alterf (λ Leo)",      142.930, 22.968,  4.31},
    {"Subra (ο Leo)",       147.883, 9.892,   3.52},
    {"Eta Leo (η Leo)",     151.833, 16.763,  3.52},
};

// Linee della costellazione (indici delle stelle sopra)
const std::vector<std::pair<int, int>> LEO_LINES = {
    {0, 4},   // Regulus - Chertan
    {4, 3},   // Chertan - Zosma
    {3, 1},   // Zosma - Denebola
    {0, 9},   // Regulus - Eta Leo
    {9, 2},   // Eta Leo - Algieba
    {2, 3},   // Algieba - Zosma
    {2, 5},   // Algieba - Adhafera
    {5, 6},   // Adhafera - Rasalas
    {6, 7},   // Rasalas - Alterf
    {0, 8},   // Regulus - Subra
};

// Genera SVG della carta
void generateSVG(const std::vector<std::shared_ptr<core::Star>>& gaiaStars,
                 const std::string& filename,
                 double centerRA, double centerDec, double radius) {
    
    const int width = 800;
    const int height = 800;
    const double scale = width / (2.0 * radius);
    
    std::ofstream svg(filename);
    
    // Header SVG
    svg << R"(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" width=")" << width << R"(" height=")" << height << R"(">
  <defs>
    <radialGradient id="starGlow" cx="50%" cy="50%" r="50%">
      <stop offset="0%" style="stop-color:white;stop-opacity:1"/>
      <stop offset="100%" style="stop-color:white;stop-opacity:0"/>
    </radialGradient>
  </defs>
  
  <!-- Sfondo cielo notturno -->
  <rect width="100%" height="100%" fill="#0a0a20"/>
  
  <!-- Titolo -->
  <text x=")" << width/2 << R"(" y="30" text-anchor="middle" 
        font-family="Arial" font-size="24" fill="white">
    Costellazione del Leone (Leo)
  </text>
  <text x=")" << width/2 << R"(" y="55" text-anchor="middle" 
        font-family="Arial" font-size="14" fill="#aaaaaa">
    Catalogo Gaia DR3 - Stelle fino a mag 6.5
  </text>
)";
    
    // Funzione per convertire RA/Dec in coordinate SVG
    auto toSVG = [&](double ra, double dec) -> std::pair<double, double> {
        double dra = (ra - centerRA) * std::cos(centerDec * M_PI / 180.0);
        double ddec = dec - centerDec;
        double x = width / 2.0 - dra * scale;  // RA cresce verso sinistra
        double y = height / 2.0 - ddec * scale;
        return {x, y};
    };
    
    // Disegna griglia
    svg << "\n  <!-- Griglia coordinate -->\n";
    svg << "  <g stroke=\"#333355\" stroke-width=\"0.5\" fill=\"none\">\n";
    
    // Linee di declinazione
    for (int dec = static_cast<int>(centerDec - radius); 
         dec <= static_cast<int>(centerDec + radius); dec += 5) {
        auto [x1, y1] = toSVG(centerRA - radius, dec);
        auto [x2, y2] = toSVG(centerRA + radius, dec);
        svg << "    <line x1=\"" << x1 << "\" y1=\"" << y1 
            << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
    }
    
    // Linee di ascensione retta
    for (int ra = static_cast<int>(centerRA - radius); 
         ra <= static_cast<int>(centerRA + radius); ra += 5) {
        auto [x1, y1] = toSVG(ra, centerDec - radius);
        auto [x2, y2] = toSVG(ra, centerDec + radius);
        svg << "    <line x1=\"" << x1 << "\" y1=\"" << y1 
            << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
    }
    svg << "  </g>\n";
    
    // Disegna linee della costellazione
    svg << "\n  <!-- Linee costellazione -->\n";
    svg << "  <g stroke=\"#4488cc\" stroke-width=\"1.5\" fill=\"none\" opacity=\"0.6\">\n";
    
    for (const auto& line : LEO_LINES) {
        const auto& s1 = LEO_MAIN_STARS[line.first];
        const auto& s2 = LEO_MAIN_STARS[line.second];
        auto [x1, y1] = toSVG(s1.ra, s1.dec);
        auto [x2, y2] = toSVG(s2.ra, s2.dec);
        svg << "    <line x1=\"" << x1 << "\" y1=\"" << y1 
            << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
    }
    svg << "  </g>\n";
    
    // Disegna stelle Gaia (sfondo)
    svg << "\n  <!-- Stelle Gaia -->\n";
    svg << "  <g>\n";
    
    for (const auto& star : gaiaStars) {
        double ra = star->getCoordinates().getRightAscension();
        double dec = star->getCoordinates().getDeclination();
        double mag = star->getMagnitude();
        
        auto [x, y] = toSVG(ra, dec);
        
        // Salta stelle fuori dal campo
        if (x < 0 || x > width || y < 0 || y > height) continue;
        
        // Raggio proporzionale alla luminosità
        double r = std::max(0.5, 4.0 - mag * 0.5);
        
        // Colore basato sul colore B-V (se disponibile)
        std::string color = "white";
        auto ciOpt = star->getColorIndex();
        if (ciOpt.has_value()) {
            double ci = ciOpt.value();
            if (ci < 0.0) color = "#aaccff";      // Blu
            else if (ci < 0.5) color = "#ffffff"; // Bianco
            else if (ci < 1.0) color = "#ffeecc"; // Giallo
            else color = "#ffaa77";               // Arancione/Rosso
        }
        
        svg << "    <circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r 
            << "\" fill=\"" << color << "\" opacity=\"0.8\"/>\n";
    }
    svg << "  </g>\n";
    
    // Disegna stelle principali con nomi
    svg << "\n  <!-- Stelle principali con nomi -->\n";
    svg << "  <g>\n";
    
    for (const auto& star : LEO_MAIN_STARS) {
        auto [x, y] = toSVG(star.ra, star.dec);
        
        // Raggio proporzionale alla luminosità
        double r = std::max(2.0, 6.0 - star.mag);
        
        // Stella con alone
        svg << "    <circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r * 2 
            << "\" fill=\"url(#starGlow)\"/>\n";
        svg << "    <circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r 
            << "\" fill=\"white\"/>\n";
        
        // Nome della stella
        svg << "    <text x=\"" << x + r + 5 << "\" y=\"" << y + 4 
            << "\" font-family=\"Arial\" font-size=\"10\" fill=\"#88aacc\">"
            << star.name << "</text>\n";
    }
    svg << "  </g>\n";
    
    // Legenda
    svg << R"SVG(
  <!-- Legenda -->
  <g transform="translate(20, )SVG" << height - 100 << R"SVG()">
    <text x="0" y="0" font-family="Arial" font-size="12" fill="white">Legenda:</text>
    <circle cx="10" cy="20" r="4" fill="white"/>
    <text x="20" y="24" font-family="Arial" font-size="10" fill="#aaaaaa">Mag 1-2</text>
    <circle cx="80" cy="20" r="3" fill="white"/>
    <text x="90" y="24" font-family="Arial" font-size="10" fill="#aaaaaa">Mag 3-4</text>
    <circle cx="150" cy="20" r="2" fill="white"/>
    <text x="160" y="24" font-family="Arial" font-size="10" fill="#aaaaaa">Mag 5-6</text>
    
    <line x1="0" y1="40" x2="30" y2="40" stroke="#4488cc" stroke-width="1.5"/>
    <text x="35" y="44" font-family="Arial" font-size="10" fill="#aaaaaa">Linee costellazione</text>
  </g>
)SVG";
    
    // Info coordinate
    svg << "\n  <!-- Info -->\n";
    svg << "  <text x=\"" << width - 20 << "\" y=\"" << height - 20 
        << "\" text-anchor=\"end\" font-family=\"Arial\" font-size=\"10\" fill=\"#666666\">\n";
    svg << "    Centro: RA " << std::fixed << std::setprecision(1) << centerRA 
        << " Dec " << centerDec << " | Raggio: " << radius << "\n";
    svg << "  </text>\n";
    
    // Footer SVG
    svg << "</svg>\n";
    
    svg.close();
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║           CARTA CELESTE DEL LEONE (LEO)                      ║\n";
    std::cout << "║           Dati dal catalogo Gaia DR3                         ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    // Parametri della carta
    const double centerRA = 160.0;   // Centro della costellazione
    const double centerDec = 17.0;
    const double radius = 25.0;      // Raggio del campo visivo
    const double maxMag = 6.5;       // Magnitudine limite
    
    std::cout << "Parametri carta:\n";
    std::cout << "  Centro: RA = " << centerRA << "°, Dec = " << centerDec << "°\n";
    std::cout << "  Raggio: " << radius << "°\n";
    std::cout << "  Magnitudine limite: " << maxMag << "\n\n";
    
    // Inizializza il client Gaia
    std::cout << "Inizializzazione catalogo Gaia...\n";
    catalog::GaiaClient gaia;
    
    if (!gaia.isAvailable()) {
        std::cerr << "ERRORE: Catalogo Gaia non disponibile!\n";
        return 1;
    }
    std::cout << "✓ Catalogo Gaia disponibile\n\n";
    
    // Query delle stelle
    std::cout << "Query stelle nella regione del Leone...\n";
    
    catalog::GaiaQueryParameters params;
    params.center = core::EquatorialCoordinates(centerRA, centerDec);
    params.radiusDegrees = radius;
    params.maxMagnitude = maxMag;
    
    auto stars = gaia.queryRegion(params);
    
    std::cout << "✓ Trovate " << stars.size() << " stelle\n\n";
    
    // Statistiche
    double minMag = 99, maxMagFound = -99;
    for (const auto& star : stars) {
        double m = star->getMagnitude();
        if (m < minMag) minMag = m;
        if (m > maxMagFound) maxMagFound = m;
    }
    
    std::cout << "Statistiche:\n";
    std::cout << "  Stelle totali: " << stars.size() << "\n";
    std::cout << "  Magnitudine minima: " << std::fixed << std::setprecision(2) << minMag << "\n";
    std::cout << "  Magnitudine massima: " << maxMagFound << "\n\n";
    
    // Mostra le stelle più luminose
    std::cout << "Stelle più luminose (mag < 4):\n";
    std::cout << "  " << std::left << std::setw(12) << "Gaia ID" 
              << std::setw(10) << "RA" << std::setw(10) << "Dec" 
              << std::setw(8) << "Mag" << "\n";
    std::cout << "  " << std::string(40, '-') << "\n";
    
    // Ordina per magnitudine
    std::vector<std::shared_ptr<core::Star>> sortedStars = stars;
    std::sort(sortedStars.begin(), sortedStars.end(),
              [](const auto& a, const auto& b) {
                  return a->getMagnitude() < b->getMagnitude();
              });
    
    int count = 0;
    for (const auto& star : sortedStars) {
        if (star->getMagnitude() >= 4.0) break;
        std::cout << "  " << std::left << std::setw(12) << star->getGaiaId()
                  << std::fixed << std::setprecision(3)
                  << std::setw(10) << star->getCoordinates().getRightAscension()
                  << std::setw(10) << star->getCoordinates().getDeclination()
                  << std::setw(8) << star->getMagnitude() << "\n";
        if (++count >= 15) break;
    }
    
    // Genera file SVG
    std::string svgFile = "leo_chart.svg";
    std::cout << "\nGenerazione carta SVG: " << svgFile << "...\n";
    generateSVG(stars, svgFile, centerRA, centerDec, radius);
    std::cout << "✓ Carta SVG generata!\n";
    
    // Converti in PNG usando ImageMagick
    std::string pngFile = "leo_chart.png";
    std::cout << "Conversione in PNG: " << pngFile << "...\n";
    std::string cmd = "magick " + svgFile + " -density 150 " + pngFile + " 2>/dev/null";
    int result = system(cmd.c_str());
    if (result == 0) {
        std::cout << "✓ Carta PNG generata!\n";
    } else {
        // Prova con convert (vecchia versione ImageMagick)
        cmd = "convert " + svgFile + " " + pngFile + " 2>/dev/null";
        result = system(cmd.c_str());
        if (result == 0) {
            std::cout << "✓ Carta PNG generata!\n";
        } else {
            std::cout << "⚠ PNG non generato (ImageMagick non trovato)\n";
        }
    }
    
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "File generati:\n";
    std::cout << "  - " << svgFile << " (vettoriale)\n";
    std::cout << "  - " << pngFile << " (raster)\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    return 0;
}
