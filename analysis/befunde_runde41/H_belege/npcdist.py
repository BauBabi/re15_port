"""Paarabstand der NPC-Familie (0x40..0x4d) + Spieler je Bild aus RE15_STATE_LOG; meldet Bilder mit
Abstand < Radien-Summe (450+450 fuer 0x40/0x42/0x47/0x49/0x4d, 0x45 500, 0x4b 300; Spieler 450)."""
import re, sys, math
R = {0x40:450,0x42:450,0x47:450,0x49:450,0x4d:450,0x45:500,0x4b:300}
rx = re.compile(r"\[(\d+) t=([0-9a-f]{2}) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]{2}) mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
rp = re.compile(r"^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+)")
fn = sys.argv[1]
mind = {}
treffer = []
for line in open(fn, errors="replace"):
    m = rp.match(line)
    if not m: continue
    f = int(m.group(1)); px, pz = int(m.group(2)), int(m.group(3))
    npcs = []
    for a in rx.finditer(line):
        t = int(a.group(2), 16)
        if t in R:
            x, z = int(a.group(12)), int(a.group(13))
            if x == -30000 and z == -30000: continue  # geparkt
            npcs.append((int(a.group(1)), t, x, z, int(a.group(3)), int(a.group(4))))
    for i in range(len(npcs)):
        for j in range(i+1, len(npcs)):
            s1,t1,x1,z1,st1,ss1 = npcs[i]; s2,t2,x2,z2,st2,ss2 = npcs[j]
            d = math.hypot(x1-x2, z1-z2)
            key = (s1,t1,s2,t2)
            if key not in mind or d < mind[key][0]: mind[key] = (d, f, (x1,z1), (x2,z2))
            if d < R[t1] + R[t2]:
                treffer.append((f, s1, t1, s2, t2, int(d), (x1,z1), (x2,z2), st1, ss1, st2, ss2))
for k,v in mind.items():
    print("Paar Slot%d(t=%02x)-Slot%d(t=%02x): min %.0f bei F%d %s %s (Summe %d)" % (k[0],k[1],k[2],k[3],v[0],v[1],v[2],v[3],R[k[1]]+R[k[3]]))
print("Bilder mit Abstand < Radien-Summe:", len(treffer))
for t in treffer[:5] + (["..."] if len(treffer) > 10 else []) + treffer[-5:]:
    print(" ", t)
