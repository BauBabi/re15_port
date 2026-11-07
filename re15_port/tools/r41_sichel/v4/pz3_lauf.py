"""Zahlen-Pruefer V4 r3: Spiellauf sfv4/pz3/lauf (eigener Lauf, bitgleich zu sfv4/r/V4r3) mit eigenem Code.
(1) koerperfest: Kette (Schwanz B) gegen Ziel (Schwanz Ziel, Keyframe der gezeichneten Pose) je Gelenk 1/3/5/7, H_E aus der Modellhoehe
    (Koerperecken ohne Schwanz, Pose 85) - wie O3; dazu
(2) FORM ohne Ziel-Zeile: Kabsch-Ausrichtung der Kette auf die Ruheform von Vorlage 2 (Keyframe clip 1 bild 26 = Pose 85, Gelenke 0-7 im
    Modellraum) mit gemeinsamer Wurzel: Restabweichung je Gelenk = Formerhalt (Knick, Biegung, Breitenprofil ist starr);
(3) Welt-Senkrechte: Abfall Biegung -> Ende (y) gegen die Ruheform (h100-Befund 'Kette waagerecht');
(4) Boden mit allen Ecken der Glied-Meshes 16-22: je Mesh die Ecken (Achs-y, Radius rho) entlang der Gliedachse, tiefster Weltpunkt
    = (Gelenk g + d * y * s).y + rho * sqrt(1 - d_y^2) (exakte Schranke ohne Rollwinkel), gegen den Boden 'B y'."""
