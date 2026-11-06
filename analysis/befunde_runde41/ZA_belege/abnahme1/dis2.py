# Wrapper: re15_disasm gegen info/Re1.5 (mod) oder Version_40% (disc)
import sys, os
REPO = r"C:\workspace\git\reAi_v2"
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as D
which = sys.argv.pop(1)   # mod | disc
if which == "disc":
    D.EXE_PATH = os.path.join(REPO, "Version_40%", "PSX.EXE")
    D.OVERLAY_DIR = os.path.join(REPO, "Version_40%", "PSX", "BIN")
D.main() if hasattr(D, "main") else None
