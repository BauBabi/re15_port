/*
 * test_r40_nahaufnahmen.c — Runde 40 "nahaufnahmen": nie gezeigte STAGE1-Nahaufnahmen beim Untersuchen.
 * Herleitung: include/re15_nahaufnahmen_r40.h.
 *
 * ECHTER WEG je Raum (Leon- und Elza-Fassung): RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter =
 * Installer-Block), Spieler vor die Stelle, Blick hin, Aktion -> re15_aot_scan (Vorwaertspunkt 620) ->
 * scd_event_fire -> Programm; je Bild re15_cam_present_tick (angezeigter Cut = work_vars[0x0A]). Geprueft:
 *   bytes   die Original-Teile der Programme == RDT (1110 sub03 @0x00D24.., 11F0 sub16 @0x015A2..).
 *   pult    11D0/11D1 Platz 2 -> Ereignis 31; msg 0 mit Cut 12; danach Raum-Cut, Cut_auto an, (2,7)=0.
 *   spind   1110/1111 Platz 16 -> Ereignis 31; msg 4 mit Cut 11; zurueck.
 *   bombe   1110/1111 Platz 20 (Ereignis 3): msg 8 mit Cut 12, Ja -> (3,0x92)=1, msg 9; zurueck.
 *   vus     1100/1101 ohne Geraet: Cuts 7, 8, 9, 10 nacheinander, msg 1 mit Cut 10; danach Cut 6, Cut_auto an.
 *   panel   11F0/11F1 Platz 1 (Ereignis 16): msg 0/1 mit Cut 12; Ja -> Cut 10, (5,0)=1 und der gemerkte Cut ist
 *           der Raum-Cut (sub17 Cut_old kehrt dorthin zurueck); Nein -> Raum-Cut, (5,0)=0.
 *   fremd   ROOM1120 bekommt nichts.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_nahaufnahmen_r40.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

void scd_register_current_rdt(const re15_rdt_t *rdt);
extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;
extern uint8_t  g_aot_action_pressed;

static int s_fail = 0, s_pruef = 0;
#define PRUEFE(cond, ...) do { s_pruef++; if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fail = 1; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

static uint8_t *s_daten = NULL;
static re15_rdt_t s_rdt;

static int raum_hoch(uint16_t raum, int32_t x, int32_t z)
{
    char pfad[256];
    long sz = 0;
    snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/STAGE1/ROOM%04X.RDT", raum);
    free(s_daten);
    s_daten = slurp(pfad, &sz);
    if (!s_daten || re15_rdt_parse(s_daten, (size_t)sz, &s_rdt) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    scd_register_current_rdt(&s_rdt);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = x; p->y = 0; p->z = z; p->rot_y = 0; p->state = 1;
    g_scd.player_mode = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, p->x, p->z, 0);
    for (int i = 0; i < 60; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); (void)re15_cam_present_tick(); }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return 0;
}

typedef struct {
    int msgs[8], n_msgs;       /* geoeffnete Nachrichten in Reihenfolge */
    int cut_bei[8];            /* angezeigter Cut im Oeffnungsbild je Nachricht */
    int cuts[16], n_cuts;      /* Folge der angezeigten Cuts (Wechsel) */
    int cut_vorher;
} lauf_t;

/* Spieler an (x,z,rot), Aktion, dann Bilder; an einer Frage "nein" = einmal umschalten (0x1000) vor dem Bestaetigen. */
static lauf_t druecken(int32_t x, int32_t z, int rot, int nein)
{
    lauf_t L; memset(&L, 0, sizeof L);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = x; p->z = z; p->rot_y = (int16_t)rot;
    L.cut_vorher = g_scd.work_vars[0x0A];
    L.cuts[L.n_cuts++] = L.cut_vorher;
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    int offen = -1, umgeschaltet = 0;
    for (int i = 0; i < 3000; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (g_scd.message_id != offen && L.n_msgs < 8) {
                offen = g_scd.message_id;
                L.cut_bei[L.n_msgs] = g_scd.work_vars[0x0A];
                L.msgs[L.n_msgs++] = offen;
            }
            if ((i & 7) == 7 && (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 ||
                                 g_scd.message_fsm == 4 || g_scd.message_fsm == 7)) {
                if (nein && !umgeschaltet && re15_msg_is_choice(g_scd.message_id)) { g_scd_pad_edge = 0x1000; umgeschaltet = 1; }
                else g_scd_pad_edge = 0x4000;
            }
        } else offen = -1;
        re15_msg_tick(0, 0, 0);
        g_scd_pad_edge = 0;
        scd_vm_tick();
        re15_aot_scan(p->x, p->z, 0xFF);
        (void)re15_cam_present_tick();
        if (L.n_cuts < 16 && g_scd.work_vars[0x0A] != L.cuts[L.n_cuts - 1]) L.cuts[L.n_cuts++] = g_scd.work_vars[0x0A];
        int laeuft = g_scd.message_active;
        for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++)
            if (g_scd.threads[k].active) laeuft = 1;
        if (!laeuft && i > 30) break;
    }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return L;
}

