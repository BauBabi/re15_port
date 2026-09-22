#!/usr/bin/env python3
"""cutscene_latch_zensus.py — WELCHE Flag-Zelle ist der Cutscene-Riegel?

Die Wegwerf-Abfrage darf nicht in eine laufende Szene platzen. Um zu BELEGEN, welche
Zelle "eine Szene laeuft" bedeutet (statt sie zu raten), zaehlt dieses Werkzeug ueber
ALLE ausgelieferten RDTs:

  (a) jedes Set (0x22) je (Zone,Bit) — getrennt nach Setzen (val!=0) und Loeschen (val==0);
  (b) jedes Ck (0x21) je (Zone,Bit) — WER liest die Zelle ueberhaupt;
  (c) fuer jede Zelle: in wie vielen Unterprogrammen sie als PAAR auftritt
      (Set(...,1) frueh und Set(...,0) spaet im SELBEN Unterprogramm) — das ist die
      Signatur eines Fensters "ab hier bis dort laeuft etwas";
  (d) ob im selben Unterprogramm ein Plc_ret (0x42) und/oder Cut_auto (0x3C) steht —
      die beiden Opcodes, mit denen eine Szene die Spielfigur und die Kamera FREIGIBT.

Der Walker (Laengen, Sprungziele, Desync-STOP) ist aus tools/discard_zensus.py
uebernommen, die ihn aus engine/src/scd_vm.c s_opcode_sizes hat.

Aufruf:  python cutscene_latch_zensus.py [ASSET_ROOT] [--zelle Z:B]
"""
import sys, os, glob, argparse, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from discard_zensus import op_size, fwd_target, rdt_section_end, section_regions, u32

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root", nargs="?", default=os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "..", "shared_assets", "PSX"))
    ap.add_argument("--zelle", action="append", default=[],
                    help="Zone:Bit — zeigt jedes Vorkommen dieser Zelle einzeln")
    a = ap.parse_args()
    watch = set()
    for w in a.zelle:
        z, b = w.split(":"); watch.add((int(z, 0), int(b, 0)))

    set1 = collections.Counter(); set0 = collections.Counter()
    ck   = collections.Counter()
    paar = collections.Counter()          # Zelle -> Anzahl Unterprogramme mit Set1..Set0
    paar_plc = collections.Counter()      # davon: mit Plc_ret im selben Unterprogramm
    paar_cut = collections.Counter()      # davon: mit Cut_auto im selben Unterprogramm
    treffer = []
    rooms = 0; desync = 0

    for path in sorted(glob.glob(os.path.join(a.root, "STAGE*", "ROOM*.RDT"))):
        d = open(path, "rb").read()
        room = os.path.basename(path).split(".")[0]
        if len(d) < 0x48: continue
        rooms += 1
        for sec, tag in ((u32(d, 0x40), "main"), (u32(d, 0x44), "sub")):
            if sec == 0 or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                start, end = sec + o, sec + e
                pc = start; maxf = start
                lokal = collections.defaultdict(list)   # Zelle -> [(off,val)]
                hat_plc = False; hat_cut = False
                while pc < end:
                    op = d[pc]; sz = op_size(d, pc)
                    if sz is None: desync += 1; break
                    if pc + sz > end: break
                    if op == 0x22:
                        zelle = (d[pc + 1], d[pc + 2]); val = d[pc + 3]
                        (set1 if val else set0)[zelle] += 1
                        lokal[zelle].append((pc, val))
                        if zelle in watch:
                            treffer.append((room, f"{tag}{idx:02d}", pc, "Set", val))
                    elif op == 0x21:
                        zelle = (d[pc + 1], d[pc + 2]); ck[zelle] += 1
                        if zelle in watch:
                            treffer.append((room, f"{tag}{idx:02d}", pc, "Ck", d[pc + 3]))
                    elif op == 0x42: hat_plc = True
                    elif op == 0x3C: hat_cut = True
                    t = fwd_target(d, pc, op)
                    if t is not None and t > maxf: maxf = t
                    if op == 0x01 and pc + 2 > maxf: break
                    pc += sz
                for zelle, vs in lokal.items():
                    ein = [x for x in vs if x[1] != 0]; aus = [x for x in vs if x[1] == 0]
                    if ein and aus and min(x[0] for x in ein) < max(x[0] for x in aus):
                        paar[zelle] += 1
                        if hat_plc: paar_plc[zelle] += 1
                        if hat_cut: paar_cut[zelle] += 1

    print(f"RDTs mit Header: {rooms}   Desync: {desync}")
    print(f"\n## Zellen mit FENSTER-Signatur (Set(x,1) .. Set(x,0) im selben Unterprogramm)")
    print(f"{'Zone:Bit':>10} {'Fenster':>8} {'+Plc_ret':>9} {'+Cut_auto':>10} "
          f"{'Set!=0':>7} {'Set==0':>7} {'Ck':>5}")
    for zelle, n in paar.most_common(25):
        print(f"{zelle[0]:>6}:{zelle[1]:<3} {n:>8} {paar_plc[zelle]:>9} {paar_cut[zelle]:>10} "
              f"{set1[zelle]:>7} {set0[zelle]:>7} {ck[zelle]:>5}")
    if treffer:
        print(f"\n## Einzelvorkommen der gesuchten Zellen ({len(treffer)})")
        for t in treffer: print("   %-9s %-7s 0x%05X  %-4s val=%d" % t)

main()
