/*
 * RE1.5 Rebuilt — RESIDENT EVIL 2 zombie brain (PORT OPTION, selectable in OPTIONS).
 *
 * Everything here is RE'd from the RE2 (Leon, retail) enemy overlay:
 *     info/re2leon/COMMON/BIN/EMZ0.BIN, loaded RAW @0x80100000
 *     disassemble:  .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMZ0.BIN
 * RE2 state table @0x8010C830 (self-verified):
 *     [0] 0x8010065C INIT   [1] 0x8010114C ACTIVE  [2] 0x80104F40 HURT   [3] 0x80108250 DEATH
 *     [4] 0x80065C88 (EXE)  [5][6] NULL            [7] 0x8010A440 CORPSE (12 substates)
 *     [8] 0x80109CFC        (RE2-EXCLUSIVE, 11 substates — RE1.5's table has no [8])
 *
 * ⚠️ WHAT "RE2 AI" REALLY IS (the overnight RE refuted the folklore, so do not re-add it):
 *   - There is NO line-of-sight / raycast for zombies. A full jal-scan over all 13267 overlay words
 *     finds ZERO calls to 0x80050858 / 0x80065518 / 0x80065890 — those belong to the em/NPC family.
 *   - There is NO crowd/swarm intelligence in the zombie: ACTIVE dispatches ONCE through
 *     (+0x10E & 0x3F) into 0x8010C854, whose 14 entries alternate on bit0 only -> exactly two
 *     variants (0x8010118C upright / 0x80101210 crawling), not eleven "modes".
 *   - The neighbour word +0x1F4 is READ by the zombie (@0x801017A4) but never WRITTEN by it; the
 *     producer is FUN_80065518 in the NPC family -> a dead branch in zombie-only rooms.
 * The real difference is LOCOMOTION, attack arbitration and hit reaction. This file starts with the
 * locomotion gate, which is the one that actually reads as "it stops staggering and comes at you".
 */

#include <stdint.h>
#include <stdlib.h>   /* getenv (headless flavor override) */
#include <stdio.h>    /* RE15_RE2_TRACE */
#include <string.h>   /* memset (Part-Record-Reset) */
#include "re15_actor.h"
#include "re15_math.h"       /* re15_vector_normal — MatrixNormal_0-Zwilling */
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_damage.h"     /* re15_ai_arc_test — the RE1.5 twin of RE2 FUN_80015614 */
#include "re15_enemy_ai.h"   /* re15_ai_set_state_word / live_init / victim FSM / is_grabbed */
#include "re15_enemy.h"      /* re15_enemy_find (RE2 bank from the Welle-A loader) */
#include "re2_ems.h"         /* re2_hybrid_perm — RE2-Part <-> RE1.5-Bone (Import-Modus) */
#include "re15_esp.h"        /* re15_esp_fx_spawn_ex (RE1.5 hit-FX stand-in, documented) */

/* ---- the flavor switch itself ------------------------------------------------------------- */

/* ⚠️ DEFAULT = RE2, Nutzer-Entscheidung 2026-08-22: „Ansonsten ist RE 2 AI mittlerweile so weit
 * in Ordnung, dass ich das gerne als Standard in den Optionen ausgewaehlt haette."
 * Das ist eine BEWUSSTE ABWEICHUNG vom Auslieferungsstand — byte-true waere RE15. Wer den
 * Originalzustand messen will, waehlt ihn im Menue (OPTIONS->AI) oder setzt
 * RE15_AI_FLAVOR=re15/0.
 * ⛔ ALLE byte-true-PINs muessen den Flavor deshalb EXPLIZIT setzen und duerfen sich nicht auf
 * den Default verlassen (etabliertes Muster: re15_ai_flavor_set(RE15_AI_FLAVOR_RE15) am
 * Testanfang, siehe test_room1140_combat.c). */
static re15_ai_flavor_t s_flavor = RE15_AI_FLAVOR_RE2;
static int s_flavor_env_read = 0;

re15_ai_flavor_t re15_ai_flavor(void)
{
    /* RE15_AI_FLAVOR waehlt das Brain ohne Menue — noetig fuer Harness UND fuer die
     * byte-true-Messung, seit der Default RE2 ist. `re15`/`0`/`1.5` erzwingt RE1.5.
     *
     * ---- BELEGUNG NACH DER MIXED-ERWEITERUNG (2026-08-23) ------------------------------------
     * ⚠️ DIE `2` BLEIBT RE2. Der Auftrag schlug vor, MIXED zusaetzlich als "2" zu verstehen; das
     * kollidiert und ist deshalb ABGELEHNT: `RE15_AI_FLAVOR=2` ist die etablierte RE2-Schreibweise
     * (dokumentiert u.a. in enemy_ai_re2_spider.c:2846, test_1090_fire_pin.c:536,
     * test_re2_room1140_ab.c:7, test_re2_room1190_ab.c:7). Waere "2" ploetzlich MIXED, wuerde
     * JEDER dieser Laeufe still von RE2 auf MIXED kippen und faelschlich als RE2 verbucht.
     * Die Zahlen sind hier ohnehin SPIEL-Nummern, keine Enum-Ordinalzahlen ("1.5" -> RE1.5).
     * MIXED hat keine Spielnummer und bekommt deshalb NUR Wortformen:
     *     "0" | "1" | "1.5" | "re15"        -> RE15_AI_FLAVOR_RE15
     *     "2" | "r…" | "re2"                -> RE15_AI_FLAVOR_RE2
     *     "m" | "M" | "mix" | "mixed"       -> RE15_AI_FLAVOR_MIXED
     * (Die ORDINALZAHL 0/1/2 gibt es weiterhin — aber in RE15_CONFIG_AI, dem Menue-Index-Env,
     * s. platform/pc/main.c pc_ai_mode_set.) */
    if (!s_flavor_env_read) {
        const char *v = getenv("RE15_AI_FLAVOR");
        s_flavor_env_read = 1;
        if (v && *v) {
            if (v[0] == '2' || v[0] == 'r' || v[0] == 'R') s_flavor = RE15_AI_FLAVOR_RE2;
            if (v[0] == '0' || v[0] == '1') s_flavor = RE15_AI_FLAVOR_RE15;   /* "0", "1.5" */
            if ((v[0] == 'r' || v[0] == 'R') && (v[1] == 'e' || v[1] == 'E')
                && v[2] == '1') s_flavor = RE15_AI_FLAVOR_RE15;               /* "re15" */
            if (v[0] == 'm' || v[0] == 'M') s_flavor = RE15_AI_FLAVOR_MIXED;  /* "m", "mixed" */
        }
    }
    return s_flavor;
}
void re15_ai_flavor_set(re15_ai_flavor_t f)
{
    /* KLEMME: jeder Fremdwert (alter persistierter Stand, atoi eines Env, kuenftige vierte Stufe)
     * faellt auf RE1.5 — der byte-true Zustand, nicht auf einen Port-Modus. Die drei gueltigen
     * Werte gehen 1:1 durch. */
    switch (f) {
        case RE15_AI_FLAVOR_RE2:   s_flavor = RE15_AI_FLAVOR_RE2;   break;
        case RE15_AI_FLAVOR_MIXED: s_flavor = RE15_AI_FLAVOR_MIXED; break;
        default:                   s_flavor = RE15_AI_FLAVOR_RE15;  break;
    }
}

/* ---- die TYP-BEZOGENE Aufloesung (vollstaendige Begruendung in re15_ai_flavor.h) ------------
 * Ersetzt jeden frueheren globalen `re15_ai_flavor() == RE15_AI_FLAVOR_RE2`-Vergleich.
 * MIXED = "alles RE1.5 ausser dem Hund" -> genau Kind 0x20 (Cerberus, enemy_ai_re2_dog.c,
 * EMD0G_MOD0.BIN Root @0x80100004). Bewusst NICHT re15_re2_owns_type gefiltert: diese Funktion
 * beantwortet "WELCHES SPIEL treibt diesen Typ", die Besitzfrage ("hat der Port dafuer ueberhaupt
 * ein RE2-Brain/eine RE2-Zeile") bleibt an den vorhandenen owns-Toren daneben. */
int re15_ai_re2_for_type(unsigned type)
{
    switch (re15_ai_flavor()) {
        case RE15_AI_FLAVOR_RE2:   return 1;
        case RE15_AI_FLAVOR_MIXED: return type == 0x20u;
        default:                   return 0;
    }
}

/* WELLE G — Modellherkunft (orthogonal zum Brain, siehe re15_ai_flavor.h). Default RE2, damit
 * der bisherige RE2-Modus UND jedes Harness mit `RE15_AI_FLAVOR=re2` byte-identisch bleiben;
 * `RE15_AI_MODELS=re15` (oder 15/0) waehlt den Hybrid headless. */
static re15_ai_models_t s_models = RE15_AI_MODELS_RE2;
static int s_models_env_read = 0;

re15_ai_models_t re15_ai_models(void)
{
    if (!s_models_env_read) {
        const char *v = getenv("RE15_AI_MODELS");
        s_models_env_read = 1;
        if (v && (strcmp(v, "re15") == 0 || strcmp(v, "RE15") == 0 ||
                  strcmp(v, "15")   == 0 || strcmp(v, "0")    == 0))
            s_models = RE15_AI_MODELS_RE15;
    }
    return s_models;
}
void re15_ai_models_set(re15_ai_models_t m) { s_models = (m == RE15_AI_MODELS_RE15)
                                                          ? RE15_AI_MODELS_RE15
                                                          : RE15_AI_MODELS_RE2;
                                              s_models_env_read = 1; }

/* The RE1.5 zombie family. RE2 folds its whole 0x10..0x1F kind range onto one group (@0x8001B738),
 * so this is the port's equivalent set — and ONLY these ever leave the RE1.5 brain. */
int re15_re2z_owns_type(unsigned type)
{
    return type == 0x10 || type == 0x11 || type == 0x12
        || type == 0x13 || type == 0x16 || type == 0x18;
}

/* ============================================================================================
 * ⛔ PORT-OPTION "RE2-ZOMBIE-UEBERNAHME IM RE1.5-MODUS"  (KEIN byte-true Fix — AUSDRUECKLICHER
 *    NUTZER-AUFTRAG 2026-08-20, woertlich:
 *      "Bei RE2 AI und RE2 AI + Model ist es so, dass auch mit der Handfeuerwaffe Gliedmassen
 *       abgeschossen werden koennen. Bei RE1.5 AI noch nicht. Und genau das, sowie die
 *       Schadenswerte fuer Zombies, moechte ich auch in RE1.5 AI haben."
 *    Der Nutzer WEISS, dass RE1.5 das im Original nicht hat, und will die Abweichung. Das hier
 *    ist deshalb bewusst NICHT als byte-true verkauft: es ist ein SCHALTER, der zwei
 *    vollstaendig belegte RE2-Systeme (Zerleger + Schadens-/HP-Modell) in den RE1.5-Modus
 *    hineinzieht. Alles, was UNTER dem Schalter laeuft, bleibt byte-true zu RE2 (jede Konstante
 *    traegt weiter ihr @0x…); nur die AUSWAHL ist eine Port-Entscheidung.
 *
 * GILT AUSSCHLIESSLICH FUER DIE ZOMBIE-FAMILIE (re15_re2z_owns_type: 0x10/0x11/0x12/0x13/
 * 0x16/0x18). Hund 0x20, Kraehe 0x21 und Spinne 0x25/0x26 bleiben im RE1.5-Modus unangetastet —
 * dafuer gibt es eine eigene Regressionswache (tests/unit/test_re15_re2z_import.c).
 *
 * DEFAULT = AN (das ist der Zustand, den der Nutzer haben will). Der EINZIGE Hebel zurueck auf
 * den byte-true RE1.5-Auslieferungsstand ist RE15_RE15_RE2Z_IMPORT=0 bzw. der Setter — und
 * genau ueber diesen Hebel pruefen die byte-true-PINs weiterhin die ORIGINALWERTE.
 * ========================================================================================== */
static int s_re15_import = 1;
static int s_re15_import_env_read = 0;

int re15_re15_re2z_import(void)
{
    if (!s_re15_import_env_read) {
        const char *v = getenv("RE15_RE15_RE2Z_IMPORT");
        s_re15_import_env_read = 1;
        if (v && (v[0] == '0' || v[0] == 'n' || v[0] == 'N')) s_re15_import = 0;
    }
    return s_re15_import;
}

void re15_re15_re2z_import_set(int on)
{
    s_re15_import = on ? 1 : 0;
    s_re15_import_env_read = 1;     /* explizit gesetzt schlaegt die Umgebungsvariable */
}

int re15_re15_re2z_import_owns(unsigned type)
{
    /* ⚠️ MIXED (2026-08-23): das Tor hiess frueher `flavor == RE15`. Der MIXED-Modus soll fuer
     * ALLES ausser dem Hund exakt das RE1.5-Verhalten liefern — und dazu gehoert diese
     * Port-Option (Default AN), die der Zombie-Familie im RE1.5-Modus RE2-Zerleger + RE2-Schaden
     * gibt. Der Test ist deshalb "dieser Typ laeuft NICHT auf einem RE2-Brain": fuer die
     * Zombie-Familie ist das in RE15 UND in MIXED wahr, in RE2 falsch — RE15 und RE2 bleiben
     * damit Bit fuer Bit wie vorher, MIXED erbt die RE1.5-Seite. */
    return !re15_ai_re2_for_type(type)
        && re15_re15_re2z_import()
        && re15_re2z_owns_type(type);
}

/* ---- W1: the RE2 WALK TURN GATE ------------------------------------------------------------
 *
 * Byte-true from the walk substate (self-disassembled 2026-07-29):
 *   80101bac: lw    v0,496(s1)        ; +0x1F0 = distance to the player (the RE2 EXE fills this in
 *                                     ;          before the dispatch, @0x800265A4-E0)
 *   80101bb4: sltiu v0,v0,0x1389      ; dist < 5001 ?
 *   80101bb8: bne   v0,zero,0x80101bec
 *   80101bbc: addiu v1,zero,8         ;   YES -> rate = +8, MONOTONE toward the steer point
 *   80101bc0: lb    v0,363(s1)        ;   NO  -> gait row +0x16B
 *   80101bd4: lhu   v0,-14044(at)     ;          tbl16[row] @0x8010C924 (32 u16, self-dumped)
 *   80101bdc: srl   v0,v0,15          ;          bit15 of the row
 *   80101be0: sll   v0,v0,4           ;          *16
 *   80101be8: subu  v1,v1,v0          ;          rate = 8 - (bit15 ? 16 : 0)  ->  +8 or -8  = WEAVE
 *   80101bf8: lh    a1,452(s1)        ; a1 = steer point X (+0x1C4)
 *   80101bfc: lh    a2,454(s1)        ; a2 = steer point Z (+0x1C6)
 *   80101c00: jal   0x80015558        ; turn toward it at rate v1
 * and a SECOND, additive turn when close:
 *   80101c94: sltiu v0,v0,0xbb8       ; dist < 3000 ?
 *   80101ca8: jal   0x80015558
 *   80101cac: addiu a3,zero,16        ;   -> an EXTRA rate-16 turn toward the same point
 *
 * So: far away the RE2 zombie WEAVES (+8/-8 flipping with the gait row's bit15), but inside 5001 it
 * turns monotonically onto you, and inside 3000 it turns at 8+16 per frame. RE1.5 instead flips the
 * sign at every gait segment boundary regardless of distance — which is exactly the "drunk" walk.
 *
 * ⚠️ DEFERRED, NOT byte-true yet: the steer point (+0x1C4/+0x1C6) is produced by the RE2 navigator
 * FUN_8004A808, called from the root BEFORE the dispatch (@0x80100354). That navigator is not ported,
 * so this uses the PLAYER position as the steer target. For open-floor pursuit the two coincide; they
 * differ around corners/obstacles. Marked here so nobody mistakes it for a finished port. */
#define RE2Z_DIST_LOCKON   0x1389u   /* 5001 @0x80101bb4 */
#define RE2Z_DIST_CLOSE    0x0bb8u   /* 3000 @0x80101c94 */
#define RE2Z_TURN_BASE     8         /*      @0x80101bbc */
#define RE2Z_TURN_EXTRA    16        /*      @0x80101cac */

/* Gait table @0x8010C924 — 32 u16, byte-verified against EMZ0.BIN. EVERY entry packs BOTH:
 *     bits 0..14 = the segment DURATION in frames   (andi 0x7fff @0x80101B84)
 *     bit  15    = the TURN-AWAY flag               (srl 15     @0x80101BDC)
 * so the sequence reads 190/50, 150/64, 110/64, 180/52, ... = a long segment turning TOWARD the
 * steer point alternating with a short one turning AWAY. That alternation IS the RE2 walk. */
const uint16_t re15_re2z_gait_tbl[32] = {
      190, 32818,   150, 32832,   110, 32832,   180, 32820,
      140, 32820,   120, 32826,   150, 32846,   140, 32818,
      150, 32830,   150, 32840,   110, 32838,   120, 32842,
      180, 32802,   110, 32820,   200, 32834,   180, 32834,
};

/* The turn helper itself is ALREADY in the port, byte-true, and is literally the same engine
 * function in both games — verified by disassembling both and diffing:
 *     RE1.5  FUN_8001aac4 (a0=tx a1=tz a2=slew, operates on the global current entity,
 *                          pos +0x34/+0x3c, yaw +0x6a, bearing via FUN_8001a6d4)
 *     RE2    FUN_80015558 (a0=actor* a1=tx a2=tz a3=rate,
 *                          pos +0x38/+0x40, yaw +0x76, bearing via FUN_800154ac)
 * Different signature and struct offsets, IDENTICAL math after the bearing:
 *     bgez rate      -> rate<0: rate=-rate AND bearing+=0x800 (steer AWAY, not "clamp the other way")
 *     delta = (rate + bearing - yaw) & 0xfff
 *     delta < 2*rate -> SNAP yaw = bearing            (@0x800155cc / RE1.5 same slt)
 *     delta < 0x801  -> yaw += rate  else  yaw -= rate (@0x800155d4 sltiu 0x801)
 * The bearing helpers are byte-identical too: same 0x400/0xc00 base, same catan((dz<<12)/dx), and
 * RE2's `(4096-at)&0xfff` == RE1.5's `(0-at)&0xfff`. So the RE2 walk turn reuses the port's
 * re15_enemy_steer_point verbatim — no second implementation, nothing re-derived by hand. */
extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);

/* ---- the RE2 PRNG (@0x80015FE8) ------------------------------------------------------------
 *   80015ff0: lw   v0,0(a0)      ; a0 = 0x800CE318, the 16-bit state
 *   80015ff8: srl  v1,v0,7
 *   80015ffc: andi v1,v1,0xff    ; h = (s >> 7) & 0xff
 *   80016000: addu v0,v1,v0
 *   80016004: andi v0,v0,0xff    ; lo = (h + s) & 0xff
 *   80016008: sll  v1,v1,8
 *   8001600c: or   v0,v0,v1      ; v = lo | (h << 8)
 *   80016014: andi v0,v0,0xff    ; RETURN v & 0xff
 *   80016018: sw   v1,0(a0)      ; STATE  v & 0xffff
 * Seeded to 0xD2706CA4 (lui 0xd270 / ori 0x6ca4) at both writers of 0x800CE318: @0x8002B908-1C and
 * @0x8003BCB0-C4. Seeding matters — from state 0 this generator is a fixed point (0 -> 0 forever).
 * Kept separate from the port's RE1.5 RNG on purpose: the RE1.5 generator hashes the CALLER's
 * argument (its state store @0x800AC774 is a dead store), this one feeds back on itself. */
static uint32_t s_re2_rng = 0xD2706CA4u;
static uint32_t s_re2_rng_draws = 0;   /* Port-Diagnose: Wurf-Zaehler (nur Tests lesen ihn) */
static uint32_t re2z_rand(void)
{
    uint32_t s = s_re2_rng;
    uint32_t h = (s >> 7) & 0xffu;
    uint32_t v = ((h + s) & 0xffu) | (h << 8);
    s_re2_rng  = v & 0xffffu;
    s_re2_rng_draws++;
    return v & 0xffu;
}
/* Die WURFZAHL ist Verhalten (jeder `jal 0x80015FE8` zaehlt); die Gore-Tests pinnen sie
 * differenziell (gleicher Pfad mit/ohne Zerleger-Zeile). */
uint32_t re15_re2_rand_draws(void) { return s_re2_rng_draws; }
/* WELLE C: the dog draws from the SAME generator (RE2 has ONE state word 0x800CE318 for every
 * overlay — 69 jal 0x80015FE8 in the dog module alone). Exported for enemy_ai_re2_dog.c. */
uint32_t re15_re2_rand(void) { return re2z_rand(); }
void re15_re2z_hit_filter_apply(int slot);        /* Vier-Gate-Filter FUN_800470C0, s. unten */
void re15_re2z_onesave_reset(void);                           /* Welle B (below): FUN_800401d4 latch */
void re15_re2z_rng_reset(void) { s_re2_rng = 0xD2706CA4u;     /* room load — keeps runs deterministic */
                                 re15_re2z_onesave_reset();
                                 re15_re2dog_room_reset(); }  /* WELLE C: 0x800CFBF4-Analog (der
                                                               * einzige EXE-Clear FUN_80052f3c ist
                                                               * ein Room-Init) */

/* ---- the GAIT MACHINE (@0x80101A7C-AC init, @0x80101B2C-90 per tick) ------------------------
 * INIT (walk entered):
 *   80101a90: andi v0,v0,0xf     ; row = (rand & 0xf)
 *   80101a94: sll  v0,v0,1       ;     * 2  -> the start row is ALWAYS EVEN
 *   80101a9c: sb   v0,363(s1)    ; +0x16B
 *   80101ac0: addu v1,v1,v0      ; timer = (tbl[row] & 0x7fff) + rand   <-- FULL byte here
 *   80101ac8: sh   v1,344(s1)    ; +0x158
 * PER TICK:
 *   80101b2c: addiu v0,v1,-1
 *   80101b30: bne  v1,zero,turn  ; timer != 0 -> just decrement and go turn
 *   80101b34: sh   v0,344(s1)
 *   80101b40: addiu v0,v0,1      ; else row += 1
 *   80101b50: slti v0,v0,32      ;      wrap at 32
 *   80101b5c: sb   zero,363(s1)
 *   80101b80: andi v0,v0,0x1f    ; timer = (tbl[row] & 0x7fff) + (rand & 0x1f)  <-- MASKED here
 *   80101b90: sh   v1,344(s1)
 * The init/expiry asymmetry (full byte vs & 0x1f) is in the bytes, not a typo on my part.
 *
 * NOT ported (read, but their consumers are unidentified — flagged rather than invented):
 *   +0x16A = (rand & 0x1f) + 30   @0x80101AD4-E8
 *   +0x14D = (rand & 0x1f)        @0x80101B04-0C
 */
void re15_re2z_gait_init(re15_actor_t *e)
{
    if (!e) return;
    uint32_t r1 = re2z_rand();
    e->re2z_gaitrow = (uint8_t)((r1 & 0x0fu) << 1);                      /* @0x80101a90/94/9c */
    uint32_t r2 = re2z_rand();
    e->re2z_gaittmr = (uint16_t)((re15_re2z_gait_tbl[e->re2z_gaitrow] & 0x7fffu) + r2);
    e->re2z_gaitinit = (uint8_t)(0x80u | (e->sub_state_1 & 0x7fu));
}

static void re2z_gait_tick(re15_actor_t *e)
{
    /* The original runs the init block on the tick the WALK SUBSTATE IS ENTERED (it sits in the
     * substate prologue @0x80101A7C, ahead of the per-tick timer code at 0x80101B2C). The port
     * detects that same entry by the substate byte changing since the last gait tick — so a zombie
     * that gets knocked down and walks again re-rolls its gait exactly like the original, instead
     * of resuming a stale row. `re2z_gaitinit` therefore stores 0x80 | sub_state_1, never a bool. */
    uint8_t tag = (uint8_t)(0x80u | (e->sub_state_1 & 0x7fu));
    if (e->re2z_gaitinit != tag) { re15_re2z_gait_init(e); e->re2z_gaitinit = tag; return; }
    if (e->re2z_gaittmr != 0) { e->re2z_gaittmr--; return; }             /* @0x80101b2c-34 */
    unsigned row = (unsigned)e->re2z_gaitrow + 1u;                       /* @0x80101b40 */
    if (row >= 32u) row = 0u;                                            /* @0x80101b50/5c */
    e->re2z_gaitrow = (uint8_t)row;
    uint32_t r = re2z_rand() & 0x1fu;                                    /* @0x80101b80 */
    e->re2z_gaittmr = (uint16_t)((re15_re2z_gait_tbl[row] & 0x7fffu) + r);
}

/* Apply the RE2 walk turn for one tick. `dist` = distance to the player (the port's ai_dist, which is
 * the same quantity RE2 keeps in +0x1F0). Returns 1 if it handled the turn. */
int re15_re2z_walk_turn(re15_actor_t *e, int32_t px, int32_t pz, uint32_t dist)
{
    if (!e) return 0;
    re2z_gait_tick(e);
    int rate;
    if (dist < RE2Z_DIST_LOCKON) {
        rate = RE2Z_TURN_BASE;                                   /* @0x80101bec lock-on */
    } else {
        int row   = e->re2z_gaitrow & 31;                        /* +0x16B, bounded by slti 32 */
        int bit15 = (re15_re2z_gait_tbl[row] >> 15) & 1;         /* @0x80101bdc/e0 */
        rate = RE2Z_TURN_BASE - (bit15 ? 16 : 0);                /* @0x80101be8 -> +8 or -8 */
    }
    re15_enemy_steer_point(e, px, pz, rate);                     /* @0x80101c00 jal 0x80015558 */
    if (dist < RE2Z_DIST_CLOSE)
        re15_enemy_steer_point(e, px, pz, RE2Z_TURN_EXTRA);      /* @0x80101ca8/cac, a3 = 16 */
    return 1;
}

/* FUN_80015758(a0=ptrA, a1=ptrB, a2=ang, a3=half) — the SECTOR test block G uses twice.
 * Self-disassembled 2026-07-29:
 *   80015778: lh a0,0(v0)    ; A.x  (the callers pass self+0x38 / PL+0x38, so X is at +0, Z at +8)
 *   8001577c: lh a1,8(v0)    ; A.z
 *   80015780: lh a2,0(v1)    ; B.x
 *   80015784: lh a3,8(v1)    ; B.z
 *   80015788: jal 0x800154ac ; bearing(A -> B)
 *   8001579c: subu v0,v0,s1  ; bearing - ang
 *   800157a8: addu v0,v0,s0  ;         + half
 *   800157ac: andi v0,v0,0xfff
 *   800157b0: sll  s0,s0,1   ; 2*half
 *   800157b4: sltu v0,v0,s0
 *   800157b8: xori v0,v0,0x1 ; RETURN !(t < 2*half)  ->  0 == INSIDE the sector
 * Same family as FUN_80015614/FUN_8001A9CC, but the cone is centred on an ARBITRARY angle instead
 * of the actor's own yaw — which is exactly why block G can test two half-sectors either side. */
static int re2z_sector(const re15_actor_t *a, const re15_actor_t *b, int ang, int half)
{
    /* the port's mesh-yaw convention carries a +0x400 offset (re15_damage.c re15_ai_arc_test) */
    int bearing = ((int)re15_atan2_q12(b->z - a->z, b->x - a->x) - 0x400) & 0xfff;
    unsigned t  = (unsigned)((bearing - ang + half) & 0xfff);
    return !(t < (unsigned)(half << 1));          /* 0 == inside */
}

/* Fill the ladder's gate struct from PORT state. Every field is one of:
 *   MAPPED   — a port quantity that provably is the same thing
 *   ZERO     — provably zero for a zombie the port actually runs (cited)
 *   OPEN     — no proven producer in the port yet; left 0, and the block it gates cannot fire.
 * WELLE B closed +0x23E (re2z_cd23e), +0x21A (re2z_flags21a) and +0x110 (contact). */
void re15_re2z_fill_gates(const re15_actor_t *e, const re15_actor_t *pl,
                          int player_claimed, re15_re2z_gates_t *g)
{
    for (unsigned i = 0; i < sizeof(*g); i++) ((unsigned char *)g)[i] = 0;
    if (!e || !pl) return;

    /* MAPPED ------------------------------------------------------------------------------- */
    g->dist    = e->ai_dist;                                    /* +0x1F0        @0x80101744 */
    g->arc1024 = re15_ai_arc_test(e, pl->x, pl->z, 1024);       /* a3 = 1024     @0x8010174c */
    g->arc512  = re15_ai_arc_test(e, pl->x, pl->z,  512);       /* a3 = 512      @0x80101754 */
    /* the third arc (half 1300, jal @0x80101788) is NOT computed: its v0 is destroyed by
     * `lbu v0,574(s0)` @0x80101790 before any reader, and the whole call chain
     * FUN_80015614 -> FUN_800154AC -> catan is store-free, so omitting it is observable nowhere. */
    g->self_106 = e->floor;                                     /* +0x106        @0x80101808 */
    g->pl_106   = pl->floor;                                    /*               @0x8010180c */
    /* WELLE B producers (closed by the Lane-Z RE + own re-disasm 2026-08-10):
     *  +0x23E = the snap-bite cooldown: ONLY writer sb 60 @0x80104E2C (EXEC[14] end), root
     *           decrement @0x80100470-80 — both now live in the port (re2z_cd23e).
     *  +0x21A = the flag word: INIT clear @0x8010087C; side latches 0x20/0x40 from grab-escape
     *           P5 (@0x80102A34-40); 0x4 lying orientation; 0x10 crawl marker (@0x8010358C);
     *           0x4000 kill-counted (@0x80108294) — re2z_flags21a.
     *  +0x110 bit 0 = the collision-query result (`sw v0,272(s0)` @0x800356D8, query
     *           FUN_8004C1BC, frame stamp +0x114 @0x800356E8). Der RE2-Zombie-Root ruft den
     *           Wrapper im Tail @0x80100638 (`jal 0x8003567c`, EMOVL10_S0.BIN) auf.
     *           PORT MAPPING (korrigiert): e->sca_wall_hit = der RUECKGABEWERT der SCA-Klemme
     *           (RE1.5 FUN_8003b0a4, `ori s7,s7,0x1` @0x8003b500 / return @0x8003b520) — dieselbe
     *           Semantik wie RE2s Bit 0 (@0x8004c4a4/@0x8004c518: "solide Zelle beruehrt"), und
     *           der run_all-Klemm-Tail schreibt ihn NACH dem AI-Tick, also dieselbe Phasenlage
     *           wie RE2s Frame-Stempel +0x114.
     *           ⛔ WAR FALSCH: hier standen die BODY-PUSH-Kontaktbits +0x1C2 (contact_flags) —
     *           das ist die Aktor-gegen-Aktor-Trennung (FUN_8002aec4/b544), NICHT die WAND.
     *           Block C bleibt so oder so inert, weil +0x1D4 in RE1.5-Raeumen nie 0xC000 traegt
     *           (siehe unten). */
    g->self_23e = e->re2z_cd23e;                                /* +0x23E        @0x80101790 */
    g->self_21a = e->re2z_flags21a;                             /* +0x21A        @0x80101928 */
    g->self_110 = (e->sca_wall_hit != 0) ? 1u : 0u;              /* +0x110 bit0   @0x80101844 */
    /* PL+0x1D3 bit 0x80 = the "this actor is claimed" latch. Mechanism fully proven: the zombie
     * SETS it in nine places (e.g. @0x80101968 / @0x801019B0) and never clears it; the CLEAR lives
     * on the player side (`andi 0x7f` + sb, @0x8003E844 / @0x800630E0 and the overlay grab-aborts
     * @0x80104FAC HURT / @0x801082F4 DEATH). The port's equivalent latch is s_player_grabbed.
     * The LOW SEVEN BITS are a player-side countdown (~40 EXE writers, decrement @0x8003BFF4-C008)
     * with no port producer — left 0 (OPEN): B/J stay gated by their OTHER conditions, which in
     * RE1.5 rooms keep them silent (B needs floors to DIFFER, J needs PL+0x8 == 15). */
    g->pl_156  = pl->hp;                                        /* +0x156        @0x801019e8 */
    g->pl_1d3  = player_claimed ? 0x80u : 0u;                   /*               @0x80101908 */
    g->g1_sector_hit = (re2z_sector(e, pl, ((int)e->rot_y + 256) & 0xfff, 256) == 0); /* @0x80101948 */
    g->g2_sector_hit = (re2z_sector(e, pl, ((int)e->rot_y - 256) & 0xfff, 256) == 0); /* @0x8010198c */

    /* ---- 0x800CFBF6: the global that gates blocks D and E ------------------------------------
     * RESOLVED (self-RE'd 2026-07-29). It is a PER-FRAME "what is the player doing right now"
     * bitfield, not a persistent flag:
     *   - It has exactly FOUR writers in the whole RE2 EXE. @0x8003BFF0 CLEARS bits 0..4
     *     (`andi 0xffe0`) — gated on 0x800CFBDC >= 0 @0x8003BFC0, so "cleared every frame" is
     *     wrong. The other three only ever OR a bit in.
     *   - Bits 0x1 and 0x10 are NEVER set anywhere, so mask 0x15 (block D) reduces to bit 0x4 and
     *     mask 0x17 (block E) to (0x2 | 0x4).
     *   - The three setters are player SUB-STATE handlers, reached through the player's action
     *     table: dispatcher @0x8003C5D4 indexes base 0x800A4084 by player+0x5, so
     *         sub 1 = 0x8003CBDC -> `ori 0x2` @0x8003CC80
     *         sub 2 = 0x8003D0E8 -> `ori 0x4` @0x8003D18C
     *         sub 3 = 0x8003D5F4 -> `ori 0x2` @0x8003D6B4
     *   - Which sub-state is which comes from the pad-driven selector @0x8003C650-C6C8:
     *         virtual bit 0x1   (forward)  -> sub 1   word 0x101 @0x801017.. see @0x8003C6B0/B4
     *         virtual bit 0x200 (run/cross)-> sub 2   word 0x201 @0x8003C6C0/C4
     *         virtual bit 0x4   (backward) -> sub 3   word 0x301 @0x8003C6A0/A4
     *         virtual bits 0xa  (turn L/R) -> sub 4   word 0x401 @0x8003C690/94  (sets NOTHING)
     *     Bit meanings from the port's own virtual-pad table (pad_common.c:27-35, RE1.5
     *     @0x80073dbc): bit0<-UP, bit2<-DOWN, bits1|3<-RIGHT|LEFT, bit9<-CROSS.
     * => bit 0x2 means "the player is WALKING" (forward or backward) and bit 0x4 means "the player
     *    is RUNNING". So block D (running only, from 3500, 25%) and block E (any movement, from
     *    2500, 50%) are movement-reactive: standing still, neither can fire. That is the RE2
     *    behaviour the folklore mistook for eyesight.
     *
     * ⚠️ PORT MAPPING, not a byte-true port of the field: the RE2 producer is RE2's player state
     * machine, which the port does not implement. The port's equivalent movement state is the
     * player motion sentinel (player_common.c:65-67: RUN = 100, WALK = 105, BACK = WALK + reverse),
     * recomputed every tick exactly like the original's per-frame bits. */
    if      (pl->motion == 100) g->global_cfbf6 = 0x04u;        /* RUN  -> @0x8003D18C */
    else if (pl->motion == 105) g->global_cfbf6 = 0x02u;        /* WALK -> @0x8003CC80 / @0x8003D6B4 */
    else                        g->global_cfbf6 = 0x00u;        /* idle/turn set NOTHING */

    /* ZERO / dead branches, with the proof ------------------------------------------------------
     * self+0x1F4/+0x1F8 (block A): ZERO writers anywhere in EMZ0.BIN — the producer is
     *   FUN_80065518 in the em/NPC family, which only ticks entity types 64..91. Zombies are
     *   clamped to 16..31 by the overlay loader (`addiu v0,t1,-16; sltiu v0,v0,0x10 -> t1=16`
     *   @0x8001B738-48), so block A cannot fire in a zombie-only room. DEAD, documented inert.
     * self+0x1D4 (block C): no writer in the zombie overlay; all EXE writers are script-/spawn-
     *   parameter driven (`lhu v0,2(a1)` @0x800570F4, setter @0x80055D90, switch @0x80056C60-D74,
     *   clears @0x800573E8/@0x80057A7C). RE1.5 room data never carries RE2's 0xC000 bits ->
     *   block C (EXEC[10] "hammering", reads +0x1D4 @0x80104220) stays INERT, documented.
     * pl_156 == -32768 (block K): the only -32768 -> +0x156 store in the game (@0x8010B730/38)
     *   writes an ENEMY's HP; how the PLAYER HP ever becomes -32768 is OPEN (FUN_800401d4 clamps
     *   death at "HP<-14"). The port's player HP never is -32768 -> K stays INERT, documented.
     * PL+0x8 (the G/J fork): player routine id 15 — semantics OPEN; 0 keeps the G branch. */
}

/* Run the RE2 walk decision for this tick and report the state word it commits, if any. */
int re15_re2z_walk_decide(const re15_actor_t *e, const re15_actor_t *pl,
                          int player_claimed, re15_re2z_decision_t *out)
{
    re15_re2z_gates_t g;
    re15_re2z_fill_gates(e, pl, player_claimed, &g);
    return re15_re2z_decide_walk(&g, out);
}

/* ============================================================================================
 * W2 — THE ATTACK-DECISION LADDER, DECISION[1] = 0x80101714
 *
 * CALL CONTEXT @0x801011A8-EC: 0x8010118C does `lbu +5` -> DECISION[0x8010C88C] -> `lbu +5` AGAIN
 * -> EXECUTOR[0x8010C8CC]. A store here therefore runs its executor THE SAME TICK.
 *
 * "LAST WRITER WINS" — self-verified, not assumed: in 0x80101714..0x80101A34 there are EXACTLY 9
 * `sw ...,4(s0)` (0x801017E4, 82C, 858, 8A0, 8E4, 958, 99C, 9E4, A10) and EXACTLY 2 jumps —
 * `j 0x80101a1c` @0x801017E0 (block A into the epilogue, the ONLY early exit) and `j 0x801019e8`
 * @0x801019AC (G skips J). No instruction in the function READS +0x4, so sequential C is exact.
 * If no block fires, +0x4 is left untouched — hence `wrote`.
 *
 * RNG DRAW COUNT IS ITSELF BEHAVIOUR: 0..2 draws per tick (D @0x80101888, E @0x801018D0), and only
 * after their first three gates pass. Right order + wrong draw count = the whole sequence desyncs.
 *
 * Word -> bytes (LE): 0x0E01 -> sub 14; 0x0A01 -> 10; 0x0C01 -> 12; 0x0301 -> 3;
 * 0x00060801 -> sub 8 with phase +0x6 = 6. Every `sw` ZEROES +0x6/+0x7 — load-bearing, because the
 * executors dispatch on +0x6.
 * ============================================================================================ */
int re15_re2z_decide_walk(const re15_re2z_gates_t *g, re15_re2z_decision_t *out)
{
    if (!g || !out) return 0;
    out->wrote = 0; out->word = 0; out->early_out = 0;
    out->claim_player = 0; out->set_10e_4000 = 0; out->rng_draws = 0;

    if (g->self_23e == 0) {                                   /* lbu 574(s0) @0x80101790, bne @0x8010179c */
        /* --- A: the ONLY early return. j 0x80101a1c @0x801017E0, store in its delay slot ------ */
        if ((g->self_1f4 & 0xC0000000u)                       /* @0x801017a0/ac/b0                 */
            && (g->self_1f4 & 0x3FFFFFFFu) < 2000u            /* sltiu 0x7d0 @0x801017c0           */
            && g->a_sector_hit) {                             /* FUN_80015714(...)==0 @0x801017d0  */
            out->wrote = 1; out->word = 0x00000E01u; out->early_out = 1;
            return 1;                                         /* sw @0x801017e4 (delay slot)       */
        }
        /* --- B: same word, but NO return — falls through into C ------------------------------ */
        if (g->arc512 == 0                                    /* sll/bne @0x801017f4/f8            */
            && g->dist < 2000u                                /* sltiu @0x801017fc (UNSIGNED)      */
            && g->self_106 != g->pl_106                       /* beq-away @0x80101814              */
            && g->pl_1d3 == 0) {                              /* bne @0x80101824 (the WHOLE byte)  */
            out->wrote = 1; out->word = 0x00000E01u;          /* sw @0x8010182c                    */
        }
    }
    /* --- C ------------------------------------------------------------------------------------ */
    if ((g->self_1d4 & (int16_t)0xC000) != 0                  /* lh @0x80101830 / andi @0x80101838 */
        && (g->self_110 & 1u) != 0) {                         /* lw @0x80101844 / andi @0x8010184c */
        out->wrote = 1; out->word = 0x00000A01u;              /* sw @0x80101858                    */
    }
    /* --- D ------------------------------------------------------------------------------------ */
    if (g->dist < 3500u                                       /* sltiu 0xdac @0x8010185c           */
        && g->arc1024 != 0                                    /* beq-away @0x80101868 -> OUTSIDE   */
        && (g->global_cfbf6 & 0x15u) != 0) {                  /* lhu @0x80101874 / andi @0x8010187c */
        out->rng_draws++;                                     /* jal 0x80015FE8 @0x80101888        */
        if ((re2z_rand() & 3u) == 0u) {                       /* andi @0x80101890 / bne @0x80101894 */
            out->wrote = 1; out->word = 0x00000C01u;          /* sw @0x801018a0                    */
        }
    }
    /* --- E: SEQUENTIAL with D, not a tier. @0x80101860 sends a FAILED D straight to E's own test
     *        @0x801018a4, so below 2500 BOTH run and BOTH draw. ------------------------------- */
    if (g->dist < 2500u                                       /* sltiu 0x9c4 @0x801018a4           */
        && g->arc1024 != 0                                    /* beq-away @0x801018b0              */
        && (g->global_cfbf6 & 0x17u) != 0) {                  /* andi @0x801018c4                  */
        out->rng_draws++;                                     /* jal @0x801018d0                   */
        if ((re2z_rand() & 1u) == 0u) {                       /* andi @0x801018d8 / bne @0x801018dc */
            out->wrote = 1; out->word = 0x00000C01u;          /* sw @0x801018e4                    */
        }
    }
    /* --- the G / J fork ----------------------------------------------------------------------- */
    if (g->pl_8 != 15) {                                      /* lbu @0x801018e8 / beq @0x801018f0 */
        /* --- G: the SIDE GRAB. Two INDEPENDENT halves; G1 falls THROUGH into G2. ------------- */
        if (g->dist < 1200u                                   /* sltiu 0x4b0 @0x801018f4           */
            && !(g->pl_1d3 & 0x80u)                           /* andi @0x80101908 / bne @0x8010190c */
            && g->self_106 == g->pl_106) {                    /* bne-away @0x80101920              */
            if (!(g->self_21a & 0x20u) && g->g1_sector_hit) { /* andi @0x80101930; jal @0x80101948 */
                out->wrote = 1; out->word = 0x00000301u;      /* sw @0x80101958                    */
                out->claim_player = 1;                        /* PL[0x1D3] |= 0x80 @0x80101968     */
            }
            if (!(g->self_21a & 0x40u) && g->g2_sector_hit) { /* andi @0x80101974; jal @0x8010198c */
                out->wrote = 1; out->word = 0x00000301u;      /* sw @0x8010199c                    */
                out->claim_player = 1;                        /* PL[0x1D3] |= 0x80 @0x801019b0     */
            }
        }
        /* j 0x801019e8 @0x801019AC — the G branch ALWAYS skips J */
    } else {
        /* --- J: block B minus the +0x106 test ------------------------------------------------ */
        if (g->self_23e == 0                                  /* lbu @0x801019b4 / bne @0x801019bc */
            && g->arc512 == 0                                 /* sll/bne @0x801019c0/c4            */
            && g->dist < 2000u                                /* sltiu @0x801019c8                 */
            && g->pl_1d3 == 0) {                              /* bne @0x801019dc                   */
            out->wrote = 1; out->word = 0x00000E01u;          /* sw @0x801019e4                    */
        }
    }
    /* --- K: the LAST writer, so it beats every block above ------------------------------------ */
    if (g->pl_156 == (int16_t)-32768                          /* lh 342(s1) @0x801019e8            */
        && g->arc512 == 0                                     /* sll/bne @0x801019f4/f8            */
        && g->dist < 1000u) {                                 /* sltiu 0x3e8 @0x801019fc           */
        out->wrote = 1; out->word = 0x00060801u;              /* lui 0x6/ori 0x801/sw @0x80101a10  */
        out->set_10e_4000 = 1;                                /* +0x10E |= 0x4000 @0x80101a08/14/18 */
    }
    return out->wrote;
}

/* ============================================================================================
 * WELLE B — THE FULL RE2 ZOMBIE BRAIN (behavior + presentation).
 *
 * re15_re2z_tick REPLACES the RE1.5 state dispatch for an owned zombie (hook in
 * re15_enemy_ai_live_tick / re15_zgirl_ai_tick). Structure mirrors the overlay:
 *   root prolog (cooldown bank) @0x8010045C-98  ->  state table @0x8010C830:
 *   [0] INIT 0x8010065C  [1] ACTIVE 0x8010114C (DECISION @0x8010C88C then EXECUTOR @0x8010C8CC
 *   on the SAME tick — 0x8010118C re-reads +0x5 between the two, @0x801011A8-EC)
 *   [2] HURT 0x80104F40  [3] DEATH 0x80108250  [7] CORPSE 0x8010A440  [8] 0x80109CFC.
 *
 * PRESENTATION: clip indices are RE2-NATIVE (the Welle-A re2_ems loader fills the actor's bank
 * with the real EM01x EMD, 31-clip action bank); SEs go through the ENEMSE hook below.
 * Every constant carries its EMZ0.BIN address (all re-disassembled 2026-08-10, see the
 * per-line cites). OPEN items are marked OPEN, mapped stand-ins are marked MAPPING.
 * ==========================================================================================*/

/* ---- the byte-verified model parameter block @0x80100000 (EMZ0.BIN offsets 0x00..0xDF) ----
 * dump 2026-08-10: 05 00 00 00 | 00 02 00 02 00 02 02 00 | 0b 0b 0e 0e | 0b 0b 0e 0e |
 *                  10 14 01 05 10 1e 01 0a | <10 grab-phase ptrs == 0x801026C0..0x80102EB4> |
 *                  @0x44: 01 02 17 16 08 09 | @0x4C: 5a 00 1e 00 ... | @0xD8: 03 03 04 0d */
static const uint8_t re2z_param_walk[8]  = { 0, 2, 0, 2, 0, 2, 2, 0 };       /* @0x80100004 */
static const uint8_t re2z_param_grab[8]  = { 0x0B,0x0B,0x0E,0x0E,0x0B,0x0B,0x0E,0x0E }; /* @0x8010000C
                                              * P0 reads [s5*2] (`sll v0,s5,1; lbu 16(v0)`
                                              * @0x801026C4-CC) -> upright 0x0B, crawler 0x0E */
static const uint8_t re2z_param_bite[8]  = { 16,20, 1,5, 16,30, 1,10 };      /* @0x80100014
                                              * (bite-frame, damage) pairs, index s5*2:
                                              * upright (16,20), crawler (1,5), kind 0x11/0x17
                                              * upright (16,30) / crawler (1,10) @0x801028A0-FC */
static const uint8_t re2z_param_clips[6] = { 0x01,0x02,0x17,0x16,0x08,0x09 };/* @0x80100044
                                              * [0..1] knockdown-fall L/R (EXEC[5] P0 @0x801032C8),
                                              * [2..3] corpse-lie 23/22 (@0x8010A490-BC),
                                              * [4..5] ground-lie idle 8/9 (@0x80103574-A8) */
static const uint8_t re2z_param_getup[4] = { 3, 3, 4, 13 };                  /* @0x801000D8
                                              * EXEC[9] get-up clip list (copied to sp+16
                                              * @0x80103E6C-84) */
static const uint8_t re2z_param_feed[8]  = { 0x12,0x13,0x14,0x12,0x13,0x14,0x12,0x13 };
                                             /* @0x801000A8 (bytes gedumpt) — der Fress-Loop
                                              * rotiert per rand&7 (@0x80103BF4-C10 P0,
                                              * @0x80103CB0-D4 P2 Re-Draw) */
static const uint8_t re2z_param_walk13[8] = { 0, 2, 0, 2, 0, 2, 0, 2 };       /* @0x801000F8
                                              * (bytes gedumpt) — EXEC[13]s EIGENE Stil-Tabelle
                                              * (kopiert @0x80104958-74), EIN Draw + andi 7
                                              * (@0x801049C4-EC) — NICHT die INIT-Tabelle */
static const uint16_t re2z_hp13_tbl[16] = { 70,84,118,65,50,85,48,65,40,73,69,56,70,55,72,55 };
                                             /* @0x8010C600 (selbst gelesen) — EXEC[13]s
                                              * HP-Re-Roll `sh v1,342(s1)` @0x801049C8 */
/* ⛔ DIE LIEGEZEIT-TABELLE DES STURZES — u16[16] @0x8010004C, selbst gelesen 2026-08-21:
 *   0x8010004c: 5a 00 1e 00 aa 00 a0 00 0a 00 28 00 64 00 32 00
 *   0x8010005c: 6e 00 14 00 23 00 46 00 1e 00 b4 00 1e 00 3c 00
 * EXEC[5] P2 zieht daraus `+0x15A` (@0x80103428-88) — das ist die Dauer, die der gefallene
 * Zombie AM BODEN bleibt, bevor die Aufsteh-Kette P6/P7/P8 laeuft. Der Port kannte bisher nur
 * den ERSTEN Eintrag (90) und hat ihn dann ganz entfernt ("KEIN Timer", Review #6/[4]) — deshalb
 * gab es im Port ueberhaupt keine Bodenzeit. */
static const uint16_t re2z_lie_tbl[16] = { 90,30,170,160,10,40,100,50,110,20,35,70,30,180,30,60 };
                                             /* @0x8010004C */

/* ---- ENEMSE audio hook (PC registers; the engine stays link-clean for PSX) ---------------- */
static void (*s_re2z_se_fn)(int se_id, int flag2000) = 0;
static void (*s_re2z_se_bank_fn)(int bank) = 0;

/* Zombie ENEMSE bank — EMPIRISCH (bleibt eine deklarierte PORT-NAEHERUNG, siehe unten).
 * Probe ueber alle 73 dekodierten EDT-Maps: Bank 0 ist eine der Baenke, deren Live-Map JEDE
 * SE-id abdeckt, die das Zombie-Overlay ausloest (eigener jal-0x8005bd6c-Scan ueber
 * EMOVL10_S0.BIN, 2026-08-17: ids {2,3,4,5,7,8,9,10,11,12,13} an 38 Stellen, id 12 allein
 * 12x). RE15_RE2_SE_BANK uebersteuert fuer A/B-Hoerproben.
 *
 * ⛔ NUTZER-REPORT 2026-08-17 "die Zombies haben den falschen Sound" — WAS DAZU BELEGT IST:
 * Der MECHANISMUS ist jetzt byte-true disassembliert (re2_ems.c re2_enemse_select_bank,
 * FUN_80052b38 @0x80052b40-c2c + Raum-kind-Paar-Aufbau im Enemy-Spawn @0x8005728c-b8 +
 * Bank-Lader FUN_8005a09c). Er liefert die Bank NICHT aus dem Gegner-kind, sondern aus einem
 * EIGENEN Byte der RE2-RAUMDATEN:
 *     Spawn-Record +3 -> KIND   (`jal 0x8001b710` @0x800571f0; die 0x10..0x1F-Klemme
 *                        @0x8001b738-40 beweist den kind-Wertebereich; Modell-Binder
 *                        FUN_8001aaa8 liest den kind aus entity+0x8 @0x8001aac8)
 *     Spawn-Record +7 -> SOUND-ID (`lbu v0,7(v0)` @0x80057274 -> `sb v0,0x1fa(s0)` @0x80057280
 *                        -> DAT_800d8cd0/cd1 @0x8005728c-b8 -> FUN_80052b38)
 * Das sind ZWEI VERSCHIEDENE Bytes. Die Paar-Tabelle @0x800a7400 fuehrt ausserdem 15 ids
 * < 0x10 (0x01..0x0F), die im CDEMD0-kind-Raum (Minimum 0x10) gar nicht existieren — die
 * Tabelle lebt also NICHT im kind-Raum. Ein byte-true kind->Bank-Mapping kann es damit nicht
 * geben; es braeuchte die RE2-RAUMDATEN (record+7), die in diesem Repo NICHT liegen
 * (info/re2leon enthaelt nur COMMON/BIN, PL0, ZMOVIE, PSX.EXE).
 * GEGENPROBE, warum hier NICHT auf "Zeile 11 = {0x10,0x00}" umgestellt wurde: deren Live-Map
 * endet bei id 10 — die im Overlay meistgenutzten ids 11/12/13 waeren stumm. Die id-Deckung
 * ist aber ebenfalls kein Beweis (viele Baenke decken die Menge), darum bleibt es beim
 * Bestandswert und der Punkt als OPEN dokumentiert statt eine Zahl gegen eine andere zu
 * tauschen. */
#define RE2Z_ENEMSE_BANK 0

void re15_re2z_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int))
{
    s_re2z_se_fn      = se_fn;
    s_re2z_se_bank_fn = bank_fn;
    if (s_re2z_se_bank_fn) {
        const char *ov = getenv("RE15_RE2_SE_BANK");
        s_re2z_se_bank_fn(ov ? atoi(ov) : RE2Z_ENEMSE_BANK);
    }
}
static void re2z_se(int id) { if (s_re2z_se_fn) s_re2z_se_fn(id, 0); }
void re15_re2z_se_play(int se_id) { re2z_se(se_id); }   /* Frame-Flag-SFX 0x801016c8-Pfad */

/* ---- FUN_800401d4 — the grab-bite player damage (decompile-read; port of the mechanism) ----
 * HP store 0x800CFD4E (= PL+0x156). Death: HP < -14 OR the one-save latch (0x800CFB74|0x1000)
 * already set; otherwise the first lethal bite is "one save": HP = 0 + latch. Return bits used
 * by the grab P3 (@0x80102900-24): bit0 -> throw-off, bit1 -> player dead (self word 0x601 +
 * PL cmd (dir<<8)|6 @0x80102924-50).
 * NOT ported (documented): the x1.5 / x5 / x2 damage scalings — they hang off RE2-only globals
 * (flag 0x40 @0x800CFB74, HP>120 on RE2's 200-HP scale, difficulty @0x800D482A) that have no
 * RE1.5-port equivalent. Raw byte-cited damages apply 1:1. */
static int s_re2z_onesave = 0;   /* cleared with the PRNG on room load (re15_re2z_rng_reset) */
static int re2z_player_damage(re15_actor_t *pl, int dmg)
{
    pl->hp = (int16_t)(pl->hp - dmg);
    if (pl->hp < 0) {
        if (pl->hp < -14 || s_re2z_onesave) return 2;    /* death (bit1) */
        pl->hp = 0; s_re2z_onesave = 1; return 1;        /* one save (bit0) -> throw-off */
    }
    return 0;
}
void re15_re2z_onesave_reset(void) { s_re2z_onesave = 0; }

/* WELLE D: der volle FUN_800401d4-MODE-Parameter (Decompile RE2_Quellcode_V2/FUN_800401d4.c,
 * selbst gelesen 2026-08-16). Der Kraehen-GRAB-Peck ruft (5, aliveflag) @0x8010265C-64
 * (`lbu a1,536(s0); jal 0x800401d4; addiu a0,zero,5` — a1 = +0x218 = HP>0?1:0, Prolog
 * @0x801024C4-D8):
 *   mode 1 (Spieler lebt): HP-=dmg; HP>=0 -> 0; HP<0 -> KEIN Todespfad, faellt in den
 *          One-Save-Schwanz (HP=0 + Latch, ret 1) — der Peck allein toetet NIE direkt.
 *   mode 0 (HP<=0):        wie gehabt (HP<-14 ODER Latch -> ret 2 Tod; sonst One-Save).
 *   mode >1:               ret 1 ohne One-Save-Schreiber (`if (param_2 != 0) return 1`).
 * Die x1.5/x5/x2-Skalierungen bleiben wie in Welle B unportiert (RE2-only-Globals). */
int re15_re2_player_damage_mode(re15_actor_t *pl, int dmg, int mode)
{
    if (mode == 0) return re2z_player_damage(pl, dmg);
    pl->hp = (int16_t)(pl->hp - dmg);
    if (mode != 1) return 1;                             /* `if (param_2 != 0) return 1` */
    if (pl->hp >= 0) return 0;                           /* `-1 < param_1*0x10000 -> 0` */
    pl->hp = 0; s_re2z_onesave = 1; return 1;            /* One-Save-Schwanz */
}
/* WELLE C: the dog's flight bite runs through the SAME FUN_800401d4 (jal @0x80104EBC, a0=20,
 * a1=0) and shares the ONE-SAVE latch DAT_800cfd4c bit 0x1000 with the zombie bite — one
 * exported entry keeps that latch single. Return: 0 survived / 1 one-save / 2 death. */
int re15_re2_player_damage(re15_actor_t *pl, int dmg) { return re2z_player_damage(pl, dmg); }

/* 0x800CFBF6 movement bits from PORT state (the mapping the ladder already used; see the
 * fill_gates comment for the producer chain @0x8003CC80/@0x8003D18C/@0x8003D6B4). */
static uint16_t re2z_cfbf6(const re15_actor_t *pl)
{
    if (pl->motion == 100) return 0x04u;     /* RUN  @0x8003D18C */
    if (pl->motion == 105) return 0x02u;     /* WALK @0x8003CC80 / @0x8003D6B4 */
    return 0;
}

/* Clip-word write `sw (rate<<16)|(frame<<8)|clip, 332(s)` -> port anim fields. The +0x14E rate
 * half maps onto the port's crossfade seed (same 0xF/7 family as RE1.5's +0x8f), the advance
 * helper 0x8002959c's a3 (256/512) onto anim_blend_rate (0x100/0x200).
 *
 * ============================================================================================
 * ⛔ WAS `loop` HIER BEDEUTET — UND WARUM ES KEIN GESCHMACKSPARAMETER IST
 * --------------------------------------------------------------------------------------------
 * NUTZER-REPORT 2026-08-21: "Bei RE2 AI ist die Sterbeanimation nicht vollstaendig, friert ein
 * vor dem kompletten Tod."
 *
 * DER RE2-ADVANCE HAT KEIN "HOLD-LAST". Selbst disassembliert (info/re2leon/PSX.EXE):
 * FUN_8002959C loest nur den Frame-Eintrag auf (`sw a2,376(a0)` @0x800295F8) und ruft
 * FUN_80029614 @0x800295FC; DESSEN Schwanz ist der Frame-Zaehler:
 *   80029b28: lbu   v0,333(s2)      ; +0x14D = Frame-Byte
 *   80029b30: addiu v0,v0,1
 *   80029b34: sb    v0,333(s2)
 *   80029b38: andi  v0,v0,0xff
 *   80029b3c: sltu  v0,v0,s3        ; s3 = Frame-Count des Clips (`lhu s3,0(v0)` @0x80029680)
 *   80029b40: bne   v0,zero,0x80029b50   ; noch drin -> return 0 (Delay-Slot `addu v0,zero,zero`)
 *   80029b48: sb    zero,333(s2)    ; **WRAP AUF 0**
 *   80029b4c: addiu v0,zero,1       ; return 1
 * Ein Clip laeuft also GENAU SO LANGE, WIE SEIN AUFRUFER WEITER ADVANCED — und wrappt dabei.
 * Wer die Rueckgabe in `+0x6` uebernimmt, verlaesst die Phase im Wrap-Tick (= play-once);
 * wer sie VERWIRFT, laesst den Clip endlos loopen, bis eine andere Bedingung die Phase beendet.
 *
 * Eigener Scan ALLER 70 `jal 0x8002959c` in EMOVL10_S0.BIN mit Ruecklauf auf die Verwendung von
 * v0: die Rueckgabe wird u.a. an diesen Stellen VERWORFEN —
 *   @0x801095D0  death_MAGNUM P4 (der kopflose Weiterlauf, Ausstieg = Timer +0x158)
 *   @0x80109288  death_RIP    P6 (der zerfetzte Torso taumelt,  Ausstieg = Timer +0x158)
 *   @0x80105790 / @0x801058C0  hit_MAIN P1/P2 (Ausstieg = die +0x158-Rampe)
 * Der Port-Advancer (re15_actors_anim_advance, player_common.c) PINNT play-once-Clips
 * dagegen auf fc-1 — das ist die Emulation von "der Aufrufer hoert auf zu advancen" und fuer
 * genau diese Phasen FALSCH. GEMESSEN (probe_re2z_deathgetup A, 64 Seeds, Magnum):
 *   f806..f818 st=3 s1=5 s2=4 clip=2 af=53/54 kf=118 LOCO b8dy=-2735  -> 13 Frames UNBEWEGT
 * 14 von 64 Laeufen mit Pin, laengster 22 Frames = die stehengebliebene Sterbeanimation.
 * Deshalb tragen die Clip-Setzer dieser Phasen `loop = 1`.
 * ========================================================================================== */
static void re2z_clip(re15_actor_t *e, int clip, int frame, int frac, int blend, int loop)
{
    e->motion     = (int16_t)clip;
    e->anim_frame = (uint16_t)frame;
    e->anim_frac  = (uint8_t)frac;
    e->anim_blend_rate = (uint16_t)blend;
    e->anim_freeze = 0;
    e->anim_flags &= (uint16_t)~(0x80u | 0x04u);
    if (loop) e->anim_flags |= 0x04u;
}

/* current loop slot of the playing clip (original +0x14D frame byte; the port counter is
 * monotonic, the renderer takes slot = frame % frame_count). No bank -> raw frame. */
static int re2z_frame_slot(const re15_actor_t *e)
{
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return (int)e->anim_frame;
    return (int)(e->anim_frame % (uint32_t)fc);
}
static int re2z_clip_done(const re15_actor_t *e)
{
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return 1;                                /* no bank -> never stall the FSM */
    return ((int)e->anim_frame >= fc - 1) ? 1 : 0;
}

/* the walk moan block @0x80101C44-88: gate +0x239==0; (rand&0x1F)==0 -> SE 10, else a SECOND
 * draw (rand&0x1F)==0 -> SE 11; on either hit +0x239 = 150. The draw COUNT is behaviour. */
static void re2z_walk_moan(re15_actor_t *e)
{
    if (e->re2z_cd239 != 0) return;                       /* @0x80101C44-4C */
    if ((re2z_rand() & 0x1fu) == 0u) {                    /* @0x80101C54-60 -> SE 10 */
        re2z_se(10); e->re2z_cd239 = 150;                 /* @0x80101C7C-88 */
    } else if ((re2z_rand() & 0x1fu) == 0u) {             /* @0x80101C68-74 -> SE 11 */
        re2z_se(11); e->re2z_cd239 = 150;
    }
}

/* RE1.5-mapped hit-FX stand-in (DOCUMENTED): RE2's FX system (FUN_8001bf10, packed group ids
 * like 0x0A001000 @0x80104DE0-F4) is not ported (Lane-I §3: row-format compatibility unproven).
 * The port spawns the RE1.5 room-bank blood the same way the RE1.5 bite/gore code does. */
/* [PORT-MAPPING] FUN_8001BF10(a0 = gepackte Effekt-Id, a1 = WINKEL, a2 = &Anker-Matrix,
 * a3 = &Geschwindigkeit|0). Der Port hat keinen ankergebundenen RE2-Emitter; der Stand-in ist
 * ein RE1.5-Raumbank-Spawn. Der WINKEL a1 wird 1:1 durchgereicht (Parameterwort des ESP-Slots),
 * damit der Wert im Port derselbe ist wie im Original.
 *
 * ⛔ NUTZER-REPORT 2026-08-19: "Das Blut beim Treffen erscheint am Fuss."
 * GEMESSEN (probe_re2_stagger, ROOM1140, echter Weg, geladene RE2-Bank EM010):
 *   Aktor-Wurzel (-1800,   0,-19600)      <- hier spawnte der Port
 *   Bone0 Huefte (-1800,-1166,-19600)     <- hier spawnt das Original  (dy = -1166)
 *   Bone1 Brust  (-1800,-1166,-19600)
 * Der Stand-in war POSITIONSLOS und benutzte die Aktor-Wurzel — und die liegt in RE1.5/RE2 auf
 * dem BODEN (+0x38/+0x3C/+0x40, +0x3C == Bodenhoehe). Also: Blut an den Fuessen.
 *
 * ---- DER ANKER IST EINE KNOCHEN-MATRIX (selbst disassembliert, re2leon/PSX.EXE) -------------
 * FUN_8001BF10 kopiert aus a2 GENAU 32 Byte = eine ganze PSX-MATRIX in den Effekt-Slot und
 * merkt sich zusaetzlich den ZEIGER, d.h. der Effekt HAENGT am Knochen:
 *   8001c03c: lw v0,0(a2)   8001c040: lw v1,4(a2)   8001c044: lw a0,8(a2)   8001c048: lw a1,12(a2)
 *   8001c04c: sw v0,76(t0)  8001c050: sw v1,80(t0)  8001c054: sw a0,84(t0)  8001c058: sw a1,88(t0)
 *   8001c05c: lw v0,16(a2) …                        8001c06c: sw v0,92(t0) … 8001c078: sw a1,104(t0)
 *   8001c094: sw a2,108(t0)      ; der ANKER-Zeiger (den FUN_8001CEFC spaeter vergleicht)
 * a2 ist immer `+0x198 + n*172 + 72` — der Modellblock hat Part-Stride 172 und die Part-MATRIX
 * bei +0x48 = 72 (Part 0 -> +72, Part 1 -> +244, Part 3 -> +588, Part 8 -> +1448 …).
 * Port-Zwilling der Matrix-Translation ist re15_enemy_bone_world_pos() — dieselbe QUERY-Pose,
 * die der RE2-HUND (enemy_ai_re2_dog.c re2d_fx) schon benutzt; der Zombie war der Ausreisser. */
/* Port-Diagnose (nur Tests, die Engine liest das nicht): Anker-Part + Weltposition des zuletzt
 * gespawnten Treffer-Effekts. */
static int     s_re2z_last_fx_part = -1;
static int32_t s_re2z_last_fx_pos[3];
int  re15_re2z_last_fx_part(void) { return s_re2z_last_fx_part; }
void re15_re2z_last_fx_pos(int32_t out[3])
{
    out[0] = s_re2z_last_fx_pos[0]; out[1] = s_re2z_last_fx_pos[1]; out[2] = s_re2z_last_fx_pos[2];
}

static int re2z_part_to_bone(const re15_actor_t *e, int part);   /* fwd: Part -> Bank-Bone-Slot */

static void re2z_blood_fx_at(re15_actor_t *e, int part, int16_t yaw)
{
    int32_t p[3];
    /* `part` ist eine RE2-PART-Nummer, re15_enemy_bone_world_pos will einen BONE-SLOT der
     * geladenen Bank — im RE1.5-Modus sind das verschiedene Nummernraeume (Block ueber
     * re2z_part_to_bone). Im RE2-Modus ist die Umrechnung die Identitaet. */
    re15_enemy_bone_world_pos(e, re2z_part_to_bone(e, part), p);
                                             /* == die Translation der Part-Matrix +0x198+n*172+72 */
    s_re2z_last_fx_part = part;
    s_re2z_last_fx_pos[0] = p[0]; s_re2z_last_fx_pos[1] = p[1]; s_re2z_last_fx_pos[2] = p[2];
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500, p[0], p[1], p[2], yaw);
}

/* Die ANKER-WAHL der drei Treffer-Emitter (Wurzel-Flinch @0x801050B0-158, Haupt-Handler-P0
 * @0x80105650-704, Stagger-P0 @0x80105D14-DBC — alle drei wortgleich):
 *     801050b0: lbu a0,466(s1)          ; +0x1D2
 *     801050b4-dc: a0 % 3               ; magisches multu 0xAAAAAAAB -> ZONE
 *     801050e4: bne a0,zero,0x8010511c
 *     801050f4: lw a2,408(s1)           ; +0x198
 *     80105110: _addiu a2,a2,244        ; ZONE 0  -> Part 1 (1*172+72)
 *     8010514c: lw s0,408(s1)
 *     80105158: _addiu a2,s0,72         ; sonst   -> Part 0 (0*172+72) */
static int re2z_blood_anchor(const re15_actor_t *e)
{
    return (((unsigned)e->re2z_hits1d2 % 3u) == 0u) ? 1 : 0;
}

static void re2z_blood_fx_dir(re15_actor_t *e, int16_t yaw)
{
    re2z_blood_fx_at(e, re2z_blood_anchor(e), yaw);
}

static void re2z_blood_fx(re15_actor_t *e)
{
    re2z_blood_fx_dir(e, (int16_t)e->rot_y);
}

/* FUN_800152C8(self, yaw_off) — applies the +0x144 vector yaw-rotated (self-disasm'd:
 *   lhu +0x76; += a1; RotY 0x8008e1f4; apply 0x8008dba4 on &self+0x144 @0x80015304;
 *   +0x38 += out.x @0x80015314-20, +0x40 += out.z @0x80015324-34).
 * The vector is REFILLED each tick by 0x80015e7c with the clip root DELTA (`sh v1,324(t0)`
 * @0x80015FD8-E4) — the e7c+152c8 pair is ONE delta application in the port
 * (re15_re2z_move_root). This helper only carries the EXTRA pushes computed into the vector
 * BETWEEN e7c and 152c8 (grab P8: delta.x -= 30 @0x80102CA0-AC). Rotation is linear, so the
 * split application is bit-equivalent to the in-vector addition. The bare `sh 11,324` seeds
 * (@0x80104824-28 / @0x80104DC8-CC / @0x80103614-1C) are DEAD STORES: the next 0x80015e7c
 * overwrites the vector before any 152c8 reads it — not modeled, documented. */
static void re2z_thrust(re15_actor_t *e, int spd)
{
    if (!spd) return;
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * spd) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * spd) >> 12);
}
/* Zwilling mit Yaw-Offset (0x800152C8 mit a1 != 0) — Definition weiter unten, hier vorgezogen,
 * weil EXEC[9] (re2z_exec_getup) ihn schon braucht. */
static void re2z_thrust_yaw(re15_actor_t *e, int spd, int yaw_ofs);

/* ⛔ RESOLVER-LATCH-FREIGABE (+0x93 Bit 0) — Nutzer-Blocker 2026-08-19
 * "Bei der RE2-AI kann ich immer noch keinen Zombie treffen. Weder mit Schusswaffe noch mit
 * Messer." GEMESSEN (probe_re2_hitpath, ROOM1140, echter Weg game_step + R1/SQUARE, Ziel = der
 * stehende Zombie Slot 2 Typ 0x10):
 *   RE1.5-Flavor, Pistole, 120 Frames Dauerfeuer -> 3 TREFFER, +0x93 endet 0x80
 *   RE2  -Flavor, Pistole, 120 Frames Dauerfeuer -> 1 TREFFER, +0x93 bleibt 0x01 (fuer immer)
 *   RE1.5-Flavor, Messer  -> Treffer, +0x93 endet 0x00
 *   RE2  -Flavor, Messer  -> 1 Treffer, +0x93 bleibt 0x01
 * Nach dem ERSTEN Treffer ist der Zombie im RE2-Modus dauerhaft UNTREFFBAR.
 *
 * MECHANISMUS (selbst disassembliert, RE1.5 PSX.EXE — der Port faehrt DIESEN Resolver
 * FUN_80011f50 in BEIDEN Flavors, es gibt keinen portierten RE2-Resolver):
 *   SETZEN     80012490+ ... 800124e8: lbu v0,147(s1)
 *                             800124f0: ori v0,v0,0x1        (danach sb) = Ein-Treffer-Latch
 *   AUSSCHLUSS 800120c0: lui s4,0x300          (Maske 0x03000000 = +0x93 Bits 0|1)
 *              800120f4: lw  v0,144(s0)        (+0x90-Wort, +0x93 ist dessen High-Byte)
 *              800120fc: and v0,v0,s4
 *              80012100: beq v0,s4,0x80012124  -> Kandidat UEBERSPRUNGEN
 *   ZWEITKONTAKT 800123fc: lbu v1,147(s1) / 80012404: andi v0,v1,0x1 / 80012408: beq
 *              8001240c: ori v0,v1,0x2 / 80012410: sb v0,147(s1) / 80012418: jal 0x80011f50
 *              -> Bit 1 dazu, Rekursion auf das NAECHSTE Opfer. Ein Aktor mit Bit0 gesetzt
 *                 kassiert also NIE wieder Schaden, bis Bit 0 geloescht wird.
 *   FREIGABE (RE1.5-Overlay STAGE1.BIN, Zwilling dieser Zeile):
 *              80105f9c: lbu v0,147(v1)
 *              80105fa4: andi v0,v0,0xfe
 *              80105fac: sb  v0,147(v1)        (Stagger-Ausgang -> ACTIVE 0x10201)
 *              gleiche Freigabe im Knockdown-Ausgang 80106b8c-90 / 80106a1c-20 und im
 *              Liege-Wecker 80103b5c-68.
 *   D.h. in RE1.5 gehoert die Freigabe dem TREFFER-REAKTIONS-HANDLER: Phase 0 setzt
 *   `+0x93 |= 1` (@0x80106af4-fc / @0x80106958-60), die Endphase loescht es beim Ruecksprung
 *   nach ACTIVE.
 *
 * WARUM ES IM RE2-BRAIN FEHLTE (nachgewiesen, nicht vermutet): das RE2-Overlay hat das Feld
 * gar nicht. Eigener Voll-Scan beider RE2-Binaries nach `sb/lbu rt,147(rs)`:
 *   info/re2leon/COMMON/BIN/EMOVL10_S0.BIN : 0 Treffer
 *   info/re2leon/PSX.EXE                   : 0 Treffer
 * (RE2 hat ein anderes Entity-Layout — z.B. HP als +0x156 `sh -1,342` @0x8010A4D4 statt
 * RE1.5 +0x9a.) Das +0x93-Protokoll ist also reines RE1.5-EXE-Resolver-Eigentum, und mit
 * der Uebernahme der Zustandsmaschine durch den RE2-Brain fiel der EINZIGE Freigeber weg.
 * PORT-VERTRAG, kein geratener Wert: dieselbe Maske 0xfe (@0x80105fa4) an derselben
 * STRUKTURELLEN Stelle — dem Endausgang jeder RE2-Treffer-Reaktion zurueck nach ACTIVE. */
static void re2z_hit_latch_release(re15_actor_t *e)
{
    e->hit_react &= (uint8_t)~1u;                                  /* andi 0xfe @0x80105fa4 */
}

/* ============================== ACTIVE: DECISIONS ========================================== */

/* DECISION[0] @0x80101294 (stand) — self-disasm'd 2026-08-10:
 *   a1024 = arc(PL,1024) @0x801012C4-C8; a800 = arc(PL,800) @0x801012E4;
 *   dist<0x1388 && a1024==0 && (+0x154&0x800)                  -> 0x101  @0x80101308-1C
 *   dist<0xBB8 && a800!=0 && (cfbf6&0x15) && (+0x154&0x800) && (rand&1)==0 -> 0xC01 @0x80101374-78
 *   dist<0x7D0 && a1024!=0 && (cfbf6&0x17) && (+0x154&0x800) && (rand&1)==0 -> 0xC01 @0x801013CC-D0
 * +0x154 & 0x800: an actor flag word with no identified port producer -> MAPPED 1 (OPEN). */
static void re2z_decide_stand(re15_actor_t *e, re15_actor_t *pl)
{
    uint32_t dist = e->ai_dist;
    int a1024 = re15_ai_arc_test(e, pl->x, pl->z, 1024);
    int a800  = re15_ai_arc_test(e, pl->x, pl->z,  800);
    uint16_t mv = re2z_cfbf6(pl);
    if (dist < 0x1388u && a1024 == 0)
        re15_ai_set_state_word(e, 0x101);                          /* @0x80101318-1C */
    if (dist < 0xbb8u && a800 != 0 && (mv & 0x15u) && (re2z_rand() & 1u) == 0u)
        re15_ai_set_state_word(e, 0xC01);                          /* @0x80101374-78 */
    if (dist < 0x7d0u && a1024 != 0 && (mv & 0x17u) && (re2z_rand() & 1u) == 0u)
        re15_ai_set_state_word(e, 0xC01);                          /* @0x801013CC-D0 */
}

/* DECISION[1] = the W2 ladder (@0x80101714) — now applied for real. */
static void re2z_decide_walk_apply(re15_actor_t *e, re15_actor_t *pl)
{
    re15_re2z_gates_t g; re15_re2z_decision_t d;
    re15_re2z_fill_gates(e, pl, re15_player_is_grabbed(), &g);
    int committed = re15_re2z_decide_walk(&g, &d);
    if (getenv("RE15_RE2_TRACE") && (committed || e->ai_dist < 1500u))
        fprintf(stderr, "[re2z] gates d=%u flo=%u/%u claimed=%u 21a=%04x cd23e=%u g1=%d g2=%d -> %s 0x%08x\n",
                g.dist, g.self_106, g.pl_106, g.pl_1d3, g.self_21a, g.self_23e,
                g.g1_sector_hit, g.g2_sector_hit, committed ? "COMMIT" : "none", d.word);
    if (committed)
        re15_ai_set_state_word(e, d.word);   /* every word now has a live executor below */
    /* d.claim_player (@0x80101968/@0x801019B0): the port claim = the grab pin; EXEC[3] P0 runs
     * on this SAME tick (decision-then-executor) and performs the full latch. */
}

/* ============================== ACTIVE: EXECUTORS ========================================== */

/* EXEC[0] @0x801013F4 (stand): P0 @0x80101458 clip 0 rate 0xF at a RANDOM start frame
 * (((rand&0x1F)<<8)|0xF0000|0), +0x158 = rand+150 @0x80101480-88, +0x15A = rand+300
 * @0x8010148C-90. P1 @0x80101494: advance(256); +0x15A expiry -> 1/2 moan SE 11 @0x801014CC-EC
 * (cooldown 150), reseed rand+300 @0x801014F0-FC. */
static void re2z_exec_stand(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        uint32_t r = re2z_rand();
        re2z_clip(e, 0, (int)(r & 0x1fu), 0xF, 0x100, 1);          /* @0x80101458-70 */
        e->sub_state_2 = 1;                                        /* @0x80101474-7C */
        e->re2z_t158 = (int16_t)(re2z_rand() + 150);               /* @0x80101478-88 */
        e->re2z_t15a = (int16_t)(re2z_rand() + 300);               /* @0x80101484-90 */
        return;
    }
    if (e->re2z_t15a != 0) { e->re2z_t15a--; return; }             /* @0x801014A8-B8 */
    if (e->re2z_cd239 == 0 && (re2z_rand() & 1u) != 0u) {          /* @0x801014BC-D8 */
        re2z_se(11); e->re2z_cd239 = 150;                          /* @0x801014E0-EC */
    }
    e->re2z_t15a = (int16_t)(re2z_rand() + 300);                   /* @0x801014F0-FC */
}

/* EXEC[1] @0x80101A40 (walk): P0 @0x80101A74 clip word = +0x218 | 0xF0000 (@0x80101A7C-8C) +
 * the gait seeding (the W1 machine). Per tick: moan block, the W1 turn gate, clip-driven
 * movement. Steer point +0x1C4/+0x1C6 = the RE2 navigator output; the PORT equivalent is its
 * own nav steer (re15_nav_update_steer fills e->steer_x/z each tick BEFORE this brain runs —
 * cross-zone it pathfinds exactly like FUN_8004A808's first-hop would). */
static void re2z_exec_walk(int slot, re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        re2z_clip(e, e->re2z_walkclip, 0, 0xF, 0x100, 1);          /* @0x80101A74-8C */
        e->sub_state_2 = 1;
        /* gait rows/timer seed themselves through re2z_gait_tick's entry tag */
    }
    re2z_walk_moan(e);                                             /* @0x80101C44-88 */
    re15_re2z_walk_turn(e, e->steer_x, e->steer_z, e->ai_dist);    /* @0x80101BAC-CAC */
    /* WALK movement = the e7c+152c8 pair (@0x80101CBC + @0x80101D60): 0x80015e7c computes the
     * PAIR-1 clip root delta into +0x144, 0x800152C8 applies it rotated -> ONE delta
     * application in the port. NOT the RE1.5 foot-lock: pair-1 clips 0/2 carry the forward
     * translation in the kf root fields (byte-read: clip 0 sx 2/47/121/209...). */
    re15_re2z_move_root(e);                                        /* e7c @0x80101CBC + 152c8 @0x80101D60 */
    (void)slot; (void)pl;
    /* WALK edge-fall death commits 0xA03/0xB03 (@0x80101E64/6C) — no reachable cliff geometry
     * in the RE1.5 rooms the port ships; OPEN, documented. */

    /* ========================================================================================
     * ⛔ OFFEN, VOLLSTAENDIG DISASSEMBLIERT, BEWUSST NOCH NICHT SCHARF:
     *    DIE DREI PRODUZENTEN VON `+0x4 = 0x201` (EXEC[2], der Gang mit den erhobenen Armen).
     *
     * Der Port fuehrte EXEC[2] bis 2026-08-22 als "Entry OPEN, toter Code". Das ist WIDERLEGT —
     * der Gang-Executor selbst committet den Zustand an drei Stellen (neu disassembliert aus
     * EMZ0.BIN, jede Zeile unten zitiert):
     *
     * (1) TIMER-UEBERGANG @0x80101D68-DBC (der Seed dazu steht in P0 @0x80101B10-20:
     *     `jal rand / sra v0,v0,3 / addiu v0,v0,100 / sh v0,346` = +0x15A = (rand>>3)+100):
     *       80101d68  lhu  v0,346(s1)                        ; +0x15A
     *       80101d70  addiu v0,v0,-1        80101d74  sh v0,346(s1)
     *       80101d78/7c sll v0,v0,16 / bne v0,zero,0x80101dc0 ; erst wenn (alt-1)&0xffff == 0
     *       80101d84  jal  rand   80101d8c sra v0,v0,3  80101d90 addiu v0,v0,100
     *       80101d98  sh   v0,346(s1)                        ; +0x15A neu
     *       80101d94  jal  rand                               ; ZWEITER Wurf
     *       80101d9c/a0 andi v0,v0,0x3 / bne                  ; 1 von 4
     *       80101da8/b0 lw v0,496 / sltiu v0,v0,0x1388        ; dist < 5000
     *       80101db8/bc addiu v0,zero,513 / sw v0,4(s1)       ; +0x4 = 0x201
     * (2) NAHBEREICH @0x80101EC4-F10:
     *       80101ecc  sltiu v0,v0,0x7d0                       ; dist < 2000
     *       80101ee8  jal 0x80015614 (a3 = 512) ; bne         ; Spieler im +-512-Kegel
     *       80101efc/04/08 lbu 0x800cfdcb / andi 0x80 / bne   ; Spieler nicht beansprucht
     *       80101f0c/10 addiu v0,zero,513 / sw v0,4(s1)       ; +0x4 = 0x201
     * (3) SPIELER RENNT @0x80101F14-F60:
     *       80101f18/20 lhu 0x800cfbf6 / andi 0x4             ; cfbf6 Bit 2 = RENNEN
     *       80101f34  sltiu v0,v0,0x9c4                       ; dist < 2500
     *       80101f50  jal 0x80015614 (a3 = 1324) ; bne        ; Spieler im +-1324-Kegel
     *       80101f5c/60 addiu v0,zero,513 / sw v0,4(s1)       ; +0x4 = 0x201
     *
     * WARUM SIE (NOCH) NICHT DRIN SIND — GEMESSEN, nicht vermutet: mit (1)+(2) scharf faellt die
     * POSITIV-KONTROLLE NAHKAMPF in test_re2_zombie_abc von 3523 auf 0 Treffer (Pistole bleibt
     * bei ~1700). Bisektion ueber die drei Bloecke einzeln: nur (1) an -> 3756, nur (2) an ->
     * 3589, (1)+(2) zusammen -> 0. Der Mechanismus dieser Kombination ist NICHT aufgeklaert,
     * und "Mechanismus nicht gefunden = NICHT fertig" gilt auch fuer eine byte-belegte
     * Ergaenzung: ein unverstandener Zusammenbruch des Nahkampfs waere schlimmer als die
     * Luecke. Der EXEC[2]-Executor selbst (unten) ist davon unabhaengig scharf — er wird ueber
     * die Treffer-Erholungs-Leitern (@0x801060F4 / @0x80105B24 / @0x80107418 / @0x801081BC)
     * erreicht, und genau dort trat der Nutzer-Befund auf.
     * NAECHSTER SCHRITT: den 0-Treffer-Zusammenbruch mit (1)+(2) aufklaeren (Verdacht: der
     * Anmarschweg endet ausserhalb des +-0x400-Spielerkegels, weil EXEC[2] nur mit +-16
     * steuert statt mit der Gang-Rate 8+16) — danach die drei Bloecke scharf schalten.
     * ======================================================================================*/
}

/* ============================================================================================
 * EXEC[2] @0x80102260 — DER ZWEITE GANG ("Arme oben"), NICHT ein Anstoss-Stub.
 *
 * ⛔ NUTZER-REPORT 2026-08-22 (fetter Zombie 0x11, ROOM1140, RE2-Modus):
 *   "Es gibt manchmal Momente, wo er kurz zu einer Animation mit GEHOBENEN ARMEN wechselt, und
 *    dann laeuft. Immer wenn diese Animation kommt, LAEUFT ER AUF DER STELLE, anstatt wirklich
 *    Richtung Spieler zu gehen."
 *
 * GEMESSEN VOR DEM FIX (probe_re2_fatz_walkspot, ROOM1140, echte Bank EM011, +0x4 = 0x201
 * gesetzt wie es die Original-Leitern tun):
 *   f0100 st=1 s1=2 s2=1 clip=4 af=  1 fc=60 LOCO=1 pos(-488,-21635) d(0,0)
 *   f0125 st=1 s1=2 s2=1 clip=4 af= 26 fc=60 LOCO=1 pos(-488,-21635) d(0,0)
 *   f0150 st=1 s1=2 s2=1 clip=4 af= 51 fc=60 LOCO=1 pos(-488,-21635) d(0,0)
 *   -> 59 Bilder Loco-Clip 4, Summe |dx|+|dz| = 0, danach zurueck in den Gang.
 * Und die "gehobenen Arme" sind MESSBAR, nicht behauptet — Mittelwert der Hand-Bone-Y ueber den
 * ganzen Clip (Y ist nach unten positiv, kleiner = hoeher), EM011-Loco-Bank:
 *   Clip 0/1 (EXEC[1]-Gang A): b11 -1609  b14 -1635
 *   Clip 2/3 (EXEC[1]-Gang B): b11 -1646  b14 -1526
 *   Clip 4/5 (EXEC[2]-Gang C): b11 -2084  b14 -1936     <- 440..475 Einheiten HOEHER
 *   Clip 6/7 (EXEC[2]-Gang D): b11 -2053  b14 -2114     <- 410..480 Einheiten HOEHER
 * EXEC[2] IST also genau der Gang mit den erhobenen Armen, und der Port hat ihn ohne
 * Fortbewegung gespielt: keine Steuerung, kein 0x80015E7C/0x800152C8.
 *
 * VOLLSTAENDIG NEU DISASSEMBLIERT 2026-08-22 (EMZ0.BIN, jede Zeile unten zitiert). Die alte
 * Fassung war ein 2-Phasen-Stub mit der Notiz "P1+ nicht RE'd; Entry OPEN, toter Code" — beides
 * ist widerlegt: der Zustand hat einen vollen Per-Tick-Body UND drei Produzenten im Gang selbst
 * (siehe re2z_exec_walk unten).
 *
 * P0 @0x80102290 (FAELLT DURCH nach P1 — kein Sprung hinter @0x80102338):
 *   80102290/94  lui a0,0xf / ori a0,a0,0x4      ; Wortbasis 0x000F0004
 *   80102298     lbu v0,536(s0)                  ; +0x218 walkclip
 *   8010229c/a0  addiu v1,1 / sb v1,6(s0)        ; +0x6 = 1
 *   801022a4/ac  addu v0,v0,a0 / sw v0,332(s0)   ; Clip-Wort = 0xF0004 + walkclip (Loco 4/6)
 *   801022a8-c0  jal rand ; andi 0x3f ; addiu 180 ; sh v0,344(s0)   ; +0x158 = (r&0x3f)+180
 *   801022b4/bc  lbu v1,569(s0) / bne v1,zero    ; nur wenn +0x239 == 0:
 *   801022c4-dc    jal rand ; andi 1 ; a0 = 11 : 10 ; jal 0x8005bd6c
 *   801022e4/e8    addiu v0,150 / sb v0,569(s0)  ; +0x239 = 150
 *   801022ec-fc  lw a1,264 / lw a2,380 / jal 0x80015e7c (a3=0)      ; PAIR-1-Re-Anker
 *   80102300-20  jal rand ; andi 0x1f ; sb v0,333(s0)               ; +0x14D = ZUFALLS-STARTBILD
 *   8010231c     jal 0x80015e7c (Pair 1) ERNEUT                     ; Re-Anker auf das neue Bild
 *   80102324-34  jal rand ; andi 0x1f ; addiu 45 ; sh v0,346(s0)    ; +0x15A = (r&0x1f)+45
 *   80102338     sb zero,362(s0)                                    ; +0x16A = 0
 * P1 @0x8010233c:
 *   8010233c-44  lb v0,362(s0) ; bne v0,zero                        ; +0x16A SIGNED gelesen
 *   8010234c-58    lh a1,452 / lh a2,454 / jal 0x80015558 (a3 = +16); Steuerung ZUM Zielpunkt
 *   8010235c       j 0x80102384
 *   80102364-70  else lh a1,452 / lh a2,454 / jal 0x80015558 (a3 = -16) ; Steuerung WEG (Weben)
 *   80102374-80       lbu v0,362 ; addiu -1 ; sb v0,362                 ; +0x16A--
 *   80102384-90  lw v0,272(s0) ; andi 1 ; beq                        ; +0x110 Bit 0 = Wandkontakt
 *   80102398-a8    lhu v1,346 ; addiu v0,v1,-1 ; bne v1,zero ; sh v0,346  ; +0x15A--
 *   801023ac-c0      jal rand ; andi 0x1f ; addiu 45 ; sh v0,346     ; +0x15A = (r&0x1f)+45
 *   801023bc-cc      jal rand ; andi 7 ; addiu 5 ; sb v0,362         ; +0x16A = (r&7)+5
 *   801023d4-e0  lw a1,264 / lw a2,380 / jal 0x80015e7c (a3=0)       ; PAIR-1-Wurzeldelta
 *   801023e4-f4  lw a1,264 / lw a2,380 / jal 0x8002959c (a3=256)     ; PAIR-1-Advance
 *   8010246c/70  jal 0x800152c8 (a1=0)                               ; ANWENDEN = die Bewegung
 *   80102550-60  lhu v1,344 ; addiu v0,v1,-1 ; beq v1,zero -> 0x8010257c ; sh v0,344
 *   80102564-74  lbu 0x800cfdcb ; andi 0x80 ; beq zero -> 0x80102584     ; Spieler beansprucht?
 *   8010257c/80  addiu v0,257 / sw v0,4(s0)                          ; +0x4 = 0x101 (zurueck)
 *   80102584-90  lw v0,496 ; sltiu 0x1389 ; beq -> 0x801025b8         ; dist < 5001 ?
 *   80102598-b0  jal 0x80015614 (a3=1024) ; beq v0,zero -> Epilog     ; im Kegel -> BLEIBEN
 *   801025b8-cc  lh v0,344 ; slti 120 ; beq -> Epilog ; sw 257,4      ; sonst ab 60 verbrauchten
 *                                                                      Bildern zurueck in den Gang
 * NICHT modelliert (benannt, nicht erfunden): der Drei-Bild-Zweig @0x801023F8-468
 * (+0x10E & 0x80 || +0x21A & 0x8000 -> 0x801016C8 + 0x8002A9C8) und die Kanten-Sturz-/
 * Jitter-Zweige @0x80102474-54C (+0x21A & 0x800 / & 0x1000) — dieselben Zweige, die auch der
 * Gang-Executor traegt und die in den ausgelieferten RE1.5-Raeumen kein Datum setzen. */
static void re2z_exec_bump(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                                     /* P0 @0x80102290 */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x8010229C-A0 */
        re2z_clip(e, 4 + e->re2z_walkclip, 0, 0xF, 0x100, 1);      /* Wort 0xF0004 + walkclip ->
                                                                    * Pair-1-Clip 4/6
                                                                    * @0x80102290-AC. LOOP: der
                                                                    * Aufrufer verwertet die
                                                                    * Advance-Rueckgabe NIE
                                                                    * (@0x801023F0 kein Test), der
                                                                    * Clip laeuft also bis der
                                                                    * Timer/die Leiter ihn beendet */
        e->re2z_t158 = (int16_t)((re2z_rand() & 0x3fu) + 180);     /* @0x801022A8-C0 (Draw IMMER) */
        if (e->re2z_cd239 == 0) {                                  /* @0x801022B4-BC */
            re2z_se((re2z_rand() & 1u) == 0u ? 11 : 10);           /* @0x801022C4-DC */
            e->re2z_cd239 = 150;                                   /* @0x801022E4-E8 */
        }
        /* @0x801022EC-FC + @0x8010231C: zwei 0x80015E7C-Re-Anker um das Zufalls-Startbild herum.
         * Der Port-Zwilling ist der Re-Anker in re15_re2z_move_root (root_prev_kf = -1), der
         * genau dieselbe Wirkung hat: das erste Bild nach dem Clip-Wechsel erzeugt keinen
         * Sprung. re2z_clip hat anim_frame schon auf 0 gesetzt, danach kommt das Zufallsbild. */
        e->anim_frame = (uint16_t)(re2z_rand() & 0x1fu);           /* +0x14D @0x80102300-20 */
        e->root_prev_kf = -1;                                      /* = der zweite e7c @0x8010231C */
        e->re2z_t15a = (int16_t)((re2z_rand() & 0x1fu) + 45);      /* @0x80102324-34 */
        e->re2z_dir16a = 0;                                        /* sb zero,362 @0x80102338 */
        /* FAELLT DURCH nach P1 — @0x80102338 hat keinen Sprung ueber @0x8010233c. */
    }
    /* ---- P1 @0x8010233c ------------------------------------------------------------------ */
    if ((int8_t)e->re2z_dir16a == 0) {                             /* lb 362 @0x8010233C-44 */
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, 16);     /* a3 = +16 @0x8010234C-58 */
    } else {
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, -16);    /* a3 = -16 @0x80102364-70 */
        e->re2z_dir16a = (uint8_t)(e->re2z_dir16a - 1u);           /* @0x80102374-80 */
    }
    if ((e->sca_wall_hit != 0)) {                                  /* +0x110 Bit 0 @0x80102384-90 */
        int16_t t = e->re2z_t15a;                                  /* @0x80102398-A8 */
        e->re2z_t15a = (int16_t)(t - 1);
        if (t == 0) {
            e->re2z_t15a  = (int16_t)((re2z_rand() & 0x1fu) + 45); /* @0x801023AC-C0 */
            e->re2z_dir16a = (uint8_t)((re2z_rand() & 7u) + 5u);   /* @0x801023BC-CC */
        }
    }
    re15_re2z_move_root(e);                                        /* e7c @0x801023DC + Advance
                                                                    * @0x801023F0 + 152c8
                                                                    * @0x8010246C — dasselbe
                                                                    * Bewegungs-PAAR wie im Gang */
    {   /* Ausstiegs-Leiter @0x80102550-CC */
        int16_t t = e->re2z_t158;                                  /* lhu 344 @0x80102550 */
        e->re2z_t158 = (int16_t)(t - 1);                           /* sh @0x80102560 */
        if (t == 0 || re15_player_is_grabbed()) {                  /* @0x8010255C / @0x80102564-74
                                                                    * (PL+0x1D3 Bit 0x80 = die
                                                                    * Greif-Klammer, s. oben) */
            re15_ai_set_state_word(e, 0x101);                      /* sw 257,4 @0x8010257C-80 */
            return;
        }
    }
    if (e->ai_dist < 0x1389u &&                                    /* sltiu 0x1389 @0x8010258C */
        re15_ai_arc_test(e, pl->x, pl->z, 1024) == 0)
        return;                                                    /* im Kegel -> bleiben @0x801025B0 */
    if (e->re2z_t158 < 120)                                        /* slti 120 @0x801025C0 */
        re15_ai_set_state_word(e, 0x101);                          /* sw 257,4 @0x801025C8-CC */
}

/* EXEC[3] @0x801025EC — THE GRAB. Phase table @0x8010001C (10 ptrs, byte-verified ==
 * 0x801026C0/27D8/2814/2838/2968/29A4/2BE8/2C30/2C60/2EB4). Player side = the port's victim/cmd
 * infra.
 * s5 = Kriech-Bit + 2*(Typ 0x11/0x17), byte-gelesen:
 *   8010266c: lhu  v0,270(s1)        ; +0x10E
 *   80102670: lbu  v1,8(s1)          ; +0x8 = TYP
 *   80102674: andi s5,v0,0x1         ; **s5 = Kriecher-Bit**
 *   8010267c: beq  v1,23 -> 0x80102690
 *   80102688: bne  v1,17 -> 0x80102694
 *   80102690: addiu s5,s5,2
 * Der Kriech-Zweig war frueher als "OPEN -> Bit 0" gefuehrt; mit dem Kriecher-Brain ist er scharf. */
static void re2z_exec_grab(re15_actor_t *e, re15_actor_t *pl)
{
    int s5 = (int)(e->re2z_f10e & 1u)                              /* andi s5,v0,0x1 @0x80102674 */
           + ((e->type == 0x11) ? 2 : 0);                          /* @0x80102678-90 (0x17 not in
                                                                    * the RE1.5 family) */
    if (e->sub_state_2 <= 3) re15_re2z_player_pin();               /* hold phases pin the player */
    switch (e->sub_state_2) {
    case 0: {                                                      /* P0 @0x801026C0 */
        e->re2z_grabclip = re2z_param_grab[s5 * 2];                /* lbu 16(sp+s5*2) @0x801026C4-CC */
        e->re2z_bitefr   = re2z_param_bite[s5 * 2];                /* sp+64 pair @0x801028A0-AC */
        e->re2z_bitedmg  = re2z_param_bite[s5 * 2 + 1];            /* @0x801028F4 */
        re2z_clip(e, e->re2z_grabclip, 0, 0xF, 0x100, 0);          /* sw (0xF<<16)|clip @0x801026D8-E0 */
        /* snap-turn onto the player: 0x80015558(PL.x,PL.z,rate 2048) @0x801026E4-F8 — with
         * clamp 0x800 the compare always snaps (same math as the RE1.5 grab [0] snap). */
        e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0x0fff);
        {   /* PL+0x4 = (FUN_80015910-dir << 8)|5 @0x80102714-28 -> the port victim variant;
             * MAPPING: the direction sub collapses onto the RE1.5 front/behind selector. */
            int behind = re15_ai_facing_aligned(e, pl);
            re15_re2z_victim_begin(e, pl, behind);                 /* PL+0x1B4=self @0x80102710,
                                                                    * claim @0x8010275C-60 */
        }
        re15_re2z_grab_anchor(e, pl, (int)e->re2z_grabclip);       /* pin helper 0x80015b94
                                                                    * @0x8010270C (one anchor) */
        /* ⛔ EIN-ANGREIFER-RIEGEL: der Griff BEANSPRUCHT den Spieler ueber PL+0x1D3 Bit 0x80.
         *   80102754: lbu v0,467(s3)      ; s3 = SPIELER (`sw s1,436(s3)` @0x80102710 = PL+0x1B4)
         *   8010275c: ori v0,v0,0x80
         *   80102760: sb  v0,467(s3)
         * Derselbe Riegel, den die Kriecher-Entscheider setzen (@0x80102FAC-C0 / @0x80102FF8 /
         * @0x80103B18-2C) und LESEN (@0x80102F4C-58 / @0x80103AD4-E0 / @0x8010455C-68). Der Port
         * bildete den Setzer bisher nur auf pl->hit_react |= 1 ab und hatte fuer das echte Feld
         * GAR KEINEN Loescher — GEMESSEN (probe_re2z_crawl_attack, ROOM1030, echter game_step,
         * geladene RE2-Baenke, zwei erzwungene Kriecher):
         *   f261 Slot 2 greift (+0x5 0->1), Riegel 0x00 -> 0x80
         *   f299..f1799 Riegel bleibt 0x80; Slot 1 kriecht die restlichen 1500 Frames in
         *                Reichweite (dist 909..918 << 0x514) und greift KEIN EINZIGES MAL an
         *   Ergebnis 1800 Frames: 2 Griff-Eintritte, danach nie wieder (nachher: 10).
         * Mit Setzer UND den drei byte-gelesenen Loeschern (Release-Exit @0x8010AEF4,
         * HURT @0x80104FA0, DEATH @0x801082E8) ist der Lebenszyklus geschlossen. */
        pl->re2z_self1d3 |= 0x80u;                                 /* ori 0x80 @0x80102754-60 */
        e->re2z_self1d3 = 15;                                      /* sb 15,467(s1) @0x8010276C-70 */
        e->ai_flags |= 1u;                                         /* port grab latch (domino infra) */
        e->grab_choreo = 1;                                        /* word0|=0x1000 @0x80102768-88 */
        if (e->re2z_cd239 == 0) {                                  /* @0x8010278C-94 */
            re2z_se((re2z_rand() & 1u) ? 10 : 11);                 /* @0x8010279C-B8 */
            e->re2z_cd239 = 150;                                   /* @0x801027BC-C0 */
        }
        /* rumble 0x800395B8(20,0,250,0) @0x801027C4-D0 — no rumble subsystem, documented. */
        e->sub_state_2 = 1;                                        /* sb 1,6(s1) @0x801026D4 */
        break;
    }
    case 1:                                                        /* P1 @0x801027D8: latch anim */
        re15_re2z_grab_rootmotion(e);                              /* pose 0x80015cb8 + advance */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* @0x80102800-10 */
        break;
    case 2:                                                        /* P2 @0x80102814 */
        re2z_clip(e, e->re2z_grabclip + 1, 0, 0, 0x100, 1);        /* bite = grab+1, PLAIN store
                                                                    * `addiu v1,1; sw` @0x80102830-34
                                                                    * -> Rate-Halbwort 0 = harter
                                                                    * Schnitt (Review #7) */
        e->re2z_t158 = 148;                                        /* struggle budget @0x80102828-2C */
        e->sub_state_2 = 3;                                        /* @0x80102820-24 */
        break;
    case 3: {                                                      /* P3 @0x80102838: bite loop */
        re15_re2z_grab_rootmotion(e);
        int mash = re15_re2z_mash();                               /* FUN_8001598C @0x80102860 */
        e->re2z_t158 = (int16_t)(e->re2z_t158 - (2 + 5 * mash));   /* -=2+5*mash @0x80102868-7C */
        if (e->re2z_t158 < 0)                                      /* bgez @0x80102884 ueberspringt
                                                                    * NUR die Phase-4-Writes — der
                                                                    * Ablauf-Tick faellt in den
                                                                    * Biss-Check DURCH (Review #6) */
            e->sub_state_2 = 4;                                    /* both sides @0x8010288C-94 */
        if (re2z_frame_slot(e) == (int)e->re2z_bitefr) {           /* +0x14D == pair[0] @0x801028A0-AC */
            re2z_se(3);                                            /* bite SE @0x801028E8-F0 */
            int r = re2z_player_damage(pl, (int)e->re2z_bitedmg);  /* FUN_800401d4 @0x801028F4-FC */
            if (r & 1) e->sub_state_2 = 4;                         /* one-save -> throw @0x80102904-1C */
            if (r & 2) {                                           /* player dead @0x80102920-50 */
                pl->state = 7;                                     /* grabbed death (port death FSM) */
                re15_re2z_victim_devour(e, re15_ai_facing_aligned(e, pl));
                                                                   /* Kill-Tick zieht FUN_80015910
                                                                    * NEU: PL-Cmd = (dir<<8)|6
                                                                    * @0x8010293C-50 — Variante
                                                                    * explizit (Review #17) */
                re15_ai_set_state_word(e, 0x601);                  /* sw 0x601 @0x80102924-38 */
            }
        }
        break;
    }
    case 4:                                                        /* P4 @0x80102968: throw-off */
        re2z_clip(e, e->re2z_grabclip + 2, 0, 7, 0x200, 0);        /* clip+2 rate 7 @0x8010297C-A0 */
        e->re2z_t158 = 0;                                          /* sh zero,344 @0x80102990 */
        e->re2z_dir16a = 0;                                        /* sb zero,362 @0x80102994 */
        e->ai_flags &= (uint16_t)~1u;                              /* port latch off (RE1.5 [4] twin) */
        re15_player_victim_throwoff();                             /* PL+0x6=4 shake-off (P3 wrote it
                                                                    * on BOTH sides @0x8010288C-94) */
        e->sub_state_2 = 5;                                        /* sb 5,6(s1) @0x8010298C */
        break;
    case 5: {                                                      /* P5 @0x801029A4: escape latch */
        if (!(e->re2z_flags21a & 0x60u) && re2z_frame_slot(e) == 3 /* @0x801029A4-C0 */
            && (s5 & 1) == 0) {                                    /* upright only @0x801029C8-CC */
            uint32_t r1 = re2z_rand(), r2 = re2z_rand();           /* @0x801029D4/DC */
            if (((r1 >> (r2 & 3u)) & 0xfu) == 0u) {                /* srav+andi @0x801029E4-F0 (1/16) */
                int dir = re15_ai_facing_aligned(e, pl) ? 1 : 0;   /* FUN_80015910 @0x80102A00 (MAPPING) */
                uint8_t d23d = (uint8_t)((unsigned)dir | (re2z_rand() & 2u)); /* @0x80102A08-1C */
                e->re2z_flags21a |= 0x40u;                         /* delay-slot sh @0x80102A28/34 */
                if (d23d & 2u)
                    e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x40u) | 0x20u); /* @0x80102A38-40 */
            }
        }
        re15_re2z_grab_rootmotion(e);                              /* pose+advance(512) @0x80102A50-68 */
        if (re2z_frame_slot(e) == 7) { e->sub_state_2 = 7; break; }/* upright cut @0x80102BD0-E4 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* @0x80102A6C-80 */
        if (e->sub_state_2 == 6) e->sub_state_2 = 7;               /* P6 @0x80102BE8 is the CRAWLER
                                                                    * exit (state 7 + HP -1); crawler
                                                                    * OPEN -> upright path only */
        break;
    }
    case 7:                                                        /* P7 @0x80102C30: double root */
        re15_re2z_move_root(e);                                    /* 2x 0x80015e7c @0x80102C3C/50
                                                                    * (delta form: one application
                                                                    * per rendered frame step) */
        e->sub_state_2 = 8;                                        /* sb 8,6(s1) @0x80102C58-5C */
        break;
    case 8: {                                                      /* P8 @0x80102C60: recover/fall */
        re15_re2z_move_root(e);                                    /* e7c @0x80102C6C fills +0x144 */
        re2z_thrust(e, -30);                                       /* +0x144.x -= 30 between e7c
                                                                    * and 152c8 (@0x80102CA0-AC):
                                                                    * throw recoil = clip delta
                                                                    * minus 30 backward per tick */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* advance @0x80102C94-A4 */
        int fell = 0;
        if (re2z_frame_slot(e) == 22) {                            /* @0x80102CB0-B8 */
            uint32_t r1 = re2z_rand(), r2 = re2z_rand();           /* @0x80102CD8/E0 */
            int fall = ((((r1 >> (r2 & 3u)) & 0xfu) ^ 2u) != 0u);  /* xori 2/sltu @0x80102CE8-D04
                                                                    * (15/16 -> falls backward; the
                                                                    * difficulty==3 branch @0x80102CC8
                                                                    * has no RE1.5 equivalent) */
            if (fall) {
                fell = 1;
                re15_ai_set_state_word(e, 0x501);                  /* sw 0x501 @0x80102D24-2C */
                e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu)); /* @0x80102D40-48 */
                e->re2z_dir16a = 1;                                /* sb 1,362 @0x80102D4C-50 */
                e->sub_state_2 = 1;                                /* sb 1,6 @0x80102D54 (skip P0 roll) */
                e->re2z_flag222 = 0;                               /* sb zero,546 @0x80102D90 */
                e->re2z_flags21a = (uint16_t)(((e->re2z_flags21a & ~0x8u) | 0x4u) | 0x202u);
                                                                   /* &=~0x8 @0x80102D60-64, |=0x4
                                                                    * @0x80102DA8-B0, |=0x202 @0x80102DBC */
                e->re2z_self1d3 |= 0x80u;                          /* ori 0x80 @0x80102D8C/DA0-A4 */
                e->re2z_f10e |= 0x2000u;                           /* +0x10E|=0x2000 @0x80102DA8/DB8-C0 */
                e->grid_id |= 0x80u;                               /* PORT-MAPPING (Review #18): der
                                                                    * flavor-blinde Damage-Resolver
                                                                    * klassifiziert ueber grid&0x80
                                                                    * (RE1.5-Zwilling @0x801022b8) */
                re2z_clip(e, 2, 0x14, 0xA, 0x100, 0);              /* word 0xA1402 @0x80102D68-7C:
                                                                    * clip 2 from frame 20, +0x8F = 10.
                                                                    * BLEND = a3 DES VERBRAUCHENDEN
                                                                    * ADVANCE, und das ist EXEC[5] P1
                                                                    * (`sb 1,6` @0x80102D54 ->
                                                                    * @0x80103370): `addiu a3,zero,256`
                                                                    * @0x80103380 = 0x100. Siehe den
                                                                    * Sammel-Beleg in re2z_exec_knockdown. */
                re2z_se((re2z_rand() & 1u) ? 12 : 13);             /* rand&1==0 -> 13, sonst 12
                                                                    * (@0x80102DC4-DC, Review #7) */
                e->re2z_t158 = 1;                                  /* sh 1,344 @0x80102DE4-EC */
            }
        }
        if (!fell && re2z_frame_slot(e) >= 7 && re2z_frame_slot(e) <= 24) {
            /* PARTNER-DOMINO (@0x80102DF0-EB0, selbst disassembliert — Review #1): waehrend der
             * Frames 7..24 (`addiu -7; sltiu 0x12` @0x80102DF8-FC) weckt der taumelnde Zombie
             * seinen PARTNER: Original via +0xD-Index in die Entity-Tabelle @0x800CFE14
             * (@0x80102E08-20; +0xD-Produzent nicht RE'd) -> PORT-MAPPING: der Body-Push-Kontakt
             * (contact_slot, +0x1AC-Analog). Gates byte-true: kind&0x10 (@0x80102E28-34),
             * HP>=0 (@0x80102E3C-44), !(+0x10E&1) Kriecher (@0x80102E4C-58), +0x1D3==0 ganzes
             * Byte (@0x80102E60-68), !(+0x21A&0x8) Aufsteh-Latch (@0x80102E70-7C),
             * !(+0x10E&0x2000) schon-gefallen (@0x80102E80-84). Aktion: Partner+0x4 = 0x901
             * (@0x80102E90-98), Partner+0x16B = FUN_80015910-Richtung (@0x80102E94-A8),
             * SE 4 (@0x80102E9C-A4 — HIER lebt der SE 4, nicht in P9). */
            int ps = (int)e->contact_slot;
            if (ps >= 1 && ps < RE15_ACTOR_MAX) {
                re15_actor_t *pz = &g_actors[ps];
                if (pz->active && re15_re2z_owns_type(pz->type)
                    && pz->hp >= 0
                    && !(pz->re2z_f10e & 0x1u)
                    && pz->re2z_self1d3 == 0
                    && !(pz->re2z_flags21a & 0x8u)
                    && !(pz->re2z_f10e & 0x2000u)) {
                    re15_ai_set_state_word(pz, 0x901);             /* sw 0x901 @0x80102E90-98 */
                    pz->re2z_gaitrow = (uint8_t)(re15_ai_facing_aligned(e, pz) ? 1 : 0);
                                                                   /* +0x16B = 15910-dir (MAPPING:
                                                                    * front/behind-Kollaps)
                                                                    * @0x80102E94-A8 */
                    re2z_se(4);                                    /* jal SE(4) @0x80102E9C-A4 */
                }
            }
        }
        break;
    }
    default:                                                       /* P9 @0x80102EB4-B8: exit */
        re15_ai_set_state_word(e, 0x1);                            /* `addiu v0,1; sw v0,4` ->
                                                                    * word 0x1 = STAND, KEIN SE
                                                                    * (Review #1) */
        break;
    }
}

/* EXEC[5] @0x80103188 — KNOCKDOWN-FALL (the "flinch" 0x501 commit falls the zombie). Own phase
 * table at param +0x6C (9 ptrs: 0x8010328C..0x801036F4). Phases the port models:
 *   P0 @0x8010328C: side = rand&1 -> +0x16A + orientation bit 0x4 (@0x8010329C-B4), fall clip
 *      param[0x44+side] (1/2) @0x801032C8-CC, +0x21A|=0x202, +0x222=0 @0x801032F4.
 *   P1..P5 (0x80103370..0x8010352C): fall plays out -> P6 (internals: FX/SE, unmodeled OPEN).
 *   P6 @0x80103568: phase=7 @0x80103580, +0x21A|=0x10 CRAWL MARKER @0x8010358C, ground-lie clip
 *      param[0x44+4+side] (8/9) rate 0xF @0x80103574-A8, 1/4 moan SE 11 cd 150 @0x801035F0-610.
 *   P7 @0x80103628: ground hold (shadow grower = render, skip). Duration: param u16 @0x8010004C
 *      = 90 (MAPPED as the lie time — P7's exact advance is not RE'd, OPEN).
 *   P8 @0x801036F4: commit 0x101 @0x801036F4-F8. */
/* ⛔ DER GEMEINSAME SCHWANZ DER STURZ-PHASEN @0x80103548 — DIE LIEGEZEIT.
 * Angesprungen von P3 (beide Zweige), P4/P5. Er zaehlt +0x15A herunter und schaltet erst bei
 * ABGELAUFENEM Timer auf Phase 6 (die Aufsteh-Kette). Selbst disassembliert:
 *   80103548: lhu  v1,346(s2)          ; +0x15A  (ALTER Wert)
 *   80103550: addiu v0,v1,-1
 *   80103554: bne  v1,zero,0x80103754  ; ALT != 0 -> nur dekrementieren und raus
 *   80103558: sh   v0,346(s2)          ; DELAY-SLOT: der Store laeuft in BEIDEN Faellen
 *   8010355c: addiu v0,zero,6
 *   80103564: sb   v0,6(s2)            ; ALT == 0 -> Phase 6
 * Das ist der ALT==0-Test mit Delay-Slot-Store, nicht NEU==0. */
static void re2z_kd_tail(re15_actor_t *e)
{
    uint16_t alt = (uint16_t)e->re2z_t15a;
    e->re2z_t15a = (int16_t)(alt - 1u);                            /* sh @0x80103558 (Delay-Slot) */
    if (alt == 0u) e->sub_state_2 = 6;                             /* @0x8010355C-64 */
}

static void re2z_exec_knockdown(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0: {
        int side = (int)(re2z_rand() & 1u);                        /* @0x8010328C-98 */
        e->re2z_dir16a = (uint8_t)side;
        e->re2z_flags21a = (uint16_t)(((e->re2z_flags21a & ~0x4u) | (side ? 0x4u : 0u)) | 0x202u);
        e->re2z_flag222 = 0;                                       /* @0x801032F4 */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80103304/330C */
        e->re2z_f10e |= 0x2000u;                                   /* +0x10E|=0x2000 @0x80103308/3320 */
        /* ⛔ BLEND-RATE = a3 DES ADVANCE, NICHT 0x200 (Nutzer-Report 2026-08-22: "Wenn der Zombie
         * zu Boden Richtung Leon springt, stimmt die Animation nicht ganz").
         * EXEC[5] advanced in JEDER seiner Phasen mit 256, selbst disassembliert aus
         * EMOVL10_S0.BIN:
         *   8010337c: jal 0x8002959c / 80103380: addiu a3,zero,256      (P1)
         *   801034f4: jal 0x8002959c / 801034f8: addiu a3,zero,256
         *   801036a0: jal 0x8002959c / 801036a4: addiu a3,zero,256
         * Der Port trug hier 0x200. WARUM DAS SICHTBAR IST — dieselbe Klemmfalle wie beim
         * RE1.5-Aufsteher (enemy_ai_common.c, Clip 0x29): das Gewicht der Vor-Pose ist
         * +0x8F * rate (FUN_8001f3bc @0x8001f474 `mult v0,a3`, IR0 = 0x1000 - Produkt
         * @0x8001f4a8-b0). Byte-true saettigt das Produkt NIE — 0xF*0x100 = 0xF00,
         * 7*0x200 = 0xE00, 3*0x400 = 0xC00, 0xA*0x100 = 0xA00, alle < 0x1000. Mit dem
         * uncitierten 0x200 ergab 0xF*0x200 = 0x1E00, was skeleton_common.c auf 0x1000 klemmt =
         * 100 % ALTE Pose fuer die ersten 8 Ticks des Sturzes: der Zombie steht acht Frames
         * eingefroren, BEVOR er faellt. Live gemessen am Feed-Aufsteher (Clip 0x15) im
         * Negativ-Test: +0x8F 15..11 => Produkt 0x1E00..0x1600, fuenf Frames Plateau. */
        re2z_clip(e, re2z_param_clips[side], side * 5 + 10, 0xF, 0x100, 0);
                                                                   /* fall clip 1/2, STARTFRAME
                                                                    * side*5+10 = 10/15 (`sll 2 +
                                                                    * addu + addiu 10 + sll 8`
                                                                    * @0x80103310-1C, sw @0x80103338
                                                                    * — Review #4); Blend = a3 = 256
                                                                    * @0x80103380 */
        if (e->re2z_cd239 == 0) {                                  /* @0x8010332C-34 (Review #5) */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* ==0 -> 13, !=0 -> 12
                                                                    * @0x8010333C-58 */
            e->re2z_cd239 = 150;                                   /* @0x8010335C-60 */
        }
        e->re2z_t158 = 0;                                          /* sh zero,344 @0x80103368 */
        e->grid_id |= 0x80u;                                       /* PORT-MAPPING (Review #18):
                                                                    * Downed-Band fuer den flavor-
                                                                    * blinden Damage-Resolver
                                                                    * (RE1.5-Zwilling-Set beim Fall) */
        e->sub_state_2 = 1;                                        /* @0x801032D0-D4 */
        break;
    }
    case 1:                                                        /* P1 @0x80103370: der Sturz
                                                                    * spielt ab */
        /* ⛔ 2026-08-21 KORRIGIERT (Nutzer-Report v0.3.5 "fallen komisch um und stehen dann
         * direkt wieder auf" + "Zombies sind unsterblich"). Hier stand `if (clip_done)
         * sub_state_2 = 6;` — ein PORT-MAPPING, das die Phasen 2..5 des Originals
         * UEBERSPRANG. Das Original zaehlt in P1 nur weiter:
         *   80103370-80: jal 0x8002959c (Advance, a3=256)
         *   80103384: lbu v1,6(s2)      ; +0x6
         *   8010338c: addu v1,v1,v0     ; += Advance-Rueckgabe
         *   80103398: sb  v1,6(s2)      ; -> PHASE 2, NICHT 6
         * und der Spieler-Claim faellt erst im AUFSCHLAG (P2 @0x80103470-90), nicht schon hier.
         * P1 ruft AUSSCHLIESSLICH den Advance — kein Steer, keine Wurzelbewegung. */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* +0x6 += ret @0x80103384-98 */
        break;
    case 2: {                                                      /* P2 @0x80103404: DER AUFSCHLAG */
        int side = (int)(e->re2z_dir16a & 1u);
        e->sub_state_2 = 3;                                        /* sb 3,6 @0x8010340C */
        /* Prone-Clip aus sp+18+(+0x16A) = param@0x80100046+side = {0x17,0x16}, Rate 0xF
         * (`lbu v0,18(v0)` @0x80103414, `lui v1,0xf` @0x80103418, `sw v0,332` @0x80103424). */
        re2z_clip(e, re2z_param_clips[2 + side], 0, 0xF, 0x100, 0);
        /* ---- DIE LIEGEZEIT: DREI Wuerfe, Zwei-Draw-Shift-Pick + Rest ----
         *   80103420: jal rand            ; r1
         *   80103428: jal rand            ; r2   (Delay: s0 = r1)
         *   80103430: jal rand            ; r3   (Delay: s1 = r2)
         *   80103438: andi s1,s1,0x3      ; r2 & 3
         *   8010343c: srav s0,s0,s1       ; r1 >> (r2 & 3)
         *   80103440/44: andi 0xf / sll 1 ; Index * 2
         *   80103450: lhu a0,16(s0)       ; = re2z_lie_tbl[idx]   (sp+32 == 0x8010004C)
         *   80103474: andi v0,v0,0xf      ; r3 & 0xf
         *   80103480/88: addu a0,a0,v0 / sh a0,346(s2)   ; +0x15A = tbl[idx] + (r3 & 0xf) */
        {   uint32_t r1 = re2z_rand(), r2 = re2z_rand(), r3 = re2z_rand();
            uint32_t idx = (r1 >> (r2 & 3u)) & 0xfu;
            e->re2z_t15a = (int16_t)(re2z_lie_tbl[idx] + (r3 & 0xfu));
        }
        e->re2z_t158    = 0;                                       /* sh zero,344 — Teil der
                                                                    * Hitbox-Reset-Gruppe
                                                                    * @0x80103478/7C (+0x9A/+0x9C) */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f @0x80103484-90 —
                                                                    * ERST HIER faellt der Claim:
                                                                    * genau deshalb ist ein
                                                                    * liegender RE2-Zombie wieder
                                                                    * treffbar (@0x80047138-40) */
        /* +0x144 = r4 & 3 (@0x8010348C `jal rand` / @0x80103498 `andi v0,v0,0x3` /
         * @0x801034A4 `sh v0,324(s2)` im Delay-Slot). Der Wert ist in P3 das TWITCH-Gate. */
        e->speed_h = (uint8_t)(re2z_rand() & 3u);
        /* Der Brand-Zweig `+0x10E & 0x80` (@0x8010349C-A0) halbiert die Liegezeit aus einem
         * ZWEITEN Zwei-Draw-Pick (@0x801034A8-D8) und nullt +0x144. */
        if (e->re2z_f10e & 0x80u) {
            uint32_t rA = re2z_rand(), rB = re2z_rand();
            e->speed_h = 0;                                 /* sh zero,324 @0x801034AC */
            e->re2z_t15a = (int16_t)(re2z_lie_tbl[(rA >> (rB & 3u)) & 0xfu] >> 1);
                                                                   /* srl 1 @0x801034D4, sh
                                                                    * @0x801034D8 */
        }
        /* Hitbox 200/200/-350/350 (@0x80103454-6C) = Praesentation/Kollision, kein Port-Zwilling. */
        break;
    }
    case 3:                                                        /* P3 @0x801034DC: Boden-Zucken */
        /* `lh v0,324(s2)` / `bne v0,zero,0x80103548` @0x801034DC-E4: bei +0x144 != 0 wird der
         * Clip NICHT fortgeschaltet (der Koerper liegt still), sonst advance + `+0x6 += ret`
         * (@0x801034F4-50C) -> P4. In BEIDEN Faellen laeuft danach der gemeinsame Schwanz. */
        if (e->speed_h == 0) {
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
        }
        re2z_kd_tail(e);                                           /* @0x80103548 */
        break;
    case 4:                                                        /* P4 @0x80103510 */
        e->re2z_t158   = (int16_t)((re2z_rand() & 0xfu) + 3u);     /* @0x80103510-20 */
        e->sub_state_2 = 5;                                        /* sb 5,6 @0x80103524-28 */
        /* FALLTHROUGH nach P5 @0x8010352C — im Original steht dort kein Sprung. */
        /* FALLTHRU */
    case 5:                                                        /* P5 @0x8010352C */
        /* `lhu v1,344` / `addiu v0,v1,-1` / `bne v1,zero,…` / `sh v0,344` IM DELAY-SLOT:
         * getestet wird der ALTE Wert (@0x8010352C-3C). Erst wenn +0x158 SCHON 0 war, geht es
         * zurueck auf Phase 3 (@0x80103540-44) — die Zuck-Schleife P3<->P4<->P5. */
        {   uint16_t alt = (uint16_t)e->re2z_t158;
            e->re2z_t158 = (int16_t)(alt - 1u);
            if (alt == 0u) e->sub_state_2 = 3;                     /* @0x80103540-44 */
        }
        re2z_kd_tail(e);                                           /* @0x80103548 */
        break;
    case 6:                                                        /* P6 @0x80103568 */
        e->re2z_flags21a |= 0x10u;                                 /* CRAWL MARKER @0x8010358C */
        re2z_clip(e, re2z_param_clips[4 + (e->re2z_dir16a & 1)], 0, 0xF, 0x100, 0); /* 8/9 rate 0xF
                                                                    * @0x80103574-A8; play-once —
                                                                    * P7 advanct DENSELBEN Clip zu
                                                                    * Ende (kein Loop-Flag) */
        if (e->re2z_cd239 == 0 && (re2z_rand() & 3u) == 0u) {      /* 1/4 moan @0x801035F0-FC */
            re2z_se(11); e->re2z_cd239 = 150;                      /* @0x80103604-10 */
        }
        /* +0x144 = 11 (@0x80103614-1C): dead store, see the re2z_thrust header */
        e->sub_state_2 = 7;                                        /* @0x8010357C-80 */
        break;
    case 7:                                                        /* P7 @0x80103628: Liege-Clip
                                                                    * laeuft aus (advance(256)
                                                                    * @0x801036A0-A4) — KEIN Timer
                                                                    * (der alte 90er war gemappt,
                                                                    * Review #6/[4]) */
        if (re2z_clip_done(e)) {
            e->re2z_flags21a &= (uint16_t)~0x10u;                  /* Kriech-Marker WEG @0x801036B8-BC */
            e->re2z_flags21a &= (uint16_t)~0x2u;                   /* @0x801036C8-CC */
            e->sub_state_2 = 8;                                    /* sb 8,6 @0x801036C4 */
        }
        break;
    default:                                                       /* P8 @0x801036F4 */
        /* ⛔ OPEN (Batch B1, Folge-RE — VISUELL, nicht im State-Log sichtbar): P8 committet
         * `addiu v0,zero,257` @0x801036F4 / `sw v0,4(s2)` @0x801036F8 = 0x101 DIREKT in den WALK —
         * der Zombie schnappt aus der Liege in den Gang, OHNE EXEC[9] Get-up (gemessen s1 5->1,
         * nie 9). Selbst nachdisassembliert und byte-true so uebernommen. Zu klaeren bleibt, ob der
         * Original-WALK-/STAND-Decide das Liege-Latch `+0x21A & 0x200` (gesetzt in P0
         * @0x801032F4) konsumiert und dann doch 0x901 einschiebt — dafuer decide[1] @0x80101714 auf
         * `lhu 538(s0)`-Reads scannen. Bis dahin KEIN Port-Eingriff (waere geraten). */
        re15_ai_set_state_word(e, 0x101);                          /* sw 0x101 @0x801036F4-F8 */
        /* P8 ist der EINZIGE Ausgang der Sturz-Kette und damit der Zwilling des RE1.5-Knockdown-
         * Ausgangs case 6, der DREI Dinge zusammen macht: `-> 0x201`, `grid_id &= 0x7f` (Downed-
         * Band weg) UND `hit_react &= ~1` (@0x80106b8c-90 / @0x80106a1c-20). Der Downed-Clear
         * stand hier schon (Review #18) — die +0x93-Freigabe fehlte, und OHNE sie bleibt jeder
         * per Flinch (0x501 @0x801050A4) niedergeschlagene Zombie fuer immer untreffbar, weil
         * der Flinch-Pfad keinen Reaktions-Handler-Endausgang durchlaeuft. */
        re2z_hit_latch_release(e);                                 /* +0x93 &= 0xfe @0x80105fa4 */
        /* Die beiden Aufraeum-Stores des Original-P8, die im Port fehlten (selbst
         * nachdisassembliert 2026-08-19, 0x801036F4..0x80103754):
         *   80103718: lbu v0,467(s2)   80103724: andi v0,v0,0x7f   80103728: sb v0,467(s2)
         *   8010373c: lhu v0,270(s2)   80103744: andi v0,v0,0xdfff 8010374c: sh v0,270(s2)
         * Ohne den ersten blieb +0x1D3 nach JEDEM Sturz auf 0x80 stehen — der Zombie waere mit
         * dem Original-Trefferfilter (@0x80047138-40) fuer immer untreffbar; der zweite nimmt
         * das in P0 gesetzte Bit 0x2000 (@0x80103308/3320) wieder zurueck. */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f  @0x80103718-28 */
        e->re2z_f10e    &= (uint16_t)~0x2000u;                     /* andi 0xdfff @0x8010373C-4C */
        e->grid_id &= (uint8_t)0x7Fu;                              /* PORT-MAPPING (Review #18):
                                                                    * Downed-Band-Clear beim
                                                                    * Aufstehen (RE1.5-Zwilling
                                                                    * @0x801022b8-bc) */
        break;
    }
}

/* EXEC[6] @0x80103954 — der Zombie beugt sich ueber den TOTEN SPIELER (Ziel des Grab-Commits
 * 0x601 @0x80102924-38).
 *
 * ⛔ 2026-08-21 KORRIGIERT. Die alte Fassung endete mit `if (clip_done) set_state_word(0x101)`
 * und zitierte dafuer "@0x80103B14" — diese Adresse liegt GAR NICHT in EXEC[6]: die Funktion
 * endet mit `jr ra` @0x80103A68, und 0x80103A70 ist bereits die decide[2]-Zeile des
 * KRIECHER-Brains (@0x8010C914). Ein Fehlzitat, und der 0x101-Ausgang war frei erfunden.
 * Folge im Port: +0x1D3 blieb nach dem Spielertod auf 0x80 stehen (P0 setzt es @0x801039F8-A04,
 * niemand loescht es) -> Gate (2) des Trefferfilters @0x80047138-40 -> der Zombie war fuer
 * immer untreffbar. Im Original passiert das nicht, weil P2 in den FRESSER geht, dessen P3 den
 * Claim wieder freigibt (`andi 0x7f` @0x80103CE4-FC).
 *
 * ORIGINAL (selbst disassembliert):
 *   Dispatch @0x80103970-A4: +0x6==1 -> P1 0x80103A08 | ==0 -> P0 0x801039B0 [Delay lui v0,0xf]
 *                            +0x6==2 -> P2 0x80103A44 [Delay-Slot **addiu v1,zero,2049 = 0x801**]
 *   P0 @0x801039B0: `ori v0,v0,0x18` + `sw v0,332` @0x801039C8 = Clip 0x18 Rate 0xF;
 *      `sb 1,6` @0x801039D4 (Delay-Slot von `jal 0x80015B94` = der Anker auf die Leiche);
 *      SE (rand&1)? 10 : 11 @0x801039D8-F4 (KEIN +0x239-Gate);
 *      +0x1D3 |= 0x80 @0x801039F8-A04.  **KEIN Sprung -> FALLTHROUGH nach P1.**
 *   P1 @0x80103A08: jal 0x80015CB8 (Pose) + jal 0x8002959C (a3=256);
 *      `+0x6 += Rueckgabe` @0x80103A30-40.
 *   P2 @0x80103A44: `lhu v0,270(s0)` / **`sw v1,4(s0)` mit v1 = 0x801** / `ori v0,v0,0x4000` /
 *      `sh v0,270(s0)` = Zustand 1 / +0x5 = 8 (FRESSEN) und +0x10E |= 0x4000 (das Limpet-Latch,
 *      das EXEC[8] P1 haelt). */
static void re2z_exec_six(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                                     /* P0 @0x801039B0 */
        re2z_clip(e, 0x18, 0, 0xF, 0x100, 0);                      /* sw 0xF0018,332 @0x801039C8;
                                                                    * Blend = a3 des einzigen Advance
                                                                    * dieser Funktion: `jal 0x8002959c`
                                                                    * @0x80103A28 / `addiu a3,zero,256`
                                                                    * @0x80103A2C (war 0x200 = Klemme,
                                                                    * s. re2z_exec_knockdown P0) */
        /* Anker 0x80015b94(PL, banks) @0x801039D0: richtet den Fresser am toten Spieler aus —
         * die Spieler-Seite haelt der Port ueber die Victim-FSM; OPEN, dokumentiert. */
        re2z_se((re2z_rand() & 1u) ? 10 : 11);                     /* rand&1==0 -> 11, sonst 10
                                                                    * (@0x801039D8-F0, KEIN cd-Gate) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x801039F8-A04 */
        e->sub_state_2 = 1;                                        /* @0x801039D4 */
        /* FALLTHROUGH nach P1 @0x80103A08 */
        /* FALLTHRU */
    }
    if (e->sub_state_2 == 1) {                                     /* P1 @0x80103A08 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* +0x6 += ret @0x80103A30-40 */
        return;
    }
    if (e->sub_state_2 == 2) {                                     /* P2 @0x80103A44 */
        re15_ai_set_state_word(e, 0x801);                          /* sw v1,4 mit v1=0x801 aus dem
                                                                    * Delay-Slot @0x801039A4 —
                                                                    * FRESSEN, nicht 0x101 */
        e->re2z_f10e |= 0x4000u;                                   /* ori 0x4000 @0x80103A4C-50 */
    }
}

/* EXEC[7] @0x80103780 — LYING SLEEPER (spawn pose; INIT commits 0x701). Phase table param+0x94.
 *   P0 @0x801037CC: clip 23, or 22 when +0x21A&0x4 (@0x801037D0-E4); +0x1C0|=1; phase 1;
 *      !(+0x21A&0x4000) -> phase 2 @0x80103824-34.
 *   P2 @0x80103838: ground idle clip 8 (9 when bit 0x4 @0x80103864-8C), rate 0xF, random frame.
 *   WAKE (phases 3/4 @0x801038D8/0x80103900): conditions not RE'd -> MAPPED to the RE1.5 lying
 *   wake (dist < 0xBB8 && player alive, the RE1.5 sleeping decide's own gate); target = the
 *   get-up commit 0x901 (EXEC[9] — the RE2 ground-rise executor). */
static void re2z_exec_lying(re15_actor_t *e, const re15_actor_t *pl)
{
    /* WAKE-Produzent (PORT-MAPPING, dokumentiert): das Original haelt den Liege-Spawn ueber das
     * Limpet-Latch +0x10E&0x4000 in P1 (@0x8010381C-28); der EINZIGE Overlay-Clear sitzt in
     * EXEC[15] @0x80104F0C (Bank-B-Kette, skript-/EXE-seitig angestossen — Produzent nicht RE'd).
     * Der Port ersetzt den fehlenden Skript-Wecker durch das RE1.5-Naehe-Gate und laesst danach
     * die byte-true Executor-Kette laufen.
     *
     * ⛔ NUTZER-REPORT v0.3.4+ GEFIXT: "Im Dining Room muss der Zombie, der gefressen wird, am
     * Boden auch bei RE2 AI und RE2 AI + Model am Boden liegen bleiben wie bei RE1.5 AI, und
     * nicht aufstehen."
     *
     * GEMESSEN (probe_lyer_1140, echte ROOM1140.RDT + raum-eigenes sub00 + ECHTE Baenke):
     *   RE1.5: slot 1 (Typ 0x16, Deskriptor 0x88) bleibt 900 Frames lang state=1/+0x5=0/+0x6=0/
     *          grid=0x88/Clip 0x13; hoechster Pose-Punkt -589 (= liegend), Spieler bis dist 649.
     *   RE2  : f0 INIT->0x701 (EXEC[7], +0x10E=0x4002) — f1 P1 liegt (Clip 0x17, Pose-Top -224) —
     *          **f44: ai_dist faellt erstmals unter 0xBB8 (gemessen 2996), die Zeile hier loescht
     *          Bit 0x4000 (+0x10E 0x4002 -> 0x0002)** — f45 P2/P3 Aufsteh-Clip 8 — f112 P4 —
     *          f113 `0x101` = ER STEHT (Pose-Top -2665). Der Ausloeser war also GENAU dieser
     *          port-erfundene Naehe-Wecker.
     *
     * DIE SOLLSEITE IST DER RE1.5-ZWILLING (RE2 kennt ROOM1140 nicht; der Liege-Zustand kommt
     * aus RE1.5-Spawndaten, also muss der RE2-Brain ihn genauso uebernehmen). Selbst
     * disassembliert aus info/Re1.5/PSX/BIN/STAGE1.BIN (roh @0x80100000):
     *   Dispatcher-Tabelle @0x8011F80C[grid & 0xf]:
     *       [5] = [6] = 0x801018F8   (Fresser)     [7] = [8] = 0x80101974   (LIEGEND)
     *       [9] = [10] = 0x801019F0  (skript-geweckt)
     *   Der Liege-Dispatcher 0x80101974 ist ein DOPPEL-Dispatch:
     *       80101990: lui   at,0x8012
     *       80101994: addiu at,at,-1576      ; at = 0x8011F9D8  = DECIDE-Basis
     *       80101998: addu  at,at,v0         ; v0 = (+0x5)*4
     *       8010199c: lw    v0,0(at)
     *       801019a4: jalr  v0               ; DECIDE
     *       801019c8: addiu at,at,-1580      ; at = 0x8011F9D4  = ANIMATE-Basis
     *       801019d8: jalr  v0               ; ANIMATE
     *   DECIDE-Zeile @0x8011F9D8[0] = 0x801039F4, und DAS ist ein LEERER STUB:
     *       801039f4: 03e00008  jr ra          (Rohbytes 08 00 e0 03)
     *       801039f8: 00000000  nop            (Rohbytes 00 00 00 00)
     *   => **Nibble 7/8 hat KEINEN Selbst-Wecker — weder nach Naehe noch nach Zeit.**
     *   Gegenprobe, dass die Nachbarzeilen ECHTE Funktionen sind (also kein Tabellen-Lesefehler):
     *       @0x8011F9D0[0] = 0x80103980 (Fresser 5/6) — dort steht der Naehe-Wecker:
     *            80103990: lw    v0,464(v1)      ; +0x1D0 = dist
     *            80103998: sltiu v0,v0,4000      ; < 0xFA0
     *            8010399c: beq   v0,zero,0x801039e4
     *            801039b8: sb    v0,6(v1)        ; +0x6 = 1  -> Aufsteh-Kette
     *       @0x8011F9DC[0] = 0x801039FC (skript-geweckt 9/10) = SOFORT-Wecker.
     *   Der Weckruf fuer 7/8 kommt im Original AUSSCHLIESSLICH vom SKRIPT, das den Deskriptor
     *   per `Member_set(12, 0x89/0x8A)` (`sb a2,9(a0)` @0x800411F8) auf Nibble 9/10 hebt — und
     *   genau dieser Weg ist im RE2-Flavor bereits als D15.2-Block in re15_re2z_tick verdrahtet.
     *   BYTE-ZENSUS ueber die ausgelieferten RDTs (Muster 0x34,0x0C,0x89|0x8A):
     *       ROOM1100 / ROOM1101 : je 5 Records (die "Leichen erwachen"-Kaskade)
     *       ROOM1070            : 5     ROOM1020 : 4
     *       ROOM1140            : **0**
     *   Der Dining-Room-Liegende hat also NIRGENDS einen Wecker — weder in sich (DECIDE-Stub)
     *   noch im Raumskript. Er liegt bis zum Raumwechsel. Genau das meldet der Nutzer.
     *
     * KONSEQUENZ: der ersetzte Naehe-Wecker gilt weiter fuer die Deskriptoren, deren RE1.5-
     * Zwilling einen Selbst-Wecker HAT (der Schlaefer sel 0x0E raeumt sein +0x9 im RE1.5-Live-
     * INIT auf 0 @0x80100FD4 und weckt bei dist < 0xBB8) — aber NICHT mehr fuer Nibble 7/8.
     * Der Deskriptor steht hier garantiert noch: 0x701 wird NUR vom INIT geschrieben (einziger
     * Produzent in dieser Datei), und P4 unten loescht grid_id erst BEIM Aufstehen. */
    if ((e->grid_id & 0x0fu) != 7u && (e->grid_id & 0x0fu) != 8u &&
        (e->re2z_f10e & 0x4000u) && e->ai_dist < 0xbb8u && pl->hp >= 0)
        e->re2z_f10e &= (uint16_t)~0x4000u;                        /* andi 0xbfff @0x80104F0C (MAPPED
                                                                    * hierher verlegt) */
    switch (e->sub_state_2) {
    case 0:                                                        /* P0 @0x801037CC */
        re2z_clip(e, (e->re2z_flags21a & 0x4u) ? 0x16 : 0x17, 0, 0, 0x100, 0);
                                                                   /* PLAIN store (`addiu 23/22; sw`
                                                                    * @0x801037CC-E4) -> Rate 0
                                                                    * (Review #7) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80103804-14 */
        e->sub_state_2 = 1;                                        /* @0x801037FC-800 */
        break;
    case 1:                                                        /* P1 @0x8010381C: Limpet-Halt */
        if (!(e->re2z_f10e & 0x4000u))                             /* lhu 270; andi 0x4000; bne
                                                                    * @0x8010381C-28 (+0x10E, NICHT
                                                                    * +0x21A — Review #15) */
            e->sub_state_2 = 2;                                    /* @0x8010382C-34 */
        break;
    case 2: {                                                      /* P2 @0x80103838 */
        int back = (int)((e->re2z_flags21a >> 2) & 1u);
        int fr = (int)(re2z_rand() & 0xfu);                        /* @0x80103838-4C */
        if (back) fr = (int)(re2z_rand() & 0xfu);                  /* ZWEITER Draw fuer Clip 9
                                                                    * (@0x80103864-78) */
        re2z_clip(e, re2z_param_clips[4 + back], fr, 0xF, 0x100, 0); /* 0xF0008/0xF0009 @0x80103840-80 */
        e->sub_state_2 = 3;                                        /* sb 3,6 @0x80103888-90 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80103884-8C */
            if ((re2z_rand() & 1u) != 0u) re2z_se(12);             /* @0x8010389C-AC */
            else if ((re2z_rand() & 1u) == 0u) re2z_se(10);        /* @0x801038B0-C0 */
            else re2z_se(11);                                      /* @0x801038C4-C8 */
            e->re2z_cd239 = 150;                                   /* @0x801038D0-D4 */
        }
        break;
    }
    case 3:                                                        /* P3 @0x801038D8: advance */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* @0x801038E4-FC */
        break;
    default:                                                       /* P4 @0x80103900 */
        re15_ai_set_state_word(e, 0x101);                          /* addiu 257; sw @0x80103900-0C */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f @0x80103914-18 */
        e->grid_id = 0;                                            /* PORT-MAPPING (Review #16): das
                                                                    * Liege-Nibble 0x88 muss beim
                                                                    * Aufstehen weg (RE1.5-Zwilling
                                                                    * enemy_ai_common.c:102), sonst
                                                                    * klassifiziert der flavor-blinde
                                                                    * Damage-Resolver den Stehenden
                                                                    * als DOWN = LEVEL-untreffbar */
        break;
    }
}

/* EXEC[8] @0x80103B74 — FEEDING (INIT spawns with 0x801 + clip 18 @0x80100AD4/DC). Feeding loop
 * clip rate 7 @0x80103C10; exit -> 0x101 exists @0x80103D94 (conditions not RE'd -> wake gate
 * MAPPED to the RE1.5 feeder wake dist < 0xFA0). Block K would enter phase 6 (eat the dead
 * player) — unreachable, see the ladder. */
static void re2z_exec_feeding(re15_actor_t *e, const re15_actor_t *pl)
{
    /* WAKE-Produzent: wie beim Lyer PORT-MAPPING (Limpet-Clear @0x80104F0C, Skript-Wecker fehlt)
     * mit dem RE1.5-Feeder-Gate; danach die byte-true Kette P1->P3->P4->P5. */
    if ((e->re2z_f10e & 0x4000u) && e->ai_dist < 0xfa0u && pl->hp >= 0)
        e->re2z_f10e &= (uint16_t)~0x4000u;
    switch (e->sub_state_2) {
    case 0:                                                        /* P0 @0x80103BE8 */
        re2z_clip(e, re2z_param_feed[re2z_rand() & 7u], 0, 7, 0x200, 0);
                                                                   /* Clip = rand&7 aus @0x801000A8
                                                                    * + 0x70000 (@0x80103BF4-C10 —
                                                                    * ROTATION 18/19/20, Review #8) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80103C04-14 */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80103BE8-F0 */
        break;
    case 1:                                                        /* P1 @0x80103C18 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e)); /* advance(512)
                                                                    * @0x80103C24-3C */
        /* Fress-Tropf-FX (word0&0x10000, FX 0x13D0 @0x80103C50-90): RE2-FX-System, OPEN. */
        if (!(e->re2z_f10e & 0x4000u))                             /* lhu 270; andi 0x4000
                                                                    * @0x80103C94-A0 */
            e->sub_state_2 = 3;                                    /* sb 3,6 @0x80103CA4-AC */
        break;
    case 2:                                                        /* P2 @0x80103CB0: Re-Draw */
        re2z_clip(e, re2z_param_feed[re2z_rand() & 7u], 3, 0, 0x100, 0);
                                                                   /* Wort = clip + 0x300 (Frame 3,
                                                                    * Rate 0) @0x80103CB8-D4 */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80103CC4-C8 */
        break;
    case 3:                                                        /* P3 @0x80103CD8: Aufstehen */
        re2z_clip(e, 0x15, 0, 0xF, 0x100, 0);                      /* sw 0xF0015 @0x80103CD8-EC;
                                                                    * Blend = a3 des P4-Advance
                                                                    * `addiu a3,zero,256` @0x80103D70
                                                                    * (war 0x200 = Klemme, s.
                                                                    * re2z_exec_knockdown P0) */
        e->sub_state_2 = 4;                                        /* sb 4,6 @0x80103CDC-E0 */
        e->re2z_self1d3 &= 0x7Fu;                                  /* andi 0x7f @0x80103CE4-FC */
        e->re2z_flags21a |= 0x10u;                                 /* ori 0x10 @0x80103D00-10 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80103D04-0C */
            if ((re2z_rand() & 1u) == 0u) {                        /* @0x80103D14-20 */
                if ((re2z_rand() & 1u) != 0u) re2z_se(11);         /* @0x80103D28-38 */
                else if ((re2z_rand() & 1u) != 0u) re2z_se(10);    /* @0x80103D3C-50 */
            }
            e->re2z_cd239 = 150;                                   /* @0x80103D58-5C */
        }
        break;
    case 4:                                                        /* P4 @0x80103D60 */
        if (re2z_clip_done(e)) {                                   /* advance(256) @0x80103D6C-74 */
            e->re2z_flags21a &= (uint16_t)~0x10u;                  /* andi 0xffef @0x80103D84-8C */
            e->sub_state_2 = 5;                                    /* sb 5,6 @0x80103D80 */
        }
        break;
    default:                                                       /* P5 @0x80103D90 */
        re15_ai_set_state_word(e, 0x101);                          /* addiu 257; sw @0x80103D90-94 */
        e->re2z_self1d3 &= 0x7Fu;                                  /* @0x80103D98.. */
        e->grid_id = 0;                                            /* PORT-MAPPING (Review #16) */
        break;
    }
    (void)pl;
}

/* ============================================================================================
 * EXEC[9] @0x80103E48 — DER STOSS-/TAUMEL-EXECUTOR (Ziel aller `sw 0x901`-Commits)
 * Vollstaendig selbst disassembliert 2026-08-21 (0x80103E48..0x80104170, EMOVL10_S0.BIN).
 *
 * ⛔ NAMENSKORREKTUR: hier stand "GET-UP from the ground". DAS IST WIDERLEGT — und war die
 * Ursache des Nutzer-Befunds "Nach dem Loesen des Raetsels im Generator-Raum steht der Zombie
 * abrupt, quasi ohne Animation, direkt vom Liegen zum Stehen" (Herleitung im D15.2-Block in
 * re15_re2z_tick). Belege, alle aus DIESER Funktion:
 *   - Die Clip-Tabelle ist @0x801000D8 = {3,3,4,0x0D}. In der Bank EM010 liegt die Brust in
 *     Frame 0 dieser Clips bei -2004 / -2528 / -2685 — also AUFRECHT. Der Boden-Aufsteher ist
 *     Clip 8/9 (Brust -150 / -129), und den spielt EXEC[7] P2 selbst (@0x80103840-80).
 *   - P0 setzt einen SCHUB `+0x144 = 400` (@0x80103F60-64) mit Yaw-Offset `+0x158 = +0x16B<<11`
 *     (@0x80103F78-80); P1 wendet ihn an (`jal 0x800152C8` @0x80103FBC) und baut ihn mit
 *     `addiu v0,v0,-30` (@0x80103FCC) ab.
 *   - Genau in dem Frame, in dem der Schub ausgelaufen ist (+0x15A == 1, @0x80103FF0-F8), faellt
 *     der Zombie mit 7/8 Wahrscheinlichkeit HIN: `s = (r1 >> (r2&3)) & 7` (@0x80104000-18),
 *     `s != 0 -> +0x4 = 0x501` (@0x80104020-28) = EXEC[5], der STURZ-Executor.
 * Das ist ein Stoss mit Taumel und wahrscheinlichem Umfallen, kein Aufstehen vom Boden.
 *
 * Aufbau (der Prolog kopiert ZWEI Tabellen auf den Stack):
 *   sp+16..19 = @0x801000D8 {3,3,4,0x0D}        (`lwl/lwr` @0x80103E74-84)  = re2z_param_getup
 *   sp+24..29 = @0x80100044 {1,2,0x17,0x16,8,9} (@0x80103E88-AC)            = re2z_param_clips
 *   Phasen ueber +0x6: ==1 -> 0x80103FB8, ==0 -> 0x80103EF0, ==2 -> 0x80104144
 *   (`lbu v1,6(s1)` @0x80103EB0, `beq v1,v0` @0x80103EB8, `beq v1,zero` @0x80103ECC,
 *    `beq v1,v0(=2)` @0x80103EE0).
 * ========================================================================================== */
static void re2z_exec_getup(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0: {                                                      /* P0 @0x80103EF0 */
        e->re2z_dir16a = (uint8_t)(re2z_rand() & 1u);              /* `jal 0x80015FE8` @0x80103EF0,
                                                                    * `andi v0,v0,0x1` @0x80103EF8,
                                                                    * `sb v0,362(s1)` @0x80103F00
                                                                    * (DELAY-SLOT = immer) */
        uint32_t rf = re2z_rand();                                 /* @0x80103EFC — Startframe */
        int row = (int)(int8_t)e->re2z_gaitrow;                    /* `lb v1,363(s1)` @0x80103F0C */
        int dir = (int)(int8_t)e->re2z_dir16a;                     /* `lb a0,362(s1)` @0x80103F10 */
        {   /* clip = sp16[row*2 + dir] (`sll v1,v1,1` @0x80103F14, `addu v1,v1,s0` @0x80103F18,
             * `addu v1,v1,a0` @0x80103F1C, `lbu v1,0(v1)` @0x80103F20); Wort = ((rf&3)<<8)|clip
             * mit RATE 0 (`sll v0,v0,8` @0x80103F08, `addu v0,v0,v1` @0x80103F28,
             * `sw v0,332(s1)` @0x80103F30). Blend = a3 = 256 des 959c @0x8010412C.
             * Der Index laeuft im Original ungeprueft ueber die 4-Byte-Tabelle hinaus; im Port
             * geklemmt (row ist in allen erreichbaren Pfaden 0 oder 1). */
            unsigned idx = (unsigned)(row * 2 + dir);
            int clip = (int)re2z_param_getup[(idx < 4u) ? idx : 0u];
            re2z_clip(e, clip, (int)(rf & 3u), 0, 0x100, 0);
        }
        if (row != 0 && dir != 0)                                  /* `beq a0,zero -> 0x80103F58`
                                                                    * @0x80103F2C / `beq v0,zero ->
                                                                    * 0x80103F5C` @0x80103F3C */
            e->anim_frame = (uint16_t)((re2z_rand() & 3u) + 20u);  /* `+0x14D = (rand&3)+20`
                                                                    * @0x80103F44-54 */
        e->sub_state_2 = 1;                                        /* `sb v0(=1),6` @0x80103F5C */
        e->speed_h     = 400;                                      /* `+0x144 = 400` @0x80103F60-64 */
        /* `+0x148 = 0` @0x80103F70 — der Port fuehrt nur die X-Komponente (s. re2z_thrust). */
        e->re2z_t15a   = 0;                                        /* `sh zero,346` @0x80103F74 */
        e->re2z_t158   = (int16_t)(row << 11);                     /* `sll v0,v0,11` @0x80103F78 /
                                                                    * `sh v0,344` @0x80103F80 */
        e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x10u) | 0x8u);
                                                                   /* `andi 0xffef` @0x80103F7C /
                                                                    * `ori 0x8` @0x80103F88 /
                                                                    * `sh v1,538` @0x80103F90 —
                                                                    * Partner-Domino-Gate 7 */
        if (e->re2z_cd239 == 0 && (re2z_rand() & 1u) != 0u) {      /* `lbu v0,569 / bne`
                                                                    * @0x80103F84-8C + `andi 0x1 /
                                                                    * beq` @0x80103F94-A0 */
            re2z_se(12);                                           /* @0x80103FA8 */
            e->re2z_cd239 = 150;                                   /* @0x80103FB0-B4 */
        }
        /* FALLTHROUGH nach P1 @0x80103FB8 — P0 endet OHNE Sprung. */
    }
    /* fall through */
    case 1: {                                                      /* P1 @0x80103FB8 */
        re2z_thrust_yaw(e, (int)e->speed_h, (int)e->re2z_t158);    /* `lh a1,344` @0x80103FB8 /
                                                                    * `jal 0x800152C8` @0x80103FBC */
        {   int v = (int)(uint16_t)e->speed_h - 30;                /* `lhu 324 / addiu -30`
                                                                    * @0x80103FC4-CC */
            e->speed_h = (int16_t)v;                               /* `sh v0,324` @0x80103FD0 */
            if ((int16_t)v < 0) {                                  /* `sll 16 / bgez` @0x80103FD4-D8 */
                e->speed_h   = 0;                                  /* `sh zero,324` @0x80103FE4 */
                e->re2z_t15a = (int16_t)(e->re2z_t15a + 1);        /* `+0x15A += 1` @0x80103FE0-EC */
            }
        }
        if (e->re2z_t15a == 1) {                                   /* `lh v1,346 / bne v1,1 ->
                                                                    * 0x80104120` @0x80103FF0-F8 —
                                                                    * NUR im Auslauf-Frame */
            uint32_t r1 = re2z_rand();                             /* @0x80104000 */
            uint32_t r2 = re2z_rand();                             /* @0x80104008 (Delay: s0 = r1) */
            unsigned s  = (unsigned)((r1 >> (r2 & 3u)) & 7u);      /* `andi v0,0x3` @0x80104010,
                                                                    * `srav s0,s0,v0` @0x80104014
                                                                    * (Rohwort 0x00508007),
                                                                    * `andi s0,0x7` @0x80104018 */
            if (s != 0u) {                                         /* `beq s0,zero -> 0x8010411C`
                                                                    * @0x8010401C — 7 von 8 */
                int fdir;
                re15_ai_set_state_word(e, 0x501);                  /* `addiu v0,zero,1281`
                                                                    * @0x80104020 / `sw v0,4(s1)`
                                                                    * @0x80104028 (DELAY-SLOT) */
                e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));
                                                                   /* `+0x223 = (rand&0xf)+16`
                                                                    * @0x80104024-34 */
                e->re2z_flags21a &= (uint16_t)~0x8u;               /* `andi 0xfff7` @0x80104040 /
                                                                    * `sh` @0x80104044 */
                e->re2z_dir16a = e->re2z_gaitrow;                  /* `sb v1,362(s1)` @0x8010404C */
                fdir = (int)(int8_t)e->re2z_dir16a;                /* `lb v1,362` @0x80104050 */
                e->re2z_flags21a &= (uint16_t)~0x4u;               /* `andi 0xfffb` @0x80104054 /
                                                                    * `sh` @0x8010405C */
                if (fdir != 0) e->re2z_flags21a |= 0x4u;           /* `ori 0x4` @0x80104060 /
                                                                    * `sh` @0x80104064 */
                e->sub_state_2   = 1;                              /* `sb v0(=1),6` @0x80104084 —
                                                                    * EXEC[5] startet in P1 */
                e->re2z_flags21a |= 0x202u;                        /* `ori 0x202` @0x80104090 /
                                                                    * `sh` @0x80104094 */
                e->re2z_flag222  = 0;                              /* `sb zero,546` @0x801040A4 */
                e->re2z_gaitrow  = 0;                              /* `sb zero,363` @0x801040A8 */
                e->re2z_self1d3 |= 0x80u;                          /* `ori 0x80` @0x801040B4 /
                                                                    * `sb v0,467` @0x801040BC */
                e->re2z_f10e    |= 0x2000u;                        /* `ori 0x2000` @0x801040B8 /
                                                                    * `sh v1,270` @0x801040D0 */
                /* word0 = (word0 & 0xF3FFFFFF) | 0x04000000 @0x80104068-AC — dieselbe Modell-/
                 * Kollisions-Gruppe wie in re2z_exec_knockdown P0 und im Kriecher-Eintritt; im
                 * Port ohne Feld (OPEN, mit Adresse). */
                e->grid_id |= 0x80u;                               /* PORT-MAPPING wie in
                                                                    * re2z_exec_knockdown P0
                                                                    * (Review #18): Downed-Band
                                                                    * fuer den flavor-blinden
                                                                    * Damage-Resolver — hier werden
                                                                    * dieselben Bits gesetzt
                                                                    * (+0x10E 0x2000, +0x21A 0x202,
                                                                    * +0x1D3 0x80) */
                re2z_clip(e, (int)re2z_param_clips[fdir & 1], fdir * 5 + 10, 0xF, 0x100, 0);
                                                                   /* `lbu a1,24(v0)` @0x8010407C
                                                                    * = sp24[+0x16A] = Sturzclip
                                                                    * 1/2; Startframe
                                                                    * `((dir*5)+10)<<8`
                                                                    * @0x801040C0-CC; Rate 15
                                                                    * (`lui v1,0xf` @0x801040D4);
                                                                    * `sw v0,332` @0x801040E8 */
                if (e->re2z_cd239 == 0) {                          /* `lbu v1,569 / bne`
                                                                    * @0x801040DC-E4 */
                    re2z_se((re2z_rand() & 1u) ? 12 : 13);         /* `andi 0x1 / beq -> a0 = 13`
                                                                    * (DELAY-SLOT @0x801040FC),
                                                                    * sonst a0 = 12 @0x80104100 */
                    e->re2z_cd239 = 150;                           /* @0x8010410C-10 */
                }
                e->re2z_t158 = 0;                                  /* `sh zero,344` @0x80104118
                                                                    * (DELAY-SLOT des `j` = immer) */
                return;                                            /* j 0x80104154 */
            }
        }
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* ADVANCE @0x80104128 (a1/a2 =
                                                                    * das DISPATCHER-Paar, a3=256),
                                                                    * `+0x6 += ret` @0x80104130-40 */
        break;
    }
    default:                                                       /* P2 @0x80104144 */
        re15_ai_set_state_word(e, 0x101);                          /* v1 = 257 aus dem DELAY-SLOT
                                                                    * @0x80103EE4, `sw v1,4(s1)`
                                                                    * @0x80104148 */
        e->re2z_flags21a &= (uint16_t)~0x8u;                       /* `andi 0xfff7` @0x8010414C /
                                                                    * `sh v0,538` @0x80104150 */
        e->grid_id = 0;                                            /* PORT-MAPPING (Review #16/#18):
                                                                    * Liege-Nibble + Downed-Bit weg,
                                                                    * sobald er wieder steht */
        break;
    }
}

/* ============================================================================================
 * EXEC[11] @0x8010439C — DER ZUSAMMENBRUCH ("auf die Knie gehen").
 *
 * ⛔ NUTZER-BLOCKER v0.3.5, ALLE DREI SYMPTOME HABEN HIER IHRE GEMEINSAME WURZEL:
 *   (A) "Die Zombies fallen manchmal komisch um und stehen dann direkt wieder auf."
 *   (B) "Manchmal haben sie eine Laufanimation, aber laufen nicht."
 *   (C) "Es kommt vor, dass die Zombies unsterblich sind."
 *
 * GEMESSEN (probe_re2z_abc, Seed-Sweep, ROOM1140, echter game_step + Pad, RE2-Bank EM010
 * geladen; seed 7, slot 2, Pistole):
 *   f140  st=2 s1= 3 s2=2                                  (Stagger-P2)
 *   f141  st=1 s1=11 s2=0                                  <-- 0xB01, EXEC[11] LAEUFT
 *   f142  st=1 s1=11 s2=1  1D3=80 10E=2004  +0x93=01       <-- Trefferfilter sperrt
 *   f191  st=1 s1= 1 s2=0  1D3=80 10E=2004  +0x93=01
 *   f192  st=1 s1= 1 s2=1  1D3=80 10E=2004  +0x93=01       <-- LAEUFT, DAUERHAFT UNTREFFBAR
 * Ueber 64 Seeds x 900 Frames x 4 Waffen tritt das in jedem Waffen-Lauf mehrfach auf.
 *
 * ⛔ SELBSTKORREKTUR AN DER ALTEN FASSUNG: die trug den Kommentar "ENTRY OPEN (no port
 * producer); executor implemented for completeness" und einen FREI ERFUNDENEN Ausgang
 * `if (clip_done) set_state_word(0x101)` ("Exit MAPPED"). Beides ist falsch:
 *   - ENTRY: der Port committet 0xB01 an DREI Stellen (re2z_hit_stagger @0x80106098,
 *     re2z_hit_main @0x80105ACC, die Death-Wurzel @0x80108224) — eigener Byte-Scan des Overlays
 *     nach `addiu/ori rt,zero,0xB01` findet genau diese drei Adressen. Der Executor ist
 *     der meistbenutzte Reaktions-Ausgang, kein toter Code.
 *   - EXIT: das Original geht NIE nach 0x101 zurueck. Es hat DREI Phasen.
 *
 * ---- ORIGINAL, selbst disassembliert (EMOVL10_S0.BIN) --------------------------------------
 * Dispatch @0x801043B8-F4:
 *   801043b8: lbu v1,6(s0)             ; +0x6
 *   801043c0: beq v1,1  -> 0x8010449C  ; P1
 *   801043d4: beq v1,zero-> 0x801043F8 ; P0     [Delay-Slot: lui v0,0xf]
 *   801043e8: beq v1,2  -> 0x801045D4  ; P2     [Delay-Slot: **lui v0,0x2**]
 *   801043f0: j 0x801045F0             ; sonst Epilog
 * P0 @0x801043F8: Clip-Wort 0xF000A; steer(+0x1C4/+0x1C6, 128) @0x80104400-14; `sb 1,6`
 *   @0x80104418; +0x158=0 @0x8010442C; +0x222=0 @0x80104430; +0x16B=0 @0x80104434;
 *   +0x21A &= ~4 dann |= 2 @0x80104438-48; word0 &= 0xF3FFFFFF dann |= 0x04000000
 *   @0x8010441C/28/40/44/50/54; +0x1D3 |= 0x80 @0x8010444C-5C; +0x10E |= 0x2000 @0x80104460-70;
 *   SE (rand&1)?10:11 nur bei +0x239==0, danach +0x239=150 @0x80104464-98.
 *   **KEIN Sprung am Ende — P0 FAELLT DURCH nach P1.**
 * P1 @0x8010449C:
 *   8010449c/a4/a8: +0x158 += 1                       (der NEUE Wert wird getestet)
 *   801044ac/b0   : (+0x158 & 1) == 0 -> 0x801044D0
 *   801044c0      : (ungerade) jal 0x80015E7C         ; Wurzel-Delta -> +0x144
 *   801044d0-e0   : (gerade)  +0x144 = (s16)+0x144 >> 2
 *   801044ec      : jal 0x8002959C (a3=256)
 *   801044f8-508  : **+0x6 += Advance-Rueckgabe**     ; -> P2, wenn der Clip durch ist
 *   80104504      : jal 0x800152C8                    ; +0x144 anwenden
 *   8010450c/14/18: +0x14D < 18 -> 0x801045BC = steer(+0x1C4/+0x1C6, 16), return
 *   ab +0x14D >= 18 der KRIECHER-TEST (siehe unten)
 * P2 @0x801045D4:
 *   801045d4/dc   : **+0x4 = 0x00020501** (lui 0x2 aus dem Delay-Slot @0x801043EC + ori 0x501)
 *                   = Zustand 1 / +0x5 = 5 / **+0x6 = 2** -> EXEC[5] PHASE 2, der AUFSCHLAG
 *   801045d8-e8   : +0x223 = (rand & 0xf) + 16
 *   801045ec      : +0x16A = 0
 *
 * ---- WARUM DAS ALLE DREI SYMPTOME ERKLAERT ------------------------------------------------
 * (C) P0 setzt +0x1D3 |= 0x80 und +0x10E |= 0x2000. Beide werden AUSSCHLIESSLICH am Ende der
 *     Sturzkette geloescht (EXEC[5] P2 @0x80103484-90 bzw. P8 @0x80103718-28 / @0x8010373C-4C).
 *     Der erfundene 0x101-Ausgang liess den Zombie an dieser Kette VORBEI in den WALK — mit
 *     gesetztem +0x1D3, und damit fuer immer hinter Gate (2) des Trefferfilters
 *     (`lbu v0,467(s0)` / `bne v0,zero` @0x80047138-40).
 * (A) Statt der Bodenkette (Aufschlag-Clip, Liegezeit, Aufstehen) ging es sofort in den Gang.
 * (B) Clip 0x0A laeuft ~50 Frames; P1 bewegt nur jeden ZWEITEN Tick per Wurzel-Delta. Danach
 *     kam im Port sofort der WALK-Clip — Laufanimation, waehrend die Fortbewegung noch aus der
 *     abklingenden Zusammenbruch-Phase stammte.
 *
 * ⛔ OFFEN, mit Adressen benannt (NICHT geraten, NICHT genommen): der KRIECHER-TEST in P1.
 *   80104530: jal 0x80015614(PL.x,PL.z,256)   ; Peilung
 *   8010453c: bne v0,zero -> Epilog
 *   80104544/4c: +0x1F0 < 0x708
 *   8010455c/64/68: 0x800CFDCB & 0x80  -> Epilog        (Einmal-Riegel PRO RAUM)
 *   80104570-80: +0x106 == 0x800CFCFE                   (gleiche Etage)
 *   80104584/8c: +0x4 = 0x101
 *   80104590-98: **+0x10E = (+0x10E & ~0x3F) | 1**      = die KRIECHER-WURZEL
 *   8010459c-b0: 0x800CFDCB |= 0x80
 * Bit 0 von +0x10E schaltet die GANZE Zustand-1-Wurzel um:
 *   80101154: lhu v0,270(a0) / 8010115c: andi v0,v0,0x3f / 80101160: sll v0,v0,2
 *   8010116c: lw v0,-14252(at)   ; Tabelle @0x8010C854
 *   80101174: jalr v0
 *   @0x8010C854 alterniert: GERADE -> 0x8010118C (aufrecht, decide @0x8010C88C / exec
 *   @0x8010C8CC), UNGERADE -> 0x80101210 (KRIECHER, decide @0x8010C90C / exec @0x8010C918 =
 *   {0x80103024 Kriechen, 0x801025EC GRAB (geteilt), 0x80103B48}).
 * Der Port hat WEDER einen Kriecher-Brain NOCH einen Produzenten fuer das Raum-Global
 * 0x800CFDCB. Den Zweig zu nehmen wuerde bedeuten, den Zombie mit +0x10E&1 in die AUFRECHTE
 * Wurzel laufen zu lassen (der Port ignoriert den Selektor) — also einen kriechenden Zombie
 * aufrecht gehen zu lassen. Deshalb wird der Zweig NICHT genommen; der Port faehrt konsequent
 * den Fall "Test schlaegt fehl", und das ist ein echter Original-Pfad (P1 -> P2). Der
 * Kriecher-Brain bleibt als eigener Auftrag offen.
 * ========================================================================================== */
/* DER EINTRITT IN DEN KRIECHER — die drei Stores, die das Original an seiner Kampf-Umwandlung
 * @0x80104584-0x801045B0 IMMER zusammen ausfuehrt:
 *   80104584: addiu v1,zero,257     /  8010458c: sw v1,4(s0)       ; +0x4 = 0x101
 *   80104588: lhu v0,270(s0)
 *   80104590: andi v0,v0,0xffc0     ; (die EINZIGE 0xffc0-Maske im ganzen Overlay)
 *   80104594: ori  v0,v0,0x1
 *   80104598: sh   v0,270(s0)       ; +0x10E = (+0x10E & ~0x3F) | 1
 *   8010459c-b0: Spieler+0x1D3 |= 0x80
 * `sub` waehlt den Kriecher-Substate: 1 = der Kampf-Eintritt des Originals (EXEC[1] = der
 * GRIFF), 0 = die LOKOMOTION. Auch 0 ist byte-gelesen: der Kriecher-HURT verlaesst sich mit
 * genau dieser Kombination (`sh 1,270` @0x80107A54 + `sw 1,4` @0x80107A58) in die Lokomotion.
 *
 * ⛔ +0x1D7 = 8 IST EINE BRUECKE, KEIN RE2-WERT. Voll-Scan von EMOVL10_S0.BIN nach jedem
 * Zugriff auf +0x1D7 (`(sb|lb|lbu) rt,471(rs)`): **NULL TREFFER** — das RE2-Zombie-Overlay
 * kennt das Feld nicht. Die SCA-Maske ist RE1.5-RAUMDATEN-Eigentum: der Port laedt unter BEIDEN
 * Flavors dieselbe RE1.5-ROOM1030.RDT, und deren Torzelle laesst nur die Maske 8 durch
 * (Port-Konsument enemy_ai_common.c: `re15_collision_constrain_enemy(..., e->sca_mask)`,
 * byte-true Zwilling @0x80100624). Der Wert 8 stammt aus der RE1.5-Kriechkette
 * (@0x801050F4 / @0x8010374C), 4 ist die aufrechte Zeile (@0x801050B4). Ohne diese Uebernahme
 * bliebe der RE2-Kriecher an derselben Zelle haengen wie ein aufrecht Stehender. */
void re15_re2z_enter_crawler(re15_actor_t *e, re15_actor_t *pl, unsigned sub)
{
    if (!e) return;
    re15_ai_set_state_word(e, 0x1u | ((sub & 0xffu) << 8));        /* sw 0x101 @0x8010458C bzw.
                                                                    * sw 1 @0x80107A58 */
    e->re2z_f10e = (uint16_t)((e->re2z_f10e & ~0x3fu) | 1u);       /* andi 0xffc0 / ori 1 / sh
                                                                    * @0x80104590-98 */
    e->sca_mask = 8;                                               /* BRUECKE, s. Block oben:
                                                                    * RE1.5-Torzelle @0x801050F4 */
    if (pl) pl->re2z_self1d3 |= 0x80u;                             /* @0x8010459C-B0 */
}

static void re2z_exec_eleven(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                                     /* P0 @0x801043F8 */
        re2z_clip(e, 0x0A, 0, 0xF, 0x100, 0);                      /* Wort 0xF000A: lui 0xf im
                                                                    * P0-Delay-Slot @0x801043D8,
                                                                    * ori 0xa @0x801043F8, sw
                                                                    * @0x8010440C; Blend = a3 des
                                                                    * P1-Advance `addiu a3,zero,256`
                                                                    * @0x801044F0 (war 0x200 = Klemme,
                                                                    * s. re2z_exec_knockdown P0) */
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, 128);    /* 0x80015558(+0x1C4/6, 128)
                                                                    * @0x80104400-14 */
        e->re2z_t158 = 0;                                          /* sh zero,344 @0x8010442C */
        e->re2z_flag222 = 0;                                       /* sb zero,546 @0x80104430 */
        e->re2z_gaitrow = 0;                                       /* sb zero,363 @0x80104434 */
        e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x4u) | 0x2u);
                                                                   /* andi 0xfffb + ori 0x2
                                                                    * @0x80104438-48 */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x8010444C-5C */
        e->re2z_f10e |= 0x2000u;                                   /* @0x80104460-70 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80104464-6C */
            re2z_se((re2z_rand() & 1u) == 0u ? 11 : 10);           /* @0x80104474-90 */
            e->re2z_cd239 = 150;                                   /* @0x80104494-98 */
        }
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80104410-18 */
        /* FALLTHROUGH nach P1 @0x8010449C — das Original hat hier KEINEN Sprung, P1 laeuft im
         * SELBEN Tick. (Wiederkehrender Fehler dieser Kampagne: Original-Fallthroughs zu
         * else-if begradigt.) */
        /* FALLTHRU */
    }
    if (e->sub_state_2 == 1) {                                     /* P1 @0x8010449C */
        e->re2z_t158 = (int16_t)(e->re2z_t158 + 1);                /* +0x158 += 1 @0x801044A4-A8 */
        if ((e->re2z_t158 & 1) != 0) {                             /* NEUER Wert & 1 @0x801044AC-B0 */
            re15_re2z_move_root(e);                                /* jal 0x80015E7C @0x801044C0 +
                                                                    * jal 0x800152C8 @0x80104504 =
                                                                    * das Bewegungs-PAAR */
        }
        /* ⛔ OPEN (benannt, nicht gefuellt): auf den GERADEN Ticks ruft das Original kein
         * 0x80015E7C, sondern `+0x144 = (s16)+0x144 >> 2` (@0x801044D0-E0) und laesst
         * 0x800152C8 diesen ABKLINGENDEN Rest anwenden. Der Port fasst e7c+152c8 zu EINER
         * Delta-Anwendung zusammen (re15_re2z_move_root) und fuehrt +0x144 gar nicht — der
         * Viertel-Nachschlag hat deshalb keinen Zwilling. Eine Zahl dafuer zu erfinden waere
         * geraten; die Richtung und die Haupt-Haelfte der Strecke stimmen. */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* +0x6 += Advance-Rueckgabe
                                                                    * @0x801044F8-508 */
        if (re2z_frame_slot(e) < 18) {                             /* sltiu 0x12 @0x8010450C-18 */
            re15_enemy_steer_point(e, e->steer_x, e->steer_z, 16); /* @0x801045BC-C8 */
            return;                                                /* j 0x801045F0 @0x801045CC */
        }
        /* ---- DER KRIECHER-TEST (@0x80104530-B8) — ab Welle 5 SCHARF ------------------------
         *   80104530: jal 0x80015614(PL.x,PL.z,a3=256)
         *   80104538/3c: `sll v0,v0,16 / bne v0,zero,0x801045F0`  ; ret16 != 0 -> raus
         *   80104544/4c/50: +0x1F0 < 0x708                        ; sonst raus
         *   8010455c/64/68: Spieler+0x1D3 & 0x80                  ; Riegel belegt -> raus
         *   80104570-80: +0x106 != Spieler+0x106                  ; andere Etage -> raus
         *   80104584/8c: +0x4 = 0x101      (v1 = 257 @0x80104584, sw im Delay-Slot @0x8010458C)
         *   80104590-98: +0x10E = (+0x10E & ~0x3F) | 1            = **die Kriecher-Wurzel**
         *   8010459c-b0: Spieler+0x1D3 |= 0x80
         * Frueher stand hier "wird NICHT genommen, weil der Port weder einen Kriecher-Brain
         * noch einen Produzenten fuer 0x800CFDCB hat". Beides ist jetzt da (re2z_crawl bzw.
         * Spieler+0x1D3), also faehrt der Port den Zweig. */
        if (re15_ai_arc_test(e, pl->x, pl->z, 256) != 0) return;   /* @0x80104530-3C */
        if (e->ai_dist >= 0x708u)          return;                 /* @0x80104544-50 */
        if (pl->re2z_self1d3 & 0x80u)      return;                 /* @0x8010455C-68 */
        if (e->floor != pl->floor)         return;                 /* @0x80104570-80 */
        re15_re2z_enter_crawler(e, pl, 1u);                        /* @0x80104584-0x801045B0 */
        return;
    }
    if (e->sub_state_2 == 2) {                                     /* P2 @0x801045D4 */
        re15_ai_set_state_word(e, 0x00020501u);                    /* lui v0,0x2 @0x801043EC +
                                                                    * ori 0x501 @0x801045D4 +
                                                                    * sw v0,4 @0x801045DC
                                                                    * = EXEC[5] PHASE 2 */
        e->re2z_res223 = (int8_t)((re2z_rand() & 0xfu) + 16u);     /* @0x801045D8-E8 */
        e->re2z_dir16a = 0;                                        /* sb zero,362 @0x801045EC */
    }
    /* sonst: Epilog @0x801045F0 — nichts zu tun. */
}

/* EXEC[12] @0x80104748 — LUNGE BITE (0x0C01, blocks D/E). Fully self-disasm'd:
 *   P0 @0x801047B8: clip 0x19 rate 7 (sw 0x70019 @0x801047B8-C0); steer(PL, 32) @0x801047DC-E0;
 *      +0x158 = arc(PL,190) @0x801047F8-80C (signed +-190 or 0); SE 10 (cd 150) @0x80104810-20.
 *   P1 @0x8010482C: arc(PL,320)==0 -> +0x158=0, phase 2 @0x80104840-50; else yaw += +0x158 each
 *      tick @0x8010485C-70 + advance(512) -> done -> phase 2 @0x80104874-80.
 *   P2 @0x80104884: clip 0x1B rate 7 (sw 0x7001B) -> phase 3 @0x80104890-94.
 *   P3 @0x8010489C: steer(PL,32); root motion 0x80015e7c @0x801048B8; done OR frame 25 ->
 *      0x101 (@0x801048D8-DC / @0x801048EC-FC). NO direct damage — the only FUN_800401d4
 *      caller in the overlay is the grab; the lunge is a distance-closer. */
static void re2z_exec_lunge(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:
        re2z_clip(e, 0x19, 0, 7, 0x200, 0);                        /* @0x801047B8-C0 */
        re15_enemy_steer_point(e, pl->x, pl->z, 32);               /* @0x801047DC-E0 */
        e->re2z_t158 = (int16_t)re15_ai_arc_test(e, pl->x, pl->z, 190); /* @0x801047F8-80C */
        if (e->re2z_cd239 == 0) { re2z_se(10); e->re2z_cd239 = 150; }   /* @0x80104810-20 */
        /* +0x144 = 11 (@0x80104824-28): dead store, see the re2z_thrust header */
        e->sub_state_2 = 1;                                        /* @0x801047C4-C8 */
        break;
    case 1:
        if (re15_ai_arc_test(e, pl->x, pl->z, 320) == 0) {         /* @0x8010482C-40 */
            e->re2z_t158 = 0; e->sub_state_2 = 2;                  /* @0x80104848-50 */
        } else {
            e->rot_y = (int16_t)(((int)e->rot_y + e->re2z_t158) & 0x0fff); /* @0x8010485C-70 */
            if (re2z_clip_done(e)) e->sub_state_2 = 2;             /* advance(512) @0x80104874-80 */
        }
        break;
    case 2:
        re2z_clip(e, 0x1B, 0, 7, 0x200, 0);                        /* @0x80104884-8C */
        e->sub_state_2 = 3;                                        /* @0x80104890-94 */
        /* fallthrough into P3's steer, like @0x80104898 falls into 0x8010489C */
        /* FALLTHRU */
    case 3:
        re15_enemy_steer_point(e, pl->x, pl->z, 32);               /* @0x8010489C-A8 */
        re15_re2z_move_root(e);                                    /* e7c @0x801048B8 + 152c8
                                                                    * @0x801048E4: clip 0x1B root
                                                                    * sx 82/141/212 (byte-read)
                                                                    * = the strike dash */
        if (re2z_clip_done(e) || re2z_frame_slot(e) == 25)         /* @0x801048D4/@0x801048F0 */
            re15_ai_set_state_word(e, 0x101);                      /* sw 0x101 @0x801048DC/FC */
        break;
    }
}

/* EXEC[13] @0x80104928 — walk-style re-roll: +0x218 re-picked from the param block
 * (@0x801049E8-EC, same two-draw shift pick as INIT), then commit 1 (@0x80104988). */
static void re2z_exec_restyle(re15_actor_t *e)
{
    /* EXEC[13] @0x80104928 ist ein VOLLER Re-Init (Review #9); heute toter Code (kein
     * 0xD01-Produzent im Port), byte-true nach eigenem Disasm: */
    re15_ai_set_state_word(e, 0x1);                                /* sw 1,4 @0x80104988 */
    e->re2z_self1d3 = 0;                                           /* sb zero,467 @0x8010499C */
    e->hp = (int16_t)re2z_hp13_tbl[re2z_rand() & 0xfu];            /* HP-Re-Roll @0x8010C600[rand&0xf]
                                                                    * (`sh v1,342` @0x801049C8) */
    e->re2z_walkclip = re2z_param_walk13[re2z_rand() & 7u];        /* EIN Draw + andi 7 aus
                                                                    * @0x801000F8 (@0x801049C4-EC) —
                                                                    * NICHT der INIT-Zwei-Draw */
    e->re2z_flags21a = 0;                                          /* sh zero,538 @0x801049DC */
    e->re2z_flag222 = 0;                                           /* sb zero,546 @0x801049E4 */
    e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));          /* @0x801049E8/A14-18 */
    e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;      /* `addiu v0,zero,13`
                                                                    * @0x801049B4, `sb v0,337/338/
                                                                    * 339` @0x801049B8-C0 */
    e->re2z_hitdir1d0 = 0;                                         /* sh zero,464 @0x8010498C */
    /* nicht modelliert (dokumentiert): Re-Bind jal 0x80028794 @0x80104984, +0x219-Clear
     * @0x801049E0, Schatten-Reset 500/-1500 @0x801049FC-0x80104A28 */
}

/* EXEC[14] @0x80104D74 — SNAP BITE (0x0E01, blocks A/B/J):
 *   P0 @0x80104DB4: clip 0x11 rate 0xF (sw 0xF0011 @0x80104DB4-BC).
 *   P1 @0x80104DD0: at frame 10 (@0x80104DD0-D8): FX packed 0x0A001000 @0x80104DE0-F4 (port:
 *      RE1.5 blood stand-in, documented) + SE 5 @0x80104DFC-04; advance(256) done ->
 *      +0x4 = 1 (sw 1 @0x80104E24 -> ACTIVE sub 0) + +0x23E = 60 (sb @0x80104E28-2C). */
static void re2z_exec_snapbite(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        re2z_clip(e, 0x11, 0, 0xF, 0x100, 0);                      /* @0x80104DB4-BC */
        /* +0x144 = 11 (@0x80104DC8-CC): dead store, see the re2z_thrust header */
        e->sub_state_2 = 1;                                        /* @0x80104DC0-C4 */
        return;
    }
    if (e->sub_state_2 == 1 && re2z_frame_slot(e) >= 10) {         /* frame 10 @0x80104DD0-D8 */
        re2z_blood_fx_at(e, 8, (int16_t)e->rot_y);                 /* FX 0x0A001000 @0x80104DE0-F4,
                                                                    * Anker `addiu a2,a2,1448`
                                                                    * @0x80104DF8 = Part 8 (Kopf) */
        re2z_se(5);                                                /* @0x80104DFC-04 */
        e->sub_state_2 = 2;                                        /* (port: one-shot marker; the
                                                                    * original fires on the exact
                                                                    * +0x14D==10 frame byte) */
    }
    if (re2z_clip_done(e)) {
        re15_ai_set_state_word(e, 0x1);                            /* sw 1 @0x80104E24 */
        e->re2z_cd23e = 60;                                        /* sb 60,574 @0x80104E28-2C */
    }
}

/* ============================================================================================
 * ============================ DER RE2-KRIECHER (+0x10E Bit 0) ===============================
 * ============================================================================================
 * NUTZER-AUFTRAG 2026-08-21: "Bei RE2 AI in der Lobby kriechen die Zombies in der Cutscene nicht
 * unter das Tor (ROOM1030)." — "Na dann baue das Kriechen nach."
 *
 * ---- GEMESSEN VOR DEM BAU (probe_re2_crawl_gate, gleicher Aktor, gleiche Vorbedingung) -------
 *   RE1.5-Flavor: Sub-Modus 0x10 @Tick 1 -> Kriech-Commit @Tick 97, grid=0x81 sca=8 motion=0x1A
 *   RE2  -Flavor: s1 bleibt 0x02, grid=0, sca=4, motion=0x04                 = KRIECHT NICHT
 * (RE2-Bank EM016 geladen: Haupt 31 Clips, loco 8, eigen 31, victim 17 — kein clip_len==0.)
 *
 * ---- DIE ZUSTAND-1-WURZEL HAT ZWEI HAELFTEN (0x8010114C, selbst disassembliert) -------------
 *   80101154: lhu v0,270(a0)        ; +0x10E
 *   8010115c: andi v0,v0,0x3f
 *   80101160: sll v0,v0,2
 *   8010116c: lw v0,-14252(at)      ; Tabelle 0x8010C854
 *   80101174: jalr v0
 * `table 0x8010c854 14` (eigener Dump) — ALLE 14 Eintraege alternieren STRIKT auf Bit 0:
 *   [0]=0x8010118C [1]=0x80101210 [2]=0x8010118C [3]=0x80101210 … [12]=0x8010118C [13]=0x80101210
 * GERADE = die aufrechte Wurzel 0x8010118C (decide @0x8010C88C / exec @0x8010C8CC, je 16),
 * UNGERADE = die KRIECHER-Wurzel 0x80101210 (decide @0x8010C90C / exec @0x8010C918, je 3):
 *   80101240: lw v0,-14068(at)      ; 0x8010C90C[+0x5]   DECIDE
 *   80101268: lw v0,-14056(at)      ; 0x8010C918[+0x5]   EXEC
 *   @0x8010C90C = {0x80102EE4, 0x801025E4(jr ra), 0x80103A70}
 *   @0x8010C918 = {0x80103024, 0x801025EC, 0x80103B48}   (ab 0x8010C924 wieder Datenworte)
 * Ein +0x5 > 2 laege im Original hinter der 3-Wort-Tabelle (Daten) — der Port faehrt dort
 * NICHTS und dokumentiert das, statt einen Zweig zu erfinden.
 *
 * ---- WER SETZT BIT 0? DREI PRODUZENTEN, ALLE BYTE-GELESEN ---------------------------------
 *  (1) @0x80104590-98  `andi 0xffc0 / ori 0x1 / sh 270` — die KAMPF-Umwandlung in EXEC[11] P1.
 *      Das ist die EINZIGE 0xffc0-Maskierung im ganzen Overlay (eigener Voll-Scan).
 *  (2) @0x80107820-24  `+0x10E = 0x2001` — Knockdown-P2 (der wiederbelebte Beinlose).
 *  (3) @0x801089B0    `addiu v1,zero,8193 / sh v1,270(s1)` = 0x2001 — die Todes-Wiederbelebung.
 *  Und der Kriecher-HURT setzt es in P2 nackt neu: @0x80107A54 `sh v0,270(s0)` mit v0 = 1
 *  (`addiu v0,zero,1` @0x801078E8) zusammen mit @0x80107A58 `sw a0,4(s0)`, a0 = 1
 *  (`addiu a0,zero,1` @0x801078B8) = **Zustand 1 / Sub 0 / Kriecher** — die byte-gelesene
 *  Kombination "Kriecher in seiner LOKOMOTION starten".
 *
 * ---- *** 0x800CFDCB IST KEIN RAUM-GLOBAL *** ----------------------------------------------
 * Die bisherige Notiz nannte es "Einmal-Riegel PRO RAUM ohne Port-Produzenten". Falsch — es ist
 * ein FELD DES SPIELERS. Beleg (dieselbe Adresse, einmal absolut, einmal basisrelativ):
 *   @0x80102F34-38  lui s2,0x800d / addiu s2,s2,-1032      ; s2 = 0x800CFBF8  = Spieler-Entity
 *   @0x80102FB0     lbu v0,-565(lui 0x800d)                ; = 0x800CFDCB
 *   @0x80102FF8     lbu v0,467(s2)                         ; = 0x800CFBF8 + 0x1D3 = 0x800CFDCB
 * Dass 0x800CFBF8 die Spieler-Entity ist, belegt dieselbe Funktion doppelt:
 *   +56  = 0x800CFC30 = Spieler-X (`lw a1,-976(lui 0x800d)` @0x80102EF4 vs `addiu a1,s2,56`)
 *   +0x106 = 0x800CFCFE = Spieler-Etage (`lbu v0,-770(...)` @0x80102F68 gegen `lbu v1,262(s1)`)
 * => 0x800CFDCB = **Spieler+0x1D3, Bit 0x80** = der globale EIN-ANGREIFER-RIEGEL.
 *    SETZER  : jeder Kriech-/Lunge-Commit (@0x80102FB8, @0x80103000, @0x80103B24, @0x801045A8)
 *    LOESCHER: `andi 0x7f` @0x80104FA0/@0x80104FAC (HURT-Grab-Abbruch) und @0x801082E8/@0x801082F4
 *              (DEATH-Grab-Abbruch), beide gegated auf `cmd == 5 && 0x800CFDAC == self`.
 *    LESER   : @0x80102F4C, @0x80103AD4, @0x8010455C, @0x8010464C, @0x80105AA4, @0x80106074,
 *              @0x80108200 — allesamt "solange ein anderer den Riegel haelt, lege ich mich nicht
 *              hin". Der Port hat das Feld pro Aktor (re2z_self1d3); der Riegel ist schlicht das
 *              Feld des SPIELER-Aktors. Kein erfundenes Global.
 * ========================================================================================== */

static void re2z_active(int slot, re15_actor_t *e, re15_actor_t *pl);   /* fwd: die aufrechte
                                                                         * Wurzel 0x8010118C */

/* Kriecher-DECIDE[0] @0x80102EE4 — der Angriffs-Entscheider der Lokomotion.
 *   80102f10: jal 0x80015614(PL.x,PL.z, a3=1024)  ; Rueckgabe VERWORFEN (v0 wird nicht gelesen)
 *   80102f2c: jal 0x80015614(PL.x,PL.z, a3=256)   ; Rueckgabe VERWORFEN (s2 ueberschreibt v0)
 *   80102f3c/40: +0x1F0 < 0x514                            sonst Epilog
 *   80102f4c/54/58: Spieler+0x1D3 & 0x80 != 0              -> Epilog
 *   80102f60-70: +0x106 != Spieler+0x106                   -> Epilog
 *   80102f78-84: +0x21A & 0x20 != 0                        -> zweite Haelfte
 *   80102f8c-a0: FUN_80015758(&self+0x38,&PL+0x38, +0x76 + 256, 256) != 0 -> zweite Haelfte
 *   80102fa4/a8: +0x4 = 0x101   80102fac-c0: Spieler+0x1D3 |= 0x80
 *   80102fc4-d0: +0x21A & 0x40 != 0                        -> Epilog
 *   80102fd4-ec: FUN_80015758(…, +0x76 - 256, 256) != 0    -> Epilog
 *   80102ff0/f4: +0x4 = 0x101   80102ff8-0x80103004: Spieler+0x1D3 |= 0x80
 * Die zwei Sektor-Tests sind KEIN if/else — die erste Haelfte faellt in die zweite. */
static void re2z_crawl_decide_move(re15_actor_t *e, re15_actor_t *pl)
{
    (void)re15_ai_arc_test(e, pl->x, pl->z, 1024);                  /* @0x80102F10, verworfen */
    (void)re15_ai_arc_test(e, pl->x, pl->z,  256);                  /* @0x80102F2C, verworfen */
    if (e->ai_dist >= 0x514u)          return;                      /* @0x80102F3C-40 */
    if (pl->re2z_self1d3 & 0x80u)      return;                      /* @0x80102F4C-58 */
    if (e->floor != pl->floor)         return;                      /* @0x80102F60-70 */
    if (!(e->re2z_flags21a & 0x20u) &&                              /* @0x80102F78-84 */
        re2z_sector(e, pl, ((int)e->rot_y + 256) & 0xfff, 256) == 0) {   /* @0x80102F8C-A0 */
        re15_ai_set_state_word(e, 0x101);                           /* @0x80102FA4-A8 */
        pl->re2z_self1d3 |= 0x80u;                                  /* @0x80102FAC-C0 */
    }
    if (e->re2z_flags21a & 0x40u)      return;                      /* @0x80102FC4-D0 */
    if (re2z_sector(e, pl, ((int)e->rot_y - 256) & 0xfff, 256) != 0) return;  /* @0x80102FD4-EC */
    re15_ai_set_state_word(e, 0x101);                               /* @0x80102FF0-F4 */
    pl->re2z_self1d3 |= 0x80u;                                      /* @0x80102FF8-0x80103004 */
}

/* Kriecher-EXEC[0] @0x80103024 — DIE KRIECH-LOKOMOTION.
 * Phasen-Dispatch auf +0x6 (`lbu v1,6(s0)` @0x80103040): 0 -> P0 @0x80103064, 1 -> P1
 * @0x801030C0, sonst Epilog @0x80103154 (`j 0x80103154` @0x8010305C).
 * P0 @0x80103064:
 *   80103064-6c: `lui v0,0xf / ori v0,v0,0x5 / sw v0,332(s0)` = +0x14C = 0x000F0005
 *                (Rate 15, Startframe 0, **Clip 5**)
 *   80103074/78: jal RNG, Delay-Slot `sb v0,6(s0)` mit v0 = 1  ->  +0x6 = 1
 *   8010308c-98: `andi 0x7 / addiu 7` -> +0x158 = (rand & 7) + 7  (`sh v0,344` @0x80103098)
 *   80103094:    jal 0x80015E7C  (BARER Aufruf: fuellt nur +0x144, 152c8 kommt erst in P1)
 *   8010309c-bc: zweiter RNG-Wurf -> +0x14D = rand & 0xF  (`sb v0,333` @0x801030BC)
 *   801030b8:    jal 0x80015E7C  (zweiter barer Aufruf)
 *   **KEIN Sprung — P0 FAELLT DURCH nach P1.**
 * P1 @0x801030C0:
 *   801030c0-cc: `lh v0,324(s0)` (+0x144) / `slti v0,v0,21` / `bne` -> **< 21 ueberspringt den
 *                ganzen Steuer-Block** und geht direkt nach 0x80103124
 *   801030d4-e0: FUN_80015558(self, +0x1C4, +0x1C6, a3=24)
 *   801030e4-f4: `lhu v1,344` / `addiu v0,v1,-1` / `bne v1,zero,0x80103120` / Delay-Slot
 *                `sh v0,344` — **getestet wird der ALTE Wert, gespeichert der neue**
 *   801030f8-108: (nur wenn ALT == 0) FUN_80015558(self, +0x1C4, +0x1C6, a3=24) ERNEUT
 *   8010310c-1c:  +0x158 = (rand & 7) + 7
 *   Danach IMMER: 8010312c jal 0x80015E7C, 80103140 jal 0x8002959C(a3=256),
 *                 8010314c jal 0x800152C8(self,0)  = das Bewegungs-PAAR des Ports */
static void re2z_crawl_exec_move(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                                     /* P0 @0x80103064 */
        /* ⛔ ZYKLUS-CLIP, NICHT play-once — und das ist byte-abgeleitet, nicht geraten:
         * P1 ruft `jal 0x8002959C` @0x80103140 in JEDEM Tick und LIEST DIE RUECKGABE NICHT
         * (kein `+0x6 +=` dahinter; die naechste Instruktion @0x80103148 laedt schon a0 fuer
         * FUN_800152C8). Der Executor kann P1 also nie verlassen — er spielt Clip 5 endlos.
         * Genau dieselbe Signatur hat der aufrechte GANG EXEC[1] (@0x80101CBC/@0x80101D60,
         * Port loop=1); wo das Original play-once meint, VERBRAUCHT es die Rueckgabe
         * (EXEC[11] P1 @0x801044F8-508, Kriecher-HURT P1 @0x80107A40-50).
         * GEMESSEN, warum das zaehlt (probe_1030_crawl_live, ROOM1030, echtes game_step,
         * geladene RE2-Bank): mit loop=0 pinnte der globale Advancer den Kriecher auf
         * `clip=5 fr=49` (dem letzten Frame der 50) — +0x144 blieb 0, der Kriecher stand ab
         * Frame ~300 fuer 2100 Frames bewegungslos bei z=-27159. */
        re2z_clip(e, 5, 0, 0xF, 0x100, 1);                         /* Wort 0x000F0005 @0x80103064-6C
                                                                    * (Blend = a3 = 256 des 959c
                                                                    * @0x80103144) */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80103070-78 */
        e->re2z_t158 = (int16_t)((re2z_rand() & 7u) + 7u);         /* @0x8010308C-98 */
        /* @0x80103094 + @0x801030B8: ZWEI BARE FUN_80015E7C. Sie fuellen nur den Delta-Vektor;
         * angewendet wird er erst durch das 152c8 am Ende von P1. Weil ZWISCHEN den beiden kein
         * Advance liegt, ist der zweite Delta zwangslaeufig 0 (prev == current) — der Wert, den
         * P1 unmittelbar danach als Torwaechter liest. Der Port haelt genau das fest, statt eine
         * Bewegung zu erfinden (gleiche Konvention wie Knockdown-P0 @0x801074C4/@0x801074E8). */
        e->re2z_root144 = 0;                                       /* sh +0x144 @0x80015FD8 */
        e->anim_frame = (uint16_t)(re2z_rand() & 0xfu);            /* +0x14D @0x8010309C-BC */
        /* FALLTHROUGH nach P1 @0x801030C0 — das Original hat hier KEINEN Sprung. */
        /* FALLTHRU */
    }
    if (e->sub_state_2 == 1) {                                     /* P1 @0x801030C0 */
        if ((int)e->re2z_root144 >= 21) {                          /* slti 21 @0x801030C8-CC */
            re15_enemy_steer_point(e, e->steer_x, e->steer_z, 24); /* @0x801030D4-E0 */
            uint16_t old = (uint16_t)e->re2z_t158;                 /* lhu 344 @0x801030E4 */
            e->re2z_t158 = (int16_t)(uint16_t)(old - 1u);          /* Delay-Slot-Store @0x801030F4 */
            if (old == 0u) {                                       /* bne v1(ALT),zero @0x801030F0 */
                re15_enemy_steer_point(e, e->steer_x, e->steer_z, 24);  /* @0x801030F8-108 */
                e->re2z_t158 = (int16_t)((re2z_rand() & 7u) + 7u); /* @0x8010310C-1C */
            }
        }
        re15_re2z_move_root(e);                                    /* e7c @0x8010312C + 152c8
                                                                    * @0x8010314C = das Paar */
        (void)re2z_clip_done(e);                                   /* 959c(a3=256) @0x80103140 —
                                                                    * die Rueckgabe wird im
                                                                    * Original NICHT gelesen
                                                                    * (kein `+0x6 +=` hier) */
    }
    /* +0x6 >= 2: Epilog @0x80103154 — nichts zu tun. */
}

/* Kriecher-DECIDE[2] @0x80103A70 — derselbe Entscheider fuer den WARTENDEN Kriecher.
 *   80103a98: jal 0x80015614(PL, 1024)   ; Rueckgabe VERWORFEN
 *   80103ab4: jal 0x80015614(PL,  512)   ; **diese wird gelesen**
 *   80103abc/c0: `sll v0,v0,16 / bne v0,zero,0x80103b30`  -> ret16 != 0 -> Epilog
 *   80103ac4/c8: +0x1F0 < 0x514                           sonst Epilog
 *   80103ad4-e0: Spieler+0x1D3 & 0x80                     -> Epilog
 *   80103ae8-f4: +0x154 & 0x800 == 0                      -> Epilog
 *   80103afc-0c: +0x106 != Spieler+0x106                  -> Epilog
 *   80103b10/14: +0x4 = 0x101   80103b18-2c: Spieler+0x1D3 |= 0x80 */
static void re2z_crawl_decide_wait(re15_actor_t *e, re15_actor_t *pl)
{
    (void)re15_ai_arc_test(e, pl->x, pl->z, 1024);                  /* @0x80103A98, verworfen */
    if (re15_ai_arc_test(e, pl->x, pl->z, 512) != 0) return;        /* @0x80103AB4-C0 */
    if (e->ai_dist >= 0x514u)               return;                 /* @0x80103AC4-C8 */
    if (pl->re2z_self1d3 & 0x80u)           return;                 /* @0x80103AD4-E0 */
    /* +0x154 & 0x800 (@0x80103AE8-F4): dasselbe Flagwort ohne Port-Produzenten wie in
     * DECISION[0] — dort seit Welle A als 1 GEMAPPT (OPEN, siehe re2z_decide_stand). Hier
     * identisch gehandhabt, damit beide Stellen dieselbe Aussage tragen. */
    if (e->floor != pl->floor)              return;                 /* @0x80103AFC-0C */
    re15_ai_set_state_word(e, 0x101);                               /* @0x80103B10-14 */
    pl->re2z_self1d3 |= 0x80u;                                      /* @0x80103B18-2C */
}

/* Kriecher-EXEC[2] @0x80103B48 — das WARTEN. Neun Instruktionen, vollstaendig:
 *   80103b48: lbu v0,6(a0)
 *   80103b50: bne v0,zero,0x80103b64    ; +0x6 != 0 -> nichts tun
 *   80103b54: addiu v0,zero,1           (Delay-Slot)
 *   80103b58: sb v0,6(a0)               ; +0x6 = 1
 *   80103b5c: addiu v0,zero,23
 *   80103b60: sw v0,332(a0)             ; +0x14C = 0x00000017 = Clip 0x17, Frame 0, **Rate 0**
 *   80103b64: jr ra
 * Es gibt KEINEN Ausgang: nur DECIDE[2] holt den Kriecher hier wieder heraus. */
static void re2z_crawl_exec_wait(re15_actor_t *e)
{
    if (e->sub_state_2 != 0) return;                                /* @0x80103B48-50 */
    e->sub_state_2 = 1;                                             /* @0x80103B54-58 */
    re2z_clip(e, 0x17, 0, 0, 0x100, 0);                             /* Wort 23 @0x80103B5C-60 */
}

/* ---- die KRIECHER-Wurzel 0x80101210 (decide-dann-exec auf DEMSELBEN Tick, @0x8010122C-74) ---
 *   8010122c: lbu v0,5(s0)        8010123c/40: 0x8010C90C[+0x5]   80101248: jalr  (DECIDE)
 *   80101254: lbu v0,5(s0)        80101264/68: 0x8010C918[+0x5]   80101270: jalr  (EXEC)
 * +0x5 wird zwischen den beiden Aufrufen FRISCH gelesen. Beide Tabellen sind DREI Worte lang;
 * ab 0x8010C924 folgt die (u16,u16)-Datentabelle (0x803200BE, 0x80400096, 0x8040006E, …).
 *
 * ⛔ OFFEN, ausdruecklich BENANNT statt gefuellt: ein Kriecher mit +0x5 > 2.
 * Erreichbar ist er im Port ueber den Wurf-Ausgang des GRIFFS (Kriecher-EXEC[1]):
 *   80102d24: addiu v0,zero,1281      ; 0x501
 *   80102d2c: sw    v0,4(s1)          ; +0x4 = 0x501  -> +0x5 = 5
 * (Grab-Phase 8, @0x80102C60..0x80102EB4). Dieser Store ist NICHT auf +0x10E gegated. Im
 * Original wuerde die Wurzel danach `lw 0x8010C918[5]` = 0x8010C92C = **0x8040006E** laden und
 * `jalr` darauf ausfuehren — ein Datenwort, also ein Absturz. Der Zustand kann im Original
 * folglich nicht vorkommen; WELCHE Vorbedingung ihn dort verhindert, ist noch nicht gefunden
 * (die naechsten Wege: die Phasenkette des Griffs unter s5=1 durchrechnen, und die vier
 * `+0x10E = 0x2001`-Produzenten @0x80106B0C / @0x80107820 / @0x801089B0 auf ihre Folgezustaende
 * pruefen).
 * PORT-VERHALTEN BIS DAHIN — die kleinstmoegliche Abweichung: fuer +0x5 > 2 bleibt exakt das
 * stehen, was der Port VOR dieser Welle getan hat (die aufrechte Tabelle @0x8010C88C/@0x8010C8CC).
 * Damit aendert diese Welle NUR die drei Substates, die der Kriecher wirklich hat, und erzeugt
 * weder einen Freeze noch einen erfundenen Zweig. Gemessen wurde genau das: ohne diesen
 * Rueckfall blieben in test_re2_zombie_abc 16 Aktoren mit `s1=5 s2=1 10E=0x2001` stehen
 * (+0x1D3 = 0x80 blieb gesetzt = unsterblich). */
static void re2z_crawl(int slot, re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_1 > 2) { re2z_active(slot, e, pl); return; }   /* s. Block oben (OPEN) */
    switch (e->sub_state_1) {                                       /* DECIDE @0x8010C90C */
    case 0: re2z_crawl_decide_move(e, pl); break;                   /* 0x80102EE4 */
    case 1: break;                                                  /* 0x801025E4 = jr ra */
    default: re2z_crawl_decide_wait(e, pl); break;                  /* [2] 0x80103A70 */
    }
    switch (e->sub_state_1) {                                       /* EXEC   @0x8010C918 */
    case 0: re2z_crawl_exec_move(e); break;                         /* 0x80103024 */
    case 1: re2z_exec_grab(e, pl); break;                           /* 0x801025EC — DIESELBE
                                                                     * Funktion wie aufrecht [3];
                                                                     * ihre Kriech-Variante waehlt
                                                                     * sie selbst ueber +0x10E&1
                                                                     * (@0x8010266C-74) */
    default: re2z_crawl_exec_wait(e); break;                        /* [2] 0x80103B48 */
    }
}

/* ============================================================================================
 * DECISION[2] @0x80101F7C — die ANGRIFFS-LEITER DES ZWEITEN GANGS.
 *
 * ⛔ Der Port fuehrte @0x8010C88C[2] als "stub / not RE'd" (`default: break`). Das ist die
 * zweite Haelfte desselben Nutzer-Befunds: sobald EXEC[2] (der Gang mit den erhobenen Armen)
 * seine byte-truen Produzenten bekommt, LEBT der Zombie im Nahbereich in Substate 2 — und ohne
 * diese Leiter koennte er dort weder greifen noch beissen. Selbst disassembliert 2026-08-22.
 *
 * Struktur = dieselbe Leiter wie DECISION[1] (@0x80101714), mit DREI belegten Unterschieden:
 *   - Der Kegel der Bloecke B / J / K ist 1300 statt 512
 *     (`jal 0x80015614` a3=1300 @0x80101FF0, Ergebnis in s3; die Rueckgabe des 1024er-Aufrufs
 *      @0x80101FB0 wird VERWORFEN — s4 uebernimmt erst der 512er @0x80101FD0).
 *   - Es gibt NUR EINEN Lunge-Block (kein D): dist < 0xBB8 && arc512 != 0 && cfbf6 & 0x17 &&
 *     (rand & 1) == 0 -> 0x0C01   (@0x801020C4-104)
 *   - Blocke A/C/G/K sind wortgleich zu DECISION[1].
 * Adressen der Commits: 0x0E01 @0x80102048/94 und @0x80102204, 0x0A01 @0x801020C0,
 * 0x0C01 @0x80102104, 0x0301 @0x80102178/@0x801021BC (+ PL+0x1D3 |= 0x80 @0x80102188/@0x801021D0),
 * 0x00060801 @0x80102230 (+ +0x10E |= 0x4000 @0x80102234-38).
 * "Last writer wins" gilt genau wie in DECISION[1]: keine Instruktion liest +0x4 zurueck, und
 * der EINZIGE fruehe Ausstieg ist Block A (`j 0x8010223c` @0x80102048).
 * ==========================================================================================*/
static void re2z_decide_bump(re15_actor_t *e, re15_actor_t *pl)
{
    uint32_t dist = e->ai_dist;                                    /* lw 496 @0x80101FAC */
    (void)re15_ai_arc_test(e, pl->x, pl->z, 1024);                 /* @0x80101FB0 — VERWORFEN */
    int a512  = re15_ai_arc_test(e, pl->x, pl->z,  512);           /* s4 @0x80101FD0-D4 */
    int a1300 = re15_ai_arc_test(e, pl->x, pl->z, 1300);           /* s3 @0x80101FF0-F4 */

    if (e->re2z_cd23e == 0) {                                      /* lbu 574 @0x80101FF8-0x80102004 */
        /* Block A @0x8010200C-4C liest +0x1F4/+0x1F8 — im Zombie-Overlay gibt es KEINEN
         * Schreiber (derselbe Beleg wie in re15_re2z_fill_gates), also inert. */
        if (a1300 == 0                                             /* sll/bne @0x8010205C-60 */
            && dist < 0x7d0u                                       /* sltiu 0x7d0 @0x80102064 */
            && e->floor != pl->floor                               /* beq-away @0x8010207C */
            && !re15_player_is_grabbed())                          /* bne @0x8010208C (ganzes Byte;
                                                                    * die unteren 7 Bit haben im
                                                                    * Port keinen Produzenten —
                                                                    * dieselbe Lage wie Block B
                                                                    * der Gang-Leiter) */
            re15_ai_set_state_word(e, 0x0E01);                     /* 3585 @0x80102090-94 */
    }
    /* Block C @0x80102098-C0 (+0x1D4 & 0xC000): RE1.5-Raumdaten tragen die Bits nie -> inert. */
    if (dist < 0xbb8u                                              /* sltiu 0xbb8 @0x801020A8/C4 */
        && a512 != 0                                               /* beq-zero-skip @0x801020D0 */
        && (re2z_cfbf6(pl) & 0x17u)                                /* andi 0x17 @0x801020E4 */
        && (re2z_rand() & 1u) == 0u)                               /* jal @0x801020F0, andi @0x801020F8 */
        re15_ai_set_state_word(e, 0x0C01);                         /* 3073 @0x80102100-04 */

    /* Der G/J-Fork @0x80102108-14 haengt an PL+0x8 == 15 (Spieler-Routinen-Id). Der Port hat
     * dafuer keinen Produzenten (OPEN, exakt wie in re15_re2z_fill_gates: `PL+0x8 -> 0`), also
     * laeuft immer der G-Zweig; der J-Zweig @0x801021D4-204 ist damit unerreichbar und bleibt
     * — wie in DECISION[1] — dokumentiert statt erfunden.
     * Block G @0x80102114-1D0 — wortgleich zu DECISION[1]s G. */
    if (dist < 0x4b0u                                              /* sltiu 0x4b0 @0x80102114 */
        && !re15_player_is_grabbed()                               /* andi 0x80 @0x80102128 */
        && e->floor == pl->floor) {                                /* bne-away @0x80102140 */
        if (!(e->re2z_flags21a & 0x20u)                            /* andi 0x20 @0x80102150 */
            && re2z_sector(e, pl, ((int)e->rot_y + 256) & 0xfff, 256) == 0) /* @0x80102158-70 */
            re15_ai_set_state_word(e, 0x0301);                     /* 769 @0x80102174-78 */
        if (!(e->re2z_flags21a & 0x40u)                            /* andi 0x40 @0x80102194 */
            && re2z_sector(e, pl, ((int)e->rot_y - 256) & 0xfff, 256) == 0) /* @0x8010219C-B4 */
            re15_ai_set_state_word(e, 0x0301);                     /* 769 @0x801021B8-BC */
    }
    /* Block K @0x80102208-38: der Port-Spieler wird nie HP == -32768 (fill_gates), inert. */
}

/* ---- ACTIVE dispatcher: decision-then-executor on the SAME tick (@0x801011A8-EC) ----------- */
static void re2z_active(int slot, re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {                                      /* DECISION @0x8010C88C */
    case 0:  re2z_decide_stand(e, pl); break;                      /* 0x80101294 */
    case 1:  re2z_decide_walk_apply(e, pl); break;                 /* 0x80101714 (the ladder) */
    case 2:  re2z_decide_bump(e, pl); break;                       /* 0x80101F7C */
    default: break;                                                /* stubs / not RE'd */
    }
    switch (e->sub_state_1) {                                      /* EXECUTOR @0x8010C8CC */
    case 0:  re2z_exec_stand(e); break;                            /* 0x801013F4 */
    case 1:  re2z_exec_walk(slot, e, pl); break;                   /* 0x80101A40 */
    case 2:  re2z_exec_bump(e, pl); break;                         /* 0x80102260 */
    case 3:  re2z_exec_grab(e, pl); break;                         /* 0x801025EC */
    case 4:  break;                                                /* 0x80103178 = jr ra (verified) */
    case 5:  re2z_exec_knockdown(e); break;                        /* 0x80103188 */
    case 6:  re2z_exec_six(e); break;                              /* 0x80103954 */
    case 7:  re2z_exec_lying(e, pl); break;                        /* 0x80103780 */
    case 8:  re2z_exec_feeding(e, pl); break;                      /* 0x80103B74 */
    case 9:  re2z_exec_getup(e); break;                            /* 0x80103E48 */
    case 10: re15_ai_set_state_word(e, 0x101); break;              /* 0x8010417C block-C hammering —
                                                                    * INERT (see fill_gates), bail */
    case 11: re2z_exec_eleven(e, pl); break;                       /* 0x8010439C */
    case 12: re2z_exec_lunge(e, pl); break;                        /* 0x80104748 */
    case 13: re2z_exec_restyle(e); break;                          /* 0x80104928 */
    case 14: re2z_exec_snapbite(e); break;                         /* 0x80104D74 */
    case 15: re15_ai_set_state_word(e, 0x101); break;              /* 0x80104E54 bank-B chain — no
                                                                    * reachable producer, OPEN */
    default: re15_ai_set_state_word(e, 0x101); break;
    }
}

/* ============================================================================================
 * HURT @0x80104F40 — die TREFFERREAKTION (Nutzer-Report 2026-08-17: "Sie reagieren nicht auf
 * die Schuesse auf sie."). Vollstaendig neu disassembliert 2026-08-18; die alte Fassung war an
 * drei Stellen fehlkalibriert und sprang statt in die Reaktion sofort zurueck in den Gang.
 *
 * ---- Die Wurzel FUN_80104F40(a0=entity, a1, a2) ----
 *   @0x80104F68-FDC  Grab-Abbruch (Spieler-Kommando 5 && PL+0x1B4 == self)
 *   @0x80104FE0-500C `lhu a0,270`; `andi 0x1` -> eigene 1D-Tabelle @0x8010CBE8[+0x5] (Eintrag
 *                    0 = NULL, 1..18 = 0x80107888) — der KRIECHER-Zweig.
 *   @0x80105014-38   `lhu 538`; `andi 0x10` -> FUN_80107A78 (Kriecher-Umbau), return.
 *   @0x8010503C-58   Schwelle v1 = (+0x10E & 0x40) ? ((+0x5 != 1) ? 23 : 0) : 0
 *   @0x8010505C-64   `lbu v0,6(s1)` / `bne v0,zero,0x80105168` — Flinch NUR in +0x6 == 0
 *   @0x8010506C-78   `lb v0,547(s1)` / `slt v0,v1,v0` / `bne` — Flinch nur wenn +0x223 <= v1
 *   @0x80105080-9C   `+0x222 == 1` ODER `+0x5 == 1` -> Flinch, sonst nur +0x222 = 1 @0x80105164
 *   @0x801050A0-C8   +0x222 = 1 ; +0x4(WORT) = 0x501 ; +0x223 = 16 + (rand & 15)
 *   @0x801050B0-15C  Blut: (+0x1D2 % 3) == 0 -> generisch (id 6096, ofs {0,800,0}, Block +0x198
 *                    +244), sonst am Knochen (id 4096, Block +0x198 +72, Pos aus +0x58C/590/594)
 *   @0x80105168-284  +0x21A & 2 (liegend) -> +0x4 = 0x60501, +0x16B = 1, Zerleger je +0x5
 *   @0x80105288-3D8  +0x21A & 0x60 / (s8)+0x152 < 0 / +0x1D0 & 0xC0 -> Gore-Zweig (SE 9 …)
 *   @0x801053E0-410  DISPATCH  `tbl[+0x5][+0x1D2]`, Basis 0x8010C940, Zeilen-Stride 36:
 *                       lbu v1,5(a0) ; sll v0,v1,3 ; addu v0,v0,v1 ; sll v0,v0,2
 *                       lbu v1,466(a0) ; addu v0,v0,a2 ; sll v1,v1,2 ; addu v1,v1,v0
 *                       lw v0,0(v1) ; jalr v0
 *
 * ---- DREI belegte Fehlkalibrierungen, die hier gefixt werden ----
 * (1) +0x223 ist KEIN Schadens-Akkumulator. Die Wurzel VERGLEICHT nur (@0x8010506C-78); der
 *     ABZUG steht im Haupt-Handler und ist eine TABELLEN-Konstante je Zeile:
 *         801055c4: lbu a0,547(s1)
 *         801055d0: lui at,0x8011
 *         801055d4: addu at,at,v0            ; v0 = +0x5
 *         801055d8: lbu v1,-13261(at)        ; = 0x8010CC33 + (+0x5)
 *         801055e0: subu a0,a0,v1
 *         801055ec: sb   a0,547(s1)
 *     Der frueher behauptete "nirgends dekrementiert"-Scan hat genau diesen Store uebersehen.
 * (2) +0x6 ist die REAKTIONS-PHASE, kein Treffer-Winkel (Produzent-Fix in re15_damage.c).
 * (3) +0x1D2 = ZONE + 3*BRACKET; die Basis-Zone ist 1 (Produzent-Fix in re15_damage.c).
 *
 * ---- Die Dispatch-Tabelle, selbst gedumpt (`table 0x8010c940 162`) ----
 * Zeile 0 (0x8010C940..63) enthaelt KEINE Zeiger (0x8032008C, 0x803E0096, …) — sie ist der
 * Schwanz der davorliegenden (u16,u16)-Datentabelle; Zeile 0 wird also nie dispatcht. Gueltig
 * sind die Zeilen 1..17 (Zeile 17 komplett NULL; ab 0x8010CBE8 beginnt die 1D-Kriecher-Tabelle).
 *   Zeile |  c0    c1    c2  |  c3    c4    c5  |  c6    c7    c8
 *      1  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      2  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      3  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      4  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *      5  |  -     -     -   |  -     -     -   |  -     -     -
 *      6  |  -     -     -   |  -     -     -   |  -     -     -
 *      7  | 7438  66FC   -   | 7438  5BC0   -   | 7438  5438   -
 *      8  | 7438  66FC   -   | 7438  5BC0   -   | 7438  5BC0   -
 *      9  | 7438  5BC0  5BC0 | 5438  5438  5438 | 5438  5438  5438
 *     10  | 5BC0  5BC0  5BC0 | 5438  5438  5438 | 5438  5438  5438
 *     11  | 5BC0  5BC0  5BC0 | 5438  5438  5438 | 5438  5438  5438
 *     12  | 7438  703C   -   | 7438  703C   -   | 7438  703C   -
 *     13  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *     14  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *     15  | 7EF0  7EF0   -   | 7EF0  7EF0   -   | 7EF0  7EF0   -
 *     16  | 5438  5438   -   | 5438  5438   -   | 5438  5438   -
 *     17  |  -     -     -   |  -     -     -   |  -     -     -
 * ("-" = NULL). Spalte = zone + 3*bracket, also c0..c2 = Bracket 0, c3..c5 = 1, c6..c8 = 2;
 * innerhalb einer Gruppe zone 0/1/2. Zone 2 (c2/c5/c8) ist ausserhalb der Zeilen 9..11 NULL —
 * eine Kombination, die das Original nie erzeugt (Zone 2 braucht word0 & 0x10000000).
 *
 * ---- ZEILEN-SEMANTIK (+0x5) = DIE RE2-ATTACKEN-ID — VOLLSTAENDIG AUFGELOEST 2026-08-18 ----
 * +0x5 ist NICHT der KI-Substate: beide RE2-Applier ueberschreiben ihn pro Treffer mit der
 * TREFFER-ART: `sb s5,5(s1)` @0x80047324 (s5 = Hitcode, Basis a3) bzw. `+0x5 = (a1>>16)+1`
 * @0x80041AA0-B4. Die frueher hier als "OPEN" gefuehrte Herkunft ist jetzt zu Ende
 * disassembliert; die Kette ist LUECKENLOS und macht die Zeile zu einer WAFFEN-Id:
 *
 *  (1) EQUIP FUN_8006B000 (info/re2leon/PSX.EXE):
 *        8006b004: lbu a3,23548(a3)      ; a3 = *(u8*)0x800D5BFC  = angewaehlter Inventar-Slot
 *        8006b028: addu a2,v0,a1         ; a2 = 0x800CC1E8 + slot*4
 *        8006b034: lbu v0,-30636(at)     ; at = a2+0x10000 -> ea = 0x800D4A3C + slot*4 = ITEM-ID
 *        8006b040: sltiu v0,v0,0x14      ; Item-Id < 20  == "das ist eine Waffe"
 *        8006b088: sb a3,23544(at)       ; 0x800D5BF8 = Slot   (128 = nichts ausgeruestet)
 *        8006b09c: sb v0,23546(at)       ; 0x800D5BFA = ITEM-ID der Waffe
 *      Dass 0x800D4A3C das Inventar-Array (Stride 4, Byte0 = Item-Id) ist, belegt FUN_800696CC
 *      @0x800696E4-0x80069704: es scannt `0x800D4A3C + i*4` nach dem Byte == a0 und liefert i.
 *  (2) UEBERNAHME in die Entity FUN_8003BAF0:
 *        8003bd30: lbu v1,23544(v1)      ; 0x800D5BF8
 *        8003bd38: beq v1,128,0x8003bd50 ; nichts ausgeruestet
 *        8003bd44: lbu v0,23546(v0)      ; 0x800D5BFA
 *        8003bd4c: sh v0,270(s2)         ; +0x10E = ITEM-ID      (8003bd50: sh zero,270 sonst)
 *  (3) ANGRIFF FUN_80047C6C @0x80047EB4-F8: `v0 = +0x10E & 0xFFF`, `a1 = ((v0-1)<<16)|band`,
 *      `a2 = 0x800A68E8 + v0*24 + 0x800A6F8C[band]*8`, dann `jal FUN_800410CC`. Zwilling
 *      FUN_80048314 @0x80048444-60. -> `+0x5 = (a1>>16)+1 = ITEM-ID` @0x80041AA0-B4.
 *  => **Die Zeile IST die Item-Id der gefuehrten Waffe.** 0 = unbewaffnet, 1..19 = Waffen.
 *
 * ---- WIE VIELE ATTACKEN-IDS? GENAU 19 (1..19) — drei unabhaengige Belege ----
 *  a) `sltiu ...,0x14` @0x8006B040 (Item-Id < 20 == Waffe).
 *  b) Die Post-Hit-Handler-Tabelle @0x800A6FDC hat exakt 20 Eintraege (0..19): 0x800A6FDC +
 *     20*4 = 0x800A702C, ab dort stehen andere Daten.
 *  c) Die per-Gegnertyp-Schadensrecords `0x800A6A88[typ] + (id-1)*20` (Stride belegt
 *     @0x800413A4-B8 `v1*5*4` bzw. @0x8004723C `addiu v0,v0,-20`) sind je 0x17C = 380 Byte
 *     = 19*20 lang (0x800A412C -> 0x800A42A8 -> 0x800A4424 -> 0x800A45A0 …).
 *
 * ---- ZOMBIE-SCHADEN JE ATTACKEN-ID (0x800A412C = Typen 0x10-0x14/0x18-0x1F, selbst gedumpt) --
 * HP-Abzug = `(word0 >> (bracket*10)) & 0x3FF` (@0x80047244-54 / FUN_800410CC):
 *   id 1: 3/0/0 | 2: 16/15/14 | 3: 16/15/14 | 4: 16/15/14 | 5: 900 | 6: 900 | 7: 200/60/40
 *   id 8: 300/80/60 | 9: 200/50/10 | 10: 200/50/5 | 11: 200/50/10 | 12: 30 | 13: 16/15/14
 *   id 14: 60 | 15: 4 | 16: 15 | 17: 900 | 18: 8 | 19: 16/15/14
 *
 * ---- WARUM DIE ZEILEN 5, 6, 17 NULL SIND: SIE SIND IM ORIGINAL UNERREICHBAR ----
 * Genau diese drei Ids tragen `0x384E1384` = 900/900/900 Schaden (@0x800A412C + 4*20 / 5*20 /
 * 16*20). Kein Zombie hat 900 HP (HP-Tabelle @0x8010C600 max 118), also setzt der Applier
 * IMMER `+0x4 = 3` (DEATH) statt 2 (`lh v1,342 / bgez / sw 2,4(s1)` @0x8004727C-90) und die
 * HURT-Wurzel FUN_80104F40 laeuft dort nie. Die NULL-Zellen sind also kein Loch, sondern der
 * Beweis, dass Magnum-/Raketen-Klasse den Zombie sofort toetet.
 *
 * ---- DIE GEOMETRIE-TABELLE @0x800A68E8 (id-Stride 24, 3 Hoehen-Gruppen a 8 Byte) ------------
 * Sie bestaetigt die Waffen-KLASSEN unabhaengig vom Schaden (selbst gedumpt): id 1 hat drei
 * EIGENE Boxen je Zielhoehe (0x800A657C/0x800A63A8/0x800A64E0 = Nahkampf-Schwung); die Ids
 * 2/3/4/13 teilen sich EINE identische Box (0x800A6618/34/50 + 0x800A666C = Pistolen-Klasse);
 * 5,6 / 7,8 / 14 / 15 haben je eigene Boxen; 9,10,11,12,16,17 zeigen auf den NULL-Record
 * 0x800A6350 — diese Waffen benutzen den Kontakt-Pfad FUN_800410CC gar nicht, sondern erzeugen
 * eigene Projektil-/Flammen-Entities (Granaten, Bolzen, Flammenstrahl, Rakete).
 * ==========================================================================================*/

/* ==========================================================================================
 * DIE RE1.5-WAFFE -> RE2-ATTACKEN-ID-ZUORDNUNG (Nutzer-Vorgabe 2026-08-18: "Einzelne staerkere
 * Waffen muessen auch in Resident Evil 1.5 reagieren, unabhaengig von der RE2-KI").
 *
 * Das Problem: der Port schiesst mit dem RE1.5-Hitscan FUN_80011F50 (`+0x5 = weapon_id`
 * @0x800124BC). RE1.5-Waffen-Id und RE2-Attacken-Id sind ZWEI VERSCHIEDENE Id-Raeume — RE1.5
 * hat 22 Zeilen (0..21, Schaden @0x8006E0D0, Tester-Dispatch @0x8006E548), RE2 hat 19 Waffen
 * (1..19). Roh durchgereicht landeten 7 von 22 RE1.5-Waffen auf einer NULL-Zelle bzw. ausserhalb
 * der Tabelle (gemessen: w 0,5,6,17,19,20,21 -> gar keine Reaktion).
 *
 * Die Zuordnung unten geht nach WAFFEN-KLASSE. Belegt ist die Klasse auf BEIDEN Seiten:
 *   RE1.5: Tester-Dispatch @0x8006E548 (0x80012574 = Schusswaffe / 0x800127FC = Nahkampf /
 *          0x800128A0 = Sprengstoff), Reach @0x8006E5A0, Schaden @0x8006E0D0, Munitions-Records
 *          @0x80074C88.. (Waffen-Props @0x80074DA8, Stride 0xC).
 *   RE2:   Geometrie-Tabelle @0x800A68E8 (s.o.), Schadensrecords @0x800A412C, Poise-Kosten
 *          @0x8010CC33 und die Reaktionszeile selbst @0x8010C940.
 * Wo RE1.5 mehr Waffen einer Klasse hat als RE2 Zeilen, wird die naechstliegende Zeile derselben
 * Klasse gewaehlt — das ist eine [PORT-ZUORDNUNG, kein Byte-Beleg], je Zeile unten begruendet.
 * INVARIANTE: KEINE Waffe darf auf eine stumme Zelle fallen (re2z_row_guard unten).
 * ========================================================================================== */
enum { RE2Z_ATK_MAX = 19 };   /* Ids 1..19 (Beleg (a)/(b)/(c) oben)                             */

/* ---- DIE RE2-WAFFEN-IDS 1..19, byte-belegt aus info/re2leon/PSX.EXE --------------------------
 * Item-Definitionstabelle `{u8 maxQty, u8 pad, u8 flags, u8 nCombine, Rec *list}` mit Stride 8
 * ab 0x800A9E1C — Beleg FUN_800695B0 @0x80069600 (`lbu a0,-25057(at)` = 0x800A9E1F + id*8) und
 * @0x80069618 (`lw v1,-25056(at)` = 0x800A9E20 + id*8). `flags` Bit 7 = DAUERFEUER, gesetzt genau
 * fuer 0x0E/0x0F/0x10/0x12 — deckungsgleich mit den Dauerfeuer-Zweigen in FUN_8006A0CC
 * (@0x8006A128 id 15, @0x8006A130 id 18, @0x8006A184 id 16, @0x8006A1D0 id 14).
 * Zweiter, unabhaengiger Beleg: die Waffen-Modelle `info/re2leon/PL0/PLD/PL00W%02X.PLW` (Leon)
 * bzw. `PL01W%02X.PLW` (Claire) — PLW-Index == Item-Id, Stub-Dateien = "hat diese Waffe nicht";
 * PL01W09/0A/0B sind byte-gleich (EIN Granatwerfer, 3 Munitionsarten), PL00W01 == PL01W01 (Messer).
 *   1 Messer | 2 Handgun (Leon, 18) | 3 Handgun Browning HP (Claire, 13) | 4 Custom Handgun (18)
 *   5 Magnum (8) | 6 Custom Magnum (8) | 7 Schrotflinte (5) | 8 Custom Schrotflinte (7)
 *   9 GL Explosiv | 10 GL Brand | 11 GL Saeure | 12 Bowgun (18) | 13 Colt S.A.A. (6)
 *   14 Spark Shot (100) | 15 SMG/Ingram (100) | 16 Flammenwerfer (100) | 17 Raketenwerfer (4)
 *   18 Gatling (100) | 19 Chris-Pistole (15, nur PL0BW13.PLW)
 * Gegenprobe gegen die Zombie-Schadensrecords oben: 1 -> 3 (Messer), 5/6 -> 900 (Magnum),
 * 7/8 -> 200/300 (Schrot), 12 -> 30 (Bowgun-Bolzen), 15/18 -> 4/8 pro Schuss (Dauerfeuer),
 * 17 -> 900 (Rakete). Beide Quellen stimmen ueberein.
 *
 * ---- RE1.5-Waffen-Id (0..21) -> RE2-Attacken-Id (Zeile @0x8010C940) --------------------------
 *  w  RE1.5-Waffe            -> id  RE2-Zeile          Kriterium
 *  -- ---------------------- --- ------------------    ------------------------------------------
 *  0  unbewaffnet (dmg 0)       1  MAIN                RE2-Id 0 hat KEINE Geometrie (@0x800A68E8
 *                                                      Zeile 0 = Datenwoerter) und feuert nie; der
 *                                                      Port kann mit w=0 aber "treffen" (Schaden 0).
 *                                                      Schwaechste gueltige Zeile. [PORT-ZUORDNUNG]
 *  1  Combat Knife              1  Messer   MAIN       IDENTITAET. Beide sind die Nahkampfwaffe mit
 *                                                      dem kleinsten Schaden; RE2-Id 1 ist die
 *                                                      EINZIGE Zeile mit eigener Nahkampf-Geometrie
 *                                                      je Zielhoehe (@0x800A6900).
 *  2  Pipe (Nahkampf, dmg 24)   1  Messer   MAIN       ebenfalls Nahkampf-Tester 0x800127FC
 *                                                      (@0x8006E550), aber RE2 hat nur EINE
 *                                                      Nahkampfzeile. [PORT-ZUORDNUNG]
 *  3  Browning HP               3  Browning HP MAIN    IDENTITAET — RE2-Id 3 IST die Browning HP
 *                                                      (Claires Startpistole, Magazin 13).
 *  4  SIG P228                  2  Handgun  MAIN       die andere Standard-Pistolenzeile (Leons
 *                                                      VP70). Gleiche Poise-Kosten 0x0F wie Zeile 3
 *                                                      (@0x8010CC35/36). [PORT-ZUORDNUNG]
 *  5  Beretta M93R (3-Schuss)   4  Custom HG MAIN      Pistolenzeile mit der GROESSTEN Poise-Kosten-
 *  6  Glock 18 (Vollauto)       4  Custom HG MAIN      Konstante 0x23 = 35 (@0x8010CC37) = das
 *                                                      RE2-Pendant zu "mehr Stopping Power pro
 *                                                      Trigger-Zug" (Burst/Vollauto, RE1.5-Schaden
 *                                                      15 statt 5). [PORT-ZUORDNUNG]
 *  7  Super Redhawk (.44)       5  Magnum   (NULL)     KLASSEN-IDENTITAET. RE2 5/6 = 900 Schaden =
 *                                                      immer toedlich; RE1.5 w7 toetet ebenfalls
 *                                                      IMMER (`hp = -1` @0x800124FC fuer type<0x20,
 *                                                      alle RE2-Zombietypen 0x10..0x18 sind <0x20).
 *                                                      Die NULL-Zeile bleibt damit genau so
 *                                                      unerreichbar wie im Original.
 *  8  Remington M870            7  Schrot   7438/RAGDOLL  KLASSEN-IDENTITAET (Standard-Schrotflinte,
 *                                                      eigene breitere Geometrie 0x800A6724).
 *  9  Hand Grenade (HE)         9  GL Explosiv STAGGER Sprengstoff-Tester 0x800128A0 @0x8006E56C-74.
 * 10  Acid Grenade             11  GL Saeure  STAGGER  Die Munitionsart ist auf BEIDEN Seiten
 * 11  Incend. Grenade          10  GL Brand   STAGGER  belegt: RE1.5 Subtyp-Byte @0x80074E14+8
 *                                                      (3 = HE, 1 = Saeure, 2 = Brand) gegen RE2
 *                                                      Ammo-Recs 0x800A9D10/1C/28 (0x18 Grenade,
 *                                                      0x19 Flame, 0x1A Acid). Zeilen 10/11 sind
 *                                                      ausserdem byte-identisch.
 * 12  Ingram M10 (MP)          15  SMG      7EF0/BURN  IDENTITAET — RE2-Id 15 IST die Ingram-MP
 *                                                      (Dauerfeuer, flags 0x80 @0x800A9E96,
 *                                                      8-Frame-Takt @0x8006A128). Die 7EF0-Zeile ist
 *                                                      genau die "leichte" Reaktion, die eine
 *                                                      Dauerfeuerwaffe braucht.
 * 13  SPAS-12 (dmg 100)         8  Custom Schrot RAGDOLL  die staerkere Schrotflintenzeile
 *                                                      (300/80/60). KLASSEN-IDENTITAET.
 * 14  Flammenwerfer            16  Flammenwerfer MAIN  IDENTITAET — RE2-Id 16 IST der Flammenwerfer
 *                                                      (Munition 0x17 Fuel, flags 0x81
 *                                                      @0x800A9E9E, Dauerfeuer @0x8006A184).
 * 15  GL Explosiv               9  GL Explosiv STAGGER  wie w9/w10/w11 — im RE1.5-Original tragen
 * 16  GL Saeure                11  GL Saeure  STAGGER   Handgranate und GL-Runde derselben Sorte
 * 17  GL Brand                 10  GL Brand   STAGGER   byte-gleiche Schadenszeilen.
 * 18  Rocket Launcher          17  Rakete   (NULL)     IDENTITAET. RE2 17 = 900 = immer toedlich;
 *                                                      RE1.5 w18 = 400 Schaden > jede Zombie-HP
 *                                                      (Tabelle @0x8011F034 max 111, RE2 max 118).
 * 19  H&K MC51 (Vollauto)      18  Gatling  7EF0/BURN  die zweite Dauerfeuerzeile; byte-identisch zu
 *                                                      Zeile 15, damit MP und Sturmgewehr dieselbe
 *                                                      leichte Reaktion tragen. [PORT-ZUORDNUNG]
 * 20  Colt Python (dmg 0)      13  Colt S.A.A. MAIN    KLASSEN-IDENTITAET (Single-Action-Revolver,
 *                                                      RE2-Magazin 6). Im RE1.5-Original unfertig
 *                                                      (Schaden 0 in jeder Gegnerzeile).
 * 21  keine Waffe (tote Zeile)  1  Messer   MAIN       nicht fuehrbar (kein PLW, ARMS-Bank 0);
 *                                                      defensiver Default. [PORT-ZUORDNUNG]
 * NICHT benutzt werden die RE2-Zeilen 6 (Custom Magnum, ebenfalls NULL/900), 12 (Bowgun — RE1.5
 * hat keine Armbrust), 14 (Spark Shot — RE1.5 hat keine Elektrowaffe) und 19 (existiert als
 * Attacken-Id, liegt physisch aber schon in der 1D-Kriecher-Tabelle, s. re2z_hit_tbl).
 */
static const uint8_t re2z_row_from_weapon[22] = {
    /* 0*/  1, /* 1*/  1, /* 2*/  1, /* 3*/  3, /* 4*/  2, /* 5*/  4,
    /* 6*/  4, /* 7*/  5, /* 8*/  7, /* 9*/  9, /*10*/ 11, /*11*/ 10,
    /*12*/ 15, /*13*/  8, /*14*/ 16, /*15*/  9, /*16*/ 11, /*17*/ 10,
    /*18*/ 17, /*19*/ 18, /*20*/ 13, /*21*/  1
};

/* Der zweite Port-Erzeuger von +0x5: re15_enemy_take_damage (FUN_80012D60-Gegner-Zweig) fuettert
 * `+0x5 = re15_react_table[attack_type]` — RE1.5-REAKTIONS-CLIP-Ids (0x03..0x14), wieder ein
 * dritter Id-Raum. Kriterium hier ist die SCHADENSKLASSE aus re15_damage_table @0x8006F418
 * {10,20,1000,1000,1000,50,100,200,300,1000,0}:
 *   Typ 0/1  (10/20, Nahkampf-Angriff eines Gegners) -> RE2 1  (Nahkampfzeile)
 *   Typ 2/3/4/9 (1000 = Instakill)                   -> RE2 17 (900er-Klasse; toetet ohnehin)
 *   Typ 5/6  (50/100, Sprengstoff)                   -> RE2 9
 *   Typ 7    (200)                                   -> RE2 10
 *   Typ 8    (300)                                   -> RE2 11 (RE2-Id 8 traegt genau 300/80/60)
 *   Typ 10   (0)                                     -> RE2 1
 * [PORT-ZUORDNUNG] — im Original gibt es diesen Pfad nicht, dort kommt jede Zeile aus +0x10E. */
static const uint8_t re2z_row_from_atktype[11] = { 1, 1, 17, 17, 17, 9, 9, 10, 11, 17, 1 };

/* INVARIANTE "kein stummer Treffer": faellt die gewaehlte Zeile in der TATSAECHLICH gestempelten
 * Spalte auf NULL, obwohl der Zombie den Treffer UEBERLEBT hat (also die HURT-Wurzel wirklich
 * laeuft), wird auf die schwerste nicht-NULL-Zeile derselben Wucht ausgewichen: 8 = 7438/RAGDOLL.
 * Das kann im Original nicht passieren (die drei NULL-Zeilen gehoeren zu 900-Schaden-Waffen, s.o.);
 * im Port ist es erreichbar, weil der RE1.5-Schaden ein anderer ist (z.B. Zombietyp 0x18 mit der
 * HP-Zeile 1058 aus @0x8011F034 ueberlebt die 400 des Raketenwerfers). [PORT-SICHERUNG] */
enum { RE2Z_ROW_FALLBACK = 8 };
/* (die drei Funktionen stehen direkt hinter re2z_hit_tbl — sie lesen die Tabelle) */

/* Kosten-Tabelle je Zeile, Bytes @0x8010CC33 + (+0x5) — gelesen `lbu v1,-13261(at)` @0x801055D8.
 * Selbst gedumpt 2026-08-18 (`bytes 0x8010cc30 40`):
 *   8010cc30: 88 78 10 80 | 09 0f 0f 23 | 00 00 00 00 | 00 00 00 00
 *   8010cc40: 14 00 00 00 | 00 00 00 00 | 30 85 10 80 …
 * Index 0 faellt auf das letzte Byte des davorstehenden Zeigers 0x80107888 (0x80) und wird nie
 * gelesen (Zeile 0 dispatcht nicht). 0x8010CC40 = Index 13 = 0x14 liegt hinter dem gueltigen
 * Zeilenbereich der Kosten (13..17 sind 0/…), wird aber der Vollstaendigkeit halber gefuehrt. */
static const uint8_t re2z_hit_cost[18] = {
    0,  9, 15, 15, 35,  0,  0,  0,  0,   /* 0..8   */
    0,  0,  0,  0, 20,  0,  0,  0,  0    /* 9..17  ([13] = 0x14 @0x8010CC40) */
};

/* Handler-Id je Tabellenzelle (0 = NULL). 1 = 0x80105438, 2 = 0x80105BC0, 3 = 0x801066FC,
 * 4 = 0x8010703C, 5 = 0x80107438, 6 = 0x80107EF0 — Werte 1:1 aus dem Dump oben. */
enum { RE2ZH_NULL = 0, RE2ZH_MAIN, RE2ZH_STAGGER, RE2ZH_66FC, RE2ZH_703C, RE2ZH_7438, RE2ZH_7EF0 };
static int s_re2z_last_handler = 0;   /* Port-Diagnose: zuletzt dispatchte Zelle (Tests) */
static const uint8_t re2z_hit_tbl[19][9] = {
/* 0*/ { 0,0,0, 0,0,0, 0,0,0 },   /* existiert nicht (Datenwoerter @0x8010C940) */
/* 1*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 2*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 3*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 4*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 5*/ { 0,0,0, 0,0,0, 0,0,0 },
/* 6*/ { 0,0,0, 0,0,0, 0,0,0 },
/* 7*/ { 5,3,0, 5,2,0, 5,1,0 },
/* 8*/ { 5,3,0, 5,2,0, 5,2,0 },
/* 9*/ { 5,2,2, 1,1,1, 1,1,1 },
/*10*/ { 2,2,2, 1,1,1, 1,1,1 },
/*11*/ { 2,2,2, 1,1,1, 1,1,1 },
/*12*/ { 5,4,0, 5,4,0, 5,4,0 },
/*13*/ { 1,1,0, 1,1,0, 1,1,0 },
/*14*/ { 1,1,0, 1,1,0, 1,1,0 },
/*15*/ { 6,6,0, 6,6,0, 6,6,0 },
/*16*/ { 1,1,0, 1,1,0, 1,1,0 },
/*17*/ { 0,0,0, 0,0,0, 0,0,0 },
/*18*/ { 6,6,0, 6,6,0, 6,6,0 }    /* @0x8010CBC8, byte-identisch zu Zeile 15; col8 faellt mit dem
                                   * NULL-Eintrag [0] der 1D-Tabelle @0x8010CBE8 zusammen */
    /* Zeile 19 existiert als Attacken-Id, liegt physisch aber schon in der 1D-Kriecher-Tabelle
     * @0x8010CBE8 (0x8010C940 + 19*36 = 0x8010CBEC = 1D[1] = 0x80107888 in allen 9 Spalten).
     * Der Port stempelt sie deshalb NIE (re2z_row_from_weapon enthaelt keine 19). */
};

/* ---- die Zuordnungs-Funktionen (Tabellen + Kriterien s. Block oben) ------------------------ */
static uint8_t re2z_row_guard(unsigned row, unsigned col, int survived)
{
    if (row == 0u || row > (unsigned)RE2Z_ATK_MAX) row = 1u;      /* nie ausserhalb 1..19 */
    if (!survived) return (uint8_t)row;                           /* DEATH-Wurzel, Zeile egal */
    if (col > 8u) col = 1u;
    if (row < 19u && re2z_hit_tbl[row][col] != RE2ZH_NULL) return (uint8_t)row;
    return (uint8_t)RE2Z_ROW_FALLBACK;                            /* [PORT-SICHERUNG] */
}

uint8_t re15_re2z_row_for_weapon(unsigned weapon_id, unsigned col, int survived)
{
    unsigned row = (weapon_id < 22u) ? re2z_row_from_weapon[weapon_id] : 1u;
    return re2z_row_guard(row, col, survived);
}

uint8_t re15_re2z_row_for_atktype(unsigned attack_type, unsigned col, int survived)
{
    unsigned row = (attack_type < 11u) ? re2z_row_from_atktype[attack_type] : 1u;
    return re2z_row_guard(row, col, survived);
}

/* ============================================================================================
 * PORT-OPTION 2026-08-22 — DIE RE2-STURZREGEL FUER DEN RE1.5-MODUS
 *
 * ⛔ KEIN byte-true RE1.5-Verhalten. NUTZER-ENTSCHEIDUNG (woertlich): "Ja, naja, der fette
 * Zombie muss nicht jeden 2. Schuss umfallen. Das ist unschoen." — dieselbe Klasse bewusster
 * Abweichung wie der bereits bestellte RE2-Schadens-/HP-Import, und wie dieser NICHT geraten,
 * sondern aus RE2 UEBERNOMMEN. Sie haengt am SELBEN Schalter (re15_re15_re2z_import), damit
 * Regel, HP und Schaden nur gemeinsam an- und abschaltbar sind: der HP-Teil der Regel
 * (`slti 81`) rechnet auf der RE2-HP-Skala und waere ohne den HP-Import sinnlos.
 * Mit ABGESCHALTETEM Schalter bleibt der RE1.5-Pfad byte-true unveraendert (PIN
 * test_re15_poise_re2_import.c, Messreihe probe_re15_poise_re2.c).
 *
 * ---- DIE BEIDEN ORIGINAL-REGELN, selbst disassembliert ------------------------------------
 * RE1.5 (STAGE1.BIN roh @0x80100000) — POISE +0x1DC:
 *   80100824 jal 0x8001af20 / 8010082c andi v0,v0,0x3 / 80100838 addiu v0,v0,4
 *   8010083c sh   v0,476(v1)                    ; +0x1DC = (rng&3)+4 = 4..7   (INIT)
 *   Schritt-Tabelle @0x8011FE30 (Zeiger auf Mini-Funktionen), Aufruf @0x80105D28 im
 *   Phase-0-Block des Staggers = EINMAL PRO TREFFER:
 *     [1]/[2]    -> 0x80105A4C `addiu v0,v0,-2`
 *     [3]..[6]   -> 0x80105A6C `addiu v0,v0,-3`     <- die PISTOLE ist Waffe 3
 *     [17]..[20] -> 0x80105A2C `addiu v0,v0,-1`;  Rest 0x80105A24 `jr ra` (0)
 *   Sturz-Tor @0x80105B18-68: `lh v0,476(v0)` @0x80105B24 / `bgez -> return` @0x80105B2C,
 *     sonst `sb 1,4` @0x80105B48 / `sb 0x11,5` @0x80105B58 / `sb 0,6` @0x80105B68 = KNOCKDOWN
 *   Aufstehen Phase[6] @0x80105444-5C laedt wieder (rng&3)+4 nach (Wurf dort konstant 0 -> 4)
 *   => Sturz beim 2. (Poise 4/5) bzw. 3. (6/7) Pistolentreffer, danach ALLE 2 TREFFER.
 *
 * RE2 (EMOVL10_S0.BIN roh @0x80100000) — RESISTENZ +0x223 MIT HP-TOR:
 *   INIT @0x80100888-9C:  jal 0x80015fe8 / `andi v0,v0,0xf` / `addiu v0,v0,16`
 *                         / `sb v0,547(s2)` @0x8010089C          ; +0x223 = 16 + (rand&0xf)
 *   Kosten je Treffer, hit_MAIN P0 @0x801055D0-EC:
 *     801055d0 lui  at,0x8011
 *     801055d4 addu at,at,v0                    ; v0 = +0x5 = Angriffszeile
 *     801055d8 lbu  v1,-13261(at)               ; Tabelle 0x8010CC33 + Zeile
 *     801055e0 subu a0,a0,v1
 *     801055ec sb   a0,547(s1)                  ; +0x223 -= cost[Zeile]
 *     Tabelle 0x8010CC33 = {_,9,15,15,35,0,0,0,0,0,0,0,0,20,0,0,0,0} (re2z_hit_cost, oben);
 *     RE2-Zeile 3 = Browning HP = Kosten 15 (@0x8010CC36).
 *   DAS HP-TOR @0x801055E4-620 — der eigentliche Unterschied zu RE1.5:
 *     801055e4 andi v0,v0,0x40                  ; +0x10E & 0x40
 *     801055e8 bne  v0,zero,0x80105624          ;   gesetzt -> KEIN Nachladen
 *     801055f0 sll  v0,a0,24
 *     801055f4 bgtz v0,0x80105624               ; Leiste noch > 0 -> KEIN Nachladen
 *     801055fc lh   v0,342(s1)                  ; +0x156 = HP
 *     80105604 slti v0,v0,81
 *     80105608 bne  v0,zero,0x80105624          ; HP < 81 -> KEIN Nachladen
 *     80105610 jal  0x80015fe8
 *     80105618 andi v0,v0,0xf
 *     8010561c addiu v0,v0,16
 *     80105620 sb   v0,547(s1)                  ; +0x223 = 16 + (rand&0xf)
 *   Sturz-Tor (HURT-Wurzel) @0x8010503C-AC: Schwelle `+0x10E & 0x40 ? (+0x5!=1 ? 23 : 0) : 0`
 *     (@0x8010503C-58), nur Phase 0 (`lbu v0,6(s1)` @0x8010505C / `bne` @0x80105064),
 *     `lb v0,547(s1)` @0x8010506C / `slt v0,v1,v0` @0x80105074 / `bne -> raus` @0x80105078;
 *     erst dann `sb 1,546` @0x801050A0 und `addiu v0,zero,1281` / `sw v0,4(s1)` @0x801050A4-AC
 *     = 0x501 = EXEC[5] = der STURZ-Executor.
 *
 * ⇒ Solange HP >= 81 laedt RE2 die Leiste bei JEDEM Erschoepfen sofort auf 16..31 nach; das
 *   Sturz-Tor kann also gar nicht feuern. Der RE2-Zombie faellt NUR im Endspiel (HP < 81).
 *   Genau das ist die vom Nutzer gewuenschte Kadenz — und sie ist belegt, nicht erfunden.
 *
 * ---- WAS DER IMPORT AENDERT ----------------------------------------------------------------
 * NUR die ZAHLEN, die in +0x1DC landen. Der MECHANISMUS bleibt der RE1.5-eigene: derselbe
 * Schritt einmal pro Treffer im Stagger-Phase-0-Block, dasselbe `bgez`-Tor @0x80105B2C,
 * dieselbe 0x11-Knockdown-Zeile. Damit bleibt der Rest des RE1.5-Gehirns unberuehrt.
 *   Seed / Neuladung      -> 16 + (rand&0xf)                (@0x8010089C / @0x80105618-20)
 *   Schritt je Treffer    -> -re2z_hit_cost[Zeile]          (@0x801055D8-EC)
 *   NEU: HP-Tor           -> Leiste <= 0 && HP >= 81 -> neu (@0x801055F0-620)
 * Die RE1.5-Waffennummer wird ueber die schon vorhandene, oben belegte Zuordnung
 * re2z_row_from_weapon in die RE2-Angriffszeile uebersetzt (Pistole 3 -> RE2-Zeile 3).
 *
 * +0x10E Bit 0x40 (`andi 0x40` @0x801055E4, das "kein Nachladen"-Bit) hat im RE1.5-Aktor
 * KEIN Gegenstueck; der Import bildet deshalb den Normalfall (Bit geloescht) ab — genau den,
 * den auch der RE2-Modus in ROOM1140 faehrt. NICHT geraten: das Bit ist im RE2-Overlay ein
 * Modifikator einzelner Zombie-Varianten, es hat keinen RE1.5-Erzeuger, den man abbilden
 * koennte, und der Zweig ohne Bit ist der einzige, der beide Regeln vergleichbar macht.
 *
 * RNG: die RE2-Konstante 16+(rand&0xf) zieht hier aus DEM RE2-Generator @0x80015FE8
 * (re15_re2_rand), zu dem sie gehoert. ⚠️ AUSNAHME am INIT-Sitz (enemy_ai_common.c
 * re15_enemy_ai_live_init): dort teilt sich die Neuladung den SCHON GEZOGENEN RE1.5-Wurf, statt
 * einen zusaetzlichen RE2-Wurf zu machen. Grund ist GEMESSEN, nicht stilistisch: am RE2-Strom
 * haengt auch der RE2-INIT-HP-Wurf (Tabelle 0x8010C670, `sh v0,342(s2)` @0x80100708-10), der
 * beim INIT der folgenden Gegner desselben Raums faellt — ein Extra-Wurf verschob ihn und damit
 * die HP (gemessen hp0(0x10) 75 -> 80, hp0(0x16) 65 -> 82). Mit dem geteilten Wurf bleiben BEIDE
 * Stroeme stehen: der RE1.5-Strom ist mit Schalter AN und AUS Wurf fuer Wurf identisch, der
 * RE2-Strom liefert unveraendert dieselben HP. Die drei uebrigen Sitze (Aufstehen,
 * Kontakt-Stagger, Nachladen im Treffer) laufen erst im Kampf, also NACH allen INIT-HP-Wuerfen
 * des Raums, und benutzen deshalb den originalen RE2-Generator.
 * ========================================================================================== */

int re15_re15_re2z_poise_reload(void)
{
    /* `jal 0x80015fe8` / `andi v0,v0,0xf` / `addiu v0,v0,16` — identisch beim INIT
     * @0x80100888-9C, im Nachladen @0x80105610-20 und im Sturz-Tor @0x801050C0-C8. */
    return 16 + (int)(re15_re2_rand() & 0xfu);
}

int re15_re15_re2z_poise_step(int cur, unsigned re15_weapon_id, int hp)
{
    unsigned row  = (re15_weapon_id < 22u) ? re2z_row_from_weapon[re15_weapon_id] : 1u;
    int      cost = (row < 18u) ? (int)re2z_hit_cost[row] : 0;   /* @0x8010CC33 + Zeile */
    int      v    = cur - cost;                                  /* `subu a0,a0,v1` @0x801055E0 */
    if (v <= 0 && hp >= 81)                                      /* `bgtz` @0x801055F4 +
                                                                  * `slti 81`/`bne` @0x80105604-08 */
        v = re15_re15_re2z_poise_reload();                       /* @0x80105610-20 */
    return v;
}

static void re2z_grab_abort(re15_actor_t *e, re15_actor_t *pl)
{
    /* @0x80104F68-FDC (HURT) / @0x801082B0-328 (DEATH): PL-cmd==5 && PL+0x1B4==self ->
     * PL+0x4=1, claim clear (andi 0x7f @0x80104FAC / @0x801082F4), PL+0x1C0&=0xED, both
     * word0 &= ~0x1004. Port: stop the pin + release the victim FSM + drop the latches. */
    if (e->ai_flags & 1u) {
        e->ai_flags &= (uint16_t)~1u;
        e->grab_choreo = 0;                                        /* word0 &= ~0x1004 @0x80104FB8-C4 */
        if (re15_player_victim_state() == 1)                       /* mid-struggle -> free him */
            re15_player_victim_throwoff();
        pl->hit_react &= (uint8_t)~1u;                             /* PL grabbed-flag release */
        /* ⛔ DER EIN-ANGREIFER-RIEGEL Spieler+0x1D3 Bit 0x80 (0x800CFDCB) WURDE NIE GELOESCHT.
         * Byte-gelesen (EMOVL10_S0.BIN):
         *   HURT  80104f94: lbu v0,-565(0x800d)   ; 0x800CFDCB
         *         80104fa0: andi v0,v0,0x7f
         *         80104fac: sb  v0,-565(at)
         *   DEATH 801082dc/801082e8/801082f4  — dieselben drei Instruktionen
         * Der Port bildete diesen Loescher bisher NUR auf pl->hit_react ab (anderes Feld) —
         * pl->re2z_self1d3 blieb gesetzt. Konsequenz (gemessen, s. den Block am Riegel-Setzer
         * in re2z_exec_grab): der Kriecher-Entscheider @0x80102F4C-58 / @0x80103AD4-E0 kehrt
         * danach fuer immer sofort zurueck = "der kriechende Zombie kann nicht angreifen". */
        pl->re2z_self1d3 &= (uint8_t)0x7Fu;                        /* andi 0x7f @0x80104FA0/@0x80104FAC
                                                                    * bzw. @0x801082E8/@0x801082F4 */
    }
}

/* Der gemeinsame Bewegungs-/Pose-Block der Reaktions-Phasen:
 *   FUN_80015558(self, +0x1C4, +0x1C6, 16)   @0x80105714-20 / @0x80105740-4C / @0x80105890-9C
 *   FUN_80015E7C(self, +0x108, +0x17C, 0)    @0x80105580-94 (im Onset zweimal, das erste Mal mit
 *                                             kurzzeitig genulltem +0x14D @0x80105594/@0x801055AC)
 *   FUN_800152C8(self, 0)                    @0x80105B98, GEGATED auf `+0x218 == (u8)+0x14C`
 *                                             (`lbu 536` / `lbu 332` / `bne` @0x80105B84-90).
 * Port-Zwillinge: re15_enemy_steer_point == FUN_80015558, re15_re2z_move_root == das Paar
 * FUN_80015E7C + FUN_800152C8 (so in Welle B etabliert). */
static void re2z_hit_move(re15_actor_t *e)
{
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, 16);         /* @0x80105714-20 */
    if ((uint8_t)e->motion == e->re2z_walkclip)                    /* @0x80105B84-90 */
        re15_re2z_move_root(e);                                    /* @0x80105590 + @0x80105B98 */
}

/* ---- DAS OBERKOERPER-ZUCKEN: die Bone-Matrix-Injektion ------------------------------------
 * Setzt den Lean-SVECTOR, den re15_skel_compute_pose (skeleton_common.c) in DIESEM Frame auf
 * Bone 0 (nach-multipliziert) und Bone 1 (vor-multipliziert, transponiert) legt. Der Aufrufer
 * setzt ihn pro Tick neu; re15_re2z_tick loescht ihn am Tick-Anfang, damit nichts akkumuliert
 * (das Original baut die Part-Matrizen im Advance jeden Tick neu).
 * Fundstellen: @0x801057A4-838 (P1), @0x801058D4-960 (P2), @0x80106A04-3C / @0x80106CA0-D4
 * (Ragdoll). Details + Ableitung stehen am Feld re2_lean in re15_actor.h. */
static void re2z_lean_pair(re15_actor_t *e, int vx, int vy, int vz)
{
    e->re2_lean[0] = (int16_t)vx;
    e->re2_lean[1] = (int16_t)vy;
    e->re2_lean[2] = (int16_t)vz;
    e->re2_lean_on = 1;
}

/* Der Zuck-Winkel der Reaktions-Phasen. `shift3` = der zusaetzliche <<3, den NUR P1 traegt:
 *   P1 @0x801057B0-C4 : mult +0x158,(s8)+0x16B -> `sll v0,t0,3` -> `subu v0,zero,v0` -> * +0x15A
 *   P2 @0x801058E0-F0 : dieselbe Kette OHNE das `sll` (selbst nachdisassembliert 2026-08-18)
 * Die zweite Matrix ist exakt der Gegenwinkel (@0x801057EC-838 / @0x80105918-960) = Transposition. */
static int re2z_lean_angle(const re15_actor_t *e, int shift3)
{
    int prod = (int)e->re2z_t158 * (int)(int8_t)e->re2z_gaitrow;   /* +0x158 * (s8)+0x16B */
    if (shift3) prod <<= 3;                                        /* `sll v0,t0,3` @0x801057BC */
    return -(prod * (int)e->re2z_t15a);                            /* * +0x15A, negiert */
}

/* ============================================================================================
 * WELLE E — DER ZERLEGER-/GORE-ZWEIG. Vollstaendig disassembliert 2026-08-18 aus
 * info/re2leon/COMMON/BIN/EMOVL10_S0.BIN (RAW @0x80100000) + info/re2leon/PSX.EXE.
 *
 * ---- DER MODELLBLOCK +0x198 ---------------------------------------------------------------
 * Ein Array von 172-Byte-Part-Records (Stride 0xAC). Beleg: die Offsetleiter der Tinten-Schreiber
 * 112/284/456/972/1488/1660/1832/2004/2176/2348/2520 in FUN_80106128 (@0x8010627C-F4) — alle
 * exakt `n*172 + 112`; die Emitter-Anker `n*172 + 72` (@0x80106178 s1+72, @0x801061AC +1448 = 8,
 * @0x801061E0 +588 = 3, @0x80106250 +1104 = 6); und `sw zero,2580` = Part 15 (@0x801010DC).
 * Gelesene/geschriebene Felder: +0x00 Flag-Wort (Bit 0 = Part vorhanden, `lw v0,516(a2)` /
 * `andi 1` @0x801061C0-CC), +0x48 Position (Emitter-Anker), +0x70 Farbwort.
 *
 * ---- DIE PART-INDIZES = die klassische RE-Knochenreihenfolge -------------------------------
 * Gemessen an der geladenen RE2-Bank EM010 (probe_re2_gore): 15 Knochen, Elternkette
 * 0<-(-1) 1<-0 2<-1 3<-2 4<-3 5<-1 6<-5 7<-6 8<-1 9<-0 10<-9 11<-10 12<-0 13<-12 14<-13.
 * Zusammen mit dem im Port bereits belegten "Kopf = Bone 8" (re15_skel_compute_pose /
 * Plc_neck, re15_actor.h) ergibt das:
 *   0 Huefte | 1 Brust | 2 R-Oberarm | 3 R-Unterarm | 4 R-Hand | 5 L-Oberarm | 6 L-Unterarm
 *   7 L-Hand | 8 Kopf | 9 R-Oberschenkel | 10 R-Schienbein | 11 R-Fuss | 12 L-Oberschenkel
 *   13 L-Schienbein | 14 L-Fuss     (15 = der Reserve-/Stumpf-Record, @0x801010DC genullt)
 *
 * ---- WER RUFT WAS (die Zeile +0x5 = die RE2-Waffen-Id) --------------------------------------
 * Liegend-Zweig @0x80105188-284 (im HURT, `+0x21A & 2`), Haupt-Handler P0 @0x80105520-78,
 * Haupt-Handler P3 @0x80105A58-64, Stagger-P0 @0x80105DC4-F18, DEATH @0x80108444-4D4:
 *   Zeile  9 (GL Explosiv)  -> FUN_8010640C  (Russ), Gate `!(+0x10E & 0x80)`
 *   Zeile 10 (GL Brand)     -> FUN_80106128  (Verkohlung), Gate `!(+0x10E & 0x80)`, danach
 *                              `+0x21A |= 0x800` (nur im Overlay-HURT, nicht im DEATH)
 *   Zeile 11 (GL Saeure)    -> FUN_80106310  (Aetzung), KEIN Gate
 *   Zeile 14 (Spark Shot)   -> FUN_80106510  (Elektro), KEIN Gate
 *   Zeile 16 (Flammenwerfer)-> +0x23A-Zaehler; ab dem 10. Treffer (`sltiu v0,v0,0x9`
 *                              @0x80105250/@0x80105544) FUN_80106128 + `+0x21A |= 0x800`
 *   Zeile 17 (Rakete)       -> FUN_8010640C  (nur im DEATH-Zweig @0x80108490-B8)
 *
 * ---- FUN_8001BF10 / FUN_8001CEFC ------------------------------------------------------------
 * FUN_8001BF10(a0 = gepackte Effekt-Id, a1 = +0x76, a2 = &Part+0x48, a3 = &Geschwindigkeit|0)
 * belegt einen der 96 Effekt-Slots @0x800D8CF0 (Stride 124, Suche @0x8001BF70-8C).
 * FUN_8001CEFC(gruppe, sub, anker) ist das Gegenstueck: es LOESCHT alle Slots, deren
 * (gruppe,sub,anker) passen (`lbu v0,28(v1)` / `lbu v0,30(v1)` / `lw v0,108(v1)` /
 * `sb zero,0(v1)` @0x8001CF14-4C) — "die am abgerissenen Teil haengenden Effekte abschalten".
 * Der Port hat weder gepackte RE2-Gruppen noch ankergebundene Effekte: [PORT-MAPPING] je
 * Original-Emitter EIN RE1.5-Raumbank-Spawn (wie schon im Ragdoll/Rutsch), FUN_8001CEFC = No-op.
 * Die RNG-WURFZAHL ist Verhalten und wird exakt nachgezogen.
 * ========================================================================================== */

/* [PORT-MAPPING] FUN_8001BF10 — Stand-in, Id nur dokumentiert (s. Block oben). `part` ist der
 * ANKER (a2 = +0x198 + part*172 + 72); jede Fundstelle traegt ihr `@0x…` am Aufruf. */
static void re2z_gore_fx(re15_actor_t *e, int part, uint32_t packed_id)
{
    (void)packed_id;
    re2z_blood_fx_at(e, part, (int16_t)e->rot_y);
}

/* Der Modellblock beim INIT: Bit 0 ("Part vorhanden") fuer die 15 echten Parts, Part 15 leer
 * (`sw zero,2580(v0)` @0x801010DC nullt ihn im Original in jedem ACTIVE-Frame). [PORT-MAPPING]
 * fuer den Anfangswert — im Original kommt er aus dem Modell-Loader. */
static void re15_re2z_part_reset(re15_actor_t *e)
{
    /* Die Flug-/Wurf-Felder des Records (+0x38..+0xA4) und die eingefrorene Matrix (+0x48):
     * im Original baut FUN_80028368 den Block komplett neu auf, im Port ist das dieser Reset. */
    memset(e->re2z_part_v,    0, sizeof e->re2z_part_v);
    memset(e->re2z_part_rot,  0, sizeof e->re2z_part_rot);
    memset(e->re2z_part_m,    0, sizeof e->re2z_part_m);
    memset(e->re2z_part_t,    0, sizeof e->re2z_part_t);
    memset(e->re2z_part_grav, 0, sizeof e->re2z_part_grav);
    memset(e->re2z_part_st86, 0, sizeof e->re2z_part_st86);
    memset(e->re2z_part_yaw98,0, sizeof e->re2z_part_yaw98);
    memset(e->re2z_part_w9a,  0, sizeof e->re2z_part_w9a);
    memset(e->re2z_part_w9c,  0, sizeof e->re2z_part_w9c);
    memset(e->re2z_part_w9e,  0, sizeof e->re2z_part_w9e);
    memset(e->re2z_part_life, 0, sizeof e->re2z_part_life);
    memset(e->re2z_part_wa4,  0, sizeof e->re2z_part_wa4);
    for (int i = 0; i < 16; i++) e->re2z_part_blend[i] = -1;   /* +0x7A: -1 = keine Setz-Blende
                                                                * (`bltz` @0x80028D2C) */
    e->re2z_part_seeded  = 0u;
    e->re2z_part_stepped = 0u;
    e->re2z_part_frame   = 0u;
    for (int i = 0; i < 16; i++) {
        e->re2z_part_flags[i] = (i < 15) ? 1u : 0u;
        /* Farbwort-Seed: der Modell-Aufbau schreibt in JEDEN Part `+0x70 = 0x808080`
         * (`puVar8[-9] = 0x808080;` FUN_80028368 in RE2_Quellcode_V2/FUN_80028368.c:55,
         * puVar8 laeuft mit Wort-Stride 0x2b = 0xAC). 0x80 ist der PSX-neutrale
         * Modulationswert (GPU: final = tex*prim/0x80) — die Tinte MULTIPLIZIERT also,
         * sie ersetzt nicht. Frueher stand hier 0 = "keine Tinte" (Port-Sentinel);
         * mit dem Renderer-Konsumenten ist der Original-Seed die richtige Zahl. */
        e->re2z_part_tint[i]  = 0x00808080u;
        e->re2z_part_mesh[i]  = (uint8_t)i;     /* Geometrie-Zeiger zeigen initial auf den
                                                 * eigenen Part (FUN_80028368-Aufbau) */
    }
}

/* ============================================================================================
 * DIE ANZEIGE-BRUECKE DES MODELLBLOCKS  (der bisher fehlende Konsument von +0x198)
 * --------------------------------------------------------------------------------------------
 * Original: FUN_80027160 @0x80027160 (RE2-EXE, ghidra_re2_Leon.txt Z.108178-108432) ist der
 * Part-Draw-Walk. Aufruf-Konvention aus dem Aufrufer @0x80026894 belegt:
 *     80026894: lw    a1,0x198(s0)     ; a1 = Part-Array (16 Records, Stride 0xAC)
 *     80026898: lw    a2,0x0(s0)       ; a2 = Entity-Flagwort
 *     8002689c: jal   FUN_80027160
 *     800268a0: _addiu a3,s0,0x38      ; a3 = &Entity-Position
 * Part-Zahl aus `lbu s3,0x107(s1)` @0x80027354.
 *
 * (A) SICHTBARKEIT — Bit 0 des Flagworts +0x00 ist DRAW ENABLE, der Test steht in BEIDEN
 *     Schleifen und ist ein FLACHES `continue`, KEIN Baum-Schnitt:
 *     einfache Schleife                       Hauptschleife
 *       80027374: lw   a2,0x0(s2)               800273b4: lw   a2,0x0(s2)
 *       8002737c: andi v0,a2,0x1                800273bc: andi v0,a2,0x9000
 *       80027380: beq  v0,zero,0x80027394       800273c0: beq  v0,zero,0x800273e4
 *       8002738c: jal  FUN_80027434             800273c4: _andi v0,a2,0x1
 *       80027390: _addiu a3,s2,0x48             800273c8: beq  v0,zero,0x800273f8
 *       800273a4: _addiu s2,s2,0xac             800273f8: addiu s0,s0,0xac
 *                                               8002740c: _addiu s2,s2,0xac
 *     Der Record-Zeiger wird UNBEDINGT im Delay-Slot weitergeschoben (0x800273A4 /
 *     0x800273F8 / 0x8010740C) — es gibt keinen Matrix-Stack, kein Push/Pop, keine Rekursion.
 *     Ein geloeschtes Bit 0 nimmt also NICHT automatisch die Knochenkette darunter mit;
 *     die Kinder wuerden mit der (eingefrorenen) Elternmatrix weiterzeichnen.
 *
 * (A2) DIE KETTE gibt es trotzdem — als FLAG-KASKADE im Kopf BEIDER Zeichner
 *     (FUN_80027434 @0x80027470-94, wortgleich in FUN_80027ff0 @0x80028010-34):
 *       80027470: lw   v0,0x94(s1)     ; +0x94 = Zeiger auf den ELTERN-Record
 *       80027478: lw   v0,0x0(v0)      ; Eltern-Flagwort
 *       80027480: andi v1,v0,0x21      ; Bit 0 und Bit 5 isolieren
 *       80027484: li   v0,0x20
 *       80027488: bne  v1,v0,0x80027498; Eltern != (0x20 gesetzt UND 0x01 klar) -> normal
 *       80027490: j    0x80027bb8      ; sonst: Epilog = NICHTS zeichnen
 *       80027494: _sw  v1,0x0(s1)      ; und das EIGENE Flagwort wird 0x20
 *     Weil FUN_80028368 die Records ELTERN-VOR-KIND ablegt (`child[+0x74] = &parent+0x48`,
 *     `child[+0x94] = &parent` FUN_80028368.c:106-109), pflanzt sich die 0x20 im selben
 *     flachen Durchlauf durch die ganze noch aktive Kette fort. Ein Kind, dessen Bit 0
 *     bereits klar ist, wird nie betreten und gibt die 0x20 auch nicht weiter.
 *     -> abgerissener Oberschenkel nimmt Unterschenkel+Fuss NUR mit, wenn er 0x20 traegt
 *        UND sein Bit 0 geloescht ist (das tut im Original die Flugphysik FUN_80028ad8 beim
 *        Aufschlag: `*param_1 = *param_1 & 0xfffffffe`, FUN_80028ad8.c:52).
 *
 * (B) TINTE — das Farbwort +0x70 geht als CVECTOR in das GTE-RGB-Register und wird von
 *     NCCT mit dem BELEUCHTUNGSERGEBNIS MULTIPLIZIERT (nicht ersetzt):
 *       80027900: lw   s8,0x70(s1)     ; Farbwort
 *       80027ae0: move a2,s8           ; -> FUN_80027bec (Tris)  / @0x80027AFC -> FUN_80027dbc (Quads)
 *       80027c08: sw   a2,0x10(sp)     ; CVECTOR r,g,b   (LOW BYTE = R)
 *       80027c18: sb   a3,0x13(sp)     ; CVECTOR.cd = GPU-Primitiv-Code -> Byte 3 ist NICHT Farbe
 *       80027c2c: ldrgb v0             ; GTE RGB-Register
 *       80027d10: NCCT                 ; out = RGB_reg * (BK + LCM*LLM*N)
 *     Neutral ist 0x808080 (FUN_80028368.c:55) — genau der PSX-Modulationsneutralwert.
 *     Der Port rendert die Beleuchtung in DEMSELBEN Raum (render_pc.c
 *     psx_prim_to_sdl_vert: "final = (tex x prim) / 0x80", Quelle psx-spx), die Tinte ist
 *     also `prim' = prim * tint / 0x80` mit Saettigung — genau ein NCCT-Modulationsschritt.
 *
 * ⛔ NUR RE2: die Bruecke wird ausschliesslich betreten, wenn der RE2-Flavor aktiv IST, der
 *    RE2-Zombie-Brain den Typ besitzt UND der INIT-Seed gelaufen ist. Im RE1.5-Pfad liefert
 *    re15_re2z_gore_resolve() 0 und der Renderer laeuft unveraendert weiter.
 *
 * (C) DAS FREIFLIEGENDE TEIL — seit Welle G verdrahtet. Parts mit 0x4A (Arm) bzw. 0x1062 (Bein)
 *    behalten Bit 0, zeichnen also weiter, bekommen aber ueber Bit 0x40 eine EIGENE Matrix
 *    (`andi v0,s3,0x40` @0x80027498) und eine eigene Physik (Bit 0x08 -> FUN_80028DAC
 *    @0x80027B98, Bit 0x20 -> FUN_80028AD8 @0x800276A0). Beide sind unten byte-true portiert;
 *    der Einstieg ist re15_re2z_gore_part_matrix(). Damit feuert auch der KASKADEN-AUSLOESER
 *    echt: FUN_80028AD8 loescht beim zweiten Bodenkontakt Bit 0 (`and v0,v0,-2` @0x80028CA0),
 *    das Flagwort des Schienbeins bleibt 0x1062 -> `(Eltern & 0x21) == 0x20` -> der Fuss
 *    verschwindet mit. */
/* ============================================================================================
 * ⛔ DIE PART-NUMMERIERUNG IM RE1.5-MODUS  (Messung 2026-08-20, probe_tmp_rigdump)
 * --------------------------------------------------------------------------------------------
 * Der Modellblock +0x198 ist in RE2-PART-Nummern indiziert. Im RE2-Modus ist die geladene Bank
 * das RE2-Rig, also Part == Bone-Slot. Im RE1.5-Modus ist die geladene Bank das RE1.5-Rig —
 * und das hat eine ANDERE Reihenfolge. SELBST GEMESSEN aus den ausgelieferten Banken
 * (Bind-Offsets, PSX-Konvention -y = oben):
 *   RE1.5 EM010: 0 Wurzel | 1,2,3 Gliedmasse nach UNTEN (228/664/783) | 4,5,6 dito
 *                7 Torso (0,0,0) | 8,9,10 Gliedmasse nach OBEN/seitlich (-550/-399)
 *                | 11,12,13 dito | 14 mittig oben (-667), groesstes Mesh
 *   RE2   EM010: 0 Wurzel | 1 (0,0,0) | 2,3,4 nach UNTEN (252/730/861) | 5,6,7 dito
 *                8 mittig oben (-783) | 9,10,11 nach OBEN/seitlich (-605/-421) | 12,13,14 dito
 * Die Umrechnung ist damit GENAU die Hybrid-Permutation, die der Port fuer den Modus "RE2 AI"
 * (RE2-Gehirn + RE1.5-Modelle) bereits fuehrt und die dort vom Nutzer visuell abgenommen ist:
 * k_perm_zombie / k_perm_zgirl in re2_ems.c (re2_hybrid_perm), RE2-Slot -> RE1.5-Index.
 * Sie hier wiederzuverwenden ist der einzige Weg, der GARANTIERT dasselbe sichtbare Teil
 * abreissen laesst wie im Modus "RE2 AI" — und genau das hat der Nutzer verlangt.
 *
 * ⚠ OFFENER BEFUND (RE2-Pfad, ABSICHTLICH HIER NICHT ANGEFASST): die Part-BENENNUNG im
 *   Welle-E-Block weiter unten ("9 R-Oberschenkel | 10 R-Schienbein | 11 R-Fuss") passt NICHT
 *   zu diesen Bind-Offsets — RE2-Bone 9/12 liegen 605 Einheiten OBERHALB der Wurzel, sind also
 *   Schultern. Auch die dort notierte Elternkette weicht ab (gemessen 8<-0, notiert 8<-1). Der
 *   MECHANISMUS (welcher Part-Index welche Flags bekommt) ist davon unberuehrt und bleibt
 *   byte-true; nur die deutschen Koerperteil-NAMEN im Kommentar sind fraglich. Das ist ein
 *   RE2-Thema und wird gemeldet, nicht nebenbei umgeschrieben.
 * ========================================================================================== */
static int re2z_perm_for(const re15_actor_t *e, const int8_t **out_perm)
{
    if (!re15_re15_re2z_import_owns(e->type)) { if (out_perm) *out_perm = NULL; return 0; }
    int n = re2_hybrid_perm((int)e->type, out_perm);
    if (n <= 0 || !out_perm || !*out_perm) { if (out_perm) *out_perm = NULL; return 0; }
    return n;
}

/* RE2-Part -> Bone-Slot der GELADENEN Bank (Identitaet ausserhalb des RE1.5-Imports). */
static int re2z_part_to_bone(const re15_actor_t *e, int part)
{
    const int8_t *perm = NULL;
    int n = re2z_perm_for(e, &perm);
    if (!n || part < 0 || part >= n) return part;
    int b = (int)perm[part];
    return (b >= 0) ? b : part;
}

/* Bone-Slot der geladenen Bank -> RE2-Part (die Umkehrung). */
static int re2z_bone_to_part(const re15_actor_t *e, int bone)
{
    const int8_t *perm = NULL;
    int n = re2z_perm_for(e, &perm);
    if (!n) return bone;
    for (int i = 0; i < n; i++) if ((int)perm[i] == bone) return i;
    return -1;                                     /* Slot ohne RE2-Gegenstueck */
}

/* ⛔ 2026-08-20 — DAS FLAVOR-GATE IST JETZT ZWEIWERTIG (Nutzer-Auftrag, Block bei
 * re15_re15_re2z_import): im RE1.5-Modus darf die Bruecke ebenfalls aktiv werden, ABER NUR
 *   (a) mit eingeschalteter Port-Option re15_re15_re2z_import() und
 *   (b) fuer die Zombie-Familie und
 *   (c) nachdem der Modellblock geseedet ist — und im RE1.5-Modus seedet ihn AUSSCHLIESSLICH
 *       re15_re15_re2z_gore_hit(), also erst beim ERSTEN Treffer.
 * Bis dahin ist re2z_part_flags[0] == 0 (Aktor-memset) und die Funktion liefert wie bisher 0,
 * d.h. der Renderpfad eines nie getroffenen RE1.5-Zombies bleibt Byte fuer Byte der alte.
 * Mit RE15_RE15_RE2Z_IMPORT=0 ist der RE1.5-Pfad in JEDEM Zustand wieder komplett stumm —
 * genau darauf pinnen test_re2_gore_render.c PIN 1 und test_re2_gore_fly.c PIN 1. */
int re15_re2z_gore_active(const re15_actor_t *e)
{
    if (!e) return 0;
    if (!re15_re2z_owns_type(e->type)) return 0;
    if (!re15_ai_re2_for_type(e->type) &&
        !re15_re15_re2z_import_owns(e->type)) return 0;
    return (e->re2z_part_flags[0] & 1u) != 0;                  /* INIT-Seed (part_reset) gelaufen */
}

int re15_re2z_gore_resolve(const re15_actor_t *e, const int8_t *bone_parent, int n,
                           uint8_t *out_draw, uint32_t *out_tint, uint8_t *out_mesh)
{
    if (!re15_re2z_gore_active(e) || !out_draw || !out_tint || !out_mesh) return 0;
    if (n > 16) n = 16;                                        /* Modellblock hat 16 Records */

    /* Der Aufrufer indiziert mit BONE-SLOTS der geladenen Bank; der Modellblock steht in
     * RE2-PART-Nummern. Im RE2-Modus ist das dasselbe, im RE1.5-Modus uebersetzt die
     * Hybrid-Permutation (Block ueber re2z_part_to_bone). Der Walk selbst laeuft danach
     * unveraendert ueber die Elternkette DER GELADENEN BANK — die Permutation ist
     * kantentreu (re2_ems.c re2_hybrid_rig_skel pinnt das mit `unmapped == 0`). */
    uint32_t fl[16];
    uint8_t  msh[16];
    for (int i = 0; i < n; i++) {
        int p = re2z_bone_to_part(e, i);
        if (p < 0 || p >= 16) {                                /* Slot ohne RE2-Part (Hundepfoten
                                                                * u.ae.) -> unveraendert zeichnen */
            fl[i] = 1u; out_tint[i] = 0x00808080u; msh[i] = (uint8_t)i;
            continue;
        }
        fl[i]       = e->re2z_part_flags[p];
        out_tint[i] = e->re2z_part_tint[p];
        /* Der Geometrie-Zeiger ist ebenfalls eine PART-Nummer: zeigt er auf sich selbst, ist
         * es der eigene Bank-Slot; der Stumpf-Tausch @0x8010531C-50 setzt 15 (der RESERVE-Part
         * des RE2-MD1) — der bleibt 15 und faellt im Renderer aus der Mesh-Klammer. */
        uint8_t m = e->re2z_part_mesh[p];
        msh[i] = (m == (uint8_t)p) ? (uint8_t)i : m;
    }

    for (int i = 0; i < n; i++) {                              /* flacher Walk, Stride 0xAC */
        out_mesh[i] = msh[i];                                  /* Stumpf-Tausch @0x8010531C-50 */
        if (!(fl[i] & 1u)) { out_draw[i] = 0; continue; }      /* andi 0x1 @0x8002737C/@0x800273C4 */
        int p = bone_parent ? (int)bone_parent[i] : -1;
        if (p >= 0 && p < n && (fl[p] & 0x21u) == 0x20u) {     /* andi 0x21 / li 0x20 @0x80027480-88 */
            fl[i] = 0x20u;                                     /* sw v1,0(s1) @0x80027494 */
            out_draw[i] = 0;                                   /* j 0x80027bb8 = Epilog */
            continue;
        }
        out_draw[i] = 1;
    }
    return 1;
}

/* ============================================================================================
 * DAS FREIFLIEGENDE TEIL — die zwei Part-Physiken (Welle G)
 * Vollstaendig disassembliert 2026-08-18:
 *   FUN_80028AD8  0x80028AD8..0x80028DA8   Wurf mit Aufschlag   (Bein, Flagwort 0x1062)
 *   FUN_80028DAC  0x80028DAC..0x80028EA0   Drift mit Ablauf     (Arm,  Flagwort 0x4A)
 *   FUN_80028F48  0x80028F48..0x8002910C   die Setz-Blende der Matrix (nur im Bein-Zweig)
 *   MatrixNormal_0 @0x8008CE30 (OuterProduct0 @0x8008DF78, VectorNormal @0x8008D424)
 *
 * ---- WER RUFT SIE, UND MIT WELCHER MATRIX ---------------------------------------------------
 * Beide haengen im ZEICHNER FUN_80027434, nicht in der KI:
 *   80027498: andi v0,s3,0x40        ; Flagbit 0x40 = "eigene Matrix"
 *   8002749c: bne  v0,zero,0x800275e4; -> Eltern-Verkettung UEBERSPRINGEN
 *   80027694: andi v0,s3,0x20
 *   800276a0: jal  0x80028ad8        ; (a0 = Record, a1 = Matrix)
 *   80027b98: jal  0x80028dac        ; nur erreichbar, wenn `param_3 & 0x18` != 0 (sonst
 *                                    ; return @0x80027... im 0x18==0-Zweig) -> Bit 0x08
 * Die Matrix IST der Record selbst: der Walk uebergibt `_addiu a3,s2,0x48` @0x80027390 bzw.
 * fuehrt `addiu s0,s2,72` @0x80027370/@0x800273B0 mit `addiu s0,s0,172` @0x800273F8 im
 * Gleichtakt zu `addiu s2,s2,172` @0x8002740C. rec+0x48 = MATRIX{ short m[3][3]; long t[3]; },
 * also m @+0x48..+0x59 und t @+0x5C/+0x60/+0x64 — genau die Offsets, die FUN_80028AD8 ueber
 * `param_2+20/24/28` und FUN_80028DAC ueber `param_1[0x17]/[0x18]/[0x19]` anfasst.
 *
 * ---- [PORT-MAPPING], je einzeln benannt ------------------------------------------------------
 *  (1) BODEN. `FUN_8004FBA0(rec+0x5C,100,0x2000,1)` @0x80028B74 ist die Raum-Boden-Sonde AN DER
 *      POSITION DES TEILS. Der Port hat in diesem Modul keine Punkt-Sonde und nimmt die
 *      Boden-Y des Aktors (e->y) — dieselbe Ersetzung, die der Ragdoll/Rutsch-Zweig mit
 *      +0x1C2/+0x232 schon macht. Fuer ein Teil, das <= 100 Einheiten weit fliegt, ist das
 *      dieselbe Ebene; auf einer Treppe waere es eine Abweichung. OFFEN, mit Adresse.
 *  (2) WAND. `FUN_8004C1BC(rec+0x5C,100,1<<(-(Y/1800)&0x1f),0x2000)` @0x80028CDC prallt das Teil
 *      an der Raumgeometrie ab (`v>>2`, dann Vorzeichenwechsel @0x80028CEC-D20). Kein
 *      Port-Zwilling -> der Zweig ist NICHT modelliert. OFFEN, mit Adresse.
 *  (3) ROTATION. `RotMatrix` @0x8008E1F4 (PsyQ) wird durch den byte-true Port-Builder
 *      mat3_from_euler (skeleton_common.c, Zwilling von FUN_8001F3BC) ersetzt — dieselbe
 *      Trig-Tabelle, dieselbe Q12-Kette, die der ganze Port fuer Knochenmatrizen benutzt.
 *  (4) MatrixNormal_0 @0x8008CE30 ist strukturell nachgebaut (r2 = r0 x r1 @0x8008CE8C,
 *      r0 = r1 x r2 @0x8008CE9C, dann VectorNormal auf alle drei Zeilen @0x8008CEA8/B4/C0).
 *      Der Zeilen-Normalisierer ist re15_vector_normal — der bereits vorhandene byte-true
 *      libgte-VectorNormal-Zwilling (RE1.5-Link-Adresse 0x80066A30, RE2 0x8008D424, dieselbe
 *      Bibliotheksroutine). Die GTE-interne Rundung des OP-Befehls ist damit nicht bit-exakt
 *      reproduziert; das ist eine PRAEZISIONS-, keine Mechanismus-Abweichung.
 *  (5) SKALIERUNG. Der `flags & 0x10`-Zweig @0x80028C54-80 schreibt zusaetzlich +0x8C/+0x90 =
 *      7000 und +0x8E = 0 — die drei Skalierungs-Halbwoerter, die der Zeichner ueber
 *      `param_3 & 0x400` / `& 0x2000` in ScaleMatrix schiebt. Der Port hat keine Part-Skalierung;
 *      der Zweig setzt hier nur die belegten Flag-/Zustandsfelder. OFFEN, mit Adresse.
 * ========================================================================================== */

/* RotMatrix-Zwilling (mat3_from_euler in skeleton_common.c). */
extern void re15_skel_euler_matrix_for_test(int ax, int ay, int az, int32_t m[9]);

/* FUN_80028F48 @0x80028F48 + MatrixNormal_0 @0x8008CE30 — die Setz-Blende:
 *   80028f4c: addiu v1,zero,4096
 *   80028f50: subu  v1,v1,a3          ; w = 4096 - t   auf die ALTE Matrix
 *   80028f58: mtc2  v1,IR0 / ... / gpf12 (0x4b98003d)   ; IR = w * M
 *   80028f80: mtc2  a3,IR0 / ... / gpl12 (0x4ba8003e)   ; IR += t * R
 * elementweise ueber alle NEUN Kurzwoerter (Bloecke @0x80028F5C-FBC, @0x80028FC4-9030,
 * @0x80029034-90E4), danach `jal 0x8008ce30` @0x80029108. */
static void re2z_part_settle(re15_actor_t *e, int i)
{
    int32_t R[9];
    re15_skel_euler_matrix_for_test((int)e->re2z_part_rot[i][0],      /* +0x3E @0x80028D34 */
                                    (int)e->re2z_part_rot[i][1],      /* +0x40 @0x80028D40 */
                                    (int)e->re2z_part_rot[i][2], R);  /* +0x42 @0x80028D4C */
    int32_t t = 4096 - ((int32_t)e->re2z_part_blend[i] << 8);         /* 4096 - c*256
                                                                       * @0x80028D6C-78 */
    int32_t w = 4096 - t;                                             /* @0x80028F4C-50 */
    int16_t *m = e->re2z_part_m[i];
    for (int k = 0; k < 9; k++)
        m[k] = (int16_t)((((int32_t)m[k] * w) + (R[k] * t)) >> 12);   /* gpf12 + gpl12 */

    /* MatrixNormal_0 @0x8008CE30 */
    int32_t r0[3] = { m[0], m[1], m[2] };
    int32_t r1[3] = { m[3], m[4], m[5] };
    int32_t r2[3];
    r2[0] = (r0[1]*r1[2] - r0[2]*r1[1]) >> 12;      /* OuterProduct0(r0,r1,r2) @0x8008CE8C, */
    r2[1] = (r0[2]*r1[0] - r0[0]*r1[2]) >> 12;      /* GTE OP mit sf=1 (0x4b78000c) */
    r2[2] = (r0[0]*r1[1] - r0[1]*r1[0]) >> 12;
    r0[0] = (r1[1]*r2[2] - r1[2]*r2[1]) >> 12;      /* OuterProduct0(r1,r2,r0) @0x8008CE9C */
    r0[1] = (r1[2]*r2[0] - r1[0]*r2[2]) >> 12;
    r0[2] = (r1[0]*r2[1] - r1[1]*r2[0]) >> 12;
    re15_vector_normal(r0[0], r0[1], r0[2], r0);    /* @0x8008CEA8 */
    re15_vector_normal(r1[0], r1[1], r1[2], r1);    /* @0x8008CEB4 */
    re15_vector_normal(r2[0], r2[1], r2[2], r2);    /* @0x8008CEC0 */
    m[0] = (int16_t)r0[0]; m[1] = (int16_t)r0[1]; m[2] = (int16_t)r0[2];  /* @0x8008CED0-E8 */
    m[3] = (int16_t)r1[0]; m[4] = (int16_t)r1[1]; m[5] = (int16_t)r1[2];  /* @0x8008CEF4-F00 */
    m[6] = (int16_t)r2[0]; m[7] = (int16_t)r2[1]; m[8] = (int16_t)r2[2];  /* @0x8008CF18-30 */

    e->re2z_part_blend[i] = (int8_t)(e->re2z_part_blend[i] - 1);      /* @0x80028D7C-88 */
}

/* FUN_80028AD8 — Wurf mit Aufschlag (Flagbit 0x20). */
static void re2z_part_phys_ad8(re15_actor_t *e, int i)
{
    int16_t *v = e->re2z_part_v[i];
    int32_t *t = e->re2z_part_t[i];

    if (e->re2z_part_st86[i] == 0) {                           /* lh v0,134 @0x80028AF4 */
        /* RotMatrix((0, +0x38, 0)) @0x80028B10-24, dann ApplyMatrixSV((+0x3C, +0x3A, 0))
         * mit dem Ergebnis ZURUECK nach +0x38/+0x3A/+0x3C (`addiu a2,s1,56` @0x80028B3C). */
        int32_t M[9];
        re15_skel_euler_matrix_for_test(0, (int)v[0], 0, M);
        int32_t ix = v[2], iy = v[1];                          /* vz bleibt 0 @0x80028B18 */
        int32_t ox = (M[0]*ix + M[1]*iy) >> 12;
        int32_t oy = (M[3]*ix + M[4]*iy) >> 12;
        int32_t oz = (M[6]*ix + M[7]*iy) >> 12;
        v[0] = (int16_t)ox; v[1] = (int16_t)oy; v[2] = (int16_t)oz;
        e->re2z_part_st86[i]  = 3;                             /* sh 3,134  @0x80028B48-4C */
        e->re2z_part_blend[i] = 15;                            /* sb 15,122 @0x80028B50-54 */
    }

    if (e->re2z_part_st86[i] >= 2) {                           /* slti v0,v0,2 @0x80028B60 */
        int32_t floor_y = (int32_t)e->y;                       /* [PORT (1)] @0x80028B74 */
        t[0] += v[0];                                          /* @0x80028B80-8C */
        v[1]  = (int16_t)(v[1] + e->re2z_part_grav[i]);        /* +0x3A += +0x79 @0x80028B90-A4 */
        t[1] += v[1];                                          /* @0x80028BAC-B8 */
        t[2] += v[2];                                          /* @0x80028BBC-CC */
        if (floor_y < t[1]) {                                  /* slt v1,s0,v1 @0x80028BD8 */
            e->re2z_part_st86[i] = (int16_t)(e->re2z_part_st86[i] - 1); /* @0x80028BE4-F0 */
            t[1] = floor_y;                                    /* @0x80028BF4 */
            v[1] = (int16_t)(((int32_t)(int16_t)(-v[1])) >> 2);/* @0x80028BF8-C10 */
            v[0] = (int16_t)(((int32_t)v[0]) >> 2);            /* @0x80028BFC-C1C */
            v[2] = (int16_t)(((int32_t)v[2]) >> 2);            /* @0x80028C14-28 */
            (void)re2z_rand();                                 /* jal 0x80015FE8 @0x80028C28 */
            re2z_gore_fx(e, i, 2000u);                         /* Anker = das fliegende Teil selbst
                                                             * (`addiu a2,s1,72` @0x80028C34),
                                                             * FUN_8001BF10 @0x80028C4C */
            if (e->re2z_part_flags[i] & 0x10u) {               /* @0x80028C54-60 */
                e->re2z_part_flags[i] = (uint16_t)(e->re2z_part_flags[i] | 0x2000u); /* @0x80028C64-68 */
                /* +0x8C/+0x90 = 7000, +0x8E = 0 @0x80028C6C-7C: Skalierung, [PORT (5)] */
                e->re2z_part_st86[i] = 1;                      /* sh 1,134 @0x80028C80 */
            }
            if (e->re2z_part_st86[i] == 1) {                   /* lh 134 / beq 1 @0x80028C84-8C */
                t[1] = floor_y;                                /* @0x80028C94 */
                e->re2z_part_flags[i] =                        /* DER KASKADEN-AUSLOESER:
                                                                * `and v0,v0,-2` @0x80028CA0 */
                    (uint16_t)(e->re2z_part_flags[i] & (uint16_t)~1u);
            }
        }
        /* [PORT (2)] Wandtest @0x80028CAC-D20 nicht modelliert. */
    }

    if (e->re2z_part_blend[i] >= 0)                            /* lb 122 / bltz @0x80028D24-2C */
        re2z_part_settle(e, i);
}

/* FUN_80028DAC — Drift mit Ablauf (Flagbit 0x08). */
static void re2z_part_phys_dac(re15_actor_t *e, int i)
{
    int32_t *t = e->re2z_part_t[i];

    uint16_t life = e->re2z_part_life[i];                      /* lhu 160 @0x80028DC0 */
    if ((life & 0x7fffu) < 0x1du)                              /* andi/sltiu @0x80028DC8-CC */
        e->re2z_part_life[i] = (uint16_t)(life + 1u);          /* sh @0x80028DD4/E0 */
    else
        e->re2z_part_flags[i] = 0u;                            /* sw zero,0 @0x80028DDC — das
                                                                * Teil verschwindet nach 29 Frames */

    e->re2z_part_w9a[i] = (int16_t)(e->re2z_part_w9a[i] + e->re2z_part_w9e[i]); /* @0x80028DF0-E00 */
    t[1] += (int32_t)e->re2z_part_w9a[i];                                        /* @0x80028E04-14 */

    int32_t M[9];
    re15_skel_euler_matrix_for_test(0, (int)e->re2z_part_yaw98[i], 0, M);        /* @0x80028DF8-E24 */
    int16_t sp = (int16_t)(e->re2z_part_w9c[i] + e->re2z_part_wa4[i]);           /* @0x80028E28-38 */
    if (sp < 0) sp = 0;                                                          /* bgez @0x80028E3C-48 */
    e->re2z_part_w9c[i] = sp;
    t[0] += (int32_t)(int16_t)((M[0] * (int32_t)sp) >> 12);                      /* out.vx @0x80028E64-74 */
    t[2] += (int32_t)(int16_t)((M[6] * (int32_t)sp) >> 12);                      /* out.vz @0x80028E78-88 */
}

int re15_re2z_gore_part_matrix(re15_actor_t *e, int part, uint32_t frame,
                               int32_t rot[9], int32_t trans[3])
{
    if (!e || !rot || !trans || part < 0 || part >= 16) return 0;
    if (!re15_re2z_gore_active(e)) return 0;                   /* RE1.5 nur mit der Port-Option
                                                                * (re15_re15_re2z_import) und erst
                                                                * nach dem ersten Treffer */
    /* `part` ist der BONE-SLOT der geladenen Bank (so ruft der Renderer); der Modellblock steht
     * in RE2-Part-Nummern (Block ueber re2z_part_to_bone). */
    part = re2z_bone_to_part(e, part);
    if (part < 0 || part >= 16) return 0;
    uint16_t fl = e->re2z_part_flags[part];
    if (!(fl & 0x40u)) return 0;                               /* andi v0,s3,0x40 @0x80027498 */
    if (!(fl & 0x01u)) return 0;                               /* Bit 0 weg -> der Walk betritt
                                                                * FUN_80027434 gar nicht erst
                                                                * (@0x8002737C/@0x800273C4) */

    uint16_t bit = (uint16_t)(1u << part);
    if (!(e->re2z_part_seeded & bit)) {
        /* [PORT] Das Original hat die Matrix schon im Record stehen (der Zeichner hat sie im
         * VORFRAME dort hineingeschrieben). Der Port berechnet die Pose jeden Frame neu und
         * friert sie hier beim ersten Frame mit Bit 0x40 ein — 1 Frame Unterschied in der
         * Ausgangspose, danach identisch. */
        for (int k = 0; k < 9; k++) e->re2z_part_m[part][k] = (int16_t)rot[k];
        for (int k = 0; k < 3; k++) e->re2z_part_t[part][k] = trans[k];
        e->re2z_part_seeded = (uint16_t)(e->re2z_part_seeded | bit);
    }

    if (e->re2z_part_frame != frame) {                         /* neuer Frame -> Schritt-Sperre auf */
        e->re2z_part_frame   = frame;
        e->re2z_part_stepped = 0u;
    }
    if (!(e->re2z_part_stepped & bit)) {
        e->re2z_part_stepped = (uint16_t)(e->re2z_part_stepped | bit);
        if (fl & 0x20u) re2z_part_phys_ad8(e, part);           /* @0x80027694-A0 */
        /* Der 0x08-Zweig haengt hinter dem `param_3 & 0x18`-Gate; 0x08 impliziert es. */
        if (fl & 0x08u) re2z_part_phys_dac(e, part);           /* @0x80027B98 */
    }

    for (int k = 0; k < 9; k++) rot[k]   = (int32_t)e->re2z_part_m[part][k];
    for (int k = 0; k < 3; k++) trans[k] = e->re2z_part_t[part][k];
    return 1;
}

/* ---- FUN_80106128 — VERKOHLUNG (+ das +0x10E-Bit 0x80) ------------------------------------ */
static void re2z_gore_burn(re15_actor_t *e)
{
    e->re2z_f10e |= 0x80u;                     /* lhu 270 / ori 0x80 / sh @0x8010613C-48 */
    if (re2z_rand() & 1u) {                    /* jal @0x80106160, andi/beq @0x80106168-6C */
        re2z_gore_fx(e, 0, 0x05032710u);                               /* Part 0 (a2 = s1+72
                                                                        * @0x80106178), v={0,200,0}
                                                                        * @0x80106174-84 */
        if (e->re2z_part_flags[8] & 1u) re2z_gore_fx(e, 8, 0x05031388u); /* Kopf (+1448 = 8*172+72
                                                                        * @0x801061AC) @0x80106190-B4 */
        if (e->re2z_part_flags[3] & 1u) re2z_gore_fx(e, 3, 0x050313E8u); /* R-Unterarm (+588
                                                                        * @0x801061E0), v={0,200,0}
                                                                        * @0x801061C0-E0/@0x80106254 */
    } else {
        re2z_gore_fx(e, 0, 0x05032710u);                               /* Part 0 , v=0
                                                                        * @0x801061E4-F4 */
        if (e->re2z_part_flags[3] & 1u) re2z_gore_fx(e, 3, 0x05031388u); /* R-Unterarm, v={0,200,0}
                                                                        * @0x80106200-24 */
        if (e->re2z_part_flags[6] & 1u) re2z_gore_fx(e, 6, 0x050313E8u); /* L-Unterarm (+1104
                                                                        * @0x80106250) @0x8010622C-58 */
    }
    if (e->sub_state_1 == 16u) return;         /* `lbu v1,5` / `beq v1,16` @0x8010625C-64 */
    e->re2z_part_tint[0]  = 0x00404040u;       /* sw a1,112  @0x8010627C */
    e->re2z_part_tint[1]  = 0x00484848u;       /* sw a2,284  @0x80106288 */
    e->re2z_part_tint[2]  = 0x00707070u;       /* sw a0,456  @0x80106294 */
    e->re2z_part_tint[5]  = 0x00707070u;       /* sw a0,972  @0x801062A0 */
    e->re2z_part_tint[8]  = 0x00505050u;       /* sw v1,1488 @0x801062AC */
    e->re2z_part_tint[9]  = 0x00505050u;       /* sw v1,1660 @0x801062B8 */
    e->re2z_part_tint[10] = 0x00484848u;       /* sw a2,1832 @0x801062C4 */
    e->re2z_part_tint[11] = 0x00404040u;       /* sw a1,2004 @0x801062D0 */
    e->re2z_part_tint[12] = 0x00505050u;       /* sw v1,2176 @0x801062DC */
    e->re2z_part_tint[13] = 0x00404040u;       /* sw a1,2348 @0x801062E8 */
    e->re2z_part_tint[14] = 0x00404040u;       /* sw a1,2520 @0x801062F4 */
}

/* ---- FUN_80106310 — SAEURE-AETZUNG -------------------------------------------------------- */
static void re2z_gore_acid(re15_actor_t *e)
{
    e->re2z_flags21a |= 0x1800u;               /* lhu 538 / ori 0x1800 / sh @0x8010632C-38 */
    e->re2z_part_tint[0]  = 0x00304040u;       /* sw v1,112  @0x8010633C */
    e->re2z_part_tint[1]  = 0x00304040u;       /* sw v1,284  @0x80106348 */
    e->re2z_part_tint[2]  = 0x00405050u;       /* sw a0,456  @0x80106354 */
    e->re2z_part_tint[5]  = 0x00405050u;       /* sw a0,972  @0x80106360 */
    e->re2z_part_tint[8]  = 0x00304040u;       /* sw v1,1488 @0x8010636C */
    e->re2z_part_tint[9]  = 0x00506060u;       /* sw a1,1660 @0x80106378 */
    e->re2z_part_tint[12] = 0x00506060u;       /* sw a1,2176 @0x80106384 */
    re2z_gore_fx(e, 0, 0x040F1770u);                                   /* Part 0  @0x80106390 */
    if (e->re2z_part_flags[12] & 1u) re2z_gore_fx(e, 12, 0x040F0FA0u); /* L-Oberschenkel
                                                                        * @0x801063A0-C0 */
    if (e->re2z_part_flags[3]  & 1u) re2z_gore_fx(e, 3, 0x040F0FA0u);  /* R-Unterarm @0x801063D0-F0 */
}

/* ---- FUN_8010640C — SPRENG-RUSS ----------------------------------------------------------- */
static void re2z_gore_soot(re15_actor_t *e)
{
    re2z_gore_fx(e, 0, 0x05032710u);                                   /* Part 0 , v={0,200,0}
                                                                        * @0x80106418-44 */
    if (e->re2z_part_flags[3] & 1u) re2z_gore_fx(e, 3, 0x050313E8u);   /* R-Unterarm @0x80106450-70 */
    e->re2z_part_tint[0]  = 0x00404040u;       /* sw a0,112  @0x8010648C */
    e->re2z_part_tint[1]  = 0x00484848u;       /* sw a2,284  @0x80106498 */
    e->re2z_part_tint[2]  = 0x00707070u;       /* sw v1,456  @0x801064A4 */
    e->re2z_part_tint[5]  = 0x00707070u;       /* sw v1,972  @0x801064B0 */
    e->re2z_part_tint[9]  = 0x00505050u;       /* sw a1,1660 @0x801064BC */
    e->re2z_part_tint[10] = 0x00484848u;       /* sw a2,1832 @0x801064C8 */
    e->re2z_part_tint[11] = 0x00404040u;       /* sw a0,2004 @0x801064D4 */
    e->re2z_part_tint[12] = 0x00505050u;       /* sw a1,2176 @0x801064E0 */
    e->re2z_part_tint[13] = 0x00404040u;       /* sw a0,2348 @0x801064EC */
    e->re2z_part_tint[14] = 0x00404040u;       /* sw a0,2520 @0x801064F8 */
}

/* ---- FUN_80106510 — SPARK-SHOT-ENTLADUNG --------------------------------------------------
 * Vier Emitter am Part 0 mit je einem eigenen Geschwindigkeitsvektor `128 - rand` je Achse
 * (`addiu s0,zero,128` @0x80106534, `subu v0,s0,v0` @0x80106538/44/50), Id = 0x06000000 |
 * ((rand << 3) + K) mit K = 5096/4096/4096/3096 (@0x80106560/@0x801065B0/@0x801065FC/@0x80106648);
 * danach drei Einzelwuerfe: 0x040C0000-Id am Part 0 (@0x80106670-84), 0x06000000-Id am Part 9
 * (`addiu a2,s0,1620` = 9*172+72 @0x801066B0) und am Part 3 (`addiu a2,s0,588` @0x801066D8).
 * MACHT ZUSAMMEN 19 RNG-WUERFE und 7 Emitter — beides Verhalten, beides exakt nachgezogen. */
static void re2z_gore_spark(re15_actor_t *e)
{
    for (int i = 0; i < 4; i++) {              /* @0x80106530-5C / 80-AC / CC-F8 / 618-44 */
        (void)re2z_rand(); (void)re2z_rand(); (void)re2z_rand();
        uint32_t r = re2z_rand();
        static const int k[4] = { 5096, 4096, 4096, 3096 };
        re2z_gore_fx(e, 0, 0x06000000u | (uint32_t)(((r & 0xffu) << 3) + (uint32_t)k[i]));
    }
    {   uint32_t r = re2z_rand();                                      /* @0x80106664 */
        re2z_gore_fx(e, 0, 0x040C0000u | (uint32_t)(((r & 0xffu) << 3) + 4096u)); /* Part 0
                                                                        * @0x80106670-84 */
    }
    {   uint32_t r = re2z_rand();                                      /* @0x80106690 */
        re2z_gore_fx(e, 9, 0x06000000u | (uint32_t)(((r & 0xffu) << 3) + 3096u)); /* Part 9 (+1620
                                                                        * @0x801066B0) @0x8010669C-B0 */
    }
    {   uint32_t r = re2z_rand();                                      /* @0x801066B8 */
        re2z_gore_fx(e, 3, 0x06000000u | (uint32_t)(((r & 0xffu) << 3) + 3096u)); /* Part 3 (+588
                                                                        * @0x801066D8) @0x801066C0-D8 */
    }
}

/* ---- die per-Zeile-Leiter (Liegend-Zweig @0x80105188-284; DEATH-Zwilling @0x80108444-4D4) --
 * `death` = 1 laesst den `+0x21A |= 0x800`-Nachtrag weg (den hat nur der HURT-Zweig) und nimmt
 * zusaetzlich Zeile 17 in den Russ-Zweig (`beq v1,9` / `bne v1,17` @0x80108490-98). */
static void re2z_dismember_row(re15_actor_t *e, int death)
{
    unsigned row = e->sub_state_1;
    if (row == 10u && !(e->re2z_f10e & 0x80u)) {                /* @0x80105190-A8 / @0x80108444-60 */
        re2z_gore_burn(e);                                     /* jal 0x80106128 @0x801051B0 */
        if (!death) e->re2z_flags21a |= 0x800u;                 /* ori 0x800 @0x801051C0-C4 */
    }
    if (row == 11u) re2z_gore_acid(e);                          /* @0x801051C8-DC / @0x80108478-84 */
    if ((row == 9u || (death && row == 17u)) && !(e->re2z_f10e & 0x80u))
        re2z_gore_soot(e);                                      /* @0x801051E8-208 / @0x801084A0-B8 */
    if (row == 14u) re2z_gore_spark(e);                         /* @0x8010520C-20 / @0x801084C4-D0 */
    if (row == 16u && !death) {                                 /* @0x80105228-84 */
        if (!(e->re2z_f10e & 0x80u) && e->re2z_burn23a >= 9u) { /* andi 0x80 @0x8010523C,
                                                                 * sltiu 0x9 @0x80105250 */
            re2z_gore_burn(e);                                  /* jal @0x8010525C */
            e->re2z_flags21a |= 0x800u;                         /* ori 0x800 @0x8010526C-70 */
        }
        e->re2z_burn23a = (uint8_t)(e->re2z_burn23a + 1u);      /* @0x80105274-84 */
    }
}

/* ---- DER ZERLEGER SELBST @0x80105288-3D8: EIN BEIN WEG ------------------------------------
 * Gate (alle drei muessen gelten):
 *   `+0x21A & 0x60` == 0   @0x80105288-8C  (Einweg — ist ein Bein weg, kommt nie ein zweites)
 *   `(s8)+0x152 < 0`       @0x80105294-9C  (die MITTLERE Zonen-Reserve ist aufgebraucht)
 *   `+0x1D0 & 0xC0` != 0   @0x801052A4-B0  (der Treffer kam von der Seite)
 * Danach: SE 9 (@0x801052B8); Seite ueber `+0x1D0 & 0x80` — gesetzt -> Part 12 (L-Oberschenkel)
 * und `+0x21A |= 0x40` (@0x801052D4-E4), sonst Part 9 (R-Oberschenkel) und `+0x21A |= 0x20`
 * (@0x801052E8-F4). Ein RNG-Wurf (@0x801052F8): bei (rand&1) != 0 UND Typ != 30
 * (`lbu v1,8(s1)` / `beq v1,30` @0x8010530C-14) werden vier Mesh-Woerter des Reserve-Parts 15
 * in den Oberschenkel kopiert (`lw v0,2588/2592/2596/2600(v1)` -> `sw ...,8/12/16/20(s0)`
 * @0x8010531C-50) = der STUMPF. Dann Blut-Emitter 8000 am Oberschenkel (1 Wurf, @0x80105354-6C),
 * das SCHIENBEIN (`addiu s0,s0,172` @0x80105370) bekommt `flags |= 0x1062` (@0x8010537C),
 * seine Wurf-Felder (+0x38 = +0x76+2048, +0x3A = -200, +0x3C = 10, +0x3E = 0, +0x40 = +0x76,
 * +0x42 = 1024, +0x79 = 50, +0x86 = 0 @0x80105384-B8) und einen zweiten Emitter 7000 (1 Wurf,
 * @0x801053BC-D4). Danach faellt der Zweig in den DISPATCH (kein return, @0x801053D8).
 * MACHT DREI RNG-WUERFE. [PORT-MAPPING]: der Mesh-Tausch und die Wurf-Felder haben keinen
 * Renderer-Zwilling — Flag-Wort und Tinte tragen den Zustand, der Wurf-Vektor bleibt OFFEN. */
static void re2z_leg_gore(re15_actor_t *e)
{
    if (e->re2z_flags21a & 0x60u) return;                       /* @0x80105288-8C */
    if (e->re2z_pool152 >= 0) return;                           /* lb 338 / bgez @0x80105294-9C */
    if (!(e->re2z_hitdir1d0 & 0xc0u)) return;                   /* lhu 464 / andi 0xC0 @0x801052A4-B0 */

    /* Dieser SE laeuft seit 2026-08-20 AUCH im RE1.5-Modus (Nutzerwunsch "ziehe es nach", zur
     * Gore-Uebernahme 83b7740c): pc_enemy_load registriert den ENEMSE-Hook jetzt zusaetzlich fuer
     * `re15_re15_re2z_import_owns(type)`. Gemessen: Bank 0, EDT-Eintrag 9 = 0x02A30000 -> prog 0,
     * tone 10, Kanal 2, Prio-Nibble 3, 2208 VAG-Bytes. BELEG, dass die Bankwahl diesen Laut nicht
     * verfaelschen kann: in ALLEN Baenken, die der RDT-Zensus fuer die Zombie-Familie liefert
     * (0/1/2/5/53), ist Eintrag 9 UND das dahinterliegende VAG-Sample byte-identisch; die
     * Hunde-Bank 6 fuehrt id 9 dagegen als 0xFFFFFFFF (stumm), die Wahl ist also nicht beliebig. */
    re2z_se(9);                                                 /* jal 0x8005bd6c, a0=9 @0x801052B4-B8 */

    int thigh;
    if (e->re2z_hitdir1d0 & 0x80u) {                            /* andi 0x80 @0x801052C8 */
        thigh = 12;                                             /* addiu s0,v1,2064 @0x801052DC */
        e->re2z_flags21a |= 0x40u;                              /* ori 0x40 @0x801052E4 */
    } else {
        thigh = 9;                                              /* addiu s0,v1,1548 @0x801052F0 */
        e->re2z_flags21a |= 0x20u;                              /* ori 0x20 @0x801052F4 */
    }
    if ((re2z_rand() & 1u) && e->type != 0x1eu) {               /* @0x801052F8-314 */
        /* STUMPF-MESH: die vier Geometrie-Woerter des RESERVE-Parts 15 wandern in den
         * Oberschenkel — der Oberschenkel zeigt danach das Stumpf-Modell:
         *   8010531c: lw v0,2588(v1)  ->  8010532c: sw v0,8(s0)
         *   80105330: lw v0,2596(v1)  ->  80105338: sw v0,16(s0)
         *   8010533c: lw v0,2592(v1)  ->  80105344: sw v0,12(s0)
         *   80105348: lw v0,2600(v1)  ->  80105350: sw v0,20(s0)
         * 2588..2600 = 15*172 + 8/12/16/20 (Stride 172), s0 = v1 + 2064 bzw. 1548 = Part 12/9.
         * Port-Zwilling: Objektindex statt Zeiger (Part i == MD1-Mesh i). Die frueher hier
         * eingesetzte Blut-TINTE war ein Stand-in ohne Original-Beleg — @0x8010531C-50
         * schreibt KEIN Farbwort — und ist damit entfallen. */
        e->re2z_part_mesh[thigh] = 15u;                         /* @0x8010531C-50 */
    }
    (void)re2z_rand();                                          /* @0x80105354, Emitter-Winkel */
    re2z_gore_fx(e, thigh, 8000u);                              /* Anker = der Oberschenkel
                                                                 * (`addiu a2,s0,72` @0x80105364),
                                                                 * Emitter @0x80105368 */

    int shin = thigh + 1;                                       /* addiu s0,s0,172 @0x80105370 */
    e->re2z_part_flags[shin] |= 0x1062u;                        /* ori 0x1062 @0x8010537C-80 */
    /* DIE WURF-FELDER (@0x80105384-C0) — jetzt verdrahtet (Welle G). Sie sind die Eingabe der
     * Flugphysik FUN_80028AD8; +0x86 = 0 loest deren INIT aus (`lh 134` @0x80028AF4). */
    e->re2z_part_v   [shin][2] = 10;                            /* +0x3C = 10    @0x80105388-8C */
    e->re2z_part_v   [shin][1] = -200;                          /* +0x3A = -200  @0x80105390-94 */
    e->re2z_part_st86[shin]    = 0;                             /* +0x86 = 0     @0x8010539C */
    e->re2z_part_grav[shin]    = 50;                            /* +0x79 = 50    @0x80105398/A0 */
    e->re2z_part_rot [shin][0] = 0;                             /* +0x3E = 0     @0x801053A4 */
    e->re2z_part_v   [shin][0] = (int16_t)(e->rot_y + 2048);    /* +0x38 = +0x76+2048
                                                                 * @0x80105384/A8-AC */
    e->re2z_part_rot [shin][2] = 1024;                          /* +0x42 = 1024  @0x801053B4-B8 */
    e->re2z_part_rot [shin][1] = (int16_t)e->rot_y;             /* +0x40 = +0x76 @0x801053B0/C0 */
    (void)re2z_rand();                                          /* @0x801053BC */
    re2z_gore_fx(e, shin, 7000u);                               /* Anker = das Schienbein
                                                                 * (`addiu s0,s0,172` @0x80105370,
                                                                 * `addiu a2,s0,72` @0x801053CC) */
}

/* ---- @0x80105E10-F10 (Stagger-P0, Zeile 11 = GL SAEURE): das UNTERSCHENKEL-WEGAETZEN -------
 * Gate `!(+0x21A & 0x1000)` (@0x80105E10-1C) — 0x1000 ist der Latch, den FUN_80106310 selbst
 * setzt (`ori 0x1800` @0x80106334); danach ein RNG-Wurf, und nur (rand & 1) == 0 macht weiter
 * (@0x80105E24-30). Zwei weitere Wuerfe waehlen die Seite: `s0 = (r1 >> (r2 & 3)) & 1`
 * (@0x80105E38-54, das `srav` @0x80105E4C). s0 == 0 -> `+0x21A |= 0x20` und Part 10/11
 * (R-Schienbein/-Fuss), s0 == 1 -> `+0x21A = (x & ~0x20) | 0x40` und Part 13/14
 * (`sll v0,s0,7 / addu v0,v0,s0 / sll v0,v0,2 / addiu v0,v0,1720` = 10*172 + s0*3*172,
 * @0x80105E58-90). Beide Parts bekommen `flags |= 0x10` (@0x80105EC4/@0x80105F00), die
 * Aetz-Tinte 0x00304040 (`lui 0x30 / ori 0x4040` @0x80105E70-74, `sw ...,112` @0x80105EC0/
 * @0x80105EFC), die Zerfalls-Felder (+0x98 = 0, +0x9A = 0, +0x9C = 5, +0x9D = 5, +0x9E =
 * -32718 @0x80105EAC-BC) und je einen Emitter (0x040F0BB8 @0x80105ED0 / 0x040F07D0
 * @0x80105F0C). MACHT DREI RNG-WUERFE (bzw. einen, wenn der erste abbricht). */
static void re2z_stagger_acid_leg(re15_actor_t *e)
{
    if (e->re2z_flags21a & 0x1000u) return;                     /* @0x80105E10-1C */
    if (re2z_rand() & 1u) return;                               /* @0x80105E24-30 */
    uint32_t r1 = re2z_rand();                                  /* @0x80105E38 */
    uint32_t r2 = re2z_rand();                                  /* @0x80105E40 */
    unsigned side = (unsigned)((r1 >> (r2 & 3u)) & 1u);         /* srav/andi @0x80105E4C-54 */
    if (side) e->re2z_flags21a = (uint16_t)((e->re2z_flags21a & ~0x20u) | 0x40u); /* @0x80105E64-6C */
    else      e->re2z_flags21a |= 0x20u;                        /* ori 0x20 @0x80105E58 */
    int shin = 10 + 3 * (int)side;                              /* @0x80105E84-90 */
    e->re2z_part_flags[shin]     |= 0x10u;                      /* ori 0x10 @0x80105EC4 */
    e->re2z_part_tint [shin]      = 0x00304040u;                /* sw s2,112 @0x80105EC0 */
    re2z_gore_fx(e, shin, 0x040F0BB8u);                         /* Anker = das Schienbein
                                                                 * (`addiu a2,s3,72` @0x80105EB0,
                                                                 * Emitter @0x80105ED0) */
    e->re2z_part_flags[shin + 1] |= 0x10u;                      /* ori 0x10 @0x80105F00 */
    e->re2z_part_tint [shin + 1]  = 0x00304040u;                /* sw s2,112 @0x80105EFC */
    re2z_gore_fx(e, shin + 1, 0x040F07D0u);                     /* Anker = der Fuss (`addiu s3,
                                                                 * s3,172` @0x80105ED4, `addiu a2,
                                                                 * s3,72` @0x80105EE4) @0x80105F0C */
}

/* ---- Handler 0x80105BC0: der "schon angeschlagen"-Treffer (Zeilen 9-11 + jeder Treffer mit
 * +0x222 == 1). Phasen wieder in +0x6, hier aber ANIM-getrieben: `+0x6 += FUN_8002959C()`
 * @0x80105F48-58 (der Advancer liefert 1, wenn der Clip umlaeuft).
 *   P0 @0x80105C38: +0x6=1, +0x15A=10, +0x14C = Clip 4 (bzw. 3 wenn +0x1D0&0x20) mit Rate 3,
 *      +0x144 = -450 / 0, Drehen auf +0x1C4/+0x1C6 (Rate 16), SE 12/13 per rand&1 mit
 *      +0x239 = 150, +0x16A = 1 / -1, Blut je Zone; Zeile 10/11 zusaetzlich Gore (OPEN).
 *   P1 @0x80105F1C: Advance + Root-Motion, Blut solange +0x15A != 0 && +0x16A != 0, +0x15A--.
 *   P2 @0x80106010: +0x4 = 0x101 (`sw` im Delay-Slot @0x8010601C laeuft IMMER); zusaetzlich bei
 *      (s8)+0x223 <= 0: +0x223 = (rand&0x10)+15 @0x80106028-30, +0x222 = 0 @0x80106034; danach
 *      die Leiter 0xB01 / 0xC01 / 0x201. */
static void re2z_hit_stagger(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80105C48 */
        e->re2z_t15a   = 10;                                       /* +0x15A = 10 @0x80105C50 */
        /* RUECKEN-TREFFER (`+0x1D0 & 0x20`) — Welle F, jetzt verdrahtet. Das Clip-Wort steht
         * als 0x00030004 in a1 (`lui a1,0x3` @0x80105C18 / `ori a1,a1,0x4` @0x80105C38) und
         * wird um EINS DEKREMENTIERT, wenn das Bit steht -> Clip 3, Rate unveraendert 3:
         *   80105c40: lhu  v1,464(s4)     ; +0x1D0
         *   80105c58: andi v1,v1,0x20
         *   80105c5c: sltu v1,zero,v1     ; 1 wenn Ruecken
         *   80105c60: subu a1,a1,v1       ; 0x00030004 -> 0x00030003
         *   80105c74: sw   a1,332(s4)     ; +0x14C
         * Der Schub kommt aus einer Zwei-Wort-Tabelle auf dem Stack, indiziert mit dem
         * gleichen Bit (`sp+16 = -450` @0x80105BEC-F0, `sp+18 = 0` @0x80105BF4):
         *   80105c64: srl  v0,v0,4
         *   80105c68: andi v0,v0,0x2      ; 0 (Front) oder 2 (Ruecken) = Byte-Index
         *   80105c78: lhu  v0,0(v0)       ; sp+16 bzw. sp+18
         *   80105c88: sh   v0,324(s4)     ; +0x144
         * Ein Ruecken-Treffer schiebt den Zombie also NICHT zurueck (Schub 0). */
        {   int back = (e->re2z_hitdir1d0 & 0x20u) != 0;           /* @0x80105C40-60 */
            re2z_clip(e, back ? 3 : 4, 0, 3, 0x400, 0);            /* sw a1,332 @0x80105C74,
                                                                    * Advance-Blend 1024 @0x80105F3C */
            re2z_thrust(e, back ? 0 : -450);                       /* sh v0,324 @0x80105C88 +
                                                                    * FUN_800152C8 @0x80105C84 */
        }
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, 16);     /* @0x80105C90-9C */
        if (e->re2z_cd239 == 0) {                                  /* @0x80105CA0-A8 */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* @0x80105CB0-C8 */
            e->re2z_cd239 = 150;                                   /* @0x80105CD0-D4 */
        }
        /* +0x16A = (+0x1D0 & 0x20) ? -1 : 1 — der `sb 1` steht im Delay-Slot und laeuft immer,
         * die -1 ueberschreibt ihn nur im Ruecken-Treffer (@0x80105CE4-F8). Frueher stand hier
         * fest 1, weil +0x1D0 keinen Produzenten hatte; den gibt es jetzt (re2z_stamp_hitdir). */
        e->re2z_dir16a = (e->re2z_hitdir1d0 & 0x20u) ? (uint8_t)0xffu : (uint8_t)1u;
        re2z_blood_fx(e);                                          /* @0x80105D80 / @0x80105DB8 */
        /* Zeile 10 = GL Brand -> Verkohlung (@0x80105DC4-E08), Zeile 11 = GL Saeure -> das
         * BEIN WEGAETZEN (@0x80105E08-F18) + FUN_80106310 (@0x80105F14). */
        if (e->sub_state_1 == 10u && !(e->re2z_f10e & 0x80u)) {    /* @0x80105DC4-E00 */
            re2z_gore_burn(e);                                     /* jal 0x80106128 @0x80105DE8 */
            e->re2z_flags21a |= 0x800u;                            /* ori 0x800 @0x80105DF8-FC */
        }
        if (e->sub_state_1 == 11u) {                               /* @0x80105E08 */
            re2z_stagger_acid_leg(e);                              /* @0x80105E10-F10 */
            re2z_gore_acid(e);                                     /* jal 0x80106310 @0x80105F14 */
        }
        /* fall through in denselben Frame (@0x80105E08 `bne … 0x80105F1C`) */
        /* FALLTHROUGH */
    case 1:
        re2z_hit_move(e);                                          /* @0x80105F28-58 */
        if (re2z_clip_done(e)) e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);  /* @0x80105F50-58 */
        if (e->re2z_t15a != 0 && e->re2z_dir16a != 0) {            /* @0x80105F5C-74 */
            re2z_blood_fx_at(e, 0, (int16_t)e->rot_y);             /* Anker in BEIDEN Zweigen
                                                                    * `addiu a2,s3,72` = Part 0
                                                                    * (@0x80105FA8 / @0x80105FDC) */
                                                                   /* @0x80105FF4 (Effekt-Ids
                                                                    * 8000/5096 je +0x222) */
            e->re2z_t15a = (int16_t)(e->re2z_t15a - 1);            /* @0x80105FFC-0C */
        }
        break;
    default:                                                       /* P2 @0x80106010 */
        re15_ai_set_state_word(e, 0x101);                          /* sw 257 @0x8010601C (Delay-Slot,
                                                                    * laeuft immer) */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        if ((int)e->re2z_res223 <= 0) {                            /* bgtz @0x80106018 */
            e->re2z_res223 = (int8_t)((re2z_rand() & 0x10u) + 15u);/* @0x80106028-30 */
            e->re2z_flag222 = 0;                                   /* sb zero,546 @0x80106034 */
        }
        if (re15_ai_arc_test(e, pl->x, pl->z, 512) == 0            /* @0x8010604C-54 */
            && e->ai_dist < 0xfa0u                                 /* +0x1F0 < 4000 @0x8010605C-68 */
            && (re2z_rand() & 3u) == 0u) {                         /* @0x80106088-94 */
            re15_ai_set_state_word(e, 0xB01);                      /* 2817 @0x80106098/@0x801060F8 */
            break;
        }
        if (e->ai_dist < 0xbb8u                                    /* < 3000 @0x8010609C-A8 */
            && re15_ai_arc_test(e, pl->x, pl->z, 900) != 0         /* @0x801060C0-C8 */
            && (re2z_rand() & 1u) != 0u) {                         /* @0x801060D0-DC */
            re15_ai_set_state_word(e, 0xC01);                      /* 3073 @0x801060E0/@0x801060F8 */
            break;
        }
        if ((re2z_rand() & 1u) != 0u)                              /* @0x801060E4-F0 */
            re15_ai_set_state_word(e, 0x201);                      /* 513 @0x801060F4/@0x801060F8 */
        break;
    }
}

/* ---- Handler 0x80107EF0 (Zeilen 15 und 18): der leichte Taumel-Treffer ---------------------
 *   P0 @0x80107F80: Clip aus der 8-Byte-Tabelle @0x80100124 {1D,1C,1E,1C,1C,1D,1C,1E}[rand&7]
 *      mit Rate 7 (`sw 0x0007_00xx,332` @0x80107FAC), +0x6 = 1 @0x80107F98, +0x158 = Winkel zum
 *      Spieler @0x80107FD8, Blut je Zone, SE 12 + +0x239 = 150 @0x801080D8-E8, Rueckstoss
 *      +0x144 = (rand&0x3F)+20 @0x80108124 / +0x148 = 64-(rand&0x7F) @0x80108140, Bewegung
 *      um (Winkel+2048) = VOM Spieler WEG @0x80108148, Yaw-Wackler +-64 @0x8010816C, dann
 *      Drehen auf den Spieler mit Rate 16 @0x8010817C-80.
 *   P1 @0x80108184: Advance (Blend 512), +0x6 += clip_done.
 *   P2 @0x801081AC: +0x4 = 0x101, 50% 0x201; danach arc512==0 && dist<5000 && 1/8 -> 0xB01. */
static const uint8_t re2z_hit7ef0_clips[8] = { 0x1D,0x1C,0x1E,0x1C, 0x1C,0x1D,0x1C,0x1E };
static void re2z_hit_light(re15_actor_t *e, re15_actor_t *pl)
{
    int clip;
    switch (e->sub_state_2) {
    case 0:
        clip = (int)re2z_hit7ef0_clips[re2z_rand() & 7u];          /* @0x80107F80-90 */
        e->sub_state_2 = 1;                                        /* @0x80107F98 */
        re2z_clip(e, clip, 0, 7, 0x200, 0);                        /* @0x80107FAC (Blend 512
                                                                    * @0x80108194) */
        re2z_blood_fx_at(e, 1, (int16_t)e->rot_y);                 /* @0x801080C4; Anker
                                                                    * `addiu a2,s1,244` @0x801080C0
                                                                    * = Part 1 (Brust), OHNE
                                                                    * Zonen-Verzweigung */
        if (e->re2z_cd239 == 0) {                                  /* @0x801080CC-D4 */
            re2z_se(12);                                           /* @0x801080D8-E0 */
            e->re2z_cd239 = 150;                                   /* @0x801080E4-E8 */
        }
        re2z_thrust(e, -(int)((re2z_rand() & 0x3fu) + 20u));       /* +0x144 @0x80108124, Bewegung
                                                                    * um Winkel+2048 (=rueckwaerts)
                                                                    * @0x80108148 */
        e->rot_y = (int16_t)((e->rot_y + 64 - (int)(re2z_rand() & 0x7fu)) & 0xfff); /* @0x8010816C */
        re15_enemy_steer_point(e, pl->x, pl->z, 16);               /* @0x8010817C-80 */
        /* faellt in denselben Frame in den Advance-Block (@0x801081A4) */
        /* FALLTHROUGH */
    case 1:
        if (re2z_clip_done(e)) e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);  /* @0x80108198-A8 */
        break;
    default:                                                       /* P2 @0x801081AC */
        re15_ai_set_state_word(e, 0x101);                          /* sw 257 @0x801081B0 */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        if ((re2z_rand() & 1u) == 0u)                              /* @0x801081B4-B8 */
            re15_ai_set_state_word(e, 0x201);                      /* 513 @0x801081BC-C0 */
        if (re15_ai_arc_test(e, pl->x, pl->z, 512) == 0            /* @0x801081D8-E0 */
            && e->ai_dist < 0x1388u                                /* < 5000 @0x801081E8-F4 */
            && (re2z_rand() & 7u) == 0u)                           /* @0x80108214-20 */
            re15_ai_set_state_word(e, 0xB01);                      /* 2817 @0x80108224-28 */
        break;
    }
}

/* ---- Haupt-Handler 0x80105438 -------------------------------------------------------------
 *   @0x80105458-70 : +0x222 == 1 -> `jal 0x80105BC0`, danach Epilog
 *   @0x80105478-B8 : Verzweigung auf +0x6 (0/1/2/3, sonst Epilog)
 *   P0 @0x801054BC : Clip auf den Walk-Clip zwingen (`+0x14C = +0x218 + 0xF0000`, nur wenn
 *                    `+0x218 != (u8)+0x14C` @0x801054C8), Treffer-SE (rand&1 ? 11 : 12) mit
 *                    +0x239 = 150, +0x16B = 24, Pose/Push, +0x6 = 1, +0x15A = 1, +0x158 = 0,
 *                    +0x223 -= cost[+0x5], Nachladen wenn (!+0x10E&0x40 && +0x223<=0 && HP>=81)
 *   P1 @0x8010573C : Push/Pose; +0x158++; war +0x158 >= 3 -> +0x6 = 2, +0x158 = 16
 *   P2 @0x8010588C : Push/Pose; +0x158--; war +0x158 == 0 -> +0x6 = 3
 *   P3 @0x80105A50 : Erholung, Leiter 0xB01 / 0xC01 / 0x201 / 0x101
 * ⛔ NICHT portiert (OPEN, reine Presentation): der Modell-Lean. Das Original dreht in jeder
 * Phase zwei Bone-Matrizen um `-(+0x158 * (s8)+0x16B * 8) * +0x15A` bzw. `+0x158 * +0x16B *
 * +0x15A` (@0x801057A4-E8 / @0x801058D4-960, RotMatrix 0x8008E1F4 + 0x8008D934/0x8008DA44 auf
 * +0x198+24 und +0x198+196). Der Port hat keine Bone-Matrix-Injektion; die Zustands-, Ton- und
 * Timing-Seite ist vollstaendig, das sichtbare Zucken des Oberkoerpers fehlt. */
static void re2z_hit_main(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->re2z_flag222 == 1) { re2z_hit_stagger(e, pl); return; } /* @0x80105458-70 */

    switch (e->sub_state_2) {
    case 0:
        if ((uint8_t)e->motion != e->re2z_walkclip)                /* @0x801054BC-C8 */
            re2z_clip(e, e->re2z_walkclip, 0, 0xF, 0x100, 1);      /* sw +0x218+0xF0000 @0x801054D4
                                                                    * ⛔ loop = 1 aus derselben
                                                                    * Regel: P1/P2 advancen
                                                                    * @0x80105790 / @0x801058C0
                                                                    * OHNE die Rueckgabe zu
                                                                    * nehmen (Ausstieg ist die
                                                                    * +0x158-Rampe), der Clip
                                                                    * wrappt also @0x80029B48.
                                                                    * Heute nicht sichtbar (P1+P2
                                                                    * dauern zusammen 19 Frames,
                                                                    * die Walk-Clips 38-65), aber
                                                                    * dieselbe Aussage wie in
                                                                    * death_MAGNUM/death_RIP. */
        if (e->re2z_cd239 == 0) {                                  /* @0x801054D8-E0 */
            re2z_se((re2z_rand() & 1u) ? 11 : 12);                 /* @0x801054E8-500 */
            e->re2z_cd239 = 150;                                   /* @0x80105508-0C */
        }
        e->re2z_gaitrow = 24;                                      /* +0x16B = 24 @0x80105514-18 */
        if (e->sub_state_1 == 16u) {                               /* Flammenwerfer @0x80105520 */
            if (!(e->re2z_f10e & 0x80u) && e->re2z_burn23a >= 9u) {/* andi 0x80 @0x80105530,
                                                                    * sltiu 0x9 @0x80105544 */
                re2z_gore_burn(e);                                 /* jal 0x80106128 @0x80105550 */
                e->re2z_flags21a |= 0x800u;                        /* ori 0x800 @0x80105560-64 */
            }
            e->re2z_gaitrow = 2;                                   /* sb 2,363 @0x8010556C-70 */
            e->re2z_burn23a = (uint8_t)(e->re2z_burn23a + 1u);     /* @0x80105568-78 */
        }
        re2z_hit_move(e);                                          /* @0x80105580-AC/@0x80105714-20 */
        e->sub_state_2 = 1;                                        /* @0x801055B0-B4 */
        e->re2z_t15a   = 1;                                        /* +0x15A = 1 @0x801055B8-BC */
        e->re2z_t158   = 0;                                        /* +0x158 = 0 @0x801055CC */
        {   unsigned row = e->sub_state_1;
            int cost = (row < 18u) ? (int)re2z_hit_cost[row] : 0;  /* tbl @0x8010CC33 @0x801055D8 */
            e->re2z_res223 = (int8_t)(e->re2z_res223 - cost);      /* subu/sb @0x801055E0/@0x801055EC */
        }
        if (!(e->re2z_f10e & 0x40u)                                /* andi 0x40 / bne @0x801055E4-E8 */
            && (int)e->re2z_res223 <= 0                            /* sll 24 / bgtz @0x801055F0-F4 */
            && (int)e->hp >= 81)                                   /* slti 81 / bne @0x801055FC-08 */
            e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));  /* @0x80105610-20 */
        /* RUECKEN-TREFFER kehrt die ZUCK-RICHTUNG um (Welle F, jetzt verdrahtet):
         *   80105624: lhu v0,464(s1)      ; +0x1D0
         *   8010562c: andi v0,v0,0x20     ; Ruecken-Bit
         *   80105630: beq  v0,zero,0x80105640
         *   80105634: addiu v0,zero,-1    ; Delay-Slot
         *   80105638: sh   v0,346(s1)     ; +0x15A = -1  (Default 1 @0x801055B8-BC)
         * re2z_lean_angle multipliziert mit +0x15A, die P1/P2-Rampe spiegelt sich also
         * am Vorzeichen (0/-192/-384/-576 -> 0/+192/+384/+576). Das ist byte-true: der
         * Zombie zuckt beim Ruecken-Treffer nach VORN statt nach hinten.
         * Der Zwilling `addiu s3,zero,2048` @0x8010563C (Default `addu s3,zero,zero`
         * @0x801055C8) dreht die Richtung des Blut-Emitters. */
        if (e->re2z_hitdir1d0 & 0x20u) e->re2z_t15a = -1;          /* @0x80105624-38 */
        /* ---- DER BLUT-EMITTER DES HAUPT-HANDLERS @0x80105640-710 (Welle G) ------------------
         * Er fehlte im Port komplett — ein normaler Treffer spritzte im RE2-Modus gar nicht.
         *   80105640: lbu  v1,5(s1)
         *   80105644: addiu v0,zero,16
         *   80105648: beq  v1,v0,0x80105714     ; Zeile 16 (Flammenwerfer) -> KEIN Blut
         *   80105650: lbu  v1,466(s1)           ; +0x1D2
         *   8010565c-74: v1 % 3                 ; Zonen-Anteil
         *   80105678: beq  v1,zero,0x801056dc   ; Zone 0 -> Anker Part 1 (+244), v = {0,800,0}
         *   8010567c: _addiu a0,zero,6096       ; Delay-Slot: BEIDE Zweige nehmen Id 6096
         *   80105680-d8: sonst Anker Part 0 (+72), v aus Part 8 (+0x2C..) mit x -= 100*+0x15A
         *                und y += 300
         *   801056e8: sll  a1,s3,16
         *   801056fc: sra  a1,a1,16
         *   80105700: subu a1,v0,a1             ; a1 = +0x76 - s3   (s3 = 2048 bei Ruecken)
         *   80105708: jal  0x8001bf10
         * [PORT-MAPPING] Der Stand-in ist positionslos, die beiden Anker-/Geschwindigkeits-
         * Varianten fallen also zusammen; die Id und der WINKEL sind die des Originals.
         * KEIN RNG-Wurf in diesem Block — die Wurfzahl bleibt unveraendert. */
        if (e->sub_state_1 != 16u) {                               /* @0x80105640-48 */
            int16_t bdir = (int16_t)((int)e->rot_y -
                                     ((e->re2z_hitdir1d0 & 0x20u) ? 2048 : 0));
            re2z_blood_fx_dir(e, bdir);                            /* Id 6096 @0x8010567C */
        }
        break;
    case 1:
        re2z_hit_move(e);                                          /* @0x80105740-90 */
        /* ZUCKEN: die Injektion steht NACH dem Advance und VOR dem +0x158-Inkrement
         * (@0x801057A4-838), rechnet also mit dem noch nicht erhoehten +0x158. */
        re2z_lean_pair(e, 0, 0, re2z_lean_angle(e, 1));            /* <<3 nur in P1 */
        {   int16_t old = e->re2z_t158;
            e->re2z_t158 = (int16_t)(old + 1);                     /* sh @0x80105858 (Delay-Slot) */
            if (!(old < 3)) {                                      /* slti 3 / bne @0x80105850-54 */
                e->sub_state_2 = 2;                                /* @0x80105860-64 */
                e->re2z_t158   = 16;                               /* @0x80105868-6C */
            }
        }
        break;
    case 2:
        re2z_hit_move(e);                                          /* @0x8010588C-C4 */
        re2z_lean_pair(e, 0, 0, re2z_lean_angle(e, 0));            /* @0x801058D4-960, KEIN <<3 */
        {   uint16_t old = (uint16_t)e->re2z_t158;
            e->re2z_t158 = (int16_t)(old - 1u);                    /* sh @0x80105974 (Delay-Slot) */
            if (old == 0u) e->sub_state_2 = 3;                     /* bne/sb @0x80105970-7C */
        }
        /* OPEN — der P2-SCHWANZ @0x80105980-0x80105A4C, der GEGENSPIELER des Zerlegers:
         * `sltiu v0,s3,0x514` (+0x1F0 < 1300 @0x80105980), `0x800CFDCB & 0x80` == 0
         * (@0x8010598C-A0), `+0x106 == *(u8*)0x800CFCFE` (@0x801059A8-B8) und
         * `**(u32**)0x800CFE18 & 1` == 0 (@0x801059C0-D8) — danach ZWEI Sektor-Tests
         * FUN_80015758(&self+0x38, <global>, +0x76 ± 256, 288) @0x80105A00 / @0x80105A38, die
         * je `+0x4 = 0x301` (GRAB) committen. JEDER der beiden ist durch eine LEG-LATCH
         * GEGATET: `+0x21A & 0x20` -> @0x801059E8-EC ueberspringt den ersten, `+0x21A & 0x40`
         * -> @0x80105A1C-20 den zweiten. Das ist die spielbare Folge des Bein-Abrisses (der
         * Zombie kann von dieser Seite nicht mehr zugreifen). Nicht portiert, weil FUN_80015758
         * und die drei RE2-Globals keinen Port-Zwilling haben — die LATCHES erzeugt Welle E
         * jetzt aber korrekt. */
        break;
    case 3:
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (RE1.5-Zwilling
                                                                    * @0x80105f9c-fac); ALLE vier
                                                                    * Ausgaenge dieser Phase
                                                                    * (@0x80105ACC/B08/B24/B38)
                                                                    * verlassen die Reaktion */
        if (e->sub_state_1 == 14u) re2z_gore_spark(e);             /* Spark Shot @0x80105A50-64
                                                                    * (v0 = 14 aus dem Delay-Slot
                                                                    * des Phasen-`beq` @0x801054B0) */
        if (re15_ai_arc_test(e, pl->x, pl->z, 512) == 0            /* @0x80105A7C-84 */
            && e->ai_dist < 0xfa0u                                 /* +0x1F0 < 4000 @0x80105A8C-98 */
            && (re2z_rand() & 0xfu) == 0u) {                       /* @0x80105AB8-C4 */
            re15_ai_set_state_word(e, 0xB01);                      /* 2817 @0x80105ACC-D4 */
            break;
        }
        if (re15_ai_arc_test(e, pl->x, pl->z, 1024) != 0           /* beq-zero-skip @0x80105AE8-F0 */
            && (re2z_rand() & 1u) != 0u) {                         /* @0x80105AF8-04 */
            re15_ai_set_state_word(e, 0xC01);                      /* 3073 @0x80105B08-10 */
            break;
        }
        if ((re2z_rand() & 1u) != 0u) {                            /* @0x80105B14-20 */
            re15_ai_set_state_word(e, 0x201);                      /* 513 @0x80105B24-2C */
            break;
        }
        re15_ai_set_state_word(e, 0x101);                          /* 257 @0x80105B38-40 */
        if ((uint8_t)e->motion != e->re2z_walkclip) {              /* @0x80105B30-3C */
            re15_ai_set_state_word(e, 0x10101);                    /* 0x10101 @0x80105B44-50 */
            e->re2z_gaitrow = (uint8_t)((re2z_rand() & 0xfu) << 1);/* @0x80105B54-5C */
            e->re2z_t158    = 30;                                  /* @0x80105B60-68 */
            e->re2z_dir16a  = (uint8_t)((re2z_rand() & 0x1fu) + 30u); /* @0x80105B6C-78 */
            e->re2z_t15a    = (int16_t)(re2z_rand() + 300u);       /* @0x80105B7C-80 */
        }
        break;
    default: break;                                                /* @0x801054B4 j Epilog */
    }
}

/* ============================================================================================
 * DIE ZERREISS-TODE — die vier DEATH-Zellen, die NICHT FUN_80108530 sind
 * --------------------------------------------------------------------------------------------
 * Der DEATH-Dispatch @0x801084E0-518 (`lbu v1,5(a0)` @0x801084E4 = ZEILE, `lbu v1,466(a0)`
 * @0x801084FC = SPALTE, Basis 0x8010CC24, Zeilen-Stride 36 via `sll 3 / addu / sll 2`
 * @0x801084F0-F8, `lw v0,0(v1)` @0x8010850C, `jalr v0` @0x80108514) schickt die schweren
 * Waffen in eigene Wurzeln. Mit der Spalte 1, die der Port stempelt (Basis-Zone 1, Bracket 0 —
 * re15_re2_stamp_hit), sind das:
 *   Zeile  5/6 Magnum        (RE1.5-Waffe 7)      -> 0x801092C4  re2z_death_magnum
 *   Zeile  7   Schrotflinte  (RE1.5-Waffe 8)      -> 0x801066FC  re2z_hit_ragdoll(death=1)
 *   Zeile  8   SPAS-12       (RE1.5-Waffe 13)     -> 0x80108BEC  re2z_death_rip
 *   Zeile  9   HE-Granate    (RE1.5-Waffe 9/15)   -> 0x80108BEC  re2z_death_rip
 *   Zeile 12   Bowgun        (im Port nie gestempelt)            -> 0x80109610 re2z_death_burst
 *   Zeile 17   Rakete        (RE1.5-Waffe 18)     -> 0x80108BEC  re2z_death_rip
 * SPALTE 0 (0x80107438 = re2z_hit_knockdown mit death=1) ist im Port heute UNERREICHBAR — die
 * Zone 0 hat keinen Produzenten, genau wie an der HURT-Tabelle dokumentiert. Der Zweig ist
 * trotzdem portiert (er ist eine Verzweigung IN einer schon portierten Funktion) und wird im
 * Test durch einen Direktaufruf der Zelle positiv kontrolliert.
 *
 * ---- WAS DIESE VIER GEMEINSAM HABEN --------------------------------------------------------
 * Alle vier laufen ueber DENSELBEN Modellblock +0x198 (16 Records a 172 Byte), den der Port
 * schon fuehrt (re2z_part_flags/_tint/_mesh/_v/_rot/_yaw98/_w9a/_w9c/_w9e/_life/_wa4/_grav/
 * _st86) und dessen Anzeige-Bruecke (re15_re2z_gore_resolve + re15_re2z_gore_part_matrix)
 * seit Welle G steht. Neu ist nur, WER die Felder stempelt:
 *   Flagwort 0x4A   = eigene Matrix (0x40) + Drift-Physik FUN_80028DAC (0x08) -> re2z_part_phys_dac
 *   Flagwort 0x1062 = eigene Matrix (0x40) + Wurf-Physik  FUN_80028AD8 (0x20) -> re2z_part_phys_ad8
 *   Flagwort 0x80   = der STUMPF-Marker (im Port ohne Konsument, s.u.)
 *
 * ⛔ DREI BENANNTE GRENZEN (mit Adresse, nicht gefuellt):
 *  (1) STUMPF-GEOMETRIE. `re2z_part_mesh[0] = 16` (@0x80108FB8-FDC kopiert die vier
 *      Geometrie-Woerter des Records 16 in Part 0) und `= 15` (@0x8010531C-50) zeigen auf die
 *      RESERVE-Meshes des RE2-MD1. Der RE1.5-MD1 hat nur 0..14, die Klammer in
 *      platform/pc/main.c (`gore_mesh[nbi] < mesh_count`) greift dann und der Rumpf bzw. der
 *      Oberschenkel bleibt INTAKT — im Modus "AI RE2" (RE1.5-Modelle) also kein Stumpf. Vom
 *      Nutzer so akzeptiert; hier wird nichts "repariert", nur korrekt gestempelt.
 *  (2) DIE KINDER EINES ABGERISSENEN TEILS. Im Original haengt jedes Kind ueber `rec+0x74` an
 *      der MATRIX des Elternteils (FUN_80028368.c:106-109), fliegt also mit. Der Port berechnet
 *      die Pose aus der Bank-Skelettkette und ersetzt in re15_re2z_gore_part_matrix nur Parts
 *      mit Bit 0x40. Schienbein und Fuss eines wegfliegenden Oberschenkels bleiben deshalb an
 *      der Leiche. Das ist die schon vorhandene Grenze der Anzeige-Bruecke (dieselbe wie beim
 *      Zerleger @0x80105288-3D8), keine neue.
 *  (3) DER STUMPF-MARKER `flags |= 0x80` + `sw 64,+0x98` / `sw 0x00101010,+0x9C`
 *      (@0x80108EAC-BC / @0x80108F68-94 / @0x80108FA8-B4 / @0x801094B8-C8). Bit 0x80 wird von
 *      keinem der beiden portierten Physik-Zweige gelesen (die pruefen 0x20/0x08) und hat im
 *      Zeichner-Zwilling keinen Test — die Felder werden BYTE-TRUE gestempelt (die Wort-Stores
 *      schreiben je ZWEI Halbwortfelder!), der Konsument bleibt OFFEN.
 * ========================================================================================== */

/* FUN_800154AC-Zwilling: die Peilung self -> Spieler, 0 = +X (dieselbe Herleitung wie in
 * re2z_stamp_hit_row; a0/a1 = self.x/z, a2/a3 = 0x800CFC30/0x800CFC38 @0x801093DC-F0). */
static int re2z_bearing_to(const re15_actor_t *e, const re15_actor_t *pl)
{
    if (!pl) return (int)e->rot_y & 0xfff;
    return ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
}

/* FUN_80015910 @0x80015910 (RE2 PSX.EXE, selbst disassembliert):
 *   80015910: lh v0,118(a1)      ; other.+0x76
 *   80015914: lh v1,118(a0)      ; self.+0x76
 *   8001591c: subu v0,v0,v1
 *   80015920: addiu v0,v0,1024
 *   80015924: andi v0,v0,0xfff
 *   8001592c: slti v0,v0,2048    ; 1 = beide blicken in dieselbe Halbebene
 * Der einzige Aufrufer hier gibt als `other` 0x800CFBF8 mit — das ist die SPIELER-ENTITY
 * (Beleg: +0x38 = 0x800CFC30 und +0x40 = 0x800CFC38, genau die beiden Loads @0x801093E4-F0). */
static int re2z_same_facing(const re15_actor_t *e, const re15_actor_t *pl)
{
    if (!pl) return 1;
    return (((int)pl->rot_y - (int)e->rot_y + 1024) & 0xfff) < 2048;
}

/* FUN_800152C8(self, yaw_ofs) mit yaw_ofs != 0 (`sll a1,a1,11` @0x8010995C): der Schub laeuft
 * um +0x76 + a1 gedreht (`lhu v0,118(s1)` / `addu v0,v0,a1` @0x800152E4-F0). */
static void re2z_thrust_yaw(re15_actor_t *e, int spd, int yaw_ofs)
{
    if (!spd) return;
    int y = (int)e->rot_y + yaw_ofs;
    e->x += (int32_t)(((int32_t)re15_cos_q12(y) * spd) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(y) * spd) >> 12);
}

/* ============================================================================================
 * FUN_80109610 — DAS WEGSCHLEUDERN (Zelle RE2ZD_9610 + die Untermaschine `+0x231 == 1`)
 * Selbst disassembliert 2026-08-21 (0x80109610..0x801099DC, EMOVL10_S0.BIN @0x80100000).
 * Drei Phasen ueber +0x6 (`lbu v1,6(s2)` @0x80109650; `beq v1,1 -> 0x80109894` @0x80109658;
 * `slti 2` / `beq zero -> 0x80109690` @0x80109660-6C; `beq v1,2 -> 0x801099B0` @0x80109680).
 * ⛔ KORREKTUR eines alten Port-Kommentars: die Zelle P2 schreibt +0x4 = **7** (CORPSE), nicht
 *    12 — v0 kommt aus dem DELAY-SLOT `addiu v0,zero,7` @0x80109684, `sw v0,4(s2)` @0x801099B0.
 * ========================================================================================== */
static void re2z_death_burst(re15_actor_t *e, re15_actor_t *pl)
{
    static const uint8_t clipsel[2] = { 1u, 2u };  /* sb 1,16(sp) @0x80109644 / sb 2,17(sp)
                                                    * @0x8010964C, gelesen `lbu v0,16(v0)`
                                                    * @0x80109850 */
    switch (e->sub_state_2) {
    case 0: {
        e->re2z_self1d3 |= 0x80u;                  /* ori 0x80 @0x801096A4 / sb 467 @0x801096AC */
        /* `+0x1C0 |= 1` @0x801096A0/A8 — Modell-/Kollisionsbyte ohne Port-Feld (OPEN, dieselbe
         * Luecke wie in re2z_death_main @0x80108614-18). */
        int bear = re2z_bearing_to(e, pl);         /* jal 0x800154AC @0x801096C0 */
        e->re2z_t158 = (int16_t)bear;              /* sh v0,344 @0x801096CC (DELAY-SLOT) */
        unsigned n = (re2z_rand() & 3u) + 1u;      /* jal @0x801096C8, `andi 0x3 / addiu 1`
                                                    * @0x801096D0-D4 */
        if (e->sub_state_1 != 12u) {               /* `beq v1,12` @0x801096E0 */
            re2z_blood_fx_at(e, 0, (int16_t)bear); /* Id 8096 @0x801096E8, ofs {0,-300,0}
                                                    * @0x801096F8-704, Anker Part 0
                                                    * (`addiu s0,s3,72` @0x8010970C) @0x80109710 */
            re2z_blood_fx_at(e, 0, (int16_t)bear); /* Id 6096 @0x80109718, ofs {0,-100,0}
                                                    * @0x80109720-2C, @0x80109734 */
            /* DIE FLEISCHBROCKEN @0x8010973C-B8: n Durchlaeufe zu je FUENF RNG-Wuerfen
             * (vx = 256-2r @0x80109744-48, vy = 56-2r @0x80109754-5C, vz = 256-2r
             * @0x80109768-6C, Id-Anteil @0x80109778-7C, Winkel-Anteil @0x80109798-A8).
             * Abbruchtest am ENDE (`beq s4,zero` @0x801097AC) = do-while, Dekrement im
             * DELAY-SLOT des ersten Wurfs (@0x80109740). */
            for (;;) {
                n--;
                (void)re2z_rand();                 /* vx */
                (void)re2z_rand();                 /* vy */
                (void)re2z_rand();                 /* vz */
                (void)re2z_rand();                 /* Id 0x09020000 | (r*4 + 1536)
                                                    * (`sll 2 / addiu 1536 / lui 0x902 / or`
                                                    * @0x80109778-88) */
                uint32_t r5 = re2z_rand();         /* @0x80109784 */
                int ang = bear - (int)((r5 * 4u) - 512u);      /* `sll v0,v0,2 / addiu -512 /
                                                                * subu a1,a1,v0` @0x80109798-A8 */
                re2z_blood_fx_at(e, 0, (int16_t)ang);          /* Anker Part 0 (`addiu a2,s3,72`
                                                                * @0x80109790) @0x801097A4 */
                if (n == 0u) break;
            }
        } else {
            /* Zeile 12 @0x801097BC-804: EIN Emitter (Id 6096) am TREFFERPART `rec+0x99` des
             * Datensatzes +0x200 (Anker `part*172 + 72`, die Multiplikation steht als
             * Schiebekette @0x801097D0-F4) und `+0x16A = rec+0x99` (@0x801097F8), a3 = 0.
             * +0x200 hat im Port KEINEN Produzenten (identische Luecke wie re2z_hit_slide P0)
             * -> Part 0. Zeile 12 (Bowgun) ist im Port ohnehin unerreichbar: keine RE1.5-Waffe
             * bildet auf sie ab (re2z_row_from_weapon). KEIN RNG-Wurf in diesem Zweig. */
            e->re2z_dir16a = 0;
            re2z_blood_fx_at(e, 0, (int16_t)bear);
        }
        e->re2z_dir16a = (uint8_t)(re2z_same_facing(e, pl) ? 0u : 1u);
                                                   /* jal 0x80015910 @0x80109810, `nor zero,v0`
                                                    * + `andi 0x1` @0x80109818-1C, `sb v0,362`
                                                    * @0x80109820 */
        e->re2z_flags21a &= (uint16_t)~4u;         /* andi 0xfffb @0x8010982C / sh @0x80109834 */
        if (e->re2z_dir16a) e->re2z_flags21a |= 4u;/* ori 0x4 @0x80109838-3C -> Leichen-Pose 22 */
        {   int dir = (int)(e->re2z_dir16a & 1u);
            re2z_clip(e, (int)clipsel[dir], 15, 0xF, 0x100, 0);
                                                   /* Wort 0x000F0F00 + clipsel[dir]
                                                    * (`lui a0,0xf / ori a0,a0,0xf00`
                                                    * @0x80109840-48, `sw v0,332` @0x80109864)
                                                    * = Rate 15, STARTFRAME 15, Clip 1/2.
                                                    * Blend = a3 = 256 des 959c @0x8010994C */
        }
        e->sub_state_2 = 1;                        /* sb v1(=1),6 @0x80109858 */
        re2z_se((re2z_rand() & 1u) ? 13 : 11);     /* `beq v0,zero -> a0 = 11` (DELAY-SLOT
                                                    * @0x80109870), sonst 13 @0x80109874 */
        /* jal 0x80018FB0 @0x80109880 (Pad-Rumble, Praesentation — OPEN wie im Ragdoll) */
        e->speed_h = 400;                          /* +0x144 = 400 @0x80109888-8C. HIER LIVE:
                                                    * P1 ruft KEIN 0x80015E7C, der Wert wird
                                                    * also nicht ueberschrieben, sondern
                                                    * abgebaut (anders als die dokumentierten
                                                    * `sh 11,324`-Dead-Stores). */
        /* `+0x148 = 0` @0x80109890 — der Port fuehrt nur die X-Komponente (s. re2z_thrust). */
        /* FALLTHROUGH @0x80109894 — P1 laeuft im selben Frame */
    }
    /* fall through */
    case 1: {
        int dir = (int)(e->re2z_dir16a & 1u);
        if (e->sub_state_1 != 12u) {               /* `beq v1,12` @0x8010989C */
            int fr = re2z_frame_slot(e);           /* +0x14D @0x801098A4 */
            int d  = dir * 10;                     /* `sll 2 / addu / sll 1` @0x801098B0-B8 */
            if (fr >= d + 4 && fr <= d + 16 && (fr & 1)) {
                                                   /* `addiu v0,v1,4 / slt` @0x801098BC-C4,
                                                    * `addiu v0,v1,16 / slt` @0x801098C8-D0,
                                                    * `andi a1,0x1` @0x801098D4-D8 */
                (void)re2z_rand();                 /* Id = r*8 + 4048 @0x80109914-24 */
                uint32_t r2 = re2z_rand();         /* Winkel = r*16 @0x80109920-2C */
                re2z_blood_fx_at(e, 0, (int16_t)(r2 * 16u));
                                                   /* Anker Part 0 (`addiu a2,s3,72`
                                                    * @0x80109938), ofs y+500 @0x801098FC */
            }
        }
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                   /* +0x6 += 959c(...,256) @0x80109948-68 */
        re2z_thrust_yaw(e, (int)e->speed_h, dir << 11);
                                                   /* 0x800152C8(self, +0x16A << 11)
                                                    * @0x8010995C/64 — mit dem NOCH ALTEN
                                                    * +0x144, der Abbau folgt erst danach */
        {   int v = (int)(uint16_t)e->speed_h - 50;/* `lhu 324 / addiu -50` @0x8010996C-74 */
            e->speed_h = (int16_t)v;               /* sh @0x80109978 */
            if ((int16_t)v < 0) e->speed_h = 0;    /* `sll 16 / bgez / sh zero` @0x8010997C-88 */
        }
        /* `+0x144 == 0 -> FUN_80016200(self,0,1)` @0x8010998C-A4: Matrix-/Positions-Nachzieher
         * (Praesentation), im Port OPEN — derselbe Aufruf wie in re2z_death_main P1. */
        break;
    }
    case 2:
        re15_ai_set_state_word(e, 7u);             /* `sw v0,4(s2)` @0x801099B0, v0 = 7 aus dem
                                                    * DELAY-SLOT @0x80109684 -> CORPSE, Sub 0 */
        break;
    default: break;                                /* j 0x801099B4 */
    }
}

/* ============================================================================================
 * FUN_801092C4 — DER MAGNUM-TOD: DER KOPF PLATZT AB (Zelle RE2ZD_92C4, Zeilen 5/6)
 * Selbst disassembliert 2026-08-21 (0x801092C4..0x801095FC).
 * Phasentabelle @0x8010014C, 5 Eintraege, selbst gedumpt (`table 0x8010014c 12`):
 *   [0] 0x80109330  [1] 0x801094F4  [2] 0x80109534  [3] 0x80109540  [4] 0x80109568
 * Eingang `lbu v1,6(s4)` @0x80109300, `sltiu v0,v1,0x5` @0x80109308, `jr v0` @0x80109328.
 * ========================================================================================== */
static void re2z_death_magnum(re15_actor_t *e, re15_actor_t *pl)
{
    static const uint8_t clipsel[2] = { 1u, 2u };  /* sb 1,16(sp) @0x801092F4 / sb 2,17(sp)
                                                    * @0x801092FC, gelesen `lbu a0,16(v0)`
                                                    * @0x80109368 */
    switch (e->sub_state_2) {
    case 0: {
        int dir = (int)(re2z_rand() & 1u);         /* jal @0x80109330, `andi 0x1 / sb v0,362`
                                                    * @0x80109338-3C */
        e->re2z_dir16a    = (uint8_t)dir;
        e->re2z_flags21a &= (uint16_t)~4u;         /* andi 0xfffb @0x80109348 / sh @0x80109350 */
        if (dir) e->re2z_flags21a |= 4u;           /* ori 0x4 @0x80109354-58 */
        e->sub_state_2    = 1;                     /* sb s3(=1),6 @0x80109374 */
        e->re2z_self1d3  |= 0x80u;                 /* ori 0x80 @0x80109378 / sb @0x8010937C */
        re2z_clip(e, (int)clipsel[dir], dir * 10, 3, 0x100, 0);
                                                   /* Wort ((dir*5)<<9) + 0x00030000 +
                                                    * clipsel[dir] (`sll 2 / addu / sll 9`
                                                    * @0x80109380-88, `lui v1,0x3` @0x8010938C,
                                                    * `sw v0,332` @0x8010939C) = Rate 3 (!),
                                                    * dir 0 -> Clip 1 ab Frame 0, dir 1 ->
                                                    * Clip 2 ab Frame 10. Blend = a3 = 256 des
                                                    * 959c @0x80109504 */
        re2z_blood_fx_at(e, 0, (int16_t)e->rot_y); /* Id 8000 @0x801093C0, Anker Part 0
                                                    * (`addiu s1,s1,72` @0x801093D0), ofs
                                                    * (m+1420, m+1424+300, m+1428)
                                                    * @0x801093A0-C4, @0x801093D4 */
        int bear = re2z_bearing_to(e, pl);         /* jal 0x800154AC @0x801093F4 */
        re2z_blood_fx_at(e, 0, (int16_t)bear);     /* Id 0x08001B58 @0x801093FC-400, Winkel =
                                                    * Peilung @0x80109408-0C, ofs y-400
                                                    * @0x8010941C-24, @0x80109420 */
        re2z_blood_fx_at(e, 0, (int16_t)e->rot_y); /* Id 0x00021F40 @0x80109428-2C, Winkel
                                                    * +0x76 @0x80109434, @0x80109438 */
        /* ---- DER KOPF AB (Part 8 = Kopf; `addiu a2,v1,1448` = 8*172+72 @0x80109464) --------
         * Flagwort |= 0x4A (`lw v0,1376 / ori 0x4a / sw` @0x80109460-98) = eigene Matrix (0x40)
         * + Drift-Physik FUN_80028DAC (0x08). Die Drift-Felder (Stride 172, Part 8 = 1376): */
        e->re2z_part_flags[8] |= 0x4Au;
        e->re2z_part_w9c  [8]  = 400;              /* +0x9C Vortrieb   `sh v0,1532` @0x80109468 */
        e->re2z_part_w9a  [8]  = -100;             /* +0x9A vy         `sh v0,1530` @0x80109470 */
        e->re2z_part_w9e  [8]  = 10;               /* +0x9E vy-Zuwachs `sh v0,1534` @0x80109478 */
        e->re2z_part_yaw98[8]  = (int16_t)((bear + 2048) & 0xfff);
                                                   /* +0x98 Kurs = Peilung+2048 (`addiu s0,s0,
                                                    * 2048` @0x80109450, `sh s0,1528`
                                                    * @0x80109480) */
        e->re2z_part_life [8]  = 0;                /* +0xA0            `sh zero,1536` @0x80109484 */
        e->re2z_part_wa4  [8]  = -50;              /* +0xA4 Vortriebs-Zuwachs `sh v0,1540`
                                                    * @0x80109488 */
        e->re2z_part_tint [8]  = 0x00101040u;      /* +0x70 Farbwort   `sw t0,1488` @0x8010948C
                                                    * (t0 = 0x00101040 @0x80109440-44) */
        /* FUN_8001CEFC(5,3,Part 8) @0x80109494 = die am Kopf haengenden Effekte abschalten;
         * der Port-Stand-in kennt keine Anker -> No-op (wie ueberall in diesem Modul). */
        /* Der STUMPF-MARKER auf Part 0 (@0x801094A8-C8), zwei WORT-Stores: */
        e->re2z_part_yaw98[0]  = 64;               /* `sw v0,152(s1)` @0x801094B8 (Low-Half) */
        e->re2z_part_w9a  [0]  = 0;                /* dito High-Half */
        e->re2z_part_w9c  [0]  = 0x1010;           /* `sw a2,156(s1)` @0x801094BC (Low-Half) */
        e->re2z_part_w9e  [0]  = 0x0010;           /* dito High-Half (a2 = 0x00101010
                                                    * @0x8010949C-A0) */
        e->re2z_part_flags[0] |= 0x80u;            /* `ori v1,v1,0x80 / sw v1,0(s1)`
                                                    * @0x801094C0/C8 */
        re2z_se(2);                                /* jal 0x8005bd6c, a0 = 2 @0x801094A4/C4 */
        /* jal 0x80018FB0 @0x801094CC (Praesentation, OPEN) */
        /* `+0x144 = 11` @0x801094D4-D8 ist ein DEAD STORE: P1 ruft weder 0x80015E7C noch
         * 0x800152C8, P4 fuellt +0x144 vorher mit dem Clip-Wurzel-Delta neu (0x80015E7C
         * @0x801095BC) — derselbe Fall wie die drei dokumentierten `sh 11,324`-Seeds. */
        /* `+0x1C0 = 1` @0x8010945C (OPEN, s.o.) */
        if (re2z_rand() & 1u) {                    /* jal @0x801094D8, `andi 0x1 / beq`
                                                    * @0x801094E0-E4 */
            e->sub_state_2 = 3;                    /* sb v0(=3),6 @0x801094F0 — 50%: der
                                                    * KOPFLOSE laeuft erst noch weiter */
            return;                                /* j 0x801095E4 */
        }
        /* FALLTHROUGH @0x801094F4 */
    }
    /* fall through */
    case 1:
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                   /* +0x6 += 959c(...,256) @0x80109500-18 */
        /* `+0x14E == 0 -> FUN_80016200(self,0,1)` @0x8010950C-28 (Praesentation, OPEN) */
        break;
    case 2:
        re15_ai_set_state_word(e, 7u);             /* `addiu v0,zero,7 / sw v0,4(s4)`
                                                    * @0x80109534-3C -> CORPSE, Sub 0 */
        break;
    case 3:
        e->sub_state_2 = 4;                        /* sb v0(=4),6 @0x80109540-44 */
        re2z_clip(e, (int)e->re2z_walkclip, 0, 0xF, 0x100, 1);
                                                   /* Wort 0x000F0000 + +0x218 (`lbu v0,536 /
                                                    * lui v1,0xf / addu / sw v0,332`
                                                    * @0x80109548-58); Blend = a3 = 256 des
                                                    * 959c @0x801095D4.
                                                    * ⛔ loop = 1: P4s Advance @0x801095D0
                                                    * VERWIRFT die Rueckgabe, der Clip wrappt
                                                    * also (`sb zero,333(s2)` @0x80029B48) bis
                                                    * der Timer +0x158 feuert. Ohne das Bit
                                                    * pinnte der Port-Advancer den kopflosen
                                                    * Zombie auf fc-1 = die eingefrorene
                                                    * Sterbeanimation (Blockkopf re2z_clip). */
        e->re2z_t158 = (int16_t)((re2z_rand() & 0x3fu) + 30u);
                                                   /* +0x158 = (rand & 0x3F) + 30
                                                    * @0x80109554-64 */
        /* FALLTHROUGH @0x80109568 */
        /* fall through */
    case 4: {
        uint16_t old = (uint16_t)e->re2z_t158;     /* lhu 344 @0x80109568 */
        e->re2z_t158 = (int16_t)(uint16_t)(old - 1u);
                                                   /* `sh v0,344` im DELAY-SLOT @0x80109578 =
                                                    * IMMER */
        if (old == 0u) {                           /* `bne v1,zero` @0x80109574 — der ALTE Wert
                                                    * entscheidet, der Zerfall feuert also einen
                                                    * Frame NACH dem Nullstand */
            e->sub_state_2 = 1;                    /* sb v0(=1),6 @0x8010958C */
            re2z_clip(e, 1, 0, 0xF, 0x100, 0);     /* Wort 0x000F0001 @0x8010957C-90 */
            e->re2z_flags21a &= (uint16_t)~4u;     /* andi 0xfffb @0x80109594-98 */
        }
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, 8);
                                                   /* FUN_80015558(self,+0x1C4,+0x1C6,8)
                                                    * @0x8010959C-AC */
        re15_re2z_move_root(e);                    /* 0x80015E7C @0x801095BC + 0x800152C8
                                                    * @0x801095DC */
        /* `jal 0x8002959c(a3=256)` @0x801095D0 — Advance ohne +0x6-Uebernahme */
        break;
    }
    default: break;
    }
}

/* ============================================================================================
 * FUN_80108BEC — DER ZERREISS-TOD: KOPF AB + BEIN(E) AB (Zelle RE2ZD_8BEC, Zeilen 8/9/17)
 * Selbst disassembliert 2026-08-21 (0x80108BEC..0x801092C0).
 * Kopf @0x80108C34-4C: `+0x231 == 1` -> `jal 0x80109610` und raus.
 * Phasentabelle @0x8010012C, 7 Eintraege, selbst gedumpt (`table 0x8010012c 24`):
 *   [0] 0x80108C80 [1] 0x80109078 [2] 0x80109158 [3] 0x801091AC
 *   [4] 0x801091EC [5] 0x801091F8 [6] 0x80109220     (`sltiu v0,v1,0x7` @0x80108C5C)
 * ========================================================================================== */
static void re2z_death_rip(re15_actor_t *e, re15_actor_t *pl)
{
    static const int16_t push [2] = { -450, 250 }; /* sp+16/sp+18 @0x80108C14-20, gelesen
                                                    * `lhu v0,16(v0)` @0x80108DA8 */
    static const uint8_t clip2[2] = { 2u, 1u };    /* sp+32/sp+33 @0x80108C24-30, gelesen
                                                    * `lbu a0,32(v0)` @0x80109180 */
    if (e->re2z_rag231 == 1u) {                    /* `lbu v1,561 / bne v1,1` @0x80108C34-3C */
        re2z_death_burst(e, pl);                   /* jal 0x80109610 @0x80108C44 */
        return;                                    /* j 0x8010929C */
    }
    if (e->sub_state_2 >= 7u) return;              /* `sltiu v0,v1,0x7 / beq` @0x80108C5C-60 */

    switch (e->sub_state_2) {
    case 0: {
        /* ---- AUSSTIEG 1 IN DEN BURST @0x80108C80-D40 --------------------------------------
         * `rand & 3` != 0 UND `0x800CFBF8 >= 0` UND Zeile weder 9 noch 17 -> +0x231 = 1 und
         * FUN_80109610. Das `lw 0x800CFBF8 / bltz` @0x80108C94-A0 liest das WORT 0 der
         * SPIELER-ENTITY (Beleg s. re2z_same_facing); Bit 31 hat im Port keinen Produzenten,
         * der Zweig laeuft also weiter — OPEN, mit Adresse. */
        if ((re2z_rand() & 3u) != 0u &&
            e->sub_state_1 != 9u && e->sub_state_1 != 17u) {
                                                   /* `beq v0,zero -> 0x80108CC0` @0x80108C8C,
                                                    * `beq v1,9 -> 0x80108CD8` @0x80108CB0,
                                                    * `bne v1,17 -> 0x80108D28` @0x80108CB8 */
            e->re2z_rag231 = 1u;                   /* `sb v0(=1),561` @0x80108D28 */
            re2z_death_burst(e, pl);               /* jal 0x80109610 @0x80108D34 */
            return;
        }
        if ((e->sub_state_1 == 9u || e->sub_state_1 == 17u) &&
            !(e->re2z_f10e & 0x80u))               /* `lhu 270 / andi 0x80 / bne` @0x80108CD8-E4 */
            re2z_gore_soot(e);                     /* jal 0x8010640C @0x80108CEC */

        /* ---- AUSSTIEG 2 (nur Zeile 9) @0x80108CF4-D40 -------------------------------------
         * Zwei Wuerfe, `s = (r1 >> (r2 & 7)) & 3`. Die Schiebe-Instruktion @0x80108D08 steht im
         * Dump als Rohwort 0x00508007 = `srav s0,s0,v0` (SPECIAL, funct 0x07). */
        {   uint32_t r1 = re2z_rand();             /* @0x80108CF4 */
            uint32_t r2 = re2z_rand();             /* @0x80108CFC */
            unsigned s  = (unsigned)((r1 >> (r2 & 7u)) & 3u);
            if (s != 0u && e->sub_state_1 == 9u) { /* `beq s0,zero` @0x80108D10, `bne v1,9`
                                                    * @0x80108D20 */
                e->re2z_rag231 = 1u;
                re2z_death_burst(e, pl);
                return;
            }
        }

        /* ---- die RICHTUNG @0x80108D44-70 ---- */
        e->re2z_gaitrow = (uint8_t)((e->re2z_hitdir1d0 & 0x20u) >> 5);
                                                   /* `lhu 464 / andi 0x20 / srl 5 / sb v0,363`
                                                    * @0x80108D44-54 */
        e->re2z_flags21a &= (uint16_t)~4u;         /* andi 0xfffb @0x80108D60 / sh @0x80108D68 */
        if (e->re2z_gaitrow == 0u) e->re2z_flags21a |= 4u;
                                                   /* ⚠ UMGEKEHRT zu den anderen Wurzeln: das
                                                    * `bne v1,zero` @0x80108D64 UEBERSPRINGT das
                                                    * `ori 0x4` @0x80108D6C, Bit 4 (Leichen-Pose
                                                    * 22) steht also beim FRONT-Treffer */
        int back = (int)(e->re2z_gaitrow & 1u);
        e->sub_state_2 = 1;                        /* sb s0(=1),6 @0x80108D90 */
        e->re2z_t15a   = 10;                       /* sh s1(=10),346 @0x80108D94 */
        re2z_clip(e, back ? 3 : 4, 0, 3, 0x400, 0);/* Wort 0x00030004 - back (`lui a1,0x3 /
                                                    * ori 0x4` @0x80108D74-78, `subu a1,a1,v1`
                                                    * @0x80108D98, `sw a1,332` @0x80108DA4);
                                                    * Blend = a3 = 1024 des 959c @0x8010909C */
        e->speed_h = push[back];                   /* sh v0,324 @0x80108DB8 (DELAY-SLOT) */
        re2z_thrust(e, (int)push[back]);           /* jal 0x800152C8 (a1 = 0) @0x80108DB4 */
        /* `+0x148 = 0` @0x80108DB0 / `+0x1C0 = 1` @0x80108DC4 — OPEN (s. Blockkopf) */
        e->re2z_self1d3 |= 0x80u;                  /* ori 0x80 @0x80108DCC / sb @0x80108DD0 */

        /* ---- DER KOPF AB (Part 8, Basis 1376 = 8*172) @0x80108DD4-E3C ---- */
        e->re2z_part_flags[8] |= 0x4Au;            /* `lw 1376 / ori 0x4a / sw` @0x80108DD4-E0 */
        e->re2z_part_w9c  [8]  = 400;              /* +0x9C `sh v0,1532` @0x80108DEC */
        e->re2z_part_w9a  [8]  = -100;             /* +0x9A `sh v0,1530` @0x80108DF4 */
        e->re2z_part_life [8]  = 0;                /* +0xA0 `sh zero,1536` @0x80108DFC */
        e->re2z_part_w9e  [8]  = 10;               /* +0x9E `sh s1,1534` @0x80108E00 */
        e->re2z_part_wa4  [8]  = -50;              /* +0xA4 `sh v0,1540` @0x80108E04 */
        e->re2z_part_tint [8]  = 0x00101030u;      /* +0x70 `sw a0,1488` @0x80108E08 (a0 =
                                                    * 0x00101030 @0x80108DC0/D8) */
        e->re2z_part_yaw98[8]  = (int16_t)((e->rot_y + 2048) & 0xfff);
                                                   /* +0x98 = +0x76 + 2048 (`lhu 118`
                                                    * @0x80108DE4, `addiu 2048` @0x80108E0C,
                                                    * `sh v1,1528` @0x80108E14) */
        re2z_blood_fx_at(e, 8, (int16_t)(re2z_rand() * 16u));
                                                   /* Id 3000 @0x80108E18, Winkel = rand*16
                                                    * (jal @0x80108E10, `sll a1,v0,4`
                                                    * @0x80108E1C), Anker Part 8 (`addiu s0,
                                                    * s0,1448` @0x80108E20), a3 = 0 @0x80108E2C,
                                                    * FUN_8001BF10 @0x80108E28 */
        /* FUN_8001CEFC(5,3,Part 8) @0x80108E38 = No-op im Port */

        /* ---- DAS RECHTE BEIN (Parts 9/10/11) @0x80108E40-EE4 ---- */
        if (!(e->re2z_flags21a & 0x20u)) {         /* `lhu 538 / andi 0x20 / bne` @0x80108E40-4C */
            e->re2z_part_flags[9] |= 0x1062u;      /* `lw 1548 / ori 0x1062 / sw` @0x80108E54-60
                                                    * = eigene Matrix (0x40) + Wurf-Physik
                                                    * FUN_80028AD8 (0x20) */
            e->re2z_part_v   [9][2] = 100;         /* +0x3C `sh v0,1608` @0x80108E6C */
            e->re2z_part_st86[9]    = 0;           /* +0x86 `sh zero,1682` @0x80108E74 — loest
                                                    * den Physik-INIT aus (`lh 134` @0x80028AF4) */
            e->re2z_part_v   [9][1] = 0;           /* +0x3A `sh zero,1606` @0x80108E78 */
            e->re2z_part_grav[9]    = 30;          /* +0x79 `sb v0,1669` @0x80108E7C */
            e->re2z_part_rot [9][0] = 0;           /* +0x3E `sh zero,1610` @0x80108E80 */
            e->re2z_part_v   [9][0] = (int16_t)((e->rot_y + 2048) & 0xfff);
                                                   /* +0x38 `sh v1,1604` @0x80108E88 */
            e->re2z_part_rot [9][2] = 1024;        /* +0x42 `sh v0,1614` @0x80108E94 */
            e->re2z_part_rot [9][1] = (int16_t)e->rot_y;
                                                   /* +0x40 `sh v1,1612` @0x80108E98 */
            /* Der STUMPF-MARKER auf Part 10 (Schienbein, Basis 1720) — zwei WORT-Stores: */
            e->re2z_part_yaw98[10]  = 64;          /* `sw v0,1872` @0x80108EAC (Low-Half) */
            e->re2z_part_w9a  [10]  = 0;           /* dito High-Half */
            e->re2z_part_w9c  [10]  = 0x1010;      /* `sw a1,1876` @0x80108EB0 (Low-Half) */
            e->re2z_part_w9e  [10]  = 0x0010;      /* dito High-Half (a1 = 0x00101010) */
            e->re2z_part_flags[10] |= 0x80u;       /* `ori v1,v1,0x80 / sw v1,1720` @0x80108EB4/BC */
        } else {                                   /* Bein schon ab (@0x80108EC0-E4) */
            e->re2z_part_flags[9]  = 0u;           /* `sw zero,1548` @0x80108EC8 */
            e->re2z_part_flags[10] = 0u;           /* `sw zero,1720` @0x80108ED0 */
            e->re2z_part_flags[11] = 0u;           /* `sw zero,1892` @0x80108EE4 */
            /* 2x FUN_8001CEFC(5,3,Part 10/11) @0x80108ECC/@0x80108EE0 = No-op */
        }

        /* ---- DAS LINKE BEIN (Parts 12/13) @0x80108EE8-F70 — nur mit 50% ---- */
        if (!(e->re2z_flags21a & 0x40u) &&         /* `andi 0x40 / bne` @0x80108EF0-F4 */
            (re2z_rand() & 1u) == 0u) {            /* jal @0x80108EFC, `andi 0x1 / bne`
                                                    * @0x80108F04-08 — der Wurf faellt NUR,
                                                    * wenn das Bein noch dran ist */
            e->re2z_part_flags[12] |= 0x1062u;     /* `lw 2064 / ori 0x1062 / sw` @0x80108F10-1C */
            e->re2z_part_v   [12][2] = 100;        /* +0x3C `sh v0,2124` @0x80108F28 */
            e->re2z_part_grav[12]    = 30;         /* +0x79 `sb v0,2185` @0x80108F30 */
            e->re2z_part_st86[12]    = 0;          /* +0x86 `sh zero,2198` @0x80108F3C */
            e->re2z_part_v   [12][1] = 0;          /* +0x3A `sh zero,2122` @0x80108F40 */
            e->re2z_part_rot [12][0] = 0;          /* +0x3E `sh zero,2126` @0x80108F44 */
            e->re2z_part_v   [12][0] = (int16_t)((e->rot_y + 2048) & 0xfff);
                                                   /* +0x38 `sh v1,2120` @0x80108F4C */
            e->re2z_part_rot [12][2] = 1024;       /* +0x42 `sh v1,2130` @0x80108F64 */
            e->re2z_part_rot [12][1] = (int16_t)e->rot_y;
                                                   /* +0x40 `sh a0,2128` @0x80108F70 */
            e->re2z_part_yaw98[13]  = 64;          /* `sw v0,2388` @0x80108F68 (Low-Half) */
            e->re2z_part_w9a  [13]  = 0;
            e->re2z_part_w9c  [13]  = 0x1010;      /* `sw a1,2392` @0x80108F6C (Low-Half) */
            e->re2z_part_w9e  [13]  = 0x0010;
            e->re2z_part_flags[13] |= 0x80u;       /* `lw 2236 / ori 0x80 / sw` @0x80108F34-5C */
        }

        /* ---- IMMER: der Stumpf-Marker auf Part 11 (R-Fuss) @0x80108F78-94 ---- */
        e->re2z_part_yaw98[11]  = 64;              /* `sw v0,2044` @0x80108F88 */
        e->re2z_part_w9a  [11]  = 0;
        e->re2z_part_w9c  [11]  = 0x1010;          /* `sw a1,2048` @0x80108F8C */
        e->re2z_part_w9e  [11]  = 0x0010;
        e->re2z_part_flags[11] |= 0x80u;           /* `lw 1892 / ori 0x80 / sw` @0x80108F80-94 */
        /* ---- IMMER: Part 0 @0x80108F98-B4 (hier 32/32, nicht 64/0x00101010) ---- */
        e->re2z_part_yaw98[0]   = 32;              /* `sw v1,152(s1)` @0x80108FA8 */
        e->re2z_part_w9a  [0]   = 0;
        e->re2z_part_w9c  [0]   = 32;              /* `sw v1,156(s1)` @0x80108FAC */
        e->re2z_part_w9e  [0]   = 0;
        e->re2z_part_flags[0]  |= 0x80u;           /* `ori v0,v0,0x80 / sw v0,0(s1)`
                                                    * @0x80108FB0/B4 */
        /* ---- DER RUMPF-STUMPF @0x80108FB8-FDC ---------------------------------------------
         * Die vier Geometrie-Woerter des Records 16 wandern in Part 0 — genau die Mechanik des
         * Bein-Stumpfs (@0x8010531C-50), nur mit dem ZWEITEN Reserve-Record:
         *   80108fc0: lw v1,2760(v0)  ->  80108fd0: sw v1,8(v0)
         *   80108fc4: lw a0,2768(v0)  ->  80108fd4: sw a0,16(v0)
         *   80108fc8: lw a1,2764(v0)  ->  80108fd8: sw a1,12(v0)
         *   80108fcc: lw a2,2772(v0)  ->  80108fdc: sw a2,20(v0)
         * 2760..2772 = 16*172 + 8/12/16/20. Port-Zwilling: Objektindex (Part i == MD1-Mesh i).
         * ⛔ Mesh 16 gibt es nur im RE2-MD1 — s. Grenze (1) im Blockkopf. */
        e->re2z_part_mesh[0] = 16u;
        {   uint32_t r1 = re2z_rand();             /* @0x80108FF0 */
            uint32_t r2 = re2z_rand();             /* @0x80108FF8 */
            int ang = (int)e->rot_y + (int)(r1 * 16u) - (int)(r2 * 16u);
                                                   /* `sll s0,s0,4` @0x8010900C, `sll v0,v0,4`
                                                    * @0x80109014, `addu/subu` @0x80109018-1C */
            re2z_blood_fx_at(e, 0, (int16_t)ang);  /* Id 8000 @0x80109000, ofs {0,-500,0}
                                                    * @0x80108FE4-F4, Anker Part 0 (`addiu s1,
                                                    * s1,72` @0x80109020) @0x80109024 */
        }
        {   (void)re2z_rand();                     /* Id 0x08000000 | ((r+4000)*4)
                                                    * @0x8010902C-44 */
            uint32_t r4 = re2z_rand();             /* Winkel @0x80109040-4C */
            re2z_blood_fx_at(e, 0, (int16_t)(r4 * 16u));   /* Anker Part 0 @0x80109050,
                                                            * FUN_8001BF10 @0x80109054 */
        }
        re2z_se(2);                                /* jal 0x8005bd6c, a0 = 2 @0x8010905C-60 */
        /* jal 0x80018FB0 @0x80109068 (Praesentation, OPEN) */
        /* `+0x144 = 11` @0x80109070-74 = DEAD STORE: P1 ruft 0x80015E7C (@0x80109084), das
         * +0x144 mit dem Clip-Wurzel-Delta ueberschreibt, BEVOR 0x800152C8 (@0x801090B0) es
         * liest — derselbe Fall wie die drei dokumentierten `sh 11,324`-Seeds. */
        /* FALLTHROUGH @0x80109078 */
    }
    /* fall through */
    case 1:
        re15_re2z_move_root(e);                    /* 0x80015E7C(a3=0) @0x80109084 + 0x800152C8
                                                    * @0x801090B0 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                   /* +0x6 += 959c(...,1024) @0x80109098-B4 */
        if (e->re2z_t15a != 0) {                   /* `lh 346 / beq` @0x801090B8-C0 */
            if (((int)e->re2z_t15a & 1) == 0) {    /* `andi 0x1 / bne` @0x801090C8-CC — nur auf
                                                    * den GERADEN Staenden */
                for (int k = 0; k < 2; k++) {      /* zwei Emitter @0x801090D4-140 */
                    uint32_t r1 = re2z_rand();     /* @0x801090D8 / @0x80109110 */
                    uint32_t r2 = re2z_rand();     /* @0x801090E0 / @0x80109118 */
                    int ang = (int)e->rot_y + (int)(r1 * 16u) - (int)(r2 * 16u);
                    re2z_blood_fx_at(e, 0, (int16_t)ang);
                                                   /* Id 8000 @0x801090E8/@0x80109120, Anker
                                                    * Part 0 (`addiu s1,s1,72` @0x80109104),
                                                    * a3 = &+0x144 @0x801090DC */
                }
            }
            e->re2z_t15a = (int16_t)(e->re2z_t15a - 1);
                                                   /* `lhu 346 / addiu -1 / sh` @0x80109144-54 */
        }
        break;
    case 2:
        if (re2z_rand() & 1u) {                    /* jal @0x80109158, `andi 0x1 / beq`
                                                    * @0x80109160-64 */
            e->sub_state_2 = 5;                    /* sb v0(=5),6 @0x80109170 — 50%: der
                                                    * zerfetzte Torso taumelt erst noch */
            return;                                /* j 0x8010929C */
        }
        {   int back = (int)(e->re2z_gaitrow & 1u);/* lb 363 @0x80109174 */
            e->sub_state_2 = 3;                    /* sb v0(=3),6 @0x80109188 */
            re2z_clip(e, (int)clip2[back], back * 15 + 10, 7, 0x200, 0);
                                                   /* Wort ((back*15+10)<<8) + 0x00070000 +
                                                    * clip2[back] (`sll 4 / subu / addiu 10 /
                                                    * sll 8` @0x8010918C-98, `lui v1,0x7`
                                                    * @0x8010919C, `sw v0,332` @0x801091A8);
                                                    * Blend = a3 = 512 des 959c @0x801091BC
                                                    * (der Setz-Frame selbst advanced noch mit
                                                    * a3 = 256 @0x80109288 — dokumentiert) */
        }
        /* FALLTHROUGH @0x801091AC */
        /* fall through */
    case 3:
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                   /* +0x6 += 959c(...,512) @0x801091B8-D0 */
        /* `+0x14E == 0 -> FUN_80016200(self,0,1)` @0x801091C4-E0 (Praesentation, OPEN) */
        break;
    case 4:
        re15_ai_set_state_word(e, 7u);             /* `addiu v0,zero,7 / sw v0,4(s3)`
                                                    * @0x801091EC-F4 -> CORPSE, Sub 0 */
        break;
    case 5:
        e->sub_state_2 = 6;                        /* sb v0(=6),6 @0x801091F8-FC */
        re2z_clip(e, (int)e->re2z_walkclip, 0, 0xF, 0x100, 1);
                                                   /* Wort 0x000F0000 + +0x218 @0x80109200-10;
                                                    * Blend = a3 = 256 des 959c @0x8010928C.
                                                    * ⛔ loop = 1: P6s Advance @0x80109288
                                                    * VERWIRFT die Rueckgabe -> Wrap-Loop
                                                    * (@0x80029B48) bis der Timer +0x158 feuert
                                                    * (Blockkopf re2z_clip). */
        e->re2z_t158 = (int16_t)((re2z_rand() & 0x1fu) + 15u);
                                                   /* +0x158 = (rand & 0x1F) + 15 @0x8010920C-1C */
        /* FALLTHROUGH @0x80109220 */
        /* fall through */
    case 6: {
        uint16_t old = (uint16_t)e->re2z_t158;     /* lhu 344 @0x80109220 */
        e->re2z_t158 = (int16_t)(uint16_t)(old - 1u);
                                                   /* `sh v0,344` im DELAY-SLOT @0x80109230 =
                                                    * IMMER */
        if (old == 0u) {                           /* `bne v1,zero` @0x8010922C — ALTER Wert */
            e->sub_state_2 = 3;                    /* sb v0(=3),6 @0x80109244 */
            re2z_clip(e, 1, 0, 7, 0x200, 0);       /* Wort 0x00070001 @0x80109234-48 */
            e->re2z_flags21a &= (uint16_t)~4u;     /* andi 0xfffb @0x8010924C-50 */
        }
        re15_enemy_steer_point(e, e->steer_x, e->steer_z, 8);
                                                   /* FUN_80015558(self,+0x1C4,+0x1C6,8)
                                                    * @0x80109254-64 */
        re15_re2z_move_root(e);                    /* 0x80015E7C @0x80109274 + 0x800152C8
                                                    * @0x80109294 */
        /* `jal 0x8002959c(a3=256)` @0x80109288 — Advance ohne +0x6-Uebernahme */
        break;
    }
    default: break;
    }
}

/* ============================================================================================
 * Handler 0x801066FC (Tabellen-Zeilen 7 und 8, Spalte 1) — der RAGDOLL-STURZ mit Bounce-Physik.
 * Vollstaendig disassembliert 2026-08-18 (0x801066FC..0x80106F1C, EMOVL10_S0.BIN @0x80100000).
 *
 * ---- Der Kopf @0x80106738-98 ----
 *   `lbu v0,561(s2)` = +0x231: 1 -> `jal 0x80109610`, 2 -> `jal 0x801092C4`, danach return.
 *   Beide Untermaschinen sind im Port OHNE Zwilling (OPEN) — und im Port UNERREICHBAR, weil
 *   ihre einzigen Setzer (s.u.) alle drei an Bedingungen haengen, die der Port nicht erzeugt.
 * ---- Phase 0 @0x8010679C ----
 *   `lw 0x800CFBF8; bgez` -> < 0 setzt +0x231 = 2 (Global ohne Port-Produzent, OPEN)
 *   `lh v0,268(s2)` (+0x10C) != 0  ODER  `lw 0x800CFBD8 & 0x10000000` -> +0x231 = 1 (OPEN)
 *   sonst: `jal 0x80015FE8` @0x801067F4 (der Wurf laeuft IMMER), und NUR wenn (rand&3) != 0
 *   UND `lbu v1,4(s2)` == 3 -> +0x231 = 1. +0x4 ist in diesem Handler zwangslaeufig 2 (die
 *   Wurzel 0x80104F40 ist der HURT-Zustand), der Zweig ist also strukturell tot — der Wurf
 *   selbst aber Verhalten und wird nachgezogen.
 *   Danach @0x8010683C-89C: +0x6 = 1; +0x13C/+0x13E/+0x140 = 0; +0x158 = 0; +0x16B = 0;
 *   +0x14C = 0x30004 (Clip 4, Rate 3; `& 0x20` von +0x1D0 -> Clip 3, OPEN); +0x148 = 0;
 *   +0x144 = -250 (bzw. +100 bei +0x1D0 & 0x20 — die Tabelle sp+16/sp+18 @0x80106728-34);
 *   `jal 0x800152C8` @0x80106898 wendet den Schub an.
 *   @0x801068A0-4C Gore-Emitter (FUN_8001BF10) — 4 feste RNG-Wuerfe plus 2 je Durchlauf einer
 *   Schleife mit (rand&3)+2 Durchlaeufen. Kein FX-Zwilling im Port; die WURF-ZAHL ist Verhalten.
 *   @0x80106950-90 SE: (rand&1)==0 -> 13 sonst 12, nur bei +0x239 == 0, danach +0x239 = 150;
 *   dann UNBEDINGT SE 9. @0x80106994-A0 +0x232 = +0x1C2 (Boden-Y). @0x8010699C
 *   `jal 0x80018FB0` = Pad-Rumble (liest +0x1CE, ruft 0x8007730C) — reine Praesentation.
 *   @0x801069A4-B0 +0x1D3 |= 0x80 (Spieler-Claim). Danach FALLTHROUGH in Phase 1.
 * ---- Phase 1 @0x801069B4 ----
 *   FUN_80015E7C + FUN_8002959C(a3=1024) + FUN_800152C8; `+0x6 += Advance-Rueckgabe`
 *   (@0x801069E8-F0). Wird +0x6 dabei 2, folgt SOFORT die Bone-Injektion @0x80106A04-3C.
 *   @0x80106A40-84: +0x14D != 0 && (+0x14D & 3) == 0 -> Blut (2 RNG-Wuerfe).
 * ---- Phase 2 @0x80106A8C ---- der Aufschlag-Abschluss:
 *   +0x21A &= ~4, +0x1D3 &= 0x7F, Modell-Felder (Praesentation), +0x3C = +0x232,
 *   +0x21A |= 1, +0x10E = 0x2001 (Bit 0 = KRIECHER), Hitbox 200/200/200/200/-350/350,
 *   +0x4 = 1 (WORT -> Zustand 1, Sub 0), +0x14C = 0xF0005 (Clip 5), word0-Bits 0x0C000000 -> 0x04000000.
 * ---- Der gemeinsame Schwanz @0x80106B5C ----
 *   Nur bei +0x6 < 2: drei Frame-Fenster ueber +0x14D rampen den Lean-Vektor
 *     0..9   : +0x13E = 0 ; +0x13C -= (s8)+0x16B*16 ; +0x140 += (s8)+0x16B*24 ; +0x16B += 1
 *     11..29 : +0x13C += (s8)+0x16B*8 ; +0x13E -= +0x158*2 ; +0x140 -= (s8)+0x16B*2 ;
 *              +0x158 += 1 ; +0x16B -= 1                       (alle Faktoren = ALTE Werte)
 *     31..49 : +0x13C -= (s8)+0x16B*4 ; +0x13E += +0x158*2 ; +0x140 -= (s8)+0x16B*2 ;
 *              +0x158 -= 1 ; +0x16B += 1
 *   danach IMMER die Bone-Injektion @0x80106CA0-D4.
 *   @0x80106CD8-D8C: +0x14D == 20 && +0x6 == 1 -> Aufschlag: +0x15A = 10, +0x16A = 2,
 *     Staub-FX (1 RNG-Wurf) und SE 9.
 *   @0x80106D90-F10: nur +0x14D >= 20 && +0x6 == 1 — die BOUNCE-PHYSIK:
 *     +0x16A != 0 -> +0x3C += +0x15A ; +0x15A += (+0x14D < 35) ? 5 : 55
 *     (+0x232 - 200 - Wurzel-Bone-Y) < +0x3C  UND  +0x16A != 0 ->
 *         +0x15A = -((s16)+0x15A >> 3) ; +0x16A -= 1 ; +0x3C = +0x232 - 300 - Wurzel-Bone-Y ;
 *         SE 4.
 *     "Wurzel-Bone-Y" = `lw v1,48(s3)` = die Translation Y der Part-0-Matrix (MATRIX +24, t[1]
 *     bei +20 -> Part +48). Im Port ist genau das die gerenderte Wurzel-Translation:
 *     skeleton_common.c setzt poses[0].trans[1] = rt_y = die Keyframe-py des laufenden Clips.
 * ========================================================================================== */

/* Die Wurzel-Translation Y des laufenden Clips (== Original `lw 48(s3)`, s. oben). Liest
 * dieselbe Bank, aus der der Renderer posiert; ohne Bank 0 (dann ist der Boden-Test die reine
 * `+0x232 - 200 < y`-Ebene und die Physik laeuft trotzdem, statt einzufrieren). */
static int re2z_root_py(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    const re15_emd_animation_t *an = &b->anim;
    const re15_emd_skeleton_t  *sk = &b->skel;
    int clip = (int)e->motion;
    if (clip < 0 || clip >= an->clip_count) return 0;
    int fc = an->clips[clip].frame_count;
    if (fc <= 0) return 0;
    int slot = (int)(e->anim_frame % (uint32_t)fc);
    int fi   = an->clips[clip].first_frame + slot;
    if (fi < 0 || fi >= an->frame_count) return 0;
    int16_t px = 0, py = 0, pz = 0;
    re15_emd_get_keyframe_position(sk, (int)(an->frames[fi] & 0xfffu), &px, &py, &pz);
    (void)px; (void)pz;
    return (int)py;
}

/* `death` = 1: die Wurzel ist die DEATH-Tabelle (+0x4 == 3, Zelle RE2ZD_66FC, Zeile 7 =
 * SCHROTFLINTE). Das schaltet die zwei `lbu v1,4` -Zweige scharf, die im HURT strukturell tot
 * sind: P0 @0x80106808-34 (3/4 -> +0x231 = 1 -> FUN_80109610 = WEGSCHLEUDERN mit
 * Fleischbrocken) und P2 @0x80106ACC-E8 (HP = 10 -> der Zombie steht als KRIECHER wieder auf). */
static void re2z_hit_ragdoll(re15_actor_t *e, re15_actor_t *pl, int death)
{
    if (e->re2z_rag231 == 1u) {                                    /* `bne v0,t2` @0x80106740 */
        re2z_death_burst(e, pl);                                   /* jal 0x80109610 @0x80106748 */
        return;                                                    /* j 0x80106F14 */
    }
    if (e->re2z_rag231 == 2u) {                                    /* `beq v0,a0` @0x8010675C */
        re2z_death_magnum(e, pl);                                  /* jal 0x801092C4 @0x801067BC */
        return;                                                    /* j 0x80106F14 */
    }

    switch (e->sub_state_2) {
    case 0:
        /* `0x800CFBF8 < 0` @0x8010679C-B0 (-> +0x231 = 2 -> FUN_801092C4) und `+0x10C != 0` /
         * `0x800CFBD8 & 0x10000000` @0x801067CC-EC (-> +0x231 = 1 -> FUN_80109610): drei
         * Eingaben ohne Port-Produzenten (OPEN, mit Adresse) -> der Pfad faellt bis zum Wurf. */
        if ((re2z_rand() & 3u) != 0u && death) {                   /* jal @0x801067F4 (IMMER),
                                                                    * `andi 0x3 / beq zero`
                                                                    * @0x801067FC-800,
                                                                    * `lbu v1,4 / bne 3`
                                                                    * @0x80106808-10 */
            e->re2z_rag231 = 1u;                                   /* sb v0(=1),561 @0x8010681C */
            re2z_death_burst(e, pl);                               /* jal 0x80109610 @0x80106828 */
            return;                                                /* j 0x80106F14 */
        }
        e->sub_state_2  = 1;                                       /* sb 1,6 @0x8010684C */
        e->re2_lean[0]  = 0;                                       /* sh zero,316 @0x80106858 */
        e->re2_lean[1]  = 0;                                       /* sh zero,318 @0x8010685C */
        e->re2_lean[2]  = 0;                                       /* sh zero,320 @0x80106860 */
        e->re2z_t158    = 0;                                       /* sh zero,344 @0x80106864 */
        e->re2z_gaitrow = 0;                                       /* sb zero,363 @0x80106868 */
        /* RUECKEN-TREFFER (`+0x1D0 & 0x20`) — Welle F, jetzt verdrahtet. Exakt dieselbe
         * Zwei-Instruktions-Mechanik wie im Stagger-P0, nur mit anderer Schub-Tabelle:
         *   80106728: addiu v0,zero,-250  /  8010672c: sh v0,16(sp)   ; Front
         *   80106730: addiu v0,zero,100   /  80106734: sh v0,18(sp)   ; Ruecken
         *   8010686c: andi v1,v1,0x20
         *   80106870: sltu v1,zero,v1
         *   80106874: subu a2,a2,v1       ; Clip-Wort 0x00030004 -> 0x00030003
         *   80106878: srl  v0,v0,4  /  8010687c: andi v0,v0,0x2      ; Byte-Index 0/2
         *   80106888: sw   a2,332(s2)     ; +0x14C
         *   8010688c: lhu  v0,0(v0)  /  8010689c: sh v0,324(s2)      ; +0x144
         * Ein Treffer in den Ruecken kippt den Zombie also NACH VORN (+100) statt nach
         * hinten (-250), mit dem gespiegelten Sturz-Clip 3. */
        {   int back = (e->re2z_hitdir1d0 & 0x20u) != 0;           /* @0x8010686C-74 */
            re2z_clip(e, back ? 3 : 4, 0, 3, 0x400, 0);            /* sw a2,332 @0x80106888,
                                                                    * Advance-Blend 1024 @0x801069D8 */
            re2z_thrust(e, back ? 100 : -250);                     /* sh v0,324 @0x8010689C +
                                                                    * FUN_800152C8 @0x80106898 */
        }
        /* Gore-Emitter @0x801068A0-4C: 4 feste Wuerfe, danach (rand&3)+2 Durchlaeufe zu je 2.
         * FX-Zwilling fehlt (Lane-I), die WURF-ZAHL ist Verhalten und wird exakt nachgezogen. */
        (void)re2z_rand();                                         /* @0x801068C0 */
        (void)re2z_rand();                                         /* @0x801068E0 */
        (void)re2z_rand();                                         /* @0x801068F4 */
        {   uint32_t n = (re2z_rand() & 3u) + 2u;                  /* @0x80106910-1C */
            for (uint32_t i = 0; i < n; i++) {
                (void)re2z_rand();                                 /* @0x80106920 */
                (void)re2z_rand();                                 /* @0x8010692C */
            }
        }
        re2z_blood_fx_at(e, 0, (int16_t)e->rot_y);                 /* Anker `addiu s1,s3,72` = Part 0
                                                                    * @0x801068D0, Emitter
                                                                    * @0x801068D8 / @0x80106908 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80106950-58 */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* @0x80106960-78 */
            e->re2z_cd239 = 150;                                   /* @0x80106980-84 */
        }
        re2z_se(9);                                                /* @0x80106988-90, unbedingt */
        e->re2z_gy232   = (int16_t)e->y;                           /* +0x232 = +0x1C2 @0x80106994-A0.
                                                                    * +0x1C2 ist das Boden-Y der
                                                                    * Entity: der EXE-Produzent
                                                                    * schreibt +0x3C und +0x1C2 aus
                                                                    * DEMSELBEN Boden-Query
                                                                    * (`sw v0,60(s0)` / `sh v0,450(s0)`
                                                                    * @0x8003EE04-18) und zieht beide
                                                                    * auf Treppen gemeinsam nach
                                                                    * (@0x8003EAA0-AC). */
        e->re2z_self1d3 |= 0x80u;                                  /* +0x1D3 |= 0x80 @0x801069A4-B0 */
        /* FALLTHROUGH @0x801069B4 — Phase 1 laeuft im selben Frame */
        /* FALLTHROUGH */
    case 1:
        re2z_hit_move(e);                                          /* FUN_80015E7C @0x801069C0 +
                                                                    * FUN_800152C8 @0x801069EC */
        if (re2z_clip_done(e))                                     /* +0x6 += Advance @0x801069E8-F0 */
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);
        break;
    default:                                                       /* Phase 2 @0x80106A8C */
        e->re2z_flags21a &= (uint16_t)~4u;                         /* andi 0xfffb @0x80106A98 */
        e->re2z_self1d3  &= 0x7fu;                                 /* andi 0x7f    @0x80106A9C */
        e->y              = (int32_t)e->re2z_gy232;                /* +0x3C = +0x232 @0x80106AC8 */
        e->re2z_flags21a |= 1u;                                    /* ori 1 @0x80106AD0-D4 */
        if (death) e->hp = 10;                                     /* `lbu v1,4 / bne 3 / sh 10,342`
                                                                    * @0x80106ACC-E8 — der DEATH-
                                                                    * Ausgang belebt als KRIECHER
                                                                    * wieder (+0x10E Bit 0 unten) */
        e->re2z_f10e      = 0x2001u;                               /* sh 8193,270 @0x80106B0C-10 —
                                                                    * Bit 0 schaltet die Trefferwurzel
                                                                    * auf die KRIECHER-Tabelle
                                                                    * @0x8010CBE8 (@0x80104FE0-500C) */
        re2z_clip(e, 5, 0, 0xF, 0x100, 0);                         /* sw 0xF0005,332 @0x80106B40 */
        re15_ai_set_state_word(e, 0x1);                            /* sw 1,4 @0x80106B3C (WORT) */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        /* Hitbox-/word0-Felder @0x80106B14-50 sind Modell-/Kollisions-Praesentation ohne
         * Port-Zwilling (OPEN). */
        return;                                                    /* j 0x80106F14 */
    }

    /* ---- der gemeinsame Schwanz @0x80106B5C ---- */
    int frame = re2z_frame_slot(e);                                /* +0x14D */
    if (e->sub_state_2 < 2) {                                      /* sltiu 2 @0x80106B64-68 */
        if (frame < 10) {                                          /* sltiu 0xa @0x80106B78 */
            int8_t k = (int8_t)e->re2z_gaitrow;                    /* lb 363 @0x80106B88/90 */
            e->re2_lean[1]  = 0;                                   /* sh zero,318 @0x80106B94 */
            e->re2_lean[0]  = (int16_t)(e->re2_lean[0] - k * 16);  /* sll 4 / subu @0x80106B9C-A4 */
            e->re2_lean[2]  = (int16_t)(e->re2_lean[2] + k * 24);  /* *3<<3 @0x80106BA8-C0 */
            e->re2z_gaitrow = (uint8_t)(e->re2z_gaitrow + 1u);     /* sb @0x80106BB8 */
        }
        if ((unsigned)(frame - 11) < 0x13u) {                      /* 11..29 @0x80106BCC-D4 */
            int8_t  k = (int8_t)e->re2z_gaitrow;                   /* ALTE Werte @0x80106BE4/C08 */
            int16_t t = e->re2z_t158;                              /* lh 344 @0x80106BF4 */
            e->re2_lean[0]  = (int16_t)(e->re2_lean[0] + k * 8);   /* sll 3 @0x80106BEC */
            e->re2_lean[1]  = (int16_t)(e->re2_lean[1] - t * 2);   /* sll 1 / subu @0x80106C00-04 */
            e->re2_lean[2]  = (int16_t)(e->re2_lean[2] - k * 2);   /* sll 1 / subu @0x80106C24-28 */
            e->re2z_t158    = (int16_t)(e->re2z_t158 + 1);         /* sh @0x80106C1C */
            e->re2z_gaitrow = (uint8_t)(e->re2z_gaitrow - 1u);     /* sb @0x80106C20 */
        }
        if ((unsigned)(frame - 31) < 0x13u) {                      /* 31..49 @0x80106C38-40 */
            int8_t  k = (int8_t)e->re2z_gaitrow;                   /* @0x80106C50/C74 */
            int16_t t = e->re2z_t158;                              /* lh 344 @0x80106C60 */
            e->re2_lean[0]  = (int16_t)(e->re2_lean[0] - k * 4);   /* sll 2 / subu @0x80106C58-5C */
            e->re2_lean[1]  = (int16_t)(e->re2_lean[1] + t * 2);   /* sll 1 / addu @0x80106C6C-70 */
            e->re2_lean[2]  = (int16_t)(e->re2_lean[2] - k * 2);   /* sll 1 / subu @0x80106C90-94 */
            e->re2z_t158    = (int16_t)(e->re2z_t158 - 1);         /* sh @0x80106C88 */
            e->re2z_gaitrow = (uint8_t)(e->re2z_gaitrow + 1u);     /* sb @0x80106C8C */
        }
    }
    /* Die Injektion laeuft in BEIDEN Faellen: fuer +0x6 < 2 hier (@0x80106CA0-D4), fuer den
     * Uebergangs-Tick nach +0x6 == 2 schon oben (@0x80106A04-3C) — beides dieselbe Operation. */
    re2z_lean_pair(e, e->re2_lean[0], e->re2_lean[1], e->re2_lean[2]);

    if (frame == 20 && e->sub_state_2 == 1) {                      /* @0x80106CD8-F0 */
        e->re2z_t15a   = 10;                                       /* +0x15A = 10 @0x80106D50-54 */
        e->re2z_dir16a = 2;                                        /* +0x16A = 2  @0x80106D58-5C */
        (void)re2z_rand();                                         /* Staub-FX-Wurf @0x80106D64 */
        re2z_blood_fx_at(e, 1, (int16_t)e->rot_y);                 /* FUN_8001BF10 @0x80106D7C,
                                                                    * Anker `addiu a2,s0,244`
                                                                    * @0x80106D78 = Part 1 */
        re2z_se(9);                                                /* @0x80106D84-8C */
    }
    if (frame < 20 || e->sub_state_2 != 1) return;                 /* @0x80106D98-AC */

    /* ---- BOUNCE-PHYSIK @0x80106E64-F10 ---- */
    int rooty = re2z_root_py(e);                                   /* `lw 48(s3)` @0x80106E64/EAC */
    if ((int8_t)e->re2z_dir16a != 0) {                             /* lb 362 / beq @0x80106E68-70 */
        e->y = e->y + (int)e->re2z_t15a;                           /* +0x3C += +0x15A @0x80106E84-88 */
        e->re2z_t15a = (int16_t)(e->re2z_t15a + (frame < 35 ? 5 : 55));
                                                                   /* +5 @0x80106E90/9C, +55 bei
                                                                    * +0x14D >= 35 @0x80106EA0-A4 */
    }
    if ((int)e->re2z_gy232 - 200 - rooty < e->y                    /* slt @0x80106EB4-C0 */
        && (int8_t)e->re2z_dir16a != 0) {                          /* lb 362 @0x80106EC8-D0 */
        e->re2z_t15a   = (int16_t)(-(int)(e->re2z_t15a >> 3));     /* sra 19 / negiert @0x80106EE4-F4 */
        e->re2z_dir16a = (uint8_t)(e->re2z_dir16a - 1u);           /* sb @0x80106F00-08 */
        e->y           = (int)e->re2z_gy232 - 300 - rooty;         /* sw 60 @0x80106EF0-F10 */
        re2z_se(4);                                                /* jal @0x80106F0C, a0 = 4 */
    }
}

/* ============================================================================================
 * Handler 0x8010703C (Tabellen-Zeile 12, Spalte 1) — der RUTSCH-RUECKSTOSS mit Bone-Blend.
 * Vollstaendig disassembliert 2026-08-18. Sprungtabelle @0x80100104 (7 Eintraege, selbst gedumpt):
 *   [0] 0x80107080  [1] 0x801071A8  [2] 0x80107248  [3] 0x80107274
 *   [4] 0x8010732C  [5] 0x8010733C  [6] 0x801073E8      (`sltiu v0,v1,0x7` @0x8010705C)
 *
 * P0 @0x80107080: liest den TREFFER-DATENSATZ `lw s2,512(s1)` (+0x200):
 *      rec+0x99 -> +0x16A (getroffener Part); ist er 3 ODER 6 -> +0x1D2 = 3 und SOFORT return
 *      (@0x8010709C-B4 — das ist der einzige BRACKET-Erzeuger, den ich im Overlay gefunden habe).
 *      sonst: +0x6 = 1; +0x158 = rec+0x9A (Rueckstoss-Winkel); +0x14C = 0xF0000 + +0x218
 *      (Walk-Clip, Rate 15); Modell-Flags &= ~0x40; SE (rand&1)==0 ? 13 : 12 bei +0x239 == 0,
 *      danach +0x239 = 150; +0x144 = 300, +0x146 = +0x148 = 0, +0x15A = 0, +0x16B = 50;
 *      FUN_800154AC (Peilung zum Spieler) und ein Blut-Emitter am Part +0x16A (1 RNG-Wurf).
 *      FALLTHROUGH in P1.
 * P1 @0x801071A8: RotMatrix(0,+0x158,0) -> ApplyMatrixLV 0x8008DBA4 auf (+0x144,+0x146,+0x148)
 *      -> +0x38 += out.x, +0x40 += out.z; FUN_8002959C(a3=256);
 *      +0x144 -= +0x15A ; +0x15A += (s8)+0x16B   (beide mit den ALTEN Werten, @0x801071F0-228)
 *      +0x144 < 0 -> +0x6 = 6.
 * P2 @0x80107248: +0x6 = 3; +0x16B = (rand&0x3F)+30; +0x158 = 4096; +0x15A = Wurzel-Bone-Y.
 * P3 @0x80107274: +0x16B--; bei 0 -> +0x6 = 4. Advance(256), dann FUN_80028F48(Identitaet
 *      @0x8009DB44, Kopie der Part-0-Matrix, Part 0, w = +0x158) und Wurzel-Y zurueckschreiben;
 *      +0x158 > 1024 -> +0x158 -= 512.
 * P4 @0x8010732C: +0x6 = 5; +0x16B = 20 (FALLTHROUGH in P5).
 * P5 @0x8010733C: +0x16B--; bei 0 -> +0x6 = 6. Advance(256), derselbe Blend, +0x158 += 153.
 * P6 @0x801073E8: Modell-Flags &= ~0x40; +0x4 = 0x101; (rand&1) != 0 -> +0x4 = 0x201.
 *
 * ⛔ OPEN: der Treffer-Datensatz +0x200 (rec+0x99 Part, rec+0x9A Winkel) hat im Port KEINEN
 * Produzenten — der Port-Hitscan FUN_80011F50 fuehrt weder Trefferpart noch Rueckstoss-Winkel.
 * Der Port setzt darum +0x16A = 0 (Part 0 = Wurzel, der auch die Bone-Blend-Stelle ist) und
 * +0x158 = die AKTUELLE Blickrichtung +0x6A, was den Schub nach der Original-Formel
 * R_y(+0x158) * (300,0,0) exakt nach VORNE legt; jede andere Zahl waere geraten. Der
 * `rec+0x99 in {3,6}` -> +0x1D2 = 3-Zweig bleibt damit unerreichbar und ist als Bracket-Beleg
 * nur dokumentiert (Bracket bleibt insgesamt OPEN, wie im Auftrag festgehalten).
 * ========================================================================================== */
static void re2z_hit_slide(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0:
        /* +0x200 OPEN (s.o.): +0x16A = 0, +0x158 = eigene Blickrichtung. */
        e->re2z_dir16a = 0;                                        /* sb +0x99,362 @0x80107090 */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x801070C0 */
        e->re2z_t158   = (int16_t)(e->rot_y & 0xfff);              /* sh +0x9A,344 @0x801070C8 */
        re2z_clip(e, e->re2z_walkclip, 0, 0xF, 0x100, 0);          /* 0xF0000+ +0x218 @0x801070D4 */
        if (e->re2z_cd239 == 0) {                                  /* @0x801070E8-F0 */
            re2z_se((re2z_rand() & 1u) ? 12 : 13);                 /* @0x801070F8-110 */
            e->re2z_cd239 = 150;                                   /* @0x80107118-1C */
        }
        e->speed_h      = 300;                                     /* +0x144 = 300 @0x8010712C */
        e->re2z_t15a    = 0;                                       /* +0x15A = 0   @0x8010713C */
        e->re2z_gaitrow = 50;                                      /* +0x16B = 50  @0x80107140 */
        (void)re2z_rand();                                         /* Blut-Wurf @0x80107184 */
        /* Anker ist HIER kein fester Part: `lb a0,362(s1)` (+0x16A) @0x8010715C, dann
         *   80107164-80: v1 = a0*172   (sll/addu/subu-Kette: 3a<<2 -a =11a, <<2 -a =43a, <<2)
         *   80107188: _addu s2,a0,v1   ; a0 = +0x198
         *   8010719c: addiu a2,s2,72   ; = &Part[+0x16A] + 0x48
         * -> der Anker ist der Part mit dem Index (s8)+0x16A. */
        re2z_blood_fx_at(e, (int)(int8_t)e->re2z_dir16a, (int16_t)e->rot_y); /* @0x801071A0 */
        /* FALLTHROUGH @0x801071A8 */
        /* FALLTHROUGH */
    case 1:
        {   /* R_y(+0x158) * (+0x144, 0, 0) -> +0x38/+0x40 (@0x801071B0-208).
             * Die Port-Trigonometrie ist dieselbe wie in re2z_thrust (dort mit +0x6A). */
            int spd = (int)e->speed_h;
            int yaw = (int)e->re2z_t158 & 0xfff;
            e->x += (int32_t)(((int32_t)re15_cos_q12(yaw) * spd) >> 12);
            e->z -= (int32_t)(((int32_t)re15_sin_q12(yaw) * spd) >> 12);
            int16_t old_v = (int16_t)e->re2z_t15a;                 /* ALTE Werte @0x801071F8/214 */
            e->speed_h    = (int16_t)(e->speed_h - old_v);         /* sh @0x8010721C */
            e->re2z_t15a  = (int16_t)(old_v + (int8_t)e->re2z_gaitrow); /* sh @0x80107228 */
            if ((int16_t)e->speed_h < 0)                           /* bgez @0x80107234 */
                e->sub_state_2 = 6;                                /* sb 6,6 @0x80107244 */
        }
        break;
    case 2:
        e->sub_state_2  = 3;                                       /* sb 3,6 @0x80107254 */
        e->re2z_gaitrow = (uint8_t)((re2z_rand() & 0x3fu) + 30u);  /* @0x80107258-60 */
        e->re2z_t158    = 4096;                                    /* +0x158 = 4096 @0x8010726C */
        e->re2z_t15a    = (int16_t)re2z_root_py(e);                /* +0x15A = `lw 48(s2)` @0x80107270 */
        /* FALLTHROUGH @0x80107274 */
        /* FALLTHROUGH */
    case 3:
        {   uint8_t old = e->re2z_gaitrow;
            e->re2z_gaitrow = (uint8_t)(old - 1u);                 /* sb @0x80107284 */
            if (old == 0u) e->sub_state_2 = 4;                     /* @0x80107288-8C */
        }
        e->re2_bone0_wgt = e->re2z_t158;                           /* FUN_80028F48 w @0x801072F4-F8 */
        if ((int16_t)e->re2z_t158 >= 1025)                         /* slti 1025 @0x80107318 */
            e->re2z_t158 = (int16_t)(e->re2z_t158 - 512);          /* @0x80107320-28 */
        break;
    case 4:
        e->sub_state_2  = 5;                                       /* sb 5,6  @0x80107330 */
        e->re2z_gaitrow = 20;                                      /* +0x16B  @0x80107338 */
        /* FALLTHROUGH @0x8010733C */
        /* FALLTHROUGH */
    case 5:
        {   uint8_t old = e->re2z_gaitrow;
            e->re2z_gaitrow = (uint8_t)(old - 1u);                 /* sb @0x8010734C */
            if (old == 0u) e->sub_state_2 = 6;                     /* @0x80107350-54 */
        }
        e->re2_bone0_wgt = e->re2z_t158;                           /* FUN_80028F48 w @0x801073BC-C0 */
        e->re2z_t158 = (int16_t)((uint16_t)e->re2z_t158 + 153u);   /* @0x801073D4-E4 */
        break;
    default:                                                       /* P6 @0x801073E8 */
        re15_ai_set_state_word(e, 0x101);                          /* sw 257,4 @0x80107408 */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        if ((re2z_rand() & 1u) != 0u)                              /* @0x8010740C-10 */
            re15_ai_set_state_word(e, 0x201);                      /* sw 513,4 @0x80107418 */
        break;
    }
    (void)pl;
}

/* ============================================================================================
 * Handler 0x80107438 (Spalte 0 der Zeilen 7/8/9/12) — der KNOCKDOWN mit ARM-ABRISS.
 * Vollstaendig disassembliert 2026-08-18 (0x80107438..0x80107884).
 * Phasen-Dispatch auf +0x6: 0 -> 0x801074B0, 1 -> 0x80107734, 2 -> 0x8010777C, sonst Epilog
 * (`beq v1,1` @0x80107478, `slti 2`/`beq zero` @0x80107480-8C, `beq v1,2` @0x801074A0).
 *
 * P0 @0x801074B0: `+0x14C = 0x000F0001` (Clip 1, Rate 15; `lui v0,0xf / ori 1 / sw v0,332`
 *   @0x80107490-C8), zwei bare FUN_80015E7C (@0x801074C4/@0x801074E8 — sie fuellen nur den
 *   Wurzel-Delta-Vektor nach, angewendet wird er erst in P1 durch FUN_800152C8, also im Port
 *   keine Bewegung in P0), dazwischen ein RNG-Wurf fuer den ZUFALLS-STARTFRAME
 *   `+0x14D = rand & 7` (@0x801074CC-EC), `+0x1D3 |= 0x80` (@0x801074F0-504), `+0x6 = 1`
 *   (@0x801074F8). Zeile 12 (Bowgun) springt sofort zum SE-Block (@0x8010750C).
 *   Sonst `+0x16B = rand & 1` (@0x80107514-24): Bit 0 -> nur die Blutfontaene, Bit 1 -> ARM AB.
 * P1 @0x80107734: FUN_80015E7C + FUN_8002959C(a3=256) + FUN_800152C8, `+0x6 += Advance`
 *   (@0x80107740-6C).
 * P2 @0x8010777C: `+0x4 == 3` -> HP-Wiederbelebung (in HURT strukturell tot, s. Ragdoll).
 *   `+0x10C != 0` ODER `0x800CFBD8 & 0x10000000` -> `+0x4 = 7` (WORT) und `+0x21A &= ~4`
 *   (@0x801077B4-F0); beide Eingaben haben im Port keinen Produzenten (OPEN, wie im Ragdoll).
 *   Sonst: Hitbox 200/200/-350/200/200/350 (@0x801077F8-818, Praesentation ohne Port-Zwilling),
 *   `+0x10E = 0x2001` (KRIECHER-Bit 0, @0x80107820-24), word0 = (word0 & 0xF3FFFFFF) | 0x04000000
 *   (@0x801077D8/@0x80107828-38), ein RNG-Wurf -> `+0x4 = 1` bzw. `0x201` (@0x80107834-4C),
 *   `+0x1D3 &= 0x7F` (@0x80107850-5C).
 * ========================================================================================== */
/* Die Drift-Felder eines abgerissenen ARMS (Flagwort 0x4A). Die drei Vorkommen @0x80107554-74,
 * @0x801075C4-E4 und @0x80107640-60 sind wortgleich; die Offsets folgen dem Part-Stride 172
 * (Part 3: 676/672/670/674/680/668 - 516; Part 5: ... - 860; Part 6: ... - 1032).
 *   +0xA0 = 0    (Lebensdauer)      sh zero,676(v1)  @0x80107554
 *   +0x9C = 0    (Vortrieb)         sh zero,672(v1)  @0x80107558
 *   +0x9A = -10  (vy)               sh s3,670(v1)    @0x8010755C   (s3 = -10 @0x80107540)
 *   +0x9E = 0    (vy-Zuwachs)       sh zero,674(v1)  @0x80107560
 *   +0xA4 = 0    (Vortriebs-Zuwachs)sh zero,680(v1)  @0x80107564
 *   +0x98 = +0x76 + 2048 (Kurs)     sh v0,668(v1)    @0x8010756C-74
 * Der Arm steigt also mit 10 Einheiten je Frame und verschwindet nach 29 Frames
 * (`sltiu v0,v0,0x1d` @0x80028DCC / `sw zero,0(s0)` @0x80028DDC) — KEIN Vortrieb, keine
 * Gravitation, keine Kaskade (Flagwort wird 0, nicht 0x20). */
static void re2z_arm_throw(re15_actor_t *e, int p)
{
    e->re2z_part_life [p] = 0;
    e->re2z_part_w9c  [p] = 0;
    e->re2z_part_w9a  [p] = -10;
    e->re2z_part_w9e  [p] = 0;
    e->re2z_part_wa4  [p] = 0;
    e->re2z_part_yaw98[p] = (int16_t)(e->rot_y + 2048);
}

static void re2z_knockdown_gore(re15_actor_t *e)
{
    uint32_t r = re2z_rand();                                      /* @0x80107514 */
    e->re2z_gaitrow = (uint8_t)(r & 1u);                           /* sb v0,363 @0x80107524 */
    if (r & 1u) {
        /* RECHTER UNTERARM AB, RECHTE HAND WEG (@0x80107534-88) */
        e->re2z_part_flags[3] |= 0x4Au;                            /* ori 0x4A @0x80107544-48 */
        e->re2z_part_tint [3]  = 0x0010104Fu;                      /* sw s1,628 @0x80107568 */
        re2z_arm_throw(e, 3);                                      /* @0x80107554-74 */
        /* FUN_8001CEFC(5,3,Part 3) @0x80107570 = die am Teil haengenden Effekte abschalten
         * (Port: No-op, der Stand-in kennt keine Anker) */
        re2z_se(2);                                                /* jal 0x8005bd6c @0x80107584 */
        e->re2z_part_flags[4]  = 0u;                               /* sw zero,688 @0x80107588 */
        if (re2z_rand() & 1u) {                                    /* @0x8010758C-98 */
            /* LINKER OBERARM + UNTERARM AB, LINKE HAND WEG (@0x801075A4-7C) */
            e->re2z_part_flags[5] |= 0x4Au;                        /* ori 0x4A @0x801075B4-B8 */
            e->re2z_part_tint [5]  = 0x0010104Fu;                  /* sw s1,972 @0x801075D8 */
            re2z_arm_throw(e, 5);                                  /* @0x801075C4-E4 */
            re2z_gore_fx(e, 5, 6000u);                             /* Part 5 (`addiu s0,v1,932`
                                                                    * @0x801075B0) @0x801075E0 */
            (void)re2z_rand();                                     /* @0x801075E8 */
            re2z_gore_fx(e, 5, 0x08001000u);                       /* Part 5 (a2 = s0 @0x801075FC)
                                                                    * @0x80107600 */
            e->re2z_part_flags[6] |= 0x4Au;                        /* ori 0x4A @0x80107630-34 */
            e->re2z_part_tint [6]  = 0x0010104Fu;                  /* sw s1,1144 @0x80107654 */
            re2z_arm_throw(e, 6);                                  /* @0x80107640-60 */
            re2z_gore_fx(e, 6, 6000u);                             /* Part 6 (`addiu s0,v1,1104`
                                                                    * @0x8010762C) @0x8010765C */
            e->re2z_part_flags[7]  = 0u;                           /* sw zero,1204 @0x8010767C */
        }
    }
    /* Die Blutfontaene am Part 3 laeuft IMMER — der Abriss faellt hier durch (@0x80107680-708). */
    /* Alle vier Fontaenen haengen am PART 3 (`addiu s0,s0,588` @0x801076A0, a2 = s0). */
    re2z_gore_fx(e, 3, 5000u);                                     /* v={0,0,0}   @0x801076A4 */
    (void)re2z_rand();                                             /* @0x801076B0 */
    re2z_gore_fx(e, 3, 6000u);                                     /* v={0,400,0} @0x801076C4 */
    (void)re2z_rand();                                             /* @0x801076D0 */
    re2z_gore_fx(e, 3, 4000u);                                     /* v={0,800,0} @0x801076E4 */
    (void)re2z_rand();                                             /* @0x801076EC */
    re2z_gore_fx(e, 3, 0x08001000u);                               /* @0x80107704 */
}

/* `death` = 1: die Wurzel ist die DEATH-Tabelle (+0x4 == 3, Zelle RE2ZD_7438 — SPALTE 0 der
 * Zeilen 5..9/17). Damit wird der P2-Zweig `lbu v1,4 / bne v0(=3)` @0x8010777C-84 scharf:
 *   @0x8010778C-98  `rand & 3` != 0 -> HP = 10  (Wiederbelebung als Kriecher)
 *   @0x801077A0-A8  sonst entscheidet +0x16B (das Arm-Abriss-Byte aus re2z_knockdown_gore,
 *                   `sb v0,363` @0x80107524): Arm ab -> CORPSE, sonst ebenfalls HP = 10.
 * ⛔ Spalte 0 verlangt Zone 0; der Port stempelt immer Zone 1 (re15_re2_stamp_hit), die Zelle
 *    ist im Spiel also heute unerreichbar — dieselbe Lage wie Handler 5 der HURT-Tabelle. Der
 *    Zweig ist portiert und wird im Test durch einen Direktaufruf positiv kontrolliert. */
static void re2z_hit_knockdown(re15_actor_t *e, re15_actor_t *pl, int death)
{
    (void)pl;
    switch (e->sub_state_2) {
    case 0:
        re2z_clip(e, 1, 0, 0xF, 0x100, 0);                         /* 0xF0001 @0x801074C8 */
        e->anim_frame = (uint16_t)(re2z_rand() & 7u);              /* +0x14D @0x801074CC-EC */
        e->re2z_self1d3 |= 0x80u;                                  /* @0x801074F0-504 */
        e->sub_state_2   = 1;                                      /* sb 1,6 @0x801074F8 */
        if (e->sub_state_1 != 12u)                                 /* `beq v1,12` @0x8010750C */
            re2z_knockdown_gore(e);
        if (e->re2z_cd239 == 0) {                                  /* @0x8010770C-14 */
            re2z_se(12);                                           /* @0x80107718-20 */
            e->re2z_cd239 = 150;                                   /* @0x80107724-28 */
        }
        /* FUN_80018FB0 @0x8010772C = Pad-Rumble (Praesentation, wie im Ragdoll dokumentiert) */
        /* FALLTHROUGH @0x80107734 — P1 laeuft im selben Frame */
        /* FALLTHROUGH */
    case 1:
        re15_re2z_move_root(e);                                    /* e7c @0x80107740 + 152c8
                                                                    * @0x8010776C */
        if (re2z_clip_done(e))                                     /* +0x6 += Advance @0x80107754-70 */
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);
        break;
    default:                                                       /* P2 @0x8010777C */
        if (death) {                                               /* `lbu v1,4 / bne v0(=3)`
                                                                    * @0x8010777C-84 */
            int hp10 = 1;
            if ((re2z_rand() & 3u) == 0u) {                        /* jal @0x8010778C, `andi 0x3 /
                                                                    * bne -> 0x801077B0`
                                                                    * @0x80107794-98 */
                if ((int8_t)e->re2z_gaitrow != 0)                  /* `lb v0,363 / bne`
                                                                    * @0x801077A0-A8 */
                    hp10 = 0;
            }
            if (!hp10) {
                re15_ai_set_state_word(e, 7u);                     /* `addiu v1,zero,7 / sw v1,4`
                                                                    * @0x801077E0-E4 -> CORPSE */
                e->re2z_flags21a &= (uint16_t)~4u;                 /* andi 0xfffb @0x801077E8 /
                                                                    * sh @0x801077F0 */
                re2z_hit_latch_release(e);
                return;                                            /* j 0x80107860 */
            }
            e->hp = 10;                                            /* sh v0(=10),342 @0x801077B0 */
        }
        /* +0x10C und 0x800CFBD8 (@0x801077B4-D4) haben im Port keinen Produzenten (OPEN) ->
         * der Original-Pfad faellt in den KRIECHER-Ausgang. */
        e->re2z_f10e = 0x2001u;                                    /* sh 8193,270 @0x80107820-24 */
        re15_ai_set_state_word(e, (re2z_rand() & 1u) ? 1u : 0x201u);/* @0x80107834-4C */
        e->re2z_self1d3 &= 0x7fu;                                  /* andi 0x7f @0x80107850-5C */
        re2z_hit_latch_release(e);                                 /* Reaktion vorbei -> wieder
                                                                    * treffbar (@0x80105f9c-fac) */
        /* Hitbox-/word0-Felder @0x801077F8-838 = Modell-/Kollisions-Praesentation (OPEN). */
        return;
    }
}

/* ---- der Treffer-Stempel des Ports: +0x1D0 (Richtung) und die Zonen-Reserven ---------------
 * Im Original macht das der EXE-Applier (FUN_800410CC/FUN_800470C0), BEVOR die HURT-Wurzel
 * laeuft. Der Port hat in diesem Modul keinen Applier-Hook (re15_damage.c ist fuer diese Welle
 * gesperrt); die Flanke ist stattdessen `+0x6 == 0` in Zustand 2 — genau das setzt
 * re15_re2_stamp_hit je Treffer (`e->sub_state_2 = 0`, Beleg `sw v0,4(s1)` @0x80047288/90).
 * [PORT-MAPPING] nur fuer den AUFRUFZEITPUNKT; die Rechnung selbst ist byte-true:
 *   Richtung  d = FUN_800154AC(Angreifer.x/z, Ziel.x/z) - Ziel+0x76   (@0x800419D8-A08)
 *             FUN_800154AC liefert die Peilung Angreifer->Ziel mit 0 = +X (dx==0 -> 1024/3072
 *             @0x800154B0-E8, sonst `4096 - ratan(dz/dx)` bzw. `2048 - ratan(...)`
 *             @0x80015530-40). Der Port-Zwilling ist `re15_atan2_q12(dz,dx) - 1024`
 *             (re15_actor.h: "0 = +Z", minus 1024 = "0 = +X").
 *   Reserven  Region = +0x1D2 % 3 (`+0x1D2 = 3*Bracket + Region` @0x80041A88-9C),
 *             Kosten = (rec.w1 >> (Bracket*3)) & 7, Saettigung bei -1 (@0x80041954-88).
 * Die Kosten-Woerter sind selbst gedumpt (`read 0x800a412c 95 --w 4`), Zombie-Record-Tabelle
 * 0x800A412C, Stride 20, Zeile = Attacken-Id 1..19; hier steht word1 (+0x04) je Zeile. */
static const uint32_t re2z_pool_cost_w1[20] = {
    /* 0 unbenutzt */ 0u,
    0x078EFC14u, 0x02851014u, 0x02851014u, 0x00810214u, 0x078EFC0Au,   /*  1.. 5 */
    0x078EFC0Au, 0x078EFC0Au, 0x078EFC0Au, 0x078EFC0Au, 0x078EFC0Au,   /*  6..10 */
    0x078EFC0Au, 0x0000000Au, 0x02851014u, 0x078EFC0Au, 0x0081020Au,   /* 11..15 */
    0x0285100Au, 0x078EFC0Au, 0x0102040Au, 0x02851014u                 /* 16..19 */
};

/* `row` ist im Original IMMER +0x5 (der Applier hat es unmittelbar davor gestempelt). Der
 * Parameter existiert nur, damit die RE1.5-Bruecke unten dieselbe Rechnung mit ihrer eigenen,
 * NICHT nach +0x5 geschriebenen Zeile fahren kann — im RE1.5-Modus ist +0x5 der RE1.5-Reaktions-
 * clip und darf nicht ueberschrieben werden. */
static void re2z_stamp_hit_row(re15_actor_t *e, const re15_actor_t *pl, unsigned row)
{
    /* --- +0x1D0: Low-Byte je Treffer neu (`andi 0xff00` @0x80041384 / @0x80047178) --- */
    int bearing = ((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 1024) & 0xfff;
    int d       = bearing - ((int)e->rot_y & 0xfff);               /* subu v1,v0,v1 @0x80041A08 */
    uint16_t dir = (uint16_t)(e->re2z_hitdir1d0 & 0xff00u);
    if ((unsigned)((d + 1024) & 0xfff) < 2048u) dir |= 0x20u;      /* @0x80041A0C-2C */
    if ((unsigned)((d + 1536) & 0xfff) < 1024u) dir |= 0x40u;      /* @0x80041A30-58 */
    if ((unsigned)((d -  512) & 0xfff) < 1024u) dir |= 0x80u;      /* @0x80041A5C-84 */
    e->re2z_hitdir1d0 = dir;

    /* --- die Zonen-Reserve der getroffenen Region --- */
    unsigned region  = (unsigned)e->re2z_hits1d2 % 3u;             /* @0x80041A88-9C */
    unsigned bracket = (unsigned)e->re2z_hits1d2 / 3u;
    if (row >= 20u) return;
    int cost = (int)((re2z_pool_cost_w1[row] >> (bracket * 3u)) & 7u);  /* @0x80041954-70 */
    int8_t *pool = (region == 0u) ? &e->re2z_pool153                    /* @0x80041900-08 */
                 : (region == 1u) ? &e->re2z_pool152                    /* @0x80041950 */
                                  : &e->re2z_pool151;                   /* @0x80041994 */
    int v = (int)*pool - cost;                                          /* subu @0x80041974 */
    *pool = (int8_t)((v < 0) ? -1 : v);                                 /* Klemme @0x8004197C-88 */
}

static void re2z_stamp_hit(re15_actor_t *e, const re15_actor_t *pl)
{
    re2z_stamp_hit_row(e, pl, e->sub_state_1);                          /* `sb v1,5` @0x80041AB4 */
}

/* ============================================================================================
 * ⛔ DIE RE1.5-BRUECKE IN DEN ZERLEGER   (PORT-OPTION, Nutzer-Auftrag — s. Block bei
 *    re15_re15_re2z_import; KEIN byte-true RE1.5-Verhalten, sondern eine gewollte Uebernahme)
 * --------------------------------------------------------------------------------------------
 * WAS IM RE2-MODUS PASSIERT (und was diese Bruecke deshalb nachbaut): der Schuss-Applier
 * stempelt +0x1D2 = 1 und die Zeile +0x5 (re15_re2_stamp_hit, re15_damage.c), im naechsten
 * KI-Tick laeuft die HURT-Wurzel @0x80104F40 (re2z_hurt) und dort in dieser Reihenfolge
 *   (1) re2z_stamp_hit  = der Applier-Zwilling FUN_800410CC/FUN_800470C0 (+0x1D0-Richtung
 *       @0x80041A0C-84 und die Zonen-Reserve +0x151/+0x152/+0x153 @0x80041900-9C),
 *   (2) re2z_leg_gore   = DER ZERLEGER @0x80105288-3D8,
 *   (3) der 2D-Dispatch @0x801053E0-410.
 * Im RE1.5-Modus laeuft statt re2z_hurt das RE1.5-Gehirn (enemy_ai_common.c). Die Bruecke ruft
 * deshalb GENAU (1) + (2) + den (3)-Zweig, der Gore erzeugt — und NICHTS von der RE2-Zustands-
 * maschine. Sie schreibt AUSSCHLIESSLICH re2z_*-Felder (die das RE1.5-Gehirn nirgends liest);
 * +0x4/+0x5/+0x6/+0x7/hp/hit_react bleiben unberuehrt.
 *
 * ---- WARUM DAS MIT DER PISTOLE UEBERHAUPT ZUENDET -------------------------------------------
 * Das Dreifach-Gate des Zerlegers @0x80105288-B0 ist:
 *     +0x21A & 0x60 == 0   (Einweg: ein zweites Bein kommt nie)
 *     (s8)+0x152 < 0       (die MITTLERE Zonen-Reserve ist aufgebraucht)
 *     +0x1D0 & 0xC0 != 0   (der Treffer kam von der SEITE)
 * Die Reserve startet bei 13 (`addiu v0,zero,13` @0x8010081C), die Kosten stehen als Wort 1 der
 * Zombie-Angriffs-Tabelle 0x800A412C (re2z_pool_cost_w1) und werden mit `>> (Bracket*3) & 7`
 * gelesen (@0x80041954-70). Pistole = RE1.5-Waffe 3 -> RE2-Zeile 3 -> w1 = 0x02851014 ->
 * Bracket 0 -> Kosten 4. Region = +0x1D2 % 3 = 1 % 3 = 1 = +0x152. Also
 *     13 -> 9 -> 5 -> 1 -> (1-4 = -3, geklemmt) -1
 * und ab dem VIERTEN Pistolentreffer ist das Reserve-Gate offen; jeder weitere Treffer, der
 * seitlich einschlaegt (+0x1D0 & 0xC0), reisst das Bein ab. Das ist exakt dieselbe Rechnung,
 * die der RE2-Modus heute schon fahrt — die Bruecke erfindet nichts.
 *
 * ---- WAS BEWUSST NICHT MITKOMMT (mit Adresse, damit die Luecke benannt ist) ------------------
 *  (a) re2z_dismember_row @0x80105188-284 (Verkohlung/Aetzung/Russ/Elektro-Tinten der
 *      Granatwerfer-Zeilen 9/10/11/14/16) sitzt im ORIGINAL INNERHALB der Reaktions-Handler
 *      (Stagger-P0 @0x80105DC4, Haupt-P0 @0x80105510, Haupt-P3 @0x80105A50, DEATH @0x80108444) —
 *      nicht im Applier. Ohne die RE2-Handler gibt es dafuer keinen belegten Zeitpunkt.
 *  (b) re2z_stagger_acid_leg @0x80105E10-F10 haengt ebenfalls in Stagger-P0.
 *  (c) Der DEATH-Zweig @0x80108250 hat seinen eigenen Gore; die Bruecke feuert deshalb NUR,
 *      wenn der Zombie den Treffer UEBERLEBT (im Original ist re2z_leg_gore Teil der
 *      HURT-Wurzel, die bei +0x4 == 3 gar nicht mehr laeuft, `sw v0,4(s1)` @0x8004728C-90).
 *  (d) SE 9 (`addiu a0,zero,9` @0x801052B4 / `jal 0x8005bd6c` @0x801052B8): NACHGEZOGEN.
 *      platform/pc/main.c registriert den ENEMSE-Hook jetzt AUCH im RE1.5-Zweig von
 *      pc_enemy_load, gegated auf re15_re15_re2z_import_owns(type) — Bank 0 (== RE2Z_ENEMSE_BANK,
 *      EDT-Map-Eintrag 9 = 0x02A30000 -> prog 0, tone 10, Kanal 2, Prio-Nibble 3; Herleitung +
 *      RDT-Zensus in tests/unit/test_re15_re2z_gore_se.c). Der Frame-Flag-Pfad
 *      re15_re2z_se_play (0x801016c8) bleibt im RE1.5-Modus unerreichbar (sein einziger
 *      Aufrufer in enemy_ai_common.c steht hinter RE15_AI_FLAVOR_RE2), es kommt also NUR
 *      dieser Gore-SE dazu.
 *  (e) STUMPF-GEOMETRIE: der Zerleger stempelt `part_mesh = 15` (@0x8010531C-50) = der
 *      RESERVE-Part des RE2-MD1. Der RE1.5-Zombie-MD1 hat nur die Meshes 0..14 (selbst
 *      gemessen: mesh_count == bone_count == 15 fuer EM10/11/12/13/16/18), also greift die
 *      Klammer in platform/pc/main.c (`gore_mesh[nbi] < mesh_count`) und der Oberschenkel
 *      bleibt INTAKT. Schienbein + Fuss fliegen trotzdem weg — genau das Ergebnis, das der
 *      Nutzer im Modus "RE2 AI" (= RE1.5-Modelle!) als korrekt bestaetigt hat.
 * ========================================================================================== */
void re15_re15_re2z_gore_hit(re15_actor_t *e, const re15_actor_t *pl, int row_src, unsigned row_id)
{
    if (!e || !pl) return;
    if (!re15_re15_re2z_import_owns(e->type)) return;      /* nur RE1.5-Modus + Option + Zombie */
    if (e->state == 3) return;                             /* toedlicher Treffer -> DEATH-Wurzel,
                                                            * der Zerleger sitzt im HURT (s.o. c) */

    /* --- der LAZY-INIT der RE2-Zerleger-Felder ------------------------------------------------
     * Im RE2-Modus macht das re2z_init (Zustand 0, Tabelle @0x8010C830). Im RE1.5-Modus laeuft
     * dieser INIT nie, also seedet der erste Treffer. Bit 0 von part_flags[0] ist der Marker:
     * ein frisch gespawnter Aktor ist genullt (re15_actor_init/Spawn-memset), also feuert der
     * Seed nach JEDEM Respawn genau einmal. */
    if (!(e->re2z_part_flags[0] & 1u)) {
        re15_re2z_part_reset(e);                           /* Modellblock (PORT-MAPPING) */
        e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;  /* `addiu v0,zero,13`
                                                                    * @0x8010081C, `sb v0,337/338/
                                                                    * 339(s2)` @0x80100820/24/28 */
        e->re2z_hitdir1d0 = 0;                             /* sh zero,464 @0x801006BC */
        e->re2z_flags21a  = 0;                             /* sh zero,538 @0x8010087C */
        e->re2z_hits1d2   = 0;                             /* +0x1D2 (Applier stempelt gleich)   */
        e->re2z_gaitrow   = 0;
    }

    /* --- der Applier-Stempel (re15_re2_stamp_hit-Zwilling, dieselben zwei Belege) ------------ */
    e->re2z_hits1d2 = 1u;                                  /* Basis-Zone 1 @0x80047294-98 +
                                                            * 3*Bracket 0 (Bracket-Herleitung im
                                                            * Block ueber re15_re2_stamp_hit) */
    unsigned row = row_src ? re15_re2z_row_for_atktype(row_id, e->re2z_hits1d2, 1)
                           : re15_re2z_row_for_weapon (row_id, e->re2z_hits1d2, 1);
    re2z_stamp_hit_row(e, pl, row);                        /* @0x80041A0C-84 + @0x80041900-9C */

    /* --- (2) DER ZERLEGER @0x80105288-3D8 ---------------------------------------------------- */
    re2z_leg_gore(e);

    /* --- (3) der Dispatch-Zweig, der Gore erzeugt: Handler 0x80107438 (Knockdown) reisst in
     *     seiner Phase 0 die ARME ab (@0x80107514-708, `beq v1,12` @0x8010750C schliesst nur
     *     Zeile 12 aus). Die Bruecke fragt DIESELBE Tabelle @0x8010C940 wie re2z_hurt, damit
     *     beide Modi im Gleichschritt bleiben.
     *     ⚠ GEMESSEN/ABGELESEN: mit der vom Port gestempelten Spalte 1 traegt KEINE Zeile den
     *     Handler 5 (Spalte 1 der Tabelle ist {1,1,1,1,-,-,3,3,2,2,2,4,1,1,6,1,-,6}) — der
     *     Zweig ist heute also in BEIDEN Modi unerreichbar. Er steht hier trotzdem, weil er im
     *     RE2-Modus an derselben Stelle steht: sobald ein Bracket-Produzent (Teilbox 1/2 bzw.
     *     AoE-Nachbrenner, Belege in re15_damage.c) nachgezogen wird, zuenden beide Modi
     *     gemeinsam statt nur einer. */
    {
        unsigned col = e->re2z_hits1d2;
        uint8_t h = (row < 19u && col < 9u) ? re2z_hit_tbl[row][col] : (uint8_t)RE2ZH_NULL;
        if (h == (uint8_t)RE2ZH_7438 && row != 12u) re2z_knockdown_gore(e);  /* @0x8010750C-708 */
    }
}

/* ============================================================================================
 * FUN_80107888 — DIE TREFFERREAKTION DES KRIECHERS (1D-Tabelle @0x8010CBE8[+0x5]).
 * Der Einstieg in der HURT-Wurzel, byte-gelesen:
 *   80104fe0: lhu  a0,270(s1)          ; +0x10E
 *   80104fe8: andi v0,a0,0x1
 *   80104fec: beq  v0,zero,0x80105014  ; kein Kriecher -> normale Reaktion
 *   80104ff4: lbu  v0,5(s1)            ; Zeile = +0x5 (Angriffs-Id)
 *   80105008: lw   v0,-13336(at)       ; 0x8010CBE8[+0x5]
 *   8010500c: j 0x8010540c             ; -> `jalr v0` @0x80105410, danach SOFORT Epilog
 * `table 0x8010cbe8 6` (eigener Dump): [0] = NULL, [1..18] alle = 0x80107888.
 *
 * Phasen ueber +0x6 (`lbu v1,6(s0)` @0x801078B4; `beq v1,1 -> 0x80107A2C` @0x801078BC;
 * `slti 2`/`beq zero -> 0x801078F4` @0x801078C4-D0; `beq v1,2 -> 0x80107A54` @0x801078E4):
 * P0 @0x801078F4:
 *   801078f4: jal RNG
 *   801078fc/00: lui v1,0x3 / ori v1,v1,0x6        ; 0x00030006
 *   80107910/14/1c/20: `andi 0x7 / sll 8 / addu v0,v0,v1 / sw v0,332(s0)`
 *                    = +0x14C = Clip 6, **Startframe = rand & 7**, Rate 3
 *   80107924/28: +0x6 = 1
 *   80107938: jal 0x8001BF10 (Blut-Id 6000, Block +0x198 + 72 = Part 0, Offset {0,0,0}
 *             @0x8010792C-3C)
 *   80107940-5c: +0x239 == 0 -> SE 12 (`jal 0x8005bd6c` a0=12) und +0x239 = 150
 *   80107960-98: Zeile 10 && !(+0x10E & 0x80) -> 0x80106128 (Brand) und +0x21A |= 0x800
 *   8010799c-b0: Zeile 11                     -> 0x80106310 (Saeure)
 *   801079b4-c8: Zeile 14                     -> 0x80106510 (Spark-Entladung)
 *   801079cc-a28: Zeile 16 -> !(+0x10E & 0x80) && +0x23A >= 9 -> Brand + +0x21A |= 0x800,
 *                 danach IMMER +0x23A += 1
 *   ⚠ Die Zeilen 9/17 (Russ) fehlen hier — anders als in der Liege-/Death-Leiter. Deshalb wird
 *     re2z_dismember_row NICHT wiederverwendet, sondern die Leiter steht ausgeschrieben.
 *   **KEIN Sprung am Ende — P0 FAELLT DURCH nach P1 @0x80107A2C.**
 * P1 @0x80107A2C: `+0x6 += FUN_8002959C(self,…,a3=1024)`  (@0x80107A38-50)
 * P2 @0x80107A54:
 *   80107a54: sh v0,270(s0)   ; +0x10E = 1  (v0 = 1 aus `addiu v0,zero,1` @0x801078E8) — ein
 *                               HALBWORT-Store, er wischt alle anderen +0x10E-Bits weg und
 *                               laesst genau das Kriecher-Bit stehen
 *   80107a58: sw a0,4(s0)     ; +0x4  = 1  (a0 = 1 aus `addiu a0,zero,1` @0x801078B8)
 *   = Zustand 1 / Sub 0 / Kriecher -> zurueck in die Kriech-Lokomotion.
 * ========================================================================================== */
static void re2z_crawl_hurt(re15_actor_t *e)
{
    if (e->sub_state_2 == 0u) {                                    /* P0 @0x801078F4 */
        re2z_clip(e, 6, (int)(re2z_rand() & 7u), 3, 0x400, 0);     /* Wort 0x00030006 | ((rand&7)<<8)
                                                                    * @0x801078F4-920; Blend = a3 =
                                                                    * 1024 des 959c @0x80107A3C */
        e->sub_state_2 = 1;                                        /* sb 1,6 @0x80107924-28 */
        re2z_gore_fx(e, 0, 6000u);                                 /* Id 6000, Part 0 (`addiu a2,
                                                                    * a2,72` @0x8010793C), Offset
                                                                    * {0,0,0} @0x8010792C-38 */
        if (e->re2z_cd239 == 0) {                                  /* @0x80107940-4C */
            re2z_se(12);                                           /* a0 = 12 @0x8010794C-54 */
            e->re2z_cd239 = 150;                                   /* @0x80107958-5C */
        }
        {   unsigned row = e->sub_state_1;
            if (row == 10u && !(e->re2z_f10e & 0x80u)) {           /* @0x80107960-7C */
                re2z_gore_burn(e);                                 /* jal 0x80106128 @0x80107984 */
                e->re2z_flags21a |= 0x800u;                        /* ori 0x800 @0x8010798C-98 */
            }
            if (row == 11u) re2z_gore_acid(e);                     /* jal 0x80106310 @0x801079AC */
            if (row == 14u) re2z_gore_spark(e);                    /* jal 0x80106510 @0x801079C4 */
            if (row == 16u) {                                      /* @0x801079CC-D4 */
                if (!(e->re2z_f10e & 0x80u) && e->re2z_burn23a >= 9u) {  /* @0x801079DC-FC */
                    re2z_gore_burn(e);                             /* jal 0x80106128 @0x80107A04 */
                    e->re2z_flags21a |= 0x800u;                    /* ori 0x800 @0x80107A0C-18 */
                }
                e->re2z_burn23a = (uint8_t)(e->re2z_burn23a + 1u); /* @0x80107A1C-28 */
            }
        }
        /* FALLTHROUGH nach P1 @0x80107A2C — das Original hat hier KEINEN Sprung. */
        /* FALLTHRU */
    }
    if (e->sub_state_2 == 1u) {                                    /* P1 @0x80107A2C */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* +0x6 += 959c(a3=1024)
                                                                    * @0x80107A38-50 */
        return;                                                    /* j 0x80107A5C */
    }
    if (e->sub_state_2 == 2u) {                                    /* P2 @0x80107A54 */
        e->re2z_f10e = 1u;                                         /* sh 1,270 @0x80107A54 — nackt,
                                                                    * wischt alle anderen Bits */
        re15_ai_set_state_word(e, 1u);                             /* sw 1,4 @0x80107A58 */
    }
}

static void re2z_hurt(re15_actor_t *e, re15_actor_t *pl)
{
    /* Der Treffer-Stempel des Applier-Zwillings (s. Block oben) — genau EINMAL je Treffer.
     * `+0x6 == 0` in Zustand 2 tritt AUSSCHLIESSLICH in dem Tick auf, in dem ein Treffer
     * eingeschlagen ist: re15_re2_stamp_hit nullt +0x6 (Beleg `sw v0,4(s1)` @0x80047288/90),
     * und jede Reaktion verlaesst diesen Tick entweder mit +0x6 = 1 (alle sechs Handler-P0)
     * oder mit einem Zustandswort ungleich 2 (Flinch 0x501 @0x801050A4, Liegend 0x60501
     * @0x8010517C, NULL-Zelle 0x101). Eine zweite Stempelung im selben Treffer ist damit
     * ausgeschlossen — ohne Zusatz-Latch. */
    if (e->sub_state_2 == 0u) re2z_stamp_hit(e, pl);

    re2z_grab_abort(e, pl);                                        /* @0x80104F68-FDC */

    /* KRIECHER-ZWEIG @0x80104FE0-500C: `lhu 270 / andi 0x1 / beq` -> 1D-Tabelle @0x8010CBE8[+0x5]
     * -> FUN_80107888, `j 0x8010540C` = jalr + SOFORTIGER Epilog (die normale Reaktion laeuft
     * NICHT mehr). Eintrag [0] ist NULL; das Original haette dort einen Nullzeiger gerufen —
     * der Port ueberspringt Zeile 0 (der Treffer-Stempel setzt +0x5 immer >= 1, s. Zeilen-
     * Semantik oben) statt einen Ersatz zu erfinden. */
    if (e->re2z_f10e & 1u) {                                       /* @0x80104FE8-EC */
        if (e->sub_state_1 != 0u) re2z_crawl_hurt(e);              /* @0x8010CBE8[1..18] */
        return;                                                    /* j 0x8010540C -> Epilog */
    }
    /* OPEN (unveraendert): +0x21A & 0x10 -> Kriecher-Umbau FUN_80107A78 (@0x80105014-38) —
     * das ist der ANDERE Umbauweg (aufrecht -> Kriecher ueber eine Treffer-Zone), er braucht
     * seine eigene Welle; ohne Produzenten fuer +0x21A Bit 0x10 ist er im Port unerreichbar. */

    /* Flinch-Schwelle @0x8010503C-58: `+0x10E & 0x40` ? (+0x5 != 1 ? 23 : 0) : 0 */
    int thr = 0;
    if (e->re2z_f10e & 0x40u) thr = (e->sub_state_1 != 1) ? 23 : 0;

    /* Flinch-Gate: NUR in Phase 0 (`bne v0,zero,0x80105168` @0x80105064) und nur wenn die
     * Resistenz aufgebraucht ist (`lb 547` / `slt v1,v0` / `bne` @0x8010506C-78). */
    if (e->sub_state_2 == 0 && (int)e->re2z_res223 <= thr) {
        if (e->re2z_flag222 == 1 || e->sub_state_1 == 1) {         /* @0x80105080-98 */
            e->re2z_flag222 = 1;                                   /* sb 1,546 @0x801050A0 */
            re15_ai_set_state_word(e, 0x501);                      /* sw 0x501 @0x801050A4-AC */
            e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));  /* @0x801050C0-C8 */
            /* Blut in BEIDEN Zonen-Zweigen (@0x801050E8-15C): Zone 0 generisch (id 6096,
             * ofs {0,800,0}), sonst am Knochen (id 4096). Der Port-FX ist positionslos, der
             * frueher hier stehende `%3 == 0`-Filter unterdrueckte den Knochen-Zweig faelschlich. */
            re2z_blood_fx(e);
            return;                                                /* j 0x80105418 */
        }
        e->re2z_flag222 = 1;                                       /* sb 1,546 @0x80105164 */
    }

    /* @0x80105168-284: liegender Zombie (+0x21A & 2) -> zurueck in den Knockdown-Executor,
     * Phase 6 (Liege-Clip). `lui 6 / ori 0x501 / sw` = 0x60501 @0x8010517C-84. */
    if (e->re2z_flags21a & 2u) {
        re15_ai_set_state_word(e, 0x60501);                        /* @0x8010517C-84 */
        e->re2z_gaitrow = 1;                                       /* sb 1,363 @0x8010518C */
        /* ⛔ TOTER ZWEIG IM ORIGINAL — NICHT "reparieren". Die Leiter @0x80105188-284 liest
         * `lbu v1,5(s1)` @0x80105188, also NACH dem Wort-Store `sw v1,4(s1)` @0x80105184 mit
         * v1 = 0x00060501 (`lui v1,0x6` @0x8010517C / `ori v1,v1,0x501` @0x80105180). Little
         * Endian: +0x4 = 0x01, **+0x5 = 0x05**, +0x6 = 0x06. Der Vergleich `bne v1,10`
         * @0x80105194 (und die Zwillinge 11/9/14/16) sieht damit IMMER 5 — keine der fuenf
         * Zeilen kann hier zuenden. Der Port bildet genau das ab: derselbe Aufruf NACH
         * demselben Zustandswort, also liest er ebenfalls die 5. Die LEBENDEN Einstiege in
         * dieselbe Leiter sind Stagger-P0 (@0x80105DC4, `lbu v1,5(s4)` ohne vorherigen
         * Wort-Store), Haupt-P0 (@0x80105510), Haupt-P3 (@0x80105A50) und DEATH (@0x80108444).
         * GEMESSEN (test_re2_gore PIN 8): dieser Aufruf kostet 0 RNG-Wuerfe, auch mit Zeile 14 —
         * der Beweis, dass er im Port genauso stumm ist wie im Original. */
        re2z_dismember_row(e, 0);                                  /* @0x80105188-284 */
        return;
    }

    /* Der ZERLEGER @0x80105288-3D8 — kein return, er faellt in den Dispatch (@0x801053D8). */
    re2z_leg_gore(e);

    /* DISPATCH @0x801053E0-410 */
    unsigned row = e->sub_state_1;
    unsigned col = e->re2z_hits1d2;
    uint8_t h = (row < 19u && col < 9u) ? re2z_hit_tbl[row][col] : (uint8_t)RE2ZH_NULL;
    s_re2z_last_handler = h;                                       /* Port-Diagnose (Tests) */
    switch (h) {
    case RE2ZH_MAIN:    re2z_hit_main(e, pl);    return;           /* 0x80105438 */
    case RE2ZH_STAGGER: re2z_hit_stagger(e, pl); return;           /* 0x80105BC0 */
    case RE2ZH_66FC:    re2z_hit_ragdoll(e, pl, 0); return;        /* 0x801066FC — HURT-Wurzel,
                                                                    * +0x4 == 2 (death = 0) */
    case RE2ZH_703C:    re2z_hit_slide(e, pl);   return;           /* 0x8010703C */
    case RE2ZH_7EF0:    re2z_hit_light(e, pl);   return;           /* 0x80107EF0 */
    case RE2ZH_7438:    re2z_hit_knockdown(e, pl, 0); return;      /* 0x80107438 — HURT-Wurzel,
                                                                    * +0x4 == 2 (death = 0) */
    default: break;
    }
    /* Jede NULL-Zelle: das Original wuerde dort `jalr 0` ausfuehren, die Kombination entsteht
     * dort also nie. Der Port haelt hier das bisherige Verhalten (zurueck in den Gang), damit
     * kein Zombie einfriert. */
    re15_ai_set_state_word(e, 0x101);
    re2z_hit_latch_release(e);                                     /* auch die Notbremse verlaesst
                                                                    * die Reaktion -> treffbar
                                                                    * (@0x80105f9c-fac) */
}

/* Port-Diagnose: welche Tabellenzelle zuletzt dispatcht wurde (0 = NULL/keine). Nur fuer die
 * Tests — die Engine liest das nicht. */
int re15_re2z_last_hit_handler(void) { return s_re2z_last_handler; }

/* ============================================================================================
 * DEATH @0x80108250 — DIE STURZ-KETTE
 *
 * ⛔ NUTZER-REPORT 2026-08-20 (v0.3.4, beide RE2-Modi): "Die Zombies fallen manchmal ganz komisch
 * nach vorne, nachdem sie frontal mit Kugel getroffen wurden, und liegen merkwuerdig abrupt am
 * Boden auf dem Bauch mit Sterbe-Animation, ohne richtigen Uebergang."
 *
 * GEMESSEN (probe_re2_zfall 4 3, ROOM1140, echter Weg, GELADENE RE2-Bank EM010) — der Port VOR
 * diesem Fix, Brustknochen 8 relativ zum Boden:
 *   f160 st=1 s1=1 clip=0  af=0  b8dy=-2766   <- STEHT
 *   f165 st=3 s1=3 s2=1 clip=7 af=0 b8dy=-548 <- EIN Frame spaeter LIEGT er (Sprung 2218 Einheiten)
 *   f234 st=7 s1=9 clip=7 af=69 b8dy=-165     <- Clip 7 laeuft am BODEN aus
 *   f235 clip=22 (0x16) b8dy=-129             <- Leiche schnappt in die BAUCH-Pose
 * Es gab also gar keinen Sturz: der Port sprang in EINEM Frame in eine Liege-Animation.
 *
 * URSACHE, selbst disassembliert (EMOVL10_S0.BIN): die DEATH-Wurzel FUN_80108250 hat VIER Zweige,
 * der Port kannte nur EINEN — und zwar den falschen.
 *   @0x80108260-6C  `lhu 538 / andi 0x4000 / bne 0x8010829c` — NUR Kill-Zaehler (0x800D46C0++
 *                   @0x8010827C-88) und Latch `+0x21A |= 0x4000` @0x80108294-98 stehen hinter
 *                   diesem Gate. ALLES danach laeuft in JEDEM DEATH-Frame. Der Port hatte die
 *                   ganze Wurzel in den Einmal-Zweig gepackt = keine Phasen-Fortschaltung.
 *   @0x8010829C-AC  `andi 0xfdff / sh 538` -> +0x21A &= ~0x200
 *   @0x801082B0-328 Grab-Abbruch
 *   @0x80108328-AC  `lbu 6 / bne` -> nur in Phase 0: fuenf Modell-Wortsetzer |= 0x8000
 *                   (Praesentation, PORT-OPEN)
 *   @0x801083B0-E0  ZWEIG 1 `lhu 270 / andi 0x1 / beq` -> 1D-Tabelle @0x8010CECC[+0x5]
 *                   ([0] = NULL, [1..18] = FUN_80108A14) `lw v0,-12596(at)` @0x801083D8,
 *                   `jalr v0` @0x80108514. **Genau dieser Handler setzt Clip 7 (@0x80108A88).**
 *                   Er gilt fuer den WIEDERBELEBTEN KRIECHER (+0x10E Bit 0 wird ausschliesslich
 *                   in der Wiederbelebung @0x801089B0 gesetzt, `sh 0x2001,270`), NICHT fuer den
 *                   normalen Stand-Tod. Der Port spielte ihn IMMER.
 *   @0x801083E4-404 ZWEIG 2 `andi v0,v1,0x10 / beq 0x80108408` -> FUN_801099E4 (Kriecher) + raus
 *   @0x80108408-DC  ZWEIG 3 `andi v0,v1,0x2 / beq 0x801084E0` -> Liegend: Blut 8000 an Part 0
 *                   (@0x8010840C-1C), SE (rand&1)?13:11 (@0x80108424-3C), Zerleger-Leiter
 *                   (@0x80108444-D0), dann `addiu v0,zero,2311 / sw v0,4` = 0x907 @0x801084D8-DC
 *   @0x801084E0-518 ZWEIG 4 (der NORMALE Stand-Tod) 2D-Dispatch
 *                   `lbu v1,5(a0)` @0x801084E4 (Zeile = Angriffs-Id, Stride 36 via
 *                   `sll 3 / addu / sll 2` @0x801084F0-F8), `lbu v1,466(a0)` @0x801084FC
 *                   (Spalte = +0x1D2, `sll 2` @0x80108504), Basis `addiu a2,a2,-13276`
 *                   @0x801084EC = 0x8010CC24, `lw v0,0(v1)` @0x8010850C, `jalr v0` @0x80108514.
 *
 * Die Vorwaerts-/Rueckwaerts-Kette steckt im Haupt-Handler FUN_80108530 (Zeile 3 = Browning HP,
 * Spalte 1 = Basis-Zone -> Zelle @0x8010CC94): Phase 0 zieht `+0x16A = rand&1` und spielt
 * Clip 1 (rueckwaerts, ab Frame 0) bzw. Clip 2 (VORWAERTS, ab Frame 10) — das sind DIESELBEN
 * Sturz-Clips, die der Knockdown-Executor benutzt (`re2z_param_clips[side]` @0x801032C8) und die
 * den Koerper sichtbar vom Stand zu Boden bringen. Zusaetzlich stellt Phase 0 `+0x21A` Bit 0x4
 * auf die gewaehlte Richtung — und GENAU dieses Bit waehlt spaeter die Leichen-Pose
 * (`+0x21A & 0x4 ? 22 : 23` @0x8010A490-BC). Der Port hat Bit 0x4 nie im Tod gesetzt, sondern
 * den Rest eines frueheren Knockdowns stehen lassen: Clip 7 (Boden) endete in einer Pose, die
 * Leichen-Pose kam aus einem fremden Latch -> der "abrupte Bauch ohne Uebergang".
 * ========================================================================================== */

/* Die DEATH-Dispatch-Tabelle @0x8010CC24, selbst gedumpt
 * (`re2_disasm.py table 0x8010cc24 170 --bin EMOVL10_S0.BIN`), 18 Zeilen a 9 Spalten.
 * Zeile 0 liegt physisch auf der 1D-Kriecher-Tabelle @0x8010CBE8 + den Kosten-Bytes @0x8010CC33
 * und wird nie dispatcht (Angriffs-Id 0 existiert nicht) — genau wie bei der HURT-Tabelle.
 * Zeile 18 Spalte 8 faellt auf 0x8010CECC = Index 0 der 1D-Tabelle = NULL, ebenfalls wie dort. */
enum {
    RE2ZD_NULL = 0,
    RE2ZD_MAIN,      /* 0x80108530 — der normale Sturz (Clip 1/2)      -> re2z_death_main      */
    RE2ZD_7438,      /* 0x80107438 — der HURT-Knockdown-Handler mit scharfem `+0x4==3`-Zweig
                      *              (Arm ab -> CORPSE, sonst HP = 10) -> re2z_hit_knockdown   */
    RE2ZD_92C4,      /* 0x801092C4 — MAGNUM: der Kopf platzt ab, Phasentabelle @0x8010014C
                      *              (5 Phasen)                        -> re2z_death_magnum    */
    RE2ZD_66FC,      /* 0x801066FC — der HURT-Ragdoll-Handler mit scharfem `+0x4==3`-Zweig
                      *              (3/4 -> Burst, sonst HP = 10)     -> re2z_hit_ragdoll     */
    RE2ZD_8BEC,      /* 0x80108BEC — ZERREISSEN: Kopf ab + Bein(e) ab, Phasentabelle
                      *              @0x8010012C (7 Phasen)            -> re2z_death_rip       */
    RE2ZD_9610       /* 0x80109610 — WEGSCHLEUDERN + Fleischbrocken; P2 setzt +0x4 = **7**
                      *              (CORPSE, v0 aus dem DELAY-SLOT @0x80109684, `sw v0,4`
                      *              @0x801099B0)                      -> re2z_death_burst     */
};
static const uint8_t re2z_death_tbl[19][9] = {
/* 0*/ { 0,0,0, 0,0,0, 0,0,0 },   /* existiert nicht (1D-Tabelle + Kosten-Bytes @0x8010CC24) */
/* 1*/ { 1,1,0, 1,1,0, 1,1,0 },   /* @0x8010CC48 */
/* 2*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 3*/ { 1,1,0, 1,1,0, 1,1,0 },   /* @0x8010CC90 — Browning HP, die Zeile des Nutzer-Reports */
/* 4*/ { 1,1,0, 1,1,0, 1,1,0 },
/* 5*/ { 2,3,0, 2,3,0, 2,3,0 },   /* @0x8010CCD8 Magnum */
/* 6*/ { 2,3,0, 2,3,0, 2,3,0 },
/* 7*/ { 2,4,0, 2,1,0, 2,1,0 },   /* @0x8010CD20 Schrot */
/* 8*/ { 2,5,0, 2,6,0, 2,1,0 },   /* @0x8010CD44 Custom Schrot */
/* 9*/ { 2,5,5, 1,1,1, 1,1,1 },   /* @0x8010CD68 GL Explosiv */
/*10*/ { 1,1,1, 1,1,1, 1,1,1 },   /* @0x8010CD8C */
/*11*/ { 1,1,1, 1,1,1, 1,1,1 },   /* @0x8010CDB0 */
/*12*/ { 1,6,0, 1,6,0, 1,6,0 },   /* @0x8010CDD4 Bowgun (im Port nie gestempelt) */
/*13*/ { 1,1,0, 1,1,0, 1,1,0 },
/*14*/ { 1,1,0, 1,1,0, 1,1,0 },
/*15*/ { 1,1,0, 1,1,0, 1,1,0 },
/*16*/ { 1,1,0, 1,1,0, 1,1,0 },
/*17*/ { 2,5,0, 2,6,0, 2,1,0 },   /* @0x8010CE88 Rakete */
/*18*/ { 1,1,0, 1,1,0, 1,1,0 }    /* @0x8010CEAC, Spalte 8 = 0x8010CECC[0] = NULL */
};
static int s_re2z_last_death_handler = 0;   /* Port-Diagnose (Tests) — 0 = keine Zelle gelaufen */
int re15_re2z_last_death_handler(void) { return s_re2z_last_death_handler; }
/* Port-Diagnose: EINE Zelle der Tabelle (Tests pinnen sie gegen den Dump). -1 = ausserhalb. */
int re15_re2z_death_cell(unsigned row, unsigned col)
{
    if (row >= 19u || col >= 9u) return -1;
    return (int)re2z_death_tbl[row][col];
}

/* ============================================================================================
 * DIE POSE-BANK DES RE2-ZOMBIES — die VOLLSTAENDIGE Karte (selbst disassembliert 2026-08-21)
 * --------------------------------------------------------------------------------------------
 * ⛔ NUTZER-REPORT (3. Anlauf, nach v0.3.7): "Die Zombies fallen teilweise IMMER NOCH komisch
 * hin, wenn sie einmal mit Waffe getroffen werden, oder haben manchmal eine kurze Hinfall-
 * Animation, stehen dann aber sofort wieder."
 *
 * GEMESSEN (probe_re2z_fallstand, 128 Seeds x 4 Schuss-Fahrplaene, ROOM1140, echter game_step,
 * GELADENE RE2-Bank EM010; Pose ueber DIESELBE Bankwahl wie der Renderer). EIN Pistolenschuss,
 * Seed 19, Brustknochen 8 relativ zum Aktor-Boden:
 *   f59  st=2 s1=3 s2=2 clip=2 af=15/60 ACT   b8dy=-2660   <- Reaktion laeuft
 *   f68  st=2 s1=3 s2=2 clip=2 af=24/60 ACT   b8dy=-1882   <- Brust sinkt
 *   f70  st=2 s1=3 s2=3 clip=2 af=26/60 ACT   b8dy=-1521   <- fast am Boden
 *   f71  st=1 s1=2 s2=0 clip=2 af=27/54 LOCO  b8dy=-2728   <- STEHT WIEDER, in EINEM Frame
 * Der Zombie faellt also gar nicht — der Port spielt DENSELBEN Clip-Index aus der FALSCHEN BANK.
 * Clip 2 ist in der LOCO-Bank (Paar 1) der 54-Frame-GANG und in der Aktions-Bank (Paar 2) der
 * 60-Frame-STURZ (re2z_param_clips[1] = 0x02). Sobald die Reaktion endet und der Zombie wieder
 * in Zustand 1 geht, kippt die Bank zurueck und die Pose SCHNAPPT hoch.
 *
 * ---- WIE DAS ORIGINAL DIE BANK WAEHLT (kein per-Frame-Regelwerk, sondern ein Aufruf-Argument) --
 * Der Advance FUN_8002959C(a0=entity, a1=EMR, a2=EDD, a3=blend) loest den Frame-Eintrag auf und
 * LEGT IHN IN DIE ENTITY, dann posiert er das Skelett aus GENAU DIESEM EMR:
 *   800295a8: lbu  v0,332(a0)      ; +0x14C = Clip-Byte
 *   800295b4: addu a2,a1,v0<<2     ; a1 = der uebergebene EDD  -> Clip-Record
 *   800295d0: lbu  v0,333(a0)      ; +0x14D = Frame-Byte
 *   800295f4: addu a2,v1,v0<<2     ; -> FRAME-EINTRAG
 *   800295f8: sw   a2,376(a0)      ; **entity+0x178 = der posierte Frame-Eintrag**
 *   800295fc: jal  0x80029614      ; posiert mit a0 = der uebergebene EMR
 * Die Bank ist damit exakt das (EMR,EDD)-Paar der jeweiligen Aufrufstelle. Der Wurzel-Dispatcher
 * reicht PAAR 2 durch (`lw a1,0x180` @0x801004C8 / @0x80100548 = +0x180/+0x184 = Port-Aktionsbank);
 * einzelne Handler laden PAAR 1 EXPLIZIT (`lw a1,264` = +0x108 / `lw a2,380` = +0x17C = Port-
 * Loco-Bank). Eigener Scan ueber ALLE `jal 0x8002959c` in EMOVL10_S0.BIN — die Paar-1-Stellen:
 *   EXEC[0] STAND  P2/P3 @0x80101610      (P0/P1 @0x801014A0 fahren Paar 2)
 *   EXEC[1] WALK         @0x80101CD0
 *   EXEC[2] BUMP         @0x801023F0
 *   EXEC[10]             @0x80104240      (im Port inert)
 *   hit_MAIN 0x80105438  P1 @0x80105790, P2 @0x801058C0     <<< DIE LUECKE
 *   hit_SLIDE 0x8010703C @0x801071E4 / @0x8010729C / @0x80107364
 *   death_MAGNUM P4      @0x801095D0
 *   death_RIP    P6      @0x80109288
 *   Zustand-8-Wurzel 0x80109CFC @0x80109E30
 * ALLE uebrigen 40 Advance-Stellen nehmen das Dispatcher-Paar (Paar 2). Gegenprobe ueber den
 * Wurzelbewegungs-Zwilling FUN_80015E7C: dieselbe Aufteilung, hit_MAIN laedt Paar 1 auch dort
 * (@0x80105580-84 P0, @0x80105754-58 P1, @0x801058A4-A8 P2).
 *
 * DIESE FUNKTION IST DIE KARTE. Sie ist die byte-belegte Erweiterung der Port-Regel
 * re15_actor_uses_loco_bank() (enemy_ai_common.c), die heute nur WALK/BUMP kennt.
 * ========================================================================================== */
int re15_re2z_poses_loco_bank(const re15_actor_t *a)
{
    if (!a) return 0;
    if (!re15_re2z_owns_type(a->type)) return 0;
    /* KRIECHER: eigene Decide-/Exec-Tabellen @0x8010C90C/@0x8010C918, deren Handler durchweg
     * das Dispatcher-Paar fahren (crawl_HURT-Advance @0x80107A38 = REG). Nie Paar 1. */
    if (a->re2z_f10e & 1u) return 0;

    switch (a->state) {
    case 1:
        switch (a->sub_state_1) {
        case 0:  return (a->sub_state_2 >= 2) ? 1 : 0;   /* STAND: P0/P1 @0x801014A0 = Paar 2,
                                                          * P2 @0x801015A4 faellt in P3 @0x801015DC
                                                          * mit dem Paar-1-Advance @0x80101610 */
        case 1:  return 1;                               /* WALK  @0x80101CD0 */
        case 2:  return 1;                               /* BUMP  @0x801023F0 */
        default: return 0;
        }
    case 2: {
        /* Welcher Reaktions-Handler laeuft? Exakt die Kaskade der HURT-Wurzel (re2z_hurt):
         * der Liege-Zweig `+0x21A & 2` (@0x80105168) verlaesst Zustand 2 im selben Tick, und
         * hit_MAIN gibt bei `+0x222 == 1` sofort an hit_STAGGER ab (@0x80105458-70) — und der
         * fahrt das Dispatcher-Paar (Advance @0x80105F3C = REG). */
        if (a->re2z_flags21a & 2u) return 0;
        {   unsigned row = a->sub_state_1, col = a->re2z_hits1d2;
            uint8_t h = (row < 19u && col < 9u) ? re2z_hit_tbl[row][col] : (uint8_t)RE2ZH_NULL;
            if (h == RE2ZH_MAIN && a->re2z_flag222 != 1u) return 1;   /* @0x80105790/@0x801058C0 */
            if (h == RE2ZH_703C) return 1;                            /* @0x801071E4/9C/@0x80107364 */
        }
        return 0;
    }
    case 3: {
        unsigned row = a->sub_state_1, col = a->re2z_hits1d2;
        uint8_t h = (row < 19u && col < 9u) ? re2z_death_tbl[row][col] : (uint8_t)RE2ZD_NULL;
        if (h == RE2ZD_92C4 && a->sub_state_2 == 4u) return 1;   /* MAGNUM P4 @0x801095D0 */
        if (h == RE2ZD_8BEC && a->sub_state_2 == 6u) return 1;   /* RIP    P6 @0x80109288 */
        return 0;
    }
    default: return 0;
    }
}

/* ---- FUN_80108A14 — der Zweig `+0x10E & 1` (der wiederbelebte Kriecher stirbt ein ZWEITES Mal).
 * Drei Phasen ueber +0x6 (`lbu v1,6(s0)` @0x80108A40, `beq v1,1 -> 0x80108BA4` @0x80108A48,
 * `slti v0,v1,2` @0x80108A50, `beq v1,zero -> 0x80108A80` @0x80108A5C, `beq v1,2 -> 0x80108BCC`
 * @0x80108A70). Phase 0 FAELLT DURCH in den Advancer (kein Sprung ueber @0x80108BA4). */
static void re2z_death_crawler(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:
        re2z_clip(e, 7, 0, 0xF, 0x100, 0);                         /* `lui v0,0xf / ori v0,v0,0x7`
                                                                    * @0x80108A60/80 -> Wort
                                                                    * 0x000F0007, Blend = a3 = 256
                                                                    * des 959c @0x80108BB4 */
        e->sub_state_2 = 1;                                        /* sb a1(=1),6 @0x80108A9C */
        e->re2z_flags21a &= (uint16_t)~0x4u;                       /* andi 0xfffb @0x80108AA0-A4 —
                                                                    * Leichen-Pose 23 (Ruecken) */
        e->re2z_self1d3 |= 0x80u;                                  /* ori 0x80 @0x80108AB4-B8 */
        re2z_gore_fx(e, 0, 8096u);                                 /* Part 0 (`addiu a2,a2,72`
                                                                    * @0x80108AD4), ofs y+300
                                                                    * @0x80108ABC-C4 */
        re2z_se((re2z_rand() & 1u) ? 13 : 11);                     /* @0x80108AD8-F4 */
        /* jal 0x80018FB0 @0x80108AF8 (Praesentation, OPEN) */
        if (e->sub_state_1 == 10u && !(e->re2z_f10e & 0x80u))
            re2z_gore_burn(e);                                     /* @0x80108B00-28 */
        if (e->sub_state_1 == 11u && !(e->re2z_flags21a & 0x1000u))
            re2z_gore_acid(e);                                     /* @0x80108B34-54 */
        if ((e->sub_state_1 == 9u || e->sub_state_1 == 17u) && !(e->re2z_f10e & 0x80u))
            re2z_gore_soot(e);                                     /* @0x80108B58-88 */
        if (e->sub_state_1 == 14u) re2z_gore_spark(e);             /* @0x80108B8C-A0 */
        /* FALLTHROUGH — @0x80108BA4 ist nur das Sprungziel von Phase 1 */
        /* fall through */
    case 1:
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* `+0x6 += 959c(...,256)`
                                                                    * @0x80108BB0-C8 */
        break;
    case 2:
        re15_ai_set_state_word(e, 7u);                             /* `sw v0,4(s0)` @0x80108BCC mit
                                                                    * v0 = 7 (`addiu v0,zero,7`
                                                                    * @0x80108A74) -> CORPSE Sub 0 */
        break;
    default: break;                                                /* j 0x80108BD0 */
    }
}

/* ---- die Zerleger-Leiter des DEATH-HANDLERS @0x801086E4-808 -------------------------------
 * NICHT identisch mit re2z_dismember_row(): sie traegt (a) den IMMER laufenden Delay-Slot
 * `sb zero,363(s1)` @0x8010871C, (b) den Zusatz-Spray der Saeure-Zeile @0x80108728-8C und
 * (c) eine EIGENE Zeile-16-Fassung ohne Brand-Zaehler @0x801087DC-808. Reihenfolge = Original. */
static void re2z_death_dismember(re15_actor_t *e)
{
    unsigned row = e->sub_state_1;
    if (row == 10u && !(e->re2z_f10e & 0x80u))
        re2z_gore_burn(e);                                         /* @0x801086E4-70C */
    e->re2z_gaitrow = 0;                                           /* +0x16B: `sb zero,363` im
                                                                    * DELAY-SLOT von `bne v1,v0`
                                                                    * @0x80108718/1C = IMMER */
    if (row == 11u) {
        re2z_gore_acid(e);                                         /* @0x80108720-24 */
        if (re2z_rand() & 1u) {                                    /* @0x80108728-34 */
            re2z_gore_fx(e, 8, 0x040F0FA0u);                       /* Anker `addiu a2,a2,1448`
                                                                    * @0x80108784 = Part 8, Id
                                                                    * 0x040F0FA0 @0x80108740-44 */
            e->re2z_gaitrow = 1;                                   /* sb 1,363 @0x80108788-8C */
        }
    }
    if ((row == 9u || row == 17u) && !(e->re2z_f10e & 0x80u))
        re2z_gore_soot(e);                                         /* @0x80108790-C0 */
    if (row == 14u) re2z_gore_spark(e);                            /* @0x801087CC-D8 */
    if (row == 16u && !(e->re2z_f10e & 0x80u))
        re2z_gore_burn(e);                                         /* @0x801087DC-808 — OHNE
                                                                    * +0x23A-Zaehler und ohne
                                                                    * `+0x21A |= 0x800` */
}

/* ---- FUN_80108530 — DER NORMALE STURZ (Zellen RE2ZD_MAIN) ---------------------------------
 * Drei Phasen ueber +0x6 (`lbu v1,6(s1)` @0x80108564, `beq v1,1 -> 0x80108810` @0x8010856C,
 * `slti v0,v1,2` @0x80108574, `beq v1,zero -> 0x801085A4` @0x80108580, `beq v1,2 -> 0x80108918`
 * @0x80108594). Phase 0 FAELLT DURCH nach Phase 1 (@0x8010880C ist die letzte Zeile von Phase 0,
 * @0x80108810 die erste von Phase 1 — kein Sprung dazwischen). */
static void re2z_death_main(re15_actor_t *e)
{
    static const uint8_t clipsel[2] = { 1u, 2u };                  /* `sb 1,16(sp)` @0x8010855C /
                                                                    * `sb 2,17(sp)` @0x80108560,
                                                                    * gelesen `lbu a2,16(v0)`
                                                                    * @0x801085F0 */
    switch (e->sub_state_2) {
    case 0: {
        /* ---- die RICHTUNGSWAHL: ein RNG-Bit, erzwungen 0 wenn +0x21A Bit 0x2000 steht ---- */
        uint32_t r = re2z_rand();                                  /* jal 0x80015FE8 @0x801085A4 */
        e->re2z_dir16a = (uint8_t)(r & 1u);                        /* `andi v0,v0,0x1` @0x801085B0 /
                                                                    * `sb v0,362(s1)` @0x801085BC
                                                                    * (DELAY-SLOT = immer) */
        if (e->re2z_flags21a & 0x2000u) e->re2z_dir16a = 0;        /* `andi v1,v1,0x2000` @0x801085B4
                                                                    * / `sb zero,362` @0x801085C0 */
        e->re2z_flags21a &= (uint16_t)~0x4u;                       /* `andi v0,v0,0xfffb` @0x801085CC
                                                                    * / `sh` @0x801085D4 */
        if (e->re2z_dir16a != 0)
            e->re2z_flags21a |= 0x4u;                              /* `ori v0,v0,0x4` @0x801085D8 /
                                                                    * `sh v0,538` @0x801085DC —
                                                                    * waehlt spaeter Leichen-Pose 22 */
        e->re2z_self1d3 |= 0x80u;                                  /* `ori v1,v1,0x80` @0x80108604 /
                                                                    * `sb v1,467` @0x80108608 */
        e->sub_state_2 = 1;                                        /* `sb s0(=1),6(s1)` @0x80108610 */
        /* `+0x1C0 |= 1` @0x80108614-18 — Modell-/Kollisions-Byte, im Port nicht gefuehrt (OPEN) */

        /* ---- der Clip: Wort = ((+0x16A*5) << 9) + 0xF0000 + clipsel[+0x16A] ----
         *   8010861c: sll v0,a1,2 / 80108620: addu v0,v0,a1   -> dir*5
         *   80108624: sll v0,v0,9                             -> dir*2560 (Frame-Byte = dir*10)
         *   80108628: addu a2,a2,v1  (v1 = `lui v1,0xf` @0x8010860C)
         *   8010862c: addu v0,v0,a2 / 80108630: sw v0,332(s1)
         * dir 0 -> 0x000F0001 = Clip 1 ab Frame 0   (Sturz nach HINTEN)
         * dir 1 -> 0x000F0A02 = Clip 2 ab Frame 10  (Sturz nach VORNE)
         * Blend = a3 = 256 des 959c-Aufrufs @0x801088E8. */
        {   int dir = (int)(e->re2z_dir16a & 1u);
            re2z_clip(e, (int)clipsel[dir], dir * 10, 0xF, 0x100, 0);
        }

        /* ---- Blut: Zone-0 generisch an Part 1, sonst am Part 0 (`+0x1D2 % 3`, magisches
         * multu 0xAAAAAAAB @0x801085F4-4C / @0x80108634-4C) ---- */
        if (((unsigned)e->re2z_hits1d2 % 3u) != 0u)
            re2z_gore_fx(e, 0, 8096u);                             /* Id 8096 @0x8010866C, Anker
                                                                    * `addiu a2,s2,72` @0x80108690,
                                                                    * ofs y+300 @0x80108670 */
        else
            re2z_gore_fx(e, 1, 6096u);                             /* Id 6096 @0x80108694, Anker
                                                                    * `addiu a2,a2,244` @0x801086B0,
                                                                    * ofs {0,800,0} @0x801086A0-A8 */
        re2z_se((re2z_rand() & 1u) ? 13 : 11);                     /* EIN Wurf @0x801086BC-D8 */
        /* jal 0x80018FB0(self) @0x801086DC (Praesentation, OPEN) */
        re2z_death_dismember(e);                                   /* @0x801086E4-808 */
        /* `sh 11,324(s1)` @0x8010880C ist ein DEAD STORE — derselbe Fall wie die drei schon
         * dokumentierten `sh 11,324`-Seeds (@0x80104824/@0x80104DC8/@0x80103614): der naechste
         * 0x80015E7C ueberschreibt +0x144 mit dem Clip-Root-Delta, bevor 0x800152C8 ihn liest. */
        /* FALLTHROUGH nach Phase 1 (@0x8010880C -> @0x80108810 ohne Sprung) */
    }
    /* fall through */
    case 1:
        /* Tropfblut-Fenster @0x80108810-D4: nur wenn `+0x1D2 % 3 != 0`, und nur solange der
         * Frame-Zaehler +0x14D in [dir*10+4, dir*10+16] liegt und UNGERADE ist
         * (`addiu v0,v1,4 / slt` @0x80108858-60, `addiu v0,v1,16 / slt` @0x80108864-6C,
         * `andi v0,a1,0x1` @0x80108870). Zwei RNG-Wuerfe je Tropfen (@0x801088B0/BC). */
        if (((unsigned)e->re2z_hits1d2 % 3u) != 0u) {
            int d  = (int)(e->re2z_dir16a & 1u) * 10;
            int fr = re2z_frame_slot(e);
            if (fr >= d + 4 && fr <= d + 16 && (fr & 1)) {
                uint32_t r1 = re2z_rand();                         /* @0x801088B0 */
                uint32_t r2 = re2z_rand();                         /* @0x801088BC — Winkel
                                                                    * `sll a1,v0,4` @0x801088C8;
                                                                    * der Port-Stand-in nimmt
                                                                    * +0x76 als Winkel */
                (void)r2;
                re2z_gore_fx(e, 0, (uint32_t)((r1 << 3) + 4048u)); /* Id `sll s0,v0,3` @0x801088B8
                                                                    * `addiu s0,s0,4048` @0x801088C0,
                                                                    * Anker `addiu a2,s2,72`
                                                                    * @0x801088D4, ofs y+500
                                                                    * @0x80108898 */
            }
        }
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re2z_clip_done(e));
                                                                   /* `+0x6 += 959c(...,256)`
                                                                    * @0x801088E4-FC */
        /* `if (+0x14E == 0) FUN_80016200(self,0,1)` @0x801088F0/F8 + @0x80108900-0C —
         * Matrix-/Positions-Nachzieher (Praesentation), im Port OPEN. */
        break;
    case 2:
        /* @0x80108918: `sw v0,4(s1)` im DELAY-SLOT des rand()-Aufrufs, v0 = 7 aus dem
         * `addiu v0,zero,7` @0x80108598 -> +0x4 = 7 (CORPSE, Sub 0). PLAIN 7, NICHT 0x907. */
        re15_ai_set_state_word(e, 7u);
        (void)re2z_rand();                                         /* der Wurf @0x80108918 gehoert
                                                                    * zur Wurfzahl und wird gezogen */
        /* Die WIEDERBELEBUNG als Kriecher haengt hinter fuenf Gates und ist im Port NICHT
         * portiert (es gibt keinen Kriecher-Executor — dieselbe dokumentierte Luecke wie an der
         * HURT-Wurzel @0x80104FE0-500C / @0x80105014-38). Die Gates, damit sie beim Nachziehen
         * vollstaendig sind:
         *   `andi v0,v0,0x3 / bne`  @0x80108920-24  3/4 -> raus
         *   `andi v0,v0,0x4 / bne`  @0x8010892C-38  +0x21A Bit 0x4 (VORWAERTS-Toter) -> raus
         *   `lh v0,268(s1) / bne`   @0x80108940-48  +0x10C != 0 -> raus (+0x10C hat im Port kein
         *                                           Feld, s. Ragdoll-P2 @0x80107784)
         *   `lb v0,363(s1) / bne`   @0x80108950-58  +0x16B != 0 -> raus
         *   `0x800CFBD8 & 0x10000000` @0x80108960-70 -> raus
         * Rumpf @0x8010895C-B0: HP = 1 (`sh 1,342` @0x80108980), Hitbox-Felder @0x80108984-A4,
         * `sh 0x2001,270` @0x801089B0 (= +0x10E Bit 0, der Eingang der 1D-Tabelle oben), dann
         * +0x4 = 0x201 bzw. das Vorgabewort (@0x801089C8-DC) und `+0x1D3 &= 0x7F` @0x801089E0-EC.
         * OPEN, mit Adressen. Bis dahin endet die Kette hier in CORPSE. */
        break;
    default: break;                                                /* j 0x801089F0 */
    }
}

static void re2z_death(re15_actor_t *e, re15_actor_t *pl)
{
    if (!(e->re2z_flags21a & 0x4000u)) {                           /* @0x80108260-6C */
        e->re2z_flags21a |= 0x4000u;                               /* kill latch @0x80108294-98 */
        /* the global kill counter 0x800D46C0++ (@0x8010827C-88) has no port equivalent */
    }
    e->re2z_flags21a &= (uint16_t)~0x200u;                         /* andi 0xfdff @0x801082A8-AC */
    re2z_grab_abort(e, pl);                                        /* @0x801082B0-328, claim clear
                                                                    * @0x801082F4 */
    /* `lbu v0,6(s0) / bne -> 0x801083B0` @0x80108328-34: nur in Phase 0 werden fuenf
     * Modell-Flagwoerter |= 0x8000 gesetzt (@0x80108338-AC) — Praesentation, PORT-OPEN. */

    if (e->re2z_f10e & 1u) {                                       /* ZWEIG 1 @0x801083B0-BC */
        s_re2z_last_death_handler = -1;                            /* 1D-Tabelle @0x8010CECC */
        re2z_death_crawler(e);
        return;
    }
    if (e->re2z_flags21a & 0x10u) {                                /* ZWEIG 2 @0x801083EC-F0 */
        /* FUN_801099E4 (Kriecher-Tod) hat im Port keinen Handler — dieselbe dokumentierte
         * Luecke wie an der HURT-Wurzel. Stand-in: die Clip-7-Kette, damit die Leiche entsteht. */
        s_re2z_last_death_handler = -2;
        re2z_death_crawler(e);
        return;
    }
    if (e->re2z_flags21a & 0x2u) {                                 /* ZWEIG 3 @0x80108408 */
        s_re2z_last_death_handler = -3;
        re2z_gore_fx(e, 0, 8000u);                                 /* Part 0 (`addiu a2,a2,72`
                                                                    * @0x80108420) @0x8010841C */
        re2z_se((re2z_rand() & 1u) ? 13 : 11);                     /* @0x80108424-3C */
        re2z_dismember_row(e, 1);                                  /* @0x80108444-D0 */
        re15_ai_set_state_word(e, 0x907);                          /* `addiu v0,zero,2311 / sw`
                                                                    * @0x801084D8-DC */
        return;
    }

    {   /* ZWEIG 4 — der 2D-Dispatch @0x801084E0-518 */
        unsigned row = e->sub_state_1;
        unsigned col = e->re2z_hits1d2;
        uint8_t h = (row < 19u && col < 9u) ? re2z_death_tbl[row][col] : (uint8_t)RE2ZD_NULL;
        s_re2z_last_death_handler = (int)h;
        switch (h) {
        case RE2ZD_MAIN: re2z_death_main(e);            return;
        case RE2ZD_7438: re2z_hit_knockdown(e, pl, 1);  return;   /* 0x80107438, Spalte 0 */
        case RE2ZD_92C4: re2z_death_magnum(e, pl);      return;   /* 0x801092C4, Zeilen 5/6 */
        case RE2ZD_66FC: re2z_hit_ragdoll(e, pl, 1);    return;   /* 0x801066FC, Zeile 7 */
        case RE2ZD_8BEC: re2z_death_rip(e, pl);         return;   /* 0x80108BEC, Zeilen 8/9/17 */
        case RE2ZD_9610: re2z_death_burst(e, pl);       return;   /* 0x80109610, Zeile 12 */
        default:
            /* Die NULL-Zellen: im Original waere das `jalr 0`, also unerreichbar
             * (PORT-SICHERUNG, kein eingefrorener Gegner). */
            re2z_death_crawler(e);
            return;
        }
    }
}

/* ============================================================================================
 * CORPSE @0x8010A440 — DIE LEICHEN-MASCHINE (12 Subs, Tabelle @0x8010019C)
 * --------------------------------------------------------------------------------------------
 * NUTZER-REPORT 2026-08-21 (RE2-KI-Modus), zwei Teile:
 *   (A) "Der am Boden getoetete Zombie laeuft sofort aus, nicht langsam wie normal."
 *   (B) "Ausserdem gibt es keine finale Todesanimation."
 *
 * GEMESSEN vor dem Fix (probe_re2z_corpse, ROOM1140, echter Weg game_step+Pad, RE2-Bank,
 * 64 Seeds): 64/64 Leichen mit POOL_HX first=min=max=last=1320 (Wachstums-Spanne 0), in
 * state 7 nur die +0x5-Werte {0,1}, clip 22 auf anim_frame 0 / Keyframe 722 fuer 900+ Frames.
 * RE1.5-Kontrolle im selben Lauf: POOL_HX 608 -> 1320 ueber ~90 Frames, +0x5 {0,1,2,3,4}.
 *
 * URSACHE: der Port fuhr nur den Sub-0-Init und hielt danach. Die elf uebrigen Zellen waren als
 * "presentation-only" abgetan — sie sind es NICHT: Sub 1/3/8 sind der LACHEN-GROWER, Sub 1/5/10
 * der Clip-Advance und Sub 2/4/6 die Zuck-Taktung. Zusaetzlich laesst der globale Port-Advancer
 * Leichen aus (`if (a->state == RE15_AI_STATE_CORPSE) continue;`, player_common.c) — voellig
 * richtig, denn im Original advanct die Leichen-Maschine SELBST (`jal 0x8002959c` @0x8010a7ec).
 * Ohne den Advance in dieser Funktion steht der Leichen-Clip also auf Frame 0.
 *
 * ROUTER @0x8010a440-8c:
 *   8010a460: lbu v1,5(s0)      ; +0x5
 *   8010a464: lw  s1,364(s0)    ; s1 = [+0x16C] = der BODEN-SCHATTEN-/PRIM-RECORD
 *   8010a468: sltiu v0,v1,0xc   ; 12 Zellen
 *   8010a46c: beq  v0,zero,0x8010a80c   ; ausserhalb -> nur der gemeinsame Schwanz
 *   8010a480: lw   v0,412(at)   ; Tabelle @0x8010019c
 * Tabelle @0x8010019c (selbst dekodiert): [0]=0x8010a490 [1]=0x8010a5d8 [2]=0x8010a60c
 *   [3]=0x8010a64c [4]=0x8010a69c [5]=0x8010a7e0 [6]=0x8010a6bc [7]=0x8010a80c
 *   [8]=0x8010a768 [9]=0x8010a7b4 [10]=0x8010a7e0 [11]=0x8010a808
 *
 * ⚠ DIE BASIS DER LACHE IST OFFEN — und wird hier NICHT erfunden. Das Original waechst auf dem
 * Wert weiter, den [+0x16C]+4/+6 beim Spawn schon haben; einen Overlay-Schreiber dafuer gibt es
 * NICHT (voller Scan von EMOVL10_S0.BIN nach `lw rX,364(rY)`: genau ZWEI Treffer — @0x80109ea8
 * ist eine Sprungtabelle, @0x8010a464 diese Wurzel). Die Basis setzt der RE2-EXE-Spawnpfad, den
 * der Port nicht hat. Der Port legt den Grower deshalb auf SEINE bereits zitierte Zombie-
 * Schattenbasis (FUN_8001af5c-Argumente 0x258/0x2bc = 600/700, platform/pc/main.c). Byte-true
 * sind RATE (+5/Tick), BUDGET (120 Ticks ueber +0x16A) und FARBE (0x00BFBF10).
 *
 * OFFEN (mit Adressen, bewusst nicht gebaut):
 *   - `+0x1C0 |= 1` @0x8010a49c-b0 — dasselbe Modell-/Kollisionsbyte ohne Port-Feld wie an den
 *     schon dokumentierten Stellen (@0x80108614, @0x801096a0, @0x8010945c).
 *   - `jal 0x80018fb0` @0x8010a520 und die fuenf `[+0x198]+{516,688,1032,1204,1376} |= 0x8000`
 *     (@0x8010a528-9c, nochmal @0x8010a6e8-64) — Modell-Sichtbarkeits-/Prim-Flags.
 *   - Der Farb-Ausblender im Schwanz @0x8010a818-68 (Gate `+0x10E & 0x80`, alle 4 Frames
 *     15x `[+0x198]+112+i*172 += 0xfffefeff`) — Part-Farbwoerter, kein Port-Kanal.
 *   - Das ABSACKEN @0x8010a86c-8f0 (Gate `+0x10C != 0`; `+0x3C -= 5` bis `+0x10C+300`, sonst
 *     `+= 2`, Zaehler +0x238) — +0x10C hat fuer den Zombie kein Port-Feld (dieselbe Luecke wie
 *     an Ragdoll-P2 @0x80107784), und ohne +0x10C ist der ganze Block im Original tot.
 * ========================================================================================== */

/* Die ADVANCE-Zelle @0x8010a7e0 (Sub 5 UND Sub 10):
 *   8010a7ec: jal 0x8002959c ; 8010a7f0: addiu a3,zero,512   (Blend 0x200)
 *   8010a7f4-804: `lbu +0x5; addu v1,v1,v0; sb v1,5(s0)`     (+0x5 += Rueckgabe)
 * FUN_8002959C -> FUN_80029614; dessen Frame-/Fade-Schwanz (RE2-PSX.EXE, selbst disassembliert):
 *   800299c0: lbu v0,334(s2) ; 800299c8: addiu v0,v0,-1 ; 800299cc: sb v0,334(s2)  (+0x14E--)
 *   80029b28: lbu v0,333(s2) ; 80029b30: addiu v0,v0,1  ; 80029b34: sb v0,333(s2)  (+0x14D++)
 *   80029b3c: sltu v0,v0,s3  ; 80029b40: bne -> return 0
 *   80029b48: sb zero,333(s2); 80029b4c: addiu v0,zero,1                (WRAP + return 1)
 * KEIN "hold last frame" — genau die Lehre aus 89053003. */
static int re2z_corpse_advance(re15_actor_t *e)
{
    e->anim_blend_rate = 0x200;                                    /* a3 = 512 @0x8010a7f0 */
    if (e->anim_frac > 0) e->anim_frac--;                          /* +0x14E-- @0x800299C0-CC */
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return 1;                                         /* keine Bank -> nicht haengen */
    uint16_t nf = (uint16_t)(e->anim_frame + 1);                   /* +0x14D++ @0x80029B30 */
    if ((int)nf >= fc) { e->anim_frame = 0; return 1; }            /* @0x80029B48-4C */
    e->anim_frame = nf;
    return 0;                                                      /* @0x80029B44 */
}

/* Der GROWER, wortgleich in Sub 1 (@0x8010a5f0-608), Sub 3 (@0x8010a664-678) und Sub 8
 * (@0x8010a780-94):  `lhu +4; lhu +6; addiu +5; addiu +5; sh; sh` auf dem Schatten-Record
 * [+0x16C]. Im Port sind das die AI-Kanaele crow_shadow_w/h (dieselben Felder, die die
 * RE2-/RE1.5-Kraehe fuer ihre Lache benutzt — re15_actor.h). */
static void re2z_corpse_pool_grow(re15_actor_t *e)
{
    e->crow_shadow_w = (uint16_t)(e->crow_shadow_w + 5);
    e->crow_shadow_h = (uint16_t)(e->crow_shadow_h + 5);
}

static void re2z_corpse(re15_actor_t *e)
{
    unsigned sub = e->sub_state_1;
    if (sub >= 12u) sub = 7u;                                      /* `sltiu v0,v1,0xc` @0x8010a468
                                                                    * -> 0x8010a80c = Zelle 7 */
    switch (sub) {
    case 0:                                                        /* @0x8010a490 INIT */
        re2z_clip(e, (e->re2z_flags21a & 0x4u) ? 0x16 : 0x17,      /* 0x00070017, bzw. 0x00070016
                                                                    * wenn +0x21A&4 @0x8010a490-bc */
                  0, 7, 0x200, 0);
        /* `+0x1C0 |= 1` @0x8010a49c-b0 — OPEN (s. Blockkopf) */
        /* Leichen-Tint: [+0x16C]+28 / +68 = (alt & 0xff000000) | 0x00BFBF10 @0x8010a4c0-508.
         * Port-Kanal: crow_pool (1 = Lachen-Faerbung statt Grau-Schatten). Die Basis-Halb-
         * ausdehnung setzt der Port hier (Blockkopf: im Original EXE-Spawn-Vorgabe). */
        e->crow_shadow_w = 600;                                    /* Port-Basis (af5c 0x258) */
        e->crow_shadow_h = 700;                                    /* Port-Basis (af5c 0x2bc) */
        e->crow_pool     = 1;
        e->hp            = -1;                                     /* sh -1,342 @0x8010a4d4 */
        e->sub_state_1   = 1;                                      /* sb 1,5 @0x8010a4e0 */
        e->re2z_dir16a   = 120;                                    /* +0x16A = 120 @0x8010a508-10 */
        e->re2z_t158     = 40;                                     /* +0x158 = 40 @0x8010a514-18 */
        e->speed_h       = 0;                                      /* sh zero,324 @0x8010a524 */
        /* `+0x0 |= 2` @0x8010a4e4-e8, `sb zero,568` (+0x238) @0x8010a51c, `jal 0x80018fb0`
         * @0x8010a520 und die fuenf Prim-Flagwoerter @0x8010a528-9c: OPEN (s. Blockkopf) */
        if ((re2z_rand() % 3u) != 0u) {                            /* jal @0x8010a59c; magisches
                                                                    * 0x55555556-Div @0x8010a5a4-c4;
                                                                    * `beq v0,v1` @0x8010a5c4 */
            e->sub_state_1 = 8;                                    /* `addiu v0,zero,8` @0x8010a5c8 /
                                                                    * `sb v0,5(s0)` @0x8010a5d0 */
            return;                                                /* `j 0x8010a904` = EPILOG,
                                                                    * OHNE den Schwanz! */
        }
        e->re2z_t15a = 0;                                          /* sh zero,346 @0x8010a5d4 */
        /* FALLTHROUGH nach Sub 1 (@0x8010a5d4 -> @0x8010a5d8, kein Sprung) */
        /* fall through */
    case 1:                                                        /* @0x8010a5d8 GROWER + ADVANCE */
        if (e->re2z_dir16a != 0) {                                 /* `lb v0,362` / `beq` @0x8010a5d8-e0 */
            e->re2z_dir16a--;                                      /* @0x8010a5e8-ec */
            re2z_corpse_pool_grow(e);                              /* @0x8010a5f0-608 */
        }
        goto advance;                                              /* j 0x8010a7e0 @0x8010a604 */
    case 2:                                                        /* @0x8010a60c ZUCK-TAKT */
        e->re2z_gaitrow = (uint8_t)((re2z_rand() & 0xfu) + 1u);    /* +0x16B @0x8010a60c-20 */
        e->sub_state_1  = 3;                                       /* @0x8010a624-28 */
        {   uint16_t t = (uint16_t)e->re2z_t158;                   /* lhu +0x158 @0x8010a618 */
            e->re2z_t158 = (int16_t)(t - 1u);                      /* DELAY-SLOT, IMMER @0x8010a634 */
            if (t == 0u) {                                         /* `bne v1,zero` @0x8010a630 */
                e->sub_state_1 = 4;                                /* @0x8010a638-3c */
                e->re2z_t158   = 30;                               /* @0x8010a640-48 */
                goto tail;                                         /* j 0x8010a80c @0x8010a644 */
            }
        }
        /* FALLTHROUGH nach Sub 3 (@0x8010a634 -> @0x8010a64c) */
        /* fall through */
    case 3:                                                        /* @0x8010a64c GROWER + Pause */
        if (e->re2z_dir16a != 0) {                                 /* @0x8010a64c-54 */
            e->re2z_dir16a--;                                      /* @0x8010a65c-60 */
            re2z_corpse_pool_grow(e);                              /* @0x8010a664-678 */
        }
        {   uint8_t g = e->re2z_gaitrow;                           /* lbu +0x16B @0x8010a67c */
            e->re2z_gaitrow = (uint8_t)(g - 1u);                   /* DELAY-SLOT, IMMER @0x8010a68c */
            if (g == 0u) e->sub_state_1 = 1;                       /* @0x8010a690-98 */
        }
        goto tail;
    case 4:                                                        /* @0x8010a69c lange Pause */
        {   uint8_t g = e->re2z_gaitrow;                           /* lbu +0x16B @0x8010a69c */
            e->re2z_gaitrow = (uint8_t)(g - 1u);                   /* DELAY-SLOT @0x8010a6ac */
            if (g == 0u) e->sub_state_1 = 5;                       /* @0x8010a6b0-b8 */
        }
        goto tail;
    case 6:                                                        /* @0x8010a6bc Zuck-Nachladen */
        e->re2z_gaitrow = (uint8_t)((re2z_rand() & 0x3fu) + 60u);  /* +0x16B @0x8010a6bc-d0 */
        e->sub_state_1  = 4;                                       /* @0x8010a6d4-d8 */
        {   uint16_t t = (uint16_t)e->re2z_t158;                   /* lhu +0x158 @0x8010a6c8 */
            e->re2z_t158 = (int16_t)(t - 1u);                      /* DELAY-SLOT @0x8010a6e4 */
            if (t == 0u) {                                         /* `bne v1,zero` @0x8010a6e0 */
                e->sub_state_1 = 7;                                /* @0x8010a6ec-f0 — RUHE */
                /* fuenf Prim-Flagwoerter |= 0x8000 @0x8010a6f4-764: OPEN */
            }
        }
        goto tail;
    case 8:                                                        /* @0x8010a768 der 2/3-Zweig:
                                                                    * NUR Grower, kein Advance */
        if (e->re2z_dir16a != 0) {                                 /* @0x8010a768-70 */
            e->re2z_dir16a--;                                      /* @0x8010a778-7c */
            re2z_corpse_pool_grow(e);                              /* @0x8010a780-94 */
        }
        if (e->re2z_dir16a == 0) e->sub_state_1 = 7;               /* `lb v0,362 / bne` @0x8010a798-a4,
                                                                    * sonst @0x8010a7a8-b0 -> RUHE */
        goto tail;
    case 9:                                                        /* @0x8010a7b4 Zweit-Clip */
        re2z_clip(e, (e->re2z_flags21a & 0x4u) ? 0x16 : 0x07,      /* 0x00070007, bzw. 0x00070016
                                                                    * wenn +0x21A&4 @0x8010a7b4-d4 */
                  0, 7, 0x200, 0);
        e->sub_state_1 = 10;                                       /* @0x8010a7d8-dc */
        /* FALLTHROUGH nach Sub 10 (@0x8010a7dc -> @0x8010a7e0) */
        /* fall through */
    case 5:                                                        /* beide -> @0x8010a7e0 */
    case 10:
    advance:
        {   /* `lbu v1,5(s0)` @0x8010a7f4 liest +0x5 NACH dem jal — Reihenfolge explizit */
            int done = re2z_corpse_advance(e);                     /* jal @0x8010a7ec */
            e->sub_state_1 = (uint8_t)(e->sub_state_1 + (uint8_t)done);   /* @0x8010a7fc-804 */
        }
        goto tail;
    case 11:                                                       /* @0x8010a808 */
        e->sub_state_1 = 0;                                        /* sb zero,5(s0) */
        /* FALLTHROUGH in den Schwanz */
        /* fall through */
    case 7:                                                        /* @0x8010a80c = nur der Schwanz */
    default:
    tail:
        e->re2z_t15a = (int16_t)(e->re2z_t15a + 1);                /* +0x15A += 1 @0x8010a80c-20 */
        /* Farb-Ausblender @0x8010a818-68 und ABSACKEN @0x8010a86c-8f0: OPEN (s. Blockkopf) */
        break;
    }
}

/* ---- STATE 8 @0x80109CFC (11 subs @0x8010CF18) — the on-the-ground-alive family. ENTRY is
 * OPEN (the DEATH/HURT helper tails @0x80107A58/@0x80107EB8 that feed it are not RE'd); the
 * port implements the resolution so any future producer lands coherently: subs 5/6 commit the
 * get-up 0x901 (@0x8010AE9C) once the current clip finished. */
static void re2z_state8(re15_actor_t *e)
{
    if (re2z_clip_done(e))
        re15_ai_set_state_word(e, 0x901);                          /* sw 0x901 @0x8010AE9C */
}

/* ---- INIT (state 0) @0x8010065C ------------------------------------------------------------ */
static void re2z_init(int slot, re15_actor_t *e)
{
    uint8_t beh = e->grid_id;                                      /* Sce_em_set pc[3] (+0x9) */
    uint8_t sel = (uint8_t)(beh & 0x1f);                           /* RE1.5 spawn behavior nibble */
    /* Liege-Familien des RE1.5-Posenpakets, GENAU wie dort auf Bit 0x80 gegated
     * (`lbu v1,9(v0); andi v0,v1,0x80; beq -> 0x80100e30` @0x80100ca4-b0).
     *
     * ⚠ ZITAT KORRIGIERT (2026-08-17, Review-Fund F5): hier stand frueher "GENAU ZWEI
     * Liege-Familien". Unter DEMSELBEN 0x80-Gate liegen in STAGE1.BIN DREI sel-Decoder
     * (selbst disassembliert, roh @0x80100000):
     *   @0x80100cb8-ce0  sel {4,7,9}   -> +0x94 = 0x0c
     *   @0x80100cfc-d24  sel {5,8,0xa} -> +0x94 = 0x0e
     *   @0x80100d3c-d68  sel {1,3}     -> +0x94 = 0x0c (`ori v0,zero,0xc` @0x80100d54 /
     *                                     `sb v0,148(a0)` @0x80100d58)
     *                                     UND +0x5 = 5 (`ori v0,zero,0x5` @0x80100d64 /
     *                                     `sb v0,5(v1)` @0x80100d68)
     *   danach EIN gemeinsames f314 @0x80100da0 (+0x95=0 @0x80100d78, +0x8f=0 @0x80100d88)
     *   @0x80100dc0-de8  FINAL sel {4,7,9}   -> +0x94 = 0x12
     *   @0x80100e04-e2c  FINAL sel {5,8,0xa} -> +0x94 = 0x13
     * Der DRITTE Zweig bekommt KEIN FINAL — er behaelt 0x0c und traegt zusaetzlich den
     * Sub-State 5 (die RE1.5-Lane bildet ihn in enemy_ai_common.c bereits so ab).
     * Ausgeliefert erreichbar: beh 0x81 in ROOM1010/1011/1220/1221/4050/4051/5060/5061,
     * beh 0x83 in ROOM3010/3011.
     * Folge fuer den RE2-Flavor: {1,3} gehoeren zu den GEGATETEN Liege-Spawns und werden wie
     * {4,5,7,8,9,0xa} auf den Liege-Executor EXEC[7] abgebildet (PORT-OPTION, dokumentiert) —
     * sonst faellt beh 0x81/0x83 in den finalen else-Zweig, `re15_ai_set_state_word(e, 0x1)`
     * ueberschreibt den vom RE1.5-Live-INIT geseedeten sub_state_1=5, und die Leiche steht auf. */
    int lying_family = (sel == 1 || sel == 3 || sel == 4 || sel == 5 || sel == 7 ||
                        sel == 8 || sel == 9 || sel == 0x0a);
    /* the RE1.5 live-init supplies the PORT spawn data (HP row @0x8011f034, steer seed) — RE2
     * room data does not exist in RE1.5 rooms, so this is the byte-true data source. Its RE1.5
     * sub-state/motion writes are overridden below with the RE2 seeds. */
    re15_enemy_ai_live_init(slot);
    if (e->type == 0x13)                                           /* girl HP: her RE1.5 INIT row
                                                                    * (rng&0x1f)+50 @0x8010ac0c-1c */
        e->hp = (int16_t)((re15_engine_rand8() & 0x1f) + 50);

    /* RE2 INIT seeds (@0x8010065C..): */
    e->re2z_self1d3  = 0;                                          /* sb zero,467 @0x801006C8 */
    e->re2z_flags21a = 0;                                          /* sh zero,538 @0x8010087C */
    e->re2z_flag222  = 0;                                          /* sb zero,546 @0x80100884 */
    e->re2z_cd239 = 0; e->re2z_cd23e = 0;
    e->re2z_hits1d2 = 0;
    e->re2z_dir16a = 0;
    e->re2z_prev_sub = 0;
    /* ---- WELLE E: die Zerleger-Seeds des INIT ---- */
    e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;      /* `addiu v0,zero,13`
                                                                    * @0x8010081C, `sb v0,337/338/
                                                                    * 339(s2)` @0x80100820/24/28 */
    e->re2z_burn23a   = 0;                                         /* sb zero,570 @0x801008B4 */
    e->re2z_hitdir1d0 = 0;                                         /* sh zero,464 @0x801006BC */
    re15_re2z_part_reset(e);                                       /* Modellblock (PORT-MAPPING) */
    {   /* +0x218 walk clip = walkstyle[(r1 >> (r2&3)) & 7] (@0x80100860-8C, two-draw pick) */
        uint32_t r1 = re2z_rand(), r2 = re2z_rand();
        e->re2z_walkclip = re2z_param_walk[(r1 >> (r2 & 3u)) & 7u];
    }
    e->re2z_res223 = (int8_t)(16 + (re2z_rand() & 0xfu));          /* @0x80100888-9C */
    /* +0x21A Bit 0x8000 = "dieser Zombie laeuft mit Frame-Wort-SEs (und dem Extra-Turn)" —
     * eines der beiden Sub-Gates vor dem Drittel-Takt der WALK-/BUMP-Executoren
     * (@0x80101cf4-f8 / @0x80102414-18, siehe enemy_ai_common.c re15_enemy_anim_sfx).
     * INIT-Produzent, selbst disassembliert:
     *   80100894: lbu v1,8(s2)                 ; Entity-Typ
     *   801008a0: addiu v0,zero,17             ; 0x11
     *   801008bc: bne v1,v0,0x801008d8         ; nur Typ 0x11 faellt durch
     *   801008c4: lhu v0,538(s2)
     *   801008c8: addiu v1,zero,250
     *   801008cc: sh  v1,342(s2)               ; +0x156 = HP = 250  (portiert, s.u.)
     *   801008d0: ori v0,v0,0x8000
     *   801008d4: sh  v0,538(s2)               ; +0x21A |= 0x8000
     * WICHTIG ZUM ZEITPUNKT: das ist der INIT-HANDLER (Zustand 0, Tabelle @0x8010C830) — im
     * Original wie im Port laeuft er im ERSTEN KI-TICK der Entity, NICHT beim Sce_em_set-Spawn.
     * Wer das Bit direkt nach dem Spawn liest (ohne einen re15_enemy_ai_run_all-Tick), sieht
     * korrekterweise 0.
     *
     * OPEN (nicht erfunden): @0x801008D8-0x80100950 setzt dasselbe Bit fuer 1 von 3 zufaelligen
     * Zombies, aber nur wenn das RE2-Spielglobal DAT_800cfb74 Bit 0x40 traegt
     * (`lw v0,-1164(v0)` @0x801008DC, `andi v0,v0,0x40` @0x801008E4, `beq -> 0x80100954`
     * @0x801008E8; Rest-3-Test @0x80100900-28; `+0x223 = (rand&0xf)+32` @0x80100944-4C).
     * Der Port hat kein Gegenstueck zu diesem RE2-Global -> Pfad bleibt OPEN.
     *
     * ⚠ +0x156 = 250 (@0x801008C8-CC) LIEGT NICHT HIER, SONDERN IN re15_damage.c
     * (re15_re2_init_hp -> re15_re2_hp_sync, gestempelt aus game_step). Grund: +0x156 ist das
     * HP-Halbwort, und der RE2-INIT wuerfelt es sonst aus einer Tabelle (`lhu v0,-14736(at)` /
     * `sh v0,342(s2)` @0x80100708-10, Tabelle 0x8010C670, Index (rand>>(rand&3))&0xf, +15 bei
     * Raum-Gegnerzahl < 4 @0x801007FC-814) — das ganze Modell gehoert an EINE Stelle, sonst
     * driften zwei Kopien derselben Regel auseinander. Der Stempel laeuft NACH dem INIT-Tick
     * und ersetzt die RE1.5-Live-INIT-HP.
     * ⛔ WICHTIG: die 250 und das Bit 0x8000 zwei Zeilen weiter unten sind DIESELBE
     * Verzweigung (`bne v1,v0` @0x801008BC). Wer eine der beiden anfasst, muss die andere
     * mitfuehren. Die frueher hier begruendete Auslassung ("nicht portiert, weil unbelegt, dass
     * RE1.5-Typ 0x11 dasselbe Wesen ist") ist am 2026-08-20 WIDERLEGT worden: beide Spiele
     * lesen den Typ aus ENTITY+0x8 (RE1.5 @0x801007d8), der Port indiziert den RE2-Asset-TOC
     * @0x8009ADF4 mit genau diesem Typ (re2_ems.c `((kind-0x10)*4+rec)*2`) und laedt fuer 0x11
     * eine EIGENE, andere EM011-Textur (313/66592 Bytes gleich zu EM010) — RE2-EM011 IST Brad
     * Vickers (BioModels.h:186). Volle Herleitung im Block ueber re15_re2_init_hp.
     *
     * NULL-STORES DESSELBEN BLOCKS (@0x801008A4-C0) — Port-Abgleich, damit die Luecke benannt ist:
     *   `sh zero,538` @0x8010087C -> re2z_flags21a = 0            PORTIERT (oben)
     *   `sb zero,546` @0x80100884 -> re2z_flag222 = 0             PORTIERT (oben)
     *   `sb v0,536`   @0x8010088C -> re2z_walkclip                PORTIERT (oben)
     *   `sb v0,547`   @0x8010089C -> re2z_res223                  PORTIERT (oben)
     *   `sb zero,537` @0x80100880 (+0x219), `sb zero,560/561` @0x801008A4/A8 (+0x230/+0x231),
     *   `sh zero,566` @0x801008AC (+0x236), `sb zero,569/570/571/572` @0x801008B0/B4/B8/C0
     *   (+0x239/+0x23A/+0x23B/+0x23C): davon hat der Port NUR +0x239 (re2z_cd239, direkt darunter
     *   genullt) und +0x219 (re2d_air219, ein HUND-Feld derselben Union — der Zombie-INIT nullt es
     *   im Original mit, der Port haelt die Flavor-Felder getrennt). +0x230/+0x231/+0x236/+0x23A/
     *   +0x23B/+0x23C haben im Port GAR KEIN Feld und auch keinen Leser -> nichts zu nullen. */
    if (e->type == 0x11) e->re2z_flags21a |= 0x8000u;              /* @0x801008BC-D4 — ZWILLING
                                                                    * der 250-HP-Zeile
                                                                    * @0x801008C8-CC (dieselbe
                                                                    * bne-Verzweigung); die HP-
                                                                    * Haelfte steht in
                                                                    * re15_re2_init_hp
                                                                    * (re15_damage.c). Beide sind
                                                                    * scharf — nie nur eine
                                                                    * aendern. */
    e->re2z_prev_hp = e->hp;
    e->speed_h = 0;                                                /* +0x144 spawn-clean (kein Walk-
                                                                    * Writer; Attacken saeen 11) */
    e->root_prev_kf = -1;                                          /* move_root delta re-anchor */
    e->sca_mask = 4;                                               /* upright SCA row (RE1.5 twin) */

    /* spawn-mode remap: the RE1.5 behavior nibble -> the RE2 spawn states. Die RE2-Spawns
     * schreiben +0x10E = 0x4004 (Feeder @0x80100A88-8C) / 0x4002 (Lyer @0x80100A34-38) —
     * das Limpet-Latch 0x4000 haelt die Executor-Ketten, bis der (gemappte) Wecker es loescht. */
    e->re2z_f10e = 0;
    if (sel == 6) {                                                /* feeding -> ACTIVE sub 8 */
        e->re2z_f10e = 0x4004u;                                    /* sh 0x4004,270 @0x80100A88-8C */
        re15_ai_set_state_word(e, 0x801);                          /* @0x80100AD4 */
        re2z_clip(e, 0x12, 0, 0, 0x100, 0);                        /* INIT-Seed Clip 18 PLAIN
                                                                    * (`addiu 18; sw 332`
                                                                    * @0x80100AD0-DC) — EXEC[8] P0
                                                                    * zieht naechsten Tick neu */
    } else if (sel == 8 || sel == 0x0b || sel == 0x0e ||
               (lying_family && (beh & 0x80u))) {                   /* lying -> ACTIVE sub 7 */
        /* ⛔ D15.1 — Nutzer-Report ROOM1100 "Zombies nicht komplett korrekt positioniert":
         * sel 7 (= behavior 0x87, die Slots 1/3 des Evidence-Korridors) fiel in den else-Zweig
         * und STAND aufrecht zwischen den liegenden 0x88ern (2 von 5 "Leichen").
         * Beleg: das RE1.5-Liege-Posenpaket (STAGE1.BIN roh, selbst disassembliert) — DREI
         * sel-Decoder unter EINEM 0x80-Gate (Zitat korrigiert 2026-08-17, Review-Fund F5;
         * hier stand frueher faelschlich "GENAU ZWEI Liege-Familien"):
         *   Gate  `lbu v1,9(v0); andi v0,v1,0x80; beq -> 0x80100e30`   @0x80100ca4-b0
         *   sel {4,7,9}   -> +0x94 = 0x0c  @0x80100cb8-e0   (Pose VOR dem f314)
         *   sel {5,8,0xa} -> +0x94 = 0x0e  @0x80100cfc-d24
         *   sel {1,3}     -> +0x94 = 0x0c  @0x80100d3c-58   UND +0x5 = 5 @0x80100d64-68
         *   +0x95 = 0 @0x80100d78, +0x8f = 0 @0x80100d88
         *   EIN  f314(+0x170/+0x174, a2=0, a3=0x200)        @0x80100da0
         *   sel {4,7,9}   -> +0x94 = 0x12  @0x80100dc0-e8   ← FINAL (ueberschreibt 0x0c NACH f314)
         *   sel {5,8,0xa} -> +0x94 = 0x13  @0x80100e04-e2c  ← FINAL
         *   sel {1,3}     -> KEIN FINAL, behaelt 0x0c + Sub-State 5
         * beh 0x87 -> sel 7 -> Clip 0x12, beh 0x88 -> sel 8 -> 0x13, beh 0x81/0x83 -> Clip 0x0c.
         * Der RE2-Flavor hat keine RE1.5-Clip-Indizes, bildet aber ALLE DREI Familien auf
         * denselben Liege-Executor EXEC[7] ab. sel 8 bleibt (wie bisher) ohne 0x80-Gate, damit die
         * bestehenden ROOM1140/ROOM1100-Pins unveraendert greifen; 1/3/4/5/7/9/0xa sind exakt so
         * gegated wie @0x80100cac. 0x0b/0x0e stammen aus dem ZWEITEN Decoder-Block, der NICHT auf
         * 0x80 gegated ist (`andi v0,v0,0x1f` @0x80100e44/@0x80100ec8; scd_vm.c:3001/3003 =
         * Clip 3 / 0x2A).
         * ⚠ ENTFERNT (2026-08-17): die Zusatzbedingung `&& sel != 7` schloss ausgerechnet sel 7
         * wieder aus — sie hatte KEINEN Byte-Beleg und widersprach sowohl diesem Kommentar als
         * auch @0x80100cbc/@0x80100cd4 (sel 7 steht dort explizit in der ERSTEN Familie).
         * PORT-OPTION (RE2-Flavor ist kein RE1.5-Original-Verhalten). */
        e->re2z_f10e = 0x4002u;                                    /* sh 0x4002,270 @0x80100A34-38 */
        re15_ai_set_state_word(e, 0x701);                          /* @0x801009E8-0x80100A84 */
    } else if (sel == 0x0d) {                                      /* pre-engaged -> WALK */
        re15_ai_set_state_word(e, 0x101);
    } else {
        re15_ai_set_state_word(e, 0x1);                            /* sw 1,4(s2) @0x801006AC-B8 */
    }
}

/* ---- the root tick ------------------------------------------------------------------------- */
int re15_re2z_tick(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e->active) return 0;

    /* root prolog = the cooldown bank, BEFORE the state dispatch (@0x801004E0 reads the table):
     * +0x239 @0x8010045C-6C, +0x23E @0x80100470-80, self+0x1D3 low-7 @0x80100484-98
     * (+0x230 @0x8010049C.. has no modeled consumer). */
    /* Die Bone-Injektion des Vor-Ticks verfaellt: das Original baut die Part-Matrizen in JEDEM
     * Advance (FUN_8002959C) neu und multipliziert danach absolut auf — nichts akkumuliert.
     * Der Port muss dieselbe Ein-Tick-Lebensdauer haben, sonst bleibt der Oberkoerper nach der
     * Reaktion verdreht stehen. Der Lean-VEKTOR (+0x13C..+0x140) bleibt erhalten, nur das
     * Anwenden wird pro Tick neu bestellt. */
    e->re2_lean_on    = 0;
    e->re2_bone0_wgt  = 0;

    if (e->re2z_cd239) e->re2z_cd239--;
    if (e->re2z_cd23e) e->re2z_cd23e--;
    if (e->re2z_self1d3 & 0x7fu)
        e->re2z_self1d3 = (uint8_t)((e->re2z_self1d3 & 0x80u) | ((e->re2z_self1d3 & 0x7fu) - 1u));

    if (e->state != 2) e->re2z_prev_hp = e->hp;                    /* HP snapshot for the HURT
                                                                    * resistance write-off */

    /* ⛔ D15.2 GEFIXT — SKRIPT-WECKER fuer die Liegenden (Nutzer-Report ROOM1100: nach dem
     * "Leichen erwachen"-Event passiert nichts). Das Weck-Signal des Spiels ist ein RE1.5-
     * MECHANISMUS und existiert in der RE2-Overlay-KI nicht:
     *   ROOM1100 sub02 fuehrt pro Zombie `Work_set(2,n); Member_set(12, 0x89 bzw. 0x8A)` aus.
     *   Member 12 ist entity+0x9 — der Sprungtabellen-Fall endet mit `j 0x80041230` @0x800411f4
     *   und dem Delay-Slot-Store **`sb a2,9(a0)` @0x800411f8**. Der Bump hebt das Liege-Nibble
     *   7/8 auf 9/10.
     *   In RE1.5 haengt an 9/10 der Dispatcher @0x8011f80c[9]/[10] = 0x801019f0, dessen DECIDE-Zeile
     *   @0x8011f9dc[0] = 0x801039fc der SOFORT-WECKER ist (Gegenstueck: 7/8 -> @0x8011f9d8[0] =
     *   0x801039f4 = `jr ra`-Stub = kein Selbstwecken). Port-Zwilling: enemy_ai_common.c case 9/10.
     *   Der RE2-Brain liest den Nibble nach dem INIT nie -> die Liegenden blieben fuer immer in
     *   s1==7 (gemessen 400+ Ticks ohne Transition).
     * PORT-MAPPING (RE2-Flavor, dokumentiert): der Bump wird hier in die byte-true RE2-Kette
     * uebersetzt — der Bump LOEST NUR DEN LIMPET-LATCH `+0x10E &= ~0x4000`. Danach laeuft die
     * Aufsteh-Kette des RE2-Liege-/Fress-Executors VON SELBST:
     *   EXEC[7] P1 @0x8010381C-28 haelt, solange +0x10E & 0x4000 steht -> P2 @0x80103838 setzt
     *           den BODEN-Aufstehclip `re2z_param_clips[4+back]` = 8/9 (@0x80103840-80),
     *           P3 @0x801038D8 spielt ihn aus, P4 @0x80103900 committet `0x101` (er steht).
     *   EXEC[8] P1 @0x80103C94-A0 haelt genauso -> P3 @0x80103CD8 Clip 0x15, P4/P5 -> `0x101`.
     *
     * ⛔ HIER STAND BIS 2026-08-21 ZUSAETZLICH `re15_ai_set_state_word(e, 0x901)` — DAS WAR DER
     * NUTZER-BEFUND "Nach dem Loesen des Raetsels im Generator-Raum steht der Zombie direkt neben
     * Leon abrupt, quasi ohne Animation, direkt vom Liegen zum Stehen."
     * GEMESSEN (probe_re2z_deathgetup B, ROOM11F0, die ECHTEN sub18-Bytes @Datei 0x178C-0x17B0
     * durch die ECHTE SCD-VM, geladene RE2-Bank EM010; Pose ueber die RENDERER-Bankwahl):
     *   f52 st=1 s1=7 s2=1 clip=23 af=9/10 kf=741 10E=4002 grid=87 -> Brust b8dy = **-150**
     *   f53 st=1 s1=9 s2=1 clip= 3 af=0/40 kf=124 10E=0002 grid=00 -> Brust b8dy = **-2004**
     * Der Zombie stand in EINEM Frame — 128 von 128 Aufsteh-Vorgaengen ueber 32 Seeds.
     * URSACHE: EXEC[9] ist BYTE-BELEGT KEIN Aufstehen vom Boden. Selbst disassembliert
     * (0x80103E48..0x80104170):
     *   Clip-Tabelle sp+16 = @0x801000D8 = {3,3,4,0x0D}, Index `(+0x16B)*2 + (+0x16A)`
     *     (`sll v1,v1,1 / addu v1,v1,s0 / addu v1,v1,a0 / lbu v1,0(v1)` @0x80103F14-20) —
     *     und +0x16A ist ein frischer RNG-Wurf (`andi v0,v0,0x1 / sb v0,362` @0x80103EF8-F00).
     *   Alle drei Clips STARTEN AUFRECHT (Bank EM010, Brust in Frame 0: Clip 3 = -2004,
     *     Clip 4 = -2528, Clip 13 = -2685) — der BODENclip ist 8/9 (-150/-129).
     *   P0 setzt ausserdem `+0x144 = 400` (@0x80103F60-64) und `+0x158 = (+0x16B)<<11`
     *     (@0x80103F78-80), P1 schiebt damit (`jal 0x800152C8` @0x80103FBC) und baut mit
     *     `addiu v0,v0,-30` (@0x80103FCC) ab; im Frame, in dem der Schub endet (+0x15A == 1,
     *     @0x80103FF0-F8), faellt der Zombie mit 7/8 Wahrscheinlichkeit HIN
     *     (`s = (r1 >> (r2&3)) & 7; s != 0 -> +0x4 = 0x501` @0x80104000-28 = EXEC[5]-Sturz).
     * EXEC[9] ist also der STOSS-/TAUMEL-Executor, nicht der Boden-Aufsteher. Der Commit riss den
     * Liegenden aus EXEC[7] heraus, BEVOR dessen eigenes P2 den Bodenclip setzen konnte.
     * Der reine Latch-Clear ist zugleich exakt derselbe Mechanismus, den der (schon vorhandene)
     * Naehe-Wecker in re2z_exec_lying benutzt und dessen glatter Verlauf dort gemessen ist
     * (f44 Clear -> f45 P2/P3 Clip 8 -> f112 P4 -> f113 `0x101`).
     * Das Liege-Nibble wird hier NICHT mehr geloescht: das erledigen die Pose-Ausgaenge selbst
     * (EXEC[7] P4 / EXEC[8] P5, beide `grid_id = 0`) — genau wie im RE1.5-Zwilling, wo der
     * Nibble 9/10 bis zum Aufstehen stehen bleibt. Der Clear hier ist idempotent.
     * Damit uebernimmt die gestaffelte sub18-Kaskade (Sleep zwischen den Bumps) dieselbe
     * Dramaturgie wie in RE1.5.
     * ⚠ MASKE KORRIGIERT (2026-08-17, Review-Fund F4): hier stand `&= ~0x4002`. Der EINZIGE
     * +0x10E-Clear des Overlays loescht nur Bit 0x4000 —
     *   80104f0c: andi v1,v1,0xbfff
     *   80104f10: sh   v1,270(s0)
     * (eigener Scan aller 84 +0x10E-Zugriffe in EMOVL10_S0.BIN: AND-Masken nur 0xbfff
     * @0x80104F0C, 0xdfff @0x80103744/sh @0x8010374C und `andi 0xffc0; ori 0x1` @0x80104590-98).
     * Bit 0x0002 ueberlebt im Original: die Low-6-Bits von +0x10E sind der State-1-Dispatch-
     * Selektor (`lhu v0,270(a0)` @0x80101154, `andi 0x3f` @0x8010115C, Tabelle @0x8010C854)
     * und werden nach dem INIT weitergelesen (`andi 0x3f; ==6` @0x80104A80-94). Der Spawn-Seed
     * `addiu v0,zero,16386` / `sh v0,270(s2)` @0x80100A34-38 ist ein WRITE, kein Beleg fuer die
     * Umkehrung. Gegenprobe in dieser Datei: dieselbe Instruktion ist an zwei weiteren Stellen
     * korrekt als `~0x4000u` mit demselben Zitat @0x80104F0C abgebildet. */
    {   uint8_t nib = (uint8_t)(e->grid_id & 0x0fu);
        if (nib >= 9 && nib <= 10 && e->state == 1
            && (e->sub_state_1 == 7 || e->sub_state_1 == 8))       /* EXEC[7] Liegend / EXEC[8]
                                                                    * Fressend — beide haengen am
                                                                    * SELBEN Latch (@0x8010381C-28
                                                                    * bzw. @0x80103C94-A0) */
            e->re2z_f10e &= (uint16_t)~0x4000u;                    /* andi 0xbfff @0x80104F0C */
    }

    /* ============================================================================================
     * ⛔ D15.3 — DIE KRIECH-BRUECKE: RE1.5-SKRIPTPROTOKOLL -> RE2-KRIECHER (ROOM1030-Tor).
     *
     * NUTZER-REPORT 2026-08-21: "Bei RE2 AI in der Lobby kriechen die Zombies in der Cutscene
     * nicht unter das Tor (ROOM1030)."
     *
     * DAS IST EINE BEWUSSTE BRUECKE ZWISCHEN ZWEI SPIELEN — wie re15_re15_re2z_import und wie
     * der D15.2-Wecker direkt darueber. Grund, byte-belegt statt behauptet:
     *   RE1.5-SEITE (der Anforderer): ROOM1030 sub07 @Datei 0x2754 `3d 04 10 / 26 00 05 04 00 10
     *     / 35 10 04` = Member16 (entity+0x1C4, `sh a2,452(a0)` @0x80041218) |= 0x1000. Die drei
     *     RE1.5-Steer-Decides lesen das Bit und schreiben +0x4 = 0x1001 (@0x80101ECC-ED8,
     *     @0x801021C0-E0, @0x80105784-A4) -> Toggle FUN_80104F80 -> Kriech-Grid.
     *     Der RUECKWEG ist das ZWEITE Skript-Bit: `+0x1C4 & 0x2000` @0x8010369C-A4 (mit
     *     `!(+0x1D8 & 0x80)` @0x801036B0-BC) -> Wort 0x601 = derselbe Toggle, zurueck auf
     *     +0x1D7 = 4 (@0x801050B4). Port-Zwilling: re15_zgirl_overflow_row11 (enemy_ai_common.c).
     *   RE2-SEITE (der Ausfuehrer): das RE2-Zombie-Overlay hat fuer 0x1000 KEINEN Konsumenten.
     *     Eigener Byte-Scan von EMOVL10_S0.BIN: alle 36 Zugriffe auf +0x1C4/+0x1C6 sind das PAAR
     *     `lh a1,452 / lh a2,454` = das STEUER-ZIEL (x,z) fuer FUN_80015558 (z.B. @0x80104400/04,
     *     @0x801030D4/D8) — dort liegt in RE2 gar kein Bitfeld. Die acht `andi 0x1000` des
     *     Overlays (@0x80101DDC, @0x80101E5C, @0x80101E9C, @0x80102490, @0x801024E8, @0x80102528,
     *     @0x80105E18, @0x80108B44) lesen ALLE +0x21A (`lhu v0,538`), nicht +0x1C4.
     *     Der RE2-Kriecher wird ausschliesslich ueber +0x10E Bit 0 betreten.
     *
     * DIE BRUECKE UEBERSETZT ALSO NUR DEN ANFORDERUNGSKANAL — die Zielwerte sind alle
     * byte-gelesen aus dem RE2-Overlay:
     *   HIN   (+0x1C4 & 0x1000): re15_re2z_enter_crawler(sub 0) = `sh 1,270` @0x80107A54 +
     *         `sw 1,4` @0x80107A58 (die Kombination, mit der der Kriecher-HURT selbst in seine
     *         LOKOMOTION zurueckkehrt) — Lokomotion, nicht der Kampf-Griff `sw 0x101`
     *         @0x8010458C, denn das Skript will Fortbewegung, keinen Angriff.
     *   ZURUECK (+0x1C4 & 0x2000): +0x10E Bit 0 loeschen und `sw 0x901` = EXEC[9] AUFSTEHEN
     *         @0x80103E48 — dasselbe Ziel, das schon der D15.2-Wecker und der RE2-Liege-Executor
     *         selbst ansteuern. Plus SCA-Zeile zurueck auf 4 (@0x801050B4, RE1.5-Raumdaten).
     * Der Spieler-Riegel wird beim Skript-Kriechen NICHT gesetzt: er gehoert dem Angriff
     * (@0x8010459C-B0 haengt hinter dem Kampf-Test), nicht der Fortbewegung.
     * ========================================================================================== */
    if ((e->anim_flags & 0x2000u) && (e->re2z_f10e & 1u)) {
        e->re2z_f10e &= (uint16_t)~1u;                             /* Kriecher-Bit aus */
        e->sca_mask  = 4;                                          /* +0x1D7 = 4 @0x801050B4 */
        re15_ai_set_state_word(e, 0x901);                          /* EXEC[9] Get-up @0x80103E48 */
    } else if ((e->anim_flags & 0x1000u) && !(e->re2z_f10e & 1u)
               && e->state == 1 && e->hp >= 0) {
        /* `e->state == 1` spiegelt die RE1.5-Seite: dort lesen NUR die Zustand-1-Decides das Bit
         * (@0x80101ECC / @0x801021C0 / @0x80105784) — ein getroffener oder toter Zombie legt sich
         * nicht auf Skriptbefehl hin. `hp >= 0` ist Gate (3) des RE2-Trefferfilters
         * (`bltz v0,0x8004740c` @0x80047150), hier als dieselbe Aussage. */
        re15_re2z_enter_crawler(e, NULL, 0u);
    }

    switch (e->state) {                                            /* table @0x8010C830 */
    case 0: re2z_init(slot, e); break;                             /* 0x8010065C */
    case 1: /* Zustand-1-Wurzel @0x8010114C: `andi 0x3f` @0x8010115C in die Tabelle @0x8010C854,
             * deren 14 Eintraege STRIKT auf Bit 0 alternieren (eigener Dump, s. Kriecher-Block):
             * ungerade -> 0x80101210 (Kriecher), gerade -> 0x8010118C (aufrecht). */
            if (e->re2z_f10e & 1u) re2z_crawl(slot, e, pl);        /* 0x80101210 */
            else                   re2z_active(slot, e, pl);       /* 0x8010118C */
            e->re2z_prev_sub = e->sub_state_1;                     /* +0x5-Schnappschuss fuer HURTs
                                                                    * sub==1-Gate (@0x80105090-98;
                                                                    * das geteilte take_damage
                                                                    * ueberschreibt +0x5) */
            break;                                                 /* 0x8010114C */
    case 2: re2z_hurt(e, pl); break;                               /* 0x80104F40 */
    case 3: re2z_death(e, pl); break;                              /* 0x80108250 */
    case 7: re2z_corpse(e); break;                                 /* 0x8010A440 */
    case 8: re2z_state8(e); break;                                 /* 0x80109CFC */
    default: re15_ai_set_state_word(e, 0x101); break;              /* [4] EXE-shared nav / [5][6]
                                                                    * NULL -> recover to walk */
    }

    /* ============================================================================================
     * ⛔ DIE TREFFBARKEIT IM RE2-ZWEIG IST +0x1D3 — NICHT DER RE1.5-LATCH +0x93
     * --------------------------------------------------------------------------------------------
     * NUTZER-REPORT 2026-08-19: "Nach ein paar Treffern schwanken die Zombies nach hinten. Beim
     * Original wuerden sie dann umfallen, wenn man sie noch mal trifft. Bei uns nicht."
     *
     * GEMESSEN (probe_re2_stagger, ROOM1140, echter Weg game_step + R1/SQUARE, RE2-Bank geladen):
     *   f132 Treffer -> +0x222 = 1, Stagger-P0 (Clip 4, Schub -450 = "nach hinten schwanken")
     *   f132..f174   -> Stagger-P1, +0x93 = 0x01 die GANZE Zeit
     *   f175         -> P2: +0x223 neu geseedet, +0x222 = 0
     *   f187         -> erst JETZT wieder ein Treffer (11 Frames nach der Latch-Freigabe)
     * Der Flinch 0x501 (@0x801050A4) — der einzige TREFFER-Weg in den Sturz-Executor EXEC[5]
     * (der zweite 0x501-Produzent @0x80102D24-2C ist der Griff-Ausgang, kein Treffer) — wurde in
     * 900 Frames NIE erreicht: sein Gate ist `+0x222 == 1` (@0x80105080-98), und +0x222 ist
     * ausschliesslich WAEHREND des Staggers 1 (gesetzt @0x80105164, geloescht in Stagger-P2
     * @0x80106034 bzw. EXEC[5]-P0 @0x801032F4). Es braucht also einen Treffer MITTEN in der
     * Reaktion — und genau den hat der Port abgewiesen.
     *
     * ---- WAS DAS ORIGINAL TUT (selbst disassembliert, info/re2leon/PSX.EXE) --------------------
     * Die RE2-Trefferschleife FUN_800470C0 filtert ihre Kandidaten so:
     *   80047124: lw   v0,0(s0)          ; Entity-Flagwort
     *   8004712c: andi v0,v0,0x1
     *   80047130: beq  v0,zero,0x8004740c ; inaktiv -> naechster Kandidat
     *   80047138: lbu  v0,467(s0)        ; +0x1D3
     *   8004713c: nop
     *   80047140: bne  v0,zero,0x8004740c ; **+0x1D3 != 0 -> Kandidat UEBERSPRUNGEN**
     *   80047148: lh   v0,342(s0)        ; HP < 0 -> ueberspringen …
     * RE2 kennt kein +0x93: eigener Voll-Scan aller `sb/lbu rt,467(rs)` in EMOVL10_S0.BIN zeigt,
     * dass WEDER der Haupt-Handler 0x80105438 NOCH der Stagger 0x80105BC0 +0x1D3 je schreiben —
     * der Zombie bleibt in beiden Reaktionen TREFFBAR. Gesetzt wird +0x1D3 |= 0x80 nur dort, wo
     * der Port es auch schon setzt: Ragdoll-P0 @0x801069A4-B0, Knockdown-P0 @0x801074F0-504,
     * EXEC[5]-P0 @0x80103304, EXEC[6] @0x801039F8 — und passend wieder geloescht.
     *
     * ---- WARUM DER PORT ES ANDERS MACHTE ------------------------------------------------------
     * Der Port faehrt in BEIDEN Flavors den RE1.5-Resolver FUN_80011f50, dessen Ein-Treffer-Latch
     * +0x93 Bit 0 (@0x800124E8/F0) einen Kandidaten bis zur Freigabe abweist. Die Freigabe wurde
     * an den RE1.5-Ort gehaengt (Reaktions-ENDE, @0x80105f9c-fac) — damit war der RE2-Zombie
     * fuer die gesamte Reaktion unverwundbar und der Flinch-Zweig toter Code.
     * PORT-VERTRAG, kein geratener Wert: der Latch, den der geteilte Resolver liest, SPIEGELT
     * jetzt exakt den Original-Filter — treffbar genau dann, wenn +0x1D3 == 0. Die bestehenden
     * Freigaben an den Reaktions-Ausgaengen bleiben stehen (sie sind dieselbe Aussage) .
     *
     * ⛔ KORREKTUR 2026-08-20 — DER SPIEGEL WAR UNVOLLSTAENDIG UND EINSEITIG.
     * Nutzer-Report v0.3.3: "(a) manchmal fallen sie einfach so hin", "(b) stehen mitten in der
     * Hinfall-Animation wieder auf", "(e) beim Anschiessen und Sterben landen sie weitestgehend
     * animationslos am Boden".
     * GEMESSEN (probe_re2_zfall Modus 4, ROOM1140, echter Weg game_step + R1/SQUARE, RE2-Bank
     * EM010 geladen — Dauerfeuer Pistole):
     *   f165 Treffer -> st=3 DEATH, s1=3, s2=1, clip 7 (Laenge 70) — die Todes-Anim startet
     *   f176 TREFFER auf die LEICHE (hp -1 -> -17), Todes-Phase s2 faellt 1 -> 0
     *   f187/198/209/220/231 fuenf WEITERE Treffer, hp bis -97, s2 bleibt bei 0 haengen
     * Der Port beschiesst also die Leiche weiter und setzt dabei jedes Mal die Todes-Phase zurueck.
     *
     * ---- DER ORIGINAL-FILTER HAT VIER GATES, DER PORT SPIEGELTE EINS -------------------------
     * FUN_800470C0, Kandidaten-Schleife (info/re2leon/PSX.EXE, selbst disassembliert):
     *   80047124: lw   v0,0(s0)          ; Entity-Flagwort
     *   8004712c: andi v0,v0,0x1
     *   80047130: beq  v0,zero,0x8004740c ; (1) inaktiv                 -> UEBERSPRUNGEN
     *   80047138: lbu  v0,467(s0)         ; +0x1D3
     *   80047140: bne  v0,zero,0x8004740c ; (2) +0x1D3 != 0             -> UEBERSPRUNGEN
     *   80047148: lh   v0,342(s0)         ; +0x156 = HP
     *   80047150: bltz v0,0x8004740c      ; (3) HP < 0 (TOT)            -> UEBERSPRUNGEN
     *   80047158: lhu  v0,270(s0)         ; +0x10E
     *   80047160: andi v0,v0,0xc000
     *   80047164: bne  v0,zero,0x8004740c ; (4) +0x10E & 0xC000 gesetzt -> UEBERSPRUNGEN
     *   8004716c: lhu  v1,464(s0)         ; erst DANACH beginnt die Trefferpruefung
     * 717d13e0 hat NUR Gate (2) gespiegelt. Gate (3) fehlte -> Leichen bleiben beschiessbar;
     * Gate (4) fehlte -> der Limpet-/Liege-Latch 0x4000 und das Schon-gefallen-Bit schuetzen nicht.
     *
     * ---- UND DER SPIEGEL WAR EINSEITIG --------------------------------------------------------
     * Der alte Code konnte den Latch nur FREIGEBEN (`&= ~1`), nie SETZEN. Damit war jeder Zombie
     * ohne +0x1D3 in JEDEM Frame treffbar — auch tot, auch liegend, auch mitten in einer laufenden
     * Reaktion. Der geteilte RE1.5-Resolver blockiert einen Kandidaten aber ausschliesslich ueber
     * den gesetzten Latch (@0x800123fc-418 Zweitkontakt-Rekursion, Port re15_damage.c:1154-1156);
     * ein Filter, der nur freigibt, ist deshalb GAR KEIN Filter. Beide Richtungen sind noetig.
     *
     * ---- WARUM BIT 1 (+0x93 & 2) HIER MITGELOESCHT WIRD ---------------------------------------
     * Das Blockieren ueber Bit 0 zieht im RE1.5-Resolver zwangslaeufig `+0x93 |= 2` nach sich
     * (@0x8001240c / @0x80012fcc, Port re15_damage.c:1155/1373) — und Bit 1 ist der Ausloeser des
     * RE1.5-Gore-Spawns FUN_80106a44 (re15_enemy_gore_tick, re15_damage.c:1411), der an der
     * AKTOR-WURZEL e->x/y/z spawnt, also AM BODEN ZWISCHEN DEN FUESSEN. Das ist der Nutzer-Befund
     * (d) "manchmal taucht das Blut an der richtigen Stelle auf, und manchmal beim Bein unten".
     * RE2 hat das Feld +0x93 UEBERHAUPT NICHT: eigener Voll-Scan beider RE2-Binaries nach
     * `sb/lbu rt,147(rs)` liefert 0 Treffer in info/re2leon/PSX.EXE UND in EMOVL10_S0.BIN
     * (Beleg schon im Kopf von re2z_hit_latch_release). Ein +0x93-getriebener Wurzel-Gore ist im
     * RE2-Gehirn also definitionsgemaess FREMDCODE. Die RE2-Blutquellen sind die drei
     * anker-gebundenen Emitter (@0x801050B0-158 / @0x80105650-704 / @0x80105D14-DBC), und die
     * laufen bereits ueber re2z_blood_fx_at an der Knochen-Matrix. Deshalb: Bit 1 gehoert bei
     * einem RE2-eigenen Zombie in JEDEM Tick auf 0 — sonst erzeugt ausgerechnet der korrekte
     * Filter das Fuss-Blut. */
    /* ============================================================================================
     * ⛔ DIE SPAWN-POSE IST VOM FILTER AUSGENOMMEN — SOLLSEITE IST RE1.5, NICHT RE2
     * --------------------------------------------------------------------------------------------
     * NUTZER-REPORT v0.3.5: "Im Dining Room trifft man den am Boden fressenden Zombie nicht."
     * GEMESSEN (Parallel-Auftrag, ROOM1140, echter Weg): RE2-Modus, Abstand 6000 -> 0 Treffer in
     * 240 Frames (+0x1D3 = 0x80, +0x10E = 0x4004, daraus hit_react = 0x01); Abstand 2600 ->
     * 5 Treffer. RE1.5-Modus, derselbe Schuss auf 6000 -> 2-3 Treffer.
     * Eigener Seed-Sweep (probe_re2z_abc, 64 Seeds x 900 Frames, 3 Waffen): der Liegende (EXEC[7])
     * ist in 64/64 Seeds dauerhaft gesperrt, die Fresser (EXEC[8]) in 57-83 Faellen.
     *
     * WARUM DER PORT DA UEBERHAUPT HINKOMMT — beide Sperren sind byte-true, aber der ZUSTAND ist
     * port-synthetisiert:
     *   (2) +0x1D3 |= 0x80 — EXEC[7] P0 `ori 0x80` @0x80103804-14, EXEC[8] P0 @0x80103C04-14
     *   (4) +0x10E = 0x4002 / 0x4004 — der SPAWN-REMAP in re2z_init (`sh 0x4002,270` @0x80100A34-38
     *       bzw. `sh 0x4004,270` @0x80100A88-8C)
     * Im Original gehoert zu jedem 0x4000-Spawn ein WECKER: der einzige Overlay-Clear steht in
     * EXEC[15] (`andi v1,v1,0xbfff` / `sh v1,270(s0)` @0x80104F0C-10), angestossen vom RE2-Raum-
     * skript. RE1.5-Raeume haben diese Records NICHT — Byte-Zensus ueber die ausgelieferten RDTs
     * (Muster 0x34,0x0C,0x89|0x8A): ROOM1100/1101 je 5, ROOM1070 5, ROOM1020 4, **ROOM1140 = 0**.
     * Der Port muss den Clear deshalb MAPPEN (re2z_exec_lying / re2z_exec_feeding, Naehe-Gate) —
     * und genau dieses Port-Mapping ist es, das die Treffbarkeit an eine Distanz haengt. Diesen
     * Zustand — "aus der Ferne unverwundbar, aus der Naehe verwundbar" — gibt es in KEINEM der
     * beiden Originale.
     *
     * DIE SOLLSEITE IST RE1.5 (ROOM1140 ist ein RE1.5-Raum, und der Nutzer trifft den Fresser
     * dort im Original). RE1.5s Kandidaten-Sammelschleife FUN_80011F50 gatet PRO AKTOR
     * AUSSCHLIESSLICH auf "aktiv" — selbst disassembliert, info/Re1.5/PSX.EXE:
     *   80011ffc: lw    v0,0(s0)          ; Entity-Flagwort
     *   80012000: nop
     *   80012004: andi  v0,v0,0x1
     *   80012008: beq   v0,zero,0x80012024 ; NUR inaktiv wird uebersprungen
     *   80012034: bne   v0,zero,0x80011ffc ; naechster Kandidat
     *   80012038: addiu s0,s0,500          ; Stride 0x1F4 = RE1.5-Entity
     * Es gibt dort WEDER ein +0x1D3- noch ein +0x10E-Gegenstueck: eine liegende oder fressende
     * RE1.5-Leiche ist ein ganz normaler Kandidat. (Der +0x93-Riegel ist ein EIN-TREFFER-Latch im
     * Auflöser, keine dauerhafte Pose-Unverwundbarkeit.)
     *
     * ⚠️ AUSDRUECKLICH GEWUENSCHTE ABWEICHUNG von RE2 (Nutzer-Entscheidung, wie 83b7740c):
     * solange der Zombie die port-synthetisierte SPAWN-POSE haelt (EXEC[7] liegend / EXEC[8]
     * fressend), zaehlen die Sperren (2) und (4) NICHT. Der Geltungsbereich ist damit exakt der
     * Ort, an dem der Port den Zustand selbst erzeugt hat. UEBERALL SONST bleibt der Filter
     * unveraendert scharf — dort sitzt der echte Nutzen: der walking-immortal-Fall aus EXEC[11]
     * (siehe der Block ueber re2z_exec_eleven) wird weiterhin von Gate (2) gefangen, sobald ein
     * Latch stehen bleibt, und Leichen (Gate 3) bleiben unbeschiessbar.
     * ========================================================================================== */
    re15_re2z_hit_filter_apply(slot);
    return 1;
}

/* ⛔ DER FILTER GEHOERT IN DEN AUFLOESER, NICHT IN DEN KI-TICK — ER MUSS AUCH LAUFEN,
 *    WENN DER KI-TICK UEBERSPRUNGEN WIRD.
 * -------------------------------------------------------------------------------------------
 * Im Original steht der Vier-Gate-Filter INNERHALB der Kandidatenschleife des Angriffs-
 * Aufloesers FUN_800470C0 (@0x80047124-30 / 38-40 / 48-50 / 58-64) — er wird also im Moment des
 * Schusses ausgewertet und haengt an KEINEM Gegner-Tick. Der Port bildet ihn ueber den Latch
 * +0x93 Bit 0 ab, den der geteilte RE1.5-Aufloeser liest; dieser Latch wurde bisher NUR am Ende
 * von re15_re2z_tick geschrieben. Beide Vor-Gates des Roots koennen den Tick aber komplett
 * ueberspringen:
 *     8010042c-3c  `lw g_pauseflags / lui 0x2000 / and / bne -> Funktionsende`  (globaler Freeze)
 *     80100450-5c  `lbu +0x9 / andi 0x20 / bne -> Funktionsende`                (Per-Entity-Skip)
 * Ein uebersprungener Tick friert damit den Latch ein: der erste Treffer setzt ihn
 * (`ori v0,v0,0x1` @0x800124f0), und niemand gibt ihn je wieder frei — der Gegner ist nach
 * EINEM Treffer dauerhaft unverwundbar. Das ist derselbe Defekt-Typ wie 2026-08-19 (eb841053),
 * nur eine Ebene hoeher.
 * GEMESSEN (probe_re2_aim4 --part=8, Zensus ueber alle 37 STAGE1-RDTs, 99 Spawns):
 *     ROOM11C0 slot02/03 typ=0x27 grid=0x30      (Gorilla-Boss, eigenes Brain)
 *     ROOM1200 slot03    typ=0x10 grid=0xA1      <-- ein RE2-Zombie mit +0x9 & 0x20
 * Der ROOM1200-Zombie tickt nie und war damit nach dem ersten Treffer nicht mehr treffbar.
 * Deshalb ist die Auswertung jetzt eine eigene, idempotente Funktion, die game_step
 * ZUSAETZLICH fuer jeden RE2-eigenen Zombie aufruft — auch fuer die, deren Tick ausfaellt.
 * Fuer alle anderen aendert sich nichts: dieselbe Rechnung, derselbe Zeitpunkt im Frame. */
void re15_re2z_hit_filter_apply(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];
    {   int spawn_pose = (e->state == 1)
                      && (e->sub_state_1 == 7 || e->sub_state_1 == 8);  /* EXEC[7]/EXEC[8] */
        /* ⛔ UND DER LATCH MUSS MIT DER POSE FALLEN, EGAL WIE SIE ENDET.
         * GEMESSEN, nachdem die Ausnahme oben griff (probe_re2z_abc, 64 Seeds, Pistole):
         *   [C-ANDERE] seed 4 slot 3 f257: st=1 s1=1 s2=1 hp=49 **1D3=80 10E=4004** clip=2
         * Also ein LAUFENDER Zombie, der den Fresser-Spawnzustand noch traegt: er wurde in der
         * Pose getroffen (das ist jetzt erlaubt), der HURT-Handler hat ihn in den WALK entlassen,
         * und die beiden port-synthetisierten Latches blieben stehen — dieselbe Unsterblichkeit
         * eine Ebene weiter. Im Original kann dieser Zustand nicht entstehen (dort ist die Pose
         * unbeschiessbar), und in RE1.5 gibt es die Latches ueberhaupt nicht.
         * Die Pose-Ausgaenge des Originals raeumen GENAU DIESE ZWEI Bytes auf:
         *   EXEC[7] P4 : `andi 0x7f` @0x80103914-18
         *   EXEC[8] P3 : `andi 0x7f` @0x80103CE4-FC     P5: @0x80103D98
         *   Wecker     : `andi v1,v1,0xbfff` / `sh v1,270(s0)` @0x80104F0C-10
         * Der Port zieht sie deshalb einmalig nach, sobald die Pose verlassen ist. Das Bit 0x4000
         * identifiziert den Zustand eindeutig: gesetzt wird es nur vom Spawn-Remap (@0x80100A34-38
         * / @0x80100A88-8C) und von EXEC[6] P2 (@0x80103A4C-50), und EXEC[6] P2 committet im
         * selben Tick 0x801, ist also im naechsten Tick wieder `spawn_pose`. */
        if (!spawn_pose && (e->re2z_f10e & 0x4000u)) {
            e->re2z_f10e    &= (uint16_t)~0x4000u;                 /* andi 0xbfff @0x80104F0C */
            e->re2z_self1d3 &= 0x7Fu;                              /* andi 0x7f   @0x80103914 /
                                                                    * @0x80103CE4-FC */
        }
        int hittable = (e->active != 0)                            /* (1) +0x0 & 1   @0x80047124-30 */
                    && (spawn_pose || e->re2z_self1d3 == 0u)       /* (2) +0x1D3     @0x80047138-40 */
                    && (e->hp >= 0)                                /* (3) HP < 0     @0x80047148-50 */
                    && (spawn_pose || !(e->re2z_f10e & 0xC000u));  /* (4) +0x10E     @0x80047158-64 */
        if (hittable) e->hit_react &= (uint8_t)~1u;                /* Kandidat freigegeben */
        else          e->hit_react |= (uint8_t)1u;                 /* Kandidat UEBERSPRUNGEN */
        e->hit_react &= (uint8_t)~2u;                              /* RE2 kennt +0x93 nicht ->
                                                                    * kein RE1.5-Wurzel-Gore */
    }
}
