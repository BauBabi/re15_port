"""Modell des Port-Intros (enemy_ai_boss_g5.c g5_intro_tick) MIT g5_alle.diff, Keyframes aus dem RE2-RAM.
Ausgabe je Szenenbild: ph nach dem Takt, x, gezeichnetes (Clip, Bild), Wurzel-y, w1. Gegenprobe zu RE2-RAM s05..s26.
Modellrechnung, keine Messung: nach dem Bau mit RE15_G5A_SPUR (test_r41_g5auftritt) nachmessen."""
import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
s = SS(SN % 5)
EDD, EMR = 0x80125d94, 0x80127304
CL = {}
for c in range(11):
    w = s.u32(EDD + 4 * c); n, off = w & 0xffff, w >> 16
    kfs = [s.u32(EDD + off + 4 * f) & 0xfff for f in range(n)]
    CL[c] = [(s.s16(EMR + 0x20 + 24 * k + 2), s.s16(EMR + 0x20 + 24 * k + 6)) for k in kfs]   # (wurzel_y, speed_x)

st = dict(ph=0, timer=0, clip=0, fr=0, x=-9000, prev=0, w1=0, zb=(0, 0), sub=2)
basis = CL[0][0][0]
log = []

def anim():
    st['zb'] = (st['clip'], st['fr'])
    if st['fr'] + 1 >= len(CL[st['clip']]):
        st['fr'] = 0; return 1
    st['fr'] += 1; return 0

def root():
    sp = CL[st['clip']][st['fr']][1]
    if st['fr'] == 0: st['prev'] = 0
    st['x'] += sp - st['prev']; st['prev'] = sp

def setclip(c):
    if st['clip'] != c: st['clip'] = c; st['fr'] = 0

def tick():
    ph = st['ph']
    if ph == 0:
        st['w1'] = 0; setclip(1); st['ph'] = 1; st['timer'] = 0; return
    if ph == 1:
        st['timer'] += 1
        if st['timer'] >= 90: st['ph'] = 2
        return
    if ph == 2:
        root(); st['w1'] = ((CL[1][st['fr']][0] - basis) * 4415) >> 11
        if anim(): st['ph'] = 3
        return
    if ph == 3:
        setclip(3); st['ph'] = 4; st['timer'] = 0; ph = 4
    if ph == 4:
        anim(); st['timer'] += 1
        if st['timer'] >= 90: st['ph'] = 5; st['timer'] = 0
        return
    if ph == 5:
        anim(); st['timer'] += 1
        if st['timer'] >= 110: setclip(4); st['ph'] = 6
        return
    if ph == 6:
        root(); st['w1'] = ((CL[4][st['fr']][0] - basis) * 8830) >> 12
        if anim(): st['ph'] = 7
        return
    if ph == 7:
        setclip(2); st['w1'] = 0; st['ph'] = 8; ph = 8
    if ph == 8:
        if anim(): st['ph'] = 9
        return
    if ph == 9:
        setclip(0); st['ph'] = 10; st['timer'] = 0; ph = 10
    if ph == 10:
        anim(); st['timer'] += 1
        if st['timer'] >= 110: st['ph'] = 11
        return
    if ph == 11:
        setclip(2); st['ph'] = 12; ph = 12
    if ph == 12:
        if anim(): st['ph'] = 13
        return
    if ph == 13:
        setclip(0); st['ph'] = 14; st['timer'] = 60; ph = 14
    if ph == 14:
        anim(); alt = st['timer']; st['timer'] -= 1
        if alt == 0: st['sub'] = 1; st['ph'] = 0

for b in range(0, 1130):
    if st['sub'] != 2: break
    tick()
    c, f = st['zb']
    log.append((b, st['ph'], st['x'], c, f, CL[c][f][0], st['w1'], st['fr'], st['timer']))

def zeige(bs):
    for b in bs:
        r = log[b]
        print("  Bild %4d ph %2d x %6d  gezeichnet Clip %d Bild %3d Wurzel-y %5d  w1 %5d  (+0x14D %3d, timer %d)" % r)
print("Uebergang [T2]->[T3] und [T6]->[T7]:")
zeige(range(268, 274)); zeige(range(648, 654))
print("Gegenprobe RE2 (Takt mit +0x14D wie im Savestate):")
for b, sn in ((129, 5), (165, 6), (215, 7), (264, 8), (479, 13), (529, 14), (578, 15), (628, 16)):
    t = SS(SN % sn); g5 = t.u32(0x800cfe1c)
    r = log[b]
    print("  Bild %d: Modell x %d +0x14D %d w1 %d | RE2 s%02d x %d +0x14D %d w1 %d" % (
        b, r[2], r[7], r[6], sn, t.s32(g5 + 0x38), t.u8(g5 + 0x14d), t.s16(t.u32(t.u32(g5 + 0x198) + 0x1e0) + 0x10)))
last = log[-1]
print("Intro-Ende: Sub 1 gesetzt in Bild %d, x %d" % (last[0], last[2]))
