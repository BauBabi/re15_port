/*
 * RE1.5 Rebuilt — Spieler-Haken der Birkin-Treffer (Kopf, Registrierung und Abgrenzung: include/re15_birkin_opfer.h).
 *
 * ⛔ NUTZER-REPORT v0.8.29: "Beim 2. Birkin fight stuerzt sogar Leon's animation ab, wenn man von birkin getroffen
 * wird" (ROOM5080, gleicher Kampf am Anfang von 5090). GEMESSEN im exe-Lauf (RE15_DEBUG_JUMP 5080, Birkin sub 3/4/5):
 * der Port setzte bei jedem Treffer nur den Pin s_player_grabbed — Leon blieb in seiner Steh-Pose eingefroren, weil
 * es fuer Typ 0x30 keine Opfer-Maschine gab. Das Original faehrt den Typ-Haken 0x8011afd8 (STAGE5).
 *
 * Alle Adressen STAGE5.BIN (RAW @0x80100000). Feld-Abbildung Spieler (Block 0x800aca54): aca58 cmd, aca59 Variante,
 * aca5a Phase, acae0 = +0x8c Tempo, acae3 = +0x8f Ueberblendung (anim_frac), acae8 = +0x94 Clip (motion),
 * acae9 = +0x95 Bild, acaf0 = Ringkampf-Zaehler, acaf2 = Biss-Wiederholung, acad8/acbc0 = Leons eigene Bank,
 * acbcc/acbd0 = Opfer-Bank des Greifers (Birkin+0x178/+0x17c, EM30 dir[5]/[6] = 3 Clips 25/66/5 Bilder, 15 Bones).
 * aca3c |= 0x40 / acc0c sind Anzeige-/Sperrbits ohne Port-Gegenstueck (dokumentiert, nicht gebaut).
 */
#include "re15_birkin_opfer.h"
#include "re15_audio.h"         /* re15_audio_core_se = FUN_80045024(0x04ss0001, &Spieler+0x34) */
#include "re15_ai_flavor.h"     /* re15_re2z_mash = FUN_80037024 */
#include "re15_player.h"        /* re15_player_pl00_clip_frames (Leons eigene Bank) */
#include "re15_math.h"
#include "re15_damage.h"        /* re15_cos_q12 / re15_sin_q12 */
#include "re15_room.h"          /* g_current_room_id */
#include <stdio.h>
#include <stdlib.h>

int  re15_player_wand_bit(void);                                          /* game_step_common.c: Spieler+0 & 0x10 */
void re15_player_victim_haken(const re15_actor_t *greifer, int an);       /* enemy_ai_common.c */

static uint8_t s_cmd  = 0;      /* aca58-Byte; 0 = kein Haken */
static uint8_t s_var  = 0;      /* aca59 */
static uint8_t s_ph   = 0xff;   /* aca5a */
static int32_t s_tempo = 0;     /* acae0 (+0x8c), s16 */
static int32_t s_zaehler = 0;   /* acaf0, s16 */
static uint8_t s_biss = 0;      /* acaf2 */
static uint8_t s_f314 = 0;      /* im Vortick lief anim_set (FUN_8001f314) — +0x8f-Abbau zu Beginn des Ticks */
static uint8_t s_eigen = 0;     /* laufende Phase spielt acad8/acbc0 */

int re15_bk_opfer_cmd(void)       { return s_cmd; }
int re15_bk_opfer_eigenbank(void) { return s_cmd && s_eigen; }
int re15_bk_opfer_phase(void)     { return s_cmd ? (int)s_ph : 0xff; }
int re15_bk_opfer_variante(void)  { return s_var; }
void re15_bk_opfer_reset(void)    { s_cmd = 0; s_var = 0; s_ph = 0xff; s_tempo = 0; s_zaehler = 0; s_biss = 0; s_f314 = 0; s_eigen = 0; }

