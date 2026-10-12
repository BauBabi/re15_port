/*
 * r31_generator.c — RIEGEL Runde 31, Thema G: das Generator-Raetsel ROOM11F0/11F1 nimmt
 * erst ab, wenn der Leistungszeiger FINAL auf 80 steht.
 *
 * Nutzer 2026-09-29: "warte erst bis der zeiger final auf 80 steht, bevor du mit ok das
 * abnimmst, das Licht anschaltest etc."
 *
 * SOLL (RE2-Angleichung, alle Belege im Kopf von include/re15_panel_zeiger.h und in
 * analysis/befunde_runde31/generator.md):
 *   RE2 ROOM2130.RDT sub04: Nachfuehrschleife (@0x011E0 while .. @0x01216 evt_next ..
 *   @0x01708 ewhile), dann sleep 30 (@0x0171C `09` / @0x0171D `0a 1e 00`), erst dann
 *   cmp(var5 == 80) @0x01752 + "Power supply OK." @0x01758 + Flag @0x0175E + Ton @0x01762.
 *   Eingabe gesperrt ueber Bank 2 Bit 7 (@0x01110 .. @0x01818).
 *   => letzter Zeigerschritt im Bild k -> Abnahme (RE1.5: Evt_exec(sub18) @0x012E6 +
 *      Set(4,238,1) @0x012EA) genau im Bild k+31.
 *
 *   A  Anker: die gehaltenen Bytes stehen so in ROOM11F0.RDT UND ROOM11F1.RDT.
 *   B  Reihenfolge: keine Abnahme waehrend der Fahrt, keine in den 30 Stillstandsbildern,
 *      Abnahme (4:238, sub18 = 4:243, Bestaetigungston) genau im Bild k+31; Zeiger bleibt 80.
 *   C  Falsche Stellungen loesen nie aus — auch nicht, wenn der Zeiger auf dem Weg nach 90
 *      durch die 80 faehrt; eine Aenderung im Stillstandsfenster startet die 30 neu.
 *   D  Eingabesperre — SEIT RUNDE 34 NACHT NUR DIE ENDSPERRE (Nutzer 2026-09-30: "man soll
 *      sich frei bewegen koennen Ausser ganz am Ende"): nach einem ZWISCHEN-Schalter bewegt
 *      der D-Pad den Cursor waehrend Fahrt und Ruhe (keine Sperre; RE1.5 setzt Bank 2 Bit 7
 *      in sub01..sub17 nie, nur @0x01736/@0x017B8); ist die Maske die Loesung (0x155,
 *      @0x012BE..0x012E2), steht der Cursor und Quadrat legt nichts um, bis zur Abnahme.
 *   E  Wiedereintritt nach dem Loesen: Zeiger sofort 80, keine Sperre, kein zweiter Ton.
 *   F  ROOM11F1 (Elzas Variante) verhaelt sich wie ROOM11F0.
 *   G  RAHMEN DES SCHALTERFELDS (Runde 36 G2): der Cursor faehrt nicht unbegrenzt. Das Original schiebt
 *      ihn in jedem Bild aus vier Klasse-2-Zellen um das Schalterfeld (Ruhe-Zweig LAB_8002c0b8 `jal
 *      0x8003b558` @0x8002c0d0, Ausschiebung FUN_8003da78 `jal` @0x8003b79c); dieselbe Fahrt wie am
 *      Original (OBEN, UNTEN, LINKS, RECHTS, je bis 15 Bilder Stillstand) endet an denselben Kanten
 *      nach derselben Zahl Fahrbilder. Teil D faehrt seit G2 VOR der 49-Bilder-Probe an den unteren
 *      Rahmen, weil 49 Bilder HOCH ab der Startlage z 22684 nach 33 Bildern an den oberen Rahmen
 *      laufen (Original B033) — der Stillstand dort ist die Ausschiebung, keine Sperre.
 *
 * RUECKBAU-NACHWEIS (in dieser Runde gefahren, Ergebnis im Dossier §6):
 *   op_evt_exec-Haken entfernt                   => B ROT (Abnahme im Bild nach dem Bit)
 *   RE15_PANEL_RUHE_BILDER 30 -> 0               => B ROT (Abnahme bei k+1 statt k+31)
 *   Pad-Maske in game_step_common.c entfernt     => D ROT
 * Runde 34 Nacht (Teil D neu, Nachweis in analysis/befunde_runde34_nacht/C_generator.md §9.4):
 *   Endsperre aus (sperrt() = 0)                 => D ROT (Cursor faehrt bei Maske 0x155)
 *   alte Runde-31-Zwischensperre wieder an       => D ROT (Cursor steht nach Schalter 7)
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
extern unsigned g_re15_panel_bestaet_zaehler;

/* Der SOLL-Stillstand steht hier bewusst als ZAHL mit Beleg und nicht als Makro aus dem
 * Port-Kopf: sonst bestaetigte sich der Riegel selbst (Rueckbau RE15_PANEL_RUHE_BILDER
 * 30 -> 0 lief mit dem Makro gruen durch, weil auch die Erwartung auf k+1 schrumpfte).
 * RE2 ROOM2130.RDT sub04 @0x0171C `09` + @0x0171D `0a 1e 00` = sleep 0x1E. */
