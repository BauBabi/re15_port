import struct, os, sys, glob, json
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sld import find_sld, sld_decompress

BSS = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\BSS"

def probe(path):
    d = open(path,'rb').read()
    assert len(d) == 0x10000, (path, len(d))
    hdr = struct.unpack_from("<4H", d, 0)
    hits = find_sld(d)
    out = {"file": path, "hdr": hdr, "sld": hits, "trailers": []}
    # search for P (4-aligned) with u32@(P-8) == sld_off
    for (off, dsz) in hits:
        cands = []
        for P in range(8, 0x10001, 4):
            v = struct.unpack_from("<I", d, P-8)[0]
            if v == off:
                f = struct.unpack_from("<I", d, P-4)[0]
                cands.append((P, f))
        out["trailers"].append({"sld_off": off, "dsz": dsz, "cands": cands})
    return out

if __name__ == "__main__":
    rooms = sys.argv[1:] if len(sys.argv)>1 else ["ROOM1000"]
    for r in rooms:
        for f in sorted(glob.glob(os.path.join(BSS, r, "BG*.BSS"))):
            o = probe(f)
            print(os.path.basename(os.path.dirname(f)), os.path.basename(f),
                  "hdr=%04x %04x %04x %04x" % o["hdr"],
                  "sld=", o["sld"], "trailers=", o["trailers"])
