/* test_re2_crow_shadow_release.c — PIN zum Nutzer-Report 2026-08-21:
 * "Platzende Kraehen durch die Pistole hinterlassen immer noch Schatten, die muessen dann
 *  natuerlich weg."
 *
 * Quelle: info/re2leon/COMMON/BIN/EMOVL21_S0.BIN (Kraehen-Overlay, Slot 0 @0x80100000) +
 * info/re2leon/PSX.EXE. Disassemblieren mit
 *   .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> [--bin EMOVL21_S0.BIN]
 *
 * DER MECHANISMUS (selbst gelesen, jede Zeile zitiert):
 *   Der Boden-Schatten ist ein RECORD aus einem 50-Slot-Pool. Allokator 0x80016480 (PSX.EXE):
 *     80016488  addiu t0,t0,-6504     ; Pool-Basis 0x800CE698
 *     800164a8  addiu t0,t0,104       ; Stride 104
 *     800164ac  lbu   v0,14(t0)       ; rec+0x0E == 0  =>  Slot FREI
 *     800164c4  addiu v0,zero,5
 *     800164d0  sw    t0,0(a0)        ; ent+0x16C = Record-Zeiger
 *     800164d4  sb    v0,14(t0)       ; rec+0x0E = 5   =>  Slot BELEGT
 *     80016530  sw    a2,4(t0)        ; rec+0x04/+0x06 = die zwei Halb-Ausdehnungen
 *     80016500  lui a1,0x80 / ori a1,a1,0x8080  ; a3==0 -> Farbe 0x00808080 = NEUTRALGRAU
 *   Umfaerben: 0x80016FE4(rec, farbe)
 *     80016fe8  lw v0,28(a0) / 80016fec lw v1,68(a0)   ; Farbwoerter rec+0x1C und rec+0x44
 *     80016ff0-ffc  and 0xff000000 / or a1
 *     80017000/08   sw v0,28(a0) / sw v1,68(a0)
 *
 *   KRAEHE INIT @0x80100400: 0x80016480(+0x16C, 0, 0x00C800C8, 0) -> 200x200, Farbe GRAU.
 *   CORPSE Sub 0 (Normal) @0x8010398C:
 *     80103a08/0c lui a1,0xbf / ori a1,a1,0xbf10   ; 0x00BFBF10
 *     80103a14    lw  a0,364(s0)                   ; [+0x16C]
 *     80103a20    jal 0x80016fe4                   ; RECOLOR -> BLUTLACHE
 *     80103a2c-3c 400 -> rec+0x04 und rec+0x06 ; 80103a40-44 60 Grow-Ticks (+10, @0x80103AB8-FC)
 *   CORPSE Sub 3 (Launch) @0x80103EB0: dieselben Zeilen @0x80103EF4-F18 / @0x80103F1C-38.
 *   CORPSE Sub 1 (GIB) @0x80103C0C:
 *     80103c24  lw v1,364(a0)
 *     80103c34  sb zero,14(v1)      ; BYTE-Store: rec+0x0E = 0  =>  SLOT FREIGEGEBEN
 *   CORPSE Sub 2 (Wandsplat) @0x80103C44: @0x80103CA8 / @0x80103CBC dasselbe.
 *
 * DER FEHLER, den dieser Test festnagelt: der Port bildet die Freigabe auf
 * crow_shadow_w/h == 0 ab, der PC-Schattenrenderer testete aber `crow_shadow_w != 0` und fiel
 * bei 0 auf seinen 500x600-DEFAULT zurueck — also blieb unter der geplatzten Kraehe ein ganz
 * normaler Charakter-Schatten liegen. Zweitens blieb die NORMALE Leiche auf der GRAUEN
 * Alloc-Farbe stehen, weil der Recolor als "Modell-Tint, Render-OFFEN" abgetan war.
 *
 * Die Render-Entscheidung (platform/pc/main.c, dort nicht linkbar) ist unten 1:1 nachgezogen —
 * dasselbe Muster wie probe_re2z_corpse.c.
 */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static re15_actor_t *PL;
static re15_actor_t *CROW;

