#ifndef STARMAP_LIBRARY_CONFIG_H
#define STARMAP_LIBRARY_CONFIG_H

#include <string>
#include <memory>

namespace starmap {
namespace config {

/**
 * @brief Configurazione globale della libreria StarMap
 * 
 * Gestisce i path ai database e le configurazioni globali della libreria.
 * Deve essere inizializzata prima dell'uso tramite LibraryConfig::initialize().
 */
class LibraryConfig {
public:
    /**
     * @brief Struttura per i path dei cataloghi
     */
    struct CatalogPaths {
        std::string gaiaSaoDatabase;
        std::string iauCatalog;
        std::string starNamesDatabase;
        
        CatalogPaths() 
            : gaiaSaoDatabase("gaia_sao_xmatch.db")
            , iauCatalog("data/IAU-CSN.json")
            , starNamesDatabase("data/common_star_names.csv") {}
    };

    /**
     * @brief Inizializza la configurazione globale
     * @param paths Path ai cataloghi (opzionale, usa defaults se non specificato)
     */
    static void initialize(const CatalogPaths& paths = CatalogPaths());

    /**
     * @brief Ottieni la configurazione corrente
     * @return Riferimento alla configurazione globale
     */
    static LibraryConfig& getInstance();

    /**
     * @brief Imposta il path al database Gaia-SAO
     * @param path Path assoluto o relativo al file .db
     */
    void setGaiaSaoDbPath(const std::string& path);

    /**
     * @brief Ottieni il path al database Gaia-SAO
     */
    const std::string& getGaiaSaoDbPath() const;

    /**
     * @brief Imposta il path al catalogo IAU
     * @param path Path al file IAU-CSN.json
     */
    void setIauCatalogPath(const std::string& path);

    /**
     * @brief Ottieni il path al catalogo IAU
     */
    const std::string& getIauCatalogPath() const;

    /**
     * @brief Imposta il path al database nomi stelle
     * @param path Path al file common_star_names.csv
     */
    void setStarNamesDbPath(const std::string& path);

    /**
     * @brief Ottieni il path al database nomi stelle
     */
    const std::string& getStarNamesDbPath() const;

    /**
     * @brief Verifica se la libreria è stata inizializzata
     */
    static bool isInitialized();

private:
    LibraryConfig() = default;
    
    // Non copiabile
    LibraryConfig(const LibraryConfig&) = delete;
    LibraryConfig& operator=(const LibraryConfig&) = delete;

    CatalogPaths paths_;
    static std::unique_ptr<LibraryConfig> instance_;
    static bool initialized_;
};

} // namespace config
} // namespace starmap

#endif // STARMAP_LIBRARY_CONFIG_H
