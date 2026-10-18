# Runde 39 Spur G "karten" - Abnahme 0 (unabhaengig, 2026-10-18)

Baum: .claude/worktrees/r39_karten (Zweig r39/karten, Kopf 40328436, Merge-Basis 22abaed9). Der Pruefer glaubt nur, was er
selbst am gebauten Stand gemessen hat. Fortlaufend geschrieben (Sitzungslimit). Arbeitsordner der Messungen (nicht im Baum):
`<scratchpad>/G39abn0/` (Skripte reg.py, phase.py, vgl2.py, gitter.py, struktur.py, tueren.py, lauf.sh).

## 0. Stand / Bau / gezielte Tests
- `git log master..HEAD`: 16 Commits (997a2cec .. 40328436), Arbeitsbaum sauber. Code-Diff (`git diff master...HEAD --
  re15_port`): engine/src/karte_r39.c (neu), include/re15_karte_r39.h (neu), engine/src/re15_map_zones.h,
  engine/src/re15_inv_screen.c (Haken), engine/src/map_hint_common.c (Haken); Tests (neu: test_r39_karten.c,
  r39_karte_b1.h, r39_karten_soll.inc, probes/r39_karten.cmake, test_r39_karten.cmake; angepasst: test_map_synth.c,
  test_r35_cut10f0.c, test_r35_karte.c/.cmake, test_r36_karte.c/.cmake, test_r36_kartenitems.c, test_r38_tagebuch.c).
- Bau: `bash re15_port/tools/local_build.sh configure && ... build` -> `=== LOCAL-BUILD-OK (build)` (ninja: no work to do,
  exe 18:40 nach dem letzten Code-Stand; 40328436 aenderte nur Doku).
- VORHER-exe zum Gegenmessen: Quellen der Merge-Basis 22abaed9 (`git archive`, ohne shared_assets) im Arbeitsordner,
  `cmake --build --target re15_pc` (PowerShell), Lauf mit `RE15_ASSET_ROOT=<baum>/re15_port/shared_assets/PSX`.
  `diff -rq` gegen den Baum: nur die 5 Spur-Dateien verschieden.
- Gezielter Lauf (93 Tests = alle Karten-/Hinweis-/Inventar-Gruppen, die der Diff beruehrt):
  `ctest --test-dir re15_port/build -R "^(unit_map_|unit_karte_|unit_r30_karte|unit_r30_hinweis|unit_r33_karte|unit_r35_karte|
  unit_r35_cut10f0|integration_r35_cut10f0|unit_r36_karte|unit_r36_kartenbits|unit_r36_kartenitems|integration_r36_karte|
  integration_r36_kartenitems_bild|unit_r38_tagebuch|integration_r38_tagebuch|unit_r39_karten|integration_r39_karten|
  integration_r39_elza_karte|unit_r39_inventar|integration_r39_inventar|integration_map_uebergang|integration_map_raum_live|
  integration_r35_karte|unit_inv_|unit_r26_inventar|unit_r35_inventar1050|integration_r35_inventar1050|unit_r36_inventar|
  integration_r36_inventar|unit_r38_kleinkram_inventar|integration_r38_kleinkram_inventar|unit_pri_inventar_riegel|
  unit_r30_android_n1_inventar_cmd)" -j 4 --timeout 900` -> **100% tests passed, 0 tests failed out of 93** (570 s).

