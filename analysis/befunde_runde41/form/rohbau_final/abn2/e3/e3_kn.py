#!/usr/bin/env python3
"""Welcher Knochen deckt den Keil (Bild 1)? Kennbild aus form_pruefung.teilbild."""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
RF = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, os.path.normpath(os.path.join(RF, '..', '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P
k = P.Kandidat(sys.argv[1] if len(sys.argv) > 1 else os.path.join(RF, 'form.json'))
zu = P.teil_zuordnung(k, 1)
zu = zu[0] if isinstance(zu, tuple) else zu
tb = P.teilbild(k, 1, zu)
kn = tb['kn']
for y in range(264, 300, 2):
    print(y, ' '.join('%2s' % (kn[y, x] if kn[y, x] >= 0 else '.') for x in range(404, 436, 2)))
