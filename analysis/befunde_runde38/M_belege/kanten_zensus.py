# Zensus: NPC-Plc_dest-Walk/Run (Modi 4/5) ueber SCA-Kantenzellen (u1&3) im eigenen Band.
# Lineare SCD-Dekodierung (keine Kontrollfluss-Simulation): Work_set(2,n) waehlt den Slot,
# Sce_em_set/Pos_set/Member_set(0x12) setzen Startpunkt/Band, Plc_dest 4/5 laeuft gerade zum Ziel.
# Probe wie FUN_8002d7d8 @0x8002d7d8: Punkt = pos + 800 * Kardinalrichtung((rot+512)&~1023),
# Zellwort wie FUN_8003b7f0 (Band = sign. Nibble floor>>4), Ergebnis 1 = Bit0, 2 = Bit1 && y < Boden.
import glob, struct, sys, math
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_npckante/analysis/nutzer_batch_2026-08-27/tools')
import re15_scd_dis as D
L=D.lens()
def s16(b,o): return struct.unpack_from('<h',b,o)[0]
def sca_cells(d):
    off=struct.unpack_from('<I',d,0x20)[0]
    if off==0 or off+24>len(d): return [],0,0,[0]*5
    cx,cz=struct.unpack_from('<hh',d,off); cnt=list(struct.unpack_from('<5I',d,off+4))
    cells=[]; p=off+24
    for g in range(5):
        for i in range(cnt[g]):
            if p+12>len(d): break
            cells.append((g,)+struct.unpack_from('<HHhhBBBB',d,p)); p+=12
    return cells,cx,cz,cnt
def quadrant(x,z,cx,cz):
    # FUN_8003b068 (Port quadrant_of): 0..4 -- vereinfachte Nachbildung ueber den Port-Code lesen
    return None
def typeword(cells,x,z,band):
    for (g,w,dd,cxx,czz,t,u0,u1,fl) in cells:
        wd=(u1|(fl<<8)); cb=((wd<<16)&0xffffffff); cb = (cb if cb<0x80000000 else cb-0x100000000)>>28
        if cb!=band: continue
        if 0<= x-cxx < w and 0<= z-czz < dd: return wd
    return 0
def room_coll(cells,x,z,r,start_band,mask):
    for b in range(start_band-1,-1,-1):
        for (g,w,dd,cxx,czz,t,u0,u1,fl) in cells:
            w10=(u1|(fl<<8)); w08=(t|(u0<<8))
            w10s = w10-0x10000 if w10&0x8000 else w10
            if (w10s & 0xf002)!=(b<<12): continue
            w08s = w08-0x10000 if w08&0x8000 else w08
            if (w08s & mask)==0: continue
            if 0<= x-(cxx+r) < (w-2*r) and 0<= z-(czz+r) < (dd-2*r): return (b+1)*-0x708
    return 0
def probe(cells,x,y,z,rot,band):
    q=((rot+512)&~1023)&0xfff
    dx={0:800,1024:0,2048:-800,3072:0}[q]; dz={0:0,1024:-800,2048:0,3072:800}[q]
    px,pz=x+dx,z+dz
    w=typeword(cells,px,pz,band)
    if w&1: return 1,px,pz,w
    if w&2:
        h=room_coll(cells,px,pz,1,band,0x100)
        if y<h: return 2,px,pz,w
    return 0,px,pz,w
def yaw_to(x,z,tx,tz):
    # Port: x += cos(yaw)*sp, z -= sin(yaw)*sp  ->  yaw = atan2(-(tz-z), tx-x)
    a=math.atan2(-(tz-z),(tx-x)); return int(round(a/(2*math.pi)*4096))&0xfff
out=[]
for f in sorted(glob.glob(r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_npckante/re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT')):
    d=open(f,'rb').read()
    if len(d)<0x48: continue
    cells,cx,cz,cnt=sca_cells(d)
    if not cells: continue
    import os; room=os.path.basename(f)[:-4]
    slots={}  # slot -> dict(type,x,y,z,band,rot)
    work=None
    for key in (0x40,0x44):
        s=struct.unpack_from('<I',d,key)[0]
        if s==0 or s>=len(d): continue
        n=struct.unpack_from('<H',d,s)[0]//2
        offs=[struct.unpack_from('<H',d,s+2*i)[0] for i in range(n)]
        for i,o in enumerate(offs):
            pc=s+o; e=s+offs[i+1] if i+1<len(offs) else len(d)
            while pc<e and pc<len(d):
                op=d[pc]; ln=L[op] or 1
                if op==0x2D: ln=34
                if op==0x44 and pc+18<len(d):
                    sl=d[pc+1]; ty=d[pc+2]; fl=d[pc+4]
                    slots[sl]=dict(type=ty,x=s16(d,pc+8),y=s16(d,pc+10),z=s16(d,pc+12),band=fl,rot=s16(d,pc+16)&0xfff)
                elif op==0x2E:
                    work = d[pc+2] if d[pc+1]==2 else None
                elif op==0x32 and work is not None and work in slots:
                    st=slots[work]; st['x']=s16(d,pc+2); st['y']=s16(d,pc+4); st['z']=s16(d,pc+6)
                elif op==0x34 and work is not None and work in slots:
                    if d[pc+1]==0x12: slots[work]['band']=d[pc+2]
                    if d[pc+1]==0x04: slots[work]['rot']=struct.unpack_from('<H',d,pc+2)[0]&0xfff
                elif op==0x40 and work is not None and work in slots:
                    mode=d[pc+2]; tx=s16(d,pc+4); tz=s16(d,pc+6); st=slots[work]
                    if 0x40<=st['type']<=0x4d and mode in (4,5):
                        sp=75 if mode==4 else 200
                        x,z=st['x'],st['z']; y=st['y']; band=st['band']
                        for k in range(2000):
                            dist=math.hypot(tx-x,tz-z)
                            if dist<(300 if mode==5 else 100): break
                            rot=yaw_to(x,z,tx,tz)
                            x+=int(math.cos(rot/4096*2*math.pi)*sp); z-=int(math.sin(rot/4096*2*math.pi)*sp)
                            r,px,pz,w=probe(cells,x,y,z,rot,band)
                            if r:
                                out.append('%s sub%s 0x%05X slot%d typ%02X mode%d band%d y%d  %s an (%d,%d) Probe(%d,%d) Wort 0x%04X'%(room,i if key==0x44 else 'M',pc,work,st['type'],mode,band,y,'KLETTERN' if r==1 else 'SPRUNG',x,z,px,pz,w))
                                if r==2:
                                    h=room_coll(cells,px,pz,0,band,0x100); y=h; band=-(h//0x708) if h else 0
                                    x,z=px,pz
                                else:
                                    y-=0x708; band+=1; x,z=px,pz
                        st['x'],st['z'],st['y'],st['band']=tx,tz,y,band
                pc+=ln
                if op==0x01 and (i+1==len(offs) or pc>=e-3): break
print('\n'.join(out)); print('Treffer:',len(out))
