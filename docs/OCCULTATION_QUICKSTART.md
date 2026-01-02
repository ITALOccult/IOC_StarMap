# Occultazioni Asteroidali - Guida Rapida

## Cosa Sono le Occultazioni Asteroidali?

Un'occultazione asteroidale si verifica quando un asteroide passa davanti a una stella, bloccandone temporaneamente la luce. L'osservazione di questi eventi permette di:

- Determinare forma e dimensioni dell'asteroide
- Scoprire eventuali satelliti
- Migliorare l'orbita dell'asteroide
- Misurare il diametro con precisione

## Tipi di Carte

### 1. Carta di Avvicinamento (Approach Chart)
- **Campo**: 10° × 10°
- **Uso**: Localizzare la regione con cercatore o binocolo
- **Stelle**: fino a mag 12

### 2. Carta Dettagliata (Detail Chart)
- **Campo**: 3° × 1.5° (default)
- **Uso**: Osservazione al telescopio
- **Stelle**: fino a mag 15

### 3. Carta Cercatrice (Finder Chart)
- **Campo**: 5° × 5°
- **Uso**: Via di mezzo tra approccio e dettaglio
- **Stelle**: fino a mag 13

## Quick Start

### Uso Semplice

```bash
# Compila il progetto
cd build
cmake ..
make occultation_chart

# Genera carte da file JSON
./examples/occultation_chart ../examples/occultation_examples/example_eros.json
```

Output automatico:
- `occultation_approach.png` - Carta di avvicinamento
- `occultation_detail.png` - Carta dettagliata
- `occultation_custom.png` - Carta personalizzata

### Uso da Codice

```cpp
#include "starmap/occultation/OccultationChartBuilder.h"

using namespace starmap::occultation;

int main() {
    OccultationChartBuilder builder;
    
    // Carica dati
    builder.fromJSONFile("event.json");
    
    // Genera carte
    builder.generateAndSaveApproachChart("approach.png");
    builder.generateAndSaveDetailChart("detail.png");
    
    return 0;
}
```

## Formato JSON Input

Esempio minimo:

```json
{
  "event_id": "2025-12-15-433-Eros",
  "source": "IOC",
  
  "target_star": {
    "catalog_id": "Gaia DR3 1234567890123456",
    "ra": 152.5678,
    "dec": 23.4567,
    "magnitude": 11.2
  },
  
  "asteroid": {
    "name": "(433) Eros",
    "number": 433,
    "magnitude": 12.5,
    "diameter_km": 16.8,
    "velocity_arcsec_per_hour": 45.2,
    "position_angle": 135.0
  },
  
  "circumstances": {
    "event_time": "2025-12-15T22:34:12.5Z",
    "duration_seconds": 8.3,
    "magnitude_drop": 0.8
  }
}
```

## Campi JSON Richiesti

### Obbligatori
- `event_id`: identificativo univoco
- `target_star.ra`: ascensione retta stella (gradi)
- `target_star.dec`: declinazione stella (gradi)
- `target_star.magnitude`: magnitudine stella
- `asteroid.name`: nome asteroide
- `asteroid.velocity_arcsec_per_hour`: velocità apparente
- `asteroid.position_angle`: direzione del moto (gradi)
- `circumstances.event_time`: data/ora evento (ISO 8601)
- `circumstances.duration_seconds`: durata evento

### Opzionali (ma utili)
- `target_star.catalog_id`: ID catalogo (Gaia, SAO, etc.)
- `target_star.sao_number`: numero SAO
- `asteroid.number`: numero MPC
- `asteroid.diameter_km`: diametro stimato
- `circumstances.magnitude_drop`: drop di magnitudine
- `circumstances.shadow_width_km`: larghezza ombra
- `observation_site.*`: dati sito osservazione

## Personalizzazione Carte

### Campo Visivo Custom

```cpp
OccultationChartConfig config;
config.fieldOfViewWidth = 5.0;   // gradi
config.fieldOfViewHeight = 3.0;  // gradi
config.limitingMagnitude = 14.0;

builder.generateCustomChart(config);
```

### Risoluzione Alta

```cpp
config.imageWidth = 4096;   // 4K
config.imageHeight = 2304;
config.quality = 100;       // qualità massima

builder.generateCustomChart(config);
```

### Traccia Asteroide Estesa

```cpp
config.pathDurationHours = 3.0;  // ±3 ore dall'evento
config.pathSteps = 120;          // più punti = traccia più liscia

builder.generateCustomChart(config);
```