#define RE2_SLEEP_BILDER 30

/* Runde 36 G2 — der RAHMEN des Schalterfelds, AM ORIGINAL GEMESSEN (DuckStation + GDB, Auslieferungs-EXE
 * @0x80026e4c `08 00 e0 03`; Spielstand analysis/befunde_runde36/G2_belege/room11f0_orig.sav, Lauf
 * G2_belege/orig_cursor_lauf1/log.txt: echte SCD-Fahrt sub02..05 @0x012F2..0x0131E, Raetsel scharf wie sub16
 * @0x015C2..0x015F2, Haltepunkte @0x8002bd44 / @0x8003da78 / @0x8002c158). Zellen: ROOM11F0 SCA, Klasse 2
 * (`u0 0xff`, FUN_8003b558 @0x8003b64c-660), Band 0 (`fl 0x03`, @0x8003b634-644), in jedem der fuenf Bereiche
 * gleich (Bereich 1 = RDT @0x009DC..0x00A00). Endlage = Kante + r 900 + 18 (FUN_8003da78 @0x8003dab0/@0x8003dab8).
 *   OBEN    B033 `nach-SCD (-19554,29284) -> nach-Pass (-19554,29232)`, Zelle @0x801223b0 r1[16] z 30150..31150
 *   UNTEN   B114 `(-19554,16032) -> (-19554,16068)`,  Zelle @0x8012238c r1[13] z 14150..15150
 *   LINKS   B181 `(-29954,16068) -> (-29782,16068)`,  Zelle @0x80122398 r1[14] x -31700..-30700
 *   RECHTS  B297 `(-9582,16068)  -> (-9668,16068)`,   Zelle @0x801222d8 r0[15] x -8750..-7750
 * Bilanz je Phase (Bilder mit Bewegung bis 15 Bilder Stillstand): OBEN 33 ab (-19554,22684), UNTEN 66, LINKS 52,
 * RECHTS 101 (Auswertung des Laufs, G2_generator_ruhezweig.md P2.2). Als ZAHLEN, nicht aus dem Port gerechnet
 * (Lehre Runde 31: selbstbestaetigende Metrik). */
#define ORIG_RAHMEN_OBEN_Z      29232
#define ORIG_RAHMEN_UNTEN_Z     16068
#define ORIG_RAHMEN_LINKS_X   (-29782)
#define ORIG_RAHMEN_RECHTS_X   (-9668)
#define ORIG_START_X          (-19554)   /* Obj_model_set @0x00E54 pc[10..11] `9e b3` */
#define ORIG_START_Z            22684    /* Obj_model_set @0x00E54 pc[14..15] `9c 58` */
#define ORIG_FAHRT_OBEN            33
#define ORIG_FAHRT_UNTEN           66
#define ORIG_FAHRT_LINKS           52
#define ORIG_FAHRT_RECHTS         101
#define CURSOR_SCHRITT            200    /* sub02 @0x012F6 `2f 02 c8 00` Speed_set(2,+200) */

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

