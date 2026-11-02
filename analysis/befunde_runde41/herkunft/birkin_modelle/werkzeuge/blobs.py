#!/usr/bin/env python3
"""Inventar der EMS-Archive von Disc und Mod: TOC der jeweiligen EXE (Leon @0x80072f38, Elza @0x80073178, Index Typ-0x10,
{Sektor, Laenge}) gegen die Blobkette (sektorausgerichtet, u32 dir_off, dir_off+36 = Laenge). Zeigt auch Blobs ohne TOC-Eintrag."""
import struct, hashlib, sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/re15_port/tools')
REPO = 'C:/workspace/git/reAi_v2/'
QUELLEN = {
    'DISC': (REPO + 'Version_40%/PSX.EXE', REPO + 'Version_40%/PSX/EMD/'),
    'MOD':  (REPO + 'info/Re1.5/PSX.EXE',  REPO + 'info/Re1.5/PSX/EMD/'),
}
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]

def toc(exe, a):
    o = a - 0x80010000 + 0x800
    return [struct.unpack_from('<II', exe, o + 8*i) for i in range(64)]

def kette(a):
    """Jeder Sektor, dessen Kopf wie ein EMD aussieht: dir_off >= 36, 4-Byte-ausgerichtet, dir_off+36 passt, n==9 oder 8."""
    out = []
    s = 0
    n = len(a)
    while s*2048 + 8 <= n:
        o = s*2048
        d, c = u32(a, o), u32(a, o+4)
        if d >= 36 and not (d & 3) and o + d + 36 <= n and c in (8, 9) and o+d+c*4 <= n:
            # Verzeichnis plausibel?
            e = [u32(a, o+d+i*4) for i in range(c)]
            if e == sorted(e) and e[0] == 8:
                ln = d + c*4
                out.append((s, ln, c))
                s += (ln + 2047) // 2048
                continue
        s += 1
    return out

for name, (exe_p, emd_p) in QUELLEN.items():
    exe = open(exe_p, 'rb').read()
    print('=====', name, exe_p, hashlib.md5(exe).hexdigest())
    for arch, adr in (('CDEMD0.EMS', 0x80072f38), ('CDEMD1.EMS', 0x80073178)):
        a = open(emd_p + arch, 'rb').read()
        t = toc(exe, adr)
        print('---', arch, len(a), 'B', hashlib.md5(a).hexdigest(), 'Sektoren', len(a)//2048)
        tm = {}
        for i, (s, l) in enumerate(t):
            if l:
                tm.setdefault(s, []).append((0x10+i, l))
        k = kette(a)
        ks = {s for s, _, _ in k}
        for s, ln, c in k:
            typen = tm.get(s, [])
            b = a[s*2048:s*2048+ln]
            print(f'  Sektor {s:5d} @0x{s*2048:07X} Laenge {ln:7d} n={c} md5 {hashlib.md5(b).hexdigest()[:10]} ' +
                  ('TOC ' + ' '.join(f'0x{t_:02X}({l_})' for t_, l_ in typen) if typen else '-- OHNE TOC-EINTRAG'))
        for s, lst in sorted(tm.items()):
            if s not in ks:
                print(f'  TOC zeigt auf Sektor {s} ohne erkannten Blobkopf: {lst}')
