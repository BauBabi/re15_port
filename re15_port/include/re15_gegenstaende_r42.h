/*
 * RE1.5 Rebuilt — Runde 42 Strang "gegenstaende" (v0.8.31), Stufe 1: jeder Gegenstand bekommt Fundort und Verwendung (Konzept K-10).
 *
 * NUTZER 16.11.2026 (woertlich): "Aktuell haben wir auch noch quest Items, wie zum Beispiel eine Mutter in den Menu, die wir nicht
 * verwenden. Wir sollten überlegen wo alle items die wir haben Verwendung finden, wenn noch nicht drin."
 * Entschieden (KONZEPT.md K-10, C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/): Mutter (2010 finden, Bruecke 2030),
 * Granaten-Bausatz der 40-%-Disc, Colt Python statt Head of Akuma (2040/2041, Bit 91), dazu die Vorschlaege aus GEGENSTAENDE.md:
 * Glock 18 fuer Leon an der Leiche 1050, Granatwerfer fuer Elza in 1071, Rohr im Kesselraum 11F0/11F1, Flammenwerfer-Brennstoff im
 * Spind 4050. Handgranaten (Nutzer 16.11.): "man findet halt die Handgranate in Irons office. Wenn man noch die Munitionsteile findet,
 * kann man noch eine bauen, aber Handgranaten sollte es nicht zu viele geben, wenn sie so stark sind." -> GENAU EIN Zuender im Spiel.
 *
 * ⛔ PORT-STORY: alle Fundorte, Mengen und Texte hier sind Port-Ergaenzungen (Lueckenfueller), kein Original-Verhalten. Die FORM ist
 * jeweils belegt (Item_aot_set @0x80040644, Aufnahme-Modal FUN_8001db28, Vorrang der Zonen FUN_80042bac), Zahlen mit Beleg oder als
 * PORT-WAHL benannt.
 *
 * ---- FORM DER FUNDSTELLEN --------------------------------------------------------------------------------------------------------
 * (a) Zone wie Item_aot_set (Handler @0x80040644): Rechteck, sat 0x31 (alle 164 ausgelieferten Saetze), Taken-Bit Bank 9 (Test
 *     @0x800406dc beim Anlegen, Setzen beim Ja des Modals @0x8001e0d0), KEIN Weltmodell (tk_prop 0xFF) wie die ausgelieferten Saetze
 *     ROOM1050 @0x00B9A (`.. e4 00 ff 00`), ROOM2040 @0x01364, ROOM5020 @0x012C0.
 * (b) VORRANG: der Aktions-Scan FUN_80042bac laeuft die Plaetze in Slot-Reihenfolge (@0x80042c50) und kehrt nach dem ERSTEN Treffer
 *     zurueck (@0x80042f8c `jalr v0` / @0x80042f94 `j 0x80043028`). Liegt der neue Gegenstand bei einem sichtbaren Original-Gegenstand
 *     (oder einem Spind/Schrank), nimmt er DESSEN Slot, und der Original-Satz zieht unveraendert in einen freien Slot (Muster
 *     itemorte_r38.c / kartenitems_r36.c). Beim Druecken kommt erst der neue Gegenstand, danach wie bisher der Original-Satz.
 *     Mehrere Item_aot_set auf DEMSELBEN Rechteck sind Original-Form (ROOM2010 @0x00766/@0x0077C, ROOM40B0 @0x00724/@0x0073A).
 *     Voraussetzung (Zensus je Stelle, Kopf der Tabelle in gegenstaende_r42.c): kein Skript des Raums spricht den Slot danach an
 *     (Aot_reset/Aot_on/Aot_set/Item_aot_set).
 * (c) Leiche 1050 (Glock): die Form der Elza-Variante 1051 bytegleich (main00 @0x00C0E Aot_set Slot 11 sce 3 -> sub03, @0x00C22
 *     Item_aot_set Slot 12 Rechteck 0, sub03 @0x00DA4..@0x00DD7 mit Message_on 5 @0x00DB6 und Aot_on 12 @0x00DBC). 1050 und 1051
 *     teilen BSS (ROOM105.BSS), Kameratabelle (RDT+0x60, 10 Cuts, bytegleich) und Nachricht 5 ("... Sorry, I'll just borrow this...",
 *     in 1050 ohne Aufruf).
 *
 * ---- BANK 9 (Zensus 17.11.2026: tools/r36_kartenbits/bank9_zensus.py original + port, re15_story_r42.h "frei in allen drei
 *      23..51"; dort vergeben 17..39 und 42; r42/finale, r42/gore, r42/rest ohne eigene Commits) -------------------------------------
 */
