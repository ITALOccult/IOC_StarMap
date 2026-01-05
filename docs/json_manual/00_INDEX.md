# Manuale di Configurazione JSON - Indice

Guida completa alla configurazione delle mappe celesti tramite file JSON.

## Capitoli

### 1. [Configurazione Base](01_BASE_CONFIG.md)
- Centro e campo visivo
- Dimensioni immagine
- Sistemi di proiezione
- Magnitudine limite
- Formule per calcolo campo visivo

### 2. [Performance e Ottimizzazione](02_PERFORMANCE.md)
- Gestione memoria per grandi cataloghi
- Batch rendering
- Calcolo automatico limiti stelle
- Strategie di ottimizzazione
- Trade-off magnitudine/performance

### 3. [Rendering Stelle e Griglia](03_RENDERING.md)
- Configurazione griglia coordinate
- Simboli stellari e dimensionamento
- Colori spettrali
- Etichette stelle
- Sistemi di coordinate

### 4. [Elementi UI e Annotazioni](04_UI_ELEMENTS.md)
- Titolo con posizionamento avanzato
- Bordo e margini
- Bussola celeste
- Scala angolare
- Legenda magnitudini

### 5. [Overlay e Elementi Grafici](05_OVERLAYS.md)
- Costellazioni (linee, confini, nomi)
- Via Lattea, eclittica, equatore
- Rettangoli AR/DEC
- Path per tracce asteroidi/comete
- Elementi personalizzati

### 6. [Esempi Avanzati](06_ADVANCED_EXAMPLES.md)
- Carta finder IAU standard
- Mappe per occultazioni asteroidali
- Deep sky con overlay multipli
- Animazioni path temporali
- Mappe multi-proiezione

### 7. [Reference Tecnica](07_TECHNICAL_REFERENCE.md)
- Formato colori RGBA
- Conversioni coordinate
- Sistemi di proiezione (dettagli matematici)
- Validazione parametri
- API C++ per caricamento JSON

### 8. [Troubleshooting e FAQ](08_TROUBLESHOOTING.md)
- Errori comuni
- Problemi di performance
- Validazione configurazioni
- Debug e logging

---

## Quick Start

**Esempio minimale**:
```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 10.0, "height": 10.0},
  "image": {"width": 2400, "height": 2400},
  "limiting_magnitude": 11.0
}
```

**Uso da C++**:
```cpp
#include <starmap/StarMap.h>

starmap::StarMapBuilder builder;
auto image = builder.generateFromFile("config.json");
image.saveAsPNG("output.png");
```

**Uso da Python** (via stellar_crossref_v2.0):
```python
import subprocess
subprocess.run(['generate_chart', 'config.json', '-o', 'output.png'])
```

---

## Convenzioni

- **Coordinate**: Tutte in gradi decimali (J2000.0)
- **Colori**: Formato RGBA uint32 (es: `4294967295` = bianco)
- **Angoli**: Senso orario da Nord (standard astronomico)
- **Magnitudini**: Sistema Gaia DR3 G-band

## File di Esempio

Vedi [`examples/config_examples/`](../../examples/config_examples/) per configurazioni complete.

## Licenza

Questo manuale è parte del progetto IOC_StarMap (MIT License).
