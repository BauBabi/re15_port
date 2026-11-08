"""SF V8 Riegel S1-S8 (Messblatt V8 Abschnitt 7, analysis/befunde_runde41/SF_MESSBLATT_V8.md, Commit 09b04bc1c).

    python riegel_v8.py [EMD] [--v7 EMD_V7] [--v5f EMD_V5F] [--szene debug.log] [--aus TXT] [--json JSON]
        Exit 0 = alle Riegel gruen, 1 = mindestens einer rot. Standard-EMD = Spiel-Asset EM30_HANGAR.EMD dieses Baums.

Pruefung in PV (Clip 17 Bild 47 Gier 587 x -8246 z -21266), Spielkamera 50A0 Cut 12, RDT-Licht (f_rast.mit_bg, Hintergrund BSS).
Kein RE-Befund; Schwellen = Messblatt V8 Abschnitt 7 (Nutzer-Skizze Darstellung.png, Vorlage n1439). Vorlage-Daten (Stationen T,
Figur-Maske, Oberkontur, Nutzerlinien) aus SF_belege/v8/messung/daten (nur gelesen).

S1 Kontur T     Aussenkante Modell entlang der 22 T-Normalen gegen die Vorlage-Kante (T + 11 px, wo nicht messbar): Mittel <= 4, Max <= 8
S2 Uebergang J  (a) Oberkontur x 330-385 gegen die Vorlage-Maske Mittel <= 4, Max <= 6; (b) Sattel der Modell-Oberkontur unter ihrer
                oberen Huelle x 300-420 <= 2; (c) Rumpf ueber der Schwanz-Oberkante an der Wurzel <= 3
S3 Ruecken/H/K  (a) Kuppenprominenz <= 4; (b) Huegel H 5-10 px ueber der Sehne x 405 -> 447 bei x 428-445; (c) keine weitere Erhebung
                >= 3 px in x 330-460; (d) Sattel Ruecken -> Kopf (x 380-447) <= 2; (e) Kopf-Vorderkante y 212-244 <= 3 px von der Geraden
S4 Breite       Ringdurchmesser in PV quer zur Achse: Wurzel >= 55, s 90-130 >= 36, monoton fallend (+-2), Ende <= 10
S5 Gesicht      Mesh 9 (Ecken, Normalen, Flaechen, UV, CLUT-Feld), Zeile 486, alle Mesh-9-Texel bytegleich V7
S6 Boden        Spitze + Dornspitze Mesh 22 in 0 von allen Keyframes unter der tiefsten Koerperecke; Szene (--szene) alle Gelenke y <= 0
S7 Adern        (a) Herkunft der Eintraege und Texel; (b) Oberarm 13 35-59 % a* >= 10; (c) Rumpf 8 lok x < 0 25-80 %; (d) Ruhe
S8 Technik      CLUT-Zeilen <= 16, 256 x Zeilen <= 4096, CLUT-Felder gueltig, 23 Meshes, 23 Knochen, 21 Clips
"""
import os, sys, json, argparse
import numpy as np
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402
import emd_ansichtsblatt as EA        # noqa: E402
import f_rast as FR                   # noqa: E402
import v4_gemein as G                 # noqa: E402

S7_ADER_A, S7_ADER_L = 15.0, 38.0     # Messblatt 6c: Ader-Eintrag a* >= 15, L* <= 38


# ------------------------------------------------------------------ Render
def render(blob, pose=V.PV, licht=True):
    mo = V.modell(blob)
    A = FR.mit_bg(mo, pose, licht=licht)
    return mo, A


def oben(bone, x0, x1, ausser=()):
    o = {}
    for x in range(x0, x1 + 1):
        ys = np.nonzero((bone[:, x] >= 0) & ~np.isin(bone[:, x], list(ausser)))[0]
        if len(ys):
            o[x] = int(ys.min())
    return o


def huckel(prof, prom=3, w=25):
    ks = sorted(prof); h = -np.array([prof[k] for k in ks], float)
    H = []
    for i in range(len(ks)):
        l = h[max(0, i - w):i]; r = h[i + 1:i + 1 + w]
        if len(l) == 0 or len(r) == 0:
            continue
        if h[i] >= l.max() and h[i] > r.max():
            p = min(h[i] - l.min(), h[i] - r.min())
            if p >= prom:
                H.append(dict(x=int(ks[i]), y=int(-h[i]), prom=round(float(p), 1)))
    return H


