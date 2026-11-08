#!/usr/bin/env python3
"""Riegel Feinschliff K5d (Nutzer 2026-11-08, Messblatt V4F). Exit 1 bei Rot.

    python riegel_k5d.py <Variante> [--json aus.json] [--cache DIR] [--gegenprobe]

<Variante> = Ordner mit FORM.EMD/FORM.TIM/form.json (3 Posen-Clips) und BAHNSTEIG_FORM.EMD/.TIM (28 Clips), z. B. feinschliff/K5d.
--gegenprobe: dieselben Zeilen am Asset K5c (stufe3/basis_k5c + shared_assets/RE2) - (b)-(g) muessen dort ROT sein.

Szenenkette wie Messblatt V4F (Tafeln 01/02): Kandidat-Szene Bild n (Disc-Licht x k, pruefung/licht.json) -> form_video.video_filter
(entzerren=False, rauschen=True, Saaten 15/12345/7) -> p_zonen.rueck ("T"). Vorlage: dieselbe Rueckkette (Messblatt V4F, ketten.original).
Pixelmengen fest aus dem Asset K5c (Kennungs-Render v4f.szene_id) und den Vorlagen-Fleckmasken (feinschliff/messung_v4f_flecken.npz),
also fuer Asset und Variante dieselben Pixel (Messblatt V4F 3.1).

Zeilen (bindend; Quelle der Grenze in Klammern):
 (a) Details: je Detailfleck (Messblatt 1.3 D1-D3, D6) Ketten-T-L* >= 80 % der Vorlage (Messblatt 5 Offen 6, "m1-Kreise gruen");
     Anteil T-L* > 40 an der Figur Bild 1 >= 9,9 %, Bild 2 >= 14,5 % (Messblatt 0.1 / 5 Offen 6).
 (b) Schwanzspitze (Mesh 6 im Nutzer-Schwanz, Bild 1): Rot-Anteil (a* >= 12, C* >= 15, L* >= 20) 25..50 % (Vorlage 37,5 %), mittleres T-L*
     >= 80 % der Vorlage (44,3), Fleck F15 T-L* >= 80 % (53,3) (Messblatt 1.1).
 (c) Gelenke weisslich: Gelenkflecken Bild 1 F04/F05/F07/F08/F10 und Bild 2 F05/F06: T-L* >= 80 % der Vorlage und T-C* <= 15
     (Vorlage a* -3..+7, b* -1..-15: weiss-grau bis kuehl, Messblatt 2.3); Albedo der Bein-/Arm-Gelenkkoerper L* >= 25, C* <= 12.
 (d) Lippe: Albedo-Rotklasse (a* > 15, C* > 15, Messblatt 2.4) auf den Lippensaum-Flaechen ohne das Maul-Band am Unterkiefer = 0 % der Texel.
 (e) Buckel vorn (Bild 2, F01+F02): T-L* >= 80 % der Vorlage, T-a* >= 70 % der Vorlage, Farbton a*-b* innerhalb 20 Grad (Messblatt 2.5).
 (f) Helligkeit: mittleres L* der Figur >= Vorlage, Bild 1 und 2, je T / Video (3 Saaten) / ohne Filter (gegen video_umkehr der Vorlage)
     (Messblatt 3.1/3.2).
 (g) Krallen vorn (Bild 2): sichtbare Krallen (>= 30 px) rechts = 1 (Kralle 31), links = 2 (14, 15) (Messblatt 2.1 Umriss-Spitzen);
     Albedo der Krallenzeile 1 mit a* > 10: 0 % der Texel; Kette T Krallen gegen Hand |dL*| <= 8 (Asset +19/+15, Vorlage 0,4/3,5).
 G0  Geometrie/Knochen/Clips: Ecken und Normalen aller Meshes gleich K5c ausser den Krallen-Meshes 19/20/28/29 (Liste der Ecken);
     Flaechen-Eckindizes gleich; Asset EDD/EMR/Morph byte-gleich, 32 Meshes / 32 Knochen / 28 Clips.
 E-Z1/E-Z2 CLUT-Zeilen <= 16, Port-Texturhoehe <= 4096, jede Flaeche liest eine gueltige Zeile (riegel_einbau.py).
 L   Leuchtstellen unveraendert: F1 (Bild 1, Fleck F01, Zeile 5) und Maul-Band am Unterkiefer (Bild 2, Zeile 6): Albedo je Pixel gleich K5c
     (>= 99 % der Pixel), Zaehne (Zeile 7) Farben gleich.
 RV6 Adern nie heller und nie roter als die umgebende Haut (riegel_farbe_v3.rv6, Texelraum), ohne die neuen Stellen (Inselflaechen).
Info: RV1-RV5 (riegel_rot.py), Anteil T-L* > 40 obere Lage gegen K4, Spielfenster nicht (Tafel f4).
"""
import argparse
import collections
import json
import os
import struct
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
FORM = os.path.normpath(os.path.join(K5, '..', '..'))
WT = os.path.normpath(os.path.join(FORM, '..', '..', '..'))
sys.path.insert(0, os.path.join(WT, 're15_port', 'tools'))
sys.path.insert(0, os.path.join(K5, 'farbe_lesung', 'V4F_skripte'))
sys.path.insert(0, HIER)
import form_emd as FE  # noqa: E402
import v4f  # noqa: E402
import gelenke as G  # noqa: E402

