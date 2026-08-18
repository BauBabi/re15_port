/**
 * @file test_re2_gore_render.c
 * @brief PIN — die ANZEIGE-BRUECKE des RE2-Modellblocks +0x198 (Welle F).
 *
 * Bis Welle E trug der Port den Gore-Zustand byte-true in re2z_part_flags[]/re2z_part_tint[],
 * hatte aber KEINEN Konsumenten — abgerissene Teile blieben sichtbar, Verkohlung/Saeure
 * unsichtbar. Dieser Test pinnt den Zwilling des RE2-Part-Draw-Walks
 * FUN_80027160 @0x80027160 (ghidra_re2_Leon.txt Z.108178-108432), also GENAU das, was der
 * Renderer in platform/pc/main.c aus dem Zustand macht — nicht nur die Flags selbst.
 *
 *  PIN 1 RE1.5 UNBERUEHRT  re15_re2z_gore_active()/_resolve() liefern im RE1.5-Flavor 0 und
 *                          lassen die Ausgabefelder unangetastet. Ohne diesen Gate wuerde der
 *                          Renderer im RE1.5-Pfad JEDES Mesh verwerfen (die Flags sind dort 0).
 *  PIN 2 SICHTBARKEIT      Bit 0 des Flagworts +0x00 ist DRAW ENABLE:
 *                            80027374: lw   a2,0x0(s2)
 *                            8002737c: andi v0,a2,0x1
 *                            80027380: beq  v0,zero,0x80027394     (Zwilling @0x800273C4-C8)
 *                          Nach dem Arm-Abriss (`sw zero,688(v0)` @0x80107588 = Part 4 = 0)
 *                          darf Mesh 4 NICHT mehr gezeichnet werden — und es traegt echte
 *                          Geometrie, das Verschwinden ist also sichtbar.
 *  PIN 3 FLACH, NICHT BAUM Der Record-Zeiger laeuft UNBEDINGT im Delay-Slot weiter
 *                          (`_addiu s2,s2,0xac` @0x800273A4/@0x8010740C) — es gibt keinen
 *                          Matrix-Stack. Ein geloeschtes Bit 0 nimmt die Kinder NICHT mit.
 *  PIN 4 DIE KASKADE       Die Kette gibt es als FLAG-Vererbung im Kopf beider Zeichner
 *                          (FUN_80027434 @0x80027470-94, wortgleich FUN_80027ff0 @0x80028010-34):
 *                            80027470: lw   v0,0x94(s1)   ; Eltern-Record
 *                            80027478: lw   v0,0x0(v0)    ; Eltern-Flagwort
 *                            80027480: andi v1,v0,0x21
 *                            80027484: li   v0,0x20
 *                            80027488: bne  v1,v0,0x80027498
 *                            80027490: j    0x80027bb8    ; nichts zeichnen
 *                            80027494: _sw  v1,0x0(s1)    ; eigenes Flagwort := 0x20
 *                          -> Eltern mit (0x21) == 0x20 loeschen die GANZE noch aktive Kette.
 *  PIN 5 TINTE             Farbwort +0x70, neutral 0x00808080 (FUN_80028368.c:55). Es geht per
 *                          `ldrgb` @0x80027C2C in das GTE-RGB-Register und wird von NCCT
 *                          (@0x80027D10) mit dem Beleuchtungsergebnis MULTIPLIZIERT. Der
 *                          neutrale Seed MUSS 0x808080 sein, sonst schwaerzt die Bruecke jedes
 *                          unversehrte Mesh (Modulation im PSX-Primitivraum: prim*tint/0x80).
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

/* EIN Treffer der Zeile `row` in Spalte `col` (wie re15_re2_stamp_hit: +0x5/+0x1D2/+0x6=0). */
static void hit(re15_actor_t *e, unsigned row, unsigned col)
{
    e->state = 2; e->sub_state_1 = (uint8_t)row; e->sub_state_2 = 0;
    e->re2z_hits1d2 = (uint8_t)col;
    e->re2z_res223 = 20;
    frame();
}

