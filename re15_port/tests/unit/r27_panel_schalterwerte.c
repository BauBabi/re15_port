/*
 * r27_panel_schalterwerte.c — RIEGEL fuer die beiden Aenderungen an ROOM11F0 (Runde 27):
 *
 *   A  DIE SCHALTERWERTE (⛔ woertliche NUTZER-VORGABE 2026-09-26, KEIN Original):
 *        Schalter :  1    2    3    4    5    6    7    8    9   10
 *        Wert     : +20  -20  -10  -30  +20  -40  +20  -50  +30  -60
 *      Der Riegel zaehlt ALLE 1024 Kombinationen selbst durch: genau EINE erreicht die
 *      Anzeige 80, und das ist die Maske 1+3+5+7+9.
 *   B  ROHWERT vs. ANZEIGE sind getrennt: der Rohwert darf negativ werden
 *      (Minimum -210), nur die ANZEIGE wird bei 0 abgeschnitten; nach oben bleibt
 *      alles unter dem RE2-Deckel 100.
 *   C  Die LOESUNGSMENGE kommt aus den AUSGELIEFERTEN RDT-Bytes, nicht aus dem Kopf:
 *      ROOM11F0.RDT sub01 @Datei 0x012BE..0x012E2 (`21 05 0d 01` .. `21 05 16 00`)
 *      wird geparst und die daraus gewonnene Maske gegen die 80er-Maske gestellt.
 *   D  LIVE im Raum: Bits setzen -> der Zeiger faehrt auf 80; nur Schalter 10 ->
 *      Rohwert -60 und Anzeige 0.
 *   E  DER TON BEIM BEWEGEN IST WEG (Nutzer 2026-09-26: "wenn ich den cursor bewege
 *      kommt die ganze zeit sound. das will ich aber keinen sound"). Gegenprobe im
 *      selben Lauf: der Zellenwechsel-Zaehler MUSS steigen, sonst misst der Riegel
 *      einen stehenden Cursor und waere auch bei kaputtem Gitter gruen.
 *   F  UND ZWAR NUR DORT: der INVENTAR-Cursor toent weiter (CORE-Satz 4,
 *      RE1.5 @0x8004a478 `lui a0,0x404` / @0x8004a47c `jal 0x80045024`; die Tab-Stellen
 *      menu_common.c:193-196). Ein globaler Eingriff wuerde hier ROT.
 *   G  DER SCHALTER-TON KOMMT JE BETAETIGUNG, NICHT JE GEHALTENEM BILD: bei 90 Bildern
 *      gehaltenem QUADRAT nie zwei Toene in aufeinanderfolgenden Bildern, und die Zahl
 *      der Toene deckt sich mit der Zahl der echten Schalter-Betaetigungen.
 *
 * Belege mit Adresse: include/re15_panel_zeiger.h (Kopf) und
 * analysis/befunde_2026-09-26/re2-schalterraetsel-2130.md.
 *
 * RUECKBAU-NACHWEIS (in dieser Runde wirklich gefahren, Zahlen im Ergebnisfeld):
 *   A/B/C/D  die zehn Gewichte auf {16,16,...} zurueckgedreht  => A/C/D ROT
 *   E        re15_audio_core_se(4) in aot_common.c wieder rein => E ROT
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
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_fade.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;
extern int      g_test_core_se_last,  g_test_core_se_count;
extern int      g_test_panel_se_last, g_test_panel_se_count;
extern unsigned g_re15_cursor_klick_zaehler;   /* aot_common.c — ZELLENWECHSEL (ohne Ton) */
extern unsigned g_re15_panel_klick_zaehler;    /* scd_vm.c     — Schalter-Betaetigung     */

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

/* Das Raetsel scharf schalten wie sub16 @Datei 0x015C2 ff. (13x `22 05 xx 01`). */
static void raetsel_scharf(void)
{
    for (int b = 0; b <= 12; b++) re15_game_flag_set(5, (uint8_t)b, 1);
}

/* ============ A/B/C: die Wertetabelle, alle 1024 Kombinationen ==================== */
static unsigned s_rdt_loesung = 0;      /* aus den RDT-Bytes gewonnen */

