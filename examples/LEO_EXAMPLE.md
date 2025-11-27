# Esempio: Mappa Costellazione del Leone

Questo esempio dimostra come generare una mappa completa della costellazione del Leone (Leo) usando StarMap Library.

## 🌟 Caratteristiche

- **Centro**: Regolo (α Leonis), la stella più brillante del Leone
- **Campo**: 30° × 30° per includere tutta la costellazione
- **Risoluzione**: 4K (3840 × 2160 px)
- **Magnitudine**: Stelle fino a mag 12
- **Catalogo**: GAIA DR3 con numeri SAO
- **Proiezione**: Stereografica
- **Overlay**: Griglia equatoriale, equatore celeste, eclittica

## 🦁 La Costellazione del Leone

### Informazioni Generali
- **Nome latino**: Leo
- **Genitivo**: Leonis
- **Abbreviazione**: Leo
- **Dimensione**: 947 gradi² (12ª più grande)
- **Visibilità**: Latitudini +90° a -65°
- **Migliore periodo**: Marzo-Aprile (primavera boreale)
- **Costellazione zodiacale**: Sì (5° segno)

### Stelle Principali

| Nome | Designazione | RA | Dec | Magnitudine | Tipo Spettrale |
|------|-------------|-----|-----|-------------|----------------|
| **Regolo** | α Leo | 10h 08m 22s | +11° 58' 02" | 1.35 | B8 IVn |
| **Denebola** | β Leo | 11h 49m 04s | +14° 34' 19" | 2.14 | A3 V |
| **Algieba** | γ Leo | 10h 19m 58s | +19° 50' 30" | 2.01 | K0 IIIb + G7 IIIb |
| **Zosma** | δ Leo | 11h 14m 06s | +20° 31' 25" | 2.56 | A4 V |
| **Chort** | θ Leo | 11h 14m 14s | +15° 25' 46" | 3.34 | A2 V |

### Asterismi
- **La Falce**: Regolo, η, γ, ζ, μ, ε Leo (testa e criniera)
- **Il Trapezio**: Denebola, β, δ, θ Leo (corpo e coda)

### Oggetti Deep-Sky
- **M65** (NGC 3623): Galassia spirale (mag 10.2)
- **M66** (NGC 3627): Galassia spirale (mag 8.9)
- **M95** (NGC 3351): Galassia spirale barrata (mag 11.4)
- **M96** (NGC 3368): Galassia spirale (mag 10.1)
- **M105** (NGC 3379): Galassia ellittica (mag 10.2)
- **Tripletto di Leo**: M65, M66, NGC 3628

## 📋 Compilazione

```bash
# Da directory build
cd build
cmake ..
make example_leo

# Esegui
./examples/example_leo
```

## 🚀 Uso

### Metodo 1: Esecuzione Diretta

```bash
./examples/example_leo
```

Output:
```
╔══════════════════════════════════════════════════════════════╗
║        Mappa Costellazione del Leone (Leo)                 ║
╚══════════════════════════════════════════════════════════════╝

📐 Configurazione Mappa:
   Centro: Regolo (α Leonis)
   RA: 10h 08m 22.3s
   Dec: +11° 58' 01.9"
   Campo: 30.0° × 30.0°
   Risoluzione: 3840 × 2160 px
   Mag limite: 12.0

🌟 Query GAIA DR3 per Leone...
   ✓ Usando catalogo locale Mag18 V2 (query veloce)
   ✓ Trovate 45732 stelle
   📊 Cache: 1247 hits, 23 misses

⭐ Stelle Principali del Leone:

  🌟 Regolo (α Leo)
  • SAO 98967 / GAIA DR3 3880785530720066560 | Mag: 1.36 | RA: 152.096° Dec: 11.967° | B-V: -0.09

  🌟 Denebola (β Leo)
  • SAO 99809 / GAIA DR3 3880656578147265280 | Mag: 2.14 | RA: 177.265° Dec: 14.572° | B-V: 0.09
  ...

🎨 Generazione mappa...
   ✓ Mappa renderizzata (3840 × 2160 px)

💾 Salvataggio mappa...
   ✓ Mappa salvata: leo_constellation_map.png
```

### Metodo 2: Configurazione JSON

```bash
./examples/example_json config_examples/leo.json
```

## 🎨 Personalizzazione

Modifica `config_examples/leo.json`:

```json
{
  "center": {
    "ra": 152.0958,
    "dec": 11.9672
  },
  "field_of_view": {
    "width_degrees": 30.0,
    "height_degrees": 30.0
  },
  "stars": {
    "limiting_magnitude": 12.0,
    "color_by_spectral_type": true
  }
}
```

### Variazioni Interessanti

#### Focus su Regolo (zoom)
```json
"field_of_view": {
  "width_degrees": 5.0,
  "height_degrees": 5.0
}
```

