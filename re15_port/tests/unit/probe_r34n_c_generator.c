/*
 * probe_r34n_c_generator.c — RIEGEL Runde 34 Nacht, Spur C: Generator ROOM11F0/11F1.
 *
 * Nutzer 2026-09-30 (woertlich, AUFTRAG.md Punkt 3 und 4):
 *   "Bei den Generator in ROOM 11F0 moechte ich nicht, das nach den Klick eines Schalters,
 *    der Cursor eingefroren bleibt, bis die Anzeige dort steht wo sie hin soll, sondern man
 *    soll sich frei bewegen koennen Ausser ganz am Ende - ganz am Ende, bevor das "OK" kommt
 *    und die Lichter angehen, dann soll der Cursor zunaechst auf den finalen Wert - also die
 *    80 gehen."
 *   "Das obere Licht soll angehen, wenn links die 3 Schalter korrekt betaetigt sind. Das
 *    untere Licht soll angehen, wenn die 2 Schalter rechts korrekt betaetigt sind. Sobald
 *    eines der jeweiligen Schalter der jeweiligen Seite nicht mehr korrekt ist, dann soll das
 *    jeweilige Licht ... wieder aus gehen."
 *
 * SOLL (Belege: include/re15_panel_zeiger.h, analysis/befunde_runde34_nacht/C_generator.md):
 *   A  FREIE FAHRT: Schalter 7 per echter Cursorfahrt + Quadrat, danach UNTEN gehalten: der
 *      Cursor bewegt sich in JEDEM Bild der Zeigerfahrt (0 -> 20) und der 30 Ruhebilder,
 *      keine Sperre; der Zeiger erreicht trotzdem 20.
 *   B  ENDSPERRE: 7, 9, 3, 1 ohne Warten, dann 5 (alles echter Eingabepfad) -> vor dem Bild
 *      b* (Maske = Loesung 0x155) nie gesperrt; ab b* bis zur Abnahme gesperrt, Cursor steht
 *      trotz HOCH+QUADRAT, kein Schalterwechsel; Abnahme (4:238) genau im Bild k+31
 *      (k = Zeiger auf 80; RE2 sleep 30 @0x0171C/@0x0171D).
 *   C  ZIELWECHSEL waehrend der Fahrt: keine Sperre, der Zeiger dreht vom aktuellen Wert
 *      (Schritt |1| je Bild, kein Sprung), keine Abnahme.
 *   D  DURCHFAHRT durch die 80 (60 -> 90 und 90 -> 60): nie gesperrt, keine Abnahme.
 *   E  LAMPEN-WAHRHEITSTAFEL ueber alle 1024 Masken gegen die ZEHN Ck-Bytes, die aus der RDT
 *      GELESEN werden (@0x012BE..0x012E2) — nicht gegen die Port-Konstanten; live im Tick
 *      dieselbe Tafel; 4:238 aendert nichts (keine Nach-Loesung-Regel).
 *   F  LAMPEN-ZEITLINIE (echter Eingabepfad): an im Bild des Schalterbits (nicht vorher),
 *      Zelle 3 im Einschaltbild, dann 4, 3, 4; aus im Bild eines falschen Bits; wieder an beim
 *      Zuruecknehmen (Zelle 3 neu) — kein Einrasten; die andere Lampe bleibt unberuehrt.
 *   G  ROOM11F1 (Elzas Variante) wie ROOM11F0.
 *   H  WIEDEREINTRITT nach der Loesung: keine Sperre, Lampen aus, und die Buehne ist nicht
 *      erreichbar (Slot 1 = Text, sub00 Else @0x0101A -> @0x0101E `2c 01 01 31`).
 *   I  ZEICHNER (platform/pc/src/panel_lampen_pc.c, echt einkompiliert): LAMPE2130.TIM liegt
 *      byte-gleich zu ROOM2130.RDT[0x0E398, +4256); gezeichnet wird nur im 22x22-Quadrat,
 *      additiv B+F mit Saettigung, Texel 0 durchsichtig — Soll unabhaengig aus den TIM-Bytes
 *      nachgerechnet; nur Raum 11F0/11F1 + Cut 10 + Lampe an.
 *
 * RUECKBAU-NACHWEIS: siehe C_generator.md §9.4 (in dieser Runde gefahren).
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
#include "re15_engine.h"
#include "re15_panel_zeiger.h"
#include "re15_audio.h"

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
extern unsigned g_re15_panel_bestaet_zaehler;

/* Zeichner aus platform/pc/src/panel_lampen_pc.c (in diese Sonde einkompiliert). */
extern void re15_panel_lampen_pc_zeichnen(void);
extern int  re15_panel_lampen_pc_dekodieren(const uint8_t *tim, int n, uint16_t *z3, uint16_t *z4);

/* SOLL-Zahlen als ZAHL mit Beleg (nicht aus dem Port-Kopf, sonst bestaetigte sich der Riegel
 * selbst — Lehre Runde 31): */
#define RE2_SLEEP_BILDER   30      /* RE2 ROOM2130.RDT sub04 @0x0171C `09` + @0x0171D `0a 1e 00` */
#define KIPP_BILDER        16      /* sub06 @0x01336 `0d 00 04 00 10 00` = For 16                */

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

