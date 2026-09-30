/*
 * test_r34n_a_rolltor.c — RIEGEL fuer Spur A (Runde 34 Nacht):
 * "Rolltor ROOM1050: Sicherung einsetzen mit Nahansicht, Tor erst danach".
 *
 * Dossier: analysis/befunde_runde34_nacht/A_rolltor.md (§4 Soll-Zeitlinie, §5 Bauplan, §9 Umsetzung).
 * Modul:   engine/src/rolltor_1050.c + include/re15_rolltor.h, Haken in scd_vm.c
 *          (scd_event_fire -> re15_rolltor_ereignis, register_opcodes -> Opcode 0x62).
 *
 * Gefahren wird der ECHTE Weg: echter ROOM1050/1051-Aufbau (main00 + sub00 durch die VM), Aktion
 * am Schalter ueber re15_aot_scan (AOT 7, sub00 @0x0C22), dann scd_event_fire(2) — also genau der
 * Haken, den game_step_common.c im Spiel ruft —, dann Bild fuer Bild Nachrichten-FSM + VM.
 *
 * GEPRUEFT WIRD DAS ERGEBNIS, nicht nur das Fadenende (Sonden-Gegenprobe: ohne Opcode 0x62 laeuft
 * der Faden sauber zu Ende und verschluckt das Einsetzen STILL):
 *   A    ohne Sicherung, Ja     : Nahansicht Cut 7 genau waehrend msg 2, zurueck Cut 3, Tor zu,
 *                                 (9,63)=0, Schalter bleibt bedienbar
 *   A-N  ohne Sicherung, Nein   : nichts, kein Cut 7
 *   B    mit Sicherung, Ja/Ja   : msg 2 + msg 20 in Cut 7, Sicherung weg (Raster nachgerueckt),
 *                                 (9,63)=1, msg 21 in Cut 8, zurueck Cut 3, Tor NOCH zu
 *   B-N  mit Sicherung, Ja/Nein : Sicherung bleibt, (9,63)=0, kein Cut 8
 *   C    danach                 : scd_event_fire(2) startet den AUSGELIEFERTEN sub02 (PC im
 *                                 RDT-Puffer = 0x0CAC), Ja -> (3,121)=1, SCA-Zelle 19 frei
 *   W    Wiederbetreten         : neuer Raumaufbau mit (9,63)=1 -> sub02
 *   L    Laden/Speichern        : capture -> Zustand weg -> restore -> Raumaufbau -> sub02,
 *                                 Inventar ohne 0x40
 *   1051 (Elza)                 : A und B identisch
 *   Herkunft der Texte 20/21 (== ROOM2060 msg 4 @0x1855 / msg 5 @0x1875) und von msg 2 (@0x0ED2)
 *   PC-Schranke: 0x62 ausserhalb des Port-Programms = pc+1 (op_unknown), nichts entfernt
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_rolltor.h"
#include "re15_savedata.h"

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

static int s_fail = 0;
#define PRUEFE(cond, ...) do { if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fail = 1; } } while (0)

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

/* SCA-Zelle 19 (Rolltor-Sperre, Breite 5000 x=13000 z=-10600) je Partition: solide = u0 0xFF. */
static int zelle19(const re15_rdt_t *r, int frei)
{
    int n = 0, basis = 0;
    for (int g = 0; g < 5; g++) {
        int fi = basis + 19;
        basis += r->sca_rgn[g];
        if (fi >= r->sca_count) continue;
        if (!frei && r->sca[fi].u0 == 0xFF) n++;
        if (frei && r->sca[fi].u0 == 0 && r->sca[fi].floor == 0) n++;
    }
    return n;
}

