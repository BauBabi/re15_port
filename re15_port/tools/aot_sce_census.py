#!/usr/bin/env python3
"""Game-wide AOT sce-type census over all shipped RE1.5 RDTs.

Walks main-SCD (RDT u32 @0x40) + sub-SCD (RDT u32 @0x44) sections opcode-exactly
using the port's byte-true opcode length table (re15_port/engine/src/scd_vm.c
s_opcode_sizes) with the disasm-verified variable-length overrides:

  0x2C Aot_set       20 / 28 if pc[3]&0x80   (@0x80040590-0x800405b0)
  0x3B Door_aot_set  32 / 40 if pc[3]&0x80   (@0x80040618-0x80040638)
  0x50 Item_aot_set  22 / 30 if pc[3]&0x80   (@0x8004065c-0x80040668)
  0x2D Obj_model_set 34 fixed                (@0x80040aa4 `addiu v0,a2,34`)
  0x46 Aot_reset     10                      (@0x800407ac `addiu v0,a2,10`)

AOT record = pc+2 (installers @0x80040580/0x80040608 store pc+2 into
DAT_800ac9b0[slot]), so the sce byte = pc[2]. Aot_reset (0x46) RETYPES an
existing record: rec[0]=pc[2] (@0x80040764 `sb v0,0(a1)`), so its pc[2] values
count as used sce types too.

Opcodes >= 0x5F do not exist in the RE1.5 dispatch table (PTR_LAB_800744a8 has
95 entries, last @0x80074620) -> treated as walk desync, stop cleanly + report.
"""
import glob, json, os, struct, sys
from collections import defaultdict

ROOT = r"c:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"

# port scd_vm.c s_opcode_sizes (fixed sizes), indices 0x00..0x5E
SIZES = {
    0x00:1, 0x01:2, 0x02:1, 0x03:4, 0x04:4, 0x05:2, 0x06:4, 0x07:4,
    0x08:2, 0x09:4, 0x0A:3, 0x0B:1, 0x0C:1, 0x0D:6, 0x0E:2, 0x0F:4,
    0x10:2, 0x11:4, 0x12:2, 0x13:4, 0x14:6, 0x15:4, 0x16:2, 0x17:6,
    0x18:2, 0x19:2, 0x1A:2, 0x1B:6, 0x1C:1, 0x1D:1, 0x1E:1,
    0x20:1, 0x21:4, 0x22:4, 0x23:6, 0x24:4, 0x25:3, 0x26:6, 0x27:4,
    0x28:1, 0x29:2, 0x2A:1, 0x2B:4, 0x2C:20, 0x2D:34, 0x2E:3, 0x2F:4,
    0x30:1, 0x31:1, 0x32:8, 0x33:8, 0x34:4, 0x35:3, 0x36:12, 0x37:4,
    0x38:12, 0x39:4, 0x3A:16, 0x3B:32, 0x3C:2, 0x3D:3, 0x3E:6, 0x3F:4,
    0x40:8, 0x41:10, 0x42:1, 0x43:4, 0x44:20, 0x45:3, 0x46:10, 0x47:2,
    0x48:16, 0x49:8, 0x4A:2, 0x4B:3, 0x4C:18, 0x4D:10, 0x4E:5, 0x4F:22,
    0x50:22, 0x51:4, 0x52:4, 0x53:3, 0x54:6, 0x55:6, 0x56:6, 0x57:4,
    0x58:4, 0x59:4, 0x5A:6, 0x5B:4, 0x5C:4, 0x5D:4, 0x5E:4,
}
# 0x1F is unregistered in the port table -> desync stop

def u16(b, o): return b[o] | (b[o+1] << 8)
def u32(b, o): return struct.unpack_from("<I", b, o)[0]
def s16(b, o): return struct.unpack_from("<h", b, o)[0]

