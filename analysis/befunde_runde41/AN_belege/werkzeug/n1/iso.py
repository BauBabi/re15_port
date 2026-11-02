"""iso.py BIN [ls PATH | get PATH OUT [raw]] - ISO9660 aus MODE2/2352-Abbild lesen (nur lesen)."""
import sys, struct
BIN = sys.argv[1]
f = open(BIN, "rb")
def sec(lba, raw=False):
    f.seek(lba * 2352)
    s = f.read(2352)
    return s if raw else s[24:24+2048]
def dirents(lba, size):
    out = []
    for k in range((size + 2047) // 2048):
        b = sec(lba + k); p = 0
        while p < 2048:
            ln = b[p]
            if ln == 0: break
            elba = struct.unpack_from("<I", b, p+2)[0]; esz = struct.unpack_from("<I", b, p+10)[0]
            fl = b[p+25]; nl = b[p+32]; nm = b[p+33:p+33+nl].decode("latin1")
            out.append((nm, elba, esz, fl))
            p += ln
    return out
pvd = sec(16)
root = pvd[156:156+34]
rl = struct.unpack_from("<I", root, 2)[0]; rs = struct.unpack_from("<I", root, 10)[0]
def find(path):
    lba, size, fl = rl, rs, 2
    for part in [x for x in path.strip("/").split("/") if x]:
        for nm, l, s, f2 in dirents(lba, size):
            if nm.split(";")[0].upper() == part.upper():
                lba, size, fl = l, s, f2; break
        else:
            raise SystemExit("nicht gefunden: " + part)
    return lba, size, fl
cmd = sys.argv[2]
if cmd == "ls":
    lba, size, fl = find(sys.argv[3] if len(sys.argv) > 3 else "/")
    for nm, l, s, f2 in dirents(lba, size):
        print("%-24s lba %7d size %10d %s" % (nm, l, s, "DIR" if f2 & 2 else ""))
elif cmd == "get":
    lba, size, fl = find(sys.argv[3]); out = open(sys.argv[4], "wb")
    raw = len(sys.argv) > 5 and sys.argv[5] == "raw"
    if raw:
        # Form-2/XA: 2336-Byte (Subheader+Daten+EDC) je Sektor; Groesse im Verzeichnis = n*2048
        n = (size + 2047) // 2048
        for k in range(n): out.write(sec(lba + k, True)[16:16+2336])
    else:
        rem = size; k = 0
        while rem > 0:
            b = sec(lba + k); out.write(b[:min(2048, rem)]); rem -= 2048; k += 1
    print("ok", sys.argv[3], "lba", lba, "size", size)
