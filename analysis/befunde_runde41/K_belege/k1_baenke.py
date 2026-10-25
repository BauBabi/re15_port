#!/usr/bin/env python3
"""Spur K (Runde 41): Banken je Typ/Archiv vergleichen (dir-Tabelle, Skelett-Hierarchie, Bind-Lagen, Clips je Bank).
Leser = re15_port/tools/emd_ansichtsblatt.py (Port der Engine-Leser emd_common.c); Lage je Blob aus der EXE-TOC
(CDEMD0 @0x80072f38, CDEMD1 @0x80073178; 8 B je Typ: u32 Sektor, u32 Laenge; FUN_80022300 @0x80022370/@0x800223e0).
    python k1_baenke.py
"""
import os, sys, struct, importlib.util
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
spec = importlib.util.spec_from_file_location('eab', os.path.join(REPO, 're15_port', 'tools', 'emd_ansichtsblatt.py'))
eab = importlib.util.module_from_spec(spec); spec.loader.exec_module(eab)
exe = open(os.path.join(REPO, 'info', 'Re1.5', 'PSX.EXE'), 'rb').read()
def toc(arch, typ):
    base = 0x80072f38 if arch == 0 else 0x80073178
    a = base + (typ - 0x10) * 8
    o = a - 0x80010000 + 0x800
    s, l = struct.unpack_from('<II', exe, o)
    return a, s, l, exe[o:o+8].hex(' ')
def blob(arch, typ):
    a, s, l, hx = toc(arch, typ)
    d = open(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'EMD', 'CDEMD%d.EMS' % arch), 'rb').read()
    return d[s*2048:s*2048+l], (a, s, l, hx)
for arch, typ in ((0, 0x42), (1, 0x42), (0, 0x4B), (1, 0x4B), (1, 0x49)):
    b, (a, s, l, hx) = blob(arch, typ)
    d = eab.emd_dir(b)
    print('CDEMD%d Typ 0x%02X: TOC @0x%08x = %s -> Sektor %d, %d B; dir-Eintraege %d' % (arch, typ, a, hx, s, l, len(d)))
    ends = d[1:] + [len(b)]
    for i in range(len(d)):
        print('   dir[%d] @0x%05x  %6d B' % (i, d[i], ends[i] - d[i]))
    sk = eab.emr_parse(b[d[2]:d[3]])
    print('   EMR dir2: Knochen %d  Eltern %s' % (sk.bone_count, sk.parent))
    print('   Bind-Lagen %s' % (sk.rel,))
    for nm, (ei, ri) in (('Bank0 dir1/dir2', (1, 2)), ('eigene dir3/dir4', (3, 4)), ('Opfer dir5/dir6', (5, 6))):
        if ei < len(d) and ends[ei] - d[ei] > 0:
            try:
                clips, frames = eab.edd_parse(b[d[ei]:ends[ei]])
                print('   %s: %d Clips, Bilder je Clip %s' % (nm, len(clips), [c[1] for c in clips]))
            except Exception as ex:
                print('   %s: Parse-Fehler %r' % (nm, ex))
    meshes = eab.md1_parse(b[d[7]:d[8]])
    print('   MD1: %d Meshes, Dreiecke je Mesh %s, Quads je Mesh %s' % (len(meshes), [len(m.tf) for m in meshes], [len(m.qf) for m in meshes]))
