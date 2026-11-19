/*
 * RE1.5 Rebuilt — RE2-ZELLENARM: das Gehirn des RE2-Retail-Typs 0x2D ("Zombie-Arme im Fenster",
 * ROOM2050) als RE2-Flavor-Fassung der ROOM1210-Gitterhaende (RE1.5-Typ 0x1A).
 *
 * Runde 16 / Phase 2 (2026-09-19). Dossier: analysis/befunde_2026-09-19/arme-1210-re2.md,
 * Gegenpruefung arme-1210-re2.skeptiker.md (deren Korrekturen gelten). Alle Adressen ohne
 * Praefix meinen das Overlay CDEMD0_EM2D_ai1.BIN (CDEMD0.EMS Sektor 0x9B0, 0x1528 B, geladen
 * @0x80100000; Volltext arme-1210-re2_em2d_ai1.dis, Stichproben gegen re2_disasm.py bit-identisch),
 * "EXE" meint info/re2leon/PSX.EXE (ghidra_re2_Leon.txt).
 *
 * ---- WER DEN ARM WANN TICKT (EXE, selbst gelesen) --------------------------------------------
 *   Per-Frame-Schleife @0x80026570-604 (Hauptschleife, undekompiliert):
 *     80026590  lhu v0,0x10e(s0) / 80026598 andi v0,v0,0x8000 / 8002659c bne v0,zero,SKIP
 *         -> ein Arm mit +0x10E Bit 0x8000 (Schlaf, gesetzt von ROOM2050 main00 @0x1A4C ff.
 *            `member 7 |= 0x8000`) wird UEBERHAUPT NICHT getickt.
 *     800265a4-d8  +0x1F0 = SquareRoot0((x-PL.x)^2 + (z-PL.z)^2)   (32-bit-Deltas, SquareRoot0)
 *     800265dc  lbu v1,8(s0) / 800265ec lw v0,0x3c74(v1) = 0x800CFE5C[kind] / 800265f4 jalr = ROOT
 *   Raumlade-Tick FUN_8004A694 (EINZIGER Aufrufer FUN_80049E48 @0x8004A35C = Raumlader, selbst
 *   aus FUN_80026B7C = Tuerwechsel): blendet 0x8000 fuer den INIT-Tick aus (@0x8004A734-38),
 *   liest +0x158 VOR dem Root (@0x8004A740) und erzwingt bei != 0 danach Routine 4 mit
 *   Sub = (+0x158 & 0xFF00) - 0x100, Clip = +0x158 & 0xFF (@0x8004A760-94). Das ist die
 *   SKRIPT-Startpose aus dem Spawn-Record; der Sce_em_set-Handler FUN_8005714C schreibt +0x158
 *   NICHT (alle Stores selbst gelistet; die Entity kommt aus der Bump-Arena 0x800D2144
 *   @0x80057228-48, Stride 0x248). Nach dem INIT ist +0x158 damit ein freier Zeitgeber des Arms —
 *   die Lesungen rng&0xF (INIT), (rng&0x1F)+60 (REACH) und (rng&0x1F)+30 (RUECKZUG) HALTEN.
 *   (Skeptiker-Punkt 5 geklaert.)
 *
 * ---- SPIELER-ROUTINE 5 (Skeptiker-Punkt 6, EXE) ------------------------------------------------
 *   Spieler-Root FUN_8003BFAC liest die Routinen-Tabelle PTR_LAB_800A4030[PL+0x4] (@0x8003C19C);
 *   [5] = 0x8004006C (@0x800A4044), [6] = 0x800400D0 (@0x800A4048).
 *   0x8004006C: 0x800CFBD8 |= 0x40; PL.word0 &= ~4; a2 = PL+0x1B4 (Greifer); kind = a2+0x8;
 *   a1 = PL+0x188, a2 = PL+0x18C; jalr 0x800CE300[kind] (@0x800400B0-B8) — 0x800CE300 + 0x2D*4
 *   = 0x800CE3B4 = genau der Hook, den der Arm-INIT installiert (`sw v0,-7244(at)` @0x80100144,
 *   at = 0x800D0000). Der Hook 0x8010121C dispatcht ueber PL+0x5 (Tabelle @0x80101514, alle
 *   vier Eintraege = 0x80101258) und dort ueber PL+0x6 (Tabelle @0x80100004):
 *     P0 @0x801012A8: PL-Clipwort 0x000F0000 (Opfer-Clip 0, Rate 15), PL+0x6 = 1,
 *                     FX 0x8005BA28(0x4000001 | ((rng&1)+1)<<16, &PL.x) = CORE-SE 1/2 an Leon,
 *                     s0 = FUN_80015910(PL, Greifer) (@0x801012E0: (Greifer.yaw - PL.yaw + 0x400)
 *                     & 0xFFF < 0x800), FUN_80015558(PL, Greifer.x, Greifer.z, 2048) = Zudrehen,
 *                     s0 != 0 -> PL.yaw += 2048 (@0x80101304-18), Advance 0x8002959C(…,256).
 *     P1 @0x8010131C: nur Advance (Clip 0 laeuft in Schleife, solange der Arm haelt).
 *     P2 @0x80101338: Clipwort 0x00070001 (Opfer-Clip 1, Rate 7), PL+0x6 = 3, Advance, +done.
 *     P3 @0x8010134C: Advance, PL+0x6 += done.
 *     P4 @0x80101374: PL+0x4 = 1 (frei), PL+0x1D3 &= 0x7F (Ein-Angreifer-Riegel loesen).
 *   KEIN Schaden: weder der Hook noch das Arm-Overlay rufen FUN_800401D4 (Zensus aller jal im
 *   Chunk: 0 Treffer). Der Griff kostet in RE2 keine HP. Mash = FUN_8001598C =
 *   (DAT_800CE310 & 0x34F) != 0 (Decompile), im Port re15_re2z_mash().
 *
 * ---- SE-BANK (Skeptiker-Punkte 7/9, EXE) -------------------------------------------------------
 *   Sce_em_set-Handler FUN_8005714C: `lbu v0,7(v0)` @0x80057274 / `sb v0,506(s0)` @0x80057280
 *   (Record+7 = em-Id -> +0x1FA), `lhu v0,4(v1)` @0x8005734C / `sh v0,270(s0)` @0x80057354
 *   (Record+4..5 = Spawn-Flags -> +0x10E: Bit 0 = Arm A/B, Bit 4 = Zeitgeber-Master), `lbu
 *   v0,3(v1)` @0x80057334 / `sb v0,8(s0)` @0x8005733C (kind). Die Bankwahl FUN_80052B38
 *   vergleicht +0x1FA gegen die Paar-Tabelle @0x800A7400 (Zeile == Bank, `lb v1,506(a0)`
 *   @0x80052C48); em-Id 0x11 steht in Zeile 42 = {0x05, 0x11} (EXE-Datei 0x97C54 = `05 11`),
 *   zweite Haelfte -> flag2000 = 1 (+0x10 in der EDT-Map, FUN_8005BD6C-Kopf).
 *
 * ---- PORT-BRUECKEN (benannt, nicht erfunden) --------------------------------------------------
 *   - Weckruf: RE2 loescht +0x10E Bit 0x8000 je Gruppe ueber AOT-Zone 3/4 -> sub03/sub04
 *     (`26 00 06 10 ff 7f` @ROOM2050 0x1D40 ff.). ROOM1210 hat statt dessen Aot_set aot=6 sce=3
 *     @0x1EAE -> sub02 @0x1EC8: zehnmal `2e 02 0N 00` + `34 0c 01 00` = Member_set(12,1) -> +0x9 = 1
 *     (@0x800411F8) fuer ALLE ZEHN in einem Bild. Der Port bildet "+0x9 & 0x1F == 1" auf
 *     "Bit 0x8000 geloescht" ab: vorher kein Tick (wie @0x8002659C). Folge — dokumentiert, kein
 *     Wunschbild: alle zehn wachen im selben Bild; gestaffelt wird nur ueber die Distanz 2500
 *     (A0), der Kegel 1024 ist eine Halbebene (`sltiu < 2*half` = 2048 = 180 Grad).
 *   - Arm-Wahl/Master: ROOM1210-Records tragen keine RE2-Spawn-Flags (+4..5 = 00 00). RE2
 *     ROOM2050 (Records @0x1970..0x1A36, pc[4]) = 10,01,00,01,00 je Fuenfergruppe. Der Port
 *     vergibt in Spawn-Reihenfolge dieselbe Folge (k%5: 0 = Master+Arm A, 1 = B, 2 = A, 3 = B,
 *     4 = A).
 *   - Anker: RE2 setzt den Ursprung 298..523 HINTER die Wandflaeche und 1930..2700 ueber den
 *     Boden (Records + collision.sca ROOM2050, Dossier 2.6). ROOM1210 stellt die Arme auf
 *     x -25000/-14000 (4164/4378 hinter der Flurkante) und y = 0. Die Werte unten
 *     (RE2ARM_1210_*) sind am EXTRAHIERTEN Originalhintergrund gemessen (Rueckprojektion der
 *     Fensterbank durch die byte-true Kamera, Dossier 6) — die Messung steht vollstaendig im
 *     Kommentar an der Definition.
 *   - Kein Scheduler, kein Steuerpunkt-Helfer FUN_8004A808 (der Arm liest dessen Ausgaben
 *     +0x150/+0x154/+0x1C4/+0x1C6 nirgends; einziger Leser im Overlay ist +0x1F0 @0x80100538).
 *   - Blut FUN_8001BF10(0x21000, …) im DEATH ist das RE2-FX-System (nicht portiert) -> RE1.5-
 *     Raumbank-Blut am Bone (derselbe Stand-in wie enemy_ai_re2_dog.c re2d_fx).
 *   - Rumble FUN_8003947C/80039514/800395B8 (B4 P0/P1): kein Port-Rumble -> uebersprungen
 *     (wie beim Hund dokumentiert).
 *   - Schatten FUN_80016480 (INIT @0x80100378-A4): der Port zeichnet den generischen Aktor-
 *     Schatten; verborgen (Bit 2) wird auch er nicht gezeichnet (platform/pc/main.c).
 *   - Der TREFFER-Kasten des Ports bleibt der RE1.5-Kasten (300/1440, re15_enemy_apply_hitbox 0x1A).
 *     Der KOERPER-PUSH gegen den Spieler laeuft dagegen seit Runde 35 Spur H (Nachbesserung 3) mit
 *     dem RE2-Segment (+0x9A = 800 @0x80100338-3C, +0x9E = 500 @0x8010032C-30, Lokal-Lage 0
 *     @0x8010035C-64) ueber re15_re2arm_body_push_player = FUN_80034D0C aus dem Spieler-Pass
 *     FUN_800355C4 (@0x80026628) — am RE2-Original gemessen (GDB-Wachpunkt; Schreiber `sw v0,56(s1)` @0x800350c8 /
 *     `sw v1,64(s1)` @0x800350cc schieben Leon nach dem Pin auf 1251 vom Ursprung). FUN_80034D0C steigt aus, wenn eine der beiden
 *     Entities Bit 2 traegt (`(*a | *b) & 2`) -> ein verborgener Arm schiebt nicht; der Port setzt
 *     hit_radius_min = 0 solange verborgen (re15_body_push_player ueberspringt ihn; zugleich aus dem
 *     Trefferfilter — RE2: Schlaf 0x8000 = kein Ziel in FUN_800470C0 & Co, nach 0x701 hp = -1 =
 *     Gate @0x80047148).
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>             /* sqrt/fabs: Richtung des RE1.5-Arms (Halte-Lage) */
#include "re15_actor.h"
#include "re15_ai_flavor.h"      /* re15_ai_re2_for_type / re15_re2_rand / re15_re2z_* Shims  */
#include "re15_enemy_ai.h"       /* re15_ai_set_state_word / re15_player_victim_force / _end   */
#include "re15_enemy.h"          /* re15_enemy_find (Opfer-Bank-Cliplaenge)                     */
#include "re15_damage.h"         /* re15_ai_arc_test (== FUN_80015614), re15_enemy_bone_world_pos */
#include "re15_skeleton.h"       /* re15_sin_q12 / re15_cos_q12 / re15_skel_compute_pose (Parts)  */
#include "re15_anim_select.h"    /* re15_compute_actor_kf (Parts-Keyframe, Nachbesserung 2)     */
#include "re15_emd.h"            /* RE15_EMD_MAX_BONES                                          */
#include "re15_math.h"           /* re15_squareroot0 (SquareRoot0)                              */
#include "re15_room.h"           /* g_current_room_id / g_room_rdt                               */
#include "re15_collision.h"      /* re15_collision_constrain_enemy (FUN_8003b0a4, Koerper-Weg)    */
#include "re15_esp.h"            /* RE1.5-Raumbank-Blut (Stand-in fuer FUN_8001BF10)             */
#include "re15_audio.h"          /* re15_audio_core_se (Hook-P0 0x8005BA28)                      */
#include "re15_scd.h"            /* g_re15_pauseflags / RE15_PAUSE_PLAYER (NB4 Blick)              */
#include "re15_engine.h"         /* g_engine.frame_count (nur Messschiene RE15_RE2_TRACE) */
#include "re15_md1.h"            /* re15_pld_part / re15_md1_parse (Leons Koerper fuer die Halte-Lage) */
#include "re15_enemy_ai_re2_zellenarm.h"

extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew); /* FUN_80015558 */
FILE *re15_re2_trace_out(void);   /* Trace-Ziel neben der exe (die GUI-exe hat kein stderr) */

/* ---- ENEMSE ------------------------------------------------------------------------------ */
#define RE2ARM_ENEMSE_BANK     42   /* Paar-Zeile 42 = {0x05, 0x11} @0x800A7400 (Datei 0x97C54) */
#define RE2ARM_ENEMSE_FLAG2000  1   /* em-Id 0x11 ist der ZWEITE kind der Zeile -> +0x10          */
static void (*s_se_fn)(int, int) = 0;
static void (*s_bank_fn)(int)    = 0;
void re15_re2arm_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int))
{
    s_se_fn = se_fn; s_bank_fn = bank_fn;
    if (s_bank_fn) s_bank_fn(RE2ARM_ENEMSE_BANK);
}
static void arm_se(int id)             /* FUN_8005BD6C(id, self) */
{
    if (s_bank_fn) s_bank_fn(RE2ARM_ENEMSE_BANK);   /* Bank vor jedem SE anlegen (Mehrbank-Cache
                                                     * kommt in audio_pc.c von anderer Hand)       */
    if (s_se_fn) s_se_fn(id, RE2ARM_ENEMSE_FLAG2000);
}

/* ---- Overlay-/Raum-Globale ----------------------------------------------------------------- */
/* 0x800CFBF4 = der raumweite Griff-Cooldown. EIN Original-Wort mit den Hunde-Bits 0x20/0x40/0x80
 * und dem Kraehen-Mutex Bit 0 (enemy_ai_re2_dog.c g_re2_room_gflags) — hier als Zaehler benutzt,
 * wie das Overlay selbst (`lhu / addiu -1 / sh` @0x80100058-80; INIT-Clear @0x80100324;
 * 120 @0x80100D5C-60; Gate == 0 @0x80100A28-30). Kein zweites Wort erfinden. */
#define s_cd_cfbf4 g_re2_room_gflags
static uint8_t  s_se_cd = 0;           /* 0x80101524: SE-4-Cooldown (60 @0x8010063C-44; -- @0x80100098-B0) */
static unsigned s_room_of_state = 0xFFFFFFFFu;   /* Raumwechsel-Erkennung (Port) */
static int      s_spawn_count = 0;     /* laufende Spawn-Nummer -> Arm-Wahl/Master (s. Kopf)   */
static int      s_holder = -1;         /* Slot des Arms, der den Spieler haelt (PL+0x1B4)      */
static uint8_t  s_hook_phase = 0;      /* PL+0x6 der Spieler-Routine 5                         */
static uint8_t  s_pin_bild = 0;        /* Nachbesserung 3: B4 P0 hat in DIESEM Bild gepinnt      */
static int32_t  s_pl_vor[2];           /* Runde 4: Leons Lage vor dem Pin                         */

/* Per-Slot-Arbeitsbytes des Arms, die der Aktor nicht als Feld traegt (Port-Speicher). */
typedef struct {
    int16_t  home_yaw;      /* +0x218 @0x80100188 */
    int32_t  home_x;        /* +0x21C @0x8010018C */
    int32_t  home_z;        /* +0x220 @0x80100194 */
    uint8_t  latch224;      /* +0x224 (INIT 0 @0x80100174; 1 @0x80100650-54)                 */
    uint8_t  bone_base;     /* +0x1C1 = Tabelle 0x80101414[var*7] = 1 (A) / 8 (B)             */
    uint8_t  hidden;        /* Entity-Wort-0 Bit 2 (0x80101164: |= 2 bei a2 = 0, &= ~2 bei 1) */
    uint8_t  init_done;
    /* Runde 35 Spur H: die Pose, die die Part-Matrizen GERADE tragen. RE2 baut sie NUR in
     * 0x8002959C -> 0x80029614 aus dem Bild VOR dem Weiterzaehlen (+0x178 = Bild +0x14D
     * @0x800295C8-F8, Zaehlen +1 erst @0x80029B30); jede Lesung `lw ...,92/100(part)` (+0x5C/+0x64)
     * sieht also die Pose des LETZTEN Advance-Aufrufs, nicht den eben gesetzten Clip. */
    int16_t  pose_clip;
    uint16_t pose_frame;
    uint8_t  pose_ok;
    /* Runde 35 Spur H, Nachbesserung 2: die Parts SELBST, wie 0x80029614 sie fuehrt — gemischte
     * Rotation je Part (+0x68) und Wurzel (+0x2C..+0x34), Mischung t1 = a3 * +0x14E VOR dem
     * Dekrement (`lbu t3,334(s2)` @0x800296a8, `mult v0,t3` @0x800296bc; Rotation
     * +0x68 := (t1*aktuell + (4096-t1)*Ziel) >> 12 @0x800299f0-0x80029ab0, Wurzel ebenso
     * @0x800296e4-f0). part+0x5C (Welt-Hand) entsteht beim Zeichnen aus genau diesen Matrizen
     * (FUN_80027160 -> FUN_80027434 `gte_stlvnl(part+0x48 .t)`), also traegt der Pin die
     * Ueberblendung mit — die reine Keyframe-Pose (re15_enemy_bone_world_pos, QUERY) nicht. */
    int16_t  pa[RE15_EMD_MAX_BONES][3];
    int32_t  proot[3];
    int32_t  ptrans[RE15_EMD_MAX_BONES][3];   /* modell-lokale Part-Lage der gemischten Pose */
    uint8_t  pvalid;
    /* NB4: der Arm wurde seit dem Raumladen einmal GEZEICHNET (sichtbar). RE2 schreibt part+0x5C (Weltlage)
     * nur beim Zeichnen (FUN_80027160 -> FUN_80027434); ein nie gezeigter Arm traegt dort (0,0,0) — im
     * RE2-RAM gemessen fuer jeden verborgenen Arm (Laeufe n4_g8c / n4_g1b, extra.txt `k1=(0,0,0)`). */
    uint8_t  gezeichnet;
    /* Runde 42 Stufe 2: Rotation der gemischten Parts (fuer den EM1A-Handgelenk-Punkt, s. MODELL unten) und der
     * RE2-TAKT (Logik-Clip/-Bild), wenn das gezeichnete Modell EM1A ist. */
    int32_t  prot[RE15_EMD_MAX_BONES][9];
    int8_t   logik_clip;
    uint16_t logik_frame;
    /* Runde 42 Stufe 2 Runde 3: der RE1.5-KOERPER (s. MODELL, ZEICHNEN / KOERPER-WEG). Record-Lage vor dem Anker,
     * Bilder seit dem Ausloeser, Lage nach B[1] + Wandklemme, eigene Pose (Clip/Bild) fuer die Zeit ohne RE2-Arm. */
    int32_t  rec_x, rec_y, rec_z;
    int16_t  rec_yaw;
    uint8_t  koerper_an;
    uint16_t koerper_bild;
    int32_t  kx, kz;
    uint8_t  kclip;
    uint16_t kframe;
    /* Runde 4: der KOERPER laeuft ausserhalb eines Griffs wie das Original (B[1] -> B[2]/B[3], Block KOERPER-ZUSTAND), im
     * Griff in Clip 2 an seiner Stelle (Block GRIFF). Felder tragen die Original-Offsets der Entity bzw. der Parts 2/3. */
    uint8_t  kmode;          /* K_B1 / K_B2 / K_B3 / K_GRIFF / K_SINKEN                                  */
    uint8_t  kphase;         /* +0x6                                                                       */
    uint8_t  k9e, k9f;       /* +0x9E / +0x9F (B[2]-Zaehler; B[3]: +0x9E = y-Zittern als s8)               */
    int16_t  k9c;            /* +0x9C (B[3]: gemerktes y)                                                  */
    int16_t  k8c;            /* +0x8C Schrittweite B[2]                                                    */
    uint16_t k1d0;           /* +0x1D0 B[2]-Runden bis B[3]                                                */
    int32_t  ky;             /* +0x38 (y der Entity; B[3] zittert +-5)                                     */
    uint8_t  sk_an[2];       /* Part 2/3 Flag 0x400 (ScaleMatrix @0x8001e9ec)                               */
    int16_t  sk_y[2];        /* Part 2/3 +0x8A (Skalierung der lokalen y-Achse, Q12)                        */
    int8_t   sk_d[2];        /* Part 2/3 +0x76 (Zuwachs je Bild)                                           */
    int16_t  sk_n82, sk_n38; /* Part 2 +0x82 (Wachs-Zaehler) / +0x38 (Schrumpf-Zaehler)                     */
    uint8_t  g_halten;       /* GRIFF: 1 = Halte-Pose eingefroren                                          */
    uint8_t  treffer;        /* RE2-Treffer/-Tod gesehen -> Rueckzug sinkt (K_SINKEN)                      */
    uint16_t g_frame;        /* GRIFF: eigenes Bild in EM1A-Clip 2                                          */
    int32_t  halte_x, halte_z;   /* Runde 5: feste Halte-Lage der Figur (einmal beim Pin bestimmt)           */
    uint8_t  q_ok;               /* Runde 7: Halte-Lage, die das Tor A3 in diesem Bild bestimmt hat (Pin nimmt sie) */
    int32_t  q_x, q_z, q_sprung;
    /* Runde 6: Zustand des Koerpers, den ein Griff/Treffer unterbrochen hat (Block GRIFF: danach geht es dort weiter) */
    uint8_t  u_mode, u_phase, u_clip;
    uint16_t u_frame;
} re2arm_t;
enum { K_B1 = 0, K_B2, K_B3, K_GRIFF, K_SINKEN };
static re2arm_t s_arm[RE15_ACTOR_MAX];

