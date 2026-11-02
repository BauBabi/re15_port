"""Stufe 3: Clips ausser Gehen/Stehen (Rennen, Hiebe, Zucken, Nachhieb, Drohen, Biss, Griff, Flucht, Sturz).

Alle Posen, Winkel und Zeitpunkte sind PORT-WAHL (Gestaltung fuer die neue Form). Aus G4 (RE2 CDEMD0.EMS kind 0x34 rec 3,
Datei 0x677000) kommen je Tick-Clip nur Bildzahl, Spur und EDD-Treffer-/Tonbits (s3_clips.py Kopf), damit die Bildfenster
des Ticks (birkin_bahnsteig.c: k_doppel @0x8010b0f0, Treffer @0x80101cb8 / @0x80104f58, Griff Bild 8 @0x80105408, Zucken
ab Bild 5 @0x80107224) unveraendert gelten. Wo eine Pose ein Bildfenster bedient, steht das Fenster dabei.
"""
import math
import numpy as np
import s3_rig as S
import form_emd as FE

D = math.radians
C_RENNEN, C_HIEB0, C_HIEB1, C_ZUCKEN, C_NACHHIEB, C_DROHEN, C_BISS, C_GRIFF = 0, 3, 4, 8, 9, 10, 26, 27


def glatt(t):
    t = max(0.0, min(1.0, t))
    return t * t * (3 - 2 * t)


def worte_g4(bau, c):
    return [x & 0xFFFFF000 for x in bau.g4_worte(c)]


def mit_spur(posen, sp):
    for f, p in enumerate(posen):
        p['spur'] = np.array(sp[f], float).copy()
    return posen


def exakt(r, p):
    q = r.kopie(p)
    q['roh'] = p['roh']
    return q


# ═══════════════════════════════════════════════════ Fussarbeit fuer Angriffe
def fuesse_auto(bau, posen, spur_x, glieder=('LH', 'RH', 'LV', 'RV'), frei=None, schwelle=420.0, dauer=8, hub=260.0, fest=(),
                rampe=5):
    """Stehende Glieder bleiben in der WELT fest (der Aktor laeuft um die Spur weiter, bb_schieben). Weicht ein Glied mehr als
    `schwelle` von seiner Lage in der Schluesselpose ab, macht es einen Schritt (dauer Bilder, Bogen hub) auf die Lage von Bild
    f + dauer. frei = {glied: [(von, bis)]}: dort fuehrt die Schluesselpose das Glied (Hieb-Arm, Griff); an den Raendern wird
    ueber `rampe` Bilder zwischen IK und Schluesselpose gemischt (slerp je Gliedknochen), ausserhalb setzt das Glied dort auf, wo
    die Pose es beim Verlassen hat. fest = Bilder, die unveraendert bleiben (Posen von J). Je Paar hoechstens ein Schritt."""
    r = bau.r
    n = len(posen)
    key = [r.kopie(p) for p in posen]
    for k, p in zip(key, posen):
        k['roh'] = p.get('roh')
    soll = []
    for p in key:
        R, T = r.fk(p)
        soll.append({g: T[bau.glieder[g][2]].copy() for g in glieder})
    frei = frei or {}
    A = list(spur_x)

    def gewicht(g, f):
        if f in fest:
            return 1.0
        w = 0.0
        for a, b in frei.get(g, []):
            if a <= f <= b:
                return 1.0
            if a - rampe < f < a:
                w = max(w, glatt((f - (a - rampe)) / float(rampe)))
            if b < f < b + rampe:
                w = max(w, glatt(1.0 - (f - b) / float(rampe)))
        return w
    welt, schritt = {}, {}
    for g in glieder:
        welt[g] = soll[0][g] + np.array([A[0], 0, 0])
        welt[g][1] = bau.neutral[g][1]
    for f in range(n):
        if f in fest:
            for g in glieder:
                welt[g] = soll[f][g] + np.array([A[f], 0, 0])
                welt[g][1] = bau.neutral[g][1]
            continue
        for g in glieder:
            w = gewicht(g, f)
            if w >= 1.0:
                # Glied frei: beim Wiederaufsetzen landet es dort, wo die Pose es am Ende der Rampe hat
                nach = [b for a, b in frei.get(g, []) if a <= f <= b]
                fb = min(n - 1, (nach[0] if nach else f) + rampe)
                welt[g] = soll[fb][g] + np.array([A[fb], 0, 0])
                welt[g][1] = bau.neutral[g][1]
                schritt.pop(g, None)
                continue
            if g in schritt:
                a, b, x0, x1 = schritt[g]
                w_lin = min(1.0, (f - a) / float(b - a))
                ziel = x0 + (x1 - x0) * glatt(w_lin)
                ziel[1] = bau.neutral[g][1] - hub * math.sin(math.pi * w_lin)
                ziel = ziel - np.array([A[f], 0, 0])
                if f >= b:
                    welt[g] = x1.copy()
                    schritt.pop(g)
            else:
                ziel = welt[g] - np.array([A[f], 0, 0])
                abw = np.linalg.norm((ziel - soll[f][g])[[0, 2]])
                paar = ('LH', 'RH') if g in ('LH', 'RH') else ('LV', 'RV')
                if w == 0.0 and abw > schwelle and not any(h in schritt for h in paar if h != g) and f < n - 3:
                    fb = min(n - 1, f + dauer)
                    x1 = soll[fb][g] + np.array([A[fb], 0, 0])
                    x1[1] = bau.neutral[g][1]
                    schritt[g] = (f, f + dauer, welt[g].copy(), x1)
            bau.glied_ziel(posen[f], g, ziel)
            if w > 0.0:
                for kn in bau.glieder[g]:
                    posen[f]['w'][kn] = S.slerp_m(posen[f]['w'][kn], key[f]['w'][kn], w)
                posen[f]['roh'] = None
    return posen


