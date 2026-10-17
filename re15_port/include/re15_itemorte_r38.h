/*
 * RE1.5 Rebuilt — Runde 38 Spur K "itemorte": der Feuerloescher haengt in ROOM1030 (statt ROOM1050), Parking Lot Key
 * (Leon) / Prison Key (Elza) haengen gut sichtbar an der Pinnwand des Communications Room ROOM10F0/10F1.
 * Dossier mit allen Messungen: analysis/befunde_runde38/K_itemorte.md.
 *
 * NUTZER-VORGABE (AUFTRAG.md Runde 38, woertlich):
 *   "Ich haette den fire extinguisher gerne in room 1030 haengen an der stelle von fire_extinguisher.bmp, anstatt in
 *    room 1050."
 *   "Ich moechte das du den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar an die Location
 *    platzierst, die ich im Bild key_location.bmp markiert habe."
 *
 * LAGE (gemessen, erzeugt von re15_port/tools/r38_itemorte/itemorte_export.py -> gen/itemorte_r38_lage.inc):
 *   fire_extinguisher.bmp = ROOM1030 Cut 0 (Bildvergleich gegen alle 428 STAGE1-Hintergruende, 15,94 gegen >= 22,29),
 *   Marke auf der -x-Flaeche des Pfeilers unter dem Notruf-Monitor (Pfeilerkanten trianguliert aus Cut 0/1/3).
 *   key_location.bmp = ROOM10F0 Cut 5 (4,34 gegen >= 90,9), Marke unten rechts auf der Pinnwand (Rueckwand z -3400).
 *
 * VORRANG DER ZONEN (PSX.EXE, selbst disassembliert): der Aktions-Scan FUN_80042bac laeuft die AOT-Saetze in Slot-
 *   Reihenfolge ab (@0x80042c50) und kehrt nach dem ERSTEN Treffer zurueck (@0x80042f8c `jalr v0`, @0x80042f94
 *   `j 0x80043028`); eine genommene Item-Zone (sce 0) wird uebersprungen (@0x80042f50 `beq v0,zero,0x8004301c`).
 *   ROOM1030 Slot 14 (msg 14 "...emergency broadcast...", main00 @0x01DA6) und ROOM10F0/10F1 Slot 8 (msg 4 "Several
 *   files from past cases...", main00 @0x00FEA) decken die beiden Stellen; die Aufhebe-Zone nimmt deshalb DEREN Slot,
 *   der Original-Satz zieht unveraendert in einen freien Slot (Muster Runde 36 N, kartenitems_r36.c: Lageplan 1F nimmt
 *   Slot 8, das Muenztelefon zieht nach 39).
 *
 * SICHTBARKEIT (PORT-ZUSATZ, kein Original-Gegenstueck — das Original hatte an beiden Stellen kein Objekt): FUN_8002c18c
 *   zeichnet ein Objekt in jedem Cut, dessen Anker-Viereck seinen Ursprung enthaelt (FUN_80014368(&pos, DAT_800ac790)),
 *   und verdeckt es nur ueber die Masken des Cuts. Gemessen (Sonde probe_r34n_e_dokumente sicht, Dossier 2.4):
 *     ROOM1030 Cut 10 (= Kamera von Cut 3 nach Cut_replace 3->10, sub00 @0x01FF7 / sub08 @0x02785): die Pfeiler-
 *       Maske 2 liegt dort auf Tiefe 164 (ab vz 10570), der Loescher bei vz 8295 -> er schiene durch den Pfeiler.
 *       Cut 3 (dieselbe Kamera) verdeckt ihn ueber Maske 2 (ab vz 8200) bis auf die zwei Spalten rechts der Maske
 *       (Maske endet bei x 226, Pfeilerkante 228,5) -> der Loescher liegt aus Kamera 3 vollstaendig hinter dem Pfeiler
 *       (sichtbarer Rand rechnerisch 13 Einheiten = 0,3 px) und wird in Cut 3 und 10 nicht gezeichnet.
 *     ROOM10F0/10F1 Cut 7: Ursprung im Anker-Viereck (Zone 20), Bildpunkt auf dem Buecherregal, Cut 7 hat keine
 *       Masken -> der Schluessel schiene durch das Regal und wird in Cut 7 nicht gezeichnet.
 */
#ifndef RE15_ITEMORTE_R38_H
#define RE15_ITEMORTE_R38_H

#include <stdint.h>

