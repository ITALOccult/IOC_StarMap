# Capitolo 3: Rendering Stelle e Griglia

Configurazione dettagliata del rendering di stelle, griglia coordinate e sistemi di visualizzazione.

## 3.1 Configurazione Griglia

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
    "label_font_size": 10.0,
    "label_format": "hms_dms"
  }
}
```

### 3.1.1 Step Griglia

**Ascensione Retta** (`ra_step`):

Conversione ore-gradi:

$$\text{step}_{\text{gradi}} = \text{step}_{\text{ore}} \times 15$$

| Step Ore | Step Gradi | Uso Tipico |
|----------|------------|------------|
| 4h | 60° | All-sky |
| 2h | 30° | Emisfero |
| 1h | 15° | Costellazione |
| 30m | 7.5° | Campo medio |
| 15m | 3.75° | Campo stretto |
| 5m | 1.25° | Dettaglio alto |
| 1m | 0.25° | CCD field |

**Declinazione** (`dec_step`):

| Step | Uso |
|------|-----|
| 30° | All-sky |
| 15° | Emisfero |
| 10° | Costellazione |
| 5° | Campo medio |
| 1° | Campo stretto |
| 0.5° | Dettaglio |

**Raccomandazione**: Usa $5-10$ linee per dimensione del campo.

Esempio: FOV $20° \times 20°$ → step $2-4°$

### 3.1.2 Rendering Griglia

**Algoritmo** (proiezione stereografica):

Per RA costante ($\alpha = \alpha_0$):

$$\delta \in [\delta_{\min}, \delta_{\max}], \quad \text{step } 0.1°$$

Per ogni $\delta_i$:

$$\begin{aligned}
x_i &= f_x(\alpha_0, \delta_i) \\
y_i &= f_y(\alpha_0, \delta_i)
\end{aligned}$$

Disegna linea da $(x_{i-1}, y_{i-1})$ a $(x_i, y_i)$.

Per Dec costante ($\delta = \delta_0$):

$$\alpha \in [\alpha_{\min}, \alpha_{\max}], \quad \text{step } 0.1°$$

**Densità punti**:

$$n_{\text{points}} = \frac{\text{range}}{\text{step}} = \frac{w_{\text{FOV}}}{0.1°}$$

Esempio: FOV $10°$ → $100$ punti per linea

### 3.1.3 Formato Etichette

```json
{
  "grid": {
    "label_format": "hms_dms"
  }
}
```

| Formato | RA | Dec | Esempio |
|---------|-----|-----|---------|
| `hms_dms` | $h^h m^m s^s$ | $d° m' s''$ | 5h 35m 17s, +5° 23' 28" |
| `decimal` | Gradi decimali | Gradi decimali | 83.822°, +5.391° |
| `hm_dm` | $h^h m^m$ | $d° m'$ | 5h 35m, +5° 23' |

**Conversione decimale → sessagesimale**:

RA (ore):

$$\begin{aligned}
h &= \lfloor \alpha / 15 \rfloor \\
m &= \lfloor (\alpha / 15 - h) \times 60 \rfloor \\
s &= ((\alpha / 15 - h) \times 60 - m) \times 60
\end{aligned}$$

Dec (gradi):

$$\begin{aligned}
\text{sign} &= \text{sgn}(\delta) \\
d &= \lfloor |\delta| \rfloor \\
m &= \lfloor (|\delta| - d) \times 60 \rfloor \\
s &= ((|\delta| - d) \times 60 - m) \times 60
\end{aligned}$$

---

## 3.2 Rendering Stelle

```json
{
  "stars": {
    "min_symbol_size": 0.5,
    "max_symbol_size": 8.0,
    "magnitude_range": 10.0,
    "use_spectral_colors": true,
    "default_color": 4294967295,
    "show_names": true,
    "show_sao_numbers": false,
    "show_magnitudes": false,
    "min_magnitude_for_label": 4.0,
    "label_color": 4294967295,
    "label_font_size": 8.0
  }
}
```

### 3.2.1 Dimensionamento Simboli

**Scaling logaritmico**:

$$s(m) = s_{\max} - (s_{\max} - s_{\min}) \times \frac{m - m_{\min}}{m_{\max} - m_{\min}}$$

dove:
- $s_{\min}$ = `min_symbol_size`
- $s_{\max}$ = `max_symbol_size`
- $m_{\min}$ = magnitudine più luminosa nel campo
- $m_{\max}$ = `limiting_magnitude`

**Esempio**:

Config: $s_{\min} = 0.5$, $s_{\max} = 8.0$, $m_{\text{range}} = 10.0$

Stelle:
- Betelgeuse ($m = 0.5$): $s = 8.0 - 7.5 \times 0.05 = 7.625$ px
- Stella media ($m = 5.0$): $s = 8.0 - 7.5 \times 0.5 = 4.25$ px
- Stella debole ($m = 10.0$): $s = 8.0 - 7.5 \times 1.0 = 0.5$ px

**Scaling Pogson** (alternativo):

Basato su rapporto di flusso:

$$s(m) = s_0 \times 10^{-0.4(m - m_0)}$$

Normalizzato:

$$s(m) = s_{\min} + (s_{\max} - s_{\min}) \times \left(\frac{10^{-0.4(m - m_{\min})} - 10^{-0.4 \Delta m}}{1 - 10^{-0.4 \Delta m}}\right)$$

dove $\Delta m = m_{\max} - m_{\min}$.

### 3.2.2 Rendering Simbolo

**Cerchio antialiased**:

Per ogni pixel $(x, y)$ attorno al centro stella $(x_s, y_s)$:

$$r = \sqrt{(x - x_s)^2 + (y - y_s)^2}$$

Alpha blending:

$$\alpha(r) = \begin{cases}
1 & r < s/2 - 0.5 \\
s/2 + 0.5 - r & s/2 - 0.5 \le r \le s/2 + 0.5 \\
0 & r > s/2 + 0.5
\end{cases}$$

**Gaussian blur** (stelle luminose):

Per $m < 3$:

$$I(r) = I_0 \exp\left(-\frac{r^2}{2\sigma^2}\right)$$

con $\sigma = s / 3$.

### 3.2.3 Colori Spettrali

```json
{
  "stars": {
    "use_spectral_colors": true
  }
}
```

**Indice colore B-V**:

Conversione B-V → RGB (relazione empirica):

$$\begin{aligned}
T_{\text{eff}} &= 4600 \left(\frac{1}{0.92(B-V) + 1.7} + \frac{1}{0.92(B-V) + 0.62}\right) \\
\end{aligned}$$

**Blackbody RGB** (Planck):

$$I_\lambda = \frac{2hc^2}{\lambda^5} \frac{1}{e^{hc/\lambda k_B T} - 1}$$

Integrazione su filtri RGB standard → $(R, G, B)$

**Lookup table** (implementazione efficiente):

| B-V | Colore | Tipo Spettrale | RGB |
|-----|--------|----------------|-----|
| -0.3 | Blu | O5 | (155, 176, 255) |
| 0.0 | Bianco-azzurro | A0 (Vega) | (248, 247, 255) |
| +0.3 | Bianco | F0 | (255, 244, 234) |
| +0.6 | Giallo | G2 (Sole) | (255, 237, 227) |
| +1.0 | Arancio | K5 | (255, 209, 178) |
| +1.5 | Rosso | M3 | (255, 180, 107) |
| +2.0 | Rosso scuro | M8 | (255, 149, 71) |

**Formula interpolazione**:

$$\text{RGB}(B-V) = \text{lerp}(\text{RGB}_1, \text{RGB}_2, t)$$

dove:

$$t = \frac{(B-V) - (B-V)_1}{(B-V)_2 - (B-V)_1}$$

### 3.2.4 Etichette Stelle

```json
{
  "stars": {
    "show_names": true,
    "show_sao_numbers": false,
    "show_magnitudes": false,
    "min_magnitude_for_label": 4.0
  }
}
```

**Condizione visualizzazione**:

$$\text{show\_label} = (m < m_{\text{min}}) \land (\text{has\_name} \lor \text{show\_sao})$$

**Posizionamento** (evita sovrapposizioni):

1. Posizione default: $(x_s + s + 2, y_s)$ (destra stella)
2. Se collisione: prova $(x_s, y_s - s - 2)$ (sopra)
3. Se collisione: prova $(x_s - w_{\text{label}} - 2, y_s)$ (sinistra)
4. Se collisione: prova $(x_s, y_s + s + 2)$ (sotto)

**Formato etichetta**:

```cpp
std::string getStarLabel(const Star& star) {
    std::string label;
    if (!star.name.empty()) {
        label = star.name;
    } else if (show_sao_numbers && star.sao_number > 0) {
        label = "SAO " + std::to_string(star.sao_number);
    }
    
    if (show_magnitudes) {
        label += " (" + formatMagnitude(star.magnitude) + ")";
    }
    
    return label;
}
```

---

## 3.3 Sistemi di Coordinate

### 3.3.1 Equatorial (J2000)

**Sistema standard**:
- Origine: Centro Terra
- Piano fondamentale: Equatore celeste (J2000.0)
- Asse X: Verso punto vernale $\gamma$
- Coordinate: $(\alpha, \delta)$

**Precessione** (da J2000 ad epoca $t$):

$$\begin{aligned}
\zeta &= 0.6406161 t + 0.0003041 t^2 + 0.0000051 t^3 \\
z &= 0.6406161 t + 0.0003041 t^2 + 0.0000051 t^3 \\
\theta &= 0.5567530 t - 0.0001185 t^2 - 0.0000116 t^3
\end{aligned}$$

dove $t = (J - 2000.0) / 100$ (secoli giuliani).

Rotazione:

$$\mathbf{R} = \mathbf{R}_z(-z) \cdot \mathbf{R}_y(\theta) \cdot \mathbf{R}_z(-\zeta)$$

### 3.3.2 Galactic

**Conversione Equatorial → Galactic**:

Polo nord galattico: $\alpha_{\text{NGP}} = 192.859°$, $\delta_{\text{NGP}} = 27.128°$

Longitudine nodo ascendente: $l_{\text{NCP}} = 122.932°$

Matrici di rotazione:

$$\begin{pmatrix} \cos l \cos b \\ \sin l \cos b \\ \sin b \end{pmatrix} = \mathbf{R}_{\text{gal}} \begin{pmatrix} \cos \alpha \cos \delta \\ \sin \alpha \cos \delta \\ \sin \delta \end{pmatrix}$$

dove:

$$\mathbf{R}_{\text{gal}} = \begin{pmatrix}
-0.0548756 & -0.8734371 & -0.4838350 \\
+0.4941095 & -0.4448296 & +0.7469823 \\
-0.8676661 & -0.1980764 & +0.4559838
\end{pmatrix}$$

### 3.3.3 Ecliptic

**Conversione Equatorial → Ecliptic**:

Obliquità eclittica (J2000): $\epsilon = 23.439291°$

$$\begin{aligned}
\tan \lambda &= \frac{\sin \alpha \cos \epsilon + \tan \delta \sin \epsilon}{\cos \alpha} \\
\sin \beta &= \sin \delta \cos \epsilon - \cos \delta \sin \epsilon \sin \alpha
\end{aligned}$$

Inversa:

$$\begin{aligned}
\tan \alpha &= \frac{\sin \lambda \cos \epsilon - \tan \beta \sin \epsilon}{\cos \lambda} \\
\sin \delta &= \sin \beta \cos \epsilon + \cos \beta \sin \epsilon \sin \lambda
\end{aligned}$$

### 3.3.4 Horizontal

**Conversione Equatorial → Horizontal**:

Richiede: latitudine $\phi$, LST (Local Sidereal Time)

Ora angolare:

$$H = \text{LST} - \alpha$$

Altitudine:

$$\sin h = \sin \phi \sin \delta + \cos \phi \cos \delta \cos H$$

Azimut:

$$\tan A = \frac{\sin H}{\cos H \sin \phi - \tan \delta \cos \phi}$$

Convenzione azimut: $0°$ = Nord, $90°$ = Est

**LST da tempo UTC**:

$$\text{LST} = \text{GST} + \lambda$$

GST (Greenwich Sidereal Time):

$$\text{GST} = 280.46061837 + 360.98564736629 \times (JD - 2451545.0)$$

---

## 3.4 Esempi Avanzati Rendering

### 3.4.1 Carta Finder Standard

```json
{
  "center": {"ra": 83.822, "dec": -5.391},
  "field_of_view": {"width": 15.0, "height": 15.0},
  "image": {"width": 2400, "height": 2400},
  "limiting_magnitude": 10.0,
  
  "grid": {
    "enabled": true,
    "ra_step": 1.0,
    "dec_step": 1.0,
    "color": 1077952767,
    "line_width": 1.0,
    "show_labels": true,
    "label_format": "hm_dm"
  },
  
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

### 3.4.2 Carta Dettaglio Colori

```json
{
  "center": {"ra": 88.793, "dec": 7.407},
  "field_of_view": {"width": 5.0, "height": 5.0},
  "image": {"width": 3000, "height": 3000},
  "limiting_magnitude": 13.0,
  
  "grid": {
    "enabled": true,
    "ra_step": 0.25,
    "dec_step": 0.25,
    "color": 1073741824,
    "line_width": 0.5,
    "show_labels": true
  },
  
  "stars": {
    "min_symbol_size": 0.3,
    "max_symbol_size": 10.0,
    "magnitude_range": 12.0,
    "use_spectral_colors": true,
    "show_names": true,
    "show_magnitudes": true,
    "min_magnitude_for_label": 8.0,
    "label_font_size": 7.0
  },
  
  "background_color": 255
}
```

### 3.4.3 Mappa Galactica

```json
{
  "center": {"ra": 266.417, "dec": -29.008},
  "field_of_view": {"width": 30.0, "height": 30.0},
  "image": {"width": 4000, "height": 4000},
  "limiting_magnitude": 11.0,
  
  "projection": {
    "type": "stereographic",
    "coordinate_system": "galactic"
  },
  
  "grid": {
    "enabled": true,
    "ra_step": 5.0,
    "dec_step": 5.0,
    "show_labels": true,
    "label_format": "decimal"
  },
  
  "stars": {
    "use_spectral_colors": true,
    "show_names": false
  },
  
  "overlays": {
    "milky_way": true,
    "constellation_lines": false
  }
}
```

---

## 3.5 Ottimizzazione Rendering

### 3.5.1 Level of Detail (LOD)

Adatta rendering alla scala:

$$\text{scale} = \frac{w_{\text{px}}}{w_{\text{FOV}}}$$

| Scale (px/°) | LOD | Rendering |
|--------------|-----|-----------|
| < 50 | Low | Punti semplici, no label |
| 50-200 | Medium | Cerchi antialiased, label selettive |
| 200-500 | High | Gaussian blur, tutte label |
| > 500 | Very High | Diffraction spikes |

### 3.5.2 Culling Visibilità

**Frustum culling**:

```cpp
bool isVisible(double x_px, double y_px, double size_px) {
    return (x_px + size_px >= 0) &&
           (x_px - size_px < width) &&
           (y_px + size_px >= 0) &&
           (y_px - size_px < height);
}
```

**Occlusione**: Stelle deboli dietro stelle luminose (raro, skip per performance)

### 3.5.3 Parallel Rendering

```cpp
#pragma omp parallel for schedule(dynamic, 100)
for (int i = 0; i < stars.size(); ++i) {
    auto [x, y] = project(stars[i].ra, stars[i].dec);
    if (isVisible(x, y, size)) {
        #pragma omp critical
        {
            drawStar(x, y, stars[i]);
        }
    }
}
```

**Speedup**: $2-4\times$ su CPU multi-core

---

[← Performance](02_PERFORMANCE.md) | [Indice](00_INDEX.md) | [Prossimo: Elementi UI →](04_UI_ELEMENTS.md)