static void teil_abc(void)
{
    printf("\n=== A/B/C: die zehn Nutzer-Gewichte gegen alle 1024 Kombinationen ===\n");

    /* --- C: die Loesungsmenge kommt aus den AUSGELIEFERTEN Bytes -------------------
     * ROOM11F0.RDT sub01 @Datei 0x012BE..0x012E2: zehn `21 05 <bit> <soll>` (Ck(Bank 5,
     * Bit 13+i) == soll). Genau die `soll == 1` sind die Schalter, die EIN sein muessen. */
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", RE15_ASSET_PSX_DIR);
    size_t n = 0; uint8_t *d = slurp(rp, &n);
    if (!d) { printf("SKIP: ROOM11F0.RDT fehlt\n"); g_fail++; return; }
    int kette_ok = (n > 0x012EE);
    unsigned rdt_maske = 0;
    for (int i = 0; i < 10 && kette_ok; i++) {
        const uint8_t *r = &d[0x012BE + i * 4];
        if (r[0] != 0x21 || r[1] != 0x05 || r[2] != (uint8_t)(13 + i) || r[3] > 1) kette_ok = 0;
        else if (r[3]) rdt_maske |= 1u << i;
    }
    /* danach `04 ff 18 12` (Evt_exec sub18) @0x012E6 und `22 04 ee 01` (Set(4,238,1)) */
    kette_ok = kette_ok && d[0x012E6] == 0x04 && d[0x012E7] == 0xFF &&
                           d[0x012E8] == 0x18 && d[0x012E9] == 0x12 &&
                           d[0x012EA] == 0x22 && d[0x012EB] == 0x04 &&
                           d[0x012EC] == 0xEE && d[0x012ED] == 0x01;
    free(d);
    s_rdt_loesung = rdt_maske;
    printf("  RE1.5-Loesungsmaske aus @0x012BE..0x012E2: 0x%03X (Schalter", rdt_maske);
    for (int i = 0; i < 10; i++) if (rdt_maske & (1u << i)) printf(" %d", i + 1);
    printf(")\n");
    CHECK("ROOM11F0 Loesungskette unveraendert (@0x012BE..0x012ED)", kette_ok);
    CHECK("RE1.5 verlangt genau die Schalter 1,3,5,7,9 (Maske 0x155)",
          rdt_maske == RE15_PANEL_LOESUNGSMASKE);

    /* --- die Gewichte selbst, gegen den Wortlaut der Nutzer-Vorgabe ---------------- */
    static const int soll[10] = { 20, -20, -10, -30, 20, -40, 20, -50, 30, -60 };
    int gew_ok = 1;
    for (int i = 0; i < 10; i++) if (re15_panel_zeiger_gewicht(i + 1) != soll[i]) gew_ok = 0;
    printf("  Gewichte:");
    for (int i = 0; i < 10; i++) printf(" %+d", re15_panel_zeiger_gewicht(i + 1));
    printf("\n");
    CHECK("die zehn Gewichte stehen woertlich so im Code (Nutzer-Vorgabe 2026-09-26)",
          gew_ok);
    CHECK("ausserhalb 1..10 gibt es kein Gewicht",
          re15_panel_zeiger_gewicht(0) == 0 && re15_panel_zeiger_gewicht(11) == 0);

    /* --- A: alle 1024 Kombinationen durchzaehlen ----------------------------------- */
    int treffer = 0; unsigned treffer_maske = 0;
    int roh_min = 1 << 30, roh_max = -(1 << 30);
    int anz_min = 1 << 30, anz_max = -(1 << 30);
    int anz_negativ = 0, roh_ungleich_summe = 0;
    for (unsigned m = 0; m < 1024u; m++) {
        int roh = re15_panel_zeiger_roh_aus_maske(m);
        int anz = re15_panel_zeiger_ziel_aus_maske(m);
        /* Gegenprobe: der Rohwert IST die Summe der Einzelgewichte (keine Klemmung). */
        int summe = 0;
        for (int i = 0; i < 10; i++) if (m & (1u << i)) summe += soll[i];
        if (roh != summe) roh_ungleich_summe++;
        if (roh < roh_min) roh_min = roh;
        if (roh > roh_max) roh_max = roh;
        if (anz < anz_min) anz_min = anz;
        if (anz > anz_max) anz_max = anz;
        if (anz < 0) anz_negativ++;
        if (anz == RE15_PANEL_ZIEL) { treffer++; treffer_maske = m; }
    }
    printf("  1024 Kombinationen: Roh %d..%d, Anzeige %d..%d, Treffer auf 80: %d "
           "(Maske 0x%03X)\n", roh_min, roh_max, anz_min, anz_max, treffer, treffer_maske);
    CHECK("der Rohwert ist ueberall die reine Summe (nirgends geklemmt)",
          roh_ungleich_summe == 0);
    CHECK("GENAU EINE Kombination erreicht die Anzeige 80", treffer == 1);
    CHECK("und das ist die Maske 1+3+5+7+9", treffer_maske == RE15_PANEL_LOESUNGSMASKE);
    CHECK("sie ist dieselbe, die RE1.5 @0x012BE..0x012E2 als Loesung prueft",
          treffer_maske == s_rdt_loesung);

    /* --- B: Rohwert und Anzeige sind getrennt -------------------------------------- */
    CHECK("der Rohwert wird negativ (Minimum -210 = nur die sechs Minus-Schalter)",
          roh_min == -210);
    CHECK("der groesste Rohwert ist 90 (die vier Plus-Schalter)", roh_max == 90);
    CHECK("die ANZEIGE wird nie negativ (max(0,Roh))", anz_negativ == 0 && anz_min == 0);
    CHECK("kein erreichbarer Anzeigewert liegt ueber 100 (RE2-Deckel @0x011C8)",
          anz_max <= RE15_PANEL_MAX);
    CHECK("Rohwert und Anzeige unterscheiden sich wirklich irgendwo "
          "(sonst prueft die Trennung nichts)",
          re15_panel_zeiger_roh_aus_maske(1u << 9) == -60 &&
          re15_panel_zeiger_ziel_aus_maske(1u << 9) == 0);

    /* --- die Loesungs-KETTE: kein Zwischenwert negativ ------------------------------
     * Reihenfolge 1,3,5,7,9 -> 0 -> 20 -> 10 -> 30 -> 50 -> 80 (Nutzer-Wortlaut). */
    static const int kette_soll[6] = { 0, 20, 10, 30, 50, 80 };
    unsigned m = 0; int kette_gleich = 1, kette_negativ = 0;
    for (int k = 0; k < 6; k++) {
        int roh = re15_panel_zeiger_roh_aus_maske(m);
        if (roh != kette_soll[k]) kette_gleich = 0;
        if (roh < 0) kette_negativ++;
        if (k < 5) m |= 1u << (k * 2);           /* Schalter 1,3,5,7,9 nacheinander EIN */
    }
    printf("  Kette 1->3->5->7->9:");
    m = 0;
    for (int k = 0; k < 6; k++) {
        printf(" %d", re15_panel_zeiger_roh_aus_maske(m));
        if (k < 5) m |= 1u << (k * 2);
    }
    printf("\n");
    CHECK("die Loesungs-Kette ist 0,20,10,30,50,80 (Nutzer-Wortlaut)", kette_gleich);
    CHECK("und kein Zwischenwert der Kette ist negativ", kette_negativ == 0);
}

