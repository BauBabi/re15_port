/**
 * @file test_r35_raeume_ziel.c
 * @brief Runde 35 Spur H, Punkt 2 — Zielscheiben-Texte im Schiessstand ROOM1190/1191.
 *
 * NUTZER-VORGABE: Scheibe ganz links und 3. von links -> "This target has a surprisingly large number
 * of bullet holes."; die anderen beiden -> "This target does not have many bullet holes".
 * Welche Scheibe welche ist, ist aus der Kamera gemessen (include/re15_ziel1190.h): Slot 0 = ganz
 * links, Slot 2 = dritte von links.
 *
 * ECHTER WEG: ROOM1190.RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter, sub01-Reseed je
 * Bild), Spieler 620 vor die Schalter-Mitte des Slots, Aktion -> re15_aot_scan (stempelt work_vars[0])
 * -> derselbe Weiterweg wie game_step (fired_event_id -> scd_event_fire) -> VM-Bilder mit dem echten
 * Text-FSM. Drei Raumzustaende, wie das Raumskript sie unterscheidet:
 *   A  Strom aus   (4,243)=0: sub01 retypt die Plaetze auf sce 3 -> sub12: msg 0, nach "Ja" msg 3
 *   B  Strom an    (4,243)=1, (4,234)=0: sce 5 -> sub02..05: msg 0, "Ja" faehrt die Scheibe ((4,n))
 *   C  nach Hunden (4,234)=1: sce 1 msg 2 ("I have nothing else to do here.")
 * Je Slot und Zustand gemessen: welche Nachricht aufgeht (6 = viele, 7 = wenige), ihr Text (Seite 1 =
 * Nutzer-Satz, Seite 2 = Original-Satz), dass die Original-Mechanik danach unveraendert laeuft
 * (A: msg 3 folgt; B: (4,n) kippt, Scheibe faehrt; C: kein Folgeereignis), und dass in ROOM1191 dasselbe
 * passiert. Gegenprobe: ohne Stempel (Harness re15_aot_fire_slot) bleibt die Original-Nachricht.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_ziel1190.h"

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

/* Schalter-Mitte je Slot (main00 Aot_set: x -4400 + 300, z-Kante + 1200). Spieler 620 davor (Ost),
 * Blick West (rot 2048): Vorwaertspunkt x - 620 (FUN_80042bac `ori 0x26c` @0x80042bd0). */
static const int32_t k_mitte_z[4] = { -24500, -20800, -17300, -13700 };

static int raum_hoch(uint16_t raum, uint8_t *daten, long sz, re15_rdt_t *r, int strom, int hunde)
{
    if (re15_rdt_parse(daten, (size_t)sz, r) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    re15_game_flag_set(4, 243, (uint8_t)strom);
    re15_game_flag_set(4, 234, (uint8_t)hunde);
    scd_register_current_rdt(r);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(r->messages, r->messages_size);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = -4100 + 620; p->y = 0; p->z = k_mitte_z[0]; p->rot_y = 2048; p->state = 1;
    g_scd.player_mode = 0;
    scd_register_room_events(r);
    scd_room_reenter(r, p->x, p->z, 0);
    for (int i = 0; i < 60; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); }
    return 0;
}

typedef struct {
    int erste_id;          /* erste offene Nachricht */
    char text[200];        /* ihr Klartext */
    int choice;            /* ist eine Ja/Nein-Frage */
    int msg3;              /* msg 3 "No response..." danach gesehen */
    int bilder;
} lauf_t;

/* Aktion an Slot s, danach Bilder: Text-FSM mit "Weiter" (0x4000) an Seitenumbruch/Ende, an der Frage
 * die Antwort ja (0x4000 bei Auswahl 0 = YES, Ck(12,31,0)). */
static lauf_t druecken(int s, int harness)
{
    lauf_t L; memset(&L, 0, sizeof L); L.erste_id = -1; L.msg3 = 0;
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->z = k_mitte_z[s];
    g_aot.fired_event_id_this_frame = 0;
    if (harness) {
        extern void re15_aot_fire_slot(int slot);
        re15_aot_fire_slot(s);                         /* KEIN Stempel (Aot_on-Weg) */
    } else {
        g_aot_action_pressed = 1;
        re15_aot_scan(p->x, p->z, 0xFF);
        g_aot_action_pressed = 0;
    }
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    for (int i = 0; i < 1500; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (L.erste_id < 0) {
                L.erste_id = g_scd.message_id;
                const char *t = re15_msg_get_text(g_scd.message_id);
                snprintf(L.text, sizeof L.text, "%s", t ? t : "");
                L.choice = re15_msg_is_choice(g_scd.message_id);
            }
            if (g_scd.message_id == 3) L.msg3 = 1;
            if ((i & 7) == 7 && (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 ||
                                 g_scd.message_fsm == 4 || g_scd.message_fsm == 7))
                g_scd_pad_edge = 0x4000;
        }
        re15_msg_tick(0, 0, 0);
        g_scd_pad_edge = 0;
        scd_vm_tick();
        re15_aot_scan(p->x, p->z, 0xFF);
        L.bilder++;
        int laeuft = g_scd.message_active;
        for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++)
            if (g_scd.threads[k].active) laeuft = 1;
        if (!laeuft && i > 30) break;
    }
    return L;
}

static const char *k_viele  = "This target has a surprisingly";
static const char *k_viele2 = "large number of bullet holes.";
static const char *k_wenige = "This target does not have";
static const char *k_wenige2 = "many bullet holes.";