PZ = v4f.PZ
BASIS = os.path.join(FORM, 'stufe3', 'basis_k5c')
ASSET = os.path.join(K5, 'einbau_k5c', 'BAHNSTEIG_FORM')   # Asset K5c (E30), seit E32 gesichert (einbau_k5c/K5C.md); shared_assets traegt K5e_f1
FLECKEN = np.load(os.path.join(K5, 'feinschliff', 'messung_v4f_flecken.npz'))
LIPPE_PKL = os.path.join(K5, 'feinschliff', 'K5d', 'lippe_k5c.pkl')
LOGO = np.zeros((480, 640), bool)
LOGO[428:, :262] = True
NT = FORM + '/nutzer_teile/maske_%s.png'
KRALLEN_EIN = (19, 20, 28, 29)
DETAIL = {1: ['B1-F03', 'B1-F04', 'B1-F05', 'B1-F07', 'B1-F08', 'B1-F10', 'B1-F12', 'B1-F13', 'B1-F06', 'B1-F11'], 2: ['B2-F05', 'B2-F06']}
GELENK_FL = {1: ['B1-F04', 'B1-F05', 'B1-F07', 'B1-F08', 'B1-F10'], 2: ['B2-F05', 'B2-F06']}
GELENKE_BA = ["Huefte l", "Huefte r", "Muffe Huefte l", "Muffe Huefte r", "Knie l", "Knie r", "Fuss l", "Fuss r", "Schulter l (Arm l - Brust)",
              "Schulter r (Arm r - Brust)", "Muffe Schulter r", "Ellbogen l", "Ellbogen r", "Hand l", "Hand r", "Muffe Hand l", "Muffe Hand r"]


def r1(x):
    return [round(float(v), 1) for v in np.atleast_1d(x)]


def labm(rgb):
    return v4f.lab(np.clip(np.asarray(rgb, float), 0, 255))


def nutzer_union(n):
    um = np.zeros((480, 640), bool)
    for _a, m in PZ.regionen(n):
        um |= m
    return um


class Stand:
    """Szene Bild 1/2 eines Stands: sauber, Kennung, Video (3 Saaten), T (3 Saaten), Lab-Mittel."""
    def __init__(self, ordner, cache=None):
        self.ordner = ordner
        self.d = {}
        for n in (1, 2):
            s, K = v4f.szene_id(ordner, n)
            vid = {sa: v4f.video(s, n, sa) for sa in v4f.SAATEN}
            T = [PZ.rueck(vid[sa], n)[0] for sa in v4f.SAATEN]
            self.d[n] = dict(sauber=s, K=K, video=vid, T=T,
                             LT=np.mean([labm(t) for t in T], 0), LV=np.mean([labm(vid[sa]) for sa in v4f.SAATEN], 0), LS=labm(s))


class Vorlage:
    def __init__(self):
        self.d = {}
        for n in (1, 2):
            o = v4f.original(n)
            T, clip = PZ.rueck(o, n)
            e = v4f.FV.video_umkehr(o, bild=n)
            self.d[n] = dict(LT=labm(T), LV=labm(o), LS=labm(e), clip=clip)


