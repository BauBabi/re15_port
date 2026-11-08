"""SF V4 (Bauer): Farben EXAKT - Eichung der Texel/CLUT der Sichel-Form V4 an den Zielen des Messblatts V4 (Abschnitt 5).

    python eichen_v4.py [--runden 4]      -> tools/r41_sichel/v4/farben_v4.npz (+ eichung_v4.json Protokoll)

Verfahren (Messblatt V4 Abschnitt 5 "Umrechnung auf Texel", hier fuer alle Regionen gemeinsam geloest):
  Schirmfarbe = Texel x Licht (b2-Rasterer = Spielrechnung: rgb = Texel x Farbe / 255, Farbe = RDT-Licht Cut 12 je Ecke, Gouraud).
  1. Code-Render: dieselbe Pose mit einer Kenn-CLUT (R = Index, G = Zeile, ohne Licht) -> je Bildpunkt (Zeile, Index) des Texels.
  2. Licht: dieselbe Pose mit dem weissen Pruefkoerper (alle Texel 248, RDT-Licht) -> lambda = W / 248 je Bildpunkt und Kanal.
  3. Vergleichsregel linear: Mittel einer Region R im Videoraum = a + b x Sum_p w_R(p) x Texel(p) x lambda(p) + Hintergrund,
     w_R = Gauss_1,5(Maske_R) / |R| (der Gauss ist symmetrisch). Damit ist jedes Regionsmittel linear in den Texelfarben.
  4. Kopf (Zeile 486) und Schwanz (Zeile 487): je Zone eine Farbe (16 Helligkeitsstufen fuer die Hautstruktur), Zonen = die Fenster
     und Flecken des Messblatts; die Zonenfarben werden per kleinsten Quadraten so geloest, dass die Regionsmittel die Ziele treffen.
     Koerper (Zeilen 480-482, Texel unveraendert): je Zeile Faktoren R,G,B und ein Helligkeitsexponent (Kontrast) fuer die Haut,
     eigene Faktoren fuer die blauen Eintraege (Jeans), geloest gegen F6-F10, F14, F19 (scipy least_squares in Lab, Grenzen).
  5. Pruefung mit riegel_v4.py (Bauer und Pruefer gleich). Die Runden wiederholen 1-4 mit den neuen Masken.
  Runde 2 (Pruefung V4 r1): Koerperfarben kommen aus r2_koerper_farben.py (farben_koerper_v4.npz ersetzt im Generator die Zeilen
  480/481/482/484 und die Koerpertexel; die Hautfaktoren 'haut' hier wirken nur noch auf die Zeilen 483/485); Augenpunkt = KOPF_V4 auge;
  Auge seitlich als Ellipse im Bild (S2/S1) mit Lichtausgleich je Texel; Haarvorhang-Texel im Vorderbild -> seitliche Haarzone;
  Schwanz in 6 Baendern (Wurzel, Aussenkante, Mitte, senkrecht, Unterseite, Endstueck) mit den Achsenpunkten T9 als Regionen.
Zonen Kopf vorn/seitlich (Anteile der Port-Kopfbox = Bbox Id 9, x von (B)-links, y von oben; Messblatt Abschnitt 2/3/7):
  Fenster KV1-KV5, KS1-KS3 wie p_riegelfenster.py; zusaetzliche Zonen KV6-KV8, KS4-KS6 mit Zielwerten aus Vorlage 1 (entblendet) bzw.
  Vorlage 2, gemessen mit v4_gemein.fenster/mittel auf der Original-Kopfbox (Ausgabe dieses Skripts, Protokoll eichung_v4.json).
  Auge vorn: Mitte 0,050 H (B)-links der Kopfmitte, 0,093 H unter der Oberkante, 0,026 x 0,033 H (K3/K4); Auge seitlich: 0,049 H hinter
  der Vorderkante, 0,79 H ueber der Sohle, 0,040 x 0,055 H (S1/S2); Haarkante seitlich vom Augenrand bis zur Vorderkante (S3).
Hautstruktur: Helligkeit des Wundfleischs (TPage 128 u 66-126 v 4-90, Zeile 480, wie V3), auf +-STRUKTUR normiert; STRUKTUR = mittlere
relative Standardabweichung der Leuchtdichte in den Originalfenstern KV1-KV5/KS1-KS3 (gemessen hier, Protokoll).
"""
import os, sys, json, argparse, struct
import numpy as np, cv2
from scipy import ndimage as nd
from scipy.optimize import least_squares
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import birkin_formen as BF
import emd_ansichtsblatt as EA
import riegel_v4 as RV

AUS = os.path.join(HIER, 'farben_v4.npz')
PROT = os.path.join(HIER, 'eichung_v4.json')
ARB = os.path.join(G.SFV4, 'eichung')
STUFEN = 16

# Zonen: (Name, Ansicht, Ziel-Lab im Videoraum, Quelle)
ZONEN = [
    ('v_basis', 'vorn', 'KV9', 'Vorlage 1 Fenster KV9 (0,12-0,52 x 0,32-0,62) - Gesichtsmitte zwischen Auge und Haarfeld'),
    ('v_oben_links', 'vorn', 'KV6', 'Vorlage 1 Fenster KV6 (0,0-0,40 x 0,0-0,32), grau-mauve links der roten Zone (Abschnitt 2)'),
    ('v_kuppe', 'vorn', 'KV1', 'Messblatt K9 / F1: 22,6 / 15,4 / 5,2'),
    ('v_rechts', 'vorn', 'KV8', 'Vorlage 1 Fenster KV8 (0,85-1,0 x 0,25-0,75), rechts neben dem Haarfeld'),
    ('v_unter_auge', 'vorn', 'KV5', 'Messblatt K2 (KV5): 18,9 / 7,1 / 0,6'),
    ('v_kinn', 'vorn', 'KV4', 'Messblatt K8 / F5: 9,9 / 9,9 / 3,8'),
    ('v_haar', 'vorn', 'KV2', 'Messblatt K5 / F3: 13,3 / 7,4 / 1,5'),
    ('v_nase', 'vorn', 'KV3', 'Messblatt K7 / F4: 18,2 / 7,1 / 1,3'),
    ('v_auge', 'vorn', 'AUGE_V', 'Messblatt K2 / F2: 46,9 / 7,2 / -1,1'),
    ('s_basis', 'seite', 'KS4', 'Vorlage 2 Fenster KS4 (0,11-0,70 x 0,30-0,55) - Kopf seitlich Mitte'),
    ('s_kuppe', 'seite', 'KS1', 'Messblatt S4 / F11: 27,6 / 15,3 / 3,3'),
    ('s_hinten_unten', 'seite', 'KS5', 'Vorlage 2 Fenster KS5 (0,0-0,44 x 0,55-0,82)'),
    ('s_unten', 'seite', 'KS3', 'Messblatt Abschnitt 3 KS3: 16,5 / 2,2 / 0,5'),
    ('s_haar', 'seite', 'KS2', 'Messblatt S3 / F13: 19,1 / 1,2 / 1,1'),
    ('s_auge', 'seite', 'AUGE_S', 'Messblatt S1 / F12: 48,4 / 4,0 / 0,5'),
]
# Schwanz-Zeile 487 (Messblatt T8); V4 Runde 2: Klassen 'mitte' (Ansatz + Biegung) und 'senkrecht' (bis zum Knick) fuer die Achse
# (Sicht-Pruefung V4 r1 1.3), Ziele T9O / T9S = Mittel der 5x5-Fenster an den Achsenpunkten (310,243)+(292,262) bzw. (289,280)+(288,300)
# der Vorlage 2 (Leser A, pruefung_sicht/messen_sicht.py ACHSE), roh
BAENDER = [('wurzel', 'F15'), ('aussen', 'F17'), ('mitte', 'T9O'), ('senkrecht', 'T9S'), ('unten', 'F16'), ('ende', 'F18')]
FENSTER_ZUS = dict(KV6=(0.0, 0.0, 0.40, 0.32), KV7=(0.0, 0.32, 0.12, 0.62), KV8=(0.85, 0.25, 1.0, 0.75), KV9=(0.12, 0.32, 0.52, 0.62),
                   KS3=(0.44, 0.82, 0.89, 0.99), KS4=(0.11, 0.30, 0.70, 0.55), KS5=(0.0, 0.55, 0.44, 0.82), KS6=(0.53, 0.04, 1.0, 0.30))


def ziele_original():
    """Ziel-Lab je Fenster im Videoraum, an den Originalen gemessen (gleiches Verfahren wie p_riegelfenster.py)."""
    z = {}
    for k, a in list(RV.FENSTER.items()) + list(FENSTER_ZUS.items()):
        b = 1 if k.startswith('KV') else 2
        z[k] = [round(float(x), 1) for x in RV.lab1(G.mittel(G.vorlage(b), G.fenster(G.KB_ORIG[b], a)))]
    # Messblatt-Werte haben Vorrang, wo das Blatt sie nennt (Abschnitt 5 / 7)
    z.update(KV1=list(RV.ZIEL['kuppe_v']), KV2=list(RV.ZIEL['haar_v']), KV3=list(RV.ZIEL['nase']), KV4=list(RV.ZIEL['kinn']),
             KV5=list(RV.ZIEL['kv5']), KS1=list(RV.ZIEL['kuppe_s']), KS2=list(RV.ZIEL['haar_s']), KS3=[16.5, 2.2, 0.5],
             AUGE_V=list(RV.ZIEL['auge_v']), AUGE_S=list(RV.ZIEL['auge_s']))
    return z


