"""Z2 Augen nach dem Nutzer + Auftrag 4 Kopfumriss (Video-Silhouette gegen Modell), Bild 2. Nur Messung, keine Geometrie."""
import json
import numpy as np, cv2
from scipy import ndimage as ndi
from g_basis import *
import g_bau
from g_zonen import REND, MM, maske_von_flaechen, flaechen_px, S2, TOT, ka

z = {e['id']: e for e in json.load(open(ARBEIT + '/zonen_v2_roh.json', encoding='utf-8'))}
MS = dict(np.load(ARBEIT + '/zonen_v2_masken.npz'))
K = REND[2]
s, t = s_t(2)
erg = {}
# ---------------------------------------------------------------- Augen
augen = []
auge_set = MM['Auge'] if 'Auge' in MM else set(material_listen()['Auge'])
mA = maske_von_flaechen(K, auge_set)
lbA, nA = ndi.label(cv2.dilate(mA.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0)
cl = []
for i in range(1, nA + 1):
    q = (lbA == i) & mA
    if q.sum():
        ys, xs = np.nonzero(q); cl.append(dict(px_grundlage=int(q.sum()), px_vorlage=round(float(q.sum() * S2[2])), mitte_grundlage=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
                                              bbox_grundlage=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                                              flaechen=sorted({fl_schluessel(int(m), int(f)) for m, f in zip(K['mesh'][q], K['flaeche'][q])})))
cl.sort(key=lambda c: c['mitte_grundlage'][0])
for st, name in zip(z['augen_vorn']['bau_info']['stellen'], ('Auge rechts (vom Wesen aus; links im Bild)', 'Auge links (vom Wesen aus; rechts im Bild)')):
    inf = st['stelle']; x0, y0, x1, y1 = inf['bbox_grundlage']
    from g_fleck import fleck
    q, _ = fleck(2, tuple(st['linienende_grundlage']), 'auge')
    fl = flaechen_px(K, q)
    mx, my = inf['schwerpunkt_grundlage']
    # naechster heutiger Augen-Cluster
    best = min(cl, key=lambda c: np.hypot(c['mitte_grundlage'][0] - mx, c['mitte_grundlage'][1] - my)) if cl else None
    d = np.hypot(best['mitte_grundlage'][0] - mx, best['mitte_grundlage'][1] - my) if best else None
    vx, vy = v_aus_g(2, mx, my)
    augen.append(dict(name=name, linienende_beschriftet=st['linienende_beschriftet'], linienende_grundlage=st['linienende_grundlage'],
                      punkt_grundlage=[mx, my], punkt_vorlage=[round(float(vx), 1), round(float(vy), 1)], punkt_bbox_grundlage=inf['bbox_grundlage'],
                      abstand_punkt_zu_linienende_px_grundlage=inf['abstand_ende_px'], punkt_px_grundlage=inf['px_grundlage'],
                      lab_punkt=inf['lab_median_fleck'], lab_umgebung=inf['lab_median_umgebung'],
                      flaechen_unter_dem_punkt={f: dict(px_grundlage=c, heute_auge=f in auge_set) for f, c in sorted(fl.items(), key=lambda t: -t[1])},
                      heutige_augenflaeche_naechste=dict(mitte_grundlage=best['mitte_grundlage'], px_vorlage=best['px_vorlage'],
                                                         abstand_mitte_px_grundlage=round(float(d), 1), abstand_mitte_px_vorlage=round(float(d * s), 1),
                                                         dx_dy_grundlage=[round(best['mitte_grundlage'][0] - mx, 1), round(best['mitte_grundlage'][1] - my, 1)],
                                                         ueberlappung_px_grundlage=int((q & mA).sum())) if best else None))
erg['augen'] = dict(nutzerpunkte=augen, heute_g4_augen=dict(flaechen=len(auge_set), sichtbar_bild2=len([f for f in auge_set if f in TOT[2]]),
                                                         px_vorlage=round(float(mA.sum() * S2[2])), gruppen=cl),
                    regel_punkt='Zusammenhangskomponente L* >= Median(21 px) + 12 an jedem gruenen Linienende (Grundlage), gemessen statt Kreis',
                    vorschlag='Flaechen unter den beiden Nutzerpunkten = Auge; die heutigen G4-Augenflaechen, die nicht darunter liegen, waeren Haut')
vor = set()
for a in augen:
    vor |= set(a['flaechen_unter_dem_punkt'])
erg['augen']['vorschlag_flaechen'] = sorted(vor)
erg['augen']['vorschlag_davon_heute_auge'] = sorted(vor & auge_set)
erg['augen']['vorschlag_neu_auge'] = sorted(vor - auge_set)
# ---------------------------------------------------------------- Kopfumriss
lab = grund_lab(2); L = lab[..., 0]; fig = figur(2)
modellkopf = np.isin(K['mesh'], [22, 23]) & K['maske']
nutzer = MS['gesicht_vorn']


def bb(m):
    ys, xs = np.nonzero(m)
    return [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None


bm, bn = bb(modellkopf), bb(nutzer)
vlab = cv2.cvtColor(vorlage(2).astype(np.float32) / 255, cv2.COLOR_RGB2LAB); VL = cv2.blur(vlab[..., 0], (5, 5))
# (a) Oberkante: L*-Profil des Videos (Vorlage, 5x5 gemittelt) in vier Spalten durch das Gesicht - gibt es eine Kante?
spalten = {}
for xv in (220, 250, 280, 310):
    prof = [round(float(VL[y, xv]), 1) for y in range(170, 400, 8)]
    spalten[str(xv)] = dict(y_von=170, schritt=8, L=prof)
# Oberkante im Video: erste Zeile ueber den Augen (von den Augen nach oben), in der L* (5x5) >= 15 wird; sonst None
ober = {}
for xv in (220, 250, 280, 310):
    yy = None
    for y in range(265, 150, -1):
        if VL[y, xv] >= 15:
            yy = y; break
    ober[str(xv)] = yy
# (b) linke Kante: Freistellung (Alpha) gegen Modellkopf, je Zeile im Bereich der Augen bis Maul
links = []
for yg in range(240, 500, 20):
    fx = np.nonzero(fig[yg])[0]; mx = np.nonzero(modellkopf[yg])[0]; nx = np.nonzero(nutzer[yg])[0]
    links.append(dict(y_vorlage=round(float(v_aus_g(2, 0, yg)[1]), 1), video_figur_links=round(float(v_aus_g(2, fx.min(), 0)[0]), 1) if len(fx) else None,
                      modellkopf_links=round(float(v_aus_g(2, mx.min(), 0)[0]), 1) if len(mx) else None,
                      nutzer_links=round(float(v_aus_g(2, nx.min(), 0)[0]), 1) if len(nx) else None))
# (c) Augenabstand und Augenhoehe
pv = [a['punkt_vorlage'] for a in augen]
mv = [[round(float(v_aus_g(2, *c['mitte_grundlage'])[0]), 1), round(float(v_aus_g(2, *c['mitte_grundlage'])[1]), 1)] for c in cl]


def vbb(b):
    x0, y0 = v_aus_g(2, b[0], b[1]); x1, y1 = v_aus_g(2, b[2], b[3]); return [round(float(x0), 1), round(float(y0), 1), round(float(x1), 1), round(float(y1), 1)]


erg['kopf'] = dict(
    modellkopf_bbox_vorlage=vbb(bm), nutzerumriss_bbox_vorlage=vbb(bn),
    modell_minus_nutzer_vorlage_px=dict(oben=round((bm[1] - bn[1]) * s, 1), unten=round((bm[3] - bn[3]) * s, 1), links=round((bm[0] - bn[0]) * s, 1),
                                        rechts=round((bm[2] - bn[2]) * s, 1)),
    video_oberkante=dict(regel='von der Augenhoehe (y 265) nach oben die erste Zeile mit L* >= 15 (Vorlage, 5x5 gemittelt)', je_spalte_x=ober,
                         profile=spalten, befund='keine Helligkeitskante: zwischen Brust und Gesicht bleibt L* im Video bei 3-12; die Oberkante ist im Video nicht messbar'),
    linke_kante=links,
    augen_video_vorlage=pv, augen_modell_heute_vorlage=mv,
    augenabstand_vorlage_px=dict(video=round(float(np.hypot(pv[1][0] - pv[0][0], pv[1][1] - pv[0][1])), 1),
                                 modell_heute=round(float(np.hypot(mv[1][0] - mv[0][0], mv[1][1] - mv[0][1])), 1)),
    augenhoehe_vorlage=dict(video=[p[1] for p in pv], modell_heute=[m[1] for m in mv]),
    iou_modell_nutzer=round(float((modellkopf & nutzer).sum() / max((modellkopf | nutzer).sum(), 1)), 3),
    modell_px_vorlage=round(float(modellkopf.sum() * S2[2])), nutzer_px_vorlage=round(float(nutzer.sum() * S2[2])),
    modell_ueber_nutzer_oberkante_px_vorlage=round(float((modellkopf & (np.arange(modellkopf.shape[0])[:, None] < bn[1])).sum() * S2[2])))
kopf_video = None
np.savez_compressed(ARBEIT + '/kopf_augen_masken.npz', modellkopf=modellkopf, augen_heute=mA)
json.dump(erg, open(ARBEIT + '/augen_kopf.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
print(json.dumps({k: (v if k != 'augen' else {kk: vv for kk, vv in v.items() if kk != 'heute_g4_augen'}) for k, v in erg.items()}, ensure_ascii=False, indent=1)[:6000])
print('heute', {k: v for k, v in erg['augen']['heute_g4_augen'].items() if k != 'gruppen'}, [(c['mitte_grundlage'], c['px_vorlage'], len(c['flaechen'])) for c in cl])
