# Stelle e Background

Configurazione del rendering delle stelle e dello sfondo della mappa.

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

### Parametri Principali

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `min_symbol_size` | float | 0.5 | Dimensione stelle deboli (pixel) |
| `max_symbol_size` | float | 8.0 | Dimensione stelle luminose (pixel) |
| `magnitude_range` | float | 10.0 | Range magnitudine per scaling |
| `use_spectral_colors` | bool | true | Colori basati su B-V |
| `default_color` | uint32 | 0xFFFFFFFF | Colore default se no B-V |

### Dimensionamento

**Scaling logaritmico** (default):
```
size(m) = size_max - (size_max - size_min) × (m - m_min) / (m_max - m_min)
```

Esempio: `min_symbol_size=0.5`, `max_symbol_size=8.0`, `magnitude_range=10.0`

- Magnitudine 0: 8.0 px
- Magnitudine 5: 4.25 px
- Magnitudine 10: 0.5 px

### Colori Spettrali

Con `use_spectral_colors: true`, le stelle colori si basano sull'indice colore B-V:

| B-V | Colore | Tipo Spettrale | RGB |
|-----|--------|----------------|-----|
| -0.3 | Blu | O5 | (155, 176, 255) |
| 0.0 | Bianco-azzurro | A0 (Vega) | (248, 247, 255) |
| +0.3 | Bianco | F0 | (255, 244, 234) |
| +0.6 | Giallo | G2 (Sole) | (255, 237, 227) |
| +1.0 | Arancio | K5 | (255, 209, 178) |
| +1.5 | Rosso | M3 | (255, 180, 107) |
| +2.0 | Rosso scuro | M8 | (255, 149, 71) |

Con `use_spectral_colors: false`, tutte le stelle usano `default_color`.

---

## Etichette Stelle

```json
{
  "stars": {
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
| `show_names` | bool | true | Mostra nomi stelle |
| `show_sao_numbers` | bool | true | Mostra numeri SAO |
| `show_magnitudes` | bool | false | Mostra magnitudini |
| `min_magnitude_for_label` | float | 4.0 | Mag limite per etichette |
| `label_color` | uint32 | 0xFFFFFFFF | Colore etichette |
| `label_font_size` | float | 8.0 | Dimensione font |

### Esempi Etichette

**Solo nomi stelle luminose**:
```json
{
  "stars": {
    "show_names": true,
    "show_sao_numbers": false,
    "show_magnitudes": false,
    "min_magnitude_for_label": 3.0
  }
}
```

**Tutti con magnitudini**:
```json
{
  "stars": {
    "show_names": true,
    "show_sao_numbers": false,
    "show_magnitudes": true,
    "min_magnitude_for_label": 8.0
  }
}
```

**Nessuna etichetta**:
```json
{
  "stars": {
    "show_names": false,
    "show_sao_numbers": false,
    "show_magnitudes": false
  }
}
```

---

## Stili Stelle

### Finder Chart

```json
{
  "stars": {
    "min_symbol_size": 1.0,
    "max_symbol_size": 6.0,
    "magnitude_range": 8.0,
    "use_spectral_colors": false,
    "default_color": 4278190080,
    "show_names": true,
    "min_magnitude_for_label": 5.0,
    "label_font_size": 10.0
  }
}
```

**Risultato**: Stelle blu, no colori spettrali, etichette grandi

### Carta Dettaglio Colori

```json
{
  "stars": {
    "min_symbol_size": 0.3,
    "max_symbol_size": 10.0,
    "magnitude_range": 12.0,
    "use_spectral_colors": true,
    "show_names": true,
    "show_magnitudes": true,
    "min_magnitude_for_label": 8.0,
    "label_font_size": 7.0
  }
}
```

**Risultato**: Stelle colorate, con magnitudini, dettaglio massimo

### Mappa Scientifica

```json
{
  "stars": {
    "min_symbol_size": 0.5,
    "max_symbol_size": 8.0,
    "magnitude_range": 10.0,
    "use_spectral_colors": true,
    "show_names": false,
    "show_sao_numbers": true,
    "show_magnitudes": false,
    "min_magnitude_for_label": 3.0,
    "label_font_size": 8.0
  }
}
```

**Risultato**: Colori spettrali, nomi SAO, no magnitudini visualizzate

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

### Colori Standard

| Nome | Hex | uint32 | Descrizione |
|------|-----|--------|-------------|
| Nero | 0x000000FF | 255 | Cielo notturno |
| Bianco | 0xFFFFFFFF | 4294967295 | Carta printing |
| Blu scuro | 0x000080FF | 32895 | Elegante |
| Blu cielo | 0x0099CCFF | 10066175 | Cielo diurno |
| Grigio | 0x808080FF | 2155905279 | Neutro |
| Marrone | 0x663300FF | 1685582591 | Vintage |

### Conversione RGB → uint32

```python
def rgb_to_uint32(r, g, b, a=255):
    return (r << 24) | (g << 16) | (b << 8) | a

# Esempi
nero = rgb_to_uint32(0, 0, 0)              # 255
bianco = rgb_to_uint32(255, 255, 255)      # 4294967295
rosso = rgb_to_uint32(255, 0, 0)           # 4278190335
verde = rgb_to_uint32(0, 255, 0)           # 16711935
blu = rgb_to_uint32(0, 0, 255)             # 65535
blu_navy = rgb_to_uint32(0, 0, 128)        # 32895
```

---

## Combinazioni Tipiche

### Dark Theme (Astronomy)

```json
{
  "background_color": 255,
  "stars": {
    "use_spectral_colors": true,
    "default_color": 4294967295,
    "show_names": true,
    "min_magnitude_for_label": 4.0
  }
}
```

### Light Theme (Print)

```json
{
  "background_color": 4294967295,
  "stars": {
    "use_spectral_colors": false,
    "default_color": 0,
    "show_names": true,
    "min_magnitude_for_label": 3.0
  }
}
```

### High Contrast

```json
{
  "background_color": 255,
  "stars": {
    "min_symbol_size": 2.0,
    "max_symbol_size": 12.0,
    "use_spectral_colors": false,
    "default_color": 4294967295,
    "show_names": true,
    "label_font_size": 12.0
  }
}
```

---

## Note Rendering

- Le stelle sono renderizzate come cerchi antialiased
- Stelle luminose (m < 3) hanno blur gaussiano
- Etichette posizionate automaticamente evitando sovrapposizioni
- L'ordine di rendering: background → griglia → stelle → overlay → UI

---

[← Griglia](03_grid.md) | [Indice](../JSON_CONFIGURATION.md) | [Prossimo: UI →](05_ui.md)
