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
  kombinierbar (Item 0x40 @0x800750a8: 0 Paare), Kiste = nur Ablage. **Bau-Stufe:** das Rolltor ist der
  EINZIGE Weg in den Suedteil (ROOM1090-Laufsteg aus 10F0 hat keinen Abstieg, gemessen, §7) -> die
  Sicherung ist Pflicht; die Cut-4-Folge aus der Gegenpruefung tritt im normalen Spiel nicht auf.
* **GEBAUT (§9):** `engine/src/rolltor_1050.c` + 2 Haken in `scd_vm.c`, Riegel `unit_r34n_a_rolltor`
  (mit Mutationsprobe), echte exe 1050 + 1051 (ohne/mit/mit_nein/danach, Laden) abgenommen.

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
  Verworfene Alternative: Aktion -> sofort Nahansicht + msg 2 (ohne Schalterfrage), die Frage erst nach dem
  Einsetzen. Das entspraeche ROOM2060s KASTEN-Zone (sub18 beginnt mit dem Zustandssatz), aber ROOM1050
  hat keinen eigenen Kasten-Platz — der Platz IST der Schalter (msg 0 "It's a shutter switch"), und fuer einen
  sicherungsgesperrten SCHALTER ist sub12 das RE1.5-Muster. Der Wechsel waere ein Handgriff im Bytecode
  (Message_on 0, Evt_next, das aeussere Ifel_ck/Ck(12,31,0) und dessen Endif entfallen), falls der Nutzer
  es anders sieht.
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
**Mechanismus (Gegenpruefung, Auflage 8):** das ist kein Standort-Zufall. Der Spieler wird gegen das
Regionsviereck des AKTIVEN Cuts gecullt (`platform/pc/main.c` "Per-cut region-quad cull", byte-true
FUN_80039ca0 -> FUN_80014368, `re15_aot_point_in_quad`), und Cut 7/8 haben nur das Blindrechteck
@0x32C/@0x340 (x 19600..22100, z -6900..-4900). Jeder Ausloese-Standort am Schalter liegt ausserhalb —
Leon/Elza sind in Cut 7/8 immer unsichtbar (bestaetigt im Bau: alle Cut-7/8-Bilder in `A_belege/bau_105x_*.jpg`).

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
      @0x01688  21 0c 1f 00  29 08               Ja -> Cut_chg 8 (Panel-Nahansicht VORHER), dann Set(5,10/11/5): sub01
                                                 @0x012B4..@0x01332 fuehrt per Richtungstaste (Sce_key_ck) den ZEIGER
                                                 obj 4 und startet bei Member_cmp==7 + Aktion sub19 (Zeiger-Panel)
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

**⛔ Berichtigt (Gegenpruefung, Auflage 1):** ROOM2060s Einsetzen ist ein **Zeiger-Panel**, keine
"gefuehrte Sicherung". obj 3 (Generator-Panel) und obj 4 (Sicherungs-Panel) tragen dasselbe
Zeigermodell (TIM @0x382B0, MD1 @0x33E4, bytegleich); Cut 7 und Cut 8/9 sind SENKRECHTE Panelkameras
(pos (19572,-17442,-22584) -> tgt (19572,15928,-22583)) ueber dem geparkten Zeiger obj 4 (@0x00F12);
sub01 @0x012B4..@0x0130C fuehrt ihn per Sce_key_ck, @0x01314..@0x01332 Ck(5,11,1) + Member_cmp(0x0F==7)
+ Sce_key_ck 0x40 -> sub19 (Bild `A_belege/gegen_2060_zeigerpanel_vs_1050_weltkamera.jpg`).
Der Verzicht auf diese Feinbedienung ist **PORT-WAHL, keine Original-Adresse — Grund:** ROOM1050s
Cut 7/8 sind WELTKAMERA-Nahansichten (Kamera @0x140/@0x160, pos (16171,-2733,-8185), Blick fast
waagerecht nach Osten, kein Panelrahmen), ROOM1050.RDT hat kein Zeigermodell (nOmodel=2) und keine
sce-5-Zonen. Die RE1.5-Form fuer Weltkamera-Nahansichten ist die DIREKTE Frage: ROOM1051 sub03
(@0x0DB4 `29 09`, @0x0DB6 `2b 05 ff ff`, derselbe Raum) und ROOM1090 sub06 (@0x02702 msg 7, @0x02708
msg 8 "Will you use the Fire Extinguisher?", @0x02712 Ja -> Wirkung). Ein Zeiger ueber Cut 7 muesste
Ebene, Zonen und Zeigerlage erfinden = raten. (Dieselbe Begruendung steht im Kopf von
`engine/src/rolltor_1050.c` und in der Commit-Message des Baus.)

