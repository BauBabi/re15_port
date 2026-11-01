"""wa2_batch.py - Abnahme W2: Messreihe im echten Fenster. Je Lauf: wa2_lauf.py (gdigrab hwnd, 30 Bilder/s, eigene PID),
neue befund.log-Zeilen sichern, wa2_blitz.py, Bildblatt (1 Bild/s), mkv loeschen (ausser keep).
Aufruf: wa2_batch.py <liste.json> [nur_prefix]"""
import os, re, subprocess, sys, json
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/W41abn2"
WZ = os.path.dirname(os.path.abspath(__file__))
BD = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_wasserblink/re15_port/build/platform/pc"
EXE = {"fix": BD + "/re15_wa2_fix.exe", "vorher": BD + "/re15_wa2_vorher.exe"}
BEF = BD + "/befund.log"
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
liste = json.load(open(sys.argv[1])); nur = sys.argv[2] if len(sys.argv) > 2 else ""
out_all = open(os.path.join(SP, "batch_ergebnis.txt"), "a", encoding="utf-8")
for L in liste:
    name = L["name"]
    if nur and not name.startswith(nur): continue
    d = os.path.join(SP, "runs", name)
    if os.path.exists(os.path.join(d, "fertig.txt")): continue
    os.makedirs(d, exist_ok=True)
    off = os.path.getsize(BEF) if os.path.exists(BEF) else 0
    r = subprocess.run([sys.executable, os.path.join(WZ, "wa2_lauf.py"), EXE[L["exe"]], d, str(L["dauer"]), "--warte", str(L["warte"])] + L["env"],
                       capture_output=True, text=True)
    neu = ""
    if os.path.exists(BEF):
        with open(BEF, "rb") as f: f.seek(off); neu = f.read().decode("utf-8", "replace")
    open(os.path.join(d, "befund_lauf.log"), "w", encoding="utf-8").write(neu)
    mkv = os.path.join(d, "capture.mkv"); zeile = name + ": "
    if r.returncode != 0 or not os.path.exists(mkv):
        zeile += "LAUF FEHLGESCHLAGEN rc=%d %s" % (r.returncode, (r.stdout + r.stderr)[-300:])
    else:
        ab = L["ab"]
        if ab == "auto":   # Raumeintritt = erstes Bild mit Mittel > 2,5 nach der schwarzen Ladephase (Mittel < 2), + 1 s; sonst 4,5 s
            import numpy as np
            rw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-i", mkv, "-vf", "scale=80:60", "-f", "rawvideo", "-pix_fmt", "gray", "-"], capture_output=True).stdout
            m = np.frombuffer(rw[: len(rw) // 4800 * 4800], np.uint8).reshape(-1, 60, 80).mean(axis=(1, 2))
            t1 = next((t for t in range(len(m)) if m[t] < 2), 0); t2 = next((t for t in range(t1, len(m)) if m[t] > 2.5), 0)
            ab = round((t2 + 30) / 30.0, 2) if t2 else 4.5
        L["ab"] = ab
        subprocess.run([sys.executable, os.path.join(WZ, "wa2_blitz.py"), mkv, os.path.join(d, "blitz"), str(ab), os.path.join(d, "befund_lauf.log")], capture_output=True, text=True)
        txt = open(os.path.join(d, "blitz.txt")).read().splitlines() if os.path.exists(os.path.join(d, "blitz.txt")) else ["ANALYSE FEHLT"]
        dbg = open(os.path.join(d, "debug.log"), errors="replace").read() if os.path.exists(os.path.join(d, "debug.log")) else ""
        raum = re.findall(r"loaded (room\w+)\.rdt", dbg); cuts = re.findall(r" R([0-9A-F]{4}) C(\d+) ", neu)
        wl = re.findall(r"\[wasserlauf41\][^\n]*", dbg); fam = re.findall(r"Spieler-Familie (PL\d+)", dbg)
        zeile += "ab=%ss %s | Raum %s | R/C zuletzt %s | Figur %s | %s" % (L["ab"], txt[0], raum[-1:] if raum else "?", cuts[-1:] if cuts else "?",
                                                              fam[-1:] if fam else "?", wl[-1][:90] if wl else "kein wasserlauf41")
        sd = os.path.join(d, "blatt"); os.makedirs(sd, exist_ok=True)
        subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", str(L["ab"]), "-i", mkv, "-vf", "select='not(mod(n\,30))',scale=320:240:flags=area",
                        "-fps_mode", "passthrough", os.path.join(sd, "b_%03d.png")])
        if not L.get("keep"): os.remove(mkv)
    out_all.write(zeile + "\n"); out_all.flush()
    open(os.path.join(d, "fertig.txt"), "w", encoding="utf-8").write(zeile + "\n")
    print(zeile, flush=True)
