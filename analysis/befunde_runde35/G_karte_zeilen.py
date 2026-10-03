"""Runde 35 Spur G — rechnet die neuen/korrigierten Zonenzeilen fuer re15_map_zones.h nach.

Aufruf (aus dem Baum): python analysis/befunde_runde35/G_karte_zeilen.py
Liest nur info/Re1.5/PSX.EXE (Massstabszeilen @0x800768b0) und gibt die Zeilen samt Probe aus.
Formel = FUN_800473f8 @0x8004741c-0x80047528 (wie re15_map_zone_marker, Zeilen-Pfad):
    t  = ((x+32000)*10*sx) >> 20 ;  px = (flip_x ? -1 : 1) * ((t+5)/10) + ox
    t2 = ((z+32000)*10*sy) >> 20 ;  py = (flip_z ? 1 : -1) * ((t2+5)/10) + oy
Ganzzahlig in int32 wie die Engine (re15_map_zones.c:494-503) - Ueberlauf wird geprueft.
"""
import os, struct

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
EXE = open(os.path.join(ROOT, 'info', 'Re1.5', 'PSX.EXE'), 'rb').read()


def fo(a):
    return a - 0x80010000 + 0x800


def zeile(idx):
    a = 0x800768b0 + 8 * idx
    ox, oy, sx, sy = struct.unpack_from('<hhHH', EXE, fo(a))
    return ox, oy, sx, sy


def i32(v):
    v &= 0xffffffff
    return v - (1 << 32) if v & 0x80000000 else v


def f(v, s):
    raw = (v + 32000) * 10 * s
    assert -(1 << 31) <= raw < (1 << 31), 'int32-Ueberlauf'
    t = i32(raw) >> 20
    return int((t + 5) / 10)


def proj(x, z, r):
    ox, oy, sx, sy, fx, fz = r
    px = (-f(x, sx) if fx else f(x, sx)) + ox
    py = (f(z, sy) if fz else -f(z, sy)) + oy
    return px, py


def passe(w0, w1, m0, m1):
    """kleinste positive Skala, deren Spanne |m1-m0| Pixel ergibt"""
    for s in range(100, 20000):
        try:
            if abs(f(w1, s) - f(w0, s)) == abs(m1 - m0):
                return s
        except AssertionError:
            return None
    return None


def strecke(x0, x1, z0, z1, mx_x0, mx_x1, my_z0, my_z1):
    """Weltkasten -> Pixelkasten; Spiegel aus der Richtung der Zielwerte."""
    fx = 1 if mx_x1 < mx_x0 else 0
    fz = 1 if my_z1 > my_z0 else 0          # y waechst mit z = gespiegelt
    sx = passe(x0, x1, mx_x0, mx_x1)
    sy = passe(z0, z1, my_z0, my_z1)
    ox = mx_x0 - (-f(x0, sx) if fx else f(x0, sx))
    oy = my_z0 - (f(z0, sy) if fz else -f(z0, sy))
    return ox, oy, sx, sy, fx, fz


def zeig(name, box, r, pruef=()):
    x0, z0, x1, z1 = box
    print('%-34s ox %4d oy %4d sx %5d sy %5d flip %d,%d  Ecken %s %s' %
          ((name,) + r[:4] + r[4:] + (proj(x0, z0, r), proj(x1, z1, r))))
    for nm, (x, z) in pruef:
        print('    %-28s (%6d,%6d) -> %s' % (nm, x, z, proj(x, z, r)))


if __name__ == '__main__':
    print('== Punkt 1: Fahrstuhlkabine ROOM1080 (Innenraum x -15750..-11550, z -4150..-50) ==')
    # Ziel = Schnitt aus gemaltem Kabinen-Innenraum (Blatt 2/3: x110..117 y135..142, Blatt 4:
    # x128..135 y138..145) und Klemmfenster des Markers (Rechteck +4: Blatt 2/3 x113..121
    # y138..146, Blatt 4 x131..139 y141..149). 180 Grad gedreht: Welt-Ost -> Karte-West,
    # Welt-Nord (+z, Tuerseite) -> Karte-Sued.
    k23 = strecke(-15750, -11550, -4150, -50, 117, 113, 138, 142)
    k4 = strecke(-15750, -11550, -4150, -50, 135, 131, 141, 145)
    zeig('1080 Blatt 2 rect 9 / Blatt 3 rect 4', (-15750, -4150, -11550, -50), k23,
         (('Ankunft', (-13650, -900)), ('Panel', (-12050, -50))))
    zeig('1080 Blatt 4 rect 0', (-15750, -4150, -11550, -50), k4)

    print('== Punkt 4: ROOM1220 = Zellen im Rahmen von ROOM1210 (Zeile @0x800769b8) ==')
    r1210 = zeile(33) + (0, 0)
    print('   1210-Zeile', r1210[:4])
    for nm, box, sp in (('L1 -> rect 4', (-27611, -11725, -21836, -4500), (-22400, -6500)),
                        ('R1 -> rect 6', (-17075, -11725, -11311, -4525), (-16600, -9900)),
                        ('L2 -> rect 5', (-27611, -19675, -21836, -11925), (-22400, -14000)),
                        ('R2 -> rect 7', (-17075, -19675, -11311, -11925), (-16600, -17900)),
                        ('R3 -> rect 8', (-17075, -26750, -11311, -19875), (-16600, -25050))):
        zeig('1220 ' + nm, box, r1210, (('Spawn aus 1210', sp),))

    print('== Punkt 3: ROOM1180/1230 auf Blatt 0 rect 0 (Gang als Ring gemalt) ==')
    # Ziel-Streifen = gemalte Innenflaeche von rect 0 (Index 1), Abschnitte s. Dossier.
    # S3: Skala nach oben durch int32 begrenzt - (6733+32000)*10*sx < 2^31 -> sx < 5544
    # (6733 = Ostkante der SCA-Huelle; der Naechstgelegen-Rueckfall kann jede Lage im Raum
    # an diese Zone geben). 15 px statt der vollen 31 px des Balkens.
    segs = (
        ('S0 C-Sued  (Laengsgang)', (2190, -4197, 5514, 15304), (160, 167, 104, 69), ()),
        ('S1 Zweig + Bein (1190)', (-9566, 9476, 2190, 15304), (136, 167, 75, 61),
         (('Tuer 1190 s2', (-8118, 9355)), ('Tuer 1190 s4', (96, 12161)))),
        ('S2 Versatz (10A0)', (-2657, -7359, 5514, -4197), (145, 167, 112, 105),
         (('Tuer 10A0', (-177, -7772)),)),
        ('S3 Suedgang (11D0)', (-5628, -16384, -2657, -4197), (126, 141, 123, 116),
         (('Tuer 11D0', (-4837, -16839)),)),
        ('S4 C-Nord  (11B0)', (2190, 15304, 5514, 31058), (160, 167, 68, 61),
         (('Tuer 11B0', (5706, 28874)),)),
    )
    for nm, (x0, z0, x1, z1), (a, b, c, d), pr in segs:
        r = strecke(x0, x1, z0, z1, a, b, c, d)
        zeig(nm, (x0, z0, x1, z1), r, pr)
        f(6733, r[2]); f(-10592, r[2]); f(32871, r[3]); f(-17477, r[3])   # Ueberlauf-Probe
