# Spur A — Rolltor ROOM1050: Sicherung einsetzen mit Nahansicht, Tor erst danach

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_rolltor`, Zweig `r34n/rolltor`.
Werkzeuge: `re15_port/tools/r34n_a/` (belege.py, raumgraph.py, lauf.sh), Sonde
`re15_port/tests/unit/probe_r34n_a_rolltor.c` (+ `probes/r34n_a_rolltor.cmake`), Belegbilder `A_belege/`.

## 0 Kurzfassung

* **Kasten und Schalter sind eine Stelle.** AOT 7 (sub00 @0x0C22, Rechteck x 16800..17600 z -8950..-8150)
  steht direkt vor dem Wandschrank, den Cut 3 klein (rote Leuchte) und Cut 7/8 gross zeigen. Am Schalter
  ist IMMER Cut 3 aktiv (RVD-Gruppen; gemessen im echten Spiel, §2).
* **RE1.5 hat das Raetsel VOLLSTAENDIG — in ROOM2060.** Generator-Frage -> Ja -> `Ck(3,144,0)` ->
  Sperrsatz (sub12 @0x015BA..@0x015D6); Kasten: Satz -> "Will you use the Fuse?" -> Ja -> `Cut_chg 8` ->
  `Set(3,144,1)` -> `Cut_chg 9` -> "You've used the Fuse." -> `Cut_chg 10` + `Cut_auto 1` (sub18/sub19).
  ROOM1050 wird nach genau diesem Muster gebaut, mit den ORIGINALSAETZEN aus ROOM2060 (msg 4/5 byte-genau).
* **Ablauf:** Schalter -> ausgelieferte Frage (msg 0) -> Nein: nichts. Ja + keine Sicherung: Nahansicht
  Cut 7 + msg 2 "I need a fuse to run the shutter." -> zurueck Cut 3, Tor bleibt zu. Ja + Sicherung:
  Cut 7 + msg 2 -> "Will you use the Fuse?" -> Ja: Sicherung raus, Bit (9,63), Cut 8 + "You've used the
  Fuse." -> Cut 3. Danach faehrt der Schalter den AUSGELIEFERTEN sub02 (Frage, Tor, Ton) unveraendert.
* **Gegenstand aus dem Inventar: RE1.5 kann es nicht** (kein Opcode der 95er-Tabelle; der Feuerloescher
  bleibt im Original ewig im Inventar — der Port nimmt ihn nur ueber die RE2-Wegwerf-Abfrage heraus).
  Ziel = RE2 Retail `Sce_item_lost` (0x62, Tabelle 0x800a74c8[0x62] -> LAB_800585e4): Platz suchen,
  Id/Anzahl/Flags nullen, nachruecken — genau so benutzt RE2 Einsetz-Gegenstaende (room1110 sub04,
  room10B0 sub10, room60D0 sub06 = die RE2-Sicherungsfassung).
* **Bauweg:** portseitiger SCD-Bytecode, den die vorhandene VM ausfuehrt (Original-Opcodes, Original-
  Zeitsemantik). Zwei Haken-Zeilen in `scd_vm.c` (Ereignis-Umleitung in `scd_event_fire`, Opcode 0x62),
  alles andere in `engine/src/rolltor_1050.c`. KEIN AOT-Slot noetig (die Vertragsslots 11/12 sind in
  ROOM1051 ORIGINAL belegt).
* **Plan an der echten VM gefahren** (Sonde, 6 Faelle 1050/1051, keine Fremd-Opcode-Position, Faden endet
  sauber; Gegenprobe: ohne registrierten Opcode 0x62 wird das Einsetzen STILL verschluckt) und
  **88 Byte-Belege** gegen die ausgelieferten Dateien geprueft (belege.py, 88/88 OK).
* **Korrektur am Vor-Dossier:** `Cut_replace 7,8` waere wirkungslos — Cut_replace etikettiert nur die
  RVD-Tabelle um (@0x80040414..@0x800404a8), Cut 7/8 sind reine Skript-Cuts. Die Nahansicht waehlt
  `Cut_chg 7`/`Cut_chg 8`, wie ROOM2060 selbst (sub18 @0x0168C, sub19 @0x016A2).
* Kein Softlock: Sicherung ab Spielbeginn erreichbar (1170 -> 1130 -> 1150), nicht wegwerfbar, nicht
  kombinierbar (Item 0x40 @0x800750a8: 0 Paare), Kiste = nur Ablage.

## 1 Nutzerwortlaut + Lesart

Woertlich (AUFTRAG.md, erster Punkt):

> Ich möchte das das Tor in ROOM 1050 nicht mehr einfach so geöffnet werden kann, sondern die CUT und
> der Text mit der Sicherung dort aktiviert werden soll. Also das Tor soll erst geöffnet werden können,
> wenn die Sicherung eingesetzt werden. Für die Sicherung und Nachricht soll es eine Nahansicht geben.

Lesart, Satz fuer Satz:

1. **"nicht mehr einfach so geoeffnet … sondern die CUT und der Text mit der Sicherung dort aktiviert"** —
   "die CUT" = die im Auslieferungsstand unerreichbare Nahansicht des Sicherungskastens (Cut 7 leer /
   Cut 8 eingesetzt, Kameratabelle @0x140/@0x160), "der Text mit der Sicherung" = ROOM1050 msg 2
   "I need a fuse to run the shutter." (@0x0ED2, im SCD nie aufgerufen). "statt einfach so geoeffnet" =
   an der Stelle, an der heute das Tor aufgeht (sub02 nach dem Ja), kommen Nahansicht + Text.
2. **"erst geoeffnet werden koennen, wenn die Sicherung eingesetzt"** — "eingesetzt" ist ein dauerhafter
   Zustand (Bank-9-Bit 63, VERTRAG §1.1), danach verhaelt sich der Schalter wie ausgeliefert.
3. **"Fuer die Sicherung und Nachricht soll es eine Nahansicht geben"** — ZWEI Nahansichten: zur
   Nachricht (Cut 7 + msg 2) und zum Einsetzen (Cut 7 -> Cut 8).

Mehrdeutigkeiten, aufgeloest (Vertrag §3.2: Wortlaut + Bilder, begruendet):

* **Reihenfolge Schalterfrage / Nahansicht.** Die ausgelieferte Schalterfrage bleibt ZUERST
  ("It's a shutter switch. / Will you push it?", msg 0), die Sperre sitzt HINTER dem Ja. Gruende:
  (i) der Nutzer ersetzt woertlich das "geoeffnet werden", nicht das Fragen; (ii) msg 2 sagt "to RUN the
  shutter" = Antwort auf einen Bedienversuch; (iii) RE1.5 hat genau diese Reihenfolge VOLLSTAENDIG im
  Schwester-Raetsel ROOM2060: Generator-Frage msg 0 -> Ja -> `Ck(3,144,0)` -> msg 1 "I need to insert the
  missing fuse before I can operate this." (sub12 @0x015BA..@0x015D6); (iv) der Nein-Zweig der Frage
  bleibt bytegleich zum Auslieferungsstand.
* **Wo eingesetzt wird.** Kasten und Schalter sind DIESELBE Stelle (§3.2) — keine zweite Zone. Die
  Einsetz-Frage folgt in derselben Nahansicht auf msg 2, wie in ROOM2060 sub18 (Zustandssatz msg 3 ->
  "Will you use the Fuse?" msg 4).
* **"Nachricht" = msg 2** (Original). Die Einsetz-Saetze sind ROOM2060 msg 4 / msg 5 BYTE-GENAU
  (Originalsaetze desselben Raetseltyps, gruener Gegenstandsname per `05 01 … 05 00`) — keine
  Neuformulierung, keine Glyphen-Zusammensetzung noetig.
* **Nach dem Einsetzen.** Der Schalter faehrt den ausgelieferten sub02 (Frage -> Ja -> Tor faehrt mit
  Ton). Eine Nahansicht mit Cut 7 kann danach nicht mehr entstehen; die einzige Nahansicht nach dem
  Einsetzen ist Cut 8 im Einsetz-Ablauf selbst (§4). Das Tor oeffnet NICHT automatisch beim Einsetzen —
  in ROOM2060 sind Einsetzen und Bedienen ebenfalls zwei Handlungen.

## 2 Ist-Zustand im Port (gemessen)

Lauf `re15_port/tools/r34n_a/lauf.sh ist|nein` (echte `re15_pc.exe` unter eigenem Namen, beschleunigter
Renderer, RE15_FRAMEDUMP, Bau dieses Baums = master cf0e68ba). Neues Spiel -> Debug-JUMP ROOM1050 (setzt
Cut 0, @0x8001d818-20) -> Spieler startet IN der RVD-Zone 0->1 und GEHT ueber die Uebergaenge 1->2 und
2->3 an den Schalter (so laeuft die Kamera wie im Spiel; nach einem Debug-JUMP bleibt sie sonst auf
Cut 0 stehen — gemessen in Runde 33, `analysis/befunde_runde33/tuer1120_werkzeug/lauf.sh` Kopf).

| Bild | Ereignis (state.log / debug.log) |
|---|---|
| F380 / F480 | Kamera 1 -> 2 -> 3 (RVD @0x200 z 4200..5200, @0x23C z -2300..-1300) |
| F590 | Spieler (16556,-8313), Blick Ost (rot -32), **cam=3** — steht vor dem Wandschrank |
| F605 | Viereck -> AOT 7 -> `[msg] room=1050 id=0`, Frage tippt, pf=FF800007 (Maske 0xFF80) |
| ~F700 | "It's a shutter switch. / Will you push it? ▸Yes No" vollstaendig |
| F758 | Viereck (Ja) -> F761 `Cut_chg(3)` (sub02 @0x0CD0), Letterbox, Spieler gesperrt (pm=2) |
| F770..F950 | Rolltor faehrt hoch (Obj 0), F1002 Steuerung zurueck (pm=0) |
| Nein-Lauf | F760 Rechts = "No", ~F776 Viereck: Frage zu, nichts passiert, kein Cut-Wechsel |

Belegbilder: `A_belege/ist_schalter_ja_tor_faehrt.jpg` (F590 Leon am Kasten hinter der Tafel, F750 Frage,
F900 Tor faehrt, F1000 offen). Befund: **Das Tor oeffnet ohne jede Bedingung.** Die Sicherung (Item 0x40,
Fundstelle Hebetisch ROOM1150, `sicherung_1150.c`, Zone-9-Bit 53) hat im Spiel keine Verwendung —
`RE15_SICHERUNG_ITEM` wird genau einmal benutzt (Aufnahme-Modal). Cut 7/8 und msg 2 sind unerreichbar.

Zusatzmessung Nahansicht: `RE15_FORCE_CUT=7`, Spieler am Schalter (16556,-8313): Cut 7 zeigt den
Wandschrank mit roter Leuchte, der Spieler ist NICHT im Bild (`A_belege/cut7_erzwungen_spieler_am_schalter.jpg`),
der Nachrichtenkasten liegt unten wie ueblich. Die Nahansicht braucht also keine Spieler-Umpositionierung.

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

Alle Byte-Angaben dieses Abschnitts prueft `re15_port/tools/r34n_a/belege.py` gegen die ausgelieferten
Dateien (Stand: **88 OK, 0 FEHLT**).

### 3.1 ROOM1050 / ROOM1051 — was ausgeliefert ist

| Was | Datei-Offset | Bytes | Bedeutung |
|---|---|---|---|
| Schalter-Zone | sub00 @0x0C22 | `2c 07 03 31 00 00 a0 41 0a dd 20 03 20 03 ff 00 18 02 00 00` | Aot_set Slot 7 sce 3 sat 0x31, x 16800..17600 z -8950..-8150, Ereignis 2 (sub02) |
| nur solange zu | sub00 @0x0C1E | `21 03 79 00` | Ck(3,121,0) |
| Frage | sub02 @0x0CAC | `2b 00 80 ff` | Message_on 0, Maske 0xFF80 |
| Ja? | sub02 @0x0CB6 | `21 0c 1f 00` | Ck(12,31,0) |
| Tor offen | sub02 @0x0CBA | `22 03 79 01` | Set(3,121,1) |
| Sperre / Letterbox | sub02 @0x0CC8 / @0x0CCC | `22 02 07 01` / `22 01 1b 01` | Set(2,7,1) / Set(1,27,1) |
| Blick der Fahrt | sub02 @0x0CD0 | `29 03` | Cut_chg 3 |
| Fahrgeraeusche | sub02 @0x0CD2 / @0x0CE2 / @0x0D3C | `36 02 0c …` / `36 02 0a …` / `36 02 0b …` | Se_on snd0 0x0C / 0x0A / 0x0B |
| Rueckgabe | sub02 @0x0D74..@0x0D82 | `22 02 07 00 22 01 1b 00 3c 01 2e 01 00 00 42` | Sperre aus, Cut_auto 1, Plc_ret |
| Nachricht | msg @0x0ED2 | `04 02 25 00 4a 41 41 40 … 57 01 00` | "I need a fuse to run the shutter." — kein `2b 02` im SCD |
| Nahansicht | Kamera @0x140 / @0x160 | erste 28 Byte gleich, pri 0x518 / 0x51C | Cut 7 / Cut 8, Kamera (16171,-2733,-8185) -> (18240,-1974,-8706) |

Kunst Cut 7 gegen Cut 8 (ROOM105.BSS Scheibe 7/8, Engine-Dekoder `probe_bg_dump`), selbst nachgemessen:
Kanalabweichung >4: **2761 Pixel, bbox x123..268 y16..181**; >8: 1544 (x137..221 y64..178); >24: 742
(x188..206 y64..162) — alles im Kasten. (Die Memory-Zahl "2317 Pixel" passt zu keiner dieser Schwellen; der
Befund "nur der Kasten unterscheidet sich" haelt.)

ROOM1051 (Elza): sub02 @0x0CC8..0x0DA4 **bytegleich** zu ROOM1050 sub02, Schalter-Zone @0x0C4C gleich,
msg 0 @0x0DEC und msg 2 @0x0E68 gleich, Kameratabelle (alle 10 Cuts) gleich. ⛔ ROOM1051 main00 belegt
Slot 11 (`2c 0b 03 31 …` @0x0C0E, Leiche -> sub03) und Slot 12 (`50 0c 09 31 …` @0x0C22, geparkte SIG P228).

### 3.2 Kasten und Schalter sind derselbe Ort — und am Schalter gilt immer Cut 3

* **Bild:** `A_belege/kunst_cut3_cut7_cut8.jpg` (ROOM105.BSS, Engine-Dekoder `probe_bg_dump`): In Cut 3
  (Gang Richtung Sued) haengt links an der OSTWAND hinter der fahrbaren Tafel ein kleiner Kasten mit
  gruener und roter Leuchte (gelber Rahmen, x 124..141 y 68..96) — derselbe Wandschrank, den Cut 7 gross
  zeigt. Im Ist-Lauf steht Leon bei F590 genau davor.
* **Geometrie:** Kollision SCA @0x550: Ostwand-Zelle i1 x 17200..18100 (z -15500..10200). Schalter-Zone
  x 16800..17600 ragt in die Wand = der Schalter sitzt AN der Wand. Kamera Cut 7 steht bei x=16171,
  z=-8185 und blickt nach Osten auf (18240, -8706) — auf dieselbe Wandstelle.
* **Cut am Schalter:** RVD @0x1B0: Heimatzone von Cut 3 x 12700..18200 z -24800..1700 (@0x250); 3->4
  erst bei z -12500..-11500 (@0x278), 4->3 bei z -11500..-10500 (@0x2A0). Die Rolltor-Zelle i19
  (z -10600..-10200) trennt: solange das Tor zu ist, erreicht man den Schalter nur von Norden = Cut 3.
  Gemessen: cam=3 ab F480 bis zum Schalter.
* Cut 7/8/9 haben in der RVD nur Anker-Eintraege auf einem Blindrechteck (@0x32C/@0x340/@0x354,
  x 19600..22100 z -6900..-4900, to 0) = reine Skript-Cuts, per Zone nie erreichbar.

### 3.3 RE1.5-Vorbild: das vollstaendige Sicherungs-Raetsel ROOM2060 (STAGE2)

```
sub00 @0x010A2  21 03 6c 00                     Ck(3,108,0)  Sicherung nicht genommen -> Slot 9 = Text msg 3
      @0x010C2  21 03 90 00 2c 09 03 31 … 18 12  Ck(3,144,0)  nicht eingesetzt -> Slot 9 sce 3 -> sub18
      (sonst)                                    Slot 9 = Text msg 6 "The fuse is in place." + Cut_replace 5,11 / 6,10
