# StarMap Library - Guida Rapida

## Installazione

### macOS

```bash
# Installa dipendenze
brew install cmake curl nlohmann-json

# Clone e compila
git clone https://github.com/manvalan/IOC_StarMap.git
cd IOC_StarMap
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8
sudo make install
```

### Linux (Ubuntu/Debian)

```bash
# Installa dipendenze
sudo apt-get update
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev

# Clone e compila
git clone https://github.com/manvalan/IOC_StarMap.git
cd IOC_StarMap
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

## Uso Base

### 1. Creare una Mappa Semplice

Crea file `my_map.cpp`:

```cpp
#include <starmap/StarMap.h>

int main() {
    // Configurazione
    starmap::map::MapConfiguration config;
    config.center = starmap::core::EquatorialCoordinates(83.75, 5.0); // Orione
    config.fieldOfViewWidth = 10.0;
    config.fieldOfViewHeight = 10.0;
    config.imageWidth = 1920;
    config.imageHeight = 1080;
    config.limitingMagnitude = 10.0;
    config.title = "Orion Region";
    
    // Genera mappa
    starmap::StarMapBuilder builder;
    auto image = builder.generate(config);
    
    // Salva
    image.saveAsPNG("orion.png");
    
    return 0;
}
```

Compila:

```bash
g++ -std=c++17 my_map.cpp -lstarmap -lcurl -o my_map
./my_map
```

### 2. Usare Configurazione JSON

Crea `config.json`:

```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 10.0, "height": 10.0},
  "image": {"width": 1920, "height": 1080},
  "limiting_magnitude": 10.0,
  "display": {"title": "My Sky Map"}
}
```

Codice:

```cpp
#include <starmap/StarMap.h>

int main() {
    starmap::StarMapBuilder builder;
    auto image = builder.generateFromFile("config.json");
    image.saveAsPNG("output.png");
    return 0;
}
```

### 3. Query Diretta a GAIA

```cpp
#include <starmap/StarMap.h>
#include <iostream>

int main() {
    // Client GAIA
    starmap::catalog::GaiaClient gaia;
    
    // Query parametri
    starmap::catalog::GaiaQueryParameters params;
    params.center = starmap::core::EquatorialCoordinates(83.75, 5.0);
    params.radiusDegrees = 1.0;
    params.maxMagnitude = 8.0;
    
    // Esegui query
    auto stars = gaia.queryRegion(params);
    
    std::cout << "Trovate " << stars.size() << " stelle\n";
    
    // Mostra prima stella
    if (!stars.empty()) {
        auto star = stars[0];
        std::cout << "GAIA ID: " << star->getGaiaId() << "\n";
        std::cout << "Magnitudine: " << star->getMagnitude() << "\n";
    }
    
    return 0;
}
```

## Opzioni Configurazione Comuni

### Centro della Mappa

```cpp
// Da gradi
config.center = starmap::core::EquatorialCoordinates(ra_deg, dec_deg);

// Oggetti famosi:
// Orione: (83.75, 5.0)
// M31 Andromeda: (10.68, 41.27)
// Pleiadi: (56.75, 24.12)
// Centro Galattico: (266.4, -29.0)
```

### Campo di Vista

```cpp
config.fieldOfViewWidth = 10.0;   // gradi
config.fieldOfViewHeight = 10.0;  // gradi

// Wide field: 20-60°
// Medio: 5-20°
// Dettaglio: 1-5°
```

### Proiezioni

```cpp
config.projection = starmap::map::ProjectionType::STEREOGRAPHIC;
// Opzioni: STEREOGRAPHIC, GNOMONIC, ORTHOGRAPHIC, MERCATOR
```

### Griglia

```cpp
config.gridStyle.enabled = true;
config.gridStyle.raStepDegrees = 15.0;  // 1 ora in RA
config.gridStyle.decStepDegrees = 10.0;
config.gridStyle.showLabels = true;
```

### Stelle

```cpp
config.starStyle.useSpectralColors = true;  // Colori realistici
config.starStyle.showSAONumbers = true;     // Mostra numeri SAO
config.starStyle.minMagnitudeForLabel = 5.0; // Label solo per mag < 5
```

## CMake Integration

Nel tuo `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyStarMapApp)

set(CMAKE_CXX_STANDARD 17)

find_package(StarMap REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE StarMap::starmap)
```

## Tips & Tricks

### Performance

```cpp
// Cache per query multiple
builder.setCacheEnabled(true);

// Timeout più lungo per regioni complesse
builder.setCatalogTimeout(120);
```

### Qualità Immagine

```cpp
// Alta risoluzione
config.imageWidth = 3840;
config.imageHeight = 2160;

// Più stelle
config.limitingMagnitude = 14.0;
```

### Debug

```cpp
// Valida configurazione
if (!config.validate()) {
    std::cerr << "Configurazione non valida\n";
}

// Controlla visibilità
if (projection->isVisible(coords)) {
    // Stella visibile
}
```

## Esempi Precompilati

Dopo l'installazione, prova gli esempi:

```bash
# Esempio base
example_basic

# Da JSON
example_json examples/config_examples/orion.json

# Configurazione programmatica
example_programmatic

# Query GAIA
example_gaia
```

## Link Utili

- **Repository**: https://github.com/manvalan/IOC_StarMap
- **Issues**: https://github.com/manvalan/IOC_StarMap/issues
- **GAIA Archive**: https://gea.esac.esa.int/archive/
- **VizieR**: https://vizier.cds.unistra.fr/

## Supporto

Per domande o problemi, apri una issue su GitHub o contatta: michele.bigi@example.com

---

**Buona osservazione del cielo! 🌟**
