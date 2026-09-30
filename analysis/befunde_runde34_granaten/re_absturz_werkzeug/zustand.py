#!/usr/bin/env python3
"""zustand.py - Runde 34 / re_absturz_original.md §2  (Zustand eines Savestates der Granatenversuche)

Je Savestate:
  * CPU: EPC / RA / pc / npc / k0 / Cause (Layout siehe ss_cpu.py)
  * ESP-Pool 0x800a73b8 (96 Plaetze x 0x84, Belege re_wurf_flug_explosion.md §1.4):
      +0x00 Routine A, +0x02 Routine B, +0x1e Zuender, +0x26 Abprall-Zaehler,
      +0x28/2a/2c Weltlage (s16), +0x6c Flags, +0x70 Kategorie, +0x71 sub
  * Spieler 0x800aca54: Modus +0x4 (0x800aca58), +0x5, HP +0x9a, Lage +0x34/38/3c, Gier +0x6a,
    Waffe 0x800aca5d, +0x93 (0x800acae7)
  * Gegner 0x800acc2c + i*0x1f4: Wort0 Bit0 aktiv, +0x4/+0x5/+0x6/+0x7, +0x8 Typ, +0x9,
    HP +0x9a, Lage +0x34/38/3c, +0x93; aktueller Aktor-Zeiger 0x800ac784
  * Licht-Latch 0x800b5358
Aufruf: python zustand.py <sav> [<sav> ...]   (--kurz: eine Zeile je Datei)
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
import re15_ss
from ss_cpu import cpu_state

POOL, NSLOT, STRIDE = 0x800a73b8, 96, 0x84
ENEMY, ESTR = 0x800acc2c, 0x1f4


def esp_slots(r):
    out = []
    for i in range(NSLOT):
        b = POOL + i * STRIDE
        fl = r.u8(b + 0x6c)
        if fl == 0:
            continue
        out.append(dict(i=i, a=r.u16(b), bb=r.u16(b + 2), fuse=r.u16(b + 0x1e), cnt=r.u16(b + 0x26),
                        x=r.s16(b + 0x28), y=r.s16(b + 0x2a), z=r.s16(b + 0x2c), fl=fl,
                        cat=r.u8(b + 0x70), sub=r.u8(b + 0x71), anim=r.u8(b + 0x6e),
                        vx=r.s16(b + 0x10), vy=r.s16(b + 0x12), vz=r.s16(b + 0x14)))
    return out


def enemies(r):
    out = []
    for i in range(16):
        e = ENEMY + i * ESTR
        w0 = r.u32(e)
        if not (w0 & 1):
            continue
        out.append(dict(i=i, adr=e, s4=r.u8(e + 4), s5=r.u8(e + 5), s6=r.u8(e + 6), s7=r.u8(e + 7),
                        typ=r.u8(e + 8), f9=r.u8(e + 9), hp=r.s16(e + 0x9a), x=r.s32(e + 0x34),
                        y=r.s32(e + 0x38), z=r.s32(e + 0x3c), h93=r.u8(e + 0x93), clip=r.u8(e + 0x94)))
    return out


def dump(path, kurz=False):
    r = re15_ss.Ram(path)
    st = cpu_state(r.blob)
    cur = r.u32(0x800ac784)
    name = os.path.basename(path)
    slots = esp_slots(r)
    gr = [s for s in slots if s["cat"] == 4 and s["sub"] == 0x0d]
    kinder = [s for s in slots if s["cat"] == 3 and s["sub"] in (0x19, 0x0b)]
    en = enemies(r)
    if kurz:
        g = gr[0] if gr else None
        gs = ("A=%d B=%d Z=%d n=%d (%d,%d,%d) fl=%02x" % (g["a"], g["bb"], g["fuse"], g["cnt"], g["x"], g["y"], g["z"], g["fl"])) if g else "keine Granate"
        es = " ".join("%d:%02x/%d/%x/%d hp%d" % (e["i"], e["typ"], e["s4"], e["s5"], e["s6"], e["hp"]) for e in en)
        print("%-18s EPC=%08x RA=%08x | %s | Kinder=%d | Spieler m=%d hp=%d | %s | cur=%08x" % (
            name, st["EPC"], st["ra"], gs, len(kinder), r.u16(0x800aca58), r.s16(0x800acaee), es, cur))
        return r, st
    print("=== %s" % path)
    print("  CPU: EPC=%08x RA=%08x pc=%08x npc=%08x k0=%08x v0=%08x a0=%08x a1=%08x sp=%08x CAUSE=%08x(Exc=%d BD=%d) cur_instr_pc=%08x" % (
        st["EPC"], st["ra"], st["pc"], st["npc"], st["k0"], st["v0"], st["a0"], st["a1"], st["sp"], st["CAUSE"],
        (st["CAUSE"] >> 2) & 0x1f, st["CAUSE"] >> 31, st["current_instruction_pc"]))
    print("  Spieler: Modus=%d +5=%d +6=%d HP=%d Lage=(%d,%d,%d) Gier=%d Waffe=%02x +93=%02x Licht5358=%02x" % (
        r.u16(0x800aca58), r.u8(0x800aca59), r.u8(0x800aca5a), r.s16(0x800acaee), r.s32(0x800aca88),
        r.s32(0x800aca8c), r.s32(0x800aca90), r.s16(0x800acabe), r.u8(0x800aca5d), r.u8(0x800acae7),
        r.u8(0x800b5358)))
    print("  aktueller Aktor 0x800ac784 = %08x" % cur)
    for s in slots:
        if s["cat"] in (3, 4):
            print("  ESP[%2d] kat=%d sub=%02x A=%2d B=%2d Zuender=%3d Zaehler=%d Welt=(%d,%d,%d) v=(%d,%d,%d) Flags=%02x Satz=%d" % (
                s["i"], s["cat"], s["sub"], s["a"], s["bb"], s["fuse"], s["cnt"], s["x"], s["y"], s["z"],
                s["vx"], s["vy"], s["vz"], s["fl"], s["anim"]))
    for e in en:
        print("  Gegner[%d] @%08x Typ=%02x +4=%d +5=%d +6=%d +7=%d +9=%02x HP=%d Lage=(%d,%d,%d) +93=%02x Clip=%d" % (
            e["i"], e["adr"], e["typ"], e["s4"], e["s5"], e["s6"], e["s7"], e["f9"], e["hp"], e["x"], e["y"], e["z"],
            e["h93"], e["clip"]))
    return r, st


if __name__ == "__main__":
    kurz = "--kurz" in sys.argv
    for p in [a for a in sys.argv[1:] if not a.startswith("--")]:
        dump(p, kurz)
