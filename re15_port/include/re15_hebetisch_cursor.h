/*
 * RE1.5 Rebuilt — HEBETISCH-CURSOR in Irons' Buero (ROOM1150/1151): der Spieler bedient das
 * Architekturmodell mit dem Welt-Cursor der Cursor-Raetsel, statt dass es sofort aufgeht.
 *
 * Runde 34 Nacht, Spur B. Dossier analysis/befunde_runde34_nacht/B_hebetisch.md (Ermittlung §1-§8,
 * Umsetzung §9), Gegenpruefung B_hebetisch.gegenpruefung.md (Auflagen 1-9).
 *
 * ⛔ NUTZER-VORGABE (Runde 34 Nacht, woertlich, AUFTRAG.md zweiter Punkt): "Bei Irons Office -
 * ROOM 1150 - moechte ich - beim Modell - nicht das das Modell einfach direkt aufgeht, sondern das
 * wir unseren Cursor haben und mit den navigieren koennen. Wenn man irgendwo hin klickt, wo nichts
 * passieren soll - soll der Text kommen "Nothing happened". Wenn man unten rechts auf die Kuppel
 * klickt, die ja, danach aufgeht, dann soll der normale Ablauf geschehen. Die Kuppel geht auf, etc.
 * Dafuer soll es das Klick Geraeusch geben, wie eben auch bei den Generator in ROOM 11F0".
 * RE1.5 hat an dieser Stelle KEINEN Cursor (der Ausloeser @0x00D7E ist im Auslieferungsstand sogar
 * abgeschaltet, sce 0). Der ABLAUF ist also neu; jede AUSFUEHRUNG (Modell, Schritt, Tasten, Kamera,
 * Ton, Textform) ist aus RE1.5 bzw. RE2 belegt, die wenigen Port-Wahlen sind als solche markiert.
 *
 * ============================================================================================
 * MECHANIK — der Cursor HAELT sub04 an, statt vor ihm zu laufen (Dossier §5.1 Nr. 1)
 * ============================================================================================
 * sub04 (ROOM1150.RDT @0x0F96..@0x10B6, ROOM1151 um -0x22 verschoben, sonst Byte fuer Byte gleich)
 * laeuft wie bisher an: Set(2,0,1) @0x0F96 (Spieler steht), Set(2,2,1) @0x0F9A (KI steht), Se_on
 * @0x0FA2, Sleep 5, Cut_chg 4 @0x0FB2, Pos_set @0x0FB4 (Plattform in den Tisch, y = -305), Sleep 5.
 * Der Port haelt den Thread dann VOR dem `For 15` @0x0FC0 (1151: @0x0F9E) an, das die Kuppel
 * oeffnet — die Kuppel liegt in Cut 4 genau jetzt geschlossen UNTEN RECHTS (Dossier §2.2). Warum
 * halten und nicht vorschalten: Cut_chg LAB_800402a0 merkt den ANGEZEIGTEN Cut
 *     800402c0  lbu a1,4068(a1)     ; 0x800b0fe4 = angezeigter Cut
 *     800402e4  sb  a1,16251(at)    ; 0x800b3f7b = gemerkter Cut (Quelle von Cut_old)
 * und Cut_old FUN_8004032c stellt ihn her (8004033c lbu a0,16251(a0) / 80040364 sh a0,4068(at)).
 * Zeigte der Port Cut 4 schon vor sub04, merkte @0x0FB2 die 4, und Cut_old @0x10B2 liesse die
 * Kamera nach der Fahrt auf dem Loch-Bild stehen (gemessen, Dossier §3.2).
 *
 * Nur der SPIELERWEG armiert: die GENERIC-Ausgabe des ACTION-Scans (game_step_common.c,
 * scd_event_fire(4) mit gestartetem Thread) ruft re15_hebetisch_cursor_aktion. Die Mess-Haken
 * RE15_FIRE_AOT und direkte scd_event_fire(4)-Aufrufe der Sonden laufen OHNE Cursor wie bisher.
 *
 * ⛔ Auflage 1 der Gegenpruefung: der Cursor tickt IM VM-TAKT — Bewegen, Druck, Treffertest,
 * Text, Klick und Abbruch laufen im Halte-Aufruf aus op_for (scd_vm.c). Damit gelten dieselben
 * Schranken wie fuer das SCD-getriebene 11F0-Vorbild: kein Takt unter Menue/Item-Modal/Wegwerf-
 * Abfrage (main.c) und unter RE15_PAUSE_SCD (scd_vm_tick, FUN_8003f038 @0x8003f040-4c), 30 Hz
 * auch im 60-Bilder-Modus.
 *
 * ============================================================================================
 * DER CURSOR = der Cursor der RE1.5-Cursor-Raetsel ("unser Cursor", Nutzer)
 * ============================================================================================
 * Modell/Textur: ROOM11F0.RDT Prop 0 (Prop-Tabelle @0x0240): MD1 @0x001928 (5556 B), TIM @0x018DAC
 * (33312 B, 8bpp 128x256, CLUT @VRAM(0,480)) — bytegleich in acht Raeumen (11F0, 4020, 30E0, 1080,
 * 2040, 5050, 1100, 1230; Gegenpruefung (b)). Eingebacken: engine/src/gen/hebetisch_cursor.inc
 * (tools/r34n_b/cursor_export.py, nur PC).
 * Lage: Obj_model_set ROOM11F0 sub00 @0x00E54
 *     2d 00 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 00 00 00 00 00 00 ...
 *   obj 0, Typ 4 (pc[2]), x = 0xb39e = -19554, y = 0, z = 0x589c = 22684, Drehung 0/0/0.
 * Typ-4-Anhebung FUN_8002c18c:
 *     8002c234  lbu v1,8(s1) / 8002c23c bne v1,4 / 8002c24c addiu v0,v0,-900 / 8002c250 sw v0,96(s1)
 *   -> gezeichnet bei y = 0 - 900; die Oberseite (49 Vierecke bei Modell-y -900) liegt bei Welt-y -1800.
 * Bewegen: ROOM11F0 sub01 fragt die GEHALTENEN virtuellen Tasten ab (Sce_key_ck 0x51 LAB_80042920,
 *     8004293c  lw v0,-14488(v0)    ; 0x800ac768 = virtuell GEHALTEN)
 *   @0x01098 `51 01 01 00` UP    -> Evt_exec sub02 @0x0109C -> sub02 `2f 02 c8 00` Speed_set(2,+200)
 *   @0x010B0 `51 01 04 00` DOWN  -> sub03 `2f 02 38 ff` Speed_set(2,-200)
 *   @0x010C8 `51 01 02 00` RIGHT -> sub04 `2f 00 c8 00` Speed_set(0,+200)
 *   @0x010E0 `51 01 08 00` LEFT  -> sub05 `2f 00 38 ff` Speed_set(0,-200)
 *   jede Richtung einzeln (vier unabhaengige Abfragen, je ein eigener Thread mit eigener
 *   Geschwindigkeit; Add_speed LAB_80040f40 addiert nur, 80040f48..80040f7c) -> diagonal moeglich,
 *   KEINE Randgrenze (gemessen: rechts gehalten verlaesst der 11F0-Cursor das Bild, Dossier §2.3).
 * Abbildung: ueber die Kamera Cut 10 von ROOM11F0 (Kameratabelle @0x0060 + 10*32 = @0x01A0:
 *   flag 0, fov 26684, pos (-19628,-17442,22616), tgt (-19628,15928,22617) = Nadir) — dieselbe
 *   "Cursor-Buehne", ueber die RE1.5 den Cursor in ALLEN Cursor-Raeumen zeigt (Gegenpruefung (b):
 *   1080 Cut 1, 4020 Cut 4, 30E0 Cut 2, 2040 Cut 14, 5050 Cut 4, 1100 Cut 12, 3050 Cut 15). Das Bild
 *   liegt ueber dem Cut-4-Bild von ROOM1150 — gleiche Pixel, Groesse (23,7 x 22,3 px) und
 *   Geschwindigkeit (2,67 px je Bild) wie in 11F0; Start in der Bildmitte (160,119).
 * Licht: der Lichtsatz, mit dem die Raum-Prop-Schleife den Cursor in 11F0 unter Cut 10 beleuchtet
 *   (main.c: licht_cut = g_re15_active_cut) — ROOM11F0.RDT RDT+0x2C -> @0x0588, 40 B je Cut,
 *   Cut 10 @0x0718: `01 01 01 01 80 80 80 80 80 80 80 80 80 3f 3f 3f d0 07 ...` = drei gerichtete
 *   Lichter (2000,2000,2000) Farbe 0x80, Umgebung 0x3f; eingebacken wie MD1/TIM. BAU-BEFUND: mit
 *   dem im Plan vorgesehenen neutralen Tint 0x80 lagen die 524 hellen 11F0-Cursorpunkte zwar
 *   deckungsgleich, aber 206 weitere Punkte waren hell statt dunkel (die abgewandten Seitenflaechen,
 *   Dossier §9.7) — der Plan hatte nur die HELLEN Farben gemessen.
 * Tiefe: PORT-WAHL — die eigene Kameratiefe minus 65536, also vor jedem Raum-Dreieck und jeder
 *   PRI-Maske (render_pc.c sortiert fallend), untereinander wie in 11F0 nach Tiefe.
 *
 * ============================================================================================
 * DRUCK, TREFFER, KLICK, TEXT, ABBRUCH
 * ============================================================================================
 * Druck: virtuelle Aktionstaste 0x0040 (<- SQUARE, Preset-Tabelle @0x80073dbc[6] = 128 = roh 0x80),
 *   wie ROOM11F0 sub01 @0x01106 `51 01 40 00`. PORT-WAHL: die FLANKE (DAT_800ac76c, Opcode 0x52
 *   LAB_8004295c: 80042978 lw v0,-14484(v0)) statt des gehaltenen Worts — 11F0 verhindert die
 *   Wiederholung ueber das Zellenbit (sub06 Set(5,1,0)), hier gibt es zwei Ausgaenge (Text/Fahrt).
 * Treffer: Heisspunkt = Bild der Cursor-Oberseite (x, -1800, z) = das Kreuz, Bezugspunkt wie der
 *   Zellstempel @0x80042f5c (prueft die OBJEKTLAGE x/z). Gegen die konvexe Huelle von Deckel
 *   (Prop 1 MD1 @0x138D4, Prop 2 @0x13B88) + Podest (Prop 0 @0x11E40) unter Cut 4 (@0x00E0) bei
 *   Plattform (-20700,-305,-17460) @0x0FB4 — Engine-Projektion (probe_r34n_b_messung), gerendert
 *   gegengeprueft (99,53 %, Dossier §3.6). Deckel + Podest = EIN sichtbares Achteck (Auflage 5).
 *   Der Druck wird VOR der Bewegung desselben Takts geprueft: getroffen wird, was im Bild steht
 *   (11F0: der Zellstempel stammt vom Vorbild-Ende, AUTO-Scan FUN_800436a8 @0x8001ce1c).
 * Klick (nur auf der Kuppel): re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK) = RE2-Raum-SE Gruppe 2 /
 *   0x0A, ROOM2130.RDT @0x01192 `36 02 0a 01 00 00 9b a0 00 fc f4 d3` — derselbe Aufruf wie beim
 *   11F0-Schalter (scd_vm.c op_sce_key_ck). NUTZER-VORGABE "wie ... Generator in ROOM 11F0". Die
 *   Bank laedt audio_pc.c ohne Raumbedingung nach (shared_assets/RE2/PANEL2130.*).
 *   Ausserhalb der Kuppel KEIN Klick: 11F0 toent ausserhalb einer Zelle nicht (gemessen m5), und das
 *   RE1.5-Vorbild fuer einen Fehldruck im Cursor, ROOM4020 sub12 @0x00B14, hat keinen Se_on.
 * Text bei Fehldruck: Vorbild ROOM4020 sub12 @0x00B14 (Zelle Slot 9 @0x00750 -> Member_cmp(15==9)
 *   @0x00888 + `51 01 40 00` @0x00892 -> `04 ff 18 0c` @0x00896): `2b 01 ff ff` @0x00B44 =
 *   msg 1 @0x0BAA "The button doesn't respond..." mit Maske 0xffff, stumm, danach ist der Cursor
 *   wieder aktiv. Hier derselbe Ablauf mit dem neuen Satz (NUTZER-VORGABE "Nothing happened") ueber
 *   den Untersuchungsweg re15_scd_show_message (sce-1-Handler LAB_80043084:
 *     80043098  lhu a3,2(v0) / 800430a4 sll a3,a3,16 -> Maske 0xffff0000) — unvertont wie jeder
 *   RE1.5-Untersuchungstext (Auflage 3: dieser Weg reiht keine Sprache ein).
 *   Textform `04 02` + Glyphen + `57` + `01 00` ("Nothing happened.", 17 Glyphen, 123 px, eine Zeile):
 *   Glyphen "Nothing" ROOM1000.RDT @0x00D24, " happened" ROOM3001.RDT @0x0199D, '.' ROOM1000 @0x00D33,
 *   Kopf/Ende ROOM1000 @0x00D22/@0x00D34 ("Nothing unusual."). Gegen 4020 msg 1 (`04 02 08 ...` +
 *   "...") bewusst OHNE fuehrendes 0x08 und mit EINEM Punkt (Auflage 6): STAGE1 fuehrt 0x08 nur in
 *   6 von 360 Untersuchungstexten, ROOM1150 msg 0/1/3 (@0x1316/@0x1385/@0x1426) nie; einzeilige
 *   Texte schliessen 256x mit '.' gegen 39x '...' (tools/r34n_b/textform_zensus.py); der Nutzer
 *   schreibt den Satz ohne Auslassungspunkte. Nachrichten-Id 20 (VERTRAG §1.3; ROOM1150 hat 15
 *   Texte, Sektion @0x012F8, ROOM1151 4, @0x010EC).
 * Abbruch (Auflage 4, Vorbild 11F0 EXIT -> sub17 @0x015FA): virtuelle Abbruchtaste 0x8000
 *   (<- CROSS, Preset @0x80073dbc[15] = 64 = roh 0x40), dieselbe Taste, die Texte und Menues
 *   abbricht — PORT-WAHL, Grund: Cut 4 hat kein gemaltes EXIT. Wirkung: der haltende sub04-Thread
 *   springt auf die ORIGINAL-Aufraeumbytes von sub04 (1150 @0x109A, 1151 @0x1078 = Halt + 0xDA):
 *     2e 03 00        Work_set(3,0)             Plattform
 *     00              Nop
 *     32 00 24 af 00 b1 cc bb   Pos_set (-20700,-20224,-17460) = wieder geparkt
 *     22 05 00 00     Set(5,0,0)
 *     22 02 00 00     Set(2,0,0)                Spieler frei
 *     22 02 02 00     Set(2,2,0)                KI frei
 *     2a              Cut_old                   zurueck in die gemerkte Raumkamera
 *     00 01 00        Nop, Evt_end
 *   Am Halt liegt noch kein For-/Block-Rahmen (sub04 hat davor weder For noch If), der Sprung ist
 *   sauber. Kein Klick, kein Text. Weil der Takt in der VM laeuft, bricht das Schliessen von
 *   "Nothing happened." mit CROSS NICHT ab (die VM steht im Schliessbild noch unter RE15_PAUSE_SCD,
 *   Gegenpruefung (c)7).
 * Erneutes Aktivieren: jede Aktion am Tisch bringt den Cursor wieder (sub04 setzt Slot 1 nie zurueck,
 *   Dossier §4.4); die Faecher bleiben nach der Aufnahme leer (Flags (9,53)/(9,56)).
 * Laden/Speichern: fluechtig (kein Bank-9-Bit, kein Speicherstandfeld). re15_hebetisch_cursor_install
 *   setzt beim Raumaufbau zurueck — an BEIDEN Stellen (scd_room_setup.c Tuer/Sprung und main.c
 *   Boot/CONTINUE, Auflage 2; Original EIN Raumlader FUN_800396fc, `jal 0x800396fc` @0x8001d5ac
 *   LOAD und @0x8001d988 Tuer).
 * PSX-Ziel: kein Halt (der Cursor braucht den PC-Zeichner) — die Fahrt laeuft dort wie bisher.
 */
