import struct, hashlib, os
R='C:/workspace/git/reAi_v2/'
exec(open('sec.py').read().split('for k,(p,s,l) in F.items():')[0])
img=open(R+'Version_40%/BH1.5-40%.bin','rb').read()
ns=len(img)//2352
print('Sektoren',ns, 'Rest',len(img)%2352)
ud=bytearray()
for k in range(ns):
    sec=img[k*2352:(k+1)*2352]
    ud+=sec[24:24+2048]
ud=bytes(ud)
# ISO9660: PVD Sektor 16, Root dir
pvd=ud[16*2048:17*2048]; print('PVD',pvd[1:6], pvd[40:72].strip())
def walk(lba,size,path,out):
    d=ud[lba*2048:lba*2048+size]; i=0
    while i<len(d):
        L=d[i]
        if L==0: i=(i//2048+1)*2048; continue
        el,=struct.unpack_from('<I',d,i+2); es,=struct.unpack_from('<I',d,i+10); fl=d[i+25]; nl=d[i+32]; nm=d[i+33:i+33+nl]
        if nm not in (b'\x00',b'\x01'):
            n=nm.decode('ascii','replace').split(';')[0]
            if fl&2: walk(el,es,path+'/'+n,out)
            else: out.append((path+'/'+n,el,es))
        i+=L
rl,=struct.unpack_from('<I',pvd,156+2); rs,=struct.unpack_from('<I',pvd,156+10)
out=[]; walk(rl,rs,'',out)
print('Dateien im Abbild',len(out))
neq=[];miss=[]
for p,l,s in out:
    lp=R+'Version_40%'+p
    if not os.path.exists(lp): miss.append(p); continue
    if open(lp,'rb').read()!=ud[l*2048:l*2048+s]: neq.append(p)
print('fehlen im Baum',miss[:5],' ungleich',neq[:10])
M1=secs(blob(*F['M1']))
x=M1[8][1]; tot=0;h=0
for o in range(0,len(x)-64,1600):
    w=x[o:o+64]
    if len(set(w))<6: continue
    tot+=1; h+= (w in ud)
print('M1-TIM Stichproben im Abbild (Nutzdaten):',h,'/',tot)
x=M1[7][1]; tot=0;h=0
for o in range(0,len(x)-64,600):
    w=x[o:o+64]
    if len(set(w))<6: continue
    tot+=1; h+= (w in ud)
print('M1-MD1 Stichproben im Abbild:',h,'/',tot)
