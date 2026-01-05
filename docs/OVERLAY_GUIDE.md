# Overlay e Elementi Personalizzati - Guida Completa

## Panoramica

La libreria supporta ora diversi elementi overlay configurabili tramite JSON:
1. **Scala Magnitudini** - Legenda con dimensioni stelle per magnitudine
2. **Rettangoli Overlay** - Evidenziare regioni del cielo (AR/DEC)
3. **Path/Tracce** - Visualizzare percorsi (asteroidi, comete, satelliti)

Tutti gli elementi sono completamente configurabili nei preset JSON.

---

## 1. Scala Magnitudini (Magnitude Legend)

### Configurazione

```json
{
  "magnitude_legend": {
    "enabled": true,
    "position": 4,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "font_size": 10.0,
    "text_color": 4294967295,
    "background_color": 3422552319,
    "show_background": true
  }
}
```

### Parametri

| Parametro | Tipo | Default | Descrizione |
|-----------|------|---------|-------------|
| `enabled` | bool | `false` | Abilita/disabilita la legenda |
| `position` | int | `4` | Posizione: 0=NONE, 1=TOP_LEFT, 2=TOP_RIGHT, 3=BOTTOM_LEFT, 4=BOTTOM_RIGHT, 5=CUSTOM |
| `custom_x` | float | `0.0` | Posizione X custom (0-1, normalizzato) |
| `custom_y` | float | `0.0` | Posizione Y custom (0-1, normalizzato) |
| `font_size` | float | `10.0` | Dimensione font |
| `text_color` | uint32 | `0xFFFFFFFF` | Colore testo (RGBA) |
| `background_color` | uint32 | `0x000000CC` | Colore sfondo (RGBA semi-trasparente) |
| `show_background` | bool | `true` | Mostra sfondo dietro la legenda |

### Posizioni Predefinite

```cpp
enum LegendPosition {
    NONE = 0,           // Non mostrare
    TOP_LEFT = 1,       // In alto a sinistra
    TOP_RIGHT = 2,      // In alto a destra
    BOTTOM_LEFT = 3,    // In basso a sinistra
    BOTTOM_RIGHT = 4,   // In basso a destra (default)
    CUSTOM = 5          // Usa custom_x, custom_y
};
```

### Esempio: Posizione Custom

```json
{
  "magnitude_legend": {
    "enabled": true,
    "position": 5,
    "custom_x": 0.85,
    "custom_y": 0.15,
    "font_size": 12.0
  }
}
```

Posiziona la legenda all'85% della larghezza e 15% dell'altezza dell'immagine.

---

## 2. Rettangoli Overlay

### Configurazione

```json
{
  "overlay_rectangles": [
    {
      "enabled": true,
      "center_ra": 83.75,
      "center_dec": 5.0,
      "width_ra": 5.0,
      "height_dec": 5.0,
      "color": 4278190335,
      "line_width": 2.0,
      "filled": false,
      "fill_color": 855638323,
      "label": "Orion Core"
    }
  ]
}
```

### Parametri

| Parametro | Tipo | Default | Descrizione |
|-----------|------|---------|-------------|
| `enabled` | bool | `false` | Abilita questo rettangolo |
| `center_ra` | double | `0.0` | Centro in Ascensione Retta (gradi, 0-360) |
| `center_dec` | double | `0.0` | Centro in Declinazione (gradi, -90 a +90) |
| `width_ra` | double | `0.0` | Larghezza in AR (gradi) |
| `height_dec` | double | `0.0` | Altezza in Dec (gradi) |
| `color` | uint32 | `0xFF0000FF` | Colore bordo (RGBA, rosso default) |
| `line_width` | float | `2.0` | Spessore linea (pixel) |
| `filled` | bool | `false` | Riempie il rettangolo |
| `fill_color` | uint32 | `0xFF000033` | Colore riempimento (RGBA semi-trasparente) |
| `label` | string | `""` | Etichetta opzionale |

### Conversione Colori RGBA

Formato: `0xRRGGBBAA` (esadecimale) → decimale

Esempi:
- Rosso: `0xFF0000FF` = `4278190335`
- Verde: `0x00FF00FF` = `16711935`
- Blu: `0x0000FFFF` = `65535`
- Giallo: `0xFFFF00FF` = `4294902015`
- Bianco: `0xFFFFFFFF` = `4294967295`
- Rosso trasparente 50%: `0xFF000080` = `4278190208`

