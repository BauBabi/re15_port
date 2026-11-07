"""tabelle.py LAUFDIR PRAEFIX [NAMEN...] - Markdown-Tabelle der Savestate-Reihe: Lauf-1-Videozeit (snap_zeit.py -> zeit.txt),
Schnitt, SCD-Zeiger, Claire/Annette/Birkin (ram_reihe.py). Ohne NAMEN alle Staende."""
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2c_ss import SS
from ram_reihe import ent, scd
d, pre = sys.argv[1], sys.argv[2]
want = sys.argv[3:]
Z = {}
for ln in open(os.path.join(d, "zeit.txt")):
    m = re.match(r"(\w+) log .*Lauf-1-Zeit\s+([\d.]+) s", ln)
    if m:
        Z[m.group(1)] = float(m.group(2))
names = sorted(n for n in Z if n.startswith(pre))
if want:
    names = [n for n in names if n in want]

def c(e):
    return "(%d, %d, %d) R%d" % (e['x'], e['y'], e['z'], e['dy']) if e else "-"

def m(e):
    return "r%s M%02X/%d" % (e['r'], e['mo'], e['mo2']) if e else "-"

print("| Stand | Lauf-1-Zeit | Schnitt | SCD (Zeiger / Ruecksprung) | Claire Lage, Richtung | Claire Routine, Bewegung | "
      "Annette (Slot 0) Lage, Richtung | Annette Routine, Bewegung | Birkin (Slot 1) Lage, Richtung | Birkin Routine, Bewegung, +0x1D4 |")
print("|---|---|---|---|---|---|---|---|---|---|")
for n in names:
    s = SS(os.path.join(d, n + ".sav"))
    pl = ent(s, s.u32(0x800c79a8)); a = ent(s, s.u32(0x800c79b0)); b = ent(s, s.u32(0x800c79b4))
    if a and a['id'] != 0x49: a = None
    if b and b['id'] != 0x33: b = None
    print("| %s | %.2f s | %d | %s | %s | %s | %s | %s | %s | %s |" % (
        n, Z[n], s.u16(0x800cc3b4), scd(s), c(pl), m(pl), c(a), m(a), c(b), (m(b) + " =%d" % b['m17']) if b else "-"))
