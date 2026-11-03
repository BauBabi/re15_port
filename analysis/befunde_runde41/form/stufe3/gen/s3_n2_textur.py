"""Nachbesserung 2, Textur-Spur: messen, wo die Textur der Riesenform kaputt wirkt, und die Ursachen beheben.

    python s3_n2_textur.py netz NAME=EMD,TIM[,naht.json] ...        blickunabhaengig: Kantensprung je gemeinsamer Kante, Blockmass,
                                                                      Hellflecken je Teil (Rumpf = Referenz) -> JSON auf stdout
    python s3_n2_textur.py bild NAME=EMD,TIM[,naht.json] ... --aus PNG [--json J]
                                                                      Spielkameras (5080 Schnitt 4, 5090 Schnitt 11) mit form_buehne-Zeichenweg:
                                                                      Kontrast ueber Polygonkanten gegen Flaecheninneres, Heatmap-Blatt
    python s3_n2_textur.py bauen K_DIR AUS_DIR                        Nachfolger von s3_n1_textur.py bauen, fuer jede K-Variante

Masse (Gestaltungsmass, kein RE-Wert):
- Kantensprung (Netz): je Paar Flaechen mit gemeinsamer 3D-Kante im selben Mesh die oertliche Mittelfarbe beider Seiten an drei Stellen
  der Kante (je Seite Texel 0,5/1,5/2,5 nach innen), Mittel |dRGB| (0-255). Gewicht = Kantenlaenge in Welteinheiten. "UV-stetig" =
  gleiche Seite/CLUT-Zeile und beide Kantenenden <= 1,5 Texel gleich.
- Gelenkkoerper-Sprung: Mittelfarbe je Koerperflaeche gegen die naechste Originalflaeche (Mitte zu Mitte) desselben Meshes.
- Blockmass (Netz): je Flaeche mittlerer Helligkeitssprung zwischen Nachbartexeln (dY je Texelschritt) mal Texelgroesse (Welteinheiten je
  Texel) / 100. Grosse, kontrastreiche Texel = Mosaik; feine Koernung bei kleinen Texeln = Haut.
- Hellfleck: Anteil der Texel einer Flaeche mit Y > Median der Flaeche + 60.
- Gestreckt: Weltflaechen-Anteil der Flaechen mit Texelgroesse > 3 x Median des Teils oder UV-Flaeche <= 1 Texel (z. B. Faecherkappen,
  deren UV auf eine Texelzeile faellt: konzentrische Streifen).
- Bild: Albedo (Textur ohne Licht), Kennung und Texel je Pixel; Kantenkontrast = Mittel |dRGB| zwischen Mittelwerten von je 2 Pixeln
  beiderseits einer Polygonkante, Innenkontrast = dasselbe innerhalb einer Flaeche; Block = Sprung an jeder Texelgrenze x Kantenlaenge des
  groesseren Texels in Pixeln (Mittel ueber die Texelgrenzen): grosse, kontrastreiche Texelbloecke = Mosaik; Texel px = mittlere
  Texelgroesse im Bild; _gelenk_grenze = Kantenkontrast genau zwischen Gelenkkoerper und uebriger Haut.
- haut_stoff(): nahtlose, fleckfreie H2-Hautstruktur fuer Seite 3 (Ersatz fuer k_bau.stoff), quilt_torus(): periodische Kachel.
Bau (Gelenkkoerper, Kappen, TIM-Seiten): s3_n2_textur_bau.py. Tabelle: s3_n2_t_tabelle.py, Blaetter: s3_n2_t_blatt.py.
"""
import os, sys, json, copy, shutil, argparse, math
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S              # noqa: E402
import form_emd as FE           # noqa: E402
import form_buehne as B         # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402
sys.path.insert(0, os.path.join(S.FORM_DIR, 'varianten', 'K_alligator', 'generator'))
import k_naht as KN             # noqa: E402

YK = np.array([0.299, 0.587, 0.114])
JE_KOERPER = dict(dreiecke=2 * KN.KUGEL_U, vierecke=(KN.KUGEL_V - 2) * KN.KUGEL_U, ecken=(KN.KUGEL_V - 1) * KN.KUGEL_U + 2)
RUMPF = (0, 7, 10)
SCHWANZ = (1, 2, 3, 4, 5, 6)
BEINE = (8, 9, 24, 25, 26, 27)
ARME = (11, 12, 13, 16, 17, 18)
KLAUEN = (14, 15, 28, 30, 19, 20, 29, 31)
TEILE = ('rumpf', 'schwanz', 'beine', 'arme', 'hals', 'kopf', 'kiefer', 'klauen', 'gelenk')
ROH = os.path.join(S.FORM_DIR, 'stufe3', 'roh', 'n2_textur')
N2 = os.path.join(S.FORM_DIR, 'stufe3', 'nachbesserung2', 'textur')

# Spielansichten wie NB1 (s3_n1_belegblatt / s3_n1_lochecht): (Raum, Schnitt, Lage, Clip, Bild, Gier)
ANSICHTEN = [('5080', 4, (-23500, 0, -18200), 2, 0, 0), ('5080', 4, (-23500, 0, -18200), 2, 0, 1024),
             ('5080', 4, (-23500, 0, -18200), 2, 0, 2048), ('5080', 4, (-23500, 0, -18200), 2, 0, 3072),
             ('5090', 11, (941, 0, -23776), 10, 45, 3874), ('5090', 11, (941, 0, -23776), 10, 45, 802),
             ('5090', 11, (941, 0, -23776), 10, 45, 1826)]


