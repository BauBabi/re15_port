/*
 * Runde 41 Spur E "kleinkram_b", Punkt 3: Elzas Werkstatt-Tuer ROOM11B1 bleibt dauerhaft zu.
 * Dossier analysis/befunde_runde41/E_kleinkram_b.md; Modul engine/src/werkstatt11b1_r41.c (re15_werkstatt11b1_r41.h).
 * Muster: test_r39_johnruf1041.c (Raumstart + Spielbild), test_r36_szenenleon.c (SCA-Zelle lesen).
 *
 * Teile (Aufruf: test_r41_kleinkram_b_11b1 <teil>):
 *   programm  Programmbytes = belegte Form (Umkehr von sub00 @0x0110E..@0x0111A + Text-Platz @0x0112A Byte fuer Byte,
 *             Cut-Rueckfuehrung in der Form @0x01102..@0x0110A), Anker = sub00 der ausgelieferten RDT.
 *   zu        ROOM11B1 (Elza) mit (3,0x83)=1 + Strom: nach dem Init-Lauf Zelle (3,0x0C) u0=ff fl=03, Platz 8 = Text
 *             msg 12, RVD-Zonen wieder auf Cut 0/2 (wie die ausgelieferte Tabelle), Eingangs-Cut 0 -> am Ende 0
 *             (nicht 7); Schacht Slot 5 bleibt Tuer, Container obj 0 bei z=-19368. Gang nach Westen: blockiert an der
 *             Zelle; Aktionstaste davor: msg 12 "The door is locked.", kein Raumwechsel. Mehrfaches Betreten: gleich.
 *   original  ohne (3,0x83): Else-Zweig der RDT unveraendert (Platz 8 Text msg 12, Schacht Slot 5 zu (sce 0), kein
 *             Ereignis 24); Leon ROOM11B0 mit (3,0x83)=1: Zelle offen (u0 0), kein Ereignis 24 (Runde 36 J gilt dort).
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
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_gameflow.h"
#include "re15_menu.h"
#include "re15_werkstatt11b1_r41.h"
#include "re15_map_hint.h"     /* re15_host_clock_set_us */
#include "re15_cut10f0.h"       /* re15_cine_active */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
void re15_actor_step_all_walkers(void);

static int g_fail = 0, g_n = 0;
#define PRUEF(bed, ...) do { g_n++; int _ok = (bed) ? 1 : 0; if (!_ok) g_fail++; \
    printf("  %s: ", _ok ? "PASS" : "FAIL"); printf(__VA_ARGS__); printf("\n"); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)sz; return b;
}

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_rdt_t         s_rdt_rein;          /* zweite, unveraenderte Parse derselben Bytes (RVD-Vergleich) */
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_bild = 0, s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static uint64_t           s_uhr = 1000000;

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt_rein) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (Muster test_r39_johnruf1041.c bild()). */
static void bild(uint16_t pressed, uint16_t held)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = pressed;
    if (re15_menu_gameplay_frozen()) { re15_game_step(&s_ctx); re15_fade_tick(); s_bild++; return; }
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut = s_shown;
    re15_game_step(&s_ctx);
    re15_fade_tick();
    s_bild++;
}

/* Raumstart wie der Spielweg (scd_room_setup ueber scd_room_reenter): Flags VOR dem Raumskript setzen. */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, int cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_bild = 0; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static const re15_sca_entry_t *zelle(const re15_rdt_t *r, int region, int index)
{
    int base = 0;
    for (int i = 0; i < region; i++) base += r->sca_rgn[i];
    int fi = base + index;
    return (fi >= 0 && fi < r->sca_count) ? &r->sca[fi] : NULL;
}

static int slot_ist(int slot, int typ, int ev)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    return a->active && (int)a->type == typ && (ev < 0 || (int)a->event_id == ev);
}

/* RVD-Zonen des geladenen Raums gegen die unveraenderte Parse: wie viele Zonen weichen ab, wie viele nennen Cut 7/8. */
static void rvd_vergleich(int *abweichend, int *offen)
{
    int ab = 0, of = 0;
    for (int i = 0; i < g_room_rdt.zone_count && i < s_rdt_rein.zone_count; i++) {
        if (g_room_rdt.zones[i].cam_from != s_rdt_rein.zones[i].cam_from ||
            g_room_rdt.zones[i].cam_to   != s_rdt_rein.zones[i].cam_to) ab++;
        if (g_room_rdt.zones[i].cam_from == RE15_WK11B1_CUT_OFFEN_A || g_room_rdt.zones[i].cam_to == RE15_WK11B1_CUT_OFFEN_A ||
            g_room_rdt.zones[i].cam_from == RE15_WK11B1_CUT_OFFEN_B || g_room_rdt.zones[i].cam_to == RE15_WK11B1_CUT_OFFEN_B) of++;
    }
    *abweichend = ab; *offen = of;
}