**Auftragspunkt 4 ("Nahansicht zeigt beim Ansehen Cut 8"):** nach dem Einsetzen gibt es KEINE weitere
Nahansicht. Das ist RE1.5-konform: ROOM2060 macht den Kasten danach zum reinen Textplatz msg 6 "The fuse
is in place." (sub00 @0x010DE, sub19 @0x016B8 `46 09 01 31 06 00 ff ff 00 00`), ohne Cut_chg. In ROOM1050
ist der Kasten zugleich der Schalter (§3.2) — der Platz faehrt danach den ausgelieferten sub02 (Frage,
Tor, Ton). Cut 8 erscheint genau einmal: beim Einsetzen, waehrend "You've used the Fuse.".

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
* **Ergaenzt (Gegenpruefung, Auflage 2) — RE1.5s Sicherung ist ein reines FLAG, nie ein Gegenstand:**
  ROOM2030 sub06 @0x01FE4 `2b 02 ff ff` "Will you take the Fuse?" -> @0x01FEE `21 0c 1f 00` Ja -> @0x01FF2
  `22 03 6c 01` Set(3,108,1) -> @0x02000 `2b 03 ff ff` "You've taken the Fuse." — kein Item_aot_set, kein
  Aot_on (Zensus: 0 Ausgaben fuer 0x40). ROOM2060 prueft nur dieses Flag (sub00 @0x010A2 `21 03 6c 00`).
  Deshalb braucht RE1.5 kein Entfernen; der Port fuehrt die Sicherung aber als Inventar-Item (Fundstelle
  Hebetisch, `sicherung_1150.c`) -> die RE2-Wahl `Sce_item_lost` ist fuer dieses Item zwingend.
* **Zwei Negativbelege, selbst disassembliert (re15_disasm.py, Bau-Stufe):**
  * Opcode 0x5E (Tabelle @0x80074620 -> 0x80042b04) ist KEIN Besitztest: `lw v0,0(v1)` 0x800aca3c,
    @0x80042b20 `ori v0,v0,0x20`, @0x80042b24 `sw`, @0x80042b30/@0x80042b34 `lbu a0,1(v0)` / `lhu a1,2(v0)`,
    @0x80042b38 `jal 0x80013278`, @0x80042b44 `ori v0,zero,0x1`, @0x80042b48 pc+4 — kein Praedikat.
  * Die Inventarauswahl schickt Nicht-Waffe/Nicht-Heilmittel in Zustand 6 (Tabelle 0x80074c28[6] @0x80074c40
    -> 0x8004b250). Der Handler (bis `jr ra` @0x8004b334) schreibt nur 0x800b25ee/0x800b25c4/0x800b25c2/
    0x800b25c3, ruft nur @0x8004b2e8 `jal 0x80027e68` (Meldung) und wartet @0x8004b300/@0x8004b308 auf
    0x800b8520 & 0x80 — kein Schreiben in die Inventarplaetze. Schluesselgegenstaende verlassen das
    RE1.5-Inventar also nie.

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
| `re15_port/tools/local_build.sh` | ~~ANPASSEN~~ **NICHT angefasst** (Auflage 6) | `RE15_MIN_TESTS` bleibt 428 im Spurzweig; die Suite laeuft mit `RE15_MIN_TESTS=429` als Umgebung. ⛔ ZUSAMMENFUEHRUNG: B (B_hebetisch.md) und C (C_generator.md) heben dieselbe Zahl — die Integration setzt sie EINMAL auf 428 + Summe der neuen Tests (A: +1 `unit_r34n_a_rolltor`) |
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

