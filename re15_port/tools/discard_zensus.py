#!/usr/bin/env python3
"""discard_zensus.py — Vollzensus fuer die "Schluessel verbraucht? Wegwerfen?"-Abfrage.

Beantwortet genau drei Fragen, jede mit ausgewiesener Abdeckungszahl:

  (1) WO wird ein Gegenstand platziert?   -> Item_aot_set (0x50): Typ, Menge, taken-Bit.
      Das taken-Bit ist byte-true pc[18] (Kurzform) bzw. pc[26] (Langform) und adressiert
      Flag-ZONE 9 — Installer @0x800406d4-0x80040718 (`FUN_8004efe4(DAT_800b1078, payload[2])`),
      im Port engine/src/scd_vm.c op_item_aot_set.

  (2) WORAN haengt der Fortschritt?       -> jedes Ck (0x21) und Set (0x22) auf ZONE 9.
      Die Kartenleser/Schloesser von RE1.5 pruefen NICHT das Inventar, sondern das
      zone-9-taken-Bit (z.B. ROOM10D0 sub20 @0x19C0: `21 09 34 01` = Ck(9,52,1) = "Blaue
      Keycard GENOMMEN?"). Wird das Bit nie geloescht, kann ein weggeworfener Gegenstand
      keine Sackgasse erzeugen.

  (3) LIEST irgendein Skript-Opcode das Inventar? -> Zensus von 0x5E (Keep_Item_ck).
      Der RE1.5-Handler LAB_80042b04 ist KEIN Praedikat: er setzt 0x800aca3c |= 0x20,
      ruft FUN_80013278(pc[1], u16@pc[2]) (Icon-/VRAM-Lader) und liefert `ori v0,zero,0x1`
      @0x80042b44 — konstant 1. Er kann also nichts gaten.

Laengen-/Sprungtabellen und der Desync-STOP sind aus tools/scd_dump_room.py uebernommen
(die wiederum aus engine/src/scd_vm.c s_opcode_sizes stammen).

Aufruf:  python discard_zensus.py <ASSET_ROOT>   (Default: ../shared_assets/PSX)
         --json <datei>   schreibt den Rohzensus als JSON
"""
import struct, sys, os, json, glob, argparse

SIZES = {
    0x00:1, 0x01:2, 0x02:1, 0x03:4, 0x04:4, 0x05:2, 0x06:4, 0x07:4,
    0x08:2, 0x09:4, 0x0A:3, 0x0B:1, 0x0C:1, 0x0D:6, 0x0E:2, 0x0F:4,
    0x10:2, 0x11:4, 0x12:2, 0x13:4, 0x14:6, 0x15:4, 0x16:2, 0x17:6,
    0x18:2, 0x19:2, 0x1A:2, 0x1B:6, 0x1C:1, 0x1D:1, 0x1E:1,
    0x20:1, 0x21:4, 0x22:4, 0x23:6, 0x24:4, 0x25:3, 0x26:6, 0x27:4,
    0x28:1, 0x29:2, 0x2A:1, 0x2B:4, 0x2C:20, 0x2D:34, 0x2E:3, 0x2F:4,
    0x30:1, 0x31:1, 0x32:8, 0x33:8, 0x34:4, 0x35:3, 0x36:12, 0x37:4,
    0x38:12, 0x39:4, 0x3A:16, 0x3B:32, 0x3C:2, 0x3D:3, 0x3E:6, 0x3F:4,
    0x40:8, 0x41:10, 0x42:1, 0x43:4, 0x44:20, 0x45:3, 0x46:10, 0x47:2,
    0x48:16, 0x49:8, 0x4A:2, 0x4B:3, 0x4C:18, 0x4D:10, 0x4E:5, 0x4F:22,
    0x50:22, 0x51:4, 0x52:4, 0x53:3, 0x54:6, 0x55:6, 0x56:6, 0x57:4,
    0x58:4, 0x59:4, 0x5A:6, 0x5B:4, 0x5C:4, 0x5D:4, 0x5E:4,
}

def u16(b,o): return b[o]|(b[o+1]<<8)
def s16(b,o): return struct.unpack_from("<h",b,o)[0]
def u32(b,o): return struct.unpack_from("<I",b,o)[0]

