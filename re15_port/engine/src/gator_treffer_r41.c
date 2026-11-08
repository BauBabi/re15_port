/*
 * gator_treffer_r41.c - Runde 41 Spur G2-RE2 NB4 (T)+(W): die Trefferreaktion der Figur auf JEDEN Alligator-Angriff wie RE2
 * (Biss im Wasser, CROSS-Biss, Rammen, Hochbiss, Biss mit Kanister) statt des RE1.5-Niederwurfs, und der Stoss von der Rampe ins Wasser.
 * Kopf: include/re15_gator_treffer_r41.h. Messblatt analysis/befunde_runde41/G2_MESSBLATT_NB4.md, Dossier G2_gatorfix.md "G2-RE2 NB4".
 *
 * (T) Orchestrator-Entscheid nach "Original oder nicht" (Memory reai-v2-original-oder-nicht; beim Alligator gilt RE2 ganz): Abnahme 6 F2 mass, dass
 * ein Biss im Wasser die Figur mit dem RE1.5-Niederwurf (s_kd_speed 1000 @0x8003615c) bis gut 13000 Einheiten weit warf. RE2 hat dafuer EINEN
 * Treffer-Einstieg und eine eigene Spieler-Routine, beide selbst disassembliert (info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN, Basis 0x80100000,
 * Auszug analysis/befunde_runde41/G2_belege/nb4/mess_re2_em23_treffer_kanister.dis):
 *
 *  TREFFER-EINSTIEG 0x80101578 (je Bild aus dem Root: `jal 0x80101388` @0x80100328 -> `jal 0x80101578` @0x8010155c):
 *    DAT_800cfc06 != 0 (`lhu v0,0(a1) / beq v0,zero` @0x80101590-98) - im Port prueft jede Angriffsstelle den Kontakt selbst (Messblatt NB3 Abschn. 3);
 *    PL+0x1D3 == 0 (`lbu v0,-565(v0) / bne` @0x801015a4-ac) - im Port pl->hit_react == 0 an allen Angriffsstellen (re15_gator_treffer_r41_tick
 *      spiegelt +0x1D3 & 0x80 in hit_react Bit 0);
 *    PL+0x1B4 = Gator (`sw s1,-596(at)` @0x801015b8-bc), PL+4 = 0x205 (`addiu v0,zero,517` @0x801015b4, `sw v0,-1028(at)` @0x801015c0-c4)
 *      = Routine 5, Unterzustand 2, Phase 0; FUN_80015910(Gator, Figur) wahr -> Unterzustand 3 (`jal 0x80015910` @0x801015c8, +5 += 1 @0x801015d8-ec);
 *      FUN_80015910 = ((pl.gier - gator.gier) + 0x400) & 0xfff < 0x800 (RE2_Quellcode_V2/FUN_80015910.c);
 *    Achse a1 in {0, 1024, 2048, 3072} nach der Gator-Lage im RE2-Korridor (@0x801015f0-80), wahr -> a, falsch -> a + 2048, dann
 *      FUN_8001569c(Figur, a1, 1536) (`jal 0x8001569c / addiu a2,zero,1536` @0x80101684-88; RE2_Quellcode_V2/FUN_8001569c.c);
 *    Schaden `lhu v0,536(s1) / andi 0x20 / addiu a0,zero,60 / addiu a0,zero,30` @0x8010168c-a0 -> 0x801038e4 -> FUN_800401d4(a0, 0)
 *      (`jal 0x800401d4 / addu a1,zero,zero` @0x80103918-1c); Rueckgabe Bit 0 -> PL+6 = 4 (`sb v0,-1026(at)` @0x80103924-34), Bit 1 -> PL+4 = 3
 *      (`sw v0,-1028(at)` @0x80103938-48); Gator: +0x238 = 70 (@0x801016b0-b4, Port re15_gr2_biss_cd beim Aufrufer).
 *    PORT-WAHL (Messblatt O-T1, Messvorschlag A90): ROOM2090 ist kein Korridor - die Achse a ist die Gator-Gier, auf die naechste der vier
 *      RE2-Achsen 0/1024/2048/3072 gerastet (die RE2-Wertemenge der Achse bleibt; RE2 nimmt die Achse des Abschnitts, in dem der Gator schwimmt).
 *
 *  SPIELER-HAKEN 0x80102D08 (INIT traegt ihn ein: `lui v0,0x8010 / addiu v0,v0,11528 / sw v0,-7284(at)` @0x801004f4-500), Tafel @0x80104690
 *  nach PL+5: [2] 0x8010307C, [3] 0x801031F8.
 *    Unterzustand 2 (Tafel @0x80100074): Ph0 SE 0x04010001 (@0x801030bc-c8), Clip-Wort 0x00030012 = Clip 18 Blend 3 (@0x801030cc-d4),
 *      +0x144 = -800 (@0x801030e0-e4), +0x146/+0x148 = 0 (@0x801030f0-f4), +0x1D3 |= 0x80 (@0x801030e8/f8/100), faellt in Ph1;
 *      Ph1 Blend +0x14E == 0 -> Ph 2 (@0x80103108-1c), FUN_800152c8(pl, 0) (@0x80103120), FUN_8002959c(pl, +0x190, +0x194, 1024)
 *      (@0x8010312c-38), +0x144 += 150 (@0x8010313c-4c); Ph2 Clip-Wort 0x00030013 = Clip 19 Blend 3 (@0x80103150-58), Ph 3 (@0x8010315c-60),
 *      faellt in Ph3: FUN_800152c8 (@0x80103168), FUN_8002959c doppelt (@0x8010317c / @0x80103194), Ph += Rueckgaben (@0x801031a0-bc),
 *      +0x144 += 15, > 0 -> 0 (@0x801031a4-c4); Ph4 Ende: PL+4 = 1 (`sw v0,4(s1)` @0x801031d0), +0x1D3 = 0 (@0x801031d4), +0x1C0 &= 0xED.
 *    Unterzustand 3 (Tafel @0x8010008c, [3] = Ende ohne Wirkung, [4] = [2]): Ph0 SE 0x04020001 (@0x80103238-44), Clip 20 Blend 3 (0x00030014
 *      @0x80103248-50), +0x144 = +800 (@0x8010325c-60), +0x1D3 |= 0x80 (@0x80103264-7c), faellt in Ph1: FUN_800152c8 (@0x80103288),
 *      FUN_8002959c einfach (@0x8010329c), Ph += Rueckgabe (@0x801032a4-b0), +0x144 -= 120, < 0 -> 0 (@0x801032b4-d8);
 *      Ph2/Ph4 Clip 17 Blend 3 (0x00030011 @0x801032dc-e4), Ph 5 (@0x801032e8-ec), +0x1D3 = 0 (@0x801032f0), faellt in Ph5: FUN_8002959c doppelt
 *      (@0x80103300 / @0x80103318), Ende -> Ph 6 + Clip-Wort 0x00030006 (@0x80103324-3c); Ph6 FUN_8002959c(pl, +0x108, +0x17C, 0x10400) =
 *      Basisbank rueckwaerts (@0x80103340-54), Ende -> PL+4 = 1, +0x1D3 = 0, +0x1C0 &= 0xED (@0x80103358-70).
 *    FUN_800152c8 (RE2_Quellcode_V2/FUN_800152c8.c): Lage += RotY(Gier) * (+0x144, 0, +0x148) - ohne Wand-Sonde; die Klemme kommt danach
 *      (EXE FUN_8003567c @0x80026634; Port: Dispatcher-Schwanz re15_player_body_and_walls -> spieler_klemme).
 *    FUN_8002959c -> FUN_80029614: Blend +0x14E -1 je Aufruf solange != 0 (`lbu v0,334(s2) / addiu -1 / sb` @0x800299c0-cc), Bild +1, Wrap und
 *      Rueckgabe 1 bei Bild + 1 >= Laenge (`lbu v0,333(s2) / addiu 1 / sb / sltu v0,v0,s3` @0x80029b28-4c); Gewichtsschritt a3 = 1024 (Port
 *      anim_blend_rate, skeleton_common.c wp = anim_frac x rate), a3 & 0x10000 = rueckwaerts (Port anim_flags 0x80 wie kd_clip).
 *    Bild fuer Bild (G2_belege/nb4/mess_re2_modell.txt): Sub 2 Schritte -800,-650,-500,-350,-200,-185..-5 = 3735 rueckwaerts in 20 Bildern,
 *      gesperrt Bild 0-19; Sub 3 +800,680,...,80 = 3080 vorwaerts, 54 Bilder, gesperrt Bild 0-20.
 *  RAUMBANK: PL+0x190/+0x194 schreibt nur FUN_80018e08 aus RDT+0x60 (`sw v0,0x0(t5)` @0x80018ea4 / `sw v0,0x4(t5)` @0x80018eb8, ghidra_re2_Leon).
 *    ROOM40A0.RDT @0x65E8 = shared_assets/RE2/GATOR40A0.RBJ (bytegleich, tools/r41_g2nb4/gator40a0_rbj_export.py); Record 0 (Leon) EDD @0x29A4
 *    (= ROOM40A0.RDT @0x8F8C): Clip 17/18/19/20 = 16/10/30/21 Bilder. Record 1 (Claire) hat nur 16 Clips - Elza posiert aus Record 0 (Messblatt O-T2,
 *    EMR-Kopf identisch: 15 Knochen x 80 B). ROOM2090/2091 haben kein RBJ (RDT+0x5C = 0, mess_rbj40a0.txt) - die Bank ist ein eigener Kanal wie
 *    RE2s +0x190/+0x194 neben der Basisbank +0x108/+0x17C, der Raum-Standardkanal der Figur bleibt unberuehrt.
 *    Basis-Clip 6 (RE2 PL00.PLD, 25 Bilder) = RE1.5 PL00 Clip 11 Keyframe fuer Keyframe (25 von 25, mess_pld_vergleich.txt).
 *  PORT-ZUORDNUNG des Kommandoworts: RE2 Routine 5 Sub 2/3 laeuft hier als eigener Zweig (game_step_common.c); pl->state = 2 (RE1.5 cmd 2 =
 *    Treffer-Klasse, re15_player_take_damage @0x80012ebc), sub_state_1 = 2/3 (= RE2 +5), Ende pl->state = 1 / 0 / 0 (RE2 Wort-Store `sw 1,4(s1)`).
 *    RE1.5 cmd 5 ist der Griff - darum NICHT pl->state = 5.
 *  Nicht Teil dieser Reaktion (Messblatt O-T3): Vibration 0x800eaad8/0x800eac28 (@0x801038f8/@0x8010390c), Gator-Effekt FUN_80103d80; Nacken-Bits
 *    +0x1C0 |= 0x12 (der Port fuehrt das Mitschauen nur im Normal-Zweig, der waehrend dieser Reaktion nicht laeuft).
 *
 * (W) Nutzer 2026-11-07 ~15:00 woertlich: "er kann den Spieler ruhig In das Wasser stoßen." RE2s Alligator-Raum hat keine erhoehte Flaeche
 * (Messblatt NB3 Abschnitt 3) - RE2 kennt den Sturz hier nicht. Der Port nimmt die Fallschritte des RE1.5-Herunterspringens (Substate 10
 * Ph2-Ph4, climb_common.c re15_climb_fall_*): fuehrt ein RE2-Schritt die Figur auf Band 1 in eine Zelle mit Abwaerts-Bit 0x2 (ROOM2090.RDT
 * [8] Nordleiste @0x754 / [9] Suedleiste @0x760, u1 0x02; Bedingung wie die Abwaerts-Sonde `if (!(s6 & 2u))` @0x8002d74c-50 und Boden darunter
 * tiefer @0x8002d764-7c), dann in DIESEM Bild y += 500, Boden aus room_coll, Band = -(h / 1800) VOR der Klemme (Ph2 @0x8003843c-0x80038638),
 * im selben Bild y += 90 + 50 t (Ph3 faellt durch @0x8003863c), weiter je Bild bis Boden - 800 < y, im Bild danach y = Boden + Spritzer
 * (Ph4 @0x80038724-2c / @0x80038758-cc). PORT-WAHL (Messblatt O-W1): die RE2-Haltung (Clip 18/19 bzw. 20/17) und die waagrechten RE2-Schritte
 * laufen waehrend des Falls weiter - keine RE1.5-Fallclips, keine Mischung ohne Beleg. Ostwand [6] und Plattform [7] (u1 0x00) bleiben Wand.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_gator_treffer_r41.h"
#include "re15_audio.h"          /* re15_audio_core_se */
#include "re15_climb.h"          /* re15_climb_fall_* (NB4 W) */
#include "re15_collision.h"
#include "re15_engine.h"         /* g_engine.frame_count */
#include "re15_skeleton.h"       /* re15_sin_q12 / re15_cos_q12 */