#ifndef RE15_GEGENSTAENDE_R42_H
#define RE15_GEGENSTAENDE_R42_H

#include <stdint.h>

#define RE15_G42_BANK            9
#define RE15_G42_BIT_GLOCK       40   /* Glock 18 genommen, Leiche 1050 (Leon)                                   */
#define RE15_G42_BIT_MUTTER      41   /* Mutter genommen, 2010/2011                                              */
#define RE15_G42_BIT_MUTTER_DRIN 43   /* Mutter am Bruecken-Panel 2030/2031 eingesetzt                           */
#define RE15_G42_BIT_ROHR        44   /* Rohr genommen, 11F0/11F1                                                */
#define RE15_G42_BIT_WERFER      45   /* Granatwerfer genommen, 1071 (Elza)                                      */
#define RE15_G42_BIT_HUELSEN_1191 46  /* Leere Granathuelsen 1191 (Elza)                                         */
#define RE15_G42_BIT_HUELSEN_30E1 47  /* Leere Granathuelsen 30E1 (Elza)                                         */
#define RE15_G42_BIT_ZUENDER     48   /* Zuender 3000/3001 (der EINZIGE im Spiel)                                */
#define RE15_G42_BIT_NITRO       49   /* Nitro-Kapsel 30E0/30E1                                                  */
#define RE15_G42_BIT_SAEURE_40B1 50   /* Saeure-Kapsel 40B1 (Elza)                                               */
#define RE15_G42_BIT_SPIND_4050  51   /* Spind 4050 Brennstoff (Leon) / 4051 Saeure-Kapsel (Elza): Spiegelraeume  */
#define RE15_G42_BIT_BRAND_5021  170  /* Brand-Kapsel 5021 (Elza). 23..51 sind damit voll; 170..179 frei im Mod-RDT-
                                       * Zensus (keine Item_aot_set/Ck/Set/Aot_reset mit Bank 9), im Port-Zensus und auf
                                       * der 40-%-Disc (dort nur 2 6 10..22 belegt, re15_story_r42.h)                 */
#define RE15_G42_BIT_PYTHON      91   /* = Original-Bit des Head of Akuma, ROOM2040/2041 main00 @0x01364 pc[18] `5b 00` */

/* ---- Gegenstaende (Ids = Namensbank DEBUG.BIN / Eigenschaftssaetze @0x80074DA8 + Id*12) ---------------------------------------- */
#define RE15_G42_ROHR       0x02   /* @0x80074DC0 `00 ..` Obergrenze 0 (Nahkampf wie das Messer, re15_pipe_r41.h)        */
#define RE15_G42_GLOCK      0x06   /* @0x80074DF0 `0f ..` Magazin 15                                                       */
#define RE15_G42_WERFER     0x0F   /* @0x80074E5C `06 ..` Magazin 6 (Explosive Rounds)                                     */
#define RE15_G42_PYTHON     0x14   /* @0x80074E98 `06 ..` Magazin 6                                                        */
#define RE15_G42_AKUMA      0x32   /* ROOM2040/2041 main00 @0x01364 `.. 32 00 01 00 5b 00 ff 00`                            */
#define RE15_G42_BRENNSTOFF 0x18   /* @0x80074EC8 `64 ..` Obergrenze 100                                                   */
#define RE15_G42_ZUENDER    0x1C   /* Kombi-Satz @0x80074D10 `1c 1c 05 00 1e 09 06 09 1f 0a 06 0a 20 0b 06 0b`             */
#define RE15_G42_HUELSEN    0x1D   /* Kombi-Satz @0x80074D20 `1d 1d 05 00 1e 19 06 0c 1f 1a 06 0d 20 1b 06 0e`             */
#define RE15_G42_NITRO      0x1E   /* @0x80074D30 `1e 1e 05 00 1c 09 06 09 1d 19 06 0c`                                     */
#define RE15_G42_SAEURE     0x1F   /* @0x80074D3C `1f 1f 05 00 1c 0a 06 0a 1d 1a 06 0d`                                     */
#define RE15_G42_BRAND      0x20   /* @0x80074D48 `20 20 05 00 1c 0b 06 0b 1d 1b 06 0e`                                     */
#define RE15_G42_MUTTER     0x2F   /* Name @0x800739d7 (Disc ナット); Eigenschaften @0x80074FDC `01 ..` Obergrenze 1         */
#define RE15_G42_HG_MUNITION 0x15  /* H. Gun Bullets                                                                         */