def sattel(prof, a, b):
    xs = np.array([x for x in range(a, b + 1) if x in prof]); ys = np.array([prof[x] for x in xs], float)
    pts = sorted(zip(xs, -ys)); Hh = []
    for p in pts:
        while len(Hh) >= 2 and (Hh[-1][0] - Hh[-2][0]) * (p[1] - Hh[-2][1]) - (Hh[-1][1] - Hh[-2][1]) * (p[0] - Hh[-2][0]) >= 0:
            Hh.pop()
        Hh.append(p)
    hull = np.interp(xs, [h[0] for h in Hh], [-h[1] for h in Hh]); dep = ys - hull
    i = int(np.argmax(dep))
    return round(float(dep.max()), 1), int(xs[i])


def huegel(prof, a, b):
    xs = np.arange(a, b + 1); ys = np.array([prof[x] for x in xs], float); ch = np.interp(xs, [a, b], [ys[0], ys[-1]])
    h = ch - ys; i = int(np.argmax(h))
    return round(float(h.max()), 1), int(xs[i])


# ------------------------------------------------------------------ S1-S4
def s1(bone):
    st = V.stationen()
    e = V.aussenkante(bone >= 0, st)
    d = [(x - s['ziel']) if x is not None else 99.0 for x, s in zip(e, st)]
    m, mx = float(np.mean(np.abs(d))), float(np.max(np.abs(d)))
    return dict(mittel=round(m, 1), max=round(mx, 1), d=[round(x, 1) for x in d], ok=m <= 4.0 and mx <= 8.0)


def s2_s3(mo, bone):
    ov = {int(k): v for k, v in V.lade('m13b_oben.json')['oben_vorlage_maske'].items()}
    om = oben(bone, 280, 470)
    da = [om[x] - ov[x] for x in range(330, 386) if x in om and x in ov]
    a = dict(mittel=round(float(np.mean(np.abs(da))), 1), max=int(np.max(np.abs(da))))
    b = sattel(om, 300, 420)
    c = j_huckel(mo)
    r2 = dict(a=a, b=b, c=c, ok=a['mittel'] <= 4 and a['max'] <= 6 and b[0] <= 2 and c['huckel_px'] <= 3)
    hk = huckel({x: om[x] for x in range(330, 461) if x in om}, prom=3, w=20)
    kuppe = [h for h in hk if 385 <= h['x'] <= 425]
    k_prom = max([h['prom'] for h in kuppe], default=0.0)
    H = huegel(om, 405, 447)
    weitere = [h for h in hk if not (428 <= h['x'] <= 445) and not (385 <= h['x'] <= 425)]   # Kuppe -> (a), H -> (b)
    sd = sattel(om, 380, 447)
    fm = {}
    for y in range(212, 244):
        xs = np.nonzero(bone[y, 420:500] >= 0)[0]
        if len(xs):
            fm[y] = 420 + int(xs.max())
    ys = np.array(sorted(fm)); xs = np.array([fm[y] for y in ys], float)
    cf = np.polyfit(ys, xs, 1); k_abw = round(float(np.abs(xs - np.polyval(cf, ys)).max()), 1)
    r3 = dict(a_kuppe=k_prom, b_huegel=H, c_weitere=weitere, d_sattel_kopf=sd, e_K=k_abw,
              ok=k_prom <= 4 and 5 <= H[0] <= 10 and 428 <= H[1] <= 445 and not weitere and sd[0] <= 2 and k_abw <= 3)
    return r2, r3, om


def j_huckel(mo):
    """S2c (Messblatt 7 "Rumpf ueber der Schwanz-Oberkante an der Wurzel", Werkzeug m26: Rumpf (Knochen 0-15) und Schwanz (16-22) getrennt
    gerastert, PV). Praezisiert (V8-Bau): Naht = rechteste Spalte x 330-420, in der die Schwanz-Oberkante nicht unter der Rumpf-Oberkante
    liegt; Hoehe = groesster Abstand Rumpf-Oberkante ueber der Schwanz-Oberkante in den Spalten Naht +- 15, in denen beide da sind.
    (m26 setzte die Schwanz-Oberkante hinter ihrem rechten Ende als Gerade fort; das misst den im Rumpf liegenden Rohrrand mit - V7 34 px
    bei x 410, Messblatt-Tabelle nennt fuer V7 "6-14" = x 360-405. Diese Fassung misst V7 an der sichtbaren Naht.)"""
    mr = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(16))); ms = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(16, 23)))
    top = lambda m, x: int(np.nonzero(m[:, x])[0].min()) if m[:, x].any() else None
    tr = {x: top(mr, x) for x in range(300, 460)}; ts = {x: top(ms, x) for x in range(300, 460)}
    oben = [x for x in range(330, 421) if ts[x] is not None and tr[x] is not None and ts[x] <= tr[x]]
    if not oben:
        return dict(naht_x=None, huckel_px=99.0, bei_x=None)
    xn = max(oben)
    H = [(ts[x] - tr[x], x) for x in range(xn - 15, xn + 16) if ts.get(x) is not None and tr.get(x) is not None]
    h = max(H)
    return dict(naht_x=xn, huckel_px=round(float(h[0]), 1), bei_x=int(h[1]))


