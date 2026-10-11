/*
 * RE1.5 Rebuilt — Runde 36 Spur Q "schluessel": PARKING LOT KEY (Leon) und PRISON KEY (Elza)
 * im Communications Room ROOM10F0/10F1, Parkplatz-Tuer ROOM11B0 -> ROOM11C0 bis zum Schluessel
 * verschlossen, Roy (ROOM1011) gibt den Prison Key nicht mehr.
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde36/Q_schluessel.md.
 *
 * NUTZER-VORGABE (AUFTRAG.md Z. 45 / Z. 49, woertlich):
 *   "Ich möchte bei Leon das die Tür zum Parking Lot ROOM 11C0 verschlossen bleibt, bis ein Parking Lot
 *    Schlüssel im Communication ROOM 10F0 gefunden wurde. Wähle dir einen Schlüssel aus den Resident
 *    Evil 2 Items aus, der am ehesten den optischen Eindruck eines Parking Lot Schlüssels macht."
 *   "Ich möchte bei Elza das der prison key nicht in Room 1011 ist, wie aktuell, sondern er soll da
 *    sein, wo dann auch der Parking Lot Schlüssel in Leons Szenario ist. Auch hier - suche die ein
 *    gutes Model aus Resident Evil 2 aus für einen prison key."
 * Kein Asset-Patch: RDTs, ITEMALL.PIX und ITPS.ITP bleiben byte-true; Prop, Zone, Tuer-Umwidmung,
 * Namen, Icon und Item-Bild entstehen portseitig (Muster inhalt_r35.c, tuer1120_1130.c,
 * re15_sicherung.h).
 *
 * WAS DAS ORIGINAL VORGIBT (selbst nachdisassembliert, Dossier Abschnitt 2):
 *   RE1.5 hat KEINEN Schluessel-Mechanismus an der Tuer. Tuer-Handler PTR_8007469c[2] = 0x800430bc:
 *     800430bc ori v0,zero,1 / 800430c4 sw a0,DAT_800ac9a8 (Nutzlast) / 800430d4 sb v0,DAT_800b5359
 *     (Uebergang = 1) / 800430d8-e4 DAT_800aca40 |= 0xff000000 / 800430e8 jr ra  — kein Test.
 *     Die RE2-Schluesselfelder des Door_aot_set (pc[29] key_id, pc[30] key_type) sind in ALLEN 649
 *     Rechteck-Tueren (sat 0x31) 0 (Zensus Dossier 2.1).
 *   RE2 Retail (das Ziel, wo RE1.5 unfertig ist) prueft im Tuer-Handler LAB_80051514:
 *     800515a8 lbu a1,0xf(s2)  key_id; 800515b0 andi v0,a1,0x80 -> verschlossen?
 *     800515c0 jal FUN_80077360 (a1 = key_id & 0x3f) Schlossbit schon gesetzt -> offen
 *     800515d0 lbu s0,0x10(s2) key_type
 *     80051628 jal FUN_800696cc  Gegenstand key_type im INVENTAR suchen; 80051634 bltz -> fehlt
 *       gefunden: 8005164c jal FUN_8002fe38 (a1 0x100, a2 5 = "You have used the <Name>."
 *                 @0x8009F0E5), 80051654 lui a0,0x225 / 80051658 jal 0x8005ba28 (Aufschliess-Ton),
 *                 80051670 Wegwerf-Fortsetzung, 80051690 jal FUN_8007730c (Schlossbit setzen),
 *                 80051698 `be 45 01 08` j 0x800516f8 -> KEIN Durchgang bei diesem Druck: der Sprung
 *                 laesst den Uebergangs-Block 800516cc..800516f4 aus (800516cc addiu v1,zero,1 /
 *                 800516d4 sb v1 -> 0x800df348, 800516e0-f4 Freeze |= 0xff000000). v0 ist dabei KEIN
 *                 Unterscheider — 0 in allen Zweigen (Delay-Slots 8005169c / 800516c4, 800516c8).
 *       fehlt:    800516a0 lui a0,0x216 / 800516a4 jal 0x8005ba28 (Verschlossen-Ton),
 *                 800516b4 addiu a2,s0,-0x4c / 800516b8 jal FUN_8002fe38 (Text key_type-0x4c)
 *   RE2-Text fuer key_type 0x5F ("P. Room Key", das Modell des Parking Lot Key) = Index 0x13
 *     @0x8009F2BB: "It's locked. / The door is marked "Power Room"." — FORM unserer Sperr-Meldung.
 */
