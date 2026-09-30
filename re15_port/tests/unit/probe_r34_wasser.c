/*
 * probe_r34_wasser — Runde 34 Integration W7: ESP-Routinen 41 (@0x80018ef4) und 42 (@0x80018f98) am
 * Raum-Effekt 0x0b (Wasserstrahl ROOM2000/2001/20B0/20B1), echte Engine (re15_esp_fx_tick), echte Daten
 * (STAGE2/ROOM2000.RDT, Effekt 0x0b ab 0x15110: 6 Stroeme, Zeile 0 A=41, Zeile 1 A=42).
 *
 * Belege (selbst disassembliert, info/Re1.5/PSX.EXE; Kommentare in engine/src/re15_esp.c case 41/42):
 *   R41  Flags := row[0x0e] @0x80018f04/0c; row[0x16] != 0 -> -- und Ende @0x80018f1c-3c; sonst
 *        Flags := row[0x1e] @0x80018f40/48, +0x6e := row[0x26] @0x80018f58/60 (Delay-Slot), Vorschub
 *        @0x80018f5c, +0x0a += FUN_8001af20(a0 = u32 neue Zeile +0x18) & 3 @0x80018f64-84.
 *   R42  row[0x0e] < row[0x26] @0x80018fa8-b8 -> row[0x0e]++ @0x80018fc0-cc; nur Effekt-Id 0x0b
 *        @0x80018fc4/d4: Treffer FUN_8002b7e8(wpos, 0x2d) @0x80018fd8-0x80019000 -> Flags |= 0x20
 *        @0x80019018-20, sonst Flags := 0x13 @0x80019024-30; row[0x0e] >= 16 -> defH += 768
 *        @0x80019040-64. Sonst SCHLEIFE @0x80019068-174: Zeile 0 neu, Cursor 0, row[0x16] 0, xlat 0,
 *        +0x6e := 1, +0x6d := Satz[1].Byte2.
 *   Spawn-Anim (FUN_80019700 @0x8001989c-bc): +0x6e := 1, +0x6d := Satz[0].Byte2 (Effekt 0x0b: 3).
 *   Kein Bodenklemmen im Takt (@0x8001a2fc-388) -> der Strahl faellt.
 *
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten verletzten Pruefung.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_esp.h"
#include "re15_scd.h"
#include "re15_actor.h"

static int s_fail = 0, s_pass = 0;
#define PRUEF(nr, cond, ...) do { if (cond) { s_pass++; printf("  ok   %3d: ", nr); printf(__VA_ARGS__); printf("\n"); } \
    else { printf("  FAIL %3d: ", nr); printf(__VA_ARGS__); printf("\n"); if (!s_fail) s_fail = nr; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)s; return b;
}
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static unsigned ru16(const re15_esp_fx_t *f, int o) { return (unsigned)(f->row[o] | (f->row[o + 1] << 8)); }

static re15_esp_t s_core, s_raum;
static uint8_t *s_rdt = NULL; static size_t s_rdt_n = 0;

static void welt_leer(void)
{
    memset(g_actors, 0, sizeof(g_actors));
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;           /* Spieler weit weg (Treffer-Test prueft ihn immer) */
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 90000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 90000;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hit_radius_min = 450; g_actors[RE15_ACTOR_SLOT_PLAYER].hit_radius_max = 450;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hit_height = 765;
}

/* Sce_espr_on ROOM2000 main00 `3a 00 0b 00 00 00 00 19 a4 a2 4c eb c0 2b 00 04`: Effekt 0x0b sub 0,
 * Skala 0x1900, (-23900,-5300,11200), param 0x400; der SCD-Weg uebergibt floor_y = y (scd_vm.c). */
static int spawn(void)
{
    re15_esp_fx_reset();
    g_re15_pauseflags = 0;
    return re15_esp_fx_spawn_rows(&s_raum, 0x0b, 0, 0x1900, -23900, -5300, 11200, -5300, 0x400);
}

