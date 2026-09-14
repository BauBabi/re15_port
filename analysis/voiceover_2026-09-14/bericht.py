#!/usr/bin/env python3
"""Treiber: verbindet die WAV-Laengen mit der Skript-Zeit und listet die
abgeschnittenen Sprachaufnahmen."""
import os, sys, glob, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import wav_inventar as WI
import scd_zeit as SZ
import stellen as ST

RDTDIR = 're15_port/shared_assets/PSX'


def raum_rdt(stage, room):
    return '%s/%s/ROOM%s.RDT' % (RDTDIR, stage, room)


def gosub_aufrufer(r, pts):
    """sub-index -> Liste Ruecksprungadressen (pc+2 der Gosub-Stelle)."""
    out = {}
    for bname, blk in (('main', r.main), ('sub', r.sub), ('extra', r.extra)):
        for i, start in enumerate(blk):
            end = ST.block_ende(r, start, pts)
            pc = start
            while pc < end and pc + 2 <= len(r.d):
                op = r.d[pc]
                n = SZ.oplen(r.d, pc)
                if op == 0x18:
                    out.setdefault(r.d[pc + 1], []).append(pc + 2)
                if n <= 0:
                    break
                pc += n
    return out


def evt_exec_ziele(r, pts):
    out = set()
    for bname, blk in (('main', r.main), ('sub', r.sub), ('extra', r.extra)):
        for i, start in enumerate(blk):
            end = ST.block_ende(r, start, pts)
            pc = start
            while pc < end and pc + 4 <= len(r.d):
                op = r.d[pc]
                n = SZ.oplen(r.d, pc)
                if op == 0x04:
                    out.add(r.d[pc + 3])
                if n <= 0:
                    break
                pc += n
    return out


def analyse():
    rows = WI.scan()
    per_room = {}
    for stage, room, msg, path, info, defekt in rows:
        per_room.setdefault((stage, room), {})[msg] = (info, defekt, path)

    ergebnis = []
    text = []
    for (stage, room) in sorted(per_room):
        clips = per_room[(stage, room)]
        rp = raum_rdt(stage, room)
        if not os.path.exists(rp):
            text.append('!! kein RDT: %s' % rp)
            continue
        r = SZ.Rdt(rp)
        pts = ST.grenzen(r)
        voiced = set(clips.keys())
        life = {mid: SZ.dialog_lifetime(raw) for mid, raw in r.msgs.items()}
        w = SZ.Walker(r, voiced, life)
        st = ST.alle_stellen(r)
        rueck = gosub_aufrufer(r, pts)
        spawns = evt_exec_ziele(r, pts)

        text.append('')
        text.append('=============== %s ROOM%s ===============' % (stage, room))
        text.append('  Subs: %d  Messages im RDT: %d  vertont: %d  '
                    'Message_on-Stellen: %d' %
                    (len(r.sub), len(r.msgs), len(voiced), len(st)))
        masken = sorted(set(s[4] for s in st))
        text.append('  Pause-Masken aller Message_on: %s   (Bit 0x0200 = '
                    'RE15_PAUSE_SCD -> Skript-Freeze)' %
                    ', '.join('0x%04x' % m for m in masken))
        spawn_msgs = sorted(set(s[3] for s in st
                                if s[0] == 'sub' and s[1] in spawns))
        if spawn_msgs:
            text.append('  Evt_exec-Subs mit Message_on: %s' % spawn_msgs)

        ids_im_skript = set(s[3] for s in st)
        tot = sorted(voiced - ids_im_skript)
        if tot:
            text.append('  ** AUFNAHMEN OHNE Message_on IN DIESEM RAUM: %s' %
                        ', '.join('main%02d.wav' % m for m in tot))

        for mid in sorted(voiced):
            info, defekt, path = clips[mid]
            bilder = info['frames'] if info else 0
            kand = [s for s in st if s[3] == mid]
            if not kand:
                ergebnis.append(dict(room=room, stage=stage, mid=mid,
                                     aufnahme=bilder, skript=None, killer=None,
                                     stelle=None, anm='kein Message_on'))
                text.append('  main%02d  Aufnahme %6.1f B  -> KEIN Message_on, '
                            'spielt nie' % (mid, bilder))
                continue
            best = None
            for (bn, i, pc, m, mask) in kand:
                stacks = [()]
                if bn == 'sub':
                    for ra in rueck.get(i, [])[:4]:
                        stacks.append((ra,))
                for sk in stacks:
                    g, killer = w.min_gap(pc, sk)
                    if g is None:
                        continue
                    if best is None or g < best[0]:
                        best = (g, killer, bn, i, pc, mask, sk)
            if best is None:
                bn, i, pc, m, mask = kand[0]
                ergebnis.append(dict(room=room, stage=stage, mid=mid,
                                     aufnahme=bilder, skript=None, killer=None,
                                     stelle='%s%02d@0x%04X' % (bn, i, pc),
                                     anm='kein toetendes Message_on erreichbar'))
                text.append('  main%02d  Aufnahme %6.1f B  %s%02d @0x%04X '
                            'mask=%04x -> kein weiteres vertontes Message_on '
                            '=> laeuft aus' % (mid, bilder, bn, i, pc, mask))
                continue
            g, killer, bn, i, pc, mask, sk = best
            ergebnis.append(dict(room=room, stage=stage, mid=mid,
                                 aufnahme=bilder, skript=g, killer=killer,
                                 stelle='%s%02d@0x%04X' % (bn, i, pc),
                                 mask=mask, anm=''))
            fehl = bilder - g
            text.append('  main%02d  Aufnahme %6.1f B  Skript %4d B  '
                        '%s%02d @0x%04X mask=%04x -> Killer main%02d   %s'
                        % (mid, bilder, g, bn, i, pc, mask, killer,
                           ('FEHLEN %.1f' % fehl) if fehl > 0
                           else ('Reserve %.1f' % -fehl)))
    return ergebnis, text


def main():
    erg, text = analyse()
    print('\n'.join(text))
    print('\n\n============ TABELLE: Fehlbetrag absteigend ============')
    print('%-6s %-7s %9s %8s %9s  %-20s %s' %
          ('ROOM', 'MSG', 'Aufnahme', 'Skript', 'Fehlt', 'Stelle', 'Killer'))
    def key(e):
        if e['skript'] is None:
            return (1, 0)
        return (0, -(e['aufnahme'] - e['skript']))
    for e in sorted(erg, key=key):
        if e['skript'] is None:
            print('%-6s main%02d %9.1f %8s %9s  %-20s %s' %
                  (e['room'], e['mid'], e['aufnahme'], '-', '-',
                   e['stelle'] or '-', e['anm']))
        else:
            print('%-6s main%02d %9.1f %8d %9.1f  %-20s main%02d' %
                  (e['room'], e['mid'], e['aufnahme'], e['skript'],
                   e['aufnahme'] - e['skript'], e['stelle'], e['killer']))


if __name__ == '__main__':
    main()
