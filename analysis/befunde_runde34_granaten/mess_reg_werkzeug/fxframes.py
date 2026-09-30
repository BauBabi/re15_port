# FX-Log in Bilder zerlegen. Integration: Feld F=. master: Anker-Effekt (erste Zeile mit Praefix
# ANKER je Bild, Default 'id=4 sub=0') trennt die Bilder; Bild = START + k.
import re, sys
def parse(path, anker=None, start=None):
    L = open(path, encoding='latin1').read().splitlines()
    out = {}  # frame -> list of dict
    k = -1; cur = None
    for i, l in enumerate(L):
        if not l.startswith('id='):
            continue
        d = {}
        m = re.match(r'id=(\d+) sub=(\d+) eidx=(-?\d+) frame=(\d+) w\(([-\d]+),([-\d]+),([-\d]+)\) phys=(\d) xlat=\(([-\d]+),([-\d]+),([-\d]+)\)', l)
        if not m: continue
        g = [int(v) for v in m.groups()]
        d.update(id=g[0], sub=g[1], eidx=g[2], fr=g[3], w=g[4:7], xlat=g[8:11])
        mf = re.search(r' F=(\d+)', l)
        mw = re.search(r'wpos=\(([-\d]+),([-\d]+),([-\d]+)\)', l)
        if mw: d['wpos'] = [int(v) for v in mw.groups()]
        mfl = re.search(r' fl=(\w+)', l)
        if mfl: d['fl'] = mfl.group(1)
        nxt = L[i+1] if i + 1 < len(L) else ''
        mm = re.match(r'\s+-> sx=(-?\d+) sy=(-?\d+) S=(\d+) n=(\d+) c0=(\d+) w16=(-?\d+)', nxt)
        if mm:
            gg = [int(v) for v in mm.groups()]
            d.update(sx=gg[0], sy=gg[1], S=gg[2], n=gg[3], c0=gg[4], w16=gg[5], drawn=1)
        else:
            d['drawn'] = 0
        mq = re.search(r' q=(\d+)', l)
        q = int(mq.group(1)) if mq else 0
        if mf:
            fr = int(mf.group(1))
        elif anker == 'q':
            # Bildgrenze: Tri-Zaehler q faellt (jede Zeile eines Bilds hat q >= Vorzeile)
            if cur is None or q < cur:
                k += 1
            cur = q
            fr = start + k
        else:
            if l.startswith(anker):
                k += 1
            fr = start + k  # Zeilen VOR dem Anker (kleinerer Platz) gehoeren zum Bild des NAECHSTEN Ankers: wahres Bild = fr + 1
        if fr is None: continue
        out.setdefault(fr, []).append(d)
    return out
