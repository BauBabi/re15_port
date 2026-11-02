"""video_ton.py LAUFDIR ZIEL.mp4 : Szenen-Video mit Ton aus dem gdigrab-Mitschnitt (fenster.mp4) und RE15_AUDIO_CAP_SYNC (ton.raw,
s16le stereo 44100, 1470 Stereo-Frames je Spielbild). Das Video wird auf die Spielbild-Zeit umgerechnet (zeit.json: t = a + b*F), der
Ton bildgenau ab demselben Spielbild ausgeschnitten. Fenster: Vorlauf-Beginn - 20 Bilder bis Szenenende + 40."""
import sys, os, json, subprocess
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
d, ziel = sys.argv[1], sys.argv[2]
z = json.load(open(os.path.join(d, "zeit.json")))
roh = os.path.join(d, "ton.raw")
n_ton = os.path.getsize(roh) // 5880                     # Spielbilder im Tonmitschnitt
F_ende = z["F_arm"] + 1162 + 40
off = z.get("ton_versatz", 0)                            # Spielbild des ersten Ton-Takts (aus dem Protokoll), sonst 0
F0 = z["F_vl"] - 20
F1 = min(F_ende, n_ton + off)
t0 = z["a"] + z["b"] * F0
dauer_v = z["b"] * (F1 - F0)
faktor = 1.0 / (30.0 * z["b"])                           # Videozeit -> Spielbildzeit
cmd = [FF, "-hide_banner", "-loglevel", "error", "-y",
       "-ss", "%.3f" % t0, "-t", "%.3f" % dauer_v, "-i", os.path.join(d, "fenster.mp4"),
       "-f", "s16le", "-ar", "44100", "-ac", "2", "-ss", "%.4f" % ((F0 - off) / 30.0), "-t", "%.4f" % ((F1 - F0) / 30.0), "-i", roh,
       "-filter:v", "setpts=(PTS-STARTPTS)*%.6f,scale=640:-2" % faktor, "-r", "30",
       "-c:v", "libx264", "-crf", "20", "-pix_fmt", "yuv420p", "-c:a", "aac", "-b:a", "160k", "-shortest", ziel]
subprocess.run(cmd, check=True)
print("ok", ziel, "Spielbilder", F0, "..", F1, "Ton-Bilder", n_ton)
