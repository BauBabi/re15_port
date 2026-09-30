/* ============================================================================================
 * probe_r34_plattform — Runde 34 (Granaten), Spur C (Plattform): was ohne Fenster pruefbar ist.
 * Dossier: analysis/befunde_runde34_granaten/bau_c.md; Code: platform/pc/src/fx_plattform_pc.c.
 *
 * Rueckgabe 0 = gruen, sonst die NUMMER der ersten verletzten Pruefung (Muster probe_r30_granate).
 * Die Sollwerte stehen hier als LITERALE aus dem Disassembly bzw. aus Datei-Bytes — NICHT ueber die
 * Makros des Produktionscodes —, damit eine verstellte Konstante dort die Sonde rot macht
 * (Mutationsproben, Dossier §Sonde).
 *
 *   T  C1  Takt: re15_pc_fx_takt tickt genau einmal je Freigabe (ESP-Tick, dann RE2-FX-Tick);
 *          ohne Freigabe / unter RE15_PAUSE_ACTION kein Fortschritt
 *   H  C4  Haken-Bindung: vorher NULL, nachher die vier Vertragsziele
 *   W  C4  Ton-Weiche: FUN_80045024-Bank/Satz/Tore; RE2-Codes 0x0113/0x0112 -> ARMS10/11 Satz 10;
 *          EDH-Bytes der Saetze aus den Dateien
 *   S  C2  TEX.TIM-Effektseiten: Kopf, Paletten 481/483/492, Seitenpixel == die gepinnten
 *          file-route-Blaetter effect8_fire.tim / effect0_blood.tim, Negativ-Kontrollen
 *   Z  C2  Zeichen-Helfer: Sichtbarkeit Bit0&Bit1, Weltlage wpos/Rueckfall, CLUT/TPAGE-Saat,
 *          defW/defH aus der Zeile (Huelse Halte-Zeile w/h 1)
 *   L  C3  Licht-Latch: Rechnung gegen K2 LICHT_LESER, einmal anwenden, bitgleich zurueck, Latch 0
 *   A  C8  Harness-Parse RE15_FORCE_AUFSCHLAG
 *   F  C7  RE2-Part-Tinte fuer RE2-KI-Typen ausserhalb der Zombie-Gore-Bruecke
 * ============================================================================================ */
#include "fx_plattform_pc.h"
#include "re15_esp.h"
#include "re15_scd.h"
#include "re15_audio.h"
#include "re15_damage.h"
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"
#include "re2_fx.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* test_support.c (Spione) */
extern int g_test_core_se_last, g_test_core_se_count;
extern int g_test_snd0_se_last, g_test_snd0_se_count;
extern int g_test_room_se_n, g_test_room_se_log[];

/* Spion fuer die Zusatzbank (die echte Funktion steht in audio_pc.c, das die Sonde nicht linkt). */
static int s_zusatz_id = -1, s_zusatz_satz = -1, s_zusatz_n = 0;
void re15_audio_arms_zusatz_se(int arms_id, int satz)
{
    s_zusatz_id = arms_id; s_zusatz_satz = satz; s_zusatz_n++;
}

static int s_fail = 0, s_pass = 0;
#define CHECK(nr, cond, ...) do {                                               \
        if (cond) { s_pass++; printf("  PASS %3d: ", nr); printf(__VA_ARGS__); printf("\n"); } \
        else { printf("  FAIL %3d: ", nr); printf(__VA_ARGS__); printf("\n");   \
               if (!s_fail) s_fail = nr; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b && n) *n = (size_t)sz; return b;
}

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

/* ------------------------------------------------------------------------------------------ */
static re15_esp_t s_core;
static uint8_t   *s_core_buf = NULL;

static int lade_core(void)
{
    size_t n = 0;
    s_core_buf = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &n);
    if (!s_core_buf) return -1;
    if (re15_esp_parse_global(s_core_buf, n, &s_core) != 0) return -2;
    re15_esp_set_global_bank(&s_core);
    re15_esp_set_room_bank(NULL);
    return 0;
}

/* erster aktiver Platz mit (id, sub) */
static const re15_esp_fx_t *finde(uint8_t id, uint8_t sub)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->effect_id == id && f->sub_index == sub) return f;
    }
    return NULL;
}

