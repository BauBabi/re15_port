import struct, sys, os

P = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
d = open(P,'rb').read()
print("file size = 0x%X (%d)" % (len(d), len(d)))
hdr = d[:8]
names8 = ["nSprite","nCut","nOmodel","nItem","nDoor","nRoom_at","reverb_lv","hdr7_maskmax"]
print("HEADER bytes 0..7:", " ".join("%02x"%b for b in hdr))
for i,n in enumerate(names8):
    print("  [%d] %-14s = %3d (0x%02x)" % (i,n,hdr[i],hdr[i]))

DIR = [
 (0x08,"snd0_edt"),(0x0c,"snd0_vh"),(0x10,"snd0_vb"),
 (0x14,"snd1_edt"),(0x18,"snd1_vh"),(0x1c,"snd1_vb"),
 (0x20,"collision(SCA)"),(0x24,"camera(RID)"),(0x28,"zone(RVD)"),
 (0x2c,"light(LIT)"),(0x30,"model_ptr_tbl"),(0x34,"floor(FLR)"),
 (0x38,"block(BLK)"),(0x3c,"message(MSG)"),(0x40,"scd_main"),
 (0x44,"scd_sub"),(0x48,"scd_extra"),(0x4c,"effect(ESP/EFF)"),
 (0x50,"esp_?? (0x50)"),(0x54,"esp_tim"),(0x58,"model_tim"),(0x5c,"animation(RBJ)")
]
vals=[]
print("\nDIRECTORY 0x08..0x5c:")
for off,name in DIR:
    v = struct.unpack_from("<I", d, off)[0]
    vals.append((off,name,v))
    print("  +0x%02x %-16s = 0x%08X (%d)%s" % (off,name,v,v," <-- OOB" if v>len(d) else ""))

# sorted boundaries
bset = sorted(set(v for _,_,v in vals if 0 < v <= len(d)))
print("\nsorted section starts:", ", ".join("0x%X"%b for b in bset))
def nextb(x):
    for b in bset:
        if b>x: return b
    return len(d)
print("\nSECTION SIZES (by next directory boundary):")
for off,name,v in sorted(vals, key=lambda t:t[2]):
    if v==0 or v>len(d): 
        print("  +0x%02x %-16s start=0x%08X  (invalid/zero)"%(off,name,v)); continue
    e=nextb(v)
    print("  +0x%02x %-16s 0x%06X..0x%06X  size=%7d  first16=%s"%(off,name,v,e,e-v," ".join("%02x"%b for b in d[v:v+16])))
