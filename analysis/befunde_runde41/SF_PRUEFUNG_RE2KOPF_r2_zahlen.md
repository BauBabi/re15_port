# SF Zahlen-Pruefung: Kopf aus RE2 (G2 0x31 Mesh 3 + G3-Zahn-Kachel), Runde 2 (Pruefer, 2026-11-07)

Gegenstand: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4re2.EMD`
(= `EM30_V4.EMD` daneben, sha256 gleich), sha256 `e9c66c2edcb846f8613ea2f309810bb82b09bb7b4e25bd0917ba7ae9297363a5`, 262796 Byte (selbst gemessen).
Basis `EM30_V4ot3.EMD` sha256 `a1bc9309...`, 262004 Byte. Referenz 2 `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` sha256 `cfd1ac42...`, 274420 Byte.
Bauer-Commits a04efd690, aa6c8a181, 24c3d423a, c8936d133 (Zweig r41/sichel_nb2; Arbeitsbaum ohne untracked Dateien, die r3-Pruefberichte sind committet).

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Unter Version_80%/, pri/, synchro/ nichts geaendert.

**Ergebnis: BESTANDEN - 0 rote Punkte in den Auftraegen (1)-(5) (bekannt rot nur T5 0,098 H, Geometrie seit Runde 5). Abschnitt 3.4 nennt die
Stellen, an denen der Bauer-Riegel vom Messblatt-Wortlaut abweicht (Methodenwechsel Auge, K5-Band, K6-Mittel, K9, T-K5 b*) - Entscheid Orchestrator,
Messblatt-Nachtrag noetig.**

## 0. Eigene Werkzeuge (unabhaengig vom Riegel des Bauers)

Alle im Scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/zp_re2/`:

| Datei | misst | Ausgabe |
|---|---|---|
| `emdl.py` | eigener EMD-Directory-/MD1-/TIM-Leser (seit Runde 2) | - |
| `struct_check_r2.py` | eigener TOC-/EMS-Leser (600 u32 aus `re2_ems_toc.inc`, Index ((kind-0x10)*4+rec)*2), Sektionen, Meshes, Umeyama-Transformation, UV + Zonenregel je Flaeche (eigene Nachrechnung), Texel, CLUT inkl. eigener Nachrechnung der Palettentransformation 486/488 | `struct_r2.json`, `struct_r2_out.txt` |
| `rend_check_r2.py` | Render (v4_gemein.zeichne / b2, Kamera 50A0 Cut 12, RDT-Licht, Posen 135/85): Kopfbox, eigene Marker-Renders (Index in der ganzen Kopfregion unbenutzt, Marker in den Zeilen 486 UND 488), eigene Teilmodelle (Augenfaecher T19/T20/T41, Zonen nach der CLUT-Zeile jeder Flaeche), K1k/K4/K5/K6/K7/S2/KS2/T-K9e, Messblatt-Fleck, Seiten, Ref2 | `rend_r2.json`, `rend_r2_out.txt`, `r2_kopf_135.png`, `r2_kopf_85.png` (Ref2 / Licht / roh / Marker), `r2_voll_*.png` |
| `nach_r2.py` | K6 ohne Maul, KV2/KV3/KV3r/KV5/KV1 nach Zonen, helle Flecken rechts, Farbwinkel der Zeilen 486/488 NACH der 5-Bit-Rundung | `nach_r2.json` |
| `bneg_r2.py`, `bneg2_r2.py` | Anteil b* < 0 je Zone, Ref2, G2-Quelltexel, Zeilen 486/488 | `bneg_r2.json`, `bneg2_r2.json` |
| `ctest_pruefer_r2.txt` | eigener ctest-Lauf der 4 Tests | - |

## 1. Koerper = Ref2, Schwanz/Arme = ot3 (Auftrag (1)) - GRUEN

