# Abnahme 1 der Spur R (Bahnsteig-Birkin): NICHT BESTANDEN

Zwei unabhaengige Pruefer (Code, echter Spiellauf), jeder Befund einzeln gegengeprueft. Stand 4ee345aaa.

## Zusammenfassungen

Codeabnahme Bahnsteig-Form (Baum r41_bahnsteig, Stand 4ee345aaa; master enthaelt denselben Code plus Paket-Gate 9c169ecd4). Acht echte Fehler, davon ein Blocker und drei hohe. Der Blocker: ein Treffer im ersten Bild nach dem Modelltausch macht die Form dauerhaft unverwundbar; die Form kann den Spieler auch nicht toeten und Tuer 4 ist zu, der Endkampf steht dann still. Die Bewegungen weichen vom G4-Overlay ab, und das ist der Kern des Nutzerauftrags: a3 von anim_step wurde als Takt gelesen (Griff, Auftritt und Zucken laufen doppelt so schnell, Weichen halb so schnell), die Ueberblendrate fehlt (Pose beim Weichen 40 von 40 Ticks eingefroren), Treffer im Weichen und der Doppelschlag erzeugen Spruenge, das Rennen schiebt Leon rund 9900 Einheiten durch den Wagen. Im echten Lauf mit stehendem Spieler verbringt die Form 55 % der Ticks in diesem Weichen. Geprueft mit Sonden gegen libre15_engine.a des Baums, zwei echten Exe-Laeufen (Rennen wiederholt) und Disassembly von CDEMD0.EMS rec 1 sowie SLUS_007.48. Unit-Tests test_birkin_bahnsteig und test_birkin_szenen sind gruen, decken diese Faelle aber nicht ab. Ohne Befund: RE2-Dateien fehlen (sauberer Rueckfall auf EM30), Speicherlecks (s_em30 einmalig, emd von re15_enemy_reset freigegeben), TIM-Upload kopiert die Pixel, Clip-Grenzen aller benutzten Bilder (Clip 3 fc 60, Clip 4 fc 50, Clip 8 fc 30, Clip 9 fc 63), Tauschbild (G4 Clip 20 Bild 0 steht am Boden), Division durch 0, Reentrancy von re15_bf_modell_hook, Zustand ueber Raumwechsel, Tod, Laden und neues Spiel (re15_enemy_reset -> re15_bf_reset -> re15_bb_reset), Selbst-Tuer (kein Weg aus dem Kampfwagen, Aot_reset Slot 4 in sub04 @0x1334), Spielstand (re15_actor_t unveraendert), make_package auf master, Android (ganzer RE2-Baum, Gate nach Baum), asset_selftest (spaete Stages bewusst nicht in der Liste), keine Altverweise auf EM30_BAHNSTEIG.EMD im Code, Linux-Portabilitaet der Tests (kein setenv, m gelinkt, Schreibweise der Dateinamen stimmt), Messhaken RE15_BB_POSE und RE15_BIRKIN_CUT im Normalpfad stumm, RE15_BIRKIN_FORM und RE15_ENDKAMPF_NUR_G5 gueltig, ENEMSE-Latch vor dem G5 (g5_se(10) kommt vor dem ersten Tentakel-Ton). Ein PSX-Bau wurde nicht versucht (bekannte Luecke, kein neuer Befund). Nachgelesen, aber nicht als Fehler gemeldet, weil im Dossier als Abweichung dokumentiert: die Form klemmt die Spieler-HP auf 1 (Lauf r2: HP faellt bis 0, danach 1). Sonden und Belege: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/code/probe_form.c, dazu Protokolle und Bilder unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/laeufe/.

Abnahme "lauf" ist NICHT bestanden: drei Blocker. Im normalen Spiel erreicht man die Form mit der Pistole gar nicht, und mit zwei anderen Wegen wird sie unerreichbar oder unverwundbar. Exe im Baum r41_bahnsteig ist aktuell (ninja: no work to do). 30 Läufe im echten Fenster (gdigrab hwnd, kein AUTOSHOT, kein SOFTWARE_RENDER), Leon 5090 und Elza 5091, je re15 und re2. Die Abläufe sind deterministisch; die Form-KI ist vom Flavor unabhängig, re15 und re2 sowie Leon und Elza liefern bildgleiche Zustände. Jeder Befund ist mindestens einmal wiederholt. Hookfrei bis zum G5 kommt man nur, wenn man mit Flinte oder Granatwerfer feuert und während der Mutation rund 6 s das Feuer einstellt (f21_leon, f21_elza).

Geprüft und in Ordnung:
- Kampfbeginn, Modellwechsel (kein Hänger, 46 ms)
- Griff, Biss, Hieb und Rennen treffen wirklich; Griff-Reichweite passt zum Modell (Vorderkante 3510 bei Bild 8)
- Boden: tiefster Punkt in allen benutzten Clips exakt y=0
- Form bleibt im Raum, x -560..24491, keine Hänger
- Ausweichen und Vorbeilaufen möglich
- Rückstoss-Szene und G5-Übergang laufen im Normalfall (Beleg 06)
- Tür am Wagenende gesperrt mit Meldung
- Nach dem Tod lädt der Raum neu mit EM30, kein Form-Rest
- Elza-Griff sauber
- Ton: Bank 24 spielt. se 5, 7, 9 und 11 sind leer bzw. 56-Sample-Attrappen, der G4-Griff ruft im Overlay ohnehin kein ENEMSE
- Bild 2 passt im Wagen ordentlich

Hinweise:
- Der Spur-Abschnitt Bild 1 (Szene am Bahnsteig) existiert im Spiel nicht, das steht offen im Dossier.
- Id 9 ist die Handgranate, der Granatwerfer ist Id 15. Handgranaten überfliegen die Form auf Kampfdistanz (~2200), das ist kein Fehler der Form.
- Die Befunde 1 und 2 liegen in älterem Code, nicht im Diff dieser Spur, sperren aber die neue Form.

Belege: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/belege/ (Rohläufe je ein Ordner daneben). Keine Quelle geändert, keine Exe-Kopie und kein eigener Prozess übrig.

## Bestaetigte Maengel

### M1 [blocker] Treffer im ersten Bild nach dem Modelltausch macht die Form dauerhaft unverwundbar

- Datei: re15_port/engine/src/birkin_bahnsteig.c:461-469 (Eintritt), :477-478 (einziger Loeser des Riegels)
- Ablauf: Mutation Phase 2 tauscht die Bank im KI-Tick (re15_bf_mutation_fertig, enemy_ai_common.c:12314), Zustand 1 / Sub 1. Im naechsten Spielschritt laeuft der Waffenschuss vor re15_enemy_ai_run_all (game_step_common.c:1981 gegen Step-Ende). Der Applier setzt state=2 und hit_react|=1. Der erste re15_bb_tick nimmt den Zweig s_bb.slot != slot und ueberschreibt mit `if (e->state != 3) { e->state = 1; bb_setze(e, BB_AUFTRITT); }` den Trefferzustand. case 2 laeuft nie, birkin_hurt_cd bleibt 0, Bit 0 von hit_react wird nie geloescht. Ab da verwerfen re15_enemy_take_damage_at (@80012fb4) und der Waffen-Resolver (re15_damage.c:2747, :2092) jeden Treffer. Die Form stirbt nicht, also keine Rueckstoss-Szene und kein G5. Sie kann den Spieler auch nicht toeten (Klemme auf 1, bb_schlag) und Tuer 4 ist per Aot_reset zu (ROOM5090 sub04 @0x1334): der Kampf steht still bis zum Beenden. Fenster genau 1 Bild; wer waehrend der Mutation durchschiesst, kann es treffen.
- Beleg: Sonde a (Datei code/probe_form.c, zweimal gelaufen, gleiches Ergebnis), echter Bankwechsel ueber re15_bf_modell_hook, Treffer ueber re15_enemy_take_damage. Ohne Treffer: 600 Ticks, 16 Treffer angenommen, hp 150 -> Tod. Mit Treffer im Fenster: `nach erstem Formen-Tick: state=1 sub=0 motion=10 hit_react=01 hurt_cd=0`, dann `600 Ticks spaeter, 60 Treffer versucht: angenommen=0 hp 100 -> 100 hit_react=03 hurt_cd=0`.
- Vorschlag: Beim Eintritt den Trefferzustand nicht verwerfen: state 2 durchlaufen lassen (Aufritt setzen, case 2 trotzdem ausfuehren) oder mindestens birkin_hurt_cd = 9 setzen, wenn e->state == 2 oder hit_react & 1. Pin: Treffer zwischen Bankwechsel und erstem Tick, danach muss der naechste Treffer wieder HP abziehen.
- Gegenprobe: Ich konnte den Befund nicht widerlegen: Er tritt im echten Spiel auf, allein durch Eingabe. Die Laeufe nutzen die echte Exe mit Eingabeskript und Magnum (Id 7), ohne RE15_BIRKIN_HP_AT und ohne direkten Aufruf von re15_enemy_take_damage. Nur Raumsprung und Waffe kamen ueber die Debug-Schalter RE15_DEBUG_JUMP und RE15_GIVE.

Ablauf im Lauf:
- F759: Bankwechsel (st1/1/0/0, mo20).
- F760: Schuss im selben Schritt wie die Tastenflanke, HP 100 -> 50. Der erste Formen-Tick ueberschreibt Zustand 2 mit 1/0.
- Danach steht hit=41, hurt0. birkin_hurt_cd bleibt 0, Bit 0 faellt nie.
- Die 21 weiteren Schuesse bis F1900 aendern die HP nicht (hit=43), kein G5.

Gegenprobe: Der gleiche Schuss ein Bild spaeter (F761) verlaeuft normal: hurt9, Riegel frei bei F770, F792 HP 0, F854 Rueckstoss, F1004 G5. Das Fenster ist also genau dieses eine Bild.

Widerlegungsversuche, alle ohne Erfolg:
- Modelltausch: pc_enemy_modell_neu laedt synchron im KI-Tick, das Fenster ist also offen.
- Reihenfolge: re15_player_weapon_fire (game_step_common.c:1981) laeuft vor run_all (:2458).
- Andere Loeser des Riegels: Fuer die Form loescht nur birkin_bahnsteig.c:478 Bit 0, und das nur bei hurt_cd > 0. Der Pausenfilter gilt nur fuer 0x20/21/25/26. Beretta-riegel_loesen braucht einen Treffer desselben Stosses.
- Andere Waffen: Die Schleife in re15_re2_gl_apply prueft den Riegel bei !re2 (Zeile 4449). Der gelesene Teil von re2_gl_typ fuehrt 0x30 nicht; ohne Treffer in seinem switch gilt der !re2-Zweig. Ob ein default-Zweig das aendert, habe ich nicht gelesen. Auch Granaten, Flammen und Raketen sind nicht im Fenster gelaufen. Direkt gesehen habe ich nur den Raketenlauf des Pruefers: dort bleibt die Form trotz Beschuss bei 150 HP (anderer Einstieg, gleicher Riegel).
- Ausweg aus dem Raum: laut SCD-Dump von ROOM5090 schliesst sub04 Tuer 4 per Aot_reset @0x1334. Tuer 5 geht nur per Aot_on @0x12D6 in sub02 auf, ausgeloest von z5:0x1c @0x12A6, und das faellt erst mit dem G5-Tod (enemy_ai_boss_g5.c:1141).
- Spielertod: bb_schlag und der Griff setzen HP < 0 auf 1. Das steht im Code und im Raketen-Log des Pruefers; in meinem Lauf lebte der Spieler bis F1900, bis HP 0 lief ich ihn nicht.

Einschraenkung zur Haeufigkeit: Die Formulierung "wer durchschiesst" stimmt so nicht. Faellt die HP in der Mutation unter 100, startet diese neu, weil re15_bf_mutation_offen wahr bleibt (l1 F586/F587, Kontrolle F759-F760 und F821-F822). Das Fenster braucht also wenig Treffer in den letzten rund 172 Bildern und einen Schuss genau im Tauschbild. Der Fall ist selten, aber ein echter Softlock ohne Ausweg.

Nicht gelaufen: Elza (5091) und andere Waffen im Fenster; beide nutzen denselben Codepfad. Die Pistole allein kam im Log des Pruefers nicht bis zur Mutation.
- Gegenprobe-Beleg: Eigene Laeufe, Leon, ROOM5090, gleiches Skript bis F635, Magnum 7:200, kein HP-Haken. state.log bildgenau, mon.log mit Riegel und hurt_cd.

Fensterlauf F760, zweimal mit identischem Ergebnis (Schussbilder, HP-Verlauf, letzte mon-Zeilen):
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_fenster\l2\mon.log
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_fenster\l2b\mon.log
F759 st1/1/0/0 mo20 hit=40 hp100 hurt0
F760 st1/0/1/0 mo10 hit=41 hp50 hurt0
F791 hit=43 hp50 hurt0
F1880 st1/8/1/0 hit=43 hp50 hurt0 (Typ 0x30, kein G5)
Magazin-Abzug ab F740: 22 Schuesse (760, 791, 822, 886, ...), HP-Verlauf nur 100 -> 50.

Gleiches Ergebnis unter RE2-KI:
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_fenster\l5_re2\mon.log

Kontrolle ein Bild spaeter (F761):
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_fenster\l3_F761\mon.log
F761 st2/7/1/0 hit=41 hp50 hurt9
F770 hit=40
F792 hp0
F854 st3 (Rueckstoss)
F1004 Typ 0x36 hp600

Kontrolle ein Bild frueher (F759): Treffer senkt HP auf 50, Mutation startet neu (F760 hp150):
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_fenster\l4_F759\mon.log

Basislauf fuer Tauschbild und Schuss-Latenz (Flanke = Trefferbild):
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_fenster\l1\state.log

Skript des Fensterlaufs:
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_fenster\l2_skript.txt

Code: birkin_bahnsteig.c:461-469 und :477-478, re15_damage.c:2092, :2747, :2812, :2832, :3118, enemy_ai_common.c:12314, platform/pc/main.c:1403.
SCD ROOM5090: 0x01334 Aot_reset `46 04 01 31 01 00 ff ff 00 00`, 0x012A6 Ck z5:0x1c, 0x012D6 Aot_on 05.

### M2 [blocker] Pistole: Original-Birkin wird nach dem ersten Zucken unverwundbar, Mutation und Form bleiben unerreichbar

- Datei: re15_port/engine/src/enemy_ai_common.c:12513 (case 2 HURT, Ausgang Flinch); ROOM5090/5091
- Ablauf: Pistole (Id 3), Birkin normal beschossen, je Schuss neu gezielt. Der 10. Treffer löst Clip 7 aus (F776). Am Clip-Ende (F816) setzt der Port hit_react |= 1, birkin_hurt_cd ist da aber 0, also löscht niemand das Bit mehr. Der Schuss-Resolver wirft Birkin danach aus der Kandidatenschleife (hit=03). HP bleibt bei 155, die Mutation (HP<100) kommt nie, die Form erscheint nie. Pistolenschaden 15, HP beim Flinch mindestens 150: mit der Pistole gibt es keinen Weg vorbei. Gleich in allen vier Kombinationen Leon/Elza, re15/re2.
- Beleg: mon_pistole_riegel.log: F816 st1/1 hit=01 hurt0, F838 hit=03, hp155 bis Ende. f4_pistole: HP 155 von F810 bis F3300 bei 80 weiteren Schüssen (mg 188 -> 109). f3, f4, f12_real, f20_leon_re2, f20_elza_re2, f20_elza_re15 identisch. Disasm STAGE3, Schalter @0x8011a284: Phase 1 setzt im Flinch je Bild `ori v0,v0,0x1` @0x8011a39c, Phase 2 @0x8011a3a8 schreibt 0x101 und löscht `andi v0,v0,0xfe` @0x8011a3c0. Der Port macht am Ausgang das Gegenteil.
- Vorschlag: Bit in der Flinch-Phase je Tick setzen, beim Übergang nach HUB löschen (Phase 2 wie @0x8011a3a8-c4). Betrifft jeden 0x30-Kampf, also auch 3070 und 5080 mitprüfen.
- Gegenprobe: Der Befund ist echt, nur die Aussage "mit der Pistole gibt es keinen Weg vorbei" ist zu absolut. Widerlegen liess sich der Kern nicht.

