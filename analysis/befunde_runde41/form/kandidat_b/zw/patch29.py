s = open('b_fein.py', encoding='utf-8').read()
s = s.replace("""                for px, x0, y0, x1, y1 in B['schwarz_treffer']:
                    if not zone[y0:y1 + 1, x0:x1 + 1].any() or px > 60:
                        continue
""", """                ka = P.Kandidat(os.path.join(aus, 'kandidat.json'))
                _s, _a, m_, _k = ka.rendern(1)
                hit = m_ & zone & z
                kk, lab, st_, _c = cv2.connectedComponentsWithStats(hit.astype(np.uint8), connectivity=8)
                liste = [[int(st_[i, 4]), int(st_[i, 0]), int(st_[i, 1]), int(st_[i, 0] + st_[i, 2] - 1), int(st_[i, 1] + st_[i, 3] - 1)] for i in range(1, kk)]
                for px, x0, y0, x1, y1 in liste:
""")
open('b_fein.py', 'w', encoding='utf-8').write(s)
s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("    return (z, round(R['bild1']['ncc_weiss'] + R['bild2']['ncc_weiss'], 3))\n", "    return (z, -R['bild1']['umriss']['schwarz_getroffen'], round(R['bild1']['ncc_weiss'] + R['bild2']['ncc_weiss'], 3))\n")
s = s.replace("def durchgang(fit_pfad, tag, ab='mesh', schritte=14, malen=None, runden=6, wahl=None):", "def durchgang(fit_pfad, tag, ab='mesh', schritte=14, malen=None, runden=8, wahl=None):")
open('b_alles.py', 'w', encoding='utf-8').write(s)
