"""SF V8 Runde 8: Schwanz-Riegel R8a-R8f fuer die Stufe q (schwanz_r8.py). Gegenprobe: Stand 7e 07b6703f muss in R8a-R8e ROT sein.

    python riegel_schwanz_r8.py [EMD] [--farbe a|b|c] [--aus TXT] [--json JSON]      Exit 0 = alle gruen

Nutzer 2026-11-14 (woertlich): "weder mit dem rot der Schwanzfarbe noch mit dem etwas zu steifen grossen Schwanz zufrieden" + Version_80%/Areas*.png.
Messziele aus dem Auftrag Runde 8 und der Analyse scratchpad/schwanz_v8/analyse/ANALYSE.md (Bauplan Schritte 1, 2, 4); Daten der Analyse (Ketten der
Szene, Vorhersage der Rollreihe, Nutzer-Umriss) in riegel_schwanz_r8.json. Kein RE-Befund, keine @0x-Konstante (Sichel-Form auf keiner Disc).

R8a Breite vorn   Rollreihe der Analyse (roll_render/teil_b) am GEBAUTEN Asset: echte Meshes 16-22 entlang der Szenenketten h22 / h24 / h64 (Bauchachse
                  zur Kamera, phi 0), Breite quer zur Kette an s .3 / .5 / .7 je 15-20 px (Auftrag "vorn 15-20 px breit", +-0,5 px Messschritt) und
                  |neu - Vorhersage der Analyse (flach, Querdicke 280)| <= 4 px; s .1 / .9 (Wurzel, Knolle) nur Angabe.
R8b Seite (PV)    Vorlage-Pose PV (Clip 17 B47 Gier 587, Keyframe-Rolle wie im Spiel), f_rast nur Knochen 16-22: s .3 >= 36 px; s .5 15-20 % duenner als
                  der Bezug 07b6703f (+-0,5 px Messschritt); s .5 / .7 / .9 gegen den Nutzer-Umriss h90 (Wurzel = Schnittkante: 23,5 / 18,5 / 15,
                  Messziel Bauplan 2) +- 4 px; mit der oberen Ecke als Wurzel (27 / 19,5 / 14,5) nur Angabe.
R8c Laenge/Spitze PV: Bildlaenge Kette + Nadel >= Bezug + 15 px (Ziel der Analyse ~190, mit der Nadel 450-550 E. erreichbar ~176-179); Nadelspitze
                  <= 4 px vom Zielkasten x 286-289 / y 349-356; Nadel auf der Gliedachse (groesste Ecke von Mesh 22: x = z = 0, y = 541 + 48 + 550).
R8d Fugen         (1) Geometrie: an allen 6 Gelenken Hochachse von Endring und Manschette / Huellradius am Gelenk (kette_v8.json x MITTE) <= 0,88;
                  (2) sichtbar von vorn: in der Rollreihe h22 / h24 / h64 ist die Breite an den sichtbaren Gelenken im Mittel >= 1,0 px schmaler als
                  in den Gliedmitten daneben; (3) Farbprobe b / c: an allen 6 Gelenken Flaechen auf Zeile 494 (helles Band) und in PV sichtbare
                  Band-Bildpunkte an >= 5 Gelenken; Farbprobe a: ohne Band (Angabe).
R8e Fleisch nur   (1) jede Flaeche auf Zeile 495 / 494 (ausser den 14 Wurzelflaechen von Mesh 16 im Rumpf, Stufe w/u) liegt mit allen Ring-Ecken
    Bauchseite    im Bauchsektor |theta| <= Grenze(u) + TOL_GRAD (Grenze 90 -> 60, schwanz_r8.grenze; theta = Parameterwinkel der Ellipse des Rings),
                  keine Flaeche auf 487 ganz im Bauchsektor (ausser der Nadel ab y 560, die ganz grau ist); (2) PV: Anteil Fleisch-Zeile an den Schwanz-Bildpunkten mit Normalen-|theta| >= 100 <= 1 %, mit |theta| <= 40 >= 50 %;
                  (3) Herkunft: Texel unter 495 / 494 = Texel, die Original-Flaechen der Original-Zeile 482 / 481 benutzen; Texel unter 487 = Texel
                  der Oberseite (487) der Meshes 16-22 im Eingang 07b6703f; Asset-Texel dort bytegleich Original bzw. Eingang.
R8f Bytegleich    Sektionen 0-6 (Skelett, Clips, Keyframes), TIM (Texturbild, 16 CLUT-Zeilen) und Meshes 0-15 (Mesh 9 = V7-Kopf) = Eingang 07b6703f;
                  Gliedlaengen 17-22 = (0, 541, 0) (re15_schwanz_r41_v8_kette).
"""
import os
import sys
import json
import argparse
import subprocess
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import schwanz_r8 as Q                # noqa: E402

