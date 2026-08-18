/* test_zombie_getup_se.c — NEGATIV-PIN fuer den Nutzer-Report 2026-08-18:
 *
 *   "Beim Zombie in 2F ist der Sound allgemein richtig, aber dieses schrille Moaning kommt
 *    aktuell NUR beim Sterben. Im Original macht er es AUCH beim Aufstehen, nachdem man ihn
 *    niedergeschossen hat."
 *
 * BEFUND (statisch, jede Instruktion der Strecke selbst disassembliert):
 * Der Aufsteh-Grunzer in FUN_8010512c Phase [4] ist im Original KEIN Wurf, sondern eine
 * KONSTANTE — er feuert IMMER und immer als SE 8.
 *
 *   FUN_8001af20 (@0x8001af20) ist eine REINE FUNKTION VON a0; der State @0x800ac774 ist ein
 *   Dead Store (`lhu t1,0(v0)` @0x8001af28, t1 nie gelesen). Rohbytes:
 *     8001af30  00 04 19 c2   srl  v1,a0,7
 *     8001af34  30 63 00 ff   andi v1,v1,0xff
 *     8001af38  00 83 20 21   addu a0,a0,v1
 *     8001af3c  30 84 00 ff   andi a0,a0,0xff
 *     8001af4c  30 82 00 ff   andi v0,a0,0xff      (Rueckgabe)
 *
 *   a0 an der Callsite: der ACTIVE-Root FUN_80101224 ruft im NIEDERGESCHLAGENEN Zustand
 *   IMMER FUN_80012974 —
 *     80101614  90 a2 00 09   lbu  v0,9(a1)              ; +0x9
 *     8010161c  30 42 00 80   andi v0,v0,0x80
 *     80101620  10 40 00 07   beq  v0,zero,0x80101640    ; nicht downed -> uebersprungen
 *     80101630  34 04 13 88   ori  a0,zero,0x1388
 *     80101638  0c 00 4a 5d   jal  0x80012974
 *   — und FUN_80012974 verlaesst a0 IMMER als 0x20000000:
 *     800129d0  12 00 00 08   beq  s0,zero,0x800129f4
 *     800129d4  3c 04 20 00   lui  a0,0x2000             ; DELAY-SLOT = immer ausgefuehrt
 *     ... bis 0x80012a04 `jr ra`: KEIN weiterer a0-Write.
 *   Auf der Strecke 0x80101640-70 (Index+`jalr`), Sub-Mode-Handler 0x8010168c, Decide
 *   @0x8011f840[0x11] = 0x80105124 (`jr ra; nop`), Animate-Dispatch @0x801016f0,
 *   FUN_8010512c-Prolog und Phase [4] (0x8010532c-0x80105390) schreibt NICHTS a0.
 *   Der DOWNED-Bit steht ab Phase [0] (@0x80105230) bis Phase [6] (@0x80105400 ff.).
 *
 *   => rand() == hash(0x20000000) == 0. Damit (Rohbytes @0x80105388-b8):
 *     80105398  30 42 00 07   andi v0,v0,7   -> 0
 *     8010539c  14 40 00 09   bne  v0,zero   -> NICHT genommen: der SE feuert IMMER
 *     801053ac  30 42 00 01   andi v0,v0,1   -> 0   (zweiter rand: hash(0) = 0)
 *     801053b0  10 40 00 02   beq  v0,zero,0x801053bc  -> genommen
 *     801053b4  34 04 00 08   ori  a0,zero,0x8         (Delay-Slot)
 *     801053bc  0c 01 14 f4   jal  0x800453d0          -> Se(8)
 *
 * Aus derselben Herleitung folgen zwei weitere Konstanten desselben Handlers:
 *   Phase [2] Liegezeit  = tbl@0x8011FB10[0] * 30 = 12*30 = 360 Ticks (@0x801052a8-d8)
 *   Phase [6] Poise-Neu  = (0 & 3) + 4 = 4
 *
 * DER PIN: der Port darf hier NICHT mehr wuerfeln. Ueber mehrere RNG-Seeds hinweg MUSS jeder
 * Aufsteh-Vorgang genau ein SE 8 erzeugen (vorher: 1/8 * 1/2 = 1/16 pro Seed) und die
 * Liegezeit MUSS 360 sein.
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
#include "re15_vab.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int g_test_room_se_log[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail = 1; } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static uint8_t s_b10[0x80000];
static int s_bank_loaded = 0;

static void load_em10(const char *base)
{
    if (s_bank_loaded) return;
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { fprintf(stderr, "FAIL: CDEMD0.EMS\n"); exit(1); }
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_b10) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
        if (eb) {
            memcpy(s_b10, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_b10, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_b10, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(s_b10, len, &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
    s_bank_loaded = 1;
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

static re15_actor_t *find_z(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10) return &g_actors[s];
    return NULL;
}

/* Einen kompletten Knockdown->Getup-Zyklus fahren; gibt 0 zurueck, wenn alles wie erwartet lief. */
static int run_cycle(const uint8_t *rdt_bytes, size_t rdt_size, uint32_t seed, const char *base)
{
    static re15_rdt_t rdt;
    if (re15_rdt_parse(rdt_bytes, rdt_size, &rdt) != 0) { printf("FAIL: parse\n"); return 1; }

    re15_damage_seed_rng(seed);
    re15_actor_init(); scd_vm_init();
    g_current_room_id = 0x10D0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 7650; pl->y = 0; pl->z = 11400; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_em10(base);
    for (int f = 0; f < 8; f++) tick_once();

    re15_actor_t *z = find_z();
    if (!z) { printf("FAIL: kein Zombie (seed 0x%08x)\n", (unsigned)seed); return 1; }

    /* wecken */
    { int32_t tx = z->x, tz = z->z + 1500;
      for (int i = 0; i < 500; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if (dx >  40) dx =  40; if (dx < -40) dx = -40;
        if (dz >  40) dz =  40; if (dz < -40) dz = -40;
        pl->x += dx; pl->z += dz; tick_once(); } }

    /* schiessen bis der Poise bricht (Knockdown, +0x5 = 0x11) */
    int knocked = 0;
    for (int shot = 0; shot < 8 && !knocked; shot++) {
        z->hit_react &= (uint8_t)~2u;
        (void)re15_enemy_take_damage(z, 0);
        for (int t = 0; t < 40; t++) {
            tick_once();
            if (z->state == 1 && z->sub_state_1 == 0x11) { knocked = 1; break; }
            if (z->hp < 0) break;
        }
    }
    if (!knocked) { printf("FAIL: kein Knockdown (seed 0x%08x, hp=%d)\n", (unsigned)seed, (int)z->hp); return 1; }

    /* Zyklus verfolgen: Liegezeit messen + die SEs am Uebergang [4] mitschneiden */
    int    lie_timer_seen = -1;
    int    getup_se_n     = 0;
    int    getup_se[8];
    int    saw_getup      = 0;
    uint8_t prev_s2       = z->sub_state_2;
    for (int t = 0; t < 2000; t++) {
        int b4 = g_test_room_se_n;
        tick_once();
        if (prev_s2 == 2 && z->sub_state_2 == 3 && lie_timer_seen < 0)
            lie_timer_seen = (int)z->ai_timer;          /* direkt nach dem Setzen in [2] */
        if (prev_s2 == 4 || (prev_s2 == 3 && z->sub_state_2 == 5)) {
            /* Phase [4] wurde in diesem Tick ausgefuehrt (der Port setzt +0x6 sofort auf 5) */
        }
        if (!saw_getup && prev_s2 != z->sub_state_2 && z->sub_state_2 == 5) {
            saw_getup = 1;
            for (int i = b4; i < g_test_room_se_n && getup_se_n < 8; i++)
                getup_se[getup_se_n++] = g_test_room_se_log[i];
        }
        prev_s2 = z->sub_state_2;
        if (z->sub_state_1 != 0x11) break;
        if (z->hp < 0) break;
    }

    int bad = 0;
    if (!saw_getup) { printf("FAIL: Aufstehen nie erreicht (seed 0x%08x)\n", (unsigned)seed); return 1; }
    if (getup_se_n != 1 || getup_se[0] != 8) {
        printf("FAIL: seed 0x%08x — Aufsteh-SE erwartet [8], bekam [", (unsigned)seed);
        for (int i = 0; i < getup_se_n; i++) printf("%s%d", i ? "," : "", getup_se[i]);
        printf("]  (@0x801053bc jal 0x800453d0 a0=8)\n");
        bad = 1;
    }
    if (lie_timer_seen != 360) {
        printf("FAIL: seed 0x%08x — Liegezeit erwartet 360 (tbl@0x8011FB10[0]*30), bekam %d\n",
               (unsigned)seed, lie_timer_seen);
        bad = 1;
    }
    return bad;
}