Mechanismus: Der Port kehrt den Flinch-Ausgang des Originals um. Original STAGE3 und STAGE5 (+0x814): Phase 1 setzt Bit 0 je Tick, Phase 2 schreibt 0x101 und loescht es. Der Port setzt es nur einmal am Clip-Ende (enemy_ai_common.c:12513) und loescht es nie. Steht birkin_hurt_cd dort auf 0, bleibt das Bit fuer immer. Der Resolver macht daraus 03 und wirft Birkin aus der Kandidatenschleife (re15_damage.c:2747 und :2092). Einen anderen Freigeber gibt es vor der Mutation nicht: alle hit_react-Schreiber fuer Typ 0x30 durchgesehen, die generischen Freigeber greifen nicht fuer 0x30, birkin_bahnsteig.c:478 laeuft erst nach der Mutation.

Zwei Gegenproben gingen ins Leere:
- Dispatch: Waffe 3 haengt in beiden Overlays an genau diesem Flinch.
- Lauf: Der Riegel haengt auch bei anderem Takt, nicht nur im Pruefer-Skript. Auch Feuer gedrueckt halten sperrt, die natuerlichste Spielweise (Takt 22).

Korrekturen am Befund:
1. Die Laeufe f4, f12 und alle f20 haben Treffer auf denselben Bildern, weil das Skript gleich ist. Sie sind also EIN Messpunkt (T=31), der zufaellig genau auf der Grenze liegt, keine sechs unabhaengigen Belege. Neutral wird das durch meine anderen Takte.
2. Es gibt ein Fenster von 8 Bildern: Landet ein Treffer bei T=32..39 nach Flinch-Beginn, steht hurt_cd beim Clip-Ende noch ueber 0 und gibt das Bit frei. Dann kommt die Mutation, und die Form erscheint, wenn der Spieler 171 Bilder lang nicht schiesst. Das ist ein Zufallsausweg, kein Spielweg; im Original gibt es ihn nicht, weil Phase 1 das Bit in jedem Tick setzt.
3. Der Defekt ist nicht von Spur R. Die Zeile stammt aus 78d2b2dcc (2026-07-26), Spur R fasst den HURT-Pfad nicht an. Sie versperrt aber den Weg zur neuen Form.

Betroffen sind Waffen, die den 10. Treffer vor HP<100 erreichen: Pistole 15, Messer 7, Spalte 12 mit 8 (Tabelle @0x8006f150). Waffe 8 (40) und 7 (50) mutieren schon vorher; das passt dazu, dass die Form bisher sichtbar war. Einmal gesperrt, sperrt es laut Code jede Waffe; das ist nicht eigens gemessen.

Der Fix-Vorschlag des Pruefers passt: Bit in Phase 1 je Tick setzen, beim Uebergang nach HUB loeschen.

Abgrenzung: Auf dem Fensterweg zeigt Dauerfeuer eine eigene Mutationsschleife (HP 150->90, Neustart bei af136). Das ist ein separater Mechanismus (beim Pruefer mon_gl_mutationsschleife.log) und nicht Teil dieses Urteils.
- Gegenprobe-Beleg: Disasm, selbst erzeugt mit re15_disasm.py:
- STAGE3: `ori v0,v0,0x1` @0x8011a39c, `j 0x8011a3e0` @0x8011a3a0, `sw v0,4(a0)` (v0=0x101 aus @0x8011a2c4) @0x8011a3a8, `andi v0,v0,0xfe` @0x8011a3c0, Schalter @0x8011a284.
- STAGE5 (5090): `ori` @0x8011abb0, `sw` @0x8011abbc, `andi` @0x8011abd4, Schalter @0x8011aa98.
- Dispatch [+5][+6]: STAGE3 @0x8011ef44, STAGE5 @0x8011ff08; Waffe 3 -> 0x8011a284 bzw. 0x8011aa98.

Eigene Laeufe mit der Worktree-Exe, gleiche Eingaben wie f4_pistole, nur die Pause im Schuss-Zyklus geaendert. Leon, 5090, RE1.5-KI, Debug-Sprung. Clip 7 = 39 Bilder, der Treffer-Tick bewegt die Anim nicht, Ende mit Treffer bei T=40. Rot wurde nicht wiederholt, weil die Laeufe ueber Spielbilder deterministisch sind und t31 den Pruefer bildgenau bestaetigt.

Takt / T / Ergebnis:
- 22 (Feuer gehalten) / T=22: Flinch F681-720, Bit F721, 03 ab F725, HP 165 bis F1500, keine Mutation. SPERRT
- 31 / T=31: Treffer F776 und F807, 03 ab F838, HP 155 bis F1500 (identisch zum Pruefer). SPERRT
- 32 / T=32: Flinch F819, Mutation F979. FREI
- 35 / T=35: Flinch F820, Treffer F855, F860 hurt4 hit=01, F864 hit=00, Mutation F995. FREI
- 39 / T=39: Flinch F829, Mutation F1024. FREI
- 40 / T=40: Flinch F880, Bit F919, 03 ab F920, HP 165 bis F1900. SPERRT
- 45 / kein Treffer im Flinch: Flinch F895, Bit F934, 03 ab F940, HP 165 bis F2000. SPERRT

Takt 35, Feuer ab Mutationsstart aus: Mutation F996-F1167, debug.log `Bahnsteig-Form: 27 Meshes, 27 Knochen, 29 Clips -> Slot 12`, danach kaempft die Form bei HP 150.

Pruefer-Logs nachgerechnet: f3_pistole hat T=24, Treffer F700 und F724, HP 160 und sperrt ebenfalls. f4, f12 und f20_* haben identische Trefferbilder 400...807.

Git: `git log -S` auf Zeile 12513 -> 78d2b2dcc 2026-07-26; master traegt dieselbe Zeile (12513). Quellbaum nicht geaendert, keine Exe-Kopie aus meinen Laeufen im Baum (die 5 vorhandenen gehoeren zu anderen Laeufen und blieben unberuehrt).

Dateien:
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/riegel_gegen/halten/mon.log
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/riegel_gegen/t35/mon.log
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/riegel_gegen/t45/mon.log
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/riegel_gegen/stopp35/mon.log
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/riegel_gegen/takt.py

### M3 [blocker] Dauerfeuer während der Mutation: Clip 0x14 startet endlos neu, die Form erscheint nicht

- Datei: re15_port/engine/src/enemy_ai_common.c:12634-12639 mit re15_bf_mutation_offen (birkin_formen.c:52)
- Ablauf: Mutation läuft (Sub 6, HP 150), der Spieler schießt weiter. HP fällt unter 100. Das Original-Tor !(grid&0x10) wäre nach Phase 0 zu, die Port-Ergänzung `|| re15_bf_mutation_offen()` hält es offen, bis der Clip fertig ist. Also wird Sub 6 Phase 0 erneut erzwungen: HP wieder 150, Clip von vorn, Se(1)+Se(8) erneut. Solange man mit etwa 1 Schuss/s oder schneller weiterschießt, kommt die Form nie. Erst rund 6 s Feuerpause beendet die Mutation (f21_leon, f21_elza).
- Beleg: Hookfrei: f14_gl (Id 15, re15): 23 Neustarts F672..F2449, alle 81 Bilder. f14_elza (Id 15): 23 Neustarts bis F2450. f14_flinte (Id 8, re2): 10 Neustarts bis F2366. f6_form (Pistole, Mutation per RE15_BIRKIN_HP_AT gestartet): 16 Neustarts bis F2544. Bei keinem dieser Läufe erschien in debug.log eine Zeile 'Bahnsteig-Form'. Datei mon_gl_mutationsschleife.log.
- Vorschlag: Die erzwungene Mutation sperren, sobald Sub 6 läuft (z.B. Tor nur solange e->sub_state_1 != 6 und !(birkin_flags & 8)), oder re15_bf_mutation_offen bereits in Phase 0 von Sub 6 schließen. HP-Fall während der Mutation dann wie im Original über den Revive-Wächter.
- Gegenprobe: Der Befund ist echt. Ich konnte ihn weder im Code noch im eigenen Lauf widerlegen.

Code: Das Original-Tor @0x801164e8 (`andi v0,v0,0x10`, `bne v0,zero,0x80116694`) schließt, weil Phase 0 der Mutation @0x80118050-54 (`ori v0,v0,0x10`, `sb v0,9(v1)`) das Bit setzt. Der Port hat in enemy_ai_common.c:12634 zusätzlich `|| re15_bf_mutation_offen()`. Dieser Ausdruck bleibt wahr, bis s_bf_mutiert gesetzt ist, und das geschieht erst in Phase 2 am Clip-Ende (Zeile 12314). Fällt die HP während Sub 6 unter 100, greift also Zeile 12637 und erzwingt Sub 6 Phase 0 neu: HP 150, Clip 0x14 von vorn, Se(1)+Se(8). Treffer sperrt in Sub 6 nichts, die HP sinkt messbar. Der Unit-Test test_birkin_szenen.c schießt während der Mutation nicht und kann den Fehler deshalb nicht sehen. Master und Worktree sind in engine, platform/pc/main.c und include gleich.

Lauf: Vier eigene Läufe ohne Haken bestätigen den Ablauf bei Leon und Elza mit Flinte, Id 15 und Pistole. Die Form kam jedes Mal genau 171 Bilder nach dem letzten Neustart, also erst nach der Feuerpause.

Präzisierungen zum Befund:
- Die Bedingung ist nicht fest 1 Schuss/s, sondern mehr als 50 HP Netto-Schaden innerhalb von 171 Bildern (5,7 s). Flinte: 2 Treffer, Id 15: 1 Treffer, Pistole: 4 Treffer im gemessenen Lauf.
- Die Pistole schafft die Mutation auch ohne Haken, der Beleg f6_form braucht den Haken also nicht.
- Es ist kein harter Hänger. Eine Feuerpause von 5,7 s oder leere Munition beendet die Schleife. Die Einstufung blocker ist deshalb eher hoch, denn der Spieler kommt weiter, sobald er aufhört zu schießen. Solange er schießt, kommt die Form nie, und jeder Treffer im Sub 6 ist Munition ohne Fortschritt.
- Gegenprobe-Beleg: Alle Läufe ohne RE15_BIRKIN_HP_AT, ohne Speicher-Schreiben, RE15_AI_FLAVOR ungesetzt (Default). Die Skript-Pause beginnt erst nach dem letzten Schuss. Laufordner unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen_mut/ (state.log, debug.log).

- a_leon_flinte (5090, Id 8, 1 Schuss/1,6 s): Mutation ab F782, 13 Neustarts bis F2042 im Abstand 72/108 Bilder. Beispiel F2041 `st1/6/0 mo20 af106 hp70`, F2042 `st1/6/1 mo20 af1 hp150`. Letzte Taste F2083, Clip-Ende F2213 (F2042+171), debug.log `[enemy] Bahnsteig-Form` erst zwischen den Zeilen [walk] F2190 und F2220; laut state.log läuft ab F2215 Clip 10 (Auftritt). 28 Treffer in Sub 6.
- d_leon_flinte_wdh: Wiederholung, identisch.
- b_elza_gl (5091, Id 15): Mutation ab F504, 33 Neustarts bis F2088 im Abstand 38/68 Bilder, Clip-Ende F2258, Bahnsteig-Form erst nach [walk] F2250.
- c_leon_pistole (5090, Id 3, etwa 1,4 Schuss/s): Mutation ab F991, 6 Neustarts bis F1819, Abstand fest 138 Bilder (hp 150 -> 90), letzte Taste F1892, Clip-Ende F1991, erst dann Bahnsteig-Form.

Disasm STAGE3.BIN: 801164e8 `andi v0,v0,0x10`, 801164ec `bne v0,zero,0x80116694`, 8011667c `slti v0,v0,100`, 80116688 `sw v0,4(v1)` (0x601), 80118050 `ori v0,v0,0x10`, 80118054 `sb v0,9(v1)`, 80118094-98 HP = 0x96.

Skripte: gm.py und ana.py im selben Ordner. Mein erster Versuch mit nur 0,5 s Zielzeit gab keine Aussage, weil die Flinte in der Zeit nicht schussbereit wird (nur 2 Schüsse). Er liegt als x_a_leon_kurz und x_b_elza_kurz bei und zählt nicht als Widerlegung.

### M4 [blocker] Tödlicher Erst-Treffer: Form bleibt für den ganzen Kampf unverwundbar

- Datei: re15_port/engine/src/enemy_ai_common.c:12543-12546; re15_port/engine/src/birkin_bahnsteig.c:477
- Ablauf: Ein Treffer über den HP (Raketenwerfer Id 18 oder Handgranate Id 9, je 1000 Schaden gemessen) setzt state 3 und hit_react Bit 0. Der Zweig `re15_bf_mutation_offen` springt direkt nach Sub 6, ohne das Bit zu löschen und ohne birkin_hurt_cd zu armieren. re15_bb_tick löscht das Bit nur über diesen Zähler. Die Form kommt mit hit=01 und kann nicht mehr getroffen werden. Das Spiel lässt sich nicht beenden, die Tür ist gesperrt.
- Beleg: f22_rakete, hookfrei: F427 hp-700 hit=01, F429 Mutation, F601 Form. 10 weitere Raketen F856..F1663 aus 1962-8184 Abstand, HP bleibt 150, F2400 Ende, hit=01 durchgehend (mon_rakete_form_unverwundbar.log). f5b_werfer (Handgranate): F613 hp-700, hit=01 von F787 bis Laufende.
- Vorschlag: Im Zweig Tod -> Mutation hit_react Bit 0 löschen. Zusätzlich beim Eintritt in re15_bb_tick (s_bb.slot != slot) den Riegel zurücksetzen.
- Gegenprobe: Der Befund ist echt. Vier Widerlegungsversuche sind gescheitert, nur die Schwere ist einzuschränken.

Mechanismus: Der tödliche Treffer setzt state 3 und Bit 0. Der Zweig enemy_ai_common.c:12543-12546 springt nach Sub 6, ohne den Riegel zu lösen und ohne birkin_hurt_cd zu armieren. Der Revive-Zweig des Originals tut beides (+0x1de=9 @0x8011a458, +0x93&=0xfe @0x8011a54c-50). Die Wurzel löscht Bit 0 nur beim Zählerübergang auf 0 (@0x801163e4-400), und birkin_bahnsteig.c:477 macht es genauso.

Widerlegungsversuche:
1. Anderer Löscher: keiner. Die Löscher-Liste zeigt für 0x30 nur INIT und den Revive-Zweig mit Mutationsbit, beide greifen hier nicht. re15_re2_pause_filter_apply gilt nur für 0x20/21/25/26. 0x30 steht nicht in re2_gl_typ, also greifen alle Riegel-Gates (re15_damage.c:2747, :3118, :4449).
2. Raketen verfehlen: nein. Der Kontrolllauf ist bis F1157 bildgleich, nur Bit 0 wird dort per Speicher gelöscht. Dieselbe Rakete von F1412 trifft dann, und Rückstoß und G5 folgen. Der Riegel ist die alleinige Ursache.
3. Ausgang: keiner. sub04 macht Slot 4, die einzige Tür des Nordwagens, zur Meldung. Slot 5 öffnet erst sub02 nach Flag 5:0x1C, das der Fall hier nie setzt. Die Form schlägt den Spieler nicht tot (Klemme, im f22-Lauf PL hp1), also kein Game Over.
4. Normaler Durchlauf: Hier greift die Einschränkung. Im Zensus von 240 RDTs (164 Item_aot_set) gibt es nur SIG, Beretta, Redhawk, M870, M10, SPAS und MC51. Die Port-Ergänzungen fügen keine Waffe hinzu. Maximal 50 Schaden (Zeile @0x8006e0d0) bringen HP>=100 nie unter 0, der Zweig bleibt dort tot. Erreichbar ist er über das Item-Debug des Statusschirms, das laut Runde-34-Notizen der Weg des Nutzers zu den Granaten ist. Das habe ich so gefahren, ohne RE15_GIVE.

Also ein echter Softlock, „blocker“ aber nur für Läufe mit Debug-Waffen (Rakete, Granaten, ungeprüft laut Tabelle auch GL Säure/Brand), nicht für den Normaldurchlauf.

