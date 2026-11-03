"""Nachbesserung 3 M1: Bildanteil Riesenform / Annette je 5090-Schnitt (Geometrie wie form_buehne, ID-Puffer statt Textur)."""
import sys, os, struct, math
import numpy as np
BAUM = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette"
sys.path.insert(0, BAUM + "/re15_port/tools")
import form_buehne as FB, form_emd as FE, emd_ansichtsblatt as EA

RE2 = BAUM + "/re15_port/shared_assets/RE2/"
def _form():
    return FE.modell(open(RE2 + "BAHNSTEIG_FORM.EMD", "rb").read(), open(RE2 + "BAHNSTEIG_FORM.TIM", "rb").read())
def _annette():
    b = open(RE2 + "ANNETTE_4D.EMD", "rb").read()
    do, n = struct.unpack_from("<II", b, 0)
    d = list(struct.unpack_from("<%dI" % n, b, do))
    mo = EA.Modell.__new__(EA.Modell)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = b, 0x4D, 0, len(b)
    mo.meshes = EA.md1_parse(b[d[7]:d[8]])
    mo.skel = EA.emr_parse(b[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(b[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(b[d[8]:do])
    mo.texh, mo.texw = mo.tex.shape
    return mo
FORM = _form(); FRAU = _annette()
RAUM = FB.Raum("5090")
def kam(cut):
    s = RAUM.schnitt(cut)
    return FB.Kamera.aus_rid(s["pos"], s["ziel"], s["fov"])

def kf(mo, clip, bild):
    f0, n = mo.clips[clip]
    return mo.frames[f0 + min(bild, n - 1)] & 0xFFF

def idpuffer(cut, objs, W=640, H=480):
    """objs: [(mo, clip, bild, (x,y,z), gier, id)] -> ID-Puffer (0 = Hintergrund) und Einzelmasken (ohne Verdeckung)."""
    k = kam(cut)
    alle = []
    einzel = {}
    for mo, clip, bild, lage, gier, oid in objs:
        tr = FB.dreiecke(mo, kf(mo, clip, bild), lage, gier, k, None)
        alle += [(t[0], t[1], oid) for t in tr]
        einzel[oid] = tr
    buf = np.zeros((H, W), np.uint8)
    def rast(tris, ziel, val):
        for _t, p, oid in sorted(tris, key=lambda t: -t[0]):
            x = np.array([q[0] + FB.PHASE for q in p], float) * 2
            y = np.array([q[1] + FB.PHASE for q in p], float) * 2
            fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
            if abs(fl) < 1e-9: continue
            x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, W)
            y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, H)
            if x0 >= x1 or y0 >= y1: continue
            PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
            w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
            w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
            inn = (w1 >= 0) & (w2 >= 0) & (1 - w1 - w2 >= 0)
            s = ziel[y0:y1, x0:x1]; s[inn] = oid if val is None else val
    rast(alle, buf, None)
    allein = {}
    for oid, tr in einzel.items():
        m = np.zeros((H, W), np.uint8); rast([(t[0], t[1], oid) for t in tr], m, 1); allein[oid] = m.astype(bool)
    return buf, allein

BAND = (48, 432)   # Kinobalken (0,0)-(320,24) / (0,216)-(320,240), render_pc.c FUN_80021a0c
def mass(buf, allein, form_id=1, frau_id=2):
    y0, y1 = BAND
    sicht = buf[y0:y1]
    flaeche = sicht.size
    r = dict(form=float((sicht == form_id).sum()) / flaeche)
    if frau_id in allein:
        a = allein[frau_id]
        tot = a.sum()
        sichtbar = (buf == frau_id).sum()
        r["frau_sichtbar"] = float(sichtbar) / tot if tot else 0.0
        ys, xs = np.nonzero(a)
        r["frau_bbox"] = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())) if len(xs) else None
        voll = len(xs) and xs.min() > 0 and xs.max() < 639 and ys.min() >= y0 and ys.max() < y1
        # Teile des ganzen Koerpers ausserhalb des Bildes: ungeschnittene Projektion fehlt -> nur bbox-Pruefung
        r["frau_ganz"] = bool(voll)
        r["frau_px"] = int(tot)
    return r

def blick(ax, az, tx, tz):
    """Gier wie annette_r41.c blick(): (re15_atan2_q12(dz, dx) - 0x400) & 0xfff; re15_atan2_q12(dz, dx) = atan2(dx, dz) in
    4096 je Umdrehung (gemessen im Fensterlauf: L -> S ergibt 3957, Annette S -> L 1915)."""
    return (int(round(math.atan2(tx - ax, tz - az) * 2048 / math.pi)) - 0x400) & 0xfff

def bild(buf, pfad):
    from PIL import Image
    pal = np.array([[30, 30, 30], [200, 60, 60], [240, 240, 240], [60, 160, 60]], np.uint8)
    Image.fromarray(pal[np.clip(buf, 0, 3)]).save(pfad)
