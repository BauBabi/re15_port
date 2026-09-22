#!/usr/bin/env python3
"""Dump die RDT-Nachrichtenbloecke eines Raums, dekodiert mit der Glyphentabelle
des Ports (re15_port/engine/src/msg_common.c:175-199, re15_msg_glyph)."""
import struct, sys, os

def g(b):
    if b == 0x00: return " "
    if 0x0C <= b <= 0x15: return chr(ord('0') + b - 0x0C)
    if b == 0x16: return ":"
    if b == 0x18: return ","
    if b == 0x19: return '"'
    if b == 0x1A: return "!"
    if b == 0x1B: return "?"
    if b == 0x1C: return "!?"
    if 0x1D <= b <= 0x36: return chr(ord('A') + b - 0x1D)
    if b == 0x37: return "["
    if b == 0x38: return "/"
    if b == 0x39: return "]"
    if b == 0x3A: return "'"
    if b == 0x3B: return "-"
    if b == 0x3C: return "."
    if 0x3D <= b <= 0x56: return chr(ord('a') + b - 0x3D)
    if b == 0x57: return "."
    if b == 0xF2: return "..."
    return None

def u16(b,o): return b[o]|(b[o+1]<<8)
def u32(b,o): return struct.unpack_from("<I",b,o)[0]

def decode(d, s, e):
    out=[]; ctrl=[]; i=s
    while i < e and i < len(d):
        b=d[i]
        if b==0x01: ctrl.append("01:END"); break
        if b in (0x02,0x04,0x05,0x06,0x09,0x0A,0x0B):
            ctrl.append(f"{b:02x}:{d[i+1]:02x}"); i+=2; continue
        if b in (0x03,0x07): ctrl.append(f"{b:02x}"); i+=1; continue
        if b==0x08: out.append(" / "); i+=1; continue
        t=g(b)
        if t is None: ctrl.append(f"?{b:02x}")
        else: out.append(t)
        i+=1
    return "".join(out), ctrl

def main():
    path=sys.argv[1]
    d=open(path,"rb").read()
    ms=u32(d,0x3C)   # message section
    print(f"{os.path.basename(path)}  msg_sec=0x{ms:X}")
    if ms==0 or ms>=len(d): print("  kein Message-Block"); return
    first=u16(d,ms); n=first//2
    tbl=[u16(d,ms+2*i) for i in range(n)]
    for idx,o in enumerate(tbl):
        st=ms+o
        en=ms+tbl[idx+1] if idx+1<n else len(d)
        txt,ctrl=decode(d,st,min(en,st+400))
        print(f"  id {idx:>2} @0x{st:04X}: {txt!r}   ctrl={ctrl}")

main()
