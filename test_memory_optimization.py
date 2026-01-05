#!/usr/bin/env python3
"""
Test script per verificare l'ottimizzazione memoria
Genera config JSON con vari livelli di complessità e misura performance
"""

import json
import subprocess
import time
import os
from pathlib import Path

def create_test_config(name, ra, dec, fov, mag, max_stars=50000, batch_size=5000):
    """Crea un file di configurazione JSON per test"""
    config = {
        "center": {"ra": ra, "dec": dec},
        "field_of_view": {"width": fov, "height": fov},
        "image": {"width": 1920, "height": 1080},
        "projection": {"type": "stereographic", "coordinate_system": "equatorial"},
        "limiting_magnitude": mag,
        "performance": {
            "max_stars": max_stars,
            "star_batch_size": batch_size
        },
        "orientation": {"rotation_angle": 0.0, "north_up": True, "east_left": True},
        "grid": {"enabled": True, "ra_step": 2.0, "dec_step": 2.0},
        "stars": {
            "use_spectral_colors": True,
            "show_names": True,
            "show_sao_numbers": True
        },
        "display": {
            "show_title": True,
            "title": f"{name} - FOV {fov}° mag {mag}"
        }
    }
    
    filename = f"/tmp/test_{name}.json"
    with open(filename, 'w') as f:
        json.dump(config, f, indent=2)
    
    return filename

def run_test(config_file, output_name):
    """Esegue generate_chart e misura il tempo"""
    output_file = f"/tmp/{output_name}.png"
    
    start = time.time()
    try:
        result = subprocess.run(
            ["./generate_chart", config_file],
            cwd="build/examples",
            capture_output=True,
            text=True,
            timeout=60
        )
        elapsed = time.time() - start
        
        # Estrai numero stelle dal output
        stars_count = "N/A"
        for line in result.stdout.split('\n'):
            if "stelle nel rettangolo" in line.lower():
                parts = line.split()
                for i, part in enumerate(parts):
                    if part.isdigit():
                        stars_count = part
                        break
        
        success = result.returncode == 0
        return {
            "success": success,
            "elapsed": elapsed,
            "stars": stars_count,
            "output": output_file if success else None,
            "error": result.stderr if not success else None
        }
    except subprocess.TimeoutExpired:
        return {
            "success": False,
            "elapsed": 60,
            "stars": "TIMEOUT",
            "output": None,
            "error": "Timeout dopo 60s"
        }
    except Exception as e:
        return {
            "success": False,
            "elapsed": time.time() - start,
            "stars": "ERROR",
            "output": None,
            "error": str(e)
        }

def main():
    print("=" * 80)
    print("TEST OTTIMIZZAZIONE MEMORIA - StarMap Library")
    print("=" * 80)
    print()
    
    # Check se il binario esiste
    if not os.path.exists("build/examples/generate_chart"):
        print("❌ ERRORE: build/examples/generate_chart non trovato")
        print("   Compila prima con: cd build && make -j4")
        return
    
    tests = [
        # (nome, ra, dec, fov, mag, max_stars, batch)
        ("small_field", 83.75, 5.0, 5.0, 10.0, 50000, 5000),
        ("medium_field", 83.75, 5.0, 10.0, 11.0, 50000, 5000),
        ("large_field", 83.75, 5.0, 15.0, 12.0, 50000, 5000),
        ("xlarge_field", 83.75, 5.0, 20.0, 12.5, 50000, 5000),
        ("high_mag", 83.75, 5.0, 10.0, 13.0, 75000, 7500),
        ("stress_test", 83.75, 5.0, 25.0, 13.0, 100000, 10000),
    ]
    
    results = []
    
    for test_name, ra, dec, fov, mag, max_stars, batch in tests:
        print(f"\n{'─' * 80}")
        print(f"📊 Test: {test_name}")
        print(f"   FOV: {fov}° × {fov}°, Mag: {mag}, MaxStars: {max_stars}")
        print("   ", end="", flush=True)
        
        # Crea config
        config_file = create_test_config(test_name, ra, dec, fov, mag, max_stars, batch)
        
        # Esegui test
        result = run_test(config_file, test_name)
        results.append((test_name, fov, mag, result))
        
        # Mostra risultato
        if result["success"]:
            print(f"✅ Completato in {result['elapsed']:.2f}s")
            print(f"   Stelle renderizzate: {result['stars']}")
        else:
            print(f"❌ Fallito dopo {result['elapsed']:.2f}s")
            if result["error"]:
                print(f"   Errore: {result['error'][:100]}")
    
    # Riepilogo
    print(f"\n{'═' * 80}")
    print("📈 RIEPILOGO RISULTATI")
    print(f"{'═' * 80}")
    print()
    print(f"{'Test':<20} {'FOV':>6} {'Mag':>5} {'Tempo':>8} {'Stelle':>10} {'Status':<10}")
    print(f"{'-' * 80}")
    
    for test_name, fov, mag, result in results:
        status = "✅ OK" if result["success"] else "❌ FAIL"
        print(f"{test_name:<20} {fov:>6.1f} {mag:>5.1f} {result['elapsed']:>7.2f}s {result['stars']:>10} {status:<10}")
    
    print()
    print("=" * 80)
    
    # Statistiche
    successful = sum(1 for _, _, _, r in results if r["success"])
    total_time = sum(r["elapsed"] for _, _, _, r in results if r["success"])
    
    print(f"✅ Test superati: {successful}/{len(tests)}")
    if successful > 0:
        print(f"⏱️  Tempo medio: {total_time/successful:.2f}s")
    print("=" * 80)

if __name__ == "__main__":
    main()
