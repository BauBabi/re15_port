/*
 * RE1.5 Rebuilt — Runde 41 Spur J (Orchestrator-Entscheid A-E5): RBJ-Marker-Binder byte-true.
 *
 * Original FUN_8001b3f8 (PSX.EXE, Raumlader `jal 0x8001b3f8` @0x80039a08 nach FUN_8003ef6c):
 *   @0x8001b404 `lw a2,92(v0)`        RBJ = RDT-Kopf +0x5C (DAT_800ac778)
 *   @0x8001b410 `addu a3,zero,zero`   Gegner-Index a3 = 0 EINMAL vor allen Records
 *   @0x8001b420 `lbu t2,4(a2)`        Record-Zahl = Byte @+4; Trailer = RBJ + u32@0 (@0x8001b41c/24)
 *   @0x8001b438 `lw a0,0(v1)`         Marker-u32 am EMR-Praefix des Records
 *   @0x8001b440 `andi v0,a0,0x1`      Bit 0 -> Spieler-Kanal 0x800acbd4/0x800acbd8 (@0x8001b450/@0x8001b460);
 *                                     jeder Record mit Bit 0 schreibt -> der LETZTE gewinnt
 *   @0x8001b470 `andi v0,a0,0x1`      je Bitposition von (Marker >> 1):
 *   @0x8001b4a0 `sw t1,0(at)`         gesetztes Bit -> Gegner-Kanal 0x800acdac + (a3 & 0xff) * 0x1F4 (+0x180)
 *   @0x8001b4c4 `addiu a3,a3,1`       Delay-Slot von @0x8001b4c0: a3++ JE BITPOSITION (auch 0-Bits unter dem
 *                                     hoechsten gesetzten), nie je Record zurueckgesetzt -> kumulativ
 * Port-Aktor-Slot = 1 + Gegner-Index (scd_vm.c SCRIPT_SLOT_TO_ACTOR).
 */
#ifndef RE15_RBJ_BINDER_R41_H
#define RE15_RBJ_BINDER_R41_H

#include <stddef.h>
#include <stdint.h>

/* PORT-WAHL (Schutzgrenzen, kein Original-Verhalten): das Original prueft weder die Record-Zahl
 * (Zahl 0 liefe als do-while 256 Mal, @0x8001b4cc/d0 `addiu t2,t2,-1` / `andi v0,t2,0xff`) noch
 * den Gegner-Index (schreibt ungeprueft hinter die Entity-Tabelle). Ausgelieferte RDTs: hoechstens
 * 3 Records, Gegner-Index hoechstens 5 (rbj_binder_zensus.py). */
#define RE15_RBJ_BINDER_MAX_REC     8
#define RE15_RBJ_BINDER_MAX_GEGNER  32

typedef struct {
    int    spieler;                                /* Record des Spieler-Kanals, -1 = nicht geschrieben */
    int8_t gegner[RE15_RBJ_BINDER_MAX_GEGNER];     /* Record je Gegner-Index (Aktor-Slot - 1), -1 = ungebunden */
} re15_rbj_bindung_t;

/* Wendet die Schleife von FUN_8001b3f8 auf einen RBJ-Block an. 0 = gueltig, -1 = kein/ungueltiger Block
 * (dann ist *out vollstaendig -1). */
int re15_rbj_binder(const uint8_t *rbj, size_t size, re15_rbj_bindung_t *out);
/* Kurzformen: Record des Spieler-Kanals / des Gegner-Index i; -1 = keiner. */
int re15_rbj_binder_spieler(const uint8_t *rbj, size_t size);
int re15_rbj_binder_gegner(const uint8_t *rbj, size_t size, int gegner_index);
/* Alte Port-Regel (bis Runde 41): ERSTER Record mit Marker-Bit (1+i) bzw. Spieler = Record 0 — nur fuer
 * die Vorher/Nachher-Messzeile `[rbj-binder]` und den Riegel. */
int re15_rbj_binder_alt_gegner(const uint8_t *rbj, size_t size, int gegner_index);

/* Messzeilen `[rbj-binder]` (stderr -> debug.log): je Raum-Bindung einmal (orig = FUN_8001b3f8, alt = Port-Regel bis
 * Runde 41), je aufgeloestem Aktor-Slot einmal (welcher Record spielt den Sub-0-Kanal). */
void re15_rbj_binder_raum_protokoll(const uint8_t *rbj, size_t size);
void re15_rbj_binder_aktor_protokoll(int slot, int typ, int marker_slot, int rec, int gueltig);

#endif /* RE15_RBJ_BINDER_R41_H */
