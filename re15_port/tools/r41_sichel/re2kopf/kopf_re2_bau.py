"""SF V5 (Bauer, Kopf aus RE2, Runde 1): Kopf der Sichel-Form = RE2-Birkin-G2-Kopf (CDEMD0.EMS kind 0x31, Mesh 3) anstelle von Mesh 9
(KOPF_V4) im EMD der Runde 3 (EM30_V4ot3.EMD); Maul = Zahn-Kachel des G3-Kopfes (kind 0x33, TIM page 0 u 2-13 v 5-40) auf den
unteren Vorderflaechen des G2-Kopfes; Haarseite ((W)-links, z > 0) mit dunkleren Eintraegen DERSELBEN G2-Palettenzeile angedunkelt.

    python kopf_re2_bau.py [--aus DIR] [--basis EMD] [--json PROT]      -> DIR/EM30_V4re2.EMD (+ EM30_V4.EMD), Protokoll

Nutzer (2026-11-07, woertlich): "Vielleicht kannst du dir auch einen Birkin aus Resident Evil 2 nehmen. Ich nehme an die duerften aehnlich
aufgebaut sein die anderen Formen...." und "bei den KI Bildern kann man so verdammt gut sehen wie das verschwommene Bild eigentlich aussehen
muss.... Man sieht alles einwandfrei! Das Auge die Haare, den Mund, den Schwanz...."
Leser A (SF_belege/re2kopf/00_koepfe_re2_vs_ki.png, mess.json): naechster Einzelkandidat = G2-Kopf (0x31 Mesh 3; dE00 zu KI-Front 3,0, zu
Beschreibung1 6,6, zu Beschreibung2 6,5); Zaehne nur im G3-Kopf (0x33, Zahn-Anteil 0,0225). Leser B (KI_LESUNG.md): Kuppel mit Schnauze,
Achse 30-35 Grad vorn-unten, Maul an der Unterseite mit zwei Zahnreihen, ein Auge oben vorn (B)-links bis mittig, (B)-rechte Haelfte
9-21 L* dunkler mit weichem Uebergang, roter Kamm.

Herkunft (byte-true, Zaehlung wie re2_leser.rec): G2 EMD = CDEMD0.EMS Sektor 2944 (193124 B, sha256 bb966cd5...), MD1 dir[7], Mesh 3 (48 Ecken,
48 Normalen, 56 Dreiecke, 14 Vierecke, UV page 128 / CLUT 480 = Zeile 0 der G2-TIM); G2 TIM = Sektor 2895 (99872 B), 384 x 256 8 bpp,
Texel u 0-125 v 0-89, CLUT-Zeile 0 (y 480). G3 TIM = Sektor 3065 (99872 B), Zahn-Kachel u 2-13 v 5-40, CLUT-Zeile 0.
Transformation (PORT-WAHL nach Messblatt-Kopfbox K10 und Leser B, s. PARAM): q = Rz(pitch) . Ry(yaw) . (skal * v) + versatz im Raum des Ref2-Knochens
9 (rel (111,-971,-133) unveraendert; Skelett, Clips, EDD unveraendert). Normalen mitgedreht (bei anisotroper Skalierung invers-transponiert).
Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante.
"""
import os, sys, json, struct, argparse, hashlib
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, HIER); sys.path.insert(0, TOOLS); sys.path.insert(0, os.path.join(TOOLS, 'r41_sichel', 'v4'))
import re2_leser as RL
import emd_ansichtsblatt as EA
import birkin_formen as BF
from skimage.color import rgb2lab

SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
BASIS = os.environ.get('RE15_V5_BASIS', os.path.join(SP, 'sfv4', 'emd', 'EM30_V4ot3.EMD'))     # sha256 a1bc9309 (Runde 3)
AUS = os.path.join(SP, 'sfv4', 'emd')
G2, G3 = 0x31, 0x33
KOPF = 9                        # Mesh/Knochen des Kopfes im EM30 (Ref2)
# TIM-Orte im EM30-Atlas (Ref2: TPage 128 Zeilen 155-255 frei, TPage 130 x 256-319 Zeilen 126-255 frei; V4-Schwanz TPage 130 u 64-127)
G2_ORT = dict(page=128, u0=0, v0=160, u=(0, 125), v=(0, 89))        # G2-Texel u 0-125 v 0-89 -> x 0-125, y 160-249
# G3-Zahn-Kachel u 2-13 v 5-40 -> x 256-267, y 200-235 (Spalten 0-1 und 14-20 der Kachel tragen den blauen Stirnstreifen des G3, b* < -15; probe9)
G3_ORT = dict(page=130, u0=0, v0=200, u=(2, 13), v=(5, 40))
ZEILE_G2, ZEILE_G3 = 486, 487   # neue CLUT-Zeilen (Ref2 hat 480-485); 8 Zeilen gesamt <= 16 (Port-Texturhoehe 2048 <= 4096)
# Maulflaechen des G2-Kopfes (Indizes in Mesh 3, T = Dreieck, Q = Viereck): die unteren Vorderflaechen unter den Augen (y > -140 vor der
# Drehung, Normale +x), s. probe2 / Protokoll 'maul_flaechen'
MAUL_T = [25, 26, 34, 35, 42, 44, 28, 29, 22, 43, 5]
MAUL_Q = []
# Parameter = Ergebnis der Suche an den Messblatt-Fenstern (Scratchpad re2kopf_bau/grid1-4.log, 2026-11-07; Masse kopf_re2_mess, Render b2
# Pose "135" / "85"): Skala gleichmaessig 1,50 (K10 0,165 x 0,176 H; Leser A rechnete 1,577 auf 724 E. Kopfhoehe, Leser B Kuppel 0,13-0,15 x
# 0,15 H, mit Maul 0,19-0,22 H - gewaehlt ist das Messblatt-Fenster 0,14-0,18 x 0,15-0,20 H, weil die Riegel T-R2 dort messen); Neigung
# 10 Grad nach unten (Leser B 30-35 Grad: ab 20 Grad kippt die Schnauze aus der Kamera 50A0/12, Kopfbox-Hoehe 0,137 H und das Gesicht
# der Beschreibung1 ist nicht mehr zu sehen - Beschreibung1 ist bindend, Offen im Dossier); Gieren -34 Grad zur (W)-linken Seite (das
# G2-Auge sitzt bei z -110 = 0,6 der halben Kopfbreite weit aussen; ohne Gieren laege es 0,08 H links der Kopfmitte, Messblatt 0,035-0,065);
# Versatz (140, +10, 0) E. (S2 Augenhoehe 0,76-0,82 H ueber der Sohle; mit y -20 lag das Auge bei 0,8203 H). PORT-WAHL, kein RE-Wert.
PARAM = dict(
    skal=(1.50, 1.50, 1.50),      # gleichmaessig
    pitch=10.0,                   # Grad, positiv = Schnauze nach unten (um die lokale z-Achse)
    yaw=-34.0,                    # Grad, positiv = Schnauze nach (W)-rechts (um die lokale y-Achse)
    versatz=(140.0, 10.0, 0.0),   # E. im Knochen-9-Raum, nach der Drehung (y +10 = 10 E. tiefer als der Knochenursprung)
    # Haar (Nutzer: "Dann rechts Seite sieht man Haare - braeunlich"; Leser B D4: (B)-rechte Haelfte 9-21 L* dunkler, weicher Uebergang,
    # keine Straehnen): (W)-linke Kopfhaelfte z > z0 .. z1 glatt auf faktor x L* (Lab-naechster Eintrag DERSELBEN G2-Zeile); das (W)-rechte
    # Auge (z -98..-125) bleibt unberuehrt
    haar=dict(z0=-80.0, z1=0.0, faktor=0.65),
    # Nase (Nutzer: "Linke Seite ist so graeulich. Bis Nase"; Messblatt K7 Nase mitteldunkel 18,2 L*; Beschreibung2 KS2 dunkle Vorderkante):
    # die leuchtende Mittelspalte der G2-Textur (u 41-60, der Gesichtsspalt) wird mitteldunkel - sonst laege der hellste Punkt des Gesichts
    # in der Mitte statt am Auge (Messblatt Abschnitt 2: "der hellste Kopfpunkt ist der Augenfleck"); Riegel KS2 (Kopf-Bildpunkte) <= Median - 5
    nase=dict(u0=41, u1=60, v_ab=0.0, rampe=1.0, faktor=0.50),
    # Schnauzenspitze (Beschreibung2: dunkle Haarkante an der Kopf-Vorderkante vor dem Auge, Fenster KS2; Messblatt Abschnitt 3 Streifen L* 12,6):
    # Flaechen mit Mittelpunkt x > x0 .. x1 (Q0 173, T33/T31 177, T27 184, T0 192, T32 196, T2 204, T21 223 - die vorderste Kante des Keils;
    # das Fenster KS2 zeigt in Pose 85 zu 0,93 die Flaeche Q0, Scratchpad re2kopf_bau/probe6) auf faktor x L*; das Auge
    # (T19/T20/T41, x 78-119) liegt dahinter
    spitze=dict(x0=150.0, x1=175.0, faktor=0.50),
)


