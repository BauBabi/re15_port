# SF Zahlen-Pruefung: Kopf aus RE2 (G2 0x31 Mesh 3 + G3-Zahn-Kachel), Runde 1 (Pruefer, 2026-11-07)

Gegenstand: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4re2.EMD`
(= `EM30_V4.EMD` daneben), sha256 `f37bc5514c6892186d90a808dfd20f646aeecb59b0667933b3daa84f296ff18d`, 262284 Byte (selbst gemessen).
Basis `EM30_V4ot3.EMD` sha256 `a1bc9309...`, 262004 Byte. Referenz 2 `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` sha256 `cfd1ac42...`, 274420 Byte.
Bauer-Commits f893718aa, 0b6d20c63, 432c36b8f, 40de9d6de, dc0f76e47 (Zweig r41/sichel_nb2).

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Unter Version_80%/, pri/, synchro/ nichts geaendert.

**Ergebnis: NICHT bestanden (2 rote Punkte am Render, Struktur vollstaendig gruen).**

## 0. Eigene Werkzeuge (unabhaengig vom Riegel des Bauers)

Alle im Scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/zp_re2/`:

| Datei | misst | Ausgabe |
|---|---|---|
| `emdl.py` | eigener EMD-Directory-/MD1-/TIM-Leser (Runde 2/3) | - |
| `struct_check.py` | eigener TOC-/EMS-Leser (600 u32 aus `re2_ems_toc.inc`, Index ((kind-0x10)*4+rec)*2), Sektionen, Meshes, Umeyama-Transformation, Texel, CLUT | `struct.json`, `struct_out.txt` |
| `emr_check.py` | EMR Sektion 2: rel/Keyframes Knochen 0-15 gegen Ref2 | stdout |
| `rend_check.py` | Render (v4_gemein.zeichne / b2, Kamera 50A0 Cut 12, RDT-Licht, Posen 135/85): Kopfbox, Fleck nach Messblatt-Methode (p_blob), eigene Marker-Renders (Index, der in der Zeilen-Region unbenutzt ist), Zaehne, Haelften, KS2 | `rend.json`, `kopf_135.png`, `kopf_85.png`, `voll_135.png`, `voll_85.png` |
| `auge_check.py` | Augenfleck unter Licht: K1-Kontrast (Videoraum), K3-Groesse | `auge.json` |
| `k7_check.py` | Messblatt-Fenster KV1-KV5 (Videoraum) neu / ot3 / Ref2 | `k7.json` |
| `ref2_vgl.py` | Kopf-L*, Haelften, Kopf/Rumpf neu / ot3 / Ref2; Kontrolle des Bauer-Marker-Index | `ref2_vgl.json`, `ref2_kopf_135.png` |
| `ctest_pruefer.txt` | eigener ctest-Lauf der 4 Tests | - |

## 1. Koerper = Ref2, Schwanz = ot3 (Auftrag (1)) - GRUEN

