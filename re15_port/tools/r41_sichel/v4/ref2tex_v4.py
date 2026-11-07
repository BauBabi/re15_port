"""SF V4 Originaltexturen (Bauer, Runde 2 von 2): Kopf- und Schwanztexel der Sichel-Form V4 aus dem Atlas der gelobten Referenz 2.

    python ref2tex_v4.py   -> tools/r41_sichel/v4/farben_ref2tex_v4.npz (+ Protokoll SF_belege/v4/ref2tex_protokoll.json)

Vorgabe (Orchestrator 2026-11-07 nach dem Nutzer "Bei der Sichel Form V4, warum sind denn da die Farben so extrem scheisse? ... die Form
baut von Farben und Texturen her ja nun wirklich auf die Urspruengliche Birkin Form auf"; ersetzt Messblatt V4 Abschnitt 5):
  (1) Koerper = Referenz 2 unveraendert (birkin_formen.py V4_TEXTUR 'ref2': keine Koerperzeilen/-texel, CLUT bytegleich).
  (2) Schwanz: Texel aus der KOERPERHAUT desselben Atlas (Oberarm, Mesh 13, Zeile 480), UV entlang der Achse durchgehend (uv art 'achse'
      aus V4 Runde 5: v = Bogenlaenge, u = Lage um den Umfang). Keine neue Farbe.
  (3) Kopf: Texel nur mit Eintraegen der Referenz-2-Zeile 480, die Referenz 2 auf ihren Koerper-Meshes zeigt; rote Kopfoberseite aus den
      roten Wundeintraegen, Auge aus den hellen, Haare aus den dunklen.
Runde 2 (Sicht-Pruefung Runde 1, SF_PRUEFUNG_REF2TEX_r1_sicht.md; Orchestrator Runde 2 (a)-(c)):
  - Kopf (main / kopf_bild_r2): Grundton = die Original-Kopftexel von Referenz 2 in IHRER Zeile 485 (Lab), je Texel auf den naechsten
    Eintrag der Zeile 480 abgebildet - keine Nachfuehrung an Video-L*-Zielen mehr (Runde 1 main_r1: Zonenhelligkeit an K5/K7/K8 13-18 L*
    nachgefuehrt -> Kopf L* P50 11,4, 80 % L* < 20). Dunkel nur das Haarfeld (vorn: Fenster KV2 + K6-Stelle, haar_vorn; seitlich: Fenster
    KS2, haar_seite - nicht mehr die Runde-5-Zone s_haar, die im Vorderbild 569 Texel der Gesichtsmitte belegte). Keine violettstichigen
    Eintraege (nicht_violett). Abnahme gegen Referenz 2: riegel_ref2tex.tk4 (T-K4 ohne Video-Farbziele, T-K5).
  - Schwanz (schwanz_textur_r2): Glieder 17-22 ab Texelzeile 56 texelgleich Runde 1; im Wurzelband (Glied 16) die Quelle laengs gedehnt
    (WURZEL_K) und je Texel ueber die ueberdeckten Quellspalten gemittelt - ohne den senkrecht gestreiften Block (Riegel T-K3d).
Kein RE-Befund: die Sichel-Form gibt es auf der Disc nicht, keine Zahl hier ist eine @0x-Konstante. Atlas-Orte sind gezaehlt (atlas()).
"""
import os, sys, json, argparse
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import birkin_formen as BF
import emd_ansichtsblatt as EA
import riegel_v4 as RV
import eichen_v4 as E

AUS = os.path.join(HIER, 'farben_ref2tex_v4.npz')
BAUM = G.BAUM
PROT = os.path.join(BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v4', 'ref2tex_protokoll.json')
ZEILE = BF.V4_REF2_ZEILE
KOERPER = list(range(9)) + list(range(10, 16))
R5_KOPF = os.path.join(HIER, 'farben_v4.npz')            # Zonenkarte des Kopfes (V4 Runde 5, committet)
# Feinstruktur Kopf: Original-Kopftexel Referenz 2 (Mesh 9 Ref2: TPage 128 u 66-126 v 4-90, Zaehlung atlas(); dieselben Texel zeigt
# Ref2 auf Mesh 6 in Zeile 480 = Wundfleisch)
# Groesstes ganz von Ref2-Kopfflaechen (Mesh 9) abgedecktes Rechteck darin: x 76-119, y 19-67 (44 x 49; Zaehlung wie atlas()); die
# erste Fassung mit dem ganzen Rechteck 66-126 x 4-90 brachte die schwarzen Randspalten ausserhalb der Kopfinsel als Streifen mit.
KOPF_QUELLE = (76, 19, 119, 67)                            # x0, y0, x1, y1 (Seite 0)
# Schwanz: groesstes ganz belegtes Rechteck der Oberarm-/Rueckenhaut (Zeile 480; Texel von Mesh 13 Oberarm, die Ref2 zum Teil auch auf
# Mesh 8 Rumpf und Mesh 10 zeigt: 5069 / 668 / 23 Texel) mit 64 Spalten (= Breite der Schwanzflaeche): Seite 0 x 1-64, y 15-104 (90 Zeilen;
# laengster ganz belegter Lauf je 64er-Spaltenfenster). Laenge 256 durch Spiegeln entlang v (keine Naht: an der
# Spiegelstelle folgt dieselbe Zeile).
SCHWANZ_QUELLE = (1, 15, 64, 104)
# Zonenklassen (Vorgabe 3): rot = Wundeintraege (a* >= 12 der Texelfarbe), hell = L* >= 45, dunkel = L* <= 22, haut = alle; jeweils nur
# Eintraege, die Ref2 auf Koerper-Meshes in Zeile 480 zeigt. Grenzen PORT-WAHL (Klasseneinteilung der Palette nach dem Vorgabe-Wortlaut
# "rote Kopfoberseite aus den roten Wundtexeln, Auge aus deren hellen Texeln, Haare aus deren dunklen").
ROT_A, HELL_L, DUNKEL_L = 12.0, 45.0, 22.0
# haut (alle Kopfzonen ausser Auge/Kuppe/Haare/Kinn) bis L* 40 (Texel): Lauf ref2tex 3 mit L* < 45 ergab weiter 2-3 K1-Komponenten (helle
# Strukturtexel in v_basis ueber der Fleckschwelle). Messblatt V4 Abschnitt 2: "der hellste Kopfpunkt ist der Augenfleck". PORT-WAHL.
HAUT_L = 40.0
KLASSE = dict(v_kuppe='rot', s_kuppe='rot', v_haar='dunkel', s_haar='dunkel', v_kinn='dunkelrot')
# Augenkern: Texel der Augenzone mit Gewicht >= AUGE_KERN bekommen helle Eintraege (Lauf ref2tex 1 mit 0,5: K3 8 x 8 px = 0,022 H, Soll
# 0,023-0,043 H; S1 0,027 H, Soll >= 0,028; Lauf ref2tex 3 mit 0,4: K3 7 x 8 px). PORT-WAHL nach Riegel K3/S1.
AUGE_KERN = 0.3
# Augenprofil: Gewicht clip(AUGE_PROFIL - d, 0, 1) ueber dem Ellipsenabstand d (1 = Sollgroesse K3/S1); 1,5 = Gewicht 0,5 am Sollrand,
# volles Gewicht bis d 0,5. PORT-WAHL nach Riegel K3/S1 (die Fleckschwelle liegt in der Mitte zwischen Kern und Umgebung).
AUGE_PROFIL = 1.5
# Feinstruktur: relative Helligkeit der Original-Kopftexel hoch STRUKTUR_EXP. Lauf ref2tex 2 mit 1,0: helle Strukturtexel in der Zone
# v_basis (Ziel KV9 29,3 L*) bildeten nach der Vergleichsregel eine zweite K1-Komponente (7 px ueber der Fleckschwelle 43,8). PORT-WAHL.
STRUKTUR_EXP = 0.7
STRUKTUR_FARBE = 0.5     # Anteil der a*/b*-Abweichung der Original-Kopftexel (PORT-WAHL, Machart; s. kopf_bild)
UMGEBUNG_DL = 6.0
AUGE_HOF = 0.85        # PORT-WAHL nach Riegel K1 (s. kopf_bild)
UMGEBUNG_SIGMA = 3.0   # Texel, oertliche Umgebung der Augenzone (PORT-WAHL, s. kopf_bild)
DAEMPF = 0.7         # Nachfuehrung je Runde: Faktor (Ziel / Ist) ** DAEMPF (Lauf ref2tex 2 mit 1,0 pendelte K2/K3/S4). PORT-WAHL
GLATT = 1.2          # Gauss (Texel) der Zonenhelligkeit an den Zonengrenzen: weicher Uebergang statt Kachelkante (PORT-WAHL, Machart)


def flaeche(uv):
    pg = uv[5] & 0xF
    pts = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])] + ([(uv[9], uv[10])] if len(uv) >= 12 else [])
    P = np.array([(x + 128 * pg, y) for x, y in pts], np.int32)
    return P[[0, 1, 3, 2]] if len(P) == 4 else P


def atlas(pfad=None):
    """Atlas einer EMD (Standard Referenz 2): Texel-Indizes, CLUT (roh), je Texel Zeile/Mesh der Koerperflaechen (0-8, 10-15)."""
    blob = open(pfad or G.REF2, 'rb').read(); d = EA.emd_dir(blob)
    tim = blob[d[8]:]
    _cx, cy, _cw, _ch, c, _e = BF._clut(tim)
    tex, _pal, _ = EA.tim_decode(tim)
    ms = EA.md1_parse(blob[d[7]:d[8]])
    zeile = np.full(tex.shape, -1, int); mesh = np.full(tex.shape, -1, int)
    for i in KOERPER:
        for uv in list(ms[i].tuv) + list(ms[i].quv):
            mk = np.zeros(tex.shape, np.uint8); cv2.fillPoly(mk, [flaeche(uv)], 1)
            zeile[mk > 0] = uv[2] >> 6; mesh[mk > 0] = i
    return dict(tex=tex, clut=c, cy=cy, zeile=zeile, mesh=mesh, rgb=BF._rgb(c), ms=ms, blob=blob, d=d)


def klassen(A):
    """Eintraege der Zeile 480, die Ref2 auf Koerperflaechen in Zeile 480 zeigt, und ihre Klassen."""
    z = ZEILE - A['cy']
    used = np.unique(A['tex'][A['zeile'] == ZEILE])
    used = used[A['clut'][z][used] != 0]
    lab = G.lab(A['rgb'][z][used])
    # haut ohne die hellen Eintraege: der hellste Kopfpunkt ist der Augenfleck (Messblatt V4 Abschnitt 2 "Keine helle Kuppe"); Lauf
    # ref2tex 1 (haut = alle): helle Feinstruktur-Texel unter dem Auge bildeten eine zweite K1-Komponente
    # hell nur die blassen Eintraege (a* <= 15): das Auge ist ein "heller, blass rosa-grauer Fleck" (Messblatt V4 Abschnitt 2, 46,9 / 7,2 /
    # -1,1); Lauf ref2tex 2 mit allen hellen Eintraegen: K2 a* 11,9 / b* 4,1 (Wundrot), dE00 6,5
    K = dict(haut=used[lab[:, 0] < HAUT_L], rot=used[lab[:, 1] >= ROT_A], hell=used[(lab[:, 0] >= HELL_L) & (lab[:, 1] <= 15)],
             dunkel=used[lab[:, 0] <= DUNKEL_L])
    # Kinn (K8 9,9 / 9,9 / 3,8: dunkel, rotstichig): dunkle Wundeintraege L* <= 30, a* >= 8 (Lauf ref2tex 6 mit a* >= 4: K8 a* 5,0, dE00 6,1)
    K['dunkelrot'] = used[(lab[:, 0] <= 30) & (lab[:, 1] >= 8)]
    return K, used


