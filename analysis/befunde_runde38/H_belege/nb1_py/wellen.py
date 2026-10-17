#!/usr/bin/env python3
"""Nachbesserung 1 Spur H R38: Wellen-Quellen (eigener Zugriff, Decoder aus ab0h/py/kreuz.py)."""
import os, sys, struct, hashlib
import numpy as np
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from kreuz import vag_decode, note2pitch2, bank
BAUM = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))
R2 = BAUM + "/info/re2leon/PL0/RDT/"
R2S = BAUM + "/info/re2leon/COMMON/SOUND/"
SND = BAUM + "/re15_port/shared_assets/PSX/SOUND/"
R15 = BAUM + "/re15_port/shared_assets/PSX/STAGE1/"

def welle_re2(path, satz):
    d = open(path, 'rb').read()
    edt, vh, vb = struct.unpack_from('<3I', d, 8)
    rec, off, sz, pitch, ti = bank(d, edt, vh, vb, satz)
    raw = d[off:off + sz]
    return np.array(vag_decode(raw), float), pitch, hashlib.sha1(raw).hexdigest()[:12], ti

def welle_r15(path, b, se):
    d = open(path, 'rb').read()
    edt, vh, vb = struct.unpack_from('<3I', d, 8 + b * 12)
    rec, off, sz, pitch, ti = bank(d, edt, vh, vb, se)
    raw = d[off:off + sz]
    return np.array(vag_decode(raw), float), pitch, hashlib.sha1(raw).hexdigest()[:12], ti

def welle_arms(edh, vbp, se, layer=0):
    e = open(edh, 'rb').read(); v = open(vbp, 'rb').read()
    vo = struct.unpack_from('<I', e, len(e) - 8)[0]
    d = e + v
    rec = d[se*4: se*4+4]
    prog = rec[1] & 0x7f; tone = (rec[2] >> 4) + layer
    vh = vo
    ps = struct.unpack_from('<H', d, vh + 0x12)[0]
    tb = vh + 0x20 + 0x800 + prog * 16 * 32 + tone * 32
    t = d[tb: tb + 32]
    vag = struct.unpack_from('<H', t, 0x16)[0]
    vt = vh + 0x20 + 0x800 + ps * 16 * 32
    sizes = [struct.unpack_from('<H', d, vt + 2 * i)[0] * 8 for i in range(256)]
    off = len(e) + sum(sizes[1:vag]); sz = sizes[vag]
    center, shift, mn = t[4], t[5], t[6]
    pitch = note2pitch2(mn, shift, center, shift)
    raw = d[off:off+sz]
    return np.array(vag_decode(raw), float), pitch, hashlib.sha1(raw).hexdigest()[:12], (t[2], t[3], center, shift, mn, vag, rec.hex())

# ENEMSE.VBS (RE2), TOC @0x800A7B1C (analysis/befunde_runde4_2026-09-12/tools/gator_banks.py)
EXE = open(BAUM + "/info/re2leon/PSX.EXE", "rb").read()
TADDR = struct.unpack_from("<I", EXE, 0x18)[0]
VBS = open(BAUM + "/re15_port/shared_assets/RE2/ENEMSE.VBS", "rb").read()
def enemse_vags(bnk):
    o = 0x800 + (0x800A7B1C - TADDR) + bnk * 0x10
    edt_size = struct.unpack_from("<I", EXE, o)[0]
    edt_off = (struct.unpack_from("<H", EXE, o+4)[0] | (EXE[o+6] << 16)) * 0x800
    vbd_size = struct.unpack_from("<I", EXE, o+8)[0]
    vbd_off = (struct.unpack_from("<H", EXE, o+12)[0] | (EXE[o+14] << 16)) * 0x800
    edt = VBS[edt_off:edt_off+edt_size]
    vh_off = struct.unpack_from("<I", edt, edt_size-8)[0]
    vh = edt[vh_off:]
    progs = struct.unpack_from("<H", vh, 0x12)[0]
    vags = struct.unpack_from("<H", vh, 0x16)[0]
    vag_tab = 0x820 + 0x200 * progs
    sizes = [struct.unpack_from("<H", vh, vag_tab + 2*i)[0] * 8 for i in range(vags + 1)]
    tones = {}
    for p in range(progs):
        for t in range(16):
            oo = 0x820 + p*0x200 + t*0x20
            vag = struct.unpack_from("<H", vh, oo+22)[0]
            if vag and vag not in tones:
                tones[vag] = (vh[oo+4], vh[oo+5], vh[oo+6], vh[oo+2])
    out = {}
    acc = 0
    vb = VBS[vbd_off:vbd_off+vbd_size]
    for i in range(1, vags + 1):
        raw = vb[acc:acc+sizes[i]]; acc += sizes[i]
        c, s, mn, vol = tones.get(i, (60, 0, 60, 127))
        out[i] = (np.array(vag_decode(raw), float), note2pitch2(mn, s, c, s), hashlib.sha1(raw).hexdigest()[:12], (vol, c, s, mn))
    return out