EINGANG = 'b9c8c5bf6'                 # Commit des Stands 7e (Spiel-Asset 07b6703f)
EINGANG_SHA = '07b6703f15879bbeedd96eff7c647afc3ad989cad41b1423d1d24927265cbb95'
DATEN = os.path.join(HIER, 'riegel_schwanz_r8.json')
S = (0.1, 0.3, 0.5, 0.7, 0.9)
ZIEL_SPITZE = ((286.0, 289.0), (349.0, 356.0))
WURZEL_495 = 14                       # Wurzelflaechen von Mesh 16 auf 495 (Kegel/Rohr im Rumpf, Stufe w/u, Indizes 0-21 / 34, UV wie 07b6703f)
TOL_GRAD = 2.5                        # R8e (1): Toleranz der Sektorpruefung. Der Parameterwinkel einer Ring-Ecke wird aus Mitte und Halbachsen der
                                      # Ring-Ausdehnung geschaetzt (Gehrungsebene, Manschette zum Vorglied verschoben); an den Grenz-Ecken des Baus
                                      # gemessen bis 1,4 Grad daneben. Die naechste Ruecken-Ecke liegt >= 25 Grad hinter der Grenze.


def eingang():
    p = os.path.join(V.SFV8, 'emd', 'ALT_07b6703f.EMD')
    if not os.path.exists(p) or V.sha(open(p, 'rb').read()) != EINGANG_SHA:
        os.makedirs(os.path.dirname(p), exist_ok=True)
        b = subprocess.run(['git', '-C', HIER, 'show', '%s:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD' % EINGANG], capture_output=True).stdout
        open(p, 'wb').write(b)
    b = open(p, 'rb').read()
    assert V.sha(b) == EINGANG_SHA
    return b


# ------------------------------------------------------------------ Rollreihe (wie roll_render.py / kam.py der Analyse)
class Kam:
    def __init__(self, k):
        POS, ZIEL = np.array(k['pos'], float), np.array(k['ziel'], float)
        d = ZIEL - POS; dist = np.linalg.norm(d); hz = np.hypot(d[0], d[2])
        sp, cp, sy, cy = -d[1] / dist, hz / dist, d[0] / hz, d[2] / hz
        self.R = np.array([[cy, 0, -sy], [sp * sy, cp, sp * cy], [cp * sy, -sp, cp * cy]])
        self.T = self.R @ (-POS); self.H = k['h']; self.POS = POS; self.v = k['psx2vid']

    def proj(self, w):
        v = (self.R @ np.asarray(w, float).T).T + self.T
        x = 160 + self.H * v[:, 0] / v[:, 2]; y = 120 + self.H * v[:, 1] / v[:, 2]
        return np.stack([self.v[0] * x + self.v[1], self.v[2] * y + self.v[3]], 1), v[:, 2]


def _n(v):
    n = np.linalg.norm(v)
    return v / n if n > 1e-9 else v


