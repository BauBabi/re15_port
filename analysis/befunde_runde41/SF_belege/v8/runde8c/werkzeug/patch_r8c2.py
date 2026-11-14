import os
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'schwanz_r8.py')
s = open(p, encoding='utf-8').read()
alt = """                ts = th if th <= 180.0 else th - 360.0
                sq = float(np.clip((ts + tb) / (2 * tb), 0.0, 1.0))
                sl = s_lang if g % 2 == 0 else 1.0 - s_lang
                uv_e.append(fleisch_uv(feld_f, sq, sl))"""
neu = """                ts = th if th <= 180.0 else th - 360.0
                if feld_f.get('abbildung') == 'spiegel':
                    # Runde 8c: quer an der Bauchmitte gespiegelt (Ecke 0 liegt auf theta 0), laengs je Ring-Abschnitt gespiegelt (Ringe abwechselnd
                    # Feldanfang / -ende): feinere Texel fuer ein kleines, geflecktes Feld (sonst 6-px-Bloecke laengs im Spielbild, Messbild mb_ring_nah)
                    sq = float(np.clip(abs(ts) / tb, 0.0, 1.0))
                    sl = float(vs)
                else:
                    sq = float(np.clip((ts + tb) / (2 * tb), 0.0, 1.0))
                    sl = s_lang if g % 2 == 0 else 1.0 - s_lang
                uv_e.append(fleisch_uv(feld_f, sq, sl))"""
assert s.count(alt) == 1
s = s.replace(alt, neu)
open(p, 'w', encoding='utf-8').write(s)
print('ok')