#ifndef RE15_SCHLUESSEL_R36_H
#define RE15_SCHLUESSEL_R36_H

#include <stdint.h>

/* ---- die zwei Gegenstaende ------------------------------------------------------------------
 * Item-Ids: zwei ausgelieferte, aber NIE platzierte Schluessel-Ids der 102er-Namenstabelle
 * (DEBUG.BIN Offsettabelle @0x800c495c, Reader FUN_80028840). Zensus (Dossier 2.3): 0
 * Item_aot_set (`50 slot 09`, Id u16 @+14) und 0 Aot_reset sce 9 in allen 206 RDTs; Eigenschafts-
 * satz @0x80074DA8 + Id*12 = `01 00 00 00 88 4c 07 80 00 00 00 00` (Obergrenze 1, Null-Kombinations-
 * satz @0x80074C88) — bytegleich mit allen Schluesselkarten 0x36..0x47; Klasse "Schluessel"
 * (re15_item_is_key: Id >= 0x22, `sltiu id,0x22` @0x80049124). Icon-Tile und ITPS-Block liegen in
 * den 72er-Dateien (Id < 0x48, nicht im Dokument-Bereich 0x48+).
 *   0x45 ausgeliefert "Water Key"  -> PARKING LOT KEY (Leon)
 *   0x42 ausgeliefert "Key Disc"   -> PRISON KEY      (Elza)
 * Name, CHECK-Text, Icon und Item-Bild ersetzt der Port (re15_schluessel_r36_name / _desc /
 * _icon_einsetzen / _bild_einsetzen). Beide ausgelieferten Namen tragen "Key"
 * (item_prompt_common.c re15_prompt_name_ist_schluessel) -> Wegwerf-Text bleibt der Schluessel-Text. */
#define RE15_R36Q_PARK_ITEM        0x45
#define RE15_R36Q_PRISON_ITEM      0x42
/* Menge 1: jede Schluessel-Ausgabe des Originals (discard_sites.inc Zensus, "Menge [1]" fuer alle
 * 8 Schluessel-Gegenstaende). */
#define RE15_R36Q_MENGE            1
/* Zone-9-Bits (VERTRAG 1.1 Spur Q): 82 = Parking Lot Key genommen (Leon 10F0), 83 = Prison Key
 * genommen (Elza 10F1). */
#define RE15_R36Q_PARK_BIT         82
#define RE15_R36Q_PRISON_BIT       83
/* 93 = Parkplatz-Tuer aufgeschlossen. Reserve-Bit (VERTRAG 1.1 "93-95 nur mit Begruendung"):
 * RE2 haelt "aufgeschlossen" in einem EIGENEN Schlossbit (FUN_8007730c @0x80051690, Feld key_id),
 * getrennt von Besitz und Aufnahme — ein benutzter Schluessel darf weggeworfen werden, die Tuer
 * bleibt offen. Bit 82 kann das nicht tragen (genommen != benutzt). */
#define RE15_R36Q_TUER_BIT         93

/* Zelle ROOM1211: das ORIGINAL-"Prison key"-Flag (4,0xEC). Gesetzt im Original NUR von ROOM1011 sub06
 * @0x00E2C `22 04 ec 01` ("You've taken the Prison key."), gelesen ROOM1211 sub00 @0x01DEA
 * Ck(4,0xEC)==0 -> Text msg 10 / sonst @0x01E36 John + Platz 6 -> sub02 (Befreiung). Die Aufnahme
 * des Prison Key in 10F1 setzt es an Stelle von sub06 (re15_schluessel_r36_genommen). */
#define RE15_R36Q_ZELLE_BANK       4
#define RE15_R36Q_ZELLE_BIT        0xEC

