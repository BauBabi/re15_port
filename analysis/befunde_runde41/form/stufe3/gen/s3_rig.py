"""Stufe 3, Bahnsteig-Form J: Gerippe, Posen, Mischen, Bein-IK, Ansichten.

KEIN RE-WERT in dieser Datei: Posen und Bewegungen sind Gestaltung (PORT-WAHL) fuer die neue Form. Format und Pose-Rechnung
folgen re15_skel_compute_pose (form_emd.pose_aus_bild): R = R_eltern * lokal, T = R_eltern * versatz + T_eltern, lokal = Rx*Ry*Rz.

Gerippe J (gelesen aus varianten/J_dicker/FORM.EMD, Namen aus J_dicker/bau.json atlas):
  0 Huefte (Wurzel)   1-6 Schwanz (Kette nach -X)   7 Wirbel   10 Brust (BB_TEIL_RUMPF)
  8/24/25 Bein links: Oberschenkel, Unterschenkel, Fuss     9/26/27 Bein rechts
  11/12/13 Arm links: Oberarm, Unterarm, Hand (Teil 13 = Hieb Seite 1)   14 15 28 30 Klauen links
  16/17/18 Arm rechts: Oberarm, Unterarm, Hand (Teil 18 = Hieb Seite 0)  19 20 29 31 Klauen rechts
  21 Hals  22 Kopf  23 Kiefer
Achsen: Vorderseite +X, Y nach unten, Z links (+Z = links, Bein 8 bei z +558).
"""
import os, sys, math, json
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, '../../../../..'))
FORM_DIR = os.path.join(WURZEL, 'analysis/befunde_runde41/form')
J_DIR = os.path.join(FORM_DIR, 'varianten/J_dicker')
sys.path.insert(0, os.path.join(WURZEL, 're15_port/tools'))
import form_emd as FE           # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

N = 32
HUEFTE, WIRBEL, BRUST, HALS, KOPF, KIEFER = 0, 7, 10, 21, 22, 23
SCHWANZ = [1, 2, 3, 4, 5, 6]
BEIN_L, BEIN_R = [8, 24, 25], [9, 26, 27]
ARM_L, ARM_R = [11, 12, 13], [16, 17, 18]
KLAUEN_L, KLAUEN_R = [14, 15, 28, 30], [19, 20, 29, 31]
NAMEN = {0: 'Huefte', 1: 'Schwanz1', 2: 'Schwanz2', 3: 'Schwanz3', 4: 'Schwanz4', 5: 'Schwanz5', 6: 'Schwanz6', 7: 'Wirbel',
         8: 'Oberschenkel L', 9: 'Oberschenkel R', 10: 'Brust', 11: 'Oberarm L', 12: 'Unterarm L', 13: 'Hand L',
         14: 'Klaue L1', 15: 'Klaue L2', 16: 'Oberarm R', 17: 'Unterarm R', 18: 'Hand R', 19: 'Klaue R1', 20: 'Klaue R2',
         21: 'Hals', 22: 'Kopf', 23: 'Kiefer', 24: 'Unterschenkel L', 25: 'Fuss L', 26: 'Unterschenkel R', 27: 'Fuss R',
         28: 'Klaue L3', 29: 'Klaue R3', 30: 'Klaue L4', 31: 'Klaue R4'}


