# Pruefung K5f Runde 1 - Zahlen-Pruefer (2026-11-09)

Geprueft: Commits 5c68d2782 und 1e56ad0e8 auf r41/form, Variante `k5f/K5f/`.
- sha256 nachgerechnet: TIM 64b8d8bd9539471d, BAHNSTEIG_FORM.EMD 9045b6dcceebf7d5, FORM.EMD e85ab12c024d75ee.
- Das Spiel-Asset ist unveraendert (shared_assets/RE2: EMD f277916d7d6518b5, TIM 31a35e66428bac1e; git status sauber).

Eigener Code liegt in `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f/pz/`:
- `pz_basis.py`: eigener TIM-Leser und eigene CIE-Lab-Formel (sRGB, D65). Die G4-TIM wird direkt aus CDEMD0.EMS @0x65e800 gelesen.
- Pruefschritte: `pz_palette.py`, `pz_rampe.py`, `pz_ader.py`, `pz_rang.py`, `pz_material.py`, `pz_kralle.py`, `pz_f1.py`, `pz_hals.py`, `pz_weiss.py`, `pz_mass.py`, `pz_spiel.py`, `pz_gd.py`, `pz_key.py`.
- Spiellaeufe: `laeufe.sh`, `laeufe2.sh`.

Aus dem Projekt genutzt: der Renderer (rl.weiss_id), form_emd.lese_emd (Flaechen/UV) und ruhe_mass.masse (nur fuer R1-R4).

Rohe Tafeln:
- `.../pz/hals_k5f_oben_k5e_f1_unten.png`: weisses Licht von unten; die 6 Halsflaechen gruen markiert.
- `.../pz/spiel_tafel_roh.png`: gdigrab; oben 5090 Griff K5e_f1 | K5f, unten 5080 K5e_f1 | K5f.
- `.../pz/weiss_familie.png`

## Ergebnis: NICHT BESTANDEN (4 rote Punkte)

| # | Punkt | Soll | Ist (eigene Messung) | Bewertung |
|---|---|---|---|---|
| 1 | Hals-Unterseite | keine roten Streifen, Haut wie der Rest | Rot ist weg (0 warm-rote Pixel). Die 6 Flaechen Mesh 22 qf 1/8/9/194/201/209 sind aber jetzt fast schwarz: Albedo L* 4,9 konstant, C* 0,1. Weisses Licht von unten, 6 Ansichten: L* 2,8-3,7 gegen Nachbarhaut 14,0-18,5; 96-98,5 % der Pixel liegen unter dem p10 der Nachbarn. K5e_f1 dort: L* 18-24 bei 0,16-0,34. | ROT: rote Streifen durch dunkle ersetzt |
| 2 | Lippe (Wunsch "durchweg grau/blau") | grau, kein Schwarz | Zeile 10 (79 Lippensaum- und 2 Maul-Band-Flaechen): 96,8 % der Texelzugriffe = 0x8000 (L* 0). Lippensaum gesamt: 26,1 % der Texel L* < 2. Weisses Licht: 34-44 % der Lippenpixel L* < 3 (K5e_f1: 0 %). Maul-Band: 100 % schwarz. | ROT |
| 3 | W5 Krallen = Hand | dE76 <= 6 | Median Kralle [20,5/0/0] gegen Hand [17,8/5,5/-3,8]: dE 7,21 (Mittel 18,6). Dazu 23,3 % der Krallentexel L* > 90, reines 0x7FFF (G4 0,2 %, K5e_f1 0 %). | ROT (bestaetigt den Bauer) |
| 4 | Helligkeit nur multiplikativ, Kontrast der Rampe erhalten | Verhaeltnis statt Versatz | Haut ja. Adern nein: Fit L' = a + b x L_G4 mit b 0,06-0,28 und a 2,7-21,4. Die Ader-Helligkeit der G4 ist damit flachgedrueckt. Die Adern sind 30,4 % der Zugriffe in Hautzeile 2 und 23-25 % in Gelenk/Fleisch. So im Messblatt 3.2 geplant (q/kappa K5e-Satz). | ROT (Regel) |

