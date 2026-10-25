# NB1: Spielstaende lesen: Modell/Waffe/HP/Status/Ausruest-Platz/Inventar + DEBUG.BIN-Residenz @0x800c00a8.
import sys, importlib.util, os
spec = importlib.util.spec_from_file_location("ss", r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts\re15_ss.py")
ss = importlib.util.module_from_spec(spec); spec.loader.exec_module(ss)
dbg = open(r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_stage3szenen_b\info\Re1.5\PSX\BIN\DEBUG.BIN", "rb").read()
for p in sys.argv[1:]:
    r = ss.Ram(p)
    inv = [(r.u8(0x800b10ac + 4 * i), r.u8(0x800b10ac + 4 * i + 1)) for i in range(8)]
    print("== %s" % os.path.basename(p))
    print("  stub %s aca5c %d aca5d %d HP(acaee) %d +0x98(acaec) %d 25c8 %d wv10 %d stage %d raum %02X cut %d" % (
        r.bytes(0x80026e4c, 4).hex(), r.u8(0x800aca5c), r.u8(0x800aca5d), r.s16(0x800acaee), r.u16(0x800acaec),
        r.u8(0x800b25c8), r.s16(0x800b0ff0), r.u16(0x800b0fe0), r.u16(0x800b0fe2), r.u16(0x800b0fe4)))
    print("  Inventar", inv)
    ram = r.bytes(0x800c00a8, 0x40)
    print("  RAM @0x800c00a8:", ram[:0x2c].hex(' '), "| DEBUG.BIN[0xa8]:", "gleich" if ram == dbg[0xa8:0xa8 + 0x40] else "VERSCHIEDEN")
