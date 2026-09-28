#!/usr/bin/env python
"""Runde 30 karten-marken-abzug: Speicherkarte des Nutzers lesen.
Sucht die Magic 0x35314552 ("RE15") in jedem Block und legt die Struktur
re15_savedata_t (include/re15_savedata.h, v8) darueber:
  u32 magic, version, playtime; s32 x,y,z; u16 room, save_count; s16 rot, hp;
  u16 status; u8 character, equipped_slot, weapon_id, camera_cut, loc_idx, discard;
  inv[11]*4; flags[16][8]*4; box[64]*4; wounds[8][2]; visited[32]; u32 checksum.
Aufruf: python r30_karte_lesen.py <karte.mcr> [--roomlist <re15_room_list.h>]"""
import struct, sys, re, os

INV, ZONES, WORDS, BOX = 11, 16, 8, 64

def room_ids(pfad):
    txt = open(pfad, encoding='utf-8', errors='replace').read()
    m = re.search(r're15_room_ids\[\][^{]*\{(.*?)\};', txt, re.S)
    return [int(x, 16) for x in re.findall(r'0x([0-9A-Fa-f]+)', m.group(1))]

def main():
    p = sys.argv[1]
    rl = 're15_port/include/re15_room_list.h'
    if '--roomlist' in sys.argv: rl = sys.argv[sys.argv.index('--roomlist') + 1]
    ids = room_ids(rl)
    basis = [r for r in ids if not (r & 1)]
    d = open(p, 'rb').read()
    print("Karte %s  %d B, Raumliste %d Eintraege (%d Basisraeume)" % (p, len(d), len(ids), len(basis)))
    pos = 0
    while True:
        i = d.find(struct.pack('<I', 0x35314552), pos)
        if i < 0: break
        pos = i + 4
        blk, off = i // 0x2000, i % 0x2000
        (magic, ver, playtime, x, y, z, room, cnt, rot, hp, status,
         ch, eq, wid, cut, loc, disc) = struct.unpack_from('<IIIiiiHHhhHBBBBBB', d, i)
        o = i + struct.calcsize('<IIIiiiHHhhHBBBBBB')
        assert (o - i) % 4 == 0, o - i
        o_inv = o; o += INV * 4
        o_flags = o; o += ZONES * WORDS * 4
        o_box = o; o += BOX * 4
        o_w = o; o += 16
        o_vis = o; o += 32
        o_ck = o
        ck = struct.unpack_from('<I', d, o_ck)[0]
        summe = sum(d[i:o_ck]) & 0xFFFFFFFF
        vis = d[o_vis:o_vis + 32]
        print("\n== Block %d (+0x%X)  Datei-Offset 0x%05X  Version %d  Raum ROOM%04X cut %d  "
              "pos=(%d,%d,%d) rot=%d hp=%d  save_count %d  Spielzeit %d Bilder  Charakter %d"
              % (blk, off, i, ver, room, cut, x, y, z, rot, hp, cnt, playtime, ch))
        print("   visited[] @Datei 0x%05X (Struktur +0x%X): %s" % (o_vis, o_vis - i, vis.hex(' ')))
        print("   checksum gespeichert 0x%08X  gerechnet 0x%08X  %s"
              % (ck, summe, "OK" if ck == summe else "FALSCH"))
        bits = [b for b in range(256) if (vis[b >> 3] >> (b & 7)) & 1]
        print("   gesetzte Besucht-Bits (%d): %s" % (len(bits), bits))
        for b in bits:
            if b >= 240:
                print("      Bit %3d = Zusatzbit %d (s_zone_zusatzbit: 240 = ROOM1000 Zone 2, 241 = ROOM2070 Zone 2)" % (b, b - 240))
            else:
                n, zweit = b // 2, b & 1
                rr = basis[n] if n < len(basis) else None
                print("      Bit %3d = ROOM%04X %s" % (b, rr if rr is not None else 0xFFFF,
                                                     "Zone 0" if not zweit else "Zone >= 1"))
        # Karten-Besitz: Flag(3,115)
        fl = struct.unpack_from('<%dI' % (ZONES * WORDS), d, o_flags)
        z3 = fl[3 * WORDS:(3 + 1) * WORDS]
        print("   Flag-Zone 3:", ' '.join('%08X' % w for w in z3))

if __name__ == '__main__':
    main()
