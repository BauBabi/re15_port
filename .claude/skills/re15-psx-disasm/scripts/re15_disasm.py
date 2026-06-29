#!/usr/bin/env python3
"""re15_disasm.py - direct MIPS R3000 disassembly + pointer-table decode of the RE1.5
PSX.EXE and the STAGE*.BIN overlays, WITHOUT a Ghidra setup ("Weg C").

This is the fast path for functions/tables that are NOT in the decompile dumps
(RE_15_Quellcode_V2/ = EXE, RE_15_Quellcode_Overlays/ = overlays). The session that
RE'd the zombie attack-action machinery hit exactly this: the generic-humanoid EXE
leaves (0x8004f..) were raw-byte in ghidra1_V2.txt, so they had to be disassembled
straight from PSX.EXE.

Address -> binary + file-offset (auto-detected; override with --bin):
  EXE     0x80010000..0x800effff : info/Re1.5/PSX.EXE  (PS-X EXE; text @ file 0x800,
          t_addr read from the header @0x18 — usually 0x80010000)
  OVERLAY 0x80100000..0x80121fff : info/Re1.5/PSX/BIN/<stage>.BIN, loaded RAW @0x80100000
          with NO 0x800 header -> file-off = addr - 0x80100000. Default stage = STAGE1.

Subcommands:
  dis   <addr> [n]   disassemble n instructions (default 48), annotating known globals/calls
  table <addr> [n]   decode n words (default 16) as a pointer table (tags EXE/overlay/data)
  scan  <addr>       summarise a function: sb writes to +0x5/+0x6/+0x4, jal targets,
                     sltiu/slti thresholds, lhu/lbu of known globals, up to the jr ra
  bytes <addr> [n]   raw hex dump of n bytes (default 64)

Examples:
  re15_disasm.py dis   0x8004f100 60          # the +0x5=0 zombie assess leaf (EXE)
  re15_disasm.py table 0x801217a0 24          # the overlay AI dispatch table (STAGE1.BIN)
  re15_disasm.py scan  0x8004f3a4             # what +0x5 transitions does this leaf make?
  re15_disasm.py dis   0x8011d6d4 30 --bin STAGE5.BIN
"""
import struct, sys, os, argparse

HERE = os.path.dirname(os.path.abspath(__file__))
# .../.claude/skills/re15-psx-disasm/scripts -> repo root is 5 levels up
REPO = os.path.abspath(os.path.join(HERE, "..", "..", "..", ".."))
EXE_PATH     = os.path.join(REPO, "info", "Re1.5", "PSX.EXE")
OVERLAY_DIR  = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN")
OVERLAY_LOAD = 0x80100000

REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]

# RE1.5-specific annotations (extend as new globals/funcs are confirmed).
GLOBALS = {
    0x800ac784:"g_entity(cur)", 0x800aca40:"g_pauseflags", 0x800aca54:"player",
    0x800aca88:"playerX", 0x800aca90:"playerZ", 0x800aca52:"g_flag52",
    0x800acad6:"player.floor(+0x82)", 0x800acae7:"player.hit_react(+0x93)",
    0x800acaee:"player.hp(+0x9a)", 0x800aca4e:"g_active_count", 0x800ac774:"g_rng_state",
    0x800acc2c:"enemy_array", 0x800a73b8:"model_inst_pool", 0x800b52c4:"attack_workstruct",
}
CALLS = {
    0x8001a6d4:"atan2_q12", 0x8001a780:"facing_aligned", 0x8001a9cc:"arc_test",
    0x8001ab9c:"arc_test(px,pz,cone)", 0x8001af20:"rng", 0x80065f60:"SquareRoot0",
    0x80012d60:"resolve_attack(dmg)", 0x8002b5d0:"hitbox_test", 0x80017fa4:"enemy_attack_dmg",
    0x8001c6e8:"room_coll", 0x8001f314:"anim_set", 0x800245d8:"pos_advance(walker)",
    0x80019e20:"action_driver", 0x80104178:"attack_point(GTE)", 0x800174e4:"lunge_finish",
}

