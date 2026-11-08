"""SF V8 Koerperbau nach Nutzer-Skizze (Bauer, 2026-11-08): baut EM30_V8.EMD aus V7 (Spiel-Asset v0.8.30, sha256 319bbd17).

    python v8_bau.py [--basis EMD] [--aus DIR] [--name NAME] [--json PROT] [--stufen sra]

Nutzer 2026-11-08 abends (woertlich, bindend):
  (A) "Das Gesicht vom 2 beinigen Birkin ist jetzt schon gut, aber mit Schwanz und sowas bin ich immer noch 0 zufrieden.... Ich verstehe
      das auch nicht. Es ist doch eigentlich KLAR erkennbar. Siehe Darstellung.png und Darstellung_Vorlage.png. Man sieht ganz klar den
      Körper Aufbau...."
  (B) "und wenn du schon dabei bist, am überarbeiten, man sieht ebenfalls beim vorlage Bild, das die Rechte Seite auch mehr rottöne hat,
      also quasi mehr adern an verschiedenen Stellen, am Arm und Körper, nicht nur grau. Übernehme die die Textur Sachen die du dafür
      brauchst vom originalen Birkin von Resident Evil 1.5"
  Skizze Version_80%/Darstellung.png: "tail area 1" / "tail area 2" (Linie T), "Tail directly smooth into the back - no hill or
  anything..." (J), "Little red hill from the back" (H), "smooth straight to head without further hills...." (K).
Messblatt: analysis/befunde_runde41/SF_MESSBLATT_V8.md (Commit 09b04bc1c), Bauplan Abschnitt 6.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Jede Zahl ist PORT-WAHL, belegt mit
Nutzer-Wortlaut oder Messung (Messblatt V8 bzw. kette_suche.py / ruecken_suche.py, Protokolle unter SF_belege/v8/bau).

Stufe s) Schwanz (Messblatt 6a): Ruhekette aus kette_v8.json (kette_suche.py: Wurzel und Gelenke im Knochenraum 8, Gliedlaenge L fuer
   alle 7 Glieder gleich, Radien je Gelenk). EMR Bank 0: relpos Knochen 16 = Wurzel, 17-22 = (0, L, 0); Keyframe-Drehungen der Knochen
   16-22 = Ruhe-Lokaldrehung x (Bewegung des Keyframes gegen den Clip-Median, aus V7), so gesetzt, dass PV genau die Suchkette zeigt;
   danach je Keyframe die Bodenregel (Knochen 16 um eine waagerechte Achse hochdrehen, bis Ringunterkanten, Spitze und Dornspitze
   ueber der tiefsten Koerperecke liegen). Knochen 0-15 und alle anderen Bits bytegleich. Meshes 16-22 neu aus den Radien (Ringe auf der
   Gehrung der Ruhekette, Manschette 0,45 r, Deckel wie V6, Dorn 0,8 r); Flaechen, UV, CLUT-Zeile 487 und Eckenzahl bleiben.
Stufe r) Ruecken (Messblatt 6b): Ecken von Mesh 8 aus ruecken_v8.json (ruecken_suche.py).
Stufe a) Adern (Messblatt 6c): Zeilen X1/X2 = V7-Zeile 480/481 + die Original-EM30-Ader-Eintraege derselben Zeile (a* >= 15,
   L* <= 38, bytegleich); Flaechen nach adern_v8.json (adern_suche.py).
"""
import os, sys, json, argparse, struct, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402
import emd_ansichtsblatt as EA        # noqa: E402

S_ = 11                               # Ecken je Ring (V4-Generator, schwanz_form.S_)
UEBERLAPP = 0.45                      # Manschette (V5f schwanz_form.UEBERLAPP, unveraendert)
DORN = 0.8                            # Dorn/Deckel 0,8 r (V5f schwanz_form.DORN, unveraendert)
WURZEL_VERJUENGUNG = 0.6              # V8: Wurzelrohr verjuengt sich im Rumpf auf 0,6 r0 (Laeufe sfv8/bau t5/t6: gerades Rohr ragte
WURZEL_DECKEL = 0.3                   #     vorn ueber die Schulter); Kegel 0,3 x Radius hinter dem Rohrende. PORT-WAHL, kette_suche.py rechnet mit.
BAUCH_T = 296.0 / 887.0               # Mesh 18 Zwischenring: Lage wie V7 (Ring 1 bei y 296 von 887), jetzt ohne Wulst (linear)
Y = np.array([0.0, 1.0, 0.0])