## 1. RE-Gate (Stichproben selbst disassembliert / gelesen)
| Behauptung | Werkzeug | Befund |
|---|---|---|
| ROOM1090-Zeile Index 9 @0x800768F8 = Stub `00 00 00 00 01 00 01 00` | re15_disasm.py bytes + eigene Tabellen-Dekodierung PSX.EXE (t_addr 0x80010000) | stimmt; ebenso 1180 @0x80076970, 11D0 @0x80076998, 1230 @0x800769C8 Stub, 11C0 @0x80076990 = (174,196,1,1), 1190 @0x80076978 = (75,127,1920,2301), 11B0 @0x80076988 = (159,110,2272,1954), 1030 @0x800768C8 = (108,194,2428,2229); 10C0/10E0/1120 = Stub -> neue Zeilen sind PORT-ERGAENZUNG |
| Formel FUN_800473f8 | `re15_disasm.py dis 0x800473f8 80` | `addiu v0,v0,32000` @0x80047428, x10 per sll/addu/sll @0x8004742c-48, `lhu` sx 0x800768b4+8i @0x80047444, `sra t0,t0,20` @0x8004745c, `addiu t0,t0,5` @0x80047460, /10 per `lui/ori 0x66666667` @0x80047450/54, `lh` ox @0x800474e8; z: `lhu` sy @0x8004747c, `subu v1,zero,v1` @0x800474b0, `lhu` oy @0x80047518 - stimmt |
| RE2 FUN_8006e120 Massstab 1/450 | `re2_disasm.py dis 0x8006e120 120` (info/re2leon/PSX.EXE) | `lui v0,0x91a2` @0x8006e1dc, `ori v0,v0,0xb3c5` @0x8006e1e8, `addiu a0,a0,28000` @0x8006e1ec, mfhi+a0, `sra v1,v1,8` @0x8006e20c, z `subu v0,zero,v0` @0x8006e268 -> (x+28000)*0x91A2B3C5/2^40 = /450, EIN Massstab + Kachelversatz - stimmt; Port nimmt 146/65536 = 1/448,9 (gerundet, PORT-WAHL, 0,24 %) |
| Seiten-Tabellen @0x80076840 (+8*Seite) | eigene Dekodierung | Blatt 0 7 Rechtecke @0x800762A0; 1F r0 (180,69,32,96) uv(0,32) @0x8007636C, r1 @0x80076378, r2 @0x80076384, r3 @0x80076390, r4 @0x8007639C, r7 @0x800763C0, r8 @0x800763CC, r9 @0x800763D8; 3F r4 (144,80,32,80) @0x80076498; B2 r3/r4/r5 @0x80076318/24/30 - alle Zitate in karte_r39.c stimmen. Huelle aller 13 Tabellen x 39..293 / y 44..206 (Blatt 12/6/7) - Begruendung der test_map_synth-Schranke stimmt |
| ROOM1180 main00 Ck(4,243) `21 04 f3 00` @0x009EA, Tuersatz @0x009EE Ziel 0x16 | Byte-Lesen ROOM1180.RDT | stimmt (Satz `3b 00 02 31 ...`, Rechteck (-6041,-17561) 2409x1444, Spawn (-300,0,-17400), Ziel Stage 0 Raum 0x16 = 1160) |
| SCA-Offsets ROOM1090 (Hof/Ebene) | Byte-Lesen (u16 w, u16 d, s16 x, s16 z) | @0x01E84 x -16382 + w 1836 = -14546, @0x01EC0 -6030+1616 = -4414, @0x01DF4 z 7720 - stimmt |

Suche im Diff nach "deferred/tunable/interim/for now/faithful/plausibel/TODO": keine Treffer in den hinzugefuegten Zeilen, kein neuer getenv (s. Abschnitt 8).


## 2. Punkt 1a - Blaetter 1F/2F/3F/B2 gegen die Nutzer-Vorlagen (eigene Messung)
Wortlaut: "Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich aussehen sollte. Keine Doppelwaende,
Tuer immer genau auf Wand platziert, treppen nicht in der Wand platziert usw. Und natuerlich muss der Spielermarker da
entsprechend passen."

Messprotokoll (eigene Werkzeuge, nicht die des Bau-Agenten):
- Abzuege: `lauf.sh <exe> <ordner> 1190 300 1190 3174 0 -11182 0 -- RE15_MAP_SHOT_SWEEP=s` (Spielstand per
  probe_r36_karte karte, exe-Kopie `re15_pc_g39abn0_<id>.exe`, RE15_WINDOW_SCALE=1, alle 13 Blaetter je Lauf) fuer die
  NEU-exe (neu_S) und die VORHER-exe (alt_S); zusaetzlich am Nutzerpunkt ROOM1090 (-12528,-9000,959) (neu_M/alt_M).
- Vorlagen: eigene Registrierung (reg.py) und eigene 3x-Phasenbestimmung ueber die Einfarbigkeit der 3x3-Bloecke
  (phase.py): links (-2,2)/(1,0)/(0,-1)/(1,-1) je 1,0000 einfarbig; rechts 1F (1101,2) 0,917, 2F (1098,0) 0,9999,
  3F (1164,-1) 0,9996, B2 (1046,-1) 1,0000 (1F rechts ist an den Kanten verwischt - Mischfarben "?WW?" -, die Blockmitten
  stimmen). Abtastung je Spielpixel in der Blockmitte, Klassen Grund/Koerper(gruen=rot)/Wand/Tuer/Treppe.
- VORHER-exe gegen LINKE Haelfte: B2 0 / 1F 0 / 3F 0 Klassen-Abweichungen (B2/3F sogar 0 RGB-Abweichungen); 2F am
  Nutzerpunkt 0 Klassen-Abweichungen (13 RGB = Ring-Blinkphase). => die linke Haelfte IST der Stand vor der Spur (bestaetigt Dossier-Abschnitt M1).
- NEU-exe gegen RECHTE Haelfte (Soll):

| Blatt | Kartenpixel verglichen | Klassen-Abweichungen | Rest |
|---|---|---|---|
| B2 (Blatt 1) | 56 207 | **0** | 15 RGB = Koerper-Mischvariante (16,56,40)/(16,56,56) auf anderem Grundtexel |
| 1F (Blatt 2) | 56 234 | **0** | 272 RGB, gleiche Ursache |
| 2F (Blatt 3, Nutzerpunkt 1090) | 56 201 | **0** | 34 RGB: der Marker-Ring (Soll = alter Ring (184..188,81..85) im gruenen Nachbarraum, neu (181..185,70..74) im Rot - gewollt, Punkt 2) + Mischvarianten |
| 3F (Blatt 4) | 56 216 | **6** | senkrechte Treppe (168,107) s.u. |

