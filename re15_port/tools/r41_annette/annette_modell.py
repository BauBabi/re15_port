#!/usr/bin/env python3
"""Runde 41 Spur AN: Modell der Frau im Kittel (Typ 0x4D, Leon 6000/6030/5090) mit dem Aussehen der RE2-Annette.

    python re15_port/tools/r41_annette/annette_modell.py          # schreibt shared_assets/RE2/ANNETTE_4D*.EMD
    python re15_port/tools/r41_annette/annette_modell.py --pruef  # nur pruefen, ob die Dateien im Repo bytegleich sind

PORT-WAHL (Dossier analysis/befunde_runde41/AN_annette.md Abschnitt 4): Gerippe und Clips bleiben die des RE1.5-Typs 0x4D
(RE1.5 CDEMD0.EMS, Blob von 0x4D, dir[1..6]); Netz (MD1, dir[7]) und Textur (TIM, dir[8]) kommen von der RE2-Annette:

    RE2 CDEMD0.EMS (TOC @0x8009adf4, engine/src/gen/re2_ems_toc.inc)
      kind 0x47 rec 3 (EMD) Sektor 4525, 51252 B  -> MD1 = EMD[dir[7]:dir[8]] (dir @EMD+0, 8 Eintraege)
      kind 0x47 rec 2 (TIM) Sektor 4476, 99872 B  -> ANNETTE_4D.EMD
      kind 0x49 rec 2 (TIM) Sektor 4616, 99872 B  -> ANNETTE_4D_BLUT.EMD (dasselbe EMD byte-gleich, Kittel mit Blut)

Belegt, dass das passt (AN_belege/rig_4d_gegen_re2_47.txt): beide Gerippe 15 Knochen, gleiche Eltern, gleiche Versaetze bis auf
die Unterarme/Haende (z -60/-61 gegen -55/-46). Netz i haengt an Knochen i wie im Port ueblich.

Nachbesserung 4: Paar 1 bekommt hinten Clip 24 = RE2-Liegeclip 0x18 (ROOM6141-RBJ, Auszug re2_6141_liegen.bin, siehe AUSZUG);
Clips 0..23, Keyframes 0..357 und Paar 2/3 bleiben bytegleich.

    python re15_port/tools/r41_annette/annette_modell.py --auszug <info/re2clair/PL1/RDT/ROOM6141.RDT> [--pruef]

Container wie tools/birkin_formen.py baue(): [u32 dir_off][u32 n=9][Sektionen dir[0..8]][Verzeichnis n*4].
Deterministisch (kein Zeitstempel, keine Zufallszahl).
"""
import argparse, hashlib, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
sys.path.insert(0, os.path.join(PORT, 'tools'))
import emd_ansichtsblatt as EA            # noqa: E402  (EMS-Kette und EMS_ORDER wie re15_ems.c)

RE2_EMS = os.path.join(PORT, 'shared_assets', 'RE2', 'CDEMD0.EMS')
AUS = os.path.join(PORT, 'shared_assets', 'RE2')
TYP = 0x4D
RE2 = {  # kind: (TIM Sektor, Groesse), (EMD Sektor, Groesse)   gen/re2_ems_toc.inc
    0x47: ((4476, 99872), (4525, 51252)),
    0x49: ((4616, 99872), (4665, 51252)),
}
DATEIEN = (('ANNETTE_4D.EMD', 0x47), ('ANNETTE_4D_BLUT.EMD', 0x49))

