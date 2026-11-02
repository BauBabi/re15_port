"""Stufe 3: Clips der Bahnsteig-Form J -> BAHNSTEIG_FORM.EMD/.TIM.

    python s3_clips.py [--aus DIR]          baut EMD + TIM + clips.json (Clip-Tabelle) nach DIR (Vorgabe stufe3/asset)

HERKUNFT JEDER ZAHL
  G4-Daten (Datei, kein Code): RE2 CDEMD0.EMS kind 0x34 rec 3 (Sektor 3310, Datei 0x677000; EDD Paar 1 ab 0x67b960, EMR Paar 1 ab
  0x67d05c), bytegleich in shared_assets/RE2/BAHNSTEIG_G4.EMD (s3_g4.py prueft das). Aus diesem EDD/EMR uebernimmt die Form je
  Tick-Clip unveraendert: Bildzahl, Treffer-Bit 16, Ton-Bit 27 + Id (Wort >> 28) und die Spur (Keyframe +6/+10, kumuliert).
  Damit gelten die Bildfenster und Tempo-Schwellen des Ticks (birkin_bahnsteig.c, dort je @0x-Adresse) fuer die Form wie fuer G4.
  PORT-WAHL (Gestaltung, keine RE-Zahl): alle Posen, Winkel, Zeitpunkte innerhalb eines Clips, Hub, Schwanzwelle, die Laenge der
  neuen Clips (Stehen, Flucht, Sturz), deren Spur und Toene. Die zwei Screenshot-Posen von J stehen unveraendert als Bilder darin.
"""
import os, sys, math, json, argparse
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S       # noqa: E402
import s3_g4 as G        # noqa: E402
import form_emd as FE    # noqa: E402

D = math.radians
AUS = os.path.join(HIER, '..', 'asset')
# Bodenkorrektur eines freien Glieds: erst das untere Glied (Unterschenkel/Unterarm) bis 70 Grad, dann das obere bis 50 Grad
# (Nachbesserung 1; vorher oben 70 / unten 50: an einem fast senkrechten Bein hebt die Huefte den Fuss nur mit 1 - cos, die Suche
# drehte den Oberschenkel dann 40-69 Grad weg, Drohen Bild 31/45/53). Gestaltung.
GLIED_REIHE = lambda kette: [(kette[1], 70.0), (kette[0], 50.0)]
KIEFER_K3 = 0.65    # Gestaltung (Nachbesserung 1): Kieferwinkel J -> K3, s. Bau.kopf

# Clip-Nummern: die Tick-Clips (birkin_bahnsteig.c BB_C_*) behalten ihre Nummer, die neuen liegen dazwischen.
C_RENNEN, C_STEHEN, C_HIEB0, C_HIEB1, C_GEHEN = 0, 2, 3, 4, 5
C_ZUCKEN, C_NACHHIEB, C_DROHEN, C_FLUCHT, C_STURZ = 8, 9, 10, 11, 12
C_TAUMELN = 13                     # Nachbesserung 1 (M5): Rueckstoss-Taumeln, PORT-WAHL (vorher Platzhalter)
C_BISS, C_GRIFF = 26, 27
N_CLIPS = 28
# G4-Clip je Form-Clip (gleiche Nummer = gleiche Rolle im Tick, R_bahnsteig.md 3.1)
G4_VON = {C_RENNEN: 0, C_HIEB0: 3, C_HIEB1: 4, C_GEHEN: 5, C_ZUCKEN: 8, C_NACHHIEB: 9, C_DROHEN: 10, C_BISS: 26, C_GRIFF: 27}


def glatt(t):
    t = max(0.0, min(1.0, t))
    return t * t * (3 - 2 * t)


def welle(t):
    return 0.5 - 0.5 * math.cos(2 * math.pi * t)


