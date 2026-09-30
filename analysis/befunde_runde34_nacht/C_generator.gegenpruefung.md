# Spur C — Gegenpruefung des Bauplans (Generator ROOM11F0/11F1)

Stand: 2026-09-30, Gegenpruefung VOR dem Bau. Pruefer = Skeptiker, hat `C_generator.md` nicht geschrieben.
Kein Port-Code geaendert. Eigene Belege: `C_belege/gp_*` (Zensus-Skript + Ausgabe, drei kleine Bilder).
Alle Datei-Offsets unten selbst an den Bytes gelesen bzw. selbst disassembliert (`re15_disasm.py`,
`re2_disasm.py`, `scd_dump_room.py`, eigener Zensus `gp_re2_esp16_zensus.py`).

## Urteil: haltbar_mit_auflagen

Der Kern haelt: die neue Sperrregel `sperrt() = aktiv && !geloest && maske == 0x155` (live) entfernt genau
die Runde-31-Zwischensperre, die das Nutzer-Symptom erzeugt, laesst die RE2-Endabnahme (80 + 30 Ruhebilder)
unveraendert und ist softlock-frei; die Lampenregel (`m & 0x1F == 0x15` oben, `m & 0x3E0 == 0x140` unten)
steht in den Ck-Bytes @0x012BE..0x012E2, die Spaltenzuordnung in den Zellen/Props. Widerlegt bzw. falsch
belegt sind drei Aussagen zur **Lampenkunst und ihrem RE2-Vorbild** (die gruene Quadrat-Lampe gibt es in
RE2 nicht, RE2s gruene Lampen sind nicht dauerhaft, und RE1.5 hat sehr wohl eine eigene gruene
Leucht-Kunst). Das aendert nicht die Mechanik, aber Kennzeichnung, Begruendung und einen Teil der
Riegel — deshalb Auflagen, kein Neuplan.

## (a) Tragende Konstanten/Adressen selbst nachgeprueft

### RE1.5 `ROOM11F0.RDT` (152588 B) — 41 zitierte Stellen gelesen

| Stelle | Ergebnis |
|---|---|
| sub16 @0x015C0 `29 0a`, @0x015C2..0x015F2 Set(5,0..12,1) | OK |
| sub01 Bewegungs-Polls @0x01098/0x010B0/0x010C8/0x010E0 `51 01 {01,04,02,08} 00`, Schalt-Poll @0x01106 `51 01 40 00` -> @0x0110A `04 ff 18 06` | OK |
| sub02..05 @0x012F6/0x01302/0x0130E/0x0131A Speed_set +-200 | OK (kein Grenz-Check im Skript: der Cursor kann das Feld verlassen, s. (e)) |
| sub06 @0x01322 `22 05 01 00`, @0x01332 `2f 05 40 00`, @0x01336 `0d 00 04 00 10 00`, @0x01340 `22 05 0d 01`, @0x0135C `22 05 01 01` | OK — Bit erst NACH der 16-Bild-Kippung, Zelle im selben Durchlauf wieder scharf |
| Loesungskette @0x012B6 `06 00 36 00`, @0x012BA `21 04 ee 00`, @0x012BE..0x012E2 (1,0,1,0,1,0,1,0,1,0), @0x012E6 `04 ff 18 12`, @0x012EA `22 04 ee 01` | OK |
| **Tabelle 3.1 Zeile "sub01 @0x012A4..0x012B0"** | um 4 Byte versetzt: @0x012A4 ist `06 00 0e 00` (Ifel_ck); `21 05 0c 01` steht @0x012A8, `22 02 00 01` @0x012AC, `22 02 02 01` @0x012B0. Nicht tragend. |
| sub18 @0x016F6 `22 04 f3 01`, @0x016FA..0x0172A Bits 0..12 aus (13..22 bleiben), @0x01736 `22 02 07 01`, @0x0173A `29 08`, @0x01742 `2b 02 ff ff`, @0x01776 `46 01 01 31 ..`, @0x01784 `22 02 07 00` | OK |
| Zensus `22 02 07 xx` ueber die GANZE Datei | genau 4 Treffer: 0x1736 (=1), 0x1784 (=0), 0x17B8 (=1), 0x180A (=0) — die Aussage "RE1.5 sperrt beim Raetsel nie" haelt |
| sub17 @0x0168C..0x016E4 Bits 0..22 aus | OK |
| sub00 Else @0x0101A `07 00 70 00` -> @0x0101E Aot_set Slot 1 sce 1 (Text), keine Hebel-Props | OK |
| Zellen @0x00D78..0x00E2C (x -27300 Schalter 1..5, x -19700 Schalter 6..10), EXIT-Zelle 12 @0x00E40 (-15300, 15750), Props @0x00E76..0x00FA8, Member_cmp 2..11 -> sub06..15, 12 -> sub17 | OK (eigener Dump) |
| `cmp ROOM11F0.RDT ROOM11F1.RDT` | byte-identisch |
| RVD (@0x2A0, Kopf +0x28): keine Zone mit Ziel-Cut 10..14; Cut 10 nur ueber sub16 @0x015C0, **Cut 12 gar nicht** (kein `29 0c`, kein Cut_replace) | eigener Parse — wichtig fuer Punkt 4 (s. (f)) |