def meshes_gedreht(mo):
    """Meshes 16-22 (Ecken, Dreiecke) mit Bauchachse e_h -> lokale x und e_q -> lokale z (wie die Rollreihe der Analyse)."""
    phi = {int(k): float(v) for k, v in json.load(open(os.path.join(HIER, 'unterseite_v8.json')))['innen_phi'].items()}
    out = {}
    for b in range(16, 23):
        m = mo.meshes[b]
        P = np.array([v[:3] for v in m.tv] + [v[:3] for v in m.qv], float)
        p = np.radians(phi[b])
        P = np.stack([P[:, 0] * np.cos(p) + P[:, 2] * np.sin(p), P[:, 1], -P[:, 0] * np.sin(p) + P[:, 2] * np.cos(p)], 1)
        nt = len(m.tv)
        F = [(f[1], f[3], f[5]) for f in m.tf]
        for f in m.qf:
            a, bb, c, d = nt + f[1], nt + f[3], nt + f[5], nt + f[7]
            F += [(a, bb, c), (bb, d, c)]
        out[b] = (P, np.array(F))
    return out


def kette_maske(M, P, phi, kam):
    """Maske 480 x 640 der Meshes entlang der Kette P (8 Gelenke, Welt), Hochachse unter phi (Grad) zum Sehstrahl (0 = Bauch zur Kamera)."""
    mask = np.zeros((480, 640), bool)
    for j in range(7):
        Vv, F = M[16 + j]
        t = _n(P[j + 1] - P[j]); mid = (P[j] + P[j + 1]) / 2; v = _n(mid - kam.POS)
        e1 = _n(v - (v @ t) * t); e2 = _n(np.cross(t, e1))
        u = np.cos(np.radians(phi)) * e1 + np.sin(np.radians(phi)) * e2
        w = _n(np.cross(t, u))
        W = P[j][None, :] + Vv[:, 0:1] * u[None, :] + Vv[:, 1:2] * t[None, :] + Vv[:, 2:3] * w[None, :]
        q, _d = kam.proj(W)
        for f in F:
            a, bb, c = q[f[0]], q[f[1]], q[f[2]]
            x0, x1 = int(max(min(a[0], bb[0], c[0]), 0)), int(min(max(a[0], bb[0], c[0]) + 1, 639))
            y0, y1 = int(max(min(a[1], bb[1], c[1]), 0)), int(min(max(a[1], bb[1], c[1]) + 1, 479))
            if x1 < x0 or y1 < y0:
                continue
            gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
            den = (bb[1] - c[1]) * (a[0] - c[0]) + (c[0] - bb[0]) * (a[1] - c[1])
            if abs(den) < 1e-9:
                continue
            l1 = ((bb[1] - c[1]) * (gx - c[0]) + (c[0] - bb[0]) * (gy - c[1])) / den
            l2 = ((c[1] - a[1]) * (gx - c[0]) + (a[0] - c[0]) * (gy - c[1])) / den
            mask[y0:y1 + 1, x0:x1 + 1] |= (l1 >= -1e-6) & (l2 >= -1e-6) & (1 - l1 - l2 >= -1e-6)
    return mask


def breiten(mask, Q, us):
    """Breite der Maske quer zur projizierten Kette Q an den Lagen us (Glieder), 0,5-px-Schritte (roll_render.stationen_breite)."""
    res = []
    for u in us:
        i = min(int(u), len(Q) - 2); f = u - i
        c = Q[i] * (1 - f) + Q[i + 1] * f
        t = _n(Q[i + 1] - Q[i]); n = np.array([-t[1], t[0]])

        def lauf(sg):
            k = 0.0
            while k < 150:
                q = c + n * sg * (k + 0.5)
                xi, yi = int(round(q[0])), int(round(q[1]))
                if not (0 <= xi < 640 and 0 <= yi < 480) or not mask[yi, xi]:
                    return k
                k += 0.5
            return k
        res.append(round(lauf(1) + lauf(-1), 1))
    return res