class Bau:
    def __init__(self):
        self.r = S.Rig()
        self.g4 = G.g4_lesen()
        r = self.r
        R, T = r.fk(r.ruhe)
        self.ruhe_R, self.ruhe_T = R, T
        # Endpunkte und Neutrallagen der vier Glieder (Ursprung Fuss / Hand = Knoechel / Handgelenk)
        self.glieder = {'LH': S.BEIN_L, 'RH': S.BEIN_R, 'LV': S.ARM_L, 'RV': S.ARM_R}
        self.neutral = {k: T[v[2]].copy() for k, v in self.glieder.items()}
        self.ende_R = {k: R[v[2]].copy() for k, v in self.glieder.items()}

    # ── G4-Daten
    def g4_bilder(self, c):
        return G.clip_daten(self.g4, G4_VON[c])

    def g4_spur(self, c):
        return [np.array(x['spur'], float) for x in self.g4_bilder(c)]

    def g4_worte(self, c):
        return [x['wort'] for x in self.g4_bilder(c)]

    # ── Grundbausteine einer Pose (alle Winkel in Grad, Gestaltung)
    def p(self, basis=None):
        return self.r.kopie(basis or self.r.ruhe)

    def koerper(self, p, nick=0.0, roll=0.0, gier=0.0, hoch=0.0, vor=0.0):
        """Huefte im Aktorraum drehen (nick + = Nase runter) und heben (hoch + = nach oben)."""
        m = S.rx(D(roll)) @ S.ry(D(gier)) @ S.rz(D(nick))
        self.r.dreh_eltern(p, S.HUEFTE, m)
        p['wurzel'][1] -= hoch
        p['wurzel'][0] += vor
        return p

    def ruecken(self, p, nick=0.0, gier=0.0, roll=0.0, anteil=0.5):
        """Wirbel (7) und Brust (10) teilen sich die Biegung."""
        for b, a in ((S.WIRBEL, anteil), (S.BRUST, 1 - anteil)):
            self.r.dreh_eltern(p, b, S.rx(D(roll * a)) @ S.ry(D(gier * a)) @ S.rz(D(nick * a)))
        return p

    def kopf(self, p, nick=0.0, gier=0.0, roll=0.0, kiefer=0.0, hals_anteil=0.45):
        self.r.dreh_eltern(p, S.HALS, S.rx(D(roll * hals_anteil)) @ S.ry(D(gier * hals_anteil)) @ S.rz(D(nick * hals_anteil)))
        self.r.dreh_eltern(p, S.KOPF, S.rx(D(roll * (1 - hals_anteil))) @ S.ry(D(gier * (1 - hals_anteil))) @ S.rz(D(nick * (1 - hals_anteil))))
        if kiefer:
            # Die Kieferwerte der Clips sind fuer den Kiefer von J gesetzt; der K3-Kiefer ist 1089 lang, bei gleichem Winkel stuende
            # die Spitze rund 1,5-mal so weit offen (K.md Abschnitt 8 Punkt 2). KIEFER_K3 bringt die Spaltweite an der Spitze zurueck
            # (Gestaltung). Der Kiefer schliesst hoechstens bis an den Gaumen (az >= 0).
            k = kiefer * KIEFER_K3
            az = S.matrix_euler(p['w'][S.KIEFER])[2] * 360.0 / 4096.0
            k = max(k, -az)
            if k:
                self.r.dreh_eltern(p, S.KIEFER, S.rz(D(k)))   # + = Kiefer nach unten auf
        return p

    def schwanz(self, p, gier=0.0, nick=0.0, phase=0.0, lauf=0.7, hebung=0.0):
        """Welle laeuft nach hinten: Glied k bekommt gier * f(k) * sin(phase - lauf*k); hebung hebt die ganze Kette (+ = hoch)."""
        for k, b in enumerate(S.SCHWANZ):
            f = 0.35 + 0.65 * k / 5.0
            g = gier * f * math.sin(phase - lauf * k)
            n = nick * f * math.sin(phase - lauf * k + 0.9)
            self.r.dreh_eltern(p, b, S.ry(D(g)) @ S.rz(D(n + hebung / 6.0)))
        return p

    def klauen(self, p, seite, beuge):
        """Krallen einer Hand beugen (+ = einkrallen, um die eigene Querachse)."""
        for b in (S.KLAUEN_R if seite == 'R' else S.KLAUEN_L):
            self.r.dreh_lokal(p, b, S.rz(D(beuge)))
        return p

    def glied_ziel(self, p, glied, ziel, ende_welt=None, ende_dreh=None):
        """IK eines Glieds auf `ziel` (Aktorraum); Ende behaelt seine Ruhe-Weltdrehung (Sohle/Hand flach), optional gedreht."""
        kette = self.glieder[glied]
        self.r.ik_glied(p, kette, ziel)
        R, T = self.r.fk(p)
        soll = self.ende_R[glied] if ende_welt is None else ende_welt
        if ende_dreh is not None:
            soll = ende_dreh @ soll
        p['w'][kette[2]] = R[kette[1]].T @ soll
        p['roh'] = None
        return p

    def glied_ziel_boden(self, p, glied, ziel, ende_welt=None):
        """Wie glied_ziel, danach liegt kein Teil des Endknochens (Fuss/Hand samt Krallen) unter y = 0: Ziel so weit anheben."""
        ziel = np.array(ziel, float).copy()
        for _ in range(3):
            self.glied_ziel(p, glied, ziel, ende_welt=ende_welt)
            k = self.glieder[glied][2]
            unter = self._tiefster_baum(p, k)
            if unter <= 10.0:
                break
            ziel[1] -= unter
        return p

    def arm_frei(self, p, seite, schulter=(0, 0, 0), ellbogen=0.0, hand=(0, 0, 0)):
        """Arm ohne IK: Oberarm im Brustraum drehen (rx, ry, rz in Grad), Ellbogen beugen, Hand drehen."""
        o, u, h = S.ARM_R if seite == 'R' else S.ARM_L
        sx, sy, sz = schulter
        self.r.dreh_eltern(p, o, S.rx(D(sx)) @ S.ry(D(sy)) @ S.rz(D(sz)))
        self.r.dreh_lokal(p, u, S.rz(D(ellbogen)))
        hx, hy, hz = hand
        self.r.dreh_lokal(p, h, S.rx(D(hx)) @ S.ry(D(hy)) @ S.rz(D(hz)))
        return p

    def bein_frei(self, p, seite, huefte=(0, 0, 0), knie=0.0, fuss=(0, 0, 0)):
        o, u, f = S.BEIN_R if seite == 'R' else S.BEIN_L
        sx, sy, sz = huefte
        self.r.dreh_eltern(p, o, S.rx(D(sx)) @ S.ry(D(sy)) @ S.rz(D(sz)))
        self.r.dreh_lokal(p, u, S.rz(D(knie)))
        fx, fy, fz = fuss
        self.r.dreh_lokal(p, f, S.rx(D(fx)) @ S.ry(D(fy)) @ S.rz(D(fz)))
        return p

    def lage_von(self, pose):
        """Endpunkte und Weltdrehungen der vier Glieder in einer Pose (fuer Posen um Bild 1/2: deren Fussstellung)."""
        R, T = self.r.fk(pose)
        return {g: (T[k[2]].copy(), R[k[2]].copy()) for g, k in self.glieder.items()}

    def boden_halten(self, p, glieder=('LH', 'RH', 'LV', 'RV'), ziele=None, von=None):
        """Nach einer Koerperbewegung die Glieder am Boden halten (IK): auf den Neutralpunkten der Ruhe, oder (von = lage_von(..))
        auf den Punkten einer anderen Pose, mit deren Fussdrehung, Hoehe auf den Boden gesetzt."""
        for g in glieder:
            if von is not None:
                z, rw = von[g]
                z = z.copy()
                z[1] = self.neutral[g][1]
                self.glied_ziel(p, g, z, ende_welt=rw)
                continue
            z = self.neutral[g] if ziele is None or g not in ziele else ziele[g]
            self.glied_ziel(p, g, z)
        return p

    # ── Bodenfreiheit: kein Teil ausser stehenden Fuessen/Haenden unter y = 0
    def unterbaum(self, b):
        out, st = [], [b]
        while st:
            k = st.pop()
            out.append(k)
            st.extend(self.r.kinder[k])
        return out

    def _tiefster_baum(self, p, b):
        P = self.r.punkte(p)
        return max(float(P[j][:, 1].max()) for j in self.unterbaum(b) if len(P[j]))

    def _heben(self, p, b, grenze, toleranz=12.0, richt_fest=None):
        """Knochen b im Elternraum um Z drehen, bis sein Unterbaum ueber dem Boden liegt (Halbierung, Richtung gemessen
        oder fest vorgegeben)."""
        y0 = self._tiefster_baum(p, b)
        if y0 <= toleranz:
            return 0.0
        alt = p['w'][b].copy()
        richt = None
        for sg in ((richt_fest,) if richt_fest else (1, -1)):
            p['w'][b] = S.rz(D(sg * 3)) @ alt
            if self._tiefster_baum(p, b) < y0:
                richt = sg
                break
        if richt is None:
            p['w'][b] = alt
            return 0.0
        lo, hi = 0.0, grenze
        p['w'][b] = S.rz(D(richt * hi)) @ alt
        if self._tiefster_baum(p, b) > toleranz:
            p['roh'] = None
            return richt * hi
        for _ in range(14):
            mid = (lo + hi) / 2
            p['w'][b] = S.rz(D(richt * mid)) @ alt
            if self._tiefster_baum(p, b) > toleranz:
                lo = mid
            else:
                hi = mid
        p['w'][b] = S.rz(D(richt * hi)) @ alt
        p['roh'] = None
        return richt * hi

    def boden_frei(self, p, merk=None, vorgabe=None, richtung=None):
        """Teile ueber den Boden heben. merk = Liste, in die (Knochen, Winkel) geschrieben wird; vorgabe = dict Knochen -> Winkel,
        dann wird nicht gesucht, sondern genau so gedreht (zweiter Durchgang mit zeitlich geglaetteten Winkeln)."""
        reihe = [(b, 22.0) for b in S.SCHWANZ] + [(S.HALS, 30.0), (S.KOPF, 20.0)]
        R, T = self.r.fk(p)
        for g, kette in self.glieder.items():
            if abs(T[kette[2]][1] - self.neutral[g][1]) > 25 or g in p.get('glieder_frei', ()):   # Glied frei
                reihe += GLIED_REIHE(kette)
        for b, grenze in reihe:
            if vorgabe is not None:
                w = vorgabe.get(b, 0.0)
                if w:
                    p['w'][b] = S.rz(D(w)) @ p['w'][b]
                    p['roh'] = None
                continue
            w = self._heben(p, b, grenze, richt_fest=(richtung or {}).get(b))
            if merk is not None:
                merk.append((b, w))
        return p

    def boden_frei_clip(self, posen):
        """Bodenfreiheit fuer einen ganzen Clip: Winkel je Bild suchen, je Knochen zeitlich glaetten (gleitendes Maximum ueber
        +-2 Bilder, dann Mittel ueber 5), auf die unveraenderten Posen anwenden. So springt ein Glied nicht zwischen 'gehoben' und
        'nicht gehoben' von einem Bild zum naechsten."""
        n = len(posen)
        aktiv = [q.get('roh') is None and q.get('boden_frei', True) for q in posen]
        def suchen(richtung):
            winkel = [dict() for _ in range(n)]
            for f, q in enumerate(posen):
                if not aktiv[f]:
                    continue
                probe = self.r.kopie(q)
                merk = []
                self.boden_frei(probe, merk=merk, richtung=richtung)
                for b, w in merk:
                    winkel[f][b] = w
            return winkel
        winkel = suchen(None)
        # je Knochen EINE Drehrichtung fuer den ganzen Clip (die mit der groesseren Summe), sonst kippt ein Glied zwischen
        # 'vorn hoch' und 'hinten hoch' von Bild zu Bild
        summe = {}
        for d in winkel:
            for b, w in d.items():
                summe.setdefault(b, [0.0, 0.0])[0 if w > 0 else 1] += abs(w)
        richtung = {b: (1 if v[0] >= v[1] else -1) for b, v in summe.items()}
        winkel = suchen(richtung)
        knochen = set(k for d in winkel for k in d)
        glatt_w = [dict() for _ in range(n)]
        for b in knochen:
            roh = [winkel[f].get(b, 0.0) for f in range(n)]
            mx = []
            for f in range(n):
                fenster = [roh[k] for k in range(max(0, f - 2), min(n, f + 3)) if aktiv[k]]
                mx.append(max(fenster, key=abs) if fenster else 0.0)
            for f in range(n):
                fenster = [mx[k] for k in range(max(0, f - 2), min(n, f + 3))]
                glatt_w[f][b] = sum(fenster) / len(fenster)
                if abs(roh[f]) > abs(glatt_w[f][b]):
                    glatt_w[f][b] = roh[f]
        for f, q in enumerate(posen):
            if aktiv[f]:
                self.boden_frei(q, vorgabe=glatt_w[f])
        return posen

    # ── Zeitleiste: Schluesselposen + Glaettung
    def zeitleiste(self, n, schluessel, glatt_art='glatt'):
        """schluessel = [(bild, pose)], erstes Bild 0. Zwischen zwei Schluesseln slerp mit Glaettung."""
        schluessel = sorted(schluessel, key=lambda x: x[0])
        out = []
        for f in range(n):
            for i in range(len(schluessel) - 1):
                a, b = schluessel[i], schluessel[i + 1]
                if a[0] <= f <= b[0]:
                    t = 0.0 if b[0] == a[0] else (f - a[0]) / float(b[0] - a[0])
                    t = glatt(t) if glatt_art == 'glatt' else t
                    q = self.r.misch(a[1], b[1], t)
                    if f == a[0]:
                        q = self.r.kopie(a[1]); q['roh'] = a[1].get('roh')
                    elif f == b[0]:
                        q = self.r.kopie(b[1]); q['roh'] = b[1].get('roh')
                    out.append(q)
                    break
            else:
                q = self.r.kopie(schluessel[-1][1]); q['roh'] = schluessel[-1][1].get('roh')
                out.append(q)
        return out


