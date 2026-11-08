# G2 Abnahme 9 - Boss-Alligator ROOM2090 mit RE2-KI, NB4 Runde 3 (Abnahme 8 M1)

Stand: Zweig r41/gator_re2, HEAD bae9fb1a0. Geprueft wird nur das Delta 77918c914..bae9fb1a0 (5 Commits: 93a57bcb5, 4fe772196, 6eafe8753, 76a6d2a52, bae9fb1a0).
Abnehmer unabhaengig vom Bauer: eigene Disassembly (eigenes Werkzeug), eigene Messkopie, eigene Laeufe.
Belege: C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/analysis/befunde_runde41/G2_belege/abn9/

## Urteil: BESTANDEN

Abnahme 8 M1 ist behoben. Der eingebaute Mechanismus ist "ein Treffer je Biss-Clip-Lauf". Das entspricht dem RE2-Mechanismus "ein Treffer je Kiefer-Lauf +0x224".
+0x238 wirkt im Port nur noch als Spurt-Sperre. Nach dem Intro beisst der Gator im ersten Lauf mit dem Maul an der Figur (F290 statt F416).
Die Laeufe oben_leon, tod_leon und tod_elza sind in state.log und climb.log bitgleich zu Abnahme 8. Damit sind (T), (W), (K) und der Gang unveraendert.
Es gibt keinen blockierenden Befund.

## 1. Diff 77918c914..HEAD -- re15_port

Geaenderte Dateien (283 Zeilen hinzu, 86 entfernt):
- engine/src/enemy_ai_boss_gator.c
- engine/src/gator_re2_r41.c (nur Kommentare)
- include/re15_boss_gator.h
- include/re15_gator_re2_r41.h (nur Kommentare)
- tests/integration/test_r41_re2gator.cmake
- tests/unit/probes/r41_re2gator.cmake
- tests/unit/test_r41_gatorfix.c
- tests/unit/test_r41_re2gator.c (nur Kommentare)

| Punkt | Soll | Ist | Beleg |
|---|---|---|---|
| Nur Bissmechanismus | Es aendern sich nur die Bissbedingungen von LUNGE, B2, Hochbiss und Rammen | `gb_biss_frei()` ist entfernt. Die Bedingungen lauten jetzt `!bite_done && bed4/bed5/bed3`. `gb_biss_abschluss` setzt fuer art >= 3 `bite_done=1`. `gb_lauf_beginn` setzt `bite_done=0` an genau den drei Stellen, an denen vorher `bite_done = 0` stand (Zeilen 2372, 2448, 2889). Beim Rammen ist nur `gb_biss_frei` entfernt. | git diff enemy_ai_boss_gator.c |
| bite_done-Querwirkung | Kein Pfad, der bite_done liest, erbt einen gesetzten Riegel | CROSS hat nur einen Einstieg (`gb_cross_begin`, Zeile 1426/1427, setzt `bite_done=0`). GUARD setzt beim Start 0 (Zeile 2152). Die Rueckzugs-Pruefung `ct>=cframes oder bite_done` (Zeile 2923) und das Rammen (Zeile 3072) liegen nur in CROSS. | grep bite_done |
| +0x238 | Spurt-Sperre unveraendert | `re15_gr2_biss_cd(...BISS_CD_BISS)` (70) und INTRO 110 sind unveraendert, im Diff von gator_re2_r41.c aendern sich nur Kommentare | git diff |
| (T)/(W)/(K)/Gang | Keine Codeaenderung | Kein Diff an Trefferreaktion, Sturz, Kanister oder Gang. Die Laeufe sind bitgleich zu Abnahme 8, siehe Abschnitt 4. | vergleich_abn8.txt |
| Messhaken | Kein Spielverhalten | `gb_lauf_bild/_ende`, `BISSLAUF` und `bisslauf_info` schreiben nur in die Mitschrift. `gb_sperre238_zu` ist ohne Pruefhaken 0. | Code |
| Weggefallene Pruefungen | Nur die Runde-2-Pruefung "+0x238 >= 70 Abstand" entfaellt | An ihre Stelle treten (a) "Zweitbiss je Lauf 0", (b) Intro-Lage und (c) "Leerlauf 0", jede mit roter Gegenprobe. Abbrueche sind nur in Sub 3 zulaessig (RE2 @0x801032f0). | git diff test_r41_gatorfix.c, harness_nb4_auszug.txt |

## 2. Eigene Disassembly

Quelle: RE2 EM23_OVL_0000.BIN, md5 0293e099bbeae989776264eb14c81bc9, Basis 0x80100000, capstone. Werkzeug: abn9/werkzeug/abn9_dis.py.

