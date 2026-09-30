#!/usr/bin/env python3
"""Spur B: Einstieg + Ausstieg der RE1.5-Cursor-Raetsel (Raeume mit D-Pad-Sce_key_ck + Typ-4-Cursor).
Je Raum: die Subs, die Bank5-Bit0 SETZEN (Einstieg) bzw. LOESCHEN (Ausstieg), und ob im Einstieg vor dem
Set ein Ck(12,31,0) (= 'Ja' der Auswahlbox, vgl. ROOM11F0 sub16 @0x015B2) und ein Message_on steht; dazu
welche Sub von welcher Zelle (Member_cmp-Wert) per Aktion 0x0040 gerufen wird."""
import os, glob, sys
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
src = open(os.path.join(REPO, "re15_port/tools/scd_dump_room.py"), encoding="utf-8").read().replace("\nmain()\n", "\n")
ns = {}; exec(compile(src, "scd_dump_room.py", "exec"), ns)
op_size, rdt_section_end, section_regions, u32 = (ns[k] for k in ("op_size", "rdt_section_end", "section_regions", "u32"))

def subs(d):
    out = {}
    for sec, tag in ((u32(d, 0x40), "main"), (u32(d, 0x44), "sub")):
        if sec == 0 or sec >= len(d): continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            ops = []; pc = sec + o; end = sec + e
            while pc < end:
                sz = op_size(d, pc)
                if sz is None or pc + sz > end: break
                ops.append((pc, d[pc:pc+sz]))
                if d[pc] == 0x01 and pc + 2 >= end: break
                pc += sz
            out[f"{tag}{idx:02d}"] = ops
    return out

for p in sorted(glob.glob(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x60: continue
    S = subs(d)
    has_dpad = any(b[0] == 0x51 and b[2] in (1, 2, 4, 8) for ops in S.values() for _, b in ops)
    has_c4 = any(b[0] == 0x2D and b[2] == 4 for ops in S.values() for _, b in ops)
    if not (has_dpad and has_c4): continue
    print(f"== {os.path.basename(p)}")
    for name, ops in S.items():
        sets = [(pc, b[3]) for pc, b in ops if b[0] == 0x22 and b[1] == 5 and b[2] == 0]
        if not sets: continue
        for pc, val in sets:
            vor = [(q, b) for q, b in ops if q < pc]
            ja = [q for q, b in vor if b[0] == 0x21 and b[1] == 12 and b[2] == 31 and b[3] == 0]
            msg = [f"msg{b[1]}@0x{q:05X}" for q, b in vor if b[0] == 0x2B]
            cut = [f"Cut{b[1]}@0x{q:05X}" for q, b in ops if b[0] == 0x29]
            print(f"   {name} Set(5,0,{val}) @0x{pc:05X}  Ja-Pruefung Ck(12,31,0): {['@0x%05X'%q for q in ja] or '-'}  "
                  f"Texte davor: {msg or '-'}  Cuts im Sub: {cut or '-'}")
    # Zellen -> Aktion -> Sub
    for name, ops in S.items():
        for i, (pc, b) in enumerate(ops):
            if b[0] == 0x3E:
                wert = b[4] | (b[5] << 8)
                nach = ops[i+1:i+4]
                ex = [bb for _, bb in nach if bb[0] == 0x04]
                key = [bb for _, bb in nach if bb[0] == 0x51]
                if key and ex:
                    print(f"   {name} Member_cmp(15=={wert}) @0x{pc:05X} + Sce_key_ck(0x{key[0][2]|(key[0][3]<<8):04X}) -> Evt_exec sub{ex[0][3]}")