def struktur_amplitude():
    """Feinstruktur der Originalhaut: relative Std der Leuchtdichte nach Abzug eines Gauss sigma 3 (Hochpass, ohne die Zonenkanten)
    in den Originalfenstern KV1-KV5, KS1-KS3; Mittel. amp = 2 x Std (die 16 Stufen bilden +-2 Std der Struktur auf +-amp ab)."""
    w = []
    for k, a in RV.FENSTER.items():
        b = 1 if k.startswith('KV') else 2
        x0, y0, x1, y1 = G.fenster(G.KB_ORIG[b], a)
        L = G.lum(G.vorlage(b))
        hp = L - cv2.GaussianBlur(L.astype(np.float32), (0, 0), 3.0)
        w.append(float(hp[y0:y1 + 1, x0:x1 + 1].std() / max(L[y0:y1 + 1, x0:x1 + 1].mean(), 1.0)))
    return 2.0 * float(np.mean(w)), w


def lab_rgb(L):
    return np.array(BF._lab_rgb(*L), float)


# ---------------------------------------------------------------------------------------------------- Bau mit Zwischenstand
def bauen(npz):
    BF.V4_FARBEN = npz
    BF.BAU_INFO.clear()
    blob = BF.baue(BF.lade_em30(), BF._variante_v4())
    info = dict(BF.BAU_INFO['kopf_v4'])
    os.makedirs(os.path.join(G.SFV4, 'emd'), exist_ok=True)
    return blob, info


def code_und_licht(mo, pose):
    """Je Bildpunkt (Zeile, Index) des Texels und lambda (RGB) - Schritte 1/2."""
    lut0 = mo.lut
    code = lut0.copy()
    for z in range(code.shape[0]):
        code[z, :, 0] = np.arange(256); code[z, :, 1] = z; code[z, :, 2] = 7
    mo.lut = code
    try:
        img_c, ids = G.zeichne(mo, pose, licht=False)
    finally:
        mo.lut = lut0
    img_w, _ = G.zeichne(mo, pose, weiss=True)
    img, _ = G.zeichne(mo, pose)
    am = ids >= 0
    idx = np.where(am, img_c[..., 0].astype(int), -1)
    zeile = np.where(am, img_c[..., 1].astype(int), -1)
    lam = img_w.astype(float) / 248.0
    return dict(idx=idx, zeile=zeile, ids=ids, lam=lam, img=img.astype(float))


def gewicht(maske):
    w = cv2.GaussianBlur(maske.astype(np.float32), (0, 0), G.SIGMA)
    return w / max(float(maske.sum()), 1.0)


# ---------------------------------------------------------------------------------------------------- Zonen
def zonen_karte(kb, H, ok_so, ansicht, shape=(480, 640), auge_seite=None):
    """Zonenlabel je Bildpunkt (Indizes in ZONEN) fuer die Kopfbox kb einer Ansicht; Prioritaet in Listenreihenfolge."""
    x0, y0, x1, y1 = kb
    yy, xx = np.mgrid[:shape[0], :shape[1]].astype(float)
    fx = (xx - x0) / max(x1 - x0, 1); fy = (yy - y0) / max(y1 - y0, 1)
    lab = np.full(shape, -1, int)
    name = {z[0]: i for i, z in enumerate(ZONEN)}

    def setz(n, m):
        lab[m] = name[n]
    if ansicht == 'vorn':
        setz('v_basis', np.ones(shape, bool))
        setz('v_oben_links', (fx < 0.40) & (fy < 0.32))
        setz('v_kuppe', (fx >= 0.40) & (fx < 0.80) & (fy < 0.32))
        setz('v_rechts', (fx >= 0.80) & (fy >= 0.20) & (fy < 0.80))
        setz('v_unter_auge', (fx < 0.40) & (fy >= 0.62) & (fy < 0.88))
        setz('v_kinn', (fx >= 0.15) & (fx < 0.70) & (fy >= 0.86))
        setz('v_haar', (fx >= 0.52) & (fx < 0.90) & (fy >= 0.25) & (fy < 0.75))   # bis 0,90: K6-Spiegelstelle ganz im Haarfeld
        setz('v_nase', (fx >= 0.44) & (fx < 0.62) & (fy >= 0.66) & (fy < 0.81))
        # Auge: aus 3D (auge_3d / texel_3d), dieselbe Stelle fuer vorn und seitlich
    else:
        ok, so = ok_so
        setz('s_basis', np.ones(shape, bool))
        setz('s_kuppe', (fx < 0.60) & (fy < 0.30))
        setz('s_hinten_unten', (fx < 0.44) & (fy >= 0.55))
        setz('s_unten', (fx >= 0.44) & (fy >= 0.82))
        # Auge: Projektion des gemeinsamen 3D-Augenpunkts (auge_3d) in die Seitenansicht; Halbachsen wie die Augenzone (AUGE_ACHSEN)
        cx, cy, ax, ay = auge_seite
        # Haarkante (S3): dunkler Streifen vor dem Auge = vom Augenrand bis zur Vorderkante, ueber die Augenhoehe und das Fenster KS2
        haar = (xx + 0.5 >= cx + ax) & (((yy >= cy - ay) & (yy <= cy + ay)) | ((fy >= 0.52) & (fy < 0.80)))
        setz('s_haar', haar)
        setz('s_auge', ((xx + 0.5 - cx) / ax) ** 2 + ((yy + 0.5 - cy) / ay) ** 2 <= 1.0)   # Runde 2: Auge seitlich im Bild (S1/S2)
    return lab


def texel_zonen(mo, info, ansicht, karte, kp):
    """Zonenlabel je Texel der Flaeche einer Ansicht (Texel -> Bildpunkt ueber die Abbildung aus kopf_v4_form_uv)."""
    a = info[ansicht]
    u0, u1 = kp[ansicht]['u']; v0, v1 = kp[ansicht]['v']
    vv, uu = np.mgrid[v0:v1 + 1, u0:u1 + 1].astype(float)
    x = a['x0'] + (uu + 0.5 - a['ou']) / a['s']; y = a['y0'] + (vv + 0.5 - a['ov']) / a['s']
    # Zonenkanten gestreut (Bayer-4x4 Versatz +-1,5 Bildpunkte): gemalter, unregelmaessiger Uebergang statt einer geraden Texelkante;
    # die Texelfarben bleiben Zonenfarben (kein Weichzeichner in der Textur, Riegel M3)
    bayer = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]], float) / 15.0 - 0.5
    jx = 3.0 * bayer[(vv.astype(int) % 4), (uu.astype(int) % 4)]
    jy = 3.0 * bayer[((uu.astype(int) + 1) % 4), (vv.astype(int) % 4)]
    xi = np.clip(np.round(x + jx).astype(int), 0, 639); yi = np.clip(np.round(y + jy).astype(int), 0, 479)
    return karte[yi, xi]


# Augenzone (Kopfraum, Halbachsen x vorn / y Hoehe / z Breite in E.): Fleck K3 vorn 0,026 x 0,033 H (H 364: 9,5 x 12 px bei
# ~0,095 px/E. = 100 x 126 E.), S1 seitlich 0,040 x 0,055 H (H 264: 10,6 x 14,5 px bei ~0,069 px/E. = 153 x 210 E.); gemalt groesser, weil
# die Halbwert-Maske nach sigma 1,5 kleiner misst (Eichlauf 1 vorn: gemalt 1,0 -> gemessen 0,6). Nachgestellt im Dossier V4.
AUGE_ACHSEN = (88.0, 74.0, 88.0)   # Runde 2: x0,93 (K3 mit 95/80/95 und Breite x1,55: 0,036 x 0,044 H, Grenze 0,036 x 0,043). Runde 1: Eichlauf 6 mit y 95: Auge vorn durch eine Flaechennaht geteilt (K1); Lauf 11 mit x/z 75/70: Auge seitlich 0,015 x 0,023 H in 4 Teilen (S1)
# Augenpunkt: K4 0,050 H (B)-links der Kopfmitte (Messblatt-Mitte), 0,11 H unter der Oberkante (Toleranz 0,07-0,12) statt 0,093: nur
# so liegt derselbe Oberflaechenpunkt in der Seitenansicht auch im S2-Band (0,034 H hinter der Kante, 0,814 H ueber der Sohle; mit 0,093
# 0,83 H > 0,82; Pruefung im Dossier V4). Eichlauf 5 mit 0,11: S2 0,829 H -> 0,118 (gemessen K4 y 0,105 bei 0,11). Ein Auge fuer beide Ansichten (Messblatt Abschnitt 3: das sichtbare Auge ist das (W)-rechte).
# Eichlauf 4 mit 0,055: Auge vorn als schmaler Streifen am Kopfrand (streifender Blick), daher die Mitte 0,050.
AUGE_K4 = (0.05, 0.12)      # nur noch fuer auge_3d (Runde 1); Runde 2 nimmt KOPF_V4 auge
# Radialer Verlauf der Augen (Faktor 1 +- AUGE_RADIAL von der Mitte zum Rand): das Original-Auge hat nach der Weichzeichnung L*-Max 56,2 bei
# Mittel 46,9 (Messblatt K1/F2, Kontrast 29,5); nachgestellt an K1/S1 (Dossier V4 Runde 2)
AUGE_RADIAL = 0.40
# V4 Runde 3: vorn eigener Wert (Lauf r3 Struktur 8/9: mit der Kopfstruktur fiel der K1-Kontrast auf 18,3-19,4; vorn 0,55 -> 20,6 bei
# K2 dE00 1,3 und K3 0,030 x 0,038 H; seitlich mit 0,55 S1-Breite 0,027 < 0,028 H, daher seitlich 0,40 wie Runde 2). PORT-WAHL,
# Nutzer-Wortlaut "nicht so komische runde aufgemalte Augen" / "Die Augen sehen nicht mal ansatzweise aus wie im Original" (weicher
# heller Fleck mit dem Kontrast des Originals, Messblatt K1 29,5).
AUGE_RADIAL_VORN = float(os.environ.get('RE15_V4_RADIAL', '0.55'))
AUGE_RAND = 0.5               # Randbreite des Augenprofils seitlich (Anteil des Ellipsenradius); PORT-WAHL, nachgestellt an S1
AUGE_RAND_VORN = 0.8          # vorn spitzer (K1-Kontrast: mit 0,5 19,4 < 20, Lauf r2 Eichung 19); PORT-WAHL
AUGE_NAH = 360.0           # E.: Abstand zum Augenpunkt, bis zu dem Vorderbild-Texel in der Seitenellipse zum Auge zaehlen; 200 liess die Kante
                           # vor dem Auge aus (S1 0,038 H hoch), 280 ebenso, 360 = S1 0,042 H (Laeufe r2 Eichung 10-11); PORT-WAHL