/* ---- MODELL: RE1.5-EM1A zeichnen, RE2-Verhalten behalten (Runde 42 Strang "original", Stufe 2) ----------------------
 * NUTZERWUNSCH 18.11.2026, woertlich: "Und unten in den Gefängnis in STAGE1 hast du die Arme aus Resident Evil 2
 * übernommen. Aber du solltest EIGENTLICH NUR die funktionalität aus Resident Evil 2 übernehmen, nicht das Modell. Das
 * Modell darf schon noch das aus Resident Evil 1.5 sein...."
 *
 * WELCHES MODELL GEHOERT ZU DEN ARMEN (gemessen):
 *   - Typ 0x1A in ROOM1210/1211: zehn Sce_em_set `44 NN 1a 00 00 00 ..` (Mod ROOM1210 @0x1D86..0x1E3A, Disc @0x1D3E..
 *     0x1DF2), Record-y = 0 bei allen zehn (Record +10, z.B. Mod @0x1D90 `00 00`).
 *   - Modell = EXE-TOC @0x80072f38 (Leon, CDEMD0) / @0x80073178 (Elza, CDEMD1), Eintrag (0x1A-0x10)*8: Disc Sektor
 *     0x27D, Mod 0x27F (CDEMD0) / 0x280 (CDEMD1), Laenge 0x12EC0 — alle vier Bloecke md5 7c315d73 (Disc == Mod,
 *     Leon == Elza). EM1A = 4 Knochen (0 Rumpf, 1 Kopf, 2/3 die zwei Arme, Eltern 0:-1 1:0 2:0 3:0), 4 Meshes,
 *     4 Clips 30/39/30/54 Bilder, TIM 256x256 (8 bpp), keine Opfer-Bank. Ein Zombie HINTER dem Gitter, dessen zwei
 *     Arme (lokal y bis 1543) durch die Stabe greifen — bodenbezogen (Rumpf-Wurzel y -1855).
 *   - Das RE1.5-Arm-Overlay (STAGE1.BIN 0x8010c1ec..0x8010d774) ist auf Disc und Mod byte-gleich (0 Unterschiede).
 *   - Bis Runde 41 zeichnete der Port statt dessen das RE2-Modell EM2D (CDEMD0.EMS kind 0x2D, 15 Knochen) — DAS ist
 *     der gemeldete Fehler.
 *
 * WAS BLEIBT (RE2): Anker (x an der Wand, y RE2ARM_1210_Y), Weckruf, A0-Tore 2500/3000, Hand-Tor 900, Halten
 * 150 - Mash, Cooldown 120, Rueckzug 30/Bild, Ende 0x701, Treffer -> 0x501, alle SEs, Trefferkasten. RUNDE 4 (Vorgabe Punkt 2,
 * NUTZERWUNSCH, Block KOERPER): die HAND des Gehirns ist die gezeichnete RE1.5-Hand, gegriffen wird nur in Leons
 * Klemmzylinder 450 (@0x80073e9a) und nur, wenn Leon an der Hand stehen kann (kontakt_moeglich); der Pin stellt Leon an die
 * RE1.5-Hand (Block HALTE-LAGE), der RE2-Koerper-Push entfaellt mit EM1A. Und der TAKT: jede Zustandsdauer, die am Clip-Ende haengt (B0 P3, B3, B5) und jede
 * Bildabfrage (`< 11` @0x80100770, `>= 5` @0x80100A18, `== 0` @0x80100CB4), laeuft weiter auf den EM2D-Laengen
 * (Paar 1: 20/25/30/15/10/19). Das gezeichnete EM1A-Bild laeuft daneben in seiner EIGENEN Geschwindigkeit: ein Bild
 * je Takt wie im RE1.5-Original (Arm-Aufruf `jal 0x8001f314` @0x8010c6f4, a3 = 0x200 @0x8010c6f8; FUN_8001f3bc zaehlt
 * +0x95 je Aufruf um 1, Decompile RE_15_Quellcode_V2/FUN_8001f3bc.c Z. 89-93),
 * Wrap an der EM1A-Laenge, Neustart bei jedem Clipwechsel — PORT-WAHL. Folge: ein Einmal-Clip wird dort abgeschnitten,
 * wo der RE2-Zustand endet (Zugriff 15 von 30 Bildern, Rueckzug 10 von 54 Bildern = der Beginn des Zuruecksinkens).
 * Gemessen verworfen: die Umrechnung bild_em1a = bild_re2 * laenge_em1a / laenge_re2 spielte das Zuruecksinken in
 * 10 statt 54 Bildern ab, die Arme lagen dann sichtbar auf dem Flurboden vor der Wand (Messbild der verworfenen
 * Fassung im Bericht Runde 42 Stufe 2, Video-Zeit 13,0 s und 23,5 s).
 *
 * CLIP-ZUORDNUNG RE2 -> EM1A (PORT-WAHL). Rollen der EM1A-Clips aus dem RE1.5-Arm-Overlay:
 *   EM1A 0 Ruhe/Wiegen      B[0] `sb zero,148(v0)` @0x8010c6bc, B[2] @0x8010c9e4
 *   EM1A 1 Wiegen (Variante) B[2] rng&1 -> `sb v1,148(v0)` @0x8010ca14
 *   EM1A 2 Ausfahren         Lunge B[1] `sb a0,148(v0)` @0x8010c818 (a0 = 2), Treffer-Zucken @0x8010d200
 *   EM1A 3 Zuruecksinken     B[4] `ori v0,zero,0x3 / sb v0,148(v1)` @0x8010ceac-b0, Tod @0x8010d520 / @0x8010d630
 * RE2-Rollen: 0/1 Ausfahren/Tasten (rng&1 @0x80100878), 3 Zugriff (@0x80100AEC), 4 Loslassen (@0x80100DD0),
 * 5 Halten (@0x80100BC8) und Warten (@0x80100EE8); Clip 2 setzt das Arm-Overlay nie (nur Leons Hook @0x80101338).
 *   0 -> 0, 1 -> 1   beide Spiele waehlen zwischen genau diesen zwei per rng&1
 *   3 -> 2           Zugriff = Ausfahren
 *   4 -> 3           Rueckzug = Zuruecksinken (RE1.5 nimmt Clip 3 selbst fuer Rueckzug und Tod)
 *   5 -> 1           Halten: RE1.5 hat KEINEN Halte-Clip (der RE1.5-Arm greift nie). Gezeichnet wird seit Runde 4 NICHT
 *                    diese Zuordnung, sondern der Koerper (Block KOERPER: Zugriff = Clip 2 Bild fuer Bild, Halten = die
 *                    Clip-2-Pose des Griffbilds steht, Rueckzug = Clip 3 ganz). Die Zuordnung fuehrt nur noch e->motion.
 *   2 -> 0           wird am Arm nie gesetzt
 *
 * WAS DAS ORIGINAL ZEIGT (GEMESSEN im echten RE1.5: DuckStation, MZD-Disc, Debug-JUMP 0x121 = ROOM1210, Leon; Lauf +
 * Bilder scratchpad original/s2r2/orig, Live-Savestates je ~0,23 s; BERICHTIGT in Runde 3 nach dem Pruefbericht Runde 2 R1):
 *   - Vor dem Ausloeser: alle zehn Typ-0x1A-Entities auf ihrem Record-x (-25000 West / -14000 Ost, y 0), Wort 0
 *     0x40000001, +0x5 = 0 (B[0]), Clip 0 — sie werden gezeichnet, stehen aber 3910 (West, Wandflaeche -21090) hinter
 *     der Wand. Im Bild ist JEDES Gitterfenster leer (Video lauf3/original_0.mp4 t 13,0 s).
 *   - Ausloeser (Aot 6, Spieler bei z -13959): A[0] @0x8010c614 sieht +0x9 & 0x1F == 1 und setzt +0x5 = 1; B[1]
 *     (Tabelle @0x80120984[1] = 0x8010c714) laeuft im selben Bild an und schiebt den Zombie nach vorn (KOERPER-WEG unten).
 *     Danach stehen sie bei x -22580 (West) / -16358 (Ost) und wiegen sich in B[2]/B[3].
 *   - Im Bild stehen danach ZWEI ZOMBIE-GESTALTEN HINTER JEDEM GITTER (dunkelrote Haare, helles Hemd, Blut; Video t 15,4
 *     bis 30 s, Messbilder pruef_s2_r2/fr/orig_kopf3.png, vergleich_fenster.png); ihre Arme ragen durch die Staebe.
 *     ⛔ Die Aussage aus Runde 2 "Rumpf und Kopf in keinem einzigen Bild" war FALSCH (Pruefbericht Runde 2 R1).
 *     Speicher passt: s4/s5 auf x -22580, Rumpf T0 y -1969, Kopf T1 (-22672,-2645) (lauf4/p030.sav, teile.py) — nur
 *     ~430 hinter der Zellenseite der Wand (Wand x -22150..-21090).
 *   - Die RE1.5-Arme: Handgelenk-Ring lokal y ~1215, Fingerspitzen y bis 1543 (EM1A Mesh 2/3). Der RE1.5-Arm greift im
 *     Original nie (kein Griff im Overlay, Runde 1). In B[3] laesst das Original die Arme aber WACHSEN (Block KOERPER,
 *     gemessen lauf4: Part 2 +0x8A bis 5398 = 1,32-fach in den Proben, hoechstens 2,04-fach) - ihre Hand reicht dann in den
 *     Flur. (Runde 3 streckte den Arm statt dessen selbst zur RE2-Hand - entfernt in Runde 4.)
 *
 * KOERPER-WEG B[1] (STAGE1.BIN, Disc == Mod byte-gleich; selbst disassembliert, re15_disasm.py dis 0x8010c714):
 *   Schritt = FUN_800245d8(yaw_ofs) (x += cos(yaw+ofs) * +0x8C, z -= sin * +0x8C; Katalog FUN_800245d8), danach im Root
 *   die Wandklemme FUN_8003b0a4(+0x34, *(+0x78)+6, 4) @0x8010c320 (a2 = 4 @0x8010c318, Radius `lhu a1,6(v0)` @0x8010c31c
 *   = 300: *(+0x78) = *(0x80120934) = 0x8012091c @0x8010c3bc, Halbwort @0x80120922 = `2c 01`).
 *     Bild 0..2  : Clip 0 (`sb zero,148` @0x8010c778), +0x8C = 0x320 = 800 @0x8010c7b4, +0x9C = 3 @0x8010c7a4,
 *                  vorwaerts (a0 = 0 @0x8010c914)                                                  -> +800 je Bild
 *     Bild 3     : +0x9C == 0 -> Clip 2 (`sb a0,148` a0 = 2 @0x8010c818), +0x8C = 0x14 @0x8010c7f4,
 *                  +0x9C = 0x1E @0x8010c804, vorwaerts (`j 0x8010c8f8` @0x8010c828)                -> +20
 *     Bild 4..33 : +0x9C 30..1, rueckwaerts (a0 = 0x800 im Delay-Slot @0x8010c8b4)               -> -20 je Bild
 *     Bild 34    : +0x9C == 0 -> Clip 0 (@0x8010c880), +0x8C = 0xC8 @0x8010c85c, +0x9C = 3 @0x8010c86c,
 *                  rueckwaerts (@0x8010c8b4)                                                       -> -200
 *     Bild 35..38: vorwaerts (@0x8010c8f8/@0x8010c914), in Bild 38 +0x5 = 2/3 per rng (@0x8010c8e0-e4) -> +200 je Bild
 *   Summe +2420: West -25000 + 2420 = -22580 = gemessen. Ost -14000 - 2420 = -16420; gemessen -16358 — die Wandklemme
 *   (Radius 300) schiebt den Ost-Zombie zurueck (Port: re15_collision_constrain_enemy, Wert im Riegel K2).
 *   Bildzaehler: +0x95 wird nur in Sub 0 genullt (@0x8010c788), anim_set (0x8001f314, a3 = 0x200) zaehlt +1 je Aufruf
 *   (RE_15_Quellcode_V2/FUN_8001f3bc.c Z. 89-93); gemessen Bild 24 = af 25 (lauf1/proben.txt t 48,62 s).
 *
 * ZEICHNEN (Runde 3, in Runde 4 umgebaut - massgeblich ist der Block KOERPER; PORT-WAHL, wo nicht anders belegt):
 *   - Der KOERPER (Rumpf, Kopf und der zweite Arm) steht dort, wo ihn das Original zeigt: Record-Lage (vor dem
 *     Anker gemerkt) + KOERPER-WEG B[1] ab dem Ausloeser (+0x9 & 0x1F == 1 — derselbe, der auch das RE2-Gehirn weckt),
 *     Blick = Record-Blick, Hoehe = Record-y (bodenbezogenes EM1A). Vor dem Ausloeser wird nichts gezeichnet (PORT-WAHL:
 *     das Original zeichnet ihn 3910 hinter der Wand, im Bild ist das Fenster leer).
 *   - Pose des Koerpers: immer die eigene RE1.5-Pose des Koerpers (Weg B[1], Wiegen B[2], B[3]; im Griff Clip 2 /
 *     Clip 3), Block KOERPER. Der Port dreht oder streckt kein Teil selbst; die Arme tragen genau die Skalierung des Originals (B[3], Runde 6).
 *   - Kein Schatten: der RE1.5-Typ-0x1A-Code ruft den Schatten 0x8001b064 nie (kein `jal 0x8001b064` in
 *     0x8010c1ec..0x8010d7cc, re15_disasm.py), anders als der RE2-Arm (`jal 0x80016480` @0x801003A0).
 *
 * HAND (Pin-Quelle @0x80100C18-38, Tore @0x801007C4 / @0x801009BC): seit Runde 4 mit EM1A die RE1.5-Hand (koerper_hand,
 * arm_hand_pose). Der folgende Stand gilt nur noch fuer Parts/Blut der Logik-Bank: mit geladener
 * EM2D-Logik-Bank rechnen die Parts (0x80029614) auf EM2D, Clip/Bild = RE2-Takt, Hand = EM2D-Handknochen 3 (Arm A) /
 * 10 (Arm B) wie bis Runde 41 (Tabelle @0x80101414, [var*7 + 2] @0x801007DC / @0x801009D4 / @0x80100BEC). Ohne
 * RE2-Archiv (keine Logik-Bank) misst das Gehirn am EM1A-Handgelenk des Arms am RE2-Anker; gezeichnet wird auch dann
 * der Koerper an der RE1.5-Stelle und der Arm zur Gehirn-Hand. */
static const re15_emd_animation_t *s_logik_anim = NULL;   /* EM2D Paar 1 (RE2-Takt) */
static const re15_enemy_bank_t    *s_logik_bank = NULL;   /* EM2D ganz (Parts, Hand) — Runde 2 */
static const uint8_t k_em1a_clip[6] = { 0, 1, 0, 2, 3, 1 };
#define RE2ARM_EM1A_KNOCHEN   4      /* EM1A: 4 Knochen (em_dump, s.o.); EM2D: 15 */
#define RE2ARM_EM1A_RING_V0   4      /* Handgelenk-Ring = Quad-Vertices 4..7 von Mesh 2/3 */
#define RE2ARM_EM1A_RING_N    4
/* KOERPER-WEG B[1] (s. MODELL): Schritt je Bild ab dem Ausloeser, + = vorwaerts (yaw_ofs 0), - = rueckwaerts (0x800). */
#define RE2ARM_B1_BILDER      39
static const int16_t k_b1_schritt[RE2ARM_B1_BILDER] = {
    800, 800, 800,                    /* 0x320 @0x8010c7b4, +0x9C = 3 @0x8010c7a4, a0 = 0 @0x8010c914           */
    20,                               /* 0x14 @0x8010c7f4, `j 0x8010c8f8` @0x8010c828                            */
    -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20,   /* +0x9C = 0x1E @0x8010c804,   */
    -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -20,   /* a0 = 0x800 @0x8010c8b4      */
    -200,                             /* 0xC8 @0x8010c85c, rueckwaerts @0x8010c8b4                                 */
    200, 200, 200, 200                /* +0x9C = 3 @0x8010c86c, vorwaerts @0x8010c8f8 / @0x8010c914              */
};
/* B[1]-Clip je Bild: 0 bis Bild 2 (@0x8010c778), 2 ab Bild 3 (@0x8010c818), 0 ab Bild 34 (@0x8010c880). */
static int b1_clip(int bild) { return (bild >= 3 && bild < 34) ? 2 : 0; }
/* RNG des Koerpers = die Disc-Formel FUN_8001af20 (Version_40%/PSX.EXE, selbst gelesen): `lhu a0,0(v0)` @0x8001af28 (s =
 * *(u16*)0x800AC774), `srl v1,a0,7 / andi v1,0xff` @0x8001af30-34, `addu a0,a0,v1 / andi a0,0xff` @0x8001af38-3c,
 * `sll v1,v1,8 / or a0,a0,v1 / sw a0` @0x8001af40-48, Rueckgabe a0 & 0xff @0x8001af4c; Startwert 0x1c3 je Raum
 * (`ori v0,zero,0x1c3` @0x8003162c, `sw v0,-14476(at)` @0x80031634). PORT-WAHL: ein EIGENER Strom fuer die zehn Koerper
 * (das Original teilt ihn mit allem seit dem Raumstart) - so bleibt jeder andere Wurf im Raum unveraendert; die Werte
 * sind deshalb nicht die des Originals, die Formel und der Startwert schon. */
static uint16_t s_rng15 = 0x1c3;
static int rng15(void)
{
    const uint16_t s = s_rng15;
    const uint16_t v1 = (uint16_t)((s >> 7) & 0xffu);
    const uint16_t a0 = (uint16_t)((s + v1) & 0xffu);
    s_rng15 = (uint16_t)((v1 << 8) | a0);
    return (int)a0;
}
#define RE2ARM_KOERPER_RADIUS 300    /* *(0x80120934) = 0x8012091c (@0x8010c3bc), Halbwort +6 @0x80120922 `2c 01`,
                                      * `lhu a1,6(v0)` @0x8010c31c -> FUN_8003b0a4 @0x8010c320 */
#define RE2ARM_KOERPER_MASKE  4u     /* `ori a2,zero,0x4` @0x8010c318 */
/* Runde 5: der Zugriff zeigt den AUSFAHR-Teil von EM1A-Clip 2 ab Bild 20 (PORT-WAHL). Gemessen an der Disc-EM1A
 * (scratchpad original/s2r5/probe/reich.c, Hand = Mittel der Quad-Vertices 0..11, Teil 2 lokal x): Bild 20 1349 (Arm
 * eingezogen) -> 25 1418 -> 29 = Bild 0 1455 (am weitesten) -> 4 1411 -> ab 5 zieht der Arm wieder ein (6..19: 1344..1400).
 * Ab Bild 20 liegt die weiteste Hand genau im Greif-Fenster des RE2-Gehirns (A3 ab RE2-Bild 5, `sltiu 5` @0x80100A18).
 * Runde 6: der Koerper tritt dabei NICHT mehr vor (das Vortreten an die Wandklemme aus Runde 5 ist entfernt) - die Reichweite
 * kommt allein aus der Original-Bewegung mit dem Wachsen B[3] (Block KOERPER). */
#define RE2ARM_GRIFF_BILD0    20

/* ---- ROOM1210-ANKER (Port-Bruecke, am Original-Hintergrund GEMESSEN) ---------------------- */
/* SCA-Zellen ROOM1210 (Band 0): [6] x -22150..-21090 = Westwand, [11] x -17696..-16676 = Ostwand
 * (Sonde Teil B). Die Wandflaeche zum Flur ist -21090 (West) bzw. -16676 (Ost).
 *
 * MESSUNG AM BILD (Phase 2, 2026-09-19; Messprogramme + Zahlen in Dossier-Abschnitt 6,
 * Bilder analysis/befunde_2026-09-19/phase2_arme-1210-re2/anker_*.png):
 *  - Ruecklauf der Kamera (re15_camera_build_view = FUN_80053ca4, Projektion sx=160+H*x/z mit
 *    H = fov>>7 = 208) auf den EXTRAHIERTEN Originalhintergrund ROOM12104.bmp (Cut 4,
 *    pos (-18522,-1962,-24048), Blick nach Norden): die Pixel der Fensterbank des westlichen
 *    Gitterfensters (v = 104 bei u = 55 / 75 / 95) liegen auf der Wandebene x = -21090 bei
 *    y = -2040 / -2057 / -2082 — eine WAAGERECHTE Kante, damit ist zugleich belegt, dass die
 *    Kunst die Wandflaeche -21090 meint. Fensterbank ROOM1210 (West, Cut 4) = y ~ -2060.
 *    Dasselbe Fenster reicht in z von -16862 (rechte Kante, u 95) bis -19428 (linke Kante, u 55).
 *  - Cut 3 (ROOM12103.bmp, pos (-19008,-3132,-14274)) zeigt das noerdliche Westfenster; dessen
 *    Bank liegt bei y ~ -2310 (Pixelkante v ~ 114 bei u 78, zwischen den Vorwaertsproben
 *    y -2000 -> v 128 und y -2513 -> v 105 interpoliert).
 *  - RE2 haelt seine zehn Ursprünge 1930..2700 ueber dem Boden (Records ROOM2050 @0x1970..0x1A36)
 *    und 298..523 hinter der Wandflaeche (collision.sca, Dossier 2.6). Projiziert man diese zehn
 *    Punkte mit derselben Mathematik in den RE2-Hintergrund ROOM20509.bmp (Cut 9 = der Cut, den
 *    sub03 vor dem Wecken schaltet), landen alle fuenf Westarme IN den dunklen Oeffnungen der
 *    vernagelten Fensterwand (Bild re2_2050_cut9_arme.png) — die RE2-Regel lautet also
 *    "Ursprung im Fensterloch", nicht "Ursprung auf einer festen Hoehe".
 * DARAUS die beiden Portwerte:
 *  - RE2ARM_1210_HINTER = 400: liegt im gemessenen RE2-Band 298..523 hinter der Flaeche.
 *  - RE2ARM_1210_Y = -2500: liegt im gemessenen RE2-Band -1930..-2700 UND ueber beiden
 *    gemessenen ROOM1210-Fensterbaenken (-2060 / -2310), der Ursprung sitzt damit im
 *    Fensterloch und nicht im Mauerwerk darunter. Gegenprobe mit der geladenen EM2D-Bank
 *    (alle 6 Clips, Bones 1..7): Bone-Welt-y -2960..-1865, Bildrechteck Cut 4 u 68..120 /
 *    v 77..111 gegen die Fensteroeffnung u 55..95 / v <=104 — bei y = -2000 (Stand vor der
 *    Messung) lagen 635 Einheiten des Arms UNTER der Bank, also im Mauerwerk.
 * OFFEN (Dossier 6.3, nicht geraten, sondern gemessen): die z-Werte der zehn ROOM1210-Records
 * treffen die Fensteroeffnungen der Kunst nur teilweise (West: Fenster z -16862..-19428 gegen
 * Arme z -15747/-17130; noerdliches Fenster z ~ -9875..-11757 gegen Arme -8847/-10247). Die
 * Record-z sind Daten und werden NICHT verschoben. */
#define RE2ARM_1210_WEST_FLAECHE  (-21090)
#define RE2ARM_1210_OST_FLAECHE   (-16676)
#define RE2ARM_1210_HINTER        400
#define RE2ARM_1210_Y             (-2500)
static void arm_anker_1210(re15_actor_t *e)
{
    int hinter = RE2ARM_1210_HINTER, y = RE2ARM_1210_Y;
    const char *m = getenv("RE15_ARM_ANKER");            /* Mess-Haken (s.o.) */
    if (m && *m) { int h2 = hinter, y2 = y; if (sscanf(m, "%d,%d", &h2, &y2) >= 1) { hinter = h2; y = y2; } }
    if (g_current_room_id != 0x1210u && g_current_room_id != 0x1211u) return;
    if (((int)e->rot_y & 0xfff) == 0)          e->x = RE2ARM_1210_WEST_FLAECHE - hinter;  /* Westreihe, Blick +x */
    else if (((int)e->rot_y & 0xfff) == 2048)  e->x = RE2ARM_1210_OST_FLAECHE  + hinter;  /* Ostreihe, Blick -x  */
    else return;
    e->y = y;
}

/* ---- KOERPER-PUSH Arm -> Spieler (Runde 35 Spur H, Nachbesserung 3) -------------------------
 * GEMESSEN am RE2-Original (DuckStation-GDB, ROOM2050, Dossier H_raeume.md Nachbesserung 3): nach dem
 * Pin @0x80100C18-38 (Schreib-Wachpunkt auf PL+0x38/+0x40; Schreiber `sw` @0x80100c20 / @0x80100c38 — GDB meldet
 * den PC der FOLGE-Instruktion, 0x80100c24/0x80100c3c) schreibt im SELBEN Bild FUN_80034D0C Leons x/z
 * (`sw v0,56(s1)` @0x800350c8 / `sw v1,64(s1)` @0x800350cc; GDB-PC 0x800350cc/0x800350d0). Leon steht danach im
 * ersten Halte-Bild 1251 (g5/g6/g7/g8/g11), 1252 (g9/g12/g13) bzw. 1255 (g1/g2) vom Ursprung des Halters
 * (r 800 + 450 + Rundung), nicht auf der Hand. g3/g4 (1295 vom Halter Satz 2) sind verfaelscht: dort schob der
 * weiter sichtbare Nachbar Satz 0 mit — 1251 von DESSEN Ursprung (Abnahme 3 M3, Dossier Nachbesserung 4).
 * Der Port liess Leon auf der Hand (~1050 vom Ursprung): Unterarm ~200 tiefer in Leon als im Original.
 * Kette (RE2 PSX.EXE, selbst disassembliert):
 *   Spieler-Pass FUN_800355C4 (@0x80026628, nach dem Spieler-Tick): fuer jeden Listeneintrag
 *     0x800CFE14..*(0x800CE334) mit word0 & 1: `jal 0x80034d0c` (Pusher, Spieler) @0x80035630;
 *     Rueckgabe -> Kontaktbit (0x8000 >> Listenindex) nach PL+0x0E (`sh s3,14(s1)` @0x80035658) —
 *     im RAM gemessen: PL+0x0E = 0x100 / PL+0x0D = 7 (Pusher-Id = Satz 5 + 2) im Griff-Bild.
 *   FUN_80034D0C (Decompile RE2_Quellcode_V2/FUN_80034d0c.c): Ausstieg bei (wPusher | wSpieler) & 2
 *     (verborgener Arm), wPusher & wSpieler & 0x1000, wSpieler & 4; Breitphase |dx|,|dz| <= R,
 *     dist = SquareRoot0, pen = R - dist > 0, Hoehenband |dy| < hA + hB, Schub (dx,dz) * pen / (dist+1);
 *     Vorzeichen-Dreh-Zweig gegen den Positionsspiegel (wie re15_g5_body_push_player); die 0x100000-
 *     Klemme prueft wPusher — das Arm-Overlay setzt das Bit nie (kein `lui ..,0x10` in
 *     CDEMD0_EM2D_ai1.BIN), Arm-word0 im RAM 0x0c003405.
 *   Segment des Arms (INIT): +0x1E8 = 1 `sw v0,488(s0)` @0x80100328; +0x9E Halbhoehe 500
 *     `addiu v0,zero,500 / sh v0,158(s0)` @0x8010032C-30; +0x9A Radius 800 `addiu v1,zero,800 /
 *     sh v1,154(s0)` @0x80100338-3C; Lokal-Lage 0 (`sh zero,148/152/150(s0)` @0x8010035C/60/64).
 *   Segment des Spielers (Ctor): r 450 @0x8003bdc0-c4, Lokal-Y -1530 @0x8003bde0-e4, Halbhoehe 1530
 *     @0x8003bde8-ec (dieselben Werte wie RE15_G5_PL_*, im RAM gemessen: PL+0x9A 450, +0x98 -1530,
 *     +0x9E 1530). */
