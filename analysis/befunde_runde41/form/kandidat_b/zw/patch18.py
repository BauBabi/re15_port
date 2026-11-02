s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("""    teile = SN.stuecke(z + '/quad')
    for st in teile:
        st['V'] = np.array(V[st['name']], float)
    form = None
""", """    teile = SN.stuecke(z + '/quad')
    for st in teile:
        st['V'] = np.array(V[st['name']], float)
    form = None
    import b_ruhe
    ruhe_w = b_ruhe.ruhe_winkel(fit)
""")
s = s.replace("snap=V, leise=r_ < runden - 1)", "snap=V, leise=r_ < runden - 1, ruhe_w=ruhe_w)")
open('b_alles.py', 'w', encoding='utf-8').write(s)
