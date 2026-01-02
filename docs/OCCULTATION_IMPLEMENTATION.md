# API Occultazioni Asteroidali - Sommario Implementazione

## File Creati

### Header Files
- `include/starmap/occultation/OccultationData.h` - Strutture dati per eventi di occultazione
- `include/starmap/occultation/OccultationChartBuilder.h` - API per generazione carte

### Source Files
- `src/occultation/OccultationData.cpp` - Implementazione strutture dati
- `src/occultation/OccultationChartBuilder.cpp` - Implementazione builder e utilità

### Esempi
- `examples/occultation_chart.cpp` - Esempio completo di utilizzo API
- `examples/occultation_examples/example_eros.json` - Dati esempio occultazione (433) Eros
- `examples/occultation_examples/occultation_helper.py` - Script helper per creazione JSON

### Documentazione
- `docs/OCCULTATION_API.md` - Documentazione completa API
- `docs/OCCULTATION_QUICKSTART.md` - Guida rapida per utenti
- `examples/occultation_examples/README.md` - Guida esempi

### Build System
- Aggiornato `CMakeLists.txt` principale
- Aggiornato `examples/CMakeLists.txt`
- Aggiornato `include/starmap/StarMap.h`

## Funzionalità Implementate

### 1. Strutture Dati

#### OccultationEvent
Contiene tutti i dati di un evento:
- Stella target (coordinate, magnitudine, cataloghi)
- Asteroide (elementi, velocità, dimensioni)
- Circostanze evento (durata, drop mag, probabilità)
- Traccia ombra (opzionale)
- Sito osservazione (opzionale)

#### OccultationChartConfig
Configurazione completa per carte:
- Tipo carta (approach, detail, finder, wide, custom)
- Campo visivo (width/height in gradi)
- Risoluzione immagine
- Magnitudine limite
- Opzioni visualizzazione (griglia, traccia, markers)
- Stili e colori personalizzabili

### 2. API Principali

#### OccultationChartBuilder
Classe principale per generazione carte:

```cpp
// Caricamento dati
bool fromJSON(const std::string& jsonData);
bool fromJSONFile(const std::string& jsonFilePath);
void setEvent(const OccultationEvent& event);

// Generazione carte standard
map::ImageBuffer generateApproachChart(config*);  // Campo largo 10°×10°
map::ImageBuffer generateDetailChart(config*);     // Campo stretto 3°×1.5°
map::ImageBuffer generateFinderChart(config*);     // Campo medio 5°×5°

// Generazione personalizzata
map::ImageBuffer generateCustomChart(const OccultationChartConfig&);

// Salvataggio
bool saveChart(const map::ImageBuffer&, filename, format);
bool generateAndSaveApproachChart(filename, config*);
bool generateAndSaveDetailChart(filename, config*);
int generateAllCharts(baseFilename, format);  // Genera tutte le carte

// Configurazione
void setCatalogTimeout(int seconds);
void setCacheEnabled(bool enabled);
void setLogLevel(int level);

// Validazione
bool validate() const;
std::vector<std::string> getValidationMessages() const;
```

### 3. Funzioni Utilità

```cpp
namespace starmap::occultation::utils {
    // Parsing JSON
    OccultationEvent parseIOCJSON(const std::string& json);
    OccultationEvent parseIOTAJSON(const std::string& json);
    
    // Calcoli
    core::EquatorialCoordinates calculateAsteroidPosition(
        const OccultationEvent&, double timeOffsetHours);
    
    std::pair<double, double> calculateOptimalFieldOfView(
        const OccultationEvent&, ChartType);
    
    std::string generateFilename(
        const OccultationEvent&, ChartType, format);
}
```

### 4. Tipi di Carta

#### ChartType::APPROACH (Avvicinamento)
- Campo: 10° × 10°
- Magnitudine limite: 12.0
- Traccia: ±3 ore
- Uso: Localizzazione con cercatore/binocolo

#### ChartType::DETAIL (Dettagliata)
- Campo: 3° × 1.5° (default)
- Magnitudine limite: 15.0
- Traccia: ±1 ora
- Uso: Osservazione telescopica

#### ChartType::FINDER (Cercatrice)
- Campo: 5° × 5°
- Magnitudine limite: 13.0
- Traccia: ±2 ore
- Uso: Via di mezzo

#### ChartType::WIDE_FIELD (Campo Ampio)
- Campo: 20° × 20°
- Magnitudine limite: 10.0
- Traccia: ±6 ore
- Uso: Overview generale

#### ChartType::CUSTOM (Personalizzata)
- Tutti i parametri configurabili

### 5. Formato JSON

#### Campi Obbligatori
```json
{
  "event_id": "string",
  "target_star": {
    "ra": number,      // gradi J2000
    "dec": number,     // gradi J2000
    "magnitude": number
  },
  "asteroid": {
    "name": "string",
    "velocity_arcsec_per_hour": number,
    "position_angle": number  // gradi
  },
  "circumstances": {
    "event_time": "ISO 8601",
    "duration_seconds": number
  }
}
```