def op_size(data, pc):
    op = data[pc]
    if op >= 0x5F or op == 0x1F:
        return None
    if op == 0x2C:  return 28 if (data[pc+3] & 0x80) else 20
    if op == 0x3B:  return 40 if (data[pc+3] & 0x80) else 32
    if op == 0x50:  return 30 if (data[pc+3] & 0x80) else 22
    return SIZES.get(op)

def collect(data, pc, op, room, script, installs):
    """Record AOT installs / retypes. Record = pc+2: rec[0]=sce, rec[1]=flags,
    rec[2]=floor, rec[3]=follow, geometry rec+4.., payload rec+0xc (rect) /
    rec+0x14 (poly)."""
    if op in (0x2C, 0x3B, 0x50):
        long_form = (data[pc+3] & 0x80) != 0
        pay = pc + (22 if long_form else 14)   # payload = rec+0x14 / rec+0xc
        entry = dict(
            room=room, script=script, off=pc, op=op,
            slot=data[pc+1], sce=data[pc+2], flags=data[pc+3],
            floor=data[pc+4], follow=data[pc+5], long=int(long_form),
            rect=[s16(data, pc+6), s16(data, pc+8), s16(data, pc+10), s16(data, pc+12)],
            p0=u16(data, pay), p1=u16(data, pay+2), p2=u16(data, pay+4),
            pb=[data[pay+i] for i in range(6)],
        )
        installs.append(entry)
    elif op == 0x46:
        installs.append(dict(
            room=room, script=script, off=pc, op=op,
            slot=data[pc+1], sce=data[pc+2], flags=data[pc+3],
            floor=None, follow=None, long=0, rect=None,
            p0=u16(data, pc+4), p1=u16(data, pc+6), p2=u16(data, pc+8),
            pb=[data[pc+4+i] for i in range(6)],
        ))

def fwd_target(data, pc, op):
    """Forward-jump target of control-flow opcodes (port scd_vm.c, byte-true):
    If 0x06 pc+4+len@+2 (op_if), Else 0x07 pc+len@+2 (op_else), For 0x0D /
    For2 0x1B pc+6+len@+2 (op_for/op_for2), While 0x0F pc+4+len@+2 (op_while),
    Do 0x11 pc+4+len@+2 (op_do), Switch 0x13 pc+4+len@+2 (op_switch exit),
    Case 0x14 pc+6+len@+2 (op_switch scan), Goto 0x17 pc+s16@+4 (op_goto)."""
    if op == 0x06: return pc + 4 + u16(data, pc+2)
    if op == 0x07: return pc + s16(data, pc+2)
    if op in (0x0D, 0x1B): return pc + 6 + s16(data, pc+2)
    if op in (0x0F, 0x11, 0x13): return pc + 4 + s16(data, pc+2)
    if op == 0x14: return pc + 6 + u16(data, pc+2)
    if op == 0x17:
        o = s16(data, pc+4)
        return pc + o if o > 0 else None
    return None

def walk(data, start, end, room, script, installs, stats):
    pc = start
    max_fwd = start
    clean = None
    while pc < end:
        if pc + 1 > len(data): break
        op = data[pc]
        if op == 0x01 and pc + 2 > max_fwd:
            clean = pc + 2          # Evt_end with no forward ref beyond -> true end
            pc += 2
            break
        sz = op_size(data, pc) if pc + 4 <= len(data) else SIZES.get(op)
        if sz is None:
            stats["stops"].append((room, script, pc, op, end - pc))
            break
        if pc + sz > end:
            # opcode straddles region end -- count as covered up to end (regions
            # from the pointer table can be conservative), then stop.
            pc = end
            break
        t = fwd_target(data, pc, op)
        if t is not None and t > max_fwd: max_fwd = t
        collect(data, pc, op, room, script, installs)
        pc += sz
    stats["walked"] += min(pc, end) - start
    stats["total"] += end - start
    if clean is not None:
        stats["residue"] += end - clean   # bytes after clean Evt_end = data, not code