### RE1.5 PSX.EXE

| Stelle | Ergebnis |
|---|---|
| FUN_80030444 @0x800304f4 `lw g_pauseflags` / @0x800304f8 `lui v1,0x100` / @0x80030514 `andi v0,v0,0xf000` / @0x8003051c `sw 0x800ac768` | OK |
| FUN_8003ecec @0x8003ed74 `sw zero,4136(at)` = 0x800b1028 (Bank 5 Wort 0 beim Raumaufbau) | OK; Port scd_room_setup.c:280 `g_game.flags[5][0] = 0` |
| ESP-Spawner FUN_80019700: @0x80019728 `srl t8,a0,24`, @0x80019734 `andi v0,t7,0x7`, @0x8001973c `srl v1,t7,3`, @0x80019754 `sll s0,v1,6`, @0x8001987c/84/88 `lhu v0,4(t5)` / `addu v0,v0,s0` / `sh v0,50(t0)` | selbst gelesen fuer Befund (f)3: RE1.5 rechnet die CLUT-Zeile genauso wie RE2 |

### RE2 `ROOM2130.RDT` (139448 B) — 19 Stellen

@0x00F3C `06 00 56 00`, @0x00F40 `21 04 3c 01`, @0x01110 `22 02 07 01`, @0x01180 `65 01`, @0x0118A
`2b 00 01 00 ff ef`, @0x01192 `36 02 0a 01`, @0x011E0 `0f 06 36 00`, @0x01216 `02`, @0x01294 `64 01 16 02`,
@0x012A4 `09 0a 1e 00`, @0x01708 `10 00`, @0x0171C `09 0a 1e 00`, @0x01752 `23 00 05 00 50 00`, @0x01758
`2b 00 07 00 ff ff`, @0x0175E `22 04 3c 01`, @0x01762 `36 02 0c 01`, @0x017A8 `64 0d 16 10`, @0x017B8
`64 0e 16 10`, @0x01818 `22 02 07 00` — alle OK. Zusaetzlich gelesen (nicht im Dossier): @0x01794 sleep 30,
**@0x01798 `36 02 0f 01` se_on direkt vor dem Gruen-Zuenden**, @0x017C8 sleep 30, @0x017CC `29 04`,
@0x01810 sleep 1, **@0x01814 `65 0d` / @0x01816 `65 0e` = die gruenen Lampen werden am Ende von sub04 geloescht**.

### RE2 PSX.EXE

