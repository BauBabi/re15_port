"""Runde 37 Spur L: Auswertung eines RE15_STATE_LOG der echten exe (platform/pc/main.c, eine Zeile je Bild).
Je Zombie (RE2-Familie, Typ 0x10..0x1F): Wechsel in den Arme-hoch-Gang ss1=2 (Abstand +0x1F0, Spieler-Motion 100 = RENNEN),
was danach kommt (Griff ss1=3, zurueck ss1=1, ...), Weg je Bild in ss1=1 und ss1=2, Ausfall ss1=12 und Griff aus dem Ausfall
(12 -> 3, DECISION[12] @0x8010460C), Spieler-Griffe (gr=1).
Aufruf: python ausw_state.py <state.log> [ab_bild] [nur_raum_ab_letztem_F1]"""
import re, sys
from collections import defaultdict

RX_F = re.compile(r"^F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) ps2=(\d+) mo=(\d+)")
RX_E = re.compile(r"\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=[0-9a-f]+ mo=(\d+) af=\d+ stun=-?\d+ d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
RX_GR = re.compile(r" gr=(\d)")


def main():
    pfad = sys.argv[1]
    ab = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    zeilen = open(pfad, errors="replace").read().splitlines()
    # nur der letzte Raumbesuch: ab der letzten Zeile, deren Bildnummer kleiner ist als die vorige (Raumwechsel setzt F zurueck)
    start = 0
    letzte = -1
    for i, z in enumerate(zeilen):
        m = RX_F.match(z)
        if not m:
            continue
        f = int(m.group(1))
        if f < letzte:
            start = i
        letzte = f
    vor = {}
    stat = defaultdict(lambda: dict(w201=0, d201=[], renn=0, nach=defaultdict(int), t1=0, w1=0, t2=0, w2=0, aus=0, aus_griff=0,
                                     griffe=0))
    pos = {}
    pl_griffe = 0; gr_vor = 0; erster_griff = -1
    for z in zeilen[start:]:
        m = RX_F.match(z)
        if not m:
            continue
        f = int(m.group(1))
        if f < ab:
            continue
        mo = int(m.group(10))
        g = RX_GR.search(z)
        gr = int(g.group(1)) if g else 0
        if gr and not gr_vor:
            pl_griffe += 1
            if erster_griff < 0:
                erster_griff = f
        gr_vor = gr
        for e in RX_E.finditer(z):
            slot, typ = int(e.group(1)), int(e.group(2), 16)
            if typ < 0x10 or typ > 0x1f:
                continue
            st, s1, d, x, zz = int(e.group(3)), int(e.group(4)), int(e.group(7)), int(e.group(8)), int(e.group(9))
            s = stat[slot]
            a = vor.get(slot)
            if a is not None:
                ast, as1 = a
                if st == 1 and s1 == 2 and not (ast == 1 and as1 == 2):
                    s["w201"] += 1; s["d201"].append(d)
                    if mo == 100:
                        s["renn"] += 1
                if ast == 1 and as1 == 2 and not (st == 1 and s1 == 2):
                    s["nach"]["%d/%d" % (st, s1)] += 1
                if st == 1 and s1 == 12 and not (ast == 1 and as1 == 12):
                    s["aus"] += 1
                if ast == 1 and as1 == 12 and st == 1 and s1 == 3:
                    s["aus_griff"] += 1
                if st == 1 and s1 == 3 and not (ast == 1 and as1 == 3):
                    s["griffe"] += 1
                if ast == 1 and st == 1 and as1 == s1 and s1 in (1, 2) and slot in pos:
                    w = abs(x - pos[slot][0]) + abs(zz - pos[slot][1])
                    if s1 == 1:
                        s["t1"] += 1; s["w1"] += w
                    else:
                        s["t2"] += 1; s["w2"] += w
            vor[slot] = (st, s1)
            pos[slot] = (x, zz)
    tot = defaultdict(int)
    for slot, s in sorted(stat.items()):
        print("slot %2d: 0x201 %d x (d %s, davon Leon rennt %d), danach %s | Weg/Bild ss1=1 %.1f (%d) ss1=2 %.1f (%d) | Ausfall %d, "
              "Griff aus Ausfall %d, Griffe gesamt %d" % (
                  slot, s["w201"], s["d201"], s["renn"], dict(s["nach"]), s["w1"] / s["t1"] if s["t1"] else 0, s["t1"],
                  s["w2"] / s["t2"] if s["t2"] else 0, s["t2"], s["aus"], s["aus_griff"], s["griffe"]))
        for k in ("w201", "renn", "aus", "aus_griff", "griffe", "t1", "w1", "t2", "w2"):
            tot[k] += s[k]
        tot["nach_griff"] += s["nach"].get("1/3", 0)
    print("SUMME: 0x201 %d (Leon rennt %d), aus 0x201 in den Griff %d, Ausfall %d, Griff aus Ausfall %d, Zombie-Griffe %d, "
          "Weg/Bild ss1=1 %.1f ss1=2 %.1f, Spieler gegriffen %d x (erstmals Bild %d)" % (
              tot["w201"], tot["renn"], tot["nach_griff"], tot["aus"], tot["aus_griff"], tot["griffe"],
              tot["w1"] / tot["t1"] if tot["t1"] else 0, tot["w2"] / tot["t2"] if tot["t2"] else 0, pl_griffe, erster_griff))


if __name__ == "__main__":
    main()
