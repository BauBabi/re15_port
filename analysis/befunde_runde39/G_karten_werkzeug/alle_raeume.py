"""Marker-Lage je Raum ueber die BEGEHBARE Flaeche (probe_r39_karten markerflaeche):
Saaten = die Ankunftspunkte aller Tueren, die in einen Raum der Stage 1 fuehren (Band aus der
Ankunfts-y: band = -y/1800, re15_collision_band_from_y). Aufruf: alle_raeume.py <probe.exe> [hexraum...]"""
import sys, os, subprocess, re
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're15_port', 'tools'))
import gen_map_zones as GZ
probe = sys.argv[1]
nur = set(int(a, 16) for a in sys.argv[2:])
saaten = {}
STUFEN = [int(a) for a in os.environ.get('R39_STAGES', '1').split(',')]
for rid in [s0 * 0x1000 + k * 0x10 for s0 in STUFEN for k in range(0x30)]:
    r = GZ.read_rdt(rid)
    if not r: continue
    for d in r[1]:
        ziel = d['dest']
        if (ziel >> 12) not in STUFEN: continue
        band = (-d['ny']) // 1800 if d['ny'] < 0 else 0
        saaten.setdefault((ziel, band), []).append((d['nx'], d['nz'], d['ny'], rid))
ges = 0
for (ziel, band), ls in sorted(saaten.items()):
    if nur and ziel not in nur: continue
    gesehen = False
    for (x, z, y, von) in ls:
        out = subprocess.run([probe, 'markerflaeche', '%04X' % ziel, str(band), str(x), str(z), str(y)],
                             capture_output=True, text=True).stdout
        zeile = [l for l in out.splitlines() if l.startswith('ROOM') or l.startswith('FAIL')]
        if not zeile: continue
        m = re.search(r'ausserhalb der Zeichnung (\d+)', zeile[-1])
        g = re.search(r'(\d+) geprueft', zeile[-1])
        print('%04X Band %d Saat (%d,%d) aus %04X: %s' % (ziel, band, x, z, von,
              ('geprueft %s ausserhalb %s' % (g.group(1), m.group(1))) if m else zeile[-1]))
        if m: ges += int(m.group(1)); gesehen = True; break
print('SUMME ausserhalb', ges)
