/* test_r38_tagebuch.c — RIEGEL Spur G (Runde 38) "tagebuch": Roys Diary statt der Code-Nachricht (Elza, ROOM1011),
 * danach die Kartenkette Communications Room -> Prison Room. Dossier analysis/befunde_runde38/G_tagebuch.md, Belege
 * include/re15_tagebuch_r38.h.
 *
 * Teile (je ein ctest-Eintrag):
 *   dokument  Dokument 5 der Tabelle (Item 0x4D, Bildsatz 30, max_page 5, H 176, Name "Roys Diary"); die acht
 *             FILE30-Dateien bytegleich dem Satz (FNV-1a + Laenge aus G_belege/FILE30_satz.txt), Papier == FILE16;
 *             ueber den ECHTEN Seitenlader re15_re2doc_pixel: "0513" gruen (TEX.TIM CLUT-Zeile 482, @0x80028974-94),
 *             ALLE gruenen Pixel im Code-Kasten, keine weissen darin, keine andere Seite gruen.
 *   ablauf    echte VM + Statusschirm: Aktionstaste am liegenden Roy -> sub07 (Ereignis 7) parkt an @0x00E64
 *             Message_on 18, KEINE Nachricht offen, Leser auf der Titelseite (Bildsatz 30, 6 Seiten, Liste Platz 0 =
 *             Dokument 5) -> "filed"-Meldung mit Id 0x4D -> Bit (9,68). Danach laeuft sub07 weiter; die Kartenkette
 *             kommt SOFORT nach dem Leser (Runde 39 Spur H; vorher erst am Evt_end @0x00E82): Eintrag ROOM10F1 (Blatt 3 / Rechteck 9) -> Folge ROOM1211 (Blatt 1
 *             / Rechteck 3), zeitgesteuert, schliesst. Zweites Untersuchen: wieder das Tagebuch, NUR LESEN (Liste
 *             bleibt 1 Eintrag, keine Meldung, kein Hinweis).
 *   karte     Ziele in der normalen Karte: vor dem Lesen keine; danach Comms (3/9) + Prison (1/3), Blaetter 3 und 1
 *             waehlbar; Prison Key (9,83) -> nur Prison; John frei (4,0xFC) -> keines. Leons drei Eintraege
 *             unveraendert und ohne Elza-Flags still.
 *   leon      Gegenprobe ROOM1010 (Leon) mit denselben Flags und demselben Druck: kein Dokument, keine Kette.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_gameflow.h"
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_files.h"
#include "re15_re2doc.h"
#include "re15_map_hint.h"
#include "re15_tagebuch_r38.h"
#include "re15_schluessel_r36.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_ASSET_RE2_DIR
#error RE15_ASSET_RE2_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
extern uint8_t g_aot_action_pressed;

static int g_fail = 0, g_n = 0;
#define PRUEF(c, ...) do { g_n++; if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ================================================================== dokument */
typedef struct { const char *name; long len; uint32_t fnv; } datei_t;
/* G_belege/FILE30_satz.txt (tools/r38_g/tagebuch_satz.py); FNV-1a 32 der Dateibytes. */
static const datei_t k_dateien[] = {
    { "FILE30_title_page.TIM",  22592, 0x698974c5u },
    { "FILE30_p00_page.TIM",    22592, 0x698974c5u },
    { "FILE30_p01_page.TIM",    22592, 0x2945796eu },
    { "FILE30_p02_page.TIM",    22592, 0x20319a0au },
    { "FILE30_p03_page.TIM",    22592, 0xd8cb5933u },
    { "FILE30_p04_page.TIM",    22592, 0x05285860u },
    { "FILE30_p05_page.TIM",    22592, 0x2a4f6f0fu },
    { "FILE30_title_paper.TIM", 33312, 0x322da36eu },   /* == FILE16_title_paper.TIM */
};

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint32_t fnv1a(const uint8_t *b, size_t n)
{
    uint32_t h = 0x811c9dc5u;
    for (size_t i = 0; i < n; i++) { h ^= b[i]; h *= 0x01000193u; }
    return h;
}

typedef struct { uint8_t r, g, b; } rgb_t;
static rgb_t rgb555(uint16_t c)
{
    rgb_t o = { (uint8_t)((c & 31) << 3), (uint8_t)(((c >> 5) & 31) << 3), (uint8_t)(((c >> 10) & 31) << 3) };
    return o;
}

