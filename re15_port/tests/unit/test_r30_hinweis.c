/* test_r30_hinweis.c — RIEGEL Runde 30, Thema B: der Kartenhinweis nach der Irons-Szene.
 *
 * ⛔ RE2-ERGAENZUNG (kein RE1.5-Original): RE1.5 hat keinen Kartenhinweis; Vorbild ist RE2s
 * Opcode 0x84 / Statusschirm-Modus 4. Belege: include/re15_map_hint.h,
 * engine/src/map_hint_common.c, analysis/befunde_runde30/karte-3010.md (Abschnitt 5.3).
 *
 * EIN Programm, fuenf Riegel (argv[1]):
 *   anker     ROOM1150.RDT: Signatur 1 Treffer @0x012E0, Anker = Evt_end @0x012EC; sub08
 *             bis zum Ende ticken -> die Anforderung steht GENAU im Tick des Thread-Endes,
 *             vorher nie. ROOM1151 / ROOM1050 (traegt dieselbe Folge!) / ROOM1100: 0 Anker.
 *   fsm       Anforderung -> Stufen 1/2 -> Schirm mit substate 1, item_state 1, Zielblatt;
 *             CORE-Spion: KEIN Satz 6, Satz 9 genau einmal (nach der Einblendung), Satz 5
 *             beim Schliessen; Hinweis-Ton im Schritt 2 und dann alle 78 Schritte (= je
 *             VBlank einer, Wanduhr); rot/Umriss-Phasen je 39 Schritte = 0,652 s bei 30, 60
 *             und 144 Bildern je Sekunde; Bestaetigen, HOCH/RUNTER, L1 aendern nichts;
 *             START und Abbruch schliessen.
 *   spurlos   vor/nach dem ganzen Hinweis bitgleich: Besucht-Bits, Etagen-Bits, Spiel-Flags,
 *             Besitz-Bits, persistente Kartenregister 260d/260e; danach zeigt der normale
 *             Kartenaufruf Blatt 4 (Irons' Buero), Blatt 3 Rechteck 9 bleibt UNVISITED.
 *   zeichner  Op-Liste im Hinweis: Zielkachel (156,76) 48x40 uv (208,80) vorhanden, obwohl
 *             unbesucht und Blatt nicht im Besitz; CLUT folgt hint_rot (502/498); kein
 *             Spielermarker (uv 224,128); keine AKTUELL-Kachel ausser dem Ziel in der roten
 *             Phase; Blatt 3 / Rechteck 9 festgehalten; ohne Hinweis Op-Liste unveraendert.
 *   bank      HINTSE.VBS 8280 B; Welle bitgleich zu RE2 ROOM3010.RDT VAG 18 @0x39788
 *             (4480 B); Durchlauf wie Sonde 3: Satz 0x2B -> Programm 1, Ton 2, Pitch 0x0400.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_menu.h"
#include "re15_fade.h"
#include "re15_inventory.h"
#include "re15_inv_screen.h"
#include "re15_map_owned.h"
#include "re15_map_hint.h"
#include "re15_collision.h"
#include "re15_vab.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif
#ifndef RE15_RE2_RDT_DIR
#define RE15_RE2_RDT_DIR "../info/re2leon/PL0/RDT"
#endif

extern scd_vm_t g_scd;
extern int scd_thread_start(int slot, const uint8_t *pc);
extern int g_test_core_se_last, g_test_core_se_count;
extern int g_test_hint_se_last, g_test_hint_se_count;

static int g_fail = 0;
#define CHECK(c, ...) do { if (c) { printf("  PASS: "); printf(__VA_ARGS__); printf("\n"); } \
                           else { printf("  FAIL: "); printf(__VA_ARGS__); printf("\n"); g_fail = 1; } } while (0)

#define SUB08_BEG 0x1106u   /* ROOM1150.RDT sub08 (Sub-Tabelle @0x0EA0)          */
#define SIG_OFF   0x12E0u   /* 14-Byte-Schwanz                                    */
#define EVT_END   0x12ECu   /* 01 00 Evt_end von sub08 = Anker                    */

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return NULL; }
    fclose(f);
    *out_size = (size_t)sz;
    return buf;
}

static uint8_t *load_room(unsigned room, size_t *sz, re15_rdt_t *rdt)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (room >> 12) & 0xF, room);
    uint8_t *raw = read_file(p, sz);
    if (!raw) { printf("  SKIP: %s fehlt\n", p); return NULL; }
    if (re15_rdt_parse(raw, *sz, rdt) < 0) { printf("  FAIL: RDT-Parse %s\n", p); g_fail = 1; free(raw); return NULL; }
    return raw;
}

