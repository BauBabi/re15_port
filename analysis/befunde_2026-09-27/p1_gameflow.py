import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elza_ed import rep

H = "re15_port/include/re15_gameflow.h"
rep(H, [
(
"""    int      character;     /* 0 = Leon (PL00), 1 = Elza (PL01) — the char-select result. */""",
"""    int      character;     /* PLD-INDEX, der Port-Zwilling von DAT_800ACA5C: 0 = Leon (PL00),
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
                             * frueheren 0/1-Kodierung war JEDER dieser Tests tot. */"""
),
(
"""extern re15_gameflow_t g_gameflow;
""",
"""extern re15_gameflow_t g_gameflow;

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
"""
),
(
"""void re15_gameflow_new_game(int character);  /* TITLE -> (CHARSELECT ->) enter INGAME at start_room */""",
"""/* char_index = der Auswahl-CURSOR (0 = Leon, 1 = Elza), NICHT das Charakter-Byte; die
 * Umrechnung `<< 2` auf 0/4 steht in re15_gameflow.c (@0x801016a4). */
void re15_gameflow_new_game(int char_index); /* TITLE -> (CHARSELECT ->) enter INGAME at start_room */"""
),
])

C = "re15_port/engine/src/re15_gameflow.c"
rep(C, [
(
"""/* Byte-true NEW-GAME start room: the RE1.5 prototype boots the intro at ROOM1240
 * (pre-intro narrator montage -> ROOM1170 helipad cutscene -> handoff to play).
 * This is the current default boot room, so NEW GAME reproduces the real opening. */
#define RE15_NEWGAME_ROOM 0x1240""",
"""/* NEW-GAME start room, OHNE Spielervariante — die haengt re15_gameflow_new_game an.
 * Der Port bootet das Spiel in die Vorspann-Montage ROOM124x; ROOM1240 uebergibt an
 * ROOM1170 (Helipad, Leon), ROOM1241 an ROOM1031 (Lobby, Elza).
 *
 * ⛔ DAS IST KEINE ERFINDUNG, SONDERN IN DEN DATEN GEMESSEN. Beide Montage-RDT tragen
 * genau EINEN Door_aot_set in main00, und sie unterscheiden sich in genau EINEM Byte —
 * dem Zielraum-Index bei +23 (Datei-Offset 0x0531):
 *   ROOM1240.RDT @0x0531 = 0x17   -> Raum 0x17 = ROOM1170  "HELIPORT"
 *   ROOM1241.RDT @0x0531 = 0x03   -> Raum 0x03 = ROOM1031  "LOBBY"
 * Das sind Zeichen fuer Zeichen die beiden Raumindizes, die der Original-Einstieg
 * FUN_8001d22c fest verdrahtet hat: `bltz v0,0x8001d324` @0x8001d2a4 auf
 * DAT_800ACA3C Bit 31, Leon `ori v0,zero,0x17` @0x8001d2a8 + `sh` @0x8001d2b0,
 * Elza `ori v0,zero,0x3` @0x8001d324 + `sh` @0x8001d32c (beide Stage 0). Die Montage
 * setzt also nur den Raumindex, den die EXE ohnehin gesetzt haette — die Variante
 * fuehrt den Spieler von selbst in seine eigene Kette.
 * (Raumnamen aus DEBUG.BIN @0x800c263c, 26-Byte-Satz je Raum, Index
 *  (637*stage + 13*raum)*2 aus @0x8001d39c-3c8: 0x03 "LOBBY", 0x17 "HELIPORT",
 *  0x24 "OPENING".) */
#define RE15_NEWGAME_ROOM 0x1240"""
),
(
"""    g_gameflow.character    = 0;          /* Leon (PL00) */""",
"""    g_gameflow.character    = 0;          /* Leon = PLD-Index 0 = PL00 (@0x801024c0) */"""
),
(
"""void re15_gameflow_new_game(int character)
{
    g_gameflow.character    = character;
    g_gameflow.start_room   = RE15_NEWGAME_ROOM;""",
"""void re15_gameflow_new_game(int char_index)
{
    /* CURSOR -> CHARAKTER-BYTE: `sll v0,v0,2` @0x801016a4, Store @0x801016ac.
     * Der Zwei-Zweig-Schreiber desselben Schirms kommt auf dieselben Werte
     * (@0x801024c0 `sb zero` = 0, @0x801024cc `ori v0,zero,0x4` + @0x801024d4 = 4). */
    g_gameflow.character    = (char_index & 3) << 2;
    /* STARTRAUM MIT VARIANTE: `srl a0,a0,31` @0x800397e4 + `addu a0,a0,v0` @0x800397ec
     * — im Port ist der Dateiindex die Raum-Id, also `| Elza-Bit`. Leon 0x1240,
     * Elza 0x1241 (Herleitung + Bytebeleg oben bei RE15_NEWGAME_ROOM). */
    g_gameflow.start_room   = re15_room_for_char(RE15_NEWGAME_ROOM);"""
),
])
