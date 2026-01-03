#ifndef STARMAP_GAIA_SAO_DATABASE_H
#define STARMAP_GAIA_SAO_DATABASE_H

#include "starmap/core/Coordinates.h"
#include <string>
#include <optional>
#include <memory>
#include <vector>

namespace starmap {
namespace catalog {

/**
 * @brief Entry nel database di cross-match Gaia-SAO
 */
struct GaiaSAOEntry {
    long long gaiaSourceId;
    int saoNumber;
    double ra;          // Right Ascension (J2000) in gradi
    double dec;         // Declination (J2000) in gradi
    double magnitude;   // Magnitudine G di Gaia
    double separation;  // Separazione in arcsec (per validazione match)
};

/**
 * @brief Database locale per cross-match tra Gaia DR3 e catalogo SAO
 * 
 * Questa classe gestisce un database SQLite locale che mappa source_id Gaia DR3
 * ai corrispondenti numeri SAO. Il database viene popolato offline da dati
 * scaricati da VizieR/SIMBAD per evitare query online ripetute.
 * 
 * Il database contiene circa 258,997 stelle SAO con cross-match verificato.
 * 
 * Performance tipiche:
 * - Query per Gaia ID: < 0.1 ms
 * - Query per coordinate: < 1 ms (con indice spaziale)
 * - Dimensione database: ~15 MB
 */
class GaiaSAODatabase {
public:
    /**
     * @brief Costruttore con path al database
     * @param dbPath Path al file database SQLite (default: "gaia_sao_xmatch.db")
     */
    explicit GaiaSAODatabase(const std::string& dbPath = "gaia_sao_xmatch.db");
    
    ~GaiaSAODatabase();

    /**
     * @brief Verifica se il database esiste ed è valido
     * @return true se database disponibile e accessibile
     */
    bool isAvailable() const;

    /**
     * @brief Cerca numero SAO per Gaia source_id
     * @param gaiaSourceId Source ID Gaia DR3
     * @return Numero SAO se trovato nel database
     */
    std::optional<int> findSAOByGaiaId(long long gaiaSourceId) const;

    /**
     * @brief Cerca numero SAO per coordinate (cone search)
     * @param coords Coordinate equatoriali J2000
     * @param radiusArcsec Raggio di ricerca in arcsec (default 5")
     * @return Numero SAO della stella più vicina se trovata
     */
    std::optional<int> findSAOByCoordinates(
        const core::EquatorialCoordinates& coords,
        double radiusArcsec = 5.0) const;

    /**
     * @brief Ottieni entry completa per Gaia ID
     * @param gaiaSourceId Source ID Gaia DR3
     * @return Entry con tutti i dati se trovata
     */
    std::optional<GaiaSAOEntry> getEntry(long long gaiaSourceId) const;

    /**
     * @brief Cerca tutte le stelle SAO in un cono
     * @param coords Centro del cono
     * @param radiusDegrees Raggio del cono in gradi
     * @param maxResults Numero massimo di risultati (default 1000)
     * @return Lista di entry trovate
     */
    std::vector<GaiaSAOEntry> coneSearch(
        const core::EquatorialCoordinates& coords,
        double radiusDegrees,
        int maxResults = 1000) const;

    /**
     * @brief Ottieni statistiche del database
     * @return Stringa con informazioni (numero entry, versione, etc.)
     */
    std::string getStatistics() const;

    /**
     * @brief Verifica integrità del database
     * @return true se database integro
     */
    bool verifyIntegrity() const;

    // ========== Funzioni per costruzione database (uso interno) ==========

    /**
     * @brief Crea nuovo database vuoto con schema
     * @return true se creato con successo
     */
    bool createNewDatabase();

    /**
     * @brief Inserisci singola entry nel database
     * @param entry Entry da inserire
     * @return true se inserimento riuscito
     */
    bool insertEntry(const GaiaSAOEntry& entry);

    /**
     * @brief Inserisci batch di entry (più efficiente)
     * @param entries Vettore di entry da inserire
     * @return Numero di entry inserite con successo
     */
    size_t insertBatch(const std::vector<GaiaSAOEntry>& entries);

    /**
     * @brief Crea indici per performance (da chiamare dopo inserimento dati)
     * @return true se indici creati con successo
     */
    bool createIndices();

    /**
     * @brief Ottimizza database (VACUUM)
     * @return true se ottimizzazione riuscita
     */
    bool optimize();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    std::string dbPath_;
    bool available_;
};

} // namespace catalog
} // namespace starmap

#endif // STARMAP_GAIA_SAO_DATABASE_H