def load(addr, binname):
    """Return (bytes, file_offset_fn) for the binary holding addr."""
    if addr >= OVERLAY_LOAD:
        path = os.path.join(OVERLAY_DIR, binname or "STAGE1.BIN")
        data = open(path, "rb").read()
        return data, (lambda a: a - OVERLAY_LOAD), path
    data = open(EXE_PATH, "rb").read()
    taddr = struct.unpack_from("<I", data, 0x18)[0]   # PS-X EXE t_addr
    return data, (lambda a: 0x800 + (a - taddr)), EXE_PATH

def word(data, fo, a): return struct.unpack_from("<I", data, fo(a))[0]

def dis_one(w, a):
    op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; sh=(w>>6)&31; fn=w&63
    imm=w&0xffff; simm=imm-0x10000 if imm&0x8000 else imm
    tgt=((a+4)&0xf0000000)|((w&0x3ffffff)<<2); R=REGS
    if w==0: return "nop", None
    if op==0:
        m={0x20:"add",0x21:"addu",0x22:"sub",0x23:"subu",0x24:"and",0x25:"or",0x26:"xor",
           0x27:"nor",0x2a:"slt",0x2b:"sltu"}
        if fn in m: return f"{m[fn]} {R[rd]},{R[rs]},{R[rt]}", None
        if fn==0:  return f"sll {R[rd]},{R[rt]},{sh}", None
        if fn==2:  return f"srl {R[rd]},{R[rt]},{sh}", None
        if fn==3:  return f"sra {R[rd]},{R[rt]},{sh}", None
        if fn==4:  return f"sllv {R[rd]},{R[rt]},{R[rs]}", None
        if fn==6:  return f"srlv {R[rd]},{R[rt]},{R[rs]}", None
        if fn==8:  return f"jr {R[rs]}", None
        if fn==9:  return f"jalr {R[rs]}", None
        if fn==0x10:return f"mfhi {R[rd]}", None
        if fn==0x12:return f"mflo {R[rd]}", None
        if fn==0x18:return f"mult {R[rs]},{R[rt]}", None
        if fn==0x19:return f"multu {R[rs]},{R[rt]}", None
        if fn==0x1a:return f"div {R[rs]},{R[rt]}", None
        if fn==0x1b:return f"divu {R[rs]},{R[rt]}", None
        return f".word 0x{w:08x}(fn{fn:02x})", None
    if op==2:  return f"j 0x{tgt:08x}", None
    if op==3:  return f"jal 0x{tgt:08x}", CALLS.get(tgt)
    if op==4:  return f"beq {R[rs]},{R[rt]},0x{a+4+simm*4:08x}", None
    if op==5:  return f"bne {R[rs]},{R[rt]},0x{a+4+simm*4:08x}", None
    if op==6:  return f"blez {R[rs]},0x{a+4+simm*4:08x}", None
    if op==7:  return f"bgtz {R[rs]},0x{a+4+simm*4:08x}", None
    if op==1:  return f"{'bltz' if rt==0 else 'bgez'} {R[rs]},0x{a+4+simm*4:08x}", None
    im={8:"addi",9:"addiu",0xa:"slti"}
    if op in im: return f"{im[op]} {R[rt]},{R[rs]},{simm}", None
    iz={0xb:"sltiu",0xc:"andi",0xd:"ori",0xe:"xori"}
    if op in iz: return f"{iz[op]} {R[rt]},{R[rs]},0x{imm:x}", None
    if op==0xf: return f"lui {R[rt]},0x{imm:x}", None
    ld={0x20:"lb",0x21:"lh",0x23:"lw",0x24:"lbu",0x25:"lhu",0x28:"sb",0x29:"sh",0x2b:"sw"}
    if op in ld: return f"{ld[op]} {R[rt]},{simm}({R[rs]})", None
    return f".word 0x{w:08x}(op{op:02x})", None

def tag_addr(v):
    if 0x80100000 <= v < 0x80122000: return "overlay"
    if 0x80010000 <= v < 0x800f0000: return "EXE"
    return "data/?"

