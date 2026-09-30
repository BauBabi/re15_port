/*
 * RE1.5 Rebuilt — die zwei Leichen ROOM1110 (Evidence Room) und ROOM1230 (B1-Gang am Waffenlager):
 * neuer Untersuchen-Text und EINMAL Handgun-Munition.
 *
 * Runde 34 Nacht, Spur F. Dossier mit allen Belegen und Messungen:
 * analysis/befunde_runde34_nacht/F_leichen.md (+ F_leichen.gegenpruefung.md).
 *
 * NUTZER-VORGABE (Runde 34 Nacht, woertlich, AUFTRAG.md Z. 49 / Z. 61-64):
 *   "im ROOM 1110 die Leiche nicht mehr den gleichen Text hat, sondern stattdessen: "It's a Police
 *    officer, he's dead. He is Holding something" und dann soll man einmal Handfeuerwaffen Munition
 *    erhalten. Der Text soll immer wiederholt werden koennen, wenn man den Koerper anklickt, bis man
 *    die Munition annimmt. Danach soll nur noch "It's a Police officer, he's dead." kommen"
 *   "in ROOM 1230 bei der Leiche ... "A miserable death…. He is Holding something." und dann soll
 *    Handfeuerwaffen Munition wieder zum mitnehmen erscheinen. Wenn man sie einmal aufgenommen hat,
 *    kommt nur noch: "A miserable death…""
 *
 * ⛔ F NUR ZUSAMMEN MIT SPUR E AUSLIEFERN. Die zwei Original-Nachrichten, die hier ersetzt werden,
 * sind die EINZIGEN Quellen der Tastenfeld-Codes: ROOM1110 msg 0 traegt "4312" (@0x0DB0 `61 05 01
 * 10 0f 0d 0e`: Farbcode `05 01`, Ziffern 0x0C + 4/3/1/2), ROOM1230 msg 10 traegt "5632" (@0x172F
 * `61 05 01 11 12 0f 0e`). Spur E legt sie in Marvin's Notes (ROOM1020, Dok 3) bzw. in die Armory Notice
 * (ROOM1010, Dok 4). F ohne E = Communication Room und Weapon Storage ohne Code (Gegenpruefung
 * Auflage 2).
 *
 * ---------------------------------------------------------------------------------------------
 * DER MECHANISMUS (alles selbst gelesen, RDT-Bytes aus shared_assets/PSX = info/Re1.5/PSX):
 *
 * 1. Beide Leichen sind ORIGINAL-EREIGNISSE, und die bleiben unveraendert der Ausloeser:
 *      ROOM1110 main00 @0x00AEE `2c 05 03 31 00 00 cc 29 66 08 e8 03 e8 03 ff 00 18 02 00 00`
 *               (Aot_set Slot 5, sce 3 = Ereignis -> sub02)
 *      ROOM1110 sub02  @0x0CEE `46 05 00..` Aot_reset(5 aus) | @0x0CF8 `2e 01 00` Work_set |
 *               @0x0CFC `3f 01 0b 00` Plc_motion(1,11,0) | @0x0D00 `09 0a 1e 00` Sleep 30 |
 *               @0x0D04 `2b 00 ff ff` Message_on(0, Maske 0xffff) | @0x0D08 `02 00` Evt_next |
 *               @0x0D0A `3f 01 0b 00` Plc_motion | @0x0D0E `43 00 80 00` Plc_flg | @0x0D12 Sleep 30 |
 *               @0x0D16 `42 00` Plc_ret | @0x0D18 `46 05 03 31 ff 00 18 02 00 00` Aot_reset(5 -> sub02)
 *      ROOM1230 main00 @0x00D52 `2c 12 03 31 00 00 48 0d 38 63 e8 03 e8 03 ff 00 18 15 00 00`
 *               (Aot_set Slot 18, sce 3 -> sub21), sub21 @0x14A6..0x14DB bytegleich aufgebaut,
 *               Message_on @0x014BC `2b 0a ff ff` (Nachricht 10), Re-Arm @0x014D0.
 *      ROOM1111 / ROOM1231 (Elza-Variante): dieselben Bytes an denselben Offsets.
 *    Msg 0 (1110/1111) und msg 10 (1230/1231) werden je NUR an dieser einen Stelle geoeffnet
 *    (Zensus msgref, Dossier 3.2). ⛔ ROOM1230 msg 0 ist das Tastenfeld ("Enter the first number.",
 *    sub17 @0x013AA) — der Schluessel ist deshalb (Raum, Nachricht), nie die Nachricht allein.
 *
 * 2. Am Message_on der Leiche (Handler LAB_800404f4: `lbu a2,1(v0)` @0x80040504 = Id,
 *    `lhu a3,2(v0)` @0x80040508 = Maske, `addiu v0,v0,4` @0x8004050c, `jal 0x80027e68` @0x80040518,
 *    `sll a3,a3,16` @0x8004051c) tauscht der Port NUR die Nachrichten-Id gegen einen eigenen Text
 *    (lang mit Angebot / kurz nach der Annahme). Maske, pc += 4 und der nicht blockierende
 *    Schreibmaschinen-Weg bleiben die des Klartext-Zweigs von op_message_on. Der Haken sitzt HINTER
 *    dem Stimmen-Riegel ("den vorigen Satz ausreden lassen", Gegenpruefung Auflage 1).
 *
 * 3. In dem Bild, in dem der Text zugeht, oeffnet das NORMALE Aufnahme-Modal (FUN_8001db28-Port
 *    item_modal_common.c) mit H. Gun Bullets. Vorbilder:
 *      RE1.5 ROOM1011 (Roy, Elza-Verhoerraum) — "Koerper haelt etwas -> nehmen -> danach anderer
 *        Text" im SELBEN Ereignisrahmen wie die Leichen:
 *        sub00 @0x00A52 `2c 05 03 31 .. 18 06 00 00` (Slot 5 -> sub06), @0x00A70 `06 00 10 00 |
 *        21 03 78 01 | 46 05 03 31 ff 00 18 07 00 00` (wenn (3,120): Slot 5 -> sub07);
 *        sub06 @0x00E0A Aot_reset / Work_set / Plc_motion(1,11,0) / Sleep 30, @0x00E20 `2b 13 ff ff`
 *        "He is holding something...", @0x00E26 `2b 14 ff ff` "You've taken the Prison key.",
 *        @0x00E2C `22 04 ec 01 | 22 03 78 01` Merkflags, ..., @0x00E42 Slot 5 -> sub07;
 *        sub07 @0x00E4E = der andere Text (msg 18).
 *      RE2 ROOM4050 @0x00F1A `4e 06 02 31 01 00 74 dc d4 95 7e 09 aa 05 49 00 01 00 be 00 ff 01` —
 *        die Leiche haelt die Wolf Medal ueber den normalen Item-AOT, md1 = 0xFF (KEIN Weltmodell);
 *        Text-AOT auf demselben Rechteck @0x00F82 msg 8 "He's holding something.".
 *    Roy gibt OHNE Frage; die Ja/Nein-Wahl ist NUTZER-VORGABE ("bis man die Munition annimmt").
 *
 * 4. "Nein" / Inventar voll -> nichts gemerkt, das naechste Untersuchen bietet wieder an. Das ist die
 *    Original-Regel des Modals (FUN_8001db28 Zustand 7 @0x8001e048): voll `bltz v0,0x8001e0ec`
 *    @0x8001e054, Nein `andi v0,v0,0x1` @0x8001e068 + `bne v0,zero,0x8001e0ec` @0x8001e06c ->
 *    Zustand 8 ohne Einfuegen und ohne Bit; nur der Ja-Weg nullt die Zone (`sb zero,0(v1)`
 *    @0x8001e090), fuegt ein (`jal 0x8004dc4c` @0x8001e0c4, a1 = `lhu a1,2(s1)` Menge) und setzt das
 *    Zone-9-Bit (`jal 0x8004ef90` @0x8001e0d0, a0 = 0x800b0fd6+162 = 0x800b1078 = Bank 9 @0x8001e0d4).
 * ---------------------------------------------------------------------------------------------
 */
