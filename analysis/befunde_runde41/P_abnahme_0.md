# Spur P "alligator2" - UNABHAENGIGE ABNAHME 0

Baum `.claude/worktrees/r41_alligator2`, Zweig r41/alligator2, Stand 1b88250cf, Basis f5a5e2f7e. Bau: `local_build.sh configure`, `local_build.sh build` -> `ninja: no work to do`.
Messordner (nicht im Repo): Scratchpad `P0/runs/<lauf>`, Werkzeuge `P0/tools/` (lauf.sh, kampf.sh, ke_natur.sh, bogen.py, zoom.py, anteil.py, spruenge.py, kletter_anteil.py), `P0/scan/` (p0_key.c, p0_sperre.c, p0_karte.c: eigene Sonden gegen die Engine-Bibliothek dieses Baums, nicht im Repo). Alle Laeufe: eigene exe-Kopie `re15_pc_p0a_<lauf>.exe`, Umgebung bereinigt, RE15_WINDOW_SCALE=1.

## Ergebnis
**NICHT BESTANDEN.** Die vier in der Aufgabe genannten Punkte sind erfuellt und gemessen. Die sechs Punkte von AUFTRAG.md Nachtrag 35 sind NICHT bearbeitet (Dossier: 0 Treffer "Nachtrag 35"), Punkt 1 davon ist laut AUFTRAG.md Pflicht fuer das Release und im Spiel kaputt (M1, M2).

## Massgeblich: AUFTRAG.md
Die Aufgabe nennt vier Punkte und sagt "AUFTRAG.md ist massgeblich". AUFTRAG.md Nachtrag 35 (2026-10-30 16:46, Commit 9af679a0a, IM ZWEIG dieser Spur): "AUFTRAG AN SPUR P, gilt fuer den laufenden Bau und die Abnahme" ... "Die Abnahme der Spur P prueft diese sechs Punkte mit; Punkt 1 ist Pflicht fuer das Release." Regel Abschlussplan v0.8.30: "Abnahmen fallen nur bei blockierenden Fehlern oder einem nicht erfuellten Nutzer-Punkt durch". `grep "Nachtrag 35|erreichbar|Putz|Finisher|Klettern" P_alligator2.md`: 0 Treffer.

## Urteile
| # | Punkt | Urteil |
|---|---|---|
| 1 | Nachtrag 31.2 Kampfbeginn vor Leon, RE2-Band, Riegel | erfuellt |
| 2 | FG-E7 Option (a), DISABLED weg, beide gruen | erfuellt |
| 3 | Lagespruenge mit dem Kanister im Maul | erfuellt (mit Hinweis H1) |
| 4 | Riegel Uebergabe Ankunftsszene -> Knall + 8 Zombies | erfuellt |
| N35.1 | Schluessel nach JEDEM Tod erreichbar, Riegel, echter Lauf bis zum Aufheben | **nicht_erfuellt (BLOCKER)** |
| N35.2 | Putz 1:1 wie RE2 | nicht_erfuellt (nicht bearbeitet) |
| N35.3 | Klettern auf den toten Alligator ausschliessen | nicht_erfuellt |
| N35.4 | Finisher: Spieler verschwindet wie RE2 | nicht_erfuellt |
| N35.5 | Leiche sitzt im Maul, folgt der Kopfbewegung | nicht_erfuellt |
| N35.6 | Kanister gerade im Maul, normale Bewegung | teilweise |

