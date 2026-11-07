"""SF V5 (Bauer, Kopf aus RE2, Runde 2): Kopf der Sichel-Form = RE2-Birkin-G2-Kopf (CDEMD0.EMS kind 0x31, Mesh 3) anstelle von Mesh 9
(KOPF_V4) im EMD der Runde 3 (EM30_V4ot3.EMD); Maul = Zahn-Kachel des G3-Kopfes (kind 0x33, TIM page 0 u 2-13 v 5-40) auf den
unteren Vorderflaechen des G2-Kopfes.

    python kopf_re2_bau.py [--aus DIR] [--basis EMD] [--json PROT]      -> DIR/EM30_V4re2.EMD (+ EM30_V4.EMD), Protokoll

Nutzer (2026-11-07, woertlich): "Vielleicht kannst du dir auch einen Birkin aus Resident Evil 2 nehmen. Ich nehme an die duerften aehnlich
aufgebaut sein die anderen Formen...." und "bei den KI Bildern kann man so verdammt gut sehen wie das verschwommene Bild eigentlich aussehen
muss.... Man sieht alles einwandfrei! Das Auge die Haare, den Mund, den Schwanz...."
Leser A (SF_belege/re2kopf/00_koepfe_re2_vs_ki.png, mess.json): naechster Einzelkandidat = G2-Kopf (0x31 Mesh 3; dE00 zu KI-Front 3,0, zu
Beschreibung1 6,6, zu Beschreibung2 6,5); Zaehne nur im G3-Kopf (0x33, Zahn-Anteil 0,0225). Leser B (KI_LESUNG.md): Kuppel mit Schnauze,
Maul an der Unterseite mit zwei Zahnreihen, ein kleines dunkles Auge oben vorn (B)-links bis mittig (D3: dunkle Pupille mit Glanzpunkt),
(B)-rechte Haelfte 9-21 L* dunkler (D4), Kopfhaut dunkles warmes Grau (D6).

Herkunft (byte-true, Zaehlung wie re2_leser.rec): G2 EMD = CDEMD0.EMS Sektor 2944 (193124 B, sha256 bb966cd5...), MD1 dir[7], Mesh 3 (48 Ecken,
48 Normalen, 56 Dreiecke, 14 Vierecke, UV page 128 / CLUT 480 = Zeile 0 der G2-TIM); G2 TIM = Sektor 2895 (99872 B), 384 x 256 8 bpp,
Texel u 0-125 v 0-89, CLUT-Zeile 0 (y 480). G3 TIM = Sektor 3065 (99872 B), Zahn-Kachel u 2-13 v 5-40, CLUT-Zeile 0.

Runde 2 (nach Zahlen-/Sicht-Pruefung r1: Kopf im RDT-Licht L* 15,6 = fast schwarz, rechte Haelfte nicht dunkler, Auge unsichtbar):
  * ALLE Kopf-Texel bleiben bytegleich G2 (Runde 1 hatte 3821 Texel auf dunklere Eintraege umgesetzt - weg).
  * Die Helligkeit kommt aus der PALETTE: Zeile 486 = G2-Zeile 0 mit L* x k_grau (a*, b* unveraendert = Farbton und Adern bleiben; der
    Sicht-Pruefer r1 nannte genau diesen Weg "keine Farbmischung"); Zeile 488 = dieselbe G2-Zeile mit L* x k_dunkel, Chroma x g_dunkel,
    L* <= lmax (Haarseite, Nase, Schnauzenspitze). Jede Flaeche bekommt eine der beiden Zeilen (uv[2]), Texel-Indizes gleich.
  * Zonen je Flaeche (G2-Koordinaten vor der Drehung): dunkel = Flaechenmittelpunkt Bild-rechts der Kopfbox-Mitte in Cut 12 Pose 135
    (Ebene PARAM['zonen']['ebene_n'/'ebene_d']; Nutzer "rechts Seite sieht man Haare - braeunlich") oder u-Mittel im Gesichtsspalt
    nase_u (Nutzer "Bis Nase") oder Schnauzenspitzen-Flaeche Q0 (Haarkante KS2 der Beschreibung2); sonst grau.
Transformation (PORT-WAHL nach Messblatt-Kopfbox K10 und Leser B, s. PARAM): q = Rz(pitch) . Ry(yaw) . Rx(roll) . (skal * v) + versatz im Raum
des Ref2-Knochens 9 (rel (111,-971,-133) unveraendert; Skelett, Clips, EDD unveraendert). Normalen mitgedreht.
Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante.
"""
import os, sys, json, struct, argparse, hashlib
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, HIER); sys.path.insert(0, TOOLS); sys.path.insert(0, os.path.join(TOOLS, 'r41_sichel', 'v4'))
import re2_leser as RL
import emd_ansichtsblatt as EA
import birkin_formen as BF
from skimage.color import rgb2lab, lab2rgb

SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
BASIS = os.environ.get('RE15_V5_BASIS', os.path.join(SP, 'sfv4', 'emd', 'EM30_V4ot3.EMD'))     # sha256 a1bc9309 (Runde 3)
AUS = os.path.join(SP, 'sfv4', 'emd')
G2, G3 = 0x31, 0x33
KOPF = 9                        # Mesh/Knochen des Kopfes im EM30 (Ref2)
# TIM-Orte im EM30-Atlas (Ref2: TPage 128 Zeilen 155-255 frei, TPage 130 x 256-319 Zeilen 126-255 frei; V4-Schwanz TPage 130 u 64-127)
G2_ORT = dict(page=128, u0=0, v0=160, u=(0, 125), v=(0, 89))        # G2-Texel u 0-125 v 0-89 -> x 0-125, y 160-249
# G3-Zahn-Kachel u 2-13 v 5-40 -> x 256-267, y 200-235 (Spalten 0-1 und 14-20 der Kachel tragen den blauen Stirnstreifen des G3, b* < -15; probe9)
G3_ORT = dict(page=130, u0=0, v0=200, u=(2, 13), v=(5, 40))
ZEILE_G2, ZEILE_G3, ZEILE_DUNKEL = 486, 487, 488   # neue CLUT-Zeilen (Ref2 hat 480-485); 9 Zeilen gesamt <= 16 (Port-Texturhoehe 2304 <= 4096)
# Maulflaechen des G2-Kopfes (Indizes in Mesh 3, T = Dreieck, Q = Viereck): die unteren Vorderflaechen unter den Augen (y > -140 vor der
# Drehung, Normale +x), s. probe2 / Protokoll 'maul_flaechen'
MAUL_T = [25, 26, 34, 35, 42, 44, 28, 29, 22, 43, 5]
MAUL_Q = []
# Zonen (Flaechenmittelpunkte im unskalierten G2-Raum, Liste Scratchpad re2kopf_r2 "Flaechen": z -161..+172, x -182..+223; Gesichtsspalt
# u 41-60, Kuppe/Hinterkopf u 101-125, (W)-linke Gesichtshaelfte u 61-100)
# Parameter: Drehung/Skala/Versatz wie Runde 1 (Suche re2kopf_bau/grid1-4.log; K10 0,165 x 0,170 H, K4 0,060 / 0,097 H, S2 0,043 / 0,812 H
# gruen in der Zahlen-Pruefung r1). roll 0: Rollen um die Schnauzenachse wurde als Weg gemessen, das Auge ins Licht zu heben (re2kopf_r2/
# grid_roll.log: Augenfaecher-Lichtfaktor p50 0,22 -> 0,51 erst bei roll -20, dabei K10-Breite 0,143 H und KS2-Fenster nur noch 0,26 Kopf) -
# verworfen, kein Vorbild zeigt einen schiefen Kopf. Palette: Suche re2kopf_r2/pal_test.py / grid_pal.log (Kopf-Median im RDT-Licht Pose 135).
PARAM = dict(
    skal=(1.50, 1.50, 1.50),      # gleichmaessig
    pitch=10.0,                   # Grad, positiv = Schnauze nach unten (um die lokale z-Achse)
    yaw=-34.0,                    # Grad, positiv = Schnauze nach (W)-rechts (um die lokale y-Achse)
    roll=0.0,                     # Grad, um die Schnauzenachse (negativ hebt die (W)-rechte Seite)
    versatz=(140.0, 10.0, 0.0),   # E. im Knochen-9-Raum, nach der Drehung (y +10 = 10 E. tiefer als der Knochenursprung)
    # Zonengrenze = Bildmitte der Kopfbox in Cut 12 Pose 135 (Beschreibung1: "Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite ...
    # Haare"): Ebene n . c = d im unskalierten G2-Kopfraum, n = normierter Bild-x-Gradient der Spielkamera 50A0/12 an der Kopfmitte
    # (re2kopf_r2/ebene.json: Gradient (0,0368, 0,0829, 0,1128) px/E., Kopfbox-Mitte x 354,5); Flaechen mit n . c > d liegen Bild-rechts.
    # Die z-Halbierung (z > 0) der ersten Runde-2-Fassung traf nur den Bildrand: die Kamera steht (W)-links-vorn-oben, in Pose 135 zeigten
    # 1454 von 2461 Kopfpixeln die dunkle Zeile (re2kopf_r2/z1_135.png), die graue nur 366 am linken Rand.
    zonen=dict(ebene_n=(0.2543, 0.5728, 0.7792), ebene_d=-60.1,
               nase_u=(41, 60),   # Gesichtsspalt (leuchtende Mittelspalte der G2-Textur) -> dunkel ("Bis Nase": die graue Seite endet an der Nase)
               spitze_Q=(0,), spitze_T=()),   # Schnauzenspitze: nur Q0 (z -72, x 173, y -252) -> dunkel; das Fenster KS2 zeigt in Pose 85 zu 0,93
                                              # diese Flaeche (Runde 1 probe6). Runde 1 hatte alle Flaechen x > 150 dunkel - in Pose 135 ist das
                                              # die Schnauzenfront links der Mitte, die nach Beschreibung1 grau sein muss (re2kopf_r2/z2_135.png).
    # Palette (Belichtung der G2-Zeile 0, je Zeile ein L*-Faktor; a*/b* der grauen Zeile unveraendert):
    # Suche re2kopf_r2/grid_pal*.log (Kopf-Median mit RDT-Licht Pose 135, Haelften, KV2, Augenfaecher; Zonen nach der Bildmitte-Ebene, Spitze
    # nur Q0): k_grau 1,7 / 1,9 -> Kopf P50 23,8 / 24,2 (dunkel 1,05/32) bzw. 23,7 (1,9 mit dunkel 1,0/30), Haelften links 27,9-29,1 gegen
    # rechts 20,9-21,8 (0,74), KV2 30,1-30,6 (Grenze Ref2-Kopf P50 - 8 = 31,4); dunkel k 0,9 / lmax 26 -> P50 20,5 (T-K9e < 23), k 1,1 / lmax 34
    # -> KV2 31,6 (rot). Gewaehlt 1,9 und 1,0 / 30 / 0,5. Die graue Zone liegt damit bei L* ~40 (RDT-Licht), die dunkle bei ~21, das Maul bei 17
    # (KI: linke Haelfte 32-38, rechte 17-24, Maul 16-18; Leser B).
    pal=dict(grau=dict(k=1.90, g=1.0, lmax=100.0),      # Zeile 486: Bild-linke Kopfhaelfte ("Linke Seite ist so graeulich"), Farbton/Adern bleiben
             dunkel=dict(k=1.00, g=0.50, lmax=30.0)),   # Zeile 488: Haarseite / Nase / Spitze (Leser B D4: 9-21 L* dunkler, braun-grau, wenig Chroma)
)