# Messblatt V4 S2: Augenmitte 0,049 H hinter der Vorderkante, 0,79 H ueber der Sohle. Gemalt wird 0,045 H (nicht 0,049): die gemessene
# Mitte des Flecks liegt nach Licht und Weichzeichnung hinter der gemalten (Runde 2: gemalt 0,045 -> gemessen S2 0,058 H, mit 0,049
# laege sie bei ~0,062 und der Abstand zur Haarkante S3 wuerde groesser). PORT-WAHL (Pruefung V4 r2 Zahlen 5: Abweichung erklaert).
S2_MITTE = (0.045, 0.79)
# V4 Runde 3 (S3): Vorderkante der gemalten Seitenellipse nach vorn - versucht mit 0,006 / 0,010 H (Laeufe r3 S3 1/2): der gemessene
# Fleck rueckt nicht vor (Abstand bleibt 0,015 H, S2 0,057), weil er an der Naht zu den Vorderbild-Flaechen endet (Dossier V4 Runde 2
# Offen 1), und die Vorderbild-Texel in der groesseren Ellipse senkten K1 auf 16,7-18,7. Daher 0 (aus); S3 bleibt offen.
AUGE_S_VOR = float(os.environ.get('RE15_V4_SVOR', '0.0'))
AUGE_S_GEMALT = 1.40         # gemalt groesser als S1, weil die Halbwert-Maske nach sigma 1,5 kleiner misst (Runde 1 vorn: gemalt 1,0 ->
                             # gemessen 0,6); nachgestellt an S1 (Dossier V4 Runde 2)
# Struktur halbiert in den Zonen, deren helle Stufen im Eichlauf 8 eigene Komponenten ueber der Halbwertschwelle von K1/S1 bildeten
# (Code-Render: vorn (333,178) s_basis lambda 0,98; seitlich (480,210) s_haar lambda 1,0); das Original hat dort genau 1 Komponente.
STRUKTUR_ZONE = dict(s_basis=0.5, s_haar=0.5, v_oben_links=0.5, s_hinten_unten=0.5,
                     # Runde 2: Augen ohne Struktur - das Original-Auge ist ein weicher heller Fleck (Messblatt Abschnitt 2/3, kein Ring, keine
                     # Pupille aufloesbar); mit voller Struktur zerfiel das Seitenauge in 2 Komponenten (Lauf r2 Eichung 1); vorn halbe Struktur (ohne: K1-Kontrast 17,8 < 20, Lauf r2 Eichung 2)
                     v_auge=0.5, s_auge=0.0)


# V4 Runde 3 (O4): Struktur aus dem Original. STRUKTUR_SIGMA = Hochpass wie struktur_amplitude (Gauss sigma 3 px, Runde 1);
# STRUKTUR_GAIN = Verstaerkung im Log-Raum, weil das Video schon weich ist und der Vergleich den Port noch einmal mit sigma 1,5
# weichzeichnet (Vergleichsregel): nachgestellt am Riegel K11 (Lum-Gradient im Kopf, Ziel aus dem Original), Laeufe r3 Struktur 1-n.
STRUKTUR_SIGMA = 3.0
STRUKTUR_GAIN = float(os.environ.get('RE15_V4_GAIN', '10.0'))
STRUKTUR_GAIN_FARBE = float(os.environ.get('RE15_V4_GAINF', '1.5'))       # Farbton (rot/grau-Flecken des Originals), PORT-WAHL,
                              # Nutzer-Wortlaut "Ich will ... auch EXAKT seine Farben" (Laeufe r3 Struktur 16/17)
# Klammer des Farbtons (log, je Kanal nach Abzug der Helligkeit): V4 Runde 4 gemessen (Pruefung V4 r3 Zahlen 5, "ohne Quelle"): die
# Farbton-Abweichung des Originals x 1,5 hat in der Kopfbox vorn p99 0,30 (max 0,95), seitlich p99 0,15 (max 0,48); 0,4 schneidet nur die
# Ausreisser ueber p99 (Kopfkontur gegen den Hintergrund) - Lauf r3 Struktur 17 zeigte ohne diese Grenze tuerkise Flecken.
STRUKTUR_FARBE_KLAMMER = 0.4
# Klammer der Helligkeit (log Faktor >= -STRUKTUR_KLAMMER): 2,0 = Faktor >= 0,135. Die dunklen Kanten zum Hintergrund in den Ecken der
# Original-Kopfbox ergeben x 10 bis -7,5 (p1 vorn); Laeufe r3 Struktur 1-n (Dossier V4 Runde 3 "Klammer -2,0").
STRUKTUR_KLAMMER = float(os.environ.get('RE15_V4_KLAMMER', '2.0'))
STRUKTUR_HELL = float(os.environ.get('RE15_V4_HELL', '0.10'))   # log Faktor <= STRUKTUR_HELL: helle Flecken ueber der Halbwertschwelle
                              # des Auges bildeten eigene Komponenten (Lauf r3 Struktur 2, Gain 3,5 ohne Grenze: K1 2, S1 3 Komponenten);
                              # V4 Runde 4: 0,15 -> 0,10 (Lauf r4 Eichung final: mit den dunklen Flecken im Hof wurde ein heller Fleck neben
                              # dem Auge zur zweiten K1-Komponente; Lauf r4 Eichung k mit 0,10: 1 Komponente, Kontrast 22,9, K11 38,5)
# Exponent je Zone (1 = volle Originalstruktur). Wie Runde 2 (STRUKTUR_ZONE): halbe Struktur in den Zonen, deren helle Stufen neben
# dem Auge eigene Komponenten ueber der Halbwertschwelle von K1/S1 bildeten; Runde 3 dazu v_basis (Lauf r3 Struktur 6/7: Komponenten
# bei (338,181) und (370,164) in v_oben_links / v_basis, seitlich (455,220) in s_basis, je 4-15 px, L* 41,6-45 bei Schwelle 40-43)
# (348,205): Zone v_nase Stufe 15 = (218,131,145) - der rosa Fleck am Mund der Sicht-Pruefung V4 r2 1.1 (Kinn/Maulzone); die Nasenzone
# ist hell (Zonenfarbe ~(160,94,104)), weil nur wenige Nasentexel im Fenster KV3 liegen.
# Wert 0,0 (Pruefung V4 r3 Zahlen 5: der Kommentar sagte "halbe Struktur"): Exponent 0 auf die HELLEN Stufen = in diesen 6 Zonen keine
# helle Struktur, die dunklen Flecken bleiben voll (Lauf r3 Struktur 6/7: mit 0,5 blieben die Komponenten neben dem Auge)
STRUKTUR_ZONE_R3 = {k: float(os.environ.get('RE15_V4_ZEXP', '0.0')) for k in ('s_basis', 's_haar', 'v_oben_links', 's_hinten_unten',
                                                                            'v_basis')}
STRUKTUR_ZONE_R3['v_nase'] = 0.0


# Augenhof (Lauf r3 Struktur 3/4: die Struktur hob den Median 21 x 21 um das Auge vorn auf 33,9 L* (Original 26,7, Messblatt K1), der
# Kontrast fiel unter 20 und helle Flecken neben dem Auge wurden eigene Komponenten): bis AUGE_HOF_1 x Augenradius im Bild keine helle
# Struktur, bis AUGE_HOF_2 x Radius linear auf volle Struktur. PORT-WAHL, nachgestellt an K1/S1 (Laeufe r3 Struktur 5-n), Nutzer-Wortlaut
# "Die Augen sehen nicht mal ansatzweise aus wie im Original" (das Original-Auge ist ein einzelner heller Fleck, Messblatt K1 1 Komponente).
AUGE_HOF_1 = float(os.environ.get('RE15_V4_HOF1', '2.0'))
AUGE_HOF_2 = float(os.environ.get('RE15_V4_HOF2', '4.0'))
# Hof dunkler als die Zone: Median 21 x 21 um das Auge vorn im Port 36 L* auf den Kopfpixeln (Original 26,7, Messblatt K1), Lauf r3
# Struktur 10; Faktor im Hof (Original-Ring um den hellen Fleck). PORT-WAHL, nachgestellt an K1, Nutzer-Wortlaut wie AUGE_HOF_1.
AUGE_HOF_DUNKEL = float(os.environ.get('RE15_V4_HOFD', '0.5'))


HOF_DUNKELSTRUKTUR = float(os.environ.get('RE15_V4_HOFSTR', '1.0'))   # V4 Runde 4: Anteil der dunklen Originalflecken im Hof (1 = voll);
                              # Nutzer-Wortlaut "Die Augen sehen nicht mal ansatzweise aus wie im Original", Sicht-Pruefung V4 r3 1.1
AUGE_HOF_DUNKEL_S = float(os.environ.get('RE15_V4_HOFDS', '0.6'))   # seitlich (Lauf r3 Struktur 15: mit 0,45 auch seitlich wurde die
                              # Kopfoberseite seitlich zur hellsten Stelle, S1/S2 fanden dort den Fleck)