static void zeige(const char *was, uint16_t raum, const lauf_t *L)
{
    printf("  ROOM%04X %-6s msgs:", raum, was);
    for (int i = 0; i < L->n_msgs; i++) printf(" %d@Cut%d", L->msgs[i], L->cut_bei[i]);
    printf("  Cuts:");
    for (int i = 0; i < L->n_cuts; i++) printf(" %d", L->cuts[i]);
    printf("  Ende cam=%d auto=%d (2,7)=%d\n", g_scd.cam_id, g_scd.cut_auto_enabled, re15_game_flag_get(2, 7));
}

static void pruefe_platz(uint16_t raum, int slot)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    PRUEFE(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_N40_EREIGNIS && (a->sce_flags & 0x7f) == 0x31,
           "ROOM%04X Platz %d: aktiv=%d Typ=%d ereignis=%d sat=%02x", raum, slot, a->active, a->type, a->event_id, a->sce_flags);
}

static void pruefe_zurueck(uint16_t raum, const char *was, const lauf_t *L, int soll)
{
    PRUEFE(g_scd.cam_id == soll && g_scd.work_vars[0x0A] == soll && g_scd.cut_auto_enabled == 1 && re15_game_flag_get(2, 7) == 0,
           "ROOM%04X %s: Ende cam=%d angezeigt=%d auto=%d (2,7)=%d, erwartet Cut %d", raum, was, g_scd.cam_id,
           g_scd.work_vars[0x0A], g_scd.cut_auto_enabled, re15_game_flag_get(2, 7), soll);
    (void)L;
}

static void fall_bytes(void)
{
    for (int v = 0; v < 2; v++) {
        int n = 0;
        const uint8_t *b = re15_nahaufnahmen_r40_programm("bombe", &n);
        if (raum_hoch((uint16_t)(0x1110 + v), 0, 0) != 0) { PRUEFE(0, "1110 Parse"); return; }
        PRUEFE(n > 42 && memcmp(b + 6, s_daten + 0x00D24, 0x24) == 0 && s_daten[0x00D48] == 0x01,
               "ROOM%04X: Zeitbomben-Teil != sub03 @0x00D24..@0x00D47", 0x1110 + v);
        const uint8_t *q = re15_nahaufnahmen_r40_programm("panel", &n);
        if (raum_hoch((uint16_t)(0x11F0 + v), 0, 0) != 0) { PRUEFE(0, "11F0 Parse"); return; }
        const uint8_t *s = s_daten;
        PRUEFE(memcmp(q + 2, s + 0x015A2, 12) == 0, "ROOM%04X: Panel msg 0/1 != @0x015A2..", 0x11F0 + v);
        PRUEFE(memcmp(s + 0x015AE, "\x06\x00\x46\x00", 4) == 0 && memcmp(q + 14, "\x06\x00\x48\x00", 4) == 0,
               "ROOM%04X: Ifel-Laenge 0x46 + 2", 0x11F0 + v);
        PRUEFE(memcmp(q + 18, s + 0x015B2, 14) == 0, "ROOM%04X: Panel Ck/Aot_reset != @0x015B2..", 0x11F0 + v);
        PRUEFE(q[32] == 0x2a && q[33] == 0x00, "Panel: Cut_old eingefuegt");
        PRUEFE(memcmp(q + 34, s + 0x015C0, 0x38) == 0, "ROOM%04X: Panel Cut_chg 10 + Sets + Endif != @0x015C0..@0x015F7",
               0x11F0 + v);
        PRUEFE(q[n - 2] == 0x01 && q[n - 1] == 0x00, "Panel: Evt_end");
    }
}

