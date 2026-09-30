# Spur C (Generator ROOM11F0/11F1) - ABNAHME 1 (unabhaengig)

Stand: FERTIG (2026-09-30). Pruefer: Abnahme-1-Agent, hat den Code NICHT geschrieben. Ziel war,
zu widerlegen, dass Spur C fertig ist.

## 0. Urteil

**ABGENOMMEN** — kein Blocker, kein wesentlicher Mangel; drei kosmetische Punkte (§6).

Widerlegungsversuche, alle an der ECHTEN exe (eigene Kopie `re15_abn1c.exe`, Spielcode 42baac77,
`RE15_WINDOW_SCALE=3`, Framedumps angesehen) bzw. am Code/an den Bytes:

* Eigener Volllauf `local_build.sh` (configure+build+test): **429/429**, `LOCAL-BUILD-OK (all)`,
  unter Parallellast anderer Baeume, ohne einen einzigen roten exe-Haken (`C_belege/abn1_suite.txt`).
* Nutzerpunkt 3 (frei / Endsperre bis 80 / OK / Lichter) und Nutzerpunkt 4 (obere Lampe = links,
  untere = rechts, falscher Schalter -> aus) halten in fuenf eigenen Laeufen R1..R5, darunter die
  vom Bauer NICHT live gefahrenen Faelle: kompletter Ablauf NACH "Power supply OK." (Meldung weg,
  Flackern Cut 0x0D/0x0E, Steuerung zurueck, Zombies wach), falscher Schalter RECHTS, Statusschirm
  (START) bei brennender Lampe, EXIT + Wiedereintritt, **ROOM11F1 live** (per `RE15_GOTO_ROOM`,
  das Dossier hielt das fuer unmoeglich), Zeiger faehrt durch die 80 in beide Richtungen ohne
  Sperre/Abnahme, eine vor b* begonnene Kippung bricht die Endsperre auf und die Sperre kommt beim
  erneuten Loesen wieder (Abnahme erneut genau k+31).
* RE-Gate: alle tragenden Konstanten gegen die Rohbytes nachgelesen (ROOM11F0/11F1, ROOM2130,
  LAMPE2130.TIM); die zwei Setzungen ohne Original (Kante 22 px, Kunst-Kombination) sind korrekt
  als PORT-WAHL mit Herleitung gekennzeichnet.

## 1. Gelesener Stand (git log, Dossier, Gegenpruefung)

* Zweig `r34n/generator`, HEAD `71057ac1` (= Cherry-pick Bau-Fix 7d4d11dd) auf `f0ff11cf`
  (Abschluss Spur C). Spielcode seit `42baac77` unveraendert (`git diff --stat 42baac77..HEAD --
  re15_port/engine re15_port/platform re15_port/include` = leer) -> die exe-Messungen des Bauers
  (§9.6, Kopie `re15_r34nc.exe`) gelten fuer den heutigen Spielcode.
* Geaendert gegen die Basis cf0e68ba (Code): `panel_zeiger_common.c` (Sperre = nur noch
  `aktiv && !4:238 && maske == 0x155`, Lampenzustand je Tick), `re15_panel_zeiger.h` (Kopf,
  Konstanten), NEU `platform/pc/src/panel_lampen_pc.c` (+ `shared_assets/RE2/LAMPE2130.TIM`),
  2 Zeilen `main.c` (Haken hinter dem Zeiger-Block), Kommentar `game_step_common.c`, Gate in
  `release/make_package.sh`, Riegel `unit_r34n_c_generator` + Umbau `r31_generator` Teil D.
* Dossier `C_generator.md` §0..§9 und Gegenpruefung (haltbar mit 10 Auflagen, alle als
  ERLEDIGT abgehakt §9.1) gelesen. Vorhandene Belege des Bauers: S1..S7 + gdigrab
  (`C_belege/bau_*`). Live NICHT gefahren (§9.9 Nr. 8): ROOM11F1, der Ablauf NACH "Power supply
  OK." (Meldung wegdruecken, Flackern Cut 0x0D/0x0E, Rueckgabe der Steuerung), Wiedereintritt/
  EXIT mit brennender Lampe, Statusschirm (START) bei brennender Lampe, falscher Schalter
  RECHTS an der exe (nur Riegel F).
