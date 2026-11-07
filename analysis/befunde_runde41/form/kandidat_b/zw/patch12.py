s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("""    for it in range(8):
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.5, sigma=70.0)
    print('Snap B1 fein: %d Ecken, Mittel %.1f, max %.1f px' % b[1], flush=True)
""", """    for it in range(8):
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.5, sigma=70.0)
    print('Snap B1 fein: %d Ecken, Mittel %.1f, max %.1f px' % b[1], flush=True)
    for it in range(8):
        s1, s2, _ = sichten()
        c = SN.schritt([s2], kuppel, ziele, rate=0.6, sigma=120.0)
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.45, sigma=70.0)
    print('Snap wechselnd: B1 %d, %.1f, %.1f  B2 %d, %.1f, %.1f' % (*b[1], *c.get(2, (0, 0, 0))), flush=True)
""")
open('b_alles.py', 'w', encoding='utf-8').write(s)
