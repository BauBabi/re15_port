"""riegel_r6.py - Riegel der Runde 6b (P5-neu "Kragen weg, echter Nacken", P4-neu "Schwanz-Unterseite fertig") fuer riegel_v8.py.

Die Form existiert auf keiner Disc: jede Zahl hier ist PORT-WAHL und traegt den Wortlaut des Auftrags oder eine Messung.

S15 (erweitert, P4-neu d)  unterseite_r5.s15_grenze (Band theta 120-130 <= 5 % Fleisch-Zeile), s15_deckel (Ring-Deckel: 0 Posen mit grauem Ring auf
     der Unterseite), s15_reifen (0 graue Texelzeilen im Fleisch-Feld); kanten_mass als Info.
S16  (a) Luecken-Soll unveraendert (luecke_r5). Entscheid E3 des Orchestrators: EIN einzelnes Szenenbild ueber dem Soll, dessen Nachbarn im Soll
     liegen und das hoechstens so gross ist wie F311 in V8 r5d, ist "GELB - Einzelbild" (zaehlt nicht als rot). Zwei oder mehr Bilder, ein
     groesseres Bild oder ein Bild einer Foto-Pose (PV / P1 / P2 und ihre Drehungen) = rot.
     (b)/(c) NEU: gelten fuer ALLE Flaechen ausser Mesh 9, gemessen mit Maler-Reihenfolge: 0 verdeckte Bildpunkte des V7-Gesichts
     (Mesh 9, lokales x >= 150) in allen 177 Ansichten; in den Szenenbildern F225-F450 am echten Spiel ueber ein Marker-Asset
     (szene_marker.bauen_ko: Gesicht magenta): Bildpunkte, die im V7-Lauf magenta sind und im Lauf des Prueflings nicht (Soll 0).
S18  "vorn ohne Kragen". Vorderansicht = Kopf-Vorwaertsachse (Knochen 9, lokale x-Achse) zeigt mit cos >= 0,5 zur Kamera. Fenster = Kasten um
     den V7-Kopf, je Seite um die halbe Kopfgroesse erweitert.
     (a1) Ecken der im Fenster sichtbaren Flaechen von Mesh 8 liegen projiziert hoechstens 2 px neben ihrem Ort in V8 r4 (Auftrag: "Ecken ...
          zurueck auf den Stand der Runde 4 - ueberall, wo es von vorn sichtbar ist"; "hoechstens 2 px ueber V8 r4").
     (a2) Umriss von Mesh 8 und 9 im Fenster hoechstens 2 px ueber dem von V8 r4 (Info neben a1: der Kragen der Runde 5 war eine INNERE
          Kontur, kein Umriss-Ueberstand - gemessen r5d <= 2 px in den Takten F333-F378).
     (a3) Flaechen von Mesh 8 ohne Gegenstueck in V8 r4 (Nackenstueck) zeigen im Fenster 0 Bildpunkte (Auftrag: "Von vorn darf es nie neben
          oder ueber dem Gesicht zu sehen sein").
     (b)  helle Bildpunkte im Kasten (420,200)-(640,400) der echten Szenenbilder F344-F378 (960x720; B >= R-6, B > 120, R > 110) je Bild
          hoechstens V7-Maximum + V7-Streuung (Maximum - Minimum), beides am V7-Lauf zur Laufzeit gemessen.
S19  Texeldichte und Streckung (P5-neu c), Mesh 8, je Dreieck, das es in V8 r4 nicht gibt oder das gegen V8 r4 geaendert ist (Eckenorte, UV oder
     CLUT-Zeile): Dichte q = (UV-Flaeche / 3D-Flaeche) / Median der unberuehrten Zeile-493-Dreiecke des Prueflings in [0,5; 2]; Streckung <= 2:
     bei unveraenderter UV die Verformung gegen r4 (groesster Singulaerwert bzw. Kehrwert des kleinsten), sonst das Seitenverhaeltnis der
     Abbildung UV -> 3D (Anisotropie). Was in r4 schon ausserhalb lag, darf nicht weiter ausserhalb liegen als dort. Normalen: gespeicherte
     Ecken-Normale gegen die flaechengewichtete Geometrie-Normale der Ecke, Soll <= groesster Wert in V8 r4.
     Info daneben: derselbe Vergleich gegen den festen Bezug "Median aller Zeile-493-Dreiecke von V8 r4" (der Bezug des Auftrags schwankt mit
     dem Pruefling: r5d 0,00101, r6 0,00227, r4 selbst 0,00279).
"""
import os, sys, subprocess
import numpy as np
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402

