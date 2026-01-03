# Database Locale Gaia-SAO Cross-Match

## Panoramica

Il sistema di cross-match Gaia-SAO utilizza un database SQLite locale per mappare rapidamente i `source_id` di Gaia DR3 ai corrispondenti numeri SAO. Questo elimina la necessità di query online ripetute e migliora drasticamente le performance.

## Caratteristiche

- **Performance**: Query per Gaia ID in < 0.1 ms
- **Copertura**: ~259,000 stelle SAO (magnitudine < 9)
- **Dimensione**: ~15 MB
- **Fallback automatico**: Se il database non è disponibile, il sistema usa query online
- **Multi-livello**: Cerca prima nel database locale, poi fallback su SIMBAD/VizieR

## Installazione

### 1. Requisiti

```bash
# macOS
brew install sqlite3

# Ubuntu/Debian
sudo apt-get install libsqlite3-dev

# Python per generazione database
pip install astroquery astropy
```

### 2. Generazione del Database

Il database viene generato tramite lo script Python fornito:

```bash
cd IOC_StarMap
python scripts/build_gaia_sao_database.py --output gaia_sao_xmatch.db
```

**Opzioni disponibili:**

- `--output, -o`: Path del file database (default: `gaia_sao_xmatch.db`)
- `--max-magnitude, -m`: Magnitudine massima stelle da includere (default: 9.0)
- `--test`: Modalità test (processa solo 100 stelle per verifiche rapide)

**Esempio con magnitudine limite personalizzata:**

```bash
python scripts/build_gaia_sao_database.py --max-magnitude 8.5 --output gaia_sao_bright.db
```

### 3. Test Rapido

Per verificare che tutto funzioni:

```bash
python scripts/build_gaia_sao_database.py --test
```

Questo crea un database di test con solo 100 stelle in pochi secondi.

## Utilizzo

### Uso Automatico (Consigliato)

Il database viene caricato automaticamente se presente nella directory di esecuzione:

```cpp
#include <starmap/catalog/SAOCatalog.h>

// Carica automaticamente gaia_sao_xmatch.db se presente
starmap::catalog::SAOCatalog catalog;

// Arricchisce stella con numero SAO
catalog.enrichWithSAO(star);
```

### Uso con Path Personalizzato

```cpp
// Specifica path personalizzato
starmap::catalog::SAOCatalog catalog("/path/to/my_gaia_sao.db");

if (catalog.hasLocalDatabase()) {
    std::cout << "Database locale disponibile!" << std::endl;
    std::cout << catalog.getDatabaseStatistics() << std::endl;
}
```

### Uso con CatalogManager

Il CatalogManager usa automaticamente il database locale quando disponibile:

```cpp
starmap::catalog::CatalogManager manager;

// enrichWithSAO=true (default) usa database locale se disponibile
auto stars = manager.queryStars(params, true);

// Tutte le stelle avranno numeri SAO se presenti nel database
```

## Strategia di Lookup Multi-Livello

Il sistema usa una strategia a cascata per massimizzare il tasso di successo:

1. **Database locale per Gaia ID** (< 0.1 ms)
   - Lookup diretto per `source_id`
   - Più veloce e affidabile

2. **Database locale per coordinate** (< 1 ms)
   - Cone search nel database locale
   - Utile se Gaia ID non presente

3. **SIMBAD online** (~500 ms)
   - Query cross-reference online
   - Solo se database locale non disponibile

4. **VizieR online** (~800 ms)
   - Query cross-match VizieR
   - Fallback finale

## Posizionamento del Database

Il database viene cercato nei seguenti path (in ordine):

1. Path specificato nel costruttore
2. Directory corrente: `./gaia_sao_xmatch.db`
3. Directory dati: `~/.starmap/gaia_sao_xmatch.db`
4. Directory installazione: `/usr/local/share/starmap/gaia_sao_xmatch.db`

**Consiglio**: Copia il database nella directory del progetto o in `~/.starmap/`

```bash
# Opzione 1: Directory progetto
cp gaia_sao_xmatch.db /path/to/project/

# Opzione 2: Directory utente
mkdir -p ~/.starmap
cp gaia_sao_xmatch.db ~/.starmap/
```

## Statistiche e Diagnostica

```cpp
starmap::catalog::SAOCatalog catalog;

if (catalog.hasLocalDatabase()) {
    // Stampa statistiche
    std::cout << catalog.getDatabaseStatistics() << std::endl;
    
    /* Output esempio:
     * Total entries: 258,997
     * Magnitude range: -1.46 - 8.99 (avg: 6.43)
     * Separation avg: 0.23" (max: 4.87")
     * Database size: 14.8 MB
     */
}
```

## Aggiornamento del Database

Per aggiornare il database con dati più recenti:

```bash
# Rigenera database
python scripts/build_gaia_sao_database.py --output new_gaia_sao.db

# Sostituisci quello vecchio
mv new_gaia_sao.db gaia_sao_xmatch.db
```