# ═══════════════════════════════════════════════════ Gang (Clip 5): Fuss-IK gegen die G4-Spur
def gang(bau, n, spur_x, zyklen, tritt_phase, last=0.72, hub=(260, 260, 360, 360), bob=40.0, schwanz_gier=7.0,
         vorwaerts=True, spur_voll=None, extra=None):
    """Seitenfolge-Gang. A(f) = Spur (Aktor laeuft um A weiter); ein stehender Fuss ist in der WELT fest, im Aktorraum
    wandert er also um -A. tritt_phase: Phase je Glied (Anteil am Zyklus). Hub je Glied: Hoehe der Schwungkurve."""
    r = bau.r
    gesamt = spur_x[-1]
    T = n / float(zyklen)

    def A(t):
        k = math.floor(t / n)
        u = t - k * n
        i = int(math.floor(u))
        w = u - i
        a = spur_x[i] if i < n else spur_x[-1]
        b = spur_x[i + 1] if i + 1 < n else gesamt + spur_x[0]
        return k * gesamt + a + (b - a) * w

    namen = ['LH', 'LV', 'RH', 'RV']
    out = []
    for f in range(n):
        p = bau.p()
        ph = 2 * math.pi * f / T
        # Koerper: zwei Senken je Zyklus, Rollen zur Standseite, Ruecken gegengleich
        bau.koerper(p, nick=1.2 * math.sin(2 * ph + 0.4), roll=2.2 * math.sin(ph), gier=1.5 * math.sin(ph + 0.6),
                    hoch=-bob * (0.5 + 0.5 * math.cos(2 * ph)))
        bau.ruecken(p, gier=-3.0 * math.sin(ph + 0.6), roll=-2.0 * math.sin(ph), nick=-1.0 * math.sin(2 * ph + 0.4))
        bau.kopf(p, nick=-2.5 * math.sin(2 * ph + 1.2), gier=2.0 * math.sin(ph + 1.5))   # Kiefer zu (Nachbesserung 1, K3)
        bau.schwanz(p, gier=schwanz_gier, nick=3.0, phase=ph)
        if extra is not None:
            extra(p, f)                       # Zusatzbewegung des Rumpfs vor der Fuss-IK (Taumeln, Nachbesserung 1)
        for gi, g in enumerate(namen):
            phi = tritt_phase[g]
            u = f / T - phi
            k = math.floor(u)
            pos = u - k
            s_k = (k + phi) * T
            def X(kk):
                s = (kk + phi) * T
                return bau.neutral[g][0] + A(s + last * T / 2)
            ziel = bau.neutral[g].copy()
            dreh = None
            if pos < last:
                ziel[0] = X(k) - A(f)
            else:
                e_k = s_k + last * T
                w_lin = (f - e_k) / ((1 - last) * T)
                w = glatt(w_lin)
                x0 = X(k) - A(e_k)
                x1 = X(k + 1) - A(s_k + T)
                ziel[0] = x0 + (x1 - x0) * w
                ziel[1] -= hub[gi] * math.sin(math.pi * w_lin)
                # Fuss rollt ab (Ferse hoch, Sohle nach hinten), Hand krallt im Schwung
                if g in ('LH', 'RH'):
                    dreh = S.rz(D(-38 * math.sin(math.pi * min(1.0, w_lin * 1.3))))
                else:
                    dreh = S.rz(D(22 * math.sin(math.pi * w_lin)))
            bau.glied_ziel(p, g, ziel, ende_dreh=dreh)
        if not vorwaerts:
            pass
        p['spur'] = np.array([spur_x[f], 0, 0], float) if spur_voll is None else np.array(spur_voll[f], float)
        out.append(p)
    return out


