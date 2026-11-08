import re, sys, numpy as np
def lese(pfad):
    out = {}
    for z in open(pfad, errors='replace'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', z)
        if not m: continue
        h = int(m.group(1)); P = [tuple(map(int, t)) for t in re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(3))]
        out[h] = np.array(P, float)
    return out
def winkel(P):
    d = np.diff(P, axis=0); d /= np.linalg.norm(d, axis=1, keepdims=True)
    return np.degrees(np.arccos(np.clip((d[:-1]*d[1:]).sum(1), -1, 1)))
if __name__ == '__main__':
    g = lese(sys.argv[1])
    for h in sorted(g):
        if h % 6 == 0: print(h, np.round(winkel(g[h])).astype(int))