def spiegel(n, h):
    return np.where((n // h) % 2 == 0, n % h, h - 1 - n % h)


def quelle_struktur(A, shape, versatz):
    """Feinstruktur der Original-Kopftexel (Zeile 480), gespiegelt gekachelt auf ein Texelbild der Groesse shape: relative Helligkeit
    (L / Mittel) ** STRUKTUR_EXP und Farbabweichung (a*, b* minus Mittel)."""
    x0, y0, x1, y1 = KOPF_QUELLE
    idx = A['tex'][y0:y1 + 1, x0:x1 + 1]
    lab = G.lab(A['rgb'][ZEILE - A['cy']][idx])
    rel = (lab[..., 0] / max(float(lab[..., 0].mean()), 1e-6)) ** STRUKTUR_EXP
    dab = lab[..., 1:] - lab[..., 1:].reshape(-1, 2).mean(0)
    h, w = rel.shape
    vv = np.arange(shape[0])[:, None] + versatz[0]; uu = np.arange(shape[1])[None, :] + versatz[1]
    my, mx = spiegel(vv, h), spiegel(uu, w)
    return rel[my, mx], dab[my, mx]


def schwanz_spanne(mo, sw):
    """Je Texelzeile v der Schwanzflaeche die genutzte u-Spanne sp(v) (Flaechen der Meshes 16-22 im UV-Raum gerastert). Die UV-Abbildung
    'achse' (birkin_formen.kette_meshes_v4) setzt u = u0 + (1 - c) / 2 x sp mit c = Kosinus des Umfangswinkels zur Aussenkante."""
    uv0 = sw['uv']['u'][0]; pb = 128 * (sw['uv']['page'] & 0xF)
    m = np.zeros((256, 64), np.uint8)
    for b_ in G.SCHWANZ:
        for uv in list(mo.meshes[b_].tuv) + list(mo.meshes[b_].quv):
            P = flaeche(uv) - np.array([pb + uv0, 0])
            cv2.fillPoly(m, [P.astype(np.int32)], 1)
    sp = np.array([(np.nonzero(m[v])[0].max() if m[v].any() else 0) for v in range(256)], float)
    return np.maximum(sp, 1.0)


def schwanz_textur(A, sp):
    """Schwanzflaeche 256 x 64 (v = Bogenlaenge, u = Umfang): Oberarm-/Rueckenhaut SCHWANZ_QUELLE, entlang v gespiegelt gekachelt; quer
    nach dem Umfangswinkel entzerrt: Spalte u zeigt die Quellspalte acos(1 - 2 u / sp(v)) / pi x (Breite - 1). Ohne Entzerrung (erste
    Fassung) lagen an Ober- und Unterkante (c = +-1, u ~ 0 bzw. ~ sp) nur 1-2 Quellspalten ueber einer ganzen Seitenflaeche: Querstreifen
    an der Schwanzwurzel (Leiste 05 der ersten Fassung)."""
    x0, y0, x1, y1 = SCHWANZ_QUELLE
    src = A['tex'][y0:y1 + 1, x0:x1 + 1]
    assert (A['zeile'][y0:y1 + 1, x0:x1 + 1] == ZEILE).all() and np.isin(A['mesh'][y0:y1 + 1, x0:x1 + 1], (8, 10, 13)).all()
    W = src.shape[1]
    my = spiegel(np.arange(256), src.shape[0])
    uu = np.arange(64)[None, :].astype(float)
    th = np.arccos(np.clip(1.0 - 2.0 * uu / sp[:, None], -1.0, 1.0))
    mx = np.clip(np.rint(th / np.pi * (W - 1)), 0, W - 1).astype(int)
    return src[my[:, None], mx].astype(np.uint8)


def zonenkarte():
    f = np.load(R5_KOPF)
    out = {}
    for k in ('kopf_vorn', 'kopf_seite'):
        a = f[k].astype(int)
        zn = np.where(a > 0, (a - 1) // 16, -1)
        out[k] = (zn, G.lum(f['clut486'][a]))
    return out


def texel_lage(mo, kp, pose):
    """Je Kopf-Bildpunkt (Id 9) der Pose: (Bild 0 = vorn / 1 = seitlich, u, v) des Texels im Kopfbild, ueber drei Kenn-Renders ohne Licht
    (Zeile 480 als Kenn-CLUT R = Index; Kopfbilder = Bildnummer + 1 bzw. u bzw. v). Rueckgabe (ids, bild, u, v) mit -1 = kein Kopftexel."""
    tex0, lut0 = mo.tex, mo.lut
    pb = 128 * (kp['page'] & 0xF)
    out = []
    try:
        l = mo.lut.copy(); z = ZEILE - mo.clut_y
        l[z, :, 0] = np.arange(256); l[z, :, 1] = 0; l[z, :, 2] = 0; l[z, :, 3] = 255
        mo.lut = l
        for art in ('bild', 'u', 'v'):
            t = tex0.copy()
            for b_, ans in enumerate((kp['vorn'], kp['seite'])):
                u0, u1 = ans['u']; v0, v1 = ans['v']
                vv, uu = np.mgrid[0:v1 - v0 + 1, 0:u1 - u0 + 1]
                t[v0:v1 + 1, pb + u0:pb + u1 + 1] = {'bild': np.full(vv.shape, b_ + 1), 'u': uu, 'v': vv}[art].astype(np.uint8)
            mo.tex = t
            img, ids = G.zeichne(mo, pose, licht=False)
            out.append(img[..., 0].astype(int))
    finally:
        mo.tex, mo.lut = tex0, lut0
    kopf = ids == 9
    bild = np.where(kopf, out[0] - 1, -1)
    return ids, bild, np.where(kopf, out[1], -1), np.where(kopf, out[2], -1)


def augen_gewicht(mo, kp, P, shapes, je_pose=False):
    """Augenfleck als Gewicht je Texel beider Kopfbilder, gesetzt im Bild der jeweiligen Vorlage-Kamera an der Riegel-Stelle (Messblatt V4
    Abschnitt 7): vorn (Pose 135) K4 0,050 H (B)-links der Kopfmitte, 0,093 H unter der Kopf-Oberkante, Groesse K3 0,026 x 0,033 H;
    seitlich (Pose 85) S2 0,049 H hinter der Kopf-Vorderkante, 0,79 H ueber der Sohle, Groesse S1 0,040 x 0,055 H. Gewicht je Bildpunkt
    clip(AUGE_PROFIL - d, 0, 1) mit d = Ellipsenabstand (1 = Sollrand); je Texel das groesste Gewicht seiner Bildpunkte. Ein Texel des
    Vorderbilds kann so zum Seitenauge gehoeren (die vordersten Flaechen an der Kopfkante tragen in Pose 85 das Vorderbild)."""
    w = [np.zeros(sh) for sh in shapes]
    wp = {pose: [np.zeros(sh) for sh in shapes] for pose in ('135', '85')}
    for pose, art in (('135', 'vorn'), ('85', 'seite')):
        _img, _ids, iv = G.zeichne(mo, P[pose], voll=True)
        ok, so, H = G.h_port(iv)
        ids, bild, uu, vv = texel_lage(mo, kp, P[pose])
        x0, y0, x1, y1 = G.kopfbox(ids)
        if art == 'vorn':
            cx, cy, ax, ay = 0.5 * (x0 + x1) - 0.050 * H, y0 + 0.093 * H, 0.5 * 0.026 * H, 0.5 * 0.033 * H
        else:
            cx, cy, ax, ay = x1 - 0.049 * H, so - 0.79 * H, 0.5 * 0.040 * H, 0.5 * 0.055 * H
        yy, xx = np.nonzero(bild >= 0)
        d = np.sqrt(((xx + 0.5 - cx) / ax) ** 2 + ((yy + 0.5 - cy) / ay) ** 2)
        g = np.clip(AUGE_PROFIL - d, 0, 1)
        for b_ in (0, 1):
            m = bild[yy, xx] == b_
            np.maximum.at(w[b_], (vv[yy, xx][m], uu[yy, xx][m]), g[m])
            np.maximum.at(wp[pose][b_], (vv[yy, xx][m], uu[yy, xx][m]), g[m])
    return (w, wp) if je_pose else w


def kopf_bild(A, K, zn, w, quelle, T, namen, ansicht, auge_rand=0.6):
    """Texelbild einer Kopf-Ansicht: Zielfarbe je Zone (T, Texelfarbe RGB), weich an den Grenzen, x Feinstruktur rel, je Texel der
    naechste Eintrag (Lab) der Klasse seiner Zone. quelle = (rel, dab) aus quelle_struktur."""
    rel, dab = quelle
    pal = A['rgb'][ZEILE - A['cy']]
    sh = zn.shape
    basis = namen.index('v_basis' if ansicht == 'vorn' else 's_basis')
    z_auge = namen.index('v_auge' if ansicht == 'vorn' else 's_auge')
    zz = np.where(zn >= 0, zn, basis)
    auge = (zn == z_auge) | (w > 0)
    # Augenzone ausserhalb des Flecks = oertliche Umgebung: normierte Gauss-Mittelung (sigma UMGEBUNG_SIGMA Texel) der Zonenfarben der
    # Nicht-Augen-Texel. Lauf ref2tex 8 mit EINER Umgebungsfarbe je Bild: 40 Augenzonen-Bildpunkte im Kinnfenster KV4 (Streifen an der
    # Kopfkante, Runde-5-Zone v_auge) trugen das Mittel ums Auge, K8 a* 4,2 statt 9,9; Lauf ref2tex 9/10 mit Umschlag in die naechste Zone:
    # S1 zerfiel in 2 Komponenten. Klasse dieser Texel im Vorderbild: die der naechsten anderen Zone, im Seitenbild 'haut'.
    logT = np.log(np.maximum(T[zz], 1.0))
    M = (~auge & (zn >= 0)).astype(np.float32)
    den = cv2.GaussianBlur(M, (0, 0), UMGEBUNG_SIGMA) + 1e-6
    t_um = np.stack([cv2.GaussianBlur((logT[..., c] * M).astype(np.float32), (0, 0), UMGEBUNG_SIGMA) / den for c in range(3)], -1)
    zn_kl = zn.copy()
    if ansicht == 'vorn' and ((zn == z_auge) & (w <= 0)).any():
        frei = (zn == z_auge) & (w <= 0)
        _d, (iy, ix) = nd.distance_transform_edt((zn == z_auge) | (zn < 0), return_indices=True)
        zn_kl[frei] = zn[iy[frei], ix[frei]]
    # Fleckrand unregelmaessig nach der Original-Feinstruktur, aber ohne Einzeltexel: Struktur fuer den Rand Gauss 1 Texel (Leiste 03 der
    # ersten Fassung: punktierter Ring aus dunklen Einzeltexeln um den Fleck)
    rel_r = cv2.GaussianBlur(np.asarray(rel, np.float32), (0, 0), 1.0)
    w2 = np.clip(w + auge_rand * (rel_r - 1.0), 0, 1)
    w2[w <= 0] = 0
    logA = np.log(np.maximum(T[z_auge], 1.0))
    # Hof: die Augenzone ausserhalb des Kerns um den Faktor AUGE_HOF dunkler als ihre Umgebung (Original vorn: Fleck 56,2 L* in einer
    # Umgebung von 26,7 L* (Median 21 x 21, K1); Lauf ref2tex 12 ohne Hof: Umgebung des Port-Flecks 33,2 L*, K1-Kontrast 19,2-19,9)
    logT[auge] = (t_um[auge] + np.log(AUGE_HOF)) * (1 - w2[auge][:, None]) + logA[None, :] * w2[auge][:, None]
    glatt = np.stack([cv2.GaussianBlur(logT[..., c].astype(np.float32), (0, 0), GLATT) for c in range(3)], -1)
    glatt = np.where(auge[..., None], logT, glatt)          # Auge selbst ungeglaettet (sein Rand ist schon weich)
    kern = nd.binary_fill_holes(nd.binary_closing(auge & (w2 >= (AUGE_KERN if ansicht == 'vorn' else AUGE_KERN_S)))) & auge     # Kern ohne Loecher und Einzeltexel
    kern = nd.binary_opening(kern) | (kern & (w2 >= 0.9))
    # Augenkern nur mit schwacher Feinstruktur (+-10 %): die hellsten blassen Hauteintraege liegen bei L* 55 (Texel, Zeile 480); mit voller
    # Struktur blieb der Fleck nach der Vergleichsregel bei L* 41 (S1 Soll 48,4, Lauf ref2tex 3). PORT-WAHL nach Riegel K2/S1.
    rel = np.where(kern, np.clip(rel, 0.9, 1.1), rel)
    # Sollfarbe: Zonenfarbe (Lab) mit der Helligkeit x rel und der Farbabweichung der Original-Kopftexel x STRUKTUR_FARBE (die Ref2-Kopf-
    # texel mischen Wundrot und Grau; mit voller Farbabweichung wechselten rote und graue Flecken, mit nur Helligkeit wurden die Zonen
    # einfarbig, Lauf ref2tex 7). Im Augenkern keine Farbabweichung.
    soll_lab = G.lab(np.clip(np.exp(glatt), 0, 255))
    soll_lab[..., 0] *= rel
    soll_lab[..., 1:] += STRUKTUR_FARBE * np.where(kern[..., None], 0.0, dab)
    kl = np.full(sh, 'haut', object)
    for nm, k in KLASSE.items():
        kl[zn_kl == namen.index(nm)] = k
    kl[kern] = 'hell'
    out = np.zeros(sh, np.uint8)
    for k in set(kl.ravel()):
        sel = kl == k
        ein = K[k]
        lab_e = G.lab(pal[ein])
        d = ((soll_lab[sel][:, None, :] - lab_e[None, :, :]) ** 2).sum(-1)
        out[sel] = ein[np.argmin(d, 1)].astype(np.uint8)
    return out, w2


def bauen():
    """EMD V4 im Modus ref2 (die Kopf-/Schwanz-Texel der Datei werden danach im Speicher ersetzt)."""
    BF.BAU_INFO.clear()
    blob = BF.baue(BF.lade_em30(), BF._variante_v4())
    return blob, dict(BF.BAU_INFO['kopf_v4'])


def setze(mo, kp, sw, bilder):
    tex = mo.tex.copy()
    pb = 128 * (kp['page'] & 0xF)
    for name, ans in (('kopf_vorn', kp['vorn']), ('kopf_seite', kp['seite'])):
        u0, u1 = ans['u']; v0, v1 = ans['v']
        tex[v0:v1 + 1, pb + u0:pb + u1 + 1] = bilder[name]
    uv = sw['uv']; pbs = 128 * (uv['page'] & 0xF)
    tex[uv['v'][0]:uv['v'][1] + 1, pbs + uv['u'][0]:pbs + uv['u'][1] + 1] = bilder['schwanz']
    mo.tex = tex


def code_render(mo, kp, pose, zn, name):
    """Zone je Bildpunkt (Kopf): Kopfbild = Zone + 1, Zeile 480 als Kenn-CLUT (R = Index), ohne Licht."""
    tex0, lut0 = mo.tex, mo.lut
    try:
        t = mo.tex.copy(); pb = 128 * (kp['page'] & 0xF)
        ans = kp['vorn' if name == 'kopf_vorn' else 'seite']
        u0, u1 = ans['u']; v0, v1 = ans['v']
        t[v0:v1 + 1, pb + u0:pb + u1 + 1] = (zn + 1).astype(np.uint8)
        mo.tex = t
        l = mo.lut.copy(); z = ZEILE - mo.clut_y
        l[z, :, 0] = np.arange(256); l[z, :, 1] = 0; l[z, :, 2] = 0; l[z, :, 3] = 255
        mo.lut = l
        img, ids = G.zeichne(mo, pose, licht=False)
    finally:
        mo.tex, mo.lut = tex0, lut0
    return np.where(ids == 9, img[..., 0].astype(int) - 1, -2)


def main_r1():
    """Runde 1 (Zonenhelligkeit an Video-L*-Zielen nachgefuehrt; ueberholt nach der Sicht-Pruefung Runde 1 b1)."""
    ap = argparse.ArgumentParser()
    ap.add_argument('--runden', type=int, default=6)
    a = ap.parse_args()
    A = atlas()
    K, used = klassen(A)
    namen = [z[0] for z in E.ZONEN]
    ziele = E.ziele_original()
    # Umgebung des Augenflecks vorn (Zonen v_basis KV9 29,3 L*, v_oben_links KV6 34,3 L*, keine eigenen Riegel): das Port-Auge erreicht mit
    # den blassen Hauteintraegen der Zeile 480 (hoechstens L* 55 Texel) nach der Vergleichsregel L* ~48 statt 56,2 (Original-Extrem); damit
    # der K1-Kontrast (>= 20, Original 29,5) haelt, liegt die Umgebung UMGEBUNG_DL tiefer (Lauf ref2tex 6: Kontrast 18,2-19,4 bei Zielhelligkeit;
    # Lauf ref2tex 11 mit 4,0 und Feinstruktur in Lab: 18,7-19,9).
    # PORT-WAHL nach Riegel K1, Messblatt Abschnitt 2 "der hellste Kopfpunkt ist der Augenfleck".
    for k_ in ('KV9', 'KV6'):
        ziele[k_] = [ziele[k_][0] - UMGEBUNG_DL] + list(ziele[k_][1:])
    P = G.posen()
    zk = zonenkarte()
    blob, _info = bauen()
    mo = b2.modell(blob)
    form = BF._variante_v4(); kp, sw = form['kopf_v4'], form['schwanz_kette']
    rel_v = quelle_struktur(A, zk['kopf_vorn'][0].shape, (0, 0))
    rel_s = quelle_struktur(A, zk['kopf_seite'][0].shape, (20, 6))
    pal = A['rgb'][ZEILE - A['cy']]
    kl_lmax = {k: float(G.lab(pal[v])[:, 0].max()) for k, v in K.items()}
    schwanz = schwanz_textur(A, schwanz_spanne(mo, sw))
    pose_von = {'kopf_vorn': '135', 'kopf_seite': '85'}
    zmap = {n: code_render(mo, kp, P[pose_von[n]], zk[n][0], n) for n in pose_von}
    w_v, w_s = augen_gewicht(mo, kp, P, (zk['kopf_vorn'][0].shape, zk['kopf_seite'][0].shape))
    # Start: Zielfarbe im Port-Schirm / Licht (weisser Pruefkoerper, Median der Zonen-Bildpunkte)
    weiss = {n: G.zeichne(mo, P[pose_von[n]], weiss=True)[0].astype(float) / 248.0 for n in pose_von}
    T = np.zeros((len(namen), 3)); pz = {}
    for i, (nm, ans, zkey, _q) in enumerate(E.ZONEN):
        n = 'kopf_vorn' if ans == 'vorn' else 'kopf_seite'
        pz[i] = np.clip(G.port_ziel(E.lab_rgb(ziele[zkey])), 1, 255)
        m = zmap[n] == i
        lam = np.median(weiss[n][m], 0) if m.sum() > 5 else np.full(3, 0.7)
        T[i] = np.clip(pz[i] / np.maximum(lam, 0.05), 1, 255)
    prot = dict(quelle=dict(kopf=KOPF_QUELLE, schwanz=SCHWANZ_QUELLE, zeile=ZEILE),
                klassen={k: [int(x) for x in v] for k, v in K.items()}, eintraege_koerper=int(len(used)), runden=[])
    best = None
    for r in range(a.runden):
        bv, _wv = kopf_bild(A, K, zk['kopf_vorn'][0], w_v, rel_v, T, namen, 'vorn')
        bs, _ws = kopf_bild(A, K, zk['kopf_seite'][0], w_s, rel_s, T, namen, 'seite')
        setze(mo, kp, sw, dict(kopf_vorn=bv, kopf_seite=bs, schwanz=schwanz))
        stand = {}
        for n, pose in pose_von.items():
            img, ids = G.zeichne(mo, P[pose])
            V = G.videoraum(img); Lab = G.lab(V)
            kb = G.kopfbox(ids)
            fl = RV.kopf_blob(V, Lab, ids == 9, kb)
            for i, (nm, ans, zkey, _q) in enumerate(E.ZONEN):
                if ('kopf_vorn' if ans == 'vorn' else 'kopf_seite') != n:
                    continue
                # Ist je Zone wie der Riegel misst: Auge = Fleckmaske (K2/S1), Fenster = Riegelfenster auf der Port-Kopfbox (KV4 nur Kopf-
                # Bildpunkte, s. riegel_ref2tex K8), sonst die Zonen-Bildpunkte
                if nm in ('v_auge', 's_auge'):
                    m = fl['maske']
                elif zkey in RV.FENSTER or zkey in E.FENSTER_ZUS:
                    bx = G.fenster(kb, RV.FENSTER.get(zkey, E.FENSTER_ZUS.get(zkey)))
                    m = np.zeros(ids.shape, bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
                    if zkey == 'KV4':
                        m &= RV.er(ids == 9, 2)
                else:
                    m = nd.binary_erosion(zmap[n] == i, iterations=1)
                if m.sum() < 4:
                    continue
                stand[nm] = [round(float(x), 1) for x in RV.lab1(V[m].mean(0))]
                f_ = np.clip(pz[i] / np.maximum(G.port_ziel(V[m].mean(0)), 1.0), 0.6, 1.6) ** DAEMPF
                T[i] = np.clip(T[i] * f_, 1, 255)
        # Obergrenze je Zone: Helligkeit der Zonenfarbe hoechstens hellster Eintrag ihrer Klasse - 3 L* (sonst klemmt jede Sollfarbe am
        # hellsten Eintrag, die Zone wird einfarbig: Lauf ref2tex 7, graue/violette Bloecke im Seitenbild)
        for i, (nm, _a, _z, _q) in enumerate(E.ZONEN):
            kl_ = 'hell' if nm in ('v_auge', 's_auge') else KLASSE.get(nm, 'haut')
            Lz = float(G.lab(T[i].reshape(1, 3))[0, 0]); grenze = kl_lmax[kl_] - 3.0
            if Lz > grenze:
                for _ in range(20):
                    T[i] *= 0.97
                    if float(G.lab(T[i].reshape(1, 3))[0, 0]) <= grenze:
                        break
        import riegel_ref2tex as RR                 # T-K4 (K1-K10 mit K8 an den Kopf-Bildpunkten, S1-S4)
        z4, _i4 = RR.tk4(mo, P)
        rot = [z['mass'] + ': ' + z['ist'] for z in z4 if not z['ok']]
        dsum = sum(float(x.split('dE00 ')[1].split(';')[0].split()[0]) for x in [z['ist'] for z in z4 if not z.get('info')] if 'dE00 ' in x)
        print('Runde', r, 'rot', len(rot), 'Summe dE00 %.1f' % dsum, rot, flush=True)
        prot['runden'].append(dict(runde=r, rot=rot, summe_de00=round(dsum, 2), zonen_ist=stand))
        # bester Stand = wenigste rote T-K4-Zeilen, dann kleinste Summe dE00 (die Nachfuehrung pendelt um das Ziel)
        if best is None or (len(rot), dsum) < best[0]:
            best = ((len(rot), dsum), r, bv.copy(), bs.copy())
    _k, rb, bv, bs = best
    prot['gewaehlt'] = rb
    print('gewaehlt Runde', rb, _k)
    np.savez(AUS, kopf_vorn=bv, kopf_seite=bs, schwanz=schwanz)
    os.makedirs(os.path.dirname(PROT), exist_ok=True)
    json.dump(prot, open(PROT, 'w'), indent=1)
    print('geschrieben', AUS)


# ---------------------------------------------------------------------------------------------------- Runde 2 (Sicht-Pruefung Runde 1)
# Kopf Runde 2: Grundton = die Original-Kopftexel von Referenz 2 SELBST (ihre Farbe in der Ref2-Kopfzeile 485, nicht in 480), je Texel auf
# den naechsten Eintrag der Zeile 480 abgebildet; KEINE Nachfuehrung an Video-L*-Zielen (K5/K7/K8 ueberholt). Abnahme gegen den Ref2-Kopf
# im selben Render (riegel_ref2tex T-K5). Dunkel nur das Haarfeld (Zonen v_haar / s_haar), rot die Kuppe, hell der Augenkern.
KOPF_ZEILE_REF2 = 485       # CLUT-Zeile der Ref2-Kopfflaechen (Mesh 9: alle Flaechen Zeile 485, TPage 128; Zaehlung atlas().ms[9])
# Violettstich (Sicht-Pruefung Runde 1 b1: 23 % der Kopfpixel b* < 0): Eintraege mit b* < 0 oder rot mit b* < 2 (a* > 8) sind auf dem
# Original-Koerper Randfarben; sie fallen aus allen Kopf-/Schwanzklassen (gezaehlt klassen_r2(), Protokoll 'violett_ausgenommen').
VIOLETT_B, VIOLETT_A, VIOLETT_B_ROT = 0.0, 8.0, 2.0
# Alle folgenden Zahlen sind PORT-WAHL (kein RE-Befund, keine @0x-Konstante); gewaehlt am Riegel riegel_ref2tex.tk4 (T-K4/T-K5, Abnahme
# gegen Ref2), Gitter ref2tex_gitter.py, Ausgabe SF_belege/v4/ot2/ref2tex_gitter.txt (36 Punkte, 6 ohne rote Zeile; gewaehlt der erste).
# Hauthelligkeit: Faktor auf die Ref2-Kopftexel, vorn / seitlich (Gitter: s 1,20 und 1,25 vorn ohne Rot; seitlich 1,0, weil Pose 85 das
# Seitenbild heller beleuchtet: mit 1,25 Grundton 35,0-35,1 bei Ref2 30,8 +- 4).
S_VORN, S_SEITE = 1.2, 1.0
# Kuppe: Farbe der Ref2-Kopftexel, a* um KUPPE_DA angehoben, Helligkeit x KUPPE_L, je Texel naechster roter Eintrag (rote Wundeintraege
# a* >= 12 bis HAUT_L2). Riegel K9 / S4 (a* >= 11, |dL*| zu Ref2 <= 8; mit KUPPE_L 0,7 lag KV1 bei 28,6 gegen Ref2 41,8).
KUPPE_DA, KUPPE_L = 6.0, 1.0
# Knie der Hauthelligkeit (Texel-L*): oberhalb KNIE nur KNIE_F der Ref2-Kopfstruktur, damit die Lichtpunkte der Original-Kopftexel (P90
# L* 57,4) unter der Fleckschwelle des Riegels K1 bleiben (Schwelle = Mitte zwischen Augenfleck und Umgebung), ohne die Struktur zu kappen
# (erster Lauf mit harter Kappe bei HAUT_L 40: Gesicht flach, 2 K1-Komponenten).
KNIE, KNIE_F = 30.0, 0.4
# Obergrenze der Hauteintraege (Texel-L*): Runde 1 kappte bei HAUT_L 40; mit dem Knie bleibt die Struktur darunter (T-K5a: mit 40 Grundton
# vorn 22,8-25,5).
HAUT_L2 = 44.0
# Haarfeld vorn: Texel von KV2 + K6-Stelle (haar_vorn) um HAAR_ERO Texel erodiert; 0 = das ganze Fenster (K5 KV2 L* <= 22,8 braucht es).
HAAR_ERO = 0
# Haar: Helligkeit HAAR_F x Median der dunklen Eintraege (haar_l 16,1; mit 1,0: K6 L* 18,4-20,8 > 18, S3 KS2 24,4-25,8); gewaehlt wird je
# Texel ein vorhandener dunkler Eintrag.
HAAR_F = 0.5
HAAR_F_V = 0.5   # vorn (s. main-Lauf); seitlich HAAR_F
# Augenhof: Haut um den Augenkern dunkler, Faktor 1 - (1 - HOF_F) x exp(-(d / HOF_R)^2), d = Abstand zum Kern in Texeln, nicht unter
# HOF_MIN (Texel-L*; darunter waere es dunkle Textur, T-K5d Albedo < 25). Ohne Hof K1-Kontrast 12,1-13,9 (Grenze 20): die Ref2-Haut ist
# blass, der hellste blasse Eintrag der Zeile 480 hat L* 55. Seitlich HOF_F_S (S1 braucht keinen Hof; mit HOF_F lag die Haut ums
# Seitenauge bei Texel-L* 24-25).
HOF_F, HOF_R, HOF_MIN = 0.62, 8.0, 29.0
HOF_F_S = 0.9
# Haarrand: Anteil Haar = Gauss(Maske, HAAR_SIGMA) + HAAR_STRUKTUR x (Ref2-Kopfstruktur - 1), Haar ab 0,5 - der Rand folgt der Original-
# Kopftextur statt der Fensterkante (ohne: Haarfeld als scharfes Rechteck).
HAAR_SIGMA, HAAR_STRUKTUR = 1.5, 0.8
# Augenkern seitlich: Gewichtsschwelle (vorn AUGE_KERN 0,3); mit 0,3: S1 0,030 x 0,027 H (Soll 0,028-0,052 x 0,040-0,070).
AUGE_KERN_S = 0.15
# Kinn (Zone v_kinn): die Ref2-Kopftexel ohne Knie, x KINN_F, bis KINN_L (Texel-L*) - die Original-Kopftextur ist am Kinn hell (Ref2
# Albedo-L* im Fenster KV4 47,9); das Kinn liegt in Pose 135 im Schatten (weisser Pruefkoerper L* P50 ~50), keine K1-Komponente moeglich.
KINN_F, KINN_L = 1.15, 56.0
# Nase (Texel, die Pose 135 im Fenster KV3 zeigt): Ref2-Kopftexel ohne Knie x NASE_F bis HAUT_L2 (Riegel K7 Nase >= Haarfeld + 8: ohne
# lag KV3 bei 21,2-23,2 und damit nur 4-8 ueber dem Haar).
NASE_F = 1.1


def nicht_violett(lab):
    return ~((lab[:, 2] < VIOLETT_B) | ((lab[:, 1] > VIOLETT_A) & (lab[:, 2] < VIOLETT_B_ROT)))


def klassen_r2(A):
    """Wie klassen(), aber ohne violettstichige Eintraege; 'haut' = alle nicht violetten Eintraege bis HAUT_L2."""
    z = ZEILE - A['cy']
    used = np.unique(A['tex'][A['zeile'] == ZEILE])
    used = used[A['clut'][z][used] != 0]
    lab = G.lab(A['rgb'][z][used].astype(float))
    nv = nicht_violett(lab)
    # rot nur bis HAUT_L2: Lauf exp 1.3:1.0 zweite K1-Komponente in der Kuppe (x 355-359 y 159-165, L* 53) aus einem hellen Wundeintrag
    K = dict(haut=used[nv & (lab[:, 0] < HAUT_L2)], rot=used[nv & (lab[:, 1] >= ROT_A) & (lab[:, 0] < HAUT_L2)],
             hell=used[nv & (lab[:, 0] >= HELL_L) & (lab[:, 1] <= 15)], dunkel=used[nv & (lab[:, 0] <= DUNKEL_L)],
             kinn=used[nv & (lab[:, 0] < KINN_L)])
    haar_l = float(np.median(G.lab(A['rgb'][z][K['dunkel']].astype(float))[:, 0]))
    return K, used, haar_l, int((~nv).sum())


def quelle_kopf(A, shape, versatz):
    """Original-Kopftexel von Referenz 2 (KOPF_QUELLE, ganz Mesh 9) in IHRER Zeile 485 als Lab, gespiegelt gekachelt auf shape."""
    x0, y0, x1, y1 = KOPF_QUELLE
    idx = A['tex'][y0:y1 + 1, x0:x1 + 1]
    lab = G.lab(A['rgb'][KOPF_ZEILE_REF2 - A['cy']][idx].astype(float))
    h, w = idx.shape
    vv = np.arange(shape[0])[:, None] + versatz[0]; uu = np.arange(shape[1])[None, :] + versatz[1]
    return lab[spiegel(vv, h), spiegel(uu, w)]


def haar_vorn(mo, kp, P, shape):
    """Haarfeld im Vorderbild: die Texel, die Pose 135 (Vorlage-1-Kamera) im Haarfenster KV2 (Messblatt V4 Abschnitt 7, K5) zeigt,
    geschlossen (3 x 3) und ohne Loecher. Grund (Sicht-Pruefung Runde 1 b1/c2): die Zonenkarte der Runde 5 legte die Zone s_haar (das
    Haar der SEITENansicht) auch ins Vorderbild, dort auf 569 Texel der Gesichtsmitte (u 10-35, v 18-46; Pose 85 sieht diese Flaechen
    nur als schmalen Streifen an der Kopfvorderkante) - von vorn wurde damit 80 % des Kopfes dunkel."""
    _img, _ids, iv = G.zeichne(mo, P['135'], voll=True)
    _ok, _so, H = G.h_port(iv)
    ids, bild, uu, vv = texel_lage(mo, kp, P['135'])
    kb = G.kopfbox(ids)
    bx = G.fenster(kb, RV.FENSTER['KV2'])
    # dazu die an der Kopfmitte gespiegelte Augenstelle (Riegel K6 "zweites Auge verdeckt", Original L* 12,9): Soll-Augenmitte K4 0,050 H
    # (B)-links der Kopfmitte, 0,093 H unter der Oberkante; gespiegelt in der groessten Fleckbox, die K3 zulaesst (0,036 x 0,043 H; der
    # Riegel spiegelt die gemessene Fleckbox). Lauf exp 1.3:1.0: die K6-Box (x 367-377) ragte 5 px rechts aus KV2 (x 354-372) heraus.
    xm = 0.5 * (kb[0] + kb[2])
    sx, sy = xm + 0.050 * H, kb[1] + 0.093 * H
    b6 = (sx - 0.018 * H, sy - 0.0215 * H, sx + 0.018 * H, sy + 0.0215 * H)
    m = np.zeros(shape, bool)
    ys, xs = np.nonzero(bild == 0)
    sel = ((xs >= bx[0]) & (xs <= bx[2]) & (ys >= bx[1]) & (ys <= bx[3])) | \
          ((xs + 0.5 >= b6[0]) & (xs + 0.5 <= b6[2]) & (ys + 0.5 >= b6[1]) & (ys + 0.5 <= b6[3]))
    m[vv[ys[sel], xs[sel]], uu[ys[sel], xs[sel]]] = True
    m = nd.binary_fill_holes(nd.binary_closing(m, np.ones((3, 3)), iterations=2))
    return nd.binary_erosion(m, iterations=int(HAAR_ERO)) if HAAR_ERO >= 1 else m


def fenster_texel(mo, kp, P, pose, fenster, bild_nr, shape):
    """Texel des Kopfbilds bild_nr (0 vorn, 1 seitlich), die die Pose im Messblatt-Fenster zeigt (geschlossen, ohne Loecher). 'UNTEN' = der
    Kopf unterhalb von SAUM_BIS der Kopfbox (Runde 3, Saum)."""
    ids, bild, uu, vv = texel_lage(mo, kp, P[pose])
    bx = G.fenster(G.kopfbox(ids), RV.FENSTER[fenster] if fenster != 'UNTEN' else (0.0, SAUM_BIS, 1.0, 1.1))
    m = np.zeros(shape, bool)
    ys, xs = np.nonzero(bild == bild_nr)
    sel = (xs >= bx[0]) & (xs <= bx[2]) & (ys >= bx[1]) & (ys <= bx[3])
    m[vv[ys[sel], xs[sel]], uu[ys[sel], xs[sel]]] = True
    return nd.binary_fill_holes(nd.binary_closing(m, np.ones((3, 3)), iterations=1))


def haar_seite(mo, kp, P, shape):
    """Haarfeld im Seitenbild: die Texel des Seitenbilds, die Pose 85 (Vorlage-2-Kamera) im Haarfenster KS2 (Messblatt V4 Abschnitt 7, S3)
    zeigt, geschlossen und ohne Loecher. Die Runde-5-Zone s_haar reicht im Seitenbild ueber das Fenster hinaus (Lauf exp HOF 0,72/10:
    dunkle Textur ausserhalb des Haarfelds Pose 85 Anteil 0,07-0,09, Ref2 0,00)."""
    ids, bild, uu, vv = texel_lage(mo, kp, P['85'])
    bx = G.fenster(G.kopfbox(ids), RV.FENSTER['KS2'])
    m = np.zeros(shape, bool)
    ys, xs = np.nonzero(bild == 1)
    sel = (xs >= bx[0]) & (xs <= bx[2]) & (ys >= bx[1]) & (ys <= bx[3])
    m[vv[ys[sel], xs[sel]], uu[ys[sel], xs[sel]]] = True
    return nd.binary_fill_holes(nd.binary_closing(m, np.ones((3, 3)), iterations=2))


def kopf_bild_r2(A, K, zn, w, src, namen, ansicht, s_haut, haar_l, haar_m=None, nase_m=None):
    """Texelbild einer Kopf-Ansicht, Runde 2: Sollfarbe je Texel = Ref2-Kopftexel (Lab, Zeile 485) mit Hauthelligkeit s_haut; Haarzone
    dunkel (Helligkeit relativ x haar_l), Kuppe rot, Augenkern hell; weich an den Zonengrenzen; je Texel der naechste Eintrag (Lab) der
    Klasse. Keine Videowerte."""
    pal = A['rgb'][ZEILE - A['cy']].astype(float)
    sh = zn.shape
    z_auge = namen.index('v_auge' if ansicht == 'vorn' else 's_auge')
    if ansicht == 'vorn':           # vorn: nur das Haarfeld aus haar_vorn (nicht s_haar, s. haar_vorn)
        haar = haar_m if haar_m is not None else (zn == namen.index('v_haar'))
    else:                           # seitlich: das Haarfeld aus haar_seite (Fenster KS2), sonst die Zone s_haar
        haar = haar_m if haar_m is not None else (zn == namen.index('s_haar'))
    kuppe = np.isin(zn, [namen.index(n) for n in ('v_kuppe', 's_kuppe')])
    L, a, b = src[..., 0], src[..., 1], src[..., 2]
    rel = L / max(float(L.mean()), 1e-6)
    soll = np.zeros(sh + (3,))
    Lk = np.where(L <= KNIE, L, KNIE + (L - KNIE) * KNIE_F)
    soll[..., 0] = np.minimum(Lk * s_haut, HAUT_L2 - 0.5); soll[..., 1] = a; soll[..., 2] = b
    # Uebergaenge: Anteil Haar / Kuppe weich (Gauss GLATT Texel), Sollfarbe gemischt; Klasse nach dem groesseren Anteil
    rel_g = cv2.GaussianBlur(rel.astype(np.float32), (0, 0), 1.0)
    wh = np.clip(cv2.GaussianBlur(haar.astype(np.float32), (0, 0), HAAR_SIGMA) + HAAR_STRUKTUR * (rel_g - 1.0) * (haar.any()), 0, 1)
    wh = np.clip((wh - 0.5) * 3.0 + 0.5, 0, 1)                 # Uebergang ~1 Texel breit, Rand nach der Struktur
    wk = cv2.GaussianBlur(kuppe.astype(np.float32), (0, 0), GLATT)
    dk = G.lab(pal[K['dunkel']])
    s_h = np.stack([(HAAR_F_V if ansicht == 'vorn' else HAAR_F) * haar_l * rel, np.full(sh, float(np.median(dk[:, 1]))), np.full(sh, float(np.median(dk[:, 2])))], -1)
    s_k = np.stack([Lk * s_haut * KUPPE_L, a + KUPPE_DA, b], -1)
    kinn = zn == namen.index('v_kinn') if 'v_kinn' in namen else np.zeros(sh, bool)
    soll[kinn, 0] = np.minimum(L[kinn] * s_haut * KINN_F, KINN_L - 0.5)
    if nase_m is not None:          # Nase (Fenster KV3): Ref2-Kopftexel ohne Knie x NASE_F bis HAUT_L2 - heller Nasenruecken
        soll[nase_m, 0] = np.minimum(L[nase_m] * s_haut * NASE_F, HAUT_L2 - 0.5)
    soll = soll * (1 - wh - wk)[..., None] + s_h * wh[..., None] + s_k * wk[..., None]
    # Auge: Kern hell (Gewicht >= AUGE_KERN, ohne Loecher), Rand weich nach der Feinstruktur, Hof AUGE_HOF
    auge = (zn == z_auge) | (w > 0)
    w2 = np.clip(w + 0.6 * (cv2.GaussianBlur(rel.astype(np.float32), (0, 0), 1.0) - 1.0), 0, 1); w2[w <= 0] = 0
    kern = nd.binary_fill_holes(nd.binary_closing(auge & (w2 >= (AUGE_KERN if ansicht == 'vorn' else AUGE_KERN_S)))) & auge
    kern = nd.binary_opening(kern) | (kern & (w2 >= 0.9))
    hl = G.lab(pal[K['hell']]); e = hl[np.argmax(hl[:, 0])]
    hof = auge & ~kern
    if kern.any():
        dk_ = nd.distance_transform_edt(~kern)
        fak = 1.0 - (1.0 - (HOF_F if ansicht == 'vorn' else HOF_F_S)) * np.exp(-(dk_ / HOF_R) ** 2)
        soll[..., 0] = np.where(kern, soll[..., 0], np.minimum(soll[..., 0], np.maximum(soll[..., 0] * fak, HOF_MIN)))
    soll[kern] = e[None, :]
    kl = np.full(sh, 'haut', object)
    kl[kinn] = 'kinn'
    kl[wh >= 0.5] = 'dunkel'; kl[(wk >= 0.5) & (wh < 0.5)] = 'rot'; kl[kern] = 'hell'
    out = np.zeros(sh, np.uint8)
    for k in set(kl.ravel()):
        sel = kl == k
        ein = K[k]
        lab_e = G.lab(pal[ein])
        d = ((soll[sel][:, None, :] - lab_e[None, :, :]) ** 2).sum(-1)
        out[sel] = ein[np.argmin(d, 1)].astype(np.uint8)
    return out


# Wurzel (Glied 16): Quellzeilen je Texelzeile v. In Pose 85 (Vorlage-2-Kamera) liegt die Oberseite der Wurzel laengs im Bild; gemessen
# (Kenn-Render u/v, scratchpad r2ot/wurzel.py): in den oberen 14 Zeilen von Id 16 aendert sich v um 1 Texel je Bildpunkt waagerecht (v 19-53
# ueber x 349-381), u um ~0,3 Texel je Bildpunkt senkrecht (u 0-5 ueber 12 Zeilen) - die Hautstruktur erscheint dort ~3-fach laengs
# gestaucht: senkrechte Streifen (Sicht-Pruefung Runde 1 b2b, waagerecht 7,45 / senkrecht 4,36). Im Wurzelband (v < WURZEL_V1) wird die
# Quelle mit WURZEL_K Quellzeilen je Texelzeile gelesen, bis WURZEL_V2 linear auf 1; ab WURZEL_V2 ist die Quellzeile = v wie in Runde 1
# (Glieder 17-22 bleiben texelgleich zu Runde 1: Sicht-Pruefung Runde 1 "Abweichung klein", Orchestrator Runde 2 (c)). Die Zeilenfolge ist
# stetig (kein Sprung). WURZEL_V1/V2: Glied 16 endet bei v 54 (Bogenlaenge 141,6 + 624 von 3632,6, x 255). PORT-WAHL nach Riegel T-K3d.
WURZEL_K, WURZEL_V1, WURZEL_V2 = 0.55, 50, 56


def quellzeile(n=256):
    """Quellzeile (Gleitkomma) je Texelzeile v: ab WURZEL_V2 gleich v, davor rueckwaerts mit Schrittweite k(v) (WURZEL_K, Rampe)."""
    v = np.arange(n, dtype=float)
    k = np.where(v < WURZEL_V1, WURZEL_K, np.where(v < WURZEL_V2, WURZEL_K + (1 - WURZEL_K) * (v - WURZEL_V1) / (WURZEL_V2 - WURZEL_V1), 1.0))
    q = v.copy()
    for i in range(WURZEL_V2 - 1, -1, -1):
        q[i] = q[i + 1] - k[i]
    return q


def schwanz_textur_r2(A, sp):
    """Schwanzflaeche Runde 2: schwanz_textur (Runde 1: Oberarm-/Rueckenhaut, gespiegelt entlang v, quer nach dem Umfangswinkel), aber
    die Quellzeile je Texelzeile aus quellzeile() - im Wurzelband gedehnt (s. WURZEL_K), ab WURZEL_V2 texelgleich zu Runde 1."""
    x0, y0, x1, y1 = SCHWANZ_QUELLE
    src = A['tex'][y0:y1 + 1, x0:x1 + 1]
    assert (A['zeile'][y0:y1 + 1, x0:x1 + 1] == ZEILE).all() and np.isin(A['mesh'][y0:y1 + 1, x0:x1 + 1], (8, 10, 13)).all()
    W = src.shape[1]
    my = spiegel(np.rint(quellzeile(256)).astype(int), src.shape[0])
    uu = np.arange(64)[None, :].astype(float)
    th = np.arccos(np.clip(1.0 - 2.0 * uu / sp[:, None], -1.0, 1.0))
    mx = np.clip(np.rint(th / np.pi * (W - 1)), 0, W - 1).astype(int)
    out = src[my[:, None], mx].astype(np.uint8)
    # Wurzelband: je Texel die mittlere Farbe (Lab) aller Quellspalten seines Winkelbereichs [theta(u), theta(u + 1)], dann der naechste
    # nicht violette Eintrag der Quelle. Am Rand (u 0-1, Oberseite der Wurzel in Pose 85) ueberdeckt ein Texel ~5 Quellspalten; mit EINER
    # Spalte (Runde 1) bestimmte eine einzelne Ader-/Lichtspalte der Quelle den ganzen Oberseitenstreifen (rosa-roter Block, Sicht-
    # Pruefung Runde 1 b2b; Kenn-Render scratchpad r2ot/wurzel_vgl.png).
    pal = A['rgb'][ZEILE - A['cy']].astype(float)
    lab_src = G.lab(pal[src])
    ein = np.unique(src); lab_e = G.lab(pal[ein]); ein = ein[nicht_violett(lab_e)]; lab_e = G.lab(pal[ein])
    for v in range(WURZEL_V2):
        tv = np.arccos(np.clip(1.0 - 2.0 * np.arange(65) / sp[v], -1.0, 1.0)) / np.pi * (W - 1)
        for u in range(64):
            k0 = int(np.floor(min(tv[u], tv[u + 1]))); k1 = int(np.ceil(max(tv[u], tv[u + 1])))
            k0 = min(k0, W - 1); k1 = min(max(k1, k0 + 1), W)
            m = lab_src[my[v], k0:k1].mean(0)
            out[v, u] = ein[np.argmin(((lab_e - m[None, :]) ** 2).sum(1))]
    return out


def kopf_mass(mo, P, pose):
    """Kopf (Id 9) im rohen Render: L* P50, Kopf/Rumpf (Mittel L*, Id 8), Anteile L* < 20 / < 12, b* < 0 (Messverfahren der Sicht-Pruefung
    Runde 1: Lab je Id-Maske, gleiches Licht)."""
    img, ids = G.zeichne(mo, P[pose])
    Lab = G.lab(img.astype(float))
    k, r = Lab[ids == 9], Lab[ids == 8]
    return dict(p50=float(np.median(k[:, 0])), verh=float(k[:, 0].mean() / r[:, 0].mean()), d20=float((k[:, 0] < 20).mean()),
                d12=float((k[:, 0] < 12).mean()), viol=float((k[:, 2] < 0).mean()))


def main_r2():
    """Runde 2: Kopf mit den festen PORT-WAHLen (S_VORN / S_SEITE ...), Schwanz schwanz_textur_r2; T-K4/T-K5 werden ausgegeben."""
    A = atlas()
    K, used, haar_l, n_viol = klassen_r2(A)
    namen = [z[0] for z in E.ZONEN]
    P = G.posen()
    zk = zonenkarte()
    blob, _info = bauen()
    mo = b2.modell(blob)
    form = BF._variante_v4(); kp, sw = form['kopf_v4'], form['schwanz_kette']
    src_v = quelle_kopf(A, zk['kopf_vorn'][0].shape, (0, 0))
    src_s = quelle_kopf(A, zk['kopf_seite'][0].shape, (20, 6))
    schwanz = schwanz_textur_r2(A, schwanz_spanne(mo, sw))
    w_v, w_s = augen_gewicht(mo, kp, P, (zk['kopf_vorn'][0].shape, zk['kopf_seite'][0].shape))
    hv = haar_vorn(mo, kp, P, zk['kopf_vorn'][0].shape)
    hs = haar_seite(mo, kp, P, zk['kopf_seite'][0].shape)
    nv_ = fenster_texel(mo, kp, P, '135', 'KV3', 0, zk['kopf_vorn'][0].shape) & ~hv
    bv = kopf_bild_r2(A, K, zk['kopf_vorn'][0], w_v, src_v, namen, 'vorn', S_VORN, haar_l, hv, nv_)
    bs = kopf_bild_r2(A, K, zk['kopf_seite'][0], w_s, src_s, namen, 'seite', S_SEITE, haar_l, hs)
    setze(mo, kp, sw, dict(kopf_vorn=bv, kopf_seite=bs, schwanz=schwanz))
    import riegel_ref2tex as RR
    z4, _i4 = RR.tk4(mo, P)
    ref = b2.modell(G.REF2)
    prot = dict(runde=2, quelle=dict(kopf=KOPF_QUELLE, kopf_zeile=KOPF_ZEILE_REF2, schwanz=SCHWANZ_QUELLE, zeile=ZEILE),
                klassen={k: [int(x) for x in v] for k, v in K.items()}, eintraege_koerper=int(len(used)), violett_ausgenommen=n_viol,
                haar_l=round(haar_l, 2), haarfeld_texel=dict(vorn=int(hv.sum()), seite=int(hs.sum())),
                port_wahl=dict(S_VORN=S_VORN, S_SEITE=S_SEITE, KUPPE_DA=KUPPE_DA, KUPPE_L=KUPPE_L, KNIE=KNIE, KNIE_F=KNIE_F, HAUT_L2=HAUT_L2,
                               HAAR_ERO=HAAR_ERO, HAAR_F=HAAR_F, HOF_F=HOF_F, HOF_R=HOF_R, HOF_MIN=HOF_MIN, HOF_F_S=HOF_F_S,
                               HAAR_SIGMA=HAAR_SIGMA, HAAR_STRUKTUR=HAAR_STRUKTUR, AUGE_KERN=AUGE_KERN, AUGE_KERN_S=AUGE_KERN_S,
                               KINN_F=KINN_F, KINN_L=KINN_L, NASE_F=NASE_F, HAAR_F_V=HAAR_F_V),
                ref2_kopf={p: kopf_mass(ref, P, p) for p in ('135', '85')}, ist_kopf={p: kopf_mass(mo, P, p) for p in ('135', '85')},
                tk4=[dict(mass=z['mass'], ist=z['ist'], ok=z['ok']) for z in z4])
    for z in z4:
        print('INFO' if z.get('info') else ('OK  ' if z['ok'] else 'ROT '), z['mass'], '|', z['ist'])
    np.savez(AUS, kopf_vorn=bv, kopf_seite=bs, schwanz=schwanz)
    os.makedirs(os.path.dirname(PROT), exist_ok=True)
    json.dump(prot, open(PROT, 'w'), indent=1)
    print('geschrieben', AUS)


# ---------------------------------------------------------------------------------------------------- Runde 3 (Kopf nach Nutzerbeschreibung)
# Nutzer 2026-11-07 (nach Ansicht des Kopfes der Runde 2): "da ist links ein Auge. Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite
# sieht man Haare - braeunlich.... Warum mischst du die Farben so daemlich?" Lesung (Orchestrator, bindend; Seiten = Bildkoordinaten der
# Spielkamera 50A0 Cut 12 wie Beschreibung1):
#   (1) linke Bildhaelfte des Kopfes = graeuliche Haut mit dem Auge (heller unbunter Fleck am Ort K4), bis zur Nase (K7 = Grenze);
#   (2) rechte Bildhaelfte = Haare, braeunlich (a* > 0, b* > 0, nicht schwarz; etwa 25 % dunkler als der Kopf-Median), eine Flaeche mit
#       weichem, strukturfolgendem Rand (kein Block, kein Fenster);
#   (3) keine Farbmischung: Kopfoberseite dunkel rotbraun (hoechstens Kopf-Median + 6 L*), nahtlos zu den Haaren;
#   (4) Farbquelle zuerst die Ref2-Palette (Zeile 480: unbunte Eintraege fuer Grau, warm-dunkle fuer Braun, dunkle rote fuer die Kuppe).
# Zonen: je Texel sein Ort im Kopfraum (Mesh 9, mesh-lokal, aus den UV-Flaechen gerastert, tex3d()) und drei Ebenen:
#   Seitenebene durch die Nase (Mitte des Messblatt-Fensters KV3 in Pose 135), senkrecht im Bild der Pose 135 (enthaelt Sehstrahl und
#   Bild-oben) -> s > 0 = Bild-rechts = Haare; Kuppe = Ellipse um die rote Zone des Messblatts im Bild der Pose 135 bzw. 85 (KUPPE_ELL).
#   Rand: s plus RAND_STRUKTUR x (Ref2-Kopfstruktur - 1), Rampe RAND_W (E.) - der Rand folgt der Original-Kopftextur statt der Ebene. Messung: Kopfpixel Pose 135 (explore2.py, Scratchpad ot3): Ebene -> grau 0,48 / Haar 0,36 /
#   Kuppe 0,11 der Kopfpixel, KV5 1,00 grau, KV2 0,84 Haar, KV3 0,53/0,47 (Grenze), Auge K4 grau.
# Alle Zahlen PORT-WAHL (kein RE-Befund; die Sichel-Form gibt es auf der Disc nicht).
GRAU_C, GRAU_L0 = 7.5, 26.0            # grau: Chroma <= 7,5 (Eintraege 72/64/64 .. 144/128/128, a* <= 6,5, b* <= 5,7), L* >= 26 (Texel); Lauf 3 mit 8,7: Render-
#                                        Chroma der grauen Zone 6,1 > 0,5 x Rumpf
BRAUN_L, BRAUN_C, BRAUN_BA = (12.0, 40.0), 17.0, 0.6    # braun: a* > 0, b* > 0, b* >= 0,6 a* (braun, nicht dunkelrosa), L* 12-40, Chroma <= 17, nicht grau
KUPPE_A, KUPPE_L, KUPPE_BA = (12.0, 22.0), (20.0, 36.0), 0.4   # kuppe (rotbraun): a* 12-22 (kein Wundrot a* > 22), b* >= 0,4 a*, L* 20-36, Chroma > 17
HELL_C = 7.5                           # Auge: hell (L* >= HELL_L) und unbunt (Chroma <= 7,5)
HAAR_REL = 0.8                         # Haar-Sollhelligkeit = HAAR_REL x Grau-Soll desselben Texels (Texel). Im Render Pose 135 liegt die Haarseite im
#                                        Schatten des RDT-Lichts: Lauf 1/2 (Klasse klemmte am Median-Eintrag, Haar texelgleich hell oder heller) schon
#                                        0,75 x Kopf-Median im Render; die Texel selbst sollen nur wenig dunkler sein als die graue Haut
KUPPE_REL = 0.9                        # Kuppe-Sollhelligkeit relativ zum Grau-Soll (Ziel <= Kopf-Median + 6; Lauf 1 mit 1,0: Render 34,0 bei Median 25,3)
GRAU_AB = 0.15                         # Anteil der a*/b*-Abweichung der Ref2-Kopftexel im Grau-Soll (Rest unbunt; Lauf 3 mit 0,3 s. GRAU_C)
# Helligkeit: die Ref2-Kopftexel (Zeile 485) sind ueber das Kopfbild nicht gleich hell (gespiegelt gekachelt; Lauf 3: src L* P50 45,8 auf der
# Haarseite, 30,8 auf der grauen Seite des Vorderbilds -> Haar texelheller als Grau). Runde 3 nimmt deshalb nur die OERTLICHE Struktur
# (L / Gauss(L, STRUKTUR_SIGMA)) und setzt die Zonenhelligkeit auf GRAU_BASIS = Albedo-L* P50 des Ref2-Kopfes (37,2; riegel_ref2tex T-K5a
# 'Ref2 37.2', Render-P50 30,8), Lichter mit dem Knie KNIE_F.
GRAU_BASIS = 40.0                      # Lauf 4 mit 37,2: Soll-L* P50 30,3, gewaehlt 27,9 (die Eintragswahl zieht zu den neutralen Eintraegen 72/64/64),
#                                        Render-P50 der grauen Zone 27,4 bei Ziel Ref2-Kopf 30,8 -> Basis 40 (Soll ~33, Eintraege 88/72/72 .. 96/88/88)
STRUKTUR_SIGMA = 6.0                   # Texel
STRUKTUR_MIN, STRUKTUR_MAX = 0.6, 1.5
# Kuppe: Ellipse im Bild der jeweiligen Vorlage-Kamera um die rote Zone des Messblatts (Abschnitt 2: Vorlage 1 x 388-406, y 140-156 in der
# Kopfbox 368-416 x 139-191 -> Mitte (0,60 / 0,17), Halbachsen (0,19 / 0,15) der Box; Abschnitt 3: Vorlage 2 x 418-441, y 193-211 in
# 412-467 x 190-263 -> Mitte (0,32 / 0,16), Halbachsen (0,21 / 0,12)), Halbachsen x KUPPE_SKAL (die gemessene Zone ist der gesaettigte
# Kern R-G >= 15 bzw. 25). Je Texel das kleinste d seiner Bildpunkte (wie augen_gewicht); Rand strukturfolgend. Lauf 1 (Kuppe als Halbraum
# ueber einer waagerechten Bildlinie): gerades Band ueber die ganze Kopfbreite (Rechteck-Kante) - verworfen.
KUPPE_ELL = {'135': (0.60, 0.17, 0.19, 0.15), '85': (0.32, 0.16, 0.21, 0.12)}
KUPPE_SKAL = 1.3                       # Lauf 2 mit 1,4: Kuppe 0,17 der Kopfpixel, reichte bis 0,33 der Kopfbox-Breite (Vorlage-Kern ab 0,42)
KUPPE_STRUKTUR = 0.35                  # Ellipsenabstand d: Gewicht rampe(1 - d + KUPPE_STRUKTUR x (Struktur - 1), KUPPE_RAMPE)
KUPPE_RAMPE = 0.25
RAND_W = 8.0                           # E.: Breite der Rampe am Zonenrand (13,2 / 11,9 E. je Bildpunkt an der Nase, Pose 135; Lauf 4 mit 15: Randgradient 2,5 < 3)
RAND_STRUKTUR = 200.0                  # E.: Rand folgt der Ref2-Kopfstruktur (Gauss RAND_SIGMA Texel); 200 x (rel - 1) ~ +-30 E. ~ 2-3 Texel (Lauf 2 mit
#                                        300: Kante Std 7,4 px, Haarflaeche ausgefranst; Riegel verlangt > 4 px)
RAND_SIGMA = 2.0
# Haaransatz (Saum): die ersten SAUM_W E. der Haarflaeche am Rand zur grauen Zone um SAUM_F dunkler - der Rand wird als Kante lesbar
# (Riegel T-K9b Randgradient 3-8 'wie Ref2-Hautstruktur'; Lauf 4-6 ohne Saum: 2,5-2,6, weil Haar und Grau als Texel nur ~3 L* auseinander
# liegen und die Haarseite erst durch das RDT-Licht dunkel wird). Der Saum folgt dem strukturfolgenden Rand (gleiches s_eff).
SAUM_W, SAUM_F = 25.0, 0.6             # Lauf 7 mit 0,7: Randgradient 2,9 (Grenze 3)
SAUM_BIS = 0.85                        # kein Saum unterhalb von SAUM_BIS der Kopfbox-Hoehe (Pose 135): das Haarfeld ist in Vorlage 1 'nach unten offen'
#                                        (Messblatt Abschnitt 2); Lauf 7 mit Saum bis zum Kinn: K8 Albedo 27,8 < 29,2
NASE_F3 = 0.9                          # Nase (Texel im Fenster KV3 auf der Grau-Seite): mitteldunkles Merkmal, Faktor auf das Grau-Soll
NASE_RAND = 60.0                       # E.: Breite des Nasenstreifens beidseits der Nasenebene (nur Grau-Seite wirksam)
HOF_F3, HOF_R3 = 0.5, 6.0              # Augenhof vorn: Faktor und Radius (Texel). Lauf 6 mit 0,65 / 5: K1 19,0. Runde 2 HOF_F 0,62 / HOF_R 8: K1-Kontrast 19,6 mit der grauen Umgebung
#                                        (Grenze 20); Lauf 5 mit 0,55 / 8: K1 22,6, aber der Hof senkte die graue Zone des Vorderbilds insgesamt
#                                        (Soll-L* P50 29,5 bei Lg 36,1 - 588 belegte graue Texel, davon ein grosser Teil im Hofradius 8)
HOF_F_S3, AUGE_KERN_S3 = 0.7, 0.10     # seitlich: Hof 0,7 (Runde 2 0,9; Lauf 3 mit 0,85: 0,038 x 0,030 H) und Kernschwelle 0,10 (Runde 2 0,15): S1 lag in Lauf 1/2 bei 0,034 x 0,034 H
#                                        (Soll 0,028-0,052 x 0,040-0,070 H) - die graue Umgebung ist heller als die rosa Haut der Runde 2, der Fleck
#                                        nach der Vergleichsregel kleiner
AB_GEW_GRAU = 0.4                      # dito grau (Lauf 4 mit 1,0: gewaehlt 27,9 bei Soll 30,3, s. GRAU_BASIS)
AB_GEW = 0.15                          # Gewicht von a*/b* bei der Eintragswahl in den Klassen braun / kuppe (Lab-Abstand): die Farbe gibt die Klasse,
#                                        die Helligkeit folgt der Struktur (Lauf 1/2 mit 1,0: die Klasse klemmte am Eintrag, der dem Klassen-Median
#                                        a*/b* am naechsten liegt - Haar einfarbig 88/72/64 bzw. 104/80/72)


def klassen_r3(A):
    """Eintraege der Zeile 480 (Ref2 auf Koerperflaechen, nicht violett) in den Zonenklassen grau / braun / kuppe (paarweise disjunkt)
    und hell (Auge, Teilmenge von grau)."""
    z = ZEILE - A['cy']
    used = np.unique(A['tex'][A['zeile'] == ZEILE])
    used = used[A['clut'][z][used] != 0]
    lab = G.lab(A['rgb'][z][used].astype(float))
    nv = nicht_violett(lab)
    L, a, b = lab[:, 0], lab[:, 1], lab[:, 2]
    C = np.hypot(a, b)
    grau = nv & (C <= GRAU_C) & (L >= GRAU_L0)
    braun = nv & ~grau & (a > 0) & (b > 0) & (b >= BRAUN_BA * a) & (L >= BRAUN_L[0]) & (L <= BRAUN_L[1]) & (C <= BRAUN_C)
    kuppe = nv & ~grau & ~braun & (a >= KUPPE_A[0]) & (a <= KUPPE_A[1]) & (b >= KUPPE_BA * a) & (L >= KUPPE_L[0]) & (L <= KUPPE_L[1]) & (C > BRAUN_C)
    hell = grau & (L >= HELL_L) & (C <= HELL_C)
    K = dict(grau=used[grau], braun=used[braun], kuppe=used[kuppe], hell=used[hell])
    return K, used, int((~nv).sum())


def tex3d(blob, kp):
    """Je Texel der Kopfbilder (vorn / seite) der Ort im Kopfraum (Mesh 9 mesh-lokal, E.), aus den UV-Flaechen von Mesh 9 der gebauten EMD
    baryzentrisch gerastert (Vierecke als zwei Dreiecke 0-1-2 / 1-3-2). Texel ohne Flaeche: NaN (fuellen mit dem naechsten belegten
    Texel in fuellen())."""
    d = EA.emd_dir(blob); m9 = EA.md1_parse(blob[d[7]:d[8]])[9]
    pb = 128 * (kp['page'] & 0xF)
    shapes = {a: (kp[a]['v'][1] - kp[a]['v'][0] + 1, kp[a]['u'][1] - kp[a]['u'][0] + 1) for a in ('vorn', 'seite')}
    P3 = {a: np.full(shapes[a] + (3,), np.nan) for a in shapes}
    V = np.array([v[:3] for v in m9.tv], float)

    def ansicht(uv):
        u = uv[0] - pb
        for a in shapes:
            if kp[a]['u'][0] <= u <= kp[a]['u'][1]:
                return a
        return None

    def tri(uvs, vids, a):
        u0, v0 = kp[a]['u'][0], kp[a]['v'][0]
        pts = np.array([(u - pb - u0, v - v0) for u, v in uvs], float)
        xs = np.arange(int(np.floor(pts[:, 0].min())), int(np.ceil(pts[:, 0].max())) + 1)
        ys = np.arange(int(np.floor(pts[:, 1].min())), int(np.ceil(pts[:, 1].max())) + 1)
        if not len(xs) or not len(ys):
            return
        X, Y = np.meshgrid(xs + 0.5, ys + 0.5)
        (x0, y0), (x1, y1), (x2, y2) = pts
        den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
        if abs(den) < 1e-9:
            return
        l0 = ((y1 - y2) * (X - x2) + (x2 - x1) * (Y - y2)) / den; l1 = ((y2 - y0) * (X - x2) + (x0 - x2) * (Y - y2)) / den; l2 = 1 - l0 - l1
        ins = (l0 >= -0.02) & (l1 >= -0.02) & (l2 >= -0.02)
        for yy, xx, a0, a1, a2 in zip(Y[ins] - 0.5, X[ins] - 0.5, l0[ins], l1[ins], l2[ins]):
            yi, xi = int(yy), int(xx)
            if 0 <= yi < shapes[a][0] and 0 <= xi < shapes[a][1]:
                P3[a][yi, xi] = a0 * V[vids[0]] + a1 * V[vids[1]] + a2 * V[vids[2]]
    for f, uv in zip(m9.tf, m9.tuv):
        a = ansicht(uv)
        if a:
            tri([(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])], [f[1], f[3], f[5]], a)
    for f, uv in zip(m9.qf, m9.quv):
        a = ansicht(uv)
        if a:
            tri([(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])], [f[1], f[3], f[5]], a)
            tri([(uv[3], uv[4]), (uv[9], uv[10]), (uv[6], uv[7])], [f[3], f[7], f[5]], a)
    return P3


def fuellen(q):
    fin = np.isfinite(q[..., 0])
    _d, (iy, ix) = nd.distance_transform_edt(~fin, return_indices=True)
    return q[iy, ix], fin


def _jacobi(pose, q_loc, R9, T9):
    """Bild-Jacobi (2 x 3) der Spielkamera am Kopfpunkt q_loc (mesh-lokal): Sehstrahl, Bild-oben und Bild-rechts im Kopfraum."""
    def proj(q):
        return BF.v4_proj(pose, (np.atleast_2d(q) @ R9.T + T9))[0]
    p0 = proj(q_loc); J = np.zeros((2, 3))
    for k in range(3):
        e = np.zeros(3); e[k] = 1.0
        J[:, k] = (proj(q_loc + e) - proj(q_loc - e)) / 2.0
    ray = np.linalg.svd(J)[2][2]
    kam_l = R9.T @ (BF.v4_kamerapunkt(pose) - T9)
    if np.dot(ray, q_loc - kam_l) < 0:
        ray = -ray
    up = np.linalg.pinv(J) @ np.array([0.0, -1.0]); up /= np.linalg.norm(up)
    right = np.linalg.pinv(J) @ np.array([1.0, 0.0]); right /= np.linalg.norm(right)
    return p0, J, ray, up, right


def geometrie_r3(mo, kp, P, blob):
    """Zonengeometrie: je Texel (vorn / seite) der Seitenabstand s (E., > 0 = Bild-rechts der Nasenebene in Pose 135). Die Nasenebene geht
    durch die Mitte des Messblatt-Fensters KV3 (Pose 135) und enthaelt Sehstrahl und Bild-oben dieser Kamera (senkrechte Bildlinie durch die
    Nase). Rueckgabe dict(vorn=dict(s, fin, belegt), seite=..., info=...)."""
    P3 = tex3d(blob, kp)
    pose = P['135']
    kf = BF._v4_kf(mo, pose); R, T = mo.pose(kf); R9 = R[9].astype(float) / 4096.0; T9 = T[9].astype(float)
    ids, bild, uu, vv = texel_lage(mo, kp, pose)
    lc = np.full(ids.shape + (3,), np.nan)
    for b_, a in ((0, 'vorn'), (1, 'seite')):
        ys, xs = np.nonzero(bild == b_)
        lc[ys, xs] = P3[a][vv[ys, xs], uu[ys, xs]]
    kb = G.kopfbox(ids)
    bx = G.fenster(kb, RV.FENSTER['KV3'])
    sub = lc[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1].reshape(-1, 3)
    q_nase = np.median(sub[np.isfinite(sub[:, 0])], 0)
    p_n, J, ray, up, right = _jacobi(pose, q_nase, R9, T9)
    n_s = np.cross(up, ray); n_s /= np.linalg.norm(n_s)
    if (J @ n_s)[0] < 0:
        n_s = -n_s
    # Seitenbestimmung: lokale z-Koordinate (Mesh 9: z > 0 = (W)-links, KOPF_V4) der Kopfpixel links / rechts der Kopfmitte
    fin = np.isfinite(lc[..., 0]); X = np.arange(ids.shape[1])[None, :]
    xm = 0.5 * (kb[0] + kb[2])
    z_links = float(np.median(lc[..., 2][fin & (X < xm)])); z_rechts = float(np.median(lc[..., 2][fin & (X >= xm)]))
    out = dict(info=dict(q_nase=q_nase.round(1).tolist(), nase_bild=p_n.round(1).tolist(), kv3_mitte=[0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])],
                         n_seite=n_s.round(4).tolist(), e_je_px_nase=[round(1.0 / np.linalg.norm(J[:, 0]), 2), round(1.0 / np.linalg.norm(J[:, 2]), 2)],
                         kopfbox135=list(kb), z_lokal_bild_links=round(z_links, 1), z_lokal_bild_rechts=round(z_rechts, 1)))
    for a in ('vorn', 'seite'):
        q, fin_a = fuellen(P3[a])
        out[a] = dict(s=(q - q_nase) @ n_s, fin=fin_a, belegt=int(fin_a.sum()))
    return out


