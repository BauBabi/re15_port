/*
 * RE1.5 Rebuilt — "Der Schluessel wird nicht mehr gebraucht. Wegwerfen?"
 *
 * NUTZER-AUFTRAG 2026-09-21: "bei KEy Items, die verwendet wurden, und nicht mehr im
 * Spiel danach benoetigt werden genauso wie in Resident Evil 2 fragst - This Item is not
 * used anymore - drop it? Yes No - oder so aehnlich.... schaue nach, wie das genau in
 * Resident Evil 2 heisst...."
 *
 * ============================================================================
 * 1. DER WORTLAUT — RE1.5 HAT EINEN EIGENEN, UND DER SCHLAEGT DIE RE2-ZEILE
 * ============================================================================
 * RE2 sagt  "This key is useless now." / "Discard?"   (RAM 0x8009F160 / 0x8009F179,
 * Nachricht (Gruppe 0x100, Index 9), Offsettabelle @0x8009F368 + Basis @0x8009EFCC,
 * Aufloeser FUN_8002FE38 @0x8002FF44/f4c).
 *
 * RE1.5 hat dafuer seine EIGENE Zeile — und deshalb wird SIE benutzt, nicht die RE2-Zeile:
 *
 *     @0x800C508B  35 4b 51 00 40 4b 4a 3a 50 00 4a 41 41 40 00 50   "You don't need t"
 *     @0x800C509B  44 45 4f 00 47 41 55 00 3d 4a 55                  "his key any"
 *     @0x800C50A6  08                                                Zeilenumbruch
 *     @0x800C50A7  49 4b 4e 41 57 00 20 45 4f 3f 3d 4e 40 00 45 50   "more. Discard it"
 *     @0x800C50B7  1b                                                '?'
 *     @0x800C50B8  03                                                Ja/Nein-Seite
 *     @0x800C50B9  02 f9 02                                          Branch, Ja-Aktion 0x02
 *     @0x800C50BC  01                                                END
 *
 *   => "You don't need this key any more. Discard it?"   Yes / No
 *
 * (DEBUG.BIN laedt RAW nach 0x800C0000, Datei == RAM; belegt debug_menu_common.c:81-96.
 *  Dekodiert mit re15_msg_glyph, msg_common.c:175-199.)
 *
 * Es ist Skript [6] von 8 der Prompt-Tabelle @0x800C4FC6 (Selektor FUN_80027e68:
 * @0x80027f20 `sll v0,a2,1`, @0x80027f30 `addiu at,at,20422` = 0x800C4FC6, @0x80027f44
 * `addu v0,v0,at` — die Tabelle ist zugleich ihre eigene Basis; der kleinste Offset 0x10
 * ist ihr Ende, also 8 Eintraege). Der dritte Branch-Operand @0x800C50BB unterscheidet
 * die Ja-Aktion: 0x00 nehmen, 0x01 benutzen/ablegen, 0x02 WEGWERFEN.
 * Das Skript liegt bereits vendort in engine/src/gen/item_prompt_data.inc, und
 * item_prompt_common.c:60 bildet Port-Schluessel 8 darauf ab (`case 8: return 6;`).
 *
 * ============================================================================
 * 2. ⛔ RE1.5 HAT KEINEN AUSLOESER — DAS IST EINE PORT-KONSTRUKTION
 * ============================================================================
 * Gemessen (Untersuchung B, 2026-09-21): `jal FUN_80027e68` = Wort 0x0C009F9A, 6 Aufruf-
 * stellen ueber PSX.EXE und alle 8 Overlays, alle inspiziert. Nur zwei betreten den Pfad
 * a1=0x100, der die Prompt-Tabelle indiziert, und zwar mit a2=0 und a2=1. Die Skripte
 * [2]..[7] — inklusive [6] — sind im Auslieferungsstand TOTE DATEN.
 * Ebenso: kein RE1.5-Skript-Opcode kann einen Gegenstand entfernen (Dispatch-Tabelle
 * @0x800744A8 hat 95 Eintraege, 0x00..0x5E; RE2s Sce_item_lost 0x62 existiert nicht).
 * Ein benutzter RE1.5-Schluessel bleibt also fuer immer im Inventar — genau die Luecke,
 * die der Auftrag schliesst.
 *
 * Die FORM der Regel ist dagegen byte-true von RE2 uebernommen (LAB_80051718):
 *     800517F4: bne v0,zero,0x80051870   ; Nachrichtensystem belegt -> noch nicht fragen
 *     80051808: lbu v0,-30635(at)        ; Zaehler-Byte des benutzten Schluessel-Slots
 *     80051810: addiu v0,v0,-1           ; ZAEHLER -= 1
 *     8005181C: sb   v0,-30635(at)
 *     80051824: bne  v0,zero,0x8005185c  ; Zaehler != 0 -> KEINE Abfrage
 *     8005182C: addiu a1,zero,256        ; 0x100
 *     80051830: addiu a2,zero,9          ; "This key is useless now / Discard?"
 *     80051834: jal  0x8002fe38
 *     80051850: sw   v0,[0x800CFBDC]     ; |= 0xFF000000 — Spieler eingefroren
 *   Ja:  80051774 sb zero,id / 80051794 sb zero,flags / 80051798 jal FUN_80069714 (kompaktieren)
 *   Nein:800517C4 sb v1,count  ; Anzahl := 1, sonst liefe sie beim naechsten Mal auf 0xFF
 * Eingehaengt wird die Fortsetzung genau dort, wo RE2 die Meldung "You have used the
 * <Name>." startet (@0x80051670) — dieselbe Stelle benutzt dieser Port (s. Punkt 3).
 *
 * ============================================================================
 * 3. DIE BENUTZUNGSSTELLE — GEMESSEN, NICHT GEWAEHLT
 * ============================================================================
 * RE1.5s Schluessel-Benutzung ist eine RAUM-Nachricht "You've used the <NAME>.". Das
 * Muster der drei Kartenleser ist in ROOM10D0/1230/11E0 sub20 byte-identisch:
 *     Message_on <frage>   ; "A card reader. You have to use the Blue Keycard ... ?" (0x03)
 *     Ck(12,31,0)          ; Antwort JA          (msg_common.c:550)
 *     Ck(9,<tk_bit>,1)     ; Karte GENOMMEN?     (zone-9-taken-Bit, Installer
 *                          ;                      @0x800406d4-0x80040718)
 *     Message_on <benutzt> ; "You've used the Blue Keycard."     <== HIER haengt die Abfrage
 *     Evt_exec sub17       ; Tuer oeffnen
 * Die Tabelle (Raum, Nachricht) -> Gegenstand wird von tools/gen_discard_sites.py aus den
 * ausgelieferten Daten ABGELEITET nach drei Aufnahmebedingungen (Name in der 102er-Item-
 * Tabelle, mindestens eine Item_aot_set-Ausgabe, GENAU EINE Benutzungsstelle je
 * Basisraum) und liegt in engine/src/gen/discard_sites.inc. Sie ist die EINZIGE Stelle,
 * an der steht, welche Gegenstaende betroffen sind.
 *
 * ============================================================================
 * 4. ⛔ KEIN GEGENSTAND KANN VERLOREN GEHEN — STRUKTURELL, NICHT PER LISTE
 * ============================================================================
 * Vollzensus tools/discard_zensus.py, 206 RDTs mit Header (+34 Stummel <0x48 B),
 * 0 Desync-Stopps, 40674 Opcodes in 72 verschiedenen Sorten:
 *   - Keep_Item_ck (0x5E) — der EINZIGE RE1.5-Opcode, dessen Handler den Inventar-Zeiger
 *     ueberhaupt anfasst (LAB_80042b04 -> FUN_80013278, liest 0x800ac99c) — kommt
 *     0 MAL vor. Und er ist ohnehin kein Praedikat: `ori v0,zero,0x1` @0x80042b44 liefert
 *     KONSTANT 1, er kann also nichts gaten.
 *   - Jedes Fortschritts-Tor ist eine FLAG-Pruefung (Ck 0x21, 1939 Vorkommen). Die
 *     Schluessel-Tore pruefen das zone-9-taken-Bit = "wurde der Gegenstand GENOMMEN",
 *     nicht "liegt er im Inventar".
 *   - Zone-9-Bits werden NIE geloescht: von 2973 Set-Opcodes zielt genau EINER auf Zone 9,
 *     und der setzt (op=1). Im ganzen Motor gibt es genau einen Schreiber,
 *     item_modal_common.c:289 `re15_game_flag_set(9, s_taken, 1)`.
 *   => Ein weggeworfener Gegenstand kann KEINEN Skript-Zweig veraendern. Eine Sackgasse
 *      ist damit ausgeschlossen, ohne dass dafuer eine Liste gepflegt werden muesste.
 * (Zusaetzlich gemessen von Untersuchung B: RE1.5 gatet auch keine Tuer ueber ein
 *  Daten-Schluesselfeld — pc[28] == 0 in ALLEN 653 Door_aot_set-Records.)
 */
