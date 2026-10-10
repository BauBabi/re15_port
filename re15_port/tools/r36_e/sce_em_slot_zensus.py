#!/usr/bin/env python3
"""Runde 36 Spur E: Zensus der Sce_em_set-Slots (Opcode 0x44, 20 B) aller ausgelieferten RDTs.

Frage: welche Gegner-Plaetze (pc[1] = Slot, Original `andi s4,v0,0x7f` @0x800420f8) benutzen die
Raeume, und wie viele davon liegen bei Slot >= 15 (Port-Aktor = Slot+1 >= 16 = bisher verworfen)?

Walker = re15_port/tools/aot_sce_census.py (opcode-genau, Laengentabelle aus scd_vm.c s_opcode_sizes,
Sektionsende-Fixes 2026-08-19). Ausgabe: je Raum Slot-Liste, Typen, Limit Save(0x12,n) (Opcode 0x24,
Index 0x12 = 0x800b0ff4, LAB_80040018), Zugriffe Work_set (0x2E) / 0x53 auf Gegner-Index >= 15.

Aufruf: python -I sce_em_slot_zensus.py <asset_root PSX> [json_out]
"""
import glob, json, os, struct, sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import aot_sce_census as W   # noqa: E402

def main():
    root = sys.argv[1]
    out_json = sys.argv[2] if len(sys.argv) > 2 else None
    files = sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT")))
    rooms = {}
    stops_total = 0
    for f in files:
        room = os.path.basename(f)[4:-4]
        data = open(f, "rb").read()
        if len(data) < 0x48: continue
        recs, saves, works = [], [], []
        def collect(data_, pc, op, room_, script, installs):
            if op == 0x44:
                recs.append(dict(off=pc, script=script, slot=data_[pc+1], type=data_[pc+2],
                                 beh=data_[pc+3], floor=data_[pc+4], persist=data_[pc+7]))
            elif op == 0x24:
                saves.append(dict(off=pc, script=script, idx=data_[pc+1],
                                  val=struct.unpack_from("<h", data_, pc+2)[0]))
            elif op in (0x2E, 0x53):
                works.append(dict(off=pc, script=script, op=op, kind=data_[pc+1], idx=data_[pc+2]))
        W.collect = collect   # walker ruft W.collect(data, pc, op, room, script, installs)
        main_s, sub_s = W.u32(data, 0x40), W.u32(data, 0x44)
        stats = dict(walked=0, total=0, residue=0, stops=[])
        for (sec, tag) in ((main_s, "main"), (sub_s, "sub")):
            if sec == 0 or sec >= len(data): continue
            send = W.rdt_section_end(data, sec)
            for (o, e, idx) in W.section_regions(data, sec, send):
                W.walk(data, sec + o, sec + e, room, f"{tag}{idx:02d}", [], stats)
        stops_total += len(stats["stops"])
        rooms[room] = dict(recs=recs, saves=saves, works=works,
                           stage=os.path.basename(os.path.dirname(f)), stops=len(stats["stops"]))
    # ---- Auswertung ----
    nrec = sum(len(r["recs"]) for r in rooms.values())
    allslots = defaultdict(int)
    for r in rooms.values():
        for x in r["recs"]: allslots[x["slot"]] += 1
    print(f"files={len(files)} Sce_em_set-Records={nrec} walk-stops={stops_total}")
    print("Slot -> Anzahl Records:", dict(sorted(allslots.items())))
    print(f"max Slot = {max(allslots) if allslots else None}")
    print("Raeume mit Slot >= 15 (Port-Aktor >= 16):")
    for room, r in sorted(rooms.items()):
        hi = [x for x in r["recs"] if x["slot"] >= 15]
        if not hi: continue
        slots = sorted(set(x["slot"] for x in r["recs"]))
        types = sorted(set(x["type"] for x in hi))
        caps = [(hex(s["off"]), s["val"]) for s in r["saves"] if s["idx"] == 0x12]
        print(f"  ROOM{room} ({r['stage']}): {len(r['recs'])} Records, Slots {slots[0]}..{slots[-1]} "
              f"({len(slots)} verschieden), Slot>=15: {len(hi)} Records "
              f"[{', '.join('%d@%s t%02X p%02X' % (x['slot'], hex(x['off']), x['type'], x['persist']) for x in hi)}], "
              f"Typen>=15 {[hex(t) for t in types]}, Save(0x12,n) {caps}")
    print("Work_set/0x53 mit Index >= 15:")
    for room, r in sorted(rooms.items()):
        w = [x for x in r["works"] if x["idx"] >= 15 and x["kind"] in (2, 3)]
        if w:
            print(f"  ROOM{room}: " + ", ".join(f"op{x['op']:#04x} kind{x['kind']} idx{x['idx']} @{x['off']:#x} {x['script']}" for x in w))
    if out_json:
        json.dump(rooms, open(out_json, "w"), indent=1)

if __name__ == "__main__":
    main()
