#!/usr/bin/env python3
"""Runde 34 (Granaten) - Port-Inventar: Clip-Laengen der Granaten-Waffenbaenke.

Liest PLD/PL00W09..0B.PLW und PL04W09..0B.PLW aus re15_port/shared_assets/PSX,
schneidet dir[0] (EDD) genau wie platform/pc/main.c:3958-3980 (Directory-Offset
im ersten u32, 4 u32-Eintraege) und zaehlt die Clips wie
engine/src/emd_common.c:48-99 (re15_emd_parse_animation: clip_count = offset0/4,
count je Clip im u16 an +0 jedes 4-Byte-Kopfes).

Zusaetzlich: sha1 der Dateien (byte-gleich?), und die oberen Bits (>>12) der
Frame-Woerter von Clip 7/9/11 (Keyframe-Event-Bits, falls vorhanden).
Nur lesend.
"""
import hashlib
import os
import struct
import sys

ROOT = os.path.join(os.path.dirname(__file__), '..', '..', '..', 're15_port',
                    'shared_assets', 'PSX', 'PLD')


def u32(b, o):
    return struct.unpack_from('<I', b, o)[0]


def u16(b, o):
    return struct.unpack_from('<H', b, o)[0]


def edd_clips(edd):
    count0 = u16(edd, 0)
    off0 = u16(edd, 2)
    n = off0 // 4
    cl = [count0]
    for i in range(1, n):
        cl.append(u16(edd, 4 * i))
    frames = []
    total = sum(cl)
    for i in range(total):
        frames.append(u32(edd, off0 + 4 * i))
    return cl, frames


def main():
    names = sys.argv[1:] or ['PL00W09', 'PL00W0A', 'PL00W0B', 'PL04W09', 'PL04W0A',
                             'PL04W0B', 'PL00W03', 'PL00W08']
    for nm in names:
        p = os.path.join(ROOT, nm + '.PLW')
        if not os.path.exists(p):
            print(f'{nm}: FEHLT ({p})')
            continue
        b = open(p, 'rb').read()
        diroff = u32(b, 0)
        de = [u32(b, diroff + 4 * k) for k in range(4)]
        edd = b[de[0]:de[1]]
        cl, frames = edd_clips(edd)
        sha = hashlib.sha1(b).hexdigest()[:12]
        print(f'{nm}.PLW {len(b)} B sha1={sha} dir@0x{diroff:X} de={[hex(x) for x in de]}')
        print(f'   EDD {len(edd)} B, {len(cl)} Clips: {cl}')
        start = 0
        for ci, c in enumerate(cl):
            if ci in (6, 7, 8, 9, 10, 11, 12):
                fl = [(frames[start + k] >> 12) for k in range(c)]
                kf = [(frames[start + k] & 0xfff) for k in range(c)]
                nz = [(k, hex(v)) for k, v in enumerate(fl) if v]
                print(f'   Clip {ci:2d} fc={c:3d} kf={kf[0]}..{kf[-1] if kf else "-"} '
                      f'Flag-Bits(>>12)!=0: {nz if nz else "keine"}')
            start += c


if __name__ == '__main__':
    main()
