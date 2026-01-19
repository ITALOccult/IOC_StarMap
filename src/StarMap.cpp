#include <iostream>
#include <cmath>
#include "starmap/StarMap.h"

namespace starmap {

void initialize(const config::LibraryConfig::CatalogPaths& catalogPaths) {
    config::LibraryConfig::initialize(catalogPaths);
}

void finalize() {
    // Cleanup se necessario in futuro
}

} // namespace starmap
