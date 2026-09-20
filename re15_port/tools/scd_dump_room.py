#!/usr/bin/env python3
"""Vollstaendiger SCD-Dump EINES Raums (main-SCD + ALLE subs), opcode-exakt.

Benutzt genau die Laengen-/Sprungtabellen aus re15_port/tools/aot_sce_census.py
(die wiederum aus re15_port/engine/src/scd_vm.c s_opcode_sizes stammen) plus die
disasm-verifizierten variablen Laengen:
  0x2C Aot_set  20/28 (pc[3]&0x80)   @0x80040590
  0x3B Door_aot_set 32/40            @0x80040618
  0x50 Item_aot_set 22/30            @0x8004065c
  0x2D Obj_model_set 34 fix          @0x80040aa4
Bei unbekanntem Opcode (>=0x5F oder 0x1F) wird der Walk ABGEBROCHEN statt
weiterzulaufen — ein desynchronisierter Walk ist kein Befund.

Aufruf:  python scd_dump_room.py <ROOMxxxx.RDT> [--sub N]
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
NAMES = {
 0x00:"Nop",0x01:"Evt_end",0x02:"Evt_next",0x03:"Evt_chain",0x04:"Evt_exec",
 0x05:"Evt_kill",0x06:"Ifel_ck",0x07:"Else_ck",0x08:"Endif",0x09:"Sleep",
 0x0A:"Sleeping",0x0B:"Wsleep",0x0C:"Wsleeping",0x0D:"For",0x0E:"Next",
 0x0F:"While",0x10:"Ewhile",0x11:"Do",0x12:"Edwhile",0x13:"Switch",0x14:"Case",
 0x15:"Default",0x16:"Eswitch",0x17:"Goto",0x18:"Gosub",0x19:"Return",
 0x1A:"Break",0x1B:"For2",0x1C:"Break_point",0x1D:"Work_copy",0x1E:"Nop1E",
 0x20:"Nop20",0x21:"Ck",0x22:"Set",0x23:"Cmp",0x24:"Save",0x25:"Copy",
 0x26:"Calc",0x27:"Calc2",0x28:"Sce_rnd",0x29:"Cut_chg",0x2A:"Cut_old",
 0x2B:"Message_on",0x2C:"Aot_set",0x2D:"Obj_model_set",0x2E:"Work_set",
 0x2F:"Speed_set",0x30:"Add_speed",0x31:"Add_aspeed",0x32:"Pos_set",
 0x33:"Dir_set",0x34:"Member_set",0x35:"Member_set2",0x36:"Se_on",
 0x37:"Sca_id_set",0x38:"Flr_set",0x39:"Sca_floor_set",0x3A:"Sce_espr_on",
 0x3B:"Door_aot_set",0x3C:"Cut_auto",0x3D:"Member_copy",0x3E:"Member_cmp",
 0x3F:"Plc_motion",0x40:"Plc_dest",0x41:"Plc_neck",0x42:"Plc_ret",
 0x43:"Plc_flg",0x44:"Sce_em_set",0x45:"Col_chg_set",0x46:"Aot_reset",
 0x47:"Aot_on",0x48:"Super_set",0x49:"Super_reset",0x4A:"Plc_gun",
 0x4B:"Cut_replace",0x4C:"Sce_espr_kill",0x4D:"Op4D",0x4E:"Item_aot_set_l",
 0x4F:"Sce_key_ck4F",0x50:"Item_aot_set",0x51:"Sce_key_ck",
 0x52:"Sce_espr_control",0x53:"Sce_fade_set",0x54:"Sce_bgm_control",
 0x55:"Member_calc",0x56:"Member_calc2",0x57:"Sce_bgmtbl_set",0x58:"Plc_rot",
 0x59:"Xa_on",0x5A:"Weapon_chg",0x5B:"Plc_cnt",0x5C:"Sce_shake_on",
 0x5D:"Mizu_div_set",0x5E:"Keep_Item_ck",
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

def dump(d,start,end,label,only=None):
    print(f"\n=== {label}  [file 0x{start:X}..0x{end:X}]  ({end-start} B)")
    pc=start; maxf=start
    while pc<end:
        op=d[pc]
        sz=op_size(d,pc)
        if sz is None:
            print(f"  0x{pc:05X} (+{pc-start:04X}) ?? op=0x{op:02X}  ABBRUCH (unbekannter Opcode)")
            return False
        if pc+sz>end:
            print(f"  0x{pc:05X} (+{pc-start:04X}) {NAMES.get(op,'?'):16s} ragt ueber Regionsende -> Stop")
            return False
        raw=" ".join(f"{b:02x}" for b in d[pc:pc+sz])
        t=fwd_target(d,pc,op)
        if t is not None and t>maxf: maxf=t
        extra=""
        if op==0x2D:
            extra=(f"  obj=0x{d[pc+1]:02X} type={d[pc+2]} band={d[pc+4]} "
                   f"pos=({s16(d,pc+10)},{s16(d,pc+12)},{s16(d,pc+14)}) "
                   f"rot=({s16(d,pc+16)},{s16(d,pc+18)},{s16(d,pc+20)}) "
                   f"box=c({s16(d,pc+22)},{s16(d,pc+24)},{s16(d,pc+26)})"
                   f"h({s16(d,pc+28)},{s16(d,pc+30)},{s16(d,pc+32)})")
        elif op in (0x2C,0x3B,0x50):
            lf=(d[pc+3]&0x80)!=0
            extra=(f"  slot={d[pc+1]} sce={d[pc+2]} flags=0x{d[pc+3]:02X} floor={d[pc+4]} "
                   f"follow={d[pc+5]} rect=({s16(d,pc+6)},{s16(d,pc+8)},{s16(d,pc+10)},{s16(d,pc+12)})"
                   f"{' LONG' if lf else ''}")
        elif op==0x46:
            extra=f"  slot={d[pc+1]} sce={d[pc+2]} flags=0x{d[pc+3]:02X} p=({u16(d,pc+4)},{u16(d,pc+6)},{u16(d,pc+8)})"
        elif op==0x04:
            extra=f"  cond=0x{d[pc+1]:02X} type=0x{d[pc+2]:02X} sub={d[pc+3]}"
        elif op==0x2E:
            extra=f"  kind={d[pc+1]} idx={d[pc+2]}"
        elif op==0x34:
            extra=f"  member=0x{d[pc+1]:02X} val={s16(d,pc+2)}"
        elif op==0x35:
            extra=f"  member=0x{d[pc+1]:02X} var={d[pc+2]}"
        elif op==0x3F:
            extra=f"  a={d[pc+1]} b={d[pc+2]} c={d[pc+3]}"
        elif op in (0x21,0x22):
            extra=f"  bank={d[pc+1]} bit={d[pc+2]} val={d[pc+3]}"
        print(f"  0x{pc:05X} (+{pc-start:04X}) {NAMES.get(op,'??'):16s} {raw}{extra}")
        if op==0x01 and pc+2>maxf:
            return True
        pc+=sz
    return True

def main():
    path=sys.argv[1]
    d=open(path,"rb").read()
    print(f"{os.path.basename(path)}  {len(d)} B")
    print("header: nSprite=%d nCut=%d nOmodel=%d nItem=%d nDoor=%d nRoom_at=%d reverb=%d"%tuple(d[0:7]))
    ms,ss=u32(d,0x40),u32(d,0x44)
    print(f"main_scd=0x{ms:X} sub_scd=0x{ss:X}")
    want=None
    if "--sub" in sys.argv:
        want=sys.argv[sys.argv.index("--sub")+1]
    for sec,tag in ((ms,"main"),(ss,"sub")):
        if sec==0 or sec>=len(d): continue
        se=rdt_section_end(d,sec)
        regs=section_regions(d,sec,se)
        print(f"\n### {tag}: 0x{sec:X}..0x{se:X}, {len(regs)} Regionen")
        for (o,e,idx) in regs:
            lbl=f"{tag}{idx:02d}"
            if want and lbl!=want and want!=tag: continue
            dump(d,sec+o,sec+e,lbl)

main()