# ═══════════════════════════════════════════════════ Rennen (Clip 0): Sprunggalopp, Bild 11 = Pose Bild 1
RENNEN_BILD1 = 11


def _kurve(punkte, f):
    """Stueckweise geglaettete Interpolation durch (bild, wert)-Punkte (Bilder aufsteigend)."""
    for (a, va), (b, vb) in zip(punkte, punkte[1:]):
        if a <= f <= b:
            t = 0.0 if b == a else (f - a) / float(b - a)
            return va + (vb - va) * glatt(t)
    return punkte[-1][1]


def rennen(bau):
    """Sprunggalopp, ein Zyklus = 18 Bilder (G4 Clip 0). Koerper aus Schluesselwerten, Glieder je Bild per IK:
    stehende Glieder sind in der Welt fest (Aktor laeuft um die G4-Spur weiter), Schwung ueber Stuetzpunkte.
    Basis aller Bilder ist Pose Bild 1 (Knie-/Ellbogenebenen von Bild 1); Bild 11 IST Pose Bild 1."""
    r = bau.r
    sp = bau.g4_spur(C_RENNEN)
    n = len(sp)
    N = bau.neutral
    A0 = [s_[0] for s_ in sp]
    tempo = [A0[0]] + [A0[i] - A0[i - 1] for i in range(1, n)]
    gesamt = A0[-1]

    def A(t):                                   # Aktorweg bis Bild t (ganzzahlig, ueber den Zyklus hinaus)
        k, u = divmod(t, n)
        return k * gesamt + A0[u]
    R1, T1 = r.fk(r.bild1)
    B = RENNEN_BILD1
    # Koerper (Versatz gegen Pose Bild 1): nick + = Nase runter, hoch + = nach oben
    koerper = dict(nick=[(0, -2), (3, -8), (6, -3), (8, 4), (B, 0), (14, 6), (18, -2)],
                   hoch=[(0, -40), (3, 120), (6, 260), (8, 110), (B, 0), (14, 170), (18, -40)],
                   ruecken=[(0, 2), (3, -4), (6, -6), (8, 3), (B, 0), (14, 5), (18, 2)],
                   kopf=[(0, 4), (3, -5), (6, -3), (8, 5), (B, 0), (14, 6), (18, 4)],
                   kiefer=[(0, 6), (3, 10), (6, 16), (8, 12), (B, 0), (14, 8), (18, 6)])
    # Glieder: Stand [von, bis] (Bilder, ueber 18 hinaus = naechster Zyklus) und Schwung-Stuetzpunkte (Bild, x rel. Ruhe, Hub)
    hinten = dict(stand=(16, 21), land_x=650.0, luft=[(9, -1350.0, 330.0), (B, None, None), (14, -250.0, 360.0)])
    vorn = dict(stand=(8, 12), land_x=None, luft=[(14, -500.0, 300.0), (18 + 3, 450.0, 430.0), (18 + 6, 900.0, 260.0)])
    plan = {'LH': (hinten, 0), 'RH': (hinten, 0), 'LV': (vorn, 0), 'RV': (vorn, 0)}

    def glied_bahn(g, f):
        d, _ = plan[g]
        s0, s1 = d['stand']
        ny = N[g][1]
        if g in ('LV', 'RV'):
            # Landepunkt so, dass das Glied in Bild 11 genau dort steht, wo Pose Bild 1 es hat
            land = T1[bau.glieder[g][2]][0] + (A(B) - A(s0))
        else:
            land = N[g][0] + d['land_x']
        # Stand: Weltpunkt fest
        for k in (-1, 0, 1):
            a, b = s0 + 18 * k, s1 + 18 * k
            if a <= f <= b:
                x = land - (A(f) - A(a))
                return np.array([x, ny, N[g][2]]), 0.0, True
        # Schwung: vom Abheben (letztes Standbild) ueber die Stuetzpunkte zum naechsten Aufsetzen
        for k in (-1, 0):
            e = s1 + 18 * k
            nxt = s0 + 18 * (k + 1)
            if e <= f <= nxt:
                x_ab = land - (A(e) - A(s0 + 18 * k))
                pts_x, pts_y = [(e, x_ab)], [(e, 0.0)]
                for (fb, x, hub) in d['luft']:
                    fb2 = fb + 18 * k if fb + 18 * k > e else fb + 18 * (k + 1)
                    if not (e < fb2 < nxt):
                        continue
                    if x is None:                  # Pose Bild 1
                        x = T1[bau.glieder[g][2]][0]
                        hub = ny - T1[bau.glieder[g][2]][1]
                    else:
                        x = N[g][0] + x
                    pts_x.append((fb2, x))
                    pts_y.append((fb2, hub))
                pts_x.sort()
                pts_y.sort()
                pts_x.append((nxt, land))
                pts_y.append((nxt, 0.0))
                x = _kurve(pts_x, f)
                hub = _kurve(pts_y, f)
                return np.array([x, ny - hub, N[g][2]]), hub, False
        raise RuntimeError('Bild %d ohne Bahn fuer %s' % (f, g))
    posen = []
    for f in range(n):
        if f == B:
            posen.append(exakt(r, r.bild1))
            continue
        p = bau.p(r.bild1)
        bau.koerper(p, nick=_kurve(koerper['nick'], f), hoch=_kurve(koerper['hoch'], f))
        bau.ruecken(p, nick=_kurve(koerper['ruecken'], f))
        bau.kopf(p, nick=_kurve(koerper['kopf'], f), kiefer=_kurve(koerper['kiefer'], f))
        bau.schwanz(p, gier=6, nick=4, phase=2 * math.pi * f / n)
        for g in ('LH', 'RH', 'LV', 'RV'):
            ziel, hub, steht = glied_bahn(g, f)
            if g in ('LH', 'RH'):
                w = 0.0 if steht else max(0.0, min(1.0, hub / 250.0))
                if abs(f - B) <= 2:
                    w = 1.0                         # um Bild 11 die Fussdrehung von Pose Bild 1
                rw = S.slerp_m(bau.ende_R[g], R1[bau.glieder[g][2]], w)
            else:
                rw = R1[bau.glieder[g][2]] if steht else S.rz(D(25 * min(1.0, hub / 300.0))) @ R1[bau.glieder[g][2]]
            bau.glied_ziel_boden(p, g, ziel, ende_welt=rw)
        posen.append(p)
    return mit_spur(posen, sp), worte_g4(bau, C_RENNEN)