**Suite dieses Baums, Stand Ermittlung (kein Engine-/Plattform-Code geaendert — `git diff cf0e68ba..HEAD`:
nur analysis/, tools/r34n_a/, Sonde + probes/r34n_a_rolltor.cmake ohne add_test):** Volllauf
`local_build.sh test` 420/428 (728 s, parallel bauende Agenten). Die 8 roten sind GUI-Integrationshaken
mit echter exe; einzeln nachgefahren sind ALLE gruen: elza_vollstart, irons_tisch_laden, irons_tisch_bild,
sicherung_laden, boot_bg_pin im 1. Anlauf; granate_laden und irons_tisch_licht im 2.; titel_puls im 3.
(die ersten beiden Anlaeufe rot mit Bilddauern bis 139 ms — Pruefung (D) haengt an der Wanduhr).

## 7 Risiken, Softlocks, Wechselwirkungen

* **Softlock — Sicherung verloren?** Nein. (a) Wegwerfen: nur an `discard_sites.inc`-Stellen; die Sicherung
  steht dort nicht (ROOM2060 msg 5 vom Generator verworfen, Bedingung B); msg 21 lebt nur im Port.
  (b) Kiste: Ablage, jederzeit zurueck (STAGE1-Kisten nur ROOM1150/1151 msg 3, `re15_itembox.c:63`, keine
  Sperrliste); die Weiche sieht nur das Inventar -> "I need a fuse…" statt Frage. In ROOM1050 gibt es keine
  Kiste und keine Fundstelle — der Besitz kann sich dort zwischen Ausloesen und Frage nicht aendern.
  (c) Kombinieren: 0 Paare (@0x800750a8). (d) Tod/Laden: Flags und Inventar werden zusammen gespeichert.
* **Fundstelle erreichbar?** (`raumgraph.py 1 wege`, Ausgabe `A_belege/raumgraph_wege.txt`) Leon: ROOM1170 ->
  ROOM1130 (Slot 4) -> ROOM1150 (Slot 2), beide "immer". Elza (Szenario-Varianten …1, Start ueber ROOM1241 -> ROOM1031): ROOM1031 -> 1041 -> 1061 ->
  1121 -> 1131 -> ROOM1151, alle "immer" — beide Male OHNE das Rolltor. Der Hebetisch-Ausloeser ROOM1150/1151
  main00 @0x0D7E steht in keinem If und wird port-seitig unbedingt armiert (scd_vm.c op_aot_set, Slot 1);
  "No" im Aufnahme-Modal armiert neu (Runde 30 nachschliff).
