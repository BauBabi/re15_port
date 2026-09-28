/* test_r30_irons_diary_ablauf.c — RIEGEL Runde 30, Thema irons-diary-dokument:
 * Speicherstand v9 und die Aufhebe-Reihenfolge.
 * Dossier: analysis/befunde_runde30/irons-diary-dokument.md (S2, S5, S8)
 *
 * TEIL S  SPEICHERSTAND (SPEICHER-VERTRAG v9, wortgleich mit Spur karten-marken)
 *   S1  Layout: sizeof 944, visited_floor @900, files @916, checksum @940 (vorher 904 / 900).
 *   S2  Rundlauf ueber eine echte .mcr: Liste mit Irons Diary auf Platz 0 -> speichern ->
 *       Liste leeren -> laden -> Platz 0 = Dokument 0, 23 Plaetze leer. RE2 speichert die
 *       Liste mit (Offset 0x6C4 im Block 0x800D44A4, 0x800 Byte @0x801c0c70-94; geladen
 *       0x798 Byte @0x801c0dfc).
 *   S3  Hebung v8 -> v9 an der SPEICHERKARTE DES NUTZERS (analysis/befunde_runde30/
 *       nutzer_marken/re15_card_nutzer_2026-09-27.mcr, geschrieben vom Port v0.8.15):
 *       jeder belegte Platz traegt roh Version 8, laedt als Version 9 mit 24 x 0xFF
 *       (RE2 leer = 0xFF, `addiu v0,zero,255` @0x800682e0) und visited_floor = 0.
 *   S4  Hebung v7 -> v9 (die Besucht-Bits werden verworfen) und ein verfaelschter v8-Block
 *       wird abgewiesen.
 *   S5  Vertrag: capture schreibt das FREMDE Feld visited_floor als Leerwert 0.
 *
 * TEIL A  AUFHEBEN (RE2: Item-Zone FUN_80051884 @0x800518f0-0x80051918, Aufnahme
 *         FUN_80071ba0 @0x80071d00-0x80071df8, Schliessen Zustand 6 @0x80072b0c-0x80072bfc)
 *   A1  Zone mit item_type 0x48 zuenden -> das Item-Modal startet NIE (seine Bildquellen
 *       enden bei 0x47: ITEM/ITPS.ITP 72 Bilder), das Menue wird angefordert.
 *   A2  nach der Oeffnen-Blende: FILE-Welle (substate 2), Leser im Zustand 7, Titelseite,
 *       die Liste traegt Platz 0 = Dokument 0 SCHON JETZT (RE2 haengt @0x80071d00 VOR
 *       dem Lesen an); im ersten Bild nach der Blende Satz 8 (@0x80071df0-f4).
 *   A3  KREUZ -> Meldung "The Irons Diary has been filed." (Zustand 8), Satz 5
 *       (@0x80072854); Flag, Zone und Weltmodell stehen UNVERAENDERT, solange die Meldung
 *       steht (RE2 wartet auf Bit 0x80 von 0x800E873C @0x80072b10-1c).
 *   A4  Bestaetigen -> im SELBEN Bild: Flag (9,taken_bit) gesetzt (@0x80072b8c), Zone
 *       inaktiv (@0x80072b40), Weltmodell aus (@0x80072bb0), Satz 5 (@0x80072bf0-f8);
 *       dann schliesst das Menue (Modus 0 @0x80072bfc).
 */
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_files.h"
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_item_modal.h"
#include "re15_item_prompt.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_fade.h"
#include "re15_actor.h"
#include "re15_room.h"

#ifndef R30_NUTZER_KARTE
#define R30_NUTZER_KARTE "analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr"
#endif

extern int g_test_core_se_last, g_test_core_se_count;   /* tests/support/test_support.c */

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

/* ein Port-Bild wie test_inv_fsm.c: Start-Abfrage + FSM, dann die Zeichen-Seite */
static void frame(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open())
        re15_inv_screen_ecg_tick();
    re15_fade_tick();
}

static uint32_t summe(const uint8_t *p, size_t n)
{
    uint32_t s = 0;
    for (size_t i = 0; i < n; i++) s += p[i];
    return s;
}