def stehen(bau, n=60):
    """Clip 2 Stehen/Atmen, Bild 0 = Ruhepose (Ruhe von J, Kiefer zu; form.json ruhe = Clip 2 Bild 0)."""
    out = []
    for f in range(n):
        if f == 0:
            q = bau.r.kopie(bau.r.ruhe); q['roh'] = bau.r.ruhe['roh']
            out.append(q)
            continue
        ph = 2 * math.pi * f / n
        p = bau.p()
        bau.koerper(p, hoch=22 * math.sin(ph) ** 2, nick=-0.8 * math.sin(ph))
        bau.ruecken(p, nick=-1.6 * math.sin(ph), gier=1.2 * math.sin(ph * 0.5 + 0.3) * math.sin(ph))
        bau.kopf(p, nick=-3 * math.sin(ph), gier=4 * math.sin(ph) * math.sin(ph * 0.5))   # Kiefer zu (Nachbesserung 1, K3)
        bau.schwanz(p, gier=4 * math.sin(ph), nick=1.5, phase=ph, lauf=0.5)
        bau.boden_halten(p)
        out.append(p)
    return out


def worte_g4(bau, c, n=None):
    w = bau.g4_worte(c)
    return [x & 0xFFFFF000 for x in w][:n or len(w)]


def spur_g4(bau, c):
    return [s.copy() for s in bau.g4_spur(c)]


