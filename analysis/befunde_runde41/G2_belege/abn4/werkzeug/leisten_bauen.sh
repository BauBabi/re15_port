#!/usr/bin/env bash
# G2 Abnahme 4: alle Bildleisten bauen. Spalten: "NB5 gelobt (bc5fb7c9c, Video Abnahme 4 vom 2026-11-02, Zeit = Anker + F/30, +-1 s)" | "neu ohne Kanister (Messhaken, Framedump F exakt)" | "neu mit Kanister (Framedump F exakt)".
# Anker NB5 (gemessen an den Videos: Kletter-Animation mo=231 F639/F640 bzw. cam-Wechsel): west4_leon F640 = 53.1 s -> 31.77; west5_elza F639 = 69.4 s -> 48.10;
# west4_elza F1116 (cam 2->3) = 88.70 s -> 51.50; frei4_leon F5492 (2->1) = 218.8 s / F6224 (1->0) = 244.0 s -> 36.2; frei4_elza F6908 (0->7) = 279.5 s -> 49.2.
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn4"
OLD="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G2abn4/runs"
A="$S/leisten"; mkdir -p "$A"; cd "$S"
M="python werkzeug/mixleiste.py"
NB5="NB5 gelobt bc5fb7c9c (Video, t=Anker+F/30)"; OHNE="neu ohne Kanister (Messhaken, Framedump)"; MIT="neu mit Kanister (Framedump)"
$M "$A/west4_leon_rampe_querung_rammen_F860-F1000.jpg" "w4L Querung" "$NB5=vid:$OLD/west4_leon/video.mp4:31.77:30" "$OHNE=fd:runs/ohne_west4_leon" "$MIT=fd:runs/neu_west4_leon" -- 860 870 875 880 885 890 900 920 940 960 980 1000
$M "$A/west4_leon_hochbiss_F1120-F1180.jpg" "w4L Hochbiss" "$NB5=vid:$OLD/west4_leon/video.mp4:31.77:30" "$OHNE=fd:runs/ohne_west4_leon" "$MIT=fd:runs/neu_west4_leon" -- 1120 1130 1140 1150 1160 1170 1180
$M "$A/west5_elza_rampe_rammen_F1050-F1960.jpg" "w5E Rammen" "$NB5=vid:$OLD/west5_elza/video.mp4:48.10:30" "$OHNE=fd:runs/ohne_west5_elza" "$MIT=fd:runs/neu_west5_elza" -- 1050 1080 1095 1100 1105 1110 1120 1140 1160 1200 1820 1850 1860 1865 1870 1880 1900 1940
$M "$A/west4_elza_nach_rammen_F1180-F2700.jpg" "w4E nach Rammen" "$NB5=vid:$OLD/west4_elza/video.mp4:51.50:30" "$OHNE=fd:runs/ohne_west4_elza" "$MIT=fd:runs/neu_west4_elza" -- 1180 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700
$M "$A/frei4_leon_plattform_suedkante_F5700-F6300.jpg" "f4L Suedkante" "$NB5=vid:$OLD/frei4_leon/video.mp4:36.2:30" "$OHNE=fd:runs/ohne_frei4_leon" "$MIT=fd:runs/neu_frei4_leon" -- 5700 5760 5820 5880 5940 6000 6060 6120 6180 6240 6300
$M "$A/frei4_elza_dash_F3300-F4000.jpg" "f4E Dash" "$NB5=vid:$OLD/frei4_elza/video.mp4:49.2:30" "$OHNE=fd:runs/ohne_frei4_elza" "$MIT=fd:runs/neu_frei4_elza" -- 3300 3370 3440 3510 3580 3650 3720 3790 3860 3930 4000
python - <<'PY'
import glob, re
from PIL import Image, ImageDraw
S = "C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn4"
fs = {int(re.search(r"fd_(\d+)\.ppm$", p).group(1)): p for p in glob.glob(S + "/runs/b_neu_west4_leon/fd_*.ppm")}
Fs = sorted(fs); cols = 6; W, H = 320, 240
rows = (len(Fs) + cols - 1) // cols
img = Image.new("RGB", (cols * (W + 2), 24 + rows * (H + 2)), (20, 20, 20)); d = ImageDraw.Draw(img)
d.text((4, 4), "Kanister-Sitz 30 B/s: neu_west4_leon F%d-F%d je Bild, Ausschnitt Kopf/Rampe (Framedump 640x480, Ausschnitt x240-640 y80-380)" % (Fs[0], Fs[-1]) if Fs else "keine Bilder", fill=(0, 255, 0))
for i, F in enumerate(Fs):
    im = Image.open(fs[F]).convert("RGB").crop((240, 80, 640, 380)).resize((W, H), Image.LANCZOS)
    img.paste(im, ((i % cols) * (W + 2), 24 + (i // cols) * (H + 2))); d.text(((i % cols) * (W + 2) + 4, 24 + (i // cols) * (H + 2) + 4), "F%d" % F, fill=(255, 255, 0))
img.save(S + "/leisten/kanister_sitz_30fps_west4_leon_hochbiss_F1115-F1175.jpg", quality=88); print("geschrieben burst", img.size, len(Fs))
PY
ls -la "$A"
