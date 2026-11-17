import numpy as np, json
from PIL import Image
from scipy import ndimage as nd
from skimage.morphology import skeletonize
import networkx as nx
D=np.asarray(Image.open(r'C:/workspace/git/reAi_v2/Version_80%/Darstellung.png').convert('RGB'),np.int32)
V=np.asarray(Image.open(r'C:/workspace/git/reAi_v2/Version_80%/Darstellung_Vorlage.png').convert('RGB'),np.int32)
d=np.abs(D-V).max(2); m=d>40
lab,n=nd.label(m,np.ones((3,3)))
farben={'rot_T1':(237,28,36),'gruen_T2':(34,177,76),'blau_J':(0,162,232),'hellblau_H':(153,217,234),'weiss_K':(195,195,195)}
# Komponenten je Linie (nur Linien, keine Schrift): Schrift liegt in Textzeilen; Auswahl per Bbox (aus m2_linien)
wahl={'rot_T1':[118],'gruen_T2':[39],'blau_J':[38,40],'hellblau_H':[41],'weiss_K':[63]}
out={}
for k,ids in wahl.items():
    M=np.isin(lab,ids)
    # Farbtreue: Abstand zur Linienfarbe
    c=np.array(farben[k]); dist=np.sqrt(((D-c)**2).sum(2))
    core=M&(dist<60)
    sk=skeletonize(M)
    ys,xs=np.nonzero(sk)
    G=nx.Graph()
    S=set(zip(ys,xs))
    for y,x in S:
        for dy in (-1,0,1):
            for dx in (-1,0,1):
                if (dy or dx) and (y+dy,x+dx) in S: G.add_edge((y,x),(y+dy,x+dx),weight=np.hypot(dy,dx))
    # laengster Pfad zwischen zwei Endpunkten je Zusammenhangskomponente
    polys=[]
    for comp in nx.connected_components(G):
        H=G.subgraph(comp)
        a=next(iter(comp)); dd=nx.single_source_dijkstra_path_length(H,a); b=max(dd,key=dd.get)
        dd=nx.single_source_dijkstra_path_length(H,b); e=max(dd,key=dd.get)
        p=nx.dijkstra_path(H,b,e)
        polys.append([[int(x),int(y)] for y,x in p])
    out[k]={'px':int(M.sum()),'kern_px':int(core.sum()),'bbox':[int(np.nonzero(M)[1].min()),int(np.nonzero(M)[0].min()),int(np.nonzero(M)[1].max()),int(np.nonzero(M)[0].max())],'polylinien':polys}
    for p in polys:
        L=sum(np.hypot(p[i+1][0]-p[i][0],p[i+1][1]-p[i][1]) for i in range(len(p)-1))
        print(k,'n',len(p),'L %.1f'%L,'von',p[0],'bis',p[-1],'stuetz',p[::max(1,len(p)//8)])
json.dump(out,open('m2_linien.json','w'))
