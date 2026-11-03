"""Nachbesserung 2, Textur-Spur, Hilfe: Spiel-Asset mit neuer Textur fuer Messung und echtes Fenster, OHNE den Generator (s3_clips) zu
laufen. Nimmt Clips/Gerippe eines vorhandenen Spiel-Assets und setzt Meshes (MD1) und TIM einer Basis ein (beide Basen haben dieselbe
Geometrie, nur UV/Seite/CLUT und TIM unterscheiden sich).

    python s3_n2_t_asset.py BASIS_DIR SPIEL_EMD AUS_DIR      -> AUS_DIR/BAHNSTEIG_FORM.EMD/.TIM
"""
import os, sys
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig  # noqa: F401,E402  (Pfad zu re15_port/tools)
import form_emd as FE  # noqa: E402


def main():
    basis, spiel, aus = sys.argv[1:4]
    b = FE.lese_emd(open(os.path.join(basis, 'FORM.EMD'), 'rb').read())
    s = FE.lese_emd(open(spiel, 'rb').read())
    if [len(m['ecken']) for m in b['meshes']] != [len(m['ecken']) for m in s['meshes']]:
        raise SystemExit('Geometrie der Basis passt nicht zum Spiel-Asset')
    s['meshes'] = b['meshes']
    os.makedirs(aus, exist_ok=True)
    open(os.path.join(aus, 'BAHNSTEIG_FORM.EMD'), 'wb').write(FE.schreibe_emd(s))
    open(os.path.join(aus, 'BAHNSTEIG_FORM.TIM'), 'wb').write(open(os.path.join(basis, 'FORM.TIM'), 'rb').read())
    print('Asset', aus)


if __name__ == '__main__':
    main()