def m_lab(L, m):
    return L[m].mean(0) if m.any() else np.full(3, np.nan)


def rot_anteil(L, m, a0=12.0, c0=15.0, l0=20.0):
    x = L[m]
    r = (x[:, 1] >= a0) & (np.hypot(x[:, 1], x[:, 2]) >= c0) & (x[:, 0] >= l0)
    return 100.0 * r.mean() if len(x) else 0.0


def flaechen_alb(ordner):
    """je Flaeche (key) die Albedo-Lab aller Texel ihres UV-Polygons (fillPoly + polylines) und deren Zahl."""
    emd = open(os.path.join(ordner, 'FORM.EMD'), 'rb').read()
    tim = FE.tim_lesen(open(os.path.join(ordner, 'FORM.TIM'), 'rb').read())
    form = FE.lese_emd(emd)
    px = tim['pixel']
    cl = np.array(tim['cluts'], np.int64)
    rgb = np.stack([cl & 31, (cl >> 5) & 31, (cl >> 10) & 31], -1).astype(float) * 255.0 / 31.0
    labz = labm(rgb.reshape(-1, 3)).reshape(rgb.shape)
    out = {}
    for mi, m in enumerate(form['meshes']):
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            for fi, f in enumerate(m[art]):
                po = (int(f['page']) & 0xF) * 128
                uv = f['uv']
                pts = [uv[0], uv[1], uv[3], uv[2]] if len(uv) == 4 else uv
                p = np.array([[a + po, b] for a, b in pts], np.int32)
                mk = np.zeros(px.shape, np.uint8)
                cv2.fillPoly(mk, [p], 1)
                cv2.polylines(mk, [p], True, 1)
                z = ((int(f['clut']) >> 6) & 0x1FF) - tim['clut_y']
                out[(mi, vi, fi)] = (z, labz[z][px[mk.astype(bool)]])
    return out, form, tim


