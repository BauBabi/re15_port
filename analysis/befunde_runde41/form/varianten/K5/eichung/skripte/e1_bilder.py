"""E1: Standbilder 1 und 2 im Quellvideo finden, Nachbarbilder je Dia, Mittelbild je Dia.
Eingang: Bilder 3380..3540 des Videos (ffmpeg select + showinfo, Datei n_%04d = Bild 3380 + i - 1), Liste fr/info.txt.
Ausgang: daten/e1_bilder.json, daten/bild{1,2}_mittel.png (Mittel ueber alle Bilder des Dias, float -> uint8 gerundet), Scratchpad bild{1,2}_mittel.npy."""
import sys, os, json, re
import numpy as np, cv2
S, E = sys.argv[1], sys.argv[2]
V80 = 'C:/workspace/git/reAi_v2/Version_80%/'
ref = {1: V80 + 'vlcsnap-2026-10-31-00h28m45s774.png', 2: V80 + 'vlcsnap-2026-10-31-00h29m02s830.png'}
R = {k: cv2.imread(v)[:, :, ::-1].astype(np.float32) for k, v in ref.items()}
info = [l.split() for l in open(S + '/fr/info.txt')]
zeit = {}
for z in open(S + '/fr/info.txt'):
    m = re.search(r'n:\s*(\d+)\s+pts:\s*(\d+)\s+pts_time:([0-9.]+)', z)
    zeit[int(m.group(1))] = float(m.group(3))
F, T = [], []
for i in sorted(zeit):
    p = S + '/fr/n_%04d.png' % (i + 1)
    F.append(cv2.imread(p)[:, :, ::-1].astype(np.float32)); T.append(zeit[i])
F = np.stack(F)
N0 = 3380
d1 = np.abs(F - R[1]).mean((1, 2, 3)); d2 = np.abs(F - R[2]).mean((1, 2, 3))
nach = np.r_[np.nan, np.abs(np.diff(F, axis=0)).mean((1, 2, 3))]
aus = dict(bilder=[], dias={})
for i in range(len(F)):
    aus['bilder'].append(dict(bild=N0 + i, zeit=round(T[i], 4), abst_bild1=round(float(d1[i]), 3), abst_bild2=round(float(d2[i]), 3),
                              abst_vorbild=None if np.isnan(nach[i]) else round(float(nach[i]), 3)))
# Dia-Grenzen: Sprung zum Vorbild > 3 (sonst Kompressionsrauschen < 1)
grenz = [0] + [i for i in range(1, len(F)) if nach[i] > 3.0] + [len(F)]
for n in (1, 2):
    d = d1 if n == 1 else d2
    best = int(np.argmin(d))
    a = max(g for g in grenz if g <= best); b = min(g for g in grenz if g > best)
    sel = list(range(a, b))
    M = F[sel].mean(0)
    st = F[sel].std(0).mean()
    np.save(S + '/bild%d_mittel.npy' % n, M.astype(np.float32))   # float-Fassung nur im Scratchpad
    cv2.imwrite(E + '/daten/bild%d_mittel.png' % n, np.clip(M + .5, 0, 255).astype(np.uint8)[:, :, ::-1])
    aus['dias']['bild%d' % n] = dict(bester=N0 + best, bester_zeit=round(T[best], 4), bester_abstand=round(float(d[best]), 4),
                                     von=N0 + a, bis=N0 + b - 1, zeit_von=round(T[a], 4), zeit_bis=round(T[b - 1], 4), anzahl=len(sel),
                                     abstand_zur_vorlage=[round(float(d[i]), 3) for i in sel],
                                     sprung_innen_max=round(float(np.nanmax(nach[a + 1:b])) if b - a > 1 else 0.0, 3),
                                     pixel_std_im_dia=round(float(st), 3),
                                     mittel_gegen_vorlage=round(float(np.abs(M - (R[n])).mean()), 3))
json.dump(aus, open(E + '/daten/e1_bilder.json', 'w'), indent=1)
print(json.dumps(aus['dias'], indent=1))
print('Grenzen', [N0 + g for g in grenz])
