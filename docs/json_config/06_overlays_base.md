# Overlay Base (Legenda, Costellazioni, Elementi Astronomici)

Configurazione della legenda magnitudini e overlay base.

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

### Parametri

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `enabled` | bool | true | Mostra legenda |
| `position` | int | 4 | Posizione (0-5) |
| `custom_x` | float | 0.0 | X custom (0-1) |
| `custom_y` | float | 0.0 | Y custom (0-1) |
| `font_size` | float | 10.0 | Dimensione font |
| `text_color` | uint32 | 0xFFFFFFFF | Colore testo |
| `background_color` | uint32 | 0xCCCCCC80 | Colore sfondo |
| `show_background` | bool | true | Mostra sfondo |

### Posizioni

Stesse del titolo:
- 1 = TOP_LEFT
- 2 = TOP_RIGHT
- 3 = BOTTOM_LEFT
- 4 = BOTTOM_RIGHT (default)
- 5 = CUSTOM

### Esempi

**Standard (bottom-right)**:
```json
{
  "magnitude_legend": {
    "enabled": true,
    "position": 4,
    "font_size": 10.0
  }
}
```

**Custom (top-left)**:
```json
{
  "magnitude_legend": {
    "position": 5,
    "custom_x": 0.05,
    "custom_y": 0.05,
    "font_size": 12.0,
    "show_background": true,
    "background_color": 2155905119
  }
}
```

**Disabilitata**:
```json
{
  "magnitude_legend": {
    "enabled": false
  }
}
```

---

## Overlay Celesti

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

### Parametri

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `constellation_lines` | bool | false | Linee costellazioni |
| `constellation_boundaries` | bool | false | Confini costellazioni |
| `constellation_names` | bool | false | Nomi costellazioni |
| `milky_way` | bool | false | Via Lattea |
| `ecliptic` | bool | false | Eclittica |
| `equator` | bool | false | Equatore celeste |

---

## Linee Costellazioni

```json
{
  "overlays": {
    "constellation_lines": true
  }
}
```

Mostra le linee di connessione tra stelle luminose che definiscono la forma della costellazione.

**Proprietà**:
- Linee sottili grigio scuro
- Seguono la curvatura della proiezione
- Non etichettate (usare constellation_names per nomi)

### Esempi

**Con nomi**:
```json
{
  "overlays": {
    "constellation_lines": true,
    "constellation_names": true
  }
}
```

---

## Confini Costellazioni

```json
{
  "overlays": {
    "constellation_boundaries": true
  }
}
```

Mostra i confini amministrativi delle 88 costellazioni IAU (Delporte 1930).

**Proprietà**:
- Linee punteggiate grigio chiaro
- Basate su coordinate eclittiche J1875 (convertite a J2000)
- Copertura completa del cielo

---

## Nomi Costellazioni

```json
{
  "overlays": {
    "constellation_names": true
  }
}
```

Mostra i nomi delle costellazioni al loro centroide.

**Formato**: Abbreviazioni IAU a 3 lettere (AND, AQL, ARI, CNC, ecc.)

### Combinazioni

```json
{
  "overlays": {
    "constellation_lines": true,
    "constellation_boundaries": false,
    "constellation_names": true
  }
}
```

Mostra linee + nomi (non confini).

---

## Via Lattea

```json
{
  "overlays": {
    "milky_way": true
  }
}
```

Mostra i contorni della Via Lattea come poligoni con shading.

**Proprietà**:
- Isofote densità superficiale Gaia DR3
- Gradiente giallo pallido
- Trasparenza per non coprire stelle

### Esempio Galactic Map

```json
{
  "overlays": {
    "milky_way": true
  },
  "projection": {
    "coordinate_system": "galactic"
  }
}
```

---

## Eclittica

```json
{
  "overlays": {
    "ecliptic": true
  }
}
```

Mostra il piano dell'orbita terrestre (inclinato 23.44° rispetto all'equatore).

**Proprietà**:
- Linea arancione/rossa
- Passa per solstizi e equinozi
- Utile per tracking pianeti

### Punti Speciali

La libreria marca i solstizi/equinozi:

| Punto | RA | Dec | Data |
|-------|-----|-----|------|
| Eq. Primavera | 0h | 0° | ~20 Mar |
| Sol. Estate | 6h | +23.44° | ~21 Giu |
| Eq. Autunno | 12h | 0° | ~23 Set |
| Sol. Inverno | 18h | -23.44° | ~21 Dic |

---

## Equatore Celeste

```json
{
  "overlays": {
    "equator": true
  }
}
```

Mostra l'equatore celeste (proiezione equatore terrestre sulla sfera).

**Proprietà**:
- Linea azzurra
- Passa per RA 0h-24h, Dec 0°
- Sempre orizzontale in coordinate equatoriali

---

## Combinazioni Overlay

### Mappa Finder Minima

```json
{
  "overlays": {
    "constellation_lines": false,
    "constellation_names": false,
    "milky_way": false,
    "ecliptic": false,
    "equator": false
  }
}
```

Solo griglia e stelle, nessun overlay.

### Mappa Costellazione Completa

```json
{
  "overlays": {
    "constellation_lines": true,
    "constellation_boundaries": true,
    "constellation_names": true,
    "milky_way": false,
    "ecliptic": true,
    "equator": true
  }
}
```

Completa con linee, confini, nomi, eclittica, equatore.

### Mappa Scientifica

```json
{
  "overlays": {
    "constellation_lines": false,
    "constellation_names": false,
    "milky_way": true,
    "ecliptic": false,
    "equator": false
  }
}
```

Via Lattea solo, per analisi struttura galattica.

### All-Sky Survey

```json
{
  "overlays": {
    "constellation_lines": true,
    "constellation_boundaries": true,
    "constellation_names": true,
    "milky_way": true,
    "ecliptic": true,
    "equator": true
  }
}
```

Tutti gli overlay per mappa completa.

---

## Note Tecniche

- Tutti gli overlay seguono la curvatura della proiezione scelta
- Per proiezione Mercator, eclittica e equatore sono linee rette
- Linee RA/Dec sono separate dagli overlay (vedi griglia)
- Overlay sono renderizzati dopo griglia, prima di UI

---

[← Stelle](04_stars.md) | [Indice](../JSON_CONFIGURATION.md) | [Prossimo: Overlay Avanzati →](07_overlays_advanced.md)
