# IOC_GaiaLib API Updates - Changelog

## Data: 28 novembre 2025

### 🎯 Nuove Features e API

#### 1. **GaiaLocalCatalog** - Full Offline Access
- **File header**: `<ioc_gaialib/gaia_local_catalog.h>`
- **Namespace**: `ioc_gaialib`
- **Scopo**: Accesso veloce a catalogo GRAPPA3E offline (~1.8B stelle Gaia DR3)
- **Data format**: HEALPix tiles (NSIDE=360), 52 bytes/record

**Metodi principali**:
```cpp
class GaiaLocalCatalog {
    // Cone search (compatibile con GaiaClient)
    std::vector<GaiaStar> queryCone(
        double ra_deg, double dec_deg, double radius_deg,
        size_t max_results = 0) const;
    
    // Cone search con filtro magnitudine (NEW)
    std::vector<GaiaStar> queryConeWithMagnitude(
        double ra_deg, double dec_deg, double radius_deg,
        double mag_min, double mag_max,
        size_t max_results = 0) const;
    
    // Query singola stella per source_id
    std::optional<GaiaStar> queryBySourceId(uint64_t source_id) const;
    
    // N stelle più brillanti in cono (NEW)
    std::vector<GaiaStar> queryBrightest(
        double ra_deg, double dec_deg, double radius_deg,
        size_t n_brightest) const;
    
    // Query rettangolare (box)
    std::vector<GaiaStar> queryBox(
        double ra_min, double ra_max,
        double dec_min, double dec_max,
        size_t max_results = 0) const;
    
    // Count veloce senza caricare dati (NEW)
    size_t countInCone(double ra_deg, double dec_deg,
                       double radius_deg) const;
    
    // Statistiche catalogo
    GaiaLocalStats getStatistics() const;
    bool isLoaded() const;
};
```

#### 2. **GrappaReader** - Asteroid Data Access
- **File header**: `<ioc_gaialib/grappa_reader.h>`
- **Namespace**: `ioc::gaia`
- **Scopo**: Lettura dati asteroidi GRAPPA3E (integrazione Gaia + dati fisici)

**Strutture**:
```cpp
struct AsteroidData {
    int64_t gaia_source_id;      ///< Gaia DR3 source_id
    int number;                  ///< Minor planet number
    std::string designation;     ///< Asteroid designation
    
    // Astrometric data (from Gaia)
    double ra, dec, parallax, pmra, pmdec;
    
    // Physical parameters
    double h_mag, g_slope, diameter_km, albedo, rotation_period_h;
    
    // Photometric data
    double phot_g_mean_mag, phot_bp_mean_mag, phot_rp_mean_mag;
    
    // Quality flags
    bool has_orbit, has_physical, has_rotation;
};

struct AsteroidQuery {
    // Spatial/magnitude/physical constraints with std::optional
    std::optional<double> ra_min, ra_max, dec_min, dec_max;
    std::optional<double> h_mag_min, h_mag_max;
    std::optional<double> diameter_min_km, diameter_max_km;
    std::optional<double> albedo_min, albedo_max;
    
    bool only_numbered;
    bool require_physical, require_rotation;
};
```

**Metodi GrappaReader**:
```cpp
class GrappaReader {
    std::optional<AsteroidData> queryBySourceId(int64_t source_id);
    std::optional<AsteroidData> queryByNumber(int number);
    std::optional<AsteroidData> queryByDesignation(const std::string&);
    std::vector<AsteroidData> queryCone(double ra, double dec, double radius);
    std::vector<AsteroidData> query(const AsteroidQuery& query);
    
    size_t getCount() const;
    struct Statistics { /* asteroid statistics */ };
    Statistics getStatistics() const;
    bool isLoaded() const;
};

// Integration helper
class GaiaAsteroidMatcher {
    bool isAsteroid(int64_t source_id);
    std::optional<AsteroidData> getAsteroidData(int64_t source_id);
    bool enrichStar(GaiaStar& star);  // Add asteroid info to star
};
```