**Gemeinsame Ursache von 1 und 2 (gemessen):**
- Die Hals- und Lippenflaechen lesen G4-Mundfleisch-Eintraege (196/37/208/244 und weitere).
- In der Hautzeile fallen diese Eintraege in die Ader-Klasse (w > 0).
- Die Ader-Abbildung legt sie auf den Boden p10' bzw. 0:
  - Hautzeile 1: Ader L' = 2,7 + 0,06 L
  - Grauzeile 10: Ader L' = -1,2 + 0,03 L, also 0x8000
- Das Rot ist weg, die Flaechen sind aber nicht Haut geworden.

## Gelb (knapp oder offen)

- **Z4 knapp und abhaengig von der Lab-Formel:**
  - Eigene CIE-Lab: K5f P-C* 4,4798 gegen Band-Minimum Zombie 4,4810, also 0,0012 darunter (ausserhalb).
  - OpenCV-Lab (Messblatt-Konvention): 4,4515 gegen 4,4126 (innerhalb).
  - Z8 Q-C* 12,157 gegen 12,14 (Rand 0,02).
  - Die Unterkante setzt ein einziger liegender Zombie.
- **Weisses Licht C*:** K5f 7,68 liegt unter allen 7 Originalen (Minimum Tyrant 8,65). Das Messblatt fuehrt den Wert nur als Info.
- **Buntheit und Farbton gegen G4 (Haut-Eintraege, nutzungsgewichtet):**

  | Hautseite | C* K5f | C* G4 | Ton K5f | Ton G4 |
  |---|---|---|---|---|
  | 1 | 15,55 | 3,34 | -25,7 | -22,6 |
  | 2 | 14,40 | 2,46 | -27,4 | -7,2 |
  | 0 | 7,12 | 4,48 | -32,8 | -61,1 |

  - Die Buntheit der Haut ist 4,7- bis 5,9-mal die der G4-Haut, die G4-roten Adern sind unbunt (a* ca. 0).
  - Die Verteilung ist damit gegenueber Capcom umgedreht. Ob das zu mauve oder "Pokemon" wirkt, ist eine Frage fuer die Sicht und den Nutzer.
- **Schwanzspitze:** 8,8 % der Texel L* > 90 (Knie fast weiss).

## Gruen (eigene Zahlen)

### 1. Zielband im Spiel

**Verfahren:**
- 5080 Bild-1-Lage (Pose -20711,-15984,756,0,11,1), eigene gdigrab-Laeufe ueber form_spiel.sh mit RE15_RE2_ASSET_ROOT-Umleitung, eigene exe-Kopien, nur eigene PIDs.
- Eigene Schluessel-TIMs (CLUT != 0 -> 0x7FFF | STP).
- Die gdigrab-Bilder sind pixelgleich mit den Framedumps (Bild 440) des Bauers: 0 px Unterschied bei 4 Bildern.
- Determinismus Zombie normal gegen normal2: 0 px.
- Der K5f-Schluessellauf blieb in 5080 viermal schwarz. Ersatz ist der K5e_f1-Schluessel; Framedump K5f-key gegen K5e_f1-key = 0 px in allen 5 Bildern (gleiche Geometrie, UV und Pose).

**Band** = min..max der Originale (eigene Masse auf den Framedumps des Bauers: Zombie 1140, Gorilla 11C0, EM30 und Tyrant 50A0 Schnitt 4).

| Mass | Band | K5f | K5e_f1 | K5c |
|---|---|---|---|---|
| P L* | 5,57..18,83 | 8,95 | 9,43 | **2,74** |
| P L-cv | 0,487..1,438 | 0,753 | **0,413** | 0,822 |
| P lokal5_rel | 0,256..0,658 | 0,332 | **0,170** | 0,296 |
| P C* | 4,481..7,58 | 4,480 (knapp, s.o.) | **4,36** | **1,90** |
| P C-std | 2,415..7,46 | 3,44 | **1,79** | **2,26** |
| Q L-cv | 0,368..1,02 | 0,556 | **0,238** | 0,757 |
| Q lokal5_rel | 0,216..0,528 | 0,273 | **0,116** | 0,307 |
| Q C* | 12,14..15,04 | 12,16 | **11,53** | **8,43** |
| Q C-std | 6,538..8,92 | 7,82 | **3,99** | **9,06** |

