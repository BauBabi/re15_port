/* test_r41_annette.c — Runde 41 Spur AN: Annette-Szene ROOM5090 (include/re15_annette_r41.h,
 * Dossier analysis/befunde_runde41/AN_annette.md).
 *
 *   bedingungen  Spawn der Frau im Kittel: 5090 nur bei (3,89)=1, (9,133)=1 und (3,42)=0 (PORT-WAHL), 6030 nach dem Original
 *                (sub07 @0x010F8 spawnt bei (4,23)=1 nur, solange (3,89)=0) - beide Raeume mit dem echten RDT-Skript.
 *   programm     Bytecode aus Original-Opcodes, Ablauf der RE2-Szene ROOM6141 sub02: Schnittfolge 15/11/13 (NB3),
 *                RE2-Sleeps, Texte 43/40/42 mit Wortlaut, 41 ohne Text, Sturz-Werte = ROOM5080 sub03, Ende sub04.
 *   programm_zug 6030-Zugstart: jedes Original-Segment bytegleich mit ROOM6030, Texte 27-30, Tuer-Programm.
 *   modell       ANNETTE_4D*.EMD: Gerippe/Clips = RE1.5-0x4D-Blob dir[0..6] bytegleich, MD1 + TIM = RE2 kind 0x47 / 0x49.
 *   szene        ganzer Ablauf mit Riesenform- und Annette-Bank ueber VM + KI-Takt: Warten (Form unsichtbar) -> Sturz
 *                (y -5600, 14 Bilder) -> Drohen -> Hieb (Treffer im Bild 14 von Clip 4, Hand nahe) -> Reaktion -> Schnitt
 *                weg, liegt (RE2-Versatz), Schrei nur Ton -> Kampfstart sub04 (Form frei), einmalig.
 *   zugstart     6030: Wagentuer nach 5090 = msg 10 und Ausgang = Tuer (main00 @0x00C72) bis zum Zugstart, danach
 *                Ausgang = msg 20 (sub06 @0x01098); Fuehrerstand: Zugstart einmal, Toene, Abgang.
 *   Blut (NB2)   RE2-Emitter FUN_80104C24: 15 Bilder ab dem Trefferbild je 2 Aufrufe 0/1/0x2000 an den Klauen 14/28,
 *                Plaetze = Zeilen der Bank, Lage = Klaue im Spawnbild, Lebensdauer der Wolke gemessen.
 *   NB3          Schnittfolge 12/15/11/13 (Sturz, Bruellen, Satz, Hieb in Cut 11), Sleeps PAL * 6/5, Hieb-Takt (Treffer 42 Bilder
 *                nach dem Signal, Schnitt weg 34 danach), Annette sackt im Trefferbild mit Clip 6 zusammen, Untertitel-Halt
 *                35/118/60, Leiche beim Wiedereintritt (3,42)=1, 6030-Abgang in Cut 0 und versteckt mit dem Schnitt auf Cut 1.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_msg.h"
#include "re15_room.h"
#include "re15_birkin_formen.h"
#include "re15_annette_r41.h"
#include "re15_esp.h"
#include "re2_ems.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

static re15_rdt_t s_rdt;
static uint8_t *raum_laden(const char *rel, unsigned id, int cut)
{
    size_t sz = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *buf = slurp(p, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) return NULL;
    extern void scd_register_current_rdt(const re15_rdt_t *);
    scd_register_current_rdt(NULL);
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    /* Flags ueberleben scd_vm_init nicht: sichern und zuruecksetzen wie ein Spielstand */
    int f89 = re15_game_flag_get(3, 89), f42 = re15_game_flag_get(3, 42), f423 = re15_game_flag_get(4, 23);
    int f133 = re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT);
    scd_vm_init();
    re15_game_flag_set(3, 89, f89); re15_game_flag_set(3, 42, f42); re15_game_flag_set(4, 23, f423);
    re15_game_flag_set(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT, f133);
    re15_annette_r41_install((uint16_t)id);
    /* Raum-Init wie FUN_8003ef6c: RDT angemeldet (Gosub/Evt_exec aus sub00, ROOM6030 sub00 `18 07`), aber ohne den
     * sub01-Reseed des Spieltakts (scd_vm_set_room_init, scd_vm.c). */
    scd_register_current_rdt(&s_rdt);
    extern void scd_vm_set_room_init(int on);
    scd_vm_set_room_init(1);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = (int16_t)cut;
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_vm_set_room_init(0);
    return buf;
}

static int slot_vom_typ(uint8_t t)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == t) return s;
    return -1;
}

/* NB4 N2: Liegepose wie RE2 ROOM6141 (@0x01162 Pos_set y 900, @0x0116A Dir_set 3300, @0x01172 Plc_motion Clip 0x18), Lage =
 * Stand S (3700,-22500) + RE2-Versatz 1215 weg / 245 quer von L (-1378,-23395) = (4854,-22047), dann 700 nach Sueden
 * (Wandfreiheit Regal SCA #28) = (4854,-22747). Gier = Peilung L -> (4854,-22047) + (3300 - Peilung RE2 William -> Liegestelle);
 * mit den Python-Winkeln des Werkzeugs n4/leiche_lage.py 1170 (Toleranz 24 fuer die BIOS-Arkustangens-Tabelle). */
static void leiche_pruefen(const re15_actor_t *a, const char *wo)
{
    fprintf(stderr, "%s: Leiche (%d,%d,%d) Gier %d rot_x %d rot_z %d Clip %d/%d hp %d\n", wo, (int)a->x, (int)a->y, (int)a->z,
            a->rot_y, a->rot_x, a->rot_z, a->motion, (int)a->anim_frame, a->hp);
    CHECK(abs((int)a->x - 4854) <= 2 && abs((int)a->z - (-22747)) <= 2, "%s: Leiche bei (%d,%d) statt (4854,-22747)", wo,
          (int)a->x, (int)a->z);
    const int dg = (((int)a->rot_y - 1170) + 2048 + 4096) % 4096 - 2048;
    CHECK(abs(dg) <= 24, "%s: Leiche Gier %d statt ~1170 (RE2-Lage relativ zu William)", wo, a->rot_y);
    CHECK(a->y == 900 && a->rot_x == 0 && a->rot_z == 0 && a->motion == 24 && a->anim_frame == 0 && a->hp == -1,
          "%s: Leiche nicht RE2-liegend (y %d rot %d/%d Clip %d/%d hp %d; Soll y 900, ungekippt, Clip 24 Bild 0, hp -1)", wo,
          (int)a->y, a->rot_x, a->rot_z, a->motion, (int)a->anim_frame, a->hp);
}

static void flags(int f89, int f42, int f423)
{
    re15_game_flag_set(3, 89, f89); re15_game_flag_set(3, 42, f42); re15_game_flag_set(4, 23, f423);
}
static void zug(int v) { re15_game_flag_set(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT, v); }