* Selbst nachgelesen (Bytes): `LAMPE2130.TIM` == `ROOM2130.RDT[0xE398, +4256)` (True),
  Kopfwort[20] = 0xE398; effect.esp @0x3188 `16 ff ..`; Zellen @+0x70 (`60 00 f0 f0` / `80 00
  f0 f0` = Zelle 3/4); Anim-Saetze @+0x10: Satz 3 `00 01 ff 20` (Sprung auf Satz 0), Satz 4
  `03 01 01 20`, Satz 5 `04 01 01 20`, Satz 6 `04 01 ff 20` (Sprung auf Satz 4) -> Zelle 3,4,3,4;
  @0x01294 `64 01 16 02 00 00 ba 02`, @0x017A8 `64 0d 16 10 .. b4 0b`, @0x017B8 `64 0e 16 10`,
  @0x01814 `65 0d 65 0e 22 02 07 00`. RE1.5 ROOM11F0 sub06/sub16/sub17/sub18 im Dump des
  Bauers (`build/r34n_c/room11f0_scd.txt`) gelesen: sub17 (EXIT) setzt Cursor + alle zehn Hebel
  zurueck (@0x01602 Pos_set, @0x0160E..0x0167A Dir_set) und loescht Bank 5 Bits 0..22.

## 2. Eigener Bau + Suite-Zeile

`bash re15_port/tools/local_build.sh` (all) im Baum r34n_generator, 08:14-08:30, parallel liefen
Suiten/Messlaeufe von r34g_int, r34n_rolltor, r34n_leichen (Prozessliste gesehen):
`ninja: no work to do` (exe 04:35 = Stand 42baac77), dann
**`100% tests passed, 0 tests failed out of 429` / `=== LOCAL-BUILD-OK (all) — Tests 429/429`**
(812,9 s). Panel-Riegel einzeln im Log gruen: unit_r34n_c_generator, unit_r31_generator,
unit_r27_panel_schalterwerte, unit_r26_panel_11f0, unit_r17_cursor_klick_pin,
unit_gen_11f0_switches, unit_gen_11f0_cursor_view, unit_11f0_cut_after_puzzle. Auch alle
Integrations-Haken mit echter exe liefen durch (kein `exit=1` wie in den Laeufen des Bauers vor
dem Bau-Fix 7d4d11dd). Beleg `C_belege/abn1_suite.txt`.

## 3. Nutzerpunkte aus AUFTRAG.md an der echten exe

Werkzeug: `C_belege/abn1_lauf.sh` (DEBUG_JUMP 11F0, FIRE_AOT Slot 1 = sub16, Meldungen blaettern,
"Ja" -> Panel offen, danach Cursor/Schalter NUR per D-Pad/Quadrat ueber RE15_INPUT_SCRIPT, dazu
RE15_PANEL_LOG + RE15_STATE_LOG), `C_belege/abn1_lauf_11f1.sh` (dasselbe in ROOM11F1),
Auswertung `C_belege/abn1_auswertung.py` (gruene Pixel in den Lampenquadraten unabhaengig vom Cut
+ dG gegen ROOM11F10.bmp) und `re15_port/tools/r34n_c/lampen_spur.py` des Bauers (Cursor/Zeiger aus
dem Bild). Zustandswechsel aller Laeufe: `C_belege/abn1_laeufe_zustand.txt`. Bild F zeigt den
Log-Stand F-1 (Zeichnen am Schleifenanfang).

### 3.1 Freie Bewegung nach einem Schalter (keine Sperre)

Nicht nur den S1-Lauf des Bauers gelesen (Streifen `bau_s1_streifen.png` angesehen: Cursor faehrt
F529..F559 durch, Zeiger steigt 0 -> 20), sondern selbst gemessen in R2: Schalter 7 setzt sein Bit
F583 (Ziel 50); WAEHREND der Zeiger 37 -> 50 faehrt, faehrt der Cursor im gerenderten Bild
y 89.7 (F590) -> 81.7 (F595) -> 65.0 (F600) -> 63.5 (F605), Zeiger y 131 -> 115, und das Quadrat
auf Schalter 6 bei F609..F611 greift (Bit F626). Unter der alten Runde-31-Regel waere F583..F632
gesperrt gewesen. `panelsperre=1` in R2 und R4 in KEINEM Bild (Summe 0). **Haelt.**

### 3.2 Endsperre bis 80 + Ruhezeit, dann OK + Lichter