#ifndef RE15_HEBETISCH_CURSOR_H
#define RE15_HEBETISCH_CURSOR_H

#include <stdint.h>
#include "re15_scd.h"   /* scd_thread_t */

/* ---- Raum und Ausloeser ------------------------------------------------------------------- */
#define RE15_HC_RAUM_JOHN        0x1150u   /* Auftrag; Record @0x00D7E */
#define RE15_HC_RAUM_ELZA        0x1151u   /* Record @0x00D7E identisch, sub04 um -0x22 verschoben */
#define RE15_HC_EREIGNIS         4u        /* Nutzlast `ff 00 18 04` @0x00D8C (einziges `18 04` im Raum) */
#define RE15_HC_CUT              4         /* sub04 @0x0FB2 `29 04` (1151 @0x0F90) */

/* ---- Halte-Stelle (Dossier §3.7, Zensus 2 Treffer in 240 RDTs) --------------------------- */
#define RE15_HC_SIG_LEN          20        /* `29 04 | 32 00 24 af cf fe cc bb | 09 0a 05 00 | 0d 00 18 00 0f 00` */
#define RE15_HC_HALT_IN_SIG      14        /* Halt = For 15 @0x0FC0 (1151 @0x0F9E) = Signatur + 14 */
#define RE15_HC_AUFRAEUM_ABSTAND 0xDA      /* Aufraeumbytes @0x109A - Halt @0x0FC0 (1151 @0x1078 - @0x0F9E) */
#define RE15_HC_AUFRAEUM_LEN     28        /* @0x109A..@0x10B5 bis einschliesslich Evt_end `01 00` */

