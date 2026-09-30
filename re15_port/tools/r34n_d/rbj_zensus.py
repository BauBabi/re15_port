#!/usr/bin/env python3
"""rbj_zensus.py - Spur D (Runde 34 Nacht): Raum-Animationsblock (RDT+0x5C, "RBJ") aller Raeume.

Warum: Leons Szenen-Gesten (SCD Plc_motion, Work_set(1,0)) spielen aus dem RBJ des RAUMS
(Record 0 = Spieler-Overlay, Port: re15_apply_room_cinematic -> re15_emd_parse_rbj(rec 0);
Original: Marker-Binder FUN_8001b3f8, Bit 0 = Spieler). Eine Geste aus "anderen Zwischensequenzen"
ist nur dann in ROOM1050 spielbar, wenn ihr INHALT im ROOM1050-RBJ liegt. Dieses Werkzeug
vergleicht die Clips inhaltlich (Folge der Keyframe-Bytes je Bild), nicht nur ueber die Nummer.

Format (identisch zu engine/src/emd_common.c re15_emd_parse_rbj_record / re15_emd_parse_animation):
  +0 u32 total_length, +4 u32 record_count; Trailer @total_length: je Record (u32 emr_prefix, u32 edd_off)
  Marker u32 @emr_prefix; EMR-Kopf @emr_prefix+4: o_arm, o_frm, count, ksize (u16)
  Keyframes @emr_prefix+4+o_frm .. edd_off (je ksize); EDD @edd_off: u16 count0, u16 offset0,
  offset0/4 Clips (u16 count, u16 rsv), danach u32-Bildeintraege (low 12 Bit = Keyframe).

Aufruf:
  rbj_zensus.py liste  <RDT...>               # je Raum/Record: Marker, Clipzahl, Bildzahlen
  rbj_zensus.py suche  <RDT> <rec> <clip> <RDT...>   # wo liegt derselbe Clip-INHALT?
  rbj_zensus.py json   <out.json> <RDT...>      # alles maschinenlesbar
"""
import hashlib, json, os, struct, sys


def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]


def rbj_block(d):
    """(start, size) des Animationsblocks oder None."""
    if len(d) < 0x60:
        return None
    s = u32(d, 0x5C)
    if s == 0 or s + 8 > len(d):
        return None
    total = u32(d, s)
    nrec = u32(d, s + 4)
    if total == 0 or nrec == 0 or nrec > 8 or s + total + nrec * 8 > len(d):
        return None
    return s, total + nrec * 8


def records(d):
    blk = rbj_block(d)
    if not blk:
        return []
    s, size = blk
    r = d[s:s + size]
    total, nrec = u32(r, 0), u32(r, 4)
    out = []
    for i in range(nrec):
        pre, edd = u32(r, total + i * 8), u32(r, total + i * 8 + 4)
        if pre == 0 or pre + 12 > len(r) or edd == 0 or edd >= len(r):
            out.append({"rec": i, "fehler": "trailer"}); continue
        marker = u32(r, pre)
        o_arm, o_frm, count, ksize = struct.unpack_from("<4H", r, pre + 4)
        kf_off = pre + 4 + o_frm
        nkf = (edd - kf_off) // ksize if ksize else 0
        kfs = [r[kf_off + k * ksize: kf_off + (k + 1) * ksize] for k in range(max(nkf, 0))]
        count0, off0 = u16(r, edd), u16(r, edd + 2)
        ncl = off0 // 4
        counts = [count0] + [u16(r, edd + 4 * c) for c in range(1, ncl)]
        clips, first = [], 0
        for c, n in enumerate(counts):
            ents = [u32(r, edd + off0 + 4 * (first + f)) for f in range(n)]
            h = hashlib.sha1()
            for e in ents:
                k = e & 0xFFF
                h.update(struct.pack("<I", e & 0xF000))
                h.update(kfs[k] if k < len(kfs) else b"OOB")
            clips.append({"clip": c, "bilder": n, "hash": h.hexdigest()[:12],
                          "kf_erst": (ents[0] & 0xFFF) if ents else None})
            first += n
        out.append({"rec": i, "marker": marker, "bones": count, "ksize": ksize,
                    "keyframes": nkf, "clips": clips,
                    "datei_off_rbj": s, "datei_off_emr": s + pre, "datei_off_edd": s + edd})
    return out


def raumname(p):
    return os.path.splitext(os.path.basename(p))[0]


def main():
    if len(sys.argv) < 3:
        print(__doc__); return 2
    cmd = sys.argv[1]
    if cmd == "liste":
        for p in sys.argv[2:]:
            d = open(p, "rb").read()
            blk = rbj_block(d)
            if not blk:
                print(f"{raumname(p)}: kein RBJ"); continue
            print(f"{raumname(p)}: RBJ @0x{blk[0]:X} ({blk[1]} B)")
            for rec in records(d):
                if "fehler" in rec:
                    print(f"  rec{rec['rec']}: {rec['fehler']}"); continue
                fc = " ".join(f"{c['clip']}:{c['bilder']}" for c in rec["clips"])
                print(f"  rec{rec['rec']} marker=0x{rec['marker']:08X} bones={rec['bones']} "
                      f"kf={rec['keyframes']} clips={len(rec['clips'])}  [{fc}]")
    elif cmd == "suche":
        src, rec, clip = sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
        want = records(open(src, "rb").read())[rec]["clips"][clip]
        print(f"Quelle {raumname(src)} rec{rec} clip{clip}: {want['bilder']} Bilder hash={want['hash']}")
        for p in sys.argv[5:]:
            for r in records(open(p, "rb").read()):
                for c in r.get("clips", []):
                    if c["hash"] == want["hash"]:
                        print(f"  = {raumname(p)} rec{r['rec']} clip{c['clip']} ({c['bilder']} Bilder)")
    elif cmd == "json":
        out = {}
        for p in sys.argv[3:]:
            out[raumname(p)] = records(open(p, "rb").read())
        json.dump(out, open(sys.argv[2], "w"), indent=1)
        print(f"{len(out)} Raeume -> {sys.argv[2]}")
    else:
        print(__doc__); return 2
    return 0


if __name__ == "__main__":
    sys.exit(main())
