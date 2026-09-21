# -*- coding: utf-8 -*-
"""Zensus A, Schritt 14: FIXVORSCHLAG am Buerostuhl vorfuehren.

Vorschlag: die Tiefschwarz-Regel liefert nicht die Silhouette eines Moebels, sondern jede
dunkle Kanten- und Schattenlinie in der Kasten-Silhouette. Statt sie rohe zu nehmen, wird
sie morphologisch GEOEFFNET (3x3-Erosion, dann Dilatation) und es bleiben nur Strukturen,
die mindestens 3 Punkte breit sind - also Koerper statt Striche. Loecher im Koerper werden
gefuellt (ein gefuellter Zwischenraum zeigt genau das, was man dort durch das Moebel saehe
- dieselbe Begruendung wie bei der Pflanzen-Huelle ROOM10D0 C6).

Gemessen wird der Effekt am BILD und an der Figur.
"""
import io, json, os, struct, sys
import numpy as np
from scipy import ndimage as nd
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum, maske_aus_png
from PIL import Image, ImageDraw

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
OUT = 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten'
DUMP = 'build/floor_dump.txt'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
K3 = np.ones((3, 3), bool)


def oeffnen(reg):
    r = nd.binary_dilation(nd.binary_erosion(reg, K3), K3)
    return nd.binary_fill_holes(r)


room, cut = 'ROOM10F0', 4
rid = int(room[4:], 16)
rdt, st = geom.load_rdt(CD, room)
cam = struct.unpack_from('<I', rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, cut)
bg = geom.load_bg(PPM, rid, cut)
e = raum.eintrag(AUS[room][str(cut)])
objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')

print('%-42s %7s %7s %7s %7s %7s' % ('Objekt', 'jetzt', 'duenn', 'neu', 'duenn', 'verlor'))
alt_ges = np.zeros((240, 320), bool)
neu_ges = np.zeros((240, 320), bool)
for o in objs:
    r = o[1]
    n = oeffnen(r)
    dn_a = int((r & ~nd.binary_erosion(r, K3)).sum())
    dn_n = int((n & ~nd.binary_erosion(n, K3)).sum())
    print('%-42s %7d %7d %7d %7d %7d'
          % (o[0][:42], int(r.sum()), dn_a, int(n.sum()), dn_n, int((r & ~n).sum())))
    alt_ges |= r
    neu_ges |= n

print()
print('GESAMT C4: jetzt %d Punkte (%.1f %% duenn) -> geoeffnet %d Punkte (%.1f %% duenn)'
      % (int(alt_ges.sum()), 100.0 * (alt_ges & ~nd.binary_erosion(alt_ges, K3)).sum() / alt_ges.sum(),
         int(neu_ges.sum()), 100.0 * (neu_ges & ~nd.binary_erosion(neu_ges, K3)).sum() / max(1, neu_ges.sum())))

# Wirkung an der Figur: derselbe Standplatz wie im Funkel-Bild, exakte Punkttiefe
wx, wz = 4600, 7400
dep_alt = np.zeros((240, 320), np.int32)
dep_neu = np.zeros((240, 320), np.int32)
for o in objs:
    if o[9] is None:
        continue
    q = [int(x) for x in o[9]]
    vz, tr = geom.quader_tiefe(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4])
    for reg, dep in ((o[1] & tr, dep_alt), (oeffnen(o[1]) & tr, dep_neu)):
        d = geometrie.rastern(np.where(reg, vz, 0.0)) * reg
        np.copyto(dep, np.where(reg & (d > 0) & ((dep == 0) | (d < dep)), d, dep))
pf = abnahme.proj(R, t, H, wx, 0, wz)
pk = abnahme.proj(R, t, H, wx, -abnahme.KOPF, wz)
fsx, fsy, fvz = pf
ksx, ksy, kvz = pk
hw = abnahme.HALB * H / fvz
x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
rows = np.arange(y0, y1)
vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, rows)
vzs = np.where(ok, vzs, fvz)[:, None]
va = alt_ges[y0:y1, x0:x1] & geometrie.verdeckt(dep_alt[y0:y1, x0:x1], vzs)
vn = neu_ges[y0:y1, x0:x1] & geometrie.verdeckt(dep_neu[y0:y1, x0:x1], vzs)
print('Standplatz (%d,%d): verdeckende Punkte jetzt %d -> geoeffnet %d (Koerperkasten %d Punkte)'
      % (wx, wz, int(va.sum()), int(vn.sum()), (x1 - x0) * (y1 - y0)))

# Bild
Z = 3
def lege(reg, col):
    ov = bg.astype(float) * 0.45
    ov[reg] = ov[reg] * 0.2 + np.array(col) * 0.8
    return ov.astype(np.uint8)
im = Image.new('RGB', (320 * Z * 3 + 16, 240 * Z + 18), (12, 12, 12))
im.paste(Image.fromarray(bg.astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST), (0, 18))
im.paste(Image.fromarray(lege(alt_ges, (255, 0, 200))).resize((320 * Z, 240 * Z), Image.NEAREST), (320 * Z + 8, 18))
im.paste(Image.fromarray(lege(neu_ges, (0, 230, 120))).resize((320 * Z, 240 * Z), Image.NEAREST), (640 * Z + 16, 18))
d = ImageDraw.Draw(im)
d.text((4, 4), 'ROOM10F0 C4  links Hintergrund | Mitte AUSGELIEFERT (%d Punkte, 43.4 %% duenne Striche) | '
               'rechts geoeffnet (%d Punkte, %.1f %% duenn) - Koerper statt Striche'
       % (int(alt_ges.sum()), int(neu_ges.sum()),
          100.0 * (neu_ges & ~nd.binary_erosion(neu_ges, K3)).sum() / max(1, neu_ges.sum())), fill=(255, 255, 255))
p = os.path.join(OUT, 'fixdemo_ROOM10F0_C4.png')
im.save(p)
print(p)
