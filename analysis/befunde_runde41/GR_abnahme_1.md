# Spur GR — Unabhaengige Abnahme 1 (nach Nachbesserung 1), Runde 41, 2026-11-02

Pruefer: unabhaengiger Abnahme-Agent (nicht der Autor). Gegenstand: Zweig `r41/granate`, Kopf 8492d3594, Basis 2f886e91a.
Werkzeuge (eigene, nicht die des Autors): `scratchpad/abn/cap.ps1` (Start per `Start-Process`, eigene PID, gdigrab `hwnd=` NUR
am eigenen Fenster, 30 B/s, beschleunigter Renderer, ohne AUTOSHOT/SOFTWARE_RENDER/TITLE_SHOT, echter Titel
`RE15_TITLE_CONFIRM_MS=1500` + echte Spielerwahl `RE15_PSELECT_AUTO=1`, Raum per Debug-Menue `RE15_DEBUG_JUMP`), `sheet.py`/
`strip.py` (Bildleisten, sequentiell dekodiert), `teil.py` (Auswertung `[gr-teil]`). Vergleichs-exe: `re15_pc_r41GR_vorher.exe`
(Baum des Autors, 11:04, enthaelt keinen GR-String: `RE15_FORCE_GRANATE` 0x, `Zeile %u: ZERFETZT` 0x) und die exe des
Hauptbaums (master-Stand 2026-11-01 08:21, nicht vom Autor gebaut).

## URTEIL: BESTANDEN (mit Maengeln 1-3, keiner blockierend)

Die drei Nutzerauftraege sind im echten Fenster erfuellt: die Handgranate zerfetzt jeden Zombie im Wirkradius, auch die dicken
(0x11), unter RE2-KI und RE1.5-KI; die Stuecke fliegen ~2,2-3,0 m hoch, landen auf dem Boden und bleiben liegen; der Hund
zerfetzt (beide KIs). Tests gruen. Der als offen gemeldete "Kanal-Fehler" ist KEIN Zeichenfehler (Abschnitt 3).

## 1. Bau und Tests (Pruefauftrag 6)

* Bau: `bash re15_port/tools/local_build.sh configure` + `build` im Worktree-Baum (kein zweiter Baum). Vorher alle in
  `2f886e91a..HEAD` geaenderten .c/.h per `touch` erneuert -> 793 Uebersetzungen, `re15_pc.exe` 16:15, 8 711 134 Byte.
* Volle Unit-/Prop-/Probe-Suite `ctest -E ^integration_ -j 5 --timeout 1500` (msys64 vorn im PATH, git im PATH):
  **1077/1077 gruen im ERSTEN Lauf**, 1097 s.
* `integration_r34_granaten` / `integration_r35_granate` / `integration_r36_granate` (`-j 3`): **3/3 gruen** (237 / 165 / 94 s).

## 2. Sichtlaeufe im echten Fenster (Pruefauftrag 1)

