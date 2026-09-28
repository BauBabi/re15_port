#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
do2_format.py -- byte-genauer Leser, Schreiber und Verfasser fuer das Tuerformat
                 DO2 (Container) / MD1 (Netz) / TIM (Textur) von RE1.5 und RE2 Retail.

Dossier mit allen Belegen: analysis/tor_1170/02_tuerformat.md
Messwerte:                 build/tor_1170/tuerformat.json   (von `alles` erzeugt)

ZWEI CONTAINER-VARIANTEN (beide an Bytes UND Lader belegt, Adressen = Instruktionen,
die im Dossier zitiert sind):

  RE1.5  (re15_port/shared_assets/PSX/DOOR/DOOR00.DO2, Lader FUN_800161e0)
    +0x00 u32 modell_off   @0x800162bc lw v0,0(a2)   a2=0x801a1000
    +0x04 u32 ton_off      @0x800162b4 lw v1,0(t0)   t0=0x801a1004
    +0x08 u32 vb_off       @0x800162b8 lw a1,0(a3)   a3=0x801a1008
    Modellblock @modell_off: drei u32  md1_rel, scd_rel, tim_rel
        Basis ist die KONSTANTE Dateibasis+0xC (@0x800162f4 ori a0,a0,0x100c),
        NICHT der Blockanfang. Deshalb schreibt der Verfasser modell_off = 0xC.
    danach MD1, SCD, TIM, Tonblock (Vorspann + VH + 8 B Nachspann), VB.

  RE2    (info/re2leon/COMMON/DOOR/DOORxx.DO2, Lader FUN_80014cd0 + FUN_80013c1c)
    KEIN Kopf. Datei = Tonteil + Nullen bis zur Sektorgrenze + Modellteil.
    Groessen stehen in der EXE, Tabelle @0x8009a520, 12 B je Tuer:
        +0 u16 ton_groesse  +2 u16 modell_groesse  +4 u32 modell_sektor
        +8 u8 pruefsumme_ton  +9 u8 pruefsumme_modell
    Tonteil:    16 B Vorspann, VH @0x10 (0xC20 B), 8 B Nachspann @0xC30, VB @0xC38
    Modellteil: u32 md1_rel, u32 tim_rel (Basis = Blockanfang), SCD @+8, MD1, TIM

MD1 (in beiden Spielen gleich, Lader FUN_80022150/FUN_8002288c bzw.
     FUN_80076b60/FUN_8002cfd8):
    +0 u32 tex_off (= Offset der Texturdaten ab MD1-Anfang)
    +4 u32 verschoben-Merker (Datei: 0)
    +8 u32 gruppen (= 2 x Meshzahl; je Mesh eine Dreiecks- und eine Vierecksgruppe)
    +12   Gruppentabelle, 28 B je Gruppe: vtx_off, vtx_anz, nrm_off, nrm_anz,
          prim_off, prim_anz, tex_off  -- alle Offsets ab MD1+12