/* GATOR40A0.RBJ Record-0-EDD @0x29A4 (= ROOM40A0.RDT @0x8F8C, mess_rbj40a0.txt): Bildzahlen der vier Reaktions-Clips */
#define GTR_CLIP_LIEGT   17      /* 16 Bilder */
#define GTR_CLIP_STOSS2  18      /* 10 Bilder */
#define GTR_CLIP_TAUMEL  19      /* 30 Bilder */
#define GTR_CLIP_STOSS3  20      /* 21 Bilder */
static const int k_fc_edd[4] = { 16, 10, 30, 21 };   /* Clip 17..20, Datei-Bytes EDD @0x29A4 + 4 * Clip */
#define GTR_BASIS_CLIP   11      /* RE2 Basis-Clip 6 = RE1.5 PL00 Clip 11 (mess_pld_vergleich.txt) */
#define GTR_BASIS_FC     25      /* RE2 PL00.PLD Basis-Clip 6: 25 Bilder (Rueckfall, wenn die Basisbank fehlt) */

extern int re15_re2_player_damage(re15_actor_t *pl, int dmg);   /* enemy_ai_re2_zombie.c = FUN_800401d4 (One-Save-Latch geteilt) */

static int32_t sqrt_i(int64_t v)
{
    if (v <= 0) return 0;
    int64_t r = 0, b = (int64_t)1 << 62;
    while (b > v) b >>= 2;
    while (b) { if (v >= r + b) { v -= r + b; r = (r >> 1) + b; } else r >>= 1; b >>= 2; }
    return (int32_t)r;
}