#define RE2ARM_SEG_R    800    /* +0x9A @0x80100338-3C */
#define RE2ARM_SEG_H    500    /* +0x9E @0x8010032C-30 */
#define RE2ARM_PL_R     450    /* @0x8003bdc0-c4 */
#define RE2ARM_PL_Y   (-1530)  /* @0x8003bde0-e4 */
#define RE2ARM_PL_H    1530    /* @0x8003bde8-ec */
static int arm_schiebt(const re15_actor_t *e, re15_actor_t *pl);   /* Block HALTE-LAGE */
int re15_re2arm_body_push_player(re15_actor_t *e, re15_actor_t *pl)
{
    if (!e || !pl || !pl->active || pl->hp < 0) return 0;   /* hp < 0: Port-Konvention aller Schub-Paesse
                                                              * (re15_body_push_player / re15_g5_body_push_player) */
    const int slot = (int)(e - g_actors);
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    /* Runde 42 Stufe 2 Runde 4: mit dem RE1.5-Modell gibt es den RE2-Arm (Segment r 800 am RE2-Ursprung) nicht mehr zu sehen -
     * kein RE2-Segment-Schub (er wuerde die Figur 1251 vom RE2-Ursprung halten, weg von der RE1.5-Hand). Runde 7 (Pruefbericht
     * Runde 6 R1: die Figur lief in den gewachsenen Arm, die Hand steckte 8 Bilder in ihr): PORT-WAHL - statt des RE2-Segments
     * ist der GEZEICHNETE RE1.5-Arm das Hindernis (arm_schiebt, Block HALTE-LAGE), solange der Koerper zu sehen ist und die
     * Figur nicht gehalten wird. Dieselbe Aufgabe wie das RE2-Segment: kein Arm steckt in der Figur, ohne zu greifen. */
    if (re15_re2arm_modell_re15(e)) {
        if (!s_arm[slot].koerper_an || re15_player_is_grabbed() || (pl->re2z_self1d3 & 0x80u)) return 0;
        return arm_schiebt(e, pl);
    }
    if (s_arm[slot].hidden) return 0;                                         /* (w|w) & 2 */
    const int32_t rs = RE2ARM_SEG_R + RE2ARM_PL_R;
    const int32_t dx = pl->x - e->x, dz = pl->z - e->z;
    if ((uint32_t)(dx + rs) > (uint32_t)(rs * 2)) return 0;                   /* Breitphase X */
    if ((uint32_t)(dz + rs) > (uint32_t)(rs * 2)) return 0;                   /* Breitphase Z */
    const int32_t dist = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
    const int32_t over = rs - dist;
    if (over <= 0) return 0;                                                  /* `if (0 < iVar10)` */
    const int32_t hs = RE2ARM_SEG_H + RE2ARM_PL_H;
    const int32_t dy = (pl->y + RE2ARM_PL_Y) - e->y;
    if (!(-hs < dy && dy < hs)) return 0;
    const int32_t d1 = dist + 1;
    int32_t px = (dx * over) / d1, pz = (dz * over) / d1;
    {   /* Vorzeichen-Dreh-Zweig (Decompile Z.45-70): Hoehe aus dem Positionsspiegel des Spielers */
        const int32_t h2 = (int32_t)(int16_t)pl->pos_s_y + RE2ARM_PL_Y - e->y;
        if (h2 <= -hs || hs <= h2) {
            const int32_t mx = (int32_t)(int16_t)pl->pos_s_x, mz = (int32_t)(int16_t)pl->pos_s_z;
            const int32_t r2 = RE2ARM_SEG_R * 2;
            if ((mx < e->x && e->x < pl->x) || (e->x < mx && pl->x < e->x)) {
                int32_t g = (-(dx * over)) / d1; px = ((g > 0) ? r2 : -r2) - g;
            }
            if ((mz < e->z && e->z < pl->z) || (e->z < mz && pl->z < e->z)) {
                int32_t g = (-(dz * over)) / d1; pz = ((g > 0) ? r2 : -r2) - g;
            }
        }
    }
    pl->x += px;
    pl->z += pz;
    return 1;
}
int re15_re2arm_take_pin_bild(void) { int r = s_pin_bild; s_pin_bild = 0; return r; }

/* ---- LEONS BLICK IM GRIFF (Runde 35 Spur H, Nachbesserung 4) ----------------------------------
 * GEMESSEN im RE2-Original-RAM (12 Griffe x 40 Halte-Bilder, analysis/.../re2_mess/re2_neck.py,
 * Dossier H_raeume.md "Nachbesserung 4"): PL+0x1C0 = 0 in jedem Bild (die Nacken-FSM laeuft auch im
 * Griff, niemand sperrt sie); im Ruecken-Griff (g2/g6/g8) Ziel PL+0x1B8 = SELBST und Akku Part8
 * +0x98/+0x9A = (0,0) in allen 40 Bildern -> Kopf = Animationspose; im Gesicht-Griff SELBST bis zur
 * ersten Suche, dann der naechste Arm VOR Leon (Bild 15..31, Zielpunkt = Arm-Ursprung).
 * MECHANISMUS (RE2 PSX.EXE, selbst disassembliert):
 *   FUN_8003BFAC ruft NACH dem Zustands-Dispatch (`jalr v0` @0x8003c1a4) in JEDEM Spielerzustand
 *   FUN_8003DB38(PL, 7000, 1500) @0x8003c1ac-b8 und die Nacken-FSM FUN_800177C0 @0x8003c1c0. Die
 *   Opfer-Routine 5 (0x8004006C: |= 0x40 auf 0x800CFBD8, word0 &= ~4, Hook des Halters) und das
 *   Arm-Overlay (schreibt +0x1C0/+0x1C1 nur am EIGENEN Eintrag @0x801001a8/@0x801001d0) fassen
 *   +0x1B8/+0x1C0 des Spielers nicht an; die Spieler-INIT setzt +0x1B8 = SELBST `sw s1,440(s1)`
 *   @0x8003c250, +0x1C1 = 8 @0x8003c268, +0x1C0 = 0 `sb zero,448(s1)` @0x8003c270, und die
 *   Raum-INIT FUN_80049E48 schickt den Spieler dorthin (`sw zero,4(s2)` @0x80049ffc).
 *   FUN_8003DB38: +0x1C0 & 1 -> aus (@0x8003db78-84, VOR dem Zaehler); Zaehler DAT_800a4004 -1, Suche
 *   nur bei altem Wert 0, dann 45 (@0x8003db90-a4 / `addiu v0,zero,45` @0x8003dba8 / sb @0x8003dbb8;
 *   .data 0x2D, einziger Schreiber); Tor DAT_800cfbf3 (em_set-Zaehler, ++ @0x80057218-24) oder
 *   0x800cfbd8 & 0x10000000 (@0x8003dbac-d4) — im Port erfuellt, sobald es einen Kandidaten gibt (jeder
 *   Port-Aktor ab Slot 1 stammt aus Sce_em_set); Liste word0 & 1 (@0x8003dbf4), Ausschluss
 *   +0x10E & 0xC000 (@0x8003dc08), Sicht 0x80050858(Kopf-Part, Ziel-Part, 0x2080, 1) @0x8003dc10-78,
 *   Kegel FUN_80015614(PL, E.x, E.z, 1500) @0x8003dc84-94, Abstand +0x1F0 (@0x8003dc24); Klasse A
 *   (+0x10E & 0x2000 = 0, @0x8003dca4) streng < 7000 (`sltu v0,s1,s4` @0x8003dcb0), Klasse B < 0x7fffffff
 *   (@0x8003dcac); Speicher A @0x8003dcfc, B @0x8003dd0c, sonst SELBST `sw s3,440(s3)` @0x8003dd10.
 * PORT: Leons Blick laeuft sonst ueber die RE1.5-Zielwahl (Prolog des cmd-1-Handlers @0x80031e04-40,
 * game_step_common.c); im Opfer-Zustand laeuft der Prolog nicht und das zuletzt gewaehlte RE1.5-Ziel
 * (der Arm HINTER Leon) blieb stehen -> Klemme -512 im Ruecken-Griff (Abnahme 3, neck.log). RE1.5 hat
 * diesen Griff nicht (Writher ohne Griff, NB2 M1) -> im Opfer-Zustand des RE2-Arms gilt die RE2-Wahl.
 * Die RE2-Suche laeuft dafuer frei mit (wie im Original jedes Bild), verbraucht wird sie nur dort.
 * PORT-MAPPINGS (benannt): Sicht = re15_re2_los_clear (RE1.5-Region-Ray, Praezedenz Kraehe/Zombie —
 * RE1.5-Raeume haben keine RE2-Saetze mit Attribut 0x2080); Schlaf-Bit 0x8000 eines Arms = die Port-
 * Abbildung "+0x9 & 0x1F != 1" (s. Kopf, wie im Tick @0x80026590-9C). */
#define RE2LOOK_ZAEHLER   45        /* `addiu v0,zero,45` @0x8003dba8, `sb` @0x8003dbb8; .data 0x2D */
#define RE2LOOK_RADIUS    7000u     /* `addiu a1,zero,7000` @0x8003c1b0 */
#define RE2LOOK_KEGEL     1500      /* `addiu a2,zero,1500` @0x8003c1b8 */
#define RE2LOOK_AUS       0xC000u   /* `andi v0,v0,0xc000` @0x8003dc08 */
#define RE2LOOK_KLASSE_B  0x2000u   /* `andi v0,v0,0x2000` @0x8003dca4 */
static uint8_t  s_look_cd   = RE2LOOK_ZAEHLER;           /* DAT_800a4004 (laeuft frei, kein Raum-Reset) */
static int8_t   s_look_tgt  = RE15_ACTOR_SLOT_PLAYER;    /* PL+0x1B8; SELBST = Spieler-Slot          */
static unsigned s_look_room = 0xFFFFFFFFu;

/* Die Wahl selbst (Schleife @0x8003dbe4-0x8003dcec + Speicher @0x8003dcf0-0x8003dd10) als reine Funktion ueber
 * eine Kandidatenliste — re2look_suche speist sie aus g_actors, der Riegel (N1) mit der RE2-Original-RAM.
 * Je Kandidat: aktiv (word0 & 1), +0x10E, Sicht frei (Ergebnis von 0x80050858), Lage x/z (+0x38/+0x40, fuer
 * Kegel und Abstand). Rueckgabe: Index des Ziels oder -1 = SELBST. */
int re15_re2arm_look_waehle(const re15_actor_t *pl, int n, const re2look_kand_t *k)
{
    uint32_t best_a = RE2LOOK_RADIUS, best_b = 0x7fffffffu;               /* a1 = 7000 / s5 = 0x7fffffff */
    int ziel_a = -1, ziel_b = -1;
    for (int i = 0; i < n; i++) {
        if (!k[i].aktiv) continue;                                        /* word0 & 1 @0x8003dbf4 */
        if (k[i].f10e & RE2LOOK_AUS) continue;                            /* +0x10E & 0xC000 @0x8003dc08 */
        if (!k[i].sicht_frei) continue;                                   /* 0x80050858 != 0 @0x8003dc78 */
        if (re15_ai_arc_test(pl, k[i].x, k[i].z, RE2LOOK_KEGEL) != 0) continue;   /* FUN_80015614 @0x8003dc94 */
        const int32_t dx = k[i].x - pl->x, dz = k[i].z - pl->z;           /* +0x1F0 @0x800265A4-E0 */
        const uint32_t d = re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz));
        if (k[i].f10e & RE2LOOK_KLASSE_B) { if (d < best_b) { best_b = d; ziel_b = i; } }   /* @0x8003dcc8 */
        else                              { if (d < best_a) { best_a = d; ziel_a = i; } }   /* @0x8003dcb0 */
    }
    return ziel_a >= 0 ? ziel_a : ziel_b;                                 /* @0x8003dcf0 / @0x8003dd00 */
}

static void re2look_suche(re15_actor_t *pl)            /* Rumpf von FUN_8003DB38 nach dem Zaehler */
{
    re2look_kand_t k[RE15_ACTOR_MAX];
    int slot[RE15_ACTOR_MAX], n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active) continue;
        const int arm = (e->type == 0x1Au && re15_re2arm_owns(e));
        k[n].aktiv = 1;
        /* +0x10E: der Arm-Schlaf 0x8000 ist im Port "+0x9 & 0x1F != 1" (Kopf, PORT-BRUECKEN). */
        k[n].f10e  = (uint16_t)(arm ? (((e->grid_id & 0x1Fu) != 1u) ? 0x8000u : 0u) : e->re2z_f10e);
        k[n].x = e->x; k[n].z = e->z;
        /* Sicht 0x80050858(Kopf-Part, Ziel-Part +0x1C1 +0x5C, 0x2080, 1): Zielpunkt = gezeichnete Part-Lage — beim
         * gezeichneten Arm sein Ursprung (RAM: Halter k1 = Ursprung), beim NIE gezeichneten Arm (0,0,0) (RAM, s.o.;
         * re2_los.py: von jeder gemessenen Leon-Lage ROOM2050 verdeckt). Strahl = PORT-MAPPING re15_re2_los_clear. */
        if (arm && !s_arm[i].gezeichnet) {
            re15_actor_t t = *e; t.x = 0; t.y = 0; t.z = 0;
            k[n].sicht_frei = (uint8_t)re15_re2_los_clear(&t, pl);
        } else {
            k[n].sicht_frei = (uint8_t)re15_re2_los_clear(e, pl);
        }
        slot[n++] = i;
    }
    const int w = re15_re2arm_look_waehle(pl, n, k);
    s_look_tgt = (int8_t)(w >= 0 ? slot[w] : RE15_ACTOR_SLOT_PLAYER);
}

void re15_re2arm_player_look(re15_actor_t *pl)
{
    if (!pl) return;
    if (g_re15_pauseflags & RE15_PAUSE_PLAYER) return;  /* RE2 `bltz v0` @0x8003bfc0: Spieler-Main aus */
    if (s_look_room != g_current_room_id) {             /* Raum-INIT -> Spieler-INIT: +0x1B8 = SELBST */
        s_look_room = g_current_room_id;
        s_look_tgt  = RE15_ACTOR_SLOT_PLAYER;           /* @0x80049ffc -> @0x8003c250 */
    }
    if (!(pl->neck_flags & 0x01u)) {                    /* @0x8003db78-84 */
        const uint8_t alt = s_look_cd;
        s_look_cd = (uint8_t)(alt - 1u);                /* @0x8003db98-a0 */
        if (alt == 0u) { s_look_cd = RE2LOOK_ZAEHLER; re2look_suche(pl); }
    }
    /* Verbrauch NUR im Opfer-Zustand des RE2-Arms (Routine 5 / Hook laeuft, s_holder gueltig). */
    if (s_holder >= 0 && re15_player_victim_state() == 4) {
        pl->neck_flags       = 0x00u;                   /* +0x1C0 = 0 (INIT @0x8003c270, gemessen fl=00) */
        pl->neck_target_slot = s_look_tgt;              /* PL+0x1B8 -> FUN_800177C0 */
    }
}
/* Test-/Mess-Auskunft (kein getenv, das Spiel ruft das nie): Zaehler (set_cd >= 0) und Ziel (set_ziel >= 0,
 * 0 = SELBST) setzen — damit stellt der Riegel die Ausgangslage der Original-Mitschnitte her (Ziel SELBST im
 * ersten Halte-Bild, erste Suche im gemessenen Bild) — und Zaehler/Ziel lesen. */
void re15_re2arm_look_debug(int set_cd, int set_ziel, int *cd, int *ziel)
{
    if (set_cd >= 0) s_look_cd = (uint8_t)set_cd;
    if (set_ziel >= 0 && set_ziel < RE15_ACTOR_MAX) s_look_tgt = (int8_t)set_ziel;
    if (cd)   *cd   = (int)s_look_cd;
    if (ziel) *ziel = (int)s_look_tgt;
}

/* ---- kleine Helfer ------------------------------------------------------------------------ */
int re15_re2arm_owns(const re15_actor_t *e)
{
    return e && e->active && e->type == 0x1Au && re15_ai_re2_for_type(0x1Au);
}
int re15_re2arm_holder_slot(void) { return s_holder; }

/* ---- MODELL-Helfer (Runde 42 Stufe 2, Belege im Block MODELL oben) ---------------------------------------------- */
int re15_re2arm_modell_re15(const re15_actor_t *e)
{
    const re15_enemy_bank_t *b = e ? re15_enemy_find(e->type) : NULL;
    return b && b->skel.bone_count == RE2ARM_EM1A_KNOCHEN;
}
int re15_re2arm_em1a_clip(int re2_clip)
{
    return (re2_clip >= 0 && re2_clip < (int)sizeof k_em1a_clip) ? (int)k_em1a_clip[re2_clip] : -1;
}
static int arm_slot(const re15_actor_t *e)
{
    int s = (int)(e - g_actors);
    return (s >= 0 && s < RE15_ACTOR_MAX) ? s : 0;
}
/* 1 = EM1A wird gezeichnet UND die RE2-Logik rechnet auf der EM2D-Bank (Parts, Hand, Blut-Knochen) — s. HAND. */
static int logik_em2d(const re15_actor_t *e)
{
    return s_logik_bank && s_logik_bank->skel.bone_count > RE2ARM_EM1A_KNOCHEN && re15_re2arm_modell_re15(e);
}
/* Gezeichneter EM1A-Arm dieses RE2-Arms: Arm A (+0x10E Bit 0 = 0) -> Teil 2, Arm B -> Teil 3 (s. ZEICHNEN). */
static int em1a_arm(const re15_actor_t *e) { return (e->re2z_f10e & 1u) ? 3 : 2; }
static int koerper_hand(const re15_actor_t *e, const re2arm_t *a, int32_t hand[3], int32_t schulter[3]);   /* s. KOERPER */
/* Reine Keyframe-Pose der EM2D-Logik-Bank fuer clip/bild -> Welt-Lage von `knochen` (wie re15_enemy_bone_world_pos auf
 * der EM2D-Bank bis Runde 41). 0 = nicht berechenbar. */
static int logik_knochen_rein(const re15_actor_t *e, int clip, int bild, int knochen, int32_t out[3])
{
    static re15_actor_t sh;
    const re15_enemy_bank_t *b = s_logik_bank;
    if (!b || clip < 0 || clip >= b->anim.clip_count || knochen < 0 || knochen >= b->skel.bone_count) return 0;
    sh = *e; sh.motion = (int16_t)clip; sh.anim_frame = (uint16_t)bild;
    const int kf = re15_compute_actor_kf(&b->anim, &b->skel, &sh, -1, (uint32_t)bild);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    const int rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_skel_bone_to_world(poses[knochen].trans, e->rot_y, e->x, e->y, e->z, out);
    return 1;
}
/* Laenge des gezeichneten EM1A-Clips (Bank des Typs). */
static int em1a_len(const re15_actor_t *e, int em1a_clip)
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || em1a_clip < 0 || em1a_clip >= b->anim.clip_count) return 0;
    return b->anim.clips[em1a_clip].frame_count;
}
/* Laenge des RE2-Clips im RE2-Takt: EM2D Paar 1, sonst (ohne RE2-Archiv) die EM1A-Laenge des zugeordneten Clips. */
static int logik_len(const re15_actor_t *e, int re2_clip)
{
    if (s_logik_anim && re2_clip >= 0 && re2_clip < s_logik_anim->clip_count)
        return s_logik_anim->clips[re2_clip].frame_count;
    return em1a_len(e, re15_re2arm_em1a_clip(re2_clip));
}
/* Naechstes gezeichnetes EM1A-Bild: +1 je Takt, Wrap an der EM1A-Laenge (PORT-WAHL, s. MODELL). */
static uint16_t em1a_bild_weiter(const re15_actor_t *e)
{
    const int l1 = em1a_len(e, (int)e->motion);
    if (l1 <= 0) return 0;
    const int f = (int)e->anim_frame + 1;
    return (uint16_t)(f < l1 ? f : 0);
}
int re15_re2arm_takt(int slot, int *re2_clip, int *re2_bild)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    if (re2_clip) *re2_clip = (int)s_arm[slot].logik_clip;
    if (re2_bild) *re2_bild = (int)s_arm[slot].logik_frame;
    return 1;
}
/* Handgelenk-Ring des EM1A-Arms `knochen` (Mesh = bone_mesh_index) im Mesh-lokalen Raum: Mittel der Quad-Vertices
 * 4..7 (s. MODELL). 0 = Mesh fehlt. */
static int em1a_ring(const re15_enemy_bank_t *b, int knochen, int32_t c[3])
{
    if (!b || knochen < 0 || knochen >= b->skel.bone_count) return 0;
    const int m = b->skel.bone_mesh_index[knochen];
    if (m < 0 || m >= b->md1.mesh_count) return 0;
    const re15_md1_mesh_t *mm = &b->md1.meshes[m];
    if (!mm->quad_vertices || mm->quad_vertex_count < RE2ARM_EM1A_RING_V0 + RE2ARM_EM1A_RING_N) return 0;
    int32_t s[3] = { 0, 0, 0 };
    for (int v = RE2ARM_EM1A_RING_V0; v < RE2ARM_EM1A_RING_V0 + RE2ARM_EM1A_RING_N; v++) {
        s[0] += mm->quad_vertices[v].x; s[1] += mm->quad_vertices[v].y; s[2] += mm->quad_vertices[v].z;
    }
    for (int k = 0; k < 3; k++) c[k] = s[k] / RE2ARM_EM1A_RING_N;
    return 1;
}
/* Ring-Punkt mit Knochen-Rotation `r` und -Lage `t` (Modellraum) -> Welt (Lage/Blick des Arms). */
static void em1a_ring_welt(const re15_actor_t *e, const int32_t c[3], const int32_t r[9], const int32_t t[3],
                           int32_t out[3])
{
    int32_t mdl[3];
    for (int k = 0; k < 3; k++)
        mdl[k] = (int32_t)(((int64_t)r[k*3+0] * c[0] + (int64_t)r[k*3+1] * c[1] + (int64_t)r[k*3+2] * c[2]) >> 12) + t[k];
    re15_skel_bone_to_world(mdl, e->rot_y, e->x, e->y, e->z, out);
}
/* Reine Keyframe-Pose (ohne +0x14E-Mischung) des EM1A fuer clip/bild -> Hand-Weltpunkt. 0 = nicht berechenbar. */
static int em1a_hand_rein(const re15_actor_t *e, int clip, int bild, int32_t out[3])
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const int hb = em1a_arm(e);
    int32_t c[3];
    if (!b || !em1a_ring(b, hb, c) || clip < 0 || clip >= b->anim.clip_count) return 0;
    const int fc = b->anim.clips[clip].frame_count;
    if (fc <= 0) return 0;
    const int kf = (int)(re15_emd_get_frame_entry(&b->anim, clip, bild % fc) & 0xfffu);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    const int rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    em1a_ring_welt(e, c, poses[hb].rot, poses[hb].trans, out);
    return 1;
}

/* Anmeldung durch den Lader (s. Header). */
void re15_re2arm_modell_einrichten(re15_enemy_bank_t *anzeige, const re15_enemy_bank_t *em2d_logik)
{
    s_logik_anim = (em2d_logik && em2d_logik->anim.clip_count > 0) ? &em2d_logik->anim : NULL;
    if (!anzeige) return;
    /* Leons Opfer-Clips sind RE2-Funktion (Spieler-Routine 5 / Hook 0x8010121C spielt PL-Clip 0/1 aus Paar 3,
     * @0x801012A8 / @0x80101338) — EM1A hat kein Paar 3 (victim_ok 0, em_dump). Die EM2D-Paar-3-Bank aliast das
     * residente RE2-Archiv. */
    if (em2d_logik && em2d_logik->victim_ok) {
        anzeige->skel_victim = em2d_logik->skel_victim;
        anzeige->anim_victim = em2d_logik->anim_victim;
        anzeige->victim_ok   = 1;
    }
    /* Runde 2: Parts/Hand rechnen auf EM2D (s. HAND); keine Wurzel-Hebung mehr (s. HOEHE) — eine Hebung aus Runde 1
     * auf derselben Bank wird zurueckgenommen. */
    s_logik_bank = (em2d_logik && em2d_logik->skel.bone_count > RE2ARM_EM1A_KNOCHEN) ? em2d_logik : NULL;
    if (anzeige->skel.bone_count == RE2ARM_EM1A_KNOCHEN) {
        anzeige->skel.root_y_fix = NULL;
        anzeige->skel.root_y_fix_count = 0;
    }
}

int re15_re2arm_hand_bone(const re15_actor_t *e)
{   /* Tabelle @0x80101414 = {1..7 | 8..14}; Hand = [var*7 + 2] (@0x801007DC / @0x801009D4 / @0x80100BEC).
     * EM1A gezeichnet: mit EM2D-Logik-Bank bleibt die Hand der EM2D-Handknochen (Runde 2, s. HAND); nur ohne RE2-Archiv
     * misst das Gehirn am EM1A-Handgelenk (Arm A -> Teil 2, Arm B -> Teil 3). */
    if (re15_re2arm_modell_re15(e) && !logik_em2d(e)) return em1a_arm(e);
    return (e->re2z_f10e & 1u) ? 10 : 3;
}
void re15_re2arm_home(int slot, int16_t *yaw, int32_t *x, int32_t *z)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    if (yaw) *yaw = s_arm[slot].home_yaw;
    if (x)   *x   = s_arm[slot].home_x;
    if (z)   *z   = s_arm[slot].home_z;
}
uint16_t re15_re2arm_part_hide_mask(const re15_actor_t *e)
{
    if (!re15_re2arm_owns(e)) return 0;
    /* EM1A (Runde 3): alle vier Teile — im Original stehen die Zombies sichtbar hinter dem Gitter (Block MODELL, WAS DAS
     * ORIGINAL ZEIGT; die Maske 0x0003 aus Runde 2 beruhte auf einer falschen Aussage). Lage/Pose: re15_re2arm_zeichen_teil. */
    if (re15_re2arm_modell_re15(e)) return 0;
    /* Die sieben Bones des ANDEREN Arms: word0 = 0 (@0x80100240 fuer Arm B inaktiv -> 1..7 weg,
     * @0x801002F8 fuer Arm A inaktiv -> 8..14 weg). */
    return (e->re2z_f10e & 1u) ? 0x00FEu : 0x7F00u;
}

/* Clipwort `sw (rate<<16)|(frame<<8)|clip, 332(s0)` -> Port-Animationsfelder (Konvention der
 * Wellen B/C: Rate -> anim_frac-Seed, a3 von 0x8002959C -> anim_blend_rate). */
static void arm_clip(re15_actor_t *e, int clip, int rate)
{
    re2arm_t *a = &s_arm[arm_slot(e)];
    a->logik_clip = (int8_t)clip; a->logik_frame = 0;      /* RE2-Takt (Runde 42 Stufe 2) */
    e->motion = (int16_t)(re15_re2arm_modell_re15(e) ? re15_re2arm_em1a_clip(clip) : clip);
    e->anim_frame = 0; e->anim_frac = (uint8_t)rate;
    e->anim_blend_rate = 256; e->anim_freeze = 0;
    e->anim_flags &= (uint16_t)~(0x80u | 0x04u);
}
static int arm_frame(const re15_actor_t *e)        /* +0x14D */
{
    if (re15_re2arm_modell_re15(e)) {                      /* RE2-Takt, nicht das gezeichnete EM1A-Bild */
        const re2arm_t *a = &s_arm[arm_slot(e)];
        const int l2 = logik_len(e, (int)a->logik_clip);
        return (l2 > 0) ? (int)(a->logik_frame % (uint32_t)l2) : (int)a->logik_frame;
    }
    int fc = re15_actor_clip_len(e);
    return (fc > 0) ? (int)(e->anim_frame % (uint32_t)fc) : (int)e->anim_frame;
}
/* 0x8002959C mit Pose-Merker (s. re2arm_t.pose_*): erst die Pose des AKTUELLEN Bilds merken
 * (= das, was 0x80029614 in die Parts schreibt), dann zaehlen. */