# ═══════════════════════════════════════════════════ Hieb (Clip 3 rechts = Teil 18, Clip 4 links = Teil 13)
def hieb(bau, seite):
    c = C_HIEB0 if seite == 'R' else C_HIEB1
    sp = bau.g4_spur(c)
    n = len(sp)
    sg = 1.0 if seite == 'R' else -1.0
    arm = 'RV' if seite == 'R' else 'LV'

    def k_ausholen(t):
        p = bau.p()
        bau.koerper(p, nick=-10 * t, hoch=220 * t, gier=8 * sg * t)
        bau.ruecken(p, nick=-8 * t, gier=10 * sg * t, roll=-6 * sg * t)
        bau.kopf(p, nick=-14 * t, gier=-10 * sg * t, kiefer=22 * t)
        bau.schwanz(p, hebung=10 * t)
        # Arm hoch und weit zur Seite: ausserhalb der Trefferprobe (Teil 13/18 Radius 1700 bzw. (0,600,0) Radius 1500 bei
        # Spieler 2600 voraus), damit der Einstieg des Doppelschlags (Clip 3 Bild 20 / Clip 4 Bild 7) nicht schon trifft
        bau.arm_frei(p, seite, schulter=(-90 * sg * t, 0, -50 * t), ellbogen=-20 * t, hand=(0, 0, -20 * t))
        bau.klauen(p, seite, -18 * t)
        return p

    def k_schlag(t):
        u = 1 - t
        p = bau.p()
        bau.koerper(p, nick=8 * t - 10 * u, hoch=220 * u - 60 * t, gier=-10 * sg * t + 8 * sg * u)
        bau.ruecken(p, nick=10 * t - 8 * u, gier=-16 * sg * t + 10 * sg * u, roll=6 * sg * t)
        bau.kopf(p, nick=10 * t - 14 * u, gier=8 * sg * t, kiefer=26)
        bau.schwanz(p, hebung=6, gier=10 * sg, phase=1.5)
        bau.arm_frei(p, seite, schulter=(-90 * sg * u + 25 * sg * t, 15 * sg * t, -50 * u - 65 * t),
                     ellbogen=-20 * u + 5 * t, hand=(0, 0, -20 * u + 30 * t))
        bau.klauen(p, seite, -18 * u + 10 * t)
        return p
    if seite == 'R':
        # G4 Clip 3: zurueck Bild 1-16, Ausfall 18-35, Treffer 26-30; Doppel: Austritt Bild 30, Einstieg Bild 20
        keys = [(0, bau.p()), (14, k_ausholen(1.0)), (21, k_ausholen(0.9)), (26, k_schlag(0.55)), (29, k_schlag(1.0)),
                (34, k_schlag(1.05)), (46, k_ausholen(0.15)), (59, bau.p())]
        frei = {arm: [(2, 40)]}
    else:
        # G4 Clip 4: Ausfall ab Bild 1, Treffer 14-17; Doppel: Austritt Bild 19, Einstieg Bild 7
        keys = [(0, bau.p()), (8, k_ausholen(1.0)), (13, k_schlag(0.45)), (16, k_schlag(1.0)), (21, k_schlag(1.05)),
                (34, k_ausholen(0.15)), (49, bau.p())]
        frei = {arm: [(2, 32)]}
    posen = mit_spur(bau.zeitleiste(n, keys), sp)
    fuesse_auto(bau, posen, [s[0] for s in sp], frei=frei)
    return posen, worte_g4(bau, c)