/* ---- Stubs fuer den einkompilierten PC-Zeichner ------------------------------------ */
static uint32_t s_fb[SCREEN_XRES * SCREEN_YRES];
uint32_t *re15_pc_framebuffer(void) { return s_fb; }
uint8_t *re15_pc_read_re2(const char *rel, int *size)
{
    char p[700]; size_t n = 0;
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_RE2_DIR, rel);
    uint8_t *b = slurp(p, &n);
    if (b && size) *size = (int)n;
    return b;
}

/* Ein Spielbild in der Reihenfolge der PC-Hauptschleife: VM vor dem Spielschritt; der
 * Zeiger-/Lampen-Tick ist der letzte Zustands-Tick des Schritts (wie r31_generator.c). */
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

/* geloest = 1: 4:238 steht schon beim Raumaufbau (wie nach Laden/Wiedereintritt) — gesetzt
 * NACH scd_vm_init (das die Baenke leert) und VOR scd_room_reenter (sub00 liest es @0x00D60). */
static int room_boot_ex(unsigned rid, int geloest)
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
    if (geloest) re15_game_flag_set(4, 238, 1);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_panel_zeiger_reset();
    for (int f = 0; f < 30; f++) frame(0, 0);
    return 1;
}
static int room_boot(unsigned rid) { return room_boot_ex(rid, 0); }

/* Das Raetsel scharf schalten wie sub16 @Datei 0x015C2 ff. (13x `22 05 xx 01`). */
static void raetsel_scharf(void)
{
    for (int b = 0; b <= 12; b++) re15_game_flag_set(5, (uint8_t)b, 1);
}

/* Schaltermaske direkt setzen: Bit i = Schalter i+1 = Bank 5 Bit 13+i (@0x012BE..0x012E2). */
static void maske_setzen(unsigned m)
{
    for (int i = 0; i < RE15_PANEL_SCHALTER; i++)
        re15_game_flag_set(5, (uint8_t)(13 + i), (m >> i) & 1u);
}

static unsigned maske_lesen(void)
{
    unsigned m = 0;
    for (int i = 0; i < 10; i++) if (re15_game_flag_get(5, (uint8_t)(13 + i))) m |= 1u << i;
    return m;
}

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

/* Die Zellen der zehn Schalter, aus den Aot_set-Bytes (sub00 @0x00D78..0x00E2C): Slot n+1,
 * Rechteck (x, z) mit Kante 2050. Schalter 1..5: x -27300, 6..10: x -19700. */
static const int32_t ZELLE_X[10] = { -27300, -27300, -27300, -27300, -27300,
                                     -19700, -19700, -19700, -19700, -19700 };
static const int32_t ZELLE_Z[10] = { 25650, 23450, 21250, 19000, 16800,
                                     25650, 23450, 21250, 19000, 16800 };

/* Buchfuehrung der echten Laeufe: jedes Bild zaehlen, ob gesperrt war. */
static int s_bild = 0, s_sperr_bilder = 0;
static void frame_z(uint16_t held, uint16_t edge)
{
    frame(held, edge);
    s_bild++;
    if (re15_panel_zeiger_sperrt()) s_sperr_bilder++;
}

/* Cursor per D-Pad in die Mitte der Zelle von Schalter `nr` fahren (echter Eingabepfad:
 * sub02..05 Speed_set +-200, @0x012F6/0x01302/0x0130E/0x0131A). */
static int cursor_zu(int nr)
{
    int ci = cursor_prop(); if (ci < 0) return 0;
    int32_t zx = ZELLE_X[nr - 1] + 1025, zz = ZELLE_Z[nr - 1] + 1025;
    for (int f = 0; f < 200; f++) {
        int32_t dx = zx - g_scd.props[ci].x, dz = zz - g_scd.props[ci].z;
        uint16_t b = 0;
        if (dx > 150) b = RE15_PAD_BIT_RIGHT; else if (dx < -150) b = RE15_PAD_BIT_LEFT;
        else if (dz > 150) b = RE15_PAD_BIT_UP; else if (dz < -150) b = RE15_PAD_BIT_DOWN;
        if (!b) { frame_z(0, 0); frame_z(0, 0); return 1; }
        frame_z(b, b);
    }
    return 0;
}