| Punkt | Soll (Bauer) | Ist (selbst gelesen) | Beleg |
|---|---|---|---|
| T1 0x801012fc | Kontakt, Bremse, PL+0x1D3, +0x224==0, scharf; kein +0x238 | `06fc4294 lhu v0,-0x3fa(v0)` @0x80101300. `10004230 andi 0x10 / 17004014 bnez` @0x8010131c-20. `cbfd4290 lbu v0,-0x235(v0) / 12004014 bnez` @0x8010132c-34. `24028280 lb v0,0x224(a0) / 0e004014 bnez` @0x8010133c-44. Zusaetzlich `+0x1C0 & 4` @0x8010134c-58 und Kanister `+0x226 in {0,7}` @0x80101360-70. `01000224 / 240282a0 sb v0,0x224(a0)` @0x80101378-7c. Kein Zugriff auf +0x238. | disasm/a9_t1.txt |
| Aufrufer T1 | @0x80100974, @0x80100c60, @0x80100d40, @0x8010125c | Bytes `bf04040c` an genau diesen 4 Stellen. @0x80100974 steht am Kopf von 0x80100964, also vor der +0x238-Abfrage @0x80100b5c. Die Tafel @0x80104640/44 zeigt auf 0x80100964 (Sub 1 und Sub 2). | a9_t1_aufrufer.txt, a9_verweise.txt |
| Toter Code 0x80100c54 | Kein j, kein jal, kein Datenwort | 0 Fundstellen fuer j, jal, Datenwort oder imm 0x0c54. Die Funktion davor endet mit `jr ra` @0x80100c4c. Die Datenwoerter in der Naehe zeigen nur auf 0x80100c20 und 0x80100cdc. | a9_verweise.txt, a9_sub12_toter_code.txt |
| Kiefer-Maschine | Tafel @0x80100004-18; Stufe 2/3 bis 4, Stufe 4/5 bis 3 mit Treffer-Ruf; Ende @0x80101558 | Tafel: Stufe 0 -> 0x80101564, 1 -> 0x801013c4, 2 -> 0x801013d4, 3 -> 0x80101418, 4 -> 0x80101488, 5 -> 0x801014f8. `04000224 / 3d006214` @0x80101468-6c. `03000224 / 1f006214` @0x801014d8-dc. `240200a2 sb zero,0x224(s0)` @0x80101558. `5e05040c jal 0x80101578` @0x8010155c ist die einzige Fundstelle. Aufruf der Maschine: `e204040c` @0x80100328. | a9_tafel.txt, a9_kiefer.txt, a9_verweise.txt |
| Treffer-Einstieg 0x80101578 | Kontakt; PL+0x1D3 != 0 -> kein Treffer; PL+4 = 0x205; +0x238 = 70 | `0000a294 / 49004010` @0x80101590-98. `cbfd4290 / 44004014` @0x801015a4-ac. `05020224` und `fcfb22ac` @0x801015b4-c4. `46000224 / 380282a0` @0x801016b0-b4. Die Funktion liest +0x238 nicht, sie schreibt nur. | a9_treffer.txt |
| Reaktion PL+0x1D3 | `|= 0x80` @0x801030f8-80103100; frei @0x801031d4 (Sub 2 Ende) und @0x801032f0 (Sub 3) | `80004234 ori 0x80` @0x801030f8, `d30122a2 sb` @0x80103100. Das ist Stufe 0 der Reaktion 0x8010307c (Tafel @0x80100074[0] = 0x801030bc). `d30120a2` @0x801031d4 (Tafel [4] = 0x801031c8) und @0x801032f0. | a9_reaktion.txt, a9_scan1d3.txt |
| +0x238-Leser | Nur @0x80100b5c, @0x80100bf4, @0x80100cb4 | Der Scan aller Lade- und Speicherbefehle mit Offset 0x238 findet als Leser @0x801004a8 (Runterzaehlen), @0x80100b5c, @0x80100bf4 und @0x80100cb4 (toter Code). Die Schreiber sind @0x801004b8, @0x8010069c, @0x80100920 und @0x801016b4. Die Leser @b5c und @bf4 entscheiden nur ueber `+4 = 0x201/0x101` (Sub 1 <-> Sub 2). | a9_scan238.txt, a9_sub12_toter_code.txt |
| Intro-Ende | Bremse frei @0x801008f8-918, dann +0x238 = 110 | `efff0324 addiu v1,zero,-0x11` @0x801008f8 .. `000082ac sw` @0x80100918, danach `6e000224 / 380202a2` @0x8010091c-20 | a9_intro.txt |
| B2 = Sub 3 | Nur T1 @0x80100d40; `sltiu 0x72` setzt nur +0x218 |= 0x10 | `bf04040c` @0x80100d40. `7200422c` @0x80100e8c, danach `10004234 ori 0x10 / 180222a6 sh 0x218` @0x80100ea0-a4 | a9_sub3.txt |

