p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8/p5_stufe.py'
s = open(p, encoding='utf-8').read()


def rep(a, b):
    global s
    assert s.count(a) == 1, (s.count(a), a[:90])
    s = s.replace(a, b)


rep("""    # K-b: t8 [60,17,4] R3 (29,38) = 0,47-fach (Pruefer flaechen.py) / 0,49 (Bank S19) -> (25,36): 0,54-fach, Streckung 1,48 an der
    #      Endgeometrie (Ecke 17 nach 7b; r7c/pz/t8such.py: Dichte >= 0,5 (Auftrag E4) mit Rand, nur Texel, die R6/R4/V7/R2/Stand 7b auf
    #      Zeile 493 schon benutzen; (26,37) ergab dort nur 0,513).
    't8': [(1, (29, 38), (25, 36))],
}""", """    # K-b: t8 [60,17,4] R3 (29,38) = 0,47-fach (Pruefer flaechen.py) / 0,49 (Bank S19) -> (23,41): 0,53-fach, Streckung 1,80 an der
    #      Endgeometrie (r7c/pz/t8such.py / t8such2.py: Dichte >= 0,5 (Auftrag E4), nur Texel, die R6/R4/V7/R2/Stand 7b auf Zeile 493 schon
    #      benutzen). Erste Wahl (25,36) gab 112 fast weisse Bildpunkte von vorn (riegel_v8 S14 b, r7c/w/weiss.py; R3 (29,38): 1);
    #      (23,41): 0 (die Nachbarn (24..28, 35..38) 58-140).
    't8': [(1, (29, 38), (23, 41))],
    # K-a / Riegel S7 (a): q2 [16,46,23,0] las seit der UV-Mitfuehrung der Ecke 46 (P5 R2/R3) mit (42,0) 3 Texel auf Zeile 494, die weder
    #      eine Original-EM30-Flaeche (S7 a "texel_ohne_original_flaeche" 3 -> ROT) noch Eingang/r4/V7 benutzen. Ecke 46 zurueck auf r6 (41,0)
    #      allein gaebe Streckung 2,13 (> 2, E4); dazu Ecke 16 (37,2) -> (38,4): S7 (a) 0, fremde Texel 0, Streckung q2 1,61 / 1,04,
    #      Dichte 1,10 / 1,21 (r7c/w/q2such.py, fremd.py; (38,3) 1,90, (37,3)/(37,4)/(36,x) mit fremden Texeln).
    'q2': [(1, (42, 0), (41, 0)), (0, (37, 2), (38, 4))],
}""")
open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('ok')