- Vorher/Nachher-Differenz der ganzen Sweeps: Blatt 0: 12 612 px, 1: 93, 2: 322, 3: 53, 4: 98, **Blaetter 5..12: je 0**
  (keine Nebenwirkung auf die Karten der Stages 2-6).
- Strukturpruefung (struktur.py, eigene Regeln, Kartenfeld x40..300 y40..210):

| Blatt | Doppelwand-Pixel vorher -> nachher | Tueren NICHT auf Wand vorher -> nachher | Treppenpixel mit Wand-Nachbar vorher -> nachher |
|---|---|---|---|
| B2 | 40 -> **0** | 2 -> **0** | 10 (in der Wand x186/187) -> 6 (Sprossenenden an x170/x186, wie Soll) |
| 1F | 223 -> **0** | 8 -> **0** | 12 (x197/x136 in der Wand) -> 12 (Enden an x119/x136/x180/x197, wie Soll) |
| 2F | 0 -> **0** | 1 -> **0** | 6 (x135 in der Wand) -> 6 (Enden an x118/x135, wie Soll) |
| 3F | 5 -> **0** | 2 -> **0** | 1 -> 3 (s.u.) |

  "Tuer nicht auf Wand" = Balken, dessen Enden nicht in Wand weiterlaufen oder der quer neben einer Wandlinie liegt.
- Die Treppen-Korrekturen 1F/2F/B2 (gitter.py, Klassengitter vorher | neu | Vorlage 3x) treffen die Vorlage spaltengenau
  (2F links x119..123/rechts x130..134 mit Wand x135; B2 x171..175 / x181..185 mit Wand x186; 1F x120..124 / x131..135 mit
  Wand x136).
- **3F-Treppe (168,107) kind 5 (re15_map_zones.h `{4,3,168,107,5,24,255,0}`)**: die Vorlage verschiebt sie um EIN
  Vorlagenpixel = 1/3 Spielpixel (Sprossen y 104,33..109,33; Abstand zur oberen Wand y103 1/3 px, zur unteren Linie y110
  2/3 px). Blockmitten-Abtastung ergibt y104..108 (= alter Stand); der Bau setzt y105..109. Folge im neuen Abzug:
  ALLE DREI Sprossen stehen jetzt unmittelbar auf der Linie y110 (x166 auf dem Tuerbalken, x168/x170 auf der Wand),
  vorher beruehrte nur die rechte Sprosse (170,104) die Wandecke (170,103). Der Bau-Riegel meldet "0 Abweichungen", weil
  sein Soll-Bild diese Teilpixel-Verschiebung anders rundet. Bei 1x passen 5 Sprossenzeilen in die 6 freien Zeilen
  y104..109 nicht mit Abstand zu BEIDEN Linien. -> Mangel M1 (gering, Entscheidung noetig).

## 3. Punkt 2 - ROOM1090 Marker im aktuellen Raum (eigene Messung)
Wortlaut: "In ROOM 1090 ist Leons Positionsmarker der Map ausserhalb des aktuellen Raums."
- VORHER-exe am Nutzerpunkt (R1090 pos=(-12528,-9000,959) aus nutzer/befund_marken_2026-10-17.txt F1126ff, Spielstand
  `probe_r36_karte karte ... 1090 -12528 -9000 959 0 besucht:1090:-12528:959:5`, Sweep): Ring (184..188,81..85),
  Mitte (186,83), ringsum GRUEN (Nachbarraum), das rote Gamma beginnt erst rechts der Wand x197 -> Befund reproduziert.
- NEU-exe, gleicher Stand: Ring (181..185,70..74), Mitte (183,72), ringsum ROT (Querband des Gammas x179..204 y67..76).
- ECHTER WEG (kein Sprung in 1090): `RE15_DEBUG_JUMP=10f0@250 RE15_SET_FLAG=9:71 RE15_PLAYER_POS=-7125,4950,0
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W10,<Schiebe-/Kletterweg aus test_r36_klettern.cmake>` - Tritt schieben,
  hochklettern, Tuer von oben -> ROOM1090 Band 5, Ankunft pos=(-13600,-9000,1300) (RE15_CLIMB_TRACE T1268), dann
  `RE15_INV_OPEN_AT=40#1090 RE15_INV_FB_SHOT=karte.bmp`: Blatt 2F, Ring Mitte (182,72), Rand links x180 = innen neben der
  Westwand x179, ringsum ROT.
- ELZA (probe_r37_feuerloescher_karte ... 1091 -12528 959 3072 y:-9000 cut:0 elza; debug.log `[pl] Spieler-Familie PL04`,
  `CONTINUE: resumed in room 1091`): Ring Mitte (183,72), ringsum ROT.
