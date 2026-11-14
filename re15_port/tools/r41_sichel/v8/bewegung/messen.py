"""Schwanz V8 in der Hangar-Szene: Messung je Videobild h aus einem Lauf (pose.log = gezeichnete Knochen je Spielbild F, F = h + 243).

    python messen.py <laufordner> <ausgabe.json> [emd] [h0 h1 schritt]

Je h (Videoraum 640 x 480, Abbildung wie b2/birkin_buehne: GTE + Homographie 50A0/12):
  - Kette: Gelenke 16..22, Gliedende (541) und Nadelspitze (groesste Ecke von Mesh 22 entlang der Achse) projiziert;
  - Sichtbarkeit: Knochen-Id-Maske (b2.zeichne mit der GEZEICHNETEN Pose aus pose.log, Malerverfahren wie der Port, ohne Spider-Man);
  - Austritt = erster sichtbarer Punkt der Mittellinie ab der Wurzel, Spitze = Nadelspitze (projiziert);
  - Breite quer zur Mittellinie an s .1/.3/.5/.7/.9 des sichtbaren Stuecks (Austritt -> Spitze) in der Id-Maske 16-22;
  - Rolle phi je Glied = Winkel zwischen Bauchachse (+e_h, gemessen: Hauptachse x-z der Meshes, Seite der Bauch-CLUT) und dem Sehstrahl
    quer zur Gliedachse (0 = Bauch zur Kamera, 90 = Flanke, 180 = Ruecken);
  - Gelenkwinkel (Raum) je Gelenk.
Video: analyse/messtabelle.json (Analyse 2026-11-14, Linien video_linien.py / Nutzer-Umrisse Areas*.png).
"""
import sys, os, re, json, math
import numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/re15_port/tools/r41_sichel/v2')
import b2                      # noqa: E402
import birkin_buehne as BB     # noqa: E402
import emd_ansichtsblatt as EA # noqa: E402

AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
EMD = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD'
F_VON_H = 243
L = 541.0


def pose_log(pfad):
    P = {}
    for z in open(pfad, encoding='latin-1'):
        m = re.match(r'\[pose\] F(\d+) typ30 mo=(-?\d+) af=(-?\d+) frac=(-?\d+) lage (-?\d+) (-?\d+) (-?\d+) rot (-?\d+) (-?\d+) (-?\d+) s (-?\d+) n (\d+)(.*)', z)
        if not m:
            continue
        F = int(m.group(1))
        teile = [list(map(int, t.split())) for t in m.group(13).split('|')[1:]]
        R = np.array([t[:9] for t in teile], dtype=np.int64).reshape(-1, 3, 3)
        T = np.array([t[9:12] for t in teile], dtype=np.int64)
        P[F] = dict(clip=int(m.group(2)), bild=int(m.group(3)), x=int(m.group(5)), y=int(m.group(6)), z=int(m.group(7)),
                    gier=int(m.group(9)), R=R, T=T)
    return P


def bauchachse(mo):
    """+e_h je Knochenraum: Hauptachse der Ecken (x-z, ohne Achs-Ecken) der Meshes 17-21, Vorzeichen zur Mitte der Flaechen, deren
    CLUT-Zeile nicht die haeufigste ist (Bauch/Baender gegen Ruecken)."""
    xz, fl = [], {}
    for b in range(17, 22):
        m = mo.meshes[b]
        for verts, faces, uvs in ((m.tv, m.tf, m.tuv), (m.qv, m.qf, m.quv)):
            Pv = np.array([v[:3] for v in verts], float)
            for v in Pv:
                if math.hypot(v[0], v[2]) > 20:
                    xz.append((v[0], v[2]))
            for f, uv in zip(faces, uvs):
                z = (uv[2] >> 6) & 0x1FF
                c = Pv[list(f[1::2])].mean(0)
                fl.setdefault(z, []).append((c[0], c[2]))
    xz = np.array(xz)
    w, e = np.linalg.eigh(np.cov(xz.T))
    ax = e[:, 1]
    haupt = max(fl, key=lambda k: len(fl[k]))
    rest = np.array([c for k, v in fl.items() if k != haupt for c in v])
    if rest.mean(0) @ ax < 0:
        ax = -ax
    return np.array([ax[0], 0.0, ax[1]])


def dorn(mo):
    m = mo.meshes[22]
    return max(v[1] for v in list(m.tv) + list(m.qv))