| Mass | Soll | Ist (eigener Leser) |
|---|---|---|
| Meshes | 23 | Ref2 23 / neu 23 / ot3 23 |
| Koerper-Meshes 0-8, 10-15, alle 8 Felder (tv, tn, tf, tu, qv, qn, qf, qu) | bytegleich Ref2 | abweichend: **keine** (auch gegen ot3 keine) |
| Schwanz-Meshes 16-22, alle 8 Felder | bytegleich ot3 | abweichend: **keine** |
| Koerper-Texel (von Koerperflaechen abgedeckt, eigene Rasterung) | bytegleich Ref2 | 34431 abgedeckt, **0** abweichend; UV-Maske Ref2 = neu |
| Schwanz-Texel (Meshes 16-22) | bytegleich ot3 | 10356 abgedeckt, **0** abweichend; Zeile 480 |
| Sektionen 0-6 | bytegleich ot3 | alle 7 gleich (4716 / 5520 / 106056 / 4 / 4 / 400 / 7688); gegen Ref2 0, 1, 3-6 gleich, Sektion 2 (EMR, V4-Schwanzgang) wie seit Runde 1 |
| Texel geaendert gegen ot3 | nur alte Kopfflaechen + G2-/G3-Orte | 11685 geaendert, **0** ausserhalb der Orte x 0-125 / y 160-249 (TPage 128) und x 256-267 / y 200-235 (TPage 130); alte ot3-Kopfflaechen (3621 Texel) liegen ganz im G2-Ort; Ueberlapp der Orte mit Koerper-/Schwanzflaechen 0 / 0 |
| Directory | 9 Eintraege | 9; MD1 35420 B (ot3 36164), TIM 102944 B (ot3 101408, +1536 = 3 x 512), 36 Byte Rest |

## 2. Herkunft Kopf = RE2 (Auftrag (2)) - GRUEN

Quelle (eigener TOC-Leser): `re15_port/shared_assets/RE2/CDEMD0.EMS` sha256 `8724200c1ed5e8d3235ceb85640893467eb9878c2a016529aa84b49a79916fca`.
kind 0x31: EMD Sektor 2944, 193124 B, sha256 `bb966cd5...`; TIM Sektor 2895, 99872 B, sha256 `e0929cef...`. kind 0x33: EMD Sektor 3114, 204968 B,
sha256 `c9ab4129...`; TIM Sektor 3065, 99872 B, sha256 `99b793e2...`. Alle = Protokoll des Bauers. G2-EMD: 8 Directory-Eintraege, MD1 18 Meshes;
Mesh 3: 48 Ecken / 48 Normalen / 56 Dreiecke / 14 Vierecke, UV CLUT 480 / TPage 128. G2-/G3-TIM 384 x 256 px 8 bpp, CLUT 256 x 3 bei (0, 480).

| Mass | Soll | Ist |
|---|---|---|
| Mesh 9 Zaehler (Header tvc/tnc/tfc/qvc/qnc/qfc) | 48 / 48 / 56 / 48 / 48 / 14 | **gleich**; qv teilt den tv-Offset, qn den tn-Offset (wie G2); ot3 hatte 61 / 73 / 18 |
| Flaechen-Indizes tf, qf | = G2 Mesh 3 | **gleich** (beide Tabellen bytegleich) |
| Aehnlichkeitstransformation (eigene Umeyama-Schaetzung Y = s R X + t) | Bauer: s 1,50, Rz(+10) Ry(-34) Rx(0), t (140, +10, 0) | **s 1,4998; Rz(10,00 Grad) Ry(-33,99 Grad); R[2,1] -0,00012 (kein Rollen); t (140,02 / 9,94 / 0,01)**; max \|R - Rz(10)Ry(-34)\| 0,00022 |
| Residuum Ecken | <= 1 E. (Rundung) | geschaetzt: **max 0,69 E., RMS 0,47 E.**; mit den Bauer-Parametern exakt: max 0,67, RMS 0,48 |
| Normalen | gedreht, Laenge erhalten | Residuum gegen R . N_G2 max 1,84 (von 4096); Laenge p50 4096 -> 4096,1 |
| Bbox neu | = Protokoll | (-204, -489, -303) .. (532, 210, 263) E. = Protokoll `g2.bbox_neu` gerundet; Geometrie unveraendert gegen Runde 1 |
| UV der 70 Flaechen | 59 = Quelle + (0, +160) auf TPage 128; 11 Maul auf TPage 130 | **59 / 11, 0 UV-Abweichungen, 0 Page-Abweichungen**; Maul = Dreiecke 5, 22, 25, 26, 28, 29, 34, 35, 42, 43, 44, UV x 0-11 / y 200-235 |
| CLUT-Zeile je Flaeche nach EIGENER Nachrechnung der Zonenregel (Flaechenmittelpunkt c im unskalierten G2-Raum: n . c > -60,1 mit n = (0,2543, 0,5728, 0,7792), \|n\| 1,000; oder u-Mittel 41-60; oder Q0 -> 488; Maul -> 487; sonst 486) | 0 Abweichungen | **grau 33 / dunkel 26 / Maul 11, 0 Abweichungen**; Listen dunkel_T/Q und grau_T/Q = Protokoll |
| von Mesh-9-Flaechen abgedeckte Texel | nur in den G2-/G3-Orten | 5415 Texel, **0** ausserhalb |
| G2-Texel (x 0-125, y 160-249 = G2 u 0-125, v 0-89) | **bytegleich** (Runde 2) | 11340 Texel (6524 belegt), **0 abweichend** (Runde 1: 3821) |
| G3-Zahn-Kachel (x 256-267, y 200-235 = G3 u 2-13, v 5-40) | bytegleich | 432 Texel, **0 abweichend** |

