/*
 * RE1.5 Rebuilt — Runde 39 Spur C "inventar" (2026-10-18). Dossier mit allen Messungen und Belegen:
 * analysis/befunde_runde39/C_inventar.md. Zensus-Werkzeug: re15_port/tools/r39_inventar/zensus_mengen.py.
 *
 * NUTZER-VORGABEN (analysis/befunde_runde39/AUFTRAG.md, woertlich):
 *   (1) "In Room 1000 möchte ich im Spind, wo vorher der Feuerlöscher drin war jetzt einmal Handfeuerwaffen Munition
 *        drin haben. Erst danach den text "Nothing useful", den du jetzt schon hast."
 *   (2) "Memory Cards sind noch nicht Stapelbar, das ist falsch, alles mit einer Zahl dran an amounts was gleich ist
 *        muss stapelbar sein."
 *   (3) "Elza hat im Player Inventory noch das Bild und die Karte von Leon oben drin."
 *   (4) "Ich habe die gelbe Karte bei Elza noch, obwohl ich gesagt habe "discard"."  -> Generator
 *        tools/gen_discard_sites.py (Regel E5 je Szenario-Variante), keine Laufzeit-Logik hier.
 */
#ifndef RE15_INVENTAR_R39_H
#define RE15_INVENTAR_R39_H

#include <stdint.h>

/* ---- (1) SPIND ROOM1000 -------------------------------------------------------------------------------------------
 * FORM = der Satz, den das Original in DERSELBEN Raum-Datei der Elza-Variante an DERSELBEN Stelle hat:
 *   ROOM1001.RDT @0x00C24 `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 15 00 1e 00 86 00 ff 00`
 *   = Item_aot_set Slot 3, sce 9, sat 0x31, Band 0, Rechteck (20500,-800) 800 x 800, Item 0x15 x 30, tk_prop 0xFF.
 * Leons Datei ROOM1000.RDT @0x00C24 traegt an dieser Stelle den Feuerloescher (Item 0x31 x1, Bit (9,0x86)); der
 * Loescher haengt seit Runde 38 in ROOM1030 und behaelt Bit 0x86 -> die Munition bekommt das VERTRAG-Bit 69.
 * Danach (Bit gesetzt bzw. Zone aufgenommen) antwortet der Raumtext msg 0 @0x0D22 "Nothing unusual." (der einzige
 * Text von ROOM1000; seit Runde 37 der Spind-Text, feuerloescher_r37.c spind_umwidmen). Der Text-Satz zieht dafuer
 * UNVERAENDERT in Slot 11 (erster freier Slot nach allen Port-Installern, Sonde `probe_r34n_e_dokumente aots 1000`);
 * der Aktions-Scan nimmt den KLEINSTEN treffenden Slot (@0x80042f94 `j 0x80043028`) und ueberspringt sce 0
 * (@0x80042f50) — vor der Aufnahme gewinnt Slot 3 (Munition), danach antwortet Slot 11 (Text). */
#define RE15_R39C_SPIND_RAUM      0x1000u
#define RE15_R39C_SPIND_SLOT      3        /* ROOM1001 @0x00C25 pc[1] = 03                                 */
#define RE15_R39C_TEXT_SLOT       11       /* PORT-WAHL: erster freier Slot in ROOM1000 (Dossier 3.1)        */
#define RE15_R39C_TEXT_MSG        0        /* ROOM1000 msg 0 @0x0D22 "Nothing unusual."                     */
#define RE15_R39C_MUNITION_ITEM   0x15     /* ROOM1001 @0x00C32 pc[14..15] `15 00` (H. Gun Bullets)          */
#define RE15_R39C_MUNITION_MENGE  30       /* ROOM1001 @0x00C34 pc[16..17] `1e 00`                           */
#define RE15_R39C_MUNITION_BIT    69       /* Bank 9 — VERTRAG Runde 39 1.1 (Riegel test_r36_kartenbits k_port) */
#define RE15_R39C_AOT_SAT         0x31     /* ROOM1001 @0x00C27 pc[3]                                        */
#define RE15_R39C_RECT_X          20500    /* ROOM1001 @0x00C2A `14 50`                                      */
#define RE15_R39C_RECT_Z          (-800)   /* ROOM1001 @0x00C2C `e0 fc`                                      */
#define RE15_R39C_RECT_W          800      /* ROOM1001 @0x00C2E `20 03`                                      */
#define RE15_R39C_RECT_D          800      /* ROOM1001 @0x00C30 `20 03`                                      */
#define RE15_R39C_KEIN_MODELL     0xFF     /* ROOM1001 @0x00C38 pc[20] `ff` (tk_prop: kein Modell im Spind)  */