| Stelle | Ergebnis |
|---|---|
| Tabelle @0x800a74c8: [9] 0x800539dc, [10] 0x80053a24, [0x3A] 0x800565a4, [0x64] 0x80056644 | OK |
| sce_espr_on2 @0x80056664..0x800566c0: a0 = pc[2]<<24 \| pc[3]<<16 \| u16 pc[6]; Lage pc[8..13]; `jal 0x8001c8c4` | OK (0x3A @0x800565a4 identisches Layout, ruft 0x8001bf10) |
| Spawner 0x8001c8c4: @0x8001c8c8 `srl t6,a0,24`, @0x8001c8d8 `andi v0,t5,0x7`, @0x8001c95c `ori v0,zero,0xa003`, @0x8001c9cc `lhu v1,4(t1)`, @0x8001c9e0 `srl v0,t5,3`, @0x8001c9e4 `sll v0,v0,6`, @0x8001c9f8/fc `addu`/`sh v1,50(t0)` | OK |
| TIM-Lader 0x8001bd38: @0x8001bd8c `addiu s4,zero,480`, @0x8001be2c `addiu v0,v0,288`, @0x8001be68 `jal 0x8008f828`, @0x8001be70 `sh v0,4(s0)` | OK |
| Routine 1 @0x8001dc30 (Tabelle @0x8009d868[1]): @0x8001dc3c/44 Flags := Zeile+18, @0x8001dc40/4c Satz := Zeile+2, @0x8001dc48..60 TPAGE \|= Zeile+20 | OK |
| Fortschalten @0x8001d7b8..0x8001d880 (Zaehler 0 -> Satz+1; Dauer 0xFF -> Satz := Byte0) | OK -> Zelle 3,4,3,4 je Bild |
| Zeichnen @0x80077a18 `andi v1,s2,0xa000`, @0x80077a44 `andi v0,s2,0x1000`, @0x80077a4c/50 Prim 0x2C/0x2E | OK |
| Pad-Leser @0x800391F8..0x80039224 (`andi v0,v0,0x3c00`) | OK |

### Port-Code (Reihenfolge, auf die sich die "Live"-Aussagen stuetzen)

`scd_vm_tick()` main.c:5378 laeuft VOR `re15_game_step()` main.c:7358; darin `re15_panel_zeiger_sperrt()`
game_step_common.c:1146 (Padwoerter fuer die NAECHSTE VM) und `re15_panel_zeiger_tick()` :2359. Evt_exec mit
cond 0xFF belegt nur Slots `SCD_EVENT_SLOT_FIRST..LAST` = 10..23 (re15_scd.h:52/53, scd_vm.c:1141ff.), die
Schalter-Subs laufen also im selben Bild NACH sub01 (Slot 1, scd_vm.c:687 aufsteigend). Damit halten
beide Aussagen des Dossiers: das Bit aus b* sperrt schon die Padwoerter fuer b*+1, und ein gehaltenes
Quadrat kann in b* nicht nachzuenden (sub01 sah die Zelle in b* noch unscharf).

### Bilder

`lights.bmp` = `ROOM11F10.bmp` + genau 590 Pixel mit Abweichung > 60, alle rot; Pfeile x 198..218 / y 42..70
und x 196..219 / y 141..172; Glas (Regel des Dossiers) x 216..229 / y 72..79 bzw. 131..140 — alles OK.
**Falsch (nicht tragend):** die Beispielwerte in 3.6/Abnahme 5. `ROOM11F10.bmp` hat @(223,75) = 0x2e3a28 und
@(223,134) = **0x242c1f** (Dossier: 0x2e3629 / 0x2f3926); Port-Framedump F517 (`build/r34n_c/ist1`):
0x2f3825 / 0x252b1d, mittlere Kanalabweichung im Lampenbereich gegen das Original 0,22 (max 2).
Ist-Messung 2.2 (49 Sperrbilder) ist mit dem Code konsistent: Fahrt 20 + Ruhe 30 - 1; Streifen angesehen.

## (b) Passt der Plan zum Nutzerwortlaut und zu den Bildern?

* **"Cursor eingefroren" = Auswahl-Cursor, "Cursor ... auf die 80" = Zeiger:** haelt. Das Nutzerzitat aus
  Runde 26 steht woertlich in `analysis/befunde_2026-09-26/raum11f0-raetsel-cursor.md:13-16`.
* **"bevor das OK kommt und die Lichter angehen":** die Lesart "Lichter = Stationslicht nach der Abnahme"
  haelt — und ist staerker belegt als im Dossier: sub18 schaltet @0x01748 auf Cut 0x0D und @0x01770 auf
  Cut 0x0E; `ROOM11F13.bmp` zeigt dunkle, `ROOM11F14.bmp` dieselben Flure BELEUCHTET
  (`C_belege/gp_cut0D_0E_lichter.png`). Die Lichter "gehen" dort buchstaeblich "an".
