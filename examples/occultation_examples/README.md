# Esempi di Eventi di Occultazione

Questa directory contiene esempi di file JSON per eventi di occultazione asteroidale.

## File Disponibili

### example_eros.json
Esempio completo di occultazione dell'asteroide (433) Eros.

**Dati evento:**
- **Asteroide**: (433) Eros (Near-Earth Object)
- **Data**: 15 Dicembre 2025, 22:34:12 UTC
- **Stella**: Gaia DR3 1234567890123456 (mag 11.2)
- **Durata**: 8.3 secondi
- **Drop mag**: 0.8
- **Località**: Italia centrale

**Uso:**
```bash
cd build
./examples/occultation_chart ../examples/occultation_examples/example_eros.json
```

## Struttura JSON

Ogni file JSON deve contenere:

### Campi Obbligatori
```json
{
  "event_id": "identificativo-univoco",
  "target_star": {
    "ra": 0.0,           // gradi
    "dec": 0.0,          // gradi
    "magnitude": 0.0
  },
  "asteroid": {
    "name": "Nome",
    "velocity_arcsec_per_hour": 0.0,
    "position_angle": 0.0
  },
  "circumstances": {
    "event_time": "2025-12-15T22:34:12Z",
    "duration_seconds": 0.0
  }
}
```

### Campi Opzionali

Aggiungi per carte più informative:
- `target_star.catalog_id`: ID catalogo stella
- `target_star.sao_number`: Numero SAO
- `asteroid.number`: Numero MPC
- `asteroid.diameter_km`: Diametro asteroide
- `circumstances.magnitude_drop`: Drop di magnitudine
- `observation_site`: Dati sito osservazione
- `shadow_path`: Traccia dell'ombra

## Creare Nuovi Esempi

1. Copia `example_eros.json`
2. Modifica i dati dell'evento
3. Testa con:
   ```bash
   ./examples/occultation_chart nuovo_evento.json
   ```

## Fonti Dati

- **IOTA**: https://occultations.org/
- **Occult Watcher**: Software per predizioni
- **IOTA-ES**: https://www.iota-es.de/

## Note

- Tutte le coordinate sono J2000.0
- Date/ore in formato ISO 8601 UTC
- Angoli in gradi decimali
- Velocità in arcsec/hour
