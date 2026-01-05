# Configurazione Base

Parametri fondamentali per definire il campo visivo e le proprietà geometriche della mappa.

## Centro e Campo Visivo

```json
{
  "center": {
    "ra": 83.75,
    "dec": 5.0
  },
  "field_of_view": {
    "width": 10.0,
    "height": 10.0
  }
}
```

| Campo | Tipo | Range | Descrizione |
|-------|------|-------|-------------|
| `center.ra` | double | 0-360 | Ascensione Retta del centro (gradi) |
| `center.dec` | double | -90 a +90 | Declinazione del centro (gradi) |
| `field_of_view.width` | double | >0 | Larghezza campo in gradi |
| `field_of_view.height` | double | >0 | Altezza campo in gradi |

**Conversione RA da ore a gradi**:
```
RA_gradi = RA_ore × 15
Esempio: 5h 35m 17.3s = 83.822°
```

---

## Dimensioni Immagine

```json
{
  "image": {
    "width": 1920,
    "height": 1080
  }
}
```

| Campo | Tipo | Range | Descrizione |
|-------|------|-------|-------------|
| `image.width` | int | >0 | Larghezza immagine in pixel |
| `image.height` | int | >0 | Altezza immagine in pixel |

**Raccomandazioni**:
- Print: 3000×3000 o superiore
- Web: 1920×1080 o 2400×2400
- Anteprima: 800×600

**Risoluzione angolare**:
```
arcsec_per_pixel = 3600 × FOV_deg / size_px
```

Esempio: Campo 10° in 2400px → 15"/pixel

---

## Proiezione

```json
{
  "projection": {
    "type": "stereographic",
    "coordinate_system": "equatorial"
  }
}
```

### Tipi di Proiezione

| Valore | Descrizione | Uso Tipico |
|--------|-------------|------------|
| `stereographic` | Proiezione stereografica | Standard per mappe celesti |
| `gnomonic` | Proiezione gnomica | Cerchi massimi = linee rette |
| `orthographic` | Proiezione ortografica | Vista dall'infinito |
| `mercator` | Proiezione di Mercatore | Mappe globali |
| `azimuthal_equidistant` | Azimutale equidistante | Osservazione locale |

**Proprietà**:

- **Stereographic**: Conforme (preserva angoli), cerchi celesti → cerchi nell'immagine
- **Gnomonic**: Cerchi massimi → linee rette, distorsione radiale forte
- **Orthographic**: Vista sferica dall'infinito, FOV max 180°
- **Mercator**: Conforme, forte distorsione ai poli
- **Azimuthal Equidistant**: Distanze dal centro preservate

### Sistemi di Coordinate

| Valore | Descrizione |
|--------|-------------|
| `equatorial` | RA/Dec (J2000) - Standard astronomico |
| `galactic` | l/b (coordinate galattiche) |
| `ecliptic` | Coordinate eclittiche |
| `horizontal` | Alt/Az (richiede observer location) |

**Equatorial**: Sistema standard J2000.0, origine al punto vernale

**Galactic**: Utile per struttura galattica, centro galattico a (l=0°, b=0°)

**Ecliptic**: Piano dell'orbita terrestre, inclinato 23.44° rispetto all'equatore

**Horizontal**: Richiede posizione osservatore e tempo, dipende da location

---

## Magnitudine Limite

```json
{
  "limiting_magnitude": 12.0
}
```

| Campo | Tipo | Range | Descrizione |
|-------|------|-------|-------------|
| `limiting_magnitude` | double | -2 a 18 | Magnitudine più debole visibile |

**Raccomandazioni**:

| Tipo Carta | Magnitudine | Stelle/deg² | Uso |
|------------|-------------|-------------|-----|
| Naked eye | 6-7 | ~1 | Osservazione a occhio nudo |
| Binocolo | 8-10 | 5-50 | Finder chart |
| Telescopio | 11-13 | 100-1000 | Astrometria, occultazioni |
| Survey | 14-16 | 2000-20000 | Ricerca scientifica |

**Attenzione**: Magnitudini >13 generano molte stelle. Usa parametri `performance` per controllare memoria (vedi [Performance](02_performance.md)).

**Numero stelle atteso** (approssimazione):
```
log10(N) ≈ 0.6×m - 3.2
Per m=12: N ≈ 10,000 stelle per steradiante
```

---

## Osservatore

```json
{
  "observation": {
    "time": "2026-01-05T20:00:00Z",
    "latitude": 45.0,
    "longitude": 10.0
  }
}
```

| Campo | Tipo | Descrizione |
|-------|------|-------------|
| `time` | string | Timestamp ISO 8601 (UTC) |
| `latitude` | double | Latitudine osservatore in gradi (-90 a +90) |
| `longitude` | double | Longitudine osservatore in gradi (-180 a +180) |

**Richiesto solo per**:
- Coordinate `horizontal` (Alt/Az)
- Calcolo posizioni precise dipendenti dal tempo
- Precessione a epoca diversa da J2000

**Formato tempo**:
```
YYYY-MM-DDTHH:MM:SSZ
Esempio: 2026-01-05T20:00:00Z
```

---

## Esempio Configurazione Base

```json
{
  "center": {
    "ra": 83.822,
    "dec": -5.391
  },
  "field_of_view": {
    "width": 15.0,
    "height": 15.0
  },
  "image": {
    "width": 3000,
    "height": 3000
  },
  "projection": {
    "type": "stereographic",
    "coordinate_system": "equatorial"
  },
  "limiting_magnitude": 11.0,
  "background_color": 255
}
```

**Risultato**:
- Centro: Betelgeuse (Orione)
- Campo: 15° × 15°
- Risoluzione: 18"/pixel
- Stelle attese: ~22,500
- Sfondo: Nero

---

[← Indice](../JSON_CONFIGURATION.md) | [Prossimo: Performance →](02_performance.md)
