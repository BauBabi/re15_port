# NB1 M2/M3: Zensus aller Save work_vars[0x10] (= Spielermodell, Opcode 0x24 idx 0x10) in allen RDTs (opcode-exakter Dump).
import subprocess, sys, os, re, glob
BAUM = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_stage3szenen_b/re15_port'
DUMP = BAUM + '/tools/scd_dump_room.py'
rx = re.compile(r'^\s+0x([0-9A-F]+) \(\+[0-9A-F]+\) (\S+)\s+((?:[0-9a-f]{2} ?)+)')
for rdt in sorted(glob.glob(BAUM + '/shared_assets/PSX/STAGE*/ROOM*.RDT')):
    txt = subprocess.run([sys.executable, DUMP, rdt], capture_output=True, text=True).stdout
    sub = None
    for line in txt.splitlines():
        if line.startswith('=== '):
            sub = line.split()[1]
            continue
        m = rx.match(line)
        if not m:
            continue
        off, name, hx = m.group(1), m.group(2), m.group(3).split()
        b = [int(x, 16) for x in hx]
        if name == 'Save' and len(b) >= 4 and b[1] == 0x10:
            print('%-14s %-6s 0x%s  Save work_vars[0x10] = %d  (%s)' % (os.path.basename(rdt), sub, off, b[2] | (b[3] << 8), ' '.join(hx)))
