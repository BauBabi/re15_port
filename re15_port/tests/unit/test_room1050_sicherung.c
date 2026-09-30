/**
 * @file test_room1050_sicherung.c
 * @brief HERKUNFTSMARKE + DURCHSPIELBARKEITS-RIEGEL fuer das herausgenommene
 *        Sicherungs-Raetsel in ROOM1050.
 *
 * NUTZER-BEFUND 2026-09-27: "ganz offensichtlich wurde in ROOM1050 Cut 7 ein Raetsel
 * herausgenommen, wo eine Sicherung eingesetzt werden muss, die Bilder/Cuts dafuer aber
 * bereits existieren."
 *
 * Dieser Haken nagelt die gemessenen Tatsachen der AUSGELIEFERTEN Dateien fest und sichert,
 * dass der Raum loesbar bleibt. Seit Runde 34 Nacht (Spur A) ist das Raetsel portseitig
 * WIEDERHERGESTELLT (engine/src/rolltor_1050.c, Riegel unit_r34n_a_rolltor): das Tor laeuft
 * erst, wenn die Sicherung eingesetzt ist. Alle Zahlen sind Datei-Byte-Offsets in den
 * AUSGELIEFERTEN RDTs unter shared_assets/PSX — sie aendern sich nie, weil der Port keine
 * Original-Assets patcht (Eingriffe laufen port-seitig, s. scd_vm.c scd_event_fire).
 *
 * WAS BELEGT IST (Dossier: analysis/befunde_2026-09-27/sicherung-verdrahtung.md):
 *
 *  1. ZWEI ZUSTAENDE EINES BLICKS. ROOM1050 Kameratabelle @0x60, 32 B je Cut
 *     (+0 flag u16, +2 const 0x683c, +4/+8/+12 Pos, +16/+20/+24 Ziel, +28 pri/bg-Offset):
 *     Cut 7 @0x1E0 und Cut 8 @0x200 sind in den ersten 28 Bytes IDENTISCH und
 *     unterscheiden sich nur im pri-Offset 0x518 / 0x51C.
 *     Der Bildinhalt: Cut 7 = rechter Sockel LEER + ROTE Leuchte, Cut 8 = zweite
 *     Sicherung eingesetzt + BEIDE GRUEN (gemessen ueber probe_bg_dump, Schwelle >4
 *     der Kanalabweichung: 2761 Pixel, bbox x123..268 y16..181).
 *
 *  2. DAS SCHWESTERRAETSEL ROOM2060 (Generator-Sicherung) hat dieselbe Form: die Paare
 *     5/11, 6/10, 8/9 sind je EIN Blick in zwei Zustaenden (28 Byte gleich, nur der
 *     pri-Offset verschieden), und es tauscht zwei davon beim Einsetzen:
 *       sub19 @0x0169E  22 03 90 01   Set(3,144,1)   "Sicherung eingesetzt"
 *       sub19 @0x016C2  4b 05 0b      Cut_replace 5,11
 *       sub19 @0x016C5  4b 06 0a      Cut_replace 6,10
 *       sub00 @0x010F2  4b 05 0b      dasselbe Paar beim WIEDERbetreten
 *       sub00 @0x010F5  4b 06 0a
 *     ⛔ KORRIGIERT (Runde 34 Nacht, Dossier A_rolltor.md §3.5): Cut_replace ist NICHT der
 *     Mechanismus der Nahansicht. Es etikettiert nur die RVD-ZONEN um (LAB_80040414
 *     @0x80040434 `lw a3,40(v0)` = RDT+0x28, Tauschschleife ueber cam_from/cam_to,
 *     Schritt 20 @0x80040498) — ROOM2060 tauscht damit seine NORMALEN Raumblicke 5/11
 *     und 6/10. Die Nahansicht selbst waehlt es mit `Cut_chg 8` (sub18 @0x0168C) und
 *     `Cut_chg 9` (sub19 @0x016A2). ROOM1050s Cut 7/8 sind reine Skript-Cuts (RVD nur
 *     Anker @0x32C/@0x340 auf einem Blindrechteck) -> Cut_replace 7,8 waere wirkungslos;
 *     der Port waehlt sie mit Cut_chg 7 / Cut_chg 8 (rolltor_1050.c).
 *
 *  3. DIE SICHERUNG IST IM SPIEL — als FLAG, nicht als Inventar-Gegenstand:
 *       ROOM2030 sub06 @0x01FE4  2b 02 ff ff   Message_on 2 "Will you take the Fuse? "
 *       ROOM2030 sub06 @0x01FEE  21 0c 1f 00   Ck(12,31,0) = Antwort JA
 *       ROOM2030 sub06 @0x01FF2  22 03 6c 01   Set(3,108,1) = "Sicherung genommen"
 *       ROOM2060 sub00 @0x010A2  21 03 6c 00   Ck(3,108,0)  = die einzige Abfrage
 *     Deshalb hat Item 0x40 "Fuse" 0 Platzierungen: es wird nie als Gegenstand vergeben.
 *
 *  4. IN ROOM1050 IST DAVON NICHTS VERDRAHTET: msg 2 "I need a fuse to run the shutter."
 *     liegt im Nachrichtenblock, aber im ganzen Raum gibt es kein `Message_on 2`
 *     (Opcode 0x2B, pc[1]==2) und kein Cut_chg/Cut_replace auf 7 oder 8.
 *
 *  5. DURCHSPIELBARKEIT (der harte Riegel): im Auslieferungsstand oeffnet der Schalter
 *     OHNE jede Bedingung. Seit Runde 34 Nacht haengt davor die Sicherung (Item 0x40,
 *     portseitige Fundstelle Hebetisch ROOM1150/1151, sicherung_1150.c, gepinnt von
 *     unit_r31_hebetisch / unit_r30_sicherung_*). Das wird hier LIVE gefahren, nicht
 *     behauptet: Raum hochfahren -> Aktion am AOT-Slot 7 -> Ja ->
 *       (6i)   OHNE Sicherung bleibt das Tor zu (flag(3,121)==0, Zelle 19 solide);
 *       (6ii)  MIT Sicherung: Einsetzen (Ja/Ja) -> flag(9,63)==1, Sicherung weg;
 *       (6iii) erneut Aktion -> ausgelieferter sub02 -> Ja -> flag(3,121)==1 und die
 *              SCA-Zelle 19 aller fuenf Partitionen ist frei (u0==0).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_rolltor.h"
#include "re15_sicherung.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

void scd_register_current_rdt(const re15_rdt_t *rdt);

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* Kameratabelle: RDT+0x60, 32 B je Cut, pri/bg-Offset bei +0x1C (dieselbe Lage, die
 * re15_port/tools/maske/geom.py:509 `cam + cut*32 + 0x1C` liest). */