| Lauf | Aufstellung | Ergebnis (Log) | Bild (GR_belege/abnahme1/) |
|---|---|---|---|
| a1 | ROOM1140, Leon, RE2-KI, ECHTER TIEF-Wurf (Aufstellung integration_r35 "zombie") | 4x `[gr] RE2 ... ZERFETZT` (0x10 -950/-920, **0x11 -750/-750**). 28 Stuecke, Scheitel y -2259..-2992, alle enden auf `boden=0` (st 2->1) nach 21-28 Bildern | `a1_leon1140_re2ki_echterwurf_ablauf.jpg` (obere Zeile), `a1_leon1140_liegen_plus5s.jpg` |
| a2 | dieselbe Aufstellung, RE1.5-KI | 3x `[gr] RE1.5 ... ZERFETZT (RE2-Modellblock)` (0x10 -935, 0x11 -750 x2); Slot 2 ausserhalb des Radius (eingriffe=3). 21 Stuecke, Scheitel -2218..-2870, alle auf `boden=0` | dieselben Bilder (untere Zeile) |
| e4 | ROOM1140, Leon, Granatwerfer Explosivrunde (Item 15), 4 Schuss | 0x10 Slot 2/3 `ZERFETZT` (-150/-170); **0x11 ueberleben mit 50 und 0 HP** (bekannt, Dossier N1.3) | `a1_leon1140_werfer_explosivrunde.jpg` |
| e1 | ROOM1041, **Elza** (PL04), Granatwerfer Explosivrunde | `[gr] RE2 Slot 1 Typ 0x16 Zeile 9: ZERFETZT (HP -21)`; Slot 2 (0x16) ueberlebt mit 0 HP. Kamera zeigt nur Elza am Rand (wie beim Autor) -> nur Logbeleg | — |
| d1 | ROOM1230, Hund, RE2-KI, ECHTER Wurf (zuendet an der Wand, trifft den Hund), `RE15_FORCE_CUT=6` | `[gr] Hund Slot 1 Waffe 9: ZERFETZT (Teile-Maske 0x079c)`, 7 Parts, Scheitel -2222..-2539, alle `boden=0` | `a1_hund1230_re2ki_echterwurf_neu_vorher.jpg` (oben neu, unten 2f886e91a) |
| d6 | ROOM1230, Hund, RE1.5-KI, Mess-Haken `RE15_FORCE_GRANATE=45:1` (echter Wurf traf nicht, eingriffe=0) | `ZERFETZT (HP -927)`, 7 Parts, alle `boden=0` | `a1_hund1230_re15ki_messhaken.jpg` |
| s2 | ROOM2030, Spinne, RE1.5-KI, Werfer-Explosivrunde (neu in GR) | Spinne Slot 1 Zustand 7, `ss1=9` (15 -> 9 wie im Code), gelbe Splitter | `a1_spinne2030_re15ki_werfer.jpg` |
| s1 | ROOM2030, Spinne, RE1.5-KI, Handgranate | 2. Wurf trifft: HP -871, platzt | — |
| s3 | ROOM2030, Spinne, **RE2-KI** (Vorgabe), Handgranate | HP 110 -> 50 -> -10: **erst die 2. Granate toetet**, dann platzt sie | `a1_spinne2030_re2ki_zweite_handgranate.jpg` |

Pruefpunkte:
* **Zerfetzt, kein heiler Koerper bleibt stehen:** a1/a2 — am Explosionsort steht ab +0,3 s keine Figur mehr; ab +3 s liegen nur
  Stuecke in Blut (`a1_leon1140_liegen_plus5s.jpg`).
* **Teile fliegen glaubhaft, liegen danach am Boden, nicht schwebend/in Waenden:** Fluglog aller 63 Stuecke (a1 28, a2 21, d1 7,
  d6 7): jedes endet auf dem Boden unter sich (`boden=0`, Physik steht, st 1). Im Bild keine Stuecke in Wand-/Moebelflaechen.
* **Bleiben bis zum Raumwechsel:** a1 bis zum Ende der Aufnahme (+5 s), d1 bis +15 s (Bild 1218) unveraendert.
* **Kein Flackern:** mittlere Bild-zu-Bild-Differenz im Bereich der liegenden Stuecke a1 +3..+5 s 0,13 (max 0,58, 57 Bildpaare);
  d1 +3..+15 s 0,86 (max 1,45; Rest = Flamme, siehe 5.1).
* **Rauch/Blitz:** sichtbar; Rauch steht in 1140 ~1,5 s (+0,43..+1,9 s) als fast deckende dunkle Flaeche ueber der rechten
  Bildhaelfte — im Video eine Saeule in der (ohnehin dunklen) Tunnelmitte. Gestaltung, kein Mangel; Blitz weiss-orange statt
  weiss-gruenlich, wie im Dossier offen benannt.
* **Video oben / Port unten:** `a1_vergleich_video_cd2_oben_port1140_unten.jpg` (CD2 #576-#630 gegen 1140 RE2-KI und RE1.5-KI,
  gleiche Bildabstaende ab Detonation). Gleich: rote Wolke ueber den Koerpern, Stuecke steigen bis zum oberen Bildrand,
  schwarzer Rauch ab +0,43 s, am Ort nichts mehr. Anders (wie im Dossier benannt): Feuerball orange, keine Wandflecken.

## 3. Kanal ROOM2000 — "Explosions- und Rauch-Sprites werden nicht gezeichnet" (Pruefauftrag 2)

**Ergebnis: NICHT reproduziert als Zeichenfehler; weder neu noch alt — die Behauptung im Dossier (N1.5, N1.8 Punkt 3) ist falsch.**