static int s_alt = 0;
void re15_gator_treffer_alt_setzen(int an) { s_alt = an; }
int  re15_gator_treffer_alt(void) { return s_alt; }

const uint8_t *(*re15_gator_treffer_r41_rbj_hook)(int *size) = NULL;

static struct {
    int      an, sub, ph, bild;
    int      clip, f, blend, pose_f, raumbank;
    int16_t  spd;                  /* +0x144 */
    int      sperre, schritt;
    int      fall, fall_t, ende_wartet;
    int16_t  boden;
    int      n_start, n_ende, n_fall, n_landung;
    int      art, achse, schaden, ret, se;
    int32_t  start_x, start_z;
    int      gier_figur_vor, gier_gator;
    int32_t  weg;
    int      hat_pose;             /* in dieser Reaktion schon eine Pose (FUN_8002959c) */
    int      zeig_clip, zeig_raum, zeig_blend;   /* zuletzt posiert */
    int      pose_latch;           /* Clip der Raumbank, den der Zeichner noch zeigt (+0x94 unveraendert) */
    uint16_t rate_vorher;
} s;

static int s_bank_ok = -1;
static re15_emd_skeleton_t  s_bank_skel;
static re15_emd_animation_t s_bank_anim;
static const re15_emd_skeleton_t *s_bank_basis = NULL;

