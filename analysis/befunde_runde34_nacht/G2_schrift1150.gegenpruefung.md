# Gegenpruefung G2 — Bauplan "blinkende Schrift ROOM1150" (Skeptiker)

Stand 2026-09-30, Stufe GEGENPRUEFUNG DES BAUPLANS (vor dem Bau), kein Port-Code geaendert.
Geprueft: `analysis/befunde_runde34_nacht/G2_schrift1150.md` (Stand e88deda8).
Werkzeug dieser Pruefung: `re15_port/tools/r34n_g/g2_gegen_zensus.py` (nur Lesen), Ausgabe
`G_belege/G2g_gegen_zensus.txt`. Alle Adressen unten selbst mit `re15_disasm.py` gegen
`info/Re1.5/PSX.EXE` disassembliert bzw. als Datei-Bytes gelesen (nicht aus dem Dossier uebernommen).

## 0. Urteil

**haltbar_mit_auflagen.**

Der Kern haelt jeder Nachpruefung stand: Mechanismus (sub05 + Opcode 0x45 -> FUN_800396a8 ->
Record-Byte0), Aufbau (FUN_800392d4), Zeichnen (FUN_80039590), Reihenfolge im Bild, RDT-Daten beider
Varianten, Takt 20 SCD-Takte je Zustand und der Port-Ist-Zustand sind richtig. Die Lesart trifft den
Nutzerwortlaut.

**Widerlegt** ist ein Nebenast: die Behauptung, der Statusschirm (Inventar) und der
Speicherkarten-Schirm loesten im Original einen Neuaufbau aus ("alle Masken wieder an") und setzten
work_vars[0x0C] neu. Das Original schreibt dort Dirty := **2**, und FUN_80021bbc springt bei
Dirty == 2 **ueber** den Aufbau hinweg. Die zwei daraus abgeleiteten Haken (menu_common.c,
Karten-/Optionsschirm) muessen raus bzw. brauchen erst einen Nachweis. Sonst fuehrte der Bau eine neue
Abweichung in JEDEM Raum ein (work_vars[0x0C] nach jedem Inventarbesuch ueberschrieben -> Cut_old
landet im falschen Cut). Dazu kommen Luecken in der Abnahme (RE15_FORCE_CUT macht das Blinken
unmessbar) und beim PSX-Haken.

## 1. Pruefpunkt (a) — tragende Konstanten/Adressen selbst nachdisassembliert

