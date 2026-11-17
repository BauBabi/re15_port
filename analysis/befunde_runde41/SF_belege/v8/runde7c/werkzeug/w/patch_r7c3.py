p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8/riegel_r7c.py'
s = open(p, encoding='utf-8').read()


def rep(a, b):
    global s
    assert s.count(a) == 1, (s.count(a), a[:90])
    s = s.replace(a, b)


rep('''def artefakt(kk, k7):
    """E13: die Silhouette um die Kerben-Bildpunkte des Prueflings (Rahmen der Kerbe + 3 px) ist bildpunktgleich zu V7 - die Kerbe zaehlt nur
    ueber die Klassen der Randpunkte (P5-Pruefer R3 zu F340: "Spaltenprofil Kopf-Oberkante / Mesh-8-Unterkante identisch")."""
    G = _kerben(kk)[4]
    if not G.any():
        return False
    ys, xs = np.nonzero(G)
    sl = (slice(max(0, ys.min() - 3), ys.max() + 4), slice(max(0, xs.min() - 3), xs.max() + 4))
    return bool(((kk[sl] >= 0) == (k7[sl] >= 0)).all())''', '''ARTEFAKT_SPALTEN = 0.8                # E13 "spaltengleich": Anteil der Kerben-Spalten, deren Silhouetten-Kante ober- und unterhalb der Kerbe bei
                                      # V7 auf +- 1 px gleich liegt. Gemessen (sfv8/r7c NOTIZ): F340 12 von 14 Spalten (P5-Pruefer R3: Kopf-
                                      # Oberkante / Mesh-8-Unterkante identisch, Mess-Artefakt), F312 5 von 11 (echte Kerbe), F311 9 von 11


def _profil(k, G, x):
    S = k >= 0
    ys = np.nonzero(G[:, x])[0]
    oben = np.nonzero(S[:ys.min(), x])[0]
    unten = np.nonzero(S[ys.max() + 1:, x])[0]
    return (int(oben.max()) if oben.size else -1, int(unten.min() + ys.max() + 1) if unten.size else -1)


def artefakt(kk, k7):
    """E13: die Kerbe des Prueflings ist bei V7 von derselben Silhouette begrenzt (Spaltenprofil: letzte Silhouetten-Zeile ueber und erste unter
    der Kerbe je Spalte, +- 1 px, in >= ARTEFAKT_SPALTEN der Spalten) - sie zaehlt nur ueber die Klassen der Randpunkte (P5-Pruefer R3 zu F340:
    "Spaltenprofil Kopf-Oberkante / Mesh-8-Unterkante identisch"). Aufrufer prueft zusaetzlich: V7 hat das Bild im Soll."""
    G = _kerben(kk)[4]
    if not G.any():
        return False
    xs = np.unique(np.nonzero(G)[1])
    gleich = 0
    for x in xs:
        a, b = _profil(kk, G, x), _profil(k7, G, x)
        gleich += abs(a[0] - b[0]) <= 1 and abs(a[1] - b[1]) <= 1
    return gleich >= ARTEFAKT_SPALTEN * len(xs)''')
rep("""          (PV+k, P2+k) mit cos < 0,4 sind GELB (E13, mit Zahl). Ein Szenenbild, das V7 im Soll hat und dessen Silhouette um die Kerbe
          (Rahmen + 3 px) bildpunktgleich zu V7 ist - die Kerbe zaehlt nur ueber die Klassen der Randpunkte -, ist ein Mess-Artefakt (E13:
          genannt, nicht gewertet).""", """          (PV+k, P2+k) mit cos < 0,4 sind GELB (E13, mit Zahl). Ein Szenenbild, das V7 im Soll hat und dessen Kerbe bei V7 von derselben
          Silhouette begrenzt ist (Spaltenprofil, artefakt()) - die Kerbe zaehlt nur ueber die Klassen der Randpunkte -, ist ein Mess-Artefakt
          (E13: genannt, nicht gewertet).""")
open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('ok')
