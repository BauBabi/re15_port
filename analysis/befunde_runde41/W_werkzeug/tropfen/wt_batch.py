"""wt_batch.py - Spur W OFFEN 4 (Variante B, Tropfen-Schleife aus): Messreihe im echten Fenster, vorher/nachher.
Kopie des Musters ../abnahme2/wa2_batch.py: je Lauf ../abnahme2/wa2_lauf.py (gdigrab hwnd=, 30 Bilder/s, eigene PID,
beschleunigter Renderer, kein AUTOSHOT, kein SOFTWARE_RENDER), dann ../abnahme2/wa2_blitz.py (Blitze, Wechselzahl je Bildpunkt,
oberer Rand), Medianbild (320x240, Grau) fuer den Vergleich vorher/nachher, Auszug aus fx.log (RE15_FX_LOG) und debug.log.
Zusaetzlich zu den Umgebungsvariablen der Liste setzt jeder Lauf RE15_FX_LOG=fx.log und RE15_EVT_TRACE=1.
Aufruf: wt_batch.py <liste.json> <out_dir> <exe_vorher> <exe_nachher> [nur_prefix]"""
import json, os, re, subprocess, sys
import numpy as np
WZ = os.path.dirname(os.path.abspath(__file__))
A2 = os.path.join(os.path.dirname(WZ), "abnahme2")
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
liste = json.load(open(sys.argv[1])); SP = sys.argv[2]
EXE = {"vorher": sys.argv[3], "nachher": sys.argv[4]}
nur = sys.argv[5] if len(sys.argv) > 5 else ""
BEF = os.path.join(os.path.dirname(EXE["vorher"]), "befund.log")
os.makedirs(os.path.join(SP, "runs"), exist_ok=True)
out_all = open(os.path.join(SP, "batch_ergebnis.txt"), "a", encoding="utf-8")
for L in liste:
    for exe in L.get("exes", ["vorher", "nachher"]):
        name = "%s_%s" % (L["name"], exe)
        if nur and not name.startswith(nur): continue
        d = os.path.join(SP, "runs", name)
        if os.path.exists(os.path.join(d, "fertig.txt")): continue
        os.makedirs(d, exist_ok=True)
        off = os.path.getsize(BEF) if os.path.exists(BEF) else 0
        env = ["RE15_FX_LOG=fx.log", "RE15_EVT_TRACE=1"] + L["env"]
        r = subprocess.run([sys.executable, os.path.join(A2, "wa2_lauf.py"), EXE[exe], d, str(L["dauer"]), "--warte", str(L["warte"])] + env,
                           capture_output=True, text=True)
        neu = ""
        if os.path.exists(BEF):
            with open(BEF, "rb") as f: f.seek(off); neu = f.read().decode("utf-8", "replace")
        open(os.path.join(d, "befund_lauf.log"), "w", encoding="utf-8").write(neu)
        mkv = os.path.join(d, "capture.mkv"); zeile = name + ": "
        if r.returncode != 0 or not os.path.exists(mkv):
            zeile += "LAUF FEHLGESCHLAGEN rc=%d %s" % (r.returncode, (r.stdout + r.stderr)[-300:])
        else:
            # Raumeintritt = erstes Bild mit Mittel > 2,5 nach der schwarzen Ladephase (Mittel < 2), + 1 s (wie wa2_batch.py ab=auto)
            rw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-i", mkv, "-vf", "scale=80:60", "-f", "rawvideo", "-pix_fmt", "gray", "-"],
                                capture_output=True).stdout
            m = np.frombuffer(rw[: len(rw) // 4800 * 4800], np.uint8).reshape(-1, 60, 80).mean(axis=(1, 2))
            t1 = next((t for t in range(len(m)) if m[t] < 2), 0); t2 = next((t for t in range(t1, len(m)) if m[t] > 2.5), 0)
            ab = round((t2 + 30) / 30.0, 2) if t2 else 4.5
            subprocess.run([sys.executable, os.path.join(A2, "wa2_blitz.py"), mkv, os.path.join(d, "blitz"), str(ab), os.path.join(d, "befund_lauf.log")],
                           capture_output=True, text=True)
            raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(ab), "-i", mkv, "-vf", "scale=320:240:flags=area",
                                  "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
            n = len(raw) // (320 * 240 * 3)
            g = np.frombuffer(raw[: n * 320 * 240 * 3], np.uint8).reshape(n, 240, 320, 3).astype(np.int16).mean(axis=3)
            np.save(os.path.join(d, "median.npy"), np.median(g, axis=0).astype(np.int16))
            np.save(os.path.join(d, "spur_oben.npy"), g[:, 0:20, :].astype(np.int16))     # obere 20 Zeilen je Bild (Tropfen-Fleck)
            txt = open(os.path.join(d, "blitz.txt")).read().splitlines() if os.path.exists(os.path.join(d, "blitz.txt")) else ["ANALYSE FEHLT"]
            dbg = open(os.path.join(d, "debug.log"), errors="replace").read() if os.path.exists(os.path.join(d, "debug.log")) else ""
            fx = open(os.path.join(d, "fx.log"), errors="replace").read() if os.path.exists(os.path.join(d, "fx.log")) else ""
            id6 = re.findall(r"^id=6 sub=0 [^\n]*", fx, re.M)
            id6_f = [int(x) for x in re.findall(r"^id=6 sub=0 .* F=(\d+)$", fx, re.M)]
            id6_w = sorted(set(re.findall(r"^id=6 sub=0 eidx=-?\d+ frame=-?\d+ w\((-?\d+),-?\d+,(-?\d+)\)", fx, re.M)))
            id0b = len(re.findall(r"^id=11 ", fx, re.M))
            fx_ids = sorted(set(int(x) for x in re.findall(r"^id=(\d+) ", fx, re.M)))
            fx_f = [int(x) for x in re.findall(r" F=(\d+)$", fx, re.M)]
            evt = re.findall(r"\[evt\][^\n]*", dbg); tr = re.findall(r"\[tropfen41\][^\n]*", dbg)
            raum = re.findall(r"loaded (room\w+)\.rdt", dbg); fam = re.findall(r"Spieler-Familie (PL\d+)", dbg)
            wl = re.findall(r"\[wasserlauf41\][^\n]*", dbg)
            zeile += ("ab=%ss bilder=%d | %s | Raum %s Figur %s | fx.log F%s..F%s ids %s, id=6 sub=0 %d Zeilen (F%s..F%s, Orte %s), id=11 %d | "
                      "evt %d: %s | tropfen41: %s | %s") % (
                ab, n, txt[0], raum[-1:] if raum else "?", fam[-1:] if fam else "?",
                min(fx_f) if fx_f else "-", max(fx_f) if fx_f else "-", fx_ids, len(id6), min(id6_f) if id6_f else "-", max(id6_f) if id6_f else "-",
                id6_w[:4], id0b, len(evt), " / ".join(e[:80] for e in evt[:4]), tr[:2] or "-", wl[-1][:70] if wl else "kein wasserlauf41")
            if not L.get("keep"): os.remove(mkv)
        out_all.write(zeile + "\n"); out_all.flush()
        open(os.path.join(d, "fertig.txt"), "w", encoding="utf-8").write(zeile + "\n")
        print(zeile, flush=True)
