import re, sys
# Nachbesserung 1, Mangel 1 (Spur R): Gift-Takte in der echten exe und die Spieler-Reaktion danach.
# Aufruf: python gift_takt_ana.py <spinnen.log> <state.log> <raum-hex, z.B. 2030>
# Beide Logs stammen aus DEMSELBEN Lauf (RE15_SPINNEN_LOG + RE15_STATE_LOG); ihr Bildzaehler beginnt je Raum neu,
# die Zeilen sind 1:1 (Pruefung F-Gleichheit je Zeile).
#   Gift-Takt  = Bild, in dem der Gift-Zaehler gz (Spieler+0x1d3 des Originals) von 0 neu geladen wird (0x78 = 120
#                bzw. 8) UND die HP um genau 2 fallen (@0x80031e78-ac).
#   Reaktion   = in den 25 Bildern danach traegt der Spieler einen Treffer-Clip (mo 8/9/10 = cmd-2-Clips
#                @0x80035e38-40 / @0x80035fbc-c4 / @0x80035bd0) oder der Spieler-Zustand pst wird 2.
#   Ein Spucktreffer (HP -10) im Fenster [-25, +25] wird markiert (dann ist die Reaktion echt).
pl_re = re.compile(r"F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+) hp=(-?\d+) st=(\d+) ps=(\w+) fl=(\d+) gz=(\d+)")
st_re = re.compile(r"F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) .*? mo=(\d+) ")
sp_lines = open(sys.argv[1], encoding="latin-1").read().splitlines()
st_lines = open(sys.argv[2], encoding="latin-1").read().splitlines()
raum = sys.argv[3].upper()
rows = []
for a, b in zip(sp_lines, st_lines):
    m = pl_re.search(a); n = st_re.search(b)
    if not m or not n: continue
    if m.group(1) != n.group(1):
        print("WARNUNG: Zeilen nicht deckungsgleich: %s / %s" % (m.group(1), n.group(1))); continue
    if m.group(2).upper() != raum: continue
    rows.append(dict(f=int(m.group(1)), hp=int(m.group(6)), st=int(m.group(7)), ps=m.group(8), gz=int(m.group(10)),
                     x=int(n.group(2)), z=int(n.group(3)), pst=int(n.group(6)), mo=int(n.group(7))))
takte = []
for i in range(1, len(rows)):
    p, r = rows[i - 1], rows[i]
    if p["gz"] == 0 and r["gz"] in (120, 8) and p["hp"] - r["hp"] == 2:
        takte.append(i)
spuck = [i for i in range(1, len(rows)) if rows[i - 1]["hp"] - rows[i]["hp"] == 10]
print("Raum %s: %d Bilder, Gift-Takte %d, Spucktreffer (-10) %d" % (raum, len(rows), len(takte), len(spuck)))
mit_reaktion = sauber_mit = sauber_ohne = 0
for k, i in enumerate(takte):
    r = rows[i]
    fenster = rows[i + 1:i + 26]
    hitclip = sum(1 for w in fenster if w["mo"] in (8, 9, 10))
    pst2 = sum(1 for w in fenster if w["pst"] == 2)
    dx = max((abs(w["x"] - r["x"]) + abs(w["z"] - r["z"]) for w in fenster), default=0)
    nahe = [rows[j]["f"] for j in spuck if abs(j - i) <= 25]
    per = (r["f"] - rows[takte[k - 1]]["f"]) if k > 0 else None
    reakt = hitclip > 0 or pst2 > 0
    if reakt: mit_reaktion += 1
    if not nahe:
        if reakt: sauber_mit += 1
        else: sauber_ohne += 1
    print("  Takt F%-5d HP %3d->%3d gz %3d | Periode %s | Treffer-Clip-Bilder %2d, pst=2 %2d, Verschiebung %4d | mo danach %s%s" % (
        r["f"], rows[i - 1]["hp"], r["hp"], r["gz"], ("%d" % per) if per is not None else "-", hitclip, pst2, dx,
        ",".join(str(w["mo"]) for w in fenster[:24]), ("  [Spucktreffer F%s]" % ",".join(map(str, nahe))) if nahe else ""))
print("Ergebnis: %d Takte, %d mit Treffer-Reaktion; ohne Spucktreffer im Fenster: %d mit Reaktion, %d ohne" % (
    len(takte), mit_reaktion, sauber_mit, sauber_ohne))
