# Leser B (SF V4) L2/L5: Strukturen an den Linienenden, Kopfboxen (Video n1485/n1435), Abblende Bild 1.
# Aufruf: python b_messen.py VERSION80 SFV4 BELEGE
import sys, json, numpy as np, cv2
from PIL import Image
from scipy import ndimage as ndi
import b_common as C
V, S, BEL = sys.argv[1:4]
L1 = json.load(open(f'{S}/B_L1.json')); HJ = json.load(open(f'{S}/B_H.json'))
H1, H2 = HJ['bild1']['H_px'], HJ['bild2']['H_px']
I1 = np.asarray(Image.open(f'{V}/Beschreibung1_Vorlage.png').convert('RGB')).astype(float)
I2 = np.asarray(Image.open(f'{V}/Beschreibung2_Vorlage.png').convert('RGB')).astype(float)
OUT = {}
# ---- Abblende gegen stehenden Hintergrund (Mittel n1447..1471), Wand links x<250, y 40..330
bg = np.mean([C.bild(S, n) for n in range(1447, 1472)], axis=0)
wm = np.zeros((480, 640), bool); wm[40:330, :250] = True
def fade(a):
    kd = []
    for c in range(3):
        x = bg[..., c][wm]; y = a[..., c][wm]; A = np.vstack([x, np.ones_like(x)]).T; (k, d), *_ = np.linalg.lstsq(A, y, rcond=None)
        kd.append((float(k), float(d), float(np.sqrt(((y - k * x - d) ** 2).mean()))))
    return kd
KD1 = fade(I1); KD2 = fade(I2)
OUT['abblende'] = dict(bild1_n1485=[[round(k, 4), round(d, 2), round(r, 2)] for k, d, r in KD1],
                       bild2_n1435=[[round(k, 4), round(d, 2), round(r, 2)] for k, d, r in KD2],
                       bezug='Mittel n1447..1471, Wand links x<250 y40..330; y = k*x + d je Kanal R,G,B; [k, d, rms]',
                       schwarzwert_letterbox_oben_n1435=round(float(I2[:40].mean()), 2), schwarzwert_letterbox_oben_n1485=round(float(I1[:40].mean()), 2))
I1k = np.stack([(I1[..., c] - KD1[c][1]) / KD1[c][0] for c in range(3)], -1)
np.save(f'{S}/B_I1k.npy', I1k); json.dump(KD1, open(f'{S}/B_KD1.json', 'w'))
VAR1 = dict(roh=I1, schwarz_ab=C.schwarz_ab(I1), abblende_korr=I1k, abblende_korr_schwarz_ab=C.schwarz_ab(I1k))
VAR2 = dict(roh=I2, schwarz_ab=C.schwarz_ab(I2))
def lstar(img): return C.lab(img.reshape(-1, 3)).reshape(img.shape)
LAB1, LAB2 = lstar(I1), lstar(I2)
def allstats(VAR, m): return {k: C.stats(v, m) for k, v in VAR.items()}
# ---- L2 Struktur am Endpunkt: L*-Mitte 3x3 gegen Ring (15x15 ohne 9x9); Schwelle = Mittelwert; Region = 8-Komponente
#      der Pixel auf der Seite der Mitte im 25x25-Fenster, die den Endpunkt (oder dessen naechstes Pixel) enthaelt.
def struktur(LAB, x, y, H):
    L = LAB[..., 0]
    c3 = float(L[y - 1:y + 2, x - 1:x + 2].mean())
    w15 = C.fenster(x, y, 15); w9 = C.fenster(x, y, 9); ring = float(L[w15 & ~w9].mean())
    t = (c3 + ring) / 2; hell = c3 > ring
    side = (L > t) if hell else (L < t); side &= C.fenster(x, y, 25)
    lab, _ = ndi.label(side, structure=np.ones((3, 3)))
    k = lab[y, x]
    if k == 0:
        ys, xs = np.nonzero(side); i = np.argmin((xs - x) ** 2 + (ys - y) ** 2); k = lab[ys[i], xs[i]]
    R = lab == k; ys, xs = np.nonzero(R)
    hull = cv2.convexHull(np.c_[xs, ys].astype(np.int32)); ha = float(cv2.contourArea(hull))
    dist_rand = float(ndi.distance_transform_edt(R)[y, x]) if R[y, x] else -float(ndi.distance_transform_edt(~R)[y, x])
    gx = ndi.sobel(L, 1)[y, x]; gy = ndi.sobel(L, 0)[y, x]
    return dict(L_mitte3x3=round(c3, 1), L_ring15ohne9=round(ring, 1), kontrast=round(c3 - ring, 1), art='hell' if hell else 'dunkel',
                schwelle_L=round(t, 1), region_px=int(R.sum()), region_bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                region_breite_px=int(xs.max() - xs.min() + 1), region_hoehe_px=int(ys.max() - ys.min() + 1),
                region_breite_H=round((xs.max() - xs.min() + 1) / H, 3), region_hoehe_H=round((ys.max() - ys.min() + 1) / H, 3),
                region_schwerpunkt=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)], konvexe_huelle_px2=round(ha, 1),
                abstand_endpunkt_rand_px=round(dist_rand, 1), sobel_L_gx_gy=[round(float(gx), 1), round(float(gy), 1)],
                region_stoesst_an_fensterrand=bool(xs.min() <= x - 12 or xs.max() >= x + 12 or ys.min() <= y - 12 or ys.max() >= y + 12)), R
