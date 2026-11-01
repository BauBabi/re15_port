"""wa_batch.py - Abnahme W1: Messreihe echtes Fenster. Je Lauf: wa_lauf.py (Aufnahme), neue befund.log-Zeilen sichern,
wa_blitz.py (Blitze), Leon-Kasten-Abgleich, Bildblatt, mkv loeschen (ausser keep). Aufruf: wa_batch.py <liste> [nur_prefix]"""
import os, re, subprocess, sys, json, shutil
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
BD = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_wasserblink/re15_port/build/platform/pc"
EXE = {"fix": BD + "/re15_wa1_fix.exe", "vorher": BD + "/re15_wa1_vorher.exe"}
BEF = BD + "/befund.log"
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
liste = json.load(open(sys.argv[1]))
nur = sys.argv[2] if len(sys.argv) > 2 else ""
out_all = open(os.path.join(SP, "batch_ergebnis.txt"), "a", encoding="utf-8")
for L in liste:
    name = L["name"]
    if nur and not name.startswith(nur): continue
    d = os.path.join(SP, "runs", name)
    if os.path.exists(os.path.join(d, "fertig.txt")): continue
    os.makedirs(d, exist_ok=True)
    off = os.path.getsize(BEF) if os.path.exists(BEF) else 0
    r = subprocess.run([sys.executable, os.path.join(SP, "tools", "wa_lauf.py"), EXE[L["exe"]], d, str(L["dauer"]), "--warte", str(L["warte"])] + L["env"],
                       capture_output=True, text=True)
    neu = ""
    if os.path.exists(BEF):
        with open(BEF, "rb") as f:
            f.seek(off); neu = f.read().decode("utf-8", "replace")
    open(os.path.join(d, "befund_lauf.log"), "w", encoding="utf-8").write(neu)
    mkv = os.path.join(d, "capture.mkv")
    zeile = name + ": "
    if r.returncode != 0 or not os.path.exists(mkv):
        zeile += "LAUF FEHLGESCHLAGEN rc=%d %s" % (r.returncode, (r.stdout + r.stderr)[-300:])
    else:
        subprocess.run([sys.executable, os.path.join(SP, "tools", "wa_blitz.py"), mkv, os.path.join(d, "blitz"), str(L["ab"])], capture_output=True, text=True)
        txt = open(os.path.join(d, "blitz.txt")).read().splitlines()
        # Leon-Kaesten (PSX-Koordinaten = Fenster 320x240) aus dem befund.log des Laufs
        kaesten = [tuple(map(int, m.groups())) for m in re.finditer(r"kasten=x(-?\d+)\.\.(-?\d+),y(-?\d+)\.\.(-?\d+)", neu)]
        leon, rest = [], []
        for t in txt[1:]:
            m = re.search(r"x(\d+)\.\.(\d+) y(\d+)\.\.(\d+)", t)
            x0, x1, y0, y1 = map(int, m.groups())
            hit = any(not (x1 < k[0] - 8 or x0 > k[1] + 8 or y1 < k[2] - 8 or y0 > k[3] + 8) for k in kaesten)
            (leon if hit else rest).append(t.strip())
        dbg = open(os.path.join(d, "debug.log"), errors="replace").read() if os.path.exists(os.path.join(d, "debug.log")) else ""
        raum = re.findall(r"loaded (room\w+)\.rdt", dbg)
        cuts = re.findall(r"\[pri\] cut=(\d+)", dbg)
        wl = re.findall(r"\[wasserlauf41\][^\n]*", dbg)
        sf = re.findall(r"\[setflag\][^\n]*", dbg)
        fam = re.findall(r"Spieler-Familie (PL\d+)", dbg)
        zeile += "%s | Raum %s | Cuts %s | Figur %s | %s | %s | Leon-nah %d | ANDERE BLITZE %d" % (
            txt[0], raum[-1:] if raum else "?", "".join(c + "," for c in cuts[-4:]), fam[-1:] if fam else "?", wl[-1:] if wl else "kein wasserlauf41", sf, len(leon), len(rest))
        for t in rest[:12]: zeile += "\n      " + t
        open(os.path.join(d, "andere_blitze.txt"), "w").write("\n".join(rest))
        # Bildblatt: 12 Bilder gleichmaessig aus dem Auswertefenster
        sd = os.path.join(d, "blatt"); os.makedirs(sd, exist_ok=True)
        subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", str(L["ab"]), "-i", mkv, "-vf", "select='not(mod(n\,30))'", "-fps_mode", "passthrough", os.path.join(sd, "b_%03d.png")])
        if not L.get("keep"): os.remove(mkv)
    out_all.write(zeile + "\n"); out_all.flush()
    open(os.path.join(d, "fertig.txt"), "w", encoding="utf-8").write(zeile + "\n")
    print(zeile, flush=True)
