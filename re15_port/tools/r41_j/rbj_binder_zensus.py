#!/usr/bin/env python3
"""rbj_binder_zensus.py - Runde 41 Spur J: RBJ-Marker-Binder FUN_8001b3f8, Original gegen Port.

Original (PSX.EXE, selbst disassembliert, re15_disasm.py dis 0x8001b3f8 48):
  8001b3fc lw v0,-14472(v0)  ; DAT_800ac778 (RDT-Kopf)      8001b404 lw a2,92(v0)  ; RBJ = RDT+0x5C
  8001b40c beq a2,zero,...   ; kein RBJ -> nichts           8001b410 addu a3,zero,zero  ; a3 = 0 EINMAL
  8001b41c lw v0,0(a2) / 8001b424 addu t0,a2,v0  ; Trailer = RBJ + u32@0
  8001b420 lbu t2,4(a2)      ; Record-Zahl (Low-Byte)       8001b428 addiu a1,t0,4  ; edd_off-Wort
  8001b42c lw v0,0(t0) / 8001b434 addu v1,a2,v0 / 8001b438 lw a0,0(v1)  ; Marker-u32 am EMR-Praefix
  8001b440 andi v0,a0,0x1 / 8001b444 beq ... / 8001b448 srl a0,a0,1
  8001b450 sw v0,0(t3)=0x800acbd4 / 8001b460 sw v0,4(t3)=0x800acbd8  ; Spieler: JEDER Record mit Bit 0
                                                                       ; ueberschreibt -> der LETZTE gewinnt
  8001b464 beq a0,zero,0x8001b4c8  ; keine Gegner-Bits -> a3 unveraendert
  8001b470 andi v0,a0,0x1 / 8001b474 beq v0,zero,0x8001b4c0 / 8001b478 srl a0,a0,1
  8001b47c..90 a3&0xff * 0x1F4 (Entity-Stride) / 8001b4a0 sw t1,0(at)=0x800acdac+i*0x1F4 / 8001b4bc sw v0 (+4)
  8001b4c0 bne a0,zero,0x8001b470 / 8001b4c4 addiu a3,a3,1  ; Delay-Slot: a3++ JE BITPOSITION (auch 0-Bits
                                                             ; unter dem hoechsten), NIE je Record zurueckgesetzt
  8001b4c8 addiu a1,a1,8 / 8001b4cc addiu t2,t2,-1 / 8001b4d4 bne v0,zero,0x8001b42c / 8001b4d8 addiu t0,t0,8
Port vor Runde 41 (enemy_common.c rbj_resolve_slot): Gegner i = ERSTER Record mit Marker-Bit (1+i);
Spieler = Record 0 (re15_apply_room_cinematic -> re15_emd_parse_rbj).

Aufruf: rbj_binder_zensus.py <PSX-Wurzel> [--alle]   (Standard: nur abweichende Raeume)
"""
import os, struct, sys


def u32(b, o): return struct.unpack_from("<I", b, o)[0]


def rbj_markers(d):
    if len(d) < 0x60: return None
    s = u32(d, 0x5C)
    if s == 0 or s + 8 > len(d): return None
    total = u32(d, s); nrec = d[s + 4]
    if total == 0 or nrec == 0 or nrec > 8 or s + total + nrec * 8 > len(d): return None
    out = []
    for r in range(nrec):
        pre = u32(d, s + total + r * 8)
        if pre == 0 or s + pre + 4 > len(d): return None
        out.append(u32(d, s + pre))
    return out


def original(markers):
    """FUN_8001b3f8: (spieler_rec, {gegner_index: rec})"""
    pl, en, a3 = -1, {}, 0
    for r, m in enumerate(markers):
        if m & 1: pl = r                      # 8001b450: letzter gewinnt
        m >>= 1
        while m:                              # 8001b464/8001b4c0
            if m & 1: en[a3 & 0xFF] = r       # 8001b4a0
            m >>= 1
            a3 += 1                           # 8001b4c4 Delay-Slot
    return pl, en


def port_alt(markers):
    pl = 0 if markers else -1
    en = {}
    for i in range(0, 31):
        for r, m in enumerate(markers):
            if m & (1 << (1 + i)): en[i] = r; break
    return pl, en


def main():
    root = sys.argv[1]; alle = "--alle" in sys.argv
    n = n_rbj = n_ab = 0
    for st in sorted(os.listdir(root)):
        if not st.startswith("STAGE"): continue
        for f in sorted(os.listdir(os.path.join(root, st))):
            if not (f.startswith("ROOM") and f.endswith(".RDT")): continue
            n += 1
            d = open(os.path.join(root, st, f), "rb").read()
            mk = rbj_markers(d)
            if not mk: continue
            n_rbj += 1
            o, p = original(mk), port_alt(mk)
            ab = o != p
            n_ab += ab
            if ab or alle:
                print("%s Marker %s Spieler orig rec%d port rec%d Gegner orig %s port %s%s" % (
                    f, [hex(m) for m in mk], o[0], p[0], o[1], p[1], "  ABWEICHEND" if ab else ""))
    print("Raeume %d mit RBJ %d abweichend %d" % (n, n_rbj, n_ab))


if __name__ == "__main__":
    main()
