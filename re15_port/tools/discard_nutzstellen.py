#!/usr/bin/env python3
"""discard_nutzstellen.py - findet ALLE Schluessel-Benutzungsstellen in RE1.5, von BEIDEN Seiten.

Seite 1 (Text):   jeder RDT-Nachrichtenblock wird mit der Glyphentabelle des Ports dekodiert
                  (engine/src/msg_common.c:175-199) und nach "used the" / "use the" durchsucht.
Seite 2 (Skript): fuer jede so gefundene Nachrichten-Id wird JEDE Message_on-Stelle (0x2B) im
                  SCD gesucht und der umgebende Block ausgegeben.

Damit laesst sich die Benutzungsstelle nicht per Textvermutung, sondern am ausfuehrenden
Skript belegen. Das Kartenleser-Muster von RE1.5 ist (ROOM10D0/1230/11E0 sub20, identisch):
    Message_on <frage>          ; ".. Will you operate the card reader?"  (0x03 = Ja/Nein)
    Ck(12,31,0)                 ; Antwort JA   (msg_common.c:550)
    Ck(9,<tk_bit>,1)            ; Karte GENOMMEN?
    Message_on <benutzt>        ; "You've used the <Karte>."
    Evt_exec sub17              ; Tuer oeffnen
Laengen-/Sprungtabellen aus tools/scd_dump_room.py.

Aufruf: python discard_nutzstellen.py [ASSET_ROOT]
"""
import struct, sys, os, glob, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from discard_zensus import (op_size, fwd_target, rdt_section_end,
                            section_regions, u16, u32, s16)

NAMES = {0x21:"Ck", 0x22:"Set", 0x2B:"Message_on", 0x04:"Evt_exec", 0x06:"Ifel_ck",
         0x07:"Else_ck", 0x08:"Endif", 0x01:"Evt_end", 0x02:"Evt_next", 0x00:"Nop",
         0x50:"Item_aot_set", 0x2C:"Aot_set", 0x46:"Aot_reset", 0x36:"Se_on",
         0x09:"Sleep", 0x13:"Switch", 0x14:"Case", 0x17:"Goto", 0x18:"Gosub"}

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

def decode(d, s, e):
    out=[]; ctrl=[]; i=s
    while i < e and i < len(d):
        b=d[i]
        if b==0x01: ctrl.append("01"); break
        if b in (0x02,0x04,0x05,0x06,0x09,0x0A,0x0B):
            ctrl.append(f"{b:02x}:{d[i+1]:02x}"); i+=2; continue
        if b in (0x03,0x07): ctrl.append(f"{b:02x}"); i+=1; continue
        if b==0x08: out.append(" / "); i+=1; continue
        t=g(b)
        if t is None: ctrl.append(f"?{b:02x}")
        else: out.append(t)
        i+=1
    return "".join(out), ctrl

def messages(d):
    ms=u32(d,0x3C)
    if ms==0 or ms>=len(d) or ms+2>len(d): return {}
    first=u16(d,ms)
    if first<2 or first%2 or ms+first>len(d): return {}
    n=first//2
    tbl=[u16(d,ms+2*i) for i in range(n)]
    out={}
    for idx,o in enumerate(tbl):
        st=ms+o
        en=ms+tbl[idx+1] if idx+1<n else len(d)
        if st>=len(d): continue
        out[idx]=decode(d,st,min(en,st+400))
    return out

def walk_ops(d,start,end):
    """liefert (pc, op, size) in Skriptreihenfolge; STOP bei Desync."""
    pc=start; maxf=start; res=[]
    while pc<end:
        op=d[pc]; sz=op_size(d,pc)
        if sz is None or pc+sz>end: break
        res.append((pc,op,sz))
        t=fwd_target(d,pc,op)
        if t is not None and t>maxf: maxf=t
        if op==0x01 and pc+2>maxf: break
        pc+=sz
    return res

def fmt(d,pc,op,sz):
    raw=" ".join(f"{b:02x}" for b in d[pc:pc+sz])
    ex=""
    if op in (0x21,0x22): ex=f"  bank={d[pc+1]} bit={d[pc+2]} val={d[pc+3]}"
    elif op==0x2B:        ex=f"  msg={d[pc+1]}"
    elif op==0x04:        ex=f"  sub={d[pc+3]}"
    elif op==0x50:        ex=f"  typ=0x{d[pc+14]:02X} n={d[pc+16]} tk_bit={d[pc+18]}"
    return f"    0x{pc:05X} {NAMES.get(op,'op%02X'%op):14s} {raw}{ex}"

def main():
    root = sys.argv[1] if len(sys.argv)>1 else os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "..", "shared_assets", "PSX")
    rdts=sorted(glob.glob(os.path.join(root,"STAGE*","ROOM*.RDT")))
    treffer=0; geprueft=0; ohne_msg=0
    for p in rdts:
        d=open(p,"rb").read()
        if len(d)<0x48: continue
        geprueft+=1
        room=os.path.basename(p).split(".")[0]
        msgs=messages(d)
        if not msgs: ohne_msg+=1; continue
        ziel={i:t for i,(t,c) in msgs.items() if "ve used the" in t or "have used the" in t}
        if not ziel: continue
        # Seite 2: Skriptstellen
        ms,ss=u32(d,0x40),u32(d,0x44)
        for sec,tag in ((ms,"main"),(ss,"sub")):
            if sec==0 or sec>=len(d): continue
            se=rdt_section_end(d,sec)
            for (o,e,idx) in section_regions(d,sec,se):
                ops=walk_ops(d,sec+o,sec+e)
                for k,(pc,op,sz) in enumerate(ops):
                    if op!=0x2B or d[pc+1] not in ziel: continue
                    treffer+=1
                    print(f"\n### {room} {tag}{idx:02d}  Message_on {d[pc+1]} "
                          f"= {ziel[d[pc+1]]!r}")
                    for (qc,qo,qs) in ops[max(0,k-8):k+4]:
                        mark="->" if qc==pc else "  "
                        print(mark+fmt(d,qc,qo,qs)[2:])
    print(f"\n==== Abdeckung: {geprueft} RDTs mit Header, davon {ohne_msg} ohne Nachrichtenblock; "
          f"{treffer} Skriptstellen mit einer \"used the\"-Nachricht ====")

if __name__ == "__main__":
    main()
