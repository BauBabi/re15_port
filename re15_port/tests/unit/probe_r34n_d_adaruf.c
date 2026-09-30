/*
 * probe_r34n_d_adaruf.c — Spur D (Runde 34 Nacht), MESS-SONDE fuer den Bauplan
 * "Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0, Sperre bis zur Ada-Rettung".
 * Dossier: analysis/befunde_runde34_nacht/D_adaruf.md (§4 Zeitlinie, §5 Bauplan).
 *
 * WAS SIE TUT: sie faehrt den GEPLANTEN Port-Bytecode (k_ruf, genau wie im Dossier §5.3) mit der
 * ECHTEN VM und dem ECHTEN Spielschritt (re15_msg_tick + scd_vm_tick + re15_game_step, Muster
 * probe_ada_escort_anim.c) ueber einen echten ROOM1050-Raumaufbau. Es gibt noch KEINEN Port-Code:
 * die Sonde spielt die zwei geplanten Engine-Teile selbst nach —
 *   (1) die Installation nach dem Init-Lauf (tuer1120-Muster): Slot 4 -> sce 3 / Ereignis 13
 *       bzw. -> sce 1 / msg 25, Tuer-Band aus door_params uebernommen;
 *   (2) die Ereignis-Weiche in scd_event_fire: Ereignis 13 -> k_ruf mit eingesetztem Rueckschritt-Ziel.
 *       Die Sonde startet den Faden selbst in Slot SCD_EVENT_SLOT_FIRST, in DEM Bild, in dem der
 *       echte Aktions-Scan das Ereignis 13 meldet (g_aot.fired_event_id_this_frame) — genau dort
 *       wuerde scd_event_fire ihn starten.
 * Alles andere (Aktions-Scan, Plc_dest-Laeufer Modus 8 mit Kollision, Plc_motion/Plc_flg,
 * Message_on, Aot_reset, Plc_ret) ist der unveraenderte Port.
 *
 * GEMESSEN je Fall: Bildzahl je Opcode-Position, Weg und Drehung des Rueckschritts, Ankunftsbild,
 * gezeigte Nachrichten, Flags (9,65) (2,7) (1,27), Letterbox-Stand, Slot 4 danach, Folgedruck,
 * Fall "Ada gerettet" (3,0xBB) und Fall "Szene schon gesehen" (9,65).
 * Kein add_test: die Sonde prueft den PLAN, nicht den Port.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_player.h"

void re15_actor_step_all_walkers(void);

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

void scd_register_current_rdt(const re15_rdt_t *rdt);

/* ---- Der geplante Bytecode (Dossier §5.3). Jede Zeile mit ihrem Vorbild. ------------------ */
#define OFF_DREH 0x28   /* x/z-Operand des Plc_dest Modus 9 (Blickpunkt, beim Ausloesen eingesetzt) */
#define OFF_ZIEL 0x3C   /* x/z-Operand des Plc_dest Modus 8 (Rueckschritt-Ziel, beim Ausloesen)     */
#define SCHRITT  755    /* |(613,-2123)-(-130,-1988)| ROOM1090 sub02 @0x0246C/@0x02470 -> @0x0247C  */
static const uint8_t k_ruf[] = {
    /* +00 */ 0x22, 0x09, 0x41, 0x01,             /* Set(9,65)=1  Szene gesehen (Einmal-Riegel am Anfang
                                                      wie ROOM11B0 sub06 @0x1478 `22 03 83 01`)          */
    /* +04 */ 0x22, 0x02, 0x07, 0x01,             /* Set(2,7)=1   Pad-Sperre   = ROOM1090 sub02 @0x02414 */
    /* +08 */ 0x22, 0x01, 0x1b, 0x01,             /* Set(1,27)=1  Letterbox    = ROOM1090 sub02 @0x02418 */
    /* +0C */ 0x2e, 0x01, 0x00, 0x00,             /* Work_set(1,0)+Nop Spieler = ROOM1090 sub02 @0x0241C */
    /* +10 */ 0x40, 0x00, 0x06, 0x3f, 0x00, 0x00, 0x00, 0x00,
                                                  /* Plc_dest Modus 6 (stehen) = ROOM1090 sub02 @0x0242C */
    /* +18 */ 0x09, 0x0a, 0x14, 0x00,             /* Sleep 20                  = ROOM1090 sub02 @0x02434 */
    /* +1C */ 0x2b, 0x16, 0x00, 0x00,             /* Message_on 22 "Woman: Hello? ..." (Form @0x02438)   */
    /* +20 */ 0x09, 0x0a, 0x64, 0x00,             /* Sleep 100                 = ROOM1090 sub02 @0x0243C */
    /* +24 */ 0x40, 0x00, 0x09, 0x20, 0x00, 0x00, 0x00, 0x00,
                                                  /* Plc_dest Modus 9 (auf der Stelle zur Tuerwand +X
                                                     drehen), Bit 0x20 = ROOM1050 sub03 @0x0DC2        */
    /* +2C */ 0x11, 0x00, 0x08, 0x00,             /* Do                        = ROOM1050 sub03 @0x0DCA  */
    /* +30 */ 0x02, 0x00,                         /* Evt_next + Nop            = @0x0DCE                  */
    /* +32 */ 0x12, 0x04,                         /* Edwhile                   = @0x0DD0                  */
    /* +34 */ 0x21, 0x05, 0x20, 0x00,             /* Ck(5,32)==0               = @0x0DD2                  */
    /* +38 */ 0x40, 0x00, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00,
                                                  /* Plc_dest Modus 8 rueckwaerts, Bit 0x20
                                                     (Form = ROOM1090 sub02 @0x0247C)                   */
    /* +40 */ 0x11, 0x00, 0x08, 0x00,             /* Do                        = ROOM1090 sub04 @0x026E6 */
    /* +44 */ 0x02, 0x00,                         /* Evt_next + Nop            = sub04 @0x026EA          */
    /* +46 */ 0x12, 0x04,                         /* Edwhile                   = sub04 @0x026EC          */
    /* +48 */ 0x21, 0x05, 0x20, 0x00,             /* Ck(5,32)==0 (Ankunft)     = sub04 @0x026EE          */
    /* +4C */ 0x09, 0x0a, 0x14, 0x00,             /* Sleep 20                  = ROOM1090 sub02 @0x02486 */
    /* +50 */ 0x2b, 0x17, 0x00, 0x00,             /* Message_on 23 "Leon: Another civilian survivor."  */
    /* +54 */ 0x3f, 0x00, 0x13, 0x00,             /* Plc_motion(0,19,0)        = ROOM1090 sub03 @0x0265C */
    /* +58 */ 0x09, 0x0a, 0x19, 0x00,             /* Sleep 25                  = sub03 @0x02660          */
    /* +5C */ 0x3f, 0x00, 0x13, 0x00,             /* Plc_motion(0,19,0)        = sub03 @0x02664          */
    /* +60 */ 0x43, 0x00, 0x80, 0x00,             /* Plc_flg(0,0x80,0) rueckw. = sub03 @0x02668          */
    /* +64 */ 0x09, 0x0a, 0x1a, 0x00,             /* Sleep 26                  = sub03 @0x0266C          */
    /* +68 */ 0x2b, 0x18, 0x00, 0x00,             /* Message_on 24 "Leon: I have to help her!"          */
    /* +6C */ 0x3f, 0x00, 0x11, 0x00,             /* Plc_motion(0,17,0)        = ROOM1170 sub02 @0x015F0 */
    /* +70 */ 0x09, 0x0a, 0x64, 0x00,             /* Sleep 100                 = ROOM1170 sub02 @0x015F4 */
    /* +74 */ 0x22, 0x02, 0x07, 0x00,             /* Set(2,7)=0                = ROOM1090 sub02 @0x024BE */
    /* +78 */ 0x22, 0x01, 0x1b, 0x00,             /* Set(1,27)=0               = ROOM1090 sub02 @0x024C2 */
    /* +7C */ 0x2e, 0x01, 0x00, 0x00,             /* Work_set(1,0)+Nop         = ROOM1090 sub02 @0x024C6 */
    /* +80 */ 0x42, 0x00,                         /* Plc_ret + Nop             = ROOM1090 sub02 @0x024CA */
    /* +82 */ 0x46, 0x04, 0x01, 0x31, 0x19, 0x00, 0xff, 0xff, 0x00, 0x00,
                                                  /* Aot_reset(4, sce 1, 0x31, msg 25, 0xffff) — Form
                                                     ROOM1130 sub01 @0x00A1C `46 03 01 31 01 00 ff ff`  */
    /* +8C */ 0x01, 0x00,                         /* Evt_end                                              */
};
static uint8_t s_prog[sizeof k_ruf];

