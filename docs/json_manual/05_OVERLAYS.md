# Capitolo 5: Overlay e Elementi Grafici

Configurazione avanzata di overlay celesti, rettangoli annotazione e tracce temporali.

## 5.1 Overlay Celesti

```json
{
  "overlays": {
    "constellation_lines": true,
    "constellation_boundaries": false,
    "constellation_names": true,
    "milky_way": false,
    "ecliptic": false,
    "equator": false,
    "galactic_plane": false
  }
}
```

### 5.1.1 Linee Costellazioni

**Data source**: IAU constellation stick figures

File formato: `constellation_lines.dat`

```
AND 3
0 17.4333 29.0906  # α And (Alpheratz)
1 1.1621 35.6206   # β And (Mirach)
2 2.0654 42.3297   # γ And (Almach)
...
```

**Rendering**:

Per ogni coppia $((\alpha_1, \delta_1), (\alpha_2, \delta_2))$:

1. Proietta entrambi i punti:
   $$\begin{aligned}
   (x_1, y_1) &= \text{project}(\alpha_1, \delta_1) \\
   (x_2, y_2) &= \text{project}(\alpha_2, \delta_2)
   \end{aligned}$$

2. Verifica visibilità (almeno un punto dentro FOV)

3. Disegna segmento con anti-aliasing (Bresenham + Wu)

**Stile linee**:

```json
{
  "overlays": {
    "constellation_lines": true,
    "constellation_line_color": 1073741824,
    "constellation_line_width": 1.0,
    "constellation_line_style": "dashed"
  }
}
```

**Pattern dashed**:

$$\text{dash} = [10, 5, 3, 5]$$

Implementazione:

```cpp
void drawDashedLine(float x1, float y1, float x2, float y2,
                    const std::vector<float>& pattern) {
    float dx = x2 - x1, dy = y2 - y1;
    float length = std::sqrt(dx*dx + dy*dy);
    float ux = dx / length, uy = dy / length;
    
    float pos = 0.0;
    int pattern_idx = 0;
    bool drawing = true;
    
    while (pos < length) {
        float segment_len = pattern[pattern_idx % pattern.size()];
        float next_pos = std::min(pos + segment_len, length);
        
        if (drawing) {
            drawLine(x1 + pos * ux, y1 + pos * uy,
                    x1 + next_pos * ux, y1 + next_pos * uy);
        }
        
        pos = next_pos;
        drawing = !drawing;
        pattern_idx++;
    }
}
```

### 5.1.2 Confini Costellazioni

**Data source**: Delporte constellation boundaries (1930)

Formato: Segmenti RA/Dec allineati all'equatore J1875

**Conversione J1875 → J2000**:

Precessione:

$$\Delta\alpha \approx +3.07^s + 1.34^s \sin\alpha \tan\delta$$
$$\Delta\delta \approx +20.04'' \cos\alpha$$

**Rendering**:

Simile a linee, ma con:
- Line width: 0.5 px
- Color: grigio scuro (`0x808080FF`)
- Style: dotted

### 5.1.3 Nomi Costellazioni

**Posizionamento**:

Centroide poligono confine:

$$\begin{aligned}
\alpha_c &= \frac{1}{n} \sum_{i=1}^n \alpha_i \\
\delta_c &= \frac{1}{n} \sum_{i=1}^n \delta_i
\end{aligned}$$

**Abbreviazioni IAU**:

| Cod | Nome Latino | Nome Italiano |
|-----|-------------|---------------|
| AND | Andromeda | Andromeda |
| AQL | Aquila | Aquila |
| ARI | Aries | Ariete |
| CNC | Cancer | Cancro |
| CMA | Canis Major | Cane Maggiore |
| GEM | Gemini | Gemelli |
| LEO | Leo | Leone |
| ORI | Orion | Orione |
| SCO | Scorpius | Scorpione |
| ... | ... | ... |

