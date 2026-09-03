#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_bss.h"
extern size_t g_vlc_src_used;
extern int g_vlc_bitcount;
extern int g_vlc_stop_after_mb;
extern int g_vlc_mb_done;

int main(int argc, char** argv){
    if (argc < 2) return 1;
    int mb = (argc>2)? atoi(argv[2]) : 300;
    FILE*f=fopen(argv[1],"rb"); if(!f){printf("ERR open\n");return 1;}
    static uint8_t buf[0x10000];
    size_t n=fread(buf,1,sizeof buf,f); fclose(f);
    re15_bss_chunk_t c;
    if(!re15_bss_parse_chunk(buf,n,&c)){printf("ERR parse\n");return 1;}
    size_t cap=((size_t)c.run_length_words+2)*4;
    int16_t*dst=malloc(cap*2);
    g_vlc_stop_after_mb = mb;
    int w=re15_bss_vlc_decode(c.vlc_payload,c.vlc_payload_size,c.run_length_words,c.quant,c.version,dst,cap);
    long bits = (long)g_vlc_src_used*8 + g_vlc_bitcount - 16;
    long endb = (bits+7)/8 + 8;   /* absolute byte offset in chunk after last VLC bit */
    printf("{\"rlw\":%u,\"quant\":%u,\"ver\":%u,\"wrote\":%d,\"mb\":%d,\"srcused\":%zu,\"bits\":%ld,\"vlcend\":%ld}\n",
        c.run_length_words,c.quant,c.version,w,g_vlc_mb_done,g_vlc_src_used,bits,endb);
    free(dst);
    return 0;
}
