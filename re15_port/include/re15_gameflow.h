/*
 * re15_gameflow.h — the shared top-level game-mode machine (FE-0.2 of
 * PORTING_ROADMAP.md). De-hardcodes the boot-into-a-room monolith: the engine
 * carries the mode + transitions; each platform main.c drives the per-mode
 * render/input. Byte-true source: the RE1.5 lifecycle-flag word DAT_800aca38
 * (main loop @0x8001cce0, INGAME = bit 0x40000000; attract handoff @0x80015838
 * clears it). We model those flags as a CLEAN enum (the observable behavior is
 * byte-true; the internal representation is the port's).
 */
#ifndef RE15_GAMEFLOW_H
#define RE15_GAMEFLOW_H

#include <stdint.h>

typedef enum {
    RE15_MODE_BOOT = 0,     /* cold init (SDL / PSX boot) -> TITLE (or INGAME debug fast-path) */
    RE15_MODE_TITLE,        /* DATA/TITLEU.TIM + PRESS START + NEW GAME/CONTINUE/OPTION menu     */
    RE15_MODE_CHARSELECT,   /* Leon / Elza pick (PL00 / PL04 — DAT_800ACA5C 0 bzw. 4).
                             * ⛔ TOTE DEKLARATION: die Auswahl laeuft als Unterschleife IN
                             * RE15_MODE_TITLE (pc_run_player_select), dieser Modus wird nirgends
                             * gesetzt oder abgefragt. Byte-true ist das nicht falsch — das
                             * Original hat dafuer ebenfalls keinen eigenen Lebenszyklus-Zustand,
                             * sondern eine Task-Ersetzung (@0x80102c9c).                        */
    RE15_MODE_FMV,          /* CAPCOM.STR opening movie (opcode 0x6F Movie_on)                   */
    RE15_MODE_INGAME,       /* the shared game step (game_step_common.c) — byte-true, unchanged  */
    RE15_MODE_PAUSE,        /* START-pause                                                       */
    RE15_MODE_INVENTORY,    /* inventory/status overlay                                          */
    RE15_MODE_GAMEOVER,     /* death presentation FSM (game_step_common.c re15_gameover_fsm_tick)*/
} re15_gameflow_mode_t;

typedef struct {
    re15_gameflow_mode_t mode;
    uint16_t start_room;    /* room entered on NEW GAME. Byte-true RE1.5 new game = the intro
                             * ROOM1240 (pre-intro montage -> ROOM1170 helipad -> gameplay). */
    int      enter_ingame;  /* one-shot request: the platform should (re)enter INGAME by loading
                             * `start_room` this frame, then clear it. Set by NEW GAME / CONTINUE. */
    int      character;     /* PLD-INDEX, der Port-Zwilling von DAT_800ACA5C: 0 = Leon (PL00),
                             * 4 = Elza (PL04). ⛔ NICHT 0/1 — das Original schreibt genau diese
                             * beiden Werte, an zwei Stellen in TITLE.BIN:
                             *   801016a4  sll v0,v0,2        ; Auswahl-Cursor << 2  -> 0 / 4
                             *   801016ac  sb  v0,-13732(at)  ; DAT_800ACA5C
                             *   801024c0  sb  zero,-13732(at)             ; LEON = 0
                             *   801024cc  ori v0,zero,0x4 / 801024d4 sb   ; ELZA = 4
                             * Der Wert IST der Index in die PLD-Dateitabelle 0x80073f70 (16 u16 =
                             * CD-Index 60..75), gelesen von FUN_800314b0 @0x800314d4 `lbu` +
                             * `sll v0,v0,1`: 0 -> 60 = PL00.PLD, 4 -> 64 = PL04.PLD.
                             * Deshalb ist der Charakter-Diskriminator im ganzen Spiel `& 4`
                             * (@0x80104008 `andi v0,v0,0x4`, gleichlautend in STAGE1..5) — mit der
                             * frueheren 0/1-Kodierung war JEDER dieser Tests tot. */
    int      boot_movie;    /* the CAPCOM.STR opening-logo gate = DAT_800aca38 bit 0x8000, tested
                             * one-shot at the main-loop front (main.c:20 / 0x80020c28) and cleared
                             * on play (& ~0x8000 @0x80020c48). In the shipped RE1.5 MZD build this
                             * bit is DORMANT — NOTHING sets it (exhaustive PSX.EXE + STAGE1-6.BIN +
                             * DEBUG.BIN scan; 7 savestates read 0; SYSTEM.CNF BOOT=PSX.EXE = single-
                             * EXE boot, no shell) — so the logo never auto-plays; the disc boots
                             * straight to the front-end. Modelled faithfully: default 0 (dormant).
                             * The byte-true movie player runs iff this is armed. */
} re15_gameflow_t;

