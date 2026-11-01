# Disassembliert info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN (Basis 0x80100000) mit dis_one aus re15_disasm.py
import struct,sys,importlib.util
R=r'C:/workspace/git/reAi_v2'
spec=importlib.util.spec_from_file_location('d',R+'/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py'); m=importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
d=open(R+'/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN','rb').read()
w=struct.unpack_from('<%dI'%(len(d)//4),d)
base=0x80100000
with open('re2_em23.dis','w') as f:
    for i,x in enumerate(w):
        t,_=m.dis_one(x,base+i*4)
        f.write(f'{base+i*4:08x}: {t}\n')
print(len(w))