/* ===== T — C1 Takt ======================================================================== */
static void teil_T(void)
{
    printf("\n[T] C1 Takt (re15_pc_fx_takt)\n");
    re15_esp_fx_reset();
    g_re15_pauseflags = 0;
    /* Muendung Id 2 sub 0 (Stroeme: Zeile 0 A 8 Flags 0x93 TPAGE|0x20 — CORE00.ESP, Dossier §C2) */
    int n = re15_esp_fx_spawn_rows(&s_core, 2, 0, 0x800, 1000, -1000, 2000, 0, 0);
    const re15_esp_fx_t *f = finde(2, 0);
    CHECK(1, n >= 1 && f && f->flags == 0x03, "Spawn Muendung 2/0: %d Stroeme, Flags 0x%02x (Spawner 0x03)",
          n, f ? f->flags : 0xff);
    /* Negativ: ohne Freigabe kein Takt */
    re15_pc_fx_takt_setzen(0);
    int lief = re15_pc_fx_takt();
    CHECK(2, lief == 0 && f && f->flags == 0x03, "ohne Freigabe: kein Takt (lief=%d, Flags 0x%02x)", lief,
          f ? f->flags : 0xff);
    /* Freigabe -> genau ein Tick: Routine 8 (@0x800175ec) setzt Flags := row[0x0e] = 0x93 */
    re15_pc_fx_takt_setzen(1);
    CHECK(3, re15_pc_fx_takt_frei() == 1, "Freigabe steht");
    lief = re15_pc_fx_takt();
    CHECK(4, lief == 1 && f && f->flags == 0x93, "freigegeben: Takt lief, Routine 8 -> Flags 0x%02x (Soll 0x93)",
          f ? f->flags : 0xff);
    CHECK(5, re15_pc_fx_takt_frei() == 0 && re15_pc_fx_takt() == 0, "Freigabe verbraucht: zweiter Aufruf tickt nicht");
    /* RE15_PAUSE_ACTION (0x10000000, Selbst-Gate @0x80019e40): Takt laeuft, Tick bleibt stehen */
    re15_esp_fx_reset();
    re15_esp_fx_spawn_rows(&s_core, 2, 0, 0x800, 1000, -1000, 2000, 0, 0);
    f = finde(2, 0);
    g_re15_pauseflags = RE15_PAUSE_ACTION;
    re15_pc_fx_takt_setzen(1);
    lief = re15_pc_fx_takt();
    CHECK(6, lief == 1 && f && f->flags == 0x03, "RE15_PAUSE_ACTION: Takt verbraucht, Platz unveraendert (0x%02x)",
          f ? f->flags : 0xff);
    g_re15_pauseflags = 0;
    re15_esp_fx_reset();
}

/* ===== H — C4 Haken-Bindung ================================================================ */
static void teil_H(void)
{
    printf("\n[H] C4 Haken-Bindung\n");
    CHECK(10, re15_esp_se_hook == NULL && re15_esp_aufschlag_hook == NULL &&
              re2fx_se_hook == NULL && re2fx_applier == NULL,
          "vor der Bindung alle vier Haken NULL (C0-Vertrag)");
    re15_pc_r34_haken_binden();
    CHECK(11, re15_esp_se_hook == re15_pc_esp_se, "re15_esp_se_hook -> re15_pc_esp_se");
    CHECK(12, re15_esp_aufschlag_hook == re2fx_aufschlag, "re15_esp_aufschlag_hook -> re2fx_aufschlag (V1d/E8)");
    CHECK(13, re2fx_se_hook == re15_pc_re2fx_se, "re2fx_se_hook -> re15_pc_re2fx_se (E9)");
    CHECK(14, re2fx_applier == re15_re2_gl_apply, "re2fx_applier -> re15_re2_gl_apply (V2b)");
}

