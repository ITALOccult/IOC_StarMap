# API per Carte di Occultazione Asteroidale

## Panoramica

Le API per le occultazioni asteroidali permettono di generare automaticamente carte celesti specifiche per l'osservazione di eventi di occultazione. Il sistema supporta tre tipi principali di carte:

1. **Carta di Avvicinamento** (Campo largo, 10° × 10°)
2. **Carta Dettagliata** (Campo stretto, 3° × 1.5° default)
3. **Carta Cercatrice** (Campo medio, 5° × 5°)

## Struttura Dati

### OccultationEvent

Struttura completa che contiene tutti i dati di un evento di occultazione:

```cpp
struct OccultationEvent {
    std::string eventId;              // ID univoco evento
    std::string source;               // Fonte dati (IOC, IOTA, etc.)
    TargetStar targetStar;           // Stella da occultare
    AsteroidData asteroid;           // Asteroide occultante
    OccultationCircumstances circumstances;  // Circostanze evento
    std::optional<ShadowPath> shadowPath;    // Traccia ombra
    std::optional<ObservationSite> observationSite;  // Sito osservazione
};
```

### Formato JSON Input

Il sistema accetta dati in formato JSON strutturato:

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
    "velocity_arcsec_per_hour": 45.2
  },
  
  "circumstances": {
    "event_time": "2025-12-15T22:34:12.5Z",
    "duration_seconds": 8.3,
    "shadow_width_km": 16.8,
    "magnitude_drop": 0.8
  }
}
```

## Utilizzo Base

### 1. Creazione del Builder

```cpp
#include "starmap/occultation/OccultationChartBuilder.h"

using namespace starmap::occultation;

OccultationChartBuilder builder;
```

### 2. Caricamento Dati

Da file JSON:

```cpp
if (!builder.fromJSONFile("event.json")) {
    std::cerr << "Errore caricamento dati\n";
    return 1;
}
```

Da stringa JSON:

```cpp
std::string jsonData = R"({...})";
if (!builder.fromJSON(jsonData)) {
    std::cerr << "Errore parsing JSON\n";
    return 1;
}
```

Da struttura dati:

```cpp
OccultationEvent event;
// ... popola event ...
builder.setEvent(event);
```

### 3. Generazione Carte

#### Carta di Avvicinamento (Campo Largo)

```cpp
// Genera e salva carta di avvicinamento
if (builder.generateAndSaveApproachChart("approach.png")) {
    std::cout << "Carta di avvicinamento generata!\n";
}
```

Caratteristiche:
- Campo visivo: 10° × 10° (default)
- Magnitudine limite: 12.0
- Traccia asteroide: ±3 ore dall'evento
- Ideale per: localizzazione generale, uso con binocolo/cercatore

#### Carta Dettagliata (Campo Stretto)

```cpp
// Genera e salva carta dettagliata
if (builder.generateAndSaveDetailChart("detail.png")) {
    std::cout << "Carta dettagliata generata!\n";
}
```

Caratteristiche:
- Campo visivo: 3° × 1.5° (default)
- Magnitudine limite: 15.0
- Traccia asteroide: ±1 ora dall'evento
- Ideale per: osservazione telescopica, identificazione precisa

#### Carta Cercatrice (Campo Medio)

```cpp
// Genera e salva carta cercatrice
if (builder.generateAndSaveFinderChart("finder.png")) {
    std::cout << "Carta cercatrice generata!\n";
}
```

### 4. Generare Tutte le Carte

```cpp
// Genera set completo di carte
int count = builder.generateAllCharts("event_2025_12_15");
std::cout << "Generate " << count << " carte\n";

// Output files:
// - event_2025_12_15_approach.png
// - event_2025_12_15_finder.png
// - event_2025_12_15_detail.png
```

## Configurazione Personalizzata

### Carta con Configurazione Custom

```cpp
OccultationChartConfig config;

// Tipo di carta
config.chartType = ChartType::CUSTOM;

// Campo visivo personalizzato
config.fieldOfViewWidth = 5.0;   // gradi
config.fieldOfViewHeight = 3.0;   // gradi

// Risoluzione immagine
config.imageWidth = 3840;   // 4K
config.imageHeight = 2160;

// Magnitudine limite
config.limitingMagnitude = 14.0;

// Visualizzazione
config.showGrid = true;
config.showAsteroidPath = true;
config.showTargetStar = true;
config.showCircumstances = true;

// Traccia asteroide
config.pathDurationHours = 1.5;
config.pathSteps = 60;

// Colori personalizzati (formato RGBA)
config.asteroidPathColor = 0xFF4444FF;  // Rosso
config.targetStarColor = 0x44FF44FF;    // Verde

// Titolo personalizzato
config.customTitle = "Occultazione (433) Eros";

// Genera con configurazione custom
auto buffer = builder.generateCustomChart(config);
builder.saveChart(buffer, "custom.png");
```

### Configurazione Default per Tipo

```cpp
// Ottieni configurazione default per un tipo specifico
auto config = OccultationChartConfig::getDefaultForType(ChartType::DETAIL);

