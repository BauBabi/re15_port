# Werkzeug zu include/re15_finale_r42.h (Wurfszene, Plan S5, Stufe 3): python re15_port/tools/r42_finale/u_wurf.py
# Misst die RE2-Rollen der Wurfszene (ROOM7020 sub12) und setzt sie in Wagen C von ROOM5090/5091 um. Nur Lesen, keine Schaetzung:
#   RE2 Wurf-Schnitt 7 (`29 07` @0x0184E) mit Ada (Pos_set `32 00 a0 92 b4 e2 14 ba` @0x01868 = (-28000,-7500,-17900),
#     Dir_set `33 00 00 00 74 0e 00 00` @0x01870 = 3700) und Reaktions-Schnitt 11 (`29 0b` @0x018FC) mit Leon (Pos_set
#     `32 00 58 b7 00 00 3c ab` @0x018D8 = (-18600,0,-21700), Dir_set `33 00 00 00 f8 f8 00 00` @0x018E0 = 0xF8F8 & 0xFFF = 0x8F8).
#   Port: der Werfer erscheint an der Ankunft in Wagen C = Ziel der einzigen Tuer nach Wagen C (ROOM5090 main00 Slot 2 @0x0108E
#     `.. e0 60 00 00 02 a4 00 08 04 09 0e ..` = (24800,0,-23550), Richtung 0x0800, Schnitt 0x0E; 5091 bytegleich, Riegel).
#   Kamera-Mass wie u3_schnitte.py (mass_re2/mass_p: Bild-x relativ +-1 = Bildrand, Groesse = H / Abstand).
import math, os, struct, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
from form_kamera import blickmatrix

R7020 = os.path.join(REPO, 'info/re2leon/PL0/RDT/ROOM7020.RDT')
R5090 = os.path.join(REPO, 're15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT')
R5091 = os.path.join(REPO, 're15_port/shared_assets/PSX/STAGE5/ROOM5091.RDT')

def rid(path, base):
    d = open(path, 'rb').read(); out = {}
    for c in range(16):
        fl, h, ex, ey, ez, tx, ty, tz, m = struct.unpack_from('<HHiiiiiiI', d, base + 32 * c)
        out[c] = (h >> 7, np.array((ex, ey, ez), float), np.array((tx, ty, tz), float))
    return out
RE2 = rid(R7020, 0x64); P50 = rid(R5090, 0x60)

def lies(path, off, n): return open(path, 'rb').read()[off:off + n]
def s16(b, o): return struct.unpack_from('<h', b, o)[0]

# ---- Bytes selbst lesen (Assert = Beleg) ----
sub12 = lies(R7020, 0x01868, 8); assert sub12 == bytes.fromhex('3200a092b4e214ba'), sub12.hex()
ADA = (s16(sub12, 2), s16(sub12, 4), s16(sub12, 6))
d = lies(R7020, 0x01870, 8); assert d == bytes.fromhex('3300000074 0e0000'.replace(' ', '')), d.hex()
ADA_G = s16(d, 4) & 0xfff
lp = lies(R7020, 0x018D8, 8); assert lp == bytes.fromhex('320058b700003cab'), lp.hex()
LEON = (s16(lp, 2), s16(lp, 4), s16(lp, 6))
ld = lies(R7020, 0x018E0, 8); assert ld == bytes.fromhex('33000000f8f80000'), ld.hex()
LEON_G = s16(ld, 4) & 0xfff
for p in (R5090, R5091):
    t = lies(p, 0x0108E, 32)
    assert t[0] == 0x3b and t[1] == 2, t.hex()
TUER = lies(R5090, 0x0108E, 32); assert TUER == lies(R5091, 0x0108E, 32)
J_X, J_Y, J_Z, J_G = s16(TUER, 14), s16(TUER, 16), s16(TUER, 18), s16(TUER, 20) & 0xfff
J_CUT = TUER[24]
assert (J_X, J_Y, J_Z, J_G, J_CUT) == (24800, 0, -23550, 2048, 14), (J_X, J_Y, J_Z, J_G, J_CUT)

def peilung(x0, z0, x1, z1):
    """finale_r42.c peilung() = FUN_800154ac: Gier von (x0,z0) nach (x1,z1); vorwaerts(g) = (cos, -sin)."""
    a = math.atan2(-(z1 - z0), (x1 - x0))
    return int(round(a * 4096 / (2 * math.pi))) & 0xfff

def mass(cam, p, kopf_y):
    h, e, t = cam; R = blickmatrix(e, t)
    v = R @ (np.array((p[0], kopf_y, p[2]), float) - e)
    if v[2] <= 50: return None
    return (v[0] / v[2]) * h / 160.0, h / math.hypot(p[0] - e[0], p[2] - e[2]), v

def frontal(cam, p, g):
    """cos zwischen Blick der Figur und Richtung Figur -> Kamera (1 = schaut in die Kamera)."""
    h, e, t = cam
    f = np.array((math.cos(g * 2 * math.pi / 4096), -math.sin(g * 2 * math.pi / 4096)))
    k = np.array((e[0] - p[0], e[2] - p[2])); k /= np.linalg.norm(k)
    return float(f @ k)

print('== RE2 (Bytes selbst gelesen)')
m = mass(RE2[7], ADA, ADA[1] - 1200)
print('Schnitt 7  Ada %s Gier %d: Bild-x %+.3f Groesse %.4f frontal %+.3f' % (ADA, ADA_G, m[0], m[1], frontal(RE2[7], ADA, ADA_G)))
m = mass(RE2[11], LEON, -1200)
rel = (LEON_G - peilung(LEON[0], LEON[2], ADA[0], ADA[2])) & 0xfff
print('Schnitt 11 Leon %s Gier %d: Bild-x %+.3f Groesse %.4f frontal %+.3f; Gier - Peilung(Leon->Ada) = %d' %
      (LEON, LEON_G, m[0], m[1], frontal(RE2[11], LEON, LEON_G), rel))
print('== Port Wagen C: Werfer an der Ankunft (Tuer Slot 2 @0x0108E) (%d,%d,%d) Gier %d, Ankunfts-Schnitt %d' % (J_X, J_Y, J_Z, J_G, J_CUT))
J = (J_X, J_Y, J_Z)
for c in (11, 12, 13, 14, 15):
    m = mass(P50[c], J, -1200)
    if m is None: print('  Schnitt %d: hinter dem Auge' % c); continue
    print('  Schnitt %d: Bild-x %+.3f Groesse %.4f frontal (Gier %d) %+.3f' % (c, m[0], m[1], J_G, frontal(P50[c], J, J_G)))
