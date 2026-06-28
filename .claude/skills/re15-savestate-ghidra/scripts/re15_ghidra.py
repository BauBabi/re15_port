#!/usr/bin/env python3
"""re15_ghidra.py - one-shot Ghidra decompile of every RE1.5 PSX.EXE function
that references a given RAM global. Wraps the analyzeHeadless invocation +
decomp_globals.py post-script, caches the Ghidra project (fast re-runs), and
prints the decompiled C. This is the "name a candidate" half of the workflow;
feed it addresses that re15_diff.py flagged.

Usage:
  re15_ghidra.py 0x800b532e [0x800b5330 ...] [--out file.c] [--reanalyze]

First run imports + analyzes code.bin (~25 s); subsequent runs reuse the cached
project (-process -noanalysis, a few seconds). --reanalyze forces a fresh import.
"""
import sys, os, subprocess, tempfile, argparse

PSX_EXE = r"C:\workspace\git\reAi_v2\info\Re1.5\PSX.EXE"
GHIDRA  = r"C:\Users\mjoedicke\Downloads\ghidra_11.4.2_PUBLIC_20250826\ghidra_11.4.2_PUBLIC\support\analyzeHeadless.bat"
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CACHE = os.path.join(tempfile.gettempdir(), "re15_ghidra_cache")
PROJ_NAME = "re15"

def ensure_codebin():
    cb = os.path.join(CACHE, "code.bin")
    if not os.path.exists(cb):
        os.makedirs(CACHE, exist_ok=True)
        with open(PSX_EXE, "rb") as f:
            f.seek(0x800)                     # strip the PS-X EXE header
            open(cb, "wb").write(f.read())
    return cb

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("addrs", nargs="+")
    ap.add_argument("--out", default=os.path.join(CACHE, "decomp_out.c"))
    ap.add_argument("--reanalyze", action="store_true")
    args = ap.parse_args()

    if not os.path.exists(GHIDRA):
        sys.exit("analyzeHeadless not found: %s" % GHIDRA)
    code = ensure_codebin()
    proj = os.path.join(CACHE, "proj")
    os.makedirs(proj, exist_ok=True)
    analyzed = os.path.exists(os.path.join(proj, PROJ_NAME + ".gpr")) and not args.reanalyze

    post = ["-scriptPath", SCRIPT_DIR, "-postScript", "decomp_globals.py", args.out] + list(args.addrs)
    if analyzed:
        cmd = [GHIDRA, proj, PROJ_NAME, "-process", "code.bin", "-noanalysis"] + post
        mode = "process (cached)"
    else:
        cmd = [GHIDRA, proj, PROJ_NAME, "-import", code,
               "-processor", "MIPS:LE:32:default", "-loader", "BinaryLoader",
               "-loader-baseAddr", "0x80010000"] + post
        mode = "import + analyze (~25 s)"
    print("[re15_ghidra] %s ; targets: %s" % (mode, " ".join(args.addrs)))
    r = subprocess.run(cmd, capture_output=True, text=True)
    tail = [l for l in (r.stdout + r.stderr).splitlines()
            if any(k in l for k in ("WROTE", "ERROR", "Exception", "succeeded"))]
    print("\n".join(tail[-6:]))
    if os.path.exists(args.out):
        print("\n===== %s =====" % args.out)
        print(open(args.out).read())
    else:
        sys.exit("no output produced")

if __name__ == "__main__":
    main()
