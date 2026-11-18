/* test_r42_finale_abspann.c — Runde 42 Strang finale, Plan S7 (Pruefbericht s1 r1: "Riegel fehlen: ohne Bit 129 bleiben Tuer 5 und
 * sub02 gesperrt").
 *
 * ROOM5090/5091 fuehren den Abspann-Vorspann sub02 (5090 @0x012C2: Set(2,7,1), Set(1,27,1) @0x012C6, Sleep 20, Message_on 0 "THANK YOU
 * FOR PLAYING !!!", Sleep 100, Aot_on(5) @0x012D6 -> Tuer Slot 5 -> ROOM6040/6041). Gestartet wird er von sub01 bei Ck(5,0x1C)
 * (5090 @0x012A6 -> Evt_exec sub02 @0x012BA). Im Port sind beide Wege gesperrt, bis das Finale beendet ist (Bank 9 Bit 129
 * RE15_R42_BIT_FINALE_ENDE, gesetzt im Kill-Bild finale_r42.c = RE2 FUN_80018fb0 @0x80103d7c): Streichliste op_evt_exec (scd_vm.c) und
 * Tuer-Riegel aot_fire_door (aot_common.c). Geprueft je Raum, echte RDT, echter SCD-Takt:
 *   ohne Bit 129: (5,0x1C) gesetzt -> 200 Bilder lang kein Kinobalken (1,27) und kein Raumwechsel; Tuer 5 direkt gefeuert -> kein
 *                 Raumwechsel.
 *   mit Bit 129:  derselbe Ablauf -> Kinobalken an, nach Sleep 20 + 100 (@0x012CA/@0x012D2) Raumwechsel nach 0x6040 bzw. 0x6041.
 * Gegenprobe: auf aaea5f8c2 (vor der Bindung an Bit 129) fehlt das Bit-Symbol, der Riegel laesst sich dort nicht bauen; die alte
 * Sperre war bedingungslos (sub02 immer gestrichen) -> der Teil "mit Bit 129" waere dort rot.
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
#include "re15_story_r42.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static re15_rdt_t s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t s_ctx;
static uint8_t *s_raw;

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

static void bild(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

/* Raum aufbauen wie probe_r18 (aaea5f8c2): RDT, Akteure, AOT, SCD, Raumereignisse, Wiedereintritt auf Schnitt 12. Kampf als gelaufen
 * markiert (3,42), damit sub01 nicht sub04 (Kampfstart) startet. */
static int raum(const char *rel, unsigned id, int bit129)
{
    size_t n = 0;
    char pfad[512];
    snprintf(pfad, sizeof pfad, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    free(s_raw);
    s_raw = slurp(pfad, &n);
    if (!s_raw || re15_rdt_parse(s_raw, n, &s_rdt) != 0) { CHECK(0, "%s nicht lesbar", rel); return 0; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 12;
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 8000; pl->y = 0; pl->z = -23500; pl->rot_y = 2048;
    re15_collision_set_band(0);
    re15_game_flag_set(3, 42, 1);                                     /* Kampf gelaufen (sub01 @0x01284 Ck(3,42)=0 -> sub04) */
    re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE, bit129);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 12);
    re15_game_flag_set(3, 42, 1);
    re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE, bit129);
    for (int f = 0; f < 5; f++) bild();
    return 1;
}

static void lauf(const char *rel, unsigned id, unsigned ziel)
{
    for (int bit = 0; bit <= 1; bit++) {
        if (!raum(rel, id, bit)) return;
        re15_game_flag_set(5, 0x1c, 1);                               /* Abspann-Ausloeser (5,0x1C), wie das Kill-Bild */
        int balken = 0, wechsel = -1;
        for (int f = 0; f < 200 && wechsel < 0; f++) {
            bild();
            if (re15_game_flag_get(1, 27)) balken = 1;               /* sub02 Set(1,27,1) @0x012C6 */
            if (g_room_change.pending) wechsel = f;
        }
        const unsigned dest = g_room_change.pending ? g_room_change.room_id : 0u;
        if (!bit) {
            CHECK(!balken, "%04X ohne Bit 129: sub02 lief (Kinobalken (1,27) an)", id);
            CHECK(wechsel < 0, "%04X ohne Bit 129: Raumwechsel nach %04X im Bild %d", id, dest, wechsel);
            g_room_change.pending = 0;
            re15_aot_fire_slot(5);                                    /* Tuer 5 direkt (Rect 0, nur ueber Aot_on erreichbar) */
            for (int f = 0; f < 3; f++) bild();
            CHECK(!g_room_change.pending, "%04X ohne Bit 129: Tuer 5 oeffnet nach %04X", id, g_room_change.room_id);
        } else {
            CHECK(balken, "%04X mit Bit 129: sub02 lief nicht (kein Kinobalken)", id);
            CHECK(wechsel >= 120 && dest == ziel, "%04X mit Bit 129: Raumwechsel nach %04X im Bild %d (soll %04X nach Sleep 20 + 100)",
                  id, dest, wechsel, ziel);
        }
        fprintf(stderr, "%04X Bit 129 = %d: Kinobalken %d, Raumwechsel %04X im Bild %d\n", id, bit, balken, dest, wechsel);
    }
}

int main(void)
{
    lauf("STAGE5/ROOM5090.RDT", 0x5090u, 0x6040u);
    lauf("STAGE5/ROOM5091.RDT", 0x5091u, 0x6041u);
    free(s_raw);
    if (fails) { fprintf(stderr, "test_r42_finale_abspann: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_r42_finale_abspann: OK\n");
    return 0;
}