| Mass | Soll | Ist (eigener Leser) |
|---|---|---|
| Meshes | 23 | Ref2 23 / neu 23 / ot3 23 |
| Koerper-Meshes 0-8, 10-15, alle 8 Felder (tv, tn, tf, tu, qv, qn, qf, qu) | bytegleich Ref2 | abweichend: **keine** (auch gegen ot3 keine) |
| Schwanz-Meshes 16-22, alle 8 Felder | bytegleich ot3 | abweichend: **keine** |
| Koerper-Texel (von Koerperflaechen abgedeckt, eigene Rasterung) | bytegleich Ref2 | 34431 abgedeckt, **0** abweichend; UV-Maske Ref2 = neu |
| Schwanz-Texel (Meshes 16-22) | bytegleich ot3 | 10356 abgedeckt, **0** abweichend |
| Sektionen 0-6 | bytegleich ot3 | alle 7 gleich (Laengen 4716 / 5520 / 106056 / 4 / 4 / 400 / 7688) |
| Sektionen 0-6 gegen Ref2 | 0-15 gleich | 0, 1, 3, 4, 5, 6 bytegleich; Sektion 2 (EMR) weicht in 28219 Byte ab: 28203 davon in den Keyframes an den Byte-Offsets 84-115 je Keyframe (Winkel der Knochen >= 16, Schwanz), 16 Byte in der Link-Tabelle vor den Keyframes; **rel-Positionen aller 23 Knochen gleich, Keyframe-Positionen gleich (0 von 912), Winkel der Knochen 0-15 gleich (0 abweichend)**, Winkel 16-22 17237 abweichend (V4-Schwanzgang, identisch zu ot3 - Basis seit Runde 1) |
| Texel geaendert gegen ot3 | nur alte Kopfflaechen + G2-/G3-Orte | 11629 geaendert, **0** ausserhalb der Orte x 0-125 / y 160-249 (TPage 128) und x 256-267 / y 200-235 (TPage 130); die 3621 Texel der alten ot3-Kopfflaechen liegen vollstaendig im G2-Ort; Ueberlapp der Orte mit Koerper-/Schwanzflaechen 0 / 0 |

## 2. Herkunft Kopf = RE2 (Auftrag (2)) - GRUEN

Quelle (eigener TOC-Leser): `re15_port/shared_assets/RE2/CDEMD0.EMS` sha256 `8724200c1ed5e8d3235ceb85640893467eb9878c2a016529aa84b49a79916fca`.
kind 0x31: EMD Sektor 2944, 193124 B, sha256 `bb966cd5...`; TIM Sektor 2895, 99872 B, sha256 `e0929cef...`.
kind 0x33: EMD Sektor 3114, 204968 B, sha256 `c9ab4129...`; TIM Sektor 3065, 99872 B, sha256 `99b793e2...`. Alle Werte = Protokoll des Bauers.
G2-EMD: 8 Directory-Eintraege, MD1 (dir[7]) 18 Meshes; Mesh 3: 48 Ecken / 48 Normalen / 56 Dreiecke / 14 Vierecke; Quelle-UV CLUT 480 / TPage 128, u 0-125, v 0-89.
G2-/G3-TIM: 192 Worte x 256 = 384 x 256 px, 8 bpp, CLUT 256 x 3 Zeilen bei (0, 480).

| Mass | Soll | Ist |
|---|---|---|
| Mesh 9 Zaehler | 48 / 48 / 56 / 14 (Header tvc/tnc/tfc/qvc/qnc/qfc) | 48 / 48 / 56 / 48 / 48 / 14; qv teilt den tv-Offset, qn den tn-Offset (wie G2); ot3 hatte 61 / 73 / 18 |
| Flaechen-Indizes tf, qf | = G2 Mesh 3 | **gleich** (beide Tabellen bytegleich) |
| Aehnlichkeitstransformation (eigene Umeyama-Schaetzung Y = s R X + t) | Bauer: s 1,50, Rz(+10) Ry(-34), t (140, +10, 0) | **s 1,4998; Rz(10,00 Grad) Ry(-33,99 Grad); t (140,02 / 9,94 / 0,01)**; max |R - Rz(10)Ry(-34)| 0,00022 |
| Residuum Ecken | <= 1 E. (Rundung) | geschaetzt: max 0,69 E., RMS 0,47 E.; mit den Bauer-Parametern exakt: max 0,67, RMS 0,48 (Ganzzahl-Rundung) |
| Normalen | gedreht, Laenge erhalten | Residuum gegen R . N_G2 max 1,84 (von 4096); Laenge p50 4096 -> 4096,1 |
| Bbox neu | - | (-204, -489, -303) .. (532, 210, 263) E. (G2: (-248, -347, -185) .. (228, 118, 180)) |
| UV der 70 Flaechen | 59 = Quelle + (0, +160), TPage 128, CLUT 486; 11 Maulflaechen TPage 130, CLUT 487 | **59 / 11 / 0 andere**; Maul = Dreiecke 5, 22, 25, 26, 28, 29, 34, 35, 42, 43, 44 (= Bauer); Maul-UV x 0-11, y 200-235 (in der Kachel x 256-267 / y 200-235) |
| von Mesh-9-Flaechen abgedeckte Texel | nur in den G2-/G3-Orten | 5415 Texel, **0** ausserhalb |
| G2-Texel (x 0-125, y 160-249 = G2 u 0-125, v 0-89) | bytegleich oder dunkler (derselben Zeile) | 11340 Texel (6524 nicht-null); **3821 abweichend, alle 3821 mit L* <= Quelle (0 heller)**, 0 Null<->Nicht-Null; dL* Mittel -9,5, min -40,3; 852 davon in den Spalten u 41-60 (Nase) - Bauer: 3821 |
| G3-Zahn-Kachel (x 256-267, y 200-235 = G3 u 2-13, v 5-40) | bytegleich | 432 Texel, **0** abweichend |
| CLUT-Zeile 486 / 487 | = G2 Zeile 0 / G3 Zeile 0, 15-Bit roh | **gleich / gleich** |

