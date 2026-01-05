# Griglia e Coordinate

Configurazione della griglia di coordinate celesti e etichettatura.

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

## Step Griglia

### Ascensione Retta

Conversione ore-gradi:
```
step_gradi = step_ore × 15
```

| Step Ore | Step Gradi | Uso Tipico |
|----------|------------|------------|
| 4h | 60° | All-sky |
| 2h | 30° | Emisfero |
| 1h | 15° | Costellazione |
| 30m | 7.5° | Campo medio |
| 15m | 3.75° | Campo stretto |
| 5m | 1.25° | Dettaglio alto |
| 1m | 0.25° | CCD field |

**Esempi**:

```json
{
  "grid": {
    "ra_step": 15.0  // 1 ora
  }
}
```

```json
{
  "grid": {
    "ra_step": 1.25  // 5 minuti
  }
}
```

### Declinazione

| Step | Uso |
|------|-----|
| 30° | All-sky |
| 15° | Emisfero |
| 10° | Costellazione |
| 5° | Campo medio |
| 1° | Campo stretto |
| 0.5° | Dettaglio |
| 0.1° | CCD field |

**Raccomandazione**: Usa 5-10 linee per dimensione del campo.

Esempio: FOV 20° × 20° → step 2-4°

---

## Formato Etichette

### Formato HMS/DMS (Default)

Ascensione Retta in ore-minuti-secondi:
```
5h 35m 17.3s
```

Declinazione in gradi-arcmin-arcsec:
```
+5° 23' 28"
```

### Formato Decimale

Entrambi in gradi decimali:
```
RA: 83.822°
Dec: +5.391°
```

**Configurazione futura** (non ancora implementata):

```json
{
  "grid": {
    "label_format": "hms_dms"  // o "decimal" o "hm_dm"
  }
}
```

---

## Stili Griglia

### Griglia Standard

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 15.0,
    "dec_step": 10.0,
    "color": 1077952767,
    "line_width": 0.5
  }
}
```

**Risultato**: Griglia grigio scuro, linee sottili

### Griglia Prominente

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 1.0,
    "dec_step": 1.0,
    "color": 3435973887,
    "line_width": 1.0,
    "show_labels": true,
    "label_font_size": 12.0
  }
}
```

**Risultato**: Griglia chiara, linee spesse, etichette grandi

### Griglia Minimal

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 5.0,
    "dec_step": 5.0,
    "color": 1073741824,
    "line_width": 0.3,
    "show_labels": false
  }
}
```

**Risultato**: Griglia molto sottile, senza etichette

---

## Colori Griglia

### Colori Comuni

| Colore | RGBA Hex | uint32 | Descrizione |
|--------|----------|--------|-------------|
| Grigio scuro | 0x404040FF | 1077952767 | Standard, discreto |
| Grigio medio | 0x808080FF | 2155905279 | Visibile ma non invasivo |
| Grigio chiaro | 0xCCCCCCFF | 3435973887 | Prominente |
| Blu scuro | 0x000080FF | 32895 | Elegante |
| Rosso scuro | 0x800000FF | 2147483903 | Contrasto forte |

### Trasparenza

Ultimo byte = alpha (0-255):
- 255 (FF): Opaco
- 192 (C0): 75% opacità
- 128 (80): 50% opacità
- 64 (40): 25% opacità

Esempio griglia semi-trasparente:

```json
{
  "grid": {
    "color": 1077952639  // 0x4040407F (50% opacità)
  }
}
```

---

## Esempi per Tipo di Mappa

### Finder Chart IAU

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 1.0,
    "dec_step": 1.0,
    "color": 1077952767,
    "line_width": 1.0,
    "show_labels": true,
    "label_color": 3435973887,
    "label_font_size": 10.0
  }
}
```

### Carta Dettaglio

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 0.25,
    "dec_step": 0.25,
    "color": 1073741824,
    "line_width": 0.5,
    "show_labels": true,
    "label_font_size": 8.0
  }
}
```

### Wide Field

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 5.0,
    "dec_step": 5.0,
    "color": 2155905279,
    "line_width": 0.8,
    "show_labels": true,
    "label_font_size": 12.0
  }
}
```

### Survey All-Sky

```json
{
  "grid": {
    "enabled": true,
    "ra_step": 30.0,
    "dec_step": 30.0,
    "color": 3435973887,
    "line_width": 1.5,
    "show_labels": true,
    "label_font_size": 14.0
  }
}
```

---

## Disabilitare Griglia

Per mappe senza griglia:

```json
{
  "grid": {
    "enabled": false
  }
}
```

O semplicemente ometti la sezione `grid` (usa default che poi può essere disabilitato nel codice).

---

## Rendering Griglia

### Algoritmo

Per ogni linea RA costante (α = α₀):
1. Sample punti da δ_min a δ_max con step 0.1°
2. Proietta ciascun punto: (x, y) = project(α₀, δ)
3. Connetti punti con segmenti

Per ogni linea Dec costante (δ = δ₀):
1. Sample punti da α_min a α_max con step 0.1°
2. Proietta ciascun punto: (x, y) = project(α, δ₀)
3. Connetti punti con segmenti

### Densità Punti

Numero punti per linea:
```
n = range_deg / 0.1°
```

Esempio: FOV 10° → 100 punti per linea

Per campi grandi (>45°), aumenta sampling per seguire curvatura della proiezione.

---

## Note Tecniche

- Le linee della griglia seguono la curvatura della proiezione scelta
- Per proiezione Gnomonic, i cerchi massimi appaiono come linee rette
- Per proiezione Mercator, meridiani e paralleli sono perpendicolari
- Etichette posizionate ai bordi del campo visivo
- Anti-aliasing automatico per linee sottili

---

[← Performance](02_performance.md) | [Indice](../JSON_CONFIGURATION.md) | [Prossimo: Stelle →](04_stars.md)