R4_COMMIT = '282e4d594'               # V8 Runde 4 (Auftrag: "zurueck auf den Stand der Runde 4 (Commit 282e4d594)")
R4_SHA = None                         # wird beim Laden ausgegeben
E3_GRENZE = (63, 37, 6.3)             # F311 in V8 r5d in DIESEM Mass (groesster Wert der 7 Halbpixel-Varianten; runde5/riegel_v8r5d.txt).
E3_ORCH = (54, 31, 5.8)               # Wortlaut des Orchestrators fuer dasselbe Bild (eine Variante) - wird mit ausgegeben
S18_COS = 0.5                         # Vorderansicht: Kopf-Vorwaertsachse zur Kamera, cos >= 0,5 (PORT-WAHL; waehlt die Takte, in denen die
                                      # Szene den Kopf von vorn / schraeg vorn zeigt, P1 und die Drehungen von PV / P2 nach vorn)
S18_PX = 2.0                          # Auftrag: "hoechstens 2 px ueber V8 r4"
S18_KASTEN = (420, 200, 640, 400)     # Auftrag: Kasten (420,200)-(640,400) der Szenenbilder 960x720
S18_BILDER = (344, 378)               # Auftrag: heller Keil F344-F378
S19_Q = (0.5, 2.0)                    # Auftrag: "0,5- bis 2-fach des Medians der unberuehrten Rueckenflaechen"
S19_STRECK = 2.0                      # Auftrag: "keine Streckung > 2"
S19_ZEILE = 493                       # Rueckenhaut von Mesh 8 (CLUT-Zeile; r4: 78 von 131 Dreiecken)
TOL = 1.01                            # Rundung ganzzahliger Ecken / UV: 1 % (PORT-WAHL)


def r4_blob(pfad=None):
    if pfad:
        return open(pfad, 'rb').read()
    return subprocess.check_output(['git', 'show', R4_COMMIT + ':re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD'], cwd=HIER)


# ------------------------------------------------------------------ S15 (P4-neu d)
def s15_plus(s15, blob):
    import unterseite_r5 as U5
    if s15.get('r8'):                                             # Runde 8: S15 = R8e (riegel_schwanz_r8.py), Regeln der Runden 5/6 ersetzt
        return s15
    try:
        g = U5.s15_grenze(blob)
        dk = U5.s15_deckel(blob)
        rf = U5.s15_reifen(blob)
        km = U5.kanten_mass(blob)
        s15['grenze'] = dict(ok=bool(g['ok']), ansichten={p: {k: v for k, v in a.items() if isinstance(v, (int, float))} for p, a in g['ansichten'].items()})
        s15['deckel'] = dict(ok=bool(dk['ok']), posen=dk['posen'], bilder=dk['bilder'], px=dk['px'], groesst=dk['groesst'])
        s15['reifen'] = dict(ok=bool(rf['ok']), grau=rf['grau'], zeilen=rf['zeilen'], kleinst=rf['kleinst'])
        s15['kanten'] = {k: {kk: vv for kk, vv in v.items() if isinstance(vv, (int, float))} for k, v in km.items()}
        s15['ok'] = bool(s15['ok'] and g['ok'] and dk['ok'] and rf['ok'])
    except Exception as e:                                        # Staende ohne Fleisch-Zeile 495 (V7, V8 r4)
        s15['r6_fehler'] = '%s: %s' % (type(e).__name__, str(e)[:160])
        s15['ok'] = False
    return s15


# ------------------------------------------------------------------ S16 (b)/(c) neu + E3
def s16_verdeckt(blob, ref_blob):
    import luecke_r5 as L
    import ruecken_r5 as RR5
    mo, mo7 = V.modell(blob), V.modell(ref_blob)
    ue, summe, kn = [], 0, {}
    n = 0
    for nm, p in L.ansichten('alle'):
        A, A7 = L.zeichne(mo, p, L.VARIANTEN[0]), L.zeichne(mo7, p, L.VARIANTEN[0])
        ges = (A7['bone'][:480] == 9) & (A7['lok'][:480][..., 0] >= RR5.GESICHT_X)
        b = A['bone'][:480]
        ver = ges & (b != 9) & (b >= 0)
        n += 1
        if ver.any():
            k = {int(x): int((b[ver] == x).sum()) for x in np.unique(b[ver])}
            ue.append((nm, int(ver.sum()), k))
            summe += int(ver.sum())
            for x, c in k.items():
                kn[x] = kn.get(x, 0) + c
    return dict(ansichten=n, summe=summe, ueber=ue, knochen=kn, ok=summe == 0)


