# Configurazione Libreria StarMap

## Inizializzazione

La libreria StarMap richiede l'inizializzazione con i path ai database prima dell'uso.

### Uso Basilare

```cpp
#include "starmap/StarMap.h"

int main() {
    // Inizializza con path di default
    starmap::initialize();
    
    // Usa la libreria
    // ...
}
```

### Configurazione Path Personalizzati

```cpp
#include "starmap/StarMap.h"

int main() {
    // Configura path ai database
    starmap::config::LibraryConfig::CatalogPaths paths;
    paths.gaiaSaoDatabase = "/path/to/gaia_sao_xmatch.db";
    paths.iauCatalog = "/path/to/IAU-CSN.json";
    paths.starNamesDatabase = "/path/to/common_star_names.csv";
    
    // Inizializza
    starmap::initialize(paths);
    
    // Usa la libreria
    // ...
}
```

### Path di Default

Se non viene chiamata `initialize()` o vengono omessi i path, la libreria usa i seguenti default:

- **gaiaSaoDatabase**: `"gaia_sao_xmatch.db"` (directory corrente)
- **iauCatalog**: `"data/IAU-CSN.json"`
- **starNamesDatabase**: `"data/common_star_names.csv"`

### Database Gaia-SAO

Il database `gaia_sao_xmatch.db` è **opzionale**. Se non presente:
- La libreria continua a funzionare normalmente
- Utilizza solo i dati scaricati da Gaia DR3
- Non sarà possibile l'arricchimento con numeri SAO

Per utilizzare il database completo:
1. Scarica o genera `stellar_crossref_complete.db`
2. Rinominalo in `gaia_sao_xmatch.db` o configura il path
3. Posizionalo nella directory di lavoro o specifica il path assoluto

### Esempio Completo

```cpp
#include "starmap/StarMap.h"
#include <iostream>

int main() {
    try {
        // Configura path
        starmap::config::LibraryConfig::CatalogPaths paths;
        paths.gaiaSaoDatabase = "build/examples/gaia_sao_xmatch.db";
        
        // Inizializza
        starmap::initialize(paths);
        
        // Genera mappa
        starmap::map::ChartConfig config;
        config.centerRA = 88.79;
        config.centerDec = 7.41;
        config.fieldRadius = 10.0;
        config.maxMagnitude = 7.0;
        config.outputPath = "my_chart";
        
        starmap::map::ChartGenerator generator;
        generator.setConfig(config);
        generator.generate();
        
        std::cout << "Mappa generata con successo!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
```

## Note Importanti

1. **Inizializzazione Una Volta**: Chiamare `initialize()` una sola volta all'inizio del programma

2. **Path Relativi vs Assoluti**: 
   - Path relativi sono calcolati dalla directory di lavoro corrente
   - Path assoluti sono sempre risolti correttamente

3. **Thread Safety**: La configurazione è globale e non thread-safe durante l'inizializzazione

4. **Performance**: Il database viene caricato on-demand, la prima query può essere più lenta

## Risoluzione Problemi

### Database non trovato
```
Cannot open Gaia-SAO database: unable to open database file
```
**Soluzione**: Verifica che il path sia corretto e il file esista

### Database non valido
```
Stellar crossref database table not found
```
**Soluzione**: Il file database non contiene la tabella `stars` corretta

### Funziona senza database
Se il database non viene trovato, la libreria continua a funzionare normalmente usando solo Gaia DR3.
