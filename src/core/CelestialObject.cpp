#include "starmap/core/CelestialObject.h"
#include <cmath>

namespace starmap {
namespace core {

std::optional<double> Star::getAbsoluteMagnitude() const {
    auto dist = getDistance();
    if (dist.has_value() && dist.value() > 0.0) {
        // M = m - 5 * log10(d) + 5
        // dove d è in parsec
        double M = magnitude_ - 5.0 * std::log10(dist.value()) + 5.0;
        return M;
    }
    return std::nullopt;
}

} // namespace core
} // namespace starmap
