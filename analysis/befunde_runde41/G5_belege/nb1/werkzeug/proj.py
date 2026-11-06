import math
def isq(x): return int(math.isqrt(x))
px,py,pz = 2700,-882,-29052; tx,ty,tz = -1584,-4608,-14850; H = 231
dx,dy,dz = tx-px, ty-py, tz-pz
dist = isq(dx*dx+dy*dy+dz*dz); hor = isq(dx*dx+dz*dz)
def tdiv(a,b): q = abs(a)//abs(b); return q if (a>=0)==(b>=0) else -q
sp = tdiv(-dy*4096, dist); cp = tdiv(hor*4096, dist)
sy = tdiv(dx*4096, hor); cy = tdiv(dz*4096, hor)
R = [cy,0,-sy, (sp*sy)>>12, cp, (sp*cy)>>12, (cp*sy)>>12, -sp, (cp*cy)>>12]
T = [ (R[0]*-px + R[1]*-py + R[2]*-pz)>>12, (R[3]*-px + R[4]*-py + R[5]*-pz)>>12, (R[6]*-px + R[7]*-py + R[8]*-pz)>>12 ]
print('R', R, 'T', T)
def view(x,y,z):
    vx = ((R[0]*x+R[1]*y+R[2]*z)>>12)+T[0]; vy = ((R[3]*x+R[4]*y+R[5]*z)>>12)+T[1]; vz = ((R[6]*x+R[7]*y+R[8]*z)>>12)+T[2]
    return vx, vy, vz
def scr(x,y,z):
    vx,vy,vz = view(x,y,z); return vx,vy,vz, 160 + vx*H//vz, 120 + vy*H//vz
for nm,(x,y,z) in [('G5 T0/port -9000',(-9000,0,-23400)),('G5 RE2 T1 -9525',(-9525,0,-23400)),('G5 s06 -5126',(-5126,0,-23400)),('G5 s07 -4899',(-4899,0,-23400)),
                   ('G5 RE2 end -2786',(-2786,0,-23400)),('G5 port end -1986',(-1986,0,-23400)),('Nordpfosten floor (-5683,0,-21990)',(-5683,0,-21990)),('Nordpfosten y-1800',(-5683,-1800,-21990)),('Tuermitte (-5683,0,-23482)',(-5683,0,-23482))]:
    print('%-36s view/scr' % nm, scr(x,y,z))
print('dvz per +800 x:', view(-9000+800,0,-23400)[2]-view(-9000,0,-23400)[2])
for d in (100,300):
    print('depth', d, 'RE2 key vz >=', math.ceil(d*32768/1023), ' RE1.5 key vz >=', math.ceil((d+1)*65536/1023))