#define CAM_BASE 0x60
static uint32_t cam_pri(const uint8_t *d, int cut)
{
    const uint8_t *p = d + CAM_BASE + cut * 32 + 0x1C;
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* "Zwei Zustaende EINES Blicks": 28 Byte gleich, nur der pri-Offset verschieden. */
static int paar_ist_ein_blick(const uint8_t *d, int a, int b, uint32_t pa, uint32_t pb,
                              const char *raum)
{
    const uint8_t *ra = d + CAM_BASE + a * 32;
    const uint8_t *rb = d + CAM_BASE + b * 32;
    if (memcmp(ra, rb, 28) != 0) {
        fprintf(stderr, "FAIL: %s Cut %d/%d — die ersten 28 Kamerabytes sind NICHT gleich\n",
                raum, a, b);
        return 1;
    }
    if (cam_pri(d, a) != pa || cam_pri(d, b) != pb) {
        fprintf(stderr, "FAIL: %s Cut %d/%d pri=0x%X/0x%X (erwartet 0x%X/0x%X)\n",
                raum, a, b, cam_pri(d, a), cam_pri(d, b), pa, pb);
        return 1;
    }
    printf("  %s Cut %2d/%-2d: 28 Byte identisch, pri 0x%03X / 0x%03X\n",
           raum, a, b, pa, pb);
    return 0;
}

static int bytes_bei(const uint8_t *d, long sz, long off, const uint8_t *soll, int n,
                     const char *was)
{
    if (off < 0 || off + n > sz || memcmp(d + off, soll, (size_t)n) != 0) {
        fprintf(stderr, "FAIL: %s — Bytes @0x%05lX stimmen nicht\n", was, off);
        return 1;
    }
    printf("  %s @0x%05lX ok\n", was, off);
    return 0;
}

/* SCA-Zelle 19 (Rolltor-Sperre) je Partition solide (u0=0xFF)? 5 = Tor zu. */
static int zelle19_solide(const re15_rdt_t *r)
{
    int n = 0, basis = 0;
    for (int g = 0; g < 5; g++) {
        int fi = basis + 19;
        basis += r->sca_rgn[g];
        if (fi < r->sca_count && r->sca[fi].u0 == 0xFF) n++;
    }
    return n;
}

/* Aktion am Schalter — derselbe Weg wie im Spiel (Scan -> Ereignis 2 -> scd_event_fire, der
 * Haken von Spur A) —, dann Bilder fahren: Nachrichten-FSM + VM; jede Frage mit Ja (0x4000 im
 * Auswahlzustand 3), jeder Text mit Bestaetigen (Wartezustand 4) beantworten. `min_bilder` haelt
 * die VM auch nach dem Ende des Fadens noch am Laufen (die Torfahrt von sub02). */
static int schalter_ja(const re15_actor_t *pl, int min_bilder)
{
    extern uint8_t  g_aot_action_pressed;
    extern uint16_t g_scd_pad_edge;
    extern uint16_t g_scd_pad_held;
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(pl->x, pl->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame != 2) {
        fprintf(stderr, "FAIL: Aktion am Schalter feuert Event %d (erwartet 2)\n",
                g_aot.fired_event_id_this_frame);
        return 1;
    }
    int slot = scd_event_fire(2);
    if (slot < 0) { fprintf(stderr, "FAIL: Ereignis 2 startet keinen Faden\n"); return 1; }
    int frage = 0;
    for (int i = 0; i < 4000 && (g_scd.threads[slot].active || i < min_bilder); i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active && (g_scd.message_fsm == 3 || g_scd.message_fsm == 4))
            g_scd_pad_edge = 0x4000;
        if (g_scd.message_active && g_scd.message_id == 0) frage = 1;
        re15_msg_tick(0, 0, 0);
        g_scd_pad_edge = 0;
        scd_vm_tick();
    }
    if (!frage) { fprintf(stderr, "FAIL: Schalterfrage msg 0 kam nicht\n"); return 1; }
    if (g_scd.threads[slot].active) { fprintf(stderr, "FAIL: Faden endet nicht\n"); return 1; }
    return 0;
}

