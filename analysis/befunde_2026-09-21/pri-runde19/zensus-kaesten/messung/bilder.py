# -*- coding: utf-8 -*-
"""Zensus A: Bildbelege. Links Hintergrund, Mitte ROHE Kasten-Silhouette,
rechts was der Kasten AUSGELIEFERT beitraegt."""
import io, json, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum
from PIL import Image, ImageDraw

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
OUT = 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
Z = 3

FARBEN = [(255, 0, 200), (0, 200, 255), (255, 200, 0), (0, 255, 80), (255, 80, 0),
          (160, 80, 255), (255, 255, 255), (0, 120, 255), (200, 255, 0), (255, 0, 80),
          (80, 255, 200)]


def lege(bg, lagen):
    ov = bg.astype(float) * 0.45
    for (reg, col) in lagen:
        ov[reg] = ov[reg] * 0.25 + np.array(col) * 0.75
    return ov.astype(np.uint8)


def tafel(bilder, titel, pfad):
    w = 320 * Z
    im = Image.new('RGB', (w * len(bilder) + 8 * (len(bilder) - 1), 240 * Z + 18), (12, 12, 12))
    for i, b in enumerate(bilder):
        im.paste(Image.fromarray(b).resize((w, 240 * Z), Image.NEAREST), (i * (w + 8), 18))
    d = ImageDraw.Draw(im)
    d.text((4, 4), titel, fill=(255, 255, 255))
    im.save(pfad)
    return pfad


for room, cut in (('ROOM10F0', 4), ('ROOM10F0', 5)):
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    bg = geom.load_bg(PPM, rid, cut)
    e = raum.eintrag(AUS[room][str(cut)])
    objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    roh = []
    wirk = []
    ci = 0
    for o in objs:
        if o[9] is None:
            continue
        q = [int(x) for x in o[9]]
        vz, tr = geom.quader_tiefe(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4])
        roh.append((tr, FARBEN[ci % len(FARBEN)]))
        wirk.append((o[1], FARBEN[ci % len(FARBEN)]))
        ci += 1
    p = tafel([bg.astype(np.uint8), lege(bg, roh), lege(bg, wirk)],
              '%s C%d   links Hintergrund | Mitte ROHE Quader-Silhouetten (%d Kaesten) | rechts was sie mit nur_kunst WIRKLICH beitragen'
              % (room, cut, len(roh)),
              os.path.join(OUT, 'kasten_roh_vs_wirksam_%s_C%d.png' % (room, cut)))
    print(p)

# ROOM1100 C2: der Ueberstand-Streifen und ein bissiger Standplatz
room, cut = 'ROOM1100', 2
rid = int(room[4:], 16)
rdt, st = geom.load_rdt(CD, room)
cam = struct.unpack_from('<I', rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, cut)
bg = geom.load_bg(PPM, rid, cut)
blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
ms = maskenbild.masken(blob, cut) or []
idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
deck, tief = abnahme.deckung_und_tiefe(ms, idx)
fl = np.load('build/r19/fl_%s_C%d.npy' % (room, cut))
ys = np.arange(240)[:, None]
streifen = deck & np.isfinite(fl)[None, :] & (ys >= np.where(np.isfinite(fl), fl, 1e9)[None, :])
wx, wz = -15700, -11500
pf = abnahme.proj(R, t, H, wx, 0, wz)
pk = abnahme.proj(R, t, H, wx, -abnahme.KOPF, wz)
fsx, fsy, fvz = pf
ksx, ksy, kvz = pk
hw = abnahme.HALB * H / fvz
x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
koerper = np.zeros((240, 320), bool)
koerper[y0:y1, x0:x1] = True
rows = np.arange(y0, y1)
vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, rows)
vzs = np.where(ok, vzs, fvz)[:, None]
biss = np.zeros((240, 320), bool)
biss[y0:y1, x0:x1] = streifen[y0:y1, x0:x1] & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs)
a = lege(bg, [(deck & ~streifen, (255, 0, 200)), (streifen, (255, 230, 0))])
b = lege(bg, [(koerper & ~biss, (0, 200, 255)), (biss, (255, 0, 0))])
p = tafel([bg.astype(np.uint8), a, b],
          'ROOM1100 C2  links Hintergrund | Mitte Maske (magenta) mit dem Ueberstand auf dem gemalten Boden (GELB, 698 Punkte) | '
          'rechts Koerperkasten (-15700,-11500) mit den 65 gebissenen Punkten (ROT)',
          os.path.join(OUT, 'ueberstand_ROOM1100_C2.png'))
print(p)
