import struct, sys
R = open(r"C:\workspace\git\reAi_v2\info\re2leon\PL0\RDT\ROOM7040.RDT", "rb").read()
md1 = R[0x28a4:0x3080]
L, unk, oc = struct.unpack_from("<III", md1, 0)
print("MD1 len %d unk %d objcount %d" % (L, unk, oc))
for m in range(oc//2):
    h = struct.unpack_from("<14I", md1, 12 + 56*m)
    print("mesh", m, h)
    tv, tvc, tn, tnc, tf, tfc, tu, qv, qvc, qn, qnc, qf, qfc, qu = h
    base = 12
    V = [struct.unpack_from("<hhhh", md1, base+tv+8*i)[:3] for i in range(tvc)]
    QV = [struct.unpack_from("<hhhh", md1, base+qv+8*i)[:3] for i in range(qvc)]
    print(" tri verts", tvc, "quad verts", qvc, "same:", V == QV)
    for i in range(tfc):
        n0,v0,n1,v1,n2,v2 = struct.unpack_from("<6H", md1, base+tf+12*i)
        u = struct.unpack_from("<BBHBBHBBH", md1, base+tu+12*i)
        print("  T%02d v %s uv (%d,%d)(%d,%d)(%d,%d) clut %04x page %04x" % (i, [V[v0],V[v1],V[v2]], u[0],u[1],u[3],u[4],u[6],u[7], u[2], u[5]))
    for i in range(qfc):
        q = struct.unpack_from("<8H", md1, base+qf+16*i)
        u = struct.unpack_from("<BBHBBHBBHBBH", md1, base+qu+16*i)
        print("  Q%02d v %s uv (%d,%d)(%d,%d)(%d,%d)(%d,%d) clut %04x page %04x" % (i, [QV[q[1]],QV[q[3]],QV[q[5]],QV[q[7]]], u[0],u[1],u[3],u[4],u[6],u[7],u[9],u[10], u[2], u[5]))
# TIM
t = R[0x33844:]
magic, flags = struct.unpack_from("<II", t, 0)
print("TIM magic %x flags %x" % (magic, flags))
off = 8
if flags & 8:
    cl = struct.unpack_from("<IHHHH", t, off); print(" CLUT len %d x %d y %d w %d h %d" % cl); off += cl[0]
im = struct.unpack_from("<IHHHH", t, off); print(" IMG len %d x %d y %d w %d h %d" % im); off += im[0]
print(" TIM total", off)