* **Rolltor fuer den Fortschritt noetig? — JA, gemessen in der Bau-Stufe (Auflage 3, §9.6 (3)).** Der
  Tuergraph fuehrt ROOM10F0 Slot 1 -> ROOM1090 -> Slot 0 -> ROOM1050-Sued, aber ROOM1090 besteht aus ZWEI
  getrennten Ebenen: die Ankunft aus 10F0 liegt auf dem Laufsteg y=-9000 (Band 5, 10F0 main00 @0x00F52
  Ziel (-13600,-9000,1300)), die Tuer nach 1050 (main00 @0x0211A, Band-Gate floor=1) im Hof y=-1800. In
  KEINEM 1090-Skript gibt es einen Treppen-/Rampen-Platz (Zensus aller Aot_set/Door_aot_set/Aot_reset:
  nur Tueren 0/1/3, Slot 2 sce 1/3, Slot 3 sce 3 -> sub07; kein sce 7/12/13), und die Nordkante des
  Stegs ist fuer den SPIELER geschlossen: Band-5-Zellen p1 i5 (x -16400..-11504, z >= 3404), p1 i1
  (x -11752..-8058, z 3826..8129, u0 0x01) und p1 i4 (x -8191..-2726, z >= 3432); FUN_8003b0a4 haelt eine
  Zelle solide, wenn (Maske & u0) != 0 (@0x8003b244 `lhu` Typ|u0, @0x8003b250 `sra 24`, @0x8003b254
  `and`, @0x8003b258 `bne` -> Push), und BEIDE Spieler-Aufrufe geben Maske 1 (@0x80031d74 und @0x800384c8
  `ori a2,zero,0x1`). An der echten exe (lauf_1090.sh): Leon laeuft bei (-9900, 3358) fest = 3826 - 468
  (Spielerradius). Folge: der Suedteil (Hof 1090 mit Ada/Feuer, Umkleide 1000 mit dem Feuerloescher,
  10A0) ist NUR durch das Rolltor erreichbar -> **die Sicherung wird Pflicht**. Kein Softlock: sie liegt
  vor dem Rolltor (ROOM1150, ab Spielbeginn erreichbar).
* **Cut-4-Folge (Gegenpruefung (c), Spur D §2.3) — tritt im normalen Spiel NICHT auf.** Das Tor-Prop obj 0
  (sub00 @0x0C36) steht nur, solange (3,121)=0 (If-Zweig @0x0C1A..@0x0C58); im Else-Zweig (Tor offen)
  wird es nicht angelegt. Suedlich des Tors (Zone 3->4 @0x278 bei z -12500..-11500, hinter der Torzelle 19
  bei z -10600..-10200) steht man nur, nachdem das Tor geoeffnet wurde — also nie mit Prop. Das von D
  gemessene Bild (Cut 4 voll Torrueckseite) entsteht nur ueber Debug-Sprung/Flag-Manipulation. Dasselbe galt
  schon im Auslieferungsstand; die Sperre aendert daran nichts.
* **Alte Spielstaende:** (3,121)=1 aus v0.8.19 -> Tor offen, Schalter weg, Sicherung bleibt nutzlos im
  Inventar (nicht wegwerfbar). Kein Fehler, nur ein ueberzaehliger Gegenstand.
* **Spur B (Hebetisch 1150/1151):** A setzt voraus, dass die Sicherung dort erhaeltlich bleibt (Bit 53,
  `sicherung_1150.c`). B aendert nur die Bedienung — bitte Aufnahme nicht unerreichbar machen.
* **Spur D (Tuer Slot 4 nach 10A0):** ~~keine Ueberschneidung (Slot 4 liegt suedlich des Rolltors)~~ —
  berichtigt (Gegenpruefung): die Wechselwirkung laeuft ueber Cut 4, nicht ueber Slots/IDs; gemessen (s. o.)
  kann sie im normalen Spiel nicht auftreten. UEBERGABE AN D: (1) Ds Praemisse "Suedteil ueber ROOM1000
  erreicht" ist falsch (ROOM1000-RVD @0x190..@0x348: drei getrennte Kameragruppen, die Umkleide haengt nur am
  Suedteil); (2) auch ueber 10F0 -> 1090 kommt man NICHT in den Suedteil (Laufsteg Band 5 abgeschlossen,
  s. o.) — Ds Szene an Slot 4 setzt also immer ein offenes Rolltor voraus, (3,121)=1, kein Tor-Prop in Cut 4;
  (3) Ds Messbild `ist_cut4_rolltor_zu_offen_cut5.png` (Tor zu) ist ein Debug-Sprung-Zustand. Braucht D
  ebenfalls Port-Bytecode ueber `scd_event_fire`, dann eine EIGENE Zeile neben HAKEN 1 (§9.1 Auflage 7).
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

