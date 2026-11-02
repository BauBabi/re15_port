#!/usr/bin/env python3
"""Gerippe der Bahnsteig-Form: Knochenbaum, Rastpose, Grenzen, Teile-Zuordnung, Vorwaertsrechnung.

Rastpose = alle Winkel 0: Rumpf entlang +X (vorn), Glieder entlang +Y (unten), Schwanz entlang -X, +Z = links der Figur.
Konvention wie der Port (form_emd.py): M = Rx*Ry*Rz, R = R_eltern*lokal, T = R_eltern*versatz + T_eltern,
welt = Y(gier)*(R v + T) + Aktorlage, Y(g) = [c 0 s; 0 1 0; -s 0 c].
"""
import math
import numpy as np
import torch

# name, eltern, seite (+1 links, -1 rechts, 0 Mitte)
KNOCHEN = [
    ('huefte', -1, 0),
    ('schwanz1', 0, 0), ('schwanz2', 1, 0), ('schwanz3', 2, 0), ('schwanz4', 3, 0), ('schwanz5', 4, 0), ('schwanz6', 5, 0),
    ('wirbel', 0, 0),
    ('oberschenkel_l', 0, 1), ('oberschenkel_r', 0, -1),
    ('brust', 7, 0),
    ('oberarm_l', 10, 1), ('unterarm_l', 11, 1), ('hand_l', 12, 1), ('klaue_l1', 13, 1), ('klaue_l2', 13, 1),
    ('oberarm_r', 10, -1), ('unterarm_r', 16, -1), ('hand_r', 17, -1), ('klaue_r1', 18, -1), ('klaue_r2', 18, -1),
    ('hals', 10, 0), ('kopf', 21, 0), ('kiefer', 22, 0),
    ('unterschenkel_l', 8, 1), ('fuss_l', 24, 1), ('unterschenkel_r', 9, -1), ('fuss_r', 26, -1),
    ('klaue_l3', 13, 1), ('klaue_r3', 18, -1),
]
NAMEN = [k[0] for k in KNOCHEN]
IDX = {n: i for i, n in enumerate(NAMEN)}
ELTERN = [k[1] for k in KNOCHEN]
SEITE = [k[2] for k in KNOCHEN]
N = len(KNOCHEN)
for _i, _p in enumerate(ELTERN):
    assert _p < _i
assert N <= 32


def klasse(name):
    """Gemeinsamer Satz Laenge + Dicke: links = rechts, alle Klauen gleich."""
    for e in ('_l', '_r'):
        if name.endswith(e):
            name = name[:-2]
    if name.startswith('klaue'):
        return 'klaue'
    return name


KLASSEN = []
for _n in NAMEN:
    if klasse(_n) not in KLASSEN:
        KLASSEN.append(klasse(_n))

RICHTUNG = {}
for _k in KLASSEN:
    if _k in ('oberarm', 'unterarm', 'oberschenkel', 'unterschenkel'):
        RICHTUNG[_k] = (0.0, 1.0, 0.0)
    elif _k.startswith('schwanz'):
        RICHTUNG[_k] = (-1.0, 0.0, 0.0)
    else:
        RICHTUNG[_k] = (1.0, 0.0, 0.0)

# Querachsen: a = Hoehe Y bei X-Knochen, Tiefe X bei Y-Knochen; b = Breite Z. Runde Klassen: a = b.
ELLIPTISCH = ('huefte', 'wirbel', 'brust', 'kopf', 'kiefer', 'hand', 'fuss')

STATIONEN = {'huefte': 4, 'wirbel': 4, 'brust': 4, 'hals': 3, 'kopf': 4, 'kiefer': 3, 'oberarm': 5, 'unterarm': 5,
             'hand': 3, 'klaue': 4, 'oberschenkel': 5, 'unterschenkel': 5, 'fuss': 4,
             'schwanz1': 4, 'schwanz2': 4, 'schwanz3': 4, 'schwanz4': 4, 'schwanz5': 4, 'schwanz6': 4}