/* Spieler in ROOM1150 an den Debug-JUMP-Spawn (DEBUG.BIN @0x0285C: X=-17370 Z=-11900) */
static void spieler_1150(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_current_room_id = 0x1150;
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -17370; pl->y = 0; pl->z = -11900; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_map_zone_update(0x1150, pl->x, pl->z);
}

/* ============================================================================================ */
static int riegel_anker(void)
{
    static re15_rdt_t rdt;
    size_t sz = 0;
    uint8_t *raw = load_room(0x1150, &sz, &rdt);
    if (!raw) return 77;
    static const uint8_t sig[14] = { 0x42, 0x00, 0x3c, 0x01, 0x22, 0x02, 0x07, 0x00,
                                     0x22, 0x01, 0x1b, 0x00, 0x01, 0x00 };
    int n = 0; size_t wo = 0;
    for (size_t o = 0; o + sizeof sig <= sz; o++)
        if (memcmp(raw + o, sig, sizeof sig) == 0) { n++; wo = o; }
    CHECK(n == 1 && wo == SIG_OFF, "ROOM1150.RDT: Signatur %d Treffer @0x%05lX (Soll 1 @0x012E0)", n, (unsigned long)wo);

    re15_actor_init(); scd_vm_init(); re15_aot_init();
    g_room_change.pending = 0;
    spieler_1150();
    scd_register_room_events(&rdt);
    CHECK(g_re15_map_hint_anchor_n == 1 && re15_map_hint_anchor_off() == (long)EVT_END,
          "Anker in ROOM1150: n=%d @0x%04lX (Soll 1 @0x012EC = Evt_end sub08)",
          g_re15_map_hint_anchor_n, re15_map_hint_anchor_off());
    CHECK(raw[EVT_END] == 0x01 && raw[EVT_END + 1] == 0x00, "Anker-Bytes 01 00 (Evt_end)");
    CHECK((long)(rdt.sub_scd[8] - raw) == (long)SUB08_BEG, "sub08 beginnt @0x%04lX (Soll 0x1106)",
          (long)(rdt.sub_scd[8] - raw));

    scd_room_reenter(&rdt, g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);
    for (int f = 0; f < 30; f++) { scd_vm_tick(); re15_actors_anim_advance(); re15_actor_step_all_walkers(); }
    CHECK(re15_map_hint_pending() < 0, "vor der Szene keine Anforderung (%d)", re15_map_hint_pending());

    const int slot = 5;
    int rc = scd_thread_start(slot, rdt.sub_scd[8]);
    CHECK(rc == 0, "sub08 in Slot %d gestartet (rc=%d)", slot, rc);
    int ende = -1, erste_anf = -1, f;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (f = 0; f < 6000; f++) {
        scd_vm_tick();
        re15_actors_anim_advance();
        re15_actor_step_all_walkers();
        re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id);
        if (erste_anf < 0 && re15_map_hint_pending() >= 0) erste_anf = f;
        if (ende < 0 && !g_scd.threads[slot].active) ende = f;
        if (ende >= 0) break;
    }
    CHECK(ende > 0, "sub08 endet in Bild %d (Sonde vorher: 1564)", ende);
    CHECK(erste_anf == ende, "Anforderung im Tick des Thread-Endes: Anforderung Bild %d, Ende Bild %d", erste_anf, ende);
    CHECK(re15_map_hint_pending() == 0, "Hinweis-Nummer %d (Soll 0 = einziger Tabelleneintrag)", re15_map_hint_pending());
    CHECK(re15_game_flag_get(3, 94) == 1, "Szene gelaufen: flag(3,94)=%d (sub08 @0x01110)", re15_game_flag_get(3, 94));
    free(raw);

    /* Andere Raeume: kein Anker. ROOM1050 traegt DIESELBE 14-Byte-Folge (@0x0DF2). */
    static const unsigned andere[3] = { 0x1151, 0x1050, 0x1100 };
    for (int k = 0; k < 3; k++) {
        static re15_rdt_t r2;
        size_t s2 = 0;
        uint8_t *b2 = load_room(andere[k], &s2, &r2);
        if (!b2) continue;
        int t = 0;
        for (size_t o = 0; o + sizeof sig <= s2; o++) if (memcmp(b2 + o, sig, sizeof sig) == 0) t++;
        g_current_room_id = andere[k];
        scd_register_room_events(&r2);
        CHECK(g_re15_map_hint_anchor_n == 0 && re15_map_hint_pending() < 0,
              "ROOM%04X: %d Signatur-Treffer in der Datei, Anker %d, Anforderung %d (Soll 0 / keine)",
              andere[k], t, g_re15_map_hint_anchor_n, re15_map_hint_pending());
        scd_register_room_events(NULL);
        free(b2);
    }
    return g_fail;
}

