/* Mess-Werkzeug (Scratchpad, Nachbesserung M1 Spur C): welche Cuts von ROOM<raum> decken die
 * Weltpunkte (x,z)? Region-Quad je Cut (re15_rdt_get_region_quad = FUN_80014324-Satz, RDT+0x28)
 * und derselbe Test wie der Effekt-Zeichner (re15_esp_fx_culled, FUN_80014368 @0x80053334). */
#include "re15_rdt.h"
#include "re15_esp.h"
#include "re15_aot.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned char *slurp(const char *p, size_t *n){FILE*f=fopen(p,"rb");if(!f)return NULL;fseek(f,0,SEEK_END);long s=ftell(f);fseek(f,0,SEEK_SET);unsigned char*b=malloc(s);fread(b,1,s,f);fclose(f);*n=(size_t)s;return b;}
int main(int argc, char **argv){
  size_t n=0; unsigned char *b = slurp(argv[1], &n);
  if(!b){printf("FAIL lesen\n");return 1;}
  static re15_rdt_t r; if (re15_rdt_parse(b, n, &r) != 0) { printf("FAIL parse\n"); return 1; }
  printf("nCut=%d\n", r.nCut);
  for (int c = 0; c < r.nCut; c++) {
    int16_t xs[4], zs[4];
    int h = re15_rdt_get_region_quad(&r, c, xs, zs);
    printf("cut %2d region=%d quad (%d,%d) (%d,%d) (%d,%d) (%d,%d) |", c, h, xs[0],zs[0],xs[1],zs[1],xs[2],zs[2],xs[3],zs[3]);
    for (int a = 2; a + 1 < argc; a += 2) {
      int x = atoi(argv[a]), z = atoi(argv[a+1]);
      printf(" (%d,%d):%s", x, z, re15_esp_fx_culled(x, z, h, xs, zs) ? "gecullt" : "SICHTBAR");
    }
    printf("\n");
  }
  return 0;
}
