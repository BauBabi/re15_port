"""trilog_nclip.py - Runde 38 Spur A: RE15_TRILOG-Ausgabe auswerten. Je TIM-Platz die Dreiecke des LETZTEN Bildes
(Folge rueckwaerts bis zur letzten Wiederholung der ersten Zeile des Platzes) mit dem NCLIP-Vorzeichen
MAC0 = (x1-x0)(y2-y0) - (y1-y0)(x2-x0) (= psx-spx NCLIP, umgeformt). Aufruf: python trilog_nclip.py <trilog> [slot]"""
import re, sys
from collections import Counter, defaultdict

PAT = re.compile(r'^(\d+) (\S+) slot=(\d+) clut=0x([0-9A-F]+) z=(-?\d+)  \((-?\d+),(-?\d+)\)uv\((\d+),(\d+)\) '
                 r'\((-?\d+),(-?\d+)\)uv\((\d+),(\d+)\) \((-?\d+),(-?\d+)\)uv\((\d+),(\d+)\)')


def main():
    zeilen = [m for m in (PAT.match(l) for l in open(sys.argv[1], encoding='utf-8', errors='replace')) if m]
    nur = sys.argv[2] if len(sys.argv) > 2 else None
    je = defaultdict(list)
    for m in zeilen:
        je[m.group(3)].append(m)
    for slot, ms in sorted(je.items(), key=lambda kv: int(kv[0])):
        if nur is not None and slot != nur:
            continue
        # letztes Bild: ab dem letzten Auftreten der ersten Zeile (gleiche Ecken + UV) dieses Platzes
        sig = lambda m: m.group(6, 7, 8, 9, 10, 11, 14, 15)
        erste = sig(ms[0])
        start = max(i for i, m in enumerate(ms) if sig(m) == erste)
        bild = ms[start:]
        c = Counter()
        for m in bild:
            x0, y0, x1, y1, x2, y2 = (int(m.group(i)) for i in (6, 7, 10, 11, 14, 15))
            mac0 = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0)
            c['<0' if mac0 < 0 else ('=0' if mac0 == 0 else '>0')] += 1
        print('slot %s: %d Dreiecke im letzten Bild, NCLIP <0: %d, =0: %d, >0: %d' % (
            slot, len(bild), c['<0'], c['=0'], c['>0']))


if __name__ == '__main__':
    main()
