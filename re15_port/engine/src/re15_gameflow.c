/*
 * re15_gameflow.c — the shared top-level game-mode machine (FE-0.2).
 * See re15_gameflow.h. This carries the mode + the byte-true transitions; the
 * per-mode render/input lives in each platform main.c so PSX and PC share the
 * exact same flow. The RE1.5 original keyed everything off the lifecycle-flag
 * word DAT_800aca38 (INGAME = bit 0x40000000, set/cleared at @0x8001ca2c-cbf4 /
 * @0x80015838); we surface those edges as named transitions.
 */
#include "re15_gameflow.h"

re15_gameflow_t g_gameflow;

/* NEW-GAME start room, OHNE Spielervariante — die haengt re15_gameflow_new_game an.
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
#define RE15_NEWGAME_ROOM 0x1240

/* KEIN Raum-Sprung-Parameter mehr. Es gab hier einen RE15_START_ROOM-Schnellweg, der direkt
 * INGAME in einen beliebigen Raum bootete. Der ist ENTFERNT (2026-08-01, auf Ansage des Nutzers),
 * weil er systematisch andere Ergebnisse lieferte als das, was im Spiel zu sehen ist:
 * er installierte den Raum AN re15_room_apply_pending VORBEI (eigene Boot-Sequenz in main.c),
 * waehrend jeder echte Raumwechsel — Tuer wie Debug-JUMP — durch apply_pending laeuft.
 * Damit fehlten dem Sprung genau die Schritte, die den Raum erst in den Spielzustand bringen
 * (Teardown, Motion-Reset, Bank-/BGM-Wechsel, Kamera-Cut, SCD-Reenter).
 * Raumwechsel gehen jetzt ausschliesslich ueber das ORIGINAL-Debug-Menue (UTILITY MENU,
 * PSX.EXE @0x80014444) bzw. ueber Tueren — beide muenden in re15_room_request_change(). */
void re15_gameflow_init(void)
{
    g_gameflow.mode         = RE15_MODE_TITLE;
    g_gameflow.start_room   = RE15_NEWGAME_ROOM;
    g_gameflow.enter_ingame = 0;
    g_gameflow.character    = 0;          /* Leon = PLD-Index 0 = PL00 (@0x801024c0) */
    g_gameflow.boot_movie   = 0;          /* dormant, byte-true to the MZD build (see header) */
}

void re15_gameflow_new_game(int char_index)
{
    /* CURSOR -> CHARAKTER-BYTE: `sll v0,v0,2` @0x801016a4, Store @0x801016ac.
     * Der Zwei-Zweig-Schreiber desselben Schirms kommt auf dieselben Werte
     * (@0x801024c0 `sb zero` = 0, @0x801024cc `ori v0,zero,0x4` + @0x801024d4 = 4). */
    g_gameflow.character    = (char_index & 3) << 2;
    /* STARTRAUM MIT VARIANTE: `srl a0,a0,31` @0x800397e4 + `addu a0,a0,v0` @0x800397ec
     * — im Port ist der Dateiindex die Raum-Id, also `| Elza-Bit`. Leon 0x1240,
     * Elza 0x1241 (Herleitung + Bytebeleg oben bei RE15_NEWGAME_ROOM). */
    g_gameflow.start_room   = re15_room_for_char(RE15_NEWGAME_ROOM);
    g_gameflow.enter_ingame = 1;         /* platform enters INGAME + loads the start room */
    g_gameflow.mode         = RE15_MODE_INGAME;
    {   /* Blut-Decal-Reset: der Wund-Builder FUN_80037c1c laeuft NUR im Spieler-Load-Pfad
         * (@0x800316c8/@0x800318cc, Teil von FUN_800314b0) und nullt Level+Akku — New Game
         * und CONTINUE/Load starten also blutfrei; Raumwechsel dagegen NICHT (BD-6,
         * analysis/blood_decals.md §5). CONTINUE laeuft durch denselben new_game-artigen
         * Einstieg der Plattform; ein separater Load-Pfad muss diesen Reset mitrufen. */
        extern void re15_wound_reset(void);
        re15_wound_reset();
        {   /* RE2-Kartensystem: neues Spiel = Karte leer (re15_map_visited.c) */
            extern void re15_map_visited_reset(void);
            re15_map_visited_reset();
        }
        {   /* ⛔ EINE VORGEMERKTE "Discard it?"-ABFRAGE DARF NICHT IN DEN NAECHSTEN LAUF
             * REITEN. Sie wuerde dort die erste echte Abfrage verschlucken
             * (re15_discard_notice_message kehrt bei s_zustand != D_AUS sofort um). Das
             * Original hat den Fall nicht, weil sein armierter Zustand ein CODE-Zeiger
             * ist, den der Antwort-Zweig aushaengt (@0x800517d0 `sw zero,DAT_800d4498`);
             * der Port fuehrt eine FSM und muss sie hier ausdruecklich raeumen. */
            extern void re15_discard_reset(void);
            re15_discard_reset();
        }
        {   /* FILE-Liste: neues Spiel = 24 leere Plaetze. RE2 fuellt sie beim Spielstart
             * mit 0xFF (`addiu a1,zero,24` @0x800682dc, `addiu v0,zero,255` @0x800682e0,
             * `sb v0,19304(at)` @0x800682f0 = 0x800d4b68 + a1). RE1.5 hat keine
             * beschreibbare Liste (Maske @0x800c6c98 ohne Schreiber), deshalb RE2. */
            extern void re15_files_reset(void);
            re15_files_reset();
        }
    }
}

void re15_gameflow_to_gameover(void)
{
    g_gameflow.mode = RE15_MODE_GAMEOVER;
    /* Tod = Ende dieses Laufs. Eine vorgemerkte Wegwerf-Abfrage faellt mit weg, sonst
     * stuende sie beim CONTINUE noch da (Begruendung bei re15_gameflow_new_game). */
    { extern void re15_discard_reset(void); re15_discard_reset(); }
}

void re15_gameflow_to_title(void)
{
    /* attract handoff (@0x80015838 clears the INGAME flag) -> back to the title. */
    g_gameflow.mode = RE15_MODE_TITLE;
    { extern void re15_discard_reset(void); re15_discard_reset(); }
}
