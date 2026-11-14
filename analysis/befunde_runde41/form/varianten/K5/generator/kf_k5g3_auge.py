#!/usr/bin/env python3
"""K5g3 Runde 2 - Augen als eigene Texel an den Nutzerpunkten (E3 "Augen = nur die hellen Nutzerpunkte (Zone 29, texelgenau, mit eigenen
Texeln)", O4 "im Spiellicht sichtbar heller als die Haut daneben"; Befund Sicht-Pruefer K5g3 r1 (5): Augen verschwunden).

Warum nicht mehr ueber die Zone-29-Texel (gemessen, GEOMETRIE.md 9.4 und t3r2/w/auge1.py): die Traegerflaechen 22|0|82 und 22|0|83 des
rechten Auges sind schon in K5e_f1 UV-entartet (alle Ecken v = 72) - sie koennen nur einen gestreckten Strich zeigen; 22|1|152 liest einen
einzigen Augen-Texel. In Bild 2 traf das Augen-Material darum 5 Pixel (Nutzer: 6,5 + 12,3 Pixel).

Weg (nur UV/Texel, keine Geometrie): Die Lage der Augen sind die Nutzerpunkte in Bild 2 (zonen_v2.json augen.nutzerpunkte.punkt_vorlage), die
Groesse die gemessene Punktflaeche (punkt_px_vorlage). `mitten` rechnet sie EINMAL auf die Oberflaeche des Kopfes (Teil 22 der fertigen Geometrie
kf_k5g3_geo, Haltung Bild 2) zurueck und schreibt die Tabelle generator/k5g3_augen_mitten.json (Mitte im Knochenraum von Teil 22, Radius so, dass
die Augenscheibe in Bild 2 genau die Flaeche des Nutzerpunkts bedeckt). Der Generator liest nur die Tabelle (offline gerechnet, als Tabelle
ausgeliefert - Memory reai-v2-psx-laufzeitbudget).

`dekal` (Generator, nach dem Spalt): jedes Dreieck von Teil 22, das die Augenscheibe beruehrt, bekommt EIGENE Texel: sein Dreieck wird
laengentreu in freien Texturplatz gelegt (eigene, flaechige UV, Massstab par textur.auge.texel_je_radius Texel je Augenradius), jeder Texel
liest dort entweder (innerhalb der Augenscheibe, 3D-Abstand zur Mitte) das Augen-Material oder (ausserhalb) genau die Farbe, die die Flaeche
vorher an dieser Stelle zeigte (Rueckabtastung ueber die alte UV, Original-Texel, gleiche Farbe -> keine Naht). Im Auge bleibt die
Helligkeitsstruktur der Original-Texel: Rang des alten Texels -> Eintrag der Augen-Rampe am gleichen texelgewichteten Rang (V3). Eine eigene
CLUT-Zeile 'auge' traegt die Worte.

    python kf_k5g3_auge.py mitten     -> generator/k5g3_augen_mitten.json (aus K5/k5g/K5g3/geo, Bild 2)
"""
import hashlib
import json
import math
import os
import shutil
import sys

import cv2
import numpy as np

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5e as E  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402

FE = E.FE
K5 = E.K5
TABELLE = os.path.join(HIER, 'k5g3_augen_mitten.json')
TEIL = 22


def _sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()