# ═══════════════════════════════════════════════════ Drehungen (Gleitkomma)
def rx(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def ry(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rz(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def achse(v, a):
    v = np.asarray(v, float)
    v = v / np.linalg.norm(v)
    x, y, z = v
    c, s, t = math.cos(a), math.sin(a), 1 - math.cos(a)
    return np.array([[t * x * x + c, t * x * y - s * z, t * x * z + s * y],
                     [t * x * y + s * z, t * y * y + c, t * y * z - s * x],
                     [t * x * z - s * y, t * y * z + s * x, t * z * z + c]])


def euler_matrix(w):
    """Engine-Winkel (1/4096) -> Gleitkomma-Matrix Rx*Ry*Rz."""
    k = 2 * math.pi / 4096.0
    return rx(w[0] * k) @ ry(w[1] * k) @ rz(w[2] * k)


def matrix_euler(m):
    """Gleitkomma-Matrix -> Engine-Winkel (vorzeichenbehaftet, 12 Bit), Umkehrung von Rx*Ry*Rz."""
    k = 4096 / (2 * math.pi)
    ay = math.asin(max(-1.0, min(1.0, m[0, 2])))
    ax = math.atan2(-m[1, 2], m[2, 2])
    az = math.atan2(-m[0, 1], m[0, 0])
    out = []
    for a in (ax, ay, az):
        v = int(round(a * k)) & 0xFFF
        out.append(v - 0x1000 if v & 0x800 else v)
    return out


def quat(m):
    t = np.trace(m)
    if t > 0:
        s = math.sqrt(t + 1.0) * 2
        q = [0.25 * s, (m[2, 1] - m[1, 2]) / s, (m[0, 2] - m[2, 0]) / s, (m[1, 0] - m[0, 1]) / s]
    elif m[0, 0] > m[1, 1] and m[0, 0] > m[2, 2]:
        s = math.sqrt(1.0 + m[0, 0] - m[1, 1] - m[2, 2]) * 2
        q = [(m[2, 1] - m[1, 2]) / s, 0.25 * s, (m[0, 1] + m[1, 0]) / s, (m[0, 2] + m[2, 0]) / s]
    elif m[1, 1] > m[2, 2]:
        s = math.sqrt(1.0 + m[1, 1] - m[0, 0] - m[2, 2]) * 2
        q = [(m[0, 2] - m[2, 0]) / s, (m[0, 1] + m[1, 0]) / s, 0.25 * s, (m[1, 2] + m[2, 1]) / s]
    else:
        s = math.sqrt(1.0 + m[2, 2] - m[0, 0] - m[1, 1]) * 2
        q = [(m[1, 0] - m[0, 1]) / s, (m[0, 2] + m[2, 0]) / s, (m[1, 2] + m[2, 1]) / s, 0.25 * s]
    q = np.array(q)
    return q / np.linalg.norm(q)


def quat_matrix(q):
    w, x, y, z = q
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def slerp_m(a, b, t):
    qa, qb = quat(a), quat(b)
    d = float(qa @ qb)
    if d < 0:
        qb, d = -qb, -d
    if d > 0.9995:
        q = qa + t * (qb - qa)
    else:
        th = math.acos(d)
        q = (math.sin((1 - t) * th) * qa + math.sin(t * th) * qb) / math.sin(th)
    return quat_matrix(q / np.linalg.norm(q))


# ═══════════════════════════════════════════════════ Rig
class Rig:
    def __init__(self, emd_pfad=None):
        self.emd_pfad = emd_pfad or os.path.join(J_DIR, 'FORM.EMD')
        self.form = FE.lese_emd(open(self.emd_pfad, 'rb').read())
        self.eltern = [k['eltern'] for k in self.form['knochen']]
        self.versatz = [np.array(k['versatz'], float) for k in self.form['knochen']]
        self.kinder = {i: [j for j in range(N) if self.eltern[j] == i] for i in range(N)}
        self.ecken = [np.array(m['ecken'], float) if m['ecken'] else np.zeros((0, 3)) for m in self.form['meshes']]
        # Die drei Posen von J: Clip 0 = Bild 1, Clip 1 = Bild 2, Clip 2 = Ruhe (form.json)
        self.pose_j = [self.aus_keyframe(self.form['pool'][c['bilder'][0]['kf']]) for c in self.form['clips']]
        self.ruhe, self.bild1, self.bild2 = self.pose_j[2], self.pose_j[0], self.pose_j[1]

    # Pose = dict(w=[3x3]*N lokale Drehungen, wurzel=np[3], spur=np[3], roh=[[ax,ay,az]]*N oder None)
    def aus_keyframe(self, k):
        return dict(w=[euler_matrix(a) for a in k['winkel']], wurzel=np.array(k['wurzel'], float),
                    spur=np.array(k['spur'], float), roh=[list(a) for a in k['winkel']])

    def kopie(self, p):
        q = dict(w=[m.copy() for m in p['w']], wurzel=p['wurzel'].copy(), spur=p['spur'].copy(), roh=None)
        if 'boden_frei' in p:
            q['boden_frei'] = p['boden_frei']
        return q

    def fk(self, p):
        R, T = [None] * N, [None] * N
        for i in range(N):
            e = self.eltern[i]
            if e < 0:
                R[i], T[i] = p['w'][i], p['wurzel'].copy()
            else:
                R[i] = R[e] @ p['w'][i]
                T[i] = R[e] @ self.versatz[i] + T[e]
        return R, T

    def punkte(self, p, R=None, T=None):
        if R is None:
            R, T = self.fk(p)
        return [self.ecken[i] @ R[i].T + T[i] for i in range(N)]

    def tiefster(self, p, knochen=None):
        P = self.punkte(p)
        ks = range(N) if knochen is None else knochen
        return max(float(P[i][:, 1].max()) for i in ks if len(P[i]))

    def boden(self, p, ziel=0.0):
        p['wurzel'][1] -= self.tiefster(p) - ziel
        return p

    # ── Drehen
    def dreh_lokal(self, p, b, m):
        """Drehung m im Raum des Knochens selbst (nach seiner Ruhedrehung)."""
        p['w'][b] = p['w'][b] @ m
        p['roh'] = None

    def dreh_eltern(self, p, b, m):
        """Drehung m im Raum des Elternknochens (vor seiner lokalen Drehung)."""
        p['w'][b] = m @ p['w'][b]
        p['roh'] = None

    def dreh_welt(self, p, b, m, R=None):
        """Drehung m im Aktorraum um den Knochenursprung: lokal' = R_eltern^T m R_eltern lokal."""
        if R is None:
            R, _ = self.fk(p)
        e = self.eltern[b]
        Re = R[e] if e >= 0 else np.eye(3)
        p['w'][b] = Re.T @ m @ Re @ p['w'][b]
        p['roh'] = None

    # ── Mischen
    def misch(self, a, b, t, knochen=None):
        out = self.kopie(a)
        ks = range(N) if knochen is None else knochen
        for i in ks:
            out['w'][i] = slerp_m(a['w'][i], b['w'][i], t)
        if knochen is None:
            out['wurzel'] = a['wurzel'] * (1 - t) + b['wurzel'] * t
            out['spur'] = a['spur'] * (1 - t) + b['spur'] * t
        return out

    # ── Bein- und Arm-IK: drei Glieder (oben, unten, Ende); Gelenke bleiben in der Ebene der Ruhepose
    def _beuge_normale_lokal(self, kette, p):
        """Normale der Gliedebene im Raum des oberen Knochens (aus Pose p; zu gestreckt -> aus der Ruhe von J)."""
        a, b, c = kette
        for q in (p, self.ruhe):
            R, T = self.fk(q)
            u, v = T[b] - T[a], T[c] - T[b]
            n = np.cross(u, v)
            if np.linalg.norm(n) > 0.05 * np.linalg.norm(u) * np.linalg.norm(v):
                return R[a].T @ (n / np.linalg.norm(n))
        return np.array([0, 0, 1.0])

    def ik_glied(self, p, kette, ziel, it=3):
        """Setzt oben/unten so, dass der Ursprung von kette[2] (Knoechel / Handgelenk) auf `ziel` (Aktorraum) liegt.
        Zwei-Glied-Loesung in der Beugeebene der Eingangspose (Normale im Raum des oberen Knochens festgehalten, damit ein fast
        gestrecktes Glied nicht umklappt), dann Ausrichtung des Glieds auf das Ziel. Streckung hoechstens 98 %."""
        a, b, c = kette
        n_lok = self._beuge_normale_lokal(kette, p)
        for _ in range(it):
            R, T = self.fk(p)
            pa, pb, pc = T[a], T[b], T[c]
            la, lb = np.linalg.norm(pb - pa), np.linalg.norm(pc - pb)
            d = np.array(ziel, float) - pa
            dl = min(np.linalg.norm(d), (la + lb) * 0.98)
            dl = max(dl, abs(la - lb) * 1.02 + 1)
            cos_k = (la * la + lb * lb - dl * dl) / (2 * la * lb)
            k_soll = math.acos(max(-1, min(1, cos_k)))            # Innenwinkel am Knie
            u, v = pb - pa, pc - pb
            n = R[a] @ n_lok
            # Innenwinkel ist, mit Vorzeichen in der Ebene um n gemessen
            k_ist = math.atan2(float(np.cross(-u, v) @ n) * -1.0, float((-u) @ v))
            k_ist_abs = math.acos(max(-1, min(1, float((-u) @ v) / (la * lb))))
            s_ = 1.0 if float(np.cross(u, v) @ n) >= 0 else -1.0
            # gewuenschter Zustand: gleiche Beugerichtung (s_ = +1), Innenwinkel k_soll
            # Drehung des unteren Glieds um n: v' = rot(n, w) v; Innenwinkel aendert sich um -w bei s_ = +1
            if s_ > 0:
                w = k_ist_abs - k_soll
            else:
                w = -(k_ist_abs + k_soll)                          # zurueck in die richtige Beugerichtung
            self.dreh_welt(p, b, achse(n, w), R)
            R, T = self.fk(p)
            pc = T[c]
            v1, v2 = pc - T[a], np.array(ziel, float) - T[a]
            ax_ = np.cross(v1, v2)
            if np.linalg.norm(ax_) > 1e-6:
                ww = math.atan2(np.linalg.norm(ax_), float(v1 @ v2))
                self.dreh_welt(p, a, achse(ax_, ww), R)
        return p

    # ── Keyframe fuer form_emd
    def keyframe(self, p, wort=0):
        if p.get('roh') is not None:
            w = [list(a) for a in p['roh']]
        else:
            w = [matrix_euler(m) for m in p['w']]
        return FE.bild(self.form, wurzel=[int(round(x)) for x in p['wurzel']], spur=[int(round(x)) for x in p['spur']],
                       winkel=w, wort=wort)


# ═══════════════════════════════════════════════════ Ansichten (Offline-Buehne)
def kamera_seite(gier_kam, abstand=11000, hoehe=-1300, ziel_y=-1100, h=260, ziel_x=0):
    """Kamera um die Figur: gier_kam 0 = von rechts (-Z), 1024 = von vorn (+X), 2048 = links, 3072 = hinten."""
    import form_buehne as FB
    a = gier_kam * 2 * math.pi / 4096.0
    pos = (ziel_x + int(abstand * math.sin(a)), hoehe, int(-abstand * math.cos(a)))
    return FB.Kamera.aus_rid(pos, (ziel_x, ziel_y, 0), h=h)


if __name__ == '__main__':
    r = Rig()
    for name, p in (('ruhe', r.ruhe), ('bild1', r.bild1), ('bild2', r.bild2)):
        R, T = r.fk(p)
        print(name, 'wurzel', p['wurzel'].astype(int).tolist(), 'tiefster %.0f' % r.tiefster(p))
        for b in (13, 18, 25, 27, 22, 6):
            print('   %-16s %s' % (NAMEN[b], T[b].astype(int).tolist()))
