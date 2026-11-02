s = open('b_snap.py', encoding='utf-8').read()
s = s.replace("""    def Z1(x, y):
        if offen1[y, x]:
            return None, None
        e = -s1[y, x]
        return e * gx1[y, x], e * gy1[y, x]
""", """    schwarz1 = z1 == 0
    s_b1 = sdf(schwarz1)
    gxb1, gyb1 = gradient(s_b1)

    def Z1(x, y):
        if schwarz1[y, x]:                  # Vorrang: sicher nicht Figur, hinaus bis ueber den grauen Saum
            e = -s_b1[y, x] + 2.0
            return e * gxb1[y, x], e * gyb1[y, x]
        if offen1[y, x]:
            return None, None
        e = -(s1[y, x] + 1.0)               # 1 px nach innen: der Pruefwert schliesst Stichloecher (geschlossen, 3 x 3), das Raster deckt angeschnittene Pixel
        return e * gx1[y, x], e * gy1[y, x]
""")
open('b_snap.py', 'w', encoding='utf-8').write(s)