/* ---- Feuerloescher ROOM1030 (Leon; Elza ROOM1031 bekommt wie bisher ROOM1051 keinen — Leon war zuerst da) -------- */
#define RE15_R38K_FL_RAUM          0x1030
#define RE15_R38K_FL_SLOT          14    /* Slot des Monitor-Satzes msg 14 (main00 @0x01DA6 `2c 0e 01 31`)          */
#define RE15_R38K_FL_SLOT_ALT_MSG  14    /* dessen Nachricht (pc[14..15] `0e 00`)                                 */
#define RE15_R38K_FL_VERDRAENGT    20    /* frei in ROOM1030 (erster freier 18) UND ROOM1031 (18 = Tuer, 19 = Rueckkehr) */

/* ---- Schluessel ROOM10F0 (Parking Lot Key 0x45) / ROOM10F1 (Prison Key 0x42), derselbe Ort ---------------------- */
#define RE15_R38K_SCHL_SLOT        8     /* Slot des Satzes msg 4 (main00 @0x00FEA `2c 08 01 31`)                 */
#define RE15_R38K_SCHL_SLOT_ALT_MSG 4    /* dessen Nachricht (pc[14..15] `04 00`)                                 */
#define RE15_R38K_SCHL_VERDRAENGT  15    /* der bisherige Schluessel-Slot der Runde 36 (0..14 Original belegt)    */
/* LICHT der zwei RE2-Schluesselmodelle: RE2 Retail stellt BEIDE mit dem Attribut-Wort 0x0010 (FLACH, ohne Raumlicht)
 * auf (Wort nach dem Freigabe-Wort, wie RE2 room20B0 @0x03158 der Lageplaene, Runde 36 N 8.3) — Obj_model_set des
 * P. Room Key ROOM6150.RDT @0x02C98 `2d 08 00 00 00 00 00 00 00 00 00 00 10 00 ...` und des Small Key (Prison Key seit
 * Nachbesserung 1, Dossier 9.3) ROOM10F0.RDT @0x00706 `2d 04 00 00 00 00 00 00 00 00 0a 00 10 00 ...`, ebenso
 * ROOM20E0.RDT @0x018F8 / @0x01A96 und ROOM2020.RDT @0x01C1E (bis Nachbesserung 1: Cabin Key ROOM1050.RDT @0x01274
 * `2d 00 ... 0a 00 10 00`, ebenfalls flach). Zeichner: RE2 FUN_800363f0 `puVar4[4] & 0x10` -> unbeleuchtet,
 * RE1.5 FUN_8002c18c @0x8002c2f4 `andi v0,v0,0x10` -> FUN_80022f0c @0x8002c320 mit 0x808080 (FUN_8002b898.c:16).
 * Runde 36 hatte die Schluessel mit Attribut 0 (NCCT) aufgestellt — an der Pinnwand gemessen fast schwarz. */
#define RE15_R38K_SCHL_ATTR        0x0010

/* Die gemessenen Lagen (Ursprung, Drehung, Rechteck) — erzeugt, mit Herleitung im Kopf der Datei. */
#include "../engine/src/gen/itemorte_r38_lage.inc"

/* Platz fuer eine Aufhebe-Zone frei machen: liegt in `slot` noch der Original-Satz dieser Stelle (ROOM1030 Slot 14
 * msg 14 / ROOM10F0+10F1 Slot 8 msg 4, sce 1), zieht er UNVERAENDERT in den Ausweich-Slot. Idempotent; in jedem
 * anderen Raum/Slot nichts. Rueckgabe 1, wenn der Satz gerade umgezogen ist. */
int re15_itemorte_r38_platz_frei(uint16_t room_id, int slot);

/* Lage des Schluessels (Ursprung, Drehung rot_x/rot_y/rot_z) je Item (0x45 / 0x42); 0 = kein Pinnwand-Schluessel. */
int re15_itemorte_r38_schluessel_lage(uint8_t item, int32_t pos[3], int16_t rot[3]);
/* Das skalierte RE2-MD1 des Schluessels (gen/itemorte_r38_md1.inc), sonst NULL. */
const uint8_t *re15_itemorte_r38_schluessel_md1(uint8_t item, int *out_size);
/* Massstab, mit dem das Werkzeug das MD1 gerechnet hat (x 10000, fuer Sonden/Riegel). */
int re15_itemorte_r38_schluessel_massstab(uint8_t item);

/* HAKEN Prop-Zeichner (platform/pc/main.c, nach re15_prop_culled): 1 = dieses Objekt in diesem Cut NICHT zeichnen
 * (Kopf: SICHTBARKEIT). */
int re15_itemorte_r38_verdeckt(uint16_t room_id, int cut, int obj_id);

#endif /* RE15_ITEMORTE_R38_H */
