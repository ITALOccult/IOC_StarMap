#ifndef STARMAP_GAIA_CLIENT_H
#define STARMAP_GAIA_CLIENT_H

#include "starmap/core/CelestialObject.h"
#include "starmap/core/Coordinates.h"
#include <vector>
#include <memory>

namespace starmap {
namespace catalog {

/**
 * @brief Parametri per query al catalogo GAIA
 */
struct GaiaQueryParameters {
    core::EquatorialCoordinates center;
    double radiusDegrees = 1.0;      // Raggio del campo in gradi
    double maxMagnitude = 15.0;      // Magnitudine limite
    int maxResults = 10000;          // Numero massimo di risultati
    bool includeProperMotion = true; // Include moto proprio
    bool includeParallax = true;     // Include parallasse
};

/**
 * @brief Client per interrogare il catalogo GAIA via TAP/ADQL
 */
class GaiaClient {
public:
    GaiaClient();
    ~GaiaClient();

    /**
     * @brief Query circolare centrata su coordinate specifiche
     * @param params Parametri della query
     * @return Lista di stelle trovate
     */
    std::vector<std::shared_ptr<core::Star>> queryRegion(
        const GaiaQueryParameters& params);

    /**
     * @brief Query per ID GAIA specifico
     * @param gaiaId ID sorgente GAIA (DR3)
     * @return Stella se trovata
     */
    std::shared_ptr<core::Star> queryById(long long gaiaId);

    /**
     * @brief Query per coordinate con box rettangolare
     * @param center Centro del campo
     * @param widthDeg Larghezza in gradi
     * @param heightDeg Altezza in gradi
     * @param maxMagnitude Magnitudine limite
     * @return Lista di stelle trovate
     */
    std::vector<std::shared_ptr<core::Star>> queryBox(
        const core::EquatorialCoordinates& center,
        double widthDeg,
        double heightDeg,
        double maxMagnitude = 15.0);

    /**
     * @brief Imposta l'URL del servizio TAP (default: Gaia ESA)
     */
    void setTapServiceUrl(const std::string& url);

    /**
     * @brief Imposta timeout per le query
     */
    void setTimeout(int seconds);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    std::string buildADQLQuery(const GaiaQueryParameters& params) const;
    std::vector<std::shared_ptr<core::Star>> parseVOTable(
        const std::string& votable) const;
};

} // namespace catalog
} // namespace starmap

#endif // STARMAP_GAIA_CLIENT_H