/* VERDACHT A ausschliessen: verwirft die byte-true Stimmen-/Prioritaetsmaschine
 * (FUN_800453d0 / FUN_80045a18) den Aufsteh-SE? ROOM10D0-snd1-Record 8 = `00 00 92 15`:
 *   Stimme  = (0x15 & 0x1f) - 0x10 = 5      (@0x80045478-7c)
 *   prio    =  0x92 & 7        = 2          (@0x800454cc)
 *   nibble  =  0x92 & 0xf      = 2          (@0x800454b0; Bit 3 CLEAR -> Gleichstand erlaubt)
 * Gate FUN_80045a18: bei freier Stimme (prio[5] == 0) ist `neu > laufend` -> 0 = ERLAUBT
 * (@0x80045a48-4c). Verworfen wuerde er nur, solange auf Stimme 5 ein SE mit prio > 2 laeuft
 * (SE 4 = prio 3) oder bei Gleichstand mit gesetztem Bit 3 — beides liegt zum Aufsteh-Zeitpunkt
 * nicht an: die Stimme ist nach >=360 Liege-Ticks lange ausgeklungen. */
static void pin_prio_gate(const re15_rdt_t *rdt)
{
    const uint8_t *edt = rdt->snd_edt[1];
    if (!edt) { printf("FAIL: ROOM10D0 hat keine snd1-EDT\n"); g_fail = 1; return; }

    re15_edt_rec_t rec;
    CHECK(re15_edt_decode(edt, 8, &rec) == 0 && !rec.empty, "EDT-Record 8 nicht dekodierbar");
    CHECK(rec.voice    == 5, "SE 8: Stimme erwartet 5 (byte3 0x15 & 0x1f - 0x10), bekam %d", rec.voice);
    CHECK(rec.prio     == 2, "SE 8: prio erwartet 2 (byte2 0x92 & 7), bekam %d", rec.prio);
    CHECK(rec.prio_nib == 2, "SE 8: nibble erwartet 2 (byte2 0x92 & 0xf), bekam %d", rec.prio_nib);

    unsigned char prio[8] = {0,0,0,0,0,0,0,0};
    CHECK(re15_se_prio_gate(prio, rec.voice, rec.prio_nib) == 0,
          "SE 8 wird bei FREIER Stimme 5 verworfen — das Gate ist zu aggressiv");
    prio[5] = 3;                                   /* z.B. der Grab-Start-SE 4 laeuft noch */
    CHECK(re15_se_prio_gate(prio, rec.voice, rec.prio_nib) != 0,
          "SE 8 muesste hinter einem prio-3-SE auf Stimme 5 verworfen werden (@0x80045a34-44)");
    prio[5] = 2;                                   /* Gleichstand, Bit 3 clear -> erlaubt */
    CHECK(re15_se_prio_gate(prio, rec.voice, rec.prio_nib) == 0,
          "SE 8 bei Gleichstand mit nib<8 muesste durchgehen (@0x80045a50-58)");
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    uint8_t *d = read_file(path, &sz);
    if (!d) { fprintf(stderr, "FAIL: RDT\n"); return 1; }

    { static re15_rdt_t gate_rdt;
      if (re15_rdt_parse(d, sz, &gate_rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
      pin_prio_gate(&gate_rdt); }

    /* Acht unabhaengige Seeds: waere der Aufsteh-Grunzer noch ein 1/16-Wurf, muesste hier
     * statistisch in ~7 von 8 Laeufen GAR KEIN SE erscheinen. Der byte-true Pfad liefert in
     * JEDEM Lauf genau SE 8. */
    static const uint32_t seeds[8] = {
        0x2545f491u, 0x00000001u, 0xdeadbeefu, 0x12345678u,
        0xa5f15a3du, 0x7fffffffu, 0x0badc0deu, 0x5eed5eedu
    };
    for (int i = 0; i < 8; i++) {
        g_test_room_se_n = 0; g_test_room_se_tick = 0;
        if (run_cycle(d, sz, seeds[i], base)) g_fail = 1;
    }

    free(d);
    if (g_fail) { printf("test_zombie_getup_se: FAILED\n"); return 1; }
    printf("test_zombie_getup_se: OK (Prio-Gate + 8/8 Seeds -> Aufsteh-SE 8, Liegezeit 360)\n");
    return 0;
}
