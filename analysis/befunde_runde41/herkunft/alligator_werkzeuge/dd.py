# Wrapper um re15_disasm.py: dieselben Kommandos gegen DISC (Version_40%) oder MOD (info/Re1.5).
# Aufruf: python dd.py disc|mod dis 0x8010c448 40 --bin STAGE2.BIN
import sys,os,runpy,importlib.util
R=r'C:/workspace/git/reAi_v2'
w=sys.argv[1]; sys.argv=['re15_disasm.py']+sys.argv[2:]
base={'disc':R+'/Version_40%','mod':R+'/info/Re1.5'}[w]
os.environ['RE_OVERLAY_DIR']=base+'/PSX/BIN'
spec=importlib.util.spec_from_file_location('re15_disasm',R+'/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py')
m=importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
m.EXE_PATH=base+'/PSX.EXE'; m.OVERLAY_DIR=base+'/PSX/BIN'
if hasattr(m,'main'): m.main()