LAENGE0 = dict(huefte=300.0, wirbel=800.0, brust=900.0, hals=350.0, kopf=750.0, kiefer=550.0,
               oberarm=1050.0, unterarm=1050.0, hand=520.0, klaue=600.0,
               oberschenkel=1100.0, unterschenkel=1100.0, fuss=650.0,
               schwanz1=650.0, schwanz2=650.0, schwanz3=650.0, schwanz4=650.0, schwanz5=650.0, schwanz6=650.0)
VERSATZ0 = dict(huefte_hinten=300.0, schwanz_y=-120.0, schenkel_x=-60.0, schenkel_y=180.0, schenkel_z=520.0,
                schulter_x=560.0, schulter_y=0.0, schulter_z=1050.0, hals_y=-120.0, kiefer_x=250.0, kiefer_y=180.0, klaue_z=0.75)
LAENGE_GRENZE = dict(huefte=(100, 800), wirbel=(300, 1600), brust=(300, 1600), hals=(100, 900), kopf=(350, 1300), kiefer=(200, 1000),
                     oberarm=(500, 2000), unterarm=(500, 2000), hand=(250, 1000), klaue=(200, 1100),
                     oberschenkel=(500, 2000), unterschenkel=(500, 2000), fuss=(300, 900))
for _i in range(1, 7):
    LAENGE_GRENZE['schwanz%d' % _i] = (300, 1200)
VERSATZ_GRENZE = dict(huefte_hinten=(50, 800), schwanz_y=(-1000, 300), schenkel_x=(-500, 500), schenkel_y=(-200, 700), schenkel_z=(200, 1100),
                      schulter_x=(0, 1200), schulter_y=(-700, 700), schulter_z=(500, 1700), hals_y=(-700, 300), kiefer_x=(0, 500), kiefer_y=(50, 400),
                      klaue_z=(0.3, 1.1))

# Dicken: je Klasse 3 Stuetzstellen s = 0, 0.5, 1; a und b (Halbachsen). None = rund.
DICKE0 = {
    'huefte': ((720, 760, 760), (800, 880, 880)),
    'wirbel': ((760, 800, 820), (880, 950, 1000)),
    'brust': ((820, 860, 800), (1000, 1050, 950)),
    'hals': ((480, 460, 440), None),
    'kopf': ((440, 450, 340), (480, 500, 380)),
    'kiefer': ((220, 200, 120), (340, 300, 180)),
    'oberarm': ((400, 370, 320), None),
    'unterarm': ((320, 300, 300), None),
    'hand': ((200, 180, 160), (340, 400, 420)),
    'klaue': ((170, 120, 35), None),
    'oberschenkel': ((430, 400, 340), None),
    'unterschenkel': ((320, 290, 230), None),
    'fuss': ((210, 180, 140), (250, 300, 300)),
    'schwanz1': ((480, 470, 450), None), 'schwanz2': ((450, 430, 410), None), 'schwanz3': ((410, 390, 370), None),
    'schwanz4': ((370, 350, 330), None), 'schwanz5': ((330, 300, 270), None), 'schwanz6': ((270, 220, 150), None),
}
DICKE_GRENZE = (15.0, 1500.0)
# (a min, a max, b min, b max) je Klasse, Spieleinheiten. Grenzen plausibler Form, nicht gemessen.
DICKE_KLASSE = {'huefte': (350, 1100, 500, 1400), 'wirbel': (450, 1100, 600, 1500), 'brust': (450, 1100, 600, 1500), 'hals': (250, 650, 250, 650),
                'kopf': (280, 680, 300, 800), 'kiefer': (90, 340, 120, 420), 'oberarm': (280, 520, 280, 520), 'unterarm': (240, 420, 240, 420),
                'hand': (100, 320, 200, 520), 'klaue': (20, 260, 20, 260), 'oberschenkel': (170, 620, 170, 620), 'unterschenkel': (100, 480, 100, 480),
                'fuss': (90, 320, 150, 430)}
