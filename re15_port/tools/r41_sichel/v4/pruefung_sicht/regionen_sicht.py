import numpy as np, cv2
from PIL import Image
o1 = np.asarray(Image.open('vorlage1_entbl.png').convert('RGB')).copy()
BOX1 = {'Kopf': (368,139,416,191), 'Rumpf': (352,196,448,292), 'OA_Bl': (300,186,345,250), 'OA_Br': (455,196,500,262),
        'UA_Br': (468,268,508,330), 'Jeans': (362,298,428,344), 'US_Bl': (356,352,392,430),
        'Maul': (370,183,412,206), 'Sch_Bl': (318,160,360,190), 'Sch_Br': (425,165,470,195)}
for k, (x0,y0,x1,y1) in BOX1.items():
    cv2.rectangle(o1, (x0,y0), (x1,y1), (255,255,0), 1); cv2.putText(o1, k, (x0, y0-2), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (0,255,255), 1, cv2.LINE_AA)
o2 = np.asarray(Image.open('vorlage2_entbl.png').convert('RGB')).copy()
BOX2 = {'Kopf': (412,190,467,263), 'Buckel': (372,186,414,215), 'OA_nah': (395,250,440,300), 'Jeans': (405,300,440,340)}
for k, (x0,y0,x1,y1) in BOX2.items():
    cv2.rectangle(o2, (x0,y0), (x1,y1), (255,255,0), 1); cv2.putText(o2, k, (x0, y0-2), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (0,255,255), 1, cv2.LINE_AA)
for p in [(330,236),(310,243),(292,262),(289,280),(288,300),(287,318),(290,335)]:
    cv2.rectangle(o2, (p[0]-2,p[1]-2), (p[0]+2,p[1]+2), (0,255,0), 1)
v4 = np.asarray(Image.open('V4_135.png').convert('RGB')).copy(); ids = np.load('V4_135_ids.npy')[:480]
rng = np.random.RandomState(3); pal = rng.randint(60, 255, (32, 3)).astype(np.uint8)
ov = v4.copy(); m = ids >= 0; ov[m] = (0.5*v4[m] + 0.5*pal[ids[m]]).astype(np.uint8)
for i in np.unique(ids[ids >= 0]):
    ys, xs = np.nonzero(ids == i); cv2.putText(ov, str(i), (int(xs.mean()), int(ys.mean())), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,255,255), 1, cv2.LINE_AA)
def c(im, b, f=2): x0,y0,x1,y1 = b; return cv2.resize(im[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
out = np.full((760, 3*640+12, 3), 40, np.uint8)
out[:760, :640] = c(o1, (240,100,560,480)); out[:620, 646:1286] = c(o2, (240,150,560,460)); out[:760, 1292:1932] = c(ov, (240,100,560,480))
Image.fromarray(out).save('P1S_regionen.png'); print('ok')