# ═══════════════════════════════════════════════════ Zucken (Clip 8, Tick steigt bei Bild 5 ein)
def zucken(bau):
    sp = bau.g4_spur(C_ZUCKEN)
    n = len(sp)

    def k(t, kiefer=0):
        p = bau.p()
        bau.koerper(p, nick=-12 * t, hoch=160 * t, roll=5 * t)
        bau.ruecken(p, nick=-10 * t, gier=8 * t, roll=-4 * t)
        bau.kopf(p, nick=-28 * t, gier=14 * t, kiefer=kiefer)
        bau.schwanz(p, hebung=18 * t, gier=8 * t, phase=1.0)
        bau.arm_frei(p, 'R', schulter=(-10 * t, 0, -28 * t), ellbogen=-20 * t)
        bau.arm_frei(p, 'L', schulter=(14 * t, 0, -22 * t), ellbogen=-24 * t)
        bau.klauen(p, 'R', -14 * t)
        bau.klauen(p, 'L', -14 * t)
        return p
    keys = [(0, bau.p()), (5, k(0.15, 6)), (9, k(1.0, 30)), (14, k(0.7, 22)), (21, k(0.25, 8)), (29, bau.p())]
    posen = mit_spur(bau.zeitleiste(n, keys), sp)
    # Der Tick schiebt im Zucken nicht (BB_ZUCKEN: nur bb_anim, @0x8010721c-68): Fuesse stehen im Aktorraum fest.
    fuesse_auto(bau, posen, [0.0] * n, glieder=('LH', 'RH'))
    fuesse_auto(bau, posen, [0.0] * n, glieder=('LV', 'RV'), frei={'LV': [(6, 22)], 'RV': [(6, 22)]})
    return posen, worte_g4(bau, C_ZUCKEN)


