s = open('b_fein.py', encoding='utf-8').read()
s = s.replace("""                zone = P.poly_maske(P.zone_b1(x['id']))
""", """                zone = P.poly_maske(P.zone_b1(x['id']))
                if x['id'] == 'L2' and x['treffer'] <= 30:
                    ziehe(teile, s, (313.0, 353.0), (0.5, 3.0), sigma=36.0, radius=10.0, nur=['k16', 'k17', 'k18', 'k19'])
                    n_ += 1
                    continue
""")
open('b_fein.py', 'w', encoding='utf-8').write(s)