/* ============================================================================================ */
/* Bild-Modell wie test_inv_fsm.c: Spiel-Haken (game_step_common.c) -> START-Poll -> FSM-Tick,
 * dann die renderseitigen Fortschritte (ECG-Kopf, Blende). Die Wanduhr laeuft je Bild um
 * g_dt_us — damit ist die Bildrate der Anzeige frei waehlbar. */
static uint64_t g_t_us = 5000000ull;
static uint64_t g_dt_us = 33333ull;

/* Mitschrift ab dem Bild, in dem der Hinweis beginnt (Wanduhr relativ zu diesem Bild). */
static int      g_rec_on;
static uint64_t g_t0;
static uint64_t g_ton_t[32];  static int g_ton_n;
static uint64_t g_flip_t[64]; static int g_flip_rot[64]; static int g_flip_n; static int g_rot_vor;
static int      g_se6, g_se9, g_se9_phase;
static int      g_core_oeffnen;   /* ALLE CORE-Toene vom Anfordern bis Phase 1 */

static void rec_reset(void)
{
    g_rec_on = 0; g_ton_n = 0; g_flip_n = 0; g_se6 = 0; g_se9 = 0; g_se9_phase = -1;
}

static void bild(uint16_t pressed)
{
    const uint64_t t = g_t_us;
    const int h0 = g_test_hint_se_count, c0 = g_test_core_se_count;
    re15_host_clock_set_us(t);
    /* der Haken aus re15_game_step (game_step_common.c), wortgleich */
    if (re15_map_hint_pending() >= 0 && re15_menu_request_map_hint(re15_map_hint_pending()))
        re15_map_hint_take();
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen()) re15_menu_fsm_tick(pressed, pressed);
    if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
    re15_fade_tick();
    if (!g_rec_on && re15_menu_is_open() && g_inv_screen.hint_aktiv) {
        g_rec_on = 1; g_t0 = t; g_rot_vor = g_inv_screen.hint_rot;
    }
    if (g_rec_on) {
        if (g_test_hint_se_count != h0 && g_ton_n < 32) g_ton_t[g_ton_n++] = t - g_t0;
        if (g_inv_screen.hint_aktiv && g_inv_screen.hint_rot != g_rot_vor && g_flip_n < 64) {
            g_flip_t[g_flip_n] = t - g_t0; g_flip_rot[g_flip_n++] = g_inv_screen.hint_rot;
            g_rot_vor = g_inv_screen.hint_rot;
        }
    }
    if (g_test_core_se_count != c0) {
        if (g_test_core_se_last == 6) g_se6++;
        if (g_test_core_se_last == 9) { g_se9++; g_se9_phase = re15_menu_phase(); }
    }
    g_t_us += g_dt_us;
}

/* Anforderung wie aus dem Skript: Anker in ROOM1150 registrieren und den Programmzeiger
 * auf das Evt_end stellen. */
static re15_rdt_t s_rdt1150;
static uint8_t   *s_raw1150;
static int anfordern(void)
{
    if (!s_raw1150) {
        size_t sz = 0;
        s_raw1150 = load_room(0x1150, &sz, &s_rdt1150);
        if (!s_raw1150) return 0;
    }
    g_current_room_id = 0x1150;
    re15_map_hint_room_scan(s_raw1150, (int)s_rdt1150.raw_size, 0x1150);
    re15_map_hint_pc(s_raw1150 + EVT_END);
    return re15_map_hint_pending() == 0;
}

static void grundstellung(void)
{
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    re15_inv_init();
    re15_fade_init();
    spieler_1150();
}

/* Oeffnen bis zur laufenden Phase 1; liefert die Zahl der Bilder (-1 = nie offen). */
static int oeffnen(void)
{
    int f, c0 = g_test_core_se_count;
    rec_reset();
    for (f = 0; f < 200; f++) {
        bild(0);
        if (re15_menu_is_open() && re15_menu_phase() == 1) {
            g_core_oeffnen = g_test_core_se_count - c0;
            return f;
        }
    }
    return -1;
}