def section_regions(data, sec_start, sec_end):
    """Pointer table of u16 offsets rel. to section start; count=first/2."""
    if sec_start == 0 or sec_start + 2 > len(data): return []
    first = u16(data, sec_start)
    if first < 2 or first % 2 or sec_start + first > len(data): return []
    n = first // 2
    offs = []
    for i in range(n):
        o = u16(data, sec_start + 2*i)
        if o == 0 or sec_start + o >= len(data): continue
        offs.append(o)
    regions = []
    so = sorted(set(offs))
    for i, o in enumerate(so):
        end = so[i+1] if i+1 < len(so) else (sec_end - sec_start)
        end = min(end, sec_end - sec_start)
        if end > o:
            regions.append((o, end))
    # map back: name each region by the FIRST table index using that offset
    names = {}
    for idx in range(n):
        o = u16(data, sec_start + 2*idx)
        if o in [r[0] for r in regions] and o not in names.values():
            names.setdefault(idx, o)
    off2name = {}
    for idx, o in sorted(names.items()):
        off2name.setdefault(o, idx)
    return [(o, e, off2name.get(o, -1)) for (o, e) in regions]

def rdt_section_end(data, sec_start):
    """Smallest plausible RDT address-table entry > sec_start, else EOF."""
    cands = []
    for o in range(0x40, 0x90, 4):
        if o + 4 > len(data): break
        v = u32(data, o)
        if sec_start < v <= len(data):
            cands.append(v)
    return min(cands) if cands else len(data)

def main():
    installs, per_room_cov = [], {}
    files = sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT")))
    for f in files:
        room = os.path.basename(f)[4:-4]           # e.g. 10001? no: ROOM1000.RDT -> "1000"
        data = open(f, "rb").read()
        if len(data) < 0x48: continue
        main_s, sub_s = u32(data, 0x40), u32(data, 0x44)
        stats = dict(walked=0, total=0, residue=0, stops=[])
        for (sec, tag) in ((main_s, "main"), (sub_s, "sub")):
            if sec == 0 or sec >= len(data): continue
            send = rdt_section_end(data, sec)
            for (o, e, idx) in section_regions(data, sec, send):
                walk(data, sec + o, sec + e, room, f"{tag}{idx:02d}", installs, stats)
        per_room_cov[room] = stats
    # ---- matrix ----
    by_sce = defaultdict(list)
    for e in installs:
        by_sce[e["sce"]].append(e)
    out = {
        "n_files": len(files),
        "n_installs": len(installs),
        "sce_counts": {str(k): len(v) for k, v in sorted(by_sce.items())},
        "installs": installs,
        "coverage": {r: dict(walked=s["walked"], total=s["total"], residue=s["residue"],
                             pct=(100.0 * s["walked"] / max(1, s["total"] - s["residue"])),
                             stops=[(sc, hex(pc), hex(op), rem) for (_, sc, pc, op, rem) in s["stops"]])
                     for r, s in per_room_cov.items()},
    }
    json.dump(out, open(sys.argv[1], "w"), indent=1)
    # ---- console summary ----
    print(f"files={len(files)} installs={len(installs)}")
    print("sce -> count (by opcode):")
    for sce in sorted(by_sce):
        ops = defaultdict(int)
        for e in by_sce[sce]: ops[e["op"]] += 1
        rooms = sorted(set(e["room"] for e in by_sce[sce]))
        print(f"  sce {sce:3d}: n={len(by_sce[sce]):4d} ops={{{', '.join(f'{k:#04x}:{v}' for k,v in sorted(ops.items()))}}} rooms={len(rooms)}")
    tw = sum(s["total"] for s in per_room_cov.values())
    ww = sum(s["walked"] for s in per_room_cov.values())
    rr = sum(s["residue"] for s in per_room_cov.values())
    code = tw - rr
    print(f"coverage: walked {ww} of {code} code bytes = {100.0*ww/code:.2f}% (residue after clean Evt_end: {rr})")
    nstop = sum(len(s["stops"]) for s in per_room_cov.values())
    print(f"rooms_with_stops={sum(1 for s in per_room_cov.values() if s['stops'])} total stops: {nstop}")

if __name__ == "__main__":
    main()
