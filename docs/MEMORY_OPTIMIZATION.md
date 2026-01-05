# Ottimizzazione Memoria - Gestione Stelle

## Problema

Quando si visualizzano campi celesti ampi o con magnitudine limite alta, il numero di stelle può diventare molto elevato (>50.000), causando:
- **Consumo eccessivo di RAM**: tutte le stelle vengono caricate in memoria
- **Rallentamenti nel rendering**: processamento di migliaia di oggetti
- **Possibili crash**: su sistemi con memoria limitata

## Soluzione Implementata

### 1. Limite Dinamico Automatico

Il sistema calcola automaticamente il numero massimo di stelle da caricare in base a:
- **Area della query** (radiusDegrees²)
- **Magnitudine limite** (densità stelle cresce 2.5x per ogni magnitudine)

```cpp
catalog::GaiaQueryParameters params;
params.center = center;
params.radiusDegrees = 5.0;
params.maxMagnitude = 12.0;

// Calcola automaticamente il limite ottimale (es. ~75.000 stelle)
params.calculateOptimalMaxResults();
```

**Formula**:
```
densità base = 1000 stelle/grado² (mag 12)
fattore magnitudine = 2.5^(mag - 12)
stelle stimate = π × raggio² × densità × fattore
limite = min(100000, max(1000, stelle_stimate))
```

### 2. Rendering in Batch

Le stelle vengono processate in batch (chunk) per ridurre il picco di memoria:

```cpp
map::MapConfiguration config;
config.maxStars = 50000;        // Limite massimo stelle
config.starBatchSize = 5000;    // Dimensione batch

// Il renderer processa automaticamente in batch se necessario
map::MapRenderer renderer(config);
auto image = renderer.render(stars);  // Rendering ottimizzato
```

**Vantaggi**:
- Memoria liberata progressivamente dopo ogni batch
- Miglior gestione della cache CPU
- Nessun impatto sulla qualità dell'immagine

### 3. Filtro Early Rejection

Le stelle vengono filtrate **prima** di essere caricate completamente:

```cpp
// In ChartGenerator
stars_.reserve(std::min(allStars.size(), 50000));

for (const auto& star : allStars) {
    // Filtro geometrico
    if (inFieldOfView(star)) {
        stars_.push_back(star);
    }
    
    // Hard limit per sicurezza
    if (stars_.size() >= 50000) {
        // Avvisa l'utente
        break;
    }
}
```

### 4. Configurazione JSON

I limiti possono essere configurati via JSON:

```json
{
  "limiting_magnitude": 13.0,
  "performance": {
    "max_stars": 50000,
    "star_batch_size": 5000
  }
}
```

## Esempi Pratici

### Esempio 1: Campo Ristretto (OK)
```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 5.0, "height": 5.0},
  "limiting_magnitude": 12.0
}
```
**Risultato**: ~15.000 stelle, nessun problema

### Esempio 2: Campo Ampio (ATTENZIONE)
```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 30.0, "height": 30.0},
  "limiting_magnitude": 14.0
}
```
**Risultato**: ~250.000 stelle stimate → **limitato a 50.000**

**Messaggio**:
```
ATTENZIONE: Raggiunto limite di 50000 stelle, troncamento necessario
Suggerimento: riduci limitingMagnitude o fieldOfView per vedere tutte le stelle
```

### Esempio 3: Ottimizzazione Personalizzata
```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 15.0, "height": 15.0},
  "limiting_magnitude": 13.0,
  "performance": {
    "max_stars": 75000,
    "star_batch_size": 10000
  }
}
```

## Raccomandazioni

### Per Prestazioni Ottimali

| Campo | Magnitudine | Stelle Tipiche | Memoria | Tempo Render |
|-------|-------------|----------------|---------|--------------|
| 5° × 5° | 10.0 | ~2.000 | ~1 MB | <100 ms |
| 10° × 10° | 11.0 | ~8.000 | ~4 MB | ~200 ms |
| 15° × 15° | 12.0 | ~25.000 | ~12 MB | ~500 ms |
| 20° × 20° | 13.0 | ~100.000 → **50k** | ~25 MB | ~1 s |

### Best Practices

✅ **Fare**:
- Usare `calculateOptimalMaxResults()` sempre
- Impostare `maxStars` in base alla RAM disponibile
- Ridurre `limitingMagnitude` per campi ampi
- Monitorare i messaggi di warning

❌ **Evitare**:
- Campi >20° con magnitude >12
- `maxStars` > 100.000 senza necessità
- Ignorare i warning di troncamento

## Monitoraggio

Il sistema fornisce feedback durante l'esecuzione:

```
Query Gaia con limite dinamico: 75000 stelle max
Query Gaia: 82341 stelle trovate (raggio 15.8°)
Stelle nel rettangolo: 64289 (dopo filtro geometrico)
ATTENZIONE: Raggiunto limite di 50000 stelle, troncamento necessario
Suggerimento: riduci limitingMagnitude o fieldOfView per vedere tutte le stelle
```

## Parametri di Default

```cpp
// GaiaQueryParameters (automatici)
maxResults = 50000  // aumentato da 10000

// MapConfiguration
maxStars = 50000
starBatchSize = 5000
```

## Implementazione Tecnica

### File Modificati

1. **include/starmap/catalog/GaiaClient.h**
   - `GaiaQueryParameters::calculateOptimalMaxResults()`
   
2. **include/starmap/map/MapConfiguration.h**
   - `maxStars` e `starBatchSize`
   
3. **src/map/MapRenderer.cpp**
   - `renderStarsBatched()` per rendering ottimizzato
   
4. **src/map/ChartGenerator.cpp**
   - Limite hard-coded a 50.000 con early rejection
   - Chiamata automatica a `calculateOptimalMaxResults()`
   
5. **src/config/JSONConfigLoader.cpp**
   - Supporto per sezione `performance` nel JSON

## Testing

```bash
cd build/examples

# Test campo normale (dovrebbe funzionare)
./generate_chart ../examples/config_examples/orion.json

# Test campo ampio (vedrà il limite)
# Crea un config con field_of_view 20x20 e magnitude 13
```

## Limitazioni Note

1. Il limite di 50.000 stelle è hard-coded in `ChartGenerator.cpp`
2. Il troncamento è basato sull'ordine di query, non su priorità
3. Non c'è selezione intelligente (es. stelle più luminose)

## Sviluppi Futuri

- [ ] Selezione prioritaria (stelle luminose/SAO prima)
- [ ] Adaptive LOD (Level of Detail) basato su zoom
- [ ] Streaming progressivo per rendering incrementale
- [ ] Compressione in-memory per stelle distanti