def s16_szene(ko_v7, ko, f0=225, f1=450):
    import szene_marker as SM
    summe, n, ges, ue = 0, 0, 0, []
    for f in range(f0, f1 + 1):
        p7, pn = os.path.join(ko_v7, 'fd', 'f%06d.ppm' % f), os.path.join(ko, 'fd', 'f%06d.ppm' % f)
        if not (os.path.exists(p7) and os.path.exists(pn)):
            continue
        m7, mn = SM.mag(SM.bild(p7)), SM.mag(SM.bild(pn))
        v = int((m7 & ~mn).sum())
        n += 1
        ges += int(m7.sum())
        summe += v
        if v:
            ue.append((f, v))
    return dict(bilder=n, gesicht_px=ges, summe=summe, ueber=ue, ok=n == f1 - f0 + 1 and summe == 0)


def e3(ueber_soll):
    """Entscheid E3: -> ('GRUEN' | 'GELB - Einzelbild' | 'ROT', Begruendung)."""
    if not ueber_soll:
        return 'GRUEN', ''
    if len(ueber_soll) > 1:
        return 'ROT', '%d Ansichten ueber dem Soll' % len(ueber_soll)
    nm, px, tief, mt = ueber_soll[0][:4]
    if not (nm[:1] == 'F' and nm[1:].isdigit()):
        return 'ROT', 'Foto-Pose %s ueber dem Soll' % nm
    if px > E3_GRENZE[0] or tief > E3_GRENZE[1] or mt > E3_GRENZE[2]:
        return 'ROT', 'Einzelbild %s (%d / %d / %.1f) groesser als F311 in r5d %s' % (nm, px, tief, mt, E3_GRENZE)
    return 'GELB - Einzelbild', '%s (%d / %d / %.1f) <= F311 in r5d %s' % (nm, px, tief, mt, E3_GRENZE)


def s16_plus(s16, blob, ref_blob, ko=None):
    st, grund = e3(s16['ueber_soll'])
    s16['e3'] = dict(status=st, grund=grund)
    s16['verdeckt'] = s16_verdeckt(blob, ref_blob)
    ok = st != 'ROT' and s16['verdeckt']['ok']
    if ko and ko[0] and ko[1]:
        s16['verdeckt_szene'] = s16_szene(*ko)
        ok = ok and s16['verdeckt_szene']['ok']
    s16['ok_r5'] = s16['ok']
    s16['ok'] = bool(ok)
    s16['status'] = 'ROT' if not ok else st
    return s16


# ------------------------------------------------------------------ S18
def vorn_cos(mo, pose):
    R, T = V.pose_r_t(mo, pose)
    f = R[9].astype(float)[:, 0] / 4096.0
    k = V.kamera_modell(pose) - T[9].astype(float)
    return float(f @ k / np.linalg.norm(k) / np.linalg.norm(f))


def _ecken(m, key):
    return list((m['tf'] if key[0] == 'd' else m['qf'])[int(key[1:])][1::2])


def fd_bild(ordner, f):
    """Framedump-Bild f: --fd / --fd-v7 nennen wie bei S14s den Framedump-Ordner selbst (.../fd); ein Laufordner (mit Unterordner fd) geht auch."""
    p = os.path.join(ordner, 'f%06d.ppm' % f)
    return p if os.path.exists(p) else os.path.join(ordner, 'fd', 'f%06d.ppm' % f)


def hell(pfad):
    from PIL import Image
    x0, y0, x1, y1 = S18_KASTEN
    im = np.asarray(Image.open(pfad).convert('RGB')).astype(int)[y0:y1, x0:x1]
    R, B = im[..., 0], im[..., 2]
    return int(((B >= R - 6) & (B > 120) & (R > 110)).sum())


