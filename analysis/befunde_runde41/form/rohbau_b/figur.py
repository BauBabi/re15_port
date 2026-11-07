# -*- coding: utf-8 -*-
"""Rohbau B: die Figur. Gerippe aus gerippe.json, je Knochen eine oder mehrere Ringfolgen, Oberflaeche Panzer.
Einheiten: Spiel, Knochenraum, Y unten, +X vorn, +Z links der Figur."""
import json, math, os, sys
import numpy as np
from ringe import Teil, ecken_normalen, aussen_drehen, flaechen_normalen

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)


def sym10(o, ol, sl, ul, u, un=None):
    """Zehn Ecken, oben beginnend, links gleich rechts: oben, oben-seitlich, Seite oben, Seite unten, unten-seitlich, unten."""
    un = u if un is None else un
    return (o, ol, sl, ul, u, un, u, ul, sl, ol)


def sym8(o, ol, s, ul, u):
    return (o, ol, s, ul, u, ul, s, ol)


def glied8(v, va, a, ha, h, hi, i, vi):
    """Acht Ecken eines Glieds: vorn, vorn-aussen, aussen, hinten-aussen, hinten, hinten-innen, innen, vorn-innen."""
    return (v, va, a, ha, h, hi, i, vi)


def lade_gerippe():
    return json.load(open(os.path.join(FORM, 'gerippe.json'), encoding='utf-8'))


class Figur:
    def __init__(self):
        self.g = lade_gerippe()
        self.namen = [k['name'] for k in self.g['knochen']]
        self.eltern = [k['eltern'] for k in self.g['knochen']]
        self.versatz = [list(k['versatz']) for k in self.g['knochen']]
        self.ix = {n: i for i, n in enumerate(self.namen)}
        self.teile = []

    def knochen_dazu(self, name, eltern, versatz):
        self.ix[name] = len(self.namen)
        self.namen.append(name)
        self.eltern.append(self.ix[eltern])
        self.versatz.append(list(versatz))
        return self.ix[name]

    def teil(self, name, knochen, **kw):
        t = Teil(name, self.ix[knochen], **kw)
        self.teile.append(t)
        return t


# ═════════════════════════════════════════════════ Rumpf
# Schuppenregel: vordere Platte liegt auf der hinteren. Laeuft t nach vorn, steigt der Radius an jeder Fuge sprunghaft, danach faellt er langsam.
def platten(t, stationen, q, tag=None, spalten=()):
    """stationen: (t, a, b, ca, stufe). stufe > 0: an dieser Stelle beginnt eine neue Platte, ihr Rand liegt um `stufe` Einheiten ueber der alten."""
    sp = list(spalten)
    for tt, a, b, ca, st in stationen:
        if st:
            t.r(tt - 16, a - st, b - st, q, ca=ca, hart=True, tag=tag)
            if sp:
                if not hasattr(t, 'tag_spalten'):
                    t.tag_spalten = {}
                t.tag_spalten[len(t.ringe) - 1] = set(sp.pop(0))
            t.r(tt, a, b, q, ca=ca, hart=True)
        else:
            t.r(tt, a, b, q, ca=ca)