#ifndef RE15_LEICHE_H
#define RE15_LEICHE_H

#include <stdint.h>

/* Raeume als RE15_ROOM_BASE (re15_gameflow.h:82, id & 0xFFF0): die unterste Hex-Ziffer ist die
 * RDT-Variante (Leon 0 / Elza 1, @0x800397e4 `srl a0,a0,31` + @0x800397ec `addu`). ROOM1111 und
 * ROOM1231 tragen die Leichen-Ereignisse und Nachrichten bytegleich an denselben Offsets. */
#define RE15_LEICHE_1110_RAUM       0x1110u
#define RE15_LEICHE_1230_RAUM       0x1230u

/* Die ORIGINAL-Nachricht, an deren Message_on der Tausch haengt:
 *   ROOM1110/1111 sub02 @0x00D04 `2b 00 ff ff` -> 0
 *   ROOM1230/1231 sub21 @0x014BC `2b 0a ff ff` -> 10
 * Beide Stellen sind die einzigen Oeffner ihrer Nachricht (Dossier 3.2, msgref-Zensus). */
#define RE15_LEICHE_1110_MSG_ORIG   0
#define RE15_LEICHE_1230_MSG_ORIG   10

/* Port-Nachrichten (re15_msg_install_text). VERTRAG.md 1.3 teilt Spur F die Ids 20..23 zu.
 * Frei nachgeprueft: ROOM1110/1111 Nachrichtensektion @0x0D54, off[0] = 0x14 -> 10 Eintraege (0..9);
 * ROOM1230/1231 @0x14F4, off[0] = 0x18 -> 12 Eintraege (0..11). Der Raum-Lader beschreibt nur
 * diese, 20..23 bleiben uns. PSX-Tabelle MSG_TABLE_N = 32 (msg_common.c) > 23. */