/* ---- Lage im Communications Room (ROOM10F0 Leon / ROOM10F1 Elza: AOT- und Prop-Satz bytegleich,
 * Sonde `aots 10F0/10F1`) ---------------------------------------------------------------------
 * Slot 15 = erster freier Aktions-Slot < 48 (0..14 belegt, Kamerazonen ab 48); obj 4 (nOmodel = 4,
 * obj 0..3 belegt; TIM-Slot RE15_TIM_SLOT_PROP(4) = 8). Props/Zonen: inhalt_r35.c-Muster. */
#define RE15_R36Q_SLOT             15
#define RE15_R36Q_OBJ              4
/* Prop-Felder wie die Item-Props des Originals (Obj_model_set LAB_80040914): Flags 0x000B
 * (pc[6..7] 0x000A | 1 @0x80040998), Band 1 — RE15_R35I_PROP_FLAGS / _BAND, 100 von 121. */
#define RE15_R36Q_PROP_FLAGS       0x000B
#define RE15_R36Q_PROP_BAND        1
#define RE15_R36Q_AOT_SAT          0x31     /* 160 von 164 Item_aot_set (RE15_R35I_AOT_SAT) */
/* LAGE — PORT-WAHL aus Messung (Dossier 3.2): auf dem Suedende der Funkkonsole an der Ostwand
 * (Kollisionszelle SCA 10 x 7400..9300, z 2500..14750), gleich links im Eintrittsbild Cut 0 (Tuer von
 * ROOM10D0, Slot 0). Plattenhoehe TRIANGULIERT am gelben Ordner auf derselben Konsole (Cut 1 (90,8 ;
 * 156,7) + Cut 2 (235,0 ; 150,0), Strahlen der Engine-Matrix, Restabstand je 18): Welt (7808,-1482,
 * 6246) = Ordner-Oberseite -> Platte -1460. Verworfen (gemessen, Dossier 3.2): der Holzschreibtisch
 * (SCA 9 Suedende, Remington-Tisch y -1300) — in Cut 5 und 6, den einzigen Kameras auf ihn, deckt
 * JEDE Stelle der Platte eine Maske (Sonde `sicht`, 28 von 28 Punkten), auch die Original-Remington.
 * Hier: Cut 0 (95..102 ; 170) und Cut 1 ohne Maske (Sonde `sicht`), Framedump-Differenz mit/ohne
 * Bit 82: Cut 0 9 Bildpunkte, Cut 1 2 (der RE2-Schluessel ist 152 x 76 gross, die Kameras weit).
 * rot_y 256 = 22,5 Grad schraeg zur Konsolenkante. */
#define RE15_R36Q_X                8050
#define RE15_R36Q_Y                (-1460)
#define RE15_R36Q_Z                2700
#define RE15_R36Q_ROT_Y            256
/* AUFHEBE-RECHTECK — PORT-WAHL 1000 x 1000 (haeufigste Groesse, 69 von 162 Item_aot_set,
 * RE15_R35I_KARTE_RECT_*) x 7850..8850, z 2050..3050: der Aktions-Scan nimmt je Druck den KLEINSTEN
 * treffenden Slot (`j 0x80043028` @0x80042f94) — der Text-Platz Slot 2 "A large communications
 * device..." (x 7000..7800, z 2600..12300) wuerde jede Zone mit x <= 7800 schlucken, deshalb beginnt
 * diese bei 7850. Stand zum Aufheben: suedlich der Konsole (z ~ 2050), Blick +z, 620 voraus
 * (@0x80042bd0) liegt in der Zone. Kein anderer Satz von 10F0/10F1 schneidet sie. */
#define RE15_R36Q_RECT_X           7850
#define RE15_R36Q_RECT_Z           2050
#define RE15_R36Q_RECT_W           1000
#define RE15_R36Q_RECT_D           1000

