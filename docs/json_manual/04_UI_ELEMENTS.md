# Capitolo 4: Elementi UI e Annotazioni

Configurazione completa di titolo, bordo, bussola, scala angolare e legenda magnitudini.

## 4.1 Titolo

```json
{
  "title": {
    "enabled": true,
    "text": "Orion Region - Betelgeuse Field",
    "position": 1,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "font_size": 20.0,
    "text_color": 4294967295,
    "background_color": 2147483903,
    "show_background": true,
    "padding_x": 15.0,
    "padding_y": 10.0
  }
}
```

### 4.1.1 Posizionamento

**Posizioni predefinite** (`position`):

| Valore | Costante | Posizione | Coordinate |
|--------|----------|-----------|------------|
| 0 | NONE | Disabilitato | - |
| 1 | TOP_LEFT | Alto sinistra | $(m, m)$ |
| 2 | TOP_RIGHT | Alto destra | $(w - w_t - m, m)$ |
| 3 | BOTTOM_LEFT | Basso sinistra | $(m, h - h_t - m)$ |
| 4 | BOTTOM_RIGHT | Basso destra | $(w - w_t - m, h - h_t - m)$ |
| 5 | CUSTOM | Personalizzata | $(x_c \cdot w, y_c \cdot h)$ |

dove:
- $(w, h)$ = dimensioni immagine
- $(w_t, h_t)$ = dimensioni box titolo
- $m$ = margine (default 20px)
- $(x_c, y_c)$ = `custom_x`, `custom_y` (normalizzati $[0,1]$)

**Calcolo dimensioni box**:

$$\begin{aligned}
w_t &= \text{text\_width}(\text{text}, \text{font\_size}) + 2 \cdot \text{padding\_x} \\
h_t &= \text{font\_size} + 2 \cdot \text{padding\_y}
\end{aligned}$$

### 4.1.2 Stili Tipografici

**Font sizes** raccomandati:

| Uso | Dimensione | Quando |
|-----|------------|--------|
| Piccolo | 12-14 | Mappe compatte, web |
| Standard | 16-20 | Print A4, display |
| Grande | 24-32 | Poster, presentazioni |
| Header | 36-48 | Title slide |

**Contrasto** (WCAG 2.1):

Rapporto di contrasto minimo:

$$C = \frac{L_1 + 0.05}{L_2 + 0.05}$$

dove $L$ è la luminanza relativa:

$$L = \begin{cases}
\frac{RGB}{12.92} & RGB \le 0.03928 \\
\left(\frac{RGB + 0.055}{1.055}\right)^{2.4} & RGB > 0.03928
\end{cases}$$

con $RGB = R/255$ (normalizzato).

**Requisiti**:
- Testo normale: $C \ge 4.5:1$
- Testo large (≥18pt): $C \ge 3:1$

Esempio:
- Bianco su nero: $C = 21:1$ ✓
- Giallo su bianco: $C = 1.07:1$ ✗
- Giallo su nero: $C = 19.56:1$ ✓

### 4.1.3 Background Trasparenza

**Alpha blending**:

Colore risultante:

$$C_{\text{final}} = \alpha \cdot C_{\text{fg}} + (1 - \alpha) \cdot C_{\text{bg}}$$

dove $\alpha \in [0, 1]$ (da uint32 RGBA: $\alpha = \text{A} / 255$).

**Esempi**:

```json
{
  "background_color": 2147483903,
  "show_background": true
}
```

Decodifica `2147483903`:

$$\begin{aligned}
R &= (2147483903 >> 24) \& 0xFF = 127 \\
G &= (2147483903 >> 16) \& 0xFF = 255 \\
B &= (2147483903 >> 8) \& 0xFF = 255 \\
A &= 2147483903 \& 0xFF = 255
\end{aligned}$$

Colore: Ciano semi-trasparente

| Descrizione | RGBA | uint32 |
|-------------|------|--------|
| Nero opaco | (0,0,0,255) | 255 |
| Nero 50% | (0,0,0,128) | 128 |
| Bianco opaco | (255,255,255,255) | 4294967295 |
| Rosso 75% | (255,0,0,191) | 4278190271 |

---

