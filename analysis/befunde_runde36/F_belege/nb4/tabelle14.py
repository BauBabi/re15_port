import os,sys,importlib.util
H=os.path.dirname(os.path.abspath(__file__))
spec=importlib.util.spec_from_file_location("fn",os.path.join(H,"fest_nach.py")); fn=importlib.util.module_from_spec(spec)
sys.argv_save=sys.argv; sys.argv=[sys.argv[0]]; spec.loader.exec_module(fn); sys.argv=sys.argv_save
A=os.path.join(H,"..","F_abn3")
pos=["-7000,12500","-14000,2000","6000,-9000","-24000,10000","7000,4000","9000,10000","-14746,-4140","7200,-4130","-14000,4000","-14500,6000","-24452,11938","-22288,11938","-26500,15500","-20000,14500"]
var=[(A,"abn0f_basis","v0.8.23"),(A,"abn3f_neu","NB3"),(H,sys.argv[1] if len(sys.argv)>1 else "nb4h","NB4")]
tot={v[2]:[0,0,0,0] for v in var}
print("| # | Leon | "+" | ".join("%s S2/S3 Angriff (fest; fern; fern nach 1. Angriff)"%v[2] for v in var)+" |")
for i,p in enumerate(pos):
    cells=[]
    for d,v,name in var:
        f=os.path.join(d,"affen_%s_%s"%(p,v),"gy.log")
        if not os.path.exists(f): cells.append("-"); continue
        r,e0,lf=fn.ausw(f)
        a2,a3=r[2][0],r[3][0]
        fe=r[2][1]+r[3][1]; fr=r[2][2]+r[3][2]; nf=r[2][4]+r[3][4]
        t=tot[name]; t[0]+=fe; t[1]+=fr; t[2]+=nf; t[3]+=r[2][3]+r[3][3]
        cells.append("%s / %s (%d/%d; %d/%d; %d/%d)"%("F%d"%a2 if a2 else "nie","F%d"%a3 if a3 else "nie",r[2][1],r[3][1],r[2][2],r[3][2],r[2][4],r[3][4]))
    print("| %d | (%s) | %s |"%(i+1,p," | ".join(cells)))
for k,t in tot.items(): print("Summe %s: fest %d, davon fern %d; nach dem ersten Angriff %d, davon fern %d"%(k,t[0],t[1],t[3],t[2]))