/* 0x80029614 auf die Parts des Arms (Nachbesserung 2): Ziel = Keyframe des Bilds +0x14D VOR dem
 * Zaehlen, Mischung mit t1 = a3 * +0x14E (VOR dem Dekrement, @0x800296a8-bc) gegen die Parts des
 * letzten Aufrufs. Die Mischformel ist die des Port-Renderers (re15_skel_compute_pose: wp = frac *
 * rate, prev*wp + kf*(4096-wp), Kuerzestweg) — hier auf einem Schatten-Aktor, damit der Takt des
 * Gehirns die Parts traegt wie im Original und nicht erst der Zeichner. */
static void arm_parts(re15_actor_t *e, int slot, int rate)
{
    static re15_actor_t sh;
    re2arm_t *a = &s_arm[slot];
    /* Runde 2 (s. HAND): mit EM2D-Logik-Bank tragen die Parts die EM2D-Pose des RE2-Takts (Clip/Bild wie bis Runde 41
     * +0x14C/+0x14D), nicht das gezeichnete EM1A. */
    const int lg = logik_em2d(e);
    const re15_enemy_bank_t *b = lg ? s_logik_bank : re15_enemy_find(e->type);
    if (!b || b->skel.bone_count <= 0 || b->skel.bone_count > RE15_EMD_MAX_BONES) { a->pvalid = 0; return; }
    sh = *e;
    if (lg) { sh.motion = (int16_t)a->logik_clip; sh.anim_frame = a->logik_frame; }
    sh.anim_frac = e->anim_frac;                         /* t3 = +0x14E vor `addiu -1` @0x800299c8 */
    sh.anim_blend_rate = (uint16_t)rate;                 /* a3 (Arm: 256 an allen Advance-Stellen) */
    sh.anim_prev_valid = a->pvalid;
    sh.neck_bone = 0; sh.hurt_bend_bone = -1;            /* keine Kopf-/Treffer-Zusaetze im Schatten */
    memcpy(sh.prev_angles, a->pa, sizeof a->pa);
    memcpy(sh.prev_root, a->proot, sizeof a->proot);
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, &sh, -1, lg ? (uint32_t)a->logik_frame : e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = &sh;
    int rv = (kf >= 0) ? re15_skel_compute_pose(&b->skel, kf, poses) : -1;
    g_anim_pose_actor = save;
    if (rv != 0) { a->pvalid = 0; return; }
    memcpy(a->pa, sh.prev_angles, sizeof a->pa);
    memcpy(a->proot, sh.prev_root, sizeof a->proot);
    for (int i = 0; i < b->skel.bone_count; i++) {
        memcpy(a->ptrans[i], poses[i].trans, sizeof a->ptrans[i]);
        memcpy(a->prot[i], poses[i].rot, sizeof a->prot[i]);   /* Runde 42 Stufe 2: EM1A-Handgelenk */
    }
    a->pvalid = 1;
}
static int arm_adv(re15_actor_t *e, int slot, int blend)
{
    const int lg = logik_em2d(e);                        /* Runde 2: Parts-Pose = RE2-Takt (s. HAND) */
    s_arm[slot].pose_clip  = lg ? (int16_t)s_arm[slot].logik_clip : e->motion;      /* +0x14C beim Aufruf */
    s_arm[slot].pose_frame = lg ? s_arm[slot].logik_frame : e->anim_frame;          /* +0x14D VOR `addiu +1` @0x80029B30 */
    s_arm[slot].pose_ok    = 1;
    arm_parts(e, slot, blend);                           /* 0x8002959C -> 0x80029614 (Parts)   */
    if (!re15_re2arm_modell_re15(e)) return re15_re2_advance_959c(e, blend);
    /* EM1A (Runde 42 Stufe 2): 0x8002959C auf dem RE2-Takt — dieselben Schritte wie re15_re2_advance_959c
     * (+0x14E-Decay @0x800299C0-CC, +1 @0x80029B30, Wrap + done @0x80029B48-4C), Laenge = EM2D-Clip; das
     * gezeichnete EM1A-Bild laeuft ein Bild je Takt auf seiner eigenen Laenge (s. MODELL). */
    re2arm_t *a = &s_arm[slot];
    e->anim_blend_rate = (uint16_t)blend;
    if (e->anim_frac > 0) e->anim_frac--;
    const int l2 = logik_len(e, (int)a->logik_clip);
    int done = 0;
    const uint32_t nf = (uint32_t)a->logik_frame + 1u;
    if (l2 <= 0 || (int)nf >= l2) { a->logik_frame = 0; done = 1; }
    else a->logik_frame = (uint16_t)nf;
    e->anim_frame = em1a_bild_weiter(e);
    return done;
}
/* part[Hand]+0x5C/+0x60/+0x64 = Hand-Weltpunkt der GEMISCHTEN Parts des letzten Advance (s. o.),
 * mit Lage/Blick des Arms wie beim Zeichnen (FUN_80027160: Entity-Matrix aus +0x74/+0x38..). */
static void arm_hand_pose(re15_actor_t *e, int slot, int32_t out[3])
{
    const int hb = re15_re2arm_hand_bone(e);
    /* Runde 4 (Vorgabe Punkt 2: "Die Reichweite folgt dem RE1.5-Arm"): mit EM1A ist die Hand des Gehirns die gezeichnete
     * RE1.5-Hand (Block KOERPER, mit der Original-Skalierung B[3]) - fuer die Tore 900 (A1) / Greifen (A3) und den Pin (B4 P0). */
    if (re15_re2arm_modell_re15(e) && s_arm[slot].koerper_an && koerper_hand(e, &s_arm[slot], out, NULL)) return;
    if (re15_re2arm_modell_re15(e) && !logik_em2d(e)) {  /* ohne RE2-Archiv: EM1A-Handgelenk (s. HAND) */
        int32_t c[3];
        if (s_arm[slot].pvalid && hb >= 0 && hb < RE15_EMD_MAX_BONES &&
            em1a_ring(re15_enemy_find(e->type), hb, c)) {
            em1a_ring_welt(e, c, s_arm[slot].prot[hb], s_arm[slot].ptrans[hb], out);
            return;
        }
        const int clip = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_clip : (int)e->motion;
        const int bild = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_frame : (int)e->anim_frame;
        if (em1a_hand_rein(e, clip, bild, out)) return;
        re15_enemy_bone_world_pos(e, hb, out);
        return;
    }
    if (s_arm[slot].pvalid && hb >= 0 && hb < RE15_EMD_MAX_BONES) {
        re15_skel_bone_to_world(s_arm[slot].ptrans[hb], e->rot_y, e->x, e->y, e->z, out);
        return;
    }
    if (logik_em2d(e)) {                                 /* Runde 2: EM2D-Handknochen, reine Pose des RE2-Takts */
        const int clip = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_clip : (int)s_arm[slot].logik_clip;
        const int bild = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_frame : (int)s_arm[slot].logik_frame;
        if (!logik_knochen_rein(e, clip, bild, hb, out)) { out[0] = e->x; out[1] = e->y; out[2] = e->z; }
        return;
    }
    const int16_t mo = e->motion; const uint16_t fr = e->anim_frame;
    if (s_arm[slot].pose_ok) { e->motion = s_arm[slot].pose_clip; e->anim_frame = s_arm[slot].pose_frame; }
    re15_enemy_bone_world_pos(e, hb, out);
    e->motion = mo; e->anim_frame = fr;
}
/* Mess-/Test-Auskunft (Nachbesserung 2): Hand der gemischten Parts (= Pin-Quelle) und die reine
 * Keyframe-Hand der Parts-Pose (ohne +0x14E), beide in Weltkoordinaten. */
int re15_re2arm_hand_parts(int slot, int32_t gemischt[3], int32_t rein[3])
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e = &g_actors[slot];
    if (gemischt) arm_hand_pose(e, slot, gemischt);
    if (rein && logik_em2d(e)) {                         /* Runde 2: EM2D-Handknochen, reine Pose (s. HAND) */
        const int clip = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_clip : (int)s_arm[slot].logik_clip;
        const int bild = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_frame : (int)s_arm[slot].logik_frame;
        if (!logik_knochen_rein(e, clip, bild, re15_re2arm_hand_bone(e), rein)) { rein[0] = e->x; rein[1] = e->y; rein[2] = e->z; }
        rein = NULL;
    }
    if (rein && re15_re2arm_modell_re15(e)) {            /* ohne RE2-Archiv: EM1A-Handgelenk, reine Pose */
        const int clip = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_clip : (int)e->motion;
        const int bild = s_arm[slot].pose_ok ? (int)s_arm[slot].pose_frame : (int)e->anim_frame;
        if (!em1a_hand_rein(e, clip, bild, rein)) re15_enemy_bone_world_pos(e, re15_re2arm_hand_bone(e), rein);
        rein = NULL;
    }
    if (rein) {
        const int16_t mo = e->motion; const uint16_t fr = e->anim_frame;
        if (s_arm[slot].pose_ok) { e->motion = s_arm[slot].pose_clip; e->anim_frame = s_arm[slot].pose_frame; }
        re15_enemy_bone_world_pos(e, re15_re2arm_hand_bone(e), rein);
        e->motion = mo; e->anim_frame = fr;
    }
    return s_arm[slot].pvalid;
}
/* ---- KOERPER (Runde 42 Stufe 2, Runde 4; Weg B[1] im Block MODELL) ---------------------------------------------------
 * NUTZERWUNSCH 18.11.2026, woertlich: "Aber du solltest EIGENTLICH NUR die funktionalität aus Resident Evil 2 übernehmen,
 * nicht das Modell. Das Modell darf schon noch das aus Resident Evil 1.5 sein...." und 02.11. (Memory
 * reai-v2-re15-modellaussehen-entscheidend): "Haben wir im Port jetzt Stummel Aermchen oder irgendeinen anderen Quatsch
 * gemacht ist das falsch." -> Ausserhalb eines Griffs laeuft der Koerper wie das Original, im Griff nur RE1.5-Posen; der
 * Port streckt, staucht und dreht KEIN Teil selbst (die Streckung aus Runde 3 ist entfernt).
 *
 * KOERPER-ZUSTAND (STAGE1.BIN, Disc == Mod im Arm-Code 0x8010c1ec..0x8010d7cc; selbst disassembliert, re15_disasm.py
 * dis 0x8010c8a0 / 0x8010cad0; B-Tabelle @0x80120984: [2] = 0x8010c938, [3] = 0x8010cb34):
 *   Ende B[1] (Bild 38, +0x9C alt 0 @0x8010c8c4): +0x5 = (rng&1)+2 @0x8010c8cc-e4, +0x6 = 0 @0x8010c8f4.
 *   B[2] WIEGEN @0x8010c938:
 *     P0: +0x95 = 0 @0x8010c98c, weiter in P1.
 *     P1 @0x8010c990: +0x9F = +0x9E = (rng&3)+1 @0x8010c998-9b0, +0x8C = (rng&0x7f)+50 @0x8010c9b4-c4, +0x6 = 2 @0x8010c9d4,
 *        Clip 0 @0x8010c9e4, rng&1 -> Clip 1 + Bild 0 @0x8010c9fc-ca24; weiter in P2.
 *     P2 @0x8010ca28: +0x9E-- (alt 0 -> +0x6 = 3 @0x8010ca54), anim_set, Schritt RUECKWAERTS (a0 = 0x800 @0x8010ca78) um +0x8C.
 *     P3 @0x8010ca7c: +0x9F-- ; alt 0 -> +0x6 = 1 @0x8010ca9c und +0x1D0-- (alt 0 -> +0x5 = 3 @0x8010cacc, +0x6 = 0
 *        @0x8010cadc, +0x1D0 = (rng&0x1f)+30 @0x8010cae0-f0); anim_set, Schritt VORWAERTS (a0 = 0 @0x8010cb10).
 *   B[3] ARME WACHSEN @0x8010cb34:
 *     P0 @0x8010cb90: +0x6 = 1, Clip = rng&1 @0x8010cba4, Bild 0 @0x8010cbb4, +0x9E = 5 @0x8010cbd4, +0x9C = y @0x8010cbe4-ec;
 *        Part 2: Flag |= 0x400 @0x8010cc18-20, Skalierung (0x1000,0x1000,0x1000) @0x8010cc08-14, +0x38 = +0x82 =
 *        N = (rng&0xff)+100 @0x8010cc24-34, +0x76 = (rng&7)+5 @0x8010cc38-40; Part 3: Flag |= 0x400 @0x8010cc68-70,
 *        Skalierung 0x1000 @0x8010cc5c-64, +0x76 = rng&7 @0x8010cc74-78; weiter in P1.
 *     P1 @0x8010cc7c: y = +0x9C + (s8)+0x9E @0x8010cc88-98, +0x9E = -+0x9E @0x8010cca8-b4; Part 3 +0x8A += +0x76
 *        @0x8010cccc-e0; Part 2 +0x8A += +0x76 @0x8010ccf8-d10; Part 2 +0x82-- (alt 0 -> +0x6 = 2 @0x8010cd30); anim_set.
 *     P2 @0x8010cd34: Part 3 +0x8A -= 2*+0x76 @0x8010cd3c-4c, Part 2 ebenso @0x8010cd64-7c; Part 2 +0x38 < 0 -> +0x6 = 3
 *        @0x8010cd78-8c; +0x38 -= 2 @0x8010cd90-9c; anim_set.
 *     P3 @0x8010cdc4: Flag 0x400 an Part 2/3 loeschen @0x8010cdcc-dfc, +0x5 = 2 @0x8010ce0c, +0x6 = 0, +0x1D0 =
 *        (rng&0x1f)+30 @0x8010ce18-30. Kein anim_set.
 *   +0x1D0 Startwert (rng&0x1f)+30 im Typ-INIT @0x8010c3e8-3fc. Wandklemme wie B[1] (Radius 300, Maske 4 @0x8010c318-320).
 *   ZEICHNEN DER SKALIERUNG (EXE FUN_8001e9ec, Decompile RE_15_Quellcode_V2/FUN_8001e9ec.c): Part-Flag 0x400 -> ScaleMatrix(
 *   Kamera*Part, (+0x88, +0x8A, +0x8C)) direkt vor SetRotMatrix. ScaleMatrix multipliziert Spalte j mit v[j] (Decompile
 *   ScaleMatrix.c), also die LOKALE y-Achse des Arm-Meshes (laengs des Arms) mit +0x8A/4096.
 *   GEMESSEN im Original (DuckStation, MZD-Disc, ROOM1210; scratchpad original/s2r2/orig/lauf4 p029..p058, Saetze s4/s5,
 *   s2r4/skal.py): nach B[1] B[3] mit Clip 1, Part 2 Flag 0x401, +0x76 = 6, +0x8A 4138 -> 5398, +0x82 335 -> 125; Part 3
 *   +0x76 = 0 (bleibt 4096); y zittert (+0x9E 5 / 251). DAS ORIGINAL STRECKT SEINE ARME ALSO SELBST: bis 4096 + (N+1)*d,
 *   hoechstens 4096 + 356*12 = 8368 (2,04-fach).
 *   RUNDE 6 (Vorgabe des Orchestrators ab Runde 6, Punkt 1: "Aussehen und Bewegung des Koerpers GENAU wie das Original ...
 *   Wachsen B[3] mit der Original-Skalierung (Zeitpunkt, Rate, Obergrenze aus den Bytes) - nicht mehr und nicht weniger"):
 *   der Port zeichnet GENAU diese Skalierung (koerper_teil_welt: Spalte 1 von Teil 2/3 mal +0x8A/4096, solange das Flag 0x400
 *   steht; Rumpf/Kopf und Spalten 0/2 immer 4096). Die Runde-5-Fassung (Faktor 1 auch in B[3]) ist entfernt.
 *   Wer die Skalierung schreibt (Zensus aller `sh`-Stores in den Arm-Code 0x8010c1ec..0x8010d7c8, scratchpad original/s2r4/
 *   arm_code.dis): NUR B[3] - Part 2 @0x8010cc0c-14 / @0x8010cd10 / @0x8010cd7c, Part 3 @0x8010cc5c-64 / @0x8010cce0 /
 *   @0x8010cd4c, Flag @0x8010cc18 / @0x8010cc68 setzen, @0x8010cdd0 loeschen. Weder B[4] (@0x8010ce54-cfd4) noch der
 *   Treffer-Zweig (@0x8010d188 ff.) aendern sie.
 *
 * GRIFF (NUTZERWUNSCH s.o.; Vorgabe des Orchestrators ab Runde 6, Punkte 1/2): Das RE2-Gehirn bestimmt Zeit, Greifen,
 * Halten, Rueckzug, Treffer, Toene, Ende. Der Koerper geht in den GRIFF, sobald das Gehirn ZUGREIFT (0x301), aus der aktuellen
 * Lage und bleibt dort stehen (kein Vortreten: die Reichweite kommt aus dem Wachsen B[3] des Originals). Er zeigt den
 * Ausfahr-Teil von EM1A-Clip 2 ab Bild 20 (RE2ARM_GRIFF_BILD0) Bild fuer Bild; ab dem Griff (0x401) steht diese Pose
 * (naechstliegende RE1.5-Pose: RE1.5 hat keinen Halte-Clip, PORT-WAHL). Leon/Elza steht dabei FEST an der Hand (Block
 * HALTE-LAGE). Nach Zugriff/Halten oder einem Treffer sinkt der Koerper mit Clip 3 ganz zurueck (wie B[4] @0x8010ceac-b0,
 * 54 Bilder). PORT-WAHL fuer den Zustand: Griff und Sinken UNTERBRECHEN den Original-Ablauf (B[1]/B[2]/B[3] mit Phase,
 * Zaehlern und Skalierung bleiben stehen - wie B[4], das die Skalierung auch nicht anfasst, s.o.); danach laeuft er an der
 * unterbrochenen Stelle weiter (Clip/Bild wie vorher). Grund: die RE2-Hand muss im Halten still an der stehenden Figur liegen
 * (Vorgabe Punkt 2); liefe B[3] weiter, wanderte die Hand mit +0x76 je Bild (bis 12/4096 der Armlaenge) in die Figur hinein
 * oder von ihr weg. Endet ein REACH ohne Zugriff (0x501 aus B1), laeuft der Koerper ununterbrochen weiter. */
#define RE2ARM_HAND_V0   0           /* Hand = Quad-Vertices 0..11 von Mesh 2/3: Fingerspitzen 0..3 (y ~1531), Handgelenk-Ring
                                      * 4..7 (y ~1215), Handmitte 8..11 (y ~1376) (mesh_verts, Block MODELL)            */
#define RE2ARM_HAND_N    12

static void koerper_anim(const re15_actor_t *e, re2arm_t *a)       /* anim_set: +1 je Aufruf, Wrap an der Clip-Laenge */
{
    const int l = em1a_len(e, (int)a->kclip);
    const int f = (int)a->kframe + 1;
    a->kframe = (uint16_t)((l > 0 && f >= l) ? 0 : f);
}
static void koerper_schritt(re2arm_t *a, int spd, int rueck)       /* FUN_800245d8 + Wandklemme @0x8010c320 */
{
    const int yaw = ((int)a->rec_yaw + (rueck ? 0x800 : 0)) & 0xfff;   /* a0 = 0 / 0x800 */
    int32_t nx = a->kx + (int32_t)(((int32_t)re15_cos_q12(yaw) * spd) >> 12);
    int32_t nz = a->kz - (int32_t)(((int32_t)re15_sin_q12(yaw) * spd) >> 12);
    if (g_room_rdt_ok)
        re15_collision_constrain_enemy(&g_room_rdt, a->kx, a->kz, &nx, &nz, RE2ARM_KOERPER_RADIUS, a->rec_y,
                                       RE2ARM_KOERPER_MASKE);
    a->kx = nx; a->kz = nz;
}
static void koerper_b2(const re15_actor_t *e, re2arm_t *a)
{
    if (a->kphase == 0) { a->kframe = 0; a->kphase = 1; }               /* @0x8010c98c, weiter in P1 */
    if (a->kphase == 1) {                                                /* @0x8010c990 */
        const int n = (rng15() & 3) + 1;
        a->k9f = (uint8_t)n; a->k9e = (uint8_t)n;
        a->k8c = (int16_t)((rng15() & 0x7f) + 50);
        a->kphase = 2; a->kclip = 0;
        if (rng15() & 1) { a->kclip = 1; a->kframe = 0; }                /* @0x8010c9fc-ca24 */
    }
    if (a->kphase == 2) {                                                /* @0x8010ca28 */
        const uint8_t alt = a->k9e; a->k9e = (uint8_t)(alt - 1u);
        if (alt == 0) a->kphase = 3;
        koerper_anim(e, a); koerper_schritt(a, a->k8c, 1);
    } else if (a->kphase == 3) {                                         /* @0x8010ca7c */
        const uint8_t alt = a->k9f; a->k9f = (uint8_t)(alt - 1u);
        if (alt == 0) {
            a->kphase = 1;
            const uint16_t r = a->k1d0; a->k1d0 = (uint16_t)(r - 1u);
            if (r == 0) { a->kmode = K_B3; a->kphase = 0; a->k1d0 = (uint16_t)((rng15() & 0x1f) + 30); }
        }
        koerper_anim(e, a); koerper_schritt(a, a->k8c, 0);
    }
}
static void koerper_b3(const re15_actor_t *e, re2arm_t *a)
{
    if (a->kphase == 0) {                                                /* @0x8010cb90 */
        a->kphase = 1;
        a->kclip = (uint8_t)(rng15() & 1); a->kframe = 0;
        a->k9e = 5; a->k9c = (int16_t)a->ky;
        a->sk_an[0] = 1; a->sk_y[0] = 0x1000;
        const int n = (rng15() & 0xff) + 100; a->sk_n38 = (int16_t)n; a->sk_n82 = (int16_t)n;
        a->sk_d[0] = (int8_t)((rng15() & 7) + 5);
        a->sk_an[1] = 1; a->sk_y[1] = 0x1000;
        a->sk_d[1] = (int8_t)(rng15() & 7);
    }
    if (a->kphase == 1) {                                                /* @0x8010cc7c */
        a->ky = (int32_t)a->k9c + (int32_t)(int8_t)a->k9e;
        a->k9e = (uint8_t)(-(int8_t)a->k9e);
        a->sk_y[1] = (int16_t)(a->sk_y[1] + a->sk_d[1]);
        a->sk_y[0] = (int16_t)(a->sk_y[0] + a->sk_d[0]);
        const uint16_t alt = (uint16_t)a->sk_n82; a->sk_n82 = (int16_t)(alt - 1u);
        if (alt == 0) a->kphase = 2;
        koerper_anim(e, a);
    } else if (a->kphase == 2) {                                         /* @0x8010cd34 */
        a->sk_y[1] = (int16_t)(a->sk_y[1] - 2 * a->sk_d[1]);
        a->sk_y[0] = (int16_t)(a->sk_y[0] - 2 * a->sk_d[0]);
        if (a->sk_n38 < 0) a->kphase = 3;
        a->sk_n38 = (int16_t)(a->sk_n38 - 2);
        koerper_anim(e, a);
    } else if (a->kphase == 3) {                                         /* @0x8010cdc4 */
        a->sk_an[0] = a->sk_an[1] = 0;
        a->kmode = K_B2; a->kphase = 0;
        a->k1d0 = (uint16_t)((rng15() & 0x1f) + 30);
    }
}
/* Runde 6: Griff/Treffer unterbrechen den Original-Ablauf (s. GRIFF, PORT-WAHL); der unterbrochene Stand wird gemerkt und nach
 * dem Sinken fortgesetzt. Skalierung, Zaehler, Lage und y bleiben dabei unberuehrt (wie B[4], s. KOERPER-ZUSTAND). */
static void koerper_unterbrechen(re2arm_t *a)
{
    if (a->kmode == K_GRIFF || a->kmode == K_SINKEN) return;
    a->u_mode = a->kmode; a->u_phase = a->kphase; a->u_clip = a->kclip; a->u_frame = a->kframe;
}
static void koerper_sinken(re2arm_t *a)
{
    koerper_unterbrechen(a);
    a->kmode = K_SINKEN; a->kclip = 3; a->kframe = 0; a->treffer = 0;    /* Clip 3 wie B[4] @0x8010ceac-b0, Bild 0 @0x8010cec0 */
}
/* Ein Bild des Koerpers: Weg B[1] ab dem Ausloeser, danach B[2]/B[3]; Griff-Kopplung an das RE2-Gehirn (Stand nach dessen
 * letztem Tick, s. GRIFF). */
