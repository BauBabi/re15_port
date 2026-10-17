/*
 * RE1.5 Rebuilt — Runde 38 Spur Q "kleinkram" (Dossier analysis/befunde_runde38/Q_kleinkram.md).
 *
 * Nutzer (AUFTRAG.md Runde 38, woertlich):
 *   P1 "Das öffnen des Player Inventories hat keinen Sounds, wenn Resident Evil 2 dort Sound hat, möchte ich auch
 *      Sound haben."
 *   P2 "In Elzas Szenario bei ROOM 1151 kommt - wenn man Irons anklickt erst - he already passed - und dann beugt sie
 *      sich hin und es kommt "I'll be fine, just worry About yourself for now". Ist natürlich quatsch … sie soll sich
 *      nur hinbeugen, und es soll stehen "he already passed"..."
 *   P3 "In Room 30E0 soll beim aufnehmen des Flammenwerfers nicht mehr kommen - not available in the preview, sondern
 *      einfach der Flammenwerfer aufgenommen werden."
 *
 * ---- P1 INVENTAR-TON (RE2 Retail = Ziel; RE1.5 ist an beiden Stellen stumm) ---------------------------------------
 * RE2 PSX.EXE (info/re2leon), Spielschleife, Annahme der Status-Anforderung:
 *   800264a0  andi v0,v0,0x4000        ; Statustaste (Flanke 0x800ce310)
 *   800264c8  sw   v0,0x800cfbd8       ; |= 0x8000 Anforderung
 *   800264dc  andi v0,v1,0x8000 / 800264e8 sw (|= 0x40)   ; angenommen
 *   80026518  lbu  v1,0x800d5c00       ; Ziel-Schirm
 *   80026528  beq  v1,2,..             ; Schirm 2 (Aufnahme-Ansicht) ohne Ton
 *   8002652c  lui  a0,0x406            ; Se_on(Bank 4 = CORE, Satz 6)
 *   80026530  jal  0x8005ba28          ; Se_on
 *   80026534  addu a1,zero,zero        ; ohne Position (UI)
 * Wege, deren Handler 0x800df348 selbst setzen (`sb ..,-3256(at)`: Item-Aufnahme @0x80051904, Box @0x80051de0,
 * Status aus der Zone @0x8005201c, Kartenhinweis @0x800591e8), sehen das Byte @0x800263fc und springen @0x80026404
 * `bne v0,zero,0x80026540` ueber den ganzen Block ->
 * KEIN Oeffnen-Ton. Im Port: nur re15_menu_start_poll (START), nicht Box/Dokument/Kartenhinweis.
 * Schliessen (RE2 Statusschirm-Kommandoleiste 0x8006a7f0, Zustand 0 = Abblenden @0x80068f08):
 *   8006a928  andi 0x2000 (Abbruch) -> 8006a930 lui a0,0x405 / 8006a934 jal 0x8005ba28 / 8006a93c sb zero,1(s0)
 *   8006a94c  andi 0x4000 (Statustaste) -> 8006a968 lui a0,0x405 / 8006a96c jal 0x8005ba28 / 8006a978 sb zero
 *   8006a8cc  Cursor == 3 (EXIT) -> 8006a8d4 lui a0,0x405 (Verzoegerungsplatz) / 8006a910 sb zero,1(s0) / 8006a918 jal
 * -> Satz 5 auf jedem Schliess-Weg. CORE00.EDH ist zwischen RE1.5 und RE2-Leon bytegleich (3176 B, Satz 5
 * `00 00 63 01`, Satz 6 `00 00 73 01`), der Port spielt Bank 4 ueber re15_audio_core_se. */
#ifndef RE15_KLEINKRAM_R38_H
#define RE15_KLEINKRAM_R38_H

#include <stdint.h>

#define RE15_R38Q_INV_TON_AUF   6   /* RE2 @0x8002652c `lui a0,0x406` + @0x80026530 `jal 0x8005ba28` */
#define RE15_R38Q_INV_TON_ZU    5   /* RE2 @0x8006a930/@0x8006a968/@0x8006a8d4 `lui a0,0x405` */

/* menu_common.c ruft dies an den drei Stellen (Oeffnen-Annahme, Abbruch/START in der Reiterwahl, EXIT). Spielt
 * re15_audio_core_se(satz) und schreibt die Messzeile "[r38q] F<n> Inventar-Ton Satz <s> (<wo>)" (stderr = debug.log). */
