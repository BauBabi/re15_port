import re, sys
p = sys.argv[1]
rx_f = re.compile(r'^F(\d+) pad=([0-9a-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) ps2=(\d+) mo=(\d+) ac=(-?\d+) fx=(\d+) mg=(-?\d+) .*? pm=(\d+)')
rx_a = re.compile(r'\[(\d+) t=36 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+)[^\]]*\] hp=(-?\d+)')
erstes = None; letztes = None; n36 = 0; pm_nicht2 = 0; pad_an = 0; pad_an_36 = 0
hp36 = set(); mg_werte = set(); pl_hp_min = 999; g_werte = []; ss2_max = -1
aim_clips = set(); pl_states = set()
for line in open(p, encoding='utf-8', errors='replace'):
    m = rx_f.match(line)
    if not m:
        continue
    f = int(m.group(1)); pad = int(m.group(2), 16); pm = int(m.group(14))
    a = rx_a.search(line)
    if pad: pad_an += 1
    if not a:
        continue
    n36 += 1
    if erstes is None: erstes = f
    letztes = f
    if pm != 2: pm_nicht2 += 1
    if pad: pad_an_36 += 1
    hp36.add(int(a.group(9))); mg_werte.add(int(m.group(13)))
    pl_hp_min = min(pl_hp_min, int(m.group(6)))
    g = a.group(6)
    if not g_werte or g_werte[-1][1] != g: g_werte.append((f, g))
    ss2_max = max(ss2_max, int(a.group(4)))
    aim_clips.add(int(m.group(11))); pl_states.add((int(m.group(7)), int(m.group(8))))
print('0x36 im Log: Bild %s bis %s (%d Bilder)' % (erstes, letztes, n36))
print('Bilder mit Pad != 0 gesamt %d, davon mit 0x36 %d' % (pad_an, pad_an_36))
print('Bilder mit 0x36 und player_mode != 2: %d' % pm_nicht2)
print('0x36 HP-Werte: %s' % sorted(hp36))
print('Granaten im Magazin (mg) waehrend 0x36: %s' % sorted(mg_werte))
print('Spieler-HP min: %d' % pl_hp_min)
print('grid-Wechsel: %s' % g_werte)
print('Szenenphase ss2 max: %d' % ss2_max)
print('Spieler (pst,ps1) waehrend 0x36: %s, Zielclip ac: %s' % (sorted(pl_states), sorted(aim_clips)))
