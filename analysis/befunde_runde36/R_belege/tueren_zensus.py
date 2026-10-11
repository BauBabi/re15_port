#!/usr/bin/env python3
"""Tuer-Zensus (Runde 36 Spur R, Nachbesserung 1 M1/M2): ALLE Door_aot_set (0x3B) jedes RDT — main UND
alle subs, mit dem Bedingungskontext (Ifel_ck/Else_ck/Case), in dem der Record steht — statt nur der
vorderen Records des main00 (so fehlten in stage2_tueren_2040.txt die Records ROOM2070 @0x00AD6/@0x00AFA/
@0x00B1C). Dazu je Raum die Treppen-AOTs (Aot_set sce 12/13 = die beiden Enden einer Treppe, Port
stair_common.c, re15_aot.h) und die Typ-4-Objekte (Kisten/Tritte: Oberkante = Band + 1).

Walker = re15_port/tools/scd_dump_room.py (Opcode-Laengen aus scd_vm.c s_opcode_sizes; 0x2C 20/28,
0x3B 32/40, 0x50 22/30 nach pc[3]&0x80; Ifel_ck Falsch-Ziel pc+4+u16, Else_ck Ziel pc+s16).

Door_aot_set-Nutzlast (32 B, Port aot_common.c op_door_aot_set): [1] slot [2] sce [3] flags [4] floor
[5] follow [6..7] x [8..9] z [10..11] w [12..13] d [14..15] next_x [16..17] next_y [18..19] next_z
[20..21] next_dir [22] stage-Index [23] room [24] cut [25] nfloor (Band bei Ankunft) [26..31] Rest.
Raumnummer = ((stage+1) << 12) | (room << 4), z.B. stage 1 room 4 -> ROOM2040, stage 0 room 0x1a -> ROOM11A0.

Aufruf: tueren_zensus.py <STAGE-Ordner>... [--graph START ZIEL [--ohne ROOM:slot ...]]
"""
import os, struct, sys

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

def u16(b, o): return b[o] | (b[o + 1] << 8)
def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]

def op_size(d, pc):
    op = d[pc]
    if op >= 0x5F or op == 0x1F: return None
    if op == 0x2C: return 28 if (d[pc + 3] & 0x80) else 20
    if op == 0x3B: return 40 if (d[pc + 3] & 0x80) else 32
    if op == 0x50: return 30 if (d[pc + 3] & 0x80) else 22
    return SIZES.get(op)