## 3. Masse am eigenen Render (Auftrag (3)) - GRUEN

Render: `v4_gemein.zeichne` (b2, BSS 50A0 Cut 12, RDT-Licht), Posen `posen_v4.json` "135" (vorn) und "85" (seitlich).
H_port = Sohle - Oberkante der Koerper-Ids ohne 16-22: Pose 135 **H 364** (OK 145, Sohle 509), Pose 85 **H 264** (OK 170, Sohle 434). Kopf-Id 9: 2461 / 1066 px.
Augen-Kern = Komponente der hellen G2-Texel (Quellpalette L* > 45, 8er-Nachbarschaft) bei u 18-22 / v 10-13 (13 Texel); Marker in den Zeilen 486 und 488
(der Kern liegt im Bild nur auf Zeile 486: nur_488 = 0 px). Marker-Index 7 (in der ganzen G2-Region + Kachel unbenutzt). Zahn-Marker = Kachel-Texel L* > 60,
Chroma < 20 (35 Texel). Augenfaecher = eigenes Teilmodell aus T19/T20/T41 (alle drei Zeile 486).

### 3.1 Auftragsmasse (alle gruen; Bauer-Riegel bestaetigt)

| Mass | Soll (Messblatt) | Ist (eigen) | Bauer |
|---|---|---|---|
| K10 Kopfbox vorn (Bbox Id 9, Pose 135) | 0,14-0,18 x 0,15-0,20 H | Box 325-384 x 160-221 -> **0,165 x 0,170 H** | 0,165 x 0,170 |
| K4 Augenlage vorn (Marker Augenkern im Faecher) | 0,035-0,065 H links; 0,07-0,12 H unter OK | 6 px (331-333 x 194-196), Schwerpunkt (332,0 / 195,2) -> **0,062 / 0,097 H** | 0,060 / 0,097 (19 px mit 1-Texel-Ring) |
| K7 Maul-Lage (Zahn-Marker) | Schwerpunkt x 0,3-0,7 / y 0,55-1,0 der Kopfbox | 55 px, Schwerpunkt (360,8 / 201,0) -> **0,596 / 0,662**; ganze Kachel 641 px bei 0,633 / 0,754 | 0,60 / 0,66 (58 px) |
| K7 Messblatt R3 (Videoraum, Fenstermittel) | \|KV3 - KV5\| <= 4; KV3 >= KV3r + 1 | KV3 29,1 / KV5 29,1 / KV3r 27,1 -> **0,0 / +2,0** | - |
| Zaehne sichtbar (Pose 135) | >= 20 Marker-px; >= 5 helle unbunte px unter 0,5 Kopfbox | **55 Marker-px**; hell-unbunt (L* > 45, C < 20) unten: Licht 78, roh (Texelfarbe) 172, Videoraum 56; Pose 85: 0 Marker (Seite) | 58 / 78 / 0 |
| S2 Augenlage seitlich (Marker, Pose 85) | 0,03-0,07 H hinter Vorderkante; 0,76-0,82 H ueber Sohle | 6 px (471-474 x 219-220), (472,5 / 219,3) -> **0,044 / 0,813 H** | 0,043 / 0,812 |
| KS2 Haarkante seitlich (Fenster 0,84-0,96 x 0,52-0,80, Pose 85, nur Kopf-px, Videoraum) | L* <= Kopf-Median (85, Videoraum 39,6) - 5 = 34,6 | Box 478-482 x 215-227, 44 von 65 px Kopf (68 %), alle 44 in der dunklen Zone 488; **Kopf L* 20,7** (p50 19,9; ganzes Fenster 26,7) | 20,7 / 26,7 |
| Bild-links: Auge | Auge links der Kopfmitte (354,5) | Kern x 332,0, Faecher x 335,5 -> **links (0,062 H)** | - |
| Bild-rechts: dunkle Seite | rechte Haelfte dunkler | Haelften L* Licht **28,4 / 20,9 (0,74)**, Videoraum 36,1 / 26,7 (0,74), Texelfarbe 39,4 / 27,3; dunkle Zone zu 69,6 % rechts der Mitte, graue Zone zu 99,1 % links | 28,4 / 20,9 |
| K1k Auge sichtbar (KI D3, Riegel des Bauers) | Faecher (Videoraum) <= Umgebung - 6; Glanzkern (Licht) >= Faecher + 8 | Faecher 104 px (0,044 x 0,071 H), **L* 18,2 gegen Umgebung 36,4 (-18,2)**; Licht: Faecher 10,6, Kern max 29,7 (**+19,1**); Lichtfaktor Faecher 0,25 | 18,2 / 36,4; 10,6 / 29,7 |
| K5 (Riegel des Bauers) | rechts <= 0,90 x links (Licht + Videoraum); KV2 <= Ref2-Kopf P50 39,4 - 8 = 31,4, a*, b* > 0 | **0,74 / 0,74**; KV2 **30,1 / 10,6 / 6,3** | gleich |
| K6 (Riegel des Bauers) | Spiegelbox des Faechers (Videoraum) <= Kopf-Median 29,2 | Box 365-380 x 185-210, **28,3** (ohne Maul 28,9); rechte Haelfte ohne Maul: 0 Flecken ueber Median + 15 | 28,3 |
| T-K9e (Riegel des Bauers) | Kopf L* P50 (Licht) >= 23; Kopf/Rumpf >= 0,9 | **23,8** (Videoraum 29,2); **1,02** (Rumpf P50 26,2); Ref2 30,8 / 1,28 | 23,8 / 1,02 |
| Zonen am Bild (Pose 135, px / L* Licht / Videoraum) | - | grau 861 / 39,4 / 45,4; dunkel 964 / 21,6 / 27,1; Maul 641 / 17,4 / 26,3; Deckung 100,2 % der Kopf-px | gleich |