- Hof (Band 1, Leon an der 1050-Tuer (-10100,-1800,4200)): Blatt 1F, Ring Mitte (189,66), ringsum ROT.
- Flaeche (Sonde, echter Kollisionsloeser + echter Kartenpfad re15_inv_map_marker): ROOM1090/1091 Band 5 0 von 1225 (jeder
  3. Punkt) bzw. **0 von 11 059 (jeder Punkt)** ausserhalb; Band 1 0 von 590. Band 6 (Tuer -> 1100, Boden 6) hat nur 3
  Zellen und ist kein eigener Bereich (Flut leckt); an der Tuer (-5820,-10800,-16000..-18690) klemmt der Marker auf
  (201,111) im Gamma (Index 1).
- RE: Original-Zeile @0x800768F8 = Stub (selbst gelesen); Formel FUN_800473f8 selbst disassembliert (Abschnitt 1); Zeilen
  ox/oy/sx/sy = PORT-ERGAENZUNG, aus SCA-Waenden gefittet - Stichprobe der Offsets selbst gelesen (stimmt).
- Riegel: unit_r39_karten_1090 (gruen), integration_r39_karten Lauf M (gruen).
**Urteil Punkt 2: erfuellt.**

## 4. Punkt 1c - B1 neu (eigene Messung)
Wortlaut: "Der Room B1 den habe ich nicht gemacht. Da muss du generell noch einmal schauen, da passt nichts so richtig
zusammen mit dem wie man sich bewegt im Vergleich zu der Karte. Die Karte von B1 ist noch ziemlicher schrott."
Ansatz des Baus (Schema-Zeichnung aus Kollision + Tuerpaaren, RE2-Massstab 1/450) entspricht der frueheren Nutzer-Vorgabe
("Kollisionsboxen pro Raum, alle Tueren ... der naechste Raum schliesst EXAKT an die Wand an", Memory
reai-v2-zu-kompliziert, Zitat geprueft).
- WIE MAN SICH BEWEGT, echte exe (Spielstand an der Stelle, Kartenschirm, Ring-Mitte per eigener Ringsuche ring.py,
  blinkphasenfest): Gang ROOM1180

| Strecke | VORHER-exe Ring | NEU-exe Ring | px je 1000 Einheiten vorher -> neu |
|---|---|---|---|
| Laengsgang x3858: z -3800 / 13000 / 30000 | (162,102) / (162,73) / (162,63) | (153,144) / (153,107) / (153,69) | 1,73 und 0,59 -> 2,20 und 2,24 |
| Versatz z-5777: x -5100 / 5000 | (128,116) / (164,108) (8 px Querdrift) | (133,148) / (156,148) | 3,56 -> 2,28, Querdrift 8 -> 0 |
| Suedbein x-4142: z -15900 / -7800 | (133,122) / (133,117) | (135,171) / (135,153) | 0,62 -> 2,22 |

  Die Sonde (probe_r39_karten marker) liefert an denselben Punkten Marker = Ring-Mitte + 1 (Ringsprite), also derselbe
  Pfad wie die exe; Vorher-Zahlen des Dossiers (0,62..3,56) bestaetigt.
- TUERDURCHGANG echte exe: Gang an der 11B0-Tuer (5450,28874) -> 11B0-Ankunft (-26483,-18094): vorher (165,63) ->
  (171,83) = 26 px Sprung, neu (156,71) -> (157,71) = 1 px. Eigene Nachrechnung aller 12 B1-Tuerrichtungen (Trigger-Mitte
  in A mit der Affine von A gegen Spawn in B mit der Affine von B, b1_affin.py aus der Tabelle): 1..6 px (11C0->11B0 6 px =
  Spawn 4 px im Raum).
- MARKER IN DER ZEICHNUNG: eigene Projektion ALLER begehbaren Rasterpunkte (Flut mit dem echten Loeser, probe_r39_karten
  flut ... 450 100 <pgm>, ungeklemmt) gegen die Zellen: Gang 15 695, 1190 Halle 33 923, Ostkammer 6 400, 11B0 3 738 +
  14 400, 11C0 50 916, 11D0 5 254, 1160 5 093, Kammer 389, 10A0 Band 4 1 220 Punkte - **0 ausserhalb**.
- ROOM1160 (Zwinger ohne Strom) echte exe: vorher KEIN Marker (Ringsuche leer), neu Ring (134,173) an der Gang-Tuer,
  (-8000,-12500) -> (117,163); Kammer (3900,-22000) -> (143,185) in der Kammer-Zelle.
- Struktur B1 (ohne Marker im Bild, Sweep aus 1090): Doppelwand-Pixel vorher 52 -> 0, Tuerbalken 8 -> 6 (die zwei
  1260-"Tueren" waren Bodenloecher - ROOM11B0 slot 5 @0x01010 / 11D0 slot 1 @0x01228 selbst gelesen: Ziel 0x26), alle 6 auf
  der gemeinsamen Linie.
- Kartenhinweis ROOM11C0 nach der 10F0-Szene (echter Lauf wie integration_r35_cut10f0 A, RE15_INV_FB_SHOT im
  Hinweis-Schirm): Blatt B1 zeigt die 11C0-Zeichnung rot, Log `[hint] ... rot` / `umriss` wechseln.
