#!/usr/bin/env python3
"""Runde 30 / tuer-verschlossen: RE2-SCD direkt aus der RDT laufen.
Laengentabelle: tools/re2_sicherung/re2_scd_walk.py (126 Laengen aus den 143 Handlern ab
0x800A74C8, 10 einzeln disassemblierte Overrides, 7 aus information293.txt).
RDT-Kopf: Offsetindex 16 (@Datei 0x48) = main-SCD, 17 (@0x4C) = sub-SCD; jeder Block
beginnt mit einer u16-Offsettabelle (erster Offset = Tabellengroesse).
Satzlagen (selbst disassembliert):
  Se_on 0x36 @0x80056428: a0 = pc[1]<<24 | pc[2]<<16 | pc[3]; pc[4] = Bezugsobjekt (0..4),
        pc[6..11] = s16 x,y,z                                  -> jal 0x8005ba28 @0x80056530
  Door_aot_se 0x3B @0x80054be4: Satz = pc+2, pc += 32; 0x68 (4p) @0x80054c50: pc += 40, sat |= 0x80
  Tuer-Handler @0x80051514 liest Nutzlast+15 = key_id, +16 = key_type
        (Nutzlast = Satz+12 bzw. Satz+20 bei 4p; @0x80051498 / FUN_80051088)
"""
import struct, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'tools', 're2_sicherung'))
_cwd = os.getcwd(); os.chdir(REPO)
import re2_scd_walk as W
os.chdir(_cwd)
NAMES = dict(W.NAMES)
NAMES.update({0x00:'Nop',0x01:'Evt_end',0x02:'Evt_next',0x03:'Evt_chain',0x04:'Evt_exec',0x05:'Evt_kill',
 0x06:'Ifel_ck',0x07:'Else_ck',0x08:'Endif',0x09:'Sleep',0x0A:'Sleeping',0x0B:'Wsleep',0x0C:'Wsleeping',
 0x0D:'For',0x0E:'Next',0x0F:'While',0x10:'Ewhile',0x11:'Do',0x12:'Edwhile',0x13:'Switch',0x14:'Case',
 0x15:'Default',0x16:'Eswitch',0x17:'Goto',0x18:'Gosub',0x19:'Return',0x1A:'Break',0x1B:'For2',
 0x1C:'Break_point',0x1D:'Work_copy',0x1E:'Nop1E',0x1F:'Nop1F'})
LENS = W.LENS
def rdt_path(room):
    return os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT', 'ROOM%s.RDT' % room)
def sections(d):
    offs = list(struct.unpack_from('<23I', d, 8))
    return offs
def blocks(d, which):
    """which: 16 = main, 17 = sub. -> [(index, datei_start, datei_ende)]"""
    offs = sections(d)
    base = offs[which]
    if base == 0: return []
    bigger = sorted(o for o in offs if o > base) + [len(d)]
    end = bigger[0]
    first = struct.unpack_from('<H', d, base)[0]
    n = first // 2
    tab = [struct.unpack_from('<H', d, base + i*2)[0] for i in range(n)]
    out = []
    for i, o in enumerate(tab):
        e = tab[i+1] if i + 1 < n else (end - base)
        out.append((i, base + o, base + e))
    return out
def walk(d, s, e):
    out = []; p = s
    while p < e:
        op = d[p]; n = LENS.get(op)
        if n is None or p + n > e:
            return out, 'desync@0x%X op=0x%02X' % (p, op)
        out.append((p, op, d[p:p+n]))
        p += n
        if op == 0x01 and p < e:
            # Evt_end: alles dahinter ist Fuellung/Folgedaten bis zum naechsten Block
            rest = d[p:e]
            if all(b == 0 for b in rest): return out, 'ok'
            # nicht abbrechen: weitere Pfade koennen folgen (Evt_end mitten im Block)
    return out, 'ok'
def door_fields(r, op):
    base = 14 if op == 0x3B else 22
    nx, ny, nz, nd = struct.unpack_from('<4h', r, base)
    st, rm, cut, nfl, dtex, dtype, knock, key_id, key_type, free = r[base+8:base+18]
    return dict(aot=r[1], sce=r[2], sat=r[3], nfloor=r[4], super=r[5],
                rect=struct.unpack_from('<4h', r, 6) if op == 0x3B else struct.unpack_from('<8h', r, 6),
                next=(nx, ny, nz, nd), stage=st, room=rm, cut=cut, dtex=dtex, dtype=dtype, knock=knock,
                key_id=key_id, key_type=key_type, free=free)
def fmt(off, op, r):
    nm = NAMES.get(op, 'op%02X' % op)
    s = '  @0x%05X  %02X %-16s %s' % (off, op, nm, r.hex(' '))
    if op == 0x36:
        s += '   ; Se_on(0x%02x%02x00%02x) bank %d id 0x%02x bezug %d pos(%d,%d,%d)' % (
            r[1], r[2], r[3], r[1], r[2], r[4], *struct.unpack_from('<3h', r, 6))
    elif op in (0x3B, 0x68):
        f = door_fields(r, op)
        s += '   ; aot %d sce %d sat 0x%02x -> stage %d room 0x%02x cut %d  dtype %d knock %d key_id 0x%02x key_type 0x%02x' % (
            f['aot'], f['sce'], f['sat'], f['stage'], f['room'], f['cut'], f['dtype'], f['knock'], f['key_id'], f['key_type'])
    elif op in (0x2C, 0x67):
        pay = r[14:] if op == 0x2C else r[22:]
        s += '   ; aot %d sce %d sat 0x%02x nutzlast %s' % (r[1], r[2], r[3], pay.hex(' '))
    elif op == 0x2B:
        s += '   ; msg %d' % r[2]
    elif op == 0x21:
        s += '   ; Ck(%d, 0x%02x) == %d' % (r[1], r[2], r[3])
    elif op == 0x22:
        s += '   ; Set(%d, 0x%02x, %d)' % (r[1], r[2], r[3])
    elif op == 0x46:
        s += '   ; aot %d sce %d sat 0x%02x daten %s' % (r[1], r[2], r[3], r[4:].hex(' '))
    elif op == 0x18:
        s += '   ; Gosub sub%02d' % r[1]
    elif op == 0x04:
        s += '   ; Evt_exec %d -> sub%02d' % (r[1], r[3])
    return s
if __name__ == '__main__':
    room = sys.argv[1]
    only = sys.argv[2:]  # z.B. main00 sub03
    d = open(rdt_path(room), 'rb').read()
    for which, nm in ((16, 'main'), (17, 'sub')):
        for i, s, e in blocks(d, which):
            tag = '%s%02d' % (nm, i)
            if only and tag not in only: continue
            recs, st = walk(d, s, e)
            print('== ROOM%s %s @Datei 0x%05X..0x%05X (%d B) %s' % (room, tag, s, e, e - s, st))
            for off, op, r in recs:
                print(fmt(off, op, r))
