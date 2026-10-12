#ifndef RE15_CODEPANEL_R37_H
#define RE15_CODEPANEL_R37_H

/* ============================================================================
 * Runde 37 Spur E "codepanel" — die drei Ziffern-Code-Panels (ROOM10D0/10D1, 11E0/11E1, 1230/1231)
 * Dossier: analysis/befunde_runde37/E_codepanel.md (Zensus, Messung, Belege, Bilder).
 * ============================================================================
 * Nutzer (AUFTRAG.md #9, woertlich): "bei den Code panels möchte ich das, so lange die Code noch falsch
 * eingegeben ist unten das blaue Licht leuchtet. Ist er dann richtig eingegeben das grüne Licht. Die Kamera
 * darf noch auf den Panels bleiben wenn der Piep und unlock sound kommt."
 *
 * ---- E2 KAMERA (RE2- und RE1.5-belegt) ---------------------------------------------------------------
 * Alle sechs Raeume tragen in sub19 (Aufschliessen, gestartet von sub01 @0x01526/@0x01510/@0x01A86/@0x00FEE)
 * denselben Schwanz, Datei-Offset X = 10D0 @0x0199A, 10D1 @0x01984, 11E0/11E1 @0x01F82, 1230/1231 @0x01462:
 *     X+0 `2a` Cut_old | X+1 `00` | X+2 `3c 01` Cut_auto 1 | X+4 `2b 05 ff ff` Message_on 5 "You've opened the
 *     lock." | X+8 `02` Evt_next | X+9 `00` | X+10 `22 01 1b 00` Set(1,27,0) | X+14 `22 02 07 00` Set(2,7,0) | X+18 Evt_end
 * Die Kamera geht also VOR der Nachricht zurueck — und an Message_on 5 haengt seit Runde 36 der RE2-Piep (+0) und das
 * Aufschliessen (+15) (aufschliessen_r36.c). Gemessen (Dossier §1): Cut_old im Bild des Pieps (F231), Raumansicht ab
 * F232, Aufschliess-Ton F246 in der Raumansicht.
 * Vorbilder fuer "Kamera bleibt":
 *   RE2 ROOM2110 sub08: @0x01B2C Cut_chg 12 (Leser) .. @0x01B5C Piep .. @0x01B90 Aufschliessen, ERST DANN
 *     @0x01B9C `29 09` Cut_chg 9 / @0x01B9E `3c 01` Cut_auto 1.
 *   RE1.5 ROOM1100 sub02 (das eigene Schloss-Panel): @0x00CBA Message_on 2 "You've opened the lock." /
 *     @0x00CBE Evt_next / @0x00CC0 Set(2,7,0) / @0x00CC4 Cut_chg 6 / @0x00CC6 Cut_auto 1 — Kamera erst NACH der Nachricht.
 * Umsetzung: X..X+3 (Cut_old, Nop, Cut_auto 1) werden uebersprungen und erst an X+10 (nach dem Schliessen der
 * Nachricht) mit den ORIGINAL-Handlern op_cut_old (RE1.5 0x8004032c) / op_cut_auto (0x800403ac) auf den
 * Original-Bytes ausgefuehrt — und nicht, bevor die Aufschliess-Folge der Nachricht (Piep + Aufschliessen,
 * re15_aufschliessen_r36_folge) gespielt ist; bis dahin wartet sub19 an X+10. Flags, Platz-Umbau, Tuer und
 * Nachricht bleiben unveraendert (Reihenfolge wie RE1.5 1100: Nachricht, dann Kamera).
 *
 * ---- E1 LAMPEN (Nutzer + Nachtraege 3/3b, AUFTRAG.md; Mechanik und Farbzeilen RE2) ------------------------
 * Nutzer, Nachtrag 3: "beim Panel in korridor 2 ... dort soll natürlich rot leuchten - richtig eingegeben - dann blau",
 *   "rot = geschlossen". Nachtrag 3b: "hintergrund rot - rot leuchten - hintergrund blau - blau leuchten".
 *   -> Je Panel leuchtet die GEMALTE Lampe in IHRER gemalten Farbe: solange zu (Code falsch / noch nicht eingegeben) die
 *      ROT gemalte, sobald der Code richtig ist die andere gemalte (10D0: BLAU, SECUGUARD: GRUEN; die gelbe bleibt dunkel).
 *   "Korridor 2" = ROOM10D0 "2F CORRIDOR" (include/debug_jump_table.h Stage 0 Index 13 = 0x0D); dort sind rot + blau
 *   gemalt. ROOM11E0 "B2 CORRIDOR" (Index 30 = 0x1E) traegt die SECUGUARD-Kunst rot/gelb/gruen.
 * RE1.5 hat fuer die Code-Panels keine Leuchtkunst (gemalte Lampen dunkel, kein Zusatz-Cut, kein ESP) -> RE2:
 *   ROOM4070 (Schloss "Clearance identification device", zwei RUNDE Lampen, links ROT gemalt, rechts BLAU gemalt):
 *     sub03 zu @0x008C6 `64 02 16 04` (Unterindex 0x04 = Strom 4, Zeile 0 ROT auf der roten Lampe), offen @0x0088C
 *     `64 03 16 0c` (0x0C = Strom 4, Zeile 1 BLAU auf der blauen Lampe); sub08 beim Oeffnen @0x00DC2 blau an,
 *     @0x00DD2 `65 02` rot aus, @0x00DD4 Se_on(2,0x0C) Piep — im SELBEN Skript-Takt. = genau der Nutzer-Wunsch fuer 10D0.
 *   ROOM2130 sub04 (Waffenkammer-Schloss, Cut 8): @0x01774/@0x01784 `64 0b/0c 16 00` ROT, @0x01798 Se_on(2,0x0F) Piep,
 *     @0x017A4/@0x017A6 `65 0b/0c` rot aus, @0x017A8/@0x017B8 `64 0d/0e 16 10` GRUEN — rot (zu) -> gruen (offen) im
 *     Piep-Takt, Kamera erst @0x017CC (Sleep 30 @0x017C8) zurueck.
 * Kunst = RE2 ESP 0x16 = shared_assets/RE2/LAMPE2130.TIM (bytegleich room2110/esp16.tim): CLUT-Zeile 0 ROT, 1 BLAU,
 *   2 GRUEN (Unterindex>>3, Spawner @0x8001c9e0..0x8001c9fc). Zellen/Anim-Saetze aus ROOM2130 effect.esp (Saetze
 *   @0x10.., Zellen @0x70..; == ROOM2110 effect.esp @0xC04.. / @0xC64..): rund = Strom 4, Saetze 7..11 (Zelle 5 S 0x18,
 *   7, 6, 7, Schleife); Quadrat = Strom 2, Saetze 4..6 (Zelle 3, 4, Schleife).
 * Farbton gemessen (Dossier §3, HSV ueber das gemalte Glas): 10D0 rot gemalt 14 Grad -> leuchtend 1 Grad, 10D0 blau
 *   212 -> 220, SECUGUARD rot 2 -> 1, gruen 121 -> 120; RE2-Zeilen 0/1/2 = 356..0 / 225..231 / 120 Grad.
 * Form folgt der gemalten Lampe: 10D0 rund (RE2 4070), SECUGUARD eckig 14 x 18 -> Quadrat-Zelle (Strom 2, RE2 2130
 *   Schalterlampe @0x01294 `64 01 16 02`, dort ROT; die gruene Quadrat-Lampe zeigt RE2 nicht -> Kombination wie die
 *   Generator-Lampe, re15_panel_zeiger.h).
 * Zustand: offen, sobald das Schloss-Flag steht (sub01 Set(3,F,1) im selben Takt wie Evt_exec sub19 = Piep-Takt),
 *   sonst zu. Gezeichnet nur im Panel-Cut (sub17 Cut_chg) des Raums, in der Hintergrund-Ebene vor dem VM-Takt
 *   (main.c, wie Zeiger und Generator-Lampen) -> sichtbar ab dem Bild nach dem Piep-Takt.
 * ==========================================================================*/