1. ECHTER Wurf in ROOM2000 (Spawn, Cut 0; zuendet an der Wand, `EV wand ... explosion sofort`): Feuerball weiss -> orange, dann
   roter/schwarzer Rauch ~0,6 s — **identisch** auf 8492d3594, 2f886e91a und der master-exe
   (`a1_kanal2000_echterwurf_neu_vorher_master.jpg`, je 9 Bilder ab Detonation). Kinder 0x03195000 / 0x030B5400 / 0x030B5800
   in allen drei Laeufen mit Platz (`gr.log` n=1, Plaetze 74/68/69/60).
2. Mess-Haken wie beim Autor (dessen Lauf `r2000_fg5`: Leon (-22500,-13000) Band 0, Cut 6, `RE15_FORCE_GRANATE=100:4`,
   P=(-22184,-500,-9711), zwei 0x16 ZERFETZT): kein Feuerball im Bild — **ebenso mit `RE15_KEIN_80PROZENT_GORE=1`** (keine
   Zerfetzung, keine Blutflut) -> nicht vom Gore der Spur verursacht (`a1_kanal2000_cut6_haken_mit80_oben_ohne80_unten.jpg`).
3. Ursache (benannt, `RE15_FX_LOG`, `a1_kanal2000_fxlog_projektion.txt`): das Explosionskind `id=3 sub=25` besteht den
   Regionstest und laeuft in jedem Bild durch den Zeichner, seine Mitte projiziert aber auf **sx=2 sy=341 bei einem
   320x240-Bild** — AUSSERHALB des Bildes unten links; der erzwungene Cut 6 sieht den liegenden Zombie von ganz nah, P (y-500)
   liegt unter der Bildkante (das Blut derselben Zerfetzung projiziert auf sy=286, nur seine Oberkante ragt ins Bild). Mit Leon
   auf Band 1 (P.y = -2300, eigener Lauf b2) erscheint derselbe Feuerball unten links am Bildrand.
   Nebenbei: der ESP-Pool steht in ROOM2000 schon ohne Gore auf 96 (Raumeffekt id=11, ~63 gezeichnete Plaetze je Bild) — der
   Vorrang der Explosionskinder (esp_granate_kind) ist dort tatsaechlich noetig.
4. Folge fuer das Dossier: N1.5/N1.8.3 und die Bildunterschrift von `n1/n1_vergleich_video_cd2_gegen_port_2000_kanal_0x16.jpg`
   ("Explosions-Rauch-Sprites in ROOM2000 unsichtbar (offen)") sind zu berichtigen; der Kanal-Vergleich zeigt die Explosion nur
   deshalb nicht, weil ihr Ort ausserhalb des erzwungenen Bildausschnitts liegt (Mangel 1).

## 4. Nebenwirkungen (Pruefauftrag 3)

* Code: jede GR-Aenderung ist an Zeile 9 (`sub_state_1 == 9`; nur w9/w15 landen dort ueber `re2z_row_from_weapon`), an w9/w15
  (RE1.5-Gehirn), an `re2z_part_gr` (Boden-/Wandtest, Liegenbleiben) bzw. an Art 2 (Schaden) gebunden; Lache 2x2 und
  Rauch-Nachlauf nur in den GR-Funktionen; der Vorrang `esp_granate_kind` nur an den Kindern der Routine 31 / des Mess-Hakens.
* Fenster-Stichprobe Schrotflinte ROOM1140 RE2-KI (4 Zombies sterben, RE2-Gore-/Kopf-Weg-Pfade): `fx.log` (jeder gezeichnete
  Effekt je Bild mit Weltlage, ohne Dreieckszaehler) und `state.log` **byte-gleich** zwischen 8492d3594 und 2f886e91a
  (2530 Zeilen). Hund ROOM1230 mit Pistole/Schrotflinte: ebenfalls byte-gleich, aber ohne Tod (die Hunde blieben fern) — als
  Tod-Stichprobe nicht verwertbar.

## 5. Nebenbefunde (nicht von GR verursacht, gemessen)

