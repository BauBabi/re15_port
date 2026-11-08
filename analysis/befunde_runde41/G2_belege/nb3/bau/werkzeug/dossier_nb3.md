
## G2-RE2 NB3 (Rampe oben, Plattform-Suedkante)
Stand 2026-11-07, Zweig r41/gator_re2 ab e92356e8e (Messblatt des Messers, G2_MESSBLATT_NB3.md). Rolle: Bauer (NB3, Runde 1 von 2). Laufordner scratchpad `g2nb3/`
(Bauer: `harness_neu*/`, `bau_runs/`, `final_runs/`, `final_runs2/`, nicht im Baum); Belege `G2_belege/nb3/bau/` (Harness-Ausgaben je Bauschritt, Werkzeuge, exe-Messung, Bildleisten).
Commits: __COMMITS__. Finale exe `re15_port/build/platform/pc/re15_pc.exe` md5 029efac644499f92ef0a5572ee3aa43d (nach Patch 9; alle exe-Belege `final_runs2/`, die exe vor Patch 9
5581916f nur noch fuer die re2gator-Laeufe der Gruppe 1), alter Stand = Kopie der
Abnahme-4-exe d872c8e6 (`g2nb3/exe_alt/`).

### Nutzerentscheid 2026-11-06 ~21:15 (woertlich, bindend) auf die vier Fragen der Abnahme 4
- F1 (Kopf-Kameragrenze auch ohne Kanister): "Zu F1, noe, wozu wenn es gut funktioniert?" -> keine Kameragrenze; der Riegel bleibt "mit Kanister nicht haeufiger als ohne".
- F2 (Sprung am Querungsbeginn): "Verstehe ich nicht..." -> bleibt wie im gelobten Stand (nicht angefasst; gb_cross_begin unveraendert).
- F3 (Takt der Rampen-Querung): "Verstehe ich auch nicht... ist damit gemeint, das er oefter schneller laeuft? Das kann er gerne machen. Er soll nur den Spieler oben auf
  der Rampe auch rammen koennen, das der Spieler oben nicht ausweichen kann. Alternativ kann er auch hoch beissen oder sowas, aber er darf halt oben nicht sicher sein."
  -> **Vorgabe A**: die Figur oben ist nicht sicher - Rammen oder Hochbiss, Takt frei.
- F4 (Haenger an der Plattform-Suedkante ohne Kanister): "Wenn er haengt muessen wir nachbessern denke ich. Er darf nicht zu lange haengen. Wenn es kurz ist, kann man mit
  dem tauchen abhilfe schaffen, aber das hilft natuerlich nicht bei der Platform." -> **Vorgabe B**: der Haenger wird in der Verfolgung selbst beseitigt (auch ohne
  Kanister), "nicht zu lange" in Zahlen (Messblatt B3: <= 450 Bilder ohne Fortschritt).
Alles andere ohne Kanister bleibt NB5 (Memory reai-v2-gelobter-stand-ist-referenz); der Pruefhaken `re15_gator_boss_nb3_alt_setzen(1)` stellt den Stand a661eb8aa her.

### Ursache (Messblatt NB3 Abschnitte 1.4 und 2.3, hier nachgemessen)
- **B, Haenger:** `gb_seg_hits_rect` (enemy_ai_boss_gator.c) meldet "Punkt innen -> Schnitt", wenn der ENDPUNKT der Strecke (die Figur) im um GB_KOERPER_M 1800 aufgeblasenen
  Block-Rechteck liegt - auch wenn sie im Wasser steht (Rampe x 50..9000 z -19900..-12700; Plattform x -3500..3650 z -21800..-10650). Der CHASE-Zweig "verbunden" lief
  dann in Ring-/Westumlauf-Ecke (zw 5/6), `gb_kanten_umweg` in die "Schattenlage" (zw 11): Pendel im Kasten (3156..4879, -24085..-21690), Kopf 6830-10057 von der Figur.
  Harness `nb3_haengt` (Stand a661eb8aa, Gegenprobe im Riegel nb3): W4200/-18800/c2 **3 Haenge-Fenster** in 3600 Bildern; exe (Messer-Laeufe, `haenglog`): rampe_leon 1
  Fenster F2649, mit Kanister 2 Fenster + 2 x RUECKWAERTS, rampe_elza 1, wand_elza 1 (F4928).
