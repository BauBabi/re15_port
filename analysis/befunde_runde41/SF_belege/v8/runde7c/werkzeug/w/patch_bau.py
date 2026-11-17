p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8/v8_bau.py'
s = open(p, encoding='utf-8').read()
alt = """        neu = SZ.stufe_b(neu, (hand or _p('hand_v8.json'))['ellbogen'], prot)
"""
neu = """        neu = SZ.stufe_b(neu, (hand or _p('hand_v8.json'))['ellbogen'], prot)
    if 'p' in stufen:                 # Runde 7c (P5, Nutzer: "smooth straight to head without further hills...."; Luecke Ruecken/Kopf ohne Kragen):
        import p5_stufe as P5         # Kandidat R3 des P5-Ablaufs + Pflicht-Korrekturen K-a..K-e (p5_stufe.py); letzte Stufe, aendert nur Mesh 8
        vor = V.sha(neu)              # (Mesh 9 = V7-Kopf wird nur geprueft)
        neu = P5.stufe(neu)
        prot['p5'] = dict(eingang_sha256=vor, korr_uv=P5.KORR_UV, korr_ecken=P5.KORR_ECKEN)
"""
assert alt in s; s = s.replace(alt, neu)
doc_alt = """   fasst nur die 36 Bit von Knochen 11 je Keyframe an (alle anderen Knochen, Meshes, Textur bytegleich). Bau Runde 6c: --stufen srwaunhb.
"""
doc_neu = doc_alt + """Stufe p) Runde 7c (P5, p5_stufe.py): Luecke Ruecken/Kopf ohne Kragen = Kandidat R3 des P5-Ablaufs (Nackenstueck r6 weg, Ecken 6/43 auf r4,
   Ecken 17/30/46 nach E10, Viereck-Ergaenzungen d77-d80 nach E11, UV-Rueckbau auf r4) und die Pflicht-Korrekturen K-a..K-e der Pruefer R3.
   Laeuft als letzte Stufe auf dem gepackten Stand, aendert nur Mesh 8 (Mesh 9 bleibt der V7-Kopf). Bau Runde 7c: --stufen srwaunhbp.
"""
assert doc_alt in s; s = s.replace(doc_alt, doc_neu)
a = "    ap.add_argument('--stufen', default='srwaunhb')"
assert a in s; s = s.replace(a, "    ap.add_argument('--stufen', default='srwaunhbp')")
open(p, 'w', encoding='utf-8', newline='\n').write(s)
k = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8_kette.sh'
s = open(k, encoding='utf-8').read()
n0 = s.count('srwaunhb')
s = s.replace('--stufen srwaunhb', '--stufen srwaunhbp')
s = s.replace("#                                    -> EM30_V8.EMD = Pin (V8)", """#       (Runde 7c P5: Stufe p = v8/p5_stufe.py: Kandidat R3 des P5-Ablaufs (scratchpad sfv8/p5/kand_r3, sha256 9860f6bd auf r6) und die
#        Pflicht-Korrekturen K-a..K-e (UV t60/q15/t8, Ecken 17 und 25); letzte Stufe, aendert nur Mesh 8; Stufenfolge s r w a u n h b p)
#                                    -> EM30_V8.EMD = Pin (V8)""")
open(k, 'w', encoding='utf-8', newline='\n').write(s)
print('ersetzt', n0)
