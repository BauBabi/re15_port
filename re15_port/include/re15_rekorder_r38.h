#ifndef RE15_REKORDER_R38_H
#define RE15_REKORDER_R38_H

/* ============================================================================
 * Runde 38 Spur J "rekorder" — das Stimmschloss (VUS) ROOM1100/ROOM1101 mit dem Minidisc Player
 * Dossier: analysis/befunde_runde38/J_rekorder.md (Messung vorher/nachher, Bilder, Belege).
 * ============================================================================
 * Nutzer (AUFTRAG.md Runde 38, woertlich): "Nutze beim Rekorder Rätsel beim reinsprechen den Sound
 * Irons_Rekorder_Sound.wav aus dem synchro Ordner, und nachdem er fertig abgespielt ist, machst du den Piep Sound für
 * Erfolgreiche Eingabe und den Tür öffnen Sound. Das zusammen mit der erfolgreich message auf den Panel."
 *
 * ---- ORIGINAL (RE1.5, ROOM1100.RDT == ROOM1101.RDT bytegleich, selbst gelesen) ------------------------------------
 *   sub01 @0x00C2C Ck(5,5,1) Work_set(3,0) Member_cmp(0,0x0F,5) Sce_key_ck(1,0x40) -> @0x00C4A Evt_exec sub02
 *         (Cursor auf dem Mikrofon-Gitter, Viereck = "reinsprechen"; vorher sub08 @0x00D86 Cut 7 SYSTEM READY,
 *         @0x00D90 Cut 8 SAY HELLO)
 *   sub02 @0x00C90 `2b 04 ff ff` Message_on 4 "You've used the Minidisc Player w/ Disc." (Pausemaske 0xffff)
 *         @0x00C94 `02 00`       Evt_next + Nop (VM steht, bis die Nachricht zu ist)
 *         @0x00C96 `29 0c`       Cut_chg 12 (Anzeige leer)  + @0x00C98 `09 0a 05 00` Sleep 5
 *         @0x00C9C `29 0b`       Cut_chg 11 ("ACCEPTED", erstes Mal) + Sleep 5; blinkt 12/11 bis @0x00CB4
 *         @0x00CBA `2b 02 ff ff` Message_on 2 "You've opened the lock."  @0x00CC4 Cut 6 / @0x00CC6 Cut_auto 1
 *   RE1.5 spielt hier keinen Ton (kein Se_on in sub02/sub07/sub08). Seit Runde 36 Spur G haengt die RE2-Folge
 *   KARTE_AUF (Piep +0 / Aufschliessen +15) an Message_on 2 (gen/r36_aufschliessen.inc, Stellen 1100/2 und 1101/2).
 *
 * ---- FORM (RE2 ROOM2110 sub08, selbst gelesen; Mini-Bank KARTSE.VBS seit Runde 36) -----------------------------
 *   @0x01B5C `36 02 0d` Se_on(2,0x0D) PIEP, @0x01B68 `65 01 65 02` + @0x01B6C/@0x01B7C `64 0n 16 08` Anzeige "offen" im
 *   SELBEN Skript-Takt, @0x01B8C `09 0a 0f 00` Sleep 15, @0x01B90 `36 02 0b` Se_on(2,0x0B) AUFSCHLIESSEN.
 *   Saetze: KARTSE.VBS Satz 1 PIEP = RE2 ROOM2110 Satz 0x0D, Satz 2 AUF_E = Satz 0x0B (gen/re2_karten_bank.inc) =
 *   dieselben Saetze wie Kartenleser und Code-Panels (aufschliessen_r36.c k_folgen[KARTE_AUF]).
 *
 * ---- UMSETZUNG (NUTZER-VORGABE in der belegten Form) ----------------------------------------------------------------
 *   (1) "beim reinsprechen": im Takt von Message_on 4 (Anker @0x00C94 = erster Opcode danach, selber Takt) startet die
 *       Aufnahme des Nutzers auf dem Stimmen-Kanal (Port-Stimmen-Id 40, VERTRAG §1.2 Spur J 1100/1101 Ids 40-44;
 *       Datei synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav, auch fuer ROOM1101).
 *   (2) "nachdem er fertig abgespielt ist": sub02 wartet an @0x00C96 (erster Opcode nach dem Schliessen der Nachricht),
 *       solange die Plattform die Aufnahme als laufend meldet (g_re15_voice_laeuft && g_re15_voice_restbilder > 0, dieselbe
 *       echte Restlaenge wie der Stimmen-Riegel in scd_vm.c op_message_on).
 *   (3) "Piep ... und den Tür öffnen Sound ... zusammen mit der erfolgreich message auf den Panel": am ERSTEN
 *       Cut_chg 11 "ACCEPTED" (@0x00C9C) startet die Runde-36-Folge KARTE_AUF der Stelle (Raum, 2): Piep im selben
 *       Skript-Takt wie die Anzeige (RE2 @0x01B5C/@0x01B6C), Aufschliessen 15 Bilder spaeter (RE2 @0x01B8C/@0x01B90).
 *       Message_on 2 (@0x00CBA) spielt die Folge dann nicht noch einmal (re15_rekorder_r38_message_on).
 *   Alle Original-Opcodes laufen unveraendert und in Original-Reihenfolge; eingefuegt ist nur das Warten an @0x00C96.
 * ==========================================================================*/

