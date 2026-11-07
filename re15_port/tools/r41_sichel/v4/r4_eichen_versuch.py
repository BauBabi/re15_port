"""SF V4 Runde 4 (Bauer): Versuchslauf der Eichung (eichen_v4.main) mit geaenderten SCHWANZ_V4 / KOPF_V4-Werten, ohne den Baum zu aendern.

    python r4_eichen_versuch.py <arbeitsordner> [--schwanz JSON-Datei] [--vor 0.06] [weitere eichen_v4-Argumente]

Schreibt <arbeitsordner>/farben_v4.npz und eichung_v4.json; ein Versuchsbau mit diesen Farben: r2_expt.bauen(sw, farben=...).
"""
import os, sys, json
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G  # noqa: F401  (Pfade)
import birkin_formen as BF

arb = sys.argv[1]
rest = sys.argv[2:]
sw = None
if '--schwanz' in rest:
    i = rest.index('--schwanz'); sw = json.load(open(rest[i + 1])); del rest[i:i + 2]
if '--vor' in rest:
    i = rest.index('--vor'); BF.KOPF_V4['vorbau'] = dict(BF.KOPF_V4['vorbau'], vor=float(rest[i + 1])); del rest[i:i + 2]
if sw:
    BF.SCHWANZ_V4.update(sw)
os.makedirs(arb, exist_ok=True)
import eichen_v4 as E
sys.argv = ['eichen_v4.py', '--aus', os.path.join(arb, 'farben_v4.npz'), '--arb', arb, '--prot', os.path.join(arb, 'eichung_v4.json')] + rest
E.main()