def rumpf(f):
    # Huefte: Becken mit Dachform, hinten stumpf, Schwanzwurzel oben auf, Sitzhoecker unten.
    t = f.teil('huefte', 'huefte', n=10)
    q = sym10(1.1, .86, .98, 1.05, .98, .92)
    t.pol(hinten=(-770, 70, 0))
    platten(t, [(-700, 380, 480, 120, 0), (-560, 620, 720, 80, 0), (-330, 690, 800, 60, 0),
                (-200, 730, 850, 50, 56), (60, 720, 860, 30, 0), (250, 760, 880, 20, 54), (480, 730, 830, 6, 0), (640, 690, 780, 0, 0)], q, tag='rotbraun_mittelton', spalten=((7, 8, 9, 0, 1), (9, 0, 1, 2, 3)))
    # Wirbel: Taille, Kern fuer die Brustschale, oben Plattenreihe, unten glatter Bauch.
    t = f.teil('wirbel', 'wirbel', n=10)
    q = sym10(1.12, .92, 1.0, 1.0, 1.0, .9)
    platten(t, [(-240, 700, 780, 0, 0), (60, 780, 760, -10, 60), (300, 760, 720, -10, 0), (520, 800, 710, -20, 58), (880, 760, 650, -20, 0), (1320, 660, 600, -40, 0)], q, tag='rotbraun_mittelton', spalten=((0, 1, 2), (6, 7, 8, 9)))
    # Brust: Schale ueber dem Wirbel, Schultergurt, Kiel unten, hinten langer Kragen fuer die Biegung.
    t = f.teil('brust', 'brust', n=10)
    q = sym10(1.1, .96, 1.02, 1.04, 1.0, 1.08)
    platten(t, [(-600, 730, 650, 50, 0), (-420, 830, 720, 40, 56), (-100, 900, 780, 30, 0), (130, 940, 830, 20, 60), (480, 920, 880, 0, 0), (640, 900, 880, -6, 58), (880, 740, 760, -40, 0), (1060, 480, 540, -120, 0)], q, tag='rotbraun_mittelton', spalten=((8, 9, 0, 1, 2, 3), (6, 7, 8, 9, 0)))


# ═════════════════════════════════════════════════ Schwanz
SCHWANZ_Q = (1.24, .94, 1.04, .98, .8, .98, 1.04, .94)      # Grat oben, Kante seitlich, flach unten


def schwanz(f):
    r = [(322, 340), (345, 335), (330, 232), (226, 172), (172, 196), (205, 250)]
    lg = [592, 598, 625, 662, 678, 663]
    for i in range(6):
        t = f.teil('schwanz%d' % (i + 1), 'schwanz%d' % (i + 1), n=8, richtung=-1)
        r0, r1 = r[i]
        rm = .5 * (r0 + r1)
        q = SCHWANZ_Q
        if i < 5:
            t.r(-110, r0 * .8, q=q)
            t.r(lg[i] * .12, r0 * .94, q=q)
            t.r(lg[i] * .5, rm * 1.03, q=q)
            t.stufe(lg[i] * .94, 44, r1 * 1.1, faktor=.8, q=q, tag='rotbraun_mittelton' if i in (1, 4) else None)
        else:
            # Spatenende: flach und breit, Spitze nach links unten
            t.r(-110, r0 * .8, q=q)
            t.r(lg[i] * .14, r0 * .96, q=q)
            t.r(lg[i] * .5, rm * .92, rm * 1.25, q=q)
            t.r(lg[i] * .9, r1 * .72, r1 * 1.5, q=q, tag='rotbraun_mittelton')
            t.r(lg[i] * 1.2, r1 * .5, r1 * 1.1, q=q, tag='rosa_stelle')
            t.pol(vorn=(lg[i] * 1.5, 0, 0))


# ═════════════════════════════════════════════════ Beine
def zehe(f, kn, x, z, w, lang, r):
    """Kralle am Fuss, Teil des Fuss-Netzes. w = Richtung in der Sohlenebene (Grad, + nach Z)."""
    t = f.teil('zeh', kn, n=4, mat='kralle')
    c, s = math.cos(math.radians(w)), math.sin(math.radians(w))
    t.E = np.array((c, .12, s))
    t.B = np.array((-s, 0, c))
    t.A = np.array((0, -1, 0.))
    t.a0 = np.array((x, -40, z), float)
    t.karte = 'zeh'
    t.zone_alle = 'rotbraun_mittelton'
    t.offen_a = True
    t.r(-60, r * .9, r)
    t.r(lang * .45, r * .62, r * .72)
    t.pol(vorn=(lang, 0, 0))
    return t