sub12 @0x015BA  2b 00 ff ff  02 00               Generator-Frage, Evt_next
      @0x015C4  21 0c 1f 00                      Ck(12,31,0) Ja
      @0x015CC  21 03 90 00  2b 01 ff ff  02 00  NICHT eingesetzt -> msg 1 "I need to insert the missing fuse
                                                 before I can operate this."      <== Vorbild fuer msg 2 in 1050
sub18 @0x01678  2b 03 ff ff  02 00  2b 04 ff ff  msg 3 "One of the fuses is missing..." -> msg 4 "Will you use the Fuse?"
      @0x01688  21 0c 1f 00  29 08               Ja -> Cut_chg 8 (Nahansicht VORHER), dann Set(5,10/11/5): sub01
                                                 @0x012B4..@0x01332 fuehrt per Richtungstaste (Sce_key_ck) Objekt 4
                                                 und startet bei Member_cmp==7 + Aktion sub19 (Einsetz-Feinbedienung)
sub19 @0x0169E  22 03 90 01  29 09               Set(3,144,1) eingesetzt, Cut_chg 9 (Nahansicht NACHHER)
      @0x016B8  46 09 01 31 06 00 ff ff 00 00    Slot 9 -> Text msg 6
      @0x016C8  2b 05 ff ff  02 00  29 0a  3c 01  msg 5 "You've used the Fuse.", Cut_chg 10, Cut_auto 1