extern re15_gameflow_t g_gameflow;

/* ELZA-BIT — der Port-Zwilling von DAT_800ACA3C Bit 31.
 * Beide Zweige des Auswahlschirms spiegeln die Wahl sofort in dieses Wort (TITLE.BIN):
 * Leon `lui v1,0x7fff` @0x801024ac + `ori v1,v1,0xffff` @0x801024b8 + `and v0,v0,v1`
 * @0x801024c8; Elza `lui v1,0x8000` @0x801024a8 + `or v0,v0,v1` @0x801024e4; Store
 * `sw v0,-13764(at)` @0x801024ec. Weil das Bit im Original AUS dem Charakter-Byte
 * abgeleitet und nur gespiegelt wird, fuehrt der Port kein zweites Feld, sondern
 * dieselbe Ableitung. Gelesen wird es als `srl a0,a0,31` @0x800397e4 (Ergebnis 0/1). */
static inline int re15_char_variant(void)
{ return (g_gameflow.character & 4) ? 1 : 0; }

/* RAUM-ID -> DATEIVARIANTE. Das Original rechnet nicht mit Raum-Ids, sondern mit
 * CD-Dateiindizes: FUN_800396fc holt den Basisindex aus der Stage-Tabelle
 * (Zeigertabelle 0x8007438c @0x800397cc, `lhu v0,0(v1)` @0x800397e0) und ADDIERT das
 * Elza-Bit darauf (`addu a0,a0,v0` @0x800397ec). Die Stage-0-Tabelle @0x8007429c laeuft
 * in Schritten von 3 (681, 684, 687, 690, …) = drei CD-Dateien je Raum (BSS +
 * Leon-RDT + Elza-RDT); die Elza-RDT ist also genau die Leon-RDT + 1.
 * Im Port heisst die Datei ROOM<stage+1><raum:2hex><variante>.RDT — die Variante ist
 * damit die NIEDRIGSTE Hex-Ziffer der Raum-Id, und `| variante` ist die ganze Rechnung.
 * (Gegenprobe des Port-Zustands: alle 649 aufloesbaren Door_aot_set der 240 RDT zeigen
 * auf einen Raum DERSELBEN Variante, aot_common.c:572-575 schleppt sie schon mit.) */
#define RE15_ROOM_BASE(id)  ((unsigned)(id) & 0xFFF0u)
static inline uint16_t re15_room_for_char(unsigned base)
{ return (uint16_t)(RE15_ROOM_BASE(base) | (unsigned)re15_char_variant()); }

/* Initialise the machine — immer Boot zum TITLE. Der frueher hier moegliche
 * RE15_START_ROOM-Schnellweg (direkt INGAME in einen Raum) ist entfernt: er umging
 * re15_room_apply_pending und wich dadurch von dem ab, was im Spiel zu sehen ist.
 * Raumwechsel laufen ausschliesslich ueber Tueren oder das Original-Debug-Menue. */
void re15_gameflow_init(void);

static inline re15_gameflow_mode_t re15_gameflow_mode(void) { return g_gameflow.mode; }
static inline void re15_gameflow_set_mode(re15_gameflow_mode_t m) { g_gameflow.mode = m; }

/* Transitions (the byte-true edges of the RE1.5 flag machine). */
/* char_index = der Auswahl-CURSOR (0 = Leon, 1 = Elza), NICHT das Charakter-Byte; die
 * Umrechnung `<< 2` auf 0/4 steht in re15_gameflow.c (@0x801016a4). */
void re15_gameflow_new_game(int char_index); /* TITLE -> (CHARSELECT ->) enter INGAME at start_room */
void re15_gameflow_to_gameover(void);        /* INGAME -> GAMEOVER (death FSM took over)            */
void re15_gameflow_to_title(void);           /* GAMEOVER/attract -> TITLE (clears the INGAME flag)  */

#endif /* RE15_GAMEFLOW_H */
