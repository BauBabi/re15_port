import struct
d=open("c:/workspace/git/reAi_v2/info/re2leon/PL0/PLD/PL00.PLD","rb").read()
base=8
c0,o0=struct.unpack_from("<HH",d,base); n=o0//4
clips=[c0]; cur=4
for i in range(1,n):
    c,o=struct.unpack_from("<HH",d,base+cur); clips.append(c); cur+=4
print("PL00 pair1: %d clips:"%n, clips)
emr=0x59C
bto,kfo,bc,kfs=struct.unpack_from("<HHHH",d,emr)
print("EMR: bones=%d kfsize=%d"%(bc,kfs))
