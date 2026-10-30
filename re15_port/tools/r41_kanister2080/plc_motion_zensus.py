# Runde 41 Spur G, Nachbesserung 1 (M1): Zensus aller Plc_motion (0x3F) / Plc_flg (0x43) in allen RE1.5-RDTs mit dem
# Work_set-Ziel und dessen Sce_em_set-Typ. Aufruf: python plc_motion_zensus.py <re15_port/tools> <shared_assets/PSX> [typ,typ]
# Zensus: alle Plc_motion (0x3F) / Plc_flg (0x43) in allen RE1.5-RDTs mit dem Work_set-Ziel und dessen Sce_em_set-Typ.
import sys, os, glob, collections
sys.path.insert(0, sys.argv[1])
import scd_walk_lib as L
root = sys.argv[2]
want_types = set(int(t, 0) for t in sys.argv[3].split(',')) if len(sys.argv) > 3 else None
rows = []
for p in sorted(glob.glob(os.path.join(root, 'STAGE*', 'ROOM*.RDT'))):
    d = open(p, 'rb').read()
    room = os.path.basename(p)[4:8]
    try:
        reg = L.regionen(d)
    except Exception as ex:
        continue
    slot_typ = {}
    for key, ops in reg.items():
        for (pc, op, sz) in ops:
            if op == 0x44:
                slot_typ.setdefault(d[pc+1], set()).add(d[pc+2])
    for key, ops in sorted(reg.items()):
        tgt = None
        for (pc, op, sz) in ops:
            if op == 0x2e:
                tgt = (d[pc+1], d[pc+2])
            elif op in (0x3f, 0x43):
                typ = None
                if tgt and tgt[0] == 2:
                    typ = slot_typ.get(tgt[1])
                rows.append((room, key[0], key[1], pc, op, d[pc+1], d[pc+2], d[pc+3], tgt, typ))
for r in rows:
    room, tag, idx, pc, op, a, b, c, tgt, typ = r
    ts = ','.join('%02x' % t for t in sorted(typ)) if typ else '-'
    if want_types is not None:
        if not typ or not (set(typ) & want_types):
            continue
    print('%s %s%02d @0x%05X %s %02x %02x %02x  work=%s typ=%s' % (room, tag, idx, pc, 'MOT' if op == 0x3f else 'FLG', a, b, c, tgt, ts))