def _p(name):
    return json.load(open(os.path.join(HIER, name)))


# ------------------------------------------------------------------ Rotationen
def lokal_aus_kf(skel, kf, b):
    return EA.mat_euler(*EA.kf_angles(skel, kf, b)).astype(float) / 4096.0


def winkel_schreiben(emr, skel, kf, b, L):
    o = EA.u16(emr, 2) + kf * skel.kf_size + 12
    for j, w in enumerate(BF._zerlege(L)):
        BF._bits_schreiben(emr, o, b * 36 + j * 12, w & 0xFFF)


def ruhe_lokal(P):
    """Gelenkpunkte (Knochenraum 8) -> quantisierte Lokaldrehungen 16..22 (kleinste Drehung von y auf das Glied, wie kette_ruhe)."""
    lok, W = [], np.eye(3)
    for i in range(7):
        d = W.T @ (P[i + 1] - P[i])
        Li = BF._quant(BF._min_rot(Y, d))
        lok.append(Li)
        W = W @ Li
    return lok


def clip_je_kf(mo):
    """Keyframe -> erster Clip, der ihn benutzt (EDD), sonst -1."""
    c_von = {}
    for c, (f0, n) in enumerate(mo.clips):
        for i in range(n):
            c_von.setdefault(mo.frames[f0 + i] & 0xFFF, c)
    return c_von


def bewegung_v7(mo):
    """Je Keyframe und Glied: Drehvektor der V7-Lokaldrehung gegen den Clip-Median (komponentenweiser Median der Drehvektoren)."""
    s = mo.skel
    c_von = clip_je_kf(mo)
    logs = np.array([[BF._log(lokal_aus_kf(s, k, b)) for b in V.SCHWANZ] for k in range(s.kf_count)])
    med = {}
    for c in set(c_von.values()):
        ks = [k for k, cc in c_von.items() if cc == c]
        med[c] = np.median(logs[ks], 0)
    w = np.zeros((s.kf_count, 7, 3))
    for k in range(s.kf_count):
        c = c_von.get(k, -1)
        if c < 0:
            continue
        for i, b in enumerate(V.SCHWANZ):
            Lm = BF._exp(med[c][i])
            w[k, i] = BF._log(Lm.T @ lokal_aus_kf(s, k, b))
    return w, c_von


def tiefste_koerperecke(mo, R, T):
    y = -1e9
    for b in range(16):
        m = mo.meshes[b]
        if not m.tv:
            continue
        P = np.array([v[:3] for v in m.tv], float)
        y = max(y, float(((P @ (R[b].astype(float) / 4096.0).T) + T[b].astype(float))[:, 1].max()))
    return y


def schwanz_tief(mo, R, T, rad, dorn_y):
    """Tiefster Punkt des Schwanzes: Gelenke 16..22 + Radius, Spitze (Knochen 22 + L) und Dornspitze (Knochen 22 + dorn_y)."""
    y = -1e9
    L = float(mo.skel.rel[22][1])
    for i, b in enumerate(V.SCHWANZ):
        y = max(y, float(T[b][1]) + rad[i])
    ax = R[22].astype(float)[:, 1] / 4096.0
    y = max(y, float(T[22][1] + ax[1] * L) + rad[7], float(T[22][1] + ax[1] * dorn_y))
    return y


def kette_punkte(R8, T8, loks, rel, L):
    """Gelenke 16..22 + Spitze im Modellraum aus Knochen 8 und den Lokaldrehungen (Gleitkomma, wie EA.Modell.pose)."""
    W, P = R8, T8 + R8 @ rel[0]
    out = []
    for i in range(7):
        W = W @ loks[i]
        out.append(P)
        P = P + W @ (rel[i + 1] if i < 6 else np.array([0.0, float(L), 0.0]))
    out.append(P)
    return np.array(out), W


def kette_tief(R8, T8, loks, rel, L, rad, dorn_y):
    (pts, W) = kette_punkte(R8, T8, loks, rel, L)
    y = max(float(p[1]) + r for p, r in zip(pts, rad))
    return max(y, float(pts[6][1] + (W @ Y)[1] * dorn_y))