#### 3. **Mag18CatalogV2 - Enhanced API**
- **File header**: `<ioc_gaialib/gaia_mag18_catalog_v2.h>`
- **Migliorie principali**:
  - HEALPix NSIDE=64 spatial index (100-300x più veloce)
  - Chunk-based compression (5x accesso più rapido)
  - Extended 80-byte record format (proper motion + quality flags)
  - Performance: 50 ms (0.5°) vs 15 sec (V1)

**Nuovi metodi**:
```cpp
class Mag18CatalogV2 {
    // NEW: Optimized cone search con filtro magnitudine
    std::vector<GaiaStar> queryConeWithMagnitude(
        double ra, double dec, double radius,
        double mag_min, double mag_max,
        size_t max_results = 0);
    
    // NEW: Get N brightest stars in cone
    std::vector<GaiaStar> queryBrightest(
        double ra, double dec, double radius,
        size_t num_stars);
    
    // NEW: Fast count without loading stars
    size_t countInCone(double ra, double dec, double radius);
    
    // NEW: Get HEALPix pixel information
    uint32_t getHEALPixPixel(double ra, double dec) const;
    std::vector<uint32_t> getPixelsInCone(
        double ra, double dec, double radius) const;
    
    // NEW: Extended record with full astrometric data
    std::optional<Mag18RecordV2> getExtendedRecord(uint64_t source_id);
    
    // NEW: Parallel processing control
    void setParallelProcessing(bool enable, size_t num_threads = 0);
    bool isParallelEnabled() const;
    size_t getNumThreads() const;
    
    // Existing methods still available
    std::optional<GaiaStar> queryBySourceId(uint64_t source_id);
    std::vector<GaiaStar> queryCone(double ra, double dec, double radius,
                                    size_t max_results = 0);
    uint64_t getTotalStars() const;
    double getMagLimit() const;
};
```

#### 4. **Types Enhancement**
- **File header**: `<ioc_gaialib/types.h>`
- **Nuove enums**:
```cpp
enum class GaiaRelease {
    DR2,   ///< Gaia Data Release 2 (2018)
    EDR3,  ///< Gaia Early Data Release 3 (2020)
    DR3    ///< Gaia Data Release 3 (2022)
};

// Date handling
struct JulianDate {
    static JulianDate J2000() { return JulianDate(2451545.0); }
    static JulianDate J2016() { return JulianDate(2457389.0); }
    double toYears() const;
};

// Query parameters
struct QueryParams {
    double ra_center, dec_center, radius;
    double max_magnitude;
    double min_parallax;
    // ... more fields
};
```

### ❌ API Deprecate/Rimosse

#### 1. **Direct GaiaMag18Catalog** (DEPRECATED)
- **Vecchio**: `class GaiaMag18Catalog`
- **Nuovo**: `class Mag18CatalogV2`
- **Motivo**: Mag18CatalogV2 supporta sia V2 che V1, con performance 100-300x migliore

**Conversione**:
```cpp
// OLD (deprecato)
ioc::gaia::GaiaMag18Catalog catalog;
auto stats = catalog.getStatistics();  // Returns struct

// NEW (consigliato)
ioc::gaia::Mag18CatalogV2 catalog;
catalog.open(path);
uint64_t total = catalog.getTotalStars();  // Direct access
```

#### 2. **Simple queryRegion()** (DEPRECATED)
- **Vecchio**: `queryCone(ra, dec, radius)` senza filtri
- **Nuovo**: `queryConeWithMagnitude(ra, dec, radius, mag_min, mag_max)`
- **Motivo**: Filtraggio server-side è 10x più veloce

**Conversione**:
```cpp
// OLD (deprecato) - filtraggio client-side
auto stars = catalog.queryCone(ra, dec, radius);
for (auto& star : stars) {
    if (star.phot_g_mean_mag <= 10.0) { ... }
}

// NEW (consigliato) - filtraggio built-in
auto stars = catalog.queryConeWithMagnitude(
    ra, dec, radius, -10.0, 10.0);  // mag filter on server
```

#### 3. **Unified GaiaCatalog Wrapper** (PROBLEMATIC)
- **Numero**: `class GaiaCatalog`
- **Issue**: Interfaccia generica non supporta nuove API
- **Soluzione**: Usare `Mag18CatalogV2` o `GaiaLocalCatalog` direttamente

### 📊 Catalogo Availability e Priorità