```
Texte (Offsettabelle @0x1758): msg 4 @0x1855 (32 B) `04 02 33 45 48 48 00 55 4b 51 00 51 4f 41 00 50 44 41
00 05 01 22 51 4f 41 05 00 1b 03 02 01 00`, msg 5 @0x1875 (29 B) `04 02 35 4b 51 3a 52 41 00 51 4f 41 40 00
50 44 41 00 05 01 22 51 4f 41 05 00 57 01 00`. Rahmen identisch mit ROOM1090 msg 8/9 (Feuerloescher,
@0x2934/@0x2961). ROOM2061 = ROOM2060.
**Einordnung (memory beta-zu-retail):** Frage-Ablauf am Weltobjekt ist in RE1.5 FERTIG (ROOM2060,
ROOM1090 sub06, ROOM10D0 sub20) -> RE1.5 massgeblich. Kein Se_on in sub18/sub19 -> Einsetzen ist stumm.
Die Feinbedienung (Objekt 4 per Richtungstaste) wird NICHT uebernommen: ROOM1050 hat fuer den Kasten
kein bewegliches Modell (nOmodel=2: obj 0 Rolltor, obj 1 Raum-Prop), der Zustandswechsel ist dort
GEMALT (Cut 7 -> Cut 8); ohne Modell gaebe es nichts zu fuehren.

### 3.4 Gegenstand aus dem Inventar — RE1.5 kann es nicht, RE2 kann es

* **RE1.5:** Die SCD-Tabelle `PTR_LAB_800744a8` hat 95 Eintraege (letzter @0x80074620 -> 0x80042b04,
  = 0x5E); keiner entfernt einen Gegenstand (Zensus include/re15_item_discard.h §4: Keep_Item_ck 0x5E =
  LAB_80042b04 -> FUN_80013278, kein Praedikat, 0 Vorkommen). Besitz prueft RE1.5 ueber das Zone-9-
  Genommen-Bit: ROOM10D0 sub20 @0x019C0 `21 09 34 01` (Blue Keycard), ROOM1000 sub01 @0x0D00
  `21 09 86 01` -> Set(3,133) (Feuerloescher).
* **Feuerloescher (die gestellte Frage):** ROOM1090 sub06 @0x02702..@0x0272A — msg 7, msg 8 "Will you use
  the Fire Extinguisher?", Ck(12,31,0), Set(2,7,1), Sleep 10, Set(3,129,1), Set(3,132,1), Aot_on 3 (Selbst-
  Tuer). **Kein Inventar-Zugriff: im Original bleibt der Loescher fuer immer im Inventar.** Der Port nimmt
  ihn ueber die RE2-Wegwerf-Abfrage heraus: `gen/discard_sites.inc` `{0x1090, 9, 0x31}` haengt an ROOM1090
  sub03 @0x02502 `2b 09 ff ff` ("You've used the Fire Extinguisher.") -> "You don't need this key any more.
  Discard it?" (BSS-Skript [6] @0x800C508B, Fortsetzung wie RE2 LAB_80051718).
* **RE2 Retail:** `Sce_item_lost` = Opcode **0x62**, Tabelle 0x800a74c8 + 0x62·4 = **0x800a7650 ->
  LAB_800585e4**, selbst disassembliert:
  ```
  800585fc lbu  a0,1(v0)          Item-Id = pc[1]
  80058600 jal  0x800696cc        Platz suchen
  80058608 bltz v0,0x80058634     kein Treffer -> nur Nachruecken
  80058618 sb   zero,0x4a3c(at)   Id := 0     (0x800d4a3c + Platz*4)
  80058624 sb   zero,0x4a3d(at)   Anzahl := 0
  80058630 sb   zero,0x4a3e(at)   Flags := 0
  80058634 jal  0x80069714        nachruecken
  80058644 addiu v1,v1,2          Satzlaenge 2, Rueckgabe 1 (@0x80058640)
  ```
  RE2 benutzt ihn genau fuer "Gegenstand in ein Weltobjekt einsetzen", in der Nahansicht (Rohbytes der
  .scd): room1110 sub04 +0x04 `22 02 07 01` Sperre, +0x14 `29 07` Nahansicht, +0x38 `62 4a` Sce_item_lost +
  `09 0a 1e 00`, +0x40 `29 06 3c 01` zurueck; room10B0 sub10 +0x0C `29 09 62 33`, +0x3A `29 03 3c 01`;
  room60D0 sub06 +0xDC `62 4d 47 01` (Fuse Case). Besitz fragt RE2 ueber `Keep_Item_ck` 0x5E ->
  0x800584f0 (`jal 0x800696cc`, Treffer? `nor`/`srl 31`) = Inventar-Suche.
* **Port-Gegenstueck (vorhanden):** `re15_inv_find_item` = RE1.5 FUN_8004dfec (@0x8004dff0 Anzahl,
  @0x8004e048 -1), `re15_inv_remove_slot` = RE1.5-Verbrauch @0x8004aef0/@0x8004af0c/@0x8004af28 `sb zero`
  + @0x8004af2c `jal 0x8004dadc` (nachruecken) — dieselbe Folge wie RE2 @0x80058600..@0x80058634.
* **Einordnung:** Entfernen ist in RE1.5 UNFERTIG -> RE2-Ziel (Sce_item_lost). Die Wegwerf-Abfrage passt
  NICHT: sie fragt nach einem SCHLUESSEL ("this key"), und die Sicherung ist im Kasten sichtbar (Cut 8) —
  im Inventar bliebe sie ein Widerspruch zum Bild. Besitz: RE1.5s Genommen-Bit gilt nur ohne Kiste; der
  Port hat die RE2-Kiste -> Besitz = Inventar-Suche (RE2 Keep_Item_ck-Semantik).
* Item 0x40 kann nicht wegkombiniert werden: Eigenschaftszeile @0x800750a8 `01 00 00 00 88 4c 07 80 00 00 00
  00` (Paarzeiger = Leerzeiger 0x80074c88, 0 Paare; Gegenprobe Item 0x24 @0x80074f58: 6 Paare).

### 3.5 ⛔ Korrektur: Cut_replace ist NICHT der Mechanismus der Nahansicht

Vor-Dossier (`befunde_2026-09-27/sicherung-verdrahtung.md` §1.3) und Kopf von
`tests/unit/test_room1050_sicherung.c` sagen "der Mechanismus ist Cut_replace 7,8". Selbst disassembliert,
LAB_80040414:
```
80040424 lw   v0,0x800ac778      RDT-Kopf
80040434 lw   a3,40(v0)          RDT+0x28 = RVD-Tabelle (NICHT die Kameratabelle +0x24)
8004044c..800404a8               Tauschschleife ueber cam_from (+2) / cam_to (+3), Schritt 20 (@0x80040498)
800404b8..800404c8               Anker-Fixup (jal FUN_800142f4 nur, wenn der Anker jetzt a traegt)
```
Cut_replace etikettiert nur die ZONEN um; ein ausdrueckliches `Cut_chg n` zeigt weiter Bild n (Kamera
@0x60+n·32, BSS-Scheibe n·0x10000). ROOM2060 tauscht damit die NORMALEN Raumblicke 5/11 und 6/10 (Kasten
im Hintergrund), die Nahansicht waehlt es mit `Cut_chg 8` (sub18 @0x0168C) und `Cut_chg 9` (sub19
@0x016A2). In ROOM1050 sind 7/8 reine Skript-Cuts (§3.2) -> `Cut_replace 7,8` haette KEINE sichtbare
Wirkung. Es gibt auch keinen Normal-Cut mit gruener Leuchte (Cut 3 zeigt den Kasten rot, §7).

### 3.6 Die Opcodes des Plans im RE1.5-Original

| Opcode | Handler | Belegte Instruktionen |
|---|---|---|
| 0x29 Cut_chg | 0x800402a0 (Tabelle @0x8007454C) | @0x800402d4 `ori 0x100` Auto-Kamera AUS; @0x800402e4 `sb` alter Cut -> 0x800b3f7b; @0x800402fc neuer Cut; @0x80040310 pc+2 |
| 0x2A Cut_old | 0x8004032c (@0x80074550) | @0x8004033c alter Cut; @0x80040378 `-257` Auto AN; pc+1 |
| 0x2B Message_on | 0x800404f4 (@0x80074554) | @0x80040500 a1=0x300, @0x80040504 Id, @0x80040508/@0x8004051c Maske<<16, pc+4 |
| 0x3C Cut_auto | 0x800403ac (@0x80074598) | pc[1]==1 -> Bit 0x100 loeschen (@0x800403d8), pc+2 |
| 0x02 Evt_next | 0x8003f258 | pc+1 (@0x8003f260), Ertrag 2 (@0x8003f26c) |
| 0x21 Ck | 0x8003fcf4 | Bank-Tabelle @0x8003fd24 -> 0x80074664; Bank 9 = 0x800b1078, Bank 12 = 0x800b8520 |

Warum der Rueckweg `Cut_chg 3` + `Cut_auto 1` ist und nicht `Cut_old`: Cut_old stellt den Cut VOR dem
LETZTEN Cut_chg her (0x800b3f7b). Im Einsetz-Zweig folgen zwei Cut_chg (7, dann 8) — Cut_old fuehrte
zurueck auf 7. ROOM1051 sub03 (dieselbe Raumgeometrie) kehrt aus der Leichen-Nahansicht genau so zurueck:
@0x0DC0 `29 03`, @0x0DC2 `3c 01`; am Schalter ist Cut 3 der einzig moegliche Vor-Cut (§3.2).

## 4 Soll-Verhalten (Zeitlinie)

Bildzahlen: **gemessen** mit der Sonde `probe_r34n_a_rolltor` (geplanter Bytecode an der echten VM, echter
ROOM1050-Aufbau; Bild 0 = Faden gestartet = Bild nach der Aktionstaste; Antworten jeweils im ersten
moeglichen Bild, Schreibmaschine 2 Bilder/Glyphe wie ausgeliefert). Im Spiel kommt die Lesezeit des
Spielers hinzu. Ergebnisse 1050 und 1051 identisch.

**A — Schalter, KEINE Sicherung im Inventar, (9,63)=0, Tor zu**

| Bild | Kamera | Anzeige / Zustand |
|---|---|---|
| 0..79 | Cut 3 | msg 0 "It's a shutter switch. / Will you push it?" tippt, ab 79 Ja/Nein |
| Nein | Cut 3 | Ende (Bild 82), nichts veraendert — wie ausgeliefert |
| 81 | **Cut 7** | Ja: Sperre (2,7), Nahansicht leer/rot, msg 2 "I need a fuse to run the shutter." tippt |
| 149 | **Cut 3** | msg 2 geschlossen -> Rueckweg, Auto-Kamera an, Sperre aus. Tor zu, (3,121)=0, (9,63)=0, SCA-Zelle 19 solide |

**B — Schalter, Sicherung im Inventar**

| Bild | Kamera | Anzeige / Zustand |
|---|---|---|
| 0..79 | Cut 3 | msg 0 wie A |
| 81 | Cut 7 | msg 2 wie A |
| 149 | Cut 7 | msg 20 "Will you use the **Fuse**?" (Name gruen) tippt, dann Ja/Nein |
| Nein (196) | Cut 3 | zurueck, Sicherung bleibt, (9,63)=0 |
| 196 | **Cut 8** | Ja: Sicherung aus dem Inventar (Sce_item_lost), (9,63)=1, Nahansicht beide gruen, msg 21 "You've used the **Fuse**." |
| 240 | Cut 3 | msg 21 geschlossen -> Rueckweg. Tor noch zu (3,121)=0 |

**C — Schalter nach dem Einsetzen ((9,63)=1)**: der ausgelieferte sub02 unveraendert: Frage -> Ja ->
Cut_chg 3, Letterbox, Se_on 0x0C/0x0A/0x0B, Tor faehrt (F761..F1002 im Ist-Lauf), (3,121)=1, Slot 7 tot.
Eine Nahansicht Cut 7 entsteht nie wieder.

**D — Laden/Speichern/Wiederbetreten:** (9,63) liegt in `g_game.flags` und wird komplett gespeichert
(`re15_savedata.c:210` memcpy / `:274` zurueck). Wiederbetreten: sub00 installiert Slot 7 (solange
(3,121)=0), die Weiche entscheidet beim Ausloesen ueber (9,63) -> nach dem Laden gilt C bzw. A/B.

## 5 Bauplan

### 5.1 Bauweg — begruendet

Portseitig eingespielter SCD-Bytecode, den die vorhandene VM ausfuehrt: Cut_chg, Message_on, Evt_next,
Ifel_ck/Ck, Set, Cut_auto, Endif, Evt_end laufen durch dieselben Handler wie jedes Raumskript
(Zeitsemantik, Nachrichten-Freeze, Ja/Nein, Kamera, Sperre). Ein C-Zustandsautomat muesste genau das
nachbauen. Der Bytecode wird NICHT in die RDT gepatcht (Linie des Ports), sondern beim Ausloesen von
Ereignis 2 statt sub02 gestartet. Zwei neue Bausteine:
1. **Weiche** in `scd_event_fire`: ROOM1050/1051, Ereignis 2, (9,63)=0, (3,121)=0 -> Port-Programm
   "mit" oder "ohne" je nach Inventar (RE2 Keep_Item_ck-Semantik, zum Zeitpunkt des Ausloesens — im
   Raum kann sich der Besitz nicht aendern: keine Kiste, keine Fundstelle, kein Wegwerfen in ROOM1050).
2. **Opcode 0x62** = RE2 Sce_item_lost, NUR fuer Port-Bytecode (liegt der PC nicht im Port-Programm, bleibt
   das Verhalten von op_unknown: Satzbreite 1, `s_opcode_sizes[0x62]`). Praezedenz fuer Port-Opcodes:
   0xFE Dbg_text (`s_opcode_sizes[0xFE] = 5`, `op_dbg_text`).

Kein AOT-Slot, kein Prop, keine neue Flag-Bank. Nachrichten 20/21 werden beim Ausloesen eingesetzt
(`re15_msg_install_text`), dann gilt es an JEDEM Raumstart-Weg (Tuer und Boot/CONTINUE) ohne weiteren Haken.

### 5.2 Dateien und Haken-Zeilen

| Datei | Art | Inhalt |
|---|---|---|
| `re15_port/include/re15_rolltor.h` | NEU | Konstanten (5.4), `const uint8_t *re15_rolltor_ereignis(uint16_t raum, uint8_t ereignis);`, `int re15_rolltor_op_item_lost(scd_thread_t *t);`, Sonden-Zugaenge `re15_rolltor_programm(int mit, int *len)`, `re15_rolltor_meldung(uint8_t id, int *len)` |
| `re15_port/engine/src/rolltor_1050.c` | NEU | Bytecode `k_ohne` / `k_mit` (5.3), Texte 20/21 (ROOM2060 msg 4/5), Weiche, Opcode 0x62 mit PC-Schranke, Protokollzeile `[rolltor]` (PC-only) |
| `re15_port/engine/src/scd_vm.c` | HAKEN 1 | in `scd_event_fire` nach der Schranke `if (event_id >= RE15_RDT_MAX_SUB_SCD \|\| !s_current_rdt) return -1;` die Zeile `const uint8_t *pc = s_current_rdt->sub_scd[event_id];` ersetzen durch `const uint8_t *pc = re15_rolltor_ereignis((uint16_t)g_current_room_id, event_id); /* Spur A */ if (!pc) pc = s_current_rdt->sub_scd[event_id];` |
| `re15_port/engine/src/scd_vm.c` | HAKEN 2 | `register_opcodes()`: `s_op_table[0x62] = re15_rolltor_op_item_lost;` (+ `#include "re15_rolltor.h"`). `s_opcode_sizes[0x62]` bleibt 1 |
| `re15_port/tests/unit/test_r34n_a_rolltor.c` | NEU (Riegel) | aus der Sonde: Faelle A/B/B-Nein/C/1051 ueber `scd_event_fire(2)` (echter Haken), plus Herkunftsmarke der Bytes (msg 20/21 == ROOM2060 @0x1855/@0x1875) und PC-Schranke (0x62 ausserhalb = pc+1) |
| `re15_port/tests/unit/probes/r34n_a_rolltor.cmake` | ERWEITERN | `add_test(unit_r34n_a_rolltor …)`; Sonde bleibt ohne Test |
| `re15_port/tests/unit/test_room1050_sicherung.c` | ANPASSEN | Teil (6) "Durchspielbarkeit" faehrt kuenftig ZUERST Fall B (Sicherung einsetzen), dann den ausgelieferten sub02 -> (3,121)=1, Zelle 19 frei; Kopf-Absatz "Mechanismus = Cut_replace" nach §3.5 berichtigen |
| `re15_port/tools/local_build.sh` | ANPASSEN | `RE15_MIN_TESTS` 428 -> 429 |
| `re15_port/tools/r34n_a/lauf.sh` | ERWEITERN | Modi `ohne`, `mit` (RE15_GIVE=0x40:1), `mit_nein`, `danach` (RE15_SET_FLAG_AT=9:63) fuer die Abnahme (§6) |

