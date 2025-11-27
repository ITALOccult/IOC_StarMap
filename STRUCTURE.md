# Struttura del Progetto StarMap

```
IOC_StarMap/
│
├── CMakeLists.txt                  # Build system principale
├── LICENSE                         # Licenza MIT
├── README.md                       # Documentazione principale
├── BUILD.md                        # Istruzioni di build dettagliate
├── QUICKSTART.md                   # Guida rapida per iniziare
├── DEPENDENCIES.md                 # Informazioni sulle dipendenze
├── download_deps.sh                # Script per scaricare dipendenze
├── .gitignore                      # File da ignorare in git
│
├── cmake/
│   └── StarMapConfig.cmake.in     # Template configurazione CMake
│
├── include/                        # Header pubblici della libreria
│   └── starmap/
│       ├── StarMap.h              # Header principale (include tutto)
│       │
│       ├── core/                  # Componenti fondamentali
│       │   ├── Coordinates.h     # Coordinate celesti (RA/Dec, Gal, ecc.)
│       │   └── CelestialObject.h # Stelle e oggetti celesti
│       │
│       ├── catalog/               # Accesso ai cataloghi
│       │   ├── GaiaClient.h      # Client per GAIA DR3
│       │   ├── SAOCatalog.h      # Catalogo SAO
│       │   └── CatalogManager.h  # Manager unificato cataloghi
│       │
│       ├── map/                   # Generazione mappe
│       │   ├── MapConfiguration.h # Configurazione completa mappa
│       │   ├── Projection.h       # Proiezioni cartografiche
│       │   ├── MapRenderer.h      # Rendering mappa
│       │   └── GridRenderer.h     # Rendering griglia coordinate
│       │
│       ├── config/                # Sistema di configurazione
│       │   ├── ConfigurationLoader.h  # Interfaccia loader
│       │   └── JSONConfigLoader.h     # Loader JSON
│       │
│       └── utils/                 # Utilità
│           └── HttpClient.h       # Client HTTP per query online
│
├── src/                            # Implementazioni
│   ├── core/
│   │   ├── Coordinates.cpp
│   │   └── CelestialObject.cpp
│   │
│   ├── catalog/
│   │   ├── GaiaClient.cpp         # Query TAP/ADQL a GAIA
│   │   ├── SAOCatalog.cpp         # Cross-match SAO via VizieR/SIMBAD
│   │   └── CatalogManager.cpp
│   │
│   ├── map/
│   │   ├── MapConfiguration.cpp
│   │   ├── Projection.cpp         # Stereographic, Gnomonic, Orthographic
│   │   ├── MapRenderer.cpp        # Rendering stelle e immagine
│   │   └── GridRenderer.cpp       # Griglia RA/Dec e overlay
│   │
│   ├── config/
│   │   ├── ConfigurationLoader.cpp
│   │   └── JSONConfigLoader.cpp   # Serializzazione/deserializzazione JSON
│   │
│   └── utils/
│       └── HttpClient.cpp         # Wrapper libcurl
│
├── examples/                       # Programmi di esempio
│   ├── CMakeLists.txt
│   ├── basic_usage.cpp            # Esempio base
│   ├── json_config.cpp            # Caricamento da JSON
│   ├── programmatic_config.cpp    # Configurazione completa via codice
│   ├── gaia_query.cpp             # Query diretta GAIA
│   │
│   └── config_examples/           # File JSON di esempio
│       ├── orion.json             # Configurazione per Orione
│       ├── andromeda.json         # Configurazione per M31
│       └── custom.json            # Template personalizzabile
│
└── external/                       # Dipendenze esterne (opzionali)
    ├── CMakeLists.txt
    ├── json/                       # nlohmann/json (se non nel sistema)
    └── stb/                        # stb_image_write (opzionale)
```

## Componenti Principali

### Core (`include/starmap/core/`)

**Coordinates.h/cpp**
- `EquatorialCoordinates`: RA/Dec (J2000)
- `GalacticCoordinates`: l/b
- `HorizontalCoordinates`: Alt/Az
- `CartesianCoordinates`: x/y per rendering
- Conversioni tra sistemi
- Formattazione HMS/DMS

**CelestialObject.h/cpp**
- `CelestialObject`: Classe base per oggetti celesti
- `Star`: Specializzazione per stelle
- Proprietà: magnitudine, coordinate, parallasse, moto proprio
- Identificatori: GAIA ID, SAO number

### Catalog (`include/starmap/catalog/`)

**GaiaClient.h/cpp**
- Query al servizio TAP di GAIA DR3
- ADQL query builder
- Parsing VOTable (XML)
- Query per regione, box, singolo oggetto
- Parametri configurabili (magnitudine, FOV, ecc.)

