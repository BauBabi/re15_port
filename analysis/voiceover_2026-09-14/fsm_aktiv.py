#!/usr/bin/env python3
"""Steht beim NAECHSTEN Message_on der Dialog-FSM der VORIGEN Zeile noch?

Das entscheidet, ob der neue Riegel in op_message_on (scd_vm.c:1500-1505)
ueberhaupt greift: er ist mit
    if (g_scd.message_query == 0 && !g_scd.message_fsm_active) { ... }
gegatet.  re15_dialog_open_mask setzt message_fsm_active = 1 (msg_common.c:383)
und erst fsm==6 loescht es (msg_common.c:546).  msg_show (Full-Text-Raeume
0x1170/0x1240) setzt es dagegen auf 0 (scd_vm.c:1645).
"""
import os, sys, glob
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scd_zeit as SZ
import stellen as ST
import wav_inventar as WI
import bericht as B

FULLTEXT = (0x1170, 0x1240)


def main():
    rows = WI.scan()
    per_room = {}
    for stage, room, msg, path, info, defekt in rows:
        per_room.setdefault((stage, room), {})[msg] = info
    print('%-6s %-7s %9s %8s %9s %9s  %s' %
          ('ROOM', 'MSG', 'Aufnahme', 'Skript', 'Text-FSM', 'FSM offen?', 'Riegel greift?'))
    for (stage, room) in sorted(per_room):
        rp = B.raum_rdt(stage, room)
        if not os.path.exists(rp):
            continue
        r = SZ.Rdt(rp)
        pts = ST.grenzen(r)
        voiced = set(per_room[(stage, room)].keys())
        life = {mid: SZ.dialog_lifetime(raw) for mid, raw in r.msgs.items()}
        w = SZ.Walker(r, voiced, life)
        st = ST.alle_stellen(r)
        rueck = B.gosub_aufrufer(r, pts)
        ft = int(room, 16) in FULLTEXT
        for mid in sorted(voiced):
            info = per_room[(stage, room)][mid]
            bilder = info['frames'] if info else 0
            kand = [s for s in st if s[3] == mid]
            best = None
            for (bn, i, pc, m, mask) in kand:
                stacks = [()]
                if bn == 'sub':
                    for ra in rueck.get(i, [])[:4]:
                        stacks.append((ra,))
                for sk in stacks:
                    g, killer = w.min_gap(pc, sk)
                    if g is not None and (best is None or g < best[0]):
                        best = (g, killer)
            lf = life.get(mid)
            lfs = ('%d/%s' % lf) if lf else '-'
            if ft:
                offen = 'nein (Full-Text: fsm_active=0)'
                riegel = 'JA'
            elif lf is None:
                offen = '?'
                riegel = '?'
            else:
                if lf[1] in ('taste', 'seite', 'auswahl'):
                    offen = 'ja (wartet auf Taste)'
                    riegel = 'NEIN'
                elif best is not None and lf[0] > best[0]:
                    offen = 'ja (%d > %d)' % (lf[0], best[0])
                    riegel = 'NEIN'
                else:
                    offen = 'nein (%s <= %s)' % (lf[0], best[0] if best else '-')
                    riegel = 'JA'
            print('%-6s main%02d %9.1f %8s %9s %-22s %s' %
                  (room, mid, bilder,
                   best[0] if best else '-', lfs, offen, riegel))


if __name__ == '__main__':
    main()
