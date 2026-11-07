"""Pruefung V4 r2 (Sicht): Ausschnitt-Tafeln Vorlage | V4 Buehne | V4 Spiel (x2/x4/x6, INTER_NEAREST)."""
import sys, os, numpy as np, cv2
from PIL import Image
OUT = sys.argv[1]; os.chdir(OUT)
def L(p): return np.asarray(Image.open(p).convert('RGB'))
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB)
def crop(im, box, f):
    x0,y0,x1,y1 = box; c = im[max(y0,0):y1, max(x0,0):x1]
    return cv2.resize(c, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
def titel(im, t):
    im = im.copy(); cv2.putText(im, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255,255,0), 1, cv2.LINE_AA); return im
def reihe(ims, pad=6):
    h = max(i.shape[0] for i in ims); w = sum(i.shape[1] for i in ims) + pad*(len(ims)-1)
    out = np.full((h, w, 3), 40, np.uint8); x = 0
    for i in ims: out[:i.shape[0], x:x+i.shape[1]] = i; x += i.shape[1] + pad
    return out
def spalte(rows, pad=6):
    w = max(r.shape[1] for r in rows); out = np.full((sum(r.shape[0] for r in rows)+pad*(len(rows)-1), w, 3), 40, np.uint8); y = 0
    for r in rows: out[y:y+r.shape[0], :r.shape[1]] = r; y += r.shape[0] + pad
    return out
v1 = L('vorlage1_entbl.png'); v2 = L('vorlage2_entbl.png')
V4a = L('V4_135.png'); V4b = L('V4_85.png'); R2a = L('REF2_135.png'); R2b = L('REF2_85.png'); V3b = L('V3r2_85.png'); V3a = L('V3r2_135.png')
sp = {h: S('spiel_V4r2_h%d.png' % h) for h in (85, 100, 118, 135)}
vid = {h: S('video_h%d.png' % h) for h in (85, 100, 118, 135)}
bx = (240, 100, 560, 480)
Image.fromarray(reihe([titel(crop(v1,bx,2),'Vorlage 1 n1485 entbl.'), titel(crop(V4a,bx,2),'V4 r2 Pose 135 (Buehne)'), titel(crop(sp[135],bx,2),'V4 r2 Spiel h135'), titel(crop(R2a,bx,2),'Referenz 2 Pose 135')])).save('R2_A_vorn_x2.png')
bx = (240, 150, 560, 460)
Image.fromarray(reihe([titel(crop(v2,bx,2),'Vorlage 2 n1435'), titel(crop(V4b,bx,2),'V4 r2 Pose 85 (Buehne)'), titel(crop(sp[85],bx,2),'V4 r2 Spiel h85'), titel(crop(R2b,bx,2),'Referenz 2 Pose 85')])).save('R2_B_seite_x2.png')
ids4 = np.load('V4_135_ids.npy')[:480]; ys, xs = np.nonzero(ids4 == 9); k4 = (xs.min()-10, ys.min()-10, xs.max()+10, ys.max()+10); print('V4 Kopfbox vorn', xs.min(), ys.min(), xs.max(), ys.max())
Image.fromarray(reihe([titel(crop(v1,(358,129,426,201),6),'Vorlage 1 Kopf x6'), titel(crop(V4a,k4,6),'V4 r2 Kopf x6 (Buehne)'), titel(crop(sp[135],(300,120,400,240),4),'V4 r2 Spiel h135 x4')])).save('R2_C_kopf_vorn_x6.png')
ids4s = np.load('V4_85_ids.npy')[:480]; ys, xs = np.nonzero(ids4s == 9); k4s = (xs.min()-10, ys.min()-10, xs.max()+10, ys.max()+10); print('V4 Kopfbox seite', xs.min(), ys.min(), xs.max(), ys.max())
Image.fromarray(reihe([titel(crop(v2,(402,180,477,273),6),'Vorlage 2 Kopf x6'), titel(crop(V4b,k4s,6),'V4 r2 Kopf seitl. x6 (Buehne)'), titel(crop(sp[85],(400,150,520,290),4),'V4 r2 Spiel h85 x4')])).save('R2_D_kopf_seite_x6.png')
bx = (260, 180, 380, 360)
Image.fromarray(reihe([titel(crop(v2,bx,3),'Vorlage 2 Schwanz x3'), titel(crop(V4b,bx,3),'V4 r2 Pose 85 Schwanz x3'), titel(crop(sp[85],bx,3),'V4 r2 Spiel h85 x3'), titel(crop(V4b,(230,160,480,360),2),'V4 r2 Pose 85 x2')])).save('R2_E_schwanz_x3.png')
rows = []
for h in (135, 118, 100, 85):
    bx = (240, 90, 560, 470)
    rows.append(reihe([titel(crop(vid[h],bx,2),'Video n%d roh' % (1350+h)), titel(crop(sp[h],bx,2),'Spiel V4 r2 h%d' % h), titel(crop(S('spiel_V3r2_h%d.png' % h),bx,2),'Spiel V3r2 h%d' % h)]))
Image.fromarray(spalte(rows)).save('R2_F_spiel_vs_video.png')
Image.fromarray(reihe([titel(L('V4_85_neutral.png'),'V4 r2 neutral 85'), titel(L('V4_135_neutral.png'),'V4 r2 neutral 135'), titel(L('REF2_135_neutral.png'),'Referenz 2 neutral 135')])).save('R2_G_neutral.png')
# Spiel-Kopf x6 h135 und h118
Image.fromarray(reihe([titel(crop(vid[135],(358,129,426,201),6),'Video n1485 Kopf x6'), titel(crop(sp[135],(310,130,390,215),6),'Spiel V4 r2 h135 Kopf x6'), titel(crop(vid[118],(300,120,400,220),4),'Video n1468 x4'), titel(crop(sp[118],(300,120,400,220),4),'Spiel V4 r2 h118 x4')])).save('R2_H_spiel_kopf_x6.png')
print('ok')
