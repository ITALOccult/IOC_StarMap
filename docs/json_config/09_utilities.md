# Utilità e Reference

Strumenti, conversioni, validazione e troubleshooting.

## Conversione Colori

### RGB → uint32 RGBA

Formato: RGBA dove ogni canale è 8-bit (0-255)

```python
def rgb_to_uint32(r, g, b, a=255):
    """Converte RGB(A) a uint32"""
    return (r << 24) | (g << 16) | (b << 8) | a

# Esempi
nero = rgb_to_uint32(0, 0, 0)                    # 255
bianco = rgb_to_uint32(255, 255, 255)            # 4294967295
rosso = rgb_to_uint32(255, 0, 0)                 # 4278190335
verde = rgb_to_uint32(0, 255, 0)                 # 16711935
blu = rgb_to_uint32(0, 0, 255)                   # 65535
blu_scuro = rgb_to_uint32(0, 0, 128)             # 32895
grigio = rgb_to_uint32(128, 128, 128)            # 2155905279
arancio = rgb_to_uint32(255, 165, 0)             # 4289003520
azzurro = rgb_to_uint32(135, 206, 235)           # 2271168767
rosso_chiaro = rgb_to_uint32(255, 200, 200)      # 4291346687
```

### uint32 RGBA → RGB (inversa)

```python
def uint32_to_rgb(color):
    """Estrae RGB da uint32"""
    r = (color >> 24) & 0xFF
    g = (color >> 16) & 0xFF
    b = (color >> 8) & 0xFF
    a = color & 0xFF
    return (r, g, b, a)

# Esempio
rosso_uint32 = 4278190335
r, g, b, a = uint32_to_rgb(rosso_uint32)
print(f"RGB({r}, {g}, {b}, {a})")  # RGB(255, 0, 0, 255)
```

### Trasparenza (Alpha Channel)

```python
def rgb_with_alpha(r, g, b, alpha):
    """Aggiunge trasparenza (0=trasparente, 255=opaco)"""
    return (r << 24) | (g << 16) | (b << 8) | alpha

# Esempi
rosso_opaco = rgb_with_alpha(255, 0, 0, 255)     # Rosso pieno
rosso_trasparente = rgb_with_alpha(255, 0, 0, 128)  # Rosso 50%
rosso_quasi_trasparente = rgb_with_alpha(255, 0, 0, 32)   # Rosso 12%
```

### Tool Online