### 3.2 Pose 85 Zusatz
Kopfbox 445-484 x 193-235 (0,152 x 0,163 H); Kopf L* Licht 37,0 / Videoraum 39,6; Haelften Licht 38,1 / 33,2; dunkle Zone 68 px (L* 9,8), Maul 9 px.

### 3.3 Messblatt-Methode K1/K3/K4 (hellster Kopfpunkt) - INFO, Methodenwechsel
Pose 135: hellstes 3x3 bei (332,7 / 170,0), L* 69,2, Kontrast 25,1, Komponente 41 px = 8 x 7 px = 0,022 x 0,019 H, Lab 64,3 / 12,0 / 5,4, **3 Komponenten >= 4 px**,
K4 danach **0,060 / 0,027 H** - das ist die graue Kuppe (Zeile 486) senkrecht UEBER dem Auge, nicht das Auge und nicht die Zaehne (Runde 1).
Pose 85: 1 Komponente, Kontrast 18,0, (473,4 / 200,6), S2 danach 0,040 / 0,884 H (ueber dem Auge). Nach dem Wortlaut des Messblatts (Auge = hellster
Fleck, aus dem Video) waeren K1 (1 Komponente), K3 (Hoehe 0,019 < 0,023) und K4-y (0,027 < 0,07) rot. Das gebaute Auge ist die KI-Lesung D3 (dunkle
Hoehle mit Glanzkern): im eigenen Render ist die Hoehle 18 L* dunkler als die Umgebung und der Kern 19 L* heller als die Hoehle (3.1, K1k) - das Auge ist
an der Messblatt-Lage (0,062 / 0,097 H) sichtbar. Der Lichtbeleg des Bauers (Faecher-Normale senkrecht zum RDT-Licht, Faktor 0,25) deckt sich mit meinem
Lichtfaktor 0,247. Ich werte die Lage als gruen, den Methodenwechsel als Nachtragspflicht im Messblatt (Bauer: Offen 1).

### 3.4 Abweichungen vom Messblatt-Wortlaut (Runde-3-Fassung Abschnitt 7), die der Bauer-Riegel nicht prueft - Entscheid Orchestrator

