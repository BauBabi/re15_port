"""Urteil zu einem Sichtbild MESSEN statt deuten: wo weicht das Spielbild vom reinen
Hintergrund ab (= dort ist die Figur gezeichnet), und wie viel davon liegt unter der Maske?

Aufruf: python sicht_pruef.py ROOM1050 6 vor <wx> <wz>
"""
import os, sys, struct
import numpy as np
from PIL import Image
WURZEL = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
import abnahme, geometrie, maskenbild as MB
from geom import load_rdt, load_bg, cut_view
from abnahme import proj
MASKS = os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "MASKS")
OUT = os.path.join(WURZEL, "analysis", "befunde_2026-09-19", "phase3_pri-kreativ")

room = sys.argv[1].upper(); cut = int(sys.argv[2]); tag = sys.argv[3]
wx, wz = int(sys.argv[4]), int(sys.argv[5])
# ⛔ Die Fusshoehe ist NICHT immer 0: in einem Treppenhaus steht der Spieler auf
# einem anderen Band. Optionales 6. Argument = Band (y = -band*0x708).
band = int(sys.argv[6]) if len(sys.argv) > 6 else 0
yf = -band * 0x708
rid = int(room[4:], 16)
bild = np.asarray(Image.open(os.path.join(OUT, "sicht_%s_C%d_%s.png" % (room, cut, tag))).convert("RGB"), int)
bg = load_bg(os.path.join(WURZEL, "build", "bg_ppm"), rid, cut).astype(int)
diff = np.abs(bild - bg).max(2)
figur = diff > 40
rdt, _ = load_rdt(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX"), room)
cam = struct.unpack_from("<I", rdt, 0x24)[0]; R, t, H = cut_view(rdt, cam, cut)
ms = MB.masken(open(os.path.join(MASKS, "%s.MSK" % room), "rb").read(), cut) or []
idx = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))[0]
deck, tief = abnahme.deckung_und_tiefe(ms, idx)
pf = proj(R, t, H, wx, yf, wz); pk = proj(R, t, H, wx, yf - 1500, wz)
sx, sy, fvz = pf
hw = 450 * H / fvz
x0, x1 = int(max(0, sx - hw)), int(min(320, sx + hw))
y0, y1 = int(max(0, min(pk[1], sy))), int(min(240, max(pk[1], sy)))
kasten = np.zeros((240, 320), bool); kasten[y0:y1, x0:x1] = True
print("%s C%d %s: Welt(%d,%d) Fuss-vz %d, Kasten x%d..%d y%d..%d" % (room, cut, tag, wx, wz, fvz, x0, x1, y0, y1))
print("  gezeichnete Figurpunkte im Kasten: %d von %d Kastenpunkten" % (int((figur & kasten).sum()), int(kasten.sum())))
print("  Maskenpunkte im Kasten: %d" % int((deck & kasten).sum()))
print("  Figurpunkte UNTER der Maske: %d  (dort ist die Maske gewonnen)" % int((figur & kasten & deck).sum()))
# Was WUERDE die Maske dort verdecken?
ys = np.arange(y0, y1)
vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
vzs = np.where(ok, vzs, fvz)
verd = np.zeros((240, 320), bool)
verd[y0:y1, x0:x1] = deck[y0:y1, x0:x1] & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs[:, None])
print("  Maskenpunkte, die an dieser Stelle VERDECKEN: %d von %d" % (int(verd.sum()), int((deck & kasten).sum())))
print("  -> Urteil: %s" % ("Figur sichtbar, Maske verdeckt sie NICHT" if (figur & kasten).sum() > 50 and verd.sum() == 0
                           else ("Figur teils verdeckt" if (figur & kasten).sum() > 50
                                 else "Figur an dieser Stelle NICHT gezeichnet")))
