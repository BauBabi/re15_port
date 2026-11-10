"""SF V8 Runde 6a - linke Hand (Mesh 12) und linker Unterarm (Mesh 11): Breitenprofil des (W)-linken Arms, Vorlage gegen Modell.

    python hand_mass.py [--emd EMD] [--json JSON] [--bild PNG]

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Alle Zahlen = Bildmessung.
Nutzer 2026-11-10 (woertlich): "Bei drei_posen.png was du gepostet hast, sieht man es ganz eindeutig. Die Vorlage hat eine breite Hand
mit breiten Unterarmt. Dadurch wirkt es natuerlich. Bei uns ist die Hand ganz klein und zierlich und wirkt dadurch verkuemmert."

Vorlage = Nutzerbilder Version_80%/birkin20..24.png (Freisteller der Videobilder f1475-f1482 in doppelter Groesse mit Alpha-Maske; nur
gelesen). Dort steht der (W)-linke Arm Bild-rechts frei: Naht (gepunktetes Band) am Ellbogen, Unterarm, Handgelenk, Hand mit Daumen
(zum Koerper) und rotem Fingerblock. In den Seitenbildern f1433-f1442 (P2, PV; birkin11-13, Beschreibung2) verdeckt der Rumpf den
linken Arm ganz - dort gibt es kein Vorlagenmass.
Mass je Bild: die Arm-Maske (ab Unterkante der Naht) wird 4fach vergroessert um die Armachse gedreht (Achse senkrecht) und zeilenweise
vermessen: Breite gesamt (aeusserster linker bis aeusserster rechter Bildpunkt quer zur Achse, also mit Daumen) und groesster Lauf (ohne
den abgespreizten Daumen). Unterarm-Stationen im Achsensystem Ellbogen -> Handgelenk, Hand-Stationen im System Handgelenk -> Spitze.
Farbe dient nur zur ORTS-Bestimmung (Beginn der roten Zone = Krallenwurzeln), nie als Farbsoll.
Modell: f_rast (Spielkamera 50A0 Cut 12, RDT-Licht), Knochen 11 + 12 frei gezeichnet (ohne Verdeckung durch den Koerper), Ellbogen und
Handgelenk = die projizierten Gelenkpunkte, Krallenwurzeln = Ringmitten der Fingerkrallen A/B/C (hand_gemein.krallen).
"""
import os, sys, json, math, argparse
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
sys.path.insert(0, os.path.join(V.R41, 'v7hand'))
import f_rast as FR                   # noqa: E402
import f_analyse as FA                # noqa: E402
import hand_gemein as HG              # noqa: E402
import v4_gemein as G                 # noqa: E402
import b2                             # noqa: E402

VORLAGE_BILDER = (20, 21, 22, 23, 24)
P1 = dict(clip=20, bild=139, gier=1422, x=-8420, z=-21908)        # Foto 1 (SF_MESSBLATT_FOTO.md)
P2 = dict(clip=12, bild=48, gier=861, x=-8182, z=-21330)          # Foto 2
POSEN = dict(P1=P1, P2=P2, PV=dict(V.PV))
STATIONEN = ('ellbogen', 'unterarm_mitte', 'handgelenk', 'handwurzel', 'hand_breit', 'krallenwurzel', 'krallen_mitte', 'spitzen')
ZOOM = 4
SPITZE_NAME = {20: 'T', 47: 'A', 29: 'B', 42: 'C'}                # Spitzen-Ecken der vier Krallen von Mesh 12 (hand_gemein.krallen)
H1 = 302.0                                                        # Figurhoehe Foto 1 (SF_MESSBLATT_FOTO.md, G.H_ORIG[1])


def laeufe(z):
    d = np.diff(np.concatenate([[0], z.astype(np.int8), [0]]))
    return list(zip(np.where(d == 1)[0].tolist(), np.where(d == -1)[0].tolist()))


def groesster(L):
    return max(L, key=lambda r: r[1] - r[0])


