/**
 * @file test_re2_gore.c
 * @brief PIN — WELLE E: der RE2-ZERLEGER-/GORE-ZWEIG der Trefferreaktion.
 *
 * Gefahren wird der ECHTE Ladeweg (wie test_re2_livepath): ROOM1140.RDT -> raum-eigenes SCD
 * (Sce_em_set-Spawns) -> RE2-Bank aus shared_assets/RE2/CDEMD0.EMS -> re15_enemy_ai_run_all +
 * re15_actors_anim_advance in game_step-Reihenfolge. Alle Sollwerte stammen aus eigener
 * Disassemblierung von info/re2leon/COMMON/BIN/EMOVL10_S0.BIN (RAW @0x80100000) bzw.
 * info/re2leon/PSX.EXE:
 *
 *  PIN 1 ANATOMIE     Die Part-Indizes des Zerlegers sind Knochen-Indizes. Die RE2-Bank EM010
 *                     muss die klassische RE-Kette liefern (15 Knochen, 9/10/11 = rechtes Bein,
 *                     12/13/14 = linkes Bein) — sonst zeigen die Offsets 1548/1720/2064/2236
 *                     (= Part 9/10/12/13, Stride 172) woanders hin.
 *  PIN 2 ZONEN-POOL   +0x151/+0x152/+0x153 = 13 nach dem INIT (`addiu v0,zero,13` @0x8010081C,
 *                     `sb v0,337/338/339(s2)` @0x80100820/24/28); je Treffer -= (rec.w1 >>
 *                     (Bracket*3)) & 7 mit Saettigung -1 (@0x80041954-88); die Region ist
 *                     +0x1D2 % 3 (`+0x1D2 = 3*Bracket + Region` @0x80041A88-9C).
 *  PIN 3 RICHTUNG     +0x1D0-Bits 0x20/0x40/0x80 aus d = Peilung(Angreifer->Ziel) - +0x76
 *                     (@0x80041A0C-84, Zwilling @0x80047370-D8).
 *  PIN 4 BEIN AB      Gate `!(+0x21A&0x60)` @0x80105288 + `(s8)+0x152 < 0` @0x8010529C +
 *                     `+0x1D0 & 0xC0` @0x801052AC; SE 9 @0x801052B8; Seite ueber 0x80
 *                     (@0x801052C8) -> Part 12/13 + `+0x21A|=0x40`, sonst Part 9/10 +
 *                     `+0x21A|=0x20`; Schienbein `flags |= 0x1062` @0x8010537C. EINMALIG.
 *  PIN 5 KNOCKDOWN    Handler 0x80107438 (Spalte 0 der Zeilen 7/8/9/12): Clip 1 @0x801074C8,
 *                     `+0x1D3 |= 0x80` @0x80107504, Arm-Abriss `flags |= 0x4A` @0x80107544 /
 *                     Hand `flags = 0` @0x80107588, Ausgang KRIECHER `+0x10E = 0x2001`
 *                     @0x80107824.
 *  PIN 6 VERKOHLUNG   Zeile 10 -> FUN_80106128: `+0x10E |= 0x80` @0x80106144, Tinten
 *                     @0x8010627C-F4, `+0x21A |= 0x800` @0x801051C0. UND: genau dieses Bit ist
 *                     das zweite Sub-Gate des Frame-Wort-SE-Pfads (@0x80101CE0-E4) — der
 *                     Typ-0x10-Zombie bekommt damit seine SCHRITT-SEs.
 *  PIN 7 FLAMMENWERFER Zeile 16: Zaehler +0x23A, Schwelle `sltiu v0,v0,0x9` @0x80105250 -> erst
 *                     der 10. Treffer verkohlt.
 *  PIN 8 WURFZAHLEN   FUN_80106510 = 19 RNG-Wuerfe (4 Emitter a 4 + 3 Einzelwuerfe),
 *                     FUN_80106128 = 1, FUN_80106310/FUN_8010640C = 0, Zerleger = 3.
 *                     Differenziell gemessen gegen eine Zeile ohne Zerleger.
 *  PIN 9 SAEURE/RUSS  Zeile 11 -> `+0x21A |= 0x1800` @0x80106334 + Tinte 0x00304040/0x00506060;
 *                     Zeile 9 -> Russ-Tinten OHNE Part-8-Tinte und OHNE +0x10E-Bit (das ist der
 *                     Unterschied zu FUN_80106128).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_room.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int      re15_re2z_last_hit_handler(void);
extern uint32_t re15_re2_rand_draws(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* ---- SE-Mitschnitt --------------------------------------------------------------------- */
static int s_se[512], s_se_n;
static void probe_se(int id, int f2) { (void)f2; if (s_se_n < 512) s_se[s_se_n++] = id; }
static int  se_count(int id) { int n = 0; for (int i = 0; i < s_se_n; i++) if (s_se[i] == id) n++; return n; }
static void bank_cap(int bank) { (void)bank; }

