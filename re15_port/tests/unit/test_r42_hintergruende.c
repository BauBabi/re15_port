/*
 * test_r42_hintergruende.c — Runde 42 "rest", Stufe 2: nie gezeigte Kameras beim Untersuchen.
 * Herleitung: include/re15_hintergruende_r42.h (10F Funkkonsole Cut 10 / Regal Cut 9, 1010 Memory Card Cut 7).
 * D4 (6031 Elza Cut 11) prueft test_r42_story6 wagen6031.
 *
 * ECHTER WEG je Raum (Leon- und Elza-Fassung): RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter = Installer-Block),
 * Spieler vor die Stelle, Blick hin, Aktion -> re15_aot_scan (Vorwaertspunkt 620) -> scd_event_fire -> Programm; je Bild in
 * der Reihenfolge von main.c (SCD-Takt nur ohne Aufnahme-Modal, main.c:5343) re15_cam_present_tick (angezeigter Cut =
 * work_vars[0x0A]). Geprueft:
 *   funk    10F0/10F1 Platz 3 (Nordwand) und Platz 2 (Ostwand): msg 0 mit Cut 10; danach Raum-Cut, Cut_auto an, (2,7)=0.
 *   regal   10F0/10F1 Platz 6: msg 2 mit Cut 9; zurueck.
 *   karte   1010/1011: Aufnahme-Frage der Memory Card mit Cut 7. "Nein" -> Karte bleibt, Platz 11 bleibt scharf; nochmal ->
 *           wieder Cut 7, "Ja" -> Bit (9,80), Karte im Inventar, Platz 11 sce 0; zurueck auf den Raum-Cut.
 *   bytes   Message_on der Programme = msg/Pause-Halbwort des Original-Satzes; Karten-Waechter = ROOM1051 sub01 @0x00CB2.. (nur
 *           wenn das Modul im Baum ist, sonst uebersprungen - die Gegenprobe auf aaea5f8c2 laeuft ohne Modul).
 *   fremd   ROOM1020 bekommt nichts.
 * Aufruf: test_r42_hintergruende [funk|regal|karte|bytes|fremd|alle]
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#if defined(__has_include)
#  if __has_include("re15_hintergruende_r42.h")
#    include "re15_hintergruende_r42.h"
#    define HG42_MODUL 1
#  endif
#endif

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
#define PRUEFE(cond, ...) do { s_pruef++; if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fail = 1; } \
                               else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

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
static int s_roy_vorbei = 0;   /* 1011: Elzas Ankunftsszene mit Roy gilt als gesehen (nach scd_vm_init setzen, das die Flags wischt) */

