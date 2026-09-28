#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
r30_karten_gen_diag.py - Runde 30, Thema F (Karten-Marken).

Faehrt tools/gen_map_zones.py UNVERAENDERT im Speicher, aber
  * JEDES Schreiben wird in build/r30_karten-marken/gen/ umgeleitet
    (der Kopf engine/src/re15_map_zones.h bleibt unberuehrt), und
  * die Marken-Rohliste bekommt eine Herkunftsspalte: fuer welche
    (Raum, Zone) fehlte die zid, so dass der Generator den Rueckfall 0 einsetzte.

Aufruf:  python analysis/befunde_runde30/r30_karten_gen_diag.py [git-rev]
         (mit git-rev wird der Generator dieses Standes aus `git show` gelesen)
"""
import builtins, io, os, sys, subprocess, json

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
TOOLS = os.path.join(REPO, 're15_port', 'tools')
AUS = os.path.join(REPO, 'build', 'r30_karten-marken', 'gen')
os.makedirs(AUS, exist_ok=True)

rev = sys.argv[1] if len(sys.argv) > 1 else None
if rev:
    src = subprocess.check_output(['git', '-C', REPO, 'show',
                                   rev + ':re15_port/tools/gen_map_zones.py']).decode('utf-8')
else:
    src = io.open(os.path.join(TOOLS, 'gen_map_zones.py'), encoding='utf-8').read()

# ---- Schreibschutz: jedes Oeffnen zum Schreiben landet in AUS ----------------
_open0, _ioopen0 = builtins.open, io.open
def _umleiten(pfad, mode):
    if isinstance(pfad, (str, bytes, os.PathLike)) and any(c in mode for c in 'wax+'):
        neu = os.path.join(AUS, os.path.basename(str(pfad)))
        sys.stderr.write('[diag] Schreiben umgeleitet: %s -> %s\n' % (pfad, neu))
        return neu
    return pfad
def open_s(pfad, mode='r', *a, **k):
    return _open0(_umleiten(pfad, mode), mode, *a, **k)
def ioopen_s(pfad, mode='r', *a, **k):
    return _ioopen0(_umleiten(pfad, mode), mode, *a, **k)
builtins.open = open_s
io.open = ioopen_s

# ---- Herkunftsspalte -----------------------------------------------------------
ALT = "'zid': zid_of.get((b, zi), 0), 'd': m})"
NEU = "'zid': zid_of.get((b, zi), 0), 'zid_fehlt': ((b, zi) not in zid_of), 'd': m})"
n = src.count(ALT)
sys.stderr.write('[diag] Rueckfall-Stelle zid_of.get(...,0): %d Treffer\n' % n)
src = src.replace(ALT, NEU)

ALT2 = "    marks = [(v['pg'], v['r'], v['mx'], v['my'], v['seite'], v['zid'],"
NEU2 = ("    import json as _j9\n"
        "    _diag = [dict(room='%04X' % v['room'], zi=v['zi'], idx=v['idx'], kind=v['kind'],\n"
        "                  pg=v['pg'], r=v['r'], mx=v['mx'], my=v['my'], seite=v['seite'],\n"
        "                  zid=v['zid'], zid2=v.get('zid2', 255),\n"
        "                  zid_fehlt=bool(v.get('zid_fehlt')),\n"
        "                  d={k9: (('%04X' % w9) if k9 in ('dest',) and isinstance(w9, int) else w9)\n"
        "                     for k9, w9 in v['d'].items()\n"
        "                     if isinstance(w9, (int, float, str, bool, type(None)))})\n"
        "             for v in marks]\n"
        "    _j9.dump(_diag, open(r'" + os.path.join(AUS, 'marken_herkunft.json') + "', 'w'), indent=1)\n"
        + ALT2)
n2 = src.count(ALT2)
sys.stderr.write('[diag] Marken-Ausgabestelle: %d Treffer\n' % n2)
src = src.replace(ALT2, NEU2)

sys.path.insert(0, TOOLS)
g = {'__name__': '__main__', '__file__': os.path.join(TOOLS, 'gen_map_zones.py')}
sys.argv = [g['__file__']]
exec(compile(src, g['__file__'], 'exec'), g)
