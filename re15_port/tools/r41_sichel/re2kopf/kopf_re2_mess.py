"""SF V5 Kopf aus RE2: Messungen am Render (b2, Spielkamera 50A0 Cut 12, RDT-Licht; Posen posen_v4 "135" / "85") fuer Suche und Riegel.

Masse (Messblatt V4 Abschnitt 2/3 und Leser B):
  K10  Kopfbox (Bbox Id 9) in H (Pose 135)                 Soll 0,14-0,18 x 0,15-0,20 H (Messblatt), Mitte 0,154 x 0,187 (ot3)
  K4   Auge (B)-links der Kopfmitte / unter der Oberkante   Soll 0,035-0,065 H / 0,07-0,12 H
  K7   Nase/Maul: Zahn-Schwerpunkt in der Kopfbox (Anteile) Soll x 0,3-0,7, y 0,55-1,0 (unter dem Auge, Mitte; Leser B: Maul an der Unterseite)
  S2   Auge seitlich: hinter der Vorderkante / ueber Sohle  Soll 0,03-0,07 H / 0,76-0,82 H
  KS2  Haarkante seitlich: Fenster (0,84-0,96 / 0,52-0,80 der Kopfbox) L* <= Kopf-Median - 5 (Videoraum)
Auge und Zaehne werden nicht per Helligkeitsschwelle gesucht, sondern per MARKER-Render: die Augen-Texel (G2-Zeile, Kern des (W)-rechten
Auges) bzw. die Zahn-Texel (G3-Kachel, L* > 60 und Chroma < 20) bekommen voruebergehend einen Magenta-Eintrag; so ist die Lage eindeutig.
"""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, HIER); sys.path.insert(0, TOOLS); sys.path.insert(0, os.path.join(TOOLS, 'r41_sichel', 'v4'))
import v4_gemein as G
import b2
import riegel_v4 as RV
import kopf_re2_bau as KB
from skimage.color import rgb2lab

MAG = np.array([255, 0, 255], np.uint8)
AUGE_T = [19, 20, 41]       # Flaechen (Dreiecke, G2-Mesh-3-Zaehlung) des (W)-rechten Auges: Faecher um den UV-Eckpunkt (17,10) vorn rechts
                            # (x 78-119, z -98..-125); dieselben Texel liegen auch hinten rechts auf T3/T8 (x -6..12) - das ist kein Auge


def teil(mo, keep_t, keep_q=None):
    """Kopie des Modells, Mesh 9 nur mit den Flaechen keep_t (Dreiecke) / keep_q (Vierecke)."""
    import copy
    mo2 = copy.copy(mo); mo2.meshes = list(mo.meshes); m9 = copy.copy(mo.meshes[KB.KOPF])
    m9.tf = [mo.meshes[KB.KOPF].tf[i] for i in keep_t]; m9.tuv = [mo.meshes[KB.KOPF].tuv[i] for i in keep_t]
    kq = keep_q if keep_q is not None else []
    m9.qf = [mo.meshes[KB.KOPF].qf[i] for i in kq]; m9.quv = [mo.meshes[KB.KOPF].quv[i] for i in kq]
    mo2.meshes[KB.KOPF] = m9
    return mo2


def _lab(rgb):
    return rgb2lab(np.asarray(rgb, float).reshape(-1, 1, 3) / 255.0).reshape(-1, 3)


_g2 = {}


def _g2_pal():
    if 'pal' not in _g2:
        import re2_leser as RL
        _g2['pal'] = RL.modell(KB.G2).pal[0]
    return _g2['pal']


