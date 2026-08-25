#!/usr/bin/env python3
"""RE1.5 SCD-Walker. Laengen = byte-true Tabelle aus re15_port/engine/src/scd_vm.c:166
(abgeleitet aus der Dispatch-Tabelle PTR_LAB_800744a8 der PSX.EXE, 95 Eintraege 0x00..0x5E).

RDT-Adresstabelle: 0x40 mainScd, 0x44 subScd, 0x48 extraScd (RE15_KNOWLEDGE.md 1.1).
"""
import struct, sys, os

L = [0]*256
_pairs = {
0x00:1,0x01:2,0x02:1,0x03:4,0x04:4,0x05:2,0x06:4,0x07:4,
0x08:2,0x09:4,0x0A:3,0x0B:1,0x0C:1,0x0D:6,0x0E:2,0x0F:4,
0x10:2,0x11:4,0x12:2,0x13:4,0x14:6,0x15:4,0x16:2,0x17:6,
0x18:2,0x19:2,0x1A:2,0x1B:6,0x1C:1,0x1D:1,0x1E:1,
0x20:1,0x21:4,0x22:4,0x23:6,0x24:4,0x25:3,0x26:6,0x27:4,
0x28:1,0x29:2,0x2A:1,0x2B:4,0x2C:20,0x2D:34,0x2E:3,0x2F:4,
0x30:1,0x31:1,0x32:8,0x33:8,0x34:4,0x35:3,0x36:12,0x37:4,
0x38:12,0x39:4,0x3A:16,0x3B:32,0x3C:2,0x3D:3,0x3E:6,0x3F:4,
0x40:8,0x41:10,0x42:1,0x43:4,0x44:20,0x45:3,0x46:10,0x47:2,
0x48:16,0x49:8,0x4A:2,0x4B:3,0x4C:18,0x4D:10,0x4E:5,0x4F:22,
0x50:22,0x51:4,0x52:4,0x53:3,0x54:6,0x55:6,0x56:6,0x57:4,
0x58:4,0x59:4,0x5A:6,0x5B:4,0x5C:4,0x5D:4,0x5E:4,
}
for k,v in _pairs.items(): L[k]=v

NAMES = {
0x00:"Nop",0x01:"Evt_end",0x02:"Evt_next",0x03:"Evt_chain",0x04:"Evt_exec",0x05:"Evt_kill",
0x06:"Ifel_ck",0x07:"Else_ck",0x08:"Endif",0x09:"Sleep",0x0A:"Sleeping",0x0B:"Wsleep",
0x0C:"Wsleeping",0x0D:"For",0x0E:"Next",0x0F:"While",0x10:"Ewhile",0x11:"Do",0x12:"Edwhile",
0x13:"Switch",0x14:"Case",0x15:"Default",0x16:"Eswitch",0x17:"Goto",0x18:"Gosub",0x19:"Return",
0x1A:"Break",0x1B:"For2",0x1C:"Break_point",0x1D:"Work_copy",0x1E:"Nop1E",0x20:"Nop20",
0x21:"Ck",0x22:"Set",0x23:"Cmp",0x24:"Save",0x25:"Copy",0x26:"Calc",0x27:"Calc2",0x28:"Sce_rnd",
0x29:"Cut_chg",0x2A:"Cut_old",0x2B:"Message_on",0x2C:"Aot_set",0x2D:"Obj_model_set",
0x2E:"Work_set",0x2F:"Speed_set",0x30:"Add_speed",0x31:"Add_aspeed",0x32:"Pos_set",0x33:"Dir_set",
0x34:"Member_set",0x35:"Member_set2",0x36:"Se_on",0x37:"Sca_id_set",0x38:"Flr_set",
0x39:"Sca_floor_set",0x3A:"Sce_espr_on",0x3B:"Door_aot_set",0x3C:"Cut_auto",0x3D:"Member_get",
0x3E:"Member_cmp",0x3F:"Plc_motion",0x40:"Plc_dest",0x41:"Plc_neck",0x42:"Plc_ret",0x43:"Plc_flg",
0x44:"Sce_em_set",0x45:"Col_chg_set",0x46:"Aot_reset",0x47:"Aot_on",0x48:"Super_set",
0x49:"Super_reset",0x4A:"Plc_gun",0x4B:"Cut_replace",0x4C:"Sce_espr_kill",0x4D:"Op4D",
0x4E:"Op4E",0x4F:"Op4F",0x50:"Item_aot_set",0x51:"Sce_key_ck",0x52:"Sce_espr_control",
0x53:"Sce_fade_set",0x54:"Sce_bgm_control",0x55:"Member_calc",0x56:"Member_calc2",
0x57:"Sce_bgmtbl_set",0x58:"Plc_rot",0x59:"Xa_on",0x5A:"Weapon_chg",0x5B:"Plc_cnt",
0x5C:"Sce_shake_on",0x5D:"Mizu_div_set",0x5E:"Keep_Item_ck",
}

