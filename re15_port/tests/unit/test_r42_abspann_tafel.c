/* test_r42_abspann_tafel.c — Runde 42 Strang finale, Stufe 7: die Abspann-Diashow ROOM6040 (Leon) / ROOM6041 (Elza) zeigt die Tafel
 * "Thank you for playing this reconstruction of Resident Evil 1.5 ..." nicht mehr (NUTZERWUNSCH 18.11.2026 "das thank you for Playing
 * fliegt vollständig raus"; Kopf include/re15_endvideo_r42.h "STUFE 7").
 *
 * Geprueft je Raum mit der echten RDT und dem echten SCD-Takt (Raumstart wie test_r42_finale_abspann.c, sub00 @0x00522 `04 ff 18 02`
 * startet sub02):
 *   - Die Soll-Abfolge wird aus den Original-Bytes von sub02 @0x0052A..@0x00597 der RDT selbst gelesen (Cut_chg `29 nn`, Sleep
 *     `09 0a <n16>`, Set(0,1,0) `22 00 01 00`). Die RDT muss das Original sein: Cut 1 mit 750 Bildern (@0x0053A + 3 x 250) und
 *     zusammen 3952 Bilder - sonst ist der Test ungueltig (rot).
 *   - Gemessen wird je Bild der angezeigte Cut (work_vars[0x0A], @0x800402fc) und das Bild, in dem sub02 endet (Sleep 2 @0x0058C,
 *     Set(0,1,0) @0x00590, Evt_end @0x00594 im selben Takt).
 *   - Soll: Cut 1 erscheint nie. Alle uebrigen Cuts 0, 2..8 erscheinen in der Original-Reihenfolge mit der Original-Standzeit (Summe
 *     ihrer Sleeps), Set(0,1,0) faellt 3952 - 750 = 3202 Bilder nach dem ersten Cut.
 * Gegenprobe: auf aaea5f8c2 (und jedem Stand vor Stufe 7) zeigt die Diashow Cut 1 750 Bilder lang -> rot.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_collision.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

#define SUB02_OFF   0x0052Au   /* sub02 in ROOM6040/6041 (tools/scd_dump_room.py, xxd)          */
#define SUB02_LEN   110u       /* @0x0052A..@0x00597                                           */
#define CUT_TAFEL1  1u         /* @0x0053A `29 01` = Tafel "Thank you for playing this reconstruction" */
#define MAX_CUTS    16

static re15_rdt_t s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t s_ctx;
static uint8_t *s_raw;
static size_t s_raw_n;

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

typedef struct { int cut; int bilder; } abschnitt_t;

/* Soll aus den Original-Bytes: Cut_chg beginnt einen Abschnitt, Sleeps summieren seine Standzeit. */
static int soll_lesen(const uint8_t *p, abschnitt_t *a, int *gesamt)
{
    int n = -1; unsigned i = 0;
    *gesamt = 0;
    while (i < SUB02_LEN) {
        const uint8_t op = p[i];
        if (op == 0x29) { if (++n >= MAX_CUTS) return -1; a[n].cut = p[i + 1]; a[n].bilder = 0; i += 2; }
        else if (op == 0x09) { const int s = p[i + 2] | (p[i + 3] << 8); if (n >= 0) a[n].bilder += s; *gesamt += s; i += 4; }
        else if (op == 0x54) i += 6;
        else if (op == 0x22) i += 4;
        else if (op == 0x01) break;
        else return -1;
    }
    return n + 1;
}

/* Laeuft noch ein Ereignis-Faden (sub02 liegt im automatisch vergebenen Slot 10..23, cond 0xFF @0x00523)? Diese Abfrage gibt es
 * auf jedem Stand (auch aaea5f8c2), anders als der INGAME-Haken re15_gameflow_ingame_aus_faellig (erst ab Plan S8). */
static int ereignis_laeuft(void)
{
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) if (g_scd.threads[s].active) return 1;
    return 0;
}

