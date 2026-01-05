# Capitolo 1: Configurazione Base

Parametri fondamentali per definire il campo visivo e le proprietà geometriche della mappa.

## 1.1 Centro e Campo Visivo

### 1.1.1 Coordinate del Centro

```json
{
  "center": {
    "ra": 83.75,
    "dec": 5.0
  }
}
```

| Campo | Tipo | Range | Unità | Descrizione |
|-------|------|-------|-------|-------------|
| `ra` | double | $[0, 360)$ | gradi | Ascensione Retta (J2000) |
| `dec` | double | $[-90, +90]$ | gradi | Declinazione (J2000) |

**Conversione RA da ore a gradi**:

$$\text{RA}_{\text{gradi}} = \text{RA}_{\text{ore}} \times 15$$

Esempio: $5^h 35^m 17.3^s = (5 + 35/60 + 17.3/3600) \times 15 = 83.822°$

**Conversione sessagesimale-decimale**:

$$\text{Dec}_{\text{decimale}} = d + \frac{m}{60} + \frac{s}{3600}$$

Con segno: $-12° 30' 45'' = -(12 + 30/60 + 45/3600) = -12.5125°$

### 1.1.2 Campo Visivo

```json
{
  "field_of_view": {
    "width": 10.0,
    "height": 10.0
  }
}
```

| Campo | Tipo | Range | Unità | Descrizione |
|-------|------|-------|-------|-------------|
| `width` | double | $(0, 180]$ | gradi | Larghezza campo |
| `height` | double | $(0, 180]$ | gradi | Altezza campo |

**Area coperta** (approssimazione per campi piccoli):

$$A_{\text{deg}^2} \approx w \times h \times \cos(\delta)$$

dove $\delta$ è la declinazione del centro.

**Numero stelle attese** (relazione empirica):

$$N_{\text{stelle}} \approx A_{\text{deg}^2} \times 10^{0.6(m_{\text{lim}} - 6)}$$

### 1.1.3 Esempi Tipici

| Tipo Carta | FOV Width | FOV Height | Uso |
|------------|-----------|------------|-----|
| Wide field | 30-60° | 30-60° | Costellazioni intere |
| Standard | 10-20° | 10-20° | Finder chart |
| Detail | 2-5° | 2-5° | Campi telescopio |
| CCD | 0.5-2° | 0.5-2° | Campo CCD/CMOS |

---

## 1.2 Dimensioni Immagine

```json
{
  "image": {
    "width": 2400,
    "height": 2400
  }
}
```

### 1.2.1 Risoluzione Angolare

**Scala immagine** (pixel per grado):

$$\text{scale}_x = \frac{w_{\text{px}}}{w_{\text{deg}}}, \quad \text{scale}_y = \frac{h_{\text{px}}}{h_{\text{deg}}}$$

**Risoluzione angolare** (arcosecondi per pixel):

$$\text{res} = \frac{3600}{\text{scale}} = \frac{3600 \times \text{FOV}_{\text{deg}}}{\text{size}_{\text{px}}}$$

### 1.2.2 Raccomandazioni

**Per stampa**:

$$\text{DPI}_{\text{print}} = \frac{\text{size}_{\text{px}}}{\text{size}_{\text{inch}}}$$

| Formato | Dimensioni | DPI minimo | Pixel raccomandati |
|---------|------------|------------|--------------------|
| A4 | 8.3×11.7" | 300 | 2480×3508 |
| A3 | 11.7×16.5" | 300 | 3508×4961 |
| Poster | 24×36" | 150 | 3600×5400 |

**Per web/display**:

| Uso | Dimensioni | Note |
|-----|------------|------|
| Anteprima | 800×600 | Preview veloce |
| Full HD | 1920×1080 | Display standard |
| 4K | 3840×2160 | High-resolution |

**Rapporto FOV/Pixel**:

Per stelle risolte singolarmente: $\text{res} < 10''$ (arcosecondi/pixel)

Esempio: Campo $10° \times 10°$ richiede almeno $3600 \times 10 / 10 = 3600$ pixel per lato.

---

## 1.3 Sistemi di Proiezione

```json
{
  "projection": {
    "type": "stereographic",
    "coordinate_system": "equatorial"
  }
}
```

### 1.3.1 Tipi di Proiezione

#### Stereographic

