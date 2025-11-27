#ifndef STARMAP_PROJECTION_H
#define STARMAP_PROJECTION_H

#include "starmap/core/Coordinates.h"
#include "MapConfiguration.h"
#include <memory>

namespace starmap {
namespace map {

/**
 * @brief Classe base per proiezioni cartografiche celesti
 */
class Projection {
public:
    virtual ~Projection() = default;

    /**
     * @brief Proietta coordinate celesti su piano cartesiano
     * @param celestial Coordinate equatoriali
     * @return Coordinate cartesiane normalizzate (-1 a +1)
     */
    virtual core::CartesianCoordinates project(
        const core::EquatorialCoordinates& celestial) const = 0;

    /**
     * @brief Proiezione inversa: da cartesiane a celesti
     * @param cartesian Coordinate cartesiane normalizzate
     * @return Coordinate equatoriali
     */
    virtual core::EquatorialCoordinates unproject(
        const core::CartesianCoordinates& cartesian) const = 0;

    /**
     * @brief Verifica se un punto è visibile nella proiezione
     */
    virtual bool isVisible(const core::EquatorialCoordinates& celestial) const = 0;

    /**
     * @brief Imposta il centro della proiezione
     */
    virtual void setCenter(const core::EquatorialCoordinates& center) = 0;

    /**
     * @brief Imposta il campo di vista
     */
    virtual void setFieldOfView(double widthDeg, double heightDeg) = 0;
};

/**
 * @brief Factory per creare proiezioni
 */
class ProjectionFactory {
public:
    static std::unique_ptr<Projection> create(
        ProjectionType type,
        const core::EquatorialCoordinates& center,
        double fovWidth,
        double fovHeight);
};

/**
 * @brief Proiezione stereografica (standard per mappe celesti)
 */
class StereographicProjection : public Projection {
public:
    StereographicProjection(const core::EquatorialCoordinates& center,
                           double fovWidth, double fovHeight);

    core::CartesianCoordinates project(
        const core::EquatorialCoordinates& celestial) const override;
    
    core::EquatorialCoordinates unproject(
        const core::CartesianCoordinates& cartesian) const override;
    
    bool isVisible(const core::EquatorialCoordinates& celestial) const override;
    
    void setCenter(const core::EquatorialCoordinates& center) override;
    void setFieldOfView(double widthDeg, double heightDeg) override;

private:
    core::EquatorialCoordinates center_;
    double fovWidth_;
    double fovHeight_;
    double scale_;
};

/**
 * @brief Proiezione gnomica (tangenziale)
 */
class GnomonicProjection : public Projection {
public:
    GnomonicProjection(const core::EquatorialCoordinates& center,
                      double fovWidth, double fovHeight);

    core::CartesianCoordinates project(
        const core::EquatorialCoordinates& celestial) const override;
    
    core::EquatorialCoordinates unproject(
        const core::CartesianCoordinates& cartesian) const override;
    
    bool isVisible(const core::EquatorialCoordinates& celestial) const override;
    
    void setCenter(const core::EquatorialCoordinates& center) override;
    void setFieldOfView(double widthDeg, double heightDeg) override;

private:
    core::EquatorialCoordinates center_;
    double fovWidth_;
    double fovHeight_;
};

/**
 * @brief Proiezione ortografica
 */
class OrthographicProjection : public Projection {
public:
    OrthographicProjection(const core::EquatorialCoordinates& center,
                          double fovWidth, double fovHeight);

    core::CartesianCoordinates project(
        const core::EquatorialCoordinates& celestial) const override;
    
    core::EquatorialCoordinates unproject(
        const core::CartesianCoordinates& cartesian) const override;
    
    bool isVisible(const core::EquatorialCoordinates& celestial) const override;
    
    void setCenter(const core::EquatorialCoordinates& center) override;
    void setFieldOfView(double widthDeg, double heightDeg) override;

private:
    core::EquatorialCoordinates center_;
    double fovWidth_;
    double fovHeight_;
};

} // namespace map
} // namespace starmap

#endif // STARMAP_PROJECTION_H