/* ============================================================================================ programm */
static void teil_programm(void)
{
    printf("== programm ==\n");
    int n = 0;
    const uint8_t *p = re15_werkstatt11b1_r41_programm(&n);
    PRUEF(p && n == 70, "Programm 70 B (%d)", n);
    if (!p || n != 70) return;
    PRUEF(!memcmp(p + 0x00, "\x4b\x00\x07\x00\x4b\x02\x08\x00", 8), "+0x00 Cut_replace(0,7)+Nop, Cut_replace(2,8)+Nop = sub00 @0x0110E/@0x01112 (Tausch zurueck)");
    PRUEF(!memcmp(p + 0x08, "\x37\x03\x0c\xff\x39\x03\x0c\x03", 8), "+0x08 Zelle (3,0x0C) u0=ff fl=03 = RDT @0x00EC8 (Umkehr von @0x01116/@0x0111A)");
    if (rdt_laden(RE15_WK11B1_RAUM) == 0) {
        PRUEF(!memcmp(p + 0x10, s_raw + 0x112A, 20), "+0x10 Aot_set Platz 8 == ROOM11B1.RDT @0x0112A Byte fuer Byte (msg 12)");
        PRUEF(p[0x10] == 0x2c && p[0x11] == RE15_WK11B1_PLATZ && p[0x12] == 1 && p[0x1e] == RE15_WK11B1_MSG,
              "  Platz %d, sce 1 (Text), msg %d", p[0x11], p[0x1e]);
        PRUEF(re15_werkstatt11b1_r41_anker_ok() == 0, "Anker gilt nur fuer den GELADENEN Raum (hier keiner): 0");
        /* Anker: sub00 der Datei == eingebettete 102 B (RDT-Kopf sub_scd Eintrag 0) */
        const uint8_t *sub = s_rdt.sub_scd[0];
        PRUEF(sub && (size_t)(sub - s_raw) == RE15_WK11B1_ANKER_OFF, "sub00 liegt @0x%05X (Soll @0x%05X)",
              sub ? (unsigned)(sub - s_raw) : 0u, (unsigned)RE15_WK11B1_ANKER_OFF);
    }
    PRUEF(!memcmp(p + 0x24, "\x06\x00\x0c\x00\x23\x00\x0a\x00\x07\x00\x29\x00\x3c\x01\x08\x00", 16),
          "+0x24 Cut 7 -> 0 (Form sub00 @0x01102..@0x0110A umgekehrt)");
    PRUEF(!memcmp(p + 0x34, "\x06\x00\x0c\x00\x23\x00\x0a\x00\x08\x00\x29\x02\x3c\x01\x08\x00", 16), "+0x34 Cut 8 -> 2");
    PRUEF(p[0x44] == 0x01 && p[0x45] == 0x00, "+0x44 Evt_end");
    PRUEF(RE15_WK11B1_EREIGNIS >= 7 && RE15_WK11B1_EREIGNIS < RE15_RDT_MAX_SUB_SCD, "Ereignis %d hinter den 7 Subs der RDT, unter %d",
          RE15_WK11B1_EREIGNIS, RE15_RDT_MAX_SUB_SCD);
    PRUEF(re15_werkstatt11b1_r41_ereignis(0x11B1, RE15_WK11B1_EREIGNIS) == p &&
          re15_werkstatt11b1_r41_ereignis(0x11B0, RE15_WK11B1_EREIGNIS) == NULL &&
          re15_werkstatt11b1_r41_ereignis(0x11B1, 23) == NULL, "Programm nur fuer (ROOM11B1, %d)", RE15_WK11B1_EREIGNIS);
}