/* ===== W — C4 Ton-Weiche ==================================================================== */
static void teil_W(void)
{
    printf("\n[W] C4 Ton-Weiche (FUN_80045024 / FUN_8005ba28)\n");
    int satz = -1;
    /* Abprall 0x010A0001 | (n<<8) @0x80018410-28: Bank 1, Satz 10, Byte1 wirkungslos */
    int k = re15_pc_esp_se_weiche(0x010A0601u, &satz);
    CHECK(20, k == RE15_SE_BANK_WEAPON && satz == 10, "0x010A0601 -> ARMS Satz 10 (Byte1 0x06 ignoriert): k=%d satz=%d", k, satz);
    k = re15_pc_esp_se_weiche(0x010A0001u, &satz);
    CHECK(21, k == RE15_SE_BANK_WEAPON && satz == 10, "0x010A0001 (Liegen) -> ARMS Satz 10");
    k = re15_pc_esp_se_weiche(0x04080001u, &satz);
    CHECK(22, k == RE15_SE_BANK_CORE && satz == 8, "0x04080001 (Explosion @0x800185e4-ec) -> CORE Satz 8: k=%d satz=%d", k, satz);
    CHECK(23, re15_pc_esp_se_weiche(0x00080001u, &satz) == 0, "Bank 0 (0x801fdd00, nicht resident) -> verworfen");
    CHECK(24, re15_pc_esp_se_weiche(0x06080001u, &satz) == 0, "Bank 6 -> verworfen (Tor sltiu 0x6 @0x80045094)");
    CHECK(25, re15_pc_esp_se_weiche(0x01210001u, &satz) == 0 &&
              re15_pc_esp_se_weiche(0x01200001u, &satz) == RE15_SE_BANK_WEAPON,
          "Satz-Tor 0x21 (@0x800450d0): Satz 0x21 verworfen, 0x20 gespielt");
    CHECK(26, re15_pc_esp_se_weiche(0x03190001u, &satz) == 0 &&
              re15_pc_esp_se_weiche(0x03180001u, &satz) == RE15_SE_BANK_SND1,
          "snd1-Tor 0x19 (@0x800450f8): 0x19 verworfen, 0x18 gespielt");
    CHECK(27, re15_pc_esp_se_weiche(0x05030001u, &satz) == RE15_SE_BANK_SND0, "Bank 5 = snd0 (@0x8004513c)");
    /* Aufruf durch den Haken: CORE-Spion aus test_support */
    int c0 = g_test_core_se_count;
    re15_esp_se_hook(0x04080001u, NULL);
    CHECK(28, g_test_core_se_count == c0 + 1 && g_test_core_se_last == 8, "Haken spielt CORE Satz 8 (Spion: %d)",
          g_test_core_se_last);
    int s0 = g_test_snd0_se_count;
    re15_esp_se_hook(0x00080001u, NULL);
    CHECK(29, g_test_core_se_count == c0 + 1 && g_test_snd0_se_count == s0, "Bank 0: kein Aufruf");
    /* RE2-FX-Codes (E9) */
    int arms = -1;
    CHECK(30, re15_pc_re2fx_se_weiche(0x01130001u, &arms, &satz) == 1 && arms == 0x10 && satz == 10,
          "0x01130001 (Op 49 @0x80021678-7c) -> ARMS10 Satz 10");
    CHECK(31, re15_pc_re2fx_se_weiche(0x01120001u, &arms, &satz) == 1 && arms == 0x11 && satz == 10,
          "0x01120001 (Op 48 @0x80020fd4/@0x80021028) -> ARMS11 Satz 10");
    CHECK(32, re15_pc_re2fx_se_weiche(0x01140001u, &arms, &satz) == 0 &&
              re15_pc_re2fx_se_weiche(0x01130101u, &arms, &satz) == 0,
          "andere Codes -> stumm (keine Raterei)");
    s_zusatz_n = 0;
    re2fx_se_hook(0x01130001u, NULL);
    CHECK(33, s_zusatz_n == 1 && s_zusatz_id == 0x10 && s_zusatz_satz == 10, "Haken: Zusatzbank ARMS10 Satz 10");
    re2fx_se_hook(0x01120001u, NULL);
    CHECK(34, s_zusatz_n == 2 && s_zusatz_id == 0x11 && s_zusatz_satz == 10, "Haken: Zusatzbank ARMS11 Satz 10");
    /* EDH-Bytes (Datei): die Saetze, auf die die Weiche zeigt */
    size_t n = 0;
    uint8_t *e10 = slurp(RE15_ASSET_PSX_DIR "/SOUND/ARMS10.EDH", &n);
    uint8_t *e11 = slurp(RE15_ASSET_PSX_DIR "/SOUND/ARMS11.EDH", &n);
    uint8_t *e09 = slurp(RE15_ASSET_PSX_DIR "/SOUND/ARMS09.EDH", &n);
    uint8_t *c00 = slurp(RE15_ASSET_PSX_DIR "/SOUND/CORE00.EDH", &n);
    static const uint8_t k3320[4] = { 0x00, 0x00, 0x33, 0x20 };
    static const uint8_t k1310[4] = { 0x00, 0x00, 0x13, 0x10 };
    static const uint8_t k9300[4] = { 0x00, 0x00, 0x93, 0x00 };
    CHECK(35, e10 && e11 && !memcmp(e10 + 0x28, k3320, 4) && !memcmp(e11 + 0x28, k3320, 4),
          "ARMS10/11.EDH @0x28 (Satz 10) = 00 00 33 20 (Prog 0 Ton 3)");
    CHECK(36, e09 && !memcmp(e09 + 0x28, k1310, 4), "ARMS09.EDH @0x28 (Satz 0x0A, Abprall) = 00 00 13 10");
    CHECK(37, c00 && !memcmp(c00 + 0x20, k9300, 4), "CORE00.EDH @0x20 (Satz 8, Explosion) = 00 00 93 00");
    free(e10); free(e11); free(e09); free(c00);
}

