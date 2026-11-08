"""SF V4 Runde 3: T7 Gang in KOERPERFESTEN Koordinaten und Boden mit Mesh-Ausdehnung (Entscheid O3 des Orchestrators).

    python t7_koerperfest_v4.py <EMD> <debug.log> [--json aus.json]

Anlass (Pruefung V4 r2 Zahlen Abschnitt 3): T7 in Bildhoehe der Vorlage-2-Kamera ist im Lauf nicht vergleichbar, weil die Figur bei h85
im Spiel in clip 11 mit Gier 1893 steht (Vorlage 2: clip 1, Gier 556). Gemessen wird deshalb relativ zum Koerper:
  - Soll: das Feder-Ziel desselben Kettenschritts = Keyframe-Gelenke der GEZEICHNETEN Pose (Welt), Messzeile "[hangar-takt] hN Schwanz
    Ziel 0(..) .. 7(..)" (schwanz_r41.c re15_schwanz_r41_ziel_gelenk, Runde 3). Ruhe = Keyframe (Messblatt V4 Abschnitt 4 Punkt 1/4).
    Ziel und Kette haengen an derselben Wurzel (Gelenk 0, Knochen 8) - die Abweichung ist koerperfest. Die geloggte Clip-Nummer allein
    reicht nicht: h85-h100 wird ueberblendet (Lauf sfv4/r/V4r3a: Wurzel des Keyframes von "clip 1 bild 4" 1300 E. neben der gezeichneten).
  - Kette = "[hangar-takt] hN Schwanz B y Y 0(..) .. 7(..)" (Welt). Laeufe ohne Ziel-Zeile (vor Runde 3): Keyframe des geloggten Bilds im
    Raum von Knochen 8 (Ersatz).
  - H_E = Hoehe der Figur in E. (Koerperecken ohne Schwanz in Pose "85", Modell-y-Spanne), Toleranz 0,06 H wie Riegel T7.
Riegel je Gangtakt (h85 = Vorlage 2 / Messblatt T7, im Lauf clip 11, und alle Takte mit clip 1 = Gang) und ueber den ganzen Lauf:
  T7a Lage: Beginn/Biegung/Knick/Ende (Gelenke 1/3/5/7) je hoechstens 0,06 H_E vom Ziel-Gelenk (3D).
  T7b Knick und Biegung bleiben: Gelenkwinkel an Gelenk 3 >= Keyframe - 10 Grad (SW_BIEGUNG_SPIEL), an Gelenk 5 >= halber Keyframe-Winkel.
  T7c keine waagerechte Kette: Strecke Biegung -> Ende entlang der Ziel-Richtung (Ziel-Gelenk 3 -> 7) >= 0,75 x Keyframe-Strecke.
  T7d keine Rueckfaltung: alle Gelenkwinkel <= 90 Grad.
  Boden: kein Punkt der Glied-Meshes unter dem Boden (Welt-y > B y): je Gelenk y + Radius des Glied-Meshes (groesster Eckenabstand
         zur Achse) und die Dornspitze (Gelenk 6 + Achse x groesste Ecke von Mesh 22 entlang der Achse) - ganzer Lauf.
Die Breite (Breitenprofil) ist starre Geometrie der Glied-Meshes und bleibt in jedem Takt gleich (M-Riegel / T5 an der Ruhe-Pose).
"""
import os, sys, json, re, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v3'))
import v4_gemein as G
import b2
import birkin_buehne as BB
import emd_ansichtsblatt as EA
import riegel_v3 as R3


def lauf_ziel(pfad):
    g = {}
    for z in open(pfad, errors='replace'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz Ziel (.*)', z)
        if m:
            g[int(m.group(1))] = np.array([tuple(map(int, t)) for t in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(2))], float)
    return g


def lauf_boden(pfad):
    B = {}
    for z in open(pfad, errors='replace'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) ', z)
        if m:
            B[int(m.group(1))] = int(m.group(2))
    return B


def mesh_mass(mo):
    """Je Gelenk 0..7 Radius der angrenzenden Glied-Meshes, Dorn = groesste Ecke von Mesh 22 entlang der Achse (Knochenraum)."""
    rad = np.zeros(8); dorn = 0.0
    for g in range(7):
        m = mo.meshes[16 + g]
        V = np.array([v[:3] for v in list(m.tv) + list(m.qv)], float)
        r = float(np.hypot(V[:, 0], V[:, 2]).max())
        rad[g] = max(rad[g], r); rad[g + 1] = max(rad[g + 1], r)
        if g == 6:
            dorn = float(V[:, 1].max())
    return rad, dorn