def kuppe_gewicht(mo, kp, P, shapes):
    """Kuppe als Ellipsenabstand je Texel beider Kopfbilder: Ellipse KUPPE_ELL (Anteile der Kopfbox) im Bild der Pose 135 bzw. 85, Halbachsen
    x KUPPE_SKAL; je Bildpunkt d (1 = Rand), je Texel das kleinste d seiner Bildpunkte (Texel ohne Bildpunkt: 9)."""
    d_tex = [np.full(sh, 9.0) for sh in shapes]
    for pose_n in ('135', '85'):
        ids, bild, uu, vv = texel_lage(mo, kp, P[pose_n])
        kb = G.kopfbox(ids); w, h = kb[2] - kb[0], kb[3] - kb[1]
        cx_, cy_, ax_, ay_ = KUPPE_ELL[pose_n]
        cx, cy, ax, ay = kb[0] + cx_ * w, kb[1] + cy_ * h, ax_ * w * KUPPE_SKAL, ay_ * h * KUPPE_SKAL
        yy, xx = np.nonzero(bild >= 0)
        d = np.sqrt(((xx + 0.5 - cx) / ax) ** 2 + ((yy + 0.5 - cy) / ay) ** 2)
        for b_ in (0, 1):
            m = bild[yy, xx] == b_
            np.minimum.at(d_tex[b_], (vv[yy, xx][m], uu[yy, xx][m]), d[m])
    return d_tex