static void bild(void)
{
    scd_vm_tick();
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

static void lauf(const char *rel, unsigned id)
{
    char pfad[512];
    snprintf(pfad, sizeof pfad, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    free(s_raw); s_raw_n = 0;
    s_raw = slurp(pfad, &s_raw_n);
    if (!s_raw || s_raw_n < SUB02_OFF + SUB02_LEN || re15_rdt_parse(s_raw, s_raw_n, &s_rdt) != 0) {
        CHECK(0, "%s nicht lesbar", rel); return;
    }
    /* Soll aus der RDT (Kopie, bevor irgendetwas laeuft) */
    abschnitt_t soll[MAX_CUTS]; int soll_gesamt = 0;
    const int n_soll = soll_lesen(s_raw + SUB02_OFF, soll, &soll_gesamt);
    int tafel1_bilder = -1;
    for (int k = 0; k < n_soll; k++) if ((unsigned)soll[k].cut == CUT_TAFEL1) tafel1_bilder = soll[k].bilder;
    CHECK(n_soll == 9 && tafel1_bilder == 750 && soll_gesamt == 3952,
          "%04X: sub02 @0x%05X ist nicht das Original (Cuts %d, Tafel 1 %d Bilder, gesamt %d; soll 9 / 750 / 3952)", id, SUB02_OFF,
          n_soll, tafel1_bilder, soll_gesamt);
    if (n_soll != 9 || tafel1_bilder != 750) return;

    /* Raum aufbauen wie test_r42_finale_abspann.c */
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    g_scd.work_vars[0x0A] = 0x7F;                                     /* Markwert: noch kein Cut_chg */
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);

    /* messen */
    abschnitt_t ist[MAX_CUTS]; int n_ist = 0;
    int start = -2, ende = -1, tafel1 = 0, cut_vor = -1, begonnen = 0;   /* -2 = noch kein Cut */
    /* Bild -1 = der Raumstart selbst: scd_room_reenter fuehrt den ersten SCD-Takt schon aus (sub00 -> sub02 bis Sleep 200), der
     * erste Cut steht also VOR der Schleife (gemessen: ohne diesen Schritt fehlte Cut 0 genau 1 Bild). */
    for (int f = -1; f < 4400 && ende < 0; f++) {
        if (begonnen) bild();
        begonnen = 1;
        const int cut = (int)(uint8_t)g_scd.work_vars[0x0A];
        if (cut != 0x7F && cut != cut_vor) {
            if (start == -2) start = f;
            if (n_ist > 0) ist[n_ist - 1].bilder = f - ist[n_ist - 1].bilder;   /* Standzeit des vorigen */
            if (n_ist < MAX_CUTS) { ist[n_ist].cut = cut; ist[n_ist].bilder = f; n_ist++; }
            cut_vor = cut;
        }
        if ((unsigned)cut == CUT_TAFEL1) tafel1++;
        if (start != -2 && !ereignis_laeuft()) ende = f;   /* sub02 ist durch: Sleep 2 @0x0058C, Set(0,1,0) @0x00590 und Evt_end
                                                             * @0x00594 laufen im selben Takt */
    }
    if (n_ist > 0 && ende >= 0) ist[n_ist - 1].bilder = ende - ist[n_ist - 1].bilder;   /* letzter Cut bis Set(0,1,0) */

    /* Soll ohne Tafel 1 */
    abschnitt_t ziel[MAX_CUTS]; int n_ziel = 0, ziel_gesamt = 0;
    for (int k = 0; k < n_soll; k++) if ((unsigned)soll[k].cut != CUT_TAFEL1) { ziel[n_ziel++] = soll[k]; ziel_gesamt += soll[k].bilder; }

    CHECK(tafel1 == 0, "%04X: Tafel 1 (Cut 1 @0x0053A, \"Thank you for playing this reconstruction\") %d Bilder lang gezeigt", id, tafel1);
    CHECK(ende >= 0, "%04X: Set(0,1,0) @0x00590 (Diashow-Ende) nicht erreicht", id);
    CHECK(n_ist == n_ziel, "%04X: %d Cuts gezeigt, soll %d (Original ohne Tafel 1)", id, n_ist, n_ziel);
    /* Die Standzeit des letzten Cuts schliesst Sleep 2 @0x0058C ein (Set(0,1,0) danach); im Soll liegt Sleep 2 ebenfalls beim
     * letzten Cut (soll_lesen addiert jeden Sleep zum laufenden Abschnitt). Messung und Soll zaehlen also gleich. */
    for (int k = 0; k < n_ist && k < n_ziel; k++) {
        CHECK(ist[k].cut == ziel[k].cut, "%04X: Stelle %d zeigt Cut %d, soll Cut %d (Original-Reihenfolge)", id, k, ist[k].cut, ziel[k].cut);
        CHECK(ist[k].bilder == ziel[k].bilder, "%04X: Cut %d steht %d Bilder, soll %d (Summe der Original-Sleeps)", id, ist[k].cut,
              ist[k].bilder, ziel[k].bilder);
    }
    CHECK(start != -2 && ende >= 0 && ende - start == ziel_gesamt, "%04X: Diashow %d Bilder (Bild %d..%d), soll %d = 3952 - 750", id,
          ende - start, start, ende, ziel_gesamt);

    fprintf(stderr, "%04X: Cuts", id);
    for (int k = 0; k < n_ist; k++) fprintf(stderr, " %d(%d)", ist[k].cut, ist[k].bilder);
    fprintf(stderr, "; Tafel 1 %d Bilder; Set(0,1,0) Bild %d, Diashow %d Bilder (Soll %d)\n", tafel1, ende, ende - start, ziel_gesamt);
}

int main(void)
{
    lauf("STAGE6/ROOM6040.RDT", 0x6040u);
    lauf("STAGE6/ROOM6041.RDT", 0x6041u);
    free(s_raw);
    if (fails) { fprintf(stderr, "test_r42_abspann_tafel: %d FAIL\n", fails); return 1; }
    fprintf(stderr, "test_r42_abspann_tafel: OK\n");
    return 0;
}