def rz(grad):
    a = np.radians(grad); c, s = np.cos(a), np.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def ry(grad):
    """Gieren um die lokale y-Achse; positiv dreht die Schnauze (+x) nach (W)-rechts (z < 0)."""
    a = np.radians(grad); c, s = np.cos(a), np.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rx(grad):
    """Rollen um die lokale x-Achse (Schnauzenachse); negativ hebt die (W)-rechte Kopfseite (z < 0) nach oben (-y) ins Licht."""
    a = np.radians(grad); c, s = np.cos(a), np.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def rot(p):
    return rz(p['pitch']) @ ry(p.get('yaw', 0.0)) @ rx(p.get('roll', 0.0))


def lab_pal(pal_rgb):
    return rgb2lab(np.asarray(pal_rgb, float).reshape(-1, 1, 3) / 255.0).reshape(-1, 3)


def clut15_rgb(c):
    c = np.asarray(c).astype(np.int64)
    return np.stack([(c & 31) * 8, ((c >> 5) & 31) * 8, ((c >> 10) & 31) * 8], -1)


_M_XYZ2RGB = np.array([[3.2406, -1.5372, -0.4986], [-0.9689, 1.8758, 0.0415], [0.0557, -0.2040, 1.0570]])
_WP_D65 = np.array([0.95047, 1.0, 1.08883])


