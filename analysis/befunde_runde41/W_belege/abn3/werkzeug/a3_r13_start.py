# Abnahme 3 Spur W (Nebenbefund zu OFFEN 5): In welchen RDTs steht Sce_espr_on Effekt 6 Sub 0 (3a xx 06 00) in einem Programm,
# und wird dieses Programm gestartet (main00/sub00/sub01 automatisch, Evt_exec 04 xx 18 NN, Gosub 18 NN, AOT sce 3 mit Daten .. 18 NN)?
import sys, os, glob, struct
WURZELN = sys.argv[1:]
sys.argv = [sys.argv[0]]
src = open(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0] if not globals().get('__file__') else __file__)), 'a3_scd.py')).read().split('def main():')[0]
exec(src)
def analyse(f):
    d = open(f, 'rb').read()
    if len(d) < 0x60: return None
    try:
        ms, ss, nxt, pl = tables(d)
    except Exception:
        return None
    if not (0 < ms < len(d) and 0 < ss < len(d)): return None
    starts = sorted(set([o for _, o in pl] + [ss])) + [nxt]
    tropfen = {}; gestartet = {'main00': 'auto', 'sub00': 'auto', 'sub01': 'auto'}
    for name, o in pl:
        ende = [x for x in starts if x > o]
        e = min(ende) if ende else nxt
        if e <= o or e > len(d): continue
        for pc, op, b, nm in walk(d, o, e):
            if op == 0x3A and len(b) >= 4 and b[2] == 6 and b[3] == 0:
                tropfen.setdefault(name, []).append(pc)
            if op == 0x04 and len(b) == 4: gestartet.setdefault('sub%02d' % b[3], 'Evt_exec %s @0x%05X' % (name, pc))
            if op == 0x18 and len(b) == 2: gestartet.setdefault('sub%02d' % b[1], 'Gosub %s @0x%05X' % (name, pc))
            if op == 0x2C and len(b) == 20 and b[2] == 3: gestartet.setdefault('sub%02d' % b[17], 'AOT sce3 %s @0x%05X' % (name, pc))
            if op == 0x46 and len(b) == 10 and b[2] == 3: gestartet.setdefault('sub%02d' % b[7], 'Aot_reset sce3 %s @0x%05X' % (name, pc))
    return tropfen, gestartet
if __name__ == '__main__':
    import glob
    for wurzel in WURZELN:
        print('=====', wurzel)
        for f in sorted(glob.glob(wurzel + '/STAGE*/ROOM*.RDT')):
            r = analyse(f)
            if not r: continue
            tropfen, gest = r
            if not tropfen: continue
            teile = ['%s (%dx ab @0x%05X) -> %s' % (prog, len(orte), orte[0], gest.get(prog, 'NIE gestartet')) for prog, orte in tropfen.items()]
            print('  %s: %s' % (os.path.basename(f), '; '.join(teile)))
