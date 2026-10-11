import re,sys
pat=re.compile(r"EINS seed 0x([0-9a-f]+) modus (\d) blockiert (-?\d+) griffe (\d+) nah (\d+) fern (\d+)")
for fn in sys.argv[1:]:
    v=[tuple(int(x) for x in m.groups()[2:]) for m in (pat.search(l) for l in open(fn)) if m]
    print("%-22s n=%d blockiert %d griffe %d nah %d fern %d"%(fn,len(v),sum(a[0] for a in v),sum(a[1] for a in v),sum(a[2] for a in v),sum(a[3] for a in v)))