/* ===== S — C2 TEX.TIM-Seiten ================================================================ */
static void teil_S(void)
{
    printf("\n[S] C2 TEX.TIM-Effektseiten\n");
    size_t n = 0;
    uint8_t *tex = slurp(RE15_ASSET_PSX_DIR "/DATA/TEX.TIM", &n);
    CHECK(40, tex != NULL && n == 0x620 + 320 * 256 * 2, "DATA/TEX.TIM geladen (%zu B; Soll 0x620 + 320*256*2)", n);
    if (!tex) return;
    static re15_pc_fx_seite_t s1e, s1f;
    re15_tim_t t1e, t1f;
    int r1e = re15_pc_fx_seite_bauen(tex, n, 0x001e, &s1e, &t1e);
    int r1f = re15_pc_fx_seite_bauen(tex, n, 0x001f, &s1f, &t1f);
    CHECK(41, r1e == 0 && r1f == 0, "Seiten 0x1e / 0x1f gebaut (rc %d / %d)", r1e, r1f);
    CHECK(42, t1e.bpp == 4 && t1e.width == 256 && t1e.height == 256 && t1e.clut_x == 272 && t1e.clut_y == 480 &&
              t1e.clut_entries == 16 * 16 && t1e.data_x == 896 && t1f.data_x == 960 && t1e.data_y == 256,
          "TIM-Beschreibung: 4 bpp 256x256, CLUT (272,480) 16 Zeilen, Seite x 896/960 y 256");
    /* Paletten gegen K3 (TEX.TIM-Bytes): 481 Rauch-Kind, 483 Feuerball, 492 Granate */
    const uint16_t *p481 = &s1e.clut[1 * 16], *p483 = &s1e.clut[3 * 16], *p492 = &s1e.clut[12 * 16];
    CHECK(43, p481[0] == 0x0000 && p481[1] == 0x9ce7 && p481[2] == 0x98c6 && p481[3] == 0x94a5,
          "Zeile 481 = 0000 9ce7 98c6 94a5 (K3 CLUT_RAUCH, TEX.TIM @0x074)");
    CHECK(44, p483[0] == 0x0000 && p483[1] == 0xffff && p483[2] == 0xebde && p483[3] == 0xd7de,
          "Zeile 483 = 0000 ffff ebde d7de (K3 CLUT_FEUERBALL, TEX.TIM @0x0F4)");
    CHECK(45, p492[0] == 0x0000 && p492[1] == 0xb631 && p492[2] == 0xb1ef && p492[3] == 0xadce,
          "Zeile 492 = 0000 b631 b1ef adce (K3 CLUT_GRANATE, TEX.TIM @0x334)");
    CHECK(46, rd16(tex + 0x334) == 0x0000 && rd16(tex + 0x336) == 0xb631 && rd16(tex + 0x0F6) == 0xffff &&
              rd16(tex + 0x076) == 0x9ce7,
          "Gegenprobe Datei-Offsets @0x334 / @0x0F4 / @0x074");
    /* Seitenpixel == die gepinnten Blaetter des Datei-Wegs (test_1090_fire_pin: effect8_fire.tim) */
    uint8_t *fire = slurp(RE15_ASSET_PSX_DIR "/../extracted_fx/effect8_fire.tim", NULL);
    uint8_t *blut = slurp(RE15_ASSET_PSX_DIR "/../extracted_fx/effect0_blood.tim", NULL);
    int gleich_fire = 0, gleich_blut = 0;
    if (fire) { const uint8_t *px = fire + 8 + 12 + 32 + 12;   /* Kopf, CLUT-Block 12+16*2, Bild-Kopf */
                gleich_fire = !memcmp(px, s1e.pix, 64 * 256 * 2); }
    if (blut) { const uint8_t *px = blut + 8 + 12 + 32 + 12;
                gleich_blut = !memcmp(px, s1f.pix, 64 * 256 * 2); }
    CHECK(47, gleich_fire, "Seite 0x1e == effect8_fire.tim-Pixel (32768 Halbworte, file-route-Pin)");
    CHECK(48, gleich_blut, "Seite 0x1f == effect0_blood.tim-Pixel (32768 Halbworte)");
    CHECK(49, fire && !memcmp(fire + 8 + 12, &s1e.clut[4 * 16], 32) && blut && !memcmp(blut + 8 + 12, &s1f.clut[5 * 16], 32),
          "Paletten 484 (Feuer) / 485 (Blut) == die CLUT der beiden Blaetter");
    free(fire); free(blut);
    /* Negativ-Kontrollen */
    re15_tim_t tx;
    CHECK(50, re15_pc_fx_seite_bauen(tex, n, 0x000e, &s1e, &tx) < 0, "tpage 0x0e (y 0) -> abgelehnt");
    CHECK(51, re15_pc_fx_seite_bauen(tex, n, 0x001d, &s1e, &tx) < 0, "tpage 0x1d (VRAM 832, ausserhalb des Messrechtecks) -> abgelehnt");
    uint8_t kaputt[64]; memcpy(kaputt, tex, 64); kaputt[0] = 0x11;
    CHECK(52, re15_pc_fx_seite_bauen(kaputt, 64, 0x001e, &s1e, &tx) < 0, "falsche Magic -> abgelehnt");
    CHECK(53, re15_pc_fx_seite_clut_ok(0x7B11) && re15_pc_fx_seite_clut_ok(0x7811) && re15_pc_fx_seite_clut_ok(0x7BD1),
          "CLUT-Worte 0x7B11 (492) / 0x7811 (480) / 0x7BD1 (495) in der Spanne");
    CHECK(54, !re15_pc_fx_seite_clut_ok(0x7C11) && !re15_pc_fx_seite_clut_ok(0x77D1) && !re15_pc_fx_seite_clut_ok(0x7B10),
          "0x7C11 (496) / 0x77D1 (479) / 0x7B10 (x 256) ausserhalb");
    free(tex);
}

