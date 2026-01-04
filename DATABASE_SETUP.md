# Stellar Crossref Database Setup

Il database `gaia_sao_xmatch.db` **non è incluso** nel repository Git a causa delle dimensioni (424 MB).

## Download e Installazione

### Opzione 1: Database Precompilato

Il database `stellar_crossref_complete.db` (versione 3.0) è disponibile in `stellar_crossref_v3.0.tar.gz`.

1. **Estrarre il database**:
   ```bash
   tar -xzf stellar_crossref_v3.0.tar.gz
   ```

2. **Copiare nella directory di lavoro**:
   ```bash
   cp stellar_crossref_v2.0/stellar_crossref_complete.db build/examples/gaia_sao_xmatch.db
   ```

### Opzione 2: Generare da Zero

Se non hai il tar.gz, puoi rigenerare il database usando gli script in `scripts/`:

```bash
cd scripts
python3 build_gaia_sao_database.py
```

## Contenuto Database

- **2.53 milioni** di stelle totali
- **2.50 milioni** con Gaia DR3 source_id
- **258,287** stelle con numero SAO
- Crossreference: Gaia DR3, Tycho-2, Hipparcos, SAO, HD, HR
- Designazioni: Bayer, Flamsteed, nomi propri

## Schema Database

```sql
CREATE TABLE stars (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    gaia_dr3 BIGINT,
    tyc1 INT,
    tyc2 INT,
    tyc3 INT,
    hip INT,
    sao INT,
    hd INT,
    hr INT,
    bayer TEXT,
    flamsteed INT,
    proper_name TEXT,
    constellation TEXT,
    ra_deg REAL,
    dec_deg REAL,
    magnitude REAL
);
```

## Uso nel Codice

```cpp
#include "starmap/StarMap.h"

int main() {
    // Configura path al database
    starmap::config::LibraryConfig::CatalogPaths paths;
    paths.gaiaSaoDatabase = "build/examples/gaia_sao_xmatch.db";
    
    // Inizializza
    starmap::initialize(paths);
    
    // Usa la libreria normalmente
    // ...
}
```

## Database Opzionale

La libreria **funziona anche senza** il database:
- Usa solo i dati scaricati da Gaia DR3 online
- Non ci sarà arricchimento con numeri SAO
- Non ci saranno designazioni Bayer/Flamsteed dal database locale

## Verifica Installazione

```bash
cd build/examples
./gaia_approach_map
```

Output atteso:
```
Gaia-SAO local database loaded successfully
Database size: 424.23 MB
Arricchite 208 stelle con numeri SAO
```

Se il database non è trovato, vedrai:
```
Cannot open Gaia-SAO database: unable to open database file
```

## Checksum

Verifica l'integrità del database scaricato:
```bash
sha256sum stellar_crossref_v2.0/stellar_crossref_complete.db
```

Checksum atteso:
```
9ed891e70bc70d29d946c11861b3c13379d806459ff8483f2c6b73ced376e298
```

## Link Download Alternativi

Se non hai il file tar.gz locale, puoi richiedere il database a:
- Repository releases (se configurato)
- Link condiviso separatamente dal team

## Aggiornamenti

La versione attuale è **v3.0** (stellar_crossref_v3.0.tar.gz).

Versioni precedenti:
- v2.0: 2.77M stelle
- v1.0: Solo SAO (258K stelle)