/* ---- Cursor (ROOM11F0) --------------------------------------------------------------------- */
#define RE15_HC_START_X          (-19554)  /* ROOM11F0 sub00 @0x00E54 pc[10..11] `9e b3` */
#define RE15_HC_START_Z          22684     /* pc[14..15] `9c 58` (Rueckstellung sub17 @0x01602) */
#define RE15_HC_OBJ_Y            0         /* pc[12..13] `00 00` */
#define RE15_HC_TYP4_ANHEBUNG    (-900)    /* FUN_8002c18c @0x8002c24c `addiu v0,v0,-900` (Typ 4, pc[2]) */
#define RE15_HC_KREUZ_Y          (-900)    /* Oberseite im Modell: 49 Vierecke bei y = -900 (MD1 @0x001928) */
#define RE15_HC_SCHRITT          200       /* sub02..05 @0x012F6/@0x01302/@0x0130E/@0x0131A `c8 00` / `38 ff` */
#define RE15_HC_TASTE_UP         0x0001u   /* sub01 @0x01098 `51 01 01 00` -> sub02: +z */
#define RE15_HC_TASTE_RIGHT      0x0002u   /* sub01 @0x010C8 `51 01 02 00` -> sub04: +x */
#define RE15_HC_TASTE_DOWN       0x0004u   /* sub01 @0x010B0 `51 01 04 00` -> sub03: -z */
#define RE15_HC_TASTE_LEFT       0x0008u   /* sub01 @0x010E0 `51 01 08 00` -> sub05: -x */
#define RE15_HC_TASTE_DRUCK      0x0040u   /* sub01 @0x01106 `51 01 40 00`; Flanke = PORT-WAHL (s.o.) */
#define RE15_HC_TASTE_ABBRUCH    0x8000u   /* Preset @0x80073dbc[15] <- roh CROSS; PORT-WAHL (Auflage 4) */