# V4 Runde 4 (Pruefung V4 r3 Sicht 1.3, Klasse A "Vierkant-Balken": Deck- und Seitenflaeche als Plateaus mit harter Kante, Querprofil-
# Stufe auf 3 px 16,6-19,9 L*, Original 5,4-9,6 mit stetigem Abfall von hell oben nach dunkel unten): Schwanzbaender mit Verlauf ueber die
# Lage c um den Umfang (c = Normale . Richtung weg von der Bogenmitte, birkin_formen.kette_meshes_v4). c-Bereich je Band = die
# Klassengrenzen des Generators (SCHWANZ_V4 uv grenze 0,5). Aussenkante und Unterseite einfarbig; die Seitenbaender (mitte, senkrecht)
# laufen von der Farbe der Aussenkante (c = +0,5) ueber ihre eigene Farbe (c = 0) zur Farbe der Unterseite (c = -0,5) - an jeder
# Klassengrenze haben beide Flaechen dieselbe Farbe. Die eigene Farbe loest die Eichung (Achsenpunkte T9 wie bisher).
SCHWANZ_VERLAUF = dict(aussen=(1.0, 0.5), mitte=(0.5, -0.5), senkrecht=(0.5, -0.5), unten=(-0.5, -1.0))


# Breite der Uebergangszone (in c) beiderseits jeder Klassengrenze. V4 Runde 4: ein durchgehender Verlauf ueber den ganzen Umfang
# (Laeufe r4 Eichung c-p) liess sich nicht stabil an T8/T9 eichen (Mitte (22,0,0), Aussenkante rot oder F17 4,9-9,6 L*); die Klassenfarben
# der Runde 3 (T8/T9 bestanden) bleiben, nur an den Grenzen laeuft die Farbe ueber UEBERGANG je Seite auf die Mitte beider Farben - die
# harte Kante (Querprofil-Stufe 16,6-19,9 L* auf 3 px, Sicht-Pruefung V4 r3 1.3) wird zum Verlauf ueber ~2 Flaechen. PORT-WAHL (Breite),
# Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung" (Vorlage 2: runder Schwanz ohne Kanten).
UEBERGANG = 0.3


def verlauf_mischung(bn, s):
    """Anteile [(Band, Gewicht), ...] der Stufe s eines Verlaufsbands: eigene Farbe, an den Klassengrenzen linear bis 50/50 mit dem
    Nachbarband (Aussenkante <-> Seite <-> Unterseite; Nachbar der Aussenkante/Unterseite ist 'mitte'). Linear in den Bandfarben
    (loese_zonen_misch); Lauf r4 Eichung a: ein Faktor relativ zur Bandfarbe der vorigen Runde schaukelte sich auf."""
    c_hi, c_lo = SCHWANZ_VERLAUF[bn]
    c = c_hi - (c_hi - c_lo) * s / (STUFEN - 1)
    out = {bn: 1.0}
    oben = 'mitte' if bn in ('aussen', 'unten') else 'aussen'
    unten = 'mitte' if bn in ('aussen', 'unten') else 'unten'
    if bn != 'aussen' and c_hi - c < UEBERGANG:          # obere Grenze (zur Aussenkante hin)
        w = 0.5 * (1.0 - (c_hi - c) / UEBERGANG); out[bn] -= w; out[oben] = out.get(oben, 0.0) + w
    if bn != 'unten' and c - c_lo < UEBERGANG:           # untere Grenze (zur Unterseite hin)
        w = 0.5 * (1.0 - (c - c_lo) / UEBERGANG); out[bn] -= w; out[unten] = out.get(unten, 0.0) + w
    return out


def loese_zonen_misch(daten, n_zonen, zeile, misch, regl_ziel, regl_gewicht=None):
    """Wie loese_zonen, aber jeder CLUT-Index ist eine Mischung der Zonenfarben: Farbe(i) = Sum_z misch[i, z] x c_z (RGB je Zone)."""
    from scipy.optimize import lsq_linear
    zeilen, rhs = [], []
    for cl, regs in daten:
        idx, zl, lam, img = cl['idx'], cl['zeile'], cl['lam'], cl['img']
        sel = zl == zeile
        ii = np.clip(idx, 0, 255)
        konst_bild = np.where(sel[..., None], 0.0, img)
        for name, m, ziel_lab, wgt in regs:
            if m.sum() < 2:
                continue
            w = gewicht(m)
            konst = (w[..., None] * konst_bild).reshape(-1, 3).sum(0)
            q = sel & (w > 0)
            A = np.zeros((n_zonen, 3))
            if q.any():
                mq = misch[ii[q]]                                     # Pixel x Zonen
                A = (mq[:, :, None] * (w[q][:, None] * lam[q])[:, None, :]).sum(0)
            p_ziel = (lab_rgb(ziel_lab) - G.EA_) / G.EB_
            for c in range(3):
                row = np.zeros(n_zonen * 3); row[c::3] = A[:, c]
                zeilen.append(row * wgt); rhs.append((p_ziel[c] - konst[c]) * wgt)
    for zz in range(n_zonen):
        rg_ = 0.15 if regl_gewicht is None else regl_gewicht[zz]
        for c in range(3):
            row = np.zeros(n_zonen * 3); row[zz * 3 + c] = rg_
            zeilen.append(row); rhs.append(rg_ * regl_ziel[zz][c])
    r = lsq_linear(np.array(zeilen), np.array(rhs), bounds=(0.0, 248.0))
    return r.x.reshape(n_zonen, 3)


def struktur_original(info, ansicht, kp, kb_port, bild, auge=None, hof_dunkel=None, hof_struktur=None):
    """Je Texel der Flaeche einer Ansicht: log der relativen Farbe des Originals (Vorlage 1 entblendet / Vorlage 2) an der Stelle mit
    denselben Anteilen der Original-Kopfbox (G.KB_ORIG) wie der Texel in der Port-Kopfbox kb_port. Rueckgabe (v, u, 3)."""
    o = G.vorlage(bild).astype(np.float32)
    tief = cv2.GaussianBlur(o, (0, 0), STRUKTUR_SIGMA)
    rel = np.log(np.maximum(o, 4.0) / np.maximum(tief, 4.0))
    x, y = texel_bild(info, ansicht, kp)
    x0, y0, x1, y1 = kb_port
    fx = (x - x0) / max(x1 - x0, 1); fy = (y - y0) / max(y1 - y0, 1)
    X0, Y0, X1, Y1 = G.KB_ORIG[bild]
    xo = np.clip(np.round(X0 + fx * (X1 - X0)).astype(int), 0, 639); yo = np.clip(np.round(Y0 + fy * (Y1 - Y0)).astype(int), 0, 479)
    # Helligkeit (Mittel der log-Kanaele) und Farbton (Rest) getrennt: je Kanal geklammert ergab die Klammer -2 auf R allein tuerkise
    # Flecken (Lauf r3 Struktur 17, Bild 'Kopf 135'); der Farbton bekommt STRUKTUR_GAIN_FARBE und +-STRUKTUR_FARBE_KLAMMER
    q = rel[yo, xo]
    hell = q.mean(-1, keepdims=True)
    r = np.clip(STRUKTUR_GAIN * hell, -STRUKTUR_KLAMMER, STRUKTUR_HELL) +         np.clip(STRUKTUR_GAIN_FARBE * (q - hell), -STRUKTUR_FARBE_KLAMMER, STRUKTUR_FARBE_KLAMMER)
    if auge is not None:                      # (cx, cy, rx, ry) im Bild der Ansicht
        d = np.sqrt(((x - auge[0]) / auge[2]) ** 2 + ((y - auge[1]) / auge[3]) ** 2)
        t = np.clip((d - AUGE_HOF_1) / (AUGE_HOF_2 - AUGE_HOF_1), 0.0, 1.0)[..., None]
        # V4 Runde 4 (Pruefung V4 r3 Sicht 1.1 Klasse A: im Spielbild h118/h135 eine glatte Kuppel, Innenflaeche Gradient 26,9/27,2 unter
        # der Originalspanne 29,0-40,4, glatte Pixel 1,7-2,7 x): der Hof reicht bis AUGE_HOF_2 x Augenradius (vorn ~20 px um das Auge =
        # fast die ganze Gesichtsflaeche) und war ohne jede Struktur. Jetzt behaelt der Hof die DUNKLEN Flecken des Originals (min(r, 0));
        # nur die hellen sind dort aus - sie bildeten die zweiten Komponenten ueber der K1/S1-Schwelle (Lauf r3 Struktur 3/4)
        r = r * t + (np.minimum(r, 0.0) * (HOF_DUNKELSTRUKTUR if hof_struktur is None else hof_struktur) + np.log(AUGE_HOF_DUNKEL if hof_dunkel is None else hof_dunkel)) * (1.0 - t)
    return r


def struktur_stufen(rel_v, rel_s):
    """16 Stufen = k-Mittel (log-RGB) ueber die Texel beider Ansichten; Rueckgabe Stufe je Texel und Faktor je Stufe (RGB, Mittel 1)."""
    X = np.vstack([rel_v.reshape(-1, 3), rel_s.reshape(-1, 3)])
    X = X - X.mean(0)
    rng = np.random.default_rng(41)
    C = X[rng.choice(len(X), STUFEN, replace=False)].copy()
    for _ in range(30):
        k = ((X[:, None, :] - C[None]) ** 2).sum(2).argmin(1)
        for j in range(STUFEN):
            if (k == j).any():
                C[j] = X[k == j].mean(0)
    def zu(rel):
        R = rel.reshape(-1, 3) - (np.vstack([rel_v.reshape(-1, 3), rel_s.reshape(-1, 3)]).mean(0))
        return ((R[:, None, :] - C[None]) ** 2).sum(2).argmin(1).reshape(rel.shape[:2])
    return zu(rel_v), zu(rel_s), np.exp(C)


def auge_3d(mo, kb, H, P):
    import kopf3d_v4 as K
    xm = 0.5 * (kb[0] + kb[2])
    t = K.treffer(mo, P['135'], (xm - AUGE_K4[0] * H, kb[1] + AUGE_K4[1] * H))
    return t[1]


