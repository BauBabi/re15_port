# -*- coding: utf-8 -*-
"""Rohbau B: Generator. figur.py -> FORM.EMD, FORM.TIM, form.json.
python bau.py [--pruefe TAG]   baut, schreibt, und ruft mit --pruefe form_pruefung.py pruefe auf (Ausgabe lauf/TAG)."""
import argparse, json, math, os, subprocess, sys
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import schau  # noqa: E402
import figur  # noqa: E402
from schau import FK, WURZEL  # noqa: E402
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_emd as FE  # noqa: E402
from ringe import ecken_normalen, aussen_drehen  # noqa: E402

FORM = schau.FORM
TOOLS = os.path.join(WURZEL, 're15_port', 'tools')
TEX_B, TEX_H = 384, 256
PAL = {m['name']: tuple(m['oberflaeche_rgb']) for m in json.load(open(os.path.join(FORM, 'farbe_palette.json'), encoding='utf-8'))['materialien']}


# ═════════════════════════════ Karten
def karte_von(t):
    """Masse einer Karte: x[0..n] Spalten, y[0..m+1] Zeilen, je 0..1, dazu Umfang und Laenge in Einheiten."""
    n = t.n
    R = np.array(t.ringe)
    kant = np.linalg.norm(R - np.roll(R, -1, axis=1), axis=2).mean(0)
    z = [np.repeat(t.pol_a[None], n, 0)] + [r for r in R] + [np.repeat(t.pol_e[None], n, 0)]
    sg = np.array([float(np.linalg.norm(z[i + 1] - z[i], axis=1).mean()) for i in range(len(z) - 1)])
    sg = np.maximum(sg, 18.0)
    w = kant
    x = np.concatenate([[0.0], np.cumsum(w)])
    y = np.concatenate([[0.0], np.cumsum(sg)])
    return dict(name=t.karte, um=float(x[-1]), lg=float(y[-1]), x=x / x[-1], y=y / y[-1], res=t.res)


def packen(karten, b=TEX_B, h=TEX_H, rand=2):
    """Skyline, Massstab (Einheiten je Texel) per Bisektion."""
    lo, hi = 4.0, 80.0
    pos = None

    def versuche(ein):
        L = sorted(range(len(karten)), key=lambda i: -karten[i]['lg'] / karten[i]['res'])
        sky = np.full(b, rand, int)
        p = {}
        for i in L:
            k = karten[i]
            w = max(int(math.ceil(k['um'] / (ein * k['res']))) + 1, 10)
            hh = max(int(math.ceil(k['lg'] / (ein * k['res']))) + 1, 8)
            if w + 2 * rand > b:
                return None
            best = None
            for x in range(rand, b - w - rand + 1, 2):
                y = int(sky[x - rand:x + w + rand].max())
                if best is None or y < best[0]:
                    best = (y, x)
            y, x = best
            if y + hh + rand > h:
                return None
            p[i] = (x, y, w, hh)
            sky[x - rand:x + w + rand] = y + hh + rand
        return p
    for _ in range(24):
        m = .5 * (lo + hi)
        p = versuche(m)
        if p is None:
            lo = m
        else:
            hi, pos = m, p
    for i, k in enumerate(karten):
        k['px'], k['py'], k['w'], k['h'] = pos[i]
    return hi


def _ip(v, s):
    i = int(math.floor(s))
    return v[i] if s == i else v[i] * (1 - (s - i)) + v[i + 1] * (s - i)


def uv(k, zeile, spalte):
    return (k['px'] + _ip(k['x'], spalte) * (k['w'] - 1), k['py'] + _ip(k['y'], zeile) * (k['h'] - 1))


def raster_uv(t):
    """Vertex -> (Zeile, Spalte) je Flaeche, in Reihenfolge der Flaechen aus Teil.netz()."""
    n, m = t.n, len(t.ringe)
    pa, pe = m * n, m * n + 1
    V, F, G = t.netz()
    aus = []
    for f, g in zip(F, G):
        i, k = g
        k2 = (k + 1) % n
        if len(f) == 4:
            d = {i * n + k: (i + 1, k), i * n + k2: (i + 1, k + 1), (i + 1) * n + k: (i + 2, k), (i + 1) * n + k2: (i + 2, k + 1)}
        elif i < 0:
            d = {pa: (0, k + .5), k: (1, k), k2: (1, k + 1)}
        else:
            d = {pe: (m + 1, k + .5), (m - 1) * n + k: (m, k), (m - 1) * n + k2: (m, k + 1)}
        aus.append(d)
    return aus


