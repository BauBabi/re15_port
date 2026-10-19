# Treppen-AOTs (Aot_set Opcode 0x2C, sce 12/13 = STAIR-Band-Zone) eines Raums mit DATEI-OFFSET, Band (pc[4]),
# Rechteck und Mitte. Parser wie re15_port/tools/gen_map_zones.py read_rdt (SZ-Tabelle von dort).
import sys, struct, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/re15_port/tools')
import gen_map_zones as G
def treppen(rid):
    p = os.path.join(G.ROOT, 're15_port', 'shared_assets', 'PSX', f'STAGE{rid >> 12}', f'ROOM{rid:04X}.RDT')
    d = open(p, 'rb').read()
    offs = {nm: struct.unpack_from('<I', d, 0x20 + 4*i)[0] for i, nm in enumerate(G.NAMES)}
    out = []
    for name in ('mainScd', 'subScd'):
        s = offs[name]
        e = (sorted(v for v in offs.values() if v > s) + [len(d)])[0]
        n0 = struct.unpack_from('<H', d, s)[0]
        if n0 == 0 or n0 > 0x200 or n0 % 2: continue
        ptrs = [struct.unpack_from('<H', d, s + 2*i)[0] for i in range(n0 // 2)]
        for si, pp in enumerate(ptrs):
            end = ptrs[si+1] if si+1 < len(ptrs) else (e - s)
            pc = pp
            while pc < end:
                op = d[s+pc]; sz = G.SZ.get(op)
                if sz is None: break
                if op == 0x2c:
                    b = d[s+pc:s+pc+sz]
                    if b[2] in (12, 13):
                        rx, rz, rw, rd = struct.unpack_from('<hhhh', b, 6)
                        out.append(dict(off=s+pc, scd=name, sub=si, sce=b[2], band=b[4], count=b[16], rx=rx, rz=rz, rw=rw, rd=rd,
                                        cx=rx + rw//2, cz=rz + rd//2, roh=b.hex()))
                pc += sz
    return out
if __name__ == '__main__':
    for a in sys.argv[1:]:
        for t in treppen(int(a, 16)):
            print(f"ROOM{a} @0x{t['off']:05X} {t['scd']}[{t['sub']}] sce {t['sce']} band {t['band']} count {t['count']} rect ({t['rx']},{t['rz']}) {t['rw']}x{t['rd']} mitte ({t['cx']},{t['cz']})  {t['roh']}")
