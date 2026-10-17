import struct, glob, os, collections
BAUM = r"C:/workspace/git/reAi_v2/.claude/worktrees/r38_wasser"
stat = collections.Counter(); bsp = collections.defaultdict(list)
for path in sorted(glob.glob(BAUM + "/re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT")):
    d = open(path, 'rb').read()
    if len(d) < 0x60: continue
    u32 = lambda o: struct.unpack_from('<I', d, o)[0]
    idh, pe, tb, te = u32(0x4c), u32(0x50), u32(0x54), u32(0x58)
    if idh == 0 or idh + 8 > len(d) or u32(idh) == 0xffffffff: continue
    room = os.path.basename(path)[4:8]
    for i in range(8):
        eid = d[idh + i]
        if eid == 0xff: break
        ent = struct.unpack_from('<i', d, pe - i*4)[0]; es = idh + ent
        clut = struct.unpack_from('<H', d, es + 4)[0]
        if tb == 0 or te == 0: continue
        e = u32(te - 4*(i+1))
        if e == 0xffffffff: continue
        tim = tb + e
        if not (u32(tim + 4) & 8): continue
        cx, cy, cw, ch = struct.unpack_from('<4H', d, tim + 12)
        row = (clut >> 6) - cy
        stat[(ch, row)] += 1
        if row != 0: bsp[(eid, ch, row)].append(room)
print("(CLUT-Zeilen im TIM, Kopf-Zeile) -> Anzahl:", dict(stat))
for k, v in sorted(bsp.items()):
    print("id %#04x zeilen=%d kopfzeile=%d: %d Raeume %s" % (k[0], k[1], k[2], len(v), " ".join(v[:12])))
