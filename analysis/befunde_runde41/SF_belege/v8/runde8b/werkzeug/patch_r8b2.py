"""Patch 2 Runde 8b: heben() (Keyframes so anheben, dass kein Punkt von Mesh 22 unter der Sohle liegt; S6 streng) und Einbau in stufe()."""
import os
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'schwanz_r8.py')
s = open(p, encoding='utf-8').read()
neu_fn = '''
# ------------------------------------------------------------------ Bodenfreiheit (Runde 8b, S6 streng)
HEBEN_RAND = 0.0                      # wie Stufe s (kette_v8.json boden_rand 0): kein Punkt UNTER der tiefsten Koerperecke (Riegel S6 "unter" = y > Sohle)


def mesh22_tief(R8, T8, loks, rel, L, P22):
    """Groesstes Welt-y (Modellraum, y nach unten) der Ecken von Mesh 22 bei den Lokaldrehungen loks (Knochen 16-22, wie v8_bau.kette_punkte)."""
    import v8_bau as VB
    pts, W22 = VB.kette_punkte(R8, T8, loks, rel, L)
    return float((pts[6][None, :] + P22 @ W22.T)[:, 1].max())


def heben(blob, prot=None):
    """Runde 8b (Pruefer pr8 (C)/(E)1, Koordinator "S6 wieder STRENG mit Nadel: in 0 von 912 Keyframes unter der Sohle"; Weg "die Schwanz-Keyframes
    anheben ... es ist Form-Anpassung an unserem eigenen Modell"): je Keyframe, in dem eine Ecke von Mesh 22 (Knolle, Nadel) unter der tiefsten
    Koerperecke liegt, die Glieder 17-22 wie die Bodenregel der Stufe s (v8_bau.schwanz) gemeinsam um eine waagerechte Achse hochdrehen (1 Grad je
    Schritt, beide Richtungen, Knochen 16 fest, jedes Gelenk <= 25 Grad), bis Mesh 22 UND die Pruefung der Stufe s (Gelenke + Radius, Dorn 589) frei
    sind. Aendert nur die 12-Bit-Winkel der Knochen 17-22 in diesen Keyframes; alle anderen Keyframes und Knochen bleiben bytegleich. Danach Probe
    mit der echten Posenformel (EA/b2 pose) und, falls die Rundung der Winkel noch eine Ecke unter die Sohle bringt, weiter drehen."""
    import struct as _st
    import v8_bau as VB
    d, _o, ms, _tim = KB.teile(blob)
    mo = V.modell(blob)
    s = mo.skel
    par = _p('kette_v8.json')
    P = np.array(par['gelenke_b8'], float)
    L = int(par['L'])
    rad = [float(r) for r in par['radien']]
    lok_ruhe = VB.ruhe_lokal(P)
    kpv = VB.BB_kf(mo, V.PV)
    wmax = np.radians(VB.WINKEL_MAX_GRAD)
    Rp, _Tp = mo.pose(kpv)
    R8p = Rp[8].astype(float) / 4096.0
    W_, Dpv = R8p @ lok_ruhe[0], []
    for g in range(1, 7):
        W_ = W_ @ lok_ruhe[g]
        Dpv.append(W_ @ Y)
    Dpv = np.array(Dpv)
    k_pv = VB.kurs(R8p)
    dorn_alt = float(L + VB.DORN * rad[7])
    rel = [np.array(s.rel[b], float) for b in SCHWANZ]
    P22 = np.array([v[:3] for v in ms[22]['v']], float)
    emr = bytearray(blob[d[2]:d[3]])

    def frei(R8, T8, loks, boden):
        return (VB.kette_tief(R8, T8, loks, rel, L, rad, dorn_alt) <= boden and mesh22_tief(R8, T8, loks, rel, L, P22) <= boden)

    gehoben, nicht = {}, []
    for k in range(s.kf_count):
        R, T = mo.pose(k)
        R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
        boden = VB.tiefste_koerperecke(mo, R, T) - HEBEN_RAND
        R22 = R[22].astype(float) / 4096.0
        if float((T[22].astype(float)[None, :] + P22 @ R22.T)[:, 1].max()) <= boden:
            continue                                          # mit der echten Pose schon frei: Keyframe unveraendert
        Ry = VB.rot_y(VB.kurs(R8) - k_pv)
        D = Dpv @ Ry.T
        W16 = R8 @ lok_ruhe[0]
        loks0 = [lok_ruhe[0]] + VB.glieder_aus_richtungen(W16, D, wmax)
        pts, _W = VB.kette_punkte(R8, T8, loks0, rel, L)
        h = pts[-1] - pts[1]
        ax = np.cross(np.array([h[0], 0.0, h[2]]), Y)
        ax = ax / max(np.linalg.norm(ax), 1e-9)
        ok = None
        for n in range(1, 121):
            for sg in (1, -1):
                Rh = BF._exp(ax * np.radians(n) * sg)
                l2 = [lok_ruhe[0]] + VB.glieder_aus_richtungen(W16, D @ Rh.T, wmax)
                if frei(R8, T8, l2, boden):
                    ok = (n * sg, l2)
                    break
            if ok:
                break
        if not ok:
            nicht.append(k)
            continue
        for i, b in enumerate(SCHWANZ):
            if b > 16:
                VB.winkel_schreiben(emr, s, k, b, ok[1][i])
        gehoben[k] = ok[0]
    neu = bytes(blob[:d[2]]) + bytes(emr) + bytes(blob[d[3]:])
    # Probe mit der echten Posenformel
    mo2 = V.modell(neu)
    rest = []
    for k in range(s.kf_count):
        R, T = mo2.pose(k)
        boden = VB.tiefste_koerperecke(mo2, R, T)
        R22 = R[22].astype(float) / 4096.0
        if float((T[22].astype(float)[None, :] + P22 @ R22.T)[:, 1].max()) > boden:
            rest.append(k)
    if prot is not None:
        prot['heben'] = dict(keyframes=len(gehoben), grad={str(k): v for k, v in sorted(gehoben.items())}, nicht_frei=nicht, rest_echte_pose=rest,
                             pv_keyframe=kpv, pv_gehoben=kpv in gehoben)
    return neu, gehoben, rest

'''
a = s.index('def felder_laden():')
s = s[:a] + neu_fn.lstrip('\n') + '\n\n' + s[a:]
alt = """    neu = KB.packen(blob, d, ms, tim)
    if prot is not None:
        prot['schwanz_r8'] = dict("""
assert alt in s
s = s.replace(alt, """    neu = KB.packen(blob, d, ms, tim)
    hp = {}
    neu, gehoben, rest = heben(neu, hp)                    # Runde 8b: Keyframes mit Mesh 22 unter der Sohle anheben (S6 streng)
    assert not rest and not hp['heben']['nicht_frei'], ('Mesh 22 nach dem Anheben noch unter der Sohle', rest, hp['heben']['nicht_frei'])
    if prot is not None:
        prot['heben'] = hp['heben']
        prot['schwanz_r8'] = dict(""")
open(p, 'w', encoding='utf-8').write(s)
print('ok')
