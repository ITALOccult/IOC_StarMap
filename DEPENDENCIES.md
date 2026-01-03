# Dipendenze

## ⭐ IOC_GaiaLib (OBBLIGATORIO)

StarMap richiede IOC_GaiaLib per accedere al catalogo GAIA DR3.

### Installazione da GitHub

```bash
# Clone repository
git clone https://github.com/manvalan/IOC_GaiaLib.git
cd IOC_GaiaLib

# Build e installa
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
sudo cmake --install .
```

### Catalogo Locale Mag18 V2 (Raccomandato)

Per query offline veloci (stelle G≤18, copre 95% use cases):

```bash
# Scarica o genera catalogo Mag18 V2 (14 GB)
# Vedi: https://github.com/manvalan/IOC_GaiaLib/blob/main/docs/MAG18_V2_QUICKSTART.md

# Percorso standard
mkdir -p ~/catalogs
# Posiziona gaia_mag18_v2.cat in ~/catalogs/
```

StarMap utilizzerà automaticamente il catalogo locale se disponibile in `~/catalogs/gaia_mag18_v2.cat`.

## libcurl

Richiesto da IOC_GaiaLib per query online.

Su macOS (con Homebrew):
```bash
brew install curl
```

Su Ubuntu/Debian:
```bash
sudo apt-get install libcurl4-openssl-dev
```

Su Fedora/RHEL:
```bash
sudo dnf install libcurl-devel
```

## SQLite3

Richiesto per il database locale di cross-match Gaia-SAO.

Su macOS (con Homebrew):
```bash
brew install sqlite3
```

Su Ubuntu/Debian:
```bash
sudo apt-get install libsqlite3-dev
```

Su Fedora/RHEL:
```bash
sudo dnf install sqlite-devel
```

## nlohmann/json

La libreria può utilizzare una versione di sistema o quella bundled.

### Installazione di Sistema

Su macOS (con Homebrew):
```bash
brew install nlohmann-json
```

Su Ubuntu/Debian (Ubuntu 20.04+):
```bash
sudo apt-get install nlohmann-json3-dev
```

### Versione Bundled

Se non disponibile nel sistema, scarica nella directory `external/`:

```bash
mkdir -p external/json
cd external/json
wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
```

Oppure come submodule git:

```bash
git submodule add https://github.com/nlohmann/json.git external/json
git submodule update --init --recursive
```

## stb_image_write (Opzionale)

Per salvare immagini PNG/JPEG, puoi usare stb_image_write:

```bash
mkdir -p external/stb
cd external/stb
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
```

Quindi modifica `src/map/MapRenderer.cpp` per includere:

```cpp
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool ImageBuffer::saveAsPNG(const std::string& filename) const {
    return stbi_write_png(filename.c_str(), width, height, 4, 
                         data.data(), width * 4) != 0;
}
```

## Build Completo

```bash
# 1. Installa dipendenze di sistema (Ubuntu/Debian)
sudo apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libxml2-dev \
    zlib1g-dev \
    libsqlite3-dev \
    nlohmann-json3-dev

# 2. Installa IOC_GaiaLib
git clone https://github.com/manvalan/IOC_GaiaLib.git
cd IOC_GaiaLib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
sudo cmake --install .
cd ../..

# 3. Clone e build StarMap
git clone https://github.com/manvalan/IOC_StarMap.git
cd IOC_StarMap
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
cmake --build . -j$(nproc)

# 4. Test
./examples/example_basic

# 5. Installa
sudo cmake --install .
```

## Troubleshooting

### Errore: "Could not find CURL"

Assicurati che libcurl sia installato e CMake possa trovarlo:

```bash
pkg-config --cflags --libs libcurl
```

Se necessario, specifica manualmente:

```bash
cmake .. -DCURL_INCLUDE_DIR=/path/to/curl/include \
         -DCURL_LIBRARY=/path/to/libcurl.so
```

### Errore: "nlohmann/json.hpp not found"

Usa la versione bundled oppure installa:

```bash
# Ubuntu 20.04+
sudo apt-get install nlohmann-json3-dev

# Altrimenti scarica header-only
mkdir -p external/json
cd external/json
wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
```

### Linking Errors

Se hai problemi di linking, verifica quale libreria è stata generata:

```bash
ls -la build/libstarmap*
```

Per linking statico:
```bash
cmake .. -DBUILD_SHARED_LIBS=OFF
```

Per linking dinamico:
```bash
cmake .. -DBUILD_SHARED_LIBS=ON
```