#ifndef RE15_ITEM_DISCARD_H
#define RE15_ITEM_DISCARD_H

#include <stdint.h>

/* Der Raum meldet, dass er gerade seine "You've used the <NAME>."-Nachricht oeffnet.
 * Aufrufer: op_message_on (scd_vm.c), unmittelbar beim Oeffnen — das ist die Stelle, an
 * der RE2 seine Fortsetzung einhaengt (@0x80051670).
 * Wirkung: steht (room, msg) in discard_sites.inc UND liegt der Gegenstand noch im
 * Inventar, wird sein Zaehler heruntergezaehlt (@0x80051810) und die Abfrage vorgemerkt,
 * falls er dabei Null erreicht (@0x80051824). Sonst passiert nichts. */
/* `thread_slot` = der SCD-Faden, der die Nachricht ausgegeben hat (Index in
 * g_scd.threads). Die Abfrage wartet, bis GENAU DIESER Faden sein Unterprogramm beendet
 * hat — sonst friert sie ein Unterprogramm ein, das noch laeuft. Siehe re15_discard_tick. */
void re15_discard_notice_message(unsigned room_id, uint8_t msg_id, int thread_slot);

/* Ein 30-Hz-Spieltakt der Abfrage-FSM. `pad_edge`/`pad_held` sind die VIRTUELLEN
 * Pad-Woerter (re15_pad_virtual_word), wie beim Item-Modal. Nur aufrufen, solange
 * re15_discard_active() — der Aufrufer friert die uebrige Welt ein (RE2 @0x80051850). */
