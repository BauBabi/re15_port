# Zensus: jedes Plc_flg (0x43) mit Maske & 0x04 in allen RDTs; Ziel (Work_set-Kontext) und vorheriger Opcode.
import subprocess, sys, os, re, glob
BAUM = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_stage3szenen_b/re15_port'
DUMP = BAUM + '/tools/scd_dump_room.py'
rx = re.compile(r'^\s+0x([0-9A-F]+) \(\+[0-9A-F]+\) (\S+)\s+((?:[0-9a-f]{2} ?)+)')
out = []
for rdt in sorted(glob.glob(BAUM + '/shared_assets/PSX/STAGE*/ROOM*.RDT')):
    txt = subprocess.run([sys.executable, DUMP, rdt], capture_output=True, text=True).stdout
    sub = None; ziel = 'Spieler(Start)'; prev = []
    for line in txt.splitlines():
        if line.startswith('=== '):
            sub = line.split()[1]; ziel = 'Spieler(Start)'; prev = []
            continue
        m = rx.match(line)
        if not m: continue
        off, name, hx = m.group(1), m.group(2), m.group(3).split()
        b = [int(x, 16) for x in hx]
        if name == 'Work_set':
            kind, idx = b[1], b[2]
            ziel = 'Spieler' if kind == 1 else ('NPC%d' % idx if kind in (2, 3) else 'Obj%d/%d' % (kind, idx))
        if name == 'Plc_flg':
            sub_op, mask = b[1], b[2] | (b[3] << 8)
            if (mask & 0x04) and sub_op in (0, 1):
                out.append((os.path.basename(rdt), sub, '0x' + off, ziel, 'op%d m=0x%02x' % (sub_op, mask),
                            ' / '.join(prev[-3:])))
        prev.append('%s %s' % (name, ' '.join(hx[:4])))
for r in out:
    print('%-14s %-6s %-8s %-15s %-12s vorher: %s' % r)
print('Summe', len(out))