/* Raum wie im Spiel hochfahren. neu = 1: frischer Spielstand (VM/Flags/Inventar leer); neu = 0:
 * Wiederbetreten (Flags + Inventar bleiben, wie beim Tuerwechsel: scd_room_reenter wischt g_scd,
 * nicht g_game.flags). Spieler 620 vor der Schalter-Mitte (17200,-8550), Blick Ost: der
 * Vorwaertspunkt (aot_common.c, FUN_80042bac `ori 0x26c` @0x80042bd0) landet genau im Rechteck. */
static int raum_hoch(uint16_t raum, uint8_t *daten, long sz, re15_rdt_t *r, int neu)
{
    if (re15_rdt_parse(daten, (size_t)sz, r) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    if (neu) { scd_vm_init(); re15_inv_init(); re15_inv_load_briefing(); }
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    scd_register_current_rdt(r);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(r->messages, r->messages_size);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = 17200 - 620; p->y = 0; p->z = -8550; p->rot_y = 0; p->state = 1;
    g_scd.player_mode = 0;
    scd_register_room_events(r);
    scd_room_reenter(r, p->x, p->z, 3);          /* Cut 3 = der Cut am Schalter (Dossier §3.2) */
    for (int i = 0; i < 60; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); }
    return 0;
}

/* Aktion am Schalter (Weg wie game_step_common.c) -> scd_event_fire(2). Liefert den Faden-Slot. */
static int ausloesen(const char *fall)
{
    const re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    PRUEFE(g_aot.fired_event_id_this_frame == RE15_ROLLTOR_EREIGNIS,
           "%s: Aktion am Schalter feuert Ereignis %d (erwartet 2)", fall,
           g_aot.fired_event_id_this_frame);
    int slot = scd_event_fire(RE15_ROLLTOR_EREIGNIS);
    PRUEFE(slot >= SCD_EVENT_SLOT_FIRST && slot <= SCD_EVENT_SLOT_LAST, "%s: kein Faden (slot %d)", fall, slot);
    return slot;
}

typedef struct {
    int bilder, frage, cut7, cut8, msg2, msg20, msg21, zurueck;
    int falscher_cut;        /* Bilder, in denen eine Nachricht im falschen Cut stand */
    int fremd;               /* PC ausserhalb der Plan-Positionen */
} lauf_t;

/* Opcode-Positionen des Plans (Evt_next schiebt um 1 -> das Nop dahinter gehoert dazu). */
static const int k_plan_ohne[] = { 0x00, 0x04, 0x05, 0x06, 0x0A, 0x0E, 0x12, 0x14, 0x18, 0x19,
                                   0x1A, 0x1C, 0x1E, 0x22, 0x24 };
static const int k_plan_mit[]  = { 0x00, 0x04, 0x05, 0x06, 0x0A, 0x0E, 0x12, 0x14, 0x18, 0x19,
                                   0x1A, 0x1E, 0x1F, 0x20, 0x24, 0x28, 0x2A, 0x2E, 0x30, 0x34,
                                   0x35, 0x36, 0x38, 0x3A, 0x3C, 0x40, 0x42 };