def glatt(t):
    t = np.clip(t, 0.0, 1.0)
    return t * t * (3 - 2 * t)


def rz(grad):
    a = np.radians(grad); c, s = np.cos(a), np.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def ry(grad):
    """Gieren um die lokale y-Achse; positiv dreht die Schnauze (+x) nach (W)-rechts (z < 0)."""
    a = np.radians(grad); c, s = np.cos(a), np.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rot(p):
    return rz(p['pitch']) @ ry(p.get('yaw', 0.0))


def lab_pal(pal_rgb):
    return rgb2lab(np.asarray(pal_rgb, float).reshape(-1, 1, 3) / 255.0).reshape(-1, 3)


def g2_kopf_mesh(p, prot):
    """Mesh 3 des G2 als birkin_formen-Mesh-Dict (v, n, tf, tuv, qf, quv), transformiert; UVs auf den EM30-Atlas gelegt."""
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
    for i, uv in enumerate(m.tuv):
        uv = list(uv)
        for k in range(3):
            uv[3 * k] = uv[3 * k] - G2_ORT['u'][0] + G2_ORT['u0']; uv[3 * k + 1] = uv[3 * k + 1] - G2_ORT['v'][0] + G2_ORT['v0']
        uv[2], uv[5] = ZEILE_G2 << 6, G2_ORT['page']
        out['tuv'].append(uv)
    for i, uv in enumerate(m.quv):
        uv = list(uv)
        for k in range(4):
            uv[3 * k] = uv[3 * k] - G2_ORT['u'][0] + G2_ORT['u0']; uv[3 * k + 1] = uv[3 * k + 1] - G2_ORT['v'][0] + G2_ORT['v0']
        uv[2], uv[5] = ZEILE_G2 << 6, G2_ORT['page']
        out['quv'].append(uv)
    prot['g2'] = dict(ecken=len(m.tv), normalen=len(m.tn), dreiecke=len(m.tf), vierecke=len(m.qf),
                      bbox_quelle=[V.min(0).tolist(), V.max(0).tolist()], bbox_neu=[Q.min(0).tolist(), Q.max(0).tolist()],
                      uv_quelle=sorted({(u[5], u[2]) for u in list(m.tuv) + list(m.quv)}))
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


def z_karte(mo2, V, shape, achse=2):
    """Je Texel des G2-Kopfbereichs die (unskalierte) Modell-Koordinate (achse 2 = z, 0 = x; Flaechenmittel ueber die abdeckenden Flaechen) -
    fuer das Haargewicht (z) und die Schnauzenspitze (x)."""
    m = mo2.meshes[3]
    summe = np.zeros(shape, float); zahl = np.zeros(shape, float)
    for faces, uvs, n in ((m.tf, m.tuv, 3), (m.qf, m.quv, 4)):
        for f, uv in zip(faces, uvs):
            idx = [f[2 * k + 1] for k in range(n)]
            pts = np.array([(uv[3 * k], uv[3 * k + 1]) for k in range(n)], np.float32)
            zz = V[idx][:, achse]
            if n == 4:
                pts = pts[[0, 1, 3, 2]]
            mk = np.zeros(shape, np.uint8); cv2.fillPoly(mk, [pts.astype(np.int32)], 1)
            # Flaechenmittel (die Texel einer Flaeche liegen nah beieinander; weicher Uebergang ueber glatt())
            summe[mk > 0] += float(zz.mean()); zahl[mk > 0] += 1
    z = np.where(zahl > 0, summe / np.maximum(zahl, 1), np.nan)
    return z


