import os
from PIL import Image
D = os.path.dirname(os.path.abspath(__file__))
SRC = r"C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_1050_F239_marke3.bmp"
im = Image.open(SRC)
print("Screenshot:", im.size, im.mode)
im.convert("RGB").save(os.path.join(D, "00_voll.png"))
# Vollbild ist 960x720 (3x 320x240)? pruefen
w, h = im.size
print("Skala gegen 320x240:", w / 320.0, h / 240.0)
sc = w // 320
small = im.convert("RGB").resize((320, 240), Image.BOX)
small.save(os.path.join(D, "00_voll_320x240.png"))
# Zoom auf Spieler + Faecher: 320-Koordinaten 150..290 x 100..240
box = (150, 100, 290, 240)
c = small.crop(box).resize(((box[2] - box[0]) * 5, (box[3] - box[1]) * 5), Image.NEAREST)
c.save(os.path.join(D, "01_zoom_x150-290_y100-240.png"))
print("scale", sc)
