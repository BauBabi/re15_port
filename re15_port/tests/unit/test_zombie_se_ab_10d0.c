/* test_zombie_se_ab_10d0.c — REGRESSIONS-PIN fuer das A/B-ORAKEL des Nutzers (Report 2026-08-17 #3)
 *
 *   "Der Zombie im 2F-Korridor (ROOM10D0), der am Boden liegt — nach der Cutscene mit Marvin —
 *    macht ANDERE Geraeusche, wenn er getroffen wird, zu Boden geht und wieder aufsteht, als die
 *    Zombies im Dinner-Raum (ROOM1140)."
 *
 * Das Orakel: GLEICHE Handlung an GLEICHEM Zustand => GLEICHE SE-ID-Kette, egal in welchem Raum.
 * Die SE-Kette der Treffer-/Sturz-/Aufsteh-Kette haengt im Original an NICHTS Raumabhaengigem:
 *
 *   HURT-Router FUN_80105a8c: `lbu +0x9; andi 0x80; beq -> Tabelle @0x8011fb90[+0x5*8 + +0x6]`
 *     (@0x80105a9c-b08); Reaktionsklasse 3 (Handfeuerwaffe, re15_react_table[0]) -> Zeile 3
 *     = 0x80105b7c (Tabellen-Dump: [24]/[25] -> 0x80105b7c).
 *   Steh-Stagger FUN_80105b7c Phase 0: SE 6 UNBEDINGT (`jal 0x800453d0` @0x80105cf4).
 *   Poise-Bruch (+0x1dc < 0 && !(+0x9&0x80)) -> +0x4=1 (@0x80105b48) / +0x5=0x11 (@0x80105b58).
 *   KNOCKDOWN FUN_8010512c, Phasentabelle @0x801000c4 = {0x8010516c, 0x80105234, 0x80105278,
 *     0x801052f8, 0x8010532c, 0x801053c4, 0x80105400}:
 *     [0] Clip 0x0b (`ori v0,0xb; sb v0,148` @0x80105190-94), +0x95=0 (@0x801051a4),
 *         +0x6=1 (@0x801051b4), +0x8f=0xf (@0x801051c8), SE-Wurf 1/4:
 *         `andi v0,v0,3; bne -> skip` (@0x801051cc-d0), dann `rng; andi 1;
 *          beq -> a0=8 (Delay-Slot @0x801051e8) sonst a0=5 (@0x801051ec); jal 0x800453d0`
 *         (@0x801051e0-f0) => (rand&1)==0 -> SE 8, sonst SE 5.
 *         Danach +0x1b8=1 (@0x80105204), +0x1dc=0x80 (@0x80105214), +0x9|=0x80 (@0x8010522c-30).
 *     [1] anim_set(+0x170,+0x174,0,0x100) (@0x80105240-4c) -> BANK 1, +0x6 += ret.
 *     [2] +0x9|=0x80, +0x6=3, +0x9c = tbl@0x8011fb10[rand&0xf]*30 (@0x801052a8-d8) —
 *         KORREKTUR 2026-08-18: `rand` ist hier KEIN Wurf. Der ACTIVE-Root ruft im DOWNED-
 *         Zustand IMMER FUN_80012974 (@0x80101620-38), die a0 IMMER als 0x20000000 verlaesst
 *         (@0x800129d4 `lui a0,0x2000` im Delay-Slot, danach kein a0-Write bis `jr ra`), und
 *         FUN_8001af20 hasht NUR a0 -> Rueckgabe 0. Also IMMER tbl[0]*30 = 360,
 *         +0x93 &= 0xfe (@0x801052f0-f4).
 *     [3] Countdown, KEIN anim_set, KEIN SE (@0x801052f8-328).
 *     [4] Clip 0x12 (@0x80105334-38), +0x95=0, +0x6=5, +0x8f=0xf, +0x93|=1, +0x1b8=0,
 *         SE: `andi v0,v0,7; bne -> skip` (@0x80105398-9c), dann (rand&1)==0 -> 8 / sonst 5
 *         (@0x801053ac-bc). KORREKTUR 2026-08-18: mit rand()==0 (s.o.) ist der Zweig NICHT
 *         genommen und der zweite Wurf ebenfalls 0 -> der Aufsteh-Grunzer ist DETERMINISTISCH
 *         SE 8 (@0x801053b4 `ori a0,zero,8` im Delay-Slot). Pin: test_zombie_getup_se.c.
 *     [5] anim_set(+0x170,+0x174,0,0x100) (@0x801053d0-dc).
 *     [6] +0x9 &= 0x7f (@0x80105414-18), Wort 0x201, Poise neu.
 *   Die FALL-/AUFPRALL-Geraeusche kommen aus den FRAME-WOERTERN des Clips 0x0b, dekodiert von
 *   FUN_8001b38c (`lw v0,360(v0)` @0x8001b3a4 / `lw v0,0(v0)` @0x8001b3ac / `srl s0,v0,22`
 *   @0x8001b3b4), unbedingt aus dem Wurzel-Tail gerufen (`jal 0x8001b38c` @0x80100634):
 *   f14 -> SE 1, f46 -> SE 0. Der Getup-Clip 0x12 traegt KEIN SE-Bit.
 *
 * KEINE dieser Konstanten haengt am Raum. Der EINZIGE raumabhaengige Faktor im ganzen SE-Pfad ist
 * FUN_800453d0 selbst — Bank 3 = `*(DAT_800ac778+0x14)` = die snd1-Scheibe der AKTUELLEN RDT — und
 * der Index-Versatz `if (*DAT_800ac784 & 0x2000) se += 12` (@0x80045404-18), dessen einziger
 * Produzent `Sce_em_set pc[5] != 0 -> entity+0x0 = 0x2001` (@0x8004229c-b0) ist; pc[5] ist in
 * BEIDEN Raeumen 0 (ROOM10D0 Datei 0x126a `44 01 10 0e 00 00 00 cd`, 0x12a4 `44 00 10 02 ...`;
 * ROOM1140 Datei 0x000bbe/bd2/be6/bfa `44 0x 1x 86 00 00 01 dx`) -> Versatz inaktiv.
 *
 * DIESER PIN faehrt beide Raeume mit IDENTISCH erzwungenem Startzustand und IDENTISCHEM RNG-Seed
 * und verlangt: BYTE-GLEICHE SE-ID-Kette. Er wird rot, sobald irgendetwas Raumabhaengiges in die
 * Zombie-SE-Kette zurueckkehrt (Bank-Regel, Clip-Wahl, Grid-Nibble-Dispatch, Handler-SE).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h" /* re15_re15_re2z_import_set — byte-true PIN schaltet die Port-Option ab */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int g_test_room_se_log[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    if (s <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)s; return b;
}