static void test_bedingungen(void)
{
    /* 5090: Annette nur bei Tor (3,89), Zugstart gesehen (9,133) und noch nicht begonnenem Kampf (3,42); NB3 M8: bei (3,42)=1
     * (Szene gelaufen) liegt sie als Leiche (RE2 MAIN sub00 else-Zweig @0x00D16) */
    static const struct { int f89, z, f42, soll; } k5090[] = {
        { 0, 0, 0, 0 }, { 1, 0, 0, 0 }, { 1, 1, 0, 1 }, { 1, 1, 1, 1 }, { 0, 1, 0, 0 }, { 0, 1, 1, 0 } };
    for (unsigned i = 0; i < sizeof k5090 / sizeof k5090[0]; i++) {
        flags(k5090[i].f89, k5090[i].f42, 1); zug(k5090[i].z);
        CHECK(re15_annette_r41_spawn_weg(0x5090u, 0x4Du) == !k5090[i].soll, "spawn_weg 5090 (3,89)=%d (9,133)=%d (3,42)=%d",
              k5090[i].f89, k5090[i].z, k5090[i].f42);
        uint8_t *b = raum_laden("STAGE5/ROOM5090.RDT", 0x5090u, 12);
        CHECK(b != NULL, "ROOM5090 nicht ladbar");
        const int s = slot_vom_typ(0x4Du);
        CHECK((s > 0) == (k5090[i].soll != 0), "5090 (3,89)=%d (9,133)=%d (3,42)=%d: Frau im Kittel %s erwartet, Slot %d",
              k5090[i].f89, k5090[i].z, k5090[i].f42, k5090[i].soll ? "" : "nicht", s);
        if (s > 0 && !k5090[i].f42)
            CHECK(g_actors[s].x == 600 && g_actors[s].z == -25300, "5090 Lage (%d,%d) statt Spawn @0x0122A (600,-25300)",
                  (int)g_actors[s].x, (int)g_actors[s].z);
        if (s > 0 && k5090[i].f42) {
            /* Leiche: erster Takt legt sie an die Lage aus @0x01162 (Stand S (3700,-22500), 1215 von L weg, 245 quer) */
            re15_annette_r41_reset();
            re15_annette_r41_tick();
            const re15_actor_t *a = &g_actors[s];
            fprintf(stderr, "Wiedereintritt: Leiche (%d,%d,%d) Gier %d rot_z %d Clip %d/%d hp %d Phase %d\n", (int)a->x, (int)a->y,
                    (int)a->z, a->rot_y, a->rot_z, a->motion, (int)a->anim_frame, a->hp, re15_annette_r41_phase());
            leiche_pruefen(a, "Wiedereintritt");
            CHECK(re15_annette_r41_phase() == RE15_AN_RUHE, "Wiedereintritt: Phase %d statt RUHE", re15_annette_r41_phase());
            CHECK(re15_annette_r41_besitzt(s), "Leiche: Besitz");
            CHECK(re15_annette_r41_kein_ziel(a) && a->hp == -1, "Leiche beim Wiedereintritt ist Schuss-Kandidat (hp %d)", a->hp);
            CHECK(strstr(re15_annette_r41_emd_datei(0x4Du), "BLUT") != NULL, "Leiche ohne Blut-Kittel");
            re15_annette_r41_reset();
        }
        CHECK(slot_vom_typ(0x30u) > 0 || k5090[i].f42, "5090: kein 0x30 (Riesenform) bei (3,42)=0");
        free(b);
    }
    zug(0);
    /* Andere Raeume und Typen bleiben unberuehrt */
    flags(0, 0, 1);
    CHECK(!re15_annette_r41_spawn_weg(0x5091u, 0x4Du) && !re15_annette_r41_spawn_weg(0x6030u, 0x4Du) &&
          !re15_annette_r41_spawn_weg(0x5090u, 0x30u), "spawn_weg greift ausserhalb 5090/0x4D");
    /* 6030 Fuehrerstand vor dem Tor (Eintritt Cut 0 -> sub00 Case 0 -> sub07): Original-Verhalten, kein Port-Eingriff */
    flags(0, 0, 1);
    re15_annette_r41_reset();
    uint8_t *b = raum_laden("STAGE6/ROOM6030.RDT", 0x6030u, 0);
    CHECK(b && slot_vom_typ(0x4Du) > 0, "6030 vor dem Tor: Frau am Pult fehlt (sub07 @0x010F8)");
    if (slot_vom_typ(0x4Du) > 0)
        CHECK(g_actors[slot_vom_typ(0x4Du)].x == -1790 && g_actors[slot_vom_typ(0x4Du)].z == -22272, "6030 Lage");
    CHECK(re15_annette_r41_zug_szene() == 0, "6030 vor dem Tor: Zugstart darf nicht laufen");
    free(b);
    /* Elza 5091: keine Frau im Kittel auf der Disc */
    flags(1, 0, 1); zug(1);
    b = raum_laden("STAGE5/ROOM5091.RDT", 0x5091u, 12);
    CHECK(b && slot_vom_typ(0x4Du) < 0, "5091 hat eine 0x4D");
    free(b);
    zug(0);
}

/* Laenge eines Opcodes (scd_vm.c s_opcode_sizes, exportiert fuer r21_discard). */
extern int scd_opcode_size_at(const uint8_t *pc);

static uint8_t *s_rdt5080, *s_rdt6030;
static size_t s_n5080, s_n6030;

static void test_programm(void)
{
    int n = 0;
    const uint8_t *p = re15_annette_r41_programm(&n);
    CHECK(p && n > 20, "Programm fehlt");
    if (!p) return;
    CHECK(p[0] == 0x22 && p[1] == 2 && p[2] == 7 && p[3] == 1 && p[4] == 0x22 && p[5] == 1 && p[6] == 27 && p[7] == 1,
          "Programmkopf: Sperre/Balken wie sub04 @0x012F2/@0x012F6");
    CHECK(n >= 6 && p[n - 6] == 0x04 && p[n - 5] == 0xff && p[n - 4] == 0x18 && p[n - 3] == 4 && p[n - 2] == 0x01,
          "Programmende: Evt_exec sub04 + Evt_end");
    /* Opcode-Gang: Schnitte, Sleeps, Message_on in der Reihenfolge der RE2-Szene (ROOM6141 sub02 @0x01024..) */
    int cuts[16], nc = 0, sl[32], ns = 0, ms[8], nm = 0, sturz = -1, ende = 0;
    for (int i = 0; i < n;) {
        const int len = scd_opcode_size_at(p + i);
        if (len <= 0 || i + len > n) { CHECK(0, "Programm: Opcode 0x%02X @%d ohne Laenge", p[i], i); break; }
        if (p[i] == 0x29 && nc < 16) cuts[nc++] = p[i + 1];
        if (p[i] == 0x09 && ns < 32) sl[ns++] = p[i + 2] | p[i + 3] << 8;
        if (p[i] == 0x2b && nm < 8) ms[nm++] = p[i + 1];
        if (p[i] == 0x32 && sturz < 0) sturz = i;
        if (p[i] == 0x01) ende = (i + len == n);
        i += len;
    }
    CHECK(ende, "Programm endet nicht genau mit Evt_end");
    static const int k_cuts[] = { 15, 11, 13 };    /* RE2 Cut 2, 0x0A (NB3: auch 0x0B und der Hieb in Cut 11), 6 */
    CHECK(nc == 3 && memcmp(cuts, k_cuts, sizeof k_cuts) == 0, "Schnittfolge (%d Schnitte) nicht 15/11/13", nc);
    /* RE2-Sleeps @0x0104C 25, @0x01072 36, @0x01076 1, @0x010FE 30, @0x01110 60, @0x01116 1, @0x0114C 1 als PAL-Spielbilder
     * (25/s, VSync(2) @0x8002ade8) -> Port-Bilder * 6/5 (NB3 M7): 30, 43, 1, 36, 72, 1, 1; Xa-Laengen in Sekunden: 0x30 35,
     * 30+Xa 0x31 = 118, 0x32 42; Hieb-Schnitt 13 + 14 * 2 + 34 = 75 (RE2-Aufnahme 69,77/70,22/71,09/72,23 s); 60 nach Leons Ruf. */
    static const int k_sl[] = { 30, 43, 1, 35, 36, 72, 1, 118, 75, 1, 42, 60 };
    CHECK(ns == (int)(sizeof k_sl / sizeof k_sl[0]) && memcmp(sl, k_sl, sizeof k_sl) == 0, "Sleep-Folge weicht ab (%d Sleeps)", ns);
    static const int k_ms[] = { RE15_AN_MSG_WILLIAM, RE15_AN_MSG_SATZ, RE15_AN_MSG_LEON };
    CHECK(nm == 3 && memcmp(ms, k_ms, sizeof k_ms) == 0, "Message_on-Folge %d Stueck (43, 40, 42 erwartet)", nm);
    /* Der Sturz laeuft im Takt (kein Pos_set im Programm, sonst blockiert er das Warten auf den Ankunfts-Puls); seine
     * Werte = ROOM5080 sub03: Pos_set y -5600 @0x00822 (`20 ea` @0x826), Speed_set(1,400) @0x0082E, For 14 @0x00832. */
    CHECK(sturz < 0, "Programm traegt einen Pos_set");
    CHECK(s_rdt5080 && s_rdt5080[0x826] == 0x20 && s_rdt5080[0x827] == 0xea && s_rdt5080[0x82E] == 0x2f &&
          s_rdt5080[0x830] == 0x90 && s_rdt5080[0x831] == 0x01 && s_rdt5080[0x836] == 0x0e, "ROOM5080 sub03 Sturz-Bytes");
    static const struct { int id; const char *t; } soll[] = {
        { 40, "Annette: W-William! You're alive!" }, { 42, "Leon: Annette!" }, { 43, "Annette: William." } };
    for (unsigned k = 0; k < sizeof soll / sizeof soll[0]; k++) {
        int len = 0; char txt[256] = { 0 };
        const uint8_t *m = re15_annette_r41_meldung(soll[k].id, &len);
        CHECK(m && len > 0, "Meldung %d fehlt", soll[k].id);
        if (!m) continue;
        re15_msg_decode_text(m, (size_t)len, txt, sizeof txt);
        CHECK(strstr(txt, soll[k].t) != NULL, "Meldung %d lautet '%s' statt '%s'", soll[k].id, txt, soll[k].t);
    }
    int l41 = -1;
    CHECK(re15_annette_r41_meldung(RE15_AN_MSG_SCHREI, &l41) == NULL && l41 == 0, "Schrei (41) hat einen Text (Nutzer F9: nur Ton)");
}

