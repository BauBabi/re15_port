import sys, os, collections
sys.path.insert(0, r'C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\g5nb1_c')
from otwalk import SS, D, walk, xy
def verts(s, a):
    c = s.u8(a+7) & 0xfc
    offs = {0x34:(8,20,32),0x3c:(8,20,32,44),0x24:(8,16,24),0x2c:(8,16,24,32),0x30:(8,16,24),0x38:(8,16,24,32),0x20:(8,12,16),0x28:(8,12,16,20)}.get(c)
    return [xy(s.u32(a+o)) for o in offs] if offs else []
for f in sys.argv[1:]:
    s = SS(os.path.join(D, f))
    g5 = s.u32(0x800cfe1c)
    # player entity: scan pointer-ish fields of G5 for buffers? print G5 +0x16c, +0x198, +0x0
    print('==', f, 'G5', hex(g5), 'pos', s.s32(g5+0x38), s.s32(g5+0x40), '+0x16c', hex(s.u32(g5+0x16c)), '+0x198', hex(s.u32(g5+0x198)))
    ob = 0x800cc22c + 0x1000*s.u8(0x800ce5e0)
    for ob in (0x800cc22c, 0x800cd22c):
        prims = walk(s, ob)
        res = collections.defaultdict(lambda: [10**9, -1, 0])
        for (b, a, ln) in prims:
            v = verts(s, a)
            if not v: continue
            cl = 'A(0x80138-0x8014f)' if 0x80138000 <= a < 0x80150000 else ('B(0x8015c-0x8016b)' if 0x8015c000 <= a < 0x8016c000 else 'other')
            xs = [p[0] for p in v]; ys = [p[1] for p in v]
            r = res[cl]; r[2] += 1; r[0] = min(r[0], b); r[1] = max(r[1], b)
            if min(xs) <= 63 and max(xs) >= 0 and min(ys) <= 239 and max(ys) >= 64:
                q = res[cl+' post']; q[0] = min(q[0], b); q[1] = max(q[1], b); q[2] += 1
                if b <= 201: res[cl+' post<=201'][2] += 1
        print('  OT', hex(ob), {k: (v[0], v[1], v[2]) for k, v in res.items()})
