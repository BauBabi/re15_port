#!/usr/bin/env python3
"""Runde 41 Spur I: SCD-Dump (main+sub) eines Raums, opcode-exakt ueber den Walker aus
aot_sce_census.py. Aufruf: python scd_dump.py 2000 [main|sub] [regionsindex]"""
import os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import aot_sce_census as C  # noqa: E402
NAMES = {0x00:"Nop",0x01:"Evt_end",0x02:"Evt_next",0x03:"Evt_chain",0x04:"Evt_exec",0x05:"Evt_kill",
 0x06:"If",0x07:"Else",0x08:"End_if",0x09:"Sleep",0x0A:"Sleeping",0x0B:"Wsleep",0x0C:"Wsleeping",
 0x0D:"For",0x0E:"Next",0x0F:"While",0x10:"Ewhile",0x11:"Do",0x12:"Edwhile",0x13:"Switch",0x14:"Case",
 0x15:"Default",0x16:"Eswitch",0x17:"Goto",0x18:"Gosub",0x19:"Return",0x1A:"Break",0x1B:"For2",
 0x1C:"Break_point",0x1D:"Work_copy",0x1E:"Nop1E",0x20:"Nop20",0x21:"Ck",0x22:"Set",0x23:"Cmp",
 0x24:"Save",0x25:"Copy",0x26:"Calc",0x27:"Calc2",0x28:"Sce_rnd",0x29:"Cut_chg",0x2A:"Cut_old",
 0x2B:"Message_on",0x2C:"Aot_set",0x2D:"Obj_model_set",0x2E:"Work_set",0x2F:"Speed_set",
 0x30:"Add_speed",0x31:"Add_aspeed",0x32:"Pos_set",0x33:"Dir_set",0x34:"Member_set",0x35:"Member_set2",
 0x36:"Se_on",0x37:"Sca_id_set",0x38:"Flr_set",0x39:"Dir_ck",0x3A:"Sce_espr_on",0x3B:"Door_aot_set",
 0x3C:"Cut_auto",0x3D:"Member_copy",0x3E:"Member_cmp",0x3F:"Plc_motion",0x40:"Plc_dest",0x41:"Plc_neck",
 0x42:"Plc_ret",0x43:"Plc_flg",0x44:"Sce_em_set",0x45:"Col_chg_set",0x46:"Aot_reset",0x47:"Aot_on",
 0x48:"Super_set",0x49:"Super_reset",0x4A:"Plc_gun",0x4B:"Cut_replace",0x4C:"Sce_espr_kill",
 0x4D:"Door_model_set",0x4E:"Item_aot_set",0x4F:"Sce_key_ck",0x50:"Sce_trg_ck",0x51:"Sce_bgm_control",
 0x52:"Sce_espr_control",0x53:"Sce_fade_set",0x54:"Sce_espr3d_on",0x55:"Member_calc",0x56:"Member_calc2",
 0x57:"Sce_bgmtbl_set",0x58:"Plc_rot",0x59:"Xa_on",0x5A:"Weapon_chg",0x5B:"Plc_cnt",0x5C:"Sce_shake_on",
 0x5D:"Mizu_div_set",0x5E:"Keep_Item_ck"}
def dump(room, which=None, ridx=None):
    import glob
    f = glob.glob(os.path.join(HERE, "..", "..", "shared_assets", "PSX", "STAGE*", f"ROOM{room}.RDT"))[0]
    d = open(f, "rb").read()
    for (sec, tag) in ((C.u32(d, 0x40), "main"), (C.u32(d, 0x44), "sub")):
        if which and tag != which: continue
        send = C.rdt_section_end(d, sec)
        for (o, e, idx) in C.section_regions(d, sec, send):
            if ridx is not None and idx != ridx: continue
            print(f"== {tag}{idx:02d} @0x{sec+o:05x}..0x{sec+e:05x}")
            pc = sec + o
            while pc < sec + e:
                op = d[pc]; sz = C.op_size(d, pc)
                if sz is None: print(f"  0x{pc:05x} ?? {op:02x} stop"); break
                print(f"  0x{pc:05x} {NAMES.get(op, hex(op)):16s} {d[pc:pc+sz].hex(' ')}")
                pc += sz
if __name__ == "__main__":
    dump(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None,
         int(sys.argv[3]) if len(sys.argv) > 3 else None)
