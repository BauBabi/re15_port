# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Tonblaetter. Einheitlicher Ton statt Textur, gleiche Lichter, Zeichenweg wie die Buehne (Maler, Rueckseiten weg). Dazu Eichmodelle."""
from u_basis import *


class Ton:
    """Textur-Ersatz: alle Texel weiss, Loecher bleiben Loecher, wenn die echte Textur eine hat."""

    def __init__(self, tex):
        self.w, self.h, self.clut_y = tex.w, tex.h, tex.clut_y
        self.px = tex.px
        lut = tex.lut.copy()
        lut[..., :3] = 215
        self.lut = lut


def tafel(mo, tex, kf, az, el, gross):
    return bild(mo, Ton(tex), kf, az, el, gross, grund=58)[0]


SICHT = (('links', 180, 6), ('rechts', 0, 6), ('vorn', 90, 8), ('hinten', -90, 16), ('3/4 vorn', 50, 20), ('3/4 hinten', -130, 24))

if __name__ == '__main__':
    for k in ROHBAU:
        mo, tex = modell(k)
        for name, clip in (('ruhe', 2), ('pose1', 0), ('pose2', 1)):
            kf = kf_von(mo, clip)
            t = []
            for s, az, el in SICHT:
                t.append(('%s %s %s' % (k, name, s), tafel(mo, tex, kf, az, el, 640)))
            FB.montage(t, 3, AUS + '/ton_%s_%s.png' % (k, name), '%s %s: Ton, gleiches Blattlicht, Zeichenweg der Buehne' % (k, name))
    for k, clip, bl in (('K34', 5, 40), ('K28', 0, 0), ('K33', 0, 0), ('K23', 0, 0), ('K22', 0, 0)):
        mo, tex = modell(k)
        kf = kf_von(mo, clip, bl)
        t = []
        for s, az, el in SICHT:
            t.append(('%s %s' % (k, s), tafel(mo, tex, kf, az, el, 640)))
        FB.montage(t, 3, AUS + '/ton_%s.png' % k, '%s: Ton, Eichmass Handwerk, keine Formvorlage' % k)