/* ------------------------------------------------------------------ TEIL S */
static void teil_s(void)
{
    printf("\n[S] Speicherstand v9\n");

    /* S1 Layout */
    CHECK(RE15_SAVE_VERSION == 9, "RE15_SAVE_VERSION = %d (Soll 9)", RE15_SAVE_VERSION);
    CHECK(sizeof(re15_savedata_t) == 944 &&
          offsetof(re15_savedata_t, visited_floor) == 900 &&
          offsetof(re15_savedata_t, files) == 916 &&
          offsetof(re15_savedata_t, checksum) == 940,
          "Layout: sizeof %u, visited_floor @%u, files @%u, checksum @%u "
          "(Soll 944 / 900 / 916 / 940)",
          (unsigned)sizeof(re15_savedata_t),
          (unsigned)offsetof(re15_savedata_t, visited_floor),
          (unsigned)offsetof(re15_savedata_t, files),
          (unsigned)offsetof(re15_savedata_t, checksum));

    /* S2 Rundlauf ueber eine .mcr */
    {
        const char *mcr = "test_r30_irons_diary.mcr";
        re15_savedata_t sd, ld;
        uint16_t room = 0;
        remove(mcr);
        memset(&g_actors[0], 0, sizeof g_actors[0]);
        g_current_room_id = 0x1150;
        re15_files_reset();
        re15_files_add(0);
        re15_savedata_capture(&sd, 100, 1);
        int leer_fremd = 1;
        for (int i = 0; i < 16; i++) if (sd.visited_floor[i] != 0) leer_fremd = 0;
        CHECK(leer_fremd, "S5 capture schreibt visited_floor (fremdes Feld) als 16 x 0");
        CHECK(sd.files[0] == 0 && sd.files[1] == 0xFF && sd.files[23] == 0xFF,
              "capture: files = %02x %02x .. %02x (Soll 00 ff .. ff)",
              sd.files[0], sd.files[1], sd.files[23]);
        CHECK(re15_memcard_save(mcr, 1, &sd, "BIOHAZARD 1.5") == 0, "speichern in Platz 1");
        re15_files_reset();
        CHECK(re15_files_count() == 0, "Liste geleert");
        int ok = re15_memcard_load(mcr, 1, &ld) == 0 && re15_savedata_restore(&ld, &room) == 0;
        CHECK(ok && ld.version == 9, "laden + wiederherstellen, Version %u", ld.version);
        CHECK(re15_files_get(0) == 0 && re15_files_count() == 1,
              "Rundlauf: Platz 0 = Dokument %d, belegt %d (Soll 0 / 1)",
              re15_files_get(0), re15_files_count());
        remove(mcr);
    }

    /* S3 Hebung an der Speicherkarte des Nutzers (v8, Port v0.8.15) */
    {
        FILE *f = fopen(R30_NUTZER_KARTE, "rb");
        static uint8_t img[131072];
        size_t n = f ? fread(img, 1, sizeof img, f) : 0;
        if (f) fclose(f);
        CHECK(n == sizeof img, "Nutzer-Karte gelesen (%u B, Soll 131072)", (unsigned)n);
        int roh_v8 = 0, gehoben = 0, gefahren = 0;
        for (int slot = 0; n == sizeof img && slot < RE15_SAVE_SLOTS; slot++) {
            const uint8_t *dir = img + (slot + 1) * 128;
            const uint8_t *b   = img + (slot + 1) * 8192;
            if ((dir[0] & 0xF0) != 0x50 || b[0] != 'S' || b[1] != 'C') continue;
            gefahren++;
            uint32_t ver;
            memcpy(&ver, b + 0x100 + offsetof(re15_savedata_t, version), sizeof ver);
            if (ver == 8) roh_v8++;
            re15_savedata_t ld;
            if (re15_memcard_load(R30_NUTZER_KARTE, slot, &ld) != 0) continue;
            int alle_ff = 1, alle_0 = 1;
            for (int i = 0; i < 24; i++) if (ld.files[i] != 0xFF) alle_ff = 0;
            for (int i = 0; i < 16; i++) if (ld.visited_floor[i] != 0) alle_0 = 0;
            if (ld.version == 9 && alle_ff && alle_0 &&
                ld.checksum == re15_savedata_checksum(&ld)) gehoben++;
        }
        printf("  Nutzer-Karte: %d belegte Plaetze, roh v8: %d, gehoben v9 mit leerer Liste: %d\n",
               gefahren, roh_v8, gehoben);
        CHECK(gefahren > 0 && roh_v8 == gefahren && gehoben == gefahren,
              "Hebung v8 -> v9 an der Nutzer-Karte: %d von %d Plaetzen (files 24 x 0xFF, "
              "visited_floor 0, Pruefwort neu)", gehoben, gefahren);
    }

    /* S4 v7 -> v9 und ein verfaelschter v8-Block */
    {
        re15_savedata_t sd;
        memset(&g_actors[0], 0, sizeof g_actors[0]);
        re15_files_reset();
        re15_savedata_capture(&sd, 5, 2);
        uint8_t *p = (uint8_t *)&sd;
        size_t off = offsetof(re15_savedata_t, visited_floor);
        /* v7-Form bauen: Pruefwort bei 900 ueber [0,900), dahinter Nullen */
        sd.version = 7;
        sd.visited[0] = 0x5a;
        memset(p + off, 0, sizeof sd - off);
        uint32_t ck = summe(p, off);
        memcpy(p + off, &ck, sizeof ck);
        re15_savedata_t v7 = sd;
        CHECK(re15_savedata_validate(&v7) == 0 && v7.version == 9 && v7.visited[0] == 0 &&
              v7.files[0] == 0xFF && v7.files[23] == 0xFF,
              "Hebung v7 -> v9: Version %u, visited[0] %02x (Soll 0, v<8 verworfen), "
              "files[0] %02x", v7.version, v7.visited[0], v7.files[0]);
        re15_savedata_t v8 = sd;
        v8.version = 8;
        ck = summe((uint8_t *)&v8, off);
        memcpy((uint8_t *)&v8 + off, &ck, sizeof ck);
        re15_savedata_t v8ok = v8;
        CHECK(re15_savedata_validate(&v8ok) == 0 && v8ok.visited[0] == 0x5a,
              "Hebung v8 -> v9 behaelt die Besucht-Bits (visited[0] %02x)", v8ok.visited[0]);
        v8.player_hp ^= 1;                      /* verfaelscht, Pruefwort alt */
        CHECK(re15_savedata_validate(&v8) != 0, "verfaelschter v8-Block wird abgewiesen");
        re15_savedata_t v9 = v8ok;
        v9.files[5] = 0;                        /* v9 verfaelscht */
        CHECK(re15_savedata_validate(&v9) != 0, "verfaelschter v9-Block wird abgewiesen");
    }
}