def Ymat(g):
    s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def kette(p, dn, n_je=24):
    """Mittellinie im Modellraum: Proben je Glied (Gelenk j .. j+1), Glied 22 bis zur Nadelspitze; Rueckgabe Punkte, Glied-Index."""
    R, T = p['R'].astype(float) / 4096.0, p['T'].astype(float)
    pts, gl = [], []
    for b in range(16, 23):
        a = R[b][:, 1] / np.linalg.norm(R[b][:, 1])
        ende = dn if b == 22 else L
        for y in np.linspace(0, ende, n_je if b < 22 else 3 * n_je, endpoint=(b == 22)):
            pts.append(T[b] + a * y); gl.append(b)
    return np.array(pts), np.array(gl)


def breite(ids, P, t, maxw=60):
    n = np.array([-t[1], t[0]]); n /= max(np.linalg.norm(n), 1e-9)
    tot = 0.0
    for sg in (1, -1):
        d = 0.0
        while d < maxw:
            q = P + sg * (d + 0.5) * n
            xi, yi = int(round(q[0])), int(round(q[1]))
            if not (0 <= xi < 640 and 0 <= yi < 480) or not (16 <= ids[yi, xi] <= 22):
                break
            d += 0.5
        tot += d
    return tot


def messe(p, mo, eh, dn, kam):
    pose = dict(clip=p['clip'], bild=p['bild'], gier=p['gier'], x=p['x'], y=p['y'], z=p['z'])
    _bild, ids = b2.zeichne(mo, pose, R_T=(p['R'], p['T']), licht=False)
    Pm, gl = kette(p, dn)
    Q = b2.projiziere(pose, Pm)
    sicht = np.zeros(len(Q), bool)
    Rn = np.linalg.norm(p['R'].astype(float), axis=1)          # Spaltenlaengen je Knochen; verdeckt = x/z-Spalte 0
    verd = {bb for bb in range(16, 23) if Rn[bb][0] < 1 or Rn[bb][2] < 1}
    for i, (x, y) in enumerate(Q):
        if gl[i] in verd:
            continue
        xi, yi = int(round(x)), int(round(y))
        if 0 <= xi < 640 and 0 <= yi < 480:
            w = ids[max(yi - 1, 0):yi + 2, max(xi - 1, 0):xi + 2]
            sicht[i] = ((w >= 16) & (w <= 22)).any()
    aus = dict(F=None, sicht_anteil=round(float(sicht.mean()), 2), spitze=[round(float(v), 1) for v in Q[-1]],
               wurzel=[round(float(v), 1) for v in Q[0]], tip_sichtbar=bool(sicht[-8:].any()))
    idx = np.nonzero(sicht)[0]
    if len(idx):
        i0 = int(idx[0])
        aus['austritt'] = [round(float(v), 1) for v in Q[i0]]
        aus['austritt_glied'] = int(gl[i0])
        seg = Q[i0:]
        d = np.r_[0, np.cumsum(np.hypot(*np.diff(seg, axis=0).T))]
        aus['laenge_sichtbar'] = round(float(d[-1]), 1)
        br = []
        for s in (0.1, 0.3, 0.5, 0.7, 0.9):
            k = int(np.searchsorted(d, s * d[-1]))
            k = min(max(k, 1), len(seg) - 2)
            t = seg[k + 1] - seg[k - 1]
            br.append(round(breite(ids, seg[k], t), 1))
        aus['breiten'] = br
        tg = np.gradient(seg, axis=0)
        a = np.unwrap(np.arctan2(tg[:, 1], tg[:, 0]))
        aus['drehung_bild'] = round(math.degrees(a[-1] - a[0]), 1)
    # Rolle je Glied, Gelenkwinkel
    R, T = p['R'].astype(float) / 4096.0, p['T'].astype(float)
    Y = Ymat(p['gier']); lage = np.array([p['x'], p['y'], p['z']], float)
    cam = np.array(kam.pos, float)
    phi, ax_l = [], []
    for b in range(16, 23):
        nn = np.linalg.norm(R[b], axis=0)
        if nn[0] < 1e-3 or nn[2] < 1e-3:                 # verdeckt (Querschnitt 0): keine Rolle messbar
            Rb = R[b] / np.maximum(nn, 1e-9); ax_l.append(Y @ Rb[:, 1]); phi.append(None); continue
        Rb = R[b] / nn
        ax = Y @ Rb[:, 1]; bauch = Y @ (Rb @ eh)
        mid = Y @ (T[b] + Rb[:, 1] * L * 0.5) + lage
        v = cam - mid; v = v - (v @ ax) * ax; v /= np.linalg.norm(v)
        bauch = bauch - (bauch @ ax) * ax; bauch /= np.linalg.norm(bauch)
        phi.append(round(math.degrees(math.acos(max(-1.0, min(1.0, float(bauch @ v))))), 1))
        ax_l.append(ax)
    aus['phi'] = phi
    aus['gelenk'] = [round(math.degrees(math.acos(max(-1.0, min(1.0, float(ax_l[j] @ ax_l[j + 1]))))), 1) for j in range(6)]
    # Twist zwischen Nachbargliedern (Bauchachse des Vorglieds parallel auf die Achse des Glieds verschoben)
    tw = []
    for j in range(1, 7):
        na, nb = np.linalg.norm(R[15 + j], axis=0), np.linalg.norm(R[16 + j], axis=0)
        if min(na[0], na[2], nb[0], nb[2]) < 1e-3:
            tw.append(None); continue
        Ra = R[15 + j] / na; Rb_ = R[16 + j] / nb
        a1, a2 = Ra[:, 1], Rb_[:, 1]
        e1, e2 = Ra @ eh, Rb_ @ eh
        v = np.cross(a1, a2); c = float(a1 @ a2)
        if np.linalg.norm(v) > 1e-9:
            K = np.array([[0, -v[2], v[1]], [v[2], 0, -v[0]], [-v[1], v[0], 0]])
            Rm = np.eye(3) + K + K @ K / (1 + c)
        else:
            Rm = np.eye(3)
        e1t = Rm @ e1
        e1t -= (e1t @ a2) * a2; e2p = e2 - (e2 @ a2) * a2
        e1t /= np.linalg.norm(e1t); e2p /= np.linalg.norm(e2p)
        tw.append(round(math.degrees(math.atan2(float(np.cross(e1t, e2p) @ a2), float(e1t @ e2p))), 1))
    aus['twist'] = tw
    return aus