static const int k_plan[] = { 0x00,0x04,0x08,0x0C,0x10,0x18,0x19,0x1C,0x20,0x21,0x24,0x2C,0x30,0x31,
                              0x32,0x34,0x38,0x40,0x44,0x45,0x46,0x48,0x4C,0x4D,0x50,0x54,0x58,0x59,
                              0x5C,0x60,0x64,0x65,0x68,0x6C,0x70,0x71,0x74,0x78,0x7C,0x80,0x82,0x8C };

/* Die geplante Weiche setzt beide Ziele aus der Spielerposition im Druckbild. */
static void ziele_einsetzen(const re15_actor_t *pl)
{
    memcpy(s_prog, k_ruf, sizeof k_ruf);
    int32_t dx = pl->x + SCHRITT, zx = pl->x - SCHRITT, zz = pl->z;
    s_prog[OFF_DREH + 0] = (uint8_t)(dx & 0xff); s_prog[OFF_DREH + 1] = (uint8_t)((dx >> 8) & 0xff);
    s_prog[OFF_DREH + 2] = (uint8_t)(zz & 0xff); s_prog[OFF_DREH + 3] = (uint8_t)((zz >> 8) & 0xff);
    s_prog[OFF_ZIEL + 0] = (uint8_t)(zx & 0xff); s_prog[OFF_ZIEL + 1] = (uint8_t)((zx >> 8) & 0xff);
    s_prog[OFF_ZIEL + 2] = (uint8_t)(zz & 0xff); s_prog[OFF_ZIEL + 3] = (uint8_t)((zz >> 8) & 0xff);
}