#define RE15_LEICHE_1110_MSG_LANG   20
#define RE15_LEICHE_1110_MSG_KURZ   21
#define RE15_LEICHE_1230_MSG_LANG   22
#define RE15_LEICHE_1230_MSG_KURZ   23

/* "Munition genommen" — Bank 9 (Zone-9, 0x800b1078, @0x8001e0d4), VERTRAG.md 1.1 Spur F: 61 / 62.
 * Frei nachgeprueft: im Auslieferungsstand 85 von 256 Bits belegt, Block 53..84 frei (Zensus
 * bank9: Item_aot_set +18, Ck/Set Bank 9, Opcode 0x59 Bank 9 = 0, Flag-AOTs alle Bank 5); Port
 * belegt 53..56 (Sicherung, Diary, Memory Card, Granate). Bank 9 steht im Speicherstand
 * (re15_savedata.h flags[RE15_FLAG_ZONES][...]), kein neues Format. */
#define RE15_LEICHE_1110_BIT        61
#define RE15_LEICHE_1230_BIT        62

/* Item 0x15 "H. Gun Bullets": Namens-Blob DAT_800c4a28 ueber die Offsettabelle DAT_800c495c
 * (inventory_common.c s_item_names[0x15]); dieselbe Id wie die 38 ausgelieferten
 * Handgun-Munitions-Saetze, z.B. ROOM1110 @0x00B18 `50 07 09 31 00 00 d2 dd 32 00 20 03 20 03 15 00
 * 1e 00 e7 00 01 00` (Item_aot_set, Typ 0x15, Menge 30). */
#define RE15_LEICHE_ITEM            0x15

/* Menge 15 — PORT-WAHL (es gibt keine Original-Leiche mit Munition): die haeufigste
 * H.-Gun-Bullets-Packung in BEIDEN Spielen — RE1.5 Typ 0x15 22 von 38 Saetzen (je 19 Plaetze ohne
 * Varianten-Doppel: 11 x 15, 8 x 30), RE2 Leon Id 0x14 37 von 44 (Zensus `munition`, Dossier 3.6).
 * Geprueft und NICHT Handgun: RE2 ROOM4050 aot 7 (Shotgun Shells x7, action 1). Nutzer "einmal ...
 * Munition" = eine Packung. Ins Inventar kommen 7: die Nutzer-Halbierung jeder Welt-Munition
 * (re15_pickup_menge_nutzer, 2026-09-20) und das Stapeln (2026-09-26) sitzen im Modal selbst. */
#define RE15_LEICHE_MENGE           15

/* Message_on-Haken (scd_vm.c op_message_on, HINTER dem Stimmen-Riegel): uebernimmt (Raum 1110/1111,
 * Nachricht 0) bzw. (1230/1231, Nachricht 10) — installiert den passenden Text unter der Port-Id,
 * queued die Stimme, oeffnet ihn mit der Maske des Original-Opcodes und merkt das Angebot.
 * Rueckgabe 1 = uebernommen (Aufrufer: t->pc += 4; return 1), 0 = nicht zustaendig. */
int re15_leiche_message_on(const uint8_t *pc, uint32_t pause_mask);

/* Je Spielbild aus re15_game_step, VOR dem Item-Modal-Freeze-Gate: ist der lange Leichen-Text zu,
 * oeffnet hier das Aufnahme-Modal. Rueckgabe 1 = Modal in diesem Bild aufgemacht. */
int re15_leiche_tick(void);

/* Pruefhaken fuer Riegel: die eingebackenen Texte (.msg-Rohbytes); raum = 0x1110/0x1230 (Varianten
 * erlaubt), kurz = 0 (mit Angebot) / 1 (nach der Annahme). NULL bei fremdem Raum. */
const uint8_t *re15_leiche_text(unsigned raum, int kurz, int *out_len);

/* Pruefhaken: wartet gerade ein Angebot auf das Schliessen des Textes? */
int re15_leiche_angebot_offen(void);

#endif /* RE15_LEICHE_H */
