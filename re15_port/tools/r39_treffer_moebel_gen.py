#!/usr/bin/env python3
"""Runde 39 Spur D: erzeugt re15_port/engine/src/gen/treffer_r39_moebel.inc — die Liste der Art-3-Kollisionszellen,
die fuer den SPIELER-SCHUSS und die ZIELHILFE durchlaessig sind (Moebel: Tische, Pulte, Betten, Stuehle, Kisten).

FORM (RE2 Retail): FUN_80050858 sperrt die Sichtlinie nur fuer Zellen mit dem Designer-Flag 0x80 (`lhu v1,8(t1)`
@0x800508bc / `and v0,v1,fp` @0x800508c4 / `beq v0,zero,0x800508b0` @0x800508c8); ~16 % der RE2-Zellen tragen es nicht
(Moebel). RE1.5 kennt dasselbe Attribut als Art-Nibble (FUN_8003dcc4(0xf00,0x300) @0x8001ba1c-24: nur Art 3 sperrt) und
die Designer haben es in ROOM1020 fuer Moebel benutzt (Art 2) — in den meisten Raeumen aber pauschal Art 3 gesetzt
(unfertiger Beta-Datenstand). INHALT (gemessen): je Raum die INSEL-Zellen (nicht an der Band-Huelle), im Hintergrund
jedes Cuts nachgesehen (analysis/befunde_runde39/D_belege/zellen/ROOM*_inseln_*.png, Werkzeug
r39_treffer_inseln_bild.py). Wand, Regal, Gitter, Saeule, Auto, Maschine bleiben Art 3 = sperrend.

    python re15_port/tools/r39_treffer_moebel_gen.py            # schreibt die .inc neu
"""
import os, struct

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PSX  = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
ZIEL = os.path.join(REPO, 're15_port', 'engine', 'src', 'gen', 'treffer_r39_moebel.inc')

# Raum -> {Zellenindex (Partition 0): Beschreibung}  (Leon-Raum; die Elza-Variante +1 wird uebernommen, wenn ihre
# Zelle byte-gleich ist)
MOEBEL = {
    0x1140: {5: 'Pult am Fenster (hinten)', 6: 'Konferenztisch', 7: 'Rednerpult', 8: 'Beistelltisch an der Tuer',
             9: 'Tischreihe vor den Karten', 10: 'Kamera-Stativ', 11: 'PC-Pult rechts', 12: 'Pult rechts',
             13: 'Leiste am Tuergang', 14: 'Stuhl/Topf', 15: 'Stuhl', 16: 'Topf/Stuhl', 17: 'Lampe', 18: 'Lampe'},
    0x10E0: {3: 'Bett (Leiche)', 4: 'Wagen hinter dem Bett', 7: 'Schreibtisch', 9: 'Tisch', 10: 'Bett', 11: 'Bett',
             13: 'Wagen', 15: 'Rolltisch', 16: 'Rolltisch', 17: 'Wagen', 18: 'Wagen', 19: 'Schreibtisch (Schreibmaschine)',
             20: 'Bett', 21: 'Tisch', 22: 'Tisch'},
    0x10D0: {1: 'Pultreihe hinten', 3: 'Pult', 6: 'Tisch am Fenster', 9: 'Theke', 10: 'Pultreihe', 13: 'Pult',
             14: 'Pult', 15: 'gruener Tisch im Gang', 16: 'Pult', 17: 'Pult', 18: 'Stuhl', 19: 'Stuhl', 20: 'Topfpflanze',
             21: 'Stuhl', 22: 'Pultteil', 23: 'Pultteil', 24: 'Pultteil', 25: 'Pultteil', 26: 'Pultteil',
             27: 'Pultteil', 28: 'Pultteil', 29: 'Pultteil'},
    0x1010: {4: 'Tischteil', 5: 'Tisch', 6: 'Tischteil', 7: 'Tischteil', 9: 'Tischteil', 10: 'Stuhl', 11: 'Stuhl',
             12: 'Stuhl', 13: 'Stuhl', 14: 'Stuhl', 15: 'Tischecke'},
    0x1020: {4: 'Pultreihe', 5: 'Pult', 6: 'Pultkante', 7: 'Pultreihe', 8: 'Stuhl'},
    0x1070: {4: 'Stuhl', 5: 'Stuhl', 6: 'Stuhl'},
    0x1110: {5: 'Karton', 10: 'Kiste'},
    0x11F0: {0: 'Rohr am Boden', 1: 'Rohr am Boden', 2: 'Rohr am Boden', 10: 'Kasten', 11: 'Tisch (Schublade)',
             12: 'Eimer'},
    0x1220: {4: 'Bank an der Wand', 13: 'Toilette', 14: 'Waschbecken', 15: 'Hocker', 16: 'Toilette', 17: 'Hocker',
             18: 'Bett', 20: 'Toilette', 21: 'Bett', 22: 'Hocker', 25: 'Bett', 26: 'Toilette', 27: 'Hocker',
             29: 'Hocker', 30: 'Toilette', 31: 'Bett', 32: 'Hocker', 33: 'Toilette', 34: 'Bett'},
    0x1250: {7: 'Tisch', 9: 'Tisch', 10: 'Tisch', 11: 'Feuerloescher', 12: 'Pult', 14: 'Tischreihe'},
    0x30D0: {22: 'Tisch', 23: 'Tisch', 25: 'Bankreihe', 26: 'Bankreihe', 29: 'Theke', 30: 'Theke'},
}

