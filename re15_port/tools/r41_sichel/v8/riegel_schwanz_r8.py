"""SF V8 Runde 8: Schwanz-Riegel R8a-R8f fuer die Stufe q (schwanz_r8.py). Gegenprobe: Stand 7e 07b6703f muss in R8a-R8e ROT sein.

    python riegel_schwanz_r8.py [EMD] [--farbe a|b|c] [--aus TXT] [--json JSON]      Exit 0 = alle gruen

Nutzer 2026-11-14 (woertlich): "weder mit dem rot der Schwanzfarbe noch mit dem etwas zu steifen grossen Schwanz zufrieden" + Version_80%/Areas*.png.
Messziele aus dem Auftrag Runde 8 und der Analyse scratchpad/schwanz_v8/analyse/ANALYSE.md (Bauplan Schritte 1, 2, 4); Daten der Analyse (Ketten der
Szene, Vorhersage der Rollreihe, Nutzer-Umriss) in riegel_schwanz_r8.json. Kein RE-Befund, keine @0x-Konstante (Sichel-Form auf keiner Disc).

R8a Breite vorn   Rollreihe der Analyse (roll_render/teil_b) am GEBAUTEN Asset: echte Meshes 16-22 entlang der Szenenketten h22 / h24 / h64 (Bauchachse
                  zur Kamera, phi 0), Breite quer zur Kette an s .3 / .5 / .7 je 15-20 px (Auftrag "vorn 15-20 px breit", +-0,5 px Messschritt) und
                  |neu - Vorhersage der Analyse (flach, Querdicke 280)| <= 4 px; s .1 / .9 (Wurzel, Knolle) nur Angabe.
R8b Seite (PV)    Runde 8b (Pruefer pr8: "in festen Abstaenden ab der Wurzel, nicht in Anteilen der Port-Laenge"): Vorlage-Pose PV, sichtbare
                  Schwanz-Bildpunkte der ganzen Figur (f_rast), Breite = 2 x Abstandstransformation auf der Mittellinie in 30-170 px ab dem
                  koerpernahen Ende (Verfahren a_mass.profil_abs des Pruefers); gegen den Nutzer-Umriss h90 mit demselben Verfahren je <= 6 px,
                  im Mittel <= 5 px; bei 90 px <= 0,9 x vorher.
R8c Laenge/Spitze Runde 8b ("nur mit der sichtbaren Laenge"): Mittellinienlaenge der sichtbaren Bildpunkte >= 180 px, tiefster sichtbarer
                  Schwanz-Bildpunkt y >= 346 (Video 351), Nadel auf der Gliedachse bis SPITZE_Y.
R8d Fugen         (1) Geometrie: an allen 6 Gelenken Hochachse von Endring und Manschette / Huellradius am Gelenk (kette_v8.json x MITTE) <= 0,88;
                  (2) sichtbar von vorn: in der Rollreihe h22 / h24 / h64 ist die Breite an den sichtbaren Gelenken im Mittel >= 1,0 px schmaler als
                  in den Gliedmitten daneben; (3) Runde 8d (Farbe b = a, Nutzer "Streifen ... gehoeren da nicht hin"): KEINE Band-/Ring-Flaeche;
                  jede Fugen-Flaeche (Ringe S/C bzw. D/E mit Deckel-Mitte, aus den Ring-Lagen lage()) ist bauch (Bauchfeld) oder grau, an
                  allen 6 Gelenken auf beiden Seiten Fugen-Flaechen mit dem Bauchfeld; nahtlos: die Bauch-UV der Ecken des Gelenkrings liegt auf
                  beiden Seiten in derselben Feldzeile (Median-v gleich +-1 Texel); in PV Bauch-Bildpunkte auf Fugen-Flaechen an >= 5 Gelenken;
                  Flanke: alle grauen Flaechen (ausser den Wurzelflaechen im Rumpf wie R8e) im EINEN Grau-Feld (schwanz_r8.json felder.grau_alle)
                  und ebenso nahtlos (Grau-v +-1 Texel).
                  Farbe "ring" (Runde 8c, Gegenprobe): Band-Flaechen = genau die Fugen-Flaechen aller 11 Sektoren, in PV an >= 5 Gelenken.
R8e Fleisch nur   (1) Runde 8c: jede Bauch-Flaeche (Klasse aus dem Fleisch-Feld, schwanz_r8.klasse_uv; Bauch und Ring liegen auf derselben
                  Zeile 493; eine Flaeche ohne Klasse ist falsch; ausser den 14 Wurzelflaechen von Mesh 16 im Rumpf, Stufe w/u) liegt mit allen Ring-Ecken
    Bauchseite    im Bauchsektor |theta| <= Grenze(u) + TOL_GRAD (Grenze 90 -> 60, schwanz_r8.grenze; theta = Parameterwinkel der Ellipse des Rings),
                  keine Flaeche auf 487 ganz im Bauchsektor (ausser der Nadel ab y 560, die ganz grau ist); (2) PV: Anteil Fleisch-Zeile an den Schwanz-Bildpunkten mit Normalen-|theta| >= 100 <= 1 %, mit |theta| <= 40 >= 50 %;
                  (3) Herkunft: Texel unter 493 / 494 / 495 = Texel, die Original-Flaechen der Original-Zeile 480 / 481 / 482 benutzen; unter 487 = Texel
                  der Oberseite (487) der Meshes 16-22 im Eingang 07b6703f; Asset-Texel dort bytegleich Original bzw. Eingang;
                  (4) Runde 8e: kein Texel der Bauch-Flaechen (Rasterung KB.raster, Farbe aus ihrer CLUT-Zeile) unter L* BAUCH_L_MIN - die dunkle
                  Naht des Original-Bilds an u 54-55 (L* 11,6-27,0) gab im Spiel den Laengsstrich an der Bauchkante (Nutzer 15.11.).
R8f Bytegleich    Sektionen 0-6 (Skelett, Clips, Keyframes), TIM (Texturbild, 16 CLUT-Zeilen) und Meshes 0-15 (Mesh 9 = V7-Kopf) = Eingang 07b6703f,
                  ausser den 12-Bit-Winkeln der Knochen 17-22 in den von schwanz_r8.heben angehobenen Keyframes; Gliedlaengen 17-22 = (0, 541, 0).
R8g Szene         (--szene) Schwanzkette der Szene gegen 7e: jede Verschiebung liegt in einem Takt, in dem die 7e-Kette mit den Mesh-Massen dieses
                  Assets die Bodenregel des Codes verletzt, oder in dessen Nachlauf; state.log bytegleich; neue Kette haelt die Bodenregel ein.
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
ABSTAENDE = (10, 30, 50, 70, 90, 110, 130, 150, 170, 190, 210)
PV_WURZEL = (376, 220)                # Wurzel im Bild (PV, Gelenk 0 projiziert 375,7 / 219,5) - waehlt das koerpernahe Ende der Mittellinie
R8B_STELLEN = (30, 50, 70, 90, 110, 130, 150, 170)
R8G_NACHLAUF = 40                     # Takte Nachlauf nach dem letzten Bodenregel-Takt (Feder/Erhalt 0,5, Mitnahme 0,8: gemessen klingt die Abweichung
                                      # in der 7e/8-Szene binnen 34 Takten auf <= 1 E. ab, szene_kette_7e_r8.txt h67 -> h101)
R8B_MAX, R8B_MITTEL = 6.0, 5.0        # Abweichung Port - Video je Stelle hoechstens 6 px, im Mittel 5 px (Video weich, +2-3 px Unschaerfe, ANALYSE 7;
                                      # das Verfahren misst Port und Video gleich); dazu bei 90 px <= 0,9 x vorher (Mitte duenner)
R8C_LAENGE = 180.0                    # sichtbare Laenge in PV (Video 216,9, vorher 164,7; der Rest liegt laut Analyse an der Wurzel und der Bahn)
R8C_TIEF = 346                        # tiefster sichtbarer Schwanz-Bildpunkt in PV (Video 351, vorher 325; Pruefer "bis y ~350")
WURZEL_495 = 14                       # Wurzelflaechen von Mesh 16 auf 495 (Kegel/Rohr im Rumpf, Stufe w/u, Indizes 0-21 / 34, UV wie 07b6703f)
BAUCH_L_MIN = 30.0                    # R8e (4): Naht-Texel L* 11,6-27,0, uebrige Texel von C1 (u 54-65 v 51-55) L* 31,8-50,4 (Runde 8e, scratchpad schwanz_r9/mess/c1lab.py)
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


def mittellinie(m):
    """Laengster Pfad auf dem Skelett der groessten Zusammenhangsflaeche der Maske m (Verfahren des Pruefers pr8 a_mass.mittellinie)."""
    import heapq
    from scipy import ndimage as ndi
    from skimage.morphology import skeletonize
    m = ndi.binary_opening(m, iterations=1) if m.sum() > 200 else m
    lab, n = ndi.label(m)
    if n == 0:
        return None
    m = lab == (np.argmax(np.bincount(lab.ravel())[1:]) + 1)
    ys, xs = np.nonzero(skeletonize(m))
    idx = {(y, x): i for i, (y, x) in enumerate(zip(ys, xs))}
    nb = [[] for _ in ys]
    for i, (y, x) in enumerate(zip(ys, xs)):
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if (dy or dx) and (y + dy, x + dx) in idx:
                    nb[i].append((idx[(y + dy, x + dx)], (dy * dy + dx * dx) ** 0.5))

    def weit(s0):
        dd_, pr = {s0: 0.0}, {s0: None}
        q = [(0.0, s0)]
        while q:
            dd, u = heapq.heappop(q)
            if dd > dd_[u]:
                continue
            for v, w in nb[u]:
                if dd + w < dd_.get(v, 1e18):
                    dd_[v] = dd + w; pr[v] = u; heapq.heappush(q, (dd + w, v))
        e = max(dd_, key=dd_.get)
        return e, pr
    a, _ = weit(0)
    b, pr = weit(a)
    pfad, u = [], b
    while u is not None:
        pfad.append((ys[u], xs[u])); u = pr[u]
    return dict(pfad=np.array(pfad), dt=ndi.distance_transform_edt(m), maske=m)


def profil_abstand(m, wurzel_xy, abst=ABSTAENDE):
    """Breite (2 x Abstandstransformation) in festen Abstaenden (px entlang der Mittellinie) ab dem koerpernahen Ende; dazu die Laenge."""
    r = mittellinie(m)
    pf = r['pfad']
    if np.hypot(pf[-1][1] - wurzel_xy[0], pf[-1][0] - wurzel_xy[1]) < np.hypot(pf[0][1] - wurzel_xy[0], pf[0][0] - wurzel_xy[1]):
        pf = pf[::-1]
    seg = np.r_[0, np.cumsum(np.hypot(*np.diff(pf, axis=0).T))]
    out = {}
    for d_ in abst:
        out[d_] = None if d_ > seg[-1] else round(2 * float(r['dt'][pf[int(np.argmin(abs(seg - d_)))][0], pf[int(np.argmin(abs(seg - d_)))][1]]), 1)
    return dict(L=round(float(seg[-1]), 1), breite=out)


def pv_sichtbar(blob):
    """PV (Vorlage-Pose), ganze Figur mit f_rast (Maler-Reihenfolge, Culling): sichtbare Schwanz-Bildpunkte (Knochen 16-22)."""
    import f_rast as FR
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.PV, licht=False)
    return (A['bone'][:480] >= 16) & (A['bone'][:480] <= 22), mo, A


def r8b_r8c(blob, bezug, D):
    """Runde 8b (Pruefer pr8 (C) "R8b in festen Abstaenden ab der Wurzel", "R8c nur mit der sichtbaren Laenge"): PV, sichtbare Schwanz-Bildpunkte der
    ganzen Figur, Mittellinie und Breiten wie das Pruefer-Werkzeug a_mass.profil_abs; Video = Nutzer-Umriss h90 mit demselben Verfahren
    (riegel_schwanz_r8.json, gemessen n7_pv_abstand.py)."""
    vid = {int(k): v for k, v in D['ansichten']['90']['abstand']['breite'].items()}
    vL, vy = D['ansichten']['90']['abstand']['L'], D['ansichten']['90']['abstand']['tief_y']
    m, mo, A = pv_sichtbar(blob)
    mb, _mob, _Ab = pv_sichtbar(bezug)
    n, b = profil_abstand(m, PV_WURZEL), profil_abstand(mb, PV_WURZEL)
    ys = np.nonzero(m)[0]; ysb = np.nonzero(mb)[0]
    abw = {d_: (None if (n['breite'][d_] is None or vid.get(d_) is None) else round(n['breite'][d_] - vid[d_], 1)) for d_ in R8B_STELLEN}
    werte = [abs(x) for x in abw.values() if x is not None]
    okb = (all(x is not None for x in abw.values()) and max(werte) <= R8B_MAX and float(np.mean(werte)) <= R8B_MITTEL
           and n['breite'][90] <= 0.9 * b['breite'][90])
    m22 = mo.meshes[22]
    P22 = np.array([v[:3] for v in m22.tv], float)
    i = int(np.argmax(P22[:, 1]))
    achse = bool(P22[i][0] == 0 and P22[i][2] == 0)
    okc = n['L'] >= R8C_LAENGE and int(ys.max()) >= R8C_TIEF and achse and int(P22[i][1]) == int(Q.SPITZE_Y)
    return (dict(breite=n['breite'], bezug=b['breite'], video=vid, abweichung=abw, mittel=round(float(np.mean(werte)), 1) if werte else None,
                 ok=bool(okb)),
            dict(laenge=n['L'], bezug=b['L'], video_laenge=vL, tief_y=int(ys.max()), bezug_tief_y=int(ysb.max()), video_tief_y=vy,
                 nadel_y=int(P22[i][1]), soll_y=int(Q.SPITZE_Y), auf_achse=achse, ok=bool(okc)), dict(mo=mo, A=A))


# ------------------------------------------------------------------ Fugen (Geometrie, Baender), Fleisch, Herkunft
def ringe_a(m, phi, k0):
    P = np.array([v[:3] for v in m['v'][k0:k0 + Q.S_]], float)
    eh, _eq = Q.achsen(phi)
    h = P @ eh
    return float(0.5 * (h.max() - h.min()))                 # Halbachse aus der Ausdehnung (der Schwerpunkt liegt wegen der 7 Ruecken-Ecken versetzt)


def lage(b):
    """Ring-Startindizes je Mesh nach dem Bau (schwanz_r8.mesh_bauen; Runde 8b mit Zwischenringen Z1/Z2, gemessen info['pos'])."""
    if b == 16:
        return dict(D=22, E=35)
    if b == 18:
        return dict(C=0, M=11, D=22, S=35, Z1=46, Z2=57, E=68)
    if b == 22:
        return dict(C=0, E=11, S=24)
    return dict(C=0, D=11, S=24, Z1=35, Z2=46, E=57)


def r8d(blob, farbe, pv, bezug=None):
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
    band, fuge_n, abweich = {}, {}, {}
    kl = klassen(ms)
    for b in range(16, 23):
        fs = fugen_flaechen(b, ms[b])
        bs = {k for k, c in kl[b].items() if c == 'band'}
        band[b] = len(bs); fuge_n[b] = len(fs)
        abweich[b] = sorted(fs ^ bs)[:6]
    A = pv['A']
    bo = A['bone'][:480]; fk = A['flaeche'][:480]
    band_pv = {b: 0 for b in range(16, 23)}
    ys, xs = np.nonzero((bo >= 16) & (bo <= 22))
    for y, x in zip(ys, xs):
        b = int(bo[y, x])
        if kl[b].get(str(fk[y, x])) == 'band':
            band_pv[b] += 1
    gel_sicht = sum(1 for j in range(1, 7) if band_pv[15 + j] + band_pv[16 + j] > 0)
    # Runde 8d: Fugen mit dem Bauchfeld, nahtlos
    fuge_kl, fuge_bauch, fuge_bauch_pv, naht, fs_alle = {}, {}, {b: 0 for b in range(16, 23)}, {}, {}
    for b in range(16, 23):
        fs = fs_alle[b] = fugen_flaechen(b, ms[b])
        fuge_kl[b] = sorted({str(kl[b][k]) for k in fs})
        fuge_bauch[b] = sum(1 for k in fs if kl[b][k] == 'bauch')
    for y, x in zip(ys, xs):
        b = int(bo[y, x]); k = str(fk[y, x])
        if kl[b].get(k) == 'bauch' and k in fs_alle[b]:
            fuge_bauch_pv[b] += 1
    naht_g = {}
    for j in range(1, 7):
        ve = gelenk_v(ms[15 + j], kl[15 + j], lage(15 + j)['E'])
        vs_ = gelenk_v(ms[16 + j], kl[16 + j], lage(16 + j)['S'])
        naht[j] = (ve, vs_, None if ve is None or vs_ is None else round(abs(ve - vs_), 1))
        ge = gelenk_v(ms[15 + j], kl[15 + j], lage(15 + j)['E'], 'grau')
        gs = gelenk_v(ms[16 + j], kl[16 + j], lage(16 + j)['S'], 'grau')
        naht_g[j] = (ge, gs, None if ge is None or gs is None else round(abs(ge - gs), 1))
    # Runde 8d Flanke: alle grauen Flaechen im EINEN Grau-Feld (schwanz_r8.json felder.grau_alle)
    ga = Q.felder_laden().get('grau_alle')
    grau_fremd = 0
    alt16 = set()
    if bezug is not None:                                  # Wurzelflaechen im Rumpf (Stufe w/u) behalten ihre UV wie 07b6703f (wie R8e)
        _d2, _o2, ms_e, _t2 = KB.teile(bezug)
        alt16 = {tuple(f[1::2]) for f in ms_e[16]['tf'] + ms_e[16]['qf'] if set(f[1::2]) <= set(range(22)) | {34}}
    for b in range(16, 23):
        for key, F, U in (('d', ms[b]['tf'], ms[b]['tuv']), ('v', ms[b]['qf'], ms[b]['quv'])):
            for i, (f, uv) in enumerate(zip(F, U)):
                if b == 16 and tuple(f[1::2]) in alt16:
                    continue
                if kl[b]['%s%d' % (key, i)] == 'grau' and not (ga and all(ga[0] <= u <= ga[1] for u in uv[0::3])
                                                                and all(ga[2] <= v <= ga[3] for v in uv[1::3])):
                    grau_fremd += 1
    gel_fuge_pv = sum(1 for j in range(1, 7) if fuge_bauch_pv[15 + j] + fuge_bauch_pv[16 + j] > 0)
    if Q.FARBEN[farbe]['band']:
        ok3 = all(not abweich[b] and band[b] > 0 for b in range(16, 23)) and gel_sicht >= 5
    else:
        ok3 = (all(band[b] == 0 for b in range(16, 23)) and all(set(fuge_kl[b]) <= {'bauch', 'grau'} for b in range(16, 23))
               and all(fuge_bauch[15 + j] > 0 and fuge_bauch[16 + j] > 0 for j in range(1, 7))
               and all(n_[2] is not None and n_[2] <= 1.0 for n_ in naht.values()) and gel_fuge_pv >= 5
               and grau_fremd == 0 and all(n_[2] is not None and n_[2] <= 1.0 for n_ in naht_g.values()))
    return dict(geometrie=geo, ok1=bool(ok1), band_flaechen=band, fugen_flaechen=fuge_n, band_ungleich_fuge=abweich, band_pv_px=band_pv,
                gelenke_mit_band_pv=gel_sicht, band=Q.FARBEN[farbe]['band'], fugen_klassen=fuge_kl, fugen_bauch=fuge_bauch,
                fugen_bauch_pv_px=fuge_bauch_pv, gelenke_mit_fugen_bauch_pv=gel_fuge_pv, naht_v=naht, naht_grau_v=naht_g,
                grau_ausserhalb_feld=grau_fremd, ok3=bool(ok3))


def gelenk_v(m, kl_b, k0, klasse='bauch'):
    """Runde 8d: Median der UV-v aller Ecken des Gelenkrings (Ring-Ecken k0 .. k0 + S_ - 1) in Flaechen der Klasse; None = keine."""
    ring = set(range(k0, k0 + Q.S_))
    vs = []
    for key, F, U in (('d', m['tf'], m['tuv']), ('v', m['qf'], m['quv'])):
        for i, (f, uv) in enumerate(zip(F, U)):
            if kl_b.get('%s%d' % (key, i)) != klasse:
                continue
            for k, e in enumerate(f[0::2]):
                if e in ring:
                    vs.append(uv[3 * k + 1])
    return float(np.median(vs)) if vs else None


def klassen(ms):
    """Runde 8c: (Knochen) -> {'d<i>' / 'v<i>': Klasse} aus Zeile, Seite und UV (schwanz_r8.klasse_uv; Schluessel wie f_rast 'flaeche')."""
    out = {}
    for b in range(16, 23):
        out[b] = {}
        for key, U in (('d', ms[b]['tuv']), ('v', ms[b]['quv'])):
            for i, uv in enumerate(U):
                out[b]['%s%d' % (key, i)] = Q.klasse_uv(uv)
    return out


def fugen_flaechen(b, m):
    """Runde 8c: Fugen-Flaechen eines Glieds = Flaechen, deren Ecken alle in (Ring S + Ring C + Anfangs-Mitte) bzw. (Ring D + Ring E + End-Mitte)
    liegen (KERBE_LAENGE x L um die Gelenke; Ring-Lagen aus lage(), Achs-Ecken je Mesh wie schwanz_r8.mesh_bauen). Mesh 22 nur am Anfang
    (Ring E dort = y 760, kein Gelenk), Mesh 16 nur am Ende."""
    l = lage(b)
    r = lambda nm: set(range(l[nm], l[nm] + Q.S_))
    if b == 16:
        mengen = [r('D') | r('E') | {33}]
    elif b == 22:
        mengen = [r('S') | r('C') | {23}]
    elif b == 18:
        mengen = [r('S') | r('C') | {34}, r('D') | r('E') | {33}]
    else:
        mengen = [r('S') | r('C') | {23}, r('D') | r('E') | {22}]
    out = set()
    for key, F in (('d', m['tf']), ('v', m['qf'])):
        for i, f in enumerate(F):
            e = set(f[1::2])
            if any(e <= mg for mg in mengen):
                out.add('%s%d' % (key, i))
    return out


def r8e(blob, bezug, pv):
    import adern_r4 as A4
    import unterseite_r5 as U5
    _d, _o, ms, tim = KB.teile(blob)
    _d2, _o2, ms_e, tim_e = KB.teile(bezug)
    phi = {int(k): float(v) for k, v in json.load(open(os.path.join(HIER, 'unterseite_v8.json')))['innen_phi'].items()}
    falsch, n495 = [], 0
    kl = klassen(ms)
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
                c = kl[b]['%s%d' % (key, i)]
                if c is None:
                    falsch.append('%d:%s%d ohne Klasse' % (b, key, i))     # Runde 8c: Zeile/UV in keinem Feld
                elif c == 'band':
                    continue                                   # Ring umlaufend (R8d (3) prueft, dass er genau auf den Fugen liegt)
                elif c == 'bauch':
                    n495 += 1
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
    fk = A['flaeche'][:480][ys, xs]
    fl = np.array([kl[int(b_)].get(str(k_)) == 'bauch' for b_, k_ in zip(bo, fk)], bool)
    aussen = th >= 100.0; innen = th <= 40.0
    an_aussen = float(fl[aussen].mean() * 100) if aussen.any() else 0.0
    an_innen = float(fl[innen].mean() * 100) if innen.any() else 0.0
    ok2 = an_aussen <= 1.0 and an_innen >= 50.0
    # (3) Herkunft
    nutz = {z_: A4.nutzung(None, z_ - 13) for z_ in (493, 494, 495)}                  # 493/494/495 = Original 480/481/482 (bytegleich)
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
    pix_gleich = bool(np.array_equal(t['pix'], te['pix']))
    ok3 = not fremd and pix_gleich
    # (4) Runde 8e: keine dunklen Naht-Texel auf den Bauch-Flaechen
    import v4_gemein as G
    dunkel, l_min = 0, 999.0
    for b in range(16, 23):
        for key, U in (('d', ms[b]['tuv']), ('v', ms[b]['quv'])):
            for i, uv in enumerate(U):
                if kl[b]['%s%d' % (key, i)] != 'bauch':
                    continue
                z = (uv[2] >> 6) & 0x1FF
                c = t['clut'][z - t['cy']].astype(int)
                lab = G.lab(np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)[None])[0]
                mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
                L = lab[t['pix'][mk]][:, 0]
                dunkel += int((L < BAUCH_L_MIN).sum())
                l_min = min(l_min, float(L.min()))
    ok4 = dunkel == 0 and n495 > 0
    return dict(falsch=falsch[:30], n_falsch=len(falsch), flaechen_495=n495, ok1=bool(ok1), pv_fleisch_aussen=round(an_aussen, 2),
                pv_fleisch_innen=round(an_innen, 1), ok2=bool(ok2), zeilen=zeilen, fremde_texel=fremd, pix_gleich=pix_gleich, ok3=bool(ok3),
                bauch_texel_dunkel=dunkel, bauch_l_min=round(l_min, 1), ok4=bool(ok4), ok=bool(ok1 and ok2 and ok3 and ok4))


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
    """Bytegleich zum Eingang ausser Meshes 16-22 und (Runde 8b) den 12-Bit-Winkeln der Knochen 17-22 in den angehobenen Keyframes: die EMR des
    Pruefllings, in der genau diese Bits auf die Werte des Eingangs zurueckgesetzt werden, muss bytegleich zur EMR des Eingangs sein."""
    import birkin_formen as BF
    import emd_ansichtsblatt as EA
    d, _o, ms, tim = KB.teile(blob)
    de, _oe, mse, time = KB.teile(bezug)
    sek = [blob[d[i]:d[i + 1]] == bezug[de[i]:de[i + 1]] for i in range(7)]
    meshes = [ms[i] == mse[i] for i in range(16)]
    mo = V.modell(blob)
    sk = mo.skel
    emr, emr_e = bytearray(blob[d[2]:d[3]]), bytes(bezug[de[2]:de[3]])
    geaendert = 0
    if len(emr) == len(emr_e):
        for k in range(sk.kf_count):
            o = EA.u16(emr, 2) + k * sk.kf_size + 12
            anders = False
            for b in range(17, 23):
                for j in range(3):
                    bit = b * 36 + j * 12
                    alt = sum(((emr_e[o + ((bit + i) >> 3)] >> ((bit + i) & 7)) & 1) << i for i in range(12))
                    neu = sum(((emr[o + ((bit + i) >> 3)] >> ((bit + i) & 7)) & 1) << i for i in range(12))
                    if alt != neu:
                        anders = True
                        BF._bits_schreiben(emr, o, bit, alt)
            geaendert += anders
    emr_rest = bytes(emr) == emr_e
    sek_ok = all(sek[i] for i in range(7) if i != 2) and emr_rest
    gl = all(tuple(mo.skel.rel[b]) == (0, 541, 0) for b in range(17, 23))
    return dict(sektionen=sek, emr_nur_schwanzwinkel=emr_rest, keyframes_angehoben=geaendert, tim=tim == time, meshes_0_15=all(meshes),
                mesh9=meshes[9], glieder_541=gl, ok=bool(sek_ok and tim == time and all(meshes) and gl))


def r8g(debug_7e, debug_neu, state_7e, state_neu, blob, nachlauf=None):
    """Runde 8b (Koordinator "Szenenkette gegen 7e ... Verschiebung muss begruendet oder beseitigt sein"): Schwanzkette der Szene (debug.log
    "[hangar-takt] h.. Schwanz B") gegen den Lauf 7e. Begruendet = (1) state.log bytegleich; (2) jede Abweichung > 1 E. liegt in einem Takt, in dem die
    7e-Kette mit den Mesh-Massen DIESES Assets die Bodenregel des Codes verletzt (schwanz_r41.c glaetten: Gelenk j + radius[j] oder Dornpunkt
    Gelenk 6 + Achse x dorn_y unter y 0; Masse wie sw_mesh_mass), oder im Nachlauf danach (<= NACHLAUF Takte, Abweichung faellt); (3) in den Takten,
    in denen die 7e-Kette verletzt, haelt die neue Kette die Bodenregel ein (Gelenke + Radius, Dornpunkt y <= 0,5)."""
    import re
    nachlauf = R8G_NACHLAUF if nachlauf is None else nachlauf

    def lese(p):
        G = {}
        for l in open(p, encoding='latin-1'):
            m_ = re.match(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', l)
            if m_:
                G[int(m_.group(1))] = np.array([[int(a), int(b_), int(c)] for a, b_, c in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m_.group(3))], float)
        return G
    A, B = lese(debug_7e), lese(debug_neu)
    mo = V.modell(blob)
    rad, dorn = np.zeros(8), 0.0
    for g in range(7):
        P = np.array([v[:3] for v in mo.meshes[16 + g].tv], float)
        r_ = float(np.hypot(P[:, 0], P[:, 2]).max())
        rad[g], rad[g + 1] = max(rad[g], r_), max(rad[g + 1], r_)
        if g == 6:
            dorn = float(P[:, 1].max())

    def verletzt(P):
        dp = P[6] + (P[7] - P[6]) * dorn / 541.0
        return bool((P[2:, 1] + rad[2:] > 0.5).any() or dp[1] > 0.5)
    hs = sorted(set(A) & set(B))
    flag = [h for h in hs if verletzt(A[h])]
    diff = {h: round(float(np.linalg.norm(A[h] - B[h], axis=1).max())) for h in hs}
    anders = [h for h in hs if diff[h] > 1]
    unbegruendet = []
    for h in anders:
        if h in flag:
            continue
        vor = [f for f in flag if 0 < h - f <= nachlauf]
        if not vor:
            unbegruendet.append((h, diff[h])); continue
        f = max(vor)
        if diff[h] > max(diff[f2] for f2 in flag if f2 <= h) + 1:
            unbegruendet.append((h, diff[h]))
    neu_verletzt = [h for h in flag if verletzt(B[h])]
    st = open(state_7e, 'rb').read() == open(state_neu, 'rb').read()
    ok = st and not unbegruendet and not neu_verletzt
    return dict(takte=len(hs), anders=len(anders), max_e=max(diff.values()) if diff else 0, bodenregel_takte=flag, unbegruendet=unbegruendet,
                neue_kette_verletzt=neu_verletzt, state_gleich=st, radius=rad.round(1).tolist(), dorn_y=dorn, nachlauf=nachlauf,
                diff=diff, ok=bool(ok))


def ist_r8(blob):
    mo = V.modell(blob)
    return max(v[1] for v in mo.meshes[22].tv) > 1000


def alles(blob, farbe='b', szene=None):
    D = json.load(open(DATEN))
    bezug = eingang()
    mo = V.modell(blob)
    r = {'sha256': V.sha(blob), 'farbe': farbe}
    r['R8a'], d2 = r8a_r8d2(mo, D)
    r['R8b'], r['R8c'], pv = r8b_r8c(blob, bezug, D)
    d = r8d(blob, farbe, pv, bezug)
    d['vorn'] = d2
    d['ok'] = bool(d['ok1'] and d2['ok'] and d['ok3'])
    r['R8d'] = d
    r['R8e'] = r8e(blob, bezug, pv)
    r['R8f'] = r8f(blob, bezug)
    if szene:                             # (debug_7e, debug_neu, state_7e, state_neu)
        r['R8g'] = r8g(*szene, blob)
    return r


def zeilen(r):
    z = ['Schwanz-Riegel Runde 8 (riegel_schwanz_r8.py) fuer sha256 %s, Farbprobe %s' % (r['sha256'], r['farbe'])]
    g = lambda ok: 'GRUEN' if ok else 'ROT'
    a = r['R8a']['ansichten']
    z.append('R8a Breite vorn (Rollreihe, Bauch zur Kamera, s .1/.3/.5/.7/.9 px; Soll s .3-.7 15-20 und |neu - Vorhersage| <= 4): %s -> %s'
             % ('; '.join('h%s %s (Vorhersage %s, Abw. %s, Video %s, rund 07b6703f %s)' % (h, v['breite'], v['vorhersage'], v['abweichung'], v['video'],
                                                                                         v['rund_07b6703f']) for h, v in a.items()), g(r['R8a']['ok'])))
    b = r['R8b']
    z.append('R8b Seite PV in festen Abstaenden ab dem koerpernahen Ende (sichtbare Schwanz-Bildpunkte, Verfahren Pruefer pr8): Abstand -> Breite '
             'neu %s | vorher %s | Video %s; Abweichung neu - Video an 30-170 %s, Mittel %s (Soll je <= %.0f, Mittel <= %.0f; bei 90 <= 0,9 x vorher) -> %s'
             % (b['breite'], b['bezug'], b['video'], b['abweichung'], b['mittel'], R8B_MAX, R8B_MITTEL, g(b['ok'])))
    c = r['R8c']
    z.append('R8c sichtbare Laenge PV: %.1f px (vorher %.1f, Video %.1f; Soll >= %.0f), tiefster sichtbarer Schwanz-Bildpunkt y %d (vorher %d, Video %d; '
             'Soll >= %d), Nadelspitze y %d (Soll %d) auf der Achse %s -> %s'
             % (c['laenge'], c['bezug'], c['video_laenge'], R8C_LAENGE, c['tief_y'], c['bezug_tief_y'], c['video_tief_y'], R8C_TIEF, c['nadel_y'],
                c['soll_y'], c['auf_achse'], g(c['ok'])))
    d = r['R8d']
    z.append('R8d Fugen: (1) Hochachse Endring und Manschette / Huellradius je Gelenk %s (<= 0,88) %s; (2) von vorn schmaler an den Gelenken im Mittel %s px '
             '(>= 1,0) %s; (3) Fugen (Farbe %s): Band-Flaechen je Glied %s (Soll 0), Fugen-Flaechen je Glied %s, Klassen der Fugen %s, davon mit '
             'Bauchfeld %s, Naht je Gelenk (v Ende | v Anfang | Abstand, <= 1) %s, PV Bauch-Bildpunkte auf Fugen %s, Gelenke damit %d (>= 5); '
             'Flanke: graue Flaechen ausserhalb des einen Grau-Felds %s (Soll 0), Naht grau %s %s -> %s'
             % (d['geometrie'], g(d['ok1']), d['vorn']['mittel_je_ansicht'], g(d['vorn']['ok']), 'mit Ringen (Gegenprobe 8c)' if d['band'] else 'einheitlich',
                d['band_flaechen'], d.get('fugen_flaechen'), d.get('fugen_klassen'), d.get('fugen_bauch'), d.get('naht_v'),
                d.get('fugen_bauch_pv_px'), d.get('gelenke_mit_fugen_bauch_pv', 0), d.get('grau_ausserhalb_feld'), d.get('naht_grau_v'), g(d['ok3']),
                g(d['ok'])))
    for h, v in r['R8a']['ansichten'].items():
        z.append('   R8d (2) h%s Gelenke %s schmaler um %s px' % (h, v['fugen']['gelenke'], v['fugen']['schmaler_px']))
    e = r['R8e']
    z.append('R8e Fleisch nur Bauchseite: (1) %d Bauch-Flaechen (bis Runde 8b: auf 495), ausserhalb des Bauchsektors bzw. graue im Bauchsektor %d %s %s; (2) PV Fleisch-Anteil '
             'bei Normalen-|theta| >= 100: %.2f %% (<= 1), <= 40: %.1f %% (>= 50) %s; (3) Herkunft: Flaechen je Zeile %s, fremde Texel %s, Texturbild '
             'bytegleich %s; (4) Bauch-Texel unter L* %.0f: %d (kleinstes L* %.1f) %s -> %s' % (e['flaechen_495'], e['n_falsch'], e['falsch'][:8], g(e['ok1']),
                                      e['pv_fleisch_aussen'], e['pv_fleisch_innen'], g(e['ok2']), e['zeilen'], e['fremde_texel'],
                                      'ja' if e['pix_gleich'] else 'nein', BAUCH_L_MIN, e['bauch_texel_dunkel'], e['bauch_l_min'], g(e['ok4']), g(e['ok'])))
    f = r['R8f']
    z.append('R8f Bytegleich zum Eingang 07b6703f ausser Meshes 16-22 und den Winkeln der Knochen 17-22 in %d angehobenen Keyframes: Sektionen 0-6 %s '
             '(EMR nach Ruecksetzen dieser Winkel gleich: %s), TIM %s, Meshes 0-15 %s (Mesh 9 %s), Gliedlaengen 541 %s -> %s'
             % (f['keyframes_angehoben'], f['sektionen'], f['emr_nur_schwanzwinkel'], f['tim'], f['meshes_0_15'], f['mesh9'], f['glieder_541'],
                g(f['ok'])))
    keys = ['R8a', 'R8b', 'R8c', 'R8d', 'R8e', 'R8f']
    if 'R8g' in r:
        q = r['R8g']
        keys.append('R8g')
        z.append('R8g Szenenkette gegen 7e: %d Takte, %d anders (groesste Verschiebung %d E.), state.log gleich %s; Bodenregel-Takte der 7e-Kette mit den '
                 'Massen dieses Assets (radius %s, dorn_y %.0f) %s; unbegruendet (nicht Bodenregel, nicht Nachlauf <= %d) %s; neue Kette verletzt %s -> %s'
                 % (q['takte'], q['anders'], q['max_e'], q['state_gleich'], q['radius'], q['dorn_y'], q['bodenregel_takte'], q['nachlauf'],
                    q['unbegruendet'], q['neue_kette_verletzt'], g(q['ok'])))
    rot = [k for k in keys if not r[k]['ok']]
    z.append('SUMME R8: %d von %d gruen%s' % (len(keys) - len(rot), len(keys), ('; ROT: ' + ', '.join(rot)) if rot else ''))
    return z, rot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd', nargs='?', default=V.ASSET)
    ap.add_argument('--farbe', default=Q.STANDARD)
    ap.add_argument('--aus'); ap.add_argument('--json')
    ap.add_argument('--szene', nargs=4, metavar=('DEBUG_7E', 'DEBUG_NEU', 'STATE_7E', 'STATE_NEU'), help='R8g: Szenenlaeufe 7e und neu')
    ap.add_argument('--nachlauf', type=int, default=None)
    a = ap.parse_args()
    r = alles(open(a.emd, 'rb').read(), a.farbe, a.szene)
    if a.nachlauf is not None and 'R8g' in r:
        r['R8g'] = r8g(*a.szene, open(a.emd, 'rb').read(), nachlauf=a.nachlauf)
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