| Behauptung (Dossier) | selbst geprueft | Ergebnis |
|---|---|---|
| Opcode-Tabelle 0x800744a8 + 0x45·4 = 0x800745bc -> 0x800428d4 | `table 0x800745b0`: [0x44] 0x800745b8 -> 0x800420a0 (Sce_em_set, bekannt), [0x45] 0x800745bc -> **0x800428d4** | bestaetigt |
| Handler: op1+1, op2, pc+=3, Rueckgabe 1 | 800428ec `lbu a0,1(v0)` / 800428f0 `lbu a1,2(v0)` / 800428f4 `jal 0x800396a8` / 800428f8 `addiu a0,a0,1` / 80042900 `ori v0,zero,0x1` / 80042904 `addiu v1,v1,3` | bestaetigt |
| FUN_800396a8: fuer i < RDT[0]: Byte1 == a0 -> Byte0 := a1 | 800396b0 `lw v0,-14472(v0)` (0x800ac778) / 800396b8 `lbu a3,0(v0)` / 800396c0 `lw v1,9604(v1)` (0x800b2584) / 800396d0 `lbu v0,1(v1)` / 800396d8 `bne v0,a0` / 800396e0 `sb a1,0(v1)` / 800396ec `addiu v1,v1,4` | bestaetigt |
| einziger Aufrufer von FUN_800396a8 = 0x800428f4 | jal-Wort-Zensus EXE **und alle sieben BIN** (STAGE1-6, TITLE, DEBUG): nur 0x800428f4 | bestaetigt (Overlays zusaetzlich geprueft) |
| FUN_800392d4: NULL -> RDT[0] := 0; RDT[0] := (u32>>16)&0xff; Byte0 der RDT[7] Records := 0; je Maske Byte0 \|= 1, Byte1 := Gruppe+1, Tiefe | 80039324-38 `lw v1,0(t5)` / `addiu v0,zero,-1` / `bne` / `sb zero,0(a0)`; 80039330 `srl t2,v1,16`, 80039358 `sb t2,0(a0)`; 8003936c-84 Loeschschleife ueber `lbu a3,7(v0)`; 800393d8-e4 `lbu/ori 0x1/sb`; 800393e8-ec `addiu v0,a3,1` / `sb v0,-1(t1)`; 80039544 `addiu a3,a3,1` auch fuer Leer-Gruppen (Sprung 800393c0 -> 80039540) | bestaetigt (Leer-Gruppen zaehlen mit). Nebenbefund: im NULL-Fall wird Byte0 NICHT geloescht (Sprung vor die Schleife) — folgenlos, Zahl = 0 |
| FUN_80039590: zeichnet nur bei Byte0 & 1, TPage 0x95, OT-Index = Tiefe | 800395c0 `lbu s4,0(v0)`; 800395f0-f4 `andi v0,v0,0x1` / `beq`; 80039630 `ori a3,zero,0x95`; 80039650-5c `lh a0,2(s3)` / `sll a0,a0,2` / `addu` | bestaetigt |
| Reihenfolge: SCD @0x8001cdec vor Masken @0x8001ce54; Gate DAT_800aca38 & 0x100000 | 8001cdec `jal 0x8003f038`; 8001ce3c-4c `lw 0x800aca38` / `lui v1,0x10` / `and` / `bne -> 0x8001ce5c` (Bit GESETZT = nicht zeichnen); 8001ce54 `jal 0x80039590` | bestaetigt |
| SCD-Laeufer steht bei g_pauseflags & 0x02000000 | 8003f040-4c `lw g_pauseflags` / `lui v1,0x200` / `and` / `bne -> 0x8003f090` | bestaetigt |
| Aufbau nur @0x80021c28 in FUN_80021bbc, "jeder Apply baut neu auf" | 80021c28 `jal 0x800392d4` einzige Stelle (Zensus). **ABER** 80021bc4 `lbu v1,21591(v1)` (Dirty 0x800b5457) / 80021bc8 `ori v0,zero,0x2` / 80021bd4 `beq v1,v0,0x80021df8` | **nur fuer Dirty == 1.** Bei Dirty == 2 ueberspringt der Sprung 80021bdc..80021df4: die Cut-Schreiber 80021bf4 `sh v0,4072(at)` (work_vars[0x0C]) und 80021bfc `sh v1,4068(at)` (work_vars[0x0A]), den Aufbau 80021c28 UND das BSS-Laden. Ausserdem gatet 80021c10-20 den Aufbau mit demselben Bit 0x100000 wie das Zeichnen |
| Dirty-Schreiber | Zensus ALLER `sb ..,0x5457(at)` (Werkzeug Abschnitt 2): := 1 @0x8001d5c8 (Laden), @0x8001daec (Raumlader), @0x80021514 (Cut-Abweichung), @0x8002e730 (Optionen, nur bei DAT_800aca38 & 0x40000000), @0x800402f4 (Cut_chg), @0x80040354 (Cut_old); **:= 2 @0x800466fc** (Statusschirm, Normalzweig DAT_800b25c0 != 1, Wert aus dem Delay-Slot 800466dc `ori v0,zero,0x2`) und **:= 2 @0x80026634** (Kartenschirm FUN_80026594, Parameter 0, Wert 8002661c `ori v0,zero,0x2`); := 0 @0x80021618, @0x80021e98 | **Dossier-Schluss widerlegt** (Abschnitt 7) |
| Statusschirm Sonderzweig DAT_800b25c0 == 1 | 800466e0-e8 `lh a0,0xfe8` / `jal 0x800142f4` = Cut-Anforderung auf work_vars[0x0C] (800142f4-300 `sb a0,-1099(at)` -> 0x800afbb5), kein Dirty-Store; ein echter Cut-Unterschied wird dann erst ueber 80021514 zu Dirty := 1 | Neuaufbau nur, wenn dieser Zweig den Cut wirklich wechselt — braucht keinen eigenen Port-Haken |
| ROOM1150/1151 sub05 = 52 Byte, Gruppen 6..11 := 0/1, Sleep 20, Goto | Datei-Bytes ROOM1150 @0x10B6 und ROOM1151 @0x1094 gelesen: `45 05 00 … 45 0a 00 09 0a 14 00 45 05 01 … 45 0a 01 09 0a 14 00 17 ff ff 00 d4 ff`, beide identisch; sub-Tabelle: sub05 @0x10B6 (1150, 9 subs) / @0x1094 (1151, 8 subs); sub00 beider Raeume `04 ff 18 02 04 ff 18 05 01 00` | bestaetigt |
| sprite.pri Cut 2 @0x66C: 11 Gruppen, 54 Masken, Gruppen 6..11 je 1 Maske (Records 48..53), Tiefe 0, Ziel x139..201 y25 | eigener Parser (Kopf `0b 00 36 00`, Gruppen 12/9/10/10/7/1/1/1/1/1/1); Records 48..53 bei 0x89C/0x8A4/0x8B0/0x8B8/0x8C0/0x8C8, dst (139,25) (160,25) (167,25) (176,25) (190,25) (201,25), Tiefe 0; Cuts 0/1/3/4 hoechste Gruppe 1/3/5/3, Cuts 5..8 NULL; 1150 == 1151 byteweise (pri und Kameratabelle); shared_assets == info (cmp) | bestaetigt |
| RDT-Byte 0 = 0 beim Laden (erste 0x45 der Init laeuft leer) | Zensus 206/206 RDTs: Byte 0 == 0 ueberall | bestaetigt |
| Record-Tabelle wird nur von diesen 5 Stellen beruehrt | Zensus aller Zugriffe mit Versatz 0x2584 (EXE + BIN): 0x80039288, 0x80039368, 0x8003938c, 0x800395c8, 0x800396c0; die zwei BIN-Treffer (STAGE2 0x8010002c = Daten, STAGE6 0x80100a54 = 0x80102584-Tabelle) sind fremd | bestaetigt — kein zweiter Mechanismus in den Overlays |
| Port: `s_opcode_sizes[0x45] = 3`, `s_op_table[0x45] = op_unknown` | scd_vm.c Z.210 `[0x45] = 3`; Z.248 Vorbelegung op_unknown, keine 0x45-Zuweisung | bestaetigt |
| Port zeichnet alle Masken jedes Bild | render_pc.c Z.984-987 `mask_n = s_pri_rect_count`, `mask_order[i] = i`; main.c Z.5739-5756 uebergibt `pri.draw_count` in Parse-Reihenfolge | bestaetigt |
| Takt: Port-SCD 1 Takt je Bild bei 30 fps | main.c Z.3593/3601 `target_fps = 30`, `frame_budget_ms = 1000/30`; Z.5377-5378 `scd_vm_tick()` je Bild bei 30 fps; Z.10835-10841 SDL_Delay-Kappung (keine Monitor-Bildrate); Trace G2_08: 0x45 bei F25/F45/F65/F85 = 20 Bilder | bestaetigt, keine Runde-30-Fehlerklasse |

