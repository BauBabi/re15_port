"""Leser A: Kopf-Ausschnitte der KI-Bilder und der Nutzer-Beschriftungen (Vorlage 1 entblendet, Vorlage 2 roh) samt Zonen-Lab.
    python re15_port/tools/r41_sichel/re2kopf/vorlagen_kopf.py OUTDIR
Kopfboxen der KI-Bilder: von Hand am Bild gesetzt (Bildkoordinaten 1024x1024, inkl. Unterkiefer), siehe KI unten.
Beschreibungs-Kopfboxen: Messblatt V4 Abschnitt 2 / v4_gemein.KB_ORIG (vorn x 368-416 y 139-191, seitlich x 412-467 y 190-263).
Zonen wie re2_kopf.py (Anteile der Kopfbox): Kuppe / links-Mitte / rechts-Mitte / Maul.
"""
import os, sys, json
import numpy as np
from PIL import Image
from skimage.color import rgb2lab
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v4'))
import v4_gemein as G   # noqa: E402  (vorlage(), KB_ORIG)

OUT = sys.argv[1]; os.makedirs(OUT, exist_ok=True)
V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
KI = {
    'ki1_front': ('image_1.png', (205, 5, 305, 115)),
    'ki1_rechts': ('image_1.png', (925, 80, 1010, 170)),
    'ki2_front': ('image_2 (2).png', (205, 15, 300, 110)),
    'ki2_rechts': ('image_2 (2).png', (865, 85, 945, 160)),
    'ki3_front': ('image_3 (3).png', (205, 10, 290, 110)),
    'ki3_rechts': ('image_3 (3).png', (590, 45, 660, 125)),
}
ZONEN = dict(kuppe=(0, 0, 1, .25), links_mitte=(0, .25, .5, .65), rechts_mitte=(.5, .25, 1, .65), maul=(0, .65, 1, 1))


def lab_mittel(rgb):
    px = np.asarray(rgb, float).reshape(-1, 3)
    L = rgb2lab(px.reshape(-1, 1, 3) / 255.0).reshape(-1, 3)
    return [round(float(x), 1) for x in L.mean(0)] + [[int(round(x)) for x in px.mean(0)]]


def zonen(crop):
    h, w = crop.shape[:2]
    out = {}
    for k, (u0, v0, u1, v1) in ZONEN.items():
        out[k] = lab_mittel(crop[int(v0 * h):int(v1 * h), int(u0 * w):int(u1 * w)])
    out['gesamt'] = lab_mittel(crop)
    return out


res = {}
for nm, (f, box) in KI.items():
    im = np.asarray(Image.open(V80 + f).convert('RGB'))
    x0, y0, x1, y1 = box
    crop = im[y0:y1, x0:x1]
    Image.fromarray(crop).save(os.path.join(OUT, nm + '.png'))
    res[nm] = dict(datei=f, box=box, px=(x1 - x0, y1 - y0), w_h=round((x1 - x0) / (y1 - y0), 3), zonen=zonen(crop))
for b in (1, 2):
    v = np.clip(G.vorlage(b), 0, 255).astype(np.uint8)
    x0, y0, x1, y1 = G.KB_ORIG[b]
    crop = v[y0:y1 + 1, x0:x1 + 1]
    Image.fromarray(crop).save(os.path.join(OUT, 'beschreibung%d_kopf.png' % b))
    res['beschreibung%d' % b] = dict(datei='Beschreibung%d_Vorlage.png' % b + (' (entblendet k=0,766/0,764/0,768, SW 17)' if b == 1 else ''),
                                    box=G.KB_ORIG[b], px=(x1 - x0 + 1, y1 - y0 + 1), w_h=round((x1 - x0 + 1) / (y1 - y0 + 1), 3), zonen=zonen(crop))
json.dump(res, open(os.path.join(OUT, 'vorlagen.json'), 'w'), indent=1)
for k, v in res.items():
    print(k, v['px'], 'w/h', v['w_h'], {z: v['zonen'][z][:3] for z in v['zonen']})
