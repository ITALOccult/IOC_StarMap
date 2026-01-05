# Esempi Completi e Utilizzo da C++

Configurazioni JSON complete e snippet C++ per generare mappe.

## Finder Chart Standard

File: [examples/config_examples/finder_chart.json](../../examples/config_examples/)

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
    "width": 2400,
    "height": 2400
  },
  "projection": {
    "type": "stereographic",
    "coordinate_system": "equatorial"
  },
  "limiting_magnitude": 10.0,
  "background_color": 255,
  "stars": {
    "min_symbol_size": 0.8,
    "max_symbol_size": 6.0,
    "use_spectral_colors": false,
    "default_color": 4278190080,
    "show_names": true,
    "min_magnitude_for_label": 5.0,
    "label_font_size": 10.0
  },
  "grid": {
    "enabled": true,
    "ra_step": 1.0,
    "dec_step": 1.0
  },
  "title": {
    "enabled": true,
    "text": "Orion Region - Finder Chart",
    "position": 1,
    "font_size": 20.0
  },
  "border": {
    "enabled": true,
    "width": 2.0
  },
  "compass": {
    "enabled": true,
    "position": 2,
    "size": 80.0
  },
  "scale": {
    "enabled": true,
    "position": 3,
    "length": 100.0
  },
  "overlays": {
    "constellation_lines": true,
    "constellation_names": true
  }
}
```

---

## Occultazione Asteroide

File: [examples/config_examples/occultation_path.json](../../examples/config_examples/)

```json
{
  "center": {
    "ra": 85.2,
    "dec": 4.8
  },
  "field_of_view": {
    "width": 2.0,
    "height": 2.0
  },
  "image": {
    "width": 3000,
    "height": 3000
  },
  "projection": {
    "type": "gnomonic"
  },
  "limiting_magnitude": 14.0,
  "stars": {
    "min_symbol_size": 0.5,
    "max_symbol_size": 8.0,
    "use_spectral_colors": true,
    "show_names": true,
    "show_magnitudes": true,
    "min_magnitude_for_label": 8.0
  },
  "grid": {
    "enabled": true,
    "ra_step": 0.25,
    "dec_step": 0.25
  },
  "title": {
    "enabled": true,
    "text": "Occultazione Asteroide - 2026-01-05",
    "position": 1,
    "font_size": 24.0
  },
  "overlay_rectangles": [
    {
      "center_ra": 85.2,
      "center_dec": 4.8,
      "width_ra": 1.0,
      "height_dec": 1.0,
      "color": 4294967295,
      "line_width": 2.0,
      "filled": true,
      "fill_color": 855638323,
      "label": "Zona Occultazione"
    }
  ],
  "overlay_paths": [
    {
      "color": 4278255360,
      "line_width": 3.0,
      "show_points": true,
      "show_labels": true,
      "show_direction": true,
      "label": "Traccia Asteroide",
      "points": [
        {"ra": 85.0, "dec": 5.0, "label": "T-1h"},
        {"ra": 85.1, "dec": 4.9, "label": "T-30m"},
        {"ra": 85.2, "dec": 4.8, "label": "T0 (ingresso)"},
        {"ra": 85.3, "dec": 4.7, "label": "T+30m"},
        {"ra": 85.4, "dec": 4.6, "label": "T+1h"}
      ]
    }
  ]
}
```

---

## Wide Field Survey

File: [examples/config_examples/wide_field.json](../../examples/config_examples/)

```json
{
  "center": {
    "ra": 180.0,
    "dec": 0.0
  },
  "field_of_view": {
    "width": 60.0,
    "height": 60.0
  },
  "image": {
    "width": 2400,
    "height": 2400
  },
  "projection": {
    "type": "mercator"
  },
  "limiting_magnitude": 11.0,
  "stars": {
    "min_symbol_size": 0.3,
    "max_symbol_size": 3.0,
    "use_spectral_colors": true,
    "show_names": false,
    "show_sao_numbers": false
  },
  "grid": {
    "enabled": true,
    "ra_step": 15.0,
    "dec_step": 15.0,
    "show_labels": true
  },
  "background_color": 255,
  "overlays": {
    "constellation_lines": true,
    "constellation_names": true,
    "milky_way": true,
    "ecliptic": true
  }
}
```

---

## Utilizzo da C++

### Caricamento da File JSON

```cpp
#include <starmap/StarMap.h>