* **Restmehrdeutigkeit (nicht weg-argumentierbar):** mit dieser Lesart geht die Lampe der zuletzt
  vervollstaendigten Seite im Bit-Bild b* an, also WAEHREND der Zeiger noch zur 80 faehrt und die Eingabe
  gesperrt ist. Wer "die Lichter" als die gruenen Lampen liest, erwartet sie erst nach der 80. Punkt 4
  ("sobald ... korrekt") gewinnt; das muss in der Rueckmeldung stehen (Auflage 9).
* **Strenge Lampenregel (alle fuenf Schalter der Seite):** haelt — "(falscher Schalter links - das obere
  ...)" macht einen zusaetzlich EIN-geschalteten Schalter 2/4 bzw. 6/8/10 zum Ausloeser fuers Ausgehen.
* **Spalten:** haelt. Zellen x -27300 = Schalter 1..5, x -19700 = 6..10; Artefakt-Gegenprobe (Cursor im
  Bild rechts, Log `maske=040` = Schalter 7); die Nutzerzahl "3 links / 2 rechts" passt nur so herum.
* **Punkt 4 "nach (4,238) dauerhaft gruen? (begruenden)":** falsch begruendet, s. (f)2 — Ergebnis ist aber
  unschaedlich, weil die Buehne danach unerreichbar ist.

## (c) Softlocks, Regressionen, Laden/Speichern, Raumvarianten

* **Endsperre softlock-frei:** Sperre nur bei m = L; nur L zielt auf 80 (r27-Riegel: genau 1 von 1024);
  Fahrt <= 90 Bilder + 30 Ruhe; die Kette @0x012B6 laeuft je Bild (Reseed Slot 1); START ist in der Sperre
  zu (game_step_common.c:1230 `!panel_sperre`), Meldungen kommen in der Phase keine. Eine vor b* gestartete
  Kippung eines anderen Schalters beendet die Sperre nach <= 16 Bildern von selbst. Bestaetigt.
* **Gehaltenes Quadrat:** frei (Zwischenphase) kippt es alle ~17 Bilder neu (0x51 liest das gehaltene Wort,
  Zelle @0x0135C sofort wieder scharf) — RE1.5-Original, bisher von der Runde-31-Sperre verdeckt; in der
  Endsperre ausgeschlossen (Slot-Reihenfolge oben). Muss in Rueckmeldung + Abnahme (Auflagen 7, 9).
* **Vorbestehender Wettlauf (nicht neu):** EXIT innerhalb der 16-Bild-Kippung (Cursor am rechten Rand von
  Zelle 11 -> EXIT-Zelle ab x -15300 in ~12 Bildern) laesst die Kippung nach sub17 weiterlaufen und das
  Schalterbit neu setzen. RE1.5-Skript, auch mit Runde-31-Sperre moeglich; Lampen folgen dann den Bits
  (konsistent). Kein Auftrag, nur Hinweis.
* **Regressionen:** `unit_r31_generator` Teil D wird rot (geplant). Zusaetzlich zu den im Dossier genannten
  `unit_r27_panel_schalterwerte` G / `unit_r26_panel_11f0` A **fehlt `unit_r17_cursor_klick_pin`**
  (probes/r17_spiel-komfort.cmake:19): 60 Bilder gehaltenes Quadrat, danach mehr Kippungen; die Checks sind
  relativ (`kleinster_abstand > 1`, `ok*4 < 60`) — einzeln nachfahren (Auflage 6).
* **Laden/Speichern:** kein Speicher-AOT in 11F0 (main00/sub00 gelesen); Bank 5 Wort 0 wird beim Aufbau
  geloescht (@0x8003ed74), 4:238 ist global — Zustand vollstaendig ableitbar, kein neues Bit noetig. OK.
* **11F1:** byte-identisch, alle Engine-Stellen ueber `RE15_PANEL_IST_RAUM`; live nicht per DEBUG_JUMP
  erreichbar (debug_menu_common.c:318 `room_id >> 4`). Riegel reicht, wenn der PC-Zeichner nur ueber
  `re15_panel_lampe_sicht` (und damit das Makro) entscheidet.
* **Stille Luecke:** fehlt `LAMPE2130.TIM`, sind die Lampen "still aus" — dieselbe Klasse, fuer die
  make_package.sh:190-193 bei TORSE.VBS ausdruecklich "Gate statt Stille" verlangt. Das Paket-Gate ist im
  Plan nur "optional" (Auflage 5).
