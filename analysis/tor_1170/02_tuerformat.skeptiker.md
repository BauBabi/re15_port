# Skeptiker-Befund zu 02_tuerformat.md (DO2 / MD1 / TIM)

Stand 2026-09-28. Das Dossier wurde nicht geaendert. Der Port wurde nicht gebaut, kein git-Befehl
ausser `status`. Alle Pruefskripte liegen unter `build/tor_1170/skeptiker_tuerformat/`.

## 1. Wie geprueft wurde

| Werkzeug (eigen) | Zweck |
|---|---|
| `sk_common.py`, `sk_dis.py`, `sk_jal.py` | eigene Disassembly ueber capstone, Adressabbildung aus dem PS-X-EXE-Kopf; `re15_disasm.py` wurde NICHT benutzt |
| `sk_lesen.py` | Minimal-Leser fuer Container (RE1.5-Kopf, RE2 ueber EXE-Tabelle @0x8009a520), MD1, TIM - ohne Import aus `do2_format.py` |
| `sk_lader_sim.py` | Lader-Nachbau auf einem RAM-Abbild: Relokation, tpage/clut-Zuschlag, Tex-Kopie, Objektaufbau, POLY_GT3-Aufbau, Lesen der Zeichenroutine - Schritt fuer Schritt nach den Instruktionen |
| `sk_statistik.py`, `sk_tim.py`, `sk_tim_uv.py` | Zaehlungen ueber alle 56 Archive |
| `sk_eigenmodell.py` | eigenes Modell (2 Meshes, 7 + 2 Dreiecke) mit `do2_format.py` GESCHRIEBEN, mit `sk_lader_sim.py` + `sk_lesen.py` GELESEN |
| `sk_timschreiber.py` | `tim_aus_bild` gegen den eigenen TIM-Leser, 10 Faelle |
| `sk_abnahme.py` | Rundlauf 56 Archive, Vergleich durch mich; Lage und Inhalt der Teile gegen den eigenen Leser |

Binaerdateien: `info/Re1.5/PSX.EXE` (718848 B), `info/re2leon/PSX.EXE` (987136 B),
`info/Re1.5/PSX/DOOR/DOOR00.DO2` (57016 B, `cmp` gleich mit `re15_port/shared_assets/PSX/DOOR/DOOR00.DO2`),
55 x `info/re2leon/COMMON/DOOR/DOOR??.DO2`. Keine gepatchte Quelle benutzt.

## 2. Ergebnis je Aussage (Schwerpunkt)