# ═══════════════════════════════════════════════════ Nachhieb (Clip 9): beidhaendig auf den Boden, Bruellen
def nachhieb(bau):
    sp = bau.g4_spur(C_NACHHIEB)
    n = len(sp)

    def hoch(t):
        p = bau.p()
        bau.koerper(p, nick=-16 * t, hoch=420 * t)
        bau.ruecken(p, nick=-12 * t)
        bau.kopf(p, nick=-18 * t, kiefer=18 * t)
        bau.schwanz(p, hebung=-6 * t)
        for s_ in ('R', 'L'):
            sg = 1 if s_ == 'R' else -1
            bau.arm_frei(p, s_, schulter=(-25 * sg * t, 0, -135 * t), ellbogen=-55 * t, hand=(0, 0, -25 * t))
            bau.klauen(p, s_, -15 * t)
        return p

    def schlag():
        p = bau.p()
        bau.koerper(p, nick=9, hoch=-140)
        bau.ruecken(p, nick=8)
        bau.kopf(p, nick=10, kiefer=10)
        bau.schwanz(p, hebung=10)
        return p

    def bruellen(t):
        p = bau.p()
        bau.koerper(p, nick=-6 * t, hoch=80 * t)
        bau.ruecken(p, nick=-6 * t)
        bau.kopf(p, nick=-30 * t, kiefer=38 * t, gier=6 * t)
        bau.schwanz(p, hebung=12 * t, gier=10 * t, phase=0.5)
        return p
    keys = [(0, bau.p()), (11, hoch(1.0)), (17, hoch(1.05)), (24, schlag()), (27, schlag()), (35, bruellen(1.0)),
            (45, bruellen(1.0)), (62, bau.p())]
    posen = mit_spur(bau.zeitleiste(n, keys), sp)
    fuesse_auto(bau, posen, [s[0] for s in sp], glieder=('LH', 'RH'))
    fuesse_auto(bau, posen, [s[0] for s in sp], glieder=('LV', 'RV'), frei={'LV': [(3, 23)], 'RV': [(3, 23)]})
    return posen, worte_g4(bau, C_NACHHIEB)


def schritt_zu(bau, posen, g, a, b, ziel_pos, ziel_R, hub=480.0):
    """Glied g macht zwischen Bild a und b einen Schritt (Bogen hub) von seiner Lage in posen[a] auf ziel_pos (Aktorraum),
    die Enddrehung mischt von der in posen[a] auf ziel_R."""
    r = bau.r
    R0, T0 = r.fk(posen[a])
    k = bau.glieder[g]
    p0, r0 = T0[k[2]].copy(), R0[k[2]].copy()
    for f in range(a, b + 1):
        if f > a:                                   # IK startet von der Loesung des Vorbilds: gleiche Beugeebene, kein Umklappen
            for kn in k[:2]:
                posen[f]['w'][kn] = posen[f - 1]['w'][kn].copy()
        w_lin = (f - a) / float(b - a)
        w = glatt(w_lin)
        z = p0 + (np.array(ziel_pos, float) - p0) * w
        z[1] -= hub * math.sin(math.pi * w_lin)
        bau.glied_ziel(posen[f], g, z, ende_welt=S.slerp_m(r0, ziel_R, w))


def kette_mischen(bau, posen, g, a, b, ziel):
    """Bilder a..b: Gliedkette g von der IK-Loesung (Endpunkt bleibt) ueber eine Rampe auf die Kette der Pose `ziel` mischen."""
    for f in range(a, b + 1):
        w = glatt((f - a + 1) / float(b - a + 1))
        for kn in bau.glieder[g]:
            posen[f]['w'][kn] = S.slerp_m(posen[f]['w'][kn], ziel['w'][kn], w)
        posen[f]['roh'] = None


# ═══════════════════════════════════════════════════ Drohen (Clip 10, Auftritt): Bild 45-52 = Pose Bild 2
DROHEN_BILD2 = (45, 52)
KNIE_FALTE = 60.0