static void gtr_log(const char *fmt, ...)
#if defined(__GNUC__)
    __attribute__((format(printf, 1, 2)))
#endif
    ;
#include <stdarg.h>
static void gtr_log(const char *fmt, ...)
{
#ifdef RE15_PLATFORM_PC
    static FILE *f = NULL; static int stumm = -1;
    if (stumm < 0) stumm = (getenv("RE15_GB_STUMM") != NULL);
    if (stumm) return;
    if (!f) f = fopen("gator_boss.log", "a");
    if (!f) return;
    va_list ap; va_start(ap, fmt);
    fputs("GTR ", f); vfprintf(f, fmt, ap); fputc('\n', f);
    va_end(ap); fflush(f);
#else
    (void)fmt;
#endif
}

int re15_gator_treffer_r41_bank(const re15_emd_skeleton_t *basis, const re15_emd_skeleton_t **skel, const re15_emd_animation_t **anim)
{
    if (!basis) return 0;
    if (s_bank_ok < 0 || s_bank_basis != basis) {
        s_bank_ok = 0; s_bank_basis = basis;
        int n = 0;
        const uint8_t *b = re15_gator_treffer_r41_rbj_hook ? re15_gator_treffer_r41_rbj_hook(&n) : NULL;
        if (b && n > 0 && re15_emd_parse_rbj_record(b, (size_t)n, 0, basis, &s_bank_skel, &s_bank_anim) == 0 &&
            s_bank_anim.clip_count > GTR_CLIP_STOSS3)
            s_bank_ok = 1;
    }
    if (!s_bank_ok) return 0;
    if (skel) *skel = &s_bank_skel;
    if (anim) *anim = &s_bank_anim;
    return 1;
}

static int fc_raum(int clip)
{
    if (s_bank_ok == 1 && clip < s_bank_anim.clip_count && s_bank_anim.clips[clip].frame_count > 0) return s_bank_anim.clips[clip].frame_count;
    return k_fc_edd[clip - GTR_CLIP_LIEGT];
}

/* FUN_8001569c (RE2_Quellcode_V2/FUN_8001569c.c; Port-Zwilling enemy_ai_boss_g5.c g5_yaw_toward) */
static void gier_auf(re15_actor_t *pl, int32_t ziel, int32_t schritt)
{
    uint32_t d; int16_t zurueck;
    if ((int16_t)schritt < 0) { schritt = -(int16_t)schritt; ziel = (ziel + 0x800) & 0xFFF; }
    d = (uint32_t)((schritt + (ziel - (int32_t)(uint16_t)pl->rot_y)) & 0xFFF);
    zurueck = (int16_t)(pl->rot_y - (int16_t)schritt);
    if ((int32_t)d < schritt * 2) { pl->rot_y = (int16_t)ziel; return; }
    pl->rot_y = zurueck;
    if (d < 0x801u) pl->rot_y = (int16_t)(zurueck + (int16_t)schritt * 2);
}
/* FUN_80015910: die Figur blickt wie der Gator (der Gator steht hinter ihr) */
static int gleiche_blickrichtung(const re15_actor_t *gator, const re15_actor_t *pl)
{
    return ((((int)pl->rot_y - (int)gator->rot_y) + 0x400) & 0xfff) < 0x800;
}

