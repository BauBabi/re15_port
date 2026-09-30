#!/usr/bin/env python3
"""Runde 34 (Granaten) - Port-Inventar: liegen die CLUT-Zeilen der Granaten-Effekte in DATA/TEX.TIM?

CLUT-Wort (PSX GPU): x = (clut & 0x3f) * 16, y = clut >> 6  (psx-spx "Texture Palette").
Die Seeds stammen aus der Port-Formel esp_fx_seed_header (engine/src/re15_esp.c:749-758):
  CLUT = hdr_clut + ((sub & 0xff) >> 3) * 0x40   (hdr_clut = EFF-Header u16 @+4, esp_rows.py)
Liest den TIM-CLUT-Block von DATA/TEX.TIM (TIM: u32 magic 0x10, u32 flags, CLUT-Block
{u32 len, u16 x, u16 y, u16 w, u16 h, data}) und gibt die 16 Eintraege je gefragter Zeile aus.
Nur lesend.
"""
import os
import struct

P = os.path.join(os.path.dirname(__file__), '..', '..', '..', 're15_port', 'shared_assets', 'PSX',
                 'DATA', 'TEX.TIM')

SEEDS = [
    ('Effekt 3 Grundpalette (Rauch)', 0x7811),
    ('Effekt 3 sub 0x0B (Kind 0x030B5400/5800)', 0x7851),
    ('Effekt 3 sub 0x19 (Kind 0x03195000)', 0x78D1),
    ('Effekt 8 (Feuer, geladen aus TEX.TIM)', 0x7911),
    ('Effekt 0 (Blut)', 0x7951),
    ('Effekt 2 (Muendung)', 0x7A51),
    ('Effekt 4 Grundpalette', 0x7AD1),
    ('Effekt 4 sub 0x0D (Granate im Flug)', 0x7B11),
]


def main():
    b = open(P, 'rb').read()
    magic, flags = struct.unpack_from('<II', b, 0)
    assert magic == 0x10, hex(magic)
    has_clut = bool(flags & 8)
    ln, cx, cy, cw, ch = struct.unpack_from('<IHHHH', b, 8)
    print(f'TEX.TIM {len(b)} B flags=0x{flags:x} (bpp-mode {flags & 3}, CLUT {has_clut}) '
          f'CLUT-Block @0x8 len={ln} VRAM({cx},{cy}) {cw}x{ch}')
    data = 8 + 12
    for name, c in SEEDS:
        x = (c & 0x3f) * 16
        y = c >> 6
        inside = cx <= x < cx + cw and cy <= y < cy + ch
        if not inside:
            print(f'{name}: CLUT 0x{c:04X} -> VRAM({x},{y}) AUSSERHALB des TEX.TIM-CLUT-Blocks')
            continue
        off = data + ((y - cy) * cw + (x - cx)) * 2
        ent = struct.unpack_from('<16H', b, off)
        nz = sum(1 for e in ent if e)
        print(f'{name}: CLUT 0x{c:04X} -> VRAM({x},{y}) Datei 0x{off:05X}: '
              + ' '.join(f'{e:04x}' for e in ent) + f'  (nicht-null {nz}/16)')


if __name__ == '__main__':
    main()
