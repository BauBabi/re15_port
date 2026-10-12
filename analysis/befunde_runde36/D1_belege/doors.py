import subprocess, sys, re
T="C:/workspace/git/reAi_v2/.claude/worktrees/r36_integration"
def s16(b,o): v=b[o]|(b[o+1]<<8); return v-65536 if v>=32768 else v
for r in sys.argv[1:]:
    out=subprocess.run([sys.executable,"-I",T+"/re15_port/tools/scd_dump_room.py",T+"/re15_port/shared_assets/PSX/STAGE1/ROOM%s.RDT"%r],capture_output=True,text=True).stdout
    print("== ROOM"+r)
    reg=None
    for line in out.splitlines():
        m=re.match(r"=== (\w+)",line)
        if m: reg=m.group(1)
        m=re.match(r"\s+0x([0-9A-F]+) \(\+\w+\) (Door_aot_set|Aot_set|Item_aot_set|Aot_reset|Obj_model_set)\s+((?:[0-9a-f]{2} )+)",line)
        if not m: continue
        off=m.group(1); op=m.group(2); b=bytes(int(x,16) for x in m.group(3).split())
        if op=="Door_aot_set":
            print("  %s @0x%s DOOR slot%d sce%d sat0x%02x fl%d rect x%d..%d z%d..%d -> stage%d room%02X pos(%d,%d,%d) rot%d cut%d nfl%d" % (reg,off,b[1],b[2],b[3],b[4],s16(b,6),s16(b,6)+s16(b,10),s16(b,8),s16(b,8)+s16(b,12),b[22],b[23],s16(b,14),s16(b,16),s16(b,18),s16(b,20),b[24],b[25]))
        elif op in("Aot_set","Item_aot_set"):
            print("  %s @0x%s %s slot%d sce%d sat0x%02x fl%d rect x%d..%d z%d..%d p=%s" % (reg,off,op,b[1],b[2],b[3],b[4],s16(b,6),s16(b,6)+s16(b,10),s16(b,8),s16(b,8)+s16(b,12),b[14:].hex()))
        elif op=="Obj_model_set":
            print("  %s @0x%s OBJ %s" % (reg,off,line.split("obj=")[1] if "obj=" in line else ""))