def bein(f, s):
    o, u, fu = 'oberschenkel_' + s, 'unterschenkel_' + s, 'fuss_' + s
    sei = 1 if s == 'l' else -1
    # Acht Ecken: vorn, vorn-aussen, aussen, hinten-aussen, hinten, hinten-innen, innen, vorn-innen
    t = f.teil(o, o, n=8, achse='y', seite=sei)
    q = glied8(1.0, 1.06, 1.04, 1.14, 1.24, 1.1, .94, 1.0)
    t.r(-210, 220, q=q)
    t.r(0, 330, 300, q)
    t.stufe(250, 30, 345, 305, .9, q, tag='rotbraun_mittelton')
    t.r(560, 280, 250, q)
    t.r(860, 215, 200, q)
    t.stufe(975, 44, 262, 245, .76, glied8(1.1, 1, 1, 1, 1, 1, 1, 1), tag='rotbraun_mittelton')
    t = f.teil(u, u, n=8, achse='y', seite=sei)
    q = glied8(1.08, .96, .96, 1.06, 1.2, 1.06, .94, .96)
    t.r(-130, 170, q=q)
    t.r(50, 225, 205, q)
    t.stufe(250, 26, 235, 200, .9, q)
    t.stufe(470, 26, 205, 175, .9, q)
    t.r(700, 148, 142, q)
    t.stufe(815, 40, 185, 178, .78, tag='rotbraun_mittelton')
    t = f.teil(fu, fu, n=8)
    q = sym8(1.08, 1.0, 1.1, 1.0, .72)
    t.r(-190, 120, 140, q)
    t.r(10, 200, 195, q)
    t.r(330, 195, 220, q)
    t.r(620, 165, 250, q)
    t.r(860, 105, 280, q)
    t.pol(vorn=(960, -30, 0))
    for z, w in ((-170, -12), (0, 0), (170, 12)):
        zehe(f, fu, 840, z, w, 380, 90)


# ═════════════════════════════════════════════════ Arme
# Finger je Hand: Versatz im Handraum, Richtung (Grad gegen +X in der Handebene, + nach Z), Laenge. Rechte Hand: Daumen innen auf +Z, aussen auf -Z. Links gespiegelt.
FINGER = {'daumen': ((220, 40, 300), 40, 430), 'zeige': ((540, 30, 190), 8, 560), 'mitte': ((540, 30, -190), -8, 560), 'aussen': ((330, 30, -390), -62, 960)}
KLAUE_R = {'klaue_r1': 'daumen', 'klaue_r2': 'zeige', 'klaue_r3': 'mitte', 'klaue_r4': 'aussen'}
KLAUE_L = {'klaue_l3': 'daumen', 'klaue_l2': 'zeige', 'klaue_l1': 'mitte', 'klaue_l4': 'aussen'}


def klaue(f, name, art, sg):
    """Finger samt Klaue, ein starrer Knochen. sg = +1 rechte Hand, -1 linke (Z gespiegelt)."""
    v, w, lg = FINGER[art]
    f.versatz[f.ix[name]] = [v[0], v[1], v[2] * sg]
    w = math.radians(w * sg)
    c, s = math.cos(w), math.sin(w)
    k = f.teil(name, name, n=6, mat='finger')
    k.E = np.array((c, 0, s))
    k.B = np.array((-s, 0, c))
    k.karte = 'finger'
    k.offen_a = True
    e = lg / 800.0
    k.r(-140, 185, 235)
    k.stufe(340 * e, 30, 165, 215, .78, tag='rotbraun_mittelton')
    k.r(600 * e, 80, 105, ca=-46, tag='rotbraun_mittelton')
    k.tag[-2] = 'rotbraun_mittelton'
    k.pol(vorn=(800 * e, -120, 0))
    return k


def schuppe(f, kn, tt, sei, br):
    """Niedrige Dreiecksschuppe an der Innenkante des Unterarms: G in Bild 2 ist gerippt, die Gipfel stehen unregelmaessig."""
    t = f.teil('schuppe', kn, n=3, mat='schuppe')
    t.E = np.array((.25, .25, -float(sei)))
    t.E = t.E / np.linalg.norm(t.E)
    t.A = np.array((0, 1, 0.))
    t.B = np.array((1, 0, 0.))
    t.a0 = np.array((40, tt, -sei * 250.0))
    t.karte = 'schuppe'
    t.offen_a = True
    t.r(-30, br, br * .8)
    t.pol(vorn=(br * 1.5, 0, 0), hinten=(-80, 0, 0))
    return t