/* ===== Z — C2 Zeichen-Helfer ================================================================ */
static void teil_Z(void)
{
    printf("\n[Z] C2 Zeichen-Helfer\n");
    re15_esp_fx_reset();
    re15_esp_fx_t f;
    memset(&f, 0, sizeof f);
    uint8_t zeile[40] = {0};
    f.rows_base = zeile; f.row_count = 1;
    f.flags = 0x03; CHECK(60, re15_pc_esp_sichtbar(&f) == 1, "Flags 0x03 sichtbar");
    f.flags = 0x13; CHECK(61, re15_pc_esp_sichtbar(&f) == 1, "Flags 0x13 sichtbar");
    f.flags = 0x0a; CHECK(62, re15_pc_esp_sichtbar(&f) == 0, "Flags 0x0a (Bit0 fehlt, Kind vor der Init) unsichtbar (@0x800532fc)");
    f.flags = 0x61; CHECK(63, re15_pc_esp_sichtbar(&f) == 0, "Flags 0x61 (Bit1 fehlt) unsichtbar (@0x80053308)");
    f.rows_base = NULL; f.flags = 0;
    CHECK(64, re15_pc_esp_sichtbar(&f) == 1, "Altpfad ohne Row-VM: sichtbar wie bisher");
    /* Weltlage */
    int32_t w[3];
    f.x = 100; f.y = -200; f.z = 300; f.xlat_x = 10; f.xlat_y = 20; f.xlat_z = 30;
    re15_pc_esp_weltlage(&f, w);
    CHECK(65, w[0] == 110 && w[1] == -180 && w[2] == 330, "wpos 0 -> Rueckfall x+xlat (%d,%d,%d)", w[0], w[1], w[2]);
    f.wpos[0] = -6851; f.wpos[1] = 12; f.wpos[2] = -18279;
    re15_pc_esp_weltlage(&f, w);
    CHECK(66, w[0] == -6851 && w[1] == 12 && w[2] == -18279, "wpos gesetzt -> slot+0x28 (%d,%d,%d)", w[0], w[1], w[2]);
    /* CLUT/TPAGE-Saat: Granate Id 4 sub 0x0D (Zeile Row-VM) und Feuerball Id 3 sub 0x19 (Altpfad) */
    re15_esp_fx_spawn_rows(&s_core, 4, 0x0D, 0x1000, 0, 0, 0, 0, 0);
    const re15_esp_fx_t *g = finde(4, 0x0D);
    CHECK(67, g && re15_pc_esp_clut(g) == 0x7B11 && re15_pc_esp_tpage(g) == 0x001f,
          "Granate 4/0x0D: CLUT 0x%04x (Soll 0x7B11 = 0x7AD1 + 1*0x40), TPAGE 0x%04x (Soll 0x001f)",
          g ? re15_pc_esp_clut(g) : 0, g ? re15_pc_esp_tpage(g) : 0);
    re15_esp_fx_t *fb = re15_esp_fx_spawn_ex(&s_core, 3, 0x19, 0x5000, 0, 0, 0, 0);
    CHECK(68, fb && !fb->rows_base && re15_pc_esp_clut(fb) == 0x78D1 && re15_pc_esp_tpage(fb) == 0x001e,
          "Feuerball 3/0x19 (Altpfad): CLUT 0x%04x (Soll 0x78D1 = 0x7811 + 3*0x40), TPAGE 0x%04x",
          fb ? re15_pc_esp_clut(fb) : 0, fb ? re15_pc_esp_tpage(fb) : 0);
    /* defW/defH: Huelse Id 4 sub 0, Zeile 0 = Routine 16, w/h 1 (Flags 0x63 sichtbar) */
    re15_esp_fx_reset();
    re15_esp_fx_spawn_rows(&s_core, 4, 0, 0x1000, 0, 0, 0, 0, 0);
    const re15_esp_fx_t *h = finde(4, 0);
    int32_t dw = -1, dh = -1;
    if (h) re15_pc_esp_defwh(h, &dw, &dh);
    CHECK(69, h && dw == 1 && dh == 1, "Huelse 4/0 Zeile 0: defW/defH = %d/%d (Soll 1/1, CORE00.ESP-Zeile)", dw, dh);
    re15_esp_fx_reset();
    re15_esp_fx_spawn_rows(&s_core, 2, 3, 0x1000, 0, 0, 0, 0, 0);
    const re15_esp_fx_t *m3 = finde(2, 3);
    if (m3) re15_pc_esp_defwh(m3, &dw, &dh);
    CHECK(70, m3 && dw == 0x1320 && dh == 0x1320, "Muendung 2/3: defW/defH = 0x%x (Soll 0x1320)", dw);
    memset(&f, 0, sizeof f);
    re15_pc_esp_defwh(&f, &dw, &dh);
    CHECK(71, dw == 0x1000 && dh == 0x1000, "Altpfad: defW/defH 0x1000");
    re15_esp_fx_reset();
}