- **A, Rampe oben:** ohne Kanister quert er nur beim Seitenwechsel (Leons naehere Kante, gb_seite_latch) und belagert danach diese Kante; der Hochbiss reicht von der
  Kantenlinie (Kante + GB_GPAT_M 1900, x >= 4450) 3900 weit: im Mittelband bei x < ~3400 nie. Harness `nb3_oben` (a661eb8aa): R2318/-16300 und R2800/-16300 **kein
  Treffer in 3600 Bildern**, R3350/-16300 erster Treffer t1236; mit Kanister in der Suedreihe Pausen 697-959 Bilder (Belagerung der fernen Kante nach der Querung).

### Was gebaut wurde (PORT-WAHL nach dem Nutzer-Wortlaut; RE2 kennt im Alligator-Raum keine erhoehte Flaeche, Messblatt Abschnitt 3; Code enemy_ai_boss_gator.c)
**Vorgabe B (Verfolgung vor der Kante):**
1. `gb_punkt_im_band(x,z)`: Punkt im Koerper-Band (GB_KOERPER_M 1800) vor einer Blockkante, nicht im Block. CHASE "verbunden": steht die Figur im Band und ist die
   Strecke gegen den Block SELBST (m = 0) frei -> Ziel = die Figur, ohne Vorhalte (Zweig 7); `gb_kanten_umweg(..., direkt)` laesst diesen Zweig ohne Umweg durch
   (die SCA-Klemme hit_radius_min 1466 haelt die Wurzel von der Kante). Sonst alles wie NB5 (Ring/Westumlauf, Dijkstra mit 1800).
2. `gb_wand_dazwischen`: steht die Figur im Band und naeher an der Wurzel als der Maulpunkt (2600 voraus, k_gb_seg[0]), ist die Wand-Frage die Strecke Wurzel -> Figur
   gegen den Block (ein Biss DURCH den Block bleibt unmoeglich). Gemessen (erster Bau ohne diese Regel, `bau/harness/patch1_haengt_leon_ohne.txt`): er stand still an der
   Klemmlinie (W1000/-20700: Wurzel (1000,-21484), 784 vor der Figur, Maulpunkt hinter ihr im Block -> 104 s ohne Biss; W4073/W4200/W3000: 96 / 96 / 87 s) - ein Haenger,
   den das RW-Mass (Kopf > 4500) nicht sieht. Mit der Regel: GBP_LUNGE zieht rueckwaerts, bis das Maul vor der Figur liegt (dort seit dem Biss-Sweep 2026-09-11), Treffer
   bei Maul <= 1500 - W1000/-20700 337 Treffer, Pause max 456 Bilder.
3. Unerreichbare Figur = belagern wie oben (`gb_pl_unerreichbar` -> player_on_platform / oben -> GBP_GUARD, Kantenziel): (a) Ostnische (`gb_pl_nische`: x > GB_RAMP_X1 im
   Rampenband; die Figur kommt nur dort ueber x 7200 hinaus - exe wand_elza x 7668 bei z -17577..-14994): durch das Wasser unerreichbar (Wurzel klemmt an Ostwand
   7200 - 1466 und Suedkante, der Maulpunkt laege im Rampenblock); die Querung der Vorgabe A ueberstreicht sie (Bahn-x bis 6500, Maul <= 1500 / Koerperkreis 1050 + 450
   > 1168). (b) Figur IM Block-Rechteck auf Wasserhoehe (`gb_pl_im_block`): nur im Defektfall - exe wand_elza (NB3-Bau) F7920 Biss-Wegstoss aus der Taschen-Ecke
   (2318,-18568) in den Pier (1382,-16749) -> (1382,-17036) (fuer die Figur nicht begehbar: `test_r41_gatorfix begehbar 2091`, Raster "weggeschoben"; beim Klettern
   springen y und z im selben Bild, probe4 F82); dort pendelte NB5 an der SW-Ecke (2 Haenge-Fenster F8239/F8389, Kopf 9076-11182). In 7 Laeufen vor/nach NB3 sonst 0 Bilder.