/* ---- Mengen ---------------------------------------------------------------------------------------------------------------------
 * Waffen = Magazin des Eigenschaftssatzes (Regel aus dem Bestand: alle 13 ausgelieferten Waffen-Item_aot_set tragen Menge == Magazin,
 * re15_kleinkram_r38.h P3). Munition/Bausatz: die Welt-Menge wird beim Aufnehmen halbiert (NUTZER-ENTSCHEIDUNG
 * re15_pickup_menge_nutzer, inventory_common.c; Mindestmenge 1). */
#define RE15_G42_MENGE_GLOCK    15   /* @0x80074DF0                                                                          */
#define RE15_G42_MENGE_WERFER   6    /* @0x80074E5C                                                                          */
#define RE15_G42_MENGE_PYTHON   6    /* @0x80074E98                                                                          */
#define RE15_G42_MENGE_ROHR     1    /* PORT-WAHL: Obergrenze 0 (@0x80074DC0), re15_inv_grant weist Menge 0 ab; das Rohr zeigt
                                      * keine Zahl (re15_pipe_r41_ohne_zahl, NUTZERWUNSCH Runde 41)                           */
#define RE15_G42_MENGE_BRENNSTOFF 100 /* Obergrenze @0x80074EC8 `64` (GEGENSTAENDE.md V-07); ins Inventar kommen 50           */
#define RE15_G42_MENGE_TEIL     6    /* PORT-WAHL Huelsen/Kapseln (Elza): GEGENSTAENDE.md V-06 "je 3" = Inventar-Menge nach der
                                      * Halbierung -> Welt 6. Bausatz-Ergebnis = kleinere der beiden Mengen (case 6
                                      * @0x8004e680: EQUAL @0x8004e6d8 / Rest @0x8004e76c / @0x8004e83c)                        */
#define RE15_G42_MENGE_ZUENDER  1    /* NUTZERWUNSCH "Handgranaten sollte es nicht zu viele geben": ein Zuender = hoechstens
                                      * EINE weitere Handgranate (Zuender + Kapsel -> 0x09/0x0A/0x0B, @0x80074D10)           */
#define RE15_G42_MENGE_NITRO_LEON 1  /* PORT-WAHL: Leon hat keinen Granatwerfer -> eine Kapsel fuer den einen Zuender        */

/* ---- Bruecken-Panel 2030/2031: die Mutter ---------------------------------------------------------------------------------------
 * K-10 nennt "Ventil 13". Das gibt es seit Runde 41 Spur N (Nachtrag 37) nicht mehr: NUTZER-VORGABE Nachtrag 30 "erstelle ein
 * Raetsel ... anstatt das komische - dreh an Valve 13" -> Pult Slot 12 (RE15_R41C_SLOT_PULT) Ereignis 24 oeffnet das Panel-Raetsel
 * (re15_panel2030_r41.h). Die Mutter sitzt deshalb am NACHFOLGER von Ventil 13, dem Pult: solange (9,43) = 0 und die Bruecke nicht
 * schon bewegt ist ((9,101)/(9,102) aus alten Spielstaenden), laeuft Ereignis 24 in das Programm unten (Form ROOM2060 sub18
 * @0x01678: Message_on @0x01678, Evt_next, Frage @0x0167E, Evt_next, Ifel_ck @0x01684 Ck(12,31)=0 @0x01688). Benutzt wird die
 * Mutter wie die Kurbel aus dem Inventar (keyitems_r41.c k_stellen); danach "wie bisher" das Panel. */
#define RE15_G42_RAUM_PULT        0x2030u
#define RE15_G42_MSG_OHNE         56   /* 2030 belegt bis 55 (re15_ventilkette_r41.h 40..55); PC-Tabelle 256 (msg_common.c)     */
#define RE15_G42_MSG_FRAGE        57
#define RE15_G42_MSG_DANACH       58

/* ---- Leiche 1050 (Glock) --------------------------------------------------------------------------------------------------------- */
#define RE15_G42_RAUM_LEICHE      0x1050u
#define RE15_G42_LEICHE_SLOT      11   /* 1051 main00 @0x00C0E `2c 0b 03 31 ..`                                                */
#define RE15_G42_LEICHE_ITEM_SLOT 12   /* 1051 main00 @0x00C22 `50 0c 09 31 ..`                                                */
#define RE15_G42_LEICHE_EREIGNIS  5    /* PORT-WAHL: 1050 hat sub00..sub04 (Zeigertabelle 5 Eintraege), 1051 nimmt sub03; Port-
                                        * Ereignisse in 1050 sind 2 (rolltor_1050.c) und 13 (adaruf_1050.c)                    */