/* Ein Spielbild in der Reihenfolge der PC-Hauptschleife: VM (main.c:5343) vor dem
 * Spielschritt (main.c:7319); der Zeiger-Tick ist der letzte Zustands-Tick des Schritts. */
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

static int room_boot(unsigned rid)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, rid);
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
    pl->x = 250; pl->z = 250; pl->rot_y = 1024;
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

/* Schaltermaske setzen: Bit i = Schalter i+1 = Bank 5 Bit 13+i (@0x012BE..0x012E2). */
static void maske_setzen(unsigned m)
{
    for (int i = 0; i < RE15_PANEL_SCHALTER; i++)
        re15_game_flag_set(5, (uint8_t)(13 + i), (m >> i) & 1u);
}

/* Bilder ohne Taste, bis der Zeiger steht und die Ruhe voll ist (Deckel gegen Endlos). */
static void einschwingen(void)
{
    for (int f = 0; f < 300; f++) {
        frame(0, 0);
        if (re15_panel_zeiger_wert() == re15_panel_zeiger_ziel() &&
            re15_panel_zeiger_ruhe() >= RE15_PANEL_RUHE_BILDER) break;
    }
}

static int cursor_prop(void)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == 0) return i;
    return -1;
}

/* ================= A: Anker ====================================================== */
static int anker_ok(const uint8_t *r)
{
    const uint8_t *p = r + RE15_PANEL_ABNAHME_OFF;
    return p[0] == 0x04 && p[1] == 0xFF && p[2] == 0x18 && p[3] == 0x12 &&
           p[4] == 0x22 && p[5] == 0x04 && p[6] == 0xEE && p[7] == 0x01 &&
           r[0x012B6] == 0x06 && r[0x012BA] == 0x21 && r[0x012BB] == 0x04 && r[0x012BC] == 0xEE;
}

static void teil_a(void)
{
    printf("\n=== A: Anker der gehaltenen Abnahme (@0x012E6/@0x012EA, Block @0x012B6) ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: ROOM11F0.RDT fehlt\n"); g_fail++; return; }
    CHECK("ROOM11F0 @0x012E6 `04 ff 18 12` + @0x012EA `22 04 ee 01`, Ifel_ck @0x012B6", anker_ok(s_raw));
    size_t n0 = s_rawsz; uint8_t *a = (uint8_t *)malloc(n0); memcpy(a, s_raw, n0);
    if (!room_boot(0x11F1)) { printf("FAIL: ROOM11F1.RDT fehlt\n"); g_fail++; free(a); return; }
    CHECK("ROOM11F1 traegt dieselben Anker", anker_ok(s_raw));
    CHECK("ROOM11F1.RDT ist byte-identisch mit ROOM11F0.RDT",
          s_rawsz == n0 && memcmp(a, s_raw, n0) == 0);
    free(a);
}

/* ================= B: die Reihenfolge ============================================ */
/* Faehrt 60 -> 80 (Schalter 5 als letzter, wie im Messlauf) und liefert die Bildnummern. */
static void lauf_60_80(const char *raum, int *bild_80, int *bild_abnahme, int *fahrt_frueh,
                       int *ruhe_frueh, int *ton_bild, int *strom_bild, int *wert_nach)
{
    raetsel_scharf();
    maske_setzen(0x145u);                         /* Schalter 1,3,7,9 = 20-10+20+30 = 60 */
    einschwingen();
    printf("  %s: eingeschwungen auf wert=%d ruhe=%d geloest=%d\n", raum,
           re15_panel_zeiger_wert(), re15_panel_zeiger_ruhe(), re15_game_flag_get(4, 238));
    unsigned t0 = g_re15_panel_bestaet_zaehler;
    *bild_80 = -1; *bild_abnahme = -1; *fahrt_frueh = 0; *ruhe_frueh = 0;
    *ton_bild = -1; *strom_bild = -1;
    maske_setzen(RE15_PANEL_LOESUNGSMASKE);       /* + Schalter 5 -> 80 (@0x012CE Bit 17) */
    for (int f = 0; f < 260; f++) {
        frame(0, 0);
        int g = re15_game_flag_get(4, 238);
        if (g && *bild_abnahme < 0) *bild_abnahme = f;
        if (g_re15_panel_bestaet_zaehler != t0 && *ton_bild < 0) *ton_bild = f;
        if (re15_game_flag_get(4, 243) && *strom_bild < 0) *strom_bild = f;
        if (re15_panel_zeiger_wert() == RE15_PANEL_ZIEL && *bild_80 < 0) *bild_80 = f;
        if (g && *bild_80 < 0) *fahrt_frueh = 1;                 /* Abnahme vor der 80 */
        if (g && *bild_80 >= 0 && f < *bild_80 + 1 + RE2_SLEEP_BILDER) *ruhe_frueh = 1;
    }
    *wert_nach = re15_panel_zeiger_wert();
    printf("  %s: Zeiger auf 80 im Bild %d, Abnahme (4:238) im Bild %d, Ton im Bild %d, "
           "sub18 (4:243) im Bild %d, Zeiger danach %d\n",
           raum, *bild_80, *bild_abnahme, *ton_bild, *strom_bild, *wert_nach);
}

