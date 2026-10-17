#!/usr/bin/env python3
"""Runde 38 Spur B, Nachbesserung 1 (M1): systematische Riegel-Auswahl.

Alle ctest-Tests, deren Test-exe-Quellen Gegner ueber re15_game_step (frame_step-Helfer)
oder re15_enemy_ai_run_all ticken, plus die, die re15_player_tick direkt rufen (Modul
player_common.c, vom Diff beruehrt). Quelle der Zuordnung: build.ninja (Ziel -> Quellen),
ctest -N -V (Test -> exe). Lokale #include "x.c"/"x.h" werden verfolgt.

Aufruf: python auswahl.py <baum>/re15_port <ctestN.txt> <ausgabe-praefix>
"""
import os
import re
import sys

port = os.path.abspath(sys.argv[1])
ctest_n = sys.argv[2]
out = sys.argv[3]
build = os.path.join(port, 'build')

RX_CALL = {
    'game_step': re.compile(r'\bre15_game_step\s*\('),
    'run_all': re.compile(r'\bre15_enemy_ai_run_all\s*\('),
    'player_tick': re.compile(r'\bre15_player_tick\s*\('),
}
RX_INC = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)

# --- build.ninja: Ziel -> Quellen ------------------------------------------------------------
ziel_quellen = {}
rx_b = re.compile(r'^build (\S+?\.obj): C_COMPILER__(\S+?)_unscanned_ (\S+)')
with open(os.path.join(build, 'build.ninja'), encoding='utf-8', errors='replace') as f:
    for z in f:
        m = rx_b.match(z)
        if not m:
            continue
        ziel = m.group(2)
        src = m.group(3).replace('$:', ':')
        ziel_quellen.setdefault(ziel, []).append(src)

# --- ctest -N -V: Test -> Kommando ------------------------------------------------------------
tests = []   # (nr, name, kommando)
kmd = {}
with open(ctest_n, encoding='utf-8', errors='replace') as f:
    for z in f:
        m = re.match(r'^(\d+): Test command: (.*)$', z.rstrip('\n'))
        if m:
            kmd[int(m.group(1))] = m.group(2)
            continue
        m = re.match(r'^\s*Test\s+#(\d+): (\S+)', z)
        if m:
            nr = int(m.group(1))
            tests.append((nr, m.group(2), kmd.get(nr, '')))

cache = {}


def scan(path, seen):
    """Aufrufe in path inkl. lokaler Includes (nur Dateien unter re15_port/tests)."""
    path = os.path.normpath(path)
    if path in seen:
        return set()
    seen.add(path)
    try:
        txt = open(path, encoding='utf-8', errors='replace').read()
    except OSError:
        return set()
    hits = {k for k, rx in RX_CALL.items() if rx.search(txt)}
    for inc in RX_INC.findall(txt):
        cand = os.path.normpath(os.path.join(os.path.dirname(path), inc))
        if cand.startswith(os.path.join(port, 'tests')) and os.path.exists(cand):
            hits |= scan(cand, seen)
    return hits


def ziel_treffer(ziel):
    if ziel in cache:
        return cache[ziel]
    hits = set()
    for src in ziel_quellen.get(ziel, []):
        if os.path.normpath(src).startswith(os.path.join(port, 'tests')):
            hits |= scan(src, set())
    cache[ziel] = hits
    return hits


auswahl, integ, rest = [], [], []
for nr, name, k in tests:
    if 'cmake.exe' in k.lower().split(' -')[0]:
        integ.append((nr, name, k))
        continue
    exe = k.split(' ')[0] if k else ''
    base = os.path.basename(exe.replace('\\', '/'))
    if base.lower().startswith('cmake'):
        integ.append((nr, name, k))
        continue
    ziel = base[:-4] if base.lower().endswith('.exe') else base
    hits = ziel_treffer(ziel)
    if hits:
        auswahl.append((nr, name, ziel, sorted(hits)))
    else:
        rest.append((nr, name, ziel))

with open(out + '_unit.txt', 'w', encoding='utf-8') as f:
    for nr, name, ziel, hits in auswahl:
        f.write(f'{nr}\t{name}\t{ziel}\t{",".join(hits)}\n')
with open(out + '_rest.txt', 'w', encoding='utf-8') as f:
    for nr, name, ziel in rest:
        f.write(f'{nr}\t{name}\t{ziel}\n')
with open(out + '_integ.txt', 'w', encoding='utf-8') as f:
    for nr, name, k in integ:
        f.write(f'{nr}\t{name}\t{k}\n')
names = sorted({a[1] for a in auswahl})
with open(out + '_regex.txt', 'w', encoding='utf-8') as f:
    f.write('^(' + '|'.join(re.escape(n) for n in names) + ')$\n')
print(f'Tests gesamt {len(tests)}: Auswahl (Gegner-/Spieler-Tick) {len(auswahl)}, '
      f'ohne Tick {len(rest)}, cmake-Skript (Integration) {len(integ)}')
by = {}
for a in auswahl:
    for h in a[3]:
        by[h] = by.get(h, 0) + 1
print('je Aufruf:', by)
