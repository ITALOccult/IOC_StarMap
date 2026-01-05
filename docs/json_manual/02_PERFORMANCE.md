# Capitolo 2: Performance e Ottimizzazione

Gestione efficiente della memoria e ottimizzazione del rendering per cataloghi di grandi dimensioni.

## 2.1 Problema della Memoria

### 2.1.1 Dimensione Tipica per Stella

Ogni stella in memoria occupa:

```cpp
struct Star {
    double ra, dec;           // 16 bytes
    float magnitude;          // 4 bytes
    float bv_color;           // 4 bytes
    uint32_t sao_number;      // 4 bytes
    std::string name;         // ~32 bytes (media)
    // Allineamento + overhead: ~8 bytes
};  // Totale: ~68 bytes
```

**Memoria richiesta**:

$$M_{\text{MB}} = \frac{N_{\text{stelle}} \times 68}{10^6}$$

Esempi:
- 10,000 stelle: $\approx 0.68$ MB
- 50,000 stelle: $\approx 3.4$ MB
- 100,000 stelle: $\approx 6.8$ MB
- 1,000,000 stelle: $\approx 68$ MB

### 2.1.2 Overhead Rendering

Durante il rendering, per ogni stella si allocano:

```cpp
struct StarSymbol {
    float x, y;               // 8 bytes (coordinate schermo)
    float size;               // 4 bytes
    uint32_t color;           // 4 bytes
    // Totale: 16 bytes
};
```

**Memoria totale di picco**:

$$M_{\text{peak}} = N \times (68 + 16) = N \times 84 \, \text{bytes}$$

Per $N = 100000$: $M_{\text{peak}} \approx 8.4$ MB

---

## 2.2 Configurazione Performance

```json
{
  "performance": {
    "max_stars": 50000,
    "star_batch_size": 5000,
    "enable_spatial_index": true,
    "early_rejection": true
  }
}
```

### 2.2.1 Parametri

| Campo | Tipo | Default | Range | Descrizione |
|-------|------|---------|-------|-------------|
| `max_stars` | int | 50000 | $[100, 10^6]$ | Limite assoluto stelle |
| `star_batch_size` | int | 5000 | $[100, 50000]$ | Dimensione batch rendering |
| `enable_spatial_index` | bool | true | - | Usa R-tree per query spaziali |
| `early_rejection` | bool | true | - | Culling prima della proiezione |

### 2.2.2 Calcolo Automatico Limite

La libreria calcola automaticamente un limite ottimale basato su:

$$N_{\text{optimal}} = \min\left(A \times \rho(m) \times 1000, N_{\text{max}}\right)$$

dove:
- $A = \pi r^2$ = area campo in gradi² (approssimazione sferica)
- $\rho(m) = 2.5^{m - 12}$ = fattore densità magnitudine
- $N_{\text{max}}$ = `max_stars` configurato

**Implementazione C++**:

```cpp
int calculateOptimalMaxResults() const {
    // Area approssimata come cerchio
    double radius_deg = std::max(width_deg, height_deg) / 2.0;
    double area = M_PI * radius_deg * radius_deg;
    
    // Densità stelle
    double mag_factor = std::pow(2.5, limiting_mag - 12.0);
    
    // Calcolo con clamp
    int optimal = static_cast<int>(area * 1000.0 * mag_factor);
    return std::clamp(optimal, 1000, max_stars);
}
```

**Esempi**:

| FOV | $m_{\text{lim}}$ | $A$ (deg²) | $\rho(m)$ | $N_{\text{opt}}$ |
|-----|------------------|------------|-----------|------------------|
| $10° \times 10°$ | 10 | 78.5 | 0.158 | 12,500 |
| $10° \times 10°$ | 12 | 78.5 | 1.0 | 78,500 → 50,000 |
| $5° \times 5°$ | 11 | 19.6 | 0.398 | 7,800 |
| $20° \times 20°$ | 13 | 314 | 2.512 | 789,000 → 50,000 |

---

## 2.3 Batch Rendering

### 2.3.1 Strategia

Invece di caricare tutte le stelle in memoria:

1. **Divide**: Query database in chunk da `star_batch_size`
2. **Process**: Rendering immediato del batch
3. **Release**: Dealloca memoria batch
4. **Repeat**: Prossimo batch fino a `max_stars`

**Complessità memoria**:

$$M_{\text{batch}} = \min(N_{\text{totali}}, B) \times 84 \, \text{bytes}$$

dove $B$ = `star_batch_size`.

### 2.3.2 Pseudocodice

```cpp
void renderStarsBatched(const GaiaQueryParameters& params) {
    int stars_rendered = 0;
    int offset = 0;
    
    while (stars_rendered < params.max_stars) {
        int batch_size = std::min(
            params.star_batch_size,
            params.max_stars - stars_rendered
        );
        
        // Query batch dal database
        auto batch = catalog_->queryCone(
            params.ra, params.dec, params.radius,
            offset, batch_size
        );
        
        // Early exit se non ci sono più stelle
        if (batch.empty()) break;
        
        // Rendering batch
        for (const auto& star : batch) {
            if (isInsideField(star)) {
                renderStar(star);
            }
        }
        
        stars_rendered += batch.size();
        offset += batch_size;
    }
}
```

