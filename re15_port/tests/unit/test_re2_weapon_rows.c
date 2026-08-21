/**
 * @file test_re2_weapon_rows.c
 * @brief ZENSUS-PIN — JEDE Waffe muss den Zombie zum Reagieren bringen (beide KI-Modi).
 *
 * Nutzer-Vorgabe 2026-08-18: "Einzelne staerkere Waffen muessen auch in Resident Evil 1.5
 * reagieren, unabhaengig von der RE2-KI — passiert im Original ja auch."
 *
 * VORHER (gemessen mit test_re2_hit_reaction, Abschnitt 5): der Port reichte die RE1.5-Waffen-Id
 * roh als Tabellenzeile (+0x5) an die RE2-Trefferreaktion @0x8010C940 durch. 7 von 22 Ids
 * (0, 5, 6, 17, 19, 20, 21) landeten auf einer NULL-Zelle oder ausserhalb der Tabelle und loesten
 * GAR KEINE Reaktion aus.
 *
 * URSACHE (byte-belegt, s. enemy_ai_re2_zombie.c): die Zeile ist im Original die ITEM-ID der
 * gefuehrten Waffe — EQUIP FUN_8006B000 (`sb v0,23546(at)` = 0x800D5BFA @0x8006B09C, Gate
 * `sltiu v0,v0,0x14` @0x8006B040) -> FUN_8003BAF0 (`sh v0,270(s2)` @0x8003BD4C = +0x10E) ->
 * FUN_80047C6C (`andi 0xfff` / `addiu a1,v0,-1` / `sll 16` @0x80047EB4-C8) -> FUN_800410CC
 * (`+0x5 = (a1>>16)+1` @0x80041AA0-B4). RE2 kennt 19 Waffen (1..19), RE1.5 22 Zeilen (0..21) —
 * zwei verschiedene Id-Raeume. Die Uebersetzung ist re15_re2z_row_for_weapon().
 *
 * GEMESSEN wird hier NICHT "die Zelle ist != NULL", sondern die tatsaechlich GEFAHRENE Reaktion:
 *   - der Schuss laeuft ueber den echten Pfad re15_player_weapon_fire()
 *   - der Zombie muss in HURT (2) oder DEATH (3) wechseln
 *   - im HURT-Fall: dispatchte Zelle != NULL, HOERBAR (SE) und SICHTBAR (Clip-/Pose-Wechsel)
 *   - RE1.5-Modus: derselbe Zensus gegen die RE1.5-Reaktion (Blut/Bend/SE 6)
 *   - NEGATIV-KONTROLLE: dieselbe Messung mit der ALTEN Zeilenwahl (rohe Waffen-Id) muss fuer
 *     genau die 7 gemeldeten Ids stumm bleiben — sonst misst der Test nichts.
 *
 * ⚠ WARUM DIE GEDRUCKTEN CLIP-INDIZES VON w=14/20/21 SICH 2026-08-21 GEAENDERT HABEN
 * (Herleitung, nicht Vermutung — bewiesen mit einem EINVARIABLEN A/B-Lauf):
 *   Der Zensus feuert ALLE 22 Waffen nacheinander auf DENSELBEN Zombie und zieht dabei aus
 *   EINEM gemeinsamen, deterministischen Wurf-Strom (re2z_rand, Zwilling von FUN_80015FE8 mit
 *   genau einem Zustandswort). GENAU EINE Waffe toetet in diesem Zensus: w=7 Super Redhawk
 *   (Zeile 5, HP 5000 -> -1). Bis zur Nachreichung der Zerreiss-Tode landete ihr Todes-Tick im
 *   Port-Stand-in re2z_death_crawler (Clip 7) mit EINEM Wurf (`re2z_se((rand&1)?13:11)`
 *   @0x80108AD8-F4). Seit der Nachreichung laeuft die Tabellenzelle @0x8010CCDC = FUN_801092C4
 *   mit ZWEI Wuerfen in P0 (Richtung `rand&1` @0x80109330, 50%-Weiche @0x801094D8) und einem
 *   dritten in P3 (`(rand&0x3F)+30` @0x80109554). Der Strom ist danach verschoben — und jede
 *   SPAETERE Waffe, deren Trefferhandler etwas per Wurf waehlt (die MAIN-Zeilen), landet auf
 *   einem anderen Wurf.
 *   BELEG (A/B, einvariabel): wird AUSSCHLIESSLICH die Zelle 0x801092C4 auf den alten Stand-in
 *   zurueckgedreht und bleiben 0x80107438/0x801066FC/0x80108BEC/0x80109610 verdrahtet, ist die
 *   Zensus-Ausgabe wieder ZEICHENGLEICH die alte. Zusaetzlich:
 *     * jede MAIN-Waffe VOR w=7 (w=0,1,2,3,4,5,6) ist unveraendert,
 *     * die Handler mit festem Clip (RAGDOLL/STAGGER/BURN, w=8..13/15..19) sind unveraendert,
 *     * der komplette RE1.5-Zensus-Block ist unveraendert.
 *   Auf der Quellseite dazu passend: die zwei Eingriffe in die HURT-Handler (re2z_hit_ragdoll /
 *   re2z_hit_knockdown) stehen vollstaendig hinter `if (death)`, und der neu hinzugekommene
 *   `re2z_rand()` steht als LINKER Operand von `&&`, wird also unbedingt gezogen — die
 *   Wurfzahl des HURT-Pfades ist unveraendert.
 *   => Es ist eine BELEGTE Folge der Todes-Portierung, kein Nebeneffekt im Trefferpfad. Dieser
 *      Test pinnt Zeilen, Handler-Zellen und "reagiert ueberhaupt" — die Clip-Indizes sind
 *      Diagnose-Ausgabe und werden bewusst NICHT gepinnt; er bleibt unveraendert gruen.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_anim_select.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int re15_re2z_last_hit_handler(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- RE1.5-Waffennamen (Belege s. Report/enemy_ai_re2_zombie.c) ---- */
