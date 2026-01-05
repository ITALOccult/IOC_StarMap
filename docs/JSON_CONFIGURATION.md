# Configurazione JSON - Guida di Riferimento

Tutte le mappe generate dalla libreria StarMap possono essere **completamente configurate via JSON**, senza necessità di codice C++.

## 📚 Documentazione Completa

Per evitare documenti troppo lunghi, la guida è stata divisa in sezioni tematiche:

### Guide Base

1. **[Configurazione Base](json_config/01_base.md)** - Centro, FOV, immagine, proiezione, magnitudine
2. **[Performance](json_config/02_performance.md)** - Ottimizzazione memoria, batch rendering, orientamento
3. **[Griglia e Coordinate](json_config/03_grid.md)** - Griglia RA/Dec, sistemi di coordinate, etichette

### Rendering

4. **[Stelle e Background](json_config/04_stars.md)** - Dimensionamento, colori spettrali, etichette, sfondo
5. **[Elementi UI](json_config/05_ui.md)** - Titolo, bordo, bussola, scala angolare

### Overlay

6. **[Legenda e Overlay Base](json_config/06_overlays_base.md)** - Scala magnitudini, costellazioni, eclittica
7. **[Overlay Avanzati](json_config/07_overlays_advanced.md)** - Rettangoli, path, marker personalizzati

### Utilità

8. **[Esempi e Utilizzo](json_config/08_examples.md)** - Esempi completi, uso da C++, file template
9. **[Utilità e Reference](json_config/09_utilities.md)** - Conversione colori, validazione, troubleshooting

---

## 🚀 Quick Start

**Esempio minimale**:
```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 10.0, "height": 10.0},
  "image": {"width": 2400, "height": 2400},
  "limiting_magnitude": 11.0
}
```

**Uso da C++**:
```cpp
#include <starmap/StarMap.h>

starmap::StarMapBuilder builder;
auto image = builder.generateFromFile("config.json");
image.saveAsPNG("output.png");
```

---

## 📖 Manuale Avanzato con LaTeX

Per formule matematiche dettagliate e spiegazioni approfondite, consulta il **[Manuale Completo](json_manual/00_INDEX.md)** con:
- Formule LaTeX per proiezioni cartografiche
- Calcoli performance e ottimizzazione
- Algoritmi rendering e interpolazione
- Teoria colori spettrali
- Esempi avanzati per occultazioni

---

## Indice Veloce Parametri