static void koerper_tick(re15_actor_t *e, re2arm_t *a)
{
    if (!re15_re2arm_modell_re15(e)) return;
    if (!a->koerper_an) {                                 /* Ausloeser: A[0] @0x8010c614 -> +0x5 = 1, B[1] im selben Bild */
        a->koerper_an = 1; a->koerper_bild = 0;
        a->kx = a->rec_x; a->kz = a->rec_z; a->ky = a->rec_y;
        a->kclip = 0; a->kframe = 0;                      /* `sb zero,148` @0x8010c778, `sb zero,149` @0x8010c788 */
        a->kmode = K_B1;
    }
    const int sub = (e->state == 1) ? (int)e->sub_state_1 : -1;
    if (sub == 3 && a->kmode != K_GRIFF && a->kmode != K_SINKEN) {       /* Zugriff 0x301: aus der aktuellen Lage */
        koerper_unterbrechen(a);                                        /* Runde 6: Original-Stand merken (s. GRIFF) */
        a->kmode = K_GRIFF; a->g_frame = RE2ARM_GRIFF_BILD0; a->g_halten = 0;
    } else if (sub == 4 && a->kmode == K_GRIFF) {
        a->g_halten = 1;                                                /* Halten 0x401: Pose steht */
    } else if ((a->kmode == K_GRIFF && sub != 3 && sub != 4) ||           /* Zugriff/Halten vorbei (0x501/0x601/Treffer) */
               (sub == 5 && a->treffer && a->kmode != K_SINKEN)) {     /* Treffer beim Wiegen */
        koerper_sinken(a);
        return;
    }
    if (getenv("RE15_RE2_TRACE")) {                       /* Messschiene: Koerper-Modus und Arm-Skalierung (Video-Planung) */
        static uint8_t s_km[RE15_ACTOR_MAX]; static uint16_t s_kt[RE15_ACTOR_MAX];
        const int sl = arm_slot(e);
        if (s_km[sl] != a->kmode || ++s_kt[sl] >= 30) {
            int32_t h[3] = { 0, 0, 0 }; (void)koerper_hand(e, a, h, NULL);
            FILE *o = re15_re2_trace_out() ? re15_re2_trace_out() : stderr;
            fprintf(o, "[re15koerper] F%u slot %d modus %d phase %d sk %d/%d an %d/%d hand (%d,%d,%d) leib (%d,%d)\n", (unsigned)g_engine.frame_count, sl,
                    (int)a->kmode, (int)a->kphase, (int)a->sk_y[0], (int)a->sk_y[1], (int)a->sk_an[0], (int)a->sk_an[1],
                    h[0], h[1], h[2], a->kx, a->kz);
            s_km[sl] = a->kmode; s_kt[sl] = 0;
        }
    }
    switch (a->kmode) {
    case K_B1: {
        const int bild = (int)a->koerper_bild;
        a->kclip = (uint8_t)b1_clip(bild);                /* Clipwechsel ohne +0x95-Reset (@0x8010c7ec-818 / @0x8010c874-880) */
        koerper_anim(e, a);
        const int st = k_b1_schritt[bild];
        koerper_schritt(a, st < 0 ? -st : st, st < 0);
        a->koerper_bild = (uint16_t)(bild + 1);
        if (bild == RE2ARM_B1_BILDER - 1) { a->kmode = (rng15() & 1) ? K_B3 : K_B2; a->kphase = 0; }   /* @0x8010c8cc-f4 */
        break;
    }
    case K_B2: koerper_b2(e, a); break;
    case K_B3: koerper_b3(e, a); break;
    case K_GRIFF:
        if (!a->g_halten) {
            const int l = em1a_len(e, 2);
            const int f = (int)a->g_frame + 1;
            a->g_frame = (uint16_t)((l > 0 && f >= l) ? 0 : f);   /* Runde 6: kein Schritt - der Koerper bleibt stehen */
        }
        break;
    case K_SINKEN: {
        const int l = em1a_len(e, 3);
        const int f = (int)a->kframe + 1;
        if (l > 0 && f >= l) {                            /* Clip 3 zu Ende: der unterbrochene Original-Ablauf geht weiter */
            a->kmode = a->u_mode; a->kphase = a->u_phase; a->kclip = a->u_clip; a->kframe = a->u_frame;
        } else a->kframe = (uint16_t)f;
        break;
    }
    default: break;
    }
}
int re15_re2arm_koerper_lage(const re15_actor_t *e, int32_t p[3])
{
    if (!e || !re15_re2arm_owns(e) || !re15_re2arm_modell_re15(e)) return 0;
    const re2arm_t *a = &s_arm[arm_slot(e)];
    if (!a->koerper_an) return 0;
    if (p) { p[0] = a->kx; p[1] = a->ky; p[2] = a->kz; }
    return 1;
}
int re15_re2arm_koerper_info(int slot, int *bild, int *clip, int *frame, int32_t rec[3])
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const re2arm_t *a = &s_arm[slot];
    if (bild)  *bild  = a->koerper_an ? (int)a->koerper_bild : -1;
    if (clip)  *clip  = (a->kmode == K_GRIFF) ? 2 : (int)a->kclip;
    if (frame) *frame = (a->kmode == K_GRIFF) ? (int)a->g_frame : (int)a->kframe;
    if (rec)   { rec[0] = a->rec_x; rec[1] = a->rec_y; rec[2] = a->rec_z; }
    return a->koerper_an;
}
int re15_re2arm_koerper_zustand(int slot, int *mode, int *phase, int sk[2], int an[2], int32_t *ky)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const re2arm_t *a = &s_arm[slot];
    if (mode)  *mode = (int)a->kmode;
    if (phase) *phase = (int)a->kphase;
    if (sk)    { sk[0] = a->sk_y[0]; sk[1] = a->sk_y[1]; }
    if (an)    { an[0] = a->sk_an[0]; an[1] = a->sk_an[1]; }
    if (ky)    *ky = a->ky;
    return a->koerper_an;
}

/* Gezeichnete RE1.5-Pose des Koerpers (reine Keyframe-Pose), 0 = nicht berechenbar. */
static int koerper_pose(const re15_actor_t *e, const re2arm_t *a, re15_skel_pose_t *out)
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const int clip = (a->kmode == K_GRIFF) ? 2 : (int)a->kclip;
    const int bild = (a->kmode == K_GRIFF) ? (int)a->g_frame : (int)a->kframe;
    if (!b || clip >= b->anim.clip_count) return 0;
    const int fc = b->anim.clips[clip].frame_count;
    if (fc <= 0) return 0;
    const int kf = (int)(re15_emd_get_frame_entry(&b->anim, clip, bild % fc) & 0xfffu);
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    const int rv = re15_skel_compute_pose(&b->skel, kf, out);
    g_anim_pose_actor = save;
    return rv == 0;
}
/* Welt-Matrix eines EM1A-Teils wie der Zeichner (nyaw * Pose, + Lage), Teil 2/3 mit der Original-Skalierung der lokalen
 * y-Achse (Spalte 1, ScaleMatrix in FUN_8001e9ec). Rueckgabe Skalierung Q12 (4096 = keine), -1 = nicht berechenbar. */
static int koerper_teil_welt(const re15_actor_t *e, const re2arm_t *a, int teil, int32_t rot[9], int32_t trans[3])
{
    static re15_skel_pose_t P[RE15_EMD_MAX_BONES];
    if (teil < 0 || teil >= RE2ARM_EM1A_KNOCHEN || !koerper_pose(e, a, P)) return -1;
    const int32_t fs = re15_sin_q12((int)a->rec_yaw), fc = re15_cos_q12((int)a->rec_yaw);
    const int32_t ny[9] = { fc, 0, fs, 0, 0x1000, 0, -fs, 0, fc };   /* wie der Zeichner (platform/pc/main.c nyaw) */
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int64_t s = 0;
            for (int k = 0; k < 3; k++) s += (int64_t)ny[r*3+k] * (int64_t)P[teil].rot[k*3+c];
            rot[r*3+c] = (int32_t)(s >> 12);
        }
        int64_t t = 0;
        for (int k = 0; k < 3; k++) t += (int64_t)ny[r*3+k] * (int64_t)P[teil].trans[k];
        trans[r] = (int32_t)(t >> 12);
    }
    trans[0] += a->kx; trans[1] += a->ky; trans[2] += a->kz;
    /* Runde 6 (Vorgabe ab Runde 6 Punkt 1, Block KOERPER): GENAU die Skalierung des Originals, nicht mehr und nicht weniger -
     * Part-Flag 0x400 (@0x8010cc18 / @0x8010cc68, geloescht @0x8010cdd0) -> ScaleMatrix(.., (+0x88, +0x8A, +0x8C)) in FUN_8001e9ec;
     * +0x88/+0x8C bleiben 0x1000 (@0x8010cc0c/14, @0x8010cc5c/64, sonst kein Store), also nur Spalte 1 mal +0x8A/4096. */
    int sk = 4096;
    if ((teil == 2 || teil == 3) && a->sk_an[teil - 2]) {
        sk = (int)a->sk_y[teil - 2];
        for (int r = 0; r < 3; r++) rot[r*3+1] = (int32_t)(((int64_t)rot[r*3+1] * sk) >> 12);   /* Spalte 1 * +0x8A */
    }
    return sk;
}
/* Hand des gezeichneten RE1.5-Arms (Mittel der Quad-Vertices 0..11 von Mesh 2/3) in Welt, dazu das Schultergelenk. */
static int koerper_hand(const re15_actor_t *e, const re2arm_t *a, int32_t hand[3], int32_t schulter[3])
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const int teil = em1a_arm(e);
    if (!b || teil >= b->skel.bone_count) return 0;
    const int m = b->skel.bone_mesh_index[teil];
    if (m < 0 || m >= b->md1.mesh_count) return 0;
    const re15_md1_mesh_t *mm = &b->md1.meshes[m];
    if (!mm->quad_vertices || mm->quad_vertex_count < RE2ARM_HAND_V0 + RE2ARM_HAND_N) return 0;
    int32_t R[9], T[3];
    if (koerper_teil_welt(e, a, teil, R, T) < 0) return 0;
    int64_t s[3] = { 0, 0, 0 };
    for (int v = RE2ARM_HAND_V0; v < RE2ARM_HAND_V0 + RE2ARM_HAND_N; v++) {
        const re15_md1_vertex_t *p = &mm->quad_vertices[v];
        for (int r = 0; r < 3; r++)
            s[r] += (((int64_t)R[r*3+0] * p->x + (int64_t)R[r*3+1] * p->y + (int64_t)R[r*3+2] * p->z) >> 12) + T[r];
    }
    for (int r = 0; r < 3; r++) hand[r] = (int32_t)(s[r] / RE2ARM_HAND_N);
    if (schulter) { schulter[0] = T[0]; schulter[1] = T[1]; schulter[2] = T[2]; }
    return 1;
}
int re15_re2arm_hand15(int slot, int32_t hand[3], int32_t schulter[3])
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const re15_actor_t *e = &g_actors[slot];
    if (!re15_re2arm_owns(e) || !re15_re2arm_modell_re15(e) || !s_arm[slot].koerper_an) return 0;
    return koerper_hand(e, &s_arm[slot], hand, schulter);
}
/* Halbe Breite der EM1A-Hand quer zum Arm (lokal x/z der Vertices 0..11) — Breite des Kontaktbands an Leon. */
static int hand_halbbreite(const re15_actor_t *e)
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const int teil = em1a_arm(e);
    if (!b || teil >= b->skel.bone_count) return 0;
    const re15_md1_mesh_t *mm = &b->md1.meshes[b->skel.bone_mesh_index[teil]];
    int x0 = 1 << 30, x1 = -(1 << 30), z0 = 1 << 30, z1 = -(1 << 30);
    for (int v = RE2ARM_HAND_V0; v < RE2ARM_HAND_V0 + RE2ARM_HAND_N; v++) {
        const re15_md1_vertex_t *p = &mm->quad_vertices[v];
        if (p->x < x0) x0 = p->x; if (p->x > x1) x1 = p->x;
        if (p->z < z0) z0 = p->z; if (p->z > z1) z1 = p->z;
    }
    const int hx = (x1 - x0) / 2, hz = (z1 - z0) / 2;
    return hx > hz ? hx : hz;
}

/* ---- HALTE-LAGE: Leon an die RE1.5-Hand (Vorgabe Runde 4, Punkt 2b; NUTZERWUNSCH s. KOERPER) --------------------------
 * Leons Koerper aus seiner PLD (Datei-Id-Tabelle @0x80073f70 -> PL%02X.PLD, Index work_vars[0x10] wie main.c
 * pc_sync_player_model; Teil 1 = EMR, Teil 2 = MD1, re15_pld_part) in der Opfer-Pose (Paar 3 der Bank, wie der Zeichner
 * main.c s_victim_skel). Runde 5/6: Leon wird beim Pin EINMAL an die Hand gestellt und steht dann fest (s. halte_loesen). */
extern uint8_t *re15_asset_read_file(const char *path, int *out_size);   /* PC-/PSX-Backend, Test-Stub */
static int s_pl_idx = -1;
static uint8_t *s_pl_buf = NULL;
static re15_emd_skeleton_t s_pl_sk;
static re15_md1_t s_pl_md1;
static int pl_modell_laden(void)
{
    const int idx = (int)(uint8_t)g_scd.work_vars[0x10];
    if (idx == s_pl_idx) return s_pl_buf != NULL;
    if (s_pl_buf) { free(s_pl_buf); s_pl_buf = NULL; }
    s_pl_idx = idx;
    char p[64];
    snprintf(p, sizeof p, "shared_assets/PSX/PLD/PL%02X.PLD", (unsigned)idx);
    int sz = 0;
    uint8_t *b = re15_asset_read_file(p, &sz);
    if (!b) return 0;
    unsigned long o1 = 0, l1 = 0, o2 = 0, l2 = 0;
    memset(&s_pl_sk, 0, sizeof s_pl_sk); memset(&s_pl_md1, 0, sizeof s_pl_md1);
    if (!re15_pld_part(b, (long)sz, RE15_PLD_EMR, &o1, &l1) || re15_emd_parse_skeleton(b + o1, (size_t)l1, &s_pl_sk) != 0 ||
        !re15_pld_part(b, (long)sz, RE15_PLD_MD1, &o2, &l2) || re15_md1_parse(b + o2, (int)l2, &s_pl_md1) != 0) {
        free(b); return 0;
    }
    s_pl_buf = b;
    return 1;
}
/* Runde 5 (Pruefbericht Runde 4 R3: Leon pendelte beim Halten ~690 mit Spruengen bis 192 je Bild, weil er jedes Bild neu an
 * seinen vordersten Vertex gestellt wurde). Jetzt wie RE2: die Figur steht das ganze Halten FEST - RE2 setzt PL+0x38/+0x40 nur
 * einmal beim Pin @0x80100C18-38, der Hook P1 @0x8010131C bewegt sie nicht. Die feste Lage wird EINMAL bestimmt, so dass die
 * RE1.5-Hand ueber die ganze Halte-Schleife (Opfer-Clip 0) an der Oberflaeche der Figur bleibt (NUTZERWUNSCH s. KOERPER;
 * Vorgabe Punkt 2: "die Hand liegt sichtbar an ihr (Schulter oder Arm), keine Finger tief im Koerper").
 * RUNDE 6 (Pruefbericht Runde 5 R1: das Spiel stand bei jedem Zugriff ~1,3 s): keine Rohsuche mehr, eine Figur-Tabelle beim
 * Raumladen. RUNDE 7 (Pruefbericht Runde 6 R1: die Figur lief an der Wand in den gewachsenen Arm, die Hand steckte 8 Bilder in
 * ihr, weil das Tor den Griff ablehnte; danach sprang sie 421 / ~570 an die Hand). Jetzt:
 *   PROFILE (profil_bauen, EINMAL je Figur beim Raumladen aus arm_init): die Figur in jedem Bild der Halte-Schleife (Opfer-Clip 0,
 *     PROF_HALTEN) und in ihrer eigenen Pose (Keyframe 0 ihrer PLD-EMR, PROF_EIGEN), figur-lokal (Blick 0, Lage 0); jede Flaeche
 *     wird abgetastet (Raster RE2ARM_PROF_RASTER), je Koerperteil und Hoehenband (RE2ARM_PROF_BH) die Bandmitte und je Sektor
 *     (RE2ARM_PROF_NA) der groesste Radius um sie. Dazu die Tabelle (Vertices je Bild) und die Dreiecke (fuer luft).
 *   ARM ALS HINDERNIS (arm_schiebt, PORT-WAHL nach dem RE2-Arm-Segment FUN_80034D0C, s. re15_re2arm_body_push_player): solange
 *     der RE1.5-Koerper zu sehen ist und die Figur nicht gehalten wird, schiebt der gezeichnete Arm (beide Arme, alle Vertices) die
 *     Figur in ihrer eigenen Pose so wenig wie moeglich aus sich heraus (32 Richtungen, nach ihrer Flur-Klemme FUN_8003b0a4
 *     @0x80031d70) - im Spieler-Pass und nach jedem Koerper-Bild. Kein Armpunkt steckt in der Figur, ohne dass sie gegriffen ist.
 *   HALTE-LAGE (halte_loesen, im Tor A3 jedes Zugriff-Bilds): mit dem Blick, den der Hook P0 ihr geben wird, je Richtung um die
 *     Hand der kleinste Abstand, bei dem der ganze Arm ausserhalb des Halte-Profils liegt; Lage, Klemme, Sprung von der jetzigen
 *     Lage. Vom kleinsten Sprung an (hoechstens RE2ARM_HALTE_PRUEF Lagen) die erste, an der die Hand in jedem Bild an der Figur
 *     liegt (luft). Gegriffen wird nur, wenn es eine solche Lage mit Sprung <= Handbreite gibt (kontakt_moeglich); der Pin nimmt
 *     genau diese Lage (Hook P0). Sonst greift der Arm nicht und schiebt nur (RE2-Rueckzug nach dem Zugriff wie bisher).
 * MASSE (Mess-/Pruefgroessen, keine Original-Konstanten):
 *   luft    = an der Halte-Lage je Bild der Schleife der kleinste 3D-Abstand der 12 Hand-Vertices zu den Dreiecken der Figur;
 *             Griff nur mit luft <= Handbreite in jedem Bild und im Mittel <= halbe Handbreite.
 *   Sprung  = Abstand Halte-Lage -> Lage vor dem Pin; Griff nur mit Sprung <= Handbreite. RE2 setzt die Figur auf die Hand
 *             (@0x80100C18-38), also bis 600 weit (Tor @0x801009BC) - der Port begrenzt das auf eine Handbreite (PORT-WAHL).
 *   Handbreite = 2 * hand_halbbreite (aus den Vertices 0..11: 2 * 101).
 *   kontakt = (nur Mess-Auskunft re15_re2arm_kontakt_rest, NICHT im Spielablauf) kleinster Abstand der 12 Hand-Vertices zu den
 *             Dreiecken der Figur im jetzigen Bild. */
#define RE2ARM_FIG_DREI 1536         /* Puffer: Leon-Dreiecke je Bild (PL00/PL04 haben weniger, gezaehlt in figur_dreiecke)  */
#define RE2ARM_PROF_Y0  (-3264)      /* Profil: Hoehenbaender ab y -3264 (Figur-lokal; PL00/PL04 reichen -3009..+59)        */
#define RE2ARM_PROF_BH  6            /* Bandhoehe 64 (Schiebung)                                                            */
#define RE2ARM_PROF_NB  54           /* 54 Baender: -3264..+192                                                             */
#define RE2ARM_PROF_NA  64           /* 64 Sektoren je 64/4096                                                              */
#define RE2ARM_PROF_SEKTOR 6         /* Winkel 0..4095 >> 6 = Sektor                                                        */
#define RE2ARM_PROF_TEILE 16         /* Koerperteile der Figur (PL00/PL04: 15)                                             */
#define RE2ARM_HALT_BILDER 64        /* Puffer: Bilder des Opfer-Clips 0 (Bank Paar 3: 19)                                 */
#define RE2ARM_PROF_RASTER 32       /* Abtastraster der Flaechen (hoechstens 16 Schritte je Kante)                           */
#define RE2ARM_ARM_PUNKTE 160        /* Puffer: Vertices eines EM1A-Arm-Meshes                                              */
#define RE2ARM_HALTE_NT 32           /* Richtungen um die Hand, je 128/4096                                                 */
#define RE2ARM_HALTE_RMAX 1536       /* weitester Abstand Handmitte -> Figur-Mitte, der versucht wird                       */
#define RE2ARM_HALTE_RSTEP 32
#define RE2ARM_HALTE_PRUEF 4
typedef struct { float p[3][3]; float c[3]; float r; uint8_t teil; } halte_drei_t;
static halte_drei_t s_fd[RE2ARM_FIG_DREI];
static double pt_tri(const double p[3], const float A[3], const float B[3], const float C[3])
{   /* Punkt-Dreieck-Abstand (Ericson, Real-Time Collision Detection 5.1.5) */
    double a[3], ab[3], ac[3], ap[3], q[3];
    for (int i = 0; i < 3; i++) { a[i] = A[i]; ab[i] = B[i] - A[i]; ac[i] = C[i] - A[i]; ap[i] = p[i] - A[i]; }
    const double d1 = ab[0]*ap[0] + ab[1]*ap[1] + ab[2]*ap[2], d2 = ac[0]*ap[0] + ac[1]*ap[1] + ac[2]*ap[2];
    if (d1 <= 0 && d2 <= 0) { for (int i = 0; i < 3; i++) q[i] = a[i]; goto fertig; }
    {   double bp[3]; for (int i = 0; i < 3; i++) bp[i] = p[i] - B[i];
        const double d3 = ab[0]*bp[0] + ab[1]*bp[1] + ab[2]*bp[2], d4 = ac[0]*bp[0] + ac[1]*bp[1] + ac[2]*bp[2];
        if (d3 >= 0 && d4 <= d3) { for (int i = 0; i < 3; i++) q[i] = B[i]; goto fertig; }
        const double vc = d1*d4 - d3*d2;
        if (vc <= 0 && d1 >= 0 && d3 <= 0) { const double v = d1 / (d1 - d3); for (int i = 0; i < 3; i++) q[i] = a[i] + v*ab[i]; goto fertig; }
        double cp[3]; for (int i = 0; i < 3; i++) cp[i] = p[i] - C[i];
        const double d5 = ab[0]*cp[0] + ab[1]*cp[1] + ab[2]*cp[2], d6 = ac[0]*cp[0] + ac[1]*cp[1] + ac[2]*cp[2];
        if (d6 >= 0 && d5 <= d6) { for (int i = 0; i < 3; i++) q[i] = C[i]; goto fertig; }
        const double vb = d5*d2 - d1*d6;
        if (vb <= 0 && d2 >= 0 && d6 <= 0) { const double w = d2 / (d2 - d6); for (int i = 0; i < 3; i++) q[i] = a[i] + w*ac[i]; goto fertig; }
        const double va = d3*d6 - d5*d4;
        if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0) {
            const double w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            for (int i = 0; i < 3; i++) q[i] = B[i] + w * (C[i] - B[i]);
            goto fertig;
        }
        const double den = 1.0 / (va + vb + vc), v = vb * den, w = vc * den;
        for (int i = 0; i < 3; i++) q[i] = a[i] + ab[i]*v + ac[i]*w;
    }