L2 = {}; MASK = {}
for bi, VAR, LAB, H in (('1', VAR1, LAB1, H1), ('2', VAR2, LAB2, H2)):
    for z in L1[bi]['zuordnung']:
        x, y = z['ende']; nm = z['text']
        st, R = struktur(LAB, x, y, H); MASK[bi + ':' + nm] = R
        Re = ndi.binary_erosion(R, iterations=2)
        L2[bi + ':' + nm] = dict(ende=[x, y], fenster9=allstats(VAR, C.fenster(x, y, 9)), fenster15=allstats(VAR, C.fenster(x, y, 15)),
                                 struktur=st, region_2px_erodiert=allstats(VAR, Re) if Re.sum() >= 3 else 'nach 2 px Erosion leer/zu klein (%d px)' % int(Re.sum()))
np.savez_compressed(f'{S}/B_L2_regionen.npz', **{k.replace(':', '_').replace(' ', '_').replace(',', '').replace('.', ''): v for k, v in MASK.items()})
OUT['L2'] = L2
# ---- L5 Kopfbox vorn (n1485)
def oberkante(a, xs, y0, y1, thr=8, run=5):
    RG = a[..., 0] - a[..., 1]; out = {}
    for x in xs:
        col = RG[y0:y1, x] >= thr
        for yy in range(len(col) - run):
            if col[yy:yy + run].all(): out[x] = y0 + yy; break
    return out
ok = oberkante(I1, range(330, 451), 110, 230); top = min(ok.values())
xm = min(ok, key=ok.get); x0 = xm; x1 = xm
while x0 - 1 in ok and ok[x0 - 1] <= top + 10: x0 -= 1
while x1 + 1 in ok and ok[x1 + 1] <= top + 10: x1 += 1
a_ = ndi.uniform_filter(LAB1[..., 1], 5); unter = []
for x in range(x0 + 5, x1 - 4):
    col = a_[top + 15:262, x]; plateau = float(np.median(a_[top + 18:top + 45, x])); peak = float(col.max()); half = (plateau + peak) / 2
    i = int(np.argmax(col >= half)); unter.append(top + 15 + i)
yb = int(np.median(unter))
kb1 = dict(x0=x0, x1=x1, y0=top, y1=yb, breite_px=x1 - x0 + 1, hoehe_px=yb - top,
           breite_H=round((x1 - x0 + 1) / H1, 3), hoehe_H=round((yb - top) / H1, 3),
           B_durch_H=round((x1 - x0 + 1) / (yb - top), 2), mitte=[(x0 + x1) / 2, (top + yb) / 2],
           methode='Spalten mit Oberkante (R-G>=8 ueber 5 Zeilen) <= Minimum+10, zusammenhaengend um das Minimum; Unterkante = je Spalte erste Zeile ab Oberkante+15, in der a* (5x5 geglaettet) die halbe Hoehe zwischen Gesichts-Plateau (Median Oberkante+18..+45) und Brust-Gipfel erreicht; Median ueber die Spalten',
           oberkante_profil={str(k): v for k, v in ok.items() if k % 5 == 0}, unterkante_spalten=unter)
