R='C:/workspace/git/reAi_v2/'
for tag,b in (('DISC','Version_40%/'),('MOD','info/Re1.5/')):
    e=open(R+b+'PSX.EXE','rb').read()
    print(tag)
    for st,rm in ((2,0),(2,5),(2,7),(2,8),(2,9),(2,10),(3,7),(3,8),(5,8),(5,9),(5,10),(5,13),(5,14),(5,15),(6,3),(6,4)):
        a=0x8006ff34+1274*(st-1)+26*rm; o=a-0x80010000+0x800; x=e[o:o+26]
        print('  %d%02X @%#x %s %r'%(st,rm,a,x.hex(' '),x.split(b'\x00')[0]))
