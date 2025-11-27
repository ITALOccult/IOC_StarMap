#ifndef STARMAP_CONFIGURATION_LOADER_H
#define STARMAP_CONFIGURATION_LOADER_H

#include "starmap/map/MapConfiguration.h"
#include <string>
#include <memory>

namespace starmap {
namespace config {

/**
 * @brief Interfaccia base per loader di configurazione
 */
class ConfigurationLoader {
public:
    virtual ~ConfigurationLoader() = default;

    /**
     * @brief Carica configurazione da file
     * @param filename Path al file di configurazione
     * @return Configurazione caricata
     */
    virtual map::MapConfiguration load(const std::string& filename) = 0;

    /**
     * @brief Salva configurazione su file
     * @param config Configurazione da salvare
     * @param filename Path di destinazione
     * @return true se salvato con successo
     */
    virtual bool save(const map::MapConfiguration& config, 
                     const std::string& filename) = 0;

    /**
     * @brief Carica da stringa
     */
    virtual map::MapConfiguration loadFromString(const std::string& data) = 0;

    /**
     * @brief Salva come stringa
     */
    virtual std::string saveToString(const map::MapConfiguration& config) = 0;
};

/**
 * @brief Factory per creare loader appropriati
 */
class ConfigurationLoaderFactory {
public:
    enum class Format {
        JSON,
        XML,
        YAML,
        INI
    };

    static std::unique_ptr<ConfigurationLoader> create(Format format);
    
    /**
     * @brief Rileva automaticamente formato dal filename
     */
    static std::unique_ptr<ConfigurationLoader> createFromFilename(
        const std::string& filename);
};

} // namespace config
} // namespace starmap

#endif // STARMAP_CONFIGURATION_LOADER_H
