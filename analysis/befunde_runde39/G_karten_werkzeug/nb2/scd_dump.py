# SCD eines Raums opcodeweise (Datei-Offset, Opcode, Rohbytes) - Laengen aus gen_map_zones.SZ
import sys, struct, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/re15_port/tools')
import gen_map_zones as G
rid = int(sys.argv[1], 16)
p = os.path.join(G.ROOT, 're15_port', 'shared_assets', 'PSX', f'STAGE{rid >> 12}', f'ROOM{rid:04X}.RDT')
d = open(p, 'rb').read()
offs = {nm: struct.unpack_from('<I', d, 0x20 + 4*i)[0] for i, nm in enumerate(G.NAMES)}
print({k: hex(v) for k, v in offs.items()})
for name in ('mainScd', 'subScd'):
    s = offs[name]
    e = (sorted(v for v in offs.values() if v > s) + [len(d)])[0]
    n0 = struct.unpack_from('<H', d, s)[0]
    ptrs = [struct.unpack_from('<H', d, s + 2*i)[0] for i in range(n0 // 2)]
    for si, pp in enumerate(ptrs):
        end = ptrs[si+1] if si+1 < len(ptrs) else (e - s)
        pc = pp
        print(f'--- {name}[{si}] @0x{s+pp:05X}')
        while pc < end:
            op = d[s+pc]; sz = G.SZ.get(op)
            if sz is None:
                print(f'  @0x{s+pc:05X} ?? op {op:02x}'); break
            print(f'  @0x{s+pc:05X} {op:02x} {d[s+pc:s+pc+sz].hex(" ")}')
            pc += sz