/* Raumaufbau wie test_r40_nahaufnahmen.c raum_hoch (Installer-Block ueber scd_room_reenter). */
static int raum_hoch(uint16_t raum, int32_t x, int32_t z)
{
    char pfad[256];
    long sz = 0;
    snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/STAGE%u/ROOM%04X.RDT", (unsigned)(raum >> 12), raum);
    free(s_daten);
    s_daten = slurp(pfad, &sz);
    if (!s_daten || re15_rdt_parse(s_daten, (size_t)sz, &s_rdt) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    if (s_roy_vorbei && raum == 0x1011) { re15_game_flag_set(4, 241, 1); re15_game_flag_set(4, 250, 1); }
    /* 10F0/10F1: die Ankunftsszenen (cut_10f0.c Ereignis 20 bei (9,71)=0, funk10f1_r39.c Ereignis 27 bei (9,70)=0) sind gesehen -
     * sonst laeuft die Szene im Hintergrund und schaltet selbst Kameras (re15_cut10f0.h:65-66, re15_funk10f1_r39.h:37-38) */
    if (raum == 0x10F0) re15_game_flag_set(9, 71, 1);
    if (raum == 0x10F1) re15_game_flag_set(9, 70, 1);
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
    int modal_n, modal_cut[4]; /* Aufnahme-Modal: wie oft geoeffnet, angezeigter Cut dabei */
    int cuts[16], n_cuts;      /* Folge der angezeigten Cuts (Wechsel) */
    int cut_vorher;
} lauf_t;

/* Spieler an (x,z,rot), Aktion, dann Bilder. nein = an der ersten Ja/Nein-Frage (Text oder Aufnahme-Modal) "No". */
static lauf_t druecken(int32_t x, int32_t z, int rot, int nein)
{
    lauf_t L; memset(&L, 0, sizeof L);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = x; p->z = z; p->rot_y = (int16_t)rot;
    /* am Standort einschwingen: die RVD-Zonen stellen die Raum-Kamera wie beim Hingehen */
    for (int i = 0; i < 30; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); (void)re15_cam_present_tick(); }
    L.cut_vorher = g_scd.work_vars[0x0A];
    L.cuts[L.n_cuts++] = L.cut_vorher;
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    int offen = -1, modal_vorher = 0, umgeschaltet = 0;
    for (int i = 0; i < 4000; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (g_scd.message_id != offen && L.n_msgs < 8) {
                offen = g_scd.message_id;
                L.cut_bei[L.n_msgs] = g_scd.work_vars[0x0A];
                L.msgs[L.n_msgs++] = offen;
            }
            if ((i & 7) == 7 && (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 ||
                                 g_scd.message_fsm == 4 || g_scd.message_fsm == 7))
                g_scd_pad_edge = 0x4000;
        } else offen = -1;
        re15_msg_tick(0, 0, 0);
        g_scd_pad_edge = 0;
        if (!re15_item_modal_active()) scd_vm_tick();      /* main.c: SCD-Takt steht waehrend des Modals */
        re15_aot_scan(p->x, p->z, 0xFF);
        (void)re15_cam_present_tick();
        const int modal = re15_item_modal_active();
        if (modal && !modal_vorher && L.modal_n < 4) L.modal_cut[L.modal_n++] = g_scd.work_vars[0x0A];
        modal_vorher = modal;
        if (modal) {
            uint16_t e = 0;
            if (re15_item_modal_prompt_ready() && (i & 7) == 7) {
                if (nein && !umgeschaltet) { e = 0x1000; umgeschaltet = 1; }   /* & 0x3000 schaltet Ja/Nein */
                else e = 0x4000;                                               /* & 0x4000 bestaetigt */
            }
            re15_item_modal_tick(e, 0);
        }
        if (L.n_cuts < 16 && g_scd.work_vars[0x0A] != L.cuts[L.n_cuts - 1]) L.cuts[L.n_cuts++] = g_scd.work_vars[0x0A];
        int laeuft = g_scd.message_active || re15_item_modal_active();
        for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++)
            if (g_scd.threads[k].active) laeuft = 1;
        if (!laeuft && i > 30) break;
    }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return L;
}

static void zeige(const char *was, uint16_t raum, const lauf_t *L)
{
    printf("  ROOM%04X %-7s msgs:", raum, was);
    for (int i = 0; i < L->n_msgs; i++) printf(" %d@Cut%d", L->msgs[i], L->cut_bei[i]);
    printf("  Modal:");
    for (int i = 0; i < L->modal_n; i++) printf(" @Cut%d", L->modal_cut[i]);
    printf("  Cuts:");
    for (int i = 0; i < L->n_cuts; i++) printf(" %d", L->cuts[i]);
    printf("  Ende cam=%d auto=%d (2,7)=%d\n", g_scd.cam_id, g_scd.cut_auto_enabled, re15_game_flag_get(2, 7));
}

static void pruefe_zurueck(uint16_t raum, const char *was, int soll)
{
    PRUEFE(g_scd.cam_id == soll && g_scd.work_vars[0x0A] == soll && g_scd.cut_auto_enabled == 1 && re15_game_flag_get(2, 7) == 0,
           "ROOM%04X %s: Ende cam=%d angezeigt=%d auto=%d (2,7)=%d, erwartet Raum-Cut %d", raum, was, g_scd.cam_id,
           g_scd.work_vars[0x0A], g_scd.cut_auto_enabled, re15_game_flag_get(2, 7), soll);
}

