#!/bin/bash

# Script per scaricare dipendenze esterne

set -e

echo "StarMap - Download External Dependencies"
echo "=========================================="

# Crea directory external se non esiste
mkdir -p external
cd external

# nlohmann/json
if [ ! -d "json" ]; then
    echo ""
    echo "Downloading nlohmann/json..."
    git clone --depth 1 --branch v3.11.2 https://github.com/nlohmann/json.git
    echo "✓ nlohmann/json downloaded"
else
    echo "✓ nlohmann/json already present"
fi

# stb (opzionale, per salvare immagini)
if [ ! -d "stb" ]; then
    echo ""
    echo "Downloading stb headers..."
    mkdir -p stb
    cd stb
    wget -q https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
    cd ..
    echo "✓ stb headers downloaded"
else
    echo "✓ stb headers already present"
fi

cd ..

echo ""
echo "=========================================="
echo "✓ All dependencies downloaded successfully"
echo ""
echo "You can now build the library:"
echo "  mkdir build && cd build"
echo "  cmake .."
echo "  make -j\$(nproc)"
echo ""
