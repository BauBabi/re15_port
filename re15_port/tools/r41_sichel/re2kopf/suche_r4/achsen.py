from r4lib import *
import birkin_buehne as BB
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
blob = open(SP + 'EM30_V5r3.EMD', 'rb').read(); mo = b2.modell(blob)
p = KB.PARAM_R3; sk = np.array(p['skal']); R = KB.rot(p); t = np.array(p['versatz'])
for h in ('85', '135'):
    pose = P[h]; kf = BB.kf_von_clip(mo, pose['clip'], pose['bild']); Rb, Tt = mo.pose(kf)
    def scr(g):
        q = (np.atleast_2d(g) * sk) @ R.T + t
        W = (Rb[9].astype(float) / 4096.0 @ q.T).T + Tt[9].astype(float)
        return b2.projiziere(pose, W)
    c = np.array([0, -115, 0.0])
    o = scr(c)[0]
    for nm, d in (('+x Schnauze', (100, 0, 0)), ('-y oben', (0, -100, 0)), ('-z (W)-rechts', (0, 0, -100))):
        e = scr(c + np.array(d))[0]
        print(h, nm, 'Bildrichtung', (e - o).round(1))
    # bone-space axes for versatz
    for nm, d in (('bone x', (100, 0, 0)), ('bone y', (0, 100, 0)), ('bone z', (0, 0, 100))):
        W0 = (Rb[9].astype(float) / 4096.0 @ t) + Tt[9]; W1 = (Rb[9].astype(float) / 4096.0 @ (t + np.array(d))) + Tt[9]
        print(h, nm, (b2.projiziere(pose, W1)[0] - b2.projiziere(pose, W0)[0]).round(2))
