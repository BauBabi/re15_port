# Generatoren der Spur in eine Scratch-Datei laufen lassen (OUT umgebogen) und gegen die eingecheckte .inc vergleichen.
import importlib.util, sys, io, contextlib, filecmp
BAUM = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port"
OUTDIR = sys.argv[1]
for name, rel, inc in [("szenen", "/tools/r41_story80/szenen_bauen.py", "/engine/src/gen/story80_szenen.inc"),
                       ("texte", "/tools/r36_p/texte_bauen.py", "/engine/src/gen/story_r36_texte.inc")]:
    spec = importlib.util.spec_from_file_location("gen_" + name, BAUM + rel)
    mod = importlib.util.module_from_spec(spec)
    sys.argv = [BAUM + rel]
    spec.loader.exec_module(mod)
    ziel = OUTDIR + "/" + name + ".inc"
    mod.OUT = ziel
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        rc = mod.main()
    gleich = filecmp.cmp(ziel, BAUM + inc, shallow=False)
    print(name, "rc", rc, "bytegleich mit eingecheckter .inc:", gleich)
