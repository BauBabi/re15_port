# Kampagne Stufe 3 Runde 2: NUR echte Lagen. tN = Ankunft einer Tuer (EIN-Zeile = Door_aot_set next_pos + Yaw),
# wN = von Ankunft N aus 2 s vorwaerts laufen (echte Bewegung mit Kollision). Je Stage Raeume aufsteigend,
# vorher betretene Raeume derselben Stage sind besucht (an ihrer ersten Ankunft), der Rest verschleiert.
import os, collections
S = os.path.dirname(os.path.abspath(__file__))
FLAGS = {'1011': ['flag:4:241'], '1031': ['flag:3:193', 'flag:3:125'], '10F0': ['flag:9:71'], '10F1': ['flag:9:71'],
         '1170': ['flag:3:193', 'flag:3:125'], '11C0': ['flag:4:64'], '3000': ['flag:4:198'], '3001': ['flag:4:198'],
         '3080': ['flag:4:96'], '3081': ['flag:4:96'], '4000': ['flag:4:38'], '4001': ['flag:4:20', 'flag:3:145'],
         '4010': ['flag:4:34'], '4011': ['flag:4:34'], '4031': ['flag:4:25'], '6000': ['flag:3:100'],
         '6001': ['flag:3:100'], '6030': ['flag:4:23'], '6031': ['flag:4:23']}
raeume = []; ein = collections.defaultdict(list)
for l in open(os.path.join(S, 'bestand.txt')):
    t = l.split()
    if t and t[0] == 'ROOM': raeume.append(t[1])
    if t and t[0] == 'EIN': ein[t[1]].append((int(t[4]), int(t[5]), int(t[6]), int(t[7]) & 4095, t[2]))
out = []
for fig, end in (('L', '0'), ('E', '1')):
    for st in '123456':
        vorher = []
        for r in sorted(x for x in raeume if x[0] == st and x[3] == end):
            uniq = []
            for p in ein.get(r, []):
                if all(abs(p[0] - u[0]) + abs(p[2] - u[2]) > 600 for u in uniq): uniq.append(p)
            if not uniq: continue
            fl = FLAGS.get(r, [])
            for i, p in enumerate(uniq):
                out.append(' '.join(['%s%s_t%d' % (fig, r, i), r, str(p[0]), str(p[1]), str(p[2]), str(p[3]), '0', p[4]] + fl + vorher))
            for i, p in enumerate(uniq[:2]):
                out.append(' '.join(['%s%s_w%d' % (fig, r, i), r, str(p[0]), str(p[1]), str(p[2]), str(p[3]), '2', p[4]] + fl + vorher))
            vorher.append('besucht:%s:%d:%d' % (r, uniq[0][0], uniq[0][2]))
open(os.path.join(S, 'kampagne.txt'), 'w').write('\n'.join(out) + '\n')
print(len(out), collections.Counter(o[0] for o in out))