#include <stddef.h>
#include <stdint.h>

#define RE15_REKORDER_STIMME_ID  40                          /* VERTRAG §1.2: Spur J, ROOM1100/1101 Ids 40-44 */
#define RE15_REKORDER_ORDNER     "synchro/STAGE1/room1100/"  /* Nutzer-Datei, nicht umbenannt/verschoben */
#define RE15_REKORDER_STAMM      "Irons_Rekorder_Sound"

/* Anker (Datei-Offsets in ROOM1100.RDT == ROOM1101.RDT, sub02). */
#define RE15_REK_MSG4       0x00C90u   /* `2b 04 ff ff` Message_on 4 */
#define RE15_REK_START      0x00C94u   /* `02` Evt_next direkt dahinter (selber Takt wie Message_on 4) */
#define RE15_REK_WARTE      0x00C96u   /* `29 0c` Cut_chg 12, erster Opcode nach dem Schliessen von msg 4 */
#define RE15_REK_ACCEPTED   0x00C9Cu   /* `29 0b` Cut_chg 11, erstes ACCEPTED */
#define RE15_REK_MSG2       0x00CBAu   /* `2b 02 ff ff` Message_on 2 "You've opened the lock." */
#define RE15_REK_SIG_LAENGE 56         /* Signatur @0x00C90..0x00CC7 (sub02 ab Message_on 4 bis Cut_auto 1) */
#define RE15_REK_ACCEPTED_CUT 11       /* `29 0b` */

/* Zustaende (Messschiene). */
#define RE15_REK_AUS     0   /* kein Anker (anderer Raum oder Signatur weicht ab) */
#define RE15_REK_BEREIT  1   /* armiert, sub02 noch nicht bei Message_on 4 */
#define RE15_REK_STIMME  2   /* Aufnahme angefordert, sub02 wartet an @0x00C96 */
#define RE15_REK_FREI    3   /* Aufnahme zu Ende, sub02 laeuft zum ersten ACCEPTED */
#define RE15_REK_PIEP    4   /* Piep/Aufschliessen am ACCEPTED gestartet, Message_on 2 noch offen */
#define RE15_REK_ENDE    5   /* Message_on 2 erreicht (Folge dort nicht wiederholt) */

/* Raumladen (scd_register_current_rdt): Anker im rohen RDT armieren, Zustand zuruecksetzen. */
void re15_rekorder_r38_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id);
extern int g_re15_rekorder_anker_n;   /* 0 ausser in ROOM1100/1101 (Wachposten im Dispatcher) */

/* Dispatcher-Haken (scd_vm.c, vor dem Opcode): 0 = Opcode normal ausfuehren, 2 = warten (Faden gibt das Bild ab, pc
 * bleibt). Der Parameter ist ein scd_thread_t *. */
int  re15_rekorder_r38_pc(void *thread);

/* Message_on-Haken (scd_vm.c op_message_on, an der Stelle des Runde-36-Hakens). pc = Opcode-Adresse im rohen RDT.
 * Rueckgabe 1 = die Aufschliess-Folge dieser Nachricht ist schon gespielt (am ACCEPTED) -> Runde-36-Haken auslassen. */
int  re15_rekorder_r38_message_on(unsigned room_id, const unsigned char *pc);

/* Je Spielbild (game_step_common.c, neben re15_aufschliessen_r36_tick; laeuft auch bei offener Nachricht): zaehlt die
 * Bilder seit der Anforderung und schreibt das Ende der Aufnahme ins Protokoll. */
void re15_rekorder_r38_tick(void);

/* Plattform-Lader (audio_pc.c re15_voice_load_clip): fuer (ROOM1100|ROOM1101, Id 40) Ordner und Dateistamm der
 * Nutzer-Datei einsetzen. Rueckgabe 1 = eingesetzt. */
int  re15_rekorder_r38_stimme(unsigned room_id, int voice_id, char *ordner, size_t n_ordner, char *stamm, size_t n_stamm);
/* Zensus (unit_synchro_stimme): 1 = die Datei <name> im Ordner room<rid> ist diese Bindung. */
int  re15_rekorder_r38_ist_datei(unsigned rid, const char *name);

/* Messschiene. */
int      re15_rekorder_r38_zustand(void);
int      re15_rekorder_r38_wartebilder(void);     /* Takte, die sub02 an @0x00C96 gewartet hat */
unsigned re15_rekorder_r38_anforderungen(void);   /* Zahl der Anforderungen der Aufnahme seit Start */
unsigned re15_rekorder_r38_pieps(void);           /* Zahl der am ACCEPTED gestarteten Folgen seit Start */
unsigned re15_rekorder_r38_unterdrueckt(void);    /* Zahl der an Message_on 2 ausgelassenen Runde-36-Folgen */
int      re15_rekorder_r38_bilder_seit_start(void);

#endif /* RE15_REKORDER_R38_H */
