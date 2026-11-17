"""Pruefer P5 (A) Unveraendertes + Format, eigener Leser (pl.py, Pruefer-Kopie). Aufruf: python struktur.py KAND.EMD"""
import sys, struct, hashlib
import numpy as np
sys.path.insert(0, __file__.rsplit('/', 1)[0])
import pl
M = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/m/'
ein = open(M + 'R6.EMD', 'rb').read(); v7 = open(M + 'V7.EMD', 'rb').read(); k = open(sys.argv[1], 'rb').read()
Ei, E7, Ek = pl.EMD(ein), pl.EMD(v7), pl.EMD(k)
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
print('Datei', sys.argv[1], len(k), 'B sha', hashlib.sha256(k).hexdigest()[:16])
print('Verzeichnis: Eingang', len(Ei.dir), Ei.dir, '| Kandidat', len(Ek.dir), Ek.dir)
# Abschnitte 0-6 und TIM
for i in range(len(Ek.dir)):
    a0, a1 = Ei.dir[i], (Ei.dir[i + 1] if i + 1 < len(Ei.dir) else u32(ein, 0))
    b0, b1 = Ek.dir[i], (Ek.dir[i + 1] if i + 1 < len(Ek.dir) else u32(k, 0))
    if i == 7:
        print('  Abschnitt 7 (MD1) Laenge Eingang %d Kandidat %d' % (a1 - a0, b1 - b0)); continue
    print('  Abschnitt %d gleich=%s (Laenge %d/%d)' % (i, ein[a0:a1] == k[b0:b1], a1 - a0, b1 - b0))
print('  Kopf vor Abschnitt 0 gleich', ein[:Ei.dir[0]][8:] == k[:Ek.dir[0]][8:] if Ei.dir[0] == Ek.dir[0] else 'Lage anders',
      '| Verzeichnis-Ende/Schwanz gleich', ein[u32(ein, 0) + 4 * len(Ei.dir):] == k[u32(k, 0) + 4 * len(Ek.dir):])
print('  EDD/EMR/TIM bytegleich', Ei.edd == Ek.edd, Ei.emr == Ek.emr, Ei.tim == Ek.tim, '| Texturbild', np.array_equal(Ei.tex, Ek.tex),
      '| CLUT', np.array_equal(Ei.clut, Ek.clut))
md = Ek.md1
print('  MD1 Kopf: laenge %d (Abschnitt %d) unbekannt %d/%d objekte %d/%d' % (u32(md, 0), len(md), u32(md, 4), u32(Ei.md1, 4), u32(md, 8), u32(Ei.md1, 8)))
anders = [i for i in range(len(Ek.meshes)) if Ek.mesh_raw[i] != Ei.mesh_raw[i]]
print('  Meshes ungleich Eingang:', anders, '| Anzahl', len(Ek.meshes), len(Ei.meshes))
# Mesh 9 Praefix gegen V7
m7, mk = E7.meshes[9], Ek.meshes[9]
ok = True
for key in ('tv', 'tn', 'qv', 'qn'):
    n = len(m7[key]); g = len(mk[key]) >= n and np.array_equal(mk[key][:n], m7[key])
    print('  Mesh9 %s V7 %d Kand %d Praefix gleich %s' % (key, n, len(mk[key]), g)); ok &= g
for key in ('tf', 'tuv', 'qf', 'quv'):
    n = len(m7[key]); g = len(mk[key]) >= n and list(mk[key][:n]) == list(m7[key])
    print('  Mesh9 %s V7 %d Kand %d Praefix gleich %s' % (key, n, len(mk[key]), g)); ok &= g
# Pad-Felder der Ecken/Normalen (pl liest nur 3 von 4 s16): Rohbytes Mesh 9 Praefix
d = Ek.md1; f9 = struct.unpack_from('<14I', d, 12 + 56 * 9); d7 = E7.md1; g9 = struct.unpack_from('<14I', d7, 12 + 56 * 9)
for nm, io, ic, w in (('tv', 0, 1, 8), ('tn', 2, 3, 8), ('tf', 4, 5, 12), ('tuv', 6, 5, 12), ('qv', 7, 8, 8), ('qn', 9, 10, 8), ('qf', 11, 12, 16), ('quv', 13, 12, 16)):
    n = g9[ic]; a = d7[12 + g9[io]:12 + g9[io] + w * n]; b = d[12 + f9[io]:12 + f9[io] + w * n]
    print('  Mesh9 roh %s %d B gleich %s' % (nm, len(a), a == b)); ok &= (a == b)