def section_ptr_extent(d, s):
    if s == 0 or s + 2 > len(d): return 0
    first = u16(d, s)
    if first < 2 or first % 2 or s + first > len(d): return 0
    mx = 0
    for i in range(first // 2):
        o = u16(d, s + 2 * i)
        if o == 0 or s + o >= len(d): continue
        if o > mx: mx = o
    return mx

def rdt_section_end(d, s):
    floor = s + section_ptr_extent(d, s)
    c = []
    for o in range(0x40, 0x60, 4):
        if o + 4 > len(d): break
        v = u32(d, o)
        if floor < v <= len(d): c.append(v)
    return min(c) if c else len(d)

def section_regions(d, ss, se):
    if ss == 0 or ss + 2 > len(d): return []
    first = u16(d, ss)
    if first < 2 or first % 2 or ss + first > len(d): return []
    n = first // 2
    tbl = [u16(d, ss + 2 * i) for i in range(n)]
    so = sorted(set(o for o in tbl if o and ss + o < len(d)))
    regs = []
    for i, o in enumerate(so):
        e = so[i + 1] if i + 1 < len(so) else (se - ss)
        e = min(e, se - ss)
        if e > o: regs.append((o, e))
    off2idx = {}
    for idx, o in enumerate(tbl):
        off2idx.setdefault(o, idx)
    return [(o, e, off2idx.get(o, -1)) for (o, e) in regs]

def room_id(stage, room): return ((stage + 1) << 12) | (room << 4)

def walk(d, start, end, label, out):
    """Sammelt Door_aot_set / Aot_set sce 12,13 / Item_aot_set / Obj_model_set Typ 4 mit Kontext."""
    pc = start
    intervals = []      # (von, bis, text)
    pending_if = []     # offene Ifel: (falsch_ziel, text)
    while pc < end:
        op = d[pc]
        sz = op_size(d, pc)
        if sz is None or pc + sz > end:
            out.append(("ABBRUCH", label, pc, f"op=0x{op:02X}"))
            return
        if op == 0x06:   # Ifel_ck: Bedingung = die direkt folgenden Ck (0x21)
            t = pc + 4 + u16(d, pc + 2)
            q = pc + 4; cks = []
            while q < end and d[q] == 0x21:
                cks.append(f"Ck({d[q+1]},{d[q+2]})={d[q+3]}"); q += 4
            text = " & ".join(cks) if cks else f"Ifel@0x{pc:05X}"
            intervals.append((pc, t, text))
            pending_if.append((t, text))
        elif op == 0x07:   # Else_ck: Gegenteil der innersten Ifel, deren Falsch-Ziel hinter diesem Else liegt
            t = pc + s16(d, pc + 2)
            text = "ELSE"
            for (ft, tx) in reversed(pending_if):
                if ft == pc + 4:
                    text = "NOT(" + tx + ")"; break
            intervals.append((pc + 4, t, text))
        elif op == 0x14:   # Case
            t = pc + 6 + u16(d, pc + 2)
            intervals.append((pc, t, f"Case({u16(d, pc + 4)})"))
        ctx = " / ".join(tx for (a, b, tx) in intervals if a <= pc < b)
        raw = d[pc:pc + sz]
        if op == 0x3B:
            stage, room = raw[22], raw[23]
            out.append(("DOOR", label, pc, dict(
                slot=raw[1], sce=raw[2], flags=raw[3], floor=raw[4],
                x=s16(raw, 6), z=s16(raw, 8), w=s16(raw, 10), dd=s16(raw, 12),
                nx=s16(raw, 14), ny=s16(raw, 16), nz=s16(raw, 18), ndir=s16(raw, 20),
                stage=stage, room=room, ziel=room_id(stage, room), cut=raw[24], nfloor=raw[25],
                rest=" ".join(f"{b:02x}" for b in raw[26:]), ctx=ctx,
                hexs=" ".join(f"{b:02x}" for b in raw))))
        elif op == 0x2C and raw[2] in (12, 13):
            out.append(("STAIR", label, pc, dict(slot=raw[1], sce=raw[2], floor=raw[4],
                x=s16(raw, 6), z=s16(raw, 8), w=s16(raw, 10), dd=s16(raw, 12), ctx=ctx)))
        elif op == 0x50:
            out.append(("ITEM", label, pc, dict(slot=raw[1], floor=raw[4],
                x=s16(raw, 6), z=s16(raw, 8), w=s16(raw, 10), dd=s16(raw, 12),
                item=raw[14], menge=raw[16], ctx=ctx)))
        elif op == 0x2D and raw[2] == 4:
            out.append(("OBJ4", label, pc, dict(obj=raw[1], band=raw[4],
                x=s16(raw, 10), y=s16(raw, 12), z=s16(raw, 14),
                hx=s16(raw, 28), hy=s16(raw, 30), hz=s16(raw, 32), ctx=ctx)))
        if op == 0x01:
            # Evt_end: wie scd_dump_room.py weiterlaufen, solange ein Vorwaertsziel dahinter liegt
            maxf = max([b for (a, b, _) in intervals] + [0])
            if pc + 2 > maxf: return
        pc += sz

def zensus_rdt(path):
    d = open(path, "rb").read()
    name = os.path.basename(path)[:-4]
    out = []
    if len(d) < 0x60: return name, out          # Platzhalter-RDT (4 Byte) ueberspringen
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d): continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            walk(d, sec + o, sec + e, f"{tag}{idx:02d}", out)
    return name, out

def fmt_door(name, lbl, pc, r):
    return (f"{name} {lbl} @0x{pc:05X} slot {r['slot']:2d} sce {r['sce']} floor {r['floor']} "
            f"rect x {r['x']}..{r['x']+r['w']} z {r['z']}..{r['z']+r['dd']} -> ROOM{r['ziel']:04X} "
            f"cut {r['cut']:2d} nfloor {r['nfloor']} spawn ({r['nx']},{r['ny']},{r['nz']}) dir {r['ndir']} "
            f"rest [{r['rest']}]" + (f"  BEDINGT: {r['ctx']}" if r['ctx'] else "  unbedingt"))

def main():
    args = sys.argv[1:]
    graph = None; ohne = set()
    if "--graph" in args:
        i = args.index("--graph"); graph = (args[i + 1], args[i + 2]); del args[i:i + 3]
    if "--ohne" in args:
        i = args.index("--ohne"); ohne = set(a.upper() for a in args[i + 1:]); del args[i:]
    rooms = {}
    for stage_dir in args:
        for fn in sorted(os.listdir(stage_dir)):
            if not fn.upper().endswith(".RDT"): continue
            name, out = zensus_rdt(os.path.join(stage_dir, fn))
            rooms[name] = out
    for name in sorted(rooms):
        out = rooms[name]
        doors = [(l, pc, r) for (k, l, pc, r) in out if k == "DOOR"]
        stairs = [(l, pc, r) for (k, l, pc, r) in out if k == "STAIR"]
        objs = [(l, pc, r) for (k, l, pc, r) in out if k == "OBJ4"]
        items = [(l, pc, r) for (k, l, pc, r) in out if k == "ITEM"]
        abbr = [(l, pc, r) for (k, l, pc, r) in out if k == "ABBRUCH"]
        if not doors and not stairs and not objs: continue
        print(f"=== {name}: {len(doors)} Tuer-Records, {len(stairs)} Treppen-AOTs, {len(objs)} Typ-4-Objekte, {len(items)} Item-AOTs")
        for (l, pc, r) in doors: print("  " + fmt_door(name, l, pc, r))
        for (l, pc, r) in stairs:
            print(f"  {name} {l} @0x{pc:05X} TREPPE slot {r['slot']} sce {r['sce']} floor {r['floor']} rect x {r['x']}..{r['x']+r['w']} z {r['z']}..{r['z']+r['dd']}" + (f"  BEDINGT: {r['ctx']}" if r['ctx'] else ""))
        for (l, pc, r) in objs:
            print(f"  {name} {l} @0x{pc:05X} OBJ4 obj {r['obj']} band {r['band']} pos ({r['x']},{r['y']},{r['z']}) h({r['hx']},{r['hy']},{r['hz']})" + (f"  BEDINGT: {r['ctx']}" if r['ctx'] else ""))
        for (l, pc, r) in items:
            print(f"  {name} {l} @0x{pc:05X} ITEM slot {r['slot']} floor {r['floor']} item 0x{r['item']:02X} x{r['menge']} rect x {r['x']}..{r['x']+r['w']} z {r['z']}..{r['z']+r['dd']}" + (f"  BEDINGT: {r['ctx']}" if r['ctx'] else ""))
        for (l, pc, r) in abbr: print(f"  {name} {l} @0x{pc:05X} ABBRUCH {r}")
    if graph:
        bfs(rooms, graph[0].upper(), graph[1].upper(), ohne)