/* --- 1:1 aus platform/pc/main.c (Kraehen-Zweig des NPC-Boden-Schattens) ------------------- */
typedef struct { int quad; int hx, hz; int pool; int tinted; int r, g, b; } shadow_t;
static void render_decide(const re15_actor_t *n, shadow_t *o)
{
    memset(o, 0, sizeof *o);
    o->hx = 500; o->hz = 600;                       /* Default-Charakterschatten */
    int crow_no_record = 0;
    if (n->type == 0x21 && re15_ai_flavor() == RE15_AI_FLAVOR_RE2 && n->crow_shadow_w == 0)
        crow_no_record = 1;
    if (n->type == 0x21 && n->crow_shadow_w != 0) {
        o->hx = n->crow_shadow_w; o->hz = n->crow_shadow_h;
        if (n->crow_pool) {
            o->pool = 1;
            if (re15_ai_flavor() == RE15_AI_FLAVOR_RE2) { o->r = 0x10; o->g = 0xBF; o->b = 0xBF; }
            else                                        { o->r = 0x38; o->g = 0xff; o->b = 0xff; }
        } else {
            int tv = (int)n->crow_tint * 2; if (tv > 255) tv = 255;
            o->r = o->g = o->b = tv; o->tinted = 1;
        }
    }
    o->quad = !crow_no_record;
}

static void fresh_crow(int32_t px, int32_t pz)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL   = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    CROW = &g_actors[1];
    memset(PL,   0, sizeof *PL);
    memset(CROW, 0, sizeof *CROW);
    PL->active = 1; PL->hp = 100; PL->x = px; PL->z = pz;
    CROW->active = 1; CROW->type = 0x21; CROW->state = 0;
}

static void tick(void)
{
    re15_actor_t *e = CROW;
    int32_t dx = PL->x - e->x, dz = PL->z - e->z;
    int64_t d2 = (int64_t)dx * dx + (int64_t)dz * dz;
    uint32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
    e->ai_dist = r;
    re15_re2crow_tick(1);
}

/* Die Kraehe direkt in eine CORPSE-Zeile setzen und EINEN Corpse-Tick fahren. */
static void corpse_sub(int sub)
{
    CROW->state = 7; CROW->sub_state_1 = (uint8_t)sub; CROW->sub_state_2 = 0;
    CROW->hp = -1;
    tick();
}