def auge_texel(mo, prot=None):
    """Maske (im EM30-Atlas) des (W)-rechten G2-Auges: heller Kern (L* > 45) plus 1 Texel Ring in der Zeile 486 im Fenster G2 u 8-26 /
    v 4-16 (Flaechen T0/T19/T20/T41 bei z -72..-125 = (W)-rechte Kopfseite; die Texel der Mittelspalte u 41-60 und der Wangenstreifen
    u 24-28 v 26-34 sind kein Auge; probe4). Leser A: Auge bei x 0,20 der Kopfbox = z ~ -110."""
    o = KB.G2_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']
    if mo.pal.shape[0] <= KB.ZEILE_G2 - mo.clut_y:
        return np.zeros(mo.tex.shape, bool)             # Modell ohne RE2-Zeilen (Gegenprobe ot3): kein G2-Auge
    sub = mo.tex[o['v0']:o['v0'] + 90, x0:x0 + 126]
    # Kern nach der QUELL-Palette (G2 Zeile 0) bestimmt, damit die Belichtung der Zeile 486 (Runde 2) die Marker-Menge nicht aendert
    lab = _lab(_g2_pal())[sub]
    m = (lab[..., 0] > 45) & (sub != 0)      # heller Kern des Auges
    k = np.zeros_like(m); k[4:17, 8:27] = True   # (W)-rechtes Auge: G2 u 8-26, v 4-16 (Flaechen T0/T19/T20/T41, z -72..-125; probe4)
    m &= k
    m = cv2.dilate(m.astype(np.uint8), np.ones((3, 3), np.uint8), iterations=1) > 0    # Kern + roter Ring (1 Texel)
    m &= (sub != 0) & k
    maske = np.zeros(mo.tex.shape, bool); maske[o['v0']:o['v0'] + 90, x0:x0 + 126] = m
    return maske


def zahn_texel(mo):
    o = KB.G3_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']; h = o['v'][1] - o['v'][0] + 1; w = o['u'][1] - o['u'][0] + 1
    if mo.pal.shape[0] <= KB.ZEILE_G3 - mo.clut_y:
        return np.zeros(mo.tex.shape, bool)
    sub = mo.tex[o['v0']:o['v0'] + h, x0:x0 + w]
    lab = _lab(mo.pal[KB.ZEILE_G3 - mo.clut_y])[sub]
    m = (lab[..., 0] > 60) & (np.hypot(lab[..., 1], lab[..., 2]) < 20) & (sub != 0)
    maske = np.zeros(mo.tex.shape, bool); maske[o['v0']:o['v0'] + h, x0:x0 + w] = m
    return maske


def marker_render(mo, pose, maske, zeile):
    """Render, in dem die Texel der Maske (CLUT-Zeile zeile) magenta sind; Rueckgabe Maske der Magenta-Pixel."""
    tex0, lut0 = mo.tex, mo.lut
    z = zeile - mo.clut_y
    lut = lut0.copy()
    if not maske.any() or z >= lut.shape[0]:
        return np.zeros((480, 640), bool)
    # Marker-Index: ein Eintrag, der in der GANZEN Region der Zeile (G2-Ort bzw. G3-Kachel) nicht vorkommt - nicht nur ausserhalb der Maske
    # (Zahlen-Pruefung RE2-Kopf r1, Info: Index 1 stand 21-mal sonst in der G2-Region); ersatzweise der seltenste Eintrag
    region = np.zeros(mo.tex.shape, bool)
    for o in (KB.G2_ORT, KB.G3_ORT):
        x0_ = 128 * (o['page'] & 0xF) + o['u0']
        region[o['v0']:o['v0'] + o['v'][1] - o['v'][0] + 1, x0_:x0_ + o['u'][1] - o['u'][0] + 1] = True
    belegt = np.bincount(mo.tex[region].ravel(), minlength=256)
    frei = [i for i in range(1, 256) if belegt[i] == 0]
    idx = frei[0] if frei else int(1 + np.argmin(belegt[1:]))
    lut[z, idx, :3] = MAG; lut[z, idx, 3] = 255
    tex = tex0.copy(); tex[maske] = idx
    mo.tex, mo.lut = tex, lut
    try:
        img, ids = G.zeichne(mo, pose, licht=False)
    finally:
        mo.tex, mo.lut = tex0, lut0
    m = (img[..., 0] > 200) & (img[..., 1] < 60) & (img[..., 2] > 200) & (ids == KB.KOPF)
    return m