def arm(f, s):
    o, u, h = 'oberarm_' + s, 'unterarm_' + s, 'hand_' + s
    sei = 1 if s == 'l' else -1
    t = f.teil(o, o, n=8, achse='y', seite=sei)
    # Schulterplatte, dann Oberarm mit Trizeps hinten
    q = glied8(1.0, 1.1, 1.2, 1.1, 1.06, .98, .9, .96)
    t.r(-300, 240, q=q)
    t.r(-60, 400, 410, q)
    t.stufe(130, 40, 430, 450, .82, q, tag='rotbraun_mittelton')
    t.r(420, 330, 320, glied8(1.04, 1, 1, 1.06, 1.14, 1.04, .94, .98))
    t.r(720, 290, 285, glied8(1.04, 1, 1, 1.06, 1.14, 1.04, .94, .98))
    t.r(900, 230)
    t = f.teil(u, u, n=8, achse='y', seite=sei)
    # Unterarm flach (a vorn-hinten, b seitlich): Bild 1 und Bild 2 rechts sehen die Breite, Bild 2 links die Schmalseite. Ellbogenkappe hinten als Keil, Stulpe am Handgelenk.
    t.r(-260, 200, 240, q=glied8(.8, 1, 1, 1.3, 1.6, 1.3, 1, 1))
    t.r(30, 265, 320, glied8(.96, 1, 1, 1.14, 1.3, 1.14, 1, 1))
    t.r(240, 250, 325, glied8(1.0, 1, 1.02, 1.06, 1.14, 1.06, 1.02, 1))
    t.r(620, 225, 295, glied8(1.04, 1, 1.0, 1, 1.06, 1, 1, 1))
    t.r(960, 200, 265, glied8(1, 1, 1, 1, 1, 1, 1, 1))
    t.stufe(1090, 44, 232, 290, .76)
    for tt, br in ((130, 44), (285, 52), (372, 40), (560, 56), (700, 46), (905, 40)):
        schuppe(f, u, tt, sei, br)
    t = f.teil(h, h, n=8, ab=((0, -1, 0), (0, 0, sei)))
    # B zeigt nach aussen. Hand breit nach aussen versetzt, Daumenseite knapp.
    q = sym8(1.05, 1.02, 1.1, 1.02, .7)
    t.offen_a = True
    t.r(-200, 190, 230, q)
    t.r(30, 270, 290, q)
    t.r(280, 235, 400, q)
    t.r(500, 180, 440, q)
    t.r(640, 110, 380, q)
    t.pol(vorn=(720, 0, 0))
    for name, art in (KLAUE_L if s == 'l' else KLAUE_R).items():
        klaue(f, name, art, -1 if s == 'l' else 1)


# ═════════════════════════════════════════════════ Kopf
def zahn(f, kn, x, y, z, lang, r, runter=1, karte='zahn'):
    t = f.teil('zahn', kn, n=3, mat='zahn')
    t.E = np.array((0.12, runter, 0.))
    t.A = np.array((1, 0, 0.))
    t.B = np.array((0, 0, 1.))
    t.a0 = np.array((x, y, z), float)
    t.karte = karte
    t.zone_alle = 'rotbraun_mittelton'
    t.offen_a = True
    t.r(-30, r)
    t.pol(vorn=(lang, 0, 0), hinten=(-80, 0, 0))
    return t