def texel_3d(mo, kp):
    """Je Texel der Kopfflaechen (vorn / seitlich) der Kopfraum-Punkt (Rasterung der Mesh-9-Flaechen im UV-Raum), sonst NaN."""
    m = mo.meshes[9]
    out = {}
    for ans in ('vorn', 'seite'):
        u0, u1 = kp[ans]['u']; v0, v1 = kp[ans]['v']
        out[ans] = np.full((v1 - v0 + 1, u1 - u0 + 1, 3), np.nan)
    def dreiecke():
        for f, uv in zip(m.tf, m.tuv):
            yield [m.tv[f[2 * k + 1]][:3] for k in range(3)], [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])], uv[2] >> 6
        for f, uv in zip(m.qf, m.quv):
            P4 = [m.qv[f[2 * k + 1]][:3] for k in range(4)]
            U4 = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])]
            for a, b, c in ((0, 1, 2), (1, 3, 2)):
                yield [P4[a], P4[b], P4[c]], [U4[a], U4[b], U4[c]], uv[2] >> 6
    for P3, U, zeile in dreiecke():
        if zeile != kp['clut']:
            continue
        P3 = np.array(P3, float); U = np.array(U, float)
        for ans in ('vorn', 'seite'):
            u0, u1 = kp[ans]['u']; v0, v1 = kp[ans]['v']
            if not (u0 - 1 <= U[:, 0].mean() <= u1 + 1 and v0 - 1 <= U[:, 1].mean() <= v1 + 1):
                continue
            a = (U[1, 0] - U[0, 0]) * (U[2, 1] - U[0, 1]) - (U[2, 0] - U[0, 0]) * (U[1, 1] - U[0, 1])
            if abs(a) < 1e-9:
                continue
            for vv in range(int(np.floor(U[:, 1].min())), int(np.ceil(U[:, 1].max())) + 1):
                for uu in range(int(np.floor(U[:, 0].min())), int(np.ceil(U[:, 0].max())) + 1):
                    x, y = uu + 0.5, vv + 0.5
                    w1 = ((x - U[0, 0]) * (U[2, 1] - U[0, 1]) - (U[2, 0] - U[0, 0]) * (y - U[0, 1])) / a
                    w2 = ((U[1, 0] - U[0, 0]) * (y - U[0, 1]) - (x - U[0, 0]) * (U[1, 1] - U[0, 1])) / a
                    w0 = 1 - w1 - w2
                    if min(w0, w1, w2) < -0.15 or not (u0 <= uu <= u1 and v0 <= vv <= v1):
                        continue
                    out[ans][vv - v0, uu - u0] = w0 * P3[0] + w1 * P3[1] + w2 * P3[2]
    return out


def texel_bild(info, ansicht, kp):
    """Bildpunkt (x, y) je Texel der Flaeche einer Ansicht (Abbildung aus kopf_v4_form_uv, ohne Streuung)."""
    a = info[ansicht]
    u0, u1 = kp[ansicht]['u']; v0, v1 = kp[ansicht]['v']
    vv, uu = np.mgrid[v0:v1 + 1, u0:u1 + 1].astype(float)
    return a['x0'] + (uu + 0.5 - a['ou']) / a['s'], a['y0'] + (vv + 0.5 - a['ov']) / a['s']


def struktur_feld(mo, shape, amp):
    """Hautstruktur je Texel (Faktor um 1, +-amp): Leuchtdichte des Wundfleischs TPage 128 u 66-126 v 4-90 Zeile 480 (wie V3)."""
    q = G.lum(mo.pal[0][mo.tex[4:90, 66:126]].astype(float))
    q = (q - q.mean()) / max(q.std(), 1e-6)
    yy, xx = np.mgrid[0:shape[0], 0:shape[1]]
    n = q[(yy * 3) % q.shape[0], (xx * 2) % q.shape[1]]
    stufe = np.clip(np.round((n + 2.0) / 4.0 * (STUFEN - 1)), 0, STUFEN - 1).astype(int)
    fak = 1.0 + amp * (stufe - (STUFEN - 1) / 2.0) / ((STUFEN - 1) / 2.0)
    return stufe, fak


