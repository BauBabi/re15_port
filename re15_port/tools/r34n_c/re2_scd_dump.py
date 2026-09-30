#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): RE2-SCD eines Raums opcode-weise ausgeben (Namen nach der RE2-Tabelle,
Laengen aus re2_scd_lens.py = abgeleitet aus den Handlern @0x800a74c8).
Aufruf: re2_scd_dump.py ROOM2130 [INIT|MAIN] [sub]"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
from re2_scd_walk import scd_blocks, block_end, walk
N = ("nop evt_end evt_next evt_chain evt_exec evt_kill if else endif sleep sleeping wsleep wsleeping for next "
     "while ewhile do edwhile switch case default eswitch goto gosub return break for2 break_point work_copy nop1e "
     "nop1f nop20 ck set cmp save copy calc calc2 sce_rnd cut_chg cut_old message_on aot_set obj_model_set work_set "
     "speed_set add_speed add_aspeed pos_set dir_set member_set member_set2 se_on sca_id_set flr_set dir_ck "
     "sce_espr_on door_aot_set cut_auto member_copy member_cmp plc_motion plc_dest plc_neck plc_ret plc_flg "
     "sce_em_set col_chg_set aot_reset aot_on super_set super_reset plc_gun cut_replace sce_espr_kill "
     "door_model_set item_aot_set sce_key_ck sce_trg_ck sce_bgm_control sce_espr_control sce_fade_set "
     "sce_espr3d_on member_calc member_calc2 sce_bgmtbl_set plc_rot xa_on weapon_chg plc_cnt sce_shake_on "
     "mizu_div_set keep_item_ck xa_vol kage_set cut_be_set sce_item_lost plc_gun_eff sce_espr_on2 "
     "sce_espr_kill2 plc_stop aot_set_4p door_aot_set_4p item_aot_set_4p light_pos_set light_kido_set "
     "rbj_reset sce_scr_move parts_set movie_on splc_ret splc_sce super_on mirror_set sce_fade_adjust "
     "sce_espr3d_on2 sce_item_get sce_line_start sce_line_main sce_line_end sce_parts_bomb sce_parts_down "
     "light_color_set light_pos_set2 light_kido_set2 light_color_set2 se_vol sce_item_cmp sce_espr_task "
     "plc_heal st_map_hint sce_em_pos_ck poison_ck poison_clr sce_item_ck_lost evt_next2 vloop_set").split()
room = sys.argv[1]; blk = sys.argv[2] if len(sys.argv) > 2 else None
want = int(sys.argv[3]) if len(sys.argv) > 3 else None
d = open(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", room + ".RDT"), "rb").read()
offs = struct.unpack_from("<23I", d, 8)
for name, base, subs in scd_blocks(d):
    if blk and name != blk: continue
    for i, so in enumerate(subs):
        if want is not None and i != want: continue
        s = base + so; e = block_end(d, base, subs, i, offs)
        ops, st = walk(d, s, e, last=(i + 1 == len(subs)))
        print("=== %s sub%02d @0x%05X..0x%05X status=%s" % (name, i, s, e, st))
        for (a, op, ln) in ops:
            nm = N[op] if op < len(N) else "op%02X" % op
            print("  @0x%05X  %-40s %s" % (a, d[a:a + ln].hex(" "), nm))
