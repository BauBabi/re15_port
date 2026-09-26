/*
 * r26_panel_11f0.c — RIEGEL fuer das Boiler-Room-Bedienfeld (ROOM11F0):
 *   A  Der KLICK am Panel-Raetsel ist der RE2-RAUM-SE Gruppe 2 / Index 0x0A,
 *      nicht mehr der UI-Ton CORE-Satz 6.
 *   B  Der rote LEISTUNGS-ZEIGER: Wert aus den zehn Schalterbits, ein Punkt je Bild,
 *      Deckel 100 / Boden 0, Ziel 80 beim belegten Loesungsmuster, und die
 *      Bildschirm-y aus der selbst vermessenen Skalen-Eichung.
 *   C  Die BESTAETIGUNG (Gruppe 2 / 0x0C) haengt an der Loesungsflanke Set(4,238,1).
 *   D  Die Ja/Nein-AUSWAHL toent (CORE 4 beim Bewegen, 6/5 beim Bestaetigen).
 *   E  Die Voraussetzungen der Abtastphasen-Korrektur stehen in den AUSGELIEFERTEN
 *      Bytes (Cursor-TIM) — faellt eine davon, ist die Begruendung des Fixes hin.
 *
 * ALLE Belege mit Adresse: include/re15_panel_zeiger.h und include/re15_audio.h.
 *
 * RUECKBAU-NACHWEIS (in dieser Runde wirklich gefahren, Ergebnis im Ergebnisfeld):
 *   A  re15_audio_re2_panel_se(0x0A) -> re15_audio_core_se(6)        => A ROT
 *   B  RE15_PANEL_GEWICHT 16 -> 36 (RE2s Rohwert)                    => B ROT
 *   B  ein Punkt je Bild -> Sprung auf den Zielwert                  => B ROT
 *   D  die drei re15_audio_core_se-Zeilen in msg_common.c entfernt   => D ROT
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_light.h"
#include "re15_panel_zeiger.h"
#include "re15_audio.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;
extern int g_test_core_se_last,  g_test_core_se_count;
extern int g_test_panel_se_last, g_test_panel_se_count;
extern unsigned g_re15_panel_klick_zaehler;
extern unsigned g_re15_panel_bestaet_zaehler;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                g_fail = 0;
static int                s_shown = 0;

#define CHECK(name, cond) do {                                          \
        int _c = (cond);                                                \
        printf("  [%s] %s\n", _c ? "OK  " : "FAIL", (name));            \
        if (!_c) g_fail++;                                              \
    } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int room_boot(unsigned rid, const char *stage, int32_t px, int32_t pz, int16_t rot)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage, rid);
    if (s_raw) { free(s_raw); s_raw = NULL; }
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) return 0;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return 0;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = rid; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_panel_zeiger_reset();
    for (int f = 0; f < 30; f++) frame(0, 0);
    return 1;
}

/* ================= TEIL A: der Klick am Panel ist ein RE2-RAUM-SE ================= */
static void teil_a(void)
{
    printf("\n=== A: der Panel-Klick (RE2 Gruppe 2 / Index 0x0A) ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) { printf("SKIP: RDT fehlt\n"); return; }

    /* Skript-Anker: ohne die bleibt der Riegel bei stiller Datendrift gruen.
     * sub01 @Datei 0x10F4 Work_set(3,0) / @0x10FC Member_cmp(15==2) /
     *       @0x1106 Sce_key_ck(1,0x40). */
    int anker = (s_raw[0x10F4] == 0x2E && s_raw[0x10F5] == 0x03 && s_raw[0x10F6] == 0x00 &&
                 s_raw[0x10FC] == 0x3E && s_raw[0x10FE] == 0x0F && s_raw[0x1100] == 0x02 &&
                 s_raw[0x1106] == 0x51 && s_raw[0x1107] == 0x01 && s_raw[0x1108] == 0x40);
    CHECK("ROOM11F0 sub01 Bestaetigungs-Block unveraendert (@0x10F4/0x10FC/0x1106)", anker);

    /* Das Raetsel scharf schalten wie sub16 @Datei 0x015C2 ff. (13x `22 05 xx 01`). */
    for (int b = 0; b <= 12; b++) re15_game_flag_set(5, (uint8_t)b, 1);

    int pse0 = g_test_panel_se_count, cse0 = g_test_core_se_count;
    unsigned k0 = g_re15_panel_klick_zaehler;
    int schalter_vor[12];
    for (int n = 1; n <= 11; n++) schalter_vor[n] = re15_game_flag_get(5, (uint8_t)n);
    int umgelegt = 0;
    for (int f = 0; f < 90; f++) {
        frame(RE15_PAD_BIT_SQUARE, (uint16_t)(f == 0 ? RE15_PAD_BIT_SQUARE : 0));
        for (int n = 1; n <= 11; n++) {
            int j = re15_game_flag_get(5, (uint8_t)n);
            if (j != schalter_vor[n]) { umgelegt++; schalter_vor[n] = j; }
        }
    }
    int pse = g_test_panel_se_count - pse0;
    unsigned kl = g_re15_panel_klick_zaehler - k0;
    printf("  90 Bilder QUADRAT: Panel-SE %d (letzte Id 0x%02X), CORE-SE %d, "
           "Schalterwechsel %d\n",
           pse, g_test_panel_se_last, g_test_core_se_count - cse0, umgelegt);

    CHECK("das Skript hat dabei wirklich Schalter umgelegt (Gegenprobe)", umgelegt > 0);
    CHECK("der Tastendruck spielt ueberhaupt einen Panel-SE", pse > 0);
    CHECK("und zwar RE2 Gruppe 2 / Index 0x0A", g_test_panel_se_last == RE15_PANEL_SE_KLICK);
    CHECK("der Zaehler der Aufrufstelle zaehlt mit", kl == (unsigned)pse);
}

/* ================= TEIL B: der rote Zeiger ======================================= */
static void teil_b(void)
{
    printf("\n=== B: der Leistungs-Zeiger (Wert, Fahrt, Bildschirmlage) ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) { printf("SKIP: RDT fehlt\n"); return; }

    /* Skript-Anker der Loesungskette: sub01 @Datei 0x012BE..0x012E2, zehn `21 05 <bit> <soll>`
     * mit soll = 1,0,1,0,1,0,1,0,1,0 auf den Bits 13..22, danach `04 ff 18 12`
     * (Evt_exec sub18) @0x012E6 und `22 04 ee 01` (Set(4,238,1)) @0x012EA. */
    int kette = 1;
    for (int i = 0; i < 10; i++) {
        const uint8_t *r = &s_raw[0x012BE + i * 4];
        if (r[0] != 0x21 || r[1] != 0x05 || r[2] != (uint8_t)(13 + i) || r[3] != ((i % 2) ? 0 : 1))
            kette = 0;
    }
    kette = kette && s_raw[0x012E6] == 0x04 && s_raw[0x012E7] == 0xFF &&
                     s_raw[0x012E8] == 0x18 && s_raw[0x012E9] == 0x12 &&
                     s_raw[0x012EA] == 0x22 && s_raw[0x012EB] == 0x04 &&
                     s_raw[0x012EC] == 0xEE && s_raw[0x012ED] == 0x01;
    CHECK("ROOM11F0 Loesungskette unveraendert (@0x012BE..0x012ED)", kette);

    /* B1: die Wertbildung. 5 EIN = 80 ist die vom Nutzer geforderte Zahl, und 5 EIN ist
     * genau das belegte Loesungsmuster. */
    CHECK("0 Schalter -> Wert 0",                re15_panel_zeiger_ziel_aus_bits(0) == 0);
    CHECK("5 Schalter (Loesung) -> Wert 80",     re15_panel_zeiger_ziel_aus_bits(5) == RE15_PANEL_ZIEL);
    CHECK("1 Schalter -> Wert 16",               re15_panel_zeiger_ziel_aus_bits(1) == 16);
    CHECK("6 Schalter -> 96 (noch unter dem Deckel)",
          re15_panel_zeiger_ziel_aus_bits(6) == 96);
    CHECK("7 Schalter (7*16 = 112) -> Deckel 100 (RE2 @0x011C8)",
          re15_panel_zeiger_ziel_aus_bits(7) == RE15_PANEL_MAX);
    CHECK("10 Schalter -> immer noch 100, nicht 160",
          re15_panel_zeiger_ziel_aus_bits(10) == RE15_PANEL_MAX);

    /* B2: die Fahrt — RE2 rueckt GENAU EINEN Punkt je Bild vor
     * (sub04+0x0106 @ROOM2130.RDT 0x01216 `02` evt_next in der Nachfuehrschleife). */
    re15_game_flag_set(5, 0, 1);
    for (int b = 13; b <= 22; b++) re15_game_flag_set(5, (uint8_t)b, 0);
    frame(0, 0);                                  /* Einschwingen: Wert = Ziel = 0 */
    CHECK("Startwert 0", re15_panel_zeiger_wert() == 0);

    re15_game_flag_set(5, 13, 1); re15_game_flag_set(5, 15, 1); re15_game_flag_set(5, 17, 1);
    re15_game_flag_set(5, 19, 1); re15_game_flag_set(5, 21, 1);
    int schritte = 0, vorher = re15_panel_zeiger_wert(), max_schritt = 0;
    for (int f = 0; f < 200 && re15_panel_zeiger_wert() != RE15_PANEL_ZIEL; f++) {
        frame(0, 0);
        int d = re15_panel_zeiger_wert() - vorher;
        if (d < 0) d = -d;
        if (d > max_schritt) max_schritt = d;
        vorher = re15_panel_zeiger_wert();
        schritte++;
    }
    printf("  Ziel=%d, erreicht nach %d Bildern, groesster Schritt je Bild %d\n",
           re15_panel_zeiger_ziel(), schritte, max_schritt);
    CHECK("Ziel ist 80",                 re15_panel_zeiger_ziel() == RE15_PANEL_ZIEL);
    CHECK("Zeiger steht am Ende auf 80", re15_panel_zeiger_wert() == RE15_PANEL_ZIEL);
    CHECK("nie mehr als EIN Punkt je Bild (RE2 @0x01216)", max_schritt == 1);
    CHECK("also genau 80 Bilder fuer 80 Punkte", schritte == RE15_PANEL_ZIEL);

    /* B3: die Bildschirmlage. Eichung aus den selbst vermessenen Teilstrichen des
     * Cut-10-Hintergrunds: y(0)=177 (unterster Strich), y(80)=79 (Fuss des roten Bandes,
     * Hoehe der roten 80-Ziffer y 74..83), y(100)=54 (Kopf des Bandes). */
    g_re15_active_cut = RE15_PANEL_CUT;
    int sx = 0, sy = 0;
    CHECK("Zeiger ist auf Cut 10 sichtbar", re15_panel_zeiger_sicht(&sx, &sy) == 1);
    printf("  bei Wert 80: Spitze (%d,%d)\n", sx, sy);
    CHECK("Spitze auf der Skalen-Saeule (x 281)", sx == RE15_PANEL_SPITZE_X);
    CHECK("Wert 80 -> y 79", sy == 79);

    /* Wert 0 und Wert 100 ueber die reine Eichung (dieselbe Formel, ohne 200 Bilder Fahrt). */
    CHECK("Eichung: Wert 0 -> y 177",
          RE15_PANEL_ANKER_Y - (0 * RE15_PANEL_SPANNE_Y + 50) / 100 == 177);
    CHECK("Eichung: Wert 100 -> y 54",
          RE15_PANEL_ANKER_Y - (100 * RE15_PANEL_SPANNE_Y + 50) / 100 == 54);

    /* B4: ausserhalb von Cut 10 wird nichts gezeichnet. */
    g_re15_active_cut = 8;
    CHECK("auf Cut 8 unsichtbar", re15_panel_zeiger_sicht(&sx, &sy) == 0);
    g_re15_active_cut = RE15_PANEL_CUT;
}

/* ================= TEIL C: Bestaetigung an der Loesungsflanke ==================== */
static void teil_c(void)
{
    printf("\n=== C: Bestaetigungs-SE (RE2 Gruppe 2 / 0x0C) an Set(4,238,1) ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) { printf("SKIP: RDT fehlt\n"); return; }
    g_re15_active_cut = RE15_PANEL_CUT;
    re15_game_flag_set(4, 238, 0);
    frame(0, 0);
    int pse0 = g_test_panel_se_count;
    unsigned b0 = g_re15_panel_bestaet_zaehler;
    re15_game_flag_set(4, 238, 1);
    frame(0, 0);
    int nach_flanke = g_test_panel_se_count - pse0;
    int id_flanke   = g_test_panel_se_last;
    for (int f = 0; f < 30; f++) frame(0, 0);       /* Flag bleibt 1 -> kein zweiter Ton */
    int nach_halten = g_test_panel_se_count - pse0;
    printf("  SE an der Flanke: %d (Id 0x%02X), nach 30 weiteren Bildern: %d, Zaehler %u\n",
           nach_flanke, id_flanke, nach_halten, g_re15_panel_bestaet_zaehler - b0);
    CHECK("genau EIN SE an der Loesungsflanke", nach_flanke == 1);
    CHECK("und zwar Index 0x0C", id_flanke == RE15_PANEL_SE_BESTAET);
    CHECK("kein Rattern, solange das Flag steht", nach_halten == 1);

    /* Und der Zeiger bleibt danach auf 80 stehen, obwohl sub18 Bank 5 abraeumt. */
    for (int b = 13; b <= 22; b++) re15_game_flag_set(5, (uint8_t)b, 0);
    for (int f = 0; f < 200; f++) frame(0, 0);
    printf("  nach dem Abraeumen von Bank 5: Ziel=%d Wert=%d\n",
           re15_panel_zeiger_ziel(), re15_panel_zeiger_wert());
    CHECK("geloest -> Zeiger haelt 80, auch ohne Schalterbits",
          re15_panel_zeiger_wert() == RE15_PANEL_ZIEL);
}

/* ================= TEIL D: die Ja/Nein-Auswahl toent ============================= */
/* Die Statemaschine liest den Pad aus zwei Globals (msg_common.c:441-442) und nur die
 * VIRTUELLEN Bits: 0x3000 = Links/Rechts (TOGGLE_MASK @msg_common.c:456),
 * 0x4000 = Bestaetigen (CONFIRM_BIT ebd.). */
extern uint16_t g_scd_pad_edge, g_scd_pad_held;

static void msg_select_an(int wahl)
{
    g_scd.message_active     = 1;
    g_scd.message_fsm_active = 1;
    g_scd.message_fsm        = 3;      /* SELECT */
    g_scd.message_select     = 1;
    g_scd.message_choice     = (uint8_t)wahl;
    g_scd.message_id         = 0;
}

static void msg_bild(uint16_t edge)
{
    const unsigned char *raw; int len, id;
    g_scd_pad_edge = edge; g_scd_pad_held = edge;
    re15_msg_tick(&raw, &len, &id);
    g_scd_pad_edge = 0; g_scd_pad_held = 0;
}

static void teil_d(void)
{
    printf("\n=== D: Ja/Nein-Auswahl (RE2 @0x80030968 / @0x80030944 / @0x8003093c) ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) { printf("SKIP: RDT fehlt\n"); return; }

    /* Cursor bewegen (virtuelles 0x1000/0x2000). */
    msg_select_an(0);
    int c0 = g_test_core_se_count;
    msg_bild(0x2000);
    int nach_bewegung = g_test_core_se_count - c0;
    int id_bewegung   = g_test_core_se_last;
    printf("  Bewegung: CORE-SE %d (Satz %d), Auswahl jetzt %d\n",
           nach_bewegung, id_bewegung, g_scd.message_choice);
    CHECK("Bewegen spielt einen Ton",    nach_bewegung == 1);
    CHECK("und zwar CORE-Satz 4",        id_bewegung == 4);
    CHECK("und schaltet die Auswahl um", g_scd.message_choice == 1);

    /* Bestaetigen mit Option 1 (Nein) -> CORE-Satz 5 (RE2 @0x8003093c lui a0,0x405). */
    msg_select_an(1);
    int c1 = g_test_core_se_count;
    msg_bild(0x4000);
    printf("  Bestaetigen (Option 1): CORE-SE %d (Satz %d)\n",
           g_test_core_se_count - c1, g_test_core_se_last);
    CHECK("Bestaetigen mit Option 1 spielt CORE-Satz 5",
          g_test_core_se_count - c1 == 1 && g_test_core_se_last == 5);

    /* Bestaetigen mit Option 0 (Ja) -> CORE-Satz 6 (RE2 @0x80030944/@0x80030950). */
    msg_select_an(0);
    int c2 = g_test_core_se_count;
    msg_bild(0x4000);
    printf("  Bestaetigen (Option 0): CORE-SE %d (Satz %d)\n",
           g_test_core_se_count - c2, g_test_core_se_last);
    CHECK("Bestaetigen mit Option 0 spielt CORE-Satz 6",
          g_test_core_se_count - c2 == 1 && g_test_core_se_last == 6);

    /* GEGENPROBE: ohne Pad-Flanke bleibt es still — sonst waere der Teil auch dann gruen,
     * wenn irgendetwas anderes im Tick CORE-SEs abfeuert. */
    msg_select_an(0);
    int c3 = g_test_core_se_count;
    msg_bild(0);
    CHECK("ohne Flanke kein Ton", g_test_core_se_count == c3);
}

/* ================= TEIL E: die Voraussetzungen der Abtastphasen-Korrektur ========= */
static void teil_e(void)
{
    printf("\n=== E: Voraussetzungen der Abtastphase (Cursor-TIM @0x018DAC) ===\n");
    /* Der Fix an der Abtastphase (render_pc.c, -0,5 Pixel; psx-spx
     * docs/graphicsprocessingunitgpu.md:303-305) haengt an zwei Eigenschaften der
     * AUSGELIEFERTEN Textur. Fallen die weg, ist die Begruendung hin.
     * ⛔ Dieser Teil ist eine VORAUSSETZUNGS-Wache, kein Pixel-Riegel: die Phase wirkt
     *    erst im SDL-Renderpfad. Der rot/gruen-Nachweis dafuer ist die A/B-Messung
     *    am echten Renderpfad (1 gegen 9 helle Zeilen), nicht dieser Test. */
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", RE15_ASSET_PSX_DIR);
    size_t n = 0; uint8_t *d = slurp(rp, &n);
    if (!d) { printf("SKIP: RDT fehlt\n"); return; }
    const size_t T = 0x018DAC;
    CHECK("TIM-Kopf am erwarteten Offset (magic 0x10, flags 0x09)",
          n > T + 0x20 && d[T] == 0x10 && d[T+4] == 0x09);
    /* CLUT-Block @T+8: len, x, y, w, h — dann die Paletteneintraege. */
    unsigned cw = (unsigned)(d[T+16] | (d[T+17] << 8));
    unsigned p2 = (unsigned)(d[T+20 + 2*2] | (d[T+20 + 2*2 + 1] << 8));
    unsigned p4 = (unsigned)(d[T+20 + 4*2] | (d[T+20 + 4*2 + 1] << 8));
    int r2 = (int)((p2 & 31) << 3), g2 = (int)(((p2 >> 5) & 31) << 3), b2 = (int)(((p2 >> 10) & 31) << 3);
    int r4 = (int)((p4 & 31) << 3), g4 = (int)(((p4 >> 5) & 31) << 3), b4 = (int)(((p4 >> 10) & 31) << 3);
    printf("  CLUT-Breite %u, Index2 = (%d,%d,%d), Index4 = (%d,%d,%d)\n",
           cw, r2, g2, b2, r4, g4, b4);
    CHECK("Index 4 ist das HELLE Gelb (216,208,0)", r4 == 216 && g4 == 208 && b4 == 0);
    CHECK("Index 2 ist die SCHATTENfarbe (64,56,0)", r2 == 64 && g2 == 56 && b2 == 0);

    /* Pixelblock: 8bpp, 128 breit. Der senkrechte Kreuzarm ist hell auf u 59..63 und sein
     * Schatten unmittelbar daneben auf u 64..68 — genau das macht ihn bei 5,25 Texel je
     * Bildschirmpixel phasenempfindlich. */
    size_t px = T + 8 + 12 + (size_t)cw * 2;      /* nach dem CLUT-Block */
    size_t img = px + 12;                          /* Bildblock-Kopf ueberspringen */
    int hell_breite = 0, schatten_breite = 0;
    const int zeile = 30;                          /* mitten im senkrechten Arm (v 21..60) */
    for (int u = 55; u <= 72; u++) {
        uint8_t v = d[img + (size_t)zeile * 128 + (size_t)u];
        if (v == 4) hell_breite++;
        if (v == 2) schatten_breite++;
    }
    printf("  Zeile v=%d: %d helle Texel (Index 4), %d Schatten-Texel (Index 2) in u 55..72\n",
           zeile, hell_breite, schatten_breite);
    CHECK("der helle Arm ist nur wenige Texel breit (<= 6)", hell_breite > 0 && hell_breite <= 6);
    CHECK("und der Schatten liegt gleich daneben",            schatten_breite > 0);
    free(d);
}

int main(void)
{
    printf("=== RIEGEL r26: ROOM11F0 Panel — Klick, Zeiger, Schatten ===\n");
    teil_a();
    teil_b();
    teil_c();
    teil_d();
    teil_e();
    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