void re15_kleinkram_r38_inv_ton(int satz, const char *wo);
int  re15_kleinkram_r38_inv_ton_zaehler(int satz);          /* Tests: wie oft Satz 5/6 gespielt */

/* ---- P2 ROOM1151 (Elza) Irons ------------------------------------------------------------------------------------
 * Ursache des Befunds (Dossier Abschnitt 2): Runde 36 (3eaf631d) baute Slot 2 auf einen Text-Platz (sce 1, Text 20)
 * um und nahm an, work_vars[0] werde dann nie 2. Falsch: der Vorwaerts-Treffer des Scans stempelt den Slot VOR dem
 * sce-Sprung fuer JEDE sce (RE1.5 FUN_80042bac @0x80042f34 `addiu v0,s2,-1` / @0x80042f3c `sh v0,0x800b0fd0`, Sprung
 * ueber die Handler-Tabelle erst @0x80042f48 ff.), und der Frame-Ende-Wisch (@0x8003ebf4 -> work_vars := -1) laeuft
 * nur mit der VM, die der Text mit Pausemaske 0xffff anhaelt (@0x8003f040-4c `g_pauseflags & 0x02000000` -> kein
 * VM-Lauf). Nach dem Text sieht sub01 @0x00E9E `Cmp(var00 == 2)` und startet sub03 (Hinbeugen + msg 2).
 * Umsetzung (NUTZER-VORGABE "nur hinbeugen ... 'he already passed'"):
 *   - Slot 2 bleibt beim ersten Mal der Original-Platz (sce 5) -> sub01 -> Set(3,0x9D) -> sub03 laeuft wie im
 *     Original: Pos/Dir @0x00ECE/@0x00ED6, Plc_neck @0x00EE2, Hinbeugen Plc_motion(1,0x0B) @0x00EEC, Cut 7 @0x00EF4.
 *   - @0x00EFA `2b 02 00 00` Message_on 2 -> Text 20 "He has already passed..." (Runde 36, re15_elza_r36_meldung),
 *     gleiche Maske 0x0000 (nicht blockierend, wie das Original).
 *   - Irons' Gesten @0x00F02/@0x00F0A `3f 00 06 00`, @0x00F12 `3f 00 03 00`, @0x00F16 `43 00 04 00` (Arbeits-Entitaet
 *     Work_set(2,0) @0x00EFE = Irons) entfallen: Irons bleibt in der Totenpose (Clip 2 Bild 89, Runde 36). Die Sleeps
 *     dazwischen (50/50/20) bleiben -> Elza bleibt gebeugt, waehrend der Text steht; danach Original (Blicke, Cut_old,
 *     Aufrichten @0x00F4E, Plc_ret @0x00F68).
 *   - @0x00F72 Evt_end von sub03: Slot 2 -> Text-Platz 20 (Runde-36-Umbau). Jedes spaetere Anklicken zeigt nur den
 *     Text; sub01 bleibt durch (3,0x9D) = 1 gesperrt (Gate @0x00EA4). elza_r36.c baut beim Raumstart nur noch um, wenn
 *     (3,0x9D) schon steht. */
#define RE15_R38Q_RAUM_1151        0x1151u
#define RE15_R38Q_1151_MSG_ORIG    2       /* @0x00EFA `2b 02 00 00` */
#define RE15_R38Q_1151_GESPRAECH_BANK 3    /* (3,0x9D): sub01 @0x00EA4 Ck / @0x00EA8 Set */
#define RE15_R38Q_1151_GESPRAECH_BIT  0x9D

