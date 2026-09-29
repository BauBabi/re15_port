#!/usr/bin/env python3
"""Runde 34 (Granaten) - Port-Inventar: SE-Records der Granaten-SEs in den Baenken.

Liest SOUND/<BANK>.EDH aus re15_port/shared_assets/PSX und zerlegt EDT-Records genau wie
engine/src/vab_common.c:252-265 (re15_edt_decode):
  e[0] bit7 -> vab_override, e[1]&0x7f -> prog, e[2]>>4 -> tone, e[2]&0xf -> prio_nib,
  (e[3]&0x1f)-0x10 -> voice, e[3]>>5 -> extra layers, empty = (e[2]==0 && e[3]==0).
Record-Anzahl = pBAV_off/4, pBAV_off = u32 @edh[size-8] (audio_pc.c load_weapon_se_vab_pc).

Aufruf: se_records.py [BANK:rec ...]   Default: die im Round-30-Dossier genannten SEs
  0x010A0001 -> Bank 1 (ARMS der ausgeruesteten Waffe) Record 0x0A
  0x04080001 -> Bank 4 (CORE) Record 0x08
Nur lesend.
"""
import os
import struct
import sys

ROOT = os.path.join(os.path.dirname(__file__), '..', '..', '..', 're15_port',
                    'shared_assets', 'PSX', 'SOUND')


def dec(e):
    return dict(raw=' '.join(f'{x:02x}' for x in e),
                vab_override=(e[0] & 0x7f) if (e[0] & 0x80) else -1,
                prog=e[1] & 0x7f, tone=e[2] >> 4, prio_nib=e[2] & 0xf,
                voice=(e[3] & 0x1f) - 0x10, extra=e[3] >> 5,
                empty=(e[2] == 0 and e[3] == 0))


def main():
    reqs = sys.argv[1:] or ['ARMS09:0', 'ARMS09:1', 'ARMS09:2', 'ARMS09:0x0a', 'ARMS0A:0x0a',
                            'ARMS0B:0x0a', 'ARMS01:0x0a', 'ARMS03:0x0a', 'CORE00:8', 'CORE04:8']
    for r in reqs:
        bank, rec = r.split(':')
        rec = int(rec, 0)
        p = os.path.join(ROOT, bank + '.EDH')
        b = open(p, 'rb').read()
        pbav = struct.unpack_from('<I', b, len(b) - 8)[0]
        n = pbav // 4
        if rec >= n:
            print(f'{bank} rec 0x{rec:02x}: AUSSERHALB (Records {n})')
            continue
        e = b[4 * rec:4 * rec + 4]
        d = dec(e)
        print(f'{bank}.EDH ({len(b)} B, pBAV@0x{pbav:X}, {n} Records) rec 0x{rec:02x} @0x{4*rec:03X}: {d}')
    # Vollbild der ARMS09-Tabelle (nicht-leere Records)
    b = open(os.path.join(ROOT, 'ARMS09.EDH'), 'rb').read()
    pbav = struct.unpack_from('<I', b, len(b) - 8)[0]
    ne = [(i, ' '.join(f'{x:02x}' for x in b[4*i:4*i+4])) for i in range(pbav // 4)
          if not (b[4*i+2] == 0 and b[4*i+3] == 0)]
    print(f'ARMS09 nicht-leere Records ({len(ne)} von {pbav//4}): {ne}')
    for bk in ('ARMS0A', 'ARMS0B'):
        c = open(os.path.join(ROOT, bk + '.EDH'), 'rb').read()
        print(f'{bk}.EDH == ARMS09.EDH: {c == b}')
    for bk in ('ARMS0A', 'ARMS0B'):
        v1 = open(os.path.join(ROOT, 'ARMS09.VB'), 'rb').read()
        v2 = open(os.path.join(ROOT, bk + '.VB'), 'rb').read()
        print(f'{bk}.VB == ARMS09.VB: {v1 == v2}')


if __name__ == '__main__':
    main()