* **Veraltete Kommentare:** re15_panel_zeiger.h:120-127 ("Eingabesperre waehrend der Fahrt"), :158-160
  (`RE15_PANEL_LOESUNGSMASKE` "nur Doku/Riegel, keine Spiellogik" — wird jetzt Spiellogik), :239-241 und
  game_step_common.c:1139-1145 beschreiben danach Falsches (Auflage 4).

## (d) Ressourcen-Kollisionen (VERTRAG.md, andere Spuren)

* Bank-9-Bits 69/70, AOT 40/41, Nachrichten 28/29: nicht gebraucht — keine Kollision.
* Spur B: Haken in main.c NACH der Prop-Zeichenschleife (B_hebetisch.md:453), liest nur PANEL2130.* und
  aendert panel_zeiger nicht (:521) — keine Zeilen-Ueberschneidung mit Cs Haken hinter dem Zeiger-Block.
* Spur G (ROOM1170, Schrift im Hintergrund): Dossier zum Pruefzeitpunkt noch leer; ein G-Haken direkt hinter
  `re15_bg_blit` waere derselbe Block — Zusammenfuehrung beachten (im Dossier 7.3 schon genannt).
* Gemeinsam fuer alle: `RE15_MIN_TESTS` (local_build.sh:340), Android-GLOB fuer die neue Datei unter
  platform/pc/src. r34g: keine Beruehrung. A/D/E/F: keine gemeinsamen Raeume, Flags, Slots.

## (e) Misst der Abnahmeplan das Nutzer-Symptom am Artefakt?

Ja im Kern: echte exe-Kopie, echter Eingabepfad (DEBUG_JUMP + FIRE_AOT + INPUT_SCRIPT), Cursor- und
Zeigerlage aus den GERENDERTEN Bildern (`cursor_spur.py`), Lampen per Framedump, einmal gdigrab. Luecken:
1. Kein Leck-Test: nach der Abnahme (Cut 8) und in einem anderen Raum muss der Lampenbereich = Hintergrund sein.
2. Gehaltenes Quadrat auf dem LETZTEN Schalter (>= 40 Bilder) ist nur behauptet, nicht gemessen.
3. Der Cursor ist nicht begrenzt (sub02..05); die EXIT-Zelle liegt bei Bild-x ~217 direkt unter den Lampen.
   Steht er ueber einer Lampe, liegt er (Ebene 3) darueber und Abnahme 5/6 misst den Cursor.
4. Abnahme 3 (Zielwechsel waehrend der Fahrt) nur am Log — dazu ein Bildpaar.
5. Abnahme 5 nennt Absolutwerte "Glas-G 0x30..0x3a", die untere Lampe hat G 0x2b/0x2c: nur relativ zum
   Hintergrund desselben Bilds messen.
6. Abnahme 6 vergleicht gegen eine Vorschau nach DERSELBEN Regel (Umsetzungstreue, nicht Richtigkeit) —
   zulaessig, aber nur zusammen mit dem Ansehen des Bilds.

## (f) Besseres Vorbild im Original/RE2 uebersehen?

1. **Die gruene Quadrat-Lampe ist KEINE RE2-Kunst, sondern eine Kombination.** Zensus ESP 0x16 ueber alle
   250 RE2-PL0-RDTs, beide Zuend-Opcodes (0x3A, 0x64), `C_belege/gp_re2_esp16_zensus.txt`: in jedem Raum ist
   Strom 0/1 = Satz 0 = Zellen 0,1,2 (Rechteck), Strom 2/3 = Satz 4 = Zellen 3/4 (Quadrat), Strom 4/5 = Satz 7
   (rund). CLUT-Zeile 2 (gruen) kommt NUR mit Strom 0 vor: ROOM2110 @0x01866/@0x01876 und ROOM2130
   @0x017A8/@0x017B8 (Unterindex 0x10 -> 0x10 & 7 = 0, s. @0x8001c8d8). Zellen 3/4 erscheinen nur rot
   (ROOM2130 @0x00F44.. / @0x01294.., ROOM3030 8x) und blau (ROOM6150 @0x02F28/@0x02F38). Die Form (Zellen 3/4
   = RE2s Schalterlampe, gleiches Bedienfeld-Design mit Platte darunter) ist fuer die quadratischen
   RE1.5-Rahmen gut gewaehlt — aber "Form aus Strom 2 + Palette aus Strom-0-Ereignis" ist PORT-WAHL und muss so
   heissen. Das Dossier (0, 1.2, 3.7, 5.2) verkauft es als gefundene RE2-Kunst.
