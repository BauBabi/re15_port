import numpy as np, cv2
from PIL import Image
def L(p): return np.asarray(Image.open(p).convert('RGB'))
def crop(im, box, f):
    x0,y0,x1,y1 = box; c = im[y0:y1, x0:x1]
    return cv2.resize(c, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
def titel(im, t):
    im = im.copy(); cv2.putText(im, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255,255,0), 1, cv2.LINE_AA); return im
def reihe(ims, pad=6):
    h = max(i.shape[0] for i in ims); w = sum(i.shape[1] for i in ims) + pad*(len(ims)-1)
    out = np.full((h, w, 3), 40, np.uint8); x = 0
    for i in ims:
        out[:i.shape[0], x:x+i.shape[1]] = i; x += i.shape[1] + pad
    return out
v1 = L('vorlage1_entbl.png'); v2 = L('vorlage2_entbl.png')
V4a = L('V4_135.png'); V3a = L('V3r2_135.png'); R2a = L('REF2_135.png')
V4b = L('V4_85.png'); V3b = L('V3r2_85.png'); R2b = L('REF2_85.png')
# A: vorn ganze Figur x2
bx = (240, 100, 560, 480)
Image.fromarray(reihe([titel(crop(v1,bx,2),'Vorlage 1 n1485 entbl.'), titel(crop(V4a,bx,2),'V4 Pose 135'), titel(crop(R2a,bx,2),'Referenz 2 Pose 135')])).save('T_A_vorn_x2.png')
# B: seitlich x2
bx = (240, 150, 560, 460)
Image.fromarray(reihe([titel(crop(v2,bx,2),'Vorlage 2 n1435'), titel(crop(V4b,bx,2),'V4 Pose 85'), titel(crop(R2b,bx,2),'Referenz 2 Pose 85')])).save('T_B_seite_x2.png')
# C: Kopf vorn x6: Original Kopfbox 368-416/139-191 +10; V4 Kopf: ids==9 bbox +10
ids4 = np.load('V4_135_ids.npy')[:480]
ys, xs = np.nonzero(ids4 == 9); k4 = (xs.min()-10, ys.min()-10, xs.max()+10, ys.max()+10)
print('V4 Kopfbox vorn', xs.min(), ys.min(), xs.max(), ys.max())
Image.fromarray(reihe([titel(crop(v1,(358,129,426,201),6),'Vorlage 1 Kopf x6'), titel(crop(V4a,k4,6),'V4 Kopf x6')])).save('T_C_kopf_vorn_x6.png')
ids4s = np.load('V4_85_ids.npy')[:480]
ys, xs = np.nonzero(ids4s == 9); k4s = (xs.min()-10, ys.min()-10, xs.max()+10, ys.max()+10)
print('V4 Kopfbox seite', xs.min(), ys.min(), xs.max(), ys.max())
Image.fromarray(reihe([titel(crop(v2,(402,180,477,273),6),'Vorlage 2 Kopf x6'), titel(crop(V4b,k4s,6),'V4 Kopf seitl. x6')])).save('T_D_kopf_seite_x6.png')
# E: neutral rundum V4 vs REF2
Image.fromarray(reihe([titel(L('V4_85_neutral.png'),'V4 neutral'), titel(L('REF2_85_neutral.png'),'Referenz 2 neutral'), titel(L('V3r2_85_neutral.png'),'V3r2 neutral')])).save('T_E_neutral.png')
print('ok')
