# homo.py bg.ppm frame.jpg out.json : SIFT+RANSAC Homographie BG(320x240)->Video
import sys,json
import cv2,numpy as np
bg=cv2.imread(sys.argv[1]); 
frs=[cv2.imread(p) for p in sys.argv[3:]]
sift=cv2.SIFT_create(nfeatures=4000,contrastThreshold=0.02)
def up(i): return cv2.resize(i,None,fx=2,fy=2,interpolation=cv2.INTER_CUBIC)
kb,db=sift.detectAndCompute(cv2.cvtColor(up(bg),cv2.COLOR_BGR2GRAY),None)
ptsb=[];ptsv=[]
for fr in frs:
    kv,dv=sift.detectAndCompute(cv2.cvtColor(up(fr),cv2.COLOR_BGR2GRAY),None)
    m=cv2.BFMatcher().knnMatch(db,dv,k=2)
    g=[a for a,b in m if a.distance<0.8*b.distance]
    ptsb+= [kb[a.queryIdx].pt for a in g]; ptsv+=[kv[a.trainIdx].pt for a in g]
A=np.float32(ptsb)/2;B=np.float32(ptsv)/2
print(len(A),'matches')
H,mask=cv2.findHomography(A,B,cv2.RANSAC,1.5,maxIters=20000)
print('inlier',int(mask.sum())); print(H/H[2,2])
json.dump({'H':(H/H[2,2]).tolist(),'inlier':int(mask.sum())},open(sys.argv[2],'w'))
