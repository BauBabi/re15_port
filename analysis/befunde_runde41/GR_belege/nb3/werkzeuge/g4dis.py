import sys, struct, importlib.util
spec = importlib.util.spec_from_file_location("d", r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_granate/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py")
d = importlib.util.module_from_spec(spec); spec.loader.exec_module(d)
ems = open(r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_granate/re15_port/shared_assets/RE2/CDEMD0.EMS","rb").read()
sec = int(sys.argv[1]); size = int(sys.argv[2],0); a0 = int(sys.argv[3],0); n = int(sys.argv[4])
blob = ems[sec*2048: sec*2048+size]
for i in range(n):
    a = a0 + 4*i
    w = struct.unpack_from("<I", blob, a-0x80100000)[0]
    s, _ = d.dis_one(w, a)
    print("%08x: %08x  %s" % (a, w, s))
