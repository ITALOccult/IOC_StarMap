#include "starmap/map/Projection.h"
#include <cmath>
#include <stdexcept>

namespace starmap {
namespace map {

// ============================================================================
// ProjectionFactory
// ============================================================================

std::unique_ptr<Projection> ProjectionFactory::create(
    ProjectionType type,
    const core::EquatorialCoordinates& center,
    double fovWidth,
    double fovHeight) {
    
    switch (type) {
        case ProjectionType::STEREOGRAPHIC:
            return std::make_unique<StereographicProjection>(center, fovWidth, fovHeight);
        
        case ProjectionType::GNOMONIC:
            return std::make_unique<GnomonicProjection>(center, fovWidth, fovHeight);
        
        case ProjectionType::ORTHOGRAPHIC:
            return std::make_unique<OrthographicProjection>(center, fovWidth, fovHeight);
        
        default:
            throw std::runtime_error("Unsupported projection type");
    }
}

// ============================================================================
// StereographicProjection
// ============================================================================

StereographicProjection::StereographicProjection(
    const core::EquatorialCoordinates& center,
    double fovWidth, double fovHeight)
    : center_(center), fovWidth_(fovWidth), fovHeight_(fovHeight) {
    
    // Calcola scala basata sul FOV
    scale_ = 2.0 / std::tan((fovWidth * M_PI / 180.0) / 2.0);
}

core::CartesianCoordinates StereographicProjection::project(
    const core::EquatorialCoordinates& celestial) const {
    
    // Converti in radianti
    double ra = celestial.getRightAscension() * M_PI / 180.0;
    double dec = celestial.getDeclination() * M_PI / 180.0;
    double ra0 = center_.getRightAscension() * M_PI / 180.0;
    double dec0 = center_.getDeclination() * M_PI / 180.0;
    
    // Differenza in RA
    double dRA = ra - ra0;
    
    // Formula della proiezione stereografica
    double cosDec = std::cos(dec);
    double sinDec = std::sin(dec);
    double cosDec0 = std::cos(dec0);
    double sinDec0 = std::sin(dec0);
    double cosDRA = std::cos(dRA);
    
    double k = scale_ / (1.0 + sinDec0 * sinDec + cosDec0 * cosDec * cosDRA);
    
    double x = k * cosDec * std::sin(dRA);
    double y = k * (cosDec0 * sinDec - sinDec0 * cosDec * cosDRA);
    
    return core::CartesianCoordinates(x, y);
}

core::EquatorialCoordinates StereographicProjection::unproject(
    const core::CartesianCoordinates& cartesian) const {
    
    double x = cartesian.getX();
    double y = cartesian.getY();
    
    double ra0 = center_.getRightAscension() * M_PI / 180.0;
    double dec0 = center_.getDeclination() * M_PI / 180.0;
    
    double rho = std::sqrt(x * x + y * y);
    double c = 2.0 * std::atan(rho / scale_);
    
    double sinC = std::sin(c);
    double cosC = std::cos(c);
    double sinDec0 = std::sin(dec0);
    double cosDec0 = std::cos(dec0);
    
    double dec = std::asin(cosC * sinDec0 + (y * sinC * cosDec0) / rho);
    double ra = ra0 + std::atan2(x * sinC, rho * cosDec0 * cosC - y * sinDec0 * sinC);
    
    return core::EquatorialCoordinates(ra * 180.0 / M_PI, dec * 180.0 / M_PI);
}

bool StereographicProjection::isVisible(
    const core::EquatorialCoordinates& celestial) const {
    
    auto projected = project(celestial);
    double x = projected.getX();
    double y = projected.getY();
    
    // Verifica se dentro il FOV
    double aspectRatio = fovWidth_ / fovHeight_;
    return (std::abs(x) <= aspectRatio && std::abs(y) <= 1.0);
}

void StereographicProjection::setCenter(const core::EquatorialCoordinates& center) {
    center_ = center;
}

void StereographicProjection::setFieldOfView(double widthDeg, double heightDeg) {
    fovWidth_ = widthDeg;
    fovHeight_ = heightDeg;
    scale_ = 2.0 / std::tan((fovWidth_ * M_PI / 180.0) / 2.0);
}

// ============================================================================
// GnomonicProjection
// ============================================================================

GnomonicProjection::GnomonicProjection(
    const core::EquatorialCoordinates& center,
    double fovWidth, double fovHeight)
    : center_(center), fovWidth_(fovWidth), fovHeight_(fovHeight) {
}

core::CartesianCoordinates GnomonicProjection::project(
    const core::EquatorialCoordinates& celestial) const {
    
    double ra = celestial.getRightAscension() * M_PI / 180.0;
    double dec = celestial.getDeclination() * M_PI / 180.0;
    double ra0 = center_.getRightAscension() * M_PI / 180.0;
    double dec0 = center_.getDeclination() * M_PI / 180.0;
    
    double dRA = ra - ra0;
    double cosDec = std::cos(dec);
    double sinDec = std::sin(dec);
    double cosDec0 = std::cos(dec0);
    double sinDec0 = std::sin(dec0);
    double cosDRA = std::cos(dRA);
    
    double cosC = sinDec0 * sinDec + cosDec0 * cosDec * cosDRA;
    
    if (cosC <= 0) {
        // Punto dietro il piano di proiezione
        return core::CartesianCoordinates(1e10, 1e10);
    }
    
    double x = cosDec * std::sin(dRA) / cosC;
    double y = (cosDec0 * sinDec - sinDec0 * cosDec * cosDRA) / cosC;
    
    // Normalizza al FOV
    double scale = 180.0 / (fovWidth_ * M_PI);
    x *= scale;
    y *= scale;
    
    return core::CartesianCoordinates(x, y);
}

core::EquatorialCoordinates GnomonicProjection::unproject(
    const core::CartesianCoordinates& cartesian) const {
    
    double scale = (fovWidth_ * M_PI) / 180.0;
    double x = cartesian.getX() / scale;
    double y = cartesian.getY() / scale;
    
    double ra0 = center_.getRightAscension() * M_PI / 180.0;
    double dec0 = center_.getDeclination() * M_PI / 180.0;
    
    double rho = std::sqrt(x * x + y * y);
    double c = std::atan(rho);
    
    double sinC = std::sin(c);
    double cosC = std::cos(c);
    double sinDec0 = std::sin(dec0);
    double cosDec0 = std::cos(dec0);
    
    double dec = std::asin(cosC * sinDec0 + (y * sinC * cosDec0) / rho);
    double ra = ra0 + std::atan2(x * sinC, rho * cosDec0 * cosC - y * sinDec0 * sinC);
    
    return core::EquatorialCoordinates(ra * 180.0 / M_PI, dec * 180.0 / M_PI);
}

bool GnomonicProjection::isVisible(const core::EquatorialCoordinates& celestial) const {
    auto projected = project(celestial);
    double x = projected.getX();
    double y = projected.getY();
    
    double aspectRatio = fovWidth_ / fovHeight_;
    return (std::abs(x) <= aspectRatio && std::abs(y) <= 1.0 && 
            x < 1e9 && y < 1e9); // Controlla se non dietro
}

void GnomonicProjection::setCenter(const core::EquatorialCoordinates& center) {
    center_ = center;
}

void GnomonicProjection::setFieldOfView(double widthDeg, double heightDeg) {
    fovWidth_ = widthDeg;
    fovHeight_ = heightDeg;
}

// ============================================================================
// OrthographicProjection
// ============================================================================

OrthographicProjection::OrthographicProjection(
    const core::EquatorialCoordinates& center,
    double fovWidth, double fovHeight)
    : center_(center), fovWidth_(fovWidth), fovHeight_(fovHeight) {
}

core::CartesianCoordinates OrthographicProjection::project(
    const core::EquatorialCoordinates& celestial) const {
    
    double ra = celestial.getRightAscension() * M_PI / 180.0;
    double dec = celestial.getDeclination() * M_PI / 180.0;
    double ra0 = center_.getRightAscension() * M_PI / 180.0;
    double dec0 = center_.getDeclination() * M_PI / 180.0;
    
    double dRA = ra - ra0;
    
    double x = std::cos(dec) * std::sin(dRA);
    double y = std::cos(dec0) * std::sin(dec) - 
               std::sin(dec0) * std::cos(dec) * std::cos(dRA);
    
    // Normalizza al FOV
    double scale = 180.0 / (fovWidth_ * M_PI);
    x *= scale;
    y *= scale;
    
    return core::CartesianCoordinates(x, y);
}

core::EquatorialCoordinates OrthographicProjection::unproject(
    const core::CartesianCoordinates& cartesian) const {
    
    double scale = (fovWidth_ * M_PI) / 180.0;
    double x = cartesian.getX() / scale;
    double y = cartesian.getY() / scale;
    
    double ra0 = center_.getRightAscension() * M_PI / 180.0;
    double dec0 = center_.getDeclination() * M_PI / 180.0;
    
    double rho = std::sqrt(x * x + y * y);
    
    if (rho > 1.0) {
        // Fuori dalla sfera proiettata
        return center_;
    }
    
    double c = std::asin(rho);
    double sinC = std::sin(c);
    double cosC = std::cos(c);
    
    double dec = std::asin(cosC * std::sin(dec0) + y * sinC * std::cos(dec0) / rho);
    double ra = ra0 + std::atan2(x * sinC, 
                                 rho * std::cos(dec0) * cosC - y * std::sin(dec0) * sinC);
    
    return core::EquatorialCoordinates(ra * 180.0 / M_PI, dec * 180.0 / M_PI);
}

bool OrthographicProjection::isVisible(const core::EquatorialCoordinates& celestial) const {
    double ra = celestial.getRightAscension() * M_PI / 180.0;
    double dec = celestial.getDeclination() * M_PI / 180.0;
    double ra0 = center_.getRightAscension() * M_PI / 180.0;
    double dec0 = center_.getDeclination() * M_PI / 180.0;
    
    double dRA = ra - ra0;
    
    // Visibile se sul lato frontale della sfera
    double cosC = std::sin(dec0) * std::sin(dec) + 
                  std::cos(dec0) * std::cos(dec) * std::cos(dRA);
    
    return cosC > 0;
}

void OrthographicProjection::setCenter(const core::EquatorialCoordinates& center) {
    center_ = center;
}

void OrthographicProjection::setFieldOfView(double widthDeg, double heightDeg) {
    fovWidth_ = widthDeg;
    fovHeight_ = heightDeg;
}

} // namespace map
} // namespace starmap