def _lab2rgb_lin(lab):
    """CIE-Lab (D65) -> lineares sRGB OHNE Clipping (skimage.lab2rgb clippt auf [0, 1] und taugt nicht als Gamut-Test)."""
    L, a, b = (float(v) for v in lab)
    fy = (L + 16.0) / 116.0; fx = fy + a / 500.0; fz = fy - b / 200.0

    def finv(t):
        return t ** 3 if t > 6.0 / 29.0 else 3.0 * (6.0 / 29.0) ** 2 * (t - 4.0 / 29.0)
    xyz = np.array([finv(fx), finv(fy), finv(fz)]) * _WP_D65
    return _M_XYZ2RGB @ xyz


def _in_gamut(lab, eps=1e-4):
    rgb = _lab2rgb_lin(lab)
    return bool(((rgb >= -eps) & (rgb <= 1 + eps)).all())


def lab_gamut(lab2):
    """Farbton-erhaltende Gamut-Abbildung: liegt (L*, a*, b*) ausserhalb von sRGB, wird nur die Chroma (a*, b* gemeinsam) verkleinert
    (Bisektion), der Farbwinkel bleibt. Ein hartes RGB-Clipping (Runde-2-Erstfassung) drehte den Farbwinkel heller Eintraege um bis zu
    38 Grad (Riegel T-R1d)."""
    out = np.array(lab2, float)
    for i in range(out.shape[0]):
        if _in_gamut(out[i]):
            continue
        lo, hi = 0.0, 1.0
        for _ in range(20):
            s = 0.5 * (lo + hi)
            if _in_gamut([out[i, 0], lab2[i, 1] * s, lab2[i, 2] * s]):
                lo = s
            else:
                hi = s
        out[i, 1:] = lab2[i, 1:] * lo
    return out


def zeile_transform(clut15, k, g=1.0, lmax=100.0):
    """Palettentransformation einer 15-Bit-CLUT-Zeile: RGB (x8 wie der Port/BB.modell) -> CIE-Lab -> L*' = min(k x L*, lmax),
    a*' = g a*, b*' = g b* -> Farbton-erhaltende Gamut-Abbildung (lab_gamut) -> RGB -> 5 Bit je Kanal. Eintrag 0 bleibt 0 (transparent),
    kein anderer Eintrag wird 0; STP-Bit bleibt."""
    c = np.asarray(clut15).astype(np.int64)
    lab = rgb2lab((clut15_rgb(c) / 255.0).reshape(-1, 1, 3)).reshape(-1, 3)
    lab2 = np.stack([np.clip(np.minimum(lab[:, 0] * k, lmax), 0, 100), lab[:, 1] * g, lab[:, 2] * g], 1)
    lab2 = lab_gamut(lab2)
    rgb2 = np.clip(lab2rgb(lab2.reshape(-1, 1, 3)).reshape(-1, 3), 0, 1)
    q = np.clip(np.rint(rgb2 * 255 / 8.0), 0, 31).astype(np.int64)
    w = q[:, 0] | (q[:, 1] << 5) | (q[:, 2] << 10) | (c & 0x8000)
    return np.where(c == 0, 0, np.where(w == 0, 1, w)).astype('<u2')