# ------------------------------------------------------------------ Meshes
def glied_mesh(m_alt, g, L, ra, re, n_anf, n_end, hinten=UEBERLAPP):
    """Mesh 16+g neu: Ringe (Winkel je Ecke wie V7), Ende auf der Gehrungsebene (Normale n_end, Gliedraum), Anfang (Manschette)
    auf der um UEBERLAPP x ra zurueckgesetzten Gehrungsebene n_anf; Deckel-/Kappenecken auf den Ringmitten; Normalen radial."""
    m = copy.deepcopy(m_alt)
    P = np.array([v[:3] for v in m['v']], float)
    nr = 3 if g == 2 else 2
    phis = []
    for r in range(nr):
        Q = P[r * S_:(r + 1) * S_]
        c = Q.mean(0)
        phis.append(np.arctan2(Q[:, 2] - c[2], Q[:, 0] - c[0]))
    neigung = (ra - re) / L

    def ring(cy, rr, n, phi):
        off = np.stack([np.cos(phi) * rr, np.zeros_like(phi), np.sin(phi) * rr], 1)
        t = -(off @ n) / n[1] if n is not None else 0.0
        return off + np.array([0.0, cy, 0.0]) + (t[:, None] * Y if n is not None else 0.0)

    ringe = []
    ringe.append(ring(-(hinten if g == 0 else UEBERLAPP) * ra, ra, n_anf, phis[0]))
    if nr == 3:
        rb = ra + BAUCH_T * (re - ra)
        ringe.append(ring(BAUCH_T * L, rb, None, phis[1]))
    ringe.append(ring(L, re, n_end, phis[-1]))
    neu = np.vstack(ringe)
    nn = []
    for r in range(nr):
        ph = phis[r]
        v = np.stack([np.cos(ph), np.full_like(ph, neigung), np.sin(ph)], 1)
        v /= np.linalg.norm(v, axis=1)[:, None]
        nn.append(v)
    nn = np.vstack(nn)
    for k in range(nr * S_):
        m['v'][k][:3] = [int(round(x)) for x in neu[k]]
        m['n'][k][:3] = [int(round(x * 4096)) for x in nn[k]]
    # Zusatzecken (V6 kappen-Reihenfolge): 16 = [Deckel-Apex, Endkappe], 17-21 = [Endkappe, Anfangskappe], 22 = [Spitze, Anfangskappe]
    ex = list(range(nr * S_, len(m['v'])))
    c_anf = ringe[0].mean(0)
    c_end = ringe[-1].mean(0)
    for i in ex:
        y_alt = m['v'][i][1]
        if g == 0 and y_alt < 0:                 # Wurzel: Rohr B x r0 hinter der Wurzel, Deckel 0,2 r0 gewoelbt (im Rumpf)
            q = np.array([0.0, -hinten * ra - DECKEL_WURZEL * ra, 0.0])
        elif g == 6 and y_alt > 0:
            q = np.array([0.0, L + DORN * re, 0.0])
        elif y_alt > 0:
            q = c_end
        else:
            q = c_anf
        m['v'][i][:3] = [int(round(x)) for x in q]
    return m


