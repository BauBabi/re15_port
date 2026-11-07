# Abnahme 3 Spur W: Auswertung je Lauf (debug.log, wf.log, fx.log, RE15_FRAMEDUMP-Serie).
# Fenster (320x240-Bildpunkte): Tropfen-Fleck Cut 2 (83,0..1), Cut 5 (193,1..2), Cut 7 (169,15) aus W_abnahme_2 H1;
# Kanal x 104..230 y 100..200 (Riegel integration_r41_wasserblink_leon/_elza); Wasserlauf x 72..124 y 146..179 (W_abnahme_2 §6).
import os, re, sys, json, glob
import numpy as np
M = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RUNS = os.path.join(M, 'runs')
FENSTER = {'rand2': (76, 92, 0, 5), 'rand5': (186, 200, 0, 5), 'rand7': (162, 176, 10, 20),
           'kanal': (104, 230, 100, 200), 'wasserlauf': (72, 124, 146, 179)}
def ppm(p):
    with open(p, 'rb') as f:
        d = f.read()
    # P6\n<w> <h>\n255\n
    parts = d.split(b'\n', 3)
    w, h = map(int, parts[1].split())
    return np.frombuffer(parts[3][:w*h*3], dtype=np.uint8).reshape(h, w, 3).astype(np.int16)
def zaehl(text, muster): return len(re.findall(muster, text, re.M))
def auswert(name):
    d = os.path.join(RUNS, name)
    dbg = open(os.path.join(d, 'debug.log'), errors='replace').read() if os.path.exists(os.path.join(d, 'debug.log')) else ''
    wf = open(os.path.join(d, 'wf.log'), errors='replace').read() if os.path.exists(os.path.join(d, 'wf.log')) else ''
    fx = open(os.path.join(d, 'fx.log'), errors='replace').read() if os.path.exists(os.path.join(d, 'fx.log')) else ''
    r = {'lauf': name}
    r['raeume'] = re.findall(r'PC loaded room([0-9a-fA-F]{4})\.rdt', dbg)
    r['tropfen41'] = zaehl(dbg, r'^\[tropfen41\] ')
    r['tropfen41_zeilen'] = sorted(set(re.findall(r'^\[tropfen41\] (ROOM\w+ main00 @0x\w+ Evt_exec sub\d+)', dbg, re.M)))
    m = re.search(r'EXIT_AT: Bild (\d+) in Raum (\w+) erreicht', dbg)
    r['exit_at'] = (m.group(1), m.group(2)) if m else None
    cuts = re.findall(r'^\[pri\] cut=(\d+) ', dbg, re.M)
    r['cuts'] = [c for i, c in enumerate(cuts) if i == 0 or cuts[i-1] != c]
    r['wasserlauf41'] = zaehl(dbg, r'^\[wasserlauf41\] ROOM200[01]: 6 Strahlen')
    r['figur'] = re.findall(r'Spieler-Familie (PL0\d)', dbg)[:1]
    r['substart'] = re.findall(r'^\[substart\] (.*)$', dbg, re.M)
    r['continue'] = re.findall(r'^\[save\] CONTINUE: (resumed in room \w+)', dbg, re.M)
    r['evt_200x'] = re.findall(r'^\[evt\] F\d+ room=(200[01]) Evt_exec sub=(\d+)', dbg, re.M)
    r['door_fire'] = re.findall(r'^\[aot\] DOOR FIRE slot=(\d+)', dbg, re.M)
    r['wf_spawn_6_0'] = zaehl(wf, r'SPAWN id=6 sub=0 ')
    r['wf_spawn_11_0'] = zaehl(wf, r'SPAWN id=11 sub=0 ')
    r['wf_spawn_6_andere'] = zaehl(wf, r'SPAWN id=6 sub=[1-9]')
    r['fx_6_0'] = zaehl(fx, r'^id=6 sub=0 ')
    r['fx_11'] = zaehl(fx, r'^id=11 ')
    r['fx_6_andere'] = zaehl(fx, r'^id=6 sub=[1-9]')
    f60 = re.findall(r'^id=6 sub=0 [^\n]*F=(\d+)$', fx, re.M)
    r['fx_6_0_erstes_bild'] = int(f60[0]) if f60 else None
    # Bilder
    bilder = sorted(glob.glob(os.path.join(d, '*_[0-9][0-9][0-9][0-9][0-9][0-9].ppm')))
    r['bilder'] = len(bilder)
    if bilder:
        r['bild_von_bis'] = (os.path.basename(bilder[0]), os.path.basename(bilder[-1]))
        st = np.stack([ppm(b) for b in bilder])          # n,h,w,3
        dif = np.abs(np.diff(st, axis=0)).max(axis=3)     # n-1,h,w  (groesster Kanalsprung je Bildpunkt)
        fen = {}
        for k, (x0, x1, y0, y1) in FENSTER.items():
            w = dif[:, y0:y1+1, x0:x1+1]
            je = w.reshape(w.shape[0], -1).max(axis=1)
            idx = np.argwhere(w > 8)
            ort = None
            if len(idx):
                # haeufigster Ort
                u, c = np.unique(idx[:, 1:], axis=0, return_counts=True)
                j = c.argmax(); ort = (int(u[j][1] + x0), int(u[j][0] + y0), int(c[j]))
            erst = None
            nz = np.nonzero(je > 8)[0]
            if len(nz): erst = os.path.basename(bilder[nz[0] + 1])
            fen[k] = {'wechsel_gt8': int((je > 8).sum()), 'paare': int(len(je)), 'groesster': int(je.max()), 'ort_haeufigst': ort, 'erster_wechsel': erst}
        r['fenster'] = fen
        # Wechselzahl-Karte (|d| > 16), Bildpunkte mit >= 6 Wechseln, Kasten der Cluster
        karte = (dif > 16).sum(axis=0)
        r['karte_punkte_ge6'] = int((karte >= 6).sum())
        ys, xs = np.nonzero(karte >= 6)
        r['karte_kasten'] = (int(xs.min()), int(xs.max()), int(ys.min()), int(ys.max())) if len(xs) else None
        np.save(os.path.join(d, 'karte.npy'), karte)
        med = np.median(st, axis=0).astype(np.uint8)
        np.save(os.path.join(d, 'median.npy'), med)
    return r
if __name__ == '__main__':
    namen = sys.argv[1:] or sorted(os.listdir(RUNS))
    alle = {}
    for n in namen:
        if not os.path.isdir(os.path.join(RUNS, n)): continue
        r = auswert(n); alle[n] = r
        print(json.dumps(r, ensure_ascii=False))
    pf = os.path.join(M, 'auswertung.json')
    alt = json.load(open(pf)) if os.path.exists(pf) else {}
    alt.update(alle)
    json.dump(alt, open(pf, 'w'), indent=1, ensure_ascii=False)