def kf_gelenke(mo, kf):
    R, T = mo.pose(kf)
    J = [T[16 + j].astype(float) for j in range(7)]
    r22 = np.linalg.norm(np.array(mo.skel.rel[22], float))
    J.append(T[22].astype(float) + R[22].astype(float)[:, 1] / 4096.0 * r22)
    return np.array(J), R, T


def winkel(P):
    w = []
    for i in range(1, len(P) - 1):
        a, b = P[i] - P[i - 1], P[i + 1] - P[i]
        w.append(float(np.degrees(np.arccos(np.clip(a @ b / np.linalg.norm(a) / np.linalg.norm(b), -1, 1)))))
    return np.array(w)


def messen(emd, log):
    mo = b2.modell(emd)
    K, PO, BY, ZI = R3.lauf_lesen(log), R3.lauf_posen(log), lauf_boden(log), lauf_ziel(log)
    p85 = G.posen()['85']
    V, _N, F = mo.geometrie(BB.kf_von_clip(mo, p85['clip'], p85['bild']))
    koerper = sorted({i for f in F if f['bone'] < 16 for i in f['v']})
    HE = float(np.ptp(V[koerper][:, 1]))
    rad, dorn = mesh_mass(mo)
    r22 = np.linalg.norm(np.array(mo.skel.rel[22], float))
    takte = {}
    for h in sorted(K):
        if h not in PO:
            continue
        pl = PO[h]
        kf = BB.kf_von_clip(mo, pl['clip'], pl['bild'])
        Jk, R, T = kf_gelenke(mo, kf)
        g = pl['gier']; s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
        Yl = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
        Pm = (K[h] - np.array([pl['x'], 0.0, pl['z']])) @ Yl          # Welt -> Modell der Laufpose
        R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
        if h in ZI and len(ZI[h]) == 8:                                 # Runde 3: Feder-Ziel der gezeichneten Pose (Welt)
            Zm = (ZI[h] - np.array([pl['x'], 0.0, pl['z']])) @ Yl
            lauf8, kf8 = Pm, Zm                                         # Welt-/Modellraum, gemeinsame Wurzel
        else:
            lauf8 = (Pm - T8) @ R8                                      # Knochenraum 8 (Ersatz)
            kf8 = (Jk - T8) @ R8
        abw = {k: float(np.linalg.norm(lauf8[j] - kf8[j]) / HE) for k, j in (('Beginn', 1), ('Biegung', 3), ('Knick', 5), ('Ende', 7))}
        wl, wk = winkel(lauf8), winkel(kf8)
        # T7c: Abfall Biegung -> Ende entlang der Richtung, in der das Ziel abfaellt (Ziel-Gelenk 3 -> 7, koerperfest)
        u = (kf8[7] - kf8[3]); u = u / max(np.linalg.norm(u), 1e-9)
        abfall_l = float((lauf8[7] - lauf8[3]) @ u) / HE; abfall_k = float((kf8[7] - kf8[3]) @ u) / HE
        # Boden (Welt, y nach unten): Gelenke + Radius, Dornspitze
        P = K[h]; by = BY.get(h, 0)
        d7 = P[7] - P[6]; L7 = np.linalg.norm(d7)
        spitze = P[6] + d7 / max(L7, 1e-9) * dorn * (L7 / r22)
        tief = max(float(np.max(P[1:, 1] + rad[1:])), float(spitze[1]))
        takte[h] = dict(clip=pl['clip'], abw=abw, w3=float(wl[2]), w3k=float(wk[2]), w5=float(wl[4]), w5k=float(wk[4]), wmax=float(wl.max()),
                        abfall=abfall_l, abfall_k=abfall_k, unter_boden=tief - by, spitze_y=float(spitze[1]) - by)
    return dict(HE=HE, rad=rad.tolist(), dorn=dorn, takte=takte)


