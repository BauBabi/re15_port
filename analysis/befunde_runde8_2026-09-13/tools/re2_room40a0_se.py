#!/usr/bin/env python3
"""RE2 ROOM40A0 Raum-SE-Bank (RDT sec0=EDT, sec1=VH, sec2=VB) dumpen + als WAV rendern."""
import struct, os, sys, math, wave
sys.path.insert(0, "c:/workspace/git/reAi_v2/analysis/befunde_runde5_2026-09-12/tools")
import room_banks as RB
REPO="c:/workspace/git/reAi_v2"
OUT=os.path.join(REPO,"analysis","befunde_runde8_2026-09-13","re2_room40a0_se")
os.makedirs(OUT,exist_ok=True)
d=open(REPO+"/info/re2leon/PL0/RDT/ROOM40A0.RDT","rb").read()
offs=[struct.unpack_from("<I",d,8+4*i)[0] for i in range(23)]
e_o,vh_o,vb_o=offs[0],offs[1],offs[2]
edt=d[e_o:vh_o]
V=RB.parse_vh(d[vh_o:])
print("ROOM40A0: EDT @0x%X (%d B, %d Rec) VH @0x%X ps=%d ts=%d vs=%d VB @0x%X Summe=%d"%(
    e_o,len(edt),len(edt)//4,vh_o,V['ps'],V['ts'],V['vs'],vb_o,V['total']))
for se in range(len(edt)//4):
    e=edt[se*4:se*4+4]
    if e[2]==0xff or (e[2]==0 and e[3]==0): continue
    prog=e[1]&0x7f; tone=e[2]>>4; prio=e[2]&0xf; voice=e[3]&0x1f; extra=e[3]>>5
    for k in range(extra+1):
        ti=prog*16+tone+k
        if ti>=len(V['tones']): break
        t=V['tones'][ti]
        if t['vag']==0 or t['vag'] not in V['offs']:
            print("  SE%2d L%d rec=%s prog=%d tone=%d -> LEER"%(se,k,e.hex(),prog,tone+k)); break
        off,siz=V['offs'][t['vag']]
        pcm=RB.adpcm(d[vb_o+off:vb_o+off+siz])
        pitch=RB.note2pitch2(t['mn'],t['shift'],t['center'],t['shift'])
        rate=(44100*pitch)>>12
        info,bar=RB.env_txt(pcm,rate)
        nm="room40A0_se%02d%s_vag%d.wav"%(se,("_L%d"%k) if k else "",t['vag'])
        w=wave.open(os.path.join(OUT,nm),'wb'); w.setnchannels(1); w.setsampwidth(2)
        w.setframerate(max(1,rate)); w.writeframes(struct.pack('<%dh'%len(pcm),*pcm)); w.close()
        print("  SE%2d L%d rec=%s voice=%d prio=%d prog=%d tone=%d vag=%d (%d B) pitch=0x%03X rate=%d vol=%d"%(
            se,k,e.hex(),voice,prio,prog,tone+k,t['vag'],siz,pitch,rate,t['vol']))
        print("        %s"%info); print("        |%s|"%bar)
