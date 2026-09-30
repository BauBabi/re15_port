# Bildzeiten der echten exe von aussen: startet die exe (Kopie), liest state.log laufend (1-ms-Abfrage) und stempelt die
# Ankunft jeder Zeile "F<n> pad=..." mit time.perf_counter(). Kein Eingriff in die exe (kein Framedump: der wuerde die
# Bildzeit selbst verlaengern). Ausgabe: <lauf>/bildzeit.txt  "<segment> <F> <t_ms> <dt_ms>".
#   python bildzeit.py <exe> <laufordner> [ENV=WERT ...]
import os, subprocess, sys, time
exe, zdir = sys.argv[1], sys.argv[2]
os.makedirs(zdir, exist_ok=True)
env = dict(os.environ)
for kv in sys.argv[3:]:
    k, v = kv.split('=', 1); env[k] = v
state = os.path.join(zdir, 'state.log')
if os.path.exists(state): os.remove(state)
p = subprocess.Popen([exe], cwd=zdir, env=env, stdout=open(os.path.join(zdir, 'stdout.txt'), 'w'),
                     stderr=open(os.path.join(zdir, 'stderr.txt'), 'w'))
t0 = time.perf_counter()
pos = 0; rest = b''; seg = 0; prev_f = -1; prev_t = None
out = open(os.path.join(zdir, 'bildzeit.txt'), 'w')
fh = None
while True:
    lebt = p.poll() is None
    if fh is None and os.path.exists(state):
        fh = open(state, 'rb')
    if fh is not None:
        fh.seek(pos)
        data = fh.read()
        if data:
            t = (time.perf_counter() - t0) * 1000.0
            pos += len(data)
            rest += data
            *zeilen, rest = rest.split(b'\n')
            for z in zeilen:
                if not z.startswith(b'F') or b' pad=' not in z: continue
                f = int(z[1:z.index(b' ')])
                if f < prev_f: seg += 1
                prev_f = f
                dt = (t - prev_t) if prev_t is not None else 0.0
                out.write('%d %d %.2f %.2f\n' % (seg, f, t, dt))
                prev_t = t
    if not lebt: break
    time.sleep(0.001)
out.close()
print('rc=%d dauer=%.1f s' % (p.wait(), time.perf_counter() - t0))
