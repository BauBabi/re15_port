import sys,capstone,struct
f=open(r'C:/workspace/git/reAi_v2/info/re2leon/PL0/PLD/CDEMD0.EMS','rb').read()
OFF=3238*2048; LEN=0xB2D4; BASE=0x80100000
ov=f[OFF:OFF+LEN]
md=capstone.Cs(capstone.CS_ARCH_MIPS,capstone.CS_MODE_MIPS32|capstone.CS_MODE_LITTLE_ENDIAN)
for arg in sys.argv[1:]:
    if arg.startswith('d'):   # Daten: dADR:N
        a,n=arg[1:].split(':'); a=int(a,16); n=int(n)
        o=a-BASE; print('DATA @0x%08x:'%a, ' '.join('%02x'%b for b in ov[o:o+n])); continue
    a,b=arg.split('-'); a=int(a,16); b=int(b,16)
    o=a-BASE
    for i in md.disasm(ov[o:b-BASE+4],a):
        print('0x%08x: %s  %-8s %s'%(i.address,i.bytes[::-1].hex(),i.mnemonic,i.op_str))
    print('---')
