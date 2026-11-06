import sys, os, glob
from ssram import R
EB=0x800acc2c; ST=0x1f4
files=[f for f in sorted(glob.glob(r"C:\workspace\git\reAi_v2\stage_saves\**\*.sav",recursive=True)) if "PATCHED" not in f]
files+=sorted(glob.glob(os.path.join(os.path.dirname(os.path.abspath(__file__)),"disc40_*.sav")))  # Version_40%-Savestate (Kopie aus DuckStation/savestates)
nsave=0; nz=0; nvar=0; nbits=0; armvar=[]; per=[]
for f in files:
    try: r=R(f)
    except Exception as e: per.append("%s ERR %s"%(os.path.basename(f),e)); continue
    if r.u32(0x80026e4c)!=0x03e00008: per.append("%s SKIP patched"%os.path.basename(f)); continue
    zs=[]
    for i in range(16):
        a0=EB+i*ST
        if (r.u32(a0)&1)==0: continue
        typ=r.u8(a0+8)
        if not (0x10<=typ<=0x1f): continue
        p=r.u32(a0+0x188)
        if not (0x80000000<=p<0x80200000): continue
        n=r.u8(a0+0x83)
        if n!=15: continue
        words=[r.u32(p+k*0xac) for k in range(15)]
        var=[k for k in range(15) if r.u16(p+k*0xac+0x8a)==0x1000 and r.u16(p+k*0xac+0x88) in (0x5f4,0x7e8) and r.u16(p+k*0xac+0x88)==r.u16(p+k*0xac+0x8c) and r.u32(p+k*0xac+0x94)==0x80 and r.u32(p+k*0xac+0x98)==0x8080e0]
        bits=[k for k in range(15) if words[k]&0x2480]
        zs.append((i,typ,var,bits,words))
    if zs:
        nsave+=1; nz+=len(zs)
        for i,typ,var,bits,words in zs:
            if var: nvar+=1
            if bits: nbits+=1
            if any(k in (8,9,12) for k in var): armvar.append((os.path.basename(f),r.exe,i,typ,var,["%x"%words[k] for k in var]))
        per.append("%-48s exe=%-4s stage=%d room=%02x zombies=%d %s"%(os.path.relpath(f,r"C:\workspace\git\reAi_v2"),r.exe,r.u16(0x800b0fe0)+1,r.u16(0x800b0fe2),len(zs),
              " ".join("s%d/t%02x/var%s/w%s"%(i,typ,var,"1" if set(words)=={1} else "X") for i,typ,var,bits,words in zs)))
print("\n".join(per))
print("SAVES mit aktiven Zombies:",nsave,"Zombie-Eintraege:",nz,"mit Varianten-Rest:",nvar,"mit Bit 0x400/0x80/0x2000 in einem Teil-Wort:",nbits)
print("Arm-Varianten (Teile 8/9/12):",armvar)