def zone_flaeche(V, idx, uv, n, zn, kind='T', i=-1):
    """'dunkel' oder 'grau' fuer eine Flaeche (Ecken-Indizes idx, UV-Liste uv mit n Ecken, Art kind 'T'/'Q' und Index i) im unskalierten
    G2-Raum; zn = PARAM['zonen']. dunkel = Flaechenmittelpunkt auf der Bild-rechten Seite der Ebene n . c > d (Bildmitte der Kopfbox in
    Cut 12 Pose 135, s. PARAM) oder Gesichtsspalt (u-Mittel in nase_u) oder die Schnauzenspitzen-Flaeche(n) spitze_Q / spitze_T."""
    c = V[idx].mean(0); um = np.mean([uv[3 * k] for k in range(n)])
    if float(np.dot(zn['ebene_n'], c)) > zn['ebene_d'] or zn['nase_u'][0] <= um <= zn['nase_u'][1] or i in zn.get('spitze_' + kind, ()):
        return 'dunkel'
    return 'grau'


def g2_kopf_mesh(p, prot):
    """Mesh 3 des G2 als birkin_formen-Mesh-Dict (v, n, tf, tuv, qf, quv), transformiert; UVs auf den EM30-Atlas gelegt, CLUT-Zeile je Zone."""
    mo = RL.modell(G2)
    m = mo.meshes[3]
    sk = np.array(p['skal'], float); R = rot(p); t = np.array(p['versatz'], float)
    V = np.array([v[:3] for v in m.tv], float)
    Q = (V * sk) @ R.T + t
    N = np.array([n[:3] for n in m.tn], float)
    Nn = (N / sk) @ R.T
    Ln = np.linalg.norm(N, axis=1, keepdims=True); Ln2 = np.linalg.norm(Nn, axis=1, keepdims=True); Ln2[Ln2 == 0] = 1
    Nn = Nn / Ln2 * Ln
    out = dict(v=[[int(round(c)) for c in q] + [v[3]] for q, v in zip(np.clip(Q, -32768, 32767), m.tv)],
               n=[[int(round(c)) for c in q] + [n[3]] for q, n in zip(Nn, m.tn)],
               tf=[list(x) for x in m.tf], qf=[list(x) for x in m.qf], tuv=[], quv=[])
    zonen = dict(T=[], Q=[])
    for kind, faces, uvs, n in (('T', m.tf, m.tuv, 3), ('Q', m.qf, m.quv, 4)):
        for i, (f, uv) in enumerate(zip(faces, uvs)):
            uv = list(uv)
            idx = [f[2 * k + 1] for k in range(n)]
            z = zone_flaeche(V, idx, uv, n, p['zonen'], kind, i)
            zonen[kind].append(z)
            for k in range(n):
                uv[3 * k] = uv[3 * k] - G2_ORT['u'][0] + G2_ORT['u0']; uv[3 * k + 1] = uv[3 * k + 1] - G2_ORT['v'][0] + G2_ORT['v0']
            uv[2], uv[5] = (ZEILE_DUNKEL if z == 'dunkel' else ZEILE_G2) << 6, G2_ORT['page']
            out['tuv' if kind == 'T' else 'quv'].append(uv)
    prot['g2'] = dict(ecken=len(m.tv), normalen=len(m.tn), dreiecke=len(m.tf), vierecke=len(m.qf),
                      bbox_quelle=[V.min(0).tolist(), V.max(0).tolist()], bbox_neu=[Q.min(0).tolist(), Q.max(0).tolist()],
                      uv_quelle=sorted({(u[5], u[2]) for u in list(m.tuv) + list(m.quv)}))
    prot['zonen'] = dict(regel=dict(p['zonen']),
                         dunkel_T=[i for i, z in enumerate(zonen['T']) if z == 'dunkel' and i not in MAUL_T],
                         grau_T=[i for i, z in enumerate(zonen['T']) if z == 'grau' and i not in MAUL_T],
                         dunkel_Q=[i for i, z in enumerate(zonen['Q']) if z == 'dunkel' and i not in MAUL_Q],
                         grau_Q=[i for i, z in enumerate(zonen['Q']) if z == 'grau' and i not in MAUL_Q], maul_T=list(MAUL_T), maul_Q=list(MAUL_Q))
    return out, mo, V


