# Abnahme 3 Spur W: eigener linearer SCD-Durchlauf (Laengen = s_opcode_sizes aus re15_port/engine/src/scd_vm.c:242ff,
# 0x2D = 34 wie op_obj_model_set). Gibt je Programm (main/sub) jede Zeile aus und am Ende einen Zensus der
# Start-Opcodes 0x04 Evt_exec, 0x18 Gosub, 0x2C Aot_set (sce + Daten), 0x3B Door_aot_set, 0x46 Aot_reset, 0x47 Aot_on,
# 0x3A Sce_espr_on (Effekt/Sub).
import sys, struct
SZ = {0x00:1,0x01:2,0x02:1,0x03:4,0x04:4,0x05:2,0x06:4,0x07:4,0x08:2,0x09:4,0x0A:3,0x0B:1,0x0C:1,0x0D:6,0x0E:2,0x0F:4,
0x10:2,0x11:4,0x12:2,0x13:4,0x14:6,0x15:4,0x16:2,0x17:6,0x18:2,0x19:2,0x1A:2,0x1B:6,0x1C:1,0x1D:1,0x1E:1,0x1F:1,0x20:1,
0x21:4,0x22:4,0x23:6,0x24:4,0x25:3,0x26:6,0x27:4,0x28:1,0x29:2,0x2A:1,0x2B:4,0x2C:20,0x2D:34,0x2E:3,0x2F:4,
0x30:1,0x31:1,0x32:8,0x33:8,0x34:4,0x35:3,0x36:12,0x37:4,0x38:12,0x39:4,0x3A:16,0x3B:32,0x3C:2,0x3D:3,0x3E:6,0x3F:4,
0x40:8,0x41:10,0x42:1,0x43:4,0x44:20,0x45:3,0x46:10,0x47:2,0x48:16,0x49:8,0x4A:2,0x4B:3,0x4C:18,0x4D:10,0x4E:5,0x4F:22,
0x50:22,0x51:4,0x52:4,0x53:3,0x54:6,0x55:6,0x56:6,0x57:4,0x58:4,0x59:4,0x5A:6,0x5B:4,0x5C:4,0x5D:4,0x5E:4}
NAME = {0x01:'Evt_end',0x02:'Evt_next',0x03:'Evt_chain',0x04:'Evt_exec',0x05:'Evt_kill',0x06:'Ifel_ck',0x07:'Else_ck',0x08:'Endif',
0x09:'Sleep',0x0A:'Sleeping',0x0D:'For',0x0E:'Next',0x0F:'While',0x10:'Ewhile',0x11:'Do',0x12:'Edwhile',0x13:'Switch',0x14:'Case',
0x15:'Default',0x16:'Eswitch',0x17:'Goto',0x18:'Gosub',0x19:'Return',0x1A:'Break',0x21:'Ck',0x22:'Set',0x23:'Cmp',0x24:'Save',
0x29:'Cut_chg',0x2B:'Message_on',0x2C:'Aot_set',0x2D:'Obj_model_set',0x2E:'Work_set',0x32:'Pos_set',0x33:'Dir_set',0x36:'Se_on',
0x3A:'Sce_espr_on',0x3B:'Door_aot_set',0x46:'Aot_reset',0x47:'Aot_on',0x00:'Nop'}

def u16(d,o): return struct.unpack_from('<H',d,o)[0]
def u32(d,o): return struct.unpack_from('<I',d,o)[0]
def s16(d,o): return struct.unpack_from('<h',d,o)[0]

def tables(d):
    ms, ss, nxt = u32(d,0x40), u32(d,0x44), u32(d,0x48)
    progs = []
    n = u16(d,ms)//2
    for i in range(n): progs.append(('main%02d'%i, ms+u16(d,ms+2*i)))
    n = u16(d,ss)//2
    for i in range(n): progs.append(('sub%02d'%i, ss+u16(d,ss+2*i)))
    return ms, ss, nxt, progs

def walk(d, start, end):
    pc = start; out = []
    while pc < end:
        op = d[pc]
        n = SZ.get(op)
        if n is None:
            out.append((pc, op, d[pc:pc+1], 'UNBEKANNT')); pc += 1; continue
        out.append((pc, op, d[pc:pc+n], NAME.get(op,'op%02X'%op)))
        pc += n
    return out

def main():
    f = sys.argv[1]; d = open(f,'rb').read()
    voll = len(sys.argv) > 2 and sys.argv[2] == 'voll'
    ms, ss, nxt, progs = tables(d)
    print('Datei %s Groesse %d  main_scd @0x%05X  sub_scd @0x%05X  naechste Sektion @0x%05X' % (f, len(d), ms, ss, nxt))
    starts = sorted(set([o for _,o in progs] + [ss])) + [nxt]
    zensus = []
    for name, o in progs:
        # Ende = naechster Programmstart > o (bzw. Sektionsgrenze)
        e = min(x for x in starts if x > o) if any(x > o for x in starts) else nxt
        lines = walk(d, o, e)
        print('--- %s @0x%05X .. @0x%05X (%d Bytes, %d Zeilen)' % (name, o, e, e-o, len(lines)))
        for pc, op, b, nm in lines:
            hexs = ' '.join('%02x'%x for x in b)
            extra = ''
            if op == 0x04: extra = 'cond 0x%02X sub%02d' % (b[1], b[3])
            elif op == 0x18: extra = 'sub%02d' % b[1]
            elif op == 0x3A: extra = 'Effekt %d Sub %d skala 0x%04X x %d y %d z %d' % (b[2], b[3], u16(b,6), s16(b,8), s16(b,10), s16(b,12))
            elif op == 0x2C: extra = 'id %d sce %d sat 0x%02X ... daten %s' % (b[1], b[2], b[3], ' '.join('%02x'%x for x in b[14:20]))
            elif op == 0x46: extra = 'Aot_reset id %d sce %d daten %s' % (b[1], b[2], ' '.join('%02x'%x for x in b[3:10]))
            elif op == 0x47: extra = 'Aot_on id %d' % b[1]
            if voll or op in (0x01,0x04,0x18,0x2C,0x3A,0x3B,0x46,0x47,0x17,0x0D) or nm == 'UNBEKANNT' or op >= 0x5F:
                print('  @0x%05X  %-13s %-50s %s' % (pc, nm, hexs[:50], extra))
            if op in (0x04,0x18,0x2C,0x46,0x47) or (op == 0x3A and b[2] == 6):
                zensus.append((name, pc, nm, extra))
    print('=== Zensus Start-/Tropfen-Zeilen')
    for z in zensus: print('  %-7s @0x%05X %-12s %s' % z)

main()