Keine Aenderung an: `scd_room_setup.c`, `main.c`, `aot_common.c`, `msg_common.c`, `game_step_common.c`,
RDTs, `gen/discard_sites.inc` (msg 21 steht in keiner RDT -> der Generator sieht sie nie; der Vorentscheid
`re15_discard_besitz_vor_nachricht(0x1050, 21)` findet keine Stelle).

### 5.3 Der Bytecode (Sonde = Plan, bis auf `62 40` an +0x28)

```
k_ohne (38 B)                                   k_mit (68 B)
+00 2b 00 80 ff  Message_on 0 (0xFF80)          +00 2b 00 80 ff  Message_on 0
+04 02 00        Evt_next                       +04 02 00        Evt_next
+06 06 00 1a 00  Ifel_ck -> +24                 +06 06 00 38 00  Ifel_ck -> +42
+0A 21 0c 1f 00  Ck(12,31,0) Ja                 +0A 21 0c 1f 00  Ck(12,31,0) Ja
+0E 22 02 07 01  Set(2,7,1)                     +0E 22 02 07 01  Set(2,7,1)
+12 29 07        Cut_chg 7                      +12 29 07        Cut_chg 7
+14 2b 02 ff ff  Message_on 2                   +14 2b 02 ff ff  Message_on 2
+18 02 00        Evt_next                       +18 02 00        Evt_next
+1A 29 03        Cut_chg 3                      +1A 2b 14 ff ff  Message_on 20 (Frage)
+1C 3c 01        Cut_auto 1                     +1E 02 00        Evt_next
+1E 22 02 07 00  Set(2,7,0)                     +20 06 00 14 00  Ifel_ck -> +38
+22 08 00        Endif                          +24 21 0c 1f 00  Ck(12,31,0) Ja
+24 01 00        Evt_end                        +28 62 40        Sce_item_lost(0x40)  [Port-Opcode]
                                                +2A 22 09 3f 01  Set(9,63,1)
                                                +2E 29 08        Cut_chg 8
                                                +30 2b 15 ff ff  Message_on 21
                                                +34 02 00        Evt_next
                                                +36 08 00        Endif
                                                +38 29 03        Cut_chg 3
                                                +3A 3c 01        Cut_auto 1
                                                +3C 22 02 07 00  Set(2,7,0)
                                                +40 08 00        Endif
                                                +42 01 00        Evt_end
```
Blocklaengen nach dem Muster sub02 @0x0CB2 `06 00 d0 00` (Blockende = pc+4+Laenge = hinter dem Endif).
Alle Opcodes halbwortgerichtet (`02 00` = Evt_next + Nop wie @0x0CB0/@0x0CB1). Die Sonde hat jeden
geplanten PC-Stand bestaetigt (fremd=0) und dass beide Nein-Zweige genau am Blockende landen.

