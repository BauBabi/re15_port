/*
 * RE1.5 Rebuilt — KLETTERN (Spieler-Substates 9 und 10).
 *
 * Der ACTION-Knopf (virtuelles Bit 0x80 von DAT_800ac76c) probiert im Original
 * ZUERST das Klettern und erst danach den AOT-/Tuer-Scan. Der Ausloeser steht
 * IDENTISCH in allen vier Sub-ENTRY-Handlern des Spielers (selbst disassembliert):
 *
 *   Idle   @0x80031fb8 lw DAT_800ac76c / @0x80031fc4 andi 0x80 / @0x80031fc8 beq
 *          @0x80031fd0 jal FUN_8002d474 / @0x80031fd8 bne v0 -> fertig
 *          sonst @0x80031fe4 jal FUN_80042bac(player, 1, 0x10)   (= AOT/Tuer)
 *   Gehen  @0x800323d8 / e0 / e4 / ec / f4  + @0x80032400
 *   Rennen @0x800326a8 / b0 / b4 / bc / c4  + @0x800326d0
 *   Sub3   @0x80032a28 / 30 / 34 / 3c / 44  + @0x80032a50
 *
 * Spieler-Entity-Basis = 0x800aca54 (belegt: @0x80031f44 `addiu s0,0x800ac768`,
 * @0x80031fdc `addiu a0,s0,748` = 0x800aca54 = der a0 von FUN_80042bac/FUN_8002d1e8;
 * und @0x8002d6e8+0x8002d6fc `addiu a0,0x800aca3c,24`). Daraus:
 *   +0x05 = 0x800aca59 SUBSTATE  (Index in PTR_LAB_80073fb0/80073ff0, @0x80031ed0-f14)
 *   +0x06 = 0x800aca5a KIND      +0x07 = 0x800aca5b PHASE
 *   +0x34/38/3c = X/Y/Z          +0x40/42/44 = posS (u16-Spiegel)
 *   +0x6a = 0x800acabe yaw       +0x82 = 0x800acad6 BAND
 *   +0x8c = 0x800acae0 speed     +0x8f = 0x800acae3 crossfade
 *   +0x93 = 0x800acae7 hit_react +0x94/95 = 0x800acae8/e9 clip/frame
 *
 * Substate 9  = LAB_80037fd8 (ENTRY 0x80037fd0 = `jr ra`) — HOCHKLETTERN
 * Substate 10 = LAB_80038314 (ENTRY 0x8003830c = `jr ra`) — HERUNTERSPRINGEN
 */
#ifndef RE15_CLIMB_H
#define RE15_CLIMB_H

#include <stdint.h>
#include "re15_rdt.h"
#include "re15_emd.h"

/* Motion-Sentinels (Port-Repraesentation der Original-Clip-Register +0x94).
 * Die Bank-Auswahl steht in anim_select_common.c. */
#define RE15_PLAYER_MOTION_CLIMB_TURN  230  /* W01  clip 5  (@0x80038030 `ori v0,zero,0x5`) */
#define RE15_PLAYER_MOTION_CLIMB_UP    231  /* PL00 clip 2  (@0x80038134 `ori v0,zero,0x2`) */
#define RE15_PLAYER_MOTION_CLIMB_D0    232  /* PL00 clip 3  (@0x80038368 `ori v0,zero,0x3`) */
#define RE15_PLAYER_MOTION_CLIMB_D1    233  /* PL00 clip 4  (@0x800383d8 `ori v0,zero,0x4`) */
#define RE15_PLAYER_MOTION_CLIMB_LAND5 234  /* PL00 clip 5  (@0x800386e4 `ori v1,zero,0x5`) */
#define RE15_PLAYER_MOTION_CLIMB_LAND6 235  /* PL00 clip 6  (@0x80038700 `ori v0,zero,0x6`) */

/* FUN_8002d474 @0x8002d474 — die ACTION-Sonde. Rueckgabe 1 = die Aktion ist
 * VERBRAUCHT (Substate 9 gesetzt), 0 = weiter zum Tuer-/AOT-Scan.
 * MUSS VOR dem AOT-Scan laufen (byte-true Reihenfolge, s.o.). */
int  re15_climb_try_start(const re15_rdt_t *rdt, int action_pressed);

/* 1 solange der Kletter-FSM den Spieler haelt (Substate 9 oder 10). */
int  re15_climb_active(void);

/* Ein Tick des Kletter-FSM (LAB_80037fd8 / LAB_80038314). */
void re15_climb_tick(const re15_rdt_t *rdt,
                     const re15_emd_skeleton_t  *pl00_skel,
                     const re15_emd_animation_t *pl00_anim,
                     const re15_emd_animation_t *w01_anim);

/* Der Per-Frame-Block @0x80031cd8-0x80031d2c, der DIREKT nach dem Spieler-FSM
 * laeuft: DAT_800ac788 = 0 / DAT_800aca3c &= ~0x4000, dann
 * FUN_8002d100(player, 0x12); traegt das Objekt Band+1 == Spieler-Band,
 * DAT_800ac788 = Objekt und DAT_800aca3c |= 0x4000 ("steht auf einem Objekt"). */
void re15_climb_standing_tick(void);

/* Derselbe Test (FUN_8002d100(player,0x12) + `obj[0x82]+1 == DAT_800acad6`,
 * @0x80031ce8-0x80031d20), aber an einer FREI GEWAEHLTEN XZ-Position und ohne
 * Zustandsschreiben. Rueckgabe = Objektindex (= DAT_800ac788) oder -1.
 * Der Objekt-Pass FUN_8002bd44 (@0x8001ce14, direkt nach FUN_80031c44 @0x8001ce0c)
 * braucht den Wert der AKTUELLEN Position, nicht den vom Frame-Ende davor. */
int  re15_climb_standing_probe(int32_t x, int32_t z);

/* Raumwechsel/Reset. */
void re15_climb_reset(void);

/* Diagnose fuer die Sonden/Tests: Substate (0/9/10), KIND (+0x06), PHASE (+0x07),
 * das getroffene Objekt (DAT_800ac78c) und das Standobjekt (DAT_800ac788). */
int  re15_climb_dbg_sub(void);
int  re15_climb_dbg_kind(void);
int  re15_climb_dbg_phase(void);
int  re15_climb_dbg_obj(void);
int  re15_climb_dbg_standing(void);

#endif /* RE15_CLIMB_H */
