# Spur G — Gegenpruefung des Bauplans (ROOM1170: blinkende Gebaeudeschrift)

Pruefer: Skeptiker, hat `G_schrift1170.md` NICHT geschrieben. Stufe: Gegenpruefung vor dem Bau.
Kein Port-Code geaendert. Eigene Messungen: `G_belege/G15_gegenpruefung_messungen.txt`.

## Urteil: HALTBAR MIT AUFLAGEN

Der Kern des Dossiers haelt jedem Widerlegungsversuch stand: **das Schild MAGAZINE CLUB blinkt im
Original nicht, der Port zeigt es pixelgleich zur BSS, ein Umbau waere erfundenes Verhalten.**
Ich habe versucht, das ueber sechs unabhaengige Wege zu kippen (anderer Zeichner im Bildspeicher,
versteckte zweite Aufnahme in der BSS, Overlay-Code, OT-Primitive im Cut-3-Rechteck, andere
Disk-Version, abweichender Port-Weg im echten Durchlauf) — keiner traegt. Die Auflagen betreffen
ueberzogene bzw. nicht belegte Einzelaussagen und die Uebergabe an den Nutzer, nicht den Plan.

Widerlegt wurden zwei Nebenaussagen (Abschnitt "Widerlegte Behauptungen"); drei Belege sind in der
Form, wie sie im Dossier stehen, nicht tragfaehig (Auflagen 3 und 4).

