# Mappe Dettagliate Occultazioni - Stile Betelgeuse

Questo documento spiega come creare mappe dettagliate per occultazioni asteroidali seguendo lo stile della mappa di riferimento `maps-betelgeuse-AC-white.jpg`.

## Caratteristiche della Mappa Betelgeuse

### Formato e Dimensioni
- **Risoluzione:** 2162 × 1184 pixel
- **Aspect Ratio:** ~16:9 (panoramico)
- **DPI:** 300 per stampa professionale
- **Formato:** Bianco e nero, ottimizzato per stampa

### Campo Visivo
- **Campo principale:** 3° (campo più ampio per contesto)
- **Orientamento:** Percorso dell'asteroide tipicamente orizzontale
- **Griglia:** 0.25° per posizionamento preciso

### Contenuti
1. **Stelle di riferimento**
   - Magnitudine fino a 16.0 (stelle molto deboli)
   - Numeri SAO mostrati fino a mag 14.0
   - Dimensioni proporzionali alla magnitudine

2. **Griglia coordinata**
   - Intervallo fine (15' = 0.25°)
   - Colore grigio chiaro (#cccccc)
   - Opacità 50% per non coprire stelle

3. **Percorso occultazione**
   - Tracciato dell'asteroide in **ROSSO (#FF0000)**
   - Fasce di incertezza
   - Timing previsti
   - ⚠️ Da aggiungere in post-processing

4. **Numeri SAO**
   - Identificazione univoca delle stelle
   - Mostrati accanto alle stelle fino a mag 14.0
   - Font leggibile ma non invasivo

## Configurazione di Base

```json
{
    "title": "Occultazione (Star) - Data",
    "subtitle": "Percorso dettagliato - Campo: 3°",
    
    "centerRA": 0.0,        // Coordinate centro percorso
    "centerDec": 0.0,
    
    "fieldRadius": 3.0,     // Campo 3° per contesto ampio
    
    "width": 2162,          // Formato panoramico
    "height": 1184,
    "pngDensity": 300,
    "maxMagnitude": 16.0,   // Stelle molto deboli
    
    "showGrid": true,
    "gridInterval": 0.25,   // 15 arcmin
    
    "showStarLabels": true,
    "labelMagnitudeLimit": 10.0,
    
    "showSAONumbers": true,
    "saoMagnitudeLimit": 14.0,  // SAO per identificazione
    
    "backgroundColor": "#ffffff",
    "starColor": "#000000",
    "gridColor": "#cccccc",
    
    "starSizeMultiplier": 0.7,  // Stelle più piccole per dettaglio
    "minStarSize": 0.4,
    "maxStarSize": 4.0,
    
    "_pathColor": "#FF0000"  // Percorso asteroide ROSSO
}
```

## Workflow di Creazione

### 1. Calcolo Effemeridi
Usa gli strumenti astronomici per calcolare:
- Coordinate del centro del percorso (RA, Dec)
- Direzione del moto dell'asteroide
- Timing dell'occultazione
- Incertezza del percorso

### 2. Generazione Mappa Base

```bash
# Modifica la configurazione con le coordinate corrette
nano occultation_detail_betelgeuse_style.json

# Genera la mappa stellare
./generate_chart occultation_detail_betelgeuse_style.json

# Output: occultation_detail.png
```

### 3. Post-Processing

La mappa base contiene solo le stelle. Aggiungi il percorso con:

#### Opzione A: Software Grafico (GIMP, Photoshop)
1. Apri `occultation_detail.png`
2. Crea nuovo layer per il percorso
3. Disegna:
   - Linea centrale (percorso nominale) in rosso
   - Fasce di incertezza (±1σ, ±3σ) in rosa/arancione
   - Marker temporali ogni X minuti
   - Nome asteroide e stella target
4. Esporta come PNG/JPG

#### Opzione B: Python Script
```python
from PIL import Image, ImageDraw, ImageFont

# Carica mappa base
img = Image.open('occultation_detail.png')
draw = ImageDraw.Draw(img)

# Converti coordinate RA/Dec → pixel
# (usa la stessa proiezione gnomonica)

# Disegna percorso in ROSSO
draw.line(path_coords, fill='#FF0000', width=3)

# Aggiungi fasce incertezza
draw.line(uncertainty_upper, fill='#FFA500', width=1)  # Arancione
draw.line(uncertainty_lower, fill='#FFA500', width=1)

# Salva
img.save('occultation_detail_complete.png')
```

#### Opzione C: Integrato nel Sistema
```cpp
// TODO: Implementare in OccultationChartBuilder
builder.setAsteroidPath(pathCoords);
builder.setUncertaintyBands(sigma1, sigma3);
builder.generateDetailChart(&config);
```

## Elementi della Mappa Finale

### Informazioni da Includere
1. **Titolo principale**
   - Nome asteroide
   - Nome stella
   - Data e ora UTC

2. **Percorso asteroide**
   - Linea centrale (ROSSO #FF0000, 2-3 px)
   - Fasce ±1σ (arancione, 1 px tratteggiato)
   - Fasce ±3σ (giallo, 1 px tratteggiato)

3. **Marker temporali**
   - Ogni 5-10 minuti lungo il percorso
   - Ora UTC o tempo relativo (T-X min)

4. **Informazioni tecniche** (in legenda o angolo)
   - Magnitudine asteroide
   - Magnitudine stella target (con SAO number se disponibile)
   - Drop atteso (Δmag)
   - Durata prevista
   - Velocità angolare

5. **Stelle di riferimento con SAO**
   - Etichetta stelle chiave vicino al percorso
   - SAO number mostrato per tutte le stelle fino a mag 14.0
   - Permette identificazione univoca e cross-check con cataloghi

## Ottimizzazione per Osservazione

### Preparazione Pre-Osservazione
1. **Stampa in B/N** su carta di qualità
2. **Plastifica** per protezione rugiada
3. **Identifica stelle chiave** a tavolino
4. **Annota SAO numbers** principali

### Durante l'Osservazione
- Usa torcia LED rossa per leggere
- La griglia fine aiuta il puntamento
- I SAO permettono identificazione certa
- Il campo panoramico mostra contesto

## Esempio Completo: Occultazione Betelgeuse

### Parametri Reali
```json
{
    "title": "Occultazione Betelgeuse - 2023-12-12",
    "subtitle": "Asteroide (319) Leona - Campo: 3°",
    
    "centerRA": 88.7929,    // α Ori (Betelgeuse)
    "centerDec": 7.4070,
    "fieldRadius": 3.0,
    
    "width": 2162,
    "height": 1184,
    "maxMagnitude": 16.0,
    
    "gridInterval": 0.25,
    
    "showSAONumbers": true,
    "saoMagnitudeLimit": 14.0,
    
    "outputPath": "betelgeuse_leona_2023"
}
```

### Risultato Atteso
- Campo centrato su Betelgeuse (mag 0.5, SAO 113271)
- Stelle di campo fino a mag 16
- SAO numbers mostrati per identificazione
- Griglia ogni 15' per puntamento preciso
- Base per sovrapposizione percorso ROSSO

### Post-Processing
1. Aggiungi percorso Leona in **ROSSO #FF0000** (direzione, velocità)
2. Marca timing critici lungo il percorso
3. Evidenzia stelle di riferimento con SAO numbers
4. Aggiungi note osservative specifiche
5. Verifica identificazione stelle con catalogo SAO

## Differenze dai Altri Tipi di Carta

| Caratteristica | Finder (30°) | Approach (20°) | Detail Betelgeuse (3°) |
|----------------|--------------|----------------|------------------------|
| **Scopo** | Localizzare area | Avvicinarsi | Osservazione diretta |
| **Formato** | Quadrato | Quadrato | Panoramico 16:9 |
| **Griglia** | 5° | 5° | 0.25° (15') |
| **Mag Limite** | 7.5 | 10.0 | 16.0 |
| **SAO** | Fino mag 6.5 | Fino mag 8.0 | Fino mag 14.0 |
| **Stelle** | Grandi, visibili | Medie | Piccole, dettagliate |
| **Percorso** | Non mostrato | Indicativo | Dettagliato ROSSO con timing |
| **Identificazione** | Nome stelle | SAO principali | SAO completo |

## File di Riferimento

- **Template:** `occultation_detail_betelgeuse_style.json`
- **Esempio:** `maps-betelgeuse-AC-white.jpg` (nella root del progetto)
- **Output:** `occultation_detail.png`

## Note Importanti

⚠️ **Percorso Asteroide:** Il sistema attualmente genera solo la mappa stellare di base. Il percorso dell'asteroide deve essere aggiunto manualmente o tramite script.

⚠️ **Orientamento:** Verifica che il percorso sia orientato in modo conveniente (tipicamente orizzontale per facilità lettura).

⚠️ **Timing:** I marker temporali sono critici per l'osservazione - calcola con precisione.

⚠️ **Coordinate:** Usa sempre J2000.0 e correggi per precessione se necessario.

## Sviluppi Futuri

Previsti miglioramenti al sistema:
- [ ] Generazione automatica percorso asteroide
- [ ] Calcolo effemeridi integrato
- [ ] Marker temporali automatici
- [ ] Export multi-formato (PNG, SVG, PDF)
- [ ] Annotazioni personalizzabili
- [ ] Template predefiniti per eventi comuni

---

**File di riferimento:** `maps-betelgeuse-AC-white.jpg` (2162×1184 px, 230 KB)  
**Configurazione:** `occultation_detail_betelgeuse_style.json`  
**Versione:** 1.0 - 2 Gennaio 2026