- Nebenbefund B1: die 10A0-Treppe 4<->3 (145,165) kind 4 liegt mit ihrer untersten Sprosse (143..147,167) direkt auf der
  Unterkante y168 der Absatz-Zeichnung (5 Pixel parallel anliegend) -> Mangel M2 (gering).

## 5. Punkt 1b "Spielermarker passt" auf 1F/2F/3F/B2 - Restfaelle (eigene Sondenlaeufe probe_r39_karten markerflaeche)
Gemessen am Stand des Baums (jeder 3. begehbare Rasterpunkt, echter Loeser, echter Kartenpfad):

| Raum | Marker neben der Zeichnung | Status laut Bau |
|---|---|---|
| ROOM1210 (B2), Saat (-25000,-2200) | **207 von 1141 (18 %)** - begehbare SW-Kammer, weder im Original noch in mapB2_correct.png gezeichnet | OFFEN, Entscheidungsfrage |
| ROOM1170 Zone 1 (3F), Saat (-21225,-18870) | **51 von 311 (16 %)** | OFFEN (Orientierung) |
| ROOM1030 (1F) | 63 von 2288 | OFFEN (ausgelieferte Zeile @0x800768C8) |
| ROOM1110 (2F) | 8 von 1237 | OFFEN |
| ROOM1060 (1F) | 4 von 155 | OFFEN |
| ROOM10A0 Band 1 (B2) | 8 von 140 | OFFEN |
| ROOM10C0 / 1120 / 10E0 / 1090 | 0 / 0 / 0 / 0 (vorher laut Dossier 123 / 77 / 26 / 160) | umgesetzt |

Die offenen Faelle bestanden schon vorher (Zeilen/Kacheln unveraendert) - keine Verschlechterung, aber der Wortlaut "natuerlich
muss der Spielermarker da entsprechend passen" ist fuer B2 (1210) und 3F (1170) nicht erfuellt. Der Bau hat das offen
benannt; die 1210-Kammer ist ein echter Zielkonflikt zur Vorlage (Nutzerentscheidung noetig).

## 6. Nebenwirkungen / Teilzustaende (eigene Messung) - REGRESSION gefunden
Die Vorlagen zeigen den Zustand "alle Raeume sichtbar". Die Korrekturen in karte_r39.c (19 Texel-Rechtecke) und die 5
Schema-Kanten von ROOM1000 loeschen je EINE der zwei Wandlinien UNBEDINGT - die verbleibende Linie gehoert dem
NACHBARRAUM. Ist der Nachbar nicht gezeichnet (nicht besucht und Etagenplan nicht im Besitz), fehlt die Wand ganz.
Messung (Spielstand per probe_r36_karte mit genau den genannten besucht:-Orten, CONTINUE, Kartenschirm FB_SHOT,
VORHER-exe gegen NEU-exe, Klassenvergleich "vorher Wand, jetzt weder Wand noch Tuer"):
- ROOM1000, besucht Ostraum + Nord-WC (aktuell Ostraum): VORHER beide Kaesten mit vier Waenden; NEU Ostraum ohne Westwand
  x207 und ohne Oberkante y89, Nord-WC ohne Westwand x207 und ohne Unterkante y138 (Bild r1000_vgl.png im Arbeitsordner).
  Die Oberkante haengt an ROOM1090 (r5 Suedwand y88), die Nord-WC-Unterkante am Sued-WC (y139) - beide im normalen Spiel
  oft noch nicht gezeichnet (ROOM1000 erreicht man nur aus 1050; die beiden WCs sind auf Nutzerwunsch getrennte Orte).
- ROOM1020 allein: 16 Wandpixel verloren (x144 y109..124, haengt an ROOM1070); echter Ablauf 1030 -> 1040 -> 1020 ohne
  1070: 1020 rot mit offener Ostseite (Bild w1020_1010.png oben rechts).
- ROOM1040 allein: 17 Wandpixel verloren (x118 y126..154, haengt an 1030/1060).
- ROOM1050 allein: 8 Wandpixel verloren (x187 y145..154, haengt an 1030).
- ROOM1010 (nach 1030/1040/1020, ohne 1050): Ostwand x179 fehlt (haengt an 1050/1090/10A0) (w1020_1010.png unten rechts).
- Alle korrigierten Orte je ALLEIN besucht (allein.sh: Spielstand nur dieser Ort, aktuell; allein_ausw.py: Pixel, die
  vorher Wand waren und jetzt weder Wand noch Tuer sind, ohne Ersatzwand daneben):

