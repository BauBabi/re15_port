"""Spur SF "Schwanz V2": Riegel (Formblatt V2 S2/S8/S10, Auftrag D). Rot = Exit-Code 1.

python riegel_v2.py <emd> [laufordner]     Gegenprobe: dasselbe mit EM30_HANGAR.EMD (Referenz 2) muss ROT sein.
Pruefungen (Grenzen = Formblatt-Vorgabe mit Messspielraum, PORT-WAHL der Toleranz):
  R1 Wurzelmitte in der Seitenansicht h85 (Pose anpassen_v2) 0,13-0,19 H unter OK       (S2: 0,15-0,17)
  R2 Wurzeldicke bei 0,2 der Laenge >= 0,11 H                                              (S10: 0,13-0,14)
  R3 Spitze im Gang (Lauf h108-h132, Kette aus der Messzeile) 0,25-0,50 H ueber dem Boden   (S8: 0,32-0,44 im Bild)
     ohne Lauf: Keyframes Clip 1 (Gang), Spitze ueber der Sohle
  R4 kein Boden: kein Gelenk tiefer als sein Radius ueber dem Boden (Lauf h0-h132 bzw. Keyframes Clip 0/1)
  R5 kein Rumpf: kein Schwanz-Vertex der Glieder 2-6 in der konvexen Huelle eines Koerper-Meshes (Keyframes Clip 0/1;
     mit Lauf zusaetzlich die Kette der Messzeile h108-h132 bei der Lauf-Pose, Gelenke 2-7)
  R6 Griff (nur mit Lauf): Abstand Sichelhand - Griffknochen ab h16 im Halten < 150 E. (griff6.py, wie NB2neu.3)
"""
import sys, os, json, re
import numpy as np
from scipy.spatial import Delaunay
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2
import mass_v2 as MS
import birkin_buehne as BB
import birkin_formen as BF

