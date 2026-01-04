#include "starmap/config/LibraryConfig.h"

namespace starmap {
namespace config {

std::unique_ptr<LibraryConfig> LibraryConfig::instance_ = nullptr;
bool LibraryConfig::initialized_ = false;

void LibraryConfig::initialize(const CatalogPaths& paths) {
    if (!instance_) {
        instance_ = std::unique_ptr<LibraryConfig>(new LibraryConfig());
    }
    instance_->paths_ = paths;
    initialized_ = true;
}

LibraryConfig& LibraryConfig::getInstance() {
    if (!instance_) {
        // Auto-inizializza con valori di default se non è stata chiamata initialize()
        initialize();
    }
    return *instance_;
}

void LibraryConfig::setGaiaSaoDbPath(const std::string& path) {
    paths_.gaiaSaoDatabase = path;
}

const std::string& LibraryConfig::getGaiaSaoDbPath() const {
    return paths_.gaiaSaoDatabase;
}

void LibraryConfig::setIauCatalogPath(const std::string& path) {
    paths_.iauCatalog = path;
}

const std::string& LibraryConfig::getIauCatalogPath() const {
    return paths_.iauCatalog;
}

void LibraryConfig::setStarNamesDbPath(const std::string& path) {
    paths_.starNamesDatabase = path;
}

const std::string& LibraryConfig::getStarNamesDbPath() const {
    return paths_.starNamesDatabase;
}

bool LibraryConfig::isInitialized() {
    return initialized_;
}

} // namespace config
} // namespace starmap