# ═════════════════════════════ Form
NORM_U0, NORM_G = .03, .75
# Einheiten je Texel relativ: Verdecktes und Kleines groeber, damit Rumpf, Schwanz, Arme und Beine mehr Texel bekommen
RES = {'wirbel': 1.7, 'hals': 2.2, 'zeh': 1.6, 'wange': 2.0, 'zahn': 1.6, 'schuppe': 1.6, 'finger': 1.2, 'fuss_l': 1.4, 'fuss_r': 1.4, 'oberarm_l': 1.5, 'kiefer': 1.3, 'hand_l': 1.2, 'hand_r': 1.2}


def licht_normalen(N, oben):
    """Disc-Licht von ROOM5080 steht fast senkrecht ueber der Figur: Oberseiten liefen hell, Seiten dunkel, im Mittel Faktor 0,63 statt gemessen 0,49. Hoehenanteil der Normalen gestaucht, die Richtung in der Waagerechten bleibt.
    In ROOM5090 (nur Ambient) wirkt das nicht. oben = Hochachse im Knochenraum, aus Pose 1."""
    u = N @ oben
    h = N - np.outer(u, oben)
    lg = np.linalg.norm(h, axis=1, keepdims=True)
    h = np.where(lg > 1e-6, h / np.maximum(lg, 1e-6), np.cross(oben, (1, 0, 0)) / max(np.linalg.norm(np.cross(oben, (1, 0, 0))), 1e-6))
    u2 = np.clip(NORM_U0 + NORM_G * u, -.98, .98)
    return np.outer(u2, oben) + h * np.sqrt(1 - u2 * u2)[:, None]


def form_bauen(f, fig):
    form = FE.neu()
    p1 = fig['posen']['bild1']
    R1, _T1 = schau.fk(f.eltern, f.versatz, p1['wurzel'], p1['winkel'])
    oben_lokal = [R.T @ np.array((0, -1.0, 0)) for R in R1]
    for nm, el, ve in zip(f.namen, f.eltern, f.versatz):
        FE.knochen_dazu(form, nm, el, [int(round(c)) for c in ve])
    karten, nach_name = [], {}
    for t in f.teile:
        t.auto_pole()
        t.karte = getattr(t, 'karte', None) or t.name
        t.res = RES.get(t.karte, 1.0)
        if t.karte not in nach_name:
            k = karte_von(t)
            nach_name[t.karte] = k
            karten.append(k)
    ein = packen(karten)
    print('Atlas: %.1f Einheiten je Texel, %d Karten' % (ein, len(karten)))
    flaechen = []     # (Teil, V, f, uv je Ecke) fuers Backen
    for ti, t in enumerate(f.teile):
        k = nach_name[t.karte]
        V, F0, G = t.netz()
        ruv = raster_uv(t)
        F = aussen_drehen(V, F0, G, t)
        gedreht = F[0] != F0[0]
        Nz = ecken_normalen(V, F, getattr(t, 'knick', 46.0))
        Nz = [licht_normalen(n, oben_lokal[t.knochen]) for n in Nz]
        m = form['meshes'][t.knochen]
        emap, nmap = {}, {}
        for j, ff in enumerate(F):
            d = ruv[j]
            e, nn, tx = [], [], []
            uvs = []
            for c, vi in enumerate(ff):
                if vi not in emap:
                    emap[vi] = FE.ecke(m, V[vi])
                sch = tuple(np.round(Nz[j][c] * 4096).astype(int))
                if sch not in nmap:
                    nmap[sch] = FE.normale(m, Nz[j][c])
                e.append(emap[vi])
                nn.append(nmap[sch])
                q = uv(k, *d[vi])
                uvs.append(q)
                tx.append((int(round(q[0])), int(round(q[1]))))
            flaechen.append((ti, V[list(ff)], np.array(uvs), Nz[j], t.tag_flaeche(G[j]) if hasattr(t, 'tag_flaeche') else None))
            if len(ff) == 3:
                FE.dreieck(m, e, nn, tx, 0)
            else:
                z = (0, 1, 3, 2)
                FE.viereck(m, [e[i] for i in z], [nn[i] for i in z], [tx[i] for i in z], 0)
    for nm, p in fig['posen'].items():
        FE.clip_dazu(form, nm, [FE.bild(form, wurzel=tuple(p['wurzel']), winkel=p['winkel'])])
    FE.orientiere(form)
    return form, karten, flaechen, ein