Nebenbei: mon_pistole_riegel.log zeigt ab F816 hit=01 mit hurt0 nach dem Flinch-Ende, also einen anderen Weg in der EM30-Phase. Der vorgeschlagene Fix deckt ihn nicht ab.
- Gegenprobe-Beleg: Eigene Läufe mit Exe md5 8aabe6ed, Standard-Flavor, ohne Debug-Haken für HP. Waffe über echtes Item-Debug (Statusschirm, SELECT, n x R1). Abgelegt in C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_riegel\ (Unterordner a1, a2, b1, g1; Treiber gp.py).

- a1 (Rakete Id 18): W12 geladen, Abzug F977. F983 st3/18/1/0 hit=01 hp-700 hurt0. F1157 Form hit=01 hurt=0 hp=150. 10 weitere Raketen F1412 bis F2219, F3000 hp=150, alle Zeilen ab Form hit=01.
- a2 (Wiederholung von a1): dieselben Schussbilder, F983 hp-700, Ende hp=150.
- b1 (Kontrolle): bis F1157 identisch, dann hit per Speicher auf 00. Abzug F1412, F1413 st3/18 hp-850, F1414 Rückstoß hp1, F1523 nd1, F1568 t36 G5 hp600 mit vier t37-Armen.
- g1 (Handgranate Id 9, W09): F1088 st3/9/1/0 hp-700 hit=01. F1262 Form hit=01 hurt=0. 13 Abzüge, F3000 hp=150, 62 von 62 Formzeilen hit=01.

Tür (a1, a2): Spieler per Speicher vor Slot 4 (24700,-23550, Blick +x). F2510 pad=8000 ergibt msg(a=1 id=1), kein DOOR FIRE slot=4.

ROOM5090-SCD (gp_scd5090.txt):
- sub04 @0x01334 `46 04 01 31 01 00 ff ff 00 00` = Aot_reset slot 4, sce 1 (MESSAGE).
- RDT-Meldung id 1: "I can't put my friends in danger. I have no choice but to stop that creature here!".
- Id 0 "THANK YOU FOR PLAYING !!!" und Aot_on 5 stehen nur in sub02.

Original (STAGE3.BIN): @0x8011a458 `a0c201de`, @0x8011a54c `304200fe`, @0x8011a550 `a0620093`. Wurzel @0x801163ec bne / @0x801163fc andi 0xfe / @0x80116400 sb +0x93.

Zensus: gp_items.py, Räume 240, Item_aot_set 164, keine Id 9/10/11/15-18, keine GL-Munition.

Keine Quelle geändert. Worktree-Status sauber, keine gpriegel-Exe im Bau-Ordner verblieben.

### M5 [hoch] a3 von anim_step als Takt missdeutet: Griff, Auftritt, Zucken doppelt, Weichen halb so schnell

- Datei: re15_port/engine/src/birkin_bahnsteig.c:281 (Weichen), :399-400 (Griff), :448-449 (Auftritt), :526-527 (Zucken); Dossier R_bahnsteig.md Tabelle 3.1
- Ablauf: Der Port liest a3 = 0x200 als zwei Bilder je Tick und a3 = 0x80 als halben Takt und ruft re15_r38_birkin_anim zweimal bzw. nur jeden zweiten Tick. Im Original ist a3 das Ueberblendgewicht je Blendschritt; das Bild laeuft je Aufruf genau um 1, und jeder Zustand ruft anim_step genau einmal je Tick. Folge im Spiel: Griff 100 Bilder in 50 statt 100 Ticks, Griff-Bild 8 nach 4 statt 8 Ticks, Auftritt 126 Bilder in 63 statt 126 Ticks, Zucken halbe Dauer, Weichen halbe Tempo. Beim doppelten Takt werden zudem nur gerade Bilder gelesen: im Auftritt fehlen die EDD-Toene Bild 51 (id 10), 79 (id 0), 81 (id 12), 113 (id 11).
- Beleg: RE2 SLUS_007.48: `800296b8 andi v0,a3,0xffff` / `800296bc mult v0,t3` mit t3 = +0x14e (Gewicht = a3 * Blende); `80029b28 lbu v0,0x14d(s2)` / `80029b30 addiu v0,v0,1` / `80029b34 sb v0,0x14d(s2)` (immer +1). Overlay: @0x801053ac Griff, @0x801015d0 Auftritt, @0x80107240 Zucken, je ein `jal 0x8002959c` mit a3 0x200; @0x80104280 Weichen a3 0x10080 (0x10000 rueckwaerts, Gewicht 0x80). Alle 45 anim_step-Stellen im Overlay: Blende*a3 ~ 0xF00 (15*0x100, 7*0x200, 31*0x80). Sonde e: `Auftritt Clip 10 (126 Bilder): 63 Ticks`, `Griff Clip 27 (100 Bilder): 50 Ticks`; echter Lauf r2 Griff F1045-1093 = 49 Ticks.
- Vorschlag: Je Tick genau einen re15_r38_birkin_anim-Aufruf in allen vier Zustaenden, s_bb.halbtakt entfernen. Tabelle 3.1 im Dossier berichtigen (kein doppelter Takt). Griff-, Treffer- und Zeitwerte danach neu messen.
- Gegenprobe: Der Befund ist echt, die Widerlegung ist gescheitert. Im Original läuft das Bild je anim_step-Aufruf genau um +1, a3 ist nur das Überblendgewicht. Der Port spielt Griff, Auftritt und Zucken doppelt so schnell und das Weichen halb so schnell. Das steht als angeblicher RE-Fakt im Code und in Dossier-Tabelle 3.1, ist also keine markierte Gestaltung.

Geprüfte Gegenwege, alle ohne Treffer:
1) a3 wird in FUN_80029614 sonst nirgends benutzt: im ganzen Bereich 0x80029614-0x80029b6c gibt es genau einen Zugriff, und FUN_8002959c reicht a3 unverändert durch.
2) Kein zweiter Vorschub: Die Schreiber von +0x14d in den Anim-Funktionen @0x80029b34, @0x8002a980 und @0x8002a9fc zählen alle +1. FUN_80015cb8 und FUN_80015e7c lesen +0x14d nur, im Griff-Zustand schreibt sonst nichts.
3) Kein zweiter Lauf von B[sub]: Beide Einträge der +0x10e-Tabelle @0x8010b010 (0x80100c20, 0x80100ca4) rufen A[sub] und B[sub] je einmal; die Hauptdispatch hat einen jalr @0x80100330. B[15] hat nur den Aufruf @0x801053ac; ein Tick-Takt der EXE-Schleife ist nicht gesondert belegt, ändert am Verhältnis aber nichts, denn Gehen läuft im Original mit gleichem Takt wie Griff.
4) Kein Frühausstieg im Griff: Clip 27 läuft bis zum Ende, +4 = 0x301 steht nur dort, und +0x14d wird danach nur noch gelesen.
5) Keine Halbierung im Renderpfad: Bei 30 fps gilt ncur = anim_frame (main.c:10338), Standard ist target_fps = 30.
6) Der Port widerspricht sich selbst: enemy_ai_re2_dog.c:204 sagt für denselben Treiber "Port: 1 Frame/Tick, a3 -> anim_blend_rate", dazu gibt es den Helfer re15_re2_advance_959c. Das Bahnsteig-Modul weicht als einziges ab; g5_anim zählt bei a3 = 512 ebenfalls +1.

Einschränkungen am Befund, der Kern bleibt:
- Ton: Die vier ausgelassenen EDD-Töne stimmen, aber in Bank 24 sind die Ids 9 bis 15 stumm (FFFFFFFF). Hörbar fehlt nur id 0 bei Bild 79.
- "Alle 45 Stellen" stimmt nicht wörtlich: exakt a3*(Blende+1) = 0x1000 gilt an 34 von 45 Stellen. Abweichend ist z.B. @0x80102d04 (a3 0x100, Blende 7). Das trägt den Befund nicht, tragend ist @0x800296b8.
- Griff: 49 Log-Zeilen plus Abschluss-Tick ergeben die 50 Ticks.
- Der Vorschlag ist unvollständig: Beim Weichen halbiert das Original +0x144 nur solange +0x14e >= 0x15 (@0x8010424c-70), das fehlt im Port. Die Form setzt zudem anim_blend_rate nicht (skeleton_common.c:224, Rückfall 0x200). a3 gehört dort hinein, statt verworfen zu werden.

Keine Quelle geändert, nichts committet, meine Exe-Kopien sind entfernt. Ein Lauf blieb unter Dauerbeschuss in der Mutation hängen (Clip 20); das ist nicht Teil dieses Befunds, und die Abnahme führt bereits ein Mutationsschleifen-Log. Der Wiederholungslauf mit späterem Feuer lief durch.
- Gegenprobe-Beleg: Eigene Disassemblierung RE2 SLUS_007.48:
`800296b8 andi v0,a3,0xffff` / `800296bc mult v0,t3`, t3 aus `800296a8 lbu t3,0x14e(s2)`. Das ist der einzige a3-Zugriff in 0x80029614-0x80029b6c.
`80029b28 lbu v0,0x14d(s2)` / `80029b30 addiu v0,v0,1` / `80029b34 sb v0,0x14d(s2)`, unbedingt.

G4-Overlay (CDEMD0.EMS kind 0x34 rec 1, Sektor 3238, 0xB2D4 Byte), je ein `jal 0x8002959c`:
@0x801053ac Griff `addiu a3,zero,0x200`
@0x801015d0 Auftritt a3 0x200
@0x80107240 Zucken a3 0x200
@0x80104280 Weichen a3 0x10080 (`lui a3,1` + `ori a3,a3,0x80`)
@0x80105408-10: `lbu v1,0x14d(s1)`, Vergleich == 8.

Port: birkin_bahnsteig.c:281, :399-400, :448-449, :526-527; Kommentar :399 "anim_step a3 = 512: zwei Bilder je Tick".

EDD BAHNSTEIG_G4.EMD: Clip 10 fc=126, Töne (30,11),(51,10),(74,1),(79,0),(81,12),(113,11),(114,1); Clip 27 fc=100.
Bank-24-Map: Ids 0-8 belegt, 9-15 = 0xFFFFFFFF.

Eigene Läufe (Exe-Kopie, eigenes Verzeichnis, Exe neuer als die Quellen):
r1_still: Auftritt F878-939, 62 Ticks, af 2..124 nur gerade, 2.05 s statt 4.20 s
r1_still: Griff F942-990, af 2..98, 1.62 s statt 3.33 s
r1_still: Weichen F1000-1039, af 1..20 in 40 Ticks
r3_schuss: Zucken F930-941, af 7..29, 0.38 s statt 0.83 s; Griff F944-992 gleich
Gehen, Biss, Hieb je Schritt 1.
pose.log: 0 ungerade Bilder in Auftritt und Griff gezeichnet.
se.log r1: Auftritt nur 11 (stumm), 1, 1; danach 0, 7, 8, 9, 9, 7 (Griff).

Dateien in C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegenprobe_a3/: r1_still/state.log, r1_still/pose.log, r1_still/se.log, r3_schuss/state.log, r3_schuss/pose.log, t45.txt

### M6 [hoch] Ueberblendrate fehlt: Pose beim Weichen komplett eingefroren, an jedem Clip-Eintritt 7 Bilder Stillstand, die Form gleitet

- Datei: re15_port/engine/src/birkin_bahnsteig.c:121-127 (bb_clip setzt anim_blend_rate nie); Wirkung re15_port/engine/src/skeleton_common.c:224-229
- Ablauf: Der Zeichenpfad rechnet wp = anim_frac * rate, rate = anim_blend_rate oder 0x200, Klemme 0x1000 = 100 % Vor-Pose. bb_clip setzt nur die Blende. Bei Blende 15 und Vorgabe 0x200 bleibt die Pose stehen, solange frac >= 8: Gehen, Hieb, Biss und Nachhieb frieren am Eintritt 7 Bilder ein, waehrend bb_vortrieb die Form weiterschiebt. Beim Weichen (Blende 31, frac faellt durch den halben Takt nur jeden zweiten Tick) bleibt frac alle 41 Ticks >= 8: die Rueckwaerts-Animation ist nie zu sehen, die Form rutscht als Standbild zurueck. Mit stehendem Spieler pendelt sie zwischen Weichen und Gehen; im Lauf r2 sind 55 % der Ticks ab F480 Weichen (831 von 1521), in den Pendelphasen 47 von 51 Ticks ohne Pose-Aenderung (40 Weichen + 7 Gehen-Eintritt, aus Sonden g und d zusammengesetzt). Das ist das bekannte NPC-Gleiten (Notiz reai-v2-npc-crossfade-decay).
- Beleg: Sonde g, gleicher Weg wie der Zeichner (re15_compute_actor_kf + re15_skel_compute_pose mit g_anim_pose_actor): `WEICHEN-Dauer 40 Ticks, Pose unveraendert in 40 davon, Weg (-335,0)`, frac 26..10. Sonde d, Hieb: Tick 0-6 maxWinkelAenderung 0 bei frac 14..8, x -460 -> -648. Original: a3 0x100 @0x80101b34, @0x8010230c, @0x80102a34, @0x80104ec0; 0x80 @0x80104284. Bild aus dem echten Lauf: laeufe/crop_weichen.png (F1106-1142, Umriss unveraendert) gegen laeufe/crop_biss.png (F1250-1283, Pose aendert sich). Dossier 3.1 nennt anim_step 256, der Code setzt es nicht.
- Vorschlag: bb_clip um die Rate erweitern und e->anim_blend_rate je Stelle setzen wie im Overlay: 0x100 fuer Blende 15 und Rennen, 0x200 fuer Griff, Auftritt, Zucken, 0x80 fuer Weichen. Test: je Zustand darf die gerenderte Pose nach dem Eintritt nicht mehr als wenige Ticks stehen.
- Gegenprobe: Der Befund ist echt. Fuenf Widerlegungsversuche sind gescheitert, alle mit der echten Exe im echten Fenster (Direct3D, kein SOFTWARE_RENDER).

1. Die Rate koennte aus der EM30-Phase stehen geblieben sein: Nein. anim_blend_rate ist im Speicher des eigenen Prozesses in allen 2602 Bildern 0. Der Zeichenpfad nimmt also die Vorgabe 0x200, und ab frac >= 8 klemmt wp auf 0x1000.

2. Die Sonde koennte vom Zeichenpfad abweichen: Nein. Im echten Lauf mit prev_angles je Bild und RE15_POSE_DUMP steht die Pose in jeder Weichen-Phase 39 von 39 Bildern, frac 31->11. Weg dabei rund 300 Einheiten, ohne Treffer. Die Knochenlagen b9/b13 bleiben identisch. Die Pendelphase passt exakt zum Befund: 47 von 51 Bildern ohne Pose-Aenderung; 55 % der Formbilder sind Weichen.

3. Es koennte eine andere Ursache geben: Nein. Ich habe im eigenen Prozess nur die Rate gesetzt (0x80/0x100/0x200, wie im Overlay), Quelle unveraendert. Unveraenderte Bilder fielen bei Weichen von 98 % auf 10 %, bei Gehen von 68 % auf 4 %, bei Biss von 26 % auf 8 %. Der Weg blieb gleich.

4. Das Original koennte ebenso stehen: Nein. RE2 rechnet a3 & 0xffff mal +0x14e ohne Klemme. 0x80*31 = 0xF80 und 0x100*15 = 0xF00 bleiben unter 0x1000, es gibt also kein 100-%-Plateau.

5. Es koennte nur bei stehendem Spieler auftreten: Nein. Die Wiederholung mit Schiessen zeigt Weichen 109 von 111 Bildern stehend und je 7 Standbilder am Eintritt beider Hieb-Clips. In den vorhandenen Laeufen ist Weichen 11-53 % der Form-Ticks (ein Lauf 0 %), auch unter Beschuss.

Die Schwere hoch ist gerechtfertigt: Der Nutzer verlangte eigene Bewegungsablaeufe, die Rueckwaertsbewegung ist aber nie zu sehen.

Korrekturen am Befund, die seinen Kern nicht beruehren:
- Das Dossier enthaelt das Wort anim_step gar nicht; es steht nur im Code-Kommentar birkin_bahnsteig.c:17 und :306.
- Das NPC-Gleiten der Notiz hat dieselbe Wirkung, aber eine andere Ursache (dort fehlte der frac-Abbau).