## (a) Tragende Konstanten/Adressen — selbst nachdisassembliert

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` gegen `info/Re1.5/PSX.EXE`
(t_addr 0x80010000, t_size 0xaf000), Sprungziele jeweils selbst geoeffnet.

| Dossier-Aussage | eigene Pruefung | Ergebnis |
|---|---|---|
| Hintergrund je Bild: `jal 0x80043870` @0x8002157c, y aus `lbu 0x800aca34` @0x8002156c | `8002156c lbu a0,-13772(a0)` / `80021574 sltu` / `80021578 subu` / `8002157c jal 0x80043870` / Delay `80021580 andi a0,a0,0xf0` | bestaetigt |
| Ziel FUN_80043870 = LoadImage(RECT{0,y,0x140,0xF0}, 0x80198000 bzw. +0x14) | `80043874 lui a1,0x8019` `80043878 ori a1,a1,0x8000`, `80043880 lw DAT_800b854c`, `80043894 ori a1,a1,0x8014`, `80043898 ori v0,zero,0x140`, `800438a0 ori v0,zero,0xf0`, `800438ac sh zero,16(sp)`, `800438b0 jal 0x80068c88` | bestaetigt |
| 0x80068c88 = LoadImage | `80068c9c/80068ca0 lui a0,0x8001 / addiu a0,a0,7068` -> String "LoadImage" @0x80011b9c | bestaetigt (Name aus dem Binary) |
| Cut-Wechsel: MDEC `jal 0x80053a8c` @0x80021e34, StoreImage @0x80021e44, RECT @0x80072f2c | `80021e34 jal 0x80053a8c` (a1 0x80190000, a2 0x80199e00, a3 0x80198000, Delay `sh v0,0(v1)` auf 0x80072f2e), `80021e44 jal 0x80068cec` + `ori a1,a1,0x8000`; 0x80068cec laedt "StoreImage" @0x80011ba8; `read 0x80072f2c` = [0,0,320,240] | bestaetigt |
| Cut-Wechsel-Bild wird nicht gezeichnet | `80021558 jal 0x80021bbc` / `80021560 j 0x800215fc` (hinter DrawOTag und Puffertausch @0x800215ec-f8) | bestaetigt |
| drei OTs @0x800215bc/d0/e4 | Ziel 0x80068fc4 laedt "DrawOTag(%08x)" @0x80011bf0; Koepfe s0-800 = 0x800ac714 (buf<<6), s0-4960 = 0x800ab6d4 (buf<<12), s0-9088 = 0x800aa6b4 (buf<<5) | bestaetigt |
| Cut-Anforderung (Grundlage der erzwungenen Cuts) | `800214e0 lw DAT_800aca3c` / `andi 0x80`; `800214f8 lh DAT_800b0fe4` gegen `80021500 lbu DAT_800afbb5`; ungleich -> `80021514 sb DAT_800b5457=1` | bestaetigt |
| **fehlt im Dossier:** zweite Bedingung vor dem Upload | `8002151c lbu DAT_800b536c` / `80021524 bne v0,zero,0x80021584` = Upload faellt aus, solange DAT_800b536c != 0; einziger Schreiber `sb a0` @0x80021638 in FUN_80021634 (Bildschirm-Modus), 10 EXE-Aufrufer, 0 Overlay-Aufrufer | kein Blink-Pfad, aber in 3.1 nachzutragen (Auflage 6) |
| RDT+0x4C = 0, kein Raum-ESP | eigener Kopf-Dump: ROOM1170 und ROOM1171 +0x4C = 0, +0x50 = 0, +0x54 = 0; nCut 13; nOmodel 6 / 2 | bestaetigt |
| BSS: 13 Chunks, keine zweite Aufnahme | ROOM117.BSS = 851968 B = 13 x 0x10000; Daten enden je Chunk bei 4..48 %, danach Nullen; kein weiterer 0x3800-Kopf | bestaetigt |
| SCD ohne Hintergrund-Opcodes | eigener Dump (scd_dump_room.py) beider Raeume: Endlosschleifen nur sub04/05 (Speed_set/Add_speed auf Objekt 3/4), sub09/10 Warteschleifen (5,32)/(5,33); ROOM1171 sub02 Bits (5,28..31) = Szenen-Synchronisation, kein Hintergrund | bestaetigt |
| "Kein Pfad veraendert die Hintergrundkopie" (16 Verweise auf 0x80198000) | reicht allein nicht: ein Blinken koennte auch als eigener LoadImage/MoveImage in den Bildspeicher kommen. Vollzensus aller jal-Worte auf LoadImage (EXE 22), MoveImage (2), StoreImage (4), ClearImage (4) in PSX.EXE und allen BIN/*.BIN: kein periodischer Schreiber in x0..319 y0..479; STAGE1..6.BIN rufen weder die Transfers noch eine der Huellen | Aussage haelt, Beleg erweitert (G15 D, Auflage 6) |
| MZD-Disk = Auslieferungsstand fuer 1170, "EXE-Code gleich bis auf einen Datei-Lade-Haken @0x80013b7c" | ISO9660 der MZD-Disk selbst gelesen: PSX.EXE, STAGE1.BIN, ROOM117.BSS, ROOM1170.RDT, ROOM1171.RDT **bytegleich** zu `info/Re1.5` (sha256 G15 C). In der Datei steht @0x80013b7c `0x3c01800c` = `lui at,0x800c`. Voller Baumvergleich: 689 von 692 Dateien bytegleich, verschieden nur CAPCOM.STR (Form-2-Sektoren, mein Leser liest 2048 B), README.TXT und SYSTEM.CNF (Zeilenenden); beide README-Koepfe "Resident Evil 1.5 (Magic Zombie Door) Update 25-01-2025" | Schluss richtig und sogar staerker (`info/Re1.5` IST die MZD-Disk des Nutzers); die Formulierung ist falsch: der Haken ist ein LAUFZEIT-Patch im RAM, kein Unterschied der EXE-Datei (widerlegt, Auflage 1) |

## (b) Passt der Plan zum Nutzerwortlaut und zu seinen Bildern?

* Zu Spur G gibt es kein Nutzerbild. "die Schrift des Gebaeudes im Hintergrund" -> MAGAZINE CLUB ist
  die woertliche Lesart; die beiden anderen Kandidaten (Plakat/Band Cut 6, Notausgang Cut 8) wurden
  mitgemessen. **Keine bequemere Lesart**: der bequeme Weg waere gewesen, einen Blink-Effekt zu
  bauen — das Dossier verweigert das mit Messung, das ist genau die RE-GATE-Regel.
* Zensus unvollstaendig: Cut 4 zeigt den rechten Rand des Schilds (18 Blau-Pixel x317..319
  y12..21; im Vorspann unter dem Kinobalken). Aendert nichts, gehoert aber in die Tabelle (Auflage 5).
* Das Ergebnis widerspricht der direkten Beobachtung des Nutzers. Nach memory
  `reai-v2-original-oder-nicht` ist dann nur eine **Messfrage** zulaessig (Abschnitt 8.1 des Dossiers,
  korrekt formuliert). Ein Versionsunterschied scheidet als Erklaerung aus: `info/Re1.5` (die
  Asset-Quelle des Ports) ist dieselbe MZD-Disk "Update 25-01-2025", die der Nutzer in ePSXe und das
  Dossier in DuckStation gefahren hat (689/692 Dateien bytegleich, G15 C). Neu fuer die Messfrage:
  das "Original" des Nutzers ist also die MZD-Disk, und deren
  README.TXT meldet fuer Raum 117 "Added new cutscene in room 117 (Leon)" und "Added sound effect to the
  helicopter of Leon's first cutscene in room 117", ausserdem mehrfach "Changed/Added new video" —
  nichts zu Schild oder Hintergrund. Die Messfrage muss daher auch nach Moment (Vorspann, Freilauf,
  Film) fragen (Auflage 7).

## (c) Softlocks, Regressionen, Laden/Speichern, Raumvarianten

* Kein Port-Code -> kein Softlock, keine Regression, Speicherstand unberuehrt.
* `probe_r34n_g_bss` hat kein add_test und haengt nur an re15_engine/re15_test_support; die Suite
  bleibt beim Basiswert.
* ROOM1171 (Elza): dieselbe BSS (auf der Disk gibt es nur ROOM117.BSS), derselbe Upload-Pfad,
  RDT+0x4C = 0, SCD ohne Hintergrund-Opcodes (sub02..07 selbst gelesen). Eine dynamische Messung ist
  entbehrlich, weil kein raumabhaengiger Zeichenweg existiert (Zensus G15 D).

## (d) Ressourcen-Kollisionen (VERTRAG.md, andere Spuren)

* Keine Flags, AOT-Slots, Nachrichten-IDs, Props. Dateien nach VERTRAG §2 benannt
  (`probe_r34n_g_*.c`, `probes/r34n_g_schrift.cmake`, `tools/r34n_g/`). Keine Beruehrung mit A..F
  oder den r34g-Baeumen.
* Nebenwirkung ausserhalb des Repos: der DuckStation-Fortsetzungsstand (resume.sav, 00:06 Uhr) ist
  verloren (Dossier §7). VERTRAG §3.8 erlaubt DuckStation nur, wenn statisch nicht belegbar — hier
  war der dynamische Beweis noetig; der Verlust muss aber dem Nutzer ausdruecklich gemeldet werden
  (Auflage 8).

## (e) Misst der Abnahmeplan das Nutzer-Symptom am Artefakt?

* **Port nur ueber den Debug-Sprung gemessen** (`RE15_DEBUG_JUMP=1170@240` aus der 1240-Montage).
  memory `reai-v2-playthrough-not-jumpin` verlangt die Gegenprobe im echten Durchlauf. **Selbst
  nachgeholt** (G15 A): echte exe (`re15_pc.exe` dieses Baums, Stand cf0e68ba), Titel -> NEW GAME ->
  Leon -> ROOM1240 (1421 Bilder) -> ROOM1170, jedes 10. Bild bis F5200:
  Cut 2 in 73 Bildern (F480..F1390) max|Port-BSS| = 0, blau 356, L 130,4 konstant;
  Cut 3 ab F1790 bis F5200 max|Δ| = 0 (F1770/F1780 = Ausblenden des Kinobalkens);
  im Vorspann liegt das Schild in Cut 3/4 unter dem Kinobalken (L 0,1..1,3); fuer Cut 3 wie im
  Original (rec4: Schild erst ab F2759 sichtbar), fuer den Cut-4-Rand ohne Original-Messung.
  -> Der Port ist auch im Nutzerweg pixelgleich (Auflage 2: ins Dossier uebernehmen).
* **Original, tragender Beleg = rec4** (selbst aus `rec4_small.raw` nachgerechnet, G15 B): Cut 2 in
  drei Laeufen 510 + 193 + 37 Bilder mit 0 Aenderungen; Cut 3 in 1741 Bildern genau 2 Aenderungen
  (F2760, F2763 = Kinobalken), danach 1736 Bilder konstant. rec4 hat Lebendnachweis (3D, Untertitel,
  Blenden aendern sich in denselben Abschnitten). Dazu die Savestates (beide Bildspeicher == RAM-Kopie)
  und die statische Beweiskette.
* **Nicht tragfaehig in der vorliegenden Form:**
  * rec3 (300 s, Ausschnitt 96x48 nur um das Schild) und die erzwungenen Cuts 5/8/9/10/11/12 (ganzes
    Bild "0 Pixel mit Hub > 40"): **kein Lebendnachweis** — ein pausierter oder stehender Emulator
    liefert exakt dasselbe Ergebnis; und die Schwelle 40 uebersieht jede kleinere Helligkeitsaenderung
    (die Aussage "Notausgang Cut 8 konstant" ist damit nicht belegt). Fuer Cut 10 traegt die exakte
    Schild-Statistik (blau 247..247, L 134,0..134,0), aber auch sie ohne Lebendnachweis.
  * Zahlen: das Dossier nennt 19 Savestates und 18 OT-Walks; eingecheckt sind 14 (G12) und 4 (G11).
    `ss_ot_walk.py` prueft per Default das Cut-2-Rechteck; die Cut-3-Staende muessen mit `--box`
    laufen. Selbst nachgeholt fuer 7 Cut-3-Staende mit `--box 290,4,320,24`: 0 Treffer (G15 E).
* Die Gegenprobe "die Messkette saehe ein Blinken" (Kinobalken-Ausblendung wird erkannt) ist richtig
  und von mir reproduziert.

## (f) Besseres Vorbild im Original/RE2 uebersehen?

* RE2 FUN_8002b968 (`RE2_Quellcode_V2/FUN_8002b968.c`) setzt das Bild aus zwei Quellen zusammen
  (0x80198000 und Raum+0x44). In `ghidra_re2_Leon.txt` hat DAT_800d4494 genau einen Schreiber, den
  SCD-Handler LAB_80058cb8 (`lhu v0,0x2(v0)` / `sh v0,DAT_800d4494` @0x80058cc8, pc += 4);
  DAT_8009dc10, DAT_8009dc0c und DAT_800d4496 haben **keinen** statischen Schreiber (Vorbelegung
  0 / 0xF0 / 0). Ergebnis: das Bild wird ab Zeile DAT_800d4494 senkrecht verschoben und oben aus
  dem zweiten Bild aufgefuellt (Rollen), kein Blinken. Die Einordnung des Dossiers
  ("Roll-Hintergrund, kein Blinken") stimmt.
* RE1.5 FUN_80043870 kennt keine zweite Quelle; das Schild ist fertig gemalt und vollstaendig gezeigt.
  Nach memory `reai-v2-beta-zu-retail` ist hier nichts "unfertig" — kein RE2-Ziel.
* Kein uebersehener RE1.5-Weg: ESP (Raum +0x4C = 0; ein ESP-Aufruf braeuchte ein SCD- oder
  Overlay-Spawn, beides fehlt), CLUT-Tausch (die BSS ist Direktfarbe; die Bildstreifen-/TIM-Helfer
  0x80014144/0x800141c4 sind tot: 0 jal, 0 Datenwort, 0 lui/addiu-Paar in EXE und Overlays),
  Masken (laut Dossier sprite.pri nur Cut 1/2/4/5; die OT-Walks zeigen kein SPRT ueber dem Schild),
  Overlay (0 Aufrufe der Bildtransfers oder ihrer Huellen), Objektmodelle (kein Schild).

## AUFLAGEN

1. **§0/§3.2 berichtigen:** Die MZD-Disk-Dateien PSX.EXE, STAGE1.BIN, ROOM117.BSS, ROOM1170.RDT,
   ROOM1171.RDT sind bytegleich zu `info/Re1.5` (sha256 in G15 C). Der Satz "EXE-Code gleich bis auf
   einen Datei-Lade-Haken" ersetzen durch: "Disk-EXE = info/Re1.5/PSX.EXE; der Haken `j 0x800c02c0`
   @0x80013b7c steht nur im RAM (Datei-Wort 0x3c01800c = `lui at,0x800c`) und wird zur Laufzeit gesetzt".
2. **§2/§6 Port-Abnahme um den echten Durchlauf ergaenzen** (G15 A, Umgebung wortgleich dort):
   Titel -> NEW GAME -> Leon -> ROOM1240 -> ROOM1170, `RE15_FRAMEDUMP="0-5200/10:…"`,
   `RE15_EXIT_AT="5200#1170"`; Hinweis, dass `frame_count` je Raum bei 0 beginnt (Dumps der Montage
   werden von denen aus 1170 ueberschrieben). Der Debug-Sprung bleibt als zweite Probe stehen.
3. **§0/§3.3 Belege ohne Lebendnachweis kennzeichnen oder ersetzen:** rec3 (300 s, 96x48) und die
   erzwungenen Cuts nicht mehr als Beweis fuer "blinkt nicht" fuehren, sondern als "ohne
   Lebendnachweis"; "0 Pixel mit Hub > 40" nicht als "konstant" auslegen. Fuer den Notausgang Cut 8
   entweder exakte Pixel-min/max wie bei Cut 10 angeben oder die Aussage streichen. Tragend sind rec4
   (nachgerechnet, G15 B), die Savestates und die statische Kette.
4. **§3.3 Zahlen belegen:** "19 Savestates / 18 OT-Walks" entweder mit den vollstaendigen Tabellen
   einchecken oder auf das Eingecheckte (G12: 14, G11: 4, dazu G15 E: 7 Cut-3-Walks) berichtigen;
   im Werkzeugkopf von `ss_ot_walk.py` vermerken, dass `--box` fuer Cut 3/10 zu setzen ist.
5. **§1 Zensus:** Cut 4 nachtragen (18 px Schildrand x317..319 y12..21, im Vorspann unter dem
   Kinobalken).
6. **§3.1 Beweiskette erweitern:** den Vollzensus der Bildtransfers (G15 D: LoadImage 22 / MoveImage 2 /
   StoreImage 4 / ClearImage 4 in der EXE, 0 in STAGE1..6.BIN) und die einzige Upload-Auslass-
   Bedingung `lbu DAT_800b536c` @0x8002151c / `bne` @0x80021524 (Schreiber FUN_80021634 @0x80021638)
   aufnehmen — die 16 Verweise auf 0x80198000 allein schliessen einen eigenen Schreiber in den
   Bildspeicher nicht aus.
7. **§8.1 Messfrage in den Rundenbericht an den Nutzer uebernehmen** (einziger offener Punkt von G,
   keine Wahlfrage): Moment (Vorspann / Freilauf / Film), Cut bzw. Screenshot oder Video mit Zeitmarke,
   Emulator + Renderer, Disk-Abbild. Ohne diese Antwort wird **kein** Blink-Effekt gebaut.
8. **§7 Nebenwirkung dem Nutzer melden:** der DuckStation-Fortsetzungsstand von 00:06 Uhr ist
   ueberschrieben. Fuer kuenftige DuckStation-Laeufe: resume.sav vor dem Start kopieren und nach dem
   Lauf zuruecklegen (nicht nur "sichern").
9. **Integration:** nur Dossier, `G_belege/`, `tools/r34n_g/` und die Sonde uebernehmen; keine
   Port-Datei aendert sich; Suite bleibt beim Basiswert (die Sonde hat kein add_test).

## Widerlegte Behauptungen

1. "MZD-Disk gegen Auslieferungsstand: EXE-Code gleich bis auf einen Datei-Lade-Haken `j 0x800c02c0`
   @0x80013b7c" — die Disk-EXE ist bytegleich zu `info/Re1.5/PSX.EXE` (sha256 e6b340fd12ec43be…),
   das Datei-Wort @0x80013b7c ist `0x3c01800c` (`lui at,0x800c`); der Haken existiert nur im RAM.
2. "Schrift auf einem Gebaeude steht nur an drei Stellen … Cuts 2, 3, 10" — Cut 4 zeigt ebenfalls
   einen Rand des Schilds (18 Blau-Pixel x317..319 y12..21).

Nicht widerlegt, aber in der vorliegenden Form **nicht belegt** (Auflagen 3/4): rec3 "18000 Bilder,
0 Aenderungen", "Cuts 5/8/9/10/11/12: 0 Pixel mit Hub > 40", "Notausgang Cut 8 konstant",
"19 Savestates / 18 OT-Walks".

Versucht und NICHT widerlegt (Kernaussagen halten): Original blinkt nicht (rec4 selbst nachgerechnet);
kein zweiter Zeichner (Transfer-Vollzensus, Overlays 0); keine zweite BSS-Aufnahme; kein Primitiv
ueber dem Cut-3-Schild (7 eigene OT-Walks); Port im echten Durchlauf pixelgleich (eigener Lauf);
MZD = Auslieferungsstand fuer ROOM1170/1171 (bytegleich); RE2 bietet kein Blink-Vorbild.