def ringe_bild(mo, pose=V.PV):
    """Je Schwanzring (Mesh 16-22, 11 Ecken): Bildmitte, Durchmesser quer zur Gliedachse im Bild (px)."""
    R, T = V.pose_r_t(mo, pose)
    out = []
    for g in range(7):
        b = 16 + g
        m = mo.meshes[b]
        P = np.array([v[:3] for v in m.tv], float)
        Rb = R[b].astype(float) / 4096.0; Tb = T[b].astype(float)
        a0 = V.proj(pose, (np.array([[0, 0, 0], [0, 400, 0]], float) @ Rb.T) + Tb)
        ax = a0[1] - a0[0]; ax /= max(np.linalg.norm(ax), 1e-9); q = np.array([-ax[1], ax[0]])
        nr = len(P) // 11
        for r in range(nr):
            Q = V.proj(pose, (P[r * 11:(r + 1) * 11] @ Rb.T) + Tb)
            w = (Q - Q.mean(0)) @ q
            out.append(dict(glied=b, ring=r, mitte=[round(float(x), 1) for x in Q.mean(0)], d=round(float(w.max() - w.min()), 1)))
    return out


def s4(mo):
    rg = ringe_bild(mo)
    st = V.stationen()
    P = np.array([s['p'] for s in st]); sv = np.array([s['s'] for s in st])
    for r in rg:
        r['s'] = int(sv[np.argmin(np.linalg.norm(P - np.array(r['mitte']), axis=1))])
    # Ringe in Kettenfolge (Endring von k und Anfangsring von k+1 liegen am selben Gelenk: das Gelenk nimmt das Maximum)
    # Wurzel = groesster Ring von Glied 16 (V8: Ring 0 ist das verjuengte Rohrende im Rumpf, v8_bau.wurzel_mesh); die Folge beginnt am
    # Wurzelring (die Ringe davor liegen im Rumpf)
    g16 = [r for r in rg if r['glied'] == 16]
    iw = max(range(len(g16)), key=lambda i: g16[i]['d'])
    folge = [r['d'] for r in rg[iw:]]
    wurzel = g16[iw]['d']
    mitte = [r['d'] for r in rg if 90 <= r['s'] <= 130 and r['glied'] <= 19]
    ende = rg[-1]['d']
    mono = all(b <= a + 2.0 for a, b in zip(folge, folge[1:]))
    return dict(wurzel=wurzel, s90_130=mitte, ende=ende, folge=folge, monoton=mono,
                ok=wurzel >= 55 and len(mitte) > 0 and min(mitte) >= 36 and mono and ende <= 10, ringe=rg)


# ------------------------------------------------------------------ S5
def s5(blob, ref_blob):
    _d, _o, ms, tim = KB.teile(blob); _d2, _o2, mr, timr = KB.teile(ref_blob)
    t, tr = KB.tim_teile(tim), KB.tim_teile(timr)
    mesh = ms[9] == mr[9]
    z486 = bool(t['clut'].shape[0] > 6 and tr['clut'].shape[0] > 6 and np.array_equal(t['clut'][6], tr['clut'][6]))
    nutz = KB.texel_nutzung([mr[9]])[0]
    texel = bool(t['pix'].shape == tr['pix'].shape and np.array_equal(t['pix'][nutz], tr['pix'][nutz]))
    return dict(mesh9=bool(mesh), zeile486=z486, texel=texel, n_texel=int(nutz.sum()), ok=bool(mesh and z486 and texel))


