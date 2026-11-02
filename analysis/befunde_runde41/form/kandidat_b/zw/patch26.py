s = open('b_fein.py', encoding='utf-8').read()
a = s.index("                if x['id'] == 'L2' and x['treffer'] <= 30:")
b = s.index("                for px, x0, y0, x1, y1 in B['schwarz_treffer']:")
s = s[:a] + s[b:]
open('b_fein.py', 'w', encoding='utf-8').write(s)
