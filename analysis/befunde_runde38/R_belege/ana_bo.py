import re, sys, collections
# Auswertung RE15_SPINNEN_LOG (Fortsetzung 2026-10-17): Lage-Y gegen bu = hoechste SCA-Bodenflaeche
# AUF/UNTER der Figur (FUN_8001c6e8 ab dem eigenen Band). y nach unten positiv:
#   y < bu - TOL  -> UEBER dem Boden (schwebt),  y > bu + TOL -> unter der Flaeche (im Block).
TOL = int(sys.argv[2]) if len(sys.argv) > 2 else 100
pl_re = re.compile(r"F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+) hp=(-?\d+) st=(\d+) ps=(\w+) fl=(\d+) gz=(\d+) bo=(-?\d+) bu=(-?\d+)\)")
sp_re = re.compile(r"\[(\d+) t=(\w+) st=(\d+) s=(\d+)\.(\d+)\.(\d+) g=(\w+) fl=(\d+) m=(\d+) gy=(-?\d+) bo=(-?\d+) bu=(-?\d+) mo=(-?\d+) af=(\d+) @\((-?\d+),(-?\d+),(-?\d+)\)")
stat = collections.defaultdict(lambda: [0,0,0,0,0])  # typ -> [bilder, schwebt, im_block, auf, sprung(m222=1)]
pl_n = pl_bad = 0
first_float = {}
slots = collections.defaultdict(lambda: [0,0])
room = None; hp_first = hp_last = None; hits = 0; prev_hp = None
for line in open(sys.argv[1], encoding="latin-1"):
    m = pl_re.search(line)
    if not m: continue
    f = int(m.group(1)); room = m.group(2); py = int(m.group(4)); pbu = int(m.group(12)); hp = int(m.group(6))
    if room != (sys.argv[3] if len(sys.argv) > 3 else room): continue
    if hp_first is None: hp_first = hp
    if prev_hp is not None and hp < prev_hp: hits += 1
    prev_hp = hp; hp_last = hp
    pl_n += 1
    if abs(py - pbu) > TOL: pl_bad += 1
    for s in sp_re.finditer(line):
        slot, t = int(s.group(1)), s.group(2)
        y, bu, m222 = int(s.group(16)), int(s.group(12)), int(s.group(9))
        stat[t][0] += 1; slots[(t,slot)][0] += 1
        if y < bu - TOL:
            if m222 == 1: stat[t][4] += 1
            else:
                stat[t][1] += 1; slots[(t,slot)][1] += 1
                if (t,slot) not in first_float:
                    first_float[(t,slot)] = (f, int(s.group(15)), y, int(s.group(17)), bu, int(s.group(3)), s.group(4), m222)
        elif y > bu + TOL: stat[t][2] += 1
        else: stat[t][3] += 1
print("Raum %s: Spieler %d Bilder, |y-bu|>%d in %d; HP %s -> %s, HP-Abzuege %d" % (room, pl_n, TOL, pl_bad, hp_first, hp_last, hits))
for t,v in sorted(stat.items()):
    print("Typ %s: Bilder %d | SCHWEBT (y < bu-%d, ohne Sprung) %d | im Sprung %d | unter der Flaeche %d | auf dem Boden %d" % (t, v[0], TOL, v[1], v[4], v[2], v[3]))
n_sl = collections.Counter(); n_fl = collections.Counter()
for (t,sl),v in slots.items():
    n_sl[t] += 1
    if v[1]: n_fl[t] += 1
for t in sorted(n_sl): print("Typ %s: %d Figuren, davon %d mindestens einmal schwebend" % (t, n_sl[t], n_fl[t]))
for k,v in sorted(first_float.items())[:12]:
    print("  erstes Schweben", k, "F%d x=%d y=%d z=%d bu=%d st=%d s1=%s m222=%d" % v)