/* ---- Parkplatz-Tuer ROOM11B0 -> ROOM11C0 (nur Leon) ----------------------------------------
 * ROOM11B0 main00 @Datei 0x00FAA `3b 01 02 31 00 00 96 97 0c 8b c4 09 ac 0d 41 9d 00 00 74 43 fb 11
 * 00 1c 00 ...` = Door_aot_set Slot 1, sce 2, sat 0x31, Band 0, Rechteck (-26730,-29940,2500,3500)
 * -> Stage 0 Raum 0x1C (ROOM11C0), Spawn (-25279,0,17268), Richtung 0x11FB, Cut 0; key_id/key_type 0.
 * ROOM11B1 (Elza) traegt denselben Satz @0x00F9C und bleibt UNBERUEHRT (die Parkplatz-Tuer bleibt fuer
 * Elza erreichbar, AUFTRAG.md Nachtrag 14:00 Spur J). Ohne Strom widmet sub01 @0x01216 den Slot ohnehin
 * zum Text msg 15 um ("It's too dark to see anything..."), Ck(4,0xF3)==0 — dann tut dieser Haken
 * nichts (er greift nur, solange Slot 1 eine TUER ist). */
#define RE15_R36Q_TUER_RAUM        0x11B0
#define RE15_R36Q_TUER_SLOT        1
/* Strom-Flag: ROOM11B0 sub01 @0x011F6 `21 04 f3 00` Ck(4,0xF3)==0 -> @0x01216 Aot_reset(1 -> Text 15);
 * gesetzt vom Kesselraum ROOM11F0 sub18 @0x016F6 Set(4,0xF3)=1 (ablauf_leon.md L17). */
#define RE15_R36Q_STROM_BANK       4
#define RE15_R36Q_STROM_BIT        0xF3
/* Nachrichten-Ids (VERTRAG 1.2: ROOM11B0 Spur Q 28..29; ROOM11B0 hat 16 RDT-Nachrichten 0..15). */
#define RE15_R36Q_MSG_ZU           28       /* "It's locked. / The door is marked "Parking Lot"." */
#define RE15_R36Q_MSG_BENUTZT      29       /* "You've used the Parking Lot Key."                   */
/* Ereignis-Nummer (VERTRAG 1.3 Spur Q = 25): der Druck auf die Tuer laeuft ueber scd_event_fire. */
#define RE15_R36Q_EREIGNIS         25
/* Text-/Ereignis-Platz: sat 0x31 wie der Tuer-Satz selbst (pc[3] @0x00FAD) — 0x10 = Aktions-Scan,
 * 0x20 = Vorwaerts-620-Punkt gegen das Rechteck, 0x01 = Spieler (re15_aot.h sce_flags). */
#define RE15_R36Q_TUER_FLAGS       0x31
/* Pausemaske jeder RE1.5-Raummeldung am Platz: 0xffff (alle 524 ausgelieferten sce-1-Saetze). */
#define RE15_R36Q_MASKE            0xffffu

/* ---- ROOM1011 (Elza): Roy gibt den Prison Key nicht mehr ---------------------------------------
 * ROOM1011 sub00 @0x00A52 Aot_set Slot 5 sce 3 -> sub06 (Ck(4,0xFA)==1 & var0A==0), @0x00A78
 * Aot_reset Slot 5 -> sub07 (Ck(3,0x78)==1); sub02 @0x00DC0 Aot_reset Slot 5 -> sub06 (Ende des
 * Dialogs). sub06 = msg 19 "He is holding something..." + msg 20 "You've taken the Prison key." +
 * @0x00E2C Set(4,0xEC) + @0x00E30 Set(3,0x78) + -> sub07; sub07 = msg 18 (Zettel "0513").
 * Der Port leitet das Ereignis 6 in ROOM1011 auf 7 um: Roy verhaelt sich, wie das Original ihn nach
 * der Schluessel-Uebergabe zeigt ((3,0x78)==1 -> sub07, @0x00A78) — ohne Schluessel. */
#define RE15_R36Q_ROY_RAUM         0x1011
#define RE15_R36Q_ROY_SUB_SCHL     6
#define RE15_R36Q_ROY_SUB_ZETTEL   7

