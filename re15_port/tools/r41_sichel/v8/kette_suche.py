"""SF V8: Suche der Ruhekette des Schwanzes in PV gegen die Nutzerlinie T und den Uebergang J -> kette_v8.json (Messblatt V8 6a).

    python kette_suche.py [--basis EMD] [--ruecken JSON] [--aus JSON]

Nutzer 2026-11-08 (Skizze Darstellung.png): "tail area 1", "tail area 2" (Aussenkontur T), "Tail directly smooth into the back - no
hill or anything..." (J). Frueher: "vom ruecken aus geschlossen ein zusammenhaengendes stueck Das direkt I. den Ruecken uebergeht".
Bauplan Messblatt V8 6a: Wurzel auf der Rueckenmitte (Seitenachse z ~ 0), Oberkante des Wurzelrings buendig mit der Ruecken-Oberkante,
Radien Wurzel 420-480 E., s 90-130 ~305 E., s 140 250 E., s 150-160 90 E., Ende 60 E.; Ruheverlauf entlang der Vorlage-Mittellinie,
Biegung je Gelenk hoechstens 25 Grad, ohne Umkehr; alle 7 Glieder gleich lang.

Modell der Suche (deterministisch, Gleitkomma): Kette = Wurzel P0 (Knochenraum 8) + 7 Glieder gleicher Laenge L; Richtung des ersten
Glieds (Gier/Nick im Knochenraum 8) und 6 Biegungen (Winkel 25 Grad x tanh(u), Achse um die Gliedrichtung gedreht). Bild = Kreise
entlang der Kette (12 je Glied + Wurzelrohr B x r0 hinter der Wurzel, B frei 0,45-B_MAX) mit dem Bildradius r x Massstab am Ort
(Projektion b2.projiziere, Pose PV). Rumpf (Knochen 0-15) = Umriss des Basis-EMD mit der Rueckenstufe (ruecken_v8.json).
Residuen (scipy least_squares, Verlust soft_l1, f_scale 2 px):
  S1  Aussenkante (erste Kreisberuehrung bzw. Rumpf) entlang der 22 T-Normalen minus Vorlage-Kante (px)
  S2a 0,5 x (Oberkontur (Kette u Rumpf) minus Vorlage-Maske) je Spalte x 330-445
  S2c Naht: rechteste Spalte x 330-420, in der die Schwanz-Oberkante oben liegt; je Spalte Naht +- 15: (Rumpf ueber Schwanz - 2 px)+
  Ende (Spitze im Bild - (283,334)); Wurzel (Wurzelmitte im Bild - J-Mitte (366,215)) (Messblatt 3.1)
  S4  (56 px - Wurzeldurchmesser)+, (37 px - kleinster Durchmesser bei s 90-130)+
  Rueckenmitte 0,5 x z / 300 je Gelenk; Boden 0,01 x (Modell-y + 300)+ je Gelenk
Radien: je Gelenk aus dem Bauplan-Profil (PROFIL_S/PROFIL_R) an seiner Bogenlaenge auf T, geglaettet (monoton, Steigungsknick <= 15 Grad).
Start: Raster ueber Gier/Nick (15 / 10 Grad) nach Summe r^2, dann 3 x least_squares. Kein RE-Wert; das Ergebnis ist PORT-WAHL.
"""
import os, sys, json, argparse
import numpy as np
from scipy.optimize import least_squares
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import kopf_orig_bau as KB   # noqa: E402
import ruecken_v8 as RV8     # noqa: E402
import v8_bau as VB          # noqa: E402

# Bauplan Messblatt V8 6a: Radius (E.) ueber der Bogenlaenge s (px) entlang T: Wurzel 420-480 (Mitte 450), s 90-130 ~305, s 140 250,
# s 150-160 90, Ende 60
PROFIL_S = [0.0, 90.0, 130.0, 140.0, 155.0, 160.0, 200.0]
PROFIL_R = [450.0, 350.0, 350.0, 250.0, 90.0, 90.0, 60.0]   # s 90-130: 350 statt ~305 (Lauf ks10/t7: 305 E. ergaben in PV nur
                                                            # 34,7-37,5 px, die Glieder liegen tiefer im Bild als der Koerper; S4 >= 36)
