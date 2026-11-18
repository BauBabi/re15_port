/*
 * RE1.5 Rebuilt — Runde 42 Strang "gegenstaende", Stufe 2: Rechner-Vollbilder beim Benutzen (305 Cut 10, 505 Cut 5, 2080 Bild 9).
 *
 * NUTZERWUNSCH 17.11. (Frage 2 der Hintergrund-Auswertung, "Rechner-Bildschirm als Vollbild (305, 2080, 505)?"): "Ja, einbauen".
 * Vorgaben: nur Original-Befehle, RDT unveraendert, Bilder ohne Kamera brauchen einen Standbild-Weg
 * (VORSCHLAEGE.md B2/B3/B4, PRUEFUNG.md Abschnitt 1 Zeilen 7/16, Memory reai-v2-unbenutzte-hintergruende).
 *
 * ---- Die Bilder (Mod-Baum, selbst nachgelesen) ----------------------------------------------------------------------------
 * 305 Bild 10 (ROOM305.BSS +0xA0000): Bildschirm (Desktop) als Vollbild. Kamera 10 @RDT 0x1A0 und Kamera 15 @0x240 haben dieselbe
 *     Lage (Bytes +0x02..+0x1B gleich, selbst verglichen); verschieden sind nur Byte +0x00 (00/01) und der Masken-Zeiger +0x1C
 *     (0x0838/0x084C). Kein Cut_chg 10 in 3050/3051 (nur 2/7/9/15), RVD nur Anker @0x49C.
 * 505 Bild 5 (ROOM505.BSS +0x50000): derselbe Bildschirm-Aufbau. Kamera 5 @0x100 / Kamera 4 @0xE0: Lage gleich, Byte +0x00 00/01,
 *     Masken-Zeiger 0x031C/0x0320; nur `29 04` @0x00880.
 * 208 Bild 9 (ROOM208.BSS +0x90000, Datei 0xA0000 = 10 Bilder): derselbe Bildschirm-Aufbau, OHNE Kamera (ROOM2080/2081 Byte 1
 *     nCut = 9). Ein Cut_chg 9 liefe im Original ueber das Tabellenende (FUN_80014324 sucht den Anker ohne Endpruefung).
 *
 * ---- Wo (PORT-WAHL, nur Original-Saetze, nur ein neuer Satz `29 nn` je Stelle) ----------------------------------------------
 * 3050/3051 sub15 @0x025E4..@0x0262F (76 B, beide Fassungen bytegleich), gestartet von sub01 @0x0221C `04 ff 18 0f` nach dem
 *     Kran-Steuerspiel: msg 5 "You've used the Red Master Keycard." @0x02608, Set(3,0xB3)/(3,0xB0), msg 6 "The system has been
 *     successfully restored." @0x02616, Slot 10 -> msg 7, Cut_chg 2 @0x0262A, Cut_auto 1. NEU: `29 0a` vor @0x02616 - der
 *     Bildschirm steht, waehrend das System als wiederhergestellt gemeldet wird; danach Cut 2 wie bisher.
 * 5050/5051 sub02 @0x0086E..@0x008B3 (70 B, bytegleich), Ereignis 2 (sub00 @0x0076C Slot 1 sce 3 `ff 00 18 02`): Frage msg 0, Ja ->
 *     Set(2,7), Cut_chg 4 @0x00880 (Rechner nah), msg 1 "Culture Experiment Room Staff Registry." @0x00882, msg 2 "Commencing with
 *     temporary registration." @0x00888, msg 3 "Register your fingerprint." @0x0088E, dann das Cursor-Spiel auf Cut 4 (Bank 5
 *     Bits 0..5 @0x00898..@0x008AC, sub01). NEU: `29 05` vor msg 2 @0x00888 (msg 1 bleibt beim Rechner Cut 4, der Bildschirm
 *     steht zu "Commencing with temporary registration.") und `29 04` vor msg 3 @0x0088E (das Spiel laeuft wie im Original auf
 *     Cut 4); Ifel-Laenge @0x00876 `3a` -> `3e`. (Ein Satz direkt hinter @0x00880 wuerde Cut 4 im selben Bild ueberschreiben -
 *     gemessen im Riegel r505: Folge 0 -> 5 ohne 4.)
 * 5050/5051 sub03 @0x008B4..@0x008FF (76 B, bytegleich), von sub01 @0x00850 `04 ff 18 03`: msg 5 "Registration complete."
 *     @0x008E0, msg 6 "Guest registration is valid for 24 hours." @0x008E6, Cut_chg 3 @0x008F0. NEU: `29 05` vor @0x008E0.
 * 2080/2081 sub02 (Ereignis 2, Wasser ablassen; im Port das Programm aus story2_r42.c ablass_bauen = Original @0x00C86.. + Hinweis):
 *     NEU: `29 09` vor dem Sleep 20 @0x00CBE `09 0a 14 00` - die Pause vor dem Wasser-Schnitt Cut 7 @0x00CC2 zeigt den
 *     Bildschirm. Dauer = die des Original-Sleeps (20 Bilder), keine neue Zahl.
 *
 * ---- Standbild-Weg (Bild ohne Kamera) ----------------------------------------------------------------------------------------
 * Ein Cut_chg auf eine Nummer >= nCut, die in k_standbilder steht, wechselt NICHT die Kamera (work_vars[0x0A], cam_id bleiben),
 * sondern zeigt Bild n des Raums als Vollbild ohne Figuren, Schatten und Masken (PC: bg_pc.c laedt Scheibe n, main.c verwirft die
 * Szenen-Ebenen wie beim Inventar, re15_render_pc_clear_scene_overlays). Wie im Original-Handler @0x800402d4 schaltet er die
 * Zonen-Automatik aus. Der naechste Cut_chg oder ein Raumwechsel beendet das Standbild.
 */
