"""Welche GEMALTEN Rahmenpunkte des Stuhls sind NICHT gedeckt — und wirken sie?
White-Top-Hat: duenne helle Struktur (Chromrohr) gegen den lokalen Hintergrund."""
import numpy as np
from scipy import ndimage as nd
bg=np.load("build/r19b/bg10d07.npy").astype(np.int32)
reg=np.load("build/r19b/reg0701.npy"); reg2=np.load("build/r19b/reg0702.npy")
deck=np.load("build/r19b/deck.npy"); tief=np.load("build/r19b/tief.npy")
r=np.load("build/r19b/render_F3843.npy").astype(np.int32)
L=bg.sum(2)  # Helligkeit r+g+b, 0..765

for SE,SCHW in ((5,16),(5,30),(7,16),(7,30),(9,24)):
    op=nd.grey_opening(L,size=(SE,SE))
    th=L-op
    grat=th>=SCHW*3   # Schwelle in Helligkeit r+g+b
    nah=nd.binary_dilation(reg,np.ones((3,3),bool))&~reg
    kand=grat&nah&~deck
    lab,n=nd.label(kand,np.ones((3,3),bool))
    gr=nd.sum(np.ones_like(lab),lab,range(1,n+1)) if n else []
    print("SE=%d Schwelle=%d: Gratpunkte am Rand und ungedeckt = %d, Komponenten %d, groesste %s"
          %(SE,SCHW,int(kand.sum()),n,sorted([int(g) for g in gr],reverse=True)[:5]))
np.save("build/r19b/L.npy",L)

# Die Kernfrage: welche UNGEDECKTEN Punkte hat der Render uebermalt, und wie hell sind sie?
d=np.abs(r-bg).sum(2)
uebermalt=(d>30)
print("\nuebermalt insgesamt %d; davon ungedeckt %d; unter deckender Maske %d"
      %(int(uebermalt.sum()),int((uebermalt&~deck).sum()),int((uebermalt&deck).sum())))
# im Stuhlfenster
fen=np.zeros_like(reg); fen[120:240,0:80]=True
u=uebermalt&fen&~deck
print("im Stuhlfenster x0..79 y120..239: uebermalt+ungedeckt %d"%int(u.sum()))
# helle darunter (Chrom)
print("  davon Helligkeit>=250: %d ; >=200: %d ; >=150: %d"%(int((u&(L>=250)).sum()),int((u&(L>=200)).sum()),int((u&(L>=150)).sum())))
lab,n=nd.label(u&(L>=200),np.ones((3,3),bool))
sz=[(int(nd.sum(np.ones_like(lab),lab,[i])[0]),i) for i in range(1,n+1)]
sz.sort(reverse=True)
print("  Komponenten der hellen ungedeckten uebermalten Punkte (>=200):")
for s,i in sz[:8]:
    ys,xs=np.nonzero(lab==i)
    print("    %3d Punkte  x%d..%d y%d..%d"%(s,xs.min(),xs.max(),ys.min(),ys.max()))
