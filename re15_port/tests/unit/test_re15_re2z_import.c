/**
 * @file test_re15_re2z_import.c
 * @brief PIN — die PORT-OPTION "RE2-ZOMBIE-UEBERNAHME IM RE1.5-MODUS".
 *
 * ⛔ DAS HIER IST KEIN byte-true RE1.5-VERHALTEN, sondern eine AUSDRUECKLICH vom Nutzer
 * verlangte Abweichung vom Original (2026-08-20, woertlich):
 *   "Bei RE2 AI und RE2 AI + Model ist es so, dass auch mit der Handfeuerwaffe Gliedmassen
 *    abgeschossen werden koennen. Bei RE1.5 AI noch nicht. Und genau das, sowie die
 *    Schadenswerte fuer Zombies, moechte ich auch in RE1.5 AI haben."
 * Der Schalter heisst re15_re15_re2z_import() (Default AN, zurueck mit
 * RE15_RE15_RE2Z_IMPORT=0). Alles UNTER dem Schalter ist byte-true zu RE2 — es sind exakt
 * dieselben Tabellen, Gates und Zuege wie im RE2-Modus; nur die AUSWAHL ist eine
 * Port-Entscheidung. Die byte-true RE1.5-Originalwerte bleiben ueber den AUS-Zustand des
 * Schalters gepinnt (hier Abschnitt 5, und in test_re2_hp_model.c Abschnitt 1/5/6).
 *
 * DIE BELEGKETTE DES ZERLEGERS (alles im RE2-Zombie-Overlay EMOVL10_S0.BIN):
 *   Applier-Stempel   +0x1D0 = Trefferrichtung   @0x80041A0C-84
 *                     +0x151/+0x152/+0x153 = Zonen-Reserve, Start 13 @0x8010081C-28,
 *                     Kosten = (w1 >> Bracket*3) & 7 aus 0x800A412C @0x80041954-70
 *   Zerleger @0x80105288-3D8, Dreifach-Gate:
 *       +0x21A & 0x60 == 0    @0x80105288-8C   (Einweg)
 *       (s8)+0x152 < 0        @0x80105294-9C   (mittlere Reserve leer)
 *       +0x1D0 & 0xC0 != 0    @0x801052A4-B0   (Seitentreffer)
 *     danach SE 9 @0x801052B4, Seite ueber +0x1D0 & 0x80 (@0x801052C8), Stumpf-Wurf
 *     @0x801052F8, Emitter @0x80105368/@0x801053CC, `flags[shin] |= 0x1062` @0x8010537C,
 *     Wurf-Felder @0x80105384-C0.  MACHT GENAU DREI RNG-WUERFE.
 *   Pistole = RE1.5-Waffe 3 -> RE2-Zeile 3 (re2z_row_from_weapon) -> w1 = 0x02851014 ->
 *     Bracket 0 -> Kosten 4; Region = +0x1D2 % 3 = 1 % 3 = 1 = +0x152.
 *     => 13 -> 9 -> 5 -> 1 -> -1 : ab dem VIERTEN Pistolentreffer ist das Reserve-Gate offen.
 *
 * DIE PART-NUMMERIERUNG: der Modellblock steht in RE2-Part-Nummern, die geladene Bank ist im
 * RE1.5-Modus das RE1.5-Rig. Umgerechnet wird mit der BEREITS VORHANDENEN, im Modus "RE2 AI"
 * (RE2-Gehirn + RE1.5-Modelle) vom Nutzer visuell abgenommenen Hybrid-Permutation
 * k_perm_zombie (re2_hybrid_perm, re2_ems.c). Dadurch fliegt im RE1.5-Modus GARANTIERT
 * dasselbe sichtbare Teil weg wie im Modus "RE2 AI".
 *
 * ABSCHNITTE
 *   1  Der Schalter selbst (Default, Setter, Besitzmenge)
 *   2  Die Zonen-Reserve laeuft byte-true herunter (13/9/5/1/-1) — echter Schuss-Weg
 *   3  DER ABRISS: 4. Pistolentreffer von der SEITE -> Flagwort 0x1062, 3 RNG-Wuerfe
 *   4  Die Anzeige-Bruecke: Teil + Kind verschwinden; Permutation Part 10 -> Bone 9
 *   5  NEGATIV: Option AUS -> kein einziges re2z-Feld wird angefasst (Originalzustand)
 *   6  NEGATIV: frontal (kein 0xC0-Bit) -> kein Abriss, obwohl die Reserve leer ist
 *   7  NEGATIV: toedlicher Treffer -> kein Abriss (der Zerleger sitzt im HURT)
 *   8  ABGRENZUNG: Hund 0x20 / Kraehe 0x21 / Spinne 0x25 bleiben voellig unberuehrt
 *   9  ABGRENZUNG: der RE2-Modus aendert sich nicht (die Bruecke ist dort ein No-op)
 *  10  DIE GRENZE: der RE1.5-MD1 hat KEIN Stumpf-Mesh 15 (gemessen, nicht behauptet)
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
#include "re15_ems.h"
#include "re2_ems.h"
#include "re15_room.h"
#include "re15_skeleton.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

extern uint32_t re15_re2_rand_draws(void);
extern int16_t  re15_atan2_q12(int32_t dz, int32_t dx);

static uint8_t *s_buf = NULL; static long s_sz = 0;

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = sz; return b;
}

static int bringup(int flavor, int import_on)
{
    re15_rdt_t rdt;
    if (re15_rdt_parse(s_buf, (size_t)s_sz, &rdt) != 0) return -1;
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_re2_damage_model_set(1);
    re15_re15_re2z_import_set(import_on);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
    if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->x = 0; pl->y = 0; pl->z = 0;
    for (int i = 0; i < 30; i++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40) return s;
    return -1;
}

/* EIN Ziel scharfmachen, alle anderen aus. HP hoch genug, dass der Treffer nicht toetet. */
static re15_actor_t *arm(int slot, uint8_t type, int hp)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    e->active = 1; e->type = type;
    re15_enemy_apply_hitbox(e, type);
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = (int16_t)hp; e->hit_react = 0; e->hit_stun = 0;
    e->grid_id = (uint8_t)(e->grid_id & 0x7f);
    e->aim_band = 2; e->anim_flags = 0; e->re2_hp_stamped = 1;
    e->rot_y = 0;
    return e;
}

