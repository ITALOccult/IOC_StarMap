# Carte di Avvicinamento Stile IAU

Questo documento descrive le configurazioni per generare carte stellari in stile IAU (International Astronomical Union), ottimizzate per l'uso professionale e la stampa.

## Caratteristiche dello Stile IAU

Le carte in stile IAU seguono gli standard internazionali per le carte stellari di riferimento:

### 1. **Qualità di Stampa Professionale**
- Risoluzione: **2400×2400 pixel** a **300 DPI**
- Garantisce stampe nitide su carta A4/Letter
- Ideale per uso sul campo in condizioni di scarsa luminosità

### 2. **Schema di Colori Ottimizzato**
```json
"backgroundColor": "#ffffff"          // Bianco puro
"starColor": "#000000"                // Nero per massimo contrasto
"gridColor": "#888888"                // Grigio medio per griglia visibile
"constellationLineColor": "#333333"   // Grigio scuro per linee costellazioni
"constellationBoundaryColor": "#666666" // Grigio per confini
"labelColor": "#000000"               // Nero per etichette leggibili
"borderColor": "#000000"              // Bordo marcato
```

### 3. **Elementi Visualizzati**
- ✅ Griglia equatoriale (5° intervallo)
- ✅ Linee delle costellazioni
- ✅ Confini delle costellazioni (IAU standard)
- ✅ Numeri SAO per riferimento preciso
- ✅ Etichette stelle principali (Bayer/Flamsteed)
- ✅ Scala graduata
- ✅ Indicatore Nord
- ✅ Legenda

### 4. **Parametri Stelle**
```json
"starSizeMultiplier": 1.2    // Dimensioni visibili ma non eccessive
"minStarSize": 0.8           // Stelle deboli ancora visibili
"maxStarSize": 6.0           // Stelle brillanti ben distinguibili
```

## File di Configurazione Disponibili

### 1. `iau_style_finder.json` - Template Generico
Configurazione base stile IAU, personalizzabile per qualsiasi regione celeste.

**Parametri chiave:**
- Campo: 30° (ottimo per localizzare regioni ampie)
- Magnitudine limite: 7.5
- Griglia: 5°

**Uso:**
```bash
./generate_chart iau_style_finder.json
```

### 2. `occultation_approach_iau.json` - Occultazioni Asteroidali
Ottimizzata per carte di avvicinamento nelle occultazioni asteroidali.

**Parametri chiave:**
- Campo: 20° (bilanciamento tra contesto e dettaglio)
- Magnitudine limite: 10.0 (include stelle di riferimento)
- SAO fino a mag 8.0

**Uso:**
```bash
./generate_chart occultation_approach_iau.json
```

### 3. `m31_finder.json` - M31 Andromeda (Aggiornato)
Esempio pratico per M31, ora con qualità IAU.

### 4. `telescope_finder.json` - M42 Orione (Aggiornato)
Esempio per M42, ottimizzato per telescopio.

### 5. `occultation_detail_betelgeuse_style.json` - Carta Dettaglio Occultazione
Mappa dettagliata in formato panoramico (16:9) per osservazione diretta dell'occultazione.

**Parametri chiave:**
- Formato: 2162×1184 px (panoramico come esempio Betelgeuse)
- Campo: 2° × 1° (percorso dettagliato)
- Magnitudine limite: 15.0 (stelle deboli visibili)
- Griglia: 0.25° (posizionamento preciso)
- SAO fino a mag 12.0

**Uso:**
```bash
./generate_chart occultation_detail_betelgeuse_style.json
```

**Note:** Basata sul file di riferimento `maps-betelgeuse-AC-white.jpg`. Il percorso dell'asteroide va aggiunto in post-processing.

## Come Personalizzare una Carta IAU

### Esempio: Carta per Occultazione Asteroide

```json
{
    "title": "Occultazione (433) Eros - 2026-03-15",
    "subtitle": "Campo: 20° - Magnitudine: ≤ 10.0",
    
    "centerRA": 123.45,      // ← Modifica coordinate
    "centerDec": 23.67,      // ← della tua regione
    "fieldRadius": 20.0,
    
    "width": 2400,
    "height": 2400,
    "pngDensity": 300,
    "maxMagnitude": 10.0,
    
    // ... resto configurazione IAU standard ...
    
    "outputPath": "eros_approach_2026"
}
```

## Confronto Tipi di Carta

| Tipo | Campo | Dimensioni | Mag Limite | SAO Limite | Griglia | Uso |
|------|-------|------------|------------|------------|---------|-----|
| **IAU Finder** | 30° | 2400×2400 | 7.5 | 6.5 | 5° | Localizzazione generale |
| **Occultation Approach** | 20° | 2400×2400 | 10.0 | 8.0 | 5° | Avvicinamento occultazione |
| **Telescope Finder** | 25° | 2400×2400 | 7.5 | 7.0 | 5° | Puntamento telescopio |
| **Occultation Detail** | 2°×1° | 2162×1184 | 15.0 | 12.0 | 0.25° | Percorso dettagliato (stile Betelgeuse) |
| **Detail Chart** | 2° | 1200×1200 | 16.0 | - | 0.5° | Campo dettagliato generico |

## Differenze Rispetto alle Carte Standard

### Carte Standard (precedenti)
- 1200×1200 px @ 200 DPI
- Colori griglia più chiari (#aaaaaa)
- Stelle più piccole
- Ottimo per visualizzazione schermo

### Carte Stile IAU (nuove)
- 2400×2400 px @ 300 DPI
- Colori più contrastati (#888888)
- Stelle più visibili (×1.2)
- Bordo marcato (2.0)
- Ottimizzato per stampa professionale

## Suggerimenti per la Stampa

### Stampa Ottimale
1. **Formato:** A4 o Letter
2. **Orientamento:** Quadrato (bordi uguali)
3. **Qualità:** 300 DPI minimo
4. **Carta:** Bianca opaca (no lucida)
5. **Modalità:** Bianco/nero ad alta qualità

### Per Uso Notturno
- Stampa su carta bianca normale
- Plastifica per protezione umidità/rugiada
- Usa torcia LED rossa per leggere
- I numeri SAO aiutano l'identificazione precisa

## Integrazione con Occultazioni

Le carte IAU sono perfette come **primo step** nelle occultazioni:

```
1. Carta IAU Finder (20°-30°)    ← Localizzare regione generale
2. Carta Occultation Approach     ← Avvicinamento (10°)
3. Carta Detail                   ← Campo telescopio (3°)
4. Carta Finale                   ← Percorso dettagliato (1°)
```

## Link Utili

- **IAU Constellation Boundaries:** https://www.iau.org/public/themes/constellations/
- **SAO Catalog:** Smithsonian Astrophysical Observatory Star Catalog
- **Coordinate Converter:** Usa il sistema per convertire J2000 → Epoca

## Note

- Le coordinate sono sempre in **J2000.0**
- La griglia mostra **AR (Right Ascension)** e **Dec (Declination)**
- I confini delle costellazioni seguono gli standard IAU del 1930
- Le dimensioni stelle sono proporzionali alla magnitudine

---

**Versione:** 1.0  
**Data:** 2 Gennaio 2026  
**Progetto:** IOC_StarMap