def bauen(aus, nur=None):
    import s3_angriffe as AN
    bau = Bau()
    clips = {}

    def soll(c):
        return nur is None or c in nur
    # Clip 5 GEHEN: G4 89 Bilder, G4-Spur, G4-Toene (Bild 43/73 Id 0)
    sp5 = bau.g4_spur(C_GEHEN)
    clips[C_GEHEN] = dict(name='gehen', posen=gang(bau, len(sp5), [s[0] for s in sp5], 2,
                                                 dict(LH=0.0, LV=0.25, RH=0.5, RV=0.75), spur_voll=sp5),
                          worte=AN.worte_g4(bau, C_GEHEN), quelle='G4 Clip 5: Laenge, Spur, Toene')
    clips[C_STEHEN] = dict(name='stehen', posen=stehen(bau), worte=None, quelle='PORT-WAHL; Bild 0 = Ruhe J')
    renn, w = AN.rennen(bau)
    clips[C_RENNEN] = dict(name='rennen', posen=renn, worte=w,
                           quelle='G4 Clip 0: Laenge, Spur, Ton; Bild %d = Pose Bild 1' % AN.RENNEN_BILD1)
    tafel = [(C_HIEB0, 'hieb_rechts', lambda: AN.hieb(bau, 'R'), 'G4 Clip 3: Laenge, Spur, Treffer, Toene'),
             (C_HIEB1, 'hieb_links', lambda: AN.hieb(bau, 'L'), 'G4 Clip 4: Laenge, Spur, Treffer, Toene'),
             (C_ZUCKEN, 'zucken', lambda: AN.zucken(bau), 'G4 Clip 8: Laenge, Spur'),
             (C_NACHHIEB, 'nachhieb', lambda: AN.nachhieb(bau), 'G4 Clip 9: Laenge, Spur, Toene'),
             (C_DROHEN, 'drohen', lambda: AN.drohen(bau),
              'G4 Clip 10: Laenge, Spur, Toene; Bild %d-%d = Pose Bild 2' % AN.DROHEN_BILD2),
             (C_BISS, 'biss', lambda: AN.biss(bau), 'G4 Clip 26: Laenge, Treffer, Toene'),
             (C_GRIFF, 'griff', lambda: AN.griff(bau), 'G4 Clip 27: Laenge, Spur (Opfer-Spur), Toene'),
             (C_FLUCHT, 'flucht', lambda: AN.flucht(bau, renn), 'PORT-WAHL (Galoppteil = Clip 0)'),
             (C_STURZ, 'sturz', lambda: AN.sturz(bau), 'PORT-WAHL; Bild 0 = Pose Bild 2'),
             (C_TAUMELN, 'taumeln', lambda: AN.taumeln(bau, gang), 'PORT-WAHL (Nachbesserung 1, M5): Rueckstoss-Taumeln, Schleife, '
              'Spur -80 je Bild')]
    for c, name, fn, quelle in tafel:
        if soll(c):
            p, w = fn()
            clips[c] = dict(name=name, posen=p, worte=w, quelle=quelle)
    return bau, clips


