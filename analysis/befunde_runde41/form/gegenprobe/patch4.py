#!/usr/bin/env python3
"""Vierter Umbau: nur der rechte Oberarm bleibt verdeckt, linke Schulter gehoert zum Oberkoerper; Dicken-Untergrenzen Arm; Versaetze und Dicken setzen."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'rig.py')
t = open(p, encoding='utf-8').read()
t = ers(t, "'oberarm': (230, 520, 230, 520), 'unterarm': (200, 420, 200, 420),", "'oberarm': (280, 520, 280, 520), 'unterarm': (240, 420, 240, 420),")
t = ers(t, "# Gegenprobe: In Bild 2 gehoeren die Oberarme zu keinem Teil. Beide Arme kommen unter der Rumpfkante hervor, die Oberarme muessen verdeckt bleiben.",
        "# Gegenprobe: In Bild 2 gehoert der rechte Oberarm zu keinem Teil, er bleibt unter der Rumpfkante. Der linke liegt hinter dem Kopfrand und bildet die rechte Ecke des Oberkoerpers.")
t = ers(t, "2: {'face_and_upper_body': ['huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer'],", "2: {'face_and_upper_body': ['huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer', 'oberarm_l'],")
open(p, 'w', encoding='utf-8', newline='\n').write(t)

p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, "            ob = np.nonzero(np.isin(kn, [RG.IDX['oberarm_l'], RG.IDX['oberarm_r']]))[0]    # Gegenprobe: Oberarme bleiben unter der Rumpfkante\n",
        "            ob = np.nonzero(kn == RG.IDX['oberarm_r'])[0]    # Gegenprobe: rechter Oberarm bleibt unter der Rumpfkante\n")
t = ers(t, """    a.add_argument('--setze', default='', help='name=wert,...: Laengen setzen (Startwert)')
""", """    a.add_argument('--setze', default='', help='name=wert,...: Laengen und Versaetze setzen (Startwert)')
    a.add_argument('--setzed', default='', help='klasse=a0/a1/a2[:b0/b1/b2];...: Dicken setzen (Startwert)')
""")
t = ers(t, """                k, w = q.split('=')
                par.l.data[:, RG.L_KEYS.index(k)] = math.log(float(w) / RG.LAENGE0[k])
""", """                k, w = q.split('=')
                if k in RG.L_KEYS:
                    par.l.data[:, RG.L_KEYS.index(k)] = math.log(float(w) / RG.LAENGE0[k])
                else:
                    j = RG.V_KEYS.index(k)
                    par.v.data[:, j] = (float(w) - float(par.V0[j])) / float(par.Vska[j])
""")
t = ers(t, """    if a.anker3d:
        kam = grob[1].kam
""", """    if a.setzed:
        with torch.no_grad():
            for q in a.setzed.split(';'):
                k, w = q.split('=')
                j = RG.D_KEYS.index(k)
                t3 = w.split(':')
                va = torch.tensor([float(x) for x in t3[0].split('/')], dtype=DT, device=GER)
                par.da.data[:, j] = torch.log(va / par.A0[j])
                if len(t3) > 1:
                    vb = torch.tensor([float(x) for x in t3[1].split('/')], dtype=DT, device=GER)
                    par.db.data[:, j] = torch.log(vb / par.B0[j])
    if a.anker3d:
        kam = grob[1].kam
""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
