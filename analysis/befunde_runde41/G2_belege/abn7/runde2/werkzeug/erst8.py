# Abnahme 8 (eigenes Werkzeug): erste abweichende state.log-Zeile zweier Laeufe im letzten Besuch (Bild fuer Bild), dazu der erste Treffer (hp-Flanke) beider Laeufe.
# Aufruf: python erst8.py <lauf_a> <lauf_b>
import sys, os, re
def lv(d):
    st = open(os.path.join(d, 'state.log'), encoding='latin-1').read().splitlines()
    out = []; cur = []; pf = None
    for z in st:
        m = re.match(r'^F(\d+) ', z)
        if not m: continue
        f = int(m.group(1))
        if pf is not None and f <= pf: out.append(cur); cur = []
        cur.append(z); pf = f
    out.append(cur)
    return out
def ersttreffer(v):
    pv = None
    for z in v:
        m = re.search(r'hp=(-?\d+)\)', z)
        if m:
            h = int(m.group(1))
            if pv is not None and h < pv: return z.split()[0]
            pv = h
    return '-'
A = lv(sys.argv[1]); B = lv(sys.argv[2])
print('Besuche %d / %d' % (len(A), len(B)))
for i in range(min(len(A), len(B))):
    a, b = A[i], B[i]
    n = min(len(a), len(b)); diff = None
    for k in range(n):
        if a[k] != b[k]: diff = k; break
    print('  Besuch %d: %d / %d Zeilen; erste Abweichung %s; erster Treffer %s / %s' % (i, len(a), len(b), ('Zeile %d (%s)' % (diff, a[diff].split()[0])) if diff is not None else ('keine (gleich bis %d)' % n), ersttreffer(a), ersttreffer(b)))
    if diff is not None:
        print('    a: ' + a[diff][:230]); print('    b: ' + b[diff][:230])