Die Original-Messung (G2_06, sechs Staende) habe ich nicht wiederholt: Byte0-Werte, Faden-pc,
Sleep-Rest und Bildspeicher sind untereinander widerspruchsfrei (Rest-Rueckrechnung liegt auf dem
40-VBlank-Raster), und der Mechanismus ist statisch vollstaendig belegt. Kein Anlass zum Zweifel.

## 2. Pruefpunkt (b) — Plan vs. Nutzerwortlaut/Bilder

Wortlaut (AUFTRAG.md): "In ROOM 1170 blinkt im Hintergrund die Schrift des Gebäudes. Bei uns nicht." +
Korrektur "Nein, nicht beim Heliport, sondern in Irons Office room 1150 blinkt die Schrift eigentlich im
Hintergrund." Zu diesem Punkt gibt es kein Nutzerbild.

* Die Leuchtschrift "HEAVEN" eines Gebaeudes draussen, durch die Jalousien in Cut 2, ist Schrift, im
  Hintergrund, an einem Gebaeude — alle drei Merkmale des Wortlauts. Selbst angesehen: G2_02
  (BSS/AN/AUS), G2_05 (Original-Bildspeicher AN/AUS), G2_07 (Port dauernd AN).
* Andere Kandidaten selbst gegengeprueft: der Raum-ESP (RDT+0x4C = 0x14460, Effekt-Id 0x01, TIM
  @0x1E520) ist ein Satz **roter Lichtpunkte/Ringe** (TIM selbst dekodiert, 256x24 4bpp, 4 CLUTs) —
  keine Schrift; ROOM1150s SCD enthaelt **kein** Sce_espr_on (Dump aller 10 Skripte). Die TIM
  @RDT+0x58 ist Objekttextur (Jalousie-Lamellen, Papier), keine Schrift.