Kein Wert in dieser Datei ist geschaetzt. Was nicht belegt ist, steht im Dossier
unter OFFEN und wird hier nicht als Konstante gefuehrt.
"""
from __future__ import annotations

import argparse
import collections
import glob
import hashlib
import json
import os
import struct
import sys
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Sequence, Tuple

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))

RE15_DOOR = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "DOOR", "DOOR00.DO2")
RE2_DOOR_GLOB = os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR", "*.DO2")
RE2_EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
RE15_EXE = os.path.join(REPO, "info", "Re1.5", "PSX.EXE")
AUSGABE = os.path.join(REPO, "build", "tor_1170")

# --------------------------------------------------------------------------------------
# Konstanten mit Herkunft
# --------------------------------------------------------------------------------------
RE15_LADEBASIS = 0x801A1000        # @0x800171f8/0x80017204 lui a1,0x801a / ori a1,a1,0x1000
RE15_REL_BASIS = 0x0C              # @0x800162f4 ori a0,a0,0x100c  (Basis der drei Modell-Offsets)
RE15_PRIM_PUFFER = 0x801AB000      # @0x8001635c lui v0,0x801a / @0x80016360 ori v0,v0,0xb000
RE15_TEILPUFFER = 0x801B1000       # @0x800166f4 lui v1,0x801b / @0x800166f8 ori v1,v1,0x1000
                                   # (anderes Halbbild: 0x801b7000 @0x800166c8/d4)
RE15_OBJEKTE = 4                   # @0x80016b24 slti v0,s4,4 (Objekt = 144 B, @0x80016b2c)
RE2_MODELLBASIS = 0x801A1000       # @0x80013d1c lui s0,0x801a / @0x80013d20 ori s0,s0,0x1000
RE2_TON_VH = 0x10                  # Nachspann-u32 @Datei 0xC30; @0x80014ea4 lw v0,-15568(v0)
                                   # = [0x801fc330], @0x80014ebc addu v0,v0,v1 (v1=0x801fb700)
RE2_TON_NACHSPANN = 0xC30          # @0x80014df0 / @0x80014e48 addiu a3,s0,3120
RE2_TON_VB = 0xC38                 # @0x80014f80 lui s1,0x801a / @0x80014f84 ori s1,s1,0x1c38
RE2_TABELLE = 0x8009A520           # @0x80014d4c lui v1,0x800a / @0x80014d50 addiu v1,v1,-23264
RE2_DATEIINDEX = 0x8009A4B0        # @0x80014d90 lhu a0,-23376(at)
RE2_OBJEKTE = 10                   # @0x80013c8c addiu a2,zero,9 .. @0x80013ccc bgez a2
                                   # (Objekt = 332 B, @0x80013cd0 addiu a1,a1,-332)
SEKTOR = 0x800                     # CD-Sektor, @0x80013868 addiu v0,v0,-2048
PRUEF_SCHRITT = 512                # @0x8001387c addiu s0,s0,512 (XOR jedes 512. Bytes)

MD1_KOPF = 12                      # @0x800163b0 addiu v0,v0,12 / @0x80013de4 addiu v1,v1,12
MD1_GRUPPE = 28                    # @0x800228c0 addiu a2,a2,28 / @0x8002cff8 addiu a1,a1,28
MD1_MESH = 56                      # @0x8001705c..6c (a1*8-a1)*8 / @0x80014b60..68
VEKTOR = 8                         # @0x80016be0 sll v0,v0,3 / @0x80014764 sll v0,v0,3
DREIECK = 12                       # @0x80016c78 addiu s3,s3,12 / @0x800148dc addiu fp,fp,12
VIERECK = 16                       # @0x8002579c addiu a3,a3,16 / @0x800257a4 addiu t2,t2,16
                                   # (FUN_800256b0; in den 56 Tuerdateien 0 Vierecke)
TEX_DREIECK = 12                   # @0x80025a6c addiu t1,t1,12
TEX_VIERECK = 16                   # @0x800221c8..d8: ((gruppe&1)+3) Worte; @0x80025bbc
PRIM_DREIECK = 80                  # @0x80025a64 addiu a3,a3,80 (2 x POLY_GT3 zu 40 B)
PRIM_VIERECK = 104                 # @0x80025ac0..d0: ((n*2+n)*4+n)*8 = n*104

RE2_TPAGE_PLUS = 0x15              # @0x80013d88 addiu a2,zero,21
RE2_CLUT_PLUS = 0x1F               # @0x80013d90 addiu a3,zero,31
TIM_SEITE = 0x15                   # @0x80016344 ori v0,zero,0x1f15 -> Byte 0 = Texturseite
TIM_CLUT_ZEILE = 0x1F              # ... Byte 1 = CLUT-Zeile; CLUT y = 480 + 0x1f (@0x8004ef38)

TIM_MAGIC = 0x10


class FormatFehler(Exception):
    """Die Bytes passen nicht zum belegten Aufbau."""


def u16(d: bytes, o: int) -> int:
    return struct.unpack_from("<H", d, o)[0]


def u32(d: bytes, o: int) -> int:
    return struct.unpack_from("<I", d, o)[0]


def auf(n: int, raster: int) -> int:
    return (n + raster - 1) // raster * raster


def pruefsumme_512(d: bytes) -> int:
    """XOR des jeweils ersten Bytes jedes 512-Byte-Abschnitts.

    RE2-Lesecallback LAB_8001376c: @0x80013878 lw v0,0(s0) / @0x8001387c addiu s0,s0,512 /
    @0x8001388c xor v1,v1,v0 / @0x80013894 sb v1,31(s1); Vergleich in FUN_80012fb8
    (DAT_800d531f == DAT_800d531e). Gemessen: 55/55 Ton- und 55/55 Modellteile treffen
    die Tabellenbytes @0x8009a520+8/+9.
    """
    x = 0
    for k in range(0, len(d), PRUEF_SCHRITT):
        x ^= d[k]
    return x


# ======================================================================================
# TIM
# ======================================================================================
@dataclass
class Tim:
    """PSX-TIM. Alle 56 Tuertexturen: flags=9 (8 bit + CLUT), CLUT 256x1 @(0,480),
    Bild 64 Worte x 256 Zeilen = 128x256 Pixel @(0,0), Gesamtlaenge 0x8220."""
    flags: int = 9
    clut_rect: Optional[Tuple[int, int, int, int]] = (0, 480, 256, 1)
    clut: List[int] = field(default_factory=list)
    pix_rect: Tuple[int, int, int, int] = (0, 0, 64, 256)
    pix: bytes = b""

    @property
    def farbtiefe(self) -> int:
        return {0: 4, 1: 8, 2: 16, 3: 24}[self.flags & 7]

    @property
    def breite_px(self) -> int:
        w = self.pix_rect[2]
        return {4: w * 4, 8: w * 2, 16: w, 24: w * 2 // 3}[self.farbtiefe]

    @property
    def hoehe_px(self) -> int:
        return self.pix_rect[3]

    @classmethod
    def lesen(cls, d: bytes, off: int = 0) -> Tuple["Tim", int]:
        """-> (Tim, Laenge in Bytes)."""
        if len(d) - off < 20:
            raise FormatFehler("TIM zu kurz")
        magic, flags = struct.unpack_from("<II", d, off)
        if magic != TIM_MAGIC:
            raise FormatFehler("TIM-Magic %#x statt 0x10 @%#x" % (magic, off))
        if flags & ~0xF or (flags & 7) > 3:
            raise FormatFehler("TIM-Flags %#x" % flags)
        p = off + 8
        clut_rect = None
        clut: List[int] = []
        if flags & 8:
            bnum, x, y, w, h = struct.unpack_from("<IHHHH", d, p)
            if bnum != 12 + w * h * 2:
                raise FormatFehler("CLUT-Blocklaenge %#x passt nicht zu %dx%d" % (bnum, w, h))
            clut = list(struct.unpack_from("<%dH" % (w * h), d, p + 12))
            clut_rect = (x, y, w, h)
            p += bnum
        bnum, x, y, w, h = struct.unpack_from("<IHHHH", d, p)
        if bnum != 12 + w * h * 2:
            raise FormatFehler("Bild-Blocklaenge %#x passt nicht zu %dx%d" % (bnum, w, h))
        if p + bnum > len(d):
            raise FormatFehler("TIM laeuft ueber das Ende")
        pix = bytes(d[p + 12:p + bnum])
        p += bnum
        return cls(flags, clut_rect, clut, (x, y, w, h), pix), p - off

    def schreiben(self) -> bytes:
        out = bytearray(struct.pack("<II", TIM_MAGIC, self.flags))
        if self.flags & 8:
            if self.clut_rect is None:
                raise FormatFehler("flags verlangen CLUT, clut_rect fehlt")
            x, y, w, h = self.clut_rect
            if len(self.clut) != w * h:
                raise FormatFehler("CLUT hat %d Eintraege, Rechteck %dx%d" % (len(self.clut), w, h))
            out += struct.pack("<IHHHH", 12 + w * h * 2, x, y, w, h)
            out += struct.pack("<%dH" % (w * h), *self.clut)
        x, y, w, h = self.pix_rect
        if len(self.pix) != w * h * 2:
            raise FormatFehler("Bilddaten %d B, Rechteck %d Worte x %d" % (len(self.pix), w, h))
        out += struct.pack("<IHHHH", 12 + w * h * 2, x, y, w, h)
        out += self.pix
        return bytes(out)

    def als_bild(self, clut_zeile: int = 0):
        """RGBA-Bild nach der GPU-Regel: Texelwert 0x0000 = nicht zeichnen."""
        from PIL import Image
        if self.farbtiefe != 8:
            raise FormatFehler("als_bild nur fuer 8 bit")
        w, h = self.breite_px, self.hoehe_px
        cw = self.clut_rect[2]
        pal = self.clut[clut_zeile * cw:(clut_zeile + 1) * cw]
        img = Image.new("RGBA", (w, h))
        px = img.load()
        for yy in range(h):
            for xx in range(w):
                v = pal[self.pix[yy * w + xx]]
                if v == 0:
                    px[xx, yy] = (0, 0, 0, 0)
                else:
                    r, g, b = v & 31, (v >> 5) & 31, (v >> 10) & 31
                    px[xx, yy] = (r << 3 | r >> 2, g << 3 | g >> 2, b << 3 | b >> 2, 255)
        return img


def farbe15(r: int, g: int, b: int, stp: int = 0) -> int:
    """8-bit-RGB -> PSX-Farbwort: Bit 0-4 R, 5-9 G, 10-14 B, Bit 15 STP."""
    return (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10) | ((stp & 1) << 15)


def tim_aus_bild(bild, *, alpha_schwelle: int = 128, schwarz: str = "stp",
                 index0_transparent: bool = True,
                 clut_xy: Tuple[int, int] = (0, 480),
                 pix_xy: Tuple[int, int] = (0, 0)) -> Tuple[Tim, Dict]:
    """8-bit-TIM aus einem RGB(A)-Bild.

    Quantisierung: zuerst auf das 15-bit-Raster der PSX (5 bit je Kanal). Passen die
    dann vorhandenen Farben in die Palette, wird NICHTS gerundet (verlustfrei gegenueber
    dem 15-bit-Bild). Sonst Median-Cut (Pillow) ohne Dithering auf die freie Platzzahl.

    Transparenz -- bewusst, nicht nebenbei:
      * Die GPU zeichnet einen Texel mit dem WERT 0x0000 nie (psx-spx, "Texture Color
        Black Limitations"). Transparente Bildpunkte (alpha < alpha_schwelle) bekommen
        deshalb Index 0, und CLUT[0] = 0x0000 (index0_transparent=True). Das ist auch
        der Stand der Originale: 48 von 56 Tuertexturen haben CLUT[0] = 0x0000.
      * Deckendes reines Schwarz wuerde als 0x0000 verschwinden. schwarz='stp' legt es
        als 0x8000 ab (Schwarz mit STP-Bit) -- so tun es die 6 Originaltexturen, die
        reines Schwarz zeichnen. Bei einem Objekt mit ABE-Bit (RE2-Objektflag 0x4000,
        @0x8001473c srl v0,v0,13 / andi v0,v0,0x2) wird ein STP-Texel halbtransparent
        gemischt; wer das nicht will, nimmt schwarz='anheben' (0x0421, dunkelstes Grau).
      * Alle uebrigen Farben tragen STP = 0.
    """
    import numpy as np
    from PIL import Image

    if schwarz not in ("stp", "anheben"):
        raise ValueError("schwarz muss 'stp' oder 'anheben' sein")
    img = bild.convert("RGBA")
    w, h = img.size
    if w % 2:
        raise FormatFehler("Breite %d ungerade: 8 bit braucht ganze VRAM-Worte" % w)
    a = np.asarray(img, dtype=np.uint8)
    durchsichtig = a[:, :, 3] < alpha_schwelle
    r5 = (a[:, :, 0] >> 3).astype(np.uint16)
    g5 = (a[:, :, 1] >> 3).astype(np.uint16)
    b5 = (a[:, :, 2] >> 3).astype(np.uint16)
    wert = r5 | (g5 << 5) | (b5 << 10)
    ersatz = 0x8000 if schwarz == "stp" else 0x0421
    plaetze = 256 - (1 if index0_transparent else 0)
    deckend = ~durchsichtig
    if not index0_transparent and durchsichtig.any():
        raise FormatFehler("Bild hat durchsichtige Punkte, aber index0_transparent=False")

    farben = np.unique(wert[deckend]) if deckend.any() else np.zeros(0, np.uint16)
    info: Dict = {"breite": w, "hoehe": h, "farben_15bit": int(len(farben)),
                  "plaetze": plaetze, "durchsichtige_punkte": int(durchsichtig.sum())}
    if len(farben) <= plaetze:
        palette = [int(v) for v in farben]
        nachschlag = {v: i for i, v in enumerate(palette)}
        idx = np.zeros((h, w), dtype=np.uint16)
        flach = wert.reshape(-1)
        idxf = idx.reshape(-1)
        for i, v in enumerate(flach):
            idxf[i] = nachschlag.get(int(v), 0)
        info["quantisiert"] = False
        info["groesster_fehler_5bit"] = 0
    else:
        rgb = np.stack([(r5 << 3 | r5 >> 2), (g5 << 3 | g5 >> 2), (b5 << 3 | b5 >> 2)],
                       axis=2).astype(np.uint8)
        q = Image.fromarray(rgb, "RGB").quantize(colors=plaetze,
                                                method=Image.Quantize.MEDIANCUT,
                                                dither=Image.Dither.NONE)
        pal = q.getpalette()[:plaetze * 3]
        palette = []
        for i in range(plaetze):
            rr, gg, bb = pal[i * 3:i * 3 + 3] if i * 3 + 2 < len(pal) else (0, 0, 0)
            palette.append((rr >> 3) | ((gg >> 3) << 5) | ((bb >> 3) << 10))
        # jeden Punkt auf den naechsten Paletteneintrag im 5-bit-Raum legen
        p5 = np.array([[v & 31, (v >> 5) & 31, (v >> 10) & 31] for v in palette], np.int32)
        pkt = np.stack([r5, g5, b5], axis=2).astype(np.int32).reshape(-1, 3)
        idxf = np.zeros(len(pkt), np.uint16)
        fehler = 0
        for s in range(0, len(pkt), 4096):
            teil = pkt[s:s + 4096]
            dist = ((teil[:, None, :] - p5[None, :, :]) ** 2).sum(axis=2)
            wahl = dist.argmin(axis=1)
            idxf[s:s + 4096] = wahl
            fehler = max(fehler, int(np.abs(teil - p5[wahl]).max()))
        idx = idxf.reshape(h, w)
        info["quantisiert"] = True
        info["groesster_fehler_5bit"] = fehler

    schwarz_da = 0
    clut_farben = []
    for v in palette:
        if v == 0:
            v = ersatz
            schwarz_da += 1
        clut_farben.append(v)
    basis = 1 if index0_transparent else 0
    clut = ([0x0000] if index0_transparent else []) + clut_farben
    clut += [0x0000] * (256 - len(clut))
    bildidx = (idx + basis).astype(np.uint16)
    bildidx[durchsichtig] = 0
    if int(bildidx.max()) > 255:
        raise FormatFehler("Index > 255")
    info["palette_belegt"] = len(clut_farben) + basis
    info["schwarz_ersetzt_durch"] = "%#06x" % ersatz if schwarz_da else None
    tim = Tim(flags=9, clut_rect=(clut_xy[0], clut_xy[1], 256, 1), clut=clut,
              pix_rect=(pix_xy[0], pix_xy[1], w // 2, h),
              pix=bildidx.astype(np.uint8).tobytes())
    return tim, info


# ======================================================================================
# MD1
# ======================================================================================
Vektor = Tuple[int, int, int, int]            # x, y, z, pad  (s16)
Dreieck = Tuple[int, int, int, int, int, int]  # n0, v0, n1, v1, n2, v2  (u16)
TexDreieck = Tuple[int, int, int, int, int, int, int, int, int]
# u0, v0, clut, u1, v1, tpage, u2, v2, pad


@dataclass
class Mesh:
    vertices: List[Vektor] = field(default_factory=list)
    normals: List[Vektor] = field(default_factory=list)
    tris: List[Dreieck] = field(default_factory=list)
    tri_tex: List[TexDreieck] = field(default_factory=list)
    quads: List[Tuple[int, ...]] = field(default_factory=list)      # n0,v0,...,n3,v3
    quad_tex: List[Tuple[int, ...]] = field(default_factory=list)   # 12 Felder

    def pruefen(self, name: str = "mesh") -> None:
        if len(self.tris) != len(self.tri_tex):
            raise FormatFehler("%s: %d Dreiecke, %d Textursaetze" %
                               (name, len(self.tris), len(self.tri_tex)))
        if len(self.quads) != len(self.quad_tex):
            raise FormatFehler("%s: %d Vierecke, %d Textursaetze" %
                               (name, len(self.quads), len(self.quad_tex)))
        nv, nn = len(self.vertices), len(self.normals)
        for t in self.tris:
            for k in (1, 3, 5):
                if not 0 <= t[k] < nv:
                    raise FormatFehler("%s: Vertexindex %d >= %d" % (name, t[k], nv))
            for k in (0, 2, 4):
                if not 0 <= t[k] < nn:
                    raise FormatFehler("%s: Normalenindex %d >= %d" % (name, t[k], nn))
        for q in self.quads:
            for k in (1, 3, 5, 7):
                if not 0 <= q[k] < nv:
                    raise FormatFehler("%s: Vertexindex %d >= %d" % (name, q[k], nv))
            for k in (0, 2, 4, 6):
                if not 0 <= q[k] < nn:
                    raise FormatFehler("%s: Normalenindex %d >= %d" % (name, q[k], nn))


def _lies_vektoren(d: bytes, o: int, n: int) -> List[Vektor]:
    return [struct.unpack_from("<4h", d, o + i * VEKTOR) for i in range(n)]


def _tex3(d: bytes, o: int) -> TexDreieck:
    return struct.unpack_from("<BBHBBHBBH", d, o)


def _tex4(d: bytes, o: int) -> Tuple[int, ...]:
    return struct.unpack_from("<BBHBBHBBHBBH", d, o)


def md1_belegung(d: bytes) -> Dict:
    """Belegungskarte: jedes Byte des MD1 genau einem Feld zuordnen.

    Dreiecks- und Vierecksgruppe eines Mesh zeigen auf DENSELBEN Vertex- und
    Normalenblock; der wird einmal gezaehlt. Liefert Abschnitte, Luecken, Ueberlappungen.
    """
    if len(d) < MD1_KOPF:
        raise FormatFehler("MD1 zu kurz")
    gruppen = u32(d, 8)
    teile: Dict[Tuple[int, int], str] = {}

    def merke(a: int, b: int, name: str) -> None:
        if b > a:
            alt = teile.get((a, b))
            teile[(a, b)] = name if alt is None or alt == name else alt + "=" + name

    merke(0, MD1_KOPF, "kopf")
    merke(MD1_KOPF, MD1_KOPF + gruppen * MD1_GRUPPE, "gruppentabelle")
    for g in range(gruppen):
        o = MD1_KOPF + g * MD1_GRUPPE
        vo, vc, no, nc, po, pc, to = struct.unpack_from("<7I", d, o)
        ps = DREIECK if g % 2 == 0 else VIERECK
        art = "dreiecke" if g % 2 == 0 else "vierecke"
        m = g // 2
        merke(MD1_KOPF + vo, MD1_KOPF + vo + vc * VEKTOR, "mesh%d.vertices" % m)
        merke(MD1_KOPF + no, MD1_KOPF + no + nc * VEKTOR, "mesh%d.normalen" % m)
        merke(MD1_KOPF + po, MD1_KOPF + po + pc * ps, "mesh%d.%s" % (m, art))
        merke(MD1_KOPF + to, MD1_KOPF + to + pc * ps, "mesh%d.%s_tex" % (m, art))
    zaehler = bytearray(len(d))
    ausserhalb = 0
    for (a, b) in teile:
        if b > len(d):
            ausserhalb += b - max(a, len(d))
        for i in range(a, min(b, len(d))):
            if zaehler[i] < 255:
                zaehler[i] += 1
    luecken = sum(1 for c in zaehler if c == 0)
    doppelt = sum(1 for c in zaehler if c > 1)
    abschnitte = [{"von": a, "bis": b, "feld": n} for (a, b), n in sorted(teile.items())]
    return {"laenge": len(d), "gruppen": gruppen, "abschnitte": abschnitte,
            "luecken_bytes": luecken, "ueberlappung_bytes": doppelt,
            "ausserhalb_bytes": ausserhalb,
            "lueckenlos": luecken == 0 and doppelt == 0 and ausserhalb == 0}


@dataclass
class Md1:
    meshes: List[Mesh] = field(default_factory=list)
    merker: int = 0   # +4: "schon verschoben"; der Lader setzt 1 (@0x800228ac sw v0,0(a0))

    # ---------------- Leser ----------------
    @classmethod
    def lesen(cls, d: bytes) -> "Md1":
        if len(d) < MD1_KOPF:
            raise FormatFehler("MD1 zu kurz")
        tex_off, merker, gruppen = struct.unpack_from("<3I", d, 0)
        if gruppen == 0 or gruppen % 2:
            raise FormatFehler("Gruppenzahl %d ist nicht gerade/positiv" % gruppen)
        if MD1_KOPF + gruppen * MD1_GRUPPE > len(d):
            raise FormatFehler("Gruppentabelle laeuft ueber das Ende")
        meshes: List[Mesh] = []
        for m in range(gruppen // 2):
            o = MD1_KOPF + m * MD1_MESH
            tvo, tvc, tno, tnc, tpo, tpc, tto = struct.unpack_from("<7I", d, o)
            qvo, qvc, qno, qnc, qpo, qpc, qto = struct.unpack_from("<7I", d, o + MD1_GRUPPE)
            if (tvo, tvc, tno, tnc) != (qvo, qvc, qno, qnc):
                raise FormatFehler("mesh%d: Vierecksgruppe nutzt andere Vertex-/Normalen"
                                   "bloecke als die Dreiecksgruppe" % m)
            b = MD1_KOPF
            for (off, anz, breite, was) in ((tvo, tvc, VEKTOR, "vertices"),
                                            (tno, tnc, VEKTOR, "normalen"),
                                            (tpo, tpc, DREIECK, "dreiecke"),
                                            (tto, tpc, TEX_DREIECK, "dreiecke_tex"),
                                            (qpo, qpc, VIERECK, "vierecke"),
                                            (qto, qpc, TEX_VIERECK, "vierecke_tex")):
                if b + off + anz * breite > len(d):
                    raise FormatFehler("mesh%d.%s laeuft ueber das Ende" % (m, was))
            mesh = Mesh(
                vertices=_lies_vektoren(d, b + tvo, tvc),
                normals=_lies_vektoren(d, b + tno, tnc),
                tris=[struct.unpack_from("<6H", d, b + tpo + i * DREIECK) for i in range(tpc)],
                tri_tex=[_tex3(d, b + tto + i * TEX_DREIECK) for i in range(tpc)],
                quads=[struct.unpack_from("<8H", d, b + qpo + i * VIERECK) for i in range(qpc)],
                quad_tex=[_tex4(d, b + qto + i * TEX_VIERECK) for i in range(qpc)],
            )
            meshes.append(mesh)
        md1 = cls(meshes=meshes, merker=merker)
        neu = md1.schreiben()
        if neu != bytes(d):
            raise FormatFehler("MD1 weicht vom Regelaufbau ab (%d B gelesen, %d B geschrieben, "
                               "tex_off Datei %#x)" % (len(d), len(neu), tex_off))
        return md1

    # ---------------- Schreiber ----------------
    def aufbau(self) -> Dict:
        """Offsets des Regelaufbaus (ab MD1+12), so wie alle 56 Originale ihn haben:
        Tabelle | alle Vertices | alle Normalen | je Mesh Dreiecke, Vierecke |
        je Mesh Dreiecks-Tex, Vierecks-Tex."""
        n = len(self.meshes)
        off = 2 * n * MD1_GRUPPE
        vo, no, po, qo, to, qto = [], [], [], [], [], []
        for m in self.meshes:
            vo.append(off)
            off += len(m.vertices) * VEKTOR
        for m in self.meshes:
            no.append(off)
            off += len(m.normals) * VEKTOR
        for m in self.meshes:
            po.append(off)
            off += len(m.tris) * DREIECK
            qo.append(off)
            off += len(m.quads) * VIERECK
        tex_anfang = off
        for m in self.meshes:
            to.append(off)
            off += len(m.tris) * TEX_DREIECK
            qto.append(off)
            off += len(m.quads) * TEX_VIERECK
        return {"vertices": vo, "normalen": no, "dreiecke": po, "vierecke": qo,
                "dreiecke_tex": to, "vierecke_tex": qto,
                "tex_anfang": tex_anfang, "ende": off}

    def schreiben(self) -> bytes:
        if not self.meshes:
            # FUN_8002288c / FUN_8002cfd8 sind do-while-Schleifen: die Gruppenzahl wird
            # VOR dem Test heruntergezaehlt (@0x800228b4 / @0x8002cfec). 0 Gruppen liefe
            # 2^32-mal.
            raise FormatFehler("MD1 ohne Mesh ist fuer den Lader nicht lesbar")
        for i, m in enumerate(self.meshes):
            m.pruefen("mesh%d" % i)
        a = self.aufbau()
        out = bytearray()
        out += struct.pack("<3I", a["tex_anfang"] + MD1_KOPF, self.merker, 2 * len(self.meshes))
        for i, m in enumerate(self.meshes):
            out += struct.pack("<7I", a["vertices"][i], len(m.vertices), a["normalen"][i],
                               len(m.normals), a["dreiecke"][i], len(m.tris),
                               a["dreiecke_tex"][i])
            out += struct.pack("<7I", a["vertices"][i], len(m.vertices), a["normalen"][i],
                               len(m.normals), a["vierecke"][i], len(m.quads),
                               a["vierecke_tex"][i])
        for m in self.meshes:
            for v in m.vertices:
                out += struct.pack("<4h", *v)
        for m in self.meshes:
            for v in m.normals:
                out += struct.pack("<4h", *v)
        for m in self.meshes:
            for t in m.tris:
                out += struct.pack("<6H", *t)
            for q in m.quads:
                out += struct.pack("<8H", *q)
        for m in self.meshes:
            for t in m.tri_tex:
                out += struct.pack("<BBHBBHBBH", *t)
            for q in m.quad_tex:
                out += struct.pack("<BBHBBHBBHBBH", *q)
        if len(out) != MD1_KOPF + a["ende"]:
            raise FormatFehler("interner Laengenfehler")
        return bytes(out)

    # ---------------- Kennzahlen ----------------
    def dreiecke_gesamt(self) -> int:
        return sum(len(m.tris) for m in self.meshes)

    def vierecke_gesamt(self) -> int:
        return sum(len(m.quads) for m in self.meshes)


# ======================================================================================
# SCD-Block der Tuer
# ======================================================================================
def scd_lesen(block: bytes) -> List[bytes]:
    """u16-Offsettabelle (Anzahl = erster Offset / 2), dahinter die Skripte.
    Das letzte Skript reicht bis zum Blockende und traegt damit ein etwaiges
    Auffuellpaar 00 00 (27 von 55 RE2-Bloecken enden auf 01 00 00 00)."""
    if len(block) < 2:
        raise FormatFehler("SCD-Block zu kurz")
    erster = u16(block, 0)
    if erster == 0 or erster % 2 or erster > len(block):
        raise FormatFehler("SCD: erster Offset %#x" % erster)
    n = erster // 2
    offs = [u16(block, 2 * i) for i in range(n)]
    for i in range(n - 1):
        if offs[i + 1] < offs[i]:
            raise FormatFehler("SCD: Offsets nicht aufsteigend")
    if offs[-1] > len(block):
        raise FormatFehler("SCD: Offset hinter dem Blockende")
    grenzen = offs + [len(block)]
    return [bytes(block[grenzen[i]:grenzen[i + 1]]) for i in range(n)]


def scd_schreiben(skripte: Sequence[bytes]) -> bytes:
    """Tabelle + Skripte, am Ende mit 00 auf ein Vielfaches von 4 aufgefuellt.
    Die 4 ist keine Schoenheit: dahinter liegt das MD1 (RE2) bzw. die TIM (RE1.5),
    und beide werden mit `lw` gelesen -- auf dem R3000 nur an durch 4 teilbaren
    Adressen. Alle 56 Originalbloecke haben eine durch 4 teilbare Laenge."""
    if not skripte:
        raise FormatFehler("SCD braucht mindestens ein Skript")
    out = bytearray()
    off = 2 * len(skripte)
    for s in skripte:
        if off > 0xFFFF:
            raise FormatFehler("SCD-Offset > 0xFFFF")
        out += struct.pack("<H", off)
        off += len(s)
    for s in skripte:
        out += s
    out += b"\0" * (auf(len(out), 4) - len(out))
    return bytes(out)


# ======================================================================================
# DO2
# ======================================================================================
@dataclass
class Do2:
    variante: str                 # 're15' | 're2'
    md1: bytes = b""
    skripte: List[bytes] = field(default_factory=list)
    tim: bytes = b""
    ton_vorspann: bytes = b""     # SE-Tabelle, 4 B je Eintrag (FUN_80045024: puVar9 + id*4)
    vh: bytes = b""
    ton_nachspann: bytes = b""    # 8 B; u32 = Offset des VH im Tonblock
    vb: bytes = b""

    # ---------------- Leser ----------------
    @classmethod
    def lesen(cls, d: bytes) -> "Do2":
        if len(d) >= 0x14 and d[0x10:0x14] == b"pBAV":
            return cls._lesen_re2(d)
        return cls._lesen_re15(d)

    @classmethod
    def _lesen_re15(cls, d: bytes) -> "Do2":
        modell, ton, vb = struct.unpack_from("<3I", d, 0)
        if modell != RE15_REL_BASIS:
            raise FormatFehler("RE1.5: modell_off %#x, der Lader rechnet fest mit 0xC" % modell)
        if not (modell < ton < vb <= len(d)):
            raise FormatFehler("RE1.5: Kopf-Offsets nicht aufsteigend")
        md1_rel, scd_rel, tim_rel = struct.unpack_from("<3I", d, modell)
        md1_a, scd_a, tim_a = (RE15_REL_BASIS + md1_rel, RE15_REL_BASIS + scd_rel,
                               RE15_REL_BASIS + tim_rel)
        if md1_a != modell + 12:
            raise FormatFehler("RE1.5: MD1 beginnt nicht direkt hinter den drei Offsets")
        if not (md1_a < scd_a < tim_a < ton):
            raise FormatFehler("RE1.5: Modell-Offsets nicht aufsteigend")
        tonblock = d[ton:vb]
        if len(tonblock) < 8 + 32:
            raise FormatFehler("RE1.5: Tonblock zu kurz")
        nach = tonblock[-8:]
        vh_off = u32(nach, 0)
        if tonblock[vh_off:vh_off + 4] != b"pBAV":
            raise FormatFehler("RE1.5: Nachspann zeigt nicht auf pBAV")
        o = cls("re15", md1=bytes(d[md1_a:scd_a]), skripte=scd_lesen(d[scd_a:tim_a]),
                tim=bytes(d[tim_a:ton]), ton_vorspann=bytes(tonblock[:vh_off]),
                vh=bytes(tonblock[vh_off:-8]), ton_nachspann=bytes(nach), vb=bytes(d[vb:]))
        return o

    @classmethod
    def _lesen_re2(cls, d: bytes) -> "Do2":
        nach = d[RE2_TON_NACHSPANN:RE2_TON_NACHSPANN + 8]
        vh_off = u32(nach, 0)
        if vh_off != RE2_TON_VH:
            raise FormatFehler("RE2: Nachspann %#x statt 0x10" % vh_off)
        gesamt = u32(d, RE2_TON_VH + 12)             # VabHdr.fsize = VH + VB
        vh_len = RE2_TON_NACHSPANN - RE2_TON_VH
        vb_len = gesamt - vh_len
        ton_ende = RE2_TON_VB + vb_len
        modell = auf(ton_ende, SEKTOR)
        if modell + 8 > len(d):
            raise FormatFehler("RE2: Modellteil fehlt")
        if any(d[ton_ende:modell]):
            raise FormatFehler("RE2: Auffuellung vor dem Modellteil ist nicht null")
        md1_rel, tim_rel = struct.unpack_from("<2I", d, modell)
        if not (8 < md1_rel < tim_rel < len(d) - modell):
            raise FormatFehler("RE2: Modell-Offsets nicht aufsteigend")
        return cls("re2", md1=bytes(d[modell + md1_rel:modell + tim_rel]),
                   skripte=scd_lesen(d[modell + 8:modell + md1_rel]),
                   tim=bytes(d[modell + tim_rel:]), ton_vorspann=bytes(d[:RE2_TON_VH]),
                   vh=bytes(d[RE2_TON_VH:RE2_TON_NACHSPANN]), ton_nachspann=bytes(nach),
                   vb=bytes(d[RE2_TON_VB:ton_ende]))

    def lage(self) -> Dict:
        """Dateioffsets aller Teile der Datei, die schreiben() liefert."""
        scd = scd_schreiben(self.skripte)
        if self.variante == "re2":
            ton_ende = len(self.tonteil())
            modell = auf(ton_ende, SEKTOR)
            md1 = modell + 8 + len(scd)
            tim = md1 + len(self.md1)
            return {"ton_vorspann": 0, "vh": len(self.ton_vorspann),
                    "ton_nachspann": len(self.ton_vorspann) + len(self.vh),
                    "vb": len(self.ton_vorspann) + len(self.vh) + 8, "ton_ende": ton_ende,
                    "modell": modell, "scd": modell + 8, "md1": md1, "tim": tim,
                    "ende": tim + len(self.tim)}
        md1 = RE15_REL_BASIS + 12
        scd_a = md1 + len(self.md1)
        tim = scd_a + len(scd)
        ton = tim + len(self.tim)
        vh = ton + len(self.ton_vorspann)
        nach = vh + len(self.vh)
        return {"kopf": 0, "modell": RE15_REL_BASIS, "md1": md1, "scd": scd_a, "tim": tim,
                "ton_vorspann": ton, "vh": vh, "ton_nachspann": nach, "vb": nach + 8,
                "ende": nach + 8 + len(self.vb)}

    # ---------------- Schreiber ----------------
    def tonteil(self) -> bytes:
        """RE2: Vorspann + VH + Nachspann + VB (ohne Auffuellung)."""
        return self.ton_vorspann + self.vh + self.ton_nachspann + self.vb

    def modellteil(self) -> bytes:
        scd = scd_schreiben(self.skripte)
        if self.variante == "re2":
            md1_rel = 8 + len(scd)
            tim_rel = md1_rel + len(self.md1)
            return struct.pack("<2I", md1_rel, tim_rel) + scd + self.md1 + self.tim
        md1_rel = 12
        scd_rel = md1_rel + len(self.md1)
        tim_rel = scd_rel + len(scd)
        return struct.pack("<3I", md1_rel, scd_rel, tim_rel) + self.md1 + scd + self.tim

    def schreiben(self) -> bytes:
        if self.variante == "re2":
            ton = self.tonteil()
            return ton + b"\0" * (auf(len(ton), SEKTOR) - len(ton)) + self.modellteil()
        if self.variante != "re15":
            raise FormatFehler("unbekannte Variante %r" % self.variante)
        modell = self.modellteil()
        ton_off = RE15_REL_BASIS + len(modell)
        tonblock = self.ton_vorspann + self.vh + self.ton_nachspann
        vb_off = ton_off + len(tonblock)
        return (struct.pack("<3I", RE15_REL_BASIS, ton_off, vb_off)
                + modell + tonblock + self.vb)

    # ---------------- EXE-Tabellen, die zur Datei gehoeren ----------------
    def re2_tabelleneintrag(self) -> Dict:
        """Die 12 Bytes @0x8009a520 + id*12, die der RE2-Lader zu dieser Datei erwartet."""
        if self.variante != "re2":
            raise FormatFehler("nur RE2 hat die Tuertabelle")
        ton = self.tonteil()
        modell = self.modellteil()
        if len(ton) > 0xFFFF or len(modell) > 0xFFFF:
            raise FormatFehler("Ton- oder Modellteil > 0xFFFF: die Tabelle fuehrt u16")
        e = {"ton_groesse": len(ton), "modell_groesse": len(modell),
             "modell_sektor": auf(len(ton), SEKTOR) // SEKTOR,
             "pruefsumme_ton": pruefsumme_512(ton),
             "pruefsumme_modell": pruefsumme_512(modell)}
        e["bytes"] = struct.pack("<HHIBBH", e["ton_groesse"], e["modell_groesse"],
                                 e["modell_sektor"], e["pruefsumme_ton"],
                                 e["pruefsumme_modell"], 0).hex()
        return e


def lader_pruefung(d: bytes) -> Dict:
    """Spielt die Rechenschritte des Original-Laders an den Bytes nach und meldet jede
    Stelle, an der er aus dem Tritt kaeme. Keine Emulation -- nur die Adressrechnung,
    die in den zitierten Instruktionen steht."""
    do2 = Do2.lesen(d)
    fehler: List[str] = []
    hinweise: List[str] = []
    md1 = do2.md1
    gruppen = u32(md1, 8)
    if u32(md1, 4) != 0:
        fehler.append("MD1+4 = %d: RE1.5 ueberspringt dann das Verschieben "
                      "(@0x80022188 bne v0,zero)" % u32(md1, 4))
    if gruppen == 0:
        fehler.append("MD1 gruppen = 0: do-while @0x800228e8 / @0x8002d020 laeuft ueber")
    if gruppen % 2:
        fehler.append("MD1 gruppen ungerade: Mesh = 56 B = zwei Gruppen")
    # Texturdaten: der Lader kopiert EINEN zusammenhaengenden Block ab gruppe[0].tex und
    # verteilt die Zeiger danach der Reihe nach (@0x8002229c..cc / @0x80076c4c..7c).
    erwartet = u32(md1, MD1_KOPF + 24)
    for g in range(gruppen):
        o = MD1_KOPF + g * MD1_GRUPPE
        anz, tex = u32(md1, o + 20), u32(md1, o + 24)
        if tex != erwartet:
            fehler.append("gruppe %d: tex_off %#x, der Lader setzt %#x" % (g, tex, erwartet))
        erwartet += anz * (TEX_DREIECK if g % 2 == 0 else TEX_VIERECK)
    if u32(md1, 0) != u32(md1, MD1_KOPF + 24) + MD1_KOPF:
        hinweise.append("MD1+0 weicht von gruppe[0].tex + 12 ab (kein Lader liest MD1+0 "
                        "zum Rechnen; alle 56 Originale halten die Gleichung)")
    bel = md1_belegung(md1)
    if not bel["lueckenlos"]:
        fehler.append("MD1-Belegung: %d Luecken-, %d Doppel-, %d Aussen-Bytes" %
                      (bel["luecken_bytes"], bel["ueberlappung_bytes"], bel["ausserhalb_bytes"]))
    try:
        tim, tl = Tim.lesen(do2.tim)
        if tl != len(do2.tim):
            fehler.append("TIM: %d B Rest hinter dem Bildblock" % (len(do2.tim) - tl))
    except FormatFehler as e:
        fehler.append("TIM: %s" % e)
        tim = None
    if len(do2.ton_nachspann) != 8 or do2.vh[:4] != b"pBAV":
        fehler.append("Tonblock: Nachspann/VH")
    quads = sum(u32(md1, MD1_KOPF + g * MD1_GRUPPE + 20) for g in range(1, gruppen, 2))
    if quads:
        hinweise.append("%d Vierecke: die Tuer-Zeichenroutinen FUN_80016b54 / FUN_8001468c "
                        "zeichnen nur die Dreiecksgruppe" % quads)

    if do2.variante == "re15":
        modell, ton, vb = struct.unpack_from("<3I", d, 0)
        md1_a = RE15_REL_BASIS + u32(d, modell)
        scd_a = RE15_REL_BASIS + u32(d, modell + 4)
        tim_a = RE15_REL_BASIS + u32(d, modell + 8)
        for name, a in (("MD1", md1_a), ("TIM", tim_a), ("Tonblock", ton)):
            if a % 4:
                fehler.append("%s @%#x nicht durch 4 teilbar (lw)" % (name, a))
        if (vb - ton) % 4:
            fehler.append("Tonblocklaenge %#x nicht durch 4 teilbar "
                          "(@0x80017128 lw v0,-8968(at))" % (vb - ton))
        grenze = RE15_PRIM_PUFFER - RE15_LADEBASIS
        if tim_a > grenze:
            fehler.append("MD1+SCD reichen bis %#x, ab Dateioffset %#x schreibt der Lader "
                          "Primitive" % (tim_a, grenze))
        ziel = grenze - (tim_a - scd_a)
        if ziel < tim_a:
            fehler.append("SCD-Kopie (FUN_800171b4) landet bei %#x im MD1/SCD" % ziel)
    else:
        e = do2.re2_tabelleneintrag()
        modell = e["modell_sektor"] * SEKTOR
        if (modell + u32(d, modell)) % 4 or (modell + u32(d, modell + 4)) % 4:
            fehler.append("MD1 oder TIM nicht durch 4 teilbar (lw)")
    return {"variante": do2.variante, "fehler": fehler, "hinweise": hinweise,
            "lesbar": not fehler}


# ======================================================================================
# Verfasser
# ======================================================================================
def _normale(p0, p1, p2) -> Tuple[int, int, int]:
    """Flaechennormale im Drehsinn der Originale, Laenge 4096.

    Richtung = (p2-p0) x (p1-p0). Gemessen an allen 56 Modellen (8605 Dreiecke): die
    gespeicherte Normale zeigt in 8455 Dreiecken in genau diese Richtung, in 137
    dagegen, 4 stehen senkrecht, 9 Flaechen sind entartet -- siehe `konventionen` ->
    normale_gegen_kreuzprodukt. Dieselbe Richtung folgt aus dem Lader: gezeichnet wird
    bei NCLIP >= 0, und fuer eine solche Flaeche zeigt (p2-p0) x (p1-p0) zur Kamera.
    """
    ax, ay, az = p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2]
    bx, by, bz = p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]
    nx, ny, nz = ay * bz - az * by, az * bx - ax * bz, ax * by - ay * bx
    l = (nx * nx + ny * ny + nz * nz) ** 0.5
    if l == 0:
        raise FormatFehler("entartetes Dreieck %r %r %r" % (p0, p1, p2))
    return (int(round(nx * 4096 / l)), int(round(ny * 4096 / l)), int(round(nz * 4096 / l)))


def mesh_aus_dreiecken(dreiecke: Sequence[Sequence], *, tpage: int = 0x80,
                       clut: int = 0x7800, doppelseitig: bool = False) -> Mesh:
    """Mesh aus Dreiecken [(p0,uv0),(p1,uv1),(p2,uv2)], p = (x,y,z), uv = (u,v).

    Die Eckenreihenfolge ist die der Datei. Sichtbar ist die Seite, von der aus die
    Ecken auf dem Bildschirm im Uhrzeigersinn laufen (NCLIP >= 0 zeichnet:
    @0x80016d24 bltz v0 / @0x800148f8 bgez v0).
    tpage/clut sind die DATEI-Werte: 0x80 = 8 bit, Seite 0; 0x7800 = CLUT-Zeile 480.
    RE2 zaehlt beim Laden 0x15 bzw. 0x1f<<6 dazu (FUN_80076b60).
    doppelseitig=True legt jede Flaeche ein zweites Mal mit umgekehrtem Drehsinn an.
    """
    m = Mesh()
    vidx: Dict[Tuple[int, int, int], int] = {}
    nidx: Dict[Tuple[int, int, int], int] = {}

    def v(p) -> int:
        k = (int(p[0]), int(p[1]), int(p[2]))
        for c in k:
            if not -32768 <= c <= 32767:
                raise FormatFehler("Koordinate %d passt nicht in s16" % c)
        if k not in vidx:
            vidx[k] = len(m.vertices)
            m.vertices.append((k[0], k[1], k[2], 0))
        return vidx[k]

    def n(k) -> int:
        if k not in nidx:
            nidx[k] = len(m.normals)
            m.normals.append((k[0], k[1], k[2], 0))
        return nidx[k]

    def eins(e0, e1, e2) -> None:
        (p0, t0), (p1, t1), (p2, t2) = e0, e1, e2
        for (uu, vv) in (t0, t1, t2):
            if not (0 <= uu <= 255 and 0 <= vv <= 255):
                raise FormatFehler("UV (%d,%d) ausserhalb 0..255" % (uu, vv))
        ni = n(_normale(p0, p1, p2))
        m.tris.append((ni, v(p0), ni, v(p1), ni, v(p2)))
        m.tri_tex.append((t0[0], t0[1], clut, t1[0], t1[1], tpage, t2[0], t2[1], 0))

    for d in dreiecke:
        eins(d[0], d[1], d[2])
        if doppelseitig:
            eins(d[0], d[2], d[1])
    m.pruefen()
    return m


def md1_bauen(meshes: Sequence[Mesh], merker: int = 0) -> Md1:
    """MD1 im Regelaufbau. Rueckgabe .schreiben() liefert die Bytes."""
    md1 = Md1(meshes=list(meshes), merker=merker)
    md1.schreiben()          # prueft Indizes und Laengen
    return md1


def evt_end_skript() -> bytes:
    """Das eine Skript aus RE1.5 DOOR00 (@Datei 0x9A6: 01 00)."""
    return b"\x01\x00"


def do2_bauen(md1, tim, skripte: Sequence[bytes], *, ton: Optional[Do2] = None,
              vh: Optional[bytes] = None, vb: Optional[bytes] = None,
              ton_vorspann: Optional[bytes] = None, ton_nachspann: Optional[bytes] = None,
              variante: str = "re15") -> Do2:
    """DO2 zusammensetzen. md1: Md1 oder bytes, tim: Tim oder bytes.

    Ton: entweder ein vorhandenes Do2 (ton=...), dessen vier Tonteile uebernommen
    werden, oder die vier Teile einzeln. Ein eigener Ton wird hier nicht erzeugt.
    Der Nachspann wird passend zur Vorspannlaenge geschrieben, wenn er fehlt.
    """
    md1_b = md1.schreiben() if isinstance(md1, Md1) else bytes(md1)
    tim_b = tim.schreiben() if isinstance(tim, Tim) else bytes(tim)
    if ton is not None:
        vh = ton.vh if vh is None else vh
        vb = ton.vb if vb is None else vb
        ton_vorspann = ton.ton_vorspann if ton_vorspann is None else ton_vorspann
    if vh is None or vb is None or ton_vorspann is None:
        raise FormatFehler("Tonteile fehlen: ton=... oder vh/vb/ton_vorspann angeben")
    soll = 16 if variante == "re2" else 8
    if len(ton_vorspann) != soll:
        # RE2 liest den Nachspann an der FESTEN Stelle 0xC30 und das VB bei 0xC38;
        # RE1.5 DOOR00 hat 8 B. Eine andere Laenge waere ein ungemessener Fall.
        if variante == "re2":
            raise FormatFehler("RE2: Vorspann muss 16 B sein (VH @0x10, Nachspann @0xC30)")
    if variante == "re2" and len(vh) != RE2_TON_NACHSPANN - RE2_TON_VH:
        raise FormatFehler("RE2: VH muss 0xC20 B sein")
    if ton_nachspann is None:
        ton_nachspann = struct.pack("<II", len(ton_vorspann), 0)
    do2 = Do2(variante, md1=md1_b, skripte=[bytes(s) for s in skripte], tim=tim_b,
              ton_vorspann=bytes(ton_vorspann), vh=bytes(vh),
              ton_nachspann=bytes(ton_nachspann), vb=bytes(vb))
    befund = lader_pruefung(do2.schreiben())
    if not befund["lesbar"]:
        raise FormatFehler("Lader-Pruefung: " + "; ".join(befund["fehler"]))
    return do2


# ======================================================================================
# Abnahme, Konventionen, Probemodell
# ======================================================================================
def alle_archive() -> List[Tuple[str, str]]:
    """[(Name, Pfad)] -- RE1.5 DOOR00 zuerst, dann die 55 RE2-Archive."""
    out = [("RE15/DOOR00", RE15_DOOR)]
    for p in sorted(glob.glob(RE2_DOOR_GLOB)):
        out.append(("RE2/" + os.path.splitext(os.path.basename(p))[0], p))
    return out


def abnahme() -> Dict:
    ergebnis = {"archive": [], "anzahl": 0, "byte_identisch": 0, "md1_lueckenlos": 0,
                "md1_regelaufbau": 0, "tim_identisch": 0, "lader_lesbar": 0}
    for name, pfad in alle_archive():
        d = open(pfad, "rb").read()
        z: Dict = {"name": name, "pfad": os.path.relpath(pfad, REPO).replace("\\", "/"),
                   "laenge": len(d), "sha1": hashlib.sha1(d).hexdigest()}
        do2 = Do2.lesen(d)
        neu = do2.schreiben()
        z["byte_identisch"] = neu == d
        z["variante"] = do2.variante
        z["skripte"] = len(do2.skripte)
        z["md1_laenge"] = len(do2.md1)
        bel = md1_belegung(do2.md1)
        z["md1_belegung"] = bel
        try:
            md1 = Md1.lesen(do2.md1)
            z["md1_regelaufbau"] = md1.schreiben() == do2.md1
            z["meshes"] = [{"vertices": len(m.vertices), "normalen": len(m.normals),
                            "dreiecke": len(m.tris), "vierecke": len(m.quads)}
                           for m in md1.meshes]
        except FormatFehler as e:
            z["md1_regelaufbau"] = False
            z["md1_fehler"] = str(e)
        tim, tl = Tim.lesen(do2.tim)
        z["tim_identisch"] = tim.schreiben() == do2.tim
        z["tim"] = {"flags": tim.flags, "clut_rect": tim.clut_rect, "pix_rect": tim.pix_rect,
                    "breite_px": tim.breite_px, "hoehe_px": tim.hoehe_px,
                    "laenge": len(do2.tim)}
        lp = lader_pruefung(d)
        z["lader"] = lp
        ergebnis["archive"].append(z)
        ergebnis["anzahl"] += 1
        ergebnis["byte_identisch"] += int(z["byte_identisch"])
        ergebnis["md1_lueckenlos"] += int(bel["lueckenlos"])
        ergebnis["md1_regelaufbau"] += int(z["md1_regelaufbau"])
        ergebnis["tim_identisch"] += int(z["tim_identisch"])
        ergebnis["lader_lesbar"] += int(lp["lesbar"])
    return ergebnis


def re2_tabelle_pruefen() -> Dict:
    """EXE-Tabelle @0x8009a520 gegen die 55 Dateien."""
    exe = open(RE2_EXE, "rb").read()
    t_addr = u32(exe, 0x18)

    def ab(adr: int) -> int:
        return 0x800 + adr - t_addr

    treffer = 0
    zeilen = []
    pfade = sorted(glob.glob(RE2_DOOR_GLOB))
    for i, p in enumerate(pfade):
        d = open(p, "rb").read()
        roh = exe[ab(RE2_TABELLE) + i * 12: ab(RE2_TABELLE) + i * 12 + 12]
        e = Do2.lesen(d).re2_tabelleneintrag()
        gleich = roh.hex() == e["bytes"]
        treffer += int(gleich)
        zeilen.append({"tuer": os.path.basename(p), "exe": roh.hex(), "aus_datei": e["bytes"],
                       "dateiindex": u16(exe, ab(RE2_DATEIINDEX) + i * 2), "gleich": gleich})
    return {"adresse": "%#x" % RE2_TABELLE, "eintraege": len(pfade), "gleich": treffer,
            "zeilen": zeilen}


def re15_dateitabelle() -> Dict:
    """RE1.5-Dateitabelle @0x8006f43c, Eintrag 0x25 (DOOR00)."""
    exe = open(RE15_EXE, "rb").read()
    t_addr = u32(exe, 0x18)
    o = 0x800 + 0x8006F43C + 0x25 * 8 - t_addr
    d = open(RE15_DOOR, "rb").read()
    return {"adresse": "%#x" % (0x8006F43C + 0x25 * 8), "bytes": exe[o:o + 8].hex(),
            "groesse_exe": u32(exe, o), "groesse_datei": len(d),
            "byte7_exe": exe[o + 7], "xor512_datei": pruefsumme_512(d),
            "tuerindex_tabelle_0x80071d2c": exe[0x800 + 0x80071D2C - t_addr:
                                               0x800 + 0x80071D2C - t_addr + 4].hex()}


def _kreuz(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def _objektsaetze(skripte: Sequence[bytes], opcode: int) -> List[Dict]:
    """Aufbau-Saetze (22 B) aus den Skripten fischen. Kein SCD-Lauf: gesucht wird das
    Opcode-Byte an einer Stelle, an der Byte 4 (aktiv) 0/1 ist, der Meshindex passt und
    der Satz ganz im Skript liegt. Reicht fuer die Lage-Statistik, ersetzt keinen
    Disassembler."""
    out = []
    for si, s in enumerate(skripte):
        p = 0
        while p + 22 <= len(s):
            if s[p] == opcode and s[p + 4] in (0, 1) and s[p + 1] < 16 and s[p + 5] < 16:
                f = struct.unpack_from("<BBBBBBHh3h3H", s, p)
                out.append({"skript": si, "off": p, "platz": f[1], "b2": f[2], "b3": f[3],
                            "aktiv": f[4], "mesh": f[5], "flags": f[6], "w8": f[7],
                            "pos": [f[8], f[9], f[10]], "rot": [f[11], f[12], f[13]]})
                p += 22
            else:
                p += 2
    return out


def konventionen(auswahl: Optional[Sequence[str]] = None) -> Dict:
    """Misst die Verfasser-Gewohnheiten an den vorhandenen Modellen."""
    import numpy as np
    out: Dict = {"modelle": [], "summe": {}}
    tpages = collections.Counter()
    cluts = collections.Counter()
    texpad = collections.Counter()
    vpad = collections.Counter()
    npad = collections.Counter()
    nlaenge = collections.Counter()
    kreuz = collections.Counter()
    eckennormalen = collections.Counter()
    umin = vmin = 255
    umax = vmax = 0
    doppel_modelle = 0
    quader: List[Dict] = []
    dreiecke_summe = 0
    for name, pfad in alle_archive():
        kurz = name.split("/")[1]
        if auswahl and name not in auswahl and kurz not in auswahl:
            continue
        d = open(pfad, "rb").read()
        do2 = Do2.lesen(d)
        md1 = Md1.lesen(do2.md1)
        z: Dict = {"name": name, "meshes": []}
        saetze = _objektsaetze(do2.skripte, 0x4D if do2.variante == "re2" else 0x4F)
        z["objektsaetze"] = len(saetze)
        wurzel = collections.Counter()
        kind = collections.Counter()
        elternbit = 0x10 if do2.variante == "re2" else 0x08
        for s in saetze:
            (kind if s["flags"] & elternbit else wurzel)[s["mesh"]] += 1
        doppel_im_modell = 0
        for mi, m in enumerate(md1.meshes):
            V = np.array([v[:3] for v in m.vertices], dtype=np.int64)
            N = np.array([v[:3] for v in m.normals], dtype=np.int64)
            for v in m.vertices:
                vpad[v[3]] += 1
            for v in m.normals:
                npad[v[3]] += 1
                l = (v[0] ** 2 + v[1] ** 2 + v[2] ** 2) ** 0.5
                nlaenge[int(round(l))] += 1
            lo = V.min(axis=0).tolist() if len(V) else [0, 0, 0]
            hi = V.max(axis=0).tolist() if len(V) else [0, 0, 0]
            flaechen = collections.Counter()
            gerichtet = set()
            doppel = 0
            for t, x in zip(m.tris, m.tri_tex):
                dreiecke_summe += 1
                p0, p1, p2 = V[t[1]], V[t[3]], V[t[5]]
                g = _kreuz((p2 - p0).tolist(), (p1 - p0).tolist())
                gl = (g[0] ** 2 + g[1] ** 2 + g[2] ** 2) ** 0.5
                ns = (N[t[0]] + N[t[2]] + N[t[4]]).tolist()
                sk = g[0] * ns[0] + g[1] * ns[1] + g[2] * ns[2]
                if gl == 0:
                    kreuz["entartet"] += 1
                elif sk > 0:
                    kreuz["gleich"] += 1
                elif sk < 0:
                    kreuz["gegen"] += 1
                else:
                    kreuz["senkrecht"] += 1
                eckennormalen["eine" if t[0] == t[2] == t[4] else
                              ("gleicher_wert" if tuple(N[t[0]]) == tuple(N[t[2]]) ==
                               tuple(N[t[4]]) else "verschieden")] += 1
                tpages[x[5]] += 1
                cluts[x[2]] += 1
                texpad[x[8]] += 1
                for (uu, vv) in ((x[0], x[1]), (x[3], x[4]), (x[6], x[7])):
                    umin, umax = min(umin, uu), max(umax, uu)
                    vmin, vmax = min(vmin, vv), max(vmax, vv)
                ecken = (tuple(p0.tolist()), tuple(p1.tolist()), tuple(p2.tolist()))
                menge = frozenset(ecken)
                # Drehsinn unabhaengig vom Startpunkt: kleinste Ecke nach vorn
                k = ecken.index(min(ecken))
                gedreht = ecken[k:] + ecken[:k]
                gegen = (gedreht[0], gedreht[2], gedreht[1])
                if gegen in gerichtet:
                    doppel += 1
                gerichtet.add(gedreht)
                flaechen[menge] += 1
            doppel_im_modell += doppel
            masse = [hi[i] - lo[i] for i in range(3)]
            e = {"mesh": mi, "vertices": len(m.vertices), "normalen": len(m.normals),
                 "dreiecke": len(m.tris), "vierecke": len(m.quads),
                 "bbox_min": lo, "bbox_max": hi, "masse": masse,
                 "gegenflaechen": doppel, "als_wurzel": wurzel[mi], "als_kind": kind[mi]}
            z["meshes"].append(e)
            if len(m.vertices) == 8 and len(m.tris) == 12:
                achsen = sorted(range(3), key=lambda i: masse[i])
                quader.append({"name": name, "mesh": mi, "bbox_min": lo, "bbox_max": hi,
                               "dicke": masse[achsen[0]], "breite": masse[achsen[1]],
                               "hoehe": masse[achsen[2]],
                               "achse_dicke": "xyz"[achsen[0]],
                               "achse_breite": "xyz"[achsen[1]],
                               "achse_hoehe": "xyz"[achsen[2]],
                               "als_wurzel": wurzel[mi], "als_kind": kind[mi]})
        z["gegenflaechen"] = doppel_im_modell
        doppel_modelle += int(doppel_im_modell > 0)
        z["saetze"] = saetze[:12]
        out["modelle"].append(z)
    out["summe"] = {
        "modelle": len(out["modelle"]), "dreiecke": dreiecke_summe,
        "tpage_werte": {"%#06x" % k: v for k, v in sorted(tpages.items())},
        "clut_werte": {"%#06x" % k: v for k, v in sorted(cluts.items())},
        "tex_pad_werte": {"%#06x" % k: v for k, v in sorted(texpad.items())},
        "vertex_pad_werte": {str(k): v for k, v in sorted(vpad.items())},
        "normalen_pad_werte": {str(k): v for k, v in sorted(npad.items())},
        "normalen_laenge": {str(k): v for k, v in sorted(nlaenge.items())},
        "normale_gegen_kreuzprodukt": dict(kreuz),
        "normalen_je_dreieck": dict(eckennormalen),
        "u_bereich": [umin, umax], "v_bereich": [vmin, vmax],
        "modelle_mit_gegenflaechen": doppel_modelle,
        "quader_8v_12d": quader,
    }
    return out


def referenzblatt() -> Dict:
    """RE1.5 DOOR00 mesh0 vollstaendig: der Quader, den 37 der 56 Modelle als mesh0
    fuehren. Zeigt Achsen, Angelkante und die Texturbelegung der beiden grossen Flaechen."""
    do2 = Do2.lesen(open(RE15_DOOR, "rb").read())
    m = Md1.lesen(do2.md1).meshes[0]
    flaechen = []
    for t, x in zip(m.tris, m.tri_tex):
        ecken = [m.vertices[t[k]][:3] for k in (1, 3, 5)]
        flaechen.append({"v": [t[1], t[3], t[5]], "n": [t[0], t[2], t[4]],
                         "ecken": [list(e) for e in ecken],
                         "normale_ecke0": list(m.normals[t[0]][:3]),
                         "uv": [[x[0], x[1]], [x[3], x[4]], [x[6], x[7]]],
                         "clut": "%#06x" % x[2], "tpage": "%#06x" % x[5]})
    # Zuordnung Textur <-> Raum auf den beiden Blattflaechen (Normale +-x)
    zuordnung = []
    for f in flaechen:
        if abs(f["normale_ecke0"][0]) == 4096:
            for e, uv in zip(f["ecken"], f["uv"]):
                zuordnung.append({"seite_x": f["normale_ecke0"][0] // 4096, "y": e[1],
                                  "z": e[2], "u": uv[0], "v": uv[1]})
    return {"vertices": [list(v) for v in m.vertices],
            "normalen": [list(v) for v in m.normals], "flaechen": flaechen,
            "blattflaechen_ecken": zuordnung}


def rohrquerschnitte() -> Dict:
    """Wie viele Ecken hat ein Rohr im Original? Gezaehlt werden die Vertices einer
    waagerechten Schnittebene, gruppiert nach Abstand in z (> 150 = neues Rohr)."""
    import numpy as np
    out = {}
    for name in ("DOOR0A", "DOOR16"):
        pf = os.path.join(os.path.dirname(RE2_DOOR_GLOB), name + ".DO2")
        m = Md1.lesen(Do2.lesen(open(pf, "rb").read()).md1).meshes[0]
        V = np.array([v[:3] for v in m.vertices])
        ebenen = collections.Counter(V[:, 1].tolist())
        y0 = ebenen.most_common(1)[0][0]
        P = V[V[:, 1] == y0][:, [0, 2]]
        P = P[np.lexsort((P[:, 0], P[:, 1]))]
        gruppen, lauf = [], [P[0]]
        for q in P[1:]:
            if abs(int(q[1]) - int(lauf[-1][1])) > 150:
                gruppen.append(lauf)
                lauf = [q]
            else:
                lauf.append(q)
        gruppen.append(lauf)
        g0 = np.array(gruppen[0])
        out[name] = {"mesh": 0, "vertices": len(V), "dreiecke": len(m.tris),
                     "bbox_min": V.min(0).tolist(), "bbox_max": V.max(0).tolist(),
                     "schnitt_y": int(y0), "punkte_im_schnitt": len(P),
                     "rohre": len(gruppen),
                     "ecken_je_rohr": dict(collections.Counter(len(g) for g in gruppen)),
                     "erstes_rohr": g0.tolist(),
                     "durchmesser_x": int(np.ptp(g0[:, 0])),
                     "durchmesser_z": int(np.ptp(g0[:, 1]))}
    return out


def scd_statistik() -> Dict:
    """Skriptzahl, Blocklaenge mod 4 und Blockende ueber alle 56 Archive."""
    enden = collections.Counter()
    mod4 = collections.Counter()
    zahl = collections.Counter()
    ungerade = 0
    for name, pfad in alle_archive():
        do2 = Do2.lesen(open(pfad, "rb").read())
        block = scd_schreiben(do2.skripte)
        enden[block[-4:].hex()] += 1
        mod4[len(block) % 4] += 1
        zahl[len(do2.skripte)] += 1
        ungerade += sum(1 for k in do2.skripte if len(k) % 2)
    return {"blockende_letzte_4_bytes": dict(enden), "blocklaenge_mod_4": dict(mod4),
            "skripte_je_archiv": {str(k): v for k, v in sorted(zahl.items())},
            "skripte_ungerader_laenge": ungerade,
            "re15_door00_block": scd_schreiben(
                Do2.lesen(open(RE15_DOOR, "rb").read()).skripte).hex()}


def tim_statistik() -> Dict:
    """Farbtiefe, Masse, CLUT[0], STP-Nutzung ueber alle 56 Texturen."""
    z = collections.Counter()
    zeilen = []
    for name, pfad in alle_archive():
        do2 = Do2.lesen(open(pfad, "rb").read())
        tim, _ = Tim.lesen(do2.tim)
        nutz = collections.Counter(tim.pix)
        genutzt = [i for i in range(len(tim.clut)) if nutz[i]]
        null_genutzt = [i for i in genutzt if tim.clut[i] == 0]
        stp_genutzt = [i for i in genutzt if tim.clut[i] & 0x8000]
        stp_schwarz = [i for i in stp_genutzt if tim.clut[i] == 0x8000]
        z["dateien"] += 1
        z["flags_%d" % tim.flags] += 1
        z["masse_%dx%d" % (tim.breite_px, tim.hoehe_px)] += 1
        z["clut_%dx%d_bei_%d_%d" % (tim.clut_rect[2], tim.clut_rect[3], tim.clut_rect[0],
                                    tim.clut_rect[1])] += 1
        z["bild_bei_%d_%d" % (tim.pix_rect[0], tim.pix_rect[1])] += 1
        z["laenge_%#x" % len(do2.tim)] += 1
        z["clut0_ist_null"] += int(tim.clut[0] == 0)
        z["zeichnet_wert_0000"] += int(bool(null_genutzt))
        z["zeichnet_stp"] += int(bool(stp_genutzt))
        z["zeichnet_8000"] += int(bool(stp_schwarz))
        zeilen.append({"name": name, "genutzte_indizes": len(genutzt),
                       "punkte_wert_0000": sum(nutz[i] for i in null_genutzt),
                       "punkte_stp": sum(nutz[i] for i in stp_genutzt),
                       "punkte_8000": sum(nutz[i] for i in stp_schwarz)})
    return {"summe": dict(z), "zeilen": zeilen}


def probemodell(ziel: str = AUSGABE) -> Dict:
    """Ein texturiertes Rechteck aus zwei Dreiecken als vollstaendiges DO2, in beiden
    Varianten. Geschrieben wird nach build/tor_1170/, nichts ins Repo."""
    from PIL import Image
    os.makedirs(ziel, exist_ok=True)
    # Pruefbild 128x256: Warnschraffur gelb/schwarz, grauer Balken, durchsichtige Ecke
    img = Image.new("RGBA", (128, 256), (0, 0, 0, 255))
    px = img.load()
    for y in range(256):
        for x in range(128):
            if (x + y) // 16 % 2 == 0:
                px[x, y] = (232, 200, 24, 255)       # gelb
            else:
                px[x, y] = (0, 0, 0, 255)            # reines Schwarz -> 0x8000
            if 96 <= y < 160 and 16 <= x < 112:
                px[x, y] = (136, 136, 144, 255)      # grau
            if x < 8 and y < 8:
                px[x, y] = (0, 0, 0, 0)              # durchsichtig -> Index 0
    img.save(os.path.join(ziel, "probe_textur.png"))
    tim, tinfo = tim_aus_bild(img, schwarz="stp")
    # Rechteck in der Blattebene des Originals: x = Dicke, y = Hoehe (oben negativ),
    # z = Breite ab der Angel bei z = 0. Masse = Quader aus RE1.5 DOOR00 mesh0.
    a = ((0, 0, 0), (0, 255))
    b = ((0, -6600, 0), (0, 0))
    c = ((0, -6600, -3600), (127, 0))
    e = ((0, 0, -3600), (127, 255))
    mesh = mesh_aus_dreiecken([(a, b, c), (a, c, e)], doppelseitig=True)
    md1 = md1_bauen([mesh])
    quelle15 = Do2.lesen(open(RE15_DOOR, "rb").read())
    quelle2 = Do2.lesen(open(sorted(glob.glob(RE2_DOOR_GLOB))[0], "rb").read())
    out: Dict = {"tim": tinfo, "dateien": {}}
    for variante, quelle in (("re15", quelle15), ("re2", quelle2)):
        do2 = do2_bauen(md1, tim, [evt_end_skript()], ton=quelle, variante=variante)
        roh = do2.schreiben()
        pfad = os.path.join(ziel, "PROBE_%s.DO2" % variante.upper())
        open(pfad, "wb").write(roh)
        zurueck = Do2.lesen(roh)
        md1_z = Md1.lesen(zurueck.md1)
        tim_z, _ = Tim.lesen(zurueck.tim)
        eintrag = {"pfad": os.path.relpath(pfad, REPO).replace("\\", "/"),
                   "laenge": len(roh), "sha1": hashlib.sha1(roh).hexdigest(),
                   "zurueckgelesen_gleich": (zurueck.schreiben() == roh
                                             and md1_z.schreiben() == md1.schreiben()
                                             and tim_z.schreiben() == tim.schreiben()),
                   "lader": lader_pruefung(roh),
                   "meshes": [{"vertices": len(m.vertices), "normalen": len(m.normals),
                               "dreiecke": len(m.tris)} for m in md1_z.meshes]}
        if variante == "re2":
            eintrag["re2_tabelle"] = zurueck.re2_tabelleneintrag()
        else:
            eintrag["re15_dateitabelle"] = {"groesse": len(roh),
                                            "xor512": pruefsumme_512(roh)}
        out["dateien"][variante] = eintrag
    open(os.path.join(ziel, "PROBE.md1"), "wb").write(md1.schreiben())
    open(os.path.join(ziel, "PROBE.tim"), "wb").write(tim.schreiben())
    out["md1"] = {"pfad": "build/tor_1170/PROBE.md1", "laenge": len(md1.schreiben()),
                  "vertices": [list(v) for v in mesh.vertices],
                  "normalen": [list(v) for v in mesh.normals],
                  "dreiecke": [list(t) for t in mesh.tris],
                  "tex": [list(t) for t in mesh.tri_tex]}
    out["tim_datei"] = {"pfad": "build/tor_1170/PROBE.tim", "laenge": len(tim.schreiben()),
                        "clut_kopf": ["%#06x" % v for v in tim.clut[:6]]}
    return out


# ======================================================================================
# Gegenprobe mit UNABHAENGIGEN Lesern
# ======================================================================================
# Leser 1: die MD1-/TIM-Parser der Engine (re15_port/engine/src/md1_common.c,
#          tim_common.c), als eigenstaendiges Pruefprogramm uebersetzt. Der Port selbst
#          wird dafuer NICHT gebaut; es entstehen nur Dateien unter build/tor_1170/pruefer/.
# Leser 2: analysis/befunde_2026-09-21/tools/md1lib.py und timlib.py (Python).
# Einen unabhaengigen DO2-Leser, der beide Varianten kann, hat das Repo nicht:
#          DO2Extractor.java kennt nur den RE1.5-Kopf und laeuft ohne Gradle nicht.
PRUEFER_C = r"""/*
 * pruefer_engine.c -- UNABHAENGIGER Gegenleser fuer do2_format.py.
 *
 * Nutzt ausschliesslich die MD1- und TIM-Parser der Engine
 * (re15_port/engine/src/md1_common.c, tim_common.c) und gibt Kennzahlen als JSON aus.
 * Die Engine hat keinen DO2-Leser; MD1/TIM-Lage kommt deshalb per Kommandozeile.
 *
 * Aufruf: pruefer_engine <datei> <md1_off> <md1_len> <tim_off> <tim_len>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_md1.h"
#include "re15_tim.h"

int main(int argc, char **argv)
{
    if (argc != 6) { fprintf(stderr, "aufruf\n"); return 2; }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { fprintf(stderr, "datei\n"); return 2; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *d = (uint8_t *)malloc((size_t)n);
    if (fread(d, 1, (size_t)n, f) != (size_t)n) return 2;
    fclose(f);
    long mo = strtol(argv[2], 0, 0), ml = strtol(argv[3], 0, 0);
    long to = strtol(argv[4], 0, 0), tl = strtol(argv[5], 0, 0);
    if (mo + ml > n || to + tl > n) { fprintf(stderr, "bereich\n"); return 2; }

    static re15_md1_t md1;
    int rm = re15_md1_parse(d + mo, (int)ml, &md1);
    printf("{\"md1_rc\":%d,\"length\":%u,\"unknown\":%u,\"object_count\":%u,\"mesh_count\":%d,\"meshes\":[",
           rm, md1.length, md1.unknown, md1.object_count, md1.mesh_count);
    for (int i = 0; rm == 0 && i < md1.mesh_count; i++) {
        const re15_md1_mesh_t *m = &md1.meshes[i];
        long long sv = 0, sn = 0, si = 0, su = 0, st = 0;
        for (int k = 0; k < m->tri_vertex_count; k++)
            sv += (long long)m->tri_vertices[k].x * 3 + (long long)m->tri_vertices[k].y * 5
                + (long long)m->tri_vertices[k].z * 7 + m->tri_vertices[k].pad;
        for (int k = 0; k < m->tri_normal_count; k++)
            sn += (long long)m->tri_normals[k].x * 3 + (long long)m->tri_normals[k].y * 5
                + (long long)m->tri_normals[k].z * 7 + m->tri_normals[k].pad;
        for (int k = 0; k < m->triangle_count; k++) {
            const re15_md1_triangle_t *t = &m->triangles[k];
            si += t->n0 + 2 * t->v0 + 3 * t->n1 + 4 * t->v1 + 5 * t->n2 + 6 * t->v2;
            const re15_md1_tri_uv_t *u = &m->triangle_uvs[k];
            su += u->u0 + 2 * u->v0 + 3 * u->u1 + 4 * u->v1 + 5 * u->u2 + 6 * u->v2;
            st += (long long)u->clut * 3 + (long long)u->page * 5 + u->_pad_a;
        }
        printf("%s{\"v\":%d,\"n\":%d,\"t\":%d,\"q\":%d,\"sv\":%lld,\"sn\":%lld,\"si\":%lld,\"su\":%lld,\"st\":%lld}",
               i ? "," : "", m->tri_vertex_count, m->tri_normal_count, m->triangle_count,
               m->quad_count, sv, sn, si, su, st);
    }
    static re15_tim_t tim;
    int rt = re15_tim_parse(d + to, (int)tl, &tim);
    unsigned long long sc = 0, sp = 0;
    if (rt == 0) {
        const uint8_t *c = (const uint8_t *)tim.clut;
        for (int k = 0; k < tim.clut_entries; k++)
            sc += (unsigned long long)(c[k * 2] | (c[k * 2 + 1] << 8)) * (unsigned)(k + 1);
        const uint8_t *p = (const uint8_t *)tim.pixels;
        for (int k = 0; k < tim.width * tim.height; k++)
            sp += (unsigned long long)p[k] * (unsigned)((k % 251) + 1);
    }
    printf("],\"tim_rc\":%d,\"bpp\":%d,\"has_clut\":%d,\"clut_x\":%d,\"clut_y\":%d,\"clut_entries\":%d,"
           "\"data_x\":%d,\"data_y\":%d,\"width\":%d,\"height\":%d,\"sc\":%llu,\"sp\":%llu}\n",
           rt, tim.bpp, tim.has_clut, tim.clut_x, tim.clut_y, tim.clut_entries,
           tim.data_x, tim.data_y, tim.width, tim.height, sc, sp);
    free(d);
    return 0;
}
"""


def _kennzahlen(md1: "Md1", tim: "Tim") -> Dict:
    """Dieselben Summen wie pruefer_engine.c, aus dem eigenen Leser."""
    meshes = []
    for m in md1.meshes:
        sv = sum(v[0] * 3 + v[1] * 5 + v[2] * 7 + v[3] for v in m.vertices)
        sn = sum(v[0] * 3 + v[1] * 5 + v[2] * 7 + v[3] for v in m.normals)
        si = sum(t[0] + 2 * t[1] + 3 * t[2] + 4 * t[3] + 5 * t[4] + 6 * t[5] for t in m.tris)
        su = sum(x[0] + 2 * x[1] + 3 * x[3] + 4 * x[4] + 5 * x[6] + 6 * x[7]
                 for x in m.tri_tex)
        st = sum(x[2] * 3 + x[5] * 5 + x[8] for x in m.tri_tex)
        meshes.append({"v": len(m.vertices), "n": len(m.normals), "t": len(m.tris),
                       "q": len(m.quads), "sv": sv, "sn": sn, "si": si, "su": su, "st": st})
    sc = sum(v * (k + 1) for k, v in enumerate(tim.clut))
    sp = sum(b * ((k % 251) + 1) for k, b in enumerate(tim.pix))
    return {"md1_rc": 0, "length": u32(md1.schreiben(), 0), "unknown": md1.merker,
            "object_count": 2 * len(md1.meshes), "mesh_count": len(md1.meshes),
            "meshes": meshes, "tim_rc": 0, "bpp": tim.farbtiefe,
            "has_clut": int(bool(tim.flags & 8)), "clut_x": tim.clut_rect[0],
            "clut_y": tim.clut_rect[1], "clut_entries": len(tim.clut),
            "data_x": tim.pix_rect[0], "data_y": tim.pix_rect[1],
            "width": tim.breite_px, "height": tim.hoehe_px, "sc": sc, "sp": sp}


def _pruefer_bauen() -> Optional[str]:
    import subprocess
    ordner = os.path.join(AUSGABE, "pruefer")
    os.makedirs(ordner, exist_ok=True)
    quelle = os.path.join(ordner, "pruefer_engine.c")
    with open(quelle, "w", encoding="utf-8", newline="\n") as f:
        f.write(PRUEFER_C)
    exe = os.path.join(ordner, "pruefer_engine.exe")
    env = dict(os.environ)
    # msys64 zuerst: Git-Bash schiebt sonst sein eigenes mingw64 davor, und cc1 stirbt
    # still (CLAUDE.md, Abschnitt Build).
    env["PATH"] = "C:\\msys64\\mingw64\\bin;C:\\msys64\\usr\\bin;" + env.get("PATH", "")
    befehl = ["gcc", "-std=c11", "-O1", "-I", os.path.join(REPO, "re15_port", "include"),
              "-o", exe, quelle,
              os.path.join(REPO, "re15_port", "engine", "src", "md1_common.c"),
              os.path.join(REPO, "re15_port", "engine", "src", "tim_common.c")]
    try:
        r = subprocess.run(befehl, env=env, capture_output=True, text=True)
    except OSError:
        return None
    if r.returncode != 0 or not os.path.isfile(exe):
        return None
    return exe


def gegenprobe() -> Dict:
    """Alle 56 Archive und beide Probemodelle gegen zwei fremde Leser."""
    import importlib.util
    import subprocess
    out: Dict = {"engine": {"gebaut": False, "geprueft": 0, "gleich": 0, "abweichung": []},
                 "python": {"geladen": False, "geprueft": 0, "gleich": 0, "abweichung": []}}
    exe = _pruefer_bauen()
    out["engine"]["gebaut"] = exe is not None
    werkzeug = os.path.join(REPO, "analysis", "befunde_2026-09-21", "tools")
    fremd = {}
    for name in ("md1lib", "timlib"):
        pf = os.path.join(werkzeug, name + ".py")
        if os.path.isfile(pf):
            spec = importlib.util.spec_from_file_location("fremd_" + name, pf)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
            fremd[name] = mod
    out["python"]["geladen"] = len(fremd) == 2
    ziele = alle_archive()
    for v in ("RE15", "RE2"):
        pf = os.path.join(AUSGABE, "PROBE_%s.DO2" % v)
        if os.path.isfile(pf):
            ziele.append(("PROBE/" + v, pf))
    for name, pfad in ziele:
        d = open(pfad, "rb").read()
        do2 = Do2.lesen(d)
        md1 = Md1.lesen(do2.md1)
        tim, _ = Tim.lesen(do2.tim)
        lage = do2.lage()
        if exe:
            r = subprocess.run([exe, pfad, str(lage["md1"]), str(len(do2.md1)),
                                str(lage["tim"]), str(len(do2.tim))],
                               capture_output=True, text=True)
            out["engine"]["geprueft"] += 1
            try:
                fremdwerte = json.loads(r.stdout)
            except ValueError:
                fremdwerte = {"fehler": r.stdout[:200] + r.stderr[:200]}
            if fremdwerte == _kennzahlen(md1, tim):
                out["engine"]["gleich"] += 1
            else:
                out["engine"]["abweichung"].append(name)
        if len(fremd) == 2:
            out["python"]["geprueft"] += 1
            a = fremd["md1lib"].parse(do2.md1)
            t = fremd["timlib"].decode(do2.tim)
            gleich = a is not None and t is not None
            if gleich:
                gleich = (a["nmesh"] == len(md1.meshes)
                          and all(list(am["tv"]) == [v[:3] for v in m.vertices]
                                  and list(am["tris"]) == [(x[1], x[3], x[5]) for x in m.tris]
                                  and len(am["quads"]) == len(m.quads)
                                  for am, m in zip(a["meshes"], md1.meshes))
                          and t["bpp"] == tim.farbtiefe and t["w"] == tim.breite_px
                          and t["h"] == tim.hoehe_px and bytes(t["px"]) == tim.pix
                          and t["cluts"][0] == [fremd["timlib"].c16(v) for v in tim.clut]
                          and (t["cx"], t["cy"]) == tuple(tim.clut_rect[:2])
                          and t["end"] == len(do2.tim))
            if gleich:
                out["python"]["gleich"] += 1
            else:
                out["python"]["abweichung"].append(name)
    return out


def _json_schreiben(name: str, daten: Dict) -> str:
    os.makedirs(AUSGABE, exist_ok=True)
    pfad = os.path.join(AUSGABE, name)
    with open(pfad, "w", encoding="utf-8") as f:
        json.dump(daten, f, indent=1, ensure_ascii=False, default=list)
    return pfad


def main(argv: Optional[Sequence[str]] = None) -> int:
    ap = argparse.ArgumentParser(description="DO2/MD1/TIM: Leser, Schreiber, Verfasser")
    ap.add_argument("befehl", choices=["abnahme", "konventionen", "tim", "tabellen",
                                       "probe", "gegenprobe", "alles", "zeige"])
    ap.add_argument("datei", nargs="?")
    a = ap.parse_args(argv)
    if a.befehl == "zeige":
        d = open(a.datei, "rb").read()
        do2 = Do2.lesen(d)
        md1 = Md1.lesen(do2.md1)
        tim, _ = Tim.lesen(do2.tim)
        print("variante", do2.variante, "laenge %#x" % len(d))
        print("md1 %#x B, %d Meshes, %d Dreiecke, %d Vierecke" %
              (len(do2.md1), len(md1.meshes), md1.dreiecke_gesamt(), md1.vierecke_gesamt()))
        print("skripte", [len(s) for s in do2.skripte])
        print("tim flags=%d %dx%d clut=%r bild=%r" % (tim.flags, tim.breite_px, tim.hoehe_px,
                                                      tim.clut_rect, tim.pix_rect))
        print("ton vorspann=%s vh=%#x nachspann=%s vb=%#x" %
              (do2.ton_vorspann.hex(), len(do2.vh), do2.ton_nachspann.hex(), len(do2.vb)))
        print("lader", lader_pruefung(d))
        return 0
    daten: Dict = {}
    if a.befehl in ("abnahme", "alles"):
        daten["abnahme"] = abnahme()
        s = daten["abnahme"]
        print("ABNAHME: %d Archive, byte-identisch %d, MD1 lueckenlos %d, MD1 Regelaufbau %d, "
              "TIM identisch %d, Lader lesbar %d" %
              (s["anzahl"], s["byte_identisch"], s["md1_lueckenlos"], s["md1_regelaufbau"],
               s["tim_identisch"], s["lader_lesbar"]))
    if a.befehl in ("tabellen", "alles"):
        daten["re2_tabelle"] = re2_tabelle_pruefen()
        daten["re15_dateitabelle"] = re15_dateitabelle()
        print("RE2-TABELLE: %d von %d Eintraegen aus der Datei nachgerechnet" %
              (daten["re2_tabelle"]["gleich"], daten["re2_tabelle"]["eintraege"]))
        print("RE15-DATEITABELLE:", daten["re15_dateitabelle"])
    if a.befehl in ("tim", "alles"):
        daten["tim_statistik"] = tim_statistik()
        print("TIM:", daten["tim_statistik"]["summe"])
    if a.befehl in ("konventionen", "alles"):
        daten["konventionen"] = konventionen()
        daten["referenzblatt"] = referenzblatt()
        daten["rohrquerschnitte"] = rohrquerschnitte()
        daten["scd_statistik"] = scd_statistik()
        print("ROHRE:", {k: (v["ecken_je_rohr"], v["durchmesser_x"], v["durchmesser_z"])
                         for k, v in daten["rohrquerschnitte"].items()})
        print("SCD:", daten["scd_statistik"])
        s = daten["konventionen"]["summe"]
        print("KONVENTIONEN:", {k: v for k, v in s.items() if k != "quader_8v_12d"})
    if a.befehl in ("probe", "alles"):
        daten["probemodell"] = probemodell()
        for v, e in daten["probemodell"]["dateien"].items():
            print("PROBE %s: %s %d B, zurueckgelesen gleich %s, Lader lesbar %s" %
                  (v, e["pfad"], e["laenge"], e["zurueckgelesen_gleich"], e["lader"]["lesbar"]))
    if a.befehl in ("gegenprobe", "alles"):
        daten["gegenprobe"] = gegenprobe()
        g = daten["gegenprobe"]
        print("GEGENPROBE Engine-Parser: gebaut %s, %d von %d gleich %s" %
              (g["engine"]["gebaut"], g["engine"]["gleich"], g["engine"]["geprueft"],
               g["engine"]["abweichung"]))
        print("GEGENPROBE md1lib/timlib: geladen %s, %d von %d gleich %s" %
              (g["python"]["geladen"], g["python"]["gleich"], g["python"]["geprueft"],
               g["python"]["abweichung"]))
    if a.befehl == "alles":
        print("geschrieben:", _json_schreiben("tuerformat.json", daten))
    else:
        print("geschrieben:", _json_schreiben("tuerformat_%s.json" % a.befehl, daten))
    return 0


if __name__ == "__main__":
    sys.exit(main())