static const char *wname(int w)
{
    static const char *n[22] = {
        "unbewaffnet", "Combat Knife", "Pipe", "Browning HP", "SIG P228", "Beretta M93R",
        "Glock 18", "Super Redhawk", "Remington M870", "Hand Grenade", "Acid Grenade",
        "Incend. Grenade", "Ingram M10", "SPAS-12", "Flammenwerfer", "GL Explosiv",
        "GL Saeure", "GL Brand", "Rocket Launcher", "H&K MC51", "Colt Python", "(keine Waffe)"
    };
    return (w >= 0 && w < 22) ? n[w] : "?";
}
static const char *hname(int h)
{
    static const char *n[7] = { "NULL", "0x80105438 MAIN", "0x80105BC0 STAGGER", "0x801066FC RAGDOLL",
                               "0x8010703C SLIDE", "0x80107438 KNOCK", "0x80107EF0 BURN" };
    return (h >= 0 && h < 7) ? n[h] : "?";
}

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
static const uint8_t *re2_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static long s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = (size_t)s; return b;
}
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb) return eb;
    eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) { eb->buf = NULL; eb->ok = 1; return eb; }
    eb->type = 0; return NULL;
}

/* ---- SE-Mitschnitt ----
 * RE2-Modus: der ENEMSE-Hook (re15_re2z_audio_hook).
 * RE1.5-Modus: der SNDL1-Spion aus tests/test_support.c (`re15_audio_room_se` -> g_test_room_se_*),
 * ueber den der RE1.5-HURT seinen Treffer-SE 6 (`FUN_800453d0(6)` @0x80105C0C) meldet. */
static int s_se[512], s_se_n;
static void probe_se(int id, int f2) { (void)f2; if (s_se_n < 512) s_se[s_se_n++] = id; }
extern int g_test_room_se_n;

/* ---- Raum-Bringup ---- */
static uint8_t *s_buf = NULL;
static re15_enemy_bank_t *s_bank = NULL;