| id | Urteil | Kern des eigenen Belegs |
|---|---|---|
| md1-kopf | bestaetigt, 2 Einschraenkungen | `@0x80022180 lw v0,4(s0)` / `@0x80022188 bnez`; `@0x8002288c..ec`: relokiert +0,+8,+16,+24 je 28 B, Basis `a3 = a0+8` = MD1+12, `@0x800228ac sw v0,0(a0)` (v0=1), `@0x800228e8 bnez t0` nach `addiu t0,t0,-1` = do-while. `@0x8001705c..94`: Mesh x56, Vierecksgruppe +28. tex_off-Gleichung 56/56. Einschraenkung siehe 3.1, 3.2 |
| md1-saetze | bestaetigt, Beleg falsch beschriftet | `@0x80016bf0 lh v0,2(a0)`, `@0x80016bd0 lh v0,6(a0)`, `@0x80016bdc lh v1,10(a0)`, je `sll ..,3`; GTE V0/V1/V2 = +2/+6/+10. Tex-Worte: `@0x800259fc lw v0,0(t3)` -> Prim+0x0C, `@0x80025a08 lw v0,-4(t1)` -> Prim+0x18, `@0x80025a1c lw v0,0(t1)` -> Prim+0x24. Siehe 3.3 |
| md1-regelaufbau | bestaetigt | eigener Leser: 56/56 lueckenlos und ueberlappungsfrei, 56/56 Reihenfolge Kopf, Tabelle, vtx, nrm, prim, tex; 124 Meshes teilen vtx/nrm zwischen beiden Gruppen |
| md1-tex-block | bestaetigt | `@0x800221a4 lw a1,24(s3)`, `@0x8002227c..84` Ziel 0x8018fff0-Summe, `@0x8002228c jal 0x800104b0`, `@0x800222b0 sw s0,4(a0)`. RE2 `@0x80076c28 bnez s5`, Aufruf `@0x80013d84 addiu a0,zero,1`. Negativkontrolle: Bit in gruppe[2].tex_off gekippt -> RE1.5-Lader merkt es nicht (Zeiger wird neu verteilt) |
| nur-dreiecke | bestaetigt | eigene jal-Suche: 0x800166c4..0x80016b50 nur `jal 0x80068098` und `jal 0x80016b54`, kein jalr; 0x80014234..0x80014688 nur `jal 0x8008e1f4` und `jal 0x8001468c`. `@0x80016ba4 lw v1,4(s4)`, RE2 `@0x80014290 addiu s1,s0,0x14` / `@0x80014650 lw a2,0(s1)`. 8605 Dreiecke, 0 Vierecke |
| drehsinn | bestaetigt | `@0x80016cfc .word 0x4b400006`, `@0x80016d18 swc2 $24`, `@0x80016d24 bltz v0,0x80016ee0`; RE2 `@0x800148f8 bgez v0,0x80014938`. Zaehlung 8455 / 137 (11 Modelle) / 4 / 9 |
| tpage-clut | bestaetigt | 0x0080/0x7800: 8371 Dreiecke in 55 Modellen; 0x0095/0x7fc0: 234 in RE2 DOOR28. `@0x80013d7c..98`, `@0x80076ba4 sll s1,s1,0x10`, `@0x80076ba8 sll s2,s2,0x16`. RE1.5 `@0x80016364 ori a0,zero,2`, `@0x80016370 move a2,zero`, `@0x8001639c move a3,zero` (Delay-Slot des jal). Nach dem Lader-Nachbau: RE2 8560 Dreiecke 0x95/0x7fc0, RE1.5 45 Dreiecke 0x80/0x7800 |
| transparenz | TEILWEISE WIDERLEGT | siehe 3.4 |
| tim-schreiber | bestaetigt, Schranke gilt nur fuer das eine Bild | siehe 3.5 |
| angel | bestaetigt | RE2-Handlertabelle Basis 0x800a74c8; Eintrag 0x2E @0x800a7580 = 0x80055904. Work_set Art 5 -> `[0x80011200]` = 0x800559b0 -> `@0x800559bc lw v0,0x4dd8(at)`. Speed_set `@0x80055aac sh a1,0x158(v1)`; Add_speed `@0x80055ae8 lh a2,0x160(a0)` -> `@0x80055b0c sh v0,0x76(a1)`. Aufbau-Saetze RE2 DOOR00: @0x5036 Lage (2000,3790,2048) Drehung 0; @0x504c Knauf (130,-3224,-3372), Flags 0x00d0 = Kind von Platz 0; @0x5096 Lage (2000,3790,-1600) Drehung (0,2048,0) |
| referenzblatt | bestaetigt | eigener Leser: 8 Vertices, 24 Normalen, 12 Dreiecke, Rahmen (-145..143, -6600..2, -3599..0); derselbe Quader als mesh0 in 37 Modellen |
| uv-bereich | bestaetigt | u 0..127, v 0..255; Blattflaechen: (y=2,z=0) -> (126,217), (y=-6600,z=-3599) -> (0,0), beide Seiten gleich; Knauf u 0..60, v 219..255; @Datei 0x788 stimmt |
| re15-reihenfolge | bestaetigt | Bytes @0x8BC8, @0x8BD0 pBAV, @0x97F0 `08 00 00 00`; `@0x80017128 lw v0,-0x2308(at)` mit at = s0+0x80200000; VH: ps=1 -> 0xC20; 0x97F8-0x8BC8 = 0xC30 |
| re15-primpuffer | bestaetigt | `@0x800171f8/204` 0x801a1000, `@0x8001635c/60/78` 0x801ab000 -> 0x800b8550; Reihenfolge der vier jal gelesen |
| abnahme | bestaetigt | 56/56 byte-identisch (Vergleich durch mich); Lage von MD1 und TIM 56/56 gleich mit dem eigenen Leser; Inhalt (Vertices, Normalen, Dreiecke, Tex) 56/56 gleich; RE2-Tabellenzeile + beide Pruefsummen 55/55; RE2 DOOR00 -> re15 (55528 B) -> re2 = Original |
| probemodell | bestaetigt, 1 Hinweis | Laengen 54784 / 54016, sha1 wie behauptet, MD1 212 B; Lader-Nachbau liefert in beiden Containern dieselben 4 Dreiecke; Tabellenzeile: 0xd300 = Dateiende, XOR 0x9a / 0x0c selbst gerechnet. Hinweis siehe 3.6 |
| eigenes Modell | bestanden | 9 Dreiecke in 2 Meshes (MD1 604 B): Lage, UV, tpage/clut, Code 0x34 und Normalen nach dem Lader-Nachbau ohne Abweichung, in beiden Containern (55176 B / 54408 B). Textur 146 Farben: 3 durchsichtig -> Index 0 / Wert 0, 14781 schwarz -> 0x8000, 17984 farbig, 0 Abweichungen. Ausgabe `FEHLER GESAMT 0` |

