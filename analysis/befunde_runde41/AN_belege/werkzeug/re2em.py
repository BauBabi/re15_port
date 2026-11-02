import sys, struct, os
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/tools")
import emd_ansichtsblatt as E
EMS = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/shared_assets/RE2/CDEMD0.EMS"
TOC = {0x47: ((4476, 99872), (4525, 51252)), 0x49: ((4616, 99872), (4665, 51252)), 0x43: ((4147,99872),(4196,77296)), 0x41: ((4076,66592),(4109,77296))}
class R2(E.Modell):
    def __init__(self, typ):
        ems = open(EMS, 'rb').read()
        (ts, tl), (es, el) = TOC[typ]
        emd = ems[es*2048:es*2048+el]; tim = ems[ts*2048:ts*2048+tl]
        self.blob = emd; self.typ = typ; self.blob_index = -1; self.blob_len = el
        do, dc = struct.unpack_from('<II', emd, 0)
        d = list(struct.unpack_from('<%dI' % dc, emd, do)) + [do]
        self.meshes = E.md1_parse(emd[d[7]:d[8]])
        self.skel = E.emr_parse(emd[d[2]:d[3]])
        self.clips, self.frames = E.edd_parse(emd[d[1]:d[2]])
        self.tex, self.pal, self.clut_y = E.tim_decode(tim)
        self.texh, self.texw = self.tex.shape
        print("typ %02x: %d meshes, %d bones, %d clips (Paar1) %s" % (typ, len(self.meshes), self.skel.bone_count, len(self.clips), [c if isinstance(c,int) else len(c) for c in self.clips][:20]))
if __name__ == "__main__":
  typ = int(sys.argv[1], 0); clip = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0
  mo = R2(typ)
  E.blatt(mo, clip, "em/re2_em%02x.png" % typ, False)