/* ---- ROOM1211 (Elza): Benutzungsstelle des Prison Key (Nachbesserung 1, Mangel M1) -------------
 * NUTZER-REGEL Runde 21 (analysis/befunde_2026-09-21/discard-umsetzung.md, woertlich: "bei KEy Items,
 * die verwendet wurden, und nicht mehr im Spiel danach benoetigt werden genauso wie in Resident Evil 2
 * fragst - ... drop it? Yes No"). FORM = RE2 (info/re2leon/PSX.EXE, selbst disassembliert):
 *   80051670 sw v0,-32080(at)            Fortsetzung beim Oeffnen der Benutzungs-Nachricht
 *   800517f0 andi v0,v0,0x80 / 800517f4 bne v0,zero,0x80051870   warten, solange belegt
 *   80051810 addiu v0,v0,-1 / 80051824 bne v0,zero,0x8005185c     Zaehler -1, bei 0 fragen:
 *   80051830 addiu a2,zero,9 / 80051834 jal 0x8002fe38             Abfrage 9 (im Port die RE1.5-Zeile
 *                                        DEBUG.BIN @0x800C508B, re15_item_discard.h)
 * BENUTZT wird der Prison Key in der Befreiungsszene ROOM1211 sub02 (Elza schliesst auf: @0x020F8
 * `3f 00 00 00` Plc_motion(0,0,0), die Zellentuer schwingt ab @0x0210C; Spur L haengt dort den
 * Aufschliess-Ton ein). DANACH NIE WIEDER GEBRAUCHT: (4,0xEC) liest im Elza-Spiel nur ROOM1211 sub00
 * @0x01DEA `21 04 ec 00` (der zweite Leser ROOM3070 @0x03408 ist Leons Raum, ROOM3071 hat keinen), und
 * sub02 @0x01F76 `22 04 fc 01` setzt (4,0xFC), das sub00 @0x01E1A `21 04 fc 00` abfragt — die Szene
 * loest nie wieder aus (Zensus analysis/story_leon_elza/flag_xref.txt).
 * STELLE = PORT-WAHL: die LETZTE Nachricht der Szene, sub02 @0x0226A `2b 09 00 00` Message_on 9
 * "Elza: John. Wait!" (Text @0x2582, Ende @0x259B `01 63` = 99 Bilder Standzeit). Dahinter folgen nur
 * noch Plc_motion/Sleep 50+51 (@0x02272-@0x02282), John weg (@0x0228A Pos_set), Set(2,7)=0 @0x0229A
 * und Plc_ret @0x022A8 — die Abfrage steht also am Szenenende, waehrend sie die Szene einfriert
 * (RE2 @0x80051844/@0x80051850); nach der Antwort laeuft der Rest der Szene (Messung im Dossier).
 * Maske 0 (pc[2..3] = 00 00) friert ueber die Nachricht selbst nichts ein, aber die ganze Szene haelt
 * das Pad: sub02 @0x01F9C `22 02 07 01` Set(2,7)=1 = RE15_PAUSE_PAD (FUN_80030444 @0x800304f8) bis
 * @0x0229A — in der Spanne "vorgemerkt" kann also weder Spieler noch Skript den Raum wechseln (sub02
 * traegt keinen Tuer-/Raumwechsel-Opcode). */
#define RE15_R36Q_ZELLE_RAUM       0x1211
#define RE15_R36Q_ZELLE_MSG        9

/* Beim Raumaufbau NACH dem Init-Lauf von main00 rufen (scd_room_setup.c scd_room_reenter und
 * platform/pc/main.c Boot/CONTINUE). Legt in 10F0/10F1 Prop + Zone an (nur ohne Bit 82/83) und
 * widmet in ROOM11B0 den Parkplatz-Tuer-Slot um (nur ohne Bit 93 und nur, wenn Slot 1 eine Tuer ist). */
void re15_schluessel_r36_install(uint16_t room_id);

/* Haken in scd_event_fire VOR der Programm-Kette: liefert die Ereignis-Nummer, die tatsaechlich
 * laufen soll (ROOM1011: 6 -> 7, sonst unveraendert). */