# ---------------------------------------------------------------------------------------------------- Loesen
def regionen(cl, rend, ansicht, ziel, P):
    """Regionen (Maske, Ziel-Lab, Gewicht) an einem Render (riegel_v4-Masken)."""
    R = []
    if ansicht == 'vorn':
        z, info, (img, ids, V, b) = RV.kopf_vorn(cl['mo'], P)
        kb = info['kopfbox']
        for k in ('KV1', 'KV2', 'KV3', 'KV4', 'KV5'):
            bx = G.fenster(kb, RV.FENSTER[k]); m = np.zeros((480, 640), bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
            R.append((k, m, ziel[k], 1.0))
        # KV6/KV8 wie die Originalfenster ohne Kopfmaske (die Originalwerte enthalten den Hintergrund in der Kopfbox-Ecke); KV7 nicht als
        # Region (das Original-Fenster x 368-374 schneidet dort das Auge an, im Port liegt das Auge anders: Eichlauf 4 trieb v_basis auf 0)
        for k in ('KV6', 'KV8', 'KV9'):
            bx = G.fenster(kb, FENSTER_ZUS[k]); m = np.zeros((480, 640), bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
            R.append((k, m, ziel[k], 0.5))
        R.append(('AUGE_V', b['maske'], ziel['AUGE_V'], 2.0))
    else:
        z, info, (img, ids, V, b) = RV.kopf_seite(cl['mo'], P)
        kb = info['kopfbox']
        for k in ('KS1', 'KS2'):
            bx = G.fenster(kb, RV.FENSTER[k]); m = np.zeros((480, 640), bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
            R.append((k, m, ziel[k], 1.0))
        for k in ('KS3', 'KS4', 'KS5', 'KS6'):
            bx = G.fenster(kb, FENSTER_ZUS[k]); m = np.zeros((480, 640), bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
            R.append((k, m, ziel[k], 0.5))
        R.append(('AUGE_S', b['maske'], ziel['AUGE_S'], 2.0))
    return R


def loese_zonen(daten, n_zonen, zeile, alt, fak_je_index, zone_je_index, regl_ziel):
    """Kleinste Quadrate im Videoraum-RGB: Regionsmittel = a + b x (konst + Sum_z A[z] x c_z), Unbekannte c_z (RGB je Zone).
    daten: Liste (code, regionen). regl_ziel: Startfarbe je Zone (Regularisierung, Gewicht klein)."""
    zeilen = []
    rhs = []
    for cl, regs in daten:
        idx, zl, lam, img = cl['idx'], cl['zeile'], cl['lam'], cl['img']
        sel = zl == zeile
        z_pix = np.where(sel, zone_je_index[np.clip(idx, 0, 255)], -1)
        fji = np.asarray(fak_je_index, float)
        fji = fji[:, None].repeat(3, 1) if fji.ndim == 1 else fji          # Runde 3: Faktor je Index RGB (Struktur des Originals)
        f_pix = np.where(sel[..., None], fji[np.clip(idx, 0, 255)], 0.0)
        konst_bild = np.where(sel[..., None], 0.0, img)
        for name, m, ziel_lab, wgt in regs:
            if m.sum() < 2:
                continue
            w = gewicht(m)
            konst = (w[..., None] * konst_bild).reshape(-1, 3).sum(0)
            A = np.zeros((n_zonen, 3))
            for zz in range(n_zonen):
                q = (z_pix == zz)
                if q.any():
                    A[zz] = (w[q][:, None] * lam[q] * f_pix[q]).sum(0)
            ziel_rgb = lab_rgb(ziel_lab)
            p_ziel = (ziel_rgb - G.EA_) / G.EB_
            for c in range(3):
                row = np.zeros(n_zonen * 3); row[c::3] = A[:, c]
                zeilen.append(row * wgt); rhs.append((p_ziel[c] - konst[c]) * wgt)
    # Regularisierung zur Startfarbe (Ziel / mittleres lambda der Zone): haelt Zonen, die keine Region allein traegt, bei ihrem Ziel
    for zz in range(n_zonen):
        for c in range(3):
            row = np.zeros(n_zonen * 3); row[zz * 3 + c] = 0.15
            zeilen.append(row); rhs.append(0.15 * regl_ziel[zz][c])
    A = np.array(zeilen); y = np.array(rhs)
    from scipy.optimize import lsq_linear
    r = lsq_linear(A, y, bounds=(0.0, 248.0))
    return r.x.reshape(n_zonen, 3)


def koerper_param(theta, basis, klassen):
    """Hautzeilen aus Parametern, je Zeile 480-482: Faktoren R,G,B und Exponent auf die relative Leuchtdichte fuer alle Eintraege
    (Reihenfolge der Helligkeiten und damit die Hautzeichnung bleiben erhalten), eigene Faktoren R,G,B fuer die blauen Eintraege
    (Jeans, b > r wie haut_zeile). Eichlauf 2 mit je Klasse (rot/blass) freien Faktoren kehrte die Zeichnung um (480 rot x2,5,
    blass x0,2: dunkle Haut mit weissen Punkten)."""
    neu = basis.copy()
    for z in range(3):
        t = theta[z * 7:(z + 1) * 7]
        L = G.lum(basis[z]); Lm = max(float(np.median(L[L > 0])), 1.0)
        f = (np.maximum(L, 1.0) / Lm) ** t[3]
        haut = klassen[z] != 2
        neu[z][haut] = np.clip(basis[z][haut] * np.exp(t[0:3]) * f[haut][:, None], 0, 248)
        bl = klassen[z] == 2
        neu[z][bl] = np.clip(basis[z][bl] * np.exp(t[4:7]), 0, 248)
    return neu


def klassen_haut(basis):
    kl = np.zeros(basis.shape[:2], int)
    r, g, b = basis[..., 0], basis[..., 1], basis[..., 2]
    kl[(r / np.maximum(g, 8)) >= 1.4] = 0          # rot
    kl[(r / np.maximum(g, 8)) < 1.4] = 1           # blass
    kl[b > r] = 2                                  # blau (Jeans, wie haut_zeile / cluts_bauen)
    return kl


def loese_koerper(daten, basis, klassen, theta0):
    """daten: Liste (code, Liste (Name, Maske, Ziel-Lab))."""
    vorb = []
    for cl, regs in daten:
        idx, zl, lam, img = cl['idx'], cl['zeile'], cl['lam'], cl['img']
        sel = (zl >= 0) & (zl <= 2)
        konst_bild = np.where(sel[..., None], 0.0, img)
        for name, m, ziel_lab in regs:
            if m.sum() < 3:
                continue
            w = gewicht(m)
            konst = (w[..., None] * konst_bild).reshape(-1, 3).sum(0)
            q = sel & (w > 1e-7)
            vorb.append((name, konst, zl[q], idx[q], (w[q][:, None] * lam[q]), np.array(ziel_lab, float)))

    def res(theta):
        neu = koerper_param(theta, basis, klassen)
        out = []
        for name, konst, zq, iq, wl, zlab in vorb:
            m = G.EA_ + G.EB_ * (konst + (wl * neu[zq, iq]).sum(0))
            la = RV.lab1(m)
            out.extend((la - zlab) * np.array([1.0, 0.7, 0.7]))
        out.extend(0.5 * np.asarray(theta))
        return np.array(out)
    # Grenzen (gegen Ausreisser-Texel: ein freier Exponent trieb Eintraege an 248 = weisse Punkte auf Schulter/Buckel, Eichlauf 1):
    # Faktor e^-1,6 .. e^1,0, Exponent -0,3 .. 0,3
    lo = np.tile([-1.6, -1.6, -1.6, -0.3, -1.6, -1.6, -1.6], 3); hi = np.tile([1.0, 1.0, 1.0, 0.6, 1.0, 1.0, 1.0], 3)
    r = least_squares(res, np.clip(theta0, lo + 1e-6, hi - 1e-6), bounds=(lo, hi), method='trf')
    return r.x


# ---------------------------------------------------------------------------------------------------- Hauptlauf
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--runden', type=int, default=4)
    ap.add_argument('--aus', default=AUS, help='Ziel der Farbdatei (Standard: farben_v4.npz im Baum)')
    ap.add_argument('--arb', default=ARB, help='Arbeitsordner (Versuchslaeufe parallel)')
    ap.add_argument('--prot', default=PROT)
    a = ap.parse_args()
    ARB_ = a.arb
    os.makedirs(ARB_, exist_ok=True)
    P = G.posen()
    ziel = ziele_original()
    amp, amp_einzeln = struktur_amplitude()
    prot = dict(ziele=ziel, struktur=dict(amp=amp, je_fenster=amp_einzeln), runden=[])
    kp, sw = BF.KOPF_V4, BF.SCHWANZ_V4
    g_ = sw['uv']['grenze']                    # V4 Runde 4: c-Bereiche der Verlaufsbaender = Klassengrenzen des Generators
    SCHWANZ_VERLAUF.update(aussen=(1.0, g_), mitte=(g_, -g_), senkrecht=(g_, -g_), unten=(-g_, -1.0))
    npz = os.path.join(ARB_, 'farben_runde.npz')
    if os.path.exists(npz):
        os.remove(npz)
    # Startfarben: Ziel im Port-Schirm / 0,6 (mittleres lambda der Kopf-/Schwanzflaechen im ersten Licht-Render)
    start_z = np.array([np.clip((lab_rgb(ziel[z[2]]) - G.EA_) / G.EB_ / 0.6, 0, 248) for z in ZONEN])
    start_b = np.array([np.clip((lab_rgb(RV.ziel(k)) - G.EA_) / G.EB_ / 0.6, 0, 248) for _n, k in BAENDER])
    ref2 = b2.modell(G.REF2)
    basis = np.array([[RV.np.array(c, float) for c in ref2.lut[z][:, :3]] for z in range(6)], float)
    klassen = klassen_haut(basis[:3])
    theta = np.zeros(21)
    zonen_c, baender_c = start_z.copy(), start_b.copy()
    for runde in range(a.runden + 1):
        # Zwischenstand schreiben: Texelbilder (Zone x 16 Stufen) + Zeilen
        blob, info = bauen(npz if os.path.exists(npz) else os.path.join(ARB_, 'nicht_da.npz'))
        mo = b2.modell(blob)
        vorn_r = G.zeichne(mo, P['135'], voll=True); seite_r = G.zeichne(mo, P['85'], voll=True)
        kv = G.kopfbox(vorn_r[1]); ks = G.kopfbox(seite_r[1])
        okv, sov, Hv = G.h_port(vorn_r[2]); oks, sos, Hs = G.h_port(seite_r[2])
        # Runde 2: Augenpunkt = KOPF_V4 auge (Bezug des Haarvorhangs, in beiden Kameras sichtbar); vorn als 3D-Zone wie Runde 1.
        # Seitlich als Ellipse im Bild der Vorlage-2-Kamera an der S2-Stelle mit der S1-Groesse (Runde 1: die 3D-Zone lag dort auf
        # streifend gesehenen Flaechen, 0,023 x 0,030 H in 5 Teilen, Riegel S1 rot): Mitte 0,049 H hinter der Box-Vorderkante,
        # 0,79 H ueber der Sohle (Messblatt S2), Halbachsen AUGE_S_GEMALT x S1 0,040 x 0,055 H / 2.
        import augenpunkt_v4 as AP
        pa = np.array(kp['auge'], float)
        auge_s = (ks[2] - S2_MITTE[0] * Hs, sos - S2_MITTE[1] * Hs, AUGE_S_GEMALT * 0.5 * 0.040 * Hs, AUGE_S_GEMALT * 0.5 * 0.055 * Hs)
        # V4 Runde 3 (S3 rot, Pruefung V4 r2 Zahlen 4.5: Abstand Haarkante -> Augenfleck 0,015 H statt <= 0,01 H): die Vorderkante der
        # gemalten Ellipse um 2 x AUGE_S_VOR nach vorn (Mitte +AUGE_S_VOR, Halbachse +AUGE_S_VOR), die Hinterkante bleibt
        auge_s = (auge_s[0] + AUGE_S_VOR * Hs, auge_s[1], auge_s[2] + AUGE_S_VOR * Hs, auge_s[3])
        karte_v = zonen_karte(kv, Hv, (okv, sov), 'vorn'); karte_s = zonen_karte(ks, Hs, (oks, sos), 'seite', auge_seite=auge_s)
        tz_v = texel_zonen(mo, info, 'vorn', karte_v, kp); tz_s = texel_zonen(mo, info, 'seite', karte_s, kp)
        t3 = texel_3d(mo, kp)
        nm = {z[0]: i for i, z in enumerate(ZONEN)}
        for ans, tz, zn in (('vorn', tz_v, 'v_auge'),):
            d = (t3[ans] - pa) / np.array(AUGE_ACHSEN)
            im_auge = np.nan_to_num((d ** 2).sum(2), nan=9.0) <= 1.0
            tz[im_auge] = nm[zn]
        # Runde 2: Vorderbild-Texel, die in der Seitenansicht in die Augenellipse fallen und nahe am Augenpunkt liegen (< AUGE_NAH E.),
        # gehoeren zum selben Auge: die vordersten Flaechen an der Kante tragen das Vorderbild, ohne sie zerfiel das Seitenauge (S1
        # 2 Komponenten, Lauf r2 Eichung 2)
        tv3 = t3['vorn'].reshape(-1, 3); ok3 = ~np.isnan(tv3[:, 0])
        if ok3.any():
            xs3, _w3 = AP.proj(mo, P['85'], tv3[ok3])
            cx, cy, ax, ay = auge_s
            inn = (((xs3[:, 0] - cx) / ax) ** 2 + ((xs3[:, 1] - cy) / ay) ** 2 <= 1.0) & (np.linalg.norm(tv3[ok3] - pa, axis=1) < AUGE_NAH)
            flat = tz_v.reshape(-1); idx_ok = np.nonzero(ok3)[0]
            flat[idx_ok[inn]] = nm['v_auge']
            # Runde 2: Vorderbild-Texel des Haarvorhangs ((W)-links der Mittelebene, Kopfraum z > -29), die in der Seitenansicht vor dem
            # Auge im Haarband liegen, bekommen die seitliche Haarzone (Sicht-Pruefung V4 r1 1.2: duenne dunkle Vorderkante; Lauf r2 Eichung 3:
            # helle 2-6-px-Reste an der Kante x 476-482, S1 2 Komponenten). In Vorlage 1 liegen sie im Haarfeld (K5/K6).
            kz = G.kopfbox(seite_r[1]); fy3 = (xs3[:, 1] - kz[1]) / max(kz[3] - kz[1], 1)
            # alle Zeilen vor dem Auge (Lauf r2 Eichung 5: mit nur dem Haarband blieb die Kante darueber hell, x 476-482 / y 201-217,
            # S1 2 Komponenten); im Vorderbild liegen diese Texel im Haarfeld
            haar3 = (xs3[:, 0] >= cx + ax) & (fy3 < 0.80)
            haar3 &= (tv3[ok3][:, 2] > -29.0) & ~inn
            flat[idx_ok[haar3]] = nm['s_haar']     # Farbe der seitlichen Haarkante (S3); mit 'v_haar' teilten sich KV2 und KS2 eine Farbe
                                                   # (Lauf r2 Eichung 4: K5 18,6 statt 13,3, K7 rot)
        st_v, f_v = struktur_feld(mo, tz_v.shape, amp); st_s, f_s = struktur_feld(mo, tz_s.shape, amp)
        # V4 Runde 3 (Pruefung V4 r2 Sicht 1.1 Klasse A, Entscheid O4): Hautstruktur des Kopfes aus dem ORIGINAL statt aus dem
        # Wundfleisch-Muster - je Texel die relative Farbe des Originals (Pixel / Gauss sigma STRUKTUR_SIGMA) an der Stelle, die dieselbe
        # Lage in der Original-Kopfbox hat (Anteile wie die Zonen), im Log-Raum mit STRUKTUR_GAIN verstaerkt, 16 RGB-Stufen (k-Mittel)
        _xa, _wa = AP.proj(mo, P['135'], pa[None, :])
        auge_v = (float(_xa[0, 0]), float(_xa[0, 1]), 0.5 * 0.026 * Hv, 0.5 * 0.033 * Hv)      # Messblatt K3 0,026 x 0,033 H
        rel_v = struktur_original(info, 'vorn', kp, kv, 1, auge_v)
        rel_s = struktur_original(info, 'seite', kp, ks, 2, (auge_s[0], auge_s[1], auge_s[2] / AUGE_S_GEMALT, auge_s[3] / AUGE_S_GEMALT),
                                 AUGE_HOF_DUNKEL_S, hof_struktur=0.0)   # Runde 4: seitlich wie Runde 3 (Lauf r4 Eichung a: mit
                                 # dunklen Flecken im Hof auch seitlich verlor das Seitenauge seine Helligkeit, S1/S2/S4 rot)
        st_v, st_s, stufen_rgb = struktur_stufen(rel_v, rel_s)
        # Runde 2: Augen mit radialem Verlauf statt Hautstruktur (heller Kern, dunklerer Rand): Stufe = (1 - Ellipsenabstand) x 15;
        # vorn aus dem 3D-Abstand zur Augenzone, seitlich aus dem Abstand zur Augenellipse im Bild
        dv = np.sqrt(np.nan_to_num(((t3['vorn'] - pa) / np.array(AUGE_ACHSEN)) ** 2, nan=9.0).sum(2))
        if ok3.any():          # ueber die Seitenellipse markierte Vorderbild-Texel: Abstand zur Seitenellipse (Lauf r2 Eichung 6: sonst Stufe 0)
            dvf = dv.reshape(-1)
            d_s3 = np.sqrt(((xs3[:, 0] - cx) / ax) ** 2 + ((xs3[:, 1] - cy) / ay) ** 2)
            dvf[idx_ok[inn]] = np.minimum(dvf[idx_ok[inn]], d_s3[inn])
        xs_t, ys_t = texel_bild(info, 'seite', kp)
        ds = np.sqrt(((xs_t - auge_s[0]) / auge_s[2]) ** 2 + ((ys_t - auge_s[1]) / auge_s[3]) ** 2)
        # Licht je Texel (Messblatt V4 Abschnitt 5 Schritt 3 "Texel-Ziel T = P / lambda"): das Auge liegt seitlich halb im Schatten
        # (lambda 1,0 oben -> 0,5 unten, Lauf r2 Eichung 16: S1 nur 0,034 H hoch) - die Stufe gleicht lambda aus
        import r2_koerper_farben as RK
        lam_t = {}
        for ans_, pose_ in (('vorn', P['135']), ('seite', P['85'])):
            _i, _d, U_, V_, Z_, LAM_ = RK.raster(mo, pose_)
            u0_, u1_ = kp[ans_]['u']; v0_, v1_ = kp[ans_]['v']; pb_ = 128 * (kp['page'] & 0xF)
            q = (Z_ == kp['clut']) & (U_ >= pb_ + u0_) & (U_ <= pb_ + u1_) & (V_ >= v0_) & (V_ <= v1_)
            sm = np.zeros((v1_ - v0_ + 1, u1_ - u0_ + 1)); n_ = np.zeros_like(sm)
            np.add.at(sm, (V_[q] - v0_, U_[q] - pb_ - u0_), LAM_[q].mean(1)); np.add.at(n_, (V_[q] - v0_, U_[q] - pb_ - u0_), 1)
            lam_t[ans_] = np.where(n_ > 0, sm / np.maximum(n_, 1), np.nan)
        for tz_, st_, d_, zn, ans_ in ((tz_v, st_v, dv, 'v_auge', 'vorn'), (tz_s, st_s, ds, 's_auge', 'seite')):
            q_ = tz_ == nm[zn]
            # Profil: Plateau bis Ellipsenabstand 1 - Rand, dann linear auf 0 am Rand (weicher Fleck mit stabiler Halbwertbreite; mit linearem
            # Verlauf ab der Mitte schwankte S1 zwischen den Eichlaeufen 0,034-0,042 H)
            prof = np.clip((1.0 - d_) / (AUGE_RAND_VORN if ans_ == 'vorn' else AUGE_RAND), 0, 1)
            # nur seitlich (vorn mit Ausgleich: Fleck 12 x 18 px, K3 rot, KV5 30 L*, Lauf r2 Eichung 18)
            lt = np.nan_to_num(lam_t[ans_], nan=1.0) if ans_ == 'seite' else np.ones_like(prof)
            ra = AUGE_RADIAL_VORN if ans_ == 'vorn' else AUGE_RADIAL
            g = (1.0 - ra + 2 * ra * prof) / np.clip(lt, 0.5, 1.0)
            if q_.any():                       # auf den Stufenbereich normiert (die Zonenfarbe traegt die absolute Helligkeit)
                g = g * (1.0 + ra) / max(float(g[q_].max()), 1e-6)
            st_[q_] = np.clip(np.round((g[q_] - (1.0 - ra)) / (2 * ra) * (STUFEN - 1)), 0, STUFEN - 1).astype(int)
        idx_v = 1 + tz_v * STUFEN + st_v; idx_s = 1 + tz_s * STUFEN + st_s
        zone_je_index = np.full(256, -1); fak_je_index = np.zeros((256, 3))
        for zz in range(len(ZONEN)):
            auge = ZONEN[zz][0] in ('v_auge', 's_auge')
            for s in range(STUFEN):
                zone_je_index[1 + zz * STUFEN + s] = zz
                if auge:                       # Augen: radialer Verlauf (Runde 2, K1-K4/S1-S2 bestanden) bleibt
                    ra = AUGE_RADIAL_VORN if ZONEN[zz][0] == 'v_auge' else AUGE_RADIAL
                    fak_je_index[1 + zz * STUFEN + s] = 1.0 + ra * (s - (STUFEN - 1) / 2.0) / ((STUFEN - 1) / 2.0)
                else:                          # Runde 3: Stufe = RGB-Faktor der Originalstruktur (Exponent je Zone, STRUKTUR_ZONE_R3)
                    lg = np.log(stufen_rgb[s])           # Exponent nur auf die hellen Stufen (dunkle Flecken bleiben voll)
                    fak_je_index[1 + zz * STUFEN + s] = np.exp(np.where(lg > 0, lg * STRUKTUR_ZONE_R3.get(ZONEN[zz][0], 1.0), lg))
        # Schwanz: Baender nach SCHWANZ_V4 uv baender (Runde 2: 5 x 51 Zeilen), Stufen aus der Struktur
        st_t, f_t = struktur_feld(mo, (256, 64), amp)
        idx_t = np.zeros((256, 64), int)
        vl = sw['uv'].get('verlauf')
        for bi, (bn, _k) in enumerate(BAENDER):
            v0_, v1_ = sw['uv']['baender'][bn]
            if vl and bn in SCHWANZ_VERLAUF:
                # V4 Runde 4: Zeile -> Lage um den Umfang c (wie birkin_formen.kette_meshes_v4 tx), Stufe = Lage im c-Bereich des Bands
                c_hi, c_lo = SCHWANZ_VERLAUF[bn]
                zj = vl['zeilen_je_c'][bn] if isinstance(vl['zeilen_je_c'], dict) else vl['zeilen_je_c']
                for r in range(v0_, v1_ + 1):
                    t_ = min(1.0, (r - v0_) / zj / (c_hi - c_lo))
                    idx_t[r] = 1 + bi * STUFEN + int(round(t_ * (STUFEN - 1)))
            else:
                idx_t[v0_:v1_ + 1] = 1 + bi * STUFEN + st_t[v0_:v1_ + 1]
        zone_t = np.full(256, -1); misch_t = np.zeros((256, len(BAENDER)))
        namen_b = [b[0] for b in BAENDER]
        for bi, (bn, _k) in enumerate(BAENDER):
            for s in range(STUFEN):
                zone_t[1 + bi * STUFEN + s] = bi
                if vl and bn in SCHWANZ_VERLAUF:
                    for nb, w in verlauf_mischung(bn, s).items():
                        misch_t[1 + bi * STUFEN + s, namen_b.index(nb)] += w
                else:
                    # Runde 2: Schwanz mit halber Hautstruktur - mit voller Struktur fielen die dunklen Stufen der Wurzel unter Chroma 6 und
                    # zaehlten in die Region F17 (Aussenkante, Chroma < 6), Lauf r2 Koerper 1: F17 L* 14,0 statt 26,8
                    misch_t[1 + bi * STUFEN + s, bi] = 1.0 + 0.5 * amp * (s - (STUFEN - 1) / 2.0) / ((STUFEN - 1) / 2.0)
        if runde > 0:
            # Loesen an den Renders dieses Stands
            mo_cl = dict(mo=mo)
            cv = code_und_licht(mo, P['135']); cs = code_und_licht(mo, P['85'])
            if runde == 1:
                # Startfarbe je Zone/Band = Ziel im Port-Schirm / mittleres lambda ihrer Bildpunkte (Messblatt Abschnitt 5 Schritt 2/3)
                for cl_, zl_, zi_, n_, st_ in ((cv, 6, zone_je_index, len(ZONEN), start_z), (cs, 6, zone_je_index, len(ZONEN), start_z),
                                               (cs, 7, zone_t, len(BAENDER), start_b)):
                    sel_ = cl_['zeile'] == zl_
                    zp = np.where(sel_, zi_[np.clip(cl_['idx'], 0, 255)], -1)
                    for zz in range(n_):
                        q = zp == zz
                        if q.sum() >= 3:
                            ziel_lab = ziel[ZONEN[zz][2]] if zi_ is zone_je_index else RV.ziel(BAENDER[zz][1])
                            st_[zz] = np.clip((lab_rgb(ziel_lab) - G.EA_) / G.EB_ / np.maximum(cl_['lam'][q].mean(0), 0.05), 0, 248)
            rv = regionen(mo_cl, None, 'vorn', ziel, P); rs = regionen(mo_cl, None, 'seite', ziel, P)
            # V4 Runde 4: die Augen-Regionen nach der Zone aus dem Code-Render (Texel der Zonen v_auge/s_auge), nicht nach dem hellsten
            # Fleck des aktuellen Renders - Lauf r4 Eichung c: der Fleck lag in einer Runde auf der Kopfoberseite seitlich, die Eichung
            # machte dort das "Auge" hell (S2 0,139 H, S4 grau) und verstaerkte den Fehler in den folgenden Runden
            augen_z = (nm['v_auge'], nm['s_auge'])
            for (cl_, rl_, kn_) in ((cv, rv, 'AUGE_V'), (cs, rs, 'AUGE_S')):
                ii_ = np.clip(cl_['idx'], 0, 255)
                # nur der helle Kern (obere Haelfte der radialen Stufen): der Riegel misst den Fleck ueber der Halbwertschwelle; mit allen
                # Augentexeln trieb die Eichung den Kern auf 72-74 L* (K2/S1 dE00 15-24, Lauf r4 Eichung e/f)
                zall = (cl_['zeile'] == 6) & np.isin(zone_je_index[ii_], augen_z)
                mz = zall & (((ii_ - 1) % STUFEN) >= STUFEN // 2)
                for j_, r_ in enumerate(rl_):
                    if r_[0] == kn_:
                        # der Fleck des Riegels (Halbwertschwelle), soweit er auf Augentexeln liegt; liegt er nicht auf dem Auge (Lauf r4
                        # Eichung c), der helle Kern der Augenzone (nur dann: mit dem Kern allein wurde das Auge zu hell, Lauf r4 Eichung g)
                        ms = r_[1] & zall
                        rl_[j_] = (kn_, ms if ms.sum() >= 0.5 * max(int(r_[1].sum()), 1) else mz, r_[2], r_[3])
            zonen_c = loese_zonen([(cv, rv), (cs, rs)], len(ZONEN), 6, None, fak_je_index, zone_je_index, start_z)
            # Schwanz-Regionen (riegel_v4.schwanz Masken)
            # Regionen genau wie riegel_v4 T8 (Kreise um T1/T2 mit Chroma >= 6, Chroma < 6 oberhalb T2, Ids zwischen T3 und T4)
            zt, s_, (img_t, ids_t, V_t, fr, rmask) = RV.schwanz(mo, P)
            # F17 doppelt gewichtet: die Region (Chroma < 6 ueber T2) enthaelt auch Mischpixel Kante/Mitte (Lauf r2 Eichung 2: 17,7 statt 26,8)
            rt = [(k, rmask[k], RV.ZIEL[k], 2.0 if k == 'F17' else 1.0) for k in ('F15', 'F16', 'F17', 'F18')]
            if vl:
                # V4 Runde 4: die Riegel-Regionen haengen am Chroma der aktuellen Farben (F17 = Chroma < 6 ueber der Biegung, F15/F16 =
                # Chroma >= 6 in Kreisen) - mit dem Verlauf tauschten die Baender dabei die Rollen (Lauf r4 Eichung c: Wurzel grau,
                # Aussenkante rot, Bandfarben schwingen 0 <-> 215). Zum LOESEN gelten deshalb die Baender aus dem Code-Render (gleiche
                # Kreise/Hoehen wie riegel_v4.schwanz, ohne Chroma-Bedingung); der Riegel misst danach wie bisher.
                bp = np.where(cs['zeile'] == 7, zone_t[np.clip(cs['idx'], 0, 255)], -1)
                yy_, xx_ = np.mgrid[:480, :640]
                me_ = RV.er(np.isin(ids_t, RV.SCHW), 1)
                b_, bi_ = s_['beginn'], s_['biegung']
                na_ = [b[0] for b in BAENDER]
                kreis15 = me_ & (np.hypot(xx_ - (b_[0] - 12), yy_ - (b_[1] - 5)) <= 12) & (bp != na_.index('aussen'))
                kreis16 = me_ & (np.hypot(xx_ - (bi_[0] + 10), yy_ - bi_[1]) <= 10) & (bp != na_.index('aussen'))
                oben17 = me_ & (bp == na_.index('aussen')) & (yy_ <= bi_[1])
                # F17 = Oberseite des Rohrs: Band 'aussen' und die oberen Verlaufsstufen der Seitenbaender (c >= ~0,5) ueber der Biegung.
                # Lauf r4 Eichung h (nur 'aussen'): F17 20,1 statt 26,8; Lauf r4 Eichung i (Riegelmaske Chroma < 6): die Maske fiel auf
                # die Mitte der Seitenbaender, die Eichung machte die Mitte hell und die Oberseite dunkel (Querprofil verkehrt)
                st_ = (np.clip(cs['idx'], 0, 255) - 1) % STUFEN
                mitte_b = np.isin(bp, [na_.index('mitte'), na_.index('senkrecht')])
                r17 = me_ & (yy_ <= bi_[1]) & ((bp == na_.index('aussen')) | (mitte_b & (st_ <= 3)))
                # Stand der committeten farben_v4.npz (Runde 4, vom Zahlen-Pruefer V4 r4 bitgleich nachgebaut): geloest nur mit F15, F18 und den
                # Achspunkten; F16 / F17 waren aus (Lauf r4 Eichung n mit F16/F17, F17-Gewicht 2: F17 kippte auf 4,9 L*, verworfen). Das
                # Dossier V4 Runde 4 nannte irrtuemlich F15/F16/F17 (berichtigt im Dossier V4 Runde 5). Seit V4 Runde 5 kommt der Schwanz
                # nicht mehr aus dieser Eichung (birkin_formen SCHWANZ_V4 uv art 'achse', tools/r41_sichel/v4/r5_schwanz_textur.py).
                # Nicht benutzt: ('F16', kreis16, RV.ZIEL['F16'], 1.0), ('F17', r17, RV.ZIEL['F17'], 2.0)
                rt = [('F15', kreis15, RV.ZIEL['F15'], 1.0), ('F18', rmask['F18'], RV.ZIEL['F18'], 1.0)]
            # Runde 2: die 7 Achsenpunkte (T9) als eigene Regionen mit dem Original-5x5-Mittel als Ziel
            for k9, (m9, z9) in RV.achse_regionen(ids_t, V_t).items():
                rt.append((k9, m9, z9, 1.0))
            # Aussenkante stark an ihre Startfarbe (F17-Ziel / mittleres Licht ihrer Bildpunkte) gebunden: im Verlauf mischt sie in die
            # Seitenbaender, und die dunkelroten Achsenpunkte (T9) zogen sie rot (Lauf r4 Eichung final 2: Aussenkante (68,24,37), F17 4,9 L*).
            # Gewicht 3,0 (Laeufe r4 Eichung r/s/t/u/v mit 1,0 / 0,4 / 0,6 / 3,0 / 10,0: F17 20,5 / 6,2 / 12,3 / 20,8 / 20,8 L*, T8 nur ab 3,0
            # gruen). PORT-WAHL (Regularisierung der Eichung, keine Spielkonstante), Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben"; seit V4 Runde 5 ohne Wirkung auf
            # das Modell (Schwanz aus r5_schwanz_textur.py).
            rgw = [float(os.environ.get('RE15_V4_RGW', '3.0')) if b[0] == 'aussen' else 0.15 for b in BAENDER]
            neu_b = loese_zonen_misch([(cs, rt)], len(BAENDER), 7, misch_t, start_b, rgw)
            # V4 Runde 4: gedaempft (halber Schritt ab Runde 2), die Regionen haengen am Render der vorigen Runde
            baender_c = neu_b if runde == 1 else 0.5 * (baender_c + neu_b)
            # Koerper
            zf, ef, (img1, ids1, V1, img2, ids2, V2, reg, reg2) = RV.koerper_farben(mo, P)
            # F8 (Oberarm (W)-links 16,4) nicht im Fit: beide Oberarme (Mesh 10 und 13) nutzen dieselbe Zeile 480 / Seite 128, das Original
            # ist links 17 L* dunkler als rechts (Licht im Video), das Port-Licht nicht - beide zugleich sind ueber die CLUT nicht erreichbar
            daten_k = [(cv, [(k, reg[k], RV.ZIEL[k]) for k in ('F6', 'F7', 'F9', 'F10')]),
                       (cs, [(k, reg2[k], RV.ZIEL[k]) for k in ('F14', 'F19')])]
            theta = loese_koerper(daten_k, basis[:3], klassen, theta)
        clut486 = np.zeros((256, 3)); clut487 = np.zeros((256, 3))
        for i in range(256):
            if zone_je_index[i] >= 0:
                clut486[i] = np.clip(zonen_c[zone_je_index[i]] * fak_je_index[i], 0, 248)
            if zone_t[i] >= 0:
                clut487[i] = np.clip(misch_t[i] @ baender_c, 0, 248)
        haut = basis.copy()
        haut[:3] = koerper_param(theta, basis[:3], klassen)
        np.savez(npz, kopf_vorn=idx_v.astype(np.uint8), kopf_seite=idx_s.astype(np.uint8), schwanz=idx_t.astype(np.uint8),
                 clut486=clut486, clut487=clut487, haut=haut)
        prot['runden'].append(dict(runde=runde, augenpunkt=[round(float(x), 1) for x in pa], auge_seite=auge_s, zonen={z[0]: [round(float(x), 1) for x in zonen_c[i]] for i, z in enumerate(ZONEN)},
                                   baender={n: [round(float(x), 1) for x in baender_c[i]] for i, (n, _k) in enumerate(BAENDER)},
                                   theta=[round(float(x), 4) for x in theta], kopfbox_vorn=kv, kopfbox_seite=ks, H=(Hv, Hs)))
        print('Runde', runde, 'Zonen', {z[0]: [int(x) for x in zonen_c[i]] for i, z in enumerate(ZONEN)}, flush=True)
    import shutil
    shutil.copy(npz, a.aus)
    json.dump(prot, open(a.prot, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print('geschrieben', a.aus, a.prot)


if __name__ == '__main__':
    main()