R1 (Eingaben wie S2 des Bauers, dann weiter): Bits 7 F532, 9 F563, 3 F618, 1 F644, 5 F693 = b*;
`panelsperre=1` genau F693..F742 (50 Bilder), Zeiger 61 -> 80 bei F712 = k, Abnahme `geloest=1`,
`bestaet=1` bei F743 = k+31, Cut 8 + "Power supply OK." ab F744 (Bild F760). **Neu gegenueber dem
Bauer:** Quadrat bei F783 schliesst die Meldung (msg 1 -> 0 F784), Cut 0x0D F786, Cut 0x0E F798 (Bild
F800: beleuchtete Flure = "die Lichter"), sub18 loest die Pad-Sperre F837 (pf 01000007 -> 00000007,
@0x01784), Cut 8 F838; danach Steuerung zurueck: RECHTS dreht Leon ab F984 (rot 0 -> 2880), HOCH
laeuft er ab F1014 (PL 250,250 -> -695,682); die vier Zombies aus sub00 stehen danach auf (state 1),
wie sub18 @0x0178C..0x017B0 es will. Bestaetigungston genau einmal (`bestaet` bleibt 1). Kein
Lampen-Leck in Cut 8/0x0D/0x0E (gruene Pixel 0 in allen Bildern F760..F1140; der erste Cut-8-Frame
F744 war im S2-Lauf des Bauers ebenfalls ohne Gruen). Bild `C_belege/abn1_r1_ende.png`. **Haelt.**

Lesart "Cursor ... auf die 80" = roter Zeiger und "die Lichter" = Flurlicht nach dem OK (Dossier
§1.2) ist am Wortlaut und an der Runde-31/26-Wortwahl des Nutzers begruendet; die Restmehrdeutigkeit
(die Lampe der zuletzt richtig gestellten Seite geht schon bei b* an, vor der 80) steht in der
Rueckmeldung §9.8 (i). Kein Mangel.

### 3.3 Lampen-Zuordnung links/rechts

Bytes (selbst): Ck-Kette @0x012BE..0x012E2 = Schalter 1..10 gegen 1,0,1,0,1,0,1,0,1,0; Zellen
x -27300 = Slots 2..6 (Schalter 1..5), x -19700 = Slots 7..11 (Schalter 6..10), Aot_set
@0x00D78..0x00E2C. Bild: in R2 (rechte Hebelspalte, Cursor-x ~160) zuendet 9+7 die **untere**
Lampe (F583), +6 (falscher Schalter RECHTS) loescht sie im Bitbild F626, -6 zuendet sie wieder
(F658); die obere bleibt dabei dunkel. In R4 bleibt die obere Lampe mit 1+5 (ohne 3) dunkel, die
untere geht mit 7 AUS aus (F869). Linke Spalte -> obere Lampe: S3 des Bauers (3,1,5 -> oben an F687,
+2 -> aus F765, -2 -> an F804; `bau_s3_lampen_zoom.png` angesehen). Zustand je Bild aus den Bits,
kein Einrasten (R5: obere Lampe AUS im Bild, in dem Schalter 4 dazukommt, F706). **Haelt.**

### 3.4 Lampen-Kunst / Lage / Zeichenreihenfolge

* Lage: `C_belege/abn1_lage_vergleich.png` = lights.bmp (Pfeile) neben Port-Bild R1 F740, gleicher
  Ausschnitt x 196..250 / y 40..175, 4-fach: beide gruenen Lampen sitzen genau in den zwei Rahmen,
  auf die die Pfeile zeigen.
* Kunst: RE2 ESP 0x16 Zellen 3/4 in CLUT-Zeile 2, additiv — Bytes nachgelesen (§5). Als PORT-WAHL
  (Kombination) gekennzeichnet, Alternative RE1.5 ESP 0x01 bewertet (`esp01_vergleich.png`
  angesehen: die RE1.5-Glanzleuchte waere ein schwacher Stern mit grossem Hof, die gewaehlte
  Quadratlampe fuellt das Glas wie die RE2-Schalterlampe `re2_2130_lampen_rot.png`).
* Wechsel Zelle 3/4 je Bild: im Log `lzelle` 3/4 abwechselnd, im Bild dG 192.5/168.1 (unten) bzw.
  195.6/174.9 (oben) abwechselnd, Zellvergleich 3:0.6 / 4:0.6 (lampen_spur.py auf R2).
