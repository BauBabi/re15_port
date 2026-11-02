import struct, capstone
R='C:/workspace/git/reAi_v2/'
exe=open(R+'Version_40%/PSX.EXE','rb').read(); s2=open(R+'Version_40%/PSX/BIN/STAGE2.BIN','rb').read()
md=capstone.Cs(capstone.CS_ARCH_MIPS,capstone.CS_MODE_MIPS32+capstone.CS_MODE_LITTLE_ENDIAN)
def code(a):
    if a>=0x80100000: return s2[a-0x80100000:]
    return exe[a-0x80010000+0x800:]
def jals(a,maxn=1500):
    out=set(); jr=0
    for i in md.disasm(code(a)[:4*maxn],a):
        if i.mnemonic=='jal': out.add(int(i.op_str,16))
        if i.mnemonic=='jr' and i.op_str=='$ra': break
    return out
TON={0x800453d0,0x80045024,0x80041624}
start=[0x8001f314,0x8001af20,0x80019700,0x800245d8,0x8010ee40,0x8001c1a4,0x8001a9cc,0x8001c0dc,0x8001bff8,0x8001a8f8,0x8001a804,0x8001a780,0x8010e91c,0x80065f60,0x8004ef90,0x8003b0a4,0x8002b544,0x8002b498,0x8002aec4,0x800279c8,0x8001bc08,0x8001b064,0x8001af5c,0x8001aa68,0x80012974]
seen={}; front=[(a,0) for a in start]
while front:
    a,dpt=front.pop()
    if a in seen or dpt>3: continue
    j=jals(a); seen[a]=j
    for t in j: front.append((t,dpt+1))
hits=[(a,sorted(hex(x) for x in j&TON)) for a,j in seen.items() if j&TON]
print('Funktionen bis Tiefe 3:',len(seen),' mit Ton-Aufruf:',[(hex(a),h) for a,h in hits])
