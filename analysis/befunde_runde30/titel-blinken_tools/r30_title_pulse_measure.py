#!/usr/bin/env python3
"""r30_title_pulse_measure.py - Runde 30 / Thema D.

DYNAMISCHE MESSUNG der Puls-Taktung im ORIGINAL aus DuckStation-Savestates, die im
Titelmenue stehen (Titel-State @0x801026c4 == 2, Sub-State @0x801026c5 == 1).

Gelesen wird je Savestate:
  Vcount   u32 @0x800787dc   VBlank-Zaehler der libetc (VSync FUN_80061fc0 liest ihn
                             @0x80062000-04 als Rueckgabe fuer mode<0 und @0x80062118 in
                             der Warteschleife v_wait FUN_80062108)
  lastV    u32 @0x800787f0   Vcount beim letzten VSync-Ruecksprung (@0x800620d8-dc)
  vsArg    u8  @0x800b5456   Argument von VSync im Flip FUN_8002137c
  ctr      u16 @0x80102946   Pulszaehler   (FUN_801028ec)
  val      u16 @0x80102944   Pulswert      (FUN_801028ec)

Aus ZWEI Savestates derselben Sitzung (Vcount laeuft seit Boot monoton) folgt
  dV   = Vcount_b - Vcount_a            (vergangene VBlanks, exakt)
  dCtr = (ctr_b - ctr_a) mod 60         (Pulsaufrufe modulo Periode)
Hypothese H30 (1 Aufruf je 2 VBlanks): dV/2  mod 60 == dCtr
Hypothese H60 (1 Aufruf je 1 VBlank ): dV    mod 60 == dCtr
Das Skript gibt je Paar beide Reste aus.  Nur lesend.
"""
import sys, os, itertools
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
import re15_ss
ROOT = r"C:\workspace\git\reAi_v2\stage_saves"

def expect_val(ctr):
    # FUN_801028ec: ctr<0x1f -> +2 sonst -2 (Vergleich VOR dem Inkrement, @0x801028fc),
    # Ruecksetzen auf 0x80 wenn ctr==0x3c (@0x80102918-28)
    up = min(ctr, 0x1f); dn = max(0, ctr - 0x1f)
    return 0x80 + 2 * up - 2 * dn

def load(name):
    r = re15_ss.Ram(os.path.join(ROOT, name))
    return dict(name=name, V=r.u32(0x800787dc), lastV=r.u32(0x800787f0), arg=r.u8(0x800b5456),
                ctr=r.u16(0x80102946), val=r.u16(0x80102944),
                st=r.u8(0x801026c4), sub=r.u8(0x801026c5), cur=r.u8(0x801026ca))

def main():
    names = sys.argv[1:] or ["boot_40.sav", "boot_44.sav", "boot_48.sav", "boot_52.sav"]
    rows = [load(n) for n in names]
    print("%-28s %-10s %-10s %-5s %-4s %-5s %-8s %-3s %-3s %-3s" % (
        "savestate", "Vcount", "lastV", "vsArg", "ctr", "val", "val_soll", "st", "sub", "cur"))
    for r in rows:
        print("%-28s %-10d %-10d %-5d %-4d 0x%02x  0x%02x     %-3d %-3d %-3d" % (
            r["name"], r["V"], r["lastV"], r["arg"], r["ctr"], r["val"], expect_val(r["ctr"]),
            r["st"], r["sub"], r["cur"]))
    print()
    print("%-28s %-28s %-7s %-6s %-12s %-12s %s" % ("a", "b", "dV", "dCtr", "H30:(dV/2)%60", "H60:dV%60", "Urteil"))
    for a, b in zip(rows, rows[1:]):
        dV = b["V"] - a["V"]; dC = (b["ctr"] - a["ctr"]) % 60
        h30 = (dV / 2.0) % 60; h60 = dV % 60
        # +-1 Toleranz: der Savestate kann zwischen VBlank und Task-Tick liegen
        ok30 = min(abs(h30 - dC), 60 - abs(h30 - dC)) <= 1.0
        ok60 = min(abs(h60 - dC), 60 - abs(h60 - dC)) <= 1.0
        print("%-28s %-28s %-7d %-6d %-13.1f %-12d %s" % (
            a["name"], b["name"], dV, dC, h30, h60,
            ("H30 passt" if ok30 else "H30 FAELLT") + " / " + ("H60 passt" if ok60 else "H60 FAELLT")))

if __name__ == "__main__":
    main()
