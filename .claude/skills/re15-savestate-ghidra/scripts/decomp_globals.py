# Ghidra post-script (Jython/Py2): decompile every function that references a set
# of RAM globals, to recover their meaning/struct layout symbolically. This is the
# "static half" of the savestate workflow -- you feed it the address a differential
# savestate diff flagged, and it tells you which function reads/writes it and how.
#
# Run via analyzeHeadless:
#   analyzeHeadless <proj> re15 -import code.bin -processor "MIPS:LE:32:default" \
#       -loader BinaryLoader -loader-baseAddr 0x80010000 \
#       -scriptPath <dir> -postScript decomp_globals.py <out.c> <addr> [<addr> ...] \
#       -deleteProject
# (code.bin = PSX.EXE with its 0x800-byte header stripped: tail -c +2049 PSX.EXE)
from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor

args = getScriptArgs()
OUT = args[0] if len(args) >= 1 else "room_funcs.c"
if len(args) >= 2:
    TARGETS = [int(a, 16) for a in args[1:]]
else:
    TARGETS = [0x800b5328, 0x800b532c, 0x800b532e, 0x800b5330]  # camera-cut struct (example)

fm = currentProgram.getFunctionManager()
refman = currentProgram.getReferenceManager()
sp = currentProgram.getAddressFactory().getDefaultAddressSpace()

eps = {}
for t in TARGETS:
    for ref in refman.getReferencesTo(sp.getAddress(t)):
        f = fm.getFunctionContaining(ref.getFromAddress())
        if f is not None:
            eps[f.getEntryPoint().getOffset()] = f

deco = DecompInterface(); deco.openProgram(currentProgram)
mon = ConsoleTaskMonitor()
out = open(OUT, "w")
out.write("// functions referencing: %s\n" % ", ".join("0x%08x" % t for t in TARGETS))
out.write("// entry points: %s\n\n" % ", ".join("0x%08x" % e for e in sorted(eps)))
for ep in sorted(eps):
    res = deco.decompileFunction(eps[ep], 120, mon)
    out.write("// ============ FUN_%08x ============\n" % ep)
    c = res.getDecompiledFunction().getC() if (res and res.getDecompiledFunction()) else "// <decompile failed>\n"
    out.write(c + "\n\n")
out.close()
print("WROTE %d functions to %s" % (len(eps), OUT))