extern int re15_annette_r41_zug_segment(int i, const uint8_t **b, int *n, int *rdt);

static void test_programm_zug(void)
{
    CHECK(s_rdt6030 != NULL, "ROOM6030.RDT");
    if (!s_rdt6030) return;
    int segs = 0, orig = 0, port = 0;
    const uint8_t *b; int n, rdt;
    for (int i = 0; re15_annette_r41_zug_segment(i, &b, &n, &rdt); i++, segs++) {
        if (rdt) {
            orig += n;
            CHECK((size_t)rdt + (size_t)n <= s_n6030 && memcmp(b, s_rdt6030 + rdt, (size_t)n) == 0,
                  "Zugstart-Segment %d (%d Byte) nicht bytegleich mit ROOM6030 @0x%05X", i, n, rdt);
        } else port += n;
    }
    int len = 0;
    const uint8_t *p = re15_annette_r41_programm_zug(&len);
    CHECK(p && len == orig + port, "Zugstart-Programm %d Byte statt %d", len, orig + port);
    fprintf(stderr, "Zugstart: %d Segmente, %d Byte Original (ROOM6030), %d Byte Port-Befehle\n", segs, orig, port);
    if (!p) return;
    CHECK(p[0] == 0x22 && p[1] == RE15_AN_ZUG_BANK && p[2] == RE15_AN_ZUG_BIT && p[3] == 1, "Zugstart setzt (9,133) nicht zuerst");
    /* NB2 Punkt 3: der Port setzt keine Musik. Der Wechsel beim Tor ist Original (ROOM6030 sub06 @0x01092 / sub07 @0x0112C
     * `54 00 01 00 00 00` hinter Ck(3,89)=1 @0x0108E / @0x01128) - in keinem der drei Port-Programme steht Sce_bgm_control. */
    {
        int lp = 0, lt = 0, bgm = 0;
        const uint8_t *prog[3] = { p, re15_annette_r41_programm(&lp), re15_annette_r41_programm_tuer(&lt) };
        const int plen[3] = { len, lp, lt };
        for (int k = 0; k < 3; k++)
            for (int i = 0; prog[k] && i < plen[k];) {
                const int l = scd_opcode_size_at(prog[k] + i);
                if (l <= 0) break;
                if (prog[k][i] == 0x54) bgm++;
                i += l;
            }
        CHECK(bgm == 0, "Port-Programm setzt Musik (%d x Sce_bgm_control 0x54)", bgm);
        CHECK(memcmp(s_rdt6030 + 0x1092, "\x54\x00\x01\x00\x00\x00", 6) == 0 && memcmp(s_rdt6030 + 0x112C, "\x54\x00\x01\x00\x00\x00", 6) == 0 &&
              memcmp(s_rdt6030 + 0x108E, "\x21\x03\x59\x01", 4) == 0 && memcmp(s_rdt6030 + 0x1128, "\x21\x03\x59\x01", 4) == 0,
              "ROOM6030: Musikwechsel bei (3,89)=1 nicht an @0x0108E/@0x01092 und @0x01128/@0x0112C");
    }
    int ms[8], nm = 0, fahrt = 0, ende = 0, em = 0;
    for (int i = 0; i < len;) {
        const int l = scd_opcode_size_at(p + i);
        if (l <= 0 || i + l > len) { CHECK(0, "Zugstart: Opcode 0x%02X @%d ohne Laenge", p[i], i); break; }
        if (p[i] == 0x2b && nm < 8) ms[nm++] = p[i + 1];
        if (p[i] == 0x04 && p[i + 3] == 5) fahrt++;
        if (p[i] == 0x44 && p[i + 2] == 0x4d) em++;
        if (p[i] == 0x01) ende = (i + l == len);
        i += l;
    }
    static const int k_ms[] = { 27, 28, 29, 30 };
    CHECK(nm == 4 && memcmp(ms, k_ms, sizeof k_ms) == 0, "Zugstart: Message_on-Folge (%d) nicht 27..30", nm);
    CHECK(fahrt == 1 && em == 1 && ende, "Zugstart: Licht-Schleife %d, Sce_em_set 0x4D %d, Ende %d", fahrt, em, ende);
    static const struct { int id; const char *t; } soll[] = {
        { 27, "Annette: There! The train's moving." }, { 28, "Annette: What was that? ...Sherry is back there!" },
        { 29, "Leon: I'll check it out." }, { 30, "Annette: No. She's my daughter. Stay here and keep an eye on the controls." } };
    for (unsigned k = 0; k < sizeof soll / sizeof soll[0]; k++) {
        int l = 0; char txt[256] = { 0 };
        const uint8_t *m = re15_annette_r41_meldung_raum(0x6030u, soll[k].id, &l);
        CHECK(m && l > 0, "6030 Meldung %d fehlt", soll[k].id);
        if (!m) continue;
        re15_msg_decode_text(m, (size_t)l, txt, sizeof txt);
        CHECK(strstr(txt, soll[k].t) != NULL, "6030 Meldung %d lautet '%s' statt '%s'", soll[k].id, txt, soll[k].t);
    }
    int lt = 0;
    const uint8_t *t = re15_annette_r41_programm_tuer(&lt);
    CHECK(t && lt == 44 && memcmp(t, s_rdt6030 + 0x0C72, 32) == 0 && memcmp(t + 32, s_rdt6030 + 0x106A, 10) == 0 &&
          memcmp(t + 42, s_rdt6030 + 0x10CA, 2) == 0,
          "Tuer-Programm nicht = main00 @0x00C72 (Ausgang Slot 2) + sub06 @0x0106A + Evt_end @0x010CA (%d Byte)", lt);
}

