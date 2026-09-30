#!/usr/bin/env python3
"""Spur B (Runde 34 Nacht): Zensus der WELT-CURSOR-Raetsel ueber ALLE RE1.5-RDTs.

Opcode-exakter Walk (Laengen/Regionen aus re15_port/tools/scd_dump_room.py, dort aus
scd_vm.c s_opcode_sizes + disasm-verifizierte variable Laengen). Je Raum:
  * jede Set/Ck auf Bank 5 Bit 0 (ROOM11F0: Cursor-Modus-Bit, sub01 @0x01090 Ck(5,0,1),
    sub16 @0x015C2 Set(5,0,1), sub17 @0x0168C / sub18 @0x016FA Set(5,0,0))
  * alle Sce_key_ck (0x51) mit Maske (0x01/0x02/0x04/0x08 = D-Pad, 0x40 = Aktion, sonst ANDERE)
  * Aot_set mit sce 5 und Objekt-Pool-Bit 0x04 (Raster-Zellen, flags 0x44 in 11F0)
  * Obj_model_set mit type 4 (11F0-Cursor @0x00E54)
  * Set(2,0)/Set(2,2) (Pause Spieler/KI, game_state.c)
Aufruf: cursor_zensus.py [--nur-treffer]
"""
import os, sys, glob, importlib.util

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
spec = importlib.util.spec_from_file_location("sdr", os.path.join(REPO, "re15_port/tools/scd_dump_room.py"))
src = open(os.path.join(REPO, "re15_port/tools/scd_dump_room.py"), encoding="utf-8").read()
src = src.replace("\nmain()\n", "\n")            # nur die Funktionen, kein Lauf
ns = {}
exec(compile(src, "scd_dump_room.py", "exec"), ns)
op_size, rdt_section_end, section_regions, u16, u32, s16 = (ns[k] for k in
    ("op_size", "rdt_section_end", "section_regions", "u16", "u32", "s16"))

def walk(d):
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d): continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            pc = sec + o; end = sec + e
            while pc < end:
                sz = op_size(d, pc)
                if sz is None or pc + sz > end: break
                yield f"{tag}{idx:02d}", pc, d[pc:pc+sz]
                if d[pc] == 0x01 and pc + 2 >= end: break
                pc += sz

def main():
    nur = "--nur-treffer" in sys.argv
    rdts = sorted(glob.glob(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT")))
    gesamt = 0
    for p in rdts:
        d = open(p, "rb").read()
        if len(d) < 0x60: continue
        gesamt += 1
        zeilen = []
        keys = {}
        zellen = 0; typ4 = []; b50 = []; pause = []
        for sub, pc, b in walk(d):
            op = b[0]
            if op in (0x21, 0x22) and b[1] == 5 and b[2] == 0:
                b50.append(f"{sub}@0x{pc:05X} {'Ck' if op == 0x21 else 'Set'}(5,0,{b[3]})")
            if op == 0x22 and b[1] == 2 and b[2] in (0, 2):
                pause.append(f"{sub}@0x{pc:05X} Set(2,{b[2]},{b[3]})")
            if op == 0x51:
                m = b[2] | (b[3] << 8)
                keys.setdefault(m, []).append(f"{sub}@0x{pc:05X}")
            if op == 0x2C and b[2] == 5 and (b[3] & 0x04):
                zellen += 1
            if op == 0x2D and b[2] == 4:
                typ4.append(f"{sub}@0x{pc:05X} obj={b[1]} pos=({s16(b,10)},{s16(b,12)},{s16(b,14)}) box_h=({s16(b,28)},{s16(b,30)},{s16(b,32)})")
        if nur and not (b50 or keys or typ4):
            continue
        print(f"== {os.path.basename(p)}")
        if b50: print("   Bank5 Bit0 : " + "; ".join(b50))
        if pause: print(f"   Pause(2,0/2): {len(pause)}x  " + "; ".join(pause[:6]) + (" ..." if len(pause) > 6 else ""))
        for m in sorted(keys):
            art = {1: "UP", 2: "RIGHT", 4: "DOWN", 8: "LEFT", 0x40: "AKTION"}.get(m, "ANDERE")
            print(f"   Sce_key_ck 0x{m:04X} ({art}) x{len(keys[m])}: " + ", ".join(keys[m][:4]) + (" ..." if len(keys[m]) > 4 else ""))
        if zellen: print(f"   Raster-Zellen (Aot_set sce5 + Objekt-Pool-Bit): {zellen}")
        for t in typ4: print(f"   Obj_model_set type4: {t}")
    print(f"# {gesamt} RDTs gelaufen")

main()