/* Bilder fahren, bis der Faden endet. Antworten je Nachricht (0 = keine Taste). */
static lauf_t fahren(int slot, uint16_t antw_schalter, uint16_t antw_sicherung, int mit_plan)
{
    lauf_t L; memset(&L, 0xFF, sizeof L); L.bilder = 0; L.falscher_cut = 0; L.fremd = 0;
    int plen = 0;
    const uint8_t *prog = mit_plan >= 0 ? re15_rolltor_programm(mit_plan, &plen) : NULL;
    const int *plan = mit_plan == 1 ? k_plan_mit : k_plan_ohne;
    int pn = mit_plan == 1 ? (int)(sizeof k_plan_mit / sizeof k_plan_mit[0])
                           : (int)(sizeof k_plan_ohne / sizeof k_plan_ohne[0]);
    scd_thread_t *t = &g_scd.threads[slot];
    for (int i = 0; i < 4000 && t->active; i++) {
        uint16_t sel = 0, wait = 0;
        if (g_scd.message_active && g_scd.message_id == 0)  sel = antw_schalter;
        if (g_scd.message_active && g_scd.message_id == RE15_ROLLTOR_MSG_FRAGE) sel = antw_sicherung;
        if (g_scd.message_active && g_scd.message_id != 0 && g_scd.message_id != RE15_ROLLTOR_MSG_FRAGE)
            wait = 0x4000;
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (g_scd.message_fsm == 3 && sel)  g_scd_pad_edge = sel;
            if (g_scd.message_fsm == 4 && wait) g_scd_pad_edge = wait;
        }
        re15_msg_tick(0, 0, 0);
        g_scd_pad_edge = 0;
        if (prog && t->active && t->pc) {
            long off = (long)(t->pc - prog);
            int ok = 0;
            for (int k = 0; k < pn; k++) if (plan[k] == off) ok = 1;
            if (!ok) L.fremd++;
        }
        scd_vm_tick();
        int b = L.bilder;
        if (g_scd.message_active) {
            int id = g_scd.message_id;
            if (id == 0 && g_scd.message_fsm == 3 && L.frage < 0) L.frage = b;
            if (id == 2 && L.msg2 < 0)  L.msg2 = b;
            if (id == RE15_ROLLTOR_MSG_FRAGE   && L.msg20 < 0) L.msg20 = b;
            if (id == RE15_ROLLTOR_MSG_BENUTZT && L.msg21 < 0) L.msg21 = b;
            /* Nahansicht genau waehrend der Texte: msg 2 und 20 in Cut 7, msg 21 in Cut 8. */
            if ((id == 2 || id == RE15_ROLLTOR_MSG_FRAGE) && g_scd.cam_id != 7) L.falscher_cut++;
            if (id == RE15_ROLLTOR_MSG_BENUTZT && g_scd.cam_id != 8) L.falscher_cut++;
        }
        if (g_scd.cam_id == 7 && L.cut7 < 0) L.cut7 = b;
        if (g_scd.cam_id == 8 && L.cut8 < 0) L.cut8 = b;
        if (L.cut7 >= 0 && g_scd.cam_id == 3 && L.zurueck < 0) L.zurueck = b;
        L.bilder++;
    }
    PRUEFE(!t->active, "Faden laeuft nach %d Bildern noch", L.bilder);
    return L;
}

static void zeige(const char *fall, const lauf_t *L, const re15_rdt_t *r)
{
    printf("  %-30s Frage@%-4d Cut7@%-4d msg2@%-4d msg20@%-4d Cut8@%-4d msg21@%-4d Cut3@%-4d Ende@%-4d"
           " | cam=%u auto=%u (2,7)=%d (3,121)=%d (9,63)=%d Fuse=%d Z19solide=%d fremd=%d\n",
           fall, L->frage, L->cut7, L->msg2, L->msg20, L->cut8, L->msg21, L->zurueck, L->bilder,
           (unsigned)g_scd.cam_id, (unsigned)g_scd.cut_auto_enabled, re15_game_flag_get(2, 7),
           re15_game_flag_get(3, 121), re15_game_flag_get(9, 63), re15_inv_find_item(0x40),
           zelle19(r, 0), L->fremd);
}

static void sicherung_geben(void)
{
    /* Briefing belegt 0..2; Sicherung auf 3, dahinter ein weiterer Gegenstand auf 4, damit das
     * Nachruecken (RE2 @0x80058634 jal 0x80069714 / Port re15_inv_compact) sichtbar wird. */
    g_inv.slots[3].id = 0x40; g_inv.slots[3].qty = 1; g_inv.slots[3].flags = 0;
    g_inv.slots[4].id = 0x21; g_inv.slots[4].qty = 1; g_inv.slots[4].flags = 0;
}

static const uint16_t JA   = 0x4000;             /* SELECT bestaetigen (msg_common.c) */
static const uint16_t NEIN = 0x1000 | 0x4000;    /* Links/Rechts schaltet, Bestaetigen im selben Bild
                                                  * wirkt erst nach dem Umschalten (case 3: lr vor act) */

