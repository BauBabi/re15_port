#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_r702_esp.py - Runde 42 Strang finale (Plan S1, D10): Raum-ESP von RE2 ROOM7020 fuer 5090/5091.

Quelle: info/re2leon/PL0/RDT/ROOM7020.RDT. Kopf: 8 Byte Zaehler, dann 23 Offsets; [18] ESP 0x2788, [19] 0x2E30 (+4 = Ende 0x2E34),
[20] TIM-Block 0x1F55C, [21] Objekt-TIM 0x24F9C. Wie bis v0.8.30 R704.ESP (G5-Szene, Werkzeug entfernt) und GATOR40A0.ESP:

  R702.ESP       = RDT[0x2788 : 0x2E34]   Ids `16 27 15 1a ff ff ff ff` (@0x02788). Bank 0x27 = Wiederholung 3 der Todessequenz
                   (`lui a0,0x270a / ori 0x1000 / jal 0x8001c8c4` @0x80103a54-60, CDEMD0 EM2B-Overlay).
  R702_%02X.TIM  = die vier 4-bpp-TIMs ab RDT[20] in Id-Reihenfolge: Tafel rueckwaerts vor RDT[21] (RDT[0x24F98] = 0, [0x24F94] =
                   0x10A0, [0x24F90] = 0x2120, [0x24F8C] = 0x31A0, [0x24F88] = 0xFFFFFFFF Ende), Eintrag i = TIM der Id i relativ zu
                   RDT[20]; Laenge je TIM aus dem Kopf (8 + CLUT-Block + Bild-Block).
                   0x16 = RDT[0x1F55C:0x205FC] == GATOR40A0_16.TIM, 0x1A = RDT[0x226FC:0x24F7C] == GATOR40A0_1A.TIM (hier geprueft).

  R702.EDT/.VH/.VB = Raumtonbank (snd0) = RDT-Kopfworte +0x08/+0x0C/+0x10 (FUN_80059e54: DAT_800dbb80 = [RDT+8], Muster
                   tools/r35_fenster/glas1090_extrakt.py): EDT @0x05C90 (0xC0 B = 48 Saetze), VH @0x05D50 (3616 B), VB @0x06B70; VB-Laenge =
                   VAB-Gesamtgroesse VH+0x0C (104448) - VH-Laenge. Gegenprobe gegen den RDT-Extrakt info/re2leon/PL0/RDT/room7020/snd0.*.
                   Satz 0x0F (@0x05CCC `00 00 c3 20`) = Einschlag (Se_on 0x020F0001 @0x80103b54-5c, CDEMD0 EM2B), Satz 0x0E (@0x05CC8) =
                   Landung des Werfers (ROOM7020 sub13, Stufe 3). Runde 3 (Pruefbericht s1 r2 R2).

Aufruf: python re15_port/tools/r42_finale/re2_r702_esp.py [--install]   (ohne --install nach build/r42_finale_esp)
Ausgabe: Name, Laenge, md5.
"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM7020.RDT")
RE2 = os.path.join(REPO, "re15_port", "shared_assets", "RE2")
ESP_VON, ESP_BIS = 0x2788, 0x2E34
IDS = (0x16, 0x27, 0x15, 0x1A)


def die(m):
    sys.stderr.write("re2_r702_esp: FEHLER: %s\n" % m)
    sys.exit(1)


def lesen(a):
    offs = struct.unpack_from("<23I", a, 8)
    if (offs[18], offs[19] + 4, offs[20], offs[21]) != (ESP_VON, ESP_BIS, 0x1F55C, 0x24F9C):
        die("Offsets %x %x %x %x" % (offs[18], offs[19], offs[20], offs[21]))
    e = a[ESP_VON:ESP_BIS]
    if e[:8] != bytes(list(IDS) + [0xFF] * 4):
        die("ESP-Ids %s" % e[:8].hex())
    tafel = []
    k = 1
    while True:                                           # rueckwaerts vor RDT[21] bis 0xFFFFFFFF; Eintrag i = TIM der Id i
        v = struct.unpack_from("<I", a, offs[21] - 4 * k)[0]
        if v == 0xFFFFFFFF:
            break
        tafel.append(v)
        k += 1
    if tafel != [0, 0x10A0, 0x2120, 0x31A0]:
        die("TIM-Tafel %s" % [hex(t) for t in tafel])
    tims = {}
    for i, rel in enumerate(tafel):
        s = offs[20] + rel
        magic, fl = struct.unpack_from("<II", a, s)
        cl = struct.unpack_from("<I", a, s + 8)[0]
        im = struct.unpack_from("<I", a, s + 8 + cl)[0]
        if (magic, fl) != (0x10, 8):
            die("TIM %d: %x %x" % (i, magic, fl))
        tims[IDS[i]] = a[s:s + 8 + cl + im]
    for b in (0x16, 0x1A):
        g = open(os.path.join(RE2, "GATOR40A0_%02X.TIM" % b), "rb").read()
        if g != tims[b]:
            die("R702_%02X.TIM != GATOR40A0_%02X.TIM" % (b, b))
    return e, tims


def tonbank(a):
    """R702.EDT/.VH/.VB: RDT-Kopfworte +0x08/+0x0C/+0x10, VB-Laenge aus dem VAB-Kopf (+0x0C Gesamtgroesse)."""
    edt, vh, vb = struct.unpack_from("<III", a, 8)
    if (edt, vh, vb) != (0x5C90, 0x5D50, 0x6B70):
        die("Tonbank-Kopf %x %x %x" % (edt, vh, vb))
    if a[vh:vh + 4] != b"pBAV":
        die("VH ohne pBAV")
    gesamt = struct.unpack_from("<I", a, vh + 12)[0]
    teile = {"R702.EDT": a[edt:vh], "R702.VH": a[vh:vb], "R702.VB": a[vb:vb + gesamt - (vb - vh)]}
    if len(teile["R702.VB"]) != gesamt - (vb - vh):
        die("VB kuerzer als der VAB-Kopf sagt")
    ex = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room7020")
    for name, quelle in (("R702.EDT", "snd0.edt"), ("R702.VH", "snd0.vh"), ("R702.VB", "snd0.vb")):
        q = os.path.join(ex, quelle)
        if os.path.exists(q) and open(q, "rb").read() != teile[name]:
            die("%s != room7020/%s" % (name, quelle))
    if teile["R702.EDT"][0x0F * 4:0x10 * 4] != bytes.fromhex("0000c320"):
        die("Satz 0x0F %s" % teile["R702.EDT"][0x3C:0x40].hex())
    return teile


def main():
    install = "--install" in sys.argv
    a = open(RDT, "rb").read()
    e, tims = lesen(a)
    out = RE2 if install else os.path.join(REPO, "build", "r42_finale_esp")
    os.makedirs(out, exist_ok=True)
    dateien = [("R702.ESP", e)] + [("R702_%02X.TIM" % b, tims[b]) for b in IDS] + sorted(tonbank(a).items())
    for name, data in dateien:
        open(os.path.join(out, name), "wb").write(data)
        print("  %s %d B md5 %s" % (os.path.join(out, name), len(data), hashlib.md5(data).hexdigest()))


if __name__ == "__main__":
    main()
