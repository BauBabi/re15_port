s = open('b_fein.py', encoding='utf-8').read()
s = s.replace("""                    ziehe(teile, s, c, g * -1.0 * (tief + 1.5) if False else -g * (tief + 1.5), sigma=45.0, radius=10.0)
""", """                    if 303 <= c[0] <= 318 and 345 <= c[1] <= 356:
                        # Spitze von L2 sitzt in der Taille: Oberkante C3 absenken, die Kante bei y=346 bleibt stehen
                        ziehe(teile, s, c + (2.0, 3.0), (0.6, 3.5), sigma=38.0, radius=9.0, nur=['k17', 'k18', 'k19', 'k16'])
                    else:
                        ziehe(teile, s, c, -g * (tief + 1.5), sigma=45.0, radius=10.0)
""")
open('b_fein.py', 'w', encoding='utf-8').write(s)
