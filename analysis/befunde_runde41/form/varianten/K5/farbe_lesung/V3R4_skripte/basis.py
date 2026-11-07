"""Nachtrag Rot (Messer, 2026-11-07): gemeinsame Grundlagen.
Regionen je Pixel aus Knochen + lokaler Flaechennormale (x vorn, y unten, z links (W), gleich fuer K5c und G4, Leser A 2),
Kette = Kandidat.rendern -> form_video.video_filter -> p_zonen.rueck (wie p_riegel.kette_riegel / riegel_farbe.kette_v2)."""
import sys, os, json, math
import numpy as np, cv2
WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
K5 = WT + '/analysis/befunde_runde41/form/varianten/K5'
FL = K5 + '/farbe_lesung'
S3 = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3'
HIER = os.path.dirname(os.path.abspath(__file__))
for p in (WT + '/re15_port/tools', FL + '/P_skripte', FL + '/B_skripte', K5 + '/generator'):
    if p not in sys.path:
        sys.path.insert(0, p)
import p_zonen as PZ          # noqa: E402  (P, FV, rueck, regionen, LICHT)
import form_buehne as FB      # noqa: E402
import form_pruefung as P     # noqa: E402

V3DIR = K5 + '/farbe/K5c_farbe_v3'
REGIONEN = ['Ruecken', 'Flanke', 'Brust/Bauch', 'Vorderbeine', 'Hinterbeine', 'Hals', 'Kopf', 'Schwanz']
RID = {r: i for i, r in enumerate(REGIONEN)}
N_GRENZE = 0.5   # Definition: Rumpfflaeche gilt als Ruecken, wenn ihre Normale weniger als 60 Grad von oben abweicht (ny < -0,5), als Bauch bei ny > 0,5


def lab(x):
    a = np.asarray(x, np.float32)
    sh = a.shape
    return cv2.cvtColor(a.reshape(1, -1, 3) / 255.0, cv2.COLOR_RGB2LAB)[0].reshape(sh)


def rgb_von_lab(L):
    a = np.asarray(L, np.float32)
    sh = a.shape
    return np.clip(cv2.cvtColor(a.reshape(1, -1, 3), cv2.COLOR_LAB2RGB)[0] * 255, 0, 255).reshape(sh)


def region_k5c(b, nx, ny):
    """K5c-Knochen (Messblatt V3 2.3, form.json teile): Rumpf 0/7/10, Schwanz 1-6, Hinterbeine 8/9/24-27, Vorderbeine 11-20/28-31, Hals 21, Kopf 22/23."""
    if b in (0, 7, 10):
        if ny < -N_GRENZE:
            return RID['Ruecken']
        if ny > N_GRENZE or (b == 10 and nx > N_GRENZE):
            return RID['Brust/Bauch']
        return RID['Flanke']
    if 1 <= b <= 6:
        return RID['Schwanz']
    if b in (8, 9, 24, 25, 26, 27):
        return RID['Hinterbeine']
    if 11 <= b <= 20 or 28 <= b <= 31:
        return RID['Vorderbeine']
    if b == 21:
        return RID['Hals']
    if b in (22, 23):
        return RID['Kopf']
    return -1


def region_g4(b, nx, ny):
    """G4-Knochen (Leser A 2, Rollen-Tabelle): Rumpf 0/9/10, Hinterbeine 1-8, Vorderbeine 11-19, Kopf 26; kleine Arme 20-25 ohne K5c-Gegenstueck."""
    if b in (0, 9, 10):
        if ny < -N_GRENZE:
            return RID['Ruecken']
        if ny > N_GRENZE or (b == 10 and nx > N_GRENZE):
            return RID['Brust/Bauch']
        return RID['Flanke']
    if 1 <= b <= 8:
        return RID['Hinterbeine']
    if 11 <= b <= 19:
        return RID['Vorderbeine']
    if b == 26:
        return RID['Kopf']
    return -1


def flaechen_normalen(mo):
    """je Knochen: lokale Normale je Flaeche (Reihenfolge tf dann qf wie Leser B fid), Mittel der Eckennormalen."""
    out = []
    for m in mo.meshes:
        nn = []
        for norms, faces in ((m.tn, m.tf), (m.qn, m.qf)):
            if not faces:
                continue
            N = np.array([x[:3] for x in norms], float) / 4096.0
            for g in faces:
                v = N[list(g[0::2])].mean(0)
                nn.append(v / max(np.linalg.norm(v), 1e-9))
        out.append(np.array(nn) if nn else np.zeros((0, 3)))
    return out


def regionen_szene(n, mo):
    """Regionenkarte (480x640, -1 = keine) fuer K5c-Geometrie in Bild n aus Leser B fid%d.npz (Knochen + Flaeche je Pixel)."""
    d = np.load(S3 + '/leserb/fid%d.npz' % n)
    kn, fa = d['kn'], d['fa']
    fn = flaechen_normalen(mo)
    reg = np.full(kn.shape, -1, np.int32)
    for b in np.unique(kn[kn >= 0]):
        sel = kn == b
        N = fn[b][fa[sel]]
        reg[sel] = [region_k5c(int(b), x, y) for x, y in zip(N[:, 0], N[:, 1])]
    return reg, kn, fa