fertig:
    {   const double dx = p[0] - q[0], dy = p[1] - q[1], dz = p[2] - q[2]; return sqrt(dx*dx + dy*dy + dz*dz); }
}
/* Leons Dreiecke in Opfer-Clip 0 / Bild g, Blick yaw, Lage (0, pl->y, 0) - Rueckgabe Anzahl (0 = nicht berechenbar). */
static int figur_dreiecke(const re15_actor_t *pl, int16_t yaw, int g)
{
    if (!pl_modell_laden()) return 0;
    static re15_emd_skeleton_t vs;
    static re15_skel_pose_t P[RE15_EMD_MAX_BONES];
    static float W[2048][3];
    const re15_enemy_bank_t *vb = re15_enemy_find(0x1Au);
    int kf = 0;
    vs = s_pl_sk;
    if (vb && vb->victim_ok && vb->skel_victim.keyframe_data && vb->anim_victim.clip_count > 0 &&
        vb->anim_victim.clips[0].frame_count > 0) {
        vs.keyframe_data      = vb->skel_victim.keyframe_data;
        vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
        vs.keyframe_count     = vb->skel_victim.keyframe_count;
        vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
        kf = (int)(re15_emd_get_frame_entry(&vb->anim_victim, 0, g % vb->anim_victim.clips[0].frame_count) & 0xfffu);
    }
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    const int rv = re15_skel_compute_pose(&vs, kf, P);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int n = 0;
    for (int bi = 0; bi < s_pl_sk.bone_count; bi++) {
        const int mi = s_pl_sk.bone_mesh_index[bi];
        if (mi < 0 || mi >= s_pl_md1.mesh_count) continue;
        const re15_md1_mesh_t *mm = &s_pl_md1.meshes[mi];
        for (int q = 0; q < 2; q++) {
            const re15_md1_vertex_t *vv = q ? mm->quad_vertices : mm->tri_vertices;
            const int nv = q ? mm->quad_vertex_count : mm->tri_vertex_count;
            const int nn = nv < 2048 ? nv : 2048;
            for (int i = 0; i < nn; i++) {
                int32_t l[3], w[3];
                for (int r = 0; r < 3; r++)
                    l[r] = (int32_t)(((int64_t)P[bi].rot[r*3] * vv[i].x + (int64_t)P[bi].rot[r*3+1] * vv[i].y +
                                      (int64_t)P[bi].rot[r*3+2] * vv[i].z) >> 12) + P[bi].trans[r];
                re15_skel_bone_to_world(l, yaw, 0, pl->y, 0, w);
                W[i][0] = (float)w[0]; W[i][1] = (float)w[1]; W[i][2] = (float)w[2];
            }
            const int nf = q ? mm->quad_count : mm->triangle_count;
            for (int f = 0; f < nf; f++) {
                int ix[4], k = 3;
                if (q) { const re15_md1_quad_t *Q = &mm->quads[f]; ix[0] = Q->v0; ix[1] = Q->v1; ix[2] = Q->v2; ix[3] = Q->v3; k = 4; }
                else   { const re15_md1_triangle_t *T = &mm->triangles[f]; ix[0] = T->v0; ix[1] = T->v1; ix[2] = T->v2; }
                if (ix[0] >= nn || ix[1] >= nn || ix[2] >= nn || (k == 4 && ix[3] >= nn)) continue;
                for (int t = 0; t < k - 2; t++) {               /* Quad: (v0,v1,v2) + (v1,v3,v2) wie der PSX-Zeichner */
                    if (n >= RE2ARM_FIG_DREI) return n;
                    const int a0 = t ? ix[1] : ix[0], a1 = t ? ix[3] : ix[1], a2 = ix[2];
                    halte_drei_t *D = &s_fd[n++];
                    D->teil = (uint8_t)bi;
                    for (int r = 0; r < 3; r++) {
                        D->p[0][r] = W[a0][r]; D->p[1][r] = W[a1][r]; D->p[2][r] = W[a2][r];
                        D->c[r] = (W[a0][r] + W[a1][r] + W[a2][r]) / 3.0f;
                    }
                    float rr = 0.0f;
                    for (int v = 0; v < 3; v++) {
                        const float ex = D->p[v][0] - D->c[0], ey = D->p[v][1] - D->c[1], ez = D->p[v][2] - D->c[2];
                        const float l2 = sqrtf(ex*ex + ey*ey + ez*ez);
                        if (l2 > rr) rr = l2;
                    }
                    D->r = rr;
                }
            }
        }
    }
    return n;
}
/* Hand des greifenden EM1A-Arms: 12 Vertices (Welt), Mitte und Schultergelenk (koerper_hand). */
static int hand_punkte(const re15_actor_t *e, const re2arm_t *a, double hv[RE2ARM_HAND_N][3], int32_t h[3], int32_t s[3])
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    const int teil = em1a_arm(e);
    if (!b || teil >= b->skel.bone_count || !koerper_hand(e, a, h, s)) return 0;
    const re15_md1_mesh_t *mm = &b->md1.meshes[b->skel.bone_mesh_index[teil]];
    int32_t R[9], T[3];
    if (koerper_teil_welt(e, a, teil, R, T) < 0) return 0;
    for (int v = 0; v < RE2ARM_HAND_N; v++) {
        const re15_md1_vertex_t *p = &mm->quad_vertices[RE2ARM_HAND_V0 + v];
        for (int r = 0; r < 3; r++)
            hv[v][r] = (double)((((int64_t)R[r*3] * p->x + (int64_t)R[r*3+1] * p->y + (int64_t)R[r*3+2] * p->z) >> 12) + T[r]);
    }
    return 1;
}
/* kontakt / innen (s. MASSE) fuer Leon an (px, pz) mit den eben gebauten n Dreiecken. */
static void halte_mass(int n, const double hv[RE2ARM_HAND_N][3], int32_t px, int32_t pz, double *kontakt, int *innen)
{
    double best = 1e30;
    double m[3] = { 0, 0, 0 };                           /* Handmitte, Leon-lokal */
    for (int v = 8; v < 12; v++) for (int r = 0; r < 3; r++) m[r] += hv[v][r] / 4.0;
    m[0] -= px; m[2] -= pz;
    uint8_t schnitt[3][RE15_EMD_MAX_BONES]; memset(schnitt, 0, sizeof schnitt);
    for (int i = 0; i < n; i++) {
        const halte_drei_t *D = &s_fd[i];
        for (int v = 0; v < RE2ARM_HAND_N; v++) {
            const double p[3] = { hv[v][0] - px, hv[v][1], hv[v][2] - pz };
            const double cx = p[0] - D->c[0], cy = p[1] - D->c[1], cz = p[2] - D->c[2];
            const double grob = sqrt(cx*cx + cy*cy + cz*cz) - D->r;
            if (grob >= best) continue;
            const double d = pt_tri(p, D->p[0], D->p[1], D->p[2]);
            if (d < best) best = d;
        }
        /* drei Strahlen von der Handmitte (+y, +x, +z): Schnitt, wenn die Handmitte in der Projektion des Dreiecks liegt und
         * die Ebene in Strahlrichtung vor ihr */
        for (int k = 0; k < 3; k++) {
            static const int8_t AX[3] = { 1, 0, 2 }, PU[3] = { 0, 1, 0 }, PW[3] = { 2, 2, 1 };   /* Strahl y/x/z, Ebene xz/yz/xy */
            const int ax = AX[k], pu = PU[k], pw = PW[k];
            const double a0 = D->p[0][pu], b0 = D->p[0][pw], a1 = D->p[1][pu], b1 = D->p[1][pw], a2 = D->p[2][pu], b2 = D->p[2][pw];
            const double den = (b1 - b2) * (a0 - a2) + (a2 - a1) * (b0 - b2);
            if (fabs(den) < 1e-6) continue;
            const double l0 = ((b1 - b2) * (m[pu] - a2) + (a2 - a1) * (m[pw] - b2)) / den;
            const double l1 = ((b2 - b0) * (m[pu] - a2) + (a0 - a2) * (m[pw] - b2)) / den;
            const double l2 = 1.0 - l0 - l1;
            if (l0 < 0 || l1 < 0 || l2 < 0) continue;
            const double c = l0 * D->p[0][ax] + l1 * D->p[1][ax] + l2 * D->p[2][ax];
            if (c > m[ax] && D->teil < RE15_EMD_MAX_BONES) schnitt[k][D->teil] ^= 1u;
        }
    }
    int in = 0;                                          /* innen = in einem Teil zeigen mind. 2 der 3 Strahlen "innen" */
    for (int t = 0; t < RE15_EMD_MAX_BONES; t++) if (schnitt[0][t] + schnitt[1][t] + schnitt[2][t] >= 2) in = 1;
    *kontakt = best; *innen = in;
}
/* ---- Runde 7: FIGUR-TABELLE + PROFILE, HALTE-LAGE mit dem kleinsten Sprung, ARM ALS HINDERNIS (Block HALTE-LAGE) ---- */
static uint32_t s_rechen = 0;            /* Rechenschritte seit dem letzten Abholen - Messgroesse Riegel T1 */
uint32_t re15_re2arm_rechenschritte(int zuruecksetzen)
{
    const uint32_t r = s_rechen;
    if (zuruecksetzen) s_rechen = 0;
    return r;
}
static int16_t (*s_tab)[3] = NULL;       /* [bild * s_tab_nv + v]: figur-lokal (Blick 0, Lage 0), Halte-Schleife */
static int s_tab_nv = 0, s_tab_fc = 0, s_tab_idx = -2, s_tab_opfer = -1;
static uint16_t (*s_tf)[3] = NULL;       /* Dreiecke der Figur als Indizes in eine Tabellen-Zeile (Quads als (v0,v1,v2)+(v1,v3,v2)) */
static int s_tf_n = 0;
/* PROFILE je Satz (PROF_HALTEN = Opfer-Clip 0 ueber alle Bilder, PROF_EIGEN = die eigene Pose der Figur), Koerperteil und
 * Hoehenband: Bandmitte und groesster Radius / 4 je Sektor um sie (0 = leer). Bau in zwei Durchgaengen (Mitten, dann Radien). */
enum { PROF_HALTEN = 0, PROF_EIGEN = 1, PROF_SAETZE = 2 };
static uint8_t  s_pp_r[PROF_SAETZE][RE2ARM_PROF_TEILE][RE2ARM_PROF_NB][RE2ARM_PROF_NA];
static int16_t  s_pp_c[PROF_SAETZE][RE2ARM_PROF_TEILE][RE2ARM_PROF_NB][2];
static uint16_t s_pp_maske[PROF_SAETZE][RE2ARM_PROF_NB];
static int32_t  s_pp_rmax[PROF_SAETZE];
static int32_t  s_pp_sum[RE2ARM_PROF_TEILE][RE2ARM_PROF_NB][3];   /* nur beim Bau: Summe x, z, Anzahl */
static int s_pp_satz = 0, s_pp_durchgang = 0, s_pp_teil = 0;
static void prof_punkt(int32_t x, int32_t y, int32_t z)
{
    const int b = (y - RE2ARM_PROF_Y0) >> RE2ARM_PROF_BH;
    if (b < 0 || b >= RE2ARM_PROF_NB || s_pp_teil < 0 || s_pp_teil >= RE2ARM_PROF_TEILE) return;
    if (s_pp_durchgang == 0) {
        int32_t *q = s_pp_sum[s_pp_teil][b];
        q[0] += x; q[1] += z; q[2]++;
        const int32_t r0 = (int32_t)re15_squareroot0((uint32_t)((int64_t)x * x + (int64_t)z * z)) + 1;
        if (r0 > s_pp_rmax[s_pp_satz]) s_pp_rmax[s_pp_satz] = r0;
        return;
    }
    const int32_t dx = x - s_pp_c[s_pp_satz][s_pp_teil][b][0], dz = z - s_pp_c[s_pp_satz][s_pp_teil][b][1];
    const int s = (re15_ratan2(dz, dx) & 0xfff) >> RE2ARM_PROF_SEKTOR;
    const uint32_t r = (re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz)) + 1u + 3u) / 4u;   /* aufrunden */
    uint8_t *q = &s_pp_r[s_pp_satz][s_pp_teil][b][s];
    const uint8_t v = (uint8_t)(r < 1u ? 1u : r > 255u ? 255u : r);
    if (v > *q) *q = v;
}
static void prof_drei(const int32_t A[3], const int32_t B[3], const int32_t C[3])
{   /* Dreieck abtasten: Raster mit hoechstens RE2ARM_PROF_RASTER Einheiten Abstand (laengste Achsen-Kante, 1..16 Schritte) */
    int32_t m = 0;
    for (int r = 0; r < 3; r++) {
        const int32_t e1 = abs(B[r] - A[r]), e2 = abs(C[r] - A[r]), e3 = abs(C[r] - B[r]);
        if (e1 > m) m = e1;
        if (e2 > m) m = e2;
        if (e3 > m) m = e3;
    }
    int n = (int)((m + RE2ARM_PROF_RASTER - 1) / RE2ARM_PROF_RASTER);
    if (n < 1) n = 1;
    if (n > 16) n = 16;
    for (int i = 0; i <= n; i++)
        for (int j = 0; i + j <= n; j++) {
            int32_t p[3];
            for (int r = 0; r < 3; r++) p[r] = A[r] + ((B[r] - A[r]) * i) / n + ((C[r] - A[r]) * j) / n;
            prof_punkt(p[0], p[1], p[2]);
        }
}
/* Ein Profil-Satz: fc Bilder (Pose je Bild aus sk/kf_fn), bei PROF_HALTEN zugleich die Tabelle (s_tab). */
static int prof_satz_bauen(int satz, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an, int fc, int nv)
{
    static re15_emd_skeleton_t vs;
    static re15_skel_pose_t P[RE15_EMD_MAX_BONES];
    static int32_t L[2][2048][3];
    s_pp_satz = satz;
    memset(s_pp_r[satz], 0, sizeof s_pp_r[satz]); memset(s_pp_c[satz], 0, sizeof s_pp_c[satz]);
    memset(s_pp_maske[satz], 0, sizeof s_pp_maske[satz]); memset(s_pp_sum, 0, sizeof s_pp_sum); s_pp_rmax[satz] = 0;
    for (s_pp_durchgang = 0; s_pp_durchgang < 2; s_pp_durchgang++) {
        for (int g = 0; g < fc; g++) {
            vs = *sk;
            const int kf = an ? (int)(re15_emd_get_frame_entry(an, 0, g) & 0xfffu) : 0;
            void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
            const int rv = re15_skel_compute_pose(&vs, kf, P);
            g_anim_pose_actor = save;
            if (rv != 0) return 0;
            int k = g * nv;
            for (int bi = 0; bi < s_pl_sk.bone_count; bi++) {
                const int mi = s_pl_sk.bone_mesh_index[bi];
                if (mi < 0 || mi >= s_pl_md1.mesh_count) continue;
                const re15_md1_mesh_t *mm = &s_pl_md1.meshes[mi];
                int nq[2];
                s_pp_teil = bi;
                for (int q = 0; q < 2; q++) {
                    const re15_md1_vertex_t *vv = q ? mm->quad_vertices : mm->tri_vertices;
                    const int n = q ? mm->quad_vertex_count : mm->tri_vertex_count;
                    nq[q] = n < 2048 ? n : 2048;
                    for (int i = 0; i < n; i++, k++)
                        for (int r = 0; r < 3; r++) {
                            const int32_t l = (int32_t)(((int64_t)P[bi].rot[r*3] * vv[i].x + (int64_t)P[bi].rot[r*3+1] * vv[i].y +
                                                         (int64_t)P[bi].rot[r*3+2] * vv[i].z) >> 12) + P[bi].trans[r];
                            if (i < 2048) L[q][i][r] = l;
                            if (satz == PROF_HALTEN && s_tab) s_tab[k][r] = (int16_t)(l < -32768 ? -32768 : l > 32767 ? 32767 : l);
                        }
                }
                for (int f = 0; f < mm->triangle_count; f++) {
                    const re15_md1_triangle_t *T = &mm->triangles[f];
                    if (T->v0 >= nq[0] || T->v1 >= nq[0] || T->v2 >= nq[0]) continue;
                    prof_drei(L[0][T->v0], L[0][T->v1], L[0][T->v2]);
                }
                for (int f = 0; f < mm->quad_count; f++) {          /* Quad: (v0,v1,v2) + (v1,v3,v2) wie der PSX-Zeichner */
                    const re15_md1_quad_t *Q = &mm->quads[f];
                    if (Q->v0 >= nq[1] || Q->v1 >= nq[1] || Q->v2 >= nq[1] || Q->v3 >= nq[1]) continue;
                    prof_drei(L[1][Q->v0], L[1][Q->v1], L[1][Q->v2]);
                    prof_drei(L[1][Q->v1], L[1][Q->v3], L[1][Q->v2]);
                }
            }
        }
        if (s_pp_durchgang == 0)                          /* Bandmitten aus dem ersten Durchgang */
            for (int t = 0; t < RE2ARM_PROF_TEILE; t++)
                for (int b = 0; b < RE2ARM_PROF_NB; b++) {
                    const int32_t *q = s_pp_sum[t][b];
                    if (q[2] <= 0) continue;
                    s_pp_c[satz][t][b][0] = (int16_t)(q[0] / q[2]); s_pp_c[satz][t][b][1] = (int16_t)(q[1] / q[2]);
                    s_pp_maske[satz][b] |= (uint16_t)(1u << t);
                }
    }
    for (int t = 0; t < RE2ARM_PROF_TEILE; t++)           /* leere Sektoren zwischen belegten Nachbarn schliessen (Abtastluecke) */
        for (int b = 0; b < RE2ARM_PROF_NB; b++) {
            if (!(s_pp_maske[satz][b] & (1u << t))) continue;
            uint8_t z[RE2ARM_PROF_NA];
            memcpy(z, s_pp_r[satz][t][b], sizeof z);
            for (int s = 0; s < RE2ARM_PROF_NA; s++) {
                if (z[s]) continue;
                const uint8_t l = z[(s + RE2ARM_PROF_NA - 1) % RE2ARM_PROF_NA], r = z[(s + 1) % RE2ARM_PROF_NA];
                if (l && r) s_pp_r[satz][t][b][s] = l < r ? l : r;
            }
        }
    return 1;
}
/* FIGUR-TABELLE, Dreiecke und PROFILE einmal je Figur (beim Raumladen aus arm_init). */
static int profil_bauen(void)
{
    if (!pl_modell_laden()) return 0;
    const re15_enemy_bank_t *vb = re15_enemy_find(0x1Au);
    const int opfer = (vb && vb->victim_ok && vb->skel_victim.keyframe_data && vb->anim_victim.clip_count > 0 &&
                       vb->anim_victim.clips[0].frame_count > 0) ? 1 : 0;
    if (s_tab && s_tab_idx == s_pl_idx && s_tab_opfer == opfer) return 1;
    free(s_tab); s_tab = NULL; s_tab_nv = 0; s_tab_fc = 0; s_tab_idx = -2;
    int nv = 0, nf = 0;
    for (int bi = 0; bi < s_pl_sk.bone_count; bi++) {
        const int mi = s_pl_sk.bone_mesh_index[bi];
        if (mi < 0 || mi >= s_pl_md1.mesh_count) continue;
        nv += s_pl_md1.meshes[mi].tri_vertex_count + s_pl_md1.meshes[mi].quad_vertex_count;
        nf += s_pl_md1.meshes[mi].triangle_count + 2 * s_pl_md1.meshes[mi].quad_count;
    }
    int fc = opfer ? vb->anim_victim.clips[0].frame_count : 1;
    if (fc > RE2ARM_HALT_BILDER) fc = RE2ARM_HALT_BILDER;
    if (nv <= 0 || nv > 65535) return 0;
    s_tab = (int16_t (*)[3])malloc((size_t)fc * (size_t)nv * sizeof *s_tab);
    free(s_tf); s_tf = (uint16_t (*)[3])malloc((size_t)(nf > 0 ? nf : 1) * sizeof *s_tf); s_tf_n = 0;
    if (!s_tab || !s_tf) { free(s_tab); s_tab = NULL; return 0; }
    {   int k0 = 0;
        for (int bi = 0; bi < s_pl_sk.bone_count; bi++) {
            const int mi = s_pl_sk.bone_mesh_index[bi];
            if (mi < 0 || mi >= s_pl_md1.mesh_count) continue;
            const re15_md1_mesh_t *mm = &s_pl_md1.meshes[mi];
            const int kt = k0, kq = k0 + mm->tri_vertex_count, tn = mm->tri_vertex_count, qn = mm->quad_vertex_count;
            for (int f = 0; f < mm->triangle_count; f++) {
                const re15_md1_triangle_t *T = &mm->triangles[f];
                if (T->v0 >= tn || T->v1 >= tn || T->v2 >= tn) continue;
                s_tf[s_tf_n][0] = (uint16_t)(kt + T->v0); s_tf[s_tf_n][1] = (uint16_t)(kt + T->v1); s_tf[s_tf_n][2] = (uint16_t)(kt + T->v2);
                s_tf_n++;
            }
            for (int f = 0; f < mm->quad_count; f++) {
                const re15_md1_quad_t *Q = &mm->quads[f];
                if (Q->v0 >= qn || Q->v1 >= qn || Q->v2 >= qn || Q->v3 >= qn) continue;
                s_tf[s_tf_n][0] = (uint16_t)(kq + Q->v0); s_tf[s_tf_n][1] = (uint16_t)(kq + Q->v1); s_tf[s_tf_n][2] = (uint16_t)(kq + Q->v2); s_tf_n++;
                s_tf[s_tf_n][0] = (uint16_t)(kq + Q->v1); s_tf[s_tf_n][1] = (uint16_t)(kq + Q->v3); s_tf[s_tf_n][2] = (uint16_t)(kq + Q->v2); s_tf_n++;
            }
            k0 += tn + qn;
        }
    }
    re15_emd_skeleton_t sk = s_pl_sk;
    if (opfer) {
        sk.keyframe_data      = vb->skel_victim.keyframe_data;
        sk.keyframe_data_size = vb->skel_victim.keyframe_data_size;
        sk.keyframe_count     = vb->skel_victim.keyframe_count;
        sk.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    }
    if (!prof_satz_bauen(PROF_HALTEN, &sk, opfer ? &vb->anim_victim : NULL, fc, nv) ||
        !prof_satz_bauen(PROF_EIGEN, &s_pl_sk, NULL, 1, nv)) { free(s_tab); s_tab = NULL; return 0; }
    s_tab_nv = nv; s_tab_fc = fc; s_tab_idx = s_pl_idx; s_tab_opfer = opfer;
    if (getenv("RE15_RE2_TRACE")) {                       /* Messschiene: wann Tabelle und Profile entstehen (Raumladen, nicht Zugriff) */
        FILE *o = re15_re2_trace_out() ? re15_re2_trace_out() : stderr;
        fprintf(o, "[re15halt] F%u Figur-Tabelle PL%02X: %d Bilder x %d Vertices, %d Dreiecke, Profile %d Baender x %d Sektoren "
                   "(Opfer-Pose %d)\n", (unsigned)g_engine.frame_count, (unsigned)s_pl_idx, fc, nv, s_tf_n, RE2ARM_PROF_NB,
                RE2ARM_PROF_NA, opfer);
    }
    return 1;
}
/* Abstand eines figur-lokalen Punkts (x, y ueber dem Boden der Figur, z) vor dem Profil `satz`: > 0 = draussen, <= 0 = drin;
 * INT32_MAX = in dieser Hoehe/Richtung ist nichts von der Figur. */
static int32_t prof_marge(int satz, int32_t x, int32_t y, int32_t z)
{
    if ((int64_t)x * x + (int64_t)z * z > (int64_t)s_pp_rmax[satz] * s_pp_rmax[satz]) return INT32_MAX;   /* weiter als jedes Teil */
    const int b = (y - RE2ARM_PROF_Y0) >> RE2ARM_PROF_BH;
    if (b < 0 || b >= RE2ARM_PROF_NB || !s_pp_maske[satz][b]) return INT32_MAX;
    int32_t m = INT32_MAX;
    for (int t = 0; t < RE2ARM_PROF_TEILE; t++) {
        if (!(s_pp_maske[satz][b] & (1u << t))) continue;
        const int32_t dx = x - s_pp_c[satz][t][b][0], dz = z - s_pp_c[satz][t][b][1];
        if (dx > 1024 || dx < -1024 || dz > 1024 || dz < -1024) continue;   /* Radius hoechstens 255 * 4 */
        s_rechen++;
        const int32_t R = 4 * (int32_t)s_pp_r[satz][t][b][(re15_ratan2(dz, dx) & 0xfff) >> RE2ARM_PROF_SEKTOR];
        if (!R) continue;
        const int32_t mm = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz)) - R;
        if (mm < m) m = mm;
    }
    return m;
}
/* Alle Punkte eines gezeichneten RE1.5-Arms (Teil 2/3: erst die Quad-Vertices - davon 0..11 = die Hand -, dann die
 * Dreieck-Vertices) in Welt, dazu die Handmitte (Mittel der Quad-Vertices 8..11). Rueckgabe Anzahl (0 = nicht berechenbar). */
static int arm_punkte(const re15_actor_t *e, const re2arm_t *a, int teil, int32_t (*w)[3], int32_t hc[3])
{
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || teil >= b->skel.bone_count) return 0;
    const int mi = b->skel.bone_mesh_index[teil];
    if (mi < 0 || mi >= b->md1.mesh_count) return 0;
    const re15_md1_mesh_t *mm = &b->md1.meshes[mi];
    if (!mm->quad_vertices || mm->quad_vertex_count < RE2ARM_HAND_V0 + RE2ARM_HAND_N) return 0;
    int32_t R[9], T[3];
    if (koerper_teil_welt(e, a, teil, R, T) < 0) return 0;
    int n = 0;
    for (int q = 1; q >= 0; q--) {
        const re15_md1_vertex_t *vv = q ? mm->quad_vertices : mm->tri_vertices;
        const int nv = q ? mm->quad_vertex_count : mm->tri_vertex_count;
        for (int i = 0; vv && i < nv && n < RE2ARM_ARM_PUNKTE; i++, n++)
            for (int r = 0; r < 3; r++)
                w[n][r] = (int32_t)((((int64_t)R[r*3] * vv[i].x + (int64_t)R[r*3+1] * vv[i].y + (int64_t)R[r*3+2] * vv[i].z) >> 12) + T[r]);
    }
    for (int r = 0; r < 3; r++) hc[r] = (w[8][r] + w[9][r] + w[10][r] + w[11][r]) / 4;
    return n;
}
/* Sind mit dem Versatz (ox, oz) alle Armpunkte d ausserhalb des Profils `satz`? mh = kleinste Marge der 12 Hand-Punkte. */
static int arm_draussen(int satz, int n, const int32_t (*d)[3], int32_t ox, int32_t oz, int32_t *mh)
{
    int32_t m_hand = INT32_MAX;
    for (int v = 0; v < n; v++) {
        const int32_t m = prof_marge(satz, d[v][0] + ox, d[v][1], d[v][2] + oz);
        if (m < 0) return 0;
        if (v < RE2ARM_HAND_N && m < m_hand) m_hand = m;
    }
    if (mh) *mh = m_hand;
    return 1;
}
/* Kleinster Abstand r in Richtung (ex, ez), ab dem alle Armpunkte draussen sind (Raster RSTEP, dann 5 Halbierungen);
 * -1 = bis RMAX keiner. */
static int32_t arm_frei_ab(int satz, int n, const int32_t (*d)[3], int32_t ex, int32_t ez, int32_t *mh)
{
    int32_t r_in = -1, r_aus = -1;
    for (int32_t r = 0; r <= RE2ARM_HALTE_RMAX; r += RE2ARM_HALTE_RSTEP) {
        if (arm_draussen(satz, n, d, (int32_t)(((int64_t)r * ex) >> 12), (int32_t)(((int64_t)r * ez) >> 12), mh)) { r_aus = r; break; }
        r_in = r;
    }
    if (r_aus < 0 || r_in < 0) return r_aus;
    for (int k = 0; k < 5; k++) {
        const int32_t r = (r_in + r_aus) / 2;
        int32_t m2 = INT32_MAX;
        if (arm_draussen(satz, n, d, (int32_t)(((int64_t)r * ex) >> 12), (int32_t)(((int64_t)r * ez) >> 12), &m2)) {
            r_aus = r; if (mh) *mh = m2;
        } else r_in = r;
    }
    return r_aus;
}
/* luft (s. MASSE): je Bild der Schleife der kleinste 3D-Abstand der 12 Hand-Punkte (figur-lokal hl) zu den Dreiecken der Figur
 * (nur Dreiecke, deren Kasten den Kasten der Hand plus 2 * hb beruehrt); *lmax = der groesste ueber die Schleife, Rueckgabe =
 * Summe (oder -1, wenn in einem Bild kein Dreieck so nahe liegt). */
static int32_t halte_luft(const int32_t hl[RE2ARM_HAND_N][3], int32_t hb, int32_t *lmax)
{
    int32_t lo[3] = { INT32_MAX, INT32_MAX, INT32_MAX }, hi[3] = { INT32_MIN, INT32_MIN, INT32_MIN };
    double hp[RE2ARM_HAND_N][3];
    for (int v = 0; v < RE2ARM_HAND_N; v++)
        for (int r = 0; r < 3; r++) {
            if (hl[v][r] < lo[r]) lo[r] = hl[v][r];
            if (hl[v][r] > hi[r]) hi[r] = hl[v][r];
            hp[v][r] = (double)hl[v][r];
        }
    const int32_t R = 2 * hb;
    for (int r = 0; r < 3; r++) { lo[r] -= R; hi[r] += R; }
    int32_t sum = 0; *lmax = 0;
    for (int g = 0; g < s_tab_fc; g++) {
        double best = 1e30;
        const int16_t (*t)[3] = (const int16_t (*)[3])&s_tab[g * s_tab_nv];
        for (int f = 0; f < s_tf_n; f++) {
            const int16_t *A = t[s_tf[f][0]], *B = t[s_tf[f][1]], *C = t[s_tf[f][2]];
            int aus = 0;
            for (int r = 0; r < 3 && !aus; r++) {
                const int32_t mn = A[r] < B[r] ? (A[r] < C[r] ? A[r] : C[r]) : (B[r] < C[r] ? B[r] : C[r]);
                const int32_t mx = A[r] > B[r] ? (A[r] > C[r] ? A[r] : C[r]) : (B[r] > C[r] ? B[r] : C[r]);
                if (mx < lo[r] || mn > hi[r]) aus = 1;
            }
            if (aus) continue;
            s_rechen++;
            const float fa[3] = { A[0], A[1], A[2] }, fb[3] = { B[0], B[1], B[2] }, fcc[3] = { C[0], C[1], C[2] };
            for (int v = 0; v < RE2ARM_HAND_N; v++) {
                const double dd = pt_tri(hp[v], fa, fb, fcc);
                if (dd < best) best = dd;
            }
        }
        if (best > (double)R) return -1;
        const int32_t dd = (int32_t)(best + 0.5);
        if (dd > *lmax) *lmax = dd;
        sum += dd;
    }
    return sum;
}
typedef struct { int ok; int32_t x, z, sprung, luft_max, luft_mittel; int richtung, kandidaten; } halte_t;
/* HALTE-LAGE (s. Block HALTE-LAGE): die Lage der Figur (Blick yaw, Boden py) mit dem kleinsten Sprung von (px, pz) - hoechstens
 * sprung_max -, an der der ganze gezeichnete Arm ausserhalb des Halte-Profils liegt und die Hand in jedem Bild der Schleife an der
 * Figur (luft), nach der Flur-Klemme der Figur (FUN_8003b0a4 @0x80031d70). Rueckgabe 1 = gefunden. */