Der Vorschlag passt zu den Bytes, auch Rennen 0x100. Mit dem Vorschlag bleiben im Messlauf 10 % stehende Bilder im Weichen, die ich nicht weiter untersucht habe. Der halbe Takt ist ausserdem nicht aus dem Original gedeckt: a3 ist dort nur Gewicht plus Richtung, +0x14d steigt je Aufruf um 1.

Nebenbeobachtung ausserhalb dieses Befunds, Ursache nicht untersucht: Ein Treffer im Weichen loescht im selben Bild Flag 0x80, und die Form springt rund 2500 Einheiten (Lauf l3, F357->358: (1315,-24003) -> (2921,-22096); ebenso F601->602).
- Gegenprobe-Beleg: Lauf l1 (unveraendert, Leon steht, RE15_BIRKIN_HP_AT=110, CUT 12): rate-Werte Counter({0: 2602}). Pose-Hash gesamt: sub 8 1255/1279 unveraendert, sub 3 267/391, sub 14 61/239. Je Phase z.B. `F1096-1135 sub8 clip5 frac 31->11 rate 0 stehend 39/39 Weg (285,90)`, `F458-497 ... 39/39` + `F498-509 sub3 ... 8/11` = 47/51.

RE15_POSE_DUMP F1096..F1143 (Auszuege gelesen): b9(0,-2102,0), b13(926,-281,1429) identisch bis frac 8 bei Clip 5; erst ab F1144 (frac 7) aendert sich die Pose.

Lauf l2 (nur Rate im eigenen Prozess gesetzt, Offset +1548): sub 8 56/587, sub 3 8/179, sub 14 20/238.

Lauf l3 (Wiederholung, unveraendert, Pistole): sub 8 109/111, `F360-388 sub2 clip3 frac 14->0 stehend 7/28`, `F389-432 sub2 clip4 ... 7/43`.

Original, Bytes aus CDEMD0.EMS Sektor 3238 (@0x80100000):
- `lui a3,1` / `ori a3,a3,0x80` @0x8010427c-84
- `lui v0,0x1f; ori v0,v0,2; sw v0,0x14c(s0)` @0x801041dc-e4 (Clip 2, Blende 31)
- a3 = 0x100 @0x80101b34, @0x8010230c, @0x80102a34, @0x80102d08, @0x80104ec0
- a3 = 0x200 @0x801015d4, @0x801053b0, @0x80107244

RE2-Engine (a3/+0x14e/+0x14d):
- `andi v0,a3,0xffff; mult v0,t3` @0x800296b8-bc, `addiu v0,zero,0x1000; subu v0,v0,t1` @0x800296e4-ec
- frac-1 @0x800299c0-cc
- +0x14d += 1 @0x80029b28-34
- Richtungsbit: `and v0,a3,0xffff0000` @0x800295b8-c4

Port:
- birkin_bahnsteig.c:121-127 (bb_clip schreibt keine Rate), Blende 0xf: Zeilen 249, 297, 318, 335, 353; 0x1f: Zeile 269
- skeleton_common.c:224-229
- einzige Schreiber von anim_blend_rate gegreppt: keiner in birkin_bahnsteig.c oder birkin_formen.c

Dateien:
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_blend\l1\rate.txt
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_blend\l1\pose.log
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_blend\l2\rate.txt
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_blend\l3\rate.txt
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_blend\crop_vgl.png (oben unveraendert, gleicher Umriss F458-497; unten mit Rate, Umriss aendert sich)

### M7 [hoch] Treffer im Weichen loescht das Rueckwaerts-Bit: Form springt rund 2000 Einheiten und laeuft danach vorwaerts

- Datei: re15_port/engine/src/birkin_bahnsteig.c:268-273 und :168-177 (bb_spur); Schreiber re15_port/engine/src/re15_damage.c (anim_flags &= ~0x80 in beiden Treffer-Appliern)
- Ablauf: bb_weichen setzt anim_flags|=0x80 nur in Phase 0. Jeder Spielertreffer loescht das Bit. Bleibt der Trefferzaehler unter 12, stellt case 2 Sub 8 Phase 1 wieder her, das Bit kommt nicht zurueck. bb_spur liest jetzt das Vorwaerts-Bild und zieht die letzte Rueckwaerts-Spur ab: tempo = sx(13) - sx(75), ein Sprung von rund 2000 Einheiten in einem Tick. Die restlichen Ticks laeuft die Form vorwaerts auf den Spieler zu statt zurueck, Pose und Tempo kehren um. Wer nah vor der Form steht und schiesst, trifft das oft, weil sie dort die meiste Zeit im Weichen ist.
- Beleg: Sonde b (echter Applier re15_enemy_take_damage): vor dem Treffer `d=(-13,-2)`, `flags=0080`; nach dem Treffer `flags=0000`, `nach Treffer tick 0: sub=8 ph=1 clip=5 bild=13 flags=0000 d=(-2017,5)`, danach `d=(42,5)`, `d=(42,10)`. Original kennt den Fall nicht: die Richtung ist Argument je Aufruf, a3 0x10080 @0x80104280 und a3=1 @0x80104248 (FUN_80015e7c).
- Vorschlag: Richtung in bb_weichen je Tick setzen (e->anim_flags |= 0x80u vor bb_vortrieb), wie a2 je Aufruf im Original. Zusatztest: Treffer im Weichen, Schritt je Tick darf 60 Einheiten nicht uebersteigen.
- Gegenprobe: Widerlegung gescheitert, der Befund ist echt. Geprueft in Code, Kontrollsonde, eigenem Lauf und Bild.

Code: bb_weichen setzt das Bit nur in Phase 0 (birkin_bahnsteig.c:268-273). Alle Treffer-Applier loeschen es ohne Bedingung: re15_damage.c:2841 (Hitscan), :3204 (take_damage) und zusaetzlich :4355 (GL). case 2 stellt danach bei Zaehler < 12 Sub 8 / Phase 1 wieder her (Zeile 493-497, Zuwachs[8] = 8). Niemand setzt das Bit neu. bb_spur (Zeile 168-176) rechnet dann Vorwaerts-Bild minus letzte Rueckwaerts-Spur.

Ursache: Eine eigene Sonde gegen die echte Bibliothek mit drei Varianten belegt, dass allein das Bit den Sprung ausloest. Kein Rueckstoss oder anderer Mechanismus steckt dahinter. Mit Bit-Rueckstellung direkt nach dem Applier sind die Schritte identisch zum Lauf ohne Treffer.

Lauf: Frischer Prozess, echte Exe, echte Eingabe, Flinte, nur lesender Zugriff, kein Messhaken. Beide Spruenge erscheinen bildgenau, das Bit faellt genau im Trefferbild. Der Fehler tritt also im normalen Kampf auf, nicht nur in der Sonde.

Bild: Im echten Fenster steht die Form in einem Bild an Leon, er wird von ihr verdeckt. Im naechsten steht sie deutlich weiter hinten im Wagen.

Original: Dort gibt es keinen Richtungszustand. B[8] uebergibt a3 bei jedem Aufruf, die Treffer-Fortsetzung laeuft ueber +0x1fc in AKTIV.

Praezisierungen zum Befund:
- Die Sprungweite haengt vom Bild ab: gemessen 1661, 2002, 2003, 2017 und 2577.
- Der Sprung geht vom Spieler weg. Danach laeuft die Form bis zum Ablauf der Uhr vorwaerts auf ihn zu.
- Bedingung ist Zaehler vor dem Treffer <= 3. Bei hoeherem Zaehler geht es laut Code ins Zucken ohne Sprung (nicht gemessen).
- Haeufigkeit im eigenen Lauf: alle 3 Treffer der Gang-Phase lagen im Weichen, 2 davon sprangen, der dritte war toedlich.
- Stand: Exe und Bibliothek sind neuer als birkin_bahnsteig.c, Arbeitsbaum sauber, beide Dateien in master gleich 4ee345aaa.
- Gegenprobe-Beleg: Eigener Lauf: Exe re15_port/build/platform/pc/re15_pc.exe (Kopie mit eigenem Namen), Raum 5090, Waffe 8:200. Protokoll C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegenprobe_bit/lauf1/bit.txt

F1299 1/8/1 mo5 af19 aflags=0080 (7928,-23087) hp110 m0 spur=(2381,-19) | PL(10024,-22168)
F1300 1/8/1 mo5 af19 aflags=0000 (6372,-23671) hp70 m8 spur=(721,63) tempo=-1660 quer=82
F1302 1/8/1 mo5 af20 aflags=0000 (6403,-23632) tempo=44 quer=25

F1461 1/8/1 mo5 af2 aflags=0080 (9030,-22724) hp70 m0 spur=(2646,0) | PL(10964,-22157)
F1462 1/8/1 mo5 af2 aflags=0000 (6557,-23449) hp30 m8 spur=(68,-7) tempo=-2578 quer=-7
F1464 1/8/1 mo5 af3 aflags=0000 (6586,-23445) tempo=30 quer=-4

Abstand zum Spieler bei F1462: 2016 -> 4592. Dieselben Zahlen stehen im unabhaengigen Vorlauf f21_leon/state.log (F1300 d=1661, F1462 d=2577). Mit Pistolenlauf f7_form: F1085 d=2002, F1302 d=2003, hp 105->90.

Sonde (gegenprobe_bit/gp_bit.c, gp_bit.out), Treffer ueber re15_enemy_take_damage bei Bild 13:
Variante 0 ohne Treffer: i=2 d=(-14,-1), i=4 d=(-16,0).
Variante 1 Treffer: flags=0000, i=2 d=(-2017,5), i=4 d=(42,5), i=6 d=(42,10).
Variante 2 Treffer + Bit zurueck: i=2 d=(-14,-1), i=4 d=(-16,0), alle Schritte gleich Variante 0.

Bilder: gegenprobe_bit/lauf1/blatt_s1.jpg und blatt_s2.jpg (obere Reihe vor, untere nach dem Sprung, Video fenster.mkv).

Original G4-Overlay (code/ovl_all.txt):
80104244 jal 0x80015e7c / 80104248 addiu a3,zero,1
8010427c lui a3,1 / 80104280 jal 0x8002959c / 80104284 ori a3,a3,0x80
80107170 sltiu v0,v0,0xc / 80107174 beqz -> 0x80107198
80107180 lw v0,0x1fc(a0) / 80107188 jal 0x80100bbc / 8010718c sw v0,4(a0)

### M8 [hoch] Treffer im Zurückweichen: Form springt in einem Bild 1660 bis 2577 Einheiten

- Datei: re15_port/engine/src/birkin_bahnsteig.c:168, 270 mit re15_port/engine/src/re15_damage.c:2841
- Ablauf: Form in WEICHEN, Clip 5 rückwärts über anim_flags 0x80. Ein Schuss löscht dieses Bit im Resolver. bb_weichen setzt es nur beim Eintritt. Der nächste bb_spur rechnet die Differenz zwischen Vorwärts-Slot und gespeichertem Rückwärts-Slot, die Form teleportiert und läuft danach im Weichen vorwärts. Das trifft jeden Spieler, der im Pendelabstand weiterschießt.
- Beleg: f7_form, f8_re2, f8_elza, f9 gleich: F1085 (8060,-23019)->(6210,-23786) d=2002; F1302 d=2003. f21_leon (Flinte, hookfrei): F1300 d=1661, F1462 (9030,-22724)->(6557,-23449) d=2577. Normal sind 12-30 je Bild.
- Vorschlag: Richtung nicht im Aktor-Bit halten: Rückwärts-Merker in s_bb führen und bb_spur/Anim-Aufruf je Tick mit a3=1 wie @0x80104248 fahren, oder das Bit in bb_weichen je Tick neu setzen.
- Gegenprobe: Der Befund ist echt. Keiner meiner Widerlegungsversuche hat gehalten; Ablauf, Zeilen und Sprungweiten stimmen.

Code: Das Rückwärts-Bit setzt nur der Eintritt in bb_weichen (birkin_bahnsteig.c:270). Der Treffer-Zweig in re15_bb_tick stellt sub_state_2 = 1 wieder her, also läuft der Eintritt nicht erneut. Alle drei Schaden-Anwender löschen das Bit (re15_damage.c:2841, 3204, 4355). Der Schreiber-Zensus der engine/src-Dateien zeigt für die Form keinen weiteren Setzer. Der nächste bb_spur (Zeile 168) rechnet deshalb Vorwärts-Slot minus gespeicherten Rückwärts-Slot.

Original: Im G4-Overlay ist die Richtung kein Zustand. B[8] übergibt sie in jedem Tick als Argument (a3 = 1 @0x80104248, a3 = 0x10080 @0x80104280). Ein Treffer kann sie dort nicht löschen; der Sprung ist also eine reine Port-Abweichung.

Widerlegungsversuche:
- Andere Sprungursache: ausgeschlossen. Die Weiten folgen auf +-1 aus der Spur von Clip 5 im Asset.
- Haken oder Geräteartefakt: ausgeschlossen. Mein hakenfreier Elza-Lauf (Item 15) zeigt denselben Sprung, der Leon-Lauf (Pistole) mit anderem Schuss-Takt ebenfalls.
- Kein Effekt im Fenster: ausgeschlossen. Im Video wächst die Form zum Schuss-Blitz sprunghaft, die Pose wechselt.

Die Sprungrichtung ist weg von Leon. Danach zählt die Spur aufwärts, die Form läuft in Sub 8 vorwärts, wie gemeldet.

Einschränkung: "jeden Spieler" ist zu breit. Der Sprung kommt nur, wenn der Treffer in Sub 8 landet und der Zähler vorher <= 3 ist. Ab 4 geht es laut Code über Sub 20 und bb_clip ohne Sprung (nur gelesen, nicht im Lauf beobachtet). Ein tödlicher Treffer startet die Rückstoß-Szene (f21_leon F1624 d=0). Trifft der Schuss erst nach Ablauf der Uhr, springt die Form ebenfalls nicht (f21_elza F1136). Die Treffer in Sub 8, die nicht in eine dieser Ausnahmen fielen, sprangen alle. In meinen zwei gültigen Läufen war das je 1 Sprung (Leon bei 9 Form-Treffern, Elza bei 2). Der dritte Lauf (Elza, Dauerfeuer) kam nie bis zur Form und zählt nicht. Schwere und Fix-Richtung halte ich für richtig. master 9c169ecd4 enthält den Stand unverändert, kein Diff an birkin_bahnsteig.c und re15_damage.c gegen 4ee345aaa.
- Gegenprobe-Beleg: Eigene Läufe mit Kopie der Exe (Baum r41_bahnsteig), anim_flags und s_bb je Bild live aus dem Prozessspeicher gelesen.

1) Leon, Pistole, Mutation per RE15_BIRKIN_HP_AT=700, Schuss-Takt 1,2 s:
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegenprobe_treffer\l1_pistole\je_bild.txt
F1084 st1/8/1 mo5 af8 aflags=0080 hp105 @(8121,-23013) meter=1 spur=(2554,-19)
F1085 st1/8/1 mo5 af9 aflags=0000 hp90 @(6030,-23927) meter=9 spur=(271,-28), d=2282, Abstand zum Spieler 4395
F1086 st1/8/1 af9 @(6063,-23913) spur=(308,-28) tempo=(37,0)

2) Elza, Item 15, hakenfrei:
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegenprobe_treffer\l3_elza\je_bild.txt
F1123 st1/8/1 af14 aflags=0080 hp150
F1124 st2/15/1 aflags=0000 hp50 @(7993,-23046) spur=(2458,-35)
F1125 st1/8/1 af15 @(6199,-23797) spur=(513,-24) tempo=(-1945,11), d=1944

3) Spur aus BAHNSTEIG_G4.EMD gerechnet (Clip 5, fc=89):
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegenprobe_treffer\spur5.txt
Vorhersage |Spur(Bild M) - Spur(88-N)| gegen Messung: 2283/2282 (l1), 1945/1944 (l3), 2003/2002-2003 (f7, f8_re2, f8_elza, f9), 1662/1661 und 2578/2577 (f21_leon).

4) Logs des Prüfers nachgerechnet: F1085 und F1302 sowie F1300 und F1462 stimmen bis auf die Stelle mit der Meldung überein, jeweils HP-Sturz im selben Bild, Sub 8.

