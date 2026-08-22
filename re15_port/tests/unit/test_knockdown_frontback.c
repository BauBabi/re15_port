/* test_knockdown_frontback.c — Front/Back-Latch (+0x93 Bit 0x80) und die Clip-Auswahl beim
 * Zu-Boden-Gehen. Beantwortet den Nutzer-Report "der liegende Zombie klingt beim Getroffenwerden
 * / Zu-Boden-Gehen / Aufstehen anders als die Dinner-Raum-Zombies".
 *
 * ================== ORIGINAL (Disasm) ==================
 *
 * (1) WER setzt +0x93 Bit 0x80 — game-weit GENAU ZWEI Stellen (eigener Scan aller 464 Schreib-
 *     zugriffe auf +0x93 ueber PSX.EXE + STAGE1-6: nur diese beiden setzen 0x80):
 *       @0x800123f4-f8  ori 0x80   — Spieler-Schuss, gated auf FUN_8001a780 (@0x800123dc)
 *       @0x80012fac-b0  ori 0x80   — Nahkampf/Gegner-Treffer, gated auf FUN_8001a7a8 (@0x80012f94)
 *     Beide Pfade LOESCHEN vorher alles ausser Bit 0 (@0x8001237c bzw. @0x80012f84-88
 *     `andi v0,v0,0x1; sb v0,147(s1)`) — der Latch wird also pro Treffer NEU berechnet.
 *
 * (2) DIE BEDINGUNG — ein +-90-Grad-Halbebenentest:
 *     FUN_8001a780 (Schuss; nur YAW-Differenz, KEINE Positionen):
 *       8001a788: lh v0,106(a0)      ; Opfer-Yaw  +0x6a
 *       8001a78c: lh v1,106(g_ent)   ; Schuetzen-Yaw
 *       8001a794: subu v0,v0,v1
 *       8001a798: addiu v0,v0,1024
 *       8001a79c: andi v0,v0,0xfff
 *       8001a7a4: slti v0,v0,2048    ; -> 1 = "von vorn"
 *     FUN_8001a7a8 (Nahkampf; Winkel zur Trefferquelle):
 *       8001a7cc: jal atan2_q12      ; Quelle -> Opfer
 *       8001a7e0: subu v0,v0,v1      ; - Opfer-Yaw
 *       8001a7e4: addiu v0,v0,1024   ; 8001a7e8: andi 0xfff ; 8001a7ec: slti 2048
 *
 * (3) WO der Latch WIRKT — NUR im TOD, nicht im Knockdown:
 *     FUN_80106c18 (Standing DEATH, +0x7-Phasenmaschine @0x80106c30):
 *       80106c98: lbu   v0,147(v1)   ; +0x93
 *       80106ca0: srl   v0,v0,7      ; Bit 0x80 -> 0/1
 *       80106ca4: sll   v0,v0,1      ; *2
 *       80106ca8: subu  v0,zero,v0   ; NEGIEREN
 *       80106cac: addiu v0,v0,13     ; Clip = 13 - 2*Bit  -> 0x0D hinten / 0x0B vorn
 *       80106cb4: sb    v0,148(v1)
 *       80106cc4-c8: +0x95 = rand & 3   (zufaelliger Startframe)
 *       80106cd4-d8: +0x8f = 0xf
 *     FUN_8010512c (KNOCKDOWN, Phase 0 @0x8010516c):
 *       80105190: ori v0,zero,0xb    ; Clip 0x0B — UNBEDINGT, kein Latch-Zugriff
 *       80105194: sb  v0,148(v1)
 *       801051a4: sb  zero,149(v0)   ; +0x95 = 0  (NICHT rand&3)
 *       801051c0-c8: +0x8f = 0xf
 *       801051cc-d0: rand&3 == 0 -> Grunzer (rand&1) ? 5 : 8
 *
 * ================== ERGEBNIS ==================
 * Der Front/Back-Latch kann die gemeldete Differenz NICHT erklaeren: das Zu-Boden-Gehen
 * (Knockdown, danach steht er wieder auf) spielt IMMER Clip 0x0B — unabhaengig von Yaw,
 * Position, Raum und Latch. Nur der TOD unterscheidet 0x0B/0x0D.
 * Frame-Wort-SEs: 0x0B f14->SE1 f46->SE0   |   0x0D f30->SE0 f48->SE0.
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

extern int g_test_room_se_log[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;
void re15_damage_seed_rng(uint32_t s);

static int g_fail = 0;
static void ok(int cond, const char *what, long got, long want)
{
    if (cond) printf("  OK   %-56s = %ld\n", what, got);
    else { printf("  FAIL %-56s = %ld (erwartet %ld)\n", what, got, want); g_fail++; }
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
    *out_size = (size_t)sz; return buf;
}

static uint8_t s_scratch[0x80000];

static int load_em10(const char *base)
{
    char emsp[600]; size_t n = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &n);
    if (!ems) return -1;
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0; int rc = -1;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_scratch) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
        if (eb) {
            memcpy(s_scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0)
                { eb->ok = 1; eb->buf = NULL; rc = 0; }
        }
    }
    free(ems); return rc;
}

/* Einen Knockdown fahren und die SE-Folge (Tick, ID, Frame) protokollieren.
 * `prior_sub` = der Zustand, AUS DEM der Zombie getroffen wird. */
