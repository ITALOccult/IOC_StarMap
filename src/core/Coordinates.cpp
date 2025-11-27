#include "starmap/core/Coordinates.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace starmap {
namespace core {

std::string EquatorialCoordinates::toHMSString() const {
    double hours = getRaInHours();
    int h = static_cast<int>(hours);
    double remainder = (hours - h) * 60.0;
    int m = static_cast<int>(remainder);
    double s = (remainder - m) * 60.0;
    
    std::ostringstream oss;
    oss << std::setfill('0') 
        << std::setw(2) << h << "h"
        << std::setw(2) << m << "m"
        << std::fixed << std::setprecision(2) << std::setw(5) << s << "s";
    return oss.str();
}

std::string EquatorialCoordinates::toDMSString() const {
    double absDec = std::abs(dec_);
    int d = static_cast<int>(absDec);
    double remainder = (absDec - d) * 60.0;
    int m = static_cast<int>(remainder);
    double s = (remainder - m) * 60.0;
    
    std::ostringstream oss;
    oss << (dec_ >= 0 ? "+" : "-")
        << std::setfill('0')
        << std::setw(2) << d << "°"
        << std::setw(2) << m << "'"
        << std::fixed << std::setprecision(1) << std::setw(4) << s << "\"";
    return oss.str();
}

double EquatorialCoordinates::angularDistance(const EquatorialCoordinates& other) const {
    // Formula dell'haversine
    double ra1 = ra_ * M_PI / 180.0;
    double dec1 = dec_ * M_PI / 180.0;
    double ra2 = other.ra_ * M_PI / 180.0;
    double dec2 = other.dec_ * M_PI / 180.0;
    
    double dRA = ra2 - ra1;
    double dDec = dec2 - dec1;
    
    double a = std::sin(dDec / 2.0) * std::sin(dDec / 2.0) +
               std::cos(dec1) * std::cos(dec2) *
               std::sin(dRA / 2.0) * std::sin(dRA / 2.0);
    
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    
    return c * 180.0 / M_PI; // Ritorna in gradi
}

GalacticCoordinates GalacticCoordinates::fromEquatorial(const EquatorialCoordinates& eq) {
    // Coordinate del polo nord galattico in sistema equatoriale (J2000)
    const double raNGP = 192.859508;  // RA in gradi
    const double decNGP = 27.128336;  // Dec in gradi
    const double lCP = 122.932;       // Longitudine del polo celeste nord
    
    double ra = eq.getRightAscension() * M_PI / 180.0;
    double dec = eq.getDeclination() * M_PI / 180.0;
    double raNGPRad = raNGP * M_PI / 180.0;
    double decNGPRad = decNGP * M_PI / 180.0;
    
    double b = std::asin(std::sin(dec) * std::sin(decNGPRad) +
                        std::cos(dec) * std::cos(decNGPRad) * std::cos(ra - raNGPRad));
    
    double y = std::cos(dec) * std::sin(ra - raNGPRad);
    double x = std::sin(dec) * std::cos(decNGPRad) -
               std::cos(dec) * std::sin(decNGPRad) * std::cos(ra - raNGPRad);
    
    double l = std::atan2(y, x) * 180.0 / M_PI + lCP;
    if (l < 0.0) l += 360.0;
    if (l >= 360.0) l -= 360.0;
    
    return GalacticCoordinates(l, b * 180.0 / M_PI);
}

EquatorialCoordinates GalacticCoordinates::toEquatorial() const {
    // Inversione della trasformazione
    const double raNGP = 192.859508;
    const double decNGP = 27.128336;
    const double lCP = 122.932;
    
    double l = (l_ - lCP) * M_PI / 180.0;
    double b = b_ * M_PI / 180.0;
    double decNGPRad = decNGP * M_PI / 180.0;
    double raNGPRad = raNGP * M_PI / 180.0;
    
    double dec = std::asin(std::sin(b) * std::sin(decNGPRad) +
                          std::cos(b) * std::cos(decNGPRad) * std::cos(l));
    
    double y = std::cos(b) * std::sin(l);
    double x = std::sin(b) * std::cos(decNGPRad) -
               std::cos(b) * std::sin(decNGPRad) * std::cos(l);
    
    double ra = std::atan2(y, x) * 180.0 / M_PI + raNGP;
    if (ra < 0.0) ra += 360.0;
    if (ra >= 360.0) ra -= 360.0;
    
    return EquatorialCoordinates(ra, dec * 180.0 / M_PI);
}

} // namespace core
} // namespace starmap
