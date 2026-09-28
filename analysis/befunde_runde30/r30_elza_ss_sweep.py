"""Runde 30 / Thema G: Savestate-Reihe ueber stage_saves/*.sav - was steht im ORIGINAL in
Raum (0x800b0fe2), Vorraum (0x800b0fe6), Cut (0x800b0fe4), Charakter-Byte (0x800aca5c),
Elza-Bit (0x800aca3c Bit 31), angefordertem PL-Index (0x800b0ff0) und den Vorspann-Flags der
Bank 3 (0x800b0ff8: Wort 3 = 0x800b1004, Wort 6 = 0x800b1010; Bit = 0x80000000 >> (n & 31),
Set-Handler @0x8003fdd0)?
Die Spalte @80026e4c trennt den Auslieferungsstand (0800e003) vom gepatchten Build (24c20108).
Werkzeug: .claude/skills/re15-savestate-ghidra/scripts/re15_ss.py
"""
import sys, os, glob
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/skills/re15-savestate-ghidra/scripts")
import re15_ss
rows=[]
for p in sorted(glob.glob(r"C:/workspace/git/reAi_v2/stage_saves/*.sav")):
    try:
        r=re15_ss.Ram(p)
    except Exception as e:
        print(os.path.basename(p),"FEHLER",e); continue
    stage=r.s16(0x800b0fe0); room=r.s16(0x800b0fe2); cut=r.s16(0x800b0fe4); prev=r.s16(0x800b0fe6)
    ch=r.u8(0x800aca5c); a3c=r.u32(0x800aca3c); a38=r.u32(0x800aca38); pl=r.s16(0x800b0ff0)
    w3=r.u32(0x800b1004); w6=r.u32(0x800b1010)
    f193=(w6>>30)&1; f207=(w6>>16)&1; f125=(w3>>2)&1; f111=(w3>>16)&1; f112=(w3>>15)&1
    patched = r.bytes(0x80026e4c,4).hex()
    rows.append((os.path.basename(p),stage,room,cut,prev,ch,a3c>>31,pl,f193,f125,f207,f111,f112,patched,a38))
print("%-46s st room cut prev ch elza pl | f193 f125 f207 f111 f112 | @80026e4c aca38"%"datei")
for x in rows:
    print("%-46s %2d 0x%02x %3d 0x%02x %2d %4d %2d |  %d    %d    %d    %d    %d   | %s %08x"%x)