| Ort (Blatt) | verlorene Wandpixel | wo | Wand haengt jetzt an |
|---|---|---|---|
| 1050 (1F r0) | 8 | x187 y145..154 | 1030 |
| 1020 (1F r1) | 16 | x144 y109..124 | 1070 |
| 1040 (1F r2) | 17 | x118 y126..154 | 1030 / 1060 |
| 1070 (1F r3) | **34** (ganze Westwand) | x196 y110..143 | 1050 |
| 1010 Zone 0 / Zone 1 (1F r8/r7) | **30 / 30** (beide Seitenwaende) | x164 + x179 | 1020 bzw. 1050/1090/10A0 |
| 1080 (1F r9) | 8 | x118 y135..142 | 1060 |
| 1000 Ostraum / Nord-WC / Sued-WC (Schema) | **41 / 26 / 12** | x207, y89, y138 | 1050, 1090, Sued-WC |
| 1220 Zone 0 / Zone 2 (B2 r4/r5) | 16 / 16 | x187 | 11E0 |
| 1030, 1110, 1210 | 0 | - | - |

  Ausschnitt ROOM1000 Nord-WC allein (aktuell, links vorher | rechts neu; W Wand, R aktuell, o Ring):
  ```
  y123 ....WWWWWWWWWWWWWWW...|  y123 ....WWWWWWWWWWWWWWW...
  y124 ....WRRRRRRRRRRRRRW...|  y124 ....RRRRRRRRRRRRRRW...
  ...
  y137 ....WRRRRRRRRRRRRRW...|  y137 ....RRRRRRRRRRRRRRW...
  y138 ....WWWWWWWWWWWWWWW...|  y138 ....RRRRRRRRRRRRRRW...
  ```
  Gegenprobe mit Etagenplan (flag:9:89, ROOM1020 allein): die Linie x145 des Umrisses von 1070 ist da, 1020 geschlossen.
  Ohne Plan: x144 Koerper, x145 leer -> offen. Realistischer Weg ohne 1F-Plan (liegt in ROOM1030): von 2F ueber das
  Treppenhaus 1060 nach 1040 -> 1020 -> 1010, bevor man die Halle 1030 betritt.
Mit dem Etagenplan im Besitz zeichnet der Port unbesuchte Nachbarn als Umriss (CLUT 498) - dann ist die Linie da; ohne
Plan (1F-Plan liegt in ROOM1030, B2 in 1200, 2F in 10D0) bleiben die Raeume offen. Vorher gab es diese Luecken nicht.
Kein Riegel der Spur prueft einen Teilzustand (alle drei Laeufe von integration_r39_karten und die Sonden arbeiten mit dem
Sweep = alles aufgedeckt). -> Mangel M3.

## 7. Punkt 1b - Spielermarker gegen die verschobenen Treppensymbole (eigene Sondenlaeufe)
Die Vorlagen verschieben die Treppen der Treppenhaeuser 1060 (1F/2F) und 10A0 (1F/B2). Die Abbildung dieser Raeume ist
unveraendert (Bbox-Streckung bzw. Zeile wie vorher). Projektion der STAIR-AOT-Mitten (ROOM1060 Aot_set @0x00D72ff
(25150,22000)/(21400,22850); ROOM10A0 @0x00E40ff (21450,21650)/(25350,22350)/(21450,20650)) = dort steht der Marker, wenn
Leon auf der Treppe ist:

| Blatt | Projektion (Marker auf der Treppe) | Treppensymbol vorher | Treppensymbol neu | Abstand neu |
|---|---|---|---|---|
| 1F 1060 | (127,144) / (135,146) | (127,144) / (135,146) | (122,144) / (133,146) | 5 / 2 px |
| 2F 1060 | (126,144) / (134,146) | (126,144) / (134,146) | (121,144) / (132,146) | 5 / 2 px |
| 1F 10A0 | (187,100)+(187,102) / (196,99) | (187,100) / (196,99) | (183,100) / (194,99) | 4 / 2 px |
| B2 10A0 | (177,113)+(177,115) / (186,112) = Wandpixel | (177,115) / (186,112) | (173,115) / (183,112) | 4 / 3 px |
| B1 10A0 (neu) | (145,163)+(145,165) / (154,162) | - | (145,165) / (154,162) | 0 / 0 |

Vorher sassen die Symbole genau auf der Projektion (nur eben in der Wand); jetzt steht der Marker auf der Treppe 2-5 px
neben dem Symbol, auf B2 auf dem Wandpixel. Fuer 10C0/1120/10E0 hat der Bau die Zeile aus den Waenden neu bestimmt, fuer
1060/10A0 ausdruecklich nicht ("nicht angefasst"). Der Wortlaut "Und natuerlich muss der Spielermarker da entsprechend
passen" ist hier nicht erfuellt. -> Mangel M4. (Tuermarken: die fuenf verschobenen Tueren liegen vorher wie nachher
1..8 px von ihren Trigger-Projektionen, Aenderung je +-2 px - kein eigener Mangel.)

## 8. Gates
- RE-Gate: Stichproben (Abschnitt 1) stimmen; jede neue Zeile/Marke/Korrektur traegt Adresse, Datei-Offset oder die
  Kennzeichnung NUTZER-VORGABE/PORT-ERGAENZUNG/PORT-WAHL (146 = gerundetes 65536/450). Kein "deferred/tunable/interim/for
  now/faithful/plausibel/TODO", kein neuer getenv im Diff. **haelt.**
