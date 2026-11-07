# Kurzzeilen aus kam.log: je Bild Wahl und je Kamera nah/f/in/p/df/dp/gut.
#   python kz.py <kam.log> <wer> [von] [bis]
import sys, re
von = int(sys.argv[3]) if len(sys.argv) > 3 else 0
bis = int(sys.argv[4]) if len(sys.argv) > 4 else 10**9
for line in open(sys.argv[1]):
    m = re.match(r'F(\d+) (\w+) wahl=(\d+) form=\((-?\d+),(-?\d+)\) pl=\((-?\d+),(-?\d+)\)', line)
    if not m or m.group(2) != sys.argv[2]: continue
    f = int(m.group(1))
    if f < von or f > bis: continue
    out = 'F%d w%s fx%s px%s |' % (f, m.group(3), m.group(4), m.group(6))
    for c in re.finditer(r'c(\d+) ok=\d nah=(\d) hi=(\d+) f=(\d+) in=(\d+) mi=(\d) p=(\d+) pi=(\d) df=(\d+) dp=(\d+) sz=(\d) kp=(\d)', line):
        out += ' c%s:%s%s f%s i%s p%s df%s dp%s %s' % (c.group(1), 'N' if c.group(2) == '1' else '', 'H' + c.group(3) if c.group(3) != '0' else '',
                                                   c.group(4), c.group(5), c.group(7), c.group(9), c.group(10),
                                                   ('S' if c.group(11) == '1' else '-') + ('K' if c.group(12) == '1' else '-'))
    print(out)