Alle Instruktionsbytes, die der Bauer im Code-Kommentar und in der Commit-Nachricht von 4fe772196 nennt, stimmen mit meiner Lesung ueberein. Die Commit-Nachricht enthaelt 14 @0x-Zitate.
Die Reihenfolge entspricht dem RE-Gate: Dossier und Disassembly in 93a57bcb5, der Code danach in 4fe772196.

Bewertung: In RE2 sperrt PL+0x1D3 jeden weiteren Treffer, solange die Reaktion laeuft. Die Treffer-Pruefung laeuft nur in Stufe 4/5, also 6 Bilder je Lauf. Daraus folgt ein Treffer je Kiefer-Lauf. Ein Feld bite_done gibt es in RE2 nicht.
Im Port uebernimmt der Lauf-Riegel bite_done diese Wirkung, weil das Port-Fenster (Bild 6..34) laenger ist als die Sub-2-Reaktion (20 Bilder). Das ist eine Abbildung des RE2-Mechanismus, keine neue Zahl.
Rammen ist eine PORT-WAHL. Der Treffer-Einstieg hat nur einen Aufrufer (@0x8010155c), RE2 trifft die Figur also nur ueber die Kiefer-Maschine. Fuer das Rammen bleibt der Lauf-Riegel "einmal je Ueberquerung" wie vor Runde 2.

## 3. Bau und ctest

| Punkt | Soll | Ist | Beleg |
|---|---|---|---|
| Bau | LOCAL-BUILD-OK | `=== LOCAL-BUILD-OK (build)`, ninja: no work to do. exe md5 5c2cd89f18863b88c0db2c8c58589bf3 (gleich wie beim Bauer) | messkopie_md5.txt |
| unit | 48/48 | `100% tests passed, 0 tests failed out of 48`, `Total Test time (real) =  92.69 sec`. nb4 und nb4_elza je `0 Fehler`: (a) 527 Laeufe, 523 Treffer, 0 Zweitbisse; (b) 2/2 Lagen treffen in Lauf 0 bei t7 mit +0x238 = 103; (c) 0 Leerlaeufe. Die Gegenproben 'Sperre alt' (27 Zweitbisse) und 'Sperre 238' (Erstbiss erst in Lauf 1 bzw. 2) sind rot. | ctest_unit.txt, harness_nb4_auszug.txt |
| exe (die 5 Riegel des Bauers) | 5/5 | `100% tests passed, 0 tests failed out of 5`, `Total Test time (real) = 1284.80 sec`. Einzeln: oben_kante_elza 526.06 s, frei 665.94 s, oben_kante 463.14 s, rampe_oben 368.26 s, re2gator 295.55 s (`r41_re2gator: ok`, bisslog ok, Finisher ab F397) | ctest_exe.txt |

## 4. Eigene exe-Laeufe

Messkopie: re15_pc_g2abn9.exe, md5 5c2cd89f18863b88c0db2c8c58589bf3. Werkzeug: abn9/werkzeug/lauf_a9.sh und welle_a9.sh.
Die Umgebung ist wortgleich zu den Abnahme-8-Laeufen, nur ohne FRAMEDUMP; bei nische ist EXIT_AT 1100 gesetzt.
Ausgewertet habe ich mit dem Testprogramm (bisslog und gtrlog, Kopie test_r41_gatorfix_g2abn9.exe) und mit den Abnahme-8-Auswertern a8.py und a8biss.py, die vom Bauer unabhaengig sind.
Es liefen nur eigene PIDs: 43759, 43760, 43764, 43765. 43759 (nische) habe ich nach dem Game Over selbst mit kill beendet, weil EXIT_AT ausserhalb von 2090 nicht greift. Beim Bauer war es ebenso (TIMEOUT).