def r8a_r8d2(mo, D):
    kam = Kam(D['kamera'])
    M = meshes_gedreht(mo)
    out, ok, dips = {}, True, []
    for h in ('22', '24', '64'):
        A = D['ansichten'][h]
        P = np.array(A['kette'], float)
        u0, u1 = A['u'][0], min(A['u'][1], 7.0)
        mask = kette_maske(M, P, 0.0, kam)
        Qp, _ = kam.proj(P)
        br = breiten(mask, Qp, [u0 + s * (u1 - u0) for s in S])
        mitte = br[1:4]
        abw = [round(a - b, 1) for a, b in zip(br, A['analyse_phi0'])]
        okh = all(14.5 <= x <= 20.5 for x in mitte) and all(abs(x) <= 4.0 for x in abw[1:4])
        ok &= okh
        # Fugen von vorn: Gelenke j im sichtbaren Bereich (u0 + 0,25 .. u1 - 0,25), Gliedmitten j -+ 0,5
        gel = [j for j in range(1, 7) if u0 + 0.25 <= j <= u1 - 0.25]
        d_ = []
        for j in gel:
            w = breiten(mask, Qp, [j - 0.5, j, j + 0.5])
            d_.append(round(0.5 * (w[0] + w[2]) - w[1], 1))
        dips.append(float(np.mean(d_)) if d_ else 0.0)
        out[h] = dict(breite=br, vorhersage=A['analyse_phi0'], abweichung=abw, video=A['video'], rund_07b6703f=A['rund'], ok=bool(okh),
                      fugen=dict(gelenke=gel, schmaler_px=d_, mittel=round(float(np.mean(d_)), 2) if d_ else None))
    d2 = dict(mittel_je_ansicht=[round(x, 2) for x in dips], ok=bool(all(x >= 1.0 for x in dips)))
    return dict(ansichten=out, ok=bool(ok)), d2


# ------------------------------------------------------------------ PV (f_rast, Keyframe-Rolle)
def pv_mass(blob):
    import f_rast as FR
    mo = V.modell(blob)
    R, T = V.pose_r_t(mo)
    A = FR.zeichne(mo, V.PV, knochen=list(range(16, 23)), licht=False)
    mask = A['bone'][:480] >= 0
    pts = [T[b].astype(float) for b in range(16, 23)]
    R22 = R[22].astype(float) / 4096.0
    pts.append(T[22].astype(float) + R22[:, 1] * 541.0)
    Qp = V.proj(V.PV, np.array(pts))
    ymax = max(v[1] for v in mo.meshes[22].tv)
    tip = V.proj(V.PV, (T[22].astype(float) + R22[:, 1] * ymax)[None])[0]
    lang = float(np.linalg.norm(np.diff(Qp, axis=0), axis=1).sum() + np.linalg.norm(tip - Qp[-1]))
    return dict(breite=breiten(mask, Qp, [7 * s for s in S]), laenge=round(lang, 1), spitze=[round(float(x), 1) for x in tip], A=A, mo=mo)


def kasten_abstand(p, k):
    dx = max(k[0][0] - p[0], 0.0, p[0] - k[0][1]); dy = max(k[1][0] - p[1], 0.0, p[1] - k[1][1])
    return float(np.hypot(dx, dy))


def r8b_r8c(blob, bezug, D):
    n, b = pv_mass(blob), pv_mass(bezug)
    br, bb = n['breite'], b['breite']
    vid = D['ansichten']['90']['video_schnittkante']                 # Messziel Bauplan 2 (Schnittkante als Wurzel)
    vid2 = D['ansichten']['90']['video_obere_ecke']
    duenner = 1.0 - br[2] / bb[2]
    abw = [round(x - y, 1) for x, y in zip(br[2:], vid[2:])]
    abw2 = [round(x - y, 1) for x, y in zip(br[2:], vid2[2:])]
    okb = br[1] >= 36.0 and (bb[2] * 0.80 - 0.5) <= br[2] <= (bb[2] * 0.85 + 0.5) and all(abs(x) <= 4.0 for x in abw)
    mo = n['mo']
    m22 = mo.meshes[22]
    P22 = np.array([v[:3] for v in m22.tv], float)
    i = int(np.argmax(P22[:, 1]))
    achse = bool(P22[i][0] == 0 and P22[i][2] == 0)
    soll_y = int(541 + Q.DORN_ALT + Q.NADEL)
    ab = kasten_abstand(n['spitze'], ZIEL_SPITZE)
    okc = n['laenge'] >= b['laenge'] + 15.0 and ab <= 4.0 and achse and int(P22[i][1]) == soll_y
    return (dict(breite=br, bezug=bb, duenner_s5=round(100 * duenner, 1), video_h90=vid, abweichung_s5_s9=abw, video_h90_obere_ecke=vid2,
                 abweichung_obere_ecke=abw2, ok=bool(okb)),
            dict(laenge=n['laenge'], bezug=b['laenge'], spitze=n['spitze'], bezug_spitze=b['spitze'], abstand_ziel=round(ab, 1),
                 nadel_y=int(P22[i][1]), soll_y=soll_y, auf_achse=achse, ok=bool(okc)), n)


