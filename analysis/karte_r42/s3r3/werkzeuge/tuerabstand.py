# Unabhaengiges Mass fuer Ankunftslagen (Pruefbericht pruef_s3_r2 GELB 3: [A] teils selbstbestaetigend):
# Abstand der sichtbaren Punktmitte (marker-1) zum GEMALTEN Tuersymbol der Ankunftstuer.
# Quelle des Symbols: Kommentar "gemaltes Symbol (x,y)" der Tuermarken in re15_map_zones.h - aus der Kunst
# DATA/MAP0x.PIX gelesen (Stufe 1 Runde 2), NICHT aus der Marker-Rechnung. Marken "Mitte der Projektionen"
# stammen aus der Zeile selbst und werden NICHT verwendet.
# Aufruf: tuerabstand.py <auswertung.json> [<auswertung2.json> ...]
import re, sys, json
H = r'C:/workspace/git/reAi_v2/.claude/worktrees/r42_karte/re15_port/engine/src/re15_map_zones.h'
sym = {}
for l in open(H, encoding='utf-8', errors='replace'):
    m = re.match(r'\s*\{\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),.*?/\*\s*([0-9A-F]{4})\.\d<->([0-9A-F]{4})\.\d.*gemaltes Symbol \((\d+),(\d+)\)', l)
    if m:
        pg = int(m.group(1)); a, b = m.group(5), m.group(6)
        sym.setdefault((pg, frozenset((a, b))), []).append((int(m.group(7)), int(m.group(8))))   # mehrere Tueren je Paar: die naechste
def leon(r): return r[:3] + '0'
def mass(js):
    erg = {}
    for x in json.load(open(js)):
        if '_t' not in x['tag'] or not x.get('marker') or x.get('blatt') is None: continue
        k = (x['blatt'], frozenset((leon(x['rid']), leon(x['aus']))))
        if k not in sym: continue
        mx, my = x['marker'][0] - 1, x['marker'][1] - 1
        erg[x['tag']] = min(max(abs(mx - sx), abs(my - sy)) for sx, sy in sym[k])
    return erg
if __name__ == '__main__':
    alle = [mass(j) for j in sys.argv[1:]]
    for j, e in zip(sys.argv[1:], alle):
        v = sorted(e.values())
        print(j.split('/')[-2] if '/' in j else j, 'Ankuenfte mit gemaltem Symbol:', len(v), 'Median', v[len(v)//2] if v else None,
              '<=4px', sum(1 for d in v if d <= 4), '<=8px', sum(1 for d in v if d <= 8), 'max', v[-1] if v else None)
    if len(alle) >= 2:
        a, b = alle[0], alle[-1]
        for t in sorted(set(a) & set(b)):
            if a[t] != b[t]: print('  ', t, a[t], '->', b[t])