**Vorgabe A (oben nicht sicher):**
4. Zaehler `oben_ohne` (gb_state_t): Bilder, die die Figur oben (Rampe/Plattform y < -900) oder unerreichbar steht, ohne Treffer (0 in gb_biss_abschluss und sobald
   sie nicht mehr oben ist). Ab **GB_OBEN_OHNE 300** (= der NB5-Wert der Pause nach einer Querung mit der Figur oben, cross_cd 300 "Leon OBEN: 10 s auf der Seite
   bleiben", GBP_CROSS) quert die Belagerung ueber die Figur hinweg (QUER-CROSS, Bahn-Ende folgt ihr, GBP_CROSS (b)) - fuer alle, mit und ohne Kanister (Vorschlag 4),
   ohne die Kamera-Bedingung der NB2 (F1 "noe"), auch mitten in der Pause nach der letzten Querung; waehrend der Pause (cross_cd) belagert er die EIGENE Kante statt des
   Westumlaufs zur fernen (Seite wie mit dem Kanister in NB2). Die alte Kanister-Regel (nur mit Kanister, nur von der Kamera weg) bleibt unter dem Pruefhaken.
5. `gb_quer_trifft(cx0, von_sued, pl)`: die Zaehler-Querung startet erst, wenn die geplante Bahn die Figur RAMMT - Nachbildung der Bahn von gb_cross_begin / GBP_CROSS
   Bild fuer Bild (Start-x geklemmt auf [4450..6500], Kante +- GB_BAHN_M, Ende-x = Figur-x geklemmt auf [3350..6500] bzw. im Rampen-Fenster (gb_seg_rect_span, Marge 300)
   auf [2550..6500], Dauer len/32 in [90..240], Blick = Snap auf (cx1,cz1), Treffer = gb_koerper_kontakt wie das Rammen; das letzte Bild zaehlt nicht, GBP_CROSS endet
   bei t >= cframes vor dem Rammen-Test). Gemessen (R2318/-16300, `bau/harness/patch4_spur2318_gator_boss.log` QUERPRUEF-Zeilen): vom Ost-Ende des Kanten-Pendels
   (x 5625) lief die Bahn 1757 seitlich vorbei (kein Treffer in 240 Bildern, erst die zweite Querung ab dem Anlauf rammte bei t603); eine Vorpruefung mit dem Maulpunkt
   <= 1500 ueber die vereinfachte Bahn hielt bei x 5230 einen Grenzfall (~1500, echte Bahn mit Ende-x 3350 vor dem Fenster und gb_maul_dist in 16er-Schritten verfehlt)
   fuer einen Treffer (t682). Mit der Bahn selbst: Start bei x 5211 (Vorpruefung t 129), Rammen t450 an (3781,-16630).
6. Der Anlauf der Zaehler-Querung laeuft ohne das Kanten-Pendel (+-1500): mit ihm stand das Ziel bis 90 Bilder 1500 oestlich des Anlaufs (t300 Zaehler, t378 Start).
7. Rammen (GBP_CROSS, gb_koerper_kontakt, Schaden gb_biss_abschluss = DAT_8006f418[5] = 50 @0x8006f422 + Niederwurf) auch ohne Kanister, wenn die Figur OBEN steht
   (gb_pl_oben); eine Figur im Wasser wird in der Verfolgungs-Querung wie in NB5 nur vom Maul getroffen (CROSS-Biss) - mit Kanister wie in Abnahme 4 auch im Wasser.
8. (Patch 9, aus dem Querbezug unten) Der Zaehler `oben_ohne` laeuft nur in den Kampfphasen (`gb_phase_kampf`: CHASE / LUNGE / FLINCH / GUARD / CROSS_APPR /
   CROSS / RE2_DASH), nicht im RE2-Intro (GBP_RE2_INTRO, Durchbruch -> Schnapper, ~300 Bilder ab dem Betreten), in der Lauerstellung oder nach dem Tod - dort kann er
   die Figur oben nicht angreifen, "oben nicht sicher" meint die Belagerung. Gemessen am fremden exe-Riegel integration_r41_re2gator, Lauf "belagerung" (Figur ab F0 auf
   der Rampe (6300,-16250), Intro bis F305): mit dem Zaehler im Intro quert er im 71. Bild der Belagerung (QUERPRUEF F305 t 1, CROSS F376, Rammen F377) VOR dem
   ersten Hochbiss (alter Stand F417), die GIER-WACHE (300 Bildpaare CHASE/GUARD ohne Gier-Sprung) wird nie erreicht und der Finisher kommt F751 statt F627 (Riegel
   rot: "keine 300 Bildpaare CHASE/GUARD"). Nur im Kampf gezaehlt laeuft die Belagerung wie im Stand a661eb8aa (Hochbiss F417 / F522 / F627 nullt den Zaehler).
   Harness-Lagen (Gator ab t0 in CHASE/GUARD): SUMME-/ok-Zeilen bitgleich (`bau/riegel_nb3*.txt`); exe-Szenario Rampe oben: die erste Querung rueckt von F383
   (= F83 + 300, Zaehler lief im Intro) auf __P9_QUER__ (Zaehler ab dem Intro-Ende). Alle exe-Belege unten sind mit der exe nach Patch 9 neu gelaufen (`final_runs2/`).
**Abgrenzung (bitgleich NB5):** alle NB3-Zweige sind aus unter dem Pruefhaken nb3_alt UND unter v4_alt (Stand f942f56c0, Kanister als Koerper: dort lief die direkte
Anfahrt mit der Kanister-Huelle in die Kopf-Klemme und endete rueckwaerts an der Westwand - Riegel fuzz P-mitte 92 2-s-Fenster ab t673 bei (-7416,-15904)). Die Band-Regel
prueft die FIGUR, nicht das Steuerziel: der VORHALTE-Punkt (Figur vom Koerper geschoben, Vorhalte 20 Bilder) lag in der Westkanal-Lage (-6000,-16000) im Plattform-Band
und wich mit der Zielpruefung von NB5 ab (Riegel gang, erster Bau: 1944 von 2700 Bildern; `bau/harness/gang_spur_lage3_1000_-22500.txt`).
Keine neue Zahl ohne Quelle: 300 (NB5 cross_cd, PORT-WAHL "darf halt oben nicht sicher sein"), 1800 (GB_KOERPER_M), 2600 (Maulpunkt), 1500/1050+450/3350/2550/6500/
4450/90..240 (bestehende Querungs- und Kontaktwerte), x > 7200 (GB_RAMP_X1, Nische). RE2-Vorbild (Messblatt Abschnitt 3): Angriffsausloeser ist die Korridorlage
(@0x80100990-98 / @0x801009d8-e0), Treffer ohne Hoehen-/Abstandsbedingung im Overlay (@0x80101578 ff.), Rammen/Hochbiss sind PORT-WAHL.

### Bauschritte mit Messung (Harness, Leon = Elza bitgleich; `bau/harness/`)
| Schritt | Aenderung | A1 Rampe oben (18 Lagen, ohne Kanister) | B1 Wasser (11 Lagen) |
|---|---|---|---|
| a661eb8aa | - | 2 Lagen ohne Treffer, spaetester erster t1236, Pause max 1236 | Fenster 1-3 (W4073/W4200/W4428), Pause max 482 |
| Patch 1 (Band-Regel, Zaehler 300, Rammen) | B1, A4, A7 | 0 ohne Treffer, erster t603 (R2318/-16300), Pause 604 | Fenster 0, aber Pause bis 3116 (W1000/-20700: still an der Klemmlinie, kein Biss) |
| Patch 2 (Wand-Regel, Vorpruefung mit Maulpunkt) | B2, A5 vereinfacht | erster t682 (Grenzfall der Vorpruefung) | Fenster 0, Pause max 520, Treffer 287-350 |
| Patch 3/4 (Bahn-Nachbildung, Anlauf ohne Pendel) | A5/A6 | erster t450, Pause 537 | unveraendert |
| Patch 5 (Rammen nur Figur oben) | A7 | unveraendert | Rammen 0 (nur CROSS-Biss), Pause max 530 |
| Patch 6/7 (Band-Regel an der Figur; aus unter v4_alt) | Abgrenzung | unveraendert | unveraendert |
| Patch 8 (Figur im Block = belagern) | B3b | unveraendert | unveraendert (Harness-Lagen nie im Block) |
| Patch 9 (Zaehler oben_ohne nur in Kampfphasen) | A8 | unveraendert (Gator ab t0 im Kampf) | unveraendert |

### exe-Szenario Rampe oben (Riegel A2, rampe_oben / rampe_oben_elza) - warum die Eingabe des Messers nicht mehr traegt
Mit der Eingabe des Messers (Schuesse bei F180-213 je Raum, Klettern bei F636) kommt die Figur im neuen Stand nicht mehr hinauf: der Alligator steht mit Vorgabe B ab
~F100 an der Kante (erster Biss F122-125, `bau_runs/runs/ohne_oben_leon`: sub=9 0 Zeilen, Figur ab F180 an der Ostwand, danach durchs Becken geworfen). Das Skript
startet je Raum neu, und das Zielen (M) dreht die Figur im Rueckkehr-Besuch per Auto-Aim auf den anrueckenden Alligator (rot 3072 -> 1699 / 1637, Probelaeufe 1/2); die
ersten Bilder der Rueckkehr sind Szenen-Haltung (Cut 12, pst=1, Eingaben bis mindestens F9 verschluckt, Probelauf 3), und bei z -18800 greift der Klettertritt nicht
(Kletterzellen z >= -18000, `kletterkarte 2090`; Probelauf 1). Eingabe jetzt `W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W170` mit RUECK_LAGE z -18750:
Klettern F48-F82, Figur ab F83 oben, Gang auf die Rampe, Schuesse unveraendert bei F180 (Kanister-Szene im ersten Besuch wie bisher). Nach der ersten Querung
(Rammen F383) holt der Alligator im RE2-Zielmodus (+0x1C0 & 4 @0x80100a34-5c) den geworfenen Kanister aus dem Nordwesten (angehaengt F1013; beim Messer lag das
Anhaengen F625 VOR dem Klettern F636) - 856 Bilder Kanister-Szene, kein Haengen: `obenlog` zaehlt darum ab "KANISTER angehaengt F<n>" (wie traglog).

### Riegel Soll | Ist (Messblatt Abschnitt 4; Harness `test_r41_gatorfix nb3 / nb3_elza`, exe `obenlog` / `haenglog`)
| Riegel | Soll | Ist (Leon = Elza) | Beleg |
|---|---|---|---|
| A1 Harness Rampe oben, mit und ohne Kanister | je Lage erster Treffer <= 600 Bilder und laengste Pause ohne Treffer <= 600, Rammen >= 1 | 0 von 18 Lagen ohne Treffer; spaetester erster Treffer t450 (R2318/-16300); laengste Pause 537 B. = 17,9 s (R2318/-16300); Rammen 36; mit = ohne bitgleich | bau/harness/patch5_oben_*.txt, ctest nb3 |
| A2 exe Rampe oben (rampe_oben / rampe_oben_elza, mit und ohne) | ab dem Anhaengen: erster Treffer oben <= 600, keine Pause > 600 | __A2__ | bau/exe_final.txt |
| B1 Harness Suedkante, mit und ohne | Haenge-Fenster 0 in 11 Lagen | 0 (vor NB3 1-3) | bau/harness/patch5_haengt_*.txt |
| B1b (PORT-WAHL, Schranke wie A1) | laengste Pause ohne Treffer in den Wasser-Lagen <= 600 | 530 B. (W2318/-18568/c2), mit Kanister 520; Treffer je Lage >= 287 | dto. |
| B2 exe Suedkante (rampe, rampe_elza, abn2_wand_elza, mit und ohne) | Haenge-Fenster 0; mit Kanister 0 Zeilen HAENGT / RUECKWAERTS an / TAUCHEN an | __B2__ | bau/exe_final.txt |
| B3 ohne Fortschritt | <= 450 Bilder | Harness 327 B. = 10,9 s (W2318/-18568/c2, RE2-Zonen-Dash nach der Querung); exe __B3__ | dto. |
| B4 bitgleich ausserhalb | gang / v4 / re2 gruen | gang (Westkanal, NO-Ecke, SO-Ecke; alter Stand = rw_alt + nb3_alt) 0 von 2700 abweichend; v4 / re2 gruen | ctest |
| Gegenproben a661eb8aa (Pruefhaken) | rot ohne den Bau | R2318/-16300 kein Treffer in 3600 Bildern; W4200/-18800/c2 3 Haenge-Fenster; Gang-Lagen (6732,0,-13687) 579/900 und (4428,-1800,-16300) 29/900 Bilder abweichend | ctest nb3 |

### Bildleisten (RE15_FRAMEDUMP-Serien, F-genau; `G2_belege/nb3/bau/`)
__LEISTEN__

### Querbezug: exe-Riegel integration_r41_re2gator, Lauf "kanister" (fremder Riegel, Spuren alligator2 / gatorszene)
In der ctest-Gruppe der exe-Riegel (-j 2, `bau/ctest_exe_riegel_gruppe1.txt`) fiel `integration_r41_re2gator` im Lauf 4 "kanister" (Leon per Autopilot im Suedbecken
(5500,-25500) -> (5500,-20500) -> (-1000,-21000) -> ..., Kanister liegend bei (-1500,-23400); Schranke "Kanister als Kind der Part-6-Weltmatrix >= 200 Bilder bei
Abweichung 0" bis EXIT_AT 1500): "nur 30 angehaengte Bilder". Einzeln nachgefahren (`re2gator/ctest_einzeln1_rot.txt`): bitgleich - Zielmodus 1/0 F1129/1141,
1252/1261, 1327/1336, 1427, Stufe 1 F1435 (quer 1040), angehaengt F1447 -, also kein Last-Flackern, sondern eine verschobene Zeitachse durch Vorgabe B: der
Autopilot fuehrt Leon durch das Rampenband ((5600,-19698) F1115, 1598 vor der Kante z -18100), die Verfolgung faehrt ihn direkt an (Zweig 7; alter Stand:
Westumlauf Zweig 6 mit Ziel (-4300,-22600)), und die Bahn des Gators liegt danach anders zum liegenden Kanister: dreimal laengs < 8880 ohne das Aufnahme-Fenster
(quer > Koerper 1466 + 267; RE2 (b) @0x80100a74-b4 loescht das Zielbit), Aufnahme erst im vierten Vorbeilauf. Alter Stand a661eb8aa: erster Vorbeilauf F1215
(quer -478), angehaengt F1227, 270 Bilder - in drei Laeufen vom 2026-11-06 bitgleich (`re2gator/kanister_zeitachse_alt_neu.txt`). Die Aufnahme selbst
(RE2-Fenster, Part-6-Sitz, Abweichung 0) ist unveraendert, nur das Messfenster war fuer die neue Zeitachse zu kurz. Nachgezogen wie beim Querbezug Spur F (c)
desselben Riegels: RE15_EXIT_AT 1500 -> 1700 (+200 = Verschiebung 220 auf die naechsten 100 gerundet), Schranken unveraendert, Kopfkommentar (f) in
`re15_port/tests/integration/test_r41_re2gator.cmake`. Nachlauf 2 (`re2gator/ctest_einzeln2_fenster1700_rot_belagerung.txt`): Lauf kanister gruen (240 angehaengte Bilder, Abweichung
0), dann Lauf 5 "belagerung" rot ("keine 300 Bildpaare CHASE/GUARD ohne Sprung/Befreiung") - Ursache der Zaehler im RE2-Intro, behoben mit Patch 9 (Punkt 8 oben;
Belege `re2gator/belagerung_*`); danach Gruppe und Einzellauf gruen (Abschnitt Tests). Fuer den Riegel B4 "bitgleich ausserhalb" heisst das: die Figur im Band VOR
der Kante ist innerhalb der Aenderung (Vorgabe B), auch wenn sie nur durchlaeuft; die Belagerung bis zum ersten Treffer ist es nicht (Patch 9).

### Tests
__TESTS__

### Offen
__OFFEN__
