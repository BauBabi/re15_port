# orig_uebergaenge.py <orig_10b1_john.log> — fasst die GDB-Stichproben (orig_10b1_john.py) auf Zustandswechsel zusammen:
# eine Zeile je Wechsel von (+0x5 Sub, +0x6 Phase, +0x82 Band, +0x94 Clip).
import re, sys
pat = re.compile(r"t=\s*([\d.]+) st=(\d+) s1=0x(\w+) s2=(\d+) typ=(\w+) x=(-?\d+) y=(-?\d+) z=(-?\d+) b=(\d+) mo=(\d+) "
                 r"af=(\d+) 9f=(-?\d+) fy=(-?\d+) 1c0=(\w+) r=(\d+) \| PL (-?\d+),(-?\d+),(-?\d+) cut (-?\d+)")
prev = None
n = 0
for l in open(sys.argv[1], encoding="utf-8"):
    m = pat.match(l)
    if not m:
        continue
    r = m.groups(); n += 1
    key = (r[2], r[3], r[8], r[9])
    if prev != key:
        print("t=%6s Sub 0x%s Phase %s Band %s Clip %s Bild %s  Lage (%s,%s,%s)  +0x9f %s  +0x1ba %s  +0x1c0 %s  Kurs %d  Cut %s" % (
            r[0], r[2], r[3], r[8], r[9], r[10], r[5], r[6], r[7], r[11], r[12], r[13], int(r[14]) & 0xfff, r[18]))
    prev = key
print("Stichproben:", n)
