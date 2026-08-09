/* test_debug_menu.c — nagelt das portierte ORIGINAL-Debug-Menue an die EXE-Bytes.
 *
 * Quelle: PSX.EXE, Menue-Funktion FUN_8001443c @0x80014444. Jede Erwartung zitiert ihre Adresse.
 * Das Menue liest die RAW-Pad-Halbwoerter 0x800AC760 (held) / 0x800AC762 (edge) — PsyQ-PadRead-
 * Layout (SELECT=0x100, D-Pad 0x1000/0x2000/0x4000/0x8000, Dreieck 0x10, Kreuz 0x40, Quadrat 0x80;
 * Schreiber FUN_80030444: DAT_800ac760 @0x8003056?, edge = press-Flanke).
 *
 * Die JUMP-Tabelle kommt zur LAUFZEIT aus DEBUG.BIN (RAW-Abbild @0x800c0000): Satz-Halbwort
 * @0x263a+2*(637*st+13*idx) — 0 = Slot uebersprungen (@0x800147B0), 2 in Slot 0 = Stage gesperrt
 * (@0x800148C8 ori 2 / @0x8001492C beq) — Name @+8 (%s-Zeiger @0x80014BB4).
 */
#include "re15_debug_menu.h"
#include "debug_jump_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* RAW-Pad-Bits (PsyQ PadRead, wie 0x800AC760/762 sie fuehren) */
#define PAD_UP     0x1000   /* @0x800145F4 */
#define PAD_DOWN   0x4000   /* @0x80014628 */
#define PAD_RIGHT  0x2000   /* @0x800146D8 */
#define PAD_LEFT   0x8000   /* @0x800147C4 */
#define EDGE_EXIT  0x0040   /* Kreuz-EDGE  @0x8001466C  */
#define EDGE_STAGE 0x0010   /* Dreieck-EDGE @0x800148B4 */
#define EDGE_LOAD  0x0080   /* Quadrat-EDGE @0x80014A38 */
#define EDGE_SEL   0x0100   /* SELECT-EDGE  @0x8001444C */

/* Frischer Druck = held+edge im selben Frame (so schreibt FUN_80030444 die Woerter). */
#define PRESS(b)  re15_debug_menu_tick((uint16_t)(b), (uint16_t)(b))
#define HOLD(b)   re15_debug_menu_tick((uint16_t)(b), 0)
#define TICKE(e)  re15_debug_menu_tick(0, (uint16_t)(e))

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static unsigned char *read_debug_bin(size_t *out)
{
    char p[512];
    snprintf(p, sizeof p, "%s/BIN/DEBUG.BIN", RE15_ASSET_PSX_DIR);
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    unsigned char *b = (unsigned char *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)sz;
    return b;
}

/* Ein frischer Druck steppt sofort; die Folgeschritte kommen aus dem Auto-Repeat. */
static void press_step(uint16_t bits) { PRESS(bits); }