/* Quadrat fuer EIN Bild (sub01 `51 01 40 00` liest das gehaltene Wort). */
static void quadrat(void)
{
    frame_z(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    frame_z(0, 0);
}

/* ================= A: freie Fahrt ================================================ */
static void teil_a(void)
{
    printf("\n=== A: nach einem Zwischenschalter frei (NUTZER-VORGABE; RE1.5 ohne Set(2,7,1) in sub01..17) ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: ROOM11F0.RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf(); maske_setzen(0); einschwingen();
    int ci = cursor_prop();
    CHECK("Cursor-Prop obj 0 vorhanden (@0x00E54)", ci >= 0);
    if (ci < 0) return;
    CHECK("Cursor erreicht die Zelle von Schalter 7 (Slot 8 @0x00DF0)", cursor_zu(7));
    quadrat();
    /* bis zum Schalterbit (Kippung 16 Bilder, dann @0x014C0 `22 05 13 01`) — dabei UNTEN halten */
    int b = -1, bilder = 0, steh = 0, gesperrt = 0, w20 = -1;
    int32_t zvor = g_scd.props[ci].z;
    for (int f = 0; f < KIPP_BILDER + 4 + 20 + RE2_SLEEP_BILDER + 5; f++) {
        frame_z(RE15_PAD_BIT_DOWN, (uint16_t)(f == 0 ? RE15_PAD_BIT_DOWN : 0));
        int32_t z = g_scd.props[ci].z;
        if (b < 0 && (maske_lesen() & 0x40u)) b = f;
        if (b >= 0 && f <= b + 20 + RE2_SLEEP_BILDER) {
            bilder++;
            if (z == zvor) steh++;
            if (re15_panel_zeiger_sperrt()) gesperrt++;
        }
        if (w20 < 0 && re15_panel_zeiger_wert() == 20) w20 = f;
        zvor = z;
    }
    printf("  Schalterbit im Bild %d, Zeiger 20 im Bild %d; %d Fahrt+Ruhe-Bilder: Cursor stand %d-mal, "
           "Sperre %d-mal; Maske jetzt %03X\n", b, w20, bilder, steh, gesperrt, maske_lesen());
    CHECK("Schalter 7 wurde umgelegt (Bank 5 Bit 19)", b >= 0);
    CHECK("der Zeiger erreicht trotzdem die 20 (20 Punkte, 1 je Bild @0x01216)", w20 == b + 19);
    CHECK("in JEDEM Bild der Fahrt und der 30 Ruhebilder bewegt sich der Cursor", bilder == 51 && steh == 0);
    CHECK("und nie eine Sperre", gesperrt == 0);
    CHECK("nur Schalter 7 umgelegt (UNTEN ohne Quadrat schaltet nichts)", maske_lesen() == 0x040u);
}

/* ================= B: Endsperre ================================================== */
static void teil_b(const char *raum, unsigned rid)
{
    printf("\n=== B (%s): Endsperre ab Maske = Loesung bis zur Abnahme (RE2 @0x01110..@0x01818) ===\n", raum);
    if (!room_boot(rid)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0); re15_game_flag_set(4, 243, 0);
    raetsel_scharf(); maske_setzen(0); einschwingen();
    int ci = cursor_prop(); if (ci < 0) { CHECK("Cursor-Prop", 0); return; }
    s_bild = 0; s_sperr_bilder = 0;
    static const int reihe[4] = { 7, 9, 3, 1 };
    for (int i = 0; i < 4; i++) { cursor_zu(reihe[i]); quadrat(); }   /* ohne auf den Zeiger zu warten */
    int vor_b = s_sperr_bilder, bild_vor = s_bild;
    cursor_zu(5); quadrat();
    /* bis b* (Maske = 0x155) ohne Taste */
    int bstern = -1, sperr_vor_bstern = s_sperr_bilder;
    for (int f = 0; f < 60 && bstern < 0; f++) {
        if (maske_lesen() == RE15_PANEL_LOESUNGSMASKE) { bstern = f; break; }
        sperr_vor_bstern = s_sperr_bilder;          /* Stand VOR diesem Bild */
        frame_z(0, 0);
        if (maske_lesen() == RE15_PANEL_LOESUNGSMASKE) bstern = f;
    }
    printf("  %s: %d Bilder freies Schalten (davon %d mit Sperre), b* nach %d Bildern, Zeiger dort %d\n",
           raum, s_bild, sperr_vor_bstern, bstern, re15_panel_zeiger_wert());
    CHECK("vor b* in KEINEM Bild eine Sperre (7, 9, 3, 1 frei, Zeiger faehrt dabei)",
          sperr_vor_bstern == 0 && vor_b == 0 && bild_vor > 0);
    CHECK("b* erreicht: Maske = Loesung 0x155", bstern >= 0);
    if (bstern < 0) return;
    CHECK("im Bild b* gesperrt (LIVE-Maske)", re15_panel_zeiger_sperrt() == 1);
    /* ab b*: HOCH + QUADRAT halten bis nach der Abnahme */
    int32_t x0 = g_scd.props[ci].x, z0 = g_scd.props[ci].z;
    unsigned t0 = g_re15_panel_bestaet_zaehler;
    int k = -1, ab = -1, ton = -1, sperr_luecke = 0, bewegt = 0, gewechselt = 0;
    for (int f = 1; f < 200; f++) {
        uint16_t bt = (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_SQUARE);
        frame(bt, (uint16_t)(f == 1 ? bt : 0));
        int g = re15_game_flag_get(4, 238);
        if (k < 0 && re15_panel_zeiger_wert() == RE15_PANEL_ZIEL) k = f;
        if (ab < 0 && g) ab = f;
        if (ton < 0 && g_re15_panel_bestaet_zaehler != t0) ton = f;
        if (ab < 0) {
            if (!re15_panel_zeiger_sperrt()) sperr_luecke++;
            if (maske_lesen() != RE15_PANEL_LOESUNGSMASKE) gewechselt++;
        }
        if (ab < 0 || f == ab) {
            if (g_scd.props[ci].x != x0 || g_scd.props[ci].z != z0) bewegt++;
        }
        if (ab >= 0 && f > ab + 3) break;
    }
    printf("  %s: Zeiger 80 im Bild b*+%d, Abnahme (4:238) b*+%d, Ton b*+%d; Sperrluecken %d, "
           "Cursor bewegt %d, Schalterwechsel %d\n", raum, k, ab, ton, sperr_luecke, bewegt, gewechselt);
    CHECK("die Sperre steht in JEDEM Bild von b* bis zur Abnahme", k >= 0 && ab > 0 && sperr_luecke == 0);
    CHECK("der Cursor steht trotz gehaltenem HOCH (bis einschliesslich Abnahmebild)", bewegt == 0);
    CHECK("gehaltenes Quadrat legt keinen Schalter um", gewechselt == 0);
    CHECK("Abnahme genau im Bild k+31 (RE2 sleep 30 @0x0171D)", ab == k + 1 + RE2_SLEEP_BILDER);
    CHECK("Bestaetigungston im Abnahmebild (RE2 @0x01762 hinter @0x0175E)", ton == ab);
    CHECK("nach der Abnahme keine Panel-Sperre mehr (4:238)", re15_panel_zeiger_sperrt() == 0);
    CHECK("beide Lampen gruen im Abnahmebild und danach (Bits 13..22 bleiben, sub18 @0x016FA..0x0172A)",
          re15_panel_lampen() == 3);
}

/* ================= C: Zielwechsel waehrend der Fahrt =============================== */
static void teil_c(void)
{
    printf("\n=== C: Ziel wechselt waehrend der Fahrt ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf(); maske_setzen(0); einschwingen();
    maske_setzen(1u << 8);                         /* Schalter 9: +30 */
    for (int f = 0; f < 25; f++) frame(0, 0);
    int w_vor = re15_panel_zeiger_wert();
    maske_setzen((1u << 8) | (1u << 2));           /* + Schalter 3: -10 -> Ziel 20 */
    int sprung = 0, gesperrt = 0, prev = w_vor, abwaerts = 0, ab = 0;
    for (int f = 0; f < 80; f++) {
        frame(0, 0);
        int w = re15_panel_zeiger_wert();
        if (w - prev > 1 || prev - w > 1) sprung++;
        if (w < prev) abwaerts++;
        if (re15_panel_zeiger_sperrt()) gesperrt++;
        if (re15_game_flag_get(4, 238)) ab++;
        prev = w;
    }
    printf("  Wert beim Wechsel %d, Ziel jetzt %d, Endwert %d; Abwaertsschritte %d, Spruenge %d, Sperre %d\n",
           w_vor, re15_panel_zeiger_ziel(), re15_panel_zeiger_wert(), abwaerts, sprung, gesperrt);
    CHECK("GEGENPROBE: der Zeiger war beim Wechsel ueber dem neuen Ziel (25 > 20)", w_vor == 25);
    CHECK("der Zeiger dreht vom aktuellen Wert: 5 Schritte abwaerts, kein Sprung", abwaerts == 5 && sprung == 0);
    CHECK("und steht auf 20", re15_panel_zeiger_wert() == 20);
    CHECK("keine Sperre, keine Abnahme", gesperrt == 0 && ab == 0);
}

/* ================= D: Durchfahrt durch die 80 ===================================== */
static void teil_d(void)
{
    printf("\n=== D: Durchfahrt durch die 80 (nur 0x155 zielt auf 80) ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf(); maske_setzen(0x145u); einschwingen();          /* 1,3,7,9 = 60 */
    int durch = 0, gesperrt = 0, ab = 0;
    maske_setzen(0x151u);                                            /* 1,5,7,9 = 90 */
    for (int f = 0; f < 80; f++) {
        frame(0, 0);
        if (re15_panel_zeiger_wert() == 80) durch |= 1;
        if (re15_panel_zeiger_sperrt()) gesperrt++;
        if (re15_game_flag_get(4, 238)) ab++;
    }
    int w90 = re15_panel_zeiger_wert();
    maske_setzen(0x145u);                                            /* zurueck auf 60 */
    for (int f = 0; f < 80; f++) {
        frame(0, 0);
        if (re15_panel_zeiger_wert() == 80) durch |= 2;
        if (re15_panel_zeiger_sperrt()) gesperrt++;
        if (re15_game_flag_get(4, 238)) ab++;
    }
    printf("  60 -> %d -> %d, durch 80 (Bits 1=auf 2=ab) %d, Sperre %d, Abnahme %d\n",
           w90, re15_panel_zeiger_wert(), durch, gesperrt, ab);
    CHECK("GEGENPROBE: in beiden Richtungen durch die 80 gefahren", durch == 3 && w90 == 90);
    CHECK("nie gesperrt, nie abgenommen", gesperrt == 0 && ab == 0);
}

/* ================= E: Lampen-Wahrheitstafel ======================================= */
static void teil_e(void)
{
    printf("\n=== E: Lampen-Wahrheitstafel aus den Ck-Bytes @0x012BE..0x012E2 ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    int sollwert[10], ok_bytes = 1;
    for (int i = 0; i < 10; i++) {
        const uint8_t *p = s_raw + 0x012BE + 4 * i;
        if (p[0] != 0x21 || p[1] != 0x05 || p[2] != (uint8_t)(13 + i) || p[3] > 1) ok_bytes = 0;
        sollwert[i] = p[3];
    }
    CHECK("zehn Ck(5, 13..22) in Reihe @0x012BE..0x012E2", ok_bytes);
    unsigned loesung = 0;
    for (int i = 0; i < 10; i++) if (sollwert[i]) loesung |= 1u << i;
    printf("  Loesung aus den Bytes: %03X\n", loesung);
    int fehl0 = 0, fehl1 = 0, beide = 0, beide_ok = 0, ziel80_ok = 0;
    for (unsigned m = 0; m < 1024; m++) {
        int o = 1, u = 1;
        for (int i = 0; i < 5; i++)  if ((int)((m >> i) & 1u) != sollwert[i]) o = 0;
        for (int i = 5; i < 10; i++) if ((int)((m >> i) & 1u) != sollwert[i]) u = 0;
        if (re15_panel_lampe_an_aus_maske(0, m) != o) fehl0++;
        if (re15_panel_lampe_an_aus_maske(1, m) != u) fehl1++;
        if (o && u) { beide++; if (m == loesung) beide_ok++; }
        if ((re15_panel_zeiger_ziel_aus_maske(m) == 80) == (o && u)) ziel80_ok++;
    }
    printf("  1024 Masken: Fehler oben %d, unten %d; beide an bei %d Maske(n); Ziel80<=>beide an: %d/1024\n",
           fehl0, fehl1, beide, ziel80_ok);
    CHECK("obere Lampe = linke Spalte (Schalter 1..5) in Loesungsstellung, alle 1024", fehl0 == 0);
    CHECK("untere Lampe = rechte Spalte (Schalter 6..10) in Loesungsstellung, alle 1024", fehl1 == 0);
    CHECK("beide Lampen genau bei der Loesungsmaske", beide == 1 && beide_ok == 1);
    CHECK("beide Lampen <=> Zeigerziel 80 (Nutzer-Gewichte)", ziel80_ok == 1024);
    /* LIVE im Tick: jede Maske ein Bild, 4:238 einmal 0 und einmal 1 */
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf();
    int live_fehl = 0;
    for (int geloest = 0; geloest < 2; geloest++) {
        re15_game_flag_set(4, 238, (uint8_t)geloest);
        for (unsigned m = 0; m < 1024; m++) {
            maske_setzen(m);
            frame(0, 0);
            int soll = re15_panel_lampe_an_aus_maske(0, m) | (re15_panel_lampe_an_aus_maske(1, m) << 1);
            if (re15_panel_lampen() != soll) live_fehl++;
        }
    }
    re15_game_flag_set(4, 238, 0);
    printf("  live (2 x 1024 Bilder, 4:238 = 0/1): %d Abweichungen\n", live_fehl);
    CHECK("live im Tick dieselbe Tafel, und 4:238 aendert nichts (keine Nach-Loesung-Regel)", live_fehl == 0);
}

/* ================= F: Lampen-Zeitlinie (echter Eingabepfad) ======================== */
static int zelle_jetzt(int nr)
{
    int x0 = 0, y0 = 0, k = 0, z = 0;
    g_re15_active_cut = RE15_PANEL_CUT;
    return re15_panel_lampe_sicht(nr, &x0, &y0, &k, &z) ? z : 0;
}

/* Schalter `nr` ueber den echten Pfad umlegen und das Bild melden, in dem sein Bit wechselt:
 * *vorher_an = Lampenstand im Bild davor, *an = im Bitbild, *z[0..3] = Zelle oben ab Bitbild. */
static int umlegen(int nr, int lampe, int *vor, int *im, int zl[4])
{
    unsigned bit = 1u << (nr - 1), m0;
    cursor_zu(nr);
    m0 = maske_lesen();
    frame_z(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    int vorher = (re15_panel_lampen() >> lampe) & 1;
    for (int f = 0; f < 40; f++) {
        vorher = (re15_panel_lampen() >> lampe) & 1;
        frame_z(0, 0);
        if ((maske_lesen() ^ m0) & bit) {
            *vor = vorher; *im = (re15_panel_lampen() >> lampe) & 1;
            zl[0] = zelle_jetzt(lampe);
            for (int j = 1; j < 4; j++) { frame_z(0, 0); zl[j] = zelle_jetzt(lampe); }
            return f;
        }
    }
    return -1;
}

static void teil_f(void)
{
    printf("\n=== F: Lampen-Zeitlinie (Bit-Bild, Zelle 3/4, kein Einrasten) ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf(); maske_setzen(0x005u); einschwingen();        /* 1,3 an: oben noch aus */
    CHECK("Start (1,3 an): beide Lampen aus", re15_panel_lampen() == 0);
    int vor = -1, im = -1, zl[4] = { 0, 0, 0, 0 };
    int f5 = umlegen(5, 0, &vor, &im, zl);
    printf("  Schalter 5 EIN: Bit nach %d Bildern, oben vorher %d / im Bitbild %d, Zellen %d %d %d %d, unten %d\n",
           f5, vor, im, zl[0], zl[1], zl[2], zl[3], (re15_panel_lampen() >> 1) & 1);
    CHECK("obere Lampe geht im Bild des Schalterbits an, nicht frueher", f5 >= 0 && vor == 0 && im == 1);
    CHECK("Zelle 3 im Einschaltbild, dann 4, 3, 4 (RE2 Saetze 4/5, je Bild)",
          zl[0] == 3 && zl[1] == 4 && zl[2] == 3 && zl[3] == 4);
    CHECK("untere Lampe bleibt aus (rechte Spalte unberuehrt)", ((re15_panel_lampen() >> 1) & 1) == 0);
    int f2 = umlegen(2, 0, &vor, &im, zl);
    printf("  Schalter 2 EIN (falsch): Bit nach %d Bildern, oben vorher %d / im Bitbild %d\n", f2, vor, im);
    CHECK("falscher Schalter links: obere Lampe AUS im Bitbild", f2 >= 0 && vor == 1 && im == 0);
    int f2b = umlegen(2, 0, &vor, &im, zl);
    printf("  Schalter 2 wieder AUS: Bit nach %d Bildern, oben vorher %d / im Bitbild %d, Zelle %d\n",
           f2b, vor, im, zl[0]);
    CHECK("zurueckgenommen: wieder AN, Zelle 3 neu (kein Einrasten)", f2b >= 0 && vor == 0 && im == 1 && zl[0] == 3);
    /* rechte Spalte: 7 und 9 -> unten an; 8 dazu -> aus */
    int f7 = umlegen(7, 1, &vor, &im, zl);
    int f9 = umlegen(9, 1, &vor, &im, zl);
    printf("  Schalter 7, 9: Bits nach %d/%d Bildern, unten vorher %d / im Bitbild %d\n", f7, f9, vor, im);
    CHECK("rechts 7 + 9: untere Lampe an im Bitbild von 9", f7 >= 0 && f9 >= 0 && vor == 0 && im == 1);
    CHECK("jetzt beide an", re15_panel_lampen() == 3);
    /* die Loesung ist erreicht -> Endsperre; zum Weitertesten Raetsel neu (Maske direkt) */
    maske_setzen(0x145u | 0x010u | 0x080u);     /* 1,3,5,7,9 + 8 = rechts falsch */
    frame(0, 0);
    CHECK("falscher Schalter rechts (8): untere Lampe aus, obere bleibt an", re15_panel_lampen() == 1);
    /* Sichtbarkeit */
    maske_setzen(0x155u); frame(0, 0);
    int x0 = 0, y0 = 0, k = 0, z = 0;
    g_re15_active_cut = RE15_PANEL_CUT;
    int s0 = re15_panel_lampe_sicht(0, &x0, &y0, &k, &z);
    printf("  sicht oben: %d Ecke (%d,%d) Kante %d Zelle %d\n", s0, x0, y0, k, z);
    CHECK("oben: Ecke (212,65), Kante 22 (Glasmitte 222.5/75.5, ROOM11F10.bmp)", s0 && x0 == 212 && y0 == 65 && k == 22);
    int s1 = re15_panel_lampe_sicht(1, &x0, &y0, &k, &z);
    CHECK("unten: Ecke (212,125), Kante 22 (Glasmitte 222.5/135.5)", s1 && x0 == 212 && y0 == 125 && k == 22);
    g_re15_active_cut = 8;
    CHECK("anderer Cut (8): nicht sichtbar", !re15_panel_lampe_sicht(0, NULL, NULL, NULL, NULL));
    g_re15_active_cut = RE15_PANEL_CUT;
    re15_game_flag_set(4, 238, 0);
}

/* ================= H: Wiedereintritt nach dem Loesen ============================= */
static void teil_h(void)
{
    printf("\n=== H: Wiedereintritt nach dem Loesen ===\n");
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    int typ_vor = g_aot.slots[1].active ? g_aot.slots[1].type : -1;
    if (!room_boot_ex(0x11F0, 1)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    int typ_nach = g_aot.slots[1].active ? g_aot.slots[1].type : -1;
    printf("  Slot 1 vor der Loesung Typ %d, danach Typ %d; Maske %03X, Lampen %d, Sperre %d, Zeiger %d\n",
           typ_vor, typ_nach, maske_lesen(), re15_panel_lampen(), re15_panel_zeiger_sperrt(),
           re15_panel_zeiger_wert());
    CHECK("Buehne nach der Loesung nicht erreichbar: Slot 1 = Text (sce 1 @0x0101E), vorher Ereignis",
          typ_nach == RE15_AOT_TYPE_MESSAGE && typ_vor != typ_nach);
    CHECK("Schalterbits beim Raumaufbau geloescht (@0x8003ed74) -> Lampen aus", maske_lesen() == 0 && re15_panel_lampen() == 0);
    CHECK("keine Sperre, Zeiger auf 80", re15_panel_zeiger_sperrt() == 0 && re15_panel_zeiger_wert() == 80);
    raetsel_scharf(); maske_setzen(0x155u); frame(0, 0);
    CHECK("Robustheit: selbst mit Loesungsmaske sperrt nach 4:238 nichts", re15_panel_zeiger_sperrt() == 0);
    re15_game_flag_set(4, 238, 0);
}

/* ================= G: ROOM11F1 ================================================== */
static void teil_g(void)
{
    printf("\n=== G: ROOM11F1 (Elzas Variante, byte-identisch) ===\n");
    teil_b("ROOM11F1", 0x11F1);
    if (!room_boot(0x11F1)) { printf("FAIL: RDT fehlt\n"); g_fail++; return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf(); maske_setzen(0x015u); frame(0, 0);
    g_re15_active_cut = RE15_PANEL_CUT;
    CHECK("ROOM11F1: obere Lampe an und sichtbar", re15_panel_lampen() == 1 &&
          re15_panel_lampe_sicht(0, NULL, NULL, NULL, NULL) == 1);
    maske_setzen(0x040u); frame(0, 0);
    CHECK("ROOM11F1: Zwischenschalter sperrt nicht", re15_panel_zeiger_sperrt() == 0);
    g_current_room_id = 0x1170;
    g_re15_active_cut = RE15_PANEL_CUT;
    CHECK("anderer Raum (1170): Lampe nie sichtbar", re15_panel_lampe_sicht(0, NULL, NULL, NULL, NULL) == 0);
    g_current_room_id = 0x11F1;
}

/* ================= I: Zeichner + Asset ============================================ */
static void teil_i(void)
{
    printf("\n=== I: Zeichner panel_lampen_pc.c + LAMPE2130.TIM ===\n");
    size_t nt = 0, nr = 0;
    char p[700];
    snprintf(p, sizeof p, "%s/LAMPE2130.TIM", RE15_ASSET_RE2_DIR);
    uint8_t *tim = slurp(p, &nt);
    snprintf(p, sizeof p, "%s/ROOM2130.RDT", RE15_RE2_RDT_DIR);
    uint8_t *rdt = slurp(p, &nr);
    CHECK("shared_assets/RE2/LAMPE2130.TIM vorhanden, 4256 B", tim && nt == 4256);
    CHECK("info/re2leon/PL0/RDT/ROOM2130.RDT vorhanden", rdt != NULL);
    if (!tim || !rdt || nt != 4256) { free(tim); free(rdt); return; }
    /* ESP-TIM-Lage wie FUN_8001bd38 (@0x8001bd64..0x8001bd7c): basis = Kopfwort[20], erster
     * Eintrag der abwaerts gelesenen Tabelle bis Kopfwort[21]. */
    uint32_t w20 = (uint32_t)rdt[8 + 80] | ((uint32_t)rdt[8 + 81] << 8) | ((uint32_t)rdt[8 + 82] << 16) | ((uint32_t)rdt[8 + 83] << 24);
    uint32_t w21 = (uint32_t)rdt[8 + 84] | ((uint32_t)rdt[8 + 85] << 8) | ((uint32_t)rdt[8 + 86] << 16) | ((uint32_t)rdt[8 + 87] << 24);
    uint32_t e = (uint32_t)rdt[w21 - 4] | ((uint32_t)rdt[w21 - 3] << 8) | ((uint32_t)rdt[w21 - 2] << 16) | ((uint32_t)rdt[w21 - 1] << 24);
    uint32_t a = w20 + e;
    printf("  ROOM2130: Kopfwort[20] 0x%05X, Tabelle bis 0x%05X -> ESP-TIM @0x%05X\n", w20, w21, a);
    CHECK("ESP-TIM liegt @0x0E398 (Kopfwort[20] + erster Eintrag)", a == 0x0E398u);
    CHECK("LAMPE2130.TIM byte-gleich ROOM2130.RDT[0x0E398, +4256)",
          a + 4256u <= nr && memcmp(tim, rdt + a, 4256) == 0);
    /* Unabhaengige Soll-Dekodierung direkt aus den TIM-Bytes (nicht ueber den Zeichner). */
    static uint16_t soll[2][32 * 32];
    const uint8_t *clut2 = tim + 20 + 2 * 16 * 2;          /* CLUT-Zeile 2 (Unterindex 0x10 >> 3) */
    const uint8_t *pix = tim + 8 + 140 + 12;               /* CLUT-Block 12 + 16*4*2 = 140 B */
    int belegt[2] = { 0, 0 }, bit15 = 1;
    for (int z = 0; z < 2; z++)
        for (int v = 0; v < 32; v++)
            for (int u = 0; u < 32; u++) {
                int uu = (z ? 128 : 96) + u;
                int idx = (pix[v * 128 + uu / 2] >> (4 * (uu & 1))) & 15;
                uint16_t c = (uint16_t)(clut2[idx * 2] | (clut2[idx * 2 + 1] << 8));
                soll[z][v * 32 + u] = c;
                if (c) { belegt[z]++; if (!(c & 0x8000u)) bit15 = 0; }
            }
    printf("  Zelle 3: %d, Zelle 4: %d belegte Texel\n", belegt[0], belegt[1]);
    CHECK("Zellen 3/4 belegt (851 / 832 von 1024), alle Nicht-Null-Farben mit Bit 15",
          belegt[0] == 851 && belegt[1] == 832 && bit15);
    uint16_t d3[32 * 32], d4[32 * 32];
    CHECK("Zeichner dekodiert dieselben Farben",
          re15_panel_lampen_pc_dekodieren(tim, (int)nt, d3, d4) == 0 &&
          memcmp(d3, soll[0], sizeof d3) == 0 && memcmp(d4, soll[1], sizeof d4) == 0);

    /* Zeichnen im Raum: obere Lampe an (Maske 0x015), Cut 10. */
    if (!room_boot(0x11F0)) { printf("FAIL: RDT fehlt\n"); g_fail++; free(tim); free(rdt); return; }
    re15_game_flag_set(4, 238, 0);
    raetsel_scharf(); maske_setzen(0x015u); frame(0, 0);
    g_re15_active_cut = RE15_PANEL_CUT;
    int zl = 0; re15_panel_lampe_sicht(0, NULL, NULL, NULL, &zl);
    const uint32_t BG = (0x30u << 24) | (0x2cu << 16) | (0x20u << 8) | 0xffu;
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) s_fb[i] = BG;
    re15_panel_lampen_pc_zeichnen();
    int aussen = 0, innen_soll = 0, innen_ist = 0, falsch = 0, unten = 0;
    const uint16_t *zs = soll[zl == 3 ? 0 : 1];
    for (int y = 0; y < SCREEN_YRES; y++)
        for (int x = 0; x < SCREEN_XRES; x++) {
            uint32_t v = s_fb[y * SCREEN_XRES + x];
            int drin = (x >= 212 && x < 234 && y >= 65 && y < 87);
            if (!drin) { if (v != BG) { aussen++; if (y >= 125) unten++; } continue; }
            uint16_t t = zs[((y - 65) * 32 / 22) * 32 + (x - 212) * 32 / 22];
            uint32_t erw = BG;
            if (t) {
                unsigned r = 0x30 + ((t & 31u) << 3), g = 0x2c + (((t >> 5) & 31u) << 3),
                         b = 0x20 + (((t >> 10) & 31u) << 3);
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;
                erw = (r << 24) | (g << 16) | (b << 8) | 0xffu;
                innen_soll++;
            }
            if (v != BG) innen_ist++;
            if (v != erw) falsch++;
        }
    printf("  obere Lampe (Zelle %d): innen %d/%d Pixel geaendert, %d Abweichungen vom Soll, aussen %d (davon unten %d)\n",
           zl, innen_ist, innen_soll, falsch, aussen, unten);
    CHECK("gezeichnet nur im 22x22-Quadrat ab (212,65); untere Lampe (aus) unberuehrt", aussen == 0);
    CHECK("jedes Pixel = Hintergrund + Texel<<3 (B+F, gesaettigt), Texel 0 durchsichtig",
          falsch == 0 && innen_ist == innen_soll && innen_soll > 200);
    /* Saettigung */
    const uint32_t HELL = (0xf0u << 24) | (0xf0u << 16) | (0xf0u << 8) | 0xffu;
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) s_fb[i] = HELL;
    re15_panel_lampen_pc_zeichnen();
    int ueber = 0, voll = 0;
    for (int y = 65; y < 87; y++)
        for (int x = 212; x < 234; x++) {
            uint32_t v = s_fb[y * SCREEN_XRES + x];
            if (v != HELL) { voll++; if (((v >> 16) & 0xffu) != 0xffu) ueber++; }
        }
    CHECK("heller Hintergrund: Gruen saettigt bei 255 (kein Ueberlauf)", voll > 0 && ueber == 0);
    /* nicht Cut 10 / anderer Raum -> nichts */
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) s_fb[i] = BG;
    g_re15_active_cut = 8;
    re15_panel_lampen_pc_zeichnen();
    int diff = 0;
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) if (s_fb[i] != BG) diff++;
    CHECK("Cut 8 (nach der Abnahme): kein Pixel gezeichnet", diff == 0);
    g_re15_active_cut = RE15_PANEL_CUT;
    maske_setzen(0x155u); frame(0, 0); g_re15_active_cut = RE15_PANEL_CUT;
    re15_panel_lampen_pc_zeichnen();
    int oben_n = 0, unten_n = 0;
    for (int y = 0; y < SCREEN_YRES; y++)
        for (int x = 0; x < SCREEN_XRES; x++)
            if (s_fb[y * SCREEN_XRES + x] != BG) { if (y < 100) oben_n++; else unten_n++; }
    CHECK("Loesung: beide Lampen gezeichnet", oben_n > 200 && unten_n > 200);
    free(tim); free(rdt);
}

int main(void)
{
    printf("=== RIEGEL r34n C: Generator ROOM11F0/11F1 — frei, Endsperre bis 80, zwei gruene Lampen ===\n");
    teil_a();
    teil_b("ROOM11F0", 0x11F0);
    teil_c();
    teil_d();
    teil_e();
    teil_f();
    teil_g();
    teil_h();
    teil_i();
    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
