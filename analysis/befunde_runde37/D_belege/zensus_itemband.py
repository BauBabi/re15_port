#!/usr/bin/env python3
"""Runde 37 Spur D - Zensus fuer das Band-Gate der Item-AOTs.

Walk = derselbe wie re15_port/tools/scd_dump_room.py (Laengentabelle + rdt_section_end/section_regions).
Gesammelt ueber ALLE ausgelieferten RDTs (shared_assets/PSX/STAGE1..6/ROOM*.RDT), main- und sub-SCD:
  * jedes Item_aot_set (0x50) mit floor (pc[4]) und dem 0x80-Ignorierbit (@0x80042cb4)
  * jeder Schreiber des Members 0x12 (= +0x82 Band, FUN_8004116c Tabelle 0x80010c8c[18] -> @0x80041228):
      Member_set 0x34 (pc[1]==0x12), Member_set2 0x35 (pc[1]==0x12), 0x5A (pc[3]==0x12), 0x5B (pc[2]==0x12)
    mit dem letzten Work_set (0x2E kind/idx) DAVOR in derselben Region (kind 1 = Spieler @0x80040da4)
  * jedes Door_aot_set (0x3B) roh (Ziel/Ankunft werden getrennt ausgewertet)
  * Aot_set/Aot_reset sce 12/13 (Treppe) mit floor
Aufruf: python zensus_itemband.py <PSX-Wurzel> [--alle] [--tueren]
"""
import struct, sys, os

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

ITEMS=[]; MEMB=[]; DOORS=[]; STAIRS=[]; STATS={'rdts':0,'ops':0,'desync':0}

def walk(d,start,end,room,lbl):
    pc=start; maxf=start; ws=None
    while pc<end:
        op=d[pc]; sz=op_size(d,pc)
        if sz is None or pc+sz>end:
            STATS['desync']+=1; return
        STATS['ops']+=1
        if op==0x2E: ws=(d[pc+1],d[pc+2])
        elif op==0x50:
            lf=(d[pc+3]&0x80)!=0
            it = d[pc+22] if lf else d[pc+14]
            am = d[pc+24] if lf else d[pc+16]
            tk = u16(d,pc+26) if lf else u16(d,pc+18)
            ITEMS.append(dict(room=room,lbl=lbl,pc=pc,slot=d[pc+1],sce=d[pc+2],sat=d[pc+3],floor=d[pc+4],
                              x=s16(d,pc+6),z=s16(d,pc+8),w=s16(d,pc+10),dd=s16(d,pc+12),item=it,amount=am,tk=tk,lf=lf))
        elif op in (0x34,0x35,0x5A,0x5B):
            mid = d[pc+1] if op in (0x34,0x35) else (d[pc+3] if op==0x5A else d[pc+2])
            if mid==0x12:
                val = s16(d,pc+2) if op==0x34 else None
                MEMB.append(dict(room=room,lbl=lbl,pc=pc,op=op,val=val,ws=ws,raw=d[pc:pc+sz].hex(' ')))
        elif op==0x3B:
            lf=(d[pc+3]&0x80)!=0
            DOORS.append(dict(room=room,lbl=lbl,pc=pc,slot=d[pc+1],sce=d[pc+2],sat=d[pc+3],floor=d[pc+4],lf=lf,
                              raw=d[pc:pc+sz]))
        elif op in (0x2C,0x46) and d[pc+2] in (12,13):
            STAIRS.append(dict(room=room,lbl=lbl,pc=pc,op=op,slot=d[pc+1],sce=d[pc+2],
                               floor=(d[pc+4] if op==0x2C else None)))
        t=fwd_target(d,pc,op)
        if t is not None and t>maxf: maxf=t
        if op==0x01 and pc+2>maxf: return
        pc+=sz

def room_scan(path,room):
    d=open(path,'rb').read()
    if len(d)<0x48: return
    STATS['rdts']+=1
    ms,ss=u32(d,0x40),u32(d,0x44)
    for sec,tag in ((ms,"main"),(ss,"sub")):
        if sec==0 or sec>=len(d): continue
        se=rdt_section_end(d,sec)
        for (o,e,idx) in section_regions(d,sec,se):
            walk(d,sec+o,sec+e,room,f"{tag}{idx:02d}")

def fmt_item(i):
    return (f"ROOM{i['room']} {i['lbl']} @0x{i['pc']:05X} slot={i['slot']} sce={i['sce']} sat=0x{i['sat']:02X} "
            f"floor={i['floor']} rect=({i['x']},{i['z']},{i['w']},{i['dd']}) item=0x{i['item']:02X} x{i['amount']} "
            f"tk={i['tk']}{' LANG' if i['lf'] else ''}")

def main():
    root=sys.argv[1]
    for st in range(1,7):
        sd=os.path.join(root,f"STAGE{st}")
        if not os.path.isdir(sd): continue
        for fn in sorted(os.listdir(sd)):
            if fn.upper().startswith("ROOM") and fn.upper().endswith(".RDT"):
                room_scan(os.path.join(sd,fn), fn[4:8].upper())
    print(f"# {STATS['rdts']} RDTs, {STATS['ops']} Opcodes, {STATS['desync']} Desync, {len(ITEMS)} Item_aot_set, "
          f"{len(MEMB)} Schreiber Member 0x12, {len(DOORS)} Door_aot_set, {len(STAIRS)} Treppen-Saetze")
    nz=[i for i in ITEMS if i['floor']!=0]
    print(f"## Item_aot_set mit floor != 0: {len(nz)}")
    for i in nz: print(fmt_item(i))
    fl={}
    for i in ITEMS: fl[i['floor']]=fl.get(i['floor'],0)+1
    print("## floor-Verteilung aller Item_aot_set:", dict(sorted(fl.items())))
    print(f"## Schreiber Member 0x12: {len(MEMB)}")
    for m in MEMB:
        print(f"ROOM{m['room']} {m['lbl']} @0x{m['pc']:05X} op=0x{m['op']:02X} val={m['val']} work={m['ws']} [{m['raw']}]")
    if "--alle" in sys.argv:
        print("## alle Item_aot_set")
        for i in ITEMS: print(fmt_item(i))
    if "--tueren" in sys.argv:
        print("## alle Door_aot_set (Kurzform: pc[14..19] Ankunft x/y/z, pc[20..21] rot, pc[22] Stage, pc[23] Raum, pc[24] Cut)")
        for dd in DOORS:
            r=dd['raw']; b=8 if dd['lf'] else 0
            ax,ay,az=s16(r,14+b),s16(r,16+b),s16(r,18+b)
            print(f"ROOM{dd['room']} {dd['lbl']} @0x{dd['pc']:05X} slot={dd['slot']} sce={dd['sce']} sat=0x{dd['sat']:02X} "
                  f"floor={dd['floor']} -> stage={r[22+b]} raum=0x{r[23+b]:02X} cut={r[24+b]} ankunft=({ax},{ay},{az}) "
                  f"band={-(int(ay/0x708)) if ay<=0 else -(ay//0x708)}{' LANG' if dd['lf'] else ''}")
main()