## 3. Masse am eigenen Render (Auftrag (3)) - 2 ROT

Render: `v4_gemein.zeichne` (b2, BSS 50A0 Cut 12 als Hintergrund, RDT-Licht), Posen `posen_v4.json` "135" (vorn, Clip 0 Bild 9, Gier 1737) und "85" (seitlich, Clip 1 Bild 26, Gier 556).
H_port = Sohle - Oberkante der Koerper-Ids ohne 16-22: Pose 135 **H 364** (OK 145, Sohle 509), Pose 85 **H 264** (OK 170, Sohle 434).

### 3.1 Gruen (Zahlen decken sich mit dem Riegel des Bauers)

| Mass | Soll (Messblatt) | Ist (eigen) | Bauer |
|---|---|---|---|
| K10 Kopfbox vorn (Bbox Id 9, Pose 135) | 0,14-0,18 x 0,15-0,20 H | Box 325-384 x 160-221 -> **0,165 x 0,170 H** | 0,165 x 0,170 |
| K4 **nach Bauer-Methode** (Marker der Augen-Texel; eigene Komponente 1 = helle G2-Texel L* > 45, u 18-22 / v 10-13, 13 Texel, L* 62) | 0,035-0,065 H links; 0,07-0,12 H unter OK | 6 Bildpunkte (331-333 x 194-196), Schwerpunkt (332,0 / 195,2) -> **0,062 H links, 0,097 H unter OK** | 0,060 / 0,097 (19 px, Maske mit 1-Texel-Ring) |
| K7 Maul-Lage (Zahn-Marker = Kachel-Texel L* > 60, Chroma < 20: 35 Texel) | Schwerpunkt x 0,3-0,7 / y 0,55-1,0 der Kopfbox | 55 Bildpunkte, Schwerpunkt (360,8 / 201,0) -> **0,596 / 0,662**; ganze Kachel 641 px bei 0,633 / 0,754 | 0,60 / 0,66 (58 px) |
| K7 Messblatt-Fenster (Runde-3-Fassung, Videoraum, Fenstermittel) | \|KV3 - KV5\| <= 4; KV3 >= KV3r + 1 | KV3 28,8 / KV5 25,6 / KV3r 27,1 -> **3,2 / +1,7**; (KV5 nur zu 43 % Kopf, kopfmaskiert 12,0) | - |
| Zaehne sichtbar (Pose 135) | >= 20 Marker-px; >= 5 helle unbunte px unter 0,5 Kopfbox | **55 Marker-px**; hell-unbunt (L* > 45, C < 20) unterhalb 0,5 Kopfbox: roh 62, mit Licht 44; Pose 85: 0 Marker (Seite) | 58 / 44 / 0 |
| S2 Augenlage seitlich (Marker Komponente 1, Pose 85) | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 6 px (471-474 x 219-220), Schwerpunkt (472,5 / 219,3) -> **0,044 H / 0,813 H** | 0,043 / 0,812 |
| KS2 Haarkante seitlich (Fenster 0,84-0,96 x 0,52-0,80 der Kopfbox, Pose 85, nur Kopf-px) | L* <= Kopf-Median (Videoraum) - 5 | Box 478-482 x 215-227, 65 px, 44 Kopf (68 %); **Kopf L* 14,8** (p50 12,5; ganzes Fenster 22,8); Kopf-Median 22,5 -> Grenze 17,5 | 14,8 / 22,8 / 22,5 |
| Bild-links: Auge | Auge links der Kopfmitte | Marker x 332,0 < Mitte 354,5 -> **links** (0,062 H) | - |

