#!/usr/bin/env python3
"""
Script per generare database di cross-match Gaia DR3 - SAO

Questo script scarica i dati dal catalogo SAO via VizieR e trova i corrispondenti
source_id Gaia DR3, creando un database SQLite per lookup veloci.

Requisiti:
    pip install astroquery astropy

Uso:
    python build_gaia_sao_database.py [--output gaia_sao_xmatch.db] [--max-magnitude 9.0]
"""

import sqlite3
import argparse
import sys
from pathlib import Path
from typing import List, Tuple, Optional
import time

try:
    from astroquery.vizier import Vizier
    from astroquery.gaia import Gaia
    from astropy.coordinates import SkyCoord
    from astropy import units as u
    import numpy as np
except ImportError:
    print("ERROR: Requisiti mancanti. Installa con:")
    print("  pip install astroquery astropy")
    sys.exit(1)


class GaiaSAODatabaseBuilder:
    """Costruttore del database Gaia-SAO cross-match"""
    
    def __init__(self, db_path: str, max_magnitude: float = 9.0):
        self.db_path = db_path
        self.max_magnitude = max_magnitude
        self.conn = None
        self.cursor = None
        
    def create_database(self):
        """Crea database SQLite con schema"""
        print(f"Creazione database: {self.db_path}")
        
        # Rimuovi database esistente se presente
        if Path(self.db_path).exists():
            Path(self.db_path).unlink()
            
        self.conn = sqlite3.connect(self.db_path)
        self.cursor = self.conn.cursor()
        
        # Schema
        self.cursor.execute("""
            CREATE TABLE gaia_sao_xmatch (
                gaia_source_id INTEGER PRIMARY KEY,
                sao_number INTEGER NOT NULL,
                ra REAL NOT NULL,
                dec REAL NOT NULL,
                magnitude REAL,
                separation REAL,
                created_at TEXT DEFAULT CURRENT_TIMESTAMP
            )
        """)
        
        # Metadata
        self.cursor.execute("""
            CREATE TABLE metadata (
                key TEXT PRIMARY KEY,
                value TEXT
            )
        """)
        
        self.cursor.execute("INSERT INTO metadata VALUES ('version', '1.0')")
        self.cursor.execute("INSERT INTO metadata VALUES ('created', datetime('now'))")
        self.cursor.execute("INSERT INTO metadata VALUES ('max_magnitude', ?)", 
                          (str(self.max_magnitude),))
        
        self.conn.commit()
        print("Database creato con successo")
        
    def fetch_sao_catalog(self) -> List[Tuple]:
        """Scarica catalogo SAO da VizieR"""
        print(f"Scaricamento catalogo SAO (mag < {self.max_magnitude})...")
        
        # Configura VizieR
        v = Vizier(columns=['SAO', '_RAJ2000', '_DEJ2000', 'Vmag'], 
                   row_limit=-1,  # Tutti i risultati
                   catalog='I/131A/sao')
        
        # Query tutto il catalogo con filtro magnitudine
        v.ROW_LIMIT = -1
        
        try:
            result = v.query_constraints(catalog='I/131A/sao', Vmag=f'<{self.max_magnitude}')
            
            if not result or len(result) == 0:
                print("ERRORE: Nessun dato ricevuto da VizieR")
                return []
                
            table = result[0]
            print(f"Ricevute {len(table)} stelle dal catalogo SAO")
            
            # Converti in lista di tuple
            stars = []
            for row in table:
                sao_num = int(row['SAO'])
                ra = float(row['_RAJ2000'])
                dec = float(row['_DEJ2000'])
                vmag = float(row['Vmag']) if row['Vmag'] else 99.0
                
                stars.append((sao_num, ra, dec, vmag))
                
            return stars
            
        except Exception as e:
            print(f"ERRORE durante query VizieR: {e}")
            return []
    
    def find_gaia_match(self, ra: float, dec: float, radius_arcsec: float = 5.0) -> Optional[Tuple[int, float]]:
        """
        Trova match Gaia DR3 per coordinate date
        
        Returns:
            (gaia_source_id, separation_arcsec) se trovato, altrimenti None
        """
        try:
            coord = SkyCoord(ra=ra*u.degree, dec=dec*u.degree, frame='icrs')
            
            # Query Gaia cone search
            job = Gaia.cone_search_async(coord, radius=radius_arcsec*u.arcsec, 
                                        output_format='votable')
            results = job.get_results()
            
            if results is None or len(results) == 0:
                return None
            
            # Prendi il match più vicino
            best_match = results[0]
            source_id = int(best_match['SOURCE_ID'])
            
            # Calcola separazione
            match_coord = SkyCoord(ra=best_match['ra']*u.degree, 
                                  dec=best_match['dec']*u.degree, 
                                  frame='icrs')
            separation = coord.separation(match_coord).arcsec
            
            return (source_id, separation)
            
        except Exception as e:
            # Errore nella query (timeout, etc)
            return None
    
    def build_cross_match(self, sao_stars: List[Tuple], batch_size: int = 100):
        """
        Costruisce cross-match tra SAO e Gaia
        
        Args:
            sao_stars: Lista di (sao_number, ra, dec, vmag)
            batch_size: Dimensione batch per inserimenti
        """
        print(f"\nCross-matching {len(sao_stars)} stelle SAO con Gaia DR3...")
        print("Questo può richiedere diversi minuti...\n")
        
        matched = 0
        failed = 0
        batch = []
        
        for i, (sao_num, ra, dec, vmag) in enumerate(sao_stars):
            # Progress
            if (i + 1) % 100 == 0:
                print(f"Progresso: {i+1}/{len(sao_stars)} ({matched} matched, {failed} failed)")
            
            # Trova match Gaia
            gaia_match = self.find_gaia_match(ra, dec)
            
            if gaia_match:
                gaia_id, separation = gaia_match
                batch.append((gaia_id, sao_num, ra, dec, vmag, separation))
                matched += 1
                
                # Inserisci batch quando pronto
                if len(batch) >= batch_size:
                    self._insert_batch(batch)
                    batch = []
            else:
                failed += 1
            
            # Rate limiting per evitare sovraccarico server
            if (i + 1) % 50 == 0:
                time.sleep(1)
        
        # Inserisci batch rimanente
        if batch:
            self._insert_batch(batch)
        
        print(f"\nCross-match completato:")
        print(f"  Matched: {matched}")
        print(f"  Failed: {failed}")
        print(f"  Success rate: {100.0 * matched / len(sao_stars):.1f}%")
        
    def _insert_batch(self, batch: List[Tuple]):
        """Inserisce batch di entry nel database"""
        self.cursor.executemany("""
            INSERT OR REPLACE INTO gaia_sao_xmatch 
            (gaia_source_id, sao_number, ra, dec, magnitude, separation)
            VALUES (?, ?, ?, ?, ?, ?)
        """, batch)
        self.conn.commit()
        
    def create_indices(self):
        """Crea indici per performance"""
        print("\nCreazione indici...")
        
        self.cursor.execute("CREATE INDEX IF NOT EXISTS idx_sao_number ON gaia_sao_xmatch(sao_number)")
        self.cursor.execute("CREATE INDEX IF NOT EXISTS idx_ra ON gaia_sao_xmatch(ra)")
        self.cursor.execute("CREATE INDEX IF NOT EXISTS idx_dec ON gaia_sao_xmatch(dec)")
        self.cursor.execute("CREATE INDEX IF NOT EXISTS idx_magnitude ON gaia_sao_xmatch(magnitude)")
        self.cursor.execute("CREATE INDEX IF NOT EXISTS idx_ra_dec ON gaia_sao_xmatch(ra, dec)")
        
        self.conn.commit()
        print("Indici creati")
        
    def optimize(self):
        """Ottimizza database"""
        print("\nOttimizzazione database...")
        self.cursor.execute("VACUUM")
        self.cursor.execute("ANALYZE")
        self.conn.commit()
        print("Database ottimizzato")
        
    def print_statistics(self):
        """Stampa statistiche database"""
        print("\n" + "="*50)
        print("STATISTICHE DATABASE")
        print("="*50)
        
        # Conta entry
        self.cursor.execute("SELECT COUNT(*) FROM gaia_sao_xmatch")
        count = self.cursor.fetchone()[0]
        print(f"Totale entry: {count:,}")
        
        # Range magnitudini
        self.cursor.execute("""
            SELECT MIN(magnitude), MAX(magnitude), AVG(magnitude) 
            FROM gaia_sao_xmatch
        """)
        min_mag, max_mag, avg_mag = self.cursor.fetchone()
        print(f"Range magnitudini: {min_mag:.2f} - {max_mag:.2f} (media: {avg_mag:.2f})")
        
        # Separazione media
        self.cursor.execute("SELECT AVG(separation), MAX(separation) FROM gaia_sao_xmatch")
        avg_sep, max_sep = self.cursor.fetchone()
        print(f"Separazione media: {avg_sep:.2f}\" (max: {max_sep:.2f}\")")
        
        # Dimensione file
        db_size_mb = Path(self.db_path).stat().st_size / (1024 * 1024)
        print(f"Dimensione database: {db_size_mb:.2f} MB")
        
        print("="*50 + "\n")
        
    def close(self):
        """Chiudi connessione database"""
        if self.conn:
            self.conn.close()