#include <stdint.h>

#define RE15_CP_RAEUME 6

#define RE15_CP_FORM_RUND     1
#define RE15_CP_FORM_QUADRAT  2

/* Farbzeilen der ESP-TIM (Unterindex >> 3, @0x8001c9e0..0x8001c9fc): 0 = rot, 1 = blau, 2 = gruen. */
#define RE15_CP_ZEILE_ROT     0
#define RE15_CP_ZEILE_BLAU    1
#define RE15_CP_ZEILE_GRUEN   2

/* Laenge des sub19-Schwanzes ab X und die zwei Stellen darin. */
#define RE15_CP_SIG_LAENGE    18
#define RE15_CP_X_NACH_MSG    10   /* X+10 = Set(1,27,0) hinter Evt_next/Nop */
#define RE15_CP_X_UEBER       4    /* Cut_old + Nop + Cut_auto 1 */

typedef struct {
    uint16_t raum;
    uint8_t  cut;          /* Panel-Cut (sub17 Cut_chg) */
    uint8_t  flag_bit;     /* Schloss-Flag Bank 3 (sub01 Set(3,F,1)) */
    uint32_t sub19_x;      /* Datei-Offset X (Cut_old in sub19) */
    uint8_t  form;         /* RE15_CP_FORM_* */
    /* Lampenmitte in HALBEN Pixeln (Glasmitte gemessen) und Farbzeile = gemalte Farbe. */
    int16_t  zu_x2, zu_y2;   uint8_t zu_zeile;    /* geschlossen: die ROT gemalte Lampe */
    int16_t  auf_x2, auf_y2; uint8_t auf_zeile;   /* offen: die andere gemalte Lampe (blau / gruen) */
} re15_cp_panel_t;

