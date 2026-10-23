/*
 * test_r40_untersuchen1120.c — Runde 40 "untersuchen1120": Text-Plaetze ROOM1120/1121 msg 1/3/4.
 * Herleitung: include/re15_untersuchen_r40.h, analysis/untersuchungstexte_2026-10-23/README.md.
 *
 * ECHTER WEG je Raum (1120 Leon, 1121 Elza): RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter =
 * Installer-Block -> re15_untersuchen_r40_install), Spieler vor den Gegenstand, Blick hin, Aktion ->
 * re15_aot_scan (Vorwaertspunkt 620 @0x80042bd0, sce-1-Weg) -> Text-FSM. Geprueft wird:
 *   (A) Plaetze 5/6/7: sce-1, msg 1/3/4, Satzform @0x00CF8 (sat 0x31, Band 0, Pause 0xffff), Rechtecke.
 *   (B) Druecken vor Bild / Fenster / Aktenschrank oeffnet genau msg 1 / 3 / 4 mit dem RDT-Wortlaut.
 *   (C) Original unberuehrt: Telefon Platz 3 traegt msg 6 und oeffnet weiter den Save-Punkt, Tueren 0/2 bleiben.
 *   (D) Ein anderer Raum (ROOM1130) bekommt nichts.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_untersuchen_r40.h"

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

static int raum_hoch(uint16_t raum, uint8_t *daten, long sz, re15_rdt_t *r)
{
    if (re15_rdt_parse(daten, (size_t)sz, r) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    scd_register_current_rdt(r);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(r->messages, r->messages_size);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = 2000; p->y = 0; p->z = -1000; p->rot_y = 0; p->state = 1;   /* Raummitte, frei */
    g_scd.player_mode = 0;
    scd_register_room_events(r);
    scd_room_reenter(r, p->x, p->z, 0);
    for (int i = 0; i < 60; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return 0;
}

/* Spieler an (x,z,rot), Aktion, dann Bilder mit "Weiter" bis der Text zu ist. -> erste offene msg (-1 = keine). */
static int druecken(int32_t x, int32_t z, int rot, char *text, size_t cap)
{
    int erste = -1;
    text[0] = 0;
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = x; p->z = z; p->rot_y = (int16_t)rot;
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    for (int i = 0; i < 1500; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (erste < 0) {
                erste = g_scd.message_id;
                const char *t = re15_msg_get_text(g_scd.message_id);
                snprintf(text, cap, "%s", t ? t : "");
            }
            if ((i & 7) == 7 && (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 ||
                                 g_scd.message_fsm == 4 || g_scd.message_fsm == 7))
                g_scd_pad_edge = 0x4000;
        }
        re15_msg_tick(0, 0, 0);
        g_scd_pad_edge = 0;
        scd_vm_tick();
        re15_aot_scan(p->x, p->z, 0xFF);
        int laeuft = g_scd.message_active;
        for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++)
            if (g_scd.threads[k].active) laeuft = 1;
        if (!laeuft && i > 30) break;
    }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return erste;
}

/* Rechteck (Ecke + Ausdehnung) je Platz, wie im Kopf dokumentiert. */
static const struct { int slot, msg; int32_t x, z, w, d; const char *wort; } k_plaetze[3] = {
    { RE15_U40_SLOT_BILD,    RE15_U40_MSG_BILD,    -200,  1150, 1150, 1000, "Music Lesson" },
    { RE15_U40_SLOT_FENSTER, RE15_U40_MSG_FENSTER, 6150, -1050, 1000, 4900, "lights of the" },
    { RE15_U40_SLOT_AKTEN,   RE15_U40_MSG_AKTEN,   4600, -5800, 1350, 1850, "reports and case" },
};

/* Halteposition + Blick (Vorwaerts = (cos, -sin), rot 0 = +x, 3072 = +z): Punkt 620 voraus liegt im Rechteck,
 * der Spieler selbst steht davor, ausserhalb der Kollision (Suedwand z 1650, Ostwand x 6650, Stuhl Zelle 12). */
static const struct { int32_t x, z; int rot; } k_halt[3] = {
    {  375,  1000, 3072 },   /* vor dem Bild, Blick +z -> (375, 1620) */
    { 5900,  1000,    0 },   /* vor dem Fenster, Blick +x -> (6520, 1000) */
    { 4700, -5300,    0 },   /* vor dem Schrank, Blick +x -> (5320, -5300) */
};

