import struct,re
R=r'C:/workspace/git/reAi_v2'
d=open(R+'/Version_40%/PSX.EXE','rb').read()
A=lambda o:0x80010000+o-0x800
n=len(d)//4
w=struct.unpack_from('<%dI'%n,d)
lo,hi=A(0x60700),A(0x62300)
print(hex(lo),hex(hi))
hits=[(hex(A(i*4)),hex(x)) for i,x in enumerate(w) if lo<=x<hi]
print('Datenzeiger',hits[:40])
# lui/addiu Referenzen
code=[]
for i,x in enumerate(w):
    if x>>26==0xf:
        rt=(x>>16)&31; h=x&0xffff
        for j in range(i+1,min(i+10,n)):
            y=w[j]
            op=y>>26
            if op in (9,0x23,0x24,0x25,0x20,0x21) and ((y>>21)&31)==rt:
                imm=y&0xffff; s=imm-0x10000 if imm&0x8000 else imm
                ea=((h<<16)+s)&0xffffffff
                if lo<=ea<hi: code.append((hex(A(i*4)),hex(A(j*4)),hex(ea),op))
                break
print('Code',code[:40])
