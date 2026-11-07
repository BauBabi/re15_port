"""Stufe 3: schreibt re15_port/tests/unit/form_j_posen.h, die Formtreue-Referenz fuer test_birkin_bahnsteig.c.

    python s3_testkopf.py

Seit Nachbesserung 1 (E20/E21): Referenz ist die Basis des Rigs, stufe3/basis_n1/FORM.EMD/.TIM = varianten/K_alligator/K3_fit (J mit
Alligator-Kopf K3, Krallen-Ton, Gelenkkoerpern; Gerippe gleich J) mit neuer Textur-UV der Gelenkkoerper (s3_n1_textur.py): CRC32/Laenge von MD1 und TIM, dazu die drei Posen des Rigs (Wurzel + 32 Winkeltripel):
Bild 1 und Bild 2 aus K3 (Kopfwinkel von K3), Ruhe = Ruhe von J mit geschlossenem Kiefer (s3_rig.RUHE_KIEFER). Der Test prueft damit,
dass das Spiel-Asset Gestalt, Textur und die Screenshot-Posen unveraendert traegt. Die Makronamen FORM_J_* bleiben. Kein RE-Wert.
"""
import os, sys, zlib
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_emd as FE       # noqa: E402

AUS = os.path.join(S.WURZEL, 're15_port', 'tests', 'unit', 'form_j_posen.h')


def main():
    emd = open(os.path.join(S.BASIS_DIR, 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(S.BASIS_DIR, 'FORM.TIM'), 'rb').read()
    md1 = FE.sektionen(emd)['md1']
    r = S.Rig()
    namen = {0: 'BILD1', 1: 'BILD2', 2: 'RUHE'}
    z = ['/* form_j_posen.h - ERZEUGT von analysis/befunde_runde41/form/stufe3/gen/s3_testkopf.py, nicht von Hand aendern.',
         ' * Referenz Formtreue: Variante J mit Kopf K4 Krokodil (stufe3/basis_n2 = K4_kroko + Textur-Bau s3_n2_textur, E19-E22): MD1/TIM,',
         ' * Posen Bild 1/Bild 2 von K4, Ruhe = Ruhe von J mit Kiefer zu. Makronamen FORM_J_* wie bisher. Kein RE-Wert. */',
         '#ifndef FORM_J_POSEN_H', '#define FORM_J_POSEN_H', '#include <stdint.h>', '',
         '#define FORM_J_MD1_LEN %du' % len(md1), '#define FORM_J_MD1_CRC 0x%08xu' % (zlib.crc32(md1) & 0xffffffff),
         '#define FORM_J_TIM_LEN %du' % len(tim), '#define FORM_J_TIM_CRC 0x%08xu' % (zlib.crc32(tim) & 0xffffffff), '']
    for ci in range(3):
        k = r.keyframe((r.bild1, r.bild2, r.ruhe)[ci])
        n = namen[ci]
        z.append('static const int16_t FORM_J_%s_WURZEL[3] = { %d, %d, %d };' % (n, *k['wurzel']))
        z.append('static const int16_t FORM_J_%s_WINKEL[32][3] = {' % n)
        for i in range(0, 32, 4):
            z.append('    ' + ' '.join('{ %d, %d, %d },' % tuple(a) for a in k['winkel'][i:i + 4]))
        z.append('};')
    # Hoehe im Gehen (Clip 5, jedes 4. Bild, hoechster Punkt ueber dem Boden), am Spiel-Asset gemessen wie der Test
    spiel = FE.lese_emd(open(os.path.join(S.WURZEL, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_FORM.EMD'), 'rb').read())
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