* Keine bequemere Lesart: das Dossier nimmt die einzige periodische Schrift-Aenderung, die das Original
  nachweislich zeigt. ROOM1151 (Elza) ist an allen beteiligten Stellen byte-gleich (selbst geprueft).

Ergebnis: Lesart passt.

## 3. Pruefpunkt (c) — Softlocks, Regressionen, Laden/Speichern, Raumvarianten

* **Softlock:** keiner. 0x45 kehrt mit 1 zurueck (@0x80042900), wie heute op_unknown; keine Warte-
  bedingung haengt an Masken. ROOM3071 sub02 (Lichtfolge) wartet nicht auf Masken — der offene
  Elza-Softlock bleibt unberuehrt (selbst gelesen: sub02 @0x356C, nur Sleep/Set/Col_chg_set in der Folge).
* **⛔ Regression durch den Menue-Haken (Plan 5.2, Zeile menu_common.c):** `g_scd.cam_change_pending = 1`
  nach JEDEM Schliessen des Statusschirms fuehrt im Port ueber `re15_cam_present_tick()`
  (room_common.c Z.45-65) zu `work_vars[0x0C] = work_vars[0x0A]` und — mit dem geplanten Haken — zum
  Neuaufbau "alle an". Das Original tut beides NICHT (Dirty := 2, Sprung @0x80021bd4). Folge in
  jedem Raum: ein Skript-`Cut_old` nach einem Inventarbesuch kehrt in den falschen Cut zurueck; in
  ROOM1150 Cut 2 springen die Buchstaben nach dem Inventar auf AN, im Original bleibt der Zustand.
* **Speichern (Kartenschirm):** dasselbe — Original Dirty := 2 @0x80026634, kein Neuaufbau. Ein
  Port-Haken dort waere eine Abweichung. Speicherstand braucht kein Feld (Original speichert die
  Tabelle nicht; LOAD = Dirty 1 @0x8001d5c8 -> Neuaufbau) — Plan richtig.
* **PSX-Ziel:** der Plan setzt nur die Zeichen-Zeile in `platform/psx/src/render.c`. Der Aufbau liefe
  auf PSX nur beim Raumwechsel (room_common.c), NICHT beim Cut-Wechsel (platform/psx/main.c Z.453
  `if (re15_cam_present_tick())`, Pri-Neuparse Z.485-491). Nach dem ersten Cut-Wechsel waere die
  Tabelle die des Eintritts-Cuts: `re15_mg_sichtbar(i)` wuerde Indizes des NEUEN Cuts mit Gruppen
  des ALTEN Cuts beantworten -> in ROOM3000/3010/1211 (sub01 je Bild) verschwaenden auf PSX falsche
  Masken. Entweder beide PSX-Haken oder keiner.
* **Andere Raeume (Plan 7.1):** selbst nachgelesen: ROOM3000 main00 @0x14B6 `Ck(4,9)==0 -> Set(5,0)=1,
  sonst Set(5,0)=0 + 7x Sce_em_set`; sub01 @0x15C4 `Ck(5,0)==0 -> Switch(work_vars[0x0A])` mit
  Case 0: Gruppen 1..3 := 0 (= alle 43 Masken von Cut 0). ROOM1211 sub01 @0x1F52 nur Case 7.
  ROOM3071 sub01 @0x350A nur Case 9. Die Tabelle 7.1 stimmt. Das ist eine SICHTBARE Aenderung in
  acht weiteren Raeumen (Zombie-Variante ROOM3000/3010: Figuren werden dort vor Vordergrund-Objekten
  gezeichnet) — Original-Verhalten, aber der Nutzer muss es vorher wissen (Auflage 9).