def lade(room):
    for st in range(1, 7):
        p = os.path.join(PSX, 'STAGE%d' % st, 'ROOM%04X.RDT' % room)
        if os.path.exists(p) and os.path.getsize(p) > 0x100:
            return open(p, 'rb').read()
    return None

def zellen(d):
    sca = struct.unpack_from('<I', d, 0x20)[0]
    cnt = struct.unpack_from('<5I', d, sca + 4)
    return [struct.unpack_from('<HHhhBBBB', d, sca + 24 + i * 12) for i in range(cnt[0])]

def main():
    zeilen = []
    for room in sorted(MOEBEL):
        d = lade(room)
        if d is None:
            raise SystemExit('RDT fehlt: %04X' % room)
        zl = zellen(d)
        de = lade(room + 1)
        zle = zellen(de) if de else None
        for idx in sorted(MOEBEL[room]):
            w, dd, x, z, t, u0, u1, fl = zl[idx]
            if (fl & 0xf) != 3:
                raise SystemExit('ROOM%04X [%d] ist nicht Art 3' % (room, idx))
            txt = MOEBEL[room][idx]
            zeilen.append('    { 0x%04X, %2d, %6d, %6d, %5d, %5d, 0x%02X, %d, "%s" },' % (room, idx, x, z, w, dd, t, fl >> 4, txt))
            if zle and idx < len(zle) and zle[idx] == zl[idx]:
                zeilen.append('    { 0x%04X, %2d, %6d, %6d, %5d, %5d, 0x%02X, %d, "%s (Elza)" },' % (room + 1, idx, x, z, w, dd, t, fl >> 4, txt))
    kopf = [
        '/* treffer_r39_moebel.inc - GENERIERT von re15_port/tools/r39_treffer_moebel_gen.py (Runde 39 Spur D).',
        ' * Art-3-Kollisionszellen, die fuer Spieler-Schuss und Zielhilfe DURCHLAESSIG sind (Moebel). Form = RE2-Flag-Praxis',
        ' * (FUN_80050858 @0x800508bc-c8), Inhalt = Hintergrundbilder je Cut (D_belege/zellen). Felder: Raum, Index',
        ' * (Partition 0, nur Doku), x, z, Breite, Tiefe, Form (type), Band, Beschreibung. Zuordnung zur Laufzeit ueber',
        ' * Raum + Geometrie (x,z,w,d), nicht ueber den Index. Elza-Varianten nur, wenn die Zelle byte-gleich ist. */',
    ]
    with open(ZIEL, 'w', newline='\n') as f:
        f.write('\n'.join(kopf) + '\n' + '\n'.join(zeilen) + '\n')
    print('->', ZIEL, len(zeilen), 'Eintraege')

if __name__ == '__main__':
    main()