int re15_gator_treffer_r41_einstieg(const re15_actor_t *gator, re15_actor_t *pl, int geschwaecht, int art)
{
    const int hinten = gleiche_blickrichtung(gator, pl);                          /* @0x801015c8-ec */
    const int a = ((((int)gator->rot_y & 0xfff) + 512) & 0xC00) & 0xfff;           /* PORT-WAHL A90 (Messblatt O-T1) */
    const int achse = hinten ? a : ((a + 2048) & 0xfff);                          /* @0x80101604-80: wahr -> a, falsch -> a + 2048 */
    const int dmg = geschwaecht ? 30 : 60;                                         /* @0x8010168c-a0 */
    const int16_t gier_vor = pl->rot_y;
    const int ret = re15_re2_player_damage(pl, dmg);                               /* FUN_800401d4(dmg, 0) @0x80103918 */
    if (ret & 2) {                                                                 /* PL+4 = 3 @0x80103938-48: Tod -> Fressen (Aufrufer) */
        pl->state = 3; pl->sub_state_1 = 0; pl->sub_state_2 = 0;                   /* Wort-Store +4 = 3, +5/+6 = 0 */
        gtr_log("F%u TOT art %d schaden %d hp %d", (unsigned)g_engine.frame_count, art, dmg, (int)pl->hp);
        return ret;
    }
    gier_auf(pl, achse, 1536);                                                     /* FUN_8001569c(pl, a1, 1536) @0x80101684-88 */
    {   extern void re15_player_aim_interrupt(void);   /* der Routinenwechsel ersetzt den Ziel-Zustand (wie cmd 2 @0x80031c88) */
        re15_player_aim_interrupt(); }
    const int war_an = s.an;
    const int fall = s.an ? s.fall : 0, fall_t = s.fall_t; const int16_t boden = s.boden;   /* ein laufender Sturz faellt weiter */
    const uint16_t rate = s.an ? s.rate_vorher : pl->anim_blend_rate;
    s.an = 1; s.sub = hinten ? 3 : 2; s.ph = 0; s.bild = 0;                        /* PL+4 = 0x205 (+1) @0x801015b4-ec */
    if (ret & 1) s.ph = 4;                                                         /* One-Save: PL+6 = 4 @0x80103924-34 */
    s.fall = fall; s.fall_t = fall_t; s.boden = boden; s.ende_wartet = 0;
    if (!war_an) s.hat_pose = 0;                                                   /* ein Neustart behaelt die gezeichnete Pose bis zum ersten Vorschub */
    s.rate_vorher = rate;
    s.n_start++; s.art = art; s.achse = achse; s.schaden = dmg; s.ret = ret; s.se = 0;
    s.start_x = pl->x; s.start_z = pl->z; s.weg = 0; s.schritt = 0;
    s.gier_figur_vor = (int)gier_vor & 0xfff; s.gier_gator = (int)gator->rot_y & 0xfff;
    pl->state = 2; pl->sub_state_1 = (uint8_t)s.sub; pl->sub_state_2 = (uint8_t)s.ph;
    gtr_log("F%u START art %d sub %d achse %d gier %d->%d schaden %d ret %d hp %d gator (%d,%d,%d) gier %d figur (%d,%d,%d) band %d",
            (unsigned)g_engine.frame_count, art, s.sub, achse, (int)gier_vor & 0xfff, (int)pl->rot_y & 0xfff, dmg, ret, (int)pl->hp,
            (int)gator->x, (int)gator->y, (int)gator->z, (int)gator->rot_y & 0xfff, (int)pl->x, (int)pl->y, (int)pl->z, re15_collision_debug_band());
    return ret;
}

int re15_gator_treffer_r41_aktiv(void) { return s.an; }

void re15_gator_treffer_r41_reset(void)
{
    const int n0 = s.n_start, n1 = s.n_ende, n2 = s.n_fall, n3 = s.n_landung;
    memset(&s, 0, sizeof s);
    s.n_start = n0; s.n_ende = n1; s.n_fall = n2; s.n_landung = n3;
}