# ------------------------------------------------------------------ S6
def s6(blob, szene=None):
    mo = V.modell(blob)
    s = mo.skel
    m22 = mo.meshes[22]
    P22 = np.array([v[:3] for v in m22.tv], float)
    dorn = P22[np.argmax(P22[:, 1])]
    L = float(np.linalg.norm(np.array(s.rel[22], float)))
    unter = []
    for k in range(s.kf_count):
        R, T = mo.pose(k)
        y = -1e9
        for b in range(16):
            m = mo.meshes[b]
            if m.tv:
                Pb = np.array([v[:3] for v in m.tv], float)
                y = max(y, float(((Pb @ (R[b].astype(float) / 4096.0).T) + T[b].astype(float))[:, 1].max()))
        R22 = R[22].astype(float) / 4096.0
        sp = T[22].astype(float) + R22[:, 1] * L
        dn = T[22].astype(float) + R22 @ dorn
        if max(sp[1], dn[1]) > y:
            unter.append(k)
    sz = None
    if szene and os.path.exists(szene):
        tief, n = -1e9, 0
        for z in open(szene, errors='replace'):
            if 'Schwanz B y' in z:
                n += 1
                for g in z.split()[6:]:
                    try:
                        tief = max(tief, int(g.split(',')[1]))
                    except (IndexError, ValueError):
                        pass
        sz = dict(takte=n, tiefstes_y=tief, unter_boden=int(tief > 0))
    ok = not unter and (sz is None or sz['tiefstes_y'] <= 0)
    return dict(keyframes=s.kf_count, unter=len(unter), beispiele=unter[:10], szene=sz, ok=ok)


# ------------------------------------------------------------------ S7
def s7(blob, A, ref_blob):
    import adern_v8 as AV
    mo = V.modell(blob)
    a = AV.herkunft(blob, ref_blob)
    img, bone, lok = A['img'][:480], A['bone'][:480], A['lok'][:480]
    LM = G.lab(G.videoraum(img))
    m13 = bone == 13
    m8 = (bone == 8) & (lok[..., 0] < 0)
    m8z = (bone == 8) & (lok[..., 2] < 0)
    b = round(float((LM[m13][:, 1] >= 10).mean() * 100), 1)
    c = round(float((LM[m8][:, 1] >= 10).mean() * 100), 1)
    cz = round(float((LM[m8z][:, 1] >= 10).mean() * 100), 1)
    d = ruhe(blob)
    return dict(a=a, b_oberarm13=b, c_rumpf8_lokx=c, c_rumpf8_lokz_info=cz, d=d,
                ok=a['ok'] and 35 <= b <= 59 and 25 <= c <= 80 and d['ok'])


def rote_flecken(blob_oder_mo, knochen=(8, 13), seite_z=True):
    mo = blob_oder_mo if hasattr(blob_oder_mo, 'skel') else V.modell(blob_oder_mo)
    A = FR.zeichne(mo, V.PV, None, licht=False)
    img, bone, lok = A['img'][:480], A['bone'][:480], A['lok'][:480]
    Lab = G.lab(img.astype(float))
    sel = np.isin(bone, knochen)
    if seite_z:
        sel &= ~((bone == 8) & (lok[..., 2] >= 0))
    rot = sel & (Lab[..., 1] >= 15)
    lab_, n = ndimage.label(rot, structure=np.ones((3, 3)))
    gr = ndimage.sum(rot, lab_, range(1, n + 1)) if n else []
    return int(sum(1 for g in gr if g >= 4)), int(rot.sum()), int(sel.sum())


def ruhe(blob):
    o = BF.lade_em30()
    n_o, px_o, sel_o = rote_flecken(o)
    n_v, px_v, sel_v = rote_flecken(blob)
    return dict(flecken_original=n_o, flecken_v8=n_v, rotpx_original=px_o, rotpx_v8=px_v, ok=n_v <= n_o)


# ------------------------------------------------------------------ S8
def s8(blob):
    mo = V.modell(blob)
    _d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    zeilen = int(t['clut'].shape[0])
    felder = set()
    for m in ms:
        for uv in m['tuv'] + m['quv']:
            felder.add((uv[2] >> 6) & 0x1FF)
    gueltig = all(480 <= z < 480 + zeilen for z in felder)
    r = dict(zeilen=zeilen, hoehe=int(t['ih'] * zeilen), felder=sorted(felder), gueltig=gueltig, meshes=len(ms),
             knochen=mo.skel.bone_count, clips=len(mo.clips))
    r['ok'] = zeilen <= 16 and r['hoehe'] <= 4096 and gueltig and r['meshes'] == 23 and r['knochen'] == 23 and r['clips'] == 21
    return r


def alles(blob, ref_blob, szene=None, mit_s7=True):
    mo, A = render(blob)
    bone = A['bone'][:480]
    out = dict(sha256=V.sha(blob))
    out['S1'] = s1(bone)
    out['S2'], out['S3'], _om = s2_s3(mo, bone)
    out['S4'] = s4(mo)
    out['S5'] = s5(blob, ref_blob)
    out['S6'] = s6(blob, szene)
    if mit_s7:
        out['S7'] = s7(blob, A, ref_blob)
    out['S8'] = s8(blob)
    return out