- Erklaert der Fix den Befund? Punkt 2 ja (Huellen-Streckung beider Ebenen, vorher/nachher am Nutzerpunkt reproduziert).
  B1 ja (5 gestreckte Gang-Abschnitte, vorher/nachher in der echten exe gemessen). 1F/2F/3F/B2: das Soll-Bild ja, die
  Teilzustaende nein (M3), der Marker an den verschobenen Treppen nein (M4).
- Vertrag: keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots/Ereignisse belegt; Pfade nur engine/src, include,
  tests (keine release/, platform/android/, shared_assets/, keine Test-CMakeLists; neue Sonde probes/r39_karten.cmake
  nach Muster). Gemeinsame Dateien: re15_inv_screen.c (38 Zeilen, Karten-Code = eigene Spur laut VERTRAG 1.3),
  map_hint_common.c (3 Haken). **haelt.**
- Linux-/Parallel-Festigkeit der neuen Tests: cmake_minimum_required, exe-Endung per LAST_EXT, `m` gelinkt, eigener
  Laufordner + exe-Kopie `re15_pc_r39g_karten_<id>`. **haelt.**
- Riegel je Punkt: vorhanden und messend - selbst gegengeprueft: `probe_r39_karten sollbild` auf die VORHER-Abzuege
  93/322/53/98 Abweichungen, `doppelwand` 47/30/188/0/0 -> rot; auf NEU 0. ABER: kein Riegel fuer Teilzustaende (M3),
  keiner fuer Treppensymbol gegen Treppen-AOT auf 1F/2F/B2 (M4); der B1-Treppenriegel laesst Wandanlage zu (M2).
- Angepasste Alt-Riegel (test_map_synth 90..240/50..200 -> 39..293/44..206 = Huelle der Original-Tabellen, selbst
  nachgerechnet; test_r35_karte 1210-Tuer 3 -> 4 px wegen der Vorlagen-Tuer x186; r36/r35-B1-Riegel auf die
  Schema-Zeichnung umgestellt): Absicht jeweils erhalten, begruendet. Gezielter Lauf 93/93 gruen.