static void fall_raum(uint16_t raum, const char *datei)
{
    long sz = 0;
    uint8_t *d = slurp(datei, &sz);
    if (!d) { PRUEFE(0, "%s fehlt", datei); return; }
    re15_rdt_t r;
    if (raum_hoch(raum, d, sz, &r) != 0) { PRUEFE(0, "ROOM%04X Parse", raum); free(d); return; }

    /* (A) Plaetze */
    for (int i = 0; i < 3; i++) {
        const re15_aot_t *a = &g_aot.slots[k_plaetze[i].slot];
        PRUEFE(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == k_plaetze[i].msg,
               "ROOM%04X Platz %d: aktiv=%d Typ=%d msg=%d", raum, k_plaetze[i].slot, a->active, a->type, a->event_id);
        PRUEFE(a->sce_flags == 0x31 && a->band == 0 && a->pause_mask16 == 0xFFFF,
               "ROOM%04X Platz %d Form sat=%02x band=%d pause=%04x", raum, k_plaetze[i].slot,
               a->sce_flags, a->band, a->pause_mask16);
        PRUEFE(a->x == k_plaetze[i].x + k_plaetze[i].w / 2 && a->z == k_plaetze[i].z + k_plaetze[i].d / 2 &&
               a->half_w == k_plaetze[i].w / 2 && a->half_h == k_plaetze[i].d / 2,
               "ROOM%04X Platz %d Rechteck Mitte (%d,%d) halb %d/%d", raum, k_plaetze[i].slot,
               a->x, a->z, a->half_w, a->half_h);
    }
    /* (C) Original-Plaetze */
    PRUEFE(g_aot.slots[3].active && g_aot.slots[3].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[3].event_id == 6,
           "ROOM%04X Telefon Platz 3 veraendert", raum);
    /* Platz 1: main00 legt erst den Aufzug-Text (@0x00C9E) und dann die Tuer (@0x00CB6) in denselben Platz. */
    for (int s = 0; s <= 4; s++)
        printf("  ROOM%04X Platz %d: aktiv=%d Typ=%d ereignis=%d\n", raum, s, g_aot.slots[s].active,
               g_aot.slots[s].type, g_aot.slots[s].event_id);
    PRUEFE(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_DOOR &&
           g_aot.slots[2].active && g_aot.slots[2].type == RE15_AOT_TYPE_DOOR, "ROOM%04X Tueren 0/2 veraendert", raum);

    /* (B) Druecken */
    for (int i = 0; i < 3; i++) {
        char text[256];
        const int id = druecken(k_halt[i].x, k_halt[i].z, k_halt[i].rot, text, sizeof text);
        printf("  ROOM%04X vor Platz %d: msg %d \"%s\"\n", raum, k_plaetze[i].slot, id, text);
        PRUEFE(id == k_plaetze[i].msg, "ROOM%04X Platz %d: msg %d statt %d", raum, k_plaetze[i].slot, id, k_plaetze[i].msg);
        PRUEFE(strstr(text, k_plaetze[i].wort) != NULL, "ROOM%04X Platz %d: Text ohne \"%s\"", raum,
               k_plaetze[i].slot, k_plaetze[i].wort);
    }
    /* (C) Telefon (m6 @0x00CF8: 3250..4250 / -4450..-3450), Blick +z von (3750,-5000): weiter der Save-Punkt
     * (re15_savepoint.c { 0x1120, 0x06, 2 }), keiner der neuen Texte. */
    {
        char text[256];
        const int id = druecken(3750, -5000, 3072, text, sizeof text);
        printf("  ROOM%04X Telefon: msg %d\n", raum, id);
        PRUEFE(id >= 0 && id != RE15_U40_MSG_BILD && id != RE15_U40_MSG_FENSTER && id != RE15_U40_MSG_AKTEN,
               "ROOM%04X Telefon: msg %d", raum, id);
    }
    free(d);
}

int main(void)
{
    fall_raum(RE15_U40_RAUM_LEON, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1120.RDT");
    fall_raum(RE15_U40_RAUM_ELZA, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1121.RDT");

    /* (D) anderer Raum */
    re15_aot_init();
    re15_untersuchen_r40_install(0x1130);
    for (int i = 0; i < 3; i++)
        PRUEFE(!g_aot.slots[k_plaetze[i].slot].active, "ROOM1130: Platz %d angelegt", k_plaetze[i].slot);

    printf("%s (%d Pruefungen)\n", s_fail ? "FAIL" : "PASS", s_pruef);
    return s_fail;
}
