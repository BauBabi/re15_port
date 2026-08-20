/**
 * @file test_re2_gore_fly.c
 * @brief PIN — DAS FREIFLIEGENDE TEIL (Welle G): die zwei Part-Physiken des RE2-Modellblocks.
 *
 * Bis Welle F trugen die abgerissenen Teile zwar ihr Flagwort (0x4A Arm / 0x1062 Bein), blieben
 * aber am Skelett haengen: der Port hatte keine Part-Physik. Damit fehlte auch der
 * KASKADEN-AUSLOESER — Bit 0 faellt im Original erst beim Aufschlag.
 *
 * Alles hier ist gegen die RE2-EXE disassembliert (info/re2leon/PSX.EXE):
 *   FUN_80028AD8 @0x80028AD8   Wurf mit Aufschlag  (Bein, ueber Flagbit 0x20 @0x800276A0)
 *   FUN_80028DAC @0x80028DAC   Drift mit Ablauf    (Arm,  ueber Flagbit 0x08 @0x80027B98)
 *   FUN_80027434 @0x80027498   `andi v0,s3,0x40` = "eigene Matrix, Eltern ueberspringen"
 *
 *  PIN 1 RE1.5 UNBERUEHRT   re15_re2z_gore_part_matrix() liefert im RE1.5-Flavor 0 und laesst
 *                           rot/trans unangetastet (dasselbe Dreifach-Gate wie _gore_active).
 *  PIN 2 NUR MIT BIT 0x40   Ein normaler Part (Flagwort 1) liefert 0 — die Skelett-Pose bleibt.
 *  PIN 3 DIE WURF-FELDER    Der Zerleger schreibt sie @0x80105384-C0:
 *                             8010538c: sh v0,60(s0)    ; +0x3C = 10
 *                             80105394: sh v0,58(s0)    ; +0x3A = -200
 *                             8010539c: sh zero,134(s0) ; +0x86 = 0   (loest den INIT aus)
 *                             801053a0: sb v0,121(s0)   ; +0x79 = 50  (Gravitation)
 *                             801053a4: sh zero,62(s0)  ; +0x3E = 0
 *                             801053ac: sh v1,56(s0)    ; +0x38 = +0x76 + 2048
 *                             801053b8: sh v0,66(s0)    ; +0x42 = 1024
 *                             801053c0: sh v1,64(s0)    ; +0x40 = +0x76
 *  PIN 4 DER WURF-INIT      `lh v0,134` == 0 @0x80028AF4 -> RotMatrix(0,+0x38,0) @0x80028B20 +
 *                           ApplyMatrixSV((+0x3C,+0x3A,0)) ZURUECK nach +0x38/+0x3A/+0x3C
 *                           (`addiu a2,s1,56` @0x80028B3C), dann +0x86 = 3 (@0x80028B4C) und
 *                           +0x7A = 15 (@0x80028B54).
 *  PIN 5 DIE FLUGBAHN       +0x3A += +0x79 @0x80028BA4, t[1] += +0x3A @0x80028BB8, t[0]/t[2] mit
 *                           +0x38/+0x3C @0x80028B8C/@0x80028BCC.
 *  PIN 6 AUFSCHLAG+KASKADE  `slt v1,s0,v1` @0x80028BD8 -> +0x86-- @0x80028BF0, t[1] = Boden
 *                           @0x80028BF4, v gedaempft/gespiegelt @0x80028BF8-C28; beim Erreichen
 *                           von +0x86 == 1: `and v0,v0,-2` @0x80028CA0 = **Bit 0 faellt**.
 *                           Das Flagwort bleibt 0x1062 -> `(Eltern & 0x21) == 0x20` @0x80027480-88
 *                           -> der FUSS verschwindet mit (Kaskade @0x80027494).
 *  PIN 7 DER ARM            +0x9A = -10 @0x8010755C, +0x9C/+0x9E/+0xA4 = 0 @0x80107558/60/64,
 *                           Lebensdauer 29 (`sltiu v0,v0,0x1d` @0x80028DCC), danach
 *                           `sw zero,0(s0)` @0x80028DDC = Flagwort 0 (KEINE Kaskade, weil 0x20
 *                           mit weggeht). Kein Vortrieb -> X/Z bleiben stehen.
 *  PIN 8 EIN SCHRITT/FRAME  Im Original haengt die Physik am Zeichnen; der Port darf sie bei
 *                           mehrfachem Zeichnen (Schatten-Pass) trotzdem nur EINMAL je Frame
 *                           laufen lassen.
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

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb) return eb;
    eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_sz);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (s_ems && re2_ems_load_bank(s_ems, (size_t)s_ems_sz, type, eb, &tim) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static void se_sink(int id, int f2) { (void)id; (void)f2; }
static void bank_sink(int b) { (void)b; }
static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* Die Identitaets-Pose, mit der die Tests die Matrix einfrieren (der Renderer uebergibt hier
 * die Skelett-Weltpose des Knochens). */