def kopf(f):
    t = f.teil('hals', 'hals', n=8)
    q = sym8(1.12, 1.0, 1.0, 1.0, .96)
    t.r(-320, 420, q=q)
    t.r(150, 440, q=q)
    t.r(170, 480, q=q, hart=True)
    t.r(420, 470, q=q)
    t.r(700, 480, q=q)
    # Schaedel: breiter Schild um den Rand (Zug Q), davor schmale schwere Schnauze, Brauenwuelste.
    t = f.teil('kopf', 'kopf', n=10)
    q0 = sym10(.92, 1.12, 1.04, .98, .9, .84)
    t.r(-170, 360, 380, q0)
    t.r(130, 520, 560, q0)
    t.r(400, 590, 660, q0)
    q_rand = sym10(.96, 1.28, 1.02, .98, .92, .86)     # zwei Eckbuckel: Zug Q laeuft ueber sie
    t.stufe(490, 30, 600, 668, .92, q_rand)
    t.r(560, 500, 560, sym10(.9, 1.16, 1.0, .96, .92, .84))
    t.r(660, 460, 510, sym10(.9, 1.2, 1.0, .96, .92, .84))     # Brauen
    t.r(840, 400, 410, sym10(.94, 1.1, .94, 1.0, .94, .84))
    t.r(1090, 320, 325, sym10(1.04, 1.02, .96, 1.04, .94, .84))  # Nase
    t.r(1250, 240, 250, sym10(.96, 1.04, 1.0, 1.0, .9, .84))
    t.pol(vorn=(1330, 20, 0))
    for sg in (1, -1):                                      # Wangenspitzen am Rand: Zug Q endet in Spitze P (366,304), Spitze im Kopfraum aus dem Sehstrahl
        w = f.teil('wange', 'kopf', n=4)
        w.karte = 'wange'
        a0 = np.array((470., 230., 500. * sg))
        sp = np.array((261., 520., 499. * sg))
        w.E = (sp - a0) / np.linalg.norm(sp - a0)
        w.A = np.array((1, 0, 0.))
        w.A = w.A - w.E * (w.A @ w.E)
        w.A = w.A / np.linalg.norm(w.A)
        w.B = np.cross(w.E, w.A)
        w.a0 = a0
        w.offen_a = True
        lw = float(np.linalg.norm(sp - a0))
        w.r(-60, 170, 150)
        w.r(lw * .7, 105, 95, tag='weisskerne')
        w.pol(vorn=(lw, 0, 0), hinten=(-120, 0, 0))
    t = f.teil('kiefer', 'kiefer', n=8)
    q = sym8(.7, 1.06, 1.1, 1.0, .9)
    t.r(-160, 180, 300, q)
    t.r(160, 230, 345, q)
    t.r(480, 190, 320, q)
    t.r(700, 150, 260, q)
    t.pol(vorn=(790, 10, 0))
    # Zaehne: oben aussen am Schnauzenrand, unten im Kiefer
    for x, z, lg, r in ((840, 300, 170, 52), (980, 270, 150, 52), (1100, 225, 300, 74), (1215, 130, 190, 52)):
        for sg in (1, -1):
            zahn(f, 'kopf', x, 240, z * sg, lg, r)
    for x, z, lg in ((380, 250, 150), (520, 225, 170), (650, 170, 190)):
        for sg in (1, -1):
            zahn(f, 'kiefer', x, -150, z * sg, lg, 50, runter=-1)


def bauen(mit_fit=True):
    f = Figur()
    f.knochen_dazu('klaue_l4', 'hand_l', (420, 0, 480))
    f.knochen_dazu('klaue_r4', 'hand_r', (420, 0, -480))
    rumpf(f)
    schwanz(f)
    bein(f, 'l')
    bein(f, 'r')
    arm(f, 'l')
    arm(f, 'r')
    kopf(f)
    if mit_fit:
        ringfit_anwenden(f)
    return f


def ringfit_anwenden(f):
    """Feinanpassung aus anpassen.py: ganze Ringe skaliert und geschoben."""
    pf = os.path.join(HIER, 'ringfit.json')
    if not os.path.exists(pf):
        return
    par = json.load(open(pf))
    for t in f.teile:
        g = t.name[:-2] if t.name.endswith(('_l', '_r')) else t.name
        for i, r in enumerate(t.ringe):
            p = par.get(g + '/%d' % i)
            if p is None:
                continue
            c = r.mean(0)
            q = r - c
            a = q @ t.A
            b = q @ t.B
            e = q - np.outer(a, t.A) - np.outer(b, t.B)
            t.ringe[i] = c + e + np.outer(a * p[0] + p[2], t.A) + np.outer(b * p[1] + p[3], t.B)