def messen(ordner, asset_pfad, vor, ref=None):
    """Alle Riegelzeilen. ref = Stand K5c (fuer feste Pixelmengen); wenn None, ist ordner selbst K5c."""
    st = Stand(ordner)
    ka = ref if ref is not None else st
    zeilen = []

    def zeile(nm, wert, ok, info=False):
        zeilen.append(dict(name=nm, wert=wert, ok=None if info else bool(ok)))

    # Figur-Pixelmenge (Messblatt 3.1): Nutzer-Teilmasken & Asset-K5c-Modell & ohne Weisskappe
    fig = {n: nutzer_union(n) & ka.d[n]['K']['maske'] & ~vor.d[n]['clip'] for n in (1, 2)}
    # ---------------- (f) Helligkeit
    for n in (1, 2):
        for k, nm in (('LT', 'T'), ('LV', 'Video'), ('LS', 'ohne Filter')):
            v_ = float(st.d[n][k][fig[n]][:, 0].mean())
            o_ = float(vor.d[n][k][fig[n]][:, 0].mean())
            zeile('(f) Bild %d Figur L* %s >= Vorlage' % (n, nm), '%.2f (Vorlage %.2f)' % (v_, o_), v_ >= o_)
    # ---------------- (a) Details
    for n in (1, 2):
        for fid in DETAIL[n]:
            m = FLECKEN[fid] & ~vor.d[n]['clip']
            v_ = float(st.d[n]['LT'][m][:, 0].mean())
            o_ = float(vor.d[n]['LT'][m][:, 0].mean())
            zeile('(a) %s Ketten-T-L* 80..125 %% der Vorlage' % fid, '%.1f (Vorlage %.1f, Fenster %.1f..%.1f)' % (v_, o_, 0.8 * o_, 1.25 * o_),
                  0.8 * o_ <= v_ <= 1.25 * o_)
    # Anteil T-L* > 40: Bild 1 auf der Figur (Grenze 9,9 %, Messblatt 0.1). Bild 2: ohne die Feuerschein-Flecken F13/F14 (Messblatt 1.2:
    # "Feuerschein" auf Hand l bzw. Kiefer + Hand r, 7941 px) - das ist Licht der Szene, kein Texturdetail; Grenze = Vorlage auf derselben Menge.
    feuer = FLECKEN['B2-F13'] | FLECKEN['B2-F14']
    for n, menge, k4 in ((1, fig[1], 15.3), (2, fig[2] & ~feuer, 16.9)):
        a_ = 100.0 * float((st.d[n]['LT'][menge][:, 0] > 40).mean())
        o_ = 100.0 * float((vor.d[n]['LT'][menge][:, 0] > 40).mean())
        zeile('(a) Bild %d Anteil T-L* > 40 an der Figur%s >= Vorlage' % (n, '' if n == 1 else ' ohne Feuerschein F13/F14'),
              '%.1f %% (Vorlage %.1f, %d px)' % (a_, o_, menge.sum()), a_ >= o_)
        f_ = 100.0 * float((st.d[n]['LT'][fig[n]][:, 0] > 40).mean())
        zeile('(a) Bild %d Anteil T-L* > 40 ganze Figur (Messblatt: Vorlage %s, K4 %.1f %%)' % (n, '9,9' if n == 1 else '14,5', k4), '%.1f %%' % f_,
              True, info=True)
    # ---------------- (b) Schwanzspitze
    K1 = ka.d[1]['K']
    schwanz = cv2.imread(NT % 'tail', 0) > 0
    m6 = (K1['mesh'] == 6) & schwanz & ~vor.d[1]['clip']
    ra = rot_anteil(st.d[1]['LT'], m6)
    ro = rot_anteil(vor.d[1]['LT'], m6)
    zeile('(b) Schwanzspitze Rot-Anteil 25..50 %', '%.1f %% (Vorlage %.1f, px %d)' % (ra, ro, m6.sum()), 25.0 <= ra <= 50.0)
    lv, lo = m_lab(st.d[1]['LT'], m6), m_lab(vor.d[1]['LT'], m6)
    zeile('(b) Schwanzspitze T-L* >= 80 % Vorlage', '%s (Vorlage %s)' % (r1(lv), r1(lo)), lv[0] >= 0.8 * lo[0])
    m15 = FLECKEN['B1-F15'] & ~vor.d[1]['clip']
    lv, lo = m_lab(st.d[1]['LT'], m15), m_lab(vor.d[1]['LT'], m15)
    zeile('(b) Fleck B1-F15 T-L* >= 80 % Vorlage', '%s (Vorlage %s)' % (r1(lv), r1(lo)), lv[0] >= 0.8 * lo[0])
    # ---------------- (c) Gelenke weisslich
    for n in (1, 2):
        for fid in GELENK_FL[n]:
            m = FLECKEN[fid] & ~vor.d[n]['clip']
            lv, lo = m_lab(st.d[n]['LT'], m), m_lab(vor.d[n]['LT'], m)
            c_ = float(np.hypot(lv[1], lv[2]))
            zeile('(c) %s T-L* 80..125 %% der Vorlage, T-C* <= 15' % fid, '%s C* %.1f (Vorlage %s)' % (r1(lv), c_, r1(lo)),
                  0.8 * lo[0] <= lv[0] <= 1.25 * lo[0] and c_ <= 15)
    alb, form_v, tim_v = flaechen_alb(ordner)
    gel, _n = G.gelenk_flaechen(ordner)
    gl = np.vstack([alb[k][1] for k, nm in gel.items() if nm in GELENKE_BA])
    gm = gl.mean(0)
    glied = {8, 9, 24, 25, 26, 27, 11, 12, 13, 16, 17, 18}
    ll = np.vstack([x for k, (z, x) in alb.items() if k[0] in glied and k not in gel])
    lm = ll.mean(0)
    zeile('(c) Albedo Bein-/Arm-Gelenkkoerper kuehl-grau und heller als die Glieder (L* >= Glied + 5, C* <= 12)',
          'Gelenke %s C* %.1f (%d Texel), Glieder %s' % (r1(gm), np.hypot(gm[1], gm[2]), len(gl), r1(lm)),
          gm[0] >= lm[0] + 5 and np.hypot(gm[1], gm[2]) <= 12)
    # ---------------- (d) Lippe
    import pickle
    lp = pickle.load(open(LIPPE_PKL, 'rb'))
    K2r = ka.d[2]['K']
    sicht = collections.Counter()
    for y, x in zip(*np.nonzero(K2r['maske'])):
        sicht[(int(K2r['mesh'][y, x]), int(K2r['flaeche'][y, x] >= 100000), int(K2r['flaeche'][y, x] % 100000))] += 1
    lip = [tuple(lp['keys'][i]) for i in lp['lc']]
    alb_ref, _f, _t = flaechen_alb(BASIS)
    band_bleibt = {k for k in lip if alb_ref[k][0] == 6 and k[0] == 23 and sicht[k] >= 20}
    band_bleibt |= {k for k, (z, _x) in alb_ref.items() if z == 6 and k[0] == 23 and sicht[k] >= 20}
    lt = np.vstack([alb[k][1] for k in lip if k not in band_bleibt])
    rt = (lt[:, 1] > 15) & (np.hypot(lt[:, 1], lt[:, 2]) > 15)
    nrot = sum(1 for k in lip if k not in band_bleibt and ((alb[k][1][:, 1] > 15) & (np.hypot(alb[k][1][:, 1], alb[k][1][:, 2]) > 15)).any())
    zeile('(d) Lippensaum ohne Unterkiefer-Band: Albedo-Rotklasse 0 %', '%.2f %% der Texel, %d von %d Flaechen mit Rot (Band bleibt: %d Flaechen)'
          % (100.0 * rt.mean(), nrot, len([k for k in lip if k not in band_bleibt]), len(band_bleibt)), rt.sum() == 0)
    # ---------------- (e) Buckel
    for fid in ('B2-F01', 'B2-F02'):
        m = FLECKEN[fid] & ~vor.d[2]['clip']
        lv, lo = m_lab(st.d[2]['LT'], m), m_lab(vor.d[2]['LT'], m)
        hv, ho = np.degrees(np.arctan2(lv[2], lv[1])), np.degrees(np.arctan2(lo[2], lo[1]))
        zeile('(e) Buckel %s T-L* >= 80 %%, a* >= 70 %%, Farbton +-20 Grad' % fid,
              '%s h %.0f (Vorlage %s h %.0f)' % (r1(lv), hv, r1(lo), ho),
              lv[0] >= 0.8 * lo[0] and lv[1] >= 0.7 * lo[1] and abs(hv - ho) <= 20)
    # ---------------- (g) Krallen
    K2 = st.d[2]['K']
    sichtbar = {kb: int((K2['mesh'] == kb).sum()) for kb in (19, 20, 29, 31, 14, 15, 28, 30)}
    re_ = [kb for kb in (19, 20, 29, 31) if sichtbar[kb] >= 30]
    li_ = [kb for kb in (14, 15, 28, 30) if sichtbar[kb] >= 30]
    zeile('(g) Krallen sichtbar (>= 30 px) rechts (W) = [31]', '%s (px %s)' % (re_, {k: sichtbar[k] for k in (19, 20, 29, 31)}), re_ == [31])
    zeile('(g) Krallen sichtbar (>= 30 px) links (W) = [14, 15]', '%s (px %s)' % (li_, {k: sichtbar[k] for k in (14, 15, 28, 30)}), li_ == [14, 15])
    kt = np.vstack([alb[k][1] for k in alb if k[0] in (14, 15, 28, 30, 19, 20, 29, 31)])
    zeile('(g) Krallen-Albedo a* > 10: 0 %', '%.2f %% von %d Texelzugriffen, Mittel %s' % (100.0 * (kt[:, 1] > 10).mean(), len(kt), r1(kt.mean(0))),
          (kt[:, 1] > 10).sum() == 0)
    # Krallen gegen Hand: im Disc-Licht-Bild ohne Videofilter (Albedo x Licht, keine Unschaerfe); die Kette T zieht den hellen Feuerboden in die
    # duennen Krallen (Kralle 31: 419 px) und steht als Info daneben. Vorlage (Messblatt 2.1): Krallen = Hand, dE 4 / 3,8 unter T.
    for hand, hm, kl in (('r (W)', 18, (31,)), ('l (W)', 13, (14, 15))):
        um = cv2.imread(NT % ('right_arm_front' if hm == 18 else 'left_arm_front'), 0) > 0
        mk = np.isin(K2['mesh'], kl) & um & ~LOGO & ~vor.d[2]['clip']
        mh = (K2['mesh'] == hm) & um & ~LOGO & ~vor.d[2]['clip']
        lk, lh = m_lab(st.d[2]['LS'], mk), m_lab(st.d[2]['LS'], mh)
        zeile('(g) Disc-Licht ohne Filter: Krallen gegen Hand %s |dL*| <= 5, dE <= 8' % hand, 'Krallen %s Hand %s dL %.1f dE %.1f'
              % (r1(lk), r1(lh), lk[0] - lh[0], np.linalg.norm(lk - lh)), abs(lk[0] - lh[0]) <= 5 and np.linalg.norm(lk - lh) <= 8)
        tk, th = m_lab(st.d[2]['LT'], mk), m_lab(st.d[2]['LT'], mh)
        ok_ = m_lab(vor.d[2]['LT'], mk)
        zeile('(g) Kette T Krallen %s (Info)' % hand, 'Krallen %s Hand %s dL %.1f; Vorlage Krallen %s' % (r1(tk), r1(th), tk[0] - th[0], r1(ok_)), True,
              info=True)
    # ---------------- G0 Geometrie
    fb = FE.lese_emd(open(os.path.join(BASIS, 'FORM.EMD'), 'rb').read())
    geaendert = {}
    idx_gleich = True
    uv_neu = 0
    for mi, (a, b) in enumerate(zip(form_v['meshes'], fb['meshes'])):
        if a['normalen'] != b['normalen']:
            geaendert.setdefault(mi, []).append('normalen')
        d = [i for i, (p, q) in enumerate(zip(a['ecken'], b['ecken'])) if p != q]
        if d or len(a['ecken']) != len(b['ecken']):
            geaendert[mi] = d
        for art in ('dreiecke', 'vierecke'):
            if len(a[art]) != len(b[art]):
                idx_gleich = False
            for x, y in zip(a[art], b[art]):
                if x['e'] != y['e'] or x['n'] != y['n']:
                    idx_gleich = False
                if x['uv'] != y['uv'] or x['page'] != y['page'] or x['clut'] != y['clut']:
                    uv_neu += 1
    ok_g = set(geaendert) <= set(KRALLEN_EIN) and idx_gleich and all(isinstance(v, list) for v in geaendert.values())
    zeile('G0 Ecken/Normalen gleich K5c ausser Krallen 19/20/28/29, Eckindizes gleich',
          'geaendert: %s; Eckindizes gleich %s; Flaechen mit neuer UV/Zeile/Seite %d' % ({k: len(v) for k, v in geaendert.items()}, idx_gleich, uv_neu), ok_g)
    if asset_pfad:
        a_v = open(asset_pfad + '.EMD', 'rb').read()
        a_r = open(ASSET + '.EMD', 'rb').read()
        sv, sr = FE.sektionen(a_v), FE.sektionen(a_r)
        gleich = [k for k in sv if k != 'md1' and sv[k] == sr[k]]
        fa = FE.lese_emd(a_v)
        n_cl = len(fa['clips'])
        md1_ok = FE.md1_lesen(sv['md1']) == form_v['meshes']
        zeile('G0 Asset: EDD/EMR/Morph byte-gleich, 32/32/28, MD1 = Variante', 'gleich %s; Meshes %d Knochen %d Clips %d; MD1 = FORM.EMD %s'
              % (gleich, len(fa['meshes']), len(fa['knochen']), n_cl, md1_ok),
              len(gleich) == 7 and len(fa['meshes']) == 32 and len(fa['knochen']) == 32 and n_cl == 28 and md1_ok)
        t_ok = open(asset_pfad + '.TIM', 'rb').read() == open(os.path.join(ordner, 'FORM.TIM'), 'rb').read()
        zeile('G0 Asset-TIM = FORM.TIM der Variante', str(t_ok), t_ok)
    # ---------------- E-Z1 / E-Z2
    timb = open(os.path.join(ordner, 'FORM.TIM'), 'rb').read()
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', timb, 8)
    _ds, bx, by, bw, bh = struct.unpack_from('<I4H', timb, 8 + cs)
    n_cl = cw * ch // 256
    zeile('E-Z1 CLUT-Zeilen <= 16, Port-Texturhoehe <= 4096', '%d Zeilen, %d x %d = %d' % (n_cl, bh, n_cl, bh * n_cl), n_cl <= 16 and bh * n_cl <= 4096)
    zz = [z for z, _x in alb.values()]
    zeile('E-Z2 jede Flaeche liest eine gueltige Zeile', '%d/%d' % (sum(0 <= z < n_cl for z in zz), len(zz)), all(0 <= z < n_cl for z in zz))
    # ---------------- L Leuchtstellen
    kv = v4f.kand(ordner).tex.lut
    kr = v4f.kand(BASIS).tex.lut
    m = FLECKEN['B1-F01'] & ka.d[1]['K']['maske'] & st.d[1]['K']['maske']
    Kv, Kr = st.d[1]['K'], ka.d[1]['K']
    av = kv[Kv['zeile'][m].clip(0), Kv['eintrag'][m].clip(0)][:, :3]
    ar = kr[Kr['zeile'][m].clip(0), Kr['eintrag'][m].clip(0)][:, :3]
    p1 = 100.0 * float((np.abs(av.astype(int) - ar.astype(int)).max(1) == 0).mean())
    lv, lr_ = m_lab(st.d[1]['LT'], m), m_lab(ka.d[1]['LT'], m)
    zeile('L F1 (Bild 1, F01) Albedo je Pixel gleich K5c >= 99 %', '%.1f %% gleich; Kette T %s gegen K5c %s' % (p1, r1(lv), r1(lr_)), p1 >= 99.0)
    Kv, Kr = st.d[2]['K'], ka.d[2]['K']
    bkeys = np.zeros((480, 640), bool)
    for y, x in zip(*np.nonzero((Kr['zeile'] == 6) & (Kr['mesh'] == 23))):
        k_ = (23, int(Kr['flaeche'][y, x] >= 100000), int(Kr['flaeche'][y, x] % 100000))
        bkeys[y, x] = k_ in band_bleibt
    m = bkeys & Kv['maske']
    av = kv[Kv['zeile'][m].clip(0), Kv['eintrag'][m].clip(0)][:, :3]
    ar = kr[Kr['zeile'][m].clip(0), Kr['eintrag'][m].clip(0)][:, :3]
    p2 = 100.0 * float((np.abs(av.astype(int) - ar.astype(int)).max(1) == 0).mean())
    lv, lr_ = m_lab(st.d[2]['LT'], m), m_lab(ka.d[2]['LT'], m)
    zeile('L Maul-Band Unterkiefer (Bild 2, Zeile 6, Mesh 23, >= 20 px) Albedo je Pixel gleich K5c >= 99 %', '%.1f %% gleich (%d px); Kette T %s gegen K5c %s'
          % (p2, m.sum(), r1(lv), r1(lr_)), p2 >= 99.0)
    tv = FE.tim_lesen(timb)['cluts']
    tr = FE.tim_lesen(open(os.path.join(BASIS, 'FORM.TIM'), 'rb').read())['cluts']
    zeile('L Zeilen 5/6/7 (F1, Band, Zaehne): alle von K5c benutzten Eintraege gleich', str(all(tv[z][e] == tr[z][e] for z in (5, 6, 7)
          for e in benutzte_eintraege(BASIS, z))), all(tv[z][e] == tr[z][e] for z in (5, 6, 7) for e in benutzte_eintraege(BASIS, z)))
    return zeilen, st