def riegel(m, ab=85):
    T = m['takte']
    gang = [h for h in T if h >= ab and (T[h]['clip'] == 1 or h == 85)]   # h85 = Vorlage 2 (Messblatt T7), im Lauf clip 11
    Z = []
    def z(name, soll, ist, ok, note=''):
        Z.append(dict(mass=name, soll=soll, ist=ist, ok=bool(ok), note=note))
    sch = {h: max(T[h]['abw'].values()) for h in gang}
    rot = [h for h in gang if sch[h] > 0.06]
    hm = max(sch, key=sch.get) if sch else None
    z('T7a Gang koerperfest Lage', 'Gelenke 1/3/5/7 je <= 0,06 H vom Ziel (Keyframe der gezeichneten Pose), alle Gangtakte',
      '%d von %d Takten > 0,06 H; max %.3f H (h%s %s)' % (len(rot), len(gang), sch[hm] if hm else 0, hm,
                                                           {k: round(v, 3) for k, v in T[hm]['abw'].items()} if hm else ''), not rot)
    b3 = [h for h in gang if T[h]['w3'] < T[h]['w3k'] - 10.0]
    b5 = [h for h in gang if T[h]['w5'] < 0.5 * T[h]['w5k']]
    z('T7b Biegung/Knick im Gang', 'Gelenk 3 >= Keyframe - 10 Grad; Gelenk 5 >= halber Keyframe-Winkel',
      'Biegung %.1f-%.1f Grad (Keyframe %.1f-%.1f), %d Takte darunter; Knick %.1f-%.1f (Keyframe %.1f-%.1f), %d darunter' % (
          min(T[h]['w3'] for h in gang), max(T[h]['w3'] for h in gang), min(T[h]['w3k'] for h in gang), max(T[h]['w3k'] for h in gang),
          len(b3), min(T[h]['w5'] for h in gang), max(T[h]['w5'] for h in gang), min(T[h]['w5k'] for h in gang),
          max(T[h]['w5k'] for h in gang), len(b5)), not b3 and not b5)
    wa = [h for h in gang if T[h]['abfall'] < 0.75 * T[h]['abfall_k']]
    z('T7c keine waagerechte Kette', 'Strecke Biegung -> Ende in Zielrichtung >= 0,75 x Keyframe',
      'min %.3f H (Keyframe %.3f H); %d Takte darunter %s' % (min(T[h]['abfall'] for h in gang), min(T[h]['abfall_k'] for h in gang),
                                                               len(wa), wa[:8]), not wa)
    alle = sorted(T)
    f90 = [h for h in alle if T[h]['wmax'] > 90.0]
    z('T7d keine Rueckfaltung', 'alle Gelenkwinkel <= 90 Grad, ganzer Lauf',
      'max %.1f Grad; %d von %d Takten > 90' % (max(T[h]['wmax'] for h in alle), len(f90), len(alle)), not f90)
    ub = [h for h in alle if T[h]['unter_boden'] > 0]
    hb = max(alle, key=lambda h: T[h]['unter_boden'])
    z('Boden Mesh', 'kein Glied-Mesh und keine Dornspitze unter dem Boden, ganzer Lauf',
      'tiefster Mesh-Punkt %+.0f E. (h%d, Dornspitze %+.0f E.); %d Takte darunter' % (
          T[hb]['unter_boden'], hb, T[hb]['spitze_y'], len(ub)), not ub,
      'Radien je Gelenk %s E., Dorn %.0f E. (Mesh 22)' % ([int(r) for r in m['rad']], m['dorn']))
    return Z


if __name__ == '__main__':
    ap = argparse.ArgumentParser(); ap.add_argument('emd'); ap.add_argument('log'); ap.add_argument('--json')
    a = ap.parse_args()
    m = messen(a.emd, a.log)
    for zl in riegel(m):
        print('%-4s %-30s soll: %-70s ist: %s %s' % ('OK' if zl['ok'] else 'ROT', zl['mass'], zl['soll'], zl['ist'], zl['note']))
    for h in (85, 90, 95, 100, 105, 110, 118, 125, 135):
        if h in m['takte']:
            t = m['takte'][h]
            print('h%d clip %d abw %s w3 %.1f/%.1f w5 %.1f/%.1f abfall %.3f/%.3f boden %+.0f' % (
                h, t['clip'], {k: round(v, 3) for k, v in t['abw'].items()}, t['w3'], t['w3k'], t['w5'], t['w5k'], t['abfall'], t['abfall_k'],
                t['unter_boden']))
    if a.json:
        json.dump(m, open(a.json, 'w'), indent=1)
