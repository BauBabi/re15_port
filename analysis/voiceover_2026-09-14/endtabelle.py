#!/usr/bin/env python3
"""Endtabelle: Aufnahme / Skript-Budget / Fehlbetrag / Riegel.

Budget  = Offline-Walker (scd_zeit.py). Er ueberspringt UNVERTONTE Message_on
          (re15_voice_play kehrt fuer sie vor re15_xa_read_s zurueck,
          audio_pc.c:1711) und nimmt in Warteschleifen den KUERZESTEN Weg.
          Gegengeprueft gegen den echten VM (probe_stimme_takt): identisch,
          wo die Sonde ueberhaupt messen kann.
Standzeit / Riegel = aus probe_stimme_takt.exe (echter Dialog-FSM).
"""
import os, sys, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bericht as B

PROBE = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                     'probe_stimme_takt.txt')


def probe_lesen():
    out = {}
    if not os.path.exists(PROBE):
        return out
    for ln in open(PROBE, encoding='utf-8', errors='replace'):
        m = re.match(r'^([0-9A-F]{4})\s+main(\d\d)\s+([\d.]+)\s+(-?\d+)\s+(\d+)\s+(\S+)', ln)
        if m:
            out[(m.group(1), int(m.group(2)))] = (int(m.group(4)), int(m.group(5)),
                                                  m.group(6))
    return out


def main():
    erg, _ = B.analyse()
    pr = probe_lesen()
    zeilen = []
    for e in erg:
        key = (e['room'], e['mid'])
        p = pr.get(key)
        standzeit = p[1] if p else None
        vm_abstand = p[0] if p else None
        budget = e['skript']
        fehl = (e['aufnahme'] - budget) if budget is not None else None
        # Riegel: greift, wenn Budget >= Standzeit (Volltext-Raeume immer)
        if e['room'] in ('1170', '1240'):
            riegel = 'JA (Volltext: fsm_active=0)'
        elif standzeit is None or budget is None:
            riegel = '-'
        elif budget >= standzeit:
            riegel = 'JA'
        else:
            riegel = 'NEIN (Text steht noch)'
        zeilen.append(dict(room=e['room'], mid=e['mid'], auf=e['aufnahme'],
                           budget=budget, fehl=fehl, killer=e['killer'],
                           stelle=e['stelle'], standzeit=standzeit,
                           vm=vm_abstand, riegel=riegel, anm=e['anm']))

    def key(z):
        return (0, -z['fehl']) if z['fehl'] is not None else (1, 0)

    print('%-5s %-7s %8s %7s %7s %6s %6s %-22s %-20s %s' %
          ('RAUM', 'MSG', 'Aufnahme', 'Budget', 'Fehlt', 'Stand', 'VM', 'Riegel',
           'Stelle', 'Killer'))
    for z in sorted(zeilen, key=key):
        print('%-5s main%02d %8.1f %7s %7s %6s %6s %-22s %-20s %s' %
              (z['room'], z['mid'], z['auf'],
               z['budget'] if z['budget'] is not None else '-',
               ('%.1f' % z['fehl']) if z['fehl'] is not None else '-',
               z['standzeit'] if z['standzeit'] is not None else '-',
               z['vm'] if z['vm'] is not None else '-',
               z['riegel'], z['stelle'] or '-',
               ('main%02d' % z['killer']) if z['killer'] is not None else z['anm']))

    n_fehl = sum(1 for z in zeilen if z['fehl'] is not None and z['fehl'] > 0)
    n_knapp = sum(1 for z in zeilen if z['fehl'] is not None and -10 < z['fehl'] <= 0)
    n_nokill = sum(1 for z in zeilen if z['budget'] is None)
    print('\n%d Aufnahmen, davon %d ABGESCHNITTEN, %d knapp (< 10 Bilder Reserve), '
          '%d ohne toetendes Message_on.' % (len(zeilen), n_fehl, n_knapp, n_nokill))
    ueber = [z for z in zeilen if z['fehl'] is not None and z['fehl'] > 90]
    print('Fehlbetrag > 90 Bilder (Deckel des Riegels): %s' %
          (', '.join('%s main%02d (%.1f)' % (z['room'], z['mid'], z['fehl'])
                     for z in ueber) or 'keine'))
    aus = [z for z in zeilen if z['riegel'].startswith('NEIN')]
    print('Riegel greift NICHT: %s' %
          (', '.join('%s main%02d (Fehlbetrag %s)' %
                     (z['room'], z['mid'],
                      ('%.1f' % z['fehl']) if z['fehl'] is not None else '-')
                     for z in aus) or 'keine'))


if __name__ == '__main__':
    main()