static uint8_t *s_an_emd[2];
static size_t s_an_len[2];
static void test_modell(void)
{
    size_t esz = 0, off = 0, len = 0, r2sz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    uint8_t *r2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &r2sz);
    CHECK(ems && r2, "CDEMD0.EMS (RE1.5/RE2) fehlt");
    if (!ems || !r2) { free(ems); free(r2); return; }
    CHECK(re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x4Du), &off, &len) == 0, "RE1.5-Blob 0x4D");
    const uint8_t *alt = ems + off;
    static const struct { const char *datei; int kind; } k[2] = { { "ANNETTE_4D.EMD", 0x47 }, { "ANNETTE_4D_BLUT.EMD", 0x49 } };
    for (int i = 0; i < 2; i++) {
        char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/../RE2/%s", k[i].datei);
        s_an_emd[i] = slurp(p, &s_an_len[i]);
        CHECK(s_an_emd[i] != NULL, "%s fehlt", k[i].datei);
        if (!s_an_emd[i]) continue;
        const uint8_t *b = s_an_emd[i];
        uint32_t dof, dn, dof_a, dn_a;
        memcpy(&dof, b, 4); memcpy(&dn, b + 4, 4); memcpy(&dof_a, alt, 4); memcpy(&dn_a, alt + 4, 4);
        CHECK(dn == 9 && dn_a == 9 && dof + 36 == s_an_len[i], "%s: Verzeichnis", k[i].datei);
        uint32_t d[9], da[9];
        memcpy(d, b + dof, 36); memcpy(da, alt + dof_a, 36);
        /* Gerippe und Clips 0..23 bytegleich mit dem RE1.5-0x4D; NB4: Paar 1 traegt hinten Clip 24 = RE2-Liegeclip 0x18
         * (ROOM6141-RBJ, Auszug tools/r41_annette/re2_6141_liegen.bin). dir[0]-Inhalt, EMR von Paar 1 (Keyframes 0..357 als
         * Praefix) und Paar 2/3 bytegleich; die Bildliste von Paar 1 gleich bis auf die 40 neuen Eintraege. */
        CHECK(d[0] == da[0] && d[1] == da[1] && memcmp(b + 8, alt + 8, da[1] - 8) == 0, "%s: dir[0] nicht bytegleich mit EM4D", k[i].datei);
        CHECK(d[3] - d[2] == da[3] - da[2] + 40 * 80 && memcmp(b + d[2], alt + da[2], da[3] - da[2]) == 0,
              "%s: EMR Paar 1 ist kein EM4D-Praefix + 40 Keyframes", k[i].datei);
        CHECK(d[7] - d[3] == da[7] - da[3] && memcmp(b + d[3], alt + da[3], da[7] - da[3]) == 0, "%s: Paar 2/3 nicht bytegleich", k[i].datei);
        {
            re15_emd_animation_t *a0 = (re15_emd_animation_t *)calloc(1, sizeof *a0);
            re15_emd_animation_t *a1 = (re15_emd_animation_t *)calloc(1, sizeof *a1);
            size_t azl = 0;
            uint8_t *az = slurp(RE15_AN_AUSZUG, &azl);
            CHECK(az && azl == 40 * (4 + 80), "Auszug re2_6141_liegen.bin fehlt/Groesse %u", (unsigned)azl);
            if (a0 && a1 && az && re15_emd_parse_animation(alt + da[1], da[2] - da[1], a0) == 0 &&
                re15_emd_parse_animation(b + d[1], d[2] - d[1], a1) == 0) {
                int gleich = a0->clip_count == 24 && a1->clip_count == 25 && a1->frame_count == a0->frame_count + 40;
                for (int c = 0; gleich && c < 24; c++)
                    gleich = a0->clips[c].frame_count == a1->clips[c].frame_count && a0->clips[c].first_frame == a1->clips[c].first_frame;
                for (int f = 0; gleich && f < a0->frame_count; f++) gleich = a0->frames[f] == a1->frames[f];
                CHECK(gleich, "%s: Clips 0..23 von Paar 1 nicht gleich EM4D", k[i].datei);
                const int kf0 = (int)((da[3] - da[2]) - 176u) / 80;          /* 358 (kf_off 176, 80 Byte) */
                const uint16_t kfo = (uint16_t)(b[d[2] + 2] | (b[d[2] + 3] << 8));
                int liegt = a1->clips[24].frame_count == 40 && a1->clips[24].first_frame == a0->frame_count;
                for (int f = 0; liegt && f < 40; f++) {
                    uint32_t e = 0; memcpy(&e, az + 4 * f, 4);
                    liegt = (a1->frames[a0->frame_count + f] & 0xFFFu) == (uint32_t)(kf0 + f) &&
                            (a1->frames[a0->frame_count + f] & ~0xFFFu) == (e & ~0xFFFu) &&
                            memcmp(b + d[2] + kfo + (kf0 + f) * 80, az + 160 + f * 80, 80) == 0;
                }
                CHECK(kf0 == 358 && liegt, "%s: Clip 24 != RE2-Liegeclip 0x18 (Auszug), Keyframe-Basis %d", k[i].datei, kf0);
            }
            free(a0); free(a1); free(az);
        }
        /* MD1 + TIM = RE2 kind (TOC) */
        const uint8_t *emd2 = NULL, *tim2 = NULL; size_t el = 0, tl = 0;
        CHECK(re2_ems_locate(r2, r2sz, k[i].kind, RE2_EMS_REC_EMD, &emd2, &el) == 0 &&
              re2_ems_locate(r2, r2sz, k[i].kind, RE2_EMS_REC_TIM, &tim2, &tl) == 0, "RE2 kind 0x%02X", k[i].kind);
        if (emd2 && tim2) {
            uint32_t r2do, r2d[9];
            memcpy(&r2do, emd2, 4); memcpy(r2d, emd2 + r2do, 32); r2d[8] = r2do;
            CHECK(d[8] - d[7] >= r2d[8] - r2d[7] && memcmp(b + d[7], emd2 + r2d[7], r2d[8] - r2d[7]) == 0, "%s: MD1 != RE2 0x%02X", k[i].datei, k[i].kind);
            CHECK(dof - d[8] >= tl && memcmp(b + d[8], tim2, tl) == 0, "%s: TIM != RE2 0x%02X", k[i].datei, k[i].kind);
        }
        re15_md1_t *md1 = (re15_md1_t *)calloc(1, sizeof *md1);
        re15_emd_skeleton_t *sk = (re15_emd_skeleton_t *)calloc(1, sizeof *sk);
        re15_emd_animation_t *an = (re15_emd_animation_t *)calloc(1, sizeof *an);
        CHECK(re15_emd_parse_container(b, s_an_len[i], md1, sk, an, NULL) == 0, "%s: Parser", k[i].datei);
        CHECK(sk->bone_count == 15 && an->clip_count == 25, "%s: %d Knochen %d Clips", k[i].datei, sk->bone_count, an->clip_count);
        free(md1); free(sk); free(an);
    }
    CHECK(strcmp(re15_annette_r41_emd_datei(0x4Du), "ANNETTE_4D.EMD") == 0 && re15_annette_r41_emd_datei(0x42u) == NULL,
          "Modellwahl");
    free(ems); free(r2);
}

static void bank_aus(uint8_t type, const uint8_t *b, size_t len)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb || re15_emd_parse_container(b, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) { fails++; return; }
    eb->ok = 1; eb->buf = NULL;
}

static uint8_t *s_form;
static size_t s_form_len;
static int form_bank(void)
{
    if (!s_form) s_form = slurp(RE15_ASSET_PSX_DIR "/../RE2/BAHNSTEIG_FORM.EMD", &s_form_len);
    if (!s_form) return 0;
    const size_t len = s_form_len;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    if (!eb) eb = re15_enemy_alloc(0x30u);
    if (!eb) return 0;
    memset(eb, 0, sizeof *eb); eb->type = 0x30u;
    if (re2_emd_parse_bank(s_form, len, eb) != 0) return 0;
    eb->ok = 1; eb->re2_rig = 1; eb->buf = NULL; eb->bb_form = 1;
    return 1;
}

static int cut_jetzt(void) { return (int)g_scd.work_vars[0x0A]; }

