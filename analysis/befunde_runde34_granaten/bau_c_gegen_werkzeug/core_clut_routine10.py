import os; exec(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),'esp_zeilen_zensus.py')).read().split("IMPL =")[0])
core=open(os.path.join(ROOT,"DATA/CORE00.ESP"),"rb").read()
for e in parse(core,0,((len(core)+3)&~3)-4):
    hc=u16(core,e['start']+4)
    for sub,s,r,row in rows_of(core,e):
        A=u16(row,0)
        if A==10:
            add=u16(row,0x1e)
            base=(hc + (sub>>3)*0x40)
            print("id %d sub %d st %d r %d: A=10 CLUT seed %04x (Zeile %d) + row1e %d -> Zeile %d"%(e['id'],sub,s,r,base,base>>6,add,(base+(add<<6))>>6))
