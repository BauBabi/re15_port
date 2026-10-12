# Vergleicht zwei Funktionen Wort fuer Wort (STAGE-A @a, STAGE-B @b, n Worte).
# Gleich = identisches Wort; "rel" = beide Worte sind Spruenge/Verzweigungen mit identischer Kodierung (PC-relativ);
# Abweichungen werden dekodiert ausgegeben (jal-Ziele in den Overlay mit Versatz b-a gelten als gleich).
import struct, sys
binA, a, binB, b, n = sys.argv[1], int(sys.argv[2],16), sys.argv[3], int(sys.argv[4],16), int(sys.argv[5])
dA = open(binA,'rb').read(); dB = open(binB,'rb').read()
BASE = 0x80100000
gleich = 0; ueber = 0; abw = []
for i in range(n):
    wa = struct.unpack_from('<I', dA, a - BASE + i*4)[0]
    wb = struct.unpack_from('<I', dB, b - BASE + i*4)[0]
    if wa == wb:
        gleich += 1; continue
    opa, opb = wa >> 26, wb >> 26
    if opa == 3 and opb == 3:
        ta = (wa & 0x3ffffff) << 2 | 0x80000000; tb = (wb & 0x3ffffff) << 2 | 0x80000000
        if ta >= BASE and tb >= BASE:
            ueber += 1; abw.append((i, wa, wb, f"jal Overlay 0x{ta:08x} <-> 0x{tb:08x} (Versatz {tb-ta:+#x})")); continue
    abw.append((i, wa, wb, "ABWEICHUNG"))
print(f"A {binA.split('/')[-1]} @0x{a:08x}  B {binB.split('/')[-1]} @0x{b:08x}  {n} Worte: identisch {gleich}, jal-Overlay-Versatz {ueber}, Rest {len(abw)-ueber}")
for i, wa, wb, t in abw:
    print(f"  Wort {i:3d} (A 0x{a+i*4:08x} / B 0x{b+i*4:08x}): 0x{wa:08x} vs 0x{wb:08x}  {t}")