/* DATA/TEX.TIM: Zeile `zeile` des CLUT-Blocks (Kopf x 256 y 480), Eintraege 0..15 (wie test_r35_inhalt_doku.c). */
static int tex_zeile(int zeile, uint16_t out[16])
{
    char p[512];
    snprintf(p, sizeof p, "%s/DATA/TEX.TIM", RE15_ASSET_PSX_DIR);
    size_t n = 0;
    uint8_t *b = slurp(p, &n);
    if (!b || n < 20) { free(b); return 0; }
    int cx = b[12] | b[13] << 8, cy = b[14] | b[15] << 8, cw = b[16] | b[17] << 8, ch = b[18] | b[19] << 8;
    int ok = (b[0] == 0x10 && (b[4] & 8) && cx == 256 && cy == 480 && zeile < ch && 20 + (size_t)(zeile + 1) * cw * 2 <= n);
    if (ok) for (int i = 0; i < 16; i++) out[i] = (uint16_t)(b[20 + zeile * cw * 2 + 2 * i] | b[20 + zeile * cw * 2 + 2 * i + 1] << 8);
    free(b);
    return ok;
}

static int in_menge(const rgb_t *m, uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < 6; i++) if (m[i].r == r && m[i].g == g && m[i].b == b) return 1;
    return 0;
}