for _i in range(1, 7):
    DICKE_KLASSE['schwanz%d' % _i] = (60, 620, 60, 620)

# Winkelgrenzen in Grad (ax, ay, az) je (lo, hi), fuer die linke Seite; rechts werden ax, ay gespiegelt.
GRENZE = {
    'huefte': ((-40, 40), (0, 0), (-90, 75)),
    'wirbel': ((-25, 25), (-35, 35), (-50, 50)),
    'brust': ((-25, 25), (-35, 35), (-55, 55)),
    'hals': ((-30, 30), (-60, 60), (-75, 75)),
    'kopf': ((-30, 30), (-60, 60), (-75, 75)),
    'kiefer': ((0, 0), (0, 0), (0, 50)),
    'oberarm': ((-70, 130), (-100, 100), (-165, 165)),
    'unterarm': ((-12, 12), (-20, 20), (-155, 2)),      # Gegenprobe: Ellbogen nicht ueberstrecken (frueher +12)
    'hand': ((-70, 70), (-70, 70), (-100, 100)),
    'klaue': ((0, 0), (-30, 30), (-30, 95)),
    'oberschenkel': ((-30, 50), (-45, 45), (-150, 65)),
    'unterschenkel': ((-12, 12), (-20, 20), (-2, 165)),  # Gegenprobe: Knie nicht ueberstrecken (frueher -10)
    'fuss': ((-40, 40), (-40, 40), (-80, 80)),
}
for _i in range(1, 7):
    GRENZE['schwanz%d' % _i] = ((-15, 15), (-50, 50), (-50, 50))

# Sichtbare Teile je Bild: Nutzer-Teil -> Knochen. Nicht genannte Knochen sind in dem Bild frei (verdeckt oder ohne Beleg).
# Gegenprobe: In Bild 2 gehoert der rechte Oberarm zu keinem Teil, er bleibt unter der Rumpfkante. Der linke liegt hinter dem Kopfrand und bildet die rechte Ecke des Oberkoerpers.
TEIL_KNOCHEN = {
    1: {'tail': ['schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6'],
        'left_leg': ['oberschenkel_l', 'unterschenkel_l', 'fuss_l'],
        'right_leg': ['oberschenkel_r', 'unterschenkel_r', 'fuss_r'],
        'right_arm': ['oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3'],
        'part_of_Body': ['huefte', 'wirbel', 'brust']},
    2: {'face_and_upper_body': ['huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer', 'oberarm_l'],
        'left_arm_front': ['unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3'],
        'right_arm_front': ['unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3']},
}
BODEN_KONTAKT = {1: ['fuss_l', 'fuss_r', 'hand_r', 'hand_l'], 2: ['fuss_l', 'fuss_r', 'hand_l', 'hand_r']}

L_KEYS = list(LAENGE0)
V_KEYS = list(VERSATZ0)
D_KEYS = list(DICKE0)


def rotm(ax, ay, az):
    """Rx*Ry*Rz, Eingabe Tensor [...], Ausgabe [...,3,3]. Entspricht EA.mat_euler."""
    sx, cx = torch.sin(ax), torch.cos(ax)
    sy, cy = torch.sin(ay), torch.cos(ay)
    sz, cz = torch.sin(az), torch.cos(az)
    m = torch.stack([cz * cy, -sz * cy, sy,
                     sz * cx + cz * sy * sx, cz * cx - sz * sy * sx, -cy * sx,
                     sz * sx - cz * sy * cx, cz * sx + sz * sy * cx, cy * cx], dim=-1)
    return m.reshape(m.shape[:-1] + (3, 3))


def roty(g):
    s, c = torch.sin(g), torch.cos(g)
    z, e = torch.zeros_like(g), torch.ones_like(g)
    m = torch.stack([c, z, s, z, e, z, -s, z, c], dim=-1)
    return m.reshape(m.shape[:-1] + (3, 3))


