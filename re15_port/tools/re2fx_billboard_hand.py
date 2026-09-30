#!/usr/bin/env python
"""re2fx_billboard_hand.py - Runde 34 Spur D, Nachbesserung N4 (Gegenpruefung M3): HANDRECHNUNG eines
RE2-FX-Billboard-Quads, unabhaengig vom Port-Code (keine Zeile aus re2_fx.c / probe_r34_re2fx_bild.c).

Eingaben = die von probe_r34_re2fx_bild ausgegebene Kamera ("Kamera rot [...] trans (...) H ...") und je Pin
Weltlage, Skala (+0x3A), Aspekt (+0x04/+0x06), Anim-Eintrag (Zelle, Anzahl, Dauer, Groesse) und UV-Eintrag
(u, v, cx, cy). Rechnung:
  RTPS (psx-spx "GTE Coordinate Calculation Commands", sf = 1, lm = 0):
      MAC = (TR*0x1000 + R*V) >> 12,  IR1/IR2 = MAC auf -0x8000..0x7FFF,  SZ3 = MAC3 auf 0..0xFFFF
      n = UNR-Division(H, SZ3) (psx-spx "GTE Division Inaccuracy"), SX = (OFX + IR1*n) >> 16 mit OFX = cx << 16
  FUN_80077ed0 (RE2 PSX.EXE, selbst disassembliert):
      t1   = Groesse * Skala * camf                     mult/mflo @0x80077f14-48
      SZ   = min(SZ3, 32767)                            @0x80077f64-74
      step = t1 / (SZ << 4)                             sll 4 / div @0x80077fd4-d8 (Abschneiden)
      w    = step * AspektX, h = step * AspektY         @0x8007800c / @0x80078038
      tx   = w // Groesse, ty = h // Groesse            divu @0x8007801c / @0x8007805c
      du   = Groesse - 1 falls tx > 0x1FFFF, sonst Groesse (dv analog)   @0x80078070-88
      ax   = (SX << 16) + cx * tx (32 Bit), x0 = ax >> 16, x1 = (ax + w) >> 16      @0x80078090-c0
      ay   = (SY << 16) + cy * ty (32 Bit), y0 = ay >> 16, y1 = (ay + h) >> 16      @0x800780a0-dc
      u0,v0 = u,v ; u1 = u + du ; v1 = v + dv          @0x80078104-14c

Aufruf: python re2fx_billboard_hand.py   (rechnet die zwei im Dossier bau_d.md §N4 belegten Pins nach)
Rueckgabe 0 = beide Pins stimmen mit den Literalen in probe_r34_re2fx_bild.c ueberein.
"""
import sys

KAMERA_ROT = [3706, 0, -1781, -421, 3972, -875, 1727, 967, 3593]
KAMERA_TR = [-904, 480, 5618]
H = 208
CAMF = 208          # FUN_80077924: lhu v0,102(v1) / srl v0,v0,7 @0x800779ac-bc (Sonde: 26684 >> 7)
CX, CY = 160, 120