def rampe(x, w):
    return np.clip(0.5 + x / (2.0 * w), 0.0, 1.0)


def zonen_r3(geo, src, ansicht, d_kuppe, w_auge):
    """Zonengewichte je Texel: wk (Kuppe, aus dem Ellipsenabstand d_kuppe), wh (Haar, aus dem Seitenabstand s), Rest grau; Rand
    strukturfolgend (Seite: RAND_STRUKTUR x (Ref2-Kopfstruktur - 1) in E.; Kuppe: KUPPE_STRUKTUR x (Struktur - 1) in d). Das Auge
    (w_auge > 0) bleibt grau."""
    g = geo[ansicht]
    L = src[..., 0]; rel = L / max(float(L.mean()), 1e-6)
    rel_g = cv2.GaussianBlur(rel.astype(np.float32), (0, 0), RAND_SIGMA).astype(float)
    st = rel_g - 1.0
    s_eff = g['s'] + RAND_STRUKTUR * st
    wh = rampe(s_eff, RAND_W)
    wk = rampe(1.0 - d_kuppe + KUPPE_STRUKTUR * st, KUPPE_RAMPE)
    wk[w_auge > 0] = 0.0; wh[w_auge > 0] = 0.0
    wh = wh * (1.0 - wk)
    saum = rampe(SAUM_W - s_eff, RAND_W) * rampe(s_eff, RAND_W)       # 0 .. SAUM_W E. hinter dem Rand, innerhalb der Haarflaeche
    return wh, wk, rel, saum