def s18(blob, ref_blob, r4, fd=None):
    import luecke_r5 as L
    mo, mo7, mo4 = V.modell(blob), V.modell(ref_blob), V.modell(r4)
    m, m4 = KB.teile(blob)[2][8], KB.teile(r4)[2][8]
    alt = set(['d%d' % i for i in range(min(len(m['tf']), len(m4['tf'])))] + ['v%d' % i for i in range(min(len(m['qf']), len(m4['qf'])))])
    a1, a2, a3, namen = [], [], [], []
    for nm, p in L.ansichten('alle'):
        if vorn_cos(mo7, p) < S18_COS:
            continue
        k7 = V.b2.umriss_schnell(mo7, p, knochen=[9])
        if not k7.any():
            continue
        namen.append(nm)
        ys, xs = np.nonzero(k7)
        r = max(ys.max() - ys.min() + 1, xs.max() - xs.min() + 1) // 2
        w = np.zeros(k7.shape, bool)
        w[max(0, ys.min() - r):ys.max() + r + 1, max(0, xs.min() - r):xs.max() + r + 1] = True
        A = L.zeichne(mo, p, L.VARIANTEN[0])
        b, fl = A['bone'][:480], A['flaeche'][:480]
        sel = (b == 8) & w[:b.shape[0], :b.shape[1]]
        keys = set(np.unique(fl[sel]).tolist())
        neu_px = int(sum(int((fl[sel] == k).sum()) for k in keys if k not in alt))
        ecken = sorted(set(i for k in keys if k in alt for i in _ecken(m, k) if i < len(m4['v'])))
        dm, bei = 0.0, -1
        if ecken:
            P1 = V.proj(p, V.b8_zu_modell(mo, np.array([m['v'][i][:3] for i in ecken], float), p))
            P0 = V.proj(p, V.b8_zu_modell(mo4, np.array([m4['v'][i][:3] for i in ecken], float), p))
            d = np.linalg.norm(np.asarray(P1, float)[:, :2] - np.asarray(P0, float)[:, :2], axis=1)
            dm, bei = float(d.max()), ecken[int(d.argmax())]
        U, U4 = V.b2.umriss_schnell(mo, p, knochen=[8, 9]), V.b2.umriss_schnell(mo4, p, knochen=[8, 9])
        ueber = U & ~U4 & w
        um = float(ndimage.distance_transform_edt(~U4)[ueber].max()) if ueber.any() else 0.0
        if dm > S18_PX:
            a1.append((nm, round(dm, 1), bei))
        if um > S18_PX:
            a2.append((nm, round(um, 1)))
        if neu_px:
            a3.append((nm, neu_px))
    out = dict(vorn=len(namen), vorn_namen=namen, a1=a1, a1_max=max([x[1] for x in a1], default=0.0), a2=a2,
               a2_max=max([x[1] for x in a2], default=0.0), a3=a3, a3_max=max([x[1] for x in a3], default=0),
               neue_flaechen=len(m['tf']) + len(m['qf']) - len(alt))
    ok = not a1 and not a3
    if fd and fd[0] and fd[1]:
        f0, f1 = S18_BILDER
        hn = [hell(fd_bild(fd[0], f)) for f in range(f0, f1 + 1)]
        h7 = [hell(fd_bild(fd[1], f)) for f in range(f0, f1 + 1)]
        soll = max(h7) + (max(h7) - min(h7))
        ueb = [(f0 + i, x) for i, x in enumerate(hn) if x > soll]
        out['b'] = dict(v7=(min(h7), max(h7)), soll=soll, neu=(min(hn), max(hn)), ueber=len(ueb), ueber_bilder=ueb, ok=not ueb)
        ok = ok and not ueb
    out['ok'] = bool(ok)
    return out


# ------------------------------------------------------------------ S19
def dreiecke(m):
    out = []
    for i, f in enumerate(m['tf']):
        uv = m['tuv'][i]
        out.append((('d%d' % i, 0), list(f[1::2]), list(f[0::2]), [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])], (uv[2] >> 6) & 0x1FF))
    for i, f in enumerate(m['qf']):
        uv = m['quv'][i]
        e, nn = list(f[1::2]), list(f[0::2])
        c = [(uv[3 * k], uv[3 * k + 1]) for k in range(4)]
        for h, idx in enumerate(((0, 1, 2), (1, 3, 2))):
            out.append((('v%d' % i, h), [e[j] for j in idx], [nn[j] for j in idx], [c[j] for j in idx], (uv[2] >> 6) & 0x1FF))
    return out


