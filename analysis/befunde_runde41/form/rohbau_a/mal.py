#!/usr/bin/env python3
"""Rohbau-Textur: nur Materialzonen in Palettenfarben und die Zeichnung an ihrer Stelle. Malen kommt spaeter.
Zeichnung: Soll-Polygone in Bildkoordinaten werden ueber die UV-Abbildung des Spielzeichners (form_buehne.dreiecke, gleiche Tiefenfolge, affin wie die PSX) auf den Atlas gelegt."""
import json, os
import numpy as np, cv2
from kn import FORM
import form_emd as FE
import form_buehne as FB
import form_pruefung as PR

PAL = {m['name']: m for m in json.load(open(os.path.join(FORM, 'farbe_palette.json'), encoding='utf-8'))['materialien']}
H, W = 256, 384
KNOCHENFARBE = (84, 66, 62)           # Klauen, Zaehne: frei, dunkles Horn. Die Bilder zeigen an Haenden und Fuessen nichts Helles


def farbe(n):
    return tuple(PAL[n]['oberflaeche_rgb'])


FARBEN = {'grund': farbe('grundhaut_dunkel'), 'mittel': farbe('rotbraun_mittelton'), 'rosa_stelle': farbe('rosa_stelle'), 'helle_baender': farbe('helle_baender'), 'feine_linien': farbe('feine_linien'),
          'weisskerne': (205, 225, 225), 'untere_streifen': (176, 150, 168), 'rotbraun_mittelton': farbe('rotbraun_mittelton'), 'z9': (85, 80, 60), 'x_rot': (131, 95, 104)}


# Bild 1: Lampe von oben, Filter hebt helle Stellen stark. Unteres Ende der Palette, sonst laufen Flaechen weiss zu (pruefung.md Abschnitt 6, Warnung).
FARBEN_ZEICHNUNG = {1: {'rosa_stelle': tuple(PAL['rosa_stelle']['oberflaeche_von']), 'helle_baender': tuple(PAL['helle_baender']['oberflaeche_von']), 'feine_linien': tuple(PAL['feine_linien']['oberflaeche_von'])},
                    2: {}}


def nach_tim(rgb):
    """5-Bit-Farben direkt in die CLUT, Index 0 bleibt frei (transparent). Mehr als 255 Farben: Medianschnitt."""
    a = (np.asarray(rgb, np.uint8) >> 3).astype(np.int32)
    k = a[..., 0] | (a[..., 1] << 5) | (a[..., 2] << 10)
    w, inv = np.unique(k, return_inverse=True)
    inv = inv.reshape(k.shape)
    if len(w) > 255:
        return FE.tim_aus_rgb(rgb, None, 255)
    c = [0] * 256
    for i, x in enumerate(w):
        c[i + 1] = int(x) if x else 0x8000
    return (inv + 1).astype(np.uint8), c


