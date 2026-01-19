#include "starmap/catalog/GaiaSAODatabase.h"
#include <sqlite3.h>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace starmap {
namespace catalog {

// Costanti per conversione coordinate
constexpr double DEG_TO_RAD = M_PI / 180.0;
constexpr double RAD_TO_DEG = 180.0 / M_PI;

/**
 * @brief Implementazione privata usando PIMPL pattern
 */
class GaiaSAODatabase::Impl {
public:
    sqlite3* db = nullptr;
    
    ~Impl() {
        if (db) {
            sqlite3_close(db);
        }
    }
    
    /**
     * @brief Calcola distanza angolare tra due punti sulla sfera celeste
     */
    static double angularSeparation(double ra1, double dec1, double ra2, double dec2) {
        // Formula dell'haversine
        double dRa = (ra2 - ra1) * DEG_TO_RAD;
        double dDec = (dec2 - dec1) * DEG_TO_RAD;
        double dec1Rad = dec1 * DEG_TO_RAD;
        double dec2Rad = dec2 * DEG_TO_RAD;
        
        double a = std::sin(dDec / 2.0) * std::sin(dDec / 2.0) +
                   std::cos(dec1Rad) * std::cos(dec2Rad) *
                   std::sin(dRa / 2.0) * std::sin(dRa / 2.0);
        
        double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
        
        return c * RAD_TO_DEG * 3600.0; // Ritorna in arcsec
    }
};

GaiaSAODatabase::GaiaSAODatabase(const std::string& dbPath)
    : pImpl_(std::make_unique<Impl>())
    , dbPath_(dbPath)
    , available_(false) {
    
    // Prova ad aprire il database
    int rc = sqlite3_open(dbPath_.c_str(), &pImpl_->db);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open Gaia-SAO database: " << sqlite3_errmsg(pImpl_->db) << std::endl;
        if (pImpl_->db) {
            sqlite3_close(pImpl_->db);
            pImpl_->db = nullptr;
        }
        return;
    }
    
    // Verifica che la tabella esista
    const char* checkQuery = "SELECT name FROM sqlite_master WHERE type='table' AND name='stars';";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(pImpl_->db, checkQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            available_ = true;
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to prepare table check query: " << sqlite3_errmsg(pImpl_->db) << std::endl;
    }
    
    if (!available_) {
        std::cerr << "Stellar crossref database table 'stars' not found in: " << dbPath_ << std::endl;
    } else {
        std::cout << "Gaia-SAO local database recognized in: " << dbPath_ << std::endl;
    }
}

GaiaSAODatabase::~GaiaSAODatabase() = default;

bool GaiaSAODatabase::isAvailable() const {
    return available_ && pImpl_->db != nullptr;
}

