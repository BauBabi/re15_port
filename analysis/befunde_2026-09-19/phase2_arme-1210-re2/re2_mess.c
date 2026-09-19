/* RE2 ROOM2050: wo sitzen die zehn Arm-Ursprünge im ORIGINAL-Hintergrund?
 * camera.rid (32 B/Eintrag, gleiche BIO15-Struktur), Cut 9 = Westgruppe (sub03 cut_chg 09),
 * Cut 10 = Ostgruppe (sub04 cut_chg 0a). Punkte werden ins PPM gemalt. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_camera.h"
static uint8_t *slurp(const char *p, size_t *n)
{ FILE *f=fopen(p,"rb"); if(!f) return NULL; fseek(f,0,SEEK_END); long s=ftell(f); fseek(f,0,SEEK_SET);
  uint8_t*b=malloc((size_t)s); if(fread(b,1,(size_t)s,f)!=(size_t)s){free(b);fclose(f);return NULL;} fclose(f); *n=(size_t)s; return b; }
static void va(const re15_camera_view_t*v,int32_t x,int32_t y,int32_t z,int32_t o[3])
{ o[0]=(int32_t)(((int64_t)v->rot[0]*x+(int64_t)v->rot[1]*y+(int64_t)v->rot[2]*z)>>12)+v->trans[0];
  o[1]=(int32_t)(((int64_t)v->rot[3]*x+(int64_t)v->rot[4]*y+(int64_t)v->rot[5]*z)>>12)+v->trans[1];
  o[2]=(int32_t)(((int64_t)v->rot[6]*x+(int64_t)v->rot[7]*y+(int64_t)v->rot[8]*z)>>12)+v->trans[2]; }
static int pr(const re15_camera_view_t*v,int32_t x,int32_t y,int32_t z,int*u,int*s)
{ int32_t c[3]; va(v,x,y,z,c); if(c[2]<=1) return 0;
  *u=160+(int)((int64_t)c[0]*v->fov_screen_dist/c[2]); *s=120+(int)((int64_t)c[1]*v->fov_screen_dist/c[2]); return 1; }
int main(int argc,char**argv)
{
    const char *rid = argv[1]; int cutid = atoi(argv[2]);
    size_t n=0; uint8_t *b=slurp(rid,&n);
    if(!b){printf("rid fehlt\n");return 1;}
    printf("%s: %d Eintraege\n", rid, (int)(n/32));
    const re15_camera_cut_t *cuts=(const re15_camera_cut_t*)b;
    for (int i=0;i<(int)(n/32);i++) printf("  cut %2d pos=(%d,%d,%d) tgt=(%d,%d,%d) fov=%u\n", i,
        cuts[i].pos_x,cuts[i].pos_y,cuts[i].pos_z,cuts[i].target_x,cuts[i].target_y,cuts[i].target_z,cuts[i].fov);
    re15_camera_view_t vw; if (re15_camera_build_view(&cuts[cutid],&vw)!=0){printf("degeneriert\n");return 1;}
    printf("Cut %d H=%d\n", cutid, vw.fov_screen_dist);
    /* Zehn RE2-Arm-Records ROOM2050 @0x1970..0x1A36 (Dossier 2.1) */
    int32_t P[10][3] = { {-27150,-2580,-12350},{-27200,-2430,-13820},{-27150,-2180,-12050},
                         {-27200,-1930,-13320},{-27130,-2200,-14000},
                         {-12100,-2480,-7000},{-12500,-2000,-7000},{-12440,-2700,-7000},
                         {-12230,-2540,-7000},{-12450,-2160,-7000} };
    int lo = (cutid==9)?0:5, hi = (cutid==9)?5:10;
    uint8_t *img=NULL; size_t is=0; uint8_t *px=NULL;
    if (argc>4) { img=slurp(argv[3],&is);
        if(img){ size_t o=2; int f=0; while(o<is&&f<3){ while(o<is&&(img[o]==' '||img[o]=='\n'))o++; while(o<is&&img[o]>' ')o++; f++; } o++; px=img+o; } }
    for (int i=lo;i<hi;i++){ int u,s;
        if(pr(&vw,P[i][0],P[i][1],P[i][2],&u,&s)) printf("  Arm %d (%d,%d,%d) -> Pixel (%d,%d)\n", i,P[i][0],P[i][1],P[i][2],u,s);
        else { printf("  Arm %d hinter der Kamera\n", i); continue; }
        if(px && u>=1 && u<319 && s>=1 && s<239)
            for(int dy=-1;dy<=1;dy++) for(int dx=-1;dx<=1;dx++){
                size_t k=((size_t)(s+dy)*320+(u+dx))*3; px[k]=255;px[k+1]=0;px[k+2]=0; }
    }
    /* Rueckprojektion beliebiger Pixel auf die Westwandebene x=-26852 bzw. Ostwand z=-7523 */
    for (int a=5;a+1<argc;a+=2){ if(argv[a][0]!='p') continue; int u=atoi(argv[a]+1),v=atoi(argv[a+1]);
        double cx=(double)(u-160)/vw.fov_screen_dist, cy=(double)(v-120)/vw.fov_screen_dist, cz=1.0;
        double d[3]={ (vw.rot[0]*cx+vw.rot[3]*cy+vw.rot[6]*cz)/4096.0,
                      (vw.rot[1]*cx+vw.rot[4]*cy+vw.rot[7]*cz)/4096.0,
                      (vw.rot[2]*cx+vw.rot[5]*cy+vw.rot[8]*cz)/4096.0 };
        double plane = (cutid==9)? -26852.0 : -7523.0;
        double t = (cutid==9) ? (plane-cuts[cutid].pos_x)/d[0] : (plane-cuts[cutid].pos_z)/d[2];
        printf("  Pixel (%d,%d) -> Ebene %s=%.0f bei y=%.0f, %s=%.0f\n", u,v, (cutid==9)?"x":"z", plane,
               cuts[cutid].pos_y+d[1]*t, (cutid==9)?"z":"x",
               (cutid==9)? cuts[cutid].pos_z+d[2]*t : cuts[cutid].pos_x+d[0]*t);
    }
    if(img && argc>4){ FILE*o=fopen(argv[4],"wb"); fwrite(img,1,is,o); fclose(o); printf("Bild: %s\n", argv[4]); }
    return 0;
}