KNICK_MAX = 15.0          # Grad, Steigungsknick des Mantels (Regel V5f, Bauplan 6a)
WURZEL_BILD = (366.0, 215.0)   # Messblatt 3.1: Wurzelmitte an J ~(366,215)
WINKEL_MAX = np.radians(25.0)  # Bauplan 6a: je Gelenk hoechstens 25 Grad
ENDE_BILD = (283.0, 334.0)     # Messblatt 3.1 / 6a: Ende ~(283,330), Linie T1 endet bei y 333 (Station s 210 liegt bei y 324)
B_MAX = float(os.environ.get('V8_B_MAX', '1.5'))
B_MIN = float(os.environ.get('V8_B_MIN', '0.2'))    # Runde 2 (Riegel S2c, Sicht-Pruefer (5)): kurzes Rohr erlaubt (Runde 1: 0,45) - weniger verdeckte
                                                    # Geometrie im Rumpf (Maler-Verfahren ohne Tiefenpuffer, render_pc.c) und kein Rohrrand unter dem Buckel   # Wurzelrohr hoechstens 1,5 r0 (verjuengt, v8_bau.wurzel_mesh)
KREISE_JE_GLIED = 12
SEITE_W = float(os.environ.get("V8_SEITE_W", "2.0"))
EBEN = os.environ.get('V8_EBEN', '1') == '1'           # Runde 2: ebener gleichsinniger Bogen (siehe kette); 0 = Runde-1-Modell
S2_W = float(os.environ.get('V8_S2_W', '1.0'))         # Runde 2: Gewicht S2a (Runde 1: 0,5) und Sattel-Term S2b (Zahlen-Pruefer V8 r1: S2 rot)   # Gewicht Rueckenmitte (Lauf ks5 mit 0,5: Gelenke bis z -1300 = Kette an der (W)-rechten Flanke)


def rotv(v, ax, a):
    ax = ax / np.linalg.norm(ax)
    return v * np.cos(a) + np.cross(ax, v) * np.sin(a) + ax * (ax @ v) * (1 - np.cos(a))


def radien_glatt(ziel, L):
    """Radienfolge nahe ziel, monoton fallend, Steigungsknick <= KNICK_MAX: von der Wurzel zur Spitze wird r_j in den erlaubten Bereich
    [r_j-1 - L tan(n_j-1 + 15 Grad), r_j-1 - L tan(max(n_j-1 - 15 Grad, 0))] geklemmt (n = Neigung des Mantels, erstes Glied gegen
    0 Grad, deterministisch)."""
    r = [float(ziel[0])]
    n_alt = 0.0
    for j in range(1, len(ziel)):
        lo = r[-1] - L * np.tan(np.radians(min(n_alt + KNICK_MAX, 80.0)))
        hi = r[-1] - L * np.tan(np.radians(max(n_alt - KNICK_MAX, 0.0)))
        rj = float(np.clip(ziel[j], lo, hi))
        n_alt = float(np.degrees(np.arctan((r[-1] - rj) / L)))
        r.append(rj)
    return np.maximum(np.array(r), 1.0)