### 2.3.3 Trade-off Batch Size

| $B$ | Pro | Contro |
|-----|-----|--------|
| Piccolo (100-1000) | Memoria minima | Overhead query database |
| Medio (5000) | **Bilanciato** | - |
| Grande (50000) | Query database minime | Picco memoria alto |

**Regola empirica**: $B \approx \sqrt{N_{\text{max}}}$

---

## 2.4 Early Rejection

### 2.4.1 Culling Spaziale

Prima di proiettare ogni stella, verifica se è dentro il FOV:

**Test rapido** (coordinate sferiche):

$$\Delta\theta = \arccos[\sin(\delta_c)\sin(\delta_s) + \cos(\delta_c)\cos(\delta_s)\cos(\alpha_s - \alpha_c)]$$

Se $\Delta\theta > r_{\text{FOV}}$ → reject

dove:
- $(\alpha_c, \delta_c)$ = centro campo
- $(\alpha_s, \delta_s)$ = stella
- $r_{\text{FOV}} = \sqrt{w^2 + h^2} / 2$ = raggio campo

**Ottimizzazione**: Usa bounding box prima del test angolare:

```cpp
bool isInsideBoundingBox(double ra, double dec) const {
    double ra_min = center_ra - fov_width / 2.0;
    double ra_max = center_ra + fov_width / 2.0;
    double dec_min = center_dec - fov_height / 2.0;
    double dec_max = center_dec + fov_height / 2.0;
    
    // Gestione wrap RA a 0°/360°
    if (ra_min < 0) {
        return (ra >= ra_min + 360 || ra <= ra_max) &&
               (dec >= dec_min && dec <= dec_max);
    }
    if (ra_max > 360) {
        return (ra >= ra_min || ra <= ra_max - 360) &&
               (dec >= dec_min && dec <= dec_max);
    }
    
    return (ra >= ra_min && ra <= ra_max) &&
           (dec >= dec_min && dec <= dec_max);
}
```

**Speedup**: $\approx 5-10\times$ per campi piccoli (<10°)

### 2.4.2 Magnitude Culling

Reject stelle più deboli del limite prima di proiettare:

```cpp
if (star.magnitude > limiting_magnitude + 0.5) {
    continue;  // Margin 0.5 per errori fotometrici
}
```

### 2.4.3 Statistiche Rejection

Frazione stelle rejettate:

$$f_{\text{reject}} \approx 1 - \frac{A_{\text{FOV}}}{A_{\text{query}}}$$

Per query circolare con $r_{\text{query}} = 1.2 \times r_{\text{FOV}}$:

$$f_{\text{reject}} \approx 1 - \frac{1}{1.44} \approx 0.31 \quad (31\%)$$

---

## 2.5 Spatial Indexing

### 2.5.1 R-Tree per Query Spaziali

Invece di scansione lineare, usa R-tree (Guttman 1984):

**Complessità**:
- Costruzione: $O(N \log N)$
- Query range: $O(\log N + k)$ dove $k$ = risultati

**Implementazione** (via Boost.Geometry):

```cpp
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// Punto 2D con payload
typedef std::pair<bg::model::point<double, 2, bg::cs::spherical_equatorial<bg::degree>>, 
                  StarData> SpatialStar;

// R-tree con 16 elementi per nodo
typedef bgi::rtree<SpatialStar, bgi::quadratic<16>> SpatialIndex;

// Query box
bg::model::box<point_type> query_box(
    point_type(ra_min, dec_min),
    point_type(ra_max, dec_max)
);

std::vector<SpatialStar> results;
spatial_index.query(bgi::within(query_box), std::back_inserter(results));
```

**Speedup per query multiple**: $10-100\times$

### 2.5.2 HEALPix Binning (Opzionale)

Per campi molto grandi, pre-organizza stelle in celle HEALPix:

**Parametro NSIDE**:

$$N_{\text{pixel}} = 12 \times N_{\text{SIDE}}^2$$

Risoluzione angolare:

$$\theta_{\text{pixel}} \approx \sqrt{\frac{4\pi}{12 N_{\text{SIDE}}^2}} \approx \frac{60°}{N_{\text{SIDE}}}$$

Esempio: $N_{\text{SIDE}} = 64$ → $\theta \approx 0.94°$, $N_{\text{pixel}} = 49152$

**Uso**:
```cpp
int pixel_id = healpix::ang2pix_ring(NSIDE, theta, phi);
// Query solo pixel che intersecano FOV
```

---

## 2.6 Trade-off Performance

### 2.6.1 Magnitudine vs Stelle

Relazione numero stelle-magnitudine limite:

$$\log_{10}(N) = 0.6m + C$$

dove $C \approx -3$ per tutto il cielo.

**Incremento $\Delta m = +1$**:

$$\frac{N_{m+1}}{N_m} = 10^{0.6} \approx 3.98 \quad (\sim 4\times)$$

