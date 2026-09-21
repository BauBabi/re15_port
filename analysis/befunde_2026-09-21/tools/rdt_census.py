#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""RE2-Leon RDT census: count object-model props (nOmodel) and cut out the MD1 blobs.

Measured header layout (derived at ROOM1110.RDT, cross-checked against the
already-extracted room1110/obj/model*.md1 file sizes):
  +0x00 u8 nSprite
  +0x01 u8 nCut          (= camera.rid size / 32)
  +0x02 u8 nOmodel       (= number of object-model props)
  +0x03 u8 nItem
  +0x04 u8 nDoor
  +0x05 u8 nRoom_at
  +0x06 u8 Reverb_lv
  +0x07 u8 (0x60)
  +0x08 .. +0x60  23 x u32 section offsets; header size = 0x64
  +0x30 -> object-model pointer table: nOmodel x { u32 tim_off; u32 md1_off }

Proof for the pointer-table layout (ROOM1110.RDT, table @0x1C4):
  md1_off[0]=0x3528 md1_off[1]=0x600C -> 0x600C-0x3528 = 10980 = size of
  room1110/obj/model00.md1 (10980 bytes). Same for model01..model11.
  tim_off[0]=0x44934 tim_off[1]=0x4CB54 -> 0x8220 = 33312 = model00.tim.
"""
import os, struct, sys, json

ROOT = r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\re2leon\PL0\RDT"


def census(path):
    d = open(path, 'rb').read()
    n = {}
    (n['nSprite'], n['nCut'], n['nOmodel'], n['nItem'], n['nDoor'],
     n['nRoom_at'], n['Reverb'], n['b7']) = struct.unpack_from('<8B', d, 0)
    offs = list(struct.unpack_from('<23I', d, 8))
    omdl = offs[(0x30 - 8) // 4]
    ents = []
    if n['nOmodel'] and omdl and omdl + 8 * n['nOmodel'] <= len(d):
        for i in range(n['nOmodel']):
            t, m = struct.unpack_from('<II', d, omdl + 8 * i)
            ents.append((t, m))
    # boundary set = every plausible offset in the file
    bset = set(o for o in offs if 0 < o <= len(d))
    for t, m in ents:
        if 0 < t <= len(d):
            bset.add(t)
        if 0 < m <= len(d):
            bset.add(m)
    bset.add(len(d))
    bl = sorted(bset)

    def nxt(o):
        for b in bl:
            if b > o:
                return b
        return len(d)
    out = []
    for i, (t, m) in enumerate(ents):
        out.append({'idx': i, 'tim_off': t, 'md1_off': m,
                    'md1_len': nxt(m) - m if 0 < m < len(d) else 0,
                    'tim_len': nxt(t) - t if 0 < t < len(d) else 0})
    return n, offs, out, len(d), omdl


def main():
    mode = sys.argv[1]
    rdts = sorted(f for f in os.listdir(ROOT) if f.upper().endswith('.RDT'))
    tot_omodel = 0
    tot_item = 0
    tot_door = 0
    rows = []
    for f in rdts:
        p = os.path.join(ROOT, f)
        n, offs, ents, size, omdl = census(p)
        tot_omodel += n['nOmodel']
        tot_item += n['nItem']
        tot_door += n['nDoor']
        rows.append({'file': f, 'hdr': n, 'omdl_tbl': omdl, 'size': size, 'ents': ents})
    print('RDT files              : %d' % len(rdts))
    print('sum nOmodel (props)    : %d' % tot_omodel)
    print('sum nItem              : %d' % tot_item)
    print('sum nDoor              : %d' % tot_door)
    if mode == 'json':
        json.dump(rows, open(sys.argv[2], 'w'), indent=0)
        print('wrote', sys.argv[2])


main()
