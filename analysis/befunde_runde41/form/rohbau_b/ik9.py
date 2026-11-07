# -*- coding: utf-8 -*-
"""Bild 2: Daumen rechts zurueck auf die Spitze (274,443). Die Feinanpassung hatte ihn in die Hand gezogen."""
import json, math, os
import numpy as np
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK

f = figur.bauen()
fig = figur.fertig(f)
a = Aufgabe(f, fig, 'bild2', 2)


def tip(name):
    art = (figur.KLAUE_L if '_l' in name else figur.KLAUE_R)[name]
    v, w, lg = figur.FINGER[art]
    sg = -1 if '_l' in name else 1
    w = math.radians(w * sg)
    return (800 * lg / 800. * math.cos(w), 120, 800 * lg / 800. * math.sin(w))


r = a.loese(['klaue_r1'], [('klaue_r1', tip('klaue_r1'), (274, 443), 1)], boden=[('klaue_r1', tip('klaue_r1'), 5)], reg=.005, grenzen={'klaue_r1': ((-20, 20), (-60, 60), (-40, 70))})
print(r, a.winkel[f.ix['klaue_r1']])
speichern(a, ['klaue_r1'])
