"""Filmstreifen von EM047-Posen (CDEMD0 = Elliot/Leon, CDEMD1 = Roy/Elza) und RBJ-Records.
Nutzt die Leser aus re15_port/tools/emd_ansichtsblatt.py (Port der Engine-Leser)."""
import sys, struct, importlib.util, numpy as np
from PIL import Image, ImageDraw
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy'
spec = importlib.util.spec_from_file_location('ab', W + '/re15_port/tools/emd_ansichtsblatt.py')
ab = importlib.util.module_from_spec(spec); spec.loader.exec_module(ab)
EMD = W + '/re15_port/shared_assets/PSX/EMD/'
def blob(ems, sector, size):
    d = open(EMD + ems, 'rb').read(); return d[sector*2048:sector*2048+size]
class M(ab.Modell):
    def __init__(self, b, bank=1, rbj=None):
        d = ab.emd_dir(b); self.blob = b; self.typ = 0x47; self.blob_index = 0; self.blob_len = len(b)
        self.meshes = ab.md1_parse(b[d[7]:d[8]])
        self.tex, self.pal, self.clut_y = ab.tim_decode(b[d[8]:]); self.texh, self.texw = self.tex.shape
        if rbj is None:
            self.skel = ab.emr_parse(b[d[bank+1]:d[bank+2]])
            self.clips, self.frames = ab.edd_parse(b[d[bank]:d[bank+1]])
        else:
            base = ab.emr_parse(b[d[2]:d[3]])
            r, rec = rbj
            tot = struct.unpack_from('<I', r, 0)[0]
            pre, edd = struct.unpack_from('<II', r, tot + rec*8)
            emr = r[pre+4:edd]          # EMR-Kopf + Keyframes des Records (Hierarchie aus der Basis)
            s = ab.emr_parse(emr)
            base.kf_data, base.kf_size, base.kf_count = s.kf_data, s.kf_size, s.kf_count
            self.skel = base
            nxt = sorted([struct.unpack_from('<I', r, tot + k*8)[0] for k in range(struct.unpack_from('<I', r, 4)[0])] + [tot])
            end = [x for x in nxt if x > edd]; end = end[0] if end else tot
            self.clips, self.frames = ab.edd_parse(r[edd:end])

    def geometrie(self, keyframe):
        R, T = self.pose(keyframe)
        V, N, F = [], [], []
        for b in range(min(self.skel.bone_count, len(self.meshes))):
            m = self.meshes[b]; rot = R[b].astype(np.float64)/4096.0; tr = T[b].astype(np.float64)
            def xf(vs):
                return np.array([[v[0],v[1],v[2]] for v in vs], float) @ rot.T + tr if vs else np.zeros((0,3))
            def xn(ns):
                if not ns: return np.zeros((0,3))
                w = np.array([[v[0],v[1],v[2]] for v in ns], float) @ rot.T
                ln = np.linalg.norm(w, axis=1, keepdims=True); ln[ln==0]=1; return w/ln
            tvb=len(V); V.extend(xf(m.tv)); tnb=len(N); N.extend(xn(m.tn))
            qvb=len(V); V.extend(xf(m.qv)); qnb=len(N); N.extend(xn(m.qn))
            for i,(n0,v0,n1,v1,n2,v2) in enumerate(m.tf):
                uv=m.tuv[i]
                F.append(dict(v=(tvb+v0,tvb+v1,tvb+v2), n=(tnb+n0,tnb+n1,tnb+n2), uv=((uv[0],uv[1]),(uv[3],uv[4]),(uv[6],uv[7])), clut=uv[2], page=uv[5], bone=b))
            for i,(n0,v0,n1,v1,n2,v2,n3,v3) in enumerate(m.qf):
                uv=m.quv[i]; a,bb=(uv[0],uv[1]),(uv[3],uv[4]); c,dd=(uv[6],uv[7]),(uv[9],uv[10])
                F.append(dict(v=(qvb+v0,qvb+v1,qvb+v2), n=(qnb+n0,qnb+n1,qnb+n2), uv=(a,bb,c), clut=uv[2], page=uv[5], bone=b))
                F.append(dict(v=(qvb+v1,qvb+v3,qvb+v2), n=(qnb+n1,qnb+n3,qnb+n2), uv=(bb,dd,c), clut=uv[2], page=uv[5], bone=b))
        Nn = np.array(N) if N else np.zeros((0,3))
        return np.array(V), Nn, F
def front(mo, kf, H=200):
    V, N, F = mo.geometrie(kf)
    right, down, depth = ab.V_LEFT
    right=np.asarray(right,float); down=np.asarray(down,float)
    scale = H*0.92/2300.0
    cx = H/2 - float(np.dot(V.mean(0), right))*scale
    cy = H*0.97
    col, alp = ab.render(mo, V, N, F, right, down, depth, H, H, scale, cx, cy)
    rgb = np.clip(col*1.62, 0, 1)**(1/1.18)
    return Image.fromarray((rgb*255).astype(np.uint8))
def strip(mo, clip, label, n=8, H=200):
    first, cnt = mo.clips[clip]
    idx = sorted(set([int(round(i*(cnt-1)/max(1,n-1))) for i in range(n)]))
    out = Image.new('RGB', (len(idx)*H, H+18), (20,20,24)); d = ImageDraw.Draw(out)
    for j, f in enumerate(idx):
        kf = mo.frames[first+f] & 0xfff
        out.paste(front(mo, kf, H), (j*H, 18))
        d.text((j*H+4, 2), '%s c%d f%d/%d' % (label, clip, f, cnt), fill=(230,230,230))
    return out
if __name__ == '__main__':
    import os
    out = sys.argv[1]
    L = blob('CDEMD0.EMS', 1644, 127640); E = blob('CDEMD1.EMS', 1639, 142272)
    jobs = []
    for spec_ in sys.argv[2:]:
        src, bank, clip = spec_.split(':')
        b = L if src.startswith('L') else E
        if bank.startswith('rbj'):
            room, rec = bank[3:].split('r')
            r = open(W + '/re15_port/shared_assets/PSX/STAGE1/ROOM%s.RDT' % room, 'rb').read()
            ao = struct.unpack_from('<I', r, 0x5C)[0]
            mo = M(b, rbj=(r[ao:], int(rec)))
        else:
            mo = M(b, bank=int(bank))
        jobs.append(strip(mo, int(clip), '%s b%s' % (src, bank)))
    Wd = max(j.width for j in jobs); Ht = sum(j.height for j in jobs)
    sheet = Image.new('RGB', (Wd, Ht), (0,0,0)); y = 0
    for j in jobs: sheet.paste(j, (0, y)); y += j.height
    sheet.save(out); print('ok', out, sheet.size)