uint8_t re15_schluessel_r36_umleiten(uint16_t room_id, uint8_t event_id);

/* Haken in der Programm-Kette von scd_event_fire: ROOM11B0 Ereignis 25 = Druck auf die gesperrte
 * Parkplatz-Tuer. Fuehrt den RE2-Tuer-Handler aus (Besitz-Test, Text, Ton, Schlossbit, Tuer zurueck)
 * und liefert ein leeres Programm (Evt_end); sonst NULL. */
const uint8_t *re15_schluessel_r36_ereignis(uint16_t room_id, uint8_t event_id);

/* Haken in re15_discard_besitz_vor_nachricht (item_discard_common.c), hinter der erzeugten Tabelle
 * gen/discard_sites.inc. Zwei Port-Benutzungsstellen:
 *   (ROOM11B0, msg 29 "You've used the Parking Lot Key.") -> 0x45 — RE2 haengt genau dort die Wegwerf-
 *     Fortsetzung ein (@0x80051670, nach Text 5 @0x8005164c), die bei Anzahl 0 "This key is useless now.
 *     Discard?" fragt (@0x80051834; im Port RE1.5s eigene Zeile DEBUG.BIN @0x800C508B "You don't need
 *     this key any more. Discard it?");
 *   (ROOM1211, msg 9 "Elza: John. Wait!", sub02 @0x0226A) -> 0x42 — Ende der Befreiungsszene, in der der
 *     Prison Key benutzt wird (RE15_R36Q_ZELLE_*, Nachbesserung 1).
 * 1 + *out_item = Gegenstand fuer diese Stelle, sonst 0. */
int re15_schluessel_r36_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item);

/* Haken im Item-Modal unmittelbar nach dem Setzen des Zone-9-Bits: der Prison Key setzt das
 * Original-Flag (4,0xEC) wie ROOM1011 sub06 @0x00E2C. */
void re15_schluessel_r36_genommen(uint8_t item_id, uint8_t taken_bit);

/* Name (Glyphen der Spielschrift, 0x07-terminiert, wie DEBUG.BIN @0x800c4a28) und CHECK-Text
 * (Beschreibungsbank-Form, 01 00-terminiert) fuer die zwei Ids; NULL fuer jede andere Id. */
const uint8_t *re15_schluessel_r36_name(uint8_t item_id);
const uint8_t *re15_schluessel_r36_desc(uint8_t item_id);
/* ASCII-Name fuer die Debug-HUD (re15_item_name), NULL fuer jede andere Id. */
const char *re15_schluessel_r36_name_ascii(uint8_t item_id);

/* Icon-Tile (ITEMALL.PIX, 1200 B je Id, Tile == Id) und Item-Bild (ITPS.ITP, Block Id*0x3000)
 * der zwei Ids im GELADENEN Puffer einsetzen — an jeder Ladestelle (Muster re15_sicherung.h). */
void re15_schluessel_r36_icon_einsetzen(uint8_t *itemall, int size);
void re15_schluessel_r36_bild_einsetzen(uint8_t *itps, int size);

/* Plattform-Lader: obj_id des Props im Raum (-1 sonst) und seine MD1/TIM-Bytes. */
int re15_schluessel_r36_obj_id(uint16_t room_id);
const uint8_t *re15_schluessel_r36_md1_bytes(uint16_t room_id, int *out_size);
const uint8_t *re15_schluessel_r36_tim_bytes(uint16_t room_id, int *out_size);

/* Pruefhaken: Meldungen als .msg-Rohbytes, Zustand der Tuer im aktuellen Raumaufbau
 * (0 = unberuehrt, 1 = gesperrt/umgewidmet), Zaehler der Druecke je Ausgang. */
const uint8_t *re15_schluessel_r36_meldung(int msg_id, int *out_len);
int re15_schluessel_r36_tuer_gesperrt(void);
extern unsigned g_re15_r36q_zu_zaehler;        /* Druecke ohne Schluessel  */
extern unsigned g_re15_r36q_benutzt_zaehler;   /* Druecke mit Schluessel   */

#endif /* RE15_SCHLUESSEL_R36_H */