/* Wieviel GEOMETRIE haengt an diesem Mesh? (Der Beweis, dass ein verworfener Part wirklich
 * etwas vom Bild nimmt und nicht nur ein leerer Slot ist.) */
static int mesh_prims(const re15_enemy_bank_t *b, int mi)
{
    if (mi < 0 || mi >= b->md1.mesh_count) return 0;
    return b->md1.meshes[mi].triangle_count + b->md1.meshes[mi].quad_count;
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
    frame();                                        /* INIT-Tick: seedet den Modellblock */

    const int nb = bank->skel.bone_count < bank->md1.mesh_count
                 ? bank->skel.bone_count : bank->md1.mesh_count;
    printf("== RE2-Bank EM010: %d Knochen, %d Meshes -> Renderschleife laeuft ueber %d\n",
           bank->skel.bone_count, bank->md1.mesh_count, nb);
    printf("   Elternkette:");
    for (int i = 0; i < bank->skel.bone_count; i++)
        printf(" %d<-%d", i, (int)bank->skel.bone_parent[i]);
    printf("\n");

    uint8_t  draw[RE15_EMD_MAX_BONES];
    uint32_t tint[RE15_EMD_MAX_BONES];
    uint8_t  mesh[RE15_EMD_MAX_BONES];

    /* ================================================== PIN 1: RE1.5 BLEIBT UNBERUEHRT ==== */
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        memset(draw, 0xAB, sizeof draw); memset(tint, 0xCD, sizeof tint);
        memset(mesh, 0xEF, sizeof mesh);
        int on   = re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
        int live = re15_re2z_gore_active(e);
        printf("== PIN 1  RE1.5-Flavor: active=%d resolve=%d (Ausgabe unberuehrt: %s)\n",
               live, on, (draw[0] == 0xAB && tint[0] == 0xCDCDCDCDu) ? "ja" : "NEIN");
        CHECK(live == 0, "re15_re2z_gore_active() muss im RE1.5-Flavor 0 sein, ist %d", live);
        CHECK(on == 0, "re15_re2z_gore_resolve() muss im RE1.5-Flavor 0 liefern, ist %d", on);
        CHECK(draw[0] == 0xAB && tint[0] == 0xCDCDCDCDu && mesh[0] == 0xEF,
              "der RE1.5-Pfad darf die Renderer-Ausgabefelder NICHT anfassen "
              "(draw[0]=0x%02X tint[0]=0x%08X mesh[0]=0x%02X)", draw[0], tint[0], mesh[0]);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    }

    /* ================================================== PIN 5a: NEUTRALER SEED ============ */
    {
        int on = re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
        CHECK(on == 1, "im RE2-Flavor muss die Bruecke nach dem INIT aktiv sein (resolve=%d)", on);
        int all_drawn = 1, all_neutral = 1;
        for (int i = 0; i < nb; i++) {
            if (!draw[i]) all_drawn = 0;
            if (tint[i] != 0x00808080u) all_neutral = 0;
        }
        printf("== PIN 5a unversehrt: alle %d Parts gezeichnet=%d, alle Tinten 0x808080=%d "
               "(tint[0]=0x%06X)\n", nb, all_drawn, all_neutral, tint[0] & 0xFFFFFFu);
        CHECK(all_drawn, "ein unversehrter Zombie muss ALLE Parts zeichnen "
                         "(Bit-0-Seed @re15_re2z_part_reset)");
        CHECK(all_neutral,
              "der Farbwort-Seed muss 0x00808080 sein (FUN_80028368.c:55) — sonst schwaerzt die "
              "NCCT-Modulation prim*tint/0x80 jedes unversehrte Mesh");
        /* Die Modulation mit dem Neutralwert ist die Identitaet — das ist der Grund, warum PIN 1
         * + PIN 5a zusammen garantieren, dass unversehrte Modelle EXAKT wie vorher aussehen. */
        int identity = 1;
        for (int v = 0; v <= 255; v++) if (((v * 0x80) >> 7) != v) identity = 0;
        CHECK(identity, "prim*0x80/0x80 ist nicht die Identitaet — die Modulationsformel stimmt nicht");
    }

    /* ================================================== PIN 3+4: FLACH vs. KASKADE ======== */
    {
        /* Die Anatomie, auf der beide Faelle ruhen (Zerleger-Offsets 1548/1720 = Part 9/10). */
        CHECK(nb > 11 && bank->skel.bone_parent[10] == 9 && bank->skel.bone_parent[11] == 10,
              "erwartet wird die Kette 9 -> 10 -> 11 (rechtes Bein), gemessen 10<-%d 11<-%d",
              nb > 10 ? (int)bank->skel.bone_parent[10] : -99,
              nb > 11 ? (int)bank->skel.bone_parent[11] : -99);

        uint16_t save[16];
        memcpy(save, e->re2z_part_flags, sizeof save);

        /* (3) Bit 0 allein geloescht -> NUR dieser Part faellt weg, die Kinder bleiben. */
        e->re2z_part_flags[9] = 0u;
        re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
        printf("== PIN 3  Part 9 = 0 (nur Bit 0 weg): draw[9]=%d draw[10]=%d draw[11]=%d\n",
               draw[9], draw[10], draw[11]);
        CHECK(draw[9] == 0, "Part 9 mit Bit 0 == 0 darf nicht gezeichnet werden (@0x8002737C-80)");
        CHECK(draw[10] == 1 && draw[11] == 1,
              "der Walk ist FLACH (`_addiu s2,s2,0xac` @0x800273A4 im Delay-Slot) — ohne Bit 5 "
              "duerfen die Kinder NICHT mitverschwinden (draw[10]=%d draw[11]=%d)",
              draw[10], draw[11]);

        /* (4) Bit 5 gesetzt UND Bit 0 geloescht -> die ganze Kette darunter faellt weg. */
        e->re2z_part_flags[9] = 0x20u;
        re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
        printf("== PIN 4  Part 9 = 0x20 (Kaskade): draw[9]=%d draw[10]=%d draw[11]=%d "
               "| Gegenbein draw[12]=%d draw[13]=%d\n",
               draw[9], draw[10], draw[11], draw[12], draw[13]);
        CHECK(draw[9] == 0 && draw[10] == 0 && draw[11] == 0,
              "(Eltern & 0x21) == 0x20 muss die GANZE Kette schneiden (@0x80027480-94): "
              "draw[9]=%d draw[10]=%d draw[11]=%d", draw[9], draw[10], draw[11]);
        CHECK(draw[12] == 1 && draw[13] == 1,
              "die Kaskade darf NUR die eigene Kette treffen — das andere Bein bleibt "
              "(draw[12]=%d draw[13]=%d)", draw[12], draw[13]);

        memcpy(e->re2z_part_flags, save, sizeof save);
    }

    /* ================================================== PIN 2: DER ARM-ABRISS IM BILD ===== */
    {
        int torn = 0;
        for (int t = 0; t < 40 && !torn; t++) {
            e->hp = 90; e->re2z_flags21a = 0; e->re2z_f10e = 0; e->re2z_self1d3 = 0;
            hit(e, 7u, 0u);                          /* Zeile 7 / Spalte 0 -> 0x80107438 */
            if (e->re2z_part_flags[4] == 0u) torn = 1;
        }
        CHECK(torn, "der Arm-Abriss (`sw zero,688` @0x80107588) ist in 40 Versuchen nie gefallen "
                    "— der Rest dieses PINs misst dann nichts");
        if (torn) {
            re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
            int prims4 = mesh_prims(bank, 4);
            int gone = 0, total = 0;
            for (int i = 0; i < nb; i++) { total += mesh_prims(bank, i); if (!draw[i]) gone += mesh_prims(bank, i); }
            printf("== PIN 2  Arm ab: flags[3]=0x%04X flags[4]=0x%04X -> draw[3]=%d draw[4]=%d; "
                   "Mesh 4 = %d Prims, verworfen %d von %d\n",
                   e->re2z_part_flags[3], e->re2z_part_flags[4], draw[3], draw[4],
                   prims4, gone, total);
            CHECK(draw[4] == 0,
                  "Part 4 (rechte Hand) ist auf 0 gesetzt (@0x80107588) und darf nicht mehr "
                  "gezeichnet werden — draw[4]=%d", draw[4]);
            CHECK(prims4 > 0 && gone > 0,
                  "der verworfene Part traegt keine Geometrie (%d Prims) — dann ist der "
                  "Abriss unsichtbar und dieser PIN wertlos", prims4);
            CHECK(draw[3] == 1,
                  "Part 3 bekommt `ori 0x4A` @0x80107544 — Bit 0 BLEIBT gesetzt, das Original "
                  "zeichnet ihn weiter (als freifliegendes Teil). draw[3]=%d", draw[3]);
            CHECK(tint[3] == 0x0010104Fu,
                  "Part 3 muss die Stumpf-Tinte 0x0010104F tragen (`sw s1,628` @0x80107568), "
                  "ist 0x%08X", tint[3]);
        }
    }

    /* ================================================== PIN 5b: VERKOHLUNG IM BILD ======== */
    {
        e->hp = 90; e->re2z_flags21a = 0; e->re2z_f10e = 0; e->re2z_self1d3 = 0;
        for (int i = 0; i < 16; i++) {
            e->re2z_part_flags[i] = (i < 15) ? 1u : 0u;
            e->re2z_part_tint[i]  = 0x00808080u;
        }
        hit(e, 10u, 1u);                             /* Zeile 10 = GL Brand -> FUN_80106128 */
        re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
        printf("== PIN 5b Verkohlung: tint[0]=0x%06X tint[2]=0x%06X tint[8]=0x%06X "
               "tint[4]=0x%06X (unbetroffen)\n",
               tint[0] & 0xFFFFFFu, tint[2] & 0xFFFFFFu, tint[8] & 0xFFFFFFu,
               tint[4] & 0xFFFFFFu);
        CHECK(tint[0] == 0x00404040u, "Part 0 Verkohlung 0x404040 (`sw a1,112` @0x8010627C), "
                                      "ist 0x%08X", tint[0]);
        CHECK(tint[2] == 0x00707070u, "Part 2 Verkohlung 0x707070 (`sw a0,456` @0x80106294), "
                                      "ist 0x%08X", tint[2]);
        CHECK(tint[8] == 0x00505050u, "Part 8 Verkohlung 0x505050 (`sw v1,1488` @0x801062AC), "
                                      "ist 0x%08X", tint[8]);
        CHECK(tint[4] == 0x00808080u,
              "FUN_80106128 schreibt KEINE Tinte auf Part 4 — der muss neutral bleiben, "
              "ist 0x%08X", tint[4]);
        /* Das Ergebnis am Renderer: 0x40 auf einem voll ausgeleuchteten Vertex (0x80) ergibt
         * 0x20 — halb so hell, nicht schwarz und nicht ersetzt (NCCT-Modulation @0x80027D10). */
        int lit = 0x80, out = (lit * (int)(tint[0] & 0xFFu)) >> 7;
        CHECK(out == 0x40, "Modulation 0x80 x 0x40 / 0x80 muss 0x40 ergeben, ist 0x%02X", out);
    }

    /* ================================================== PIN 6: DAS STUMPF-MESH ============ */
    {
        /* Der Zerleger tauscht in 50 % der Faelle die vier GEOMETRIE-Woerter des Reserve-Parts
         * 15 in den Oberschenkel (`lw 2588/2592/2596/2600(v1)` -> `sw 8/12/16/20(s0)`
         * @0x8010531C-50; 2588.. = 15*172 + 8/12/16/20). Im Port ist der Zwilling der
         * MD1-Objektindex: Part 12 zeichnet danach Mesh 15 statt Mesh 12.
         * Voraussetzung, dass das ueberhaupt darstellbar ist: die RE2-Bank MUSS mehr Meshes
         * als Knochen haben — Mesh 15 ist genau der Reserve-Part. */
        printf("== PIN 6  Reserve-Mesh 15: %d Prims (Oberschenkel-Mesh 12: %d Prims)\n",
               mesh_prims(bank, 15), mesh_prims(bank, 12));
        CHECK(bank->md1.mesh_count > 15 && mesh_prims(bank, 15) > 0,
              "die RE2-Bank hat keinen Reserve-Part 15 mit Geometrie (%d Meshes, Mesh15 %d Prims) "
              "— dann ist der Stumpf-Tausch @0x8010531C-50 nicht darstellbar",
              bank->md1.mesh_count, mesh_prims(bank, 15));

        int swapped = 0, tries = 0;
        for (; tries < 40 && !swapped; tries++) {
            /* frischer Zombie: Modellblock wie nach dem INIT, Riegel +0x21A & 0x60 offen */
            e->state = 2; e->sub_state_2 = 0; e->hp = 90; e->hit_react = 0;
            e->re2z_res223 = 20; e->re2z_flag222 = 0; e->re2z_flags21a = 0; e->re2z_f10e = 0;
            e->re2z_self1d3 = 0; e->re2z_t158 = 0; e->re2z_t15a = 0; e->re2z_dir16a = 0;
            for (int i = 0; i < 16; i++) {
                e->re2z_part_flags[i] = (i < 15) ? 1u : 0u;
                e->re2z_part_tint[i]  = 0x00808080u;
                e->re2z_part_mesh[i]  = (uint8_t)i;
            }
            e->rot_y = 0; pl->x = e->x; pl->z = e->z + 800;   /* Seitentreffer -> 0x80 = links */
            e->re2z_pool152 = -1;                             /* Zonen-Reserve leer */
            hit(e, 3u, 1u);
            if (e->re2z_part_mesh[12] == 15u) swapped = 1;
        }
        re15_re2z_gore_resolve(e, bank->skel.bone_parent, nb, draw, tint, mesh);
        printf("   nach %d Versuchen: Stumpf=%d, mesh[12]=%d mesh[9]=%d "
               "(Part 13 Flags=0x%04X)\n",
               tries, swapped, mesh[12], mesh[9], e->re2z_part_flags[13]);
        CHECK(swapped, "der Stumpf-Tausch (`rand & 1` @0x801052F8) ist in 40 Versuchen nie "
                       "gefallen — der Rest dieses PINs misst nichts");
        if (swapped) {
            CHECK(mesh[12] == 15,
                  "Part 12 muss nach dem Tausch die Geometrie des Reserve-Parts 15 zeichnen "
                  "(@0x8010531C-50), zeigt aber auf Mesh %d", mesh[12]);
            CHECK(mesh[9] == 9,
                  "nur der getroffene Oberschenkel wird getauscht — Part 9 muss bei seinem "
                  "eigenen Mesh bleiben, ist %d", mesh[9]);
            /* Die Prim-ZAHL taugt hier nicht als Unterscheidung (beide 30) — verglichen wird
             * die GEOMETRIE selbst, sonst belegt der PIN nicht, dass sich das Bild aendert. */
            const re15_md1_mesh_t *m12 = &bank->md1.meshes[12];
            const re15_md1_mesh_t *m15 = &bank->md1.meshes[15];
            int diff = (m12->quad_vertex_count != m15->quad_vertex_count) ||
                       (m12->tri_vertex_count  != m15->tri_vertex_count);
            int nv = m12->quad_vertex_count < m15->quad_vertex_count
                   ? m12->quad_vertex_count : m15->quad_vertex_count;
            for (int v = 0; v < nv && !diff; v++)
                if (m12->quad_vertices[v].x != m15->quad_vertices[v].x ||
                    m12->quad_vertices[v].y != m15->quad_vertices[v].y ||
                    m12->quad_vertices[v].z != m15->quad_vertices[v].z) diff = 1;
            printf("   Mesh12 verts %d/%d  Mesh15 verts %d/%d  -> Geometrie verschieden=%d\n",
                   m12->tri_vertex_count, m12->quad_vertex_count,
                   m15->tri_vertex_count, m15->quad_vertex_count, diff);
            CHECK(diff,
                  "Reserve-Mesh 15 und Oberschenkel-Mesh 12 sind geometrisch IDENTISCH — dann "
                  "belegt dieser PIN nicht, dass der Tausch das Bild aendert");
        }
    }

    free(buf);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(fails ? "test_re2_gore_render: %d FAIL\n" : "test_re2_gore_render: OK\n", fails);
    return fails ? 1 : 0;
}