5) Disasm G4-Overlay (CDEMD0.EMS rec 1, Sektor 3238):
@0x80104244 jal 0x80015e7c / @0x80104248 addiu a3,zero,1
@0x8010427c lui a3,1 / @0x80104280 jal 0x8002959c / @0x80104284 ori a3,a3,0x80
Beides läuft in Phase 1 je Tick.

6) Sicht im echten Fenster (obere Reihe vor, untere nach dem Sprung):
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegenprobe_treffer\l1_vorher_nachher.png

### M9 [hoch] Bild friert bis zu 2 Minuten ein: erzwungene Szenen-Kamera kämpft gegen die Kamerazone des Spielers

- Datei: re15_port/engine/src/birkin_formen.c:336-339 (bf_kamera), Aufrufe :381, :392, :215-216; Zonen-Scan engine/src/aot_common.c:1658-1660
- Ablauf: Rückstoss-Szene und G5-Auftritt setzen jedes Bild Cut 12 bzw. 11. Steht der Spieler in einer Zone, deren Wechsel aus genau diesem Cut führt, setzt der Scan cam_id zurück und bf_kamera wieder hin. Der Apply läuft damit jedes Bild (Mechanismus aus dem Code gelesen; die Messung passt: 233 ms je Bild = Dauer eines Kamerawechsels, Einzelwechsel gemessen 235 ms). Das Spiel läuft mit 4-5 fps weiter, das Fenster zeigt aber ein Standbild, das Feuer steht still. Betroffen: x 1203..2703 und 14000..15400 (Szene Cut 12), x 2800..4200 (G5-Auftritt Cut 11).
- Beleg: f15_west und f15_west2 identisch: 4,3 fps von F1400 bis F2149, Fenster steht von F1636 bis F2140 rund 115 s (Bild 04_standbild_g5_auftritt_F1636-2140.jpg). f16_c: F1411-F1467 13 s (Bild 05_standbild_rueckstoss_F1411-1465.jpg). f16_e, x=14916: F1410-F1482 16,5 s bei 5,3 fps. Gegenprobe f9 (x=11577): 29 fps ohne Standbild; f16_d (x=13000): 29,7 fps ohne Standbild.
- Vorschlag: Für die Szene die Zonen abschalten (cut_auto_enabled = 0, wie Skript-Kameras), danach wieder an, statt cam_id jedes Bild zu überschreiben. bf_kamera nur bei echter Änderung und nicht gegen den Scan feuern lassen.
- Gegenprobe: Widerlegung gescheitert, der Befund ist echt. Er liess sich ohne Speicherschreiben nur per Eingabeskript nachstellen, auch im Standard-KI-Modus RE2.

Mechanismus (Code gelesen, Logzeilen bestaetigen ihn): Je Bild laufen pc_cam_present_apply (main.c:5705), dann re15_aot_scan (game_step_common.c:2263), dann re15_bf_raumtick (enemy_ai_common.c:15103). Der Scan setzt cam_id auf das Zonenziel (aot_common.c:1657-1659). bf_kamera setzt cam_id zurueck und cam_change_pending=1 (birkin_formen.c:338). Am Bildende sieht re15_kamerahalt_r41_haelt pending (kamerahalt_r41.c:36) und haelt: kein Present, dazu 6 Bilddauern Wartezeit (main.c:11703-04, bildhalt_r41_pc.c). Das ergibt 33 + 200 = 233 ms je Bild.

Praezisierung zur Meldung: Die Zeit kommt vom Kamerahalt aus Spur E, nicht vom Hintergrund-Laden. Der Cut aendert sich nicht, also laeuft re15_bg_load_cut gar nicht.

Es gibt keinen Schutz: ROOM5090 sub04 endet mit Cut_auto 01 @0x134C, die Zonen sind im Kampf also an. Cut_chg (@0x800402d4) und cut_chg_zwilling (gatorszene_r41.c:790) schalten sie ab, bf_kamera nicht. Das ist die Ursache; der Vorschlag der Meldung passt dazu.

Die Streifen sind im normalen Spiel erreichbar: Der Kampf beginnt bei x=13775, 225 Einheiten vor dem Oststreifen. Der Weg zum G5 fuehrt durch x 4200..2800, und die Tentakel warfen Leon in g5w in diesen Streifen zurueck.

Grenzen: Wer waehrend Szene und Auftritt ausserhalb der Streifen steht, merkt nichts (x=12953, x=11577, x=22789). Den Streifen x 1203..2703 habe ich nicht selbst per Eingabe gefahren; dafuer stehen RVD-Zeile und f16_c des Pruefers (dort mit Speicherschreiben). bf_kamera steckt schon im Basisstand 6e3aa3289 (seit 2341475d0), ist also nicht erst durch Spur R entstanden, bleibt aber ein Fehler des Endkampfs.
- Gegenprobe-Beleg: Exe r41/bahnsteig (Stand 02:04, HEAD 4ee345aaa); birkin_formen.c, kamerahalt_r41.c und aot_common.c sind gleich mit master 9c169ecd4. Echter Weg: Tuer 2, Kampfstart. Laeufe unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen_kam/ (Skripte *.skript, Auswertung ana.py). Fenster per gdigrab hwnd, Standbild per freezedetect -75 dB. EM30 in allen Laeufen per RE15_BIRKIN_HP_AT=700 getoetet (F710); RE15_KAMERAHALT_LOG ist nur Protokoll.

Oststreifen, Szene Cut 12:
- g4b: Leon rein per Eingabe auf x=14339, Cut 13, Form-Tod per Haken F1410. HALTEN F1410-F1477 ununterbrochen (68 Bilder, 15,6 s, 4,3 fps), z.B. "[kamerahalt] F1411 ... pend=1 wv0A=12 cam=12 -> HALTEN" + "[standbild] F1425 6 Bilder 198 ms". Fenster steht Video 62,6-78,5 s = F1406-F1477.
- g4c (Wiederholung): dasselbe. Letzter Schuss traf aus dem Streifen (F1401, hp 15 auf 0), Tod per Haken F1410.
- g4r (Wiederholung, Standard-KI RE2): dasselbe.
- Gegenprobe g4: dasselbe Skript, Rueckschritt 0,7 s kuerzer, x=12953: nur 6 HALTEN im ganzen Lauf, kein Standbild in der Szene.

Weststreifen, G5-Auftritt Cut 11 (Form-Tod in beiden Laeufen durch echten Treffer F1395, x=11577):
- g5x: Leon geht per U3.6 nach x=3585. HALTEN F1642-F2138 (497 Bilder, 115,2 s, 4,3 fps). Fenster steht F1638-F2138 (4,4 + 111,1 s).
- g5w (Wiederholung, Leon laeuft durch): F1642-F1660 (19 Bilder, 4,2 s), dann werfen ihn die Tentakel auf x=3005: F1871-F2135 (265 Bilder, 61,4 s). Fenster steht 60,2 s; Bilder t=90 s und t=140 s PSNR 78 dB.

Kontrollen ausserhalb: g1 (x=25024), g2 (x=22789), g3 (x=25024): je 9 HALTEN im ganzen Lauf, Szene ohne Standbild.

RVD ROOM5090 @0x260, Stride 20: @0x490 `01 ff 0b 0c f0 0a` = Cut 11 -> 12, x 2800..4200; @0x4b8 `01 ff 0c 0b b4 04` = Cut 12 -> 11, x 1203..2703; @0x4cc `01 ff 0c 0d b0 36` = Cut 12 -> 13, x 14000..15400. ROOM5091 hat dieselbe Tabelle.

Keine Quelldatei geaendert, nichts committet, Hauptbaum nicht angefasst, eigene Exe-Kopien sind wieder entfernt.

### M10 [mittel] Doppelschlag: Form springt 680 bzw. 545 Einheiten, weil die Spur nicht neu aufgesetzt wird

- Datei: re15_port/engine/src/birkin_bahnsteig.c:125 (bb_clip nullt die Spur), :318 (Clip-Wechsel mitten im Clip)
- Ablauf: Beim Doppelschlag wechselt die Form auf den Gegen-Clip ab Bild 7 oder 20. bb_clip setzt spur_x/spur_z = 0. Der naechste bb_vortrieb nimmt deshalb die ganze aufgelaufene Spur des Einstiegsbilds als Schritt: Clip 4 Bild 7 = (608,-305), Clip 3 Bild 20 = (-495,-225). Das sind 25 Prozent von Leons Hoehe in einem Bild, sichtbar als Ruck mitten im Hieb.
- Beleg: Sonde c: `Wechsel Clip 3 Bild 29 -> Clip 4 Bild 7`, `Tick+1: Clip 4 Bild 8 Versatz (604,-312)`, davor und danach rund 100-160. `Wechsel Clip 4 Bild 18 -> Clip 3 Bild 20`, `Tick+1 ... Versatz (-499,-219)`. Original setzt die Spur eigens neu: @0x80101c4c `jal 0x80015e7c` und direkt danach @0x80101c60 `jal 0x80015e7c`, der zweite Aufruf liefert Schritt 0 (FUN_80015e7c: +0x144 = neu - +0x20c).
- Vorschlag: Nach dem Clip-Wechsel im Doppelschlag bb_spur zweimal aufrufen wie @0x80101c4c/@0x80101c60 oder spur_x/spur_z auf die Werte des Einstiegsbilds setzen.
- Gegenprobe: Der Befund ist echt, ich konnte ihn nicht widerlegen. Fünf Widerlegungsversuche sind gescheitert.

1. Original: Ich habe das G4-Overlay und FUN_80015e7c selbst aus den Bytes disassembliert. Der zweite Aufruf ergibt +0x144 = 0. Der Rücksetz-Zweig greift nicht, weil das Bild ungleich 0 ist, und das Tick-Ende addiert diese 0 auf die Lage. Im Original gibt es hier also keinen Sprung.

2. Asset: Die Spur stammt nicht aus dem Umbau. 0 von 1296 Keyframes weichen beim Spurfeld vom unveränderten RE2-G4 ab.

3. Port-Code: bb_clip nullt spur_x/spur_z (Zeile 125). bb_spur setzt nur bei Bild 0 neu auf, also nimmt der nächste bb_vortrieb die ganze aufgelaufene Spur des Einstiegsbilds als Schritt. Pose-Sprung als Verdeckung scheidet aus: die Pose blendet über anim_frac 15 (skeleton_common.c:220-228), die Lage springt allein.

4. Echtes Spiel: Der Sprung kommt ohne Zwangslage der Sonde. Zwei eigene Fensterläufe zeigen ihn in beiden Richtungen; lauf3 ist die Wiederholung eines fremden Laufs und traf ihn zweimal. Im Video ist er messbar und läuft gegen die Bewegungsrichtung.

5. Ursache: Eine gepatchte Kopie von birkin_bahnsteig.c im eigenen Ordner, mit nur `bb_spur(e); bb_spur(e);` nach dem Clip-Wechsel, nimmt den Sprung bei allen ausgewerteten Wechseln weg. Der Tick danach hat Versatz 0, wie im Original. Der Vorschlag des Prüfers ist damit tragfähig.

Einschränkungen zum Befundtext, die ihn nicht kippen:
- 25 Prozent von Leons Höhe gilt nur für 680, die 544 sind rund 20 Prozent.
- Die normalen Schritte im Hieb liegen bei 100 bis 233, nicht nur bei 100 bis 160.
- Der Doppelschlag kommt nicht in jedem Kampf. HIEB wird in der Nabe nur bei dist 3300 bis 4000 gewählt, darunter gewinnt der Biss (Zeilen 222-223). In lauf1 stand Leon nah an der Form und es gab keinen Doppelschlag. Läuft die Form aus der Distanz an, folgte er in allen Hieben (8 von 8 in der Sonde).

Nicht Teil dieses Befunds und nicht untersucht: lauf2 zeigt im WEICHEN weitere Sprünge von rund 2000 Einheiten (F1085, F1302).
- Gegenprobe-Beleg: ORIGINAL (roh aus CDEMD0.EMS Sektor 3238 @0x80100000):
- @0x80101c4c `9f57000c jal 0x80015e7c`, Delay-Slot @0x80101c50 `4c0142ae sw v0,0x14c(s2)`, das neue Clip-Wort gilt also schon im ersten Aufruf.
- @0x80101c60 `9f57000c jal 0x80015e7c` mit a3 = 0.
- Tick-Ende @0x80102348 `jal 0x800152c8`.
- Tabelle @0x8010b0f0 = `1e 07 13 14`, das entspricht k_doppel.

FUN_80015e7c (roh aus info/re2leon/SLUS_007.48):
- @0x80015f04-0c `lbu v0,0x14d(t0)` / `bnez`: kein Rücksetzen bei Bild ungleich 0.
- @0x80015fb4 `lhu v1,0x20c(t0)`, @0x80015fc4 `sw v0,0x20c(t0)`, @0x80015fcc `subu v1,t1,v1`, @0x80015fd8 `sh v1,0x144(t0)`.
- Der zweite Aufruf ergibt also neu - neu = 0.

ASSET: rohes RE2-G4 rec 3 gegen BAHNSTEIG_G4.EMD, 0 von 1296 Keyframes mit abweichender Spur.
- Clip 4 Bild 7 = (608,-305), Bild 8 = (723,-329).
- Clip 3 Bild 20 = (-495,-225), Bild 21 = (-372,-192).

EIGENE FENSTERLÄUFE (Exe-Kopie re15_pc_gegenprobe_*, state.log je Bild):
- lauf2, F1331 mo3 af20 d=71; F1332 mo3 af21 (8768,-22718)->(8332,-23044) d=544; F1333 d=127.
- lauf3, F765 mo4 af7 d=162; F766 mo4 af8 (5363,-23385)->(5956,-23717) d=680; F767 d=117.
- lauf3, F1102 mo4 af8 d=681.

FREIE SONDE gegen libre15_engine.a (Form ungezwungen, Spieler steht, nur zwischen Angriffen zurückgesetzt):
- Hiebe=8, Wechsel=8, Sprünge>300 im Tick danach: 7; der achte Wechsel lag am Ende des Laufs und wurde nicht mehr gemessen. Größter Wert 681, größter Schritt sonst im Hieb 233.
- Mit gepatchter Kopie: 8 Wechsel, 7 ausgewertet, jeweils Tick+1 Versatz (0,0), Sprünge 0.

SICHTBAR (lauf2, Schnitt 12, Kamera ROOM5090 @0x1e0, H=208, Schirmbild 640x480):
- Projektion F1331->F1332 = -16.4 px, Nachbarbilder +3 bis +6 px.
- Im Fenstervideo Rumpfmuster v09->v10 = -14 px, danach +7, +4, +4.

DATEIEN:
- C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_doppel\lauf2\state.log
- C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_doppel\lauf3\state.log
- C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_doppel\gp_probe.c
- C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_doppel\birkin_bahnsteig_fix.c
- C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_doppel\v2_crop.png

### M11 [mittel] Rennen dreht nach dem Treffer nicht ab und schiebt Leon rund 9900 Einheiten durch den Wagen

- Datei: re15_port/engine/src/birkin_bahnsteig.c:416 (Lenken ungegatet), :425-429 (getroffen wird nie gelesen)
- Ablauf: Spieler steht fern (dist >= 0x1f41), die Form rennt mit rund 345 Einheiten je Tick los und trifft bei tempo >= 201. Der Port lenkt danach weiter mit 64 je Tick auf den Spieler und rennt noch bis zum zweiten Clip-Ende. Der Koerperschub schiebt Leon vor sich her und ein zweiter Treffer landet unterwegs. Das Dossier meldet das Schieben als behoben (Abschnitt 4), behoben ist nur der Weg ueber das Weichen. Das Original lenkt nach dem Treffer nicht mehr zum Spieler und dreht aktiv weg.
- Beleg: Echter Lauf r4, wiederholt als r4b mit denselben Zahlen (laeufe/r4b/state.log): `F384 PL 12000 hp 95 | FORM x 8417`, `F396 PL 14765`, `F408 PL 18718`, `F414 PL 21148 hp 90`, `F420 PL 21896`. Original: @0x80102b90 `andi v0,v0,2` / `bnez` ueberspringt das Lenken @0x80102be4 bei +0x21a & 2; beim Treffer @0x80102f8c steer mit a3 = -0x80 (weg vom Spieler), +0x15a = 6 @0x80102f68, dann @0x80103014 je Tick steer a3 = -0x10.
- Vorschlag: In bb_rennen: Lenken nur bei !s_bb.getroffen; beim Treffer re15_enemy_steer_point(..., -128) und 6 Ticks lang -16, wie die zitierten Stellen. Test: nach dem Rennen-Treffer darf der Spieler nicht weiter als der Rueckstoss getragen werden.
- Gegenprobe: Widerlegung gescheitert, der Befund ist echt. Alle Wege fuehren zum selben Ergebnis.