**Equazione**:

$$x = \frac{2R \cos(\delta) \sin(\alpha - \alpha_0)}{1 + \sin(\delta_0)\sin(\delta) + \cos(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0)}$$

$$y = \frac{2R (\cos(\delta_0)\sin(\delta) - \sin(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0))}{1 + \sin(\delta_0)\sin(\delta) + \cos(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0)}$$

**Proprietà**:
- Conforme (preserva angoli)
- Cerchi celesti → cerchi nell'immagine
- Distorsione moderata fino a 60°
- **Uso**: Carte stellari standard, finder chart

#### Gnomonic

**Equazione**:

$$x = R \frac{\cos(\delta)\sin(\alpha - \alpha_0)}{\sin(\delta_0)\sin(\delta) + \cos(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0)}$$

$$y = R \frac{\cos(\delta_0)\sin(\delta) - \sin(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0)}{\sin(\delta_0)\sin(\delta) + \cos(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0)}$$

**Proprietà**:
- Cerchi massimi → linee rette
- Forte distorsione radiale
- FOV limitato (<90°)
- **Uso**: Pianificazione percorsi, astrometria

#### Orthographic

**Equazione**:

$$x = R \cos(\delta)\sin(\alpha - \alpha_0)$$

$$y = R (\cos(\delta_0)\sin(\delta) - \sin(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0))$$

**Proprietà**:
- Vista dall'infinito (sfera 3D proiettata)
- Non conforme
- FOV ≤ 180°
- **Uso**: Visualizzazione emisferi, educazione

#### Mercator

**Equazione**:

$$x = R(\alpha - \alpha_0)$$

$$y = R \ln\left[\tan\left(\frac{\pi}{4} + \frac{\delta}{2}\right)\right]$$

**Proprietà**:
- Conforme
- Meridiani e paralleli perpendicolari
- Forte distorsione ai poli
- **Uso**: Mappe globali, survey all-sky

#### Azimuthal Equidistant

**Equazione**:

$$\rho = R \cdot c, \quad \theta = \arctan\left(\frac{\cos(\delta)\sin(\alpha - \alpha_0)}{\cos(\delta_0)\sin(\delta) - \sin(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0)}\right)$$

dove:

$$c = \arccos[\sin(\delta_0)\sin(\delta) + \cos(\delta_0)\cos(\delta)\cos(\alpha - \alpha_0)]$$

$$x = \rho \sin(\theta), \quad y = \rho \cos(\theta)$$

**Proprietà**:
- Distanze dal centro preservate
- Non conforme
- **Uso**: Coordinate horizon (Alt/Az)

### 1.3.2 Sistemi di Coordinate

```json
{
  "projection": {
    "coordinate_system": "equatorial"
  }
}
```

#### Equatorial (RA/Dec)

Sistema standard J2000.0:
- Origine: Punto vernale (First Point of Aries)
- Asse X: verso $\gamma$ (equinozio)
- Asse Z: polo nord celeste
- Coordinate: $(\alpha, \delta)$ con $\alpha \in [0°, 360°)$, $\delta \in [-90°, +90°]$

#### Galactic (l/b)

Conversione Equatorial → Galactic:

$$\cos(b)\cos(l - 33°) = \cos(\delta)\cos(\alpha - 282.25°)$$
$$\cos(b)\sin(l - 33°) = \cos(\delta)\sin(\alpha - 282.25°)\sin(62.6°) + \sin(\delta)\cos(62.6°)$$
$$\sin(b) = \sin(\delta)\sin(62.6°) - \cos(\delta)\sin(\alpha - 282.25°)\cos(62.6°)$$

**Uso**: Studio struttura galattica

#### Ecliptic (λ/β)

Conversione Equatorial → Ecliptic ($\epsilon = 23.44°$ obliquità):

$$\tan(\lambda) = \frac{\sin(\alpha)\cos(\epsilon) + \tan(\delta)\sin(\epsilon)}{\cos(\alpha)}$$

$$\sin(\beta) = \sin(\delta)\cos(\epsilon) - \cos(\delta)\sin(\epsilon)\sin(\alpha)$$

**Uso**: Sistema solare, pianeti

#### Horizontal (Alt/Az)

Richiede posizione osservatore e tempo. Conversione Equatorial → Horizontal:

$$H = \text{LST} - \alpha \quad \text{(ora angolare)}$$

$$\sin(\text{Alt}) = \sin(\phi)\sin(\delta) + \cos(\phi)\cos(\delta)\cos(H)$$

$$\cos(\text{Az}) = \frac{\sin(\delta) - \sin(\text{Alt})\sin(\phi)}{\cos(\text{Alt})\cos(\phi)}$$

dove $\phi$ è la latitudine.

**Uso**: Osservazione locale, pianificazione

---

## 1.4 Magnitudine Limite

```json
{
  "limiting_magnitude": 12.0
}
```

### 1.4.1 Relazione Magnitudine-Flusso

Definizione classica:

$$m_1 - m_2 = -2.5 \log_{10}\left(\frac{F_1}{F_2}\right)$$

**Numero stelle cumulative** (legge di Schuster):

$$\log_{10}(N) \approx 0.6m - 3.2$$

Per $m = 12$: $N \approx 10^{4.0} = 10000$ stelle per steradiante.

**Conversione steradiante → gradi quadrati**:

$$1 \, \text{sr} = \left(\frac{180}{\pi}\right)^2 \approx 3283 \, \text{deg}^2$$

Quindi:

$$N_{\text{deg}^2} \approx \frac{10^{0.6m - 3.2}}{3283}$$

### 1.4.2 Raccomandazioni

| Tipo | $m_{\text{lim}}$ | Stelle/deg² | Uso |
|------|------------------|-------------|-----|
| Naked eye | 6-7 | ~1 | Carta osservativa |
| Binocolo | 8-10 | 5-50 | Finder chart |
| Telescopio | 11-13 | 100-1000 | Astrometria |
| Survey | 14-16 | 2000-20000 | Ricerca scientifica |

**Attenzione**: Per $m > 13$ considerare l'uso di `max_stars` per evitare overflow di memoria.

---

## 1.5 Orientamento

```json
{
  "orientation": {
    "rotation_angle": 0.0,
    "north_up": true,
    "east_left": true
  }
}
```

### 1.5.1 Angolo di Rotazione

Rotazione in senso orario (convenzione astronomica):

$$\begin{pmatrix} x' \\ y' \end{pmatrix} = \begin{pmatrix} \cos(\theta) & \sin(\theta) \\ -\sin(\theta) & \cos(\theta) \end{pmatrix} \begin{pmatrix} x \\ y \end{pmatrix}$$

Angolo $\theta$ in gradi: $\theta \in [0°, 360°)$

### 1.5.2 Convenzioni Standard

**Astronomia standard**:
- Nord = up ($y$ positivo)
- Est = left ($x$ negativo)
- Motivo: vista dall'interno della sfera celeste

**Coordinate terrestri**:
- Nord = up
- Est = right
- Per ottenere: `"east_left": false`

**Campo al telescopio** (Newton/Cassegrain):
- Immagine invertita
- Rotazione dipende da montatura
- Usare `rotation_angle` per match esatto

### 1.5.3 Angolo Parallattico

Per osservazioni da terra:

$$q = \arctan\left(\frac{\sin(H)}{\tan(\phi)\cos(\delta) - \sin(\delta)\cos(H)}\right)$$

dove $H$ è l'ora angolare, $\phi$ la latitudine, $\delta$ la declinazione.

Per match con vista telescopio: `"rotation_angle": q`

---

## Esempio Completo Base

```json
{
  "center": {
    "ra": 83.822,
    "dec": -5.391
  },
  "field_of_view": {
    "width": 15.0,
    "height": 15.0
  },
  "image": {
    "width": 3000,
    "height": 3000
  },
  "projection": {
    "type": "stereographic",
    "coordinate_system": "equatorial"
  },
  "limiting_magnitude": 11.0,
  "orientation": {
    "rotation_angle": 0.0,
    "north_up": true,
    "east_left": true
  },
  "background_color": 255
}
```

**Risultato**:
- Risoluzione: $15° / 3000\text{px} = 18''$ per pixel
- Stelle attese: $\approx 225 \times 10^{0.6 \times 5} = 225 \times 10^3 \approx 22500$ stelle
- Orientamento: Nord alto, Est sinistra (standard)

---

[← Indice](00_INDEX.md) | [Prossimo: Performance →](02_PERFORMANCE.md)
