"""claire_hp_tor.py - Abnahme 4: steht das RE2-hp-Tor der Schusswaffen (Leon FUN_800410CC: lbu v0,467(s2) / bne / lh v0,342(s2) /
bltz v0 @0x80041270-88) auch im Claire-EXE SLES_109.72? Sucht die Wortfolge [924201d3 00000000 1440xxxx 00000000 86420156 00000000 0440xxxx]."""
import struct, sys
for name, path in (("Leon PSX.EXE", sys.argv[1]), ("Claire SLES_109.72", sys.argv[2])):
    b = open(path, "rb").read(); t = struct.unpack_from("<I", b, 0x18)[0]
    W = [struct.unpack_from("<I", b, 0x800 + 4*i)[0] for i in range((len(b) - 0x800)//4)]
    for i in range(len(W) - 7):
        if W[i] == 0x924201d3 and W[i+1] == 0 and (W[i+2] >> 16) == 0x1440 and W[i+3] == 0 and W[i+4] == 0x86420156 \
           and W[i+5] == 0 and (W[i+6] >> 16) == 0x0440:
            a = t + 4*i
            def tgt(k): off = W[i+k] & 0xffff; off = off - 0x10000 if off & 0x8000 else off; return a + 4*k + 4 + 4*off
            print("%s: lbu v0,467(s2) @0x%08x, bne -> 0x%08x, lh v0,342(s2) @0x%08x, bltz v0 @0x%08x -> 0x%08x"
                  % (name, a, tgt(2), a + 16, a + 24, tgt(6)))
