# Sucht in allen STAGE*.BIN alle Schreiber in die Typ-Tabelle @0x80072bac (Disc + Mod) per lineare lui/addiu/sw-Verfolgung.
import struct,sys
R=r'C:/workspace/git/reAi_v2'
def scan(path):
    d=open(path,'rb').read()
    n=len(d)//4
    w=struct.unpack_from('<%dI'%n,d)
    reg={}
    out={}
    for i,x in enumerate(w):
        op=x>>26; rs=(x>>21)&31; rt=(x>>16)&31; imm=x&0xffff; s=imm-0x10000 if imm&0x8000 else imm
        a=0x80100000+i*4
        if op==0xf: reg[rt]=(imm<<16)&0xffffffff
        elif op==9 and rs in reg: reg[rt]=(reg[rs]+s)&0xffffffff
        elif op==0xd and rs in reg: reg[rt]=reg[rs]|imm
        elif op==0x2b and rs in reg:
            ea=(reg[rs]+s)&0xffffffff
            if 0x80072bac<=ea<0x80072bac+0x100*4:
                t=(ea-0x80072bac)//4
                out[t]=(a,reg.get(rt))
    return out
for v,b in (('disc',R+'/Version_40%/PSX/BIN'),('mod',R+'/info/Re1.5/PSX/BIN')):
    print('==',v)
    for s in (1,2,3,4,5,6):
        o=scan(f'{b}/STAGE{s}.BIN')
        print(f'STAGE{s}:',' '.join(f'{t:02X}={o[t][1] and hex(o[t][1])}@{o[t][0]:x}' for t in sorted(o)))