int main(void)
{
    char p[600];
    size_t nc = 0;
    snprintf(p, sizeof p, "%s/DATA/CORE00.ESP", RE15_ASSET_PSX_DIR);
    uint8_t *core = slurp(p, &nc);
    snprintf(p, sizeof p, "%s/STAGE2/ROOM2000.RDT", RE15_ASSET_PSX_DIR);
    s_rdt = slurp(p, &s_rdt_n);
    if (!core || !s_rdt) { printf("FAIL: Assets fehlen\n"); return 1; }
    if (re15_esp_parse_global(core, nc, &s_core) != 0) { printf("FAIL: CORE00\n"); return 1; }
    re15_esp_set_global_bank(&s_core);
    if (re15_esp_parse(s_rdt, s_rdt_n, u32(s_rdt + 0x4c), u32(s_rdt + 0x50), u32(s_rdt + 0x54),
                       u32(s_rdt + 0x58), &s_raum) != 0) { printf("FAIL: ROOM2000-ESP\n"); return 1; }
    re15_esp_set_room_bank(&s_raum);
    printf("=== probe_r34_wasser (Integration W7: ESP-Routinen 41/42, Effekt 0x0b ROOM2000) ===\n");

    /* ---- 1: Spawn ------------------------------------------------------------------------ */
    welt_leer();
    int n = spawn();
    int alle41 = 1;
    for (int i = 0; i < 6; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i); if (!f || ru16(f, 0) != 41 || f->flags != 0x03) alle41 = 0; }
    PRUEF(1, n == 6 && alle41, "6 Stroeme, Zeile 0 A=41, Flags 0x03 (n %d)", n);
    const re15_esp_fx_t *s0 = re15_esp_fx_get(0);
    PRUEF(2, s0 && s0->frame == 1 && s0->timer == 3,
          "Spawn-Anim wie FUN_80019700: Satz 1 / Zeitgeber Satz[0].Byte2 = 3 (frame %d timer %d)", s0 ? s0->frame : -1, s0 ? s0->timer : -1);

    /* ---- 2: Bild 1 ------------------------------------------------------------------------ */
    re15_esp_fx_tick(&s_raum);
    s0 = re15_esp_fx_get(0);
    PRUEF(10, s0 && ru16(s0, 0) == 42 && s0->row_cursor == 1 && s0->flags == 0x13,
          "Strom 0 (Halten 0): R41 gibt frei -> Zeile 1 A=42, Flags row[0x1e] 0x13 (A %u cursor %d fl %02x)",
          s0 ? ru16(s0, 0) : 0, s0 ? s0->row_cursor : -1, s0 ? s0->flags : 0);
    PRUEF(11, s0 && s0->frame == 0 && s0->timer == 2,
          "+0x6e := row[0x26] = 0 OHNE Zeitgeber, Anim-Schritt: Zeitgeber 3 -> 2, Satz bleibt 0 (frame %d timer %d)",
          s0 ? s0->frame : -1, s0 ? s0->timer : -1);
    PRUEF(12, s0 && s0->accel_y == 12 && ru16(s0, 0x0a) == 12,
          "+0x0a = 12 + (FUN_8001af20(u32 Zeile1+0x18 = 0) & 3) = 12 (accel_y %d row %u)", s0 ? s0->accel_y : 0, s0 ? ru16(s0, 0x0a) : 0);
    PRUEF(13, s0 && s0->xlat_x == 50 && s0->xlat_y == 0 && s0->drift_x == 49 && s0->drift_y == 12,
          "Physik mit Zeile 1: xlat (50,0) drift (49,12) (xlat %d,%d drift %d,%d)",
          s0 ? s0->xlat_x : 0, s0 ? s0->xlat_y : 0, s0 ? s0->drift_x : 0, s0 ? s0->drift_y : 0);
    {
        int ok = 1;
        for (int k = 1; k < 6; k++) {
            const re15_esp_fx_t *f = re15_esp_fx_get(k);
            if (!f || ru16(f, 0) != 41 || f->flags != 0x61 || ru16(f, 0x16) != (unsigned)(5 * k - 1) || f->xlat_x != 0) ok = 0;
        }
        PRUEF(14, ok, "Stroeme 1..5: R41 haelt (Flags 0x61 unsichtbar + Physik-/Bild-Stopp), Halten 5k -> 5k-1");
    }

    /* ---- 3: Zaehler, Wachsen, Fallen ----------------------------------------------------- */
    int zaehler_ok = 1, hoehe16 = -1, hoehe25 = -1;
    for (int t = 2; t <= 26; t++) {
        re15_esp_fx_tick(&s_raum);
        s0 = re15_esp_fx_get(0);
        if (!s0 || ru16(s0, 0) != 42 || ru16(s0, 0x0e) != (unsigned)(t - 1)) zaehler_ok = 0;
        if (t == 17) hoehe16 = s0 ? (int)ru16(s0, 0x06) : -1;
        if (t == 26) hoehe25 = s0 ? (int)ru16(s0, 0x06) : -1;
        if (t == 11)
            PRUEF(20, s0 && s0->xlat_y == 660 && s0->flags == 0x13,
                  "kein Bodenklemmen (Takt @0x8001a2fc-388): xlat_y nach Bild 11 = 12*55 = 660 (%d), Flags 0x13 ohne Treffer (%02x)",
                  s0 ? s0->xlat_y : 0, s0 ? s0->flags : 0);
    }
    PRUEF(21, zaehler_ok, "R42: row[0x0e] = Bild - 1 fuer Bild 2..26 (Zaehler bis Grenze row[0x26] = 25)");
    PRUEF(22, hoehe16 == 0x19c4 + 768 && hoehe25 == 0x19c4 + 768 * 10,
          "defH ab Zaehler 16 je Bild +768: Zaehler 16 -> %#x (Soll %#x), 25 -> %#x (Soll %#x)",
          hoehe16, 0x19c4 + 768, hoehe25, 0x19c4 + 768 * 10);

    /* ---- 4: Schleife (Bild 27) ------------------------------------------------------------ */
    re15_esp_fx_tick(&s_raum);
    s0 = re15_esp_fx_get(0);
    PRUEF(30, s0 && ru16(s0, 0) == 41 && s0->row_cursor == 0 && ru16(s0, 0x16) == 0 && s0->xlat_x == 0 &&
              s0->xlat_y == 0 && s0->frame == 1 && s0->timer == 2 && ru16(s0, 0x06) == 1,
          "Schleife: Zeile 0 (A 41, defH 1), Cursor 0, row[0x16] 0, xlat 0, Satz 1, Zeitgeber Satz[1].Byte2 3 -> 2 "
          "(A %u cursor %d h16 %u xlat %d,%d frame %d timer %d defH %u)", s0 ? ru16(s0, 0) : 0, s0 ? s0->row_cursor : -1,
          s0 ? ru16(s0, 0x16) : 0, s0 ? s0->xlat_x : 0, s0 ? s0->xlat_y : 0, s0 ? s0->frame : -1, s0 ? s0->timer : -1,
          s0 ? ru16(s0, 0x06) : 0);
    re15_esp_fx_tick(&s_raum);                              /* Bild 28: R41 sofort frei (Halten 0) */
    s0 = re15_esp_fx_get(0);
    PRUEF(31, s0 && ru16(s0, 0) == 42 && ru16(s0, 0x0e) == 0 && s0->xlat_x == 50,
          "Bild 28: R41 ohne Halten -> Zeile 1, Zaehler 0, xlat 50 (A %u z %u xlat %d)", s0 ? ru16(s0, 0) : 0,
          s0 ? ru16(s0, 0x0e) : 0, s0 ? s0->xlat_x : 0);
    {
        const re15_esp_fx_t *s3 = re15_esp_fx_get(3);
        /* Strom 3: Halten 15 -> frei im Bild 16 (15 Bilder --), also laengst Zeile 1; Satz startete bei
         * row[0x26] = 0x0b = 11 (Zeile 0 von Strom 3). */
        PRUEF(32, s3 && ru16(s3, 0) == 42 && s3->flags == 0x13,
              "Strom 3 (Halten 15) laeuft im Bild 28 in Zeile 1 (A %u fl %02x)", s3 ? ru16(s3, 0) : 0, s3 ? s3->flags : 0);
    }

    /* ---- 5: Strom 3 im Freigabebild --------------------------------------------------------- */
    welt_leer();
    spawn();
    for (int t = 1; t <= 15; t++) re15_esp_fx_tick(&s_raum);
    const re15_esp_fx_t *s3 = re15_esp_fx_get(3);
    PRUEF(40, s3 && ru16(s3, 0) == 41 && ru16(s3, 0x16) == 0 && s3->flags == 0x61,
          "Strom 3 nach Bild 15: noch Zeile 0, Halten 0, Flags 0x61 (A %u h %u fl %02x)", s3 ? ru16(s3, 0) : 0,
          s3 ? ru16(s3, 0x16) : 0, s3 ? s3->flags : 0);
    re15_esp_fx_tick(&s_raum);
    s3 = re15_esp_fx_get(3);
    /* Satz: R41 setzt +0x6e := row[0x26] = 11 ohne den Zeitgeber. Der Zeitgeber lief waehrend des
     * Haltens mit Bild-Stopp (Flags 0x61, Bit 6: Satz bleibt, Zeitgeber wird neu geladen @0x8001a3a8-
     * 460) ab 3 (Spawn, Satz[0].Byte2) mit Periode 3: nach Bild 15 steht er auf 0 -> der Anim-Schritt
     * des Freigabebilds (jetzt ohne Bild-Stopp) zaehlt weiter: 11 -> 12, Zeitgeber Satz[12].Byte2 3 -> 2. */
    PRUEF(41, s3 && ru16(s3, 0) == 42 && s3->flags == 0x13 && s3->frame == 12 && s3->timer == 2,
          "Strom 3 Bild 16: frei, Flags 0x13, Satz row[0x26] = 11, Zeitgeber 0 -> Anim-Schritt 12 / 2 "
          "(A %u fl %02x frame %d timer %d)", s3 ? ru16(s3, 0) : 0, s3 ? s3->flags : 0, s3 ? s3->frame : -1,
          s3 ? s3->timer : -1);

    /* ---- 6: RNG-Weg von R41 mit einem Zeilenwort != 0 (Zeile 1 von Strom 0 im eigenen RDT-Puffer) -- */
    welt_leer();
    spawn();
    s0 = re15_esp_fx_get(0);
    uint8_t *z1 = (uint8_t *)(uintptr_t)(s0->rows_base + 40);   /* Zeile 1 (der Puffer gehoert der Sonde) */
    uint8_t alt[4]; memcpy(alt, z1 + 0x18, 4);
    z1[0x18] = 0x81; z1[0x19] = 0; z1[0x1a] = 0; z1[0x1b] = 0;  /* w = 0x81: (0x81 + 1) & 0xff = 0x82, & 3 = 2 */
    re15_esp_fx_tick(&s_raum);
    s0 = re15_esp_fx_get(0);
    PRUEF(50, s0 && s0->accel_y == 14, "w = 0x81 -> v = (0x81 + ((0x81>>7)&0xff)) & 0xff = 0x82, +0x0a = 12 + 2 = 14 (%d)", s0 ? s0->accel_y : 0);
    memcpy(z1 + 0x18, alt, 4);

    /* ---- 7: Treffer-Test FUN_8002b7e8 (Radius 0x2d) ------------------------------------------ */
    welt_leer();
    spawn();
    re15_esp_fx_tick(&s_raum);                              /* Bild 1: Strom 0 in Zeile 1 */
    re15_esp_fx_tick(&s_raum);                              /* Bild 2: wpos steht */
    s0 = re15_esp_fx_get(0);
    re15_actor_t *z = &g_actors[1];
    z->active = 1; z->type = 0x10;
    z->x = s0->wpos[0]; z->z = s0->wpos[2]; z->y = s0->wpos[1] + 500;
    z->hit_radius_min = 300; z->hit_radius_max = 300; z->hit_height = 600; z->hit_offset_y = -500;
    int32_t xl = s0->xlat_x;
    re15_esp_fx_tick(&s_raum);                              /* Bild 3: R42 trifft -> Flags |= 0x20 */
    s0 = re15_esp_fx_get(0);
    int32_t xl3 = s0->xlat_x;
    PRUEF(60, s0 && s0->flags == 0x33, "Treffer (Gegner an wpos): Flags 0x13 | 0x20 = 0x33 (%02x)", s0 ? s0->flags : 0);
    PRUEF(61, xl3 == xl, "Physik-Stopp im Trefferbild: xlat_x bleibt %d (%d)", xl, xl3);
    z->x += 5000;                                           /* Gegner weg */
    re15_esp_fx_tick(&s_raum);
    s0 = re15_esp_fx_get(0);
    PRUEF(62, s0 && s0->flags == 0x13 && s0->xlat_x != xl3, "NEGATIV: ohne Treffer Flags := 0x13, Physik laeuft (%02x, xlat %d)",
          s0 ? s0->flags : 0, s0 ? s0->xlat_x : 0);

    printf("probe_r34_wasser: %s (%d ok, erste Verletzung %d)\n", s_fail ? "ROT" : "ALLE PRUEFUNGEN GRUEN", s_pass, s_fail);
    return s_fail;
}
