"""Loest die Ziffern-Kette der drei RE1.5-Kartenleser auf:
welche Taste (Member 0x0F) setzt welchen Riegel 5.13..5.16?

sub01 gatet jede Taste mit `Ck(5,1,1) + Member_cmp(0x0F == k) + Sce_key_ck(0x40) -> sub N`;
sub N prueft ein Ketten-Bit und setzt den naechsten Riegel. Die Reihenfolge der Riegel
13->14->15->16 ist die Reihenfolge der vier Ziffern."""
import os, sys, glob, collections
HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.normpath(os.path.join(HERE, "..", "..", "..", "re15_port", "tools"))
sys.path.insert(0, TOOLS)
from scd_walk_lib import (u16, u32, op_size, fwd_target, rdt_section_end,
                          section_regions, walk_ops, regionen, messages, NAMES)


ROOT = os.path.normpath(os.path.join(HERE, "..", "..", "..", "re15_port", "shared_assets", "PSX"))
RAEUME = ["STAGE1/ROOM10D0", "STAGE1/ROOM10D1", "STAGE1/ROOM1230",
          "STAGE1/ROOM1231", "STAGE1/ROOM11E0", "STAGE1/ROOM11E1"]

for r in RAEUME:
    p = os.path.join(ROOT, r + ".RDT")
    d = open(p, "rb").read()
    ss = u32(d, 0x44)
    se = rdt_section_end(d, ss)
    regs = {}
    for (o, e, idx) in section_regions(d, ss, se):
        regs[idx] = walk_ops(d, ss + o, ss + e)

    # sub01: Taste k -> sub N
    taste = {}
    ops = regs.get(1, [])
    for i, (pc, op, sz) in enumerate(ops):
        if op != 0x3E or d[pc + 2] != 0x0F:
            continue
        k = u16(d, pc + 4)
        for (pc2, op2, sz2) in ops[i:i + 4]:
            if op2 == 0x04:
                taste[k] = d[pc2 + 3]
                break

    # sub N: welchen Riegel setzt sie, und welches Kettenbit prueft/setzt sie
    info = {}
    for n in sorted(set(taste.values())):
        o = regs.get(n, [])
        riegel, prueft, armiert = None, None, []
        for (pc, op, sz) in o:
            if op == 0x21 and d[pc + 1] == 5 and prueft is None:
                prueft = (d[pc + 2], d[pc + 3])
            if op == 0x22 and d[pc + 1] == 5:
                b, v = d[pc + 2], d[pc + 3]
                if 13 <= b <= 16 and v == 1:
                    riegel = b
                elif 3 <= b <= 12 and v == 1:
                    armiert.append(b)
        info[n] = (riegel, prueft, armiert)

    # Kette: Riegel 13 zuerst
    print("\n=== %s" % r)
    nach_riegel = {}
    for k, n in sorted(taste.items()):
        riegel, prueft, armiert = info.get(n, (None, None, []))
        print("   Taste member0x0F==%-2d -> sub%-2d  prueft 5.%s==%s  setzt Riegel %s  armiert %s"
              % (k, n, prueft[0] if prueft else "-", prueft[1] if prueft else "-",
                 riegel if riegel else "-", armiert))
        if riegel:
            nach_riegel[riegel] = k
    kette = [nach_riegel.get(b) for b in (13, 14, 15, 16)]
    print("   ==> Tastenfolge (Riegel 13,14,15,16): %s" % kette)
