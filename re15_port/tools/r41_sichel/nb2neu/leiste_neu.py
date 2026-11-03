"""NB2neu Pflicht-Leisten: vier beschriftete Spalten Video 4 | NB2 6a1e9430d | r41/sichel 3f91e5c57 | neu (r41/sichel_nb2), gleiches Bild h
je Zeile, grosse Kacheln. Wo der Nutzer ein KI-hochskaliertes Einzelbild genau dieses h geliefert hat (zuordnung.py), steht es in der
Videospalte (Beschriftung "hochskaliert"); sonst das Videobild mit abgezogenem Schwarzwert. Messzeilen je Kachel: art 'griff'
(Abstand Sichelhand - Griffknochen, griff6.py), 'knie' (Kniewinkel, beine6.py), sonst keine.
python leiste_neu.py <aus.jpg> <titel> x0 y0 x1 y1 f h,h,.. NB2lauf,vorherlauf,neulauf art [hell]"""
import sys, os, json, glob
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c6 import *
import griff6, beine6
KOPF = ('Video 4 (Schwarzwert abgezogen; hochskaliert = Bild des Nutzers)', 'NB2 6a1e9430d', 'r41/sichel 3f91e5c57', 'neu r41/sichel_nb2')
KURZ = ('Video', 'NB2', '3f91e5c57', 'neu')
Z = json.load(open(os.path.join(RUNS, 'zuordnung_screenshots.json')))
HOCH = {v['h']: os.path.join(r'C:/workspace/git/reAi_v2/Version_80%', k) for k, v in Z.items() if v['ncc'] > 0.95}
def videobild(h):
    if h in HOCH:
        return cv2.resize(cv2.imread(HOCH[h]), (640, 480), interpolation=cv2.INTER_AREA), True
    return vsw(vid(h)), False
def kachel(im, box, f, titel, unten=None, hell=1.0):
    x0, y0, x1, y1 = box
    im = np.clip(im.astype(np.float32) * hell, 0, 255).astype(np.uint8)
    t = zoom(im, x0, y0, x1, y1, f=f, grid=50, lab=100)
    label(t, titel, (5, 18), 0.5)
    if unten:
        cv2.rectangle(t, (0, t.shape[0] - 20), (t.shape[1], t.shape[0]), (0, 0, 0), -1)
        cv2.putText(t, unten, (4, t.shape[0] - 6), 0, 0.42, (255, 255, 255), 1)
    return t
def main():
    out, titel = sys.argv[1], sys.argv[2]
    box = tuple(int(v) for v in sys.argv[3:7]); f = float(sys.argv[7])
    hs = [int(v) for v in sys.argv[8].split(',')]; runs = sys.argv[9].split(','); art = sys.argv[10]
    hell = float(sys.argv[11]) if len(sys.argv) > 11 else 1.0
    G = {r: griff6.abstaende(r) for r in runs} if art == 'griff' else {}
    B = {r: beine6.beine(r) for r in runs} if art == 'knie' else {}
    rows = []
    for h in hs:
        v, hoch = videobild(h)
        r = [kachel(v, box, f, 'Video h%d%s' % (h, ' hochskaliert' if hoch else ''), hell=1.0 if hoch else hell)]
        for k, run in enumerate(runs):
            im = port(run, h)
            if im is None: im = np.zeros((480, 640, 3), np.uint8)
            u = None
            if art == 'griff':
                d = G[run].get(h); u = ('Griff-Abstand %d Einheiten / %d px' % (d['welt'], d['bild'])) if d else 'kein Spider-Man'
            elif art == 'knie':
                b = B[run].get(h); u = ('Knie %d/%d Grad  Fuss %d' % (b['knie_r'], b['knie_l'], b['fuss'])) if b else None
            r.append(kachel(im, box, f, '%s h%d' % (KURZ[k + 1], h), u, hell))
        rows.append(r)
    kopf = np.zeros((60, sum(t.shape[1] for t in rows[0]), 3), np.uint8)
    cv2.putText(kopf, titel, (8, 22), 0, 0.6, (0, 255, 255), 2)
    x = 0
    for k, t in enumerate(rows[0]):
        cv2.putText(kopf, KOPF[k], (x + 6, 50), 0, 0.42, (255, 255, 255), 1); x += t.shape[1]
    cv2.imwrite(out, np.vstack([kopf] + [np.hstack(r) for r in rows]), [cv2.IMWRITE_JPEG_QUALITY, 88])
main()
