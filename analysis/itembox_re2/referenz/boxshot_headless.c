/* Rastert unsere Box-Bildschirm-Ops so, wie inv_render_pc.c es tut, und schreibt
 * ein PPM — damit sichtbar wird, was der Port wirklich zeichnet. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_inv_screen.h"
#include "re15_itembox.h"
#include "re15_inventory.h"

extern re15_inv_screen_t g_inv_screen;
static uint16_t fb[240][320];
static int blend_ch(int d5, int f8){ int v=(((d5<<3)|(d5>>2))+f8)>>4; return v>31?31:v; }
static void put(int x,int y,uint16_t v){ if((unsigned)x<320&&(unsigned)y<240) fb[y][x]=v; }

int main(void)
{
    static re15_inv_op_t ops[768];
    /* ein paar Gegenstaende in die Box legen, damit Zeilen Inhalt haben */
    re15_itembox_init();
    for (int i = 0; i < 6; i++) {
        g_itembox.slots[i].id = (uint8_t)(1 + i);
        g_itembox.slots[i].qty = (uint8_t)(1 + i);
    }
    re15_itembox_screen_open();
    for (int i=0;i<3;i++) re15_itembox_screen_tick(0,0);
    int n = re15_inv_screen_build(&g_inv_screen, ops, 768);
    fprintf(stderr, "ops=%d  box_side=%d  state=%d  scroll=%d\n", n,
            g_inv_screen.box_side, re15_itembox_screen_state(),
            re15_itembox_screen_scroll());
    for (int i=0;i<n;i++){
        const re15_inv_op_t *o=&ops[i];
        if (o->kind==RE15_INV_OP_FILL)
            fprintf(stderr, "  [%3d] FILL (%3d,%3d) %3dx%-3d rgb(%3d,%3d,%3d)%c",
                    i,o->x,o->y,o->w,o->h,o->r,o->g,o->b, 10);
    }
    memset(fb,0,sizeof fb);
    for (int i=n-1;i>=0;i--){            /* hinten -> vorn, wie der Rasterizer */
        const re15_inv_op_t *o=&ops[i];
        if (o->kind==RE15_INV_OP_FILL){
            uint16_t s=(uint16_t)((o->r>>3)|((o->g>>3)<<5)|((o->b>>3)<<10));
            for(int y=0;y<o->h;y++) for(int x=0;x<o->w;x++) put(o->x+x,o->y+y,s);
        } else if (o->kind==RE15_INV_OP_LINE){
            if(o->y!=o->h){ int y0=o->y,y1=o->h; if(y1<y0){int t=y0;y0=y1;y1=t;}
                for(int y=y0;y<=y1;y++){ if((unsigned)o->x>=320||(unsigned)y>=240)continue;
                    uint16_t d=fb[y][o->x];
                    fb[y][o->x]=(uint16_t)(blend_ch(d&31,o->r)|(blend_ch((d>>5)&31,o->g)<<5)|(blend_ch((d>>10)&31,o->b)<<10)); } }
            else { int x0=o->x,x1=o->w; if(x1<x0){int t=x0;x0=x1;x1=t;}
                for(int x=x0;x<=x1;x++){ if((unsigned)x>=320||(unsigned)o->y>=240)continue;
                    uint16_t d=fb[o->y][x];
                    fb[o->y][x]=(uint16_t)(blend_ch(d&31,o->r)|(blend_ch((d>>5)&31,o->g)<<5)|(blend_ch((d>>10)&31,o->b)<<10)); } }
        } else if (o->kind==RE15_INV_OP_SPRT){
            for(int y=0;y<o->h;y++) for(int x=0;x<o->w;x++) put(o->x+x,o->y+y,(uint16_t)(12|(12<<5)|(12<<10)));
        }
    }
    FILE *f=fopen("boxshot.ppm","wb");
    fprintf(f,"P6%c320 240%c255%c",10,10,10);
    for(int y=0;y<240;y++)for(int x=0;x<320;x++){
        uint16_t v=fb[y][x]; unsigned char p[3]={(unsigned char)((v&31)<<3),(unsigned char)(((v>>5)&31)<<3),(unsigned char)(((v>>10)&31)<<3)};
        fwrite(p,1,3,f);
    }
    fclose(f);
    return 0;
}
