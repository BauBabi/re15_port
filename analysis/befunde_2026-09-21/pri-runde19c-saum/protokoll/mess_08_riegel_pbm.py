"""Referenz-Silhouette des Riegels nachziehen: probes/r19b_marke4_stuhl.pbm auf die NEUE
Silhouette (Freistellung + Saum des Nutzers). Vorher wird geprueft, dass die ALTE Datei
genau die alte Freistellung ist — sonst wuerde hier ein fremder Stand ueberschrieben.
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import abnahme                                              # noqa: E402

PBM = "re15_port/tests/unit/probes/r19b_marke4_stuhl.pbm"
alt = np.load("build/r19c/alt.npy")
neu = np.load("build/r19c/neu.npy")

vor = abnahme.pbm_lesen(PBM)
print("alte Referenz %s: %d Punkte; == alte Freistellung 07_01: %s"
      % (os.path.basename(PBM), int(vor.sum()), bool(np.array_equal(vor, alt))))
if not np.array_equal(vor, alt):
    raise SystemExit("⛔ die alte Referenz ist NICHT die alte Freistellung — nicht ueberschreiben")

if len(sys.argv) > 1 and sys.argv[1] == "schreiben":
    abnahme.pbm_schreiben(PBM, neu)
    nach = abnahme.pbm_lesen(PBM)
    print("neue Referenz geschrieben: %d Punkte; == neue Freistellung: %s"
          % (int(nach.sum()), bool(np.array_equal(nach, neu))))
else:
    print("neue Silhouette haette %d Punkte (+%d) — mit Argument 'schreiben' ausfuehren"
          % (int(neu.sum()), int(neu.sum() - vor.sum())))
