"""SF V8 Koerperbau nach Nutzer-Skizze (Bauer, 2026-11-08): baut EM30_V8.EMD aus V7 (Spiel-Asset v0.8.30, sha256 319bbd17).

    python v8_bau.py [--basis EMD] [--aus DIR] [--name NAME] [--json PROT] [--stufen srea]

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
   alle 7 Glieder gleich, Radien je Gelenk). EMR Bank 0: relpos Knochen 16 = Wurzel, 17-22 = (0, L, 0); Keyframe-Drehungen (Runde 2):
   Knochen 16 = Ruhedrehung in jedem Keyframe; Knochen 17-22 haengen im Modellraum wie in PV (mit dem Kurs von Knochen 8 um die
   Hochachse gedreht), je Gelenk <= 25 Grad, Bodenregel durch gemeinsames Hochdrehen der Zielrichtungen (Knochen 16 fest).
   Knochen 0-15 und alle anderen Bits bytegleich. Meshes 16-22 neu aus den Radien (Ringe auf der
   Gehrung der Ruhekette, Manschette 0,45 r, Deckel wie V6, Dorn 0,8 r); Flaechen, UV, CLUT-Zeile 487 und Eckenzahl bleiben.
Stufe r) Ruecken (Messblatt 6b): Ecken von Mesh 8 aus ruecken_v8.json (ruecken_suche.py).
Stufe e) Runde 3 (O5): Wurzelrohr-Ende (Ring 0) und Kegelspitze von Mesh 16 unter die Rueckenhaut (einbetten).
Stufe w) Runde 4 (S10/O5, wurzel_r4.py): Ring 1 von Mesh 16 entlang der Gliedachse an die Haut (15 E. darunter), Ring 0 und Kegelspitze auf
   Ring 1 - das Wurzelrohr hat keine Flaeche mehr (der Port malt in Maler-Reihenfolge, das Rohr im Rumpf wurde ueber die Haut gemalt).
   Ersetzt Stufe e. Bau Runde 4: --stufen srwa.
Stufe u) Runde 5 (P4, unterseite_r5.py + unterseite_v8.json): Flaechen der Schwanz-Unterseite (Ringecke |theta| < 125 Grad gegen die
   Innenrichtung des Bogens) auf die Original-Zeile 482 (Spiel-Asset 495) mit UV auf den Haut/Fleisch-Verlauf des Original-Unterarms (Seite 2,
   u 6-31, v 50-106); Oberseite bleibt Zeile 487. Bau Runde 5: --stufen srwaun (n = Einsatz an Mesh 8, nacken_r5.py).
Stufe h) Runde 6a (P6, hand_r6.py + hand_v8.json): linke Hand (Mesh 12) = Original-Klauenhand des RE1.5-Birkin in der Groesse und Drehung um
   die Handachse, die das Messblatt Hand verlangt (hand_suche.py waehlt nach Zahl), linker Unterarm (Mesh 11) = Original mit Ringverlauf.
   Nur Ecken und Normalen, Flaechen/UV/CLUT-Zeilen (490 Hand, 491 Daumenkralle, 492 Fingerkrallen) bleiben. Bau Runde 6: --stufen srwaunh.
Stufe a) Runde 4 (adern_r4.py, modus "original_r4"): wie Runde 3, dazu ganz Mesh 8 ausser der Brust auf den Original-Zeilen (Naht rosa/grau)
   und ganze UV-Gruppen mit einem Schub (Feld C von Mesh 13, Unterarmband von Mesh 14, Huegel und Wurzelstreifen von Mesh 8) auf Texel, die
   Original-Flaechen derselben Zeile benutzen.
Stufe a) Runde 3 (O1/O2, adern_r3.py, modus "original"): Zeilen 493-495 = Original-EM30-Zeilen 480-482 bytegleich auf den (W)-rechten Flaechen
   von Mesh 8/13/14 (ohne Feld C und Brust), Huegel-Flaechen mit UV-Schub auf Original-Fleisch. Runde 2 (modus fehlt): Zeilen X1/X2 = V7-Zeile 480/481 + die Original-EM30-Ader-Eintraege derselben Zeile (a* >= 15,
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
# Runde 2 Manschette (Messung sfv8/r2, Lauf k4 + r8 in PV): die Manschette des Glieds g+1 liegt auf dem Gehrungsring des Gelenks, entlang
# des VORGLIEDS zurueckgesetzt. Ruecksetzung 0,45 ra (V5f) und Radius 0,97: Oberkante hinter dem Gelenk 17 3,2 px unter ihrer Huelle (Delle
# bei x 329, Riegel S2b); 0,15-0,2 ra und Radius 0,99-1,0: Delle dort weg (Sattel wandert nach x 364, 3,0). Gewaehlt 0,2 ra / 0,99:
# deckt beim Nachknicken bis etwa 0,2 rad (11 Grad) gegen die Ruhe ohne Spalt; 1 % enger als das Vorglied (gleich dicke Glieder s 90-130
# wuerden sonst mit dem Vorglied zusammenfallen). PORT-WAHL.
MANSCHETTE_R = 0.99
MANSCHETTE_S = 0.2


def glied_mesh(m_alt, g, L, ra, re, n_anf, n_end, hinten=UEBERLAPP, a_vor=None):
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
    if a_vor is not None:
        # Runde 2 (Riegel S2b, Messung sfv8/r2 vtx.py: die Manschette von Glied 17 lag zurueckgesetzt entlang der EIGENEN Achse und stand
        # in PV 5-7 px ueber der Oberkante des Vorglieds - ein Huckel an jedem Gelenk auf der Aussenseite des Bogens): Manschette = der
        # Gehrungsring des Gelenks, entlang der Achse des VORGLIEDS (a_vor, im eigenen Raum) um UEBERLAPP x ra zurueckgesetzt und um
        # MANSCHETTE_R enger - in Ruhe liegt sie im Vorglied, beim Nachknicken fuellt sie den Spalt wie bisher.
        q = ring(0.0, ra, n_anf, phis[0])
        a = a_vor / np.linalg.norm(a_vor)
        ax_ = (q @ a)[:, None] * a[None, :]
        ringe.append(ax_ + MANSCHETTE_R * (q - ax_) - MANSCHETTE_S * ra * a[None, :])
    else:
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


def rohr_drehung(lok1, grad):
    """Runde 2 (Riegel S2b/S2c, Messung sfv8/r2 probe_b.py: das gerade Rohr lag in PV waagerecht unter dem Buckel, dessen Oberkante 6-9 px
    darueber; kuerzer machen vergroesserte die Stufe auf 13-22 px): das Wurzelrohr darf in der Biegeebene des Gelenks 17 um `grad` gegen
    die Gliedachse abknicken (+ = zur Aussenseite des Bogens, also zum Buckel hin). Rueckgabe: Drehung um Gelenk 16 (Gliedraum 16), die
    Ring 0 und Deckel dorthin bringt. Der Winkel ist das Ergebnis der Feinsuche (kette_v8.json "rohr_grad"), PORT-WAHL.
    Nutzer: "Tail directly smooth into the back - no hill or anything..."."""
    d1 = lok1 @ Y
    n = np.cross(Y, d1)
    sn = np.linalg.norm(n)
    if sn < 1e-9 or not grad:
        return np.eye(3)
    return BF._exp(n / sn * np.radians(float(grad)))


def wurzel_mesh(m18, L, ra, re, n_end, hinten, M0=None, verj=WURZEL_VERJUENGUNG):
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
    rb = verj * ra
    yb = -hinten * ra
    neig = [(rb - ra) / (hinten * ra), (ra - re) / L]

    def ring(cy, rr, n, phi):
        off = np.stack([np.cos(phi) * rr, np.zeros_like(phi), np.sin(phi) * rr], 1)
        t = -(off @ n) / n[1] if n is not None else 0.0
        return off + np.array([0.0, cy, 0.0]) + (t[:, None] * Y if n is not None else 0.0)

    M0 = np.eye(3) if M0 is None else M0
    ringe = [ring(yb, rb, None, phis[0]) @ M0.T, ring(0.0, ra, None, phis[1]), ring(L, re, n_end, phis[2])]
    neu = np.vstack(ringe)
    nn = []
    for r, ng in zip(range(3), (neig[0], 0.5 * (neig[0] + neig[1]), neig[1])):
        v = np.stack([np.cos(phis[r]), np.full_like(phis[r], ng), np.sin(phis[r])], 1)
        v = v / np.linalg.norm(v, axis=1)[:, None]
        nn.append(v @ M0.T if r == 0 else v)
    nn = np.vstack(nn)
    for k in range(3 * S_):
        m['v'][k][:3] = [int(round(x)) for x in neu[k]]
        m['n'][k][:3] = [int(round(x * 4096)) for x in nn[k]]
    for i in range(3 * S_, len(m['v'])):
        if m['v'][i][1] > 0:
            m['v'][i][:3] = [int(round(x)) for x in ringe[2].mean(0)]
        else:
            m['v'][i][:3] = [int(round(c)) for c in M0 @ np.array([0.0, yb - WURZEL_DECKEL * rb, 0.0])]
            m['n'][i][:3] = [int(round(c * 4096)) for c in M0 @ np.array([0.0, -1.0, 0.0])]
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
# Runde 2 (Pruefung V8 r1): die Keyframes der Knochen 16-22 tragen NICHT mehr die V7-Bewegung (Keyframe gegen Clip-Median). Befunde:
#  - Sicht-Pruefer (1b): in P2 (Clip 12 Bild 48) lief der Schwanz fast waagerecht nach links und endete in halber Hoehe mit einem Haken
#    (Spitze ~(247,273) statt Beschreibung2 "Schwanzspitze Ende" ~(282,320));
#  - Sicht-Pruefer (5): F322 Balken quer ueber dem Ruecken, F328-F340 senkrecht "wie ein Horn"; graue Dreiecke des Wurzelrohrs stechen
#    durch die Rueckenhaut (Bodenregel drehte Knochen 16 in 151 Keyframes bis 90 Grad, die V7-Bewegung drehte ihn mit);
#  - Zahlen-Pruefer: 714 von 912 Keyframes mit einem Gelenk > 25 Grad (bis 95,5).
# Neu (PORT-WAHL; Messung: Messblatt V8 "Schwanzband steht n1435-n1440 still", Foto 2 = n1435 und die Vorlage n1439 im selben Schnitt):
#  - Knochen 16 (Wurzelrohr) bleibt in JEDEM Keyframe auf seiner Ruhedrehung gegen Knochen 8 - das Rohr liegt immer so im Rumpf wie in PV.
#  - Die Glieder 17-22 haengen in jedem Keyframe so im Modellraum wie in PV (Richtung je Glied aus PV), nur um die Hochachse mit dem
#    Rumpf mitgedreht (Kurs der x-Achse von Knochen 8 gegen PV): der Schwanz faellt in jeder Pose nach hinten unten zum Boden.
#  - Jedes Gelenk 17-22 knickt hoechstens WINKEL_MAX_GRAD (Bauplan 6a "je hoechstens 25 Grad"); reicht das nicht, nimmt das naechste
#    Gelenk den Rest (der Bogen bleibt gleichsinnig).
#  - Boden (Riegel S6): liegt Ring + Radius, Spitze oder Dornspitze unter der tiefsten Koerperecke, werden alle Zielrichtungen gemeinsam
#    um eine waagerechte Achse hochgedreht (1 Grad je Schritt), bis der Schwanz frei ist - Knochen 16 bleibt dabei fest.
WINKEL_MAX_GRAD = 25.0


def kurs(R):
    """Kurs der x-Achse (vorn/hinten) eines Knochens im Modellraum um die Hochachse (Winkel wie Rot_y(t) @ (1,0,0) = (cos t, 0, -sin t))."""
    v = np.asarray(R, float)[:, 0]
    return float(np.arctan2(-v[2], v[0]))


def rot_y(t):
    c, s_ = np.cos(t), np.sin(t)
    return np.array([[c, 0.0, s_], [0.0, 1.0, 0.0], [-s_, 0.0, c]])


def begrenze(d, wmax):
    """Richtung d (Gliedraum) auf hoechstens wmax (Bogenmass) gegen die Gliedachse y begrenzen (in der Ebene y, d)."""
    d = d / np.linalg.norm(d)
    c = float(np.clip(d @ Y, -1, 1))
    if np.arccos(c) <= wmax:
        return d
    q = d - c * Y
    n = np.linalg.norm(q)
    q = q / n if n > 1e-9 else np.array([1.0, 0.0, 0.0])
    return Y * np.cos(wmax) + q * np.sin(wmax)


def glieder_aus_richtungen(W16, D, wmax):
    """Lokaldrehungen der Knochen 17-22: Glied g zeigt im Modellraum moeglichst auf D[g-1], jedes Gelenk <= wmax; quantisiert."""
    W, lok = W16, []
    for g in range(6):
        d = begrenze(W.T @ D[g], wmax)
        Lg = BF._quant(BF._min_rot(Y, d))
        lok.append(Lg)
        W = W @ Lg
    return lok


def schwanz(blob, d, ms, par, prot, nur_kf=None):
    mo = V.modell(blob)
    s = mo.skel
    assert s.bone_count == 23 and s.kf_size == 116, (s.bone_count, s.kf_size)
    P = np.array(par['gelenke_b8'], float)
    L = int(par['L'])
    rad = [float(r) for r in par['radien']]
    lok_ruhe = ruhe_lokal(P)
    kpv = BB_kf(mo, V.PV)
    wmax = np.radians(WINKEL_MAX_GRAD)
    emr = bytearray(blob[d[2]:d[3]])
    o16 = 8 + 16 * 6
    struct.pack_into('<3h', emr, o16, *[int(round(x)) for x in P[0]])
    for b in range(17, 23):
        struct.pack_into('<3h', emr, 8 + b * 6, 0, L, 0)
    # Zielrichtungen aus PV (Modellraum; Knochen 0-15 sind die von V7, die Pose von Knochen 8 bleibt)
    Rp, Tp = mo.pose(kpv)
    R8p = Rp[8].astype(float) / 4096.0
    W, Dpv = R8p @ lok_ruhe[0], []
    for g in range(1, 7):
        W = W @ lok_ruhe[g]
        Dpv.append(W @ Y)
    Dpv = np.array(Dpv)
    k_pv = kurs(R8p)
    blob2 = bytes(blob[:d[2]]) + bytes(emr) + bytes(blob[d[3]:])
    n_end, n_anf = gehrungen(lok_ruhe)
    alt = copy.deepcopy(ms)
    for g in range(7):
        if g == 0:
            ms[16] = wurzel_mesh(alt[18], L, rad[0], rad[1], n_end[0], float(par['hinten']),
                                 M0=rohr_drehung(lok_ruhe[1], par.get('rohr_grad', 0.0)),
                                 verj=float(par.get('verjuengung', WURZEL_VERJUENGUNG)))
        else:
            ms[16 + g] = glied_mesh(alt[16 + g], g, L, rad[g], rad[g + 1], n_anf[g], n_end[g], a_vor=lok_ruhe[g].T @ Y)
    dorn_y = float(L + DORN * rad[7])
    mo2 = V.modell(blob2)
    rel = [np.array(mo2.skel.rel[b], float) for b in V.SCHWANZ]
    gehoben, kmax = {}, []
    for k in (range(s.kf_count) if nur_kf is None else nur_kf):     # nur_kf: Suche (nur PV), nie im Bau
        R, T = mo2.pose(k)
        R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
        boden = tiefste_koerperecke(mo2, R, T) - float(par.get('boden_rand', 0))
        Ry = rot_y(kurs(R8) - k_pv)
        D = Dpv @ Ry.T
        W16 = R8 @ lok_ruhe[0]
        loks = [lok_ruhe[0]] + glieder_aus_richtungen(W16, D, wmax)
        if kette_tief(R8, T8, loks, rel, L, rad, dorn_y) > boden:
            pts, _W = kette_punkte(R8, T8, loks, rel, L)
            h = pts[-1] - pts[1]
            ax = np.cross(np.array([h[0], 0.0, h[2]]), Y)
            ax = ax / max(np.linalg.norm(ax), 1e-9)
            ok = None
            for n in range(1, 91):
                for sg in (1, -1):
                    Rh = BF._exp(ax * np.radians(n) * sg)
                    l2 = [lok_ruhe[0]] + glieder_aus_richtungen(W16, D @ Rh.T, wmax)
                    if kette_tief(R8, T8, l2, rel, L, rad, dorn_y) <= boden:
                        ok = (n * sg, l2)
                        break
                if ok:
                    break
            if ok:
                loks = ok[1]
            gehoben[k] = ok[0] if ok else None
        for i, b in enumerate(V.SCHWANZ):
            winkel_schreiben(emr, s, k, b, loks[i])
        kmax.append(max(float(np.degrees(np.arccos(np.clip((Lg @ Y) @ Y, -1, 1)))) for Lg in loks[1:]))
    blob3 = bytes(blob2[:d[2]]) + bytes(emr) + bytes(blob2[d[3]:])
    prot['schwanz'] = dict(L=L, wurzel=[int(round(x)) for x in P[0]], radien=[round(r, 1) for r in rad], dorn_y=dorn_y,
                           kf_pv=kpv, bodenregel=dict(keyframes=len(gehoben), grad={str(k): v for k, v in sorted(gehoben.items())}),
                           gelenk_max_grad=dict(max=round(max(kmax), 2), median=round(float(np.median(kmax)), 2)),
                           gelenkwinkel_ruhe=[round(float(np.degrees(np.arccos(np.clip((Lr @ Y) @ Y, -1, 1)))), 1) for Lr in lok_ruhe])
    return blob3


def BB_kf(mo, pose):
    return V.BB.kf_von_clip(mo, pose['clip'], pose['bild'])


# ------------------------------------------------------------------ Stufe e (Runde 3, O5)
# Sicht-Pruefer V8 r2 (5): "an der Wurzel sticht weiterhin ein grauer Keil mit harter Kante in die Rueckenhaut (F328-F346)"; Orchestrator O5
# "Wurzelring/Manschette ganz in die Rueckenflaeche einbetten". Gemessen (sfv8/r3 gap.py, V8 r2): Ring 0 des Wurzelrohrs bis 121 E., der
# Wurzelring (Ring 1) bis 175 E. ausserhalb von Mesh 8; offline von hinten (Takt F328) waren Kegel und Rohr (Mesh 16 d13-d17, v1-v7) sichtbar.
# Regel: jede Ecke von Ring 0 (Ende des Wurzelrohrs) und der Kegelspitze (Mesh 16), die in der Ruhelage gegen Knochen 8 (Knochen 16 liegt in
# jedem Keyframe auf seiner Ruhedrehung, Stufe s) weniger als WURZEL_TIEF unter der Haut von Mesh 8 liegt, wird auf der Geraden zur Mitte von
# Ring 0 so weit nach innen gezogen, bis sie WURZEL_TIEF tief liegt (Halbierung, 24 Schritte): Rohr und Kegel (die Flaechen, die von hinten als
# Keil durch die Haut stachen) liegen ganz im Rumpf. Der Wurzelring (Ring 1) bleibt - ihn einzuziehen riss in PV die Naht an J auf (Lauf
# sfv8/r3 kf2: Sattel 4,1, Naht 5,0 statt <= 2 / <= 3); seine Lage legt die Feinsuche (kette_fein.py, Wurzel-Riegel S10 von hinten). Normalen, Flaechen, UV bleiben. PORT-WAHL (Nutzer: "Tail directly smooth into
# the back - no hill or anything...").
WURZEL_TIEF = 20.0


def einbetten(blob, ms, prot):
    import r3_gemein as R3
    mo = V.modell(blob)
    kf = BB_kf(mo, V.PV)
    R, T = mo.pose(kf)
    R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
    R16, T16 = R[16].astype(float) / 4096.0, T[16].astype(float)
    A = R8.T @ R16; t = R8.T @ (T16 - T8)                         # Knochenraum 16 -> 8
    tri = R3.dreiecke(ms[8])
    m = ms[16]
    P = np.array([v[:3] for v in m['v']], float)
    gruppen = [list(range(0, S_)), [len(P) - 1]]
    mitten = [P[0:S_].mean(0), P[0:S_].mean(0)]
    liste = []
    for g, c in zip(gruppen, mitten):
        for i in g:
            q = P[i] @ A.T + t
            dd, sg = R3.abstand_innen(q, tri)[0]
            if dd * sg >= WURZEL_TIEF:
                continue
            cq = c @ A.T + t
            lo, hi = 0.0, 1.0                                          # Anteil Richtung Mitte
            if R3.abstand_innen(cq, tri)[0][0] * R3.abstand_innen(cq, tri)[0][1] < WURZEL_TIEF:
                hi = 1.0
            for _ in range(24):
                mid = 0.5 * (lo + hi)
                d2, s2 = R3.abstand_innen(q + (cq - q) * mid, tri)[0]
                if d2 * s2 >= WURZEL_TIEF:
                    hi = mid
                else:
                    lo = mid
            neu = P[i] + (c - P[i]) * hi
            m['v'][i][:3] = [int(round(x)) for x in neu]
            liste.append(dict(ecke=i, tiefe_vorher=round(dd * sg, 1), anteil=round(hi, 3)))
    prot['einbetten'] = dict(wurzel_tief=WURZEL_TIEF, ecken=liste)


# ------------------------------------------------------------------ Bau
def bauen(basis, stufen='sra', kette=None, ruecken=None, adern=None, nur_kf=None, blob=None, unterseite=None, nacken=None, hand=None):
    blob = open(basis, 'rb').read() if blob is None else blob
    prot = dict(basis=basis, basis_sha256=V.sha(blob), stufen=stufen)
    d, _do, ms, tim = KB.teile(blob)
    if 's' in stufen:
        blob = schwanz(blob, d, ms, kette or _p('kette_v8.json'), prot, nur_kf=nur_kf)
        d, _do, _ms, _tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    if 'r' in stufen:
        import ruecken_v8 as RV
        RV.ruecken(ms, ruecken or _p('ruecken_v8.json'), prot)
    if 'e' in stufen:
        einbetten(KB.packen(blob, d, ms, tim), ms, prot)
    if 'w' in stufen:                 # Runde 4 (S10/O5): Ring 1 an die Haut, Wurzelrohr und Kegel ohne Flaeche (wurzel_r4.py); Runde 5: optional
        import wurzel_r4 as W4        # kette_v8.json "ring1_versatz" (im Spiel-Asset Runde 5 nicht gesetzt, Probe verworfen: wurzel_r4.KEGEL_UMKEHR)
        W4.stufe_w(KB.packen(blob, d, ms, tim), ms, prot, ring_versatz=(kette or _p('kette_v8.json')).get('ring1_versatz'))
    if 'a' in stufen:
        import adern_v8 as AV
        AV.adern(ms, t, adern or _p('adern_v8.json'), prot)
    if 'u' in stufen:                 # Runde 5 (P4, Nutzer 2026-11-09 "Die Unterseite des großen Schwanzes ist fleischig ... Nur die oberseite ist
        import unterseite_r5 as U5    # durchweg grau"): Unterseite auf den Haut/Fleisch-Verlauf des Original-Unterarms (unterseite_r5.py)
        U5.anwenden(ms, t, unterseite or _p('unterseite_v8.json'), prot)
    if 'n' in stufen:                 # Runde 5d (P5, Auftrag (iii)): kleiner geschlossener Einsatz an Mesh 8 gegen den Keil in F332,
        import nacken_r5 as N5        # Haut = Texel der Wirtsflaeche (nacken_r5.py, nacken_v8.json); nach a/u, damit Zeile und Seite stehen
        N5.anwenden(ms, t, nacken or _p('nacken_v8.json'), prot)
    if 'h' in stufen:                 # Runde 6a (P6, Nutzer 2026-11-10 "Die Vorlage hat eine breite Hand mit breiten Unterarmt ... Bei uns ist die
        import hand_r6 as H6          # Hand ganz klein und zierlich"): Original-Klauenhand und Original-Unterarm in der Groesse des Messblatts
        H6.anwenden(ms, t, hand or _p('hand_v8.json'), prot)   # (hand_r6.py, hand_v8.json); aendert nur Ecken und Normalen von Mesh 11 und 12
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
    ap.add_argument('--stufen', default='srwaunh')
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