/* ---- P3 ROOM30E0 Flammenwerfer ------------------------------------------------------------------------------------
 * Original: Fernansicht ueber das Panel (Slot 3 @0x00A5A, sub09 Cut 2), Cursor auf Zelle 4 (sub10 Cut 3), dann Zelle 5
 * + Viereck -> sub04: @0x00FE0 Aot_reset(3,0) (Panel weg), @0x00FEE `2b 0f ff ff` Message_on 15 "There's a
 * Flamethrower here.This weapon is not available in this preview." (@0x14D2), @0x00FFA `22 03 9c 01` Set(3,0x9C)
 * (main00 @0x00A56 `Ck(3,0x9C) == 0` baut Panel/Cursor nur davor auf). Einziger Waffen-Preview-Text aller 240 RDTs;
 * ROOM30E1 (Elza) hat keinen.
 * Port: an @0x00FEE startet statt des Texts die NORMALE Aufnahme (Item-Modal = FUN_8001db28-Port, armiert wie der
 * Item-Handler @0x80043328: Typ, Menge, kein Zonenbit, kein Weltmodell), Faden parkt bis zum Ende des Modals.
 *   Typ 0x0E = FLAMETHROWER (Waffen-Record @0x80074e50, Bild ITPS.ITP @0x2A000, Name re15_item_name 0x0e)
 *   Menge 100 = Magazin des Records @0x80074e50 `64 00 00 00`; Regel aus dem Bestand: alle 13 ausgelieferten
 *   Waffen-Item_aot_set (7 Typen) tragen Menge == Magazin (Zensus Dossier Abschnitt 2.3).
 *   Genommen ("Yes" + Platz): weiter wie das Original (Evt_next, Cut 2, Sleep 30, Set(3,0x9C), Cut 1, Cut_auto).
 *   Nicht genommen ("No" oder voll — derselbe Zweig @0x8001e054/@0x8001e06c im Modal): Set(3,0x9C) @0x00FFA entfaellt
 *   und Slot 3 wird wieder das Panel (Aot_set @0x00A5A: sce 3, Flags 0x31, Nutzlast `ff 00 18 09 00 00`), wie ein
 *   liegengebliebenes Item erneut erreichbar. */
#define RE15_R38Q_RAUM_30E0        0x30E0u
#define RE15_R38Q_FW_ITEM          0x0E    /* Waffen-Record @0x80074e50 */
#define RE15_R38Q_FW_MENGE         100     /* @0x80074e50 `64 00 00 00` */
#define RE15_R38Q_FW_PANEL_SLOT    3       /* main00 @0x00A5A `2c 03 03 31 ..` */
#define RE15_R38Q_FW_PANEL_SCE     3
#define RE15_R38Q_FW_PANEL_FLAGS   0x31
#define RE15_R38Q_FW_PANEL_P0      0x00FFu /* Nutzlast +0 `ff 00` */
#define RE15_R38Q_FW_PANEL_P1      0x0918u /* Nutzlast +2 `18 09` -> sub 9 = p1>>8 (@0x80043100) */
#define RE15_R38Q_FW_DONE_BANK     3       /* (3,0x9C): main00 @0x00A56, sub04 @0x00FFA */
#define RE15_R38Q_FW_DONE_BIT      0x9C

/* Raum-Scan (scd_register_current_rdt) und Opcode-Haken (scd_vm_tick, vor dem Opcode-Sprung).
 * Rueckgabe des Hakens: 0 = Opcode normal ausfuehren, 1 = PC versetzt, weiter, 2 = Faden parkt (Bild-Ende). */
extern int g_re15_r38q_anker_n;
void re15_kleinkram_r38_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id);
int  re15_kleinkram_r38_pc(void *thread);

/* Tests/Messung: Zaehler je Aktion und Ergebnis der letzten Flammenwerfer-Aufnahme. */
enum {
    RE15_R38Q_AKT_IRONS_TEXT = 1,   /* @0x00EFA Message_on 2 -> Text 20             */
    RE15_R38Q_AKT_IRONS_GESTE,      /* @0x00F02/0A/12/16 uebersprungen              */
    RE15_R38Q_AKT_IRONS_ENDE,       /* @0x00F72 Slot 2 -> Text-Platz                */
    RE15_R38Q_AKT_FW_MODAL,         /* @0x00FEE Modal gestartet                    */
    RE15_R38Q_AKT_FW_GENOMMEN,      /* Modal zu, Flammenwerfer im Inventar         */
    RE15_R38Q_AKT_FW_LIEGT,         /* Modal zu, nicht genommen -> Panel neu scharf */
    RE15_R38Q_AKT_FW_BIT_UEBER,     /* @0x00FFA Set(3,0x9C) uebersprungen           */
    RE15_R38Q_AKT_N
};
int  re15_kleinkram_r38_zaehler(int aktion);
void re15_kleinkram_r38_reset(void);

#endif /* RE15_KLEINKRAM_R38_H */
