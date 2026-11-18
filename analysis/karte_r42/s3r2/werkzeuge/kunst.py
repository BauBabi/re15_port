# Gemeinsame Helfer Stufe 3 Runde 2: gemalte Flaeche je (Blatt, Rechteck) aus der ORIGINAL-Kunst (DATA/MAPxx.PIX,
# Seitentabelle @0x80076840) und die Zonen-Tabelle des Ports (re15_map_zones.h).
import io, os, re
WT = r'C:\workspace\git\reAi_v2\.claude\worktrees\r42_karte'
T = os.path.join(WT, 're15_port', 'tools')
_s = io.open(os.path.join(T, 'gen_map_zones.py'), encoding='utf-8').read()
G = {'__name__': 'kunst', '__file__': os.path.join(T, 'gen_map_zones.py')}
exec(compile(_s[:_s.index('def main(')], 'gen', 'exec'), G)
flaeche = G['_kunst_flaeche']; rects = G['rects']

def zonen(kopf=None):
    kopf = kopf or os.path.join(WT, 're15_port', 'engine', 'src', 're15_map_zones.h')
    t = io.open(kopf, encoding='utf-8').read()
    i = t.index('s_map_zones[] = {'); j = t.index('\n};', i)
    out = []
    for z in t[i:j].split('\n'):
        if not z.strip().startswith('{'): continue
        m = [int(x, 0) for x in re.findall(r'0x[0-9A-Fa-f]+|-?\d+', z.split('/*')[0])]
        if len(m) >= 17:
            m = m + [0] * (18 - len(m))
            out.append(dict(room=m[0], box=tuple(m[1:5]), page=m[5], rect=m[6], idx=m[7], zid=m[8], etage=m[16]))
    return out

def abstand(menge, x, y, maxd=6):
    """Abstand (Chebyshev) von (x,y) zur Menge gemalter Punkte, None wenn > maxd."""
    for d in range(0, maxd + 1):
        for dy in range(-d, d + 1):
            for dx in range(-d, d + 1):
                if max(abs(dx), abs(dy)) == d and (x + dx, y + dy) in menge: return d
    return None
