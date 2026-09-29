#!/usr/bin/env python3
"""Gegenpruefung R34: unabhaengiger Wurf-Simulator nach eigener Disassembly.
Bild 0 = Spawn-Bild. Je Bild (FUN_80019e20): Schleife 1 Routine A (R30 im Bild 0; R31 ab Liegen+1),
Hauptlauf: (c) Welt-y = h + xlat_y (RotY laesst y unveraendert, Euler=0), (d) R29, (e) Physik falls Flags&0x20==0:
xlat += vel; vel += acc. C-Division trunc (0x55555556-Idiom)."""
import sys
def cdiv3(v):
    q = abs(v) // 3
    return q if v >= 0 else -q
def sim(h, vx, vy, vz, ax, n, ay=10, verbose=False):
    xl = [0, 0, 0]; v = [vx, vy, vz]; a = [ax, ay, 0]
    flags = 3; A = 0; B = 29; fuse = 42; L = None; contacts = []; events = {}
    for k in range(0, 400):
        # Schleife 1: Routine A
        if k > 0 and A == 31:
            if fuse == 0:
                events['frei'] = k; break
            if fuse == 7: events['explosion'] = k; flags = 0x61
            if fuse == 2: events['zuender2'] = k
            fuse -= 1
        # Hauptlauf (c)
        wy = (h + xl[1])
        wy = ((wy + 0x8000) & 0xffff) - 0x8000   # s16 wie lh 42(t0)
        # (d) R29
        if B == 29 and wy > 0:
            if n == 0:
                contacts.append((k, wy, 'liegen')); flags = 0x63; A = 31; B = 0; L = k
            else:
                v[0] = v[0] - cdiv3(v[0]); n -= 1
                xl[1] -= wy
                v[1] = -cdiv3(v[1])
                contacts.append((k, wy, 'abprall n=%d' % n))
        # (e) Physik
        if not (flags & 0x20):
            for i in range(3): xl[i] += v[i]
            for i in range(3): v[i] += a[i]
            v = [((x + 0x8000) & 0xffff) - 0x8000 for x in v]  # s16 Felder
        if verbose: print(k, wy, xl, v)
    return contacts, L, events, xl
if __name__ == '__main__':
    cases = [('HOCH gesund', -3171, 380, -110, 21, -2, 7), ('MITTE gesund', -2474, 280, -50, 24, -1, 7),
             ('TIEF gesund', -772, 80, 0, 1, -1, 5), ('HOCH vergiftet', -3171, 380, -110, 21, -2, 9),
             ('MITTE vergiftet', -2474, 280, -50, 24, -1, 9)]
    for name, h, vx, vy, vz, ax, n in cases:
        c, L, ev, xl = sim(h, vx, vy, vz, ax, n)
        print('%-16s 1.Kontakt %3d  Kontakte %2d  Liegen %3d  Explosion %s  Z2 %s  frei %s  Weg x/z %d/%d' % (
            name, c[0][0], len(c), L, ev.get('explosion'), ev.get('zuender2'), ev.get('frei'), xl[0], xl[2]))
        print('   Kontaktbilder:', [(k, wy) for k, wy, _ in c])