/* ---- Die vier Nachrichten (Dossier §5.4, Worte belegt mit tools/r34n_d/texte_bauen.py) ------ */
static const uint8_t k_msg22[] = { 0x04,0x00,0x05,0x02, 0x33,0x4b,0x49,0x3d,0x4a,0x16, 0x05,0x00, 0x00,
    0x24,0x41,0x48,0x48,0x4b,0x1b,0x00, 0x1d,0x4a,0x55,0x4b,0x4a,0x41,0x1b,0x00,
    0x2c,0x48,0x41,0x3d,0x4f,0x41,0x18, 0x08,
    0x43,0x41,0x50,0x00,0x49,0x41,0x00,0x4b,0x51,0x50,0x00,0x4b,0x42,0x00,0x44,0x41,0x4e,0x41,0x1a,
    0x04,0x01,0x01,0x63 };
static const uint8_t k_msg23[] = { 0x04,0x00,0x05,0x01, 0x28,0x41,0x4b,0x4a,0x16, 0x05,0x00, 0x00,
    0x1d,0x4a,0x4b,0x50,0x44,0x41,0x4e,0x00, 0x3f,0x45,0x52,0x45,0x48,0x45,0x3d,0x4a,0x00,
    0x4f,0x51,0x4e,0x52,0x45,0x52,0x4b,0x4e,0x57, 0x04,0x01,0x01,0x63 };
static const uint8_t k_msg24[] = { 0x04,0x00,0x05,0x01, 0x28,0x41,0x4b,0x4a,0x16, 0x05,0x00, 0x00,
    0x25,0x00,0x44,0x3d,0x52,0x41,0x00,0x50,0x4b,0x00,0x44,0x41,0x48,0x4c,0x00,0x44,0x41,0x4e,0x1a,
    0x04,0x01,0x01,0x63 };