**SAOCatalog.h/cpp**
- Cross-match con catalogo SAO
- Query VizieR per coordinate
- Query SIMBAD per ID GAIA
- Arricchimento stelle con numeri SAO

**CatalogManager.h/cpp**
- Interfaccia unificata per cataloghi multipli
- Gestione cache
- Arricchimento parallelo (opzionale)

### Map (`include/starmap/map/`)

**MapConfiguration.h/cpp**
- Configurazione completa mappa
- Centro, FOV, dimensioni immagine
- Tipo proiezione, sistema coordinate
- Stili: griglia, stelle, colori
- Overlay: equatore, eclittica, bussola
- Validazione parametri

**Projection.h/cpp**
- Interfaccia `Projection`
- `StereographicProjection`: Standard per mappe celesti
- `GnomonicProjection`: Linee rette = cerchi massimi
- `OrthographicProjection`: Vista dall'infinito
- Proiezione/deproiezione coordinate
- Test visibilità

**MapRenderer.h/cpp**
- Rendering completo mappa
- `ImageBuffer`: Buffer RGBA
- Disegno stelle con dimensioni proporzionali a magnitudine
- Colori spettrali basati su B-V
- Antialiasing per simboli
- Esportazione PNG/JPEG (con stb)

**GridRenderer.h/cpp**
- Griglia coordinate RA/Dec
- Etichette coordinate
- Linee equatore/eclittica
- Rosa dei venti
- Scala grafica

### Config (`include/starmap/config/`)

**ConfigurationLoader.h/cpp**
- Interfaccia per loader configurazione
- Factory pattern per formati multipli
- Supporto: JSON (implementato), XML, YAML, INI (future)

**JSONConfigLoader.h/cpp**
- Serializzazione/deserializzazione JSON
- Conversione tra `MapConfiguration` e JSON
- Carica/salva da file o stringa
- Usa nlohmann/json

### Utils (`include/starmap/utils/`)

**HttpClient.h/cpp**
- Wrapper per libcurl
- GET e POST requests
- Headers personalizzati
- Timeout configurabile
- Error handling

## Flusso di Utilizzo

### 1. Configurazione
```
MapConfiguration → JSONConfigLoader (opzionale) → StarMapBuilder
```

### 2. Query Cataloghi
```
GaiaClient → Query GAIA DR3 → Stars
SAOCatalog → Cross-match SAO → Arricchimento
```

### 3. Rendering
```
MapConfiguration + Stars → Projection → MapRenderer → ImageBuffer → PNG/JPEG
```

## Build Artifacts

### Libreria Statica
```
build/libstarmap.a          (Linux/macOS)
build/starmap.lib           (Windows)
```

### Libreria Dinamica
```
build/libstarmap.so         (Linux)
build/libstarmap.dylib      (macOS)
build/starmap.dll           (Windows)
```

### Esempi Compilati
```
build/examples/example_basic
build/examples/example_json
build/examples/example_programmatic
build/examples/example_gaia
```

## Installazione (dopo `make install`)

```
/usr/local/
├── include/starmap/        # Header pubblici
│   ├── StarMap.h
│   ├── core/
│   ├── catalog/
│   ├── map/
│   ├── config/
│   └── utils/
│
├── lib/
│   ├── libstarmap.so       # Libreria condivisa
│   ├── libstarmap.a        # Libreria statica (se BUILD_SHARED_LIBS=OFF)
│   └── cmake/StarMap/      # File CMake per find_package
│
├── bin/examples/           # Esempi (opzionale)
│   ├── example_basic
│   └── ...
│
└── share/starmap/examples/ # File di configurazione esempio
    ├── orion.json
    └── ...
```

## Dipendenze Runtime

### Obbligatorie
- libcurl (per query cataloghi online)

### Opzionali
- nlohmann/json (header-only, può essere bundled)
- stb_image_write (header-only, per PNG/JPEG export)

## Note Tecniche

- **C++ Standard**: C++17
- **Thread Safety**: GaiaClient e SAOCatalog sono thread-safe per lettura
- **Memory Management**: Smart pointers (unique_ptr, shared_ptr)
- **Error Handling**: Exceptions per errori critici
- **Coordinamento**: Sistema J2000.0 (ICRS)
- **Encoding**: UTF-8 per stringhe

## Estensibilità

### Aggiungere Nuova Proiezione
1. Deriva da `Projection`
2. Implementa `project()`, `unproject()`, `isVisible()`
3. Aggiungi a `ProjectionFactory`

### Aggiungere Nuovo Catalogo
1. Crea classe simile a `GaiaClient`
2. Implementa query specifiche
3. Integra in `CatalogManager`

### Aggiungere Formato Config
1. Deriva da `ConfigurationLoader`
2. Implementa `load()`, `save()`
3. Aggiungi a `ConfigurationLoaderFactory`

---

**Versione**: 1.0.0
**Ultima modifica**: 2025-11-27
