# Abnahme 8 (eigenes Werkzeug): Biss-Fenster des LUNGE (Gator-Clip 4, Bild 6..34) aus der Telemetrie "BISS af= dist= maul= wand= hr=" in gator_boss.log,
# Bild fuer Bild ausgerichtet auf die Gator-Clip-4-Laeufe des state.log (letzter Besuch). Je Clip-4-Lauf: Treffer im Fenster? Bilder, in denen die
# Bissbedingung ausser der Bisssperre erfuellt war (Reichweite maul<=1500 oder dist<1000, wand=0, hr=0) ohne Treffer = "gesperrter Biss".
# Aufruf: python a8biss.py <laufordner> [-v]
import sys, re, os
d = sys.argv[1]; V = '-v' in sys.argv
gb = open(os.path.join(d, 'gator_boss.log'), encoding='latin-1').read().splitlines()
st = open(os.path.join(d, 'state.log'), encoding='latin-1').read().splitlines()
rb = re.compile(r'^BISS af=(\d+) dist=(-?\d+) maul=(-?\d+) wand=(-?\d+) hr=(-?\d+)')
B = []
for z in gb:
    m = rb.match(z)
    if m: B.append(tuple(int(x) for x in m.groups()))
rs = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+)')
rg = re.compile(r'\[\d+ t=\d+ st=\d+ ss1=\d+ ss2=\d+ ss3=\d+ g=\S+ mo=(\d+) af=(\d+) stun=(\d+)')
fr = []
for z in st:
    m = rs.match(z); g = rg.search(z)
    if m: fr.append((int(m.group(1)), int(m.group(5)), int(g.group(1)) if g else -1, int(g.group(2)) if g else -1, int(g.group(3)) if g else -1, int(m.group(6))))
# alle Besuche: Bilder mit Gator mo=4 und af 6..34 (in Reihenfolge) - die BISS-Zeilen werden im Gegner-Pass VOR dem af++ geschrieben (af der Zeile = af im state.log - 1)
seq = []
vis = 0; prevF = -1
for k, f in enumerate(fr):
    if f[0] <= prevF: vis += 1
    prevF = f[0]
    if f[1] < 0: break                      # nach dem toedlichen Biss (hp < 0) beginnt das Fressen mit Clip 4 - nicht mehr ausrichten
    if f[2] == 4 and 7 <= f[3] <= 35: seq.append((vis, k))
print('%s: BISS-Zeilen %d, state.log-Bilder Gator mo=4 af 7..35 (alle Besuche) %d' % (os.path.basename(os.path.normpath(d)), len(B), len(seq)))
# Ausrichtung: je BISS-Zeile das naechste state.log-Bild mit mo=4 und af = BISS-af + 1 (gleicher Besuch-Index egal)
j = 0; pair = []; miss = 0
for b in B:
    while j < len(seq) and fr[seq[j][1]][3] != b[0] + 1: j += 1
    if j >= len(seq): miss += 1; continue
    pair.append((b, seq[j])); j += 1
# Clip-4-Laeufe: neue Gruppe, wenn das BISS-af nicht um 1 steigt oder der state-Index nicht um 1 steigt
grp = []; cur = []
for b, s in pair:
    if cur and not (b[0] == cur[-1][0][0] + 1 and s[1] == cur[-1][1][1] + 1): grp.append(cur); cur = []
    cur.append((b, s))
if cur: grp.append(cur)
nvis = max(v for v, k in seq) if seq else 0
def reach(b): return (b[2] <= 1500 or b[1] < 1000) and b[3] == 0 and b[4] == 0
tot = dict(grp=0, hit=0, nach=0, nach_frames=0, leer=0, leer_frames=0)
zeilen = []
for g in grp:
    if g[0][1][0] != nvis:                   # fruehere Besuche: nur leere Laeufe melden
        sup0 = [fr[s[1]][0] for (b, s) in g if reach(b)]
        hit0 = any(s[1] > 0 and fr[s[1]][1] < fr[s[1]-1][1] for (b, s) in g)
        if sup0 and not hit0: zeilen.append('  (Besuch %d) F%d-F%d KEIN Treffer, Bissbedingung ohne Sperre erfuellt in %d Bildern' % (g[0][1][0], fr[g[0][1][1]][0], fr[g[-1][1][1]][0], len(sup0)))
        continue
    tot['grp'] += 1
    hitk = None
    for i, (b, s) in enumerate(g):
        k = s[1]
        if k > 0 and fr[k][1] < fr[k-1][1]: hitk = i; break   # hp-Flanke im Bild dieser BISS-Zeile
    F0 = fr[g[0][1][1]][0]; F1 = fr[g[-1][1][1]][0]
    if hitk is not None:
        tot['hit'] += 1
        sup = [fr[s[1]][0] for (b, s) in g[hitk+1:] if reach(b)]
        if sup: tot['nach'] += 1; tot['nach_frames'] += len(sup)
        zeilen.append('  F%d-F%d Treffer F%d; danach Bissbedingung ohne Sperre erfuellt in %d Bildern %s' % (F0, F1, fr[g[hitk][1][1]][0], len(sup), sup[:3]))
    else:
        sup = [fr[s[1]][0] for (b, s) in g if reach(b)]
        if sup:
            tot['leer'] += 1; tot['leer_frames'] += len(sup)
            zeilen.append('  F%d-F%d KEIN Treffer, Bissbedingung ohne Sperre erfuellt in %d Bildern (erstes F%d, maul %d, dist %d)' % (F0, F1, len(sup), sup[0],
                          [b for (b, s) in g if reach(b)][0][2], [b for (b, s) in g if reach(b)][0][1]))
print('  ausgerichtet %d von %d BISS-Zeilen (nicht zuordenbar %d); letzter Besuch: Clip-4-Lauefe mit Bissfenster %d, davon mit Treffer %d; '
      'nach dem Treffer im selben Lauf gesperrte Bisse (frueher M1-Zweitbiss): %d Laeufe / %d Bilder; Laeufe OHNE Treffer, in denen das Maul die Figur '
      'erreichte (Bisssperre): %d Laeufe / %d Bilder' % (len(pair), len(B), miss, tot['grp'], tot['hit'], tot['nach'], tot['nach_frames'], tot['leer'], tot['leer_frames']))
if V:
    for z in zeilen: print(z)
