/* test_r41_annette.c — Runde 41 Spur AN: Annette-Szene ROOM5090 (include/re15_annette_r41.h,
 * Dossier analysis/befunde_runde41/AN_annette.md).
 *
 *   bedingungen  Spawn der Frau im Kittel: 5090 nur bei (3,89)=1 und (3,42)=0 (PORT-WAHL), 6030 nach dem Original
 *                (sub07 @0x010F8 spawnt bei (4,23)=1 nur, solange (3,89)=0) - beide Raeume mit dem echten RDT-Skript.
 *   programm     Bytecode aus Original-Opcodes: Sperre, Cut 11, Texte 40/41/42 mit Wortlaut, Ende Evt_exec sub04.
 *   modell       ANNETTE_4D*.EMD: Gerippe/Clips = RE1.5-0x4D-Blob dir[0..6] bytegleich, MD1 + TIM = RE2 kind 0x47 / 0x49.
 *   szene        ganzer Ablauf mit Riesenform- und Annette-Bank ueber VM + KI-Takt: Drohen -> Weichen -> Hieb (Treffer im
 *                Bild 14 von Clip 4, Hand nahe) -> Flug -> liegt -> Kampfstart sub04 (Form frei, Annette bleibt Leiche),
 *                einmalig.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    scd_vm_init();
    re15_game_flag_set(3, 89, f89); re15_game_flag_set(3, 42, f42); re15_game_flag_set(4, 23, f423);
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

static void flags(int f89, int f42, int f423)
{
    re15_game_flag_set(3, 89, f89); re15_game_flag_set(3, 42, f42); re15_game_flag_set(4, 23, f423);
}

static void test_bedingungen(void)
{
    static const struct { int f89, f42, soll; } k5090[] = { { 0, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 0, 1, 0 } };
    for (unsigned i = 0; i < sizeof k5090 / sizeof k5090[0]; i++) {
        flags(k5090[i].f89, k5090[i].f42, 1);
        CHECK(re15_annette_r41_spawn_weg(0x5090u, 0x4Du) == !k5090[i].soll, "spawn_weg 5090 (3,89)=%d (3,42)=%d",
              k5090[i].f89, k5090[i].f42);
        uint8_t *b = raum_laden("STAGE5/ROOM5090.RDT", 0x5090u, 12);
        CHECK(b != NULL, "ROOM5090 nicht ladbar");
        const int s = slot_vom_typ(0x4Du);
        CHECK((s > 0) == (k5090[i].soll != 0), "5090 (3,89)=%d (3,42)=%d: Frau im Kittel %s erwartet, Slot %d",
              k5090[i].f89, k5090[i].f42, k5090[i].soll ? "" : "nicht", s);
        if (s > 0) CHECK(g_actors[s].x == 600 && g_actors[s].z == -25300, "5090 Lage (%d,%d) statt Spawn @0x0122A (600,-25300)",
                         (int)g_actors[s].x, (int)g_actors[s].z);
        CHECK(slot_vom_typ(0x30u) > 0 || k5090[i].f42, "5090: kein 0x30 (Riesenform) bei (3,42)=0");
        free(b);
    }
    /* Andere Raeume und Typen bleiben unberuehrt */
    flags(0, 0, 1);
    CHECK(!re15_annette_r41_spawn_weg(0x5091u, 0x4Du) && !re15_annette_r41_spawn_weg(0x6030u, 0x4Du) &&
          !re15_annette_r41_spawn_weg(0x5090u, 0x30u), "spawn_weg greift ausserhalb 5090/0x4D");
    /* 6030 Fuehrerstand (Eintritt Cut 0 -> sub00 Case 0 -> sub07): Original-Verhalten, kein Port-Eingriff */
    flags(0, 0, 1);
    uint8_t *b = raum_laden("STAGE6/ROOM6030.RDT", 0x6030u, 0);
    CHECK(b && slot_vom_typ(0x4Du) > 0, "6030 vor dem Tor: Frau am Pult fehlt (sub07 @0x010F8)");
    if (slot_vom_typ(0x4Du) > 0)
        CHECK(g_actors[slot_vom_typ(0x4Du)].x == -1790 && g_actors[slot_vom_typ(0x4Du)].z == -22272, "6030 Lage");
    free(b);
    flags(1, 0, 1);
    b = raum_laden("STAGE6/ROOM6030.RDT", 0x6030u, 0);
    CHECK(b && slot_vom_typ(0x4Du) < 0, "6030 nach dem Tor (3,89)=1: Frau am Pult darf nicht mehr da sein");
    free(b);
    /* Elza 5091: keine Frau im Kittel auf der Disc */
    flags(1, 0, 1);
    b = raum_laden("STAGE5/ROOM5091.RDT", 0x5091u, 12);
    CHECK(b && slot_vom_typ(0x4Du) < 0, "5091 hat eine 0x4D");
    free(b);
}