* **Raumvarianten:** 1150/1151 byte-gleich (pri, Kameratabelle, sub00, sub05). Plan richtig.
* **Laden des Raums / roher Puffer:** `re15_rdt_t` fuehrt `raw`/`raw_size` (re15_rdt.h Z.235-236,
  gesetzt von re15_rdt_parse). Der Aufbau kann damit am geparsten Raum haengen statt an einem
  eigenen Zwischenspeicher — so ist die Falle aus memory reai-v2-playthrough-not-jumpin (alte Bytes,
  neue Offsets) vermeidbar; der Plan muss es aber festlegen und messen (Auflage 7).

## 4. Pruefpunkt (d) — Ressourcen-Kollisionen (VERTRAG.md, andere Spuren)

* Keine Flags, AOT-Slots, Nachrichten, Dokumente, obj_ids noetig — keine Kollision mit dem Vertrag.
* Gemeinsame Dateien: scd_vm.c (eine Tabellenzeile + Handler), main.c, render_pc.c, room_common.c,
  PSX render.c/main.c — je 1-3 Zeilen, vertragskonform. Mit Auflage 1 entfaellt menu_common.c ganz.
* ⛔ `re15_port/tools/local_build.sh` (Plan 5.1 "RE15_MIN_TESTS anheben") ist eine gemeinsame Datei,
  die jede der sieben Spuren anfassen wuerde. Die Schranke ist eine UNTERGRENZE (`-ge` Z.345) — neue
  Tests brauchen sie nicht. Nicht anfassen (Auflage 8).
* Spur B (Hebetisch 1150/1151): sub04 faehrt Cut_chg 4 -> Cut_old (Datei 0xFB2/0x10B2), sub06/07
  Cut_chg 6/8 -> Cut_old. Cut_old = Dirty 1 (@0x80040354) -> Neuaufbau "alle an" beim Zurueckkehren
  in Cut 2 — Original-konform, keine Wechselwirkung mit Bs Plaetzen (AOT 11/12, Nachricht 20/21,
  obj 8). Baut B eine eigene Nahansicht ueber `cam_change_pending`, gilt dasselbe (Dirty-1-Aequivalent).
* Die Dateinamen `masken_gruppen.c` / `re15_masken_gruppen.h` weichen vom Muster `<thema>_<raum>.c` ab —
  vertretbar, weil die Korrektur raumuebergreifend ist (Plan 7.1); im Dossier so begruenden.

## 5. Pruefpunkt (e) — misst die Abnahme das Nutzer-Symptom am Artefakt?

* 6.3 (echte exe, Lade-Weg, RE15_FRAMEDUMP = Readback des komponierten Bilds vor Present,
  render_pc.c Z.1566 `SDL_RenderReadPixels`, beschleunigter Renderer) misst genau das Symptom
  "blinkt / blinkt nicht" am Bild, mit Takt und Referenzbildern. Gut.
* ⛔ 6.4 nennt "oder RE15_FORCE_CUT". FORCE_CUT setzt in `pc_cam_present_apply` (main.c Z.2873-2874)
  JEDES Bild `cam_change_pending = 1` -> `re15_cam_present_tick()` liefert jedes Bild 1 -> mit dem
  geplanten Haken Neuaufbau jedes Bild VOR dem SCD-Takt -> AUS nur im Umschaltbild selbst (1 von 40
  Bildern). Unter FORCE_CUT ist das Blinken also nicht messbar; Messung 1 des Dossiers (FORCE_CUT)
  taugt nur fuer den Ist-Zustand. Die vorhandenen Riegel `test_r30_irons_tisch_bild/_laden/_licht`
  laufen mit `RE15_FORCE_CUT=2` in ROOM1150 und vergleichen Laeufe gleicher Zeitlage — sie bleiben
  gruen, sind aber kein Blink-Beleg.
* ⛔ 6.5 prueft eine falsche Erwartung ("nach dem Schliessen AN"). Richtig ist: Zustand nach dem
  Schliessen == Zustand beim Oeffnen; der Sleep-Rest laeuft danach weiter (SCD stand im Menue:
  @0x8003f040-4c im Original, `re15_menu_gameplay_frozen()` main.c Z.5355 im Port).
