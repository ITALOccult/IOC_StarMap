# Scripts Directory

Questo directory contiene script di utilità per IOC_StarMap.

## build_gaia_sao_database.py

Script per generare il database locale di cross-match tra Gaia DR3 e catalogo SAO.

### Requisiti

```bash
pip install astroquery astropy
```

### Uso Base

```bash
# Genera database completo (magnitudine < 9.0)
python build_gaia_sao_database.py

# Specifica output personalizzato
python build_gaia_sao_database.py --output my_database.db

# Limite magnitudine personalizzato
python build_gaia_sao_database.py --max-magnitude 8.5

# Test rapido (100 stelle)
python build_gaia_sao_database.py --test
```

### Opzioni

- `--output, -o`: Path file database output (default: `gaia_sao_xmatch.db`)
- `--max-magnitude, -m`: Magnitudine massima stelle da includere (default: 9.0)
- `--test`: Modalità test con solo 100 stelle

### Note

- Il processo completo richiede 30-60 minuti a causa dei rate limits dei servizi online
- Il database finale occupa circa 15 MB
- Include ~259,000 stelle con magnitudine < 9.0
- Usa VizieR per scaricare il catalogo SAO e Gaia ESA Archive per il cross-match

### Output

Il database SQLite generato contiene:

- Tabella `gaia_sao_xmatch` con mapping Gaia source_id → SAO number
- Indici ottimizzati per query veloci
- Metadata sulla versione e data di creazione

### Esempio Output

```
==================================================
GAIA-SAO CROSS-MATCH DATABASE BUILDER
==================================================

Creazione database: gaia_sao_xmatch.db
Database creato con successo
Scaricamento catalogo SAO (mag < 9.0)...
Ricevute 258997 stelle dal catalogo SAO

Cross-matching 258997 stelle SAO con Gaia DR3...
Questo può richiedere diversi minuti...

Progresso: 100/258997 (94 matched, 6 failed)
...

Cross-match completato:
  Matched: 243568
  Failed: 15429
  Success rate: 94.1%

Creazione indici...
Indici creati

Ottimizzazione database...
Database ottimizzato

==================================================
STATISTICHE DATABASE
==================================================
Totale entry: 243,568
Range magnitudini: -1.46 - 8.99 (media: 6.43)
Separazione media: 0.23" (max: 4.87")
Dimensione database: 14.8 MB
==================================================

Database salvato in: gaia_sao_xmatch.db
```

### Troubleshooting

**Problema**: `ImportError: No module named 'astroquery'`

**Soluzione**:
```bash
pip install astroquery astropy
```

**Problema**: Query timeout o rate limiting

**Soluzione**: Lo script include automaticamente rate limiting. Se persistono errori, riavvia lo script - riprenderà da dove si era interrotto.

**Problema**: Database corrotto

**Soluzione**: Cancella il database e rigenera:
```bash
rm gaia_sao_xmatch.db
python build_gaia_sao_database.py
```

## Documentazione Aggiuntiva

Per maggiori dettagli sull'uso del database, vedi:
- [docs/GAIA_SAO_DATABASE.md](../docs/GAIA_SAO_DATABASE.md)