static int halte_loesen(const re15_actor_t *e, const re2arm_t *a, int16_t yaw, int32_t py, int32_t px, int32_t pz,
                        int32_t sprung_max, int32_t mittel_max, halte_t *o)
{
    memset(o, 0, sizeof *o); o->sprung = INT32_MAX;
    if (!profil_bauen()) return 0;
    static int32_t w[RE2ARM_ARM_PUNKTE][3], d[RE2ARM_ARM_PUNKTE][3];
    int32_t hc[3];
    const int n = arm_punkte(e, a, em1a_arm(e), w, hc);
    if (n < RE2ARM_HAND_N) return 0;
    const int32_t hb = hand_halbbreite(e);
    const int32_t cs = re15_cos_q12((int)yaw), sn = re15_sin_q12((int)yaw);   /* wie re15_skel_bone_to_world */
    for (int v = 0; v < n; v++) {                        /* figur-lokal relativ zur Handmitte: Rot(-yaw) (w - hc) */
        const int32_t dx = w[v][0] - hc[0], dz = w[v][2] - hc[2];
        d[v][0] = (int32_t)(((int64_t)cs * dx - (int64_t)sn * dz) >> 12);
        d[v][1] = w[v][1] - py;
        d[v][2] = (int32_t)(((int64_t)sn * dx + (int64_t)cs * dz) >> 12);
    }
    /* 1. je Richtung: der kleinste Abstand, bei dem der ganze Arm draussen ist; Lage, Klemme, Sprung (nach Sprung sortiert) */
    int32_t k_sp[RE2ARM_HALTE_NT], k_ox[RE2ARM_HALTE_NT], k_oz[RE2ARM_HALTE_NT], k_x[RE2ARM_HALTE_NT], k_z[RE2ARM_HALTE_NT];
    int k_th[RE2ARM_HALTE_NT], nk = 0;
    for (int t = 0; t < RE2ARM_HALTE_NT; t++) {
        const int th = t * (4096 / RE2ARM_HALTE_NT);
        const int32_t ex = re15_cos_q12(th), ez = re15_sin_q12(th);
        const int32_t r = arm_frei_ab(PROF_HALTEN, n, (const int32_t (*)[3])d, ex, ez, NULL);
        if (r < 0) continue;
        int32_t ox = (int32_t)(((int64_t)r * ex) >> 12), oz = (int32_t)(((int64_t)r * ez) >> 12);
        const int32_t qx = hc[0] - (int32_t)(((int64_t)cs * ox + (int64_t)sn * oz) >> 12);
        const int32_t qz = hc[2] - (int32_t)((-(int64_t)sn * ox + (int64_t)cs * oz) >> 12);
        int32_t cx = qx, cz = qz;
        if (g_room_rdt_ok) re15_collision_constrain(&g_room_rdt, px, pz, &cx, &cz);   /* FUN_8003b0a4 @0x80031d70 */
        if (cx != qx || cz != qz) {                      /* geklemmt: an der wirklichen Lage nachpruefen */
            const int32_t ux = hc[0] - cx, uz = hc[2] - cz;
            ox = (int32_t)(((int64_t)cs * ux - (int64_t)sn * uz) >> 12);
            oz = (int32_t)(((int64_t)sn * ux + (int64_t)cs * uz) >> 12);
            if (!arm_draussen(PROF_HALTEN, n, (const int32_t (*)[3])d, ox, oz, NULL)) continue;
        }
        const int32_t sx = cx - px, sz = cz - pz;
        const int32_t sp = (int32_t)re15_squareroot0((uint32_t)((int64_t)sx * sx + (int64_t)sz * sz));
        if (sp > sprung_max) continue;
        int i = nk++;
        while (i > 0 && k_sp[i - 1] > sp) {
            k_sp[i] = k_sp[i - 1]; k_ox[i] = k_ox[i - 1]; k_oz[i] = k_oz[i - 1]; k_x[i] = k_x[i - 1]; k_z[i] = k_z[i - 1];
            k_th[i] = k_th[i - 1]; i--;
        }
        k_sp[i] = sp; k_ox[i] = ox; k_oz[i] = oz; k_x[i] = cx; k_z[i] = cz; k_th[i] = th;
    }
    o->kandidaten = nk;
    /* 2. vom kleinsten Sprung an (hoechstens RE2ARM_HALTE_PRUEF Lagen) die erste, an der die Hand in jedem Bild der Schleife an
     *    der Figur liegt: luft hoechstens eine Handbreite, im Mittel hoechstens mittel_max (MASSE) */
    int32_t mit[RE2ARM_HALTE_PRUEF], mx[RE2ARM_HALTE_PRUEF];
    for (int i = 0; i < nk && i < RE2ARM_HALTE_PRUEF; i++) {
        int32_t hl[RE2ARM_HAND_N][3], lmax = 0;
        for (int v = 0; v < RE2ARM_HAND_N; v++) { hl[v][0] = d[v][0] + k_ox[i]; hl[v][1] = d[v][1]; hl[v][2] = d[v][2] + k_oz[i]; }
        const int32_t sum = halte_luft((const int32_t (*)[3])hl, hb, &lmax);
        mx[i] = lmax; mit[i] = (sum < 0 || lmax > 2 * hb) ? INT32_MAX : sum / s_tab_fc;
    }
    for (int i = 0; i < nk && i < RE2ARM_HALTE_PRUEF; i++) {
        if (mit[i] > mittel_max) continue;
        o->ok = 1; o->x = k_x[i]; o->z = k_z[i]; o->sprung = k_sp[i]; o->richtung = k_th[i];
        o->luft_max = mx[i]; o->luft_mittel = mit[i];
        break;
    }
    return o->ok;
}
/* ARM ALS HINDERNIS (Runde 7, Pruefbericht Runde 6 R1; PORT-WAHL nach RE2 FUN_80034D0C, s. re15_re2arm_body_push_player): die
 * Figur in ihrer eigenen Pose (Profil PROF_EIGEN, Blick pl->rot_y) wird so wenig wie moeglich aus beiden gezeichneten Armen
 * geschoben, damit kein Armpunkt in ihr liegt. Rueckgabe 1 = geschoben. */
static int arm_schiebt(const re15_actor_t *e, re15_actor_t *pl)
{
    if (!profil_bauen()) return 0;
    const re2arm_t *a = &s_arm[arm_slot(e)];
    static int32_t w[2 * RE2ARM_ARM_PUNKTE][3], d[2 * RE2ARM_ARM_PUNKTE][3];
    int32_t hc[3];
    int n = 0;
    for (int teil = 2; teil <= 3; teil++) n += arm_punkte(e, a, teil, w + n, hc);
    if (n <= 0) return 0;
    const int32_t cs = re15_cos_q12((int)pl->rot_y), sn = re15_sin_q12((int)pl->rot_y);
    int nah = 0;
    for (int v = 0; v < n; v++) {                        /* figur-lokal relativ zur jetzigen Lage */
        const int32_t dx = w[v][0] - pl->x, dz = w[v][2] - pl->z;
        d[v][0] = (int32_t)(((int64_t)cs * dx - (int64_t)sn * dz) >> 12);
        d[v][1] = w[v][1] - pl->y;
        d[v][2] = (int32_t)(((int64_t)sn * dx + (int64_t)cs * dz) >> 12);
        if ((int64_t)d[v][0] * d[v][0] + (int64_t)d[v][2] * d[v][2] <= (int64_t)s_pp_rmax[PROF_EIGEN] * s_pp_rmax[PROF_EIGEN]) nah = 1;
    }
    if (!nah || arm_draussen(PROF_EIGEN, n, (const int32_t (*)[3])d, 0, 0, NULL)) return 0;
    int32_t best = INT32_MAX, nx = pl->x, nz = pl->z;
    for (int t = 0; t < RE2ARM_HALTE_NT; t++) {          /* die Figur um r gegen Richtung th verschieben = die Armpunkte um +r */
        const int th = t * (4096 / RE2ARM_HALTE_NT);
        const int32_t ex = re15_cos_q12(th), ez = re15_sin_q12(th);
        const int32_t r = arm_frei_ab(PROF_EIGEN, n, (const int32_t (*)[3])d, ex, ez, NULL);
        if (r < 0 || r >= best) continue;
        const int32_t ox = (int32_t)(((int64_t)r * ex) >> 12), oz = (int32_t)(((int64_t)r * ez) >> 12);
        int32_t qx = pl->x - (int32_t)(((int64_t)cs * ox + (int64_t)sn * oz) >> 12);
        int32_t qz = pl->z - (int32_t)((-(int64_t)sn * ox + (int64_t)cs * oz) >> 12);
        const int32_t ux = qx, uz = qz;
        if (g_room_rdt_ok) re15_collision_constrain(&g_room_rdt, pl->x, pl->z, &qx, &qz);   /* FUN_8003b0a4 @0x80031d70 */
        if (qx != ux || qz != uz) {                      /* geklemmt: an der wirklichen Lage nachpruefen */
            const int32_t vx = pl->x - qx, vz = pl->z - qz;
            const int32_t px2 = (int32_t)(((int64_t)cs * vx - (int64_t)sn * vz) >> 12);
            const int32_t pz2 = (int32_t)(((int64_t)sn * vx + (int64_t)cs * vz) >> 12);
            if (!arm_draussen(PROF_EIGEN, n, (const int32_t (*)[3])d, px2, pz2, NULL)) continue;
        }
        best = r; nx = qx; nz = qz;
    }
    if (best == INT32_MAX) return 0;
    if (getenv("RE15_RE2_TRACE")) {
        FILE *o = re15_re2_trace_out() ? re15_re2_trace_out() : stderr;
        fprintf(o, "[re15halt] F%u slot %d Hindernis: Figur (%d,%d) -> (%d,%d), Schub %d\n", (unsigned)g_engine.frame_count,
                arm_slot(e), pl->x, pl->z, nx, nz, best);
    }
    pl->x = nx; pl->z = nz;
    return 1;
}
/* kontakt / innen im jetzigen Bild - NUR Mess-Auskunft (re15_re2arm_kontakt_rest, Riegel/Messschiene), nie im Spielablauf. */
static int halte_jetzt(const re15_actor_t *e, const re2arm_t *a, const re15_actor_t *pl, double *kontakt, int *innen)
{
    double hv[RE2ARM_HAND_N][3]; int32_t h[3], s[3];
    if (!hand_punkte(e, a, hv, h, s)) return 0;
    const int n = figur_dreiecke(pl, pl->rot_y, (int)pl->anim_frame);
    if (n <= 0) return 0;
    halte_mass(n, hv, pl->x, pl->z, kontakt, innen);
    return 1;
}

/* Mess-Auskunft: kontakt (Block HALTE-LAGE, MASSE) des haltenden Arms in diesem Bild, auf Abruf gerechnet; -1 = kein Halten. */
int re15_re2arm_kontakt_rest(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || s_holder != slot) return -1;
    const re15_actor_t *e = &g_actors[slot];
    double kk = 0.0; int ii = 0;
    if (!re15_re2arm_modell_re15(e) || !halte_jetzt(e, &s_arm[slot], &g_actors[RE15_ACTOR_SLOT_PLAYER], &kk, &ii)) return -1;
    return (int)lround(kk);
}

/* Welt-Lage eines EM1A-Teils (s. KOERPER). `poses`/`n` werden nicht mehr gebraucht (der Koerper fuehrt seine Pose selbst).
 * Rueckgabe 1 = rot/trans gesetzt; 0 = nicht dieser Weg; -1 = Teil nicht zeichnen (Koerper noch nicht da). `streck` (darf
 * NULL sein) = Skalierung der lokalen y-Achse Q12 — nur die des Originals (B[3]), sonst 4096. */
int re15_re2arm_zeichen_teil(const re15_actor_t *e, int teil, const re15_skel_pose_t *poses, int n,
                             int32_t rot[9], int32_t trans[3], int32_t *streck)
{
    (void)poses; (void)n;
    if (streck) *streck = 4096;
    if (!e || !re15_re2arm_owns(e) || !re15_re2arm_modell_re15(e)) return 0;
    const re2arm_t *a = &s_arm[arm_slot(e)];
    if (!a->koerper_an) return -1;
    const int sk = koerper_teil_welt(e, a, teil, rot, trans);
    if (sk < 0) return -1;
    if (streck) *streck = sk;
    return 1;
}

/* FUN_800152C8(self, yaw_ofs): x += cos(yaw+ofs)*+0x144 >> 12, z -= sin(...)*+0x144 >> 12
 * (EXE @0x800152E4-334, s. enemy_ai_re2_dog.c re2d_move). +0x146/+0x148 sind beim Arm immer 0. */
static void arm_move(re15_actor_t *e, int spd, int yaw_ofs)
{
    int yaw = ((int)e->rot_y + yaw_ofs) & 0xfff;
    e->speed_h = (int16_t)spd;                                   /* +0x144 */
    e->x += (int32_t)(((int32_t)re15_cos_q12(yaw) * spd) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(yaw) * spd) >> 12);
}
/* FUN_800157D4(&PL, part+0x5C, r): Spieler-Abstand zum Part-Weltpunkt STRIKT < r
 * (`sltu v0,v0,s0` — Skeptiker-Tabelle 7). */
static int arm_hand_within(re15_actor_t *e, int slot, const re15_actor_t *pl, int r, int32_t out_hand[3])
{
    int32_t h[3];
    arm_hand_pose(e, slot, h);                           /* part[Hand]+0x5C der Parts-Pose (s. re2arm_t) */
    if (out_hand) { out_hand[0] = h[0]; out_hand[1] = h[1]; out_hand[2] = h[2]; }
    int32_t dx = pl->x - h[0], dz = pl->z - h[2];
    uint32_t d = re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz));
    return d < (uint32_t)r;
}
/* 0x80101164(self, var, a2): Parts des aktiven Arms Bit 0 setzen/loeschen + Entity-Bit 2. Der Port
 * fuehrt beides als e->no_draw (Bit 2) — die Part-Bits des aktiven Arms sind immer gleich
 * dem Entity-Bit (beide Aufrufer schalten sie gemeinsam). hit_radius = 0 solange verborgen
 * (Beleg im Kopf). */
static void arm_show(re15_actor_t *e, int slot, int show)
{
    s_arm[slot].hidden = show ? 0 : 1;
    if (show) s_arm[slot].gezeichnet = 1;                 /* NB4: part+0x5C ab jetzt die Arm-Lage */
    e->no_draw = show ? 0 : 1;
    if (show) { e->hit_radius_min = 300; e->hit_radius_max = 300; e->hit_height = 1440; } /* RE1.5-Kasten 0x1A
                                                                                         * (re15_enemy_apply_hitbox) */
    else      { e->hit_radius_min = 0;   e->hit_radius_max = 0; }
}

/* ---- INIT @0x801000F8 ------------------------------------------------------------------------ */
static void arm_init(re15_actor_t *e, int slot)
{
    re2arm_t *a = &s_arm[slot];
    re15_ai_set_state_word(e, 0x00000001u);               /* sw 1,4(s0) @0x80100118: Routine 1, Sub 0, Phase 0 */
    /* Hook 0x800CE3B4 = 0x8010121C @0x8010011C-144 (Port: arm_hook unten) */
    e->re2z_self1d3 = 0;                                  /* sb zero,467 @0x80100130 */
    e->speed_h = 0;                                       /* +0x144/+0x148/+0x146 = 0 @0x80100134-3C */
    e->re2z_t158 = (int16_t)(re15_re2_rand() & 0xFu);     /* @0x80100148-64 */
    e->hp = 250;                                          /* +0x156 = 250 @0x80100168-6C */
    a->latch224 = 0;                                      /* +0x224 = 0 @0x80100174 */
    /* +0x151/152/153 = 0x82 (@0x80100170-80): Trefferzonen-Pools, im Port ohne Leser. */
    /* Arm-Wahl + Master (Port-Bruecke, s. Kopf): RE2-ROOM2050-Folge pc[4] = 10,01,00,01,00. */
    {   int k = s_spawn_count++ % 5;
        e->re2z_f10e = (uint16_t)(((k & 1) ? 1u : 0u) | ((k == 0) ? 0x10u : 0u));
    }
    a->rec_x = e->x; a->rec_y = e->y; a->rec_z = e->z;    /* Runde 3: Record-Lage fuer den RE1.5-Koerper (s. MODELL) */
    a->rec_yaw = e->rot_y; a->koerper_an = 0;
    a->k1d0 = (uint16_t)((rng15() & 0x1f) + 30);         /* Runde 4: +0x1D0 im Typ-INIT @0x8010c3e8-3fc (s. KOERPER) */
    if (re15_re2arm_modell_re15(e)) (void)profil_bauen();   /* Runde 6/7: Figur-Profil beim Raumladen, nicht beim Zugriff (HALTE-LAGE) */
    arm_anker_1210(e);                                    /* Port-Bruecke: Ursprung an die Wand (s. Kopf) */
    a->home_yaw = e->rot_y; a->home_x = e->x; a->home_z = e->z;   /* +0x218/+0x21C/+0x220 @0x80100188-194 */
    a->bone_base = (uint8_t)((e->re2z_f10e & 1u) ? 8 : 1);        /* +0x1C1 = 0x80101414[var*7] @0x801001D0/@0x80100288 */
    if (re15_re2arm_modell_re15(e) && !logik_em2d(e))             /* ohne RE2-Archiv: EM1A-Arm 2/3 (Blut am Schultergelenk) */
        a->bone_base = (uint8_t)em1a_arm(e);
    /* Part-Werte 96/512/312 (@0x801001D8-F8) = Part-Physikfelder, im Port ohne Leser.
     * 0x80101164(self, var, 0) @0x80100304: aktiver Arm ausgeblendet, Entity-Bit 2. */
    arm_show(e, slot, 0);
    s_cd_cfbf4 = 0;                                       /* sh zero,-1036(at) @0x80100324 = 0x800CFBF4 */
    /* +0x1E8 = 1 (Box-Zahl), +0x9E = 500, +0x9A/+0x9C/+0x90/+0x92/+0x1EE = 800 @0x8010032C-34C:
     * RE2-Trefferkasten — nicht uebernommen (Kopf). */
    s_se_cd = 0;                                          /* sb zero,5412(at) @0x80100358 = 0x80101524 */
    arm_clip(e, 0, 0);                                    /* sw zero,332(s0) @0x80100368 (Clip 0, Rate 0) */
    (void)arm_adv(e, slot, 256);                          /* jal 0x8002959C @0x80100370 */
    a->init_done = 1;
}

/* ---- A0 @0x8010050C / B0 @0x80100674 (RUHE) ------------------------------------------------- */
static void arm_A0(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    re2arm_t *a = &s_arm[slot];
    int aa = re15_ai_arc_test(e, pl->x, pl->z, 1024);     /* FUN_80015614(…,1024) @0x8010053C-40 */
    int bb = re15_ai_arc_test(e, pl->x, pl->z, 760);      /* FUN_80015614(…,760)  @0x80100548-58 */
    uint32_t d = e->ai_dist;                              /* lw s2,496(s1) @0x80100538 */
    int fired = 0;
    if (d < 0x9C4u && aa == 0 && e->sub_state_2 >= 2) {   /* sltiu 0x9c4 @0x80100564, a==0 @0x80100570,
                                                           * Phase < 2 -> Zweig 2 @0x80100578-84 */
        re15_ai_set_state_word(e, 0x101u);                /* sw 257,4(s1) @0x80100598-9C */
        arm_show(e, slot, 1);                             /* 0x80101164(self,var,1) @0x801005A0 */
        e->z = a->home_z; e->x = a->home_x;               /* +0x40/+0x38 := +0x220/+0x21C @0x801005B0/BC (nicht +0x76) */
        e->re2z_self1d3 = 0;                              /* sb zero,467 @0x801005B8 */
        fired = 1;
    } else if (d < 0xBB8u && bb == 0 && e->sub_state_2 == 2) {  /* sltiu 0xbb8 @0x801005D4, b==0 @0x801005E0,
                                                                  * Phase == 2 @0x801005E4-F0 */
        e->sub_state_2 = 3;                               /* sb 3,6(s1) @0x80100604 */
        arm_show(e, slot, 1);                             /* @0x80100608 */
        e->re2z_self1d3 = 0;                              /* @0x8010061C */
        fired = 1;
    }
    if (!fired) return;
    if (a->latch224 == 1) {                               /* lh 548 == 1 @0x801005B4-C4 / @0x80100610-18 */
        if (s_se_cd == 0) { arm_se(4); s_se_cd = 60; }    /* @0x80100620-44 */
    } else {
        a->latch224 = 1;                                  /* sh 1,548 @0x80100650-54 */
    }
}
static void arm_B0(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    re2arm_t *a = &s_arm[slot];
    (void)pl;
    switch (e->sub_state_2) {
    case 0:                                               /* @0x801006C8 */
        arm_clip(e, 0, 15);                               /* sw 0xF0000,332 @0x801006CC-D4 */
        (void)arm_adv(e, slot, 256);                      /* jal 0x8002959C @0x801006D0 */
        e->sub_state_2 = 1;                               /* sb 1,6 @0x801006F4 */
        e->rot_y = a->home_yaw; e->x = a->home_x; e->z = a->home_z;   /* @0x8010070C-714 */
        e->re2z_self1d3 |= 0x80u;                         /* ori 0x80 @0x80100708 / sb @0x8010071C */
        arm_move(e, 400, 2048);                           /* +0x144 = 400 @0x801006F8-FC, FUN_800152C8(self, 2048) @0x80100718 */
        break;
    case 1: {                                             /* @0x80100720 */
        int16_t v = e->re2z_t158;
        e->re2z_t158 = (int16_t)(v - 1);                  /* sh v0,344 im Delay-Slot @0x80100730: IMMER */
        if (v == 0) e->sub_state_2 = 2;                   /* bne v1,zero @0x8010072C; sb 2,6 @0x8010073C */
        break;
    }
    case 2:                                               /* @0x801006B0 -> Ausgang: nichts */
        break;
    default: {                                            /* Phase 3 @0x80100740 (von A0 Zweig 2 gesetzt) */
        int done = arm_adv(e, slot, 256);                 /* jal 0x8002959C @0x80100740 */
        if (done) {
            re15_ai_set_state_word(e, 0x101u);            /* sw 257,4 @0x80100758 */
            e->x = a->home_x; e->z = a->home_z;           /* @0x8010075C/64 */
        } else if (arm_frame(e) < 11) {                   /* sltiu v0,v0,0xb @0x80100770 */
            arm_move(e, 50, 0);                           /* +0x144 = 50 @0x80100780-84, FUN_800152C8(self, 0) @0x8010078C */
        }
        break;
    }
    }
}

/* ---- A1 @0x801007A8 / B1 @0x80100830 (REACH 0x101) ------------------------------------------- */
static void arm_A1(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    if (arm_hand_within(e, slot, pl, 900, NULL))                /* FUN_800157D4(&PL, part[Hand]+0x5C, 900) @0x801007C4/@0x80100804 */
        re15_ai_set_state_word(e, 0x301u);                /* sw 769,4 @0x80100814-18 */
}
static void arm_B1(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    re2arm_t *a = &s_arm[slot];
    if (e->sub_state_2 == 0) {                            /* P0 @0x80100870 (faellt in P1 durch @0x801008D4) */
        uint32_t r = re15_re2_rand();                     /* jal rng @0x80100870 */
        arm_clip(e, (int)(r & 1u), 15);                   /* 0xF0000 | (rng&1) @0x80100878-88 */
        e->sub_state_2 = 1;                               /* sb 1,6 @0x8010088C/94 */
        e->re2z_t158 = (int16_t)((re15_re2_rand() & 0x1Fu) + 60u);   /* @0x80100890-A4 */
        if (re15_re2_rand() & 1u) {                       /* @0x801008A0-AC */
            arm_se((re15_re2_rand() & 1u) ? 0 : 1);       /* @0x801008B4-D0: rng&1 -> Id 0, sonst Id 1 */
        }
    }
    if (e->sub_state_2 == 1) {                            /* P1 @0x801008D8 */
        int d = re15_ai_arc_test(e, pl->x, pl->z, 32);    /* FUN_80015614(…,32) @0x801008E8-EC */
        int16_t yaw = (int16_t)(e->rot_y + d);            /* addu / sh 118 @0x801008F8-FC */
        int16_t hi  = (int16_t)(a->home_yaw + 384), lo = (int16_t)(a->home_yaw - 384);
        if (hi < yaw) yaw = hi;                           /* slt @0x80100914-20 */
        if (yaw < lo) yaw = lo;                           /* slt @0x80100934-40 */
        e->rot_y = yaw;
        (void)arm_adv(e, slot, 256);                      /* jal 0x8002959C @0x80100950 */
        int16_t v = e->re2z_t158;
        e->re2z_t158 = (int16_t)(v - 1);                  /* sh im Delay-Slot @0x80100968: IMMER */
        if (v == 0) re15_ai_set_state_word(e, 0x501u);    /* @0x8010096C-70 */
    }
}

