#!/usr/bin/env python3
"""Runde 42 Strang finale, Stufe 3 (Plan S5/U2): John (Typ 0x49) mit dem STEHENDEN Wurf des RE1.5-Spielers (Handgranate).

    python re15_port/tools/r42_finale/john_wurf_modell.py          # schreibt shared_assets/RE2/JOHN_49_WURF.EMD
    python re15_port/tools/r42_finale/john_wurf_modell.py --pruef  # prueft nur, ob die Datei im Baum bytegleich ist

NUTZERWUNSCH 07.11.2026 (woertlich): "Das jemand Leon den Raketenwerfer zuwirft ..."; Nutzerentscheid 17.11.2026 (FINALE_PLAN
F1): in v0.8.31 wirft John bei Leon UND bei Elza. NUTZER 18.11.2026 (woertlich): "Die Wurf Animation ist aktuell noch Schrott. ...
Orientiere dich da mehr an Resident Evil 2." und "Aus der Vorschau - die Wurfanimation sah immer noch unpassend aus." -> Vorgabe
Stufe 3 Runde 2: Wurf OHNE Knien (Adas RE2-Clip 0x12 kommt aus der Hocke, weil Ada von einem hohen Steg wirft), Quelle belegt.

QUELLE (selbst gelesen, Asserts unten): der Handgranaten-Wurf des RE1.5-Spielers, Bank PLD/PL00W09.PLW (Waffe 9 = Handgranate;
Leon). Die Feuer-FSM der Schusswaffen (PSX.EXE) spielt
  Heben  = Clip 6:  `ori v0,zero,0x6` / `sb v0,-13592(at)` (0x800acae8 = Clip) @0x80032f68-0x80032f70 (Sub 0),
  Wurf   = Clip 7 + 2*hoch + 4*tief: `srl v0,a0,15` / `sll v0,v0,1` / `addiu v0,v0,7` / `srl a0,a0,11` / `andi a0,a0,0x4` /
           `addu` / `sb v0,-13592(at)` @0x800334a8-0x800334c8 (Sub 2) -> MITTE = Clip 7,
  Loslassen MITTE im Clip-Bild 0x16 = 22: `ori v0,zero,0x16` / `bne v1,v0` (0x800acae9 = Bild) / Flag 0x4000 / `jal 0x80019700`
           (Wurfkoerper 0x040D1000) @0x800336fc-0x8003374c (Gate Waffe 9 @0x80033684-0x8003368c).
  PL00W09.PLW: Verzeichnis [EDD, EMR, ...], EMR-Kopf (0, 8, 15, 80) = 15 Knochen, 80 Byte je Keyframe wie 0x49; EDD 13 Clips,
  Clip 6 = 10 Bilder (Keyframes 0x84..0x8D), Clip 7 = 35 Bilder (0x8E..0xB0), Bild-Flags 0.
  Gerippe: Leon PL00.PLD und RE1.5-0x49 haben dieselben Eltern [-1,0,1,2,3,1,5,6,0,0,9,10,0,12,13] und dieselben Beinlaengen
  (Knochen 2..7, bis auf 1 Einheit: Schienbein 811 gegen 810); Keyframes sind Winkel + Wurzellage, darum gelten sie auf Johns Gerippe (wie annette_modell.py).
  Der werfende Arm ist 9-10-11 = Teil 10, an dem RE2 den Werfer haelt (Super_set @0x01854): Teil 10 + (0,600,-50) geht im Clip 7
  von (337,-1834) ueber (-960,-2039) (Ausholen, Bild 13) und (358,-948) (Schwung, Bild 17) nach (1402,-2014) (Bild 22, vorn).

UMBAU (PORT-WAHL, Muster annette_modell.py baue()): Paar 1 (dir[1] EDD / dir[2] EMR, 24 Clips = die Bank, die der Port-Renderer
bei Plc_motion(1, c) zeichnet, State 4 Sub 1, main.c) bekommt hinten Clip 24 = W09-Clip 6 (Heben) und Clip 25 = W09-Clip 7
(Wurf), je Bild ein eigener Keyframe in Bildreihenfolge, Bild-Flags aus der PLW (0). Clips 0..23, ihre Keyframes, Paar 2/3, MD1
und TIM bleiben bytegleich (nur verschoben). Container wie tools/birkin_formen.py: [u32 dir_off][u32 n=9][Sektionen][Verzeichnis
n*4]. Deterministisch. Runde 1 hatte hier Adas RE2-Clip 0x12 (Hocke) - ersetzt.
"""
import argparse, hashlib, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
REPO = os.path.normpath(os.path.join(PORT, '..'))
sys.path.insert(0, os.path.join(PORT, 'tools'))
import emd_ansichtsblatt as EA            # noqa: E402  (EMS-Kette und EMS_ORDER wie re15_ems.c)

