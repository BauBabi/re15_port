/* test_anim_rate.c — PIN auf die byte-true Semantik des 4. anim_set-Arguments (a3).
 *
 * ANLASS: die Vermutung, a3 sei eine Q8-ANIMATIONS-RATE (0x100 = 1.0 Frame/Tick,
 * 0x200 = 2.0), und der Port laufe deshalb port-weit "halb so schnell". Diese
 * Vermutung ist am Original WIDERLEGT; dieser Test friert die Widerlegung ein,
 * damit sie nicht versehentlich "repariert" wird.
 *
 * ================== WAS a3 WIRKLICH IST (Disasm, selbst nachgelesen) ==================
 *
 * FUN_8001f314 (anim_set) @0x8001f314 ist nur ein Dispatcher. Er sign-extendet a3
 * und reicht es weiter:
 *   8001f380: sll v0,a3,16    8001f384: sra v0,v0,16    8001f388: sw v0,16(sp)
 *   8001f38c: jal 0x8001f3bc                      ; a3 als 5. (Stack-)Argument
 *   8001f3a0: sll a3,a3,16    8001f3a4: jal 0x8001f8b4    8001f3a8: sra a3,a3,16
 *
 * In FUN_8001f3bc wird a3 AUSSCHLIESSLICH als CROSSFADE-GEWICHTSSCHRITT benutzt,
 * multipliziert mit dem Crossfade-Zaehler entity+0x8f:
 *   8001f3ec: lw a1,72(sp)          ; a1 = die a3-Rate (Caller-Slot 16(sp))
 *   8001f408: lbu s0,143(v1)        ; s0 = +0x8f  (Crossfade-Zaehler)
 *   8001f468: sll v0,a1,16   8001f46c: sra v0,v0,16
 *   8001f474: mult v0,a3            ; Produkt = Rate * +0x8f
 *   8001f4a8: ori v0,zero,0x1000
 *   8001f4ac: mflo v1    8001f4b0: subu v0,v0,v1   ; GTE IR0 = 0x1000 - Rate*+0x8f
 *   -> gpf12/gpl12 blenden NEUE Pose gegen die VORIGE mit genau diesem Gewicht.
 *   Zweite Nutzung fuer die Knochenwinkel:
 *   8001f598: mult v0,v1     8001f5cc: ori a3,zero,0x1000   8001f5d4: subu a3,a3,s3
 *            -> FUN_80020510(..., 0x1000 - Rate*+0x8f)
 *   +0x8f dekrementiert einmal pro Aufruf: 8001f5a8: lbu v0,143(v1)
 *                                          8001f5b0: addiu v0,v0,-1
 *                                          8001f5b4: sb v0,143(v1)
 *
 * DER FRAME-VORSCHUB IST VON a3 VOELLIG UNABHAENGIG — immer exakt +1:
 *   8001f610: lbu   v0,149(v1)      ; +0x95 = Frame-Cursor (BYTE)
 *   8001f618: addiu v0,v0,1         ; +1  — KEINE Rate, KEIN Sub-Frame-Rest
 *   8001f61c: sb    v0,149(v1)
 *   8001f624: sltu  v0,v0,s4        ; s4 = Framezahl des Clips
 *   8001f628: bne   -> weiterlaufen
 *   8001f638: ori v0,zero,0x1   8001f63c: sb zero,149(v1)   ; fertig -> 1, Cursor = 0
 * FUN_8001f8b4 hat exakt denselben Epilog (`+0x95 + 1`, Wrap gegen dieselbe Framezahl).
 * Es gibt KEIN Sub-Frame-Rest-Feld: der Cursor ist ein Byte und wird nur inkrementiert.
 *
 * FOLGE: ein Clip braucht immer GENAU so viele Ticks wie er Frames hat, egal ob der
 * Caller 0x100, 0x200 oder 0x400 uebergibt. a3 steuert nur, WIE LANG die Vor-Pose
 * ausblendet: Crossfade-Laenge = 0x1000 / Rate.
 *
 * Die drei game-weit vorkommenden Raten (Scan aller 1018 `jal 0x8001f314`-Call-Sites
 * ueber PSX.EXE + STAGE1-6/TITLE: nur 0x200 (813x), 0x100 (157x), 0x400 (44x); die
 * 4 registergefuehrten Sites in PSX.EXE loeschen vorher +0x8f, a3 ist dort egal)
 * sind im Original IMMER mit einem passenden +0x8f-Startwert gepaart:
 *
 *   Rate    +0x8f   Produkt   Crossfade   Belegstellen (+0x8f / a3)
 *   0x100   0xf     0xf00     16 Ticks    @0x80103a28-30  /  @0x80103b14
 *   0x200   0x7     0xe00      8 Ticks    @0x80104968-6c  /  @0x801049a0-a4
 *                                         @0x80104ac8-d0  /  @0x80104afc-b00
 *   0x400   0x3     0xc00      4 Ticks    @0x8010691c-20  /  @0x801069b8-bc
 *
 * INVARIANTE: das Produkt bleibt IMMER < 0x1000 — das Original klemmt nie und
 * erzeugt nie ein 100%-Alt-Pose-Plateau.
 *
 * ================== DIE PINS ==================
 *  PIN 1  Daten: em10-Bank1 = 43 Clips, fc[0x29] = 59, fc[0x2A] = 28 (roh aus der
 *         EDD von CDEMD0.EMS gelesen; genau die Woerter, die FUN_8001f3bc @0x8001f334
 *         als `andi t1,v0,0xffff` liest).
 *  PIN 2  Vorschub = 1 Frame/Tick auf dem 0x200-Clip 0x2A (Aufwach-Clip des
 *         ROOM10D0-Liegers, Call-Site @0x801055a8 mit `ori a3,zero,0x200` @0x801055ac).
 *         Unter der widerlegten Raten-Hypothese waere das 2 Frames/Tick -> ROT.
 *  PIN 3  Gegenbeispiel Rate 0x100 (Liege-Wake FUN_80103a58 Phase 2, a3 = 0x100
 *         @0x80103b14): identischer Vorschub 1 Frame/Tick — muss unveraendert bleiben.
 *  PIN 4  Crossfade-Paarung: frac*rate saettigt nie (< 0x1000), inklusive des vom
 *         Port beim Feeding->Standup gesetzten Paares (Clip 0x29, +0x8f = 7 @0x80104968).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_ai_flavor.h"   /* Default ist seit 2026-08-22 RE2 — dieser PIN misst RE1.5 */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int g_fail = 0;

