# -*- coding: utf-8 -*-
"""Kandidat C: Generator. Geometrie (teile.py) + Knochen + Karten + Textur -> FORM.EMD, FORM.TIM, lage.json."""
from atlas import *
import pickle, subprocess, sys, time

AUS = HIER

# Knochen: (Name, Eltern, Gelenk im Aktorraum Pose 1). Vorgabe Tick: 10 Rumpf, 13 und 18 Enden der Schlagglieder am Ende N.
def knochen_tafel(T):
    tn = {t['name']: t for t in T}
    p = p3
    zm = 60.0
    fr_h = p(403, 262, 6760); fr_k = p(399, 298, 6810); fr_a = p(399, 350, 6950); fr_z = p(418, 362, 7000)

    def fl(q):
        return np.array([q[0] + 420.0, q[1], 2 * zm - q[2]])
    k = [('rumpf_h', -1, (-520, -1900, 0)),
         ('fr_huefte', 0, (-1150, -2150, 660)), ('fr_bein', 1, fr_k), ('fr_fuss', 2, fr_a), ('fr_zeh', 3, fr_z),
         ('fl_huefte', 0, fl((-1150, -2150, 660))), ('fl_bein', 5, fl(fr_k)), ('fl_fuss', 6, fl(fr_a)), ('fl_zeh', 7, fl(fr_z)),
         ('rumpf_m', 0, (-120, -1900, 0)),
         ('rumpf_v', 9, (320, -1900, 40)),
         ('nr_ober', 10, p(325, 300, 5860)), ('nr_unter', 11, p(327, 350, 5360)), ('nr_hand', 12, (560, -230, 330)),
         ('hals', 10, (860, -1740, 260)), ('vorsprung', 14, tuple(HAKEN_J)),
         ('nl_ober', 10, p(258, 294, 5040)), ('nl_kugel_o', 16, p(232, 345, 4770)), ('nl_kugel_u', 17, p(226, 380, 4700)),
         ('nl_unter', 16, p(268, 346, 5000)),
         ('kiefer', 14, tuple(tn['kiefer']['V'][np.array(tn['kiefer']['ringe'][0])].mean(0)))]
    return k


TEIL_ZU_KNOCHEN = {'fr_huefte': 1, 'fr_bein': 2, 'fr_fuss': 3, 'fr_zeh': 4, 'fl_huefte': 5, 'fl_bein': 6, 'fl_fuss': 7, 'fl_zeh': 8,
                   'nr_ober': 11, 'nr_unter': 12, 'nr_hand': 13, 'hals': 14, 'vorsprung': 15, 'nl_ober': 16, 'nl_kugel_o': 17, 'nl_kugel_u': 18, 'nl_unter': 19, 'kiefer': 20}
SCHNITT_H, SCHNITT_V, UEBERLAPP = -420.0, 240.0, 130.0


POSE2_FL = (-5.5, -5.0, -12.0)


def posen_definieren(kn):
    """Pose 1 = Bindung. Pose 2 weicht nur ab, wo Bild 2 es verlangt. Ruhe = Bindung, Fuss FL auf der Spiegellage."""
    p1 = Pose(kn)
    d2 = {1: dreh('x', -8) @ dreh('z', 6),      # Bein FR: Fuss aus dem Boden-Bereich BO heraus, hinter das Feuer
          5: dreh('x', POSE2_FL[2]) @ dreh('z', POSE2_FL[0]), 6: dreh('z', POSE2_FL[1]),      # Bein FL: deckt V3, Fuss vom Boden BO weg (Suche fl_such3.py)
          7: (dreh('x', POSE2_FL[2]) @ dreh('z', POSE2_FL[0]) @ dreh('z', POSE2_FL[1])).T,    # Fuss FL bleibt flach
          16: dreh('z', 8), 19: dreh('z', -7),
          15: yrot_grad(HAKEN_DREH),             # Haken: rosa Seite schaut in Bild 2 weg
          20: dreh('z', 78)}                     # Kiefer in Bild 2 eingezogen: Z3 und Z4 fehlen dort
    fr = os.path.join(HIER, 'pose2_fr.json')
    if os.path.exists(fr):                       # Bein FR: Stand aus Bild 1 fiele auf sicheren Raum und Boden, Suche fr_such.py
        w = json.load(open(fr))
        r1 = dreh('y', w['a1y']) @ dreh('x', w['a1x']) @ dreh('z', w['a1z'])
        r2 = dreh('z', w['a2z'])
        d2[1], d2[2], d2[3] = r1, r2, (r1 @ r2).T
    nl = os.path.join(HIER, 'pose2_nl.json')
    if os.path.exists(nl):                       # Bein NL in Bild 2 angehoben, Suche nl_such.py
        w = json.load(open(nl))
        d2[16] = dreh('x', w['a16x']) @ dreh('z', w['a16z']) @ dreh('y', 45.0)        # Rolle: die Marken aus Bild 1 schauen in Bild 2 weg
        d2[17] = dreh('z', w['a17z'])
        d2[18] = dreh('z', w['a18z'])
        d2[19] = dreh('x', w['a19x']) @ dreh('z', w['a19z'])   # Saeule NL und Innenglied: Luecke V2 schliessen
    p2 = Pose(kn, d2)
    r = Pose(kn)
    return p1, p2, r