def drehen(m, E, T, extra=()):
    """Maske m (bool) 4fach vergroessert so drehen, dass die Achse E -> T senkrecht nach unten zeigt.
    -> (gedrehte Maske, M 2x3: Quell-Pixel (x, y) -> gedrehtes Bild, gedrehte extra-Kanaele, Winkel der Achse gegen die Senkrechte)."""
    Z = ZOOM
    pad = int(max(m.shape) * 0.6) * Z
    def gross(k, interp):
        kb = cv2.resize(k, None, fx=Z, fy=Z, interpolation=interp)
        return cv2.copyMakeBorder(kb, pad, pad, pad, pad, cv2.BORDER_CONSTANT, value=0)
    big = gross(m.astype(np.uint8), cv2.INTER_NEAREST)
    c = ((E[0] + 0.5) * Z + pad, (E[1] + 0.5) * Z + pad)
    w = math.degrees(math.atan2(T[0] - E[0], T[1] - E[1]))
    R = cv2.getRotationMatrix2D(c, -w, 1.0)
    gr = (big.shape[1], big.shape[0])
    rot = cv2.warpAffine(big, R, gr, flags=cv2.INTER_NEAREST) > 0
    S = np.array([[Z, 0, 0.5 * Z + pad], [0, Z, 0.5 * Z + pad], [0, 0, 1]], float)
    ex = [cv2.warpAffine(gross(k.astype(np.float32), cv2.INTER_NEAREST), R, gr, flags=cv2.INTER_NEAREST) for k in extra]
    return rot, R @ S, ex, w


def punkt(M, p):
    return M @ np.array([p[0], p[1], 1.0])


def zurueck(M, q):
    return np.linalg.solve(M[:, :2], np.asarray(q, float) - M[:, 2])


def zeilen_profil(rot, y0):
    """Je Zeile ab y0: (gesamt, groesster Lauf, Zahl der Laeufe, x links, x rechts, Laeufe) in vergroesserten Pixeln; None = leer."""
    out = []
    for y in range(int(round(y0)), rot.shape[0]):
        L = laeufe(rot[y])
        if not L:
            out.append(None)
            continue
        g = groesster(L)
        out.append((L[-1][1] - L[0][0], g[1] - g[0], len(L), L[0][0], L[-1][1], L))
    while out and out[-1] is None:
        out.pop()
    return out