// Modifica alcuni parametri
config.limitingMagnitude = 16.0;
config.imageWidth = 4096;

// Usa la configurazione modificata
auto buffer = builder.generateDetailChart(&config);
```

## Configurazioni Avanzate

### Timeout e Cache Cataloghi

```cpp
// Imposta timeout per query ai cataloghi (secondi)
builder.setCatalogTimeout(60);

// Abilita/disabilita cache
builder.setCacheEnabled(true);
```

### Logging

```cpp
// Imposta livello di log
// 0 = nessuno, 1 = errori, 2 = warning, 3 = info, 4 = debug
builder.setLogLevel(3);
```

### Validazione Dati

```cpp
// Valida i dati dell'evento
if (!builder.validate()) {
    std::cerr << "Dati non validi!\n";
    
    // Ottieni messaggi di validazione
    auto messages = builder.getValidationMessages();
    for (const auto& msg : messages) {
        std::cerr << "  - " << msg << "\n";
    }
}
```

## Funzioni di Utilità

### Calcolo Posizione Asteroide

```cpp
using namespace starmap::occultation::utils;

// Calcola posizione dell'asteroide 2 ore prima dell'evento
auto pos = calculateAsteroidPosition(event, -2.0);

std::cout << "RA: " << pos.getRightAscension() << "°\n";
std::cout << "Dec: " << pos.getDeclination() << "°\n";
```

### Campo Visivo Ottimale

```cpp
// Calcola campo visivo ottimale per un tipo di carta
auto [width, height] = calculateOptimalFieldOfView(event, ChartType::DETAIL);

std::cout << "FOV ottimale: " << width << "° × " << height << "°\n";
```

### Generazione Nome File

```cpp
// Genera nome file automatico per l'evento
std::string filename = generateFilename(event, ChartType::DETAIL, "png");

std::cout << "Nome file: " << filename << "\n";
// Output: 2025_12_15_(433) Eros_detail.png
```

## Tipi di Carta

```cpp
enum class ChartType {
    APPROACH,      // Campo largo (10° × 10°)
    DETAIL,        // Campo stretto (3° × 1.5°)
    FINDER,        // Campo medio (5° × 5°)
    WIDE_FIELD,    // Campo molto ampio (20° × 20°)
    CUSTOM         // Configurazione personalizzata
};
```

## Formati di Output

Il sistema supporta diversi formati di output:

```cpp
// PNG (default)
builder.saveChart(buffer, "chart.png", "png");

// SVG (vettoriale)
builder.saveChart(buffer, "chart.svg", "svg");

// PDF (per stampa)
builder.saveChart(buffer, "chart.pdf", "pdf");
```

## Esempio Completo

```cpp
#include "starmap/occultation/OccultationChartBuilder.h"
#include <iostream>

int main() {
    using namespace starmap::occultation;
    
    // Crea builder
    OccultationChartBuilder builder;
    
    // Carica dati
    if (!builder.fromJSONFile("event.json")) {
        std::cerr << "Errore caricamento dati\n";
        return 1;
    }
    
    // Valida
    if (!builder.validate()) {
        std::cerr << "Dati non validi\n";
        return 1;
    }
    
    // Mostra info evento
    const auto& event = builder.getEvent();
    std::cout << "Evento: " << event.eventId << "\n";
    std::cout << "Asteroide: " << event.asteroid.name << "\n";
    std::cout << "Data: " << event.circumstances.eventTime << "\n";
    
    // Genera carte
    builder.generateAndSaveApproachChart("approach.png");
    builder.generateAndSaveDetailChart("detail.png");
    
    std::cout << "Carte generate con successo!\n";
    
    return 0;
}
```

## Compilazione

```bash
# Compila l'esempio
cd build
cmake ..
make occultation_chart

# Esegui
./examples/occultation_chart ../examples/occultation_examples/example_eros.json
```

## Note Importanti

1. **Requisiti Cataloghi**: Le API richiedono accesso ai cataloghi Gaia (locale o online)
2. **Memoria**: Carte ad alta risoluzione con molte stelle richiedono memoria significativa
3. **Tempo**: La generazione può richiedere da pochi secondi a qualche minuto
4. **Precisione**: Il calcolo della posizione dell'asteroide usa approssimazione lineare
5. **Coordinate**: Tutte le coordinate sono in gradi decimali J2000.0

## Troubleshooting

### Errore "Catalogo non disponibile"
```cpp
builder.setCatalogTimeout(120);  // Aumenta timeout
```

### Stelle mancanti
```cpp
config.limitingMagnitude = 16.0;  // Aumenta magnitudine limite
```

### Campo visivo troppo piccolo/grande
```cpp
config.fieldOfViewWidth = 5.0;   // Regola campo
config.fieldOfViewHeight = 3.0;
```

## Risorse

- Documentazione completa: `docs/API_OCCULTATION.md`
- Esempi: `examples/occultation_chart.cpp`
- Dati esempio: `examples/occultation_examples/`
- Issue tracker: GitHub Issues

## Contributi

Per domande, bug report o feature request, aprire una issue su GitHub.
