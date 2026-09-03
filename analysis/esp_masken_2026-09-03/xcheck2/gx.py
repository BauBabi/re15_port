import sys, re, io
PATH=r"C:/workspace/git/reAi_v2/ghidra1_V2.txt"
def dump(start,end,path=PATH):
    s=int(start,16) if isinstance(start,str) else start
    e=int(end,16) if isinstance(end,str) else end
    out=[]
    inr=False
    with open(path,'r',encoding='utf-8',errors='replace') as f:
        for line in f:
            m=re.match(r'^\s*([0-9a-f]{8})\s',line)
            if m:
                a=int(m.group(1),16)
                if s<=a<=e:
                    out.append(line.rstrip())
                    inr=True
                elif inr and a>e:
                    # keep scanning; addresses may repeat in data section
                    pass
    return out
if __name__=="__main__":
    for l in dump(sys.argv[1],sys.argv[2]):
        print(l)