/* ---- RE2-Bank laden (pc_enemy_load-Spiegel) ---------------------------------------------- */
static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_sz);
    if (!s_ems) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* Der Zombie frisch als lebender, stehender Gegner. */
static void reset_z(re15_actor_t *e)
{
    e->state = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = 90; e->hit_react = 0;
    e->re2z_res223 = 20; e->re2z_flag222 = 0; e->re2z_cd239 = 0;
    e->re2z_hits1d2 = 1;                       /* Basis-Zone 1 @0x80047294-98, Bracket 0 */
    e->re2z_flags21a = 0; e->re2z_f10e = 0;
    e->re2z_t158 = 0; e->re2z_t15a = 0; e->re2z_dir16a = 0; e->re2z_gaitrow = 0;
    e->re2z_rag231 = 0; e->re2z_gy232 = 0;
    e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;
    e->re2z_burn23a = 0; e->re2z_hitdir1d0 = 0;
    e->re2z_self1d3 = 0;
    e->anim_frame = 0; e->anim_frac = 0;
    for (int i = 0; i < 16; i++) { e->re2z_part_flags[i] = (i < 15) ? 1u : 0u; e->re2z_part_tint[i] = 0; }
}

/* EIN Treffer der Zeile `row` in der Spalte `col`: genau das, was re15_re2_stamp_hit macht
 * (+0x5 = Zeile, +0x1D2 = Spalte, +0x6 = 0), danach EIN KI-Tick. */
static void hit(re15_actor_t *e, unsigned row, unsigned col)
{
    e->state = 2; e->sub_state_1 = (uint8_t)row; e->sub_state_2 = 0;
    e->re2z_hits1d2 = (uint8_t)col;
    e->re2z_res223 = 20;                        /* Flinch-Gate offen halten (@0x8010506C-78) */
    frame();
}

