"""Stufe 3: Clip-Daten des RE2-G4 (BAHNSTEIG_G4.EMD = CDEMD0.EMS kind 0x34, EDD/Spur unveraendert laut R_bahnsteig.md 3)."""
import os, sys
HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, '../../../../..'))
sys.path.insert(0, os.path.join(WURZEL, 're15_port/tools'))
import form_emd as FE

def g4_roh():
    """RE2 CDEMD0.EMS kind 0x34 rec 3 (Sektor 3310, Datei 0x677000, 273812 Byte; TOC @0x8009adf4 ueber
    tools/birkin_bahnsteig.lade_rohdaten). EDD Paar 1 ab Datei 0x67b960, EMR Paar 1 ab 0x67d05c."""
    import birkin_bahnsteig as BB
    emd, _tim = BB.lade_rohdaten()
    return emd


def g4_lesen():
    return FE.lese_emd(g4_roh())

def clip_daten(f, ci):
    out = []
    for i, b in enumerate(f['clips'][ci]['bilder']):
        k = f['pool'][b['kf']]
        out.append(dict(bild=i, wort=b['wort'], wurzel=k['wurzel'], spur=k['spur']))
    return out

if __name__ == '__main__':
    f = g4_lesen()
    for ci, c in enumerate(f['clips']):
        d = clip_daten(f, ci)
        tr = [x['bild'] for x in d if x['wort'] & FE.BILD_TREFFER]
        ton = [(x['bild'], x['wort'] >> 28) for x in d if x['wort'] & FE.BILD_TON]
        sx = [x['spur'][0] for x in d]; sz = [x['spur'][2] for x in d]
        dx = [sx[i] - sx[i - 1] for i in range(1, len(sx))]
        andere = sorted(set(x['wort'] & ~(FE.BILD_TREFFER | FE.BILD_TON | 0xF0000000) & 0xFFFFF000 for x in d))
        wy = [x['wurzel'][1] for x in d]
        print('Clip %2d Bilder %3d treffer %s ton %s spur_x %d..%d (ende %d) spur_z ende %d tempo/bild mittel %.1f max %d  wurzel_y %d..%d andere %s' % (
            ci, len(d), tr, ton, min(sx), max(sx), sx[-1], sz[-1], (sx[-1] - sx[0]) / max(1, len(sx) - 1), max(dx) if dx else 0, min(wy), max(wy), [hex(a) for a in andere]))
