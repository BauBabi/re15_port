s = open('b_fein.py', encoding='utf-8').read()
s = s.replace("""                        for t_ in ('k17', 'k18', 'k19'):
                            ziehe(teile, s, c + (2.0, 2.0), (1.5, 3.2), sigma=45.0, radius=20.0, nur=[t_])
""", """                        for t_ in ('k18', 'k19'):
                            ziehe(teile, s, c + (1.0, 3.0), (0.5, 3.5), sigma=34.0, radius=22.0, nur=[t_])
""")
open('b_fein.py', 'w', encoding='utf-8').write(s)
