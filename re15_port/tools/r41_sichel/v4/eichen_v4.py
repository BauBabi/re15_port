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
BAENDER = [('wurzel', 'F15'), ('aussen', 'F17'), ('unten', 'F16'), ('ende', 'F18')]   # Schwanz-Zeile 487 (Messblatt T8)
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
AUGE_ACHSEN = (95.0, 80.0, 95.0)   # Eichlauf 6 mit y 95: Auge vorn durch eine Flaechennaht geteilt (K1); Lauf 11 mit x/z 75/70: Auge seitlich 0,015 x 0,023 H in 4 Teilen (S1)
# Augenpunkt: K4 0,050 H (B)-links der Kopfmitte (Messblatt-Mitte), 0,11 H unter der Oberkante (Toleranz 0,07-0,12) statt 0,093: nur
# so liegt derselbe Oberflaechenpunkt in der Seitenansicht auch im S2-Band (0,034 H hinter der Kante, 0,814 H ueber der Sohle; mit 0,093
# 0,83 H > 0,82; Pruefung im Dossier V4). Eichlauf 5 mit 0,11: S2 0,829 H -> 0,118 (gemessen K4 y 0,105 bei 0,11). Ein Auge fuer beide Ansichten (Messblatt Abschnitt 3: das sichtbare Auge ist das (W)-rechte).
# Eichlauf 4 mit 0,055: Auge vorn als schmaler Streifen am Kopfrand (streifender Blick), daher die Mitte 0,050.
AUGE_K4 = (0.05, 0.12)
# Struktur halbiert in den Zonen, deren helle Stufen im Eichlauf 8 eigene Komponenten ueber der Halbwertschwelle von K1/S1 bildeten
# (Code-Render: vorn (333,178) s_basis lambda 0,98; seitlich (480,210) s_haar lambda 1,0); das Original hat dort genau 1 Komponente.
STRUKTUR_ZONE = dict(s_basis=0.5, s_haar=0.5, v_oben_links=0.5, s_hinten_unten=0.5)


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
        f_pix = np.where(sel, fak_je_index[np.clip(idx, 0, 255)], 0.0)
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
                    A[zz] = (w[q][:, None] * lam[q] * f_pix[q][:, None]).sum(0)
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
    a = ap.parse_args()
    os.makedirs(ARB, exist_ok=True)
    P = G.posen()
    ziel = ziele_original()
    amp, amp_einzeln = struktur_amplitude()
    prot = dict(ziele=ziel, struktur=dict(amp=amp, je_fenster=amp_einzeln), runden=[])
    kp, sw = BF.KOPF_V4, BF.SCHWANZ_V4
    npz = os.path.join(ARB, 'farben_runde.npz')
    if os.path.exists(npz):
        os.remove(npz)
    # Startfarben: Ziel im Port-Schirm / 0,6 (mittleres lambda der Kopf-/Schwanzflaechen im ersten Licht-Render)
    start_z = np.array([np.clip((lab_rgb(ziel[z[2]]) - G.EA_) / G.EB_ / 0.6, 0, 248) for z in ZONEN])
    start_b = np.array([np.clip((lab_rgb(RV.ZIEL[k]) - G.EA_) / G.EB_ / 0.6, 0, 248) for _n, k in BAENDER])
    ref2 = b2.modell(G.REF2)
    basis = np.array([[RV.np.array(c, float) for c in ref2.lut[z][:, :3]] for z in range(6)], float)
    klassen = klassen_haut(basis[:3])
    theta = np.zeros(21)
    zonen_c, baender_c = start_z.copy(), start_b.copy()
    for runde in range(a.runden + 1):
        # Zwischenstand schreiben: Texelbilder (Zone x 16 Stufen) + Zeilen
        blob, info = bauen(npz if os.path.exists(npz) else os.path.join(ARB, 'nicht_da.npz'))
        mo = b2.modell(blob)
        vorn_r = G.zeichne(mo, P['135'], voll=True); seite_r = G.zeichne(mo, P['85'], voll=True)
        kv = G.kopfbox(vorn_r[1]); ks = G.kopfbox(seite_r[1])
        okv, sov, Hv = G.h_port(vorn_r[2]); oks, sos, Hs = G.h_port(seite_r[2])
        pa = auge_3d(mo, kv, Hv, P)
        import augenpunkt_v4 as AP
        xs_a, _w = AP.proj(mo, P['85'], pa[None])
        sx = AP.proj(mo, P['85'], (pa + np.array([AUGE_ACHSEN[0], 0, AUGE_ACHSEN[2]]))[None])[0][0] - xs_a[0]
        sy = AP.proj(mo, P['85'], (pa + np.array([0, AUGE_ACHSEN[1], 0]))[None])[0][0] - xs_a[0]
        auge_s = (xs_a[0][0], xs_a[0][1], float(np.hypot(*sx)), float(np.hypot(*sy)))
        karte_v = zonen_karte(kv, Hv, (okv, sov), 'vorn'); karte_s = zonen_karte(ks, Hs, (oks, sos), 'seite', auge_seite=auge_s)
        tz_v = texel_zonen(mo, info, 'vorn', karte_v, kp); tz_s = texel_zonen(mo, info, 'seite', karte_s, kp)
        t3 = texel_3d(mo, kp)
        nm = {z[0]: i for i, z in enumerate(ZONEN)}
        for ans, tz, zn in (('vorn', tz_v, 'v_auge'), ('seite', tz_s, 's_auge')):
            d = (t3[ans] - pa) / np.array(AUGE_ACHSEN)
            im_auge = np.nan_to_num((d ** 2).sum(2), nan=9.0) <= 1.0
            tz[im_auge] = nm[zn]
        # Haarstraehne (KOPF_V4 straehne = letztes Viereck von Mesh 9): ihre Texel ganz in der Haarzone (Eichlauf 7: Zonenstreuung an
        # der Kante zeigte ein Schachbrett auf der Straehne)
        m9 = mo.meshes[9]
        if len(m9.qf) > 18:
            uvq = m9.quv[-1]
            poly = np.array([(uvq[0], uvq[1]), (uvq[3], uvq[4]), (uvq[9], uvq[10]), (uvq[6], uvq[7])], np.int32)
            su0, su1 = kp['seite']['u']; sv0, sv1 = kp['seite']['v']
            mk = np.zeros((sv1 - sv0 + 1, su1 - su0 + 1), np.uint8)
            cv2.fillConvexPoly(mk, poly - np.array([su0, sv0], np.int32), 1)
            tz_s[mk > 0] = nm['s_haar']
        st_v, f_v = struktur_feld(mo, tz_v.shape, amp); st_s, f_s = struktur_feld(mo, tz_s.shape, amp)
        idx_v = 1 + tz_v * STUFEN + st_v; idx_s = 1 + tz_s * STUFEN + st_s
        zone_je_index = np.full(256, -1); fak_je_index = np.zeros(256)
        for zz in range(len(ZONEN)):
            a_z = amp * STRUKTUR_ZONE.get(ZONEN[zz][0], 1.0)
            for s in range(STUFEN):
                zone_je_index[1 + zz * STUFEN + s] = zz
                fak_je_index[1 + zz * STUFEN + s] = 1.0 + a_z * (s - (STUFEN - 1) / 2.0) / ((STUFEN - 1) / 2.0)
        # Schwanz: Baender je 64 Zeilen (SCHWANZ_V4 uv baender), Stufen aus der Struktur
        st_t, f_t = struktur_feld(mo, (256, 64), amp)
        idx_t = np.zeros((256, 64), int)
        for bi in range(len(BAENDER)):
            idx_t[bi * 64:(bi + 1) * 64] = 1 + bi * STUFEN + st_t[bi * 64:(bi + 1) * 64]
        zone_t = np.full(256, -1); fak_t = np.zeros(256)
        for bi in range(len(BAENDER)):
            for s in range(STUFEN):
                zone_t[1 + bi * STUFEN + s] = bi; fak_t[1 + bi * STUFEN + s] = fak_je_index[1 + s]
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
                            ziel_lab = ziel[ZONEN[zz][2]] if zi_ is zone_je_index else RV.ZIEL[BAENDER[zz][1]]
                            st_[zz] = np.clip((lab_rgb(ziel_lab) - G.EA_) / G.EB_ / np.maximum(cl_['lam'][q].mean(0), 0.05), 0, 248)
            rv = regionen(mo_cl, None, 'vorn', ziel, P); rs = regionen(mo_cl, None, 'seite', ziel, P)
            zonen_c = loese_zonen([(cv, rv), (cs, rs)], len(ZONEN), 6, None, fak_je_index, zone_je_index, start_z)
            # Schwanz-Regionen (riegel_v4.schwanz Masken)
            # Regionen genau wie riegel_v4 T8 (Kreise um T1/T2 mit Chroma >= 6, Chroma < 6 oberhalb T2, Ids zwischen T3 und T4)
            zt, s_, (img_t, ids_t, V_t, fr, rmask) = RV.schwanz(mo, P)
            rt = [(k, rmask[k], RV.ZIEL[k], 1.0) for k in ('F15', 'F16', 'F17', 'F18')]
            baender_c = loese_zonen([(cs, rt)], len(BAENDER), 7, None, fak_t, zone_t, start_b)
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
                clut487[i] = np.clip(baender_c[zone_t[i]] * fak_t[i], 0, 248)
        haut = basis.copy()
        haut[:3] = koerper_param(theta, basis[:3], klassen)
        np.savez(npz, kopf_vorn=idx_v.astype(np.uint8), kopf_seite=idx_s.astype(np.uint8), schwanz=idx_t.astype(np.uint8),
                 clut486=clut486, clut487=clut487, haut=haut)
        prot['runden'].append(dict(runde=runde, augenpunkt=[round(float(x), 1) for x in pa], auge_seite=auge_s, zonen={z[0]: [round(float(x), 1) for x in zonen_c[i]] for i, z in enumerate(ZONEN)},
                                   baender={n: [round(float(x), 1) for x in baender_c[i]] for i, (n, _k) in enumerate(BAENDER)},
                                   theta=[round(float(x), 4) for x in theta], kopfbox_vorn=kv, kopfbox_seite=ks, H=(Hv, Hs)))
        print('Runde', runde, 'Zonen', {z[0]: [int(x) for x in zonen_c[i]] for i, z in enumerate(ZONEN)}, flush=True)
    import shutil
    shutil.copy(npz, AUS)
    json.dump(prot, open(PROT, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print('geschrieben', AUS, PROT)


if __name__ == '__main__':
    main()