1. Bytes: Das G4-Overlay habe ich unabhaengig aus info/re2leon/PL0/PLD/CDEMD0.EMS neu disassembliert (Sektor 3238, identisch mit shared_assets/RE2). Alle zitierten Stellen stehen so da. FUN_80015558 ist an den Rohbytes geprueft, negatives a3 heisst Peilung +0x800, also weg vom Spieler.

2. Port: Zeile 416 lenkt ohne Gate. s_bb.getroffen wird in bb_rennen nur geschrieben, nie gelesen (gelesen nur in bb_hieb und bb_biss). Die Abweichung steht weder in Abschnitt 6 noch im Modulkopf als Gestaltung, der Kommentar zitiert nur @0x80102be4.

3. Eigene Laeufe: Der Effekt reproduziert sich bildgenau und ist kein Harness-Artefakt. Er tritt auch auf, wenn Leon zielt und schiesst, seitlich versetzt steht oder ueber die echte Tuer 2 ohne RE15_PLAYER_POS im Wagen kommt.

4. Gegenprobe: Nur das bb_rennen-Objekt wurde in einer Scratch-Exe getauscht (Gate plus -128 und 6 mal -16). Die Trageweite sinkt von 9896 auf 1373 Einheiten, der zweite Treffer entfaellt, die Form laeuft an Leon vorbei. Bis zum Treffer sind beide Laeufe bildgleich. Damit ist die Kausalkette belegt.

Praezisierungen zum Befund:
a) Der Koerperschub selbst ist Original-Mechanik (FUN_8002aec4, re15_body_push_player), kein Port-Fehler. Ursache ist allein das fehlende Abdrehen.
b) Die 9900 sind der Weg ab x=12000. Die Trageweite haengt vom Standort ab, gemessen 3701 bis 9896; Leon endet jedes Mal am Gangende, sofern das Rennen nicht vorher endet.
c) Das Tempo liegt bei rund 355 je Tick ueber einen Clip (6387 in 18 Bildern), nicht bei 345.
d) Die vorgeschlagene Testschranke ist zu eng: Auch mit Originallogik wandert Leon rund 1100 bis 1400 Einheiten (Rueckstoss 500 plus seitlicher Schub beim Vorbeilaufen). Die Schranke muss darueber liegen, sonst ist der Test auch mit Fix rot.

Zusatz fuer den Fix: Nach dem Abdrehen schrammt die Form an der Gangwand entlang, z bleibt bei -22718. Das stoert die Gegenprobe nicht.
- Gegenprobe-Beleg: ORIGINAL (G4-Overlay, Bytes):
- @0x80102b88 lhu v0,0x21a(s3); @0x80102b90 andi v0,v0,2; @0x80102b94 16004014 bnez v0,0x80102bf0, springt ueber das Steer @0x80102be4.
- @0x80102f3c v1=6; @0x80102f68 sh v1,0x15a(s3); @0x80102f6c/70 +0x21a |= 2.
- @0x80102f8c jal 0x80015558 mit Delay-Slot 80ff0724 a3=-0x80.
- @0x80102ff4/ffc +0x15a pruefen; @0x80103014 jal 0x80015558 mit f0ff0724 a3=-0x10; @0x80103028 +0x15a -= 1. Der Rest des Blocks ist nur Partikel (jal 0x8001bf10).
- RE2-EXE @0x8001559c bgez s0; @0x800155a8 addiu v0,v1,0x800; @0x800155ac andi v1,v0,0xfff (ghidra_re2_Leon.txt:80414-80418).

PORT: birkin_bahnsteig.c:416 re15_enemy_steer_point(e, pl->x, pl->z, 64) ohne Bedingung; :428 s_bb.getroffen = 1, in bb_rennen nie gelesen.

EIGENE LAEUFE (Exe aus dem Baum, Stand master; je Lauf eigene Exe-Kopie, exit=0):
- s0 (Wiederholung r4b): F383 PL 12000 hp100 | Form 8143; F384 hp95 | Form 8417; F396 PL 14765; F408 PL 18718; F410 hp90; F414 PL 21148; F417 PL 21896 und Ruhe, also 9896 Einheiten. Kamera 12 -> 13 (F395) -> 14 (F412). s1 identisch.
- s2 (zielt und schiesst): Treffer F384 und F410, Spielerweg im Rennen 9225.
- s3 (z=-24400 statt -23400): Spielerweg im Rennen 8131.
- n1 (DOOR FIRE slot=2 spawn 24800, geht bis x=13775, steht): F622 hp95, F645 hp90, F652 PL 21885, also 8110 Einheiten.
- n2 (Tuer, geht bis x=8150, wendet, laeuft nach Osten; Rennen ab F597, Leon dabei bei (10625,-24732)): Spielerweg im Rennen 3701.

GEGENPROBE (gleicher Lauf, nur bb_rennen gepatcht):
- p1s0: bis F383 bildgleich, F384 Form r3951 statt r4095, Leon ruht bei (13138,-24169), dx 1138, dz -769, nur 1 Treffer, Form faehrt bei z=-22718 vorbei bis x=14937.
- p1n1: 925/-619, 1 Treffer.
- p1s2: Spielerweg im Rennen 1369, 1 Treffer.
- p1s3: 666.
- p1n2: 664.

WAGEN: SCA Box 28/29 ergeben Gang z -25200..-21700 bis x~20225; x=21896 liegt im Durchgang zwischen Box 26 und 27.

Dateien:
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_rennen\l\s0\state.log
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_rennen\l\n1\state.log
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_rennen\l\p1s0\state.log
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gegen_rennen\p1\birkin_bahnsteig.c
Baum unveraendert, keine Exe-Kopien liegen geblieben.

### M12 [mittel] Trefferzaehler indexiert die Tabelle mit der KI-Unterphase statt mit der Waffe

- Datei: re15_port/engine/src/birkin_bahnsteig.c:486 und :490
- Ablauf: k_zuwachs @0x8010b0fb ist je Waffe, das Original liest +0x5 im Trefferzustand, also die Waffen-Id, die auch der Port-Applier dort ablegt (re15_damage.c e->sub_state_1 = weapon_id). Der Port nimmt die gesicherte KI-Unterphase. Folge: Schrotflinte und Pistole zaehlen gleich, das Zucken haengt am Zustand: im Biss (Sub 14, +20) zuckt die Form schon beim ersten Treffer egal mit welcher Waffe, im Griff (Sub 15, +2) erst nach sechs, im Gang (Sub 3, +3) nach vier. Original: Pistole +3, Schrot +10 bzw. +15 unabhaengig vom Zustand.
- Beleg: Overlay `80107130 lbu v1,5(s2)` / `80107138-40 lbu v1,-0x4f05(at)` (0x8010b0fb + v1); dieselbe Funktion prueft +5 gegen Waffen-Ids (@0x80106f58 0x10, @0x8010704c 1, @0x801070a0 9). Tabelle @0x8010b0fb gelesen: 8,1,3,3,3,10,15,5,8,10,8,8,3,2,20,2,2,20. Der Test test_birkin_bahnsteig.c:296 traegt die falsche Lesart mit (`Zuwachs 3`, Gang = Sub 3).
- Vorschlag: In case 2 den Index aus e->sub_state_1 lesen, bevor die Sicherung zurueckgeschrieben wird, Port-Waffen-Id ueber re15_re2z_weapon_id abbilden; die Wiederherstellung bleibt bei s_bb.s_sub. Test auf zwei Waffen statt auf zwei Unterphasen umstellen.
- Gegenprobe: Der Befund ist echt. Vier Widerlegungsversuche sind gescheitert; drei Details im Wortlaut sind zu berichtigen, der Kern bleibt.

WIDERLEGUNGSVERSUCHE
1. Ist +5 im Trefferzustand vielleicht doch keine Waffen-Id? Nein. Beide RE2-Applier schreiben sie: FUN_800410cc `sb (hi16+1),5` @0x80041aa8-b4 und FUN_800470c0 `sb s5,5(s1)` @0x80047324. Die alte Lage liegt in +0x1fc. Die Record-Zeile derselben Id trägt den Waffenschaden.
2. Überschreibt etwas +5 zwischen Applier und Handler? Nein. Im G4-Overlay gibt es nur zwei `sb ..,5(..)`, @0x8010098c und @0x801009e8, beide im Ctor-Bereich vor 0x80100bbc. Der Vortakt @0x80100134 berührt +5 nicht.
3. Läuft der 0x30-Treffer im Port vielleicht anders? Nein. re15_re2_stamp_hit überschreibt +5 nur für re15_re2z_owns_type; beim 0x30 steht dort die Waffen-Id (re15_damage.c:2796). birkin_bahnsteig.c:486/490 liest sie aber nicht, sondern indiziert mit s_bb.s_sub.
4. Tritt es im echten Spiel vielleicht nicht auf? Doch. Eigene Läufe der echten Exe mit identischem Eingabeskript und Zähler je Bild aus dem Prozessspeicher zeigen es. Redhawk zählt in Sub 3 wie die Pistole +3 (Original +10), in Sub 0 +8 (Original +10). Pistole in Sub 0 zählt +8 (Original +3), Remington in Sub 8 +8 (Original +5).

SPIELFOLGE
Ein Pistolentreffer im Biss löst bei Zähler 0 sofort das Zucken aus und bricht den Biss ab (F1231). Im Original wären das +3. Die Form kommt mit HP 150 (@0x80118094), Redhawk macht 50 je Treffer. Im Gang kommt das Zucken mit dem 5. Treffer, also nie vor dem Rückstoß. Im Original käme es nach 2 Treffern. Nur die Pistole im Gang stimmt, und das zufällig: gleicher Wert 3 bei Index 3 und bei Sub 3.

BERICHTIGUNGEN AM BEFUND
- "Schrot +10 bzw. +15" ist falsch beschriftet. Index 5/6 sind Magnum/Custom Magnum (+10/+15). Schrot ist Index 7/8 (+5/+8).
- Die Konstante 9 steht @0x8010708c. @0x801070a0 ist das `jal`.
- "Vier" und "sechs" gelten nur ohne Abbau. Bei 11 Ticks Abstand gemessen: 5 und 9. Der Abbau entspricht dem Original.

ZUM VORSCHLAG
Er trägt, mit zwei Ergänzungen.
- Die Tabelle braucht 19 Einträge: Index 18 = 4 @0x8010b10d. Port-w19 bildet auf Zeile 18 ab, der Port hat 18 Einträge und fiele auf 8 zurück.
- Alle Port-Schreiber von +5 liefern RE1.5-Waffen-Ids (re15_damage.c:2796, :3172, :4348), re15_re2z_weapon_id passt also für jeden Pfad.
- test_birkin_bahnsteig.c:296ff muss mit: Der Test schreibt sub_state_1 = 1 (Messer, +1), sieben Treffer ergäben dann kein Zucken mehr.

Keine Quelldatei geändert, keine Exe-Kopie übrig.
- Gegenprobe-Beleg: ORIGINAL (G4-Overlay, CDEMD0.EMS Sektor 3238, 0xB2D4 Byte, @0x80100000)
80106f54 lbu v1,5(s2) / 80106f58 li v0,0x10 / 80106f60 li v0,0xc: Vergleich gegen Waffen-Ids
80107130 lbu v1,5(s2) / 80107134 lbu v0,0x21d(s2) / 80107138 lui at,0x8011 / 8010713c addu at,at,v1 / 80107140 lbu v1,-0x4f05(at) / 80107148 addu v0,v0,v1 / 8010714c sb v0,0x21d(s2)
Bytes @0x8010b0fb: 08 01 03 03 03 0a 0f 05 08 0a 08 08 03 02 14 02 02 14 04

RE2-EXE (info/re2leon/PSX.EXE)
80041aa8 srl v1,t0,0x10 / 80041ab0 addiu v1,v1,1 / 80041ab4 sb v1,5(t0)
80047288 sw v0,4(s1) (Zustand 2, löscht +5) / 80047324 sb s5,5(s1)
Records @0x800a412c (Zombie), w0 & 0x3ff: Id 1 = 3, Id 2-4 = 16, Id 5/6 = 900, Id 7 = 200, Id 8 = 300: Id 5/6 = Magnum, Id 7/8 = Schrot.

PORT
birkin_bahnsteig.c:486 `uint8_t sub = s_bb.gesichert ? s_bb.s_sub : (uint8_t)BB_GEHEN;`
birkin_bahnsteig.c:490 `s_bb.meter + (sub < 18 ? k_zuwachs[sub] : 8)`
re15_damage.c:2796 `e->sub_state_1 = (uint8_t)weapon_id;`

SONDE (echter re15_player_weapon_fire + re15_enemy_ai_run_all, ROOM5090, Form-Bank, 11 Ticks Abstand)
Treffer bis Zucken, alle acht Waffen w3/w4/w7/w8/w13/w12/w19/w20 zeilengleich:
Sub 3 = 5, Sub 2 = 5, Sub 4 = 5, Sub 8 = 2, Sub 14 = 1, Sub 15 = 9
Soll nach Overlay-Tabelle und gleichem Abbau, unabhängig von der Phase: w3 5, w4 5, w7 2, w8 3, w13 2, w12 8, w19 4, w20 2.
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen_zuwachs/probe_zuwachs.c

EIGENE ECHTLÄUFE (echte Exe, rc=0, s_bb.meter je Bild aus dem Prozessspeicher, identisches Skript)
w3: F898 Sub 0 0->8 | F937 Sub 0 6->Zucken | F1050 Sub 3 0->3 | F1084 Sub 8 1->9 | F1121 Sub 3 7->10 | F1158 Sub 14 8->Zucken | F1231 Sub 14 0->Zucken | F1305 Sub 15 0->2
w7: F899 Sub 0 0->8 | F935 Sub 0 6->Zucken | F1047 Sub 3 0->3
w8: F898 Sub 0 0->8 | F1380 Sub 8 0->8
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen_zuwachs/w3/je_bild.txt
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen_zuwachs/w7/je_bild.txt
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen_zuwachs/w8/je_bild.txt

### M13 [mittel] Treffer auf die Form zeigen kein Blut

- Datei: re15_port/engine/src/birkin_bahnsteig.c:484-488 (case 2)
- Ablauf: Schuss trifft die Form, Zähler unter 12: nur Ton se(2), keine sichtbare Reaktion. EM30 davor und G5 danach spritzen sichtbar. Der Spieler kann nur am Ton erkennen, ob er trifft.
- Beleg: Bild 02_form_treffer_F1050_kein_blut.jpg gegen 03_em30_treffer_F714_blut.jpg. fx-Zähler je Treffer: Form 22-23, EM30 23-24. G4-Overlay Treffer-Zweig ruft den Effekt-Spawner `jal 0x8001bf10` @0x80106f9c, @0x80106fdc, @0x80107000, @0x80107040 (Dossier 7 führt ihn als fehlend, 'Partikel'); G5 hat ihn als re15_esp_fx_spawn_ex.
- Vorschlag: Im Treffer-Zweig dieselben Spawns wie enemy_ai_boss_g5.c:849-887 auf den G4-Teilen ausführen.
- Gegenprobe: Widerlegung gescheitert, der Kern stimmt: Treffer auf die Form erzeugen kein Blut. Zwei Stützen des Melders sind aber falsch und müssen vor einem Fix korrigiert werden.

