# StarMap Library

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![C++](https://img.shields.io/badge/C++-17-green.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

Una libreria C++ professionale per la generazione di mappe celesti di alta qualità, simili a quelle prodotte da [C2A](https://www.astrosurf.com/c2a/), utilizzando i dati del catalogo GAIA DR3 e numeri SAO.

## ✨ Caratteristiche

- 🌌 **Query GAIA DR3**: Accesso diretto al catalogo GAIA tramite servizio TAP/ADQL
- 🔢 **Numeri SAO**: Cross-matching automatico con catalogo SAO tramite VizieR/SIMBAD
- 🗺️ **Proiezioni Multiple**: Stereografica, Gnomica, Ortografica, Mercatore, Azimutale
- 🎨 **Colori Spettrali**: Rendering stelle con colori basati su tipo spettrale/indice colore
- 📐 **Griglia Configurabile**: Griglia coordinate RA/Dec personalizzabile
- ⚙️ **Altamente Configurabile**: Via codice C++, JSON, o file OOP
- 📦 **Libreria Statica/Dinamica**: Supporto per linking statico e dinamico
- 🎯 **Campo Variabile**: Da wide-field a dettagli di pochi gradi

## 🏗️ Architettura

```
starmap/
├── core/           # Coordinate celesti e oggetti astronomici
├── catalog/        # Accesso a GAIA, SAO e altri cataloghi
├── map/            # Proiezioni, rendering e configurazione mappe
├── config/         # Caricamento configurazioni (JSON, XML, YAML)
└── utils/          # HTTP client e utilità
```

## 📋 Requisiti

- **Compiler**: C++17 o superiore (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake**: 3.15+
- **Dipendenze**:
  - [IOC_GaiaLib](https://github.com/manvalan/IOC_GaiaLib) - **OBBLIGATORIO**: Accesso catalogo GAIA DR3
  - [libcurl](https://curl.se/libcurl/) - Per query HTTP ai cataloghi (richiesto da IOC_GaiaLib)
  - [nlohmann/json](https://github.com/nlohmann/json) - Per configurazione JSON
  - Opzionale: [stb_image_write](https://github.com/nothings/stb) - Per salvare PNG/JPEG
  - Opzionale: Catalogo Mag18 V2 (14 GB) - Per query offline veloci (stelle G≤18)

## 🚀 Installazione

### 1. Installa IOC_GaiaLib (Dipendenza Obbligatoria)

```bash
# Clone e compila IOC_GaiaLib
git clone https://github.com/manvalan/IOC_GaiaLib.git
cd IOC_GaiaLib

# Build e installa
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
sudo cmake --install .

# (Opzionale) Download catalogo Mag18 V2 per query offline veloci
# Consulta https://github.com/manvalan/IOC_GaiaLib/docs/MAG18_V2_QUICKSTART.md
```

### 2. Build StarMap

```bash
# Clone repository
git clone https://github.com/yourusername/IOC_StarMap.git
cd IOC_StarMap

# Crea directory build
mkdir build && cd build

# Configura con CMake (IOC_GaiaLib deve essere installata)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compila
cmake --build .

# Installa (opzionale)
sudo cmake --install .
```

### Opzioni CMake

```bash
cmake .. \
  -DBUILD_SHARED_LIBS=ON \     # ON per libreria dinamica, OFF per statica
  -DBUILD_EXAMPLES=ON \         # Compila esempi
  -DBUILD_TESTS=OFF             # Compila test (richiede Google Test)
```

## 💻 Uso Rapido

### Esempio Base

```cpp
#include <starmap/StarMap.h>

int main() {
    // Configura mappa
    starmap::map::MapConfiguration config;
    config.center = starmap::core::EquatorialCoordinates(83.75, 5.0); // Orione
    config.fieldOfViewWidth = 10.0;  // 10 gradi
    config.fieldOfViewHeight = 10.0;
    config.imageWidth = 1920;
    config.imageHeight = 1080;
    config.limitingMagnitude = 10.0;
    
    // Genera mappa
    starmap::StarMapBuilder builder;
    auto image = builder.generate(config, true);
    
    // Salva
    image.saveAsPNG("orion_map.png");
    
    return 0;
}
```

### Configurazione JSON

**config.json:**
```json
{
  "center": {
    "ra": 83.75,
    "dec": 5.0
  },
  "field_of_view": {
    "width": 10.0,
    "height": 10.0
  },
  "image": {
    "width": 1920,
    "height": 1080
  },
  "limiting_magnitude": 10.0,
  "stars": {
    "use_spectral_colors": true,
    "show_sao_numbers": true
  },
  "display": {
    "title": "Orion Region"
  }
}
```

**Codice:**
```cpp
#include <starmap/StarMap.h>

int main() {
    starmap::StarMapBuilder builder;
    auto image = builder.generateFromFile("config.json");
    image.saveAsPNG("output.png");
    return 0;
}
```

### Configurazione Programmatica Completa

```cpp
#include <starmap/StarMap.h>

int main() {
    starmap::map::MapConfiguration config;
    
    // Centro e FOV
    config.center = starmap::core::EquatorialCoordinates(10.68, 41.27); // M31
    config.fieldOfViewWidth = 5.0;
    config.fieldOfViewHeight = 5.0;
    
    // Immagine
    config.imageWidth = 2400;
    config.imageHeight = 1800;
    
    // Proiezione
    config.projection = starmap::map::ProjectionType::STEREOGRAPHIC;
    
    // Griglia
    config.gridStyle.enabled = true;
    config.gridStyle.raStepDegrees = 1.0;
    config.gridStyle.decStepDegrees = 1.0;
    config.gridStyle.showLabels = true;
    
    // Stelle
    config.starStyle.useSpectralColors = true;
    config.starStyle.showSAONumbers = true;
    config.starStyle.minMagnitudeForLabel = 6.0;
    
    // Display
    config.showTitle = true;
    config.title = "Andromeda Galaxy (M31)";
    config.showCompass = true;
    config.showEquator = true;
    
    // Genera
    starmap::StarMapBuilder builder;
    auto image = builder.generate(config, true);
    image.saveAsPNG("andromeda.png");
    
    return 0;
}
```

## 📚 Esempi

La directory `examples/` contiene diversi esempi:

- **basic_usage.cpp**: Uso base della libreria
- **json_config.cpp**: Caricamento da file JSON
- **programmatic_config.cpp**: Configurazione completa via codice
- **gaia_query.cpp**: Query diretta a GAIA senza rendering

### Compilare gli Esempi

```bash
cd build
cmake --build . --target example_basic
./examples/example_basic
```

## 🎨 Proiezioni Supportate

- **Stereographic**: Standard per mappe celesti, preserva angoli
- **Gnomonic**: Linee rette rappresentano cerchi massimi
- **Orthographic**: Vista "da lontano" della sfera celeste
- **Mercator**: Preserva direzioni
- **Azimuthal Equidistant**: Distanze dal centro proporzionali

## 🌟 Cataloghi

### GAIA DR3
- Query online tramite TAP/ADQL
- Magnitudine G fino a ~21
- Coordinate astrometriche precise
- Parallasse e moto proprio
- Fotometria BP/RP per colori

### SAO (Smithsonian Astrophysical Observatory)
- ~259,000 stelle con mag < 9
- Cross-matching automatico via VizieR
- Query SIMBAD per ID alternativi
- Numeri SAO storicamente importanti

## ⚙️ API Principale

### Classes Core

```cpp
// Coordinate
starmap::core::EquatorialCoordinates coords(ra, dec);
coords.toHMSString();  // "12h30m15.5s"
coords.toDMSString();  // "+45°30'20.0""

// Oggetti celesti
starmap::core::Star star;
star.setCoordinates(coords);
star.setMagnitude(4.5);
star.setSAONumber(12345);
```

### Cataloghi

```cpp
// GAIA client
starmap::catalog::GaiaClient gaia;
starmap::catalog::GaiaQueryParameters params;
params.center = coords;
params.radiusDegrees = 2.0;
params.maxMagnitude = 12.0;
auto stars = gaia.queryRegion(params);

// SAO catalog
starmap::catalog::SAOCatalog sao;
auto saoNum = sao.findSAOByCoordinates(coords);
```

### Rendering

```cpp
// Configurazione
starmap::map::MapConfiguration config;
// ... imposta parametri ...

// Rendering
starmap::map::MapRenderer renderer(config);
auto image = renderer.render(stars);

// Salva
image.saveAsPNG("output.png");
image.saveAsJPEG("output.jpg", 95);
```

### Configurazione

```cpp
// JSON loader
starmap::config::JSONConfigLoader loader;
auto config = loader.load("config.json");
loader.save(config, "output.json");

// String
std::string json = loader.saveToString(config);
auto config2 = loader.loadFromString(json);
```

## 🔧 Linking nella tua Applicazione

### CMake

```cmake
find_package(StarMap REQUIRED)
target_link_libraries(your_app PRIVATE StarMap::starmap)
```

### Manuale

```bash
# Compilazione
g++ -std=c++17 -I/usr/local/include your_app.cpp \
    -L/usr/local/lib -lstarmap -lcurl -o your_app
```

## 📖 Documentazione

La documentazione completa dell'API è generabile con Doxygen:

```bash
cd docs
doxygen Doxyfile
```

## 🤝 Contribuire

Contributi benvenuti! Per favore:

1. Fork del repository
2. Crea un branch per la tua feature (`git checkout -b feature/AmazingFeature`)
3. Commit delle modifiche (`git commit -m 'Add AmazingFeature'`)
4. Push al branch (`git push origin feature/AmazingFeature`)
5. Apri una Pull Request

## 📝 TODO

- [ ] Supporto per output SVG/PDF vettoriale
- [ ] Costellazioni (linee, confini, nomi)
- [ ] Via Lattea overlay
- [ ] Cache locale per cataloghi
- [ ] Supporto per altri cataloghi (Hipparcos, Tycho-2)
- [ ] Rendering testo con FreeType
- [ ] Proiezioni aggiuntive (Mollweide, Hammer-Aitoff)
- [ ] Animazioni per moto proprio
- [ ] GUI/Tool interattivo

## 📄 Licenza

Questo progetto è distribuito sotto licenza MIT. Vedi file `LICENSE` per dettagli.

## 🙏 Ringraziamenti

- **ESA GAIA Mission** per i dati astrometrici
- **CDS VizieR** e **SIMBAD** per l'accesso ai cataloghi
- **C2A** per l'ispirazione sulle mappe celesti
- **nlohmann/json** per l'eccellente libreria JSON C++

## 📧 Contatti

- **Autore**: Michele Bigi
- **Repository**: https://github.com/manvalan/IOC_StarMap
- **Issues**: https://github.com/manvalan/IOC_StarMap/issues

## 🌟 Esempi di Output

Le mappe generate includono:
- ⭐ Stelle con dimensioni proporzionali alla magnitudine
- 🎨 Colori spettrali realistici (blu → bianco → giallo → rosso)
- 📐 Griglia coordinate RA/Dec
- 🔢 Numeri SAO per stelle luminose
- 🧭 Rosa dei venti (N, E, S, W)
- 📏 Scala grafica
- 🏷️ Titolo e informazioni

---

**⭐ Se trovi utile questa libreria, lascia una stella su GitHub!**
