#!/usr/bin/env python3
"""RE1.5-Raum-Sound-Baenke (snd0/snd1 aus RDT +0x08..+0x1C) dumpen + jeden SE als WAV rendern.
Format-Belege: RE15_KNOWLEDGE.md 1.1 (RDT 0x08-0x1C), vab_common.c re15_vab_parse (VH-Layout),
re15_edt_decode (FUN_800453d0-Feldsemantik), re15_vab_note2pitch2 (note2pitch2.c)."""
import struct, os, math, wave, sys

REPO = "c:/workspace/git/reAi_v2"
OUT  = os.path.join(REPO, "analysis", "befunde_runde5_2026-09-12", "gator_room_se")
os.makedirs(OUT, exist_ok=True)

# ---- note2pitch2 (EXAKT wie vab_common.c:320, LUT DAT_80077520) ----
LUT = [
 [0x1000,0x100E,0x101D,0x102C,0x103B,0x104A,0x1059,0x1068,0x1078,0x1087,0x1096,0x10A5,0x10B5,0x10C4,0x10D4,0x10E3],
 [0x10F3,0x1103,0x1113,0x1122,0x1132,0x1142,0x1152,0x1162,0x1172,0x1182,0x1193,0x11A3,0x11B3,0x11C4,0x11D4,0x11E5],
 [0x11F5,0x1206,0x1216,0x1227,0x1238,0x1249,0x125A,0x126B,0x127C,0x128D,0x129E,0x12AF,0x12C1,0x12D2,0x12E3,0x12F5],
 [0x1306,0x1318,0x132A,0x133C,0x134D,0x135F,0x1371,0x1383,0x1395,0x13A7,0x13BA,0x13CC,0x13DE,0x13F1,0x1403,0x1416],
 [0x1428,0x143B,0x144E,0x1460,0x1473,0x1486,0x1499,0x14AC,0x14BF,0x14D3,0x14E6,0x14F9,0x150D,0x1520,0x1534,0x1547],
 [0x155B,0x156F,0x1583,0x1597,0x15AB,0x15BF,0x15D3,0x15E7,0x15FB,0x1610,0x1624,0x1638,0x164D,0x1662,0x1676,0x168B],
 [0x16A0,0x16B5,0x16CA,0x16DF,0x16F4,0x170A,0x171F,0x1734,0x174A,0x175F,0x1775,0x178B,0x17A1,0x17B6,0x17CC,0x17E2],
 [0x17F9,0x180F,0x1825,0x183B,0x1852,0x1868,0x187F,0x1896,0x18AC,0x18C3,0x18DA,0x18F1,0x1908,0x191F,0x1937,0x194E],
 [0x1965,0x197D,0x1995,0x19AC,0x19C4,0x19DC,0x19F4,0x1A0C,0x1A24,0x1A3C,0x1A55,0x1A6D,0x1A85,0x1A9E,0x1AB7,0x1ACF],
 [0x1AE8,0x1B01,0x1B1A,0x1B33,0x1B4C,0x1B66,0x1B7F,0x1B98,0x1BB2,0x1BCC,0x1BE5,0x1BFF,0x1C19,0x1C33,0x1C4D,0x1C67],
 [0x1C82,0x1C9C,0x1CB7,0x1CD1,0x1CEC,0x1D07,0x1D22,0x1D3D,0x1D58,0x1D73,0x1D8E,0x1DA9,0x1DC5,0x1DE0,0x1DFC,0x1E18],
 [0x1E34,0x1E50,0x1E6C,0x1E88,0x1EA4,0x1EC1,0x1EDD,0x1EFA,0x1F16,0x1F33,0x1F50,0x1F6D,0x1F8A,0x1FA7,0x1FC5,0x1FE2]]