# ------------------------------------------------------------------ Fugen (Geometrie, Baender), Fleisch, Herkunft
def ringe_a(m, phi, k0):
    P = np.array([v[:3] for v in m['v'][k0:k0 + Q.S_]], float)
    eh, _eq = Q.achsen(phi)
    h = P @ eh
    return float(0.5 * (h.max() - h.min()))                 # Halbachse aus der Ausdehnung (der Schwerpunkt liegt wegen der 7 Ruecken-Ecken versetzt)


def lage(b):
    """Ring-Startindizes je Mesh nach dem Bau (schwanz_r8.mesh_bauen)."""
    if b == 16:
        return dict(D=22, E=35)
    if b == 18:
        return dict(C=0, M=11, D=22, S=35, E=46)
    if b == 22:
        return dict(C=0, S=24)
    return dict(C=0, D=11, S=24, E=35)


def r8d(blob, farbe, pv):
    _d, _o, ms, _t = KB.teile(blob)
    phi = {int(k): float(v) for k, v in json.load(open(os.path.join(HIER, 'unterseite_v8.json')))['innen_phi'].items()}
    geo = []
    R = Q.gelenk_radien(json.load(open(os.path.join(HIER, 'kette_v8.json'))))
    for j in range(1, 7):
        bv, bn = 15 + j, 16 + j
        lv, ln = lage(bv), lage(bn)
        try:
            e_d = ringe_a(ms[bv], phi[bv], lv['E']) / R[j]          # Endring von Glied j-1 gegen den Huellradius am Gelenk j
            s_c = ringe_a(ms[bn], phi[bn], ln['S']) / R[j]          # Manschette von Glied j ebenso
        except (KeyError, ValueError):
            e_d = s_c = 1.0
        geo.append((j, round(e_d, 3), round(s_c, 3)))
    ok1 = all(a <= 0.88 and c <= 0.88 for _j, a, c in geo) and len(ms[22]['v']) > 47
    band = {}
    for b in range(16, 23):
        band[b] = sum(1 for uv in ms[b]['tuv'] + ms[b]['quv'] if ((uv[2] >> 6) & 0x1FF) == Q.ZEILE_BAND)
    A = pv['A']
    z = A['zeile'][:480]; bo = A['bone'][:480]
    band_pv = {b: int(((z == Q.ZEILE_BAND) & (bo == b)).sum()) for b in range(16, 23)}
    gel_sicht = sum(1 for j in range(1, 7) if band_pv[15 + j] + band_pv[16 + j] > 0)
    if Q.FARBEN[farbe]['band']:
        ok3 = all(band[15 + j] > 0 and band[16 + j] > 0 for j in range(1, 7)) and gel_sicht >= 5
    else:
        ok3 = True
    return dict(geometrie=geo, ok1=bool(ok1), band_flaechen=band, band_pv_px=band_pv, gelenke_mit_band_pv=gel_sicht, band=Q.FARBEN[farbe]['band'],
                ok3=bool(ok3))