def maul_uv(out, V, p, prot):
    """Maulflaechen (MAUL_T/MAUL_Q) auf die G3-Zahn-Kachel legen: Parallelprojektion der (unskalierten) Ecken dieser Flaechen auf die Ebene
    (z, y) -> u aus z (Breite), v aus y (oben = obere Zahnreihe); Kachel u 2-13 v 5-40 in G3-Zaehlung."""
    idx = sorted({out['tf'][i][2 * k + 1] for i in MAUL_T for k in range(3)} | {out['qf'][i][2 * k + 1] for i in MAUL_Q for k in range(4)})
    P = V[idx]
    z0, z1 = P[:, 2].min(), P[:, 2].max(); y0, y1 = P[:, 1].min(), P[:, 1].max()
    u0, u1 = G3_ORT['u']; v0, v1 = G3_ORT['v']

    def uv_von(vi):
        q = V[vi]
        # (W)-links (z > 0) liegt im Vorderbild rechts -> u waechst mit -z (Blick von vorn auf das Gesicht: Bild-rechts = (W)-links)
        u = u0 + (z1 - q[2]) / max(z1 - z0, 1) * (u1 - u0)
        v = v0 + (q[1] - y0) / max(y1 - y0, 1) * (v1 - v0)
        return int(round(u - u0 + G3_ORT['u0'])), int(round(v - v0 + G3_ORT['v0']))
    n_t = n_q = 0
    for i in MAUL_T:
        f = out['tf'][i]; uv = out['tuv'][i]
        for k in range(3):
            uv[3 * k], uv[3 * k + 1] = uv_von(f[2 * k + 1])
        uv[2], uv[5] = ZEILE_G3 << 6, G3_ORT['page']; n_t += 1
    for i in MAUL_Q:
        f = out['qf'][i]; uv = out['quv'][i]
        for k in range(4):
            uv[3 * k], uv[3 * k + 1] = uv_von(f[2 * k + 1])
        uv[2], uv[5] = ZEILE_G3 << 6, G3_ORT['page']; n_q += 1
    prot['maul_flaechen'] = dict(dreiecke=MAUL_T, vierecke=MAUL_Q, ecken=idx, z=(float(z0), float(z1)), y=(float(y0), float(y1)),
                                 kachel=dict(G3_ORT), n=n_t + n_q)


def clut_roh(mo):
    t = mo.tim; cs_, = struct.unpack_from('<I', t, 8); cw_, ch_ = struct.unpack_from('<HH', t, 16)
    return np.frombuffer(t[20:20 + 2 * cw_ * ch_], '<u2').reshape(ch_, cw_)


def zeilen_neu(p, mo2, mo3):
    """(Zeile 486, Zeile 487, Zeile 488) als 15-Bit-Worte: G2-Zeile 0 transformiert (grau), G3-Zeile 0 bytegleich, G2-Zeile 0 transformiert (dunkel)."""
    z2, z3 = clut_roh(mo2)[0], clut_roh(mo3)[0]
    pg, pd = p['pal']['grau'], p['pal']['dunkel']
    return zeile_transform(z2, pg['k'], pg['g'], pg['lmax']), z3.astype('<u2'), zeile_transform(z2, pd['k'], pd['g'], pd['lmax'])