# Entsaettigung in den angedunkelten Zonen: a*/b* x (1 - ENTSAETT x w). Ohne sie blieben die roten Adern und das (W)-linke Auge (T49/T55/T38/
# T40, G2 u 60-79 v 17-34) als roter Glanz Bild-rechts oben stehen (Lauf c6: 34 rote Bildpunkte a* > 25 dort) - Beschreibung1: das zweite Auge
# liegt unter den Haaren; Leser B D4: (B)-rechte Haelfte braun-grau 17-24 / 6-7 / 3-4, D3: "ein Auge, Spiegelstelle dunkel"; Messblatt Nase
# 18,2 / 7,1 / 1,3. PORT-WAHL.
ENTSAETT = 0.6


def haar_andunkeln(tex_g2, pal_g2, z, p, prot, x=None):
    """(W)-linke Kopfhaelfte (Haar), Mittelspalte (Nase) und Schnauzenspitze dunkler und entsaettigt: Ziel-Lab = (L* x f, a* x g, b* x g)
    mit f = 1 - (1 - faktor) x w, g = 1 - ENTSAETT x w; Haar: w = glatt((z - z0) / (z1 - z0)); Nase: w = Rampe in v ab v_ab in den Spalten
    u0-u1 (G2-Zaehlung); Spitze: w = glatt((x - x0) / (x1 - x0)).
    Je Texel der naechste Eintrag DERSELBEN G2-Zeile (Lab-Abstand). Index 0 (transparent) bleibt. Rueckgabe neue Indizes + Umfang."""
    lab = lab_pal(pal_g2)
    h = p['haar']; nz = p.get('nase')
    w = glatt((z - h['z0']) / (h['z1'] - h['z0'])); w = np.where(np.isnan(z), 0.0, w)
    fak = np.full(w.shape, h['faktor'])
    if nz:
        vv = np.arange(tex_g2.shape[0], dtype=float)[:, None] * np.ones((1, tex_g2.shape[1]))
        wn = glatt((vv - nz['v_ab']) / nz['rampe']); wn[:, :nz['u0']] = 0; wn[:, nz['u1'] + 1:] = 0
        fak = np.where(wn > w, nz['faktor'], fak); w = np.maximum(w, wn)
    sp = p.get('spitze')
    if sp and x is not None:
        ws = glatt((np.where(np.isnan(x), -1e9, x) - sp['x0']) / (sp['x1'] - sp['x0']))
        fak = np.where(ws > w, sp['faktor'], fak); w = np.maximum(w, ws)
    neu = tex_g2.copy()
    kand = np.array([i for i in range(256) if i != 0 and (pal_g2[i] != 0).any()])
    ys, xs = np.nonzero((w > 0.02) & (tex_g2 != 0))
    dL = []
    for y, x in zip(ys, xs):
        i = int(tex_g2[y, x]); L, a, b = lab[i]
        f = 1.0 - (1.0 - fak[y, x]) * w[y, x]
        g = 1.0 - ENTSAETT * w[y, x]
        ziel = np.array([L * f, a * g, b * g])
        kd = kand[lab[kand, 0] <= L + 1e-6]          # nie heller als die Quelle (Riegel T-R1b)
        d = np.linalg.norm(lab[kd] - ziel, axis=1)
        j = int(kd[np.argmin(d)])
        neu[y, x] = j; dL.append(lab[j, 0] - L)
    dL = np.array(dL) if len(dL) else np.zeros(1)
    prot['haar'] = dict(param=dict(h), nase=dict(nz) if nz else None, spitze=dict(sp) if sp else None, entsaett=ENTSAETT, texel_gewichtet=int(len(ys)), texel_geaendert=int((neu != tex_g2).sum()),
                        dL_mittel=float(dL.mean()), dL_p10_p90=[float(np.percentile(dL, 10)), float(np.percentile(dL, 90))],
                        w_voll=int((w >= 0.98).sum()))
    return neu, w


