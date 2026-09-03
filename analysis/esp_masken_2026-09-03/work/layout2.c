/* BSS-Chunk-Layout-Sonde: VLC-Ende (exakt in Bits) + SLD-Block + Trailer, je Chunk. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include "re15_bss.h"
extern size_t g_vlc_src_used;
extern int g_vlc_bitcount;
extern int g_vlc_stop_after_mb;
extern int g_vlc_mb_done;

static uint8_t buf[0x10000];
static uint8_t out[0x40000];

/* Port von FUN_800c47e8 (SLD/LZ). Rueckgabe: konsumierte Quell-Bytes, oder -1. */
static long sld_decomp(const uint8_t*src, size_t n, size_t sp, size_t size, uint8_t*dst, size_t limit)
{
    size_t dp=0, cap = (limit && limit<size)? limit : size;
    int flagMask=0; unsigned flagByte=0;
    while (dp < cap) {
        if (!flagMask) { if (sp>=n) return -1; flagByte=src[sp++]; flagMask=0x80; }
        if (sp>=n) return -1;
        unsigned a=src[sp++];
        if (a < 0x80) { if(dp>=size) return -1; dst[dp++]=(uint8_t)a; }
        else if (!(flagByte & flagMask)) { if(dp>=size) return -1; dst[dp++]=(uint8_t)a; flagMask>>=1; }
        else {
            if (sp>=n) return -1;
            unsigned b2=src[sp++];
            int raw=(int)((a<<4)|(b2>>4));
            int ln, off;
            if (b2 & 0x0F) { ln=(b2&0x0F)+2; off=(raw&0x800)? raw-0x1000 : raw; }
            else {
                if (sp>=n) return -1;
                unsigned b3=src[sp++];
                ln=(int)((b3&0x3F)+3);
                int s=(raw&0x800)? raw-0x1000 : raw;
                off=(s<<2)|(int)(b3>>6);
            }
            flagMask>>=1;
            long base=(long)dp+off;
            if (base<0) return -1;
            if (dp+(size_t)ln > size) return -1;
            for(int k=0;k<ln;k++) dst[dp+k]=dst[base+k];
            dp+=(size_t)ln;
        }
    }
    return (long)sp;
}

/* Ist an Offset o ein gueltiger SLD->TIM-Block? gibt entpackte Groesse zurueck, sonst 0 */
static uint32_t sld_probe(const uint8_t*d, size_t o, long*consumed_end)
{
    if (o+4 >= 0x10000) return 0;
    uint32_t sz; memcpy(&sz, d+o, 4);
    if (sz < 0x2000 || sz > 0x40000) return 0;
    long e = sld_decomp(d, 0x10000, o+4, sz, out, 16);
    if (e < 0) return 0;
    if (!(out[0]==0x10 && out[1]==0 && out[2]==0 && out[3]==0 && (out[4]&7)<=3)) return 0;
    e = sld_decomp(d, 0x10000, o+4, sz, out, 0);
    if (e < 0) return 0;
    if (consumed_end) *consumed_end = e;
    return sz;
}

int main(int argc, char**argv)
{
    for (int ai=1; ai<argc; ai++) {
        const char*path=argv[ai];
        FILE*f=fopen(path,"rb");
        if(!f){ printf("{\"f\":\"%s\",\"err\":\"open\"}\n",path); continue; }
        fseek(f,0,SEEK_END); long fsz=ftell(f); fseek(f,0,SEEK_SET);
        long nch = fsz / 0x10000;
        if (nch == 0) { printf("{\"f\":\"%s\",\"err\":\"tiny\",\"fsz\":%ld}\n",path,fsz); fclose(f); continue; }
        for (long ci=0; ci<nch; ci++) {
            fseek(f, ci*0x10000, SEEK_SET);
            size_t n=fread(buf,1,0x10000,f);
            if (n != 0x10000) break;
            re15_bss_chunk_t c;
            int ok = re15_bss_parse_chunk(buf,n,&c);
            long vlcend=-1, bits=-1; int wrote=-1;
            if (ok && c.id==RE15_BSS_VLC_ID) {
                size_t cap=((size_t)c.run_length_words+2)*4;
                int16_t*dst=(int16_t*)malloc(cap*2);
                g_vlc_stop_after_mb=300;
                wrote=re15_bss_vlc_decode(c.vlc_payload,c.vlc_payload_size,c.run_length_words,c.quant,c.version,dst,cap);
                bits=(long)g_vlc_src_used*8 + g_vlc_bitcount - 16;
                vlcend=(bits+7)/8 + 8;
                free(dst);
            }
            long pred = (vlcend>=0)? (vlcend/4)*4+4 : -1;
            long sldend=-1;
            uint32_t sz_pred = (pred>=0)? sld_probe(buf,(size_t)pred,&sldend) : 0;
            long scan_off=-1; uint32_t scan_sz=0; long scan_end=-1;
            if (!sz_pred) {
                for (long o=8; o+8<0x10000; o+=4) {
                    long e; uint32_t z=sld_probe(buf,(size_t)o,&e);
                    if (z) { scan_off=o; scan_sz=z; scan_end=e; break; }
                }
            }
            long trailerP=-1; long trailerFlag=-1;
            long refoff = sz_pred ? pred : scan_off;
            if (refoff>=0) {
                for (long P=8;P<=0x10000;P+=4) {
                    uint32_t v; memcpy(&v,buf+P-8,4);
                    if ((long)v==refoff) { uint32_t fl; memcpy(&fl,buf+P-4,4); trailerP=P; trailerFlag=(long)fl; break; }
                }
            }
            unsigned char pb[16]; memset(pb,0,16);
            long pbase = (vlcend>=0)? (vlcend/4)*4 : 0;
            if (pbase+16<0x10000) memcpy(pb, buf+pbase, 16);
            printf("{\"f\":\"%s\",\"c\":%ld,\"id\":%u,\"rlw\":%u,\"q\":%u,\"ver\":%u,\"wrote\":%d,\"bits\":%ld,\"vlcend\":%ld,\"pred\":%ld,\"sldsize\":%u,\"sldend\":%ld,\"scanoff\":%ld,\"scansz\":%u,\"scanend\":%ld,\"P\":%ld,\"flag\":%ld,\"post\":\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"}\n",
                   path, ci, ok?c.id:0, ok?c.run_length_words:0, ok?c.quant:0, ok?c.version:0, wrote,
                   bits, vlcend, pred, sz_pred, sldend, scan_off, scan_sz, scan_end, trailerP, trailerFlag,
                   pb[0],pb[1],pb[2],pb[3],pb[4],pb[5],pb[6],pb[7],pb[8],pb[9],pb[10],pb[11],pb[12],pb[13],pb[14],pb[15]);
        }
        fclose(f);
    }
    return 0;
}