### 5.4 Konstanten-Tabelle

| Konstante | Wert | Beleg bzw. Kennzeichnung |
|---|---|---|
| Raeume | 0x1050, 0x1051 | ROOM1051 sub02/Schalter/msg 0/msg 2/Kamera bytegleich (belege.py) |
| Ereignis der Weiche | 2 | Slot-7-Nutzlast `ff 00 18 02` (sub00 @0x0C30..@0x0C33); sce-3-Handler 0x800430f0: @0x800430fc `lhu a0,0(v0)` Bedingung, @0x80043100 `lbu a1,3(v0)` sub, @0x80043104 `jal 0x8003ee3c` |
| Tor-offen-Flag | (3,121) | sub00 @0x0C1E `21 03 79 00`, sub02 @0x0CBA `22 03 79 01` |
| Eingesetzt-Bit | Bank 9 Bit 63 | PORT-WAHL (VERTRAG §1.1 Spur A 63/64; RE1.5 hat in STAGE1 keine Sicherung). Bank 9 = 0x800b1078 (@0x80074688), gespeichert |
| Reserve | Bank 9 Bit 64 | unbenutzt, bleibt Reserve |
| Gegenstand | 0x40 "Fuse" | `re15_sicherung.h` RE15_SICHERUNG_ITEM (DEBUG.BIN Namenstabelle @0x800C495C) |
| Frage Schalter | msg 0, Maske 0xFF80 | sub02 @0x0CAC `2b 00 80 ff` (unveraendert) |
| Ja | Ck(12,31,0) | sub02 @0x0CB6; Bank 12 = 0x800b8520 (@0x80074694) |
| Sperre | Set(2,7,1) / Set(2,7,0) | ROOM1051 sub03 @0x0DA4 / @0x0DD2 (Nahansicht im selben Raum) |
| Nahansicht vorher | Cut_chg 7 | NUTZER-VORGABE ("Nahansicht fuer … Nachricht"); Kamera @0x140; Rolle wie ROOM2060 sub18 @0x0168C |
| Sperrsatz | msg 2, Maske 0xFFFF | ROOM1050 msg @0x0ED2; Maske wie ROOM2060 sub12 @0x015D0 `2b 01 ff ff` |
| Frage Sicherung | msg 20 = 32 B | ROOM2060 msg 4 @0x1855 byte-genau; Id nach VERTRAG §1.3; Maske 0xFFFF wie sub18 @0x0167E |
| Entfernen | Opcode 0x62, 2 B, Id 0x40 | RE2 0x800a74c8[0x62] -> 0x800585e4 (@0x80058600/@0x80058618/@0x80058634/@0x80058644) |
| Nahansicht nachher | Cut_chg 8 | Kamera @0x160; Rolle wie ROOM2060 sub19 @0x016A2 `29 09` |
| Benutzt | msg 21 = 29 B | ROOM2060 msg 5 @0x1875 byte-genau; Maske 0xFFFF wie sub19 @0x016C8 |
| Rueckweg | Cut_chg 3 + Cut_auto 1 | ROOM1051 sub03 @0x0DC0 `29 03` / @0x0DC2 `3c 01`; Cut 3 gemessen (§2) |
| Kein Warten nach Cut 8 | — | ROOM2060 sub19 @0x016A2..@0x016C8 ohne Sleep |
| Kein Ton beim Einsetzen | — | ROOM2060 sub18/sub19 ohne Se_on (RE1.5 fertig -> massgeblich). Ja/Nein-Toene kommen aus der Dialog-FSM (CORE 4/5/6, msg_common.c:574/577, RE2 @0x80030968/@0x8003093c/@0x80030944) |
| Kein Letterbox, keine Plc-Bewegung | — | ROOM1051 sub03, ROOM2060 sub18/19 setzen (1,27) nicht; RE1.5-Vorbild ohne Einsetz-Animation (RE2 room1110 sub04 hat Plc_motion(1,6,0) — nicht uebernommen, RE1.5 fertig) |
| Slot des Fadens | erster freier 10..23 | `scd_event_fire` unveraendert (SCD_EVENT_SLOT_FIRST/LAST) |

