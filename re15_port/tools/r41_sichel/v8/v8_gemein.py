"""SF V8 Koerperbau nach Nutzer-Skizze (Bauer, 2026-11-08): gemeinsame Helfer fuer Kettensuche, Bau, Riegel und Tafeln.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Masse = Bildmessung an der Vorlage
(Version_80%/Darstellung_Vorlage.png = Videobild n1439 = SF_belege/roh/alle/f1440.png, Messblatt V8 Abschnitt 1) und am Modell in der
Pose PV (Clip 17 Bild 47 Gier 587 x -8246 z -21266, Messblatt V8 Abschnitt 1), Spielkamera 50A0 Cut 12 (b2.raum, RDT-Licht).

Achsen (gemessen, sfv8/bau/x2.py, x3.py; Messblatt V8 nennt die Seitenachse x - das ist korrigiert):
  Knochenraum 8 (= Mesh 8): z = Seitenachse ((W)-rechts = z < 0: Oberarm Knochen 13 relpos z -1136, Knochen 10 z +943, Huefte 1/4
  z -229/+229), x = vorn/hinten (Kopf Mesh 9 x -156..353 vorn, Ruecken bis x -1075 hinten), y = entlang der Wirbelsaeule (y < 0 oben).
  In PV steht die Kamera im Knochenraum 8 in Richtung (0,27, -0,50, -0,82): vorn, oben, (W)-rechts.
Nutzerlinien und Vorlage-Daten: analysis/befunde_runde41/SF_belege/v8/messung/daten (Commit 09b04bc1c, nur gelesen).
"""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
R41 = os.path.dirname(HIER)
TOOLS = os.path.dirname(R41)
for _p in (os.path.join(R41, 'v4'), os.path.join(R41, 'v2'), os.path.join(R41, 'v3'), os.path.join(R41, 'foto'),
           os.path.join(R41, 'v6kopf'), os.path.join(R41, 'v6drei'), os.path.join(R41, 'v5koerper'), TOOLS, HIER):
    if _p not in sys.path:
        sys.path.insert(0, _p)
import b2                                   # noqa: E402
import birkin_buehne as BB                  # noqa: E402
import emd_ansichtsblatt as EA              # noqa: E402

BAUM = os.path.dirname(os.path.dirname(TOOLS))
MESS = os.path.join(BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v8', 'messung', 'daten')
V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
ASSET = os.path.join(BAUM, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD')
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
SFV8 = os.environ.get('SFV8', os.path.join(SP, 'sfv8'))
V7_SHA = '319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6'

PV = dict(clip=17, bild=47, gier=587, x=-8246, z=-21266)          # Messblatt V8 Abschnitt 1 (m9_pose.json fein[0])
SCHWANZ = tuple(range(16, 23))
# Messblatt V8 Abschnitt 6a "Ruheverlauf": Vorlage-Mittellinie (3.1) Wurzel (366,215) -> ... -> Ende (283,330)
MITTE = [(366, 215), (316, 238), (302, 262), (293, 280), (278, 297), (285, 314), (283, 330)]
PX_JE_E = 65.4 / 1000.0                                           # Messblatt V8 Kopf: 65,4 px je 1000 E. am Koerper (PV)


def lade(name):
    p = os.path.join(MESS, name)
    return np.load(p) if name.endswith('.npy') else json.load(open(p))


def modell(blob_oder_pfad):
    return b2.modell(blob_oder_pfad)


def pose_r_t(mo, pose=PV):
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    return mo.pose(kf)


def b8_zu_modell(mo, P, pose=PV, RT=None):
    R, T = RT if RT is not None else pose_r_t(mo, pose)
    return (np.atleast_2d(np.asarray(P, float)) @ (R[8].astype(float) / 4096.0).T) + T[8].astype(float)


def proj(pose, P_modell):
    return b2.projiziere(pose, P_modell)


def kamera_modell(pose=PV):
    """Kameraort im Modellraum der Pose (ohne Knochen)."""
    r, kam, M, li = b2.raum()
    g = int(pose['gier'])
    s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    V = kam.V.astype(float) / 4096.0
    cw = -V.T @ kam.t.astype(float)
    return Y.T @ (cw - np.array([pose['x'], pose.get('y', 0), pose['z']], float))


def stationen():
    """22 Stationen entlang der Nutzerlinie T (Messblatt V8 3.1 / Riegel S1): Punkt p, Normale n, Ziel-Aussenkante (px ab p)."""
    d = lade('m27_breite.json')
    return [dict(s=r['s'], p=np.array(r['p'], float), n=np.array(r['n'], float), ziel=float(r['ziel_aussen']),
                 quelle=r['ziel_quelle']) for r in d['stationen']]


def aussenkante(maske, st, lo=-30, hi=71):
    """Erste belegte Stelle (px ab p) entlang der Normale (wie m11_kontur.lauf: Rundung auf ganze Pixel)."""
    out = []
    for s in st:
        e = None
        for o in range(lo, hi):
            x, y = s['p'] + o * s['n']
            xi, yi = int(round(x)), int(round(y))
            if 0 <= xi < maske.shape[1] and 0 <= yi < maske.shape[0] and maske[yi, xi]:
                e = o
                break
        out.append(e)
    return out


def sha(b):
    import hashlib
    return hashlib.sha256(b).hexdigest()