/* ---- funk / regal: 10F0/10F1 ------------------------------------------------------------------------------------------ */
static void teil_funk(void)
{
    printf("== funk (10F Cut 10, msg 0) ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t r = (uint16_t)(0x10F0 + v);
        lauf_t L;
        /* Platz 3 Nordwand (900,12200,6000,800): Stand (3900,11700), Blick +z -> 620 voraus z 12320 */
        if (raum_hoch(r, 3900, 11700) != 0) { PRUEFE(0, "ROOM%04X Parse", r); continue; }
        L = druecken(3900, 11700, 3072, 0); zeige("nord", r, &L);
        PRUEFE(L.n_msgs >= 1 && L.msgs[0] == 0 && L.cut_bei[0] == 10, "ROOM%04X Platz 3 (Nordwand): msg 0 mit Cut 10", r);
        pruefe_zurueck(r, "nord", L.cut_vorher);
        /* Platz 2 Ostwand (7000,2600,800,9700): Stand (6500,10000), Blick +x -> 620 voraus x 7120 */
        if (raum_hoch(r, 6500, 10000) != 0) { PRUEFE(0, "ROOM%04X Parse", r); continue; }
        L = druecken(6500, 10000, 0, 0); zeige("ost", r, &L);
        PRUEFE(L.n_msgs >= 1 && L.msgs[0] == 0 && L.cut_bei[0] == 10, "ROOM%04X Platz 2 (Ostwand): msg 0 mit Cut 10", r);
        pruefe_zurueck(r, "ost", L.cut_vorher);
    }
}

static void teil_regal(void)
{
    printf("== regal (10F Cut 9, msg 2) ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t r = (uint16_t)(0x10F0 + v);
        /* Platz 6 (-3600,-1400,3200,800): Stand (-2000,0), Blick -z -> 620 voraus z -620 */
        if (raum_hoch(r, -2000, 0) != 0) { PRUEFE(0, "ROOM%04X Parse", r); continue; }
        lauf_t L = druecken(-2000, 0, 1024, 0); zeige("regal", r, &L);
        PRUEFE(L.n_msgs >= 1 && L.msgs[0] == 2 && L.cut_bei[0] == 9, "ROOM%04X Platz 6 (Regal): msg 2 mit Cut 9", r);
        pruefe_zurueck(r, "regal", L.cut_vorher);
    }
}

/* ---- karte: 1010/1011 ------------------------------------------------------------------------------------------------- */
static int inv_menge(uint8_t id)
{
    int m = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) m += g_inv.slots[i].qty;
    return m;
}

static void teil_karte(void)
{
    printf("== karte (1010 Cut 7, Memory Card) ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t r = (uint16_t)(0x1010 + v);
        /* 1011: Elzas Ankunftsszene mit Roy (sub00 @0x009E8 Ck(4,241)=0 -> sub02) ist vorbei - sub02 @0x00AFE/@0x00B02 setzt
         * (4,241) und (4,250); danach legt sub00 @0x00A30 nur Roy hin (sub05). raum_hoch setzt sie nach scd_vm_init. */
        s_roy_vorbei = 1;
        /* Stand vor dem Regal wie test_r35_inhalt_items (3300,-1033), Blick +x -> 620 voraus x 3920 */
        if (raum_hoch(r, 3300, -1033) != 0) { PRUEFE(0, "ROOM%04X Parse", r); continue; }
        lauf_t L = druecken(3300, -1033, 0, 1); zeige("nein", r, &L);
        PRUEFE(L.modal_n == 1 && L.modal_cut[0] == 7, "ROOM%04X Nein: Aufnahme-Frage mit Cut 7 (Modal %d, Cut %d)", r, L.modal_n,
               L.modal_n ? L.modal_cut[0] : -1);
        PRUEFE(re15_game_flag_get(9, 80) == 0 && inv_menge(0x21) == 0, "ROOM%04X Nein: Bit (9,80)=%d, Karte im Inventar %d", r,
               re15_game_flag_get(9, 80), inv_menge(0x21));
        pruefe_zurueck(r, "nein", L.cut_vorher);
        L = druecken(3300, -1033, 0, 0); zeige("ja", r, &L);
        PRUEFE(L.modal_n == 1 && L.modal_cut[0] == 7, "ROOM%04X Ja: Aufnahme-Frage wieder mit Cut 7", r);
        PRUEFE(re15_game_flag_get(9, 80) == 1 && inv_menge(0x21) > 0, "ROOM%04X Ja: Bit (9,80)=%d, Karte im Inventar %d", r,
               re15_game_flag_get(9, 80), inv_menge(0x21));
        PRUEFE(g_aot.slots[11].type != RE15_AOT_TYPE_GENERIC, "ROOM%04X Ja: Platz 11 stumm (Typ %d, Form ROOM1051 @0x00CBA)", r,
               g_aot.slots[11].type);
        pruefe_zurueck(r, "ja", L.cut_vorher);
        /* nach dem Nehmen: dieselbe Stelle loest nichts mehr aus */
        L = druecken(3300, -1033, 0, 0);
        PRUEFE(L.modal_n == 0 && L.n_cuts == 1, "ROOM%04X danach: kein Modal, kein Schnitt (Modal %d, Wechsel %d)", r, L.modal_n,
               L.n_cuts - 1);
        re15_game_flag_set(9, 80, 0);
    }
}