* Zeichenreihenfolge: Ebene 1 (Framebuffer) nach BG und Zeiger. Gegenprobe R2: der freie Cursor
  (3D, Ebene 3) liegt auf dem Weg zum EXIT UEBER der Lampe (F830 oben, F855 unten) — erwartetes
  Verhalten (Cursor oben). Statusschirm (START) in der freien Phase: R2 F675/F745 Ueberblendbilder
  (Lampe dunkelt mit der Szene ab), F680..F740 Statusschirm deckend, **keine Lampe darueber**
  (Bild F700 in `abn1_r2_rechts_start_exit.png`); nach dem Schliessen (F750) brennt die untere Lampe
  wieder, Maske unveraendert 140. **Haelt.**

### 3.5 Varianten 11F0/11F1, Laden/Speichern, Wiederbetreten, nach der Loesung

* **ROOM11F1 live (neu):** `RE15_GOTO_ROOM=11F1` (main.c :7624-7645, derselbe g_room_change-
  Verbrauchspfad wie Tuer und Debug-Sprung; debug.log: `[goto] queued room change -> ROOM11F1`,
  `[room] PC loaded room11f1.rdt`, `[fire-aot] slot=1 at F40 (Raum 11F1)`). Mit den R1-Eingaben:
  raum=11F1, Bits 7 F532 / 9 F563 (untere Lampe an) / 3 F618 / 1 F644 / 5 F693 = b* (Sperre, beide
  Lampen), Abnahme F743 = k+31, Cut 8 F744 — bildgenau wie 11F0 (`C_belege/abn1_r3_11f1.png`).
* Wiederbetreten des Panels (R2): EXIT bei F885 (sub17) -> aktiv 0, Maske 000, beide Lampen aus,
  Zeiger faehrt verdeckt auf 0; zweites Oeffnen ueber RE15_SUBSTART 16 -> Panel ab F1176: Maske 000,
  Lampen aus, Hebel zurueckgesetzt (sub17 @0x0160E..0x0167A Dir_set), Zeiger 0, Cursor in der
  Startzelle (Bild F1180); Schalter 6 dort -> Maske 020, Lampen richtig aus.
* Laden/Speichern: kein Speicherpunkt in 11F0/11F1 (main00/sub00 gelesen), Bank 5 Wort 0 wird beim
  Raumaufbau geloescht (@0x8003ed74, von der Gegenpruefung nachgelesen), 4:238 ist global — der
  Lampenzustand ist vollstaendig aus Flags abgeleitet; Riegel H prueft den Wiedereintritt nach der
  Loesung. Nicht live gefahren (kein Speicherstand an dieser Stelle noetig: es gibt keinen
  Spur-C-Zustand, der gespeichert werden muesste).
* Nach der Loesung: R1 zeigt, dass Cut 10 nach "OK" nicht wiederkommt (Cut 8 -> 0x0D -> 0x0E -> 8,
  Slot 1 danach Text @0x01776); "Lampen dauerhaft gruen" waere unsichtbar. Begruendung des Bauers
  (RE2 loescht Gruen @0x01814/@0x01816, Bytes selbst gelesen) haelt.

## 4. Gegenproben (falsche Eingaben, Durchlaufen der 80, Nachbarverhalten)

| # | Gegenprobe | Lauf | Ergebnis |
|---|---|---|---|
| G1 | Zeiger faehrt nur DURCH die 80 (kein Ausloesen) | R4: 7, 9, 5, 1 = Maske 0x151 (Ziel 90), spaeter 7 aus = 0x111 (Ziel 70) | Durchfahrt aufwaerts F734 (79/80/81) und abwaerts F878 (81/80/79): `panelsperre=0` in allen Bildern (Summe 0), `geloest` bleibt 0 |
| G2 | Endsperre wird von einer VOR b* begonnenen Kippung aufgebrochen, dann erneut geloest | R5: Schalter 5 (b* F693) und sofort 4 (Kippung ab ~F690) | Sperre F693..F705, Bit 4 F706 -> Maske 0x15D, Sperre faellt im selben Bild, obere Lampe aus, Ziel 50; 4 zurueck F798 -> Sperre wieder, Zeiger 51 -> 80 bei F827, Abnahme F858 = k+31, ein Ton. Kein Haenger, kein Doppel-Ausloesen |
| G3 | Falscher Schalter RECHTS | R2 | untere Lampe aus im Bitbild F626, wieder an F658 |
| G4 | Statusschirm (START) in der freien Phase | R2 | oeffnet (RE1.5 sperrt dort nicht), deckt die Lampe ab, Zustand nach dem Schliessen unveraendert; in der Endsperre ist START gesperrt (game_step_common.c:1231 `!panel_sperre`, Code gelesen) |
| G5 | EXIT mit brennender Lampe + Wiederoeffnen | R2 | alles zurueckgesetzt (§3.5) |
| G6 | Nachbarraum / fremder Cut | R1 (Cut 8, 0x0D, 0x0E), S5 des Bauers (11E0) | 0 gruene Pixel ausserhalb Cut 10 |
| G7 | Raumvariante | R3 (11F1) | identisch zu 11F0 |
| G8 | Andere Panel-Raeume (1080/10D0/1100/11E0/...) | Code | Sperre und Lampen nur ueber `RE15_PANEL_IST_RAUM` (0x11F0/0x11F1); grep ueber engine/platform: keine weitere Stelle |

