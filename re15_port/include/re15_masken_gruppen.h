/*
 * RE1.5 Rebuilt — SICHTBARKEIT DER sprite.pri-MASKEN JE GRUPPE (Record-Tabelle DAT_800b2584)
 * und SCD-Opcode 0x45 "Col_chg_set".
 *
 * Runde 34 Nacht, Spur G2 (Nutzer: "in Irons Office room 1150 blinkt die Schrift eigentlich im
 * Hintergrund"). Dossier: analysis/befunde_runde34_nacht/G2_schrift1150.md (Mechanismus §3,
 * Bauplan §5, Umsetzung §9), Gegenpruefung G2_schrift1150.gegenpruefung.md.
 *
 * WAS DAS ORIGINAL TUT (PSX.EXE, selbst disassembliert mit re15_disasm.py):
 *
 *   Je Vordergrund-Maske eines Cuts gibt es einen 4-Byte-Record in der Tabelle DAT_800b2584
 *   (Pool aus FUN_80039270, Platz fuer RDT[7] Records):
 *       Byte0  Sichtbarkeit (gezeichnet nur bei Bit 0)
 *       Byte1  Gruppenindex + 1 (Gruppe = Kopf der sprite.pri-Sektion, Leer-Gruppen zaehlen mit)
 *       +2     Tiefe (Halbwort) — der Port fuehrt die Tiefe weiter im Maskenparser (re15_pri.h)
 *   Die Zahl der Records, die 0x45 und das Zeichnen sehen, ist RDT[0] (erstes Byte der RDT im RAM).
 *
 *   AUFBAU FUN_800392d4 (einziger Aufruf @0x80021c28 im Cut-Apply FUN_80021bbc):
 *       80039324  lw   v1,0(t5)          ; erstes u32 der Sektion
 *       80039328  addiu v0,zero,-1       ; FF FF FF FF = NULL-Sektion ...
 *       8003932c  bne  v1,v0,...
 *       80039338  sb   zero,0(a0)        ; ... -> RDT[0] := 0 (Byte0 wird dann NICHT geloescht)
 *       80039330  srl  t2,v1,16
 *       80039358  sb   t2,0(a0)          ; RDT[0] := deklarierte Maskenzahl & 0xFF
 *       8003936c  lbu  a3,7(v0)          ; RDT[7]
 *       80039374..84                     ; do { a3--; Byte0 := 0; } while (a3 & 0xFF)
 *       800393d8  lbu  v0,0(s5)
 *       800393e0  ori  v0,v0,0x1
 *       800393e4  sb   v0,0(s5)          ; je gebauter Maske Byte0 |= 1
 *       800393e8  addiu v0,a3,1
 *       800393ec  sb   v0,-1(t1)         ; Byte1 := Gruppenindex + 1
 *       80039494  addiu s5,s5,4          ; Record-Zeiger je Maske
 *       80039544  addiu a3,a3,1          ; Gruppenindex, auch fuer Leer-Gruppen (Sprung 800393c0)
 *   -> jeder Cut-Apply mit Dirty == 1 schaltet ALLE Masken des Cuts ein.
 *
 *   OPCODE 0x45 (Tabelle @0x800745bc -> 0x800428d4) -> FUN_800396a8 (einziger Aufrufer @0x800428f4):
 *       800428ec  lbu  a0,1(v0)          ; op1
 *       800428f0  lbu  a1,2(v0)          ; op2 = neuer Byte0-Wert (ganzes Byte)
 *       800428f8  addiu a0,a0,1          ; Schluessel = op1 + 1
 *       80042900  ori  v0,zero,0x1       ; Rueckgabe 1 = weiter im selben SCD-Takt
 *       80042904  addiu v1,v1,3          ; pc += 3
 *       800396b8  lbu  a3,0(v0)          ; Zahl = RDT[0]
 *       800396cc  andi a0,a0,0xff        ; Schluessel 8 Bit
 *       800396d0  lbu  v0,1(v1)          ; Byte1
 *       800396d8  bne  v0,a0,...
 *       800396e0  sb   a1,0(v1)          ; Byte1 == Schluessel -> Byte0 := op2
 *       800396e4  sltu v0,a2,a3          ; fuer alle i < RDT[0]
 *
 *   ZEICHNEN FUN_80039590 (je Bild @0x8001ce54, NACH dem SCD-Laeufer @0x8001cdec):
 *       800395c0  lbu  s4,0(v0)          ; Zahl = RDT[0]
 *       800395e8  lbu  v0,0(s3)
 *       800395f0  andi v0,v0,0x1
 *       800395f4  beq  v0,zero,...       ; Byte0 & 1 == 0 -> Maske NICHT zeichnen
 *
 * WANN NEU AUFGEBAUT WIRD (Dirty-Flag DAT_800b5457 == 1 -> FUN_80021bbc -> @0x80021c28):
 *   @0x8001d5c8 Laden/Sitzungsstart, @0x8001daec Raumlader, @0x80021514 Cut-Abweichung,
 *   @0x800402f4 Cut_chg, @0x80040354 Cut_old. Im Port bilden diese Stellen der Raumlader
 *   (room_common.c Schritt 9) und der re15_cam_present_tick()-Zweig des Praesentations-Applys
 *   ab (PC main.c pc_cam_present_apply, PSX main.c) — der Port fuehrt alle seine
 *   cam_change_pending-Setzer als Dirty-1-Aequivalente.
 *   ⛔ KEIN Neuaufbau nach Statusschirm (Inventar) und Kartenschirm: dort schreibt das Original
 *   Dirty := 2 (@0x800466fc, Wert @0x800466dc; @0x80026634, Wert @0x8002661c), und
 *   FUN_80021bbc springt bei 2 ueber den Aufbau hinweg (@0x80021bc4 lbu Dirty / @0x80021bc8
 *   ori v0,zero,0x2 / @0x80021bd4 beq -> 0x80021df8, vorbei an @0x80021bf4, @0x80021bfc und
 *   @0x80021c28). Der Port hat keinen Dirty-2-Weg und bekommt keinen: dort bleibt der Zustand
 *   stehen (Gegenpruefung Auflage 1).
 *
 * WO ES WIRKT (Zensus aller 240 RDTs, Dossier §7.1): ROOM1150/1151 Cut 2 (sub05 blinkt die
 * Leuchtschrift "HEAVEN" = Gruppen 6..11, je 20 SCD-Takte an/aus), ROOM1211 Cut 7, ROOM3000/3001,
 * ROOM3010/3011 (Zombie-Variante), ROOM3071 Cut 9 (Lichtfolge), ROOM5060/5061 Cut 11.
 */