- [Color Hex](https://www.color-hex.com/) - Conversione HEX/RGB
- [Color picker](https://htmlcolorcodes.com/) - Selettore colori interattivo
- [RGB to Hex](https://www.rapidtables.com/convert/color/rgb-to-hex.html)

---

## Validazione Parametri

La libreria valida automaticamente i parametri al caricamento JSON.

### Errori di Validazione

| Parametro | Range | Errore se |
|-----------|-------|-----------|
| `center.ra` | 0-360 | < 0 o > 360 |
| `center.dec` | -90 a +90 | < -90 o > +90 |
| `field_of_view` | > 0 | <= 0 |
| `image.width/height` | > 0 | <= 0 |
| `limiting_magnitude` | -2 a +18 | fuori range |
| `performance.max_stars` | > 0 | <= 0 |
| `performance.star_batch_size` | > 0 | <= 0 |
| `compass.size` | > 0 | <= 0 |
| `scale.length` | > 0 | <= 0 |

### Validazione Manuale (C++)

```cpp
#include <starmap/config/Validator.h>

auto config = loader.load("config.json");

// Valida automaticamente, genera eccezione se errore
try {
    validator.validate(config);
} catch (const starmap::ConfigException& e) {
    std::cerr << "Errore configurazione: " << e.what() << std::endl;
    return 1;
}
```

---

## Coordinate Astronomiche

### Conversione RA (ore) ↔ gradi

Ascensione Retta in ore (0-24h):
```
gradi = ore × 15
ore = gradi / 15
```

Esempi:
```
0h = 0°           Punto vernale
6h = 90°
12h = 180°
18h = 270°
24h = 360° (=0°)
```

### Conversione RA in Arcsec

```
arcsec = ore × 3600 × 15
arcsec = gradi × 3600
```

Esempio: 1 arcmin RA = 4 secondi di tempo
```
1 arcmin = 4 sec tempo
1 arcsec = 0.0667 sec tempo
```

### Declinazione

- Intervallo: -90° (polo sud) a +90° (polo nord)
- 0° = equatore celeste
- Positivo = nord, negativo = sud

---

## Troubleshooting

### Mappa Completamente Bianca/Vuota

**Cause possibili**:

1. **Magnitudine limite troppo bassa**
   ```json
   "limiting_magnitude": 2.0  // Troppo poche stelle
   ```
   Soluzione: Aumentare a 8-14

2. **Coordinate non valide**
   ```json
   "center": {"ra": 400, "dec": 150}  // Fuori range
   ```
   Soluzione: RA 0-360, Dec -90 a +90

3. **FOV troppo piccolo**
   ```json
   "field_of_view": {"width": 0.01, "height": 0.01}  // 0.6 arcmin
   ```
   Soluzione: Aumentare a 1-10° per test

4. **Database stelle non caricato**
   Controllare: Gaia DR3, SAO disponibili nel build

### Stella Non Etichettata

Controllare parametri:
```json
"stars": {
  "show_names": true,  // Deve essere true
  "min_magnitude_for_label": 8.0  // Stella > 8 non etichettata
}
```

Se stella ha magnitudine > 8, aumentare limite:
```json
"min_magnitude_for_label": 10.0
```

### Overlay Non Visibile

**Rettangoli**:
```json
"overlay_rectangles": [
  {
    "enabled": true,  // Deve essere true
    "color": 255      // Nero su nero non visibile
  }
]
```

Soluzione: Usare colore contrastante con background.

**Path**:
```json
"overlay_paths": [
  {
    "enabled": true,
    "points": []  // Nessun punto = niente
  }
]
```

Soluzione: Aggiungere almeno 2 punti.

### Prestazioni Lente

Problemi:
1. `limiting_magnitude` troppo alta (molte stelle)
2. `image.width/height` molto grandi
3. `performance.max_stars` superiore a memoria

Soluzione:
```json
{
  "limiting_magnitude": 11.0,  // Ridurre
  "image": {"width": 1920, "height": 1920},  // Meno pixel
  "performance": {
    "max_stars": 30000,  // Meno stelle
    "star_batch_size": 3000
  }
}
```

---

## Checklist Configurazione

- [ ] Coordinate valide (RA 0-360, Dec -90 a +90)
- [ ] Dimensioni immagine > 0
- [ ] FOV > 0
- [ ] Magnitudine ragionevole (-2 a 18)
- [ ] Colori uint32 validi
- [ ] Proiezione supportata
- [ ] Overlay coordinate entro FOV
- [ ] Path ha almeno 2 punti
- [ ] File JSON sintassi corretta
- [ ] Database stelle caricato

---

## Parametri Predefiniti

**Configurazione Minima**:
```json
{
  "center": {"ra": 0, "dec": 0},
  "field_of_view": {"width": 10, "height": 10},
  "image": {"width": 1920, "height": 1920}
}
```

**Configurazione Standard**:
```json
{
  "center": {"ra": 83.75, "dec": 5.0},
  "field_of_view": {"width": 10, "height": 10},
  "image": {"width": 2400, "height": 2400},
  "limiting_magnitude": 11.0,
  "title": {"enabled": true, "text": "My Map"},
  "border": {"enabled": true},
  "compass": {"enabled": true},
  "scale": {"enabled": true}
}
```

**Configurazione Scientifica**:
```json
{
  "center": {"ra": 180, "dec": 0},
  "field_of_view": {"width": 5, "height": 5},
  "image": {"width": 3000, "height": 3000},
  "limiting_magnitude": 14.0,
  "projection": {
    "type": "gnomonic",
    "coordinate_system": "equatorial"
  },
  "stars": {"use_spectral_colors": true},
  "grid": {"ra_step": 0.5, "dec_step": 0.5}
}
```

---

## FAQ

**Q: Qual è il FOV massimo?**
A: Teoricamente ilimitato, ma > 180° non ha senso per stellari.

**Q: Quante stelle posso mostrare?**
A: Default 50000, configurabile in `performance.max_stars`.

**Q: I colori delle stelle sono accurati?**
A: Sì, basati su indice colore B-V Gaia DR3.

**Q: Posso usare coordinate galattiche?**
A: Sì, usa `coordinate_system: "galactic"`.

**Q: Come aggiungo l'orario dell'osservazione?**
A: Aggiungi nella `title.text` o usa `observation.time`.

---

[← Esempi](08_examples.md) | [Indice](../JSON_CONFIGURATION.md)