## 4.2 Bordo

```json
{
  "border": {
    "enabled": true,
    "color": 4294967295,
    "width": 3.0,
    "margin": 10.0,
    "style": "solid"
  }
}
```

### 4.2.1 Geometria Bordo

**Rettangolo esterno**:

$$\begin{aligned}
x_1 &= m \\
y_1 &= m \\
x_2 &= w - m \\
y_2 &= h - m
\end{aligned}$$

dove $m$ = `margin` (pixel dal bordo immagine).

**Spessore**:

Linea di larghezza $b$ = `width`:

- Esterno: $(x_1 - b/2, y_1 - b/2)$ a $(x_2 + b/2, y_2 + b/2)$
- Interno: $(x_1 + b/2, y_1 + b/2)$ a $(x_2 - b/2, y_2 - b/2)$

### 4.2.2 Stili Linea

**Solid** (default):

Linea continua uniforme.

**Dashed** (futuro):

Segmenti alternati: $[d_{\text{on}}, d_{\text{off}}]$

Pattern dash: $[10, 5]$ = 10px on, 5px off

**Double** (futuro):

Due linee parallele separate da gap:

$$\text{gap} = \frac{b}{3}$$

Linee a distanza: $b_1 = b/3$, $b_2 = 2b/3$

### 4.2.3 Corner Styles

**Square** (default):

Angoli retti.

**Rounded** (futuro):

Angoli arrotondati con raggio $r = b$:

Arco di cerchio da $(x_1, y_1)$ a $(x_1, y_1 + r)$:

$$\begin{aligned}
x(t) &= x_1 + r(1 - \cos t) \\
y(t) &= y_1 + r \sin t
\end{aligned}$$

con $t \in [0, \pi/2]$.

---

## 4.3 Bussola Celeste

```json
{
  "compass": {
    "enabled": true,
    "position": 2,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "size": 100.0,
    "north_color": 4278190335,
    "east_color": 16711935,
    "line_color": 4294967295,
    "line_width": 2.5,
    "show_labels": true,
    "label_font_size": 12.0
  }
}
```

### 4.3.1 Geometria Bussola

**Design**: Croce con bracci N/E/S/W

Centro: $(x_c, y_c)$ (da `position` o `custom_x/y`)

Lunghezza bracci: $L = \text{size} / 2$

**Coordinate bracci** (orientamento standard: N=up, E=left):

$$\begin{aligned}
\text{Nord} &: (x_c, y_c) \to (x_c, y_c - L) \\
\text{Sud} &: (x_c, y_c) \to (x_c, y_c + L) \\
\text{Est} &: (x_c, y_c) \to (x_c - L, y_c) \\
\text{Ovest} &: (x_c, y_c) \to (x_c + L, y_c)
\end{aligned}$$

**Con rotazione** $\theta$ (da `orientation.rotation_angle`):

$$\begin{pmatrix} x' \\ y' \end{pmatrix} = \begin{pmatrix} \cos\theta & -\sin\theta \\ \sin\theta & \cos\theta \end{pmatrix} \begin{pmatrix} x - x_c \\ y - y_c \end{pmatrix} + \begin{pmatrix} x_c \\ y_c \end{pmatrix}$$

### 4.3.2 Rendering Direzionale

**Frecce direzionali**:

Punta Nord (triangolo):

$$\begin{aligned}
P_1 &= (x_c, y_c - L) \\
P_2 &= (x_c - w, y_c - L + h) \\
P_3 &= (x_c + w, y_c - L + h)
\end{aligned}$$

con $w = L/5$, $h = L/4$ (proporzioni freccia).

**Colori direzionali**:

Standard astronomico:
- Nord: Rosso (`0xFF0000FF`)
- Est: Verde (`0x00FF00FF`)
- Sud/Ovest: Bianco o grigio

**Etichette**:

Posizione testo:

$$\begin{aligned}
\text{N} &: (x_c, y_c - L - \text{font\_size}) \\
\text{E} &: (x_c - L - \text{font\_size}, y_c) \\
\text{S} &: (x_c, y_c + L + \text{font\_size}) \\
\text{W} &: (x_c + L + \text{font\_size}, y_c)
\end{aligned}$$