#### Campi Opzionali (Consigliati)
- `target_star.catalog_id`: ID catalogo
- `target_star.sao_number`: Numero SAO
- `asteroid.number`: Numero MPC
- `asteroid.diameter_km`: Diametro
- `circumstances.magnitude_drop`: Drop magnitudine
- `observation_site`: Dati sito
- `shadow_path`: Traccia geografica

## Uso Rapido

### Compilazione
```bash
cd build
cmake ..
make occultation_chart
```

### Esecuzione
```bash
# Con dati esempio
./examples/occultation_chart ../examples/occultation_examples/example_eros.json

# Con propri dati
./examples/occultation_chart mio_evento.json
```

### Output
- `occultation_approach.png` - Carta avvicinamento
- `occultation_detail.png` - Carta dettagliata
- `occultation_custom.png` - Carta personalizzata
- `event_ID_approach.png` - Set completo (approach)
- `event_ID_finder.png` - Set completo (finder)
- `event_ID_detail.png` - Set completo (detail)

## Esempio Codice Minimo

```cpp
#include "starmap/occultation/OccultationChartBuilder.h"

int main() {
    using namespace starmap::occultation;
    
    OccultationChartBuilder builder;
    builder.fromJSONFile("event.json");
    
    // Genera carta di avvicinamento
    builder.generateAndSaveApproachChart("approach.png");
    
    // Genera carta dettagliata
    builder.generateAndSaveDetailChart("detail.png");
    
    return 0;
}
```

## Tool Helper

### Script Python
```bash
# Crea template vuoto
python3 occultation_helper.py template > template.json

# Input interattivo
python3 occultation_helper.py interactive

# Output: YYYY-MM-DD-Asteroid.json
```

## Validazione

Il sistema valida automaticamente:
- ✓ Presenza campi obbligatori
- ✓ Range coordinate (RA: 0-360°, Dec: -90/+90°)
- ✓ Range magnitudini ragionevoli
- ✓ Durata evento positiva
- ✓ Formato data/ora ISO 8601

## Formati Output

Supportati:
- **PNG**: Default, ideale per stampa e display
- **SVG**: Vettoriale, editabile
- **PDF**: Per report e condivisione

## Personalizzazione

### Campo Visivo Custom
```cpp
OccultationChartConfig config;
config.fieldOfViewWidth = 5.0;
config.fieldOfViewHeight = 3.0;
```

### Risoluzione Alta
```cpp
config.imageWidth = 4096;   // 4K
config.imageHeight = 2304;
```

### Colori Personalizzati
```cpp
config.asteroidPathColor = 0xFF0000FF;  // Rosso
config.targetStarColor = 0x00FF00FF;    // Verde
config.gridColor = 0x404040FF;          // Grigio
```

### Traccia Estesa
```cpp
config.pathDurationHours = 3.0;  // ±3 ore
config.pathSteps = 120;           // Più punti
```

## Integrazione

### Con Cataloghi
- Gaia DR3 (automatico)
- SAO Catalog (opzionale)
- Cataloghi locali (configurabile)

### Con Altri Tool
- Occult Watcher: parser da implementare
- IOTA Reports: compatibile
- Stellarium: export futuro

## Limitazioni Attuali

1. **Calcolo posizione**: Usa approssimazione lineare (non elementi orbitali)
2. **Parser**: Solo formato IOC implementato
3. **Overlay**: Alcuni elementi grafici in TODO
4. **Formati**: Solo PNG completamente supportato

## Sviluppi Futuri

### Priorità Alta
- [ ] Implementare overlay grafici completi
- [ ] Parser formato IOTA
- [ ] Calcolo preciso da elementi orbitali
- [ ] Export SVG/PDF completo

### Priorità Media
- [ ] Parser Occult Watcher
- [ ] Visualizzazione traccia geografica
- [ ] Integrazione con cataloghi locali
- [ ] Preview interattivo

### Priorità Bassa
- [ ] Export Stellarium
- [ ] Animazione traccia
- [ ] Web API
- [ ] App mobile

## Testing

Testato con:
- ✓ Dati esempio (433) Eros
- ✓ Vari campi visivi (1° - 20°)
- ✓ Diverse magnitudini limite (10 - 16)
- ✓ Configurazioni personalizzate

Da testare:
- [ ] Eventi reali IOTA
- [ ] Occult Watcher import
- [ ] Edge cases (poli, equatore)
- [ ] Performance con molte stelle

## Documentazione

- **API completa**: `docs/OCCULTATION_API.md`
- **Quick start**: `docs/OCCULTATION_QUICKSTART.md`
- **Esempi**: `examples/occultation_chart.cpp`
- **Dati test**: `examples/occultation_examples/`

## Supporto

Per domande o problemi:
- GitHub Issues
- Email maintainer
- IOTA Forum

## Contributi

Contributi benvenuti! Vedi `CONTRIBUTING.md`

---

**Implementato da**: IOC_StarMap Team  
**Data**: 3 Dicembre 2025  
**Versione**: 1.0.0