## Punkt 1 - Nachtrag 31.2
Messung: Laeufe kL (Leon), kE (Elza), Rezept des Riegels (`RE15_SET_FLAG=3:245 RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=650#2090 RE15_FRAMEDUMP=298-650/1:fd_`).
- debug.log beide: `[gatorszene] Kampfbeginn F299: Gator (3033,-21904) Gier 1843 Zone 0, Spielfigur (-8200,-25550) Zone 0, Abstand 11808`. Band 11430..13924 (SquareRoot0 11402..13890): im Band. Zonen 0/0 wie RE2; RE2-Zonengrenzen selbst gelesen: `80100990 lw v0,56(s0)`, `80100998 slti v0,v0,-17877`, `801009d8 lw v0,64(s0)`, `801009e0 slti v0,v0,-7322` (EM23_OVL_0000.BIN).
- state.log kL: F436 mo=4 = Biss +137, F443 hp 100 -> 50 = Treffer +144. Band 121..158: im Band. (`Kontakt F514 +215` ist die Laengsachsen-Zeile; das Riegel-Mass ist "Kontakt ODER Angriff" = +137.)
- Eigenes Mass anteil.py (numpy, nicht der Auswerter der Spur): rechte Bildhaelfte Median 26,9 %, Spitze 45,5 % (+95), > 50 %: 0 Bilder, ganzes Bild Median 16,5 %. Leon = Elza.
- Bildbogen selbst angesehen (`bogen_kL_a.png`, F301..F445): Kopf und Hals ab dem ersten Bild im Becken rechts vor Leon, Rumpf folgt, Biss an der Leiter. Kein Bild, in dem die Figur die Linse fuellt. Die Geometrie-Rechnung des Dossiers habe ich nachgerechnet (Blick (-0,918, 0,395), halber Winkel 37,6 Grad, Bildkante bei z -18100 rund 9870 von Leon): die Wurzel kann im Band nicht im Bild liegen.
- Riegel: unit_r41_alligator2_kamera(_elza), integration_r41_alligator2_kamera(_elza) gruen. Bilder P_belege/n31_nachher_leon.png, n31_nachher_elza.png vorhanden.
Urteil: **erfuellt.**

## Punkt 2 - FG-E7 Option (a)
- `git diff master -- re15_port/tests/unit/probes/r41_gatorszene.cmake`: Zeile DISABLED weg. integration_r41_gatorszene_tod_r1 Passed 129 s, _tod_r1_elza Passed 185 s (eigener ctest-Lauf).
- Lauf k3L (Rezept K3): `Schluessel-Kandidat: Kamera 0 ... Tiefe 2606 = Quadrat 14 Bildpunkte (Stufe 0, Naehe 3072)`, Kamera 1 weiter 6 Bildpunkte; `KOPF-KLEMME an F62`, `R1 ende (Leiche) pos=(3413,-23942)` wie Rezept FG-E7. Schluessel-Lage (0,-1723,-26800) Kamera 0.
- Werkzeug des Baus selbst gefahren: `Scan Raster 1000: 1688 Lagen, Stufe 0: 1595, 1: 48, 2: 33, 3: 12, 4: 0, unter 10,4: 0, groesster Abstand zur Leiche 5271` = Dossier.
- Code gelesen: Schleife ueber Stufen, Abbruch bei >= 104, Rueckfall beste Lage. Mass 10,4 = Riegel `_quad10 LESS 104`.
Urteil: **erfuellt** fuer den Wortlaut (Bildgroesse). Die Lagewahl prueft weiterhin nur Bild und Wasserzone, nicht das Aufheben (M1).

## Punkt 3 - Lagespruenge
Eigener Auswerter spruenge.py (state.log, Wurzel je Bildpaar von "KANISTER angehaengt" bis R1), natuerliche Laeufe, Rezept integration_r41_kanister2080_kampf:

| Lauf | Figur / Start | Aufnahme / R1 | Bildpaare | max | > 340 | > 400 | Weg |
|---|---|---|---|---|---|---|---|
| kl | Leon (-6000,-17500) | F143 / F445 | 302 | 340 | 0 | 0 | 18779 |
| ke | Elza dto. | F143 / F322 | 179 | 340 | 0 | 0 | 11680 |
| nl | Leon (0,-7000) | F160 / F199 | 39 | 306 | 0 | 0 | 1795 |
| we | Elza (-8000,-16000) | F142 / F199 | 57 | 340 | 0 | 0 | 5571 |
| pl | Leon (0,-16000) | F112 / F199 | 87 | 340 | 0 | 0 | 2553 |
| tl | Leon, Schuesse ab F900 | F114 / F1792 | 1678 | 340 | 0 | 0 | 58145 |
| tw | Leon (-8000,-16000), dto. | F116 / F1300 | 1184 | 340 | 1 (Rundung) | 0 | 42078 |
| tn | Elza (0,-7000), dto. | F170 / F919 | 749 | 340 | 0 | 0 | 32169 |

