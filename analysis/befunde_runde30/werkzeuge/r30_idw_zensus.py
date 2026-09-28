# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: Zensus ueber ALLE RE1.5-RDTs (opcode-exakter Walk).

  (1) jeder Item_aot_set (0x50): Item-Id, Menge, Zone-9-Bit, Prop, Rechteck, sat
  (2) Belegung der Zone-9-Bits: Item_aot_set-Nutzlast + jeder Ck/Set mit bank=9
  (3) jede Erwaehnung der Item-Ids 0x20 / 0x21 / 0x48 in item-bezogenen Opcodes
      (0x50 Item_aot_set, 0x51 Sce_key_ck, 0x5E Keep_Item_ck, 0x46 Aot_reset)

Der Walker ist scd_walk_lib (EINE Laengentabelle, s. dort). Ein desynchroner Block
wird gezaehlt und gemeldet, nicht stillschweigend uebergangen.
"""
import os, sys, glob, collections, struct

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as W

CD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')


def alle_rdts():
    out = []
    for st in range(1, 7):
        out += sorted(glob.glob(os.path.join(CD, 'STAGE%d' % st, 'ROOM*.RDT')))
    return out


def main():
    rdts = alle_rdts()
    items = []
    bank9 = collections.defaultdict(list)     # bit -> [(raum, off, was)]
    erw = []                                  # Erwaehnungen der gesuchten Ids
    n_header = 0; n_stummel = 0; bloecke = 0; ops = 0
    abbruch = []
    for p in rdts:
        d = open(p, 'rb').read()
        raum = os.path.basename(p)[4:8]
        if len(d) < 0x60:
            n_stummel += 1
            continue
        n_header += 1
        reg = W.regionen(d)
        for (tag, idx), liste in sorted(reg.items()):
            bloecke += 1
            ops += len(liste)
            # Desync-Erkennung: endet der Walk nicht auf Evt_end, merken
            if liste and liste[-1][1] != 0x01:
                abbruch.append((raum, '%s%02d' % (tag, idx), liste[-1][0], liste[-1][1]))
            for (pc, op, sz) in liste:
                if op == 0x50:
                    lang = (d[pc + 3] & 0x80) != 0
                    o = 8 if lang else 0
                    x, z, w, dd = struct.unpack_from('<hhhh', d, pc + 6)
                    it = dict(raum=raum, blk='%s%02d' % (tag, idx), off=pc, slot=d[pc + 1], sce=d[pc + 2],
                              sat=d[pc + 3], floor=d[pc + 4], sup=d[pc + 5], x=x, z=z, w=w, d=dd,
                              item=W.u16(d, pc + 14 + o), menge=W.u16(d, pc + 16 + o),
                              bit=W.u16(d, pc + 18 + o), prop=d[pc + 20 + o], act=d[pc + 21 + o], lang=lang,
                              roh=d[pc:pc + sz].hex())
                    items.append(it)
                    bank9[it['bit']].append((raum, pc, 'Item_aot_set id=0x%02X' % it['item']))
                    if it['item'] in (0x20, 0x21, 0x48):
                        erw.append((raum, pc, 'Item_aot_set', d[pc:pc + sz].hex()))
                elif op in (0x21, 0x22):
                    if d[pc + 1] == 9:
                        bank9[d[pc + 2]].append((raum, pc, 'Ck' if op == 0x21 else 'Set'))
                elif op in (0x51, 0x5E):
                    if d[pc + 1] in (0x20, 0x21, 0x48):
                        erw.append((raum, pc, W.NAMES[op], d[pc:pc + sz].hex()))
    print('RDTs gefunden: %d | mit Header: %d | Stummel: %d' % (len(rdts), n_header, n_stummel))
    print('SCD-Bloecke: %d | Opcodes gewalkt: %d | Bloecke, die NICHT auf Evt_end enden: %d' % (bloecke, ops, len(abbruch)))
    for a in abbruch[:20]:
        print('   offen: ROOM%s %s letzter op @0x%05X = 0x%02X' % a)
    print()
    print('Item_aot_set gesamt: %d' % len(items))
    c = collections.Counter((it['w'], it['d']) for it in items)
    print('Rechteck-Groessen (w x d): Anzahl')
    for (w, dd), n in c.most_common():
        print('   %5d x %5d : %3d' % (w, dd, n))
    c = collections.Counter(it['sat'] for it in items)
    print('sat-Byte:', ', '.join('0x%02X: %d' % kv for kv in sorted(c.items())))
    c = collections.Counter(it['sce'] for it in items)
    print('sce-Byte:', ', '.join('%d: %d' % kv for kv in sorted(c.items())))
    c = collections.Counter(it['act'] for it in items)
    print('action-Byte (+21):', ', '.join('%d: %d' % kv for kv in sorted(c.items())))
    c = collections.Counter(it['floor'] for it in items)
    print('floor-Byte (+4):', ', '.join('%d: %d' % kv for kv in sorted(c.items())))
    c = collections.Counter(it['sup'] for it in items)
    print('super-Byte (+5):', ', '.join('%d: %d' % kv for kv in sorted(c.items())))
    mit = [it for it in items if it['prop'] != 0xFF]
    print('mit Welt-Modell (prop != 0xFF): %d | ohne: %d' % (len(mit), len(items) - len(mit)))
    print()
    print('Zone-9-Bits belegt: %d von 256' % len(bank9))
    bel = sorted(bank9)
    print('   belegt:', ' '.join(str(b) for b in bel))
    frei = [b for b in range(256) if b not in bank9]
    # zusammenhaengende freie Bloecke
    bl = []; s = None
    for b in range(257):
        if b < 256 and b not in bank9:
            if s is None: s = b
        else:
            if s is not None: bl.append((s, b - 1)); s = None
    print('   freie Bloecke:', ' '.join('%d..%d(%d)' % (a, b, b - a + 1) for a, b in bl))
    print()
    print('Erwaehnungen der Item-Ids 0x20/0x21/0x48 (Item_aot_set, Sce_key_ck, Keep_Item_ck): %d' % len(erw))
    for e in erw:
        print('   ROOM%s @0x%05X %s %s' % e)
    print()
    print('Alle Items im Raum 1150/1151:')
    for it in items:
        if it['raum'] in ('1150', '1151'):
            print('   ', it)
    # vollstaendige Liste fuer das Dossier
    ziel = os.path.join(REPO, 'build', 'r30_irons-diary-welt', 'item_aot_zensus.txt')
    with open(ziel, 'w') as f:
        for it in items:
            f.write('ROOM%s %s @0x%05X slot=%d sce=%d sat=0x%02X floor=%d rect=(%d,%d,%d,%d) item=0x%02X menge=%d bit=%d prop=%d act=%d\n'
                    % (it['raum'], it['blk'], it['off'], it['slot'], it['sce'], it['sat'], it['floor'],
                       it['x'], it['z'], it['w'], it['d'], it['item'], it['menge'], it['bit'], it['prop'], it['act']))
    print('geschrieben:', os.path.relpath(ziel, REPO))


if __name__ == '__main__':
    main()