static int riegel_fsm(void)
{
    grundstellung();
    CHECK(re15_map_hint_periode() == 78, "Periode der Zaehlfolge nachgespielt: %d Schritte (Dossier 3.5 a: 78)",
          re15_map_hint_periode());
    if (!anfordern()) return 77;
    int f_offen = oeffnen();
    CHECK(f_offen >= 0 && g_rec_on, "Hinweis-Schirm lebt, Phase 1 nach %d Bildern", f_offen);
    CHECK(re15_map_hint_pending() < 0, "Anforderung verbraucht");
    CHECK(re15_menu_is_open() && re15_menu_substate() == 1 && g_inv_screen.item_state == 1,
          "offen=%d substate=%d item_state=%d (Soll 1/1/1)", re15_menu_is_open(),
          re15_menu_substate(), g_inv_screen.item_state);
    {
        int pg = -1, rc = -1;
        re15_map_hint_ziel(0, &pg, &rc);
        CHECK(g_inv_screen.map_page == pg && g_inv_screen.hint_page == pg && g_inv_screen.hint_rect == rc,
              "Blatt %d / Ziel %d/%d (Zonen-Tabelle %d/%d)", g_inv_screen.map_page,
              g_inv_screen.hint_page, g_inv_screen.hint_rect, pg, rc);
    }
    CHECK(g_se6 == 0, "kein Oeffnen-Ton Se(4,6) (%d), RE2 @0x80026404 ueberspringt @0x8002652C", g_se6);
    CHECK(g_se9 == 1 && g_se9_phase == 1, "Se(4,9) genau einmal (%d), im Bild des Uebergangs in Phase 1 (%d)",
          g_se9, g_se9_phase);
    CHECK(g_core_oeffnen == 1 && g_test_core_se_last == 9,
          "beim Oeffnen genau EIN CORE-Ton, und das ist Satz 9 (%d Toene, zuletzt %d)",
          g_core_oeffnen, g_test_core_se_last);

    /* Blinker laufen lassen, bis 4 s seit dem Beginn des Hinweises vergangen sind. */
    int core_lauf0 = g_test_core_se_count;
    while (g_t_us - g_t0 < 4000000ull) bild(0);
    CHECK(g_test_hint_se_last == 0x2B, "Hinweis-Ton = Satz 0x%02X (Soll 0x2B, @0x8006F234)", g_test_hint_se_last);
    /* Soll: Ton im Schritt 2 + 78k, Schritt s faellt bei s / 59,826 s. Das Bild, in dem er
     * faellt, ist das erste mit Wanduhr >= diesem Zeitpunkt (Bildabstand g_dt_us). */
    int ton_ok = 1;
    for (int k = 0; k < g_ton_n; k++) {
        uint64_t soll = ((uint64_t)(2 + 78 * k) * 1000000000ull + 59825) / 59826;
        if (!(g_ton_t[k] >= soll && g_ton_t[k] < soll + g_dt_us)) ton_ok = 0;
        printf("     Ton %d: t=%llu us (Soll ab %llu us)\n", k, (unsigned long long)g_ton_t[k],
               (unsigned long long)soll);
    }
    CHECK(g_ton_n == 4 && ton_ok, "%d Toene in 4 s, je Schritt 2+78k (1,304 s Abstand) auf ein Bild genau", g_ton_n);
    int flip_ok = 1;
    for (int k = 0; k < g_flip_n; k++) {
        uint64_t s = 2 + 39 * (uint64_t)k;           /* 2 (rot), 41 (umriss), 80 (rot) ... */
        uint64_t soll = (s * 1000000000ull + 59825) / 59826;
        int soll_rot = (k % 2) == 0;
        if (!(g_flip_t[k] >= soll && g_flip_t[k] < soll + g_dt_us) || g_flip_rot[k] != soll_rot) flip_ok = 0;
        if (k < 4)
            printf("     Wechsel %d: t=%llu us -> %s (Soll ab %llu us)\n", k, (unsigned long long)g_flip_t[k],
                   g_flip_rot[k] ? "rot" : "umriss", (unsigned long long)soll);
    }
    CHECK(g_flip_n == 7 && flip_ok, "%d Phasenwechsel in 4 s, rot ab Schritt 2, je 39 Schritte (0,652 s) auf ein Bild genau",
          g_flip_n);
    CHECK(g_test_core_se_count == core_lauf0, "im Lauf keine CORE-Toene (%d)", g_test_core_se_count - core_lauf0);

    /* Eingaben, die NICHTS tun duerfen */
    {
        int pg = g_inv_screen.map_page, c0 = g_test_core_se_count;
        bild(RE15_PAD_BIT_SQUARE);  bild(0);
        bild(RE15_PAD_BIT_UP);      bild(0);
        bild(RE15_PAD_BIT_DOWN);    bild(0);
        bild(RE15_PAD_BIT_L1);      bild(0);
        bild(RE15_PAD_BIT_R1);      bild(0);
        CHECK(re15_menu_is_open() && re15_menu_phase() == 1 && re15_menu_substate() == 1 &&
              g_inv_screen.item_state == 1 && g_inv_screen.map_page == pg && g_inv_screen.hint_aktiv,
              "Bestaetigen/HOCH/RUNTER/L1/R1: Schirm unveraendert (Phase %d, Blatt %d, item_state %d)",
              re15_menu_phase(), g_inv_screen.map_page, g_inv_screen.item_state);
        CHECK(g_test_core_se_count == c0, "dabei kein Ton (%d)", g_test_core_se_count - c0);
    }

    /* START schliesst: Se(4,5), Ausblenden, Spiel laeuft */
    {
        int c0 = g_test_core_se_count, f;
        bild(RE15_PAD_BIT_START);
        CHECK(g_test_core_se_count == c0 + 1 && g_test_core_se_last == 5, "START: Se(4,5) (@0x8006F890)");
        for (f = 0; f < 200 && (re15_menu_is_open() || re15_menu_stage() != 0); f++) bild(0);
        CHECK(!re15_menu_is_open() && re15_menu_stage() == 0 && !re15_menu_gameplay_frozen(),
              "nach %d Bildern zu, Spiel frei", f);
        CHECK(!g_inv_screen.hint_aktiv && !re15_menu_map_hint_active(), "Hinweis abgebaut");
    }
    /* zweiter Durchgang: Abbruch (virtuell 0x8000 <- CROSS) schliesst ebenso */
    {
        if (!anfordern()) return 77;
        (void)oeffnen();
        int c0 = g_test_core_se_count, f;
        bild(RE15_PAD_BIT_CROSS);
        CHECK(g_test_core_se_count == c0 + 1 && g_test_core_se_last == 5, "Abbruch: Se(4,5)");
        for (f = 0; f < 200 && (re15_menu_is_open() || re15_menu_stage() != 0); f++) bild(0);
        CHECK(!re15_menu_is_open() && re15_menu_stage() == 0, "Abbruch schliesst (%d Bilder)", f);
    }
    return g_fail;
}

