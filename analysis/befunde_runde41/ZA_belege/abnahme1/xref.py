import struct, sys, os
ROOT=r"C:\workspace\git\reAi_v2"
def bins(which):
    base = os.path.join(ROOT,"Version_40%") if which=="disc" else os.path.join(ROOT,"info","Re1.5")
    exe=open(os.path.join(base,"PSX.EXE"),"rb").read()
    ta=struct.unpack_from("<I",exe,0x18)[0]
    out=[("EXE",exe[0x800:],ta)]
    bd=os.path.join(base,"PSX","BIN")
    for f in sorted(os.listdir(bd)):
        if f.upper().endswith(".BIN"):
            out.append((f,open(os.path.join(bd,f),"rb").read(),0x80100000))
    return out
def jal_xref(which, tgt):
    w=0x0c000000|((tgt>>2)&0x3ffffff); j=0x08000000|((tgt>>2)&0x3ffffff)
    res=[]
    for name,d,base in bins(which):
        for o in range(0,len(d)-3,4):
            v=struct.unpack_from("<I",d,o)[0]
            if v==w or v==j: res.append((name,base+o,"jal" if v==w else "j"))
            # lui/addiu pairs
    return res
def ptr_xref(which,tgt):
    res=[]
    for name,d,base in bins(which):
        for o in range(0,len(d)-3,4):
            if struct.unpack_from("<I",d,o)[0]==tgt: res.append((name,base+o))
    return res
if __name__=="__main__":
    which=sys.argv[1]
    for t in sys.argv[2:]:
        t=int(t,16)
        print("==",hex(t),"jal/j:",[(n,hex(a),k) for n,a,k in jal_xref(which,t)])
        print("   data:",[(n,hex(a)) for n,a in ptr_xref(which,t)])