#define RE15_G42_LEICHE_X         15250 /* 1051 @0x00C0E Rechteck `92 3b ae e3 e8 03 e8 03`                                    */
#define RE15_G42_LEICHE_Z         (-7250)
#define RE15_G42_LEICHE_W         1000
#define RE15_G42_LEICHE_D         1000
#define RE15_G42_SAT              0x31  /* pc[3] aller 164 ausgelieferten Item_aot_set und der Leichen-Zone @0x00C0E           */

/* ---- Haken ----------------------------------------------------------------------------------------------------------------------- */
/* Raumaufbau (scd_room_setup.c, nach allen anderen Installern, vor re15_aot_settle_at). */
void re15_gegenstaende_r42_install(uint16_t room_id);
/* Je Bild (scd_vm_tick): 1050 Leichen-Satz stilllegen, sobald die Glock genommen ist (Form 1051 sub01 @0x00CB2). */
void re15_gegenstaende_r42_tick(void);
/* scd_event_fire-Kette: Port-Programm oder NULL. */
const uint8_t *re15_gegenstaende_r42_ereignis(uint16_t room_id, uint8_t event_id);
/* item_discard_common.c: Benutzungsstelle (2030/2031, Text 58) -> Mutter. */
int re15_gegenstaende_r42_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item);
/* re15_inv_desc_entry: CHECK-Texte 0x2F, 0x1C..0x20; sonst NULL. */
const uint8_t *re15_gegenstaende_r42_desc(uint8_t item_id);
/* menu_common.c Klassifizierer: 1 = KEIN Heilmittel, obwohl im Original-Fenster @0x8004ab48 `sltiu v0,v0,0xe` (0x22..0x2F).
 * PORT-KORREKTUR fuer 0x2F: das Original setzt beim Benutzen HP := 77 (@0x8004ae98 `ori v0,zero,0x4d` / @0x8004aea0
 * `sh v0,-13586(at)`) und verbraucht die Mutter; sie soll am Panel wirken (K-10 "Einbaufallen"). */
int re15_gegenstaende_r42_kein_heilmittel(uint8_t item_id);
/* menu_common.c Kombinations-Suche (vor der Tabelle): Glock + H. Gun Bullets. PORT-KORREKTUR (K-10 "Glock wird beim Nachladen zur SIG"):
 *   Glock zuerst:  @0x80074C98 `15 04 02 00` -> Ergebnis 0x04 (SIG); die Pistolen @0x80074C8C/90/94 `15 03` `15 04` `15 05` tragen ihre
 *                  eigene Id -> Ergebnis 0x06, Aktion 2 wie dort.
 *   Munition zuerst: Liste @0x80074CCC `15 15 05 00 03 03 03 00 04 04 03 00 05 05 03 00 04 04 03 00 0c 0c 03 00 13 13 03 00` hat an
 *                  Stelle 5 (@0x80074CDC) `04 04 03 00` doppelt und keine 0x06 -> Partner 0x06 fehlt; PORT-KORREKTUR `06 06 03 00`.
 * Rueckgabe Aktion (0 = nicht zustaendig), *res = Ergebnis-Id, *pic = Mischbild (0). */
int re15_gegenstaende_r42_paar(uint8_t id_a, uint8_t id_b, uint8_t *res, uint8_t *pic);

/* ITPS-Puffer (alle drei Ladestellen wie re15_gatorszene_r41_bild_einsetzen): Item-Bilder der Kapseln 0x1E..0x20 = ihre
 * Inventar-Kachel 2-fach (tools/r42_gegenstaende/kapselbilder.py); die Mod-/Disc-Bloecke tragen nur Capcoms Platzhalter.
 * Stufe 2: Block 0x47 (Blue Master Keycard, Item 0x47; Fundort ROOM4010 main00 @0x00592) traegt "MICHAEL" statt "GORO / MACIDA"
 * (tools/r42_gegenstaende/michaelkarte.py; nur Bild +0x220, CLUT +0x14 bleibt). Die Notiz in 4010 (msg 39 @0x183D) nennt Michael.
 * ULTIMATE 1.5 nimmt fuer 0x47 kein Disc-Bild (re15_ultimate15_bild_quelle), zeigt also dasselbe Bild. */
#define RE15_G42_KARTE_ID   0x47
void re15_gegenstaende_r42_bild_einsetzen(uint8_t *itps, int size);

/* Pruefhaken (Riegel). */
int  re15_gegenstaende_r42_stellen_anzahl(void);
int  re15_gegenstaende_r42_stelle(int i, uint16_t *raum, uint8_t *item, uint8_t *menge, uint8_t *bit, int *vorrang_slot);
const uint8_t *re15_gegenstaende_r42_programm(int welches, int *out_len);   /* 0 = Leiche 1050, 1 = Mutter am Pult */

#endif /* RE15_GEGENSTAENDE_R42_H */
