"""Zahlen-Pruefer: Kopf-Riegel K1-K10 direkt am SPIELBILD (FRAMEDUMP h118/h125/h135, entzerrt), Kopfbox aus dem b2-Render der Laufpose."""
import sys, os, re, numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
import pz_mess as PM
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
r_, kam, M, li = b2.raum()
Hf = np.asarray(M, float) @ np.array([[1 / 3.0, 0, BB.PHASE], [0, 1 / 3.0, BB.PHASE], [0, 0, 1]])
mo = b2.modell(G.EMD_V4)
txt = open(S + 'r/V4/debug.log', encoding='latin-1').read()
for h in (118, 125, 135):
    m = re.search(r'\[hangar-takt\] h%d F(\d+) B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)' % h, txt)
    F, c, b, g, x, z = map(int, m.groups())
    img, ids, idv = G.zeichne(mo, dict(clip=c, bild=b, gier=g, x=x, z=z), voll=True)
    fd = cv2.cvtColor(cv2.imread(S + 'r/V4/fd/f%06d.ppm' % F), cv2.COLOR_BGR2RGB)
    fdv = cv2.warpPerspective(fd, Hf, (640, 480), flags=cv2.INTER_AREA)
    for nm, bild in (('b2', img), ('Spiel', fdv)):
        o = PM.kopf_vorn(bild, ids, idv, nm)
        print('h%d %-5s H %d Kopfbox %s | K1 komp %s Kontrast %.1f | K2 %s dE %.1f | K3 %.3f x %.3f | K4 %.3f / %.3f | K5 %s dE %.1f | K6 L %.1f | K7 dE %.1f | K8 dE %.1f Anteil %.2f | K9 %s dE %.1f | K10 %.3f x %.3f' % (
            h, nm, o['H'], o['kopfbox'], o['K1']['komps'][:3], o['K1']['kontrast'], o['K2']['lab'], o['K2']['dE'], o['K3']['w'], o['K3']['h'],
            o['K4']['links'], o['K4']['unter_ok'], o['K5']['lab'], o['K5']['dE'], o['K6']['L'], o['K7']['dE'], o['K8']['dE'], o['K8']['anteil35'],
            o['K9']['lab'], o['K9']['dE'], o['K10']['w'], o['K10']['h']))
    kb = o['kopfbox']
    crop = np.hstack([img[kb[1] - 8:kb[3] + 9, kb[0] - 8:kb[2] + 9], fdv[kb[1] - 8:kb[3] + 9, kb[0] - 8:kb[2] + 9]])
    cv2.imwrite(S + 'pz/spielkopf_h%d.png' % h, cv2.resize(cv2.cvtColor(crop, cv2.COLOR_RGB2BGR), None, fx=5, fy=5, interpolation=cv2.INTER_NEAREST))
