p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8/riegel_v8.py'
s = open(p, encoding='utf-8', newline='').read()
crlf = '\r\n' in s
s = s.replace('\r\n', '\n')
def rep(a, b):
    global s
    assert s.count(a) == 1, (s.count(a), a[:90])
    s = s.replace(a, b)
rep("""    rot = [k for k in ('S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8', 'S9', 'S10', 'S11', 'S12', 'S13', 'S14', 'S15', 'S16', 'S17', 'S18', 'S19', 'S20', 'S10s', 'S9s', 'S14s')
           if k in r and not r[k]['ok']]
    gelb = ['S16'] if r.get('S16', {}).get('status', '').startswith('GELB') else []""",
"""    if 'R7c' in r:                    # Runde 7c (riegel_r7c.py, Entscheide E9-E13): Urteile S16/S18/S3/S9 nach E9-E13, neu S21
        import riegel_r7c as R7C
        z += R7C.zeilen(r)
    rot = [k for k in ('S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8', 'S9', 'S10', 'S11', 'S12', 'S13', 'S14', 'S15', 'S16', 'S17', 'S18', 'S19', 'S20', 'S21',
                       'S10s', 'S9s', 'S14s')
           if k in r and not r[k]['ok']]
    gelb = [k for k in ('S3', 'S9', 'S16', 'S18', 'S21') if k in r and str(r[k].get('status', '')).startswith('GELB')]""")
rep("""                                             ('; GELB - Einzelbild (E3): ' + ', '.join(gelb)) if gelb else '',""",
"""                                             (('; GELB: ' if 'R7c' in r else '; GELB - Einzelbild (E3): ') + ', '.join(gelb)) if gelb else '',""")
rep("""    ap.add_argument('--ko', help='Marker-Lauf KO des Prueflings')""",
"""    ap.add_argument('--ko', help='Marker-Lauf KO des Prueflings')
    # Runde 7c (riegel_r7c.py, Entscheide E9-E13): Laeufe mit lauf_v4.sh, Marker LM = riegel_r7c.py lm (F225-F450 ausser --r7c-plain-r4)
    ap.add_argument('--r7c-lm', help='Runde 7c: Marker-Lauf LM des Prueflings'); ap.add_argument('--r7c-plain', help='Runde 7c: Szenenlauf des Prueflings')
    ap.add_argument('--r7c-lm-v7'); ap.add_argument('--r7c-plain-v7')
    ap.add_argument('--r7c-lm-ein', help='Runde 7c: LM-Lauf des Eingangs (Stand 7b)'); ap.add_argument('--r7c-plain-ein')
    ap.add_argument('--r7c-plain-r4', help='Runde 7c: Szenenlauf V8 r4 (helle Bildpunkte E12, F344-F378)')
    ap.add_argument('--r7c-ein', help='Runde 7c: Eingang (Standard git show db62602be = Stand 7b 87b02681)')
    ap.add_argument('--r7c', action='store_true', help='Runde 7c: Urteile nach E9-E13 (auch ohne Laeufe: dann nur offline)')""")
rep("""    z, rot = zeilen(r)""", """    if a.r7c or a.r7c_lm:
        import riegel_r7c as R7C
        R7C.einbauen(r, blob, ref, R6.r4_blob(a.r4), a)
    z, rot = zeilen(r)""")
if crlf: s = s.replace('\n', '\r\n')
open(p, 'w', encoding='utf-8', newline='').write(s)
print('ok', crlf)