def tim_bauen(tim_alt, mo2, mo3, p, prot):
    """EM30-TIM: Texel der ot3-Kopfflaechen (TPage 128 v 160-223) auf 0 zurueck, G2-Kopftexel (bytegleich) nach G2_ORT, G3-Zahn-Kachel nach
    G3_ORT; CLUT-Block um die Zeilen 486 (G2 grau), 487 (G3) und 488 (G2 dunkel) verlaengert, Zeilen 480-485 bytegleich."""
    tim = bytearray(tim_alt)
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', tim, 8)
    c = np.frombuffer(bytes(tim[20:20 + 2 * cw * ch]), '<u2').reshape(ch, cw).copy()
    ende = 8 + cs
    iw, ih = struct.unpack_from('<HH', tim, ende + 8); breite = iw * 2
    pix = np.frombuffer(bytes(tim[ende + 12:ende + 12 + breite * ih]), np.uint8).reshape(ih, breite).copy()
    kp = BF._variante_v4()['kopf_v4']
    pb = 128 * (kp['page'] & 0xF)
    alt_kopf = 0
    for ans in ('vorn', 'seite'):
        u0, u1 = kp[ans]['u']; v0, v1 = kp[ans]['v']
        alt_kopf += int((pix[v0:v1 + 1, pb + u0:pb + u1 + 1] != 0).sum())
        pix[v0:v1 + 1, pb + u0:pb + u1 + 1] = 0
    # G2 (bytegleich)
    o = G2_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']
    tex_g2 = mo2.tex[o['v'][0]:o['v'][1] + 1, o['u'][0]:o['u'][1] + 1]
    hh, ww = tex_g2.shape
    pix[o['v0']:o['v0'] + hh, x0:x0 + ww] = tex_g2
    # G3 Kachel
    o3 = G3_ORT; x3 = 128 * (o3['page'] & 0xF) + o3['u0']
    kach = mo3.tex[o3['v'][0]:o3['v'][1] + 1, o3['u'][0]:o3['u'][1] + 1]
    pix[o3['v0']:o3['v0'] + kach.shape[0], x3:x3 + kach.shape[1]] = kach
    assert ch == 6 and cy == 480, (ch, cy)
    zg, z3, zd = zeilen_neu(p, mo2, mo3)
    alle = np.vstack([c, zg[None], z3[None], zd[None]]).astype('<u2')
    blk = struct.pack('<I4H', 12 + alle.size * 2, cx, cy, cw, alle.shape[0]) + alle.tobytes()
    neu = bytes(tim[:8]) + blk + bytes(tim[ende:ende + 12]) + pix.tobytes() + bytes(tim[ende + 12 + breite * ih:])
    # Protokoll der Palettentransformation: Lab der Quelle und der Zeilen (belegte Eintraege)
    q2 = clut_roh(mo2)[0]
    idx = np.unique(tex_g2[tex_g2 != 0])
    labq = lab_pal(clut15_rgb(q2)); labg = lab_pal(clut15_rgb(zg)); labd = lab_pal(clut15_rgb(zd))
    prot['tim'] = dict(clut_zeilen=int(alle.shape[0]),
                       zeilen_neu={ZEILE_G2: 'G2 0x31 CLUT-Zeile 0 (y 480), L* x %.2f, Chroma x %.2f, L* <= %.0f' % (p['pal']['grau']['k'], p['pal']['grau']['g'], p['pal']['grau']['lmax']),
                                   ZEILE_G3: 'G3 0x33 CLUT-Zeile 0 (y 480) bytegleich',
                                   ZEILE_DUNKEL: 'G2 0x31 CLUT-Zeile 0 (y 480), L* x %.2f, Chroma x %.2f, L* <= %.0f' % (p['pal']['dunkel']['k'], p['pal']['dunkel']['g'], p['pal']['dunkel']['lmax'])},
                       g2_ort=dict(G2_ORT), g3_ort=dict(G3_ORT), alt_kopf_texel_geloescht=alt_kopf,
                       g2_texel=int((tex_g2 != 0).sum()), g3_texel=int((kach != 0).sum()),
                       port_texturhoehe=int(ih * alle.shape[0]),
                       palette_lab_p50_belegt=dict(quelle=[float(v) for v in np.median(labq[idx], 0)], grau=[float(v) for v in np.median(labg[idx], 0)],
                                                   dunkel=[float(v) for v in np.median(labd[idx], 0)]),
                       texel_lab_p50=dict(quelle=[float(v) for v in np.median(labq[tex_g2[tex_g2 != 0]], 0)],
                                          grau=[float(v) for v in np.median(labg[tex_g2[tex_g2 != 0]], 0)],
                                          dunkel=[float(v) for v in np.median(labd[tex_g2[tex_g2 != 0]], 0)]))
    return neu


