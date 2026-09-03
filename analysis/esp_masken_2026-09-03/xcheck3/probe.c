#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_bss.h"
extern long g_bits_at_limit, g_final_bits; extern int g_blocks, g_block_limit;

int main(int argc,char**argv){
    const char*path=argv[1]; int cut=atoi(argv[2]); g_block_limit=atoi(argv[3]);
    FILE*f=fopen(path,"rb"); if(!f){printf("ERR open\n");return 1;}
    fseek(f,(long)cut*65536,SEEK_SET);
    static uint8_t ch[65536];
    if(fread(ch,1,65536,f)!=65536){printf("ERR read\n");return 1;} fclose(f);
    unsigned rlw = ch[0]|(ch[1]<<8), id=ch[2]|(ch[3]<<8), q=ch[4]|(ch[5]<<8), ver=ch[6]|(ch[7]<<8);
    size_t cap=((size_t)rlw+2)*4;
    int16_t*co=(int16_t*)malloc(cap*2);
    int w=re15_bss_vlc_decode(ch+8,65536-8,(int)rlw,(int)q,(int)ver,co,cap);
    printf("%s cut=%d rlw=%u id=%x q=%u ver=%u written=%d blocks=%d B_at_limit=%ld B_final=%ld\n",
           path,cut,rlw,id,q,ver,w,g_blocks,g_bits_at_limit,g_final_bits);
    return 0;
}
