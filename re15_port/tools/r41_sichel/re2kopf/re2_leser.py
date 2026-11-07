"""Leser A (Runde 41, SF): RE2-Birkin-Kinds aus CDEMD0.EMS lesen (TOC @0x8009adf4, re2_ems_toc.inc).

    python re15_port/tools/r41_sichel/re2kopf/re2_leser.py            # Statistik 0x30/0x31/0x33/0x34/0x36

Index ((kind-0x10)*4+rec)*2 -> Sektor, Laenge; rec 2 = TIM, rec 3 = EMD (wie birkin_bahnsteig.lade_rohdaten).
"""
import os, re, sys, struct, hashlib
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, TOOLS)
import emd_ansichtsblatt as EA          # noqa: E402

BAUM = os.path.dirname(os.path.dirname(TOOLS))
EMS_PFAD = os.path.join(BAUM, 're15_port', 'shared_assets', 'RE2', 'CDEMD0.EMS')
TOC_PFAD = os.path.join(BAUM, 're15_port', 'engine', 'src', 'gen', 're2_ems_toc.inc')
u16, u32 = EA.u16, EA.u32

_toc = _ems = None
def toc():
    global _toc
    if _toc is None:
        inc = open(TOC_PFAD, encoding='utf-8', errors='replace').read()
        m = re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S)
        _toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', m.group(1))]
    return _toc

def ems():
    global _ems
    if _ems is None:
        _ems = open(EMS_PFAD, 'rb').read()
    return _ems

def rec(kind, r):
    i = ((kind - 0x10) * 4 + r) * 2
    t = toc()
    return t[i], t[i + 1], ems()[t[i] * 0x800:t[i] * 0x800 + t[i + 1]]

def lade(kind):
    """(emd, tim) eines Kinds."""
    return rec(kind, 3)[2], rec(kind, 2)[2]

def tim_info(tim):
    o = 8
    cs = u32(tim, o); cx, cy = u16(tim, o + 4), u16(tim, o + 6); cw, ch = u16(tim, o + 8), u16(tim, o + 10)
    o += cs
    ix, iy = u16(tim, o + 4), u16(tim, o + 6); iw, ih = u16(tim, o + 8), u16(tim, o + 10)
    return dict(bpp_flag=u32(tim, 4), clut_xy=(cx, cy), clut_wh=(cw, ch), img_xy=(ix, iy), img_wh_words=(iw, ih), img_px_w=iw * 2)

def modell(kind):
    """EA.Modell aus RE2-Daten (wie birkin_bahnsteig.modell)."""
    emd, tim = lade(kind)
    mo = EA.Modell.__new__(EA.Modell)
    d = EA.emd_dir(emd)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = emd, kind, 0, len(emd)
    mo.meshes = EA.md1_parse(emd[d[7]:])
    mo.skel = EA.emr_parse(emd[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(emd[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(tim)
    mo.texh, mo.texw = mo.tex.shape
    mo.tim = tim
    return mo

if __name__ == '__main__':
    for kind in (0x30, 0x31, 0x33, 0x34, 0x36):
        se, ln, emd = rec(kind, 3); st, lt, tim = rec(kind, 2)
        d = EA.emd_dir(emd)
        sk = EA.emr_parse(emd[d[2]:d[3]])
        clips, frames = EA.edd_parse(emd[d[1]:d[2]])
        ms = EA.md1_parse(emd[d[7]:])
        ti = tim_info(tim)
        print('kind 0x%02x: EMD sektor %d len %d sha %s | TIM sektor %d len %d' % (kind, se, ln, hashlib.sha256(emd).hexdigest()[:8], st, lt))
        print('   dir(%d):' % len(d), ['0x%x' % x for x in d])
        print('   knochen %d  kf_size %d  keyframes %d  clips %d  frames %d  meshes %d' % (sk.bone_count, sk.kf_size, sk.kf_count, len(clips), len(frames), len(ms)))
        print('   parents', sk.parent)
        print('   rel', sk.rel)
        print('   TIM', ti)
        print('   mesh tri/quad:', [(len(m.tf), len(m.qf)) for m in ms])