/* ============================================================================================ zu */
static void elza_11b1(int schacht, int32_t px, int32_t pz, int16_t rot, int cut)
{
    scd_vm_init();   /* loescht die Flags */
    re15_game_flag_set(4, 0xF3, 1);                       /* Strom (sub01 @0x011A0: sonst Slots 1/4/7/8 -> msg 14) */
    re15_game_flag_set(3, 0x46, 0);                       /* keine Sherry-Suche (sub03) in diesem Riegel */
    re15_game_flag_set(RE15_WK11B1_FLAG_BANK, RE15_WK11B1_FLAG_BIT, schacht ? 1 : 0);
    room_boot(RE15_WK11B1_RAUM, px, pz, rot, cut);
}

static void teil_zu(void)
{
    printf("== zu ==\n");
    unsigned starts0 = re15_werkstatt11b1_r41_starts();
    /* Eintritt aus dem Gang (11B1 Slot 0 <- 1181 Slot 3 @0x00A74: Cut 0) — sub00 schaltet bei (3,0x83) auf Cut 7 */
    elza_11b1(1, -27000, -18000, 0, 0);
    PRUEF(re15_werkstatt11b1_r41_anker_ok() == 1, "Anker: geladene RDT == ausgelieferte sub00-Bytes @0x010F6");
    bild(0, 0);   /* Init-Lauf: main00, sub00, Ereignis 24 */
    PRUEF(re15_werkstatt11b1_r41_starts() == starts0 + 1, "Ereignis %d im Init-Lauf gestartet (%u)", RE15_WK11B1_EREIGNIS,
          re15_werkstatt11b1_r41_starts() - starts0);
    for (int f = 0; f < 4; f++) bild(0, 0);
    {
        const re15_sca_entry_t *z = zelle(&g_room_rdt, RE15_WK11B1_ZELLE_REGION, RE15_WK11B1_ZELLE_INDEX);
        PRUEF(z && z->u0 == RE15_WK11B1_ZELLE_U0 && z->floor == RE15_WK11B1_ZELLE_FL,
              "Zelle (3,0x0C): u0=0x%02x fl=0x%02x (Soll ff/03, RDT @0x00EC8)", z ? z->u0 : 0u, z ? z->floor : 0u);
    }
    PRUEF(slot_ist(RE15_WK11B1_PLATZ, RE15_AOT_TYPE_MESSAGE, RE15_WK11B1_MSG), "Platz 8 = Text msg 12 \"The door is locked.\"");
    PRUEF(slot_ist(5, RE15_AOT_TYPE_DOOR, -1), "Schacht Slot 5 bleibt Tuer (Zweig (3,0x83)==1: kein Aot_reset @0x0113E)");
    {
        int ab = 0, of = 0; rvd_vergleich(&ab, &of);
        /* Die RVD-Tabelle fuehrt das offene Netz 7..10 selbst (Runde 36 J N1.1) - massgeblich ist: identisch zur
         * unveraenderten Parse (sub00-Tausch 0<->7/2<->8 durch das Programm zurueckgenommen). */
        PRUEF(ab == 0, "RVD-Zonen wie ausgeliefert: %d abweichend (Zonen mit Cut 7/8 in der RDT selbst: %d)", ab, of);
    }
    PRUEF((int)g_scd.cam_id == 0, "Eingangs-Cut 0 bleibt 0 (sub00 @0x01108 -> 7, Programm -> 0): cam_id=%d", (int)g_scd.cam_id);
    {   /* Container obj 0 (Work_set(3,0) @0x0111E + Member_set @0x01122): z = -19368 — der Zweig bleibt bis auf die Tuer */
        int z_ok = (g_scd.prop_count > 0 && g_scd.props[0].active && g_scd.props[0].z == -19368);
        PRUEF(z_ok, "Container obj 0 (prop 0) bei z=%d (Soll -19368, Member_set @0x01122 laeuft weiter)",
              g_scd.prop_count > 0 ? (int)g_scd.props[0].z : 0);
    }

    /* Gang von Osten gegen die Zelle: x bleibt oestlich von -21410 + Radius */
    elza_11b1(1, -20200, -16230, 2048, 2);
    for (int f = 0; f < 5; f++) bild(0, 0);
    int32_t x0 = g_actors[RE15_ACTOR_SLOT_PLAYER].x;
    for (int f = 0; f < 90; f++) bild(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);
    int32_t x1 = g_actors[RE15_ACTOR_SLOT_PLAYER].x;
    PRUEF(x1 < x0 && x1 >= -21410 - 50, "Lauf nach Westen (yaw 2048 = -x): x %d -> %d, blockiert vor der Zelle x>=-21410 (Radius 450)", (int)x0, (int)x1);
    {   /* Aktionstaste an der Zelle: Text msg 12, kein Raumwechsel */
        int msg = -1;
        g_room_change.pending = 0;
        bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        for (int f = 0; f < 40; f++) { if (g_scd.message_active && msg < 0) msg = (int)g_scd.message_id; if (g_room_change.pending) break; bild(0, 0); }
        PRUEF(msg == RE15_WK11B1_MSG && !g_room_change.pending, "Aktionstaste vor der Tuer: msg %d (Soll 12), Raumwechsel %d", msg, g_room_change.pending);
    }
    /* Zweites Betreten (Rueckkehr): wieder zu */
    elza_11b1(1, -27000, -18000, 0, 0);
    for (int f = 0; f < 5; f++) bild(0, 0);
    {
        const re15_sca_entry_t *z = zelle(&g_room_rdt, RE15_WK11B1_ZELLE_REGION, RE15_WK11B1_ZELLE_INDEX);
        int ab = 0, of = 0; rvd_vergleich(&ab, &of);
        PRUEF(z && z->u0 == 0xFF && slot_ist(RE15_WK11B1_PLATZ, RE15_AOT_TYPE_MESSAGE, RE15_WK11B1_MSG) && ab == 0,
              "Wiederbetreten: Zelle zu (u0 0x%02x), Platz 8 Text, Zonen wie ausgeliefert (%d abweichend, %d mit 7/8)",
              z ? z->u0 : 0u, ab, of);
    }
}

