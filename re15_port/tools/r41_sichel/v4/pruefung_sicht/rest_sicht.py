import sys, numpy as np, cv2, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
out = {}
o1 = L('vorlage1_entbl.png'); v4 = G.videoraum(L('V4_135.png')); ids = np.load('V4_135_ids.npy')[:480]
kbo = (368,139,416,191); kb4 = (327,154,378,221)
def um(p):
    u = (p[0]-kbo[0])/(kbo[2]-kbo[0]); v = (p[1]-kbo[1])/(kbo[3]-kbo[1]); return (int(round(kb4[0]+u*(kb4[2]-kb4[0]))), int(round(kb4[1]+v*(kb4[3]-kb4[1]))))
def f5(img, p): x,y = p; return G.r1(G.lab(img[y-2:y+3, x-2:x+3].reshape(-1,3)).mean(0))
for name, p in (('Auge', (382,164)), ('Haare', (404,160)), ('Nase', (395,177))):
    q = um(p); out['kopf_vorn_'+name] = dict(orig_pkt=p, orig_lab5=f5(o1, p), v4_pkt=q, v4_lab5=f5(v4, q), v4_id=int(ids[q[1], q[0]]))
# hellster und dunkelster 5x5-Punkt im V4-Kopf (Videoraum) und im Original-Kopf
def extrem(img, maske):
    Lb = cv2.blur(G.lab(img)[..., 0].astype(np.float32), (5,5)); Lb2 = np.where(maske, Lb, np.nan)
    hi = np.unravel_index(np.nanargmax(Lb2), Lb2.shape); lo = np.unravel_index(np.nanargmin(Lb2), Lb2.shape)
    return dict(hellst=((int(hi[1]), int(hi[0])), round(float(Lb2[hi]),1)), dunkelst=((int(lo[1]), int(lo[0])), round(float(Lb2[lo]),1)))
mo = np.zeros(o1.shape[:2], bool); mo[kbo[1]+3:kbo[3]-2, kbo[0]+3:kbo[2]-2] = True
out['kopf_vorn_extrema_orig'] = extrem(o1, mo)
m4 = cv2.erode((ids == 9).astype(np.uint8), np.ones((5,5), np.uint8)).astype(bool); out['kopf_vorn_extrema_v4'] = extrem(v4, m4)
# Schwanz Textur breiter Teil: Original Box (300,228,340,248) nur Schwanzpixel (R-G>=5 & Lum<120), V4 Pose 85 Ids 16-22 in derselben Box
o2 = L('vorlage2_entbl.png'); v485 = G.videoraum(L('V4_85.png')); ids85 = np.load('V4_85_ids.npy')[:480]
bx = (300,226,342,250)
sub = o2[bx[1]:bx[3], bx[0]:bx[2]]; lum = sub @ np.array([0.299,0.587,0.114]); mk = ((sub[...,0]-sub[...,1]) >= 5) & (lum < 120)
out['schwanz_textur_orig'] = dict(n=int(mk.sum()), lum_std=round(float(lum[mk].std()),1), L_std=round(float(G.lab(sub[mk])[:,0].std()),1))
sub4 = v485[bx[1]:bx[3], bx[0]:bx[2]]; mk4 = np.isin(ids85[bx[1]:bx[3], bx[0]:bx[2]], range(16,23)); lum4 = sub4 @ np.array([0.299,0.587,0.114])
out['schwanz_textur_v4'] = dict(n=int(mk4.sum()), lum_std=round(float(lum4[mk4].std()),1), L_std=round(float(G.lab(sub4[mk4])[:,0].std()),1))
# auch ungeblurrt (Port-Schirm), damit die Blur-Regel die Textur nicht wegnimmt
r4 = L('V4_85.png'); sub4r = r4[bx[1]:bx[3], bx[0]:bx[2]]; lum4r = sub4r @ np.array([0.299,0.587,0.114])
out['schwanz_textur_v4_schirm'] = dict(lum_std=round(float(lum4r[mk4].std()),1))
# schwarzer Lappen Kopf seitlich: Pixel des Kopfes (Id 9) mit L* < 8 (Port-Schirm) in x >= 470
lab85 = G.lab(r4); k9 = ids85 == 9; dunkel = k9 & (lab85[...,0] < 8); ys, xs = np.nonzero(dunkel & (np.arange(640)[None,:] >= 470))
out['kopf_seite_schwarz_x>=470'] = dict(n=int(len(xs)), bbox=(int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())) if len(xs) else None)
ys, xs = np.nonzero(dunkel); out['kopf_seite_schwarz_gesamt'] = dict(n=int(len(xs)), anteil_kopf=round(float(len(xs)/k9.sum()),3))
# Original Vorderkante seitlich: Lum-Minimum Zeile y244 x 455-470 (Leser A: x462-465, Lum 23-40)
lo = o2 @ np.array([0.299,0.587,0.114]); z = lo[240:249, 455:471].mean(0); out['orig_vorderkante_y240-248'] = dict(x_min=int(455+np.argmin(z)), lum_min=round(float(z.min()),1), lum_x455=round(float(z[0]),1), lum_x470=round(float(z[-1]),1))
json.dump(out, open('rest_sicht.json','w'), indent=1, default=str)
for k, v in out.items(): print(k, v)