### 3.2 ROT 1: Auge vorn ist unter RDT-Licht kein heller Fleck; K4 nach Messblatt-Methode liegt auf den Zaehnen

Messblatt V4 Abschnitt 7 definiert K4 als "Fleck-Schwerpunkt minus Kopfbox-Mitte", der Fleck nach K1 (p_blob: hellstes 3x3 in der Kopfbox, Schwelle Mitte aus Extrem und Median 21x21, Videoraum sigma 1,5), mit K1-Kontrast >= 20 und K3-Groesse 0,016-0,036 x 0,023-0,043 H. Der Bauer-Riegel T-R2 K4 misst stattdessen per Marker-Render der Augen-Texel (Methodenwechsel, im Messblatt nicht nachgetragen).

| Mass | Soll (Messblatt) | Ist (eigen, Pose 135) |
|---|---|---|
| K1 hellster Kopfpunkt | = Augenfleck ("der hellste Kopfpunkt ist der Augenfleck", Abschnitt 2) | hellstes 3x3: (368,5 / 196,8), Komponente 36 px, Lab 45,6 / 12,0 / 10,7, Bbox 364-372 x 194-199 = **liegt in der Zahn-Marker-Bbox 347-371 x 191-213: die Zaehne**; 2 Komponenten >= 4 px |
| K4 nach Messblatt (Fleck) | 0,035-0,065 H links | **-0,038 H (rechts der Mitte)** / 0,101 H unter OK |
| Auge unter Licht (Marker-Ort) | heller Fleck; K2: L* >= KV5 + 20 | Lab mit Licht **18,8 / 8,5 / 6,4**; Videoraum-Maximum am Auge 20,0 gegen Median 21x21 = 21,7 -> **K1-Kontrast -1,8** (Soll >= 20); kein Fleck ueber der p_blob-Schwelle (0 px) |
| K3 Groesse | 0,016-0,036 x 0,023-0,043 H | Marker **3 x 3 px = 0,008 x 0,008 H** |
| zum Vergleich | - | roher Render (ohne Licht): Auge weiss-rosa, L* 62 der Texel; Vorlage 1 Augenfleck 46,9 / 7,2 / -1,1 |

Bild `zp_re2/kopf_135.png` (links mit Licht, Mitte roh, rechts Overlay: gruen = Messblatt-Fleck, magenta = Zahn-Marker): mit Licht ist das Auge ein dunkelroter Punkt, der helle Fleck ist das Gebiss. Der Nutzer-Satz "Man sieht alles einwandfrei! Das Auge ..." ist im Vorderbild damit nicht erfuellt; T-R2 K4 ist nur mit der Marker-Definition gruen.

Pose 85 zum selben Punkt: Auge mit Licht Lab 33,9 / 23,4 / 17,9 (roter Punkt, im Bild sichtbar), Marker 4 x 2 px = 0,015 x 0,008 H; im Videoraum Maximum 18,4 gegen Umgebung 20,7 (Kontrast -2,3); p_blob findet keinen Kopf-Fleck (hellstes Kopf-3x3 42,8 < Hintergrund 50,8). S1 ("1 heller Fleck") damit ebenfalls nicht nach Messblatt-Methode messbar; S2 oben nur per Marker.

### 3.3 ROT 2: "dunkle Seite rechts" ist nicht erfuellt