W09 = os.path.join(PORT, 'shared_assets', 'PSX', 'PLD', 'PL00W09.PLW')
PL00 = os.path.join(PORT, 'shared_assets', 'PSX', 'PLD', 'PL00.PLD')
ZIEL = os.path.join(PORT, 'shared_assets', 'RE2', 'JOHN_49_WURF.EMD')
TYP = 0x49
KF_GROESSE = 80
QUELL_CLIPS = ((6, 10), (7, 35))   # (W09-Clip, Bilder): Heben @0x80032f68-70, Wurf MITTE @0x800334a8-c8
NEU_CLIP = 24              # erste neue Clip-Nummer in Paar 1 (hinter 0..23): 24 = Heben, 25 = Wurf

u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]


def w09_clips():
    """PL00W09.PLW: EDD/EMR -> [(Bildeintraege, Keyframes je Bild)] fuer Clip 6 und 7."""
    b = open(W09, 'rb').read()
    do, n = struct.unpack_from('<II', b, 0)
    d = [u32(b, do + 4 * i) for i in range(n)] + [do]
    clips, frames = EA.edd_parse(b[d[0]:d[1]])
    e = b[d[1]:d[2]]
    kopf = struct.unpack_from('<4H', e, 0)
    assert kopf[2:] == (15, KF_GROESSE), kopf
    kd = e[kopf[1]:]
    assert len(clips) == 13, len(clips)
    aus = []
    for c, nb in QUELL_CLIPS:
        f0, cnt = clips[c]
        assert cnt == nb, (c, cnt)
        ents = [frames[f0 + i] for i in range(cnt)]
        assert all((x >> 12) == 0 for x in ents), [hex(x) for x in ents]
        kfs = [kd[(x & 0xFFF) * KF_GROESSE:((x & 0xFFF) + 1) * KF_GROESSE] for x in ents]
        assert all(len(k) == KF_GROESSE for k in kfs)
        aus.append((ents, kfs))
    assert [x & 0xFFF for x in aus[0][0]] == list(range(0x84, 0x8E)), 'Clip 6 Keyframes'
    assert [x & 0xFFF for x in aus[1][0]] == list(range(0x8E, 0xB1)), 'Clip 7 Keyframes'
    return aus


def leon_gerippe():
    """PL00.PLD (Leon): EMR = dir[1]."""
    b = open(PL00, 'rb').read()
    do, n = struct.unpack_from('<II', b, 0)
    d = [u32(b, do + 4 * i) for i in range(n)] + [do]
    e = b[d[1]:d[2]]
    return EA.emr_parse(e), struct.unpack_from('<4H', e, 0)


