#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_bss.h"
extern long g_bits_at_limit; extern int g_blocks, g_block_limit;
static uint8_t ch[65536];
int main(int argc,char**argv){
    g_block_limit=1800;
    FILE*out=fopen(argv[1],"w");
    const char*stages[6]={"1","2","3","4","5","6"};
    int counts[6]={40,16,16,16,24,8};
    char path[512];
    for(int st=0;st<6;st++) for(int r=0;r<counts[st];r++){
        sprintf(path,"info/Re1.5/PSX/STAGE%s/ROOM%X%02X.BSS",stages[st],st+1,r);
        FILE*f=fopen(path,"rb"); if(!f) continue;
        fseek(f,0,SEEK_END); long fsz=ftell(f); fseek(f,0,SEEK_SET);
        int n=(int)(fsz/65536);
        for(int c=0;c<n;c++){
            fseek(f,(long)c*65536,SEEK_SET);
            if(fread(ch,1,65536,f)!=65536) break;
            unsigned rlw=ch[0]|(ch[1]<<8),id=ch[2]|(ch[3]<<8),q=ch[4]|(ch[5]<<8),ver=ch[6]|(ch[7]<<8);
            size_t cap=((size_t)rlw+2)*4;
            int16_t*co=(int16_t*)malloc(cap*2);
            int w=re15_bss_vlc_decode(ch+8,65536-8,(int)rlw,(int)q,(int)ver,co,cap);
            fprintf(out,"%d %d %u %u %u %u %d %d %ld\n",st+1,r,c,rlw,q,ver,w,g_blocks,g_bits_at_limit);
            free(co);
        }
        fclose(f);
    }
    fclose(out); return 0;
}
