# Zensus 2: fuer jedes Spieler-Plc_flg mit Bit 0x04 den naechsten Spieler-Befehl (Plc_motion/Plc_flg/Plc_dest/Plc_ret)
# im selben Sub suchen: wird Bit 0x04 vor dem naechsten Plc_motion/Plc_dest/Plc_ret wieder geloescht?
import subprocess, sys, os, re, glob
BAUM = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_stage3szenen_b/re15_port'
DUMP = BAUM + '/tools/scd_dump_room.py'
rx = re.compile(r'^\s+0x([0-9A-F]+) \(\+[0-9A-F]+\) (\S+)\s+((?:[0-9a-f]{2} ?)+)')
for rdt in sorted(glob.glob(BAUM + '/shared_assets/PSX/STAGE*/ROOM*.RDT')):
    room = os.path.basename(rdt)
    if room not in ('ROOM1011.RDT','ROOM1031.RDT','ROOM11B0.RDT','ROOM2020.RDT','ROOM2021.RDT','ROOM3061.RDT','ROOM3091.RDT','ROOM6031.RDT'): continue
    txt = subprocess.run([sys.executable, DUMP, rdt], capture_output=True, text=True).stdout
    sub=None; ziel='S'; offen=None
    for line in txt.splitlines():
        if line.startswith('=== '):
            if offen: print(room, sub, offen, '-> Sub-Ende ohne Folgebefehl')
            sub=line.split()[1]; ziel='S'; offen=None; continue
        m=rx.match(line)
        if not m: continue
        off,name,b=m.group(1),m.group(2),[int(x,16) for x in m.group(3).split()]
        if name=='Work_set': ziel='S' if b[1]==1 else 'N'
        if ziel!='S': continue
        if name=='Plc_flg':
            sop,mask=b[1],b[2]|(b[3]<<8)
            if offen and (sop==1 and not mask&4 or sop==2 and mask&4):
                print(room, sub, offen, '-> LOESCHT Bit 4 @0x'+off, 'op%d m=0x%x'%(sop,mask)); offen=None
            elif (mask&4) and sop in (0,1): offen='Plc_flg4@0x'+off
        elif name in ('Plc_motion','Plc_dest','Plc_ret') and offen:
            print(room, sub, offen, '-> naechster Spielerbefehl', name, '@0x'+off); offen=None