def zeilen(r):
    z = []
    s = r['S1']; z.append('S1 Kontur T: Mittel %.1f Max %.1f px (Soll <= 4 / <= 8) -> %s' % (s['mittel'], s['max'], 'GRUEN' if s['ok'] else 'ROT'))
    z.append('   d je Station (px, + = Modell weiter innen): %s' % s['d'])
    s = r['S2']; z.append('S2 Uebergang J: (a) Mittel %.1f Max %d (<= 4 / <= 6); (b) Sattel %.1f bei x %d (<= 2); (c) Huckel an J %.1f bei x %d (<= 3) -> %s'
                          % (s['a']['mittel'], s['a']['max'], s['b'][0], s['b'][1], s['c']['huckel_px'], s['c']['bei_x'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S3']; z.append('S3 Ruecken/H/K: (a) Kuppe %.1f (<= 4); (b) H %.1f bei x %d (5-10, x 428-445); (c) weitere %s; (d) Sattel Kopf %.1f bei x %d (<= 2); (e) K %.1f (<= 3) -> %s'
                          % (s['a_kuppe'], s['b_huegel'][0], s['b_huegel'][1], s['c_weitere'], s['d_sattel_kopf'][0], s['d_sattel_kopf'][1], s['e_K'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S4']; z.append('S4 Breite: Wurzel %.1f (>= 55), s 90-130 %s (>= 36), Ende %.1f (<= 10), monoton %s; Folge %s -> %s'
                          % (s['wurzel'], s['s90_130'], s['ende'], s['monoton'], s['folge'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S5']; z.append('S5 Gesicht: Mesh 9 %s, Zeile 486 %s, Texel %s (%d) -> %s' % (s['mesh9'], s['zeile486'], s['texel'], s['n_texel'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S6']; z.append('S6 Boden: %d von %d Keyframes unter der tiefsten Koerperecke %s; Szene %s -> %s'
                          % (s['unter'], s['keyframes'], s['beispiele'], s['szene'], 'GRUEN' if s['ok'] else 'ROT'))
    if 'S7' in r:
        s = r['S7']; z.append('S7 Adern: (a) %s; (b) Oberarm 13 %.1f %% (35-59); (c) Rumpf 8 lok x<0 %.1f %% (25-80) [lok z<0: %.1f %%]; (d) Flecken %d (Original %d) -> %s'
                              % ('Herkunft ok' if s['a']['ok'] else 'Herkunft FALSCH %s' % s['a'], s['b_oberarm13'], s['c_rumpf8_lokx'], s['c_rumpf8_lokz_info'],
                                 s['d']['flecken_v8'], s['d']['flecken_original'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S8']; z.append('S8 Technik: %d Zeilen, Hoehe %d, Felder gueltig %s, %d Meshes, %d Knochen, %d Clips -> %s'
                          % (s['zeilen'], s['hoehe'], s['gueltig'], s['meshes'], s['knochen'], s['clips'], 'GRUEN' if s['ok'] else 'ROT'))
    rot = [k for k in ('S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8') if k in r and not r[k]['ok']]
    z.append('SUMME: %d von %d gruen%s' % (len([k for k in r if k.startswith('S')]) - len(rot), len([k for k in r if k.startswith('S')]),
                                           ('; ROT: ' + ', '.join(rot)) if rot else ''))
    return z, rot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd', nargs='?', default=V.ASSET)
    ap.add_argument('--v7', default=None, help='Referenz V7 (Standard: git show 6a25db27b:EM30_HANGAR.EMD nach SFV8/emd/EM30_V7.EMD)')
    ap.add_argument('--szene'); ap.add_argument('--aus'); ap.add_argument('--json')
    a = ap.parse_args()
    v7 = a.v7 or os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD')
    ref = open(v7, 'rb').read()
    assert V.sha(ref) == V.V7_SHA, 'Referenz V7 %s hat sha256 %s' % (v7, V.sha(ref))
    blob = open(a.emd, 'rb').read()
    r = alles(blob, ref, a.szene)
    z, rot = zeilen(r)
    kopf = ['Riegel V8 (Messblatt V8 Abschnitt 7) fuer %s' % a.emd, 'sha256 %s' % r['sha256'], 'Referenz V7 %s' % V.V7_SHA]
    txt = '\n'.join(kopf + z)
    print(txt)
    if a.aus:
        open(a.aus, 'w', encoding='utf-8').write(txt + '\n')
    if a.json:
        json.dump(r, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(1 if rot else 0)


if __name__ == '__main__':
    main()