print('MESH9-PRAEFIX', 'GRUEN' if ok else 'ROT')
# angehaengt
for key, kf in (('tf', 'tuv'), ('qf', 'quv')):
    n = len(m7[key])
    for j in range(n, len(mk[key])):
        uv = mk[kf][j]
        print('    neu Mesh9 %s%d idx %s clut-zeile %d seite %d uv %s' % (key, j, mk[key][j], (uv[2] >> 6) & 0x1FF, uv[5] & 0xF, (uv[0], uv[1], uv[3], uv[4], uv[6], uv[7]) + ((uv[9], uv[10]) if key == 'qf' else ())))
# Formatgrenzen
fehler = []
zeilen_ein = set()
for m in Ei.meshes:
    for uv in m['tuv'] + m['quv']: zeilen_ein.add((uv[2] >> 6) & 0x1FF)
seiten_ein = set()
for m in Ei.meshes:
    for uv in m['tuv'] + m['quv']: seiten_ein.add(uv[5])
for mi, m in enumerate(Ek.meshes):
    for key, vk, nk, w in (('tf', 'tv', 'tn', 3), ('qf', 'qv', 'qn', 4)):
        for j, f in enumerate(m[key]):
            if max(f[1::2]) >= len(m[vk]) or max(f[0::2]) >= len(m[nk]): fehler.append((mi, key, j, 'Index'))
    for key in ('tuv', 'quv'):
        for j, uv in enumerate(m[key]):
            if ((uv[2] >> 6) & 0x1FF) not in zeilen_ein: fehler.append((mi, key, j, 'Zeile neu %d' % ((uv[2] >> 6) & 0x1FF)))
            if uv[5] not in seiten_ein: fehler.append((mi, key, j, 'Seite neu %d' % uv[5]))
    for key in ('tv', 'tn', 'qv', 'qn'):
        if len(m[key]) and (np.abs(m[key]) > 32767).any(): fehler.append((mi, key, 's16'))
ntri = sum(len(m['tf']) + 2 * len(m['qf']) for m in Ek.meshes)
print('FORMAT Meshes %d (<= MD1_MAX_MESHES 32), Dreiecke+2*Vierecke %d (< TEXTRI_QUEUE_MAX 8192), Fehler %s' % (len(Ek.meshes), ntri, fehler[:10]))
print('  Rundlauf: MD1-Laenge Feld %d = Abschnitt %d: %s' % (u32(md, 0), len(md), u32(md, 0) == len(md)))
# Mesh 8 Diff
a, b = Ei.meshes[8], Ek.meshes[8]
print('Mesh 8 Eingang v/n/tf/qf %d/%d/%d/%d -> Kand %d/%d/%d/%d' % (len(a['tv']), len(a['tn']), len(a['tf']), len(a['qf']), len(b['tv']), len(b['tn']), len(b['tf']), len(b['qf'])))
n = min(len(a['tv']), len(b['tv']))
for i in range(n):
    if not np.array_equal(a['tv'][i], b['tv'][i]): print('   Ecke %d %s -> %s' % (i, a['tv'][i].tolist(), b['tv'][i].tolist()))
for i in range(min(len(a['tn']), len(b['tn']))):
    if not np.array_equal(a['tn'][i], b['tn'][i]): print('   Normale %d %s -> %s' % (i, a['tn'][i].tolist(), b['tn'][i].tolist()))
print('   tv==qv Kand', np.array_equal(b['tv'], b['qv']), 'tn==qn', np.array_equal(b['tn'], b['qn']))
for key, kf in (('tf', 'tuv'), ('qf', 'quv')):
    for j in range(max(len(a[key]), len(b[key]))):
        x = (a[key][j], a[kf][j]) if j < len(a[key]) else None; y = (b[key][j], b[kf][j]) if j < len(b[key]) else None
        if x != y: print('   %s%d %s -> %s' % (key, j, x, y))