1. **Rote Leuchte in Cut 3 nach dem Einsetzen — BEKANNTE GRENZE (Auflage 5, keine Frage an den Nutzer):**
   Cut 3 zeigt den Kasten (x124..141 y68..96, ~17x28 px) nach dem Einsetzen weiter mit der roten Leuchte.
   ROOM105.BSS hat fuer Cut 3 keine Kunstvariante; RE1.5 loest dasselbe in ROOM2060 mit Varianten-Cuts
   10/11 per Cut_replace (sub19 @0x016C2/@0x016C5, sub00 @0x010F2/@0x010F5). Ohne neue Kunst nicht loesbar
   — im Liefertext als Grenze genannt.
2. **Ton beim Einsetzen:** RE1.5 (ROOM2060) stumm, RE2 spielt einen Raum-SE (room1110 sub04 +0x2C Se_on
   2/0x0F). ROOM1050s snd0 hat nur die drei Rolltor-SE. Plan: stumm (RE1.5 fertig -> massgeblich).
3. ~~Nicht gemessen~~ **GESCHLOSSEN (Bau-Stufe, Auflage 3):** ROOM1090s obere Ebene fuehrt NICHT zur Tuer
   nach ROOM1050 — nicht wegen des Feuers (die Emitter stehen im Hof bei x 1374..3104), sondern weil der
   Laufsteg (Band 5) keinen Uebergang zum Hof (Band 1) hat; Belege §7 und §9.6 (3). Das Rolltor ist damit
   fortschrittsnoetig, die Sicherung Pflicht, ein Softlock bleibt ausgeschlossen (Fundstelle vor dem Tor).
4. ~~Nebenbefund~~ geprueft, KEIN Befund: `re15_tuer1120_install` steht nur am Tuerweg
   (scd_room_setup.c:428), nicht am Boot-/CONTINUE-Weg (main.c 4658..4685) — aber STAGE1 speichert nur in
   ROOM1120/1150 (`re15_savepoint.c:46/47`), ein Laden startet also nie in ROOM1130. Spur A braucht den
   Boot-Weg ebenfalls nicht (Texte 20/21 werden beim Ausloesen eingesetzt, §5.1).
5. ~~Der Bau muss die Kopfkommentare von `test_room1050_sicherung.c` nachziehen~~ — erledigt (§9.2).

## 9 Umsetzung (Stufe BAU)

Stand: in Arbeit. Abschnitte werden nach jedem Teilschritt gefuellt und committet.

### 9.1 Auflagen der Gegenpruefung — abgehakt / abgelehnt

