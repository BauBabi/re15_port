#!/usr/bin/env python3
"""r30_karte3010_core_satz.py - Saetze einer CORE-Bank (EDH+VB) dekodieren.

EDH = EDT-Saetze (4 Byte) + VH ('pBAV'); letztes u32-Paar: [len-8] = Offset des VH.
Satz: Byte1&0x7f = Programm, Byte2>>4 = Ton, Byte2&0x0f = Prio (wie r30_diary_core_baenke.py).
Aufruf: r30_karte3010_core_satz.py <EDH> <VB> <satz> [<satz> ...]
"""
import struct, sys, hashlib
edh = open(sys.argv[1], "rb").read(); vb = open(sys.argv[2], "rb").read()
pbav = struct.unpack_from("<I", edh, len(edh) - 8)[0]
nrec = pbav // 4
vh = edh[pbav:]
assert vh[:4] == b"pBAV"
ver, vabid, fsize, res0, ps, ts, vs = struct.unpack_from("<IIIHHHH", vh, 4)
toneoff = 32 + 16 * 128
vagoff = toneoff + 32 * 16 * ps
vagsz = struct.unpack_from("<256H", vh, vagoff)
sizes = [s << 3 for s in vagsz[1:vs + 1]]
offs = []; o = 0
for s in sizes: offs.append(o); o += s
print("%s: %d Saetze, VH @0x%X (ps=%d ts=%d vs=%d), VB %d B" % (sys.argv[1], nrec, pbav, ps, ts, vs, len(vb)))
for a in sys.argv[3:]:
    i = int(a, 0)
    r = edh[i * 4:i * 4 + 4]
    if r == b"\xff\xff\xff\xff":
        print("  Satz 0x%02X @Datei 0x%X = ff ff ff ff  LEER" % (i, i * 4)); continue
    prog = r[1] & 0x7f; tone = r[2] >> 4
    t = toneoff + prog * 0x200 + tone * 0x20
    tn = vh[t:t + 32]
    vag = struct.unpack_from("<H", tn, 22)[0]
    w = vb[offs[vag - 1]:offs[vag - 1] + sizes[vag - 1]]
    print("  Satz 0x%02X @Datei 0x%X = %s -> Programm %d Ton %d; Tone @VH+0x%X vol=%d pan=%d center=%d; VAG %d @VB 0x%X %d B sha1 %s" % (
        i, i * 4, r.hex(" "), prog, tone, t, tn[2], tn[3], tn[4], vag, offs[vag - 1], sizes[vag - 1], hashlib.sha1(w).hexdigest()))