/* ---- bytes ------------------------------------------------------------------------------------------------------------ */
static void teil_bytes(void)
{
    printf("== bytes ==\n");
#ifdef HG42_MODUL
    int n = 0;
    for (int v = 0; v < 2; v++) {
        const uint16_t r = (uint16_t)(0x10F0 + v);
        if (raum_hoch(r, 0, 0) != 0) { PRUEFE(0, "ROOM%04X Parse", r); continue; }
        const uint8_t *f = re15_hintergruende_r42_programm("funk", &n);
        /* Aot_set-Satz: pc[14..15] msg, pc[16..17] Pause-Halbwort (Platz 3 @0x00F86, Platz 2 @0x00F72) */
        PRUEFE(f && f[6] == 0x2b && f[7] == s_daten[0x0F86 + 14] && f[8] == s_daten[0x0F86 + 16] && f[9] == s_daten[0x0F86 + 17] &&
               !memcmp(s_daten + 0x0F72 + 14, s_daten + 0x0F86 + 14, 4),
               "ROOM%04X funk: Message_on = Satz @0x00F86/@0x00F72 (msg %d, Pause %02x%02x)", r, s_daten[0x0F86 + 14],
               s_daten[0x0F86 + 17], s_daten[0x0F86 + 16]);
        const uint8_t *g = re15_hintergruende_r42_programm("regal", &n);
        PRUEFE(g && g[6] == 0x2b && g[7] == s_daten[0x0FC2 + 14] && g[8] == s_daten[0x0FC2 + 16] && g[9] == s_daten[0x0FC2 + 17],
               "ROOM%04X regal: Message_on = Satz @0x00FC2 (msg %d)", r, s_daten[0x0FC2 + 14]);
    }
    if (raum_hoch(0x1051, 0, 0) == 0) {
        const uint8_t *k = re15_hintergruende_r42_programm("karte", &n);
        /* Waechter ROOM1051 sub01 @0x00CB2..@0x00CC5, nur das Bit-Byte @0x00CB8 (0xA5) ist das Karten-Bit */
        uint8_t w[20]; memcpy(w, s_daten + 0x0CB2, 20); w[6] = 80;
        PRUEFE(k && n > 30 && !memcmp(k + 10, w, 20), "karte: Waechter = ROOM1051 sub01 @0x00CB2..@0x00CC5 mit Bit 80");
        PRUEFE(k && k[6] == 0x47 && s_daten[0x0DBC] == 0x47 && k[8] == s_daten[0x0DBE],
               "karte: Aot_on + Evt_next = ROOM1051 sub03 @0x00DBC/@0x00DBE");
    } else PRUEFE(0, "ROOM1051 Parse");
#else
    printf("  (Modul nicht im Baum - uebersprungen)\n");
#endif
}

/* ---- fremd ------------------------------------------------------------------------------------------------------------ */
static void teil_fremd(void)
{
    printf("== fremd ==\n");
    if (raum_hoch(0x1020, 0, 0) != 0) { PRUEFE(0, "ROOM1020 Parse"); return; }
    int n = 0;
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC &&
            (g_aot.slots[s].event_id == 30 || g_aot.slots[s].event_id == 31)) n++;
    PRUEFE(n == 0, "ROOM1020: %d Ereignis-30/31-Plaetze", n);
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "alle";
    const int alle = !strcmp(t, "alle");
    if (alle || !strcmp(t, "funk"))  teil_funk();
    if (alle || !strcmp(t, "regal")) teil_regal();
    if (alle || !strcmp(t, "karte")) teil_karte();
    if (alle || !strcmp(t, "bytes")) teil_bytes();
    if (alle || !strcmp(t, "fremd")) teil_fremd();
    printf("%s (%d Pruefungen)\n", s_fail ? "FAIL" : "PASS", s_pruef);
    free(s_daten);
    return s_fail;
}