* Es fehlt ein Nachweis, dass der Aufbau nur an Dirty-1-Stellen laeuft (nicht je Bild, nicht nach dem
  Inventar) und dass die Tabelle zum gezeigten (Raum, Cut) passt — sonst faellt ein falscher
  Aufbau im EINTRITTS-Cut (z.B. ROOM1150 Cut 0, 2 Masken) durch die Blink-Messung in Cut 2 nicht auf.
* 6.6 (andere Raeume mit DuckStation-Gegenprobe ROOM3000 Cut 0) ist richtig angelegt.

## 6. Pruefpunkt (f) — besseres Vorbild im Original/RE2?

RE2 `RE2_Quellcode_V2/FUN_80049ca8.c` (selbst gelesen): gleiche 4-Byte-Records, `(*pbVar5 & 1) != 0`
UND `FUN_80077360(&DAT_800d4928 + DAT_800d4820*4, Record-Byte1)` — ein Bitfeld je Index, getestet mit
Gruppe+1. RE2 fuehrt also zusaetzlich eine Gruppen-Sichtbarkeit ueber Flags. Nach
reai-v2-beta-zu-retail gilt RE2 nur, wo RE1.5 unfertig ist; RE1.5 hat das System hier vollstaendig
(Daten in 10 Raeumen, Opcode, Aufbau, Zeichnen) und in sich stimmig (Neuaufbau "alle an" bei jedem
Dirty-1-Apply, sub05 blendet binnen hoechstens 40 Takten wieder aus). Kein besseres Vorbild; die
Einordnung des Dossiers (RE1.5 byte-true) ist richtig.

## 7. Widerlegt

1. **Dossier 4 (letzter Punkt), 5.2 (Zeile menu_common.c), 5.4 (Zeile "Neuaufbau nach Statusschirm"),
   6.5, 7.2 (zweiter Punkt):** "Nach dem Statusschirm setzt das Original DAT_800b5457 := 2 ->
   naechster Present ruft FUN_80021bbc -> alle Masken wieder an" und "setzt wie das Original
   work_vars[0x0C] := gezeigter Cut (@0x80021bf4)". FALSCH: FUN_80021bbc prueft zuerst
   @0x80021bc4 `lbu v1,21591(v1)` / @0x80021bc8 `ori v0,zero,0x2` / @0x80021bd4 `beq v1,v0,0x80021df8`
   und springt bei Dirty == 2 ueber 0x80021bdc..0x80021df4 — ueber beide Cut-Schreiber (@0x80021bf4,
   @0x80021bfc), den einzigen Aufbau-Aufruf (@0x80021c28) und das BSS-Laden. Bei 2 werden nur die
   Kamera-/Projektionsschritte ab 0x80021df8 wiederholt. Nach dem Inventar bleibt der Maskenzustand
   also stehen, work_vars[0x0C] unveraendert.
2. **Dossier 7.2:** "Ein spaeteres Cut_old nach einem Inventarbesuch kehrt damit zum gezeigten Cut
   zurueck — Original-Verhalten". FALSCH (folgt aus 1): im Original kehrt Cut_old zum Cut VOR dem
   letzten Dirty-1-Apply zurueck.
3. **Dossier 4 / 5.2 / 5.4 (Kartenschirm):** "Neuaufbau nach dem Speicherkarten-Schirm (@0x80026634,
   Parameter 0)". FALSCH: dort steht Dirty := 2 (Wert 8002661c `ori v0,zero,0x2`) -> kein Neuaufbau.
4. **Dossier 5.2 (Optionsschirm):** Dirty := 1 @0x8002e730 ist richtig gelesen (Bit 0x40000000 wird
   beim Sitzungsstart gesetzt, 0x8001d564 `sw v0,-13768(at)` nach `lui v1,0x4000`), aber unbelegt ist,
   dass diese 1 den Apply erreicht: kehrt der Optionsschirm in den Statusschirm zurueck, schreibt dessen
   Schliessen danach 2 (@0x800466fc), und der Apply-Aufruf ist bei DAT_800b536c != 0 ausgesetzt
   (@0x8002151c `lbu v0,21356(v0)` / @0x80021524 `bne v0,zero,0x80021584` — vorbei an @0x80021558
   `jal 0x80021bbc`); ob 0x800b536c im Statusschirm != 0 ist, ist nicht belegt. Ohne Nachweis kein Haken.