### 5.5 Sprachaufnahmen

Keine noetig: Untersuchungs-/Gedankensaetze sind im Bestand unvertont (ROOM1090 msg 7/8/9, ROOM1050 msg 0..5
ohne Datei; `synchro/STAGE1/room1050` hat nur main08.wav). Falls gewuenscht:
`synchro/STAGE1/room1050/main02.wav`, `main20.wav`, `main21.wav` (+ dieselben unter `room1051/`).

## 6 Abnahmeplan

Echte exe, echter Eingabepfad (`lauf.sh`, Weg ueber die RVD-Uebergaenge, Viereck = Aktion/Bestaetigen,
Rechts = "No"), Bilder per RE15_FRAMEDUMP ansehen, state.log/debug.log. Je Nutzerpunkt:

| # | Nutzerpunkt | Messung | Erwartung | Gegenprobe |
|---|---|---|---|---|
| 1 | "nicht mehr einfach so geoeffnet" | `lauf.sh ohne` | Frage -> Ja -> Framedump Cut 7 (rote Leuchte) + "I need a fuse to run the shutter." -> Cut 3; Tor-Modell steht, (3,121)=0 | Ist-Lauf (§2) oeffnete das Tor bei F761..F950 |
| 2 | "Nahansicht fuer Nachricht" | dieselben Bilder | Kamera 7 genau solange msg 2 offen ist (state.log cam) | Nein-Antwort: kein Cut 7 |
| 3 | "erst geoeffnet, wenn eingesetzt" | `lauf.sh mit` (RE15_GIVE=0x40:1) | Cut 7 + msg 2 -> msg 20 -> Ja -> Cut 8 (beide gruen) + msg 21 -> Cut 3; Statusschirm ohne Sicherung; (9,63)=1 | `mit_nein`: Sicherung bleibt, (9,63)=0, naechster Versuch fragt wieder |
| 4 | "danach oeffnet es" | im selben Lauf 2. Schalterdruck, bzw. `danach` | ausgelieferter Ablauf: Letterbox, Tor faehrt, Ton-Log Se_on 0x0C/0x0A/0x0B | ohne Einsetzen niemals (3,121)=1 |
| 5 | ROOM1051 | Riegel `unit_r34n_a_rolltor` Fall 1051 | wie 1050 | — |
| 6 | Laden/Wiederbetreten | Tuer 1050->1030->1050 nach dem Einsetzen; CONTINUE-Lauf mit Karte (9,63)=1 | Schalter = ausgeliefert, keine Nahansicht 7 | Karte ohne Bit: Fall A/B |
| 7 | kein Softlock | Riegel + `test_room1050_sicherung` (6) neu | mit Sicherung erreichbar geoeffnet; Sicherung nicht wegwerfbar (kein discard-Eintrag), Kiste | — |