# ═══════════════════════════════════════════════════ Laden, Teile
class Tex:
    def __init__(self, tim_bytes):
        t = FE.tim_lesen(tim_bytes)
        self.px = t['pixel'].astype(np.int64)
        self.cy = t['clut_y']
        self.h, self.w = self.px.shape
        c = np.array(t['cluts'], dtype=np.int64)
        self.lut = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], -1).astype(float)
        self.cluts = t['cluts']

    def zeile(self, clut):
        return ((int(clut) >> 6) & 0x1FF) - self.cy

    def farben(self, x, y, z):
        """x, y Texel (ganzzahlig, schon mit Seite), z CLUT-Zeile -> (N,3), gueltig (N,)."""
        x = np.asarray(x, int)
        y = np.asarray(y, int)
        ok = (x >= 0) & (x < self.w) & (y >= 0) & (y < self.h) & (0 <= z < len(self.lut))
        xx, yy = np.clip(x, 0, self.w - 1), np.clip(y, 0, self.h - 1)
        zz = min(max(z, 0), len(self.lut) - 1)
        return self.lut[zz][self.px[yy, xx]], ok & (self.px[yy, xx] != 0)


def lade(emd_pfad, tim_pfad):
    return FE.lese_emd(open(emd_pfad, 'rb').read()), Tex(open(tim_pfad, 'rb').read())


def koerper_zahl(form, naht):
    """Mesh -> Zahl der angehaengten Gelenkkoerper (k_naht.schliessen haengt sie in naht['gelenke']-Reihenfolge an)."""
    out = {}
    if not naht:
        return out
    for nm, (kind, fq, fn, _ax) in naht['gelenke'].items():
        if fq <= 0 or fn <= 0:
            continue
        b = kind if nm.startswith('Muffe ') else form['knochen'][kind]['eltern']
        out[b] = out.get(b, 0) + 1
    return out


def koerper_namen(form, naht):
    out = {}
    if not naht:
        return out
    for nm, (kind, fq, fn, _ax) in naht['gelenke'].items():
        if fq <= 0 or fn <= 0:
            continue
        b = kind if nm.startswith('Muffe ') else form['knochen'][kind]['eltern']
        out.setdefault(b, []).append(nm)
    return out


def teil_von(b, f, ist_koerper):
    if ist_koerper:
        return 'gelenk'
    if b in RUMPF:
        return 'rumpf'
    if b in SCHWANZ:
        return 'schwanz'
    if b in BEINE:
        return 'beine'
    if b in (13, 18) and (int(f['page']) & 0xF) == 3:
        return 'klauen'
    if b in ARME:
        return 'arme'
    if b == 21:
        return 'hals'
    if b == 22:
        return 'kopf'
    if b == 23:
        return 'kiefer'
    return 'klauen'