/* Peilung Spieler->Ziel in der Rechnung des Appliers (@0x800419D8-A08). */
static int bearing_to(const re15_actor_t *e, const re15_actor_t *pl)
{
    return ((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 1024) & 0xfff;
}

/* Spieler so um das Ziel setzen, dass +0x1D0 das gewuenschte Bit bekommt (Fenstermitten aus
 * @0x80041A0C-84: 0x20 -> d=0, 0x40 -> d=3072, 0x80 -> d=1024, frontal -> d=2048). */
static void place(re15_actor_t *e, int32_t back, unsigned want)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int target = (want == 0x40u) ? 3072 : (want == 0x80u) ? 1024 : (want == 0x20u) ? 0 : 2048;
    int best = 0, best_err = 1 << 30;
    for (int k = 0; k < 64; k++) {
        int a = (k * 64) & 0xfff;
        re15_actor_t probe = *pl;
        probe.x = e->x + (int32_t)(((int64_t)re15_cos_q12(a) * back) >> 12);
        probe.z = e->z + (int32_t)(((int64_t)re15_sin_q12(a) * back) >> 12);
        int d = (bearing_to(e, &probe) - ((int)e->rot_y & 0xfff)) & 0xfff;
        int err = (d - target) & 0xfff;
        if (err > 2048) err = 4096 - err;
        if (err < best_err) { best_err = err; best = a; }
    }
    pl->x = e->x + (int32_t)(((int64_t)re15_cos_q12(best) * back) >> 12);
    pl->z = e->z + (int32_t)(((int64_t)re15_sin_q12(best) * back) >> 12);
    pl->y = e->y;
    pl->rot_y = (int16_t)bearing_to(e, pl);
    pl->hit_react = 0;
}

/* EIN Pistolenschuss (Waffe 3) auf `e`, aus der Richtung `want`. Rueckgabe 1 = getroffen. */
static int shoot(re15_actor_t *e, unsigned want)
{
    place(e, 4000, want);                       /* 4000 = ausserhalb des Schrot-Crit-Radius */
    e->hit_react = 0;
    re15_player_set_equipped_weapon(3);
    return re15_player_weapon_fire(3) != 0;
}

