#!/usr/bin/env python3
"""Runde 37 Spur N — Zensus CDEMD0 (Leon) gegen CDEMD1 (Elza) je Typ und dir-Eintrag.

Quelle der Blob-Lage = die TOCs der EXE (FUN_80022300: Leon @0x80072f38, Elza @0x80073178,
Index Typ-0x10 @0x80022338, 8 B je Eintrag `sll v1,a0,3` @0x80022344/@0x800223b4).
Zusaetzlich: liefert die Blobkette des Ports (re15_ems.c, s_ems_order) fuer CDEMD0 denselben
Blob wie die TOC? (= ist der Leon-Weg des Ports der Original-Weg)

Aufruf: python n1_zensus.py [--baum <worktree>]
"""
import struct, sys, os, hashlib

BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
for i, a in enumerate(sys.argv):
    if a == '--baum': BAUM = sys.argv[i + 1]
EXE = open(os.path.join(BAUM, 'info/Re1.5/PSX.EXE'), 'rb').read()
T = struct.unpack_from('<I', EXE, 0x18)[0]
def rd(a, n): o = 0x800 + a - T; return EXE[o:o + n]
EMD = os.path.join(BAUM, 're15_port/shared_assets/PSX/EMD/')
E = {0: open(EMD + 'CDEMD0.EMS', 'rb').read(), 1: open(EMD + 'CDEMD1.EMS', 'rb').read()}
TOC = {0: 0x80072f38, 1: 0x80073178}
BYT = {0: 0x80073138, 1: 0x80073378}
ORDER = [0x10, 0x11, 0x12, 0x13, 0x16, 0x18, 0x1A, 0x20, 0x21, 0x24, 0x25, 0x26, 0x27, 0x29, 0x2B,
         0x2D, 0x30, 0x36, 0x40, 0x42, 0x45, 0x47, 0x49, 0x4B, 0x4D]   # re15_ems.c s_ems_order
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]

def toc(a, typ):
    s, z = struct.unpack_from('<II', rd(TOC[a] + (typ - 0x10) * 8, 8))
    return s, z

def blob(a, typ):
    s, z = toc(a, typ)
    return E[a][s * 2048: s * 2048 + z] if z else None

def walk(ems, index):                                   # re15_ems_get_entry
    cur = i = 0; n = len(ems)
    while cur + 4 <= n:
        do = u32(ems, cur)
        if do < 36 or (do & 3): break
        ln = do + 36
        if cur + ln > n: break
        if i == index: return cur, ln
        i += 1
        nx = cur + ln
        if nx >= n: break
        al = (nx + 2047) & ~2047
        if al <= cur: break
        cur = al; sk = 0
        while cur + 4 <= n and u32(ems, cur) == 0: cur += 4; sk += 4
        if sk: cur = (cur + 2047) & ~2047
    return None

def dirs(b):
    do = u32(b, 0); cnt = u32(b, 4)
    return [u32(b, do + 4 * k) for k in range(cnt)], do

def sect(b, d, k, do):
    st = d[k]
    nxt = [x for x in d if x > st] + [do]
    return b[st:min(nxt)]

def edd(b, o):
    n = u16(b, o + 2) // 4
    return [u16(b, o + j * 4) for j in range(n)]

def emr_head(b, o):
    return (u16(b, o), u16(b, o + 2), u16(b, o + 4), u16(b, o + 6))

NAMES = ['d0', 'edd1(loco)', 'emr2(struktur+loco)', 'edd3(eigene)', 'emr4(eigene)', 'edd5(opfer)',
         'emr6(opfer)', 'md1', 'tim']

def md1_meshes(b, o):
    return u32(b, o + 4) if o + 8 <= len(b) else -1    # u32 Laenge, u32 ?, ... (nur Kennzahl)

out = []
for i in range(64):
    typ = i + 0x10
    a = blob(0, typ); b = blob(1, typ)
    if not a and not b: continue
    s0, z0 = toc(0, typ); s1, z1 = toc(1, typ)
    b0 = rd(BYT[0] + i, 1)[0]; b1 = rd(BYT[1] + i, 1)[0]
    line = 'Typ 0x%02X  CDEMD0 Sektor %4d %6d B byte %02x | CDEMD1 Sektor %4d %6d B byte %02x' % (
        typ, s0, z0, b0, s1, z1, b1)
    # Port-Kette (Leon-Weg) gegen die TOC
    if typ in ORDER:
        for arch in (0, 1):
            w = walk(E[arch], ORDER.index(typ))
            tb = blob(arch, typ)
            if w and tb:
                off, ln = w
                same = (off == toc(arch, typ)[0] * 2048) and E[arch][off:off + ln] == tb[:ln]
                line += '\n    Port-Kette %s idx %2d @0x%06x %6d B == TOC-Blob[:%d]: %s' % (
                    ('CDEMD0', 'CDEMD1')[arch], ORDER.index(typ), off, ln, ln, same)
    else:
        line += '\n    (Typ nicht in s_ems_order)'
    if a and b and a == b:
        line += '\n    BLOB IDENTISCH (md5 %s)' % hashlib.md5(a).hexdigest()[:8]
        out.append(line); continue
    if not a or not b:
        line += '\n    nur in %s' % ('CDEMD1' if b else 'CDEMD0')
        out.append(line); continue
    da, doa = dirs(a); db, dob = dirs(b)
    line += '\n    dir-Zahl %d / %d' % (len(da), len(db))
    for k in range(min(len(da), len(db))):
        pa = sect(a, da, k, doa); pb = sect(b, db, k, dob)
        nm = NAMES[k] if k < len(NAMES) else 'd%d' % k
        if pa == pb:
            line += '\n    %-22s gleich (%d B)' % (nm, len(pa)); continue
        x = '\n    %-22s VERSCHIEDEN %d / %d B' % (nm, len(pa), len(pb))
        if k in (1, 3, 5) and len(pa) >= 4 and len(pb) >= 4:
            ea = edd(a, da[k]); eb = edd(b, db[k])
            x += '  Clips %d / %d' % (len(ea), len(eb))
            dif = [(j, ea[j] if j < len(ea) else None, eb[j] if j < len(eb) else None)
                   for j in range(max(len(ea), len(eb)))
                   if (ea[j] if j < len(ea) else None) != (eb[j] if j < len(eb) else None)]
            x += '  Bilderzahl-Abweichung %s' % (dif if dif else 'keine (nur Keyframe-Zuordnung/Flags)')
        if k in (2, 4, 6) and len(pa) >= 8 and len(pb) >= 8:
            ha = emr_head(a, da[k]); hb = emr_head(b, db[k])
            x += '  Kopf %s / %s' % (ha, hb)
            if ha == hb and k == 2:
                nb = ha[2]
                rpa = a[da[k] + 8: da[k] + 8 + nb * 6]; rpb = b[db[k] + 8: db[k] + 8 + nb * 6]
                hia = a[da[k] + ha[0]: da[k] + ha[1]]; hib = b[db[k] + hb[0]: db[k] + hb[1]]
                x += '  Bind-Lagen %s, Hierarchie %s' % ('gleich' if rpa == rpb else 'VERSCHIEDEN',
                                                      'gleich' if hia == hib else 'VERSCHIEDEN')
        line += x
    out.append(line)
print('\n'.join(out))