2. **RE2s Gruen ist voruebergehend, nicht dauerhaft.** sub04: "OK" @0x01758 -> Cut 8 @0x01772 -> rot
   @0x01774/84 -> sleep 30 -> se_on 0x0F @0x01798 -> rot aus, gruen an @0x017A8/B8 -> sleep 30 @0x017C8 ->
   Cut 4 -> **@0x01814/@0x01816 gruen geloescht**. Dauerhaft (sub03 @0x00F3C..) sind die ROTEN Schalterlampen
   (Unterindex 0x02 = CLUT-Zeile 0). Das Dossier zitiert sub03 als Vorbild fuer "gruen dauerhaft" — falsch
   uebertragen. Folgenlos nur, weil in RE1.5 nach der Loesung Cut 10 unerreichbar ist (sub00 @0x0101A,
   RVD ohne Zone nach Cut 10), Cut 12 (zweite Ansicht der Lampen) wird nie angefahren (s. (a) RVD).
3. **RE1.5 hat eine eigene gruene Leucht-Kunst — uebersehen.** ESP 0x01 (14 RDTs, u.a. ROOM1150, 3070, 4010,
   40A0, 5030, 5060, 6020): vier Paletten rot/gruen/orange/blau, Stern-/Glanzform
   (`C_belege/gp_re15_esp01_paletten.png`). Live gezuendet als Anzeige-Lampe an einem Laborpult:
   ROOM5060 sub06 @0x03124 `3a 00 01 08 00 01 00 0d 9c ff 6a fa c8 9c 00 00` (Unterindex 0x08 -> CLUT-Zeile 1
   = GRUEN, Formel RE1.5 @0x8001973c/@0x80019754/@0x80019884) und @0x031C0 `3a 00 01 18 ..` (blau). Das
   Dossier hat nur die ESP von ROOM11F0 und den Sicherungskasten 1050 geprueft ("RE1.5 hat keine"). Ob die
   runde Glanzform auf die quadratischen Rahmen passt, ist eine Abwaegung — aber sie muss im Dossier stehen
   (Beta -> Retail: RE1.5 zuerst, wo es etwas hat).
4. **Ton:** "RE2 zuendet Lampen ohne se_on" stimmt fuer die roten Schalterlampen (@0x01294 -> @0x012A4 sleep).
   Das einzige RE2-Gruen-Ereignis, dessen Palette uebernommen wird, hat dagegen se_on 0x0F @0x01798 direkt
   davor. "Kein Ton" bleibt vertretbar (Seitenlampe = Schalterlampe, nicht Schloss), die Begruendung muss das
   aber sagen.

## Widerlegt

1. "Kunst gefunden ... CLUT-Zeile 2 = gruen (RE2 benutzt sie ... fuer Strom da)" als Beleg fuer eine gruene
   QUADRAT-Lampe: RE2 zeigt Zellen 3/4 nie gruen; Gruen nur Zellen 0..2 (Strom 0), Zensus gp_re2_esp16_zensus.txt.
2. "Lampe an nach der Loesung — Vorbild RE2 sub03 @0x00F3C": RE2s dauerhafte Lampen sind rot; die gruenen
   werden @0x01814/@0x01816 geloescht.
3. "RE1.5 hat keine (Lampenkunst)" / "RE1.5 ist hier unfertig -> RE2" als Ergebnis einer vollstaendigen Suche:
   ESP 0x01 gruen, live in ROOM5060 @0x03124.
4. Nebenfehler: Offsets der Zeile "sub01 @0x012A4..0x012B0" (je +4), Beispielpixel (223,134) = 0x242c1f statt
   0x2f3926.

## Auflagen

1. **Kunst ehrlich kennzeichnen** (Dossier 0/1.2/3.7/5.2, Kopf re15_panel_zeiger.h, Kommentar in
   panel_lampen_pc.c, Commit-Message): "PORT-WAHL (Kombination): Form = RE2-Schalterlampe ESP 0x16 Strom 2 /
   Satz 4 / Zellen 3/4 (ROOM2130 @0x01294, dort ROT); Palette = CLUT-Zeile 2 aus dem Strom-0-Ereignis
   @0x017A8/@0x017B8 (dort Rechteck-Lampe Zellen 0..2). RE2 zeigt die gruene Quadrat-Lampe nirgends
   (Zensus 250 RDTs)." Die Formulierung "Kunst gefunden" faellt weg.