/* Fall A / A-N (ohne Sicherung). */
static void fall_ohne(const char *name, uint16_t raum, const uint8_t *d, long sz, uint16_t antwort)
{
    uint8_t *k = (uint8_t *)malloc((size_t)sz); memcpy(k, d, (size_t)sz);   /* Sca_id_set schreibt */
    re15_rdt_t r;
    PRUEFE(raum_hoch(raum, k, sz, &r, 1) == 0, "%s: Raumaufbau", name);
    int slot = ausloesen(name);
    int plen = 0; const uint8_t *prog = re15_rolltor_programm(0, &plen);
    PRUEFE(g_scd.threads[slot].pc == prog, "%s: scd_event_fire startet NICHT das Port-Programm (Haken 1)", name);
    lauf_t L = fahren(slot, antwort, 0, 0);
    zeige(name, &L, &r);
    PRUEFE(L.fremd == 0, "%s: %d Bilder mit PC ausserhalb des Plans", name, L.fremd);
    PRUEFE(L.frage >= 0, "%s: Schalterfrage msg 0 fehlt", name);
    if (antwort == JA) {
        PRUEFE(L.cut7 >= 0 && L.msg2 >= 0, "%s: Nahansicht Cut 7 / msg 2 fehlt", name);
        PRUEFE(L.falscher_cut == 0, "%s: %d Bilder Text im falschen Cut", name, L.falscher_cut);
        PRUEFE(L.zurueck >= 0 && g_scd.cam_id == 3, "%s: nicht zurueck in Cut 3", name);
        PRUEFE(g_scd.cut_auto_enabled == 1, "%s: Auto-Kamera bleibt aus", name);
    } else {
        PRUEFE(L.cut7 < 0 && L.msg2 < 0, "%s: Nein zeigt trotzdem die Nahansicht", name);
    }
    PRUEFE(L.msg20 < 0 && L.msg21 < 0 && L.cut8 < 0, "%s: Einsetz-Teil ohne Sicherung", name);
    PRUEFE(!re15_game_flag_get(3, 121), "%s: Tor ist offen (3,121)=1 ohne Sicherung", name);
    PRUEFE(!re15_game_flag_get(9, 63), "%s: (9,63) gesetzt ohne Sicherung", name);
    PRUEFE(!re15_game_flag_get(2, 7), "%s: Sperre (2,7) bleibt stehen", name);
    PRUEFE(zelle19(&r, 0) == 5, "%s: SCA-Zelle 19 nicht mehr solide", name);
    PRUEFE(g_aot.slots[7].active && g_aot.slots[7].event_id == 2,
           "%s: Schalter (AOT 7) nicht mehr bedienbar", name);
    free(k);
}

/* Fall B / B-N (mit Sicherung). Laesst den Zustand stehen, wenn danach != NULL: dann folgen
 * C (zweiter Druck), W (Wiederbetreten) und L (Laden) auf diesem Stand. */
