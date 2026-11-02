import re, sys
p = sys.argv[1]
bis = int(sys.argv[2]) if len(sys.argv) > 2 else 10**9
schritt = int(sys.argv[3]) if len(sys.argv) > 3 else 30
rx_f = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? mo=(\d+) .*? mg=(-?\d+) ')
rx_a = re.compile(r'\[(\d+) t=(3[06]) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+)[^\]]*\] hp=(-?\d+)')
last = None
for line in open(p, encoding='utf-8', errors='replace'):
    m = rx_f.match(line)
    if not m:
        continue
    f = int(m.group(1))
    if f > bis:
        break
    a = rx_a.search(line)
    key = None
    if a:
        key = (a.group(2), a.group(3), a.group(4), a.group(8))
    pl_hp = int(m.group(5))
    zeigen = (f % schritt == 0) or (key != last)
    if zeigen:
        s = 'F%d PL(%s,%s) hp=%d mo=%s mg=%s' % (f, m.group(2), m.group(3), pl_hp, m.group(6), m.group(7))
        if a:
            s += ' | slot %s t=%s st=%s ss1=%s ss2=%s ss3=%s g=%s mo=%s af=%s hp=%s' % a.groups()
        print(s)
    last = key