static void run_knockdown(const char *tag, uint8_t prior_sub, int16_t enemy_yaw,
                          int *out_clip, int *out_se1_frame, int *out_se0_frame)
{
    re15_actor_init();
    re15_damage_seed_rng(0xA5A5F00Du);
    re15_actor_t *pl = &g_actors[0];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x10; e->grid_id = 0x00; e->hp = 60; e->hit_radius_min = 0;
    e->state = 1; e->sub_state_1 = prior_sub; e->sub_state_2 = 0;
    e->x = 1200; e->z = 0; e->rot_y = enemy_yaw;
    g_actor_count = 2;

    /* In den Knockdown schalten (sub 0x11, Phase 0) — genau der Zustand, den der
     * Poise-Break im Original erzeugt (FUN_8010512c). */
    e->sub_state_1 = 0x11; e->sub_state_2 = 0;

    g_test_room_se_n = 0;
    int clip = -1, se1_f = -1, se0_f = -1;
    printf("  [%s] yaw=%d prior_sub=0x%02x\n", tag, (int)enemy_yaw, prior_sub);
    for (int t = 0; t < 120; t++) {
        g_test_room_se_tick = t;
        int b4 = g_test_room_se_n;
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        if (clip < 0 && e->motion != 0) clip = (int)e->motion;
        for (int i = b4; i < g_test_room_se_n; i++) {
            int id = g_test_room_se_log[i];
            printf("       t=%-3d SE %-2d  clip=0x%02X frame=%u\n", t, id, (int)e->motion, e->anim_frame);
            if (id == 1 && se1_f < 0) se1_f = (int)e->anim_frame;
            if (id == 0 && se0_f < 0) se0_f = (int)e->anim_frame;
        }
        if (e->sub_state_1 != 0x11 && t > 4) break;
    }
    *out_clip = clip; *out_se1_frame = se1_f; *out_se0_frame = se0_f;
}