import re, sys, os, json, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
from pz3_g1 import secs, md1, emr
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
LOG = sys.argv[1] if len(sys.argv) > 1 else SP + 'pz3/lauf/debug.log'
EMD = sys.argv[2] if len(sys.argv) > 2 else SP + 'pz3/emd/EM30_V4.EMD'
pt = lambda s: np.array([tuple(map(int, t)) for t in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', s)], float)
K, Z, BY, PO = {}, {}, {}, {}
for l in open(LOG, encoding='latin-1'):
    m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', l)
    if m: K[int(m.group(1))] = pt(m.group(3)); BY[int(m.group(1))] = int(m.group(2))
    m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz Ziel (.*)', l)
    if m: Z[int(m.group(1))] = pt(m.group(2))
    m = re.search(r'\[hangar-takt\] h(-?\d+) F(\d+) B clip (\d+) bild (\d+) gier (-?\d+)', l)
    if m: PO[int(m.group(1))] = dict(F=int(m.group(2)), clip=int(m.group(3)), bild=int(m.group(4)), gier=int(m.group(5)))
mo = b2.modell(EMD)
p85 = G.posen()['85']
V, _N, F = mo.geometrie(BB.kf_von_clip(mo, p85['clip'], p85['bild']))
HE = float(np.ptp(V[sorted({i for f in F if f['bone'] < 16 for i in f['v']})][:, 1]))
R, T = mo.pose(BB.kf_von_clip(mo, p85['clip'], p85['bild']))
J = [T[16 + j].astype(float) for j in range(7)]; J.append(T[22].astype(float) + R[22].astype(float)[:, 1] / 4096.0 * 258.0)
RU = np.array(J)                                         # Ruheform Vorlage 2 (Modellraum, y nach unten)
def winkel(P): return [float(np.degrees(np.arccos(np.clip((P[i] - P[i - 1]) @ (P[i + 1] - P[i]) / np.linalg.norm(P[i] - P[i - 1]) / np.linalg.norm(P[i + 1] - P[i]), -1, 1)))) for i in range(1, 7)]
def kabsch(A, B):                                        # dreht A (um Wurzel) auf B (um Wurzel)
    a, b = A - A[0], B - B[0]; U, S_, Vt = np.linalg.svd(a.T @ b); d = np.sign(np.linalg.det(Vt.T @ U.T))
    Rm = Vt.T @ np.diag([1, 1, d]) @ U.T; return (Rm @ a.T).T + B[0]
B = secs(open(EMD, 'rb').read()); M = md1(B[7]); rel = np.frombuffer(emr(B[2])['rel'], '<i2').reshape(-1, 3)
LR = [float(np.linalg.norm(rel[17 + g])) for g in range(6)] + [None]
MV = [np.vstack([M[16 + g]['tv'][:, :3], M[16 + g]['qv'][:, :3]]).astype(float) for g in range(7)]
out = {}
for h in sorted(K):
    P = K[h]; r = dict(clip=PO.get(h, {}).get('clip'), F=PO.get(h, {}).get('F'))
    if h in Z:
        r['abw_ziel'] = {k: round(float(np.linalg.norm(P[j] - Z[h][j]) / HE), 3) for k, j in (('Beginn', 1), ('Biegung', 3), ('Knick', 5), ('Ende', 7))}
    al = kabsch(P, RU); r['form_rest'] = round(float(np.linalg.norm(al - RU, axis=1).max() / HE), 3)
    w = winkel(P); r['w3'], r['w5'], r['wmax'] = round(w[2], 1), round(w[4], 1), round(max(w), 1)
    r['abfall_37_welt'] = round(float(P[7][1] - P[3][1]) / HE, 3)        # y nach unten: + = Ende tiefer als Biegung
    tief = -1e9
    for g in range(7):
        d = P[g + 1] - P[g]; L = np.linalg.norm(d); d = d / L; s = L / LR[g] if LR[g] else 1.0
        y = MV[g][:, 1]; rho = np.hypot(MV[g][:, 0], MV[g][:, 2])
        wy = P[g][1] + d[1] * y * s + rho * np.sqrt(max(0.0, 1 - d[1] ** 2))
        tief = max(tief, float(wy.max()))
    r['tiefster_meshpunkt'] = round(tief - BY[h])
    out[h] = r
wr = winkel(RU)
print('H_E %.0f; Ruheform Pose 85: Biegung %.1f, Knick %.1f Grad, Abfall Biegung->Ende %.3f H' % (HE, wr[2], wr[4], (RU[7][1] - RU[3][1]) / HE))
print('Boden B y je Takt:', sorted(set(BY.values())))
gang = [h for h in out if h >= 85 and (out[h]['clip'] == 1 or h == 85)]
print('Gangtakte (h85 + clip 1):', len(gang), gang[0], '..', gang[-1])
mz = {h: max(out[h]['abw_ziel'].values()) for h in gang if 'abw_ziel' in out[h]}
if mz: print('(1) gegen Ziel: max %.3f H (h%d), Takte > 0,06: %d' % (max(mz.values()), max(mz, key=mz.get), sum(v > 0.06 for v in mz.values())))
fr = {h: out[h]['form_rest'] for h in gang}
print('(2) Form gegen Ruheform Vorlage 2 (Kabsch): max %.3f H (h%d), Median %.3f, Takte > 0,06: %d' % (max(fr.values()), max(fr, key=fr.get), np.median(list(fr.values())), sum(v > 0.06 for v in fr.values())))
ab = {h: out[h]['abfall_37_welt'] for h in gang}
print('(3) Welt-Abfall Biegung->Ende: min %.3f H (h%d), max %.3f; Ruhe %.3f' % (min(ab.values()), min(ab, key=ab.get), max(ab.values()), (RU[7][1] - RU[3][1]) / HE))
print('    Biegung w3 %.1f-%.1f, Knick w5 %.1f-%.1f (Ruhe %.1f / %.1f), wmax ganzer Lauf %.1f' % (min(out[h]['w3'] for h in gang), max(out[h]['w3'] for h in gang), min(out[h]['w5'] for h in gang), max(out[h]['w5'] for h in gang), wr[2], wr[4], max(o['wmax'] for o in out.values())))
tb = {h: out[h]['tiefster_meshpunkt'] for h in out}
hb = max(tb, key=tb.get)
print('(4) Boden alle Mesh-Ecken: tiefster Punkt %+d E. (h%d), Takte > 0: %d von %d; die 5 tiefsten: %s' % (tb[hb], hb, sum(v > 0 for v in tb.values()), len(tb), sorted(tb.items(), key=lambda x: -x[1])[:5]))
for h in (85, 88, 90, 95, 100, 102, 105, 110, 118, 125, 135, max(gang)):
    if h in out: print('  h%d %s' % (h, out[h]))
json.dump(dict(HE=HE, takte=out), open(sys.argv[3] if len(sys.argv) > 3 else SP + 'pz3/lauf_mess.json', 'w'), indent=1)