/* ============ D: live im Raum ===================================================== */
static void teil_d(void)
{
    printf("\n=== D: live in ROOM11F0 — der Zeiger folgt den Bits ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) { printf("SKIP: RDT fehlt\n"); g_fail++; return; }
    g_re15_active_cut = RE15_PANEL_CUT;
    re15_game_flag_set(4, 238, 0);
    re15_game_flag_set(5, 0, 1);
    for (int b = 13; b <= 22; b++) re15_game_flag_set(5, (uint8_t)b, 0);
    frame(0, 0);
    CHECK("Startwert 0", re15_panel_zeiger_wert() == 0 && re15_panel_zeiger_roh() == 0);

    /* nur Schalter 10 (Bank 5 Bit 22) -> Roh -60, Anzeige 0 */
    re15_game_flag_set(5, 22, 1);
    frame(0, 0);
    printf("  nur Schalter 10: roh=%d ziel=%d\n",
           re15_panel_zeiger_roh(), re15_panel_zeiger_ziel());
    CHECK("nur Schalter 10 -> Rohwert -60", re15_panel_zeiger_roh() == -60);
    CHECK("nur Schalter 10 -> Anzeige/Ziel 0", re15_panel_zeiger_ziel() == 0);
    re15_game_flag_set(5, 22, 0);

    /* EIN SCHALTER KANN JEDERZEIT WIEDER AUS (Nutzer-Wortlaut). Das wird an einer
     * NICHT-Loesung gemessen, denn sobald das Raetsel faellt, steht das Ziel per Flag
     * 4/238 fest auf 80 (ROOM11F0.RDT @0x012EA `22 04 ee 01`, s. re15_panel_zeiger_tick).
     * Schalter 1 (+20) und 5 (+20) sind zusammen 40 und keine Loesung. */
    re15_game_flag_set(5, 13, 1); re15_game_flag_set(5, 17, 1);
    frame(0, 0);
    printf("  Schalter 1+5 EIN: roh=%d ziel=%d (geloest=%d)\n",
           re15_panel_zeiger_roh(), re15_panel_zeiger_ziel(), re15_game_flag_get(4, 238));
    CHECK("Schalter 1+5 -> 40", re15_panel_zeiger_roh() == 40 &&
                                re15_panel_zeiger_ziel() == 40);
    re15_game_flag_set(5, 13, 0);                /* Schalter 1 wieder AUS */
    frame(0, 0);
    printf("  Schalter 1 wieder AUS: roh=%d ziel=%d\n",
           re15_panel_zeiger_roh(), re15_panel_zeiger_ziel());
    CHECK("Schalter 1 aus -> wieder 20, der ZUSTAND entscheidet (kein Weg-Zaehler)",
          re15_panel_zeiger_roh() == 20 && re15_panel_zeiger_ziel() == 20);
    re15_game_flag_set(5, 17, 0);

    /* die Loesungsmenge aus den RDT-Bytes (Bits 13+i) setzen */
    for (int i = 0; i < 10; i++)
        re15_game_flag_set(5, (uint8_t)(13 + i), (s_rdt_loesung & (1u << i)) ? 1 : 0);
    for (int f = 0; f < 200 && re15_panel_zeiger_wert() != RE15_PANEL_ZIEL; f++) frame(0, 0);
    printf("  Loesungsbits gesetzt: roh=%d ziel=%d wert=%d\n",
           re15_panel_zeiger_roh(), re15_panel_zeiger_ziel(), re15_panel_zeiger_wert());
    CHECK("Rohwert 80",  re15_panel_zeiger_roh()  == RE15_PANEL_ZIEL);
    CHECK("Ziel 80",     re15_panel_zeiger_ziel() == RE15_PANEL_ZIEL);
    CHECK("Zeiger steht auf 80", re15_panel_zeiger_wert() == RE15_PANEL_ZIEL);
}

/* ============ E: der Bewegungs-Ton ist weg (mit Gegenprobe) ======================= */
static void teil_e(void)
{
    printf("\n=== E: Cursor bewegen ist STILL (Nutzer 2026-09-26) ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) { printf("SKIP: RDT fehlt\n"); g_fail++; return; }

    /* Skript-Anker der Bewegung: sub01 @0x01098 Sce_key_ck(1,0x01) -> Evt_exec sub02
     * @0x0109C, und sub02 @0x012F6 Speed_set(Achse 2, +200). Faellt der weg, bewegt sich
     * der Cursor nicht mehr und der Riegel wuerde einen stehenden Cursor "still" nennen. */
    int anker = (s_raw[0x1098] == 0x51 && s_raw[0x1099] == 0x01 && s_raw[0x109A] == 0x01 &&
                 s_raw[0x109C] == 0x04 && s_raw[0x109D] == 0xFF && s_raw[0x109E] == 0x18 &&
                 s_raw[0x109F] == 0x02 &&
                 s_raw[0x10B0] == 0x51 && s_raw[0x10B1] == 0x01 && s_raw[0x10B2] == 0x04);
    CHECK("ROOM11F0 sub01 Bewegungs-Block unveraendert (@0x1098/@0x109C/@0x10B0)", anker);

    raetsel_scharf();
    for (int f = 0; f < 5; f++) frame(0, 0);

    unsigned z0 = g_re15_cursor_klick_zaehler;
    int c0 = g_test_core_se_count, p0 = g_test_panel_se_count;
    /* Den Cursor ueber das Brett fahren: 6 x 40 Bilder abwechselnd HOCH/RUNTER
     * (sub02/sub03, Speed_set Achse 2 = Z; das Brett hat 5 Zeilen im Abstand 2200 und
     * der Cursor laeuft 200 je Bild) und zwei Laeufe LINKS/RECHTS. */
    for (int runde = 0; runde < 6; runde++) {
        uint16_t b = (runde & 1) ? RE15_PAD_BIT_DOWN : RE15_PAD_BIT_UP;
        for (int f = 0; f < 40; f++) frame(b, (uint16_t)(f == 0 ? b : 0));
    }
    for (int runde = 0; runde < 2; runde++) {
        uint16_t b = (runde & 1) ? RE15_PAD_BIT_LEFT : RE15_PAD_BIT_RIGHT;
        for (int f = 0; f < 40; f++) frame(b, (uint16_t)(f == 0 ? b : 0));
    }
    unsigned zellen = g_re15_cursor_klick_zaehler - z0;
    int core = g_test_core_se_count - c0, panel = g_test_panel_se_count - p0;
    printf("  320 Bilder Cursor fahren: %u Zellenwechsel, CORE-SE %d, Panel-SE %d\n",
           zellen, core, panel);
    CHECK("GEGENPROBE: der Cursor ist wirklich ueber Zellen gefahren", zellen > 0);
    CHECK("dabei kein einziger CORE-SE (der Bewegungs-Ton ist weg)", core == 0);
    CHECK("und auch kein Panel-SE (ohne Tastendruck kein Schalter-Ton)", panel == 0);
}

/* ============ F: der Inventar-Cursor toent weiter ================================= */
static void inv_frame(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen()) re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
    re15_fade_tick();
}

static void teil_f(void)
{
    printf("\n=== F: der INVENTAR-Cursor toent weiter (kein globaler Eingriff) ===\n");
    /* RE1.5 spielt den Cursor-Ton aus SE-Bank 4, Satz 4: @0x8004a478 `lui a0,0x404`
     * / @0x8004a47c `jal 0x80045024`. Die Tab-Stellen sind menu_common.c:193-196. */
    re15_inv_init();
    re15_menu_toggle();
    for (int f = 0; f < 12; f++) inv_frame(0, 0);
    printf("  Menue offen=%d, Tab=%u\n", re15_menu_is_open(), (unsigned)g_inv_screen.tab);

    int c0 = g_test_core_se_count;
    uint8_t tab_vor = g_inv_screen.tab;
    inv_frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);
    int nach_rechts = g_test_core_se_count - c0;
    int id_rechts   = g_test_core_se_last;
    uint8_t tab_nach = g_inv_screen.tab;
    printf("  Tab RECHTS: CORE-SE %d (Satz %d), Tab %u -> %u\n",
           nach_rechts, id_rechts, (unsigned)tab_vor, (unsigned)tab_nach);
    CHECK("GEGENPROBE: der Inventar-Cursor hat sich wirklich bewegt", tab_nach != tab_vor);
    CHECK("der Inventar-Cursor spielt weiterhin einen Ton", nach_rechts == 1);
    CHECK("und zwar CORE-Satz 4 (@0x8004a478)", id_rechts == 4);

    int c1 = g_test_core_se_count;
    inv_frame(RE15_PAD_BIT_LEFT, RE15_PAD_BIT_LEFT);
    printf("  Tab LINKS: CORE-SE %d (Satz %d)\n",
           g_test_core_se_count - c1, g_test_core_se_last);
    CHECK("auch zurueck toent es (CORE-Satz 4, @0x8004986c-80)",
          g_test_core_se_count - c1 == 1 && g_test_core_se_last == 4);

    int c2 = g_test_core_se_count;
    inv_frame(0, 0);
    CHECK("ohne Tastendruck bleibt das Inventar still", g_test_core_se_count == c2);
    re15_menu_toggle();
    for (int f = 0; f < 12; f++) inv_frame(0, 0);
}