def note2pitch2(note, fine, center, shift):
    s = ((fine & 0xffff) + (shift & 0xff)) >> 3
    frac, carry = (s, 0) if s <= 15 else (s - 16, 1)
    sem = carry + note + 0x3c - center
    oct_ = int(sem / 12) - 5   # MIPS div: trunc Richtung 0
    rem = sem - 12 * int(sem / 12)
    idx = rem * 16 + frac
    idx = max(0, min(191, idx))
    p = LUT[idx // 16][idx % 16]
    if oct_ < 0: p >>= (-oct_) & 0x1f
    elif oct_ >= 1: p = 0x3FFF
    return p & 0xffff

F1=[0,60,115,98,122]; F2=[0,0,-52,-55,-60]
def adpcm(data):
    out=[]; h0=h1=0
    for b in range(0,len(data)-15,16):
        blk=data[b:b+16]; sh=blk[0]&0xF; fl=(blk[0]>>4)&0xF
        if fl>4: fl=0
        for i in range(14):
            by=blk[2+i]
            for nib in (by&0xF, by>>4):
                s=nib<<12
                if s&0x8000: s-=0x10000
                s>>=sh; s+=(h0*F1[fl]+h1*F2[fl]+32)>>6
                s=max(-32768,min(32767,s)); out.append(s); h1,h0=h0,s
        if blk[1]&1: break
    return out

def parse_vh(vh):
    if vh[:4] != b'pBAV':
        raise ValueError("VH-Magic unbekannt: " + vh[:8].hex())
    ps=struct.unpack_from('<H',vh,18)[0]; ts=struct.unpack_from('<H',vh,20)[0]
    vs=struct.unpack_from('<H',vh,22)[0]
    tone_base=0x20+128*16
    tones=[]
    for p in range(ps):
        for t in range(16):
            o=tone_base+p*0x200+t*0x20
            tones.append(dict(prog=p,tone=t,prior=vh[o],mode=vh[o+1],vol=vh[o+2],pan=vh[o+3],
                              center=vh[o+4],shift=vh[o+5],mn=vh[o+6],mx=vh[o+7],
                              vag=struct.unpack_from('<H',vh,o+22)[0]))
    st=tone_base+ps*0x200
    sizes=[struct.unpack_from('<H',vh,st+2*i)[0]*8 for i in range(256)]
    offs={}; run=0; idx=0
    for i,b in enumerate(sizes):
        if b==0: continue
        idx+=1  # 1-basierter VAG-Index in Reihenfolge
        offs[idx]=(run,b); run+=b
    return dict(ps=ps,ts=ts,vs=vs,tones=tones,offs=offs,total=run)

def zcr(pcm,rate):
    if len(pcm)<2: return 0
    z=sum(1 for i in range(1,len(pcm)) if (pcm[i-1]<0)!=(pcm[i]<0))
    return z*rate/(2*len(pcm))

def env_txt(pcm,rate):
    n=len(pcm)
    if n==0: return "leer",""
    win=max(1,int(rate*0.03)); rms=[]
    for i in range(0,n,win):
        seg=pcm[i:i+win]; rms.append(math.sqrt(sum(s*s for s in seg)/len(seg)))
    pk=max(rms) or 1; ch=" .:-=+*#%@"
    bar="".join(ch[min(9,int(r/pk*9.99))] for r in rms)
    pi=rms.index(max(rms)); att=pi*0.03
    dec=None
    for j in range(pi,len(rms)):
        if rms[j]<max(rms)*0.1: dec=(j-pi)*0.03; break
    loud=sum(1 for r in rms if r>max(rms)*0.3)/len(rms)
    return "dauer=%.2fs attack=%.2fs decay=%s loud30=%d%% zcr=%d" % (
        n/rate, att, ("%.2fs"%dec) if dec is not None else ">rest", int(loud*100), int(zcr(pcm,rate))), bar

def dump_room(room, banks=(0,1)):
    p=os.path.join(REPO,"re15_port","shared_assets","PSX","STAGE%s"%room[0],"ROOM%s.RDT"%room)
    d=open(p,'rb').read()
    offs=struct.unpack_from('<6I',d,8)
    ed=[offs[0],offs[3]]; vh_=[offs[1],offs[4]]; vb_=[offs[2],offs[5]]
    print("\n===== ROOM%s (%d B) snd0: edt=0x%X vh=0x%X vb=0x%X | snd1: edt=0x%X vh=0x%X vb=0x%X"
          % (room,len(d),ed[0],vh_[0],vb_[0],ed[1],vh_[1],vb_[1]))
    for b in banks:
        if not ed[b] or not vh_[b] or not vb_[b]:
            print("  snd%d: FEHLT"%b); continue
        edt=d[ed[b]:vh_[b]]
        try: V=parse_vh(d[vh_[b]:])
        except ValueError as e:
            print("  snd%d: %s"%(b,e)); continue
        print("  snd%d: EDT %d B (%d Records) VH ps=%d ts=%d vs=%d VB-Summe=%d B"
              % (b,len(edt),len(edt)//4,V['ps'],V['ts'],V['vs'],V['total']))
        for se in range(len(edt)//4):
            e=edt[se*4:se*4+4]
            if e[2]==0 and e[3]==0: continue
            prog=e[1]&0x7f; tone=e[2]>>4; prio=e[2]&0xf; voice=(e[3]&0x1f)-16; extra=e[3]>>5
            ovr=(e[0]&0x7f) if (e[0]&0x80) else -1
            for k in range(extra+1):
                ti=prog*16+tone+k
                if ti>=len(V['tones']): break
                t=V['tones'][ti]
                if t['vag']==0 or t['vag'] not in V['offs']:
                    print("    SE%2d L%d: rec=%s prog=%d tone=%d -> LEERER TONE"%(se,k,e.hex(),prog,tone+k)); break
                off,siz=V['offs'][t['vag']]
                vb=d[vb_[b]+off:vb_[b]+off+siz]
                pcm=adpcm(vb)
                pitch=note2pitch2(t['mn'],t['shift'],t['center'],t['shift'])
                rate=(44100*pitch)>>12
                info,bar=env_txt(pcm,rate)
                name="room%s_snd%d_se%02d%s_vag%d.wav"%(room,b,se,("_L%d"%k) if k else "",t['vag'])
                w=wave.open(os.path.join(OUT,name),'wb')
                w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate)
                w.writeframes(struct.pack('<%dh'%len(pcm),*pcm)); w.close()
                print("    SE%2d L%d: rec=%s voice=%d prio=%d prog=%d tone=%d vag=%d (%d B) pitch=0x%03X rate=%d vol=%d pan=%d ovr=%d"
                      % (se,k,e.hex(),voice,prio,prog,tone+k,t['vag'],siz,pitch,rate,t['vol'],t['pan'],ovr))
                print("           %s"%info)
                print("           |%s|"%bar)

if __name__ == "__main__":
    for r in sys.argv[1:] or ["2090"]:
        dump_room(r)