#ifndef RE15_MASKEN_GRUPPEN_H
#define RE15_MASKEN_GRUPPEN_H

#include <stdint.h>

#include "re15_rdt.h"
#include "re15_scd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Hoechstzahl Records: RDT[7] ist ein u8 und die Loeschschleife @0x80039374-84 laeuft als
 * do-while ueber (a3 & 0xFF) — hoechstens 256 Records (RDT[7] == 0 -> 256 Durchlaeufe). */
#define RE15_MG_MAX 256

/* FUN_800392d4: die Record-Tabelle fuer (rdt, cut) neu aufbauen (Belege im Kopf).
 * rdt == NULL oder ungueltiger Cut/Offset -> Zahl 0 (wie eine NULL-Sektion).
 * `grund` erscheint nur in der Mess-Zeile (RE15_MG_LOG). */
void re15_mg_aufbauen(const re15_rdt_t *rdt, int cut, const char *grund);

/* FUN_800396a8: fuer alle i < Zahl mit Byte1 == g1 -> Byte0 := wert (ganzes Byte). */
void re15_mg_setzen(uint8_t g1, uint8_t wert);

/* FUN_80039590 @0x800395f0-f4: Maske i wird gezeichnet gdw. Byte0 & 1.
 * PORT-KONSTRUKTION (keine Original-Adresse): i >= Zahl -> 1. Solche Indizes gibt es nur fuer
 * nachgezeichnete R15M-Masken (re15_pri.h), die ausschliesslich bei einer NULL-Sektion
 * kommen — dort ist im Original RDT[0] = 0 (@0x80039338), der Opcode findet nichts, und die
 * Port-Masken bleiben unveraendert sichtbar. */
int re15_mg_sichtbar(int i);

/* Fuer Pins/Messung. */
int      re15_mg_zahl(void);          /* RDT[0]-Aequivalent */
uint8_t  re15_mg_byte0(int i);
uint8_t  re15_mg_byte1(int i);

/* SCD-Opcode 0x45 Col_chg_set (3 Byte), registriert in scd_vm.c. */
int op_col_chg_set(scd_thread_t *t);

#ifdef __cplusplus
}
#endif

#endif /* RE15_MASKEN_GRUPPEN_H */