# ═════════════════════════════ Backen
def rastern_uv(f, flaechen):
    """Je Texel Punkt im Knochenraum, Normale, Teil-Nummer."""
    P = np.zeros((TEX_H, TEX_B, 3), np.float32)
    N = np.zeros((TEX_H, TEX_B, 3), np.float32)
    ID = np.full((TEX_H, TEX_B), -1, np.int16)
    for ti, V, q, Nz, _tag in flaechen:
        ecken = range(1, len(V) - 1)
        for i in ecken:
            a, b, c = 0, i, i + 1
            u = q[[a, b, c]]
            x0, y0 = max(int(u[:, 0].min()) - 1, 0), max(int(u[:, 1].min()) - 1, 0)
            x1, y1 = min(int(math.ceil(u[:, 0].max())) + 2, TEX_B), min(int(math.ceil(u[:, 1].max())) + 2, TEX_H)
            if x1 <= x0 or y1 <= y0:
                continue
            XX, YY = np.meshgrid(np.arange(x0, x1, dtype=np.float32), np.arange(y0, y1, dtype=np.float32))
            fl = (u[1, 0] - u[0, 0]) * (u[2, 1] - u[0, 1]) - (u[2, 0] - u[0, 0]) * (u[1, 1] - u[0, 1])
            if abs(fl) < 1e-9:
                continue
            w1 = ((XX - u[0, 0]) * (u[2, 1] - u[0, 1]) - (u[2, 0] - u[0, 0]) * (YY - u[0, 1])) / fl
            w2 = ((u[1, 0] - u[0, 0]) * (YY - u[0, 1]) - (XX - u[0, 0]) * (u[1, 1] - u[0, 1])) / fl
            w0 = 1 - w1 - w2
            e = -0.6 / max(abs(fl) ** .5, 1.0)
            m = (w0 >= e) & (w1 >= e) & (w2 >= e)
            if not m.any():
                continue
            pa, na = V[[a, b, c]], Nz[[a, b, c]]
            s = (slice(y0, y1), slice(x0, x1))
            P[s][m] = (w0[..., None] * pa[0] + w1[..., None] * pa[1] + w2[..., None] * pa[2])[m]
            N[s][m] = (w0[..., None] * na[0] + w1[..., None] * na[1] + w2[..., None] * na[2])[m]
            ID[s][m] = ti
    return P, N, ID