static void fall_mit(const char *name, uint16_t raum, const uint8_t *d, long sz, uint16_t antw2, int danach)
{
    uint8_t *k = (uint8_t *)malloc((size_t)sz); memcpy(k, d, (size_t)sz);
    re15_rdt_t r;
    PRUEFE(raum_hoch(raum, k, sz, &r, 1) == 0, "%s: Raumaufbau", name);
    sicherung_geben();
    int slot = ausloesen(name);
    int plen = 0; const uint8_t *prog = re15_rolltor_programm(1, &plen);
    PRUEFE(g_scd.threads[slot].pc == prog, "%s: scd_event_fire startet NICHT das Port-Programm MIT", name);
    lauf_t L = fahren(slot, JA, antw2, 1);
    zeige(name, &L, &r);
    PRUEFE(L.fremd == 0, "%s: %d Bilder mit PC ausserhalb des Plans", name, L.fremd);
    PRUEFE(L.msg2 >= 0 && L.msg20 > L.msg2, "%s: Reihenfolge msg 2 -> msg 20 verletzt", name);
    PRUEFE(L.falscher_cut == 0, "%s: %d Bilder Text im falschen Cut", name, L.falscher_cut);
    PRUEFE(g_scd.cam_id == 3 && g_scd.cut_auto_enabled == 1, "%s: nicht zurueck in Cut 3 / Auto", name);
    PRUEFE(!re15_game_flag_get(3, 121), "%s: Tor oeffnet schon beim Einsetzen", name);
    PRUEFE(zelle19(&r, 0) == 5, "%s: SCA-Zelle 19 nicht mehr solide", name);
    PRUEFE(!re15_game_flag_get(2, 7), "%s: Sperre (2,7) bleibt stehen", name);
    if (antw2 == JA) {
        /* DAS ERGEBNIS: Sicherung weg (Raster nachgerueckt), (9,63)=1, msg 21 in Cut 8. */
        PRUEFE(re15_inv_find_item(0x40) < 0, "%s: Sicherung noch im Inventar (Opcode 0x62?)", name);
        PRUEFE(g_inv.slots[3].id == 0x21 && g_inv.slots[4].id == 0,
               "%s: Raster nicht nachgerueckt (Platz 3=0x%02X, 4=0x%02X)", name,
               g_inv.slots[3].id, g_inv.slots[4].id);
        PRUEFE(re15_game_flag_get(9, 63), "%s: (9,63) nicht gesetzt", name);
        PRUEFE(L.cut8 > L.msg20 && L.msg21 >= 0, "%s: Cut 8 / msg 21 fehlt", name);
    } else {
        PRUEFE(re15_inv_find_item(0x40) == 3, "%s: Sicherung bei Nein verschwunden", name);
        PRUEFE(!re15_game_flag_get(9, 63), "%s: (9,63) bei Nein gesetzt", name);
        PRUEFE(L.cut8 < 0 && L.msg21 < 0, "%s: Cut 8 / msg 21 bei Nein", name);
    }

    if (danach) {
        /* C: zweiter Druck -> ausgelieferter sub02 (PC im RDT-Puffer @0x0CAC bzw. 1051 @0x0CC8). */
        long soll = (raum == 0x1050) ? 0x0CAC : 0x0CC8;
        int s2 = ausloesen("C danach");
        PRUEFE(g_scd.threads[s2].pc == k + soll,
               "C: zweiter Druck startet nicht den ausgelieferten sub02 @0x%04lX (PC-Versatz %ld)",
               soll, (long)(g_scd.threads[s2].pc - k));
        lauf_t C = fahren(s2, JA, 0, -1);
        zeige("C danach: ausgeliefert", &C, &r);
        PRUEFE(re15_game_flag_get(3, 121), "C: Tor oeffnet nach dem Einsetzen nicht");
        PRUEFE(zelle19(&r, 1) == 5, "C: SCA-Zelle 19 nicht frei");
        PRUEFE(C.cut7 < 0 && C.cut8 < 0, "C: Nahansicht nach dem Einsetzen");
    }
    free(k);
}

