"""NB4 N5: ID-Puffer-Bilder (grau Hintergrund, rot Form, weiss Annette) fuer den Satz: heutige Lage in Cut 11 (Ruecken) und
Cut 15 (Form vor dem Auge), dazu die besten Treffer der Suche suche5.py in Cut 11 und Cut 14 (Gesicht sichtbar, aber Szene
gespiegelt und Annette klein). Ausgabe: AN_belege/n4/n5_satz_kameras.png"""
import sys, os, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/befunde_runde41/AN_belege/werkzeug/n3')
from szmass import *
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
L = (-1378, -23395); S = (3700, -22500)
erg = json.load(open(os.path.join(HIER, 'suche5.json')))
def best(c):
    h = [e for e in erg if e['cut'] == c]
    return max(h, key=lambda e: (e['bbox'][3] - e['bbox'][1])) if h else None
faelle = [(11, S, L, 'heute Cut 11: Ruecken (Winkel 134 Grad)'), (15, S, L, 'heute Cut 15: Form 70 %, Annette 25 % frei')]
for c in (11, 14):
    e = best(c)
    if e: faelle.append((c, tuple(e['A']), tuple(e['F']), 'Suche Cut %d: A %s F %s, Annette %dx%d px' % (
        c, tuple(e['A']), tuple(e['F']), e['bbox'][2] - e['bbox'][0], e['bbox'][3] - e['bbox'][1])))
pal = np.array([[40, 40, 40], [200, 60, 60], [240, 240, 240]], np.uint8)
bl = Image.new('RGB', (2 * 640, 2 * 500), (255, 255, 255)); d = ImageDraw.Draw(bl)
for i, (c, A, F, t) in enumerate(faelle):
    buf, al = idpuffer(c, [(FORM, 2, 0, (F[0], 0, F[1]), blick(*F, *A), 1), (FRAU, 1, 0, (A[0], 0, A[1]), blick(*A, *F), 2)])
    x, y = (i % 2) * 640, (i // 2) * 500
    bl.paste(Image.fromarray(pal[np.clip(buf, 0, 2)]), (x, y + 20)); d.text((x + 4, y + 4), t, fill=(0, 0, 0))
aus = os.path.join(HIER, '..', '..', 'n4', 'n5_satz_kameras.png')
bl.save(aus); print(aus)