def drohen(bau):
    r = bau.r
    sp = bau.g4_spur(C_DROHEN)
    n = len(sp)

    def duck(t, kiefer=0):
        p = bau.p()
        bau.koerper(p, nick=-6 * t, hoch=-260 * t)
        bau.ruecken(p, nick=-6 * t)
        bau.kopf(p, nick=-16 * t, kiefer=kiefer)
        bau.schwanz(p, hebung=-8 * t, gier=6 * t, phase=1.2)
        bau.boden_halten(p)
        return p

    def auf(t, kiefer=40):
        p = bau.p()
        bau.koerper(p, nick=-32 * t, hoch=900 * t)
        bau.ruecken(p, nick=-14 * t)
        bau.kopf(p, nick=-22 * t, kiefer=kiefer)
        bau.schwanz(p, hebung=-14 * t)
        for s_ in ('R', 'L'):
            sg = 1 if s_ == 'R' else -1
            bau.arm_frei(p, s_, schulter=(-55 * sg * t, 0, -95 * t), ellbogen=-35 * t, hand=(0, 0, -20 * t))
            bau.klauen(p, s_, -20 * t)
        bau.boden_halten(p, glieder=('LH', 'RH'))
        return p
    a, b = DROHEN_BILD2
    keys = [(0, bau.p()), (18, duck(0.6, 6)), (30, duck(1.0, 40)), (a, exakt(r, r.bild2)), (b, exakt(r, r.bild2)),
            (66, duck(0.8, 20)), (80, auf(1.0)), (86, auf(1.02, 44)), (96, duck(0.9, 14)), (110, duck(0.4, 26)),
            (125, bau.p())]
    # Spur: in Bild 45-52 steht Pose Bild 2 still; das G4-Tempo dieser Bilder (zusammen rund -400) wandert in Bild 53-60,
    # die Summe des Clips bleibt die von G4 (PORT-WAHL, das Tempo im Auftritt bedient keine Schwelle des Ticks).
    tempo = [sp[0][0]] + [sp[i][0] - sp[i - 1][0] for i in range(1, n)]
    rest = sum(tempo[a:b + 1])
    for f in range(a, b + 1):
        tempo[f] = 0
    for f in range(b + 1, b + 9):
        tempo[f] += rest / 8.0
    acc, sp2 = 0.0, []
    for f in range(n):
        acc += tempo[f]
        sp2.append(np.array([round(acc), sp[f][1], sp[f][2]], float))
    sp = sp2
    posen = mit_spur(bau.zeitleiste(n, keys), sp)
    # Beine ziehen auf dem Weg in die Hocke von Bild 2 und zurueck an (Knie beugt staerker), sonst streifen sie durch den Boden
    for (f0, f1) in ((30, 44), (53, 67)):
        for f in range(f0 + 1, f1):
            w = (f - f0) / float(f1 - f0)
            for kn, ver in ((24, 0.0), (26, 0.25)):
                u = min(1.0, max(0.0, (w - ver * 0.5) / (1 - ver * 0.5)))
                r.dreh_lokal(posen[f], kn, S.rz(D(KNIE_FALTE * math.sin(math.pi * u))))
    for f in list(range(31, a)) + list(range(b + 1, 67)):
        posen[f]['glieder_frei'] = ('LH', 'RH')         # Beine im Uebergang: Bodenfreiheit auch am Knie
    fest = set(range(a, b + 1))
    um_b2 = (32, 62)
    fuesse_auto(bau, posen, [s[0] for s in sp], glieder=('LH', 'RH'), fest=fest, frei={'LH': [(31, 66)], 'RH': [(31, 66)]})
    fuesse_auto(bau, posen, [s[0] for s in sp], glieder=('LV', 'RV'), fest=fest,
                frei={'LV': [um_b2, (72, 92)], 'RV': [um_b2, (72, 92)]})
    return posen, worte_g4(bau, C_DROHEN)


# ═══════════════════════════════════════════════════ Biss (Clip 26): Treffer 16-30
def biss(bau):
    sp = bau.g4_spur(C_BISS)
    n = len(sp)

    def zurueck(t):
        p = bau.p()
        bau.koerper(p, nick=-8 * t, hoch=120 * t)
        bau.ruecken(p, nick=-6 * t)
        bau.kopf(p, nick=-24 * t, kiefer=30 * t)
        bau.schwanz(p, hebung=8 * t)
        bau.boden_halten(p)
        return p

    def vor(kiefer, gier=0.0):
        p = bau.p()
        p['wurzel'][0] += 520
        bau.koerper(p, nick=12, hoch=-120)
        bau.ruecken(p, nick=10, gier=gier * 0.3)
        bau.kopf(p, nick=14, kiefer=kiefer, gier=gier)
        bau.schwanz(p, hebung=-4, gier=gier * 0.5, phase=0.3)
        bau.boden_halten(p)
        return p
    keys = [(0, bau.p()), (10, zurueck(1.0)), (14, vor(42)), (16, vor(-2)), (19, vor(0, 14)), (22, vor(0, -14)),
            (25, vor(0, 12)), (28, vor(2, -10)), (31, vor(24)), (34, bau.p())]
    return mit_spur(bau.zeitleiste(n, keys), sp), worte_g4(bau, C_BISS)


# ═══════════════════════════════════════════════════ Griff (Clip 27): Griff bei Bild 8, Wegschleudern, Bruellen
GRIFF_BILD = 8


