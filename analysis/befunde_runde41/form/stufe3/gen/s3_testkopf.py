"""Stufe 3: schreibt re15_port/tests/unit/form_j_posen.h, die Formtreue-Referenz fuer test_birkin_bahnsteig.c.

    python s3_testkopf.py

Aus varianten/J_dicker/FORM.EMD (vom Nutzer abgenommen, E19): die drei Posen von J (Wurzel + 32 Winkeltripel) und
CRC32/Laenge von MD1 und TIM. Der Test prueft damit, dass das Spiel-Asset Gestalt, Textur und die Screenshot-Posen von J
unveraendert traegt. Kein RE-Wert.
"""
import os, sys, zlib
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_emd as FE       # noqa: E402

AUS = os.path.join(S.WURZEL, 're15_port', 'tests', 'unit', 'form_j_posen.h')


def main():
    emd = open(os.path.join(S.J_DIR, 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(S.J_DIR, 'FORM.TIM'), 'rb').read()
    f = FE.lese_emd(emd)
    md1 = FE.sektionen(emd)['md1']
    namen = {0: 'BILD1', 1: 'BILD2', 2: 'RUHE'}
    z = ['/* form_j_posen.h - ERZEUGT von analysis/befunde_runde41/form/stufe3/gen/s3_testkopf.py, nicht von Hand aendern.',
         ' * Referenz Formtreue: Variante J (varianten/J_dicker/FORM.EMD, Nutzer-Entscheid E19). Kein RE-Wert. */',
         '#ifndef FORM_J_POSEN_H', '#define FORM_J_POSEN_H', '#include <stdint.h>', '',
         '#define FORM_J_MD1_LEN %du' % len(md1), '#define FORM_J_MD1_CRC 0x%08xu' % (zlib.crc32(md1) & 0xffffffff),
         '#define FORM_J_TIM_LEN %du' % len(tim), '#define FORM_J_TIM_CRC 0x%08xu' % (zlib.crc32(tim) & 0xffffffff), '']
    for ci, c in enumerate(f['clips']):
        k = f['pool'][c['bilder'][0]['kf']]
        n = namen[ci]
        z.append('static const int16_t FORM_J_%s_WURZEL[3] = { %d, %d, %d };' % (n, *k['wurzel']))
        z.append('static const int16_t FORM_J_%s_WINKEL[32][3] = {' % n)
        for i in range(0, 32, 4):
            z.append('    ' + ' '.join('{ %d, %d, %d },' % tuple(a) for a in k['winkel'][i:i + 4]))
        z.append('};')
    # Hoehe im Gehen (Clip 5, jedes 4. Bild, hoechster Punkt ueber dem Boden), am Spiel-Asset gemessen wie der Test
    spiel = FE.lese_emd(open(os.path.join(S.WURZEL, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_FORM.EMD'), 'rb').read())
    r = S.Rig()
    hs = []
    for i, b in enumerate(spiel['clips'][5]['bilder']):
        if i % 4:
            continue
        P = r.punkte(r.aus_keyframe(spiel['pool'][b['kf']]))
        hs.append(-min(float(x[:, 1].min()) for x in P if len(x)))
    z.append('')
    z.append('/* Hoehe der Riesenform im Gehen (Clip 5 des Spiel-Assets, Mittel ueber jedes 4. Bild), Formtreue-Pin. */')
    z.append('#define FORM_HOEHE_GEHEN %d' % int(round(sum(hs) / len(hs))))
    z += ['', '#endif', '']
    open(AUS, 'w', encoding='utf-8', newline='\n').write('\n'.join(z))
    print(AUS)


if __name__ == '__main__':
    main()
