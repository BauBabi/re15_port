import struct, sys
ENC = {" ": 0x00, ":": 0x16, ",": 0x18, "\"": 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10): ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i
DEC = {v: k for k, v in ENC.items()}; DEC[0x3C] = "."
def dec(b):
    out=[]; i=0
    while i < len(b):
        x=b[i]
        if x in DEC: out.append(DEC[x]); i+=1
        elif x == 0xFC: out.append(" / "); i+=1
        elif x == 0xFE: out.append("<fe %02x>" % b[i+1] if i+1 < len(b) else "<fe>"); break
        elif x in (0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA):
            out.append("<%02x %02x>" % (x, b[i+1] if i+1<len(b) else 0)); i+=2
        else: out.append("<%02x>" % x); i+=1
    return "".join(out)
path = sys.argv[1]; which = int(sys.argv[2]) if len(sys.argv) > 2 else 14
d = open(path, "rb").read()
offs = struct.unpack_from("<23I", d, 8)
ms = offs[which]
first = struct.unpack_from("<H", d, ms)[0]; n = first // 2
tbl = [struct.unpack_from("<H", d, ms + 2*i)[0] for i in range(n)]
for i, o in enumerate(tbl):
    st = ms + o; en = ms + tbl[i+1] if i + 1 < n else st + 300
    print("  id %2d @0x%05X: %s   [%s]" % (i, st, dec(d[st:en]), d[st:min(en, st+8)].hex(" ")))
