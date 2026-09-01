# -*- coding: utf-8 -*-
"""Unabhaengiger RDT-Walker fuer ROOM1170 — zwei Groessentabellen im Vergleich."""
import struct, os, sys, json
ROOT = r'C:\workspace\git\reAi_v2'
NAMES = ["collision","camera","zone","light","md1ptr","floor","block","message",
         "mainScd","subScd","extraScd","effect","x50","espTim","modelTim","anim"]

# (A) Tabelle des GENERATORS (gen_map_zones.py:192)
SZ_GEN = {0x00:1,0x01:2,0x02:1,0x03:4,0x04:4,0x05:2,0x06:4,0x07:4,0x08:2,0x09:4,0x0A:3,0x0B:1,
0x0C:1,0x0D:6,0x0E:2,0x0F:4,0x10:2,0x11:4,0x12:2,0x13:4,0x14:6,0x15:4,0x16:2,0x17:6,0x18:2,
0x19:2,0x1A:2,0x1B:6,0x1C:1,0x1D:1,0x1E:1,0x20:1,0x21:4,0x22:4,0x23:6,0x24:4,0x25:3,0x26:6,
0x27:4,0x28:1,0x29:2,0x2A:1,0x2B:4,0x2C:20,0x2D:34,0x2E:3,0x2F:4,0x30:1,0x31:1,0x32:8,0x33:8,
0x34:4,0x35:3,0x36:12,0x37:4,0x38:3,0x39:4,0x3A:16,0x3B:32,0x3C:2,0x3D:4,0x3E:6,0x3F:4,0x40:8,
0x41:10,0x42:1,0x43:4,0x44:20,0x45:3,0x46:10,0x47:2,0x48:16,0x49:8,0x4A:2,0x4B:3,0x4C:5,0x4D:22,
0x4E:22,0x4F:4,0x50:22,0x51:4,0x52:4,0x53:3,0x54:6,0x55:6,0x56:6,0x57:4,0x58:4,0x59:4,0x5A:6,
0x5B:4,0x5C:4,0x5D:4,0x5E:4,0x5F:2,0x60:14,0x61:4,0x62:2,0x63:1,0x64:16,0x65:2,0x66:1,0x67:28,
0x68:40,0x69:30,0x6A:6,0x6B:4,0x6C:1,0x6D:4,0x6E:6,0x6F:2,0x70:1,0x71:1,0x72:16,0x73:8,0x74:4,
0x75:22,0x76:3,0x77:4,0x78:6,0x79:1,0x7A:16,0x7B:16,0x7C:6,0x7D:6,0x7E:6,0x7F:6,0x80:2,0x81:3,
0x82:3,0x83:1,0x84:2,0x85:6,0x86:1,0x87:1,0x88:3,0x89:1,0x8A:6,0x8B:6,0x8C:1,0x8D:24,0x8E:2}

# (B) Tabelle der ENGINE (scd_vm.c:166, disasm-verifiziert gegen PTR_LAB_800744a8)
SZ_ENG = dict(SZ_GEN)
for k, v in {0x2D:34, 0x38:12, 0x3D:3, 0x4C:18, 0x4D:10, 0x4E:5, 0x4F:22}.items():
    SZ_ENG[k] = v
for k in list(SZ_ENG):
    if k > 0x5E: del SZ_ENG[k]      # RE1.5-Dispatch endet bei 0x5E
SZ_ENG[0x68] = 40                    # nur zum Erkennen, kommt in RE1.5 nicht vor

def load(rid):
    p = os.path.join(ROOT,'re15_port','shared_assets','PSX',f'STAGE{rid>>12}',f'ROOM{rid:04X}.RDT')
    d = open(p,'rb').read()
    offs = {nm: struct.unpack_from('<I', d, 0x20+4*i)[0] for i,nm in enumerate(NAMES)}
    return d, offs