static void fall(uint16_t raum, uint8_t *daten, long sz, int strom, int hunde, const char *name,
                 const char *orig_satz, int orig_id)
{
    re15_rdt_t r;
    for (int s = 0; s < 4; s++) {
        if (raum_hoch(raum, daten, sz, &r, strom, hunde) != 0) { PRUEFE(0, "Parse"); return; }
        const int vorher_bank4 = re15_game_flag_get(4, (uint8_t)s);
        lauf_t L = druecken(s, 0);
        const int viele = (s == 0 || s == 2);
        const int soll = viele ? RE15_ZIEL1190_MSG_VIELE : RE15_ZIEL1190_MSG_WENIGE;
        printf("  ROOM%04X %-12s Slot %d: msg %d choice=%d msg3=%d (4,%d) %d->%d  \"%s\"\n",
               raum, name, s, L.erste_id, L.choice, L.msg3, s, vorher_bank4,
               re15_game_flag_get(4, (uint8_t)s), L.text);
        PRUEFE(L.erste_id == soll, "ROOM%04X %s Slot %d: Nachricht %d, erwartet %d", raum, name, s,
               L.erste_id, soll);
        PRUEFE(strstr(L.text, viele ? k_viele : k_wenige) && strstr(L.text, viele ? k_viele2 : k_wenige2),
               "ROOM%04X %s Slot %d: Scheiben-Satz fehlt: \"%s\"", raum, name, s, L.text);
        PRUEFE(strstr(L.text, orig_satz) != NULL,
               "ROOM%04X %s Slot %d: Original-Seite (msg %d) fehlt: \"%s\"", raum, name, s, orig_id, L.text);
        PRUEFE(strstr(L.text, viele ? k_wenige : k_viele) == NULL,
               "ROOM%04X %s Slot %d: falscher Satz", raum, name, s);
        if (orig_id == 0) {
            PRUEFE(L.choice, "ROOM%04X %s Slot %d: Ja/Nein-Frage verloren", raum, name, s);
            if (!strom) PRUEFE(L.msg3, "ROOM%04X %s Slot %d: nach Ja muss msg 3 (kein Strom) folgen",
                               raum, name, s);
            else PRUEFE(re15_game_flag_get(4, (uint8_t)s) != vorher_bank4,
                        "ROOM%04X %s Slot %d: nach Ja muss die Scheibe fahren ((4,%d) kippt, sub0%d)",
                        raum, name, s, s, s + 2);
        } else {
            PRUEFE(!L.choice, "ROOM%04X %s Slot %d: msg-2-Platz ist keine Frage", raum, name, s);
        }
        /* Gegenprobe: Aot_on-Weg ohne Stempel -> Original-Nachricht unveraendert */
        if (s == 1) {
            raum_hoch(raum, daten, sz, &r, strom, hunde);
            lauf_t G = druecken(s, 1);
            printf("  ROOM%04X %-12s Slot %d ohne Stempel: msg %d\n", raum, name, s, G.erste_id);
            PRUEFE(G.erste_id == orig_id || G.erste_id < 0,
                   "ROOM%04X %s: ohne work_vars-Stempel darf nur das Original kommen (%d)", raum, name,
                   G.erste_id);
        }
    }
}

int main(void)
{
    long sz0 = 0, sz1 = 0;
    uint8_t *d0 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", &sz0);
    uint8_t *d1 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1191.RDT", &sz1);
    if (!d0 || !d1) { printf("FAIL: RDT fehlt\n"); return 1; }

    /* Aufbau-Bytes: Glyphen gegen den Port-Dekoder, Seite 2 Byte fuer Byte = Original */
    {
        re15_rdt_t r; re15_rdt_parse(d0, (size_t)sz0, &r);
        re15_msg_clear_room_block();
        re15_msg_load_room_block(r.messages, r.messages_size);
        uint8_t b[192]; int on = 0;
        const unsigned char *o = re15_msg_get_raw(0, &on);
        int n = re15_ziel1190_bauen(0, 0, b, (int)sizeof b);
        PRUEFE(n > 0 && o && n == 2 + 60 + 2 + (on - 2), "Aufbau Slot 0: Laenge %d (orig %d)", n, on);
        PRUEFE(n > 0 && memcmp(b + n - (on - 2), o + 2, (size_t)(on - 2)) == 0,
               "Seite 2 muss Byte fuer Byte msg 0 sein");
        PRUEFE(n > 0 && b[2 + 60] == 0x02 && b[2 + 60 + 1] == 0x00, "Seitenumbruch 02 00 fehlt");
        PRUEFE(re15_ziel1190_viele(0) == 1 && re15_ziel1190_viele(2) == 1 &&
               re15_ziel1190_viele(1) == 0 && re15_ziel1190_viele(3) == 0 && re15_ziel1190_viele(4) < 0,
               "Zuordnung viele/wenige");
        PRUEFE(n <= 128, "PSX-Puffer MSG_RAW_LEN 128: %d", n);
    }

    fall(0x1190, d0, sz0, 0, 0, "Strom aus", "There's a switch here.", 0);
    fall(0x1190, d0, sz0, 1, 0, "Strom an", "There's a switch here.", 0);
    fall(0x1190, d0, sz0, 1, 1, "nach Hunden", "I have nothing else to do", 2);
    fall(0x1191, d1, sz1, 1, 0, "Strom an", "There's a switch here.", 0);
    fall(0x1191, d1, sz1, 1, 1, "nach Hunden", "I have nothing else to do", 2);

    free(d0); free(d1);
    if (s_fail) { printf("test_r35_raeume_ziel: FAIL\n"); return 1; }
    printf("test_r35_raeume_ziel: OK\n");
    return 0;
}
