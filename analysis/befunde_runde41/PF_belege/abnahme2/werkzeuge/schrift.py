"""Eigene Dekodierung der 8x8-Spielschrift aus DATA/TEX.TIM (4 bpp) und Zeichenerkennung im Panel-Bild.
Zeichen c: k = c - 0x20, u = 256 + (k & 0x1f) * 8, v = ((k & 0xe0) >> 5) * 8 (FUN_80029214 @0x8002931c..@0x8002932c, tpage VRAM 768 = TIM-Texel 256)."""
import struct
import numpy as np

TIM = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/shared_assets/PSX/DATA/TEX.TIM'


def lade():
    b = open(TIM, 'rb').read()
    magic, flags = struct.unpack_from('<II', b, 0)
    assert magic == 0x10 and (flags & 7) == 0 and flags & 8, (magic, flags)
    o = 8
    clen, cx, cy, cw, ch = struct.unpack_from('<IHHHH', b, o)
    o += clen
    ilen, ix, iy, iw, ih = struct.unpack_from('<IHHHH', b, o)
    data = np.frombuffer(b, np.uint16, iw * ih, o + 12).reshape(ih, iw)
    idx = np.zeros((ih, iw * 4), np.uint8)
    for k in range(4):
        idx[:, k::4] = (data >> (4 * k)) & 0xF
    return idx


_IDX = None


def glyphe(c):
    global _IDX
    if _IDX is None:
        _IDX = lade()
    k = c - 0x20
    u = 256 + (k & 0x1f) * 8
    v = ((k & 0xe0) >> 5) * 8
    return _IDX[v:v + 8, u:u + 8]


ZEICHEN = [c for c in range(0x21, 0x7f)]


def erkenne(zelle, fremd=None, transp=(0,)):
    """zelle 8x8x3 (logisch). Bestes Zeichen: jede Glyphen-Klasse (Index != 0) genau eine Farbe, Tinte unterscheidet sich vom Grund.
    Rueckgabe (zeichen, fehler, zweitbester_abstand)."""
    z = zelle.astype(int)
    if fremd is None:
        fremd = np.zeros((8, 8), bool)
    erg = []
    for c in ZEICHEN:
        g = glyphe(c).copy()
        g[np.isin(g, list(transp))] = 0
        if not (g != 0).any():
            continue
        fehler = 0
        for i in np.unique(g):
            if i == 0:
                continue
            px = z[(g == i) & ~fremd]
            if len(px) == 0:
                continue
            fehler += int(np.abs(px - px[0]).sum())
        tinte = z[g != 0].mean(0)
        grund = z[(g == 0) & ~fremd]
        # Grundpixel duerfen nicht die Tintenfarbe der Hauptklasse tragen (sonst passt jede "duennere" Glyphe)
        haupt = np.bincount(g[g != 0].ravel()).argmax()
        hf = z[g == haupt][0]
        falsch = int((np.abs(grund - hf).sum(-1) == 0).sum())
        erg.append((fehler + 1000 * falsch, c))
    erg.sort()
    return chr(erg[0][1]), erg[0][0], erg[1][0] - erg[0][0]
