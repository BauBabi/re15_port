#!/usr/bin/env bash
# alle Laeufe nacheinander, Fenster oben rechts, je Lauf eine Framedump-Serie (F-Bereich/Schritt). Reihenfolge: kurze Rampenlaeufe zuerst.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn4"
B="$SP/werkzeug/batch2.sh"
for P in neu ohne; do echo "=== $P west4_leon $(date +%T) ==="; bash "$B" west4_leon $P "600-1300/5:fd_"; done
for P in neu ohne; do echo "=== $P west5_elza $(date +%T) ==="; bash "$B" west5_elza $P "1000-2000/5:fd_"; done
for P in neu ohne; do echo "=== $P west4_elza $(date +%T) ==="; bash "$B" west4_elza $P "1100-2700/10:fd_"; done
for P in neu ohne; do echo "=== $P frei4_leon $(date +%T) ==="; bash "$B" frei4_leon $P "5600-6400/10:fd_"; done
for P in neu ohne; do echo "=== $P frei4_elza $(date +%T) ==="; bash "$B" frei4_elza $P "3300-4000/10:fd_"; done
echo "=== burst west4_leon (30 B/s Kanister-Sitz, mit Kanister) $(date +%T) ==="; PFX=b_ bash "$B" west4_leon neu "1115-1175/1:fd_"
echo ALLE_FERTIG
