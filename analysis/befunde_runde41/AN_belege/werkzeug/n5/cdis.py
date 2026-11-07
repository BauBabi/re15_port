import sys, importlib.util
spec = importlib.util.spec_from_file_location("d", r"C:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re2_disasm.py")
d = importlib.util.module_from_spec(spec); spec.loader.exec_module(d)
d.EXE_PATH = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/an/cdisc/SLES_109.72"
sys.argv = ["x"] + sys.argv[1:]; d.main()