static uint32_t rd32le(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

/* ESP-Plaetze des Annette-Bluts (Effekt 0, Skala des Bluts; andere Effekte in 5090 = Feuer) */
static int blut_plaetze_aktiv(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *x = re15_esp_fx_get(i);
        if (x && x->effect_id == 0 && x->scale16 == (unsigned)re15_annette_r41_blut_skala()) n++;
    }
    return n;
}

static void test_szene(void)
{
    flags(1, 0, 1); zug(1);
    re15_annette_r41_reset();
    /* NB2: ESP-Baenke wie im Spiel (Raumbank ROOM5090 + CORE00.ESP), damit Plaetze und Lebensdauer messbar sind */
    static re15_esp_t esp5090, gesp;
    size_t rsz = 0, gsz = 0;
    static uint8_t *raw5090, *core;
    if (!raw5090) raw5090 = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!core) core = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &gsz);
    CHECK(raw5090 && re15_esp_parse(raw5090, rsz, rd32le(raw5090 + 0x4C), rd32le(raw5090 + 0x50), rd32le(raw5090 + 0x54),
                                    rd32le(raw5090 + 0x58), &esp5090) == 0, "ESP-Bank ROOM5090");
    CHECK(core && re15_esp_parse_global(core, gsz, &gesp) == 0, "CORE00.ESP");
    re15_esp_set_room_bank(&esp5090);
    re15_esp_set_global_bank(&gesp);
    re15_esp_fx_reset();
    const int zeilen = re15_esp_fx_spawn_rows(&esp5090, 0, 1, 0x2000, 0, 0, 0, 0, 0);   /* Zeilen je Aufruf (aus der Bank) */
    re15_esp_fx_reset();
    uint8_t *b = raum_laden("STAGE5/ROOM5090.RDT", 0x5090u, 13);
    CHECK(b != NULL, "ROOM5090");
    if (!b) return;
    re15_bf_modell_hook(NULL);
    re15_annette_r41_modell_hook(NULL);
    CHECK(form_bank(), "Bank der Riesenform");
    if (s_an_emd[0]) bank_aus(0x4Du, s_an_emd[0], s_an_len[0]);
    const int fs = slot_vom_typ(0x30u), as = slot_vom_typ(0x4Du);
    CHECK(fs > 0 && as > 0, "Riesenform %d / Annette %d", fs, as);
    if (fs <= 0 || as <= 0) { free(b); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 13775; pl->z = -23550; pl->hp = 100;         /* Ausloeser-Lage im Fensterlauf (debug.log Runde 41) */
    /* Kampfstart-Ausloeser wie im Spiel: Cut 12 in Wagen C (sub01 @0x01290 Cmp(0x0A,12)) */
    g_scd.work_vars[0x0A] = 12; g_scd.cam_id = 12;
    int ph_bild[8], cut_folge[16], ncut = 0, f, vorher = 0, reihenfolge_ok = 1, versteckt_ok = 1;
    for (int i = 0; i < 8; i++) ph_bild[i] = -1;
    int sturz_y0 = 1, sturz_bilder = -1, treffer_bild = -1, treffer_clip = -1;
    int32_t treffer_abstand = -1, steh_x = 0, steh_z = 0;
    int frau_clip_treffer = -1;
    int kampf = 0;
    int blut_erst = -1, blut_letzt = -1, blut_n = 0, wolke_weg = -1, wolke_max = 0, lage_ok = 0, lage_n = 0, pool_max = 0;
    for (f = 0; f < 3000; f++) {
        re15_game_flag_set(5, 0x20, 1);                  /* Plc_dest-Ankunft (Spieler-Laufmaschine, hier nicht im Takt) */
        scd_vm_tick();
        re15_enemy_ai_run_all(0);
        if (re15_annette_r41_blut() != blut_n) {
            blut_n = re15_annette_r41_blut();
            if (blut_erst < 0) {
                blut_erst = f;
                /* Lage im Spawnbild = Weltlage der Klauen 14 / 28 (Einheitsanker + Teil-T, RE2 @0x80105950-78) */
                int32_t k14[3], k28[3];
                re15_enemy_bone_world_pos(&g_actors[fs], 14, k14);
                re15_enemy_bone_world_pos(&g_actors[fs], 28, k28);
                for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
                    const re15_esp_fx_t *x = re15_esp_fx_get(i);
                    if (!x || x->effect_id != 0 || x->scale16 != (unsigned)re15_annette_r41_blut_skala()) continue;
                    lage_n++;
                    if ((x->x == k14[0] && x->y == k14[1] && x->z == k14[2]) || (x->x == k28[0] && x->y == k28[1] && x->z == k28[2]))
                        lage_ok++;
                }
                fprintf(stderr, "Blut F%d: Klaue 14 (%d,%d,%d), 28 (%d,%d,%d), Annette (%d,%d), %d/%d Plaetze an den Klauen\n", f,
                        (int)k14[0], (int)k14[1], (int)k14[2], (int)k28[0], (int)k28[1], (int)k28[2], (int)g_actors[as].x,
                        (int)g_actors[as].z, lage_ok, lage_n);
            }
            blut_letzt = f;
        }
        re15_esp_fx_tick(re15_esp_room_bank());
        {   const int w = blut_plaetze_aktiv();
            if (w > wolke_max) wolke_max = w;
            if (re15_esp_fx_count() > pool_max) pool_max = re15_esp_fx_count();
            if (blut_erst >= 0 && w == 0 && wolke_weg < 0 && f > blut_letzt) wolke_weg = f; }
        const int ph = re15_annette_r41_phase();
        {   static int b21 = -1; const int b = re15_game_flag_get(5, 0x21);
            if (getenv("AN_DIAG") && b != b21) fprintf(stderr, "F%d (5,0x21)=%d Annette (%d,%d) ss1 %d\n", f, b,
                                                       (int)g_actors[as].x, (int)g_actors[as].z, g_actors[as].sub_state_1);
            b21 = b; }
        if (ph < vorher && ph != RE15_AN_AUS) reihenfolge_ok = 0;
        if (ph != vorher && ph >= 0 && ph < 8 && ph_bild[ph] < 0) ph_bild[ph] = f;
        if (ph == RE15_AN_WARTEN && !g_actors[fs].no_draw) versteckt_ok = 0;
        if (ph == RE15_AN_STURZ && sturz_y0 > 0) sturz_y0 = (int)g_actors[fs].y;
        if (ph == RE15_AN_DROHEN && sturz_bilder < 0) sturz_bilder = f - ph_bild[RE15_AN_STURZ];
        if (ph == RE15_AN_HIEB && vorher != RE15_AN_HIEB) { steh_x = g_actors[as].x; steh_z = g_actors[as].z; }
        if (ph == RE15_AN_REAKTION && treffer_bild < 0) {
            int32_t h[3];
            frau_clip_treffer = g_actors[as].motion;
            treffer_bild = (int)g_actors[fs].anim_frame; treffer_clip = g_actors[fs].motion;
            re15_enemy_bone_world_pos(&g_actors[fs], 13, h);
            const int64_t dx = h[0] - g_actors[as].x, dz = h[2] - g_actors[as].z;
            treffer_abstand = (int32_t)(dx < 0 ? -dx : dx) + (int32_t)(dz < 0 ? -dz : dz);
        }
        if (ncut == 0 || cut_folge[ncut - 1] != cut_jetzt()) { if (ncut < 16) cut_folge[ncut++] = cut_jetzt(); }
        vorher = ph;
        if (ph == RE15_AN_FREI) { kampf = 1; break; }
    }
    fprintf(stderr, "Szene: Phasen-Bilder W%d S%d D%d H%d R%d L%d F%d, Sturz ab y %d in %d Bildern, Annette stand (%d,%d), "
            "Treffer Clip %d Bild %d Hand %d, Blut %d, Leiche (%d,%d,%d) rot_z %d, Toene K%d S%d B%d, Schrei %d, Schnitte",
            ph_bild[1], ph_bild[2], ph_bild[3], ph_bild[4], ph_bild[5], ph_bild[6], ph_bild[7], sturz_y0, sturz_bilder,
            (int)steh_x, (int)steh_z, treffer_clip, treffer_bild, (int)treffer_abstand, re15_annette_r41_blut(),
            (int)g_actors[as].x, (int)g_actors[as].y, (int)g_actors[as].z, (int)g_actors[as].rot_z,
            re15_annette_r41_ton(RE15_AN_SE_KRACH), re15_annette_r41_ton(RE15_AN_SE_STURZ), re15_annette_r41_ton(RE15_AN_SE_BRUELL),
            re15_annette_r41_schrei());
    for (int i = 0; i < ncut; i++) fprintf(stderr, " %d", cut_folge[i]);
    fprintf(stderr, "\n");
    if (!kampf)
        fprintf(stderr, "Haenger: Annette st %d ss1 0x%02X ss2 %d Clip %d Bild %d Ziel (%d,%d) Bit 0x%02X=%d\n",
                g_actors[as].state, g_actors[as].sub_state_1, g_actors[as].sub_state_2, g_actors[as].motion,
                (int)g_actors[as].anim_frame, (int)g_actors[as].steer_x, (int)g_actors[as].steer_z, g_actors[as].walk_flag_bit,
                re15_game_flag_get(5, g_actors[as].walk_flag_bit));
    CHECK(reihenfolge_ok, "Phasen laufen rueckwaerts");
    CHECK(kampf, "Szene endet nicht mit dem Kampfstart");
    for (int k = RE15_AN_WARTEN; k <= RE15_AN_FREI; k++) CHECK(ph_bild[k] >= 0, "Phase %d nie erreicht", k);
    CHECK(versteckt_ok, "Form vor dem Sturz sichtbar");
    CHECK(sturz_y0 <= -5200 && sturz_y0 >= -5600, "Sturz beginnt nicht bei y -5600 (5080 @0x00822): %d", sturz_y0);
    CHECK(sturz_bilder >= 13 && sturz_bilder <= 16, "Sturz %d Bilder statt 14 x Add_speed (5080 @0x00832)", sturz_bilder);
    /* Schnitte: 12 (Ausloeser) 15 11 13, danach sub04 */
    static const int k_cut[] = { 12, 15, 11, 13 };
    CHECK(ncut >= 4 && memcmp(cut_folge, k_cut, sizeof k_cut) == 0, "Schnittfolge der Szene weicht ab");
    CHECK(abs(steh_x - 3700) + abs(steh_z - (-22500)) < 600, "Annette steht beim Hieb bei (%d,%d) statt S (3700,-22500)",
          (int)steh_x, (int)steh_z);
    CHECK(treffer_clip == 4 && treffer_bild == 15, "Treffer nicht im Hieb-Clip 4 Bild 14 (Clip %d, Bild nach dem Vorschub %d)",
          treffer_clip, treffer_bild);
    CHECK(treffer_abstand >= 0 && treffer_abstand < 1500, "Hand links beim Treffer %d von ihr entfernt", (int)treffer_abstand);
    /* NB3 M4: im Trefferbild sackt sie mit Clip 6 zusammen; M7: Schnitt -> Treffer = Vorlauf 13 + 14 Bilder * 2 Takte + 1
     * (Treffer im Takt nach dem Bild 14), RE2 69,77 -> 71,09 s = 1,32 s = 40 Bilder */
    CHECK(frau_clip_treffer == 6, "Annette im Trefferbild Clip %d statt 6 (zusammensacken)", frau_clip_treffer);
    CHECK(ph_bild[RE15_AN_REAKTION] - ph_bild[RE15_AN_HIEB] == 42, "Hieb -> Treffer %d Bilder statt 42",
          ph_bild[RE15_AN_REAKTION] - ph_bild[RE15_AN_HIEB]);
    CHECK(ph_bild[RE15_AN_LEICHE] - ph_bild[RE15_AN_REAKTION] >= 33 && ph_bild[RE15_AN_LEICHE] - ph_bild[RE15_AN_REAKTION] <= 36,
          "Treffer -> Schnitt weg %d Bilder statt RE2 34", ph_bild[RE15_AN_LEICHE] - ph_bild[RE15_AN_REAKTION]);
    /* NB3 M6: Halt der Untertitel = RE2-Stimmlaenge (Xa 0x30 35, Xa 0x31 118) bzw. Sleep bis zum Kampfstart (60) */
    {   static const struct { int id, halt; } k_halt[] = { { RE15_AN_MSG_WILLIAM, 35 }, { RE15_AN_MSG_SATZ, 118 },
                                                           { RE15_AN_MSG_LEON, 60 } };
        for (unsigned k = 0; k < sizeof k_halt / sizeof k_halt[0]; k++) {
            int len = 0; const uint8_t *m = re15_annette_r41_meldung(k_halt[k].id, &len);
            const int d = m ? re15_msg_compute_duration(m, (size_t)len, 0) : -1;
            CHECK(d == k_halt[k].halt, "Untertitel %d steht %d Bilder statt %d", k_halt[k].id, d, k_halt[k].halt);
        }
    }
    /* NB2 Blut = RE2 FUN_80104C24: +0x16B = 15 @0x80104D10 ab dem Trefferbild, je Bild 2 x FUN_801058BC @0x80104D38/4C */
    fprintf(stderr, "Blut: %d Aufrufe in F%d..F%d (Treffer F%d), %d Zeilen je Aufruf, %d Plaetze belegt, Wolke max %d, "
            "weg ab F%d (%d Bilder nach dem ersten Spawn), Pool max %d/%d\n", re15_annette_r41_blut(), blut_erst, blut_letzt,
            ph_bild[RE15_AN_REAKTION], zeilen, re15_annette_r41_blut_plaetze(), wolke_max, wolke_weg,
            wolke_weg >= 0 ? wolke_weg - blut_erst : -1, pool_max, RE15_ESP_FX_MAX);
    CHECK(re15_annette_r41_blut() == 30, "Blut-Aufrufe %d statt 15 Bilder x 2 Klauen (@0x80104D10, @0x80104D38/4C)",
          re15_annette_r41_blut());
    CHECK(blut_erst == ph_bild[RE15_AN_REAKTION] && blut_letzt - blut_erst == 14,
          "Blut-Bilder F%d..F%d statt ab dem Trefferbild F%d 15 Bilder", blut_erst, blut_letzt, ph_bild[RE15_AN_REAKTION]);
    /* Pool voll: das Original verliert den Rest (FUN_8001B910 @0x8001B98C-98 `j 0x8001bc1c` / v0 = 0xFF); 5090 haelt dazu
     * die Feuer (main00) im selben 96er-Pool */
    CHECK(zeilen > 0 && (re15_annette_r41_blut_plaetze() == 30 * zeilen ||
                         (re15_annette_r41_blut_plaetze() < 30 * zeilen && pool_max == RE15_ESP_FX_MAX)),
          "Blut-Plaetze %d statt 30 x %d Zeilen ohne vollen Pool (Pool max %d)", re15_annette_r41_blut_plaetze(), zeilen,
          pool_max);
    /* Groesse gegen RE2: RAM e22 (71,54 s) hielt 60 Plaetze 0/1/0x2000 gleichzeitig (10 Gruppen zu 6) */
    CHECK(wolke_max >= 60, "Blutwolke hoechstens %d Plaetze gleichzeitig, RE2 e22: 60", wolke_max);
    CHECK(lage_n == 2 * zeilen && lage_ok == lage_n, "Blut im ersten Bild: %d/%d Plaetze an den Klauen 14/28", lage_ok, lage_n);
    CHECK(wolke_weg > blut_letzt, "Blutwolke verschwindet nicht nach dem letzten Spawn (F%d)", wolke_weg);
    CHECK(re15_annette_r41_ton(RE15_AN_SE_KRACH) == 1 && re15_annette_r41_ton(RE15_AN_SE_STURZ) == 1 &&
          re15_annette_r41_ton(RE15_AN_SE_BRUELL) == 1, "Toene Krach/Sturz/Bruellen nicht je einmal");
    CHECK(re15_annette_r41_schrei() == 1, "Schrei (Stimme 41 ohne Text) %d mal", re15_annette_r41_schrei());
    (void)steh_x; (void)steh_z;   /* NB4: die Liegestelle ist fest (RE2 @0x01162 setzt absolute Werte), nicht vom Stand abhaengig */
    CHECK(re15_game_flag_get(3, 42), "(3,42) nach dem Kampfstart nicht gesetzt (sub01 @0x0129A)");
    leiche_pruefen(&g_actors[as], "nach der Szene");
    CHECK(re15_annette_r41_besitzt(as) && !re15_annette_r41_besitzt(fs), "Besitz nach der Szene");
    CHECK(re15_annette_r41_kein_ziel(&g_actors[as]) && !re15_annette_r41_kein_ziel(&g_actors[fs]),
          "kein Schuss-Kandidat: nur die Leiche (NB4 N3)");
    CHECK(re15_annette_r41_emd_datei(0x4Du) && strstr(re15_annette_r41_emd_datei(0x4Du), "BLUT"), "nach dem Treffer keine Blut-Textur");
    /* Kampfstart lief wirklich: sub04 Pos_set (1200,-23350) @0x012FE und Member_set(0x0C,0x13) @0x0130A */
    for (int k = 0; k < 10; k++) { scd_vm_tick(); re15_enemy_ai_run_all(0); }
    CHECK(g_actors[fs].grid_id == 0x13, "Form nach sub04 nicht freigegeben (grid 0x%02X)", g_actors[fs].grid_id);
    /* Einmalig: ein zweiter Aufruf startet nichts */
    static const uint8_t dummy[2] = { 1, 0 };
    CHECK(re15_annette_r41_evt_exec(0x5090u, 4, dummy) == dummy, "Szene startet ein zweites Mal");
    free(b);
    zug(0);
}