/* Blinker bei verschiedenen Bildraten: die WANDUHR-Zeiten der Phasenwechsel muessen
 * gleich bleiben (Fehlerklasse Titelmenue). */
static int blink_messen(uint64_t dt, uint64_t *wechsel, int max)
{
    int n = 0, rot;
    uint64_t t = 777777ull, t0 = t;
    re15_host_clock_set_us(t);
    re15_map_hint_begin();
    rot = re15_map_hint_rot();
    while (t - t0 < 3000000ull && n < max) {
        t += dt;
        re15_host_clock_set_us(t);
        re15_map_hint_tick();
        if (re15_map_hint_rot() != rot) { rot = re15_map_hint_rot(); wechsel[n++] = t - t0; }
    }
    return n;
}

static int riegel_takt(void)
{
    static const uint64_t dts[3] = { 33333ull, 16667ull, 6944ull };   /* 30 / 60 / 144 Bilder/s */
    uint64_t w[3][32]; int n[3];
    for (int i = 0; i < 3; i++) n[i] = blink_messen(dts[i], w[i], 32);
    int ok = (n[0] == n[1] && n[1] == n[2] && n[0] == 5);   /* Schritte 2,41,80,119,158 < 3 s */
    for (int k = 0; ok && k < n[0]; k++) {
        uint64_t soll = ((uint64_t)(2 + 39 * k) * 1000000000ull + 59825) / 59826;
        for (int i = 0; i < 3; i++)
            if (!(w[i][k] >= soll && w[i][k] < soll + dts[i])) ok = 0;
    }
    printf("     Phasenwechsel (us): 30 Hz %llu %llu %llu | 60 Hz %llu %llu %llu | 144 Hz %llu %llu %llu\n",
           (unsigned long long)w[0][0], (unsigned long long)w[0][1], (unsigned long long)w[0][2],
           (unsigned long long)w[1][0], (unsigned long long)w[1][1], (unsigned long long)w[1][2],
           (unsigned long long)w[2][0], (unsigned long long)w[2][1], (unsigned long long)w[2][2]);
    CHECK(ok, "Phasenwechsel bei 30/60/144 Bildern je s zur selben Wanduhr (je %d/%d/%d, Soll Schritt 2+39k / 59,826 Hz)",
          n[0], n[1], n[2]);
    /* Stillstand: 10 s ohne Bild, dann weiter — Zustand wie nach der vollen Zeit, hoechstens ein Ton */
    {
        uint64_t t = 1000000ull;
        re15_host_clock_set_us(t); re15_map_hint_begin();
        int h0 = g_test_hint_se_count;
        t += 10000000ull; re15_host_clock_set_us(t); re15_map_hint_tick();
        uint64_t soll_schritte = re15_map_hint_vblanks(10000000ull);
        uint8_t z = RE15_HINT_ZAEHLER_START; (void)z;
        CHECK(re15_map_hint_schritte() == soll_schritte && g_test_hint_se_count - h0 <= 1,
              "Stillstand 10 s: %llu Schritte (Soll %llu), %d Ton", (unsigned long long)re15_map_hint_schritte(),
              (unsigned long long)soll_schritte, g_test_hint_se_count - h0);
    }
    return g_fail;
}

