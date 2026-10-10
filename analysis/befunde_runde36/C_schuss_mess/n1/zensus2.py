import struct, re, sys, importlib.util
spec = importlib.util.spec_from_file_location("d", r"C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss/.claude/skills/re15-psx-disasm/scripts/re2_disasm.py")
BIN = r"C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss/info/re2leon/COMMON/BIN/EMZ0.BIN"
import subprocess
data = open(BIN, "rb").read()
JAL = 0x0C000000 | ((0x8001bf10 & 0x0fffffff) >> 2)
sites = [0x80100000 + i for i in range(0, len(data) - 3, 4) if struct.unpack_from("<I", data, i)[0] == JAL]
D = r"C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss/.claude/skills/re15-psx-disasm/scripts/re2_disasm.py"
for s in sites:
    out = subprocess.run([sys.executable, D, "dis", hex(s - 4 * 60), "62", "--bin", "EMZ0.BIN"], capture_output=True, text=True).stdout
    lines = [l.strip() for l in out.splitlines() if re.match(r"\s*[0-9a-f]{8}:", l)]
    # Fenster: ab dem letzten anderen jal vor der Stelle (Delay-Slot der Stelle mitnehmen)
    idx = max(i for i, l in enumerate(lines) if l.startswith("%08x:" % s))
    start = 0
    for i in range(idx - 1, -1, -1):
        if re.search(r": jal ", lines[i]) or re.search(r": jr ra", lines[i]):
            start = i + 2; break
    win = lines[start: idx + 2]
    a3 = [l for l in win if re.search(r" a3,", l)]
    a2 = [l for l in win if re.search(r" a2,", l)]
    st = [l for l in win if re.search(r"(24|26|28)\(sp\)", l) and re.search(r": sh ", l)]
    a3s = " | ".join(x.split(": ",1)[1].strip() for x in a3[-1:])
    a2s = " | ".join(x.split(": ",1)[1].strip() for x in a2[-1:])
    sts = " ; ".join(x.split(": ",1)[1].strip() for x in st[-3:])
    print("%08x a3[%s] a2[%s] sp[%s]%s" % (s, a3s, a2s, sts, "" if start else " (Fenster voll)"))
