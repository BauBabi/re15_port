# Abnahme 5: Port-Messzeile RE15_G5A_LAMPE_FARBEN=-6000 (eigene Laeufe) gegen die RE2-Pakete s04 Objekt 4 Puffer 0 (pakete5b.py)
import json, re, sys
re2 = json.load(open(sys.argv[1]))
for lauf in sys.argv[2:]:
    port = {}
    for z in open(lauf + '/debug.log', encoding='latin-1'):
        m = re.match(r'\[g5lampe-farben\] ([TQ])(\d+) Fach (\d+) OTZ (\d+) xy (.*?) rgb (.*)$', z.strip())
        if not m: continue
        xy = [tuple(map(int, p)) for p in re.findall(r'\((-?\d+),(-?\d+)\)', m.group(5))]
        rgb = [tuple(map(int, p)) for p in re.findall(r'\((\d+),(\d+),(\d+)\)', m.group(6))]
        n = 4 if m.group(1) == 'Q' else 3
        port[(m.group(1), int(m.group(2)))] = (xy[:n], rgb[:n])
    gleich = 0; abw = []
    for (art, i), (xy, rgb) in sorted(port.items()):
        c, v, code = re2[art]['0'][i]
        v = [tuple(p) for p in v]; c = [tuple(p) for p in c]
        if v == xy and c == rgb: gleich += 1
        else: abw.append((art, i, xy, v, rgb, c))
    fehlt = [('%s%02d' % (a, i)) for a in 'TQ' for i in range(18 if a == 'T' else 19) if (a, i) not in port]
    print('%s: %d Polygone gezeichnet, %d gleich RE2 (Lage + Eckfarben), nicht gezeichnet: %s' % (lauf.split('/')[-1], len(port), gleich, ' '.join(fehlt)))
    for a in abw: print('   ABWEICHUNG', a)
    for f in fehlt:
        a, i = f[0], int(f[1:]); c, v, code = re2[a]['0'][i]; print('   RE2 %s: Lage %s Farbe %s' % (f, v, c[0]))