## 3. Abweichungen und Einschraenkungen

### 3.1 Der Merker MD1+4 gilt nur fuer RE1.5
RE2 `FUN_8002cfd8` (`@0x8002cfd8 lw a3,4(a0)` ... `@0x8002d020 bnez a3`) prueft und setzt keinen Merker;
`FUN_80076b60` ruft die Relokation unbedingt (`@0x80076b98 jal 0x8002cfd8`). "Lader setzt 1" ist RE1.5.

### 3.2 vtx_anz, nrm_anz und MD1+0 liest kein Lader
Die Relokation ueberspringt +4 und +12 der Gruppe, die Zeichenroutinen rechnen nur mit Indizes.
Negativkontrolle: ein gekipptes Bit in diesen Feldern aendert am Lader-Nachbau nichts (156 von 604
MD1-Bytes des eigenen Modells sind fuer den Tuer-Lader ohne Wirkung: MD1+0, beide Zaehler, die ganze
Vierecksgruppe, alle pad-Felder, tex_off der Gruppen > 0). Die Deutung der Zaehler ruht auf der
Belegung (56/56 lueckenlos), nicht auf einer Instruktion.

### 3.3 Falsch beschriftete Adressen im Beleg zu md1-saetze
`0x800259fc`, `0x80025a08/14/1c/6c` und `0x8002579c` stehen im Beleg hinter "RE2", sind aber RE1.5-Adressen.
In RE2 ist `0x8002579c` = `sw s7,0x44(sp)` (Funktionsvorspann). Die RE2-Gegenstuecke:
`@0x8002cc88 lw v0,0(t3)`, `@0x8002cc94 lw v0,-4(t1)`, `@0x8002cca8 lw v0,0(t1)`, `@0x8002ccf8 addiu t1,t1,0xc`.

### 3.4 transparenz: "7 zeichnen tatsaechlich" stimmt nicht
`tim_statistik` zaehlt, ob ein Index IM BILD vorkommt, nicht ob er in einem UV-Dreieck liegt.
Konservative Messung (Texelzelle beruehrt ein UV-Dreieck, `sk_tim_uv.py`):

| | im Bild vorhanden | von einem UV-Dreieck beruehrt |
|---|---|---|
| Wert 0x0000 | 7 (DOOR10, 16, 19, 1F, 28, 2A, 2B) | 3 (DOOR10 8677, DOOR1F 763, DOOR28 156 Texel) |
| Wert 0x8000 | 6 | 5 (DOOR09, 0E, 1A, 2A, 2D) |
| STP-Bit gesetzt | 8 | 6 |

Bestaetigt bleiben: CLUT[0] = 0x0000 in 48 von 56; RE2 `@0x80014734 lhu v0,0x144(s7)` /
`@0x8001473c srl v0,v0,0xd` / `@0x80014740 andi v0,v0,2` / `@0x80014744 ori v0,v0,0x34`.
RE1.5 setzt den Code fest auf 0x34 (`@0x80016bb8 ori v0,zero,0x34` / `@0x80016bbc sb v0,0x67(s4)`).

### 3.5 tim-schreiber: die Fehlerschranke ist keine Eigenschaft des Schreibers
| Fall | Farben | groesster Fehler (von 31) | Mittel |
|---|---|---|---|
| 17 / 255 Farben, 64 durchsichtig | 17 / 255 | 0 | 0 |
| gate_03.png eingesetzt | 241 | 0 | 0 |
| ROOM11712 BILINEAR verkleinert | 273 | 1 | 0,00073 (groesster Kanal) |
| ROOM11712 NEAREST | 285 | 2 | 0,00143 |
| 256 Farben, keine Transparenz | 256 | 4 | 0,023 |
| 1500 Farben | 1500 | 6 | 2,47 |

- 273 Farben entstehen nur mit BILINEAR; LANCZOS gibt 310, NEAREST 285. Der Filter steht nicht in der Aussage.
- 256 Farben ohne Transparenz werden quantisiert, weil `index0_transparent=True` einen Platz sperrt.
- Der vom Werkzeug gemeldete Fehler schliesst durchsichtige Punkte ein (gemeldet 5, ueber deckende gemessen 3).
- In keinem Fall wurde ein deckender Punkt zu 0x0000, kein farbiger Punkt trug STP.

### 3.6 Probemodell laeuft in u gegen die Referenz
Probe: u=0 an der Angel (z=0), u=127 an der freien Kante. Referenzblatt: u=126 an der Angel, u=0 an der
freien Kante. Von der Kameraseite (+x, Bild-rechts = +z) gesehen steht die Probentextur damit
spiegelverkehrt zur Konvention der Originale. Fuer die Warnschraffur unauffaellig, fuer die Texttafel nicht.