/* 6030: Zugstart nach dem Tor, einmalig; vorher bleibt die Wagentuer nach 5090 beim Original-Text msg 10. */
static void test_zugstart(void)
{
    /* Wagen (Eintritt Cut 6, sub06): Tuer Slot 3 = Text msg 10, solange der Zugstart nicht lief */
    flags(1, 0, 1); zug(0);
    re15_annette_r41_reset();
    uint8_t *b = raum_laden("STAGE6/ROOM6030.RDT", 0x6030u, 6);
    CHECK(b != NULL, "ROOM6030");
    fprintf(stderr, "6030 Wagen ohne Zugstart: Slot 2 Typ %d Ereignis %d Ziel %u/%u, Slot 3 Typ %d Ereignis %d\n",
            g_aot.slots[2].type, g_aot.slots[2].event_id, g_aot.door_params[2].dest_stage, g_aot.door_params[2].dest_room,
            g_aot.slots[3].type, g_aot.slots[3].event_id);
    CHECK(b && g_aot.slots[3].type == RE15_AOT_TYPE_MESSAGE, "6030 Wagen vor dem Zugstart: Tuer nach 5090 offen (Typ %d)",
          g_aot.slots[3].type);
    /* NB2: der Zug steht -> Ausgang Slot 2 ist die Tuer aus main00 @0x00C72 (Ziel Stage-Byte 5 Raum 0 = ROOM6000), nicht
     * msg 20 "You can't get off the train right now." (@0x01098) */
    CHECK(b && g_aot.slots[2].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[2].dest_stage == 5 &&
          g_aot.door_params[2].dest_room == 0, "6030 Wagen vor dem Zugstart: Ausgang Slot 2 Typ %d Ziel %u/%u statt Tuer nach 6000",
          g_aot.slots[2].type, g_aot.door_params[2].dest_stage, g_aot.door_params[2].dest_room);
    free(b);
    zug(1);
    b = raum_laden("STAGE6/ROOM6030.RDT", 0x6030u, 6);
    fprintf(stderr, "6030 Wagen nach Zugstart: Slot 2 Typ %d Ereignis %d, Slot 3 Typ %d\n", g_aot.slots[2].type,
            g_aot.slots[2].event_id, g_aot.slots[3].type);
    CHECK(b && g_aot.slots[3].type == RE15_AOT_TYPE_DOOR, "6030 Wagen nach dem Zugstart: Tuer nach 5090 zu (Typ %d)",
          g_aot.slots[3].type);
    CHECK(b && g_aot.slots[2].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[2].event_id == 20,
          "6030 Wagen nach dem Zugstart: Ausgang Slot 2 nicht msg 20 (@0x01098): Typ %d Ereignis %d", g_aot.slots[2].type,
          g_aot.slots[2].event_id);
    free(b);
    /* Fuehrerstand (Eintritt Cut 0): Zugstart einmal */
    zug(0);
    re15_annette_r41_reset();
    if (s_an_emd[0]) bank_aus(0x4Du, s_an_emd[0], s_an_len[0]);
    b = raum_laden("STAGE6/ROOM6030.RDT", 0x6030u, 0);
    CHECK(b != NULL, "ROOM6030 Cut 0");
    if (!b) return;
    CHECK(re15_annette_r41_zug_szene() == 1, "Zugstart-Programm %d mal gestartet", re15_annette_r41_zug_szene());
    CHECK(re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT), "(9,133) nicht gesetzt");
    const int as = slot_vom_typ(0x4Du);
    CHECK(as > 0, "6030 Zugstart: Annette nicht am Pult (Sce_em_set @0x010F8)");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -3873; pl->z = -23487; pl->hp = 100;          /* Tuer Slot 1 main00 @0x00C52 Ziel */
    int ende = -1, ton_an = -1, ton_krach = -1, f, cut_vorher = -1, cut_vor_weg = -1, cut_weg = -1;
    int tuer_auf = -1, tuer_zu = -1, tuer_n = 0, leon_ziel = 0, f_weg = -1;   /* NB4 N4 */
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    for (f = 0; f < 3000; f++) {
        re15_game_flag_set(5, 0x20, 1);                  /* Plc_dest-Ankunft Leon */
        scd_vm_tick();
        re15_enemy_ai_run_all(0);
        if (pl->walk_dest_x == -3000 && pl->walk_dest_z == -23700) leon_ziel = 1;   /* NB4: Leon tritt zur Seite */
        {   scd_audio_event_t ev;
            while (scd_audio_queue_pop(&ev))
                if (ev.kind == SCD_AUDIO_SE_ON && ev.bank == 0x0E) {
                    tuer_n++;
                    if (ev.sample_id == 0 && tuer_auf < 0) tuer_auf = f;
                    if (ev.sample_id == 1 && tuer_zu < 0) tuer_zu = f;
                }
        }
        if (as > 0 && cut_weg < 0 && g_actors[as].z == 8646) { cut_weg = (int)g_scd.work_vars[0x0A]; cut_vor_weg = cut_vorher; f_weg = f; }
        cut_vorher = (int)g_scd.work_vars[0x0A];
        if (ton_an < 0 && re15_annette_r41_ton(RE15_AN_SE_ZUG_AN)) ton_an = f;
        if (ton_krach < 0 && re15_annette_r41_ton(RE15_AN_SE_KRACH)) ton_krach = f;
        if (f > 30 && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27)) { ende = f; break; }
    }
    fprintf(stderr, "Zugstart: Ton an F%d, Krach F%d, Ende F%d, Annette (%d,%d,%d)\n", ton_an, ton_krach, ende,
            as > 0 ? (int)g_actors[as].x : 0, as > 0 ? (int)g_actors[as].y : 0, as > 0 ? (int)g_actors[as].z : 0);
    CHECK(ende > 0, "Zugstart endet nicht (Sperren (2,7)/(1,27) bleiben)");
    CHECK(ton_an >= 0 && ton_krach > ton_an, "Toene: Zug an F%d, Krach F%d", ton_an, ton_krach);
    if (as > 0) CHECK(g_actors[as].z == 8646 && g_actors[as].x == -10507, "Annette nach dem Abgang nicht versteckt (6000 @0x0100A)");
    /* NB3 M2: Abgang in Cut 0 (Tuer im Bild), versteckt im selben Bild wie der Schnitt auf Cut 1 (Auge in der Tuer) */
    CHECK(cut_vor_weg == 0 && cut_weg == 1, "Abgang: vor dem Verstecken Cut %d (statt 0), beim Verstecken Cut %d (statt 1)",
          cut_vor_weg, cut_weg);
    /* NB4 N4: Leon tritt zur Seite (Form 6031 @0x00D86), Tuerton der RE2-Tuerbank (Port-Bank 0x0E): Satz 0 (oeffnen) im Bild des
     * Verschwindens, Satz 1 (zufallen) 30 Bilder spaeter (RE2 Claire ROOM4081 sub05 @0x03634 / Sleep 30 @0x03640 / @0x03644) */
    int bild_weg = -1;
    {   /* Bild des Verschwindens = erstes Bild mit Cut 1 nach dem Gehen; aus der Reihenfolge der Segmente */
        const uint8_t *sb; int sn, sr, i_auf = -1, i_cut1 = -1, i_zu = -1, i_s30 = -1;
        for (int i = 0; re15_annette_r41_zug_segment(i, &sb, &sn, &sr); i++) {
            if (sn == 12 && sb[0] == 0x36 && sb[1] == 0x0E && sb[2] == 0) i_auf = i;
            if (sn == 12 && sb[0] == 0x36 && sb[1] == 0x0E && sb[2] == 1) i_zu = i;
            if (sr == 0x0114A) i_cut1 = i;
            if (sr == 0x01364) i_s30 = i;
        }
        CHECK(i_auf >= 0 && i_cut1 == i_auf + 1 && i_s30 > i_cut1 && i_zu == i_s30 + 1,
              "Tuerton-Segmente: auf %d, Cut 1 %d, Sleep 30 %d, zu %d", i_auf, i_cut1, i_s30, i_zu);
        bild_weg = f_weg;
    }
    fprintf(stderr, "Abgang: Leon zur Seite %d, Tuerton auf F%d zu F%d (%d Se_on Bank 0x0E)\n", leon_ziel, tuer_auf, tuer_zu, tuer_n);
    CHECK(leon_ziel, "Leon tritt nicht zur Seite (Plc_dest (-3000,-23700))");
    CHECK(tuer_n == 2 && tuer_auf >= 0 && tuer_auf == bild_weg && tuer_zu - tuer_auf >= 30 && tuer_zu - tuer_auf <= 31,
          "Tuerton: %d Se_on Bank 0x0E, auf F%d (versteckt F%d), zu F%d (Soll 2, auf = versteckt, Abstand 30)", tuer_n, tuer_auf,
          bild_weg, tuer_zu);
    /* Wiedereintritt: kein zweiter Zugstart, keine Annette */
    free(b);
    b = raum_laden("STAGE6/ROOM6030.RDT", 0x6030u, 0);
    CHECK(b && re15_annette_r41_zug_szene() == 1 && slot_vom_typ(0x4Du) < 0, "6030 Wiedereintritt: Zugstart %d, Annette-Slot %d",
          re15_annette_r41_zug_szene(), slot_vom_typ(0x4Du));
    free(b);
    zug(0);
}

