# Abnahme 7 (eigenes Werkzeug): MIPS-Disassembly des RE2-Overlays EM23_OVL_0000.BIN (Basis 0x80100000, kein Header) per capstone.
# Aufruf: python -I dis_em23.py <start_hex> <end_hex> [...]
import sys
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
BIN=r'C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN'
BASE=0x80100000
data=open(BIN,'rb').read()
md=Cs(CS_ARCH_MIPS, CS_MODE_MIPS32|CS_MODE_LITTLE_ENDIAN)
args=sys.argv[1:]
for i in range(0,len(args),2):
    a=int(args[i],16); b=int(args[i+1],16)
    print('==== %08x-%08x (Datei 0x%x-0x%x, Groesse %d) ===='%(a,b,a-BASE,b-BASE,len(data)))
    chunk=data[a-BASE:b-BASE]
    pos=a
    for ins in md.disasm(chunk,a):
        raw=data[ins.address-BASE:ins.address-BASE+4]
        print('%08x  %s  %-8s %s'%(ins.address,raw.hex(),ins.mnemonic,ins.op_str))
        pos=ins.address+4
    while pos<b:
        raw=data[pos-BASE:pos-BASE+4]
        print('%08x  %s  (undekodiert)'%(pos,raw.hex())); pos+=4
