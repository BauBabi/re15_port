# -*- coding: utf-8 -*-
"""Runde 30 Nachtrag K (Granate im Hebetisch): Zensus ueber ALLE RE1.5-RDTs.

  (1) jeder Item_aot_set (0x50) mit einer Granaten-/Werfer-Id: Raum, Menge, Zone-9-Bit, Prop
  (2) fuer jeden solchen Satz mit Prop != 0xFF: das Obj_model_set (0x2D) desselben Props
      und die MD1/TIM des Modells aus der RDT+0x30-Tabelle (TIM,MD1 je obj_id)
  (3) Belegung der Zone-9-Bits (Item_aot_set-Nutzlast + Ck/Set mit bank=9)
  (4) jede Erwaehnung der Ids in Sce_key_ck (0x51) / Keep_Item_ck (0x5E)

Walker: re15_port/tools/scd_walk_lib.py (EINE Laengentabelle). Satzlage Item_aot_set wie
scd_vm.c op_item_aot_set / RE1.5 @0x80040644 (+14 Id, +16 Menge, +18 Bit, +20 Prop; Langform +8).
"""
import os, sys, glob, collections, struct, hashlib

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as W

CD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
NAMEN = {0x09: 'Hand Grenade', 0x0A: 'Acid Grenade', 0x0B: 'Incendiary Grenade',
         0x0F: 'Grenade Launcher', 0x10: 'Grenade Launcher', 0x11: 'Grenade Launcher',
         0x19: 'Explosive Rounds', 0x1A: 'Acid Rounds', 0x1B: 'Incendiary Rounds',
         0x1D: 'Empty Grenade Shells', 0x1E: 'Nitro Capsule', 0x1F: 'Acid Capsule',
         0x20: 'Incendiary Capsule'}


def alle_rdts():
    out = []
    for st in range(1, 7):
        out += sorted(glob.glob(os.path.join(CD, 'STAGE%d' % st, 'ROOM*.RDT')))
    return out


def modell(d, obj):
    """TIM/MD1-Offsets von obj aus der RDT+0x30-Tabelle (rdt_common.c parse_props)."""
    n = d[2]
    tbl = struct.unpack_from('<I', d, 0x30)[0]
    if obj >= n or tbl == 0:
        return None
    tim, md1 = struct.unpack_from('<II', d, tbl + 8 * obj)
    return tim, md1, n


def main():
    rdts = alle_rdts()
    items = []
    bank9 = collections.defaultdict(list)
    erw = []
    objset = collections.defaultdict(list)    # (raum) -> [(pc, obj, roh)]
    alle_ids = collections.Counter()
    daten = {}
    for p in rdts:
        d = open(p, 'rb').read()
        raum = os.path.basename(p)[4:8]
        if len(d) < 0x60:
            continue
        daten[raum] = (p, d)
        reg = W.regionen(d)
        for (tag, idx), liste in sorted(reg.items()):
            for (pc, op, sz) in liste:
                if op == 0x50:
                    o = 8 if (d[pc + 3] & 0x80) else 0
                    it = dict(raum=raum, blk='%s%02d' % (tag, idx), off=pc, slot=d[pc + 1],
                              item=W.u16(d, pc + 14 + o), menge=W.u16(d, pc + 16 + o),
                              bit=W.u16(d, pc + 18 + o), prop=d[pc + 20 + o],
                              roh=d[pc:pc + sz].hex(' '))
                    alle_ids[it['item']] += 1
                    bank9[it['bit']].append((raum, pc, 'Item_aot_set id=0x%02X' % it['item']))
                    if it['item'] in NAMEN:
                        items.append(it)
                elif op in (0x21, 0x22) and d[pc + 1] == 9:
                    bank9[d[pc + 2]].append((raum, pc, 'Ck' if op == 0x21 else 'Set'))
                elif op in (0x51, 0x5E) and d[pc + 1] in NAMEN:
                    erw.append((raum, pc, W.NAMES[op], d[pc:pc + sz].hex(' ')))
                elif op == 0x2D:
                    objset[raum].append((pc, d[pc + 1], d[pc:pc + sz].hex(' ')))
    print('RDTs: %d, Item_aot_set gesamt: %d' % (len(daten), sum(alle_ids.values())))
    print()
    print('(1) Item_aot_set mit Granaten-/Werfer-Ids:')
    for iid in sorted(NAMEN):
        n = [it for it in items if it['item'] == iid]
        print('  0x%02X %-22s Platzierungen: %d' % (iid, NAMEN[iid], len(n)))
        for it in n:
            print('      ROOM%s %s @0x%05X slot=%d menge=%d bit=%d prop=%s   %s'
                  % (it['raum'], it['blk'], it['off'], it['slot'], it['menge'], it['bit'],
                     'kein' if it['prop'] == 0xFF else str(it['prop']), it['roh']))
    print()
    print('(2) Welt-Modelle dieser Platzierungen:')
    for it in items:
        if it['prop'] == 0xFF:
            continue
        p, d = daten[it['raum']]
        m = modell(d, it['prop'])
        os_ = [x for x in objset[it['raum']] if x[1] == it['prop']]
        print('  ROOM%s Id 0x%02X Prop %d: Obj_model_set %s' % (
            it['raum'], it['item'], it['prop'],
            '; '.join('@0x%05X %s' % (a, r) for a, _, r in os_) or 'KEINS'))
        if m:
            tim, md1, n = m
            print('      nOmodel=%d  TIM @0x%05X  MD1 @0x%05X  md1-sha1=%s' % (
                n, tim, md1, hashlib.sha1(d[md1:md1 + 64]).hexdigest()[:12]))
    print()
    print('(3) Zone-9-Bits (RDT) belegt: %d von 256' % len(bank9))
    for b in (53, 54, 55, 56, 57, 58):
        print('   Bit %d: %s' % (b, bank9.get(b, 'FREI in allen RDTs')))
    print()
    print('(4) Sce_key_ck / Keep_Item_ck mit diesen Ids: %d' % len(erw))
    for e in erw:
        print('   ROOM%s @0x%05X %s %s' % e)


if __name__ == '__main__':
    main()