def wurzel_mesh(m18, L, ra, re, n_end, hinten):
    """Mesh 16 (V8): Topologie von Mesh 18 (3 Ringe, Endkappe, Anfangskappe; Flaechen/UV/Zeile 487 wie Mesh 18) - Ring 0 = Ende des
    Wurzelrohrs im Rumpf (B x r0 hinter der Wurzel, Radius WURZEL_VERJUENGUNG x r0, senkrecht), Ring 1 = Wurzelring an der Wurzel (r0,
    senkrecht), Ring 2 = Ende auf der Gehrung; Anfangskappe = Kegel WURZEL_DECKEL x Radius hinter Ring 0. Kette_suche.py rechnet dieselbe
    Form (Kreise). Grund (Lauf sfv8/bau t5/t6): ein gerades Rohr unter dem Buckel ragte in der Ansicht von vorn ueber die Schulter."""
    m = copy.deepcopy(m18)
    P = np.array([v[:3] for v in m['v']], float)
    phis = []
    for r in range(3):
        Q = P[r * S_:(r + 1) * S_]
        c = Q.mean(0)
        phis.append(np.arctan2(Q[:, 2] - c[2], Q[:, 0] - c[0]))
    rb = WURZEL_VERJUENGUNG * ra
    yb = -hinten * ra
    neig = [(rb - ra) / (hinten * ra), (ra - re) / L]

    def ring(cy, rr, n, phi):
        off = np.stack([np.cos(phi) * rr, np.zeros_like(phi), np.sin(phi) * rr], 1)
        t = -(off @ n) / n[1] if n is not None else 0.0
        return off + np.array([0.0, cy, 0.0]) + (t[:, None] * Y if n is not None else 0.0)

    ringe = [ring(yb, rb, None, phis[0]), ring(0.0, ra, None, phis[1]), ring(L, re, n_end, phis[2])]
    neu = np.vstack(ringe)
    nn = []
    for r, ng in zip(range(3), (neig[0], 0.5 * (neig[0] + neig[1]), neig[1])):
        v = np.stack([np.cos(phis[r]), np.full_like(phis[r], ng), np.sin(phis[r])], 1)
        nn.append(v / np.linalg.norm(v, axis=1)[:, None])
    nn = np.vstack(nn)
    for k in range(3 * S_):
        m['v'][k][:3] = [int(round(x)) for x in neu[k]]
        m['n'][k][:3] = [int(round(x * 4096)) for x in nn[k]]
    for i in range(3 * S_, len(m['v'])):
        if m['v'][i][1] > 0:
            m['v'][i][:3] = [int(round(x)) for x in ringe[2].mean(0)]
        else:
            m['v'][i][:3] = [0, int(round(yb - WURZEL_DECKEL * rb)), 0]
            m['n'][i][:3] = [0, -4096, 0]
    return m


def gehrungen(lok):
    """Gehrungsnormale je Gelenk im Raum des Glieds davor (n_end von Glied g) und des Glieds danach (n_anf von Glied g + 1)."""
    n_end, n_anf = [None] * 7, [None] * 7
    for g in range(6):
        d = lok[g + 1] @ Y                      # Richtung Glied g + 1 im Raum von Glied g
        n = Y + d
        n /= np.linalg.norm(n)
        n_end[g] = n
        n_anf[g + 1] = lok[g + 1].T @ n         # dieselbe Ebene im Raum von Glied g + 1
    return n_end, n_anf