Vorher laut Abnahme G 4: 1517 / 37 Bildpaare > 400. Werkzeug des Baus: `TRAGSCAN: 306 Laeufe (270 ohne Aufnahme), 275400 Bilder, Sprung max 340 ... > 340: 0, Ueberstand max 2282, Bilder Ueberstand > 300: 1192` = Dossier. Kein Regions-Verwurf: `AUSSERHALB` 0 in allen acht Laeufen, Kanister Stufe 13 innerhalb (z.B. tl (-8900,-1918,-9351)). Der Alligator schwimmt mit dem Kanister weiter (Clips 0/1/2/3/4, Weg oben). Riegel unit_r41_alligator2_tragen, integration_r41_alligator2_tragen(_elza), unit_r41_kanister2080_kopfklemme, integration_r41_kanister2080_kampf gruen.
Bild `zoom_kl_slide.png` (klf F140-F154): Wurzel F143 (-5020,-18280) -> F149 (-3184,-18295) = 1836 in 6 Bildern bei gleichzeitiger Wende. Kein Bild-Sprung, aber ein schnelles Seitwaerts-Gleiten.
Urteil: **erfuellt.** Hinweis H1: die Klemme verteilt den Sprung auf 340 je Bild; in natuerlichen Laeufen ragt der Kopf dafuer bis 1640 ueber die Wand (`groesster Ueberstand`, ke), das Dossier nennt das (OFFEN 3).

## Punkt 4 - Riegel Uebergabe
Lauf ueL, Rezept des Riegels. debug.log: nur `[setflag] flag(3,245/0xf5) = 1`, keine Zeile 9,110. Reihenfolge `DOOR16` (Hinweg) -> `Folge A F46` -> `Folge C F299` -> `DOOR16` (Rueckweg) -> `[k2080] F331 Knall ...` -> `[k2080] F331 ROOM2080 nach Bit 110: 8 lebende Zombies`; 8 x `Sce_em_set type=0x16 behavior=0x02`, y -14400. Bild `bogen_ueL.png`: Zombies oben in 2080 sichtbar. integration_r41_alligator2_uebergabe Passed, _elza Passed.
Urteil: **erfuellt.**

## Nachtrag 35 Punkt 1 - Schluessel erreichbar: nicht_erfuellt (BLOCKER)
Kein Riegel ueber Todeslagen mit Aufheben, kein echter Lauf bis zum Aufheben im Diff; `unit_r41_alligator2_schluessel` misst nur die Bildgroesse.

**M1: Die Aktionstaste am Schluessel klettert auf den Schluessel.** gatorszene_r41.c key_prop: Flags GSZ_KEY_FLAGS 0x000B (Bit 0), Band 0, Box 0 (memset). climb_common.c climb_probe_object nimmt jedes Objekt mit `(flags & 0x101) != 0`; FUN_8002da4c trifft eine Null-Box mit Rand -112 fuer |dx| >= 112 UND |dz| >= 112 (selbst gelesen `8002da94 addu v0,v0,t1 / 8002da98 addu a1,a1,v0 / 8002da9c sll v0,v0,1 / 8002daa0 sltu v0,v0,a1`, `8002d358 andi v0,v0,0x101`, `8002d35c beq v0,zero`). Die Sonde laeuft vor dem AOT-Scan und verbraucht die Aktion.
- k3Lr (Leon), k3Er (Elza): Schluessel (0,-1723,-26800), Figur (-391,-26408) rot 15, Quadrat: climb.log `sub=9 kind=0x81 ph=0 obj=4` (obj 4 = Water Key), Figur landet bei (757,-1800,-26408) band=1, 1148 weiter, 1800 hoch, ausserhalb der Zone. Bild `bogen_k3Er.png`: Elza steht auf Hoehe 1800 in der Luft, im Bild auf dem Kadaver.
- k3Lq: dasselbe nach Westen, Figur landet bei (-1539,-26408).
- Gegenprobe k3La/k3Lb (Leon), k3Ea (Elza): Figur (-190,-26088) rot 1071: Sonde (-190,-26888), |dz| = 88 < 112 -> kein Klettern, itemband.log `ITEM slot 15 ... -> FREI`, Bild `bogen_k3Lb.png` "Will you take the Water Key?".
- Anteil (kletter_anteil.py, genaue Rechnung): 64,0 % aller Aufheb-Stellungen klettern; ueber 1688 echte Lagen mit der Wand (p0_key): 56,1 %.