| Zeile | Messblatt R3 | Ist (eigen) | Bauer-Riegel |
|---|---|---|---|
| K5 Haarfeld | KV2 L* 0,62-0,90 x Kopf-Median (Videoraum) = 18,1-26,3 | KV2 ganz **30,1 (1,03 x)**; KV2 enthaelt 258 Maul-px (Zaehne, L* 34,4) und 374 dunkle-Zone-px; nur dunkle Zone **27,1 (0,93 x)** - 0,8 L* ueber dem Band | ersetzt durch <= Ref2 P50 - 8 (31,4) und Haelften <= 0,90 |
| K6 Spiegelstelle | Mittel <= Kopf-Median - 3 = 26,2; kein heller Fleck | Mittel **28,3** (ohne Maul 28,9); Box = 213 Maul-px + 193 dunkle px, 0 graue; max Videoraum ohne Maul 43,8 (Licht 30,9), Maul (Zaehne) 56,5; keine Komponente > Median + 15 ausserhalb des Mauls | ersetzt durch <= Kopf-Median |
| K9 Kuppe | a* >= 8 (R3; R2: >= 11), b* > 0, L* <= Median + 8 | KV1 **30,0 / 7,2 / 4,0** (a* knapp unter 8; nur graue px 5,9, nur dunkle 7,5); L* ok | INFO |
| T-K5 b* | Anteil b* < 0 im Kopf <= 0,02 | Licht Pose 135: b* < -1 **0,23**, < -3 0,17, < -5 0,018; Videoraum < -3 0,02; Ref2 0,00. Herkunft: G2-Quelltexel selbst b* < -1 zu 0,374 (p10 -3,9) - geerbt aus dem byte-gleichen RE2-Kopf, kein Mischfehler; graue Zone p50 b* 0,0 (neutrales Grau) | nicht enthalten |
| S3 Abstand | KS2-Fenster zum Augenfleck <= 0,01 H | Luecke Marker-Bbox 474 -> Fenster 478 = 3 px = **0,011 H** (wie Runde 1) | nicht enthalten |
| T-R1d Farbton | Bauer: Farbwinkel VOR 5-Bit-Rundung 0,00 Grad | bestaetigt (Zielwerte); NACH der Rundung (reale 15-Bit-Worte, Quell-Chroma > 10): Zeile 486 p50 2,5 / p90 10,3 / max 50,5 Grad bei Ziel-Chroma > 3 (texelgewichtet p90 12,4), Zeile 488 p90 14,7 - Folge der 5-Bit-Quantisierung bei Chroma ~10 (Schritt 8/255), kein Transformationsfehler | OK (vor Rundung) |

## 4. CLUT-Zeilen und Texturhoehe (Auftrag (4)) - GRUEN

TIM-Flags 9 (8 bpp mit CLUT); CLUT 256 x **9 Zeilen** bei (0, 480); Bild 192 Worte x 256 = 384 x 256 px. **Port-Texturhoehe 256 x 9 = 2304 <= 4096**; 9 <= 16 Zeilen.
Zeilen 480-485 bytegleich Ref2 (6 / 6). Zeile 487 = G3 Zeile 0 bytegleich. Zeilen 486 und 488: **eigene Nachrechnung der dokumentierten Transformation**
(RGB = 5 Bit x 8 -> CIE-Lab -> L* x k, <= lmax, Chroma x g -> Farbton-erhaltende Gamut-Abbildung nur ueber die Chroma (eigene Bisektion mit sRGB-Matrix) -> 5 Bit,
STP-Bit, Eintrag 0 bleibt 0): **486 = 256/256 Worte gleich (k 1,90, g 1,0, lmax 100), 488 = 256/256 gleich (k 1,00, g 0,50, lmax 30)**; beide ungleich der rohen
G2-Zeile; STP-Bits = Quelle; keine Null-Worte ausser Eintrag 0. Belegte Eintraege 187: Lab p50 Quelle 39,4 / 25,1 / 14,5 -> grau 74,3 / 4,6 / 1,2 (L* x 1,88 p50) ->
dunkel 29,6 / 12,8 / 7,5 (L* x 0,76 p50, max 31,4); Chroma p50 31,3 -> 9,7 / 15,1; 48,7 % der grauen Eintraege gamut-reduziert; Rundung dE76 grau max 5,6 (p90 3,8),
dunkel max 4,8. Zeilen je Mesh: Koerper 480/481/482/484, Schwanz 480, Kopf 486/487/488.

