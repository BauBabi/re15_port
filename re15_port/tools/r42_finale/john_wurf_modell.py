#!/usr/bin/env python3
"""Runde 42 Strang finale, Stufe 3 (Plan S5/U2): John (Typ 0x49) mit Adas Wurf-Clip aus dem RE2-Ende.

    python re15_port/tools/r42_finale/john_wurf_modell.py          # schreibt shared_assets/RE2/JOHN_49_WURF.EMD
    python re15_port/tools/r42_finale/john_wurf_modell.py --pruef  # prueft nur, ob die Datei im Baum bytegleich ist

NUTZERWUNSCH 07.11.2026 (woertlich): "Das jemand Leon den Raketenwerfer zuwirft ..."; Nutzerentscheid 17.11.2026 (FINALE_PLAN
F1): in v0.8.31 wirft John bei Leon UND bei Elza.

QUELLE (selbst gelesen, Asserts unten):
  RE2 ROOM7020 MAIN sub12 laesst Ada werfen: Plc_motion `3f 0a 12 40` @0x01878 und noch einmal @0x018AA = Clip 0x12 des RAUM-
  Animationsblocks: RDT-Kopf offs[22] = 0x2E34 (RDT+0x60, `34 2e 00 00`), Block `54 2e 00 00 01 00 00 00` = 1 Record, Marker 0x3
  (Bit 0 Spieler, Bit 1 Gegner-Index 0; Binder FUN_8001b3f8 wie re15_rbj_binder_r41.c), 15 Knochen, 80 Byte je Keyframe,
  19 Clips [1 x15, 20, 30, 20, 55]: Clip 0x12 = 55 Bilder, Clip 0x11 = Leons Reaktion (@0x01932 `3f 00 11 40`).
  Gerippe: RE1.5-0x49 (CDEMD0.EMS, bytegleich in CDEMD1.EMS: md5 ef5fc85e.. beide 137200 B) und RE2-Ada (RE2 CDEMD0.EMS kind 0x41
  EMD Sektor 4109, 77296 B) haben 15 Knochen mit denselben Eltern [-1,0,1,2,3,1,5,6,0,0,9,10,0,12,13] und dieselbe Keyframe-
  Groesse 80 (EMR-Kopf (100,176,15,80) bzw. RE2 (100,176,15,80)); nur die Laengen unterscheiden sich (Arm 9-10 z.B. 485 gegen
  452). Dieselbe Uebertragung wie Annette (tools/r41_annette/annette_modell.py: RE2-Clip 0x18 als Clip 24 auf das 0x4D-Gerippe).

UMBAU (PORT-WAHL, Muster annette_modell.py baue()): Paar 1 (dir[1] EDD / dir[2] EMR, 24 Clips = die Bank, die der Port-Renderer
bei Plc_motion(1, c) zeichnet, State 4 Sub 1, main.c) bekommt hinten Clip 24 = RE2-Clip 0x12 (55 Bildeintraege, je ein eigener
Keyframe in Bildreihenfolge, Bild-Flags = obere 20 Bit des RE2-Eintrags). Clips 0..23, ihre Keyframes, Paar 2/3, MD1 und TIM
bleiben bytegleich (nur verschoben). Container wie tools/birkin_formen.py: [u32 dir_off][u32 n=9][Sektionen][Verzeichnis n*4].
Deterministisch.
"""
import argparse, hashlib, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
REPO = os.path.normpath(os.path.join(PORT, '..'))
sys.path.insert(0, os.path.join(PORT, 'tools'))
import emd_ansichtsblatt as EA            # noqa: E402  (EMS-Kette und EMS_ORDER wie re15_ems.c)

R7020 = os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT', 'ROOM7020.RDT')
RE2_EMS = os.path.join(PORT, 'shared_assets', 'RE2', 'CDEMD0.EMS')
ZIEL = os.path.join(PORT, 'shared_assets', 'RE2', 'JOHN_49_WURF.EMD')
TYP = 0x49
KF_GROESSE = 80
RBJ_ZEIGER = 0x60          # RDT-Kopf offs[22] (8 + 22*4)
WURF_CLIP, WURF_BILDER = 0x12, 55
NEU_CLIP = 24              # Clip-Nummer in Paar 1 (hinter 0..23)

u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]