| # | Auflage | Stand | Wo / Beleg |
|---|---|---|---|
| 1 | §3.3 berichtigen (Zeiger-Panel, Verzicht als PORT-WAHL mit Grund), Auftragspunkt 4 beantworten | **erledigt** | §3.3 "Berichtigt"; Kopf `engine/src/rolltor_1050.c`; Commit-Message des Baus; Belegbytes selbst gelesen: ROOM2060 obj 3 @0x00EF0 / obj 4 @0x00F12, Slot 7 sce 5 flags 0x44 @0x0100C, Textplatz msg 6 sub00 @0x010DE / sub19 @0x016B8 |
| 2 | §3.4 ergaenzen (RE1.5-Sicherung = Flag; zwei Negativbelege) | **erledigt** | §3.4, beide Negativbelege in der Bau-Stufe SELBST disassembliert (0x80042b04..0x80042b58; 0x80074c40 -> 0x8004b250..0x8004b334) |
| 3 | §7/§8.3 schliessen, Cut-4-Folge messen, an D uebergeben | siehe §9.6 (3) | `tools/r34n_a/lauf_1090.sh`, Bilder `A_belege/bau_1090_*.jpg` |
| 4a | ROOM1051 an der echten exe | **erledigt** | `lauf.sh elza_ohne/elza_mit` ueber den echten Weg (Charakterwahl -> ROOM1241 -> ROOM1031 Tuer Slot 0 @0x01CD8 -> ROOM1051); `A_belege/bau_1051_*.jpg` |
| 4b | Statusschirm nach dem Einsetzen | **erledigt** | `lauf.sh mit` F1300 (Sicherung weg, kein Loch), `mit_nein` F1170/F1200 (Sicherung da); `A_belege/bau_1050_mit*.jpg` |
| 4c | Doppelausloesen ausschliessen | **erledigt** | je Druck genau EINE `[rolltor]`-Zeile; Viereck schliesst msg 2 (F881/882) bzw. msg 21 (F1128) — im selben und im Folgebild KEINE neue Frage (state.log `A_belege/bau_laeufe.txt`). Die zwei `[msg] id=0`-Zeilen je Frage sind Oeffnen + Entblocken derselben Wahl-Nachricht (op_message_on parkt und betritt den Opcode erneut, scd_vm.c "CHOICE message") — ebenso im Auslieferungsstand |
| 4d | Laden/Speichern reproduzierbar | **erledigt** | Riegel-Fall L (capture -> Zustand weg -> restore -> Raumaufbau -> Schalter startet sub02 @0x0CAC, Inventar ohne 0x40) + echter Kartenlauf `tools/r34n_a/lauf_laden.sh` (§9.6 (4)) |
| 4e | Wiederbetreten als Riegel-Fall | **erledigt** | Riegel-Fall W (neuer Raumaufbau mit (9,63)=1 -> sub02 @0x0CAC) |
| 4f | Cut-4-Lauf | siehe Auflage 3 | |
| 5 | §8.1 umformulieren (bekannte Grenze, keine Frage) | **erledigt** | §8.1 |
| 6 | RE15_MIN_TESTS nicht anheben oder Konflikt markieren | **erledigt (beides)** | `local_build.sh` unberuehrt, Suite mit `RE15_MIN_TESTS=429`; Zusammenfuehrungshinweis in §5.2 |
| 7 | (Soll) HAKEN 1 mit D abstimmen | **offen fuer die Integration, begruendet** | Ds Dossier (Stand `21ccd4dc`, §5 "Bauplan" leer) plant bisher KEINE Ereignis-Umleitung. Eine gemeinsame Tabelle nach `s_power_gates`-Muster passt nicht ohne Umbau: `s_power_gates` lenkt eine Ereignis-Id auf eine ANDERE RDT-Sub-Id um, A liefert einen Port-Programmzeiger (Bytecode ausserhalb der RDT). HAKEN 1 ist deshalb EINE Zeile mit EINER Funktion (`re15_rolltor_ereignis`); braucht D spaeter dasselbe, ist die Integration mit einer zweiten Zeile `if (!pc) pc = re15_<d>_ereignis(...)` konfliktfrei |
| 8 | (Soll) §2 Mechanismus der Unsichtbarkeit; echte Kette oder RE15_GIVE begruenden | **erledigt** | §2 (Regions-Cull `main.c` FUN_80039ca0 -> FUN_80014368 gegen Blindrechteck @0x32C/@0x340). RE15_GIVE gleichwertig: die Weiche liest NUR `re15_inv_find_item(0x40)` beim Ausloesen, RE15_GIVE schreibt dieselben `g_inv.slots` wie die Aufnahme; die Aufnahme selbst pinnen `unit_r31_hebetisch`, `unit_r30_sicherung_nein`, `integration_r30_sicherung_laden` |

### 9.2 Dateien und Haken