/* ============================================================================================ */
static int riegel_spurlos(void)
{
    grundstellung();
    uint8_t vis0[32], vis1[32], fl0[16], fl1[16];
    static uint32_t flags0[sizeof g_game.flags / sizeof(uint32_t)];
    re15_map_visited_export(vis0);
    re15_map_visited_floor_export(fl0);
    memcpy(flags0, g_game.flags, sizeof flags0);
    uint32_t own0 = re15_map_owned_bits();
    uint8_t pg0 = re15_inv_map_page(), rm0 = re15_inv_map_room();
    int bek3_0 = re15_map_page_known(3);

    if (!anfordern()) return 77;
    (void)oeffnen();
    for (int f = 0; f < 90; f++) bild(0);              /* 3 s Blinken */
    bild(RE15_PAD_BIT_START);
    for (int f = 0; f < 200 && (re15_menu_is_open() || re15_menu_stage() != 0); f++) bild(0);

    re15_map_visited_export(vis1);
    re15_map_visited_floor_export(fl1);
    CHECK(memcmp(vis0, vis1, 32) == 0, "Besucht-Bits (32 B) bitgleich");
    CHECK(memcmp(fl0, fl1, 16) == 0, "Etagen-Bits (16 B) bitgleich");
    CHECK(memcmp(flags0, g_game.flags, sizeof flags0) == 0, "Spiel-Flags g_game.flags bitgleich");
    CHECK(re15_map_owned_bits() == own0, "Besitz-Bits 0x%08X -> 0x%08X", own0, re15_map_owned_bits());
    CHECK(re15_inv_map_page() == pg0 && re15_inv_map_room() == rm0,
          "persistente Kartenregister 260e/260d: %d/%d -> %d/%d", pg0, rm0, re15_inv_map_page(), re15_inv_map_room());
    CHECK(re15_map_page_known(3) == bek3_0, "Blatt 3 erblaetterbar: %d -> %d", bek3_0, re15_map_page_known(3));
    CHECK(re15_map_rect_state(3, 9) == RE15_MAP_RECT_UNVISITED, "Blatt 3 Rechteck 9 weiter UNVISITED (%d)",
          re15_map_rect_state(3, 9));
    CHECK(re15_map_hint_pending() < 0, "keine Anforderung mehr");

    /* normaler Kartenaufruf: Statusschirm + L1 */
    re15_menu_toggle();
    bild(RE15_PAD_BIT_L1);
    for (int f = 0; f < 40; f++) bild(0);
    CHECK(re15_menu_substate() == 1 && g_inv_screen.map_page == 4 && !g_inv_screen.hint_aktiv,
          "normaler Aufruf: substate %d, Blatt %d (Soll 4 = POLICE STATION 3F), hint_aktiv %d",
          re15_menu_substate(), g_inv_screen.map_page, g_inv_screen.hint_aktiv);
    CHECK(re15_map_rect_state(4, 2) == RE15_MAP_RECT_CURRENT, "Irons' Buero (Blatt 4 Rechteck 2) wieder AKTUELL (%d)",
          re15_map_rect_state(4, 2));
    re15_menu_toggle();
    return g_fail;
}

/* ============================================================================================ */
static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];

static int ziel_op(const re15_inv_op_t *ops, int n, int *clut)
{
    for (int i = 0; i < n; i++) {
        const re15_inv_op_t *o = &ops[i];
        if (o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_MAP4 &&
            o->x == 156 && o->y == 76 && o->w == 48 && o->h == 40 && o->u == 208 && o->v == 80) {
            if (clut) *clut = o->clut;
            return i;
        }
    }
    return -1;
}
static int marker_op(const re15_inv_op_t *ops, int n)
{
    for (int i = 0; i < n; i++)
        if (ops[i].kind == RE15_INV_OP_SPRT && ops[i].page == RE15_INV_PAGE_TEX4 &&
            ops[i].u == 224 && ops[i].v == 128 && ops[i].w == 8 && ops[i].h == 8) return i;
    return -1;
}
static int aktuell_ops(const re15_inv_op_t *ops, int n, int ausser)
{
    int k = 0;
    for (int i = 0; i < n; i++)
        if (i != ausser && ops[i].kind == RE15_INV_OP_SPRT && ops[i].clut == RE15_INV_CLUT_MAP_AKTUELL) k++;
    return k;
}

