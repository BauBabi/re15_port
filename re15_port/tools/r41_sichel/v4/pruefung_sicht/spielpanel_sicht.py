import sys, numpy as np, cv2
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, W + '/nb2neu'); sys.path.insert(0, W + '/v4')
import c6, v4_gemein as G
from PIL import Image
def crop(im, box, f):
    x0,y0,x1,y1 = box; return cv2.resize(im[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
def titel(im, t):
    im = im.copy(); cv2.putText(im, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255,255,0), 1, cv2.LINE_AA); return im
def reihe(ims, pad=6):
    h = max(i.shape[0] for i in ims); w = sum(i.shape[1] for i in ims) + pad*(len(ims)-1)
    out = np.full((h, w, 3), 40, np.uint8); x = 0
    for i in ims:
        out[:i.shape[0], x:x+i.shape[1]] = i; x += i.shape[1] + pad
    return out
rows = []
for h in (135, 118, 100):
    v = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB)
    p4 = cv2.cvtColor(cv2.imread('spiel_V4_h%d.png' % h), cv2.COLOR_BGR2RGB)
    p3 = cv2.cvtColor(cv2.imread('spiel_V3r2_h%d.png' % h), cv2.COLOR_BGR2RGB)
    bx = (240, 90, 560, 470)
    rows.append(reihe([titel(crop(v,bx,2),'Video n%d (roh, wie der Nutzer es sieht)' % (1350+h)), titel(crop(p4,bx,2),'Spiel V4 h%d' % h), titel(crop(p3,bx,2),'Spiel V3r2 h%d' % h)]))
    if h == 135:
        # Kopf x6 aus dem Spielbild: Kopf per Rotmaske/Silhouette grob: nehme Box um die Figur-Oberkante
        rows_k = reihe([titel(crop(v,(358,129,426,201),6),'Video n1485 Kopf x6'), titel(crop(p4,(300,120,400,240),4),'Spiel V4 h135 Kopfbereich x4'), titel(crop(p3,(300,120,400,240),4),'Spiel V3r2 h135 Kopfbereich x4')])
        Image.fromarray(rows_k).save('T_G_spiel_kopf.png')
h = max(r.shape[1] for r in rows); out = np.full((sum(r.shape[0] for r in rows)+12, h, 3), 40, np.uint8); y = 0
for r in rows: out[y:y+r.shape[0], :r.shape[1]] = r; y += r.shape[0] + 6
Image.fromarray(out).save('T_F_spiel_vs_video.png')
# dE00 Tabelle Schwanz-Achse und Kopf seitlich
import json
m = json.load(open('messung_sicht.json'))
print('Schwanz-Achse dE00 (orig n1435 5x5 roh vs Port Videoraum 5x5):')
for p, lo in m['schwanz_orig2'].items():
    row = [p, 'orig %s' % lo]
    for k in ('V4_85', 'V3r2_85', 'REF2_85'):
        q = m[k]['Schwanz_achse'][p]
        row.append('%s %s dE %.1f (d=%s)' % (k, q['lab'], G.de00(lo, q['lab']) if q['lab'] else 99, q['abstand']))
    print('  ', ' | '.join(row))
print('Kopf seitlich: orig Box 412-467/190-263 = 56x74 px = %.3f x %.3f H2(253)' % (56/253, 74/253))
for k in ('V4_85', 'V3r2_85', 'REF2_85'):
    b, h_ = m[k]['Kopf_BxH_px'], 264
    print('  %s Kopfbox %s BxH %s = %.3f x %.3f H_port(264); Kopf Lab %s vs orig %s dE %.1f' % (k, m[k]['Kopfbox'], b, b[0]/h_, b[1]/h_, m[k]['Kopf']['lab'], m['orig2']['Kopf']['lab'], G.de00(m['orig2']['Kopf']['lab'], m[k]['Kopf']['lab'])))
print('Vorn Regionen dE00 (orig Box vs Port Id):')
for r in ('Kopf','Rumpf','Oberarm_B_links','Oberarm_B_rechts','Unterarm_B_rechts','Jeans','Maulzone'):
    o = m['orig1'][r]['lab']
    print('  %-18s orig %s | ' % (r, o) + ' | '.join('%s %s dE %.1f' % (k, m[k][r]['lab'], G.de00(o, m[k][r]['lab'])) for k in ('V4','V3r2','REF2')))