static void test_programm(void)
{
    int n = 0;
    const uint8_t *p = re15_annette_r41_programm(&n);
    CHECK(p && n > 20, "Programm fehlt");
    if (!p) return;
    CHECK(p[0] == 0x22 && p[1] == 2 && p[2] == 7 && p[3] == 1 && p[4] == 0x22 && p[5] == 1 && p[6] == 27 && p[7] == 1,
          "Programmkopf: Sperre/Balken wie sub04 @0x012F2/@0x012F6");
    CHECK(p[8] == 0x29 && p[9] == 11, "Programm: Cut 11 (#145) fehlt");
    CHECK(n >= 6 && p[n - 6] == 0x04 && p[n - 5] == 0xff && p[n - 4] == 0x18 && p[n - 3] == 4 && p[n - 2] == 0x01,
          "Programmende: Evt_exec sub04 + Evt_end");
    int msgs = 0;
    for (int i = 0; i + 3 < n; i++)
        if (p[i] == 0x2b && (p[i + 1] == 40 || p[i + 1] == 41 || p[i + 1] == 42) && p[i + 2] == 0 && p[i + 3] == 0) msgs++;
    CHECK(msgs == 3, "Programm: %d von 3 Message_on 40/41/42", msgs);
    static const char *soll[3] = { "Annette: W-William! You're alive!", "Annette: Aaaah!", "Leon: Annette!" };
    for (int id = 40; id <= 42; id++) {
        int len = 0;
        const uint8_t *m = re15_annette_r41_meldung(id, &len);
        char txt[256] = { 0 };
        CHECK(m && len > 0, "Meldung %d fehlt", id);
        if (!m) continue;
        re15_msg_decode_text(m, (size_t)len, txt, sizeof txt);
        CHECK(strstr(txt, soll[id - 40]) != NULL, "Meldung %d lautet '%s' statt '%s'", id, txt, soll[id - 40]);
    }
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
        /* Gerippe und Clips (dir[0..6]) bytegleich mit dem RE1.5-0x4D */
        CHECK(memcmp(d, da, 8 * 4) == 0 && memcmp(b + 8, alt + 8, da[7] - 8) == 0, "%s: dir[0..6] nicht bytegleich mit EM4D", k[i].datei);
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
        CHECK(sk->bone_count == 15 && an->clip_count == 24, "%s: %d Knochen %d Clips", k[i].datei, sk->bone_count, an->clip_count);
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

static void test_szene(void)
{
    flags(1, 0, 1);
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
    pl->x = 13000; pl->z = -23550; pl->hp = 100;
    /* Kampfstart-Ausloeser wie im Spiel: Cut 12 in Wagen C (sub01 @0x01290 Cmp(0x0A,12)) */
    g_scd.work_vars[0x0A] = 12; g_scd.cam_id = 12;
    int phase_max = 0, treffer_bild = -1, treffer_ok = 0, sub04 = 0, f;
    int32_t treffer_abstand = -1;
    int reihenfolge_ok = 1, vorher = 0;
    for (f = 0; f < 1500; f++) {
        re15_game_flag_set(5, 0x20, 1);                  /* Plc_dest-Ankunft (Spieler-Laufmaschine, hier nicht im Takt) */
        scd_vm_tick();
        re15_enemy_ai_run_all(0);
        const int ph = re15_annette_r41_phase();
        if (ph < vorher) reihenfolge_ok = 0;
        vorher = ph;
        if (ph > phase_max) phase_max = ph;
        if (ph == RE15_AN_FLUG && treffer_bild < 0) {
            int32_t h[3];
            treffer_bild = (int)g_actors[fs].anim_frame;
            re15_enemy_bone_world_pos(&g_actors[fs], 13, h);
            const int64_t dx = h[0] - g_actors[as].x, dz = h[2] - g_actors[as].z;
            treffer_abstand = (int32_t)(dx < 0 ? -dx : dx) + (int32_t)(dz < 0 ? -dz : dz);
            treffer_ok = g_actors[fs].motion == 4;
        }
        if (ph == RE15_AN_FREI) { sub04 = 1; break; }
    }
    fprintf(stderr, "Szene: Phase bis %d nach %d Bildern, Treffer Clip-4-Bild %d Hand-Abstand %d, Blut %d, Annette (%d,%d,%d) rot_z %d\n",
            phase_max, f, treffer_bild, (int)treffer_abstand, re15_annette_r41_blut(), (int)g_actors[as].x, (int)g_actors[as].y,
            (int)g_actors[as].z, (int)g_actors[as].rot_z);
    CHECK(reihenfolge_ok, "Phasen laufen rueckwaerts");
    CHECK(sub04, "Szene endet nicht mit dem Kampfstart (Phase %d)", phase_max);
    CHECK(treffer_ok && treffer_bild == 15, "Treffer nicht im Hieb-Clip 4 Bild 14 (Clip-Bild nach dem Vorschub %d)", treffer_bild);
    CHECK(treffer_abstand >= 0 && treffer_abstand < 1500, "Hand links beim Treffer %d von ihr entfernt", (int)treffer_abstand);
    CHECK(re15_annette_r41_blut() >= 6, "Blut %d < 6 (4 beim Treffer, 2 beim Aufschlag)", re15_annette_r41_blut());
    CHECK(re15_game_flag_get(3, 42), "(3,42) nach dem Kampfstart nicht gesetzt (sub01 @0x0129A)");
    CHECK(g_actors[as].hp == -1 && g_actors[as].rot_z == -683 && g_actors[as].y == -480 && g_actors[as].motion == 6,
          "Leiche: hp %d rot_z %d y %d Clip %d", g_actors[as].hp, g_actors[as].rot_z, (int)g_actors[as].y, g_actors[as].motion);
    CHECK(re15_annette_r41_besitzt(as) && !re15_annette_r41_besitzt(fs), "Besitz nach der Szene");
    CHECK(re15_annette_r41_volle_drehung(&g_actors[as]) && !re15_annette_r41_volle_drehung(&g_actors[fs]), "Kippen nur fuer Annette");
    CHECK(re15_annette_r41_emd_datei(0x4Du) && strstr(re15_annette_r41_emd_datei(0x4Du), "BLUT"), "nach dem Treffer keine Blut-Textur");
    /* Kampfstart lief wirklich: sub04 Pos_set (1200,-23350) @0x012FE und Member_set(0x0C,0x13) @0x0130A */
    for (int k = 0; k < 10; k++) { scd_vm_tick(); re15_enemy_ai_run_all(0); }
    CHECK(g_actors[fs].grid_id == 0x13, "Form nach sub04 nicht freigegeben (grid 0x%02X)", g_actors[fs].grid_id);
    /* Einmalig: ein zweiter Aufruf startet nichts */
    static const uint8_t dummy[2] = { 1, 0 };
    CHECK(re15_annette_r41_evt_exec(0x5090u, 4, dummy) == dummy, "Szene startet ein zweites Mal");
    free(b);
}

int main(void)
{
    test_bedingungen();
    test_programm();
    test_modell();
    test_szene();
    if (fails) { fprintf(stderr, "test_r41_annette: %d Fehler\n", fails); return 1; }
    printf("test_r41_annette: OK\n");
    return 0;
}
