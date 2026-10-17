"""Runde 37 Spur L, Nachbesserung 2: Ausgaenge der Erholungsleiter P3 (Treffer-Handler 0x80105438, re2z_hit_main case 3)
aus einem RE15_STATE_LOG der echten exe. Ein Ausgang = Platz mit st=2 ss2=3 im Bild F, im Bild F+1 nicht mehr st=2:
  1/1/1 = 0x10101 (Gleichheitsfall @0x80105B44-50), 1/1/0 = 0x101 (@0x80105B40), 1/2 = 0x201 (@0x80105B24),
  1/11 = 0xB01 (@0x80105ACC), 1/12 = 0xC01 (@0x80105B08).
Je Ausgang eine Zeile (Bild, Platz, Typ, mo/af davor und danach) und eine Summe.
Aufruf: python erholung_ausw.py <state.log> [besuch]   (besuch = n-ter Raumbesuch ab 1, Standard: alle)"""
import re, sys
from collections import Counter

RX_F = re.compile(r"^F(\d+) ")
RX_E = re.compile(r"\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=[0-9a-f]+ mo=(\d+) af=(\d+) stun=-?\d+ d=(\d+) "
                  r"@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
NAMEN = {(1, 1, 1): "0x10101", (1, 1, 0): "0x101"}


def main():
    pfad = sys.argv[1]
    nur = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    besuch, letzte = 0, -1
    vor = {}
    summe = Counter()
    for z in open(pfad, errors="replace"):
        m = RX_F.match(z)
        if not m:
            continue
        f = int(m.group(1))
        if f < letzte or letzte < 0:
            besuch += 1
            vor = {}
        letzte = f
        if nur and besuch != nur:
            continue
        jetzt = {}
        for e in RX_E.finditer(z):
            slot, typ = int(e.group(1)), int(e.group(2), 16)
            if typ < 0x10 or typ > 0x1f:
                continue
            jetzt[slot] = (int(e.group(3)), int(e.group(4)), int(e.group(5)), int(e.group(6)), int(e.group(7)), typ,
                           int(e.group(8)))
        for slot, (st, s1, s2, mo, af, typ, d) in jetzt.items():
            a = vor.get(slot)
            if a is None or not (a[0] == 2 and a[2] == 3) or st == 2:
                continue
            if (st, s1) == (1, 2):
                name = "0x201"
            elif (st, s1) == (1, 11):
                name = "0xB01"
            elif (st, s1) == (1, 12):
                name = "0xC01"
            else:
                name = NAMEN.get((st, s1, s2), "%d/%d/%d" % (st, s1, s2))
            summe[name] += 1
            print("B%d F%d Platz %d Typ %02x: P3 mo=%d af=%d -> %s (st %d/%d/%d mo=%d af=%d, d=%d)" % (
                besuch, f, slot, typ, a[3], a[4], name, st, s1, s2, mo, af, d))
        vor = jetzt
    print("SUMME", dict(summe))


if __name__ == "__main__":
    main()