/* ============================================================================================ original */
static void teil_original(void)
{
    printf("== original ==\n");
    unsigned starts0 = re15_werkstatt11b1_r41_starts();
    elza_11b1(0, -27000, -18000, 0, 0);
    for (int f = 0; f < 5; f++) bild(0, 0);
    PRUEF(re15_werkstatt11b1_r41_starts() == starts0, "ohne (3,0x83): kein Ereignis %d", RE15_WK11B1_EREIGNIS);
    {
        const re15_sca_entry_t *z = zelle(&g_room_rdt, 3, 0x0C);
        int ab = 0, of = 0; rvd_vergleich(&ab, &of);
        PRUEF(z && z->u0 == 0xFF, "Else-Zweig der RDT: Zelle zu (u0 0x%02x)", z ? z->u0 : 0u);
        PRUEF(slot_ist(8, RE15_AOT_TYPE_MESSAGE, 12), "Else-Zweig: Platz 8 msg 12 (RDT @0x0112A)");
        PRUEF(!slot_ist(5, RE15_AOT_TYPE_DOOR, -1), "Else-Zweig: Schacht Slot 5 kein Tuer-Satz (Aot_reset sce 0 @0x0113E), Typ %d aktiv %d",
              (int)g_aot.slots[5].type, (int)g_aot.slots[5].active);
        PRUEF(ab == 0, "Else-Zweig: RVD-Zonen wie ausgeliefert (%d abweichend, %d mit 7/8)", ab, of);
    }
    /* Leon ROOM11B0 mit (3,0x83)=1: Runde 36 J entscheidet dort ((9,84)); ohne (9,84) ist die Zelle offen */
    scd_vm_init();
    re15_game_flag_set(4, 0xF3, 1); re15_game_flag_set(4, 0x06, 1); re15_game_flag_set(3, 0x83, 1);
    re15_game_flag_set(9, 84, 0);
    starts0 = re15_werkstatt11b1_r41_starts();
    room_boot(0x11B0, -27000, -18000, 0, 0);
    for (int f = 0; f < 5; f++) bild(0, 0);
    {
        const re15_sca_entry_t *z = zelle(&g_room_rdt, 3, 0x0C);
        PRUEF(re15_werkstatt11b1_r41_starts() == starts0, "Leon ROOM11B0: kein Ereignis %d", RE15_WK11B1_EREIGNIS);
        PRUEF(z && z->u0 == 0x00, "Leon ROOM11B0 ohne (9,84): Zelle (3,0x0C) offen (u0 0x%02x) — unberuehrt", z ? z->u0 : 0xFFu);
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "programm") || !strcmp(teil, "alle")) teil_programm();
    if (!strcmp(teil, "zu")       || !strcmp(teil, "alle")) teil_zu();
    if (!strcmp(teil, "original") || !strcmp(teil, "alle")) teil_original();
    printf("%s: %d/%d bestanden\n", g_fail ? "FAIL" : "OK", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
