"""Patch riegel_v8.py / riegel_r6.py fuer Runde 8 (S4, S6, S15)."""
import os
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'riegel_v8.py')
s = open(p, encoding='utf-8').read()
rep = [
("""Szene (Optionen --mk-a/--mk-b/--gr/--gr-v7, echte Framedumps mit Marker-Assets, szene_marker.py): S10s Wurzel F328-F346 = 0; S9s Schlitz""",
 """Runde 8 (Schwanz, Stufe q schwanz_r8.py; Nutzer 2026-11-14 "weder mit dem rot der Schwanzfarbe noch mit dem etwas zu steifen grossen Schwanz
                zufrieden" + Version_80%/Areas*.png; eigene Riegel riegel_schwanz_r8.py R8a-R8f). Fuer ein Asset mit dem Schwanz der Runde 8
                (Mesh 22 mit Nadel, groesste Ecke y > 1000; riegel_schwanz_r8.ist_r8) gelten drei Regeln neu, die alten Werte stehen als Angabe dabei:
S4 Runde 8      Wurzel >= 55, monoton, Ende <= 10, s 170-200 >= 9 wie bisher; "s 90-130 >= 36" und "(b) s 140-160 >= 27" (Messblatt V8 an der
                Vorlage n1439, Linie T) ersetzt durch R8b (dasselbe Bild n1439 = h90 mit dem Nutzer-Umriss Areas.png vom 14.11.: PV s .3 >= 36,
                s .5 15-20 % duenner als 07b6703f, s .5/.7/.9 gegen den Umriss +- 4) - Auftrag Runde 8 "Schwanzmitte (s .5) 15-20 % duenner".
S6 Runde 8      (a) Keyframes: Spitze (Gelenk 7) und das Knollen-Ende (groesste Ecke von Mesh 22 bis y 600 = Ort des alten Dorns y 589) unter der
                tiefsten Koerperecke in 0 Keyframes; die Nadel (bis y 1139) offline nur Angabe - im Spiel hebt die Bodenregel des Codes die Kette
                mit der Nadel als Dornspitze (schwanz_r41.c sw_mesh_mass / glaetten); (c) Szene (--szene): alle Gelenke UND die Nadelspitze
                (Gelenk 6 + (Gelenk 7 - Gelenk 6) x 1139 / 541) y <= 0, Gelenkwinkel <= 25,5.
S15 Runde 8     ersetzt durch R8e (Fleisch nur auf der Bauchseite, Grenze 90 -> 60 Grad, Herkunft; Nutzer "weder mit dem rot der Schwanzfarbe ...");
                die Regeln der Runden 5/6 (Unterseite bis theta 110/120, Ring-Deckel, Reifen) gelten fuer diesen Schwanz nicht mehr.
Szene (Optionen --mk-a/--mk-b/--gr/--gr-v7, echte Framedumps mit Marker-Assets, szene_marker.py): S10s Wurzel F328-F346 = 0; S9s Schlitz"""),
("""    m22 = mo.meshes[22]
    P22 = np.array([v[:3] for v in m22.tv], float)
    dorn = P22[np.argmax(P22[:, 1])]
    L = float""", """    m22 = mo.meshes[22]
    P22 = np.array([v[:3] for v in m22.tv], float)
    dorn = P22[np.argmax(P22[:, 1])]
    r8 = bool(dorn[1] > 1000)                                     # Runde 8: Nadel (riegel_schwanz_r8.ist_r8)
    nadel = dorn.copy()
    if r8:
        K = P22[P22[:, 1] <= 600.0]
        dorn = K[np.argmax(K[:, 1])]                              # Knollen-Ende (Ort des alten Dorns y 589)
    L = float"""),
("""    unter = []
    for k in range(s.kf_count):""", """    unter, unter_nadel = [], []
    for k in range(s.kf_count):"""),
("""        dn = T[22].astype(float) + R22 @ dorn
        if max(sp[1], dn[1]) > y:
            unter.append(k)""", """        dn = T[22].astype(float) + R22 @ dorn
        if max(sp[1], dn[1]) > y:
            unter.append(k)
        if r8 and float((T[22].astype(float) + R22 @ nadel)[1]) > y:
            unter_nadel.append(k)"""),
("""        wmax = 0.0
        for z in open(szene, errors='replace'):""", """        wmax, ntief = 0.0, -1e9
        for z in open(szene, errors='replace'):"""),
("""                    wmax = max(wmax, max(float(np.degrees(np.arccos(np.clip((dd[j] * dd[j + 1]).sum(), -1, 1)))) for j in range(6)))
        sz = dict(takte=n, tiefstes_y=tief, unter_boden=int(tief > 0), gelenk_max_grad=round(wmax, 1))
    ok = not unter and (sz is None or (sz['tiefstes_y'] <= 0 and sz['gelenk_max_grad'] <= 25.5))
    return dict(keyframes=s.kf_count, unter=len(unter), beispiele=unter[:10], szene=sz, ok=ok)""",
 """                    wmax = max(wmax, max(float(np.degrees(np.arccos(np.clip((dd[j] * dd[j + 1]).sum(), -1, 1)))) for j in range(6)))
                    if r8:                                        # Runde 8: Nadelspitze = Gelenk 6 + (Gelenk 7 - Gelenk 6) x y_nadel / 541
                        ntief = max(ntief, float((pts[6] + (pts[7] - pts[6]) * float(nadel[1]) / 541.0)[1]))
        sz = dict(takte=n, tiefstes_y=tief, unter_boden=int(tief > 0), gelenk_max_grad=round(wmax, 1))
        if r8:
            sz['nadel_tiefstes_y'] = round(ntief, 1)
    ok = not unter and (sz is None or (sz['tiefstes_y'] <= 0 and sz['gelenk_max_grad'] <= 25.5 and sz.get('nadel_tiefstes_y', -1) <= 0))
    return dict(keyframes=s.kf_count, unter=len(unter), beispiele=unter[:10], szene=sz, ok=ok, r8=r8,
                nadel_unter_info=len(unter_nadel) if r8 else None, nadel_y=int(nadel[1]), knolle_y=int(dorn[1]))"""),
("""    out['S15'] = s15(blob, ref_blob)
    out['S16'] = s16(blob, ref_blob)""", """    import riegel_schwanz_r8 as RS8
    if RS8.ist_r8(blob):              # Runde 8: S4 mit R8b, S15 = R8e (Kopf dieser Datei)
        r8 = RS8.alles(blob, RS8.Q.STANDARD if farbe is None else farbe)
        out['R8'] = r8
        out['S4']['r8_alt_ok'] = out['S4']['ok']
        out['S4']['ok'] = bool(out['S4']['wurzel'] >= 55 and out['S4']['monoton'] and out['S4']['ende'] <= 10
                               and bool(out['S4']['unten']['s170_200']) and min(out['S4']['unten']['s170_200']) >= 9 and r8['R8b']['ok'])
        out['S15'] = dict(r8=True, ok=bool(r8['R8e']['ok']), r8e=r8['R8e'])
    else:
        out['S15'] = s15(blob, ref_blob)
    out['S16'] = s16(blob, ref_blob)"""),
("""def alles(blob, ref_blob, szene=None, mit_s7=True, marker=None, fd=None, r4=None, ko=None):""",
 """def alles(blob, ref_blob, szene=None, mit_s7=True, marker=None, fd=None, r4=None, ko=None, farbe=None):"""),
("""    s = r['S5']; z.append('S5 Gesicht:""", """    if 'R8' in r:
        s = r['S4']; b8 = r['R8']['R8b']
        z.append('S4 Runde 8 (Auftrag Runde 8 / Nutzer-Umriss Areas.png): Wurzel %.1f (>= 55), monoton %s, Ende %.1f (<= 10), s 170-200 %s (>= 9); '
                 'R8b PV s .1-.9 %s, s .3 %.1f (>= 36), s .5 %.1f %% duenner als 07b6703f (15-20), Abw. s .5/.7/.9 gegen den Umriss %s (<= 4); '
                 'alte Regel (s 90-130 >= 36 / s 140-160 >= 27) waere %s -> %s'
                 % (s['wurzel'], s['monoton'], s['ende'], s['unten']['s170_200'], b8['breite'], b8['breite'][1], b8['duenner_s5'],
                    b8['abweichung_s5_s9'], 'GRUEN' if s['r8_alt_ok'] else 'ROT', 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S5']; z.append('S5 Gesicht:"""),
("""                          % (s['unter'], s['keyframes'], s['beispiele'], s['szene'], 'GRUEN' if s['ok'] else 'ROT'))""",
 """                          % (s['unter'], s['keyframes'], s['beispiele'], s['szene'], 'GRUEN' if s['ok'] else 'ROT'))
    if s.get('r8'):
        z.append('   S6 Runde 8: Keyframes mit Spitze + Knollen-Ende (Mesh 22 y %d) gezaehlt; Nadel (y %d) offline unter der Sohle in %d von %d Keyframes '
                 '(Angabe - im Spiel hebt die Bodenregel des Codes die Kette, Dorn = Nadel); Szene: Nadelspitze tiefstes y %s (Soll <= 0)'
                 % (s['knolle_y'], s['nadel_y'], s['nadel_unter_info'], s['keyframes'], (s['szene'] or {}).get('nadel_tiefstes_y', 'ohne --szene')))"""),
("""    if 'S15' in r:
        s = r['S15']; a, c, d = s['flaechen'], s['bild']['ansichten'], s['ruhe']['ansichten']""",
 """    if 'S15' in r and r['S15'].get('r8'):
        e = r['S15']['r8e']
        z.append('S15 Runde 8 = R8e (Fleisch nur Bauchseite): Flaechen auf 495 %d, ausserhalb des Bauchsektors %d; PV Fleisch bei |theta| >= 100 %.2f %%, '
                 '<= 40 %.1f %%; fremde Texel %s -> %s (Regeln der Runden 5/6 fuer diesen Schwanz ersetzt)'
                 % (e['flaechen_495'], e['n_falsch'], e['pv_fleisch_aussen'], e['pv_fleisch_innen'], e['fremde_texel'], 'GRUEN' if r['S15']['ok'] else 'ROT'))
    if 'R8' in r:
        import riegel_schwanz_r8 as RS8
        z += ['   ' + x for x in RS8.zeilen(r['R8'])[0]]
    if 'S15' in r and not r['S15'].get('r8'):
        s = r['S15']; a, c, d = s['flaechen'], s['bild']['ansichten'], s['ruhe']['ansichten']"""),
("""    ap.add_argument('--r7c', action='store_true', help='Runde 7c: Urteile nach E9-E13 (auch ohne Laeufe: dann nur offline)')""",
 """    ap.add_argument('--r7c', action='store_true', help='Runde 7c: Urteile nach E9-E13 (auch ohne Laeufe: dann nur offline)')
    ap.add_argument('--farbe', default=None, help='Runde 8: Farbprobe des Schwanzes (a / b / c) fuer R8d (Baender)')"""),
("""              ko=(a.ko_v7, a.ko) if (a.ko_v7 and a.ko) else None)""", """              ko=(a.ko_v7, a.ko) if (a.ko_v7 and a.ko) else None, farbe=a.farbe)"""),
]
for a, b in rep:
    assert a in s, a[:80]
    s = s.replace(a, b)
open(p, 'w', encoding='utf-8').write(s)
p = os.path.join(W, 'riegel_r6.py')
s = open(p, encoding='utf-8').read()
for a, b in (("""def s15_plus(s15, blob):
    import unterseite_r5 as U5
    try:""", """def s15_plus(s15, blob):
    import unterseite_r5 as U5
    if s15.get('r8'):                                             # Runde 8: S15 = R8e (riegel_schwanz_r8.py), Regeln der Runden 5/6 ersetzt
        return s15
    try:"""), ("""    s = r.get('S15', {})
    if 'r6_fehler' in s:""", """    s = r.get('S15', {})
    if s.get('r8'):
        pass
    elif 'r6_fehler' in s:""")):
    assert a in s, a[:60]
    s = s.replace(a, b)
open(p, 'w', encoding='utf-8').write(s)
print('ok')