def griff(bau):
    sp = bau.g4_spur(C_GRIFF)
    n = len(sp)

    def greifen(t, zu):
        p = bau.p()
        p['wurzel'][0] += 650 * t
        bau.koerper(p, nick=8 * t, hoch=40 * t)
        bau.ruecken(p, nick=4 * t)
        bau.kopf(p, nick=6 * t, kiefer=30 * t)
        for s_ in ('R', 'L'):
            sg = 1 if s_ == 'R' else -1
            bau.arm_frei(p, s_, schulter=(-28 * sg * t + 30 * sg * zu, 0, -78 * t), ellbogen=-10 * t, hand=(0, 0, -10 * t))
            bau.klauen(p, s_, -22 * t + 40 * zu)
        bau.boden_halten(p, glieder=('LH', 'RH'))
        return p

    def schleudern(t):
        p = bau.p()
        bau.koerper(p, nick=-22 * t, hoch=520 * t, gier=-12 * t)
        bau.ruecken(p, nick=-12 * t, gier=-14 * t)
        bau.kopf(p, nick=-20 * t, kiefer=34 * t, gier=-10 * t)
        bau.schwanz(p, hebung=10 * t, gier=14 * t, phase=0.8)
        bau.arm_frei(p, 'R', schulter=(20 * t, 25 * t, -150 * t), ellbogen=-30 * t)
        bau.arm_frei(p, 'L', schulter=(-25 * t, 0, -120 * t), ellbogen=-40 * t)
        bau.klauen(p, 'R', 10 * t)
        bau.klauen(p, 'L', -10 * t)
        bau.boden_halten(p, glieder=('LH', 'RH'))
        return p

    def bruellen(t, gier=0.0, arm=0.0):
        p = bau.p()
        bau.koerper(p, nick=-12 * t, hoch=260 * t)
        bau.ruecken(p, nick=-8 * t, gier=gier * 0.4)
        bau.kopf(p, nick=-32 * t, kiefer=44 * t, gier=gier)
        bau.schwanz(p, hebung=12 * t, gier=12, phase=gier * 0.1)
        if arm:
            s_ = 'R' if arm > 0 else 'L'
            sg = 1 if s_ == 'R' else -1
            bau.arm_frei(p, s_, schulter=(-30 * sg * abs(arm), 0, -110 * abs(arm)), ellbogen=-60 * abs(arm))
        bau.boden_halten(p, glieder=('LH', 'RH'))
        return p
    keys = [(0, bau.p()), (5, greifen(1.0, 0)), (GRIFF_BILD, greifen(1.0, 1.0)), (16, schleudern(1.0)), (26, schleudern(0.3)),
            (36, bruellen(1.0, 8, 0.9)), (46, bruellen(1.0, -8, -0.9)), (56, bruellen(1.0, 8, 0.9)), (66, bruellen(1.0, -6, 0)),
            (82, bruellen(0.9, 10, 0)), (99, bau.p())]
    posen = mit_spur(bau.zeitleiste(n, keys), sp)    # Spur = Opfer-Spur (FUN_80015cb8 @0x80105398), der Griff schiebt nicht
    fuesse_auto(bau, posen, [0.0] * n, glieder=('LV', 'RV'),
                frei={'LV': [(1, 40), (44, 60)], 'RV': [(1, 40), (32, 46), (52, 62)]})
    return posen, worte_g4(bau, C_GRIFF)


# ═══════════════════════════════════════════════════ Flucht (Clip 11, Ende 5080): Zucken, Galopp, Sprung nach oben
FLUCHT_N = 57