/* ============ G: der Schalter-Ton kommt je Betaetigung ============================ */
static void teil_g(void)
{
    printf("\n=== G: der Schalter-Ton je BETAETIGUNG, nicht je gehaltenem Bild ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) { printf("SKIP: RDT fehlt\n"); g_fail++; return; }
    raetsel_scharf();

    int p0 = g_test_panel_se_count, c0 = g_test_core_se_count;
    unsigned k0 = g_re15_panel_klick_zaehler;
    /* Die zehn Schalter-ZUSTAENDE sind Bank 5 / Bits 13..22 (@0x012BE ff.). Jede
     * Aenderung eines dieser Bits ist genau EINE echte Betaetigung — anders als die
     * Zellen-Sperrbits 1..11, die je Betaetigung ZWEIMAL wechseln (Set(5,N,0) @0x01322
     * und Set(5,N,1) @0x0135C). */
    int zustand[10];
    for (int i = 0; i < 10; i++) zustand[i] = re15_game_flag_get(5, (uint8_t)(13 + i));
    int betaetigt = 0, letztes = -99, engster = 999;
    const int BILDER = 90;
    /* 30 NACHLAUF-Bilder ohne Taste: ein Umlegen dreht den Hebel erst ueber 16 Bilder
     * (sub06 @0x01336 `0d 00 04 00 10 00` For 0x10) und setzt das Zustandsbit erst danach
     * (@0x01340 `22 05 0d 01`). Ohne Nachlauf zaehlte der letzte Ton ohne sein Bit. */
    for (int f = 0; f < BILDER + 30; f++) {
        int vor = g_test_panel_se_count;
        uint16_t taste = (f < BILDER) ? RE15_PAD_BIT_SQUARE : 0;
        frame(taste, (uint16_t)(f == 0 ? RE15_PAD_BIT_SQUARE : 0));
        if (g_test_panel_se_count != vor) {
            if (f - letztes < engster) engster = f - letztes;
            letztes = f;
        }
        for (int i = 0; i < 10; i++) {
            int j = re15_game_flag_get(5, (uint8_t)(13 + i));
            if (j != zustand[i]) { betaetigt++; zustand[i] = j; }
        }
    }
    int panel = g_test_panel_se_count - p0;
    printf("  %d Bilder GEHALTENES Quadrat: Panel-SE %d (Id 0x%02X, engster Abstand %d "
           "Bilder), Schalter-Betaetigungen %d, CORE-SE %d\n",
           BILDER, panel, g_test_panel_se_last, engster, betaetigt,
           g_test_core_se_count - c0);
    CHECK("GEGENPROBE: es wurden wirklich Schalter betaetigt", betaetigt > 0);
    CHECK("der Ton kommt (RE2 Gruppe 2 / Index 0x0A)",
          panel > 0 && g_test_panel_se_last == RE15_PANEL_SE_KLICK);
    CHECK("der Aufrufzaehler deckt sich mit den Toenen",
          g_re15_panel_klick_zaehler - k0 == (unsigned)panel);
    CHECK("NICHT je Bild: Toene weit unter der Bildzahl", panel * 4 < BILDER);
    CHECK("nie zwei Toene in aufeinanderfolgenden Bildern", engster > 1);
    CHECK("genau ein Ton je Betaetigung", panel == betaetigt);
    CHECK("und kein CORE-SE dabei (der Schalter-Ton ist ein RAUM-SE)",
          g_test_core_se_count - c0 == 0);
}

int main(void)
{
    printf("=== RIEGEL r27: ROOM11F0 — Schalterwerte (Nutzer-Vorgabe) + stiller Cursor ===\n");
    teil_abc();
    teil_d();
    teil_e();
    teil_f();
    teil_g();
    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
