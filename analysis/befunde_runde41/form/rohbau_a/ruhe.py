#!/usr/bin/env python3
"""Ruhepose (Clip 2): alle Viere am Boden, Schwanz faellt zum Boden, Kopf vorn. Frei gesetzt, beide Seiten gleich."""
from kn import INDEX as K


def g(d):
    return int(round(d * 4096 / 360.0))


def pose(ks, posen):
    """Alle Viere am Boden, Rumpf steigt 15 Grad zur Brust, Schwanz faellt im Bogen zum Boden, Kopf waagerecht, Maul leicht offen. Frei gesetzt, Rechnung im Dossier."""
    w = [[0, 0, 0] for _ in ks]
    w[K['huefte']] = [0, 0, g(2)]
    w[K['wirbel']] = [0, 0, g(3)]
    w[K['brust']] = [0, 0, g(-20)]
    w[K['hals']] = [0, 0, g(18)]
    w[K['kopf']] = [0, 0, g(-3)]
    w[K['kiefer']] = [0, 0, g(8)]
    for i, wi in zip(('schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6'), (-34, -18, -8, 0, 14, 24)):
        w[K[i]] = [0, 0, g(wi)]
    for s, sg in (('r', 1), ('l', -1)):
        w[K['oberarm_' + s]] = [0, 0, g(30)]
        w[K['unterarm_' + s]] = [0, 0, g(-15)]
        w[K['hand_' + s]] = [0, 0, 0]
        w[K['oberschenkel_' + s]] = [0, 0, g(-32)]
        w[K['unterschenkel_' + s]] = [0, 0, g(64)]
        w[K['fuss_' + s]] = [0, 0, g(-34)]
        w[K['klaue_%s1' % s]] = [0, g(sg * -10), g(6)]
        w[K['klaue_%s2' % s]] = [0, 0, g(6)]
        w[K['klaue_%s3' % s]] = [0, g(sg * 10), g(6)]
        w[K['klaue_%s4' % s]] = [0, g(sg * -40), g(6)]
    return dict(lage=[0, 0, 0], gier=0, wurzel=[0, -2190, 0], winkel=w)