class Suche:
    def __init__(self, blob, ruecken=None):
        d, _o, ms, tim = KB.teile(blob)
        if ruecken:
            RV8.ruecken(ms, ruecken, {})
            blob = KB.packen(blob, d, ms, tim)
        self.mo = V.modell(blob)
        R, T = V.pose_r_t(self.mo)
        self.R8, self.T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
        self.C = V.kamera_modell()
        self.rumpf = V.b2.umriss_schnell(self.mo, V.PV, knochen=list(range(16)))
        self.st = V.stationen()
        self.Pst = np.array([s['p'] for s in self.st]); self.Nst = np.array([s['n'] for s in self.st])
        self.Zst = np.array([s['ziel'] for s in self.st])
        kb = V.aussenkante(self.rumpf, self.st)
        self.k_rumpf = np.array([70.0 if k is None else float(k) for k in kb])
        self.ov = {int(k): v for k, v in V.lade('m13b_oben.json')['oben_vorlage_maske'].items()}
        self.xs = np.arange(300, 461)
        self.top_r = np.array([np.nonzero(self.rumpf[:, x])[0].min() if self.rumpf[:, x].any() else 999 for x in self.xs], float)

    # Kette
    def kette(self, x):
        P0 = np.array(x[0:3])
        yaw, pit = x[3], x[4]
        d = np.array([np.cos(pit) * np.cos(yaw), np.sin(pit), np.cos(pit) * np.sin(yaw)])
        L = x[17]
        pts, dirs = [P0], [d]
        if EBEN:
            # Runde 2 (Zahlen-Pruefer V8 r1 "Bauplan 6a ohne Umkehr": Bild-Drehwinkel -22,2/-8,8/-25,9/+8,3/-20,4/-15,0, Biegeebene kehrt am
            # Gelenk 20 um): ebener Bogen - EINE Biegeachse (senkrecht zur Ebene durch die erste Gliedrichtung, Rollwinkel x[6]), alle 6
            # Biegungen gleichsinnig 0..25 Grad (WINKEL_MAX x (1 + tanh) / 2). Ein ebener, gleichsinnig gekruemmter Bogen projiziert sich
            # ohne Wendepunkt.
            e1 = np.cross(d, [0, 0, 1.0])
            e1 /= np.linalg.norm(e1)
            ax = rotv(e1, d, x[6])
            for j in range(6):
                a = WINKEL_MAX * 0.5 * (1.0 + np.tanh(x[5 + 2 * j]))
                pts.append(pts[-1] + d * L); d = rotv(d, ax, a); dirs.append(d)
            pts.append(pts[-1] + d * L)
            return np.array(pts), np.array(dirs)
        for j in range(6):
            a = WINKEL_MAX * np.tanh(x[5 + 2 * j]); phi = x[6 + 2 * j]
            e1 = np.cross(d, [0, 0, 1.0])
            e1 /= np.linalg.norm(e1)
            ax = rotv(e1, d, phi)
            pts.append(pts[-1] + d * L); d = rotv(d, ax, a); dirs.append(d)
        pts.append(pts[-1] + d * L)
        return np.array(pts), np.array(dirs)

    def s_auf_T(self, q):
        """Bildpunkte -> Bogenlaenge s (px) entlang T (Stationen alle 10 px), stetig (Lotfusspunkt auf dem Polygonzug)."""
        P, sv = self.Pst, np.array([s['s'] for s in self.st], float)
        out = []
        for p in np.atleast_2d(q):
            best = (1e18, 0.0)
            for i in range(len(P) - 1):
                a, b = P[i], P[i + 1]
                t = float(np.clip((p - a) @ (b - a) / ((b - a) @ (b - a)), 0, 1))
                dd = float(np.sum((a + t * (b - a) - p) ** 2))
                if dd < best[0]:
                    best = (dd, sv[i] + t * (sv[i + 1] - sv[i]))
            out.append(best[1])
        return np.array(out)

    def radien(self, x, q_gelenke=None):
        """Radius je Gelenk aus dem Bauplan-Profil (Messblatt 6a) an der Bogenlaenge s des Gelenks auf T, dann Steigungsknick <= 15 Grad
        und monoton (radien_glatt)."""
        if q_gelenke is None:
            pts, _d = self.kette(x)
            q_gelenke = V.proj(V.PV, pts @ self.R8.T + self.T8)
        sj = self.s_auf_T(q_gelenke)
        sj[0] = min(sj[0], 0.0)
        ziel = np.interp(sj, PROFIL_S, PROFIL_R)
        if len(x) > 22:                      # Runde 2: Wurzelradius frei im Bauplan-Band 420-480 E., Gelenk 17 frei bis zum Wurzelradius
            ziel[0] = float(np.clip(x[21], 420.0, 480.0))
            ziel[1] = float(np.clip(x[22], ziel[2], ziel[0]))
        return radien_glatt(ziel, float(x[17]))

    def kreise(self, x):
        pts, dirs = self.kette(x)
        rad = self.radien(x, V.proj(V.PV, pts @ self.R8.T + self.T8))
        C, Rr = [], []
        r0 = rad[0]
        B = float(np.clip(x[18], B_MIN, B_MAX))
        rb = VB.WURZEL_VERJUENGUNG * r0
        d0 = dirs[0]
        rg = float(x[19]) if len(x) > 19 else 0.0        # Runde 2: Rohrknick (v8_bau.rohr_drehung), Grad
        if rg:
            nb = np.cross(dirs[0], dirs[1])
            if np.linalg.norm(nb) > 1e-9:
                d0 = rotv(dirs[0], nb, np.radians(rg))
        if len(x) > 20:
            rb = float(np.clip(x[20], 0.3, 1.0)) * r0       # Runde 2: Verjuengung des Rohrs frei 0,3-1,0 (Runde 1 fest 0,6)
        for t in np.linspace(0, 1, 10):       # Wurzelrohr im Rumpf (v8_bau.wurzel_mesh): r0 -> rb ueber B r0, dann Kegel
            C.append(pts[0] - d0 * B * r0 * t); Rr.append(r0 + (rb - r0) * t)
        C.append(pts[0] - d0 * (B * r0 + 0.5 * VB.WURZEL_DECKEL * rb)); Rr.append(0.5 * rb)
        for g in range(7):
            for t in np.linspace(0, 1, KREISE_JE_GLIED, endpoint=False):
                C.append(pts[g] + (pts[g + 1] - pts[g]) * t); Rr.append(rad[g] + (rad[g + 1] - rad[g]) * t)
        C.append(pts[7]); Rr.append(rad[7])
        C = np.array(C); Rr = np.array(Rr)
        Pm = C @ self.R8.T + self.T8
        q = V.proj(V.PV, Pm)
        v = Pm - self.C
        e = np.cross(v, [0, 1.0, 0]); e /= np.linalg.norm(e, axis=1)[:, None]
        q2 = V.proj(V.PV, Pm + 100 * e)
        s = np.linalg.norm(q2 - q, axis=1) / 100.0
        return q, Rr * s, pts, Pm, rad

    def kanten(self, q, rp):
        dc = q[None, :, :] - self.Pst[:, None, :]                     # 22 x K x 2
        t = np.einsum('ikj,ij->ik', dc, self.Nst)
        perp2 = np.einsum('ikj,ikj->ik', dc, dc) - t ** 2
        h2 = rp[None, :] ** 2 - perp2
        o = np.where(h2 >= 0, t - np.sqrt(np.maximum(h2, 0)), 1e9)
        o = np.where(o < -30, np.where(t + np.sqrt(np.maximum(h2, 0)) >= -30, -30.0, 1e9), o)
        return np.minimum(o.min(1), self.k_rumpf)

    def top(self, q, rp):
        dx = self.xs[:, None] - q[None, :, 0]
        h2 = rp[None, :] ** 2 - dx ** 2
        y = np.where(h2 >= 0, q[None, :, 1] - np.sqrt(np.maximum(h2, 0)), 999.0)
        return y.min(1)

    def naht(self, ts):
        """S2c (Messblatt 7, wie riegel_v8.s2c): Naht = rechteste Spalte x 330-420, in der die Schwanz-Oberkante (Schwanz allein)
        nicht unter der Rumpf-Oberkante liegt; Fenster Naht +- 15 Spalten, Hoehe des Rumpfs ueber der Schwanz-Oberkante."""
        oben = (self.xs >= 330) & (self.xs <= 420) & (ts < 900) & (ts <= self.top_r)
        if not oben.any():
            return 330, np.zeros(31)
        xn = int(self.xs[oben].max())
        h = np.zeros(31)
        for i, x_ in enumerate(range(xn - 15, xn + 16)):
            k = x_ - int(self.xs[0])
            if 0 <= k < len(self.xs) and ts[k] < 900 and self.top_r[k] < 900:
                h[i] = ts[k] - self.top_r[k]
        return xn, h

    def sattel(self, tu):
        """S2b wie riegel_v8.sattel: groesste Tiefe der Oberkontur (Schwanz u Rumpf) unter ihrer oberen Huelle, x 300-420 (stetig)."""
        sel = (self.xs >= 300) & (self.xs <= 420) & (tu < 900)
        xs, ys = self.xs[sel].astype(float), tu[sel]
        pts = sorted(zip(xs, -ys)); H = []
        for p in pts:
            while len(H) >= 2 and (H[-1][0] - H[-2][0]) * (p[1] - H[-2][1]) - (H[-1][1] - H[-2][1]) * (p[0] - H[-2][0]) >= 0:
                H.pop()
            H.append(p)
        hull = np.interp(xs, [h[0] for h in H], [-h[1] for h in H])
        return float((ys - hull).max())

    def residuen(self, x, detail=False):
        q, rp, pts, Pm, rad = self.kreise(x)
        e = self.kanten(q, rp)
        d = e - self.Zst
        ts = self.top(q, rp)
        tu = np.minimum(ts, self.top_r)
        sel = (self.xs >= 330) & (self.xs <= 445)
        da = tu[sel] - np.array([self.ov[x_] for x_ in self.xs[sel]])
        xn, hn = self.naht(ts)
        sv = np.array([s['s'] for s in self.st])
        near = sv[np.argmin(np.linalg.norm(q[:, None, :] - self.Pst[None, :, :], axis=2), 1)]
        m = (near >= 90) & (near <= 130)
        dw = 2 * rp[11]
        dm = 2 * rp[m].min() if m.any() else 0.0
        r = np.concatenate([
            d,                                                           # S1
            (S2_W if EBEN else 0.5) * da,                                # S2a (116 Spalten)
            ([3.0 * max(0.0, self.sattel(tu) - 1.5), 2.0 * max(0.0, float(np.abs(da[:56]).max()) - 5.0)] if EBEN else []),  # S2b, S2a-Max
            np.maximum(0.0, hn - 2.0),                                  # S2c
            1.0 * (q[-1] - np.array(ENDE_BILD)),                        # Ende
            1.0 * (q[11] - np.array(WURZEL_BILD)),                      # Wurzel an J
            [max(0.0, 56.0 - dw), max(0.0, 37.0 - dm)],                 # S4
            SEITE_W * pts[:, 2] / 300.0,                                 # Rueckenmitte
            0.01 * np.maximum(0, (pts @ self.R8.T + self.T8)[:, 1] + 300)])
        if detail:
            return dict(r=r, d=d, s2a=da, naht=xn, hn=hn, wurzel_px=dw, mitte_px=dm, q=q, rp=rp, pts=pts, rad=rad)
        return r

    def kosten(self, x, detail=False):
        D = self.residuen(x, True)
        c = float(np.sum(D['r'] ** 2))
        if detail:
            D['kosten'] = c
            D['hj'] = float(D['hn'].max()) if len(D['hn']) else 0.0
            return D
        return c


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=V.ASSET)
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--aus', default=os.path.join(HIER, 'kette_v8.json'))
    ap.add_argument('--start', help='JSON mit x eines frueheren Laufs (statt Raster)')
    a = ap.parse_args()
    blob = open(a.basis, 'rb').read()
    S = Suche(blob, json.load(open(a.ruecken)) if a.ruecken and os.path.exists(a.ruecken) else None)
    x0 = np.array([-851.0, -108.0, 0.0, 0.0, 0.0] + [0.5, 0.0] * 6 + [500.0, 1.25])
    best = None
    if a.start:
        x = np.array(json.load(open(a.start))['x'], float)
        if len(x) < 19:
            x = np.r_[x, 1.25]
        best = (S.kosten(x), x)
    for yw in (np.radians(np.arange(0, 360, 15)) if not a.start else []):
        for pt in np.radians(np.arange(-80, 81, 10)):
            for ro in (np.radians(np.arange(0, 360, 30)) if EBEN else [0.0]):
                x = x0.copy(); x[3] = yw; x[4] = pt
                if EBEN:
                    x[6] = ro
                c = S.kosten(x)
                if best is None or c < best[0]:
                    best = (c, x)
    x = best[1]
    print('Raster %.3f Gier %.0f Nick %.0f' % (best[0], np.degrees(x[3]), np.degrees(x[4])))
    skal = np.array([50, 50, 50, 0.05, 0.05] + [0.1, 0.1] * 6 + [20, 0.1])
    for i in range(3):
        r = least_squares(S.residuen, x, x_scale=skal, loss='soft_l1', f_scale=2.0, diff_step=1e-3, max_nfev=4000)
        x = r.x
        print('least_squares %d: Summe r^2 %.3f (%s)' % (i, S.kosten(x), r.message))
    D = S.kosten(x, True)
    pts, rad = D['pts'], D['rad']
    L = float(x[17])
    out = dict(quelle='kette_suche.py (Basis %s sha256 %s, Ruecken %s, Start %s)' % (os.path.basename(a.basis), V.sha(blob),
                                                                                    os.path.basename(a.ruecken), a.start and os.path.basename(a.start)),
               x=[float(v) for v in x], gelenke_b8=[[round(float(c), 1) for c in p] for p in pts], L=int(round(L)),
               radien=[round(float(r), 1) for r in rad], kosten=round(float(D['kosten']), 4),
               s1_d=[round(float(v), 2) for v in D['d']], s1_mittel=round(float(np.mean(np.abs(D['d']))), 2),
               s1_max=round(float(np.max(np.abs(D['d']))), 2), s2a=[round(float(v), 1) for v in D['s2a']], j_huckel=round(D['hj'], 1),
               wurzel_px=round(float(D['wurzel_px']), 1), bild=[[round(float(c), 1) for c in qq] for qq in D['q'][11::KREISE_JE_GLIED]],
               hinten=round(float(np.clip(x[18], B_MIN, B_MAX)), 3),
               eben=EBEN, s2b_sattel=round(S.sattel(np.minimum(S.top(D['q'], D['rp']), S.top_r)), 2),
               biegung_grad=[round(float(np.degrees(WINKEL_MAX * (0.5 * (1.0 + np.tanh(x[5 + 2 * j])) if EBEN else np.tanh(x[5 + 2 * j])))), 2)
                             for j in range(6)])
    json.dump(out, open(a.aus, 'w'), indent=1)
    print('S1 Mittel %.2f Max %.2f, Naht-Huckel %.1f, Wurzel %.1f px, L %d, Radien %s' % (out['s1_mittel'], out['s1_max'], out['j_huckel'],
                                                                                     out['wurzel_px'], out['L'], out['radien']))
    print('Biegung', out['biegung_grad'])


if __name__ == '__main__':
    main()
