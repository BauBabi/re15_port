"""hp.py state.log [typ_hex,...] - HP-/Zustandswechsel je Slot (nur Typen), mit Zeilennummer, Spielerwaffe-Magazin mg."""
import re, sys
f = sys.argv[1]; typs = set(int(t, 16) for t in sys.argv[2].split(',')) if len(sys.argv) > 2 else None
rx = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(-?\d+) ss1=(-?\d+) ss2=(-?\d+) ss3=(-?\d+) g=([0-9a-f]+) mo=(-?\d+) af=(-?\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
last = {}
for n, line in enumerate(open(f, errors='replace'), 1):
    m0 = re.match(r'F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*?mg=(-?\d+)', line)
    if not m0: continue
    fr, pad, px, pz, prot, php, mg = m0.groups()
    for m in rx.finditer(line):
        s, t, st, ss1, ss2, ss3, g, mo, af, stun, d, x, z, r, hp = m.groups()
        t = int(t, 16)
        if typs and t not in typs: continue
        key = (int(s), t)
        cur = (int(st), int(hp))
        if last.get(key) != cur:
            print(f"L{n} F{fr} pad={pad} mg={mg} plhp={php} slot={s} t={t:02x} st={st} ss1={ss1} ss2={ss2} g={g} mo={mo} hp={hp} d={d} @({x},{z})")
            last[key] = cur
