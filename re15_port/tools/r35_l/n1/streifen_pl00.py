# gesten_streifen.py mit PL00.EDD/PL00.EMR (Spieler-Clips) statt Raum-RBJ: Aufruf <dummy> 0 <clip> <out> [...]
import sys, os
T = "C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/tools/r34n_d"
sys.path.insert(0, T); sys.path.insert(0, T + "/..")
import gesten_streifen as G
E = G.E
class LeonPL(G.Leon):
    def __init__(self, rdt_pfad, rec):
        self.meshes = E.md1_parse(open(os.path.join(G.PLD, "PL00.MD1"), "rb").read())
        self.skel = E.emr_parse(open(os.path.join(G.PLD, "PL00.EMR"), "rb").read())
        self.tex, self.pal, self.clut_y = E.tim_decode(open(os.path.join(G.PLD, "PL00.TIM"), "rb").read())
        self.texh, self.texw = self.tex.shape
        self.clips, self.frames = E.edd_parse(open(os.path.join(G.PLD, "PL00.EDD"), "rb").read())
G.Leon = LeonPL
G.main()
