# gesten_streifen.py fuer RE2-Retail-RDTs: RBJ-Zeiger @+0x60 statt @+0x5C (nur der Zeiger, Format gleich).
import sys, struct
T = "C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/tools/r34n_d"
sys.path.insert(0, T); sys.path.insert(0, T + "/..")
import rbj_zensus as R
_orig = R.rbj_block
def rbj_block_re2(d):
    s = struct.unpack_from("<I", d, 0x60)[0]
    total, nrec = struct.unpack_from("<II", d, s)
    return s, total + nrec * 8
R.rbj_block = rbj_block_re2
import gesten_streifen as G
G.R.rbj_block = rbj_block_re2
sys.argv = [sys.argv[0]] + sys.argv[1:]
G.main()