| Datei | Art | Inhalt |
|---|---|---|
| `re15_port/include/re15_rolltor.h` | NEU | Konstanten mit Beleg (Raeume, Ereignis 2 @0x0C33, (3,121) @0x0C1E/@0x0CBA, (9,63) PORT-WAHL/VERTRAG, msg 20/21, Opcode 0x62), Schnittstelle |
| `re15_port/engine/src/rolltor_1050.c` | NEU | Port-Programme `k_ohne` (38 B) / `k_mit` (68 B) Byte fuer Byte wie §5.3, Texte 20/21 = ROOM2060 msg 4/5, Weiche `re15_rolltor_ereignis`, Opcode `re15_rolltor_op_item_lost` (RE2 0x800585e4) mit PC-Schranke, Protokollzeilen `[rolltor]` (nur PC) |
| `re15_port/engine/src/scd_vm.c` | HAKEN 1 (+3 Zeilen) | `scd_event_fire`: `pc = re15_rolltor_ereignis(room, event_id); if (!pc) pc = sub_scd[event_id];` |
| `re15_port/engine/src/scd_vm.c` | HAKEN 2 (+4 Zeilen) | `register_opcodes`: `s_op_table[0x62] = re15_rolltor_op_item_lost;` (+ `#include "re15_rolltor.h"`); `s_opcode_sizes[0x62]` bleibt 1 |
| `re15_port/tests/unit/test_r34n_a_rolltor.c` | NEU (Riegel) | Faelle A/A-N/B/B-N/C (1050, 1051), W, L, Herkunft 20/21/msg 2, PC-Schranke |
| `re15_port/tests/unit/probes/r34n_a_rolltor.cmake` | ERWEITERT | `add_test(unit_r34n_a_rolltor)`; die Sonde bleibt ohne Test |
| `re15_port/tests/unit/test_room1050_sicherung.c` | ANGEPASST | (6) = (6i) ohne Sicherung zu / (6ii) Einsetzen / (6iii) ausgelieferter sub02 oeffnet; Kopf berichtigt (Cut_replace, Fundstelle) |
| `re15_port/tools/r34n_a/lauf.sh` | ERWEITERT | Modi `ohne`, `mit`, `mit_nein`, `danach`, `elza_ohne`, `elza_mit` (+ RE15_SE_DEBUG) |
| `re15_port/tools/r34n_a/lauf_laden.sh` | NEU | echter Kartenlauf Speichern (1150) -> CONTINUE -> 1050 |
| `re15_port/tools/r34n_a/lauf_1090.sh` | NEU | Auflage 3: 10F0 -> 1090 (Feuer brennt) -> Tuer Slot 0 -> 1050 Sued |

Nicht angefasst: `scd_room_setup.c`, `main.c`, `aot_common.c`, `msg_common.c`, `game_step_common.c`, RDTs,
`gen/discard_sites.inc`, `local_build.sh`. Keine AOT-Slots, keine Props, keine neue Flag-Bank; Ressourcen
aus dem VERTRAG: Bank-9-Bit 63 (64 Reserve unbenutzt), Nachrichten 20/21.

**Sprachaufnahmen (VERTRAG §1.3), vom Nutzer aufzunehmen — ohne Datei laufen die Zeilen stumm mit
Untertitel:** `synchro/STAGE1/room1050/main20.wav` ("Will you use the Fuse?"),
`synchro/STAGE1/room1050/main21.wav` ("You've used the Fuse."), wahlweise `main02.wav` ("I need a fuse to run
the shutter.", Originalsatz); fuer Elza dieselben drei unter `synchro/STAGE1/room1051/` (der Pfad traegt die
volle Raum-Id, `platform/pc/src/audio_pc.c` "synchro/STAGE%u/room%04X/").

### 9.3 Commits

(folgt)

### 9.4 Riegel + Mutationsprobe

`unit_r34n_a_rolltor` (`test_r34n_a_rolltor.c`) faehrt den ECHTEN Weg: Raumaufbau (main00 + sub00 durch
die VM), Aktion am Schalter ueber `re15_aot_scan` (AOT 7), `scd_event_fire(2)` (= HAKEN 1, wie
`game_step_common.c`), dann Bild fuer Bild Nachrichten-FSM + VM. Er prueft das ERGEBNIS (Lehre der
Sonden-Gegenprobe), nicht nur das Fadenende. Ausgabe (gruen):

