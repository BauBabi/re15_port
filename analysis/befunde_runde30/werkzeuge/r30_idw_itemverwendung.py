# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: WER benutzt ein Item im CODE?

Sucht in PSX.EXE, DEBUG.BIN und den sechs STAGE-Overlays jede Aufrufstelle der
Inventar-Suchfunktion FUN_8004dfec (find-slot-by-item-id, engine/src/inventory_common.c:141)
und liest das Argument a0 aus den bis zu 6 Instruktionen davor bzw. dem Delay-Slot
(ori/addiu/li a0,zero,imm). Ausgegeben wird je Aufrufstelle die Item-Id.

Zusaetzlich: jede Stelle, an der 0x21 als Sofortwert in a0/a1/a2 geladen wird und
innerhalb der naechsten 4 Instruktionen ein jal in den Inventarbereich 0x8004da00..0x8004e400
folgt (faengt auch andere Inventarfunktionen).

Kein Beleg fuer ABWESENHEIT im strengen Sinn (ein ueber Register durchgereichtes Argument
faellt durch) - deshalb wird die Trefferzahl JE Item-Id mit ausgegeben: wenn andere Ids
gefunden werden, sieht das Verfahren Aufrufe dieser Form.
"""
import os, sys, struct, collections

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
CD = os.path.join(REPO, 'info', 'Re1.5')

ZIELE = {0x8004dfec: 'FUN_8004dfec find-slot-by-id'}


def laden():
    out = []
    exe = open(os.path.join(CD, 'PSX.EXE'), 'rb').read()
    t_addr = struct.unpack_from('<I', exe, 0x18)[0]
    out.append(('PSX.EXE', exe[0x800:], t_addr))
    dbg = open(os.path.join(CD, 'PSX', 'BIN', 'DEBUG.BIN'), 'rb').read()
    out.append(('DEBUG.BIN', dbg, 0x800C0000))
    for n in range(1, 7):
        p = os.path.join(CD, 'PSX', 'BIN', 'STAGE%d.BIN' % n)
        if os.path.exists(p):
            out.append(('STAGE%d.BIN' % n, open(p, 'rb').read(), 0x80100000))
    return out


def imm_a(w):
    """liefert (reg, wert) fuer ori/addiu rX,zero,imm mit rX in a0..a3, sonst None."""
    op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xFFFF
    if op in (0x0D, 0x09) and rs == 0 and 4 <= rt <= 7:
        if op == 0x09 and imm & 0x8000:
            imm -= 0x10000
        return rt, imm
    return None


def main():
    je_id = collections.Counter()
    stellen = collections.defaultdict(list)
    breit = []
    for name, d, base in laden():
        n = len(d) // 4
        W = struct.unpack_from('<%dI' % n, d, 0)
        for i, w in enumerate(W):
            if (w >> 26) == 3:                     # jal
                ziel = ((w & 0x03FFFFFF) << 2) | 0x80000000
                addr = base + 4 * i
                if ziel in ZIELE:
                    wert = None
                    # Delay-Slot zuerst, dann rueckwaerts
                    for k in [i + 1] + list(range(i - 1, max(i - 7, -1), -1)):
                        if 0 <= k < n:
                            r = imm_a(W[k])
                            if r and r[0] == 4:
                                wert = r[1]; break
                    je_id[wert] += 1
                    stellen[wert].append('%s @0x%08X' % (name, addr))
                if 0x8004DA00 <= ziel < 0x8004E400:
                    for k in [i + 1] + list(range(i - 1, max(i - 5, -1), -1)):
                        if 0 <= k < n:
                            r = imm_a(W[k])
                            if r and r[1] == 0x21:
                                breit.append('%s @0x%08X jal 0x%08X mit a%d=0x21 (@0x%08X)' % (
                                    name, addr, ziel, r[0] - 4, base + 4 * k))
                                break
    print('Aufrufe von FUN_8004dfec mit Sofortwert in a0, je Item-Id:')
    for k in sorted(je_id, key=lambda v: (v is None, v)):
        print('   %s : %d   %s' % ('0x%02X' % k if k is not None else 'a0 nicht als Sofortwert', je_id[k],
                                   ', '.join(stellen[k][:6])))
    print()
    print('jal in den Inventarbereich 0x8004da00..0x8004e400 mit 0x21 in a0..a3: %d' % len(breit))
    for b in breit:
        print('   ' + b)


if __name__ == '__main__':
    main()
