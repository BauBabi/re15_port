s = open('b_fit.py', encoding='utf-8').read()
s = s.replace("""    if n == 1:
        Y_, X_ = np.mgrid[0:480, 0:640]
""", """    if n == 1:
        nah = cv2.dilate((z > 0).astype(np.uint8), np.ones((31, 31), np.uint8)) > 0
        w[(z == 0) & nah] = 3.0
        Y_, X_ = np.mgrid[0:480, 0:640]
""")
open('b_fit.py', 'w', encoding='utf-8').write(s)
s = open('b_fein.py', encoding='utf-8').read()
a = s.index("            if 'treffer' in x and n == 1:")
b = s.index("            if x['id'] == 'V6':")
s = s[:a] + s[b:]
open('b_fein.py', 'w', encoding='utf-8').write(s)