static void teil_dokument(void)
{
    printf("== dokument ==\n");
    const re15_file_doc_t *d = re15_files_doc(RE15_TAGEBUCH_R38_DOK);
    PRUEF(d && d->item_id == 0x4D && d->item_id == RE15_FILES_FIRST_ITEM_ID + RE15_TAGEBUCH_R38_DOK,
          "Dokument %d = Item-Id 0x%02X (0x48 + 5; RE2 `addiu a0,a3,-104` @0x80071d04)", RE15_TAGEBUCH_R38_DOK,
          d ? d->item_id : 0);
    PRUEF(d && d->bildsatz == RE15_TAGEBUCH_R38_BILDSATZ && d->max_page == RE15_TAGEBUCH_R38_MAX_PAGE &&
          d->page_h == RE15_TAGEBUCH_R38_SEITE_H, "Bildsatz %d, max_page %d, H %d (Soll 30/5/176)",
          d ? d->bildsatz : -1, d ? d->max_page : -1, d ? d->page_h : -1);
    PRUEF(re15_files_doc_from_item(0x4D) == RE15_TAGEBUCH_R38_DOK && re15_files_doc_from_item(0x4C) == 4,
          "Item 0x4D -> Dokument 5, 0x4C bleibt Dokument 4 (Armory Notice)");
    {
        char name[40]; int k = 0;
        for (const uint8_t *p = d ? d->name : (const uint8_t *)"\x07"; *p != 0x07 && k < 39; p++)
            name[k++] = (*p == 0x00) ? ' ' : (char)(*p + 0x24);   /* Kodierung "Chris' Diary" @0x800c4e04 */
        name[k] = 0;
        PRUEF(strcmp(name, "Roys Diary") == 0, "Listenname dekodiert \"%s\" (NUTZER-VORGABE \"Header: Roys Diary\")",
              name);
    }
    char pfad[700];
    for (size_t i = 0; i < sizeof k_dateien / sizeof k_dateien[0]; i++) {
        size_t n = 0;
        snprintf(pfad, sizeof pfad, "%s/FILES/%s", RE15_ASSET_RE2_DIR, k_dateien[i].name);
        uint8_t *b = slurp(pfad, &n);
        PRUEF(b && (long)n == k_dateien[i].len && fnv1a(b, n) == k_dateien[i].fnv, "%s %ld B FNV %08x (Soll %08x)",
              k_dateien[i].name, (long)n, b ? fnv1a(b, n) : 0u, k_dateien[i].fnv);
        free(b);
    }
    {
        size_t n1 = 0, n2 = 0;
        snprintf(pfad, sizeof pfad, "%s/FILES/FILE16_title_paper.TIM", RE15_ASSET_RE2_DIR);
        uint8_t *a = slurp(pfad, &n1);
        snprintf(pfad, sizeof pfad, "%s/FILES/FILE30_title_paper.TIM", RE15_ASSET_RE2_DIR);
        uint8_t *b = slurp(pfad, &n2);
        PRUEF(a && b && n1 == n2 && memcmp(a, b, n1) == 0,
              "Hintergrund FILE30_title_paper.TIM == FILE16_title_paper.TIM (NUTZER-VORGABE, RE2 \"Chief's diary\")");
        snprintf(pfad, sizeof pfad, "%s/FILES/FILE30_p06_page.TIM", RE15_ASSET_RE2_DIR);
        FILE *f = fopen(pfad, "rb");
        PRUEF(!f, "kein FILE30_p06 (max_page 5 = letzte Seite)");
        if (f) fclose(f);
        free(a); free(b);
    }

    /* Gruen ueber den ECHTEN Seitenlader. */
    char root[512];
    snprintf(root, sizeof root, "%s/FILES", RE15_ASSET_RE2_DIR);
    re15_re2doc_set_root(root);
    uint16_t z0[16], z2[16];
    PRUEF(tex_zeile(0, z0) && tex_zeile(2, z2), "TEX.TIM CLUT Zeilen 0 und 2 (y 480/482: 05 01 -> @0x80028974-94)");
    rgb_t gruen[6], weiss[6];
    for (int i = 0; i < 6; i++) { gruen[i] = rgb555(z2[i + 1]); weiss[i] = rgb555(z0[i + 1]); }
    int w = 0, h = 0;
    PRUEF(re15_re2doc_size(RE15_TAGEBUCH_R38_BILDSATZ, 5, RE15_RE2DOC_PAGE, &w, &h) && w == 256 && h == 176,
          "Seite p05 %dx%d (256x176)", w, h);
    /* G_belege/FILE30_satz.txt: "p05 Zeile 5 (y 80..95): '0513' Kern-x 11..42 gruen" */
    const int bx0 = 11, bx1 = 42, by0 = 80, by1 = 95;
    int gruen_n = 0, g_box = 0, w_box = 0, w_n = 0, gx0 = 999, gx1 = -1;
    for (int v = 0; v < h; v++)
        for (int u = 0; u < w; u++) {
            uint8_t r, g, b;
            if (!re15_re2doc_pixel(RE15_TAGEBUCH_R38_BILDSATZ, 5, RE15_RE2DOC_PAGE, u, v, &r, &g, &b)) continue;
            int in_box = (u >= bx0 && u <= bx1 && v >= by0 && v <= by1);
            if (in_menge(gruen, r, g, b)) { gruen_n++; if (in_box) g_box++; if (u < gx0) gx0 = u; if (u > gx1) gx1 = u; }
            if (in_menge(weiss, r, g, b)) { w_n++; if (in_box) w_box++; }
        }
    PRUEF(gruen_n == 87 && g_box == gruen_n, "p05: %d gruene Kernpixel (Soll 87 = Kern der Glyphen 0,5,1,3), alle im Code-Kasten "
          "x %d..%d y %d..%d (%d)", gruen_n, bx0, bx1, by0, by1, g_box);
    PRUEF(gx0 == bx0 && gx1 == bx1 && w_box == 0, "ganzer Code gruen (x %d..%d), kein weisser Kernpixel im Kasten (%d)",
          gx0, gx1, w_box);
    PRUEF(w_n > 1000, "der uebrige Text bleibt weiss (%d Kernpixel)", w_n);
    int andere = 0;
    for (int p = -1; p <= 4; p++) {
        int ww = 0, hh = 0;
        if (!re15_re2doc_size(RE15_TAGEBUCH_R38_BILDSATZ, p, RE15_RE2DOC_PAGE, &ww, &hh)) { andere += 1000; continue; }
        for (int v = 0; v < hh; v++)
            for (int u = 0; u < ww; u++) {
                uint8_t r, g, b;
                if (re15_re2doc_pixel(RE15_TAGEBUCH_R38_BILDSATZ, p, RE15_RE2DOC_PAGE, u, v, &r, &g, &b) &&
                    in_menge(gruen, r, g, b)) andere++;
            }
    }
    PRUEF(andere == 0, "Titelseite + p00..p04: 0 gruene Pixel (%d)", andere);
    PRUEF(re15_re2doc_page_count(RE15_TAGEBUCH_R38_BILDSATZ) == RE15_TAGEBUCH_R38_MAX_PAGE + 2,
          "Seitenbestand: Titel + p00..p05 = %d Dateien", re15_re2doc_page_count(RE15_TAGEBUCH_R38_BILDSATZ));
}