int main(void)
{
    /* ---- (1) GIB-LEICHE: Record FREIGEGEBEN -> der Port darf NICHTS zeichnen ------------- */
    {
        fresh_crow(20000, 0);
        tick();                                     /* INIT @0x801002FC: 200x200, Farbe grau */
        CHECK(CROW->crow_shadow_w != 0,
              "Vorbedingung: die lebende Kraehe HAT einen Record (w=%u)", CROW->crow_shadow_w);
        shadow_t s; render_decide(CROW, &s);
        CHECK(s.quad == 1, "lebende Kraehe muss einen Schatten zeichnen");

        corpse_sub(1);                              /* CORPSE Sub 1 = GIB @0x80103C0C */
        CHECK(CROW->crow_shadow_w == 0 && CROW->crow_shadow_h == 0,
              "GIB: `sb zero,14(rec)` @0x80103C34 = Slot frei -> kein Record (w=%u h=%u)",
              CROW->crow_shadow_w, CROW->crow_shadow_h);
        CHECK(CROW->crow_pool == 0 && CROW->crow_tint == 0,
              "GIB: Record weg -> weder Lache noch Tint (pool=%d tint=%u)",
              (int)CROW->crow_pool, (unsigned)CROW->crow_tint);
        render_decide(CROW, &s);
        CHECK(s.quad == 0,
              "⛔ REGRESSION: geplatzte Kraehe zeichnet noch ein Quad (halb=%d/%d) — genau der "
              "Nutzer-Report; der alte 500x600-Default hat hier zugeschlagen", s.hx, s.hz);

        /* Bleibt auch ueber viele Ticks weg (der Root-Tail-Schattenhelfer 0x8010026C laeuft
         * nur fuer state != 7, @0x80100234-254). */
        for (int i = 0; i < 200; i++) tick();
        render_decide(CROW, &s);
        CHECK(CROW->crow_shadow_w == 0 && s.quad == 0,
              "GIB: nach 200 Ticks wieder ein Schatten da (w=%u quad=%d)",
              CROW->crow_shadow_w, s.quad);
    }

    /* ---- (2) WANDSPLAT-LEICHE: derselbe Record-Free @0x80103CBC ------------------------- */
    {
        fresh_crow(20000, 0);
        tick();
        corpse_sub(2);                              /* CORPSE Sub 2 = Wandsplat @0x80103C44 */
        CHECK(CROW->crow_shadow_w == 0 && CROW->crow_shadow_h == 0,
              "Wandsplat: `sb zero,14(rec)` @0x80103CBC -> kein Record (w=%u)",
              CROW->crow_shadow_w);
        shadow_t s; render_decide(CROW, &s);
        CHECK(s.quad == 0, "Wandsplat-Leiche zeichnet noch ein Quad (halb=%d)", s.hx);
    }

    /* ---- (3) NORMALE LEICHE: BLUTLACHE 0x00BFBF10, nicht Neutralgrau -------------------- */
    {
        fresh_crow(20000, 0);
        tick();
        corpse_sub(0);                              /* CORPSE Sub 0 @0x8010398C */
        CHECK(CROW->crow_shadow_w >= 400,
              "Normal-Leiche: Lache-Basis 400 (@0x80103A2C-3C), w=%u", CROW->crow_shadow_w);
        CHECK(CROW->crow_pool == 1,
              "Normal-Leiche: 0x80016FE4(rec,0x00BFBF10) @0x80103A20 faerbt den Record zur "
              "LACHE — der Port liess ihn auf dem grauen Alloc-Default 0x00808080 stehen");
        shadow_t s; render_decide(CROW, &s);
        CHECK(s.pool == 1 && s.r == 0x10 && s.g == 0xBF && s.b == 0xBF,
              "Normal-Leiche: Farbe muss 0x00BFBF10 sein (r=%d g=%d b=%d pool=%d)",
              s.r, s.g, s.b, s.pool);
        /* Grower: 60 Ticks a +10 (@0x80103A40-44 / @0x80103AB8-FC) -> 400+600 = 1000 */
        for (int i = 0; i < 120; i++) tick();
        CHECK(CROW->crow_shadow_w == 1000 && CROW->crow_shadow_h == 1000,
              "Normal-Leiche: 400 + 60*10 = 1000 (@0x80103AB8-FC), w=%u h=%u",
              CROW->crow_shadow_w, CROW->crow_shadow_h);
    }

    /* ---- (4) LAUNCH-LEICHE: derselbe Recolor @0x80103F14 -------------------------------- */
    {
        fresh_crow(20000, 0);
        tick();
        corpse_sub(3);                              /* CORPSE Sub 3 @0x80103EB0 */
        CHECK(CROW->crow_pool == 1,
              "Launch-Leiche: 0x80016FE4(rec,0x00BFBF10) @0x80103F14 fehlt");
        CHECK(CROW->crow_shadow_w >= 400,
              "Launch-Leiche: Lache-Basis 400 (@0x80103F1C-38), w=%u", CROW->crow_shadow_w);
        shadow_t s; render_decide(CROW, &s);
        CHECK(s.pool == 1 && s.r == 0x10, "Launch-Leiche: Lachenfarbe fehlt (r=%d)", s.r);
    }

    /* ---- (5) NEGATIV: der RE1.5-Modus bleibt auf SEINER Farbe 0x00FFFF38 --------------- */
    {
        fresh_crow(20000, 0);
        tick();
        corpse_sub(0);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);    /* nur die RENDER-Entscheidung umschalten */
        shadow_t s; render_decide(CROW, &s);
        CHECK(s.pool == 1 && s.r == 0x38 && s.g == 0xff && s.b == 0xff,
              "RE1.5-Kraehe muss weiter 0x00FFFF38 (@0x80115880-c8) bekommen (r=%d g=%d b=%d)",
              s.r, s.g, s.b);
        /* Und: im RE1.5-Modus darf crow_shadow_w == 0 den ALTEN Default behalten — der
         * RE1.5-Wipe schreibt naemlich `sh 1` (@0x80115938-3c), nicht 0. */
        re15_actor_t tmp = *CROW;
        tmp.crow_shadow_w = 0; tmp.crow_shadow_h = 0;
        render_decide(&tmp, &s);
        CHECK(s.quad == 1 && s.hx == 500 && s.hz == 600,
              "RE1.5-Zweig darf NICHT auf 'kein Record' umgestellt werden (quad=%d halb=%d/%d)",
              s.quad, s.hx, s.hz);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    if (fails == 0) printf("test_re2_crow_shadow_release: OK\n");
    else            printf("test_re2_crow_shadow_release: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