def zeilen_karte(k, n):
    """CLUT-Zeile je Pixel der Variante in Bild n (gleiche Dreiecke und gleiche Textur-Alpha wie Kandidat.rendern; Zeile z -> Grauwert 10 + 20 z)."""
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    t = FB.Textur.__new__(FB.Textur)
    t.px, t.clut_y, t.h, t.w = k.tex.px, k.tex.clut_y, k.tex.h, k.tex.w
    t.lut = k.tex.lut.copy()
    for z in range(t.lut.shape[0]):
        t.lut[z, :, :3] = 10 + 20 * z
    img = np.zeros((480, 640, 3), np.uint8)
    m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None), t, img, kam.abbildung)
    z = np.where(m, np.rint((img[..., 0].astype(float) - 10) / 20).astype(int), -1)   # rastern schneidet ab (x 0,9999): runden
    return z


# ---------------------------------------------------------------- neutraler Render mit Regionen (wie render.blick: Licht 0,55 + 0,45 max(0, N.L), Umkugel 0,46)
def blick_regionen(mo, kf, az, el, regfn, gross=380):
    import render as R
    Rm, T = mo.pose(kf)
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    Ld = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    teile, pts = [], []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        Wm = Rm[b].astype(float) / 4096.0
        t = T[b].astype(float)
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = (np.array([x[:3] for x in verts], float) @ Wm.T) + t
            nl = np.array([x[:3] for x in norms], float) / 4096.0
            teile.append((b, v, nl, nl @ Wm.T, faces, uvs, viereck))
            pts.append(v)
    alle = np.vstack(pts)
    mitte = (alle.max(0) + alle.min(0)) / 2
    s = gross * 0.46 / float(np.linalg.norm(alle - mitte, axis=1).max())
    lit, alb, lbl = [], [], []
    for b, v, nl, nn, faces, uvs, viereck in teile:
        q = (v - mitte) @ A.T
        sx = gross / 2 + q[:, 0] * s
        sy = gross / 2 + q[:, 1] * s
        hell = np.clip(R.AMB + R.DIF * np.maximum(nn @ Ld, 0), 0, 1) * 255.0
        for g, uv in zip(faces, uvs):
            ie, inn = g[1::2], g[0::2]
            p = [(float(sx[i]), float(sy[i])) for i in ie]
            m0 = FB._mac0(p[0], p[1], p[2])
            if (m0 < 0) if viereck else (m0 <= 0):
                continue
            fnl = nl[list(inn)].mean(0)
            fnl = fnl / max(np.linalg.norm(fnl), 1e-9)
            rid = regfn(b, fnl[0], fnl[1])
            cl = np.array([10 + 20 * (rid + 1), 0, 0], float)            # Region + 1 (0 = keine), konstant je Flaeche, 20er-Stufen gegen Abschneiden
            po = (uv[5] & 0xF) * 128
            zeile = (uv[2] >> 6) & 0x1FF
            c = [np.full(3, hell[i]) for i in inn]
            w = np.full(3, 255.0)
            if viereck:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                for x, y, zz in ((0, 1, 3), (0, 3, 2)):
                    tf = float(q[ie[x], 2] + q[ie[y], 2] + q[ie[zz], 2]) / 3
                    pp, tt = (p[x], p[y], p[zz]), (tx[x], tx[y], tx[zz])
                    lit.append((tf, pp, tt, zeile, (c[x], c[y], c[zz]))); alb.append((tf, pp, tt, zeile, (w, w, w))); lbl.append((tf, pp, tt, zeile, (cl, cl, cl)))
            else:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                tf = float(q[ie[0], 2] + q[ie[1], 2] + q[ie[2], 2]) / 3
                lit.append((tf, tuple(p), tuple(tx), zeile, tuple(c))); alb.append((tf, tuple(p), tuple(tx), zeile, (w, w, w))); lbl.append((tf, tuple(p), tuple(tx), zeile, (cl, cl, cl)))
    ab = dict(ax=1.0, bx=0.0, ay=1.0, by=0.0)
    img = np.full((gross, gross, 3), 92, np.uint8); msk = FB.rastern(lit, mo.T, img, ab)
    ia = np.zeros((gross, gross, 3), np.uint8); FB.rastern(alb, mo.T, ia, ab)
    wt = type(mo.T).__new__(type(mo.T)); wt.__dict__.update(mo.T.__dict__); wt.lut = mo.T.lut.copy(); wt.lut[..., :3] = 255
    il = np.zeros((gross, gross, 3), np.uint8); FB.rastern(lbl, wt, il, ab)
    reg = np.where(msk, np.rint((il[..., 0].astype(float) - 10) / 20).astype(int) - 1, -1)
    return img, ia, msk, reg
