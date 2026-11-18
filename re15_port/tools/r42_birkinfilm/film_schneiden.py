#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""film_schneiden.py - Runde 42 Strang "birkinfilm": RE2-Film "Birkins Mutation" (R409.STR) ohne den Einschub in der Gegenwart.

NUTZER (18.11.2026, woertlich): "Zusaetzlich moechte ich das du die Birkin Mutation Erklaerungscutscene aus Resident Evil 2
irgendwo sinnvoll mit einbaust." Orchestrator-Entscheid 18.11.: den 12-s-Einschub (Annette in der Gegenwart, Off-Frage von
Ada bzw. Claire) herausschneiden, Schnitt zwischen Weissblende und Kanal.

QUELLE: info/re2leon/PL0/ZMOVIE/R409.STR (Leon-Disc), md5 80241f0a3211979b83410e7565cda960, 21465 Sektoren zu 2336 Byte.
  RE2 ruft den Film mit Movie_on 0x0B (ROOM4090 sub02 @0x2818 `6f 0b`); Filmtabelle @0x800a216c + 0x0B*0x20 = @0x800a22cc
  (FUN_80032150 `addiu v1,v1,0x216c` @0x8003216c): +0x00 Datei-Id 0x0351, +0x0A Breite 0x140, +0x12 Bildlage y = 0x28 = 40.
  Aufbau (gemessen, dieses Werkzeug): 18782 Bildsektoren (Untermodus 0x42, STR-Kopf 0x0160/0x8001, 320x160), 2683 Tonsektoren
  (Untermodus 0x64, Codierung 0x01 = 37800 Hz Stereo 4 Bit), je Bild genau 5 Sektoren, Ton jeder 8. Sektor; 4293 Bilder.

SCHNITT: entfernt werden die Sektoren der Bilder 2327..2692 (Bildnummer im STR-Kopf +0x08, 1-basiert) = 0-basiert 2326..2691 =
  Filmzeit 1:17,53 .. 1:29,70 (Abschrift scratchpad zws_birkin/r409_abschrift.txt: Weissblende bis Bild 2325, Einschub
  2326..2691, Kanal ab 2692). Sektorbereich [11630, 13460) = 1830 Sektoren = 366 Bilder, darin 229 Tonsektoren (12,213 s Ton
  gegen 12,200 s Bild: +13 ms Versatz ab dem Schnitt). Alle uebrigen Sektoren bleiben BYTEGLEICH und in Reihenfolge; die
  Bildnummern im Kopf springen nach dem Schnitt von 2326 auf 2693 (der Port zaehlt Bilder fortlaufend an cur == 0,
  engine/src/re15_str.c re15_str_open, und liest die Nummer nicht).

WARUM EINE NEUE DATEI statt der Original-STR mit Bildbereich: der Port liest nur shared_assets/ (CLAUDE.md), die Original-STR
  muesste ohnehin dorthin kopiert werden (50,1 MB); der Schnitt spart 4,3 MB und haelt Bild und Ton ohne Sonderweg im Spieler
  zusammen (re15_xa_decode_movie dekodiert alle Tonsektoren der Datei am Stueck, Bild f laeuft bei f/30 s nach der Tonuhr).

Ausgabe: shared_assets/PSX/MOVIE/R409BIRK.STR (+ .TXT Herkunft).
Aufruf: python re15_port/tools/r42_birkinfilm/film_schneiden.py [--check]
"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HERE, "..", ".."))


def _haupt(rel):
    """info/ liegt im Hauptbaum; ein Arbeitsbaum .claude/worktrees/<x> sucht nach oben."""
    d = os.path.abspath(os.path.join(PORT, ".."))
    for _ in range(5):
        p = os.path.join(d, rel)
        if os.path.exists(p):
            return p
        d = os.path.dirname(d)
    return os.path.join(PORT, "..", rel)


