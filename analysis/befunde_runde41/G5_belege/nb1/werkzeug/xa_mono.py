"""xa_mono.py - VOICE07.XAS (2336-B-Sektoren, 8 Kanaele in 16er-Zeilen) -> mono 37800-Hz-WAV eines Xa-Eintrags.
Fenster wie RE2 FUN_800129b4: Start = blk*16+ch (@0x80012af4..), Ende = -0x1E + (blk+len)*16 + ch; gespielt werden die
Sektoren des Kanals mit Lage < Ende (Abbruch @0x80012e3c sltu). Dekodierung psx-spx CD-XA ADPCM (4 Bit mono: 8 Einheiten je Gruppe)."""
import sys, struct, wave
K0=[0,60,115,98]; K1=[0,0,-52,-55]
def dec_sector(sec, hist):
    data=sec[8:8+0x900]
    out=[]
    for g in range(18):
        grp=data[g*128:(g+1)*128]
        for u in range(8):
            p=grp[4+u]; sh=p&0x0f; f=(p>>4)&3
            if sh>12: sh=9
            for j in range(28):
                byte=grp[16+j*4+(u>>1)]
                nib=(byte>>4) if (u&1) else (byte&0xf)
                t=nib-16 if nib>=8 else nib
                s=(t<<12)>>sh
                s+= (hist[0]*K0[f]+hist[1]*K1[f]+32)>>6
                s=max(-32768,min(32767,s))
                hist[1]=hist[0]; hist[0]=s
                out.append(s)
    return out
def clip(xas, ch, blk, ln, full=False):
    b=open(xas,'rb').read(); n=len(b)//2336
    start=blk*16+ch; end=-0x1e+(blk+ln)*16+ch
    hist=[0,0]; pcm=[]; secs=[]
    i=start
    while i<n and (full or i<end):
        sec=b[i*2336:(i+1)*2336]
        assert sec[1]==ch and sec[0]==1, (i,sec[:4])
        if sec[2]&0x04:   # audio
            assert sec[3]==0, 'coding %02x'%sec[3]
            pcm+=dec_sector(sec,hist); secs.append(i)
        if sec[2]&0x80 and full: break
        i+=16
    return pcm, secs, start, end
if __name__=='__main__':
    xas,ch,blk,ln,out=sys.argv[1],int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4]),sys.argv[5]
    pcm,secs,st,en=clip(xas,ch,blk,ln)
    w=wave.open(out,'wb'); w.setnchannels(1); w.setsampwidth(2); w.setframerate(37800)
    w.writeframes(struct.pack('<%dh'%len(pcm),*pcm)); w.close()
    print(out,'sectors',len(secs),'first',secs[0],'last',secs[-1],'window',st,en,'samples',len(pcm),'dur %.4f s'%(len(pcm)/37800), 'read %.4f s'%((en-st)/150))
