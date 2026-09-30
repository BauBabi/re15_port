#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): Schalter -> Zelle -> Sub -> Prop -> Bit -> Loesung, direkt aus den Bytes von
ROOM11F0.RDT (und ROOM11F1.RDT) — reproduziert Dossier-Tabelle 3.4 und leitet die Lampenmasken ab.

Gelesen wird (Datei-Offsets, selbst gedumpt):
  sub00 Aot_set      @0x00D78 + 20*k   `2c ss 05 44 00 00 <x s16> <z s16> <w> <d> ...`  (Zellen, k = 0..10)
  sub00 Obj_model_set @0x00E76 + 34*k  `2d oo 00 00 02 00 0a 00 00 00 <x s16> <y s16> <z s16> ...` (Hebel)
  sub01 Bloecke      @0x010EC + 40*k   `06 00 24 00 21 05 nn 01 2e 03 00 00 06 00 16 00 3e 00 0f 00 kk 00 06 00 0a 00
                                         51 01 40 00 04 ff 18 ss 08 00 08 00 08 00` (Zelle kk -> Sub ss)
  Schalter-Subs      Set(5,bit,1) nach der For-Schleife (sub06 @0x01340 `22 05 0d 01` ...)
  Loesungskette      @0x012BE + 4*i    `21 05 <bit> <soll>`
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
raum = sys.argv[1] if len(sys.argv) > 1 else "ROOM11F0"
d = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE1", raum + ".RDT"), "rb").read()
fehler = 0
def need(ok, text):
    global fehler
    if not ok:
        fehler += 1; print("FEHLER", text)

zellen = {}
for k in range(11):
    a = 0x00D78 + 20 * k
    need(d[a] == 0x2C and d[a + 2] == 0x05, "Aot_set @0x%05X" % a)
    slot = d[a + 1]; x, z = struct.unpack_from("<hh", d, a + 6)
    zellen[slot] = (x, z, a)
props = {}
for k in range(10):
    a = 0x00E76 + 34 * k
    need(d[a] == 0x2D, "Obj_model_set @0x%05X" % a)
    obj = d[a + 1]; x, y, z = struct.unpack_from("<hhh", d, a + 10)
    props[obj] = (x, z, a)
subs_off = struct.unpack_from("<%dH" % 20, d, 0xD34)   # sub-Tabelle (main 0xCC4, sub 0xD34)
bloecke = []
for k in range(11):
    a = 0x010EC + 40 * k
    need(d[a] == 0x06 and d[a + 4] == 0x21 and d[a + 5] == 0x05, "sub01-Block @0x%05X" % a)
    zbit = d[a + 6]
    kk = d[a + 20]            # Member_cmp-Wert (@+0x10: 3e 00 0f 00 kk 00)
    ss = d[a + 33]            # Evt_exec-Sub (@+0x1E: 04 ff 18 ss)
    need(d[a + 16] == 0x3E and d[a + 30] == 0x04, "Member_cmp/Evt_exec @0x%05X" % a)
    bloecke.append((zbit, kk, ss, a))
kette = []
for i in range(10):
    a = 0x012BE + 4 * i
    need(d[a] == 0x21 and d[a + 1] == 0x05, "Ck @0x%05X" % a)
    kette.append((d[a + 2], d[a + 3], a))
print("Raum %s" % raum)
print("Schalter | Zellbit | Zelle(slot) x/z          | Member_cmp @ | Sub   | Prop obj x/z            | Zustandsbit | Loesung | Spalte/Zeile")
maske_l = 0
for n, (zbit, kk, ss, a) in enumerate(bloecke[:10], start=1):
    base = struct.unpack_from("<H", d, 0xD34 + 2 * ss)[0] + 0xD34
    # Zustandsbit: erstes `22 05 bb 01` nach `0e 00` (Next) im Schalter-Sub
    q = base; zbit_s = None
    while q < base + 0x40:
        if d[q] == 0x0E and d[q + 1] == 0x00 and d[q + 2] == 0x22 and d[q + 3] == 0x05 and d[q + 5] == 0x01:
            zbit_s = d[q + 4]; set_at = q + 2; break
        q += 1
    obj = None
    q = base
    while q < base + 0x10:
        if d[q] == 0x2E and d[q + 1] == 0x03: obj = d[q + 2]; break
        q += 1
    px, pz, pa = props[obj]
    cx, cz, ca = zellen[kk]
    soll = [s for (b, s, ka) in kette if b == zbit_s][0]
    if soll: maske_l |= 1 << (n - 1)
    spalte = "links" if px < -22000 else "rechts"
    zeile = [26000, 24000, 22000, 19800, 17800].index(pz) + 1
    print("%8d | %7d | %2d (%6d,%6d) @0x%05X | 0x%05X      | sub%02d | 0x%02X (%6d,%6d) @0x%05X | %2d @0x%05X | %s | %s/%d"
          % (n, zbit, kk, cx, cz, ca, a + 16, ss, obj, px, pz, pa, zbit_s, set_at, "EIN" if soll else "aus", spalte, zeile))
links = maske_l & 0x01F; rechts = maske_l & 0x3E0
print("Loesungsmaske = 0x%03X (Bit 0 = Schalter 1)" % maske_l)
print("Lampe oben  (linke Spalte, Schalter 1..5):  (m & 0x01F) == 0x%03X  -> %d EIN-Schalter" % (links, bin(links).count("1")))
print("Lampe unten (rechte Spalte, Schalter 6..10): (m & 0x3E0) == 0x%03X  -> %d EIN-Schalter" % (rechts, bin(rechts).count("1")))
need(maske_l == 0x155 and links == 0x015 and rechts == 0x140, "Masken")
print("Ergebnis:", "OK" if fehler == 0 else "%d FEHLER" % fehler)
sys.exit(1 if fehler else 0)
