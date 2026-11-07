# ST80 Abnahme 2: Auswertung eines exe-Laufs (debug.log, state.log, stimme.log). Nur lesen.
#   python -I auswerten.py <laufordner> [--soll "11,31,13,..."]
import re, sys, os
sys.stdout.reconfigure(encoding="utf-8")
d = sys.argv[1]
soll = None
if "--soll" in sys.argv: soll = [int(x) for x in sys.argv[sys.argv.index("--soll") + 1].split(",")]
dbg = open(os.path.join(d, "debug.log"), encoding="utf-8", errors="replace").read().splitlines()
print("## debug.log: [story80] / [msgtext] / [voice] in Reihenfolge")
ids = []; texte = {}
for z in dbg:
    if z.startswith("[story80]") or z.startswith("[msgtext]") or z.startswith("[voice]") or "EXIT_AT" in z or "CONTINUE: resumed" in z or z.startswith("[input-script]") or z.startswith("[audio]"):
        print(z[:200])
    m = re.match(r'\[msgtext\] room=(\w+) id=(\d+) "(.*)"', z)
    if m: ids.append((m.group(1), int(m.group(2)))); texte[int(m.group(2))] = m.group(3)
print()
print("## Folge der Texte:", " ".join(str(i) for _, i in ids))
dop = sorted({i for _, i in ids if [k for _, k in ids].count(i) > 1})
print("## doppelt:", dop if dop else "keine")
if soll is not None:
    ist = [i for _, i in ids]
    print("## Soll-Folge:", " ".join(map(str, soll)), "->", "GLEICH" if ist == soll else "ABWEICHUNG")
p21 = next((n for n, (_, i) in enumerate(ids) if i == 21), len(ids))
vor = [i for _, i in ids[:p21] if re.search(r"Sherry|daughter|little girl", texte.get(i, ""))]
print("## Sherry-Zeilen vor Johns Vorstellung (msg 21):", vor if vor else "keine")
# Stimmen: clip loaded / keine Aufnahme
cl = [z for z in dbg if z.startswith("[voice]")]
print("## Stimmen-Zeilen:", len(cl))
# state.log: Anzeige-Bereiche je Id (msg a=1) und Spielerfreigabe
st = os.path.join(d, "state.log")
bereiche = []; cur = None; frei = []; vorher_pst = None; pos = {}
for line in open(st, encoding="latin-1"):
    m = re.match(r'F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) .*?cam=(\d+) msg\(a=(\d) fsm=(\d) id=(\d+)', line)
    if not m: continue
    f = int(m.group(1)); pst = int(m.group(7)); cam = int(m.group(8)); a = int(m.group(9)); i = int(m.group(11))
    pos[f] = (int(m.group(3)), int(m.group(4)), int(m.group(5)), pst, cam)
    if a == 1:
        if cur and cur[0] == i and f - cur[2] <= 2: cur[2] = f
        else:
            if cur: bereiche.append(cur)
            cur = [i, f, f, cam]
    if vorher_pst is not None and pst != vorher_pst: frei.append((f, vorher_pst, pst, cam, pos[f][:3]))
    vorher_pst = pst
if cur: bereiche.append(cur)
print("## Untertitel im Bild (state.log msg a=1): Id Bildbereich Kamera")
for i, s, e, c in bereiche: print("  id=%2d F%d-F%d (%d Bilder) cam=%d" % (i, s, e, e - s + 1, c))
print("## Spielerzustand-Wechsel (pst): Bild alt->neu Kamera Lage")
for f, a, b, c, p in frei: print("  F%d pst %d->%d cam=%d PL%s" % (f, a, b, c, p))
sl = os.path.join(d, "stimme.log")
if os.path.exists(sl):
    print("## stimme.log")
    for z in open(sl, encoding="latin-1"): print("  " + z.rstrip())
last = max(pos) if pos else None
if last: print("## letztes Bild F%d PL%s" % (last, pos[last]))