int main(void)
{
    fall_bytes();

    for (int v = 0; v < 2; v++) {
        const uint16_t r11d = (uint16_t)(0x11D0 + v), r111 = (uint16_t)(0x1110 + v);
        const uint16_t r110 = (uint16_t)(0x1100 + v), r11f = (uint16_t)(0x11F0 + v);
        lauf_t L;

        /* pult: Platz 2 (-3600,-20400,800,800), Blick +z */
        if (raum_hoch(r11d, -3200, -21000) == 0) {
            pruefe_platz(r11d, RE15_N40_PULT_SLOT);
            L = druecken(-3200, -20620, 3072, 0); zeige("pult", r11d, &L);
            PRUEFE(L.n_msgs >= 1 && L.msgs[0] == 0 && L.cut_bei[0] == 12, "ROOM%04X pult: msg 0 mit Cut 12", r11d);
            pruefe_zurueck(r11d, "pult", &L, L.cut_vorher);
        } else PRUEFE(0, "11D0 Parse");

        /* spind: Platz 16 (-12950,-2500,5100,1000), Blick -z */
        if (raum_hoch(r111, -12000, -500) == 0) {
            pruefe_platz(r111, RE15_N40_SPIND_SLOT);
            L = druecken(-12000, -1380, 1024, 0); zeige("spind", r111, &L);
            PRUEFE(L.n_msgs >= 1 && L.msgs[0] == 4 && L.cut_bei[0] == 11, "ROOM%04X spind: msg 4 mit Cut 11", r111);
            pruefe_zurueck(r111, "spind", &L, L.cut_vorher);
            /* bombe: Platz 20 (-14200,-1250,800,1000), Blick -x */
            L = druecken(-13180, -750, 2048, 0); zeige("bombe", r111, &L);
            PRUEFE(L.n_msgs >= 2 && L.msgs[0] == 8 && L.cut_bei[0] == 12 && L.msgs[1] == 9 && L.cut_bei[1] == 12,
                   "ROOM%04X bombe: msg 8/9 mit Cut 12", r111);
            /* sub03 @0x00D32 Set(3,0x92)=1, @0x00D36 Aot_reset Platz 20 sce 0 (bleibt registriert, Typ NONE) */
            PRUEFE(re15_game_flag_get(3, 0x92) == 1 && g_aot.slots[20].type != RE15_AOT_TYPE_GENERIC,
                   "ROOM%04X bombe: (3,0x92)=%d Platz 20 aktiv=%d Typ=%d", r111, re15_game_flag_get(3, 0x92),
                   g_aot.slots[20].active, g_aot.slots[20].type);
            pruefe_zurueck(r111, "bombe", &L, L.cut_vorher);
        } else PRUEFE(0, "1110 Parse");

        /* vus: Platz 2 (-25749,-9956,1097,564) ohne Geraet ((3,0x67)=0), Blick +z */
        if (raum_hoch(r110, -25200, -11000) == 0) {
            pruefe_platz(r110, RE15_N40_VUS_SLOT);
            L = druecken(-25200, -10294, 3072, 0); zeige("vus", r110, &L);
            int folge = 0;
            for (int i = 0; i + 3 < L.n_cuts; i++)
                if (L.cuts[i] == 7 && L.cuts[i + 1] == 8 && L.cuts[i + 2] == 9 && L.cuts[i + 3] == 10) folge = 1;
            PRUEFE(folge, "ROOM%04X vus: Folge 7 8 9 10", r110);
            PRUEFE(L.n_msgs >= 1 && L.msgs[0] == 1 && L.cut_bei[0] == 10, "ROOM%04X vus: msg 1 mit Cut 10", r110);
            pruefe_zurueck(r110, "vus", &L, 6);
        } else PRUEFE(0, "1100 Parse");

        /* panel: Platz 1 (-2200,-15850,800,1400), Blick -x; Nein, dann Ja */
        if (raum_hoch(r11f, -900, -15150) == 0) {
            L = druecken(-1180, -15150, 2048, 1); zeige("nein", r11f, &L);
            PRUEFE(L.n_msgs >= 2 && L.msgs[0] == 0 && L.cut_bei[0] == 12 && L.msgs[1] == 1 && L.cut_bei[1] == 12,
                   "ROOM%04X panel nein: msg 0/1 mit Cut 12", r11f);
            PRUEFE(re15_game_flag_get(5, 0) == 0, "ROOM%04X panel nein: Raetsel nicht gestartet", r11f);
            pruefe_zurueck(r11f, "nein", &L, L.cut_vorher);
        } else PRUEFE(0, "11F0 Parse");
        if (raum_hoch(r11f, -900, -15150) == 0) {
            L = druecken(-1180, -15150, 2048, 0); zeige("ja", r11f, &L);
            PRUEFE(L.n_msgs >= 2 && L.cut_bei[0] == 12 && L.cut_bei[1] == 12, "ROOM%04X panel ja: msg 0/1 mit Cut 12", r11f);
            PRUEFE(g_scd.cam_id == 10 && g_scd.work_vars[0x0A] == 10 && re15_game_flag_get(5, 0) == 1,
                   "ROOM%04X panel ja: Raetselbuehne Cut 10 (cam %d, (5,0)=%d)", r11f, g_scd.cam_id, re15_game_flag_get(5, 0));
            PRUEFE(g_scd.cam_id_prev == L.cut_vorher,
                   "ROOM%04X panel ja: gemerkter Cut %d statt Raum-Cut %d (sub17 Cut_old)", r11f, g_scd.cam_id_prev, L.cut_vorher);
        } else PRUEFE(0, "11F0 Parse");
    }

    /* fremd: ROOM1120 unberuehrt */
    if (raum_hoch(0x1120, 2000, -1000) == 0) {
        int n = 0;
        for (int s = 0; s < RE15_AOT_MAX; s++)
            if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[s].event_id == RE15_N40_EREIGNIS) n++;
        PRUEFE(n == 0, "ROOM1120: %d Ereignis-31-Plaetze", n);
    }

    printf("%s (%d Pruefungen)\n", s_fail ? "FAIL" : "PASS", s_pruef);
    free(s_daten);
    return s_fail;
}
