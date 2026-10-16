# Runde 37 Spur G — Aufruf: python esp_dump.py re15_port/shared_assets/PSX/DATA/CORE00.ESP 2,3,4 0,2
# CORE00.ESP Row-Dump (Layout wie re15_esp.c esp_parse_core/esp_rowblk/re15_esp_row_stream)
import struct, sys
p = sys.argv[1]; want = [int(x) for x in sys.argv[2].split(',')]; subs = [int(x) for x in sys.argv[3].split(',')]
raw = open(p,'rb').read(); size = len(raw)
ptr_end = ((size + 3) & ~3) - 4
eff = {}
for i in range(64):
    idv = raw[i]
    if idv == 0xFF: break
    ent = struct.unpack_from('<i', raw, ptr_end - i*4)[0]
    st = ent
    w0 = struct.unpack_from('<I', raw, st)[0]
    ca, cb = w0 & 0xffff, w0 >> 16
    end = st + (ca*2 + cb + 2)*4
    eff[idv] = (st, end, ca, cb)
print('ids', {k:(hex(v[0]),hex(v[1]),v[2],v[3]) for k,v in eff.items()})
for e in want:
    st, end, ca, cb = eff[e]
    blk = end
    subtab = struct.unpack_from('<8H', raw, blk)
    print(f'effect {e}: rowblk @0x{blk:x} subtab {subtab}')
    for s in subs:
        base = blk + subtab[s & 7]*4
        streams = struct.unpack_from('<H', raw, base)[0]
        print(f'  sub {s}: base @0x{base:x} streams {streams}')
        q = base + 4
        for si in range(streams):
            nr = struct.unpack_from('<H', raw, q)[0]
            print(f'    stream {si} @0x{q:x} nrows {nr}')
            for r in range(nr):
                ro = q + 4 + r*40
                row = raw[ro:ro+40]
                A,B,w,h,ax,ay,az,p0e,vx,vy,vz,p16,avx,avy,avz,p1e,ex,ey,ez,p26 = struct.unpack('<HHhhhhhHhhhHhhhHhhhH', row)
                print(f'      row {r} @0x{ro:x}: A={A} B={B} wh=({w},{h}) acc=({ax},{ay},{az}) p0e=0x{p0e:04x} vel=({vx},{vy},{vz}) p16=0x{p16:04x} angv=({avx},{avy},{avz}) p1e=0x{p1e:04x} eul=({ex},{ey},{ez}) adv=0x{p26:04x}')
            q += 4 + nr*40