/* Abbildungskamera = ROOM11F0.RDT Kameratabelle Eintrag 10 @0x001A0 (32 Byte):
 *   00 00 3c 68 54 b3 ff ff de bb ff ff 58 58 00 00 54 b3 ff ff 38 3e 00 00 59 58 00 00 */
#define RE15_HC_KAM_FOV          26684
#define RE15_HC_KAM_POS_X        (-19628)
#define RE15_HC_KAM_POS_Y        (-17442)
#define RE15_HC_KAM_POS_Z        22616
#define RE15_HC_KAM_TGT_X        (-19628)
#define RE15_HC_KAM_TGT_Y        15928
#define RE15_HC_KAM_TGT_Z        22617

/* ---- Text ---------------------------------------------------------------------------------- */
#define RE15_HC_TEXT_ID          20        /* VERTRAG §1.3 Spur B; < MSG_TABLE_N 32 */
#define RE15_HC_TEXT_MASKE       0xFFFF0000u /* sce-1 @0x80043098/@0x800430a4; 4020 `2b 01 ff ff` @0x00B44 */

/* ---- Trefferflaeche (Deckel + Podest unter Cut 4, 320x240, im Uhrzeigersinn) --------------- */
#define RE15_HC_KUPPEL_N         12
/* Die Ecken stehen in hebetisch_cursor_1150.c (k_kuppel); unit_r34n_b_kuppel rechnet sie mit der
 * Engine-Projektion aus den ausgelieferten Modellbytes nach. */

