# schwerkraft_zensus.py — Nebenwirkungs-Zensus der NPC-Schwerkraft (Runde 38 Spur M).
# Die NPC-Wurzeln rufen FUN_8001bd60 (@0x8011dcc4) in JEDEM Bild: eine NPC faellt, wenn ihr Mittelpunkt mindestens den
# Radius (Box +6 = 450, Kasten @0x80121658) tief in einer Absprungzelle (u1 & 2) IHRES Bandes liegt und y == -(Band*1800)
# (@0x8001bd94-bdec). Gesucht: jede Stelle, an der eine Original-Szene eine NPC (Typ 0x40..0x4d) per Sce_em_set / Pos_set
# HINSTELLT (nicht hinlaufen laesst) und sie dort sofort fallen wuerde. Lineare SCD-Dekodierung (kanten_zensus.py).
import glob, os, struct, sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_npckante/analysis/nutzer_batch_2026-08-27/tools')
import re15_scd_dis as D
L = D.lens()


def s16(b, o):
    return struct.unpack_from('<h', b, o)[0]


def zellen(d):
    off = struct.unpack_from('<I', d, 0x20)[0]
    if off == 0 or off + 24 > len(d):
        return []
    cnt = list(struct.unpack_from('<5I', d, off + 4))
    out, p = [], off + 24
    for g in range(5):
        for i in range(cnt[g]):
            if p + 12 > len(d):
                break
            w, dd, cx, cz, t, u0, u1, fl = struct.unpack_from('<HHhhBBBB', d, p)
            out.append((cx, cz, w, dd, u1, fl))
            p += 12
    return out


def wort(cs, x, z, band, r):
    # FUN_8003b7f0 mit Radius -r (Zelle um r geschrumpft), Band = sign. Nibble Bit 15..12 von (u1 | floor<<8)
    for (cx, cz, w, dd, u1, fl) in cs:
        wd = u1 | (fl << 8)
        cb = ((wd << 16) & 0xffffffff)
        cb = (cb if cb < 0x80000000 else cb - 0x100000000) >> 28
        if cb != band:
            continue
        if 0 <= x - (cx + r) < w - 2 * r and 0 <= z - (cz + r) < dd - 2 * r:
            return wd
    return 0


treffer = 0
for f in sorted(glob.glob(r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_npckante/re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT')):
    d = open(f, 'rb').read()
    if len(d) < 0x48:
        continue
    cs = zellen(d)
    if not any(c[4] & 2 for c in cs):
        continue
    raum = os.path.basename(f)[:-4]
    slots = {}
    work = None
    for key in (0x40, 0x44):
        s = struct.unpack_from('<I', d, key)[0]
        if s == 0 or s >= len(d):
            continue
        n = struct.unpack_from('<H', d, s)[0] // 2
        offs = [struct.unpack_from('<H', d, s + 2 * i)[0] for i in range(n)]
        for i, o in enumerate(offs):
            pc = s + o
            e = s + offs[i + 1] if i + 1 < len(offs) else len(d)
            while pc < e and pc < len(d):
                op = d[pc]
                ln = L[op] or 1
                if op == 0x2D:
                    ln = 34
                stelle = None
                if op == 0x44 and pc + 18 < len(d):
                    sl, ty, fl = d[pc + 1], d[pc + 2], d[pc + 4]
                    slots[sl] = dict(type=ty, x=s16(d, pc + 8), y=s16(d, pc + 10), z=s16(d, pc + 12), band=fl)
                    stelle = ('Sce_em_set', sl)
                elif op == 0x2E:
                    work = d[pc + 2] if d[pc + 1] == 2 else None
                elif op == 0x32 and work is not None and work in slots:
                    st = slots[work]
                    st['x'], st['y'], st['z'] = s16(d, pc + 2), s16(d, pc + 4), s16(d, pc + 6)
                    stelle = ('Pos_set', work)
                elif op == 0x34 and work is not None and work in slots and d[pc + 1] == 0x12:
                    slots[work]['band'] = d[pc + 2]
                    stelle = ('Member_set 0x12', work)
                if stelle:
                    st = slots[stelle[1]]
                    if 0x40 <= st['type'] <= 0x4d and st['x'] != -30000 and st['y'] == -st['band'] * 1800:
                        w = wort(cs, st['x'], st['z'], st['band'], 450)
                        if w & 2:
                            treffer += 1
                            print('%s %s 0x%05X %s Slot %d Typ %02X (%d,%d,%d) Band %d -> Absprungzelle Wort 0x%04X' % (
                                raum, 'main' if key == 0x40 else 'sub%d' % i, pc, stelle[0], stelle[1], st['type'],
                                st['x'], st['y'], st['z'], st['band'], w))
                pc += ln
                if op == 0x01 and (i + 1 == len(offs) or pc >= e - 3):
                    break
print('Treffer (NPC hingestellt in Absprungzelle auf Bandhoehe):', treffer)
