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


def augen_gewicht(mo, kp, P, shapes):
    """Augenfleck als Gewicht je Texel beider Kopfbilder, gesetzt im Bild der jeweiligen Vorlage-Kamera an der Riegel-Stelle (Messblatt V4
    Abschnitt 7): vorn (Pose 135) K4 0,050 H (B)-links der Kopfmitte, 0,093 H unter der Kopf-Oberkante, Groesse K3 0,026 x 0,033 H;
    seitlich (Pose 85) S2 0,049 H hinter der Kopf-Vorderkante, 0,79 H ueber der Sohle, Groesse S1 0,040 x 0,055 H. Gewicht je Bildpunkt
    clip(AUGE_PROFIL - d, 0, 1) mit d = Ellipsenabstand (1 = Sollrand); je Texel das groesste Gewicht seiner Bildpunkte. Ein Texel des
    Vorderbilds kann so zum Seitenauge gehoeren (die vordersten Flaechen an der Kopfkante tragen in Pose 85 das Vorderbild)."""
    w = [np.zeros(sh) for sh in shapes]
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
    return w


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
    """Texel des Kopfbilds bild_nr (0 vorn, 1 seitlich), die die Pose im Messblatt-Fenster zeigt (geschlossen, ohne Loecher)."""
    ids, bild, uu, vv = texel_lage(mo, kp, P[pose])
    bx = G.fenster(G.kopfbox(ids), RV.FENSTER[fenster])
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


def main():
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


if __name__ == '__main__':
    main()
