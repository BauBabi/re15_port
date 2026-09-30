#!/usr/bin/env python3
"""ss_cpu.py - Runde 34 / re_absturz_original.md §2.7

Liest den CPU-Zustand (GPR, hi/lo, pc/npc, COP0) aus einem DuckStation-Savestate.

Layout = CPU::DoState der installierten DuckStation (settings.ini [AutoUpdater] LastVersion
e8938f06347e4837c32ef4c71c21cbd43245f244, Quelle src/core/cpu_core.cpp dieses Standes):
    Marker "CPU" (u32 Laenge 3 + 'CPU')
    s32 pending_ticks, s32 downcount, u32 gte_completion_tick, u32 muldiv_completion_tick
    u32 regs.r[Reg::count]  (Reg::count = 34: r0..r31, hi, lo  -- src/core/cpu_types.h)
    u32 pc, u32 npc
    u32 BPC, BDA, TAR, BadVaddr, BDAM, BPCM, EPC, PRID, SR, CAUSE, DCIC
    u32 next_instruction, u32 current_instruction, u32 current_instruction_pc
    bool current_instruction_in_branch_delay_slot, current_instruction_was_branch_taken,
         next_instruction_is_branch_delay_slot, branch_was_taken, exception_raised, bus_error
Zusaetzlich: RAM-Werte (Spieler/aktueller Aktor) ueber re15_ss.

Aufruf:  python ss_cpu.py <sav> [<sav> ...]
"""
import sys, struct, os
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
import re15_ss

GPR = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
       "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]
COP0 = ["BPC", "BDA", "TAR", "BadVaddr", "BDAM", "BPCM", "EPC", "PRID", "SR", "CAUSE", "DCIC"]


def cpu_state(blob):
    i = blob.find(b"\x03\x00\x00\x00CPU")
    if i < 0:
        raise RuntimeError("CPU-Marker nicht gefunden")
    o = i + 7
    w = list(struct.unpack_from("<%dI" % 54, blob, o))
    st = {"marker_off": i}
    st["pending_ticks"], st["downcount"], st["gte_tick"], st["muldiv_tick"] = w[0:4]
    for k in range(32):
        st[GPR[k]] = w[4 + k]
    st["hi"], st["lo"] = w[36], w[37]
    st["pc"], st["npc"] = w[38], w[39]
    for k, n in enumerate(COP0):
        st[n] = w[40 + k]
    st["next_instruction"], st["current_instruction"], st["current_instruction_pc"] = w[51], w[52], w[53]
    b = blob[o + 54 * 4: o + 54 * 4 + 6]
    st["cur_in_bd"], st["cur_branch_taken"], st["next_is_bd"], st["branch_was_taken"], \
        st["exception_raised"], st["bus_error"] = list(b)
    return st


def dump(path):
    r = re15_ss.Ram(path)
    st = cpu_state(r.blob)
    print("=== %s" % path)
    print("  CPU-Marker @blob 0x%x, RAM-Basis 0x%x" % (st["marker_off"], r.base))
    line = []
    for k in range(32):
        line.append("%-4s=%08x" % (GPR[k], st[GPR[k]]))
        if len(line) == 4:
            print("  " + "  ".join(line)); line = []
    print("  hi=%08x lo=%08x" % (st["hi"], st["lo"]))
    print("  pc=%08x npc=%08x   next_instr=%08x cur_instr=%08x cur_instr_pc=%08x" % (
        st["pc"], st["npc"], st["next_instruction"], st["current_instruction"], st["current_instruction_pc"]))
    print("  EPC=%08x SR=%08x CAUSE=%08x (ExcCode=%d, IP=%02x) BadVaddr=%08x" % (
        st["EPC"], st["SR"], st["CAUSE"], (st["CAUSE"] >> 2) & 0x1f, (st["CAUSE"] >> 8) & 0xff, st["BadVaddr"]))
    print("  flags: cur_in_bd=%d cur_branch_taken=%d next_is_bd=%d branch_was_taken=%d exc_raised=%d bus_err=%d" % (
        st["cur_in_bd"], st["cur_branch_taken"], st["next_is_bd"], st["branch_was_taken"],
        st["exception_raised"], st["bus_error"]))
    return r, st


if __name__ == "__main__":
    for p in sys.argv[1:]:
        dump(p)