int re15_bk_opfer_start(const re15_actor_t *birkin, uint8_t cmd, uint8_t variante)
{
    /* cmd 6 (Wurf, aca59 = 0): STAGE5 registriert ihn auf den Haken selbst (@0x8011defc) -> [0]; STAGE3 auf den
     * eigenen Stub 0x8011b168 (STAGE3_overlay.c:12761), dessen Tabelle @0x8011f4b0[0] = 0x8011b1a8 die WURF-
     * TOD-Maschine ist (in STAGE5 als aca59-Eintrag [3] @0x8011b9bc mitgefuehrt, dort von keinem Treffer
     * erreicht). Port: Variante 3. Andere Stages registrieren fuer 0x30 nichts. */
    unsigned stage = (g_current_room_id >> 12) & 0xFu;
    if (stage != 3u && stage != 5u) return 0;   /* Registrierungs-Scan aller sechs Overlays: nur STAGE3/5 setzen 0x800ac818 */
    if (cmd == 6) {
        if (stage == 3u) variante = 3;
        else if (stage != 5u) return 0;
    }
    if (variante > 3) return 0;
    /* Ohne Opfer-Bank (EM30 dir[5]/[6] nicht geladen, z.B. Unit-Harness ohne Modell) kann der Haken Leon nicht
     * posieren — re15_player_victim_tick wuerde ihn sofort abbrechen; dann bleibt der alte Pin. */
    { const re15_enemy_bank_t *vb = birkin ? re15_enemy_find(birkin->type) : NULL;
      if (!vb || !vb->victim_ok) return 0; }
    s_cmd = cmd; s_var = variante; s_ph = 0;   /* Wort-Store aca58 = (aca59 << 8) | cmd: aca5a = 0 */
    s_f314 = 0; s_eigen = 0;
    re15_player_victim_haken(birkin, 1);       /* acbfc = Birkin, acbcc/acbd0 = Birkin+0x178/+0x17c */
    return 1;
}

/* anim_set(Bank, a2, a3) auf dem Spieler (Konvention wie affen3_r38.c opfer_anim): +0x95 weiter, Rueckgabe 1 am
 * Clip-Ende (Bild auf 0 gewickelt), a2 = 1 -> rueckwaerts (anim_flags 0x80), a3 = Ueberblend-Schritt. */
static int anim(re15_actor_t *pl, const re15_enemy_bank_t *vb, int eigen, int rueck, uint16_t a3)
{
    int fc;
    s_eigen = (uint8_t)eigen;
    if (eigen) fc = re15_player_pl00_clip_frames((int)pl->motion);
    else fc = (vb && (int)pl->motion < vb->anim_victim.clip_count) ? vb->anim_victim.clips[pl->motion].frame_count : 1;
    if (fc < 1) fc = 1;
    if (rueck) pl->anim_flags |= 0x80u; else pl->anim_flags &= (uint16_t)~0x80u;
    int ende = ((int)pl->anim_frame + 1 >= fc);
    pl->anim_frame = (uint8_t)(((int)pl->anim_frame + 1) % fc);
    pl->anim_blend_rate = a3;
    s_f314 = 1;
    return ende;
}

/* FUN_800245d8(ofs): +0x8c entlang Gier + ofs (0x800 = rueckwaerts). Keine Klemme hier — die Wandklemme @0x80031d70
 * laeuft im Dispatcher-Schwanz danach (game_step: re15_player_body_and_walls mit dem Vor-Haken-Standpunkt). */
static void vorschub(re15_actor_t *pl, int ofs)
{
    int32_t v = (int32_t)(int16_t)s_tempo;
    int16_t a = (int16_t)(((int)pl->rot_y + ofs) & 0xfff);
    pl->x += (int32_t)(((int32_t)re15_cos_q12(a) * v) >> 12);
    pl->z -= (int32_t)(((int32_t)re15_sin_q12(a) * v) >> 12);
}

static void clip(re15_actor_t *pl, uint8_t c) { pl->motion = c; pl->anim_frame = 0; }

/* Ringkampf-Zaehler: 75/25/0x32 Basis, HP < 30 -> 275/225/0xfa, HP < 10 -> += 380 (alle drei Saetze unten). */
static void zaehler(const re15_actor_t *pl, int basis, int unter30)
{
    s_zaehler = basis;
    if (pl->hp < 30) s_zaehler = unter30;
    if (pl->hp < 10) s_zaehler = (int16_t)(s_zaehler + 380);
}