def cmd_dis(a, n, binname):
    data, fo, path = load(a, binname)
    print(f"; {path}  @0x{a:08x} ({n} instr)")
    hi = {}
    for i in range(n):
        w = word(data, fo, a + i*4); s, call = dis_one(w, a + i*4)
        ann = call or ""
        op = w >> 26; rt = (w>>16)&31; rs=(w>>21)&31; imm=w&0xffff
        simm = imm-0x10000 if imm&0x8000 else imm
        if op == 0xf: hi[rt] = imm << 16
        elif op in (0x23,0x24,0x25,0x21,0x20,0x28,0x29,0x2b,9) and rs in hi:
            g = (hi[rs] + simm) & 0xffffffff
            if g in GLOBALS: ann = GLOBALS[g]
            elif 0x80000000 <= g < 0x80200000: ann = f"0x{g:08x}"
        print(f"  {a+i*4:08x}: {s:34s} {ann}")

def cmd_table(a, n, binname):
    data, fo, path = load(a, binname)
    print(f"; {path}  pointer table @0x{a:08x}")
    for i in range(n):
        v = word(data, fo, a + i*4)
        print(f"  [{i:2d}] 0x{a+i*4:08x} -> 0x{v:08x}  {tag_addr(v)}")

def cmd_scan(a, binname, maxn=256):
    data, fo, path = load(a, binname)
    print(f"; {path}  scan @0x{a:08x}")
    reg = [0]*32; setf=[False]*32
    for i in range(maxn):
        w = word(data, fo, a + i*4)
        op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; fn=w&63
        imm=w&0xffff; simm=imm-0x10000 if imm&0x8000 else imm
        tgt=((a+i*4+4)&0xf0000000)|((w&0x3ffffff)<<2)
        if op==0xd and rs==0: reg[rt]=imm; setf[rt]=True
        elif op==9 and rs==0: reg[rt]=simm; setf[rt]=True
        elif op==0xf: reg[rt]=imm<<16; setf[rt]=True
        elif op==0x28:                                  # sb rt,off(rs)
            v = reg[rt] if setf[rt] else "?"
            if simm in (4,5,6,7): print(f"  0x{a+i*4:08x}  +0x{simm:x} = {v}")
        elif op in (0x29,0x2b) and simm==4:             # sh/sw to +0x4 (state word)
            print(f"  0x{a+i*4:08x}  +0x4 (word) = {reg[rt] if setf[rt] else '?'}")
        elif op==0xb: print(f"  0x{a+i*4:08x}  sltiu < {imm} (0x{imm:x}) threshold")
        elif op==3:   print(f"  0x{a+i*4:08x}  jal 0x{tgt:08x}  {CALLS.get(tgt,'')}")
        elif op==0 and fn==8:
            print(f"  0x{a+i*4:08x}  jr ra (end)"); return

def cmd_bytes(a, n, binname):
    data, fo, path = load(a, binname)
    off = fo(a); b = data[off:off+n]
    for j in range(0, len(b), 16):
        row = b[j:j+16]
        hexs = " ".join("%02x" % x for x in row)
        print(f"  {a+j:08x}: {hexs}")

def main():
    ap = argparse.ArgumentParser(description="RE1.5 PSX.EXE/overlay disassembler (Weg C)")
    ap.add_argument("cmd", choices=["dis","table","scan","bytes"])
    ap.add_argument("addr")
    ap.add_argument("n", nargs="?", type=lambda s:int(s,0), default=None)
    ap.add_argument("--bin", default=None, help="overlay BIN name (default STAGE1.BIN) for addr>=0x80100000")
    a = ap.parse_args()
    addr = int(a.addr, 0)
    if a.cmd == "dis":   cmd_dis(addr, a.n or 48, a.bin)
    elif a.cmd == "table": cmd_table(addr, a.n or 16, a.bin)
    elif a.cmd == "scan":  cmd_scan(addr, a.bin)
    elif a.cmd == "bytes": cmd_bytes(addr, a.n or 64, a.bin)

if __name__ == "__main__":
    main()