def posen(f):
    """Grundlage gerippe.json, Aenderungen stehen in posen.json (je Pose: {Knochenname: [ax, ay, az]}, lage, gier, wurzel_h)."""
    g = f.g
    n = len(f.namen)
    aus = {}
    extra = {}
    pf = os.path.join(HIER, 'posen.json')
    if os.path.exists(pf):
        extra = json.load(open(pf, encoding='utf-8'))
    for nm in ('bild1', 'bild2'):
        p = g['posen'][nm]
        w = [list(x) for x in p['winkel']] + [[0, 0, 0] for _ in range(n - len(p['winkel']))]
        aus[nm] = dict(lage=list(p['lage']), gier=p['gier'], wurzel=list(p['wurzel']), winkel=w)
    for nm, e in extra.items():
        if nm == 'ruhe':
            continue
        p = aus[nm]
        for k, v in e.get('winkel', {}).items():
            p['winkel'][f.ix[k]] = list(v)
        if 'lage' in e:
            p['lage'] = list(e['lage'])
        if 'gier' in e:
            p['gier'] = e['gier']
        if 'wurzel' in e:
            p['wurzel'] = list(e['wurzel'])
    aus['ruhe'] = dict(lage=[0, 0, 0], gier=0, wurzel=list(g['posen']['bild1']['wurzel']), winkel=[list(x) for x in aus['bild1']['winkel']])
    for k, v in extra.get('ruhe', {}).get('winkel', {}).items():
        aus['ruhe']['winkel'][f.ix[k]] = list(v)
    if 'wurzel' in extra.get('ruhe', {}):
        aus['ruhe']['wurzel'] = list(extra['ruhe']['wurzel'])
    return aus


ZUORDNUNG = {
    1: {'tail': ['schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6'],
        'left_leg': ['oberschenkel_l', 'unterschenkel_l', 'fuss_l'],
        'right_leg': ['oberschenkel_r', 'unterschenkel_r', 'fuss_r'],
        'right_arm': ['oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'],
        'part_of_Body': ['huefte', 'wirbel', 'brust']},
    2: {'face_and_upper_body': ['huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer', 'oberarm_l'],
        'left_arm_front': ['unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'],
        'right_arm_front': ['unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4']},
}


def fertig(f):
    """Netze je Knochen zusammenstellen. Rueckgabe dict fuer schau.py."""
    netze, netze_n = [], []
    for t in f.teile:
        V, F, G = t.netz()
        F = aussen_drehen(V, F, G, t)
        netze.append((t.knochen, V, F))
        FN = flaechen_normalen(V, F)
        FN = FN / np.maximum(np.linalg.norm(FN, axis=1, keepdims=True), 1e-9)
        netze_n.append((t.knochen, V, F, FN))
    fig = dict(eltern=f.eltern, versatz=f.versatz, netze=netze, netze_n=netze_n, posen=posen(f),
               zuordnung={n: {k: [f.ix[b] for b in v] for k, v in z.items()} for n, z in ZUORDNUNG.items()})
    import schau
    for nm, p in fig['posen'].items():      # tiefster Punkt je Pose auf y = 0
        R, T = schau.fk(f.eltern, f.versatz, p['wurzel'], p['winkel'])
        tief = max(float((V @ R[b].T + T[b])[:, 1].max()) for b, V, _F in netze)
        p['wurzel'][1] = int(round(p['wurzel'][1] - tief))
    fl = sum(len(F) for _, _, F in netze)
    print('Flaechen', fl, 'Knochen', len(f.namen))
    return fig


if __name__ == '__main__':
    import schau
    f = bauen()
    schau.alles(fertig(f))
