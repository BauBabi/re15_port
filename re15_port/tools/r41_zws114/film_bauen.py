#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41, Zwischensequenz Raum 114 (ROOM1141, John und Elza): der Gorilla-Film des Nutzers als STR fuer den Port.

  python re15_port/tools/r41_zws114/film_bauen.py [--quelle <mp4>] [--ziel <STR>] [--budget <Sektoren>]

QUELLE (unveraendert gelesen, nie geschrieben): Cutscenes/Rooms/114/Nachbau_Blender/Raum114_Gorillas_v3_Ton.mp4
  (Nutzer-Film, 640x480, 30 Bilder/s, 367 Bilder = 12,233 s, AAC-Ton 44,1 kHz Stereo).
ZIEL: re15_port/shared_assets/PSX/MOVIE/R114GORI.STR (+ R114GORI.TXT mit der Herkunft).

FORMAT = das der einzigen Film-Datei des Spiels, MOVIE/CAPCOM.STR, damit der Port den Film mit seinen vorhandenen
Dekodern abspielt (engine/src/re15_str.c Demux, bss_vlc.c/bss_mdec.c MDEC, re15_xa.c CD-XA; Vertrag in
include/re15_str.h und include/re15_xa.h):
  * 2336-Byte-Sektoren (CD-XA Mode 2 ohne Sync/Kopf): 8 Byte Subheader + 2048 Nutzdaten + 280 Byte EDC/ECC.
    EDC/ECC bleiben 0 (der Port prueft sie nicht; fuer eine echte Disc muesste ein Mastering-Werkzeug sie rechnen).
  * Video-Sektor: Subheader 00 01 48 00 (x2) wie CAPCOM.STR Sektor 0; STR-Kopf 32 B (0x0160, 0x8001, cur, cnt,
    Bild 1-basiert, demuxSize, 320, 240, Kopie der ersten 8 BS-Bytes, 0); danach 2016 B MDEC-Bitstrom.
  * Bitstrom = BS Version 2 (CAPCOM.STR: Kopf `.. 00 38 01 00 02 00`): [Laenge in 32-Bit-Worten, 0x3800, qscale, 2],
    DC 10 Bit roh, AC MPEG-1-Tabelle B.14 (Folgekoeffizient, EOB `10`), Escape `000001` + 6 Bit Lauf + 10 Bit Wert,
    16-Bit-Worte MSB zuerst, little-endian gespeichert (bss_vlc.c read_word_le/show_bits). Makrobloecke spaltenweise,
    Bloecke Cr, Cb, Y0..Y3 (bss_mdec.c yuv2rgb_column), Quant-Tabelle IQ_TABLE (bss_mdec.c, = MDEC-Standardtabelle),
    AC-Rueckrechnung (IQ * qscale * Wert) >> 3, DC IQ[0] * Wert (bss_mdec.c rl2blk).
  * Ton-Sektor: Subheader 01 01 64 01 (x2) wie CAPCOM.STR Sektor 7 = Audio, Stereo, 37800 Hz, 4 Bit; 18 Tongruppen x
    128 B, Parameter je Einheit (Filter << 4 | Shift), Daten 28 Worte (re15_xa.c re15_xa_decode_sector).
  * Bildgroesse 320x240 wie CAPCOM.STR: der Port-Dekoder kann hoechstens 256 Zeilen (bss_mdec.c s_col_buf 16*3*256),
    und das Spielbild ist 320x240 (render_pc.c SCREEN_XRES/YRES). 640x480 -> 320x240 = 2x2-Mittel (ffmpeg area).

KODIERER: eigener (dieses Skript), weil psxavenc v0.3.1 (info/.../psxavenc-linux) am Dateiende abstuerzt ("buffer
overflow detected"), den XA-Kopf falsch schreibt und Bilder zeitlich verschiebt (gemessen 2026-11-13: 364..366 statt 367
Bilder, Bild 100 passt zur Quelle 99). Die AC-Codetabelle ist aus psxavenc/mdec.c uebernommen (ac_huffman_tree,
zlib-Lizenz: Copyright (c) 2019, 2020 Adrian "asie" Siekierka, (c) 2019 Ben "GreaseMonkey" Russell, (c) 2023 spicyjpeg;
hier als Python-Tabelle umgeschrieben). Jeder Bitstrom wird beim Bauen mit einem Python-Nachbau der Port-Dekoder
zurueckgelesen (Koeffizienten bitgleich); der Riegel test_r41_zws114 dekodiert die fertige Datei mit den echten
Port-Dekodern.

