# Overlay Avanzati (Rettangoli, Path, Marker)

Configurazione di overlay personalizzati per regioni, tracce e marker.

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

### Parametri

| Campo | Tipo | Descrizione |
|-------|------|-------------|
| `enabled` | bool | Abilita rettangolo |
| `center_ra` | double | RA centro (gradi) |
| `center_dec` | double | Dec centro (gradi) |
| `width_ra` | double | Larghezza in RA (gradi) |
| `height_dec` | double | Altezza in Dec (gradi) |
| `color` | uint32 | Colore bordo RGBA |
| `line_width` | float | Spessore linea pixel |
| `filled` | bool | Se riempito |
| `fill_color` | uint32 | Colore riempimento |
| `label` | string | Etichetta rettangolo |

### Conversione RA in Gradi

Per specificare larghezza in ore di RA:
```
larghezza_gradi = larghezza_ore × 15
```

Esempi:
- 1 ora = 15°
- 0.5 ore (30 min) = 7.5°
- 0.25 ore (15 min) = 3.75°

### Esempi

**FOV osservazione**:
```json
{
  "overlay_rectangles": [
    {
      "center_ra": 83.75,
      "center_dec": 5.0,
      "width_ra": 0.75,
      "height_dec": 0.75,
      "color": 16711935,
      "line_width": 2.0,
      "label": "FOV Telescopio 2°"
    }
  ]
}
```

**Regione occultazione**:
```json
{
  "overlay_rectangles": [
    {
      "center_ra": 85.0,
      "center_dec": 4.5,
      "width_ra": 1.5,
      "height_dec": 1.5,
      "color": 4294967295,
      "line_width": 3.0,
      "filled": true,
      "fill_color": 855638323,
      "label": "Zona Occultazione"
    }
  ]
}
```

**Multipli rettangoli**:
```json
{
  "overlay_rectangles": [
    {
      "center_ra": 80.0,
      "center_dec": 0.0,
      "width_ra": 5.0,
      "height_dec": 5.0,
      "color": 4278190335,
      "label": "Zone1"
    },
    {
      "center_ra": 90.0,
      "center_dec": 5.0,
      "width_ra": 5.0,
      "height_dec": 5.0,
      "color": 16711935,
      "label": "Zone2"
    }
  ]
}
```

---

## Path Overlay (Tracce)

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

### Parametri Path

| Campo | Tipo | Descrizione |
|-------|------|-------------|
| `enabled` | bool | Abilita path |
| `color` | uint32 | Colore linea |
| `line_width` | float | Spessore linea pixel |
| `show_points` | bool | Mostra nodi |
| `point_size` | float | Dimensione nodi pixel |
| `show_labels` | bool | Mostra label nodi |
| `show_direction` | bool | Mostra freccia direzione |
| `label` | string | Nome path |
| `points` | array | Array punti {ra, dec, label} |

### Parametri Punto

| Campo | Tipo | Descrizione |
|-------|------|-------------|
| `ra` | double | Ascensione retta (gradi) |
| `dec` | double | Declinazione (gradi) |
| `label` | string | Etichetta punto (opzionale) |

### Interpolazione

La libreria interpola automaticamente tra punti:
- Catmull-Rom spline per curve fluide
- Segue curvatura della proiezione

### Esempi

**Traccia asteroide semplice**:
```json
{
  "overlay_paths": [
    {
      "color": 16711935,
      "line_width": 2.0,
      "show_points": true,
      "label": "Apophis 2029",
      "points": [
        {"ra": 12.0, "dec": 45.0, "label": "2029-04-13"},
        {"ra": 13.5, "dec": 44.5, "label": "2029-04-14"},
        {"ra": 15.0, "dec": 43.8, "label": "2029-04-15"}
      ]
    }
  ]
}
```

**Con direzione e timeline**:
```json
{
  "overlay_paths": [
    {
      "color": 4294967295,
      "line_width": 3.0,
      "show_points": true,
      "show_labels": true,
      "show_direction": true,
      "label": "Cometa C/2025",
      "points": [
        {"ra": 100.0, "dec": 20.0, "label": "Jan 2025"},
        {"ra": 105.0, "dec": 25.0, "label": "Feb 2025"},
        {"ra": 110.0, "dec": 28.0, "label": "Mar 2025"},
        {"ra": 115.0, "dec": 30.0, "label": "Apr 2025"}
      ]
    }
  ]
}
```

**Multipli path**:
```json
{
  "overlay_paths": [
    {
      "color": 16711935,
      "label": "Luna",
      "points": [...]
    },
    {
      "color": 65535,
      "label": "Giove",
      "points": [...]
    }
  ]
}
```

---

## Marker Personalizzati

Supporto per marker custom tramite path a 1 punto:

```json
{
  "overlay_paths": [
    {
      "enabled": true,
      "color": 4278190335,
      "show_points": true,
      "point_size": 8.0,
      "label": "Target",
      "points": [
        {"ra": 83.75, "dec": 5.0, "label": "Stella X"}
      ]
    }
  ]
}
```

Crea un marker azzurro singolo a coordinate specifiche.

---

## Combinazioni Overlay Avanzati

### Occultazione Asteroide

```json
{
  "overlay_rectangles": [
    {
      "center_ra": 85.0,
      "center_dec": 5.0,
      "width_ra": 2.0,
      "height_dec": 2.0,
      "color": 4294967295,
      "filled": true,
      "fill_color": 855638323,
      "label": "Zona Occultazione"
    }
  ],
  "overlay_paths": [
    {
      "color": 4278255360,
      "line_width": 2.5,
      "show_direction": true,
      "label": "Traccia Asteroide",
      "points": [
        {"ra": 83.0, "dec": 6.0, "label": "T-2h"},
        {"ra": 84.0, "dec": 5.5, "label": "T-1h"},
        {"ra": 85.0, "dec": 5.0, "label": "T0"},
        {"ra": 86.0, "dec": 4.5, "label": "T+1h"}
      ]
    }
  ]
}
```

### Multi-Target Survey

```json
{
  "overlay_rectangles": [
    {
      "center_ra": 80.0,
      "center_dec": 0.0,
      "width_ra": 1.0,
      "height_dec": 1.0,
      "color": 16711935,
      "label": "Target 1"
    },
    {
      "center_ra": 90.0,
      "center_dec": 10.0,
      "width_ra": 1.0,
      "height_dec": 1.0,
      "color": 65535,
      "label": "Target 2"
    }
  ]
}
```

---

## Note Rendering

- Tutti gli overlay seguono la curvatura della proiezione
- Path sono interpolati per curve fluide
- Ordine rendering: overlay_rectangles → overlay_paths → UI
- Trasparenza supportata negli uint32 color

---

[← Legenda](06_overlays_base.md) | [Indice](../JSON_CONFIGURATION.md) | [Prossimo: Esempi →](08_examples.md)