/* ---- Zeichnen (nur PC) --------------------------------------------------------------------- */
#define RE15_HC_TIM_SLOT         28        /* main.c RE15_TIM_SLOT_PROP(8) = 26 + (8 - 6); obj_id 8 =
                                            * VERTRAG §1.5 Spur B; in 1150/1151 frei (nOmodel 4,
                                            * Port-Props 4..7 = Slots 8/9/26/27) */
#define RE15_HC_LICHT_CUT        10        /* Lichtsatz ROOM11F0 Cut 10 @0x0718 (s.o.) */
#define RE15_HC_TINT_OHNE_NORMALE 0x80     /* nur falls eine Flaeche keine Normale traegt (neutral) */
#define RE15_HC_TIEFE_VERSATZ    65536     /* PORT-WAHL: vor allem (s.o.) */

/* ---- Zustaende / Rueckgaben ---------------------------------------------------------------- */
#define RE15_HC_AUS              0
#define RE15_HC_VERLANGT         1         /* Aktion am Tisch, sub04 gestartet, Halt noch nicht erreicht */
#define RE15_HC_AKTIV            2         /* sub04 steht vor dem For, Cursor sichtbar und bedienbar */

#define RE15_HC_WEITER           0         /* For normal ausfuehren */
#define RE15_HC_HALT             1         /* Thread steht in diesem Takt (op_for -> Yield) */
#define RE15_HC_SPRUNG           2         /* PC auf die Aufraeumbytes gesetzt (op_for -> weiter) */