QUALITAET: je Bild die kleinste Quantisierung qscale >= 1, deren Bitstrom in --budget Sektoren passt (Vorgabe 10 = 20160 B;
re15_str.c STR_MAX_FRAME_SEC erlaubt 16). Eine 2x-CD (150 Sektoren/s) traegt bei 30 Bildern/s nur ~4,4 Sektoren je Bild;
der Film ist fuer den PC-Port gebaut, die PSX-Fassung des Ports spielt ihn nicht.
TON: ffmpeg resampelt 44,1 kHz -> 37,8 kHz; je 28-Proben-Block Filter 0..3 und Shift 0..12 mit dem kleinsten Fehler,
gerechnet mit genau der Dekoder-Arithmetik von re15_xa.c (Vorhersage (old*K0 + older*K1 + 32) >> 6, Saettigung).
"""
import argparse
import hashlib
import os
import struct
import subprocess
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
QUELLE_REL = os.path.join("Cutscenes", "Rooms", "114", "Nachbau_Blender", "Raum114_Gorillas_v3_Ton.mp4")


def quelle_finden():
    """Cutscenes/ ist der unversionierte Arbeitsordner des Nutzers: er liegt nur im Hauptbaum, nicht in einem Worktree."""
    kand = [os.path.join(REPO, QUELLE_REL)]
    try:
        gemeinsam = subprocess.run(["git", "-C", REPO, "rev-parse", "--path-format=absolute", "--git-common-dir"],
                                   check=True, stdout=subprocess.PIPE, text=True).stdout.strip()
        kand.append(os.path.join(os.path.dirname(gemeinsam), QUELLE_REL))
    except (OSError, subprocess.CalledProcessError):
        pass
    for k in kand:
        if os.path.isfile(k):
            return k
    return kand[0]


ZIEL =os.path.join(PORT, "shared_assets", "PSX", "MOVIE", "R114GORI.STR")

W, H = 320, 240
FPS = 30
SEKTOR = 2336
NUTZ = 2016                      # re15_str.c STR_PAYLOAD_LEN
XA_RATE = 37800
XA_PROBEN = 18 * 4 * 28          # re15_xa.c XA_FRAMES_SEC = 2016 Stereo-Proben je Sektor

# bss_mdec.c ZSCAN / IQ_TABLE (Raster-Reihenfolge)
ZSCAN = [0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
         35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55,
         62, 63]
IQ = [2, 16, 19, 22, 26, 27, 29, 34, 16, 16, 22, 24, 27, 29, 34, 37, 19, 22, 26, 27, 29, 34, 34, 38, 22, 22, 26, 27, 29, 34,
      37, 40, 22, 26, 27, 29, 32, 35, 40, 48, 26, 27, 29, 32, 35, 40, 48, 58, 26, 27, 29, 34, 38, 46, 56, 69, 27, 29, 35, 38,
      46, 56, 69, 83]

# psxavenc/mdec.c ac_huffman_tree: (Bits ohne Vorzeichen, Code, Lauf, |Wert|); geschrieben wird (Code << 1 | Vorzeichen).
AC_TABELLE = [
    (2, 0x3, 0, 1), (3, 0x3, 1, 1), (4, 0x4, 0, 2), (4, 0x5, 2, 1), (5, 0x05, 0, 3), (5, 0x06, 4, 1), (5, 0x07, 3, 1),
    (6, 0x04, 7, 1), (6, 0x05, 6, 1), (6, 0x06, 1, 2), (6, 0x07, 5, 1), (7, 0x04, 2, 2), (7, 0x05, 9, 1), (7, 0x06, 0, 4),
    (7, 0x07, 8, 1), (8, 0x20, 13, 1), (8, 0x21, 0, 6), (8, 0x22, 12, 1), (8, 0x23, 11, 1), (8, 0x24, 3, 2), (8, 0x25, 1, 3),
    (8, 0x26, 0, 5), (8, 0x27, 10, 1), (10, 0x008, 16, 1), (10, 0x009, 5, 2), (10, 0x00A, 0, 7), (10, 0x00B, 2, 3),
    (10, 0x00C, 1, 4), (10, 0x00D, 15, 1), (10, 0x00E, 14, 1), (10, 0x00F, 4, 2), (12, 0x010, 0, 11), (12, 0x011, 8, 2),
    (12, 0x012, 4, 3), (12, 0x013, 0, 10), (12, 0x014, 2, 4), (12, 0x015, 7, 2), (12, 0x016, 21, 1), (12, 0x017, 20, 1),
    (12, 0x018, 0, 9), (12, 0x019, 19, 1), (12, 0x01A, 18, 1), (12, 0x01B, 1, 5), (12, 0x01C, 3, 3), (12, 0x01D, 0, 8),
    (12, 0x01E, 6, 2), (12, 0x01F, 17, 1), (13, 0x0010, 10, 2), (13, 0x0011, 9, 2), (13, 0x0012, 5, 3), (13, 0x0013, 3, 4),
    (13, 0x0014, 2, 5), (13, 0x0015, 1, 7), (13, 0x0016, 1, 6), (13, 0x0017, 0, 15), (13, 0x0018, 0, 14), (13, 0x0019, 0, 13),
    (13, 0x001A, 0, 12), (13, 0x001B, 26, 1), (13, 0x001C, 25, 1), (13, 0x001D, 24, 1), (13, 0x001E, 23, 1),
    (13, 0x001F, 22, 1), (14, 0x0010, 0, 31), (14, 0x0011, 0, 30), (14, 0x0012, 0, 29), (14, 0x0013, 0, 28),
    (14, 0x0014, 0, 27), (14, 0x0015, 0, 26), (14, 0x0016, 0, 25), (14, 0x0017, 0, 24), (14, 0x0018, 0, 23),
    (14, 0x0019, 0, 22), (14, 0x001A, 0, 21), (14, 0x001B, 0, 20), (14, 0x001C, 0, 19), (14, 0x001D, 0, 18),
    (14, 0x001E, 0, 17), (14, 0x001F, 0, 16), (15, 0x0010, 0, 40), (15, 0x0011, 0, 39), (15, 0x0012, 0, 38),
    (15, 0x0013, 0, 37), (15, 0x0014, 0, 36), (15, 0x0015, 0, 35), (15, 0x0016, 0, 34), (15, 0x0017, 0, 33),
    (15, 0x0018, 0, 32), (15, 0x0019, 1, 14), (15, 0x001A, 1, 13), (15, 0x001B, 1, 12), (15, 0x001C, 1, 11),
    (15, 0x001D, 1, 10), (15, 0x001E, 1, 9), (15, 0x001F, 1, 8), (16, 0x0010, 1, 18), (16, 0x0011, 1, 17),
    (16, 0x0012, 1, 16), (16, 0x0013, 1, 15), (16, 0x0014, 6, 3), (16, 0x0015, 16, 2), (16, 0x0016, 15, 2),
    (16, 0x0017, 14, 2), (16, 0x0018, 13, 2), (16, 0x0019, 12, 2), (16, 0x001A, 11, 2), (16, 0x001B, 31, 1),
    (16, 0x001C, 30, 1), (16, 0x001D, 29, 1), (16, 0x001E, 28, 1), (16, 0x001F, 27, 1),
]

# Codetabelle (Lauf 0..63, Wert & 0x3FF) -> (Code, Bits); sonst Escape 000001 + Lauf(6) + Wert(10) = 22 Bit.
LUT_CODE = np.zeros((64, 1024), dtype=np.int64)
LUT_BITS = np.full((64, 1024), 22, dtype=np.int64)
for _r in range(64):
    for _v in range(1024):
        LUT_CODE[_r, _v] = (1 << 16) | (_r << 10) | _v
for _b, _c, _r, _l in AC_TABELLE:
    LUT_CODE[_r, _l & 0x3FF] = (_c << 1) | 0
    LUT_BITS[_r, _l & 0x3FF] = _b + 1
    LUT_CODE[_r, (-_l) & 0x3FF] = (_c << 1) | 1
    LUT_BITS[_r, (-_l) & 0x3FF] = _b + 1

# orthonormale 8x8-DCT: F = D f D^T; DC = Summe/8 (= JPEG-FDCT, deren Umkehrung bss_mdec.c idct rechnet).
_D = np.zeros((8, 8))
for _k in range(8):
    for _n in range(8):
        _D[_k, _n] = 0.5 * (np.sqrt(0.5) if _k == 0 else 1.0) * np.cos((2 * _n + 1) * _k * np.pi / 16)
_IQ = np.array(IQ, dtype=np.float64).reshape(8, 8)
_ZS = np.array(ZSCAN)


def ffmpeg(args):
    return subprocess.run(["ffmpeg", "-v", "error", "-nostdin"] + args, check=True, stdout=subprocess.PIPE).stdout


def bilder_lesen(quelle):
    roh = ffmpeg(["-i", quelle, "-vf", "scale=%d:%d:flags=area" % (W, H), "-f", "rawvideo", "-pix_fmt", "rgb24", "-"])
    n = len(roh) // (W * H * 3)
    return np.frombuffer(roh[:n * W * H * 3], dtype=np.uint8).reshape(n, H, W, 3)


def ton_lesen(quelle):
    roh = ffmpeg(["-i", quelle, "-vn", "-ac", "2", "-ar", str(XA_RATE), "-f", "s16le", "-acodec", "pcm_s16le", "-"])
    return np.frombuffer(roh, dtype="<i2").reshape(-1, 2).astype(np.int64)


# ---------------------------------------------------------------------------------------------------------------- Video
def bloecke(rgb):
    """RGB-Bild -> (Makrobloecke spaltenweise x 6, 8, 8) Gleitkomma, Reihenfolge Cr, Cb, Y0, Y1, Y2, Y3."""
    f = rgb.astype(np.float64)
    r, g, b = f[..., 0], f[..., 1], f[..., 2]
    y = 0.299 * r + 0.587 * g + 0.114 * b - 128.0
    cb = -0.168736 * r - 0.331264 * g + 0.5 * b
    cr = 0.5 * r - 0.418688 * g - 0.081312 * b
    cb2 = cb.reshape(H // 2, 2, W // 2, 2).mean(axis=(1, 3))
    cr2 = cr.reshape(H // 2, 2, W // 2, 2).mean(axis=(1, 3))
    out = []
    for mx in range(W // 16):            # bss_mdec.c: for col ... for mb (Spalte fuer Spalte)
        for my in range(H // 16):
            yy = y[my * 16:my * 16 + 16, mx * 16:mx * 16 + 16]
            out.append(cr2[my * 8:my * 8 + 8, mx * 8:mx * 8 + 8])
            out.append(cb2[my * 8:my * 8 + 8, mx * 8:mx * 8 + 8])
            out.append(yy[0:8, 0:8]); out.append(yy[0:8, 8:16]); out.append(yy[8:16, 0:8]); out.append(yy[8:16, 8:16])
    return np.array(out)


def dct(bl):
    return np.einsum("ij,bjk,lk->bil", _D, bl, _D)


def quantisieren(F, qs):
    """-> (Bloecke, 64) ganzzahlig in Zickzack-Reihenfolge. DC = F/2 (DC-Rueckrechnung IQ[0]*Wert, IQ[0] = 2),
    AC = F*8/(IQ*qs) (Rueckrechnung (IQ*qs*Wert) >> 3); Klemmen wie psxavenc (0x1FF = BS-v2-Bildende).
    DC + 2: der Port-Dekoder rundet beim Herausskalieren nicht (bss_mdec.c descale = value >> 5, "No rounding"), das
    kostet im Mittel 0,5 Stufen je Pixel; ein DC-Schritt = 1/4 Stufe (DC-only-Pfad (8*DC) >> 5), +2 gleicht das aus."""
    q = np.empty_like(F)
    q[:] = F * 8.0 / (_IQ * qs)
    q[:, 0, 0] = F[:, 0, 0] / 2.0 + 2.0
    q = np.clip(np.rint(q), -0x200, 0x1FE).astype(np.int64)
    return q.reshape(-1, 64)[:, _ZS]


def bits_packen(codes, nbits):
    """Codes MSB zuerst hintereinander, 16-Bit-Worte little-endian (bss_vlc.c read_word_le/show_bits)."""
    gesamt = int(nbits.sum())
    pos = np.concatenate(([0], np.cumsum(nbits)[:-1]))
    bits = np.zeros(((gesamt + 15) // 16) * 16, dtype=np.uint8)
    for b in range(int(nbits.max())):
        m = nbits > b
        bits[pos[m] + b] = ((codes[m] >> (nbits[m] - 1 - b)) & 1).astype(np.uint8)
    be = np.packbits(bits)                              # MSB zuerst je Byte
    return be.reshape(-1, 2)[:, ::-1].tobytes()         # Wort-Bytes tauschen -> little-endian


def bild_kodieren(Z):
    """Z = (Bloecke, 64) Zickzack -> BS-v2-Bitstrom ohne Kopf, Zahl der MDEC-Halbworte."""
    nb = Z.shape[0]
    keys, codes, nbits = [], [], []
    keys.append(np.arange(nb) * 128)                    # DC: 10 Bit roh
    codes.append(Z[:, 0] & 0x3FF)
    nbits.append(np.full(nb, 10))
    bi, ki = np.nonzero(Z[:, 1:])
    ki = ki + 1
    if len(bi):
        neu = np.ones(len(bi), dtype=bool)
        neu[1:] = bi[1:] != bi[:-1]
        vor = np.empty_like(ki)
        vor[0] = 0
        vor[1:] = ki[:-1]
        vor[neu] = 0
        lauf = ki - vor - 1
        wert = Z[bi, ki] & 0x3FF
        keys.append(bi * 128 + ki)
        codes.append(LUT_CODE[lauf, wert])
        nbits.append(LUT_BITS[lauf, wert])
    keys.append(np.arange(nb) * 128 + 100)              # EOB `10`
    codes.append(np.full(nb, 2))
    nbits.append(np.full(nb, 2))
    k = np.concatenate(keys)
    o = np.argsort(k, kind="stable")
    c = np.concatenate(codes)[o]
    n = np.concatenate(nbits)[o]
    c = np.concatenate((c, [0x1FF]))                    # BS-v2-Bildende (psxavenc mdec.c end_of_block 0x1FF)
    n = np.concatenate((n, [10]))
    halbworte = 2 * nb + len(bi)                        # DC + AC + EOB je Block
    return bits_packen(c, n), halbworte


def bs_rahmen(Z, qs):
    strom, hw = bild_kodieren(Z)
    hw = ((hw + 2) + 0x3F) & ~0x3F                      # psxavenc: +2, auf 64 Halbworte (32-Wort-DMA) aufrunden
    worte = (hw + 1) >> 1
    kopf = struct.pack("<HHHH", worte, 0x3800, qs, 2)
    r = kopf + strom
    r += b"\0" * ((-len(r)) % 4)
    return r


# Python-Nachbau von bss_vlc.c (nur v2) zur Gegenprobe: liest den Strom zurueck in Zickzack-Werte je Block.
def strom_zurueck(strom, nb):
    w = np.frombuffer(strom + b"\0\0\0\0", dtype="<u2")
    bits = np.unpackbits(w.astype(">u2").view(np.uint8))
    s = "".join("1" if x else "0" for x in bits.tolist())
    p = 0
    out = np.zeros((nb, 64), dtype=np.int64)
    rueck = {}
    for (b, c, r, l) in AC_TABELLE:
        rueck[(b + 1, (c << 1) | 0)] = (r, l)
        rueck[(b + 1, (c << 1) | 1)] = (r, -l)
    for blk in range(nb):
        dc = int(s[p:p + 10], 2); p += 10
        out[blk, 0] = dc - 1024 if dc & 0x200 else dc
        k = 0
        while True:
            if s[p:p + 2] == "10":
                p += 2
                break
            if s[p:p + 6] == "000001":
                r = int(s[p + 6:p + 12], 2); v = int(s[p + 12:p + 22], 2); p += 22
                v = v - 1024 if v & 0x200 else v
            else:
                for L in range(3, 18):
                    t = rueck.get((L, int(s[p:p + L], 2)))
                    if t:
                        r, v = t; p += L
                        break
                else:
                    raise ValueError("Code unbekannt bei Bit %d" % p)
            k += r + 1
            out[blk, k] = v
    return out


def video_kodieren(bilder, budget_bytes, pruefen_jedes):
    rahmen, qss = [], []
    qs = 1
    for i, rgb in enumerate(bilder):
        F = dct(bloecke(rgb))
        qs = max(1, qs - 2)                             # Suche ab knapp unter dem Vorgaenger
        while True:
            Z = quantisieren(F, qs)
            r = bs_rahmen(Z, qs)
            if len(r) <= budget_bytes or qs >= 63:
                break
            qs += 1
        if len(r) > budget_bytes:
            sys.exit("ABBRUCH: Bild %d passt auch mit qscale 63 nicht in %d B" % (i + 1, budget_bytes))
        if pruefen_jedes and i % pruefen_jedes == 0:
            Zr = strom_zurueck(r[8:], Z.shape[0])
            if not np.array_equal(Zr, Z):
                sys.exit("ABBRUCH: Bild %d - Bitstrom liest nicht bitgleich zurueck" % (i + 1))
        rahmen.append(r)
        qss.append(qs)
    return rahmen, qss


# ----------------------------------------------------------------------------------------------------------------- Ton
K0 = np.array([0, 60, 115, 98])                         # re15_xa.c s_xa_k0 / s_xa_k1
K1 = np.array([0, 0, -52, -55])
_FIL = np.repeat(np.arange(4), 13)
_SH = np.tile(np.arange(13), 4)
_K0 = K0[_FIL]
_K1 = K1[_FIL]


def xa_block(x, old, older):
    """28 Proben eines Kanals -> (Param, Nibbles, old, older); alle Filter/Shift-Paare, kleinster quadratischer Fehler."""
    o1 = np.full(52, old, dtype=np.int64)
    o2 = np.full(52, older, dtype=np.int64)
    fehler = np.zeros(52, dtype=np.int64)
    nib = np.zeros((52, 28), dtype=np.int64)
    for j in range(28):
        pred = (o1 * _K0 + o2 * _K1 + 32) >> 6
        t = np.rint((x[j] - pred) * (2.0 ** _SH) / 4096.0).astype(np.int64)
        t = np.clip(t, -8, 7)
        v = ((t << 12) >> _SH) + pred
        v = np.clip(v, -32768, 32767)
        fehler += (v - x[j]) ** 2
        nib[:, j] = t & 0xF
        o2 = o1
        o1 = v
    i = int(np.argmin(fehler))
    return (int(_FIL[i]) << 4) | int(_SH[i]), nib[i], int(o1[i]), int(o2[i])


def ton_kodieren(pcm):
    n = len(pcm)
    sek = (n + XA_PROBEN - 1) // XA_PROBEN
    pcm = np.concatenate((pcm, np.zeros((sek * XA_PROBEN - n, 2), dtype=np.int64)))
    hist = [[0, 0], [0, 0]]
    sektoren = []
    for s in range(sek):
        daten = bytearray(18 * 128)
        for g in range(18):
            grp = bytearray(128)
            for blk in range(4):
                a = (s * 18 * 4 + g * 4 + blk) * 28
                for ch in range(2):
                    param, nib, o1, o2 = xa_block(pcm[a:a + 28, ch], hist[ch][0], hist[ch][1])
                    hist[ch] = [o1, o2]
                    u = blk * 2 + ch
                    grp[4 + u] = param
                    if u < 4:
                        grp[u] = param
                    else:
                        grp[8 + u] = param
                    for j in range(28):
                        grp[16 + blk + j * 4] |= int(nib[j]) << (ch * 4)
            daten[g * 128:(g + 1) * 128] = grp
        sektoren.append(bytes([1, 1, 0x64, 0x01, 1, 1, 0x64, 0x01]) + bytes(daten) + b"\0" * 24)
    return sektoren, n


# ---------------------------------------------------------------------------------------------------------------- Muxen
def video_sektoren(nr, rahmen):
    cnt = (len(rahmen) + NUTZ - 1) // NUTZ
    out = []
    for cur in range(cnt):
        kopf = struct.pack("<HHHHIIHH", 0x0160, 0x8001, cur, cnt, nr, len(rahmen), W, H) + rahmen[:8] + b"\0" * 4
        nutz = rahmen[cur * NUTZ:(cur + 1) * NUTZ]
        nutz += b"\0" * (NUTZ - len(nutz))
        out.append(bytes([0, 1, 0x48, 0, 0, 1, 0x48, 0]) + kopf + nutz + b"\0" * 280)
    return out


def muxen(rahmen, ton):
    """Ton-Sektor k (Proben k*2016 ..) kommt vor das Bild f, sobald k*2016/37800 < (f+1)/30 (Ton laeuft knapp voraus)."""
    sektoren, k = [], 0
    for f, r in enumerate(rahmen):
        while k < len(ton) and k * XA_PROBEN * FPS < (f + 1) * XA_RATE:
            sektoren.append(ton[k]); k += 1
        sektoren.extend(video_sektoren(f + 1, r))
    sektoren.extend(ton[k:])
    for s in sektoren:
        assert len(s) == SEKTOR
    return b"".join(sektoren)


def md5(p):
    h = hashlib.md5()
    with open(p, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--quelle", default=None)
    ap.add_argument("--ziel", default=ZIEL)
    ap.add_argument("--budget", type=int, default=10, help="Sektoren je Bild (re15_str.c STR_MAX_FRAME_SEC = 16)")
    ap.add_argument("--pruefen-jedes", type=int, default=1, help="jedes n-te Bild zuruecklesen (0 = aus)")
    a = ap.parse_args()
    if not 1 <= a.budget <= 16:
        sys.exit("--budget 1..16 (re15_str.c STR_MAX_FRAME_SEC)")
    a.quelle = a.quelle or quelle_finden()
    bilder = bilder_lesen(a.quelle)
    pcm = ton_lesen(a.quelle)
    print("Quelle %s: %d Bilder %dx%d, Ton %d Proben (%.4f s) bei %d Hz" % (os.path.basename(a.quelle), len(bilder), W, H,
                                                                          len(pcm), len(pcm) / XA_RATE, XA_RATE))
    rahmen, qss = video_kodieren(bilder, a.budget * NUTZ, a.pruefen_jedes)
    ton, n_ton = ton_kodieren(pcm)
    daten = muxen(rahmen, ton)
    os.makedirs(os.path.dirname(a.ziel), exist_ok=True)
    with open(a.ziel, "wb") as f:
        f.write(daten)
    sek_bild = [(len(r) + NUTZ - 1) // NUTZ for r in rahmen]
    print("Video: %d Bilder, qscale min/mittel/max %d/%.2f/%d, Sektoren je Bild min/max %d/%d" % (
        len(rahmen), min(qss), sum(qss) / len(qss), max(qss), min(sek_bild), max(sek_bild)))
    print("Ton: %d Sektoren (%.4f s)" % (len(ton), len(ton) * XA_PROBEN / XA_RATE))
    print("Ziel %s: %d B = %d Sektoren, md5 %s" % (a.ziel, len(daten), len(daten) // SEKTOR, md5(a.ziel)))
    txt = os.path.splitext(a.ziel)[0] + ".TXT"
    with open(txt, "w", encoding="ascii", newline="\n") as f:
        f.write("R114GORI.STR - Herkunft (Runde 41, Zwischensequenz Raum 114, ROOM1141 John und Elza)\n"
                "Quelle: Cutscenes/Rooms/114/Nachbau_Blender/%s (Film des Nutzers, md5 %s,\n"
                "        %d Bilder 30/s, Ton %.3f s). Datei unveraendert gelesen.\n"
                "Werkzeug: re15_port/tools/r41_zws114/film_bauen.py --budget %d\n"
                "Format wie MOVIE/CAPCOM.STR: 2336-Byte-Sektoren, MDEC BS v2 320x240, CD-XA 37800 Hz 4 Bit Stereo,\n"
                "EDC/ECC = 0 (nur fuer den PC-Port; der Port prueft sie nicht).\n"
                "Ergebnis: %d Bilder, qscale %d..%d (Mittel %.2f), %d Ton-Sektoren, %d B, md5 %s\n"
                "Abspielen: engine/src/zws114_r41.c + platform/pc/src/zws114_r41_pc.c (include/re15_zws114_r41.h).\n"
                % (os.path.basename(a.quelle), md5(a.quelle), len(bilder), len(pcm) / XA_RATE, a.budget, len(rahmen),
                   min(qss), max(qss), sum(qss) / len(qss), len(ton), len(daten), md5(a.ziel)))
    print("Herkunft: %s" % txt)


if __name__ == "__main__":
    main()