def form_bauen(T, karten, kn, posen):
    """form_emd-Beschreibung. Rumpf wird nach X auf drei Knochen geteilt, Flaechen am Schnitt liegen in beiden Teilen."""
    form = FE.neu()
    J = [np.array(k[2], float) for k in kn]
    for i, (nm, el, g) in enumerate(kn):
        v = J[i] if el < 0 else J[i] - J[el]
        FE.knochen_dazu(form, nm, el, [int(round(c)) for c in v])

    def normalen_licht(Vn):
        """Disc-Licht ROOM5080 steht fast senkrecht ueber der Figur. Damit Textur mal Licht dem gemessenen Faktor entspricht, wird der Hoehenanteil der Normalen
        gestaucht: senkrecht stehende Flaechen bekommen N*L rund 0,36 (Faktor 0,49), Oberseiten laufen nicht weiss zu, Unterseiten bleiben im Umgebungslicht."""
        up = -Vn[:, 1]
        u2 = np.where(up >= 0, 0.36 + 0.12 * up, 0.36 + 0.9 * up)
        u2 = np.clip(u2, -0.98, 0.98)
        h = Vn[:, [0, 2]]
        lg = np.linalg.norm(h, axis=1, keepdims=True) + 1e-6
        h = h / lg * np.sqrt(1 - u2 * u2)[:, None]
        return np.stack([h[:, 0], -u2, h[:, 1]], 1)

    def ablegen(bone, V, Vn, flaechen):
        """flaechen = Liste (Ring, uv). Ecken je Knochen nur einmal, Normalen ebenso."""
        m = form['meshes'][bone]
        emap, nmap = {}, {}
        for ring, uv in flaechen:
            e, nn, tx = [], [], []
            for vi, q in zip(ring, uv):
                if vi not in emap:
                    emap[vi] = FE.ecke(m, V[vi] - J[bone])
                    nmap[vi] = FE.normale(m, Vn[vi])
                e.append(emap[vi]); nn.append(nmap[vi]); tx.append((int(round(q[0])), int(round(q[1]))))
            if len(ring) == 3:
                FE.dreieck(m, e, nn, tx, 0)
            else:
                z = (0, 1, 3, 2)
                FE.viereck(m, [e[i] for i in z], [nn[i] for i in z], [tx[i] for i in z], 0)

    for t, k in zip(T, karten):
        Vn = normalen_licht(eck_normalen(t))
        ff = flaechen_uv(t, k)
        if t['name'] == 'rumpf':
            V = t['V']
            cx = [float(V[list(r)][:, 0].mean()) for r, _ in ff]
            ablegen(0, V, Vn, [f for f, c in zip(ff, cx) if c < SCHNITT_H + UEBERLAPP])
            ablegen(9, V, Vn, [f for f, c in zip(ff, cx) if SCHNITT_H - UEBERLAPP <= c < SCHNITT_V + UEBERLAPP])
            ablegen(10, V, Vn, [f for f, c in zip(ff, cx) if c >= SCHNITT_V - UEBERLAPP])
        else:
            ablegen(TEIL_ZU_KNOCHEN[t['name']], t['V'], Vn, ff)
    for nm, p in posen:
        FE.clip_dazu(form, nm, [FE.bild(form, wurzel=tuple(int(round(c)) for c in p.T[0]), winkel=p.winkel())])
    FE.orientiere(form)
    return form