1. **Dauerflamme am RE2-Hund nach Granatentod:** in d1 brennt an den Resten ab dem Explosionsbild eine Flamme `id=8 sub=3`
   (Raumbank, A=18) bis zum Laufende (358 Bilder). Gegenprobe 2f886e91a mit 4 Zeitversaetzen: **2 von 4 Laeufen** zeigen sie
   ebenfalls (256 bzw. 248 Bilder bis Laufende). Quelle: RE2-Zeile-9-Effekt FX 7 an einem zufaelligen Part (`jal 0x80105070`
   mit a2 = 7 @0x8010474c), Art 0x85 -> Port-Abbildung 5 -> 8 (re2d_fx) = Flamme der Raumbank 1230, die endlos laeuft. Alter
   Befund, eigene Spur.
2. In ROOM2000/1230/2030 zuendet ein echter Wurf an mehreren Standorten sofort an der Wand (`EV wand ... explosion sofort`),
   identisch auf 2f886e91a und master — Runde-35-Wurfregel, nicht GR.
3. Zombies (0x11 in 1140, 0x16 in 1041) bleiben nach der Werfer-Explosion mit **HP 0** im Zustand 1 aktiv und greifen weiter an
   (Schaden unveraendert gegenueber 2f886e91a).

## 6. RE15_FORCE_GRANATE (Pruefauftrag 4)

`re15_pc_force_granate_takt` liest das Env einmal und kehrt ohne Wert sofort zurueck; einziger Aufruf main.c:7979 hinter
`re15_pc_fx_takt_frei()`. Gemessen: in allen Laeufen ohne Env keine `[harness] RE15_FORCE_GRANATE`-Zeile; Schrot-Lauf neu/alt
byte-gleich (Abschnitt 4). Kein Leck ins Spiel.

## 7. Adressen und PORT-WAHL (Pruefauftrag 5), selbst gelesen

* RE1.5 (info/Re1.5/PSX.EXE UND Version_40%/PSX.EXE, byte-gleich): Schadenstabelle @0x8006f418 `10,20,1000,...` (Art 2 = 1000,
  `e8 03` @0x8006f41c); `ori a0,zero,0x1f4` @0x80018598, `addiu v0,v0,-500` @0x800185a8, `ori a2,zero,0x2` @0x800185b4,
  `jal 0x80012d60` @0x800185b8; Bodensonde `addu a1,zero,zero` @0x800187dc, `ori a2,zero,0x8` @0x800187e8, `ori a3,zero,0x100`
  @0x800187f4, `jal 0x8001c6e8` @0x800187f8; Rauchkind `lui a0,0x30b` @0x80018698 / `ori a0,a0,0x5800` @0x800186a8; Pool
  `sltiu v0,t3,0x60` @0x8001978c; Kinder `jal 0x800199d4` @0x80018640/@0x80018660, `lui/ori 0x30b5400` @0x80018648-4c,
  `jal` @0x800186c8. Spinne STAGE2.BIN @0x80118f38: [9] = 0x80113ff8, [15] = 0x8011461c; `ori v0,zero,0x7`/`sb v0,5(v1)`
  @0x80114b1c-20. STAGE1.BIN DEATH[9] @0x8011feac = 0; `sw zero,1720(v0)` @0x801081d0.
* RE2 (ghidra_re2_Leon.txt, FUN_80028AD8): `slti v0,v0,0x2` @0x80028b60; `jal FUN_8004fba0` mit a1 = 0x64, a2 = 0x2000, a3 = 1
  @0x80028b6c-78; `li v1,-0x2` @0x80028c9c + `and v0,v0,v1` @0x80028ca0 (das Port-Zitat "`and v0,v0,-2`" fasst zwei Befehle
  zusammen); Wandtest `lui 0x91a2`/`ori 0xb3c5`/`mult` (= /1800) @0x80028ca8-cb4, `jal FUN_8004c1bc` @0x80028cdc, Abprall
  `sra ..,0x12` + `subu zero` + `sh` @0x80028cec-d20. EMZ0.BIN: `ori v0,v0,0x1062` @0x80108e5c und @0x8010537c,
  `addiu v0,zero,30` @0x80108e70 / `sb` @0x80108e7c, 50 @0x80105398, Brad `addiu v1,zero,250` @0x801008c8. EMD0G_MOD0.BIN:
  Parts `02 03 04 07 08 09 0a` @0x80105680, `ori v0,v0,0x4a` @0x801044a4, `lui a2,0x10`/`ori a2,a2,0x1040` @0x80104454-5c,
  `sw a2,112(v1)` @0x801044c8. **Alle Stichproben stimmen.**