/* FUN_8002959c -> FUN_80029614 (Blend -1 @0x800299c0-cc, Bild +1 / Wrap / Rueckgabe @0x80029b28-4c); pose_f = das posierte Bild */
static int adv(int fc)
{
    if (s.blend > 0) s.blend--;
    s.pose_f = s.f; s.zeig_clip = s.clip; s.zeig_raum = s.raumbank; s.zeig_blend = s.blend; s.hat_pose = 1;   /* FUN_80029614 posiert (Knochen) */
    s.f++;
    if (s.f >= fc) { s.f = 0; return 1; }
    return 0;
}
/* Clip-Wort `sw (3 << 16) | clip, 332(s1)`: +0x14C Clip, +0x14D Bild 0, +0x14E Blend 3. Gezeichnet wird erst, was der naechste FUN_8002959c posiert
 * (Sub 3 Ph5 -> Ph6 schreibt das Wort ohne Vorschub @0x80103328-3c: in diesem Bild steht noch die letzte Pose von Clip 17). */
static void clip_wort(int clip, int raumbank) { s.clip = clip; s.f = 0; s.blend = 3; s.raumbank = raumbank; }

/* FUN_800152c8(pl, 0): Lage += RotY(Gier) * (+0x144, 0, 0) - ApplyMatrixSV, Ergebnis >> 12 (wie re15_player_knockback_delta) */
static void vorschub(re15_actor_t *pl)
{
    const int a = (int)pl->rot_y & 0xfff;
    const int32_t dx = (int32_t)((re15_cos_q12(a) * (int32_t)s.spd) >> 12);
    const int32_t dz = (int32_t)((-re15_sin_q12(a) * (int32_t)s.spd) >> 12);
    pl->x += dx; pl->z += dz;
    s.schritt = s.spd; s.weg += s.spd < 0 ? -s.spd : s.spd;
}

static void ende(re15_actor_t *pl, int32_t von_x, int32_t von_z)
{
    s.an = 0; s.sperre = 0; s.n_ende++;
    pl->state = 1; pl->sub_state_1 = 0; pl->sub_state_2 = 0;                       /* `sw v0(1),4(s1)` @0x801031d0 / @0x80103364 */
    pl->hit_react &= (uint8_t)~1u;                                                  /* +0x1D3 = 0 @0x801031d4 / @0x80103368 */
    pl->anim_blend_rate = s.rate_vorher;
    if (s.hat_pose && !s.zeig_raum && (pl->anim_flags & 0x80u)) {   /* Endpose = Bild 0 vorwaerts, Reverse-Bit faellt (wie kd_ende) */
        pl->anim_frame = 0; pl->anim_flags &= (uint16_t)~0x80u;
    }
    s.pose_latch = (s.hat_pose && s.zeig_raum) ? s.zeig_clip : -1;
    gtr_log("F%u ENDE sub %d bilder %d weg %d von (%d,%d) luft %d figur (%d,%d,%d) band %d fall %d", (unsigned)g_engine.frame_count, s.sub, s.bild + 1, (int)s.weg, (int)von_x, (int)von_z,
            (int)sqrt_i((int64_t)(pl->x - s.start_x) * (pl->x - s.start_x) + (int64_t)(pl->z - s.start_z) * (pl->z - s.start_z)),
            (int)pl->x, (int)pl->y, (int)pl->z, re15_collision_debug_band(), s.fall);
}

/* (W): Sturz von der Rampe - Fallschritte des Substate 10 (climb_common.c re15_climb_fall_*) */
static void kante_pruefen(const re15_rdt_t *rdt, re15_actor_t *pl)
{
    if (s.fall != 0 || !rdt || s.schritt == 0) return;
    const int band = re15_collision_debug_band();
    if (band != 1) return;
    const uint16_t w = re15_collision_floor_typeword(rdt, pl->x, pl->z, band, 0);      /* FUN_8003b7f0, Bit 0x2 wie @0x8002d74c-50 */
    if (!(w & 2u)) return;
    const int16_t h = re15_collision_room_coll(rdt, pl->x, pl->z, 1, band, 0x100);     /* Boden darunter tiefer wie @0x8002d764-7c */
    if (!(pl->y < (int32_t)h)) return;
    const int32_t y0 = pl->y;
    re15_climb_fall_beginn(rdt, pl, &s.boden);                                          /* Ph2 @0x8003843c-0x80038638 */
    s.fall = 1; s.fall_t = 0; s.n_fall++;
    gtr_log("F%u KANTE band %d -> %d y %d -> %d boden %d figur (%d,%d) sub %d bild %d", (unsigned)g_engine.frame_count, band,
            re15_collision_debug_band(), (int)y0, (int)pl->y, (int)s.boden, (int)pl->x, (int)pl->z, s.sub, s.bild);
    if (re15_climb_fall_schritt(pl, &s.fall_t, s.boden)) s.fall = 3;                    /* Ph3 im selben Bild (Durchfall @0x8003863c) */
}
static void fall_takt(re15_actor_t *pl)
{
    if (s.fall == 3) {                                                                  /* Ph4 im Bild nach der Schwelle */
        re15_climb_fall_landung(pl, s.boden, 0);                                        /* +0x93 regelt die RE2-Sperre */
        s.fall = 2; s.n_landung++;
        gtr_log("F%u LANDUNG y %d figur (%d,%d) band %d", (unsigned)g_engine.frame_count, (int)pl->y, (int)pl->x, (int)pl->z,
                re15_collision_debug_band());
    } else if (s.fall == 1) {
        if (re15_climb_fall_schritt(pl, &s.fall_t, s.boden)) s.fall = 3;                /* Ph3 @0x80038660-9c / @0x800386ac-c8 */
    }
}