def re2_wurf():
    """ROOM7020 RBJ Record 0 Clip 0x12 -> (Bildeintraege, Keyframes je Bild)."""
    rdt = open(R7020, 'rb').read()
    s = u32(rdt, RBJ_ZEIGER)
    assert s == 0x2E34, hex(s)
    r = rdt[s:]
    total, nrec = u32(r, 0), u32(r, 4)
    assert (total, nrec) == (0x2E54, 1), (hex(total), nrec)
    pre, edd = struct.unpack_from('<II', r, total)
    assert u32(r, pre) == 0x3, 'Marker'
    _arm, o_frm, nb, ks = struct.unpack_from('<4H', r, pre + 4)
    assert nb == 15 and ks == KF_GROESSE, (nb, ks)
    c0, o0 = u16(r, edd), u16(r, edd + 2)
    zahl = [c0] + [u16(r, edd + 4 * c) for c in range(1, o0 // 4)]
    assert zahl == [1] * 15 + [20, 30, 20, 55], zahl
    erst = sum(zahl[:WURF_CLIP])
    ents = [u32(r, edd + o0 + 4 * (erst + f)) for f in range(WURF_BILDER)]
    kf0 = pre + 4 + o_frm
    kfs = [r[kf0 + (e & 0xFFF) * ks: kf0 + ((e & 0xFFF) + 1) * ks] for e in ents]
    assert all(len(k) == ks for k in kfs)
    return ents, kfs


def ada_gerippe():
    """RE2-Ada (kind 0x41) Eltern + EMR-Kopf aus RE2 CDEMD0.EMS (Sektor/Laenge gen/re2_ems_toc.inc)."""
    ems = open(RE2_EMS, 'rb').read()
    e = ems[4109 * 2048:4109 * 2048 + 77296]
    do, dc = u32(e, 0), u32(e, 4)
    d = list(struct.unpack_from('<%dI' % dc, e, do))
    sk = EA.emr_parse(e[d[2]:d[3]])
    return sk, struct.unpack_from('<4H', e, d[2])


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
    sk_a, kopf_a = ada_gerippe()
    assert kopf_a == kopf, (kopf_a, kopf)
    assert list(sk_j.parent) == list(sk_a.parent) == [-1, 0, 1, 2, 3, 1, 5, 6, 0, 0, 9, 10, 0, 12, 13], (sk_j.parent, sk_a.parent)
    kf_n = (len(emr) - kopf[1]) // KF_GROESSE
    ents, kfs = re2_wurf()
    c0, o0 = struct.unpack_from('<HH', edd, 0)
    assert o0 // 4 == 24, 'Paar 1 hat 24 Clips'
    tab = bytearray(edd[:o0])
    n_alt = c0 + sum(struct.unpack_from('<H', edd, 4 * i)[0] for i in range(1, o0 // 4))
    bilder = edd[o0:o0 + 4 * n_alt]
    assert edd[o0 + 4 * n_alt:] == struct.pack('<I', o0 + 4 * n_alt), edd[o0 + 4 * n_alt:].hex()
    for i in range(o0 // 4):                             # Eintrag i: (u16 Bilder, u16 Byte-Offset der Bildliste) -> +4
        cnt, of = struct.unpack_from('<HH', tab, 4 * i)
        struct.pack_into('<HH', tab, 4 * i, cnt, of + 4)
    tab += struct.pack('<HH', len(ents), o0 + 4 + 4 * n_alt)
    neu_bilder = b''.join(struct.pack('<I', (e & ~0xFFF) | (kf_n + i)) for i, e in enumerate(ents))
    edd_neu = bytes(tab) + bilder + neu_bilder
    edd_neu += struct.pack('<I', len(edd_neu))
    emr_neu = emr + b''.join(kfs)
    vor = blob[8:d[1]] + edd_neu + emr_neu + blob[d[3]:dir_off]
    delta = len(edd_neu) - len(edd) + len(emr_neu) - len(emr)
    neu_dir = [d[0], d[1], d[1] + len(edd_neu)] + [x + delta for x in d[3:9]]
    while (8 + len(vor)) % 4:
        vor += b'\0'
    k = struct.pack('<II', 8 + len(vor), n)
    return k + vor + struct.pack('<%dI' % n, *neu_dir), kf_n, ents


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--pruef', action='store_true')
    a = ap.parse_args()
    b, kf_n, ents = baue()
    md5 = hashlib.md5(b).hexdigest()
    print('RE2 ROOM7020 RBJ @0x2E34 Clip 0x12: %d Bilder, Flags %s; Keyframes ab %d' % (
        len(ents), sorted(set('%05x' % (e >> 12) for e in ents)), kf_n))
    if a.pruef:
        alt = open(ZIEL, 'rb').read() if os.path.exists(ZIEL) else b''
        print('JOHN_49_WURF.EMD %s %s' % (md5, 'gleich' if alt == b else 'ABWEICHEND'))
        return 0 if alt == b else 1
    open(ZIEL, 'wb').write(b)
    print('JOHN_49_WURF.EMD %7d B  md5 %s' % (len(b), md5))
    return 0


if __name__ == '__main__':
    sys.exit(main())