def op_size(d,pc):
    op=d[pc]
    if op>=0x5F or op==0x1F: return None
    if op==0x2C: return 28 if (d[pc+3]&0x80) else 20
    if op==0x3B: return 40 if (d[pc+3]&0x80) else 32
    if op==0x50: return 30 if (d[pc+3]&0x80) else 22
    return SIZES.get(op)

def fwd_target(d,pc,op):
    if op==0x06: return pc+4+u16(d,pc+2)
    if op==0x07: return pc+s16(d,pc+2)
    if op in (0x0D,0x1B): return pc+6+s16(d,pc+2)
    if op in (0x0F,0x11,0x13): return pc+4+s16(d,pc+2)
    if op==0x14: return pc+6+u16(d,pc+2)
    if op==0x17:
        o=s16(d,pc+4); return pc+o if o>0 else None
    return None

def section_ptr_extent(d,s):
    if s==0 or s+2>len(d): return 0
    first=u16(d,s)
    if first<2 or first%2 or s+first>len(d): return 0
    mx=0
    for i in range(first//2):
        o=u16(d,s+2*i)
        if o==0 or s+o>=len(d): continue
        if o>mx: mx=o
    return mx

def rdt_section_end(d,s):
    floor=s+section_ptr_extent(d,s)
    c=[]
    for o in range(0x40,0x60,4):
        if o+4>len(d): break
        v=u32(d,o)
        if floor<v<=len(d): c.append(v)
    return min(c) if c else len(d)

def section_regions(d,ss,se):
    if ss==0 or ss+2>len(d): return []
    first=u16(d,ss)
    if first<2 or first%2 or ss+first>len(d): return []
    n=first//2
    tbl=[u16(d,ss+2*i) for i in range(n)]
    so=sorted(set(o for o in tbl if o and ss+o<len(d)))
    regs=[]
    for i,o in enumerate(so):
        e=so[i+1] if i+1<len(so) else (se-ss)
        e=min(e,se-ss)
        if e>o: regs.append((o,e))
    off2idx={}
    for idx,o in enumerate(tbl):
        off2idx.setdefault(o,idx)
    return [(o,e,off2idx.get(o,-1)) for (o,e) in regs]


class Zensus:
    def __init__(self):
        self.items=[]        # Item_aot_set-Records
        self.zone9_ck=[]     # Ck(9, bit, val)
        self.zone9_set=[]    # Set(9, bit, op)
        self.all_set=[]      # jedes Set, fuer die Gegenprobe "wer loescht?"
        self.keep_item=[]    # 0x5E
        self.opcount={}
        self.rooms=0
        self.stubs=[]
        self.desync=0

    def walk(self, room, tag, idx, d, start, end):
        pc=start; maxf=start
        while pc<end:
            op=d[pc]
            sz=op_size(d,pc)
            if sz is None:
                self.desync+=1; return False
            if pc+sz>end: return False
            self.opcount[op]=self.opcount.get(op,0)+1
            where=dict(room=room, sub=f"{tag}{idx:02d}", off=pc)
            if op==0x50:
                lf=(d[pc+3]&0x80)!=0
                rec=dict(where)
                rec.update(slot=d[pc+1], sce=d[pc+2], long=lf,
                           type=d[pc+22] if lf else d[pc+14],
                           amount=d[pc+24] if lf else d[pc+16],
                           tk_bit=d[pc+26] if lf else d[pc+18],
                           tk_prop=d[pc+28] if lf else d[pc+20])
                self.items.append(rec)
            elif op==0x21:
                if d[pc+1]==9:
                    r=dict(where); r.update(bit=d[pc+2], val=d[pc+3]); self.zone9_ck.append(r)
            elif op==0x22:
                r=dict(where); r.update(bank=d[pc+1], bit=d[pc+2], op=d[pc+3])
                self.all_set.append(r)
                if d[pc+1]==9: self.zone9_set.append(r)
            elif op==0x5E:
                r=dict(where); r.update(a=d[pc+1], b=u16(d,pc+2)); self.keep_item.append(r)
            t=fwd_target(d,pc,op)
            if t is not None and t>maxf: maxf=t
            if op==0x01 and pc+2>maxf: return True
            pc+=sz
        return True

    def room(self, path):
        d=open(path,"rb").read()
        room=os.path.basename(path).split(".")[0]
        if len(d) < 0x48:            # Stummel-RDT (z.B. 4 Byte) - kein Header, kein SCD
            self.stubs.append((room, len(d)))
            return
        self.rooms+=1
        ms,ss=u32(d,0x40),u32(d,0x44)
        for sec,tag in ((ms,"main"),(ss,"sub")):
            if sec==0 or sec>=len(d): continue
            se=rdt_section_end(d,sec)
            for (o,e,idx) in section_regions(d,sec,se):
                self.walk(room,tag,idx,d,sec+o,sec+e)


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("root", nargs="?", default=os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "..", "shared_assets", "PSX"))
    ap.add_argument("--json")
    a=ap.parse_args()
    rdts=sorted(glob.glob(os.path.join(a.root,"STAGE*","ROOM*.RDT")))
    z=Zensus()
    for p in rdts: z.room(p)

    print(f"RDTs gelesen: {z.rooms} von {len(rdts)}   Stummel (<0x48 B): {len(z.stubs)} {z.stubs}   Desync-Stopps: {z.desync}")
    print(f"Item_aot_set (0x50): {len(z.items)} Records, "
          f"{len(set(i['type'] for i in z.items))} verschiedene Typen")
    print(f"Keep_Item_ck (0x5E): {len(z.keep_item)} Vorkommen")
    print(f"Ck  auf Zone 9: {len(z.zone9_ck)}   Set auf Zone 9: {len(z.zone9_set)}")
    print(f"Set gesamt (alle Zonen): {len(z.all_set)}")

    # (3) Wer LOESCHT ein zone-9-Bit?  op 0 = clear, 7 = toggle (scd_vm.c:2020-2022)
    loescher=[s for s in z.zone9_set if s["op"] in (0,7)]
    print(f"\n=== Zone-9-Bits, die geloescht/getoggelt werden: {len(loescher)} ===")
    for s in loescher: print("   ", s)

    # (1)+(2) je Gegenstand
    bybit={}
    for i in z.items: bybit.setdefault(i["tk_bit"], []).append(i)
    ckbybit={}
    for c in z.zone9_ck: ckbybit.setdefault(c["bit"], []).append(c)
    setbybit={}
    for s in z.zone9_set: setbybit.setdefault(s["bit"], []).append(s)

    bytype={}
    for i in z.items: bytype.setdefault(i["type"], []).append(i)
    print(f"\n=== Gegenstaende: Platzierung -> taken-Bit -> Tore (Ck auf Zone 9) ===")
    print(f"{'Typ':>5} {'n':>3} {'Menge':>6} {'tk_bit':>7} {'Ck':>4} {'Set':>4}  Raeume / Tore")
    for t in sorted(bytype):
        recs=bytype[t]
        bits=sorted(set(r["tk_bit"] for r in recs))
        mengen=sorted(set(r["amount"] for r in recs))
        nck=sum(len(ckbybit.get(b,[])) for b in bits)
        nset=sum(len(setbybit.get(b,[])) for b in bits)
        rooms=",".join(sorted(set(r["room"] for r in recs)))
        tore=";".join(f"{c['room']}/{c['sub']}@0x{c['off']:X}(val={c['val']})"
                      for b in bits for c in ckbybit.get(b,[]))
        print(f" 0x{t:02X} {len(recs):>3} {str(mengen):>6} {str(bits):>7} {nck:>4} {nset:>4}  {rooms}")
        if tore: print(f"        Tore: {tore}")

    print(f"\n=== Ck auf Zone 9, deren Bit KEIN Item_aot_set traegt "
          f"(Skript-Vergabe oder fremde Bedeutung) ===")
    for b in sorted(ckbybit):
        if b in bybit: continue
        src=";".join(f"{s['room']}/{s['sub']}@0x{s['off']:X}(op={s['op']})" for s in setbybit.get(b,[]))
        print(f"   bit {b:>3}: {len(ckbybit[b])} Ck   Set: {src or '—'}")
        for c in ckbybit[b]: print(f"        {c['room']}/{c['sub']}@0x{c['off']:X} val={c['val']}")

    if a.json:
        with open(a.json,"w") as f:
            json.dump(dict(rooms=z.rooms, desync=z.desync, items=z.items,
                           zone9_ck=z.zone9_ck, zone9_set=z.zone9_set,
                           keep_item=z.keep_item, opcount=z.opcount), f, indent=1)
        print(f"\nJSON -> {a.json}")

if __name__ == "__main__":
    main()