void re15_gator_treffer_r41_tick(const re15_rdt_t *rdt, re15_actor_t *pl, const re15_emd_animation_t *pl00)
{
    if (!s.an || !pl) return;
    const int fall_vorher = s.fall;
    const int32_t von_x = pl->x, von_z = pl->z;   /* Lage am Takt-Anfang (Messzeile: Weg des Hakens) */
    s.schritt = 0;
    int fertig = 0;
    if (s.ende_wartet) fertig = 1;                     /* Reaktion vorbei, der Sturz laeuft noch zu Ende */
    else if (s.sub == 2) {
        switch (s.ph) {
        case 0:                                                                     /* 0x801030BC */
            re15_audio_core_se(1); s.se = 1;                                        /* Se 0x04010001 @0x801030bc-c8 */
            clip_wort(GTR_CLIP_STOSS2, 1);                                          /* 0x00030012 @0x801030cc-d4 */
            s.ph = 1; s.spd = -800; s.sperre = 1;                                   /* @0x801030d8-dc, @0x801030e0-e4, @0x801030e8-100 */
            pl->anim_blend_rate = 1024;                                             /* a3 = 1024 @0x80103138 */
            /* faellt in Ph1 @0x80103108 */
        case 1:
            if (s.blend == 0) s.ph = 2;                                             /* @0x80103108-1c */
            vorschub(pl);                                                           /* @0x80103120 */
            (void)adv(fc_raum(s.clip));                                             /* @0x80103134 */
            s.spd = (int16_t)(s.spd + 150);                                         /* @0x8010313c-4c */
            break;
        case 2:                                                                     /* 0x80103150 */
            clip_wort(GTR_CLIP_TAUMEL, 1); s.ph = 3;                                /* 0x00030013 @0x80103150-60 */
            /* faellt in Ph3 @0x80103164 */
        case 3: {
            vorschub(pl);                                                           /* @0x80103168 */
            int d = adv(fc_raum(s.clip)); d += adv(fc_raum(s.clip));                /* @0x8010317c / @0x80103194 */
            s.ph += d;                                                              /* @0x801031a0-bc */
            s.spd = (int16_t)(s.spd + 15); if (s.spd > 0) s.spd = 0;                /* @0x801031a4-c4 */
            break; }
        case 4:                                                                     /* 0x801031C8 */
            fertig = 1; break;
        default: break;                                                             /* `sltiu v0,v1,0x5` @0x80103098 */
        }
    } else {
        switch (s.ph) {
        case 0:                                                                     /* 0x80103238 */
            re15_audio_core_se(2); s.se = 2;                                        /* Se 0x04020001 @0x80103238-44 */
            clip_wort(GTR_CLIP_STOSS3, 1);                                          /* 0x00030014 @0x80103248-50 */
            s.ph = 1; s.spd = 800; s.sperre = 1;                                    /* @0x80103254-58, @0x8010325c-60, @0x80103264-7c */
            pl->anim_blend_rate = 1024;
            /* faellt in Ph1 @0x80103284 */
        case 1: {
            vorschub(pl);                                                           /* @0x80103288 */
            const int d = adv(fc_raum(s.clip));                                     /* @0x8010329c */
            s.ph += d;                                                              /* @0x801032a4-b0 */
            s.spd = (int16_t)(s.spd - 120); if (s.spd < 0) s.spd = 0;               /* @0x801032b4-d8 */
            break; }
        case 2: case 4:                                                             /* 0x801032DC */
            if (s.ph == 4) pl->anim_blend_rate = 1024;   /* One-Save-Einstieg (PL+6 = 4) */
            clip_wort(GTR_CLIP_LIEGT, 1); s.ph = 5; s.sperre = 0;                  /* 0x00030011 @0x801032dc-e4, @0x801032e8-ec, @0x801032f0 */
            /* faellt in Ph5 @0x801032f4 */
        case 5: {
            int d = adv(fc_raum(s.clip)); d += adv(fc_raum(s.clip));                /* @0x80103300 / @0x80103318 */
            if (d) { s.ph = 6; clip_wort(GTR_BASIS_CLIP, 0); }                      /* 0x00030006 @0x80103324-3c */
            break; }
        case 6: {                                                                   /* 0x80103340 */
            const int fc = (pl00 && pl00->clip_count > GTR_BASIS_CLIP && pl00->clips[GTR_BASIS_CLIP].frame_count > 0)
                         ? pl00->clips[GTR_BASIS_CLIP].frame_count : GTR_BASIS_FC;
            if (adv(fc)) fertig = 1;                                                /* a3 0x10400 @0x80103344-54, Ende @0x80103358-70 */
            break; }
        default: break;                                                             /* [3] = 0x80103374, `sltiu v0,v1,0x7` @0x80103214 */
        }
    }
    /* (W) Kante: nach dem RE2-Schritt, vor der Klemme des Dispatcher-Schwanzes */
    if (fall_vorher == 1 || fall_vorher == 3) fall_takt(pl);
    else kante_pruefen(rdt, pl);
    /* Port-Felder der Pose (One-Save Sub 2 = Ende ohne Clip-Wort: die Figur behaelt ihre Pose) */
    if (s.hat_pose) {
    pl->motion = (int16_t)s.zeig_clip;
    pl->anim_frame = s.pose_f;
    pl->anim_frac = (uint8_t)s.zeig_blend;
    pl->motion_init_delay = 0;
    if (s.zeig_raum) pl->anim_flags &= (uint16_t)~0x80u; else pl->anim_flags |= 0x80u;   /* a3 & 0x10000 = rueckwaerts */
    s.pose_latch = s.zeig_raum ? s.zeig_clip : -1;
    }
    pl->sub_state_2 = (uint8_t)s.ph;
    if (s.sperre) pl->hit_react |= 1u; else pl->hit_react &= (uint8_t)~1u;
    if (fertig) {
        if (s.fall == 1 || s.fall == 3) {                                                   /* Sturz noch nicht gelandet: zu Ende fallen */
            if (!s.ende_wartet) gtr_log("F%u ENDE wartet auf die Landung (Sturz Bild %d)", (unsigned)g_engine.frame_count, s.fall_t);
            s.ende_wartet = 1;
        } else ende(pl, von_x, von_z);
    } else {
        gtr_log("F%u sub %d ph %d bild %d schritt %d clip %d/%d %s sperre %d figur (%d,%d,%d) gier %d band %d fall %d von (%d,%d)", (unsigned)g_engine.frame_count,
                s.sub, s.ph, s.bild, s.schritt, s.zeig_clip, s.pose_f, s.zeig_raum ? "raum" : "basis-rw", s.sperre, (int)pl->x, (int)pl->y, (int)pl->z,
                (int)pl->rot_y & 0xfff, re15_collision_debug_band(), s.fall, (int)von_x, (int)von_z);
        s.bild++;
    }
}