int main(void)
{
    long sz0 = 0, sz1 = 0, sz60 = 0, sz61 = 0;
    uint8_t *d1050 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1050.RDT", &sz0);
    uint8_t *d1051 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1051.RDT", &sz1);
    uint8_t *d2060 = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2060.RDT", &sz60);
    uint8_t *d2061 = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2061.RDT", &sz61);
    if (!d1050 || !d1051 || !d2060 || !d2061) { printf("FAIL: RDT nicht lesbar\n"); return 1; }
    printf("=== unit_r34n_a_rolltor: Rolltor ROOM1050/1051, Sicherung einsetzen ===\n");

    /* Herkunft: Texte 20/21 == ROOM2060/2061 msg 4 @0x1855 / msg 5 @0x1875; msg 2 in 1050/1051. */
    {
        int n20 = 0, n21 = 0;
        const uint8_t *m20 = re15_rolltor_meldung(RE15_ROLLTOR_MSG_FRAGE, &n20);
        const uint8_t *m21 = re15_rolltor_meldung(RE15_ROLLTOR_MSG_BENUTZT, &n21);
        PRUEFE(n20 == 32 && sz60 > 0x1855 + 32 && memcmp(m20, d2060 + 0x1855, 32) == 0,
               "msg 20 != ROOM2060 msg 4 @0x1855");
        PRUEFE(n21 == 29 && memcmp(m21, d2060 + 0x1875, 29) == 0, "msg 21 != ROOM2060 msg 5 @0x1875");
        /* ROOM2061 traegt dieselben Saetze — an ihrem eigenen Offset (Nachrichtensuche). */
        int f20 = 0, f21 = 0;
        for (long o = 0; o + 32 <= sz61; o++) if (memcmp(d2061 + o, m20, 32) == 0) f20++;
        for (long o = 0; o + 29 <= sz61; o++) if (memcmp(d2061 + o, m21, 29) == 0) f21++;
        PRUEFE(f20 == 1 && f21 == 1, "ROOM2061 msg 4/5 nicht gleich (%d/%d)", f20, f21);
        static const uint8_t msg2_kopf[] = { 0x04, 0x02, 0x25, 0x00, 0x4a, 0x41, 0x41, 0x40 };   /* "I need" */
        PRUEFE(memcmp(d1050 + 0x0ED2, msg2_kopf, sizeof msg2_kopf) == 0, "ROOM1050 msg 2 @0x0ED2");
        PRUEFE(memcmp(d1051 + 0x0E68, msg2_kopf, sizeof msg2_kopf) == 0, "ROOM1051 msg 2 @0x0E68");
        printf("  Herkunft: msg 20/21 == ROOM2060 @0x1855/@0x1875 (auch 2061), msg 2 @0x0ED2/@0x0E68\n");
    }

    fall_ohne("A    1050 ohne, Ja",   0x1050, d1050, sz0, JA);
    fall_ohne("A-N  1050 ohne, Nein", 0x1050, d1050, sz0, NEIN);
    fall_mit ("B    1050 mit, Ja/Ja",   0x1050, d1050, sz0, JA, 1);
    fall_mit ("B-N  1050 mit, Ja/Nein", 0x1050, d1050, sz0, NEIN, 0);
    fall_ohne("A    1051 ohne, Ja",   0x1051, d1051, sz1, JA);
    fall_mit ("B    1051 mit, Ja/Ja",   0x1051, d1051, sz1, JA, 1);

    /* W: Wiederbetreten nach dem Einsetzen — neuer Raumaufbau, Flags + Inventar bleiben. */
    {
        uint8_t *k = (uint8_t *)malloc((size_t)sz0); memcpy(k, d1050, (size_t)sz0);
        re15_rdt_t r;
        raum_hoch(0x1050, k, sz0, &r, 1);
        sicherung_geben();
        int s = ausloesen("W vorher");
        fahren(s, JA, JA, 1);
        PRUEFE(re15_game_flag_get(9, 63), "W: Einsetzen fehlgeschlagen");
        uint8_t *k2 = (uint8_t *)malloc((size_t)sz0); memcpy(k2, d1050, (size_t)sz0);
        re15_rdt_t r2;
        raum_hoch(0x1050, k2, sz0, &r2, 0);                  /* Wiederbetreten */
        PRUEFE(g_aot.slots[7].active && g_aot.slots[7].event_id == 2, "W: Schalter fehlt nach Wiederbetreten");
        int s2 = ausloesen("W");
        PRUEFE(g_scd.threads[s2].pc == k2 + 0x0CAC, "W: Wiederbetreten startet nicht sub02 @0x0CAC");
        printf("  W  Wiederbetreten mit (9,63)=1: Schalter -> ausgelieferter sub02 @0x0CAC\n");
        free(k); free(k2);
    }

    /* L: Laden/Speichern — capture nach dem Einsetzen, Zustand weg, restore, Raumaufbau. */
    {
        uint8_t *k = (uint8_t *)malloc((size_t)sz0); memcpy(k, d1050, (size_t)sz0);
        re15_rdt_t r;
        raum_hoch(0x1050, k, sz0, &r, 1);
        sicherung_geben();
        int s = ausloesen("L vorher");
        fahren(s, JA, JA, 1);
        re15_savedata_t sd;
        re15_savedata_capture(&sd, 0, 1);
        uint8_t *k2 = (uint8_t *)malloc((size_t)sz0); memcpy(k2, d1050, (size_t)sz0);
        re15_rdt_t r2;
        raum_hoch(0x1050, k2, sz0, &r2, 1);                  /* frischer Stand: Bit weg */
        sicherung_geben();
        PRUEFE(!re15_game_flag_get(9, 63), "L: frischer Stand traegt (9,63) noch");
        uint16_t rr = 0;
        PRUEFE(re15_savedata_restore(&sd, &rr) == 0, "L: restore");
        PRUEFE(re15_game_flag_get(9, 63), "L: (9,63) nach dem Laden weg");
        PRUEFE(re15_inv_find_item(0x40) < 0, "L: Sicherung nach dem Laden wieder im Inventar");
        uint8_t *k3 = (uint8_t *)malloc((size_t)sz0); memcpy(k3, d1050, (size_t)sz0);
        re15_rdt_t r3;
        raum_hoch(0x1050, k3, sz0, &r3, 0);
        int s2 = ausloesen("L");
        PRUEFE(g_scd.threads[s2].pc == k3 + 0x0CAC, "L: nach dem Laden startet nicht sub02 @0x0CAC");
        printf("  L  Laden (capture/restore): (9,63)=1, keine Sicherung, Schalter -> sub02 @0x0CAC\n");
        free(k); free(k2); free(k3);
    }

    /* PC-Schranke: 0x62 ausserhalb des Port-Programms = op_unknown (pc+1), nichts entfernt.
     * Bytes `62 | 02 | 00 | 01 00`: richtig -> pc+1 landet auf `02` Evt_next (Ertrag, PC = +2);
     * falsch (2-Byte-Opcode ueberall) -> Gegenstand 0x02 weg, `00` Nop, `01` Evt_end, Faden tot. */
    {
        static const uint8_t synth[] = { 0x62, 0x02, 0x00, 0x01, 0x00 };
        scd_register_current_rdt(NULL);   /* kein Raum: kein sub01-Reseed auf einen alten Puffer */
        g_current_room_id = 0x1050;
        scd_vm_init(); re15_inv_init(); re15_inv_load_briefing();
        g_inv.slots[5].id = 0x02; g_inv.slots[5].qty = 1;
        scd_thread_start(SCD_EVENT_SLOT_FIRST, synth);
        scd_vm_tick();
        const scd_thread_t *t = &g_scd.threads[SCD_EVENT_SLOT_FIRST];
        PRUEFE(t->active && t->pc == synth + 2, "PC-Schranke: 0x62 im RDT-Bytecode laeuft nicht wie op_unknown "
               "(aktiv=%d, PC+%ld)", t->active, t->active ? (long)(t->pc - synth) : -1L);
        PRUEFE(re15_inv_find_item(0x02) == 5, "PC-Schranke: 0x62 ausserhalb entfernt einen Gegenstand");
        printf("  PC-Schranke: 0x62 ausserhalb des Port-Programms = pc+1, Inventar unberuehrt\n");
    }

    free(d1050); free(d1051); free(d2060); free(d2061);
    printf(s_fail ? "RESULT: FAIL\n" : "RESULT: OK\n");
    return s_fail ? 1 : 0;
}
