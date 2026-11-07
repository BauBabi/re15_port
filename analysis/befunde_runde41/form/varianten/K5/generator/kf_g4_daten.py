#!/usr/bin/env python3
"""Messdaten fuer kf_g4.py / riegel_farbe_v3.py nach generator/v3_daten (einmalig, aus den Lesungen V3):
- Masken der Feuerstellen (Pruefer, MESSBLATT_FARBE_V3 3.1/3.2; Scratchpad formfarbe3/pruef): F1_pruef (Bild 1, 646 px), band2_pruef (Bild 2, 2227 px),
  zahn2_pruef (Bild 2, 408 px).
- Flaechen-ID je Pixel (Leser B flaechen_id.py, formfarbe3/leserb/fid1.npz, fid2.npz; 99,95 % gleich form_pruefung.teilbild): je Flaeche der Anteil
  ihrer sichtbaren Pixel, der im Band bzw. in F1 liegt, und die Knochenraum-Punkte (lok) der Maskenpixel.
Ausgabe: v3_daten/sonderfelder.json, v3_daten/lokpunkte.npz, v3_daten/masken.npz."""
import collections
import json
import os
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3'
AUS = os.path.join(HIER, 'v3_daten')
BAND_SCHWELLE = 0.3   # PORT-WAHL Bauer: Flaeche gehoert zum Band, wenn >= 30 % ihrer in Bild 2 sichtbaren Pixel im Band liegen
                      # (Verteilung kf_g4 t4.py: 0,2 -> 43 Fl. / 1593 px, 0,3 -> 39 / 1546, 0,5 -> 34 / 1447 von 1673 Bandpixeln auf 22/23)


def main():
    os.makedirs(AUS, exist_ok=True)
    F1 = np.load(S + '/pruef/F1_pruef.npy')
    band = np.load(S + '/pruef/band2_pruef.npy')
    zahn = np.load(S + '/pruef/zahn2_pruef.npy')
    np.savez_compressed(os.path.join(AUS, 'masken.npz'), F1=np.packbits(F1), band=np.packbits(band), zahn=np.packbits(zahn), form=np.array(F1.shape))
    out = dict(quelle=dict(masken='formfarbe3/pruef (Pruefer V3, orte.py)', fid='formfarbe3/leserb/fid1.npz, fid2.npz (Leser B flaechen_id.py)'),
               band_schwelle=BAND_SCHWELLE)
    lok = {}
    for n, msk, nm, knochen in ((1, F1, 'F1', (0,)), (2, band, 'band', (22, 23)), (2, zahn, 'zahn', (22, 23))):
        d = np.load(S + '/leserb/fid%d.npz' % n)
        kn, fa = d['kn'], d['fa']
        c = collections.Counter(zip(kn[msk].tolist(), fa[msk].tolist()))
        tot = collections.Counter(zip(kn.ravel().tolist(), fa.ravel().tolist()))
        out['%s_flaechen' % nm] = sorted([[int(b), int(f), round(c[(b, f)] / tot[(b, f)], 3)] for (b, f) in c if b in knochen],
                                         key=lambda x: -x[2])
        out['%s_px' % nm] = int(msk.sum())
        out['%s_px_knochen' % nm] = {str(k): int(v) for k, v in collections.Counter(kn[msk].tolist()).items()}
        sel = msk & np.isin(kn, knochen)
        lok['%s_lok' % nm] = d['lok'][sel].astype(np.float32)
        lok['%s_kn' % nm] = kn[sel].astype(np.int16)
    np.savez_compressed(os.path.join(AUS, 'lokpunkte.npz'), **lok)
    json.dump(out, open(os.path.join(AUS, 'sonderfelder.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print({k: v for k, v in out.items() if not k.endswith('_flaechen')})


if __name__ == '__main__':
    main()
