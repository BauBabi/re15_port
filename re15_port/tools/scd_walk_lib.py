#!/usr/bin/env python3
"""scd_walk_lib.py - der SCD-Walker als importierbare Bibliothek, OHNE eigene
Laengentabelle.

Warum es diese Datei gibt: mehrere Mess-Skripte brauchen denselben opcode-exakten
Walk. Eine zweite Laengentabelle waere eine zweite Wahrheit, die still auseinander
driftet. Deshalb wird hier die EINE vorhandene Tabelle aus `scd_dump_room.py`
weiterverwendet (die wiederum aus `engine/src/scd_vm.c s_opcode_sizes` stammt plus
den disasm-verifizierten variablen Laengen 0x2C/0x3B/0x50/0x2D).

`scd_dump_room.py` ruft am Dateiende `main()` auf und ist damit nicht importierbar.
Statt die Datei zu aendern (sie ist gemeinsam genutzt), wird ihre Quelle hier
eingelesen, der Aufruf entfernt und der Rest ausgefuehrt - die Tabelle bleibt eine
einzige. Faellt der Aufruf-Fund weg, bricht der Import mit AssertionError ab, statt
stillschweigend etwas anderes zu tun.

Zusaetzlich liefert diese Datei den Nachrichten-Dekoder (Glyphentabelle = die des
Ports, `engine/src/msg_common.c:175-199`).

Exportiert: SIZES, NAMES, u16, s16, u32, op_size, fwd_target, rdt_section_end,
            section_regions, walk_ops, regionen, messages, glyph
"""
import os, sys, types, struct

_P = os.path.join(os.path.dirname(os.path.abspath(__file__)), "scd_dump_room.py")
_SRC = open(_P, encoding="utf-8").read()
assert "\nmain()\n" in _SRC, \
    "scd_dump_room.py ruft am Ende nicht mehr `main()` - Import-Trick pruefen"
_MOD = types.ModuleType("scd_dump_room_lib")
_MOD.__file__ = _P
exec(compile(_SRC.replace("\nmain()\n", "\n"), _P, "exec"), _MOD.__dict__)

SIZES           = _MOD.SIZES
NAMES           = _MOD.NAMES
u16             = _MOD.u16
s16             = _MOD.s16
u32             = _MOD.u32
op_size         = _MOD.op_size
fwd_target      = _MOD.fwd_target
rdt_section_end = _MOD.rdt_section_end
section_regions = _MOD.section_regions


def walk_ops(d, start, end):
    """(pc, op, size) in Skriptreihenfolge. STOPPT bei Desync - ein desynchronisierter
    Walk ist kein Befund (dieselbe Regel wie scd_dump_room.dump)."""
    pc, maxf, res = start, start, []
    while pc < end:
        op = d[pc]
        sz = op_size(d, pc)
        if sz is None or pc + sz > end:
            break
        res.append((pc, op, sz))
        t = fwd_target(d, pc, op)
        if t is not None and t > maxf:
            maxf = t
        if op == 0x01 and pc + 2 > maxf:
            break
        pc += sz
    return res


def regionen(d):
    """{(tag, idx): [(pc, op, size), ...]} fuer main- und sub-SCD."""
    out = {}
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d):
            continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            out[(tag, idx)] = walk_ops(d, sec + o, sec + e)
    return out


# ---- Nachrichten-Block (RDT+0x3C). Glyphentabelle = engine/src/msg_common.c:175-199 ----
def glyph(b):
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
    out, ctrl, i = [], [], s
    while i < e and i < len(d):
        b = d[i]
        if b == 0x01:
            ctrl.append("01"); break
        if b in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            ctrl.append("%02x:%02x" % (b, d[i + 1])); i += 2; continue
        if b in (0x03, 0x07):
            ctrl.append("%02x" % b); i += 1; continue
        if b == 0x08:
            out.append(" / "); i += 1; continue
        t = glyph(b)
        if t is None: ctrl.append("?%02x" % b)
        else:         out.append(t)
        i += 1
    return "".join(out), ctrl


def messages(d):
    """{msg_id: (text, [steuercodes])} - leer, wenn der Block fehlt/unplausibel ist."""
    ms = u32(d, 0x3C)
    if ms == 0 or ms >= len(d) or ms + 2 > len(d):
        return {}
    first = u16(d, ms)
    if first < 2 or first % 2 or ms + first > len(d):
        return {}
    n = first // 2
    tbl = [u16(d, ms + 2 * i) for i in range(n)]
    out = {}
    for idx, o in enumerate(tbl):
        st = ms + o
        en = ms + tbl[idx + 1] if idx + 1 < n else len(d)
        if st >= len(d):
            continue
        out[idx] = decode(d, st, min(en, st + 400))
    return out


if __name__ == "__main__":
    print("scd_walk_lib: %d Opcode-Laengen, %d Namen (Quelle: %s)"
          % (len(SIZES), len(NAMES), _P))
