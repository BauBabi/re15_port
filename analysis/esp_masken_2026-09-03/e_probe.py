import struct, os, sys, glob, json

ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"

HDRNAMES = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb",
            "sca","rid","rvd","lit","md1tim_ptr","flr","blk","msg",
            "main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]

def find_rdt(room):
    st = room[4]
    pats = [os.path.join(ROOT, "STAGE%s"%st, room+".RDT")]
    for p in pats:
        if os.path.exists(p): return p
    hits = glob.glob(os.path.join(ROOT,"**",room+".RDT"), recursive=True)
    return hits[0] if hits else None

def load(room):
    p = find_rdt(room)
    return p, open(p,'rb').read()

def hdr(b):
    d = {}
    d['sprite']=b[0]; d['nCut']=b[1]; d['nOmodel']=b[2]; d['items']=b[3]
    d['door']=b[4]; d['room_at']=b[5]; d['reverb']=b[6]; d['sprite_max']=b[7]
    d['dirs'] = {}
    for i,n in enumerate(HDRNAMES):
        off = 8+i*4
        d['dirs'][n] = (off, struct.unpack_from("<I", b, off)[0])
    return d

def cuts(b, n):
    out=[]
    for i in range(n):
        base = 0x60 + i*32
        rec = b[base:base+32]
        pri = struct.unpack_from("<I", rec, 0x1c)[0]
        out.append((i, base, rec, pri))
    return out

if __name__ == "__main__":
    for room in sys.argv[1:]:
        p,b = load(room)
        h = hdr(b)
        print("="*100)
        print("%s  %s  size=%d (0x%X)" % (room, p, len(b), len(b)))
        print("  hdr[0..7] = %s   nCut=%d sprite_max(hdr7)=%d" % (" ".join("%02X"%x for x in b[0:8]), h['nCut'], h['sprite_max']))
        for n,(off,v) in h['dirs'].items():
            print("    +0x%02X %-12s = 0x%08X (%d)%s" % (off, n, v, v, "  <== 0/NULL" if v in (0,0xFFFFFFFF) else ""))
        for (i,base,rec,pri) in cuts(b, h['nCut']):
            note=""
            if pri == 0xFFFFFFFF: note = "  pri_offset ITSELF = FFFFFFFF"
            elif pri < len(b):
                tgt = b[pri:pri+16]
                note = "  -> @0x%06X: %s" % (pri, " ".join("%02X"%x for x in tgt))
            else:
                note = "  -> OUT OF FILE"
            print("  cut %2d @0x%04X pri=0x%08X%s" % (i, base+0x1c, pri, note))
            print("        rec: %s" % " ".join("%02X"%x for x in rec))