## 4. Nebenbei gemessen (nicht Schwerpunkt)

| id | Urteil | Beleg |
|---|---|---|
| re15-kopf, re15-relbasis | bestaetigt | `@0x800162b4..cc`, `@0x800162ec/f4` 0x801a100c, Bytes @0x00 und @0x0C |
| re15-scd-kopie | bestaetigt | `@0x800171b8 lw a1,0x5364(a1)` = Zeiger auf das FELD, `@0x800171c4 subu v1,v0,a0` |
| re2-modellteil, re2-prim-auf-tim | bestaetigt | `@0x80013d24..54` |
| re2-tabelle, pruefsumme-512 | bestaetigt an den Daten | 55/55 Zeilen, XOR des ersten Bytes je 512 B; die Instruktionen LAB_8001376c nicht gelesen |
| tim-einheitlich | bestaetigt | 56/56 |
| tim-lader-lage | bestaetigt fuer RE1.5 | `@0x8004eea0..ed4`, `@0x8004ef38 addiu v0,v0,0x1e0`; RE2 FUN_80076a40 nicht gelesen |
| objektplaetze | bestaetigt | `@0x80016b18 addiu s1,s1,0x90`; RE2 `@0x80014660 addiu s1,s1,0x14c`, `@0x80014664 slti v0,s2,0xa` |
| aufbau-satz | bestaetigt (die gedeuteten Bytes) | RE1.5 Tabelle Basis 0x800744a8, Eintrag 0x4F @0x800745e4 = 0x80016f20, `@0x80017020 addiu v0,a2,0x16`; RE2 `@0x80014cac` |
| normalen | WORTLAUT WIDERLEGT | 5622 = "nicht alle drei gleich": drei verschiedene Vektoren 3642, zwei verschiedene 1980, alle gleich 2983 (2593 + 390). Laengen 17609 / 434 / 189 / 4 stimmen |
| doppelseitig | bestaetigt | Gegenflaechen (umgekehrter Drehsinn) DOOR04 2, DOOR10 8, DOOR1F 6. Zusatz: DOOR1E, DOOR33, DOOR35 haben je 112 deckungsgleiche Dreiecke mit GLEICHEM Drehsinn |
| pad-felder, scd-block, door00-gleich | bestaetigt | 5676 / 18236 / 8605; 56/56 durch 4, 27 enden auf 01 00 00 00, 2..18 Skripte; MD1 2444 B und TIM gleich, 33 / 48 verschiedene |
| primitiv-groesse | Zahl 307 bleibt abgeleitet | `@0x800166c8..f8`: Teilungspuffer 0x801b7000 bei Bildpuffer 0, 0x801b1000 bei Bildpuffer 1; 0x6000 / 80 = 307 |

Nicht geprueft: re2-kein-kopf (Tonteil), re15-dateitabelle (nur XOR = 221 nachgerechnet), ton-vorspann,
java-abweichung, gegenprobe, blattmasse, rohrquerschnitt.

## 5. Offene Risiken fuer den Modellbau

1. RE1.5 kopiert die Textursaetze nach 0x8018fff0 - 12 x Dreieckszahl. Belegt ist nur der Fall DOOR00
   (540 B ab 0x8018fdd4). Was darunter liegt, ist nicht bestimmt; bei 307 Dreiecken beginnt die Kopie bei 0x8018f18c.
2. 8 RE2-Modelle haben mehr als 307 Dreiecke (DOOR26/31 490, 2E 474, 27 447, 2D 430, 0A 416, 1E/33/35 336, 19 310).
   Die RE1.5-Grenze ist fuer das Tor unkritisch, fuer uebernommene RE2-Tueren nicht.
3. RE1.5 laedt die TIM nach Seite 0x15 / CLUT-Zeile 511, laesst die Primitive aber auf 0x0080 / 0x7800.
   Ein byte-treuer RE1.5-Lauf zeigt die Tuertextur nicht; die Zuschlaege 0x15 / 0x1F stehen nur in RE2.

## 6. Befehle

```
cd build/tor_1170/skeptiker_tuerformat
python sk_lesen.py            # 56 Archive, Lage von MD1/TIM, Dateiende
python sk_statistik.py        # Zaehlungen MD1
python sk_tim.py ; python sk_tim_uv.py
python sk_lader_sim.py        # Lader-Nachbau an drei Originalen
python sk_eigenmodell.py      # eigenes Modell schreiben + unabhaengig lesen
python sk_timschreiber.py
python sk_abnahme.py
python sk_dis.py 15 0x80022150 112 ; python sk_dis.py 2 0x80076b60 86
python sk_jal.py 15 0x800166c4 0x80016b50 ; python sk_jal.py 2 0x80014234 0x80014688
```