**M2: Der R1-Kadaver sperrt den Westkanal.** Nach dem R1 bleibt `g->phase == GBP_RE2_TOD` (kein Uebergang nach GBP_DEAD, Lauf ken3 F640 `st=1 ss1=3 mo=9 af=36`), der Koerperlaengen-Push prueft nur `phase != GBP_DIE && != GBP_DEAD`. Fuenf Kreise bis 5750 voraus / 6000 zurueck sperren den Kanal (x -8432..-2168), der laut Karte `p0_karte` die einzige Verbindung Nord <-> Sued auf Band 0 ist (x > 1850 bei z -18500..-14500 geschlossen).
- ken2/ken3/ken5 (Elza, natuerlicher Tod Rezept kampf_elza, Leiche (-5676,-19980) Gier 2095, Schluessel (-8700,-23100)): Elza bleibt stehen bei (-7143,-19051), (-8432,-18453), (-2168,-19264). ken6: Plattform von dort nicht erkletterbar (0 x sub=9).
- kln (Leon, kampf_leon, Leiche (-5835,-19103), Schluessel (-5500,-23400)): stehen bei (-2168,-19677). Bild `bogen_ken3.png`.
- Flutfuellung mit den Kreisen (p0_sperre): ke, kl, we: Schluessel UND Leiter nicht erreichbar. 1688 Lagen: Schluessel abgeschnitten 352 (Spieler Suedbecken), 266 (Spieler Nordbecken), Leiter 296.
Ohne Schluessel kein Tor 2090/20A0, also Sackgasse im Spielweg. Beide Maengel bestehen unveraendert seit vor Spur P (der Diff beruehrt weder GSZ_KEY_FLAGS noch das Push-Gate), sind aber genau Nachtrag 35 Punkt 1.
Der vorhandene Aufnahme-Riegel `unit_r41_gatorszene_tod` (test_r41_gatorszene.c Z. 508 `pl->x = kx + 620; pl->z = kz; pl->rot_y = 2048`) faehrt genau die eine Stellung mit |dz| = 0 und kann M1 nicht sehen.
In allen 1688 Lagen ist die Zone ohne Kadaver mindestens per Vorwaertspunkt erreichbar (`unerreichbar 0`, 202 Lagen nur per Vorwaertspunkt); die Lagewahl selbst setzt den Schluessel also nicht hinter Waende. Ursachen sind M1 und M2.

## Nachtrag 35 Punkte 2-6
**Punkt 2 Putz: nicht_erfuellt.** `git diff master -- re15_port | grep -ci putz` = 0. Kein Beleg RE2 ROOM40A0 sub14 gegen die PORT-WAHL "Putz-Bursts um Leon" im Dossier, kein Bildbogen, kein Riegel. Am Bild nicht neu gemessen, weil keine Aenderung vorliegt: der Nutzer-Befund steht unveraendert.

