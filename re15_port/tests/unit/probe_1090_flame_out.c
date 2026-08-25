/* probe_1090_flame_out.c — MESSSONDE (Nutzer-Report 2026-08-27):
 * "die Flammen in ROOM1090 verschwinden nach dem Loeschen nicht".
 *
 * REINE MESSUNG (kein add_test, keine CHECK-Behauptungen): faehrt ROOM1090 ueber die ECHTE
 * SCD-VM hoch (scd_room_reenter -> sub00, Muster probe_1090_flame_touch.c) und protokolliert
 * pro Bild:
 *   - Anzahl aktiver Typ-0x26-Aktoren (die sieben Feuer-Emitter)
 *   - davon TANGIBEL (spider_phase >= 13, `sltiu v0,v0,0xd` @0x80116328-2c = die Schadenszone)
 *   - Anzahl aktiver ESP-FX je Effekt-Id (0x08 CORE00-Flamme, 0x09 Funken, 0x10 Raum-Flamme)
 *   - Zustand der Flagbits, die ROOM1090 sub00 abfragt
 *
 * DANACH die Kandidaten-Sweeps (das ist die eigentliche Frage: gibt es im Port ueberhaupt
 * einen Weg, die Flammen zu BEENDEN?):
 *   B) Flag VOR dem Raum-Boot gesetzt   (Wiedereintritts-Semantik)
 *   C) Flag WAEHREND des Laufs gesetzt  (Laufzeit-Semantik)
 *   D) das Loesch-Event ROOM1090 sub06 zur Laufzeit gefeuert (scd_event_fire(6))
 *   E) Neu-Boot des Raums NACH dem Event (der einzige Weg, den der Port heute hat)
 *
 * KANDIDATEN — alle aus dem ausgelieferten ROOM1090.RDT selbst disassembliert
 * (analysis/nutzer_batch_2026-08-27/tools/re15_scd_dis.py; Opcode-Laengen aus
 *  re15_port/engine/src/scd_vm.c:166 s_opcode_sizes[], dort je Eintrag gegen den
 *  Handler-PC-Advance auditiert). SUB-Block-Basis = Datei 0x21B4 (RDT+0x44):
 *
 *   sub00 @0x21C4:
 *     0x021C4  06 Ifel_ck   00 22 00
 *     0x021C8  21 Ck        03 81 00      ; Kamera-Startcut nur solange es brennt
 *     0x021EA  06 Ifel_ck   00 B4 00
 *     0x021EE  21 Ck        03 81 00      ; <== DAS FEUER-GATE
 *     0x02214  44 Sce_em_set 00 26 00 ...  ; die 7 Emitter (Typ 0x26, grid 0,1,2,4,3,3,4)
 *     ... 0x0228C  44 Sce_em_set 06 26 04 ...
 *     0x022A0  08 Endif
 *     0x022A2  06 Ifel_ck   00 40 00
 *     0x022A6  21 Ck        03 84 01      ; nach dem Loeschen: Typ 0x42 + Evt_exec sub03
 *     0x022E6  06 Ifel_ck   00 1C 00
 *     0x022EA  21 Ck        03 81 00      ; Feuer-BGM-Programm (0x54 = Sce_bgm_control)
 *     0x02306  06 Ifel_ck   00 24 00
 *     0x0230A  21 Ck        03 85 00
 *     0x0232E  06 Ifel_ck   00 42 00
 *     0x02332  21 Ck        03 81 00      ; die Ausloese-Flaeche des Loesch-Events
 *     0x02336  2C Aot_set   02 03 B1 ...  ; sce 3 = Auto-Event
 *     0x02376  06 Ifel_ck   00 78 00
 *     0x0237A  21 Ck        03 BB 01
 *
 *   sub06 @0x2702 (das Loesch-Event):
 *     0x02702  2B Message_on 07 FF FF     ; msg7 "I must hurry up and get something to
 *                                         ;        put out this fire to save that woman"
 *     0x02708  2B Message_on 08 FF FF     ; msg8 "Will you use the <ITEM>"
 *     0x0270E  06 Ifel_ck   00 18 00
 *     0x02712  21 Ck        0C 1F 00
 *     0x02716  22 Set       02 07 01
 *     0x0271A  09 Sleep     0A 0A 00
 *     0x0271E  22 Set       03 81 01      ; <== DAS LOESCH-FLAG (einziger Setzer im Raum)
 *     0x02722  22 Set       03 84 01
 *     0x02726  47 Aot_on    03
 *     0x0272A  01 Evt_end
 *
 * Roh-Gegenprobe ueber alle ausgelieferten RDTs (Byte-Scan auf `22 03 81` / `21 03 81`):
 *   ROOM1090 0x21C8/0x21EE/0x22EA/0x2332 = Ck(3,0x81,0), 0x271E = Set(3,0x81,1);
 *   ausserdem ROOM10B1 0x17C6 Ck(3,0x81,1), 0x1832 Set(3,0x81,1), 0x1954 Set(3,0x81,0).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_esp.h"
#include "re15_fade.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_esp_t         s_bank, s_gbank;
static int                s_has_gbank = 0;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t rd32(const uint8_t *p, uint32_t o)
{ return (uint32_t)p[o] | ((uint32_t)p[o+1]<<8) | ((uint32_t)p[o+2]<<16) | ((uint32_t)p[o+3]<<24); }

/* --- ein Bild des echten Spiel-Ablaufs (identisch zu probe_1090_flame_touch.c) ------------ */
static int s_cine_was_active = 0;
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(&s_bank);
}