5. **Dossier 6.4 "oder RE15_FORCE_CUT":** mit dem geplanten Haken nicht messbar (Abschnitt 5).
6. Kleinigkeit 5.2 Zeile pc_cam_present_apply: "@0x80021514" begruendet KEINEN Neuaufbau bei gleichem
   Cut — der schreibt nur bei Cut-Abweichung. Gleicher Cut baut nur ueber Cut_chg/Cut_old
   (@0x800402f4/@0x80040354), Raumlader (@0x8001daec) und Laden (@0x8001d5c8) neu auf. Die
   Schlussfolgerung (Haken im `re15_cam_present_tick()`-Zweig) bleibt richtig, weil der Port alle
   heutigen `cam_change_pending`-Setzer als Dirty-1-Aequivalente fuehrt.

## 8. AUFLAGEN (nummeriert, umsetzbar)

1. **Kein Neuaufbau nach Statusschirm und Kartenschirm.** Die Haken `g_scd.cam_change_pending = 1` in
   menu_common.c (`close_phase`) und im Speicher-/Kartenschirm-Ruecksprung ENTFALLEN. Beleg im Dossier
   und im Code-Kommentar der Tabelle: Dirty := 2 @0x800466fc (Wert @0x800466dc) bzw. @0x80026634 (Wert
   @0x8002661c); FUN_80021bbc @0x80021bc4/@0x80021bc8/@0x80021bd4 springt bei 2 nach 0x80021df8 ueber
   @0x80021bf4, @0x80021bfc und @0x80021c28. Dossier-Abschnitte 4, 5.2, 5.4, 6.5, 7.2 entsprechend
   korrigieren.
2. **Optionsschirm-Haken nur mit Nachweis.** Vor einem Haken belegen (statisch ueber die Aufrufkette
   von FUN_8002e470 / Zeiger 0x80073da4 und FUN_80046540 / Zeiger 0x80074be4, oder in DuckStation:
   Cut 2, AUS-Phase, Status -> Optionen -> zurueck -> schliessen, Byte0 der Records 48..53 lesen), ob die
   1 von @0x8002e730 vor der 2 von @0x800466fc einen Apply erreicht. Ohne Beleg: kein Haken (der Port-
   Zustand bleibt dann wie beim Inventar stehen).
3. **PSX vollstaendig oder gar nicht.** Neben `if (!re15_mg_sichtbar(i)) continue;` in
   platform/psx/src/render.c auch `re15_mg_aufbauen(...)` in platform/psx/main.c im Zweig
   `if (re15_cam_present_tick())` (Z.~453), Beleg @0x80021c28. Sonst beide PSX-Zeilen weglassen und
   die Luecke im Dossier fuehren.
4. **Blink-Abnahme ohne RE15_FORCE_CUT.** 6.4 streicht "oder RE15_FORCE_CUT"; Cut 2 wird ueber den
   RVD-Zonenwechsel (RE15_PLAYER_POS + RE15_INPUT_SCRIPT, Weg aus Cut 1 bzw. Cut 0) oder die Karte mit
   camera_cut 2 erreicht. Der Integrationshaken test_r34n_g_schrift1150.cmake setzt FORCE_CUT nicht.
   Im Dossier festhalten, dass FORCE_CUT (main.c Z.2873-2874, Dirty jedes Bild) das Blinken unterdrueckt
   und die r30-Tisch-Riegel deshalb kein Blink-Beleg sind.
5. **6.5 mit richtiger Erwartung:** Inventar in Cut 2 waehrend einer AUS-Phase oeffnen
   (`RE15_INV_OPEN_AT`), N Bilder offen, schliessen -> AUS bleibt, bis sub05s naechstes `:= 1` nach dem
   Rest des Sleep kommt (SCD stand im Menue); ebenso in einer AN-Phase. Dazu work_vars[0x0C] vor/nach
   dem Menue im Log: unveraendert.
6. **Aufbau-Zaehler als Pin.** `RE15_MG_LOG` schreibt je Aufbau eine Zeile mit Bild, Raum, Cut, Grund.
   In den Laeufen 6.3/6.4/6.5 gilt: Aufbau-Zeilen nur bei Raumeintritt/Laden und bei echten Apply-
   Ereignissen (Cut-Wechsel, Cut_chg/Cut_old); keine zwei in aufeinanderfolgenden Bildern ohne
   Cut-Ereignis, keine nach dem Inventar/Speichern. Verletzung = rot.