std::optional<int> GaiaSAODatabase::findSAOByGaiaId(long long gaiaSourceId) const {
    if (!isAvailable()) return std::nullopt;
    
    const char* query = "SELECT sao FROM stars WHERE gaia_dr3 = ? AND sao IS NOT NULL AND sao > 0 LIMIT 1;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(pImpl_->db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_int64(stmt, 1, gaiaSourceId);
    
    std::optional<int> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

std::optional<int> GaiaSAODatabase::findSAOByCoordinates(
    const core::EquatorialCoordinates& coords,
    double radiusArcsec) const {
    
    if (!isAvailable()) return std::nullopt;
    
    double ra = coords.getRightAscension();
    double dec = coords.getDeclination();
    
    // Converti raggio in gradi per bounding box approssimato
    double radiusDeg = radiusArcsec / 3600.0;
    double raMin = ra - radiusDeg / std::cos(dec * DEG_TO_RAD);
    double raMax = ra + radiusDeg / std::cos(dec * DEG_TO_RAD);
    double decMin = dec - radiusDeg;
    double decMax = dec + radiusDeg;
    
    // Query con bounding box
    const char* query = R"(
        SELECT sao, ra_deg, dec_deg 
        FROM stars 
        WHERE ra_deg BETWEEN ? AND ? 
          AND dec_deg BETWEEN ? AND ?
          AND sao IS NOT NULL AND sao > 0
        ORDER BY magnitude
        LIMIT 50;
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(pImpl_->db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_double(stmt, 1, raMin);
    sqlite3_bind_double(stmt, 2, raMax);
    sqlite3_bind_double(stmt, 3, decMin);
    sqlite3_bind_double(stmt, 4, decMax);
    
    std::optional<int> bestMatch;
    double minSeparation = radiusArcsec;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int saoNum = sqlite3_column_int(stmt, 0);
        double starRa = sqlite3_column_double(stmt, 1);
        double starDec = sqlite3_column_double(stmt, 2);
        
        double separation = Impl::angularSeparation(ra, dec, starRa, starDec);
        
        if (separation < minSeparation) {
            minSeparation = separation;
            bestMatch = saoNum;
        }
    }
    
    sqlite3_finalize(stmt);
    return bestMatch;
}

std::optional<GaiaSAOEntry> GaiaSAODatabase::getEntry(long long gaiaSourceId) const {
    if (!isAvailable()) return std::nullopt;
    
    const char* query = R"(
        SELECT gaia_source_id, sao_number, ra, dec, magnitude, separation
        FROM gaia_sao_xmatch 
        WHERE gaia_source_id = ? 
        LIMIT 1;
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(pImpl_->db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_int64(stmt, 1, gaiaSourceId);
    
    std::optional<GaiaSAOEntry> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        GaiaSAOEntry entry;
        entry.gaiaSourceId = sqlite3_column_int64(stmt, 0);
        entry.saoNumber = sqlite3_column_int(stmt, 1);
        entry.ra = sqlite3_column_double(stmt, 2);
        entry.dec = sqlite3_column_double(stmt, 3);
        entry.magnitude = sqlite3_column_double(stmt, 4);
        entry.separation = sqlite3_column_double(stmt, 5);
        result = entry;
    }
    
    sqlite3_finalize(stmt);
    return result;
}

std::vector<GaiaSAOEntry> GaiaSAODatabase::coneSearch(
    const core::EquatorialCoordinates& coords,
    double radiusDegrees,
    int maxResults) const {
    
    std::vector<GaiaSAOEntry> results;
    if (!isAvailable()) return results;
    
    double ra = coords.getRightAscension();
    double dec = coords.getDeclination();
    
    // Bounding box
    double raMin = ra - radiusDegrees / std::cos(dec * DEG_TO_RAD);
    double raMax = ra + radiusDegrees / std::cos(dec * DEG_TO_RAD);
    double decMin = dec - radiusDegrees;
    double decMax = dec + radiusDegrees;
    
    const char* query = R"(
        SELECT gaia_source_id, sao_number, ra, dec, magnitude, separation
        FROM gaia_sao_xmatch 
        WHERE ra BETWEEN ? AND ? 
          AND dec BETWEEN ? AND ?
        ORDER BY magnitude
        LIMIT ?;
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(pImpl_->db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        return results;
    }
    
    sqlite3_bind_double(stmt, 1, raMin);
    sqlite3_bind_double(stmt, 2, raMax);
    sqlite3_bind_double(stmt, 3, decMin);
    sqlite3_bind_double(stmt, 4, decMax);
    sqlite3_bind_int(stmt, 5, maxResults);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        GaiaSAOEntry entry;
        entry.gaiaSourceId = sqlite3_column_int64(stmt, 0);
        entry.saoNumber = sqlite3_column_int(stmt, 1);
        entry.ra = sqlite3_column_double(stmt, 2);
        entry.dec = sqlite3_column_double(stmt, 3);
        entry.magnitude = sqlite3_column_double(stmt, 4);
        entry.separation = sqlite3_column_double(stmt, 5);
        
        // Verifica che sia realmente nel cono
        double separation = Impl::angularSeparation(ra, dec, entry.ra, entry.dec);
        if (separation <= radiusDegrees * 3600.0) {
            results.push_back(entry);
        }
    }
    
    sqlite3_finalize(stmt);
    return results;
}

std::string GaiaSAODatabase::getStatistics() const {
    if (!isAvailable()) {
        return "Database not available";
    }
    
    std::ostringstream stats;
    
    // Conta totale entry
    const char* countQuery = "SELECT COUNT(*) FROM gaia_sao_xmatch;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(pImpl_->db, countQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            stats << "Total entries: " << count << "\n";
        }
        sqlite3_finalize(stmt);
    }
    
    // Range magnitudini
    const char* magQuery = "SELECT MIN(magnitude), MAX(magnitude), AVG(magnitude) FROM gaia_sao_xmatch;";
    if (sqlite3_prepare_v2(pImpl_->db, magQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            double minMag = sqlite3_column_double(stmt, 0);
            double maxMag = sqlite3_column_double(stmt, 1);
            double avgMag = sqlite3_column_double(stmt, 2);
            stats << "Magnitude range: " << minMag << " - " << maxMag 
                  << " (avg: " << avgMag << ")\n";
        }
        sqlite3_finalize(stmt);
    }
    
    // Dimensione database
    const char* sizeQuery = "SELECT page_count * page_size as size FROM pragma_page_count(), pragma_page_size();";
    if (sqlite3_prepare_v2(pImpl_->db, sizeQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            long long size = sqlite3_column_int64(stmt, 0);
            stats << "Database size: " << (size / 1024.0 / 1024.0) << " MB\n";
        }
        sqlite3_finalize(stmt);
    }
    
    return stats.str();
}