def _basis(a, b):
    ex = a / np.linalg.norm(a)
    n = np.cross(a, b)
    ey = np.cross(n / np.linalg.norm(n), ex)
    return np.array([[a @ ex, b @ ex], [a @ ey, b @ ey]])


def mass(P, c):
    """-> (3D-Flaeche, UV-Flaeche, Anisotropie der Abbildung UV -> 3D)."""
    U = np.array(c, float)
    a, b = P[1] - P[0], P[2] - P[0]
    A3 = 0.5 * float(np.linalg.norm(np.cross(a, b)))
    du, dv = U[1] - U[0], U[2] - U[0]
    Auv = 0.5 * abs(float(du[0] * dv[1] - du[1] * dv[0]))
    if A3 < 1e-9 or Auv < 1e-9:
        return A3, Auv, float('inf')
    s = np.linalg.svd(_basis(a, b) @ np.linalg.inv(np.array([[du[0], dv[0]], [du[1], dv[1]]])), compute_uv=False)
    return A3, Auv, float(s[0] / s[1])


def verformung(P0, P1):
    s = np.linalg.svd(_basis(P1[1] - P1[0], P1[2] - P1[0]) @ np.linalg.inv(_basis(P0[1] - P0[0], P0[2] - P0[0])), compute_uv=False)
    return float(max(s[0], 1.0 / s[1]))


def normalen_winkel(m):
    """Je Dreiecks-Ecke: Winkel (Grad) zwischen gespeicherter Normale und flaechengewichteter Geometrie-Normale der Ecke (ueber alle Dreiecke
    von Mesh 8 an diesem Eckenort). Vorzeichen der Geometrie-Normale = das, das im Mesh ueberwiegt. -> {(key, h): groesster Winkel}."""
    D = dreiecke(m)
    gn = {}
    fn = {}
    for key, e, nn, c, z in D:
        P = np.array([m['v'][j][:3] for j in e], float)
        g = np.cross(P[1] - P[0], P[2] - P[0])
        fn[key] = g
        for j in e:
            o = tuple(m['v'][j][:3])
            gn[o] = gn.get(o, 0) + g
    vz = []
    for key, e, nn, c, z in D:
        for j, k in zip(e, nn):
            N = np.array(m['n'][k][:3], float)
            vz.append(np.sign(N @ gn[tuple(m['v'][j][:3])]))
    s = 1.0 if np.sum(vz) >= 0 else -1.0
    out = {}
    for key, e, nn, c, z in D:
        w = 0.0
        for j, k in zip(e, nn):
            N = np.array(m['n'][k][:3], float)
            G = s * gn[tuple(m['v'][j][:3])]
            if np.linalg.norm(N) < 1e-9 or np.linalg.norm(G) < 1e-9:
                continue
            w = max(w, float(np.degrees(np.arccos(np.clip(N @ G / np.linalg.norm(N) / np.linalg.norm(G), -1, 1)))))
        out[key] = w
    return out