def main():
    lauf, aus_p = sys.argv[1], sys.argv[2]
    emd = sys.argv[3] if len(sys.argv) > 3 and sys.argv[3] != '-' else EMD
    h0, h1, st = (int(v) for v in sys.argv[4:7]) if len(sys.argv) > 6 else (16, 104, 2)
    mo = b2.modell(emd)
    eh, dn = bauchachse(mo), dorn(mo)
    kam = b2.raum()[1]
    P = pose_log(os.path.join(lauf, 'pose.log'))
    V = json.load(open(os.path.join(AN, 'messtabelle.json')))
    erg = dict(bauchachse=[round(float(v), 4) for v in eh], dorn=dn, je_h={})
    for h in range(h0, h1 + 1, st):
        F = h + F_VON_H
        if F not in P:
            continue
        r = messe(P[F], mo, eh, dn, kam)
        r['F'] = F
        v = V.get(str(h), {})
        r['video'] = {k: v.get(k) for k in ('v_sicht', 'v_wurzel', 'v_spitze', 'v_breiten', 'v_laenge', 'v_drehung')}
        if v.get('v_spitze') and v.get('v_sicht') not in (None, 'nein'):
            r['spitze_fehler'] = round(math.hypot(r['spitze'][0] - v['v_spitze'][0], r['spitze'][1] - v['v_spitze'][1]), 1)
        if v.get('v_wurzel') and r.get('austritt') and v.get('v_sicht') not in (None, 'nein'):
            r['austritt_fehler'] = round(math.hypot(r['austritt'][0] - v['v_wurzel'][0], r['austritt'][1] - v['v_wurzel'][1]), 1)
        erg['je_h'][str(h)] = r
        print(h, 'F', F, 'sicht', r['sicht_anteil'], 'spitze', r['spitze'], 'V', r['video']['v_spitze'], 'fehl', r.get('spitze_fehler'),
              'aus', r.get('austritt'), 'V', r['video']['v_wurzel'], 'br', r.get('breiten'), 'V', r['video']['v_breiten'],
              'phi', r['phi'], 'gel', r['gelenk'], 'tw', r['twist'], flush=True)
    json.dump(erg, open(aus_p, 'w'), indent=1)


if __name__ == '__main__':
    main()