def tim_genau(rgb):
    """Bild -> 8 bpp ohne Mittelwerte: die Stufen der Malerei bleiben exakt. Nur wenn es mehr als 255 Farben sind, werden die seltensten auf die naechste gelegt."""
    q = np.clip((np.round(rgb.astype(np.float32) / 8.0)).astype(np.int32), 0, 31)
    code = q[..., 0] | (q[..., 1] << 5) | (q[..., 2] << 10)
    u, inv, n = np.unique(code, return_inverse=True, return_counts=True)
    if len(u) > 255:
        rang = np.argsort(-n)
        halten = rang[:255]
        fb = np.stack([u & 31, (u >> 5) & 31, (u >> 10) & 31], 1).astype(float)
        ziel = np.arange(len(u))
        for i in rang[255:]:
            d = ((fb[halten] - fb[i]) ** 2).sum(1)
            ziel[i] = halten[int(d.argmin())]
        neu = {int(a): i for i, a in enumerate(halten)}
        inv2 = np.vectorize(lambda v: neu[int(ziel[v])])(np.arange(len(u)))[inv]
        cl = [fb[a] for a in halten]
    else:
        inv2 = inv
        cl = [np.array([v & 31, (v >> 5) & 31, (v >> 10) & 31], float) for v in u]
    px = (inv2.reshape(code.shape) + 1).astype(np.uint8)
    farben = [(0, 0, 0)] + [tuple(int(c) << 3 for c in f) for f in cl]
    return px, FE.clut_aus_rgb(farben, transparent0=True), len(cl)


def schreiben(form, rgb, maske, name='FORM'):
    emd = FE.schreibe_emd(form)
    px, clut, nf = tim_genau(rgb)
    print('Farben', nf)
    tim = FE.tim_schreiben(px, [clut])
    open(os.path.join(AUS, name + '.EMD'), 'wb').write(emd)
    open(os.path.join(AUS, name + '.TIM'), 'wb').write(tim)
    return emd, tim


def lage_json(name='FORM', pose1=None, pose2=None):
    j = dict(name='C', emd=name + '.EMD', tim=name + '.TIM', bild1=dict(lage=list(L1), gier=G1, clip=0, bild=0), bild2=dict(lage=list(L2), gier=G2, clip=1, bild=0), ruhe=dict(clip=2, bild=0))
    json.dump(j, open(os.path.join(AUS, 'lage.json'), 'w', encoding='utf-8'), indent=1)


def pruefen(tag):
    aus = os.path.join(AUS, 'lauf', tag)
    os.makedirs(aus, exist_ok=True)
    env = dict(os.environ, PYTHONIOENCODING='utf-8')
    r = subprocess.run([sys.executable, os.path.join(TOOLS, 'form_pruefung.py'), 'pruefe', os.path.join(AUS, 'lage.json'), '--aus', aus], capture_output=True, text=True, env=env, encoding='utf-8', errors='replace')
    if r.returncode:
        print(r.stdout[-2000:], r.stderr[-2000:])
    return aus


def dreh(achse, grad):
    a = math.radians(grad)
    c, s = math.cos(a), math.sin(a)
    if achse == 'x':
        return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])
    if achse == 'y':
        return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


class Pose:
    """drehung = {Knochen: 3x3 lokal}, versatz = Wurzelversatz. Liefert fuer jeden Knochen R, T: p_pose = R (p_bind - J) + T."""

    def __init__(self, kn, drehung=None, versatz=(0, 0, 0)):
        self.kn = kn
        self.J = [np.array(k[2], float) for k in kn]
        self.d = drehung or {}
        self.versatz = np.array(versatz, float)
        n = len(kn)
        self.R = [None] * n
        self.T = [None] * n
        for i, k in enumerate(kn):
            L = self.d.get(i, np.eye(3))
            e = k[1]
            if e < 0:
                self.R[i], self.T[i] = L, self.J[i] + self.versatz
            else:
                self.R[i] = self.R[e] @ L
                self.T[i] = self.R[e] @ (self.J[i] - self.J[e]) + self.T[e]

    def trafo(self, T):
        namen = [t['name'] for t in T]

        def f(ti, V):
            nm = namen[ti]
            if nm == 'rumpf':
                b = 10
            else:
                b = TEIL_ZU_KNOCHEN[nm]
            return (V - self.J[b]) @ self.R[b].T + self.T[b]
        return f

    def winkel(self):
        return [list(FE.winkel_aus_matrix(self.d[i])) if i in self.d else [0, 0, 0] for i in range(len(self.kn))]
