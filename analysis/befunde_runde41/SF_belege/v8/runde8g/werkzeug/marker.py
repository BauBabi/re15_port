"""Messkopien (nie Spiel-Asset) fuer die Zuordnung im Spielbild.

    python marker.py klasse <ein.emd> <aus.emd>   jede Schwanz-Flaeche einfarbig nach ihrer Lage (aus der Geometrie, gleich fuer 8c/8d/8f/8g):
        Bn Bauch ohne Fuge = rot, Bf Bauch-Fuge = magenta, Fn Flanke/Ruecken ohne Fuge = gruen, Ff Flanke/Ruecken-Fuge = cyan,
        N Nadel = blau, W Wurzel im Rumpf = gelb
    python marker.py glied <ein.emd> <aus.emd>    jede Schwanz-Flaeche einfarbig nach ihrem Glied 16..22 (rot, gruen, blau, gelb, magenta, cyan, weiss)
    python marker.py naht <ein.emd> <aus.emd>     Texel Seite 0 u 54-55 v 46-60 (die Naht des Original-Bilds) gruen, sonst unveraendert (wie Runde 8e mark.py)

Verfahren: Palettenindex 0 benutzt kein Texel des Bilds (gemessen), Texel Seite 0 (0,0) benutzt keine Flaeche (gemessen). klasse/glied: Texel (0,0) auf
Index 0, Index 0 der CLUT-Zeile 480 + k = Farbe k, jede Flaeche der Klasse k auf Zeile 480 + k, Seite 0, alle UV-Ecken (0,0). Die Ecken, Flaechen und
Keyframes bleiben, also auch die Bildpunkte jeder Flaeche; nur die Schwanz-Farben aendern sich.
"""
import sys
import numpy as np
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0] = [T + '/v8', T + '/v6kopf', T + '/v4']
import kopf_orig_bau as KB          # noqa: E402
import riegel_schwanz_r8 as RS      # noqa: E402
import schwanz_r8 as Q              # noqa: E402

FARBEN6 = [0x001F, 0x7C1F, 0x03E0, 0x7FE0, 0x7C00, 0x03FF]          # Bn, Bf, Fn, Ff, N, W  (PSX 15 Bit: R = Bit 0-4, G 5-9, B 10-14)
FARBEN7 = [0x001F, 0x03E0, 0x7C00, 0x03FF, 0x7C1F, 0x7FE0, 0x7FFF]  # Glied 16..22


def klassen_flaechen(blob):
    _d, _o, ms, _t = KB.teile(blob)
    alt16 = RS._alt16(RS.eingang())
    out = {}
    for b in range(16, 23):
        m = ms[b]
        bm = RS.bauch_flaechen(b, m, alt16)
        fm = RS.fugen_flaechen(b, m)
        for key, F in (('d', m['tf']), ('v', m['qf'])):
            for i, f in enumerate(F):
                k = '%s%d' % (key, i)
                e = list(f[1::2])
                if b == 16 and tuple(e) in alt16:
                    c = 5
                elif b == 22 and all(m['v'][q][1] >= Q.ENDE_Y - 1.0 for q in e):
                    c = 4
                elif k in bm:
                    c = 1 if k in fm else 0
                else:
                    c = 3 if k in fm else 2
                out[(b, k)] = c
    return out


def main():
    art, src, dst = sys.argv[1:4]
    blob = open(src, 'rb').read()
    d, o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    assert int((t['pix'] == 0).sum()) == 0, 'Index 0 wird benutzt'
    if art == 'naht':
        t['pix'][46:61, 54:56] = 0
        t['clut'][:, 0] = 0x03E0
    else:
        use = np.zeros((256, 384), bool)
        for m in ms:
            for uv in m['tuv'] + m['quv']:
                use |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
        assert not use[0, 0], 'Texel (0,0) wird benutzt'
        t['pix'][0, 0] = 0
        kl = klassen_flaechen(blob) if art == 'klasse' else None
        farben = FARBEN6 if art == 'klasse' else FARBEN7
        for k, c in enumerate(farben):
            t['clut'][k, 0] = c
        zahl = [0] * len(farben)
        for b in range(16, 23):
            for key, U in (('d', 'tuv'), ('v', 'quv')):
                for i, uv in enumerate(ms[b][U]):
                    c = kl[(b, '%s%d' % (key, i))] if kl else b - 16
                    zahl[c] += 1
                    x = list(uv)
                    for j in range(0, len(x), 3):
                        x[j], x[j + 1] = 0, 0
                    x[2] = (480 + c) << 6
                    x[5] = 128 + 0
                    ms[b][U][i] = x
        print('Flaechen je Klasse', zahl)
    neu = KB.tim_packen(t)
    assert len(neu) == len(tim)
    b2 = bytearray(KB.packen(blob, d, ms, tim))
    d2, _o2, _m2, tim2 = KB.teile(bytes(b2))
    b2[d2[8]:d2[8] + len(neu)] = neu
    open(dst, 'wb').write(bytes(b2))
    print('ok', dst)


if __name__ == '__main__':
    main()