# ---- Graph auf (Raum, Band): Tuer-Kanten (Raum, floor) -> (Ziel, nfloor); Treppen-Kanten (gleiches Rechteck,
# zwei Etagen); Kletter-Kanten an Typ-4-Objekten (Band -> Band+1, NUR wenn das Objekt an der richtigen Stelle
# steht — als "KLETTERN" markiert). Annahme: zwei Tueren desselben Bands eines Raums sind verbunden — FALSCH fuer
# ROOM2070 (drei getrennt ummauerte Gaenge, SCA Band 0 r0[0..3]/r0[4..7]/r0[8..11]): 2070 wird nach Gang geteilt.
def knoten(room, band, z=None):
    if room == "ROOM2070" or room == "ROOM2071":
        if z is None: return f"{room}/B{band}"
        if z < -12000: return f"{room}N/B{band}"
        if z < 0: return f"{room}M/B{band}"
        return f"{room}S/B{band}"
    return f"{room}/B{band}"

def bfs(rooms, start, ziel, ohne):
    import collections
    adj = collections.defaultdict(list)
    for name, out in rooms.items():
        for (k, l, pc, r) in out:
            if k == "DOOR":
                if f"{name}:{r['slot']}" in ohne: continue
                tgt = f"ROOM{r['ziel']:04X}"
                a = knoten(name, r['floor'], r['z'] + r['dd'] // 2)
                b = knoten(tgt, r['nfloor'], r['nz'])
                adj[a].append((b, f"{name} slot {r['slot']} @0x{pc:05X} ({l}) floor {r['floor']} -> {tgt} nfloor {r['nfloor']}" + (f" [{r['ctx']}]" if r['ctx'] else "")))
        st = [(l, pc, r) for (k, l, pc, r) in out if k == "STAIR"]
        for i in range(len(st)):
            for j in range(i + 1, len(st)):
                ri, rj = st[i][2], st[j][2]
                if (ri['x'], ri['z'], ri['w'], ri['dd']) == (rj['x'], rj['z'], rj['w'], rj['dd']) and ri['floor'] != rj['floor']:
                    a = knoten(name, ri['floor'], ri['z']); b = knoten(name, rj['floor'], rj['z'])
                    adj[a].append((b, f"{name} TREPPE slot {ri['slot']}/{rj['slot']} floor {ri['floor']} <-> {rj['floor']}"))
                    adj[b].append((a, f"{name} TREPPE slot {rj['slot']}/{ri['slot']} floor {rj['floor']} <-> {ri['floor']}"))
        for (l, pc, r) in [(l, pc, r) for (k, l, pc, r) in out if k == "OBJ4"]:
            a = knoten(name, r['band'], r['z']); b = knoten(name, r['band'] + 1, r['z'])
            adj[a].append((b, f"{name} KLETTERN obj {r['obj']} @0x{pc:05X} band {r['band']} -> {r['band']+1} (Objekt muss an der Stelle stehen)"))
            adj[b].append((a, f"{name} ABSTEIGEN obj {r['obj']} band {r['band']+1} -> {r['band']}"))
    # Start/Ziel duerfen "ROOMxxxx/bN" oder "ROOMxxxx" (irgendein Band) sein
    starts = [n for n in adj if n.startswith(start)] if "/" not in start else [start]
    prev = {s: None for s in starts}
    dq = collections.deque(starts)
    while dq:
        n = dq.popleft()
        if n.startswith(ziel) and ("/" not in ziel or n == ziel):
            path = []; cur = n
            while prev[cur] is not None:
                p, why = prev[cur]; path.append(why); cur = p
            print(f"\nWEG {start} -> {n} ({len(path)} Kanten), ohne {sorted(ohne) or '-'}:")
            for why in reversed(path): print("  " + why)
            return
        for (m, why) in adj.get(n, []):
            if m not in prev:
                prev[m] = (n, why); dq.append(m)
    print(f"\nKEIN WEG {start} -> {ziel} ohne {sorted(ohne) or '-'}")

main()