## 5. Spiel-Asset, Pin, Tests, Spiellauf (Auftrag (5)) - GRUEN

- `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` sha256 `cfd1ac42729e56365847db42b352337d7f0c06fdcc1ef3413d0aee32d4a97a95` = Inhalt `release/em30_hangar.sha256`;
  `git diff HEAD --stat -- release/ re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` leer.
- Eigener Lauf `ctest --test-dir C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/build -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form" --output-on-failure --timeout 900`:
  **"100% tests passed, 0 tests failed out of 4"**, Total Test time (real) = 76.62 sec, EXIT=0 (`zp_re2/ctest_pruefer_r2.txt`). Exe `re15_port/build/platform/pc/re15_pc.exe` unveraendert (Bau 2026-11-07 12:37).
- Spiellauf `scratchpad/sfv4/r/V4re2b/debug.log` (1008 Zeilen) gelesen: Zeile 2 `[asset] cd-root: .../sfv4/r/V4re2b/cd (+Fallbacks)`, Zeile 136 `[enemy] EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)`,
  Zeile 137 `[enemy] EM30 loaded: 23 meshes, 23 bones, 21 clips -> slot 12`; `grep -ciE "FAILED|error|fehler|assert"` = **0**; Overlay `sfv4/r/V4re2b/cd/EMD/EM30_HANGAR.EMD` sha256 `e9c66c2e...` = Pruefgegenstand;
  191 Bilder in `fd/`; `lauf_V4re2b.txt` EXIT=0.

## 6. Riegel-Tabelle des Bauers (riegel_re2kopf_V4re2.txt) gegen eigene Zahlen

T-K1a/b/c, T-K2s, T-R0, T-R1a (inkl. Zonenregel), T-R1b (0 von 11340), T-R1c, T-R1d (bytegleiche Nachrechnung), T-R4, M3: in allen Zahlen bestaetigt (Abschnitte 1, 2, 4).
T-R2 K10 / K4 / K7 / S2 / KS2 / K1k / K6 / K5 / T-K9e und T-R3: bestaetigt (3.1; Unterschiede nur aus der Marker-Maske: 6 statt 19 px, gleiche Lage auf 0,002 H).
Eigene Tafeln `zp_re2/r2_kopf_135.png` / `r2_kopf_85.png` (Ref2 | Licht | roh | Marker: blau Faecher, gruen Kern, magenta Zaehne, gelb Messblatt-Fleck): grau-rosa Kuppe mit
Adern Bild-links, dunkle braun-graue Seite Bild-rechts, dunkle Augenhoehle mit Glanzpunkt, offenes Maul mit Zahnreihen; seitlich Hoehle mit weissem Glanzpunkt vor der dunklen Spitze.
T5 0,098 H bekannt rot seit Runde 5 (Geometrie, kein Grund).

## 7. Urteil

**BESTANDEN.** Auftraege (1)-(5) ohne roten Punkt: Koerper = Ref2 und Schwanz = ot3 bytegleich; Kopf = G2 Mesh 3 (s 1,4998, Rz 10,00 / Ry -33,99 Grad, kein Rollen,
t (140,0 / 9,9 / 0,0), Residuum max 0,69 E.), UV = Quelle + (0,160), Zonenregel 0 Abweichungen, Texel 11340/11340 und 432/432 bytegleich, Zeilen 486/488 bytegleich der
eigenen Nachrechnung; K10 0,165 x 0,170, K4 0,062 / 0,097, K7 0,60 / 0,66, S2 0,044 / 0,813, KS2 20,7 <= 34,6, 55 Zahn-px, Auge links, rechts dunkler (0,74);
9 CLUT-Zeilen / 2304; Asset + Pin unveraendert, 4/4 Tests, Spiellauf sauber.
Offen fuer den Orchestrator (3.3 / 3.4): Messblatt-Nachtrag fuer den RE2-Kopf (Auge = dunkle Hoehle mit Glanz statt heller Fleck; K5-Band 0,93 statt <= 0,90 in der
dunklen Zone; K6-Mittel 28,3 statt <= 26,2; K9 a* 7,2; b* < -1 bei 23 % der Kopfpixel, aus der G2-Quelle geerbt; S3-Abstand 0,011 H).