def bauen(p=PARAM, basis=BASIS):
    prot = dict(basis=basis, basis_sha256=hashlib.sha256(open(basis, 'rb').read()).hexdigest(), param=json.loads(json.dumps(p)),
                quellen={})
    for kind in (G2, G3):
        se, ln, emd = RL.rec(kind, 3); st, lt, tim = RL.rec(kind, 2)
        prot['quellen']['0x%02x' % kind] = dict(emd_sektor=se, emd_len=ln, emd_sha256=hashlib.sha256(emd).hexdigest(),
                                                tim_sektor=st, tim_len=lt, tim_sha256=hashlib.sha256(tim).hexdigest(),
                                                ems=RL.EMS_PFAD, ems_sha256=hashlib.sha256(RL.ems()).hexdigest())
    blob = open(basis, 'rb').read()
    d = EA.emd_dir(blob); dir_off, n = EA.u32(blob, 0), EA.u32(blob, 4)
    md1 = blob[d[7]:d[8]]; tim = blob[d[8]:dir_off]
    meshes = BF.md1_lesen(md1)
    kopf, mo2, V = g2_kopf_mesh(p, prot)
    maul_uv(kopf, V, p, prot)
    prot['mesh9_alt'] = dict(ecken=len(meshes[KOPF]['v']), dreiecke=len(meshes[KOPF]['tf']), vierecke=len(meshes[KOPF]['qf']))
    meshes[KOPF] = kopf
    md1_neu = BF.md1_schreiben(meshes)
    mo3 = RL.modell(G3)
    tim_neu = tim_bauen(tim, mo2, mo3, p, prot)
    if len(tim_neu) % 4:
        tim_neu += bytes(4 - len(tim_neu) % 4)
    sek = [blob[d[i]:d[i + 1]] for i in range(7)] + [md1_neu, tim_neu]
    neu_dir, o_ = [], 8
    for x in sek:
        assert len(x) % 4 == 0
        neu_dir.append(o_); o_ += len(x)
    neu = struct.pack('<II', 8 + sum(len(x) for x in sek), n) + b''.join(sek) + struct.pack('<%dI' % n, *neu_dir)
    prot['emd'] = dict(bytes=len(neu), sha256=hashlib.sha256(neu).hexdigest(), sektionen_gleich=[i for i in range(7)])
    return neu, prot, dict(V=V)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--aus', default=AUS); ap.add_argument('--basis', default=BASIS); ap.add_argument('--json')
    ap.add_argument('--name', default='EM30_V4re2.EMD'); ap.add_argument('--param', help='JSON mit PARAM-Ueberschreibungen')
    a = ap.parse_args()
    p = json.loads(json.dumps(PARAM))
    if a.param:
        p.update(json.load(open(a.param)))
    neu, prot, _ = bauen(p, a.basis)
    os.makedirs(a.aus, exist_ok=True)
    ziel = os.path.join(a.aus, a.name)
    open(ziel, 'wb').write(neu)
    open(os.path.join(a.aus, 'EM30_V4.EMD'), 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s); EM30_V4.EMD darauf' % (ziel, len(neu), prot['emd']['sha256'][:16]))
    print('TIM', prot['tim']); print('Zonen', prot['zonen']); print('G2', prot['g2']['bbox_neu'])
    if a.json:
        json.dump(prot, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
