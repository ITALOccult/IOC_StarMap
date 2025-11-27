# Build Instructions

## Prerequisites

### System Requirements

- **Operating System**: Linux, macOS, or Windows (with WSL/MSYS2)
- **Compiler**: 
  - GCC 7+ or Clang 5+ (Linux/macOS)
  - MSVC 2017+ (Windows)
  - Xcode 10+ (macOS)
- **CMake**: 3.15 or higher
- **Internet Connection**: Required for downloading dependencies and querying catalogs

### Dependencies

#### Required

1. **libcurl** - For HTTP requests to catalogs

   **macOS (Homebrew)**:
   ```bash
   brew install curl
   ```

   **Ubuntu/Debian**:
   ```bash
   sudo apt-get install libcurl4-openssl-dev
   ```

   **Fedora/RHEL**:
   ```bash
   sudo dnf install libcurl-devel
   ```

2. **nlohmann/json** - For JSON configuration

   **macOS (Homebrew)**:
   ```bash
   brew install nlohmann-json
   ```

   **Ubuntu/Debian** (20.04+):
   ```bash
   sudo apt-get install nlohmann-json3-dev
   ```

   **Alternative**: Will be downloaded automatically if not found

#### Optional

- **stb_image_write** - For PNG/JPEG export (header-only, auto-downloaded)
- **Doxygen** - For generating documentation

## Quick Build

### Linux/macOS

```bash
# 1. Clone repository
git clone https://github.com/manvalan/IOC_StarMap.git
cd IOC_StarMap

# 2. Download dependencies (if not installed system-wide)
chmod +x download_deps.sh
./download_deps.sh

# 3. Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 4. Install (optional)
sudo make install
```

### macOS (Apple Silicon)

```bash
# Install dependencies
brew install cmake curl nlohmann-json

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
make -j$(sysctl -n hw.ncpu)
sudo make install
```

## Build Options

### CMake Configuration Options

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \          # Release, Debug, RelWithDebInfo
  -DBUILD_SHARED_LIBS=ON \              # ON=shared, OFF=static
  -DBUILD_EXAMPLES=ON \                 # Build example programs
  -DBUILD_TESTS=OFF \                   # Build unit tests
  -DCMAKE_INSTALL_PREFIX=/usr/local     # Installation directory
```

### Build Types

**Release** (Recommended for production):
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Debug** (For development):
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
```

**RelWithDebInfo** (Optimized with debug symbols):
```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## Static vs Dynamic Library

### Build Static Library

```bash
cmake .. -DBUILD_SHARED_LIBS=OFF
make
# Produces: libstarmap.a
```

Usage:
```bash
g++ -std=c++17 myapp.cpp -L/path/to/lib -lstarmap -lcurl -o myapp
```

### Build Dynamic Library

```bash
cmake .. -DBUILD_SHARED_LIBS=ON
make
# Produces: libstarmap.so (Linux), libstarmap.dylib (macOS), starmap.dll (Windows)
```

Usage:
```bash
g++ -std=c++17 myapp.cpp -L/path/to/lib -lstarmap -o myapp
export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH  # Linux
export DYLD_LIBRARY_PATH=/path/to/lib:$DYLD_LIBRARY_PATH  # macOS
```

## Advanced Build

### Cross-Compilation

**For Raspberry Pi (on x86_64 Linux)**:
```bash
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/rpi-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
make
```

### Custom Install Location

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/starmap
make install
# Library installed to: $HOME/starmap/lib/libstarmap.so
# Headers installed to: $HOME/starmap/include/starmap/
```

### Verbose Build

```bash
make VERBOSE=1
# or
cmake --build . --verbose
```

## Testing the Build

### Run Examples

```bash
cd build

# Basic example
./examples/example_basic

# JSON config
./examples/example_json ../examples/config_examples/orion.json

# GAIA query
./examples/example_gaia
```

### Verify Installation

```bash
# Check library
ls -l /usr/local/lib/libstarmap*

# Check headers
ls -l /usr/local/include/starmap/

# Check pkg-config (if installed)
pkg-config --cflags --libs starmap
```

## Using in Your Project

### Method 1: CMake find_package

After installation, in your `CMakeLists.txt`:

```cmake
find_package(StarMap REQUIRED)
target_link_libraries(your_app PRIVATE StarMap::starmap)
```

### Method 2: Manual Linking

```bash
g++ -std=c++17 \
    -I/usr/local/include \
    myapp.cpp \
    -L/usr/local/lib \
    -lstarmap \
    -lcurl \
    -o myapp
```

### Method 3: CMake FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    starmap
    GIT_REPOSITORY https://github.com/manvalan/IOC_StarMap.git
    GIT_TAG main
)

FetchContent_MakeAvailable(starmap)

target_link_libraries(your_app PRIVATE starmap)
```

## Troubleshooting

### "Could not find CURL"

```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev

# macOS
brew install curl

# Verify
pkg-config --cflags --libs libcurl
```

### "nlohmann/json.hpp: No such file"

Option 1 - Install system package:
```bash
sudo apt-get install nlohmann-json3-dev  # Ubuntu 20.04+
brew install nlohmann-json                # macOS
```

Option 2 - Use bundled version:
```bash
./download_deps.sh
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Linker Errors with libcurl

If you see undefined references to curl functions:

```bash
# Check if libcurl is installed
ldconfig -p | grep curl

# Link explicitly
g++ myapp.cpp -lstarmap -lcurl -o myapp
```

### macOS: "library not found"

```bash
# Add library path
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH

# Or use rpath during linking
g++ myapp.cpp -lstarmap -Wl,-rpath,/usr/local/lib -o myapp
```

## Build from Source (Complete Example)

### Ubuntu 22.04

```bash
# System dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libcurl4-openssl-dev \
    nlohmann-json3-dev

# Clone
git clone https://github.com/manvalan/IOC_StarMap.git
cd IOC_StarMap

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
make -j$(nproc)

# Test
./examples/example_basic

# Install
sudo make install
```

### macOS Ventura+

```bash
# Dependencies
brew install cmake curl nlohmann-json

# Clone
git clone https://github.com/manvalan/IOC_StarMap.git
cd IOC_StarMap

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
make -j$(sysctl -n hw.ncpu)

# Test
./examples/example_basic

# Install
sudo make install
```

## Uninstall

```bash
cd build
sudo make uninstall
# or manually:
sudo rm -rf /usr/local/include/starmap
sudo rm /usr/local/lib/libstarmap*
sudo rm -rf /usr/local/lib/cmake/StarMap
```

## Performance Tips

1. **Release Build**: Always use `-DCMAKE_BUILD_TYPE=Release` for production
2. **Parallel Build**: Use `make -j$(nproc)` to utilize all CPU cores
3. **LTO**: Enable link-time optimization for better performance:
   ```bash
   cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
   ```

## Next Steps

After building:

1. Read [QUICKSTART.md](QUICKSTART.md) for usage examples
2. Explore `examples/` directory
3. Read API documentation (generate with Doxygen)
4. Check [README.md](README.md) for detailed features

---

For issues or questions, please visit: https://github.com/manvalan/IOC_StarMap/issues