# Nachbesserung 4 (Abnahme 2 N2, Dossier AN_annette.md N4.2): RE2 legt Annette nach dem Hieb mit Plc_motion(0, 0x18, 4)
# (Claire PL1/RDT/ROOM6141.RDT MAIN sub02 @0x01172 `3f 00 18 04`; beim spaeteren Besuch Sce_em_set @0x00D16 Motion
# `18 01`) = Clip 0x18 aus dem Raum-Animationsblock: ROOM6141 RDT+0x60 -> RBJ @0x2E70 (`b8 79 00 00 01 00 00 00`), Record 0
# (Marker 5, 15 Knochen, 80 Byte je Keyframe), Clip 24 = 40 Bilder, Keyframes 239..278. RAM-Beleg Savestates lauf3 e23..e30:
# Annette +0x14C = 0x18, Bildzaehler +0x14D 6/26/6/22 (Schleife 40), Teile (+0x198, je 0xAC, Lage +0x5C/+0x60/+0x64) y -433..-24.
# Gerippe RE2-Annette = 0x4D (AN_belege/rig_4d_gegen_re2_47.txt), Keyframe-Groesse 80 wie Paar 1 der 0x4D.
# Der Auszug (40 Bildeintraege u32 + 40 Keyframes zu 80 Byte) liegt als re2_6141_liegen.bin hier, weil info/re2clair nicht
# versioniert ist; --auszug <ROOM6141.RDT> zieht ihn neu und vergleicht.
AUSZUG = os.path.join(HIER, 're2_6141_liegen.bin')
KF_GROESSE = 80
LIEGEN_RBJ_ZEIGER, LIEGEN_REC, LIEGEN_CLIP, LIEGEN_BILDER = 0x60, 0, 0x18, 40

u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]


