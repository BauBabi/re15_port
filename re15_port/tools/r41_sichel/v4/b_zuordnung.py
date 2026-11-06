# Leser B: Belegungstabelle Text x Endpunkt, Wahl, Kreuzungen, Belegbild x3.
import sys, json, itertools, numpy as np, cv2
from PIL import Image, ImageDraw, ImageFont
from collections import deque
sys.path.insert(0, sys.argv[3])
from b_linien_lib import belegung, seg_intersect
V, OUT, _, BEL = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
R = json.load(open(f'{OUT}/B_linien_roh.json'))
NAMEN = {'1': ['Auge', 'Haare', 'Nase'],
         '2': ['Auge von der Seite etwa hier', 'Haare von der Seite', 'Schwanzspitze Ende',
               'Schwanz Beginn in den Ruecken ueberfuehrend.', 'Schwanz Knick, von da an wird es breiter nach oben']}
try: F = ImageFont.truetype('arial.ttf', 22); Fs = ImageFont.truetype('arial.ttf', 16)
except Exception: F = Fs = ImageFont.load_default()
erg = {}
for bi in ('1', '2'):
    r = R[bi]; M = np.load(f'{OUT}/B_linienmaske_{bi}.npy')
    # Absaetze: Textbloecke mit Zeilenabstand <= 10 px zusammenfassen (gemessen: innerhalb 10, zwischen >= 12)
    tb = sorted(r['texte'], key=lambda t: t['y0']); par = [[tb[0]]]
    for t in tb[1:]:
        if t['y0'] - par[-1][-1]['y1'] <= 10: par[-1].append(t)
        else: par.append([t])
    pars = [dict(x0=min(b['x0'] for b in p), y0=min(b['y0'] for b in p), x1=max(b['x1'] for b in p), y1=max(b['y1'] for b in p),
                 zeilen=len(p)) for p in par]
    assert len(pars) == len(NAMEN[bi]) == len(r['starts']), (len(pars), len(r['starts']))
    # Start je Absatz: naechster Start zur linken Kante (Mitte) des Absatzes
    anker = []
    for p in pars:
        a = (p['x0'], (p['y0'] + p['y1']) / 2)
        s = min(r['starts'], key=lambda s: np.hypot(s[0] - a[0], s[1] - a[1]))
        anker.append(dict(text_anker_links_mitte=a, linienstart=s, abstand=round(float(np.hypot(s[0] - a[0], s[1] - a[1])), 1)))
    E = r['enden']
    tab = [[round(belegung(M, an['linienstart'], e), 4) for e in E] for an in anker]
    # beste Gesamtzuordnung (Permutation mit max Summe), plus je Text: gewaehlt vs beste Alternative
    best = max(itertools.permutations(range(len(E)), len(anker)), key=lambda pm: sum(tab[i][j] for i, j in enumerate(pm)))
    zeilen = []
    for i, an in enumerate(anker):
        j = best[i]; alt = max((tab[i][k], k) for k in range(len(E)) if k != j)
        zeilen.append(dict(text=NAMEN[bi][i], textbox=pars[i], **an, ende=E[j], belegung=tab[i][j],
                           beste_alternative=E[alt[1]], belegung_alternative=alt[0]))
    # Knick der Auge-Linie in Bild 1 (Haken): Ecke per approxPolyDP auf der Linienkomponente
    if bi == '1':
        z = zeilen[0]; s, e = z['linienstart'], z['ende']
        # geodaetischer Weg in der Maske
        H, W = M.shape; prev = {tuple(s[::-1]): None}; q = deque([tuple(s[::-1])])
        while q:
            c = q.popleft()
            if c == tuple(e[::-1]): break
            for dy, dx in ((1,0),(-1,0),(0,1),(0,-1),(1,1),(1,-1),(-1,1),(-1,-1)):
                n = (c[0] + dy, c[1] + dx)
                if 0 <= n[0] < H and 0 <= n[1] < W and M[n] and n not in prev: prev[n] = c; q.append(n)
        path = []; c = tuple(e[::-1])
        while c is not None: path.append(c); c = prev[c]
        pts = np.array([[p[1], p[0]] for p in path[::-1]], np.int32).reshape(-1, 1, 2)
        poly = cv2.approxPolyDP(pts, 2.0, False).reshape(-1, 2).tolist()
        z['polylinie'] = poly
        z['belegung_polylinie'] = round(float(np.mean([belegung(M, poly[k], poly[k + 1]) for k in range(len(poly) - 1)])), 4)
        z['hinweis'] = 'Linie mit Haken: Endpunkt = Spitze des Hakens (Strichende); Ecke ' + str(poly[-2])
        # Zeile Auge neu: Polylinie S -> (Ecken der gemessenen Linie ohne das Ende) -> E_j, laengengewichtet
        def bel_poly(pp):
            L = [np.hypot(pp[k+1][0]-pp[k][0], pp[k+1][1]-pp[k][1]) for k in range(len(pp)-1)]
            return round(float(sum(l * belegung(M, pp[k], pp[k+1]) for k, l in enumerate(L)) / sum(L)), 4)
        tab[0] = [bel_poly(poly[:-1] + [e]) for e in E]
        z['belegung_gerade_S_E'] = z['belegung']; z['belegung'] = tab[0][E.index(z['ende'])]
        alt = max((tab[0][k], k) for k in range(len(E)) if E[k] != z['ende'])
        z['beste_alternative'] = E[alt[1]]; z['belegung_alternative'] = alt[0]
        z['belegung_polylinie'] = z['belegung']
    # Kreuzungen der gewaehlten Strecken
    kr = []
    for a, b in itertools.combinations(range(len(zeilen)), 2):
        pa = zeilen[a].get('polylinie') or [zeilen[a]['linienstart'], zeilen[a]['ende']]
        pb = zeilen[b].get('polylinie') or [zeilen[b]['linienstart'], zeilen[b]['ende']]
        p = None
        for u in range(len(pa) - 1):
            for v in range(len(pb) - 1):
                p = p or seg_intersect(pa[u], pa[u + 1], pb[v], pb[v + 1])
        if p: kr.append(dict(linien=[zeilen[a]['text'], zeilen[b]['text']], punkt=p, im_bildteil=p[0] < 640))
    erg[bi] = dict(tabelle=dict(zeilen=[n for n in NAMEN[bi]], spalten=E, werte=tab), zuordnung=zeilen, kreuzungen=kr)
    print('Bild', bi, 'Enden', E)
    for n, row in zip(NAMEN[bi], tab): print('  %-50s' % n[:50], row)
    for z in zeilen: print('  =>', z['text'][:40], 'Start', z['linienstart'], 'Ende', z['ende'], 'Bel', z['belegung'], z.get('belegung_gerade_S_E',''), '| Alt', z['beste_alternative'], z['belegung_alternative'], z.get('polylinie', ''))
    print('  Kreuzungen', kr)
    # Belegbild x3
    im = Image.open(f'{V}/Beschreibung{bi}.png').convert('RGB')
    im = im.resize((im.width * 3, im.height * 3), Image.NEAREST); d = ImageDraw.Draw(im)
    cols = [(0, 255, 255), (255, 255, 0), (0, 255, 0), (255, 0, 255), (60, 140, 255)]
    for i, z in enumerate(zeilen):
        c = cols[i]; pts = z.get('polylinie') or [z['linienstart'], z['ende']]
        d.line([(p[0] * 3 + 1, p[1] * 3 + 1) for p in pts], fill=c, width=2)
        ex, ey = z['ende']; d.ellipse((ex * 3 - 7, ey * 3 - 7, ex * 3 + 9, ey * 3 + 9), outline=c, width=3)
        lab = f"{i + 1}: {z['text'][:28]}  E=({ex},{ey}) Bel {z['belegung']*100:.1f}% / Alt {z['belegung_alternative']*100:.1f}%"
        d.text((8, 8 + 26 * i), lab, fill=c, font=F, stroke_width=2, stroke_fill=(0, 0, 0))
        d.text((ex * 3 - 40, ey * 3 + 12), str(i + 1), fill=c, font=F, stroke_width=2, stroke_fill=(0, 0, 0))
    for k in kr:
        x, y = k['punkt']; d.rectangle((x * 3 - 6, y * 3 - 6, x * 3 + 6, y * 3 + 6), outline=(255, 255, 255), width=2)
    im.save(f'{BEL}/B_L1_linien_bild{bi}_x3.png')
json.dump(erg, open(f'{OUT}/B_L1.json', 'w'), indent=1, ensure_ascii=False)
