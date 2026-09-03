import struct, os, sys, glob, json
ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"
HDRNAMES = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb",
            "sca","rid","rvd","lit","md1tim_ptr","flr","blk","msg",
            "main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]
rdts = sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT")))
rows = []
for p in rdts:
    room = os.path.basename(p)[:-4]
    b = open(p,'rb').read()
    if len(b) < 0x60: continue
    nCut = b[1]; hdr7 = b[7]
    dirs = {n: struct.unpack_from("<I", b, 8+i*4)[0] for i,n in enumerate(HDRNAMES)}
    lit = dirs['lit']
    pris = [struct.unpack_from("<I", b, 0x60+i*32+0x1c)[0] for i in range(nCut)]
    if not pris: continue
    pmin, pmax = min(pris), max(pris)
    # next section boundary after pmax
    cands = [v for v in dirs.values() if v > pmax and v < len(b)]
    nxt = min(cands) if cands else len(b)
    litend = lit + nCut*40
    # trailing table detection: nCut dwords + 1 dword at the very end of the pri block
    tbl_start = nxt - 4*(nCut+1)
    tbl = [struct.unpack_from("<I", b, tbl_start+4*i)[0] for i in range(nCut+1)] if tbl_start>=0 and nxt<=len(b) else []
    exp_rev = [v - pmin for v in reversed(pris)]
    tbl_ok = (tbl[:nCut] == exp_rev) if len(tbl)==nCut+1 else False
    last = tbl[-1] if tbl else None
    nulls = sum(1 for v in pris if struct.unpack_from("<I", b, v)[0]==0xFFFFFFFF)
    rows.append(dict(room=room, nCut=nCut, hdr7=hdr7, lit=lit, litend=litend, pmin=pmin, pmax=pmax,
                     nxt=nxt, nxt_name=[k for k,v in dirs.items() if v==nxt],
                     lit_ok=(litend==pmin), tbl_ok=tbl_ok, last=last, last_eq_hdr7=(last==hdr7),
                     nulls=nulls, gap=nxt-pmin, need=4*(nCut+1),
                     dirs={k:dirs[k] for k in ("esp","eff","esp_tim","model_tim","rbj")}))
json.dump(rows, open(r"C:\workspace\git\reAi_v2\analysis\esp_masken_2026-09-03\e_sweep.json","w"), indent=0)
bad_lit=[r['room'] for r in rows if not r['lit_ok']]
bad_tbl=[r['room'] for r in rows if not r['tbl_ok']]
bad_last=[r['room'] for r in rows if not r['last_eq_hdr7']]
print("rooms:",len(rows))
print("lit+nCut*40 != pri_min :", len(bad_lit), bad_lit[:20])
print("trailing table mismatch:", len(bad_tbl), bad_tbl[:20])
print("last dword != hdr7     :", len(bad_last), [(r['room'],r['last'],r['hdr7']) for r in rows if not r['last_eq_hdr7']][:20])