def s19(blob, r4):
    m, m4 = KB.teile(blob)[2][8], KB.teile(r4)[2][8]
    D, D4 = dreiecke(m), {x[0]: x for x in dreiecke(m4)}
    nw, nw4 = normalen_winkel(m), normalen_winkel(m4)
    n_soll = max(nw4.values())
    d4_493 = [mass(np.array([m4['v'][j][:3] for j in e], float), c) for key, e, nn, c, z in D4.values() if z == S19_ZEILE]
    med_r4 = float(np.median([x[1] / x[0] for x in d4_493 if x[0] > 0]))
    rows, unb = [], []
    for key, e, nn, c, z in D:
        P = np.array([m['v'][j][:3] for j in e], float)
        A3, Auv, an = mass(P, c)
        alt = D4.get(key)
        if alt:
            P0 = np.array([m4['v'][j][:3] for j in alt[1]], float)
            if (P0 == P).all() and alt[3] == c and alt[4] == z:
                if z == S19_ZEILE and A3 > 0:
                    unb.append(Auv / A3)
                continue
        rows.append((key, z, P, c, A3, Auv, an, alt))
    med = float(np.median(unb)) if unb else float('nan')

    def urteil(bezug):
        aus = []
        for key, z, P, c, A3, Auv, an, alt in rows:
            q = (Auv / A3 / bezug) if A3 > 0 else float('inf')
            q4 = an4 = None
            st, art = an, 'Seitenverhaeltnis'
            if alt:
                P0 = np.array([m4['v'][j][:3] for j in alt[1]], float)
                a30, auv0, an4 = mass(P0, alt[3])
                q4 = (auv0 / a30 / bezug) if a30 > 0 else float('inf')
                if alt[3] == c:
                    st, art = verformung(P0, P), 'Verformung gegen r4'
            grund = []
            if not (S19_Q[0] <= q <= S19_Q[1]):
                if q4 is None or (S19_Q[0] <= q4 <= S19_Q[1]) or abs(np.log(q)) > abs(np.log(q4)) + np.log(TOL):
                    grund.append('Dichte %.2f%s' % (q, '' if q4 is None else ' (r4 %.2f)' % q4))
            if st > S19_STRECK * TOL:
                if art != 'Seitenverhaeltnis' or an4 is None or st > an4 * TOL:
                    grund.append('%s %.2f%s' % (art, st, '' if (an4 is None or art != 'Seitenverhaeltnis') else ' (r4 %.2f)' % an4))
            if nw[key] > n_soll + 0.5:
                grund.append('Normale %.1f Grad' % nw[key])
            if grund:
                aus.append(('%s.%d' % key, z, '; '.join(grund)))
        return aus

    aus = urteil(med) if unb else [('-', 0, 'keine unberuehrte Zeile-%d-Flaeche' % S19_ZEILE)]
    aus_r4 = urteil(med_r4)
    qs = [(Auv / A3 / med) for key, z, P, c, A3, Auv, an, alt in rows if A3 > 0] if unb else []
    return dict(geprueft=len(rows), neu=len([1 for x in rows if x[7] is None]), unberuehrt=len(unb), median=med, median_r4=med_r4,
                q_bereich=(round(min(qs), 2), round(max(qs), 2)) if qs else None, normalen_soll=round(n_soll, 1),
                normalen_max=round(max([nw[x[0]] for x in rows], default=0.0), 1), aus=aus, aus_r4=aus_r4, ok=not aus)


# ------------------------------------------------------------------ Einbau und Ausgabe
def einbauen(out, blob, ref_blob, r4, ko=None, fd=None):
    s15_plus(out['S15'], blob)
    s16_plus(out['S16'], blob, ref_blob, ko)
    out['S18'] = s18(blob, ref_blob, r4, fd)
    out['S19'] = s19(blob, r4)
    out['r4_sha256'] = V.sha(r4)
    return out


