#ifndef STARMAP_CELESTIAL_OBJECT_H
#define STARMAP_CELESTIAL_OBJECT_H

#include "Coordinates.h"
#include <string>
#include <optional>

namespace starmap {
namespace core {

/**
 * @brief Tipo di oggetto celeste
 */
enum class ObjectType {
    STAR,
    PLANET,
    GALAXY,
    NEBULA,
    CLUSTER,
    ASTEROID,
    COMET,
    UNKNOWN
};

/**
 * @brief Classe base per oggetti celesti
 */
class CelestialObject {
public:
    CelestialObject() 
        : type_(ObjectType::UNKNOWN), magnitude_(99.0), 
          gaiaId_(0), saoNumber_(0) {}
    
    virtual ~CelestialObject() = default;

    // Coordinate
    const EquatorialCoordinates& getCoordinates() const { return coordinates_; }
    void setCoordinates(const EquatorialCoordinates& coords) { coordinates_ = coords; }

    // Identificatori
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    long long getGaiaId() const { return gaiaId_; }
    void setGaiaId(long long id) { gaiaId_ = id; }
    
    std::optional<int> getSAONumber() const { 
        return saoNumber_ > 0 ? std::optional<int>(saoNumber_) : std::nullopt; 
    }
    void setSAONumber(int sao) { saoNumber_ = sao; }

    // Proprietà fisiche
    double getMagnitude() const { return magnitude_; }
    void setMagnitude(double mag) { magnitude_ = mag; }
    
    ObjectType getType() const { return type_; }
    void setType(ObjectType type) { type_ = type; }

    // Colore/Spettro
    const std::string& getSpectralType() const { return spectralType_; }
    void setSpectralType(const std::string& type) { spectralType_ = type; }
    
    // Parallasse (per distanza)
    std::optional<double> getParallax() const { 
        return parallax_ > 0.0 ? std::optional<double>(parallax_) : std::nullopt; 
    }
    void setParallax(double parallax) { parallax_ = parallax; }
    
    // Distanza in parsec (calcolata da parallasse)
    std::optional<double> getDistance() const {
        if (parallax_ > 0.0) {
            return 1000.0 / parallax_; // parallax in mas, distanza in pc
        }
        return std::nullopt;
    }

    // Moto proprio
    std::optional<double> getProperMotionRA() const {
        return pmRA_ != 0.0 ? std::optional<double>(pmRA_) : std::nullopt;
    }
    void setProperMotionRA(double pm) { pmRA_ = pm; }
    
    std::optional<double> getProperMotionDec() const {
        return pmDec_ != 0.0 ? std::optional<double>(pmDec_) : std::nullopt;
    }
    void setProperMotionDec(double pm) { pmDec_ = pm; }

protected:
    EquatorialCoordinates coordinates_;
    std::string name_;
    ObjectType type_;
    double magnitude_;
    
    // Identificatori catalogo
    long long gaiaId_;
    int saoNumber_;
    
    // Proprietà stellari
    std::string spectralType_;
    double parallax_;      // in milliarcsec
    double pmRA_;          // moto proprio in RA (mas/yr)
    double pmDec_;         // moto proprio in Dec (mas/yr)
};

/**
 * @brief Classe specializzata per stelle
 */
class Star : public CelestialObject {
public:
    Star() { type_ = ObjectType::STAR; }
    
    // Colore B-V, B-R, ecc.
    std::optional<double> getColorIndex() const {
        return colorIndex_ != 0.0 ? std::optional<double>(colorIndex_) : std::nullopt;
    }
    void setColorIndex(double ci) { colorIndex_ = ci; }
    
    // Magnitudine assoluta
    std::optional<double> getAbsoluteMagnitude() const;

private:
    double colorIndex_; // B-V color index
};

} // namespace core
} // namespace starmap

#endif // STARMAP_CELESTIAL_OBJECT_H