void re15_discard_tick(uint16_t pad_edge, uint16_t pad_held);

/* Nicht-null, solange die Abfrage laeuft oder auf das Ende der Raum-Nachricht wartet.
 * Der Warte-Zustand friert NICHT ein (RE2 wartet dort ebenfalls nur, @0x800517F4). */
int re15_discard_active(void);
int re15_discard_frozen(void);   /* nur waehrend die Abfrage sichtbar ist */

/* RENDER-Abfrage: 0 = nichts, sonst der Port-Prompt-Schluessel 8 (= BSS-Skript [6]).
 * *out_item = der Gegenstand (fuer den 0x06-Namen-Einsatz), *out_choice: 0 = Yes, 1 = No. */
int re15_discard_prompt(uint8_t *out_item, int *out_choice);
int re15_discard_reveal(void);        /* Schreibmaschinen-Budget (Glyphen bisher)       */
int re15_discard_reveal_total(void);  /* Glyphenzahl des offenen Skripts                */
int re15_discard_ready(void);         /* Text fertig getippt -> Yes/No waehlbar         */
uint8_t re15_discard_blink(void);     /* Blink-Zaehler DAT_800b8525 (@0x800285e8)       */

/* Raumwechsel / Spielstand laden: eine offene Abfrage verwerfen und den Zaehler
 * zuruecksetzen, damit kein Slot mit Anzahl 0 zurueckbleibt. */
void re15_discard_reset(void);

/* Pruefstand: wie oft wurde bisher gefragt / weggeworfen (Riegel-Messgroessen). */
int re15_discard_gefragt(void);
int re15_discard_weggeworfen(void);

#endif /* RE15_ITEM_DISCARD_H */