def achsen_np(lae, ver, hand_b):
    """Eine Figur in Python-Zahlen: off[N,3], start[N,3], ende[N,3]. Linear in lae und ver bis auf klaue_z*hand_b."""
    off, start, ende = np.zeros((N, 3)), np.zeros((N, 3)), np.zeros((N, 3))
    for i, n in enumerate(NAMEN):
        k = klasse(n)
        L = lae[k]
        r = RICHTUNG[k]
        if k == 'klaue':
            w = math.radians({'1': 24.0, '2': 0.0, '3': -24.0}[n[-1]])
            r = (math.cos(w), 0.0, math.sin(w))
            off[i] = (lae['hand'], 0, ver['klaue_z'] * hand_b * {'1': 1.0, '2': 0.0, '3': -1.0}[n[-1]])
        elif n == 'huefte':
            start[i, 0] = -ver['huefte_hinten']
        elif n == 'schwanz1':
            off[i] = (-ver['huefte_hinten'], ver['schwanz_y'], 0)
        elif k.startswith('schwanz'):
            off[i, 0] = -lae['schwanz%d' % (int(k[-1]) - 1)]
        elif n == 'wirbel':
            off[i, 0] = lae['huefte']
        elif k == 'oberschenkel':
            off[i] = (ver['schenkel_x'], ver['schenkel_y'], SEITE[i] * ver['schenkel_z'])
        elif n == 'brust':
            off[i, 0] = lae['wirbel']
        elif k == 'oberarm':
            off[i] = (ver['schulter_x'], ver['schulter_y'], SEITE[i] * ver['schulter_z'])
        elif k == 'unterarm':
            off[i, 1] = lae['oberarm']
        elif k == 'hand':
            off[i, 1] = lae['unterarm']
        elif n == 'hals':
            off[i] = (lae['brust'], ver['hals_y'], 0)
        elif n == 'kopf':
            off[i, 0] = lae['hals']
        elif n == 'kiefer':
            off[i] = (ver['kiefer_x'], ver['kiefer_y'], 0)
        elif k == 'unterschenkel':
            off[i, 1] = lae['oberschenkel']
        elif k == 'fuss':
            off[i, 1] = lae['unterschenkel']
        ende[i] = (L * r[0], L * r[1], L * r[2])
    return off, start, ende


