import struct
A=open(r"C:\workspace\git\reAi_v2\Version_40%\PSX.EXE","rb").read()
B=open(r"C:\workspace\git\reAi_v2\info\Re1.5\PSX.EXE","rb").read()
ta=struct.unpack_from("<I",A,0x18)[0]; tb=struct.unpack_from("<I",B,0x18)[0]
print("t_addr",hex(ta),hex(tb),"t_size",hex(struct.unpack_from("<I",A,0x1c)[0]),hex(struct.unpack_from("<I",B,0x1c)[0]))
print("hdr diff", [i for i in range(0x800) if A[i]!=B[i]][:40])
diffs=[]
for off in range(0x800,len(A),4):
    if A[off:off+4]!=B[off:off+4]: diffs.append(ta+off-0x800)
# group
grp=[];s=None;p=None
for a in diffs:
    if s is None: s=p=a
    elif a-p<=0x20: p=a
    else: grp.append((s,p)); s=p=a
if s is not None: grp.append((s,p))
print(len(diffs),"words,",len(grp),"groups")
for s,p in grp: print(hex(s),hex(p),(p-s)//4+1)