/* ---- A3 @0x801009A0 / B3 @0x80100A90 (ZUGRIFF 0x301) ----------------------------------------- */
static int kontakt_moeglich(re15_actor_t *e, int slot, re15_actor_t *pl);   /* s. Hook unten */
static void arm_A3(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    /* Runde 4..7 (Vorgabe Punkt 2): mit EM1A greift die RE1.5-Hand nur, wenn sie die Figur wirklich erreicht - RE2-Tor 600
     * (@0x801009BC; bis Runde 6 der Klemmradius 450 @0x80073e9a, den die Hand an der geschobenen Figur kaum unterschreitet) UND es
     * gibt eine Halte-Lage mit der Hand an der Figur und Sprung <= Handbreite (kontakt_moeglich, Block HALTE-LAGE). */
    const int em1a = re15_re2arm_modell_re15(e) && s_arm[slot].koerper_an;
    if (!arm_hand_within(e, slot, pl, 600, NULL)) return;   /* FUN_800157D4(..,600) @0x801009BC */
    if (arm_frame(e) < 5) return;                         /* sltiu 5 @0x80100A18 */
    if (s_cd_cfbf4 != 0) return;                          /* lhu 0x800CFBF4 @0x80100A28-30 */
    if (em1a && !(pl->re2z_self1d3 & 0x80u) && !re15_player_is_grabbed() && !kontakt_moeglich(e, slot, pl)) return;
    if ((pl->re2z_self1d3 & 0x80u) || re15_player_is_grabbed()) {   /* PL+0x1D3 & 0x80 @0x80100A3C-48
                                                                     * (+ Port-Pin anderer Greifer) */
        re15_ai_set_state_word(e, 0x601u);                /* addiu 1537 @0x80100A4C / sw @0x80100A78 */
        return;
    }
    re15_ai_set_state_word(e, 0x401u);                    /* sw 1025,4 @0x80100A50-54 */
    pl->re2z_self1d3 |= 0x80u;                            /* @0x80100A58-6C */
}
static void arm_B3(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    if (e->sub_state_2 == 0) {                            /* P0 @0x80100AEC (faellt in P1 @0x80100AFC) */
        arm_clip(e, 3, 15);                               /* 0xF0003 @0x80100ACC/AEC-F0 */
        e->sub_state_2 = 1;                               /* @0x80100AF4-F8 */
    }
    if (e->sub_state_2 == 1) {                            /* P1 @0x80100AFC */
        re15_enemy_steer_point(e, pl->x, pl->z, 48);      /* FUN_80015558(self, PL.x, PL.z, 48) @0x80100B10-14 */
        int done = arm_adv(e, slot, 256);                 /* @0x80100B24 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + done);/* @0x80100B34-3C */
    } else if (e->sub_state_2 == 2) {
        re15_ai_set_state_word(e, 0x501u);                /* @0x80100AE0 / @0x80100B40 */
    }
}

/* ---- B4 @0x80100B68 (HALTEN 0x401; A4 = jr ra) ---------------------------------------------- */
static void arm_B4(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    switch (e->sub_state_2) {
    case 0: {                                             /* P0 @0x80100BC4 (kein Durchfall: j 0x80100D6C) */
        int32_t h[3];
        arm_clip(e, 5, 15);                               /* 0xF0005 @0x80100BC8-CC */
        e->sub_state_2 = 1;                               /* @0x80100BD0-D4 */
        /* Runde 35 Spur H: part[Hand] @0x80100BEC-C14 traegt die Pose des LETZTEN 0x8002959C
         * (B3 P1 @0x80100B24 im Vortakt: Clip 3 Bild k) — P0 ruft selbst keinen Advance, er endet
         * `j 0x80100D6C` (@0x80100CAC). Vorher rechnete der Port die Hand aus dem eben gesetzten
         * Clip 5 Bild 0: Leon stand bis ~170 Einheiten seitlich versetzt (Messung Dossier 4.1). */
        arm_hand_pose(e, slot, h);
        if (getenv("RE15_RE2_TRACE")) {                   /* Messschiene: alter gegen neuer Pin */
            int32_t alt[3], rein[3];
            /* Runde 3 (Pruefbericht Runde 2 G5): "Clip 5 Bild 0" auf der Bank, auf der der Hand-Knochen liegt — mit
             * EM2D-Logik-Bank ist das EM2D (Knochen 3/10), nicht die gezeichnete EM1A-Bank (4 Knochen). */
            if (!logik_em2d(e) || !logik_knochen_rein(e, 5, 0, re15_re2arm_hand_bone(e), alt))
                re15_enemy_bone_world_pos(e, re15_re2arm_hand_bone(e), alt);
            (void)re15_re2arm_hand_parts(slot, NULL, rein);
            FILE *o = re15_re2_trace_out() ? re15_re2_trace_out() : stderr;
            fprintf(o, "[re2arm] PIN slot %d yaw %d: Parts gemischt Clip %d Bild %u -> (%d,%d); rein (ohne +0x14E) "
                       "-> (%d,%d); Clip 5 Bild 0 -> (%d,%d); Leon vorher (%d,%d) yaw %d\n", slot, (int)e->rot_y,
                    (int)s_arm[slot].pose_clip, (unsigned)s_arm[slot].pose_frame, h[0], h[2], rein[0], rein[2],
                    alt[0], alt[2], pl->x, pl->z, (int)pl->rot_y);
        }
        s_pl_vor[0] = pl->x; s_pl_vor[1] = pl->z;         /* Runde 4: Lage vor dem Pin (Flur-Klemme der Halte-Lage) */
        pl->x = h[0];                                     /* sw v0,0x800CFC30 (PL+0x38) @0x80100C18-20 */
        pl->z = h[2];                                     /* sw v0,0x800CFC38 (PL+0x40) @0x80100C24-38 */
        s_holder = slot;                                  /* 0x800CFDAC = PL+0x1B4 = self @0x80100C2C-30 */
        s_pin_bild = 1;                                   /* Nachbesserung 3: Koerper-Push NACH dem Pin im
                                                           * selben Bild (FUN_800355C4 @0x80026628) */
        /* 0x800CFD80/84 = PL+0x188/+0x18C := self+0x188/+0x18C @0x80100C3C-5C (Opfer-Bank Paar 3);
         * PL+0x4 = 5 @0x80100C4C-54 -> Spieler-Routine 5 (Port: Victim-Modus 4 = extern gefuehrt) */
        re15_player_victim_force(0x1Au, 0, 0);
        s_hook_phase = 0;
        /* Rumble FUN_8003947C(4,0)/80039514(4,250,0)/800395B8(100,50,150,0) @0x80100C60-88: kein Port-Rumble */
        e->re2z_t15a = 150;                               /* +0x15A = 150 @0x80100C8C-90 */
        /* lh PL+0x156 / slti 101 @0x80100C94-A4: beide Zweige -> Ausgang (toter Vergleich) */
        break;
    }
    case 1: {                                             /* P1 @0x80100CB4 */
        if (arm_frame(e) == 0) {                          /* lbu 333 / bne @0x80100CB4-BC */
            /* Rumble @0x80100CC4-E8: kein Port-Rumble */
            arm_se((re15_re2_rand() & 1u) ? 5 : 7);       /* @0x80100CEC-D08 */
        }
        (void)arm_adv(e, slot, 256);                      /* @0x80100D18 */
        int mash = re15_re2z_mash();                      /* FUN_8001598C @0x80100D20 */
        e->re2z_t15a = (int16_t)(e->re2z_t15a - 1 - 2 * mash);   /* @0x80100D28-38 */
        if (e->re2z_t15a < 0) {                           /* sll/bgez @0x80100D3C-40 */
            e->sub_state_2 = 2;                           /* @0x80100D44-48 */
            s_hook_phase = 2;                             /* sb 2,0x800CFBFE (PL+0x6) @0x80100D4C-50 */
        }
        break;
    }
    default:                                              /* P2 @0x80100D5C */
        s_cd_cfbf4 = 120;                                 /* sh 120,0x800CFBF4 @0x80100BB8/@0x80100D5C-60 */
        re15_ai_set_state_word(e, 0x501u);                /* @0x80100D64-68 */
        break;
    }
}

/* ---- B5 @0x80100D90 (RUECKZUG 0x501; A5 = jr ra) -------------------------------------------- */
static void arm_B5(re15_actor_t *e, int slot)
{
    if (e->sub_state_2 == 0) {                            /* P0 @0x80100DD0 (faellt in P1 @0x80100DFC) */
        arm_clip(e, 4, 15);                               /* 0xF0004 @0x80100DD0-E0 */
        e->sub_state_2 = 1;                               /* @0x80100DE8-EC */
        e->re2z_self1d3 |= 0x80u;                         /* @0x80100DE4/F0/F8 */
        arm_se(6);                                        /* addiu a0,6 @0x80100DD8 / jal @0x80100DF4 */
    }
    if (e->sub_state_2 == 1) {                            /* P1 @0x80100E00 */
        int done = arm_adv(e, slot, 256);                 /* @0x80100E08 */
        if (done) {
            e->re2z_t158 = (int16_t)((re15_re2_rand() & 0x1Fu) + 30u);   /* @0x80100E18-34 */
            arm_show(e, slot, 0);                         /* 0x80101164(self,var,0) @0x80100E38 */
            re15_ai_set_state_word(e, 0x701u);            /* sw 1793,4 @0x80100E40-44 */
            e->hp = -1;                                   /* sh -1,342 @0x80100E4C-50 */
            e->re2z_self1d3 |= 0x80u;                     /* @0x80100E48-58 */
        }
        arm_move(e, 30, 2048);                            /* beide Ausgaenge: +0x144 = 30 @0x80100E64-68,
                                                           * FUN_800152C8(self, 2048) @0x80100E70 */
    }
}

/* ---- B6 @0x80100E9C (WARTEN 0x601; A6 = jr ra) ---------------------------------------------- */
static void arm_B6(re15_actor_t *e, re15_actor_t *pl, int slot)
{
    switch (e->sub_state_2) {
    case 0: arm_clip(e, 5, 15); e->sub_state_2 = 1; break;      /* 0xF0005 @0x80100EE8-EC, sb 1,6 @0x80100EF0-F8 */
    case 1:
        (void)arm_adv(e, slot, 256);                             /* @0x80100F00 */
        if (!(pl->re2z_self1d3 & 0x80u) && !re15_player_is_grabbed())   /* PL+0x1D3 & 0x80 @0x80100F08-18 */
            e->sub_state_2 = 2;                                  /* @0x80100F1C-24 */
        break;
    default: re15_ai_set_state_word(e, 0x101u); break;          /* @0x80100EDC / @0x80100F28 */
    }
}

/* ---- HURT @0x80100F50 / DEATH @0x80100FE0 ----------------------------------------------------- */
/* Beide Tabellen @0x801014CC / @0x801014F0 fuehren fuer jede erreichbare Zeile denselben Zweig
 * (0x80100F8C bzw. 0x8010101C; die Nullzeilen 2/5/8 waeren jalr 0 = Absturz, also nie gestempelt). */
static void arm_hurt(re15_actor_t *e)
{
    s_arm[arm_slot(e)].treffer = 1;                       /* Runde 4: der RE1.5-Koerper sinkt zurueck (Block KOERPER, GRIFF) */
    re15_ai_set_state_word(e, 0x501u);                    /* sw 1281,4 @0x80100FA0-A4 */
    e->re2z_self1d3 |= 0x80u;                             /* @0x80100FA8-B0 */
    arm_se((re15_re2_rand() & 1u) ? 2 : 3);               /* @0x80100FAC-C8: rng&1 -> 2, sonst 3 */
}
static void arm_death(re15_actor_t *e, int slot)
{
    s_arm[slot].treffer = 1;                              /* Runde 4: s. arm_hurt */
    /* FUN_8001BF10(0x21000, yaw, part[0x80101414[var*7]]+0x48, {0,0,0}) @0x80101028-8C: RE2-FX
     * (nicht portiert) -> RE1.5-Raumbank-Blut am Bone 1/8 (Stand-in wie enemy_ai_re2_dog.c). */
    {   int32_t p[3];
        if (!logik_em2d(e) || !logik_knochen_rein(e, (int)s_arm[slot].logik_clip, (int)s_arm[slot].logik_frame,
                                                  (int)s_arm[slot].bone_base, p))     /* Runde 2: EM2D-Knochen 1/8 */
            re15_enemy_bone_world_pos(e, (int)s_arm[slot].bone_base, p);
        const re15_esp_t *bank = re15_esp_room_bank();
        if (bank) re15_esp_fx_spawn_ex(bank, 0, 0, 0x2000, p[0], p[1], p[2], (int16_t)e->rot_y);
    }
    e->hp = 250;                                          /* sh 250,342 @0x80101090-94 */
    re15_ai_set_state_word(e, 0x501u);                    /* @0x8010109C-A0 */
    e->re2z_self1d3 |= 0x80u;                             /* @0x80101098-AC */
    arm_se((re15_re2_rand() & 1u) ? 2 : 3);               /* @0x801010B0-C4 */
}

/* Runde 4..7: darf die RE1.5-Hand jetzt greifen? Vorhersage von Pin und Hook P0: der Pin stellt die Figur auf die Hand
 * (@0x80100C18-38), dann Blick zum Greifer FUN_80015558(..,2048) @0x801012F8, +2048 wenn FUN_80015910 @0x801012E0. Mit diesem
 * Blick die Halte-Lage mit dem kleinsten Sprung (halte_loesen, Block HALTE-LAGE). Runde 7 (Pruefbericht Runde 6 R1): gegriffen
 * wird, sobald dieser Sprung hoechstens eine Handbreite ist - also sobald die gezeichnete Hand die Figur erreicht, nicht erst,
 * wenn sie schon durch sie hindurch ist. Das Ergebnis merkt sich der Arm fuer den Pin im selben Bild (q_*). */
static int kontakt_moeglich(re15_actor_t *e, int slot, re15_actor_t *pl)
{
    int32_t h[3];
    re2arm_t *a = &s_arm[slot];
    a->q_ok = 0;
    if (!koerper_hand(e, a, h, NULL)) return 0;
    re15_actor_t k = *pl;
    k.x = h[0]; k.z = h[2];                               /* Pin @0x80100C18-38 vor dem Hook P0 */
    const int same = ((((int)e->rot_y - (int)k.rot_y) + 0x400) & 0xfff) < 0x800;
    re15_enemy_steer_point(&k, e->x, e->z, 2048);
    if (same) k.rot_y = (int16_t)(((int)k.rot_y + 2048) & 0xfff);
    halte_t q;
    /* luft im Mittel hoechstens eine halbe Handbreite; im LETZTEN Bild, in dem das Tor A3 noch greifen kann (B3 zaehlt danach
     * `done` @0x80100B24-3C, dann 0x501), hoechstens eine ganze (PORT-WAHL: lieber eine etwas lockere Hand als kein Griff) */
    const int letzte = (arm_frame(e) + 1 >= logik_len(e, (int)a->logik_clip));
    const int32_t hb = hand_halbbreite(e);
    const int ok = halte_loesen(e, a, k.rot_y, pl->y, pl->x, pl->z, 2 * hb, letzte ? hb : hb / 2, &q);
    if (getenv("RE15_RE2_TRACE")) {
        FILE *o = re15_re2_trace_out() ? re15_re2_trace_out() : stderr;
        fprintf(o, "[re15halt] F%u slot %d Pruefung: ok %d Sprung %d (hoechstens %d, Kandidaten %d) Luft max %d Mittel %d Richtung %d "
                   "Lage (%d,%d) Figur (%d,%d) Blick %d\n", (unsigned)g_engine.frame_count, slot, ok, ok ? (int)q.sprung : -1, 2 * hand_halbbreite(e),
                q.kandidaten, (int)q.luft_max, (int)q.luft_mittel, q.richtung, q.x, q.z, pl->x, pl->z, (int)k.rot_y);
    }
    if (!ok) return 0;
    a->q_ok = 1; a->q_x = q.x; a->q_z = q.z; a->q_sprung = q.sprung;
    return 1;
}

/* ---- SPIELER-HOOK 0x8010121C -> 0x80101258 (Spieler-Routine 5, s. Kopf) ---------------------- */
static int victim_clip_len(int clip)
{
    re15_enemy_bank_t *b = re15_enemy_find(0x1Au);
    if (!b || !b->victim_ok || clip < 0 || clip >= b->anim_victim.clip_count) return 0;
    return b->anim_victim.clips[clip].frame_count;
}
static int hook_advance(re15_actor_t *pl)                /* 0x8002959C auf PL+0x188/+0x18C: +1, Wrap, done */
{
    int fc = victim_clip_len((int)pl->motion);
    if (pl->anim_frac > 0) pl->anim_frac--;               /* +0x14E-Decay @0x800299C0-CC */
    if (fc <= 0) return 1;
    uint32_t nf = pl->anim_frame + 1u;
    if ((int)nf >= fc) { pl->anim_frame = 0; return 1; }
    pl->anim_frame = (uint16_t)nf;
    return 0;
}
static void arm_hook(re15_actor_t *e, re15_actor_t *pl)
{
    switch (s_hook_phase) {
    case 0: {                                             /* @0x801012A8 */
        pl->motion = 0; pl->anim_frame = 0; pl->anim_frac = 15; pl->anim_blend_rate = 256;   /* 0xF0000 */
        s_hook_phase = 1;                                 /* sb 1,6(s1) @0x801012B0-B8 */
        /* FX 0x8005BA28(0x4000001 | ((rng&1)+1)<<16, PL+0x38) @0x801012B4-D8 = CORE-SE 1/2 */
        re15_audio_core_se((int)((re15_re2_rand() & 1u) + 1u));
        int same = ((((int)e->rot_y - (int)pl->rot_y) + 0x400) & 0xfff) < 0x800;   /* FUN_80015910 @0x801012E0 */
        re15_enemy_steer_point(pl, e->x, e->z, 2048);     /* FUN_80015558(PL, Greifer.x/z, 2048) @0x801012F8 */
        if (same) pl->rot_y = (int16_t)(((int)pl->rot_y + 2048) & 0xfff);   /* @0x80101304-18 */
        (void)hook_advance(pl);                           /* @0x80101328 */
        if (re15_re2arm_modell_re15(e))                   /* Runde 5..7: FESTE Halte-Lage an der RE1.5-Hand (Block HALTE-LAGE), */
        {   re2arm_t *a = &s_arm[arm_slot(e)];             /* im selben Bild vom Tor A3 bestimmt (kontakt_moeglich)             */
            if (a->q_ok) { pl->x = a->q_x; pl->z = a->q_z; }
            else { pl->x = s_pl_vor[0]; pl->z = s_pl_vor[1]; }   /* ohne Halte-Lage: die Figur bleibt, wo sie war */
            a->q_ok = 0;
            a->halte_x = pl->x; a->halte_z = pl->z; }
        break;
    }
    case 1:                                               /* @0x8010131C */
        (void)hook_advance(pl);
        if (re15_re2arm_modell_re15(e))                   /* Runde 5: die Figur steht fest (wie RE2) */
        {   re2arm_t *a = &s_arm[arm_slot(e)];
            pl->x = a->halte_x; pl->z = a->halte_z; }
        break;
    case 2: {                                             /* @0x80101338 */
        pl->motion = 1; pl->anim_frame = 0; pl->anim_frac = 7; pl->anim_blend_rate = 256;    /* 0x70001 */
        s_hook_phase = 3;                                 /* @0x80101344-48 */
        s_hook_phase = (uint8_t)(s_hook_phase + hook_advance(pl));   /* @0x80101358-70 */
        if (re15_re2arm_modell_re15(e)) {                 /* Runde 5: Leon reisst sich los (Clip 1 ab hier) - der RE1.5-Koerper sinkt
                                                           * im SELBEN Bild mit Clip 3 (wie B[4] @0x8010ceac-b0), sonst laege die
                                                           * stehende Hand ein Bild lang in Leons Losreiss-Pose (PORT-WAHL) */
            re2arm_t *a = &s_arm[arm_slot(e)];
            if (a->kmode == K_GRIFF) koerper_sinken(a);
        }
        break;
    }
    case 3: s_hook_phase = (uint8_t)(s_hook_phase + hook_advance(pl)); break;   /* @0x8010134C */
    default:                                              /* P4 @0x80101374 */
        re15_player_victim_force_end();                   /* PL+0x4 = 1 @0x80101374-80 */
        pl->re2z_self1d3 &= 0x7Fu;                        /* PL+0x1D3 &= 0x7F @0x80101384-90 */
        pl->motion = 0; pl->anim_frame = 0;               /* Port: Spielerclip zurueck auf den Stand (kein
                                                           * Original-Store, der Spieler-Root uebernimmt) */
        s_holder = -1; s_hook_phase = 0;
        break;
    }
}

/* ---- ROOT @0x80100018 + Routinen-Tabelle @0x80101424 ----------------------------------------- */
int re15_re2arm_tick(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!re15_re2arm_owns(e)) return 0;

    if (s_room_of_state != g_current_room_id) {           /* Raumwechsel (Port): Zaehler/Cooldowns neu */
        s_room_of_state = g_current_room_id;
        s_spawn_count = 0; s_se_cd = 0; s_holder = -1; s_hook_phase = 0;
        memset(s_arm, 0, sizeof s_arm);
        s_rng15 = 0x1c3;                                  /* Runde 4: Startwert je Raum @0x8003162c (s. rng15) */
    }
    if (e->state == 0) { arm_init(e, slot); return 1; }   /* Raumlade-Tick FUN_8004A694 (0x8000 ausgeblendet) */
    if (!s_arm[slot].init_done) { arm_init(e, slot); return 1; }

    /* SCHLAF: +0x10E & 0x8000 -> kein Tick (@0x80026590-9C). ROOM1210: +0x9 & 0x1F != 1 (sub02
     * Member_set(12,1) @0x1EDA -> `sb a2,9(a0)` @0x800411F8). */
    if ((e->grid_id & 0x1Fu) != 1u) return 1;
    koerper_tick(e, &s_arm[slot]);                        /* Runde 3: RE1.5-Koerper B[1] ab dem Ausloeser (s. MODELL) */
    /* Runde 7: der Spieler-Pass (re15_body_push_player) laeuft VOR diesem Tick; aendert der Koerper hier seine Pose (Wachsen,
     * Ende des Sinkens), schiebt der gezeichnete Arm die Figur im selben Bild (Block HALTE-LAGE, arm_schiebt) */
    if (re15_re2arm_modell_re15(e) && s_arm[slot].koerper_an && pl->active && pl->hp >= 0 && !re15_player_is_grabbed() &&
        !(pl->re2z_self1d3 & 0x80u))
        (void)arm_schiebt(e, pl);

    /* +0x1F0 = SquareRoot0(dx^2 + dz^2) @0x800265A4-E0 (32-bit-Deltas). */
    {   int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        e->ai_dist = re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz));
    }
    /* Root @0x80100018: FUN_8004A808 (Steuerpunkt, ohne Leser im Arm) / Pause 0x800CFBDC&0x20000000
     * (der Port-Aufrufer haelt die KI an) / Cooldowns nur beim Master +0x10E & 0x10: */
    if (e->re2z_f10e & 0x10u) {
        if (s_cd_cfbf4 != 0) s_cd_cfbf4--;                /* @0x80100054-80 */
        if (s_se_cd != 0)    s_se_cd--;                   /* @0x80100084-B0 */
    }
    switch (e->state) {                                   /* Tabelle @0x80101424 */
    case 1: {                                             /* ACTIVE @0x801003C4 -> Variante @0x80101444[+0x10E&1],
                                                           * beide = A-Tabelle @0x8010144C / B-Tabelle @0x8010146C
                                                           * (Dual-Dispatch, +0x5 wird fuer B NEU gelesen @0x80100448) */
        switch (e->sub_state_1) {
        case 0: arm_A0(e, pl, slot); break;               /* 0x8010050C */
        case 1: arm_A1(e, pl, slot); break;               /* 0x801007A8 */
        case 3: arm_A3(e, pl, slot); break;               /* 0x801009A0 */
        default: break;                                   /* 2/4/5/6/7 = jr ra */
        }
        switch (e->sub_state_1) {
        case 0: arm_B0(e, pl, slot); break;               /* 0x80100674 */
        case 1: arm_B1(e, pl, slot); break;               /* 0x80100830 */
        case 3: arm_B3(e, pl, slot); break;               /* 0x80100A90 */
        case 4: arm_B4(e, pl, slot); break;               /* 0x80100B68 */
        case 5: arm_B5(e, slot);     break;               /* 0x80100D90 */
        case 6: arm_B6(e, pl, slot); break;               /* 0x80100E9C */
        default: break;                                   /* 2/7 = jr ra */
        }
        break;
    }
    case 2: arm_hurt(e);        break;                    /* 0x80100F50 */
    case 3: arm_death(e, slot); break;                    /* 0x80100FE0 */
    default: break;                                       /* 4 = SKRIPT-Hook (nur ueber den Scheduler
                                                           * erreichbar, s. Kopf), 5/6 = 0, 7 = jr ra */
    }
    /* Root-Tail FUN_80035530 (Segmente + Push des ARMS aus anderen Entities): den Arm schiebt niemand — sein
     * word0 traegt Bit 0x4 (INIT `ori v0,v0,0x1404` @0x80100394, im RAM 0x0c003405), und FUN_80034D0C steigt bei wGeschobener & 4
     * aus (Decompile Z.1); der Spieler-Push durch den Arm (FUN_800355C4 ->
     * FUN_80034D0C(Arm, Spieler) @0x80035630) ist re15_re2arm_body_push_player, gerufen aus
     * re15_body_push_player. Matrizen ueber den Renderer. */

    /* Spieler-Routine 5 laeuft im Original NACH allen Entities (@0x80026620 FUN_8003BFAC). */
    if (s_holder == slot) {
        if (re15_player_victim_state() == 4) arm_hook(e, pl);
        else { s_holder = -1; s_hook_phase = 0; }         /* Opfer-Maschine von aussen beendet */
    }
    if (getenv("RE15_RE2_TRACE")) {
        static uint32_t s_last[RE15_ACTOR_MAX];
        uint32_t sig = ((uint32_t)e->state << 16) | ((uint32_t)e->sub_state_1 << 8) | e->sub_state_2;
        if (sig != s_last[slot]) {
            FILE *o = re15_re2_trace_out() ? re15_re2_trace_out() : stderr;
            fprintf(o, "[re2arm] slot %d state=%d sub=%d/%d clip=%d fr=%d hp=%d dist=%u pos=(%d,%d,%d) hidden=%d cd=%u\n",
                    slot, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion, (int)e->anim_frame,
                    (int)e->hp, (unsigned)e->ai_dist, (int)e->x, (int)e->y, (int)e->z,
                    (int)s_arm[slot].hidden, (unsigned)s_cd_cfbf4);
            s_last[slot] = sig;
        }
    }
    return 1;
}