def tim_bauen(tim_alt, tex_g2_neu, mo2, mo3, prot):
    """EM30-TIM: Texel der ot3-Kopfflaechen (TPage 128 v 160-223) auf 0 zurueck, G2-Kopftexel nach G2_ORT, G3-Zahn-Kachel nach G3_ORT;
    CLUT-Block um die Zeilen 486 (= G2 Zeile 0) und 487 (= G3 Zeile 0) verlaengert, Zeilen 480-485 bytegleich."""
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
    # G2
    o = G2_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']
    hh, ww = tex_g2_neu.shape
    pix[o['v0']:o['v0'] + hh, x0:x0 + ww] = tex_g2_neu
    # G3 Kachel
    o3 = G3_ORT; x3 = 128 * (o3['page'] & 0xF) + o3['u0']
    kach = mo3.tex[o3['v'][0]:o3['v'][1] + 1, o3['u'][0]:o3['u'][1] + 1]
    pix[o3['v0']:o3['v0'] + kach.shape[0], x3:x3 + kach.shape[1]] = kach

    # CLUT: Rohwerte (15 Bit) der RE2-TIMs, bytegleich
    def clut_roh(mo):
        t = mo.tim; cs_, = struct.unpack_from('<I', t, 8); cw_, ch_ = struct.unpack_from('<HH', t, 16)
        return np.frombuffer(t[20:20 + 2 * cw_ * ch_], '<u2').reshape(ch_, cw_)
    z2, z3 = clut_roh(mo2)[0], clut_roh(mo3)[0]
    assert ch == 6 and cy == 480, (ch, cy)
    alle = np.vstack([c, z2[None], z3[None]]).astype('<u2')
    blk = struct.pack('<I4H', 12 + alle.size * 2, cx, cy, cw, alle.shape[0]) + alle.tobytes()
    neu = bytes(tim[:8]) + blk + bytes(tim[ende:ende + 12]) + pix.tobytes() + bytes(tim[ende + 12 + breite * ih:])
    prot['tim'] = dict(clut_zeilen=int(alle.shape[0]), zeilen_neu={ZEILE_G2: 'G2 0x31 CLUT-Zeile 0 (y 480)', ZEILE_G3: 'G3 0x33 CLUT-Zeile 0 (y 480)'},
                       g2_ort=dict(G2_ORT), g3_ort=dict(G3_ORT), alt_kopf_texel_geloescht=alt_kopf,
                       g2_texel=int((tex_g2_neu != 0).sum()), g3_texel=int((kach != 0).sum()),
                       port_texturhoehe=int(ih * alle.shape[0]))
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
    # Texel
    o = G2_ORT
    tex_g2 = mo2.tex[o['v'][0]:o['v'][1] + 1, o['u'][0]:o['u'][1] + 1].copy()
    z = z_karte(mo2, V, tex_g2.shape); x = z_karte(mo2, V, tex_g2.shape, achse=0)
    tex_neu, w = haar_andunkeln(tex_g2, mo2.pal[0], z, p, prot, x)
    mo3 = RL.modell(G3)
    tim_neu = tim_bauen(tim, tex_neu, mo2, mo3, prot)
    if len(tim_neu) % 4:
        tim_neu += bytes(4 - len(tim_neu) % 4)
    sek = [blob[d[i]:d[i + 1]] for i in range(7)] + [md1_neu, tim_neu]
    neu_dir, o_ = [], 8
    for x in sek:
        assert len(x) % 4 == 0
        neu_dir.append(o_); o_ += len(x)
    neu = struct.pack('<II', 8 + sum(len(x) for x in sek), n) + b''.join(sek) + struct.pack('<%dI' % n, *neu_dir)
    prot['emd'] = dict(bytes=len(neu), sha256=hashlib.sha256(neu).hexdigest(), sektionen_gleich=[i for i in range(7)])
    return neu, prot, dict(tex_g2=tex_g2, tex_neu=tex_neu, z=z, w=w)


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
    print('TIM', prot['tim']); print('Haar', prot['haar']); print('G2', prot['g2']['bbox_neu'])
    if a.json:
        json.dump(prot, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