def uv_render(tris, abbildung):
    """Je Bildpixel Texel (u, v), gleiche Reihenfolge und Abtastung wie form_buehne.rastern."""
    U = np.full((480, 640), -1, np.int32)
    V = np.full((480, 640), -1, np.int32)
    for _t, p, uv, _z, _f in sorted(tris, key=lambda t: -t[0]):
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), abbildung)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, 640)
        y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, 480)
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
        uu = (w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(np.int32)
        vv = (w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(np.int32)
        U[y0:y1, x0:x1][innen] = uu[innen]
        V[y0:y1, x0:x1][innen] = vv[innen]
    return U, V


def zeichnung_liste():
    """Stellen aus form_pruefung (Z1-Z9, Q1-Q5, G, P) plus die uebrigen benannten Stellen aus FORMBLATT2 Abschnitt 6 (A05, A06, A10, B10, B11, B12), Material aus farbe_palette."""
    aus = []
    for s in PR.stellen():
        aus.append((s['bild'], s['polygone'], s['material'], s['id']))
    M = PR.merkmale()
    pm = {x['id']: x.get('material') for x in json.load(open(os.path.join(FORM, 'farbe_palette.json'), encoding='utf-8'))['merkmale']}
    for i in ('A05', 'A06', 'A10', 'A35', 'A25', 'A26', 'B10', 'B11', 'B04', 'B02', 'B15'):
        if i in M and pm.get(i):
            aus.append((M[i].get('bild', 1 if i[0] == 'A' else 2), [M[i]['polygon']], pm[i], i))
    aus.append((1, [[(252, 424), (286, 424), (290, 479), (246, 479)]], 'x_rot', 'X'))       # Schwanzende, FORMBLATT2 Abschnitt 1: rosa (250-288,420-479)
    return aus


def grundzonen(rgb, N, streifen):
    """Materialzonen je Teil: Klauen, Zaehne, Hauer Knochen; Schwanzende rosa; erhabene Flaechen Mittelton, Rest Grundhaut."""
    for m in N:
        s = streifen[m['name']]
        t = m['teil']
        x0, y0, w, h = s['x'], s['y'], s['w'], s['h']
        if t.mat == 'klaue':
            rgb[y0:y0 + h, x0:x0 + w + 1] = KNOCHENFARBE
            continue
        if t.mat == 'rosa':
            continue            # rosa Stelle kommt als Zeichnung X aus dem Bild
        R = t.R / np.maximum(1.0, t.R.mean(1, keepdims=True))
        nr, n = R.shape
        vv = s['v']
        for i in range(nr - 1):
            for j in range(n):
                e = 0.25 * (R[i, j] + R[i, (j + 1) % n] + R[i + 1, j] + R[i + 1, (j + 1) % n])
                if e > 1.10 and t.name.startswith(('huefte', 'wirbel', 'brust', 'oberarm', 'oberschenkel', 'hals')):
                    xa, xb = int(x0 + w * j / n), int(x0 + w * (j + 1) / n)
                    ya, yb = int(y0 + (h - 1) * vv[i]), int(y0 + (h - 1) * vv[i + 1])
                    rgb[ya:yb + 1, xa:xb + 1] = (62, 32, 42)      # Muskelbauch: helles Ende der Grundhaut, Palette oberflaeche_bis


def textur(N, ks, form, posen, streifen, karte, ohne_zeichnung):
    rgb = np.zeros((H, W, 3), np.uint8)
    rgb[:] = FARBEN['grund']
    grundzonen(rgb, N, streifen)
    if not ohne_zeichnung:
        emd = FE.schreibe_emd(form, pruefen=False)
        px0, cl0 = nach_tim(rgb)
        tim0 = FE.tim_schreiben(px0, [cl0])
        mo = FE.modell(emd, tim0)
        for n in (1, 2):
            p = posen['bild%d' % n]
            kam = PR.kamera(n)
            f0, _k = mo.clips[n - 1]
            kf = mo.frames[f0] & 0xFFF
            tr = FB.dreiecke(mo, kf, tuple(int(c) for c in p['lage']), int(p['gier']), kam, None)
            U, V = uv_render(tr, kam.abbildung)
            for bild, polys, mat, _id in zeichnung_liste():
                if bild != n:
                    continue
                m = np.zeros((480, 640), np.uint8)
                for q in polys:
                    cv2.fillPoly(m, [np.round(np.array(q, float)).astype(np.int32)], 1)
                m = (m > 0) & (V >= 8)        # Zeilen 0-7: Fleck der Kappen, bleibt Grundhaut
                f = FARBEN_ZEICHNUNG[n].get(mat, FARBEN[mat])
                uu, vv = U[m], V[m]
                for du in (0, 1):
                    for dv in (0, 1):
                        rgb[np.clip(vv + dv, 0, H - 1), np.clip(uu + du, 0, W - 1)] = f
    atlas_blatt(rgb, streifen)
    return nach_tim(rgb)


def atlas_blatt(rgb, streifen):
    """textur.png: Atlas wie im TIM. atlas_karte.png: 3-fach mit Streifen und Namen, Grundlage fuer das Malen."""
    from kn import HIER
    cv2.imwrite(os.path.join(HIER, 'textur.png'), rgb[..., ::-1])
    b = cv2.resize(rgb[..., ::-1], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
    for nm, s in streifen.items():
        p0 = (s['x'] * 3, s['y'] * 3)
        p1 = ((s['x'] + s['w']) * 3, (s['y'] + s['h']) * 3)
        cv2.rectangle(b, p0, p1, (0, 200, 255), 1)
        if s['h'] >= 6:
            cv2.putText(b, nm, (p0[0] + 2, p0[1] + 10), 0, 0.3, (255, 255, 255), 1, cv2.LINE_AA)
    cv2.imwrite(os.path.join(HIER, 'atlas_karte.png'), b)
