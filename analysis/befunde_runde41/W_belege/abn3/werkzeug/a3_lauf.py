# Abnahme 3 Spur W: startet Messlaeufe der exe-Kopie (eigene PIDs, nie taskkill /IM), je Lauf eigenes Verzeichnis.
# Aufruf: python a3_lauf.py <liste.json> [parallel] [nur=name1,name2]
# Liste: [{"name":..., "exe":"re15_wa3.exe", "env":{...}, "timeout":s, "vorher":[[cmd...]]}]
import json, os, subprocess, sys, time, threading
M = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = os.path.join(M, 'bin'); RUNS = os.path.join(M, 'runs')
BASIS = {'SDL_ASSERT':'always_ignore','RE15_NO_INTRO':'1','RE15_NOAUDIO':'1','RE15_WINDOW_SCALE':'1',
         'RE15_TITLE_CONFIRM_MS':'1500','RE15_PSELECT_AUTO':'1'}
ergebnis = {}
lock = threading.Lock()
def lauf(r):
    d = os.path.join(RUNS, r['name'])
    os.makedirs(d, exist_ok=True)
    for f in os.listdir(d):
        p = os.path.join(d, f)
        if os.path.isfile(p): os.remove(p)
    env = dict(os.environ); env.update(BASIS); env.update(r.get('env', {}))
    for c in r.get('vorher', []):
        cc = [os.path.join(BIN, c[0])] + c[1:]
        out = subprocess.run(cc, cwd=d, capture_output=True, text=True, timeout=120)
        open(os.path.join(d, 'vorher.txt'), 'a').write(' '.join(cc) + '\n' + out.stdout + out.stderr + 'rc=%d\n' % out.returncode)
    exe = os.path.join(BIN, r.get('exe', 're15_wa3.exe'))
    t0 = time.time()
    err = open(os.path.join(d, 'stderr.txt'), 'w')
    p = subprocess.Popen([exe], cwd=d, env=env, stdout=err, stderr=subprocess.STDOUT)
    status = 'ok'
    try:
        rc = p.wait(timeout=r.get('timeout', 300))
    except subprocess.TimeoutExpired:
        status = 'ZEITLIMIT'
        p.kill()   # nur die eigene PID
        rc = p.wait()
    err.close()
    with lock:
        ergebnis[r['name']] = {'pid': p.pid, 'rc': rc, 'status': status, 'dauer_s': round(time.time() - t0, 1), 'exe': r.get('exe', 're15_wa3.exe'), 'env': r.get('env', {})}
        print('%-24s PID %6d rc %s %s %.1f s' % (r['name'], p.pid, rc, status, time.time() - t0), flush=True)
def main():
    liste = json.load(open(sys.argv[1]))
    par = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    nur = None
    for a in sys.argv[3:]:
        if a.startswith('nur='): nur = set(a[4:].split(','))
    if nur: liste = [r for r in liste if r['name'] in nur]
    offen = list(liste); aktiv = []
    while offen or aktiv:
        aktiv = [t for t in aktiv if t.is_alive()]
        while offen and len(aktiv) < par:
            r = offen.pop(0); t = threading.Thread(target=lauf, args=(r,)); t.start(); aktiv.append(t); time.sleep(3)
        time.sleep(1)
    alt = {}
    pf = os.path.join(M, 'laeufe.json')
    if os.path.exists(pf): alt = json.load(open(pf))
    alt.update(ergebnis)
    json.dump(alt, open(pf, 'w'), indent=1)
main()
