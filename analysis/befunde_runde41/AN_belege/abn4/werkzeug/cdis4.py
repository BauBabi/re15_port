"""cdis4.py - re2_disasm.py (Skill re15-psx-disasm) gegen das Claire-EXE SLES_109.72 (Pfad als 1. Argument)."""
import sys, importlib.util
spec = importlib.util.spec_from_file_location("d", r"C:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re2_disasm.py")
d = importlib.util.module_from_spec(spec); spec.loader.exec_module(d)
d.EXE_PATH = sys.argv[1]
sys.argv = ["x"] + sys.argv[2:]; d.main()