- K5e_f1 faellt wie verlangt bei Kontrast und Buntheit heraus.

**Weisses Licht** (eigene Masse, 4 Winkel). Band = G4, EM30, G5, Gorilla, Tyrant, Hund, Zombie:

| Mass | Band | K5f | K5e_f1 |
|---|---|---|---|
| L-cv | 0,409..1,135 | 0,669 | **0,291** |
| lokal5_rel | 0,175..0,461 | 0,361 | **0,147** |
| C-std | 4,78..13,57 | 6,65 | **3,16** |
| L* | 7,38..35,0 | 20,6 | |

### 2. Ruhe

Weisses Licht, eigene Renders:
- K5f: S5 2,5 (Maximum je Winkel 3), F_fremd 0, Spanne 15,3, Block 4,5. Alle vier innerhalb (<= 5 / 7,4 / 76,8 / 16,3).
- K5c: 5,75 / 16,0 / 88,1 / 26,4, also ausserhalb.

### 3. Regeln

- **Texel:** bytegleich K5c und K5e_f1. G4-Seiten 0-2 = G4.
- **UV und Seite:** 3828/3828 = K5c.
- **Haut multiplikativ:** L'/L_G4 = 2,45-2,51 (cv 0,068-0,077); der Versatz streut dagegen mit cv 0,52-0,59. Gelenk 3,61-3,70, Fleisch 3,83, jeweils x rg bzw. x rf.
- **Rangfolge:** 0 Vertauschungen je Klasse (haut/hell/ader) in allen 9 Zeilen.
- **Adern dunkler und nicht roeter (L* / a* Ader gegen Haut):**

  | Hautzeile | L* Ader | L* Haut | a* Ader | a* Haut |
  |---|---|---|---|---|
  | 1 | 4,9 | 14,7 | 0 | 6,0 |
  | 2 | 15,3 | 31,5 | 0,4 | 14,0 |
  | 3 | 10,7 | 28,8 | 0 | 12,7 |

### 4. Wuensche

- **Gelenk** (Median): L* 36,0, C* 5,1, gegen Haut 22,9.
- **Schwanzspitze:** [49,8 / 17,9 / -2,6], Ton -8.
- **Buckel:** [29,8 / 17,1 / -2,6], gegen Mesh-10-Haut 22,9: +6,9.
- **Unterkiefer (Mesh-23-Haut):** grau, L* 20,5, C* 0, kein Leuchten.
- **F1-Rand:** Median 13,9 = Mesh-0-Haut 13,9; 19,2 % der Randtexel L* < 5; kein schwarzer Ring im Median.
- **Im Spiel (Tafel oben):** Die roten Streifen am Hals sind weg; die Stelle ist dunkel.
  - Der 5090-Vergleich per gdigrab ist nicht pixelgenau, weil das Feuer laeuft und gdigrab nicht bildsynchron ist.

### 5. Technik

- 12 Zeilen, Hoehe 3072.
- Geometrie, Knochen, Clips, Pool und roh sind gleich K5e_f1. Die EMD unterscheidet sich nur in 824 Bytes (CLUT-Worte).
- Zweiter Bau (kf_k5f.py, Kopie der par) ist in allen 4 Assets und form.json byte-gleich.
- local_build.sh configure+build: LOCAL-BUILD-OK.
- ctest -R "form|bahnsteig|birkin_form|r41_form" mit RE15_RE2_ASSET_ROOT=k5f/K5f: 11/11 bestanden (`.../pz/ctest.log`).

## Nicht selbst gemessen

- **G4 und G5 im Spiellicht:** nicht moeglich, sie sind nicht im Raum platzierbar. G5- und G1-Paare des Bauers haben 0 Bilder mit Figur. G4 und G5 stehen darum nur unter weissem Licht im Band.
- **Spielbilder der Originale:** Sie stammen aus den Framedump-Paaren des Bauers. Die Masse darauf sind eigener Code.