#### Include galassie (stelle più deboli)
```json
"stars": {
  "limiting_magnitude": 15.0
}
```

#### Solo stelle luminose (visibili ad occhio nudo)
```json
"stars": {
  "limiting_magnitude": 6.0,
  "show_sao_numbers": true
}
```

## 🔭 Regolo (α Leonis)

### Caratteristiche Fisiche
- **Tipo**: Sistema multiplo (4 componenti)
- **Componente principale**: Gigante blu-bianca (B8 IVn)
- **Magnitudine**: 1.35 (21ª stella più luminosa)
- **Distanza**: ~79 anni luce
- **Temperatura**: ~12,000 K
- **Massa**: ~3.5 masse solari
- **Raggio**: ~3.5 raggi solari
- **Luminosità**: ~350 volte il Sole

### Rotazione Estrema
- **Velocità**: ~316 km/s (all'equatore)
- **Periodo**: ~15.9 ore
- **Forma**: Oblata (schiacciata ai poli)
- **Gravità**: Ridotta del 50% all'equatore vs poli
- **Vicina al limite**: ~96% della velocità di rottura

### Posizione Speciale
- **Eclittica**: Solo 0.46° a nord
- **Occultazioni**: Frequenti da Luna e pianeti
- **Punto vernale**: Era vicina ~2000 anni fa
- **Precessione**: Si allontana gradualmente

## 📚 Mitologia

### Grecia Antica
Il Leone rappresenta il **Leone di Nemea**, una bestia invincibile uccisa da Eracle come prima delle sue dodici fatiche. La pelle del leone era impenetrabile, quindi Eracle lo strangolò a mani nude.

### Babilonia
Associato al Sole e al fuoco, simbolo di regalità.

### Egitto
Il Nilo si gonfiava quando il Sole entrava nel Leone, da cui l'associazione con l'acqua.

## 🌌 Osservazione

### Periodo Migliore
- **Emisfero Nord**: Marzo-Aprile (alto nel cielo serale)
- **Culminazione**: ~21:00 ora locale a inizio aprile
- **Altitudine massima**: Dipende dalla latitudine (culmina al meridiano)

### Come Trovarlo
1. Trova l'Orsa Maggiore (Grande Carro)
2. Segui la curva del manico fino ad Arturo (Boote)
3. Prosegui verso sud-ovest fino a Spica (Vergine)
4. Il Leone è a nord di Spica
5. La "Falce" è facilmente riconoscibile come punto interrogativo rovesciato

### Binocolo/Telescopio
- **M65/M66**: Galassie visibili con piccolo telescopio
- **M95/M96**: Richiedono cieli bui
- **Tripletto di Leo**: Sfida per astrofotografia

## 💻 Codice Rilevante

```cpp
// Centro su Regolo
config.center = core::EquatorialCoordinates(152.0958, 11.9672);

// Campo ampio
config.fieldOfViewWidth = 30.0;
config.fieldOfViewHeight = 30.0;

// Alta risoluzione
config.imageWidth = 3840;
config.imageHeight = 2160;

// Include stelle deboli
config.limitingMagnitude = 12.0;

// Query con IOC_GaiaLib
catalog::GaiaQueryParameters queryParams;
queryParams.center = config.center;
queryParams.radiusDegrees = 15.0;
auto stars = catalogMgr.queryStars(queryParams, true);
```

## 📊 Performance

Con catalogo Mag18 V2 locale:
- **Query time**: ~50-100 ms per 30° × 30°
- **Stelle trovate**: ~45,000 (mag ≤ 12)
- **Rendering time**: ~1-2 secondi (4K, 45k stelle)
- **Memoria**: ~200 MB per buffer 4K RGBA

Senza catalogo locale (online):
- **Query time**: ~10-30 secondi
- **Rate limit**: 10 query/min (GAIA ESA)

## 🔗 Riferimenti

- [Costellazione del Leone (Wikipedia)](https://it.wikipedia.org/wiki/Leone_(costellazione))
- [Regolo (SIMBAD)](http://simbad.u-strasbg.fr/simbad/sim-id?Ident=Regulus)
- [IAU Constellation Boundaries](https://www.iau.org/public/themes/constellations/)
- [GAIA Archive](https://gea.esac.esa.int/archive/)

## 📝 Note

- La mappa include automaticamente numeri SAO per stelle luminose (mag < 6)
- Usa catalogo locale Mag18 V2 se disponibile in `~/catalogs/gaia_mag18_v2.cat`
- Fallback automatico a query online se catalogo non disponibile
- Output PNG richiede stb_image_write (header-only, auto-download)
- Colori stelle basati su indice B-V (spettro realistico)

---

**Autore**: IOC_StarMap Library  
**Versione**: 1.0.0  
**Data**: 27 novembre 2025
