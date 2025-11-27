#include "starmap/map/MapConfiguration.h"

namespace starmap {
namespace map {

MapConfiguration::MapConfiguration() {
    // Valori di default già impostati negli header
}

bool MapConfiguration::validate() const {
    // Validazione dimensioni
    if (imageWidth <= 0 || imageHeight <= 0) {
        return false;
    }
    
    if (fieldOfViewWidth <= 0.0 || fieldOfViewHeight <= 0.0) {
        return false;
    }
    
    if (fieldOfViewWidth > 180.0 || fieldOfViewHeight > 180.0) {
        return false; // FOV troppo grande
    }
    
    // Validazione coordinate centro
    if (center.getDeclination() < -90.0 || center.getDeclination() > 90.0) {
        return false;
    }
    
    // Validazione magnitudine
    if (limitingMagnitude < 0.0 || limitingMagnitude > 30.0) {
        return false;
    }
    
    return true;
}

MapConfiguration MapConfiguration::clone() const {
    MapConfiguration copy = *this;
    return copy;
}

} // namespace map
} // namespace starmap