/* ===== L — C3 Licht-Latch =================================================================== */
static void teil_L(void)
{
    printf("\n[L] C3 Licht-Latch (K2 LICHT_LESER)\n");
    re15_light_cut_t c;
    memset(&c, 0, sizeof c);
    c.type_flags[2] = 1;
    c.colors[2][0] = 0x10; c.colors[2][1] = 0xf0; c.colors[2][2] = 0x20;
    c.positions[2][0] = 111; c.positions[2][1] = 222; c.positions[2][2] = 333;
    c.brightness[2] = 42;
    c.colors[0][0] = 0x77; c.brightness[0] = 1234;       /* Licht 0/1 bleiben */
    re15_pc_licht_latch_rechnen(&c, -7600, 0, -17600, 0);
    CHECK(80, c.type_flags[2] == 0, "Typ Licht 2 := 0 (@0x8001cef8)");
    CHECK(81, c.colors[2][0] == 0xd2 && c.colors[2][1] == 0xf0 && c.colors[2][2] == 0x50,
          "Farbe = max(alt, D2/8C/50): (%02x,%02x,%02x) Soll (d2,f0,50) (@0x8001cf20-ac)",
          c.colors[2][0], c.colors[2][1], c.colors[2][2]);
    CHECK(82, c.positions[2][0] == -6400 && c.positions[2][1] == -800 && c.positions[2][2] == -17600,
          "Gier 0: Lage (%d,%d,%d) Soll (-6400,-800,-17600) = Leon + (1200,-800,0)",
          c.positions[2][0], c.positions[2][1], c.positions[2][2]);
    CHECK(83, c.brightness[2] == 0x1770, "Helligkeit 0x1770 (@0x8001d080)");
    CHECK(84, c.colors[0][0] == 0x77 && c.brightness[0] == 1234, "Licht 0 unberuehrt");
    /* Gier 1024 (90 Grad): x' = cos*1200 >> 12, z' = -sin*1200 >> 12 (RotMatrix @0x80068098) */
    int32_t cs = re15_cos_q12(1024), sn = re15_sin_q12(1024);
    int32_t ex = (int16_t)(uint16_t)((uint16_t)100 + (uint16_t)((cs * 1200) >> 12));
    int32_t ez = (int16_t)(uint16_t)((uint16_t)200 + (uint16_t)((-sn * 1200) >> 12));
    re15_pc_licht_latch_rechnen(&c, 100, 50, 200, 1024);
    CHECK(85, c.positions[2][0] == ex && c.positions[2][2] == ez && c.positions[2][1] == -750,
          "Gier 1024: Lage (%d,%d,%d) Soll (%d,-750,%d)", c.positions[2][0], c.positions[2][1],
          c.positions[2][2], ex, ez);
    CHECK(86, ez == 200 - 1200 && ex == 100, "Gier 1024 zeigt nach -z (cos 0, sin 4096 in der Spieltabelle)");
    /* anwenden / zurueck */
    re15_light_set_t ls;
    memset(&ls, 0, sizeof ls);
    ls.cut_count = 3;
    for (int i = 0; i < 3; i++) { ls.cuts[i].colors[2][0] = (uint8_t)(0x30 + i); ls.cuts[i].type_flags[2] = 5; }
    re15_light_set_t orig = ls;
    g_re15_licht_latch = 0;
    int a = re15_pc_licht_latch_anwenden(&ls, 1, -7600, 0, -17600, 0);
    CHECK(87, a == 0 && !memcmp(&ls, &orig, sizeof ls), "Latch 0: nichts umgestellt");
    g_re15_licht_latch = 1;
    a = re15_pc_licht_latch_anwenden(&ls, 1, -7600, 0, -17600, 0);
    CHECK(88, a == 1 && ls.cuts[1].type_flags[2] == 0 && ls.cuts[1].colors[2][0] == 0xd2 &&
              !memcmp(&ls.cuts[0], &orig.cuts[0], sizeof ls.cuts[0]) &&
              !memcmp(&ls.cuts[2], &orig.cuts[2], sizeof ls.cuts[2]),
          "Latch 1: NUR der aktive Cut 1 umgestellt");
    re15_pc_licht_latch_zurueck(&ls);
    CHECK(89, !memcmp(&ls, &orig, sizeof ls) && g_re15_licht_latch == 0,
          "zurueck: Satz bitgleich (@0x8001d1ac), Latch 0 (@0x8001d1b4)");
    g_re15_licht_latch = 1;
    a = re15_pc_licht_latch_anwenden(&ls, 7, 0, 0, 0, 0);   /* Cut ausserhalb */
    re15_pc_licht_latch_zurueck(&ls);
    CHECK(90, a == 0 && !memcmp(&ls, &orig, sizeof ls) && g_re15_licht_latch == 0,
          "ungueltiger Cut: nichts umgestellt, Latch trotzdem geloescht");
}

