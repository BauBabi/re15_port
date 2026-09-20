/* test_dormant_activation.c — PIN (Nutzer-Auftrag 2026-08-30 "Wenn schlafender Content
 * einfach und sinnvoll zu aktivieren ist, aktiviere ihn"):
 *  (1) ROOM1150: der Port ARMIERT den Record, den die Autoren selbst dafuer angelegt
 *      haben — Slot 1 @0x0D7E (`2c 01 00 31 ... ff 00 18 04 00 00`: sce=0 = abgeschaltet,
 *      Nutzlast aber unveraendert die Event-Form auf sub 4). Das Feuern von sub04
 *      (@0x0F96-0x10B6) holt Objekt 0 aus der Parkposition ueber dem Raum
 *      (main00 @0x0E00: y=-20324) auf Arbeitshoehe (Pos_set y=-305 @0x0FB4) und parkt
 *      es am Ende selbst zurueck.
 *      ⛔ GEAENDERT 2026-09-20 (Nutzer-Rueckfrage "wie triggert man das?"): bis dahin
 *      stand hier ein ERFUNDENER Record in Slot 60. Der war GEMESSEN unerreichbar
 *      (probe_irons_mittelmodell: 0 von 2816 Standort/Richtungs-Kombinationen; der
 *      grosse MESSAGE-Record Slot 4 @0x0DBA umschliesst den Bereich und verbraucht den
 *      Tastendruck) UND er ueberschrieb die vierte RVD-Kamerazone des Raums, die dort
 *      liegt (16 Zonen auf Slot 48..63, rdt_common.c:441). Mit Slot 1: 472 Treffer.
 *  (2) ROOM20A0: die Ambient-Effektschleife sub02 (@0x1BF0-0x1CB0, 11x Sce_espr_on
 *      Effekt-Id 6) laeuft ab Raum-Eintritt.
 * Die RDT-Bytes bleiben unangetastet (kein Asset-Patch) — nur Trigger kommen dazu. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_esp.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static re15_rdt_t s_rdt;

static int enter(const char *room, uint16_t rid)
{
    char rp[600]; size_t n = 0;
    snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, room);
    uint8_t *raw = slurp(rp, &n);
    if (!raw) return 0;
    if (re15_rdt_parse(raw, n, &s_rdt) < 0) { free(raw); return 0; }
    scd_vm_init();
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = rid; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

int main(void)
{
    printf("=== Schlafender Content: Aktivierungs-Pins (1150 sub04 / 20A0 sub02) ===\n");

    /* --- (1) ROOM1150: Trigger installiert + Szene hebt das Geraet --- */
    if (!enter("STAGE1/ROOM1150.RDT", 0x1150)) { printf("SKIP: 1150 fehlt\n"); return 77; }
    for (int f = 0; f < 30; f++) scd_vm_tick();
    /* Der AUTORISIERTE Record: Slot 1 @0x0D7E, armiert wie ein Aot_reset(1,3,…) es taete
     * (LAB_80040738) — Event 4, Rechteck und Bank unveraendert aus dem RDT-Record. */
    CHECK("Slot 1 (@0x0D7E) auf Event 4 armiert",
          g_aot.slots[1].active && g_aot.slots[1].event_id == 4);
    CHECK("Rechteck des Records unveraendert (Ecke -21800,-20000, Groesse 1500x3300)",
          g_aot.slots[1].x - g_aot.slots[1].half_w == -21800 &&
          g_aot.slots[1].x + g_aot.slots[1].half_w == -20300 &&
          g_aot.slots[1].z - g_aot.slots[1].half_h == -20000 &&
          g_aot.slots[1].z + g_aot.slots[1].half_h == -16700);
    CHECK("Slot 60 bleibt die RVD-Kamerazone des Raums (kein erfundener Record mehr)",
          g_aot.slots[60].active && g_aot.slots[60].type == RE15_AOT_TYPE_CAM_SWITCH);
    CHECK("Objekt 0 startet ueber dem Raum geparkt (main00 @0x0E00: y=-20324)",
          g_scd.prop_count > 0 && g_scd.props[0].y == -20324);
    CHECK("Raum-Slots 0-6 unangetastet (kein Kollisionsschaden)", g_aot.slots[4].active);
    /* ANHAENGE-FORM pc[5]=0xC0 (LAB_80040914 @0x80040a84): Objekt 1 und 2 haengen an
     * Objekt 0. Ohne das standen die beiden Deckelhaelften bei (0,0,0). */
    CHECK("Objekt 1 und 2 haengen an Objekt 0 (pc[5]=0xC0)",
          g_scd.prop_count >= 3 &&
          g_scd.props[1].parent_obj == 0 && g_scd.props[2].parent_obj == 0 &&
          g_scd.props[0].parent_obj == -1);
    scd_event_fire(4);                                   /* = der Examine-Fire */
    int seen_up = 0;
    for (int f = 0; f < 600; f++) {
        scd_vm_tick();
        if (g_scd.prop_count > 0 && g_scd.props[0].y == -305) seen_up = 1;
    }
    printf("  [1150] prop0.y nach Lauf: %ld (gesehen -305: %d)\n",
           (long)(g_scd.prop_count ? g_scd.props[0].y : 0), seen_up);
    CHECK("sub04 hebt das Geraet auf Arbeitshoehe (Pos_set y=-305 @0x0FB4)", seen_up);
    CHECK("und parkt es am Ende selbst zurueck (@0x109E: y=-20224)",
          g_scd.prop_count > 0 && g_scd.props[0].y == -20224);

    /* --- (1b) ROOM6020: Box-Trigger installiert; sub02-Fire oeffnet die BOX (Intercept
     *     auf msg 0) statt der Preview-Meldung, und die Mockup-Kamera (Cut 8 = das
     *     vorgerenderte "Item Storage"-Bild) wird auf den Gameplay-Cut zurueckgestellt --- */
    if (!enter("STAGE6/ROOM6020.RDT", 0x6020)) { printf("SKIP: 6020 fehlt\n"); return 77; }
    for (int f = 0; f < 10; f++) scd_vm_tick();
    CHECK("6020: Box-Trigger Slot 60 installiert (GENERIC, Event 2)",
          g_aot.slots[60].active && g_aot.slots[60].event_id == 2);
    {
        extern void re15_savepoint_set_cut(int cut);
        extern void re15_itembox_reset(void);
        extern int  re15_itembox_pending(void);
        re15_itembox_reset();
        re15_savepoint_set_cut(3);                       /* Gameplay-Cut im Fire-Moment */
        g_scd.cam_id = 3;
        scd_event_fire(2);                               /* = der Examine-Fire */
        for (int f = 0; f < 10 && !re15_itembox_pending(); f++) scd_vm_tick();
        CHECK("6020: sub02 oeffnet die BOX (Intercept, keine Preview-Meldung)",
              re15_itembox_pending());
        CHECK("6020: Mockup-Cut unterdrueckt — Kamera zurueck auf dem Gameplay-Cut",
              g_scd.cam_id == 3);
        re15_itembox_reset();
    }

    /* --- (2) ROOM20A0: Ambient-Schleife laeuft ab Eintritt --- */
    if (!enter("STAGE2/ROOM20A0.RDT", 0x20A0)) { printf("SKIP: 20A0 fehlt\n"); return 77; }
    for (int f = 0; f < 40; f++) scd_vm_tick();
    printf("  [20A0] fx aktiv: %d\n", re15_esp_fx_count());
    CHECK("Ambient-Effekte spawnen ab Raum-Eintritt (sub02, 11x Effekt-Id 6)",
          re15_esp_fx_count() > 0);

    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