def _norm12(v):
    v &= 0xFFF
    return v - 0x1000 if v & 0x800 else v


def _abstand(a, b):
    return max(abs(((v - u + 2048) % 4096) - 2048) for u, v in zip(a, b))


def euler_stetig(bilder, fest):
    """Je Knochen die Euler-Darstellung waehlen, die am wenigsten springt: (ax, ay, az) oder (ax+2048, 2048-ay, az+2048),
    gleiche Drehmatrix. Der Port mischt Winkel komponentenweise (re15_blend_angle12, kuerzester Bogen) beim Clipwechsel; ein
    Darstellungswechsel zwischen Folgebildern waere dort ein Dreher. Bilder mit festen Winkeln (Posen von J) bleiben, wie sie sind."""
    n = len(bilder)
    if n < 2:
        return
    nb = len(bilder[0]['winkel'])
    for b in range(nb):
        kand = []
        for i, bi in enumerate(bilder):
            a = [int(x) for x in bi['winkel'][b]]
            alt = [_norm12(a[0] + 2048), _norm12(2048 - a[1]), _norm12(a[2] + 2048)]
            kand.append([a] if fest[i] else [a, alt])
        # Viterbi ueber zwei Zustaende
        kosten = [[0.0] * len(kand[0])]
        weg = [[-1] * len(kand[0])]
        for i in range(1, n):
            k_i, w_i = [], []
            for c in kand[i]:
                best, arg = None, -1
                for j, pc in enumerate(kand[i - 1]):
                    v = kosten[-1][j] + _abstand(pc, c)
                    if best is None or v < best:
                        best, arg = v, j
                k_i.append(best)
                w_i.append(arg)
            kosten.append(k_i)
            weg.append(w_i)
        j = min(range(len(kosten[-1])), key=lambda x: kosten[-1][x])
        for i in range(n - 1, -1, -1):
            bilder[i]['winkel'][b] = list(kand[i][j])
            j = weg[i][j]


