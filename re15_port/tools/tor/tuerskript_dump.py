#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tuerskript_dump.py - DO2-Tuerarchive zerlegen, Tuerskripte disassemblieren, Sequenz simulieren.

Quelle jeder Regel in diesem Werkzeug ist eine gelesene Instruktion der RE2-Retail-EXE
(info/re2leon/PSX.EXE, identisch SLUS_007.48) bzw. der RE1.5-EXE (info/Re1.5/PSX.EXE).
Die Adressen stehen an der jeweiligen Stelle im Code. Dossier: analysis/tor_1170/03_tuersequenz.md

Unterkommandos:
  container <DO2>            Aufbau eines Archivs (RE2: zwei Teile; RE1.5: Zeigertabelle @+0xC)
  dis <DO2> [skript]         Skripte feldgenau ausgeben
  stat <verzeichnis>         Opcode-Haeufigkeit + 0x4D-Statistik ueber alle DO2 eines Verzeichnisses
  sim <DO2> <door_type>      Sequenz Bild fuer Bild fahren (Kamera, Objekte, Ereignisse, Bildrechtecke)
  json <verzeichnis> <aus> [re15.DO2]
                             alles zusammen als JSON (build/tor_1170/tuersequenz.json)

Nichts in diesem Werkzeug schreibt in den Port oder in Spieldaten.
"""
import sys, os, struct, json, math, glob, collections

# ---------------------------------------------------------------------------------------------
# Opcode-Tabelle RE2 Retail: Dispatch PTR_LAB_800a74c8 (@0x800140cc im Tuer-Scheduler indiziert).
# Laenge = PC-Vorschub, am Handler gelesen. Nur Opcodes, die in Tuerskripten vorkommen, sind hier
# einzeln belegt; ein Opcode ausserhalb dieser Tabelle bricht die Zerlegung ab (kein Raten).
#   op: (name, laenge, handler, beleg)
# ---------------------------------------------------------------------------------------------
OPS = {
    0x00: ("Nop",             1, 0x800537e4, "@0x800537ec addiu v0,v0,1"),
    0x01: ("Evt_end",         2, 0x800537fc, "@0x80053810 sb zero,1(a3) / @0x80053850 sw a1,28(a3) (Ruecksprung); Satz 2 B aus den Daten"),
    0x02: ("Evt_next",        1, 0x80053860, "@0x80053868 addiu v0,v0,1; Rueckgabe 2 @0x80053874"),
    0x03: ("Evt_chain",       4, 0x80053878, "@0x80053888 lbu a1,3(v0) -> 0x800530ec; kein Vorschub (Skriptwechsel)"),
    0x04: ("Evt_exec",        4, 0x800538a4, "@0x800538bc addiu v0,v0,4"),
    0x05: ("Evt_kill",        2, 0x800538dc, "@0x80053914 addiu v0,v0,2"),
    0x06: ("Ifel_ck",         4, 0x80053924, "@0x8005392c addiu a1,a2,4"),
    0x07: ("Else_ck",         4, 0x80053964, "@0x8005397c addu v1,v1,v0 (Sprung um u16@2)"),
    0x08: ("Endif",           2, 0x800539a0, "@0x800539b8 addiu v0,v0,2"),
    0x09: ("Sleep",           4, 0x800539dc, "@0x800539e4 addiu v0,a2,1 (PC+1), liest u16@2 @0x80053a10; Rest laeuft als 0x0A"),
    0x0a: ("Sleeping",        3, 0x80053a24, "@0x80053a6c addiu v0,v0,3"),
    0x0d: ("For",             6, 0x80053b1c, "@0x80053b58 addiu t0,t0,6"),
    0x0e: ("Next",            2, 0x80053cbc, "@0x80053d1c addiu v0,v0,2"),
    0x0f: ("While",           4, 0x80053d3c, "@0x80053da0 addiu a1,a1,4"),
    0x10: ("Ewhile",          2, 0x80053e0c, "@0x80053e34 sw v0,28(a0) (Ruecksprung)"),
    0x13: ("Switch",          4, 0x80054020, "@0x80054040 addiu a3,a3,4"),
    0x14: ("Case",            6, 0x800540f8, "@0x80054100 addiu v0,v0,6"),
    0x15: ("Default",         2, 0x80054110, "@0x80054118 addiu v0,v0,2"),
    0x16: ("Eswitch",         2, 0x80054128, "@0x8005414c addiu v0,v0,2"),
    0x17: ("Goto",            6, 0x8005415c, "@0x80054190 addu a1,a1,t0 (s16@4 relativ zum Opcode)"),
    0x18: ("Gosub",           2, 0x800541a8, "@0x800541b4 addiu v1,v1,2 (Ruecksprungadresse)"),
    0x1a: ("Break",           2, 0x80054268, "@0x80054298 sw v1,28(a0) (Sprung ans Blockende)"),
    0x1d: ("Work_copy",       4, 0x800542b4, "@0x800542c8 addiu a1,a1,4"),
    0x23: ("Cmp",             6, 0x80054474, "@0x80054484 addiu v0,v0,6"),
    0x24: ("Save",            4, 0x8005451c, "@0x8005452c addiu v0,v0,4"),
    0x25: ("Copy",            3, 0x8005454c, "@0x8005455c addiu v0,v0,3"),
    0x26: ("Calc",            6, 0x8005458c, "@0x800545a4 addiu v0,v0,6"),
    0x2e: ("Work_set",        3, 0x80055904, "@0x80055928 addiu v0,v0,3"),
    0x2f: ("Speed_set",       4, 0x80055a84, "@0x80055a94 addiu v0,v0,4"),
    0x30: ("Add_speed",       1, 0x80055ab0, "@0x80055b1c addiu v0,v0,1"),
    0x31: ("Add_aspeed",      1, 0x80055b2c, "@0x80055b8c addiu v1,v1,1"),
    0x34: ("Member_set",      4, 0x80055c00, "@0x80055c30 addiu v0,v0,4"),
    0x35: ("Member_set2",     3, 0x80055c50, "@0x80055c90 addiu v0,v0,3"),
    0x36: ("Se_on",          12, 0x80056428, "@0x8005653c addiu v1,s0,12"),
    0x3d: ("Member_copy",     3, 0x80055e38, "@0x80055e54 addiu v0,v0,3"),
    0x4d: ("Door_model_set", 22, 0x80014ba4, "@0x80014cac addiu v0,a1,22"),
    0x53: ("Sce_fade_set",    6, 0x80057ef0, "@0x80057fa8 addiu v0,s2,6"),
    0x74: ("Sce_fade_adjust", 4, 0x80057fd8, "@0x80058004 addiu s0,s0,4"),
    0x8a: ("Vib_set0",        6, 0x80059348, "@0x80059378 addiu v1,v1,6"),
    0x8b: ("Vib_set1",        6, 0x80059394, "@0x800593c8 addiu v1,v1,6"),
    0x8c: ("Vib_fade_set",    8, 0x800593e4, "@0x8005941c addiu v1,v1,8"),
}

# Kamera und Projektion der RE2-Tuersequenz (Door_init FUN_80013c1c)
RE2_EYE    = (10000, 0, 0)   # @0x80010830..38 (Bytes 10 27 00 00 | 00.. | 00..), uebergeben @0x80013e38 addiu a0,sp,20
RE2_TARGET = (0, 0, 0)       # @0x8001083c..44, uebergeben @0x80013e40 addiu a1,sp,32
RE2_H      = 290             # @0x80013e34 addiu a0,zero,290 -> jal 0x8008de24 (SetGeomScreen)
OFX, OFY   = 160, 120        # SetGeomOffset(160,120): @0x80049cac/@0x80049cb0 und @0x80068e80/@0x80068e88;
                             # Door_init selbst ruft 0x8008de04 NICHT auf (kein jal im Bereich 0x80013c1c..0x80013eb0)
RE15_EYE    = (30000, 0, 0)  # @0x80016460 ori v0,zero,0x7530 -> sw 0x800b2210
RE15_TARGET = (22000, 0, 0)  # @0x80016468 ori v0,zero,0x55f0 -> sw 0x800b221c


def u16(d, o): return struct.unpack_from("<H", d, o)[0]
def s16(d, o): return struct.unpack_from("<h", d, o)[0]
def u32(d, o): return struct.unpack_from("<I", d, o)[0]


# ---------------------------------------------------------------------------------------------
# Container
# ---------------------------------------------------------------------------------------------
class Do2:
    """Ein DO2-Archiv. kind = 're2' (zwei Teile) oder 're15' (Zeigertabelle)."""

    def __init__(self, path):
        self.path = path
        self.d = d = open(path, "rb").read()
        self.kind = None
        h0, h1, h2 = struct.unpack_from("<3I", d, 0)
        if h0 == 0x0c and h1 < len(d) and h2 < len(d):
            # RE1.5: @0x800162b4..c8 relozieren [4],[8]; @0x800162cc/d4 Tabelle = Basis+[0];
            # @0x800162f4 ori a0,a0,0x100c -> die drei Tabelleneintraege zaehlen ab Basis+0xC
            self.kind = "re15"
            self.vh_tab, self.vb = h1, h2
            e = struct.unpack_from("<3I", d, h0)
            self.md1 = e[0] + 0xc
            self.scd = e[1] + 0xc
            self.tim = e[2] + 0xc
            self.scd_end = self.tim
            self.part2 = 0
        else:
            # RE2: Teil 2 liegt sektorgenau; Tabelle DAT_8009a520 (12 B je Tuertextur-Typ) traegt
            # u16 Tonteil-Groesse, u16 Teil-2-Groesse, u32 Sektorversatz (@0x800150b0 lw t2,4(v1),
            # @0x800150f4 lhu v1,2(v1)). Hier ohne EXE bestimmt: erster Sektor, dessen zwei Kopfworte
            # auf ein MD1 und ein TIM (Magic 10 00 00 00) zeigen.
            self.kind = "re2"
            self.part2 = None
            for off in range(0x800, len(d), 0x800):
                a, b = struct.unpack_from("<II", d, off)
                if 0 < a < b < len(d) - off and d[off + b:off + b + 4] == b"\x10\x00\x00\x00":
                    self.part2 = off
                    break
            if self.part2 is None:
                raise ValueError("kein Teil 2 gefunden: " + path)
            a, b = struct.unpack_from("<II", d, self.part2)
            self.md1 = self.part2 + a      # @0x80013d3c lw v0,0(s0) / @0x80013d48 addu v0,v0,s0
            self.tim = self.part2 + b      # @0x80013d24 lw a0,0(v1) / @0x80013d34 addu a0,a0,s0
            self.scd = self.part2 + 8      # @0x80013e10..2c  DAT_800d8cbc = 0x801a1008
            self.scd_end = self.md1
        self.offs = self._script_table()

    def _script_table(self):
        d, t = self.d, self.scd
        first = u16(d, t)
        n = first // 2
        return [u16(d, t + 2 * k) for k in range(n)]

    def script(self, k):
        s = self.scd + self.offs[k]
        e = self.scd + (self.offs[k + 1] if k + 1 < len(self.offs) else self.scd_end - self.scd)
        return self.d[s:e], s

    def meshes(self):
        d, m = self.d, self.md1
        ln, unk, cnt = struct.unpack_from("<3I", d, m)
        out = []
        base = m + 12                       # RE2 @0x80013de4 addiu v1,v1,12 ; RE1.5 @0x800163b0 addiu v0,v0,12
        for i in range(cnt // 2):
            o = base + i * 56               # RE2 @0x80014b60..68 (a1*7*8) ; RE1.5 @0x8001705c..6c
            vo, vc, no, nc, to, tc, txo = struct.unpack_from("<7I", d, o)
            qvo, qvc, qno, qnc, qo, qc, qtx = struct.unpack_from("<7I", d, o + 28)
            vs = [struct.unpack_from("<3h", d, base + vo + 8 * k) for k in range(vc)]
            tris = [struct.unpack_from("<6H", d, base + to + 12 * k) for k in range(tc)]
            out.append(dict(index=i, verts=vs, tris=[(t[1], t[3], t[5]) for t in tris],
                            tri_count=tc, quad_count=qc, vert_count=vc,
                            bbox=dict(x=[min(v[0] for v in vs), max(v[0] for v in vs)],
                                      y=[min(v[1] for v in vs), max(v[1] for v in vs)],
                                      z=[min(v[2] for v in vs), max(v[2] for v in vs)]) if vs else None))
        return out


# ---------------------------------------------------------------------------------------------
# Disassembler
# ---------------------------------------------------------------------------------------------
def decode(code, p):
    """Ein Befehl -> dict(op, name, len, felder). Bricht bei unbekanntem Opcode ab."""
    op = code[p]
    if op not in OPS:
        raise ValueError("Opcode 0x%02x @0x%x nicht belegt" % (op, p))
    name, ln, handler, _ = OPS[op]
    if p + ln > len(code):
        raise ValueError("Befehl 0x%02x @0x%x laeuft ueber das Skriptende" % (op, p))
    b = code[p:p + ln]
    f = {}
    if op == 0x4d:
        # Handler @0x80014ba4, Feld fuer Feld:
        f = dict(
            obj=b[1],            # @0x80014bb8 lbu v0,1(a1) -> Zeiger aus 0x800d4dd8[obj]
            id=b[2],             # @0x80014bd0 lbu v0,2(a1) -> sb obj+0x08
            frame=b[3],          # @0x80014bdc lbu v0,3(a1) -> sh obj+0x10e (Bildnummer fuer Bit 0x400)
            on=b[4],             # @0x80014be8 lbu v0,4(a1) -> sw obj+0x00 (gezeichnet, wenn != 0: @0x8001429c)
            mesh=b[5],           # @0x80014bf4 lbu v0,5(a1) -> sh obj+0x146 ; Mesh-Eintrag @0x80014b60..70
            flags=u16(b, 6),     # @0x80014c00 lhu v1,6(a1) -> sh obj+0x144
            attr=s16(b, 8),      # @0x80014c0c lh  v0,8(a1) -> sw obj+0x10  (Bits 0xC = ABR, @0x8002cbc4)
            pos=(s16(b, 10), s16(b, 12), s16(b, 14)),   # @0x80014c18/24/30 -> sw obj+0x38/3c/40
            rot=(u16(b, 16), u16(b, 18), u16(b, 20)),   # @0x80014c3c/48/58 -> sh obj+0x74/76/78
        )
        fl = f["flags"]
        f["parent"] = (fl & 0xf) if (fl & 0x10) else None   # @0x80014c64 andi v0,v1,0x10 ; @0x80014c6c andi v0,v1,0xf
        f["close_se"] = bool(fl & 0x800)                    # @0x80014c90 andi v0,v1,0x800 -> work+0x248 = 1
    elif op == 0x2e:
        f = dict(kind=b[1], id=struct.unpack_from("<b", b, 2)[0])
    elif op == 0x2f:
        f = dict(index=b[1], value=s16(b, 2))
    elif op in (0x09,):
        f = dict(count=u16(b, 2))
    elif op == 0x0a:
        f = dict(count=u16(b, 1))
    elif op == 0x0d:
        f = dict(size=s16(b, 2), count=u16(b, 4))
    elif op == 0x04:
        f = dict(thread=b[1], inner=b[2], script=b[3])
    elif op == 0x18:
        f = dict(script=b[1])
    elif op == 0x13:
        f = dict(var=b[1], size=u16(b, 2))
    elif op == 0x14:
        f = dict(size=u16(b, 2), value=s16(b, 4))
    elif op in (0x06, 0x07):
        f = dict(size=u16(b, 2))
    elif op == 0x0f:
        f = dict(cond_len=b[1], size=s16(b, 2))
    elif op == 0x17:
        f = dict(ifel=b[1], loop=b[2], rel=s16(b, 4))
    elif op == 0x23:
        f = dict(var=b[2], cmp=b[3], value=s16(b, 4))
    elif op == 0x24:
        f = dict(var=b[1], value=s16(b, 2))
    elif op == 0x25:
        f = dict(dst=b[1], src=b[2])
    elif op == 0x26:
        f = dict(operator=b[2], var=b[3], value=s16(b, 4))
    elif op == 0x34:
        f = dict(member=b[1], value=s16(b, 2))
    elif op == 0x35:
        f = dict(member=b[1], var=b[2])
    elif op == 0x3d:
        f = dict(var=struct.unpack_from("<b", b, 1)[0], member=struct.unpack_from("<b", b, 2)[0])
    elif op == 0x36:
        f = dict(vab=b[1], se=s16(b, 2), work=s16(b, 4), pos=(s16(b, 6), s16(b, 8), s16(b, 10)))
    elif op == 0x53:
        f = dict(ch=b[1], kind=b[2], mask=b[3], step=s16(b, 4))
    elif op == 0x74:
        f = dict(ch=b[1], level=s16(b, 2))
    elif op == 0x8a:
        f = dict(a=u16(b, 2), b=u16(b, 4))
    elif op == 0x8b:
        f = dict(a=u16(b, 2), b=b[1], c=u16(b, 4))
    elif op == 0x8c:
        f = dict(a=u16(b, 4), b=b[2], c=b[3], d=u16(b, 6))
    elif op == 0x1d:
        f = dict(var=b[1], rel=b[2], byte=b[3])
    return dict(off=p, op=op, name=name, len=ln, raw=b.hex(" "), f=f)


def disassemble(code):
    """Linear zerlegen. Gibt (befehle, rest) zurueck; rest = Bytes nach dem letzten Befehl."""
    out, p = [], 0
    while p < len(code):
        # Nullbytes am Skriptende sind Ausrichtung, kein Code: nur gelten lassen, wenn bis zum
        # Ende ausschliesslich 0x00 folgt UND der letzte echte Befehl Evt_end war.
        if out and out[-1]["op"] == 0x01 and not any(code[p:]):
            return out, len(code) - p
        out.append(decode(code, p))
        p += out[-1]["len"]
    return out, 0


def fmt(c):
    f = c["f"]
    if c["op"] == 0x4d:
        return ("Door_model_set obj=%d id=%d frame=%d on=%d mesh=%d flags=0x%04x attr=0x%04x "
                "pos=(%d,%d,%d) rot=(%d,%d,%d) parent=%s" % (
                    f["obj"], f["id"], f["frame"], f["on"], f["mesh"], f["flags"], f["attr"] & 0xffff,
                    f["pos"][0], f["pos"][1], f["pos"][2], f["rot"][0], f["rot"][1], f["rot"][2],
                    "-" if f["parent"] is None else f["parent"]))
    return c["name"] + ("(" + ", ".join("%s=%s" % kv for kv in f.items()) + ")" if f else "")


# ---------------------------------------------------------------------------------------------
# Simulation der Sequenz (RE2): Door_move FUN_80013eb4 -> Scheduler FUN_80014058 -> Modell FUN_80014234
# ---------------------------------------------------------------------------------------------
class Thread:
    def __init__(self):
        self.active = False
        self.script = 0
        self.pc = 0
        self.calls = []          # Gosub-Ruecksprung (skript, pc, schleifenstapel)
        self.loops = []          # [zaehler, startpc, endepc]
        self.obj = None
        self.speed = [0] * 6     # thread+0x158.. (s16), 0..2 Position, 3..5 Drehung
        self.accel = [0] * 6     # thread+0x164..


def s16w(v):
    v &= 0xffff
    return v - 0x10000 if v & 0x8000 else v


class Sim:
    def __init__(self, do2, door_type, load_frames=0, dtex=0):
        self.do2 = do2
        # bytearray: 0x1d Work_copy schreibt in den Skripttext (selbstaendernder Code, @0x800542ec/0x80054304)
        self.codes = [bytearray(do2.script(k)[0]) for k in range(len(do2.offs))]
        self.th = {i: Thread() for i in range(10, 14)}
        self.objs = [None] * 10
        # Variablen: Tabelle 0x800d47ec, s16 je Index (@0x80054098 lh v1,18412(at)).
        # Door_init schreibt 0x800d4804/06/08/0a = Index 12/13/14/15:
        #   12 = door_type & 0x7f  (@0x80013e6c andi v0,v0,0xff7f ; @0x80013e74 sh 0x800d4804)
        #   13 = 1                 (@0x80013e60 addiu v1,zero,1  ; @0x80013e68 sh 0x800d4806)
        #   14 = door_type & 0x80  (@0x80013e84 andi v0,v0,0x80  ; @0x80013e8c sh 0x800d4808)
        #   15 = Tuertextur-Typ    (@0x80013e90 lbu v0,12(a0)    ; @0x80013e98 sh 0x800d480a)
        self.var = collections.defaultdict(int)
        self.var[12] = door_type & 0x7f
        self.var[13] = 1
        self.var[14] = door_type & 0x80
        self.var[15] = dtex
        self.load_frames = load_frames
        self.frame = 0
        self.events = []
        self.close_se = False
        self.fade = dict(level=None, step=0, kind=None)
        self.track = []
        self.start(10, 0)          # @0x80013e24 addu a1,zero,zero ; @0x80013e28 jal 0x800530ec (Skript 0)

    def start(self, tid, script):
        t = self.th[tid]
        t.active, t.script, t.pc = True, script, 0
        t.calls, t.loops = [], []
        t.speed, t.accel = [0] * 6, [0] * 6     # @0x80053218..28 (sechs Worte ab thread+0x158 geloescht)

    def step_thread(self, tid):
        t = self.th[tid]
        guard = 0
        while t.active:
            guard += 1
            if guard > 100000:
                raise RuntimeError("Endlosschleife ohne Evt_next in Thread %d" % tid)
            code = self.codes[t.script]
            c = decode(code, t.pc)
            op, f = c["op"], c["f"]
            nxt = t.pc + c["len"]
            if op in (0x00,):
                t.pc = nxt
            elif op == 0x01:
                if not t.calls:
                    t.active = False
                    return
                t.script, t.pc, t.loops = t.calls.pop()
            elif op == 0x02:
                t.pc = nxt
                return
            elif op == 0x04:
                t.pc = nxt
                self.events.append(dict(frame=self.frame, what="Evt_exec", thread=f["thread"], script=f["script"]))
                if f["thread"] in self.th:
                    self.start(f["thread"], f["script"])
                else:
                    # @0x800531ac sltiu v0,a0,0xe: Kennung >= 14 sucht den ersten freien Platz 10..13
                    for k in range(10, 14):
                        if not self.th[k].active:
                            self.start(k, f["script"])
                            break
            elif op == 0x09:
                t.loops.append([f["count"], None, None])
                t.pc += 1
            elif op == 0x0a:
                t.loops[-1][0] = (t.loops[-1][0] - 1) & 0xffff
                if t.loops[-1][0] == 0:
                    t.loops.pop()
                    t.pc = nxt
                return                                      # @0x80053a88 Rueckgabe 2 in jedem Fall
            elif op == 0x0d:
                if f["count"] == 0:
                    t.pc = nxt + f["size"]                 # @0x80053bb8..c8
                else:
                    t.loops.append([f["count"], nxt, None])
                    t.pc = nxt
            elif op == 0x0e:
                t.loops[-1][0] = (t.loops[-1][0] - 1) & 0xffff
                if t.loops[-1][0] != 0:
                    t.pc = t.loops[-1][1]
                else:
                    t.loops.pop()
                    t.pc = nxt
            elif op == 0x13:
                v = s16w(self.var[f["var"]])
                end = nxt + f["size"]
                p = nxt
                t.loops.append([0, None, end])
                while True:
                    o = code[p]
                    if o == 0x15:
                        p += 2
                        break
                    if o == 0x16:
                        p += 2
                        t.loops.pop()
                        break
                    if o != 0x14:
                        raise ValueError("Switch: 0x%02x statt Case @0x%x" % (o, p))
                    size, val = u16(code, p + 2), s16(code, p + 4)
                    p += 6
                    if val == v:
                        break
                    p += size
                t.pc = p
            elif op in (0x14, 0x15):
                t.pc = nxt
            elif op == 0x16:
                if t.loops:
                    t.loops.pop()
                t.pc = nxt
            elif op == 0x1a:
                end = t.loops.pop()[2]
                t.pc = end if end is not None else nxt
            elif op == 0x18:
                t.calls.append((t.script, nxt, t.loops))
                t.script, t.pc, t.loops = f["script"], 0, []
            elif op == 0x06:
                # Ifel_ck: Bedingungen folgen unmittelbar; alle muessen wahr sein, sonst Sprung
                p = nxt
                ok = True
                while code[p] in (0x23,):
                    cc = decode(code, p)
                    ok = ok and self.cmp(cc["f"])
                    p += cc["len"]
                t.pc = p if ok else nxt + f["size"]
            elif op == 0x07:
                t.pc = t.pc + f["size"]
            elif op == 0x08:
                t.pc = nxt
            elif op == 0x17:
                # @0x80054170 lbu v1,1(a1) -> sb v1,4(a2) (Ifel-Tiefe) ; @0x80054174 lbu a3,2(a1) ->
                # @0x8005419c sb a3,8(a2) (Schleifentiefe): 0xff = -1 = Stapel leer
                keep = 0 if f["loop"] == 0xff else f["loop"] + 1
                del t.loops[keep:]
                t.pc = t.pc + f["rel"]
            elif op == 0x24:
                self.var[f["var"]] = f["value"]
                t.pc = nxt
            elif op == 0x25:
                self.var[f["dst"]] = self.var[f["src"]]
                t.pc = nxt
            elif op == 0x2e:
                t.speed, t.accel = [0] * 6, [0] * 6        # @0x80055908..1c
                t.obj = f["id"] if f["kind"] == 5 else None  # Sprungtabelle @0x800111f0[4] = 0x800559b0
                t.pc = nxt
            elif op == 0x2f:
                i = f["index"]
                (t.speed if i < 6 else t.accel)[i % 6] = f["value"]
                t.pc = nxt
            elif op == 0x30:
                o = self.objs[t.obj] if t.obj is not None else None
                if o:
                    for k in range(3):
                        o["pos"][k] += t.speed[k]
                        o["rot"][k] = (o["rot"][k] + t.speed[3 + k]) & 0xffff
                t.pc = nxt
            elif op == 0x31:
                for k in range(6):
                    t.speed[k] = s16w(t.speed[k] + t.accel[k])
                t.pc = nxt
            elif op == 0x4d:
                self.objs[f["obj"]] = dict(mesh=f["mesh"], on=f["on"], flags=f["flags"], attr=f["attr"],
                                           pos=list(f["pos"]), rot=list(f["rot"]), parent=f["parent"],
                                           frame=f["frame"], id=f["id"])
                if f["close_se"]:
                    self.close_se = True
                self.events.append(dict(frame=self.frame, what="Door_model_set", obj=f["obj"], mesh=f["mesh"]))
                t.pc = nxt
            elif op == 0x36:
                self.events.append(dict(frame=self.frame, what="Se_on", vab=f["vab"], se=f["se"]))
                t.pc = nxt
            elif op == 0x53:
                # @0x80057f50 jal 0x8002c1a0 ; danach Pegel = step+0x8000, wenn step < 0 (@0x80057f94..a0)
                self.fade = dict(step=f["step"], kind=f["kind"], mask=f["mask"],
                                 level=(f["step"] + 0x8000) & 0xffff if f["step"] < 0 else 0)
                self.events.append(dict(frame=self.frame, what="Sce_fade_set", **f))
                t.pc = nxt
            elif op == 0x74:
                self.fade["level"] = f["level"] & 0xffff
                self.events.append(dict(frame=self.frame, what="Sce_fade_adjust", **f))
                t.pc = nxt
            elif op in (0x8a, 0x8b, 0x8c):
                self.events.append(dict(frame=self.frame, what=c["name"], **f))
                t.pc = nxt
            elif op == 0x0f:
                # While @0x80053d3c: Bedingungen liegen in den cond_len Bytes hinter dem Kopf,
                # Ende = Kopf+4+size (@0x80053dac addu a3,a1,a3); Ewhile springt auf den Kopf zurueck
                start, end = t.pc, nxt + f["size"]
                p, ok = nxt, None
                stop = nxt + f["cond_len"]
                while p < stop:
                    cc = decode(code, p)
                    r = self.cmp(cc["f"])
                    p += cc["len"]
                    if ok is None:
                        ok = r
                    if p < stop:
                        logic = u16(code, p)          # @0x80053fac lhu s0,0(a1): 0 = UND, sonst ODER
                        p += 2
                        cc = decode(code, p)
                        r2 = self.cmp(cc["f"])
                        p += cc["len"]
                        ok = (ok or r2) if logic else (ok and r2)
                if ok:
                    t.loops.append([0, start, end])
                    t.pc = p
                else:
                    t.pc = end
            elif op == 0x10:
                t.pc = t.loops.pop()[1]
            elif op == 0x26:
                v = s16w(self.var[f["var"]])
                if f["operator"] == 0:
                    v = v + f["value"]                # Sprungtabelle @0x800111c0[0] = 0x80054658 (addu)
                elif f["operator"] == 1:
                    v = v - f["value"]                # [1] = 0x80054664 (subu)
                else:
                    raise NotImplementedError("Calc-Operator %d" % f["operator"])
                self.var[f["var"]] = s16w(v)
                t.pc = nxt
            elif op == 0x1d:
                v = self.var[f["var"]] & 0xffff
                dst = nxt + f["rel"]
                if f["byte"]:
                    code[dst:dst + 2] = struct.pack("<H", v)   # @0x800542ec sh v1,0(v0)
                else:
                    code[dst] = v & 0xff                       # @0x80054304 sb v1,0(v0)
                t.pc = nxt
            elif op in (0x34, 0x35):
                val = f["value"] if op == 0x34 else s16w(self.var[f["var"]])
                o = self.objs[t.obj] if t.obj is not None else None
                self.member_set(o, f["member"], val)
                t.pc = nxt
            elif op == 0x3d:
                o = self.objs[t.obj] if t.obj is not None else None
                self.var[f["var"]] = s16w(self.member_get(o, f["member"]))
                t.pc = nxt
            elif op == 0x05:
                raise NotImplementedError("Evt_kill in der Simulation nicht nachgebaut")
            elif op == 0x03:
                raise NotImplementedError("Evt_chain in der Simulation nicht nachgebaut")
            elif op == 0x23:
                # freistehendes Cmp: Rueckgabe 0/1 des Handlers ist der Scheduler-Code; ausserhalb
                # von Ifel_ck/While kommt das in den Tuerskripten nicht vor
                raise NotImplementedError("freistehendes Cmp")
            else:
                raise NotImplementedError("Opcode 0x%02x" % op)

    # Member-Tabellen: Setzen @0x80011228 (Ziele ab 0x80055cd4, je 8 B), Lesen @0x800112f8
    # (Ziele ab 0x80055f78, je 12 B). Belegt sind hier nur die in Tuerskripten benutzten:
    #   10 -> obj+0x10 (attr)   11/12/13 -> obj+0x38/3c/40 (Position)   14/15/16 -> obj+0x74/76/78 (Drehung)
    def member_set(self, o, m, val):
        if o is None:
            return
        if m == 10:
            o["attr"] = val
        elif 11 <= m <= 13:
            o["pos"][m - 11] = val
        elif 14 <= m <= 16:
            o["rot"][m - 14] = val & 0xffff
        else:
            raise NotImplementedError("Member %d" % m)

    def member_get(self, o, m):
        if o is None:
            return 0
        if m == 10:
            return o["attr"]
        if 11 <= m <= 13:
            return o["pos"][m - 11]
        if 14 <= m <= 16:
            return o["rot"][m - 14]
        raise NotImplementedError("Member %d" % m)

    def cmp(self, f):
        a, b = s16w(self.var[f["var"]]), f["value"]
        # Sprungtabelle @0x800111a0, Faelle @0x800544cc..0x80054510
        return [a == b, a > b, a >= b, a < b, a <= b, a != b, (a & b) != 0][f["cmp"]]

    def fade_tick(self):
        # FUN_8002c378: Helligkeit = Pegel >> 7 (@ sra 0x17 nach sll 0x10), dann Pegel += Schritt
        fd = self.fade
        if fd["level"] is None or fd["level"] & 0x8000:
            return None
        b = fd["level"] >> 7
        fd["level"] = (fd["level"] + fd["step"]) & 0xffff
        return b

    def run(self, max_frames=4000):
        while self.th[10].active and self.frame < max_frames:
            if self.frame >= self.load_frames:
                self.var[13] = 0       # @0x80013f54..68: Ladebit 0x20000 geloescht -> sh zero,0x800d4806
            for tid in range(10, 14):  # @0x80014068 addiu s2,zero,10 .. @0x80014150 sltiu v0,s2,0xe
                if self.th[tid].active:
                    self.step_thread(tid)
            self.track.append(dict(frame=self.frame, fade=self.fade_tick(),
                                   objs=[None if o is None else dict(pos=list(o["pos"]), rot=list(o["rot"]))
                                         for o in self.objs]))
            self.frame += 1            # @0x8001401c addiu v1,v1,1 ; @0x80014024 sh v1,558(v0)
        return self.frame


# ---------------------------------------------------------------------------------------------
# Geometrie: Kamera FUN_80076cb0, Objektmatrix FUN_80014234, RotMatrix 0x8008e1f4
# ---------------------------------------------------------------------------------------------
def rot_matrix(rx, ry, rz):
    """PsyQ RotMatrix (0x8008e1f4): M = Rx * Ry * Rz, 4096 = 360 Grad."""
    a = [v * 2.0 * math.pi / 4096.0 for v in (rx, ry, rz)]
    sx, cx, sy, cy, sz, cz = math.sin(a[0]), math.cos(a[0]), math.sin(a[1]), math.cos(a[1]), math.sin(a[2]), math.cos(a[2])
    return [[cy * cz, -cy * sz, sy],
            [sz * cx + cz * sy * sx, cz * cx - sz * sy * sx, -cy * sx],
            [sz * sx - cz * sy * cx, cz * sx + sz * sy * cx, cy * cx]]


def mul(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(3)) for j in range(3)] for i in range(3)]


def app(m, v):
    return [sum(m[i][k] * v[k] for k in range(3)) for i in range(3)]


def camera(eye, target):
    """FUN_80076cb0: erst Neigung, dann Gier; t = M * (-eye)."""
    dx, dy, dz = (target[i] - eye[i] for i in range(3))
    l3 = math.sqrt(dx * dx + dy * dy + dz * dz)
    l2 = math.sqrt(dx * dx + dz * dz)
    m = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
    c, s = l2 / l3, dy / l3
    m = mul(m, [[1, 0, 0], [0, c, -s], [0, s, c]])
    if l2:
        c, s = dz / l2, dx / l2
        m = mul(m, [[c, 0, -s], [0, 1, 0], [s, 0, c]])
    t = app(m, [-eye[0], -eye[1], -eye[2]])
    return m, t


def world(objs, i, cam, memo=None):
    """Zusammengesetzte Matrix eines Objekts (FUN_80014234): Eltern * lokal, t = Eltern*pos + Eltern.t"""
    o = objs[i]
    pm, pt = cam if o["parent"] is None or objs[o["parent"]] is None else world(objs, o["parent"], cam)
    lm = rot_matrix(*o["rot"])
    m = mul(pm, lm)
    # @0x800145d8..e8: die Position geht als 16-Bit-Vektor in die GTE (lhu/lwc2) -> s16
    p = [s16w(v) for v in o["pos"]]
    t = [a + b for a, b in zip(app(pm, p), pt)]
    return m, t


def project(v, h):
    if v[2] <= 0:
        return None
    return (OFX + v[0] * h / v[2], OFY + v[1] * h / v[2], v[2])


def screen_boxes(objs, meshes, eye, target, h):
    cam = camera(eye, target)
    out = []
    for i, o in enumerate(objs):
        if o is None or not o.get("on", 1):
            out.append(None)
            continue
        m, t = world(objs, i, cam)
        pts = []
        for v in meshes[o["mesh"]]["verts"]:
            w = [a + b for a, b in zip(app(m, v), t)]
            p = project(w, h)
            if p:
                pts.append(p)
        if not pts:
            out.append(None)
            continue
        out.append(dict(x=[round(min(p[0] for p in pts), 1), round(max(p[0] for p in pts), 1)],
                        y=[round(min(p[1] for p in pts), 1), round(max(p[1] for p in pts), 1)],
                        z=[round(min(p[2] for p in pts)), round(max(p[2] for p in pts))]))
    return out


# ---------------------------------------------------------------------------------------------
# Kommandos
# ---------------------------------------------------------------------------------------------
def cmd_container(path):
    a = Do2(path)
    print("%s  %d B  Art=%s" % (path, len(a.d), a.kind))
    print("  Teil2 @0x%x  MD1 @0x%x  SCD-Tabelle @0x%x  TIM @0x%x  Skripte=%d" % (
        a.part2, a.md1, a.scd, a.tim, len(a.offs)))
    for m in a.meshes():
        print("  Mesh %d: %d Ecken, %d Dreiecke, %d Vierecke, bbox %s" % (
            m["index"], m["vert_count"], m["tri_count"], m["quad_count"], m["bbox"]))


def cmd_dis(path, which=None):
    a = Do2(path)
    for k in range(len(a.offs)):
        if which is not None and k != which:
            continue
        code, fo = a.script(k)
        print("--- Skript %d  @Datei 0x%x  %d B" % (k, fo, len(code)))
        cs, rest = disassemble(code)
        for c in cs:
            print("  %04x  %-66s ; %s" % (c["off"], c["raw"], fmt(c)))
        if rest:
            print("  (+%d Nullbyte Ausrichtung)" % rest)


def dispatch_map(a):
    """Skript 0 lesen: Switch(var) -> {wert: gosub-skript}"""
    cs, _ = disassemble(a.script(0)[0])
    out, var, cur = {}, None, []
    for c in cs:
        if c["op"] == 0x13:
            var = c["f"]["var"]
        elif c["op"] == 0x14:
            # Case mit size=0 faellt in den naechsten Case durch: der Handler 0x800540f8 schiebt
            # den PC nur um 6 weiter, also sammeln, bis ein Gosub kommt
            cur.append(c["f"]["value"])
        elif c["op"] == 0x15:
            cur.append("default")
        elif c["op"] == 0x18 and cur:
            for v in cur:
                out.setdefault(v, c["f"]["script"])
            cur = []
        elif c["op"] == 0x1a:
            cur = []
    return var, out


def collect(dirname):
    res = dict(archives=[], histogram=collections.Counter())
    for path in sorted(glob.glob(os.path.join(dirname, "DOOR*.DO2"))):
        a = Do2(path)
        ent = dict(file=os.path.basename(path), size=len(a.d), part2=a.part2, md1=a.md1, tim=a.tim,
                   scripts=len(a.offs), meshes=[dict(index=m["index"], verts=m["vert_count"],
                                                     tris=m["tri_count"], quads=m["quad_count"],
                                                     bbox=m["bbox"]) for m in a.meshes()],
                   sets=[], padding=0)
        # Tonkopf: 16 B = vier Eintraege je 4 B am Dateianfang; 0xffffffff = leer
        # (FUN_8005ba28: @ pbVar9 = Kopf + se*4, Abbruch bei -1). FUN_80014cd0 legt den Kopf nach
        # 0x801fb700 (DAT_800dbb78).
        ent["sound_entries"] = [a.d[i * 4:i * 4 + 4].hex(" ") for i in range(4)]
        ent["sounds"] = sum(1 for i in range(4) if a.d[i * 4:i * 4 + 4] != b"\xff\xff\xff\xff")
        for k in range(len(a.offs)):
            cs, rest = disassemble(a.script(k)[0])
            ent["padding"] += rest
            for c in cs:
                res["histogram"][c["op"]] += 1
                if c["op"] == 0x4d:
                    ent["sets"].append(dict(script=k, off=c["off"], **c["f"]))
        var, mp = dispatch_map(a)
        ent["switch_var"] = var
        ent["dispatch"] = {str(k): v for k, v in mp.items()}
        res["archives"].append(ent)
    return res


def cmd_stat(dirname):
    r = collect(dirname)
    print("Archive: %d   Skripte: %d" % (len(r["archives"]), sum(a["scripts"] for a in r["archives"])))
    tot = sum(r["histogram"].values())
    for op, n in sorted(r["histogram"].items(), key=lambda kv: -kv[1]):
        print("  0x%02x %-16s %5d  %5.1f %%   %s" % (op, OPS[op][0], n, 100.0 * n / tot, OPS[op][3]))
    print("  Summe %d Befehle" % tot)
    sets = [s for a in r["archives"] for s in a["sets"]]
    print("Door_model_set: %d" % len(sets))
    print("  Flag-Bits (Anzahl Saetze):")
    for bit in range(16):
        n = sum(1 for s in sets if s["flags"] & (1 << bit))
        if n:
            print("    0x%04x: %d" % (1 << bit, n))
    print("  mit Elternmatrix (0x10): %d" % sum(1 for s in sets if s["parent"] is not None))
    print("  attr-Werte: %s" % dict(collections.Counter(s["attr"] & 0xffff for s in sets)))
    print("  Skript-0-Variable: %s" % dict(collections.Counter(a["switch_var"] for a in r["archives"])))


def simulate(a, door_type, load_frames=0, eye=RE2_EYE, target=RE2_TARGET, h=RE2_H):
    s = Sim(a, door_type, load_frames)
    n = s.run()
    meshes = a.meshes()
    first = None
    for tr in s.track:
        if any(o is not None for o in tr["objs"]):
            first = tr
            break
    def boxes(tr):
        objs = []
        for o, st in zip(s.objs, tr["objs"]):
            if o is None or st is None:
                objs.append(None)
            else:
                q = dict(o)
                q["pos"], q["rot"] = st["pos"], st["rot"]
                objs.append(q)
        return screen_boxes(objs, meshes, eye, target, h)
    return s, n, (boxes(first) if first else None), (boxes(s.track[-1]) if s.track else None)


def cmd_sim(path, door_type, load_frames=0):
    a = Do2(path)
    s, n, b0, b1 = simulate(a, door_type, load_frames)
    print("%s door_type=%d: %d Bilder bis Thread 10 endet%s" % (
        os.path.basename(path), door_type, n, "" if not s.th[10].active else " (ABBRUCH, Thread laeuft noch)"))
    for e in s.events:
        print("  Bild %4d  %s" % (e["frame"], {k: v for k, v in e.items() if k != "frame"}))
    print("  Schluss-Ton beim Verlassen (Flag 0x800): %s" % s.close_se)
    print("  Bildrechtecke erstes Bild:", b0)
    print("  Bildrechtecke letztes Bild:", b1)
    last = None
    for tr in s.track:
        key = json.dumps(tr["objs"])
        if key != last:
            print("  Bild %4d fade=%s %s" % (tr["frame"], tr["fade"],
                                            [None if o is None else (o["pos"], o["rot"]) for o in tr["objs"]]))
            last = key


STD_LEAF = (288, 6602, 3599)      # Mesh 0 von DOOR00: x[-145,143] y[-6600,2] z[-3599,0]


def _size(b):
    return (b["x"][1] - b["x"][0], b["y"][1] - b["y"][0], b["z"][1] - b["z"][0])


def summary(r, dirname):
    ok = [s for s in r["simulations"] if "error" not in s]
    fr = sorted(s["frames"] for s in ok)
    arch = {a["file"]: a for a in r["archives"]}
    std_files = [a["file"] for a in r["archives"] if a["meshes"] and _size(a["meshes"][0]["bbox"]) == STD_LEAF]
    boxes = collections.Counter()
    for s in ok:
        if s["file"] in std_files and s["box_first"] and s["box_first"][0]:
            b = s["box_first"][0]
            boxes[(b["x"][0], b["x"][1], b["y"][0], b["y"][1], b["z"][0], b["z"][1])] += 1
    se = collections.Counter()
    for s in ok:
        f = [e["frame"] for e in s["events"] if e["what"] == "Se_on"]
        se[f[0] if f else None] += 1
    fin = collections.Counter()
    fout = collections.Counter()
    for s in ok:
        for e in s["events"]:
            if e["what"] == "Sce_fade_set":
                (fin if e["step"] < 0 else fout)[e["step"]] += 1
    swing = []
    for typ in (0, 1):
        a = Do2(os.path.join(dirname, "DOOR00.DO2"))
        sm = Sim(a, typ)
        sm.run()
        ry = [tr["objs"][0]["rot"][1] for tr in sm.track]
        ch = [i for i in range(1, len(ry)) if ry[i] != ry[i - 1]]
        edge = app(rot_matrix(0, ry[ch[-1]], 0), [0, 0, -3599])
        swing.append(dict(door_type=typ, rot_y_start=ry[0], rot_y_end=ry[ch[-1]], first_frame=ch[0],
                          last_frame=ch[-1], free_edge_rel_hinge=[round(v) for v in edge]))
    return dict(
        simulations=len(ok),
        frames=dict(min=fr[0], median=fr[len(fr) // 2], max=fr[-1],
                    histogram={str(k): v for k, v in sorted(collections.Counter(fr).items())}),
        standard_leaf=dict(size=STD_LEAF, archives=len(std_files), files=std_files,
                           first_frame_boxes=[dict(x=[k[0], k[1]], y=[k[2], k[3]], z=[k[4], k[5]],
                                                   width_px=round(k[1] - k[0], 1), height_px=round(k[3] - k[2], 1),
                                                   count=v) for k, v in boxes.most_common()]),
        se_on_first_frame={str(k): v for k, v in sorted(se.items(), key=lambda kv: (kv[0] is None, kv[0]))},
        fade_in_steps={str(k): v for k, v in fin.items()},
        fade_out_steps={str(k): v for k, v in fout.items()},
        swing_door00=swing,
        px_per_unit_at_depth_8000=RE2_H / 8000.0,
    )


def re15_block(path, dirname):
    a = Do2(path)
    code, fo = a.script(0)
    cs, rest = disassemble(code)
    # dieselbe Aufstellung wie RE2 DOOR00 Typ 0, aber durch die RE1.5-Kamera gesehen:
    # H = 1000 aus InitGeom (@0x80066c88 addiu t0,zero,1000 ; @0x80066c8c ctc2 -> H), Door_init ruft
    # @0x80016434 jal 0x80066c40 und danach KEIN SetGeomScreen (0x80066c30 hat nur die Aufrufer
    # 0x80021e6c und 0x80046124).
    r2 = Do2(os.path.join(dirname, "DOOR00.DO2"))
    s, n, b0, b1 = simulate(r2, 0, eye=RE15_EYE, target=RE15_TARGET, h=1000)
    return dict(file=path, size=len(a.d), md1=a.md1, scd=a.scd, tim=a.tim, scripts=len(a.offs),
                script0=dict(file_offset=fo, bytes=code.hex(" "), ops=[c["name"] for c in cs]),
                meshes=[dict(index=m["index"], verts=m["vert_count"], tris=m["tri_count"],
                             quads=m["quad_count"], bbox=m["bbox"]) for m in a.meshes()],
                camera=dict(eye=RE15_EYE, target=RE15_TARGET, h=1000, offset=[OFX, OFY]),
                frames_until_thread10_ends=1,
                re2_door00_type0_seen_by_re15_camera=b0)


def cmd_json(dirname, out, re15_path=None):
    r = collect(dirname)
    r["histogram"] = {"0x%02x" % op: dict(name=OPS[op][0], count=n, length=OPS[op][1],
                                          handler="0x%08x" % OPS[op][2], evidence=OPS[op][3])
                      for op, n in sorted(r["histogram"].items())}
    r["camera"] = dict(re2=dict(eye=RE2_EYE, target=RE2_TARGET, h=RE2_H, offset=[OFX, OFY]),
                       re15=dict(eye=RE15_EYE, target=RE15_TARGET, h=1000, offset=[OFX, OFY]))
    sims = []
    for ent in r["archives"]:
        a = Do2(os.path.join(dirname, ent["file"]))
        for key, script in sorted(ent["dispatch"].items()):
            if key == "default":
                continue
            try:
                s, n, b0, b1 = simulate(a, int(key))
                sims.append(dict(file=ent["file"], door_type=int(key), script=script, frames=n,
                                 finished=not s.th[10].active, events=s.events, close_se=s.close_se,
                                 box_first=b0, box_last=b1,
                                 objs_first=[o for o in (s.track[0]["objs"] if s.track else [])],
                                 objs_last=[o for o in (s.track[-1]["objs"] if s.track else [])]))
            except NotImplementedError as e:
                sims.append(dict(file=ent["file"], door_type=int(key), script=script, error=str(e)))
    r["simulations"] = sims
    r["summary"] = summary(r, dirname)
    if re15_path:
        r["re15"] = re15_block(re15_path, dirname)
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with open(out, "w", encoding="utf-8") as fh:
        json.dump(r, fh, indent=1)
    print("geschrieben:", out, "Archive", len(r["archives"]), "Simulationen", len(sims))


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    c = argv[1]
    if c == "container":
        cmd_container(argv[2])
    elif c == "dis":
        cmd_dis(argv[2], int(argv[3]) if len(argv) > 3 else None)
    elif c == "stat":
        cmd_stat(argv[2])
    elif c == "sim":
        cmd_sim(argv[2], int(argv[3], 0), int(argv[4]) if len(argv) > 4 else 0)
    elif c == "json":
        cmd_json(argv[2], argv[3], argv[4] if len(argv) > 4 else None)
    else:
        print(__doc__)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
