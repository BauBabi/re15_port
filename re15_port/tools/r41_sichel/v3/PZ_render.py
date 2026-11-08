# Zahlen-Pruefer V3 r1: Renders (nur Zeichen-Infrastruktur BF.nah_raster/EA.render/b2.zeichne), Ausgabe npz je EMD.
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import emd_ansichtsblatt as EA, birkin_formen as BF, b2, birkin_buehne as BB
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
OUT = os.environ.get('PZ_OUT', SC + '/sfv3/pz/')

def nah(mo, breite=1200, cx=720):
    NAH = dict(BF.KOPF_V3['nah'], breite=breite, cx=cx)
    r = BF.nah_raster(mo, NAH)
    col, alp = EA.render(mo, r['V'] - r['mitte'], r['N'], r['F'], np.array([0, 0, 1.0]), np.array([0.0, 1.0, 0.0]), np.array([1.0, 0, 0]),
                         NAH['breite'], NAH['hoehe'], NAH['massstab'], NAH['cx'], NAH['cy'])
    return (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8), alp, r['knochen']

if __name__ == '__main__':
    name, emd = sys.argv[1], sys.argv[2]
    mo = b2.modell(emd)
    b, alp, kn = nah(mo)
    cv2.imwrite(OUT + 'nah_%s.png' % name, cv2.cvtColor(b, cv2.COLOR_RGB2BGR))
    d = dict(nah=b, nah_alp=alp, nah_kn=kn)
    PS = json.load(open(SC + '/sfv2/posen_seite.json')); PV = json.load(open(SC + '/sfv2/posen_vorn.json'))
    for h in ('83', '85', '92'):
        im, ids = b2.zeichne(mo, PS[h], None)
        imf, idf = b2.zeichne(mo, PS[h], None, knochen=tuple(range(16, 23)))
        kf = BB.kf_von_clip(mo, PS[h]['clip'], PS[h]['bild'])
        j = b2.projiziere(PS[h], b2.kette_punkte(mo, kf))
        d['s%s' % h] = im; d['s%s_ids' % h] = ids; d['s%s_f' % h] = imf; d['s%s_fids' % h] = idf; d['s%s_j' % h] = j
        cv2.imwrite(OUT + 's%s_%s.png' % (h, name), cv2.cvtColor(im, cv2.COLOR_RGB2BGR))
    for h in sorted(PV):
        im, ids = b2.zeichne(mo, PV[h], None)
        d['v%s' % h] = im; d['v%s_ids' % h] = ids
        cv2.imwrite(OUT + 'v%s_%s.png' % (h, name), cv2.cvtColor(im, cv2.COLOR_RGB2BGR))
    np.savez_compressed(OUT + 'r_%s.npz' % name, **d)
    print(name, 'ok', sorted(PV))