static void teil_b(void)
{
    printf("\n=== B: Abnahme erst k+31 (RE2 @0x01708 ewhile -> @0x0171C sleep 30 -> @0x01752) ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0); re15_game_flag_set(4, 243, 0);
    int b80, bab, ff, rf, tb, sb, wn;
    lauf_60_80("ROOM11F0", &b80, &bab, &ff, &rf, &tb, &sb, &wn);
    CHECK("Port-Konstante = RE2-Sleep (@0x0171D `0a 1e 00`)", RE15_PANEL_RUHE_BILDER == RE2_SLEEP_BILDER);
    CHECK("der Zeiger erreicht die 80 (20 Punkte, 1 je Bild)", b80 == 19);
    CHECK("keine Abnahme waehrend der Fahrt", !ff);
    CHECK("keine Abnahme in den 30 Stillstandsbildern", !rf);
    CHECK("Abnahme genau im Bild k+31", bab == b80 + 1 + RE2_SLEEP_BILDER);
    CHECK("sub18 laeuft im selben Bild (4:243 @0x016F6)", sb == bab);
    CHECK("Bestaetigungston (RE2 Gruppe 2 / 0x0C) im selben Bild, nicht frueher", tb == bab);
    CHECK("der Zeiger bleibt nach dem Loesen auf 80", wn == RE15_PANEL_ZIEL);
    CHECK("nach dem Loesen keine Panel-Sperre mehr", re15_panel_zeiger_sperrt() == 0);
}

/* ================= C: falsche Stellungen ========================================= */
static void teil_c(void)
{
    printf("\n=== C: falsche Stellungen loesen nie aus ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf();
    maske_setzen(0x145u);                               /* 60 */
    einschwingen();
    /* 60 -> 90: Schalter 1,5,7,9 (0x151) = 20+20+20+30. Der Zeiger faehrt DURCH die 80. */
    maske_setzen(0x151u);
    int durch80 = 0, ausgeloest = 0;
    for (int f = 0; f < 200; f++) {
        frame(0, 0);
        if (re15_panel_zeiger_wert() == RE15_PANEL_ZIEL) durch80 = 1;
        if (re15_game_flag_get(4, 238)) ausgeloest = 1;
    }
    printf("  60 -> 90: durch die 80 gefahren=%d, Endwert %d, ausgeloest=%d\n",
           durch80, re15_panel_zeiger_wert(), ausgeloest);
    CHECK("GEGENPROBE: der Zeiger ist wirklich durch die 80 gefahren", durch80);
    CHECK("und steht auf 90", re15_panel_zeiger_wert() == 90);
    CHECK("keine Abnahme bei 90", !ausgeloest);

    /* Loesung, aber im Stillstandsfenster wieder weg und wieder hin: die 30 beginnen neu. */
    maske_setzen(RE15_PANEL_LOESUNGSMASKE);
    int f80 = -1;
    for (int f = 0; f < 40 && f80 < 0; f++) { frame(0, 0); if (re15_panel_zeiger_wert() == 80) f80 = f; }
    for (int f = 0; f < 10; f++) frame(0, 0);           /* 10 von 30 Ruhebildern */
    int vor_weg = re15_game_flag_get(4, 238);
    maske_setzen(0x145u);                               /* weg (60) */
    frame(0, 0);
    maske_setzen(RE15_PANEL_LOESUNGSMASKE);             /* und wieder hin */
    int abnahme = -1, b80 = -1;
    for (int f = 0; f < 120; f++) {
        frame(0, 0);
        if (re15_panel_zeiger_wert() == 80 && b80 < 0) b80 = f;
        if (re15_game_flag_get(4, 238) && abnahme < 0) abnahme = f;
    }
    printf("  Loesung/weg/Loesung: vorher geloest=%d, wieder auf 80 im Bild %d, Abnahme %d\n",
           vor_weg, b80, abnahme);
    CHECK("nach 10 Ruhebildern noch keine Abnahme", vor_weg == 0);
    CHECK("nach dem Hin und Her beginnt die Ruhe neu (Abnahme k+31 ab der NEUEN 80)",
          b80 >= 0 && abnahme == b80 + 1 + RE2_SLEEP_BILDER);
}

/* ================= D: Eingabesperre — nur am Ende (Runde 34 Nacht) ================ */
static unsigned maske_lesen(void)
{
    unsigned m = 0;
    for (int i = 0; i < 10; i++) if (re15_game_flag_get(5, (uint8_t)(13 + i))) m |= 1u << i;
    return m;
}

static void teil_d(void)
{
    printf("\n=== D: Eingabesperre nur am Ende (Runde 34 Nacht; Endsperre = RE2 Bank 2 Bit 7 @0x01110..@0x01818) ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf();
    maske_setzen(0);
    einschwingen();
    int ci = cursor_prop();
    CHECK("Cursor-Prop obj 0 vorhanden (@0x00E54)", ci >= 0);
    if (ci < 0) return;

    /* (1) ZWISCHEN-Schalter 7 (+20): Fahrt 20 Bilder, dann 30 Ruhe. Die ganze Zeit HOCH
     * halten — der Cursor faehrt (sub02 @0x012F6 Speed_set(2,+200)), keine Sperre.
     * Runde 36 G2: 49 Bilder HOCH passen nur vom UNTEREN Rahmen aus ins Schalterfeld (16068 + 48*200 =
     * 25668 < 29232); ab der Startlage z 22684 steht der Cursor im Original nach 33 Bildern am oberen
     * Rahmen (orig_cursor_lauf1 B033, Konstanten oben). Darum zuerst UNTEN an den Rahmen fahren. */
    for (int f = 0; f < 120; f++)
        frame(RE15_PAD_BIT_DOWN, (uint16_t)(f == 0 ? RE15_PAD_BIT_DOWN : 0));
    for (int f = 0; f < 3; f++) frame(0, 0);
    printf("  Vorlauf UNTEN: Cursor bei (%d,%d)\n", (int)g_scd.props[ci].x, (int)g_scd.props[ci].z);
    CHECK("Vorlauf: der Cursor steht am unteren Rahmen (Original z 16068, B114)",
          g_scd.props[ci].z == ORIG_RAHMEN_UNTEN_Z);
    maske_setzen(1u << 6);
    int32_t z1 = g_scd.props[ci].z;
    int sperr_bilder = 0, bilder = 0, steh = 0;
    for (int f = 0; f < 20 + RE2_SLEEP_BILDER - 1; f++) {
        int32_t zv = g_scd.props[ci].z;
        frame(RE15_PAD_BIT_UP, (uint16_t)(f == 0 ? RE15_PAD_BIT_UP : 0));
        bilder++;
        if (re15_panel_zeiger_sperrt()) sperr_bilder++;
        if (f > 0 && g_scd.props[ci].z == zv) steh++;       /* Bild 0: Pad wirkt ab der naechsten VM */
    }
    printf("  Zwischenschalter 7, Fahrt + Ruhe (%d Bilder) HOCH gehalten: dz=%d, Stillstandsbilder %d, "
           "Sperrbilder %d, Zeiger %d\n", bilder, (int)(g_scd.props[ci].z - z1), steh, sperr_bilder,
           re15_panel_zeiger_wert());
    CHECK("nach einem Zwischenschalter bewegt sich der Cursor in jedem Bild (keine Sperre)",
          steh == 0 && g_scd.props[ci].z - z1 > 0);
    CHECK("und zwar +200 je Bild ab dem zweiten (sub02 @0x012F6): dz = 48*200",
          g_scd.props[ci].z - z1 == 48 * CURSOR_SCHRITT);
    CHECK("und die Sperre stand in keinem Bild", sperr_bilder == 0);
    CHECK("der Zeiger ist trotzdem auf 20 gefahren (1 Punkt je Bild, @0x01216)", re15_panel_zeiger_wert() == 20);

    /* (1b) Runde 36 G2: weiter HOCH bis zum Stillstand — der Cursor steht am OBEREN RAHMEN, genau an der
     * Lage des Originals (B033: 29284 -> 29232), und die Panel-Sperre ist dabei in keinem Bild an. */
    int sperr_b = 0, steh_b = 0, ueber = 0;
    for (int f = 0; f < 120 && steh_b < 15; f++) {
        int32_t zv = g_scd.props[ci].z;
        frame(RE15_PAD_BIT_UP, 0);
        if (re15_panel_zeiger_sperrt()) sperr_b++;
        if (g_scd.props[ci].z > ORIG_RAHMEN_OBEN_Z) ueber++;
        steh_b = (g_scd.props[ci].z == zv) ? steh_b + 1 : 0;
    }
    printf("  weiter HOCH: Cursor steht bei z=%d (%d Bilder Stillstand), Sperrbilder %d, ueber dem Rahmen %d\n",
           (int)g_scd.props[ci].z, steh_b, sperr_b, ueber);
    CHECK("der Stillstand ist der obere Rahmen des Originals (z 29232, B033), nicht eine Sperre",
          g_scd.props[ci].z == ORIG_RAHMEN_OBEN_Z && steh_b >= 15 && sperr_b == 0);
    CHECK("der Cursor war in keinem Bild jenseits des Rahmens", ueber == 0);
    /* wieder ins Feld (20 Bilder UNTEN), damit "steht trotz HOCH" in (2) nicht schon am Rahmen gilt */
    for (int f = 0; f < 20; f++) frame(RE15_PAD_BIT_DOWN, (uint16_t)(f == 0 ? RE15_PAD_BIT_DOWN : 0));
    for (int f = 0; f < 3; f++) frame(0, 0);

    /* (2) ENDSPERRE: 60 einschwingen, dann Schalter 5 -> Maske = Loesung 0x155. Ab diesem
     * Bild HOCH+QUADRAT halten: Cursor steht, kein Schalterwechsel, Sperre in jedem Bild bis
     * zur Abnahme (k+31). */
    maske_setzen(0x145u);
    einschwingen();
    maske_setzen(RE15_PANEL_LOESUNGSMASKE);
    int32_t x2 = g_scd.props[ci].x, z2 = g_scd.props[ci].z;
    int k = -1, ab = -1, luecke = 0, umgelegt = 0, n = 0;
    for (int f = 0; f < 200; f++) {
        uint16_t b = (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_SQUARE);
        int gesperrt_vor = re15_panel_zeiger_sperrt();
        frame(b, (uint16_t)(f == 0 ? b : 0));
        if (k < 0 && re15_panel_zeiger_wert() == RE15_PANEL_ZIEL) k = f;
        if (ab < 0 && re15_game_flag_get(4, 238)) ab = f;
        if (ab < 0) {
            n++;
            if (!gesperrt_vor || !re15_panel_zeiger_sperrt()) luecke++;
            if (maske_lesen() != RE15_PANEL_LOESUNGSMASKE) umgelegt++;
        }
        if (ab >= 0) break;
    }
    printf("  Loesungsmaske: Zeiger 80 im Bild %d, Abnahme %d, %d Bilder davor: Sperrluecken %d, "
           "Schalterwechsel %d, Cursor dx=%d dz=%d\n", k, ab, n, luecke, umgelegt,
           (int)(g_scd.props[ci].x - x2), (int)(g_scd.props[ci].z - z2));
    CHECK("Endsperre: gesperrt in jedem Bild ab Maske 0x155 bis zur Abnahme", k >= 0 && ab > 0 && luecke == 0);
    CHECK("Endsperre: der Cursor steht trotz HOCH", g_scd.props[ci].x == x2 && g_scd.props[ci].z == z2);
    CHECK("Endsperre: gehaltenes Quadrat legt keinen Schalter um", umgelegt == 0);
    CHECK("Abnahme genau im Bild k+31 (RE2 sleep 30 @0x0171D)", ab == k + 1 + RE2_SLEEP_BILDER);
    CHECK("nach der Abnahme keine Panel-Sperre mehr (4:238 @0x012EA)", re15_panel_zeiger_sperrt() == 0);
    /* (3) Ausserhalb des Raetsels (Bank 5 Bit 0 aus) sperrt nichts, selbst bei Maske 0x155. */
    re15_game_flag_set(4, 238, 0);
    re15_game_flag_set(5, 0, 0);
    maske_setzen(RE15_PANEL_LOESUNGSMASKE);
    frame(0, 0);
    CHECK("Raetsel aus (5:0 = 0): keine Sperre, auch bei Loesungsmaske", re15_panel_zeiger_sperrt() == 0);
    re15_game_flag_set(4, 238, 0);
}

/* ================= E: Wiedereintritt nach dem Loesen ============================= */
static void teil_e(void)
{
    printf("\n=== E: Wiedereintritt nach dem Loesen ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 1);                      /* geloest (global, im Spielstand) */
    re15_panel_zeiger_reset();
    unsigned t0 = g_re15_panel_bestaet_zaehler;
    frame(0, 0);
    for (int f = 0; f < 40; f++) frame(0, 0);
    printf("  wert=%d ruhe=%d sperrt=%d Ton-Zaehler +%u\n", re15_panel_zeiger_wert(),
           re15_panel_zeiger_ruhe(), re15_panel_zeiger_sperrt(), g_re15_panel_bestaet_zaehler - t0);
    CHECK("Zeiger steht sofort auf 80 (keine Fahrt beim Eintritt)", re15_panel_zeiger_wert() == 80);
    CHECK("keine Sperre", re15_panel_zeiger_sperrt() == 0);
    CHECK("kein zweiter Bestaetigungston", g_re15_panel_bestaet_zaehler == t0);
    re15_game_flag_set(4, 238, 0);
}

/* ================= F: ROOM11F1 ================================================== */
static void teil_f(void)
{
    printf("\n=== F: ROOM11F1 (Elzas Variante) wie ROOM11F0 ===\n");
    if (!room_boot(0x11F1)) { printf("FAIL: ROOM11F1.RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0); re15_game_flag_set(4, 243, 0);
    int b80, bab, ff, rf, tb, sb, wn;
    lauf_60_80("ROOM11F1", &b80, &bab, &ff, &rf, &tb, &sb, &wn);
    CHECK("ROOM11F1: Zeiger faehrt (80 nach 20 Bildern)", b80 == 19);
    CHECK("ROOM11F1: Abnahme genau im Bild k+31", bab == b80 + 1 + RE2_SLEEP_BILDER);
    CHECK("ROOM11F1: keine Abnahme vorher", !ff && !rf);
    g_re15_active_cut = RE15_PANEL_CUT;
    int sx = 0, sy = 0;
    CHECK("ROOM11F1: Zeiger auf Cut 10 sichtbar", re15_panel_zeiger_sicht(&sx, &sy) == 1);
    re15_game_flag_set(4, 238, 0); re15_game_flag_set(4, 243, 0);
}

/* ================= G: Rahmen des Schalterfelds (Runde 36 G2) ===================== */
/* Taste halten, bis der Cursor nach dem Anlaufen 15 Bilder steht — wie die Original-Messung
 * (G2_belege/orig_11f0_cursor.py phase(): steh_ende 15), danach 5 Bilder ohne Taste.
 * Rueckgabe: Bilder mit Bewegung. */
static int halte_bis_stand(uint16_t taste, int ci, int *sperr)
{
    int bewegt = 0, steh = 0;
    *sperr = 0;
    for (int f = 0; f < 200 && steh < 15; f++) {
        const int32_t vx = g_scd.props[ci].x, vz = g_scd.props[ci].z;
        frame(taste, (uint16_t)(f == 0 ? taste : 0));
        if (re15_panel_zeiger_sperrt()) (*sperr)++;
        if (g_scd.props[ci].x != vx || g_scd.props[ci].z != vz) { bewegt++; steh = 0; }
        else if (bewegt > 0) steh++;
    }
    for (int f = 0; f < 5; f++) frame(0, 0);
    return bewegt;
}

static void teil_g(void)
{
    printf("\n=== G: Rahmen des Schalterfelds = Original (Runde 36 G2; Ruhe-Zweig @0x8002c0d0, FUN_8003da78 @0x8003b79c) ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf();
    maske_setzen(0);
    einschwingen();
    int ci = cursor_prop();
    CHECK("Cursor-Prop obj 0 vorhanden (@0x00E54)", ci >= 0);
    if (ci < 0) return;
    CHECK("Startlage = Obj_model_set @0x00E54 pc[10..11]/[14..15] (-19554, 22684)",
          g_scd.props[ci].x == ORIG_START_X && g_scd.props[ci].z == ORIG_START_Z);
    CHECK("obj+0 = 0x0101 (pc[6..7] `00 01` | 1, @0x80040990-9a4): Bit 3 frei, die Zellen zaehlen (@0x8003b668-674)",
          g_scd.props[ci].flags == 0x0101u);
    static const struct { const char *name; uint16_t taste; int fahrt; int32_t x, z; } P[4] = {
        { "OBEN",   RE15_PAD_BIT_UP,    ORIG_FAHRT_OBEN,   ORIG_START_X,         ORIG_RAHMEN_OBEN_Z  },
        { "UNTEN",  RE15_PAD_BIT_DOWN,  ORIG_FAHRT_UNTEN,  ORIG_START_X,         ORIG_RAHMEN_UNTEN_Z },
        { "LINKS",  RE15_PAD_BIT_LEFT,  ORIG_FAHRT_LINKS,  ORIG_RAHMEN_LINKS_X,  ORIG_RAHMEN_UNTEN_Z },
        { "RECHTS", RE15_PAD_BIT_RIGHT, ORIG_FAHRT_RECHTS, ORIG_RAHMEN_RECHTS_X, ORIG_RAHMEN_UNTEN_Z },
    };
    for (int d = 0; d < 4; d++) {
        int sperr = 0;
        const int32_t sx = g_scd.props[ci].x, sz = g_scd.props[ci].z;
        const int n = halte_bis_stand(P[d].taste, ci, &sperr);
        printf("  %-6s start (%d,%d) bewegt %3d ende (%d,%d) Sperrbilder %d | Original: bewegt %3d ende (%d,%d)\n",
               P[d].name, (int)sx, (int)sz, n, (int)g_scd.props[ci].x, (int)g_scd.props[ci].z, sperr,
               P[d].fahrt, (int)P[d].x, (int)P[d].z);
        char t[200];
        snprintf(t, sizeof t, "%s: %d Fahrbilder, Stand am Rahmen (%d,%d) wie das Original, keine Sperre",
                 P[d].name, P[d].fahrt, (int)P[d].x, (int)P[d].z);
        CHECK(t, n == P[d].fahrt && g_scd.props[ci].x == P[d].x && g_scd.props[ci].z == P[d].z && sperr == 0);
    }
    CHECK("dabei kein Schalter umgelegt (Fahrt ohne Quadrat)", maske_lesen() == 0u);
}

int main(void)
{
    printf("=== RIEGEL r31: Generator ROOM11F0 — Abnahme erst bei stehendem Zeiger ===\n");
    teil_a();
    teil_b();
    teil_c();
    teil_d();
    teil_e();
    teil_f();
    teil_g();
    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