2. **RE1.5-Kandidat ESP 0x01 bewerten** (Abschnitt 3.5 ergaenzen): Vorschau der gruenen ESP-0x01-Glanzform
   (CLUT-Zeile 1, Groesse nach derselben Herleitung) neben `vorschau_k22_z3.png` in C_belege; Wahl mit Grund
   festschreiben (Form des Rahmens vs. RE1.5 zuerst) und in der Rueckmeldung beide Bilder nennen.
3. **Punkt 4 neu begruenden, `|| geloest` streichen oder als PORT-WAHL ohne RE2-Anspruch fuehren:** RE2 haelt
   Gruen nicht (@0x01814/@0x01816); in RE1.5 ist Cut 10 nach der Loesung unerreichbar (sub00 @0x0101A), Cut 12
   ohne RVD-Zone/Cut_chg, im Abnahmebild ist m = L ohnehin. Riegel H entsprechend ("nicht erreichbar"
   statt "Lampen an") oder als reine Robustheit kennzeichnen.
4. **Veraltete Kommentare mitziehen:** re15_panel_zeiger.h:120-127, :158-160 (`LOESUNGSMASKE` wird Spiellogik),
   :239-241 und die eigenen Runde-31-Hakenzeilen game_step_common.c:1139-1145 (eine Zeile "nur Endsperre, s.
   re15_panel_zeiger.h"). Keine fremden Zeilen umformatieren.
5. **Paket-Gate fuer `shared_assets/RE2/LAMPE2130.TIM` verbindlich** (wie TORSE.VBS, make_package.sh:190-193),
   plus Riegel-Check: Datei vorhanden und byte-gleich `ROOM2130.RDT[0x0E398, +4256)`.
6. **Riegelliste ergaenzen:** `unit_r17_cursor_klick_pin` neben r27 G / r26 A einzeln fahren (je 2x bei Rot,
   Memory gui-tests-flattern); r31 D umschreiben wie geplant, Soll-Zahlen mit @0x-Beleg.
7. **Abnahme ergaenzen:** (a) Framedump nach der Abnahme in Cut 8 und in einem Nachbarraum: Lampenrechteck =
   Hintergrund; (b) gehaltenes Quadrat >= 40 Bilder auf dem letzten Schalter: in der Endsperre 0
   Schalterwechsel, Abnahme k+31; (c) in allen Lampen-Messbildern Cursor-x < 200 nachweisen (Cursor ist
   unbegrenzt, EXIT-Zelle bei x ~217); (d) Abnahme 3 mit Bildpaar; (e) Abnahme 5 nur relativ zum
   Hintergrund desselben Bilds (untere Lampe G 0x2b/0x2c, nicht 0x30..0x3a).
8. **Ton-Begruendung korrigieren:** "kein Ton" mit dem Hinweis auf se_on 0x0F @0x01798 (RE2-Gruen-Ereignis =
   Schloss) und warum die Seitenlampe der stillen Schalterlampe (@0x01294) folgt.
9. **Rueckmeldung an den Nutzer** (kurz, mit Bild): (i) die Lampe der zuletzt richtig gestellten Seite geht im
   Bild des Schalterbits an — vor der 80, waehrend die Eingabe gesperrt ist; (ii) "die Lichter" nach dem OK =
   die Flure (Cut 0x0D dunkel -> 0x0E hell), unveraendert; (iii) Quadrat GEHALTEN legt einen Schalter alle ~17
   Bilder erneut um (RE1.5-Original, vorher von der Sperre verdeckt); (iv) die untere Lampe gehoert zur
   RECHTEN Spalte, obwohl sie auf Hoehe Zeile 3/4 sitzt; (v) Groesse 22 px ist PORT-WAHL.
10. **Nebenfehler im Dossier berichtigen:** Tabelle 3.1 Offsets (@0x012A4 `06 00 0e 00`, @0x012A8/AC/B0),
    Beispielpixel 3.6 (0x2e3a28 / 0x242c1f).
