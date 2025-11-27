#ifndef STARMAP_COORDINATES_H
#define STARMAP_COORDINATES_H

#include <cmath>
#include <string>

namespace starmap {
namespace core {

/**
 * @brief Classe per gestire coordinate equatoriali (RA, Dec)
 */
class EquatorialCoordinates {
public:
    EquatorialCoordinates() : ra_(0.0), dec_(0.0) {}
    EquatorialCoordinates(double ra, double dec) : ra_(ra), dec_(dec) {}

    // Getters
    double getRightAscension() const { return ra_; }
    double getDeclination() const { return dec_; }
    
    // Setters
    void setRightAscension(double ra) { ra_ = ra; }
    void setDeclination(double dec) { dec_ = dec; }
    
    // RA in ore (0-24h)
    double getRaInHours() const { return ra_ / 15.0; }
    void setRaFromHours(double hours) { ra_ = hours * 15.0; }
    
    // Conversione in stringa HMS/DMS
    std::string toHMSString() const;
    std::string toDMSString() const;
    
    // Distanza angolare tra due coordinate
    double angularDistance(const EquatorialCoordinates& other) const;

private:
    double ra_;  // Right Ascension in gradi (0-360)
    double dec_; // Declination in gradi (-90 to +90)
};

/**
 * @brief Classe per coordinate altazimutali
 */
class HorizontalCoordinates {
public:
    HorizontalCoordinates() : altitude_(0.0), azimuth_(0.0) {}
    HorizontalCoordinates(double altitude, double azimuth) 
        : altitude_(altitude), azimuth_(azimuth) {}

    double getAltitude() const { return altitude_; }
    double getAzimuth() const { return azimuth_; }
    
    void setAltitude(double alt) { altitude_ = alt; }
    void setAzimuth(double az) { azimuth_ = az; }

private:
    double altitude_; // Altitudine in gradi (0-90)
    double azimuth_;  // Azimuth in gradi (0-360)
};

/**
 * @brief Coordinate galattiche
 */
class GalacticCoordinates {
public:
    GalacticCoordinates() : l_(0.0), b_(0.0) {}
    GalacticCoordinates(double l, double b) : l_(l), b_(b) {}

    double getL() const { return l_; }
    double getB() const { return b_; }
    
    void setL(double l) { l_ = l; }
    void setB(double b) { b_ = b; }
    
    // Conversione da/a coordinate equatoriali
    static GalacticCoordinates fromEquatorial(const EquatorialCoordinates& eq);
    EquatorialCoordinates toEquatorial() const;

private:
    double l_; // Longitudine galattica (0-360)
    double b_; // Latitudine galattica (-90 to +90)
};

/**
 * @brief Coordinate cartesiane per il rendering
 */
class CartesianCoordinates {
public:
    CartesianCoordinates() : x_(0.0), y_(0.0) {}
    CartesianCoordinates(double x, double y) : x_(x), y_(y) {}

    double getX() const { return x_; }
    double getY() const { return y_; }
    
    void setX(double x) { x_ = x; }
    void setY(double y) { y_ = y; }

private:
    double x_;
    double y_;
};

} // namespace core
} // namespace starmap

#endif // STARMAP_COORDINATES_H
