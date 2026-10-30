import struct,hashlib,sys
d=open('Version_40%/PSX.EXE','rb').read()
def toc(a):
    o=a-0x80010000+0x800
    return [struct.unpack_from('<II',d,o+8*i) for i in range(64)]
T={'L':toc(0x80072f38),'E':toc(0x80073178)}
A={'L':open('Version_40%/PSX/EMD/CDEMD0.EMS','rb').read(),'E':open('Version_40%/PSX/EMD/CDEMD1.EMS','rb').read()}
M={'L':open('info/Re1.5/PSX/EMD/CDEMD0.EMS','rb').read(),'E':open('info/Re1.5/PSX/EMD/CDEMD1.EMS','rb').read()}
dm=open('info/Re1.5/PSX.EXE','rb').read()
def tocm(a):
    o=a-0x80010000+0x800
    return [struct.unpack_from('<II',dm,o+8*i) for i in range(64)]
TM={'L':tocm(0x80072f38),'E':tocm(0x80073178)}
print('Typ  Leon-Archiv(Sektor,Laenge)   Elza-Archiv(Sektor,Laenge)  Blob gleich?  | im Mod-Stand gleich?')
for i in range(64):
    tl,te=T['L'][i],T['E'][i]; ml,me=TM['L'][i],TM['E'][i]
    if tl[1]==0 and te[1]==0 and ml[1]==0 and me[1]==0: continue
    t=0x10+i
    bl=A['L'][tl[0]*2048:tl[0]*2048+tl[1]] if tl[1] else b''
    be=A['E'][te[0]*2048:te[0]*2048+te[1]] if te[1] else b''
    x=M['L'][ml[0]*2048:ml[0]*2048+ml[1]] if ml[1] else b''
    y=M['E'][me[0]*2048:me[0]*2048+me[1]] if me[1] else b''
    print(f'0x{t:02X} L:{tl[1]:7d} E:{te[1]:7d}  {"gleich" if bl==be and bl else "VERSCHIEDEN"}   Mod L:{ml[1]:7d} E:{me[1]:7d}  L-unveraendert:{bl==x} E-unveraendert:{be==y}')
