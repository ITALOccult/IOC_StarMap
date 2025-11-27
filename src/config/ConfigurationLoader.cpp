#include "starmap/config/ConfigurationLoader.h"
#include "JSONConfigLoader.h"
#include <stdexcept>
#include <algorithm>

namespace starmap {
namespace config {

std::unique_ptr<ConfigurationLoader> ConfigurationLoaderFactory::create(Format format) {
    switch (format) {
        case Format::JSON:
            return std::make_unique<JSONConfigLoader>();
        
        // Altri formati possono essere aggiunti qui
        case Format::XML:
        case Format::YAML:
        case Format::INI:
            throw std::runtime_error("Format not yet implemented");
        
        default:
            throw std::runtime_error("Unknown configuration format");
    }
}

std::unique_ptr<ConfigurationLoader> ConfigurationLoaderFactory::createFromFilename(
    const std::string& filename) {
    
    // Estrai estensione
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        throw std::runtime_error("Cannot determine file format from filename");
    }
    
    std::string ext = filename.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return create(Format::JSON);
    } else if (ext == "xml") {
        return create(Format::XML);
    } else if (ext == "yaml" || ext == "yml") {
        return create(Format::YAML);
    } else if (ext == "ini" || ext == "cfg") {
        return create(Format::INI);
    }
    
    throw std::runtime_error("Unsupported file format: " + ext);
}

} // namespace config
} // namespace starmap
