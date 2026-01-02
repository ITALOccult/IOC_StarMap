#!/usr/bin/env python3
"""
Script helper per convertire dati di occultazione in formato JSON IOC
"""

import json
import sys
from datetime import datetime

def create_template():
    """Crea un template JSON vuoto"""
    template = {
        "event_id": "YYYY-MM-DD-NNNN-AsteroidName",
        "source": "IOC",
        "data_version": "1.0",
        "description": "Descrizione evento",
        
        "target_star": {
            "catalog_id": "Gaia DR3 xxxx",
            "ra": 0.0,
            "dec": 0.0,
            "magnitude": 0.0,
            "bv_color": None,
            "spectral_type": "",
            "sao_number": None,
            "proper_name": ""
        },
        
        "asteroid": {
            "name": "(NNNN) Name",
            "number": 0,
            "designation": "",
            "ra": 0.0,
            "dec": 0.0,
            "magnitude": 0.0,
            "diameter_km": 0.0,
            "velocity_arcsec_per_hour": 0.0,
            "position_angle": 0.0
        },
        
        "circumstances": {
            "event_time": "YYYY-MM-DDTHH:MM:SS.SZ",
            "duration_seconds": 0.0,
            "shadow_width_km": 0.0,
            "shadow_speed_km_s": 0.0,
            "position_angle": 0.0,
            "uncertainty_along_track_km": 0.0,
            "uncertainty_cross_track_km": 0.0,
            "magnitude_drop": 0.0,
            "probability": 1.0
        },
        
        "observation_site": {
            "name": "",
            "latitude": 0.0,
            "longitude": 0.0,
            "altitude_m": 0.0,
            "timezone": "UTC"
        },
        
        "notes": ""
    }
    
    return template

def ra_hms_to_degrees(hours, minutes, seconds):
    """Converte RA da ore:minuti:secondi a gradi"""
    return (hours + minutes/60.0 + seconds/3600.0) * 15.0

def dec_dms_to_degrees(degrees, minutes, seconds):
    """Converte Dec da gradi:minuti:secondi a gradi decimali"""
    sign = 1 if degrees >= 0 else -1
    return sign * (abs(degrees) + minutes/60.0 + seconds/3600.0)

def parse_occult_watcher(filename):
    """
    Parse output di Occult Watcher
    (Implementazione base - da personalizzare)
    """
    # TODO: implementare parsing specifico
    print("Parsing Occult Watcher non ancora implementato")
    return None

def interactive_input():
    """Input interattivo per creare un evento"""
    print("\n" + "="*60)
    print("CREAZIONE EVENTO OCCULTAZIONE")
    print("="*60 + "\n")
    
    event = create_template()
    
    # Event ID
    print("EVENT ID:")
    date = input("  Data (YYYY-MM-DD): ")
    asteroid = input("  Nome asteroide: ")
    event["event_id"] = f"{date}-{asteroid}"
    
    # Target star
    print("\nSTELLA TARGET:")
    event["target_star"]["catalog_id"] = input("  Catalog ID (es. Gaia DR3 xxx): ")
    
    ra_input = input("  RA - Formato (1=decimal deg, 2=HH:MM:SS): ")
    if ra_input == "1":
        event["target_star"]["ra"] = float(input("    RA (gradi): "))
    else:
        h = float(input("    RA ore: "))
        m = float(input("    RA minuti: "))
        s = float(input("    RA secondi: "))
        event["target_star"]["ra"] = ra_hms_to_degrees(h, m, s)
    
    dec_input = input("  Dec - Formato (1=decimal deg, 2=DD:MM:SS): ")
    if dec_input == "1":
        event["target_star"]["dec"] = float(input("    Dec (gradi): "))
    else:
        d = float(input("    Dec gradi: "))
        m = float(input("    Dec minuti: "))
        s = float(input("    Dec secondi: "))
        event["target_star"]["dec"] = dec_dms_to_degrees(d, m, s)
    
    event["target_star"]["magnitude"] = float(input("  Magnitudine: "))
    
    # Asteroid
    print("\nASTEROIDE:")
    event["asteroid"]["name"] = input("  Nome completo (es. (433) Eros): ")
    number = input("  Numero MPC (invio per skip): ")
    if number:
        event["asteroid"]["number"] = int(number)
    
    event["asteroid"]["magnitude"] = float(input("  Magnitudine: "))
    event["asteroid"]["diameter_km"] = float(input("  Diametro (km): "))
    event["asteroid"]["velocity_arcsec_per_hour"] = float(input("  Velocità (arcsec/h): "))
    event["asteroid"]["position_angle"] = float(input("  Position Angle (gradi): "))
    
    # Circumstances
    print("\nCIRCOSTANZE:")
    event_time = input("  Data/ora evento (YYYY-MM-DDTHH:MM:SS.SZ): ")
    event["circumstances"]["event_time"] = event_time
    event["circumstances"]["duration_seconds"] = float(input("  Durata (secondi): "))
    event["circumstances"]["magnitude_drop"] = float(input("  Drop magnitudine: "))
    
    # Observation site (optional)
    print("\nSITO OSSERVAZIONE (opzionale, invio per skip):")
    site_name = input("  Nome sito: ")
    if site_name:
        event["observation_site"]["name"] = site_name
        event["observation_site"]["latitude"] = float(input("  Latitudine (gradi): "))
        event["observation_site"]["longitude"] = float(input("  Longitudine (gradi): "))
        event["observation_site"]["altitude_m"] = float(input("  Altitudine (m): "))
    
    return event

def main():
    if len(sys.argv) < 2:
        print("Uso:")
        print("  python3 occultation_helper.py template > template.json")
        print("  python3 occultation_helper.py interactive")
        print("  python3 occultation_helper.py parse <file_input>")
        sys.exit(1)
    
    command = sys.argv[1]
    
    if command == "template":
        # Genera template
        template = create_template()
        print(json.dumps(template, indent=2))
    
    elif command == "interactive":
        # Input interattivo
        event = interactive_input()
        
        output_file = f"{event['event_id']}.json"
        with open(output_file, 'w') as f:
            json.dump(event, f, indent=2)
        
        print(f"\n✅ File creato: {output_file}")
        print(f"\nPer generare le carte:")
        print(f"  ./occultation_chart {output_file}")
    
    elif command == "parse":
        if len(sys.argv) < 3:
            print("Errore: specifica file input")
            sys.exit(1)
        
        input_file = sys.argv[2]
        print(f"Parsing {input_file}...")
        
        # Auto-detect formato
        # TODO: implementare rilevamento automatico
        print("Auto-detect non ancora implementato")
    
    else:
        print(f"Comando sconosciuto: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()