/* NB4 N3 (Abnahme 2: F2751 Annette hp -1 -> -6, st 3, ss1 3 = Waffe 3): der Waffen-Resolver re15_player_weapon_fire waehlte die
 * Leiche (naechstes Ziel im Keil) vor der G5. Lage wie im Abnahme-Lauf: Leon (14378,-23510) Gier 2049, G5 (0x36) (1800,-23400).
 *   (a) Leiche (RE2-Gate @0x80047148/50 hp < 0): kein Kandidat, der Schuss trifft die G5 dahinter.
 *   (b) dasselbe NPC ohne Leichen-Zustand (lebendes NPC, RE1.5): es ist das naechste Ziel, die Typ-Pruefung @0x80012178-84
 *       (`sltiu v0,v0,0x40` @0x80012180) laesst den Schuss nichts treffen - kein Schaden am NPC, keiner an der G5. */
static void test_schuss(void)
{
    re15_game_flag_set(3, 89, 1); re15_game_flag_set(3, 42, 1); zug(1);
    uint8_t *b = raum_laden("STAGE5/ROOM5090.RDT", 0x5090u, 12);
    CHECK(b != NULL, "ROOM5090 nicht ladbar");
    if (!b) return;
    const int s = slot_vom_typ(0x4Du);
    CHECK(s > 0, "keine 0x4D in 5090 bei (3,42)=1");
    if (s <= 0) { free(b); return; }
    re15_annette_r41_reset();
    re15_annette_r41_tick();                                   /* Wiedereintritt: Leiche gelegt (Phase RUHE) */
    re15_actor_t *a = &g_actors[s];
    re15_enemy_apply_hitbox(a, 0x4Du);
    int g = -1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (!g_actors[i].active) { g = i; break; }
    CHECK(g > 0, "kein freier Slot fuer die G5");
    if (g <= 0) { free(b); return; }
    for (int i = 1; i < RE15_ACTOR_MAX; i++)                  /* nur Leiche und G5 im Raum */
        if (i != s && g_actors[i].active) g_actors[i].active = 0;
    re15_actor_t *f = &g_actors[g];
    memset(f, 0, sizeof *f);
    f->active = 1; f->type = 0x36; f->hp = 345; f->x = 1800; f->y = 0; f->z = -23400; f->rot_y = 3;
    re15_enemy_apply_hitbox(f, 0x36u);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 14378; pl->y = 0; pl->z = -23510; pl->rot_y = 2049;
    const re15_actor_t a0 = *a, f0 = *f;
    int r = re15_player_weapon_fire(3);
    fprintf(stderr, "Schuss (a) Leiche: Rueckgabe %d (G5-Slot %d), Leiche hp %d st %d, G5 hp %d\n", r, g, a->hp, a->state, f->hp);
    CHECK(r == g + 1 && a->hp == -1 && a->state == a0.state && f->hp < 345,
          "Leiche schluckt den Schuss oder nimmt Schaden: r %d, Leiche hp %d st %d, G5 hp %d", r, a->hp, a->state, f->hp);
    *a = a0; *f = f0;
    re15_annette_r41_reset();                                  /* kein Leichen-Zustand: dieselbe 0x4D als lebendes NPC */
    CHECK(!re15_annette_r41_kein_ziel(a), "nach reset noch kein_ziel");
    r = re15_player_weapon_fire(3);
    fprintf(stderr, "Schuss (b) lebendes NPC: Rueckgabe %d, NPC hp %d st %d, G5 hp %d\n", r, a->hp, a->state, f->hp);
    CHECK(r == 0 && a->hp == -1 && a->state == a0.state && f->hp == 345,
          "RE1.5 @0x80012178-84: NPC als naechstes Ziel muss den Schuss ins Leere gehen lassen: r %d, NPC hp %d st %d, G5 hp %d",
          r, a->hp, a->state, f->hp);
    re15_game_flag_set(3, 42, 0); re15_game_flag_set(3, 89, 0); zug(0);
    free(b);
}

int main(void)
{
    s_rdt5080 = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5080.RDT", &s_n5080);
    s_rdt6030 = slurp(RE15_ASSET_PSX_DIR "/STAGE6/ROOM6030.RDT", &s_n6030);
    test_bedingungen();
    test_programm();
    test_programm_zug();
    test_modell();
    test_szene();
    test_zugstart();
    test_schuss();
    if (fails) { fprintf(stderr, "test_r41_annette: %d Fehler\n", fails); return 1; }
    printf("test_r41_annette: OK\n");
    return 0;
}