### Esempio: Evidenziare Regione M42

```json
{
  "overlay_rectangles": [
    {
      "enabled": true,
      "center_ra": 83.82,
      "center_dec": -5.39,
      "width_ra": 2.0,
      "height_dec": 2.0,
      "color": 4278255615,
      "line_width": 3.0,
      "filled": true,
      "fill_color": 1157562624,
      "label": "M42 - Orion Nebula"
    }
  ]
}
```

### Esempio: Multipli Rettangoli

```json
{
  "overlay_rectangles": [
    {
      "enabled": true,
      "center_ra": 83.75,
      "center_dec": 5.0,
      "width_ra": 10.0,
      "height_dec": 10.0,
      "color": 4278190335,
      "line_width": 2.0,
      "label": "Wide Field"
    },
    {
      "enabled": true,
      "center_ra": 84.05,
      "center_dec": -1.94,
      "width_ra": 1.0,
      "height_dec": 1.0,
      "color": 4294902015,
      "line_width": 1.5,
      "label": "Alnilam Detail"
    }
  ]
}
```

---

## 3. Path/Tracce Overlay

### Configurazione

```json
{
  "overlay_paths": [
    {
      "enabled": true,
      "color": 4278255360,
      "line_width": 2.5,
      "show_points": true,
      "point_size": 4.0,
      "show_labels": false,
      "show_direction": false,
      "label": "Asteroid Trajectory",
      "points": [
        {"ra": 85.0, "dec": 5.0, "label": "T0"},
        {"ra": 86.0, "dec": 4.5, "label": "T1"},
        {"ra": 87.0, "dec": 4.0, "label": "T2"}
      ]
    }
  ]
}
```

### Parametri Path

| Parametro | Tipo | Default | Descrizione |
|-----------|------|---------|-------------|
| `enabled` | bool | `false` | Abilita questo path |
| `color` | uint32 | `0x00FF00FF` | Colore linea (RGBA, verde default) |
| `line_width` | float | `2.0` | Spessore linea (pixel) |
| `show_points` | bool | `true` | Mostra i punti lungo il percorso |
| `point_size` | float | `3.0` | Dimensione punti (pixel) |
| `show_labels` | bool | `false` | Mostra etichette sui punti |
| `show_direction` | bool | `false` | Mostra frecce di direzione (TODO) |
| `label` | string | `""` | Etichetta generale del path |

### Parametri Punto

| Parametro | Tipo | Default | Descrizione |
|-----------|------|---------|-------------|
| `ra` | double | `0.0` | Ascensione Retta (gradi) |
| `dec` | double | `0.0` | Declinazione (gradi) |
| `label` | string | `""` | Etichetta per questo punto |

### Esempio: Traccia Asteroide

```json
{
  "overlay_paths": [
    {
      "enabled": true,
      "color": 4294967040,
      "line_width": 2.0,
      "show_points": true,
      "point_size": 5.0,
      "show_labels": true,
      "label": "(433) Eros - 2026-01-05 to 2026-01-12",
      "points": [
        {"ra": 78.5, "dec": 10.2, "label": "05 Jan"},
        {"ra": 79.1, "dec": 9.8, "label": "06 Jan"},
        {"ra": 79.7, "dec": 9.4, "label": "07 Jan"},
        {"ra": 80.3, "dec": 9.0, "label": "08 Jan"},
        {"ra": 80.9, "dec": 8.6, "label": "09 Jan"},
        {"ra": 81.5, "dec": 8.2, "label": "10 Jan"},
        {"ra": 82.1, "dec": 7.8, "label": "11 Jan"},
        {"ra": 82.7, "dec": 7.4, "label": "12 Jan"}
      ]
    }
  ]
}
```

### Esempio: Multipli Path (Cometa + Asteroide)

```json
{
  "overlay_paths": [
    {
      "enabled": true,
      "color": 4278255360,
      "line_width": 3.0,
      "show_points": true,
      "point_size": 6.0,
      "label": "Comet Path",
      "points": [
        {"ra": 75.0, "dec": 15.0, "label": "Perihelion"},
        {"ra": 80.0, "dec": 12.0, "label": "+1 week"},
        {"ra": 85.0, "dec": 9.0, "label": "+2 weeks"}
      ]
    },
    {
      "enabled": true,
      "color": 4294902015,
      "line_width": 2.0,
      "show_points": true,
      "point_size": 4.0,
      "label": "Asteroid Path",
      "points": [
        {"ra": 90.0, "dec": 5.0, "label": "Start"},
        {"ra": 88.0, "dec": 4.5, "label": "Mid"},
        {"ra": 86.0, "dec": 4.0, "label": "End"}
      ]
    }
  ]
}
```

