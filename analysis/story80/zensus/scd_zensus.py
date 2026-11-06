#!/usr/bin/env python3
"""ST80-P Story-Zensus: SCD-Walk ueber ALLE RDTs eines Baums (Port/Mod oder 40%-Disc).

Je RDT: Regionen (main/sub), statische Erreichbarkeit der subs, NPC-Sce_em_set (Typ 0x40..0x4F),
Textaufrufe (Message_on, Aot_set sce 1, Aot_reset sce 1) mit Bedingungs-Stack, Ereignis-Kanten.

Opcode-Laengen/Spruenge: re15_port/tools/scd_walk_lib.py (EINE Tabelle, aus scd_vm.c s_opcode_sizes).
Feld-Layouts (Port-VM, gegen die Original-Disasm kommentiert):
  Message_on 0x2B   msg = pc[1]                       scd_vm.c op_message_on  (@0x80040504 lbu a2,1(v0))
  Aot_set    0x2C   slot pc[1], sce pc[2], Nutzlast pc[14..] kurz / pc[22..] lang (pc[3]&0x80)
                    sce 1 = Text: msg = u16 Nutzlast+0;  sce 3 = Ereignis: sub = Nutzlast+3 (pc[17]/pc[25])
  Aot_reset  0x46   slot pc[1], sce pc[2], Nutzlast pc[4..9]  (@0x80040788-a8)
  Evt_exec   0x04   sub = pc[3]   (op_evt_exec);  Evt_chain 0x03 sub = pc[3];  Gosub 0x18 sub = pc[1]
  Sce_em_set 0x44   slot pc[1], Typ pc[2], x pc[8], y pc[10], z pc[12], Richtung pc[16]
Erreichbarkeit (statisch, wie analysis/story_begegnungen/werkzeuge/k12_subs.py): Start main*, sub00, sub01
(@0x8003efb0/@0x8003efd4/@0x8003f080), Kanten Evt_chain/Evt_exec/Gosub/AOT-sce-3.
Port-C-Ausloeser (scd_event_fire usw.) sind NICHT hier, sie stehen im Dossier je Raum.

Aufruf: python scd_zensus.py <PSX-Wurzel> <ausgabe.json>
"""
import sys, os, glob, json, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as L
u16, s16, u32, op_size, fwd_target = L.u16, L.s16, L.u32, L.op_size, L.fwd_target

COND_OPS = (0x21, 0x23, 0x3E, 0x51, 0x4F, 0x5E, 0x28)

def cond_text(d, pc):
    op = d[pc]
    if op == 0x21: return 'Ck(%d,%d)=%d' % (d[pc+1], d[pc+2], d[pc+3])
    if op == 0x23: return 'Cmp(V[%d] op%d %d)' % (d[pc+2], d[pc+3], s16(d, pc+4))
    if op == 0x3E: return 'Member_cmp(%s)' % d[pc:pc+6].hex()
    if op == 0x51: return 'Sce_key_ck(%s)' % d[pc+1:pc+4].hex()
    if op == 0x4F: return 'Op4F'
    if op == 0x5E: return 'Keep_Item_ck(%d,%d,%d)' % (d[pc+1], d[pc+2], d[pc+3])
    if op == 0x28: return 'Sce_rnd'
    return 'op%02x' % op

def walk(d, start, end):
    """linear wie scd_walk_lib.walk_ops, plus Bedingungs-Stack aus Ifel_ck/Else_ck."""
    pc, maxf, ev, st = start, start, [], []
    while pc < end:
        while st and pc >= st[-1][0]: st.pop()
        op = d[pc]; sz = op_size(d, pc)
        if sz is None or pc + sz > end:
            ev.append((pc, -1, '', list(x[1] for x in st))); break
        if op == 0x06:
            size = u16(d, pc+2); q = pc+4; c = []
            while q < pc+4+max(size, 0) and q < end and d[q] in COND_OPS:
                c.append(cond_text(d, q)); q += op_size(d, q)
            st.append((pc+4+size, '&'.join(c) or '?'))
        elif op == 0x07:
            t = pc + s16(d, pc+2)
            if st: st[-1] = (t, 'NICHT[' + st[-1][1] + ']')
        ev.append((pc, op, d[pc:pc+sz].hex(), [x[1] for x in st]))
        t = fwd_target(d, pc, op)
        if t is not None and t > maxf: maxf = t
        if op == 0x01 and pc + 2 > maxf: break
        pc += sz
    return ev