def zeilen(r):
    z = []
    s = r.get('S15', {})
    if s.get('r8'):
        pass
    elif 'r6_fehler' in s:
        z.append('S15 Runde 6 (Grenze / Deckel / Reifen): nicht messbar (%s) -> ROT' % s['r6_fehler'])
    elif 'grenze' in s:
        g, d, f, k = s['grenze'], s['deckel'], s['reifen'], s['kanten']
        z.append('S15 Runde 6 (P4-neu): Band theta 120-130 Anteil Fleisch-Zeile %s (Soll <= 5 %%) -> %s; Ring-Deckel mit grauem Ring auf der '
                 'Unterseite: %s Posen, %s Bilder, %s px, groesster Fleck %s (Soll 0 Posen) -> %s; graue Texelzeilen im Fleisch-Feld %s '
                 '(Soll 0) -> %s; Kanten Info (Zahl / mittlerer Sprung a*): %s'
                 % ('; '.join('%s %.1f %%' % (p, a.get('band', float('nan'))) for p, a in g['ansichten'].items()), 'GRUEN' if g['ok'] else 'ROT',
                    d['posen'], d['bilder'], d['px'], d['groesst'], 'GRUEN' if d['ok'] else 'ROT', f['grau'], 'GRUEN' if f['ok'] else 'ROT',
                    '; '.join('%s %s / %s (groesster %s)' % (n, v.get('kanten'), v.get('da'), v.get('da_max')) for n, v in k.items())))
    s = r.get('S16', {})
    if 'verdeckt' in s:
        v = s['verdeckt']
        z.append('S16 Runde 6: (a) Entscheid E3: %s%s [Grenze F311 in r5d in diesem Mass %s; Orchestrator nennt %s]; (b)/(c) NEU verdeckte '
                 'Bildpunkte des V7-Gesichts durch Flaechen ausser Mesh 9, Maler-Reihenfolge, %d Ansichten: %d (Soll 0), je Knochen %s, '
                 'Ansichten %s%s; Runde-5-Mass (b)/(c) nur noch Info -> %s'
                 % (s['e3']['status'], (' - ' + s['e3']['grund']) if s['e3']['grund'] else '', E3_GRENZE, E3_ORCH, v['ansichten'], v['summe'],
                    v['knochen'], v['ueber'][:24],
                    ('; Szenenbilder F225-F450 (Marker-Lauf, %d Bilder, %d Gesichts-Bildpunkte in V7): %d verdeckt (Soll 0) %s'
                     % (s['verdeckt_szene']['bilder'], s['verdeckt_szene']['gesicht_px'], s['verdeckt_szene']['summe'],
                        s['verdeckt_szene']['ueber'][:24])) if 'verdeckt_szene' in s else '; Szenenbilder: kein Marker-Lauf angegeben',
                    s['status']))
    if 'S18' in r:
        s = r['S18']
        z.append('S18 vorn ohne Kragen (P5-neu a/b), %d Vorderansichten (cos >= %.1f): (a1) Ecken sichtbarer Flaechen von Mesh 8 im Kopf-Fenster '
                 'gegen V8 r4 > %.0f px: %d Ansichten, groesste %.1f px %s; (a3) Bildpunkte von %d Flaechen ohne Gegenstueck in r4 im Kopf-Fenster '
                 '(Soll 0): %d Ansichten, groesste %d px %s; (a2, Info) Umriss Mesh 8+9 ueber r4 > %.0f px: %d Ansichten, groesste %.1f px %s%s -> %s'
                 % (s['vorn'], S18_COS, S18_PX, len(s['a1']), s['a1_max'], s['a1'][:14], s['neue_flaechen'], len(s['a3']), s['a3_max'],
                    s['a3'][:14], S18_PX, len(s['a2']), s['a2_max'], s['a2'][:10],
                    ('; (b) helle Bildpunkte Kasten (420,200)-(640,400) F%d-F%d: neu %d-%d, V7 %d-%d, Soll <= %d (V7-Maximum + Streuung), '
                     'darueber %d von %d Bildern -> %s'
                     % (S18_BILDER[0], S18_BILDER[1], s['b']['neu'][0], s['b']['neu'][1], s['b']['v7'][0], s['b']['v7'][1], s['b']['soll'],
                        s['b']['ueber'], S18_BILDER[1] - S18_BILDER[0] + 1, 'GRUEN' if s['b']['ok'] else 'ROT')) if 'b' in s
                    else '; (b) helle Bildpunkte: kein Szenenlauf angegeben',
                    'GRUEN' if s['ok'] else 'ROT'))
    if 'S19' in r:
        s = r['S19']
        z.append('S19 Texeldichte und Streckung (P5-neu c), Mesh 8: %d Dreiecke neu oder gegen r4 geaendert (davon %d neu), Bezug = Median '
                 'der %d unberuehrten Zeile-%d-Dreiecke %.5f (r4-Bezug %.5f), q-Bereich %s, Normalen groesster Winkel %.1f Grad (Soll <= r4 %.1f); '
                 'ausserhalb (Soll 0): %d %s -> %s'
                 % (s['geprueft'], s['neu'], s['unberuehrt'], S19_ZEILE, s['median'], s['median_r4'], s['q_bereich'], s['normalen_max'],
                    s['normalen_soll'], len(s['aus']), s['aus'][:30], 'GRUEN' if s['ok'] else 'ROT'))
        z.append('   S19 Info, fester Bezug V8 r4 (Median %.5f): ausserhalb %d %s' % (s['median_r4'], len(s['aus_r4']), s['aus_r4'][:30]))
    return z


if __name__ == '__main__':          # schnelle Teilmessung ohne S16 (a): riegel_r6.py <emd> <v7> [r4]
    blob, ref = open(sys.argv[1], 'rb').read(), open(sys.argv[2], 'rb').read()
    r4 = r4_blob(sys.argv[3] if len(sys.argv) > 3 else None)
    r = dict(S18=s18(blob, ref, r4), S19=s19(blob, r4))
    r['S16'] = dict(ueber_soll=[], ok=True)
    s16_plus(r['S16'], blob, ref)
    print('\n'.join(zeilen(r)))
