#include "starmap/occultation/OccultationData.h"
#include <cmath>
#include <stdexcept>

namespace starmap {
namespace occultation {

bool OccultationEvent::validate() const {
    // Validazione dati base
    if (eventId.empty()) return false;
    if (targetStar.catalogId.empty()) return false;
    if (asteroid.name.empty()) return false;
    if (circumstances.eventTime.empty()) return false;
    
    // Validazione coordinate
    double ra = targetStar.coordinates.getRightAscension();
    double dec = targetStar.coordinates.getDeclination();
    if (ra < 0.0 || ra > 360.0) return false;
    if (dec < -90.0 || dec > 90.0) return false;
    
    // Validazione magnitudini
    if (targetStar.magnitude < -2.0 || targetStar.magnitude > 20.0) return false;
    if (asteroid.magnitude < 0.0 || asteroid.magnitude > 25.0) return false;
    
    // Validazione durata
    if (circumstances.duration < 0.0 || circumstances.duration > 3600.0) return false;
    
    return true;
}

OccultationChartConfig OccultationChartConfig::getDefaultForType(ChartType type) {
    OccultationChartConfig config;
    config.chartType = type;
    
    switch (type) {
        case ChartType::APPROACH:
            // Campo largo per localizzare la regione
            config.fieldOfViewWidth = 10.0;
            config.fieldOfViewHeight = 10.0;
            config.limitingMagnitude = 12.0;
            config.pathDurationHours = 3.0;
            config.showGrid = true;
            config.showAsteroidPath = true;
            break;
            
        case ChartType::DETAIL:
            // Campo stretto per osservazione dettagliata
            config.fieldOfViewWidth = 3.0;
            config.fieldOfViewHeight = 1.5;
            config.limitingMagnitude = 15.0;
            config.pathDurationHours = 1.0;
            config.showGrid = true;
            config.showAsteroidPath = true;
            break;
            
        case ChartType::FINDER:
            // Campo medio, via di mezzo
            config.fieldOfViewWidth = 5.0;
            config.fieldOfViewHeight = 5.0;
            config.limitingMagnitude = 13.0;
            config.pathDurationHours = 2.0;
            config.showGrid = true;
            config.showAsteroidPath = true;
            break;
            
        case ChartType::WIDE_FIELD:
            // Campo molto ampio per overview
            config.fieldOfViewWidth = 20.0;
            config.fieldOfViewHeight = 20.0;
            config.limitingMagnitude = 10.0;
            config.pathDurationHours = 6.0;
            config.showGrid = true;
            config.showAsteroidPath = true;
            break;
            
        case ChartType::CUSTOM:
            // Configurazione di default generica
            config.fieldOfViewWidth = 5.0;
            config.fieldOfViewHeight = 5.0;
            config.limitingMagnitude = 13.0;
            config.pathDurationHours = 2.0;
            break;
    }
    
    return config;
}

} // namespace occultation
} // namespace starmap
