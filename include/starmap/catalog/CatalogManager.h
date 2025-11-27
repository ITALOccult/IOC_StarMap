#ifndef STARMAP_CATALOG_MANAGER_H
#define STARMAP_CATALOG_MANAGER_H

#include "GaiaClient.h"
#include "SAOCatalog.h"
#include "starmap/core/CelestialObject.h"
#include <memory>
#include <vector>

namespace starmap {
namespace catalog {

/**
 * @brief Manager unificato per gestire query a cataloghi multipli
 */
class CatalogManager {
public:
    CatalogManager();
    ~CatalogManager();

    /**
     * @brief Query unificata che combina GAIA e SAO
     * @param params Parametri query GAIA
     * @param enrichWithSAO Se true, cerca numeri SAO per le stelle trovate
     * @return Lista di stelle con dati completi
     */
    std::vector<std::shared_ptr<core::Star>> queryStars(
        const GaiaQueryParameters& params,
        bool enrichWithSAO = true);

    /**
     * @brief Query per regione rettangolare
     */
    std::vector<std::shared_ptr<core::Star>> queryRectangularRegion(
        const core::EquatorialCoordinates& center,
        double widthDeg,
        double heightDeg,
        double maxMagnitude = 15.0,
        bool enrichWithSAO = true);

    /**
     * @brief Accesso ai client individuali
     */
    GaiaClient& getGaiaClient() { return gaiaClient_; }
    SAOCatalog& getSAOCatalog() { return saoCatalog_; }

    /**
     * @brief Imposta opzioni di caching e performance
     */
    void setCacheEnabled(bool enabled);
    void setParallelEnrichment(bool enabled);

private:
    GaiaClient gaiaClient_;
    SAOCatalog saoCatalog_;
    bool cacheEnabled_;
    bool parallelEnrichment_;
};

} // namespace catalog
} // namespace starmap

#endif // STARMAP_CATALOG_MANAGER_H