def r8e(blob, bezug, pv):
    import adern_r4 as A4
    import unterseite_r5 as U5
    _d, _o, ms, tim = KB.teile(blob)
    _d2, _o2, ms_e, tim_e = KB.teile(bezug)
    phi = {int(k): float(v) for k, v in json.load(open(os.path.join(HIER, 'unterseite_v8.json')))['innen_phi'].items()}
    falsch, n495 = [], 0
    alt16 = {tuple(f[1::2]) for f in ms_e[16]['tf'] + ms_e[16]['qf'] if set(f[1::2]) <= set(range(22)) | {34}}
    for b in range(16, 23):
        m = ms[b]
        P = np.array([v[:3] for v in m['v']], float)
        eh, eq = Q.achsen(phi[b])
        th, gr = ring_winkel(b, P, eh, eq)
        for key, F, U in (('d', m['tf'], m['tuv']), ('v', m['qf'], m['quv'])):
            for i, (f, uv) in enumerate(zip(F, U)):
                z = (uv[2] >> 6) & 0x1FF
                e = list(f[1::2])
                if b == 16 and tuple(e) in alt16:
                    continue                                   # Wurzelflaechen im Rumpf (Stufe w/u), UV wie 07b6703f
                ring = [k for k in e if k in th]
                if z in (Q.ZEILE_FLEISCH, Q.ZEILE_BAND):
                    n495 += z == Q.ZEILE_FLEISCH
                    if any(th[k] > gr[k] + TOL_GRAD for k in ring):
                        falsch.append('%d:%s%d' % (b, key, i))
                elif z == Q.ZEILE_GRAU:
                    if b == 22 and all(P[k][1] >= Q.ENDE_Y - 1.0 for k in e):
                        continue                               # Nadel (ab y 560) ganz grau (schwanz_r8.uv_setzen)
                    if _ganz_bauch(ring, th, gr):
                        falsch.append('%d:%s%d' % (b, key, i))
    ok1 = not falsch and n495 > 0
    # (2) PV: Normalen-theta je Bildpunkt
    A = pv['A']
    ys, xs, bo, th = U5.theta_bild(pv['mo'], V.PV, A)
    zz = A['zeile'][:480][ys, xs]
    fl = zz == Q.ZEILE_FLEISCH
    aussen = th >= 100.0; innen = th <= 40.0
    an_aussen = float(fl[aussen].mean() * 100) if aussen.any() else 0.0
    an_innen = float(fl[innen].mean() * 100) if innen.any() else 0.0
    ok2 = an_aussen <= 1.0 and an_innen >= 50.0
    # (3) Herkunft
    nutz = {Q.ZEILE_FLEISCH: A4.nutzung(None, 482), Q.ZEILE_BAND: A4.nutzung(None, 481)}
    grau = np.zeros((256, 384), bool)
    for b in range(16, 23):
        for uv in ms_e[b]['tuv'] + ms_e[b]['quv']:
            if ((uv[2] >> 6) & 0x1FF) == Q.ZEILE_GRAU:
                grau |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    nutz[Q.ZEILE_GRAU] = grau
    fremd, zeilen = {}, {}
    for b in range(16, 23):
        for uv in ms[b]['tuv'] + ms[b]['quv']:
            z = (uv[2] >> 6) & 0x1FF
            zeilen[z] = zeilen.get(z, 0) + 1
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            if z not in nutz:
                fremd[z] = fremd.get(z, 0) + int(mk.sum())
                continue
            n = int((mk & ~nutz[z]).sum())
            if n:
                fremd[z] = fremd.get(z, 0) + n
    t, te = KB.tim_teile(tim), KB.tim_teile(tim_e)
    ok3 = not fremd and bool(np.array_equal(t['pix'], te['pix']))
    return dict(falsch=falsch[:30], n_falsch=len(falsch), flaechen_495=n495, ok1=bool(ok1), pv_fleisch_aussen=round(an_aussen, 2),
                pv_fleisch_innen=round(an_innen, 1), ok2=bool(ok2), zeilen=zeilen, fremde_texel=fremd, ok3=bool(ok3),
                ok=bool(ok1 and ok2 and ok3))


def ring_bloecke(b, n):
    """Startindizes der 11er-Ringe von Mesh b nach dem Bau (schwanz_r8.mesh_bauen): Bloecke ab 0, die 2 Achs-Ecken hinter nr Ringen (nr = 3 fuer
    16/18, sonst 2) ausgelassen."""
    nr = 3 if b in (16, 18) else 2
    k, out = 0, []
    while k + Q.S_ <= n:
        if k == nr * Q.S_:
            k += 2
            continue
        out.append(k)
        k += Q.S_
    return out