def breite(prof, t, feld, halb, skala):
    """Median der Breite in den Zeilen t +- halb (t, halb in Quell-Pixeln) -> (Breite in Videopixeln, mittlere Zeile)."""
    a, b = max(int(round((t - halb) * ZOOM)), 0), max(int(round((t + halb) * ZOOM)) + 1, 1)
    w = [p[feld] for p in prof[a:b] if p is not None]
    return (float(np.median(w)) / ZOOM * skala) if w else 0.0, min((a + b) // 2, len(prof) - 1)


def arm_messen(m, E, W, T, skala, wurzel_ab=None, daumen=True, m_hand=None):
    """m: Arm-Maske (bool, Quell-Pixel); E Ellbogen, W Handgelenk, T Spitze (x, y in Quell-Pixeln); skala: Quell-Pixel -> Videopixel
    (640 x 480); wurzel_ab: Abstand W -> Krallenwurzeln entlang der Handachse (Quell-Pixel); m_hand: eigene Maske
    fuer die Hand-Stationen (Modell: nur Knochen 12). -> dict, alle Laengen in Videopixeln.
    Station Handgelenk = ENGSTELLE: kleinste Gesamtbreite quer zur Achse Ellbogen -> Spitze zwischen 25 % und 65 % dieser Strecke
    (dieselbe Regel fuer Vorlage und Modell; in der Vorlage ist das zugleich der Ort W)."""
    E, W, T = (np.asarray(p, float) for p in (E, W, T))
    px1 = 1.0 / skala                                              # ein Videopixel in Quell-Pixeln
    r1, M1, _e, w1 = drehen(m, E, W)
    p1 = zeilen_profil(r1, punkt(M1, E)[1])
    Lu = float(np.linalg.norm(W - E))
    r2, M2, _e, w2 = drehen(m if m_hand is None else m_hand, W, T)
    r0, M0, _e, _w0 = drehen(m, E, T)
    yE0 = punkt(M0, E)[1]
    p0 = zeilen_profil(r0, yE0)
    i0, i1 = int(0.25 * len(p0)), max(int(0.65 * len(p0)), int(0.25 * len(p0)) + 1)
    ie = i0 + int(np.argmin([(p[0] if p else 10 ** 6) for p in p0[i0:i1]]))
    yW = punkt(M2, W)[1]
    p2 = zeilen_profil(r2, yW)
    Lh = len(p2) / ZOOM                                            # Handgelenk -> letzter belegter Bildpunkt entlang der Handachse
    if wurzel_ab is None:
        wurzel_ab = 0.55 * Lh
    ges = [(p[0] if p else 0) for p in p2]
    lauf = [(p[1] if p else 0) for p in p2]
    tb = int(np.argmax(ges)) / ZOOM
    orte = dict(ellbogen=(p1, M1, punkt(M1, E)[1], 3.0 * px1),      # 3 Videopixel unter der Naht / dem Gelenk
                unterarm_mitte=(p1, M1, punkt(M1, E)[1], Lu / 2),
                handgelenk=(p0, M0, yE0, ie / ZOOM), handwurzel=(p2, M2, yW, 0.2 * Lh), hand_breit=(p2, M2, yW, tb),
                krallenwurzel=(p2, M2, yW, wurzel_ab), krallen_mitte=(p2, M2, yW, (wurzel_ab + Lh) / 2),
                spitzen=(p2, M2, yW, Lh - 2.0 * px1))              # 2 Videopixel vor dem Ende
    st, linien = {}, {}
    for k in STATIONEN:
        prof, M, y0, t = orte[k]
        b, i = breite(prof, t, 0, px1, skala)
        st[k] = round(b, 1)
        p = prof[i] if prof else None
        if p is not None:
            linien[k] = [zurueck(M, (p[3], y0 + i)).tolist(), zurueck(M, (p[4], y0 + i)).tolist()]
    nu = max(int(Lu * ZOOM), 1)
    f10 = (0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0)
    f09 = (0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9)
    out = dict(stationen=st, unterarm_laenge=round(Lu * skala, 1), hand_laenge=round(Lh * skala, 1),
               bis_krallenwurzel=round(wurzel_ab * skala, 1), krallen_zone=round((Lh - wurzel_ab) * skala, 1),
               hand_breit_bei=round(tb * skala, 1), engstelle_bei=round(ie / max(len(p0), 1), 3),
               hand_ohne_daumen=round(max(lauf[int(0.35 * len(lauf)):max(int(0.8 * len(lauf)), 1)] or [0]) / ZOOM * skala, 1),
               winkel_unterarm=round(w1, 1), winkel_hand=round(w2, 1),
               unterarm_min=round(min((p[0] for p in p1[:nu] if p), default=0) / ZOOM * skala, 1),
               unterarm_max=round(max((p[0] for p in p1[:nu] if p), default=0) / ZOOM * skala, 1),
               profil_unterarm=[round(breite(p1, f * Lu, 0, px1, skala)[0], 1) for f in f10],
               profil_hand=[round(breite(p2, f * Lh, 0, px1, skala)[0], 1) for f in f09],
               profil_hand_lauf=[round(breite(p2, f * Lh, 1, px1, skala)[0], 1) for f in f09])
    out['_linien'] = linien
    if daumen:
        out['daumen'] = daumen_messen(p2, skala)
    return out


def daumen_messen(p2, skala):
    """Daumen = der kleinere Lauf ab der Gabel (erste Zeile, ab der mindestens 3 Videopixel lang zwei Laeufe nebeneinander liegen)."""
    n3 = int(round(3.0 / skala * ZOOM))
    gab = next((i for i in range(len(p2) - n3) if all(q is not None and q[2] >= 2 for q in p2[i:i + n3])), None)
    if gab is None:
        return None
    ende = gab
    for i in range(gab, len(p2)):
        if p2[i] is None or p2[i][2] < 2:
            break
        ende = i

    def teil(p):
        g = groesster(p[5])
        return g, groesster([r for r in p[5] if r != g])

    def mi(r):
        return 0.5 * (r[0] + r[1])
    start = min(gab + int(ZOOM / skala), ende)                    # ein Videopixel unter der Gabel
    g0, d0 = teil(p2[start])

    def ueb(r, s):
        return min(r[1], s[1]) - max(r[0], s[0])
    g1, d1 = g0, d0                                                # Nachtrag r6a: der Daumen-Lauf wird Zeile fuer Zeile VERFOLGT (sonst springt
    for i in range(start + 1, ende + 1):                           # das Mass am Modell auf eine Fingerkralle, sobald sich die Krallen trennen)
        kand = [r for r in p2[i][5] if ueb(r, d1) > 0 and ueb(r, d1) >= ueb(r, g1)]
        if not kand:
            ende = i - 1
            break
        dn = max(kand, key=lambda r: ueb(r, d1))
        rest = [r for r in p2[i][5] if r != dn]
        if not rest:
            ende = i - 1
            break
        g1, d1 = max(rest, key=lambda r: (ueb(r, g1), r[1] - r[0])), dn
    dx, dy = (mi(d1) - mi(d0)) / ZOOM, (ende - gab) / ZOOM
    return dict(gabel_ab_handgelenk=round(gab / ZOOM * skala, 1), laenge=round(math.hypot(dx, dy) * skala, 1),
                dicke_wurzel=round((d0[1] - d0[0]) / ZOOM * skala, 1), winkel_zur_handachse=round(math.degrees(math.atan2(dx, dy)), 1),
                seite='Bild-links (zum Koerper)' if mi(d0) < mi(g0) else 'Bild-rechts (vom Koerper weg)',
                fingerblock_laenge=round((len(p2) - gab) / ZOOM * skala, 1),
                fingerblock_breite=round((g0[1] - g0[0]) / ZOOM * skala, 1),
                abstand_wurzel=round(abs(mi(g0) - mi(d0)) / ZOOM * skala, 1), abstand_spitze=round(abs(mi(g1) - mi(d1)) / ZOOM * skala, 1),
                spalt_wurzel=round(max(g0[0] - d0[1], d0[0] - g0[1], 0) / ZOOM * skala, 1),
                spalt_spitze=round(max(g1[0] - d1[1], d1[0] - g1[1], 0) / ZOOM * skala, 1))


# ------------------------------------------------------------------ Vorlage
def ki_laden(k):
    return cv2.imdecode(np.fromfile(V.V80 + 'birkin%02d.png' % k, np.uint8), cv2.IMREAD_UNCHANGED)


def vorlage(k):
    """Nutzerbild birkin<k> (doppelte Groesse): Arm-Maske ab Naht, E, W, T, Beginn der roten Zone. Masse in Videopixeln."""
    im = ki_laden(k)
    m = im[..., 3] > 127
    g = cv2.cvtColor(im[..., :3], cv2.COLOR_BGR2GRAY).astype(float)
    lab = G.lab(cv2.cvtColor(im[..., :3], cv2.COLOR_BGR2RGB).astype(float))
    ys = np.where(m.any(1))[0]
    Hfig = float(ys[-1] - ys[0] + 1)

    def kontrast(y):
        L = laeufe(m[y])
        if not L or L[-1][1] - L[-1][0] < 30:
            return 0.0
        a, b = L[-1]
        return float(np.abs(np.diff(g[y, a + 3:b - 3])).mean())
    # Naht: Zeile mit dem groessten waagrechten Kontrast im aeussersten rechten Lauf (25 % .. 50 % der Figurhoehe)
    y0, y1 = int(ys[0] + 0.25 * Hfig), int(ys[0] + 0.50 * Hfig)
    ko = np.array([kontrast(y) for y in range(y0, y1)])
    naht = y0 + int(np.argmax(ko))
    yE = next((y for y in range(naht, naht + 16) if kontrast(y) < 0.5 * ko.max()), naht + 8)      # Unterkante des Bands
    a, b = laeufe(m[yE])[-1]
    La = laeufe(m[max(naht - 20, 0)])[-1]
    arm = np.zeros_like(m)
    arm[yE:] = m[yE:]
    _n, lb = cv2.connectedComponents(arm.astype(np.uint8), connectivity=8)
    arm = lb == lb[yE, (a + b) // 2]
    E = np.array([(a + b - 1) / 2.0, float(yE)])
    py, px = np.nonzero(arm)
    d = np.hypot(px - E[0], py - E[1])
    T = np.array([float(px[np.argmax(d)]), float(py[np.argmax(d)])])           # Spitze = fernster Punkt (Fingerblock; der Daumen liegt naeher)
    # Handgelenk = schmalste Stelle zwischen 25 % und 65 % der Strecke Ellbogen -> Spitze
    r0, M0, _e, _w = drehen(arm, E, T)
    yE0 = punkt(M0, E)[1]
    p0 = zeilen_profil(r0, yE0)
    i0, i1 = int(0.25 * len(p0)), int(0.65 * len(p0))
    iw = i0 + int(np.argmin([p[0] for p in p0[i0:i1]]))
    W = zurueck(M0, (0.5 * (p0[iw][3] + p0[iw][4]), yE0 + iw))
    # Beginn der roten Zone entlang der Handachse: a* im groessten Lauf >= Mitte zwischen Handruecken (erstes Viertel) und Fingerblock
    r2, M2, (a2,), _w2 = drehen(arm, W, T, extra=[lab[..., 1]])
    yW = int(round(punkt(M2, W)[1]))
    p2 = zeilen_profil(r2, yW)
    am = np.array([float(a2[yW + i, groesster(p[5])[0]:groesster(p[5])[1]].mean()) if p else 0.0 for i, p in enumerate(p2)])
    grau, rot = float(np.median(am[:len(am) // 4])), float(np.percentile(am, 95))
    schw = 0.5 * (grau + rot)
    ir = next((i for i in range(len(am) - 2 * ZOOM) if am[i:i + 2 * ZOOM].min() >= schw), int(0.55 * len(am)))
    out = arm_messen(arm, E, W, T, 0.5, wurzel_ab=ir / ZOOM)
    out.update(bild='birkin%02d' % k, video='f%04d' % (b2.KI_MAP[k][0] + 1), figur_hoehe=Hfig * 0.5, oberarm_breite=(La[1] - La[0]) * 0.5,
               naht_y=int(naht), E=E.round(1).tolist(), W=W.round(1).tolist(), T=T.round(1).tolist(),
               a_handruecken=round(grau, 1), a_fingerblock=round(rot, 1), am_rand=bool(arm[:, -2:].any()),
               spitze_ueber_boden=round((ys[-1] - T[1]) * 0.5, 1), handgelenk_ueber_boden=round((ys[-1] - W[1]) * 0.5, 1),
               ellbogen_ueber_boden=round((ys[-1] - E[1]) * 0.5, 1))
    out['_maske'], out['_bild'] = arm, im
    return out


# ------------------------------------------------------------------ Modell
def schneiden(p1, p2, p3, p4):
    """Schneiden sich die Strecken p1-p2 und p3-p4 echt (2D)?"""
    def o(a, b, c):
        return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])
    return o(p1, p2, p3) * o(p1, p2, p4) < 0 and o(p3, p4, p1) * o(p3, p4, p2) < 0


def modell(mo, pose):
    """Arm (Knochen 11 + 12) frei gezeichnet; E, W = Gelenkpunkte; Krallen einzeln. Masse in Videopixeln."""
    A = FR.zeichne(mo, pose, knochen=(11, 12))
    Av = FR.mit_bg(mo, pose)
    bone = A['bone'][:480]
    arm = bone >= 0
    figur = Av['bone'][:480] >= 0
    ys = np.where(figur.any(1))[0]
    E = np.asarray(FA.welt(mo, pose, 11, np.zeros((1, 3)))[0], float)[:2]
    W = np.asarray(FA.welt(mo, pose, 12, np.zeros((1, 3)))[0], float)[:2]
    me = mo.meshes[12]
    P = np.array([x[:3] for x in list(me.tv) + list(me.qv)], float)
    pp = np.asarray(FA.welt(mo, pose, 12, P), float)[:, :2]
    py, px = np.nonzero(bone == 12)
    d = np.hypot(px - W[0], py - W[1])
    T = np.array([float(px[np.argmax(d)]), float(py[np.argmax(d)])])
    kr = HG.krallen(dict(v=[list(x) for x in me.tv], tf=me.tf, qf=me.qf))
    for k in kr:                                                   # Namen nach der Spitzen-Ecke (HG.krallen nennt den Daumen nach dem
        k['name'] = SPITZE_NAME[k['spitze']]                       # groessten Ring-x; das gilt nach einer Drehung um die Handachse nicht mehr)
    ax = (T - W) / np.linalg.norm(T - W)
    # Station Krallenwurzeln wie in der Vorlage = Beginn der roten Zone entlang der Handachse: erste Stelle, ab der 2 Videopixel lang
    # mindestens die Haelfte des groessten Laufs auf der Finger-Zeile 492 liegt; ohne solche Stelle die Ringmitten der Fingerkrallen.
    rot = (bone == 12) & (A['zeile'][:480] == HG.Z_FINGER)
    r2, M2, (e2,), _w = drehen(bone == 12, W, T, extra=[rot.astype(float)])
    yW = int(round(punkt(M2, W)[1]))
    ant = [float(e2[yW + i, groesster(p[5])[0]:groesster(p[5])[1]].mean()) if p else 0.0 for i, p in enumerate(zeilen_profil(r2, yW))]
    ir = next((i for i in range(max(len(ant) - 2 * ZOOM, 0)) if min(ant[i:i + 2 * ZOOM]) >= 0.5), None)
    wurz = ir / ZOOM if ir is not None else float(np.mean([(pp[k['ring']].mean(0) - W) @ ax for k in kr if k['name'] != 'T']))
    out = arm_messen(arm, E, W, T, 1.0, wurzel_ab=max(wurz, 0.0), daumen=True, m_hand=(bone == 12))
    kl = {}
    for k in kr:
        ring = pp[k['ring']]
        c, s = ring.mean(0), pp[k['spitze']]
        v = s - c
        kl[k['name']] = dict(wurzel=c.round(1).tolist(), spitze=s.round(1).tolist(), laenge=round(float(np.linalg.norm(v)), 1),
                             dicke_wurzel=round(max(float(np.linalg.norm(ring[i] - ring[j])) for i in range(3) for j in range(i)), 1),
                             winkel_zur_handachse=round(-math.degrees(math.atan2(ax[0] * v[1] - ax[1] * v[0], float(ax @ v))), 1),   # < 0 = Bild-links wie daumen_messen
                             px=int(((bone == 12) & np.isin(A['flaeche'][:480], k['flaechen'])).sum()))
    nm = sorted(kl)
    kreuz = [nm[j] + nm[i] for i in range(len(nm)) for j in range(i)
             if schneiden(kl[nm[i]]['wurzel'], kl[nm[i]]['spitze'], kl[nm[j]]['wurzel'], kl[nm[j]]['spitze'])]

    def ab(a, b, was):
        return round(float(np.linalg.norm(np.array(kl[a][was]) - np.array(kl[b][was]))), 1)
    out.update(figur_hoehe=float(ys[-1] - ys[0] + 1), E=E.round(1).tolist(), W=W.round(1).tolist(), T=T.round(1).tolist(),
               krallen=kl, kreuzen=kreuz, px_hand=int((bone == 12).sum()), px_unterarm=int((bone == 11).sum()),
               px_hand_sichtbar=int((Av['bone'][:480] == 12).sum()), px_unterarm_sichtbar=int((Av['bone'][:480] == 11).sum()),
               spitze_ueber_boden=round(float(ys[-1] - T[1]), 1), handgelenk_ueber_boden=round(float(ys[-1] - W[1]), 1),
               ellbogen_ueber_boden=round(float(ys[-1] - E[1]), 1),
               abstand_wurzeln=[ab('A', 'C', 'wurzel'), ab('C', 'B', 'wurzel')], abstand_spitzen=[ab('A', 'C', 'spitze'), ab('C', 'B', 'spitze')])
    out['_maske'], out['_bild'], out['_voll'] = arm, A['img'][:480], Av['img'][:480]
    return out


# ------------------------------------------------------------------ Ausgabe
def roh(v):
    return {x: y for x, y in v.items() if not x.startswith('_')}


def zeige(name, v):
    print('%s: H %.1f | Stationen %s | Unterarm %.1f (min %.1f max %.1f), Hand %.1f (bis Krallenwurzel %.1f, Zone %.1f), ohne Daumen %.1f | '
          'ueber Boden E %.1f W %.1f T %.1f | Winkel U %.1f H %.1f'
          % (name, v['figur_hoehe'], v['stationen'], v['unterarm_laenge'], v['unterarm_min'], v['unterarm_max'], v['hand_laenge'],
             v['bis_krallenwurzel'], v['krallen_zone'], v['hand_ohne_daumen'], v['ellbogen_ueber_boden'], v['handgelenk_ueber_boden'],
             v['spitze_ueber_boden'], v['winkel_unterarm'], v['winkel_hand']))
    print('     Profil Unterarm 10-100 %%: %s | Hand 0-90 %%: %s | Hand groesster Lauf: %s' % (v['profil_unterarm'], v['profil_hand'], v['profil_hand_lauf']))
    if 'oberarm_breite' in v:
        print('     Oberarm %.1f, am Rand %s' % (v['oberarm_breite'], v['am_rand']))
    if v.get('daumen'):
        print('     Daumen %s' % v['daumen'])
    if 'krallen' in v:
        print('     Krallen (Laenge, Dicke, Winkel, px) %s | kreuzen %s | Abstand Wurzeln %s Spitzen %s | px Hand %d (sichtbar %d) Unterarm %d (sichtbar %d)'
              % ({n: (k['laenge'], k['dicke_wurzel'], k['winkel_zur_handachse'], k['px']) for n, k in v['krallen'].items()}, v['kreuzen'],
                 v['abstand_wurzeln'], v['abstand_spitzen'], v['px_hand'], v['px_hand_sichtbar'], v['px_unterarm'], v['px_unterarm_sichtbar']))


def kachel(img, v, skala, kante, f=6.0, titel=''):
    """Ausschnitt um den Arm, f-fach je Videopixel, mit E, W, T und den Stationslinien."""
    P = np.array([v['E'], v['W'], v['T']] + [p for l in v['_linien'].values() for p in l], float)
    cx, cy = 0.5 * (P[:, 0].min() + P[:, 0].max()), 0.5 * (P[:, 1].min() + P[:, 1].max())
    h = kante / skala / 2.0
    x0, y0 = int(round(cx - h)), int(round(cy - h))
    n = int(round(2 * h))
    aus = np.full((n, n, 3), 96, np.uint8)
    H_, W_ = img.shape[:2]
    xa, ya, xb, yb = max(x0, 0), max(y0, 0), min(x0 + n, W_), min(y0 + n, H_)
    if img.shape[2] == 4:
        al = img[ya:yb, xa:xb, 3:4].astype(float) / 255.0
        aus[ya - y0:yb - y0, xa - x0:xb - x0] = (img[ya:yb, xa:xb, 2::-1] * al + 96 * (1 - al)).astype(np.uint8)
    else:
        aus[ya - y0:yb - y0, xa - x0:xb - x0] = img[ya:yb, xa:xb, :3]
    g = int(round(kante * f))
    aus = cv2.resize(aus, (g, g), interpolation=cv2.INTER_NEAREST)
    aus = cv2.cvtColor(aus, cv2.COLOR_RGB2BGR)
    z = g / float(n)

    def q(p):
        return (int(round((p[0] - x0 + 0.5) * z)), int(round((p[1] - y0 + 0.5) * z)))
    farbe = dict(ellbogen=(0, 255, 255), unterarm_mitte=(0, 255, 255), handgelenk=(0, 255, 0), handwurzel=(255, 255, 0),
                 hand_breit=(255, 255, 0), krallenwurzel=(255, 0, 255), krallen_mitte=(255, 0, 255), spitzen=(255, 0, 255))
    for i, k in enumerate(STATIONEN):
        if k in v['_linien']:
            a, b = v['_linien'][k]
            cv2.line(aus, q(a), q(b), farbe[k], 1, cv2.LINE_AA)
            cv2.putText(aus, '%d %.0f' % (i + 1, v['stationen'][k]), (q(b)[0] + 3, q(b)[1] + 4), cv2.FONT_HERSHEY_SIMPLEX, 0.38, farbe[k], 1, cv2.LINE_AA)
    for p, c in ((v['E'], (0, 255, 255)), (v['W'], (0, 255, 0)), (v['T'], (255, 0, 255))):
        cv2.circle(aus, q(p), 3, c, -1, cv2.LINE_AA)
    cv2.putText(aus, titel, (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.42, (255, 255, 255), 1, cv2.LINE_AA)
    return aus


def bild(vor, mod, pfad, weitere=None):
    """Messblatt-Bild: oben die fuenf Vorlagebilder, unten das Modell in P1, P2, PV (frei gezeichnet) + P1 in der Szene."""
    oben = [kachel(v['_bild'], v, 0.5, 136, 3.0, '%s (%s)' % (v['bild'], v['video'])) for v in vor.values()]
    unten = [kachel(v['_bild'], v, 1.0, 136, 3.0, 'Modell %s frei' % pn) for pn, v in mod.items()]
    unten += [kachel(v['_voll'], v, 1.0, 136, 3.0, 'Modell %s Szene' % pn) for pn, v in mod.items()][:2]
    for extra in (weitere or []):
        unten.append(extra)
    n = max(len(oben), len(unten))
    leer = np.full_like(oben[0], 40)
    z1 = np.hstack(oben + [leer] * (n - len(oben)))
    z2 = np.hstack(unten[:n] + [leer] * (n - len(unten)))
    cv2.imwrite(pfad, np.vstack([z1, z2]))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', default=V.ASSET)
    ap.add_argument('--json')
    ap.add_argument('--bild')
    a = ap.parse_args()
    vor = {k: vorlage(k) for k in VORLAGE_BILDER}
    for k, v in vor.items():
        zeige('birkin%02d (%s)' % (k, v['video']), v)
    mo = V.modell(open(a.emd, 'rb').read())
    mod = {pn: modell(mo, pose) for pn, pose in POSEN.items()}
    for pn, v in mod.items():
        zeige('Modell ' + pn, v)
    res = dict(emd=V.sha(open(a.emd, 'rb').read()), vorlage={k: roh(v) for k, v in vor.items()}, modell={k: roh(v) for k, v in mod.items()})
    if a.json:
        json.dump(res, open(a.json, 'w'), indent=1)
    if a.bild:
        bild(vor, mod, a.bild)
    return res


if __name__ == '__main__':
    main()
