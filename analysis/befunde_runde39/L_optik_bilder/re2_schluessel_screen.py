# RE2-Vorbild: Bildgroesse der RE2-Schluesselmodelle an ihren Original-Platzierungen (Kamera-Saetze RE2 RDT @0x64, 32 B
# wie RE1.5: +0 flag, +2 fov, +4 from xyz, +0x10 to xyz, +0x1c pri). Kamera wie re15_camera_build_view (camera_common.c).
import struct, math
RE2=r"C:\workspace\git\reAi_v2\.claude\worktrees\r39_optik\info\re2leon\PL0\RDT"
def sqrt0(v): return int(math.sqrt(v))
def build_view(fx,fy,fz,tx,ty,tz,fov):
    dx,dy,dz=tx-fx,ty-fy,tz-fz
    dist=sqrt0(dx*dx+dy*dy+dz*dz); horiz=sqrt0(dx*dx+dz*dz)
    sp=int((-dy)*4096/dist); cp=int(horiz*4096/dist)
    sy=int(dx*4096/horiz); cy=int(dz*4096/horiz)
    r=[cy,0,-sy,(sp*sy)>>12,cp,(sp*cy)>>12,(cp*sy)>>12,-sp,(cp*cy)>>12]
    t=[(r[0]*-fx+r[1]*-fy+r[2]*-fz)>>12,(r[3]*-fx+r[4]*-fy+r[5]*-fz)>>12,(r[6]*-fx+r[7]*-fy+r[8]*-fz)>>12]
    return r,t,fov>>7
def proj(view,p):
    r,t,H=view
    vx=(r[0]*p[0]+r[1]*p[1]+r[2]*p[2])/4096+t[0]; vy=(r[3]*p[0]+r[4]*p[1]+r[5]*p[2])/4096+t[1]; vz=(r[6]*p[0]+r[7]*p[1]+r[8]*p[2])/4096+t[2]
    if vz<=0: return None
    return 160+vx*H/vz,120+vy*H/vz,vz
def rot(p,ang):  # Q12-Winkel (4096=360), Reihenfolge wie der Port-Prop (rx, dann ry, dann rz) - fuer die Groesse unerheblich
    x,y,z=p
    for axis,a in (("x",ang[0]),("y",ang[1]),("z",ang[2])):
        c,s=math.cos(a*2*math.pi/4096),math.sin(a*2*math.pi/4096)
        if axis=="x": y,z=y*c-z*s,y*s+z*c
        elif axis=="y": x,z=x*c+z*s,-x*s+z*c
        else: x,y=x*c-y*s,x*s+y*c
    return (x,y,z)
def cuts(room):
    b=open(RE2+"\ROOM%s.RDT"%room,"rb").read()
    n=b[1]; off=struct.unpack_from("<I",b,0x24)[0]; out=[]
    for i in range(n):
        fl,fov,fx,fy,fz,tx,ty,tz,pri=struct.unpack_from("<HHiiiiiiI",b,off+32*i)
        out.append((fov,fx,fy,fz,tx,ty,tz,pri))
    return out
# Platzierungen: (Raum, Objekt-Satz-Offset, Modell, sichtbare natuerliche Masse L x W)
P=[("10F0",0x706,"Small Key",138.8,65.1),("20E0",0x18f8,"Small Key",138.8,65.1),("20E0",0x1a96,"Small Key",138.8,65.1),
   ("2020",0x1c1e,"Small Key",138.8,65.1),("6150",0x2c98,"P. Room Key",113.7,72.4)]
for room,o,name,L,W in P:
    b=open(RE2+"\ROOM%s.RDT"%room,"rb").read()
    attr,x,y,z,dx,dy,dz=struct.unpack_from("<hhhhhhh",b,o+12)
    print("RE2 ROOM%s @0x%04X %s attr 0x%04X pos (%d,%d,%d) dir (%d,%d,%d)"%(room,o,name,attr&0xffff,x,y,z,dx,dy,dz))
    for ci,(fov,fx,fy,fz,tx,ty,tz,pri) in enumerate(cuts(room)):
        v=build_view(fx,fy,fz,tx,ty,tz,fov)
        c=proj(v,(x,y,z))
        if not c: continue
        sx,sy,vz=c
        if not (0<=sx<320 and 0<=sy<240): continue
        H=v[2]
        corners=[rot((ex*L/2,0,ez*W/2),(dx,dy,dz)) for ex in(-1,1) for ez in(-1,1)]
        pts=[proj(v,(x+cx,y+cy,z+cz)) for cx,cy,cz in corners]
        xs=[p[0] for p in pts]; ys=[p[1] for p in pts]
        print("   Cut %d fov %d H %d: Schluessel bei (%.1f,%.1f) vz %.0f | L*H/vz = %.2f px | gezeichnete Huelle %.1f x %.1f px"%(
            ci,fov,H,sx,sy,vz,L*H/vz,max(xs)-min(xs),max(ys)-min(ys)))