int main() {
    // Carica configurazione da file
    starmap::StarMapBuilder builder;
    auto image = builder.generateFromFile("config.json");
    
    // Salva come PNG
    image.saveAsPNG("output.png");
    
    return 0;
}
```

### Configurazione Programmatica

```cpp
#include <starmap/StarMap.h>
#include <starmap/config/Config.h>

int main() {
    // Crea configurazione a mano
    starmap::config::Config config;
    
    // Coordinate
    config.center = {83.75, 5.0};    // RA, Dec
    config.field_of_view = {10.0, 10.0};
    config.image = {2400, 2400};
    
    // Stelle
    config.limiting_magnitude = 11.0;
    config.stars.min_symbol_size = 0.5;
    config.stars.max_symbol_size = 8.0;
    
    // UI
    config.title.enabled = true;
    config.title.text = "My Star Map";
    config.title.position = starmap::Position::TOP_LEFT;
    
    // Proiezione
    config.projection.type = "stereographic";
    config.projection.coordinate_system = "equatorial";
    
    // Genera
    starmap::StarMapBuilder builder;
    auto image = builder.generate(config);
    image.saveAsPNG("output.png");
    
    return 0;
}
```

### Batch Processing

```cpp
#include <starmap/StarMap.h>
#include <starmap/config/JSONConfigLoader.h>
#include <filesystem>

int main() {
    starmap::config::JSONConfigLoader loader;
    starmap::StarMapBuilder builder;
    
    // Processa tutti i file JSON in una directory
    std::string config_dir = "examples/config_examples";
    for (const auto& entry : std::filesystem::directory_iterator(config_dir)) {
        if (entry.path().extension() == ".json") {
            try {
                auto config = loader.load(entry.path().string());
                auto image = builder.generate(config);
                
                // Estrai nome file per output
                std::string output = entry.path().stem().string() + ".png";
                image.saveAsPNG(output);
                
                std::cout << "Generata: " << output << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Errore " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
    
    return 0;
}
```

### Modifica Dinamica Configurazione

```cpp
#include <starmap/StarMap.h>
#include <starmap/config/JSONConfigLoader.h>

int main() {
    // Carica da file
    starmap::config::JSONConfigLoader loader;
    auto config = loader.load("base_config.json");
    
    // Modifica parametri
    config.limiting_magnitude = 13.0;  // Più stelle
    config.image.width = 3000;         // Più risoluzione
    config.stars.use_spectral_colors = true;
    
    // Genera con parametri modificati
    starmap::StarMapBuilder builder;
    auto image = builder.generate(config);
    image.saveAsPNG("modified_output.png");
    
    return 0;
}
```

---

## Template File

Sono disponibili template pronti all'uso in `examples/config_examples/`:

| File | Descrizione |
|------|-------------|
| `orion.json` | Configurazione minima |
| `orion_full.json` | Con tutti gli overlay |
| `finder_chart.json` | Carta finder standard |
| `occultation_*.json` | Occultazioni con path |
| `telescope_finder.json` | FOV telescopio |
| `wide_field.json` | Grande FOV survey |

---

## Errori Comuni

### `coordinate_system` Sconosciuto

```
Error: Unknown coordinate system 'equatoral'
```

Soluzione: Controllare spelling (è `equatorial`, non `equatoral`)

### Magnitudine Troppo Alta

```
Error: limiting_magnitude 25.0 > max (18)
```

Soluzione: Ridurre a valore ragionevole (8-14 consigliato)

### File Non Trovato

```
Error: Cannot open file 'my_config.json'
```

Soluzione: Controllare path relativo/assoluto e permessi file

### Nessuna Stella Renderizzata

```
// Mappa bianca/vuota
```

Possibili cause:
- Magnitudine limite troppo bassa
- Centro non valido
- FOV molto piccolo
- Elenco stelle non caricato

---

## Conversione da Formato Vecchio

Se usi configurazione deprecata con `display`:

```json
{
  "display": {
    "show_title": true,
    "title": "Old Format"
  }
}
```

Converti a nuovo formato:

```json
{
  "title": {
    "enabled": true,
    "text": "Old Format"
  }
}
```

---

[← Overlay Avanzati](07_overlays_advanced.md) | [Indice](../JSON_CONFIGURATION.md) | [Prossimo: Utilità →](09_utilities.md)