### Configurazione Base
- [Centro e Campo Visivo](json_config/01_base.md#centro-e-campo-visivo)
- [Dimensioni Immagine](json_config/01_base.md#dimensioni-immagine)
- [Proiezione](json_config/01_base.md#proiezione)
- [Magnitudine Limite](json_config/01_base.md#magnitudine-limite)

### Performance e Orientamento
- [Performance](json_config/02_performance.md#performance)
- [Orientamento](json_config/02_performance.md#orientamento)

### Rendering
- [Griglia](json_config/03_grid.md#griglia)
- [Stelle](json_config/04_stars.md#stelle)
- [Background](json_config/04_stars.md#background)

### Elementi UI
- [Titolo](json_config/05_ui.md#titolo)
- [Bordo](json_config/05_ui.md#bordo)
- [Bussola](json_config/05_ui.md#bussola)
- [Scala](json_config/05_ui.md#scala)

### Overlay
- [Scala Magnitudini](json_config/06_overlays_base.md#scala-magnitudini)
- [Overlay Base](json_config/06_overlays_base.md#overlay)
- [Rettangoli Overlay](json_config/07_overlays_advanced.md#rettangoli-overlay)
- [Path Overlay](json_config/07_overlays_advanced.md#path-overlay)

### Altro
- [Osservatore](json_config/01_base.md#osservatore)

---

## Centro e Campo Visivo

```json
{
  "center": {
    "ra": 83.75,
    "dec": 5.0
  },
  "field_of_view": {
    "width": 10.0,
    "height": 10.0
  }
}
```

| Campo | Tipo | Range | Descrizione |
|-------|------|-------|-------------|
| `center.ra` | double | 0-360 | Ascensione Retta del centro (gradi) |
| `center.dec` | double | -90 a +90 | Declinazione del centro (gradi) |
| `field_of_view.width` | double | >0 | Larghezza campo in gradi |
| `field_of_view.height` | double | >0 | Altezza campo in gradi |

---

## Dimensioni Immagine

```json
{
  "image": {
    "width": 1920,
    "height": 1080
  }
}
```

| Campo | Tipo | Range | Descrizione |
|-------|------|-------|-------------|
| `image.width` | int | >0 | Larghezza immagine in pixel |
| `image.height` | int | >0 | Altezza immagine in pixel |

**Raccomandazioni**:
- Print: 3000×3000 o superiore
- Web: 1920×1080 o 2400×2400
- Anteprima: 800×600

---

## Proiezione

```json
{
  "projection": {
    "type": "stereographic",
    "coordinate_system": "equatorial"
  }
}
```

### Tipi di Proiezione

| Valore | Descrizione | Uso Tipico |
|--------|-------------|------------|
| `stereographic` | Proiezione stereografica | Standard per mappe celesti |
| `gnomonic` | Proiezione gnomica | Cerchi massimi = linee rette |
| `orthographic` | Proiezione ortografica | Vista dall'infinito |
| `mercator` | Proiezione di Mercatore | Mappe globali |
| `azimuthal_equidistant` | Azimutale equidistante | Osservazione locale |

### Sistemi di Coordinate

| Valore | Descrizione |
|--------|-------------|
| `equatorial` | RA/Dec (J2000) |
| `galactic` | l/b (coordinate galattiche) |
| `ecliptic` | Coordinate eclittiche |
| `horizontal` | Alt/Az (richiede observer location) |

---

## Magnitudine Limite

```json
{
  "limiting_magnitude": 12.0
}
```

| Campo | Tipo | Range | Descrizione |
|-------|------|-------|-------------|
| `limiting_magnitude` | double | -2 a 18 | Magnitudine più debole visibile |

**Raccomandazioni**:
- Carta finder: 8-10
- Carta standard: 11-12
- Carta dettagliata: 13-14
- Attenzione: >13 genera molte stelle (vedi Performance)

---

## Performance

```json
{
  "performance": {
    "max_stars": 50000,
    "star_batch_size": 5000
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `max_stars` | int | 50000 | Numero massimo di stelle da caricare |
| `star_batch_size` | int | 5000 | Dimensione batch per rendering |

Vedi [MEMORY_OPTIMIZATION.md](MEMORY_OPTIMIZATION.md) per dettagli.

---

## Orientamento

```json
{
  "orientation": {
    "rotation_angle": 0.0,
    "north_up": true,
    "east_left": true
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `rotation_angle` | double | 0.0 | Rotazione in gradi (senso orario) |
| `north_up` | bool | true | Nord verso l'alto |
| `east_left` | bool | true | Est a sinistra (standard astronomico) |

---

## Griglia

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 1.0,
    "dec_step": 1.0,
    "color": 1077952767,
    "line_width": 0.5,
    "show_labels": true,
    "label_color": 3435973887,
    "label_font_size": 10.0
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `enabled` | bool | true | Mostra griglia coordinate |
| `ra_step` | double | 15.0 | Step RA in gradi (1h = 15°) |
| `dec_step` | double | 10.0 | Step Dec in gradi |
| `color` | uint32 | 0x404040FF | Colore linee RGBA |
| `line_width` | float | 0.5 | Spessore linee |
| `show_labels` | bool | true | Mostra etichette coordinate |
| `label_color` | uint32 | 0xCCCCCCFF | Colore etichette |
| `label_font_size` | float | 10.0 | Dimensione font etichette |

---

## Stelle

```json
{
  "stars": {
    "min_symbol_size": 0.5,
    "max_symbol_size": 8.0,
    "magnitude_range": 10.0,
    "use_spectral_colors": true,
    "default_color": 4294967295,
    "show_names": true,
    "show_sao_numbers": true,
    "show_magnitudes": false,
    "min_magnitude_for_label": 4.0,
    "label_color": 4294967295,
    "label_font_size": 8.0
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `min_symbol_size` | float | 0.5 | Dimensione stelle deboli |
| `max_symbol_size` | float | 8.0 | Dimensione stelle luminose |
| `magnitude_range` | float | 10.0 | Range magnitudine per scaling |
| `use_spectral_colors` | bool | true | Colori basati su B-V |
| `default_color` | uint32 | 0xFFFFFFFF | Colore default (bianco) |
| `show_names` | bool | true | Mostra nomi stelle |
| `show_sao_numbers` | bool | true | Mostra numeri SAO |
| `show_magnitudes` | bool | false | Mostra magnitudini |
| `min_magnitude_for_label` | float | 4.0 | Mag limite per etichette |
| `label_color` | uint32 | 0xFFFFFFFF | Colore etichette |
| `label_font_size` | float | 8.0 | Dimensione font |

---

## Background

```json
{
  "background_color": 255
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `background_color` | uint32 | 0x000000FF | Colore sfondo RGBA |

Colori comuni:
- Nero: `255` (0x000000FF)
- Bianco: `4294967295` (0xFFFFFFFF)
- Blu scuro: `16777471` (0x0000FFFF)

---

## Titolo

```json
{
  "title": {
    "enabled": true,
    "text": "My Star Map",
    "position": 1,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "font_size": 18.0,
    "text_color": 4294967295,
    "background_color": 2147483903,
    "show_background": true,
    "padding_x": 10.0,
    "padding_y": 10.0
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `enabled` | bool | true | Mostra titolo |
| `text` | string | "" | Testo del titolo |
| `position` | int | 1 | Posizione (vedi sotto) |
| `custom_x` | float | 0.0 | X custom (0-1) se position=5 |
| `custom_y` | float | 0.0 | Y custom (0-1) se position=5 |
| `font_size` | float | 18.0 | Dimensione font |
| `text_color` | uint32 | 0xFFFFFFFF | Colore testo |
| `background_color` | uint32 | 0x00000000 | Colore sfondo |
| `show_background` | bool | false | Mostra sfondo dietro testo |
| `padding_x` | float | 10.0 | Padding orizzontale |
| `padding_y` | float | 10.0 | Padding verticale |

**Posizioni**: 0=NONE, 1=TOP_LEFT, 2=TOP_RIGHT, 3=BOTTOM_LEFT, 4=BOTTOM_RIGHT, 5=CUSTOM

### Backward Compatibility

```json
{
  "display": {
    "show_title": true,
    "title": "My Map"
  }
}
```

Ancora supportato ma deprecato. Usa il nuovo formato `title`.

---

## Bordo

```json
{
  "border": {
    "enabled": true,
    "color": 4294967295,
    "width": 2.0,
    "margin": 0.0
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `enabled` | bool | true | Mostra bordo |
| `color` | uint32 | 0xFFFFFFFF | Colore bordo (bianco) |
| `width` | float | 2.0 | Spessore in pixel |
| `margin` | float | 0.0 | Margine interno in pixel |

---

## Bussola

```json
{
  "compass": {
    "enabled": true,
    "position": 2,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "size": 80.0,
    "north_color": 4278190335,
    "east_color": 16711935,
    "line_color": 4294967295,
    "line_width": 2.0,
    "show_labels": true,
    "label_font_size": 10.0
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `enabled` | bool | true | Mostra bussola |
| `position` | int | 2 | Posizione (0-5) |
| `custom_x` | float | 0.0 | X custom (0-1) |
| `custom_y` | float | 0.0 | Y custom (0-1) |
| `size` | float | 80.0 | Dimensione in pixel |
| `north_color` | uint32 | 0xFF0000FF | Colore Nord (rosso) |
| `east_color` | uint32 | 0x00FF00FF | Colore Est (verde) |
| `line_color` | uint32 | 0xFFFFFFFF | Colore linee (bianco) |
| `line_width` | float | 2.0 | Spessore linee |
| `show_labels` | bool | true | Mostra N/E/S/W |
| `label_font_size` | float | 10.0 | Dimensione font |

---

## Scala

```json
{
  "scale": {
    "enabled": true,
    "position": 3,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "length": 100.0,
    "color": 4294967295,
    "line_width": 2.0,
    "font_size": 10.0,
    "show_text": true
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `enabled` | bool | true | Mostra scala |
| `position` | int | 3 | Posizione (0-5) |
| `custom_x` | float | 0.0 | X custom (0-1) |
| `custom_y` | float | 0.0 | Y custom (0-1) |
| `length` | float | 100.0 | Lunghezza in pixel |
| `color` | uint32 | 0xFFFFFFFF | Colore (bianco) |
| `line_width` | float | 2.0 | Spessore linea |
| `font_size` | float | 10.0 | Dimensione font |
| `show_text` | bool | true | Mostra valore in gradi |

---

## Scala Magnitudini

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

Vedi [OVERLAY_GUIDE.md](OVERLAY_GUIDE.md#1-scala-magnitudini-magnitude-legend) per dettagli completi.

---

## Overlay

```json
{
  "overlays": {
    "constellation_lines": true,
    "constellation_boundaries": false,
    "constellation_names": true,
    "milky_way": false,
    "ecliptic": false,
    "equator": false
  }
}
```

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `constellation_lines` | bool | false | Linee costellazioni |
| `constellation_boundaries` | bool | false | Confini costellazioni |
| `constellation_names` | bool | false | Nomi costellazioni |
| `milky_way` | bool | false | Via Lattea |
| `ecliptic` | bool | false | Eclittica |
| `equator` | bool | false | Equatore celeste |

---

## Rettangoli Overlay

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
      "label": "Target"
    }
  ]
}
```

Vedi [OVERLAY_GUIDE.md](OVERLAY_GUIDE.md#2-rettangoli-overlay) per dettagli completi.

---

## Path Overlay

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
      "label": "Asteroid Track",
      "points": [
        {"ra": 85.0, "dec": 5.0, "label": "T0"},
        {"ra": 86.0, "dec": 4.5, "label": "T1"},
        {"ra": 87.0, "dec": 4.0, "label": "T2"}
      ]
    }
  ]
}
```

Vedi [OVERLAY_GUIDE.md](OVERLAY_GUIDE.md#3-pathtracce-overlay) per dettagli completi.

---

## Osservatore

```json
{
  "observation": {
    "time": "2026-01-05T20:00:00Z",
    "latitude": 45.0,
    "longitude": 10.0
  }
}
```

| Campo | Tipo | Descrizione |
|-------|------|-------------|
| `time` | string | Timestamp ISO 8601 (opzionale) |
| `latitude` | double | Latitudine osservatore (gradi) |
| `longitude` | double | Longitudine osservatore (gradi) |

Richiesto solo per coordinate `horizontal` (Alt/Az).

---

## Esempio Completo

File: [`examples/config_examples/fully_configured.json`](../examples/config_examples/fully_configured.json)

```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 12.0, "height": 12.0},
  "image": {"width": 2400, "height": 2400},
  "projection": {
    "type": "stereographic",
    "coordinate_system": "equatorial"
  },
  "limiting_magnitude": 11.5,
  "performance": {
    "max_stars": 50000,
    "star_batch_size": 5000
  },
  "title": {
    "enabled": true,
    "text": "Orion Region",
    "position": 1,
    "font_size": 20.0,
    "show_background": true
  },
  "border": {"enabled": true, "width": 3.0},
  "compass": {"enabled": true, "size": 90.0},
  "scale": {"enabled": true, "length": 120.0},
  "magnitude_legend": {"enabled": true},
  "overlays": {
    "constellation_lines": true,
    "constellation_names": true
  }
}
```

---

## Conversione Colori

Da RGB a uint32 RGBA:
```python
def rgb_to_uint32(r, g, b, a=255):
    return (r << 24) | (g << 16) | (b << 8) | a

rosso = rgb_to_uint32(255, 0, 0)     # 4278190335
verde = rgb_to_uint32(0, 255, 0)     # 16711935
blu = rgb_to_uint32(0, 0, 255)       # 65535
bianco = rgb_to_uint32(255, 255, 255) # 4294967295
nero = rgb_to_uint32(0, 0, 0)        # 255
```

Tool online: https://www.color-hex.com/

---

## Validazione

La libreria valida automaticamente i parametri:
- Coordinate AR/Dec nei range corretti
- Dimensioni immagine > 0
- Magnitudine ragionevole (-2 a 18)

Gli errori vengono segnalati durante il caricamento del JSON.

---

## Uso da Codice

```cpp
#include <starmap/StarMap.h>

int main() {
    starmap::config::JSONConfigLoader loader;
    auto config = loader.load("my_config.json");
    
    // Genera mappa
    starmap::StarMapBuilder builder;
    auto image = builder.generate(config);
    image.saveAsPNG("output.png");
    
    return 0;
}
```

O più semplicemente:

```cpp
starmap::StarMapBuilder builder;
auto image = builder.generateFromFile("my_config.json");
image.saveAsPNG("output.png");
```

---

## File di Esempio

- [`orion.json`](../examples/config_examples/orion.json) - Configurazione base
- [`orion_with_overlays.json`](../examples/config_examples/orion_with_overlays.json) - Con overlay
- [`occultation_with_path.json`](../examples/config_examples/occultation_with_path.json) - Con path asteroide
- [`fully_configured.json`](../examples/config_examples/fully_configured.json) - **Tutti i parametri**

---

## Note

- Tutti i colori sono in formato RGBA uint32
- Le posizioni custom sono normalizzate (0.0 = sinistra/alto, 1.0 = destra/basso)
- I parametri `display.show_*` sono deprecati ma ancora supportati
- Vedi documentazione specifica per overlay avanzati