```
  A    1050 ohne, Ja     Frage@79 Cut7@81 msg2@81 Cut3@149 Ende@150 | cam=3 auto=1 (2,7)=0 (3,121)=0 (9,63)=0 Z19solide=5 fremd=0
  A-N  1050 ohne, Nein   Frage@79 Cut7@-1 ... Ende@82              | (3,121)=0 (9,63)=0
  B    1050 mit, Ja/Ja   Frage@79 Cut7@81 msg2@81 msg20@149 Cut8@196 msg21@196 Cut3@240 Ende@241 | (3,121)=0 (9,63)=1 Fuse=-1
  C danach: ausgeliefert  Frage@79 ... Ende@302                   | (3,121)=1 (9,63)=1 Z19solide=0
  B-N  1050 mit, Ja/Nein  ... msg20@149 Cut8@-1 Cut3@196          | (9,63)=0 Fuse=3
  A/B  1051 identisch, C 1051 startet sub02 @0x0CC8
  W  Wiederbetreten mit (9,63)=1: Schalter -> ausgelieferter sub02 @0x0CAC
  L  Laden (capture/restore): (9,63)=1, keine Sicherung, Schalter -> sub02 @0x0CAC
  PC-Schranke: 0x62 ausserhalb des Port-Programms = pc+1, Inventar unberuehrt
```
Die Bildzahlen sind bitgleich zur Plan-Sonde (§4) — der eingebaute Weg hat die geplante Zeitsemantik.

**Mutationsprobe** (`A_belege/mutationsprobe.txt`; je Mutation einzeln gebaut, danach Rueckbau aus einer
Sicherungskopie, `cmp` bestaetigt): 
| Mutation | Ergebnis |
|---|---|
| a: HAKEN 1 raus (`pc = 0`) | ROT, 45 FAIL-Zeilen ("scd_event_fire startet NICHT das Port-Programm", Tor offen ohne Sicherung) |
| b: HAKEN 2 raus (0x62 = op_unknown) | ROT, 23 FAIL-Zeilen ("Sicherung noch im Inventar", "(9,63) nicht gesetzt", "Cut 8 / msg 21 fehlt") — genau das stille Verschlucken der Sonden-Gegenprobe |
| c: PC-Schranke raus | ROT ("0x62 im RDT-Bytecode laeuft nicht wie op_unknown", "entfernt einen Gegenstand") |
| d: Weiche ohne (9,63)-Pruefung | ROT (C: "zweiter Druck startet nicht den ausgelieferten sub02", "Tor oeffnet nach dem Einsetzen nicht") |
| e: Rueckweg `29 07` statt `29 03` | ROT ("nicht zurueck in Cut 3", 1050 und 1051) |
| f: Nahansicht Cut 8 statt 7 fuer msg 2/20 | ROT ("114 Bilder Text im falschen Cut") |
| ohne Mutation (neu gebaut) | `RESULT: OK` |

⛔ Beim ersten Anlauf der Mutationsprobe fehlte `git` im Bau-PATH, der Rueckbau per `git checkout` lief ins
Leere und die Mutationen haeuften sich — erkannt an identischen FAIL-Listen, Dateien per git wiederhergestellt,
Probe mit Datei-Sicherungskopien wiederholt (die Tabelle oben ist der zweite, saubere Lauf).

`unit_room1050_sicherung` (angepasst) prueft zusaetzlich die Durchspielbarkeit ueber den ganzen Ablauf
(6i ohne Sicherung zu -> 6ii Einsetzen -> 6iii ausgelieferter sub02: (3,121)=1, Zelle 19 frei): gruen.

### 9.5 Suite

(folgt)

### 9.6 Eigene Abnahme an der echten exe (Bilder)

(folgt)

### 9.7 Abweichungen vom Plan (mit Grund)

(folgt)

### 9.8 Offene Punkte / bekannte Grenzen

(folgt)
