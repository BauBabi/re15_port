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
 *   - Der Trefferkasten des Ports bleibt der RE1.5-Kasten (300/1440, re15_enemy_apply_hitbox
 *     0x1A); RE2s +0x9A/+0x9C/+0x90/+0x92 = 800, +0x9E = 500 (@0x8010032C-348) sind NICHT
 *     uebernommen, weil die Push-Semantik von FUN_80034D0C (Root-Tail FUN_80035530 -> 0x80035408
 *     + 0x80034D0C) nicht zu Ende RE'd ist. Was belegt ist: FUN_80034D0C steigt aus, wenn eine
 *     der beiden Entities Bit 2 traegt (`(*a | *b) & 2`, Decompile Z.1) -> ein verborgener Arm
 *     schiebt und wird nicht geschoben; der Port setzt dafuer hit_radius_min = 0 solange
 *     verborgen (das nimmt ihn zugleich aus dem Trefferfilter — RE2: Schlaf 0x8000 = kein
 *     Ziel in FUN_800470C0 & Co, nach 0x701 hp = -1 = Gate @0x80047148).
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_ai_flavor.h"      /* re15_ai_re2_for_type / re15_re2_rand / re15_re2z_* Shims  */
#include "re15_enemy_ai.h"       /* re15_ai_set_state_word / re15_player_victim_force / _end   */
#include "re15_enemy.h"          /* re15_enemy_find (Opfer-Bank-Cliplaenge)                     */
#include "re15_damage.h"         /* re15_ai_arc_test (== FUN_80015614), re15_enemy_bone_world_pos */
#include "re15_skeleton.h"       /* re15_sin_q12 / re15_cos_q12                                 */
#include "re15_math.h"           /* re15_squareroot0 (SquareRoot0)                              */
#include "re15_room.h"           /* g_current_room_id / g_room_rdt                               */
#include "re15_esp.h"            /* RE1.5-Raumbank-Blut (Stand-in fuer FUN_8001BF10)             */
#include "re15_audio.h"          /* re15_audio_core_se (Hook-P0 0x8005BA28)                      */
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

/* Per-Slot-Arbeitsbytes des Arms, die der Aktor nicht als Feld traegt (Port-Speicher). */
typedef struct {
    int16_t  home_yaw;      /* +0x218 @0x80100188 */
    int32_t  home_x;        /* +0x21C @0x8010018C */
    int32_t  home_z;        /* +0x220 @0x80100194 */
    uint8_t  latch224;      /* +0x224 (INIT 0 @0x80100174; 1 @0x80100650-54)                 */
    uint8_t  bone_base;     /* +0x1C1 = Tabelle 0x80101414[var*7] = 1 (A) / 8 (B)             */
    uint8_t  hidden;        /* Entity-Wort-0 Bit 2 (0x80101164: |= 2 bei a2 = 0, &= ~2 bei 1) */
    uint8_t  init_done;
} re2arm_t;
static re2arm_t s_arm[RE15_ACTOR_MAX];

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

/* ---- kleine Helfer ------------------------------------------------------------------------ */
int re15_re2arm_owns(const re15_actor_t *e)
{
    return e && e->active && e->type == 0x1Au && re15_ai_re2_for_type(0x1Au);
}
int re15_re2arm_holder_slot(void) { return s_holder; }
int re15_re2arm_hand_bone(const re15_actor_t *e)
{   /* Tabelle @0x80101414 = {1..7 | 8..14}; Hand = [var*7 + 2] (@0x801007DC / @0x801009D4 / @0x80100BEC) */
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
    /* Die sieben Bones des ANDEREN Arms: word0 = 0 (@0x80100240 fuer Arm B inaktiv -> 1..7 weg,
     * @0x801002F8 fuer Arm A inaktiv -> 8..14 weg). */
    return (e->re2z_f10e & 1u) ? 0x00FEu : 0x7F00u;
}

/* Clipwort `sw (rate<<16)|(frame<<8)|clip, 332(s0)` -> Port-Animationsfelder (Konvention der
 * Wellen B/C: Rate -> anim_frac-Seed, a3 von 0x8002959C -> anim_blend_rate). */