typedef struct { int n26, tangible, fx08, fx09, fx10, fxo; int hp; } snap_t;

static snap_t take(void)
{
    snap_t s; memset(&s, 0, sizeof s);
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        if (!g_actors[i].active || g_actors[i].type != 0x26u) continue;
        s.n26++;
        if (g_actors[i].spider_phase >= 13) s.tangible++;   /* @0x80116328-2c */
    }
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || !f->active) continue;
        if      (f->effect_id == 0x10) s.fx10++;
        else if (f->effect_id == 0x08) s.fx08++;
        else if (f->effect_id == 0x09) s.fx09++;
        else                           s.fxo++;
    }
    s.hp = g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
    return s;
}

/* AOT-Slots, die auf ein Auto-/Generic-Event zeigen (der Loesch-Trigger ist einer davon). */
static int count_event_aots(void)
{
    int n = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active &&
            (g_aot.slots[i].type == RE15_AOT_TYPE_AUTO_EVENT ||
             g_aot.slots[i].type == RE15_AOT_TYPE_GENERIC)) n++;
    return n;
}

/* ⛔ scd_vm_init() ruft re15_game_state_init() (scd_vm.c:441), und das macht
 * `memset(&g_game, 0, sizeof(g_game))` (game_state.c:68) — ALLE Flagbaenke werden beim
 * Raum-Boot genullt. Ein VOR boot_room() gesetztes Flag ist danach weg. Die Vorbelegung
 * muss deshalb ZWISCHEN scd_vm_init() und scd_room_reenter() passieren. */
static uint8_t s_pre[8][2]; static int s_pre_n = 0;

static void boot_room(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    re15_esp_fx_reset();
    re15_esp_set_room_bank(&s_bank);
    re15_esp_set_global_bank(s_has_gbank ? &s_gbank : NULL);
    g_current_room_id = 0x1090; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;   /* Tuerspawn 1050 */
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_inv_load_briefing();
    s_shown = 0; s_cine_was_active = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int i = 0; i < s_pre_n; i++) re15_game_flag_set(s_pre[i][0], s_pre[i][1], 1);
    scd_room_reenter(&s_rdt, pl->x, pl->z, /*entry cut*/ 3);
    g_scd.cut_auto_enabled = 1;
}

static void clear_flags(void)
{
    for (int z = 0; z < RE15_FLAG_ZONES; z++)
        for (int b = 0; b < 256; b++) re15_game_flag_set((uint8_t)z, (uint8_t)b, 0);
}