/* Installer: NACH feuerloescher_r37_install (scd_room_setup.c + Boot-/CONTINUE-Weg main.c). Nur ROOM1000 (Leon);
 * ROOM1001 hat die Munition schon im Original. Idempotent. */
void re15_inventar_r39_install(uint16_t room_id);

/* ---- (2) STAPELN ----------------------------------------------------------------------------------------------------
 * "mit einer Zahl dran": Ziffern-Gate FUN_80048f28 `sltiu id,0x22` @0x80049124 -> Ids 0x01..0x21.
 * "an amounts ... gleich": die Zahl ist die ANZAHL des Gegenstands selbst —
 *   Munition 0x15..0x20 + Memory Card 0x21 (RE1.5-Fenster `sltiu id,0x15` @0x80047d54 / `sltiu id,0x22`
 *   @0x80049124 = Gegenstueck zu RE2s Kisten-Gate 0x14..0x1F @0x80070490-94; RE2 Ink Ribbon 0x1E Satz @0x800A9DB8
 *   `1e 05 1e ff` = Selbst-Stapeln) und die Wurfgranaten = Waffen-Klasse mit der Stapel-Obergrenze 250
 *   (@0x80074E14/E20/E2C `fa 00 00 00`; Feuerwaffen tragen ihr Magazin 4..100). Feuerwaffen: Zahl = Magazin EINER
 *   Waffe (Nachladen FUN_8004ebdc), keine Anzahl.
 * Form des Stapelns = RE1.5s eigene Selbst-Saetze (Aktion 5 SELF-STACK, Executor @0x8004e664, z.B. 0x15 @0x80074CCC
 * `15 15 05 00`) mit der Obergrenze @0x80074DA8+id*12 (`lbu` @0x8004e338). */
#define RE15_R39C_STAPEL_OBERGRENZE 250    /* 0xFA: Munition/Memory Card/Granaten-Zeilen @0x80074DA8 (Zensus) */
#define RE15_R39C_AKTION_SELBST     5      /* Matcher-Aktion SELF-STACK (@0x8004e664, RE2 `1e 05 1e ff`)    */
int re15_inventar_r39_stapelt(uint8_t id);

/* Menue-Matcher (menu_common.c exchange_match, FUN_8004e900): Paar gleicher Gegenstaende ohne eigenen Selbst-Satz
 * (0x21, 0x09..0x0B) -> Aktion 5, Ergebnis = Id, Bild 0 (Form `15 15 05 00`). a/b = normierte Plaetze. Rueckgabe
 * 0 = kein Port-Paar (der Original-Satz entscheidet). Wandert ein ausgeruesteter Granaten-Stapel (hoeherer Platz)
 * ganz in den unteren, zieht die Ausruestung mit (sonst zeigte DAT_800b25c8 nach der Kompaktierung FUN_8004dadc
 * auf den nachgerueckten Gegenstand). */
int re15_inventar_r39_paar(int a, int b, uint8_t *result, uint8_t *pic);

/* ---- (3) STATUS-BILD JE FIGUR ---------------------------------------------------------------------------------------
 * DEBUG.BIN FUN_800c01c4: `lbu v1,-13732(v1)` @0x800c01d0 (DAT_800aca5c), Tabelle @0x80073ad8 (`addiu v0,v0,15064`
 * @0x800c01d8) = u16 [0x2c..0x3b], `lhu a0,0(v0)` @0x800c01e4, `jal 0x80013b60` @0x800c01f0 (CD-Lader). Datei-Id
 * 0x2c + k = ITEM/STPIC_<k>.TIM (Datei-Tabelle @0x8006f43c, 12492 B je Eintrag). Aufrufer @0x800316d0 im
 * Spieler-Lader FUN_800314b0 -> derselbe Index wie das Modell (Port: work_vars[0x10], main.c pc_sync_player_model).
 * Leon 0 -> STPIC_00 (Polizeiausweis), Elza 4 -> STPIC_04 (Raccoon City Driver License). */
#define RE15_R39C_STPIC_BASIS       0x2C   /* @0x80073ad8[0] */
int  re15_inventar_r39_stpic_index(void);
void re15_inventar_r39_stpic_pfad(int index, char *out, int out_len);

/* ---- Messschiene (env-gegatet, kein Spielverhalten) -----------------------------------------------------------------
 * RE15_INV_LOG=1: eine Zeile "[inv-r39] F<n> R<raum> inv=... box=..." nach debug.log, sobald sich Inventar oder
 * ITEM BOX aendern (Riegel tests/integration/test_r39_inventar*.cmake). */
void re15_inventar_r39_log_frame(unsigned frame);

#endif /* RE15_INVENTAR_R39_H */
