"""vergleich.py <cam> <hjson> <bg.ppm> <frame.jpg> <fx> <fy> <yaw> <aus.jpg> <crop x y w h> <gamma> modell:clip:bild[:skala] ...
Links Videobild, daneben die Modelle in derselben Kamera auf dem Hintergrund."""
import sys
exec(open(__file__.replace('vergleich.py', 'buehne.py'), encoding='utf-8').read())
from PIL import Image, ImageDraw, ImageFont

cam_key, hj, bgp, frp = sys.argv[1:5]
fx, fy, yaw = float(sys.argv[5]), float(sys.argv[6]), float(sys.argv[7])
aus = sys.argv[8]
cx, cy, cw, ch = [int(v) for v in sys.argv[9:13]]
gamma = float(sys.argv[13])
cam = Cam(cam_key, hj)
fr = cv2.cvtColor(cv2.imread(frp), cv2.COLOR_BGR2RGB)
H, W = fr.shape[:2]
SS = 4
bg = cv2.cvtColor(cv2.imread(bgp), cv2.COLOR_BGR2RGB)
bgv = cv2.warpPerspective(bg, cam.B, (W, H), flags=cv2.INTER_LINEAR).astype(np.float32)/255
for k in range(3):
    a, b = bgv[..., k], fr[..., k].astype(np.float32)/255
    bgv[..., k] = a*((a*b).sum()/max((a*a).sum(), 1e-6))
pos = cam.vid2boden(fx, fy)
font = ImageFont.truetype('arial.ttf', 20)
tafeln = []


def tafel(arr, txt):
    a = (np.clip(arr, 0, 1)**gamma*255).astype(np.uint8)
    a = a[cy*SS:(cy+ch)*SS, cx*SS:(cx+cw)*SS]
    im = Image.fromarray(a)
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 8+d.textlength(txt, font=font), 26), fill=(0, 0, 0))
    d.text((4, 2), txt, fill=(255, 255, 0), font=font)
    tafeln.append(im)


vg = cv2.resize(fr, (W*SS, H*SS), interpolation=cv2.INTER_LANCZOS4).astype(np.float32)/255
tafel(vg, 'Video')
for spec in sys.argv[14:]:
    t = spec.split(':')
    name, clip, bild_i = t[0], int(t[1]), int(t[2])
    sk = float(t[3]) if len(t) > 3 else 1.0
    y_ = float(t[4]) if len(t) > 4 else yaw
    mo = lade(name)
    first, cnt = mo.clips[clip]
    kf = mo.frames[first + bild_i % cnt] & 0xfff
    V, N, F = mo.geometrie(kf)
    Vw = dreh_y(V - np.array([V[:, 0].mean(), 0, V[:, 2].mean()]), y_)*sk + np.array([pos[0], 0, pos[2]])
    Nw = dreh_y(N, y_)
    col, alp = bild(mo, cam, Vw, Nw, F, W, H, ss=SS)
    grund = cv2.resize(bgv, (W*SS, H*SS), interpolation=cv2.INTER_LINEAR)
    misch = grund*(1-alp[..., None]) + col*alp[..., None]
    pv, z = cam.welt2vid(Vw)
    print(spec, 'Hoehe', round(-Vw[:, 1].min()), 'Bild y', round(pv[:, 1].min(), 1), round(pv[:, 1].max(), 1), 'x', round(pv[:, 0].min(), 1), round(pv[:, 0].max(), 1), 'z', round(z.mean()))
    tafel(misch, spec)
ges = Image.new('RGB', (sum(i.width for i in tafeln), tafeln[0].height))
x = 0
for i in tafeln:
    ges.paste(i, (x, 0))
    x += i.width
ges.save(aus, quality=93)
print(aus, ges.size)