int main(void)
{
    size_t dbsz = 0;
    unsigned char *dbin = read_debug_bin(&dbsz);
    CHECK(dbin != NULL, "DEBUG.BIN unter %s/BIN gefunden (Test braucht die echte Tabelle)",
          RE15_ASSET_PSX_DIR);
    if (!dbin) return 1;

    re15_debug_menu_reset();
    re15_debug_menu_set_bin(dbin, (unsigned)dbsz);
    const re15_debug_menu_t *m = re15_debug_menu_state();

    CHECK(!re15_debug_menu_open(), "das Menue startet geschlossen");
    re15_debug_menu_toggle();
    CHECK(re15_debug_menu_open(), "toggle oeffnet das Menue");

    /* Open-Sync (@0x800144B0/@0x800144E0): Cursor auf den aktuellen Raum. */
    re15_debug_menu_sync_cursor(0x1240);
    CHECK(m->stage == 0 && m->room_idx[0] == 0x24,
          "sync 0x1240 -> Stage 0, Index 0x24 OPENING (Open-Block @0x800144B0/E0), got st=%d idx=0x%02x",
          m->stage, m->room_idx[0]);
    CHECK(strncmp(re15_debug_menu_room_name(), "OPENING", 7) == 0,
          "Name @DEBUG.BIN 0x2642+off: OPENING, got \"%s\"", re15_debug_menu_room_name());

    /* Auswahlzeile: 0..2; Schritt nur bei Repeat-Puls (frischer Druck feuert sofort,
     * @0x800145E0 bgez-Gate + FUN_80030444-Tail). */
    CHECK(m->row == 0, "Startzeile 0");
    press_step(PAD_UP);
    CHECK(m->row == 0, "Zeile 0 darf nicht unter 0 (@0x8001460C beq v0,zero)");
    press_step(PAD_DOWN);
    CHECK(m->row == 1, "frischer Druck runter -> Zeile 1 (JUMP, @0x80014698)");
    press_step(PAD_DOWN);
    CHECK(m->row == 2, "runter -> Zeile 2 (MEMORY VIEWER, @0x800146A8)");
    press_step(PAD_DOWN);
    CHECK(m->row == 2, "Zeile 2 ist das Maximum (@0x80014644 sltiu 0x2)");
    press_step(PAD_UP);
    CHECK(m->row == 1, "hoch -> zurueck auf JUMP");

    /* Auto-Repeat-Kadenz (FUN_80030640(0xf000,5,1) @0x80014470-7C + FUN_80030444-Tail
     * @0x800305A4-0x80030628): frischer Druck steppt bei t=0, dann t=6, 8, 10, ... */
    re15_debug_menu_sync_cursor(0x1000);       /* Stage 0, Index 0 (BATH-LOCKERS) */
    CHECK(m->room_idx[0] == 0x00, "Kadenz-Start Index 0");
    PRESS(PAD_RIGHT);                          /* t=0: fire, Zaehler := 5 */
    CHECK(m->room_idx[0] == 0x01, "t=0 frischer Druck steppt sofort, got 0x%02x", m->room_idx[0]);
    for (int t = 1; t <= 5; t++) {
        HOLD(PAD_RIGHT);                       /* t=1..5: Zaehler 5->0, kein Puls */
        CHECK(m->room_idx[0] == 0x01, "t=%d haelt (Delay 5, @0x80014474), got 0x%02x",
              t, m->room_idx[0]);
    }
    HOLD(PAD_RIGHT);                           /* t=6: Zaehler 0 -> fire, := Rate 1 */
    CHECK(m->room_idx[0] == 0x02, "t=6 erster Repeat-Schritt, got 0x%02x", m->room_idx[0]);
    HOLD(PAD_RIGHT);                           /* t=7: kein Puls */
    CHECK(m->room_idx[0] == 0x02, "t=7 haelt (Rate 1, @0x8001447C), got 0x%02x", m->room_idx[0]);
    HOLD(PAD_RIGHT);                           /* t=8: fire */
    CHECK(m->room_idx[0] == 0x03, "t=8 zweiter Repeat-Schritt, got 0x%02x", m->room_idx[0]);

    /* Leere Slots (Halbwort 0 @0x800147B0) werden uebersprungen: Stage 0 ist 0x00-0x23 belegt,
     * 0x24 OPENING ist Marker 0 (nur per Open-Sync erreichbar), dann 0x25/0x26 (gemessen aus
     * MZD-DEBUG.BIN 0x263a). 16 Links-Schritte von 0x24 -> 0x14 BRIEFING = die Hardware-Messung
     * der re15-room-capture-Basis. */
    re15_debug_menu_sync_cursor(0x1230);
    CHECK(m->room_idx[0] == 0x23, "Start 0x23");
    press_step(PAD_RIGHT);
    CHECK(m->room_idx[0] == 0x25, "rechts von 0x23 ueberspringt OPENING(0x24, Marker 0) -> 0x25, got 0x%02x",
          m->room_idx[0]);
    re15_debug_menu_sync_cursor(0x1240);
    for (int i = 0; i < 16; i++) press_step(PAD_LEFT);
    CHECK(m->room_idx[0] == 0x14, "16 Links-Schritte von OPENING -> 0x14 BRIEFING (Hardware-Anker), got 0x%02x",
          m->room_idx[0]);
    CHECK(strncmp(re15_debug_menu_room_name(), "BRIEFING ROOM", 13) == 0,
          "Name = BRIEFING ROOM, got \"%s\"", re15_debug_menu_room_name());

    /* Rueckwaerts-Wrap ist byte-genau ASYMMETRISCH: bei idx==0 wird erst 0x2F gesetzt
     * (sb a2 @0x8001480C), DANN dekrementiert (@0x8001482C) — erster Kandidat 0x2E. In Stage 0
     * sind 0x27..0x30 leer, gelandet wird auf 0x26 SEWER PASSAGE. */
    re15_debug_menu_sync_cursor(0x1000);
    press_step(PAD_LEFT);
    CHECK(m->room_idx[0] == 0x26, "links von 0 -> 0x26 (Wrap 0x2E abwaerts, @0x8001480C/2C), got 0x%02x",
          m->room_idx[0]);
    CHECK(strncmp(re15_debug_menu_room_name(), "SEWER PASSAGE", 13) == 0,
          "Name = SEWER PASSAGE, got \"%s\"", re15_debug_menu_room_name());

    /* Wrap vorwaerts 0x31->0 (@0x80014734/4C), immer nur auf belegte Slots.
     * Stage 5 hat NUR 0x00-0x03 (ENDING/EMPTY sind Marker 0). */
    for (int i = 0; i < 6; i++) if (m->stage != 5) TICKE(EDGE_STAGE);
    /* robust hinnavigieren: */
    while (m->stage != 5) TICKE(EDGE_STAGE);
    CHECK(m->room_idx[5] <= 0x03, "Stage-5-Index liegt auf einem belegten Slot, got 0x%02x", m->room_idx[5]);
    press_step(PAD_LEFT);
    CHECK(m->room_idx[5] <= 0x03, "links in Stage 5 bleibt in 0x00-0x03 (ENDING 0x04 hat Marker 0), got 0x%02x",
          m->room_idx[5]);
    for (int i = 0; i < 8; i++) press_step(PAD_RIGHT);
    CHECK(m->room_idx[5] <= 0x03, "rechts in Stage 5 zykliert 0x00-0x03, got 0x%02x", m->room_idx[5]);

    /* Dreieck = Stage + 1 (EDGE @0x800148B4), Wrap bei 6 (@0x800148E4/F0); der Raumindex ist
     * PRO STAGE (@0x800146F4). */
    while (m->stage != 0) TICKE(EDGE_STAGE);
    uint8_t st0_idx = m->room_idx[0];
    for (int i = 0; i < 6; i++) TICKE(EDGE_STAGE);
    CHECK(m->stage == 0, "6 x Dreieck ist ein voller Ring, got %d", m->stage);
    CHECK(m->room_idx[0] == st0_idx, "Stage-0-Index unberuehrt vom Ring");

    /* Stage-Sperr-Marker 2 (@0x800148C8/0x8001492C): synthetisch Stage 1 sperren ->
     * Dreieck von Stage 0 springt direkt auf Stage 2. */
    {
        unsigned char *mod = (unsigned char *)malloc(dbsz);
        memcpy(mod, dbin, dbsz);
        mod[0x263a + 0x4fa] = 2; mod[0x263a + 0x4fa + 1] = 0;   /* Stage 1, Slot 0 := 2 */
        re15_debug_menu_set_bin(mod, (unsigned)dbsz);
        while (m->stage != 0) TICKE(EDGE_STAGE);
        TICKE(EDGE_STAGE);
        CHECK(m->stage == 2, "Slot-0-Marker 2 sperrt Stage 1 (@0x8001492C beq v0,a0), got %d", m->stage);
        free(mod);
        re15_debug_menu_set_bin(dbin, (unsigned)dbsz);          /* echte Tabelle zurueck */
        while (m->stage != 0) TICKE(EDGE_STAGE);
    }

    /* Ausserhalb der JUMP-Zeile bewegt Rechts nichts (@0x80014698 nur Zeile 1). */
    press_step(PAD_DOWN);
    CHECK(m->row == 2, "auf Zeile 2 fuer den Gegentest");
    { uint8_t before = m->room_idx[0];
      press_step(PAD_RIGHT);
      CHECK(m->room_idx[0] == before, "ausserhalb der JUMP-Zeile aendert Rechts nichts"); }
    /* Zeile 2 + Quadrat = MEMORY VIEWER (Original state 3, LAB_80014e78) — im Port OFFEN:
     * kein Laden, Menue bleibt offen. */
    CHECK(TICKE(EDGE_LOAD) == 0, "Quadrat auf Zeile 2 laedt keinen Raum");
    CHECK(re15_debug_menu_open(), "Menue bleibt offen (Memory Viewer im Port OFFEN)");
    press_step(PAD_UP);
    CHECK(m->row == 1, "zurueck auf JUMP");

    /* Laden: Quadrat-EDGE (@0x80014A38) -> want_load, Raum = (stage+1)<<8 | idx
     * (@0x8001D644/60, hardware-verifiziert BRIEFING=0x114). */
    re15_debug_menu_sync_cursor(0x1140);
    CHECK(TICKE(EDGE_LOAD) == 1, "Quadrat loest das Laden aus (@0x80014A38)");
    CHECK(m->load_room == 0x114, "geladen wird 0x114 BRIEFING ROOM, got 0x%03x", m->load_room);
    CHECK(!re15_debug_menu_open(), "Laden schliesst das Menue (DAT_800bbe5c=0 @0x80014A50-Block)");

    /* Verlassen: Kreuz-EDGE 0x40 (lhu ac762 @0x80014664, andi @0x8001466C) — NICHT held. */
    re15_debug_menu_toggle();
    HOLD(0);  /* ein Leer-Tick */
    re15_debug_menu_tick(EDGE_EXIT /* held */, 0);
    CHECK(re15_debug_menu_open(), "GEHALTENES Kreuz schliesst NICHT (Original prueft die EDGE)");
    TICKE(EDGE_EXIT);
    CHECK(!re15_debug_menu_open(), "Kreuz-EDGE schliesst (@0x8001466C -> sw 2 @0x8001467C)");

    /* SELECT-EDGE schliesst ein offenes Menue (@0x80014468 bne -> @0x80014524 sw 2). */
    re15_debug_menu_toggle();
    TICKE(EDGE_SEL);
    CHECK(!re15_debug_menu_open(), "SELECT-EDGE schliesst das offene Menue");

    /* point_at: Cursor per Tabelle setzen, Sprung erst durch die Lade-Flanke. */
    CHECK(re15_debug_menu_point_at(0x1140) == 1, "point_at findet ROOM1140");
    CHECK(m->stage == 0 && m->room_idx[0] == 0x14 && m->row == 1, "point_at stellt Stage/Index/Zeile");
    CHECK(TICKE(EDGE_LOAD) == 1 && m->load_room == 0x114, "Lade-Flanke nach point_at springt 0x114");

    free(dbin);
    if (fails == 0) printf("test_debug_menu: OK\n");
    else            printf("test_debug_menu: %d FEHLER\n", fails);
    return fails ? 1 : 0;
}