static const uint8_t k_msg25[] = { 0x04,0x02,
    0x25,0x00,0x44,0x3d,0x52,0x41,0x00,0x50,0x4b,0x00,0x44,0x41,0x48,0x4c,0x00,
    0x50,0x44,0x41,0x00,0x2f,0x51,0x4e,0x52,0x45,0x52,0x4b,0x4e,0x00,0x42,0x45,0x4e,0x4f,0x50,0x1a,
    0x01,0x00 };

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void texte_installieren(void)
{
    static const struct { uint8_t id; const uint8_t *b; size_t n; } t[] = {
        { 22, k_msg22, sizeof k_msg22 }, { 23, k_msg23, sizeof k_msg23 },
        { 24, k_msg24, sizeof k_msg24 }, { 25, k_msg25, sizeof k_msg25 } };
    for (unsigned i = 0; i < sizeof t / sizeof t[0]; i++) {
        re15_msg_install_text(t[i].id, t[i].b, t[i].n);
        int d = re15_msg_compute_duration(t[i].b, t[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(t[i].id, d);
        printf("  msg %d: %u Bytes, Standzeit re15_msg_compute_duration = %d Bilder\n",
               t[i].id, (unsigned)t[i].n, d);
    }
}

/* (1) geplante Installation — Muster re15_tuer1120_install. */
static const char *installieren(void)
{
    if (g_current_room_id != 0x1050) return "nicht ROOM1050";
    if (re15_game_flag_get(3, 0xBB)) return "(3,0xBB)=1 -> Tuer bleibt";
    re15_aot_t *a = &g_aot.slots[4];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return "Slot 4 ist keine Tuer";
    texte_installieren();
    a->band = g_aot.door_params[4].band;
    if (!re15_game_flag_get(9, 65)) {
        re15_aot_retype(4, 3, 0x31, 0x00FF, (uint16_t)((13 << 8) | 0x18), 0);
        return "Slot 4 -> sce 3 / Ereignis 13 (Szene)";
    }
    re15_aot_retype(4, 1, 0x31, 25, 0xFFFF, 0);
    return "Slot 4 -> sce 1 / msg 25 (Text)";
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c: scd_vm_tick (:5378) ->
 * re15_actor_step_all_walkers (:5419) -> re15_letterbox_tick (:5440) -> re15_msg_tick (:5503) ->
 * re15_game_step (:7358). */
static int s_cine_was = 0;
static void frame_step(uint16_t pressed)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    /* Szenen-Uebergabe wie platform/pc/main.c:5445-5467 (player_mode 2 solange flag(1,27) ||
     * flag(2,7); nach dem Loeschen 15 Bilder Balken-Rampe, dann player_mode 0). */
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = pressed; s_ctx.pad_pressed = pressed;
    re15_game_step(&s_ctx);
}

static int raum_hoch(int32_t x, int32_t z, int16_t rot, int bb, int b65)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();    /* scd_vm_init nullt die Flags */
    re15_game_flag_set(3, 121, 1);                         /* Rolltor offen (Spur A: einziger Weg) */
    if (bb)  re15_game_flag_set(3, 0xBB, 1);
    if (b65) re15_game_flag_set(9, 65, 1);
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = 0x1050;
    scd_register_current_rdt(&s_rdt);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100; p->x = x; p->y = 0; p->z = z; p->rot_y = rot;
    p->state = 1;
    re15_collision_set_band(0);
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0; s_cine_was = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, x, z, 0);
    for (int i = 0; i < 30; i++) frame_step(0);
    return 0;
}