def kopf_bild_r3(A, K, src, w, geo, d_kuppe, ansicht, zn=None, nase_m=None, unten_m=None, w_eigen=None):
    """Texelbild einer Kopf-Ansicht, Runde 3: Grau-Zone = GRAU_BASIS x oertliche Struktur der Ref2-Kopftexel (Zeile 485, Knie) mit reduzierter Buntheit ->
    naechster grauer Eintrag; Haarzone = HAAR_REL x Grau-Soll -> naechster brauner Eintrag; Kuppe = KUPPE_REL x Grau-Soll -> naechster
    rotbrauner Eintrag; Auge = Kern hell/unbunt mit Hof (Runde 2); Nase = Grau-Soll x NASE_F3 im Fenster KV3 (Grau-Seite)."""
    pal = A['rgb'][ZEILE - A['cy']].astype(float)
    sh = src.shape[:2]
    wh, wk, rel, saum = zonen_r3(geo, src, ansicht, d_kuppe, w)
    # Augenzone = Augengewicht (K4 / S2) und die Augenzone der Runde-5-Zonenkarte (zn, wie Runde 2; Lauf 1-5 ohne zn: S1 0,034 H hoch, Soll
    # >= 0,040): bleibt grau, ohne Saum. Kein Saum im unteren Kopf (unten_m, s. SAUM_BIS).
    auge = (w > 0) | (zn if zn is not None else False)
    wh[auge] = 0.0; wk[auge] = 0.0; saum[auge] = 0.0
    if unten_m is not None:
        saum[unten_m] = 0.0
    L, a, b = src[..., 0], src[..., 1], src[..., 2]
    # oertliche Struktur statt absoluter Ref2-Helligkeit (s. GRAU_BASIS), Lichter mit Knie
    rel_l = np.clip(L / np.maximum(cv2.GaussianBlur(L.astype(np.float32), (0, 0), STRUKTUR_SIGMA).astype(float), 1.0), STRUKTUR_MIN, STRUKTUR_MAX)
    rel_k = np.where(rel_l <= 1.0, rel_l, 1.0 + (rel_l - 1.0) * KNIE_F)
    Lg = np.minimum(GRAU_BASIS * rel_k, HAUT_L2 - 0.5)
    if nase_m is not None:
        nm = nase_m & (geo[ansicht]['s'] < 0) & (geo[ansicht]['s'] > -NASE_RAND)
        Lg = np.where(nm, Lg * NASE_F3, Lg)
    lab_b = G.lab(pal[K['braun']]); lab_k = G.lab(pal[K['kuppe']])
    soll_g = np.stack([Lg, GRAU_AB * a, GRAU_AB * b], -1)
    soll_h = np.stack([Lg * HAAR_REL, np.full(sh, float(np.median(lab_b[:, 1]))), np.full(sh, float(np.median(lab_b[:, 2])))], -1)
    soll_k = np.stack([np.minimum(Lg * KUPPE_REL, KUPPE_L[1] - 0.5), np.full(sh, float(np.median(lab_k[:, 1]))), np.full(sh, float(np.median(lab_k[:, 2])))], -1)
    wg = 1.0 - wh - wk
    soll = soll_g * wg[..., None] + soll_h * wh[..., None] + soll_k * wk[..., None]
    soll[..., 0] *= 1.0 - (1.0 - SAUM_F) * saum * (wh >= 0.5)         # Haaransatz (s. SAUM_W)
    # Auge (Runde 2): Kern hell und unbunt, Rand nach der Feinstruktur, Hof um den Kern (K1-Kontrast) - nur in der Grau-Zone
    w2 = np.clip(w + 0.6 * (cv2.GaussianBlur(rel.astype(np.float32), (0, 0), 1.0) - 1.0), 0, 1); w2[~auge] = 0
    kern = nd.binary_fill_holes(nd.binary_closing(auge & (w2 >= (AUGE_KERN if ansicht == 'vorn' else AUGE_KERN_S3)))) & auge
    kern = nd.binary_opening(kern) | (kern & (w2 >= 0.9))
    hl = G.lab(pal[K['hell']]); e = hl[np.argmax(hl[:, 0])]
    # Hof nur um den Kern des Auges der EIGENEN Vorlage-Kamera (w_eigen: vorn Pose 135, seitlich Pose 85). Das Seitenauge (Pose 85) liegt zum
    # Teil auf Vorderbild-Texeln, die Pose 135 am Kinn zeigt (Lauf 7/8: 19 von 74 KV4-Bildpunkten mit Augengewicht) - sein Hof senkte dort die
    # Albedo (K8 27,8 < 29,2).
    kern_hof = kern & (w_eigen > 0) if w_eigen is not None else kern
    if kern_hof.any():
        dk_ = nd.distance_transform_edt(~kern_hof)
        fak = 1.0 - (1.0 - (HOF_F3 if ansicht == 'vorn' else HOF_F_S3)) * np.exp(-(dk_ / (HOF_R3 if ansicht == 'vorn' else HOF_R)) ** 2)
        grau_m = (wh < 0.5) & (wk < 0.5)
        soll[..., 0] = np.where(kern | ~grau_m, soll[..., 0], np.minimum(soll[..., 0], np.maximum(soll[..., 0] * fak, HOF_MIN)))
    soll[kern] = e[None, :]
    kl = np.full(sh, 'grau', object)
    kl[wh >= 0.5] = 'braun'; kl[wk >= 0.5] = 'kuppe'; kl[kern] = 'hell'
    out = np.zeros(sh, np.uint8)
    for k in set(kl.ravel()):
        sel = kl == k
        ein = K[k]
        lab_e = G.lab(pal[ein])
        gew = np.array([1.0, AB_GEW, AB_GEW]) if k in ('braun', 'kuppe') else (np.array([1.0, AB_GEW_GRAU, AB_GEW_GRAU]) if k == 'grau' else np.ones(3))
        d = (((soll[sel][:, None, :] - lab_e[None, :, :]) ** 2) * gew).sum(-1)
        out[sel] = ein[np.argmin(d, 1)].astype(np.uint8)
        if os.environ.get('RT_DEBUG'):
            print('DEBUG', ansicht, k, 'n', int(sel.sum()), 'soll L P10/50/90', np.percentile(soll[sel][:, 0], (10, 50, 90)).round(1), 'Lg P50', round(float(np.median(Lg[sel])), 1),
                  'gewaehlt L P50', round(float(np.median(G.lab(pal[out[sel]])[:, 0])), 1), 'src L P50', round(float(np.median(L[sel])), 1),
                  'wg/wh/wk P50', np.median(wg[sel]).round(2), np.median(wh[sel]).round(2), np.median(wk[sel]).round(2), 'rel_k P50', np.median(rel_k[sel]).round(3),
                  'belegt', int(geo[ansicht]['fin'][sel].sum()), 'soll L P50 belegt', round(float(np.median(soll[sel & geo[ansicht]['fin']][:, 0])), 1) if (sel & geo[ansicht]['fin']).any() else None,
                  'Lg P50 belegt', round(float(np.median(Lg[sel & geo[ansicht]['fin']])), 1) if (sel & geo[ansicht]['fin']).any() else None)
    zone = np.where(kl == 'braun', 2, np.where(kl == 'kuppe', 3, 1)).astype(np.uint8)
    return out, zone, dict(wh=wh, wk=wk, kern=kern)