typedef struct { uint8_t zone, bit; const char *why; } cand_t;
static const cand_t k_cand[] = {
    { 3, 0x81, "Ck(3,0x81,0) @0x21EE gated die 7 Sce_em_set; Set(3,0x81,1) @0x271E" },
    { 3, 0x84, "Ck(3,0x84,1) @0x22A6 spawnt Typ 0x42; Set(3,0x84,1) @0x2722" },
    { 3, 0x85, "Ck(3,0x85,0) @0x230A gated die sce-1-Message-Flaeche" },
    { 3, 0x80, "Ck(3,0x80,1) @0x22F8 / Set(3,0x80,1) @0x240A (sub01)" },
    { 3, 0xBB, "Ck(3,0xBB,1) @0x237A gated 4 weitere Sce_em_set (Typ 0x10/0x11)" },
    { 0x0C, 0x1F, "Ck(0x0C,0x1F,0) @0x2712 — das Gate IN sub06" },
};
#define NCAND ((int)(sizeof k_cand / sizeof k_cand[0]))

int main(void)
{
    size_t rsz = 0, gsz = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM1090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }
    if (re15_esp_parse(raw, rsz, rd32(raw,0x4C), rd32(raw,0x50),
                       rd32(raw,0x54), rd32(raw,0x58), &s_bank) != 0) {
        printf("FEHLT: ESP-Parse\n"); return 77;
    }
    uint8_t *graw = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &gsz);
    if (graw && re15_esp_parse_global(graw, gsz, &s_gbank) == 0) s_has_gbank = 1;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    /* ================= A) BASIS-LAUF ==================================================== */
    printf("A) BASIS — ROOM1090 ueber die echte SCD-VM, 200 Bilder, alle Flags 0\n");
    clear_flags();
    boot_room();
    printf("     f  n0x26 tang  fx10 fx08 fx09 rest  evtAOT  f(3,0x81)  hp\n");
    for (int f = 0; f < 200; f++) {
        frame(0, 0);
        if (f < 16 || f % 20 == 0 || f == 199) {
            snap_t s = take();
            printf("   %4d   %3d  %3d   %3d  %3d  %3d  %3d    %3d       %d      %d\n",
                   f, s.n26, s.tangible, s.fx10, s.fx08, s.fx09, s.fxo,
                   count_event_aots(), re15_game_flag_get(3, 0x81), s.hp);
        }
    }
    snap_t base = take();
    printf("   ENDE Basis: n0x26=%d tangibel=%d fx(0x10)=%d fx(0x08)=%d fx(0x09)=%d rest=%d\n",
           base.n26, base.tangible, base.fx10, base.fx08, base.fx09, base.fxo);

    /* ================= B) FLAG VOR DEM BOOT ============================================== */
    printf("\nB) KANDIDATEN — Flag VOR dem Raum-Boot gesetzt (Wiedereintritt), 200 Bilder\n");
    for (int c = 0; c < NCAND; c++) {
        clear_flags();
        s_pre[0][0] = k_cand[c].zone; s_pre[0][1] = k_cand[c].bit; s_pre_n = 1;
        boot_room();
        s_pre_n = 0;
        for (int f = 0; f < 200; f++) frame(0, 0);
        snap_t s = take();
        printf("   flag(%2u,0x%02X)=1 -> n0x26=%d tang=%d fx10=%d fx08=%d fx09=%d rest=%d "
               "evtAOT=%d %s\n",
               k_cand[c].zone, k_cand[c].bit, s.n26, s.tangible, s.fx10, s.fx08, s.fx09, s.fxo,
               count_event_aots(),
               (s.n26 != base.n26 || s.fx10 != base.fx10 || s.fx08 != base.fx08)
                   ? "<== AENDERT ETWAS" : "");
        printf("       Grund: %s\n", k_cand[c].why);
    }

    /* ================= C) FLAG ZUR LAUFZEIT ============================================== */
    printf("\nC) KANDIDATEN — Flag ERST NACH 100 Bildern gesetzt (Laufzeit), dann 200 weitere\n");
    for (int c = 0; c < NCAND; c++) {
        clear_flags();
        boot_room();
        for (int f = 0; f < 100; f++) frame(0, 0);
        snap_t pre = take();
        re15_game_flag_set(k_cand[c].zone, k_cand[c].bit, 1);
        for (int f = 0; f < 200; f++) frame(0, 0);
        snap_t post = take();
        printf("   flag(%2u,0x%02X): vorher n0x26=%d fx10=%d fx08=%d | nachher n0x26=%d "
               "fx10=%d fx08=%d  %s\n",
               k_cand[c].zone, k_cand[c].bit, pre.n26, pre.fx10, pre.fx08,
               post.n26, post.fx10, post.fx08,
               (post.n26 != pre.n26 || post.fx10 != pre.fx10 || post.fx08 != pre.fx08)
                   ? "<== AENDERT ETWAS" : "(unveraendert)");
    }

    /* ================= D) DAS LOESCH-EVENT SELBST ======================================== */
    printf("\nD) LOESCH-EVENT — ROOM1090 sub06 zur Laufzeit gefeuert (scd_event_fire(6))\n");
    printf("   VORBEDINGUNG flag(3,0x85)=1: nur im ELSE-Zweig von Ck(3,0x85,0) @0x230A\n"
           "   installiert sub00 (a) die sce-3-Flaeche @0x2336 -> sub06 und (b) die\n"
           "   SELBST-TUER `Door_aot_set 03` @0x2352 (Ziel ROOM1090, Cut 6,\n"
           "   Spawn 1252/-1800/-2529), die sub06 per `Aot_on 03` @0x2726 ausloest.\n");
    {
        clear_flags();
        s_pre[0][0] = 3; s_pre[0][1] = 0x85; s_pre_n = 1;
        boot_room();
        s_pre_n = 0;
        {   /* Beleg, dass die Kette wirklich installiert ist. */
            printf("   AOT-Slots nach sub00: ");
            for (int i = 0; i < 8; i++)
                if (g_aot.slots[i].active)
                    printf("[%d]type=%d ev=%d ", i, g_aot.slots[i].type, g_aot.slots[i].event_id);
            printf("\n");
        }
        for (int f = 0; f < 100; f++) frame(0, 0);
        snap_t pre = take();
        int slot = scd_event_fire(6);
        printf("   scd_event_fire(6) -> Thread-Slot %d\n", slot);
        /* Der Dialog-Bestaetiger ist die VIRTUELLE Taste 0x4000 = physisch SQUARE
         * (msg_common.c:409-422; Original FUN_80028134, Flanke @0x80028570 / held
         * @0x80028214) — ohne sie bleibt sub06 ewig in `Message_on 07` haengen. */
        for (int f = 0; f < 400; f++) {
            uint16_t held = (f % 8 < 2) ? (uint16_t)RE15_PAD_BIT_SQUARE : 0u;
            uint16_t edge = (f % 8 == 0) ? (uint16_t)RE15_PAD_BIT_SQUARE : 0u;
            frame(held, edge);
            if (f == 60 || f == 200 || f == 399) {
                snap_t s = take();
                printf("   +%3d Bilder: n0x26=%d tang=%d fx10=%d fx08=%d "
                       "f(3,0x81)=%d f(3,0x84)=%d evtAOT=%d | room_change pending=%d "
                       "room=%04X cut=%d spawn=(%d,%d,%d)\n",
                       f, s.n26, s.tangible, s.fx10, s.fx08,
                       re15_game_flag_get(3,0x81), re15_game_flag_get(3,0x84),
                       count_event_aots(),
                       g_room_change.pending, (unsigned)g_room_change.room_id,
                       (int)g_room_change.target_cut, g_room_change.x, g_room_change.y,
                       g_room_change.z);
            }
        }
        snap_t post = take();
        printf("   ERGEBNIS: vorher n0x26=%d fx10=%d fx08=%d -> nachher n0x26=%d fx10=%d "
               "fx08=%d  %s\n",
               pre.n26, pre.fx10, pre.fx08, post.n26, post.fx10, post.fx08,
               (post.fx10 || post.fx08)
                   ? "<== AKTOREN weg, FLAMMEN-FX BLEIBEN (Nutzer-Befund)"
                   : "<== alles weg");

        /* GEGENPROBE zum Fix-Vorschlag: das Original wischt beim JEDEM Raumladen die 96
         * Effekt-Slots (FUN_80019354, Schleife @0x80019368-80 `sb zero,0(at)` @0x80019378,
         * Basis 0x800A7424, Stride 132, 0x60 = 96 Slots; einziger Aufrufer
         * FUN_800396fc @0x8003996c = die Raum-Ladekette). Im Port ist das
         * re15_esp_fx_reset() — heute NUR in re15_room_reset_render_pc (room_pc.c:130),
         * das ausschliesslich der RAUMWECHSEL-Pfad re15_room_apply_pending aufruft. */
        re15_esp_fx_reset();
        snap_t after_wipe = take();
        printf("   Gegenprobe re15_esp_fx_reset() an dieser Stelle: fx10=%d fx08=%d fx09=%d "
               "-> das ist der fehlende Schritt\n",
               after_wipe.fx10, after_wipe.fx08, after_wipe.fx09);
    }

    /* ================= E) NEUEINTRITT NACH DEM EVENT ===================================== */
    printf("\nE) WIEDEREINTRITT nach dem Loesch-Event (der Weg, den der Port heute HAT)\n");
    {
        clear_flags();
        s_pre[0][0] = 3; s_pre[0][1] = 0x85; s_pre_n = 1;
        boot_room();
        s_pre_n = 0;
        for (int f = 0; f < 100; f++) frame(0, 0);
        scd_event_fire(6);
        for (int f = 0; f < 400; f++) {
            uint16_t held = (f % 8 < 2) ? (uint16_t)RE15_PAD_BIT_SQUARE : 0u;
            uint16_t edge = (f % 8 == 0) ? (uint16_t)RE15_PAD_BIT_SQUARE : 0u;
            frame(held, edge);
        }
        int f81 = re15_game_flag_get(3, 0x81), f84 = re15_game_flag_get(3, 0x84);
        printf("   nach dem Event: flag(3,0x81)=%d flag(3,0x84)=%d\n", f81, f84);
        /* Die Flags ueber den Boot RETTEN — scd_vm_init() nullt g_game (Kommentar oben). */
        s_pre_n = 0;
        if (f81) { s_pre[s_pre_n][0] = 3; s_pre[s_pre_n][1] = 0x81; s_pre_n++; }
        if (f84) { s_pre[s_pre_n][0] = 3; s_pre[s_pre_n][1] = 0x84; s_pre_n++; }
        boot_room();                       /* Raum neu laden, Flags werden mitgenommen */
        s_pre_n = 0;
        for (int f = 0; f < 200; f++) frame(0, 0);
        snap_t s = take();
        printf("   nach Neu-Boot: n0x26=%d tang=%d fx10=%d fx08=%d fx09=%d evtAOT=%d\n",
               s.n26, s.tangible, s.fx10, s.fx08, s.fx09, count_event_aots());

        /* Kontroll-Lauf: flag(3,0x81)+flag(3,0x84) VON HAND gesetzt, korrekt nach
         * scd_vm_init(). Zeigt, ob das sub00-Gate im Port ueberhaupt greift — unabhaengig
         * davon, ob sub06 durchlaeuft. */
        clear_flags();
        s_pre[0][0] = 3; s_pre[0][1] = 0x81;
        s_pre[1][0] = 3; s_pre[1][1] = 0x84; s_pre_n = 2;
        boot_room();
        s_pre_n = 0;
        for (int f = 0; f < 200; f++) frame(0, 0);
        snap_t s2 = take();
        printf("   Kontrolle flag(3,0x81)=1 + flag(3,0x84)=1 vor sub00: n0x26=%d tang=%d "
               "fx10=%d fx08=%d fx09=%d evtAOT=%d\n",
               s2.n26, s2.tangible, s2.fx10, s2.fx08, s2.fx09, count_event_aots());
    }

    free(graw); free(raw);
    printf("\nprobe_1090_flame_out: MESSUNG FERTIG (reine Diagnose, kein Pass/Fail)\n");
    return 0;
}
