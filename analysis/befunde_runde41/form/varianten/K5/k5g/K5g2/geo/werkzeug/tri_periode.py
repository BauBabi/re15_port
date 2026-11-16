"""Bildlaenge aus einem RE15_TRILOG-Protokoll ohne Bildmarken, wenn alle Dreiecke im selben TIM-Slot liegen:
die Form steht in fester Pose, ihre Dreiecke kehren Bild fuer Bild wortgleich wieder. Abstand der Wiederkehr = Dreiecke je Bild.
  python tri_periode.py <log> <log_mit_getrennten_slots> <slot_spielerin> <slot_form>
"""
import json
import sys
from collections import Counter

def zeilen(pfad):
    aus = []
    with open(pfad, "r", errors="replace") as f:
        for z in f:
            i = z.find(" ")
            aus.append(z[i + 1:].rstrip())
    return aus

def clut(z):
    i = z.find("clut=")
    return z[i + 5:i + 11]

L = zeilen(sys.argv[1])
B = zeilen(sys.argv[2])
sp, fo = "slot=%s " % sys.argv[3], "slot=%s " % sys.argv[4]
cl_sp = set(clut(z) for z in B if sp in z)
cl_fo = set(clut(z) for z in B if fo in z)
# Wiederkehr: letztes volles Stueck des Protokolls
ende = L[-60000:]
per = Counter()
ort = {}
for i, z in enumerate(ende):
    if z in ort:
        per[i - ort[z]] += 1
    ort[z] = i
haeufig = per.most_common(4)
P = haeufig[0][0]
# je Bild zaehlen: Protokoll von hinten in Stuecke der Laenge P, solange das Stueck wortgleich wiederkehrt (nur Form-Zeilen verglichen)
n = len(L)
bilder = []
k = n
ist_form = lambda z: clut(z) in cl_fo and clut(z) not in cl_sp
while k - 2 * P >= 0:
    a, b = L[k - P:k], L[k - 2 * P:k - P]
    fa = [z for z in a if ist_form(z)]
    fb = [z for z in b if ist_form(z)]
    if fa != fb:
        break
    bilder.append((len(a), len(fa), len(a) - len(fa)))
    k -= P
print(json.dumps(dict(zeilen=n, clut_spielerin=sorted(cl_sp), clut_form=sorted(cl_fo), gemeinsam=sorted(cl_sp & cl_fo), wiederkehr=haeufig, periode=P,
                      bilder=len(bilder), max_gesamt=max(b[0] for b in bilder), max_form=max(b[1] for b in bilder), max_rest=max(b[2] for b in bilder))))
