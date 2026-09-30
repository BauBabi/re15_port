#!/usr/bin/env python3
"""tuer_graph.py - Spur D (Runde 34 Nacht): Tuergraph aller Raeume einer Stage + Erreichbarkeit.

Liest JEDEN Door_aot_set (0x3B) aus main- UND sub-SCD (scd_walk_lib, eine Laengentabelle) samt
Kontext: in welchem Thread, hinter welchen Ck-Bedingungen (Ifel_ck-Kette unmittelbar davor) er
steht. Satzlayout (include/re15_tuer1120.h, ROOM1130 @0x008AE):
  [1] Slot [2] sce [3] flags [4] Band [6..13] Rechteck x,z,w,h [14..19] Spawn x,y,z [20..21] dir
  [22] Stage-Byte (0 = STAGE1 ...) [23] Raum (low byte) [24] Cut
  Viereck-Saetze (flags & 0x80, Satzbreite 40 statt 32, 4 Stueck in ROOM4030/4031) tragen 8 Bytes mehr
  Flaeche: dort Stage/Raum/Cut an [30]/[31]/[32].
  BERICHTIGT (Gegenpruefung Spur D, Auflage 1): die erste Fassung nahm die Stage aus dem QUELLraum und
  ignorierte das Stage-Byte — ROOM11A0 @0x01006 Slot 4 (Byte 22 = 0x01) zeigte so faelschlich auf
  ROOM10A0 statt ROOM20A0.
Erreichbarkeit: BFS ab einem Startraum, optional ohne einen gesperrten Raum ("--ohne 10A0").
Bedingungen werden NUR ausgegeben, nicht ausgewertet — die Bewertung steht im Dossier.

Aufruf: tuer_graph.py <STAGEDIR> [--start 1170] [--ohne 10A0] [--nur-leon]
"""
import argparse, glob, os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import scd_walk_lib as W                      # noqa: E402


def tueren(d):
    out = []
    for (tag, idx), ops in sorted(W.regionen(d).items()):
        conds = []
        for i, (pc, op, sz) in enumerate(ops):
            if op == 0x06:                              # Ifel_ck: Bedingung folgt als Ck/Cmp
                conds.append(pc)
            if op == 0x3B:
                ck = []
                # die Ck/Cmp-Saetze im selben Block davor (einfacher Kontext, keine Auswertung)
                for (pc2, op2, sz2) in ops[max(0, i - 12):i]:
                    if op2 == 0x21:
                        ck.append("Ck(%d,%d)==%d" % (d[pc2 + 1], d[pc2 + 2], d[pc2 + 3]))
                    elif op2 == 0x23:
                        ck.append("Cmp@0x%X" % pc2)
                    elif op2 in (0x07,):
                        ck.append("Else")
                q = sz - 32                                 # Viereck-Satz: 8 Bytes mehr Flaeche
                out.append(dict(thread="%s%02d" % (tag, idx), off=pc, slot=d[pc + 1], sce=d[pc + 2],
                                flags=d[pc + 3], band=d[pc + 4], ziel=d[pc + 23 + q],
                                stagebyte=d[pc + 22 + q], cut=d[pc + 24 + q], kontext=ck))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("stagedir"); ap.add_argument("--start", default="1170")
    ap.add_argument("--ohne", default=""); ap.add_argument("--nur-leon", action="store_true")
    a = ap.parse_args()
    graph = {}
    for f in sorted(glob.glob(os.path.join(a.stagedir, "ROOM*.RDT"))):
        raum = os.path.basename(f)[4:8].upper()
        if a.nur_leon and raum[-1] != "0":
            continue
        d = open(f, "rb").read()
        if len(d) < 0x60:
            continue
        kanten = []
        for t in tueren(d):
            # Raum-Id = Stage (Stage-Byte + 1) + Raumbyte (2 Hex) + Spieler-Variante (0 Leon / 1 Elza)
            ziel = "%X%02X%s" % (t["stagebyte"] + 1, t["ziel"], raum[-1])
            kanten.append((ziel, t))
        graph[raum] = kanten
    for raum, kanten in graph.items():
        for ziel, t in kanten:
            print("ROOM%s %s @0x%05X Slot %2d sce %d flags 0x%02X Band %d -> ROOM%s Cut %d %s"
                  % (raum, t["thread"], t["off"], t["slot"], t["sce"], t["flags"], t["band"], ziel,
                     t["cut"], ("  [" + ", ".join(t["kontext"]) + "]") if t["kontext"] else ""))
    ohne = a.ohne.upper()
    seen, q = {a.start.upper()}, [a.start.upper()]
    while q:
        r = q.pop(0)
        for ziel, t in graph.get(r, []):
            if ziel == ohne or ziel in seen or ziel not in graph:
                continue
            if t["sce"] != 2:
                continue
            seen.add(ziel); q.append(ziel)
    print("\nErreichbar ab ROOM%s%s (%d Raeume, Bedingungen NICHT ausgewertet):"
          % (a.start, (" ohne ROOM" + ohne) if ohne else "", len(seen)))
    print(" ".join("ROOM" + r for r in sorted(seen)))


if __name__ == "__main__":
    main()
