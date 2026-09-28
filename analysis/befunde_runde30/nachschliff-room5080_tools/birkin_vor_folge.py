#!/usr/bin/env python3
"""Birkin VOR der Generator-Folge: Weg und Wandkontakte aus einem RE15_STATE_LOG.

Liest die state.log eines Echtlaufs (r30_5080_echtlauf.sh) ab dem Eintritt in ROOM5080
(Spieler (-9950,-18200), Tuer-Datensatz ROOM6010 @0x00FCE) und wertet den Aktor Typ 0x30 aus:
  - erstes Bild IN der Rohflaeche der Nordwand SCA @0x00320 (x -28961..-6841, z -8914..-6654)
  - Bilder IN der Rohflaeche des Rauten-Blocks SCA @0x003C8 (x -26936..-19656, z -16509..-9229)
  - kleinstes z (je kleiner, desto weiter im Raum; der Raum liegt bei z < -6654)
  - Spieler-hp-Verlauf (erstes Bild mit hp < 100, kleinster Wert)
  - ob die Folge lief (Plc_dest in debug.log)
Zellgrenzen: sca_5080.py (Datei-Bytes ROOM5080.RDT). Keine Deutung ausser der Ausgabe.
Aufruf: birkin_vor_folge.py <laufverzeichnis> [...]"""
import os, re, sys

WAND = ("SCA @0x00320 Nordwand", -28961, -6841, -8914, -6654)
RAUTE = ("SCA @0x003C8 Rauten-Block NW", -26936, -19656, -16509, -9229)
PL = re.compile(r"^F(\d+) pad=\S+ PL\((-?\d+),(-?\d+),rot=-?\d+,hp=(-?\d+)\)")
BK = re.compile(r"\[(\d+) t=30 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=(\w+) mo=(\d+) af=\d+ "
                r"stun=\S+ d=\S+ @\((-?\d+),(-?\d+),r-?\d+\)\]")

def drin(x, z, c):
    return c[1] <= x <= c[2] and c[3] <= z <= c[4]

for lauf in sys.argv[1:]:
    zeilen = open(os.path.join(lauf, "state.log"), encoding="latin-1").read().splitlines()
    im_raum, erste_zeile = False, None
    wand_erst, raute, zmin, hp_erst, hp_min, n = None, [], None, None, 100, 0
    letzte = None
    for z in zeilen:
        m = PL.match(z)
        if not m:
            continue
        f, px, pz, hp = int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4))
        if not im_raum:
            if (px, pz) == (-9950, -18200):
                im_raum, erste_zeile = True, f
            else:
                continue
        b = BK.search(z)
        if hp < 100 and hp_erst is None:
            hp_erst = (f, hp)
        hp_min = min(hp_min, hp)
        if not b:
            continue
        n += 1
        bx, bz, g, sub = int(b.group(7)), int(b.group(8)), b.group(5), int(b.group(3))
        if wand_erst is None and drin(bx, bz, WAND):
            wand_erst = (f, bx, bz)
        if drin(bx, bz, RAUTE):
            raute.append((f, bx, bz))
        if zmin is None or bz < zmin[2]:
            zmin = (f, bx, bz)
        letzte = (f, bx, bz, g, sub, px, pz, hp)
    dbg = os.path.join(lauf, "debug.log")
    folge = any("Plc_dest" in l for l in open(dbg, encoding="latin-1")) if os.path.exists(dbg) else None
    print(f"== {lauf}")
    print(f"  Eintritt ROOM5080 ab F{erste_zeile}, {n} Bilder mit Birkin; Generator-Folge (Plc_dest) lief: {folge}")
    print(f"  erstes Bild in {WAND[0]}: {wand_erst}")
    print(f"  Bilder in {RAUTE[0]}: {len(raute)}" + (f", erstes {raute[0]}, letztes {raute[-1]}" if raute else ""))
    print(f"  kleinstes z (tiefste Lage im Raum): {zmin}")
    print(f"  Spieler: erstes hp<100 {hp_erst}, kleinstes hp {hp_min}")
    print(f"  letztes Bild: F{letzte[0]} Birkin=({letzte[1]},{letzte[2]}) g={letzte[3]} sub={letzte[4]} "
          f"Spieler=({letzte[5]},{letzte[6]}) hp={letzte[7]}" if letzte else "  kein Birkin")
