"""Was DECKT die ausgelieferte Maske wirklich, mit welcher Tiefe — und was sieht man
im Render F3843? Grundlage: der ECHTE Ladeweg (maskenbild + abnahme)."""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import abnahme, maskenbild, geom, geometrie

CD="re15_port/shared_assets/PSX"; ROOM,CUT="ROOM10D0",7
blob=open(os.path.join(CD,"MASKS","%s.MSK"%ROOM),"rb").read()
ms=maskenbild.masken(blob,CUT) or []
tim=open(os.path.join(CD,"MASKS","%s_PRI%02d.TIM"%(ROOM,CUT)),"rb").read()
idx=maskenbild.lies_tim_bytes(tim)[0]
deck,tief=abnahme.deckung_und_tiefe(ms,idx)
print("Masken: %d, gedeckte Punkte: %d, Tiefen %d..%d"%(len(ms),int(deck.sum()),int(tief[deck].min()),int(tief[deck].max())))
np.save("build/r19b/deck.npy",deck); np.save("build/r19b/tief.npy",tief)

reg=np.load("build/r19b/reg0701.npy"); reg2=np.load("build/r19b/reg0702.npy")
bg=np.load("build/r19b/bg10d07.npy"); r=np.load("build/r19b/render_F3843.npy")
print("07_01: %d Soll, %d gedeckt, %d fehlen"%(reg.sum(),(reg&deck).sum(),(reg&~deck).sum()))
print("07_02: %d Soll, %d gedeckt, %d fehlen"%(reg2.sum(),(reg2&deck).sum(),(reg2&~deck).sum()))
# Tiefen der 07_01-Punkte
t1=tief[reg&deck]
print("Tiefen unter 07_01: %d..%d  Histogramm:"%(t1.min(),t1.max()))
for v,c in zip(*np.unique(t1,return_counts=True)): print("   t=%3d  %5d Punkte"%(v,c))
# je Spalte 69..72
print("\nTiefen der Freistellung in den Spalten 66..72:")
for x in range(66,73):
    rr=np.nonzero(reg[:,x])[0]
    print("  x=%2d y%3d..%3d: %s"%(x,rr.min(),rr.max(),
        " ".join("%d@%d"%(tief[y,x],y) for y in rr)))
