/**
 * @file ConstellationData.h
 * @brief Dichiarazioni dati costellazioni: figure e confini IAU
 */

#pragma once

#include "starmap/map/ChartGenerator.h"
#include <map>
#include <vector>
#include <string>

namespace starmap {
namespace map {

/**
 * @brief Mappa delle costellazioni con dati figure
 * Chiavi: nomi inglesi (Orion, Leo, etc.)
 */
extern const std::map<std::string, ConstellationData> ALL_CONSTELLATIONS;

/**
 * @brief Confini IAU delle costellazioni
 */
extern const std::vector<ConstellationBoundary> CONSTELLATION_BOUNDARIES;

/**
 * @brief Ottiene i segmenti di confine per una costellazione
 * @param abbr Abbreviazione IAU (es. "Ori", "Leo")
 * @return Vettore di linee del confine
 */
std::vector<ConstellationLine> getConstellationBoundaryLines(const std::string& abbr);

} // namespace map
} // namespace starmap
