s = open('b_fein.py', encoding='utf-8').read()
a = s.index("            if 'treffer' in x and n == 1:")
b = s.index("            if x['id'] == 'V6':")
neu = """            if 'treffer' in x and n == 1:
                z = cv2.imread(FORM + '/ziel_bild1.png', 0) == 0
                inn = EDT(z)
                sg = cv2.GaussianBlur(inn.astype(np.float32), (0, 0), 2.0)
                gx = cv2.Sobel(sg, cv2.CV_32F, 1, 0, ksize=3) / 8.0
                gy = cv2.Sobel(sg, cv2.CV_32F, 0, 1, ksize=3) / 8.0
                for px, x0, y0, x1, y1 in B['schwarz_treffer']:
                    if px < 5:
                        continue
                    c = np.array([(x0 + x1) / 2.0, (y0 + y1) / 2.0])
                    xi, yi = int(c[0]), int(c[1])
                    g = np.array([gx[yi, xi], gy[yi, xi]], float)
                    g /= max(np.linalg.norm(g), 1e-3)
                    tief = float(inn[y0:y1 + 1, x0:x1 + 1].max())
                    ziehe(teile, s, c, -g * (tief + 2.0), sigma=55.0, radius=16.0)
                    n_ += 1
                continue
"""
s = s[:a] + neu + s[b:]
open('b_fein.py', 'w', encoding='utf-8').write(s)
