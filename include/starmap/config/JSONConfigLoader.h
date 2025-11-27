#ifndef STARMAP_JSON_CONFIG_LOADER_H
#define STARMAP_JSON_CONFIG_LOADER_H

#include "ConfigurationLoader.h"
#include <nlohmann/json.hpp>

namespace starmap {
namespace config {

/**
 * @brief Loader per configurazioni JSON
 */
class JSONConfigLoader : public ConfigurationLoader {
public:
    JSONConfigLoader() = default;
    ~JSONConfigLoader() override = default;

    map::MapConfiguration load(const std::string& filename) override;
    bool save(const map::MapConfiguration& config, 
             const std::string& filename) override;
    
    map::MapConfiguration loadFromString(const std::string& data) override;
    std::string saveToString(const map::MapConfiguration& config) override;

private:
    nlohmann::json configToJson(const map::MapConfiguration& config);
    map::MapConfiguration jsonToConfig(const nlohmann::json& j);
    
    // Helper per conversione enum
    std::string projectionTypeToString(map::ProjectionType type);
    map::ProjectionType stringToProjectionType(const std::string& str);
    
    std::string coordinateSystemToString(map::CoordinateSystem sys);
    map::CoordinateSystem stringToCoordinateSystem(const std::string& str);
};

} // namespace config
} // namespace starmap

#endif // STARMAP_JSON_CONFIG_LOADER_H
