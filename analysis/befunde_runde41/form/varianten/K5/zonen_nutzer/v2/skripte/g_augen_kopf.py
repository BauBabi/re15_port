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
# ---------------------------------------------------------------- Augen (Runde 2: Auge = die TEXEL unter den Nutzerpunkten, eine Liste)
import g_texel as GT
from g_fleck import fleck
augen = []
auge_set = set(material_listen()['Auge'])
mA = maske_von_flaechen(K, auge_set)
auge_texel_heute = np.zeros(GT.TH * GT.TW, bool)
for f in auge_set:
    b_, vi_, i_ = map(int, f.split('|')); auge_texel_heute[GT.UVB[(b_, vi_ * 100000 + i_)]] = True
lbA, nA = ndi.label(cv2.dilate(mA.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0)
cl = []
for i in range(1, nA + 1):
    q = (lbA == i) & mA
    if q.sum():
        ys, xs = np.nonzero(q); cl.append(dict(px_grundlage=int(q.sum()), px_vorlage=round(float(q.sum() * S2[2]), 1), mitte_grundlage=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
                                              bbox_grundlage=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                                              flaechen=sorted({fl_schluessel(int(m), int(f)) for m, f in zip(K['mesh'][q], K['flaeche'][q])})))
cl.sort(key=lambda c: c['mitte_grundlage'][0])
MMx = json.load(open(ARBEIT + '/modellmengen_v2.json', encoding='utf-8'))
hoehle = set(MMx['Augenhoehle'])
T_alle = np.zeros(GT.TH * GT.TW, bool); punktmasken = {}
for st, name, kurz in zip(z['augen_vorn']['bau_info']['stellen'], ('Auge rechts (vom Wesen aus; links im Bild)', 'Auge links (vom Wesen aus; rechts im Bild)'), ('rechts', 'links')):
    inf = st['stelle']
    q, _ = fleck(2, tuple(st['linienende_grundlage']), 'auge')
    punktmasken[kurz] = q
    fl = flaechen_px(K, q)
    mx, my = inf['schwerpunkt_grundlage']
    best = min(cl, key=lambda c: np.hypot(c['mitte_grundlage'][0] - mx, c['mitte_grundlage'][1] - my)) if cl else None
    d = np.hypot(best['mitte_grundlage'][0] - mx, best['mitte_grundlage'][1] - my) if best else None
    vx, vy = v_aus_g(2, mx, my)
    te, T, tr = GT.texel_zone('augen_vorn', (22,), (), mg=q)
    T_alle |= T.reshape(-1)
    Ti = T.reshape(-1)
    augen.append(dict(name=name, linienende_beschriftet=st['linienende_beschriftet'], linienende_grundlage=st['linienende_grundlage'],
                      punkt_grundlage=[mx, my], punkt_vorlage=[round(float(vx), 1), round(float(vy), 1)], punkt_bbox_grundlage=inf['bbox_grundlage'],
                      abstand_punkt_zu_linienende_px_grundlage=inf['abstand_ende_px'], punkt_px_grundlage=int(q.sum()), punkt_px_vorlage=round(float(q.sum() * S2[2]), 1),
                      lab_punkt=inf['lab_median_fleck'], lab_umgebung=inf['lab_median_umgebung'],
                      auge_texel=dict(zahl=te['texel_zahl'], texel=te['texel'], traeger=te['traegerflaechen'],
                                      traeger_art={f: ('heute G4-Auge' if f in auge_set else 'Augenhöhle (Maulinneres-Texturstelle)' if f in hoehle else 'Haut')
                                                   for f in te['traegerflaechen']},
                                      davon_im_heutigen_augen_texturbereich=int((Ti & auge_texel_heute).sum())),
                      flaechen_unter_dem_punkt_px_grundlage={f: c for f, c in sorted(fl.items(), key=lambda t: -t[1])},
                      heutige_augenflaeche_naechste=dict(mitte_grundlage=best['mitte_grundlage'], px_vorlage=best['px_vorlage'],
                                                         abstand_mitte_px_grundlage=round(float(d), 1), abstand_mitte_px_vorlage=round(float(d * s), 1),
                                                         dx_dy_grundlage=[round(best['mitte_grundlage'][0] - mx, 1), round(best['mitte_grundlage'][1] - my, 1)],
                                                         ueberlappung_px_grundlage=int((q & mA).sum()), ueberlappung_px_vorlage=round(float((q & mA).sum() * S2[2]), 1)) if best else None))
erg['augen'] = dict(nutzerpunkte=augen,
                    heute_g4_augen=dict(flaechen=len(auge_set), sichtbar_bild2=len([f for f in auge_set if f in TOT[2]]),
                                        px_vorlage=round(float(mA.sum() * S2[2]), 1), gruppen=cl, texel_im_uv_bereich=int(auge_texel_heute.sum())),
                    regel_punkt='Zusammenhangskomponente L* >= Median(21 px) + 12 an jedem gruenen Linienende (Grundlage), gemessen statt Kreis',
                    vorschlag=dict(regel='Auge = NUR die Texel unter den beiden Nutzerpunkten (Texel-Maske, Mehrheitsregel). Die Traegerflaechen tragen das Auge, '
                                         'sie werden NICHT ganz zum Auge. Heutige G4-Augenflaechen ausserhalb der Punkte waeren nach dieser Lesung Haut (Frage an den Nutzer).',
                                   auge_texel_zahl=int(T_alle.sum()),
                                   je_auge={a['name']: dict(texel=a['auge_texel']['zahl'], punkt_px_vorlage=a['punkt_px_vorlage']) for a in augen}))
np.savez_compressed(ARBEIT + '/augen_punkte.npz', **punktmasken)
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