def ring_winkel(b, P, eh, eq):
    """Je Ring-Ecke: |Parameterwinkel| gegen den Bauch (Ellipse des Rings: Mitte/Halbachsen aus der Ausdehnung in e_h / e_q) und die
    Fleischgrenze schwanz_r8.grenze(u) mit u = Glied + Median-y des Rings / 541."""
    th, gr = {}, {}
    for k0 in ring_bloecke(b, len(P)):
        R = P[k0:k0 + Q.S_]
        h, q = R @ eh, R @ eq
        ch, cq = 0.5 * (h.max() + h.min()), 0.5 * (q.max() + q.min())
        A, B = max(0.5 * (h.max() - h.min()), 1e-6), max(0.5 * (q.max() - q.min()), 1e-6)
        u = (b - 16) + float(np.clip(np.median(R[:, 1]) / 541.0, 0.0, 1.0))
        for i in range(Q.S_):
            th[k0 + i] = abs(float(np.degrees(np.arctan2((q[i] - cq) / B, (h[i] - ch) / A))))
            gr[k0 + i] = Q.grenze(u)
    return th, gr


def _ganz_bauch(ring, th, gr):
    """Graue Flaeche ganz im Bauchsektor (alle Ring-Ecken innerhalb der Grenze, mindestens eine deutlich) = Fehler."""
    return all(th[k] <= gr[k] + TOL_GRAD for k in ring) and any(th[k] < gr[k] - TOL_GRAD for k in ring)


def r8f(blob, bezug):
    d, _o, ms, tim = KB.teile(blob)
    de, _oe, mse, time = KB.teile(bezug)
    sek = [blob[d[i]:d[i + 1]] == bezug[de[i]:de[i + 1]] for i in range(7)]
    meshes = [ms[i] == mse[i] for i in range(16)]
    mo = V.modell(blob)
    gl = all(tuple(mo.skel.rel[b]) == (0, 541, 0) for b in range(17, 23))
    return dict(sektionen=sek, tim=tim == time, meshes_0_15=all(meshes), mesh9=meshes[9], glieder_541=gl,
                ok=bool(all(sek) and tim == time and all(meshes) and gl))


def ist_r8(blob):
    mo = V.modell(blob)
    return max(v[1] for v in mo.meshes[22].tv) > 1000


def alles(blob, farbe='b'):
    D = json.load(open(DATEN))
    bezug = eingang()
    mo = V.modell(blob)
    r = {'sha256': V.sha(blob), 'farbe': farbe}
    r['R8a'], d2 = r8a_r8d2(mo, D)
    r['R8b'], r['R8c'], pv = r8b_r8c(blob, bezug, D)
    d = r8d(blob, farbe, pv)
    d['vorn'] = d2
    d['ok'] = bool(d['ok1'] and d2['ok'] and d['ok3'])
    r['R8d'] = d
    r['R8e'] = r8e(blob, bezug, pv)
    r['R8f'] = r8f(blob, bezug)
    return r