/* Byte-Anker-Suche nach einem Item_aot_set-Record (Opcode 0x50, sce=9) mit dieser
 * Item-Id — ignoriert die Opcode-Struktur und kann deshalb nichts uebersehen.
 * Feldlage wie tools/scd_dump_room.py: +14 i_item, +16 n_item, +18 flag, +20 md1. */
static int item_records(const uint8_t *d, long sz, int id)
{
    int n = 0;
    for (long o = 0; o + 22 <= sz; o++) {
        if (d[o] != 0x50 || d[o + 2] != 9) continue;
        int it = d[o + 14] | (d[o + 15] << 8);
        if (it == id) n++;
    }
    return n;
}

int main(void)
{
    int fail = 0;
    long sz1050 = 0, sz2030 = 0, sz2060 = 0;
    uint8_t *d1050 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1050.RDT", &sz1050);
    uint8_t *d2030 = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2030.RDT", &sz2030);
    uint8_t *d2060 = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2060.RDT", &sz2060);
    if (!d1050 || !d2030 || !d2060) {
        fprintf(stderr, "FAIL: RDT nicht lesbar (1050/2030/2060)\n");
        return 1;
    }

    printf("=== ROOM1050 Sicherung: Herkunftsmarke + Durchspielbarkeit ===\n");

    /* (1) Das Cut-Paar 7/8 ist EIN Blick in zwei Zustaenden. */
    fail |= paar_ist_ein_blick(d1050, 7, 8, 0x518, 0x51C, "ROOM1050");

    /* (2) Dieselbe Form bei dem Raetsel, das RE1.5 fertig ausgeliefert hat. */
    fail |= paar_ist_ein_blick(d2060, 5, 11, 0x65C, 0x674, "ROOM2060");
    fail |= paar_ist_ein_blick(d2060, 6, 10, 0x660, 0x670, "ROOM2060");
    fail |= paar_ist_ein_blick(d2060, 8,  9, 0x668, 0x66C, "ROOM2060");

    /* (2b) …und ROOM2060 tauscht genau diese Paare per Cut_replace. */
    {
        static const uint8_t set144[]  = { 0x22, 0x03, 0x90, 0x01 };   /* Set(3,144,1) */
        static const uint8_t cr_5_11[] = { 0x4b, 0x05, 0x0b };
        static const uint8_t cr_6_10[] = { 0x4b, 0x06, 0x0a };
        fail |= bytes_bei(d2060, sz2060, 0x0169E, set144,  4, "ROOM2060 sub19 Set(3,144,1)");
        fail |= bytes_bei(d2060, sz2060, 0x016C2, cr_5_11, 3, "ROOM2060 sub19 Cut_replace 5,11");
        fail |= bytes_bei(d2060, sz2060, 0x016C5, cr_6_10, 3, "ROOM2060 sub19 Cut_replace 6,10");
        fail |= bytes_bei(d2060, sz2060, 0x010F2, cr_5_11, 3, "ROOM2060 sub00 Cut_replace 5,11");
        fail |= bytes_bei(d2060, sz2060, 0x010F5, cr_6_10, 3, "ROOM2060 sub00 Cut_replace 6,10");
    }

    /* (3) Die einzige Fundstelle einer Sicherung im ganzen Spiel — ein FLAG. */
    {
        static const uint8_t msg2[]   = { 0x2b, 0x02, 0xff, 0xff };   /* Message_on 2 */
        static const uint8_t ck1231[] = { 0x21, 0x0c, 0x1f, 0x00 };   /* Ck(12,31,0) */
        static const uint8_t set108[] = { 0x22, 0x03, 0x6c, 0x01 };   /* Set(3,108,1) */
        static const uint8_t ck108[]  = { 0x21, 0x03, 0x6c, 0x00 };   /* Ck(3,108,0) */
        fail |= bytes_bei(d2030, sz2030, 0x01FE4, msg2,   4, "ROOM2030 sub06 Message_on 2");
        fail |= bytes_bei(d2030, sz2030, 0x01FEE, ck1231, 4, "ROOM2030 sub06 Ck(12,31,0)");
        fail |= bytes_bei(d2030, sz2030, 0x01FF2, set108, 4, "ROOM2030 sub06 Set(3,108,1)");
        fail |= bytes_bei(d2060, sz2060, 0x010A2, ck108,  4, "ROOM2060 sub00 Ck(3,108,0)");
    }

    /* (4) Item 0x40 "Fuse" hat im ausgelieferten Bestand KEINEN Ausgabe-Record —
     *     weder in ROOM1050 noch in den beiden Sicherungsraeumen. Die Sicherungs-
     *     Bedingung am Schalter setzt deshalb die PORTSEITIGE Fundstelle voraus
     *     (Hebetisch ROOM1150/1151, sicherung_1150.c); diese Original-Bytes bleiben
     *     unberuehrt. */
    {
        int n1 = item_records(d1050, sz1050, 0x40);
        int n2 = item_records(d2030, sz2030, 0x40);
        int n3 = item_records(d2060, sz2060, 0x40);
        if (n1 || n2 || n3) {
            fprintf(stderr, "FAIL: Item 0x40 Records 1050=%d 2030=%d 2060=%d (erwartet 0/0/0)\n",
                    n1, n2, n3);
            fail = 1;
        } else {
            printf("  Item 0x40 \"Fuse\": 0 Ausgabe-Records in 1050/2030/2060 (Byte-Anker)\n");
        }
    }

    /* (5) ROOM1050 msg 2 steht da, wird aber nie aufgerufen. */
    {
        re15_rdt_t rdt;
        if (re15_rdt_parse(d1050, (size_t)sz1050, &rdt) != 0) {
            fprintf(stderr, "FAIL: ROOM1050 RDT-Parse\n");
            return 1;
        }
        if (!rdt.messages || rdt.messages_size <= 0) {
            fprintf(stderr, "FAIL: ROOM1050 ohne Nachrichtenblock\n");
            fail = 1;
        } else {
            /* Nachrichtenblock @0xE44, Offsettabelle u16; msg 2 @0xED2 -> off 0x8E. */
            uint16_t off2 = (uint16_t)(rdt.messages[4] | (rdt.messages[5] << 8));
            if (off2 != 0x8E) {
                fprintf(stderr, "FAIL: ROOM1050 msg-Offset[2]=0x%X (erwartet 0x8E)\n", off2);
                fail = 1;
            } else {
                printf("  ROOM1050 msg 2 @0x%05X = \"I need a fuse to run the shutter.\"\n",
                       0xE44 + off2);
            }
        }
        /* Kein Message_on 2 im gesamten SCD-Bereich 0xAD8..0xE3C (main + sub). */
        {
            int treffer = 0;
            for (long o = 0x0AD8; o + 4 <= 0x0E3C; o++)
                if (d1050[o] == 0x2B && d1050[o + 1] == 0x02) treffer++;
            if (treffer != 0) {
                fprintf(stderr, "FAIL: ROOM1050 SCD enthaelt %d x `2b 02` — msg 2 waere "
                                "doch verdrahtet\n", treffer);
                fail = 1;
            } else {
                printf("  ROOM1050 SCD 0xAD8..0xE3C: 0 x Message_on 2 — im Auslieferungsstand "
                       "verwaist (portseitig: rolltor_1050.c)\n");
            }
        }
        /* Kein Cut_chg/Cut_replace auf 7 oder 8 im SCD. */
        {
            int treffer = 0;
            for (long o = 0x0AD8; o + 3 <= 0x0E3C; o++) {
                if (d1050[o] == 0x29 && (d1050[o + 1] == 7 || d1050[o + 1] == 8)) treffer++;
                if (d1050[o] == 0x4B && (d1050[o + 1] == 7 || d1050[o + 2] == 8)) treffer++;
            }
            if (treffer != 0) {
                fprintf(stderr, "FAIL: ROOM1050 SCD schaltet doch auf Cut 7/8 (%d Stellen)\n",
                        treffer);
                fail = 1;
            } else {
                printf("  ROOM1050 SCD: kein Cut_chg/Cut_replace auf 7 oder 8 — beide Cuts "
                       "sind im Auslieferungsstand unerreichbar (portseitig: rolltor_1050.c)\n");
            }
        }

        /* ---- (6a) GEGENPROBE: misst (6) wirklich den Schalter, oder nur sich selbst?
         * Derselbe Aufbau mit flag(3,121)=1 — sub00 @0x00C1E nimmt dann den ELSE-Zweig
         * und installiert Slot 7 GAR NICHT. Genau diese Form haette ein Raum, in dem der
         * Schalter hinter einer Sicherungs-Bedingung liegt. Der Lauf muss den fehlenden
         * Schalter SEHEN. Er laeuft auf einer KOPIE der Bytes, weil Sca_id_set die SCA
         * im RDT-Puffer bleibend umschreibt — sonst waere (6) danach schon "offen". */
        {
            uint8_t *kopie = (uint8_t *)malloc((size_t)sz1050);
            re15_rdt_t rk;
            memcpy(kopie, d1050, (size_t)sz1050);
            if (re15_rdt_parse(kopie, (size_t)sz1050, &rk) != 0) {
                fprintf(stderr, "FAIL: Gegenprobe — RDT-Parse der Kopie\n"); fail = 1;
            } else {
                re15_actor_init(); re15_aot_init(); scd_vm_init();
                memset(&g_room_change, 0, sizeof g_room_change);
                g_current_room_id = 0x1050;
                scd_register_current_rdt(&rk);
                re15_msg_load_room_block(rk.messages, rk.messages_size);
                re15_game_flag_set(3, 121, 1);          /* Shutter gilt als schon offen */
                re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                p->active = 1; p->type = 0; p->hp = 100;
                p->x = 17200 - 620; p->y = 0; p->z = -8550; p->rot_y = 0; p->state = 1;
                g_scd.player_mode = 0;
                scd_register_room_events(&rk);
                scd_room_reenter(&rk, p->x, p->z, 0);
                for (int i = 0; i < 120; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); }
                if (g_aot.slots[7].active && g_aot.slots[7].event_id == 2) {
                    fprintf(stderr, "FAIL: Gegenprobe — Slot 7 ist trotz flag(3,121)=1 da, "
                                    "der Haken misst den Schalter also nicht\n");
                    fail = 1;
                } else {
                    printf("  Gegenprobe: mit flag(3,121)=1 bleibt Slot 7 WEG (act=%d) — "
                           "der Haken sieht einen fehlenden Schalter\n", g_aot.slots[7].active);
                }
            }
            free(kopie);
        }

        /* ---- (6) DURCHSPIELBARKEIT: der Shutter oeffnet, LIVE gefahren ---- */
        re15_actor_init();
        re15_aot_init();
        scd_vm_init();
        memset(&g_room_change, 0, sizeof g_room_change);
        g_current_room_id = 0x1050;
        scd_register_current_rdt(&rdt);
        re15_msg_load_room_block(rdt.messages, rdt.messages_size);
        re15_game_flag_set(3, 121, 0);        /* frischer Zustand: Shutter zu */

        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        /* Mitte des Schalter-Rechtecks: der RDT-Record traegt die ECKE (16800,-8950) und
         * die volle Groesse (800,800), der Port rechnet daraus Mitte (17200,-8550) und
         * Halbmass (400,400) — gemessen am installierten Slot, nicht angenommen. */
        /* 620 Einheiten VOR dem Schalter, Blick darauf: flags 0x31 hat Bit 0x40 NICHT,
         * der Port prueft also nur den VORWAERTSPUNKT pos + rotate((620,0)) gegen das
         * Rechteck (aot_common.c:1125-1133, byte-true FUN_80042bac `ori 0x26c`
         * @0x80042bd0). Mit rot_y=0 landet er genau auf der Mitte (17200,-8550). */
        pl->x = 17200 - 620; pl->y = 0; pl->z = -8550; pl->rot_y = 0; pl->state = 1;
        g_scd.player_mode = 0;

        /* Raum genau so hochfahren wie der Tuer-Eintritt im Spiel (probe_room1040_switch).
         * ⛔ NICHT `scd_thread_start(1, sub_scd[0])`: Slot 1 gehoert dem Per-Frame-Reseed
         * auf sub_scd[1] (FUN_8003f038 @0x8003f064/70/80/84) und wird sonst im selben Bild
         * ueberschrieben — gemessen, sub00 lief dann kein einziges Opcode. */
        scd_register_room_events(&rdt);
        scd_room_reenter(&rdt, pl->x, pl->z, 0);
        for (int i = 0; i < 120; i++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0xFF); }

        /* VORZUSTAND, damit die Freigabe unten nicht schon vorher gilt: die Sperrzelle 19
         * muss in allen fuenf Partitionen SOLIDE sein (u0=0xFF, floor=3 — Rohbytes des
         * SCA-Blocks @0x550, Zelle 19: Breite 5000, Dichte 400, x=13000, z=-10600). */
        {
            int solide = 0, basis = 0;
            for (int g = 0; g < 5; g++) {
                int fi = basis + 19;
                basis += rdt.sca_rgn[g];
                if (fi < rdt.sca_count && rdt.sca[fi].u0 == 0xFF && rdt.sca[fi].floor == 3)
                    solide++;
            }
            if (solide != 5) {
                fprintf(stderr, "FAIL: Vorzustand — nur %d von 5 Partitionen haben Zelle 19 "
                                "solide (u0=0xFF, floor=3)\n", solide);
                fail = 1;
            } else {
                printf("  Vorzustand: SCA-Zelle 19 in allen 5 Partitionen SOLIDE "
                       "(u0=0xFF, floor=3)\n");
            }
        }

        /* sub00 @0x00C22 installiert den Schalter: Aot_set slot=7 sce=3 flags=0x31
         * rect=(16800,-8950,800,800), Nutzlast ff 00 18 02 -> sub02. */
        {
            const re15_aot_t *a = &g_aot.slots[7];
            if (!a->active || a->event_id != 2 || a->x != 17200 || a->z != -8550 ||
                a->half_w != 400 || a->half_h != 400) {
                fprintf(stderr, "FAIL: Schalter-AOT 7 act=%d ev=%d c=(%d,%d) h=(%d,%d)\n",
                        a->active, a->event_id, (int)a->x, (int)a->z,
                        (int)a->half_w, (int)a->half_h);
                fail = 1;
            } else {
                printf("  Schalter: AOT 7 (sub00 @0x00C22) ev=2 Mitte(17200,-8550) "
                       "halb(400,400)\n");
            }
        }

        /* (6i) OHNE Sicherung: Aktion -> Frage -> Ja -> Nahansicht + "I need a fuse ..." -> das
         * Tor bleibt zu (Port-Programm rolltor_1050.c statt sub02). */
        if (schalter_ja(pl, 8) != 0) fail = 1;
        if (re15_game_flag_get(3, 121) || re15_game_flag_get(9, 63) || zelle19_solide(&rdt) != 5) {
            fprintf(stderr, "FAIL: (6i) ohne Sicherung oeffnet das Tor ((3,121)=%d (9,63)=%d)\n",
                    re15_game_flag_get(3, 121), re15_game_flag_get(9, 63));
            fail = 1;
        } else {
            printf("  (6i)  ohne Sicherung: Frage -> Ja -> Tor bleibt zu, Zelle 19 solide\n");
        }

        /* (6ii) MIT Sicherung (Fundstelle Hebetisch ROOM1150, sicherung_1150.c): Aktion -> Ja ->
         * "Will you use the Fuse?" -> Ja -> eingesetzt. */
        g_inv.slots[0].id = RE15_SICHERUNG_ITEM; g_inv.slots[0].qty = 1; g_inv.slots[0].flags = 0;
        if (schalter_ja(pl, 8) != 0) fail = 1;
        if (!re15_game_flag_get(RE15_ROLLTOR_EINGESETZT_BANK, RE15_ROLLTOR_EINGESETZT_BIT) ||
            re15_inv_find_item(RE15_SICHERUNG_ITEM) >= 0 || re15_game_flag_get(3, 121)) {
            fprintf(stderr, "FAIL: (6ii) Einsetzen: (9,63)=%d Sicherung-Platz=%d (3,121)=%d\n",
                    re15_game_flag_get(9, 63), re15_inv_find_item(RE15_SICHERUNG_ITEM),
                    re15_game_flag_get(3, 121));
            fail = 1;
        } else {
            printf("  (6ii) mit Sicherung: Ja/Ja -> eingesetzt, (9,63)=1, Sicherung aus dem Inventar\n");
        }

        /* (6iii) danach: der ausgelieferte sub02 — Frage -> Ja -> das Tor faehrt. */
        if (schalter_ja(pl, 1500) != 0) fail = 1;

        /* sub02 @0x00CBA Set(3,121,1) = Shutter offen. */
        if (!re15_game_flag_get(3, 121)) {
            fprintf(stderr, "FAIL: flag(3,121) bleibt 0 — der Shutter oeffnet nicht\n");
            fail = 1;
        } else {
            printf("  flag(3,121)=1 (sub02 @0x00CBA) — der Shutter ist offen\n");
        }

        /* sub02 @0x00D4C..0x00D70: Sca_id_set/Sca_floor_set Region 0..4, Index 0x13 = 19
         * auf 0 -> die Sperrzelle (Breite 5000, Dichte 400, x=13000, z=-10600) ist frei. */
        {
            int offen = 0, basis = 0;
            for (int g = 0; g < 5; g++) {
                int fi = basis + 19;
                basis += rdt.sca_rgn[g];
                if (fi < rdt.sca_count && rdt.sca[fi].u0 == 0 && rdt.sca[fi].floor == 0) offen++;
            }
            if (offen != 5) {
                fprintf(stderr, "FAIL: nur %d von 5 SCA-Partitionen haben Zelle 19 freigegeben\n",
                        offen);
                fail = 1;
            } else {
                printf("  SCA-Zelle 19 in allen 5 Partitionen frei (u0=0, floor=0) — der Weg "
                       "nach Sueden ist offen\n");
            }
        }

        free(d1050); free(d2030); free(d2060);
    }

    if (fail) { printf("RESULT: FAIL\n"); return 1; }
    printf("RESULT: OK\n");
    return 0;
}