7. **Tabelle == gezeigter (Raum, Cut).** Der Aufbau liest die Gruppenkoepfe aus `rdt->raw`/`raw_size`
   des AKTUELLEN Parse (re15_rdt.h Z.235-236), keinen eigenen Zwischenspeicher. Die Aufbau-Logzeile
   traegt pri_offset und Zahl; sie muss zur `[pri] cut=.. pri_offset=.. masks=..`-Zeile desselben
   (Raum, Cut) passen — gemessen im EINTRITTS-Cut nach Debug-Sprung aus einem anderen Raum und nach
   CONTINUE, fuer 1150 und 1151 (ROOM1150 Cut 0: Zahl 2, beide Masken sichtbar).
8. **local_build.sh nicht anfassen.** RE15_MIN_TESTS ist eine Untergrenze (`-ge`, Z.345); neue add_test
   erfordern keine Aenderung, jede Aenderung erzeugt Zusammenfuehrungs-Konflikte mit sechs Spuren.
9. **Sichtbare Aenderung in acht weiteren Raeumen ankuendigen.** Dossier, Commit und Versionshinweis
   nennen ROOM1211 (Cut 7), ROOM3000/3001, ROOM3010/3011 (Zombie-Variante, Flag (4,9)/(4,10)),
   ROOM3071 (Cut 9, Lichtfolge), ROOM5060/5061 (Cut 11) mit Vorher/Nachher-Bild aus 6.6; die
   DuckStation-Gegenprobe ROOM3000 Cut 0 (Byte0 der 43 Records + Bildspeicher) liegt VOR der
   Auslieferung vor.
10. **Begruendung im Code-Kommentar praezisieren** (Abschnitt 7 Punkt 6): der Haken im
   `re15_cam_present_tick()`-Zweig bildet die Dirty-1-Setzer @0x8001d5c8/@0x8001daec/@0x80021514/
   @0x800402f4/@0x80040354 ab; einen Dirty-2-Pfad hat der Port nicht und bekommt keinen.

## 9. Eigene Messwerkzeuge/Belege dieser Gegenpruefung

* `re15_port/tools/r34n_g/g2_gegen_zensus.py` — jal-Aufrufe der Masken-Funktionen (EXE + 7 BIN),
  alle Dirty-Stores mit Wert, Kopf von FUN_80021bbc. Ausgabe: `G_belege/G2g_gegen_zensus.txt`.
* Selbst disassembliert (re15_disasm.py dis): 0x800428d4 (16), 0x800396a8 (22), 0x80039270..0x80039590
  (230), 0x80039590..0x800396a0, 0x8001cdd0 (40), 0x80021500 (30), 0x80021568 (48), 0x80021bbc..0x80021eac
  (190), 0x800466a0 (30), 0x8002e710 (12), 0x80026600 (20), 0x800142f4 (14), 0x800402d0/0x80040330 (je 14),
  0x8001d540 (30), 0x8003f038 (20), 0x80021634 (12); table 0x800745b0 / 0x800744a8.
* Datei-Bytes: ROOM1150/1151.RDT Kopf, sprite.pri aller Cuts, sub05, sub00, sub-Tabellen, ESP-Kopf und
  beide TIMs (dekodiert, angesehen); SCD-Dumps ROOM3000 (main00/sub00/sub01), ROOM1211 sub01,
  ROOM3071 sub01/sub02, ROOM1150 alle Skripte.
* Port-Quellen gelesen: scd_vm.c (Tabelle/op_unknown), room_common.c (cam_present_tick, Schritt 8/9),
  main.c (pc_cam_present_apply, pri-Block, Tuer-Umhaengen rdt_buf, SCD-Tick-Gates, Bildratenkappung),
  render_pc.c (Maskenliste, set_pri_rects), pri_common.c (Parse-Reihenfolge, draw_count),
  platform/psx/main.c + render.c (Cut-Wechsel, Maskenschleife), re15_rdt.h/re15_room.h,
  tests/integration/test_r30_irons_tisch_*.cmake (FORCE_CUT=2 in ROOM1150).