def baue():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(TYP))
    blob = ems[off:off + ln]
    d = EA.emd_dir(blob)
    dir_off, n = u32(blob, 0), u32(blob, 4)
    assert dir_off + n * 4 == len(blob) and d[0] == 8 and n == 9
    edd, emr = blob[d[1]:d[2]], blob[d[2]:d[3]]
    kopf = struct.unpack_from('<4H', emr, 0)
    assert kopf == (100, 176, 15, KF_GROESSE), kopf
    sk_j = EA.emr_parse(emr)
    sk_l, kopf_l = leon_gerippe()
    assert kopf_l == kopf, (kopf_l, kopf)
    assert list(sk_j.parent) == list(sk_l.parent) == [-1, 0, 1, 2, 3, 1, 5, 6, 0, 0, 9, 10, 0, 12, 13], (sk_j.parent, sk_l.parent)
    assert all(abs(a - b) <= 1 for ra, rb in zip(sk_j.rel[2:8], sk_l.rel[2:8]) for a, b in zip(ra, rb)), \
        ('Beine', sk_j.rel[2:8], sk_l.rel[2:8])                 # gleich bis auf 1 Einheit (Schienbein 811/810)
    kf_n = (len(emr) - kopf[1]) // KF_GROESSE
    neu = w09_clips()
    c0, o0 = struct.unpack_from('<HH', edd, 0)
    assert o0 // 4 == 24, 'Paar 1 hat 24 Clips'
    tab = bytearray(edd[:o0])
    n_alt = c0 + sum(struct.unpack_from('<H', edd, 4 * i)[0] for i in range(1, o0 // 4))
    bilder = edd[o0:o0 + 4 * n_alt]
    assert edd[o0 + 4 * n_alt:] == struct.pack('<I', o0 + 4 * n_alt), edd[o0 + 4 * n_alt:].hex()
    for i in range(o0 // 4):                             # Eintrag i: (u16 Bilder, u16 Byte-Offset der Bildliste) -> +4
        cnt, of = struct.unpack_from('<HH', tab, 4 * i)
        struct.pack_into('<HH', tab, 4 * i, cnt, of + 4)
    for i in range(o0 // 4):                             # zweiter neuer Tabelleneintrag -> noch einmal +4
        cnt, of = struct.unpack_from('<HH', tab, 4 * i)
        struct.pack_into('<HH', tab, 4 * i, cnt, of + 4)
    o_neu = o0 + 8 + 4 * n_alt
    neu_bilder = b''
    kf_neu = b''
    k = kf_n
    for ents, kfs in neu:
        tab += struct.pack('<HH', len(ents), o_neu + len(neu_bilder))
        neu_bilder += b''.join(struct.pack('<I', (e & ~0xFFF) | (k + i)) for i, e in enumerate(ents))
        kf_neu += b''.join(kfs)
        k += len(ents)
    edd_neu = bytes(tab) + bilder + neu_bilder
    edd_neu += struct.pack('<I', len(edd_neu))
    emr_neu = emr + kf_neu
    vor = blob[8:d[1]] + edd_neu + emr_neu + blob[d[3]:dir_off]
    delta = len(edd_neu) - len(edd) + len(emr_neu) - len(emr)
    neu_dir = [d[0], d[1], d[1] + len(edd_neu)] + [x + delta for x in d[3:9]]
    while (8 + len(vor)) % 4:
        vor += b'\0'
    k = struct.pack('<II', 8 + len(vor), n)
    return k + vor + struct.pack('<%dI' % n, *neu_dir), kf_n, neu


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--pruef', action='store_true')
    a = ap.parse_args()
    b, kf_n, neu = baue()
    md5 = hashlib.md5(b).hexdigest()
    print('PL00W09.PLW Clip 6 (%d Bilder) -> Clip 24, Clip 7 (%d Bilder) -> Clip 25; Keyframes ab %d' % (
        len(neu[0][0]), len(neu[1][0]), kf_n))
    if a.pruef:
        alt = open(ZIEL, 'rb').read() if os.path.exists(ZIEL) else b''
        print('JOHN_49_WURF.EMD %s %s' % (md5, 'gleich' if alt == b else 'ABWEICHEND'))
        return 0 if alt == b else 1
    open(ZIEL, 'wb').write(b)
    print('JOHN_49_WURF.EMD %7d B  md5 %s' % (len(b), md5))
    return 0


if __name__ == '__main__':
    sys.exit(main())
