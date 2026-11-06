"""SF V4 (Bauer): Feinsuche Biegung (Gelenke 2-4) gegen T2-Drehung 60-100 Grad bei T2-T6 gruen (Ausgabe im Dossier V4)."""
import sys, os, itertools, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import form_suche_v4 as F
J0 = [[0.12, -0.08], [0.03, -0.05], [-0.107, 0.014], [-0.244, 0.0785], [-0.268, 0.1907], [-0.268, 0.3029], [-0.256, 0.3626], [-0.245, 0.4222]]
D = [0.16, 0.15, 0.1526, 0.1284, 0.0475, 0.022, 0.038, 0.038]
erg = []
for j2y, j3y, j4x in itertools.product((0.014, 0.035, 0.055), (0.06, 0.0785), (-0.268, -0.285)):
    J = [list(x) for x in J0]; J[2][1] = j2y; J[3][1] = j3y; J[4][0] = j4x
    s, _i = F.messen(dict(gelenke=[tuple(x) for x in J], durchmesser=D, tiefe=[0] * 8))
    t = (abs(s['T2'][0] + 0.245) <= 0.03 and abs(s['T2'][1] - 0.036) <= 0.03 and 60 <= s['drehung'] <= 100,
         abs(s['T3'][0] + 0.265) <= 0.03 and abs(s['T3'][1] - 0.257) <= 0.03, abs(s['T4'][0] + 0.245) <= 0.03 and abs(s['T4'][1] - 0.397) <= 0.03)
    print(j2y, j3y, j4x, 'T1 %.3f/%.3f T2 %.3f/%.3f (%.0f) T3 %.3f/%.3f T4 %.3f/%.3f f %.3f %.3f %.3f %.3f' % (
        *s['T1'], *s['T2'], s['drehung'], *s['T3'], *s['T4'], s['T5'][0.1], s['T5'][0.3], s['T5'][0.5], s['T5'][0.7]), t, flush=True)