def messen(mo, P=None, bilder=False):
    P = P or G.posen()
    out = {}
    am, zm = auge_texel(mo), zahn_texel(mo)
    for h in ('135', '85'):
        img, ids, iv = G.zeichne(mo, P[h], voll=True)
        ok, so, H = G.h_port(iv)
        kb = G.kopfbox(ids)
        r = dict(H=float(H), ok=int(ok), sohle=int(so), kopfbox=kb)
        if kb:
            x0, y0, x1, y1 = kb
            r['K10'] = ((x1 - x0 + 1) / H, (y1 - y0 + 1) / H)
            ma = marker_render(mo, P[h], am, KB.ZEILE_G2)
            if ma.any() and len(mo.meshes[KB.KOPF].tf) > max(AUGE_T):
                _i2, ids_a = G.zeichne(teil(mo, AUGE_T), P[h], licht=False)
                ma &= ids_a == KB.KOPF
            mz = marker_render(mo, P[h], zm, KB.ZEILE_G3)
            xm = 0.5 * (x0 + x1)
            if ma.any():
                nl, lb, st, _ = cv2.connectedComponentsWithStats(ma.astype(np.uint8), connectivity=8)
                if nl > 2:
                    ma = lb == (1 + int(np.argmax(st[1:, cv2.CC_STAT_AREA])))
                ys, xs = np.nonzero(ma); sp = (float(xs.mean()), float(ys.mean()))
                r['auge'] = dict(px=int(ma.sum()), sp=sp, bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())])
                if h == '135':
                    r['K4'] = ((xm - sp[0]) / H, (sp[1] - y0) / H)
                else:
                    r['S2'] = ((x1 - sp[0]) / H, (so - sp[1]) / H)
            if mz.any():
                ys, xs = np.nonzero(mz); sp = (float(xs.mean()), float(ys.mean()))
                r['zaehne'] = dict(px=int(mz.sum()), sp=sp, anteil=((sp[0] - x0) / (x1 - x0 + 1), (sp[1] - y0) / (y1 - y0 + 1)),
                                   bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())])
            else:
                r['zaehne'] = dict(px=0)
            V = G.videoraum(img); Lab = G.lab(V)
            km = ids == KB.KOPF
            r['kopf_p50_v'] = float(np.median(Lab[km][:, 0]))
            r['kopf_lab_roh'] = [float(v) for v in np.median(G.lab(img.astype(float))[km], 0)]
            if h == '85':
                l3, bx3 = RV.fenster_lab(V, kb, RV.FENSTER['KS2'])
                sub = V[bx3[1]:bx3[3] + 1, bx3[0]:bx3[2] + 1]; mk = km[bx3[1]:bx3[3] + 1, bx3[0]:bx3[2] + 1]
                lk = RV.lab1(sub[mk].mean(0)) if mk.any() else [float('nan')] * 3
                r['KS2'] = dict(lab=[float(v) for v in l3], box=bx3, kopf=[float(v) for v in lk], kopf_anteil=float(mk.mean()))
            # Haelften (Pose 135): L* links / rechts der Kopfmitte (roh)
            Lr = G.lab(img.astype(float))[..., 0]
            xs_ = np.arange(img.shape[1])[None, :]
            li = km & (xs_ < xm); re = km & (xs_ >= xm)
            r['haelften_L'] = (float(np.median(Lr[li])) if li.any() else None, float(np.median(Lr[re])) if re.any() else None)
            if bilder:
                r['img'] = img; r['ids'] = ids; r['ma'] = ma; r['mz'] = mz
        out[h] = r
    return out


def kurz(m):
    a = m['135']; b = m['85']
    s = 'K10 %.3f x %.3f H' % a.get('K10', (0, 0))
    s += ' | K4 %s' % ('%.3f / %.3f' % a['K4'] if 'K4' in a else '-')
    s += ' | Zaehne135 %d px%s' % (a['zaehne']['px'], (' @ %.2f/%.2f' % a['zaehne']['anteil']) if a['zaehne']['px'] else '')
    s += ' | S2 %s' % ('%.3f / %.3f' % b['S2'] if 'S2' in b else '-')
    s += ' | Zaehne85 %d' % b['zaehne']['px']
    s += ' | KS2 L* %.1f / Kopf %.1f (%.0f %%; Median %.1f)' % (b['KS2']['lab'][0], b['KS2']['kopf'][0], 100 * b['KS2']['kopf_anteil'], b['kopf_p50_v']) if 'KS2' in b else ''
    s += ' | Haelften135 L %s' % (a.get('haelften_L'),)
    return s


if __name__ == '__main__':
    pfad = sys.argv[1]
    mo = b2.modell(pfad)
    m = messen(mo)
    print(kurz(m))
    print(json.dumps({h: {k: v for k, v in r.items() if k not in ('img', 'ids', 'ma', 'mz')} for h, r in m.items()}, indent=1, default=str))