#### Supporto Cataloghi Offline (in ordine di priorità):
1. **GRAPPA3E** (~/catalogs/GRAPPA3E/)
   - 1.8 billion stars (full Gaia DR3)
   - Unlimited magnitude
   - HEALPix NSIDE=360
   - 146 GB uncompressed
   - **Use**: `GaiaLocalCatalog`

2. **Mag18 V2** (~/catalogs/gaia_mag18_v2.mag18v2)
   - ~100M stars (mag ≤ 18)
   - 14 GB
   - HEALPix NSIDE=64
   - Query time: 50-300 ms
   - **Use**: `Mag18CatalogV2` with `setParallelProcessing(true)`

3. **Mag18 V1** (~/catalogs/gaia_mag18.cat.gz) - LEGACY
   - ~100M stars (mag ≤ 18)
   - 9 GB compressed
   - Query time: 15+ seconds
   - **Use**: `Mag18CatalogV2` (auto-detects V1 format)
   - ⚠️ Performance warning

4. **Online TAP** (fallback)
   - ~1.7B stars (Gaia DR3)
   - Dynamic queries
   - Network required
   - ~1-5 second latency
   - **Use**: `GaiaClient::queryCone()`

### 🔧 StarMap Integration Updates

#### GaiaClient.cpp - Nuova Logica di Fallback:
```cpp
// Ordine di tentativo:
1. GRAPPA3E local catalog (fastest, most complete)
   └─ queryConeWithMagnitude() with filtered results
   └─ queryBox() for rectangular regions
   
2. Mag18 V2/V1 local catalog (fast, limited to mag ≤ 18)
   └─ queryConeWithMagnitude() with HEALPix index
   └─ queryBrightest() for top N stars
   └─ countInCone() for quick estimates
   
3. Online TAP query (slowest, most flexible)
   └─ GaiaClient::queryCone() with ADQL
   └─ Full Gaia DR3 data available
```

#### Parallelizzazione:
```cpp
// Auto-enable parallel processing for local catalogs
mag18Catalog_->setParallelProcessing(true, 0);  // 0 = auto-detect CPU cores

// Query performance expectations:
- GRAPPA3E: 5-50 ms (1.8B stars indexed)
- Mag18 V2: 50-300 ms (100M stars indexed)
- Mag18 V1: 15-60 sec (legacy, not recommended)
```

### 📈 Performance Improvements

| Operation | V1 (Legacy) | V2 (New) | Improvement |
|-----------|------------|---------|------------|
| Cone 0.5° | 15 sec | 50 ms | 300x |
| Cone 5° | 48 sec | 500 ms | 96x |
| Source by ID | <1 ms | <1 ms | Same |
| Magnitude filter | Client-side | Server-side | ~10x |
| Parallel queries | None | Auto | N/A |

### ⚠️ Breaking Changes

1. **GaiaMag18Catalog removed** (use Mag18CatalogV2)
2. **getStatistics()** returns different struct in new API
3. **queryCone()** no longer does magnitude filtering (use queryConeWithMagnitude)
4. **Constructor changes** for Mag18CatalogV2

### 🚀 Migration Checklist

- ✅ Update includes: `gaia_mag18_catalog.h` → `gaia_mag18_catalog_v2.h` + `gaia_local_catalog.h`
- ✅ Replace `GaiaMag18Catalog` with `Mag18CatalogV2`
- ✅ Use `queryConeWithMagnitude()` instead of filtering client-side
- ✅ Add `setParallelProcessing(true)` for best performance
- ✅ Try GRAPPA3E first, fallback to Mag18 V2, then online
- ✅ Update error handling for new optional returns

### 📚 Documentation References

- **GRAPPA3E Format**: https://ftp.imcce.fr/pub/catalogs/GRAPPA3E/
- **Gaia DR3**: https://www.cosmos.esa.int/web/gaia/dr3
- **HEALPix**: https://healpix.jpl.nasa.gov/

---

**Migration completed**: GaiaClient.cpp updated with full support for:
- ✅ GaiaLocalCatalog (GRAPPA3E)
- ✅ Mag18CatalogV2 (optimized queries)
- ✅ Parallel processing
- ✅ Intelligent fallback chain