/* Raumaufbau (beide Stellen, Auflage 2): Zustand AUS, Signatur-Cache leeren. */
void re15_hebetisch_cursor_install(uint16_t room_id);

/* GENERIC-Ausgabe des ACTION-Scans: `ereignis` feuerte, `thread_slot` = Rueckgabe von
 * scd_event_fire (< 0 = nicht gestartet). Armiert nur in 1150/1151, fuer Ereignis 4, bei
 * gestartetem Thread, und nur auf dem PC. */
void re15_hebetisch_cursor_aktion(uint8_t ereignis, int thread_slot);

/* Haken in op_for (scd_vm.c), VOR jeder Zustandsaenderung des For. Liefert RE15_HC_WEITER /
 * RE15_HC_HALT / RE15_HC_SPRUNG. Im Zustand AUS nur ein Vergleich. */
int re15_hebetisch_cursor_for(scd_thread_t *t, const uint8_t *raw, int raw_size);

/* Zeichnen: 1 = der Cursor ist zu zeigen, Lage in 11F0-Weltkoordinaten. Prueft defensiv, dass der
 * armierte Thread wirklich auf dem Halt steht (sonst Zustand AUS, Auflage 2). */
int re15_hebetisch_cursor_sicht(int32_t *x11f0, int32_t *z11f0);

int      re15_hebetisch_cursor_zustand(void);
unsigned re15_hebetisch_cursor_sitzung(void);   /* zaehlt die Cursor-Sitzungen (Textur-Upload) */

/* Modell->Kamera-Matrix des Cursors an (x, z) unter Cut 10 von ROOM11F0 (Typ-4-Lage y = -900). */
void re15_hebetisch_cursor_matrix(int32_t x, int32_t z, int32_t rot[9], int32_t trans[3], int *h);
/* GTE-RTPS wie main.c PROJECT_VERT; 0 = hinter der Nahebene (vz < 64). */
int  re15_hebetisch_cursor_projiziere(const int32_t rot[9], const int32_t trans[3], int h,
                                      int32_t mx, int32_t my, int32_t mz,
                                      int *sx, int *sy, int32_t *vz);
/* Heisspunkt = Bild von (x, -1800, z). */
void re15_hebetisch_cursor_heisspunkt(int32_t x, int32_t z, int *sx, int *sy);
/* 1 = Bildpunkt liegt in der Kuppel-Huelle (Rand eingeschlossen). */
int  re15_hebetisch_cursor_in_kuppel(int sx, int sy);
/* Die 12 Huellenecken (fuer Riegel/Zeichnen der Messbilder). */
const int16_t *re15_hebetisch_cursor_kuppel(void);   /* 2*RE15_HC_KUPPEL_N Werte x0,y0,x1,y1,... */

/* "Nothing happened." als .msg-Rohbytes. */
const uint8_t *re15_hebetisch_cursor_text(int *out_len);

/* Eingebackene Cursor-Bytes (nur PC; sonst NULL): MD1, TIM, Lichtsatz Cut 10 (40 B). */
const uint8_t *re15_hebetisch_cursor_md1_bytes(int *out_size);
const uint8_t *re15_hebetisch_cursor_tim_bytes(int *out_size);
const uint8_t *re15_hebetisch_cursor_licht_bytes(int *out_size);

/* Messschienen (Riegel, Integrations-Haken). */
extern unsigned g_re15_hebetisch_klick_zaehler;
extern unsigned g_re15_hebetisch_text_zaehler;
extern unsigned g_re15_hebetisch_abbruch_zaehler;

#ifdef RE15_PLATFORM_PC
/* platform/pc/src/hebetisch_cursor_pc.c — aus main.c nach der Prop-Zeichenschleife. */
void re15_hebetisch_cursor_zeichnen_pc(void);
#endif

#endif /* RE15_HEBETISCH_CURSOR_H */