def zonen_render(mo, kp, pose, zmaps):
    """Zone je Bildpunkt (Kopf Id 9): 1 grau, 2 Haar, 3 Kuppe; Kenn-Render ohne Licht (Zeile 480 als Kenn-CLUT, R = Index)."""
    tex0, lut0 = mo.tex, mo.lut
    try:
        t = mo.tex.copy(); pb = 128 * (kp['page'] & 0xF)
        for name, ans in (('vorn', kp['vorn']), ('seite', kp['seite'])):
            u0, u1 = ans['u']; v0, v1 = ans['v']
            t[v0:v1 + 1, pb + u0:pb + u1 + 1] = zmaps[name].astype(np.uint8)
        mo.tex = t
        l = mo.lut.copy(); z = ZEILE - mo.clut_y
        l[z, :, 0] = np.arange(256); l[z, :, 1] = 0; l[z, :, 2] = 0; l[z, :, 3] = 255
        mo.lut = l
        img, ids = G.zeichne(mo, pose, licht=False)
    finally:
        mo.tex, mo.lut = tex0, lut0
    return np.where(ids == 9, img[..., 0].astype(int), -1), ids


def main():
    """Runde 3: Kopf nach der Nutzerbeschreibung (Zonen grau / braun / Kuppe aus der Kopfgeometrie, Farben aus den Ref2-Klassen);
    Schwanz texelgleich Runde 2 (schwanz_textur_r2)."""
    A = atlas()
    K, used, n_viol = klassen_r3(A)
    K2, _u, haar_l, _n = klassen_r2(A)
    P = G.posen()
    zk = zonenkarte()
    blob, _info = bauen()
    mo = b2.modell(blob)
    form = BF._variante_v4(); kp, sw = form['kopf_v4'], form['schwanz_kette']
    shapes = {'vorn': zk['kopf_vorn'][0].shape, 'seite': zk['kopf_seite'][0].shape}
    src_v = quelle_kopf(A, shapes['vorn'], (0, 0))
    src_s = quelle_kopf(A, shapes['seite'], (20, 6))
    schwanz = schwanz_textur_r2(A, schwanz_spanne(mo, sw))
    alt = dict(np.load(AUS)) if os.path.exists(AUS) else {}
    if 'schwanz' in alt:
        assert (alt['schwanz'] == schwanz).all(), 'Schwanz muss texelgleich Runde 2 bleiben'
    geo = geometrie_r3(mo, kp, P, blob)
    (w_v, w_s), wp = augen_gewicht(mo, kp, P, (shapes['vorn'], shapes['seite']), je_pose=True)
    dk_v, dk_s = kuppe_gewicht(mo, kp, P, (shapes['vorn'], shapes['seite']))
    nv_ = fenster_texel(mo, kp, P, '135', 'KV3', 0, shapes['vorn'])
    namen = [z[0] for z in E.ZONEN]
    za_v = zk['kopf_vorn'][0] == namen.index('v_auge'); za_s = zk['kopf_seite'][0] == namen.index('s_auge')
    un_ = fenster_texel(mo, kp, P, '135', 'UNTEN', 0, shapes['vorn'])
    bv, zv, iv = kopf_bild_r3(A, K, src_v, w_v, geo, dk_v, 'vorn', za_v, nv_, un_, wp['135'][0])
    bs, zs, is_ = kopf_bild_r3(A, K, src_s, w_s, geo, dk_s, 'seite', za_s, w_eigen=wp['85'][1])
    setze(mo, kp, sw, dict(kopf_vorn=bv, kopf_seite=bs, schwanz=schwanz))
    pal = A['rgb'][ZEILE - A['cy']].astype(float)
    zonen_lab = {}
    for name, bild, zone in (('vorn', bv, zv), ('seite', bs, zs)):
        lab = G.lab(pal[bild])
        zonen_lab[name] = {zn: dict(texel=int((zone == zi).sum()), lab_p50=[round(float(x), 1) for x in np.median(lab[zone == zi], 0)]) if (zone == zi).any() else dict(texel=0)
                           for zn, zi in (('grau', 1), ('braun', 2), ('kuppe', 3))}
    zmap = {}
    for pose_n in ('135', '85'):
        zm, ids = zonen_render(mo, kp, P[pose_n], dict(vorn=zv, seite=zs))
        k = ids == 9
        zmap[pose_n] = {zn: round(float((zm[k] == zi).mean()), 3) for zn, zi in (('grau', 1), ('braun', 2), ('kuppe', 3))}
    import riegel_ref2tex as RR
    zk_ = dict(vorn=zv, seite=zs)
    zb_ = RR.zonen_bild(mo, kp, P, zk_)
    hf_ = dict((p_, (zb_[p_]['zm'] >= 2) & (zb_[p_]['ids'] == 9)) for p_ in ('135', '85'))
    N_ = RR._kopf_fenster(mo, P, hf_)
    Z, _i = RR.tk4(mo, P, hf_, N_)
    Z9, _i9 = RR.tk9(mo, P, kp, zb_, zk_, N_)
    ref = b2.modell(G.REF2)
    prot = dict(runde=3, quelle=dict(kopf=KOPF_QUELLE, kopf_zeile=KOPF_ZEILE_REF2, schwanz=SCHWANZ_QUELLE, zeile=ZEILE),
                klassen={k: [int(x) for x in v] for k, v in K.items()}, klassen_lab={k: [[round(float(c), 1) for c in r] for r in G.lab(pal[v])] for k, v in K.items()},
                eintraege_koerper=int(len(used)), violett_ausgenommen=n_viol, ableitungen=[],
                geometrie=geo['info'], belegt={a: geo[a]['belegt'] for a in ('vorn', 'seite')},
                zonen_texel=zonen_lab, zonen_bild=zmap,
                port_wahl=dict(GRAU_C=GRAU_C, GRAU_L0=GRAU_L0, BRAUN_L=BRAUN_L, BRAUN_C=BRAUN_C, BRAUN_BA=BRAUN_BA, KUPPE_A=KUPPE_A, KUPPE_L=KUPPE_L,
                               KUPPE_BA=KUPPE_BA, HELL_C=HELL_C, HAAR_REL=HAAR_REL, KUPPE_REL=KUPPE_REL, GRAU_AB=GRAU_AB, KUPPE_ELL=KUPPE_ELL,
                               KUPPE_SKAL=KUPPE_SKAL, KUPPE_STRUKTUR=KUPPE_STRUKTUR, KUPPE_RAMPE=KUPPE_RAMPE, RAND_W=RAND_W, RAND_STRUKTUR=RAND_STRUKTUR, RAND_SIGMA=RAND_SIGMA,
                               NASE_F3=NASE_F3, NASE_RAND=NASE_RAND, SAUM_W=SAUM_W, SAUM_F=SAUM_F, SAUM_BIS=SAUM_BIS, GRAU_BASIS=GRAU_BASIS, STRUKTUR_SIGMA=STRUKTUR_SIGMA, STRUKTUR_MIN=STRUKTUR_MIN,
                               STRUKTUR_MAX=STRUKTUR_MAX, KNIE_F=KNIE_F, HAUT_L2=HAUT_L2,
                               HOF_F3=HOF_F3, HOF_R3=HOF_R3, HOF_F_S3=HOF_F_S3, AUGE_KERN_S3=AUGE_KERN_S3, AB_GEW=AB_GEW, AB_GEW_GRAU=AB_GEW_GRAU, HOF_R=HOF_R, HOF_MIN=HOF_MIN, HOF_F_S=HOF_F_S, AUGE_KERN=AUGE_KERN, AUGE_KERN_S=AUGE_KERN_S),
                ref2_kopf={p: kopf_mass(ref, P, p) for p in ('135', '85')}, ist_kopf={p: kopf_mass(mo, P, p) for p in ('135', '85')},
                riegel=[dict(mass=z['mass'], ist=z['ist'], ok=z['ok']) for z in Z + Z9])
    for z in Z + Z9:
        print('INFO' if z.get('info') else ('OK  ' if z['ok'] else 'ROT '), z['mass'], '| soll:', z['soll'], '| ist:', z['ist'])
    print('Zonen Texel', json.dumps(zonen_lab), '\nZonen Bild', zmap)
    np.savez(AUS, kopf_vorn=bv, kopf_seite=bs, schwanz=schwanz, zonen_vorn=zv, zonen_seite=zs)
    os.makedirs(os.path.dirname(PROT), exist_ok=True)
    json.dump(prot, open(PROT, 'w'), indent=1)
    print('geschrieben', AUS)

if __name__ == '__main__':
    main()