def main():
    parser = argparse.ArgumentParser(
        description='Costruisce database cross-match Gaia DR3 - SAO'
    )
    parser.add_argument('--output', '-o', 
                       default='gaia_sao_xmatch.db',
                       help='Path file database output (default: gaia_sao_xmatch.db)')
    parser.add_argument('--max-magnitude', '-m',
                       type=float,
                       default=9.0,
                       help='Magnitudine massima stelle SAO da includere (default: 9.0)')
    parser.add_argument('--test',
                       action='store_true',
                       help='Modalità test: processa solo prime 100 stelle')
    
    args = parser.parse_args()
    
    print("="*60)
    print("GAIA-SAO CROSS-MATCH DATABASE BUILDER")
    print("="*60)
    print()
    
    builder = GaiaSAODatabaseBuilder(args.output, args.max_magnitude)
    
    try:
        # Crea database
        builder.create_database()
        
        # Scarica catalogo SAO
        sao_stars = builder.fetch_sao_catalog()
        
        if not sao_stars:
            print("ERRORE: Impossibile scaricare catalogo SAO")
            return 1
        
        # Modalità test
        if args.test:
            print("\n*** MODALITÀ TEST: processing solo 100 stelle ***\n")
            sao_stars = sao_stars[:100]
        
        # Cross-match
        builder.build_cross_match(sao_stars)
        
        # Crea indici
        builder.create_indices()
        
        # Ottimizza
        builder.optimize()
        
        # Statistiche
        builder.print_statistics()
        
        print(f"Database salvato in: {args.output}")
        print("\nPer usare il database, copia il file nella directory del progetto")
        print("o specifica il path nel costruttore di GaiaSAODatabase.")
        
        return 0
        
    except KeyboardInterrupt:
        print("\n\nInterrotto dall'utente")
        return 1
        
    except Exception as e:
        print(f"\nERRORE: {e}")
        import traceback
        traceback.print_exc()
        return 1
        
    finally:
        builder.close()


if __name__ == '__main__':
    sys.exit(main())