#ifndef RE15_RECHNERBILDER_R42_H
#define RE15_RECHNERBILDER_R42_H

#include <stdint.h>

#define RE15_RB42_RAUM_305     0x3050u
#define RE15_RB42_RAUM_505     0x5050u
#define RE15_RB42_RAUM_208     0x2080u
#define RE15_RB42_CUT_305      10      /* ROOM3050 Kamera 10 @0x1A0                                     */
#define RE15_RB42_CUT_505      5       /* ROOM5050 Kamera 5 @0x100                                      */
#define RE15_RB42_BILD_208     9       /* ROOM208.BSS +0x90000, ohne Kamera (nCut 9)                    */
#define RE15_RB42_SUB_305      15      /* sub15 @0x025E4                                                */
#define RE15_RB42_SUB_505_EIN  2       /* sub02 @0x0086E = Ereignis 2                                   */
#define RE15_RB42_SUB_505_AUS  3       /* sub03 @0x008B4                                                */

/* scd_vm.c op_cut_chg: 1 = als Standbild uebernommen (Kamera bleibt), 0 = normaler Cut_chg (beendet ein laufendes Standbild). */
int re15_rb42_cut_chg(uint8_t cut);
/* Laufendes Standbild (Bild-Nummer im BSS des Raums) oder -1. */
int re15_rb42_standbild(void);
/* scd_vm.c: Ereignis-/Evt_exec-Haken (NULL = ausgelieferter sub). */
const uint8_t *re15_rb42_ereignis(uint16_t room_id, uint8_t event_id);
const uint8_t *re15_rb42_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *pc);
/* story2_r42.c ablass_bauen: Satz `29 09`, der vor dem Sleep @0x00CBE eingefuegt wird (2 Byte). */
extern const uint8_t re15_rb42_satz_208[2];
/* Pruefhaken (Riegel): Programm 0 = 3050 sub15, 1 = 5050 sub02, 2 = 5050 sub03 (fuer den laufenden Raum gebaut). */
const uint8_t *re15_rb42_programm(int welches, int *out_len);
/* Zaehler fuer Tests: [0] Standbild an, [1] Standbild aus, [2] Programme 305, [3] Programme 505 */
extern unsigned g_re15_rb42_zaehler[4];

#endif /* RE15_RECHNERBILDER_R42_H */
