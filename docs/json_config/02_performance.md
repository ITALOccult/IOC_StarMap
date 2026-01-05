# Performance e Orientamento

Gestione efficiente della memoria e controllo dell'orientamento della mappa.

## Performance

```json
{
  "performance": {
    "max_stars": 50000,
    "star_batch_size": 5000
  }
}
```

| Campo | Tipo | Default | Range | Descrizione |
|-------|------|---------|-------|-------------|
| `max_stars` | int | 50000 | 100-1M | Numero massimo di stelle da caricare |
| `star_batch_size` | int | 5000 | 100-50k | Dimensione batch per rendering |

### Problema della Memoria

Ogni stella occupa circa **68 bytes** in memoria:
- Coordinate RA/Dec: 16 bytes
- Magnitudine, colore B-V: 8 bytes
- Numero SAO: 4 bytes
- Nome: ~32 bytes (media)
- Overhead: ~8 bytes

**Memoria richiesta**:
```
Memoria_MB = (N_stelle × 68) / 1,000,000

Esempi:
- 10,000 stelle: ~0.7 MB
- 50,000 stelle: ~3.4 MB
- 100,000 stelle: ~6.8 MB
```

### Calcolo Automatico Limite

La libreria calcola automaticamente un limite ottimale:

```
N_optimal = min(Area × ρ(m) × 1000, max_stars)

dove:
- Area = π × r² (approssimazione campo circolare)
- ρ(m) = 2.5^(m-12) (densità magnitudine)
- max_stars = parametro configurato
```

**Esempi**:

| FOV | m_lim | Area (deg²) | ρ(m) | N_optimal |
|-----|-------|-------------|------|-----------|
| 10°×10° | 10 | 78.5 | 0.158 | 12,500 |
| 10°×10° | 12 | 78.5 | 1.0 | 78,500 → 50,000 (clamped) |
| 5°×5° | 11 | 19.6 | 0.398 | 7,800 |
| 20°×20° | 13 | 314 | 2.512 | 789,000 → 50,000 (clamped) |

### Batch Rendering

Invece di caricare tutte le stelle:

1. **Divide**: Query database in chunk da `star_batch_size`
2. **Process**: Rendering immediato del batch
3. **Release**: Dealloca memoria batch
4. **Repeat**: Prossimo batch fino a `max_stars`

**Benefici**:
- Memoria di picco ridotta
- Streaming da database
- Interruzione anticipata se limite raggiunto

**Trade-off Batch Size**:

| Size | Pro | Contro |
|------|-----|--------|
| 100-1000 | Memoria minima | Overhead query alto |
| **5000** | **Bilanciato** | **Ideale** |
| 50000 | Query minime | Picco memoria alto |

**Regola empirica**: `batch_size ≈ √max_stars`

### Early Rejection

Prima di proiettare ogni stella:

1. **Bounding box** rapido (RA/Dec rettangolare)
2. **Test angolare** se necessario (distanza sferica)
3. **Magnitude culling** (reject stelle più deboli di limite)

**Speedup**: 5-10× per campi piccoli

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

| Campo | Tipo | Default | Range | Descrizione |
|-------|------|---------|-------|-------------|
| `rotation_angle` | double | 0.0 | 0-360 | Rotazione in gradi (senso orario) |
| `north_up` | bool | true | - | Nord verso l'alto |
| `east_left` | bool | true | - | Est a sinistra (standard astronomico) |

### Convenzioni Standard

**Astronomia standard**:
- Nord = up (y positivo)
- Est = left (x negativo)
- Motivo: vista dall'interno della sfera celeste

```json
{
  "orientation": {
    "north_up": true,
    "east_left": true
  }
}
```

**Coordinate terrestri**:
- Nord = up
- Est = right
- Per ottenere:

```json
{
  "orientation": {
    "north_up": true,
    "east_left": false
  }
}
```

### Rotazione

Angolo in gradi, senso orario da Nord:

```json
{
  "orientation": {
    "rotation_angle": 45.0
  }
}
```

**Casi d'uso**:

- **0°**: Standard (Nord alto)
- **90°**: Est alto (rotazione quarter)
- **180°**: Sud alto (invertito)
- **270°**: Ovest alto

**Campo al telescopio**:

Immagini Newton/Cassegrain sono invertite. Usa `rotation_angle` per match esatto con vista oculare.

**Angolo parallattico**:

Per match con osservazione da terra a una certa ora:

```
q = arctan(sin(H) / (tan(φ)cos(δ) - sin(δ)cos(H)))

dove:
- H = ora angolare
- φ = latitudine
- δ = declinazione
```

Imposta `rotation_angle` = q

---

## Esempi Performance

### Preview Veloce

```json
{
  "limiting_magnitude": 9.0,
  "performance": {
    "max_stars": 5000,
    "star_batch_size": 1000
  }
}
```

**Risultato**: Rendering <0.5s, memoria ~0.4 MB

### Standard Qualità

```json
{
  "limiting_magnitude": 11.5,
  "performance": {
    "max_stars": 30000,
    "star_batch_size": 5000
  }
}
```

**Risultato**: Rendering ~1s, memoria ~2 MB

### Alta Risoluzione

```json
{
  "limiting_magnitude": 13.0,
  "performance": {
    "max_stars": 50000,
    "star_batch_size": 10000
  }
}
```

**Risultato**: Rendering ~3s, memoria ~3.4 MB

### Survey Completo

```json
{
  "limiting_magnitude": 15.0,
  "performance": {
    "max_stars": 100000,
    "star_batch_size": 20000
  }
}
```

**Risultato**: Rendering ~10s, memoria ~7 MB

**⚠️ Attenzione**: Per m>14 considera l'uso di spatial indexing

---

## Trade-off Performance

### Magnitudine vs Tempo

Incremento magnitudine di +1:
- Stelle: ×4 (legge di Pogson: 10^0.6 ≈ 4)
- Tempo rendering: ×3-4
- Memoria: ×4

### FOV vs Performance

Raddoppiando FOV:
- Area: ×4
- Stelle: ×4
- Tempo: ×4

### Immagine vs Rendering

Raddoppiando risoluzione:
- Pixel: ×4
- Tempo rendering stelle: costante
- Tempo salvataggio PNG: ×4

---

## Benchmark Tipici

**Setup**: MacBook Pro M1, 16GB RAM, SSD

| FOV | m_lim | Stelle | Query | Render | Totale |
|-----|-------|--------|-------|--------|--------|
| 10°×10° | 10 | 3,247 | 0.08s | 0.12s | 0.20s |
| 10°×10° | 11 | 6,521 | 0.15s | 0.24s | 0.39s |
| 10°×10° | 12 | 12,843 | 0.28s | 0.51s | 0.79s |
| 20°×20° | 11 | 24,185 | 0.52s | 0.89s | 1.41s |
| 10°×10° | 13 | 20,127 | 0.41s | 0.73s | 1.14s |

---

[← Configurazione Base](01_base.md) | [Indice](../JSON_CONFIGURATION.md) | [Prossimo: Griglia →](03_grid.md)