bool GaiaSAODatabase::verifyIntegrity() const {
    if (!isAvailable()) return false;
    
    const char* integrityQuery = "PRAGMA integrity_check;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(pImpl_->db, integrityQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    bool isOk = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        isOk = (std::string(result) == "ok");
    }
    
    sqlite3_finalize(stmt);
    return isOk;
}

// ========== Funzioni per costruzione database ==========

bool GaiaSAODatabase::createNewDatabase() {
    // Se il database esiste già, chiudilo
    if (pImpl_->db) {
        sqlite3_close(pImpl_->db);
        pImpl_->db = nullptr;
    }
    
    // Crea nuovo database
    int rc = sqlite3_open(dbPath_.c_str(), &pImpl_->db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot create database: " << sqlite3_errmsg(pImpl_->db) << std::endl;
        return false;
    }
    
    // Schema della tabella
    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS gaia_sao_xmatch (
            gaia_source_id INTEGER PRIMARY KEY,
            sao_number INTEGER NOT NULL,
            ra REAL NOT NULL,
            dec REAL NOT NULL,
            magnitude REAL,
            separation REAL,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
    )";
    
    char* errMsg = nullptr;
    rc = sqlite3_exec(pImpl_->db, createTableSQL, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    // Crea tabella metadata
    const char* createMetaSQL = R"(
        CREATE TABLE IF NOT EXISTS metadata (
            key TEXT PRIMARY KEY,
            value TEXT
        );
        INSERT OR REPLACE INTO metadata (key, value) VALUES ('version', '1.0');
        INSERT OR REPLACE INTO metadata (key, value) VALUES ('created', datetime('now'));
    )";
    
    rc = sqlite3_exec(pImpl_->db, createMetaSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating metadata: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    available_ = true;
    return true;
}

bool GaiaSAODatabase::insertEntry(const GaiaSAOEntry& entry) {
    if (!pImpl_->db) return false;
    
    const char* insertSQL = R"(
        INSERT OR REPLACE INTO gaia_sao_xmatch 
        (gaia_source_id, sao_number, ra, dec, magnitude, separation)
        VALUES (?, ?, ?, ?, ?, ?);
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(pImpl_->db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int64(stmt, 1, entry.gaiaSourceId);
    sqlite3_bind_int(stmt, 2, entry.saoNumber);
    sqlite3_bind_double(stmt, 3, entry.ra);
    sqlite3_bind_double(stmt, 4, entry.dec);
    sqlite3_bind_double(stmt, 5, entry.magnitude);
    sqlite3_bind_double(stmt, 6, entry.separation);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    
    return success;
}

size_t GaiaSAODatabase::insertBatch(const std::vector<GaiaSAOEntry>& entries) {
    if (!pImpl_->db || entries.empty()) return 0;
    
    // Inizia transazione per performance
    sqlite3_exec(pImpl_->db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    
    const char* insertSQL = R"(
        INSERT OR REPLACE INTO gaia_sao_xmatch 
        (gaia_source_id, sao_number, ra, dec, magnitude, separation)
        VALUES (?, ?, ?, ?, ?, ?);
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(pImpl_->db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(pImpl_->db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return 0;
    }
    
    size_t insertedCount = 0;
    for (const auto& entry : entries) {
        sqlite3_bind_int64(stmt, 1, entry.gaiaSourceId);
        sqlite3_bind_int(stmt, 2, entry.saoNumber);
        sqlite3_bind_double(stmt, 3, entry.ra);
        sqlite3_bind_double(stmt, 4, entry.dec);
        sqlite3_bind_double(stmt, 5, entry.magnitude);
        sqlite3_bind_double(stmt, 6, entry.separation);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            insertedCount++;
        }
        
        sqlite3_reset(stmt);
    }
    
    sqlite3_finalize(stmt);
    sqlite3_exec(pImpl_->db, "COMMIT;", nullptr, nullptr, nullptr);
    
    return insertedCount;
}

bool GaiaSAODatabase::createIndices() {
    if (!pImpl_->db) return false;
    
    const char* createIndicesSQL = R"(
        CREATE INDEX IF NOT EXISTS idx_sao_number ON gaia_sao_xmatch(sao_number);
        CREATE INDEX IF NOT EXISTS idx_ra ON gaia_sao_xmatch(ra);
        CREATE INDEX IF NOT EXISTS idx_dec ON gaia_sao_xmatch(dec);
        CREATE INDEX IF NOT EXISTS idx_magnitude ON gaia_sao_xmatch(magnitude);
        CREATE INDEX IF NOT EXISTS idx_ra_dec ON gaia_sao_xmatch(ra, dec);
    )";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(pImpl_->db, createIndicesSQL, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating indices: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool GaiaSAODatabase::optimize() {
    if (!pImpl_->db) return false;
    
    const char* optimizeSQL = "VACUUM; ANALYZE;";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(pImpl_->db, optimizeSQL, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error optimizing database: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

} // namespace catalog
} // namespace starmap