class Gerippe:
    """Rechnet mit Massen als Tensoren der Stapelgroesse B: lae[B,nL], ver[B,nV], a3[B,nD,3], b3[B,nD,3]."""

    def __init__(self, geraet='cuda', dtyp=torch.float32):
        self.g, self.d = geraet, dtyp
        lo, hi = np.zeros((N, 3)), np.zeros((N, 3))
        for i, n in enumerate(NAMEN):
            gr = GRENZE[klasse(n)]
            for j in range(3):
                a, b = gr[j]
                if SEITE[i] < 0 and j < 2:
                    a, b = -b, -a
                lo[i, j], hi[i, j] = math.radians(a), math.radians(b)
        self.lo = torch.tensor(lo, dtype=dtyp, device=geraet)
        self.hi = torch.tensor(hi, dtype=dtyp, device=geraet)
        self.frei = (self.hi > self.lo).to(dtyp)
        st_kn, st_s, st_kl, gew = [], [], [], []
        for i, n in enumerate(NAMEN):
            k = klasse(n)
            ns = STATIONEN[k]
            for s in np.linspace(0, 1, ns):
                st_kn.append(i)
                st_s.append(float(s))
                st_kl.append(D_KEYS.index(k))
                gew.append((2 * (s - .5) * (s - 1), -4 * s * (s - 1), 2 * s * (s - .5)))
        self.st_kn = torch.tensor(st_kn, device=geraet)
        self.st_s = torch.tensor(st_s, dtype=dtyp, device=geraet)
        self.st_kl = torch.tensor(st_kl, device=geraet)
        self.st_w = torch.tensor(gew, dtype=dtyp, device=geraet)
        self.K = len(st_kn)
        self.st_ns = torch.tensor([STATIONEN[klasse(NAMEN[i])] for i in st_kn], dtype=dtyp, device=geraet)
        ach_a = [(1.0, 0.0, 0.0) if RICHTUNG[klasse(n)][1] != 0 else (0.0, 1.0, 0.0) for n in NAMEN]
        self.ach_a = torch.tensor(ach_a, dtype=dtyp, device=geraet)
        self.ach_b = torch.tensor([(0.0, 0.0, 1.0)] * N, dtype=dtyp, device=geraet)
        # Lineare Abbildung (Laengen, Versaetze) -> off, start, ende. klaue_z getrennt.
        nq = len(L_KEYS) + len(V_KEYS)
        nullL, nullV = {k: 0.0 for k in L_KEYS}, {k: 0.0 for k in V_KEYS}
        M = np.zeros((3, N * 3, nq))
        for j in range(nq):
            lae, ver = dict(nullL), dict(nullV)
            if j < len(L_KEYS):
                lae[L_KEYS[j]] = 1.0
            else:
                ver[V_KEYS[j - len(L_KEYS)]] = 1.0
            o, s, e = achsen_np(lae, ver, 0.0)
            M[0, :, j], M[1, :, j], M[2, :, j] = o.ravel(), s.ravel(), e.ravel()
        self.M = torch.tensor(M, dtype=dtyp, device=geraet)
        kl = [(i, {'1': 1.0, '2': 0.0, '3': -1.0}[n[-1]]) for i, n in enumerate(NAMEN) if klasse(n) == 'klaue']
        self.kl_i = torch.tensor([i for i, _ in kl], device=geraet)
        self.kl_s = torch.tensor([s for _, s in kl], dtype=dtyp, device=geraet)
        self.i_klaue_z = V_KEYS.index('klaue_z')
        self.i_hand = D_KEYS.index('hand')
        # Ebenen des Baums fuer die Vorwaertsrechnung
        tiefe = [0] * N
        for i in range(1, N):
            tiefe[i] = tiefe[ELTERN[i]] + 1
        self.ebenen = []
        for t in range(1, max(tiefe) + 1):
            ids = [i for i in range(N) if tiefe[i] == t]
            self.ebenen.append((torch.tensor(ids, device=geraet), torch.tensor([ELTERN[i] for i in ids], device=geraet)))
        reihenfolge = [0] + [i for t in range(1, max(tiefe) + 1) for i in range(N) if tiefe[i] == t]
        self.zurueck = torch.tensor(np.argsort(reihenfolge), device=geraet)

    def achsen(self, lae, ver, b3):
        q = torch.cat([lae, ver], 1)
        B = q.shape[0]
        erg = self.M[None] @ q[:, None, :, None]    # [B,3,3N,1]
        erg = erg[..., 0].reshape(B, 3, N, 3)
        off, start, ende = erg[:, 0], erg[:, 1], erg[:, 2]
        z = ver[:, self.i_klaue_z, None] * b3[:, self.i_hand, 2, None] * self.kl_s[None]
        off = off.clone()
        off[:, self.kl_i, 2] = z
        return off, start, ende

    def querschnitte(self, a3, b3):
        a = (a3[:, self.st_kl] * self.st_w[None]).sum(-1)
        b = (b3[:, self.st_kl] * self.st_w[None]).sum(-1)
        return a, b

    def fk(self, off, wurzel, gier, ang):
        """wurzel[B,3] Welt, gier[B], ang[B,N,3] -> R[B,N,3,3], T[B,N,3] in Welt."""
        ae = ang * self.frei
        L = rotm(ae[..., 0], ae[..., 1], ae[..., 2])
        Y = roty(gier)
        Ralle, Talle = (Y @ L[:, 0])[:, None], wurzel[:, None]
        pos = torch.zeros(N, dtype=torch.long, device=self.g)
        n_bisher = 1
        for ids, elt in self.ebenen:
            Rp = Ralle[:, pos[elt]]
            Tp = Talle[:, pos[elt]]
            Rn = Rp @ L[:, ids]
            Tn = Tp + (Rp @ off[:, ids][..., None])[..., 0]
            Ralle = torch.cat([Ralle, Rn], 1)
            Talle = torch.cat([Talle, Tn], 1)
            pos[ids] = torch.arange(n_bisher, n_bisher + len(ids), device=self.g)
            n_bisher += len(ids)
        return Ralle[:, self.zurueck], Talle[:, self.zurueck]

    def primitive(self, R, T, start, ende, qa, qb):
        """Ellipsoid-Kette: Mitte c[B,K,3], Achsen e0,e1,e2[B,K,3] mal Halbachse."""
        kn, s = self.st_kn, self.st_s
        Rk = R[:, kn]
        ach = ende[:, kn] - start[:, kn]
        p_lok = start[:, kn] + ach * s[None, :, None]
        c = T[:, kn] + (Rk @ p_lok[..., None])[..., 0]
        L = ach.norm(dim=-1, keepdim=True).clamp(min=1.0)
        e_c = (Rk @ (ach / L)[..., None])[..., 0]
        e_a = (Rk @ self.ach_a[kn][None, ..., None])[..., 0]
        e_b = (Rk @ self.ach_b[kn][None, ..., None])[..., 0]
        d = L[..., 0] / (self.st_ns[None, :] - 1.0)
        rmin = torch.minimum(qa, qb)
        ca = torch.maximum(1.15 * d, 0.75 * rmin)
        return c, e_c * ca[..., None], e_a * qa[..., None], e_b * qb[..., None]


