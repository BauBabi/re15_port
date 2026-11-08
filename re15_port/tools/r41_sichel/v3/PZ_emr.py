# Zahlen-Pruefer V3 r1: eigener EMR-Vergleich Knochen 0-15 (Bezug Ref2) - Bit-Felder je Keyframe.
import sys, struct, json
sys.path.insert(0, __import__('os').path.dirname(__file__))
from PZ_emd import sektionen

def u16(b, o): return struct.unpack_from('<H', b, o)[0]

def bits(d, off, n):
    v = 0
    for i in range(n):
        bi = (off + i) >> 3
        v |= ((d[bi] >> ((off + i) & 7)) & 1) << i
    return v

def emr(e):
    bt, ko, nb, ks = u16(e, 0), u16(e, 2), u16(e, 4), u16(e, 6)
    rel = [struct.unpack_from('<3h', e, 8 + b * 6) for b in range(nb)]
    kids = [(u16(e, bt + b * 4), u16(e, bt + b * 4 + 2)) for b in range(nb)]
    kids_l = [list(e[bt + co: bt + co + cc]) for cc, co in kids]
    kf = e[ko:]; n = len(kf) // ks
    return dict(bt=bt, ko=ko, nb=nb, ks=ks, rel=rel, kids=kids_l, kf=kf, n=n)

A, B = (open(p, 'rb').read() for p in sys.argv[1:3])
sa, _ = sektionen(A); sb, _ = sektionen(B)
ea, eb = emr(A[sa[2][0]:sa[2][1]]), emr(B[sb[2][0]:sb[2][1]])
out = dict(kopf_ref=[ea[k] for k in ('bt', 'ko', 'nb', 'ks', 'n')], kopf_var=[eb[k] for k in ('bt', 'ko', 'nb', 'ks', 'n')])
out['rel_0_15_gleich'] = ea['rel'][:16] == eb['rel'][:16]
out['rel_16_22'] = [ea['rel'][16:], eb['rel'][16:]]
out['kinder_0_15_gleich'] = ea['kids'][:16] == eb['kids'][:16]
out['kinder_diff'] = [(b, ea['kids'][b], eb['kids'][b]) for b in range(min(ea['nb'], eb['nb'])) if ea['kids'][b] != eb['kids'][b]]
diff_kf_kopf, diff_bones = 0, {}
for k in range(min(ea['n'], eb['n'])):
    a = ea['kf'][k * ea['ks']:(k + 1) * ea['ks']]; b = eb['kf'][k * eb['ks']:(k + 1) * eb['ks']]
    if a[:12] != b[:12]: diff_kf_kopf += 1
    for bo in range(min(ea['nb'], eb['nb'])):
        if bits(a[12:], bo * 36, 36) != bits(b[12:], bo * 36, 36):
            diff_bones[bo] = diff_bones.get(bo, 0) + 1
out['kf_kopf_12byte_abweichend'] = diff_kf_kopf
out['kf_bitfeld_abweichend_je_knochen'] = diff_bones
# Bytes ausserhalb der Knochenfelder (Rest am Keyframe-Ende)
print(json.dumps(out, default=str))