**Punkt 3 Klettern: nicht_erfuellt.** Lauf k3Lk: drei Aktionstasten direkt am Kadaver (2209,-24834), (2129,-24715), (1922,-23271): 0 x sub=9, der Koerper selbst ist nicht erkletterbar. Geklettert wird aber neben ihm auf den Schluessel (M1): k3Er, Elza steht auf y -1800 sichtbar ueber dem Kadaver. Das passt zum Nutzer-Befund "drauf klettern". Beleg RE2 "keine Kletter-/Stufen-Hoehe" fehlt, Riegel fehlt.

**Punkt 4 Finisher: nicht_erfuellt.** Lauf finLc (Leon, kein Eingriff, `RE15_EXIT_AT=1108#2090`, FRESSEN F840..F1115): `zoom_fin.png`: Leons Beine ragen in F1012, F1020, F1072, F1080 aus dem Kopf des Alligators, bis zum Ende der Sequenz sichtbar. Diff: nur die vorhandene Bedingung `phase != GBP_FRESSEN`, kein Part/Flag/Bild aus RE2 @0x80100fbc-0x801010e4.

**Punkt 5 Leiche: nicht_erfuellt.** Lauf szL, `zoom_fressen.png` (F88..F240): ab F150 liegt die Leiche unter Hals und Rumpf, das offene Maul zeigt nach oben und ist leer (F165, F180, F200, F215, F240). Diff: 0 Treffer GSZ_KOPPEL_BILD / leiche_dran. Die Leiche wurde verlegt, der Sitz nicht angefasst.

**Punkt 6 Kanister: teilweise.** Bewegung: Punkt 3 oben (Sprung weg, Weg 18779..58145, Clips 0/1/2/3/4, Biss-Zyklen laufen). Sitz: Diff 0 Treffer KAN_LOKAL / 1750; Lage und Drehung wurden nicht gegen RE2 gemessen, kein Bildvergleich im Dossier. Bild `zoom_kan.png`, F120: Kanister als langer Balken seitlich aus dem Maul. Der Nutzer-Befund "schief" ist unbearbeitet.

## Gates
**RE-Gate.** Stichprobe selbst gelesen:
1. `8010246c addiu v1,zero,-500` / `80102470 sh v1,324(s1)` (EM23_OVL_0000.BIN): stimmt; +324 = r1_speed (Lese-/Schreibstellen @0x8010257c/@0x80102594). Port wirkt skaliert (`q12mul(-speed, scale)`, also ~333 je Bild), die Kommentarzeile "-500 je Bild" ist das Rohmass. Keine Auswirkung, Hinweis H2.
2. `80100998 slti v0,v0,-17877`, `801009e0 slti v0,v0,-7322`: stimmt.
3. STAGE2.BIN @0x80118b98 `e8 03 30 fd 00 00 98 08 d0 02 20 03` = {1000,-720,0,2200,720,800}: stimmt.
4. ROOM2090.RDT @0x060 `00 00 3c 68 c2 0a .. c4 f3 .. b2 99`: (2754,-3132,-26190), fov 26684 -> H 208; @0x09D6 pc[14..15] `ac f9` = -1620; @0x0996 `3b 00 02 31`, x -9400; ROOM2080.RDT @0x0BC2 `3b 01 02 31`: stimmt.
5. 340: `unit_r41_re2gator_lauer` gibt `Grenze 340 (authored Clip 6/3/0/1 340/72/104/259 x Skala)` aus: stimmt.
6. 0x60: vorhandene Port-Wenderate (enemy_ai_boss_gator.c Z. 1846, GIER-WACHE), so benannt.
Port-Werte ohne Adresse sind als NUTZER-VORGABE / PORT-WAHL mit Messung gekennzeichnet: GSZ_LEICHE_*, GSZ_GATOR_GIER_STD, GSZ_KEY_AUF_R*, GSZ_BILD_RAND, GSZ_SUCH_STUFEN, GSZ_SPIELER_*. Suche im Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME: 0 Treffer. getenv im Diff: 0; re15_gr2_kk_hart_setzen ist Pruefhaken ohne Spielweg. Der Fix erklaert den Befund der vier Punkte (Ursachen dort getrennt gemessen). **Haelt.**