def unr_tabelle():
    # psx-spx: unr_table[i] = max(0, (0x40000 / (i + 0x100) + 1) / 2 - 0x101), i = 0..0x100
    return [max(0, (0x40000 // (i + 0x100) + 1) // 2 - 0x101) for i in range(0x101)]


UNR = unr_tabelle()


def gte_division(h, sz3):
    # psx-spx "GTE Division Inaccuracy": if H < SZ3*2 -> UNR-Newton-Raphson, sonst 0x1FFFF (Ueberlauf)
    if not h < sz3 * 2:
        return 0x1FFFF
    z = 0
    while z < 16 and not (sz3 << z) & 0x8000:
        z += 1
    n = h << z
    d = sz3 << z
    u = UNR[(d - 0x7FC0) >> 7] + 0x101
    d = (0x2000080 - (d * u)) >> 8
    d = (0x0000080 + (d * u)) >> 8
    return min(0x1FFFF, ((n * d) + 0x8000) >> 16)


def klemme(v, lo, hi):
    return lo if v < lo else (hi if v > hi else v)


def rtps(w):
    mac = []
    for r in range(3):
        s = (KAMERA_TR[r] << 12) + sum(KAMERA_ROT[3 * r + c] * w[c] for c in range(3))
        mac.append(s >> 12)
    ir1, ir2 = klemme(mac[0], -0x8000, 0x7FFF), klemme(mac[1], -0x8000, 0x7FFF)
    sz3 = klemme(mac[2], 0, 0xFFFF)
    n = gte_division(H, sz3)
    sx = ((CX << 16) + ir1 * n) >> 16
    sy = ((CY << 16) + ir2 * n) >> 16
    return sx, sy, sz3, n


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def quad(welt, skala, ax_, ay_, groesse, u, v, cx, cy):
    sx, sy, sz3, n = rtps(welt)
    sz = min(sz3, 32767)
    t1 = (groesse * skala * CAMF) & 0xFFFFFFFF
    step = int(t1 / (sz << 4))                        # div: Abschneiden Richtung 0 (t1, SZ > 0)
    w = (step * ax_) & 0xFFFFFFFF
    h = (step * ay_) & 0xFFFFFFFF
    tx, ty = w // groesse, h // groesse
    du = groesse - 1 if tx > 0x1FFFF else groesse
    dv = groesse - 1 if ty > 0x1FFFF else groesse
    ax = (((sx & 0xFFFF) << 16) + cx * tx) & 0xFFFFFFFF
    ay = (((sy & 0xFFFF) << 16) + cy * ty) & 0xFFFFFFFF
    x0, x1 = s16(ax >> 16), s16(((ax + w) & 0xFFFFFFFF) >> 16)
    y0, y1 = s16(ay >> 16), s16(((ay + h) & 0xFFFFFFFF) >> 16)
    zw = dict(sx=sx, sy=sy, sz3=sz3, n=n, t1=t1, step=step, w=w, h=h, tx=tx, ty=ty)
    return (x0, y0, x1, y1, u, v, (u + du) & 0xFF, (v + dv) & 0xFF), zw


# (Name, Welt, Skala, AspektX, AspektY, Groesse, u, v, cx, cy, Literal aus probe_r34_re2fx_bild.c)
PINS = [
    ('Saeure 0x030F2000 Bild X+1 Platz 94', (0, 0, 0), 8192, 4096, 4096, 16, 0, 32, -24, -20,
     (97, 113, 116, 132, 0, 32, 16, 48)),
    ('Bodenflamme 0x0505 Bild X+1 Platz 92', (0, 0, 0), 11008, 4096, 4096, 40, 0, 168, -20, -32,
     (94, 86, 157, 149, 0, 168, 40, 208)),
    ('Saeure 0x040D2800 Bild X+4 Platz 89 (Trim)', (716, 69, -1), 10240, 6656, 6656, 16, 0, 72, -24, -28,
     (96, 72, 132, 108, 0, 72, 15, 87)),
]


def main():
    fehler = 0
    for name, welt, skala, a_x, a_y, g, u, v, cx, cy, soll in PINS:
        ist, zw = quad(welt, skala, a_x, a_y, g, u, v, cx, cy)
        ok = ist == soll
        fehler += 0 if ok else 1
        print('%s: %s' % (name, 'OK' if ok else 'ABWEICHUNG'))
        print('   RTPS SX %(sx)d SY %(sy)d SZ3 %(sz3)d n %(n)d | t1 %(t1)d step %(step)d w %(w)d h %(h)d tx %(tx)d ty %(ty)d' % zw)
        print('   Hand (%d,%d)-(%d,%d) uv (%d,%d)-(%d,%d); Literal (%d,%d)-(%d,%d) uv (%d,%d)-(%d,%d)' % (ist + soll))
    return 1 if fehler else 0


if __name__ == '__main__':
    sys.exit(main())