WIDERLEGUNGSVERSUCHE:
1. Zweiter Blutpfad im Port: keiner. `re15_birkin_root` führt bei G4-Bank nur nach `re15_bb_tick` (enemy_ai_common.c:15010). `case 2` (birkin_bahnsteig.c:484-504) spielt nur `bb_se(2)` und zählt den Zähler hoch. `birkin_formen.c` und `birkin_bahnsteig.c` enthalten keinen ESP-Aufruf. `birkin_opfer.c:316` gehört zum Griff. `re15_re15_re2z_gore_hit` kehrt für Nicht-Zombies sofort zurück (Gate `re15_re15_re2z_import_owns`, enemy_ai_re2_zombie.c:7137).
2. Eigene Läufe im echten Fenster: Leon mit KI re15, Leon mit Vorgabe-KI re2, Elza in 5091 mit re2. Je 10 echte Treffer, 0 Blut.
3. Kontrolle, dass die Messgröße greift: EM30 11 von 11 (Lauf b1) und 60 von 60 (Lauf a1) mit id=0-Spawn. Gezoomtes Fenster: EM30 spritzt nach dem Treffer F745 bei F747-751 sichtbar rot, die Form nach dem Treffer F1050 zeigt nichts. Der Modellwechsel fasst die ESP-Bank nicht an.
4. Original: Der Einwand, die Bodenphase laufe über einen anderen Handler, trifft zu, kippt die Aussage aber nicht. Der dortige Handler 2 spawnt ebenfalls viermal Effekt 0.
5. Regression: Vor Spur R (6e3aa3289) lief die Form über `re15_birkin_ai_tick` mit dem TREFFER-BLUT-Block (@0x8011a144-1d8). Der Block ist noch da, wird für die Form aber nicht mehr erreicht. Der Nutzer sieht also: EM30 blutet, die Form direkt danach nicht.
Reaktion: 8 von 10 Treffern zeigen nichts außer dem Ton, 2 führen zum Zucken (F930, F1147). Das passt zu "Zähler unter 12".

KORREKTUREN AM BEFUND:
(a) "G5 danach spritzt sichtbar" ist FALSCH. In 37 Läufen mit G5-Zähler steht `blut=n/0/0/0`: Der Trefferzweig wird betreten (n bis 32), `g5_treffer_blut` aber nie gerufen. Lauf a2: 22 G5-Treffer, 0 id=0. Ursache vermutlich die Wache `if (g->ph == 0 && g->sub != 0xF)` (enemy_ai_boss_g5.c:1612), denn beim Treffer stand ph in den Stichproben auf 1-4. Das ist ein eigener Fehler außerhalb dieses Befunds, die Ursache habe ich nicht weiter verfolgt.
(b) Damit taugt enemy_ai_boss_g5.c:849-887 nicht als Vorlage (zudem Bahnsteig-Klemmen auf Z und Korridor-Wache). Vorlage ist Handler 2 oder der EM30-Block enemy_ai_common.c:12488-12496.
(c) Die zitierten `jal`-Adressen @0x80106f9c ff. liegen in Handler 1 (Phase 1). Die Form spielt die Bodenphase: `sh 1,+0x10e @0x8010154c` im Clip-10-Eintritt T0[6], den der Port als AUFTRITT nachbildet. Richtig sind @0x80107454/94/b8/f8 in Handler 2 @0x80107280.
(d) Nebenbei: Handler 2 spielt se(0xd), nicht se(2), und hat keinen Vergleich auf 12, sondern Absprünge 0xb01/0x501. Das ist nur gelesen, nicht weiter geprüft, gehört also nicht zu diesem Befund. Auch `case 2` folgt damit Handler 1.

Einschränkung: Die Form wurde per Haken `RE15_BIRKIN_HP_AT=705` erreicht, der Form-Tick danach ist der normale. Lauf a1 erreichte die Form nicht: Bei Dauerfeuer während Clip 20 startete die Mutation immer neu. Für den EM30-Vergleich ist er gültig. Keine Quelldatei geändert, nichts committet, Arbeitsbaum sauber.
- Gegenprobe-Beleg: EIGENE LÄUFE (Fx-Log je Partikel, state.log, echtes Fenster), Verzeichnis C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gp_blutform/
- a2 Leon re15: Form-Treffer F895, 930, 1050, 1085, 1116, 1147, 1209, 1267, 1302, 1333, HP 150->0, id=0-Spawns 0. fx.log gesamt: ids 2/3/4/8, keine id=0-Zeile. Dazu 22 G5-Treffer, ebenfalls 0.
- a3 Leon re2 (Vorgabe): dieselben 10 Treffer, 0 Spawns.
- a4 Elza 5091 re2: dieselben 10 Treffer, 0 Spawns.
- F715 (300->0) ist in a2/a3/a4 der Haken, kein Treffer.
- b1 ohne Haken: EM30 11 Treffer, 11 mit id=0-Spawn (je 11 Bilder sichtbar, z.B. F745-755 bei sx=132 sy=73, Sprite rund 25 px breit im 320er-Raster).
- a1: EM30 60 von 60.
BILDER (Vergrößerung aus fenster.mkv):
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gp_blutform/zoom_em30_F745.png
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gp_blutform/zoom_form_F1050.png
Auswertung: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gp_blutform/ana_blut.py

ORIGINAL (CDEMD0.EMS rec 1, Sektor 3238, @0x80100000, selbst disassembliert):
- Handler-Wahl @0x80106d44-64: `lhu v0,0x10e(a0)` / `andi v0,v0,1` / Tabelle @0x8010b110 = {0x80106d7c, 0x80107280}.
- Bit setzen: `addiu v0,zero,1` @0x80101548 / `sh v0,0x10e(s2)` @0x8010154c.
- Bodenphase, Handler 2: Wache `lbu v0,6(s2)` / `bnez` @0x801072ac-b4. Überspringen bei +5 == 0x10 @0x80107414 oder 0xc @0x8010741c.
- Spawn 1: `addiu v0,v0,0x17d0` @0x80107434, `lui a0,1` @0x80107438, `jal 0x8001bf10` @0x80107454 (Effekt 0, Sub 1, Skala 0x17d0 + (rng&0xff)*8).
- Spawns 2-4: `jal 0x8001bf10` @0x80107494, @0x801074b8, @0x801074f8 (Sub 0).
- a1 = atan2 @0x801072d8, a2 = 0x8009db44 @0x80107448-4c, a3 = sp+0x10 bzw. sp+0x18.
- Anker: `lw v1,0x714/0x718/0x71c(s1)` @0x80107390-b8 = Translation Teil 10 (+0x700 = Teil 10 + 0x48, +0x14).
- Ton Handler 2: `addiu a0,zero,0xd` @0x8010750c, `jal 0x8005bd6c` @0x80107510.
- FUN_8001bf10 ist der ESP-Spawner (a0>>24 = Effekt; RE2_Quellcode_V2/FUN_8001bf10.c, Beschreibung auch in enemy_ai_boss_g5.c:721-731).

PORT:
- birkin_bahnsteig.c:484-504 ohne Spawn.
- enemy_ai_common.c:15010 `if (re15_bb_bank_da(e)) re15_bb_tick(s);`, im Stand 6e3aa3289 unbedingt `re15_birkin_ai_tick(s);`.
- EM30-Blut enemy_ai_common.c:12488-12496.
- G5-Wache enemy_ai_boss_g5.c:1612.
- Dossier R_bahnsteig.md Abschnitt 7 führt "Partikel 0x8001bf10" selbst als fehlend.
- G5-Zähler, Beispiele: a2 `blut=19/0/0/0` im Lauf und 22/0/0/0 am Ende; gegen/e3 `blut=32/0/0/0`; f7_form `blut=28/0/0/0`.

### M14 [mittel] Spieler kann gegen die Form nicht sterben, bei niedriger HP Griff-Schleife

- Datei: re15_port/engine/src/birkin_bahnsteig.c:197-198, 394-395
- Ablauf: Form greift weiter an, HP läuft auf 0. hp 0 ist noch nicht tot (Tod erst bei hp<0), der nächste Treffer klemmt von -10 auf 1. Ohne Tasten-Mash wechselt der Griff dann alle rund 265 Bilder, dazwischen liegt der Spieler rund 225 Bilder, frei sind nur 7-40 Bilder. Kein YOU DIED gegen die Form, der Kampf kann in dieser Phase nicht verloren werden. Dossier Abschnitt 6 nennt die Klemme, begründet sie aber nur mit dem 0x30.
- Beleg: f5_werfer: hp 10 -> 0 F1710, hp 1 ab F1950, Griffe F1939/F2200/F2460/F2728, bis F3000 kein Tod. RE2-Griff kann töten: `jal 0x800401d4` @0x801052b8 mit 70, Todeszweig @0x801052c0-fc. G5 tötet regulär (f17_tod F2760 hp -1).
- Vorschlag: Entscheiden und umsetzen: Griff und Biss dürfen wie RE2 töten, oder der Griff wird unter 15 HP gesperrt. Das Klemmen auf 1 ist für diese Form kein Auftrag.
- Gegenprobe: Widerlegung gescheitert, der Befund stimmt im Kern.

Code: birkin_bahnsteig.c schreibt die Spieler-HP nur an zwei Stellen (197-198, 394-395), beide klemmen hp<0 auf 1. Tod heisst hp<0 (re15_player_is_dead). Der Griff laeuft ueber re15_bk_opfer_start(e,6,0), in Stage 5 also Haken [0] ohne Todesvariante (STAGE5 @0x8011afd8, Tabelle @0x80120468). Der Wurf-Tod haengt an cmd 6 nur in STAGE3 (Tabelle @0x8011f4b0).

Einwand 'wie die 0x30-KI' traegt nicht:
- Die Klemme ist fuer den 0x30 zwar Original (STAGE5 @0x80118024, @0x801184a8, @0x8011934c). Das im Dossier zitierte @0x80117810 ist die STAGE3-Adresse.
- Das Griff-Gate hp<15 stammt aber aus RE2 (`slti v0,v0,0x1e` @0x801024b4). Dort ist dieser Griff immer toedlich: 70 Schaden @0x801052b0, FUN_800401d4 liefert 2 unter -14, Todeszweig @0x801052c4-d0.
- Der Port hat das Gate uebernommen, die Toedlichkeit nicht. Die Schleife entsteht aus dieser Mischung. Der 0x30 hat im Port kein HP-Gate im HUB (enemy_ai_common.c:12136-12154), und in RE2 endet der Griff toedlich.

Einordnung: Das ist kein Geschmack. Der Nutzer hat denselben Fehlertyp in Runde 41 selbst gemeldet (AUFTRAG.md:50 'Der Spieler kann nicht sterben ...'), Nachtrag 38 nennt RE2 als Vorbild. Das Dossier erwaehnt die Klemme, begruendet das Griff-Gate aber nicht.

Korrekturen am Befund:
- Ohne Mash sind die freien Fenster 4-32 Bilder statt 7-40, ab dem 5. Griff bei hp 1 konstant 4.
- Ein Standbild-Stillstand ist es nicht, mit Mash bleiben Fenster. Ob der Kampf aus der Schleife noch gewinnbar ist, habe ich nicht gemessen: Mein Skript dreht nicht zur Form, ihre HP blieben 150.
- Gegenprobe-Beleg: Eigene Laeufe mit Standard-KI, Exe des Zweigs r41/bahnsteig, Leon 5090, Debug-Sprung, Werfer bis zur Mutation wie f5_werfer. Logs unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gp_griff/ (steh, mash, dauermash, em30; Auswertung auswert.py, frei.py).

steh (keine Eingabe, bis F6300):
- hp 10 -> 0 bei F1704, 0 -> 1 bei F1940, danach dauerhaft 1, 0 Bilder mit hp<0.
- Griff-Eintritte F1936, F2194, F2455, F2720, F2956, dann alle 236 Bilder bis F6260.
- Je Zyklus 232 Bilder liegend, frei 26/29/32, dann konstant 4.
- Griffbild 8 bei dist 2187-2440.

mash (Mash 60 % der Zeit, dazu Zielen und Schuss, bis F6300):
- Griff alle 199 Bilder, 159-174 liegend, 25-50 frei.
- hp bleibt 1, kein Tod, Form-HP 150.

dauermash (nur Mash, bis F3600): Griff alle 199 Bilder, 145 liegend, 53-67 frei, hp 1.

em30 (Vergleich, stehen, ohne Mutation, bis F5400): kein einziger Griff, hp 90, nur Biss-Sub 4 ohne Kontakt.

Alter Lauf f5_werfer (Flavor re15) zeigt dasselbe: Griffe F1939/F2200/F2460/F2728.

Disasm:
- STAGE5 @0x80118004-24: `addiu v0,v0,-10`, `bgez`, `ori v0,zero,1`, `sh`.
- G4-Overlay @0x801052a8-b8: `slti v0,v1,0xab`, `addiu a0,zero,0x46`, `jal 0x800401d4`.
- @0x801052c4-d0: `bne v0,v1` (v1=2), aca58 = 0x206.
- Biss 20 @0x8010501c, Hieb 15 @0x80101dcc haben denselben Todeszweig.

G5-Tod naturgemaess: f17_tod F2723 hp 1 -> -1.

### M15 [mittel] Griff von hinten schleudert den Spieler auf die Form zu

- Datei: re15_port/engine/src/birkin_bahnsteig.c:388-397 (bb_griff)
- Ablauf: Spieler kehrt der Form den Rücken zu, Griff greift bei Bild 8. Der Haken cmd 6 schiebt ihn entlang seiner eigenen Rückwärtsrichtung; die Spieler-Gier wird anders als beim Hieb @0x80117790-a8 nicht gesetzt. Er fliegt zur Form hin statt weg und landet in einem Bild neben ihr.
- Beleg: f18_vorbei und f18_vorbei2 gleich: F1239->F1240 PL (-975,-24506) -> (27,-25121) d=1175, r2048, Form bei (141,-23672). Bild 07_griff_von_hinten_F1232-1253.jpg.
- Vorschlag: Vor re15_bk_opfer_start die Spieler-Gier so setzen, dass der Rücken von der Form wegzeigt, wie der Gier-Latch @0x80117754-a8.
- Gegenprobe: Die Widerlegung ist gescheitert, der Befund ist echt. Steht Leon mit dem Rücken zur Form, wirft ihn der Griff in die Form hinein statt von ihr weg. Ursache im Code: bb_griff (birkin_bahnsteig.c:391) ruft re15_bk_opfer_start(e, 6, 0), ohne pl->rot_y zu setzen. Haken [0] schiebt danach mit vorschub(pl, 0x800) entlang Leons eigener Rückwärtsrichtung (birkin_opfer.c:182). Ob der Griff ihn weg- oder hinwirft, hängt also nur an seiner Blickrichtung.

Drei Widerlegungsversuche, alle gescheitert:
1. In f18_vorbei steigt der Abstand zur Wurzel sogar (1393 -> 1453). Das ist ein Sonderfall: Leon stand an der Westwand, daher ph 0->5 schon im ersten Tick. Der Hakenschritt ging auf (75,-24506), d = 836, und der Körperschub drückte ihn auf den Kreis 1450 zurück. In den eigenen Läufen sinkt d klar.
2. Das Original setzt bei cmd 6 ebenfalls keine Gier. Das stimmt, aber der Wurf-Zweig folgt dort immer auf den Griff-Schwanz (aca58 == 5), also auf den Gier-Latch des Hiebs. bb_griff trifft dagegen einen freien Spieler mit beliebiger Gier. Die Vorlage wird also außerhalb ihrer Vorbedingung benutzt.
3. RE2 zieht Leon beim Griff ebenfalls heran. Das Dossier §6 beschreibt aber bewusst einen Rückstoß ("Leon fliegt"), und der Lauf zeigt kein Halten, sondern den Flug durch den Körper auf die andere Seite der Form.

Erreichbarkeit: Die Griff-Bedingung hängt nur am Blick der Form (Kegel 256, dist < 0xed8) und ist der erste Angriff (biss_rest 0). Es genügt also, sich zum Fliehen wegzudrehen.

Präzisierung des Befunds: "landet in einem Bild neben ihr" gilt nur für den Wandfall f18. Im offenen Gang erreicht Leon den Körperkreis in 2 Bildern und wandert dann 13 Bilder darauf entlang. An der Südwand fliegt er in 3 Bildern an der Form vorbei. Der Schritt von 1050 im ersten Bild ist kein eigener Fehler: 0x4b0 @0x8011b090 minus 150, er kommt auch mit Blick zur Form vor. Der Defekt ist allein die Richtung.

Zum Vorschlag: Der Latch @0x80117754-a8 belegt nur den Mechanismus. Sein Wert ist quer zur Peilung (Peilung +-0x400), nicht "Rücken weg".
- Gegenprobe-Beleg: Stand: Exe des Baums r41_bahnsteig, keine Quelldatei neuer als die Exe. master ist gleich (git diff 4ee345aaa master leer für birkin_bahnsteig.c, birkin_opfer.c, birkin_formen.c). Alle Läufe mit RE15_BIRKIN_HP_AT=700, Griff trifft in r1 bis r4c bei F951, in r5 bei F1028.

