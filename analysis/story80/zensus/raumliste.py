#!/usr/bin/env python3
"""Raumliste Port (Mod-Baum) gegen 40%-Disc.
Je Raumnummer: Dateien im Port-Baum (echte RDT / 4-Byte-Platzhalter), Dateien auf der Disc, EXE-Raumname der Disc
(raumnamen.py, @0x8006ff36), Kamerazahl (RDT+0x01) und Kamerablock (RDT+0x24, je Kamera die ersten 28 von 32 Byte = Lage/Ziel, ohne PRI-Zeiger) Mod gegen Disc,
NPC-Typen (Sce_em_set 0x40..0x4F, nicht geparkt) und Textzahl je Datei aus den Zensus-JSONs.
Aufruf: python raumliste.py <port.json> <disc.json> <namen_disc.json> <port-PSX> <disc-PSX> <aus.md>"""
import sys, os, json, glob, struct
P = json.load(open(sys.argv[1], encoding='utf-8')); D = json.load(open(sys.argv[2], encoding='utf-8'))
N = json.load(open(sys.argv[3], encoding='utf-8')); PR, DR = sys.argv[4], sys.argv[5]
EM = {0x40: 'Marvin', 0x42: 'Ada', 0x45: 'Irons', 0x47: 'Roy/Elliot', 0x49: 'John', 0x4B: 'Sherry', 0x4D: 'Annette(0x4D)'}
def rdt(root, k):
    p = os.path.join(root, 'STAGE' + k[0], 'ROOM%s.RDT' % k)
    return open(p, 'rb').read() if os.path.exists(p) else None
def kam(d):
    if d is None or len(d) < 0x60: return None
    n = d[1]; o = struct.unpack_from('<I', d, 0x24)[0]
    return n, [d[o + i * 32: o + i * 32 + 28] for i in range(n)]   # Position/Ziel je Kamera, ohne PRI-Zeiger +28
def figs(r):
    s = sorted(set(EM.get(n['typ'], hex(n['typ'])) for n in r['npcs'] if 0x40 <= n['typ'] <= 0x4F and not n['park']))
    return ', '.join(s)
nums = sorted(set(k[:3] for k in P) | set(k[:3] for k in D) | set(N))
rows = []; cnt = dict(port_echt=0, port_stub=0, disc=0, nur_mod=0, beide=0, keine=0, kamgleich=0, kamanders=0)
for rn in nums:
    pf = [rn + v for v in '01' if rn + v in P]
    pe = [k for k in pf if 'stub' not in P[k]]; ps = [k for k in pf if 'stub' in P[k]]
    df = [rn + v for v in '01' if rn + v in D]
    cnt['port_echt'] += len(pe); cnt['port_stub'] += len(ps); cnt['disc'] += len(df)
    if pe and df: st = 'Disc + Mod'; cnt['beide'] += 1
    elif pe: st = 'NUR MOD'; cnt['nur_mod'] += 1
    elif df: st = 'nur Disc'
    else: st = 'KEINE DATEI'; cnt['keine'] += 1
    kv = []
    for k in pe:
        if k in D:
            a, b = kam(rdt(PR, k)), kam(rdt(DR, k))
            gl = sum(1 for x in a[1] if x in b[1])
            if a[0] == b[0] and gl == a[0]: kv.append('%s gleich (%d)' % (k[3], a[0])); cnt['kamgleich'] += 1
            else: kv.append('%s %d von %d Mod-Kameras auf Disc (Disc %d)' % (k[3], gl, a[0], b[0])); cnt['kamanders'] += 1
    fig = '; '.join('%s: %s' % (k[3], figs(P[k])) for k in pe if figs(P[k]))
    dfig = '; '.join('%s: %s' % (k[3], figs(D[k])) for k in df if figs(D[k]))
    tx = '/'.join(str(len(P[k]['texts'])) for k in pe) or '-'
    dtx = '/'.join(str(len(D[k]['texts'])) for k in df) or '-'
    rows.append('| %s | %s | %s | %s | %s | %s | %s | %s | %s | %s |' % (
        rn, N.get(rn, ''), st, ' '.join(pe) + ((' (Platzhalter: ' + ' '.join(ps) + ')') if ps else ''),
        ' '.join(df) or '-', '; '.join(kv) or '-', tx, dtx, fig or '-', dfig or '-'))
hdr = ['| Raum | EXE-Name (Disc) | Bestand | Port-Dateien | Disc-Dateien | Kameras Mod=Disc? | Texte Port | Texte Disc | Figuren Port (nicht geparkt) | Figuren Disc |',
       '|---|---|---|---|---|---|---|---|---|---|']
open(sys.argv[6], 'w', encoding='utf-8').write('# Raumliste Port gegen 40%-Disc (erzeugt von raumliste.py)\n\n' +
    'Zaehlung: %s\n\n' % json.dumps(cnt) + '\n'.join(hdr + rows) + '\n')
print(cnt)