static void ident(int32_t rot[9]) {
    rot[0]=0x1000; rot[1]=0; rot[2]=0;
    rot[3]=0; rot[4]=0x1000; rot[5]=0;
    rot[6]=0; rot[7]=0; rot[8]=0x1000;
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
    if (!bank || !bank->ok) {
        printf("FAIL: RE2-Bank EM010 fehlt (shared_assets/RE2/CDEMD0.EMS)\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot10) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot10];
    re15_re2z_audio_hook(se_sink, bank_sink);
    frame();                                     /* INIT-Tick: seedet den Modellblock */

    const int nb = bank->skel.bone_count < bank->md1.mesh_count
                 ? bank->skel.bone_count : bank->md1.mesh_count;

    int32_t rot[9], tr[3];
    uint32_t fr = 1u;
    uint8_t  draw[RE15_EMD_MAX_BONES];
    uint32_t tint[RE15_EMD_MAX_BONES];
    uint8_t  mesh[RE15_EMD_MAX_BONES];

    /* ================================================== PIN 1: RE1.5 BLEIBT UNBERUEHRT ====
     * ⛔ 2026-08-20: der Nutzer hat den Zerleger AUSDRUECKLICH auch fuer den RE1.5-KI-Modus
     * verlangt; die Port-Option re15_re15_re2z_import() (Default AN) macht das. Der
     * ORIGINALZUSTAND ist damit nicht mehr der Default — aber weiterhin erreichbar, und genau
     * er wird hier weiterhin gepinnt (Option explizit AUS). PIN 1b pinnt die Gegenrichtung. */
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        re15_re15_re2z_import_set(0);
        e->re2z_part_flags[10] = 0x1063u;                      /* wie nach dem Zerleger */
        ident(rot); tr[0] = 111; tr[1] = 222; tr[2] = 333;
        int own = re15_re2z_gore_part_matrix(e, 10, fr++, rot, tr);
        printf("== PIN 1  RE1.5-Flavor, Import AUS: own=%d trans=(%d,%d,%d)\n",
               own, tr[0], tr[1], tr[2]);
        CHECK(own == 0, "im RE1.5-Flavor OHNE Import muss die Part-Physik 0 liefern, ist %d", own);
        CHECK(tr[0] == 111 && tr[1] == 222 && tr[2] == 333 && rot[0] == 0x1000,
              "der RE1.5-Pfad OHNE Import darf rot/trans NICHT anfassen (trans=(%d,%d,%d) "
              "rot[0]=%d)", tr[0], tr[1], tr[2], rot[0]);

        /* PIN 1b — Option AN: die Physik LAEUFT, und zwar auf dem RE1.5-BONE-SLOT. Der
         * Modellblock steht in RE2-Part-Nummern; der Renderer indiziert mit Bank-Slots, also
         * uebersetzt die Bruecke ueber re2_hybrid_perm (RE2-Slot -> RE1.5-Index, hier
         * Part 10 -> Bone 9). Genau diese Permutation benutzt der Modus "RE2 AI" zum
         * Zeichnen — dadurch fliegt in beiden Modi DASSELBE sichtbare Teil weg. */
        re15_re15_re2z_import_set(1);
        {
            const int8_t *perm = NULL;
            int np = re2_hybrid_perm(0x10, &perm);
            int bone = (np > 10 && perm) ? (int)perm[10] : 10;
            CHECK(bone == 9, "k_perm_zombie[10] muss 9 sein (RE2-Part 10 -> RE1.5-Bone 9), ist %d",
                  bone);
            ident(rot); tr[0] = 111; tr[1] = 222; tr[2] = 333;
            int own15 = re15_re2z_gore_part_matrix(e, bone, fr++, rot, tr);
            printf("== PIN 1b RE1.5-Flavor, Import AN: Part 10 -> Bone %d, own=%d "
                   "trans=(%d,%d,%d)\n", bone, own15, tr[0], tr[1], tr[2]);
            CHECK(own15 == 1, "mit der Port-Option MUSS die Part-Physik im RE1.5-Modus laufen "
                              "(own=%d)", own15);
            /* Und der FALSCHE Slot (die ungetauschte Nummer) darf NICHT laufen — das ist der
             * Beweis, dass die Uebersetzung wirklich greift und nicht zufaellig passt. */
            ident(rot); tr[0] = 111; tr[1] = 222; tr[2] = 333;
            int own_wrong = re15_re2z_gore_part_matrix(e, 10, fr++, rot, tr);
            CHECK(own_wrong == 0, "Bone-Slot 10 gehoert im RE1.5-Rig zu RE2-Part 11 und darf "
                                  "keine eigene Matrix haben (own=%d)", own_wrong);
        }
        e->re2z_part_flags[10] = 1u;
        e->re2z_part_seeded = 0u; e->re2z_part_stepped = 0u;   /* die PIN-1b-Schritte zuruecknehmen */
        memset(e->re2z_part_v,   0, sizeof e->re2z_part_v);
        memset(e->re2z_part_t,   0, sizeof e->re2z_part_t);
        memset(e->re2z_part_m,   0, sizeof e->re2z_part_m);
        memset(e->re2z_part_st86,0, sizeof e->re2z_part_st86);
        for (int i = 0; i < 16; i++) e->re2z_part_blend[i] = -1;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    }

    /* ================================================== PIN 2: OHNE 0x40 KEINE EIGENE MATRIX */
    {
        ident(rot); tr[0] = 111; tr[1] = 222; tr[2] = 333;
        int own = re15_re2z_gore_part_matrix(e, 10, fr++, rot, tr);
        printf("== PIN 2  Part 10 Flagwort 0x%04X: own=%d\n", e->re2z_part_flags[10], own);
        CHECK(own == 0, "ohne Bit 0x40 (`andi v0,s3,0x40` @0x80027498) darf kein eigener "
                        "Matrix-Zweig laufen, own=%d", own);
        CHECK(tr[1] == 222, "trans wurde trotzdem veraendert (%d)", tr[1]);
    }

    /* ================================================== PIN 3+4+5+6: DAS BEIN =============== */
    {
        /* Zerleger scharf machen: Seiten-Treffer (+0x1D0 & 0xC0), mittlere Reserve leer,
         * +0x21A-Riegel offen. Zeile 3 / Spalte 1. rot_y = 0 macht die Zahlen pruefbar. */
        int torn = 0, tries = 0;
        for (; tries < 40 && !torn; tries++) {
            e->state = 2; e->sub_state_2 = 0; e->hp = 90; e->hit_react = 0;
            e->re2z_res223 = 20; e->re2z_flag222 = 0; e->re2z_flags21a = 0; e->re2z_f10e = 0;
            e->re2z_self1d3 = 0; e->re2z_t158 = 0; e->re2z_t15a = 0; e->re2z_dir16a = 0;
            for (int i = 0; i < 16; i++) {
                e->re2z_part_flags[i] = (i < 15) ? 1u : 0u;
                e->re2z_part_tint[i]  = 0x00808080u;
                e->re2z_part_mesh[i]  = (uint8_t)i;
            }
            e->re2z_part_seeded = 0; e->re2z_part_stepped = 0; e->re2z_part_frame = 0;
            e->rot_y = 0; pl->x = e->x; pl->z = e->z + 800;   /* Seitentreffer -> 0x80 = links */
            e->re2z_pool152 = -1;
            e->state = 2; e->sub_state_1 = 3; e->sub_state_2 = 0; e->re2z_hits1d2 = 1;
            frame();
            if (e->re2z_part_flags[13] & 0x1000u) torn = 1;   /* linkes Schienbein abgesprengt */
        }
        CHECK(torn, "der Bein-Zerleger (@0x8010537C) ist in %d Versuchen nie gefallen", tries);
        if (torn) {
            const int shin = 13, foot = 14;
            printf("== PIN 3  Schienbein %d Flagwort 0x%04X | +0x38=%d +0x3A=%d +0x3C=%d "
                   "+0x3E=%d +0x40=%d +0x42=%d +0x79=%d +0x86=%d\n",
                   shin, e->re2z_part_flags[shin],
                   e->re2z_part_v[shin][0], e->re2z_part_v[shin][1], e->re2z_part_v[shin][2],
                   e->re2z_part_rot[shin][0], e->re2z_part_rot[shin][1], e->re2z_part_rot[shin][2],
                   (int)e->re2z_part_grav[shin], (int)e->re2z_part_st86[shin]);
            CHECK(e->re2z_part_flags[shin] == 0x1063u,
                  "Flagwort = 1 | 0x1062 = 0x1063 (@0x8010537C-80), ist 0x%04X",
                  e->re2z_part_flags[shin]);
            CHECK(e->re2z_part_v[shin][2] == 10,   "+0x3C = 10 (@0x8010538C), ist %d",
                  e->re2z_part_v[shin][2]);
            CHECK(e->re2z_part_v[shin][1] == -200, "+0x3A = -200 (@0x80105394), ist %d",
                  e->re2z_part_v[shin][1]);
            CHECK(e->re2z_part_v[shin][0] == 2048, "+0x38 = rot_y+2048 (@0x801053AC), ist %d",
                  e->re2z_part_v[shin][0]);
            CHECK(e->re2z_part_grav[shin] == 50,   "+0x79 = 50 (@0x801053A0), ist %d",
                  (int)e->re2z_part_grav[shin]);
            CHECK(e->re2z_part_st86[shin] == 0,    "+0x86 = 0 (@0x8010539C), ist %d",
                  (int)e->re2z_part_st86[shin]);
            CHECK(e->re2z_part_rot[shin][0] == 0 && e->re2z_part_rot[shin][1] == 0 &&
                  e->re2z_part_rot[shin][2] == 1024,
                  "+0x3E/+0x40/+0x42 = 0/rot_y/1024 (@0x801053A4/C0/B8), ist %d/%d/%d",
                  e->re2z_part_rot[shin][0], e->re2z_part_rot[shin][1], e->re2z_part_rot[shin][2]);

            /* ---- der erste Schritt: INIT + eine Frame Flug ---- */
            int32_t F = (int32_t)e->y;                        /* [PORT] Boden = Aktor-Y */
            ident(rot); tr[0] = 1000; tr[1] = F - 400; tr[2] = 2000;
            int own = re15_re2z_gore_part_matrix(e, shin, fr++, rot, tr);
            printf("== PIN 4  INIT: own=%d v=(%d,%d,%d) +0x86=%d +0x7A=%d -> trans=(%d,%d,%d)\n",
                   own, e->re2z_part_v[shin][0], e->re2z_part_v[shin][1], e->re2z_part_v[shin][2],
                   (int)e->re2z_part_st86[shin], (int)e->re2z_part_blend[shin],
                   tr[0], tr[1], tr[2]);
            CHECK(own == 1, "Bit 0x40 steht -> eigene Matrix erwartet, own=%d", own);
            /* RotY(2048) = [-1,0,0; 0,1,0; 0,0,-1] (Q12), v_in = (10,-200,0)
             *   -> v = (-10, -200, 0)  (ApplyMatrixSV @0x80028B40).
             * GEMESSEN wird NACH dem Aufruf, und derselbe Aufruf laeuft weiter in den
             * Bewegungsblock (`slti v0,v0,2` @0x80028B60 ist mit +0x86 = 3 nicht erfuellt),
             * der +0x3A sofort um +0x79 = 50 anhebt (@0x80028BA4) — der INIT und der erste
             * Flugschritt sind EIN Frame, genau wie im Original. Erwartung also -150. */
            CHECK(e->re2z_part_v[shin][0] == -10 && e->re2z_part_v[shin][1] == -150 &&
                  e->re2z_part_v[shin][2] == 0,
                  "INIT+Schritt 1: RotY(2048) x (10,-200,0) = (-10,-200,0), danach "
                  "+0x3A += 50 -> (-10,-150,0), ist (%d,%d,%d)",
                  e->re2z_part_v[shin][0], e->re2z_part_v[shin][1], e->re2z_part_v[shin][2]);
            CHECK(e->re2z_part_st86[shin] == 3,  "+0x86 = 3 nach dem INIT (@0x80028B4C), ist %d",
                  (int)e->re2z_part_st86[shin]);
            CHECK(e->re2z_part_blend[shin] == 14,
                  "+0x7A = 15 im INIT (@0x80028B54), danach ein Blend-Schritt -> 14 "
                  "(@0x80028D88), ist %d", (int)e->re2z_part_blend[shin]);
            /* Frame 1: t[0] += -10, +0x3A = -200+50 = -150, t[1] += -150 */
            CHECK(tr[0] == 990 && tr[1] == F - 550 && tr[2] == 2000,
                  "Frame 1 muss (990, Boden-550, 2000) liefern (t[0]+=+0x38 @0x80028B8C, "
                  "+0x3A+=50 @0x80028BA4, t[1]+=+0x3A @0x80028BB8), ist (%d,%d,%d)",
                  tr[0], tr[1] - F + F, tr[2]);

            /* ---- die Wurfparabel bis zum ERSTEN Aufschlag ---- */
            int contact1 = -1;
            for (int k = 2; k <= 12 && contact1 < 0; k++) {
                re15_re2z_gore_part_matrix(e, shin, fr++, rot, tr);
                if (e->re2z_part_st86[shin] == 2) contact1 = k;
            }
            printf("== PIN 5/6 erster Aufschlag in Frame %d: trans=(%d,%d[Boden %d]) "
                   "v=(%d,%d,%d) +0x86=%d\n", contact1, tr[0], tr[1], F,
                   e->re2z_part_v[shin][0], e->re2z_part_v[shin][1], e->re2z_part_v[shin][2],
                   (int)e->re2z_part_st86[shin]);
            CHECK(contact1 == 9, "die Parabel (-200, +50/Frame, Start 400 ueber dem Boden) muss "
                                 "in Frame 9 aufschlagen, war Frame %d", contact1);
            CHECK(tr[1] == F, "beim Aufschlag wird t[1] auf den Boden geklemmt "
                              "(`sw s0,24(s2)` @0x80028BF4), ist %d (Boden %d)", tr[1], F);
            CHECK(e->re2z_part_v[shin][1] == -63,
                  "vy = -(250) >> 2 = -63 (`subu`+`sra 18` @0x80028C00-10), ist %d",
                  e->re2z_part_v[shin][1]);
            CHECK(e->re2z_part_v[shin][0] == -3,
                  "vx = -10 >> 2 = -3 (`sra v1,v1,18` @0x80028C18), ist %d",
                  e->re2z_part_v[shin][0]);
            CHECK((e->re2z_part_flags[shin] & 1u) != 0,
                  "beim ERSTEN Aufschlag (+0x86 3->2) darf Bit 0 noch NICHT fallen "
                  "(`beq v1,v0` @0x80028C8C), Flagwort 0x%04X", e->re2z_part_flags[shin]);

            /* ---- der ZWEITE Aufschlag loescht Bit 0 und zuendet die Kaskade ---- */
            int died = -1;
            for (int k = 0; k < 12 && died < 0; k++) {
                re15_re2z_gore_part_matrix(e, shin, fr++, rot, tr);
                if (!(e->re2z_part_flags[shin] & 1u)) died = k;
            }
            re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
            printf("   zweiter Aufschlag nach %d weiteren Frames: Flagwort 0x%04X +0x86=%d "
                   "-> draw[%d]=%d draw[%d]=%d (Gegenbein draw[10]=%d)\n",
                   died, e->re2z_part_flags[shin], (int)e->re2z_part_st86[shin],
                   shin, draw[shin], foot, draw[foot], draw[10]);
            CHECK(died >= 0, "der zweite Aufschlag ist nie gekommen — Bit 0 faellt nie");
            CHECK(e->re2z_part_flags[shin] == 0x1062u,
                  "nach `and v0,v0,-2` @0x80028CA0 muss GENAU Bit 0 fehlen (0x1062), "
                  "ist 0x%04X", e->re2z_part_flags[shin]);
            CHECK(draw[shin] == 0, "das Schienbein darf nicht mehr gezeichnet werden, draw=%d",
                  draw[shin]);
            CHECK(draw[foot] == 0,
                  "DIE KASKADE: (0x1062 & 0x21) == 0x20 @0x80027480-88 muss den Fuss "
                  "mitnehmen (@0x80027494), draw[%d]=%d", foot, draw[foot]);
            CHECK(draw[10] == 1, "die Kaskade darf nur die eigene Kette treffen (draw[10]=%d)",
                  draw[10]);
            /* Und ab jetzt ist das Teil komplett stumm. */
            int32_t before = tr[1];
            int own2 = re15_re2z_gore_part_matrix(e, shin, fr++, rot, tr);
            CHECK(own2 == 0 && tr[1] == before,
                  "ohne Bit 0 betritt der Walk FUN_80027434 gar nicht (@0x8002737C) — "
                  "own=%d t[1]=%d(vorher %d)", own2, tr[1], before);
        }
    }

    /* ================================================== PIN 7: DER ARM ====================== */
    {
        int torn = 0, tries = 0;
        for (; tries < 60 && !torn; tries++) {
            e->hp = 90; e->hit_react = 0; e->re2z_flags21a = 0; e->re2z_f10e = 0;
            e->re2z_self1d3 = 0; e->re2z_res223 = 20;
            for (int i = 0; i < 16; i++) e->re2z_part_flags[i] = (i < 15) ? 1u : 0u;
            e->re2z_part_seeded = 0; e->re2z_part_stepped = 0; e->re2z_part_frame = 0;
            e->rot_y = 0;
            e->state = 2; e->sub_state_1 = 7; e->sub_state_2 = 0; e->re2z_hits1d2 = 0;
            frame();                                    /* Zeile 7 / Spalte 0 -> 0x80107438 */
            if (e->re2z_part_flags[4] == 0u) torn = 1;   /* Hand weg == Arm-Abriss gefallen */
        }
        CHECK(torn, "der Arm-Abriss (@0x80107588) ist in %d Versuchen nie gefallen", tries);
        if (torn) {
            printf("== PIN 7  Arm: Flagwort[3]=0x%04X +0x9A=%d +0x9C=%d +0x9E=%d +0xA4=%d "
                   "+0x98=%d +0xA0=%d\n", e->re2z_part_flags[3],
                   e->re2z_part_w9a[3], e->re2z_part_w9c[3], e->re2z_part_w9e[3],
                   e->re2z_part_wa4[3], e->re2z_part_yaw98[3], e->re2z_part_life[3]);
            CHECK(e->re2z_part_flags[3] == 0x4Bu, "Flagwort = 1 | 0x4A = 0x4B, ist 0x%04X",
                  e->re2z_part_flags[3]);
            CHECK(e->re2z_part_w9a[3] == -10, "+0x9A = -10 (@0x8010755C), ist %d",
                  e->re2z_part_w9a[3]);
            CHECK(e->re2z_part_w9c[3] == 0 && e->re2z_part_w9e[3] == 0 &&
                  e->re2z_part_wa4[3] == 0,
                  "+0x9C/+0x9E/+0xA4 = 0 (@0x80107558/60/64), ist %d/%d/%d",
                  e->re2z_part_w9c[3], e->re2z_part_w9e[3], e->re2z_part_wa4[3]);
            CHECK(e->re2z_part_yaw98[3] == 2048, "+0x98 = rot_y+2048 (@0x8010756C-74), ist %d",
                  e->re2z_part_yaw98[3]);

            ident(rot); tr[0] = 500; tr[1] = -700; tr[2] = -900;
            int own = re15_re2z_gore_part_matrix(e, 3, fr++, rot, tr);
            CHECK(own == 1, "Bit 0x40 steht (0x4B) -> eigene Matrix, own=%d", own);
            CHECK(tr[1] == -710 && tr[0] == 500 && tr[2] == -900,
                  "Frame 1: nur t[1] += -10 (Vortrieb 0) — ist (%d,%d,%d)", tr[0], tr[1], tr[2]);
            int gone = -1;
            for (int k = 2; k <= 40 && gone < 0; k++) {
                re15_re2z_gore_part_matrix(e, 3, fr++, rot, tr);
                if (e->re2z_part_flags[3] == 0u) gone = k;
            }
            printf("   Ablauf nach %d Frames: Flagwort 0x%04X trans=(%d,%d,%d) +0xA0=%d\n",
                   gone, e->re2z_part_flags[3], tr[0], tr[1], tr[2], e->re2z_part_life[3]);
            CHECK(gone == 30,
                  "die Lebensdauer ist 29 Zaehlschritte (`sltiu v0,v0,0x1d` @0x80028DCC), das "
                  "Flagwort faellt also im 30. Frame auf 0 (`sw zero,0(s0)` @0x80028DDC) — "
                  "war Frame %d", gone);
            CHECK(tr[0] == 500 && tr[2] == -900,
                  "ohne Vortrieb (+0x9C = +0xA4 = 0) darf X/Z sich nie bewegen, ist (%d,%d)",
                  tr[0], tr[2]);
            CHECK(tr[1] == -700 - 10 * gone,
                  "t[1] muss je Frame um 10 steigen (%d erwartet, ist %d)",
                  -700 - 10 * gone, tr[1]);
            uint8_t  d2[RE15_EMD_MAX_BONES]; uint32_t t2[RE15_EMD_MAX_BONES];
            uint8_t  m2[RE15_EMD_MAX_BONES];
            re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, d2, t2, m2);
            CHECK(d2[3] == 0, "mit Flagwort 0 faellt Bit 0 mit — der Arm ist weg, draw[3]=%d",
                  d2[3]);
            CHECK(d2[2] == 1,
                  "KEINE Kaskade: `sw zero,0(s0)` @0x80028DDC nimmt Bit 5 mit, also ist "
                  "(0 & 0x21) != 0x20 — der Oberarm bleibt (draw[2]=%d)", d2[2]);
        }
    }

    /* ================================================== PIN 8: EIN SCHRITT JE FRAME ========= */
    {
        for (int i = 0; i < 16; i++) e->re2z_part_flags[i] = (i < 15) ? 1u : 0u;
        e->re2z_part_seeded = 0; e->re2z_part_stepped = 0; e->re2z_part_frame = 0;
        e->re2z_part_flags[3] = 0x4Bu;                       /* Arm, wie nach @0x80107544 */
        e->re2z_part_w9a[3] = -10; e->re2z_part_w9c[3] = 0;
        e->re2z_part_w9e[3] = 0;   e->re2z_part_wa4[3] = 0;
        e->re2z_part_yaw98[3] = 0; e->re2z_part_life[3] = 0;
        ident(rot); tr[0] = 0; tr[1] = 0; tr[2] = 0;
        uint32_t f8 = fr++;
        re15_re2z_gore_part_matrix(e, 3, f8, rot, tr);       /* Haupt-Zeichnung */
        int32_t after1 = tr[1];
        re15_re2z_gore_part_matrix(e, 3, f8, rot, tr);       /* Schatten-Pass, SELBER Frame */
        re15_re2z_gore_part_matrix(e, 3, f8, rot, tr);
        printf("== PIN 8  drei Aufrufe im selben Frame: t[1]=%d (nach dem ersten %d), "
               "+0xA0=%d\n", tr[1], after1, e->re2z_part_life[3]);
        CHECK(tr[1] == after1 && after1 == -10,
              "die Physik darf je Frame nur EINMAL laufen (t[1]=%d, nach dem ersten Aufruf %d)",
              tr[1], after1);
        CHECK(e->re2z_part_life[3] == 1,
              "auch die Lebensdauer darf nur einmal zaehlen, ist %d", e->re2z_part_life[3]);
        re15_re2z_gore_part_matrix(e, 3, fr++, rot, tr);     /* naechster Frame -> ein Schritt */
        CHECK(tr[1] == -20 && e->re2z_part_life[3] == 2,
              "im naechsten Frame muss genau ein Schritt folgen (t[1]=%d, +0xA0=%d)",
              tr[1], e->re2z_part_life[3]);
    }

    free(buf);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(fails ? "test_re2_gore_fly: %d FAIL\n" : "test_re2_gore_fly: OK\n", fails);
    return fails ? 1 : 0;
}