def z_puffer(fig, pose, kam):
    Z = np.full((480, 640), np.inf, np.float32)
    for b, W, F in schau.welt_netz(fig, pose):
        p = FK.projiziere(kam, W)
        d = FK.tiefe(kam, W)
        for ff in F:
            for i in range(1, len(ff) - 1):
                tri = (ff[0], ff[i], ff[i + 1])
                q = p[list(tri)]
                if not np.isfinite(q).all():
                    continue
                s = (q[1, 0] - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (q[1, 1] - q[0, 1])
                if s >= 0:
                    continue
                x0, x1 = max(int(q[:, 0].min()), 0), min(int(q[:, 0].max()) + 2, 640)
                y0, y1 = max(int(q[:, 1].min()), 0), min(int(q[:, 1].max()) + 2, 480)
                if x1 <= x0 or y1 <= y0:
                    continue
                XX, YY = np.meshgrid(np.arange(x0, x1, dtype=np.float32), np.arange(y0, y1, dtype=np.float32))
                w1 = ((XX - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (YY - q[0, 1])) / s
                w2 = ((q[1, 0] - q[0, 0]) * (YY - q[0, 1]) - (XX - q[0, 0]) * (q[1, 1] - q[0, 1])) / s
                w0 = 1 - w1 - w2
                m = (w0 >= -.02) & (w1 >= -.02) & (w2 >= -.02)
                dz = w0 * d[tri[0]] + w1 * d[tri[1]] + w2 * d[tri[2]]
                zz = Z[y0:y1, x0:x1]
                u = m & (dz < zz)
                zz[u] = dz[u]
    return Z


SICHT_SPIEL = 260.0     # Das Spiel sortiert Flaechen, es hat keinen Tiefenpuffer: dicht uebereinander liegende Teile bekommen die Zeichnung beide


def pixel_der_texel(f, fig, P, ID, n, N=None):
    """Je Texel: Bildpunkt und Tiefe in Pose n, sichtbar ja/nein."""
    pose = fig['posen']['bild%d' % n]
    kam = FK.lade_kamera(n)
    R, T = schau.fk(fig['eltern'], fig['versatz'], pose['wurzel'], pose['winkel'])
    Y = schau.ry(pose['gier'] * schau.E)
    la = np.array(pose['lage'], float)
    Z = z_puffer(fig, pose, kam)
    px = np.full(ID.shape + (2,), -1, np.int32)
    sicht = np.zeros(ID.shape, bool)
    for ti, t in enumerate(f.teile):
        m = ID == ti
        if not m.any():
            continue
        b = t.knochen
        W = (P[m].astype(np.float64) @ R[b].T + T[b]) @ Y.T + la
        q = FK.projiziere(kam, W)
        d = FK.tiefe(kam, W)
        x = np.round(np.nan_to_num(q[:, 0], nan=-1)).astype(int)
        y = np.round(np.nan_to_num(q[:, 1], nan=-1)).astype(int)
        ok = (x >= 0) & (x < 640) & (y >= 0) & (y < 480)
        s = np.zeros(len(x), bool)
        s[ok] = d[ok] <= Z[y[ok], x[ok]] + SICHT_SPIEL
        if N is not None:                   # Rueckseiten bekommen keine Zeichnung, auch nicht im Spielraum der Tiefe
            Nw = (N[m].astype(np.float64) @ R[b].T) @ Y.T
            s &= ((W - np.asarray(kam['auge'], float)) * Nw).sum(1) < 0
        px[m] = np.stack([x, y], 1)
        sicht[m] = s
    return px, sicht


def tim_genau(rgb):
    """Bild -> 8 bpp, Stufen bleiben exakt, wenn es hoechstens 255 Farben sind."""
    q = np.clip(np.round(rgb.astype(np.float32) / 8.0).astype(np.int32), 0, 31)
    code = q[..., 0] | (q[..., 1] << 5) | (q[..., 2] << 10)
    u, inv, zahl = np.unique(code, return_inverse=True, return_counts=True)
    inv = inv.reshape(code.shape)
    if len(u) > 255:
        rang = np.argsort(-zahl)
        halten = rang[:255]
        fb = np.stack([u & 31, (u >> 5) & 31, (u >> 10) & 31], 1).astype(float)
        ziel = np.arange(len(u))
        for i in rang[255:]:
            ziel[i] = halten[int(((fb[halten] - fb[i]) ** 2).sum(1).argmin())]
        neu = np.zeros(len(u), int)
        for j, a in enumerate(halten):
            neu[a] = j
        inv = neu[ziel][inv]
        cl = [fb[a] for a in halten]
    else:
        cl = [np.array([v & 31, (v >> 5) & 31, (v >> 10) & 31], float) for v in u]
    px = (inv + 1).astype(np.uint8)
    farben = [(0, 0, 0)] + [tuple(int(c) << 3 for c in x) for x in cl]
    return px, FE.clut_aus_rgb(farben, transparent0=True), len(cl)


def schreiben(f, fig, tag=None):
    form, karten, flaechen, ein = form_bauen(f, fig)
    FE.boden_nachfuehren(form, 0)
    import zeichnung
    rgb = zeichnung.textur(f, fig, form, flaechen, karten, sys.modules[__name__])
    emd = FE.schreibe_emd(form)
    px, clut, nf = tim_genau(rgb)
    tim = FE.tim_schreiben(px, [clut])
    open(os.path.join(HIER, 'FORM.EMD'), 'wb').write(emd)
    open(os.path.join(HIER, 'FORM.TIM'), 'wb').write(tim)
    cv2.imwrite(os.path.join(HIER, 'textur.png'), cv2.resize(rgb[..., ::-1], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))
    json.dump(dict(texel_einheit=round(ein, 1), karten=[dict(name=k['name'], x=k['px'], y=k['py'], b=k['w'], h=k['h'], umfang=round(k['um']), laenge=round(k['lg'])) for k in karten]),
              open(os.path.join(HIER, 'zonen.json'), 'w', encoding='utf-8'), indent=0)
    p = fig['posen']
    teile = {}
    for nz in fig['zuordnung'].values():
        for k, v in nz.items():
            teile.setdefault(k, [])
            teile[k] += [b for b in v if b not in teile[k]]
    j = dict(name='rohbau_b', emd='FORM.EMD', tim='FORM.TIM', bild1=dict(lage=p['bild1']['lage'], gier=p['bild1']['gier'], clip=0, bild=0),
             bild2=dict(lage=p['bild2']['lage'], gier=p['bild2']['gier'], clip=1, bild=0), ruhe=dict(clip=2, bild=0), teile=teile)
    json.dump(j, open(os.path.join(HIER, 'form.json'), 'w', encoding='utf-8'), indent=1)
    F = sum(len(m['dreiecke']) + len(m['vierecke']) for m in form['meshes'])
    print('geschrieben: %d Flaechen, %d Knochen, %d Farben, Textur %dx%d, Boden %s' % (F, len(form['knochen']), nf, rgb.shape[1], rgb.shape[0], [round(FE.tiefster_punkt(form, c['bilder'][0]), 1) for c in form['clips']]))
    return form


def pruefen(tag):
    aus = os.path.join(HIER, 'lauf', tag)
    os.makedirs(aus, exist_ok=True)
    env = dict(os.environ, PYTHONIOENCODING='utf-8')
    r = subprocess.run([sys.executable, os.path.join(TOOLS, 'form_pruefung.py'), 'pruefe', os.path.join(HIER, 'form.json'), '--aus', aus], capture_output=True, text=True, env=env, encoding='utf-8', errors='replace')
    if r.returncode:
        print(r.stdout[-3000:], r.stderr[-3000:])
    return aus


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--pruefe')
    a = a.parse_args()
    f = figur.bauen()
    fig = figur.fertig(f)
    form = schreiben(f, fig)
    schau.alles(fig)
    if a.pruefe:
        print(pruefen(a.pruefe))
