#ifndef STARMAP_GAIA_CLIENT_H
#define STARMAP_GAIA_CLIENT_H

#include "starmap/core/CelestialObject.h"
#include "starmap/core/Coordinates.h"
#include <vector>
#include <memory>
#include <string>

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
 * @brief Wrapper per IOC_GaiaLib - interroga il catalogo GAIA
 * 
 * Questa classe integra IOC_GaiaLib per accedere al catalogo GAIA
 * usando sia query online (TAP/ADQL) sia cataloghi locali (Mag18).
 * 
 * @see https://github.com/manvalan/IOC_GaiaLib
 */
class GaiaClient {
public:
    /**
     * @brief Costruttore con opzione catalogo locale
     * @param useMag18 Se true, usa catalogo locale Mag18 (raccomandato per G≤18)
     * @param mag18Path Path al file gaia_mag18_v2.cat (opzionale)
     */
    explicit GaiaClient(bool useMag18 = true, 
                       const std::string& mag18Path = "");
    ~GaiaClient();

    /**
     * @brief Query circolare centrata su coordinate specifiche
     * @param params Parametri della query
     * @return Lista di stelle trovate
     */
    std::vector<std::shared_ptr<core::Star>> queryRegion(
        const GaiaQueryParameters& params);

    /**
     * @brief Query per ID GAIA specifico (source_id)
     * @param gaiaId ID sorgente GAIA DR3
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
     * @brief Imposta timeout per le query online
     */
    void setTimeout(int seconds);
    
    /**
     * @brief Imposta rate limit per query online (queries/min)
     */
    void setRateLimit(int queriesPerMinute);
    
    /**
     * @brief Verifica se il catalogo locale è disponibile
     */
    bool isLocalCatalogAvailable() const;
    
    /**
     * @brief Ottieni statistiche cache/catalogo
     */
    struct CatalogStats {
        size_t totalStars = 0;
        double magLimit = 0.0;
        bool isOnline = false;
        std::pair<size_t, size_t> cacheHitsMisses = {0, 0};
    };
    
    CatalogStats getStatistics() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Conversione da ioc::gaia::GaiaStar a starmap::core::Star
    std::shared_ptr<core::Star> convertGaiaStar(const void* gaiaStar) const;
};

} // namespace catalog
} // namespace starmap

#endif // STARMAP_GAIA_CLIENT_H