/* Ist irgendein re2z-Zerleger-Feld angefasst worden? */
static int re2z_untouched(const re15_actor_t *e)
{
    if (e->re2z_pool151 || e->re2z_pool152 || e->re2z_pool153) return 0;
    if (e->re2z_hitdir1d0 || e->re2z_flags21a || e->re2z_hits1d2) return 0;
    for (int i = 0; i < 16; i++)
        if (e->re2z_part_flags[i] || e->re2z_part_tint[i] || e->re2z_part_mesh[i]) return 0;
    return 1;
}

/* Die echte RE1.5-Bank (headless gibt es keinen pc_enemy_load).
 * ⚠ Der Blob muss PRO TYP am Leben bleiben — re15_emd_parse_container legt Zeiger IN den Puffer,
 * ein gemeinsamer Puffer wuerde die zuvor geparsten Banken entwerten. */
static uint8_t *s_blob[0x40];
static re15_enemy_bank_t *load_re15_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    long n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return NULL;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, (size_t)n, idx, &off, &len) != 0) { free(ems); return NULL; }
    uint8_t *blob = (uint8_t *)malloc(len);
    if (!blob) { free(ems); return NULL; }
    memcpy(blob, ems + off, len);
    free(ems);
    if (type < 0x40) { free(s_blob[type]); s_blob[type] = blob; }
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) { printf("  (kein freier Bank-Slot fuer EM%02X)\n", type); return NULL; }
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return NULL;
    eb->ok = 1; eb->buf = NULL;
    return eb;
}

