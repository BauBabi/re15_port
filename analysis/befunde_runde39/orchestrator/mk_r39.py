# mk_r39.py — baut r39.js aus r39_kopf.js (Spuren der Runde 39) + dem Rumpf von r38.js (Prompts, Schemata, Ablauf).
# Aufruf: python analysis/befunde_runde39/orchestrator/mk_r39.py
import os
hier = os.path.dirname(os.path.abspath(__file__))
r38 = open(os.path.join(hier, '..', '..', 'befunde_runde38', 'orchestrator', 'r38.js'), encoding='utf-8').read()
kopf = open(os.path.join(hier, 'r39_kopf.js'), encoding='utf-8').read()
start = r38.index('const REGELN = ')
rumpf = r38[start:]
ersetzen = [
    ('befunde_runde38', 'befunde_runde39'),
    ('befunde_runde37', 'befunde_runde38'),
    ('Runde 38', 'Runde 39'),
    ('r38', 'r39'),
    ('(2026-10-16)', '(2026-10-18)'),
    ('Stand 2026-10-16', 'Stand 2026-10-18'),
    ('v0.8.23 3ec5b1f8 / v0.8.24 e8b3958f / v0.8.25 2077bb8e, dann die Runde-37-Merges',
     'v0.8.24 e8b3958f / v0.8.25 2077bb8e / v0.8.26 (Tag v0.8.26), dann die Runde-38-Merges'),
    ('v0.8.25-exe', 'v0.8.26-exe'),
    ('Muster probes/r37_*.cmake', 'Muster probes/r38_*.cmake'),
    ('Arbeitsordner eines Tests nie gleich dem Target-Namen (Linux ohne .exe).',
     'Arbeitsordner eines Tests nie gleich dem Target-Namen (Linux ohne .exe). Tests parallel- und Linux-fest '
     '(VERTRAG §2, Lehren des Release v0.8.26): exe-Kopie mit Praefix JE TEST (nie ein Glob, der Kopien eines '
     'Geschwister-Tests loescht), eigener Laufordner je Lauf, keine Umgebungsvariablen des Aufrufers erben; '
     "'m' linken bei sqrt/sin/cos/atan2/pow; cmake_minimum_required(VERSION 3.16) in jedem -P-Skript; "
     'POSIX-Funktionen mit _DEFAULT_SOURCE.'),
]
for alt, neu in ersetzen:
    n = rumpf.count(alt)
    if n == 0:
        raise SystemExit('nicht gefunden: ' + alt[:60])
    rumpf = rumpf.replace(alt, neu)
    print('%3dx %s' % (n, alt[:70]))
open(os.path.join(hier, 'r39.js'), 'w', encoding='utf-8', newline='\n').write(kopf.rstrip('\n') + '\n\n' + rumpf)
print('r39.js geschrieben')
