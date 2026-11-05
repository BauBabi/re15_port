# NB6: Zeichenreihenfolge der Lampen-Pakete im RE2-RAM (OT-Verkettung: Tag = Laenge<<24 | naechste Adresse) - s04 Puffer 0 (x -6000).
import sys
sys.path.insert(0, ".")
from re2ram4 import Ram, SN
n = int(sys.argv[1]); obj = int(sys.argv[2]); buf = int(sys.argv[3])
r = Ram(SN % n)
o = 0x800d0324 + obj * 0x1f8
pk = {}
for i in range(18): pk[(r.u32(o + 0x18) + buf * 40 + i * 0x50) & 0xffffff] = "T%02d" % i
for i in range(19): pk[(r.u32(o + 0x20) + buf * 52 + i * 104) & 0xffffff] = "Q%02d" % i
# Kette ab jedem Lampenpaket verfolgen, bis ein anderes Lampenpaket erreicht ist (max 4000 Glieder)
folge = {}
for a, name in pk.items():
    p = a; schritte = 0
    while schritte < 4000:
        tag = r.u32(0x80000000 | p); nxt = tag & 0xffffff
        if nxt == 0xffffff: break
        p = nxt; schritte += 1
        if p in pk: folge[name] = (pk[p], schritte); break
print("Paket -> naechstes Lampenpaket in der OT-Kette (Schritte):")
for name in sorted(folge): print("  %s -> %s (%d)" % (name, folge[name][0], folge[name][1]))
# Gesamtreihenfolge: Start = Paket, auf das keines zeigt
ziele = set(v[0] for v in folge.values())
start = [k for k in folge if k not in ziele] + [k for k in pk.values() if k not in folge and k not in ziele]
for s in start:
    kette = [s]
    while kette[-1] in folge: kette.append(folge[kette[-1]][0])
    print("Kette:", " ".join(kette))
