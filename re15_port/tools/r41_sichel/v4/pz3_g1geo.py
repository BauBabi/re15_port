"""Zahlen-Pruefer V4 r3: G1 nach Entscheid O1 - Geometrie der Koerper-Meshes 0-8/10-15 (Ecken, Normalen, Flaechen-Indizes,
UV-Koordinaten, TPage) gegen Ref2, CLUT-Feld getrennt; dazu CLUT-Zeilen je Mesh, Texel-Diff in Koerper-UVs, Mesh 10/13 geteilte Texel."""
import sys, os, numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz3_g1 import secs, md1, tim, emr, R, S
a, b = open(R, 'rb').read(), open(S + 'EM30_V4.EMD', 'rb').read()
A, B = secs(a), secs(b); ma, mb = md1(A[7]), md1(B[7])
koerper = list(range(9)) + list(range(10, 16))
geo_abw, clut = {}, {}
for i in koerper:
    x, y = ma[i], mb[i]; d = []
    for k in ('tv', 'tn', 'qv', 'qn', 'tf', 'qf'):
        if x['blk'][k] != y['blk'][k]: d.append(k)
    if [t[:2] + t[3:] for t in x['tuv_raw']] != [t[:2] + t[3:] for t in y['tuv_raw']]: d.append('tuv_ohne_clut')
    if [q[:2] + q[3:] for q in x['quv_raw']] != [q[:2] + q[3:] for q in y['quv_raw']]: d.append('quv_ohne_clut')
    if d: geo_abw[i] = d
    ca = sorted({t[2] for t in x['tuv_raw']} | {q[2] for q in x['quv_raw']}); cb = sorted({t[2] for t in y['tuv_raw']} | {q[2] for q in y['quv_raw']})
    if ca != cb: clut[i] = ([c >> 6 for c in ca], [c >> 6 for c in cb], [c & 63 for c in cb])
print('Geometrie-Abweichungen Koerper (O1 verlangt leer):', geo_abw or 'keine')
print('CLUT-Feld geaendert (Mesh: Ref2-Zeilen -> V4-Zeilen, x-Teil):', clut)
print('Bytegleich:', [i for i in koerper if ma[i]['blk'] == mb[i]['blk']])
ea, eb = emr(A[2]), emr(B[2])
print('EMR rel 0-15 gleich:', ea['rel'][:96] == eb['rel'][:96], 'Tabelle gleich:', ea['tab'] == eb['tab'])
ks = ea['ks']; ka = np.frombuffer(ea['kf'], np.uint8)[: len(ea['kf']) // ks * ks].reshape(-1, ks); kb = np.frombuffer(eb['kf'], np.uint8)[: len(eb['kf']) // ks * ks].reshape(-1, ks)
print('Keyframes', ka.shape, kb.shape, 'Bytes 0-83 gleich:', ka.shape == kb.shape and np.array_equal(ka[:, :84], kb[:, :84]))
print('Sektionen gleich (0,1,3,4,5,6):', {i: A[i] == B[i] for i in (0, 1, 3, 4, 5, 6)})
ta, tb = tim(A[8]), tim(B[8])
print('TIM CLUT Zeilen Ref2 %d ab %d, V4 %d ab %d' % (ta['ch'], ta['cy'], tb['ch'], tb['cy']))
for z in range(tb['ch']):
    st = 'neu' if z >= ta['ch'] else ('gleich' if np.array_equal(ta['clut'][z], tb['clut'][z]) else 'geaendert')
    print('  CLUT %d: %s' % (tb['cy'] + z, st))
# Texel-Belegung je Mesh (UV-Polygone), geteilte Texel 10/13 und 11/14
def bel(m):
    z = np.zeros(ta['pix'].shape, np.uint8)
    for q in m['quv_raw']:
        pts = np.array([[q[0], q[1]], [q[3], q[4]], [q[9], q[10]], [q[6], q[7]]], np.int32); pts[:, 0] += (q[5] & 0xf) * 128
        cv2.fillPoly(z, [pts], 1); cv2.polylines(z, [pts], True, 1)
    for t in m['tuv_raw']:
        pts = np.array([[t[0], t[1]], [t[3], t[4]], [t[6], t[7]]], np.int32); pts[:, 0] += (t[5] & 0xf) * 128
        cv2.fillPoly(z, [pts], 1); cv2.polylines(z, [pts], True, 1)
    return z > 0
Bm = {i: bel(mb[i]) for i in range(16)}
for p, q in ((10, 13), (11, 14), (10, 11), (13, 14)):
    print('geteilte Texel Mesh %d/%d: %d' % (p, q, int((Bm[p] & Bm[q]).sum())))
dm = ta['pix'] != tb['pix']
kor = np.zeros_like(dm)
for i in koerper: kor |= Bm[i]
print('Texel geaendert gesamt %d, davon in Koerper-UVs %d' % (int(dm.sum()), int((dm & kor).sum())))