/* Loesen: aca58 = 1 (Wort), acc0c = 0, +0x93 &= 0xfe, aca3c &= ~0x40 (@0x8011b3b0-3ec / @0x8011b688-6c4 /
 * @0x8011b92c-968). Lief der Schluss-Clip rueckwaerts, ist die Endpose Bild 0 (wie kd_ende / affen3 Phase 10). */
static void loesen(re15_actor_t *pl)
{
    pl->hit_react &= (uint8_t)~1u;
    if (pl->anim_flags & 0x80u) { pl->anim_frame = 0; pl->anim_flags &= (uint16_t)~0x80u; }
    s_cmd = 0; s_ph = 0xff; s_eigen = 0;
    re15_player_victim_haken(NULL, 0);
}

/* [0] 0x8011b018 — von VORN: Rueckstoss, an der Wand Ringkampf. Phasen @0x801004d4 (aca5a < 0xe @0x8011b028). */
static void haken_vorn(re15_actor_t *pl, const re15_enemy_bank_t *vb)
{
    switch (s_ph) {
    case 0:                                                            /* @0x8011b054 */
        s_ph = 1;                                                      /* aca5a = 1 @0x8011b068 */
        pl->motion = 0x0d; pl->anim_frame = 6; pl->anim_frac = 3;      /* acae8 0xd / acae9 6 / acae3 3 @0x8011b06c-8c */
        s_tempo = 0x4b0;                                               /* acae0 = 0x4b0 @0x8011b090-98 */
        pl->hit_react |= 1;                                            /* @0x8011b0b0-c8 */
        re15_audio_core_se(1);                                         /* a0 = 0x04010001 @0x8011b054/@0x8011b0d0, jal @0x8011b0cc */
        /* faellt in [1] @0x8011b0d4 */
    case 1:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 0, 0x400));            /* anim_set(acad8, acbc0, 0, 0x400) @0x8011b0e8, += @0x8011b100 */
        s_tempo = (int16_t)(s_tempo - 150);                            /* @0x8011b104 */
        if (s_tempo < 400) s_tempo = 400;                              /* slti 400 / sh 0x190 @0x8011b118-130 */
        if (re15_player_wand_bit()) s_ph = 5;                          /* Spieler & 0x10 @0x8011b138-144, aca5a = 5 @0x8011b204 */
        break;
    case 2:                                                            /* @0x8011b154 */
        s_ph = 3; pl->motion = 0x0e; pl->anim_frame = 0; pl->anim_frac = 3;   /* @0x8011b154-178 */
        /* faellt in [3] @0x8011b17c */
    case 3:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 0, 0x400));            /* @0x8011b190, += @0x8011b1a8 */
        s_tempo = (int16_t)(s_tempo - 50);                             /* @0x8011b1ac */
        if (s_tempo < 0) s_tempo = 0;                                  /* bgez / sh zero @0x8011b1c4-d0 */
        if (re15_player_wand_bit() && s_tempo >= 300) s_ph = 5;        /* @0x8011b1d4-208 */
        break;
    case 5:                                                            /* @0x8011b214 — gegen die Wand */
        s_ph = 6; pl->motion = 0x0f; pl->anim_frame = 0; pl->anim_frac = 0;   /* @0x8011b228-248 */
        s_tempo = 0;                                                   /* sh zero acae0 @0x8011b24c-50 */
        re15_audio_core_se(7);                                         /* 0x04070001 @0x8011b214-18, jal @0x8011b254 */
        re15_audio_core_se(2);                                         /* 0x04020001 @0x8011b25c-60, jal @0x8011b264 */
        /* j 0x8011b378 (a2 = 0) @0x8011b26c */
    case 6:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 0, 0x400));            /* @0x8011b388, += @0x8011b39c */
        break;
    case 7:                                                            /* @0x8011b274 */
        s_ph = 8; pl->motion = 0;                                      /* aca5a 8 / acae8 0 @0x8011b27c-290 */
        zaehler(pl, 0x4b, 0x113);                                      /* 0x4b @0x8011b288 / 0x113 @0x8011b2a4 / +380 @0x8011b2c8 */
        /* faellt in [8] @0x8011b2d4 */
    case 8: {
        (void)anim(pl, vb, 0, 0, 0x400);                               /* anim_set(acbcc, acbd0, 0, 0x400) @0x8011b2e8 */
        int m = re15_re2z_mash();                                      /* jal 0x80037024 @0x8011b2f0 */
        s_zaehler = (int16_t)(s_zaehler - 4 - 9 * m);                  /* v*8+v @0x8011b2f8-304, -4 @0x8011b308 */
        if (s_zaehler < 0) s_ph = 9;                                   /* bgez / aca5a 9 @0x8011b31c-328 */
        break; }
    case 9:                                                            /* @0x8011b334 */
        s_ph = 10; clip(pl, 0x10);                                     /* @0x8011b334-350 */
        /* j 0x8011b378 (a2 = 0) */
    case 10:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 0, 0x400));
        break;
    case 11:                                                           /* @0x8011b35c — Aufstehen = 0xb rueckwaerts */
        s_ph = 12; pl->motion = 0x0b;                                  /* @0x8011b35c-370, Bild bleibt (gewickelt 0) */
        /* a2 = 1 @0x8011b374 */
    case 12:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 1, 0x400));
        break;
    case 4: case 13:                                                   /* @0x8011b3b0 */
        loesen(pl);
        break;
    default:
        break;                                                         /* aca5a >= 0xe -> @0x8011b3f0 */
    }
    vorschub(pl, 0x800);                                               /* jal 0x800245d8, a0 = 0x800 @0x8011b3f0-f4 */
}