int main(void)
{
    /* ⛔ Der Default-Flavor ist seit 2026-08-22 RE2 (Nutzer-Entscheidung). Dieser PIN misst
     * byte-true RE1.5-Verhalten und muss den Modus deshalb EXPLIZIT setzen, statt sich auf
     * den Default zu verlassen. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    if (load_em10(base) != 0) { fprintf(stderr, "FAIL: em10-Bank nicht ladbar\n"); return 1; }
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    if (!b || !b->ok) { fprintf(stderr, "FAIL: keine em10-Bank\n"); return 1; }

    /* ---- PIN 1 — die beiden Fall-Clips und ihre SE-Signaturen (roh aus der EDD) ---- */
    printf("PIN 1 — Fall-Clips 0x0B / 0x0D:\n");
    {
        const re15_emd_clip_t *c0b = &b->anim.clips[0x0b];
        const re15_emd_clip_t *c0d = &b->anim.clips[0x0d];
        ok(c0b->frame_count == 55, "Clip 0x0B Frames", c0b->frame_count, 55);
        ok(c0d->frame_count == 55, "Clip 0x0D Frames", c0d->frame_count, 55);
        ok((b->anim.frames[c0b->first_frame + 14] >> 22) == 0x002, "0x0B f14 -> SE 1",
           (long)(b->anim.frames[c0b->first_frame + 14] >> 22), 2);
        ok((b->anim.frames[c0b->first_frame + 46] >> 22) == 0x001, "0x0B f46 -> SE 0",
           (long)(b->anim.frames[c0b->first_frame + 46] >> 22), 1);
        ok((b->anim.frames[c0d->first_frame + 30] >> 22) == 0x001, "0x0D f30 -> SE 0",
           (long)(b->anim.frames[c0d->first_frame + 30] >> 22), 1);
        ok((b->anim.frames[c0d->first_frame + 48] >> 22) == 0x001, "0x0D f48 -> SE 0",
           (long)(b->anim.frames[c0d->first_frame + 48] >> 22), 1);
    }

    /* ---- PIN 2 — die Todes-Formel Clip = 13 - 2*Bit (@0x80106ca0-ac) ---- */
    printf("PIN 2 — Todes-Clipwahl 13 - 2*(+0x93>>7):\n");
    {
        re15_actor_init();
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x10;
        e->hit_react = 0x00;                       /* Treffer von HINTEN */
        ok((int)((e->hit_react & 0x80) ? 0x0b : 0x0d) == 0x0d,
           "Bit 0x80 klar -> Clip 0x0D", (e->hit_react & 0x80) ? 0x0b : 0x0d, 0x0d);
        e->hit_react = 0x80;                       /* Treffer von VORN */
        ok((int)((e->hit_react & 0x80) ? 0x0b : 0x0d) == 0x0b,
           "Bit 0x80 gesetzt -> Clip 0x0B", (e->hit_react & 0x80) ? 0x0b : 0x0d, 0x0b);
        /* Gegenprobe zur negierten Form: 13 - 2*bit == die Port-Auswahl. */
        for (int bit = 0; bit <= 1; bit++)
            ok((13 - 2 * bit) == (bit ? 0x0b : 0x0d), "Formel 13-2*Bit == Port-Auswahl",
               13 - 2 * bit, bit ? 0x0b : 0x0d);
    }

    /* ---- PIN 3 — die MESSUNG (a)/(b)/(c) ---- */
    /* Der Knockdown ist latch-UNABHAENGIG (@0x80105190 `ori v0,zero,0xb` ohne +0x93-Zugriff).
     * Deshalb MUESSEN alle drei Faelle dieselbe SE-Folge liefern: SE 1 auf Frame 14,
     * SE 0 auf Frame 46 — egal ob der Zombie vorher lag, stand, oder in welchem Raum. */
    printf("PIN 3 — MESSUNG Knockdown-SE-Folge (a) lag / (b) stand / (c) Dinner-Steher:\n");
    {
        int clip_a, s1a, s0a, clip_b, s1b, s0b, clip_c, s1c, s0c;
        /* (a) kam aus der SCHLAF-/LIEGE-Submaschine (+0x5 = 0x12), Yaw wie der 10D0-Lieger */
        run_knockdown("a: lag (sub 0x12)", 0x12, 2048, &clip_a, &s1a, &s0a);
        /* (b) derselbe Zombie, nachdem er steht (+0x5 = 2 ENGAGE) */
        run_knockdown("b: stand (sub 0x02)", 0x02, 2048, &clip_b, &s1b, &s0b);
        /* (c) Dinner-Raum-Steher, andere Blickrichtung (Latch waere invertiert) */
        run_knockdown("c: Steher, Yaw 0", 0x02, 0, &clip_c, &s1c, &s0c);

        ok(clip_a == 0x0b, "(a) Fall-Clip", clip_a, 0x0b);
        ok(clip_b == 0x0b, "(b) Fall-Clip", clip_b, 0x0b);
        ok(clip_c == 0x0b, "(c) Fall-Clip", clip_c, 0x0b);
        ok(s1a == 14 && s1b == 14 && s1c == 14, "SE 1 in allen drei Faellen auf Frame 14",
           s1a * 10000 + s1b * 100 + s1c, 141414);
        ok(s0a == 46 && s0b == 46 && s0c == 46, "SE 0 in allen drei Faellen auf Frame 46",
           s0a * 10000 + s0b * 100 + s0c, 464646);
        ok(clip_a == clip_b && clip_b == clip_c,
           "(a)==(b)==(c): der Knockdown ist latch- und raum-unabhaengig", 1, 1);
    }

    printf("\n%s (%d Fehler)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
