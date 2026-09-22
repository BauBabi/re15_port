"""Marke ROOM10F0 C4: wie viel Figur nimmt welches Maskenmodell weg?

NULLMODELL zuerst (gar keine Maske = 0 %), dann der Auslieferungsstand, dann die
zwei Gegenmodelle. Die FIGUR ist in allen Faellen dieselbe (Dump der Sonde
probe_r22_10f0_figur, posierte PL00 im Renderer-Transform).
"""
import os, sys, struct
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
from PIL import Image
from scipy import ndimage as nd
import geom, msk

SC = os.path.dirname(os.path.abspath(__file__)) + "/"
R_ = "C:/workspace/git/reAi_v2/"
W_ = "C:/workspace/git/reAi_v2/.claude/worktrees/wf_b4972d66-6b5-2/"

fig = np.fromfile(W_ + "fig_marke.bin", np.int32).reshape(240, 320)
gez = fig > 0
bkfig = (1023 * fig.astype(np.int64)) >> 16
print("Figur an der Marke: %d gezeichnete Punkte" % gez.sum())

masks, gc, mdecl, draw, tief, wer, px, clut = msk.tiefenkarte('ROOM10F0', 4)
deck = tief < (1 << 30)

img = np.asarray(Image.open(R_ + 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/ausgeliefert_ROOM10F0_C4.png').convert('RGB'), np.uint8)
bg = img[:, :640][0::2, 0::2]
lasso = np.zeros((240, 320), bool)
for n, (x0, y0) in {'04_01': (88, 84), '04_02': (68, 109), '04_03': (59, 218)}.items():
    a = np.asarray(Image.open(R_ + 'pri/STAGE1/10F0/%s.png' % n).convert('RGBA'))[:, :, 3]
    h, w = a.shape
    lasso[y0:y0 + h, x0:x0 + w] |= (a > 110)
dunkel = bg.astype(int).sum(2) < 45
saum7 = (nd.distance_transform_edt(~lasso) <= 7)     # Regel 1: Kunst-Saum <= 7

def urteil(maskentief, name):
    verd = gez & (maskentief < bkfig)
    print("  %-34s Figur verdeckt %4d von %d = %5.1f %%"
          % (name, int(verd.sum()), int(gez.sum()), 100.0 * verd.sum() / gez.sum()))
    return verd

def variante(sel):
    t = np.full((240, 320), 1 << 30, np.int64)
    t[sel] = tief[sel]
    return t

print()
print("Marke F335 (-448,0,14087) rot=13040 — Anteil der Figur, den die Maske wegnimmt:")
urteil(np.full((240, 320), 1 << 30, np.int64), "NULLMODELL (keine Maske)")
v_ist = urteil(variante(deck), "AUSGELIEFERT (105 Rechtecke)")
v_las = urteil(variante(deck & lasso), "NUR Nutzer-Lasso")
v_s7 = urteil(variante(deck & (lasso | (dunkel & saum7))), "Lasso + Tiefschwarz-Saum <=7")
urteil(variante(deck & ~dunkel), "ohne JEDES Tiefschwarz-Pixel")

print()
print("Deckung der Varianten (Masken-Texel):")
print("  ausgeliefert                 %6d" % deck.sum())
print("  nur Lasso                    %6d" % (deck & lasso).sum())
print("  Lasso + Saum<=7              %6d" % (deck & (lasso | (dunkel & saum7))).sum())

# Rechtfertigung: liegt an den verdeckten Figurpunkten eine SOLIDE Zelle im Sehstrahl?
rdt = open(W_ + "re15_port/shared_assets/PSX/STAGE1/ROOM10F0.RDT", "rb").read()
cam_off = struct.unpack_from("<I", rdt, 0x24)[0]
Rm, t, H = geom.cut_view(rdt, cam_off, 4)
cache = SC + "wandtiefe_c4.npy"
wd = np.load(cache) if os.path.exists(cache) else None
if wd is None:
    wd = np.asarray(geom.kollisionstiefe_schnell(Rm, t, H, geom.sca_wandzellen(rdt, band=0)), float)
    np.save(cache, wd)
sperr = (wd > 0) & (np.floor(wd) < bkfig)          # Wand VOR der Figur
print()
print("Kriterium des Zensus Runde 19 (\"solides Sperrfeld auf dem Sehstrahl\"):")
print("  verdeckte Figurpunkte gesamt          %4d" % int(v_ist.sum()))
print("  davon MIT solider Wand davor          %4d" % int((v_ist & sperr).sum()))
print("  davon OHNE                            %4d  = %.1f %%"
      % (int((v_ist & ~sperr).sum()), 100.0 * (v_ist & ~sperr).sum() / max(1, v_ist.sum())))

# Bild: Figur, Auslieferung vs. nur-Lasso
out = bg.copy()
out[gez] = [90, 90, 255]
out[v_ist] = [255, 0, 0]
c = out[45:130, 66:130]
Image.fromarray(c).resize((c.shape[1] * 9, c.shape[0] * 9), Image.NEAREST).save(SC + 'marke_ist.png')
out2 = bg.copy()
out2[gez] = [90, 90, 255]
out2[v_las] = [255, 0, 0]
c2 = out2[45:130, 66:130]
Image.fromarray(c2).resize((c2.shape[1] * 9, c2.shape[0] * 9), Image.NEAREST).save(SC + 'marke_lasso.png')
