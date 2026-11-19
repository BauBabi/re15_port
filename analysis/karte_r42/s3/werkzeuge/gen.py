# Kampagne Stufe 3: je Figur ein Durchlauf, je Stage Raeume aufsteigend; vor jedem Abzug sind die
# vorher "durchlaufenen" Raeume derselben Stage besucht (besucht:<rid>:<x>:<z>), der Rest bleibt verschleiert.
import collections, os
S = os.path.dirname(os.path.abspath(__file__))
FLAGS = {'4000': ['flag:4:38'], '4001': ['flag:4:20', 'flag:3:145'], '4010': ['flag:4:34'], '4011': ['flag:4:34'],
         '4031': ['flag:4:25'], '6000': ['flag:3:100'], '6001': ['flag:3:100'], '6030': ['flag:4:23'], '6031': ['flag:4:23']}
cams = collections.defaultdict(list)
for l in open(os.path.join(S, 'bestand.txt')):
    t = l.split()
    if t and t[0] == 'CAM': cams[t[1]].append(t)
out = []
for fig, end in (('L', '0'), ('E', '1')):
    for st in '123456':
        rs = sorted(r for r in cams if r[0] == st and r[3] == end)
        vorher = []
        for r in rs:
            uniq = []
            for c in cams[r]:
                k = (c[3], c[4])
                if k not in [(u[3], u[4]) for u in uniq]: uniq.append(c)
            n = 3 if fig == 'L' else 2
            if len(uniq) > n:
                idx = sorted(set(round(i * (len(uniq) - 1) / (n - 1)) for i in range(n)))
                pick = [uniq[i] for i in idx]
            else: pick = uniq
            for i, c in enumerate(pick):
                out.append(' '.join(['%s%s_%d' % (fig, r, i), r, c[3], c[4], c[10], c[11], c[12]] + FLAGS.get(r, []) + vorher))
            vorher.append('besucht:%s:%s:%s' % (r, uniq[0][3], uniq[0][4]))
open(os.path.join(S, 'kampagne.txt'), 'w').write('\n'.join(out) + '\n')
print(len(out))
