/* test_anim_reverse_se.c — PIN: Rueckwaerts-Wiedergabe (anim_set a2) und der Frame-Wort-SE-Dekoder
 * lesen DENSELBEN Slot.
 *
 * ================== ORIGINAL-MECHANISMUS (Disasm) ==================
 *
 * anim_set FUN_8001f314 waehlt den Frame-Index anhand des 3. Arguments a2:
 *   8001f338: beq   t2,zero,0x8001f358   ; a2 == 0 -> VORWAERTS
 *   8001f344: lbu   v0,149(t0)           ;   sonst v0 = +0x95 (Cursor)
 *   8001f34c: subu  v0,t1,v0             ;   v0 = Framezahl - Cursor      (t1 = Framezahl,
 *   8001f354: addiu v0,v0,-1             ;   v0 = Framezahl - Cursor - 1   @0x8001f33c)
 *   8001f358: lbu   v0,149(t0)           ; vorwaerts: v0 = +0x95
 *   8001f364: sll   v0,v0,2
 *   8001f368: addu  a2,v1,v0             ; a2 = &frame_entry[index]
 *   8001f36c: sw    a2,360(t0)           ; +0x168 = Zeiger auf DAS Frame-Wort
 *
 * Der Cursor +0x95 laeuft in BEIDEN Richtungen aufwaerts (@0x8001f618 `addiu v0,v0,1`);
 * nur die Abbildung Cursor -> Slot spiegelt. Die "fertig"-Rueckgabe haengt am Cursor,
 * nicht an der Richtung (@0x8001f624 `sltu v0,v0,s4`).
 *
 * Der Frame-Wort-SE-Dekoder FUN_8001b38c liest EXAKT denselben Zeiger:
 *   8001b3a4: lw   v0,360(v0)            ; +0x168
 *   8001b3ac: lw   v0,0(v0)              ; das Frame-Wort
 *   8001b3b4: srl  s0,v0,22              ; obere 10 Bit = SE-Bitmaske
 *   8001b3c0: andi v0,s0,0x1             ; Bit N gesetzt -> SE N
 *   8001b3cc: jal  0x800453d0            ; = re15_audio_room_se(Bit-Index)
 * Pose und SE koennen im Original deshalb NICHT auseinanderlaufen. Der Port hielt die
 * Richtung als Aktor-Bit (anim_flags & 0x80) und las im SE-Dekoder trotzdem fest den
 * VORWAERTS-Slot -> bei jedem Rueckwaerts-Clip feuerten die SEs am gespiegelten Frame.
 *
 * Rueckwaerts-Call-Sites game-weit (eigener a2-Scan ueber alle 1018 `jal 0x8001f314`):
 * 38 Stellen mit a2 = 1, u.a. STAGE1 @0x80102af8 (Stagger-Back, Clip 0x11, a3 = 0x100
 * @0x80102afc) und @0x801049a0 (Feeding-Hinknien, Clip 0x29). Alle anderen 905 aufloesbaren
 * Stellen sind a2 = 0.
 *
 * ================== DIE PINS ==================
 *  PIN 1  Daten: Clip 0x11 = 60 Frames mit SE 1 auf Frame 42; die Clips des ROOM10D0-Liegers
 *         (0x2A Aufwachen, 0x29 Aufstehen) tragen NULL Frame-Wort-SEs.
 *  PIN 2  Die Spiegel-Regel selbst (re15_actor_playback_slot == @0x8001f34c-54 / @0x8001f358).
 *  PIN 3  LIVE: der Stagger-Back (Clip 0x11 RUECKWAERTS) feuert SE 1 auf Tick 17
 *         (= 60-1-42), nicht auf Tick 42. Das ist die Messung vorher/nachher.
 *  PIN 4  Kein haengengebliebenes Richtungs-Bit: der Aufsteher (a2 = 0 @0x80104af0) und der
 *         Aufwach-Clip (a2 = 0 @0x8010559c) laufen vorwaerts, auch direkt nach einem
 *         Rueckwaerts-Clip.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_anim_select.h"
#include "re15_ems.h"
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int g_test_room_se_log[2048];
extern int g_test_room_se_tickof[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;
void re15_damage_seed_rng(uint32_t s);

static int g_fail = 0;
static void ok(int cond, const char *what, long got, long want)
{
    if (cond) printf("  OK   %-58s = %ld\n", what, got);
    else { printf("  FAIL %-58s = %ld (erwartet %ld)\n", what, got, want); g_fail++; }
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

static int load_em10(const char *base)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) return -1;
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0; int rc = -1;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_scratch) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
        if (eb) {
            memcpy(s_scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL; rc = 0;
            }
        }
    }
    free(ems);
    return rc;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    if (load_em10(base) != 0) { fprintf(stderr, "FAIL: em10-Bank nicht ladbar\n"); return 1; }
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    if (!b || !b->ok) { fprintf(stderr, "FAIL: keine em10-Bank\n"); return 1; }

    /* ---------------- PIN 1 — die Frame-Wort-SE-Daten ---------------- */
    printf("PIN 1 — Frame-Wort-SE-Bits (Wort >> 22, roh aus der EDD):\n");
    const re15_emd_clip_t *c11 = &b->anim.clips[0x11];
    ok(c11->frame_count == 60, "Clip 0x11 (Stagger-Back) Frames", c11->frame_count, 60);
    {
        int se_frame = -1, n = 0;
        for (int f = 0; f < c11->frame_count; f++) {
            uint32_t m = b->anim.frames[c11->first_frame + f] >> 22;
            if (m) { n++; if (se_frame < 0 && (m & 2u)) se_frame = f; }
        }
        ok(n == 1, "Clip 0x11: genau ein SE-Frame", n, 1);
        ok(se_frame == 42, "Clip 0x11: SE 1 auf Frame 42", se_frame, 42);
    }
    /* Die Clips des ROOM10D0-Liegers sind STUMM — deshalb kann die Richtungs-Frage die
     * gemeldete Geraeusch-Differenz beim Aufwachen/Aufstehen NICHT erklaeren. */
    for (int ci = 0; ci < 2; ci++) {
        int clip = ci ? 0x2a : 0x29;
        const re15_emd_clip_t *cc = &b->anim.clips[clip];
        int n = 0;
        for (int f = 0; f < cc->frame_count; f++)
            if ((b->anim.frames[cc->first_frame + f] >> 22) != 0) n++;
        char lbl[80];
        snprintf(lbl, sizeof lbl, "Clip 0x%02X (%s): Frame-Wort-SEs", clip,
                 clip == 0x29 ? "Aufstehen" : "Aufwachen");
        ok(n == 0, lbl, n, 0);
    }

    /* ---------------- PIN 2 — die Spiegel-Regel ---------------- */
    printf("PIN 2 — Spiegel-Regel (@0x8001f34c-54 vs @0x8001f358):\n");
    {
        re15_actor_init();
        re15_actor_t *a = &g_actors[1];
        a->active = 1; a->type = 0x10;
        a->anim_flags = 0;                                  /* a2 = 0 */
        ok(re15_actor_anim_reverse(a) == 0, "vorwaerts: reverse-Flag", re15_actor_anim_reverse(a), 0);
        ok(re15_actor_playback_slot(a, 0, 60) == 0,  "vorwaerts: Cursor 0  -> Slot", re15_actor_playback_slot(a, 0, 60), 0);
        ok(re15_actor_playback_slot(a, 17, 60) == 17, "vorwaerts: Cursor 17 -> Slot", re15_actor_playback_slot(a, 17, 60), 17);
        ok(re15_actor_playback_slot(a, 59, 60) == 59, "vorwaerts: Cursor 59 -> Slot", re15_actor_playback_slot(a, 59, 60), 59);
        a->anim_flags = 0x80;                               /* a2 = 1 */
        ok(re15_actor_anim_reverse(a) == 1, "rueckwaerts: reverse-Flag", re15_actor_anim_reverse(a), 1);
        ok(re15_actor_playback_slot(a, 0, 60) == 59, "rueckwaerts: Cursor 0  -> Slot", re15_actor_playback_slot(a, 0, 60), 59);
        ok(re15_actor_playback_slot(a, 17, 60) == 42, "rueckwaerts: Cursor 17 -> Slot (= SE-Frame)", re15_actor_playback_slot(a, 17, 60), 42);
        ok(re15_actor_playback_slot(a, 59, 60) == 0,  "rueckwaerts: Cursor 59 -> Slot", re15_actor_playback_slot(a, 59, 60), 0);
    }

    /* ---------------- PIN 3 — LIVE: SE folgt der Richtung ---------------- */
    /* Stagger-Back: Clip 0x11 RUECKWAERTS (a2 = 1 @0x80102aec, `jal` @0x80102af8,
     * a3 = 0x100 @0x80102afc, +0x8f = 0xf). Der SE-Frame 42 wird bei Cursor 17
     * erreicht (60-1-42). Vor dem Fix las der Dekoder den Vorwaerts-Slot -> Tick 42. */
    printf("PIN 3 — LIVE Stagger-Back (Clip 0x11 rueckwaerts):\n");
    {
        re15_actor_init();
        re15_damage_seed_rng(0x0BADC0DEu);
        re15_actor_t *pl = &g_actors[0];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 30000; pl->z = 30000;
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x10; e->grid_id = 0x00; e->hp = 60; e->hit_radius_min = 0;
        /* Sub 0x0e: kein Handler (Dispatch-default) -> die FSM schreibt den Aktor NICHT um,
         * und die Bank-Regel liefert die AKTIONS-Bank (Loco gilt nur fuer +0x5 in {2,7,0x13}
         * bzw. den State-2-Stagger, re15_actor_uses_loco_bank) — dort liegt Clip 0x11. */
        e->state = 1; e->sub_state_1 = 0x0e;
        e->motion = 0x11; e->anim_frame = 0; e->anim_frac = 0xf;
        e->anim_blend_rate = 0x100;                   /* a3 = 0x100 @0x80102afc */
        e->anim_flags = 0x80;                         /* a2 = 1  @0x80102aec */
        e->x = 0; e->z = 0;
        g_actor_count = 2;
        g_test_room_se_n = 0;
        /* Gemessen wird der CURSOR (+0x95) im Moment des SE — das ist die byte-true Groesse.
         * (Der Schleifenindex liegt um 1 darunter, weil der Advancer VOR der AI laeuft.) */
        int se1_cursor = -1;
        for (int t = 0; t < 60 && se1_cursor < 0; t++) {
            g_test_room_se_tick = t;
            int b4 = g_test_room_se_n;
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
            for (int i = b4; i < g_test_room_se_n; i++)
                if (g_test_room_se_log[i] == 1 && se1_cursor < 0) se1_cursor = (int)e->anim_frame;
        }
        printf("     [Messung] SE 1 bei Cursor +0x95 = %d -> Pose-Slot %d (SE-Frame ist 42)\n",
               se1_cursor, se1_cursor >= 0 ? 59 - se1_cursor : -1);
        /* Rueckwaerts: Slot = 60-1-Cursor (@0x8001f34c-54). Slot 42 <=> Cursor 17.
         * VOR dem Fix las der Dekoder den Vorwaerts-Slot -> SE erst bei Cursor 42. */
        ok(se1_cursor == 17, "Stagger-Back: SE 1 bei Cursor 17 (nicht 42)", se1_cursor, 17);
    }

    /* ---------------- PIN 4 — kein haengengebliebenes Richtungs-Bit ---------------- */
    /* Im Original ist die Richtung ein PRO-AUFRUF-Argument; im Port ein Aktor-Bit. Der
     * Aufsteher (a2 = 0 @0x80104af0) darf das Bit des Hinkniens (a2 = 1 @0x80104994) NICHT
     * erben. decide[0xc] FUN_801048a8 springt bei dist < 0xbb8 (@0x801048bc) SOFORT auf
     * +0x4 = 0xd01 (@0x801048d8-dc) — auch mitten im Hinknien. */
    printf("PIN 4 — Richtungs-Bit haengt nicht (Aufsteher a2 = 0 @0x80104af0):\n");
    {
        re15_actor_init();
        re15_damage_seed_rng(0x11112222u);
        re15_actor_t *pl = &g_actors[0];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0;
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x10; e->hp = 100; e->hit_radius_min = 0;
        e->state = 1; e->sub_state_1 = 0x0d;          /* Standup-Sub (@0x8011f890[0xd]) */
        e->sub_state_2 = 0;                           /* Phase 0 = Clip 0x29 setzen */
        e->motion = 0x27; e->anim_frame = 0;
        e->anim_flags = 0x80;                         /* STALE Rueckwaerts-Bit vom Hinknien */
        e->x = 600; e->z = 0;
        g_actor_count = 2;
        re15_enemy_ai_run_all(0);
        ok(e->motion == 0x29, "Standup setzt Clip 0x29", (int)e->motion, 0x29);
        ok((e->anim_flags & 0x80) == 0,
           "Standup loescht das Rueckwaerts-Bit", (e->anim_flags & 0x80) ? 1 : 0, 0);
        ok(re15_actor_playback_slot(e, 5, 59) == 5,
           "Aufsteher posiert vorwaerts (Cursor 5 -> Slot 5)",
           re15_actor_playback_slot(e, 5, 59), 5);
    }

    printf("\n%s (%d Fehler)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