def auszug_aus_rdt(rdt):
    """RE2 ROOM6141: RBJ-Block (RDT+0x60), Record 0, Clip 0x18 -> (Bildeintraege, Keyframes). Format wie
    re15_port/tools/r34n_d/rbj_zensus.py (+0 u32 Gesamtlaenge, +4 u32 Records, Trailer je Record (u32 EMR, u32 EDD))."""
    u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
    s = u32(rdt, LIEGEN_RBJ_ZEIGER)
    r = rdt[s:]
    total, nrec = u32(r, 0), u32(r, 4)
    assert nrec >= 1
    pre, edd = struct.unpack_from('<II', r, total + LIEGEN_REC * 8)
    _arm, o_frm, nb, ks = struct.unpack_from('<4H', r, pre + 4)
    assert nb == 15 and ks == KF_GROESSE, (nb, ks)
    kf0 = pre + 4 + o_frm
    c0, o0 = u16(r, edd), u16(r, edd + 2)
    zahl = [c0] + [u16(r, edd + 4 * c) for c in range(1, o0 // 4)]
    erst = sum(zahl[:LIEGEN_CLIP])
    assert zahl[LIEGEN_CLIP] == LIEGEN_BILDER, zahl[LIEGEN_CLIP]
    ents = [u32(r, edd + o0 + 4 * (erst + f)) for f in range(LIEGEN_BILDER)]
    kfs = b''.join(r[kf0 + (e & 0xFFF) * ks: kf0 + ((e & 0xFFF) + 1) * ks] for e in ents)
    neu = [(e & ~0xFFF) | i for i, e in enumerate(ents)]
    return struct.pack('<%dI' % LIEGEN_BILDER, *neu) + kfs


def re2_liegen():
    """(Keyframes 40 x 80 Byte, Bildeintraege mit Keyframe 0..39) aus dem Auszug."""
    a = open(AUSZUG, 'rb').read()
    assert len(a) == LIEGEN_BILDER * (4 + KF_GROESSE), len(a)
    ents = list(struct.unpack_from('<%dI' % LIEGEN_BILDER, a, 0))
    return a[LIEGEN_BILDER * 4:], ents


def re2_teile(kind):
    ems = open(RE2_EMS, 'rb').read()
    (ts, tl), (es, el) = RE2[kind]
    emd, tim = ems[es * 2048:es * 2048 + el], ems[ts * 2048:ts * 2048 + tl]
    assert len(emd) == el and len(tim) == tl
    do, dc = u32(emd, 0), u32(emd, 4)
    assert dc == 8, 'RE2-EMD traegt 8 Verzeichniseintraege (re2_ems.h)'
    d = list(struct.unpack_from('<%dI' % dc, emd, do)) + [do]
    md1 = emd[d[7]:d[8]]
    assert tim[:4] == b'\x10\x00\x00\x00', 'TIM-Kopf'
    return md1, tim


def baue(kind):
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(TYP))
    blob = ems[off:off + ln]
    d = EA.emd_dir(blob)
    dir_off, n = u32(blob, 0), u32(blob, 4)
    assert dir_off + n * 4 == len(blob) and d[0] == 8 and n == 9
    md1, tim = re2_teile(kind)
    while len(md1) % 4:
        md1 += b'\0'
    # Nachbesserung 4 (Abnahme 2 N2): Clip 24 = RE2-Liegeclip (re2_liegen) hinten an Paar 1 (dir[1] EDD / dir[2] EMR, 24 Clips).
    # Clips 0..23 und ihre Keyframes bleiben bytegleich; dir[3..6] (Paar 2/3) werden nur verschoben.
    edd, emr = blob[d[1]:d[2]], blob[d[2]:d[3]]
    kf_n = (len(emr) - struct.unpack_from('<H', emr, 2)[0]) // KF_GROESSE      # 358 Keyframes (kf_off 176, ksize 80)
    lkf, lents = re2_liegen()
    c0, o0 = struct.unpack_from('<HH', edd, 0)
    tab = bytearray(edd[:o0])
    n_alt = c0 + sum(struct.unpack_from('<H', edd, 4 * i)[0] for i in range(1, o0 // 4))   # 358 Bildeintraege
    bilder = edd[o0:o0 + 4 * n_alt]
    # Hinter der Bildliste steht ein u32 = Byte-Ende der Liste (EM4D: @EDD+0x5F8 `f8 05 00 00` = 96 + 4 * 358); er wandert mit.
    assert edd[o0 + 4 * n_alt:] == struct.pack('<I', o0 + 4 * n_alt), edd[o0 + 4 * n_alt:].hex()
    for i in range(o0 // 4):                             # Eintrag i: (u16 Bilder, u16 Byte-Offset der Bildliste) -> +4
        cnt, off = struct.unpack_from('<HH', tab, 4 * i)
        struct.pack_into('<HH', tab, 4 * i, cnt, off + 4)
    tab += struct.pack('<HH', len(lents), o0 + 4 + 4 * n_alt)
    neu_bilder = b''.join(struct.pack('<I', (e & ~0xFFF) | (kf_n + (e & 0xFFF) - (lents[0] & 0xFFF))) for e in lents)
    edd_neu = bytes(tab) + bilder + neu_bilder
    edd_neu += struct.pack('<I', len(edd_neu))
    emr_neu = emr + lkf
    vor = blob[8:d[1]] + edd_neu + emr_neu + blob[d[3]:d[7]]
    delta = len(edd_neu) - len(edd) + len(emr_neu) - len(emr)
    neu_dir = [d[0], d[1], d[1] + len(edd_neu)] + [x + delta for x in d[3:8]]
    neu_dir += [neu_dir[7] + len(md1)]
    neu = vor + md1 + tim
    while (8 + len(neu)) % 4:
        neu += b'\0'
    kopf = struct.pack('<II', 8 + len(neu), n)
    return kopf + neu + struct.pack('<%dI' % n, *neu_dir)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--pruef', action='store_true')
    ap.add_argument('--auszug', metavar='ROOM6141.RDT', help='RE2-Liegeclip aus der Claire-RDT ziehen und mit re2_6141_liegen.bin vergleichen')
    a = ap.parse_args()
    fehler = 0
    if a.auszug:
        neu = auszug_aus_rdt(open(a.auszug, 'rb').read())
        alt = open(AUSZUG, 'rb').read() if os.path.exists(AUSZUG) else b''
        if a.pruef:
            print('re2_6141_liegen.bin   %s %s' % (hashlib.md5(neu).hexdigest(), 'gleich' if neu == alt else 'ABWEICHEND'))
            fehler += 0 if neu == alt else 1
        else:
            open(AUSZUG, 'wb').write(neu)
            print('re2_6141_liegen.bin   %7d B  md5 %s' % (len(neu), hashlib.md5(neu).hexdigest()))
    for name, kind in DATEIEN:
        b = baue(kind)
        ziel = os.path.join(AUS, name)
        md5 = hashlib.md5(b).hexdigest()
        if a.pruef:
            alt = open(ziel, 'rb').read() if os.path.exists(ziel) else b''
            ok = alt == b
            fehler += 0 if ok else 1
            print('%-22s %s %s' % (name, md5, 'gleich' if ok else 'ABWEICHEND'))
        else:
            open(ziel, 'wb').write(b)
            print('%-22s %7d B  md5 %s' % (name, len(b), md5))
    return fehler


if __name__ == '__main__':
    sys.exit(main())