**Nota**: Il processo di generazione può richiedere 30-60 minuti per il catalogo completo a causa dei rate limits dei servizi online.

## Struttura Database

### Tabella `gaia_sao_xmatch`

| Colonna | Tipo | Descrizione |
|---------|------|-------------|
| `gaia_source_id` | INTEGER (PRIMARY KEY) | Source ID Gaia DR3 |
| `sao_number` | INTEGER | Numero SAO |
| `ra` | REAL | Right Ascension (J2000, gradi) |
| `dec` | REAL | Declination (J2000, gradi) |
| `magnitude` | REAL | Magnitudine G di Gaia |
| `separation` | REAL | Separazione cross-match (arcsec) |
| `created_at` | TEXT | Timestamp creazione entry |

### Indici

- `idx_sao_number`: Lookup per numero SAO
- `idx_ra_dec`: Cone search per coordinate
- `idx_magnitude`: Filtro per magnitudine

## API Avanzata

### Query Diretta al Database

```cpp
#include <starmap/catalog/GaiaSAODatabase.h>

starmap::catalog::GaiaSAODatabase db("gaia_sao_xmatch.db");

// Query per Gaia ID
auto sao = db.findSAOByGaiaId(5853498713190525696);
if (sao) {
    std::cout << "SAO " << *sao << std::endl;
}

// Cone search
auto entries = db.coneSearch(coords, 1.0); // 1 grado
for (const auto& entry : entries) {
    std::cout << "Gaia: " << entry.gaiaSourceId 
              << " -> SAO: " << entry.saoNumber << std::endl;
}

// Statistiche
std::cout << db.getStatistics() << std::endl;

// Verifica integrità
if (db.verifyIntegrity()) {
    std::cout << "Database integro" << std::endl;
}
```

## Troubleshooting

### Database non trovato

**Sintomo**: `Warning: Gaia-SAO local database not available`

**Soluzione**:
1. Verifica che il file `gaia_sao_xmatch.db` esista
2. Controlla i permessi di lettura
3. Genera il database con lo script Python

### Performance lente

**Sintomo**: Query lente anche con database locale

**Soluzione**:
```bash
# Ottimizza database
sqlite3 gaia_sao_xmatch.db "VACUUM; ANALYZE;"
```

### Database corrotto

**Sintomo**: Errori di integrità

**Soluzione**:
```bash
# Verifica integrità
sqlite3 gaia_sao_xmatch.db "PRAGMA integrity_check;"

# Se corrotto, rigenera
python scripts/build_gaia_sao_database.py --output gaia_sao_xmatch.db
```

## Esempio Completo

```cpp
#include <starmap/StarMap.h>
#include <iostream>

int main() {
    // Crea catalog manager (usa database locale se disponibile)
    starmap::catalog::CatalogManager manager;
    
    // Verifica database locale
    if (manager.getSAOCatalog().hasLocalDatabase()) {
        std::cout << "✓ Database locale Gaia-SAO disponibile\n";
        std::cout << manager.getSAOCatalog().getDatabaseStatistics() << "\n";
    } else {
        std::cout << "⚠ Database locale non disponibile, uso query online\n";
    }
    
    // Query stelle con arricchimento SAO automatico
    starmap::catalog::GaiaQueryParameters params;
    params.center = starmap::core::EquatorialCoordinates(10.68, 41.27); // M31
    params.radiusDegrees = 1.0;
    params.maxMagnitude = 8.0;
    
    auto stars = manager.queryStars(params, true); // enrichWithSAO = true
    
    std::cout << "Trovate " << stars.size() << " stelle\n";
    
    // Conta quante hanno numero SAO
    int withSAO = 0;
    for (const auto& star : stars) {
        if (star->getSAONumber()) {
            withSAO++;
            std::cout << "Gaia " << star->getGaiaId() 
                      << " -> SAO " << *star->getSAONumber() << "\n";
        }
    }
    
    std::cout << "\nStelle con SAO: " << withSAO << "/" << stars.size()
              << " (" << (100.0 * withSAO / stars.size()) << "%)\n";
    
    return 0;
}
```

## Performance Benchmark

| Operazione | Database Locale | Query Online |
|------------|----------------|--------------|
| Lookup per Gaia ID | < 0.1 ms | 500-800 ms |
| Cone search 1° | < 1 ms | 1000-2000 ms |
| 1000 stelle | ~50 ms | 8-15 minuti |

**Conclusione**: Il database locale è **~5000-10000x più veloce** delle query online.

## Licenza e Crediti

- **Gaia DR3**: ESA/Gaia collaboration
- **SAO Catalog**: Smithsonian Astrophysical Observatory
- **VizieR**: CDS, Strasbourg
- **SIMBAD**: CDS, Strasbourg

Per citazioni scientifiche, fare riferimento a:
- Gaia Collaboration et al. (2022), A&A, 649, A1
- SAO Staff (1966), Smithsonian Astrophysical Observatory Star Catalog