* PORT-WAHL-Kennzeichnung: GR_G/GR_DECKE/GR_STEIG_MIN/GR_VH0 und GR_RAUCH_START/TAKT/ZAHL tragen PORT-WAHL + Videozeit
  (CD2 #577/#586-#600/#590/#632). Ohne Beleg/Begruendung: GR_RAUCH_MAX 8, Nachlauf-Abstand 1000 (Port-Parameter); die
  Zufallsmasken `rand & 0x3f` / `rand*2` / `(rand & 0xff) - 128` sind im Block nur als "Zufall" benannt — geringfuegig (Mangel 3).

## 8. Maengel

1. **Falscher Kanal-Befund im Dossier** (N1.5, N1.8 Punkt 3, Bildunterschrift `n1_vergleich_video_cd2_gegen_port_2000_kanal_0x16.jpg`):
   "Explosions-/Rauch-Sprites in ROOM2000 nicht gezeichnet, auch beim echten Wurf, vermutlich Textur/Bank". Gemessen: gezeichnet
   (echter Wurf, drei exe-Staende gleich); im Haken-Lauf liegt die Explosionsmitte ausserhalb des Bildes (sx=2 sy=341 / 320x240).
   Beleg Abschnitt 3. Schwere: gering (Dokumentation; meldet dem Nutzer einen nicht vorhandenen Fehler). Berichtigen.
2. **Spinne unter der Vorgabe-KI (RE2) wird von EINER Handgranate nicht zerfetzt** (HP 110 -> 50, erst die 2. Granate toetet
   und laesst sie platzen; Lauf s3). Nutzerauftrag 2: "andere Gegner die zerfetzen koennen sollen ... zerfetzen von einer
   Granate". Der Autor fuehrt das als "zur Entscheidung" (N1.3). Dieselbe Begruendung, mit der er fuer die Zombie-Familie die
   eigene Zahl des Gegenstands (1000 @0x8006f41c) statt der RE2-Werferzeile nimmt ("RE2 hat keine Handgranate, die Zeile 9 war
   eine Zuordnung"), gilt woertlich auch fuer die Spinne (RE2 60 = Werferzeile). Schwere: mittel (Abweichung vom Nutzerwort
   unter der Vorgabe-KI; die Entscheidung gehoert begruendet getroffen oder ausdruecklich dem Nutzer vorgelegt). Gleiches gilt
   fuer die Werfer-Explosivrunde an 0x11/0x16 (gemessen e4/e1: Ueberlebende mit 50/0 HP).
3. **Kleinigkeiten:** Schalter `RE15_KEIN_80PROZENT_GORE=1` laesst neben dem Schaden auch den Vorrang `esp_granate_kind` an (im
   Dossier ist nur der Schaden genannt); GR_RAUCH_MAX/Abstand 1000 ohne Begruendung; RE1.5-Hund `case 3` ruft
   `re15_re2d_granate_teile` jedes Bild erneut, solange `re2z_part_gr` 0 bleibt (kein Part "frei") — theoretischer Randfall
   (Rauch-Nachlauf alle 53 Takte neu), nicht beobachtet. Schwere: gering.

## 9. Bildleisten (GR_belege/abnahme1/)

`a1_vergleich_video_cd2_oben_port1140_unten.jpg`, `a1_leon1140_re2ki_echterwurf_ablauf.jpg`, `a1_leon1140_liegen_plus5s.jpg`,
`a1_leon1140_werfer_explosivrunde.jpg`, `a1_hund1230_re2ki_echterwurf_neu_vorher.jpg`, `a1_hund1230_re15ki_messhaken.jpg`,
`a1_spinne2030_re15ki_werfer.jpg`, `a1_spinne2030_re2ki_zweite_handgranate.jpg`, `a1_kanal2000_echterwurf_neu_vorher_master.jpg`,
`a1_kanal2000_cut6_haken_mit80_oben_ohne80_unten.jpg`, `a1_kanal2000_fxlog_projektion.txt`.