def zeilen(r):
    z = ['Schwanz-Riegel Runde 8 (riegel_schwanz_r8.py) fuer sha256 %s, Farbprobe %s' % (r['sha256'], r['farbe'])]
    g = lambda ok: 'GRUEN' if ok else 'ROT'
    a = r['R8a']['ansichten']
    z.append('R8a Breite vorn (Rollreihe, Bauch zur Kamera, s .1/.3/.5/.7/.9 px; Soll s .3-.7 15-20 und |neu - Vorhersage| <= 4): %s -> %s'
             % ('; '.join('h%s %s (Vorhersage %s, Abw. %s, Video %s, rund 07b6703f %s)' % (h, v['breite'], v['vorhersage'], v['abweichung'], v['video'],
                                                                                         v['rund_07b6703f']) for h, v in a.items()), g(r['R8a']['ok'])))
    b = r['R8b']
    z.append('R8b Seite PV (f_rast, Keyframe-Rolle): s .1-.9 %s (Bezug 07b6703f %s); s .3 >= 36: %.1f; s .5 %.1f %% duenner (Soll 15-20); s .5/.7/.9 '
             'gegen Nutzer-Umriss h90 (Wurzel Schnittkante, Messziel Bauplan 2) %s: Abw. %s (<= 4); Angabe Wurzel obere Ecke %s: Abw. %s -> %s'
             % (b['breite'], b['bezug'], b['breite'][1], b['duenner_s5'], b['video_h90'][2:], b['abweichung_s5_s9'], b['video_h90_obere_ecke'][2:],
                b['abweichung_obere_ecke'], g(b['ok'])))
    c = r['R8c']
    z.append('R8c Laenge/Spitze PV: Laenge %.1f px (Bezug %.1f, Soll >= Bezug + 15; Ziel der Analyse ~190 Offen), Nadelspitze %s (Bezug %s), Abstand '
             'zum Zielkasten x 286-289 / y 349-356 %.1f px (<= 4), Nadel y %d (Soll %d) auf der Achse %s -> %s'
             % (c['laenge'], c['bezug'], c['spitze'], c['bezug_spitze'], c['abstand_ziel'], c['nadel_y'], c['soll_y'], c['auf_achse'], g(c['ok'])))
    d = r['R8d']
    z.append('R8d Fugen: (1) Hochachse Endring und Manschette / Huellradius je Gelenk %s (<= 0,88) %s; (2) von vorn schmaler an den Gelenken im Mittel %s px '
             '(>= 1,0) %s; (3) Band (Probe %s): Flaechen auf 494 je Glied %s, PV-Bildpunkte %s, Gelenke mit Band in PV %d (>= 5) %s -> %s'
             % (d['geometrie'], g(d['ok1']), d['vorn']['mittel_je_ansicht'], g(d['vorn']['ok']), 'mit Baendern' if d['band'] else 'ohne Baender',
                d['band_flaechen'], d['band_pv_px'], d['gelenke_mit_band_pv'], g(d['ok3']) if d['band'] else 'Angabe', g(d['ok'])))
    for h, v in r['R8a']['ansichten'].items():
        z.append('   R8d (2) h%s Gelenke %s schmaler um %s px' % (h, v['fugen']['gelenke'], v['fugen']['schmaler_px']))
    e = r['R8e']
    z.append('R8e Fleisch nur Bauchseite: (1) %d Flaechen auf 495, ausserhalb des Bauchsektors bzw. graue im Bauchsektor %d %s %s; (2) PV Fleisch-Anteil '
             'bei Normalen-|theta| >= 100: %.2f %% (<= 1), <= 40: %.1f %% (>= 50) %s; (3) Herkunft: Flaechen je Zeile %s, fremde Texel %s, Texturbild '
             'bytegleich %s -> %s' % (e['flaechen_495'], e['n_falsch'], e['falsch'][:8], g(e['ok1']), e['pv_fleisch_aussen'], e['pv_fleisch_innen'],
                                      g(e['ok2']), e['zeilen'], e['fremde_texel'], 'ja' if e['ok3'] else 'nein', g(e['ok'])))
    f = r['R8f']
    z.append('R8f Bytegleich zum Eingang 07b6703f: Sektionen 0-6 %s, TIM %s, Meshes 0-15 %s (Mesh 9 %s), Gliedlaengen 541 %s -> %s'
             % (f['sektionen'], f['tim'], f['meshes_0_15'], f['mesh9'], f['glieder_541'], g(f['ok'])))
    rot = [k for k in ('R8a', 'R8b', 'R8c', 'R8d', 'R8e', 'R8f') if not r[k]['ok']]
    z.append('SUMME R8: %d von 6 gruen%s' % (6 - len(rot), ('; ROT: ' + ', '.join(rot)) if rot else ''))
    return z, rot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd', nargs='?', default=V.ASSET)
    ap.add_argument('--farbe', default=Q.STANDARD)
    ap.add_argument('--aus'); ap.add_argument('--json')
    a = ap.parse_args()
    r = alles(open(a.emd, 'rb').read(), a.farbe)
    z, rot = zeilen(r)
    txt = '\n'.join(z)
    print(txt)
    if a.aus:
        open(a.aus, 'w', encoding='utf-8').write(txt + '\n')
    if a.json:
        r2 = {k: v for k, v in r.items()}
        json.dump(r2, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(1 if rot else 0)


if __name__ == '__main__':
    main()