static uint8_t s_b10[0x80000], s_b11[0x80000], s_b16[0x80000];
static void load_bank(const char *base, uint8_t type, uint8_t *scratch, size_t cap)
{
    char emsp[600]; size_t n = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = slurp(emsp, &n);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= cap) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(scratch, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(scratch, len, &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
}

static void tick_once(void)
{
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);
    if (g_aot.fired_event_id_this_frame != 0)
        (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

/* MESS-TICK der Treffer-Phase: NUR Anim-Advance + Gegner-KI. Das Original teilt sich EINEN
 * RNG (FUN_8001af20 @0x8001af20) zwischen KI und Raum-Skript; laesst man die SCD-VM
 * mitlaufen, verschiebt jeder raumeigene RNG-Zug die KI-Wuerfe (Fall-Grunzer 1/4, Getup-
 * Grunzer 1/8, Engage-Re-Roll) — das ist byte-true, aber es ist RAUM-SKRIPT-Phase, nicht
 * Zombie-Logik. Der Pin will die ZOMBIE-Logik isolieren, also faehrt die Treffer-Phase ohne
 * die VM (der Raum ist zu diesem Zeitpunkt fertig aufgebaut und der Zombie steht frei). */
static void tick_ai_only(void)
{
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

/* Faehrt EINEN Raum: Zombie suchen, alle anderen Aktoren stilllegen (der SE-Spion ist global),
 * Startzustand HART gleichsetzen, RNG gleich seeden, dann Treffer -> Sturz -> Liegen -> Aufstehen.
 * Gibt die SE-ID-Kette zurueck. */
static int run_room(const char *base, const char *room, unsigned room_id,
                    int32_t px, int32_t pz, int marvin_seen, int want_type,
                    int *out, int out_cap)
{
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, room);
    uint8_t *d = slurp(path, &sz);
    if (!d) { printf("FAIL: %s nicht ladbar\n", room); fails++; return 0; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(d, sz, &rdt) != 0) { printf("FAIL: %s parse\n", room); fails++; free(d); return 0; }

    re15_actor_init(); scd_vm_init(); re15_enemy_reset();
    if (marvin_seen) re15_game_flag_set(4, 247, 1);      /* sub21 +0x07e0 (Datei 0x19ec) */
    g_current_room_id = room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_bank(base, 0x10, s_b10, sizeof s_b10);
    load_bank(base, 0x11, s_b11, sizeof s_b11);
    load_bank(base, 0x16, s_b16, sizeof s_b16);
    for (int f = 0; f < 8; f++) tick_once();

    re15_actor_t *z = NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == (uint8_t)want_type) { z = &g_actors[s]; break; }
    if (!z) { printf("FAIL: %s kein Zombie 0x%02x\n", room, want_type); fails++; free(d); return 0; }
    int zslot = (int)(z - g_actors);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != zslot && g_actors[s].active) g_actors[s].active = 0;

    /* IDENTISCHER Startzustand: stehender Engage-Zombie, Bank-1-Clip-Uhr frisch.
     * (Der Weg DORTHIN unterscheidet sich raumbedingt — 10D0 Sleeping-Lyer 0x12,
     *  10D0-nach-Cutscene Grid-Nibble-2-Schlaefer, 1140 Feeder 0x0c — und darf die
     *  SE-Kette der TREFFER-Handlung nicht beeinflussen. Genau das pinnt dieser Test.) */
    z->state = 1; z->sub_state_1 = 0x02; z->sub_state_2 = 1; z->sub_state_3 = 0;
    z->grid_id &= (uint8_t)0x0f;                 /* Downed-Bit sicher aus, Nibble bleibt raum-echt */
    z->motion = 3; z->anim_frame = 0; z->anim_frac = 0xf; z->anim_flags = 0x04;
    z->hurt_clip = 3; z->hit_stun = 4; z->hp = 200; z->hit_react = 0;
    z->x = pl->x + 2600; z->z = pl->z + 2600;    /* feste Distanz: kein Grab dazwischen */
    /* Auch die GEOMETRIE gleichsetzen: der Engage-Brain (FUN_80102058) entscheidet ueber
     * arc_test gegen den Spieler (Turn 0x07 / Approach 0x13 / Grab 0x03) und zieht je Zweig
     * unterschiedlich viele RNG-Werte aus dem EINEN geteilten RNG (FUN_8001af20). Die
     * Spawn-Yaws der beiden Raeume sind verschieden (ROOM10D0 1256, ROOM1140 eigen), also
     * muss der Pin sie egalisieren — sonst misst er Raumgeometrie statt Zombie-Logik. */
    z->y = 0; z->floor = 0; z->rot_y = 0; pl->rot_y = 0; pl->y = 0; pl->floor = 0;
    z->ai_timer = 0; z->speed_h = 0; z->grab_kill_ctr = 0;
    z->steer_x = (int16_t)pl->x; z->steer_z = (int16_t)pl->z;

    re15_damage_seed_rng(0x0BADC0DEu);
    g_test_room_se_n = 0;
    int knocked = 0;
    for (int shot = 0; shot < 8 && !knocked; shot++) {
        z->hit_react &= (uint8_t)~1u;
        (void)re15_enemy_take_damage(z, 0);      /* attack_type 0 -> re15_react_table[0] = 3 */
        for (int t = 0; t < 60; t++) {
            tick_ai_only();
            if (z->state == 1 && z->sub_state_1 == 0x11) { knocked = 1; break; }
            if (z->hp < 0) break;
        }
    }
    if (!knocked) { printf("FAIL: %s kein Knockdown erreicht\n", room); fails++; }
    for (int t = 0; t < 2500; t++) {
        tick_ai_only();
        if (z->state == 1 && z->sub_state_1 != 0x11 && t > 60) break;
    }
    int n = g_test_room_se_n < out_cap ? g_test_room_se_n : out_cap;
    for (int i = 0; i < n; i++) out[i] = g_test_room_se_log[i];
    free(d);
    return n;
}