Nicht nachgefahren (vorbestehend, kein Spur-C-Code): der von der Gegenpruefung genannte Wettlauf
"EXIT innerhalb der 16-Bild-Kippung setzt das Bit nach sub17 neu" (RE1.5-Skript). Die Lampen folgen
dann dem Bit (konsistent mit dem Zeiger, nicht mit dem zurueckgesetzten Hebel) — nur Hinweis.

## 5. RE-Gate-Pruefung des Codes

Gelesen: `panel_zeiger_common.c` (ganz), `re15_panel_zeiger.h` (Diff), `panel_lampen_pc.c` (ganz),
`main.c` :5270-5300, `game_step_common.c` :1139-1151 / :1231, `scd_vm.c` :4905-4955 (Klickton),
`r31_generator.c` Teil D (Diff), Check-Liste `probe_r34n_c_generator.c`.

| Konstante / Regel | Wert | Beleg im Code | selbst gegen Bytes geprueft |
|---|---|---|---|
| Endsperre | `aktiv(5:0) && !4:238 && maske == 0x155` (LIVE) | NUTZER-VORGABE (Wortlaut im Kopf), Sperrwirkung RE2 @0x01110..@0x01818 / RE1.5 @0x800304f4..0x8003051c | ROOM11F0 `22 02 07 xx` nur @0x1736/0x1784/0x17B8/0x180A (sub18/sub19) -> RE1.5 sperrt beim Schalten nie: OK |
| Lampe oben / unten | 0x01F/0x015, 0x3E0/0x140 | Ck @0x012BE..0x012E2 | Rohbytes `21 05 0d 01 .. 21 05 16 00` = 1,0,1,0,1,0,1,0,1,0: OK |
| Zellen 3/4, S 32 | u 96 / 128 | effect.esp @0x7C/@0x80 | `60 00 f0 f0` / `80 00 f0 f0`: OK |
| Takt 3,4,3,4 je Bild | Takt 0 = Zelle 3 | Anim-Saetze 4..6, FUN_8001d68c | Saetze `03 01 01 20` / `04 01 01 20` / `04 01 ff 20` (Satz 3 `00 01 ff 20` springt ebenso auf Satz 0): OK |
| Palette CLUT-Zeile 2 | gruen | Unterindex 0x10 @0x017A8/@0x017B8 | `64 0d 16 10`, `64 0e 16 10`: OK |
| Asset LAMPE2130.TIM | 4256 B | ROOM2130.RDT @0x0E398 | byte-gleich, Kopfwort[20] = 0xE398: OK |
| Additiv B+F, Modulation 0x80 | — | @0x8001dc3c..60, @0x80077a44..50, @0x800783cc/d0 | nicht selbst nachdisassembliert (von der Gegenpruefung Befehl fuer Befehl bestaetigt, gp §(a) RE2 PSX.EXE) |
| Lage (212,65)/(212,125) | Glasmitte (222.5,75.5)/(222.5,135.5) | am Original-BG ROOM11F10.bmp gemessen (Datei-Beleg) | lights.bmp: Rahmen x 213..231 / y 67..85 bzw. 127..143 (Pfeile zeigen darauf): OK |
| Kante 22 px | — | ausdruecklich PORT-WAHL mit Herleitung aus RE2 (FUN_80077ed0-Formel, scale16 0x02BA @0x01294+6) | Kennzeichnung gemaess VERTRAG §3.1 zulaessig |
| Kunst (Form RE2-Quadrat + Palette gruen) | — | ausdruecklich PORT-WAHL (Kombination), Zensus 250 RDTs | Kennzeichnung korrekt (Auflage 1) |
| Ruhe 30 Bilder, Fahrt 1/Bild | unveraendert aus Runde 31 | RE2 @0x0171C / @0x01216 | — |

