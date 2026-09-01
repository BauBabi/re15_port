# -*- coding: utf-8 -*-
"""Relative Frame-Drehung zweier Raeume aus dem TUER-PAAR, robust:
   Achse aus der RECHTECK-FORM (schmal = Durchgangsachse), Vorzeichen aus der Lage des
   Referenzpunktes RELATIV ZUM RECHTECK (nicht zur Mitte). Ungueltig, wenn der Punkt im
   Rechteck liegt oder der Abstand < MARG ist."""
import sys, math, collections, struct
sys.path.insert(0,'analysis/karte_audit_0901')
from sonde_lib import *
MARG = 200

def _axis(d):
    if d['rw'] < d['rd']: return 'X'
    if d['rd'] < d['rw']: return 'Z'
    return None

def _side(d, px, pz, ax):
    """-1 = Punkt liegt unterhalb (kleiner) des Rechtecks, +1 = darueber, 0 = drin/zu nah."""
    if ax == 'X': lo, hi, p = d['rx'], d['rx']+d['rw'], px
    else:         lo, hi, p = d['rz'], d['rz']+d['rd'], pz
    if p < lo - MARG: return -1
    if p > hi + MARG: return +1
    return 0

def pair_rot(a, b, d, e):
    """Drehung, die A-Koordinaten in B-Koordinaten dreht (0/1/2/3 * 90 Grad), oder None."""
    aa, ab = _axis(d), _axis(e)
    if aa is None or ab is None: return None
    sa = _side(d, e['nx'], e['nz'], aa)      # wo steht der Spieler in A, bevor er durchgeht
    sb = _side(e, d['nx'], d['nz'], ab)      # wo steht er in B, nachdem er durch ist
    if sa == 0 or sb == 0: return None
    # Reiserichtung in A = -sa (vom Punkt zur Tuer und hindurch), in B = +sb
    tA, tB = -sa, sb
    if aa == ab:                              # X->X oder Z->Z
        return 0 if tA == tB else 2
    # X->Z oder Z->X: 90 Grad. Drehung R mit R*(Achse A, Richtung tA) = (Achse B, tB)
    # R90: X+ -> Z+ ; Z+ -> X-      (Rechtsdrehung im (x,z)-System)
    if aa == 'X':  return 1 if tA == tB else 3
    else:          return 3 if tA == tB else 1
