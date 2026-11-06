"""Bildleisten fuer PF_abnahme_2 (beschriftete Spalten)."""
import numpy as np, json, cv2
from PIL import Image, ImageDraw, ImageFont
import bilder, kasten_mass as km, referenz, affin
OUT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/analysis/befunde_runde41/PF_belege/abnahme2/'
try:
    FONT = ImageFont.truetype('arial.ttf', 18); FK = ImageFont.truetype('arial.ttf', 14)
except Exception:
    FONT = FK = ImageFont.load_default()
def leiste(bilder_, titel, kopf=None, abstand=8, hoehe_titel=26):
    h = max(b.shape[0] for b in bilder_); w = sum(b.shape[1] for b in bilder_) + abstand * (len(bilder_) - 1)
    oben = hoehe_titel + (22 if kopf else 0)
    im = Image.new('RGB', (w, h + oben), (24, 24, 24)); d = ImageDraw.Draw(im)
    if kopf: d.text((4, 2), kopf, fill=(255, 255, 255), font=FK)
    x = 0
    for b, t in zip(bilder_, titel):
        im.paste(Image.fromarray(np.ascontiguousarray(b.astype(np.uint8))), (x, oben))
        d.text((x + 4, oben - hoehe_titel + 3), t, fill=(255, 230, 0), font=FONT)
        x += b.shape[1] + abstand
    return im
def stapel(ims, abstand=10):
    w = max(i.size[0] for i in ims); h = sum(i.size[1] for i in ims) + abstand * (len(ims) - 1)
    out = Image.new('RGB', (w, h), (24, 24, 24)); y = 0
    for i in ims: out.paste(i, (0, y)); y += i.size[1] + abstand
    return out
def gitter(img, schritt, farbe=(0, 255, 255)):
    a = img.copy()
    a[::schritt, :] = (a[::schritt, :] * 0.4 + np.array(farbe) * 0.6).astype(np.uint8)
    a[:, ::schritt] = (a[:, ::schritt] * 0.4 + np.array(farbe) * 0.6).astype(np.uint8)
    return a
if __name__ == "__main__":
    # 1) Kasten Cut 3 im Fenster
    cr = lambda a: a[30:690, 450:840]
    k = {(f, s): np.load('k_k_%s_%s_auf.npy' % (s, f)) for f in ('leon', 'elza') for s in ('vor', 'nach')}
    z = {(f, s): np.load('k_k_%s_%s_zu.npy' % (s, f)) for f in ('leon', 'elza') for s in ('vor', 'nach')}
    stapel([leiste([cr(k[('leon', 'vor')]), cr(k[('leon', 'nach')]), cr(k[('elza', 'vor')]), cr(k[('elza', 'nach')])],
                   ['Leon: vor NB1', 'Leon: nach NB1', 'Elza: vor NB1', 'Elza: nach NB1'],
                   'Kasten ROOM2010/2011, Kamera Cut 3 (@0xC0), Tuer offen, gdigrab 960x720 (Ausschnitt x 450..840, y 30..690)'),
            leiste([cr(z[('leon', 'vor')]), cr(z[('leon', 'nach')]), cr(z[('elza', 'vor')]), cr(z[('elza', 'nach')])],
                   ['Leon: vor NB1', 'Leon: nach NB1', 'Elza: vor NB1', 'Elza: nach NB1'],
                   'dieselbe Stelle, Tuer geschlossen (vor/nach bitgleich, = BG03)')]).save(OUT + 'kasten_cut3_vor_nach.png')
    # 2) entzerrt, mit Gitter (200 Welteinheiten = 50 Bildpunkte)
    rv = {}
    for s in ('vor', 'nach'):
        rv[s] = km.port_flaechen(k[('leon', s)])
    ref = {'innen': referenz.rueckwand(), 'fluegel': referenz.fluegel(131.3)[0]}
    ein = {('leon_nach_%s' % n): affin.registriere(rv['nach'][j], ref[n]) for j, n in enumerate(('innen', 'fluegel'))}
    reihen = []
    for j, n in enumerate(('innen', 'fluegel')):
        e = ein['leon_nach_%s' % n]
        port = rv['nach'][j]
        hh, ww = port.shape[:2]
        vv, uu = np.mgrid[0:hh, 0:ww].astype(np.float32); uu -= ww / 2; vv -= hh / 2
        # Vorlage 2060 in den Port-Rahmen: nur Massstab + Lage der Nachher-Registrierung (keine Scherung)
        # Registrierung lief auf dem Innenausschnitt (Rand 10 % / 6 %): Lage auf den ganzen Rahmen umrechnen
        x0 = int(ww * .10); y0 = int(hh * .06); tw = ww - 2 * x0; th = hh - 2 * y0
        mx = (e['a'] * (np.mgrid[0:hh, 0:ww][1] - x0 - tw / 2) + e['tx']).astype(np.float32)
        my = (e['d'] * (np.mgrid[0:hh, 0:ww][0] - y0 - th / 2) + e['ty']).astype(np.float32)
        rr = cv2.remap(ref[n], mx, my, cv2.INTER_LINEAR)
        titel = ('Rueckwand' if n == 'innen' else 'Fluegel')
        reihen += [gitter(rv['vor'][j], 50), gitter(rv['nach'][j], 50), gitter(rr, 50)]
    leiste(reihen, ['Rueckw. vor NB1', 'Rueckw. nach NB1', 'Rueckw. 2060', 'Fluegel vor NB1', 'Fluegel nach NB1', 'Fluegel 2060'],
           'Fensterbild ueber Cut 3 auf die Port-Flaechen entzerrt (4 Welteinheiten/Punkt, Gitter 200); 2060 = BG06 ueber Cut 6 (@0x120), Fluchtpunkt 131,3; Leon (Elza bildgleich)').save(OUT + 'kasten_entzerrt_vor_nach_2060.png')
    print('Kasten-Leisten fertig')
