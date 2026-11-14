import os
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'schwanz_r8.py')
s = open(p, encoding='utf-8').read()


def ers(alt, neu):
    global s
    assert s.count(alt) == 1, alt[:60]
    s = s.replace(alt, neu)


# 'spiegel' wieder heraus (Messbild mb_c1s: regelmaessiges Augen-/Rautenmuster)
ers("""                if feld_f.get('abbildung') == 'spiegel':
                    # Runde 8c: quer an der Bauchmitte gespiegelt (Ecke 0 liegt auf theta 0), laengs je Ring-Abschnitt gespiegelt (Ringe abwechselnd
                    # Feldanfang / -ende): feinere Texel fuer ein kleines, geflecktes Feld (sonst 6-px-Bloecke laengs im Spielbild, Messbild mb_ring_nah)
                    sq = float(np.clip(abs(ts) / tb, 0.0, 1.0))
                    sl = float(vs)
                else:
                    sq = float(np.clip((ts + tb) / (2 * tb), 0.0, 1.0))
                    sl = s_lang if g % 2 == 0 else 1.0 - s_lang
""", """                sq = float(np.clip((ts + tb) / (2 * tb), 0.0, 1.0))
                sl = s_lang if g % 2 == 0 else 1.0 - s_lang
""")
ers("""                    # Runde 8c: Ring - quer gespiegelt |theta| / 180 (Bauch 0, Ruecken 1), laengs t 0 am Gelenk (helles Ende des Felds) -> 1 am vollen
                    # Querschnitt (dunkles Ende); Richtung des hellen Endes "richt" (v- = hell bei v1)
                    ts = th if th <= 180.0 else th - 360.0
                    fr = felder['ring']
                    t_ = 1.0 - am_gelenk
                    vv = fr['v1'] - t_ * (fr['v1'] - fr['v0']) if fr['richt'] == 'v-' else fr['v0'] + t_ * (fr['v1'] - fr['v0'])
                    uv_e.append((fr['u0'] + abs(ts) / 180.0 * (fr['u1'] - fr['u0']), vv))""",
    """                    # Runde 8c: Ring - quer gespiegelt |theta| / 180 (Bauch 0, Ruecken 1), laengs t 0 am Gelenk (helles Ende des Felds) -> 1 am vollen
                    # Querschnitt (dunkles Ende). "richt" = Achse und Seite des hellen Endes im Feld: v+ hell bei v0, v- bei v1, u+ bei u0, u- bei u1
                    ts = th if th <= 180.0 else th - 360.0
                    fr = felder['ring']
                    t_ = 1.0 - am_gelenk
                    q_ = abs(ts) / 180.0
                    if fr['richt'][0] == 'v':
                        vv = fr['v1'] - t_ * (fr['v1'] - fr['v0']) if fr['richt'] == 'v-' else fr['v0'] + t_ * (fr['v1'] - fr['v0'])
                        uv_e.append((fr['u0'] + q_ * (fr['u1'] - fr['u0']), vv))
                    else:
                        uu = fr['u1'] - t_ * (fr['u1'] - fr['u0']) if fr['richt'] == 'u-' else fr['u0'] + t_ * (fr['u1'] - fr['u0'])
                        uv_e.append((uu, fr['v0'] + q_ * (fr['v1'] - fr['v0'])))""")
open(p, 'w', encoding='utf-8').write(s)
print('ok')