static int riegel_zeichner(void)
{
    grundstellung();
    int pg = -1, rc = -1, x, y, w, h, u, v;
    CHECK(re15_map_hint_ziel(0, &pg, &rc) && pg == 3 && rc == 9,
          "Ziel ROOM10F0 Zone 0 = Blatt %d Rechteck %d (festgehalten: 3 / 9; aendert sich die Zonen-Tabelle, faellt dieser Riegel)", pg, rc);
    re15_map_rect_geometry(3, 9, &x, &y, &w, &h);
    re15_map_rect_uv(3, 9, &u, &v);
    CHECK(x == 156 && y == 76 && w == 48 && h == 40 && u == 208 && v == 80,
          "Kachel (%d,%d) %dx%d uv (%d,%d) (Soll (156,76) 48x40 uv (208,80))", x, y, w, h, u, v);
    CHECK(re15_map_rect_state(3, 9) == RE15_MAP_RECT_UNVISITED && !re15_map_owned_page(3),
          "Ziel unbesucht (%d), Blatt 3 nicht im Besitz (%d)", re15_map_rect_state(3, 9), re15_map_owned_page(3));

    /* Vergleich: normale Karte (Blatt 4) vor dem Hinweis */
    static re15_inv_op_t norm0[RE15_INV_MAX_OPS];
    re15_menu_toggle(); bild(RE15_PAD_BIT_L1);
    for (int f = 0; f < 40; f++) bild(0);
    int n_norm0 = re15_inv_screen_build(&g_inv_screen, norm0, RE15_INV_MAX_OPS);
    CHECK(g_inv_screen.map_page == 4 && marker_op(norm0, n_norm0) >= 0, "normale Karte: Blatt %d mit Spielermarker", g_inv_screen.map_page);
    re15_menu_toggle();

    if (!anfordern()) return 77;
    (void)oeffnen();
    int gesehen_rot = 0, gesehen_umriss = 0, ok = 1;
    for (int f = 0; f < 60; f++) {
        bild(0);
        int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
        int clut = -1, zi = ziel_op(s_ops, n, &clut);
        int soll = g_inv_screen.hint_rot ? RE15_INV_CLUT_MAP_AKTUELL : RE15_INV_CLUT_MAP_UNBESUCHT;
        if (zi < 0 || clut != soll) ok = 0;
        if (marker_op(s_ops, n) >= 0) ok = 0;
        if (aktuell_ops(s_ops, n, zi) != 0) ok = 0;
        if (re15_map_zone_current() == NULL) ok = 0;   /* Schalter nach dem Bauen wieder aus */
        if (g_inv_screen.hint_rot) gesehen_rot++; else gesehen_umriss++;
    }
    CHECK(ok, "60 Bilder: Zielkachel immer da, CLUT folgt hint_rot, kein Marker, kein fremdes AKTUELL, Schalter aus");
    CHECK(gesehen_rot > 0 && gesehen_umriss > 0, "beide Phasen gezeichnet (rot %d / Umriss %d Bilder)", gesehen_rot, gesehen_umriss);
    bild(RE15_PAD_BIT_START);
    for (int f = 0; f < 200 && (re15_menu_is_open() || re15_menu_stage() != 0); f++) bild(0);

    /* normale Karte nach dem Hinweis: Op-Liste bitgleich zu vorher */
    static re15_inv_op_t norm1[RE15_INV_MAX_OPS];
    re15_menu_toggle(); bild(RE15_PAD_BIT_L1);
    for (int f = 0; f < 40; f++) bild(0);
    int n_norm1 = re15_inv_screen_build(&g_inv_screen, norm1, RE15_INV_MAX_OPS);
    int gleich = (n_norm0 == n_norm1);
    for (int i = 0; gleich && i < n_norm0; i++) {
        const re15_inv_op_t *a = &norm0[i], *b = &norm1[i];
        /* der Marker pulsiert (ecg_glow) — Farbe nicht vergleichen, Lage schon */
        if (a->kind != b->kind || a->page != b->page || a->clut != b->clut || a->x != b->x ||
            a->y != b->y || a->w != b->w || a->h != b->h || a->u != b->u || a->v != b->v) gleich = 0;
    }
    CHECK(gleich, "normale Karte vor/nach dem Hinweis: %d / %d Ops, gleiche Lage und CLUT", n_norm0, n_norm1);
    CHECK(aktuell_ops(norm1, n_norm1, -1) > 0, "normale Karte hebt Irons' Buero hervor (%d AKTUELL-Ops)", aktuell_ops(norm1, n_norm1, -1));
    re15_menu_toggle();
    return g_fail;
}

