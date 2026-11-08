# fx_lauf.py <basis> <name> <teil r1|expl> <elza 0|1> <pos x,z,rot,y> <skript-vorne> <exit_at> [ohne] [fd]
# Ein Messlauf der exe (Kopie unter eigenem Namen) mit dem Rezept der Riegel integration_r41_gatorszene_tod_r1 / _tod_explosion
# (test_r41_gatorszene.cmake), nur Lage, Warten und Ende frei. Danach Auszug aus gator_boss.log / debug.log.
import os, re, shutil, subprocess, sys, time
basis, name, teil, elza, pos, vorne, exit_at = sys.argv[1:8]
rest = sys.argv[8:]
ohne = "ohne" in rest
fd = [a for a in rest if a.startswith("fd=")]
elza = elza == "1"
binv = os.path.join(basis, "bin")
run = os.path.join(basis, "runs", name)
if os.path.isdir(run):
    shutil.rmtree(run)
os.makedirs(run)
exe = os.path.join(binv, "re15_pc_fx_%s.exe" % name)
shutil.copyfile(os.path.join(binv, "re15_pc.exe"), exe)
raum = "2091" if elza else "2090"
env = {k: v for k, v in os.environ.items() if not k.startswith("RE15_") and not k.startswith("G2_")}
env.update({"RE15_NO_INTRO": "1", "RE15_NOAUDIO": "1", "RE15_WINDOW_SCALE": "1", "SDL_ASSERT": "always_ignore",
            "RE15_SET_FLAG": "9:110,9:112" if ohne else "9:110", "RE15_DEBUG_JUMP": raum + "@250", "RE15_STATE_LOG": "state.log",
            "RE15_PLAYER_POS": pos, "RE15_GB_TEST": "5", "RE15_GATOR_KANISTER": "4430,-26160", "RE15_GIVE": "3:15,21:50",
            "RE15_EQUIP": "3", "RE15_INPUT_SCRIPT_BASIS": "spiel", "RE15_INPUT_SCRIPT_START": "1",
            "RE15_INPUT_SCRIPT": vorne + ",M0.3,MA0.1" * 12, "RE15_EXIT_AT": exit_at + "#" + raum})
if teil == "expl":
    env["RE15_R39_MESS_SPIELER_HP"] = "100"
if fd:
    env["RE15_FRAMEDUMP"] = fd[0][3:]
if elza:
    env.update({"RE15_TITLE_CONFIRM_MS": "1500", "RE15_PSELECT_AUTO": "1", "RE15_PSELECT_AUTO_SWITCH": "1"})
else:
    env.update({"RE15_TITLE_SHOT": "title.bmp", "RE15_TITLE_SHOT_AF": "2"})
t0 = time.time()
try:
    rc = subprocess.run([exe], cwd=run, env=env, timeout=400, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode
except subprocess.TimeoutExpired:
    rc = "TIMEOUT"
dt = time.time() - t0
try:
    os.remove(exe)
except OSError:
    pass
def zeilen(f):
    p = os.path.join(run, f)
    return open(p, errors="replace").read().splitlines() if os.path.exists(p) else []
gb = zeilen("gator_boss.log"); dbg = zeilen("debug.log")
out = []
im_r1 = False; r1 = []; klemme_r1 = []; klemme = []; kam = []; gtr = []; leiche = ""; lunge = []
for z in gb:
    if "R1 start pos=" in z: im_r1 = True; r1.append(z.split("RE2 ")[-1])
    elif "R1 ende (Leiche)" in z: im_r1 = False; leiche = z.split("pos=")[-1]
    elif "KOPF-KLEMME an F" in z:
        (klemme_r1 if im_r1 else klemme).append(z.split("RE2 ")[-1][:80])
    elif "KAMERA R1 Sperre ohne Schnitt" in z: kam.append("SPERRE")
    elif "KAMERA R1 EXPLOSION" in z:
        m = re.search(r"-> Cut (-?\d+) F(\d+)", z); kam.append("EXPL E%s F%s" % (m.group(1), m.group(2)) if m else "EXPL?")
    elif " START art " in z and z.startswith("GTR"):
        m = re.match(r"GTR F(\d+) START art (\d+)", z); gtr.append("F%s/a%s" % m.groups() if m else z[:30])
    elif z.startswith("BISSLAUF F"):
        lunge.append(z.split()[1])
r1f = [z for z in gb if "R1 VSync 4" in z]
lage = [z for z in dbg if "Schluessel-Lage: Kamera" in z]
schnitt = [z for z in dbg if "Schluessel-Schnitt statt R1-Rueckschnitt" in z]
einst = [z for z in dbg if "Schluessel-Einstellung: Cut" in z]
ende = [z for z in dbg if "Schluessel-Einstellung Ende: Cut" in z]
quad = ""
if lage:
    m = re.search(r"Kamera (\d+) \(groesstes Bild, laufende (\d+)\), Lage \(([-0-9,]+)\) Tiefe (\d+)", lage[0])
    if m:
        quad = "K%s L%s Lage(%s) Tiefe %s Quad %.1f" % (m.group(1), m.group(2), m.group(3), m.group(4), 180.0 * 208 / int(m.group(4)))
ex = ""
if schnitt:
    m = re.search(r"Explosions-Cut (\d+) -> Ausgangs-Cut (\d+) F(\d+)", schnitt[0]); ex = "E%s X%s F%s" % m.groups() if m else schnitt[0]
es = ""
if einst:
    m = re.search(r"Cut (\d+) \(vorher Cut (\d+)\) F(\d+)", einst[0]); es = "Einst K%s vorher %s F%s" % m.groups() if m else einst[0]
ee = ""
if ende:
    m = re.search(r"Ende: Cut (\d+) F(\d+)", ende[0]); ee = "Ende Cut %s F%s" % m.groups() if m else ende[0]
print("%-14s rc=%s %4.0fs | R1 %s %s | Kam %s | Klemme-R1 %s | Klemme-vor %d | GTR %s | Biss %s | Leiche %s | %s | %s | %s %s" % (
    name, rc, dt, ",".join(z.split()[-1] for z in r1f), ";".join(r1), ",".join(kam), klemme_r1[:1], len(klemme), ",".join(gtr[:4]),
    ",".join(lunge[:3]), leiche, quad, ex, es, ee))
