import cv2, numpy as np, sys, glob, os
src, dst, fps, cols, rows, w = sys.argv[1], sys.argv[2], float(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6])
t0=float(sys.argv[7]) if len(sys.argv)>7 else 0.0
fs=sorted(glob.glob(os.path.join(src,'*.jpg')))
h=int(w*3/4); n=cols*rows
for s in range(0,len(fs),n):
    tiles=[]
    for k,f in enumerate(fs[s:s+n]):
        im=cv2.resize(cv2.imread(f),(w,h))
        t=t0+(s+k)/fps  # ffmpeg fps filter: frame k at ~k/fps
        lab=f'{int(t//60)}:{t%60:04.1f}'
        cv2.rectangle(im,(0,0),(70,18),(0,0,0),-1)
        cv2.putText(im,lab,(2,14),0,0.5,(0,255,255),1)
        tiles.append(im)
    while len(tiles)<n: tiles.append(np.zeros((h,w,3),np.uint8))
    sheet=np.vstack([np.hstack(tiles[r*cols:(r+1)*cols]) for r in range(rows)])
    cv2.imwrite(f'{dst}_{s//n:02d}.jpg',sheet,[cv2.IMWRITE_JPEG_QUALITY,85])
