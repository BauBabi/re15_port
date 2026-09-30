#!/usr/bin/env python3
"""Spur B (Runde 34 Nacht): sucht einen Text in ALLEN Nachrichten von RE1.5 und RE2.

RE1.5: RDT-Nachrichtenblock (Zeiger @RDT+0x3C), Glyphentabelle wie rdt_msgdump.py
       (= re15_port/engine/src/msg_common.c re15_msg_glyph), Ende 0x01.
RE2:   die aus den RDTs extrahierten .msg-Dateien unter info/re2leon/PL0/RDT/room*/msg/
       (2315 Stueck). Zeichen 0x1D..0x36 = A..Z, 0x3D..0x56 = a..z, 0x00 = Leerzeichen
       (Codepage wie analysis/befunde_2026-09-26/re2-schalterraetsel-2130.md §1.2),
       Steuercodes >= 0xF0 werden als {xx} ausgegeben, 0xFE = Ende.
Dazu RE1.5 PSX.EXE/DEBUG.BIN und RE2 PSX.EXE als Rohbytes: dort wird nach der
kodierten Bytefolge des Suchworts gesucht (beide Codepages, Buchstaben identisch).

Aufruf: msg_suche.py <regex> [--alle]
"""
import os, re, struct, sys, glob

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
HAUPT = "C:/workspace/git/reAi_v2"   # nur lesen (info/ ist dort vollstaendig)

def g15(b):
    if b == 0x00: return " "
    if 0x0C <= b <= 0x15: return chr(ord('0') + b - 0x0C)
    tab = {0x16: ":", 0x18: ",", 0x19: '"', 0x1A: "!", 0x1B: "?", 0x37: "[", 0x38: "/",
           0x39: "]", 0x3A: "'", 0x3B: "-", 0x3C: ".", 0x57: ".", 0xF2: "..."}
    if b in tab: return tab[b]
    if 0x1D <= b <= 0x36: return chr(ord('A') + b - 0x1D)
    if 0x3D <= b <= 0x56: return chr(ord('a') + b - 0x3D)
    return None

def dec15(d, s, e):
    out = []; i = s
    while i < e and i < len(d):
        b = d[i]
        if b == 0x01: break
        if b in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B): out.append("{%02x:%02x}" % (b, d[i+1])); i += 2; continue
        if b in (0x03, 0x07): out.append("{%02x}" % b); i += 1; continue
        if b == 0x08: out.append(" / "); i += 1; continue
        t = g15(b)
        out.append(t if t is not None else "{%02x}" % b)
        i += 1
    return "".join(out)

def g2(b):
    if b == 0x00: return " "
    if b == 0x01: return "."
    if 0x1D <= b <= 0x36: return chr(ord('A') + b - 0x1D)
    if 0x3D <= b <= 0x56: return chr(ord('a') + b - 0x3D)
    if 0x0C <= b <= 0x15: return chr(ord('0') + b - 0x0C)
    tab = {0x3A: "'", 0x3B: '"', 0x02: ",", 0x03: "-", 0x16: ":", 0x1A: "!", 0x1B: "?"}
    return tab.get(b)

def dec2(d):
    out = []; i = 0
    while i < len(d):
        b = d[i]
        if b == 0xFE: break
        if b >= 0xF0:
            if i + 1 < len(d): out.append("{%02x:%02x}" % (b, d[i+1])); i += 2; continue
            out.append("{%02x}" % b); i += 1; continue
        t = g2(b)
        out.append(t if t is not None else "{%02x}" % b)
        i += 1
    return "".join(out)

def re15_rdts():
    for p in sorted(glob.glob(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x60: continue
        ms = struct.unpack_from("<I", d, 0x3C)[0]
        if ms == 0 or ms >= len(d): continue
        first = d[ms] | (d[ms+1] << 8)
        n = first // 2
        if n == 0 or n > 64: continue
        tbl = [d[ms+2*i] | (d[ms+2*i+1] << 8) for i in range(n)]
        for idx, o in enumerate(tbl):
            st = ms + o
            en = ms + tbl[idx+1] if idx + 1 < n else min(len(d), st + 600)
            yield os.path.basename(p), idx, st, dec15(d, st, min(en, st + 600))

def re2_msgs():
    for p in sorted(glob.glob(os.path.join(HAUPT, "info/re2leon/PL0/RDT/room*/msg/*.msg"))):
        d = open(p, "rb").read()
        yield os.path.relpath(p, HAUPT).replace("\\", "/"), dec2(d)

def enc(s):
    out = bytearray()
    for ch in s:
        if ch == " ": out.append(0)
        elif "A" <= ch <= "Z": out.append(0x1D + ord(ch) - 65)
        elif "a" <= ch <= "z": out.append(0x3D + ord(ch) - 97)
        else: return None
    return bytes(out)

def main():
    pat = re.compile(sys.argv[1], re.I)
    n15 = n2 = 0
    for room, idx, st, txt in re15_rdts():
        n15 += 1
        if pat.search(txt): print(f"RE1.5 {room} msg {idx} @0x{st:05X}: {txt!r}")
    for f, txt in re2_msgs():
        n2 += 1
        if pat.search(txt): print(f"RE2   {f}: {txt!r}")
    print(f"# durchsucht: RE1.5 {n15} RDT-Nachrichten, RE2 {n2} .msg-Dateien")
    # Rohbyte-Suche in den Programmdateien (Wortfolgen aus dem Muster, nur Buchstaben/Leerzeichen)
    worte = [w for w in re.findall(r"[A-Za-z ]{4,}", sys.argv[1])]
    for w in worte:
        e = enc(w)
        if not e: continue
        for bin_ in ("info/Re1.5/PSX.EXE", "info/Re1.5/PSX/BIN/DEBUG.BIN", "info/re2leon/PSX.EXE"):
            p = os.path.join(HAUPT, bin_)
            if not os.path.exists(p): continue
            d = open(p, "rb").read()
            hits = [m.start() for m in re.finditer(re.escape(e), d)]
            print(f"# Rohbytes {w!r} in {bin_}: {len(hits)} Treffer " + " ".join(f"@0x{h:X}" for h in hits[:10]))

main()