static void arm_clip(re15_actor_t *e, int clip, int rate)
{
    e->motion = (int16_t)clip; e->anim_frame = 0; e->anim_frac = (uint8_t)rate;
    e->anim_blend_rate = 256; e->anim_freeze = 0;
    e->anim_flags &= (uint16_t)~(0x80u | 0x04u);
}
static int arm_frame(const re15_actor_t *e)        /* +0x14D */
{
    int fc = re15_actor_clip_len(e);
    return (fc > 0) ? (int)(e->anim_frame % (uint32_t)fc) : (int)e->anim_frame;
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
static int arm_hand_within(const re15_actor_t *e, const re15_actor_t *pl, int r, int32_t out_hand[3])
{
    int32_t h[3];
    re15_enemy_bone_world_pos(e, re15_re2arm_hand_bone(e), h);
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
    arm_anker_1210(e);                                    /* Port-Bruecke: Ursprung an die Wand (s. Kopf) */
    a->home_yaw = e->rot_y; a->home_x = e->x; a->home_z = e->z;   /* +0x218/+0x21C/+0x220 @0x80100188-194 */
    a->bone_base = (uint8_t)((e->re2z_f10e & 1u) ? 8 : 1);        /* +0x1C1 = 0x80101414[var*7] @0x801001D0/@0x80100288 */
    /* Part-Werte 96/512/312 (@0x801001D8-F8) = Part-Physikfelder, im Port ohne Leser.
     * 0x80101164(self, var, 0) @0x80100304: aktiver Arm ausgeblendet, Entity-Bit 2. */
    arm_show(e, slot, 0);
    s_cd_cfbf4 = 0;                                       /* sh zero,-1036(at) @0x80100324 = 0x800CFBF4 */
    /* +0x1E8 = 1 (Box-Zahl), +0x9E = 500, +0x9A/+0x9C/+0x90/+0x92/+0x1EE = 800 @0x8010032C-34C:
     * RE2-Trefferkasten — nicht uebernommen (Kopf). */
    s_se_cd = 0;                                          /* sb zero,5412(at) @0x80100358 = 0x80101524 */
    arm_clip(e, 0, 0);                                    /* sw zero,332(s0) @0x80100368 (Clip 0, Rate 0) */
    (void)re15_re2_advance_959c(e, 256);                  /* jal 0x8002959C @0x80100370 */
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
        (void)re15_re2_advance_959c(e, 256);              /* jal 0x8002959C @0x801006D0 */
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
        int done = re15_re2_advance_959c(e, 256);         /* jal 0x8002959C @0x80100740 */
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
static void arm_A1(re15_actor_t *e, re15_actor_t *pl)
{
    if (arm_hand_within(e, pl, 900, NULL))                /* FUN_800157D4(&PL, part[Hand]+0x5C, 900) @0x801007C4/@0x80100804 */
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
        (void)re15_re2_advance_959c(e, 256);              /* jal 0x8002959C @0x80100950 */
        int16_t v = e->re2z_t158;
        e->re2z_t158 = (int16_t)(v - 1);                  /* sh im Delay-Slot @0x80100968: IMMER */
        if (v == 0) re15_ai_set_state_word(e, 0x501u);    /* @0x8010096C-70 */
    }
}

/* ---- A3 @0x801009A0 / B3 @0x80100A90 (ZUGRIFF 0x301) ----------------------------------------- */
static void arm_A3(re15_actor_t *e, re15_actor_t *pl)
{
    if (!arm_hand_within(e, pl, 600, NULL)) return;       /* FUN_800157D4(…,600) @0x801009BC/@0x801009FC */
    if (arm_frame(e) < 5) return;                         /* sltiu 5 @0x80100A18 */
    if (s_cd_cfbf4 != 0) return;                          /* lhu 0x800CFBF4 @0x80100A28-30 */
    if ((pl->re2z_self1d3 & 0x80u) || re15_player_is_grabbed()) {   /* PL+0x1D3 & 0x80 @0x80100A3C-48
                                                                     * (+ Port-Pin anderer Greifer) */
        re15_ai_set_state_word(e, 0x601u);                /* addiu 1537 @0x80100A4C / sw @0x80100A78 */
        return;
    }
    re15_ai_set_state_word(e, 0x401u);                    /* sw 1025,4 @0x80100A50-54 */
    pl->re2z_self1d3 |= 0x80u;                            /* @0x80100A58-6C */
}
static void arm_B3(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {                            /* P0 @0x80100AEC (faellt in P1 @0x80100AFC) */
        arm_clip(e, 3, 15);                               /* 0xF0003 @0x80100ACC/AEC-F0 */
        e->sub_state_2 = 1;                               /* @0x80100AF4-F8 */
    }
    if (e->sub_state_2 == 1) {                            /* P1 @0x80100AFC */
        re15_enemy_steer_point(e, pl->x, pl->z, 48);      /* FUN_80015558(self, PL.x, PL.z, 48) @0x80100B10-14 */
        int done = re15_re2_advance_959c(e, 256);         /* @0x80100B24 */
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
        re15_enemy_bone_world_pos(e, re15_re2arm_hand_bone(e), h);   /* part[Hand] @0x80100BEC-C14 */
        pl->x = h[0];                                     /* sw v0,0x800CFC30 (PL+0x38) @0x80100C18-20 */
        pl->z = h[2];                                     /* sw v0,0x800CFC38 (PL+0x40) @0x80100C24-38 */
        s_holder = slot;                                  /* 0x800CFDAC = PL+0x1B4 = self @0x80100C2C-30 */
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
        (void)re15_re2_advance_959c(e, 256);              /* @0x80100D18 */
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
        int done = re15_re2_advance_959c(e, 256);         /* @0x80100E08 */
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
static void arm_B6(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: arm_clip(e, 5, 15); e->sub_state_2 = 1; break;      /* 0xF0005 @0x80100EE8-EC, sb 1,6 @0x80100EF0-F8 */
    case 1:
        (void)re15_re2_advance_959c(e, 256);                     /* @0x80100F00 */
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
    re15_ai_set_state_word(e, 0x501u);                    /* sw 1281,4 @0x80100FA0-A4 */
    e->re2z_self1d3 |= 0x80u;                             /* @0x80100FA8-B0 */
    arm_se((re15_re2_rand() & 1u) ? 2 : 3);               /* @0x80100FAC-C8: rng&1 -> 2, sonst 3 */
}
static void arm_death(re15_actor_t *e, int slot)
{
    /* FUN_8001BF10(0x21000, yaw, part[0x80101414[var*7]]+0x48, {0,0,0}) @0x80101028-8C: RE2-FX
     * (nicht portiert) -> RE1.5-Raumbank-Blut am Bone 1/8 (Stand-in wie enemy_ai_re2_dog.c). */
    {   int32_t p[3];
        re15_enemy_bone_world_pos(e, (int)s_arm[slot].bone_base, p);
        const re15_esp_t *bank = re15_esp_room_bank();
        if (bank) re15_esp_fx_spawn_ex(bank, 0, 0, 0x2000, p[0], p[1], p[2], (int16_t)e->rot_y);
    }
    e->hp = 250;                                          /* sh 250,342 @0x80101090-94 */
    re15_ai_set_state_word(e, 0x501u);                    /* @0x8010109C-A0 */
    e->re2z_self1d3 |= 0x80u;                             /* @0x80101098-AC */
    arm_se((re15_re2_rand() & 1u) ? 2 : 3);               /* @0x801010B0-C4 */
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
        break;
    }
    case 1: (void)hook_advance(pl); break;                /* @0x8010131C */
    case 2: {                                             /* @0x80101338 */
        pl->motion = 1; pl->anim_frame = 0; pl->anim_frac = 7; pl->anim_blend_rate = 256;    /* 0x70001 */
        s_hook_phase = 3;                                 /* @0x80101344-48 */
        s_hook_phase = (uint8_t)(s_hook_phase + hook_advance(pl));   /* @0x80101358-70 */
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
    }
    if (e->state == 0) { arm_init(e, slot); return 1; }   /* Raumlade-Tick FUN_8004A694 (0x8000 ausgeblendet) */
    if (!s_arm[slot].init_done) { arm_init(e, slot); return 1; }

    /* SCHLAF: +0x10E & 0x8000 -> kein Tick (@0x80026590-9C). ROOM1210: +0x9 & 0x1F != 1 (sub02
     * Member_set(12,1) @0x1EDA -> `sb a2,9(a0)` @0x800411F8). */
    if ((e->grid_id & 0x1Fu) != 1u) return 1;

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
        case 1: arm_A1(e, pl);       break;               /* 0x801007A8 */
        case 3: arm_A3(e, pl);       break;               /* 0x801009A0 */
        default: break;                                   /* 2/4/5/6/7 = jr ra */
        }
        switch (e->sub_state_1) {
        case 0: arm_B0(e, pl, slot); break;               /* 0x80100674 */
        case 1: arm_B1(e, pl, slot); break;               /* 0x80100830 */
        case 3: arm_B3(e, pl);       break;               /* 0x80100A90 */
        case 4: arm_B4(e, pl, slot); break;               /* 0x80100B68 */
        case 5: arm_B5(e, slot);     break;               /* 0x80100D90 */
        case 6: arm_B6(e, pl);       break;               /* 0x80100E9C */
        default: break;                                   /* 2/7 = jr ra */
        }
        break;
    }
    case 2: arm_hurt(e);        break;                    /* 0x80100F50 */
    case 3: arm_death(e, slot); break;                    /* 0x80100FE0 */
    default: break;                                       /* 4 = SKRIPT-Hook (nur ueber den Scheduler
                                                           * erreichbar, s. Kopf), 5/6 = 0, 7 = jr ra */
    }
    /* Root-Tail FUN_80035530 (Part-Matrizen + Koerper-Push, Bit-2-Ausstieg): Push im Port ueber
     * hit_radius (0 solange verborgen), Matrizen ueber den Renderer. */

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