int re15_gator_treffer_r41_rbj_clip(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (s.an && s.hat_pose) return s.zeig_raum ? s.zeig_clip : -1;
    if (s.pose_latch >= GTR_CLIP_LIEGT && pl->motion == s.pose_latch) return s.pose_latch;   /* +0x168 zeigt noch in die Raumbank */
    s.pose_latch = -1;                                                              /* +0x94 neu geschrieben: der naechste anim_set hat umgesetzt */
    return -1;
}
int  re15_gator_treffer_r41_pl00(void) { return s.an && s.hat_pose && !s.zeig_raum; }

void re15_gator_treffer_r41_info(re15_gtr_info_t *o)
{
    if (!o) return;
    memset(o, 0, sizeof *o);
    o->aktiv = s.an; o->sub = s.sub; o->ph = s.ph; o->bild = s.bild;
    o->clip = s.hat_pose ? s.zeig_clip : -1; o->pose_f = s.pose_f; o->raumbank = s.zeig_raum;
    o->sperre = s.sperre; o->schritt = s.schritt; o->fall = s.fall;
    o->n_start = s.n_start; o->n_ende = s.n_ende; o->n_fall = s.n_fall; o->n_landung = s.n_landung;
    o->art = s.art; o->achse = s.achse; o->schaden = s.schaden; o->ret = s.ret; o->se = s.se;
    o->start_x = s.start_x; o->start_z = s.start_z;
    o->gier_figur_vor = s.gier_figur_vor; o->gier_gator = s.gier_gator;
}