int main(void)
{
    s_buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &s_sz);
    if (!s_buf) { printf("SKIP: ROOM1140.RDT fehlt\n"); return 0; }

    /* ================= 1) DER SCHALTER ==================================================== */
    printf("=== 1) Der Schalter re15_re15_re2z_import ===\n");
    {
        re15_re15_re2z_import_set(1);
        CHECK(re15_re15_re2z_import() == 1, "Setter(1) wirkt nicht");
        re15_re15_re2z_import_set(0);
        CHECK(re15_re15_re2z_import() == 0, "Setter(0) wirkt nicht");
        re15_re15_re2z_import_set(1);

        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        static const uint8_t zomb[] = { 0x10, 0x11, 0x12, 0x13, 0x16, 0x18 };
        for (unsigned k = 0; k < sizeof zomb / sizeof zomb[0]; k++)
            CHECK(re15_re15_re2z_import_owns(zomb[k]),
                  "Typ 0x%02X muss zur Import-Menge gehoeren", zomb[k]);
        static const uint8_t other[] = { 0x20, 0x21, 0x25, 0x26, 0x27, 0x2b, 0x30, 0x40 };
        for (unsigned k = 0; k < sizeof other / sizeof other[0]; k++)
            CHECK(!re15_re15_re2z_import_owns(other[k]),
                  "Typ 0x%02X darf NICHT zur Import-Menge gehoeren", other[k]);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        CHECK(!re15_re15_re2z_import_owns(0x10),
              "im RE2-Modus darf die Import-Menge LEER sein (dort macht das re2z_hurt selbst)");
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        re15_re15_re2z_import_set(0);
        CHECK(!re15_re15_re2z_import_owns(0x10), "mit Option AUS muss die Menge leer sein");
        re15_re15_re2z_import_set(1);
        printf("  Menge = genau die Zombie-Familie, nur im RE1.5-Modus, nur mit Option\n");
    }

    /* ================= 2) DIE ZONEN-RESERVE ============================================== */
    printf("\n=== 2) Zonen-Reserve +0x152: 13 -> 9 -> 5 -> 1 -> -1 (Kosten 4, Zeile 3) ===\n");
    {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
        CHECK(slot > 0, "kein Gegner in ROOM1140");
        if (slot > 0) {
            re15_actor_t *e = arm(slot, 0x10, 5000);
            CHECK(re15_re2z_gore_active(e) == 0,
                  "vor dem ersten Treffer darf die Bruecke NICHT aktiv sein (kein INIT-Seed)");
            static const int want[5] = { 9, 5, 1, -1, -1 };
            for (int h = 0; h < 5; h++) {
                CHECK(shoot(e, 0x20), "Treffer %d kam nicht an", h + 1);   /* Ruecken: kein Abriss */
                CHECK((int)e->re2z_pool152 == want[h],
                      "nach Treffer %d ist +0x152 = %d, erwartet %d "
                      "(Start 13 @0x8010081C, Kosten 4 aus 0x800A412C w1 @0x80041954-70)",
                      h + 1, (int)e->re2z_pool152, want[h]);
            }
            CHECK(re15_re2z_gore_active(e) == 1,
                  "nach dem ersten Treffer MUSS der Modellblock geseedet und die Bruecke aktiv sein");
            CHECK(e->re2z_pool151 == 13 && e->re2z_pool153 == 13,
                  "nur die Region 1 (+0x152) darf abgebucht werden — +0x1D2 = 1, 1 %% 3 = 1 "
                  "(@0x80041A88-9C); ist 151=%d 153=%d",
                  (int)e->re2z_pool151, (int)e->re2z_pool153);
            CHECK((e->re2z_flags21a & 0x60u) == 0,
                  "Ruecken-Treffer (+0x1D0 & 0x20) duerfen NIE zerlegen (@0x801052A4-B0)");
            printf("  5 Pistolentreffer von hinten: +0x152 = 9/5/1/-1/-1, kein Abriss\n");
        }
    }

    /* ================= 3) DER ABRISS ====================================================== */
    printf("\n=== 3) Der Zerleger @0x80105288: 4. Pistolentreffer von der SEITE ===\n");
    int torn_shin = -1;
    {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
        if (slot > 0) {
            re15_actor_t *e = arm(slot, 0x10, 5000);
            int fired_at = -1;
            uint32_t draws_of_tear = 0;
            for (int h = 0; h < 6 && fired_at < 0; h++) {
                uint32_t d0 = re15_re2_rand_draws();
                CHECK(shoot(e, 0x40), "Seiten-Treffer %d kam nicht an", h + 1);
                CHECK((e->re2z_hitdir1d0 & 0xc0u) != 0,
                      "Treffer %d: +0x1D0 = 0x%04X traegt kein Seiten-Bit — der Messrahmen "
                      "steht falsch", h + 1, (unsigned)e->re2z_hitdir1d0);
                if (e->re2z_flags21a & 0x60u) {
                    fired_at = h + 1;
                    draws_of_tear = re15_re2_rand_draws() - d0;
                }
            }
            CHECK(fired_at == 4, "der Abriss muss beim VIERTEN Treffer kommen (13/4 = 3 Treffer "
                                 "bis zur Leere, der vierte zuendet), kam bei %d", fired_at);
            CHECK(draws_of_tear == 3,
                  "der Zerleger macht GENAU 3 RNG-Wuerfe (Stumpf @0x801052F8 + 2 Emitter-Winkel "
                  "@0x80105354/@0x801053BC), gemessen %u", (unsigned)draws_of_tear);
            int thigh = (e->re2z_flags21a & 0x40u) ? 12 : 9;
            torn_shin = thigh + 1;                                   /* addiu s0,s0,172 @0x80105370 */
            CHECK((e->re2z_part_flags[torn_shin] & 0x1062u) == 0x1062u,
                  "das Schienbein (Part %d) muss `flags |= 0x1062` tragen (@0x8010537C), ist 0x%04X",
                  torn_shin, (unsigned)e->re2z_part_flags[torn_shin]);
            CHECK(e->re2z_part_v[torn_shin][2] == 10 && e->re2z_part_v[torn_shin][1] == -200 &&
                  e->re2z_part_grav[torn_shin] == 50 && e->re2z_part_st86[torn_shin] == 0,
                  "die Wurf-Felder @0x80105384-C0 fehlen (vz=%d vy=%d g=%d st=%d)",
                  (int)e->re2z_part_v[torn_shin][2], (int)e->re2z_part_v[torn_shin][1],
                  (int)e->re2z_part_grav[torn_shin], (int)e->re2z_part_st86[torn_shin]);
            /* EINWEG: ein zweites Bein kommt nie (@0x80105288-8C). */
            uint16_t f21a = e->re2z_flags21a;
            for (int h = 0; h < 4; h++) shoot(e, 0x40);
            CHECK(e->re2z_flags21a == f21a || (e->re2z_flags21a & 0x60u) == (f21a & 0x60u),
                  "das +0x21A-Einweg-Gate haelt nicht (0x%04X -> 0x%04X)",
                  (unsigned)f21a, (unsigned)e->re2z_flags21a);
            printf("  Abriss beim 4. Treffer, Seite 0x%02X, Part %d = 0x%04X, 3 RNG-Wuerfe\n",
                   (unsigned)(e->re2z_flags21a & 0x60u), torn_shin,
                   (unsigned)e->re2z_part_flags[torn_shin]);
            /* --- Abschnitt 4 haengt an genau diesem Aktor --- */
            printf("\n=== 4) Die Anzeige-Bruecke: Permutation + Kaskade ===\n");
            re15_enemy_bank_t *b = load_re15_bank(0x10);
            if (!b || !b->ok) {
                printf("  SKIP: RE1.5-Bank EM010 nicht ladbar (EMD/CDEMD0.EMS)\n");
            } else {
                const int8_t *perm = NULL;
                int np = re2_hybrid_perm(0x10, &perm);
                CHECK(np == 15 && perm, "re2_hybrid_perm(0x10) muss 15 Slots liefern, liefert %d", np);
                int bone = (np > torn_shin && perm) ? (int)perm[torn_shin] : torn_shin;
                CHECK(bone != torn_shin,
                      "die Permutation muss im RE1.5-Rig eine ANDERE Nummer liefern "
                      "(Part %d -> Bone %d)", torn_shin, bone);
                printf("  RE1.5-Bank: %d Knochen, %d Meshes; abgerissener Part %d -> Bone %d\n",
                       b->skel.bone_count, b->md1.mesh_count, torn_shin, bone);
                /* Die Flugphysik so lange laufen lassen, bis der zweite Bodenkontakt Bit 0
                 * loescht (`and v0,v0,-2` @0x80028CA0) — DAS ist der Kaskaden-Ausloeser. */
                for (uint32_t f = 1; f <= 400u; f++) {
                    int32_t rot[9], tr[3];
                    for (int k = 0; k < 9; k++) rot[k] = (k % 4 == 0) ? 4096 : 0;
                    tr[0] = e->x; tr[1] = e->y; tr[2] = e->z;
                    (void)re15_re2z_gore_part_matrix(e, bone, f, rot, tr);
                    if (!(e->re2z_part_flags[torn_shin] & 1u)) break;
                }
                CHECK((e->re2z_part_flags[torn_shin] & 1u) == 0,
                      "nach dem Aufschlag muss Bit 0 des Teils geloescht sein (@0x80028CA0), "
                      "flags=0x%04X", (unsigned)e->re2z_part_flags[torn_shin]);
                uint8_t  draw[RE15_EMD_MAX_BONES];
                uint32_t tint[RE15_EMD_MAX_BONES];
                uint8_t  mesh[RE15_EMD_MAX_BONES];
                int nb = b->skel.bone_count < b->md1.mesh_count
                       ? b->skel.bone_count : b->md1.mesh_count;
                int on = re15_re2z_gore_resolve(e, b->skel.bone_parent, nb, draw, tint, mesh);
                CHECK(on == 1, "die Bruecke muss im RE1.5-Modus mit Option aktiv sein");
                CHECK(bone < nb && draw[bone] == 0,
                      "der abgerissene Bone %d darf NICHT mehr gezeichnet werden", bone);
                int child = -1, drawn = 0;
                for (int i = 0; i < nb; i++) {
                    if ((int)b->skel.bone_parent[i] == bone) child = i;
                    if (draw[i]) drawn++;
                }
                CHECK(child >= 0 && draw[child] == 0,
                      "das KIND %d des abgerissenen Teils muss ueber die Kaskade "
                      "(@0x80027480-94) mitverschwinden", child);
                CHECK(drawn == nb - 2,
                      "genau ZWEI Slots duerfen verschwinden (Teil + Kind), es sind %d von %d",
                      nb - drawn, nb);
                for (int i = 0; i < nb; i++)
                    if (draw[i]) CHECK(tint[i] == 0x00808080u,
                        "unversehrte Parts muessen die NEUTRALE Tinte behalten (Bone %d = 0x%06X)",
                        i, tint[i] & 0xffffffu);
                printf("  Bone %d + Kind %d verschwinden, %d/%d bleiben mit neutraler Tinte\n",
                       bone, child, drawn, nb);
            }
        }
    }

    /* ================= 5) NEGATIV: OPTION AUS ============================================= */
    printf("\n=== 5) NEGATIV: Option AUS -> kein einziges re2z-Feld wird angefasst ===\n");
    {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 0);
        if (slot > 0) {
            re15_actor_t *e = arm(slot, 0x10, 5000);
            for (int h = 0; h < 8; h++) CHECK(shoot(e, 0x40), "Treffer %d kam nicht an", h + 1);
            CHECK(re2z_untouched(e),
                  "mit Option AUS muss der RE1.5-Pfad die re2z-Felder komplett in Ruhe lassen "
                  "(+0x152=%d +0x1D0=0x%04X +0x21A=0x%04X flags[0]=0x%04X)",
                  (int)e->re2z_pool152, (unsigned)e->re2z_hitdir1d0,
                  (unsigned)e->re2z_flags21a, (unsigned)e->re2z_part_flags[0]);
            CHECK(re15_re2z_gore_active(e) == 0, "mit Option AUS darf die Bruecke nie aktiv sein");
            printf("  8 Treffer, alle re2z-Felder unberuehrt, Bruecke inaktiv\n");
        }
    }

    /* ================= 6) NEGATIV: FRONTAL ================================================ */
    printf("\n=== 6) NEGATIV: frontal (+0x1D0 & 0xC0 == 0) -> kein Abriss @0x801052A4-B0 ===\n");
    {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
        if (slot > 0) {
            re15_actor_t *e = arm(slot, 0x10, 5000);
            for (int h = 0; h < 8; h++) shoot(e, 0x00);
            CHECK((e->re2z_hitdir1d0 & 0xc0u) == 0,
                  "der Messrahmen steht falsch: +0x1D0 = 0x%04X traegt doch ein Seiten-Bit",
                  (unsigned)e->re2z_hitdir1d0);
            CHECK((int)e->re2z_pool152 == -1, "die Reserve muss trotzdem leer sein (%d)",
                  (int)e->re2z_pool152);
            CHECK((e->re2z_flags21a & 0x60u) == 0,
                  "frontal darf NIE zerlegt werden, +0x21A = 0x%04X", (unsigned)e->re2z_flags21a);
            printf("  8 frontale Treffer: Reserve leer (-1), aber kein Abriss\n");
        }
    }

    /* ================= 7) NEGATIV: TOEDLICHER TREFFER ===================================== */
    printf("\n=== 7) NEGATIV: toedlicher Treffer -> kein Abriss (Zerleger sitzt im HURT) ===\n");
    {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
        if (slot > 0) {
            re15_actor_t *e = arm(slot, 0x10, 5000);
            for (int h = 0; h < 3; h++) shoot(e, 0x40);        /* Reserve auf 1 herunter */
            CHECK((int)e->re2z_pool152 == 1, "Vorbereitung: +0x152 = %d statt 1",
                  (int)e->re2z_pool152);
            e->hp = 1;                                          /* der naechste Treffer toetet */
            shoot(e, 0x40);
            CHECK(e->state == 3, "der Treffer haette toedlich sein muessen (state=%d)", e->state);
            CHECK((e->re2z_flags21a & 0x60u) == 0,
                  "ein toedlicher Treffer darf NICHT zerlegen — die HURT-Wurzel laeuft dann gar "
                  "nicht mehr (`sw v0,4(s1)` @0x8004728C-90); +0x21A = 0x%04X",
                  (unsigned)e->re2z_flags21a);
            printf("  toedlicher Treffer: state=3, kein Abriss\n");
        }
    }

    /* ================= 8) ABGRENZUNG: HUND / KRAEHE / SPINNE ============================== */
    printf("\n=== 8) ABGRENZUNG: Hund 0x20 / Kraehe 0x21 / Spinne 0x25 unberuehrt ===\n");
    {
        static const uint8_t t[] = { 0x20, 0x21, 0x25, 0x26 };
        for (unsigned k = 0; k < sizeof t / sizeof t[0]; k++) {
            int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
            if (slot < 0) break;
            re15_actor_t *e = arm(slot, t[k], 5000);
            for (int h = 0; h < 8; h++) shoot(e, 0x40);
            CHECK(re2z_untouched(e),
                  "Typ 0x%02X: die Option hat re2z-Felder angefasst (+0x152=%d +0x21A=0x%04X)",
                  t[k], (int)e->re2z_pool152, (unsigned)e->re2z_flags21a);
            CHECK(re15_re2z_gore_active(e) == 0,
                  "Typ 0x%02X darf die Gore-Bruecke NIE aktivieren", t[k]);
        }
        printf("  0x20/0x21/0x25/0x26: je 8 Treffer, keine re2z-Felder, Bruecke inaktiv\n");
    }

    /* ================= 9) ABGRENZUNG: DER RE2-MODUS ======================================= */
    printf("\n=== 9) ABGRENZUNG: im RE2-Modus ist die Bruecke ein No-op ===\n");
    {
        /* Im RE2-Modus macht re2z_hurt (@0x80104F40) den Stempel + den Zerleger SELBST, im
         * naechsten KI-Tick. re15_re15_re2z_gore_hit darf dort NICHTS tun — sonst wuerde der
         * Stempel doppelt laufen und die Reserve doppelt so schnell fallen. Gemessen: nach EINEM
         * Schuss OHNE KI-Tick duerfen die Zerleger-Felder unveraendert sein. */
        int slot = bringup(RE15_AI_FLAVOR_RE2, 1);
        if (slot > 0) {
            re15_actor_t *e = arm(slot, 0x10, 5000);
            e->re2z_pool152 = 13; e->re2z_flags21a = 0; e->re2z_hitdir1d0 = 0;
            CHECK(shoot(e, 0x40), "RE2-Modus: Treffer kam nicht an");
            CHECK(e->re2z_pool152 == 13 && e->re2z_hitdir1d0 == 0,
                  "im RE2-Modus darf die RE1.5-Bruecke den Applier-Stempel NICHT ausfuehren "
                  "(+0x152=%d +0x1D0=0x%04X) — das macht dort re2z_hurt",
                  (int)e->re2z_pool152, (unsigned)e->re2z_hitdir1d0);
            printf("  RE2-Modus: Schuss ohne KI-Tick laesst +0x152/+0x1D0 unveraendert\n");
        }
    }

    /* ================= 10) DIE GRENZE: KEIN STUMPF-MESH =================================== */
    printf("\n=== 10) GRENZE (gemessen, nicht behauptet): RE1.5-MD1 hat kein Stumpf-Mesh 15 ===\n");
    {
        /* Direkt aus CDEMD0.EMS parsen — NICHT ueber den Bank-Allokator (dessen Slots sind in
         * diesem Test laengst vergeben, und fuer eine reine Mesh-Zahl braucht es keine Bank). */
        static const uint8_t zt[] = { 0x10, 0x11, 0x12, 0x13, 0x16, 0x18 };
        long n = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
        int any = 0;
        for (unsigned k = 0; ems && k < sizeof zt / sizeof zt[0]; k++) {
            int idx = re15_ems_index_for_type(zt[k]);
            size_t off = 0, len = 0;
            if (idx < 0 || re15_ems_get_entry(ems, (size_t)n, idx, &off, &len) != 0) continue;
            static re15_md1_t md; static re15_emd_skeleton_t sk; static re15_emd_animation_t an;
            memset(&md, 0, sizeof md); memset(&sk, 0, sizeof sk); memset(&an, 0, sizeof an);
            if (re15_emd_parse_container(ems + off, len, &md, &sk, &an, NULL) != 0) continue;
            any = 1;
            printf("  EM%02X: %d Knochen, %d Meshes\n", zt[k], sk.bone_count, md.mesh_count);
            CHECK(md.mesh_count <= 15,
                  "EM%02X haette ein Mesh 15 (%d Meshes) — dann waere die Stumpf-Grenze hinfaellig "
                  "und der Zerleger koennte den Stumpf zeigen", zt[k], md.mesh_count);
            CHECK(sk.bone_count == md.mesh_count,
                  "EM%02X: bone_count %d != mesh_count %d — die Part->Bone-Zuordnung des Ports "
                  "setzt Gleichstand voraus", zt[k], sk.bone_count, md.mesh_count);
        }
        free(ems);
        CHECK(any, "keine einzige RE1.5-Zombie-Bank ladbar — die Grenze ist ungemessen");
        printf("  => der Stumpf-Tausch part_mesh = 15 (@0x8010531C-50) findet im RE1.5-MD1 KEINE\n");
        printf("     Geometrie; der Oberschenkel bleibt intakt, Schienbein+Fuss fliegen weg.\n");
        printf("     Genau dieses Ergebnis hat der Nutzer im Modus \"RE2 AI\" abgenommen.\n");
    }

    re15_re15_re2z_import_set(1);
    printf("\n%s\n", fails ? "FAILED" : "OK");
    return fails ? 1 : 0;
}