static void print_chain(const char *tag, const int *v, int n)
{
    printf("  SE-Kette %-6s (%d):", tag, n);
    for (int i = 0; i < n; i++) printf(" %d", v[i]);
    printf("\n");
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    /* Dieser Test pinnt die byte-true RE1.5-SE-KETTE des KNOCKDOWNS. Seit 2026-08-22 zieht die
     * Port-Option re15_re15_re2z_import() (Default AN, Nutzer-Entscheidung "der fette Zombie muss
     * nicht jeden 2. Schuss umfallen") die RE2-STURZREGEL in den RE1.5-Modus: die Leiste +0x1DC
     * wird bei HP >= 81 nach jedem Erschoepfen sofort nachgeladen (`slti v0,v0,81` @0x80105604 /
     * `andi 0xf`+`addiu 16` @0x80105618-1C), der Zombie faellt also im gesunden Zustand gar nicht
     * mehr — und dieser Test schiesst genau in diesen Zustand hinein. Er schaltet die Option
     * deshalb ab und pinnt weiter GENAU das, was er immer gepinnt hat (dasselbe Verfahren wie
     * test_room1140_combat.c). Was der SPIELMODUS heute tut, misst probe_re15_poise_re2.c /
     * pinnt test_re15_poise_re2_import.c. */
    re15_re15_re2z_import_set(0);
    int a[256], b[256], c[256];
    /* ROOM10D0 Erstbesuch (Liegender, behavior 0x0e, Datei 0x126a) — Tuer-15-Spawn (7650,0,11400) */
    int na = run_room(base, "ROOM10D0", 0x10D0,  7650,  11400, 0, 0x10, a, 256);
    /* ROOM10D0 nach der Marvin-Cutscene (behavior 0x02, Grid-Nibble 2, Datei 0x12a4) */
    int nc = run_room(base, "ROOM10D0", 0x10D0,  7650,  11400, 1, 0x10, c, 256);
    /* ROOM1140 Dinner-Raum (Feeder behavior 0x86, Datei 0x000bbe) — Tuer-0-Spawn */
    int nb = run_room(base, "ROOM1140", 0x1140, -1300, -13950, 0, 0x10, b, 256);
    print_chain("10D0", a, na);
    print_chain("10D0c", c, nc);
    print_chain("1140", b, nb);

    CHECK(na > 0 && nb > 0 && nc > 0, "alle drei Faelle liefern eine SE-Kette");
    CHECK(na == nb && memcmp(a, b, (size_t)na * sizeof(int)) == 0,
          "ROOM10D0-Liegender == ROOM1140-Stehender (Nutzer-Orakel: gleiche Handlung, gleiche SEs)");
    CHECK(nc == nb && memcmp(c, b, (size_t)nc * sizeof(int)) == 0,
          "ROOM10D0 nach Marvin-Cutscene (Grid-Nibble 2) == ROOM1140");

    /* Byte-true Soll-Kette der Handlung (Adressen im Kopfkommentar):
     *   SE 6  = Treffer, unbedingt  (@0x80105cf4)
     *   SE 8  = Fall-Grunzer 1/4    (@0x801051e8/f0, (rand&1)==0)
     *   SE 1  = Fall-Clip 0x0b f14  (Frame-Wort, FUN_8001b38c @0x8001b3b4)
     *   SE 0  = Fall-Clip 0x0b f46  (Bodenaufprall, Frame-Wort)
     * Der Getup-Clip 0x12 traegt KEIN Frame-Wort-SE; sein 1/8-Grunzer (@0x801053bc) haengt am
     * RNG-Strom und wird deshalb NICHT als feste Position gepinnt — wohl aber, dass die vier
     * obigen in DIESER Reihenfolge vorkommen. */
    {
        static const int want[] = { 6, 1, 0 };   /* die RNG-UNABHAENGIGEN drei */
        int k = 0;
        for (int i = 0; i < na && k < 3; i++) if (a[i] == want[k]) k++;
        CHECK(k == 3, "Soll-Teilkette 6 (Treffer @0x80105cf4) -> 1 (Fall-Clip f14) -> 0 (f46) in Reihenfolge");
        /* Nur 0..9 zulaessig: FUN_800453d0 verwirft >=0x19 (@0x80045420), und der ganze
         * Zombie-Pfad benutzt ausschliesslich die Basis-IDs 0..9 — der +12-Versatz
         * (@0x80045404-18, `*DAT_800ac784 & 0x2000`) ist fuer diese Spawns inaktiv,
         * weil Sce_em_set pc[5] (@0x8004229c-b0) in beiden Raeumen 0 ist. */
        for (int i = 0; i < na; i++)
            if (a[i] < 0 || a[i] > 9) { CHECK(0, "SE-ID %d ausserhalb 0..9", a[i]); break; }
    }
    /* Die drei Frame-Wort-Konstanten direkt aus der Bank (unabhaengig vom Lauf). */
    {
        re15_enemy_bank_t *bk = re15_enemy_find(0x10);
        CHECK(bk && bk->ok, "EM010-Bank geladen");
        if (bk && bk->ok) {
            const re15_emd_clip_t *fall = &bk->anim.clips[0x0b];
            const re15_emd_clip_t *up   = &bk->anim.clips[0x12];
            CHECK((bk->anim.frames[fall->first_frame + 14] >> 22) == 0x002,
                  "Fall-Clip 0x0b f14 -> SE 1");
            CHECK((bk->anim.frames[fall->first_frame + 46] >> 22) == 0x001,
                  "Fall-Clip 0x0b f46 -> SE 0 (Bodenaufprall)");
            int any = 0;
            for (int f = 0; f < up->frame_count; f++)
                if ((bk->anim.frames[up->first_frame + f] >> 22) != 0) any = 1;
            CHECK(any == 0, "Getup-Clip 0x12 traegt KEIN Frame-Wort-SE");
        }
    }
    printf(fails ? "\nFAILED (%d)\n" : "\nOK\n", fails);
    return fails ? 1 : 0;
}