/* ------------------------------------------------------------------ TEIL A */
static void teil_a(void)
{
    enum { SLOT = 5, TAKEN = 0x33, OBJ = 7 };
    printf("\n[A] Aufheben: Reihenfolge\n");

    re15_files_reset();
    memset(&g_aot, 0, sizeof g_aot);
    g_aot.slots[SLOT].active = 1;
    g_aot.slots[SLOT].type   = RE15_AOT_TYPE_ITEM;
    g_aot.item_params[SLOT].item_type  = 0x48;
    g_aot.item_params[SLOT].amount     = 1;
    g_aot.item_params[SLOT].taken_bit  = TAKEN;
    g_aot.item_params[SLOT].taken_prop = OBJ;
    g_scd.prop_count = 1;
    g_scd.props[0].obj_id = OBJ;
    g_scd.props[0].active = 1;
    re15_game_flag_set(9, TAKEN, 0);

    /* A1 */
    re15_aot_fire_slot(SLOT);
    CHECK(!re15_item_modal_active(), "A1 Zone 0x48: das Item-Modal startet NICHT");
    CHECK(re15_menu_doc_active() && re15_menu_stage() != 0,
          "A1 der Aufnahme-Leser ist angefordert (Menue-Stufe %d)", re15_menu_stage());

    /* A2: bis der Lauf im Leser steht */
    int bild = 0, modal_je = 0, se_offen = -1, se0 = g_test_core_se_count;
    while (bild < 200 && !(re15_menu_phase() == 1 && g_inv_screen.item_state == 3)) {
        int vor = g_test_core_se_count;
        frame(0, 0);
        bild++;
        if (re15_item_modal_active()) modal_je = 1;
        if (se_offen < 0 && g_test_core_se_count > vor && g_test_core_se_last == 8)
            se_offen = bild;
        if (re15_menu_phase() == 1 && g_inv_screen.item_state == 7 &&
            g_inv_screen.file_text_x == 0x140 - 28) {
            CHECK(se_offen == bild,
                  "A2 Satz 8 im ersten Fahrbild nach der Blende (Bild %d; RE2 erst Blende, "
                  "dann Ton @0x80071dec-f4)", bild);
        }
    }
    printf("  Leser liest nach %d Bildern (Toene seit Anforderung: %d)\n", bild,
           g_test_core_se_count - se0);
    CHECK(re15_menu_substate() == 2 && g_inv_screen.item_state == 3 &&
          g_inv_screen.file_bild == 1 && g_inv_screen.file_bildsatz == 25 &&
          g_inv_screen.file_end == 18 && g_inv_screen.file_reader_page == 0,
          "A2 FILE-Welle, Leser Zustand 3, Satz 25, 18 Seiten, Seite 0 (Titel)");
    CHECK(re15_files_get(0) == 0 && re15_files_count() == 1 && re15_menu_doc_trace(0) > 0,
          "A2 die Liste traegt Platz 0 = Dokument 0 schon beim Lesen (RE2 @0x80071d00)");
    CHECK(!re15_game_flag_get(9, TAKEN) && g_aot.slots[SLOT].active == 1 &&
          g_scd.props[0].active == 1,
          "A2 beim Lesen: Flag, Zone und Weltmodell unveraendert");

    /* blaettern geht (eine Seite vor) */
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);
    for (int i = 0; i < 22; i++) frame(0, 0);
    CHECK(g_inv_screen.file_reader_page == 1 && g_inv_screen.item_state == 3,
          "A2 Blaettern: Seite %d", g_inv_screen.file_reader_page);

    /* A3 KREUZ schliesst -> Meldung */
    int vor = g_test_core_se_count;
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    uint8_t mid = 0; int rev = -1;
    CHECK(g_inv_screen.item_state == 8 && re15_menu_doc_msg(&mid, &rev) == 1 && mid == 0x48,
          "A3 KREUZ -> Zustand 8, Meldung steht, Namens-Id 0x%02x (Soll 0x48)", mid);
    CHECK(g_test_core_se_count == vor + 1 && g_test_core_se_last == 5,
          "A3 Satz 5 beim Schliessen (RE2 @0x80072854)");
    int total = re15_menu_doc_msg_total();
    CHECK(total == 30, "A3 Meldung \"The Irons Diary has been filed.\" = %d Glyphen (Soll 30)",
          total);
    int bilder_meldung = 0;
    while (bilder_meldung < 400) {
        if (!re15_menu_doc_msg(&mid, &rev) || rev >= total) break;
        frame(0, 0);
        bilder_meldung++;
        if (re15_game_flag_get(9, TAKEN) || g_aot.slots[SLOT].active == 0 ||
            g_scd.props[0].active == 0) break;
    }
    CHECK(re15_menu_doc_msg(&mid, &rev) == 1 && rev == total,
          "A3 Schreibmaschine fertig nach %d Bildern", bilder_meldung);
    for (int i = 0; i < 30; i++) frame(0, 0);            /* Meldung steht, niemand drueckt */
    CHECK(!re15_game_flag_get(9, TAKEN) && g_aot.slots[SLOT].active == 1 &&
          g_scd.props[0].active == 1 && re15_menu_doc_msg(&mid, &rev) == 1,
          "A3 solange die Meldung steht: KEIN Abraeumen (RE2 @0x80072b10-1c)");

    /* A4 Bestaetigen */
    vor = g_test_core_se_count;
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(re15_menu_doc_msg(&mid, &rev) == 0, "A4 Meldung weg");
    CHECK(re15_game_flag_get(9, TAKEN) && g_aot.slots[SLOT].active == 0 &&
          g_scd.props[0].active == 0,
          "A4 im selben Bild: Flag (9,0x33) gesetzt, Zone inaktiv, Weltmodell aus");
    CHECK(g_test_core_se_count == vor + 1 && g_test_core_se_last == 5,
          "A4 Satz 5 (RE2 @0x80072bf0-f8)");
    uint32_t t0 = re15_menu_doc_trace(0), t1 = re15_menu_doc_trace(1),
             t2 = re15_menu_doc_trace(2), t3 = re15_menu_doc_trace(3),
             t4 = re15_menu_doc_trace(4), t5 = re15_menu_doc_trace(5);
    printf("  Reihenfolge (Bild des Laufs): angehaengt %u, geschlossen %u, Meldung weg %u, "
           "Flag %u, Zone %u, Weltmodell %u\n", t0, t1, t2, t3, t4, t5);
    CHECK(t0 > 0 && t0 < t1 && t1 < t2 && t2 == t3 && t3 == t4 && t4 == t5,
          "A4 Reihenfolge: anhaengen < schliessen < Meldung weg = Flag = Zone = Weltmodell");

    /* das Menue schliesst */
    int zu = 0;
    while (zu < 200 && re15_menu_is_open()) { frame(0, 0); zu++; if (re15_item_modal_active()) modal_je = 1; }
    CHECK(!re15_menu_is_open() && !re15_menu_doc_active() && g_inv_screen.file_bild == 0,
          "A4 Menue geschlossen nach %d Bildern, Aufnahme-Leser abgebaut", zu);
    CHECK(!modal_je, "A1 das Item-Modal war im ganzen Lauf nie aktiv");
    CHECK(re15_files_count() == 1, "die Liste behaelt das Dokument (%d)", re15_files_count());
}

int main(void)
{
    printf("=== r30 irons-diary-dokument: Speicherstand + Aufheben ===\n");
    teil_s();
    teil_a();
    if (fails) { printf("\nR30 IRONS DIARY ABLAUF: FAIL (%d)\n", fails); return 1; }
    printf("\nR30 IRONS DIARY ABLAUF: alle Riegel halten\n");
    return 0;
}