rel = {}
for z in L1['1']['zuordnung']:
    x, y = z['ende']
    rel[z['text']] = dict(x_rel=round((x - kb1['x0']) / kb1['breite_px'], 3), y_rel=round((y - kb1['y0']) / kb1['hoehe_px'], 3),
                          dx_mitte_px=round(x - kb1['mitte'][0], 1), dy_mitte_px=round(y - kb1['mitte'][1], 1),
                          dx_mitte_H=round((x - kb1['mitte'][0]) / H1, 3), dy_mitte_H=round((y - kb1['mitte'][1]) / H1, 3),
                          seite='(B) links = (W) rechts' if x < kb1['mitte'][0] else '(B) rechts = (W) links')
E = {z['text']: z['ende'] for z in L1['1']['zuordnung']}
paare = {}
for a, b in (('Auge', 'Haare'), ('Auge', 'Nase'), ('Haare', 'Nase')):
    dx, dy = E[b][0] - E[a][0], E[b][1] - E[a][1]
    paare[f'{a}->{b}'] = dict(dx_px=dx, dy_px=dy, dist_px=round(float(np.hypot(dx, dy)), 1), dx_H=round(dx / H1, 3), dy_H=round(dy / H1, 3),
                              dist_H=round(float(np.hypot(dx, dy)) / H1, 3))
cx = int(round(kb1['mitte'][0]))
prof = [dict(y=y, L=round(float(LAB1[y, cx - 8:cx + 9, 0].mean()), 1), a=round(float(LAB1[y, cx - 8:cx + 9, 1].mean()), 1)) for y in range(top, yb + 12)]
OUT['L5_vorn'] = dict(kopfbox=kb1, endpunkte_relativ=rel, abstaende=paare, profil_kopfmitte_x=[cx - 8, cx + 8], profil=prof)
# ---- L5 Kopf seitlich (n1435): Abweichung vom stehenden Pfeiler-Hintergrund
bgs = np.median([C.bild(S, n) for n in (1391, 1401, 1411, 1421, 1446, 1451, 1456, 1461)], axis=0)
D = np.abs(I2.mean(2) - bgs.mean(2)); Dm = (D >= 12); Dm[:, :455] = False; Dm[:200] = False; Dm[275:] = False; Dm[:, 475:] = False
lab, _ = ndi.label(Dm, structure=np.ones((3, 3)))
auge = L1['2']['zuordnung'][0]['ende']; haar = L1['2']['zuordnung'][1]['ende']
ks = {int(lab[yy, xx]) for yy in range(haar[1] - 3, haar[1] + 4) for xx in range(haar[0] - 6, haar[0] + 2) if lab[yy, xx]}
R = np.isin(lab, list(ks)); ys, xs = np.nonzero(R)
OUT['L5_seite'] = dict(
    methode='|L(n1435) - Median L(n1391,1401,1411,1421,1446,1451,1456,1461)| >= 12 im Pfeiler-Streifen x 455..474, y 200..274; Komponente am Haare-Ende',
    region_px=int(R.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
    auge_haare=dict(dx_px=haar[0] - auge[0], dy_px=haar[1] - auge[1], dist_px=round(float(np.hypot(haar[0] - auge[0], haar[1] - auge[1])), 1),
                    dist_H=round(float(np.hypot(haar[0] - auge[0], haar[1] - auge[1])) / H2, 3)))
np.save(f'{S}/B_seite_haarstreifen.npy', R)
Image.fromarray((np.clip(D * 6, 0, 255)).astype(np.uint8)).crop((400, 190, 500, 280)).resize((400, 360), Image.NEAREST).save(f'{BEL}/B_L5_seite_abweichung_pfeiler_x4.png')
json.dump(OUT, open(f'{S}/B_L2_L5.json', 'w'), indent=1, ensure_ascii=False)
kk = dict(kb1); kk.pop('oberkante_profil'); kk.pop('unterkante_spalten')
print('abblende', OUT['abblende']); print('kopfbox', kk); print('rel', rel); print('paare', paare); print('seite', OUT['L5_seite'])
for k, v in L2.items():
    s = v['struktur']
    print(k[:32], v['ende'], s['art'], 'kontr', s['kontrast'], 'reg', s['region_px'], s['region_bbox'], 'B/H', s['region_breite_H'], s['region_hoehe_H'], 'huelle', s['konvexe_huelle_px2'],
          'rand', s['abstand_endpunkt_rand_px'], 'stoesst', s['region_stoesst_an_fensterrand'], '| roh9', v['fenster9']['roh']['rgb'], v['fenster9']['roh']['lab'])
print('Profil Mitte (y,L,a)', [(p['y'], p['L'], p['a']) for p in prof])