**Vertrag.** `git diff master --name-only`: nur analysis/, re15_port/engine/src (3), include (2), tests, tools/r41_gatorszene. Kein release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine Test-CMakeLists. Keine neuen Bits, keine Nachrichten-Ids. Neue Tests: eigene Laufordner, exe-Kopie mit Praefix, `cmake_minimum_required`, libm nur `if(NOT WIN32)`. Hinweis H3: enemy_ai_boss_gator.c +21 Zeilen ist mehr als "1-5 Zeilen Haken", aber der Mechanismus liegt dort und ist markiert. **Haelt.**

**Tests.** Eigener Lauf `ctest -R <Regex Dossier> -j5 --timeout 1200`: **`99% tests passed, 1 tests failed out of 256`**, 1768 s. Rot nur `integration_r35_granate`, einzeln wiederholt, gleiche Zeile: `r35_granate [birkin]: Slot 2: Typ 30 grid 10 im Bild 399 (soll 36 / 13`, Teile wand/zombie/gator ok. Der Diff beruehrt weder G5 noch den Riegel, a48c4d4f0 (Birkin-Zweig, auf der Basis) schon. Den Lauf auf der Basis habe ich NICHT wiederholt. `r41_alligator2` 12/12 gruen. Nachgezogene Pruefungen in test_r41_gatorszene.c (d_min bis doppelt, k_cut != 0 entfaellt) sind begruendet; "E/K/X paarweise verschieden" haelt der exe-Riegel tod_explosion(_elza), gruen.

## Maengel
1. **M1 (BLOCKER, N35.1 + N35.3):** Aktionstaste am Water Key startet in 56..64 % der Stellungen das Klettern auf obj 4 statt des Aufhebens. Ursache key_prop: Flags 0x000B mit Bit 0, Band 0, Null-Box -> climb_probe_object / FUN_8002da4c. Nachpruefen: Rezept k3Lr, `RE15_CLIMB_TRACE=climb.log`, Zeile `sub=9 kind=0x81 obj=4`. Der Schluessel ist kein Kletterobjekt, das Aufheben muss aus jeder Stellung der Zone gehen.
2. **M2 (BLOCKER, N35.1):** Nach dem R1-Tod bleibt `phase == GBP_RE2_TOD`, der Koerperlaengen-Push bleibt aktiv, der Kadaver sperrt den Westkanal, die einzige Verbindung Nord <-> Sued auf Band 0. Natuerlich: kampf_leon, kampf_elza, west_elza; Schluessel und Leiter fuer die Figur nicht erreichbar. Nachpruefen: Lauf ken3/ken5/kln, Figur steht bei z ~-19000..-19700. RE2 belegen, was der Kadaver an Kollision hat.
3. **M3 (N35.1):** Kein Riegel ueber viele Todeslagen inkl. Wandlagen, kein echter Lauf bis zum Aufheben. Der Bestands-Riegel Z. 508 faehrt nur die Stellung ohne Klettern.
4. **M4 (N35.2):** Putz nicht bearbeitet.
5. **M5 (N35.4):** Finisher: Beine bis zum Ende sichtbar.
6. **M6 (N35.5):** Leiche sitzt nicht im Maul.
7. **M7 (N35.6):** Sitz des Kanisters nicht gemessen und nicht angeglichen.
8. **M8 (Dossier):** Nachtrag 35 fehlt ganz, die Fertigmeldung "alle vier Punkte" laesst den Pflichtpunkt fuer das Release unerwaehnt.

## Offen, kein Mangel
- H1: Seitwaerts-Gleiten bis 340 je Bild, Kopf bis 1640 ueber der Wand in natuerlichen Laeufen (Dossier OFFEN 3).
- H2, H3 wie oben.
- integration_r35_granate Teil birkin rot, vor dem Release klaeren.
- Schluessel in der Ankunft ~8 x 7, gemessen F6-F34 hellster 82..112 (keybild.py), Dossier OFFEN 5.
- Nicht geprueft: PSX-Bau, RE2-Original am Emulator, volle Suite.
