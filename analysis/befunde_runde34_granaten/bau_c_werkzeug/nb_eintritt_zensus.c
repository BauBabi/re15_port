/* Uebersetzen (Git-Bash, gegen das Bauverzeichnis des Arbeitsbaums; KEIN Projekt-Bau):
 *   WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c/re15_port
 *   PATH=/c/msys64/mingw64/bin:$PATH gcc -std=c11 -O1 -DRE15_PLATFORM_PC=1 -I$WT/include <datei>.c \
 *     -L$WT/build_r34_c/tests -L$WT/build_r34_c/engine -lre15_test_support -lre15_engine -lre15_test_support -lm \
 *     -o <datei>.exe
 */
/* Mess-Werkzeug (Scratchpad, Nachbesserung M1 Spur C): welche Raeume spawnen im EINTRITTSBILD
 * (scd_room_reenter = der erste SCD-Takt in re15_room_apply_pending) ESP-Effekte, und was wird
 * daraus
 *   (A) in der Port-Reihenfolge des Tuer-/Sprungwegs: Raumbank = NULL (re15_room_reset_render_pc
 *       setzt sie zurueck, pc_load_room_esp laeuft erst NACH apply_pending, main.c:7891)
 *   (B) in der Reihenfolge des Originals / des Lade-Wegs: Raumbank VOR dem SCD geparst
 *       (FUN_800396fc -> FUN_80019354 = Clear + Parse im Raumlader; main.c:3726 beim Boot).
 * Ausgabe je Raum: Anzahl Plaetze (A: ohne Zeilen/ohne Bank = verloren) und (B) je Effekt-Id die
 * Plaetze + Routine A der Zeile 0. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_esp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t *slurp(const char *p, size_t *n){FILE*f=fopen(p,"rb");if(!f)return NULL;fseek(f,0,SEEK_END);long s=ftell(f);fseek(f,0,SEEK_SET);if(s<=0){fclose(f);return NULL;}uint8_t*b=malloc((size_t)s);fread(b,1,(size_t)s,f);fclose(f);*n=(size_t)s;return b;}
static uint32_t u32(const uint8_t*p){return p[0]|(p[1]<<8)|(p[2]<<16)|((uint32_t)p[3]<<24);}
static void eintritt(const re15_rdt_t *rdt, unsigned rid){
  scd_vm_init();
  g_current_room_id = (int)rid;
  re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
  pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
  scd_register_room_events(rdt);
  scd_room_reenter(rdt, 0, 0, 0);
}
int main(int argc, char **argv){
  const char *A = argv[1];
  char p[600]; size_t nc = 0;
  snprintf(p, sizeof p, "%s/DATA/CORE00.ESP", A); uint8_t *core = slurp(p, &nc);
  static re15_esp_t g; if (!core || re15_esp_parse_global(core, nc, &g) != 0) { printf("FAIL CORE00\n"); return 1; }
  re15_esp_set_global_bank(&g);
  int raeume = 0, mit = 0, verloren_ges = 0;
  for (int st = 1; st <= 7; st++) {
    for (unsigned rid = 0x1000u * st; rid < 0x1000u * st + 0x300u; rid++) {
      snprintf(p, sizeof p, "%s/STAGE%d/ROOM%04X.RDT", A, st, rid);
      size_t n = 0; uint8_t *d = slurp(p, &n);
      if (!d) continue;
      if (n < 0x60) { free(d); continue; }
      static re15_rdt_t rdt; if (re15_rdt_parse(d, n, &rdt) != 0) { free(d); continue; }
      raeume++;
      /* (A) Port-Tuerweg: Bank NULL */
      re15_esp_fx_reset(); re15_esp_set_room_bank(NULL);
      eintritt(&rdt, rid);
      int a_n = 0, a_lost = 0;
      for (int i = 0; i < RE15_ESP_FX_MAX; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i); if (!f) continue; a_n++; if (!f->rows_base) a_lost++; }
      /* (B) Original-/Ladeweg: Bank vorher geparst */
      static re15_esp_t r; memset(&r, 0, sizeof r);
      int rc = re15_esp_parse(d, n, u32(d+0x4c), u32(d+0x50), u32(d+0x54), u32(d+0x58), &r);
      re15_esp_fx_reset(); re15_esp_set_room_bank(rc == 0 ? &r : NULL);
      eintritt(&rdt, rid);
      int b_n = 0; char ids[512]; ids[0] = 0; int cnt[256]; unsigned selA[256]; memset(cnt, 0, sizeof cnt);
      for (int i = 0; i < RE15_ESP_FX_MAX; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i); if (!f) continue; b_n++;
        int id = f->effect_id & 0xff; if (!cnt[id]) selA[id] = f->rows_base ? (unsigned)(f->row[0] | (f->row[1] << 8)) : 999u; cnt[id]++; }
      for (int id = 0; id < 256; id++) if (cnt[id]) { char t[48]; snprintf(t, sizeof t, " id%02x x%d(A0=%u)", id, cnt[id], selA[id]); strncat(ids, t, sizeof ids - strlen(ids) - 1); }
      if (a_n || b_n) {
        mit++; verloren_ges += a_lost;
        printf("ROOM%04X  Tuerweg: %2d Plaetze, %2d ohne Zeilen/Bank | Ladeweg: %2d Plaetze:%s\n", rid, a_n, a_lost, b_n, ids);
      }
      free(d);
    }
  }
  printf("Raeume %d, mit Eintritts-Effekt %d, auf dem Tuerweg ohne Zeilen/Bank gespawnt: %d Plaetze\n", raeume, mit, verloren_ges);
  return 0;
}