/* ================================================================== Raum + Bilder */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0, s_bild = 0;
static uint16_t           s_rdt_raum = 0;
static uint64_t           s_uhr = 1000000;
static int                s_hint_gesehen = -1, s_hint_bild = -1;

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c: ist das Spiel eingefroren (Statusschirm samt Blenden),
 * laeuft nur der Schirm (re15_game_step -> re15_menu_fsm_tick, Blende im Renderer); sonst VM, Gaenger, Meldungen,
 * Spielschritt. Die Wanduhr laeuft 1/60 s je Bild (der Hinweis-Blinker liest sie). */
static void bild(uint16_t pressed, uint16_t held)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = pressed;
    if (re15_menu_gameplay_frozen()) {
        re15_game_step(&s_ctx);
        if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
        re15_fade_tick();
        s_bild++;
        return;
    }
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut = s_shown;
    re15_tagebuch_r38_tick();          /* dieselbe Stelle wie im Spielschritt: die Anforderung ist hier noch sichtbar */
    if (re15_map_hint_pending() >= 0 && s_hint_gesehen < 0) { s_hint_gesehen = re15_map_hint_pending(); s_hint_bild = s_bild; }
    re15_game_step(&s_ctx);
    re15_fade_tick();
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
    s_bild++;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = 0; s_cine_was = 0; s_bild = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_map_hint_room_scan(s_raw, (int)s_rawsz, room);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* EIN Aktionsdruck (Viereck) am ECHTEN Weg: Spielschritt mit der Taste (game_step_common.c:1110 g_aot_action_pressed
 * aus SQUARE, Aktions-Scan, scd_event_fire des gefeuerten Ereignisses game_step_common.c:2277). */
static void druck(int32_t x, int32_t z, int16_t rot)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->z = z; pl->rot_y = rot;
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
}

/* Datei-Offset des ersten Fadens, der im RDT-Puffer steht und zwischen lo und hi parkt (-1 = keiner). */
static long faden_zwischen(unsigned lo, unsigned hi)
{
    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        const scd_thread_t *t = &g_scd.threads[i];
        if (!t->active || !t->pc || t->pc < s_rdt.raw || t->pc >= s_rdt.raw + s_rawsz) continue;
        unsigned o = (unsigned)(t->pc - s_rdt.raw);
        if (o >= lo && o <= hi) return (long)o;
    }
    return -1;
}

static int bis_leser(int max)
{
    int n = 0;
    while (n++ < max && !(re15_menu_phase() == 1 && g_inv_screen.item_state == 3)) bild(0, 0);
    return n;
}

/* Stand nach dem Dialog sub02: (4,0xF1)/(4,0xFA)/(3,0x74) = @0x00AFE/@0x00B02/@0x00B06; Elza (Figur 4). Elza steht vor
 * Roy, Blick +z (Gier 3072: Vorwaertspunkt x + 620 cos, z - 620 sin @0x80042bd0) -> (-1900, 7220) im Rechteck von
 * Slot 5 (x -2400..-1400, z 6950..7950, sub00 @0x00A52). */
#define STAND_X  (-1900)
#define STAND_Z  6600
#define STAND_R  3072

static void elza_1011(void)
{
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    g_gameflow.character = 4;
    re15_game_flag_set(4, 0xF1, 1); re15_game_flag_set(4, 0xFA, 1); re15_game_flag_set(3, 0x74, 1);
    (void)room_boot(0x1011, STAND_X, STAND_Z, STAND_R);
    for (int f = 0; f < 20; f++) bild(0, 0);
}