static void fall_szene(void)
{
    printf("\n=== Fall A: erster Druck, Ada NICHT gerettet, Rolltor offen (3,121)=1 ===\n");
    raum_hoch(16580, -13700, 0, 0, 0);           /* Standplatz: probe_r33_tueren standplatz 1050 10A0 */
    printf("  Installation: %s\n", installieren());
    re15_aot_t *a = &g_aot.slots[4];
    printf("  Slot 4 danach: type=%d event_id=%d band=%d\n", a->type, a->event_id, a->band);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    frame_step(RE15_PAD_BIT_SQUARE);
    int ev = g_aot.fired_event_id_this_frame;
    printf("  Quadrat-Druck: Aktions-Scan meldet Ereignis %d (Soll 13)\n", ev);
    if (ev != 13) { printf("FAIL: Ereignis nicht gemeldet\n"); return; }

    /* (2) geplante Weiche: Ziel = Spielerposition - 755 in x (weg von der Tuer auf der Ostwand) */
    ziele_einsetzen(pl);
    int32_t zx = pl->x - SCHRITT, zz = pl->z;
    int32_t sx = pl->x, sz = pl->z; int16_t srot = pl->rot_y;
    if (scd_thread_start(SCD_EVENT_SLOT_FIRST, s_prog) != 0) { printf("FAIL: Slot belegt\n"); return; }
    printf("  Start: Spieler (%d,%d) rot %d, Ziel Rueckschritt (%d,%d)\n",
           (int)sx, (int)sz, srot, (int)zx, (int)zz);

    int last_off = -1, fremd = 0, last_msg = -1, bild_ende = -1, ankunft = -1;
    int prev_mo = -1, prev_fl = -1, lb_max = 0, lb_voll = -1;
    for (int f = 1; f <= 600; f++) {
        scd_thread_t *t = &g_scd.threads[SCD_EVENT_SLOT_FIRST];
        int off = (t->active && t->pc) ? (int)(t->pc - s_prog) : -1;
        if (off != last_off) {
            int ok = (off < 0);
            for (unsigned k = 0; k < sizeof k_plan / sizeof k_plan[0]; k++) if (k_plan[k] == off) ok = 1;
            if (!ok) fremd++;
            printf("  B%3d  PC +%02X%s  Spieler (%5d,%6d) rot %4d  st=%d walk=%d mo=%d fl=0x%02X  "
                   "(2,7)=%d (1,27)=%d pm=%d lb=%d msg=%s%d\n",
                   f, off < 0 ? 0xFF : off, ok ? "" : " FREMD",
                   (int)pl->x, (int)pl->z, pl->rot_y, pl->state, pl->walk_active, pl->motion,
                   pl->anim_flags, re15_game_flag_get(2, 7), re15_game_flag_get(1, 27),
                   g_scd.player_mode, g_letterbox_level,
                   g_scd.message_active ? "" : "-", g_scd.message_active ? g_scd.message_id : 0);
            last_off = off;
        }
        if (pl->motion != prev_mo || pl->anim_flags != prev_fl) {
            printf("  B%3d        Clip %d Flags 0x%02X\n", f, pl->motion, pl->anim_flags);
            prev_mo = pl->motion; prev_fl = pl->anim_flags;
        }
        if (g_scd.message_active && g_scd.message_id != last_msg) {
            printf("  B%3d        Nachricht %d sichtbar\n", f, g_scd.message_id);
            last_msg = g_scd.message_id;
        }
        if (ankunft < 0 && off > 0x48) { ankunft = f;
            double w = sqrt((double)(pl->x - sx) * (pl->x - sx) + (double)(pl->z - sz) * (pl->z - sz));
            printf("  B%3d        Rueckschritt fertig: Weg %.0f, jetzt (%d,%d) rot %d\n",
                   f, w, (int)pl->x, (int)pl->z, pl->rot_y); }
        if (g_letterbox_level > lb_max) lb_max = g_letterbox_level;
        if (lb_voll < 0 && g_letterbox_level >= 0xF0) lb_voll = f;
        if (off < 0 && bild_ende < 0) { bild_ende = f; }
        frame_step(0);
        if (bild_ende > 0 && f > bild_ende + 40) break;
    }
    printf("  Faden-Ende Bild %d, Fremd-PC %d, Letterbox voll ab Bild %d (max 0x%02X), jetzt 0x%02X\n",
           bild_ende, fremd, lb_voll, lb_max, g_letterbox_level);
    printf("  Flags: (9,65)=%d (2,7)=%d (1,27)=%d  player_mode=%d  state=%d\n",
           re15_game_flag_get(9, 65), re15_game_flag_get(2, 7), re15_game_flag_get(1, 27),
           g_scd.player_mode, pl->state);
    printf("  Slot 4 danach: type=%d (MESSAGE=%d) event_id=%d maske=0x%04X band=%d\n",
           a->type, RE15_AOT_TYPE_MESSAGE, a->event_id, a->pause_mask16, a->band);

    /* Folgedruck: Spieler zurueck an die Tuer, Quadrat -> msg 25 */
    while (g_scd.message_active) frame_step(0);
    pl->x = 16580; pl->z = -13700; pl->rot_y = 0;
    for (int i = 0; i < 5; i++) frame_step(0);
    frame_step(RE15_PAD_BIT_SQUARE);
    for (int i = 0; i < 3; i++) frame_step(0);
    printf("  Folgedruck: Nachricht aktiv=%d id=%d (Soll 25), Raumwechsel angefordert=%d\n",
           g_scd.message_active, g_scd.message_id, g_room_change.pending);
}

static void fall_install(const char *titel, int bb, int b65)
{
    printf("\n=== %s ===\n", titel);
    raum_hoch(16580, -13700, 0, bb, b65);
    printf("  Installation: %s\n", installieren());
    re15_aot_t *a = &g_aot.slots[4];
    printf("  Slot 4: type=%d (DOOR=%d, MESSAGE=%d, GENERIC=%d) event_id=%d\n", a->type,
           RE15_AOT_TYPE_DOOR, RE15_AOT_TYPE_MESSAGE, RE15_AOT_TYPE_GENERIC, a->event_id);
    frame_step(RE15_PAD_BIT_SQUARE);
    for (int i = 0; i < 3; i++) frame_step(0);
    printf("  Druck: Nachricht aktiv=%d id=%d, Raumwechsel angefordert=%d (Ziel 0x%04X)\n",
           g_scd.message_active, g_scd.message_id, g_room_change.pending,
           (unsigned)g_room_change.room_id);
}