def projektion(P, auge, vorn, c, e0, e1, e2):
    """Bildellipsen je Primitiv. P[3,4], c[B,K,3]. Liefert Mitte u,v, Q=(q11,q12,q22), reff, z (Tiefe), rz."""
    Pm, Pt = P[:, :3], P[:, 3]
    h = c @ Pm.T + Pt
    w = h[..., 2].clamp(min=1.0)
    u, v = h[..., 0] / w, h[..., 1] / w

    def bild(e):
        hh = e @ Pm.T
        return (hh[..., 0] - u * hh[..., 2]) / w, (hh[..., 1] - v * hh[..., 2]) / w
    x0, y0 = bild(e0)
    x1, y1 = bild(e1)
    x2, y2 = bild(e2)
    E11 = x0 * x0 + x1 * x1 + x2 * x2 + 1e-3
    E12 = x0 * y0 + x1 * y1 + x2 * y2
    E22 = y0 * y0 + y1 * y1 + y2 * y2 + 1e-3
    det = (E11 * E22 - E12 * E12).clamp(min=1e-3)
    q11, q12, q22 = E22 / det, -E12 / det, E11 / det
    reff = det.pow(0.25)
    z = (c - auge) @ vorn
    rz = torch.sqrt((e0 @ vorn) ** 2 + (e1 @ vorn) ** 2 + (e2 @ vorn) ** 2 + 1e-6)
    return u, v, q11, q12, q22, reff, z, rz


def abstand(px, py, u, v, q11, q12, q22, reff):
    """Vorzeichen-Abstand je Primitiv und Pixel. px, py [P]; u.. [B,K]. Ergebnis [B,K,P], Pixel; f [B,K,P] (1 = Rand)."""
    x = px[None, None, :] - u[..., None]
    y = py[None, None, :] - v[..., None]
    qx = q11[..., None] * x + q12[..., None] * y
    qy = q12[..., None] * x + q22[..., None] * y
    f = torch.sqrt((x * qx + y * qy).clamp(min=0) + 1e-8)
    g = torch.sqrt(qx * qx + qy * qy + 1e-12)
    faktor = torch.minimum(f / g, 2.0 * reff[..., None] + 1.0)
    return (f - 1.0) * faktor, f