static void teil_ablauf(void)
{
    printf("== ablauf ==\n");
    elza_1011();
    PRUEF(g_aot.slots[5].active && g_aot.slots[5].event_id == 6,
          "nach dem Raumaufbau: Slot 5 (Roy) -> Ereignis 6 (sub00 @0x00A52)");
    s_hint_gesehen = -1;
    druck(STAND_X, STAND_Z, STAND_R);
    int angefordert = -1;
    for (int f = 0; f < 200 && angefordert < 0; f++) {
        if (re15_menu_doc_active()) { angefordert = s_bild; break; }
        bild(0, 0);
    }
    long parkt = faden_zwischen(0x00E64, 0x00E64);
    PRUEF(angefordert >= 0 && parkt == 0x00E64, "Aktionstaste -> sub07 parkt an @0x00E64 (Message_on 18), Leser "
          "angefordert in Bild %d", angefordert);
    PRUEF(!g_scd.message_active && g_scd.message_id != RE15_TAGEBUCH_R38_MSG,
          "KEINE Nachricht offen (msg 18 \"He has passed out ... 0513\" entfaellt)");
    int n = bis_leser(400);
    PRUEF(g_inv_screen.item_state == 3 && g_inv_screen.file_reader_page == 0 &&
          g_inv_screen.file_bildsatz == RE15_TAGEBUCH_R38_BILDSATZ && g_inv_screen.file_end == RE15_TAGEBUCH_R38_MAX_PAGE + 1,
          "Leser nach %d Bildern: Titelseite, Bildsatz %d, %d Seiten", n, (int)g_inv_screen.file_bildsatz,
          (int)g_inv_screen.file_end);
    PRUEF(re15_files_get(0) == RE15_TAGEBUCH_R38_DOK && re15_files_count() == 1 &&
          !re15_game_flag_get(9, RE15_TAGEBUCH_R38_BIT),
          "schon beim Lesen in der FILE-Liste (Platz 0 = Dokument 5; RE2 @0x80071d00 vor dem Lesen), Bit 68 noch frei");
    /* durchblaettern bis zur Ende-Stellung, dort schliessen (Viereck) */
    for (int k = 0; k < RE15_TAGEBUCH_R38_MAX_PAGE + 1; k++) {
        bild(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);
        for (int f = 0; f < 40; f++) bild(0, 0);
    }
    PRUEF(g_inv_screen.file_reader_page == g_inv_screen.file_end, "RECHTS x6 -> Ende-Stellung (Seite %d von %d)",
          (int)g_inv_screen.file_reader_page, (int)g_inv_screen.file_end);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    uint8_t mid = 0; int rev = -1, total = re15_menu_doc_msg_total();
    n = 0;
    while (n++ < 400 && re15_menu_doc_msg(&mid, &rev) && rev < total) bild(0, 0);
    PRUEF(mid == 0x4D && rev == total && total > 0, "Meldung \"The Roys Diary has been filed.\" steht (Id 0x%02X, %d Glyphen)",
          mid, total);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    PRUEF(re15_game_flag_get(9, RE15_TAGEBUCH_R38_BIT) == 1, "Bestaetigen -> Bit (9,68)");
    n = 0;
    while (n++ < 300 && re15_menu_gameplay_frozen()) bild(0, 0);
    PRUEF(!re15_menu_gameplay_frozen() && s_hint_gesehen < 0, "Schirm zu nach %d Bildern, Hinweis noch nicht angefordert", n);
    /* RUNDE 39 SPUR H (Nutzer: "nach aufnehmen von Roys Diary muss direkt die Karte kommen ... noch bevor das Spiel zum
     * bewegen wieder zurueckgegeben wird"): die Kette wird im ERSTEN freien Bild angefordert (tagebuch_r38.c
     * leser_zu_pruefen, vor re15_map_hint_pending im game_step); der Faden parkt dabei noch auf dem Message_on @0x00E64 —
     * kein Evt_next, kein Aufrichten (@0x00E6A), kein Plc_ret (@0x00E76) davor. Vorher: erst am Evt_end @0x00E82. */
    const long off_vor = faden_zwischen(0x00E64, 0x00E82);
    bild(0, 0);
    const long off_bei = faden_zwischen(0x00E64, 0x00E82);
    const int k1 = re15_map_hint_eintrag_fuer(RE15_TAGEBUCH_R38_RAUM, RE15_TAGEBUCH_R38_ZIEL1_RAUM);
    const int k2 = re15_map_hint_eintrag_fuer(RE15_TAGEBUCH_R38_RAUM, RE15_TAGEBUCH_R38_ZIEL2_RAUM);
    PRUEF(s_hint_gesehen == k1 && s_hint_bild == s_bild - 1 && off_vor == 0x00E64 && off_bei == 0x00E64 &&
          re15_tagebuch_r38_hinweis_faellig(),
          "Kette (Eintrag %d) im ERSTEN freien Bild %d angefordert; sub07 parkt auf @0x%05lX (Message_on), nicht am Evt_end",
          s_hint_gesehen, s_hint_bild, off_bei);
    PRUEF(re15_menu_gameplay_frozen(), "Hinweis-Schirm angenommen: Spiel steht wieder (keine Steuerung dazwischen)");
    /* der Hinweis-Schirm: Ziel 1, dann (Zeit) Ziel 2, dann zu */
    int p1 = -1, r1 = -1, p2 = -1, r2 = -1, s1 = 0, s2 = 0;
    n = 0;
    while (n++ < 2000 && (re15_menu_gameplay_frozen() || n < 5)) {
        bild(0, 0);
        if (g_inv_screen.hint_aktiv) {
            if (!s2 && g_inv_screen.hint_page == 3) { p1 = g_inv_screen.hint_page; r1 = g_inv_screen.hint_rect; s1++; }
            if (g_inv_screen.hint_page == 1) { p2 = g_inv_screen.hint_page; r2 = g_inv_screen.hint_rect; s2++; }
        }
    }
    PRUEF(p1 == 3 && r1 == 9 && s1 > 100, "Hinweis 1: Blatt %d Rechteck %d (Communications Room ROOM10F1), %d Bilder",
          p1, r1, s1);
    PRUEF(p2 == 1 && r2 == 3 && s2 > 100, "Hinweis 2 (Folge %d): Blatt %d Rechteck %d (Prison ROOM1211), %d Bilder",
          k2, p2, r2, s2);
    PRUEF(!re15_menu_gameplay_frozen() && !re15_menu_map_hint_active(), "Kette zu Ende, Schirm zu (%d Bilder)", n);
    /* Runde 39 Spur H: ERST JETZT laeuft sub07 hinter dem Message_on weiter (Evt_next, Aufrichten @0x00E6A, Sleep 30,
     * Plc_ret @0x00E76, Aot_reset @0x00E78, Evt_end @0x00E82). */
    PRUEF(faden_zwischen(0x00E64, 0x00E82) == 0x00E64 && re15_tagebuch_r38_hinweis_faellig(),
          "nach dem Hinweis: sub07 parkt noch auf dem Message_on (@0x%05lX), die Freigabe faellt in den naechsten VM-Schritt",
          faden_zwischen(0x00E64, 0x00E82));
    bild(0, 0);   /* der Haken liefert jetzt 1: PC hinter das Message_on, Evt_next/Plc_motion/Plc_flg/Sleep im selben Schritt */
    PRUEF(faden_zwischen(0x00E64, 0x00E82) > 0x00E64 && !re15_tagebuch_r38_hinweis_faellig(),
          "ein Bild spaeter: sub07 hinter dem Message_on (@0x%05lX), Kette abgehakt", faden_zwischen(0x00E64, 0x00E82));
    for (int f = 0; f < 200 && faden_zwischen(0x00E64, 0x00E82) >= 0; f++) bild(0, 0);
    PRUEF(faden_zwischen(0x00E64, 0x00E82) < 0, "sub07 bis zum Evt_end @0x00E82 durchgelaufen");
    PRUEF(g_aot.slots[5].active && g_aot.slots[5].event_id == 7, "Slot 5 wieder scharf -> Ereignis 7 (@0x00E78)");

    /* ---- zweites Untersuchen: wieder das Tagebuch, nur lesen ---- */
    s_hint_gesehen = -1;
    for (int f = 0; f < 10; f++) bild(0, 0);
    druck(STAND_X, STAND_Z, STAND_R);
    angefordert = -1;
    for (int f = 0; f < 200; f++) { if (re15_menu_doc_active()) { angefordert = s_bild; break; } bild(0, 0); }
    PRUEF(angefordert >= 0 && faden_zwischen(0x00E64, 0x00E64) == 0x00E64 && !g_scd.message_active,
          "zweites Untersuchen (Ereignis 7): wieder Roys Diary statt msg 18 (Bild %d)", angefordert);
    n = bis_leser(400);
    PRUEF(g_inv_screen.item_state == 3 && g_inv_screen.file_bildsatz == RE15_TAGEBUCH_R38_BILDSATZ &&
          re15_files_count() == 1 && re15_files_get(0) == RE15_TAGEBUCH_R38_DOK,
          "Leser zeigt Bildsatz 30; FILE-Liste unveraendert 1 Eintrag (kein zweites Anhaengen)");
    bild(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    int meldung = 0;
    n = 0;
    while (n++ < 300 && re15_menu_gameplay_frozen()) { if (re15_menu_doc_msg(NULL, NULL)) meldung = 1; bild(0, 0); }
    PRUEF(!meldung && !re15_menu_gameplay_frozen(), "Schliessen -> KEINE \"filed\"-Meldung, Schirm zu nach %d Bildern", n);
    for (int f = 0; f < 200; f++) bild(0, 0);
    PRUEF(s_hint_gesehen < 0 && re15_files_count() == 1, "kein zweiter Kartenhinweis, Liste weiter 1 Eintrag");
}

/* ================================================================== karte */
static void teil_karte(void)
{
    printf("== karte ==\n");
    re15_game_state_init();
    re15_map_visited_reset();
    const int alt = re15_map_hint_eintrag_fuer(0x1150, 0x10F0);
    const int a1  = re15_map_hint_eintrag_fuer(0x10F0, 0x11C0);
    const int a2  = re15_map_hint_eintrag_fuer(0x10F0, 0x1150);
    const int k1  = re15_map_hint_eintrag_fuer(RE15_TAGEBUCH_R38_RAUM, RE15_TAGEBUCH_R38_ZIEL1_RAUM);
    const int k2  = re15_map_hint_eintrag_fuer(RE15_TAGEBUCH_R38_RAUM, RE15_TAGEBUCH_R38_ZIEL2_RAUM);
    /* Runde 39 Spur P: + Eintrag ROOM1041 -> ROOM1141 hinter den Eintraegen dieser Spur (include/re15_johnruf_r39.h) */
    const int p1  = re15_map_hint_eintrag_fuer(0x1041, 0x1141);
    PRUEF(alt == 0 && a1 == 1 && a2 == 2 && k1 >= 3 && k2 >= 3 && k1 != k2 && p1 > k1 && p1 > k2 && re15_map_hint_anzahl() == 6,
          "Tabelle: Leon 0/1/2 an ihrem Platz, Elza %d/%d, Runde 39 P %d, 6 Eintraege", k1, k2, p1);
    PRUEF(re15_map_hint_folge(alt) == -1 && !re15_map_hint_zeitgesteuert(alt) &&
          re15_map_hint_folge(a1) == a2 && re15_map_hint_folge(a2) == -1 &&
          re15_map_hint_zeitgesteuert(a1) && re15_map_hint_zeitgesteuert(a2),
          "Leons Eintraege unveraendert (Runde 33: START schliesst; Runde 35 K: 11C0 -> 1150 zeitgesteuert)");
    PRUEF(re15_map_hint_folge(k1) == k2 && re15_map_hint_folge(k2) == -1 &&
          re15_map_hint_zeitgesteuert(k1) && re15_map_hint_zeitgesteuert(k2),
          "Elza: Kette Comms -> Prison -> Ende, beide zeitgesteuert (Form der Runde 35 K)");
    int p = -1, r = -1;
    PRUEF(re15_map_hint_ziel(k1, &p, &r) && p == 3 && r == 9, "Ziel 1 ROOM10F1 = Blatt %d Rechteck %d (2F, 3/9)", p, r);
    PRUEF(re15_map_hint_ziel(k2, &p, &r) && p == 1 && r == 3, "Ziel 2 ROOM1211 = Blatt %d Rechteck %d (B2, 1/3)", p, r);
    int zp = 0, zr = 0;
    PRUEF(!re15_map_ziel_aktiv_n(0, &zp, &zr) && !re15_map_ziel_blatt_frei(1) && !re15_map_ziel_blatt_frei(3),
          "vor dem Tagebuch: kein Ziel, Blaetter 1/3 nicht freigegeben");
    re15_game_flag_set(9, RE15_TAGEBUCH_R38_BIT, 1);
    int b0 = re15_map_ziel_aktiv_n(0, &zp, &zr); int q0p = zp, q0r = zr;
    int b1 = re15_map_ziel_aktiv_n(1, &zp, &zr); int q1p = zp, q1r = zr;
    int b2 = re15_map_ziel_aktiv_n(2, &zp, &zr);
    PRUEF(b0 && q0p == 3 && q0r == 9 && b1 && q1p == 1 && q1r == 3 && !b2,
          "nach dem Tagebuch: Ziel 0 = Comms (%d/%d), Ziel 1 = Prison (%d/%d), kein drittes", q0p, q0r, q1p, q1r);
    PRUEF(re15_map_ziel_blatt_frei(3) && re15_map_ziel_blatt_frei(1) && re15_map_blatt_waehlbar(3) &&
          re15_map_blatt_waehlbar(1), "Blaetter 2F (3) und B2 (1) in der normalen Karte waehlbar");
    re15_map_zone_update(0x10F1, -5000, 5000);                     /* nur betreten: Comms blinkt weiter */
    PRUEF(re15_map_ziel_aktiv_n(0, &zp, &zr) && zp == 3, "ROOM10F1 nur betreten (Zone besucht): Comms blinkt weiter");
    re15_game_flag_set(9, RE15_R36Q_PRISON_BIT, 1);
    b0 = re15_map_ziel_aktiv_n(0, &zp, &zr); q0p = zp;
    b1 = re15_map_ziel_aktiv_n(1, &zp, &zr);
    PRUEF(b0 && q0p == 1 && !b1, "Prison Key genommen (9,%d) -> nur noch Prison (Blatt %d)", RE15_R36Q_PRISON_BIT, q0p);
    re15_map_zone_update(0x1211, -26225, -20000);                  /* Zelle betreten, John noch drin */
    PRUEF(re15_map_ziel_aktiv_n(0, &zp, &zr) && zp == 1, "ROOM1211 betreten, John noch eingesperrt: Prison blinkt weiter");
    re15_game_flag_set(4, 0xFC, 1);
    PRUEF(!re15_map_ziel_aktiv_n(0, &zp, &zr), "John befreit (4,0xFC) @0x01F76 -> kein Ziel mehr");

    /* Leon: seine Flags schalten nur seine Ziele, nie die Elzas */
    re15_game_state_init();
    re15_map_visited_reset();
    re15_game_flag_set(3, 94, 1);
    b0 = re15_map_ziel_aktiv_n(0, &zp, &zr); q0p = zp; q0r = zr;
    b1 = re15_map_ziel_aktiv_n(1, &zp, &zr);
    PRUEF(b0 && q0p == 3 && q0r == 9 && !b1, "Leon nach der Irons-Szene (3,94): nur ROOM10F0 (3/9) wie bisher");
    re15_game_flag_set(9, 71, 1);
    re15_map_zone_update(0x10F0, -5000, 5000);
    b0 = re15_map_ziel_aktiv_n(0, &zp, &zr); q0p = zp; q0r = zr;
    b1 = re15_map_ziel_aktiv_n(1, &zp, &zr); q1p = zp;
    b2 = re15_map_ziel_aktiv_n(2, &zp, &zr);
    PRUEF(b0 && q0p == 0 && q0r == 4 && b1 && q1p == 4 && !b2,
          "Leon nach der 10F0-Szene: 11C0 (0/4) + 1150 (4/..), kein Elza-Ziel");
}

/* ================================================================== leon */
static void teil_leon(void)
{
    printf("== leon ==\n");
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    g_gameflow.character = 0;
    re15_game_flag_set(4, 0xF1, 1); re15_game_flag_set(4, 0xFA, 1); re15_game_flag_set(3, 0x74, 1);
    if (room_boot(0x1010, STAND_X, STAND_Z, STAND_R) != 0) return;
    for (int f = 0; f < 20; f++) bild(0, 0);
    s_hint_gesehen = -1;
    druck(STAND_X, STAND_Z, STAND_R);
    int doc = 0;
    for (int f = 0; f < 300; f++) { if (re15_menu_doc_active()) doc = 1; bild(0, 0); }
    PRUEF(!doc && re15_files_count() == 0 && !re15_game_flag_get(9, RE15_TAGEBUCH_R38_BIT) && s_hint_gesehen < 0,
          "ROOM1010 (Leon), gleicher Druck: kein Dokument, Liste leer, Bit 68 frei, kein Hinweis");
    /* der Haken antwortet nur in ROOM1011 auf msg 18 */
    scd_thread_t t; memset(&t, 0, sizeof t);
    static const uint8_t op18[4] = { 0x2b, 0x12, 0xff, 0xff }, op17[4] = { 0x2b, 0x11, 0xff, 0xff };
    t.pc = op18;
    int leon18 = re15_tagebuch_r38_message_on(&t);
    g_current_room_id = 0x1011; t.pc = op17;
    int elza17 = re15_tagebuch_r38_message_on(&t);
    PRUEF(leon18 == 0 && elza17 == 0, "Haken nicht zustaendig: ROOM1010 msg 18 (%d), ROOM1011 msg 17 (%d)", leon18, elza17);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    if (!strcmp(teil, "dokument"))    teil_dokument();
    else if (!strcmp(teil, "ablauf")) teil_ablauf();
    else if (!strcmp(teil, "karte"))  teil_karte();
    else if (!strcmp(teil, "leon"))   teil_leon();
    else { printf("Teil? dokument|ablauf|karte|leon\n"); return 2; }
    printf("%d Pruefungen, %d Fehler\n", g_n, g_fail);
    return g_fail ? 1 : 0;
}
