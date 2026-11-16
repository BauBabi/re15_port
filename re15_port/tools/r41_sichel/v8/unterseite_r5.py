"""SF V8 Runde 5 (P4): Unterseite des grossen Schwanzes fleischig, Oberseite grau -> unterseite_v8.json; Bau-Stufe u (v8_bau.py).

    python unterseite_r5.py messen [EMD] [--aus JSON]     Messung an den Vorlagen (n1439 / Foto 2 n1435 / Foto 1 n1485), Tafel-Rohdaten
    python unterseite_r5.py json [EMD] [--aus JSON]       Innenrichtung je Glied aus der Pose PV -> unterseite_v8.json (Standard: neben dieser Datei)

Nutzer 2026-11-09 nach Ansicht der V8-Vorschau (woertlich, bindend): "Der 2 Beiner Birkin sieht in der Vorschau schon mal super aus. Nur eine
Sache passt noch nicht so ganz. Die Unterseite des großen Schwanzes ist fleischig wenn man in die Vorlagen Bilder schaut. Nur die oberseite
ist durchweg grau."
Regel (Memory "Texturen nie aus Videobildern eichen", Nachtraege 2026-11-08/09): Material NUR aus Original-Texeln des RE1.5-Birkin (EM30 aus
CDEMD0.EMS, Typ 0x30) mit der Palette der Original-Zeile; keine gemalten Inseln, keine Eichung je Fleck/Region am Video. Das Video liefert nur
den Ort (welche Seite des Schwanzes rot ist) und die Farbrichtung.
Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Jede Zahl ist PORT-WAHL, belegt mit dem
Nutzer-Wortlaut oder der Messung unten (Protokoll SF_belege/v8/runde5/unterseite_messung.txt, Messblatt V8 Abschnitt 10).

Mass "Lage um die Achse" theta (Grad): Winkel zwischen der Flaechennormale (senkrecht zur Gliedachse) und der Richtung von der Gliedmitte zum
Mittelpunkt des Bogens (Kreis durch die Gelenke 17-22 + Spitze in der Pose, kreis_mitte). theta 0 = Innenseite des Bogens (zum Koerper / zum
Boden), theta 180 = Aussenseite (Oberseite, Ruecken des Schwanzes). Der Bogen liegt in PV fast in einer Ebene; das Mass ist symmetrisch (|theta|).

Messung (messen(), Spiel-Asset V8 Runde 5 Geometrie = Runde 4, Pose PV / P2 / P1 wie Messblatt V8, Spielkamera 50A0 Cut 12; Vorlage-Lab je
Bildpunkt gemittelt ueber n1438-n1440 bzw. n1434-n1436; Schwanzmaske des Modells (Knochen 16-22) 2 px erodiert):
  - PV (n1438-n1440, alle Knochen): theta 70-110 a* Median 6,6-9,7, Anteil a* >= 10 25,7-48,8 % (rot/fleischig); theta 110-120 a* 4,5,
    1,1 %; theta >= 120 a* 1,4-4,2, 0 %, L* 22,9-38,4 (grau, hell) -> Grenze grau/fleischig bei theta ~115 (Uebergang 110-120).
  - P2 (n1434-n1436) gleich: theta 70-110 a* 5,7-8,0, 15,6-31,5 %; 110-120 a* 4,3, 1,0 %; >= 120 a* <= 3,0, 0 %.
  - theta < 60 (Innenseite selbst) dunkel (L* 8-12, a* 0-4): im Schatten, Farbe nicht lesbar; die Regel nimmt sie als fleischig (Nutzer:
    "Unterseite ... fleischig").
  - Laengs (theta 60-115 je Knochen, PV): 16 a* 12,8 / 75,8 %, 17 10,3 / 56,6 %, 18 7,6 / 10,2 %, 19 4,9 / 2,9 %, 20-22 a* -0,0-1,6 / 0 %
    bei L* 10-11 (dunkel; Bildzeilen y 250-285 an der Innenkante a* 4-11, darunter 0,5-2) -> sicher rot von der Wurzel bis Knochen 18;
    die Regel gilt bis zur Spitze (keine neue Querkante mitten im Schwanz; Nutzer "Die Unterseite des großen Schwanzes"), Offen.
  - Foto 1 (P1, von vorn): vom Schwanz sind nur 215 Bildpunkte (Knochen 20-22, dunkel, a* -2,6) sichtbar - keine Aussage.

Material (Original-EM30, birkin_formen.lade_em30): Seite 2, u 0-43 = Unterarm/Klaue des Original-Birkin (Original-Flaechen der Meshes 11, 12,
14, 15, alle Zeile 482), Texel bytegleich im Spiel-Asset (gemessen: 0 abweichende Texel in den von Original-Flaechen benutzten Texeln). Unter
der Original-Zeile 482 (= Spiel-Asset Zeile 495, bytegleich) verlaeuft dort entlang v: graue Haut mit Adern (v 45-85, L* 12-23, a* 4-8),
eine gewellte Kante (v 86-95) und dunkelrotes Fleisch mit weichen Streifen (v 96-120, L* 17-26, a* 20-26) - der Uebergang Haut/Fleisch des
Original-Birkin. Die Unterseite bekommt GENAU diesen Verlauf: auf den Uebergangsflaechen (Mittel |theta| 100-130) folgt v dem theta (V_GRAU an der Grenze ->
V_FLEISCH_AB -> V_TIEF innen) und u der Gliedachse (U0 -> U1, je Glied gespiegelt, damit an jedem Gelenk dieselbe Texelspalte anliegt); auf
den Fleischflaechen (Mittel |theta| < FLEISCH_KL) laufen die weichen Streifen des Original-Fleischs entlang der Gliedachse (v VF0 -> VF1 je Glied
gespiegelt, u um den Umfang). Probe U4 (alles quer, v folgt theta): regelmaessige rote Querriefen alle ~8 px, die die Vorlage nicht zeigt
(Tafel 14); laengs (U8): ein ruhiges dunkelrotes Band. Die Oberseite (alle Ecken |theta| >= GRAU_AB)
bleibt auf Zeile 487 mit unveraenderter UV (V6/V7, "dunkel gruenlich-grau").
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402
import adern_r4 as A4                 # noqa: E402

S_ = 11                               # Ecken je Ring (v8_bau.S_)
SCHWANZ_ZEILE = 487                   # V6/V7-Schwanzzeile (Oberseite, bleibt)
ORIG_ZEILE = 482                      # Original-Zeile des Unterarm-Verlaufs (Seite 2, u 0-43); im Spiel-Asset die neue Zeile mit gleichem Inhalt
SEITE = 2                             # Textur-Seite des Verlaufs (Original-Meshes 11/12/14/15)
# Grenze und Verlauf (PORT-WAHL aus der Messung oben, PV und P2 gleich):
GRAU_AB = 125.0                       # |theta| >= 125: Oberseite grau (Vorlage: ab 120 a* <= 4,2 und 0 % rot; 5 Grad Abstand zur Uebergangszone)
KLASSE_AB = 130.0                     # Flaeche grau (Zeile 487, UV unveraendert), wenn das Mittel ihrer Ringecken |theta| >= 130: die Ringecken
                                      # liegen alle 32,7 Grad (11 je Ring) bei |theta| ~ 59/92/125/157 bzw. 72/105/137/170 (gemessen, innen_phi);
                                      # mit "alle Ecken >= 125" kippte der Sektor 125-157 an Ecken bei 118,9-124,6 auf die Unterseite (Probe U3: in PV
                                      # theta 135-150 0 % grau). Mittel 130 -> Sektoren 125-157 und 137-170 grau, 92-125 und 105-137 Uebergang.
FLEISCH_AB = 100.0                    # |theta| <= 100: Fleisch voll (Vorlage: 70-110 rot 28-58 %); 125 -> 100 = Uebergang (Vorlage 110-120)
V_GRAU = 64.0                         # Texelzeile an der Grenze: dunkelgraue Haut ohne violette Adern (Original-Zeile 482 v 60-85: L* 12-16, a* 3,8,
                                      # b* 0-1,4; Probe U1 mit v 50 = violetter Adernstreifen v 45-60, b* -3,4..-3,8, in PV als violettes Band sichtbar)
V_FLEISCH_AB = 90.0                   # Zeile an der Fleischgrenze: die gewellte Kante Haut/Fleisch (v 86-95; v 90 a* 14,5)
V_TIEF = 96.0                         # Innenseite: erste ganze Fleischzeile (v 96 a* 25,5). Nicht tiefer: Probe U2 (bis v 102) zeigte das Fleisch
                                      # hellrot mit kraeftigen Querriefen; die Vorlage zeigt die Unterseite dunkelrot (Farbrichtung, n1439 theta
                                      # 70-110 L* 9-11) - die Zeilen an der Kante sind die dunkleren (Haut/Fleisch gemischt)
LAENGS = True                         # Fleischflaechen (Mittel |theta| < FLEISCH_KL) laengs abgebildet - siehe UF0/UF1, VF0/VF1
FLEISCH_KL = 100.0                    # Sektoren 59-92 / 72-105 und tiefer (Mittel 75 / 88); der Sektor 92-125 (Mittel 108) traegt die gewellte Kante
UF0, UF1, TF = 4.0, 36.0, 110.0       # u um den Umfang: theta -110 .. +110 -> u 4 .. 36 (Nutzungsmaske v 84-106: u 0-40)
VF0, VF1 = 90.0, 100.0                # v entlang der Gliedachse im Fleisch, je Glied gespiegelt: v 90-100 = Fleisch ab der gewellten Kante (v 90 a* 14,5,
                                      # v 96-100 a* 25,5). Proben in PV (prof.py, Lab je theta-Band, Videoraum): v 94-104 (U7) theta 75-90 L* 22,9 a* 14,3
                                      # hellrot; v 90-100 (U8) L* 23,1 a* 13,3, theta 60-75 L* 12,4; v 86-96 (U9) Fleisch kaum sichtbar (a* 9,1/2,3).
                                      # Vorlage dort dunkelrot (L* 9-11, a* 7-10): Farbrichtung "dunkelrot" -> U8 (keine Eichung, nur die Wahl der
                                      # Original-Zeilen im Verlauf)
U0, U1 = 6.0, 31.0                    # Texelspalten entlang der Gliedachse: die Nutzungsmaske der Original-Flaechen (Zeile 482, Seite 2) reicht bei
                                      # v 50 von u 5 bis 32, bei v 51-106 von u <= 4 bis >= 32 (gemessen, Ausgabe in unterseite_messung.txt)


# ------------------------------------------------------------------ Runde 6 (P4-neu) - ueberschreibt die Grenzen der Runde 5 oben
# Auftrag nach der Pruefung der Runde 5 (Zahlen-Pruefer p4kante / p4b, Sicht-Pruefer F315-F324): "(a) RING-DECKEL: Deckel-Dreiecke an den
# Segmentgrenzen (heute Zeile 487, grau) folgen derselben Grenze wie der Mantel ... (b) GRENZE etwa 10 Grad tiefer: rot bis theta 110,
# Uebergang 110-120, grau ab 120; im Band 120-130 hoechstens 5 % Zeile 495 (heute 35,6 / 53,7 %)". Die Ringecken liegen alle 32,7 Grad
# (|theta| der Normalen ~105 / 137 und ~92 / 125): keine Zuordnung ganzer Flaechen trifft 110 / 120. Deshalb werden die Flaechen, die eine
# Grenze kreuzen, an ihr GETEILT (teilen()): neue Ecken auf den Ringsehnen dort, wo die interpolierte Normale (dasselbe Mass wie theta_bild
# je Bildpunkt) genau 110 bzw. 120 Grad hat. Danach liegt jede Flaeche ganz in einem der Baender < 110 (Fleisch, laengs), 110-120
# (Uebergang, v folgt theta) oder >= 120 (grau, Zeile 487, UV der Oberseite linear weitergefuehrt).
GRAU_AB = 120.0                       # Auftrag (b) "grau ab 120" (Vorlage: ab 120 a* <= 4,2 und 0 % rot, Messung im Kopf dieser Datei)
KLASSE_AB = 120.0                     # Flaeche grau, wenn das Mittel ihrer Ring-Ecken |theta| >= 120 (nach dem Teilen eindeutig)
FLEISCH_AB = 110.0                    # Auftrag (b) "rot bis theta 110" (Vorlage: 70-110 rot 28-58 %, 110-120 Uebergang a* 4,5)
FLEISCH_KL = 110.0                    # Flaeche laengs im Fleisch, wenn ihr Mittel < 110
TEIL_GRENZEN = (FLEISCH_AB, GRAU_AB)  # an diesen |theta| wird geteilt
TEIL_MIN = 1.0                        # Grad: naeher als 1 Grad an einer Ring-Ecke wird nicht geteilt (kein Splitter; gemessen kommt das nicht vor:
                                      # die Ring-Ecken liegen 5-17 Grad von den Grenzen entfernt)
TEIL_FANG = 10.0                      # Grad: kreuzt eine Grenze nur EINE der zwei Ringkanten eines Vierecks (gemessen: Ring 2 von Mesh 16 / 18 und
                                      # Ring 0 von Mesh 19, Ecke 8 bei -119 statt -125), laeuft sie am anderen Ring durch die naechste Ring-Ecke;
                                      # die darf hoechstens so weit von der Grenze liegen (gemessen 1 Grad)
DECKEL = 'teilen'                     # Ring-Deckel: 'teilen' (an der Grenze geteilt, unten Fleisch / oben grau) oder 'schwerpunkt' (ungeteilt, nach
                                      # dem Mittel der Ring-Ecken); beide gemessen (kanten_mass, s15_deckel) - Zahlen im Dossier "V8 Runde 6", P4
VF0, VF1 = 98.0, 108.0                # v entlang der Gliedachse im Fleisch. Runde 5 nahm v 90-100 "ab der gewellten Kante": die Texelzeilen 90-92 sind
                                      # aber noch Haut (a* 4-13 bei u 10-33, gemessen tex_u.py), und weil die Glieder abwechselnd gespiegelt sind, lagen
                                      # an jedem zweiten Gelenk zwei solche Zeilen aneinander = grauer Reifen auf dem MANTEL der Unterseite (F311, alle
                                      # Bildpunkte dort Klasse 'unten', Zeile 495). Ab v 98 ist jede Texelzeile bei u 4-36 Fleisch (a* >= 15); die
                                      # Nutzungsmaske der Original-Flaechen reicht bei u >= 4 bis v 108. Gleiche Texeldichte wie Runde 5 (10 Zeilen je
                                      # Glied). Keine Farbwahl am Video (Entscheid E4): die Runde-5-Proben U7 / U8 unterschieden sich in PV ohnehin nur
                                      # um L* 0,2.
V_KAPPE = 0.5 * (VF0 + VF1)           # Deckel im Fleisch: Texelzeile der Mittel-Ecke = Mitte von VF0 / VF1 (der Deckel bekommt eine echte Texelflaeche
                                      # zwischen der Ringkante und der Mitte, alles Original-Fleisch)
U_KAPPE = 5.0                         # Deckel im Uebergang: Texelspalte der Mittel-Ecke 5 Texel neben der Ringkante (zur Mitte von U0 / U1 hin)
S15_BAND = (120.0, 130.0)             # Riegel S15 (e): Band direkt ueber der Grenze
R7B_FANG_ZONE = 1.5                   # Runde 7b: Grad unter KLASSE_AB, in denen die TEIL_FANG-Ring-Ecken liegen (gemessen 118,8-119,4)
S15_BAND_SOLL = 5.0                   # Auftrag (b): "im Band 120-130 hoechstens 5 % Zeile 495"
S15_REIFEN_MIN = 50.0                 # Riegel S15 (g): "grauer Reifen auf dem Mantel" = eine Texelzeile quer zur Gliedachse (v fest) unter den laengs
                                      # belegten Fleischflaechen, in der weniger als die Haelfte der Texel Fleisch ist (a* >= FLEISCH_A)
S15_DECKEL_RAND = 2.0                 # Riegel S15 (f): grauer Deckel "auf der Unterseite" = Bildpunkt einer Deckel-Flaeche auf Zeile 487 mit
                                      # theta < 120 - 2 (2 Grad fuer die Rundung der Teilungs-Ecken und -Normalen auf ganze Zahlen)


def kreis_mitte(P):
    """Mittelpunkt des Kreises durch die Punkte P (Ausgleich in ihrer Hauptebene)."""
    c = P.mean(0)
    _U, _S, Vt = np.linalg.svd(P - c)
    e1, e2 = Vt[0], Vt[1]
    Q = np.c_[(P - c) @ e1, (P - c) @ e2]
    x = np.linalg.lstsq(np.c_[2 * Q, np.ones(len(Q))], (Q ** 2).sum(1), rcond=None)[0]
    return c + x[0] * e1 + x[1] * e2


def kette_pose(mo, pose):
    R, T = V.pose_r_t(mo, pose)
    L = float(mo.skel.rel[22][1])
    P = [T[b].astype(float) for b in V.SCHWANZ]
    P.append(P[-1] + R[22].astype(float)[:, 1] / 4096.0 * L)
    return R, T, np.array(P)


def innen_modell(mo, pose):
    """Je Glied g (0..6 = Knochen 16..22): Innenrichtung im Modellraum (senkrecht zur Gliedachse, zum Bogenmittelpunkt)."""
    R, T, P = kette_pose(mo, pose)
    C = kreis_mitte(P[1:])
    out = []
    for g in range(7):
        d = P[g + 1] - P[g]; d /= np.linalg.norm(d)
        k = C - 0.5 * (P[g] + P[g + 1]); k -= (k @ d) * d
        out.append((d, k / np.linalg.norm(k)))
    return R, T, P, out


def innen_glied(mo, pose=V.PV):
    """Innenrichtung je Knochen 16-22 im Knochenraum als Winkel phi_k in der Ringebene (x-z; Ringecke j liegt bei atan2(z, x))."""
    R, _T, _P, ik = innen_modell(mo, pose)
    out = {}
    for g, (_d, k) in enumerate(ik):
        b = 16 + g
        kb = (R[b].astype(float) / 4096.0).T @ k
        out[str(b)] = round(float(np.degrees(np.arctan2(kb[2], kb[0]))), 3)
    return out


def theta_ecke(p, phi_k):
    """|theta| (Grad) einer Ecke p (Knochenraum) gegen die Innenrichtung phi_k; None fuer Ecken auf der Achse."""
    r = np.hypot(p[0], p[2])
    if r < 1.0:
        return None
    d = (np.degrees(np.arctan2(p[2], p[0])) - phi_k + 180.0) % 360.0 - 180.0
    return abs(d)


def theta_signiert(p, phi_k):
    if np.hypot(p[0], p[2]) < 1.0:
        return None
    return (np.degrees(np.arctan2(p[2], p[0])) - phi_k + 180.0) % 360.0 - 180.0


def v_von(th):
    """Texelzeile v aus |theta|: Oberseiten-Grenze GRAU_AB -> V_GRAU, FLEISCH_AB -> V_FLEISCH_AB, Innenseite 0 -> V_TIEF (linear)."""
    if th >= GRAU_AB:
        return V_GRAU
    if th >= FLEISCH_AB:
        return V_GRAU + (GRAU_AB - th) / (GRAU_AB - FLEISCH_AB) * (V_FLEISCH_AB - V_GRAU)
    return V_FLEISCH_AB + (FLEISCH_AB - th) / FLEISCH_AB * (V_TIEF - V_FLEISCH_AB)


def wurzel_keys(m):
    import szene_marker as SM
    return set(SM.wurzel_flaechen(m))


def ring_ecke(b, i):
    """Ecke i von Mesh b liegt auf einem Ring: die Ring-Ecken des Baus (v8_bau.glied_mesh) und die in Runde 6 eingefuegten Teilungs-Ecken, die
    hinter den zwei Achs-Ecken (Deckel-Mitten bzw. Dorn-Spitze) stehen."""
    nr = 3 if b in (16, 18) else 2
    return i < nr * S_ or i >= nr * S_ + 2


def theta_normale(n, phi_k):
    """Signiertes theta (Grad) einer Ecken-Normale gegen die Innenrichtung - dasselbe Mass wie theta_bild je Bildpunkt (interpolierte Normale,
    senkrecht zur Gliedachse)."""
    if abs(n[0]) + abs(n[2]) < 1:
        return None
    return float((np.degrees(np.arctan2(n[2], n[0])) - phi_k + 180.0) % 360.0 - 180.0)


def _wickel(x):
    return (x + 180.0) % 360.0 - 180.0


def ecken_theta(m, b, f, phi):
    """Signiertes theta je Ecke der Flaeche f ([Normale, Ecke] abwechselnd); None fuer Achs-Ecken."""
    return [theta_normale(m['n'][f[2 * k]], phi) if ring_ecke(b, f[2 * k + 1]) else None for k in range(len(f) // 2)]


def _ring_y(m, b):
    nr = 3 if b in (16, 18) else 2
    nr = min(nr, (len(m['v']) - 2) // S_)        # Staende vor V8 (Gegenprobe V7: Mesh 16 hat 24 Ecken = 2 Ringe zu 11 + 2 Achs-Ecken,
                                                 # gemessen sfv8/r6c/riegel_v7.out: IndexError); im V8-Bau (35 / 24 Ecken, mit Teilung
                                                 # 43 / 32 / 47) bleibt es bei 3 bzw. 2 Ringen
    ys = [m['v'][i][1] for i in range(nr * S_)]
    return float(min(ys)), float(max(ys))


def ist_kappe(m, b, key, f, y0, y1):
    """Deckel/Kappe: Dreieck mit einer Achs-Ecke innerhalb des Glieds (v8_bau.glied_mesh). Dorn (Spitze hinter dem letzten Ring von Mesh 22)
    und Wurzelkegel zaehlen nicht dazu."""
    return key[0] == 'd' and any((not ring_ecke(b, i)) and y0 - 1.0 <= m['v'][i][1] <= y1 + 1.0 for i in f[1::2])


def flaechen_klassen(ms, par):
    """-> {b: [(key, klasse, [|theta| je Ecke])]} fuer Knochen 16-22. klasse: 'unten' (Mantel/Dorn, Mittel der Ring-Ecken < KLASSE_AB), 'grau'
    (>= KLASSE_AB; dazu die Wurzelflaechen von Mesh 16 - Rohr/Kegel im Rumpf, Marker S10s), 'kappe_unten' / 'kappe' (Ring-Deckel unter /
    ueber der Grenze). theta = Normalen-theta der Ecken (Runde 6; Runde 5 nahm den Ort der Ecke, 0-5 Grad daneben)."""
    out = {}
    for b in V.SCHWANZ:
        m = ms[b]
        phi = float(par['innen_phi'][str(b)])
        ausn = wurzel_keys(m) if b == 16 else set()
        y0, y1 = _ring_y(m, b)
        res = []
        for key, f in [('d%d' % i, f) for i, f in enumerate(m['tf'])] + [('v%d' % i, f) for i, f in enumerate(m['qf'])]:
            th = [None if x is None else abs(x) for x in ecken_theta(m, b, f, phi)]
            tr = [t for t in th if t is not None]
            if key in ausn and tr and par.get('fleisch_r7b') and float(np.mean(tr)) < KLASSE_AB:
                # Runde 7b (K2, Zahlen-Pruefer r6 "(D) graue Deckel ... 7 graue Dreiecke, alle Wurzel-Deckel in Mesh 16"): die Dreiecke des
                # Wurzelkegels (Spitze = Achs-Ecke 34, Ringecken auf Ring 0 = Ort von Ring 1) unter der Grenze sind Deckel der Unterseite und
                # bekommen Zeile und Texel ihrer Nachbarn (Mittel |theta| < 120: d11-d14, d19-d21, gemessen sfv8/r7b/deckel7.py); ebenso die
                # Vierecke des Wurzelrohrs darunter (Laenge 0 seit Runde 4, unsichtbar), sonst stuenden ihre Kanten als 495/487-Kanten am Kegel
                res.append((key, 'kappe_unten' if key[0] == 'd' else 'unten', th))
            elif key in ausn or not tr:
                res.append((key, 'grau', th))
            elif (par.get('fleisch_r7b') and len(tr) == len(th) and min(tr) >= KLASSE_AB - R7B_FANG_ZONE and max(tr) >= KLASSE_AB + 0.5
                  and float(np.mean(tr)) < KLASSE_AB):
                # Runde 7b: Teilstuecke der Abschnitts-Teilung (unterseite_r7b.mitte_teilen) aus einem grauen TEIL_FANG-Dreieck (Runde 6: die Grenze
                # laeuft dort durch die Ring-Ecke bei 118,8-119,4 statt 120) bleiben grau wie ihr Ausgangsdreieck; gemessen an der Probe U7
                # (3 Abschnitte): 18:d23 [119,4 118,8 121,7] und 19:d23 [120,8 118,8 119,2] fielen mit Mittel < 120 auf die Fleisch-Zeile und
                # zeigten in P2 Fleisch bis theta 121,7 (S15 e 6,2 %). Am Stand a474188a und am 2-Abschnitt-Bau gibt es solche Flaechen nicht.
                res.append((key, 'grau', th))
            elif ist_kappe(m, b, key, f, y0, y1):
                res.append((key, 'kappe_unten' if float(np.mean(tr)) < KLASSE_AB else 'kappe', th))
            elif float(np.mean(tr)) >= KLASSE_AB:
                res.append((key, 'grau', th))
            else:
                res.append((key, 'unten', th))
        out[b] = res
    return out


def teilen(ms, par):
    """Runde 6 (P4-neu b): Mantel-Vierecke, Dorn- und (Modus 'teilen') Deckel-Dreiecke, deren Ringkante eine Grenze TEIL_GRENZEN kreuzt, an ihr
    teilen. Neue Ecke = Punkt der Ringsehne, an dem die linear interpolierte Normale das Grenz-theta hat (Ort, Normale und UV linear
    interpoliert, auf ganze Zahlen gerundet; Normale auf Laenge 4096). Kreuzt die Grenze nur eine der zwei Ringkanten eines Vierecks, laeuft
    sie am anderen Ring durch die naechstliegende Ring-Ecke (Teilstueck = Dreieck). Das erste Viereck-Teilstueck ersetzt die Flaeche an ihrem
    Index, die weiteren stehen am Ende der Listen (die Indizes der Wurzelflaechen von Mesh 16 - Marker S10s - bleiben). -> Zaehlung je Mesh."""
    modus = par.get('deckel', DECKEL)
    assert modus in ('teilen', 'schwerpunkt'), modus
    zahl = {}
    for b in V.SCHWANZ:
        m = ms[b]
        assert len(m['v']) == len(m['n']), b
        phi = float(par['innen_phi'][str(b)])
        ausn = wurzel_keys(m) if b == 16 else set()
        y0, y1 = _ring_y(m, b)
        cache = {}
        n0 = (len(m['v']), len(m['tf']), len(m['qf']))

        def th(i):
            return theta_normale(m['n'][i], phi)

        def ecke_auf(lo, hi, g):
            """Neue Ecke auf der Ringkante lo -> hi (lo < hi) beim Normalen-theta g -> (Index, t von lo aus)."""
            if (lo, hi, g) not in cache:
                na = np.array(m['n'][lo][:3], float); nc = np.array(m['n'][hi][:3], float)
                f0 = _wickel(theta_normale(na, phi) - g)
                a_, b_ = 0.0, 1.0
                for _ in range(48):
                    mi = 0.5 * (a_ + b_)
                    fm = _wickel(theta_normale((1 - mi) * na + mi * nc, phi) - g)
                    if (fm > 0) == (f0 > 0):
                        a_ = mi
                    else:
                        b_ = mi
                t = 0.5 * (a_ + b_)
                nn = (1 - t) * na + t * nc
                nn = nn / np.linalg.norm(nn) * 4096.0
                pa = np.array(m['v'][lo][:3], float); pc = np.array(m['v'][hi][:3], float)
                pp = (1 - t) * pa + t * pc
                m['v'].append([int(round(float(x_))) for x_ in pp] + [0])
                m['n'].append([int(round(float(x_))) for x_ in nn] + [0])
                cache[(lo, hi, g)] = (len(m['v']) - 1, t)
            return cache[(lo, hi, g)]

        def lage(ia, ic, g):
            """Grenze g auf der Ringkante ia -> ic: None (kreuzt nicht), 0.0 / 1.0 (naeher als TEIL_MIN an ia / ic: laeuft durch die Ecke)
            oder t in (0, 1)."""
            ta = th(ia); d = _wickel(th(ic) - ta); x = _wickel(g - ta)
            if d == 0 or not (0.0 < x / d < 1.0):
                return None
            if abs(x) < TEIL_MIN:
                return 0.0
            if abs(d - x) < TEIL_MIN:
                return 1.0
            return x / d

        def punkt(ia, uva, ic, uvc, g, lg):
            """(Ecke, UV) der Grenze g auf der Kante ia -> ic bei Lage lg (0.0 / 1.0 = Ring-Ecke selbst)."""
            if lg == 0.0:
                return (ia, uva)
            if lg == 1.0:
                return (ic, uvc)
            lo, hi = (ia, ic) if ia < ic else (ic, ia)
            idx, t = ecke_auf(lo, hi, g)
            if ia > ic:
                t = 1.0 - t
            return (idx, (int(round(uva[0] + t * (uvc[0] - uva[0]))), int(round(uva[1] + t * (uvc[1] - uva[1])))))

        def grenzen(ia, ic):
            """Signierte Grenzen in der Reihenfolge, in der die Kante ia -> ic sie trifft."""
            d = _wickel(th(ic) - th(ia))
            gs = [g for g0 in TEIL_GRENZEN for g in (g0, -g0)]
            return sorted(gs, key=lambda g: _wickel(g - th(ia)) * (1 if d >= 0 else -1))

        for qi in range(n0[2]):
            if ('v%d' % qi) in ausn:
                continue
            f, uv = m['qf'][qi], m['quv'][qi]
            ie = list(f[1::2])
            assert list(f[0::2]) == ie, ('Normalen-Index != Ecken-Index', b, qi)
            if not all(ring_ecke(b, i) for i in ie):
                continue
            c = [(uv[3 * k], uv[3 * k + 1]) for k in range(4)]
            oben, unten = [(ie[0], c[0])], [(ie[2], c[2])]
            for g in grenzen(ie[0], ie[1]):
                lo_, lu_ = lage(ie[0], ie[1], g), lage(ie[2], ie[3], g)
                if lo_ is None and lu_ is None:
                    continue
                if lo_ is None or lu_ is None:    # nur eine Ringkante kreuzt: am anderen Ring durch die naechstliegende Ring-Ecke
                    ka, kc = (ie[0], ie[1]) if lo_ is None else (ie[2], ie[3])
                    da, dc = abs(_wickel(th(ka) - g)), abs(_wickel(th(kc) - g))
                    assert min(da, dc) <= TEIL_FANG, ('Grenze kreuzt nur eine Ringkante und liegt weit von der anderen', b, qi, g, da, dc)
                    ers = 0.0 if da <= dc else 1.0
                    lo_, lu_ = (ers, lu_) if lo_ is None else (lo_, ers)
                if lo_ == lu_ and lo_ in (0.0, 1.0):
                    continue                      # Grenze laeuft durch zwei Ring-Ecken derselben Seite: Flaechenkante, nichts zu teilen
                oben.append(punkt(ie[0], c[0], ie[1], c[1], g, lo_))
                unten.append(punkt(ie[2], c[2], ie[3], c[3], g, lu_))
            oben.append((ie[1], c[1])); unten.append((ie[3], c[3]))
            if len(oben) == 2:
                continue
            erstes = True
            for j in range(len(oben) - 1):
                o0, o1, u0, u1 = oben[j], oben[j + 1], unten[j], unten[j + 1]
                if o0[0] == o1[0] and u0[0] == u1[0]:
                    continue
                if o0[0] == o1[0] or u0[0] == u1[0]:      # Dreieck (Umlaufsinn wie die Viereck-Haelften 0-1-2 bzw. 1-3-2)
                    e = [o0, o1, u0] if u0[0] == u1[0] else [o1, u1, u0]
                    fn = []
                    for i_, _uv in e:
                        fn += [i_, i_]
                    un = [0] * 9
                    un[2], un[5], un[8] = uv[2], uv[5], uv[8]
                    for k, (_i, (u_, v_)) in enumerate(e):
                        un[3 * k], un[3 * k + 1] = u_, v_
                    m['tf'].append(fn); m['tuv'].append(un)
                    continue
                e = [o0, o1, u0, u1]
                fn = []
                for i_, _uv in e:
                    fn += [i_, i_]
                un = list(uv)
                for k, (_i, (u_, v_)) in enumerate(e):
                    un[3 * k], un[3 * k + 1] = u_, v_
                if erstes:
                    m['qf'][qi], m['quv'][qi] = fn, un
                    erstes = False
                else:
                    m['qf'].append(fn); m['quv'].append(un)
            assert not erstes, ('kein Viereck-Teilstueck', b, qi)
        for di in range(n0[1]):
            key = 'd%d' % di
            if key in ausn:
                continue
            f, uv = m['tf'][di], m['tuv'][di]
            ie = list(f[1::2])
            ring = [k for k in range(3) if ring_ecke(b, ie[k])]
            if len(ring) != 2 or list(f[0::2]) != ie:
                continue
            if modus == 'schwerpunkt' and ist_kappe(m, b, key, f, y0, y1):
                continue
            ka, kc = ring
            c = [(uv[3 * k], uv[3 * k + 1]) for k in range(3)]
            kt = [(ie[ka], c[ka])]
            for g in grenzen(ie[ka], ie[kc]):
                lg = lage(ie[ka], ie[kc], g)
                if lg is None or lg in (0.0, 1.0):
                    continue
                kt.append(punkt(ie[ka], c[ka], ie[kc], c[kc], g, lg))
            kt.append((ie[kc], c[kc]))
            for j in range(len(kt) - 1):
                fn, un = list(f), list(uv)
                for k, (i_, (u_, v_)) in ((ka, kt[j]), (kc, kt[j + 1])):
                    fn[2 * k] = fn[2 * k + 1] = i_
                    un[3 * k], un[3 * k + 1] = u_, v_
                if j == 0:
                    m['tf'][di], m['tuv'][di] = fn, un
                else:
                    m['tf'].append(fn); m['tuv'].append(un)
        zahl[str(b)] = dict(ecken=len(m['v']) - n0[0], dreiecke=len(m['tf']) - n0[1], vierecke=len(m['qf']) - n0[2])
    return zahl


def zeile_index(t):
    """Index der Zeile im Spiel-Asset, die bytegleich (15 Bit) der Original-Zeile 482 ist (nach Stufe a: 495)."""
    _MO, TO = A4.original()
    z = TO['clut'][ORIG_ZEILE - 480] & 0x7FFF
    for i in range(13, t['clut'].shape[0]):
        if np.array_equal(t['clut'][i] & 0x7FFF, z):
            return 480 + i
    raise AssertionError('keine Zeile = Original-Zeile 482 (Stufe a fehlt?)')


def anwenden(ms, t, par, prot):
    """Bau-Stufe u: (Runde 6) Flaechen an den Grenzen 110 / 120 teilen, dann die Flaechen der Unterseite (Mantel, Dorn und Ring-Deckel mit
    Mittel |theta| < KLASSE_AB) -> Zeile = Original-Zeile 482, Seite 2, UV = Verlauf Haut/Fleisch des Original-Unterarms (Fleisch laengs unter
    110, Uebergang 110-120 mit v aus theta, u entlang der Gliedachse, je Glied gespiegelt). Oberseite: Zeile 487, UV linear weitergefuehrt."""
    zeile = zeile_index(t)
    nutz = A4.nutzung(None, ORIG_ZEILE)
    teilung = teilen(ms, par)
    mitte = None
    if par.get('fleisch_r7b'):                # Runde 7b (K1): Mittelring je Glied fuer die Fleisch-Abbildung in Original-Dichte (unterseite_r7b.py)
        import unterseite_r7b as U7
        abb = U7.konfig(par['fleisch_r7b'])
        mitte = U7.mitte_teilen(ms, wurzel_keys)
    kl = flaechen_klassen(ms, par)
    zahl, deckel, fehl = {}, {}, []
    for b in V.SCHWANZ:
        m = ms[b]
        P = np.array([v[:3] for v in m['v']], float)
        y0, y1 = _ring_y(m, b)
        spiegel = (b - 16) % 2 == 1
        faces = {('d%d' % i): (f, m['tuv'][i]) for i, f in enumerate(m['tf'])}
        faces.update({('v%d' % i): (f, m['quv'][i]) for i, f in enumerate(m['qf'])})
        n = nd = 0
        for key, klasse, th in kl[b]:
            if klasse not in ('unten', 'kappe_unten'):
                continue
            f, uv = faces[key]
            ie = list(f[1::2])
            tr = [x for x in th if x is not None]
            th_m = float(np.mean(tr))
            phi = float(par['innen_phi'][str(b)])
            laengs = LAENGS and th_m < FLEISCH_KL
            # Runde 7b (K2), verworfen (Commit d73c04008 -> zurueckgenommen): das Kegel-Dreieck d14 (Ecken 87 / 113 Grad) mit der Uebergangs-
            # Abbildung statt laengs nahm zwar die letzte 495/487-Kante mit da* >= 15 (d14/d15, da* 28,3, im Rumpf) weg, legte aber ein
            # Fleisch-Dreieck (Mittel < 110) auf die Texelzeilen 83-91 -> Riegel S15 (g) "graue Texelzeilen im Fleisch-Feld" ROT
            # (Zeilen 85-89 mit 0-33 % Fleisch, gemessen am Asset f3e1ceff). d14 bleibt laengs.
            ts_l = [theta_signiert(P[i], phi) if ring_ecke(b, i) else None for i in ie]
            ts_r = [x for x in ts_l if x is not None]
            if laengs and par.get('fleisch_r7b'):  # Runde 7b (K1): Fleisch in Original-Dichte (unterseite_r7b.py); ohne Schluessel wie Runde 6
                import unterseite_r7b as U7
                for k, (u_, v_) in enumerate(U7.uv_flaeche(m, b, ie)):
                    uv[3 * k], uv[3 * k + 1] = u_, v_
                if klasse == 'kappe_unten':
                    nd += 1
                AV.setze_zeile(uv, zeile)
                uv[5] = (uv[5] & ~0xF) | SEITE
                mk = KB.raster(uv, 3 if len(uv) == 9 else 4, SEITE * 128)
                if (mk & ~nutz).any():
                    fehl.append('%d:%s' % (b, key))
                n += 1
                continue
            for k, i in enumerate(ie):
                tt = th[k] if th[k] is not None else th_m
                s = np.clip((P[i][1] - y0) / max(y1 - y0, 1.0), 0.0, 1.0)
                if spiegel:
                    s = 1.0 - s
                if laengs:                # Fleischflaeche: Streifen des Original-Fleischs laufen entlang der Gliedachse (v), u um den Umfang
                    ts = ts_l[k] if ts_l[k] is not None else float(np.mean(ts_r))
                    uv[3 * k] = int(round(UF0 + (ts + TF) / (2 * TF) * (UF1 - UF0)))
                    uv[3 * k + 1] = int(round(VF0 + s * (VF1 - VF0)))
                else:
                    uv[3 * k] = int(round(U0 + s * (U1 - U0)))
                    uv[3 * k + 1] = int(round(v_von(tt)))
            if klasse == 'kappe_unten':   # Ring-Deckel: an der Ringkante dieselben Texel wie der Mantel, die Mittel-Ecke daneben im selben Material
                kz = [k for k, i in enumerate(ie) if not ring_ecke(b, i)][0]
                kr = [k for k in range(3) if k != kz]
                if laengs:
                    uv[3 * kz] = int(round(np.mean([uv[3 * k] for k in kr])))
                    uv[3 * kz + 1] = int(round(V_KAPPE))
                else:
                    ur = uv[3 * kr[0]]
                    uv[3 * kz] = int(round(ur + (U_KAPPE if ur < 0.5 * (U0 + U1) else -U_KAPPE)))
                    uv[3 * kz + 1] = int(round(np.mean([uv[3 * k + 1] for k in kr])))
                nd += 1
            AV.setze_zeile(uv, zeile)
            uv[5] = (uv[5] & ~0xF) | SEITE
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, SEITE * 128)
            if (mk & ~nutz).any():
                fehl.append('%d:%s' % (b, key))
            n += 1
        zahl[str(b)] = n
        deckel[str(b)] = nd
    assert not fehl, ('Texel ausserhalb der Original-Nutzung', fehl)
    prot['unterseite'] = dict(zeile=zeile, flaechen=zahl, summe=int(sum(zahl.values())), deckel_unten=deckel, deckel=par.get('deckel', DECKEL),
                              teilung=teilung, grau_ab=GRAU_AB, fleisch_ab=FLEISCH_AB, v=(V_GRAU, V_FLEISCH_AB, V_TIEF), u=(U0, U1),
                              innen_phi=par['innen_phi'])
    if mitte is not None:
        prot['unterseite']['fleisch_r7b'] = dict(mittelring=mitte, abbildung=abb)


# ------------------------------------------------------------------ Messung / Riegel-Helfer
def theta_bild(mo, pose, A):
    """|theta| je Schwanz-Bildpunkt aus der interpolierten Normale (f_rast 'nl') -> (ys, xs, bone, theta)."""
    R, _T, _P, ik = innen_modell(mo, pose)
    bone = A['bone'][:480]
    ys, xs = np.nonzero((bone >= 16) & (bone <= 22))
    th = np.zeros(len(ys))
    for i, (y, x) in enumerate(zip(ys, xs)):
        b = int(bone[y, x]); d, k = ik[b - 16]
        n = (R[b].astype(float) / 4096.0) @ A['nl'][y, x].astype(float)
        n = n - (n @ d) * d
        th[i] = np.degrees(np.arccos(np.clip(n @ k / (np.linalg.norm(n) + 1e-9), -1, 1)))
    return ys, xs, bone[ys, xs], th


def vorlage_lab(bilder):
    from PIL import Image
    import v4_gemein as G
    roh = os.path.join(V.BAUM.replace('r41_sichel_v8', 'r41_sichel'), 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')
    if not os.path.isdir(roh):
        roh = os.path.join(V.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')
    ims = [np.asarray(Image.open(os.path.join(roh, 'f%04d.png' % (n + 1))).convert('RGB')).astype(float) for n in bilder]
    return np.mean([G.lab(im) for im in ims], 0)


MESS_ANSICHTEN = (('PV', 'PV', (1438, 1439, 1440)), ('P2', 'P2', (1434, 1435, 1436)), ('P1', 'P1', (1485,)))


def pose_von(name):
    import r3_gemein as R3
    return {'PV': V.PV, 'P1': R3.P1, 'P2': R3.P2}[name]


def messen(blob):
    import f_rast as FR
    from scipy import ndimage
    mo = V.modell(blob)
    out = {}
    for nm, pn, bilder in MESS_ANSICHTEN:
        pose = pose_von(pn)
        A = FR.zeichne(mo, pose, None, licht=False)
        ys, xs, bb, th = theta_bild(mo, pose, A)
        tail = np.zeros((480, 640), bool); tail[ys, xs] = True
        er = ndimage.binary_erosion(tail, iterations=2)
        sel = er[ys, xs]
        Lb = vorlage_lab(bilder)
        a, L = Lb[ys, xs, 1], Lb[ys, xs, 0]
        zeilen = []
        for lo in range(0, 180, 10):
            m = sel & (th >= lo) & (th < lo + 10)
            if m.sum() >= 8:
                zeilen.append(dict(theta=[lo, lo + 10], n=int(m.sum()), a_med=round(float(np.median(a[m])), 1),
                                   rot=round(float((a[m] >= 10).mean() * 100), 1), L_med=round(float(np.median(L[m])), 1)))
        glied = []
        for b in V.SCHWANZ:
            for nmk, (lo, hi) in (('innen_0_60', (0, 60)), ('seite_60_115', (60, 115)), ('oben_125_180', (125, 181))):
                m = sel & (bb == b) & (th >= lo) & (th < hi)
                if m.sum() >= 8:
                    glied.append(dict(knochen=b, bereich=nmk, n=int(m.sum()), a_med=round(float(np.median(a[m])), 1),
                                      rot=round(float((a[m] >= 10).mean() * 100), 1), L_med=round(float(np.median(L[m])), 1)))
        out[nm] = dict(bilder=list(bilder), schwanz_px=int(tail.sum()), erodiert=int(sel.sum()), theta=zeilen, glied=glied)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('was', choices=('messen', 'json'))
    ap.add_argument('emd', nargs='?', default=V.ASSET)
    ap.add_argument('--aus')
    a = ap.parse_args()
    blob = open(a.emd, 'rb').read()
    if a.was == 'messen':
        r = messen(blob)
        for nm, d in r.items():
            print('== %s Vorlage n%s, Schwanz %d px (erodiert %d)' % (nm, '/n'.join(str(x) for x in d['bilder']), d['schwanz_px'], d['erodiert']))
            for z in d['theta']:
                print('  theta %3d-%3d n %4d a* Median %5.1f  a* >= 10 %5.1f %%  L* %5.1f' % (z['theta'][0], z['theta'][1], z['n'], z['a_med'], z['rot'], z['L_med']))
            for z in d['glied']:
                print('  Knochen %d %-13s n %4d a* Median %5.1f  a* >= 10 %5.1f %%  L* %5.1f' % (z['knochen'], z['bereich'], z['n'], z['a_med'], z['rot'], z['L_med']))
        if a.aus:
            json.dump(r, open(a.aus, 'w'), indent=1)
    else:
        mo = V.modell(blob)
        par = dict(quelle='unterseite_r5.py json %s (sha256 %s), Pose PV' % (os.path.basename(a.emd), V.sha(blob)),
                   innen_phi=innen_glied(mo))
        p = a.aus or os.path.join(HIER, 'unterseite_v8.json')
        json.dump(par, open(p, 'w'), indent=1)
        print('geschrieben', p, par['innen_phi'])


if __name__ == '__main__':
    main()


# ------------------------------------------------------------------ Riegel S15 (riegel_v8.py)
FLEISCH_A = 15.0                      # Fleisch-Texel: Paletteneintrag seiner Zeile mit a* >= 15 (Mass der Adern/Flecken-Riegel, adern_v8.ADER_A)
S15_UNTEN_SOLL = 30.0                 # (c) Anteil Fleisch-Texel an den Schwanz-Bildpunkten |theta| <= 100 in PV und P2 >= 30 % (Vorlage theta 70-110:
                                      # a* >= 10 25,7-48,8 % in PV, 15,6-31,5 % in P2 - Groessenordnung; V8 r4 0 %)
S15_UNTEN_TH, S15_OBEN_TH = 100.0, 135.0


def fleisch_eintraege(clut_zeile):
    import v4_gemein as G
    z = np.asarray(clut_zeile, np.int64)
    rgb = np.stack([(z & 31) << 3, ((z >> 5) & 31) << 3, ((z >> 10) & 31) << 3], -1).astype(float)
    L = G.lab(rgb.reshape(-1, 1, 3)).reshape(-1, 3)
    return (L[:, 1] >= FLEISCH_A) & (z != 0)


_REF = {}


def ohne_u(v7_blob):
    """Vergleichsstand fuer (b): derselbe Bau ohne Stufe u (v8_bau.bauen(V7, 'srwa')) - die Schwanz-UV der Oberseite muss ihm gleichen."""
    k = V.sha(v7_blob)
    if k not in _REF:
        import v8_bau as VB
        _REF[k] = VB.bauen(None, 'srwa', blob=v7_blob)[0]
    return _REF[k]


def s15_flaechen(blob, ref_blob, par=None):
    """(a) Herkunft: jede Schwanzflaeche (Mesh 16-22) ausserhalb Zeile 487 liegt auf der Zeile = Original-Zeile 482, Seite 2, alle Texel unter
    ihr = Original-Texel UND von Original-Flaechen der Zeile 482 benutzt (0 fremde Texel); (b) Klassen nach der gemessenen Grenze: jede
    Unterseiten-Flaeche (Mittel |theta| < KLASSE_AB) auf dieser Zeile und mit Fleisch-Texeln, wenn ihr Mittel < FLEISCH_KL; jede Oberseiten-
    Flaeche auf Zeile 487 ohne Fleisch-Texel und mit der UV des Baus ohne Stufe u (ungeteilte Flaeche: gleiche UV; in Runde 6 geteilte
    Flaeche: alle ihre Texel in den Texeln, die die Oberseite desselben Glieds im Bau ohne u benutzt, 1 Texel Rundung). Runde 6: Ring-Deckel
    unter der Grenze ('kappe_unten') muessen auf der Fleisch-Zeile liegen (deckel_grau_unten = 0), Deckel darueber auf Zeile 487.
    ref_blob = V7."""
    from scipy import ndimage
    par = par or json.load(open(os.path.join(HIER, 'unterseite_v8.json')))
    _d, _o, ms, tim = KB.teile(blob); _d2, _o2, mr, _t2 = KB.teile(ohne_u(ref_blob))
    t = KB.tim_teile(tim)
    MO, TO = A4.original()
    z_orig = ORIG_ZEILE - 480
    zeile = None
    for i in range(13, t['clut'].shape[0]):
        if np.array_equal(t['clut'][i] & 0x7FFF, TO['clut'][z_orig] & 0x7FFF):
            zeile = 480 + i
    nutz = A4.nutzung(None, ORIG_ZEILE)
    kl = flaechen_klassen(ms, par)
    fremd = falsch_zeile = 0
    n_unten = n_oben = unten_ohne_fleisch = oben_fehler = n_fl_tex = n_deckel_unten = deckel_grau_unten = 0
    for b in V.SCHWANZ:
        alt = np.zeros(t['pix'].shape, bool)
        ref = {}
        for k2, f2, u2 in [('d%d' % i, f, mr[b]['tuv'][i]) for i, f in enumerate(mr[b]['tf'])] + \
                          [('v%d' % i, f, mr[b]['quv'][i]) for i, f in enumerate(mr[b]['qf'])]:
            ref[k2] = (list(f2[1::2]), u2)
            alt |= KB.raster(u2, 3 if len(u2) == 9 else 4, (u2[5] & 0xF) * 128)
        alt = ndimage.binary_dilation(alt, iterations=1)
        ecken = {('d%d' % i): list(f[1::2]) for i, f in enumerate(ms[b]['tf'])}
        ecken.update({('v%d' % i): list(f[1::2]) for i, f in enumerate(ms[b]['qf'])})
        for key, klasse, th in kl[b]:
            uv = AV.flaeche(ms[b], key)
            z = (uv[2] >> 6) & 0x1FF
            tr = [x for x in th if x is not None]
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            if z != SCHWANZ_ZEILE:
                if z != zeile or (uv[5] & 0xF) != SEITE:
                    falsch_zeile += 1
                fremd += int((mk & ~nutz).sum()) + int((t['pix'][mk] != TO['pix'][mk]).sum())
            ist_fleisch = zeile is not None and z == zeile
            nf = int(fleisch_eintraege(t['clut'][z - 480])[t['pix'][mk]].sum()) if ist_fleisch else 0
            if klasse == 'unten':
                n_unten += 1
                if not ist_fleisch:
                    unten_ohne_fleisch += 1
                elif float(np.mean(tr)) < FLEISCH_KL and nf == 0:
                    unten_ohne_fleisch += 1
                n_fl_tex += nf
            elif klasse == 'kappe_unten':
                n_deckel_unten += 1
                if not ist_fleisch:
                    deckel_grau_unten += 1
                n_fl_tex += nf
            elif klasse == 'kappe':
                if z != SCHWANZ_ZEILE:
                    oben_fehler += 1
            else:
                n_oben += 1
                if key in ref and ref[key][0] == ecken[key]:
                    ur = ref[key][1]
                    gleich = [uv[i] for i in range(len(uv)) if i % 3 != 2] == [ur[i] for i in range(len(ur)) if i % 3 != 2]
                else:
                    gleich = not (mk & ~alt).any()
                if z != SCHWANZ_ZEILE or not gleich or nf:
                    oben_fehler += 1
    return dict(zeile=zeile, unten=n_unten, oben=n_oben, unten_ohne_fleisch=unten_ohne_fleisch, oben_fehler=oben_fehler,
                deckel_unten=n_deckel_unten, deckel_grau_unten=deckel_grau_unten,
                fleisch_texel=n_fl_tex, fremde_texel=fremd, falsche_zeile=falsch_zeile,
                ok=bool(zeile is not None and fremd == 0 and falsch_zeile == 0 and unten_ohne_fleisch == 0 and oben_fehler == 0 and n_unten > 0
                        and deckel_grau_unten == 0))


def s15_bild(blob, posen=('PV', 'P2')):
    """(c) In PV und P2 (Spielkamera, f_rast ohne Licht fuer die Texel-Zuordnung): Anteil Bildpunkte mit Fleisch-Texel an den Schwanz-Bildpunkten
    |theta| <= S15_UNTEN_TH (Soll >= S15_UNTEN_SOLL) und an denen mit |theta| >= S15_OBEN_TH (Soll 0)."""
    import f_rast as FR
    mo = V.modell(blob)
    _d, _o, _ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    fl = {480 + i: fleisch_eintraege(t['clut'][i]) for i in range(t['clut'].shape[0])}
    out = {}
    for pn in posen:
        pose = pose_von(pn)
        A = FR.zeichne(mo, pose, None, licht=False)
        ys, xs, _bb, th = theta_bild(mo, pose, A)
        z = A['zeile'][ys, xs]; ix = A['idx'][ys, xs]
        ist = np.array([bool(fl[int(zz)][int(ii)]) if int(zz) in fl and ii >= 0 else False for zz, ii in zip(z, ix)])
        u = th <= S15_UNTEN_TH; o = th >= S15_OBEN_TH
        out[pn] = dict(unten_px=int(u.sum()), unten_fleisch=round(float(ist[u].mean() * 100) if u.any() else 0.0, 1),
                       oben_px=int(o.sum()), oben_fleisch=round(float(ist[o].mean() * 100) if o.any() else 0.0, 1))
    ok = all(v['unten_fleisch'] >= S15_UNTEN_SOLL and v['oben_fleisch'] == 0.0 for v in out.values())
    return dict(ansichten=out, ok=bool(ok))


S15_RUHE_TH = (90, 180, 15)           # (d) Uebergangsbereich theta 90-180 in 15-Grad-Baendern (Grenze 125, Uebergang 100-130)


def s15_ruhe(blob, posen=('PV', 'P2')):
    """(d) Ruhe des Uebergangs grau/fleischig (Auftrag: "keine harte gerade Kante, kein Streifen"; "so ruhig wie im Original-Birkin zwischen
    Haut und Fleisch"): L* (Videoraum, RDT-Licht) je theta-Band ueber den Uebergangsbereich S15_RUHE_TH; der groesste Sprung zwischen
    Nachbarbaendern ist hoechstens so gross wie der der Vorlage in denselben Baendern (Vorlage = Ort und Verlauf, keine Eichung: es wird nur
    die Sprunghoehe verglichen). Dazu (nur Info) der oertliche Sprung an der Zeilenkante 487/Fleischzeile (7 x 7-Mediane) gegen den Sprung an
    der Haut/Fleisch-Kante des Original-Unterarms (Meshes 11/12/14/15) in derselben Pose."""
    import f_rast as FR
    import v4_gemein as G
    from scipy import ndimage
    mo = V.modell(blob)
    out = {}
    ok = True
    for pn, _p, bilder in MESS_ANSICHTEN:
        if pn not in posen:
            continue
        pose = pose_von(pn)
        A = FR.mit_bg(mo, pose)
        ys, xs, _bb, th = theta_bild(mo, pose, A)
        tail = np.zeros((480, 640), bool); tail[ys, xs] = True
        sel = ndimage.binary_erosion(tail, iterations=2)[ys, xs]
        Lm = G.lab(G.videoraum(A['img'][:480]))[ys, xs, 0]
        Lv = vorlage_lab(bilder)[ys, xs, 0]
        a0, a1, st = S15_RUHE_TH
        pm, pv = [], []
        for lo in range(a0, a1, st):
            m = sel & (th >= lo) & (th < lo + st)
            pm.append(float(np.median(Lm[m])) if m.sum() >= 8 else None)
            pv.append(float(np.median(Lv[m])) if m.sum() >= 8 else None)
        sm = [abs(b - a) for a, b in zip(pm, pm[1:]) if a is not None and b is not None]
        sv = [abs(b - a) for a, b in zip(pv, pv[1:]) if a is not None and b is not None]
        r = dict(modell=[None if x is None else round(x, 1) for x in pm], vorlage=[None if x is None else round(x, 1) for x in pv],
                 sprung_modell=round(max(sm), 1) if sm else None, sprung_vorlage=round(max(sv), 1) if sv else None)
        r['ok'] = bool(sm and sv and max(sm) <= max(sv))
        ok = ok and r['ok']
        out[pn] = r
    return dict(ansichten=out, ok=bool(ok))


# ------------------------------------------------------------------ Runde 6: Grenze, Ring-Deckel, Kanten (Riegel S15 e/f, Mass des Zahlen-Pruefers)
def _zeile_oder_none(t):
    try:
        return zeile_index(t)
    except AssertionError:
        return None


def s15_grenze(blob, posen=('PV', 'P2')):
    """(e) Auftrag Runde 6 (b): Anteil Zeile 495 an den Schwanz-Bildpunkten im Band theta 120-130 (Soll <= S15_BAND_SOLL %), dazu (Info) in
    den Baendern 100-110 und 110-120. Spielkamera, theta je Bildpunkt aus der interpolierten Normale (theta_bild)."""
    import f_rast as FR
    mo = V.modell(blob)
    _d, _o, _ms, tim = KB.teile(blob)
    zeile = _zeile_oder_none(KB.tim_teile(tim))
    out = {}
    for pn in posen:
        pose = pose_von(pn)
        A = FR.zeichne(mo, pose, None, licht=False)
        ys, xs, _bb, th = theta_bild(mo, pose, A)
        z = A['zeile'][ys, xs]
        r = {}
        for nm, (lo, hi) in (('band', S15_BAND), ('uebergang', (FLEISCH_AB, GRAU_AB)), ('fleisch', (FLEISCH_AB - 10.0, FLEISCH_AB))):
            m = (th >= lo) & (th < hi)
            r[nm + '_px'] = int(m.sum())
            r[nm] = round(float((z[m] == zeile).mean() * 100), 1) if (zeile is not None and m.any()) else 0.0
        out[pn] = r
    return dict(ansichten=out, ok=bool(all(v['band'] <= S15_BAND_SOLL for v in out.values())))


def deckel_theta(m, b, f, phi, lok):
    """|theta| eines Bildpunkts (Ort lok im Knochenraum) auf dem Deckel-Dreieck f: Strahl von der Achs-Ecke durch den Punkt auf die Ringkante,
    dort die Normale zwischen den zwei Ring-Ecken interpoliert. (Die interpolierte Bild-Normale taugt auf dem Deckel nicht: an der Achs-Ecke
    zeigt sie laengs der Achse, ihr Rest quer dazu ist Rauschen - gemessen 6 Bildpunkte mit "theta 14-17" auf Deckeln der Oberseite.)"""
    ie = list(f[1::2])
    kz = [k for k in range(3) if not ring_ecke(b, ie[k])][0]
    ka, kc = [k for k in range(3) if k != kz]
    Z = np.array(m['v'][ie[kz]][:3], float)
    M = np.stack([np.array(m['v'][ie[ka]][:3], float) - Z, np.array(m['v'][ie[kc]][:3], float) - Z], 1)
    w = np.linalg.lstsq(M, np.asarray(lok, float) - Z, rcond=None)[0]
    t = float(np.clip(w[1] / (w[0] + w[1]), 0.0, 1.0)) if (w[0] + w[1]) > 1e-6 else 0.5
    n = (1 - t) * np.array(m['n'][f[2 * ka]][:3], float) + t * np.array(m['n'][f[2 * kc]][:3], float)
    return abs(theta_normale(n, phi))


def s15_deckel(blob, par=None, posen=None):
    """(f) Auftrag Runde 6 (a) "in keinem Szenenbild grauer Ring/Schlitz auf der Unterseite": Bildpunkte von Ring-Deckeln (Klasse kappe /
    kappe_unten) auf Zeile 487 mit theta < KLASSE_AB - S15_DECKEL_RAND (theta je Bildpunkt: deckel_theta) in den Posen der Szene
    (r3_gemein.szene_posen, F235-F378) -> Zahl der Posen mit mindestens einem solchen Bildpunkt (Soll 0) und Summe der Bildpunkte."""
    import f_rast as FR
    import r3_gemein as R3
    par = par or json.load(open(os.path.join(HIER, 'unterseite_v8.json')))
    mo = V.modell(blob)
    _d, _o, ms, _tim = KB.teile(blob)
    kl = flaechen_klassen(ms, par)
    kap = {b: set(k for k, c, _t in kl[b] if c in ('kappe', 'kappe_unten')) for b in V.SCHWANZ}
    P = R3.szene_posen()
    namen = sorted(P) if posen is None else list(posen)
    bilder, summe, groesst = 0, 0, (0, None)
    for k in namen:
        A = FR.zeichne(mo, P[k], None, licht=False)
        bone = A['bone'][:480]
        ys, xs = np.nonzero((bone >= 16) & (bone <= 22) & (A['zeile'][:480] == SCHWANZ_ZEILE))
        n = 0
        for y, x in zip(ys, xs):
            b = int(bone[y, x]); key = str(A['flaeche'][y, x])
            if key in kap[b]:
                f = ms[b]['tf'][int(key[1:])]
                if deckel_theta(ms[b], b, f, float(par['innen_phi'][str(b)]), A['lok'][y, x]) < KLASSE_AB - S15_DECKEL_RAND:
                    n += 1
        if n:
            bilder += 1; summe += n
            if n > groesst[0]:
                groesst = (n, 'F%s' % k)
    return dict(posen=len(namen), bilder=bilder, px=summe, groesst=groesst, ok=bool(bilder == 0))


def kanten_mass(blob):
    """Mass des Zahlen-Pruefers (Runde 5, p4kante.py) nachgebaut: jede Kante, an der im selben Mesh eine Schwanz-Flaeche der Fleisch-Zeile an
    eine der Zeile 487 stoesst (gleiche Ecken-Orte): 9 Proben je Kante, 1 Texel nach innen, unbeleuchtet; dL / da* (Fleisch-Zeile minus 487),
    mit der Kantenlaenge gewichtet; getrennt nach grauer Nachbarflaeche Viereck (Mantel) / Dreieck (Ring-Deckel, Dorn)."""
    _d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    zeile = _zeile_oder_none(t)
    H, W = t['pix'].shape

    def probe(z, a, b, mitte):
        ein = mitte - (a + b) / 2.0
        ein = ein / (np.linalg.norm(ein) + 1e-9)
        c = []
        for s_ in np.linspace(0.1, 0.9, 9):
            q = a + (b - a) * s_ + ein
            u = int(np.clip(round(q[0]), 0, W - 1)); v = int(np.clip(round(q[1]), 0, H - 1))
            c.append(int(t['clut'][z - 480][t['pix'][v, u]]))
        return AV.lab15(np.array(c))
    res = []
    for b in V.SCHWANZ:
        m = ms[b]
        kanten = {}
        for kind, F, U in (('d', m['tf'], m['tuv']), ('v', m['qf'], m['quv'])):
            for k, (fc, uv) in enumerate(zip(F, U)):
                po = (uv[5] & 0xF) * 128; z = (uv[2] >> 6) & 0x1FF
                nk = len(fc) // 2
                tx = [(uv[3 * i] + po, uv[3 * i + 1]) for i in range(nk)]
                Pk = [tuple(m['v'][i][:3]) for i in fc[1::2]]
                if kind == 'v':
                    Pk = [Pk[0], Pk[1], Pk[3], Pk[2]]; tx = [tx[0], tx[1], tx[3], tx[2]]
                tx = np.array(tx, float)
                for i in range(nk):
                    a, c = Pk[i], Pk[(i + 1) % nk]
                    if a == c:
                        continue
                    key = (a, c) if a < c else (c, a)
                    ta, tc = (tx[i], tx[(i + 1) % nk]) if a < c else (tx[(i + 1) % nk], tx[i])
                    art = 'd' if (kind == 'd' and not all(ring_ecke(b, i_) for i_ in fc[1::2])) else 'v'
                    kanten.setdefault(key, []).append((art + str(k), z, ta, tc, tx.mean(0)))
        for key, lst in sorted(kanten.items()):
            f5 = [x for x in lst if x[1] == zeile]; f7 = [x for x in lst if x[1] == SCHWANZ_ZEILE]
            if f5 and f7:
                A = probe(zeile, f5[0][2], f5[0][3], f5[0][4]); B = probe(SCHWANZ_ZEILE, f7[0][2], f7[0][3], f7[0][4])
                lang = float(np.linalg.norm(np.array(key[0], float) - np.array(key[1], float)))
                res.append((b, f5[0][0], f7[0][0], lang, float(A[:, 0].mean() - B[:, 0].mean()), float(A[:, 1].mean() - B[:, 1].mean())))
    out = {}
    for art, sel in (('alle', res), ('mantel', [r for r in res if r[2][0] == 'v']), ('deckel', [r for r in res if r[2][0] == 'd'])):
        l = sum(r[3] for r in sel)
        out[art] = dict(kanten=len(sel), laenge=round(l), da=round(sum(r[3] * r[5] for r in sel) / l, 2) if l else 0.0,
                        dL=round(sum(r[3] * r[4] for r in sel) / l, 2) if l else 0.0,
                        da_max=round(max([abs(r[5]) for r in sel]), 1) if sel else 0.0)
    return out


def s15_reifen(blob, par=None):
    """(g) Runde 6: kein grauer Reifen auf dem MANTEL der Unterseite. Die Fleischflaechen (Klasse unten / kappe_unten, Mittel |theta| <
    FLEISCH_KL) sind laengs belegt: v laeuft entlang der Gliedachse, eine Texelzeile (v fest) liegt als Ring um das Glied. Je Texelzeile unter
    diesen Flaechen der Anteil Fleisch-Texel (a* >= FLEISCH_A, unbeleuchtet aus der CLUT) -> Zeilen unter S15_REIFEN_MIN % (Soll 0)."""
    par = par or json.load(open(os.path.join(HIER, 'unterseite_v8.json')))
    _d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    zeile = _zeile_oder_none(t)
    if zeile is None:
        return dict(zeilen=0, grau=[], kleinst=0.0, ok=False)
    fl = fleisch_eintraege(t['clut'][zeile - 480])
    kl = flaechen_klassen(ms, par)
    mk = np.zeros(t['pix'].shape, bool)
    for b in V.SCHWANZ:
        for key, klasse, th in kl[b]:
            tr = [x for x in th if x is not None]
            if klasse not in ('unten', 'kappe_unten') or float(np.mean(tr)) >= FLEISCH_KL:
                continue
            uv = AV.flaeche(ms[b], key)
            if ((uv[2] >> 6) & 0x1FF) != zeile:
                continue
            mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    grau, kleinst, n = [], 100.0, 0
    for v in np.nonzero(mk.any(1))[0]:
        px = t['pix'][v][mk[v]]
        if len(px) < 4:
            continue
        n += 1
        a = float(fl[px].mean() * 100)
        kleinst = min(kleinst, a)
        if a < S15_REIFEN_MIN:
            grau.append((int(v), round(a, 1)))
    return dict(zeilen=n, grau=grau, kleinst=round(kleinst, 1), ok=bool(n > 0 and not grau))