Auftrag (3): "Bild-links/-rechts-Zuordnung (Auge links, dunkle Seite rechts)". Nutzer: "Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich"; Messblatt K5: Haarfeld (B)-rechts, KV2 L* <= Augenfleck - 25.

| Mass (Pose 135, Kopf-Id 9, Haelften an der Kopfbox-Mitte x 354,5) | links (1236 px) | rechts (1225 px) | Befund |
|---|---|---|---|
| L* Median mit RDT-Licht | 15,4 | **15,9** | rechts nicht dunkler (+0,5) |
| L* Median roh (Texelfarbe) | 17,0 | **20,2** | rechts heller (+3,2) |
| L* Median Videoraum | 20,8 | **21,8** | rechts heller (+1,0) |
| Chroma roh (Median) / a*, b* | 5,8 / 4,0, 1,5 (grau) | 16,9 / 15,0, 7,4 (rot-braun) | Farbunterschied ja, Helligkeitsunterschied nein |
| Vergleich ot3 (Runde 3) L* mit Licht | 28,9 | 23,1 | dort rechts 5,8 dunkler |
| K5 Fenster KV2 (Videoraum) | - | L* 27,3 | Soll <= Augenfleck - 25: mit Augenfleck 20,0 (3.2) nicht erfuellbar |

Das Andunkeln (Haar z -80..0 Faktor 0,65, Nase, Spitze; 3821 Texel) ist im Atlas belegt (Abschnitt 2), kommt aber im Bild als Seitenunterschied nicht an: beide Haelften sind gleich dunkel (15-16 L*).

### 3.4 Info (kein Riegel des Bauers, aber Messblatt-Masse der Runde 3 / Abweichung zur gelobten Referenz)

| Mass | neu | ot3 | Ref2 (gelobt) | Messblatt |
|---|---|---|---|---|
| Kopf L* Median mit Licht, Pose 135 / 85 | **15,6 / 17,0** | 27,4 / 31,8 | 30,8 / 30,8 | T-K9 e) (Runde 3): >= 23 |
| Kopf/Rumpf (Id 8) mit Licht, Pose 135 / 85 | **0,59 / 0,52** | 1,05 / 0,97 | 1,16 / 1,02 | T-K9 e): >= 0,9 |
| Kopf L* Median Videoraum, Pose 135 / 85 | 21,1 / 22,5 | 32,6 / 37,0 | 39,4 / 35,2 | Video-Kopf-Median 22,5 (Riegel KS2) |
| KV1 a* (K9 Kopf-Oberseite rot), Videoraum | 6,1 (kopfmaskiert 5,5) | 14,4 | 18,2 | K9: a* >= 11 |
| KS2-Fenster zum Augen-Marker (Pose 85) | Luecke 3 px = 0,011 H (Marker-Bbox 474 / Fenster ab 478) | - | - | S3: Abstand <= 0,01 H (grenzwertig; T-R2 KS2 prueft ihn nicht) |

Der Kopf ist im Videoraum so dunkel wie die Video-Vorlage (21,1 / 22,5 gegen 22,5), aber im Spielbild halb so hell wie Ref2 und wie der eigene Rumpf; die Runde-3-Riegel T-K5/T-K9 (Grundton, Kopf/Rumpf, Kuppe rot) sind im neuen Riegel `riegel_re2kopf.py` nicht mehr enthalten. Entscheid dem Orchestrator/Sicht-Pruefer ueberlassen; hier nur Zahl.

Kontrolle Bauer-Marker (`kopf_re2_mess.marker_render`): gewaehlt wird der erste Index 1..255, der nicht in `tex[maske]` vorkommt - das ist Index 1, der **21-mal sonst in der G2-Region** (192-mal im Atlas) steht. Der Bauer filtert mit einem Teilmodell (nur Flaechen T19/T20/T41), dadurch ist die Verfaelschung auf diese drei Flaechen begrenzt; meine Marker nutzen einen in der ganzen Zeilen-Region unbenutzten Index. Die Abweichung 19 px (Bauer) zu 6 px (eigen) erklaert sich aus der 1-Texel-Ringerweiterung der Bauer-Maske (41 statt 13 Texel). Fuer K4/S2 ergibt sich dieselbe Lage (0,002 H Unterschied).