/* [1] 0x8011b40c — von HINTEN: Sturz nach vorn in den Ringkampf. Phasen @0x8010050c (aca5a < 9 @0x8011b418). */
static void haken_hinten(re15_actor_t *pl, const re15_enemy_bank_t *vb)
{
    switch (s_ph) {
    case 0:                                                            /* @0x8011b444 */
        s_ph = 1; pl->motion = 0x0c; pl->anim_frac = 3;                /* aca5a 1 / acae8 0xc / acae3 3 @0x8011b450-46c */
        s_tempo = 0x190;                                               /* @0x8011b470-478 */
        pl->anim_frame = 0;                                            /* acae9 0 @0x8011b49c */
        pl->hit_react |= 1;                                            /* @0x8011b490-4b4 */
        re15_audio_core_se(2);                                         /* 0x04020001 @0x8011b484/@0x8011b4bc, jal @0x8011b4b8 */
        /* faellt in [1] @0x8011b4c0 */
    case 1:
        if (pl->anim_frac == 1) re15_audio_core_se(7);                 /* acae3 == 1 -> 0x04070001 @0x8011b4c8-4e0 */
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 0, 0x400));            /* @0x8011b4fc, += @0x8011b514 */
        s_tempo = (int16_t)(s_tempo - 40);                             /* @0x8011b518 */
        if (s_tempo < 0) s_tempo = 0;                                  /* @0x8011b530-53c */
        break;
    case 2:                                                            /* @0x8011b548 */
        s_ph = 3; pl->motion = 0;                                      /* @0x8011b550-564 */
        zaehler(pl, 0x19, 0xe1);                                       /* 0x19 @0x8011b55c / 0xe1 @0x8011b578 / +380 @0x8011b59c */
        /* faellt in [3] @0x8011b5a8 */
    case 3: {
        (void)anim(pl, vb, 0, 0, 0x400);                               /* anim_set(acbcc, acbd0) @0x8011b5bc */
        int m = re15_re2z_mash();                                      /* @0x8011b5c4 */
        s_zaehler = (int16_t)(s_zaehler - 4 - 10 * m);                 /* (v*4+v)*2 @0x8011b5cc-5dc, -4 @0x8011b5e0 */
        if (s_zaehler < 0) s_ph = 4;                                   /* @0x8011b5f4-600 */
        break; }
    case 4:                                                            /* @0x8011b60c */
        s_ph = 5; clip(pl, 0x10);                                      /* @0x8011b60c-628 */
        /* j 0x8011b650 (a2 = 0) */
    case 5:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 0, 0x400));            /* @0x8011b660 */
        break;
    case 6:                                                            /* @0x8011b634 */
        s_ph = 7; pl->motion = 0x0b;                                   /* @0x8011b634-648 */
        /* a2 = 1 @0x8011b64c */
    case 7:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 1, 0x400));
        break;
    case 8:                                                            /* @0x8011b688 */
        loesen(pl);
        break;
    default:
        break;
    }
    vorschub(pl, 0);                                                   /* jal 0x800245d8, a0 = 0 @0x8011b6c8-cc */
}