| $m$ | $N$ (FOV 10°×10°) | Memoria | Tempo render |
|-----|-------------------|---------|--------------|
| 8 | ~800 | 0.05 MB | <0.1s |
| 10 | ~3,000 | 0.2 MB | ~0.3s |
| 12 | ~12,000 | 0.8 MB | ~1s |
| 13 | ~20,000 | 1.4 MB | ~2s |
| 14 | ~50,000 | 3.4 MB | ~5s |

### 2.6.2 FOV vs Performance

**Area vs dimensione**:

$$A \propto w \times h$$

Raddoppiando FOV: $N_{\text{stelle}} \times 4$

| FOV | Stelle ($m=11$) | Rendering |
|-----|-----------------|-----------|
| 5°×5° | ~1,500 | Veloce |
| 10°×10° | ~6,000 | Medio |
| 20°×20° | ~24,000 | Lento |
| 30°×30° | ~54,000 | Molto lento |

### 2.6.3 Raccomandazioni Pratiche

**Configuration presets**:

```json
{
  "presets": {
    "quick_preview": {
      "limiting_magnitude": 10.0,
      "max_stars": 10000,
      "star_batch_size": 5000
    },
    "standard": {
      "limiting_magnitude": 11.5,
      "max_stars": 30000,
      "star_batch_size": 5000
    },
    "high_detail": {
      "limiting_magnitude": 13.0,
      "max_stars": 50000,
      "star_batch_size": 10000
    },
    "survey": {
      "limiting_magnitude": 15.0,
      "max_stars": 100000,
      "star_batch_size": 20000,
      "enable_spatial_index": true
    }
  }
}
```

---

## 2.7 Benchmark e Profiling

### 2.7.1 Test Case Standard

**Setup**: MacBook Pro M1, 16GB RAM, SSD NVMe

| Config | Stelle | Query DB | Rendering | Totale |
|--------|--------|----------|-----------|--------|
| 10°×10°, m=10 | 3,247 | 0.08s | 0.12s | **0.20s** |
| 10°×10°, m=11 | 6,521 | 0.15s | 0.24s | **0.39s** |
| 10°×10°, m=12 | 12,843 | 0.28s | 0.51s | **0.79s** |
| 20°×20°, m=11 | 24,185 | 0.52s | 0.89s | **1.41s** |
| 10°×10°, m=13 | 20,127 | 0.41s | 0.73s | **1.14s** |

### 2.7.2 Bottleneck Analysis

**Profilo tipico**:
- Query database: 30-40%
- Proiezione coordinate: 20-25%
- Rendering simboli: 25-30%
- Overlay/labels: 10-15%
- I/O (PNG save): 5-10%

**Ottimizzazioni applicate**:
1. ✅ Database indexing (B-tree su RA/Dec/Mag)
2. ✅ Batch rendering
3. ✅ Early rejection
4. ✅ SIMD per proiezioni (dove disponibile)
5. ⚠️ Parallel rendering (futuro: OpenMP)

---

## 2.8 Debugging Performance

### 2.8.1 Log Verbosity

```json
{
  "performance": {
    "enable_profiling": true,
    "log_level": "debug"
  }
}
```

Output tipico:

```
[INFO] Loading configuration from config.json
[DEBUG] FOV: 10.0° × 10.0°, limiting_mag: 11.5
[DEBUG] Calculated optimal max_stars: 32,147 (clamped to 30,000)
[INFO] Querying Gaia catalog: RA=83.75°, Dec=5.0°, radius=7.07°
[DEBUG] Database query returned 32,451 stars in 0.152s
[DEBUG] Early rejection: 2,451 stars outside FOV
[DEBUG] Rendering 30,000 stars in 6 batches of 5,000
[DEBUG] Batch 1/6 rendered in 0.081s
[DEBUG] Batch 2/6 rendered in 0.079s
...
[INFO] Total rendering time: 0.521s
[INFO] Saved output to map.png (2400×2400, 1.2 MB)
```

### 2.8.2 Statistiche Runtime

La libreria espone metriche:

```cpp
struct RenderStats {
    int stars_queried;
    int stars_rejected;
    int stars_rendered;
    double query_time_ms;
    double projection_time_ms;
    double render_time_ms;
    double total_time_ms;
    size_t peak_memory_mb;
};
```

---

## Esempio Completo Performance

```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 10.0, "height": 10.0},
  "image": {"width": 2400, "height": 2400},
  "limiting_magnitude": 12.0,
  
  "performance": {
    "max_stars": 50000,
    "star_batch_size": 5000,
    "enable_spatial_index": true,
    "early_rejection": true,
    "enable_profiling": true,
    "log_level": "info"
  }
}
```

**Risultato atteso**:
- ~15,000 stelle renderizzate
- Tempo totale: ~0.8s
- Memoria picco: ~2.5 MB
- Output: 2400×2400 PNG (~1.5 MB)

---

[← Configurazione Base](01_BASE_CONFIG.md) | [Indice](00_INDEX.md) | [Prossimo: Rendering →](03_RENDERING.md)
