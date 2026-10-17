import sys, glob, struct, os, importlib.util
spec = importlib.util.spec_from_file_location("esp_ids", os.path.join(os.path.dirname(os.path.abspath(__file__)), "esp_ids.py"))
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "esp_ids.py")).read().replace("main()\n", "")
ns = {}
exec(src, ns)
parse, rows, u32 = ns['parse'], ns['rows'], ns['u32']
base = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "re15_port", "shared_assets", "PSX")
files = sorted(glob.glob(base + "/STAGE*/ROOM*.RDT")) + sorted(glob.glob(base + "/**/*.ESP", recursive=True))
hits = {}
for f in files:
    raw = open(f, 'rb').read()
    try:
        if f.upper().endswith('.ESP'):
            pe = ((len(raw) + 3) & ~3) - 4
            effs = parse(raw, 0, pe, True)
        else:
            effs = parse(raw, u32(raw, 0x4C), u32(raw, 0x50))
    except Exception as ex:
        continue
    for e in effs:
        for sub in range(8):
            try:
                n, st = rows(raw, e, sub)
            except Exception:
                continue
            if n <= 0 or n > 64: continue
            for (p, nr, rr) in st:
                if nr > 64: continue
                for r in rr:
                    if len(r) < 2: continue
                    A = struct.unpack_from('<h', r, 0)[0]
                    if A in (24, 25):
                        key = (os.path.basename(f), e['id'], sub)
                        hits.setdefault(key, set()).add(A)
ids = {}
for (f, eid, sub), As in sorted(hits.items()):
    ids.setdefault(eid, set()).add(f)
for eid, fs in sorted(ids.items()):
    print("Effekt 0x%02x mit A 24/25 in %d Dateien: %s" % (eid, len(fs), " ".join(sorted(fs))[:400]))