Zusaetzlich: Suite im eigenen Baum (429 Tests), GUI-Haken einzeln nachfahren (memory gui-tests-flattern).

## 7 Risiken, Softlocks, Wechselwirkungen

* **Softlock — Sicherung verloren?** Nein. (a) Wegwerfen: nur an `discard_sites.inc`-Stellen; die Sicherung
  steht dort nicht (ROOM2060 msg 5 vom Generator verworfen, Bedingung B); msg 21 lebt nur im Port.
  (b) Kiste: Ablage, jederzeit zurueck (STAGE1-Kisten nur ROOM1150/1151 msg 3, `re15_itembox.c:63`, keine
  Sperrliste); die Weiche sieht nur das Inventar -> "I need a fuse…" statt Frage. In ROOM1050 gibt es keine
  Kiste und keine Fundstelle — der Besitz kann sich dort zwischen Ausloesen und Frage nicht aendern.
  (c) Kombinieren: 0 Paare (@0x800750a8). (d) Tod/Laden: Flags und Inventar werden zusammen gespeichert.
* **Fundstelle erreichbar?** ROOM1150 ab Spielbeginn: ROOM1170 -> ROOM1130 (Slot 4) -> ROOM1150 (Slot 2),
  beide "immer" (raumgraph.py); der Hebetisch ist unbedingt armiert (scd_vm.c op_aot_set, ROOM1150 Slot 1).
  "No" im Aufnahme-Modal armiert neu (Runde 30 nachschliff).