/* Fall E: Druck von den Raendern des Druckbereichs (Punkt 620 voraus im Rechteck
 * (16700..17700, -14700..-12700), FUN_80042bac / aot_common.c) — haengt der Rueckschritt irgendwo? */
static void fall_ecken(void)
{
    static const struct { int32_t x, z; int16_t rot; const char *n; } st[] = {
        { 16580, -13700,    0, "Standplatz, Blick +X" },
        { 16100, -12750,    0, "Nordrand, Blick +X" },
        { 16100, -14650,    0, "Suedrand, Blick +X" },
        { 16300, -12300,  512, "Nordwest, Blick SO (+X-Z)" },
        { 16300, -15100, 3584, "Suedwest, Blick NO (+X+Z)" },
        { 17000, -12200, 1024, "im Rechteck-Norden, Blick -Z" },
        { 17000, -15150, 3072, "im Rechteck-Sueden, Blick +Z" },
    };
    printf("\n=== Fall E: Rueckschritt von verschiedenen Druckstellen ===\n");
    for (unsigned k = 0; k < sizeof st / sizeof st[0]; k++) {
        raum_hoch(st[k].x, st[k].z, st[k].rot, 0, 0);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int32_t x0 = pl->x, z0 = pl->z;
        (void)installieren();
        frame_step(RE15_PAD_BIT_SQUARE);
        int ev = g_aot.fired_event_id_this_frame;
        if (ev != 13) { printf("  %-32s (%d,%d): kein Ereignis (Punkt 620 voraus nicht im Rechteck), Spieler (%d,%d)\n",
                               st[k].n, (int)x0, (int)z0, (int)pl->x, (int)pl->z); continue; }
        ziele_einsetzen(pl);
        int32_t sx = pl->x, sz = pl->z;
        scd_thread_start(SCD_EVENT_SLOT_FIRST, s_prog);
        int start8 = -1, ankunft = -1, ende = -1;
        for (int f = 1; f <= 700 && ende < 0; f++) {
            scd_thread_t *t = &g_scd.threads[SCD_EVENT_SLOT_FIRST];
            int off = (t->active && t->pc) ? (int)(t->pc - s_prog) : -1;
            if (start8 < 0 && off > 0x38) start8 = f;
            if (ankunft < 0 && off > 0x48) ankunft = f;
            if (off < 0) ende = f;
            frame_step(0);
        }
        double w = sqrt((double)(pl->x - sx) * (pl->x - sx) + (double)(pl->z - sz) * (pl->z - sz));
        printf("  %-32s Druck bei (%d,%d): Schritt B%d..B%d (%d Bilder), Weg %.0f, Ende (%d,%d) rot %d, Faden-Ende B%d\n",
               st[k].n, (int)sx, (int)sz, start8, ankunft, ankunft - start8, w, (int)pl->x, (int)pl->z,
               pl->rot_y, ende);
    }
}

int main(void)
{
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1050.RDT", &n);
    if (!buf || re15_rdt_parse(buf, n, &s_rdt) != 0) { printf("FAIL: ROOM1050.RDT\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 4;
    printf("probe_r34n_d_adaruf: k_ruf %u Bytes, RBJ %d Bytes\n", (unsigned)sizeof k_ruf,
           s_rdt.animation_size);

    /* Clip-Laengen der Gesten im Raum-RBJ Record 0 (so wie der Port sie laedt:
     * re15_emd_parse_rbj, Basis-Hierarchie 15 Knochen). */
    {
        static re15_emd_skeleton_t base, sk; static re15_emd_animation_t an;
        memset(&base, 0, sizeof base); base.bone_count = 15;
        int rc = re15_emd_parse_rbj(s_rdt.animation, (size_t)s_rdt.animation_size, &base, &sk, &an);
        printf("  RBJ rec0: rc=%d, %d Clips; Clip 17 = %d Bilder, Clip 19 = %d Bilder\n", rc,
               an.clip_count, an.clip_count > 17 ? an.clips[17].frame_count : -1,
               an.clip_count > 19 ? an.clips[19].frame_count : -1);
    }

    fall_szene();
    fall_install("Fall B: Szene schon gesehen (9,65)=1, Ada nicht gerettet", 0, 1);
    fall_install("Fall C: Ada gerettet (3,0xBB)=1 -> Originaltuer", 1, 0);
    fall_install("Fall D: Ada gerettet UND Szene gesehen", 1, 1);
    fall_ecken();
    return 0;
}