def s16(b,o): return struct.unpack_from("<h",b,o)[0]
def u16(b,o): return struct.unpack_from("<H",b,o)[0]

def blocks(d):
    out=[]
    for off,name in ((0x40,"main"),(0x44,"sub"),(0x48,"extra")):
        base=struct.unpack_from("<I",d,off)[0]
        if base==0 or base>=len(d): continue
        n=u16(d,base)
        if n==0 or n%2: continue
        out.append((name,base,[u16(d,base+2*i) for i in range(n//2)]))
    return out

def oplen(d,pc):
    op=d[pc]
    # variable Laengen, byte-true aus den Handlern (siehe re15_scd_oplen.py):
    #   0x2C Aot_set        @0x80040590 lbu pc[3]; andi 0x80 -> 28 : 20
    #   0x3B Door_aot_set   @0x80040618 lbu pc[3]; andi 0x80 -> 40 : 32
    #   0x50 Item_aot_set   @0x8004065c lbu pc[3]; andi 0x80 -> 30 : 22
    if op in (0x2C,0x3B,0x50):
        big={0x2C:28,0x3B:40,0x50:30}[op]; sml={0x2C:20,0x3B:32,0x50:22}[op]
        return big if (d[pc+3]&0x80) else sml
    return L[op]

def fmt(d,pc,op,n):
    p=d[pc:pc+n]
    hx=" ".join("%02x"%c for c in p)
    nm=NAMES.get(op,"OP%02X"%op)
    extra=""
    if op in (0x21,0x22):  extra=" bank=%d bit=0x%02x val=%d"%(p[1],p[2],p[3])
    elif op==0x06: extra=" skip=%d"%u16(p,2)
    elif op==0x07: extra=" skip=%d"%u16(p,2)
    elif op==0x04: extra=" ev=0x%02x a=0x%02x b=0x%02x"%(p[1],p[2],p[3])
    elif op==0x18: extra=" sub=%d"%p[1]
    elif op==0x17: extra=" cnt=%d flg=%d rel=%d"%(p[1],p[2],s16(p,4))
    elif op==0x29: extra=" cut=%d"%p[1]
    elif op==0x2B: extra=" msg=0x%02x p2=0x%02x p3=0x%02x"%(p[1],p[2],p[3])
    elif op==0x2C: extra=" aot=%d type=0x%02x flr=%d x=%d z=%d w=%d h=%d d=%04x %04x %04x"%(
        p[1],p[2],p[3],s16(p,4),s16(p,6),s16(p,8),s16(p,10),u16(p,12),u16(p,14),u16(p,16))
    elif op==0x50: extra=" aot=%d flr=%d x=%d z=%d w=%d h=%d item=0x%02x amt=%d md=0x%04x flg=0x%04x"%(
        p[1],p[3],s16(p,4),s16(p,6),s16(p,8),s16(p,10),u16(p,12),u16(p,14),u16(p,16),u16(p,18))
    elif op==0x44: extra=" slot=%d type=0x%02x pose=%d flr=%d x=%d y=%d z=%d rot=%d ai=0x%04x snd=0x%04x"%(
        p[2],p[3],p[4],p[5],s16(p,6),s16(p,8),s16(p,10),s16(p,12),u16(p,14),u16(p,16))
    elif op==0x2D: extra=" id=%d"%p[1]
    elif op==0x36: extra=" bank=%d smp=%d vol=%d pan=%d x=%d y=%d z=%d"%(p[2],p[3],p[4],p[5],s16(p,6),s16(p,8),s16(p,10))
    elif op==0x3A: extra=" "+hx[3:]
    elif op==0x4C: extra=" "+hx[3:]
    elif op==0x46: extra=" aot=%d"%p[1]
    elif op==0x47: extra=" aot=%d"%p[1]
    elif op==0x3B: extra=" aot=%d flr=%d x=%d z=%d w=%d h=%d nx=%d ny=%d nz=%d ndir=%d stage=%d room=%d cut=%d"%(
        p[1],p[3],s16(p,4),s16(p,6),s16(p,8),s16(p,10),s16(p,12),s16(p,14),s16(p,16),s16(p,18),p[20],p[21],p[22])
    elif op==0x2E: extra=" kind=%d idx=%d"%(p[1],p[2])
    elif op==0x34: extra=" member=0x%02x val=%d"%(p[1],s16(p,2))
    elif op==0x3E: extra=" member=0x%02x op=%d val=%d"%(p[2],p[1],s16(p,4)) 
    elif op==0x24: extra=" var=%d val=%d"%(p[1],s16(p,2))
    elif op==0x23: extra=" var=%d op=%d val=%d"%(p[2],p[3],s16(p,4))
    elif op==0x13: extra=" var=%d"%p[1]
    elif op==0x14: extra=" val=%d skip=%d"%(u16(p,4),u16(p,2))
    elif op==0x54: extra=" "+hx[3:]
    elif op==0x53: extra=" "+hx[3:]
    elif op==0x5E: extra=" item=0x%02x"%p[2]
    elif op==0x88 or op==0x76: extra=" "+hx[3:]
    return "%s  %-16s %s | %s"%("0x%06X"%pc, nm, extra, hx)

def walk(d,s,e,out):
    pc=s
    while pc<e:
        op=d[pc]; n=oplen(d,pc)
        if n==0:
            out.append("0x%06X  ???%02X  <unknown, stop>  %s"%(pc,op," ".join("%02x"%c for c in d[pc:pc+12])))
            return
        if pc+n>e:
            out.append("0x%06X  %s  <overrun %d>"%(pc,NAMES.get(op,"OP%02X"%op),pc+n-e)); return
        out.append(fmt(d,pc,op,n))
        pc+=n

if __name__=="__main__":
    path=sys.argv[1]
    d=open(path,"rb").read()
    print("file=%s size=0x%X"%(path,len(d)))
    hdr=struct.unpack_from("<8B",d,0)
    print("nSprite=%d nCut=%d nOmodel=%d nItem=%d nDoor=%d nRoom_at=%d reverb=%d"%hdr[:7])
    tbl=struct.unpack_from("<24I",d,0x20)
    for i,v in enumerate(tbl):
        print("  tbl[0x%02X] = 0x%08X"%(0x20+4*i,v))
    for name,base,subs in blocks(d):
        print("\n===== %s SCD @0x%X  (%d subs) ====="%(name,base,len(subs)))
        for i,o in enumerate(subs):
            end = base+subs[i+1] if i+1<len(subs) else None
            if end is None:
                cand=[x for x in tbl if base+o < x <= len(d)]
                end=min(cand) if cand else len(d)
            out=[]
            print("--- %s%02d @0x%X (0x%X..0x%X) ---"%(name,i,base+o,base+o,end))
            walk(d,base+o,end,out)
            print("\n".join(out))
