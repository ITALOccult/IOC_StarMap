#ifndef STARMAP_H
#define STARMAP_H

/**
 * @file StarMap.h
 * @brief Header principale per la libreria StarMap
 * 
 * Questo header include tutte le componenti principali della libreria
 * per la generazione di mappe celesti simili a C2A.
 */

// Core components
#include "starmap/core/Coordinates.h"
#include "starmap/core/CelestialObject.h"

// Catalog access
#include "starmap/catalog/GaiaClient.h"
#include "starmap/catalog/SAOCatalog.h"
#include "starmap/catalog/CatalogManager.h"

// Map generation
#include "starmap/map/MapConfiguration.h"
#include "starmap/map/Projection.h"
#include "starmap/map/MapRenderer.h"
#include "starmap/map/GridRenderer.h"

// Configuration
#include "starmap/config/ConfigurationLoader.h"
#include "starmap/config/JSONConfigLoader.h"

// Occultation charts
#include "starmap/occultation/OccultationData.h"
#include "starmap/occultation/OccultationChartBuilder.h"

namespace starmap {

/**
 * @brief Versione della libreria
 */
constexpr const char* VERSION = "1.0.0";

/**
 * @brief Classe di utilità per operazioni comuni
 */
class StarMapBuilder {
public:
    StarMapBuilder();
    ~StarMapBuilder();

    /**
     * @brief Genera una mappa celeste completa
     * @param config Configurazione della mappa
     * @param enrichWithSAO Se true, arricchisce con numeri SAO
     * @return Buffer immagine renderizzato
     */
    map::ImageBuffer generate(const map::MapConfiguration& config,
                              bool enrichWithSAO = true);

    /**
     * @brief Genera mappa da file di configurazione JSON
     * @param configFile Path al file JSON
     * @param enrichWithSAO Se true, arricchisce con numeri SAO
     * @return Buffer immagine renderizzato
     */
    map::ImageBuffer generateFromFile(const std::string& configFile,
                                      bool enrichWithSAO = true);

    /**
     * @brief Imposta timeout per query ai cataloghi
     */
    void setCatalogTimeout(int seconds);

    /**
     * @brief Abilita/disabilita cache
     */
    void setCacheEnabled(bool enabled);

    /**
     * @brief Accesso al catalog manager
     */
    catalog::CatalogManager& getCatalogManager();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

/**
 * @brief Inizializza la libreria (opzionale)
 * Può essere chiamata per inizializzare risorse globali
 */
void initialize();

/**
 * @brief Finalizza la libreria (opzionale)
 * Rilascia risorse globali
 */
void finalize();

} // namespace starmap

#endif // STARMAP_H