def walk(d, offs, SZ, label):
    res = {'doors':[], 'stairs':[], 'rearm':[], 'desync':[]}
    for name in ('mainScd','subScd'):
        s = offs[name]
        ends = sorted(v for v in offs.values() if v > s) + [len(d)]
        e = ends[0]
        if s == 0 or e <= s: continue
        n0 = struct.unpack_from('<H', d, s)[0]
        nsub = n0//2
        ptrs = [struct.unpack_from('<H', d, s+2*i)[0] for i in range(nsub)]
        for si, pp in enumerate(ptrs):
            end = ptrs[si+1] if si+1 < nsub else (e-s)
            end = min(end, e-s)
            pc = pp
            while pc < end:
                op = d[s+pc]
                sz = SZ.get(op)
                if sz is None:
                    res['desync'].append((name,si,hex(s+pc),hex(op))); break
                b = d[s+pc:s+pc+sz]
                if op == 0x46 and len(b) >= 3:
                    res['rearm'].append(dict(off=s+pc, sub=si, slot=b[1], sce=b[2]))
                if op == 0x2c and len(b) == 20 and b[2] in (12,13):
                    rx,rz,rw,rd = struct.unpack_from('<hhhh', b, 6)
                    res['stairs'].append(dict(off=s+pc, scd=name, sub=si, slot=b[1],
                        axis=b[2], band=b[4], count=b[16],
                        rx=rx, rz=rz, rw=rw, rd=rd, x=rx+rw//2, z=rz+rd//2,
                        raw=b.hex()))
                if op == 0x3b and len(b) == 32:
                    rx,rz,rw,rd = struct.unpack_from('<hhhh', b, 6)
                    nx,ny,nz = struct.unpack_from('<hhh', b, 14)
                    res['doors'].append(dict(off=s+pc, scd=name, sub=si, slot=b[1], sce=b[2],
                        rx=rx, rz=rz, rw=rw, rd=rd, lx=rx+rw//2, lz=rz+rd//2,
                        band=b[4], nx=nx, ny=ny, nz=nz, stg=b[22], rmd=b[23],
                        dest=((b[22]+1)<<12)|(b[23]<<4), raw=b.hex()))
                pc += sz
    return res

d, offs = load(0x1170)
print('SEKTIONEN ROOM1170.RDT:')
for nm in NAMES: print('   %-10s 0x%06X' % (nm, offs[nm]))
print('   Dateigroesse 0x%06X' % len(d))
A = walk(d, offs, SZ_GEN, 'GEN')
B = walk(d, offs, SZ_ENG, 'ENG')
for lbl, R in (('GEN', A), ('ENG', B)):
    print('\n===== Tabelle %s =====' % lbl)
    print(' desync:', R['desync'])
    print(' rearm :', R['rearm'])
    print(' TUEREN (Opcode 0x3b, 32 B):')
    for x in R['doors']:
        print('   off=0x%06X scd=%-7s sub=%d slot=%2d sce=%2d band=%d  rect=(%6d,%6d,%5d,%5d) mitte=(%6d,%6d) ziel=(%6d,%6d,%6d) dest=ROOM%04X'
              % (x['off'],x['scd'],x['sub'],x['slot'],x['sce'],x['band'],x['rx'],x['rz'],x['rw'],x['rd'],x['lx'],x['lz'],x['nx'],x['ny'],x['nz'],x['dest']))
    print(' TREPPEN (Opcode 0x2c, sce 12/13):')
    for x in R['stairs']:
        print('   off=0x%06X scd=%-7s sub=%d slot=%2d achse=%d band=%d count=%d rect=(%6d,%6d,%5d,%5d) mitte=(%6d,%6d)'
              % (x['off'],x['scd'],x['sub'],x['slot'],x['axis'],x['band'],x['count'],x['rx'],x['rz'],x['rw'],x['rd'],x['x'],x['z']))
print('\nGLEICH? Tueren:', [ (a['off'],a['band']) for a in A['doors']] == [ (b['off'],b['band']) for b in B['doors']],
      ' Treppen:', [ (a['off'],a['band']) for a in A['stairs']] == [ (b['off'],b['band']) for b in B['stairs']])
json.dump({'gen':A,'eng':B}, open(os.path.join(ROOT,'analysis','karte_0901','_A1_1170.json'),'w'), indent=1)
