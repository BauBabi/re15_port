# Abnahme 2 G39: eigener SCD-Walker (Laengen = s_opcode_sizes / scd_opcode_size_at aus scd_vm.c) fuer ROOM1210/1211.
import struct, sys
P = r'C:/workspace/git/reAi_v2/info/Re1.5/PSX/STAGE1/'
L = {0x00:1,0x01:2,0x02:1,0x03:4,0x04:4,0x05:2,0x06:4,0x07:4,0x08:2,0x09:4,0x0A:3,0x0B:1,0x0C:1,0x0D:6,0x0E:2,0x0F:4,
     0x10:2,0x11:4,0x12:2,0x13:4,0x14:6,0x15:4,0x16:2,0x17:6,0x18:2,0x19:2,0x1A:2,0x1B:6,0x1C:1,0x1D:1,0x1E:1,0x20:1,
     0x21:4,0x22:4,0x23:6,0x24:4,0x25:3,0x26:6,0x27:4,0x28:1,0x29:2,0x2A:1,0x2B:4,0x2E:3,0x2F:4,0x30:1,0x31:1,0x32:8,
     0x33:8,0x34:4,0x35:3,0x36:12,0x37:4,0x38:12,0x39:4,0x3A:16,0x3C:2,0x3D:3,0x3E:6,0x3F:4,0x40:8,0x41:10,0x42:1,
     0x43:4,0x44:20,0x45:3,0x46:10,0x47:2,0x48:16,0x49:8,0x4A:2,0x4B:3,0x4C:18,0x4D:10,0x4E:5,0x4F:22,0x51:4,0x52:4,
     0x53:3,0x54:6,0x55:6,0x56:6,0x57:4,0x58:4,0x59:4,0x5A:6,0x5B:4,0x5C:4,0x5D:4,0x5E:4}
def size(b, o):
    op = b[o]
    if op == 0x2C: return 28 if b[o+3] & 0x80 else 20
    if op == 0x3B: return 40 if b[o+3] & 0x80 else 32
    if op == 0x50: return 30 if b[o+3] & 0x80 else 22
    if op == 0x2D: return 34
    return L.get(op, -1)
def walk(b, start, end, name, zeige):
    o = start
    print(f'  {name} @0x{start:05X}..0x{end:05X}')
    while o < end:
        n = size(b, o)
        if n < 0: print(f'   @0x{o:05X} ?? op {b[o]:02x} - Abbruch'); return
        if zeige == "alle" or b[o] in zeige:
            print(f'   @0x{o:05X} op {b[o]:02x} len {n:2d}: ' + ' '.join(f'{x:02x}' for x in b[o:o+n]))
        if b[o] == 0x01 and False: pass
        o += n
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
if __name__ == "__main__":
  room = sys.argv[1]; zeige = sys.argv[2] if len(sys.argv) > 2 else 'alle'
  if zeige != 'alle': zeige = set(int(x, 16) for x in zeige.split(','))
  b = open(P + room + '.RDT', 'rb').read()
  main, sub, ext = u32(b, 0x40), u32(b, 0x44), u32(b, 0x48)
  for (nm, base, nxt) in (('main', main, sub), ('sub', sub, ext if ext > sub else len(b))):
      first = u16(b, base); n = first // 2
      offs = [u16(b, base + 2*i) for i in range(n)]
      print(f'{room} {nm} @0x{base:05X}: {n} Funktionen, Tabelle {offs[:16]}')
      for i, of in enumerate(offs):
          st = base + of
          en = base + offs[i+1] if i + 1 < n else nxt
          walk(b, st, en, f'{nm}{i:02d}', zeige)