# ------------------------------------------------------------------ offline: Mitten aus Bild 2
def mitten(geo_ordner=None, aus=TABELLE, scr=None):
    sys.path.insert(0, os.path.join(K5, 'farbe_lesung', 'V4F_skripte'))
    import v4f
    import kf_k5g3_geo as G3
    geo_ordner = geo_ordner or os.path.dirname(G3.GEO3_ZIEL)
    scr = scr or os.path.join(os.environ.get('K5G3_SCR', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/'
                                                       r'c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/k5g3tex'), 'augen_mitten')
    shutil.rmtree(scr, ignore_errors=True)
    os.makedirs(scr)
    shutil.copy(os.path.join(geo_ordner, 'FORM.EMD'), os.path.join(scr, 'FORM.EMD'))
    shutil.copy(os.path.join(K5, 'ruhe', 'K5e_f1', 'FORM.TIM'), os.path.join(scr, 'FORM.TIM'))
    fj = json.load(open(os.path.join(K5, 'ruhe', 'K5e_f1', 'form.json'), encoding='utf-8'))
    json.dump(fj, open(os.path.join(scr, 'form.json'), 'w', encoding='utf-8'), indent=1)
    v4f._KC.pop(scr, None)
    k = v4f.kand(scr)
    lage, gier, clip, bild = k.stand(2)
    kam = v4f.P.kamera(2)
    licht, _kk = v4f.P.licht_fuer(2, k.licht)
    kf = k.kf(clip, bild)
    tris = v4f.dreiecke_id(k.mo, kf, lage, gier, kam, licht)
    _s, K = v4f.szene_id(scr, 2)
    Z = TX.lade_zonen()
    m = k.mo.meshes[TEIL]
    tv = np.array([x[:3] for x in m.tv], float)
    out = []
    for pkt in Z['augen']['nutzerpunkte']:
        x, y = (float(v) for v in pkt['punkt_vorlage'])
        xi, yi = int(math.floor(x)), int(math.floor(y))
        if K['mesh'][yi, xi] != TEIL:
            raise SystemExit('Nutzerpunkt %s liegt in Bild 2 nicht auf Teil %d' % (pkt['punkt_vorlage'], TEIL))
        fi = int(K['flaeche'][yi, xi])
        if fi >= 100000:
            raise SystemExit('Nutzerpunkt auf einem Viereck (Teil 22 hat nach B4 nur Dreiecke)')
        tr = [t for t in tris if t[5] == (TEIL, fi, 0)]
        if len(tr) != 1:
            raise SystemExit('Dreieck %d nicht eindeutig' % fi)
        p = tr[0][1]
        xs, ys = v4f.FB.abbilden(np.array([q[0] + v4f.FB.PHASE for q in p], float), np.array([q[1] + v4f.FB.PHASE for q in p], float),
                                 kam.abbildung)
        # Bildkoordinate des Punkts: Pixelmitte = x + 0,5 (rastern_id tastet bei +0,5 ab)
        P2 = np.array([x, y])
        A = np.array([[xs[0], xs[1], xs[2]], [ys[0], ys[1], ys[2]], [1.0, 1.0, 1.0]])
        w = np.linalg.solve(A, np.array([P2[0], P2[1], 1.0]))
        ie = m.tf[fi][1::2]
        V = tv[list(ie)]
        c = w @ V
        fl_bild = abs((xs[1] - xs[0]) * (ys[2] - ys[0]) - (xs[2] - xs[0]) * (ys[1] - ys[0])) / 2.0
        fl_3d = float(np.linalg.norm(np.cross(V[1] - V[0], V[2] - V[0]))) / 2.0
        a_px = float(pkt['punkt_px_vorlage'])
        r3 = math.sqrt(a_px / math.pi / (fl_bild / fl_3d))
        out.append(dict(name=pkt['name'], punkt_vorlage=[x, y], punkt_px_vorlage=a_px, dreieck=fi, bary=[round(float(t), 6) for t in w],
                        mitte=[round(float(t), 4) for t in c], radius=round(r3, 4), bild_je_raum=round(fl_bild / fl_3d, 6),
                        radius_px_bild2=round(math.sqrt(a_px / math.pi), 4)))
    tab = dict(_doc='Augenmitten (Knochenraum Teil 22) und Radien aus den Nutzerpunkten in Bild 2 (zonen_v2.json augen.nutzerpunkte: punkt_vorlage, '
                    'punkt_px_vorlage), zurueckgerechnet auf die fertige Geometrie (kf_k5g3_geo, FORM.EMD Key 1 = Bild 2) mit dem Buehnen-Renderer '
                    'v4f.dreiecke_id/szene_id. Radius: Augenscheibe deckt in Bild 2 genau die Flaeche des Nutzerpunkts (pi r^2 x Bild/Raum = '
                    'punkt_px_vorlage). Erzeugt mit kf_k5g3_auge.py mitten.',
               geo_form_emd_sha256=_sha(os.path.join(geo_ordner, 'FORM.EMD')), geo_emd_sha256=_sha(os.path.join(geo_ordner, 'BAHNSTEIG_FORM.EMD')),
               zonen_v2_sha256=_sha(TX.ZONEN), teil=TEIL, augen=out)
    json.dump(tab, open(aus, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    for a in out:
        print(a)
    return tab


# ------------------------------------------------------------------ Generator: eigene Augen-Texel
def _gx(f):
    po = (int(f['page']) & 15) * 128
    return [(int(u) + po, int(v)) for u, v in f['uv']]


def _punkt_dreieck_abstand(p, a, b, c):
    """kleinster Abstand Punkt p zum Dreieck abc (3D)."""
    ab, ac, ap = b - a, c - a, p - a
    d1, d2 = ab @ ap, ac @ ap
    if d1 <= 0 and d2 <= 0:
        return float(np.linalg.norm(ap))
    bp = p - b
    d3, d4 = ab @ bp, ac @ bp
    if d3 >= 0 and d4 <= d3:
        return float(np.linalg.norm(bp))
    vc = d1 * d4 - d3 * d2
    if vc <= 0 and d1 >= 0 and d3 <= 0:
        v = d1 / (d1 - d3)
        return float(np.linalg.norm(p - (a + v * ab)))
    cp = p - c
    d5, d6 = ab @ cp, ac @ cp
    if d6 >= 0 and d5 <= d6:
        return float(np.linalg.norm(cp))
    vb = d5 * d2 - d1 * d6
    if vb <= 0 and d2 >= 0 and d6 <= 0:
        w = d2 / (d2 - d6)
        return float(np.linalg.norm(p - (a + w * ac)))
    va = d3 * d6 - d5 * d4
    if va <= 0 and (d4 - d3) >= 0 and (d5 - d6) >= 0:
        w = (d4 - d3) / ((d4 - d3) + (d5 - d6))
        return float(np.linalg.norm(p - (b + w * (c - b))))
    n = np.cross(ab, ac)
    return abs(float(n @ ap)) / float(np.linalg.norm(n))


def _bary2(P, T):
    """baryzentrische Koordinaten der Punkte P (n x 2) im 2D-Dreieck T (3 x 2); None bei Flaeche 0."""
    a, b, c = (np.asarray(t, float) for t in T)
    d = (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1])
    if abs(d) < 1e-9:
        return None
    w1 = ((P[:, 0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (P[:, 1] - a[1])) / d
    w2 = ((b[0] - a[0]) * (P[:, 1] - a[1]) - (P[:, 0] - a[0]) * (b[1] - a[1])) / d
    return np.stack([1 - w1 - w2, w1, w2], 1)


def _klemmen(w):
    """baryzentrische Koordinaten auf das Dreieck klemmen (Randtexel der Dilatation)."""
    w = np.clip(w, 0.0, None)
    s = w.sum(1, keepdims=True)
    return w / np.where(s > 0, s, 1.0)


def dekal(form, px, reihen, T, rampe, par, belegt):
    """Augen-Texel (siehe Kopf). form: fertige Form (nach Spalt und Kopien), px: Index-Bild (wird beschrieben), reihen: Liste der CLUT-Zeilen
    (eine Zeile 'auge' wird angehaengt), T: TIM-Kopf (clut_y), rampe: dict(worte, w, L) der Augen-Rampe (aufsteigend nach Quell-L*),
    belegt: bool-Bild der gelesenen Texel (wird erweitert). Rueckgabe Bericht."""
    tab = json.load(open(TABELLE, encoding='utf-8'))
    tpr = float(par['textur']['auge']['texel_je_radius'])
    m = form['meshes'][tab['teil']]
    V = np.array(m['ecken'], float)[:, :3]
    zeile_neu = len(reihen)
    cy = int(T['clut_y'])
    lab_reihen = [TX.lab_zeile(z) for z in reihen]
    worte_neu = []          # Worte der neuen Zeile (Index = Position + 1)
    wort_idx = {}

    def idx_von(w):
        if w not in wort_idx:
            worte_neu.append(int(w))
            wort_idx[w] = len(worte_neu)
        return wort_idx[w]
    ber = dict(augen=[], zeile=zeile_neu)
    gesehen = set()
    auftraege = []          # (auge_nr, fi, uv_neu (3x2 int, global), block-Lage, texel-Liste)
    for an, a in enumerate(tab['augen']):
        c = np.array(a['mitte'], float)
        r = float(a['radius'])
        s_tex = tpr / r
        S = []
        for fi, d in enumerate(m['dreiecke']):
            ie = d['e']
            if _punkt_dreieck_abstand(c, V[ie[0]], V[ie[1]], V[ie[2]]) < r:
                S.append(fi)
        info = dict(name=a['name'], dreiecke=len(S), radius=r, texel_je_einheit=round(s_tex, 6), flaechen=[])
        eye_texel = []      # (fi, x, y, L_alt, wort_alt) fuer den Rang
        for fi in S:
            if fi in gesehen:
                raise SystemExit('Dreieck %d beruehrt beide Augen' % fi)
            gesehen.add(fi)
            d = m['dreiecke'][fi]
            ie = d['e']
            A3, B3, C3 = V[ie[0]], V[ie[1]], V[ie[2]]
            e1 = (B3 - A3) / np.linalg.norm(B3 - A3)
            n = np.cross(B3 - A3, C3 - A3)
            e2 = np.cross(n, e1)
            e2 /= np.linalg.norm(e2)
            P2 = np.array([[0.0, 0.0], [float((B3 - A3) @ e1), 0.0], [float((C3 - A3) @ e1), float((C3 - A3) @ e2)]]) * s_tex
            P2 -= P2.min(0)
            uvr = np.rint(P2).astype(int) + 1          # 1 Texel Rand
            h, w = int(uvr[:, 1].max()) + 2, int(uvr[:, 0].max()) + 2
            mk = np.zeros((h, w), np.uint8)
            cv2.fillPoly(mk, [uvr.astype(np.int32)], 1)
            cv2.polylines(mk, [uvr.astype(np.int32)], True, 1)
            mk = cv2.dilate(mk, np.ones((3, 3), np.uint8)).astype(bool)
            lage = TX.platzieren(belegt, mk)
            if lage is None:
                raise SystemExit('kein freier Texturplatz fuer das Augen-Dreieck %d' % fi)
            nx, ny = lage
            belegt[ny:ny + h, nx:nx + w] |= mk
            uvg = uvr + np.array([nx, ny])
            # Texelmitten -> 3D (ueber die GERUNDETE UV, so wie der Renderer abtastet) -> alte UV
            ys, xs = np.nonzero(mk)
            Pc = np.stack([xs + 0.5, ys + 0.5], 1).astype(float)
            wb = _bary2(Pc, uvr.astype(float))
            if wb is None:
                cen = np.full((len(Pc), 3), 1.0 / 3.0)
            else:
                cen = _klemmen(wb)
            X3 = cen @ np.stack([A3, B3, C3])
            alt = _gx(d)
            zalt = ((int(d['clut']) >> 6) & 0x1FF) - cy
            uva = cen @ np.array(alt, float)
            ua = np.clip(np.floor(uva[:, 0]).astype(int), 0, px.shape[1] - 1)
            va = np.clip(np.floor(uva[:, 1]).astype(int), 0, px.shape[0] - 1)
            ia = px[va, ua].astype(int)
            wa = [int(reihen[zalt][i]) for i in ia]
            dist = np.linalg.norm(X3 - c[None, :], axis=1)
            texel = []
            for j in range(len(xs)):
                gx_, gy_ = int(xs[j] + nx), int(ys[j] + ny)
                if dist[j] <= r:
                    eye_texel.append((len(auftraege), j, float(lab_reihen[zalt][ia[j], 0])))
                    texel.append([gx_, gy_, None])
                else:
                    texel.append([gx_, gy_, wa[j]])
            auftraege.append(dict(fi=fi, uvg=uvg, texel=texel))
            info['flaechen'].append(dict(dreieck=fi, uv_alt=alt, zeile_alt=zalt, block=[int(nx), int(ny), w, h],
                                         augen_texel=int((dist <= r).sum()), texel=int(len(xs))))
        # Rang der alten Texel im Auge -> Augen-Rampe (texelgewichtet, V3)
        if not eye_texel:
            raise SystemExit('Auge %s ohne Texel' % a['name'])
        Lb = np.array([t[2] for t in eye_texel])
        o = np.argsort(Lb, kind='stable')
        q = np.empty(len(Lb))
        q[o] = (np.arange(len(Lb)) + 0.5) / len(Lb)
        rw = np.asarray(rampe['w'], float)
        rc = np.cumsum(rw)
        rq = (rc - 0.5 * rw) / rc[-1]
        for (ai, j, _L), qq in zip(eye_texel, q):
            jj = int(np.clip(np.searchsorted(rq, qq), 0, len(rq) - 1))
            auftraege[ai]['texel'][j][2] = int(rampe['worte'][jj])
        info['augen_texel'] = len(eye_texel)
        ber['augen'].append(info)
    # Worte -> Indizes, Texel schreiben, UV setzen
    for au in auftraege:
        for gx_, gy_, w in au['texel']:
            px[gy_, gx_] = idx_von(int(w))
    if len(worte_neu) > 255:
        raise SystemExit('Augen-Zeile: %d Worte > 255' % len(worte_neu))
    augen_worte = set(int(w) for w in rampe['worte'])
    fuell = worte_neu[0]
    zeile = [fuell] + worte_neu + [fuell] * (255 - len(worte_neu))
    reihen.append(zeile)
    for au in auftraege:
        d = m['dreiecke'][au['fi']]
        uvg = au['uvg']
        npg = int(uvg[:, 0].min()) // 128
        uv = [[int(x - npg * 128), int(y)] for x, y in uvg]
        if max(u for u, _v in uv) > 255 or max(v for _u, v in uv) > 255:
            raise SystemExit('Augen-UV ausserhalb der Seite: %s' % uv)
        d['uv'] = uv
        d['page'] = (int(d['page']) & ~0xF) | npg
        d['clut'] = ((((cy + zeile_neu) & 0x1FF) << 6) | (int(d['clut']) & 0x3F))
    ber['worte'] = len(worte_neu)
    ber['augen_eintraege'] = sorted(wort_idx[w] for w in augen_worte if w in wort_idx)
    ber['dreiecke'] = sorted(gesehen)
    return ber


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == 'mitten':
        mitten()
    else:
        raise SystemExit(__doc__)