/* ===== A — C8 Harness-Parse ================================================================= */
static void teil_A(void)
{
    printf("\n[A] C8 RE15_FORCE_AUFSCHLAG-Parse\n");
    int art = 0; int32_t q[3] = {0, 0, 0};
    int r = re15_pc_force_aufschlag_eintrag("2@400,1@500", 400, -7600, 0, -17600, 0, &art, q);
    CHECK(100, r == 1 && art == 2 && q[0] == -7600 + 1500 && q[1] == 0 && q[2] == -17600,
          "2@400: Saeure, q (%d,%d,%d) = 1500 vor Leon", q[0], q[1], q[2]);
    r = re15_pc_force_aufschlag_eintrag("2@400,1@500", 500, 0, -10, 0, 1024, &art, q);
    CHECK(101, r == 1 && art == 1 && q[0] == 0 && q[1] == -10 && q[2] == -1500, "1@500 bei Gier 1024: Brand, q (%d,%d,%d)",
          q[0], q[1], q[2]);
    CHECK(102, re15_pc_force_aufschlag_eintrag("2@400,1@500", 401, 0, 0, 0, 0, &art, q) == 0, "Bild 401: kein Eintrag");
    CHECK(103, re15_pc_force_aufschlag_eintrag("3@400", 400, 0, 0, 0, 0, &art, q) == 0 &&
               re15_pc_force_aufschlag_eintrag("x@400", 400, 0, 0, 0, 0, &art, q) == 0 &&
               re15_pc_force_aufschlag_eintrag(NULL, 400, 0, 0, 0, 0, &art, q) == 0,
          "Art 3 / Unsinn / NULL -> nichts");
}