def flucht(bau, renn):
    r = bau.r
    n = FLUCHT_N

    def zuck(t):
        p = bau.p()
        bau.koerper(p, nick=-14 * t, hoch=200 * t, roll=6 * t)
        bau.ruecken(p, nick=-10 * t, gier=-10 * t)
        bau.kopf(p, nick=-30 * t, gier=-16 * t, kiefer=42 * t)
        bau.schwanz(p, hebung=16 * t, gier=10 * t, phase=0.4)
        bau.boden_halten(p)
        return p

    def duck():
        p = bau.p()
        bau.koerper(p, nick=8, hoch=-380)
        bau.ruecken(p, nick=6)
        bau.kopf(p, nick=-10, kiefer=10)
        bau.schwanz(p, hebung=-10)
        bau.boden_halten(p)
        return p

    def sprung(t):
        p = bau.p()
        bau.koerper(p, nick=-28 * t, hoch=6200 * t)
        bau.ruecken(p, nick=-8 * t)
        bau.kopf(p, nick=-14 * t, kiefer=16)
        bau.schwanz(p, hebung=-20 * t)
        for s_ in ('R', 'L'):
            sg = 1 if s_ == 'R' else -1
            bau.arm_frei(p, s_, schulter=(-12 * sg * t, 0, -150 * t), ellbogen=-25 * t)
        bau.bein_frei(p, 'L', huefte=(0, 0, 40 * t), knie=-25 * t, fuss=(0, 0, -40 * t))
        bau.bein_frei(p, 'R', huefte=(0, 0, 44 * t), knie=-25 * t, fuss=(0, 0, -40 * t))
        return p
    # 0-15 Schmerz und Gebruell am Platz (Tempo 0), 16-36 Galopp = Clip 0 Bild 1..21 (endet im Abstoss Bild 3),
    # danach Sprung nach oben (Wurzel steigt um 6200, die Form verschwindet nach oben aus dem Bild); Laenge FLUCHT_N.
    m = len(renn)
    teil1 = bau.zeitleiste(16, [(0, bau.p()), (6, zuck(1.0)), (11, zuck(0.7)), (15, r.kopie(renn[0]))])
    galopp = list(range(1, 22))
    teil2 = [exakt(r, renn[f % m]) if renn[f % m].get('roh') else r.kopie(renn[f % m]) for f in galopp]
    rest = n - len(teil1) - len(teil2)
    teil3 = bau.zeitleiste(rest, [(0, sprung(0.08)), (4, sprung(0.35)), (12, sprung(1.0)), (rest - 1, sprung(1.0))])
    posen = teil1 + teil2 + teil3
    sp0 = [s_[0] for s_ in bau.g4_spur(C_RENNEN)]
    t0 = [sp0[0]] + [sp0[i] - sp0[i - 1] for i in range(1, len(sp0))]
    tempo = [0] * len(teil1) + [t0[f % m] for f in galopp] + [300] * rest
    acc = 0
    for f, p in enumerate(posen):
        acc += tempo[f]
        p['spur'] = np.array([acc, 0, 0], float)
    worte = [0] * n
    worte[5] = FE.ton(10)                                 # Gebruell, Id 10 wie G4 Clip 10 Bild 51 (PORT-WAHL)
    worte[len(teil1) + len(teil2)] = FE.ton(1)            # Absprung, Id 1 wie G4 Clip 0 Bild 17 (PORT-WAHL)
    return posen, worte


# ═══════════════════════════════════════════════════ Sturz (Clip 12, Ende 5090): aus dem Zug geschossen, Bild 0 = Pose Bild 2
STURZ_N = 56


def sturz(bau):
    r = bau.r
    n = STURZ_N

    von2 = bau.lage_von(r.bild2)

    def wanken(t):
        p = bau.p(r.bild2)
        bau.koerper(p, nick=-24 * t, hoch=320 * t, roll=-8 * t)
        bau.ruecken(p, nick=-14 * t, gier=10 * t)
        bau.kopf(p, nick=-30 * t, kiefer=40 * t, gier=12 * t)
        bau.schwanz(p, hebung=-6 * t, gier=16 * t, phase=0.3)
        bau.arm_frei(p, 'R', schulter=(-20 * t, 0, -70 * t), ellbogen=-30 * t)
        bau.arm_frei(p, 'L', schulter=(25 * t, 0, -60 * t), ellbogen=-40 * t)
        bau.boden_halten(p, glieder=('LH', 'RH'), von=von2)
        return p

    def kippen(t, fall):
        p = bau.p(r.bild2)
        p['wurzel'][1] = r.ruhe['wurzel'][1]
        bau.koerper(p, nick=-(40 + 50 * t), hoch=600 - fall, roll=-14 * t)
        bau.ruecken(p, nick=-16, gier=6)
        bau.kopf(p, nick=-20, kiefer=44, gier=-8 * t)
        bau.schwanz(p, hebung=-30 * t, gier=8, phase=1.0)
        bau.arm_frei(p, 'R', schulter=(-30, 0, -70 - 60 * t), ellbogen=-20)
        bau.arm_frei(p, 'L', schulter=(35, 0, -60 - 55 * t), ellbogen=-30)
        bau.bein_frei(p, 'L', huefte=(0, 0, -30 * t), knie=20 * t)
        bau.bein_frei(p, 'R', huefte=(0, 0, -45 * t), knie=10 * t)
        return p
    keys = [(0, exakt(r, r.bild2)), (8, wanken(1.0)), (16, wanken(1.1)), (26, kippen(0.2, 0)), (36, kippen(0.7, 1800)),
            (n - 1, kippen(1.0, 6000))]
    posen = bau.zeitleiste(n, keys)
    for f, p in enumerate(posen):
        if f >= 24:
            p['boden_frei'] = False          # faellt rueckwaerts aus dem Wagen, darf unter y = 0
    tempo = [0] * 3 + [-90] * 13 + [-160] * 12 + [-220] * (n - 28)
    acc = 0
    for f, p in enumerate(posen):
        acc += tempo[f]
        p['spur'] = np.array([acc, 0, 0], float)
    worte = [0] * n
    worte[2] = FE.ton(10)                                 # Gebruell, Id 10 (PORT-WAHL)
    return posen, worte