**Font style**:

```json
{
  "overlays": {
    "constellation_names": true,
    "constellation_name_font_size": 14.0,
    "constellation_name_color": 3435973887,
    "constellation_name_style": "abbreviation"
  }
}
```

Stili: `abbreviation`, `latin`, `vernacular`

---

## 5.2 Elementi Astronomici

### 5.2.1 Via Lattea

**Data source**: Contorni densità superficiale

File: `milky_way_outlines.dat`

Isofote: $\mu_V = 22, 23, 24$ mag/arcsec²

**Rendering**:

Poligoni riempiti con gradiente di intensità:

$$I(x, y) = I_0 \times \exp\left(-\frac{d^2}{2\sigma^2}\right)$$

dove $d$ = distanza dal centro galattico proiettato.

**Colori**:

```json
{
  "overlays": {
    "milky_way": true,
    "milky_way_color": 4292927712,
    "milky_way_opacity": 0.3
  }
}
```

Colore base: Giallo pallido con alpha blending

### 5.2.2 Eclittica

**Definizione**:

Piano orbita terrestre, inclinato $\epsilon = 23.439291°$ rispetto equatore.

**Parametrizzazione**:

$$\begin{aligned}
\alpha(\lambda) &= \arctan\left(\frac{\sin\lambda \cos\epsilon}{\cos\lambda}\right) \\
\delta(\lambda) &= \arcsin(\sin\lambda \sin\epsilon)
\end{aligned}$$

con $\lambda \in [0°, 360°]$ (longitudine eclittica).

**Rendering**:

Sample ogni $\Delta\lambda = 1°$, connetti punti.

**Annotazioni**:

Posizioni solstizi/equinozi:

| Punto | $\lambda$ | RA | Dec | Data |
|-------|-----------|-----|-----|------|
| Equinozio primavera | 0° | 0h | 0° | ~20 Mar |
| Solstizio estate | 90° | 6h | +23.44° | ~21 Giu |
| Equinozio autunno | 180° | 12h | 0° | ~23 Set |
| Solstizio inverno | 270° | 18h | -23.44° | ~21 Dic |

```json
{
  "overlays": {
    "ecliptic": true,
    "ecliptic_color": 4294934528,
    "ecliptic_show_solstices": true
  }
}
```

### 5.2.3 Equatore Celeste

**Definizione**:

Proiezione equatore terrestre sulla sfera celeste.

**Equazione**:

$$\delta = 0°, \quad \alpha \in [0°, 360°]$$

**Rendering**:

Linea semplice, sample ogni $\Delta\alpha = 1°$.

```json
{
  "overlays": {
    "equator": true,
    "equator_color": 4278255360,
    "equator_line_width": 1.5
  }
}
```

### 5.2.4 Piano Galattico

**Definizione**:

$b = 0°$ in coordinate galattiche.

**Conversione**:

Dal piano $b = 0$, calcola $(\alpha, \delta)$ per $l \in [0°, 360°]$.

Usa matrice rotazione inversa:

$$\mathbf{R}_{\text{gal}}^{-1} = \mathbf{R}_{\text{gal}}^T$$

---