---

## Esempio Completo

File: `examples/config_examples/orion_with_overlays.json`

```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 15.0, "height": 15.0},
  "image": {"width": 2400, "height": 2400},
  "limiting_magnitude": 11.0,
  
  "display": {
    "show_title": true,
    "title": "Orion Region with Overlays"
  },
  
  "magnitude_legend": {
    "enabled": true,
    "position": 4
  },
  
  "overlay_rectangles": [
    {
      "enabled": true,
      "center_ra": 83.75,
      "center_dec": 5.0,
      "width_ra": 5.0,
      "height_dec": 5.0,
      "color": 4278190335,
      "line_width": 2.0,
      "label": "Orion Core"
    }
  ],
  
  "overlay_paths": [
    {
      "enabled": true,
      "color": 4278255360,
      "line_width": 2.5,
      "show_points": true,
      "point_size": 4.0,
      "label": "Orion Belt",
      "points": [
        {"ra": 85.19, "dec": -1.20, "label": "Alnitak"},
        {"ra": 84.05, "dec": -1.94, "label": "Alnilam"},
        {"ra": 83.00, "dec": -2.60, "label": "Mintaka"}
      ]
    }
  ]
}
```

---

## Uso Programmatico (C++)

### Scala Magnitudini

```cpp
#include <starmap/StarMap.h>

starmap::map::MapConfiguration config;
config.magnitudeLegend.enabled = true;
config.magnitudeLegend.position = starmap::map::LegendPosition::BOTTOM_RIGHT;
config.magnitudeLegend.fontSize = 12.0f;
```

### Rettangolo

```cpp
starmap::map::OverlayRectangle rect;
rect.enabled = true;
rect.centerRA = 83.75;
rect.centerDec = 5.0;
rect.widthRA = 5.0;
rect.heightDec = 5.0;
rect.color = 0xFF0000FF; // Rosso
rect.lineWidth = 2.0f;
rect.label = "Target Region";

config.overlayRectangles.push_back(rect);
```

### Path

```cpp
starmap::map::OverlayPath path;
path.enabled = true;
path.color = 0x00FF00FF; // Verde
path.lineWidth = 2.5f;
path.showPoints = true;
path.label = "Asteroid Track";

starmap::map::PathPoint p1{78.5, 10.2, "T0"};
starmap::map::PathPoint p2{79.1, 9.8, "T1"};
starmap::map::PathPoint p3{79.7, 9.4, "T2"};

path.points = {p1, p2, p3};
config.overlayPaths.push_back(path);
```

---

## Tips & Tricks

### Colori Consigliati

```
Rettangoli:
- Target principale: Rosso (0xFF0000FF)
- Regione secondaria: Giallo (0xFFFF00FF)
- Campo riferimento: Blu (0x0000FFFF)

Path:
- Asteroide: Verde (0x00FF00FF)
- Cometa: Ciano (0x00FFFFFF)
- Satellite: Magenta (0xFF00FFFF)
```

### Performance

- **Rettangoli**: Impatto minimo, anche con 10+ rettangoli
- **Path**: ~1ms per 100 punti
- **Scala magnitudini**: Trascurabile

### Limiti

- Max rettangoli raccomandati: **20**
- Max punti per path: **1000**
- Max path simultanei: **10**

---

## Conversione Colori

Tool online: https://www.color-hex.com/

Da RGB a RGBA uint32:
```python
def rgb_to_uint32(r, g, b, a=255):
    return (r << 24) | (g << 16) | (b << 8) | a

# Esempi
rosso = rgb_to_uint32(255, 0, 0)      # 4278190335
verde = rgb_to_uint32(0, 255, 0)      # 16711935
blu = rgb_to_uint32(0, 0, 255)        # 65535
rosso_50 = rgb_to_uint32(255, 0, 0, 128)  # 4278190208
```

---

## TODO Future

- [ ] Rendering testo con FreeType per etichette
- [ ] Frecce direzionali per path
- [ ] Riempimento rettangoli con pattern
- [ ] Ellissi e cerchi
- [ ] Poligoni arbitrari
- [ ] Animazione path (GIF/video)