Eigene Läufe:
- r4 (nur Pad-Eingaben, kein Speicher-Eingriff: L0.37, dann stehen): rot -9, Blick*Peilung -0.94, Form (5951,-23071). PL x 8243 -> 7193 -> 6476 -> 5597, d 2463 -> 1529 -> 1045 -> 970 (unter dem Körperkreis 1450). Ende (4819,-22158), also auf der Westseite der Form. r4b (Wiederholung) und r4c (Flavor re2, der Standard) liefern dieselben Eckwerte, r4c auch dieselbe Bahn Bild für Bild.
- r1 (offener Gang, Lage einmal bei F900 gesetzt): rot 0, Blick*Peilung -1.00, d 2958 -> 1909 -> 1453, dann F953-F965 auf dem Kreis um (5942,-23052) bis (5813,-24504).
- r2 (Kontrolle, gleiche Stelle, rot 2048): d 2958 -> 4008, Ende (15500,-23100), d 9558.
- r3 (Blick*Peilung -0.10, seitlich): d 1463 -> 3027.
- r5 (im Gehen, Blick*Peilung -0.91): d 1701 -> 1663 -> 1847, Abstand bleibt ungefähr gleich; dieser Fall bestätigt den Befund nicht.
- Haken-Zeile r4: "cmd 6 var 0 ph 0->1 ... pos (7193,-22180) gier -9".
- Haken-Zeile f18_vorbei: "ph 0->5 ... pos (75,-24506) gier 2048".

Original:
- STAGE3 @0x80117eb0-efc (cmd 6): kein Schreiber auf acabe, nur acbfc, acbcc/d0, aca58 = 6, acc1c/e.
- STAGE5: acabe-Schreiber @0x80117fbc (Hieb), cmd 6 @0x801186f8 ohne Gier-Schreiber dazwischen.
- Sub 5 (STAGE3): 0x501 @0x80117150 nur nach aca58 == 5 (bne @0x80117124); 0x801 @0x80117174 -> 0x501 @0x80118c4c / @0x80118cb4.
- RE2 G4 @0x80105358-68: Spieler+0x15a = Form-Gier + 0x800. Das passt zu "Gesicht zur Form", aber der Leser von +0x15a ist nicht gefunden (EXE und CDEMD0.EMS gescannt). Dieser Punkt ist also kein Beweis, und auch das Einlesen der Spieler-Gier im RE2-Griff ist nicht belegt.

Testlücke: test_birkin_bahnsteig.c:246-251 prüft nur Griff und Schaden, keine Richtung.

Bilder und Logs:
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_griff\r4_zoom.jpg
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_griff\r1_zoom.jpg
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_griff\r4_rein_steh\state.log
C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s40\abnahme_r\gp_griff\r4_rein_steh\debug.log

Regelabweichung: Ein Tippfehler von mir legte kurz die leere Datei /tmp/x.py außerhalb des erlaubten Ordners an. Sie ist gelöscht. Quellen und Hauptbaum sind unberührt, die Exe-Kopien sind entfernt.

### M16 [gering] Keine Treffer-Effekte auf der Form: Blut der 0x30-Phase und G4-Partikel fehlen

- Datei: re15_port/engine/src/birkin_bahnsteig.c:484-504 (case 2)
- Ablauf: In der Original-Birkin-Phase kommt das Treffer-Blut aus re15_birkin_ai_tick (re15_esp_fx_spawn_ex, enemy_ai_common.c Zustand 2). re15_bb_tick ersetzt diesen Tick und ruft nichts Entsprechendes. Schuesse auf die Form zeigen also keine Treffer-Wirkung ausser dem Ton, direkt nach einer Phase, in der es Blut gab. Das Dossier nennt die Partikel in Abschnitt 7 als Bestand fuer ein volles Overlay, aber nicht als sichtbaren Rueckschritt im Kampf.
- Beleg: Einzige Birkin-Blut-Aufrufe enemy_ai_common.c `re15_esp_fx_spawn_ex(... 0x3000 ...)` in Zustand 2 und im Mutations-Waechter; in birkin_bahnsteig.c kein Effekt-Aufruf. Original-Treffer @0x80106f9c, @0x80106fdc, @0x80107000, @0x80107040: vier `jal 0x8001bf10` am Teil (+0x668..+0x670) Richtung Spieler.
- Vorschlag: Fuer die Form mindestens denselben Blut-Aufruf wie case 2 von re15_birkin_ai_tick setzen, Teil aus dem G4-Handler (Rumpf +0x668), Skala nach Abstand wie @0x8011a120-130.
- Gegenprobe: Der Befund ließ sich nicht widerlegen: Im echten Ablauf zeigen Treffer auf EM30 den Effekt 0, Treffer auf die Form nicht.

Geprüft habe ich drei Lauf-Wiederholungen (e4, e5, e6) mit gleichem Ablauf: Tür, Kampfstart, Magnum, Feuerpause während der Mutation. Die Ergebnisse sind in allen drei gleich.

**Code:** `re15_bb_tick` Zustand 2 ruft nur `bb_se(2)` und den Zähler. Auch im Schadenspfad (`re15_damage.c`, Hitscan-Zweig ab `dmg_row`, Zeile 2811ff) gibt es für 0x30 keinen Effekt-Aufruf. Einen Ersatzpfad habe ich nicht gefunden, und der Lauf zeigt auch keinen.

**Original:** Die vier zitierten `jal 0x8001bf10` stimmen. Die zweite Treffer-Variante `@0x80107280` hat ebenfalls vier davon. Im Dossier oder im Test steht keine Festlegung gegen Blut.

**Korrekturen am Befund:**
1. "Keine Treffer-Wirkung außer dem Ton" ist zu weit. Ab Zähler 12 zuckt die Form (Clip 8, gemessen bei F1279). Es fehlt der Effekt, nicht jede Reaktion.
2. Für den Vorschlag gilt: `+0x668` ist Teil 9 (9*0xAC + 0x5C), nicht `BB_TEIL_RUMPF` 10.
3. Der Mutations-Wächter spawnt mit 0x2000, nicht mit 0x3000.

**Schwere:** "gering" bestätigt. Es gab keinen Absturz, und der Kampf lief bis zum G5 durch.

**Grenzen der Messung:**
- Der echte Ablauf lief nur mit Magnum (4 Treffer auf die Form).
- Der Lauf e3 mit `RE15_BIRKIN_FORM` taugt nicht als Beleg, weil die Form dort HP 0 hat und der erste Treffer sofort in die Rückstoß-Szene führt.
- Der G4 selbst lief nicht live, der Beleg dort ist die Disassemblierung.

**Nebenbefunde außerhalb dieses Befunds:**
- Mutations-Schleife: In e2 (Magnum, Treffer im 27-Bilder-Takt) startet Clip 0x14 endlos neu, weil HP < 100 und `re15_bf_mutation_offen()` bis zum Clip-Ende den FORCE-Zweig (`enemy_ai_common.c:12634`, seit 2341475d0) erneut auslösen. HP läuft 150, 100, 50, dann Reset; die Form wurde bis F2700 nicht erreicht.
- G5: Auch dort kommt kein Blut. In e4 meldet `birkin_dbg.log` `blut=12/0/0/0`, also 12 Treffer und 0 Aufrufe von `g5_treffer_blut`. Die Wache `g->ph == 0` (`enemy_ai_boss_g5.c:1612`) blockte dort jedes Mal.
- Gegenprobe-Beleg: **Messlauf:** eigene Exe-Kopie, Worktree `r41_bahnsteig`, Exe-Stand 02:04:15 (neuer als alle Quellen). Umgebung: `RE15_DEBUG_JUMP=5090@gp`, `RE15_GIVE=7:99`, `RE15_EQUIP=7`, `RE15_FX_LOG`, `RE15_STATE_LOG`. e4, e5 und e6 liefern bildgleich dieselben Treffer.

| Phase | Treffer in Bild | Zeilen `id=0` in `fx.log` | aktive Effekt-Plätze (`fx=`) |
|---|---|---|---|
| EM30, F511-954 | 511, 619, 646, 754, 781 | 55 | 17 -> 22 |
| Form, F955-1345 (Clips 10/27/5/26/8 ab F954) | 1225, 1279, 1306, 1333 | 0 | 17 -> 21 |

Die Form hat je Treffer genau einen Platz weniger. Das misst den Spawn und ist vom Regions-Cull des Zeichners unabhängig.

**Bild:** `RE15_FRAMEDUMP` zeigt am EM30 ab F754 über 11 Bilder einen hellen Ausstoß, Effekt 0 bei w(3762,-2315,-23683). An der Form zeigen F1279-1291 nichts am Modell, nur das Zucken.

Dateien:
- C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen/bilder/em30_zoom.png
- C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen/bilder/form_zoom.png
- C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen/e4/fx.log
- C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegen/e4/state.log

**Port:** `enemy_ai_common.c:12493` spawnt in Zustand 2 `re15_esp_fx_spawn_ex(room_bank, 0, 0, dist>=0xfa1 ? 0x2400 : 0x3000, ...)`. `birkin_bahnsteig.c` enthält keinen Aufruf von `re15_esp_fx_*` (Grep leer).

**G4-Overlay** (`CDEMD0.EMS` Sektor 3238, selbst disassembliert): Zustand 2 `@0x80106d3c`, Tabelle `@0x8010b110` = {0x80106d7c, 0x80107280}.
- `@0x80106d7c`: Wache `lbu v0,6(s2)` / `bnez` bei `@0x80106da8-b0` (nur Phase 0), `lw s1,0x198(s2)` `@0x80106dd0`, `lw v1,0x668/0x66c/0x670(s1)`.
- `@0x80106f54-64`: Wache `+5 != 0x10` und `!= 0xc`.
- `@0x80106f7c-84`: `a0 = (rand&0xff)*8 + 0x17d0 | 0x10000`, also Effekt 0, Sub 1.
- Vier `jal 0x8001bf10` bei `@0x80106f9c`, `@0x80106fdc`, `@0x80107000`, `@0x80107040`, Bytes `c4 6f 00 0c`.
- Variante `@0x80107280`: vier weitere bei `@0x80107454`, `@0x80107494`, `@0x801074b8`, `@0x801074f8`.

`RE2_Quellcode_V2/FUN_8001bf10.c` belegt den Spawner: Effekt = `a0>>24`, Sub = `(a0>>16)&0xff`, Skala = `a0<<16`.

### M17 [gering] Doppelschlag ruckt die Form in einem Bild 544 bis 701 Einheiten

- Datei: re15_port/engine/src/birkin_bahnsteig.c:318 (bb_clip nullt die Spur)
- Ablauf: Beim Wechsel in den Gegen-Clip mitten im Clip setzt bb_clip spur_x/spur_z auf 0. Der nächste bb_spur nimmt den kumulierten Wert des neuen Clips als Einzelschritt. Sichtbar als Ruck im Hieb.
- Beleg: f7/f9 F1332 (8768,-22718)->(8332,-23044) d=544; f18_vorbei und f18_vorbei2 F1129 d=701; f16_b F1780 d=654. Das Overlay ruft nach dem Clip-Wechsel FUN_80015e7c zweimal: `jal 0x80015e7c` @0x80101c4c und @0x80101c60, damit die Differenz 0 ist.
- Vorschlag: Nach dem Clip-Wechsel im Doppelschlag bb_spur einmal ohne Schieben aufrufen.
- Gegenprobe: Der Befund ist echt, die Widerlegung ist in allen Punkten gescheitert. Mechanismus: bb_clip nullt spur_x/spur_z (birkin_bahnsteig.c:125, aufgerufen in :318). Der Einstieg ist Bild 7 bzw. 20, also greift der Null-Zweig fuer Bild 0 in :174 nicht, und :175 nimmt den kumulierten Spurwert als Einzelschritt.

Das Original tut das nicht. Nach dem Setzen des Clip-Worts ruft es FUN_80015e7c zweimal (@0x80101c4c, @0x80101c60), der zweite Aufruf ergibt +0x144 = 0. Zwischen @0x80101c68 und @0x80102350 schreibt niemand mehr +0x144/+0x148, und FUN_800152c8 (@0x80102348) wendet die 0 an.

Eigene Laeufe: In meinem Fensterlauf mit unveraenderter Exe springt die Form in F1332 um 544,4 Einheiten (davor 71, danach 127). Eine Kontroll-Exe, die nur den Originalablauf ergaenzt (bb_spur zweimal nach dem bb_clip des Doppelschlags), laeuft bis F1331 lagegleich und zeigt in F1332 0,0.

Im Bild kommt der Ruck vom Lagesprung, nicht vom Posenwechsel: Am Uebergang F1331->F1332 versetzt sich die Form um 12 px bei Skala 0,88, in der Kontrolle um 0 px bei Skala 1,00. Nutzer-sichtbar bestaetigt, Schwere gering ist stimmig (ein Bild Ruck, kein Abbruch).

Geprueft und verworfen: Glaettung der Weltlage beim Zeichnen gibt es nicht. Der Fussanker gilt nur fuer Zustand 4, die Form laeuft in Zustand 1.

Korrekturen am Befund:
1. "f16_b F1780 d=654" ist falsch zugeordnet: Der Wert steht in f15_ost, in f16_b laeuft bei F1780 schon der G5.
2. Der reine Sprung betraegt 544 bzw. 680. Die 654 entstehen durch eine Klemme, z bleibt -22718. In den 701 steckt ein zusaetzlicher Schub von rund 157 Einheiten im selben Bild, dessen Ursache ich nicht geprueft habe.
3. Der Ruck braucht einen Hieb. Steht Leon still und nah, kommt keiner (mein Lauf gui1: nur Griff, Biss, Weichen). Weicht Leon aus und schiesst, kommt er.
4. Vorschlag: Ein bb_spur reicht rechnerisch, weil bb_clip die Spur nullt. Gelaufen ist aber nur die Variante mit zwei Aufrufen wie im Original.
- Gegenprobe-Beleg: Original roh: FUN_80015e7c in info/re2leon/SLUS_007.48: `bnez v0` @0x80015f0c ueberspringt das Nullen bei +0x14d != 0, `subu v1,t1,v1` @0x80015fcc + `sh v1,0x144(t0)` @0x80015fd8 schreiben neu minus alt, `sw v0,0x20c(t0)` @0x80015fc4 legt den neuen Wert ab. Overlay (CDEMD0.EMS Sektor 3238): `sw v0,0x14c(s2)` @0x80101c50 im Delay-Slot von `jal 0x80015e7c` @0x80101c4c, zweites `jal 0x80015e7c` @0x80101c60, `jal 0x800152c8` @0x80102348.

Asset BAHNSTEIG_G4.EMD: Clip 4 Bild 7 Spur (608,-305), Laenge 680,2; Clip 3 Bild 20 Spur (-495,-225), Laenge 543,7.

Eigener Fensterlauf mit unveraenderter Exe (ROOM5090, Ausweichen + Schuesse), Spielbilder F1331->F1332: gui2 (8768,-22718)->(8332,-23044) d=544,4; gui2k (Kontroll-Exe) (8768,-22718)->(8768,-22718) d=0,0.

Sonde mit 8 Hieben, 8 Wechseln, 7 messbar: IST 7 Spruenge >300, groesster 681, sonst im Hieb max 233; Kontrolle 0 Spruenge, groesster 0.

Alte Logs mit meinem Skript: 12 Wechsel, alle mit Sprung 544-701 gegen Median 52 je Bild im Hieb.

Fenster 640x480, Template-Matching: Uebergang F1331->F1332 IST dx -12,0 dy -4,5 Skala 0,88; Kontrolle dx 0,0 dy 0,0 Skala 1,00. Aufnahmen bis dahin gleich (d um 0,13), danach d um 1,6. Projektion mit der echten Kamera Schnitt 12: -16,4 px gegen 0,6-5,2 px in den Bildern davor und danach.

Dateien unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r/gegenprobe_doppelruck/ : gui2/state.log, gui2k/state.log, gross.png, probe_ist.txt, probe_kontrolle.txt, tm2.py, ruck2.py

## In der Gegenprobe verworfen

- Leon steht im Vorderkörper der Form, Oberkörper verschwindet