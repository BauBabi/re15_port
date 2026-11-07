"""SF V4 Originaltexturen (Bauer, Runde 1 von 2): Kopf- und Schwanztexel der Sichel-Form V4 aus dem Atlas der gelobten Referenz 2.

    python ref2tex_v4.py [--runden 6]   -> tools/r41_sichel/v4/farben_ref2tex_v4.npz (+ Protokoll SF_belege/v4/ref2tex_protokoll.json)

Vorgabe (Orchestrator 2026-11-07 nach dem Nutzer "Bei der Sichel Form V4, warum sind denn da die Farben so extrem scheisse? ... die Form
baut von Farben und Texturen her ja nun wirklich auf die Urspruengliche Birkin Form auf"; ersetzt Messblatt V4 Abschnitt 5):
  (1) Koerper = Referenz 2 unveraendert (birkin_formen.py V4_TEXTUR 'ref2': keine Koerperzeilen/-texel, CLUT bytegleich).
  (2) Schwanz: Texel aus der KOERPERHAUT desselben Atlas (Oberarm, Mesh 13, Zeile 480), UV entlang der Achse durchgehend (uv art 'achse'
      aus V4 Runde 5: v = Bogenlaenge, u = Lage um den Umfang). Keine neue Farbe.
  (3) Kopf: Texel nur mit Farben der Referenz-2-Zeile 480, und zwar nur mit Eintraegen, die Referenz 2 auf ihren Koerper-Meshes in Zeile 480
      tatsaechlich zeigt; rote Kopfoberseite aus den roten Wundeintraegen, Auge aus den hellen, Haare aus den dunklen. Feinstruktur = die
      Original-Kopftexel von Referenz 2 (TPage 128 u 66-126 v 4-90, dieselben Indizes, Helligkeit in Zeile 480), keine Videostruktur.
      Orte der Merkmale: die Zonenkarte des Kopfes aus V4 Runde 5 (farben_v4.npz, Index = 1 + Zone x 16 + Stufe; die Zonen sind die
      Messblatt-Fenster KV1-KV9 / KS1-KS6 und die Augenorte K4/S2), weil Kopfform und Kopf-UV seit Runde 5 unveraendert sind (Vorgabe 4).
      Helligkeit je Zone: so, dass die Kopf-Riegel K1-K10 / S1-S4 (Messblatt V4 Abschnitt 7, Ziele Abschnitt 2/3) gruen sind; die
      Zonenhelligkeit wird nach der Vergleichsregel (Abschnitt 5, nur als Messverfahren) nachgefuehrt, gewaehlt wird immer ein vorhandener
      Eintrag der Zeile 480 (naechster in Lab), nie eine neue Farbe.
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
    kern = nd.binary_fill_holes(nd.binary_closing(auge & (w2 >= AUGE_KERN))) & auge     # Kern ohne Loecher und Einzeltexel
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


def main():
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


if __name__ == '__main__':
    main()