/* ============================================================================================ */
static int riegel_bank(void)
{
    char p[600];
    size_t n = 0, n2 = 0;
    snprintf(p, sizeof p, "%s/HINTSE.VBS", RE15_ASSET_RE2_DIR);
    uint8_t *buf = read_file(p, &n);
    if (!buf) { printf("  FAIL: %s fehlt\n", p); return 1; }
    re15_map_hint_bank_rec_t rec;
    re15_map_hint_bank_rec(&rec);
    CHECK(n == 8280 && n == rec.edt_size + rec.vbd_size, "HINTSE.VBS %lu B (Soll 8280 = %u + %u)",
          (unsigned long)n, rec.edt_size, rec.vbd_size);
    CHECK(rec.se_hint == 0x2B, "Satz 0x%02X (Soll 0x2B, lui a0,0x22b @0x8006F234)", rec.se_hint);

    snprintf(p, sizeof p, "%s/ROOM3010.RDT", RE15_RE2_RDT_DIR);
    uint8_t *re2 = read_file(p, &n2);
    if (re2) {
        CHECK(n2 >= 0x39788 + 4480 && memcmp(re2 + 0x39788, buf + rec.vbd_off, 4480) == 0,
              "Welle bitgleich zu RE2 ROOM3010.RDT VAG 18 @0x39788 (4480 B)");
        CHECK(memcmp(re2 + 0x1F824, buf + 0x2B * 4, 4) == 0, "EDT-Satz 0x2B = ROOM3010.RDT @0x1F824 (00 01 23 00)");
        free(re2);
    } else {
        printf("  HINWEIS: %s fehlt - Wellenvergleich uebersprungen\n", p);
    }

    static re15_vab_t vab;
    uint32_t vh_off = (uint32_t)buf[rec.edt_size - 8] | ((uint32_t)buf[rec.edt_size - 7] << 8) |
                      ((uint32_t)buf[rec.edt_size - 6] << 16) | ((uint32_t)buf[rec.edt_size - 5] << 24);
    int rc = re15_vab_parse(buf + vh_off, (size_t)rec.edt_size - vh_off, &vab);
    CHECK(rc == 0 && vab.vag_count == 1 && vab.samples[0].size == 4480, "VAB: rc %d, %d Welle(n), %lu B",
          rc, vab.vag_count, (unsigned long)vab.samples[0].size);
    re15_edt_rec_t er;
    rc = re15_edt_decode(buf, 0x2B, &er);
    CHECK(rc == 0 && !er.empty && er.prog == 1 && er.tone == 2, "Satz 0x2B -> Programm %d Ton %d (Soll 1/2)", er.prog, er.tone);
    int vags[8], tones[8];
    int nl = re15_edt_resolve_layers_ex(buf, &vab, 0x2B, vags, tones, 8);
    CHECK(nl == 1, "eine Lage (%d)", nl);
    if (nl >= 1) {
        const re15_vab_tone_t *t = &vab.tones[tones[0]];
        uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
        CHECK(pitch == 0x0400 && t->center_note == 85 && t->min_note == 61,
              "Pitch 0x%04X = %lu Hz (Soll 0x0400 = 11025 Hz; center 85, Note 61)", pitch,
              (unsigned long)((44100ul * pitch) >> 12));
    }
    free(buf);
    return g_fail;
}

int main(int argc, char **argv)
{
    const char *was = argc > 1 ? argv[1] : "";
    printf("== unit_r30_hinweis_%s ==\n", was);
    int rc;
    if      (!strcmp(was, "anker"))    rc = riegel_anker();
    else if (!strcmp(was, "fsm"))      { rc = riegel_fsm(); if (rc != 77) rc = riegel_takt(); }
    else if (!strcmp(was, "spurlos"))  rc = riegel_spurlos();
    else if (!strcmp(was, "zeichner")) rc = riegel_zeichner();
    else if (!strcmp(was, "bank"))     rc = riegel_bank();
    else { printf("Aufruf: %s anker|fsm|spurlos|zeichner|bank\n", argv[0]); return 2; }
    printf("%s\n", rc == 0 ? "ALLE PRUEFUNGEN BESTANDEN" : (rc == 77 ? "SKIP" : "FEHLGESCHLAGEN"));
    return rc;
}