def payload(b):
    return b[22:] if (b[3] & 0x80) else b[14:]

def analyse(path):
    d = open(path, 'rb').read()
    if len(d) < 0x60:
        return dict(stub=d.hex(), size=len(d), subs=[], reach={}, edges={}, npcs=[], msgs=[], texts={}, desync=[])
    subs = {}
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    for sec, tag in ((ms, 'main'), (ss, 'sub')):
        if sec == 0 or sec >= len(d): continue
        se = L.rdt_section_end(d, sec)
        for (o, e, idx) in L.section_regions(d, sec, se):
            subs['%s%02d' % (tag, idx)] = walk(d, sec+o, sec+e)
    # Kanten
    edges = {}
    for k, ev in subs.items():
        out = []
        for pc, op, h, c in ev:
            if op < 0: continue
            b = bytes.fromhex(h)
            if op in (0x03, 0x04): out.append(('sub%02d' % b[3], pc, 'Evt_chain' if op == 3 else 'Evt_exec', c))
            elif op == 0x18: out.append(('sub%02d' % b[1], pc, 'Gosub', c))
            elif op == 0x2C and b[2] == 3: out.append(('sub%02d' % payload(b)[3], pc, 'AOT%d-sce3' % b[1], c))
            elif op == 0x46 and b[2] == 3: out.append(('sub%02d' % b[7], pc, 'Aot_reset%d-sce3' % b[1], c))
        edges[k] = out
    reach = {}
    todo = [(k, None) for k in subs if k.startswith('main')] + [('sub00', None), ('sub01', None)]
    while todo:
        k, via = todo.pop(0)
        if k in reach or k not in subs: continue
        reach[k] = via
        for tgt, pc, how, c in edges[k]:
            if tgt not in reach: todo.append((tgt, '%s@0x%X %s' % (k, pc, how)))
    npcs, msgs = [], []
    for k, ev in subs.items():
        for pc, op, h, c in ev:
            if op < 0: continue
            b = bytes.fromhex(h)
            if op == 0x44:
                x, y, z = s16(b, 8), s16(b, 10), s16(b, 12)
                npcs.append(dict(sub=k, pc=pc, slot=b[1], typ=b[2], beh=b[3], p4=b[4], x=x, y=y, z=z,
                                 dir=s16(b, 16), park=(x == -30000 and z == -30000), cond=c, live=k in reach))
            elif op == 0x2B:
                msgs.append(dict(sub=k, pc=pc, msg=b[1], how='Message_on', mask=u16(b, 2), cond=c, live=k in reach))
            elif op == 0x2C and b[2] == 1:
                msgs.append(dict(sub=k, pc=pc, msg=u16(payload(b), 0), how='AOT%d-sce1' % b[1], mask=0, cond=c, live=k in reach))
            elif op == 0x46 and b[2] == 1:
                msgs.append(dict(sub=k, pc=pc, msg=u16(b, 4), how='Aot_reset%d-sce1' % b[1], mask=0, cond=c, live=k in reach))
    texts = {i: t for i, (t, ctrl) in L.messages(d).items()}
    bad = [k for k, ev in subs.items() if ev and ev[-1][1] == -1]
    return dict(subs=sorted(subs), reach=reach, edges={k: v for k, v in edges.items() if v},
                npcs=npcs, msgs=msgs, texts=texts, desync=bad, size=len(d))

def main():
    root, out = sys.argv[1], sys.argv[2]
    res = {}
    for s in range(1, 7):
        for p in sorted(glob.glob(os.path.join(root, 'STAGE%d' % s, 'ROOM*.RDT'))):
            res[os.path.basename(p)[4:8].upper()] = analyse(p)
    json.dump(res, open(out, 'w', encoding='utf-8'), ensure_ascii=False, indent=0)
    nd = sum(1 for r in res.values() if r['desync'])
    print('RDT', len(res), 'mit Desync', nd, [k for k, r in res.items() if r['desync']][:20])

if __name__ == '__main__':
    main()
