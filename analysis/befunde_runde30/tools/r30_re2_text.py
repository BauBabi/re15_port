#!/usr/bin/env python3
"""RE2-Textdekoder (ENG), Runde 30. Zeichentabelle aus analysis/befunde_2026-09-26/
messung-re2-tonregel.md (dort aus ROOM1010.RDT Block 14 hergeleitet):
0x00=Leer, 0x0B-0x14='0'-'9', 0x1D-0x36='A'-'Z', 0x3D-0x56='a'-'z'.
Steuercodes: 0xFC/0xFD Zeilenumbruch (1 B); 0xF8/0xF9/0xFA/0xFB/0xFE 2 B.
Unbekannte Bytes erscheinen als <xx> - nichts wird geraten.
"""
PUNCT = {0x00: ' '}
def ch(b):
    if b in PUNCT: return PUNCT[b]
    if 0x0B <= b <= 0x14: return chr(ord('0') + b - 0x0B)
    if 0x1D <= b <= 0x36: return chr(ord('A') + b - 0x1D)
    if 0x3D <= b <= 0x56: return chr(ord('a') + b - 0x3D)
    return '<%02x>' % b
def decode(buf, off, limit=400):
    out = []; i = off
    while i < len(buf) and i - off < limit:
        b = buf[i]
        if b == 0xFE:
            out.append('{END:%02x}' % buf[i+1]); i += 2; break
        if b in (0xFC, 0xFD):
            out.append('\n' if b == 0xFC else '\p'); i += 1; continue
        if b in (0xF8, 0xF9, 0xFA, 0xFB):
            out.append('{%02X:%02x}' % (b, buf[i+1])); i += 2; continue
        out.append(ch(b)); i += 1
    return ''.join(out), i
if __name__ == '__main__':
    import struct, sys
    d = open('info/re2leon/PSX.EXE', 'rb').read()
    T = struct.unpack_from('<I', d, 0x18)[0]
    def off(a): return 0x800 + a - T
    # SET-A ENG: Tabelle @0x8009f368 (u16), Texte @0x8009efcc  (@0x80030130/@0x80030138)
    tab, base = 0x8009f368, 0x8009efcc
    n = (struct.unpack_from('<H', d, off(tab))[0])  # erster Offset = 0 -> Anzahl aus Tabellenabstand
    cnt = (tab - base)  # Texte liegen VOR der Tabelle
    first = struct.unpack_from('<H', d, off(tab))[0]
    # Anzahl Eintraege: bis zum ersten Offset, der >= (tab-base) waere, max 40
    i = 0
    while i < 40:
        o = struct.unpack_from('<H', d, off(tab) + i*2)[0]
        if o >= cnt: break
        s, e = decode(d, off(base + o))
        print('SET-A[%2d] @0x%08x (Tab @0x%08x = 0x%04x): %s' % (i, base + o, tab + i*2, o, s))
        i += 1