## Workflow Tipico

1. **Ricevi dati occultazione** (da IOTA, Occult Watcher, etc.)
2. **Converti in formato JSON** (se necessario)
3. **Genera carte**:
   ```bash
   ./occultation_chart event.json
   ```
4. **Stampa carta dettagliata** per uso al telescopio
5. **Porta carta di avvicinamento** per localizzazione
6. **Osserva e registra** tempi di immersione/emersione

## Tips per l'Osservazione

### Prima dell'Evento
- [ ] Verifica coordinate stella target
- [ ] Pianifica sequenza di puntamento
- [ ] Verifica magnitudine stella e drop atteso
- [ ] Prepara registrazione tempi (video/DSLR)
- [ ] Sincronizza orologio con GPS/NTP

### Durante l'Evento
- [ ] Localizza stella con carta di avvicinamento
- [ ] Identifica stella con carta dettagliata
- [ ] Inizia registrazione 5-10 min prima
- [ ] Registra fino a 5-10 min dopo

### Dopo l'Evento
- [ ] Analizza dati
- [ ] Invia report a IOTA
- [ ] Condividi risultati

## Fonti Dati Occultazioni

### Online
- **IOTA**: https://occultations.org/
- **Occult Watcher**: Software per predizioni
- **Minor Planet Center**: https://minorplanetcenter.net/
- **IOTA-ES**: https://www.iota-es.de/

### Formati Supportati
- IOC (International Occultation Centre)
- IOTA standard
- Custom JSON

## Troubleshooting

### "Stelle non trovate"
```cpp
// Aumenta magnitudine limite
config.limitingMagnitude = 16.0;

// Aumenta raggio ricerca
config.searchRadiusFactor = 1.5;
```

### "Campo troppo piccolo"
```cpp
// Per stelle deboli, usa campo più largo
if (event.targetStar.magnitude > 10.0) {
    config.fieldOfViewWidth = 5.0;
    config.fieldOfViewHeight = 3.0;
}
```

### "Traccia asteroide non visibile"
```cpp
// Aumenta durata traccia
config.pathDurationHours = 3.0;

// Colore più visibile
config.asteroidPathColor = 0xFF0000FF;  // Rosso puro
```

## Esempi Inclusi

### example_eros.json
Occultazione di (433) Eros, asteroide Near-Earth ben conosciuto.

**Caratteristiche**:
- Stella target: mag 11.2
- Durata: 8.3 secondi
- Drop: 0.8 magnitudini
- Località: Italia centrale

## Integrazione con Altri Tool

### Da Occult Watcher
Occult Watcher può esportare previsioni. Converti in JSON con script:

```python
# occult_to_json.py
import json

# Leggi output Occult Watcher
# Converti in formato IOC JSON
# Salva file JSON
```

### Da IOTA Reports
I report IOTA contengono tutti i dati necessari:

```bash
# Scarica dati
curl https://occultations.org/api/event/2025-12-15 > event.json

# Genera carte
./occultation_chart event.json
```

## Formati Output

### PNG (Default)
Ideale per stampa e visualizzazione:
```cpp
builder.saveChart(buffer, "chart.png", "png");
```

### SVG (Vettoriale)
Ideale per editing e ridimensionamento:
```cpp
builder.saveChart(buffer, "chart.svg", "svg");
```

### PDF (Stampa)
Ideale per report e condivisione:
```cpp
builder.saveChart(buffer, "chart.pdf", "pdf");
```

## Supporto

Per domande o problemi:
- GitHub Issues: https://github.com/manvalan/IOC_StarMap/issues
- Email: [maintainer email]
- IOTA Forum: https://occultations.org/forum/

## Risorse Aggiuntive

- **Documentazione API completa**: `docs/OCCULTATION_API.md`
- **Esempi codice**: `examples/occultation_chart.cpp`
- **Dati test**: `examples/occultation_examples/`
- **Tutorial video**: [link YouTube]

## Contributi

Contributi benvenuti! Vedi `CONTRIBUTING.md` per linee guida.

### Idee per Contributi
- [ ] Parser per altri formati dati
- [ ] Integrazione con Occult Watcher
- [ ] Export verso planetari (Stellarium, etc.)
- [ ] Calcolo automatico traccia da elementi orbitali
- [ ] Visualizzazione 3D
- [ ] App mobile companion

## Licenza

Vedi file `LICENSE` nel repository principale.

---

**International Occultation Timing Association (IOTA)**  
*Osservare l'universo, un'occultazione alla volta*