* **Rolltor fuer den Fortschritt noetig?** Der Suedteil (Tueren 10A0/1090/Umkleide mit dem Feuerloescher)
  ist laut Tuergraph auch ueber ROOM10D0 -> 10F0 -> 1090 erreichbar (Ankunft ROOM1090 auf y=-9000); ob die
  obere Ebene von ROOM1090 zur Tuer nach 1050 fuehrt, ist NICHT gemessen (§8). Unabhaengig davon ist die
  Sicherung vor dem Rolltor immer erreichbar -> kein Softlock in beiden Faellen.
* **Alte Spielstaende:** (3,121)=1 aus v0.8.19 -> Tor offen, Schalter weg, Sicherung bleibt nutzlos im
  Inventar (nicht wegwerfbar). Kein Fehler, nur ein ueberzaehliger Gegenstand.
* **Spur B (Hebetisch 1150/1151):** A setzt voraus, dass die Sicherung dort erhaeltlich bleibt (Bit 53,
  `sicherung_1150.c`). B aendert nur die Bedienung — bitte Aufnahme nicht unerreichbar machen.
* **Spur D (Tuer Slot 4 nach 10A0):** keine Ueberschneidung (Slot 4 liegt suedlich des Rolltors). Braucht D
  ebenfalls Port-Bytecode ueber `scd_event_fire`, dann eine EIGENE Zeile neben HAKEN 1 — beim Zusammenfuehren
  zwei Nachbarzeilen.
* **Spur E (Dokument an der sitzenden Leiche):** Leiche an der Ostwand noerdlich des Schalters (Cut 3,
  Kollision i12 x 15700..17200 z -7850..-5950). Ueberlappt Es Zone das Schalter-Rechteck (x 16800..17600
  z -8950..-8150), gewinnt Slot 7 (Scan-Reihenfolge aufsteigend, erster Treffer verbraucht die Taste —
  scd_vm.c op_aot_set Kommentar "(a) … liegt in der Scan-Reihenfolge VOR 60").
* **VERTRAG:** Spur-A-Slots 11/12 sind in ROOM1051 original belegt (§3.1) — der Plan braucht sie nicht.
* **Unregistriertes 0x62 — GEMESSEN (Sonden-Gegenprobe, `A_belege/sonde_r34n_a_rolltor.txt`):** faellt
  HAKEN 2 weg, schiebt op_unknown um 1 (`s_opcode_sizes[0x62]` = 1), liest `40 22 09 3f 01 29 08 2b` als
  Plc_dest (Fremd-Operanden!), dann `15 ff ff 02` als Default, und erst ab dem Endif @+0x36 laeuft der Plan
  weiter. Ergebnis nach Ja/Ja: KEIN (9,63), Sicherung bleibt, kein Cut 8, keine msg 21 — das Einsetzen wird
  STILL verschluckt, und der Faden endet trotzdem sauber (die PC-Stichprobe am Bildanfang sieht es nicht).
  Der Riegel muss deshalb das ERGEBNIS des Ja/Ja-Laufs pruefen ((9,63)=1, Sicherung weg, msg 21 gezeigt),
  nicht nur, dass der Faden endet.
* **Cut 3 zeigt den Kasten immer rot** (BSS-Kunst, keine Variante). Nach dem Einsetzen bleibt die kleine
  rote Leuchte im Normalblick. Kein RE1.5-Gegenstueck (ROOM2060 hat Varianten 11/10, ROOM1050 nicht).

## 8 Offene Punkte

1. **Rote Leuchte in Cut 3 nach dem Einsetzen** (§7): nur ueber ein bearbeitetes Hintergrundbild
   loesbar (neue Kunst) — nicht Teil des Auftrags, dem Nutzer vorlegen.
2. **Ton beim Einsetzen:** RE1.5 (ROOM2060) stumm, RE2 spielt einen Raum-SE (room1110 sub04 +0x2C Se_on
   2/0x0F). ROOM1050s snd0 hat nur die drei Rolltor-SE. Plan: stumm (RE1.5 fertig -> massgeblich).
3. **Nicht gemessen:** ob ROOM1090s obere Ebene (Ankunft aus 10F0, y=-9000) zur Tuer nach ROOM1050 fuehrt —
   nur fuer die Frage "Rolltor fortschrittsnoetig?" relevant, nicht fuer einen Softlock.
4. **Nebenbefund (nicht Spur A):** `re15_tuer1120_install` steht nur am Tuerweg (scd_room_setup.c:428), nicht
   am Boot-/CONTINUE-Weg in main.c (4658..4685 ruft sicherung/irons_tisch/granate). Ob in ROOM1130 geladen
   werden kann, nicht geprueft.
5. Der Bau muss die Kopfkommentare von `test_room1050_sicherung.c` (Cut_replace-These, "keine
   Sicherungs-Bedingung") nachziehen (§3.5).
