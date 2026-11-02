s = open('b_mal2.py', encoding='utf-8').read()
s = s.replace("""def faktor_nachziehen(aus, pfad='zw/faktor.json', schritt=0.22):""", """FEST_TON = {'Z7': 0.55, 'Z9': 0.0}        # Stellen am Umriss: Y im Polygon misst dort den Boden mit, kein Nachziehen


def faktor_nachziehen(aus, pfad='zw/faktor.json', schritt=0.22):""")
s = s.replace("""            q = z['y_video'] / max(z['y_render'], 1.0)
            if q > 1.12:""", """            q = z['y_video'] / max(z['y_render'], 1.0)
            if z['id'] in FEST_TON:
                FAKTOR[z['id']] = FEST_TON[z['id']]
            elif q > 1.12:""")
s = s.replace("""        elif mat == 'weisskerne':
            rgb[tr] = stufe_waehlen(w[tr], WEISS)
""", """        elif mat == 'weisskerne':
            rgb[tr] = stufe_waehlen(w[tr], WEISS if s['id'] == 'Z3' else WEISS_P)
""")
s = s.replace("STREIFEN = np.array(", "WEISS_P = np.array([[206, 216, 222], [240, 248, 248]], float)\nSTREIFEN = np.array(")
s = s.replace("""def innen_maske(erode=5):""", """def nahschatten(rgb, pos, bone, belegt, netze):
    \"\"\"Gemalte Verschattung: Haut nah an einem anderen Teil dunkler (Achsel, Schritt, Fugen). Rein Textur, die Engine rechnet nur diffus.\"\"\"
    from scipy.spatial import cKDTree
    gruppe = np.full(32, -1)
    gr = {'rumpf': 0, 'rumpf_m': 0, 'rumpf_f': 0, 'maul_l': 0, 'maul_r': 0, 'fr_ober': 1, 'fr_unter': 2, 'fr_fuss': 3, 'fr_zeh': 3, 'fl_ober': 4, 'fl_unter': 5, 'fl_fuss': 6, 'fl_zeh': 6,
          'nl_ober': 7, 'nl_unter': 8, 'nl_faust': 9, 'nl_innen': 10, 'nr_ober': 11, 'nr_unter': 12, 'nr_faust': 13, 'nr_innen': 14}
    for k, v in gr.items():
        gruppe[T.IDX[k]] = v
    ecken = {}
    for nz in netze:
        g = gruppe[nz['bone']]
        idx = sorted({a for e in nz['f'] for a, _ in e})
        ecken.setdefault(g, []).append(nz['v'][idx])
    bg = np.where(bone >= 0, gruppe[np.maximum(bone, 0)], -1)
    for g in sorted(ecken):
        m = bg == g
        if not m.any():
            continue
        andere = np.vstack([np.vstack(v) for k, v in ecken.items() if k != g])
        d, _ = cKDTree(andere).query(pos[m])
        f = np.where(d < 95, 0.5, np.where(d < 170, 0.72, 1.0))
        z = rgb[m]
        z *= f[:, None]
        rgb[m] = z
    return rgb


def innen_maske(erode=5):""")
s = s.replace("""    F1 = lichtfaktor(1, s1, pos, nrm, bone)
    rgb = strich_aus_bild""", """    rgb = nahschatten(rgb, pos, bone, belegt, netze)
    F1 = lichtfaktor(1, s1, pos, nrm, bone)
    rgb = strich_aus_bild""")
open('b_mal2.py', 'w', encoding='utf-8').write(s)