Keine Rate-Woerter ("plausibel", "interim", "tunable", "etwa", TODO) in den drei Spur-C-Dateien
(grep leer). `s_lampe_takt > 0x3FFF -> &= 1` ist ein Ueberlaufschutz ohne Verhaltenswirkung
(nur die Paritaet zaehlt). `RE15_PANEL_LAMPE_ZELLE_B` ist definiert, aber unbenutzt (der Zeichner
nimmt `z == ZELLE_A ? 0 : 1`) — rein kosmetisch.
**Ergebnis RE-Gate: kein Verstoss gefunden.**

## 6. Maengelliste

| # | Titel | Schwere | Beleg |
|---|-------|---------|-------|
| K1 | Dossier behauptet, ROOM11F1 sei live nicht fahrbar (§2.4, §8 Nr. 6, §9.6 Zeile 8, §9.9 Nr. 8) — es geht ueber `RE15_GOTO_ROOM=11F1` (main.c :7624-7645); gemessen: identisch zu 11F0 | kosmetisch | R3, `C_belege/abn1_r3_11f1.png`, `abn1_laeufe_zustand.txt` Abschnitt r3_11f1 |
| K2 | `local_build.sh` :148-153 (Commit a2c3ec22): Kommentar "ohne es ... faellt der Pfadfilter auf das GLOBALE taskkill zurueck" ist nach dem Cherry-pick 7d4d11dd ueberholt (absoluter PowerShell-Pfad, kein /IM-Rueckfall mehr, :305-311); die PATH-Erweiterung ist nur noch redundant. Bei der Zusammenfuehrung den master-Stand behalten | kosmetisch | `git diff 7d4d11dd HEAD -- re15_port/tools/local_build.sh` |
| K3 | `RE15_PANEL_LAMPE_ZELLE_B` (re15_panel_zeiger.h) ist definiert, wird aber nirgends benutzt; der Zeichner schreibt u0 96/128 als Literale (mit Byte-Beleg @0x7C/@0x80) | kosmetisch | `panel_lampen_pc.c:85`, `:153` |

Keine blocker, keine wesentlichen Maengel. Hinweise ohne Mangelcharakter (vom Bauer selbst als
offen gefuehrt, §9.9): PSX-Zeichner der Lampe fehlt (PSX-Bau laeuft derzeit ohnehin nicht);
Android braucht einen frischen Configure fuer die neue Datei — `release/build_android.sh` verwirft
`app/.cxx` ohnehin, und `platform/android/jni/CMakeLists.txt:42` sammelt `platform/pc/src/*.c` per
GLOB (gelesen); Haken-Zeile in main.c hinter dem Zeiger-Block bei der Zusammenfuehrung mit Spur G
beachten.

## 7. Belege (Dateien unter C_belege/)

| Datei | Inhalt |
|---|---|
| `abn1_lauf.sh`, `abn1_lauf_11f1.sh` | Messlaeufe (eigene exe-Kopie, RE15_PANEL_LOG + RE15_STATE_LOG) |
| `abn1_auswertung.py` | Framedump-Auswertung (gruene Pixel je Lampenquadrat, dG, Log F-1) |
| `abn1_suite.txt` | eigener Volllauf 429/429 + Panel-Riegel |
| `abn1_laeufe_zustand.txt` | Zustandswechsel R1..R5 (Panel-Log), Durchfahrten der 80 (R4), Spielerlage/Kamera/Pausenwort (R1) |
| `abn1_r1_ende.png` | R1: F740 beide Lampen + Zeiger 80, F760 "Power supply OK.", F800 Cut 0x0E, F840 Cut 8, F1040 Leon laeuft |
| `abn1_r2_rechts_start_exit.png` | R2: untere Lampe an / aus (falscher Schalter 6) / an, Statusschirm, Cursor ueber Lampe, Wiedereintritt |
| `abn1_r3_11f1.png` | R3 ROOM11F1: untere an, beide an, Cut 8 |
| `abn1_lage_vergleich.png` | lights.bmp (Nutzerpfeile) neben dem Port-Bild mit beiden Lampen |

Mess-Rohdaten (PPM, Logs) liegen unversioniert unter `build/r34n_c_abn1/r1..r5` im Baum.