/* [2] 0x8011b6e0 — BISS: Opfer-Clip 2 zweimal, Ringkampf, Aufstehen. Phasen @0x80100534 (aca5a < 9 @0x8011b6ec).
 * Kein Vorschub (die Maschine endet @0x8011b96c ohne FUN_800245d8). */
static void haken_biss(re15_actor_t *pl, const re15_enemy_bank_t *vb)
{
    switch (s_ph) {
    case 0:                                                            /* @0x8011b718 */
        s_ph = 1; pl->anim_frac = 3; pl->motion = 2;                   /* aca5a 1 / acae3 3 / acae8 2 @0x8011b72c-74c */
        s_biss = 2;                                                    /* acaf2 = 2 @0x8011b754 */
        pl->anim_frame = 0;                                            /* @0x8011b764 */
        pl->hit_react |= 1;                                            /* @0x8011b758-784 */
        re15_audio_core_se(0);                                         /* 0x04000001 @0x8011b718/@0x8011b78c, jal @0x8011b788 */
        /* faellt in [1] @0x8011b790 */
    case 1:
        s_biss = (uint8_t)(s_biss - anim(pl, vb, 0, 0, 0x400));        /* anim_set(acbcc, acbd0, 0, 0x400) @0x8011b7a4, -= @0x8011b7b8 */
        if (s_biss == 0) s_ph = 2;                                     /* @0x8011b7c4-7d4 */
        break;
    case 2:                                                            /* @0x8011b7e0 */
        s_ph = 3; pl->motion = 0; pl->anim_frac = 6;                   /* @0x8011b7e8-810 */
        zaehler(pl, 0x32, 0xfa);                                       /* 0x32 @0x8011b7f4 / 0xfa @0x8011b81c / +380 @0x8011b840 */
        /* faellt in [3] @0x8011b84c */
    case 3: {
        (void)anim(pl, vb, 0, 0, 0x200);                               /* a3 = 0x200 @0x8011b864 */
        int m = re15_re2z_mash();                                      /* @0x8011b868 */
        s_zaehler = (int16_t)(s_zaehler - 4 - 10 * m);                 /* @0x8011b870-884 */
        if (s_zaehler < 0) s_ph = 4;                                   /* @0x8011b898-8a4 */
        break; }
    case 4:                                                            /* @0x8011b8b0 */
        s_ph = 5; clip(pl, 0x10);                                      /* @0x8011b8b0-8cc */
    case 5:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 0, 0x400));            /* @0x8011b904 */
        break;
    case 6:                                                            /* @0x8011b8d8 */
        s_ph = 7; pl->motion = 0x0b;                                   /* @0x8011b8d8-8ec */
    case 7:
        s_ph = (uint8_t)(s_ph + anim(pl, vb, 1, 1, 0x400));            /* a2 = 1 @0x8011b8f0 */
        break;
    case 8:                                                            /* @0x8011b92c */
        loesen(pl);
        break;
    default:
        break;
    }
}

