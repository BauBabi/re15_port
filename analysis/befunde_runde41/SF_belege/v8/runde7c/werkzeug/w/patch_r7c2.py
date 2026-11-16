p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8/riegel_r7c.py'
s = open(p, encoding='utf-8').read()


def rep(a, b):
    global s
    assert s.count(a) == 1, (s.count(a), a[:90])
    s = s.replace(a, b)


# (1) E13-Artefakt nur, wenn V7 dasselbe Bild im Soll hat (sonst ist es eine Kerbe, die V7 auch hat -> Klassenvergleich)
rep("""            if cos_szene['F%d' % f] >= VORN_COS:
                if mit_art is not None and f in mit_art and artefakt(klein(K[f]), klein(mit_art[f])):
                    art.append(x); continue""", """            if cos_szene['F%d' % f] >= VORN_COS:
                if mit_art is not None and f in mit_art:
                    k7 = klein(mit_art[f])
                    if im_soll(mass(k7)) and artefakt(klein(K[f]), k7):
                        art.append(x); continue""")
rep("""          (cos < 0,4) und die Foto-Posen PV/P1/P2; die Vorderansichten als KLASSE gegen V7 (Anzahl ueber Soll und deren Summe, offline und in
          echten Bild-Dumps, hoechstens wie V7, gemessen mit demselben Code im selben Lauf). Gedrehte Foto-Posen ohne Szenen-Gegenstueck
          (PV+k, P2+k) mit cos < 0,4 sind GELB (E13, mit Zahl). Ein Szenenbild, dessen Silhouette im Kopffenster bildpunktgleich zu V7 ist
          und nur ueber die Klassen der Randpunkte als Kerbe zaehlt, ist ein Mess-Artefakt (E13: genannt, nicht gewertet).""",
    """          (cos < 0,4) und die Foto-Posen PV/P1/P2; die Vorderansichten als KLASSE gegen V7 (Anzahl ueber Soll und deren Summe, offline und in
          echten Bild-Dumps, hoechstens wie V7, gemessen mit demselben Code im selben Lauf). Gedrehte Foto-Posen ohne Szenen-Gegenstueck
          (PV+k, P2+k) mit cos < 0,4 sind GELB (E13, mit Zahl). Ein Szenenbild, das V7 im Soll hat und dessen Silhouette um die Kerbe
          (Rahmen + 3 px) bildpunktgleich zu V7 ist - die Kerbe zaehlt nur ueber die Klassen der Randpunkte -, ist ein Mess-Artefakt (E13:
          genannt, nicht gewertet).""")
# (2) Verdeckt offline: Mesh 8 je Ansicht gegen den Eingang (gleiche Messung riegel_r6.s16_verdeckt): neu = ROT, gleich = Erbe
rep("""    v = out['S16'].get('verdeckt', {})
    r['verdeckt_off'] = dict(mesh8=int(v.get('knochen', {}).get(8, 0)), rest={k: c for k, c in v.get('knochen', {}).items() if k != 8},
                             ansichten=v.get('ueber', []))""", """    import riegel_r6 as R6
    v = out['S16'].get('verdeckt', {})
    ve = R6.s16_verdeckt(ein, ref_blob)
    je_e = {x[0]: x[2].get(8, 0) for x in ve['ueber']}
    neu8 = [(x[0], x[2].get(8, 0), je_e.get(x[0], 0)) for x in v.get('ueber', []) if x[2].get(8, 0) > je_e.get(x[0], 0)]
    r['verdeckt_off'] = dict(mesh8=int(v.get('knochen', {}).get(8, 0)), mesh8_ein=int(ve['knochen'].get(8, 0)), mesh8_neu=neu8,
                             rest={k: c for k, c in v.get('knochen', {}).items() if k != 8},
                             rest_ein={k: c for k, c in ve['knochen'].items() if k != 8}, ansichten=v.get('ueber', []))""")
rep("""    ok16 = e9o['ok'] and r['verdeckt_off']['mesh8'] == 0""", """    ok16 = e9o['ok'] and not r['verdeckt_off']['mesh8_neu']""")
rep("""    gelb16 = e9o['gelb'] or bool(r['verdeckt_off']['rest']) or""", """    gelb16 = e9o['gelb'] or bool(r['verdeckt_off']['rest']) or r['verdeckt_off']['mesh8'] > 0 or""")
rep("""    z.append('S16 (b)/(c) offline: V7-Gesicht verdeckt durch Mesh 8 %d (Soll 0, K-c), durch andere Meshes (Erbe E12) %s %s'
             % (vo['mesh8'], vo['rest'], vo['ansichten'][:8]))""", """    z.append('S16 (b)/(c) offline (riegel_r6.s16_verdeckt, je Bildpunkt lok x >= 150): V7-Gesicht verdeckt durch Mesh 8 %d (Eingang %d), mehr als '
             'der Eingang in %s (Soll keine, K-c); durch andere Meshes %s (Eingang %s, Erbe E12); Ansichten %s'
             % (vo['mesh8'], vo['mesh8_ein'], vo['mesh8_neu'], vo['rest'], vo['rest_ein'], vo['ansichten'][:8]))""")
# (3) S18: a1 immer mit und ohne die E10-Ecken; Urteil nach den Bedingungen
rep("""    k_vorn_ok = True
    s18 = s18_r7c(blob, ref_blob, r4, frei=())
    bedingungen""", """    s18 = s18_r7c(blob, ref_blob, r4, frei=E10_ECKEN)
    bedingungen""")
rep("""                       gesicht=r['verdeckt_off']['mesh8'] == 0 and (r.get('gesicht_echt') is None or r['gesicht_echt']['mesh8'] == 0),""",
    """                       gesicht=not r['verdeckt_off']['mesh8_neu'] and (r.get('gesicht_echt') is None or r['gesicht_echt']['mesh8'] == 0),""")
rep("""    frei = E10_ECKEN if all(bedingungen.values()) else ()
    if frei:
        s18 = s18_r7c(blob, ref_blob, r4, frei=frei)
    s18['e10_bedingungen'] = bedingungen
    s18['e10_frei'] = list(frei)""", """    frei = E10_ECKEN if all(bedingungen.values()) else ()
    s18['e10_bedingungen'] = bedingungen
    s18['e10_frei'] = list(frei)
    a1_urteil = s18['a1'] if frei else (s18['a1'] + s18['a1_frei'])""")
rep("""    ok18 = not s18['a1'] and not s18['a3'] and s18['kragen_px'] == 0 and s18.get('b_e12', {}).get('ok', False)""",
    """    ok18 = not a1_urteil and not s18['a3'] and s18['kragen_px'] == 0 and s18.get('b_e12', {}).get('ok', False)""")
open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('ok')