## 4. CLUT-Zeilen und Texturhoehe (Auftrag (4)) - GRUEN

TIM-Flags 9 (8 bpp mit CLUT); CLUT 256 x **8 Zeilen** bei (0, 480); Bild 192 Worte x 256 = 384 x 256 px. Zeilen 480-485 bytegleich Ref2 (6 / 6), 486 = G2 Zeile 0, 487 = G3 Zeile 0. **Port-Texturhoehe 256 x 8 = 2048 <= 4096**; 8 <= 16 Zeilen. TIM-Sektion 102432 B = ot3 + 1024 (2 x 512). Directory 9 Eintraege, 36 Byte Rest nach der TIM.

## 5. Spiel-Asset, Pin, Tests, Spiellauf (Auftrag (5)) - GRUEN

- `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` sha256 `cfd1ac42729e56365847db42b352337d7f0c06fdcc1ef3413d0aee32d4a97a95` = Inhalt `release/em30_hangar.sha256`; `git diff HEAD -- release/ re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` leer.
- Eigener Lauf `ctest --test-dir .../r41_sichel/re15_port/build -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form" --output-on-failure --timeout 900`: **"100% tests passed, 0 tests failed out of 4"**, Total Test time (real) = 64.47 sec, EXIT=0 (`zp_re2/ctest_pruefer.txt`). Exe `re15_port/build/platform/pc/re15_pc.exe` unveraendert (Bau 2026-11-07).
- Spiellauf `scratchpad/sfv4/r/V4re2/debug.log` (1008 Zeilen) gelesen: Zeile 2 `[asset] cd-root: .../sfv4/r/V4re2/cd (+Fallbacks)`, Zeile 136 `[enemy] EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)`, Zeile 137 `[enemy] EM30 loaded: 23 meshes, 23 bones, 21 clips -> slot 12`; `grep -i "FAILED|error|fehler|assert"`: **0 Treffer**; Overlay-Datei `sfv4/r/V4re2/cd/EMD/EM30_HANGAR.EMD` sha256 `f37bc551...` = Pruefgegenstand; 191 Bilder in `fd/`.

## 6. Riegel-Tabelle des Bauers (riegel_re2kopf_V4re2.txt) gegen eigene Zahlen

T-K1a/b/c, T-K2s, T-R0, T-R1a-d, T-R4, G1, M3: in allen Zahlen bestaetigt (Abschnitte 1, 2, 4). T-R2 K10 / K4 (Marker) / K7 / S2 / KS2 und T-R3: bestaetigt (3.1). Nicht im Bauer-Riegel, aber im Messblatt: K1/K3/K4-nach-Fleck (3.2 ROT), Seitenzuordnung dunkel rechts (3.3 ROT), T-K9 e) / K9 / S3-Abstand (3.4 Info). T5 0,098 H bekannt rot seit Runde 5 (Geometrie, kein Grund).

## 7. Urteil

**NICHT bestanden.** Struktur, Herkunft, Transformation (s 1,4998, Rz 10,00 / Ry -33,99 Grad, t (140,0 / 9,9 / 0,0), Residuum max 0,69 E.), Texel-Herkunft (7519 bytegleich, 3821 dunkler in derselben G2-Zeile, Zahn-Kachel 432/432), CLUT 8 Zeilen / 2048, Koerper = Ref2, Schwanz = ot3, Tests 4/4, Spiellauf sauber.
Rot: (1) K4 nach Messblatt-Methode -0,038 H (hellster Kopfpunkt = Zaehne), Auge unter Licht L* 18,8 / Kontrast -1,8 / 0,008 H gross - das Auge ist vorn kein sichtbarer heller Fleck; (2) rechte Kopfhaelfte nicht dunkler als die linke (15,9 gegen 15,4 L* mit Licht), nur farbiger.