static re15_actor_t *bringup(re15_ai_flavor_t flavor)
{
    long sz = 0;
    if (!s_buf) {
        s_buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
        if (!s_buf) { CHECK(0, "ROOM1140.RDT fehlt"); return NULL; }
    } else {
        /* der Puffer bleibt gueltig; Groesse fuer den Re-Parse neu ermitteln */
        FILE *f = fopen(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", "rb");
        if (f) { fseek(f, 0, SEEK_END); sz = ftell(f); fclose(f); }
    }
    re15_rdt_t rdt;
    if (re15_rdt_parse(s_buf, (size_t)sz, &rdt) != 0) { CHECK(0, "RDT-Parse"); return NULL; }

    re15_ai_flavor_set(flavor);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
    if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;

    int first = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10) { first = s; break; }
    if (first < 0) { CHECK(0, "kein Zombie 0x10 in ROOM1140"); return NULL; }
    /* NUR EIN Zombie darf ticken — die Handler-Diagnose ist global. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && s != first) g_actors[s].active = 0;
    if (flavor == RE15_AI_FLAVOR_RE2) {
        s_bank = load_re2_bank(0x10);
        CHECK(s_bank != NULL && s_bank->skel.bone_count > 1,
              "RE2-Zombie-Bank fehlt (shared_assets/RE2/CDEMD0.EMS)");
    }
    /* AUFWAERMEN: erst der AI-INIT installiert die Schaden-Hitbox (+0x78 / hit_radius_min), und
     * ohne sie ueberspringt der Auto-Aim den Gegner komplett (`hit_radius_min <= 0 -> continue`
     * in re15_player_weapon_fire, byte-true FUN_80011F50 iteriert ueber die Hitboxen). */
    for (int i = 0; i < 60; i++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
    return &g_actors[first];
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* Frischer, lebendiger Zombie in Schussweite. `hp` steuert, ob der Treffer ueberlebt wird. */
static void arm_zombie(re15_actor_t *e, int hp, int dist)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = (int16_t)hp; e->hit_react = 0; e->hit_stun = 0;
    /* Resistenz VOLL (20 > Schwelle 0): damit ueberspringt die HURT-Wurzel das Flinch-Gate
     * (`lb v0,547` / `slt v0,v1,v0` / `bne` @0x8010506C-78) und laeuft in den TABELLEN-DISPATCH
     * @0x801053E0 — genau den Pfad, den dieser Zensus misst. Das ist auch der normale
     * Spielverlauf: +0x223 wird erst durch mehrere Treffer aufgebraucht. */
    e->re2z_res223 = 20; e->re2z_flag222 = 0;
    e->re2z_cd239 = 0; e->re2z_flags21a = 0; e->re2z_f10e = 0;
    e->re2z_t158 = 0; e->re2z_t15a = 0; e->re2z_dir16a = 0; e->re2z_gaitrow = 0;
    e->re2z_rag231 = 0; e->re2z_gy232 = 0;
    e->anim_frame = 0; e->anim_frac = 0; e->anim_flags = 0;
    e->re2_lean[0] = e->re2_lean[1] = e->re2_lean[2] = 0;
    e->re2_lean_on = 0; e->re2_bone0_wgt = 0;
    e->motion = 0;
    /* aufrecht: das DOWNED-Bit (+0x9 & 0x80) nimmt dem Gegner das LEVEL-Band und damit die
     * Ziel-Eignung fuer den waagerechten Schuss (@0x80101614-3C). */
    e->grid_id = (uint8_t)(e->grid_id & 0x7f);
    pl->x = e->x - dist; pl->z = e->z; pl->rot_y = 0; pl->hit_react = 0;
}

/* Die Reaktion EINER Waffe messen. Rueckgabe: 1 = reagiert, 0 = stumm. */
typedef struct {
    int fired, state, row, handler, se_n, pose_changes, hp_before, hp_after;
    int clip_before, clip_after, clip_changed, bend_seen, hurt_frames, lean_seen, res_delta;
} react_t;

static int measure(re15_actor_t *e, int w, int hp, int raw_row_override, react_t *out)
{
    /* Schusswaffen (Tester 0x80012574 @0x8006E548) treffen unbegrenzt weit; auf 3500 bleibt der
     * Schrotflinten-Crit (dist < 3000 @0x800124FC) aus, sodass die Ragdoll-Zeile wirklich laeuft.
     * Nahkampf/Sprengstoff (0x800127FC/0x800128A0) sind radial begrenzt -> 800. */
    static const uint8_t gun_strip[22] = { 1,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,1,0,1 };
    int dist = gun_strip[w] ? 3500 : 800;
    arm_zombie(e, hp, dist);
    memset(out, 0, sizeof *out);
    out->hp_before  = e->hp;
    out->clip_before = (int)e->motion;
    s_se_n = 0; g_test_room_se_n = 0;
    re15_player_set_equipped_weapon(w);
    out->fired = re15_player_weapon_fire(w);
    if (!out->fired) return -1;                      /* Treffer kam nicht an (Reichweite/Kegel) */
    if (raw_row_override >= 0) e->sub_state_1 = (uint8_t)raw_row_override;  /* NEGATIV-KONTROLLE */
    out->row      = e->sub_state_1;
    out->state    = e->state;
    out->hp_after = e->hp;

    uint32_t lastp = ((uint32_t)e->motion << 16) | (e->anim_frame & 0xffffu);
    int handler = 0;
    for (int f = 0; f < 24; f++) {
        frame();
        if (!handler) handler = re15_re2z_last_hit_handler();
        if ((int)e->motion != out->clip_before) out->clip_changed = 1;   /* Reaktions-Clip laeuft */
        if (e->hurt_bend_bone >= 0) out->bend_seen = 1;                  /* RE1.5-Oberkoerper-Bend */
        /* RE2: das Oberkoerper-ZUCKEN — die Bone-0/1-Matrix-Injektion des Haupt-Handlers
         * (@0x801057A4-838 P1 / @0x801058D4-960 P2). Der Haupt-Handler wechselt KEINEN Clip
         * (P0 setzt nur den Gehclip @0x801054BC-D4), die Reaktion IST das Zucken. */
        if (e->re2_lean_on && (e->re2_lean[0] || e->re2_lean[1] || e->re2_lean[2]))
            out->lean_seen = 1;
        if (e->state == 2) out->hurt_frames++;
        uint32_t p = ((uint32_t)e->motion << 16) | (e->anim_frame & 0xffffu);
        if (p != lastp) { out->pose_changes++; lastp = p; }
    }
    out->handler = handler;
    out->clip_after = (int)e->motion;
    out->se_n = s_se_n + g_test_room_se_n;      /* ENEMSE (RE2) + SNDL1 (RE1.5) */
    return 0;
}

/* ======================================================================================= */
int main(void)
{
    re15_re2z_audio_hook(probe_se, NULL);

    int null_rows_re2 = 0, silent_re2 = 0;

    /* =================== (1) RE2-MODUS — Zensus ueber alle 22 Waffen ==================== */
    {
        re15_actor_t *e = bringup(RE15_AI_FLAVOR_RE2);
        if (!e) { printf("test_re2_weapon_rows: %d FAIL\n", fails ? fails : 1); return 1; }
        printf("== RE2-MODUS: Waffe -> RE2-Attacken-Id (Zeile) -> gefahrene Reaktion ==\n");
        for (int w = 0; w < 22; w++) {
            /* (a) die reine Zuordnung: nie 0, nie > 19, nie eine NULL-Zelle in Spalte 1 */
            unsigned row = re15_re2z_row_for_weapon((unsigned)w, 1u, 1);
            CHECK(row >= 1u && row <= 19u,
                  "w%d (%s): Zeile %u ausserhalb 1..19 (RE2 kennt genau 19 Waffen-Ids, "
                  "Beleg `sltiu v0,v0,0x14` @0x8006B040 + Handler-Tabelle @0x800A6FDC)",
                  w, wname(w), row);

            /* (b) der ECHTE Schuss: PANZER-Zombie (hp 5000), damit die HURT-Wurzel garantiert
             *     laeuft und nicht der Tod die Reaktion "gewinnt". */
            react_t r;
            if (measure(e, w, 5000, -1, &r) != 0) {
                CHECK(0, "w%d (%s): der Schuss trifft gar nicht (weapon_fire == 0)", w, wname(w));
                continue;
            }
            /* SICHTBAR = neuer Reaktions-Clip ODER das Oberkoerper-Zucken; HOERBAR = SE;
             * MECHANISCH = eine nicht-NULL-Zelle wurde wirklich dispatcht. Tod zaehlt auch. */
            int reacted = (r.state == 2 && r.handler != 0 && (r.clip_changed || r.lean_seen)
                           && r.se_n > 0) || r.state == 3;
            if (r.handler == 0 && r.state == 2) null_rows_re2++;
            if (!reacted) silent_re2++;
            printf("   w=%-2d %-16s -> Zeile %-2d -> %-19s state=%d Clip %d->%d Zucken=%d SE=%-2d HP %d->%d\n",
                   w, wname(w), r.row, hname(r.handler), r.state, r.clip_before, r.clip_after,
                   r.lean_seen, r.se_n, r.hp_before, r.hp_after);
            CHECK(reacted,
                  "w%d (%s) loest KEINE Reaktion aus (Zeile %d, Zelle %s, state %d, "
                  "Clip-Wechsel %d, Zucken %d, SE %d) — Nutzer-Vorgabe: jede Waffe muss reagieren",
                  w, wname(w), r.row, hname(r.handler), r.state, r.clip_changed, r.lean_seen,
                  r.se_n);
        }
        CHECK(null_rows_re2 == 0,
              "%d Waffen fallen im RE2-Modus immer noch auf eine NULL-Zelle", null_rows_re2);
        CHECK(silent_re2 == 0, "%d von 22 Waffen bleiben im RE2-Modus stumm", silent_re2);

        /* (c) die BELEGTEN Klassen-Identitaeten muessen wirklich in ihrer Klasse landen. */
        struct { int w; unsigned row; const char *why; } id[] = {
            {  1,  1, "Messer -> RE2-Messer (einzige Nahkampfzeile, Geometrie @0x800A6900)" },
            {  3,  3, "Browning HP -> RE2-Id 3 IST die Browning HP (PL01W03)" },
            {  7,  5, "Super Redhawk -> Magnum-Klasse (900 Schaden @0x800A412C+4*20)" },
            {  8,  7, "Remington M870 -> Schrotflinte (Ragdoll-Zeile 7)" },
            { 12, 15, "Ingram M10 -> RE2-Id 15 IST die Ingram-MP (Dauerfeuer flags 0x80)" },
            { 13,  8, "SPAS-12 -> Custom-Schrotflinte (300/80/60)" },
            { 14, 16, "Flammenwerfer -> RE2-Id 16 IST der Flammenwerfer (Fuel 0x17)" },
            { 18, 17, "Rocket Launcher -> RE2-Id 17 IST der Raketenwerfer (900 Schaden)" },
            { 20, 13, "Colt Python -> Colt S.A.A. (Single-Action-Revolver)" },
        };
        for (unsigned k = 0; k < sizeof id / sizeof id[0]; k++) {
            unsigned got = re15_re2z_row_for_weapon((unsigned)id[k].w, 1u, 0);  /* ungefiltert */
            CHECK(got == id[k].row, "w%d: Zeile %u statt %u — %s",
                  id[k].w, got, id[k].row, id[k].why);
        }

        /* (d) die drei NULL-Zeilen bleiben im Original unerreichbar, WEIL diese Waffen immer
         *     toeten. Der Port muss das nachfahren: mit REALISTISCHER HP toeten w7 und w18. */
        for (int k = 0; k < 2; k++) {
            int w = k ? 18 : 7;
            react_t r;
            if (measure(e, w, 60, -1, &r) == 0) {
                CHECK(r.state == 3,
                      "w%d (%s) muss den Zombie mit realistischer HP (60) SOFORT toeten "
                      "(RE2-Zeile 900 Schaden = DEATH statt HURT, @0x8004727C-90) — state %d",
                      w, wname(w), r.state);
                printf("   [900er-Klasse] w=%d (%s) HP %d -> %d, state %d (DEATH erwartet)\n",
                       w, wname(w), r.hp_before, r.hp_after, r.state);
            }
        }

        /* (e) PORT-SICHERUNG: ueberlebt der Zombie eine 900er-Klasse-Waffe trotzdem (Port-Schaden
         *     != RE2-Schaden), darf die NULL-Zeile nicht stumm bleiben. */
        {
            unsigned g = re15_re2z_row_for_weapon(18u, 1u, 1);
            CHECK(g == 8u, "PORT-SICHERUNG: w18 muss bei ueberlebtem Treffer auf Zeile 8 "
                           "ausweichen (schwerste nicht-NULL-Zeile), ist %u", g);
        }

        /* ---------------- NEGATIV-KONTROLLE ----------------
         * Dieselbe Messung, aber die Zeile wird nach dem Schuss wieder auf die ROHE Waffen-Id
         * gesetzt = exakt der Stand vor diesem Fix. Genau die 7 gemeldeten Ids muessen dann
         * stumm sein — sonst misst der Zensus oben nichts. */
        {
            static const int reported[7] = { 0, 5, 6, 17, 19, 20, 21 };
            int silent = 0;
            printf("   -- NEGATIV-KONTROLLE (Zeile = rohe Waffen-Id, Stand vor dem Fix) --\n");
            for (int k = 0; k < 7; k++) {
                int w = reported[k]; react_t r;
                if (measure(e, w, 5000, w, &r) != 0) continue;
                int mute = (r.state == 2 && r.handler == 0);
                if (mute) silent++;
                printf("      w=%-2d %-16s -> Zeile %-2d -> %-19s %s\n", w, wname(w), r.row,
                       hname(r.handler), mute ? "<== STUMM (erwartet)" : "reagiert");
            }
            CHECK(silent == 7,
                  "NEGATIV-KONTROLLE: mit der rohen Waffen-Id muessen GENAU die 7 gemeldeten Ids "
                  "stumm bleiben, gemessen %d — der Zensus oben ist sonst ohne Aussage", silent);
        }
    }

    /* =================== (2) RE1.5-MODUS — derselbe Zensus ============================== */
    {
        re15_actor_t *e = bringup(RE15_AI_FLAVOR_RE15);
        if (!e) { printf("test_re2_weapon_rows: %d FAIL\n", fails ? fails : 1); return 1; }
        printf("== RE1.5-MODUS: Waffe -> RE1.5-Reaktion (Zustand/Pose/SE) ==\n");
        int silent = 0;
        for (int w = 0; w < 22; w++) {
            react_t r;
            if (measure(e, w, 5000, -1, &r) != 0) {
                CHECK(0, "RE1.5: w%d (%s) trifft gar nicht (weapon_fire == 0)", w, wname(w));
                continue;
            }
            /* Der RE1.5-HURT-Handler (FUN_80105a8c/0x80105b7c) ist NICHT tabellen-gegated: er
             * spielt den Stagger fuer JEDEN Treffer. Erwartet werden Zustandswechsel, ein
             * Pose-Wechsel und der Treffer-SE 6 (`FUN_800453d0(6)` @0x80105c0c). */
            int reacted = (r.state == 2 && r.se_n > 0 && (r.clip_changed || r.bend_seen))
                          || r.state == 3;
            if (!reacted) silent++;
            printf("   w=%-2d %-16s -> +0x5=%-2d state=%d HURT-Frames=%-2d Bend=%d Clip%s SE=%-2d HP %d->%d\n",
                   w, wname(w), r.row, r.state, r.hurt_frames, r.bend_seen,
                   r.clip_changed ? "-Wechsel" : " gleich", r.se_n, r.hp_before, r.hp_after);
            CHECK(reacted,
                  "RE1.5: w%d (%s) loest KEINE Reaktion aus (state %d, Clip-Wechsel %d, "
                  "Bend %d, SE %d)", w, wname(w), r.state, r.clip_changed, r.bend_seen, r.se_n);
        }
        CHECK(silent == 0, "RE1.5-Modus: %d von 22 Waffen bleiben stumm", silent);

        /* Die RE1.5-Zeile +0x5 MUSS im RE1.5-Modus die rohe Waffen-Id bleiben (@0x800124BC) —
         * die RE2-Uebersetzung darf den byte-true Default nicht anfassen. */
        for (int w = 0; w < 22; w++) {
            react_t r;
            if (measure(e, w, 5000, -1, &r) != 0) continue;
            CHECK(r.row == w,
                  "RE1.5-Modus: +0x5 muss die rohe Waffen-Id %d bleiben (`sb v0,5(v1)` "
                  "@0x800124BC), ist %d", w, r.row);
        }
    }

    free(s_buf);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(fails ? "test_re2_weapon_rows: %d FAIL\n" : "test_re2_weapon_rows: OK\n", fails);
    return fails ? 1 : 0;
}
