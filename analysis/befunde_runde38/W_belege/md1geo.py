import struct, sys
rdt = sys.argv[1]; obj = int(sys.argv[2])
d = open(rdt,'rb').read()
nO = d[2]
tbl = struct.unpack_from('<I', d, 0x30)[0]
tim_off, md1_off = struct.unpack_from('<II', d, tbl + obj*8)
print('nOmodel', nO, 'obj', obj, 'tim', hex(tim_off), 'md1', hex(md1_off))
m = d[md1_off:]
length, unk, objc = struct.unpack_from('<III', m, 0)
print('md1 length', length, 'objcount', objc)
base = 12
allv = []
for i in range(objc//2):
    h = struct.unpack_from('<14I', m, 12 + i*56)
    tv_off, tv_cnt, tn_off, tn_cnt, tf_off, tf_cnt, tu_off, qv_off, qv_cnt, qn_off, qn_cnt, qf_off, qf_cnt, qu_off = h
    vs = [struct.unpack_from('<hhhh', m, base+tv_off+k*8)[:3] for k in range(tv_cnt)]
    qs = [struct.unpack_from('<hhhh', m, base+qv_off+k*8)[:3] for k in range(qv_cnt)]
    print('mesh', i, 'tris', tf_cnt, 'tverts', tv_cnt, 'quads', qf_cnt, 'qverts', qv_cnt)
    allv += vs + qs
    for nm, lst in (('tv', vs), ('qv', qs)):
        if lst:
            xs=[v[0] for v in lst]; ys=[v[1] for v in lst]; zs=[v[2] for v in lst]
            print('   ', nm, 'x', min(xs), max(xs), 'y', min(ys), max(ys), 'z', min(zs), max(zs))
    if qf_cnt and len(sys.argv) > 3:
        for k in range(qf_cnt):
            f = struct.unpack_from('<8H', m, base+qf_off+k*16)
            print('   quad', k, f)
