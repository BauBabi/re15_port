import struct, sys, glob
BAUM = r"C:/workspace/git/reAi_v2/.claude/worktrees/r38_wasser"
room = sys.argv[1]
d = open(glob.glob(BAUM + f"/re15_port/shared_assets/PSX/STAGE*/ROOM{room}.RDT")[0], 'rb').read()
u32 = lambda o: struct.unpack_from('<I', d, o)[0]
idh, pe, tb, te = u32(0x4c), u32(0x50), u32(0x54), u32(0x58)
ids = list(d[idh:idh+8])
print("ids", ids, "pe", hex(pe), "tb", hex(tb), "te", hex(te))
used = [i for i in ids if i != 0xff]
for i, eid in enumerate(used):
    eptr = u32(pe - 4*(i+1))   # EFF-Zeiger abwaerts
    e = tb + u32(te - 4*(i+1))
    flag, bpp = u32(e), u32(e+4)
    s = f"eff[{i}] id={eid:#x} eff_ptr={eptr:#x} tim@{e:#x} magic={flag:#x} flags={bpp:#x}"
    if bpp & 8:
        clen = u32(e+8); cx, cy, cw, ch = struct.unpack_from('<4H', d, e+12)
        s += f" CLUT xy=({cx},{cy}) wh=({cw},{ch})"
        p = e + 8 + clen
    else:
        p = e + 8
    plen = u32(p); px, py, pw, ph = struct.unpack_from('<4H', d, p+4)
    s += f" PIX xy=({px},{py}) wh=({pw},{ph})"
    print(s)
