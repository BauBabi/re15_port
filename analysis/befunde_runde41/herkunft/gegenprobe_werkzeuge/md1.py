import struct, hashlib, re
exec(open('sec.py').read().split('for k,(p,s,l) in F.items():')[0])
def md1(m):
    ln,unk,cnt=struct.unpack_from('<3I',m,0)
    base=12; objs=[]
    for i in range(cnt//2):
        t=struct.unpack_from('<7I',m,base+i*56); q=struct.unpack_from('<7I',m,base+i*56+28)
        def part(e):
            vo,vc,no,nc,po,pc,to=e
            return (m[base+vo:base+vo+vc*8], m[base+no:base+no+nc*8], vc, pc)
        objs.append((part(t),part(q)))
    return objs
D0=md1(secs(blob(*F['D0']))[7][1]); M1=md1(secs(blob(*F['M1']))[7][1])
print('Meshes D0',len(D0),'M1',len(M1))
for i in range(len(M1)):
    tv=M1[i][0][0]; qv=M1[i][1][0]
    eqD0= (tv==D0[i][0][0] and qv==D0[i][1][0]) if i<len(D0) else None
    print(i,'M1 triV',M1[i][0][2],'quadV',M1[i][1][2],'Tri',M1[i][0][3],'Quad',M1[i][1][3],'| Vertices = D0[%d]:'%i,eqD0)
print('M1 Mesh15 == Mesh16 (tri+quad Vertices):', M1[15][0][0]==M1[16][0][0] and M1[15][1][0]==M1[16][1][0])