_BEN = {}


def benutzte_eintraege(ordner, z):
    if ordner not in _BEN:
        tim = FE.tim_lesen(open(os.path.join(ordner, 'FORM.TIM'), 'rb').read())
        form = FE.lese_emd(open(os.path.join(ordner, 'FORM.EMD'), 'rb').read())
        px = tim['pixel']
        b = collections.defaultdict(set)
        for m in form['meshes']:
            for art in ('dreiecke', 'vierecke'):
                for f in m[art]:
                    po = (int(f['page']) & 0xF) * 128
                    uv = f['uv']
                    pts = [uv[0], uv[1], uv[3], uv[2]] if len(uv) == 4 else uv
                    mk = np.zeros(px.shape, np.uint8)
                    p = np.array([[a + po, bb] for a, bb in pts], np.int32)
                    cv2.fillPoly(mk, [p], 1)
                    cv2.polylines(mk, [p], True, 1)
                    b[((int(f['clut']) >> 6) & 0x1FF) - tim['clut_y']] |= set(np.unique(px[mk.astype(bool)]).tolist())
        _BEN[ordner] = b
    return sorted(_BEN[ordner][z])


def rv6_zeile(ordner):
    """RV6 (riegel_farbe_v3.rv6, Texelraum) fuer den 16-Zeilen-Stand. Flaechen, UV und Indizes von K5c (stufe3/basis_k5c), Farben aus der
    Palette der Variante: so misst RV6 die Adern/Haut-Paare des Assets mit den umgefaerbten (aufgehellten) Eintraegen; die neuen Stellen
    (Inseln mit neuen Eintraegen) sind keine Adern und liegen ausserhalb (Auftrag: RV6 gruen ausser an den neuen Fleischstellen).
    Zeilen auf die 17-Zeilen-Zaehlung von V3 R5 zurueckgelegt (FARBE.md E.1: Zeile 2 entfiel, z -> z + 1 fuer z >= 2), bau.json
    (Basis-Zuordnung, Flaechenherkunft) von farbe/K5c_farbe_v3."""
    import riegel_farbe_v3 as RF3
    KG = RF3.KG
    v = RF3.Var(BASIS)
    cl_v = FE.tim_lesen(open(os.path.join(ordner, 'FORM.TIM'), 'rb').read())['cluts']
    v.vdir = os.path.join(K5, 'farbe', 'K5c_farbe_v3')
    v.flaechen = [(key, mi, z if z < 2 else z + 1, ecken, a3) for key, mi, z, ecken, a3 in v.flaechen]
    leer = [0x8000] * 256
    v.cl = [list(z) for z in cl_v[:2]] + [leer] + [list(z) for z in cl_v[2:]]
    v.rgb_zeilen = np.stack([KG.wort_rgb(z) for z in v.cl])
    _g4, g4tim, _q = KG.g4_modell()
    _px, cl_g4 = KG.g4_cluts(g4tim)
    return RF3.rv6(v, cl_g4)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('variante', nargs='?')
    ap.add_argument('--json')
    ap.add_argument('--gegenprobe', action='store_true')
    ap.add_argument('--ohne-rv6', action='store_true')
    a = ap.parse_args()
    vor = Vorlage()
    ref = Stand(BASIS)
    if a.gegenprobe:
        ordner, asset = BASIS, None
        titel = 'Gegenprobe Asset K5c (stufe3/basis_k5c)'
    else:
        ordner, asset = a.variante, os.path.join(a.variante, 'BAHNSTEIG_FORM')
        titel = a.variante
    zeilen, _st = messen(ordner, asset, vor, ref)
    if not a.ohne_rv6:
        r6, ok6 = rv6_zeile(ordner)
        zeilen.append(dict(name='RV6 Adern nie heller/roter als die Haut (Flaechen/Indizes K5c, Palette der Variante)', wert=str(r6), ok=bool(ok6)))
    print('== Riegel Feinschliff K5d:', titel)
    g = n = 0
    for z in zeilen:
        st = 'Info' if z['ok'] is None else ('GRUEN' if z['ok'] else 'ROT')
        if z['ok'] is not None:
            n += 1
            g += z['ok']
        print('  %-78s %-6s %s' % (z['name'], st, z['wert']))
    print('  -> %d/%d bindende Zeilen GRUEN' % (g, n))
    if a.json:
        json.dump(dict(titel=titel, zeilen=zeilen, gruen=g, bindend=n), open(a.json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    if a.gegenprobe:
        # Gegenprobe am Asset K5c: je Nutzerpunkt (b)-(g) muss mindestens eine bindende Zeile ROT sein, sonst misst der Riegel nichts.
        fehlt = [b for b in 'bcdefg' if not any(z['name'].startswith('(%s)' % b) and z['ok'] is False for z in zeilen)]
        print('  Gegenprobe: Punkte ohne ROT-Zeile: %s' % (fehlt or 'keine'))
        print('EXIT %d' % (1 if fehlt else 0))
        sys.exit(1 if fehlt else 0)
    print('EXIT %d' % (0 if g == n else 1))
    sys.exit(0 if g == n else 1)


if __name__ == '__main__':
    main()