## 5.3 Rettangoli Overlay

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
      "fill_color": 1073741824,
      "fill_opacity": 0.2,
      "label": "Target Region",
      "label_position": "top"
    }
  ]
}
```

### 5.3.1 Geometria Rettangolo

**Vertici** (coordinate celesti):

$$\begin{aligned}
\alpha_1 &= \alpha_c - \Delta\alpha / 2, \quad \delta_1 = \delta_c - \Delta\delta / 2 \\
\alpha_2 &= \alpha_c + \Delta\alpha / 2, \quad \delta_2 = \delta_c - \Delta\delta / 2 \\
\alpha_3 &= \alpha_c + \Delta\alpha / 2, \quad \delta_3 = \delta_c + \Delta\delta / 2 \\
\alpha_4 &= \alpha_c - \Delta\alpha / 2, \quad \delta_4 = \delta_c + \Delta\delta / 2
\end{aligned}$$

**Correzione Dec**:

Larghezza RA dipende da declinazione:

$$\Delta\alpha_{\text{real}} = \frac{\Delta\alpha}{\cos\delta_c}$$

Per rettangoli piccoli ($<10°$), approssimazione sufficiente.

Per rettangoli grandi, usa great circle arcs.

### 5.3.2 Rendering con Curvatura

**Proiezione stereografica**: Rettangoli celesti → curve sferiche proiettate

**Algoritmo**:

Per ciascun lato:

1. Sample $N$ punti lungo great circle
2. Proietta tutti i punti
3. Connetti con segmenti

Numero sample:

$$N = \max\left(10, \frac{L_{\text{deg}}}{0.5°}\right)$$

dove $L_{\text{deg}}$ = lunghezza lato in gradi.

**Esempio**:

Lato $10°$ → $N = 20$ sample

### 5.3.3 Fill Pattern

**Solid fill**:

Riempimento uniforme con $\alpha$ blending:

```cpp
for (int y = y_min; y <= y_max; ++y) {
    for (int x = x_min; x <= x_max; ++x) {
        if (isInsidePolygon(x, y, vertices)) {
            blendPixel(x, y, fill_color, fill_opacity);
        }
    }
}
```

**Hatch pattern** (futuro):

Linee diagonali spaziate:

$$y = x \tan\theta + k \cdot d$$

con $\theta = 45°$, $d = 5$ px (spacing).

---

## 5.4 Path Overlay (Tracce Temporali)

```json
{
  "overlay_paths": [
    {
      "enabled": true,
      "color": 4278255360,
      "line_width": 2.5,
      "show_points": true,
      "point_size": 4.0,
      "point_style": "circle",
      "show_labels": true,
      "show_direction": true,
      "label": "433 Eros Trajectory",
      "points": [
        {"ra": 85.123, "dec": 5.234, "time": "2026-01-05T20:00:00Z", "label": "T0"},
        {"ra": 85.456, "dec": 5.012, "time": "2026-01-05T21:00:00Z", "label": "T1"},
        {"ra": 85.789, "dec": 4.790, "time": "2026-01-05T22:00:00Z", "label": "T2"},
        {"ra": 86.122, "dec": 4.568, "time": "2026-01-05T23:00:00Z", "label": "T3"}
      ]
    }
  ]
}
```

### 5.4.1 Interpolazione Path

**Linear interpolation** (default):

Segmenti retti tra punti consecutivi.

**Spline cubica** (smooth):

Hermite spline:

$$\mathbf{P}(t) = (2t^3 - 3t^2 + 1)\mathbf{P}_0 + (t^3 - 2t^2 + t)\mathbf{m}_0 + (-2t^3 + 3t^2)\mathbf{P}_1 + (t^3 - t^2)\mathbf{m}_1$$

dove $t \in [0,1]$, $\mathbf{m}_i$ = tangenti.

**Calcolo tangenti** (Catmull-Rom):

$$\mathbf{m}_i = \frac{\mathbf{P}_{i+1} - \mathbf{P}_{i-1}}{2}$$

**Implementazione**:

```cpp
std::vector<Point> interpolateSpline(const std::vector<Point>& ctrl_points, int samples) {
    std::vector<Point> result;
    
    for (size_t i = 0; i < ctrl_points.size() - 1; ++i) {
        auto p0 = ctrl_points[i];
        auto p1 = ctrl_points[i + 1];
        auto m0 = (i > 0) ? (ctrl_points[i+1] - ctrl_points[i-1]) * 0.5 : (p1 - p0);
        auto m1 = (i < ctrl_points.size() - 2) ? 
                  (ctrl_points[i+2] - ctrl_points[i]) * 0.5 : (p1 - p0);
        
        for (int j = 0; j <= samples; ++j) {
            float t = static_cast<float>(j) / samples;
            float t2 = t * t, t3 = t2 * t;
            
            auto p = p0 * (2*t3 - 3*t2 + 1) +
                     m0 * (t3 - 2*t2 + t) +
                     p1 * (-2*t3 + 3*t2) +
                     m1 * (t3 - t2);
            
            result.push_back(p);
        }
    }
    
    return result;
}
```

### 5.4.2 Indicatori Direzionali

**Frecce lungo il path**:

Ogni $N$ segmenti, disegna freccia:

$$\mathbf{v} = \frac{\mathbf{P}_{i+1} - \mathbf{P}_i}{||\mathbf{P}_{i+1} - \mathbf{P}_i||}$$

Perpendic olare:

$$\mathbf{v}_\perp = (-v_y, v_x)$$

Punta freccia:

$$\begin{aligned}
\mathbf{P}_{\text{tip}} &= \mathbf{P}_i + 0.5 \times \mathbf{v} \\
\mathbf{P}_{\text{left}} &= \mathbf{P}_i + 0.3 \times \mathbf{v} - 0.2 \times \mathbf{v}_\perp \\
\mathbf{P}_{\text{right}} &= \mathbf{P}_i + 0.3 \times \mathbf{v} + 0.2 \times \mathbf{v}_\perp
\end{aligned}$$

Disegna triangolo: $[\mathbf{P}_{\text{tip}}, \mathbf{P}_{\text{left}}, \mathbf{P}_{\text{right}}]$

### 5.4.3 Animazione Temporale

**Time-based coloring**:

Colore varia lungo il path in base al tempo:

$$\text{color}(t) = \text{lerp}(\text{color}_{\text{start}}, \text{color}_{\text{end}}, t_{\text{norm}})$$

dove:

$$t_{\text{norm}} = \frac{t - t_0}{t_{\text{end}} - t_0}$$

**Heat map**:

$$\text{RGB}(t) = \begin{cases}
(0, 0, 255) & t < 0.25 \quad \text{(blu)} \\
(0, 255, 0) & 0.25 \le t < 0.75 \quad \text{(verde)} \\
(255, 0, 0) & t \ge 0.75 \quad \text{(rosso)}
\end{cases}$$

Con interpolazione smooth.

### 5.4.4 Esempio Occultazione Asteroidale

```json
{
  "center": {"ra": 85.5, "dec": 4.9},
  "field_of_view": {"width": 2.0, "height": 2.0},
  "limiting_magnitude": 13.0,
  
  "overlay_paths": [
    {
      "enabled": true,
      "label": "433 Eros occults TYC 1234-5678",
      "color": 4278190335,
      "line_width": 3.0,
      "show_points": true,
      "point_size": 5.0,
      "show_labels": true,
      "show_direction": true,
      "interpolation": "spline",
      "points": [
        {"ra": 85.120, "dec": 5.010, "time": "2026-01-05T20:00:00Z", "label": "C-60min"},
        {"ra": 85.250, "dec": 4.950, "time": "2026-01-05T20:30:00Z", "label": "C-30min"},
        {"ra": 85.380, "dec": 4.890, "time": "2026-01-05T20:55:00Z", "label": "C-5min"},
        {"ra": 85.445, "dec": 4.860, "time": "2026-01-05T21:00:00Z", "label": "Contact", "marker": "star"},
        {"ra": 85.510, "dec": 4.830, "time": "2026-01-05T21:05:00Z", "label": "C+5min"},
        {"ra": 85.640, "dec": 4.770, "time": "2026-01-05T21:30:00Z", "label": "C+30min"}
      ]
    }
  ],
  
  "overlay_rectangles": [
    {
      "enabled": true,
      "center_ra": 85.445,
      "center_dec": 4.860,
      "width_ra": 0.5,
      "height_dec": 0.5,
      "color": 4294934528,
      "line_width": 2.0,
      "label": "Critical Zone"
    }
  ]
}
```

---

## 5.5 Overlay Personalizzati

### 5.5.1 Markers Personalizzati

```json
{
  "custom_markers": [
    {
      "ra": 83.822,
      "dec": -5.391,
      "symbol": "cross",
      "size": 10.0,
      "color": 4278190335,
      "label": "Betelgeuse"
    }
  ]
}
```

**Simboli disponibili**:

| Tipo | Rendering |
|------|-----------|
| `circle` | Cerchio |
| `cross` | Croce + |
| `x` | Croce × |
| `square` | Quadrato |
| `diamond` | Rombo |
| `star` | Stella 5 punte |
| `triangle` | Triangolo |

### 5.5.2 Ellissi (Error Regions)

```json
{
  "error_ellipses": [
    {
      "center_ra": 85.445,
      "center_dec": 4.860,
      "semi_major": 0.1,
      "semi_minor": 0.05,
      "position_angle": 30.0,
      "color": 4278255360,
      "confidence": 0.95
    }
  ]
}
```

**Parametrizzazione**:

$$\begin{aligned}
x(t) &= a \cos t \cos\theta - b \sin t \sin\theta \\
y(t) &= a \cos t \sin\theta + b \sin t \cos\theta
\end{aligned}$$

con $t \in [0, 2\pi]$, $\theta$ = position angle.

**Scaling per confidence**:

$$\sigma = \chi^2_{\text{inv}}(1 - \alpha, 2)$$

Per $95\%$: $\sigma = 2.448$

Ellisse: $(a \times \sigma, b \times \sigma)$

---

## 5.6 Esempi Completi

### 5.6.1 Mappa Costellazione Completa

```json
{
  "center": {"ra": 88.793, "dec": 7.407},
  "field_of_view": {"width": 30.0, "height": 30.0},
  "image": {"width": 4000, "height": 4000},
  "limiting_magnitude": 10.0,
  
  "overlays": {
    "constellation_lines": true,
    "constellation_boundaries": true,
    "constellation_names": true,
    "constellation_name_style": "latin",
    "milky_way": false,
    "ecliptic": true,
    "equator": true
  },
  
  "grid": {
    "enabled": true,
    "ra_step": 2.0,
    "dec_step": 5.0
  },
  
  "title": {
    "text": "Orion Constellation",
    "font_size": 24.0
  }
}
```

### 5.6.2 Carta Occultazione Dettagliata

```json
{
  "center": {"ra": 85.445, "dec": 4.860},
  "field_of_view": {"width": 1.0, "height": 1.0},
  "image": {"width": 3000, "height": 3000},
  "limiting_magnitude": 14.0,
  
  "grid": {
    "enabled": true,
    "ra_step": 0.1,
    "dec_step": 0.1,
    "label_format": "hm_dm"
  },
  
  "overlay_paths": [
    {
      "label": "Shadow Path",
      "color": 4278190335,
      "line_width": 4.0,
      "show_direction": true,
      "interpolation": "spline",
      "points": [ /* ephemeris data */ ]
    }
  ],
  
  "overlay_rectangles": [
    {
      "label": "CCD Field",
      "center_ra": 85.445,
      "center_dec": 4.860,
      "width_ra": 0.25,
      "height_dec": 0.25,
      "color": 4294934528,
      "filled": true,
      "fill_opacity": 0.1
    }
  ],
  
  "custom_markers": [
    {
      "ra": 85.445,
      "dec": 4.860,
      "symbol": "star",
      "size": 15.0,
      "color": 4294934528,
      "label": "Target Star"
    }
  ]
}
```

---

[← Elementi UI](04_UI_ELEMENTS.md) | [Indice](00_INDEX.md) | [Prossimo: Esempi Avanzati →](06_ADVANCED_EXAMPLES.md)