def schreiben(bau, clips, aus):
    r = bau.r
    form = dict(knochen=r.form['knochen'], meshes=r.form['meshes'], tim=r.form['tim'], clips=[])
    tabelle = []
    for c in range(N_CLIPS):
        if c in clips:
            d = clips[c]
            worte = d.get('worte') or [0] * len(d['posen'])
            bau.boden_frei_clip(d['posen'])
            bilder = [r.keyframe(p, wort=w) for p, w in zip(d['posen'], worte)]
            euler_stetig(bilder, [q.get('roh') is not None for q in d['posen']])
            tabelle.append(dict(clip=c, name=d['name'], bilder=len(bilder), quelle=d.get('quelle', ''),
                                treffer=[i for i, w in enumerate(worte) if w & FE.BILD_TREFFER],
                                ton=[[i, w >> 28] for i, w in enumerate(worte) if w & FE.BILD_TON]))
        else:
            bilder = [r.keyframe(r.ruhe)]
            tabelle.append(dict(clip=c, name='frei (Ruhe)', bilder=1, quelle='Platzhalter, vom Tick nicht benutzt'))
        form['clips'].append(dict(name=tabelle[-1]['name'], bilder=bilder))
    os.makedirs(aus, exist_ok=True)
    emd = FE.schreibe_emd(form)
    open(os.path.join(aus, 'BAHNSTEIG_FORM.EMD'), 'wb').write(emd)
    open(os.path.join(aus, 'BAHNSTEIG_FORM.TIM'), 'wb').write(open(os.path.join(S.BASIS_DIR, 'FORM.TIM'), 'rb').read())
    with open(os.path.join(aus, 'clips.json'), 'w', encoding='utf-8') as f:
        json.dump(tabelle, f, indent=1)
    return emd, tabelle


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default=AUS)
    g = a.parse_args()
    bau, clips = bauen(g.aus)
    emd, tab = schreiben(bau, clips, g.aus)
    print('EMD %d Byte, %d Clips, %d Bilder' % (len(emd), len(tab), sum(t['bilder'] for t in tab)))
