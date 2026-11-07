# Abnahme 5: RE2-Assets der NB5/NB6 gegen ROOM7040.RDT (eigene Ausschnitte, nicht der Erzeuger)
import struct, hashlib
W = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration"
a = open(W + r"/info/re2leon/PL0/RDT/ROOM7040.RDT", 'rb').read()
RE2 = W + r"/re15_port/shared_assets/RE2/"
def f(n): return open(RE2 + n, 'rb').read()
offs = struct.unpack_from('<23I', a, 8)
print('Objektmodell 3/4:', struct.unpack_from('<II', a, 0x244 + 24), struct.unpack_from('<II', a, 0x244 + 32), 'Modell 5 MD1', hex(struct.unpack_from('<II', a, 0x244 + 40)[1]))
for name, von, bis in [('R704_LAMPE.MD1', 0x28A4, 0x3080), ('R704_LAMPE.TIM', 0x33844, 0x33844 + 34336), ('R704.ESP', 0x35D8, 0x36FC), ('GATOR40A0_19.TIM', 0x18984, 0x1B1C4)]:
    d = f(name)
    print('%-18s %6d B  == RDT[0x%05X:0x%05X] %s  sha256 %s' % (name, len(d), von, bis, d == a[von:bis], hashlib.sha256(d).hexdigest()[:16]))
print('RDT-Laenge', len(a), 'TIM-Ende', 0x33844 + 34336)
print('Tafelwort vor RDT[21] (0x%X-4):' % offs[21], struct.unpack_from('<I', a, offs[21] - 4)[0], ' R704.ESP[288..292] =', struct.unpack_from('<I', f('R704.ESP'), 288)[0])
# Beben-Ton: EDT-Satz 12, VAG 17
edt, vh, vb = offs[0], offs[1], offs[2]
print('EDT[12] =', a[edt + 48: edt + 52].hex(' '), ' VH-Magie', a[vh:vh + 4])
nprog = struct.unpack_from('<H', a, vh + 0x12)[0]; sizetab = vh + 0x820 + nprog * 0x200
raw = a[edt + 48: edt + 52]; prog = raw[1] & 0x7F; tone = raw[2] >> 4; lagen = (raw[3] >> 5) + 1
print('prog', prog, 'tone', tone, 'lagen', lagen)
vags = []
for k in range(lagen):
    t = a[vh + 0x820 + prog * 0x200 + (tone + k) * 0x20:][:32]
    vags.append(struct.unpack_from('<H', t, 0x16)[0])
print('VAG-Nummern der Lagen', vags)
vbs = f('R704_BEBEN.VBS')
for vi in sorted(set(vags)):
    pos = vb + sum(struct.unpack_from('<H', a, sizetab + j * 2)[0] << 3 for j in range(vi))
    ln = struct.unpack_from('<H', a, sizetab + vi * 2)[0] << 3
    wav = a[pos:pos + ln]
    print('VAG %d: RDT@0x%X Laenge %d, in R704_BEBEN.VBS ab 3144: %s' % (vi, pos, ln, vbs[3144:3144 + ln] == wav))
print('VBS-Laenge', len(vbs), 'Trailer vh_off', struct.unpack_from('<I', vbs, 3144 - 8)[0])