/* Was der Zeichner in diesem Bild malt. */
typedef struct {
    int x0, y0;    /* Ecke (Pixel, 320x240) */
    int kante;     /* Kantenlaenge (Pixel) */
    int zelle;     /* ESP-Zelle 3/4 (Quadrat) bzw. 5/6/7 (rund) */
    int s;         /* Zellgroesse in Texeln (24 fuer Zelle 5, sonst 32) */
    int zeile;     /* CLUT-Zeile RE15_CP_ZEILE_* (= gemalte Farbe der leuchtenden Lampe) */
    int offen;     /* 0 = geschlossen (rote Lampe), 1 = offen */
} re15_cp_lampe_t;

/* Tabelle (Riegel, Sonden). */
const re15_cp_panel_t *re15_codepanel_r37_panel(int i);
/* Panel des Raums oder NULL. */
const re15_cp_panel_t *re15_codepanel_r37_raum(unsigned room_id);

/* Raumladen (scd_register_current_rdt): Anker X im rohen RDT armieren, Zustand zuruecksetzen. */
void re15_codepanel_r37_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id);
extern int g_re15_codepanel_anker_n;   /* 0 ausser in den sechs Raeumen (Wachposten im Dispatcher) */

/* Dispatcher-Haken (scd_vm.c, vor dem Opcode): 0 = Opcode normal ausfuehren, 1 = behandelt (pc versetzt,
 * weiter), 2 = warten (Faden gibt das Bild ab, pc bleibt). Der Parameter ist ein scd_thread_t *. */
int  re15_codepanel_r37_pc(void *thread);

/* Lampe dieses Bilds: 1 = zeichnen. bild = fortlaufender Bildzaehler (Animationstakt, RE2 je Bild eine Zelle),
 * cut_gezeigt = angezeigter Cut. */
int  re15_codepanel_r37_lampe(unsigned bild, int cut_gezeigt, re15_cp_lampe_t *out);

/* Messschiene. */
int  re15_codepanel_r37_haelt(void);          /* 1 = Kamera wird gerade auf dem Panel gehalten */
unsigned re15_codepanel_r37_freigaben(void);  /* Zahl der Freigaben (Kamera zurueck) seit Start */
int  re15_codepanel_r37_wartebilder(void);    /* Bilder, die sub19 an X+10 gewartet hat (letzte Freigabe) */

#endif /* RE15_CODEPANEL_R37_H */