### 4.3.3 Adattamento Proiezione

Per proiezioni non conformi (es. Mercator), la bussola segue la distorsione locale:

**Gradiente proiezione**:

$$\mathbf{J} = \begin{pmatrix}
\frac{\partial x}{\partial \alpha} & \frac{\partial x}{\partial \delta} \\
\frac{\partial y}{\partial \alpha} & \frac{\partial y}{\partial \delta}
\end{pmatrix}$$

Direzione Nord proiettata:

$$\mathbf{v}_N = \mathbf{J} \begin{pmatrix} 0 \\ 1 \end{pmatrix} = \begin{pmatrix} \partial x / \partial \delta \\ \partial y / \partial \delta \end{pmatrix}$$

Angolo:

$$\theta_N = \arctan\left(\frac{v_{N,x}}{v_{N,y}}\right)$$

---

## 4.4 Scala Angolare

```json
{
  "scale": {
    "enabled": true,
    "position": 3,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "length": 120.0,
    "color": 4294967295,
    "line_width": 2.0,
    "font_size": 11.0,
    "show_text": true,
    "angular_size": 1.0
  }
}
```

### 4.4.1 Calcolo Lunghezza

**Scala immagine** (pixel per grado):

$$s = \frac{w_{\text{px}}}{w_{\text{FOV}}}$$

**Lunghezza scala** per $\theta$ gradi:

$$L_{\text{px}} = \theta \times s$$

Esempio:
- FOV: $10°$, immagine: $2400$ px
- Scala: $s = 2400/10 = 240$ px/°
- Scala $1°$: $L = 240$ px

**Dimensione angolare automatica**:

Se `length` specificato (pixel), calcola $\theta$:

$$\theta = \frac{L_{\text{px}}}{s} = \frac{L_{\text{px}} \times w_{\text{FOV}}}{w_{\text{px}}}$$

Arrotonda a valori "nice": $[0.1, 0.25, 0.5, 1, 2, 5, 10, 15, 30, 60]$ gradi.

### 4.4.2 Formato Testo

**Conversione gradi → arcmin/arcsec**:

$$\theta_{\text{deg}} = \begin{cases}
\theta & \theta \ge 1° \\
\theta \times 60 \, \text{arcmin} & 1' \le \theta < 60' \\
\theta \times 3600 \, \text{arcsec} & \theta < 1'
\end{cases}$$

Esempi:
- $5.0°$ → "5.0°"
- $0.5°$ → "30'"
- $0.0083°$ → "30''"

**Formato**:

```cpp
std::string formatAngularSize(double deg) {
    if (deg >= 1.0) {
        return fmt::format("{:.1f}°", deg);
    } else if (deg >= 1.0/60.0) {
        return fmt::format("{:.0f}'", deg * 60);
    } else {
        return fmt::format("{:.0f}\"", deg * 3600);
    }
}
```

### 4.4.3 Rendering

**Linea scala**:

Segmento orizzontale con tick alle estremità:

$$\begin{aligned}
\text{Linea} &: (x_s, y_s) \to (x_s + L, y_s) \\
\text{Tick sx} &: (x_s, y_s - h) \to (x_s, y_s + h) \\
\text{Tick dx} &: (x_s + L, y_s - h) \to (x_s + L, y_s + h)
\end{aligned}$$

con $h = 5$ px (altezza tick).

**Testo**:

Centrato sopra la linea:

$$\text{pos} = \left(x_s + \frac{L}{2} - \frac{w_{\text{text}}}{2}, y_s - h - \text{font\_size}\right)$$

---

## 4.5 Legenda Magnitudini

```json
{
  "magnitude_legend": {
    "enabled": true,
    "position": 4,
    "custom_x": 0.0,
    "custom_y": 0.0,
    "font_size": 10.0,
    "text_color": 4294967295,
    "background_color": 2130706687,
    "show_background": true,
    "padding": 10.0
  }
}
```

### 4.5.1 Layout Legenda

**Simboli magnitudine**:

Mostra scala da $m_{\min}$ a $m_{\max}$ con step = 2 mag:

$$m_i \in \{m_{\min}, m_{\min} + 2, m_{\min} + 4, \ldots, m_{\max}\}$$

**Esempio**: $m \in [0, 12]$ → simboli per $\{0, 2, 4, 6, 8, 10, 12\}$

**Dimensioni**:

Per ogni magnitudine $m_i$:

$$s_i = s_{\max} - (s_{\max} - s_{\min}) \times \frac{m_i - m_{\min}}{m_{\max} - m_{\min}}$$

Layout verticale:

$$y_i = y_0 + i \times (s_{\max} + \text{spacing})$$

con spacing = 8 px.

### 4.5.2 Rendering Simboli

**Cerchio + etichetta**:

Per ciascun $(m_i, s_i)$:

1. Disegna cerchio: centro $(x_0, y_i)$, raggio $s_i/2$
2. Disegna testo: $m_i$ a $(x_0 + s_{\max}/2 + 5, y_i)$

**Box totale**:

$$\begin{aligned}
w_{\text{box}} &= s_{\max} + 5 + w_{\text{text}} + 2 \cdot \text{padding} \\
h_{\text{box}} &= n \times (s_{\max} + \text{spacing}) + 2 \cdot \text{padding}
\end{aligned}$$

dove $n$ = numero simboli.

### 4.5.3 Esempio Completo

FOV $10° \times 10°$, $m \in [2, 12]$:

Simboli: $\{2, 4, 6, 8, 10, 12\}$ (6 simboli)

Dimensioni:
- $s(2) = 8.0$ px
- $s(4) = 6.4$ px
- $s(6) = 4.8$ px
- $s(8) = 3.2$ px
- $s(10) = 1.6$ px
- $s(12) = 0.5$ px

Layout:
- Spacing: 8 px
- Altezza totale: $6 \times (8.0 + 8) + 2 \times 10 = 116$ px

---

## 4.6 Esempi Combinati

### 4.6.1 Layout IAU Standard

```json
{
  "title": {
    "enabled": true,
    "text": "NGC 2237 (Rosette Nebula)",
    "position": 1,
    "font_size": 18.0,
    "show_background": false
  },
  
  "border": {
    "enabled": true,
    "color": 4278190080,
    "width": 2.0,
    "margin": 5.0
  },
  
  "compass": {
    "enabled": true,
    "position": 2,
    "size": 80.0,
    "north_color": 4278190335,
    "east_color": 16711935
  },
  
  "scale": {
    "enabled": true,
    "position": 3,
    "length": 100.0,
    "show_text": true
  },
  
  "magnitude_legend": {
    "enabled": true,
    "position": 4
  }
}
```

### 4.6.2 Stile Minimal

```json
{
  "title": {
    "enabled": true,
    "text": "M42 Orion Nebula",
    "position": 1,
    "font_size": 16.0,
    "text_color": 3435973887,
    "show_background": false
  },
  
  "border": {
    "enabled": false
  },
  
  "compass": {
    "enabled": true,
    "position": 2,
    "size": 60.0,
    "show_labels": false
  },
  
  "scale": {
    "enabled": true,
    "position": 3,
    "length": 80.0
  },
  
  "magnitude_legend": {
    "enabled": false
  }
}
```

### 4.6.3 Poster Style

```json
{
  "title": {
    "enabled": true,
    "text": "ORION CONSTELLATION",
    "position": 1,
    "font_size": 36.0,
    "text_color": 4294934528,
    "background_color": 2147483903,
    "show_background": true,
    "padding_x": 20.0,
    "padding_y": 15.0
  },
  
  "border": {
    "enabled": true,
    "color": 4294934528,
    "width": 5.0,
    "margin": 20.0
  },
  
  "compass": {
    "enabled": true,
    "position": 5,
    "custom_x": 0.95,
    "custom_y": 0.05,
    "size": 120.0
  },
  
  "scale": {
    "enabled": true,
    "position": 5,
    "custom_x": 0.05,
    "custom_y": 0.95,
    "length": 150.0,
    "font_size": 14.0
  }
}
```

---

[← Rendering](03_RENDERING.md) | [Indice](00_INDEX.md) | [Prossimo: Overlay →](05_OVERLAYS.md)
