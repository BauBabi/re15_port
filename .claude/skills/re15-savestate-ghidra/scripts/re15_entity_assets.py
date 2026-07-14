#!/usr/bin/env python3
"""re15_entity_assets.py — GROUND-TRUTH model+animation resolver (kills the trial-and-error).

For any entity in a DuckStation savestate of the ORIGINAL, this reads the entity's OWN
asset pointers out of live RAM and byte-matches them against the extracted asset files, so
you learn the EXACT model (MD1), skeleton (EMR), animation set (EDD) and clip index the
original uses — with ZERO guessing.

Why this works: an RE1.5 actor struct stores, per entity, pointers to its resident anim
data. The player-select model uses +0x170=EMR(skeleton) / +0x174=EDD(anim); enemies/NPCs
carry the same kind of pointers (the exact offset varies by struct, so we scan a window and
match whatever the pointer resolves to). The clip actually playing is +0x94 (motion), the
frame +0x95. Match the pointed-to bytes to every *section* of every PLD/PLW/EMD container →
the file + section that IS that data.

Usage:
  python re15_entity_assets.py <savestate.sav> <entity_addr> [<entity_addr> ...]
  python re15_entity_assets.py <savestate.sav> --scan 0x800acc2c 0x2000 0x200
      (--scan <start> <span> <stride>: try every struct base in [start, start+span) )

Get entity addresses from re15_enemy_state.py (it prints each slot's base), or use known
bases (player-select Leon=0x800acc2c, right char=0x800ace20).
"""
import sys, os, struct, glob, importlib.util

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("re15_ss", os.path.join(HERE, "re15_ss.py"))
ss = importlib.util.module_from_spec(spec); spec.loader.exec_module(ss)

# Asset roots to match against (extend as needed).
ASSET_DIRS = [
    r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\PLD",
    r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\EMD",
]
MATCH_LEN = 128   # bytes compared (section header + first records is plenty to be unique)

def build_section_index():
    """Return list of (label, first MATCH_LEN bytes) for every section of every container."""
    idx = []
    for d in ASSET_DIRS:
        if not os.path.isdir(d): continue
        for p in sorted(glob.glob(os.path.join(d, "*"))):
            try:
                data = open(p, "rb").read()
            except Exception:
                continue
            name = os.path.basename(p)
            # whole-file (covers already-split .EDD/.EMR/.MD1/.TIM)
            idx.append((name, data[:MATCH_LEN]))
            # PLD/PLW container: header word0=dir_offset, word1=count, then section offsets
            if len(data) >= 8:
                do = struct.unpack('<I', data[0:4])[0]
                cnt = struct.unpack('<I', data[4:8])[0]
                if 0 < cnt <= 16 and 8 <= do < len(data) - cnt*4:
                    offs = [struct.unpack('<I', data[do+i*4:do+i*4+4])[0] for i in range(cnt)]
                    bounds = offs + [do]
                    secnames = {0:"EDD", 1:"EMR", 2:"MD1", 3:"TIM"}
                    for i in range(cnt):
                        if 0 <= bounds[i] < len(data):
                            idx.append(("%s[%d=%s]@0x%x" % (name, i, secnames.get(i, "?"), bounds[i]),
                                        data[bounds[i]:bounds[i]+MATCH_LEN]))
    return idx

def match_ptr(ram_bytes, index):
    hits = [label for (label, blob) in index if blob == ram_bytes and len(blob) == MATCH_LEN]
    return hits

def sgn(v): return v - 0x100000000 if v >= 0x80000000 else v

def dump_entity(r, base, index):
    def u32(a):
        b = r.bytes(a, 4); return b[0] | (b[1]<<8) | (b[2]<<16) | (b[3]<<24)
    def u8(a): return r.bytes(a, 1)[0]
    print("=== entity @0x%08x ===" % base)
    clip = u8(base+0x94); frame = u8(base+0x95)
    print("  clip(+0x94)=%d  anim_frame(+0x95)=%d  rot_y(+0x6a)=%d" % (clip, frame, u32(base+0x6a) & 0xffff))
    print("  pos +0x34/38/3c = (%d, %d, %d)" % (sgn(u32(base+0x34)), sgn(u32(base+0x38)), sgn(u32(base+0x3c))))
    # scan pointer window; match every RAM pointer to a file section
    for off in range(0x150, 0x188, 4):
        p = u32(base+off)
        if 0x80010000 <= p < 0x80200000:
            try:
                rb = r.bytes(p, MATCH_LEN)
            except Exception:
                continue
            hits = match_ptr(rb, index)
            if hits:
                print("  +0x%03x -> 0x%08x == %s" % (off, p, ", ".join(hits)))

def main():
    if len(sys.argv) < 3:
        print(__doc__); sys.exit(1)
    sav = sys.argv[1]
    r = ss.Ram(sav)
    index = build_section_index()
    print("indexed %d asset sections from %d dirs\n" % (len(index), len(ASSET_DIRS)))
    if sys.argv[2] == "--scan":
        start = int(sys.argv[3], 0); span = int(sys.argv[4], 0); stride = int(sys.argv[5], 0)
        for base in range(start, start+span, stride):
            def u32(a):
                b = r.bytes(a, 4); return b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24)
            hit = any(0x80010000 <= u32(base+off) < 0x80200000 and
                      match_ptr(r.bytes(u32(base+off), MATCH_LEN), index)
                      for off in range(0x150, 0x188, 4))
            if hit:
                dump_entity(r, base, index)
    else:
        for a in sys.argv[2:]:
            dump_entity(r, int(a, 0), index)

if __name__ == "__main__":
    main()