static void ok(int cond, const char *what, long got, long want)
{
    if (cond) {
        printf("  OK   %-58s = %ld\n", what, got);
    } else {
        printf("  FAIL %-58s = %ld (erwartet %ld)\n", what, got, want);
        g_fail++;
    }
}

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

static uint8_t s_scratch[0x80000];

static int load_bank(const char *base, uint8_t type)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { fprintf(stderr, "FAIL: %s nicht lesbar\n", emsp); return -1; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    int rc = -1;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_scratch) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(s_scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                rc = 0;
            }
        }
    }
    free(ems);
    return rc;
}

/* Einen Zombie so aufstellen, dass der globale Advancer ihn NICHT auslaesst, und
 * dann N Ticks advancen. Rueckgabe: anim_frame nach N Ticks. */
static int advance_ticks(uint8_t motion, uint8_t s1, uint8_t s2, uint8_t grid, int ticks)
{
    re15_actor_init();
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x10; e->hp = 100;
    e->state = 1; e->sub_state_1 = s1; e->sub_state_2 = s2; e->grid_id = grid;
    e->motion = motion; e->anim_frame = 0; e->anim_frac = 0;
    e->anim_flags = 0;              /* play-once: kein LOOP-Bit (0x04) */
    e->motion_init_delay = 0;
    for (int t = 0; t < ticks; t++) re15_actors_anim_advance();
    return (int)e->anim_frame;
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);   /* byte-true RE1.5 messen, nicht den neuen Default */
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);

    if (load_bank(base, 0x10) != 0) {
        fprintf(stderr, "FAIL: em10-Bank nicht ladbar\n");
        return 1;
    }
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    if (!b || !b->ok) { fprintf(stderr, "FAIL: keine em10-Bank\n"); return 1; }

    /* ---------------- PIN 1 — die Framezahlen kommen roh von der Disc ---------------- */
    printf("PIN 1 — EDD-Framezahlen (em10 Bank 1, dir[3]):\n");
    ok(b->anim.clip_count == 43, "em10 clip_count", b->anim.clip_count, 43);
    int fc29 = b->anim.clip_count > 0x29 ? b->anim.clips[0x29].frame_count : -1;
    int fc2a = b->anim.clip_count > 0x2a ? b->anim.clips[0x2a].frame_count : -1;
    /* Beide Werte stehen woertlich als low16 des EDD-Clipworts auf der Disc — genau das,
     * was FUN_8001f3bc @0x8001f334/@0x8001f33c (`lw v0,0(a2)` / `andi t1,v0,0xffff`) als
     * Wrap-Grenze laedt. KEINE Halbierung: 14/30 waeren die (widerlegte) Raten-Hypothese. */
    ok(fc29 == 59, "fc[0x29] Aufsteh-Clip (Frames)", fc29, 59);
    ok(fc2a == 28, "fc[0x2A] Aufwach-Clip (Frames)", fc2a, 28);

    /* ---------------- PIN 2 — Rate 0x200: trotzdem 1 Frame/Tick ---------------- */
    /* Aufwach-Clip 0x2A des ROOM10D0-Liegers: Call-Site @0x801055a8, a3 = 0x200
     * @0x801055ac. Die Schlafmaschine FUN_801054f4 ruft anim_set GENAU EINMAL pro Tick
     * (Sprungtabelle @0x801000e4: [2] = 0x80105554 startet, [3] = 0x80105594 spielt ab;
     * die decide-Row @0x8011f840[0x12] = FUN_80105470 enthaelt KEIN jal 0x8001f314).
     * Also: Tick k -> Frame k. Unter "0x200 = 2 Frames/Tick" waere es Frame 2k. */
    printf("PIN 2 — Vorschub bei a3 = 0x200 (Clip 0x2A, @0x801055ac):\n");
    for (int k = 1; k <= 5; k++) {
        char lbl[80];
        snprintf(lbl, sizeof lbl, "Clip 0x2A: anim_frame nach %d Ticks", k);
        ok(advance_ticks(0x2a, 0x12, 2, 0x00, k) == k, lbl,
           advance_ticks(0x2a, 0x12, 2, 0x00, k), k);
    }
    /* Der ganze Clip: nach fc-1 = 27 Ticks steht der LETZTE Frame (27). Danach haelt der
     * Port play-once auf fc-1 (Emulation des "Caller ruft anim_set nicht mehr"). */
    ok(advance_ticks(0x2a, 0x12, 2, 0x00, 27) == 27,
       "Clip 0x2A: letzter Frame nach 27 Ticks", advance_ticks(0x2a, 0x12, 2, 0x00, 27), 27);
    ok(advance_ticks(0x2a, 0x12, 2, 0x00, 40) == 27,
       "Clip 0x2A: play-once Hold auf fc-1", advance_ticks(0x2a, 0x12, 2, 0x00, 40), 27);

    /* Aufsteh-Clip 0x29 (59 Frames), Call-Sites @0x801049a0 und @0x80104afc, beide a3 = 0x200. */
    ok(advance_ticks(0x29, 0x11, 4, 0x00, 30) == 30,
       "Clip 0x29: anim_frame nach 30 Ticks", advance_ticks(0x29, 0x11, 4, 0x00, 30), 30);
    ok(advance_ticks(0x29, 0x11, 4, 0x00, 58) == 58,
       "Clip 0x29: letzter Frame nach 58 Ticks", advance_ticks(0x29, 0x11, 4, 0x00, 58), 58);

    /* ---------------- PIN 3 — Gegenbeispiel Rate 0x100 ---------------- */
    /* Liege-Wake FUN_80103a58 Phase 2 spielt +0x94 (Clip 0x13, 120 Frames) mit
     * a3 = 0x100 @0x80103b14. Der Vorschub ist IDENTISCH — a3 aendert ihn nicht.
     * Freigabe des 0x12/0x13-Liege-Pins im Advancer: Nibble 9, +0x5 = 0, +0x6 >= 2. */
    printf("PIN 3 — Gegenbeispiel a3 = 0x100 (Clip 0x13, @0x80103b14):\n");
    for (int k = 1; k <= 5; k++) {
        char lbl[80];
        snprintf(lbl, sizeof lbl, "Clip 0x13: anim_frame nach %d Ticks", k);
        ok(advance_ticks(0x13, 0x00, 2, 0x09, k) == k, lbl,
           advance_ticks(0x13, 0x00, 2, 0x09, k), k);
    }
    ok(advance_ticks(0x13, 0x00, 2, 0x09, 40) == 40,
       "Clip 0x13: nach 40 Ticks (kein 0x100-Sonderweg)",
       advance_ticks(0x13, 0x00, 2, 0x09, 40), 40);

    /* ---------------- PIN 4 — Crossfade-Paarung saettigt nie ---------------- */
    /* Byte-true Paare (+0x8f, a3). Produkt = Gewicht der VORIGEN Pose in Q12
     * (FUN_8001f3bc @0x8001f474 `mult v0,a3`, IR0 = 0x1000 - Produkt @0x8001f4a8-b0).
     * Saettigt das Produkt (>= 0x1000), zeigt der Port 100% alte Pose = eingefrorenes
     * Plateau. Im Original passiert das NIE. */
    printf("PIN 4 — Crossfade-Paarung frac*rate < 0x1000:\n");
    struct { int frac, rate; const char *cite; } pairs[] = {
        { 0x0f, 0x100, "+0x8f@0x80103a28 / a3@0x80103b14"  },
        { 0x07, 0x200, "+0x8f@0x80104968 / a3@0x801049a4"  },
        { 0x07, 0x200, "+0x8f@0x80104ac8 / a3@0x80104b00"  },
        { 0x03, 0x400, "+0x8f@0x8010691c / a3@0x801069bc"  },
    };
    for (unsigned i = 0; i < sizeof pairs / sizeof pairs[0]; i++) {
        int prod = pairs[i].frac * pairs[i].rate;
        char lbl[100];
        snprintf(lbl, sizeof lbl, "frac 0x%x * rate 0x%x  [%s]",
                 pairs[i].frac, pairs[i].rate, pairs[i].cite);
        ok(prod < 0x1000, lbl, prod, 0x0fff);
    }

    /* Und das Paar, das der Port beim Feeding->Standup selbst setzt: Clip 0x29 wird
     * game-weit NUR mit +0x8f = 7 geladen (@0x80104968 Feeding-Phase0, @0x80104ac8
     * Standup-Phase0) — beide mit a3 = 0x200. Ein 0xf waere 0x1e00 = geklemmtes
     * 100%-Plateau ueber die ersten 8 der 59 Aufsteh-Frames. */
    {
        re15_actor_init();
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = 0; pl->y = 0; pl->z = 0;
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x10; e->hp = 100;
        e->state = 1; e->grid_id = 0x05;      /* Nibble 5 = feeding (@0x8011f80c[5]) */
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->motion = 0x27; e->anim_frame = 0; e->anim_frac = 0;
        e->x = 700; e->y = 0; e->z = 0;       /* dist < 4000 -> Wake-Stufe A */

        int frac_at_standup = -1;
        for (int t = 0; t < 200; t++) {
            re15_enemy_ai_live_step(1);
            if (e->motion == 0x29) { frac_at_standup = e->anim_frac; break; }
            re15_actors_anim_advance();
        }
        ok(frac_at_standup == 7,
           "Feeding->Standup Clip 0x29: +0x8f (byte-true 7 @0x80104968)",
           frac_at_standup, 7);
        if (frac_at_standup >= 0)
            ok(frac_at_standup * 0x200 < 0x1000,
               "  -> frac*0x200 saettigt nicht", frac_at_standup * 0x200, 0x0fff);
    }

    printf("\n%s (%d Fehler)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
