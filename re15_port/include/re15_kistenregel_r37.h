/*
 * RE1.5 Rebuilt — Runde 37 Spur Q "kistenregel": ein Gegenstand wird nur benutzt, wenn er im INVENTAR ist
 * (die RE2-Item-Box des Ports zaehlt nicht). Dossier: analysis/befunde_runde37/Q_kistenregel.md.
 *
 * ORCHESTRATOR-AUFTRAG Q (AUFTRAG.md, woertlich gekuerzt): "legt Leon den Feuerloescher VOR dem Loeschen in die
 * Item-Box, kann er das Feuer in ROOM1090 trotzdem loeschen — ROOM1090 sub00 @0x02306 `21 03 85 00` prueft nur das
 * Aufnahme-Bit (3,0x85), nie das Inventar. [...] in RE2 benutzt man ein Item nur, wenn man es TRAEGT. [...] Wo die Box
 * den Fall oeffnet: wie RE2 das Item im Inventar verlangen (eine Port-Regel an EINER Stelle, je Fall per Tabelle)".
 *
 * WARUM DAS ORIGINAL EIN BIT FRAGT (selbst disassembliert, Dossier Abschnitt 2.1):
 *   RE1.5 hat keinen Besitz-Opcode (0x5E @0x80042b04 liefert konstant 1, @0x80042b44 `ori v0,zero,0x1`), die
 *   Inventar-Suche FUN_8004dfec (@0x8004e014 Basis 0x800b10ac) hat nur die Nachlade-Aufrufer @0x8004ebbc/@0x8004ec34,
 *   und kein Opcode nimmt einen Gegenstand aus dem Inventar. Ohne Kiste ist "Bit gesetzt" == "im Inventar".
 * WIE RE2 PRUEFT (Dossier 2.2): Keep_Item_ck = Tabelle 0x800A74C8 + 0x5E*4 -> 0x800584F0, `jal 0x800696cc`
 *   @0x8005850c; FUN_800696CC liest nur das INVENTAR (@0x800696e4 `lbu v0,19004(at)` = 0x800d4a3c + i*4), nicht
 *   das Box-Feld 0x800d4a68. Form der Benutzungs-Stelle: ROOM6150 sub02 @0x02D3C prueft "schon benutzt" ZUERST
 *   (`21 04 9f 01`), dann `5e 3f` Keep_Item_ck -> Frage, sonst Raumtext msg 0 "I could open it if I had the key."
 *   (ROOM2080 sub13 @0x0172C `5e 53` -> sonst msg 1 "It's locked. A special kind of key is required.").
 *
 * DIE PORT-REGEL (eine Stelle: der Ck-Opcode, scd_vm.c op_ck = LAB_8003fcf4): liest ein Raumskript an einer Stelle
 * der Tabelle unten sein Besitz-Bit, gilt das Bit nur, wenn
 *     (a) die Benutzung schon geschehen ist ("fertig"-Bit — RE2s Reihenfolge ROOM6150 @0x02D3C), oder
 *     (b) der Gegenstand im INVENTAR ist (re15_inv_find_item = FUN_8004dfec / RE2 FUN_800696CC).
 * Sonst liest das Skript 0 und nimmt SEINEN EIGENEN Zweig "nicht im Besitz" — mit dem Original-Raumtext (je Fall
 * im Dossier 3.1; RE2 zeigt in derselben Lage ebenfalls einen Raumtext). Kein neuer Text, keine neue Nachrichten-Id,
 * kein neues Bit, kein Patch an den RDTs. Ohne Box-Inhalt und mit dem Gegenstand im Inventar ist das Verhalten
 * bytegleich zum Original.
 */
#ifndef RE15_KISTENREGEL_R37_H
#define RE15_KISTENREGEL_R37_H

#include <stdint.h>

/* Ein Fall = eine Ck-Stelle eines Raumskripts. */
typedef struct {
    uint16_t raum;          /* g_current_room_id (Leon gerade, Elza ungerade)                                */
    uint16_t ck_off;        /* Datei-Offset des Ck-Satzes `21 bank bit val` in ROOMxxxx.RDT                  */
    uint8_t  bank, bit;     /* das Besitz-Bit, das der Ck liest (pc[1], pc[2])                               */
    uint8_t  wert;          /* pc[3] des Ck (nur fuer den Riegel: die Original-Bytes)                         */
    uint8_t  item;          /* Gegenstand, der im INVENTAR sein muss (Item-Id der 102er-Tabelle)             */
    uint8_t  fertig_bank, fertig_bit;   /* "schon benutzt": steht es, bleibt das Original-Bit                */
    uint8_t  fall;          /* K-Nummer des Dossiers (1..11)                                                 */
} re15_kr37_fall_t;

/* Die Regel: `got` = Original-Wert des Bits (re15_game_flag_get). Liefert den Wert, den der Ck sehen soll.
 * off = Datei-Offset des Ck-Satzes (t->pc - rdt->raw). Ausserhalb der Tabelle: `got` unveraendert. */
int re15_kistenregel_r37_ck(uint16_t raum, uint32_t off, uint8_t bank, uint8_t bit, int got);

/* Introspektion (Riegel). */
int                     re15_kistenregel_r37_anzahl(void);
const re15_kr37_fall_t *re15_kistenregel_r37_zeile(int i);
/* Zaehler: wie oft die Regel ein gesetztes Besitz-Bit auf 0 gezogen hat (seit Prozessstart). */
extern int g_re15_kr37_gesperrt_zaehler;

#endif /* RE15_KISTENREGEL_R37_H */
