"""puls_sim.py - RE2 Blob 3 (Vorkampf-Puls @0x801044FC) nachgerechnet, Bild 0 = [T0]-Takt (blob 3, ph 0 @0x80101208/@0x80101210)."""
import struct, sys
B = open(sys.argv[1], "rb").read()
TAB = struct.unpack_from("<110h", B, 0x5ad4)          # @0x80105ad4 (Link 0x80100000), 8000 - 24*t @0x801047ec-808
def s16(v): return ((v + 0x8000) & 0xffff) - 0x8000
st = dict(ph=0, w0=0, w3=0, sp=0, f=0, t=0, i=0)
def tick(st, rnd=28):
    ph = st['ph']
    if ph >= 8: return
    if ph == 0:
        st['ph'] = 1; st['t'] = (rnd & 0x1f) + 30; st['sp'] = 6; st['f'] = 0; ph = 1
    if ph == 1:
        if s16(st['w3']) != 0: st['w3'] = s16(st['w3']) >> 1
        st['w0'] = s16(st['w0'] + st['sp'])
        if st['sp'] < 162 and st['f'] == 0:
            st['sp'] = s16(st['sp'] + 6)
            if st['sp'] >= 162: st['f'] = 1; st['sp'] = 162
        if st['w0'] < 8501: return
        st['sp'] = s16(st['sp'] - 6)
        if st['sp'] >= 0: return
        st['ph'] = 2; st['sp'] = 8; st['f'] = 0; return
    if ph == 2:
        st['w0'] = s16(st['w0'] - st['sp'])
        if st['sp'] < 192 and st['f'] == 0:
            st['sp'] = s16(st['sp'] + 8)
            if st['sp'] >= 192: st['f'] = 1; st['sp'] = 192
        if st['w0'] < -1048:
            st['sp'] = s16(st['sp'] - 8)
            if st['sp'] < 0: st['ph'] = 1; st['sp'] = 6; st['f'] = 0
        if ((st['w0'] & 0xffff) - 1) & 0xffffffff < 0x7ff: st['ph'] = 6
        if st['t'] != 0: st['t'] -= 1
        return
    if ph == 6:
        st['i'] = 0; st['ph'] = 7; ph = 7
    if ph == 7:
        st['w0'] = 8000 - 24 * TAB[st['i']]
        st['i'] += 1
        if st['i'] == 110: st['ph'] = 6
        return
w = []
for b in range(0, 1200):
    tick(st)
    w.append((b, st['ph'], st['w0'], st['sp'], st['t'], st['i']))
RAM = {31: (1, 3078, 162), 80: (1, 10296, 66), 129: (2, 5796, 192), 165: (7, -184, 192)}
for b, (ph, w0, sp) in RAM.items():
    print("Bild %4d RE2 ph %d w0 %6d tempo %3d | Sim ph %d w0 %6d tempo %3d t %d i %d" % ((b, ph, w0, sp) + w[b][1:]))
print("Eintritt ph7:", next(x[0] for x in w if x[1] == 7))
ram_w0 = {"s07": -4000, "s08": 9440, "s09": -4168, "s10": 8000, "s11": -3928, "s12": -2200, "s13": 8000, "s14": -4216, "s15": 7520,
          "s16": -3904, "s17": 3152, "s18": -3928, "s19": -880, "s20": 9200, "s21": -3904, "s22": 6080, "s23": -4192, "s24": 1160, "s25": -3472}
for k, v in ram_w0.items():
    idx = [i for i, t in enumerate(TAB) if 8000 - 24 * t == v]
    print(k, v, "Tabellenindex", idx)
mn = min(x[2] for x in w[:1162]); mx = max(x[2] for x in w[:1162])
print("Spanne Bild 0..1161: %d .. %d" % (mn, mx))
for b in (165, 264, 313, 479, 677):
    print("Bild", b, w[b])