SCR = os.environ.get('SFV2', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2')
fehler = []


def pruefe(ok, text):
    print(('  ok   ' if ok else '  ROT  ') + text)
    if not ok:
        fehler.append(text)


def huellen(mo, kf):
    R, T = mo.pose(kf)
    H = {}
    for b in range(16):
        m = mo.meshes[b]
        L = np.array([v[:3] for v in m.tv], float)
        if len(L) < 5:
            continue
        W = L @ (R[b].astype(float) / 4096.0).T + T[b].astype(float)
        try:
            H[b] = Delaunay(W)
        except Exception:
            pass
    return H, R, T


def schwanz_vertices(mo, R, T, glieder=range(18, 23)):
    out = []
    for b in glieder:
        L = np.array([v[:3] for v in mo.meshes[b].tv], float)
        out.append(L @ (R[b].astype(float) / 4096.0).T + T[b].astype(float))
    return np.vstack(out)


def main():
    emd = sys.argv[1]
    lauf = sys.argv[2] if len(sys.argv) > 2 else None
    mo = b2.modell(emd)
    ref2 = mo.skel.rel[16][0] > -700 if mo.skel.bone_count == 23 else True
    versenkt = 0 if ref2 else BF.SCHWANZ_V2['versenkt']
    print('Riegel Schwanz V2: %s%s' % (os.path.basename(emd), '' if not lauf else ' + Lauf ' + lauf))
    P = json.load(open(os.path.join(SCR, 'posen_seite.json')))['85']
    m = MS.schwanz_mass(mo, P, versenkt=versenkt)
    pruefe(0.13 <= m['wurzel_unter_ok'] <= 0.19, 'R1 Wurzelmitte h85 %.3f H unter OK (0,13-0,19)' % m['wurzel_unter_ok'])
    pruefe(m['dicke'][0.2] >= 0.11, 'R2 Wurzeldicke h85 %.3f H (>= 0,11)' % m['dicke'][0.2])
    # Keyframes Clip 0/1
    rad = []
    for b in range(16, 23):
        L = np.array([v[:3] for v in mo.meshes[b].tv], float)
        rad.append(np.median(np.hypot(L[:, 0], L[:, 2])))
    rad.append(rad[-1] * 0.5)
    spitzen, boden_min, rumpf_max = [], 1e9, 0
    for clip in (0, 1):
        f0, n = mo.clips[clip]
        for i in range(0, n, 2):
            kf = mo.frames[f0 + i] & 0xFFF
            V, _N, F = mo.geometrie(kf)
            sohle = V[:, 1].max(); oben = V[:, 1].min()
            Pk = b2.kette_punkte(mo, kf)
            if clip == 1:
                spitzen.append((sohle - Pk[-1][1]) / (sohle - oben))
            boden_min = min(boden_min, min((sohle - Pk[j][1]) - rad[j] for j in range(8)))
            Hh, R, T = huellen(mo, kf)
            sv = schwanz_vertices(mo, R, T)
            innen = sum(int((h.find_simplex(sv) >= 0).sum()) for h in Hh.values())
            rumpf_max = max(rumpf_max, innen)
    if not lauf:
        pruefe(0.25 <= min(spitzen) and max(spitzen) <= 0.50,
               'R3 Spitze Gang (Keyframes Clip 1) %.2f-%.2f H ueber der Sohle (0,25-0,50)' % (min(spitzen), max(spitzen)))
    pruefe(boden_min >= -5, 'R4 Boden (Keyframes Clip 0/1): kleinster Abstand Gelenk - Radius zur Sohle %.0f E. (>= -5)' % boden_min)
    pruefe(rumpf_max == 0, 'R5 Rumpf (Keyframes Clip 0/1): hoechstens %d Schwanz-Vertices (Glieder 2-6) im Koerper (0)' % rumpf_max)
    if lauf:
        d = os.path.join(SCR, 'r', lauf) if not os.path.isabs(lauf) else lauf
        K, B = {}, {}
        for Lz in open(os.path.join(d, 'debug.log'), errors='ignore'):
            mm = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', Lz)
            if mm:
                K[int(mm.group(1))] = (int(mm.group(2)), np.array([[int(v) for v in t] for t in
                                                                    re.findall(r'\((-?\d+),(-?\d+),(-?\d+)\)', mm.group(3))], float))
            mm = re.search(r'\[hangar-takt\] h(-?\d+) F\d+ B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)', Lz)
            if mm:
                B[int(mm.group(1))] = [int(mm.group(i)) for i in range(2, 7)]
        # Formblatt S8 (0,32-0,44 H) ist in der SEITENansicht h83-h92 gemessen; dort liegt das Bildmass 0,04-0,06 H ueber dem
        # Weltmass (Buehne, leisten_v2/mass_v2: V1 h85 Welt 0,31 / Bild 0,35). Im Gang zur Kamera hebt die Perspektive die
        # Spitze hinter dem Koerper viel staerker (Referenz 2 dort 0,35-0,44 im Bild bei 0,10-0,27 in der Welt) -> Riegel im
        # Weltmass: Mittel 0,26-0,42 H (= S8 minus Perspektive), kleinster Wert >= 0,18 H (Spitze frei, nicht am Boden).
        Vg, _n, Fg = mo.geometrie(mo.frames[mo.clips[1][0]] & 0xFFF)
        kb = sorted({j for ff in Fg if ff['bone'] < 16 for j in ff['v']})
        Hm = float(Vg[kb, 1].max() - Vg[kb, 1].min())
        welt = [(K[h][0] - K[h][1][-1][1]) / Hm for h in range(108, 133) if h in K]
        sp = []
        for h in range(108, 133):
            if h not in K or h not in B:
                continue
            clip, bild, gier, x, z = B[h]
            pose = dict(clip=clip, bild=bild, gier=gier, x=x, z=z)
            s_, c_ = np.sin(gier * 2 * np.pi / 4096), np.cos(gier * 2 * np.pi / 4096)
            J = K[h][1][-1] - np.array([x, K[h][0], z], float)
            jm = np.array([c_ * J[0] - s_ * J[2], J[1], s_ * J[0] + c_ * J[2]])
            km = b2.umriss_schnell(mo, pose, knochen=range(16))
            ys = np.nonzero(km.any(1))[0]
            ty = b2.projiziere(pose, [jm])[0][1]
            sp.append((ys.max() - ty) / float(ys.max() - ys.min()))
        pruefe(len(welt) > 0 and 0.26 <= np.mean(welt) <= 0.42 and min(welt) >= 0.18,
               'R3 Spitze Gang (Lauf h108-h132, Welt) Mittel %.2f H, %.2f-%.2f H ueber dem Boden (Mittel 0,26-0,42, min >= 0,18); '
               'im Bild %.2f-%.2f' % (np.mean(welt), min(welt), max(welt), min(sp), max(sp)))
        bmin = min(min((K[h][0] - K[h][1][j][1]) - rad[j] for j in range(8)) for h in K if 0 <= h <= 132)
        pruefe(bmin >= -5, 'R4 Boden (Lauf h0-h132): kleinster Abstand Gelenk - Radius zum Boden %.0f E. (>= -5)' % bmin)
        innen = 0
        for h in range(108, 133):
            if h not in K or h not in B:
                continue
            clip, bild, gier, x, z = B[h]
            kf = BB.kf_von_clip(mo, clip, bild)
            Hh, R, T = huellen(mo, kf)
            s, c = np.sin(gier * 2 * np.pi / 4096), np.cos(gier * 2 * np.pi / 4096)
            J = K[h][1] - np.array([x, K[h][0], z], float)
            Jm = np.c_[c * J[:, 0] - s * J[:, 2], J[:, 1], s * J[:, 0] + c * J[:, 2]]   # Welt -> Modell (Umkehrung der Gier)
            innen += sum(int((hh.find_simplex(Jm[2:]) >= 0).sum()) for hh in Hh.values())
        pruefe(innen == 0, 'R5 Rumpf (Lauf h108-h132): %d Kettengelenke 2-7 im Koerper (0)' % innen)
        sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'nb2neu'))
        os.environ.setdefault('NB6_RUNS', os.path.join(SCR, 'r'))
        import griff6
        G = griff6.abstaende(d)
        halt = [v['welt'] for h, v in G.items() if 16 <= h <= 70]
        pruefe(len(halt) >= 40 and max(halt) < 150, 'R6 Griff h16-h70: %d Takte, hoechstens %d E. (< 150)' % (len(halt), max(halt) if halt else -1))
    print('ERGEBNIS: %s (%d rot)' % ('GRUEN' if not fehler else 'ROT', len(fehler)))
    sys.exit(1 if fehler else 0)


if __name__ == '__main__':
    main()
