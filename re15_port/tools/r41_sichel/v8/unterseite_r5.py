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
  - PV (n1439) Knochen 16-19: theta 70-110 a* Median 7,0-10,5, Anteil a* >= 10 28-58 % (rot/fleischig); theta 110-120 a* 4,6, 1 %;
    theta >= 120 a* 1,4-4,2, 0 %, L* 23-38 (grau, hell) -> Grenze grau/fleischig bei theta ~115 (Uebergang 110-120).
  - P2 (n1435) gleich: theta 70-110 a* 7,1-8,7, 20-38 %; 110-120 a* 4,6, 4 %; >= 120 a* <= 3,3, 0 %.
  - theta < 60 (Innenseite selbst) dunkel (L* 8-12, a* 0-4): im Schatten, Farbe nicht lesbar; die Regel nimmt sie als fleischig (Nutzer:
    "Unterseite ... fleischig").
  - Laengs: rot sicher von der Wurzel bis Knochen 18/19 (Bild y <= 285); Knochen 20-22 dunkel (L* 10-20), a* innen 2-4 gegen aussen -1,6
    (schwacher Hinweis, Offen) -> die Regel gilt von der Wurzel bis zur Spitze (keine neue Querkante mitten im Schwanz).
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


def flaechen_klassen(ms, par):
    """-> {b: [(key, 'grau'|'unten'|'kappe', [theta je Ecke])]} fuer Knochen 16-22; Wurzelflaechen von Mesh 16 (Rohr/Kegel, im Rumpf; Marker S10s)
    bleiben ausgenommen."""
    out = {}
    for b in V.SCHWANZ:
        m = ms[b]
        P = np.array([v[:3] for v in m['v']], float)
        phi = float(par['innen_phi'][str(b)])
        nr = 3 if b in (16, 18) else 2
        ausn = wurzel_keys(m) if b == 16 else set()
        y0, y1 = float(P[:nr * S_, 1].min()), float(P[:nr * S_, 1].max())
        res = []
        for key, f in [('d%d' % i, f) for i, f in enumerate(m['tf'])] + [('v%d' % i, f) for i, f in enumerate(m['qf'])]:
            ie = list(f[1::2])
            th = [theta_ecke(P[i], phi) if i < nr * S_ else None for i in ie]
            tr = [t for t in th if t is not None]
            if key[0] == 'd' and any(i >= nr * S_ and y0 - 1.0 <= P[i][1] <= y1 + 1.0 for i in ie):
                # Deckel/Kappe (Mittelpunkt auf der Achse innerhalb des Glieds, v8_bau.glied_mesh): liegt auf der Gehrung im Nachbarglied,
                # bleibt wie gebaut (Zeile 487). Dorn (Spitze hinter Ring 2 von Mesh 22) und Wurzelkegel (vor Ring 0) zaehlen nicht dazu.
                res.append((key, 'kappe', th))
            elif key in ausn or not tr or float(np.mean(tr)) >= KLASSE_AB:
                res.append((key, 'grau', th))
            else:
                res.append((key, 'unten', th))
        out[b] = res
    return out


def zeile_index(t):
    """Index der Zeile im Spiel-Asset, die bytegleich (15 Bit) der Original-Zeile 482 ist (nach Stufe a: 495)."""
    _MO, TO = A4.original()
    z = TO['clut'][ORIG_ZEILE - 480] & 0x7FFF
    for i in range(13, t['clut'].shape[0]):
        if np.array_equal(t['clut'][i] & 0x7FFF, z):
            return 480 + i
    raise AssertionError('keine Zeile = Original-Zeile 482 (Stufe a fehlt?)')


def anwenden(ms, t, par, prot):
    """Bau-Stufe u: Flaechen der Unterseite (mindestens eine Ringecke |theta| < GRAU_AB) -> Zeile = Original-Zeile 482, Seite 2, UV = Verlauf
    Haut/Fleisch des Original-Unterarms (v aus theta, u entlang der Gliedachse, je Glied gespiegelt). Oberseite unveraendert."""
    zeile = zeile_index(t)
    nutz = A4.nutzung(None, ORIG_ZEILE)
    kl = flaechen_klassen(ms, par)
    zahl, fehl = {}, []
    for b in V.SCHWANZ:
        m = ms[b]
        P = np.array([v[:3] for v in m['v']], float)
        nr = 3 if b in (16, 18) else 2
        y0, y1 = float(P[:nr * S_, 1].min()), float(P[:nr * S_, 1].max())
        spiegel = (b - 16) % 2 == 1
        faces = {('d%d' % i): (f, m['tuv'][i]) for i, f in enumerate(m['tf'])}
        faces.update({('v%d' % i): (f, m['quv'][i]) for i, f in enumerate(m['qf'])})
        n = 0
        for key, klasse, th in kl[b]:
            if klasse != 'unten':
                continue
            f, uv = faces[key]
            ie = list(f[1::2])
            tr = [x for x in th if x is not None]
            th_m = float(np.mean(tr))
            phi = float(par['innen_phi'][str(b)])
            laengs = LAENGS and th_m < FLEISCH_KL
            ts_l = [theta_signiert(P[i], phi) for i in ie]
            ts_r = [x for x in ts_l if x is not None]
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
            AV.setze_zeile(uv, zeile)
            uv[5] = (uv[5] & ~0xF) | SEITE
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, SEITE * 128)
            if (mk & ~nutz).any():
                fehl.append('%d:%s' % (b, key))
            n += 1
        zahl[str(b)] = n
    assert not fehl, ('Texel ausserhalb der Original-Nutzung', fehl)
    prot['unterseite'] = dict(zeile=zeile, flaechen=zahl, summe=int(sum(zahl.values())), grau_ab=GRAU_AB, fleisch_ab=FLEISCH_AB,
                              v=(V_GRAU, V_FLEISCH_AB, V_TIEF), u=(U0, U1), innen_phi=par['innen_phi'])


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
S15_UNTEN_SOLL = 30.0                 # (c) Anteil Fleisch-Texel an den Schwanz-Bildpunkten |theta| <= 100 in PV und P2 >= 30 % (Vorlage dort: a* >= 10
                                      # 26-58 % in PV, 22-38 % in P2; V8 r4 0 %)
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
    Flaeche auf Zeile 487 mit der UV des Baus ohne Stufe u (= V8 Runde 4/5 vor P4; 0 Fleisch-Texel). ref_blob = V7."""
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
    n_unten = n_oben = unten_ohne_fleisch = oben_fehler = n_fl_tex = 0
    for b in V.SCHWANZ:
        for key, klasse, th in kl[b]:
            uv = AV.flaeche(ms[b], key); ur = AV.flaeche(mr[b], key)
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
            elif klasse == 'kappe':
                if z != SCHWANZ_ZEILE:
                    oben_fehler += 1
            else:
                n_oben += 1
                gleich = [uv[i] for i in range(len(uv)) if i % 3 != 2] == [ur[i] for i in range(len(ur)) if i % 3 != 2]
                if z != SCHWANZ_ZEILE or not gleich or nf:
                    oben_fehler += 1
    return dict(zeile=zeile, unten=n_unten, oben=n_oben, unten_ohne_fleisch=unten_ohne_fleisch, oben_fehler=oben_fehler,
                fleisch_texel=n_fl_tex, fremde_texel=fremd, falsche_zeile=falsch_zeile,
                ok=bool(zeile is not None and fremd == 0 and falsch_zeile == 0 and unten_ohne_fleisch == 0 and oben_fehler == 0 and n_unten > 0))


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