def flaechen(form, naht):
    """Liste aller Flaechen: dict(b, art 0/1, i, f, teil, koerper (Name oder None))."""
    kz = koerper_namen(form, naht)
    out = []
    for b, m in enumerate(form['meshes']):
        nk = len(kz.get(b, []))
        nd, nv = len(m['dreiecke']), len(m['vierecke'])
        d0, v0 = nd - nk * JE_KOERPER['dreiecke'], nv - nk * JE_KOERPER['vierecke']
        for art, liste, grenze, je in ((0, m['dreiecke'], d0, JE_KOERPER['dreiecke']), (1, m['vierecke'], v0, JE_KOERPER['vierecke'])):
            for i, f in enumerate(liste):
                kname = kz[b][(i - grenze) // je] if i >= grenze else None
                out.append(dict(b=b, art=art, i=i, f=f, teil=teil_von(b, f, kname is not None), koerper=kname))
    return out


# ═══════════════════════════════════════════════════ Texel einer Flaeche
def uv_tris(f):
    uv = np.array(f['uv'][:len(f['e'])], float)
    return [uv[[0, 1, 2]]] if len(uv) == 3 else [uv[[0, 1, 3]], uv[[0, 3, 2]]]


def texel_menge(f):
    """Texel (u, v) im UV-Polygon der Flaeche (Texelmitte u+0,5 innen); leer -> Texel unter der UV-Mitte."""
    pts = set()
    for t in uv_tris(f):
        x0, y0 = np.floor(t.min(0)).astype(int)
        x1, y1 = np.ceil(t.max(0)).astype(int)
        if x1 - x0 > 300 or y1 - y0 > 300:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        a, b_, c = t
        fl = (b_[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b_[1] - a[1])
        if abs(fl) < 1e-9:
            continue
        w1 = ((PX - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (PY - a[1])) / fl
        w2 = ((b_[0] - a[0]) * (PY - a[1]) - (PX - a[0]) * (b_[1] - a[1])) / fl
        inn = (w1 >= -1e-6) & (w2 >= -1e-6) & (1 - w1 - w2 >= -1e-6)
        for xx, yy in zip(np.floor(PX[inn]).astype(int), np.floor(PY[inn]).astype(int)):
            pts.add((int(xx), int(yy)))
    if not pts:
        m = np.array(f['uv'][:len(f['e'])], float).mean(0)
        pts.add((int(m[0]), int(m[1])))
    return sorted(pts)


def flaeche_texel(f, tex):
    pts = texel_menge(f)
    po = (int(f['page']) & 0xF) * 128
    xy = np.array(pts, int)
    c, ok = tex.farben(xy[:, 0] + po, xy[:, 1], tex.zeile(f['clut']))
    return xy, c, ok


def welt_flaeche(V, f):
    P = V[f['e']]
    if len(P) == 3:
        return 0.5 * np.linalg.norm(np.cross(P[1] - P[0], P[2] - P[0]))
    return 0.5 * np.linalg.norm(np.cross(P[1] - P[0], P[3] - P[0])) + 0.5 * np.linalg.norm(np.cross(P[3] - P[0], P[2] - P[0]))


def uv_flaeche(f):
    return sum(0.5 * abs((t[1][0] - t[0][0]) * (t[2][1] - t[0][1]) - (t[2][0] - t[0][0]) * (t[1][1] - t[0][1])) for t in uv_tris(f))


def kanten(f):
    """Umlaufkanten als Eckindex-Paare in der Flaechenliste (Viereck Z-foermig: a,b,d,c)."""
    if len(f['e']) == 3:
        return [(0, 1), (1, 2), (2, 0)]
    return [(0, 1), (1, 3), (3, 2), (2, 0)]


def oertlich(f, tex, k0, k1, ts=(0.2, 0.5, 0.8)):
    """Oertliche Mittelfarbe je Stelle t entlang der Kante (Ecke k0 -> k1), je Seite Texel 0,5/1,5/2,5 nach innen (zur UV-Mitte)."""
    uv = np.array(f['uv'][:len(f['e'])], float)
    c = uv.mean(0)
    po = (int(f['page']) & 0xF) * 128
    z = tex.zeile(f['clut'])
    out = []
    for t in ts:
        p = uv[k0] + t * (uv[k1] - uv[k0])
        d = c - p
        L = float(np.linalg.norm(d))
        if L < 1e-6:
            q = [p]
        else:
            d = d / L
            q = [p + d * min(s, L) for s in (0.5, 1.5, 2.5)]
        q = np.array(q)
        col, ok = tex.farben(np.floor(q[:, 0]).astype(int) + po, np.floor(q[:, 1]).astype(int), z)
        out.append(col[ok].mean(0) if ok.any() else None)
    return out


# ═══════════════════════════════════════════════════ Netzmass
def netz_mass(form, tex, naht=None):
    F = flaechen(form, naht)
    erg = {t: dict(kl=0.0, kj=0.0, kl_bruch=0.0, kl_unstet=0.0, kj_unstet=0.0, n_kanten=0, fl=0.0, block=0.0, dy=0.0, gr=0.0,
                   hell=0.0, texel=0, gk=[]) for t in TEILE}
    je_flaeche = {}
    # je Mesh: Kanten ueber Lage (Ecken mit gleicher Lage teilen sich die Kante, auch ueber doppelte Ecken an UV-Naehten)
    for b, m in enumerate(form['meshes']):
        V = np.array(m['ecken'], float)
        if not len(V):
            continue
        fb = [x for x in F if x['b'] == b]
        schl = {}
        for k, x in enumerate(fb):
            f = x['f']
            for (a, c) in kanten(f):
                pa, pc = tuple(np.round(V[f['e'][a]] / 2.0).astype(int)), tuple(np.round(V[f['e'][c]] / 2.0).astype(int))
                if pa == pc:
                    continue
                key = (pa, pc) if pa < pc else (pc, pa)
                schl.setdefault(key, []).append((k, a, c, pa))
        for key, lst in schl.items():
            if len(lst) != 2:
                continue
            (k1, a1, c1, p1), (k2, a2, c2, p2) = lst
            x1, x2 = fb[k1], fb[k2]
            if x1['koerper'] != x2['koerper']:
                continue
            f1, f2 = x1['f'], x2['f']
            # Ecken gleicher Lage gegenueberstellen
            if p1 != p2:
                a2, c2 = c2, a2
            m1 = oertlich(f1, tex, a1, c1)
            m2 = oertlich(f2, tex, a2, c2)
            d = [float(np.abs(u - v).mean()) for u, v in zip(m1, m2) if u is not None and v is not None]
            if not d:
                continue
            j = float(np.mean(d))
            L = float(np.linalg.norm(V[f1['e'][a1]] - V[f1['e'][c1]]))
            uv1 = np.array(f1['uv'][:len(f1['e'])], float)
            uv2 = np.array(f2['uv'][:len(f2['e'])], float)
            stetig = (f1['page'] == f2['page'] and f1['clut'] == f2['clut'] and np.abs(uv1[a1] - uv2[a2]).max() <= 1.5
                      and np.abs(uv1[c1] - uv2[c2]).max() <= 1.5)
            t = x1['teil'] if x1['teil'] == x2['teil'] else ('gelenk' if 'gelenk' in (x1['teil'], x2['teil']) else x1['teil'])
            e = erg[t]
            e['kl'] += L
            e['kj'] += j * L
            e['n_kanten'] += 1
            if j > 30:
                e['kl_bruch'] += L
            if not stetig:
                e['kl_unstet'] += L
                e['kj_unstet'] += j * L
            for kk in (k1, k2):
                q = je_flaeche.setdefault((b, fb[kk]['art'], fb[kk]['i']), [0.0, 0.0])
                q[0] = max(q[0], j)
        # Flaechenmasse
        for x in fb:
            f = x['f']
            xy, col, ok = flaeche_texel(f, tex)
            if not ok.any():
                continue
            Y = col @ YK
            lut = {(int(u), int(v)): y for (u, v), y, o in zip(xy, Y, ok) if o}
            dif = [abs(y - lut[(u + 1, v)]) for (u, v), y in lut.items() if (u + 1, v) in lut]
            dif += [abs(y - lut[(u, v + 1)]) for (u, v), y in lut.items() if (u, v + 1) in lut]
            dy = float(np.mean(dif)) if dif else 0.0
            A = welt_flaeche(V, f)
            Au = max(uv_flaeche(f), 0.25)
            gr = math.sqrt(A / Au)
            hell = float((Y[ok] > np.median(Y[ok]) + 60).mean())
            e = erg[x['teil']]
            e.setdefault('gr_liste', []).append((A, gr, Au))
            e['fl'] += A
            e['block'] += A * dy * gr / 100.0
            e['dy'] += A * dy
            e['gr'] += A * gr
            e['hell'] += A * hell
            e['texel'] += int(ok.sum())
            q = je_flaeche.setdefault((b, x['art'], x['i']), [0.0, 0.0])
            q[1] = dy * gr / 100.0
    # Gelenkkoerper gegen die naechste Originalflaeche (Mittelfarbe)
    for b, m in enumerate(form['meshes']):
        fb = [x for x in F if x['b'] == b]
        org = [x for x in fb if x['koerper'] is None]
        kor = [x for x in fb if x['koerper'] is not None]
        if not kor or not org:
            continue
        V = np.array(m['ecken'], float)
        om = np.array([V[x['f']['e']].mean(0) for x in org])
        ofarbe = []
        for x in org:
            _xy, c, ok = flaeche_texel(x['f'], tex)
            ofarbe.append(c[ok].mean(0) if ok.any() else None)
        for x in kor:
            _xy, c, ok = flaeche_texel(x['f'], tex)
            if not ok.any():
                continue
            mi = V[x['f']['e']].mean(0)
            j = int(np.argmin(np.linalg.norm(om - mi, axis=1)))
            if ofarbe[j] is None:
                continue
            erg['gelenk']['gk'].append(float(np.abs(c[ok].mean(0) - ofarbe[j]).mean()))
    out = {}
    for t, e in erg.items():
        if e['fl'] <= 0 and e['kl'] <= 0:
            continue
        gl = e.get('gr_liste', [])
        if gl:
            med = float(np.median([g for _a, g, _u in gl]))
            gestreckt = sum(a for a, g, u in gl if g > 3 * med or u <= 1.0) / max(e['fl'], 1e-9)
        else:
            gestreckt = 0.0
        out[t] = dict(gestreckt=round(gestreckt, 3), kantensprung=round(e['kj'] / max(e['kl'], 1e-9), 1),
                      anteil_bruch_ueber_30=round(e['kl_bruch'] / max(e['kl'], 1e-9), 3),
                      anteil_uv_unstetig=round(e['kl_unstet'] / max(e['kl'], 1e-9), 3),
                      sprung_unstetig=round(e['kj_unstet'] / max(e['kl_unstet'], 1e-9), 1) if e['kl_unstet'] else None,
                      blockmass=round(e['block'] / max(e['fl'], 1e-9), 2), dy_texelschritt=round(e['dy'] / max(e['fl'], 1e-9), 1),
                      texelgroesse=round(e['gr'] / max(e['fl'], 1e-9), 1), hellfleck=round(e['hell'] / max(e['fl'], 1e-9), 4),
                      kanten=e['n_kanten'], flaeche_welt=round(e['fl']),
                      koerper_gegen_umgebung=round(float(np.mean(e['gk'])), 1) if e['gk'] else None)
    ges_fl = sum(e['fl'] for e in erg.values())
    ges_kl = sum(e['kl'] for e in erg.values())
    out['_gesamt'] = dict(kantensprung=round(sum(e['kj'] for e in erg.values()) / max(ges_kl, 1e-9), 1),
                          anteil_bruch_ueber_30=round(sum(e['kl_bruch'] for e in erg.values()) / max(ges_kl, 1e-9), 3),
                          blockmass=round(sum(e['block'] for e in erg.values()) / max(ges_fl, 1e-9), 2),
                          hellfleck=round(sum(e['hell'] for e in erg.values()) / max(ges_fl, 1e-9), 4))
    return out, je_flaeche


# ═══════════════════════════════════════════════════ Bild: Zeichenweg der Buehne mit Kennung je Pixel
def dreiecke_k(mo, kf, lage, gier, kam, licht=None):
    """form_buehne.dreiecke mit Kennung (b, art, i) je Dreieck."""
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = mo.pose(kf)
    lage = np.array(lage, dtype=np.int64)
    ctx = B.licht_kontext(licht, lage) if licht else None
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW = (kam.V @ W) >> 12
        vT = ((kam.V @ t) >> 12) + kam.t
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], dtype=np.int64)
            q = (VW @ v.T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = B._gte(vx, vy, vz, kam.h)
            if ctx is not None and norms:
                L = (W.T @ ctx[0].T).T >> 12
                fa = B.prim_zu_sdl(B.ncct(L, ctx[1], ctx[2], np.array([x[:3] for x in norms], dtype=np.int64)))
            else:
                fa = np.full((max(len(norms), 1), 3), 255, dtype=np.int64)
            for gi, (g, uv) in enumerate(zip(faces, uvs)):
                ie = g[1::2]
                inn = g[0::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = B._mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                po = (uv[5] & 0xF) * 128
                zeile = (uv[2] >> 6) & 0x1FF
                kenn = (b, viereck, gi)
                if viereck:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for a, bb, cc in ((0, 1, 3), (0, 3, 2)):
                        tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                        aus.append((tief, (p[a], p[bb], p[cc]), (tx[a], tx[bb], tx[cc]), zeile, (fa[inn[a]], fa[inn[bb]], fa[inn[cc]]), kenn))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    tief = (int(vz[ie[0]]) + int(vz[ie[1]]) + int(vz[ie[2]])) / 3.0
                    aus.append((tief, tuple(p), tuple(tx), zeile, (fa[inn[0]], fa[inn[1]], fa[inn[2]]), kenn))
    return aus


def rastern_k(tris, tex, ziel, abbildung):
    """form_buehne.rastern, dazu Albedo (Textur ohne Licht) und Kennung je Pixel (b * 100000 + art * 50000 + i, -1 = leer)."""
    H, W = ziel.shape[:2]
    alb = np.zeros((H, W, 3), np.uint8)
    kenn = np.full((H, W), -1, np.int64)
    texel = np.full((H, W), -1, np.int64)
    for _t, p, uv, zeile, farbe, kk in sorted(tris, key=lambda t: -t[0]):
        x, y = B.abbilden(np.array([q[0] + B.PHASE for q in p], float), np.array([q[1] + B.PHASE for q in p], float), abbildung)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, W)
        y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1.0 - w1 - w2
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not innen.any():
            continue
        u = np.array([q[0] for q in uv], float)
        v = np.array([q[1] for q in uv], float)
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, tex.w - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, tex.h - 1)
        z = zeile - tex.clut_y
        z = z if 0 <= z < tex.lut.shape[0] else 0
        rgba = tex.lut[z][tex.px[vv, uu]]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        s = ziel[y0:y1, x0:x1]
        s[tref] = rgb[tref]
        a = alb[y0:y1, x0:x1]
        a[tref] = rgba[..., :3][tref]
        kb = kenn[y0:y1, x0:x1]
        kb[tref] = kk[0] * 100000 + kk[1] * 50000 + kk[2]
        tb = texel[y0:y1, x0:x1]
        tb[tref] = (vv * 4096 + uu)[tref]
    return alb, kenn, texel


_RAUM = {}


def raum(r):
    if r not in _RAUM:
        _RAUM[r] = B.Raum(r)
    return _RAUM[r]


def ansicht(emd, tim, raum_name, schnitt, lage, clip, bild, gier, hintergrund=True):
    R = raum(raum_name)
    s = R.schnitt(schnitt)
    kam = B.Kamera.aus_rid(s['pos'], s['ziel'], s['fov'])
    mo = FE.modell(emd, tim)
    tx = B.Textur(tim)
    kf = B.bild_von_clip(mo, clip, bild)
    bg = None
    if hintergrund:
        p = B.hintergrund_bss(raum_name, schnitt)
        if p:
            bg = B.bild_laden(p)
    ziel = np.array(bg, copy=True) if bg is not None else np.full((480, 640, 3), 40, np.uint8)
    alb, kenn, texel = rastern_k(dreiecke_k(mo, kf, lage, gier, kam, R.licht(schnitt)), tx, ziel, kam.abbildung)
    return ziel, alb, kenn, texel


def teil_karte(form, naht):
    """Kennung -> Teilname."""
    out = {}
    for x in flaechen(form, naht):
        out[x['b'] * 100000 + x['art'] * 50000 + x['i']] = x['teil']
    return out


def gauss(img, s=2.0):
    from scipy import ndimage
    return np.stack([ndimage.gaussian_filter(img[..., k].astype(float), s) for k in range(img.shape[-1])], -1)


def block_bild(alb, kenn, texel):
    """Texelbloecke im Bild: je Pixel Groesse seines Texels (Wurzel der Pixelzahl gleicher Flaeche und gleichen Texels) und an jeder
    Texelgrenze innerhalb einer Flaeche der Sprung |dRGB| (Albedo). Blockmass = Sprung x Kantenlaenge des groesseren Texels (px):
    grosse, kontrastreiche Bloecke = Mosaik. Rueckgabe (Sprung-Karte, Groessen-Karte, Liste (Pixelmaske-Index, d, s))."""
    fig = kenn >= 0
    key = np.where(fig, kenn * 70000 + np.maximum(texel, 0) % 70000, -1)
    u, inv, cnt = np.unique(key[fig], return_inverse=True, return_counts=True)
    gr = np.zeros(kenn.shape)
    gr[fig] = np.sqrt(cnt[inv])
    A = alb.astype(float)
    sprung = np.zeros(kenn.shape)
    bl = np.zeros(kenn.shape)
    for axis in (0, 1):
        if axis == 0:
            k0, k1, t0, t1, a0, a1, g0, g1 = kenn[:-1], kenn[1:], texel[:-1], texel[1:], A[:-1], A[1:], gr[:-1], gr[1:]
        else:
            k0, k1, t0, t1, a0, a1, g0, g1 = kenn[:, :-1], kenn[:, 1:], texel[:, :-1], texel[:, 1:], A[:, :-1], A[:, 1:], gr[:, :-1], gr[:, 1:]
        gr_ = (k0 >= 0) & (k0 == k1) & (t0 != t1)
        d = np.abs(a0 - a1).mean(-1)
        w = np.maximum(g0, g1)
        sp = sprung[:-1] if axis == 0 else sprung[:, :-1]
        np.maximum(sp, np.where(gr_, d, 0), out=sp)
        bb = bl[:-1] if axis == 0 else bl[:, :-1]
        np.maximum(bb, np.where(gr_, d * w, 0), out=bb)
    return sprung, gr, bl


def bild_mass(alb, kenn, tk, texel=None):
    """Kantenkontrast (Polygonkanten) gegen Innenkontrast, Mosaik im Bild, je Teil. Rueckgabe dict + Heatmaps (Kante, Mosaik)."""
    A = alb.astype(float)
    fig = kenn >= 0
    H, W = kenn.shape
    kante_heat = np.zeros((H, W))
    sums = {}

    def dazu(t, art, v):
        q = sums.setdefault(t, {}).setdefault(art, [0.0, 0])
        q[0] += float(v.sum())
        q[1] += int(v.size)
    teil = np.full((H, W), '', dtype=object)
    for k, t in tk.items():
        pass
    # Teil je Pixel ueber Nachschlag (schneller: eindeutige Kennungen)
    ids = np.unique(kenn[fig])
    tmap = {int(i): tk.get(int(i), '?') for i in ids}
    for axis in (0, 1):
        # Paare (p, p+1) entlang axis; Mittel von 2 Pixeln je Seite: p-1,p gegen p+1,p+2
        if axis == 0:
            a1, a0, b0, b1 = A[:-3], A[1:-2], A[2:-1], A[3:]
            k1, k0, l0, l1 = kenn[:-3], kenn[1:-2], kenn[2:-1], kenn[3:]
        else:
            a1, a0, b0, b1 = A[:, :-3], A[:, 1:-2], A[:, 2:-1], A[:, 3:]
            k1, k0, l0, l1 = kenn[:, :-3], kenn[:, 1:-2], kenn[:, 2:-1], kenn[:, 3:]
        ok = (k1 >= 0) & (k0 >= 0) & (l0 >= 0) & (l1 >= 0)
        kante = ok & (k1 == k0) & (l0 == l1) & (k0 != l0)
        innen = ok & (k1 == k0) & (k0 == l0) & (l0 == l1)
        d = np.abs((a1 + a0) / 2 - (b0 + b1) / 2).mean(-1)
        if axis == 0:
            kh = kante_heat[1:-2]
        else:
            kh = kante_heat[:, 1:-2]
        np.maximum(kh, np.where(kante, d, 0), out=kh)
        # Grenze Gelenkkoerper / uebrige Haut (genau eine Seite Gelenkkoerper)
        gk_ids = np.array([i for i, tt in tmap.items() if tt == 'gelenk'], np.int64)
        if len(gk_ids):
            g0, g1 = np.isin(k0, gk_ids), np.isin(l0, gk_ids)
            dazu('_gelenk_grenze', 'kante', d[kante & (g0 != g1)])
            dazu('_haut_kante', 'kante', d[kante & ~g0 & ~g1])
        else:
            dazu('_haut_kante', 'kante', d[kante])
        for i in np.unique(k0[kante | innen]):
            t = tmap.get(int(i), '?')
            mk = (k0 == i)
            dazu(t, 'kante', d[mk & kante])
            dazu(t, 'innen', d[mk & innen])
    g = gauss(A, 2.0)
    mos = np.abs(A - g).mean(-1)
    mos[~fig] = 0
    if texel is not None:
        _sp, gr, bl = block_bild(alb, kenn, texel)
    else:
        gr = bl = np.zeros(kenn.shape)
    out = {}
    for t, q in sums.items():
        ka = q.get('kante', [0, 0])
        ia = q.get('innen', [0, 0])
        out[t] = dict(kante=round(ka[0] / max(ka[1], 1), 1), innen=round(ia[0] / max(ia[1], 1), 1), kante_px=ka[1])
    for t in list(out):
        sel = np.zeros_like(fig)
        for i, tt in tmap.items():
            if tt == t:
                sel |= kenn == i
        out[t]['mosaik'] = round(float(mos[sel].mean()), 2) if sel.any() else None
        out[t]['px'] = int(sel.sum())
        out[t]['texel_px'] = round(float(gr[sel].mean()), 2) if sel.any() else None
        bs = sel & (bl > 0)
        out[t]['block'] = round(float(bl[bs].mean()), 1) if bs.any() else None
    ka = sum(q.get('kante', [0, 0])[0] for q in sums.values()); kn = sum(q.get('kante', [0, 0])[1] for q in sums.values())
    ia = sum(q.get('innen', [0, 0])[0] for q in sums.values()); inn = sum(q.get('innen', [0, 0])[1] for q in sums.values())
    out['_gesamt'] = dict(kante=round(ka / max(kn, 1), 1), innen=round(ia / max(inn, 1), 1), mosaik=round(float(mos[fig].mean()), 2) if fig.any() else None,
                          px=int(fig.sum()), texel_px=round(float(gr[fig].mean()), 2) if fig.any() else None,
                          block=round(float(bl[fig & (bl > 0)].mean()), 1) if (fig & (bl > 0)).any() else None)
    return out, kante_heat, bl


def farbskala(v, vmax):
    """0..vmax -> schwarz-blau-gelb-weiss."""
    t = np.clip(v / vmax, 0, 1)
    r = np.clip(3 * t - 1, 0, 1)
    g = np.clip(3 * t - 1.5, 0, 1) * 0.5 + np.clip(2 * t - 0.6, 0, 1) * 0.5
    b = np.clip(2 * t, 0, 1) * (1 - np.clip(3 * t - 2, 0, 1)) + np.clip(3 * t - 2, 0, 1)
    return (np.stack([r, g, b], -1) * 255).astype(np.uint8)


def heatbild(render, kenn, kante, mos):
    """Links: Render abgedunkelt mit Kantenspruengen (> 20) farbig; rechts: Mosaik-Karte."""
    from scipy import ndimage
    fig = kenn >= 0
    a = (render.astype(float) * 0.35).astype(np.uint8)
    k = ndimage.grey_dilation(kante, size=(2, 2))
    m = (k > 20) & ndimage.binary_dilation(fig, iterations=1)
    a[m] = farbskala(k[m], 90)[...]
    b = np.zeros_like(render)
    mm = ndimage.grey_dilation(mos, size=(3, 3))
    b[fig] = farbskala(mm[fig], 120)
    return a, b


def lade_satz(spec):
    """NAME=EMD,TIM[,naht.json] -> (name, emd bytes, tim bytes, naht oder None)."""
    name, rest = spec.split('=', 1)
    teile = rest.split(',')
    emd = open(teile[0], 'rb').read()
    tim = open(teile[1], 'rb').read()
    naht = json.load(open(teile[2], encoding='utf-8')) if len(teile) > 2 and teile[2] else None
    return name, emd, tim, naht


def lauf_netz(saetze):
    out = {}
    for spec in saetze:
        name, emd, tim, naht = lade_satz(spec)
        form = FE.lese_emd(emd)
        m, _ = netz_mass(form, Tex(tim), naht)
        out[name] = m
        print(name, json.dumps(m['_gesamt']), flush=True)
    return out


def lauf_bild(saetze, aus, json_aus=None, ansichten=None, ausschnitt=True):
    from PIL import Image, ImageDraw
    ansichten = ansichten or ANSICHTEN
    zeilen = []
    erg = {}
    for spec in saetze:
        name, emd, tim, naht = lade_satz(spec)
        form = FE.lese_emd(emd)
        tk = teil_karte(form, naht)
        erg[name] = {}
        reihe = []
        for (r, sch, lage, c, bi, g) in ansichten:
            ren, alb, kenn, texel = ansicht(emd, tim, r, sch, lage, c, bi, g)
            m, kh, mos = bild_mass(alb, kenn, tk, texel)
            erg[name]['%s_g%d' % (r, g)] = m
            hk, hm = heatbild(ren, kenn, kh, mos)
            reihe.append(((r, g), ren, hk, hm, kenn >= 0))
            print(name, r, g, json.dumps(m['_gesamt']), flush=True)
        zeilen.append((name, reihe))
    if json_aus:
        json.dump(erg, open(json_aus, 'w', encoding='utf-8'), indent=1)
    # Blatt: je Ansicht eine Zeile, je Satz drei Kacheln (Render | Kantenspruenge | Mosaik), auf die Figur zugeschnitten
    tafeln = []
    for ai in range(len(ansichten)):
        fig = np.zeros((480, 640), bool)
        for _n, reihe in zeilen:
            fig |= reihe[ai][4]
        if not fig.any():
            continue
        ys, xs = np.nonzero(fig)
        y0, y1 = max(ys.min() - 8, 0), min(ys.max() + 8, 480)
        x0, x1 = max(xs.min() - 8, 0), min(xs.max() + 8, 640)
        stuecke = []
        for _n, reihe in zeilen:
            _rg, ren, hk, hm, _f = reihe[ai]
            stuecke.append(np.hstack([ren[y0:y1, x0:x1], hk[y0:y1, x0:x1], hm[y0:y1, x0:x1]]))
        tafeln.append((ansichten[ai], stuecke))
    pad = 6
    Wt = max(sum(s.shape[1] for s in st) + pad * len(st) for _a, st in tafeln)
    Ht = 40 + sum(st[0].shape[0] + 22 for _a, st in tafeln)
    out = Image.new('RGB', (Wt, Ht), (18, 18, 22))
    d = ImageDraw.Draw(out)
    d.text((6, 4), 'Textur-Heatmap (Buehne, Spielkamera, Raumlicht): je Satz Render | Kantenspruenge ueber Polygonkanten (> 20, blau-gelb-weiss bis 90) | '
           'Texelbloecke (Sprung an Texelgrenzen x Texelgroesse px, bis 120)', fill=(240, 200, 120))
    d.text((6, 20), 'Saetze von links: ' + ' | '.join(n for n, _r in zeilen), fill=(200, 200, 200))
    y = 40
    for (r, sch, lage, c, bi, g), st in tafeln:
        d.text((6, y + 3), '%s Schnitt %d, Clip %d Bild %d, Gier %d' % (r, sch, c, bi, g), fill=(220, 220, 220))
        x = 0
        for s_ in st:
            out.paste(Image.fromarray(s_), (x, y + 20))
            x += s_.shape[1] + pad
        y += st[0].shape[0] + 22
    out.save(aus)
    print(aus, out.size)
    return erg


# ═══════════════════════════════════════════════════ Hautstoff: Quilting auf dem Torus (periodisch, nahtlos, nicht gespiegelt)
def saubere_quelle(Yrgb, maske, hell=35.0, bunt=35.0):
    """Quelltexel ohne Hell-/Wundflecken: Y gegen den oertlichen Median (9 x 9, nur Quelltexel) <= +hell, Buntheit (max - min) gegen den
    oertlichen Median <= +bunt. Yrgb = H x W x 3 (float), maske = Quelltexel. Rueckgabe sauber (H x W bool)."""
    from scipy import ndimage
    Y = Yrgb @ YK
    ch = Yrgb.max(-1) - Yrgb.min(-1)
    big = np.where(maske, Y, np.nan)
    medY = ndimage.generic_filter(big, np.nanmedian, size=9, mode='nearest')
    bigc = np.where(maske, ch, np.nan)
    medC = ndimage.generic_filter(bigc, np.nanmedian, size=9, mode='nearest')
    ok = maske & (Y - medY <= hell) & (ch - medC <= bunt)
    return ok


def kandidaten(rgb, sauber, B, ton=None, tol=6.0, mindest=40, schritt=1):
    """Alle B x B-Fenster ganz in `sauber` (Integralbild); optional mittleres Y im Fenster nahe `ton` (+- tol, notfalls weiter)."""
    H, W = sauber.shape
    I = np.pad(sauber.astype(np.int64), ((1, 0), (1, 0))).cumsum(0).cumsum(1)
    s = I[B:, B:] - I[:-B, B:] - I[B:, :-B] + I[:-B, :-B]
    ys, xs = np.nonzero(s[::schritt, ::schritt] == B * B)
    ys, xs = ys * schritt, xs * schritt
    if not len(ys):
        return None, None
    Y = rgb @ YK
    IY = np.pad(Y, ((1, 0), (1, 0))).cumsum(0).cumsum(1)
    my = (IY[ys + B, xs + B] - IY[ys, xs + B] - IY[ys + B, xs] + IY[ys, xs]) / (B * B)
    if ton is not None:
        t = tol
        while True:
            sel = np.abs(my - ton) <= t
            if sel.sum() >= mindest or t > 80:
                break
            t *= 1.5
        ys, xs = ys[sel], xs[sel]
    return ys, xs


def _pfad(e, achse):
    """Minimaler Schnitt durch die Fehlerflaeche e (h x w): achse 0 = Pfad von oben nach unten (je Zeile eine Spalte)."""
    if achse == 1:
        return _pfad(e.T, 0)
    h, w = e.shape
    E = e.copy()
    for y in range(1, h):
        for x in range(w):
            E[y, x] += min(E[y - 1, max(x - 1, 0):min(x + 2, w)])
    p = np.zeros(h, int)
    p[-1] = int(np.argmin(E[-1]))
    for y in range(h - 2, -1, -1):
        x = p[y + 1]
        lo = max(x - 1, 0)
        p[y] = lo + int(np.argmin(E[y, lo:min(x + 2, w)]))
    return p


def quilt_torus(idx_src, rgb_src, ys, xs, Pw, Ph, schritt, O, rng, vielfalt=None):
    """Periodische Kachel Ph x Pw (Indizes der Quelle, also nur vorhandene Farben) aus Bloecken B = schritt + O, Raster schritt, Schnitt
    mit minimalem Fehler an jedem schon gefuellten Rand (auch ueber den Umlauf: rechts/unten in der letzten Spalte/Zeile)."""
    B = schritt + O
    nx, ny = Pw // schritt, Ph // schritt
    assert nx * schritt == Pw and ny * schritt == Ph
    out_i = np.zeros((Ph, Pw), np.int64)
    out_c = np.zeros((Ph, Pw, 3))
    voll = np.zeros((Ph, Pw), bool)
    C = np.stack([rgb_src[y:y + B, x:x + B] for y, x in zip(ys, xs)])
    for j in range(ny):
        for i in range(nx):
            rr = (np.arange(B) + j * schritt) % Ph
            cc = (np.arange(B) + i * schritt) % Pw
            E = out_c[np.ix_(rr, cc)]
            M = voll[np.ix_(rr, cc)]
            if M.any():
                ssd = (((C - E[None]) ** 2).sum(-1) * M[None]).sum((1, 2))
                if vielfalt:
                    # Vielfalt (haut_stoff): unter den n besten Bloecken mit Fehler <= 1,3 x Minimum zufaellig, sonst wiederholt sich
                    # bei wenigen Quellbloecken ein Block sichtbar
                    ord_ = np.argsort(ssd)[:vielfalt]
                    gut = ord_[ssd[ord_] <= ssd.min() * 1.3 + 1e-9]
                else:
                    gut = np.nonzero(ssd <= ssd.min() * 1.1 + 1e-9)[0]
            else:
                gut = np.arange(len(C))
            k = int(rng.choice(gut))
            y0, x0 = ys[k], xs[k]
            blk_c = rgb_src[y0:y0 + B, x0:x0 + B]
            blk_i = idx_src[y0:y0 + B, x0:x0 + B]
            neu = np.ones((B, B), bool)
            if M.any():
                e = ((blk_c - E) ** 2).sum(-1)
                if M[:, :O].all():          # links gefuellt
                    p = _pfad(e[:, :O], 0)
                    for y in range(B):
                        neu[y, :p[y]] = False
                if M[:O, :].all():          # oben
                    p = _pfad(e[:O, :], 1)
                    for x in range(B):
                        neu[:p[x], x] = False
                if M[:, B - O:].all():      # rechts (Umlauf)
                    p = _pfad(e[:, B - O:], 0)
                    for y in range(B):
                        neu[y, B - O + p[y] + 1:] = False
                if M[B - O:, :].all():      # unten (Umlauf)
                    p = _pfad(e[B - O:, :], 1)
                    for x in range(B):
                        neu[B - O + p[x] + 1:, x] = False
            for a in range(B):
                for b_ in range(B):
                    if neu[a, b_] or not M[a, b_]:
                        out_i[rr[a], cc[b_]] = blk_i[a, b_]
                        out_c[rr[a], cc[b_]] = blk_c[a, b_]
                        voll[rr[a], cc[b_]] = True
    return out_i, out_c


def haut_stoff(tx, bau, ziel_wh, quelle=('huefte', 'wirbel', 'brust'), seed=7, glatt=10.0):
    """Nahtlose, fleckfreie H2-Hautstruktur fuer Seite 3 (Ersatz fuer k_bau.stoff, gleiche Signatur und Rueckgabe):
        tx      form_buehne.Textur(J-TIM)        bau  J_dicker/bau.json (Atlas)        ziel_wh (W, H)
    -> (struktur H x W x 3 float32, Mittel 1 je Kanal, nur Struktur; basis RGB = Mittel der sauberen Quelltexel).
    Quelle: die Rumpfinseln von J (huefte, wirbel, brust; CLUT-Zeile 0) ohne Hell-/Wundtexel (saubere_quelle). Synthese: Quilting auf dem
    Torus (periodisch in W und H, also ohne Naht am Umlauf der Zylinder-UV von Kopf/Kiefer; keine Spiegelung). Danach Grossform raus
    (Teilen durch Gauss sigma `glatt`, Umlauf), damit nur Koernung/Falten/Poren bleiben und der Kopf keine Fleckung aus der Brust erbt.
    W und H muessen Vielfache von 8 sein (Raster 8, Ueberlappung 3)."""
    from scipy import ndimage
    W, H = ziel_wh
    atl = {a['name']: a for a in bau['atlas']}
    Hh, Ww = tx.px.shape
    rgb = np.zeros((Hh, Ww, 3))
    msk = np.zeros((Hh, Ww), bool)
    for nm in quelle:
        x, y, w, h = atl[nm]['rechteck']
        z = atl[nm]['zeile']
        rgb[y:y + h, x:x + w] = tx.lut[z][tx.px[y:y + h, x:x + w]][..., :3]
        msk[y:y + h, x:x + w] = tx.px[y:y + h, x:x + w] != 0
    sauber = saubere_quelle(rgb, msk)
    # streng: nur der Hauptton der Haut (keine braun-gelben Wundraender, keine Hell-/Dunkelflecken): Buntheit <= p80 + 8,
    # Y zwischen p5 und Median + 30 der Quelle
    Y = rgb @ YK
    ch = rgb.max(-1) - rgb.min(-1)
    q = sauber
    sauber = q & (ch <= np.percentile(ch[q], 80) + 8) & (Y >= np.percentile(Y[q], 5)) & (Y <= np.median(Y[q]) + 30)
    schritt, O = 8, 3
    ys, xs = kandidaten(rgb, sauber, schritt + O, schritt=2)
    haut_stoff.kandidaten = len(ys)
    rng = np.random.RandomState(seed)
    Pw, Ph = ((W + schritt - 1) // schritt) * schritt, ((H + schritt - 1) // schritt) * schritt
    _i, c = quilt_torus(np.zeros(rgb.shape[:2], np.int64), rgb, ys, xs, Pw, Ph, schritt, O, rng, vielfalt=8)
    c = c[:H, :W]
    lo = np.stack([ndimage.gaussian_filter(c[..., k], glatt, mode='wrap') for k in range(3)], -1)
    st = c / np.maximum(lo, 1.0)
    st = st / np.maximum(st.reshape(-1, 3).mean(0), 1e-6)
    basis = rgb[sauber].mean(0)
    return st.astype(np.float32), basis


def main():
    a = argparse.ArgumentParser()
    a.add_argument('was', choices=('netz', 'bild', 'bauen'))
    a.add_argument('pfade', nargs='*')
    a.add_argument('--aus', default=None)
    a.add_argument('--json', default=None)
    g = a.parse_args()
    if g.was == 'netz':
        out = lauf_netz(g.pfade)
        txt = json.dumps(out, indent=1)
        if g.json:
            open(g.json, 'w', encoding='utf-8').write(txt)
        else:
            print(txt)
    elif g.was == 'bild':
        lauf_bild(g.pfade, g.aus, g.json)
    else:
        import s3_n2_textur_bau as TB
        TB.bauen(g.pfade[0], g.pfade[1])


if __name__ == '__main__':
    main()