int main(void)
{
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1140.RDT fehlt\n"); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
    if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;

    int slot10 = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10) { slot10 = s; break; }
    if (slot10 < 0) { printf("FAIL: kein Zombie 0x10 in ROOM1140\n"); return 1; }
    re15_enemy_bank_t *bank = load_re2_bank(0x10);
    if (!bank || !bank->ok) { printf("FAIL: RE2-Bank EM010 fehlt (shared_assets/RE2/CDEMD0.EMS)\n");
                             return 1; }
    load_re2_bank(0x11); load_re2_bank(0x16);

    /* Nur EIN Gegner bleibt aktiv — sonst laufen fremde RNG-Wuerfe in die Wurfzahl-Pins. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot10) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot10];
    re15_re2z_audio_hook(probe_se, bank_cap);
    frame();                                    /* INIT-Tick (Zustandstabelle @0x8010C830[0]) */

    /* ================================================================= PIN 1: ANATOMIE ==== */
    printf("== PIN 1  RE2-Bank EM010: %d Knochen, %d Clips\n",
           bank->skel.bone_count, bank->anim.clip_count);
    printf("   Elternkette:");
    for (int i = 0; i < bank->skel.bone_count; i++)
        printf(" %d<-%d", i, (int)bank->skel.bone_parent[i]);
    printf("\n");
    CHECK(bank->skel.bone_count == 15,
          "PIN 1: der Zerleger adressiert Part 0..14 (Offsets 112..2520, Stride 172); "
          "gemessen %d Knochen", bank->skel.bone_count);
    if (bank->skel.bone_count == 15) {
        /* Was der Zerleger BRAUCHT, ist die STRUKTUR, nicht ein Wunsch-Array: zwei Dreierketten
         * ab der Wurzel (die Beine — Part 9/12 sind die Oberschenkel, 10/13 die Schienbeine,
         * 11/14 die Fuesse) und zwei Dreierketten ab der Brust (die Arme — 2/3/4 und 5/6/7).
         * Genau diese Indizes stehen in den Offsets 1548/1720/2064/2236 bzw. 516/860/1032. */
        const int8_t *p = bank->skel.bone_parent;
        CHECK(p[0] == -1 && p[1] == 0, "PIN 1: Part 0 = Wurzel, Part 1 = Brust");
        CHECK(p[9] == 0 && p[10] == 9 && p[11] == 10,
              "PIN 1: BEIN A muss die Kette 0->9->10->11 sein; gemessen %d/%d/%d",
              p[9], p[10], p[11]);
        CHECK(p[12] == 0 && p[13] == 12 && p[14] == 13,
              "PIN 1: BEIN B muss die Kette 0->12->13->14 sein; gemessen %d/%d/%d",
              p[12], p[13], p[14]);
        CHECK(p[2] == 1 && p[3] == 2 && p[4] == 3,
              "PIN 1: ARM A muss die Kette 1->2->3->4 sein (Handler 0x80107438 reisst Part 3 ab "
              "und loescht Part 4)");
        CHECK(p[5] == 1 && p[6] == 5 && p[7] == 6,
              "PIN 1: ARM B muss die Kette 1->5->6->7 sein");
        /* Abweichung zur Lehrbuch-Kette, ehrlich gemessen und dokumentiert: der Kopf (Part 8)
         * haengt in DIESER Bank an der WURZEL, nicht an der Brust. Fuer den Zerleger ist nur
         * wichtig, dass Part 8 ein Blatt ist (er wird nur getintet/beraucht, nie abgetrennt). */
        int head_leaf = 1;
        for (int i = 0; i < 15; i++) if (p[i] == 8) head_leaf = 0;
        CHECK(head_leaf, "PIN 1: Part 8 (Kopf) muss ein Blatt sein");
        printf("   (gemessen: Kopf Part 8 haengt an Part %d — Blatt: %s)\n", p[8],
               head_leaf ? "ja" : "nein");
    }
    CHECK(e->re2z_pool151 == 13 && e->re2z_pool152 == 13 && e->re2z_pool153 == 13,
          "PIN 2a: INIT muss +0x151/+0x152/+0x153 = 13 setzen (@0x80100820-28); gemessen %d/%d/%d",
          e->re2z_pool151, e->re2z_pool152, e->re2z_pool153);
    CHECK(e->re2z_burn23a == 0, "PIN 2a: +0x23A = 0 nach dem INIT (@0x801008B4)");

    /* ================================================ PIN 3: die TREFFERRICHTUNG +0x1D0 ==== */
    {
        struct { int32_t dx, dz; unsigned want; const char *name; } v[4] = {
            { -800,    0, 0x20u, "Spieler bei -X  (Zombie schaut +X -> RUECKEN)" },
            { +800,    0, 0x00u, "Spieler bei +X  (frontal, keine Seitenbits)"   },
            {    0, -800, 0x60u, "Spieler bei -Z  (Seite 0x40 + Ruecken-Ueberlappung)" },
            {    0, +800, 0x80u, "Spieler bei +Z  (Seite 0x80)"                  },
        };
        printf("== PIN 3  +0x1D0 aus d = Peilung(Angreifer->Ziel) - +0x76 (@0x80041A0C-84)\n");
        for (int i = 0; i < 4; i++) {
            reset_z(e);
            e->rot_y = 0;
            pl->x = e->x + v[i].dx; pl->z = e->z + v[i].dz;
            hit(e, 13u, 1u);                    /* Zeile 13 = MAIN, kein Zerleger */
            printf("   %-52s -> +0x1D0 = 0x%02X (Soll 0x%02X)\n",
                   v[i].name, (unsigned)(e->re2z_hitdir1d0 & 0xffu), v[i].want);
            CHECK((e->re2z_hitdir1d0 & 0xffu) == v[i].want,
                  "PIN 3: %s -> 0x%02X statt 0x%02X", v[i].name,
                  (unsigned)(e->re2z_hitdir1d0 & 0xffu), v[i].want);
        }
    }

    /* ============================================= PIN 2b: die Zonen-Reserve zaehlt runter == */
    {
        reset_z(e); e->rot_y = 0; pl->x = e->x; pl->z = e->z + 800;   /* Seitentreffer 0x80 */
        int seq[5], n = 0;
        for (int i = 0; i < 5; i++) { hit(e, 2u, 1u); seq[n++] = e->re2z_pool152; }
        printf("== PIN 2b +0x152 nach 5 Treffern der Zeile 2 (Kosten w1&7 = %u): %d %d %d %d %d\n",
               (unsigned)(0x02851014u & 7u), seq[0], seq[1], seq[2], seq[3], seq[4]);
        CHECK(seq[0] == 9 && seq[1] == 5 && seq[2] == 1 && seq[3] == -1 && seq[4] == -1,
              "PIN 2b: 13 -> 9 -> 5 -> 1 -> -1 (Kosten 4, Klemme @0x8004197C-88); gemessen "
              "%d %d %d %d %d", seq[0], seq[1], seq[2], seq[3], seq[4]);
        CHECK(e->re2z_pool151 == 13 && e->re2z_pool153 == 13,
              "PIN 2b: nur die getroffene Region (+0x1D2 %% 3 == 1 -> +0x152) darf sinken");
    }

    /* ===================================================== PIN 4: DAS BEIN KOMMT AB ======== */
    {
        /* VORHER: Reserve noch da -> KEIN Zerleger, obwohl der Treffer von der Seite kommt. */
        reset_z(e); e->rot_y = 0; pl->x = e->x; pl->z = e->z + 800;
        s_se_n = 0;
        hit(e, 3u, 1u);
        printf("== PIN 4  vorher: +0x152=%d +0x21A=0x%04X SE9=%d Part13-Flags=0x%04X\n",
               e->re2z_pool152, (unsigned)e->re2z_flags21a, se_count(9),
               (unsigned)e->re2z_part_flags[13]);
        CHECK((e->re2z_flags21a & 0x60u) == 0u,
              "PIN 4: mit intakter Reserve (+0x152 = %d >= 0) darf der Zerleger NICHT feuern "
              "(`bgez` @0x8010529C)", e->re2z_pool152);

        /* NACHHER: Reserve leer -> Zerleger, EINMAL. */
        reset_z(e); e->rot_y = 0; pl->x = e->x; pl->z = e->z + 800;
        e->re2z_pool152 = 0;                   /* der naechste Treffer (Kosten 4) klemmt auf -1 */
        s_se_n = 0;
        hit(e, 3u, 1u);
        unsigned f21a = e->re2z_flags21a, p13 = e->re2z_part_flags[13], p10 = e->re2z_part_flags[10];
        int se9 = se_count(9);
        /* Der Stumpf ist seit Welle F ein MESH-Tausch (Reserve-Part 15 -> Oberschenkel,
         * @0x8010531C-50), keine Tinte mehr — der Stand-in war ohne Beleg. Gepinnt wird er
         * in test_re2_gore_render (PIN 6); hier nur zur Anzeige. */
        printf("   nachher: +0x152=%d +0x21A=0x%04X SE9=%d Part13=0x%04X Part10=0x%04X "
               "Stumpf-Mesh(Part12)=%d\n",
               e->re2z_pool152, f21a, se9, p13, p10, (int)e->re2z_part_mesh[12]);
        CHECK(e->re2z_pool152 == -1, "PIN 4: +0x152 muss auf -1 klemmen; ist %d", e->re2z_pool152);
        CHECK((f21a & 0x40u) != 0u,
              "PIN 4: Seitentreffer 0x80 -> LINKES Bein, `+0x21A |= 0x40` (@0x801052E4); "
              "+0x21A = 0x%04X", f21a);
        CHECK(se9 >= 1, "PIN 4: SE 9 beim Abriss (`addiu a0,zero,9` @0x801052B4 / jal @0x801052B8)");
        CHECK((p13 & 0x1062u) == 0x1062u,
              "PIN 4: das Schienbein (Part 13) bekommt `flags |= 0x1062` (@0x8010537C); ist 0x%04X",
              p13);
        CHECK((p10 & 0x1062u) == 0u, "PIN 4: das ANDERE Bein bleibt unangetastet");

        /* EINMALIG: der `+0x21A & 0x60`-Riegel @0x80105288 blockt jeden weiteren Abriss. */
        s_se_n = 0;
        unsigned before = e->re2z_part_flags[10];
        for (int i = 0; i < 4; i++) { e->re2z_pool152 = -1; hit(e, 3u, 1u); }
        printf("   4 weitere Seitentreffer: SE9=%d +0x21A=0x%04X Part10=0x%04X\n",
               se_count(9), (unsigned)e->re2z_flags21a, (unsigned)e->re2z_part_flags[10]);
        CHECK(se_count(9) == 0 && e->re2z_part_flags[10] == before,
              "PIN 4: der Zerleger ist EINWEG (`andi 0x60` / `bne` @0x80105288-8C)");

        /* Gegenprobe SEITE: 0x40 -> RECHTES Bein (Part 9/10) + `+0x21A |= 0x20` @0x801052F4. */
        reset_z(e); e->rot_y = 0; pl->x = e->x; pl->z = e->z - 800;
        e->re2z_pool152 = -1; s_se_n = 0;
        hit(e, 3u, 1u);
        printf("   Gegenseite: +0x1D0=0x%02X +0x21A=0x%04X Part10=0x%04X Part13=0x%04X\n",
               (unsigned)(e->re2z_hitdir1d0 & 0xffu), (unsigned)e->re2z_flags21a,
               (unsigned)e->re2z_part_flags[10], (unsigned)e->re2z_part_flags[13]);
        CHECK((e->re2z_flags21a & 0x20u) != 0u && (e->re2z_part_flags[10] & 0x1062u) == 0x1062u,
              "PIN 4: ohne 0x80 -> Part 9/10 + `+0x21A |= 0x20` (@0x801052E8-F4)");

        /* FRONTAL: kein 0xC0 -> kein Abriss, obwohl die Reserve leer ist (@0x801052AC-B0). */
        reset_z(e); e->rot_y = 0; pl->x = e->x + 800; pl->z = e->z;
        e->re2z_pool152 = -1; s_se_n = 0;
        hit(e, 3u, 1u);
        CHECK((e->re2z_flags21a & 0x60u) == 0u,
              "PIN 4: frontaler Treffer (+0x1D0 & 0xC0 == 0) darf nichts abreissen; "
              "+0x1D0 = 0x%02X", (unsigned)(e->re2z_hitdir1d0 & 0xffu));
    }

    /* ================================================ PIN 5: Handler 0x80107438 =========== */
    {
        int saw_arm = 0, saw_hand = 0, saw_second = 0, saw_crawler = 0, saw_clip1 = 0, saw_claim = 0;
        for (int trial = 0; trial < 12; trial++) {
            reset_z(e); e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
            hit(e, 7u, 0u);                     /* Zeile 7, Spalte 0 -> 0x80107438 */
            if (trial == 0)
                printf("== PIN 5  Handler-Id der Zelle [7][0] = %d (Soll 5 = 0x80107438), "
                       "Clip=%d +0x1D3=0x%02X +0x16B=%u\n",
                       re15_re2z_last_hit_handler(), (int)e->motion,
                       (unsigned)e->re2z_self1d3, (unsigned)e->re2z_gaitrow);
            CHECK(re15_re2z_last_hit_handler() == 5,
                  "PIN 5: Zelle [7][0] muss 0x80107438 dispatchen (Tabelle @0x8010C940)");
            if (e->motion == 1) saw_clip1 = 1;                 /* 0xF0001 @0x801074C8 */
            if (e->re2z_self1d3 & 0x80u) saw_claim = 1;        /* @0x801074F0-504 */
            if (e->re2z_part_flags[3] & 0x4Au) saw_arm = 1;    /* @0x80107544 */
            if (e->re2z_part_flags[4] == 0u)   saw_hand = 1;   /* @0x80107588 */
            if (e->re2z_part_flags[6] & 0x4Au) saw_second = 1; /* @0x80107630 */
            /* bis in die Ausgangsphase durchlaufen */
            for (int f = 0; f < 200 && e->state == 2; f++) frame();
            if (e->re2z_f10e == 0x2001u) saw_crawler = 1;      /* @0x80107820-24 */
        }
        printf("   12 Durchlaeufe: Clip1=%d Claim=%d Arm-ab=%d Hand-weg=%d 2.Arm=%d Kriecher=%d\n",
               saw_clip1, saw_claim, saw_arm, saw_hand, saw_second, saw_crawler);
        CHECK(saw_clip1,  "PIN 5: P0 muss Clip 1 setzen (`lui v0,0xf / ori 1 / sw 332` @0x801074B0-C8)");
        CHECK(saw_claim,  "PIN 5: P0 setzt `+0x1D3 |= 0x80` (@0x801074F0-504)");
        CHECK(saw_arm && saw_hand,
              "PIN 5: der 50%%-Zweig (rand&1 @0x80107514) reisst den rechten Unterarm ab "
              "(flags |= 0x4A @0x80107544) und loescht die Hand (`sw zero,688` @0x80107588)");
        CHECK(saw_second, "PIN 5: der zweite 50%%-Zweig (@0x8010758C) nimmt den linken Arm mit");
        CHECK(saw_crawler,
              "PIN 5: P2 muss als KRIECHER enden (`+0x10E = 0x2001` @0x80107820-24)");
    }

    /* ============================== PIN 6: VERKOHLUNG + die Schritt-SEs fuer Typ 0x10 ======
     * Der LEBENDE Einstieg ist Stagger-P0 (@0x80105DC4, `lbu v1,5(s4)` OHNE vorherigen
     * Zustandswort-Store). Zeile 10, Spalte 1 -> Tabellenzelle 2 = 0x80105BC0. */
    {
        reset_z(e);
        e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
        hit(e, 10u, 1u);                        /* Zeile 10 = GL Brand -> STAGGER */
        CHECK(re15_re2z_last_hit_handler() == 2,
              "PIN 6: Zelle [10][1] muss 0x80105BC0 dispatchen (Tabelle @0x8010C940)");
        printf("== PIN 6  nach Zeile 10: +0x10E=0x%04X +0x21A=0x%04X Tinte[0]=0x%06X "
               "Tinte[2]=0x%06X Tinte[8]=0x%06X\n",
               (unsigned)e->re2z_f10e, (unsigned)e->re2z_flags21a,
               (unsigned)e->re2z_part_tint[0], (unsigned)e->re2z_part_tint[2],
               (unsigned)e->re2z_part_tint[8]);
        CHECK((e->re2z_f10e & 0x80u) != 0u,
              "PIN 6: FUN_80106128 setzt `+0x10E |= 0x80` (@0x8010613C-48)");
        CHECK((e->re2z_flags21a & 0x800u) != 0u,
              "PIN 6: der Aufrufer haengt `+0x21A |= 0x800` an (@0x80105DF0-FC)");
        CHECK(e->re2z_part_tint[0] == 0x00404040u && e->re2z_part_tint[2] == 0x00707070u &&
              e->re2z_part_tint[8] == 0x00505050u && e->re2z_part_tint[14] == 0x00404040u,
              "PIN 6: Verkohlungs-Tinten @0x8010627C-F4");

        /* Die BONUS-WIRKUNG: mit +0x10E&0x80 faellt das Sub-Gate des Frame-Wort-SE-Pfads
         * (@0x80101CE0-E4) und der Typ-0x10-Zombie bekommt seine Schritt-SEs. */
        const int WIN = 300;
        re15_actor_t save = *e;
        e->re2z_f10e &= (uint16_t)~0x80u;
        e->re2z_flags21a &= (uint16_t)~0x8000u;
        re15_ai_set_state_word(e, 0x101); e->sub_state_2 = 0; e->anim_frame = 0;
        pl->x = e->x + 30000; pl->z = e->z + 30000;
        s_se_n = 0;
        for (int t = 0; t < WIN; t++) frame();
        int mute = 0; for (int i = 0; i < s_se_n; i++) if (s_se[i] < 2) mute++;
        *e = save;
        e->re2z_f10e |= 0x80u;
        e->re2z_flags21a &= (uint16_t)~0x8000u;
        re15_ai_set_state_word(e, 0x101); e->sub_state_2 = 0; e->anim_frame = 0;
        s_se_n = 0;
        for (int t = 0; t < WIN; t++) frame();
        int loud = 0; for (int i = 0; i < s_se_n; i++) if (s_se[i] < 2) loud++;
        printf("   Schritt-SEs Typ 0x%02X in %d Frames: ohne +0x10E&0x80 = %d, mit = %d\n",
               e->type, WIN, mute, loud);
        CHECK(mute == 0, "PIN 6: ohne beide Sub-Gate-Bits KEIN Frame-Wort-SE (@0x80101CF8)");
        CHECK(loud > 0,
              "PIN 6: mit dem Verkohlungs-Bit muss der Typ-0x10-Zombie Schritt-SEs spielen "
              "(`bne v0,zero,0x80101d00` @0x80101CE4)");
    }

    /* ================================================ PIN 7: der Flammenwerfer-Zaehler =====
     * Lebender Einstieg: Haupt-Handler P0 (@0x80105510 `lbu v1,5(s1)`), Zeile 16 / Spalte 1. */
    {
        reset_z(e); e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
        int burn_at = -1;
        for (int i = 1; i <= 12; i++) {
            hit(e, 16u, 1u);
            if (burn_at < 0 && (e->re2z_f10e & 0x80u)) burn_at = i;
        }
        printf("== PIN 7  Zeile 16: Verkohlung beim %d. Treffer, +0x23A = %u, +0x16B = %u\n",
               burn_at, (unsigned)e->re2z_burn23a, (unsigned)e->re2z_gaitrow);
        CHECK(burn_at == 10,
              "PIN 7: `sltiu v0,v0,0x9` @0x80105250/@0x80105544 -> erst der 10. Treffer verkohlt; "
              "gemessen %d", burn_at);
    }

    /* ============ PIN 8: die RNG-WURFZAHLEN, differenziell ueber den DEATH-Einstieg ========
     * DEATH @0x80108250 traegt dieselbe Leiter, mit dem einzigen Gate `!(+0x21A&0x10) &&
     * (+0x21A&2)` (@0x801083E4-408) und einem festen Vorspann (Emitter 8000 @0x8010841C +
     * EIN SE-Wurf @0x80108424). Der Rest des Zweigs ist wurf-frei -> die Differenz zur Zeile 13
     * IST die Wurfzahl der Zerleger-Funktion. */
    {
        struct { unsigned row; uint32_t want; const char *what; } t[5] = {
            { 13u,  0u, "Referenz (Zeile 13, keine Zerleger-Zeile)" },
            {  9u,  0u, "FUN_8010640C Russ"                         },
            { 11u,  0u, "FUN_80106310 Saeure"                       },
            { 10u,  1u, "FUN_80106128 Verkohlung"                   },
            { 14u, 19u, "FUN_80106510 Spark Shot"                   },
        };
        uint32_t base = 0;
        printf("== PIN 8  RNG-Wuerfe je DEATH-Treffer (Differenz zur Referenz)\n");
        for (int i = 0; i < 5; i++) {
            reset_z(e); e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
            e->state = 3; e->sub_state_1 = (uint8_t)t[i].row; e->sub_state_2 = 0;
            e->re2z_flags21a = 2u;              /* liegend, kein Kill-Latch 0x4000 */
            uint32_t d0 = re15_re2_rand_draws();
            frame();
            uint32_t used = re15_re2_rand_draws() - d0;
            if (i == 0) base = used;
            printf("   Zeile %-2u %-42s: %u Wuerfe (Differenz %d, Soll %u)\n",
                   t[i].row, t[i].what, used, (int)used - (int)base, t[i].want);
            if (i > 0)
                CHECK(used - base == t[i].want,
                      "PIN 8: %s muss %u Wuerfe kosten; gemessen %d", t[i].what, t[i].want,
                      (int)used - (int)base);
        }
        /* Der Zerleger selbst: 3 Wuerfe (@0x801052F8 / @0x80105354 / @0x801053BC). */
        reset_z(e); e->rot_y = 0; pl->x = e->x; pl->z = e->z + 800;
        uint32_t a0 = re15_re2_rand_draws(); hit(e, 3u, 1u);
        uint32_t no_gore = re15_re2_rand_draws() - a0;
        reset_z(e); e->rot_y = 0; pl->x = e->x; pl->z = e->z + 800; e->re2z_pool152 = -1;
        uint32_t a1 = re15_re2_rand_draws(); hit(e, 3u, 1u);
        uint32_t with_gore = re15_re2_rand_draws() - a1;
        printf("   Zerleger: %u vs %u Wuerfe (Differenz %d, Soll 3)\n",
               with_gore, no_gore, (int)with_gore - (int)no_gore);
        CHECK(with_gore - no_gore == 3u,
              "PIN 8: der Zerleger-Zweig kostet 3 Wuerfe; gemessen %d",
              (int)with_gore - (int)no_gore);

        /* ⛔ und der GEGENBEWEIS zum LIEGEND-Zweig @0x80105188-284: dort liest `lbu v1,5(s1)`
         * @0x80105188 NACH dem Wort-Store `sw v1,4(s1)` @0x80105184 (v1 = 0x00060501, also
         * +0x5 = 5) — die Leiter kann dort NIE zuenden. Soll: 0 Zusatzwuerfe, auch mit Zeile 14. */
        reset_z(e); e->re2z_flags21a = 2u; e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
        uint32_t b0 = re15_re2_rand_draws(); hit(e, 13u, 1u);
        uint32_t lie13 = re15_re2_rand_draws() - b0;
        reset_z(e); e->re2z_flags21a = 2u; e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
        uint32_t b1 = re15_re2_rand_draws(); hit(e, 14u, 1u);
        uint32_t lie14 = re15_re2_rand_draws() - b1;
        printf("   Liegend-Zweig (toter Code): Zeile 13 = %u, Zeile 14 = %u Wuerfe (Soll gleich)\n",
               lie13, lie14);
        CHECK(lie13 == lie14,
              "PIN 8: der Liegend-Zweig darf NICHTS zuenden — +0x5 ist dort durch `sw v1,4(s1)` "
              "@0x80105184 auf 5 gesetzt (@0x80105188 liest danach)");
    }

    /* ============================ PIN 9: SAEURE und RUSS (ueber den DEATH-Einstieg) ======== */
    {
        reset_z(e); e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
        e->state = 3; e->sub_state_1 = 11; e->sub_state_2 = 0; e->re2z_flags21a = 2u;
        frame();
        printf("== PIN 9  Zeile 11 (Saeure): +0x21A=0x%04X Tinte[0]=0x%06X Tinte[9]=0x%06X\n",
               (unsigned)e->re2z_flags21a, (unsigned)e->re2z_part_tint[0],
               (unsigned)e->re2z_part_tint[9]);
        CHECK((e->re2z_flags21a & 0x1800u) == 0x1800u,
              "PIN 9: FUN_80106310 setzt `+0x21A |= 0x1800` (@0x8010632C-38)");
        CHECK(e->re2z_part_tint[0] == 0x00304040u && e->re2z_part_tint[9] == 0x00506060u &&
              e->re2z_part_tint[2] == 0x00405050u,
              "PIN 9: Aetz-Tinten @0x8010633C-84");
        CHECK(e->re2z_part_tint[10] == 0u && e->re2z_part_tint[13] == 0u,
              "PIN 9: FUN_80106310 tintet NUR Part 0/1/2/5/8/9/12");

        reset_z(e); e->rot_y = 0; pl->x = e->x - 900; pl->z = e->z;
        e->state = 3; e->sub_state_1 = 9; e->sub_state_2 = 0; e->re2z_flags21a = 2u;
        frame();
        printf("   Zeile 9 (Russ): +0x10E=0x%04X Tinte[0]=0x%06X Tinte[8]=0x%06X "
               "Tinte[10]=0x%06X\n",
               (unsigned)e->re2z_f10e, (unsigned)e->re2z_part_tint[0],
               (unsigned)e->re2z_part_tint[8], (unsigned)e->re2z_part_tint[10]);
        CHECK((e->re2z_f10e & 0x80u) == 0u,
              "PIN 9: FUN_8010640C setzt das +0x10E-Bit NICHT (nur FUN_80106128 @0x80106144)");
        CHECK(e->re2z_part_tint[0] == 0x00404040u && e->re2z_part_tint[10] == 0x00484848u &&
              e->re2z_part_tint[8] == 0u,
              "PIN 9: Russ-Tinten @0x8010648C-F8 — Part 8 (Kopf) bleibt UNGETINTET, genau das "
              "unterscheidet FUN_8010640C von FUN_80106128");
    }

    free(buf);
    printf(fails ? "test_re2_gore: %d FAIL\n" : "test_re2_gore: OK (%d)\n", fails);
    return fails ? 1 : 0;
}