/* [3] STAGE3 0x8011b1a8 (cmd 6 aus dem Stub 0x8011b168) — WURF-TOD. Verzweigung nach aca5a @0x8011b1f4-234.
 *   P0 @0x8011b23c: aca5a 1 @0x8011b244, acae3 3 @0x8011b24c, acae8 1 (Opfer-Clip 1, 66 Bilder) @0x8011b260,
 *      acae0 0 @0x8011b280, acae9 0 @0x8011b278, +0x93 |= 1 @0x8011b288-298, SE 0x04030001 (`lui a0,0x403`
 *      @0x8011b21c, `ori` @0x8011b258, jal @0x8011b29c); faellt in den Anim-Schwanz @0x8011b558.
 *   P1 @0x8011b558: anim_set(acbcc, acbd0, 0, 0x400) @0x8011b56c, aca5a += @0x8011b588.
 *   P2 @0x8011b594: aca58 = 7 (`ori v0,zero,0x7` @0x8011b230 im Delay-Slot, `sw` @0x8011b598) = cmd 7, die
 *      Blutlache 0x8003694c; die ruft kein anim_set — die Pose bleibt die des letzten Bildes.
 * ⛔ OFFEN (nicht gebaut, Adressen benannt): der P0-Block @0x8011b2a4-554 misst Koerperteile des Spielers
 *   (acbdc + 0x40 + k*0xac, Translation +0x14/+0x1c) gegen den Trefferpunkt acc1c/acc1e, den der Wurf aus der
 *   Angriffsbox FUN_8001a5e0 schreibt (@0x80117eec-f4 STAGE3), und setzt bei Abstand >= 301 (`slti 301`) Blut
 *   FUN_80019700 (a0 0x3000/0x2800) plus Teil-Flags |= 0x2000|0x80 und +0x88..+0x98. Der Port hat FUN_8001a5e0
 *   nicht (Abstand/Kegel-Proxy re15_bk_arc, kein Trefferpunkt) — ohne ihn waere jeder Abstand geraten. */
static void haken_wurf3(re15_actor_t *pl, const re15_enemy_bank_t *vb)
{
    switch (s_ph) {
    case 0:
        s_ph = 1; pl->anim_frac = 3; pl->motion = 1;
        pl->anim_frame = 0; s_tempo = 0;
        pl->hit_react |= 1;
        re15_audio_core_se(3);
        /* faellt in den Anim-Schwanz @0x8011b558 */
    case 1:
        if (anim(pl, vb, 0, 0, 0x400)) {
            s_ph = 2;
            int fc = (vb && 1 < vb->anim_victim.clip_count) ? vb->anim_victim.clips[1].frame_count : 1;
            pl->anim_frame = (uint8_t)(fc > 0 ? fc - 1 : 0);   /* Endpose halten (cmd 7 ruft kein anim_set) */
        }
        break;
    case 2:
        s_ph = 3;
        if (pl->hp >= 0) pl->hp = -1;   /* PORT-PLUMBING wie beim Hund (@0x80111ea0): die Tod-/Gameover-FSM des Ports
                                         * keyt auf hp < 0; das Original schreibt hier keine HP */
        pl->state = 7;                  /* cmd 7 @0x8011b598 */
        break;
    default:
        break;                          /* cmd 7 besitzt den Spieler; der Haken ruft nichts mehr */
    }
}

void re15_bk_opfer_tick(re15_actor_t *pl, const re15_enemy_bank_t *vb)
{
    if (!s_cmd || !pl) return;
    if (s_f314 && pl->anim_frac > 0) pl->anim_frac--;                  /* FUN_8001f3bc mischt, dann +0x8f-- */
    s_f314 = 0;
    uint8_t var = s_var, ph = s_ph;
    switch (s_var) {                                                   /* jalr @0x80120468[aca59] @0x8011afdc-b000 */
    case 0:  haken_vorn(pl, vb);   break;
    case 1:  haken_hinten(pl, vb); break;
    case 2:  haken_biss(pl, vb);   break;
    default: haken_wurf3(pl, vb);  break;
    }
    /* Messzeile (RE15_BIRKIN_DBG, stderr = debug.log): je Haken-Bild Variante/Phase/Clip/Bild/Lage. */
    { static int an = -1; if (an < 0) an = getenv("RE15_BIRKIN_DBG") ? 1 : 0;
      if (an) fprintf(stderr, "[bk-haken] cmd %d var %d ph %d->%d clip 0x%02x bild %d eigen %d tempo %d zaehler %d pos (%d,%d) gier %d\n",
                      s_cmd, var, ph, s_ph, pl->motion, pl->anim_frame, s_eigen, (int)s_tempo, (int)s_zaehler,
                      (int)pl->x, (int)pl->z, (int)pl->rot_y); }
}