- Nebenwirkung "Rot nur fuer die Zone, in der man steht" (re15_inv_screen.c, Schema-Durchgang): betrifft nur Schema-Zonen
  (ROOM1000 drei Orte, B1). Gemessen: ROOM1000 Ostraum aktuell, Nord-WC besucht -> vorher beide rot, neu nur der Ostraum.
  Deckt sich mit dem Nutzerwortlaut 2026-09-13 ("immer nur der markiert und geladen werden ... in den man auch drin
  ist", test_map_zone_bits.c:5) - Hinweis an den Nutzer, kein Mangel.
- Andere Stages: Sweep aller 13 Blaetter vorher/nachher, Blaetter 5..12 pixelgleich.

## 9. Urteile
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1a Blaetter 1F/2F/3F/B2 nach den Vorlagen (Doppelwaende, Tueren auf Wand, Treppen nicht in der Wand) | **teilweise** | Vollzustand: Klassenbild 0/0/0 Abweichungen (B2/1F/2F), 3F 6 (Treppe, M1); Doppelwand 0, Tueren auf Wand 5/5 Blaetter. ABER Wandluecken in Teilzustaenden (M3, Regression). |
| 1b "natuerlich muss der Spielermarker da entsprechend passen" (Polizeistation) | **teilweise** | 1090/10C0/1120/10E0 jetzt 0 neben der Zeichnung; Treppenhaeuser 1060/10A0: Marker auf der Treppe 2-5 px neben den verschobenen Symbolen, B2 auf dem Wandpixel (M4); 1210 18 %, 1170 Zone 1 16 % u.a. weiter neben der Zeichnung (M5, offen benannt). |
| 1c B1 neu ("passt nichts zusammen mit dem wie man sich bewegt") | **erfuellt** | echte exe: Gang 2,20..2,28 px je 1000 statt 0,59..3,56, Querdrift 0, Tuersprung 11B0 26 -> 1 px; alle 12 Tuerrichtungen 1..6 px; 0 begehbare Punkte neben der Zeichnung; 1160 hat jetzt Marker; 0 Doppelwand. Kleinigkeit M2. |
| 1 gesamt | **teilweise** | 1a/1b nicht voll erfuellt |
| 2 ROOM1090 Leons Marker im aktuellen Raum | **erfuellt** | Nutzerpunkt vorher (186,83) im gruenen Nachbarraum -> neu (183,72) im Rot; echter Weg ueber 10F0 (Klettern + Tuer) Ring (182,72) im Rot; Elza 1091 (183,72) im Rot; Flaeche 0 von 11 059. |

## 10. Maengel (nummeriert, nachpruefbar)
- **M3 (hoch, Regression):** Die "keine Doppelwand"-Korrekturen loeschen je eine Wandlinie unbedingt (karte_r39.c s_korr,
  19 Rechtecke; s_kanten 5 Schema-Kanten). Ist der Nachbar, dessen Linie stehen bleibt, nicht gezeichnet (nicht besucht,
  Etagenplan nicht im Besitz), ist der Raum offen. Gemessen (je Ort allein besucht, VORHER-exe vs NEU-exe): 1070 34 px
  (ganze Westwand x196), 1010 je Zone 30 px (beide Seitenwaende), ROOM1000 Ostraum 41 / Nord-WC 26 / Sued-WC 12 px,
  1040 17, 1020 16, 1220 Zonen 0/2 je 16, 1050 8, 1080 8. Im normalen Spiel erreichbar (ohne 1F-Plan aus ROOM1030):
  ROOM1000 Nord-WC vor dem Sued-WC (WCs sind auf Nutzerwunsch getrennte Orte), 2F -> 1060 -> 1040 -> 1020 -> 1010 vor der
  Halle 1030 (Treppenhaus 1060 verbindet 3F/2F/1F mit 1040). 1050 nur, wenn man es vor der Halle 1030 betritt (aus
  10A0 = Treppenhaus 1F/B1/B2); 1220 nur in Sonderzustaenden (11E0 kommt immer vorher). Pruefen: Spielstand `probe_r36_karte karte <mcr> 1000 650 0 50 0 besucht:1000:650:50`, CONTINUE,
  Kartenschirm: Nord-WC ohne Westwand x207 und ohne Unterkante y138. Kein Riegel prueft Teilzustaende.
- **M4 (mittel):** Treppensymbole 1060 (1F/2F) und 10A0 (1F/B2) wurden nach der Vorlage verschoben, die Abbildung dieser
  Raeume nicht: Marker auf der Treppe (Projektion der STAIR-AOT-Mitten, ROOM1060 @0x00D72ff, ROOM10A0 @0x00E40ff) liegt
  1F 1060 (127,144)/(135,146) gegen Symbole (122,144)/(133,146), 2F (126,144)/(134,146) gegen (121,144)/(132,146),
  1F 10A0 (187,100)/(196,99) gegen (183,100)/(194,99), B2 (177,115)/(186,112 = Wandpixel) gegen (173,115)/(183,112).
  Echte exe: Leon auf der 1060-Treppe (25150,0,22000) -> Ring (126,143) rechts neben dem Symbol x120..124. Kein Riegel.
- **M5 (mittel, teils Nutzerentscheidung):** Marker weiter neben der Zeichnung: ROOM1210 207/1141 (SW-Kammer, weder
  Original noch Vorlage zeichnen sie - Entscheidungsfrage des Baus berechtigt), ROOM1170 Zone 1 51/311, ROOM1030 63/2288,
  ROOM1110 8/1237, ROOM1060 4/155, ROOM10A0 Band 1 8/140 (vorbestehend, im Dossier OFFEN benannt).
- **M1 (gering):** 3F-Treppe `{4,3,168,107,5,...}`: die Vorlage verschiebt sie um 1/3 Spielpixel; umgesetzt als volles
  Pixel nach unten -> alle drei Sprossen stehen auf der Linie y110 (x166 auf dem Tuerbalken 1130/1170). Blockmitten der
  Vorlage ergeben y104..108; der Riegel (r39_karten_soll.inc) rundet anders und meldet 0. Entscheidung/andere Loesung
  noetig (bei 1x passen 5 Sprossenzeilen nicht mit Abstand zu beiden Linien in y104..109).
- **M2 (gering):** B1 10A0-Treppe 4<->3 `{0,255,145,165,4,...}`: unterste Sprosse (143..147,167) liegt parallel direkt
  auf der Unterkante y168 der Absatz-Zeichnung; unit_r39_karten_b1 prueft nur "nicht auf der Randlinie".

## 11. Hinweise (kein Mangel)
- "Rot nur fuer die Zone, in der man steht" (ROOM1000/B1) deckt sich mit dem Nutzerwortlaut 2026-09-13; dem Nutzer melden.
- B1 zeigt keine Original-Kunst MAP01.PIX mehr, sondern die Schema-Rechtecke (Nutzer-Modell Runde ~09-02); Optik beurteilt
  der Nutzer.
- Alte Spielstaende: die Gang-Abschnitte idx 1..4 sind entfallen; wer den Gang nur dort betreten hatte, sieht ihn bis zum
  naechsten Betreten als unbesucht (nicht gemessen, aus der Tabelle abgeleitet).
- 11D0-Kammer "Zwinger gilt im Randstreifen als aktuell" (Dossier OFFEN): an begehbaren Randpunkten (4000,-19850),
  (5650,-19850), (3050,-21000) nicht reproduziert - Sonde meldet zid 111.

## 12. Ergebnis
**bestanden = false** (Punkt 1 teilweise: M3 Regression, M4/M5 Marker, M1/M2 gering; Punkt 2 erfuellt; alle Gates halten).