QUELLE = _haupt(os.path.join("info", "re2leon", "PL0", "ZMOVIE", "R409.STR"))
QUELLE_MD5 = "80241f0a3211979b83410e7565cda960"
ZIEL = os.path.join(PORT, "shared_assets", "PSX", "MOVIE", "R409BIRK.STR")
S = 2336
BILD_VON, BILD_BIS = 2327, 2692          # STR-Kopf +0x08 (1-basiert), beide einschliesslich entfernt


def schneiden():
    d = open(QUELLE, "rb").read()
    assert hashlib.md5(d).hexdigest() == QUELLE_MD5, "R409.STR ist nicht die Disc-Datei"
    assert len(d) % S == 0
    n = len(d) // S
    start, ton = {}, []
    for i in range(n):
        s = d[i * S:(i + 1) * S]
        if s[2] & 0x04:                                       # Untermodus Bit 2 = Audio (re15_xa.c)
            assert s[2] & 0x7F == 0x64 and s[3] == 0x01, (i, s[:4].hex())   # letzter Sektor 0xE4 = + EOF-Bit 0x80
            ton.append(i)
            continue
        assert struct.unpack_from("<HH", s, 8) == (0x0160, 0x8001), (i, s[8:12].hex())
        cur = struct.unpack_from("<H", s, 8 + 4)[0]
        if cur == 0:
            fr = struct.unpack_from("<I", s, 8 + 8)[0]
            assert fr not in start
            start[fr] = i
            assert struct.unpack_from("<HH", s, 8 + 0x10) == (320, 160), (i, fr)
    assert sorted(start) == list(range(1, 4294)), "Bildnummern nicht 1..4293"
    a, b = start[BILD_VON], start[BILD_BIS + 1]
    assert (a, b) == (11630, 13460), (a, b)
    raus_ton = sum(1 for t in ton if a <= t < b)
    neu = d[:a * S] + d[b * S:]
    return d, neu, a, b, raus_ton, len(ton)


def main():
    d, neu, a, b, raus_ton, n_ton = schneiden()
    bilder = 4293 - (BILD_BIS - BILD_VON + 1)
    md5 = hashlib.md5(neu).hexdigest()
    info = ("R409BIRK.STR - Herkunft (Runde 42 Strang birkinfilm, RE2-Film \"Birkins Mutation\" ohne Einschub)\n"
            "Quelle: info/re2leon/PL0/ZMOVIE/R409.STR (RE2 Leon-Disc, Movie_on 0x0B, ROOM4090 sub02 @0x2818), md5 %s,\n"
            "        21465 Sektoren zu 2336 B, 4293 Bilder 320x160 bei 30/s, CD-XA 37800 Hz Stereo 4 Bit.\n"
            "Schnitt: Sektoren [%d, %d) entfernt = Bilder %d..%d (STR-Kopf +0x08) = Filmzeit 1:17,53..1:29,70 (Annette in der\n"
            "        Gegenwart, Off-Frage), darin %d Tonsektoren. Alle uebrigen Sektoren bytegleich, in Reihenfolge.\n"
            "Werkzeug: re15_port/tools/r42_birkinfilm/film_schneiden.py\n"
            "Ergebnis: %d Bilder 320x160, %d Ton-Sektoren, %d B, md5 %s\n"
            "Bildlage im Port: 40 Zeilen Rand oben wie RE2 (Filmtabelle @0x800a22cc +0x12 = 0x0028, FUN_80032150 @0x80032254).\n"
            "Abspielen: engine/src/zws_r41.c + platform/pc/src/zws_r41_pc.c (include/re15_zws_r41.h, include/re15_birkinfilm_r42.h).\n"
            % (QUELLE_MD5, a, b, BILD_VON, BILD_BIS, raus_ton, bilder, n_ton - raus_ton, len(neu), md5))
    if "--check" in sys.argv:
        alt = open(ZIEL, "rb").read() if os.path.exists(ZIEL) else b""
        ok = hashlib.md5(alt).hexdigest() == md5
        print("gleich" if ok else "VERALTET", md5)
        sys.exit(0 if ok else 1)
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    open(ZIEL, "wb").write(neu)
    open(ZIEL[:-4] + ".TXT", "w", newline="\n").write(info)
    print(info)


if __name__ == "__main__":
    main()