# ------------------------------------------------------------------ Stufe s
def schwanz(blob, d, ms, par, prot):
    mo = V.modell(blob)
    s = mo.skel
    assert s.bone_count == 23 and s.kf_size == 116, (s.bone_count, s.kf_size)
    P = np.array(par['gelenke_b8'], float)
    L = int(par['L'])
    rad = [float(r) for r in par['radien']]
    lok_fit = ruhe_lokal(P)
    w, c_von = bewegung_v7(mo)
    kpv = BB_kf(mo, V.PV)
    lok_ruhe = [lok_fit[i] @ BF._exp(w[kpv, i]).T for i in range(7)]
    emr = bytearray(blob[d[2]:d[3]])
    o16 = 8 + 16 * 6
    struct.pack_into('<3h', emr, o16, *[int(round(x)) for x in P[0]])
    for b in range(17, 23):
        struct.pack_into('<3h', emr, 8 + b * 6, 0, L, 0)
    for k in range(s.kf_count):
        for i, b in enumerate(V.SCHWANZ):
            winkel_schreiben(emr, s, k, b, lok_ruhe[i] @ BF._exp(w[k, i]))
    blob2 = bytes(blob[:d[2]]) + bytes(emr) + bytes(blob[d[3]:])
    # Meshes
    n_end, n_anf = gehrungen(lok_ruhe)
    alt = copy.deepcopy(ms)
    for g in range(7):
        if g == 0:
            ms[16] = wurzel_mesh(alt[18], L, rad[0], rad[1], n_end[0], float(par['hinten']))
        else:
            ms[16 + g] = glied_mesh(alt[16 + g], g, L, rad[g], rad[g + 1], n_anf[g], n_end[g])
    dorn_y = float(L + DORN * rad[7])
    # Bodenregel je Keyframe (S6): Knochen 16 um eine waagerechte Achse hochdrehen (1 Grad je Schritt, beide Richtungen, hoechstens
    # 90 Grad), bis der Schwanz (Gelenke + Radius, Spitze + Radius, Dornspitze) nicht tiefer liegt als die tiefste Koerperecke
    # (Knochen 0-15, alle Ecken) - Regel wie schwanz_r41.c glaetten (ganze Kette starr um die Wurzel), hier offline je Keyframe.
    mo2 = V.modell(blob2)
    gehoben = {}
    rel = [np.array(mo2.skel.rel[b], float) for b in V.SCHWANZ]
    for k in range(s.kf_count):
        R, T = mo2.pose(k)
        boden = tiefste_koerperecke(mo2, R, T) - float(par.get('boden_rand', 0))
        R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
        loks = [lokal_aus_kf(mo2.skel, k, b) for b in V.SCHWANZ]
        if kette_tief(R8, T8, loks, rel, L, rad, dorn_y) <= boden:
            continue
        sp, _W = kette_punkte(R8, T8, loks, rel, L)
        ax = np.cross(sp[-1] - sp[0], Y)
        ax = ax / max(np.linalg.norm(ax), 1e-9)
        ok = None
        for n in range(1, 91):
            for sg in (1, -1):
                Ln = BF._quant(R8.T @ BF._exp(ax * np.radians(n) * sg) @ R8 @ loks[0])
                if kette_tief(R8, T8, [Ln] + loks[1:], rel, L, rad, dorn_y) <= boden:
                    ok = (n * sg, Ln)
                    break
            if ok:
                break
        if ok:
            winkel_schreiben(emr, s, k, 16, ok[1])
        gehoben[k] = ok[0] if ok else None
    blob3 = bytes(blob2[:d[2]]) + bytes(emr) + bytes(blob2[d[3]:])
    prot['schwanz'] = dict(L=L, wurzel=[int(round(x)) for x in P[0]], radien=[round(r, 1) for r in rad], dorn_y=dorn_y,
                           kf_pv=kpv, bodenregel=dict(keyframes=len(gehoben), grad={str(k): v for k, v in sorted(gehoben.items())}),
                           gelenkwinkel_ruhe=[round(float(np.degrees(np.arccos(np.clip((Lr @ Y) @ Y, -1, 1)))), 1) for Lr in lok_ruhe])
    return blob3


def BB_kf(mo, pose):
    return V.BB.kf_von_clip(mo, pose['clip'], pose['bild'])


# ------------------------------------------------------------------ Bau
def bauen(basis, stufen='sra', kette=None, ruecken=None, adern=None):
    blob = open(basis, 'rb').read()
    prot = dict(basis=basis, basis_sha256=V.sha(blob), stufen=stufen)
    d, _do, ms, tim = KB.teile(blob)
    if 's' in stufen:
        blob = schwanz(blob, d, ms, kette or _p('kette_v8.json'), prot)
        d, _do, _ms, _tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    if 'r' in stufen:
        import ruecken_v8 as RV
        RV.ruecken(ms, ruecken or _p('ruecken_v8.json'), prot)
    if 'a' in stufen:
        import adern_v8 as AV
        AV.adern(ms, t, adern or _p('adern_v8.json'), prot)
    tim_neu = KB.tim_packen(t)
    neu = KB.packen(blob, d, ms, tim_neu)
    prot['tim'] = dict(clut_zeilen=int(t['clut'].shape[0]), port_texturhoehe=int(t['ih'] * t['clut'].shape[0]))
    prot['emd'] = dict(bytes=len(neu), sha256=V.sha(neu))
    return neu, prot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=V.ASSET)
    ap.add_argument('--aus', default=os.path.join(V.SFV8, 'emd'))
    ap.add_argument('--name', default='EM30_V8.EMD')
    ap.add_argument('--json')
    ap.add_argument('--stufen', default='sra')
    a = ap.parse_args()
    neu, prot = bauen(a.basis, a.stufen)
    os.makedirs(a.aus, exist_ok=True)
    open(os.path.join(a.aus, a.name), 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s)' % (os.path.join(a.aus, a.name), len(neu), prot['emd']['sha256']))
    print('TIM', prot['tim'])
    if 'schwanz' in prot:
        p = prot['schwanz']
        print('schwanz L %d Wurzel %s Radien %s Bodenregel %d Keyframes' % (p['L'], p['wurzel'], p['radien'], p['bodenregel']['keyframes']))
    if a.json:
        json.dump(prot, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
