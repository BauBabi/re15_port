#!/usr/bin/env python3
"""r30_karte3010_msg.py - Raumtexte eines RDT dekodieren (RE1.5 und RE2).

RE1.5: Textblock = RDT-Kopfwort +0x3C (messageStart, RE15_KNOWLEDGE.md §1.1)
RE2  : Textblock = RDT-Kopfwort +0x40 (Index 14) bzw. +0x3C (Index 13, zweite Sprache)
Block = u16-Offsettabelle (erster Eintrag = Tabellenlaenge), Zeichentabelle aus
src/main/java/de/re15/extractors/MSGParser.java.
Aufruf: r30_karte3010_msg.py <RDT> <Kopf-Offset hex>
"""
import struct, sys
T = {0x00:" ",0x01:".",0x02:">",0x03:"<",0x04:">",0x05:"(",0x06:")",0x07:"#",0x08:"#",
     0x09:'"',0x0A:'"',0x0B:"v",0x16:":",0x17:",",0x18:",",0x19:'"',0x1A:"!",0x1B:"?",
     0x1C:"!?",0x37:"[",0x38:"/",0x39:"]",0x3A:"'",0x3B:"-",0x3C:"."}
for i in range(10): T[0x0C + i] = str(i)
for i in range(26): T[0x1D + i] = chr(65 + i); T[0x3D + i] = chr(97 + i)

def main():
    p = sys.argv[1]; hdr = int(sys.argv[2], 16)
    d = open(p, "rb").read()
    base = struct.unpack_from("<I", d, hdr)[0]
    if base == 0: print("kein Textblock"); return
    n = struct.unpack_from("<H", d, base)[0] // 2
    offs = [struct.unpack_from("<H", d, base + i * 2)[0] for i in range(n)]
    print("%s  Textblock @0x%05X, %d Nachrichten" % (p, base, n))
    for i, o in enumerate(offs):
        a = base + o; out = []; k = a
        while k < len(d) and k < a + 600:
            b = d[k]; k += 1
            if b == 0xFE:
                k += 1; break
            if b == 0xFC: out.append(" / ")
            elif b == 0xFD: out.append("|P|"); 
            elif b in (0xF8, 0xF9, 0xFA, 0xFB, 0xF0, 0xF3, 0xF4):
                out.append("{%02X %02X}" % (b, d[k])); k += 1
            elif b in T: out.append(T[b])
            else: out.append("<%02X>" % b)
        print("  msg %2d @0x%05X: %s" % (i, a, "".join(out)))
main()