| Lauf | Soll | Ist | Beleg |
|---|---|---|---|
| a9_nische (Leon) | Erster Biss nach dem Intro im ersten Lauf, kein Leer-Schnappen mit dem Maul an der Figur | INTRO ende, Kamera frei F280. Erster Lauf BISSLAUF F283-F327 mit 1 Treffer bei Bedingung 1: Biss **F290** (HP 60 -> 0, Sub 2, 20 Bilder). Zweiter Lauf F390-F398: Tod F397 (Abstand 107). bisslog: Lauf 0 ist der erste Lauf mit Bedingung und der erste Lauf mit Treffer, 0 Laeufe ohne Biss, 0 Zweitbisse -> ok. a8biss: 0 Laeufe ohne Treffer mit dem Maul an der Figur (Abnahme 8: 2 Laeufe / 37 Bilder). state.log ist bis F289 bitgleich zu a8_nische2 und weicht ab F290 ab (Zeile 541). | laeufe/a9_nische, vergleich_abn8.txt |
| a9_tod_leon | Ein Treffer je Anlauf, 0 Zweitbisse, kein Tod in 20 Bildern | Biss F942 (100 -> 40), Tod F1049, Abstand 107, Zweitbiss 0, gtrlog ok, bisslog ok | laeufe/a9_tod_leon |
| a9_tod_elza (PL04) | dito | Biss F942 (100 -> 40), Tod F1049, Abstand 107, Zweitbiss 0, gtrlog ok, bisslog ok | laeufe/a9_tod_elza |
| a9_oben_leon | Stoss ins Wasser unveraendert | 35 Reaktionen (Sub 2 2, Sub 3 33), Stuerze 1, Abweichungen 0, 34 Laeufe mit je 1 Treffer, Zweitbiss 0, Leerlauf 0. state.log (md5 ecbf7cc0...) und climb.log (md5 dca6cf69...) sind bitgleich zu a8_oben_leon. | laeufe/a9_oben_leon, vergleich_abn8.txt |
| tod_leon/tod_elza gegen Abnahme 8 | Unveraendert | state.log und climb.log sind bitgleich zu a8_tod_leon und a8_tod_elza | vergleich_abn8.txt |

Hinweis zum gtrlog des Laufs nische: Er meldet "VERFEHLT", weil der Lauf nach dem Game Over (ab etwa F672) in einer neuen Sitzung bis F15837 weiterlief. Dort entstanden 60 RE1.5-Niederwurf-Bilder ausserhalb von 2090.
Die Raum-2090-Auswertung von a8.py ueber F1..F672 zeigt dagegen 0 Niederwurf-Bilder, Sub 2 mit 20 Bildern und 0 Zweitbisse.
Der Riegel integration_r41_re2gator faehrt diesen Lauf mit EXIT_AT 640 und ist gruen. Das ist kein Befund am Bau.

## 5. Abnahme 8 M1

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| M1 +0x238 als Bisssperre | Der Biss fragt +0x238 nicht (T1 @0x801012fc), nach dem Intro ist der Biss frei (@0x801008f8-918) | Die Port-Bedingungen fragen +0x238 nicht mehr, nur noch mit dem Pruefhaken sperre238, der im Spiel nie gesetzt ist. Der erste Biss nach dem Intro kommt F290 im ersten Lauf (Abnahme 8: F416 nach 2 Leerlaeufen). | behoben |
| Zweitbiss (Abnahme 7) bleibt behoben | 0 Zweitbisse je Lauf | Harness 0 von 523 Treffern, exe 0 in allen Laeufen | erfuellt |
| Texte berichtigt | "Spurt-Sperre" statt "Biss-Sperre" | Erledigt in gator_re2_r41.c, re15_gator_re2_r41.h, test_r41_re2gator.c und probes/r41_re2gator.cmake | erfuellt |

## Offen (nicht blockierend, unveraendert durch dieses Delta)

- O1 Abstand zwischen zwei Biss-Laeufen: RE2 kann bei andauerndem Kontakt nach dem Reaktionsende einen neuen Kiefer-Lauf scharf machen (T1 verlangt nur PL+0x1D3 == 0 und +0x224 == 0). Der Port beginnt einen neuen LUNGE erst bei hit_stun == 0 (0x64, Stand vor NB4). Im Lauf nische liegen deshalb 107 Bilder zwischen Biss und Tod (F290 -> F397). Das gehoert nicht zum Delta und betrifft den Takt, nicht die M1-Frage. Es sollte in einer eigenen Runde gegen RE2 belegt werden.
- O2 Harness: 2 von 523 Treffern (CROSS-Biss, danach Rammen) brechen eine Sub-3-Reaktion im Liegen ab. Nach RE2 @0x801032f0 ist das zulaessig. Rammen bleibt PORT-WAHL.
- O3 Unveraendert aus Runde 2: A90 (O-T1) ohne Nutzer-Wortlaut, fuzz im Stand f942f56c0, Sturz-Animation (O-W1), Koerper-/Boss-Schub, B2 nur per env, PSX nicht gebaut. 15 der 20 exe-Riegel faehrt die Integration.