/* ===== F — C7 Part-Tinte ==================================================================== */
static void teil_F(void)
{
    printf("\n[F] C7 RE2-Part-Farbwort (+0x70)\n");
    re15_actor_t e;
    memset(&e, 0, sizeof e);
    e.type = 0x20;
    uint32_t t[32];
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    e.re2z_part_tint[3] = 0x00202020u;
    int r = re15_pc_re2_part_tint(&e, 17, 1, t, 32);
    CHECK(110, r == 0 && t[3] == 0x00808080u, "RE1.5-KI-Hund: keine Tinte (neutral)");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    r = re15_pc_re2_part_tint(&e, 17, 1, t, 32);
    CHECK(111, r == 1 && t[3] == 0x00202020u && t[0] == 0x00808080u && t[16] == 0x00808080u,
          "RE2-KI-Hund: Part 3 = 0x%08x (Soll 0x00202020, Verkohlung H_TOD_BRAND), ungesetzte Parts neutral",
          t[3]);
    memset(e.re2z_part_tint, 0, sizeof e.re2z_part_tint);
    for (int i = 0; i < 16; i++) e.re2z_part_tint[i] = 0x00808080u;
    r = re15_pc_re2_part_tint(&e, 17, 1, t, 32);
    CHECK(112, r == 0, "alle Parts neutral 0x808080 -> kein Tint-Pfad (Renderpfad bitgleich)");
    e.re2z_part_tint[5] = 0x00003F2Fu;
    r = re15_pc_re2_part_tint(&e, 4, 1, t, 32);
    CHECK(113, r == 0 && t[5] == 0x00808080u, "n = 4 Bones: Part 5 liegt ausserhalb -> neutral");
    r = re15_pc_re2_part_tint(&e, 17, 1, t, 32);
    CHECK(114, r == 1 && t[5] == 0x00003F2Fu, "Part 5 = 0x00003F2F (H_TOD_SAEURE)");

    /* Integration W6: alle 20 Part-Woerter (re2z_part_tint[20], Spur B) und Part -> Bone der
     * RE1.5-Bank. Hund-Tod Brand faerbt 17 Parts (EMD0G_MOD0 `sw a0,112(v0)` @0x801047f4 /
     * `sltiu v0,s0,0x11` @0x801047f8) mit 0x00202020, Spinnen-Tod 20 Parts (EMS25 FUN_8010609C
     * `sw a1,112(v0)` @0x801060b0 / `sltiu v0,a2,0x14` @0x801060b4) mit 0x00202F2F. */
    memset(e.re2z_part_tint, 0, sizeof e.re2z_part_tint);
    for (int i = 0; i < 17; i++) e.re2z_part_tint[i] = 0x00202020u;
    r = re15_pc_re2_part_tint(&e, 17, 1, t, 32);
    CHECK(115, r == 1 && t[16] == 0x00202020u && t[7] == 0x00202020u && t[17] == 0x00808080u,
          "RE2-Rig Hund: Part 16 -> Bone 16 = 0x%08x (Soll 0x00202020; vorher Grenze 16 -> neutral), "
          "Bone 17 ausserhalb neutral", t[16]);
    /* RE1.5-Rig (Rueckfall ohne RE2-Archiv): Part p -> Bone k_perm_dog[p] (re2_ems.c
     * { 0,1,2,3,4,5,6,-1, 7, 8,-1, 9,10,11,12,13,14 }), die Pfoten-Parts 7/10 haben keinen Bone. */
    for (int i = 0; i < 17; i++) e.re2z_part_tint[i] = 0x00808080u;
    e.re2z_part_tint[8]  = 0x00202020u;     /* Part 8 -> Bone 7  */
    e.re2z_part_tint[7]  = 0x00003F2Fu;     /* Part 7 -> -1      */
    e.re2z_part_tint[16] = 0x00101F3Fu;     /* Part 16 -> Bone 14 */
    r = re15_pc_re2_part_tint(&e, 15, 0, t, 32);
    CHECK(116, r == 1 && t[7] == 0x00202020u && t[14] == 0x00101F3Fu && t[8] == 0x00808080u &&
               t[15] == 0x00808080u && t[16] == 0x00808080u,
          "RE1.5-Rig Hund: Bone 7 = 0x%08x (Part 8), Bone 14 = 0x%08x (Part 16), Part 7 (Pfote) faellt weg",
          t[7], t[14]);
    r = re15_pc_re2_part_tint(&e, 15, 1, t, 32);
    CHECK(117, t[7] == 0x00003F2Fu && t[8] == 0x00202020u,
          "NEGATIV-KONTROLLE: dieselben Woerter als RE2-Rig gelesen landen auf Bone 7 = Part 7 (0x%08x)", t[7]);
    e.type = 0x25;                          /* Spinne: 20 Parts, Identitaet (k_perm_ident) */
    memset(e.re2z_part_tint, 0, sizeof e.re2z_part_tint);
    for (int i = 0; i < 20; i++) e.re2z_part_tint[i] = 0x00202F2Fu;
    r = re15_pc_re2_part_tint(&e, 20, 1, t, 32);
    CHECK(118, r == 1 && t[19] == 0x00202F2Fu && t[16] == 0x00202F2Fu && t[20] == 0x00808080u,
          "RE2-Rig Spinne: Part 19 -> Bone 19 = 0x%08x (Soll 0x00202F2F)", t[19]);
    r = re15_pc_re2_part_tint(&e, 20, 0, t, 32);
    CHECK(119, r == 1 && t[19] == 0x00202F2Fu,
          "RE1.5-Rig Spinne (Identitaet): Part 19 -> Bone 19 = 0x%08x", t[19]);
    e.type = 0x20;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
}

int main(void)
{
    printf("=== probe_r34_plattform (Runde 34, Spur C) ===\n");
    int rc = lade_core();
    CHECK(0 + 200, rc == 0, "CORE00.ESP (RE1.5 DATA) geladen + geparst (rc %d)", rc);
    if (rc != 0) { printf("ABBRUCH\n"); return 200; }
    teil_H();          /* zuerst: prueft den Anfangszustand NULL */
    teil_T();
    teil_W();
    teil_S();
    teil_Z();
    teil_L();
    teil_A();
    teil_F();
    printf("\n=== %s: %d bestanden, erste Verletzung %d ===\n", s_fail ? "ROT" : "GRUEN", s_pass, s_fail);
    return s_fail;
}
