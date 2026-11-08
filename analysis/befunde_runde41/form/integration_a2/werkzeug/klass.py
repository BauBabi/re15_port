# Einordnung der dunklen Kampfbilder (> 600 Promille Luma<24): Schnitt, Lagen, Achsenregel (b) der Wache nachgerechnet
# (bf_nahsicht_wache, birkin_formen.c: k_gang_kam Cut 12 Auge x 18526 Blick -x RID @0x1e0, Cut 13 Auge x 11050 Blick +x RID @0x200,
#  BF_NAH_ACHSE 4500, Mitte = x + cos(rot)*1700 (hit_offset_x, re15_skel_bone_to_world)), Zonen-Schnitt re15_bf_schnitt_aus_lage.
import sys,re,glob,math
S=sys.argv[1]; tag=sys.argv[2]; f0=int(sys.argv[3]); f1=int(sys.argv[4])
L=glob.glob(S+'/wd_'+tag+'/lauf_*')[0]
st={};dk={};dn={}
for l in open(L+'/state.log',errors='replace'):
    m=re.match(r'F(\d+) pad=\S+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)',l)
    if not m: continue
    fm=re.search(r't=30 st=(\d+) ss1=(\d+) ss2=\d+ ss3=\d+ g=\w+ mo=(\d+) af=(\d+)[^@]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)',l)
    st[int(m.group(1))]=(int(m.group(2)),fm and tuple(int(x) for x in fm.groups()))
for l in open(L+'/deck.log'):
    m=re.match(r'F(\d+) cam=(-?\d+)(.*)',l)
    if m: dk[int(m.group(1))]=int(m.group(2))
for l in open(S+'/dunkel_'+tag+'.txt'):
    m=re.match(r'F(\d+) dunkel=(\d+)',l)
    if m: dn[int(m.group(1))]=int(m.group(2))
K=[(12,18526,-1),(13,11050,1)]
def zone(x): return 11 if x<2750 else 12 if x<14000 else 13 if x<20250 else 14
grp={}
for f in range(f0,f1+1):
    if dn.get(f,0)<=600: continue
    plx,fm=st[f]; cut=dk.get(f)
    st_,ss1,mo,af,fx,fz,rot,hp=fm
    auft = st_==1 and ss1==0 and mo==10 and af<60
    mitte=fx+int(math.cos(rot*2*math.pi/4096)*1700)
    regel='-'
    if cut in (12,13):
        k=0 if cut==12 else 1; c,ax,bl=K[k]; c2,ax2,bl2=K[k^1]
        fa=(mitte-ax)*bl; pa=(plx-ax)*bl; gf=(mitte-ax2)*bl2; gp=(plx-ax2)*bl2
        conds=(fa>-2000, fa<4500, pa>fa, gp>=1500, gf>gp)
        names=['fa>-2000','fa<4500','Spieler hinter Form','Gegen>=1500','Form hinter Spieler']
        regel='Form %d vor Auge, Spieler %d, Gegenkamera Spieler %d -> %s' % (fa,pa,gp,'WECHSEL' if all(conds) else 'kein Wechsel (%s)'%','.join(n for n,c_ in zip(names,conds) if not c_))
    key=('Auftritt' if auft else 'Kampf',cut)
    grp.setdefault(key,[]).append((f,dn[f],plx,fx,rot,mitte,zone(plx),regel))
for k,v in grp.items():
    print('%s Cut %s: %d Bilder, max %d, F%d-F%d' % (k[0],k[1],len(v),max(x[1] for x in v),v[0][0],v[-1][0]))
    for x in v[:2]+v[-1:]:
        print('   F%d %d Promille Spieler x %d Form x %d rot %d Mitte %d Zonen-Schnitt %d | %s' % x)
