# Abnahme 3 der Spur R: Rest nach zwei Runden

Stand 1ecd2ed01. M1 bis M17 sind erfuellt. Offen sind die folgenden Punkte der Abnahme 2.

## Zu beheben

### N1 [blocker] G5 und Arme schieben den Spieler durch die Wand, er bleibt dauerhaft eingesperrt

- Ablauf: Leon steht im Westen des Wagens (x 2000 bis 4600) und schießt nicht, oder ein Griff wirft ihn in die Südwest-Nische. Die Arme (Kind-Kollision r 800 + 450, Port tent_kind_kollision) und der Körper (Segment r 6000 + 450, re15_g5_body_push_player) schieben ihn nach der SCA-Klemme, Original-Reihenfolge, im Gang aber ohne Platz. Er landet in Box 30, Box 29 oder Box 22. Die Klemme setzt ihn auf z -27668 (Südseite Box 29: -27200 - 450 - 18) in die massive Box 22. Dort wirkt keine Eingabe mehr. Die Pistole trifft den G5 nicht, der G5 trifft ihn nicht: Softlock.
- Beleg: p_falle (Leon) und p_falle_e (Elza), Pistole, kein Haken: ab F2939 z -27668, von F3106 bis Laufende F5000 Dauerfeuer, Magazinzähler 213 -> 127, G5 HP 600 unverändert, Spieler HP 35 unverändert. Ohne Bewegung des G5 steht er bis Laufende auf (14953,-27668). Eingaben: z_aus_a und z_aus_c (Laufen, Drehen, Rennen, Spielerlage nur vom G5 verändert, am Ende Laufanimation auf der Stelle), z_w3b und z_w3c (Leon und Elza in Box 22 bei (-360,-26997) ab F1023, nach dem G5-Tod noch bis F2400, keine Bewegung). z_w4, z_w4b, z_w4c, v_w4: 3 von 3 plus Sichtlauf, Schritt 2100 in F1979. Raketen treffen von dort noch (z_aus_b, G5 tot F2150). k_a (Lage gesetzt): ebenfalls Box 22. Häufigkeit: nicht in 16 freien Läufen, nicht in 4 Läufen im Westen mit Dauerfeuer (p_f_a bis d), dort schiebt der Körper ihn durch den Gang nach Osten. Bild: l2_p_falle.jpg, l2_v_w4_b.jpg (Leon steht außen an der Wand).
- Vorschlag: In 5090/5091 nach Körper- und Arm-Schub den Spieler erneut gegen SCA klemmen, von der letzten gültigen Lage aus, und als Gestaltung markieren. Riegel: kein Bild mit Spieler-Mittelpunkt in Box 22, 29, 30 länger als ein Bild, Integrationslauf mit Leon im Westen ohne Feuer. Der Port-Pin mit Endstand 0 deckt diesen Fall nicht.

### N2 [hoch] Arm nagelt Leon für 12,8 Sekunden in der Südwand fest

- Ablauf: Leon steht an der Südwand bei x rund 7000 bis 8000, ein Arm kommt und bleibt liegen. Leon wird je Bild nach z -25290 in Box 29 gedrückt. Er ist hinter der Wandmaske kaum zu sehen. Eingaben wirken nicht, der Schaden bleibt aus, bis der Arm abzieht.
- Beleg: z_fern, z_wand_a, z_wand_b: Box 29 F1187 bis F1573 (388 Bilder). Eingaben ab F1250: Vorwärts, Rückwärts, Drehen, Rennen, Lage bleibt (6865,-25290), ein Sprung F1281 wird im nächsten Bild zurückgesetzt. Danach frei. Bild: l2_z_fern_wand.jpg, l2_z_wand_b.jpg.
- Vorschlag: Dieselbe Klemme wie beim Blocker. Der Hinweis im Dossier, die Klemme des Folgebilds löse das, stimmt im echten Lauf nicht.

### N3 [hoch] Kamera Cut 13 steckt im G5, Bild über 50 Prozent verdeckt, Tod und Fressen nicht zu sehen

- Ablauf: Der G5 schiebt den Spieler ostwärts über x 14000, die Zone schaltet auf Cut 13. Diese Kamera steht bei (11050,-3110,-24812), RID @0x200, und blickt nach Osten. Der G5 steht dann um x 10000, also auf der Kamera. Vom Spieler sieht man nur die Beine oder nichts. Die Lage entsteht ohne Fehlbedienung, auch im Pistolen-Dauerfeuer aus dem Auftrag.
- Beleg: p_leon_pistole und p_elza_pistole ab F2773 bis zum Versinken rund F3680 (G5 tot F2903): 57 bis 58 Prozent des Bildes weichen von der Referenz ab, l2_g5_cut13_ref.jpg, l2_g5_cut13.jpg, l2_p_lang_kadaver.jpg. Getippt ab F2550, l2_t_tipp_g5.jpg. g_leon: 1430 von 2875 G5-Bildern. v_g5: Fressen F3590 bis F3680 nur G5-Fläche, kein Spieler, l2_v_g5_tod.jpg. Freies Spiel 5 von 16 Läufen, 202 bis 641 Bilder. Griff der Form bei G5-freiem Cut 13 kurz ähnlich: r_fern F841 bis F865, l2_s_griff2.jpg.
- Vorschlag: Gestaltung entscheiden: Zonenwechsel 12 -> 13 sperren, solange der G5 lebt und x über rund 6500 liegt, oder den G5 am Vorrücken über die Kamera hindern, oder Cut 12 für den Bereich bis x 15400 halten. Riegel gegen die RID-Lage.

### N4 [mittel] Regression: Beim Wechsel vom Weichen in den Hieb springt die Pose für ein Bild

- Ablauf: Leon steht, die Form weicht zurück, bei dist >= 2501 geht sie in den Hieb. bb_weichen löscht dabei Bit 0x80 (birkin_bahnsteig.c:329), der Tick endet ohne neues Bild. Der Zeichner nimmt Clip 5 dann vorwärts statt rückwärts, also Slot N statt 88-N. Das sieht man als Ruck der ganzen Form direkt vor dem Hieb. Bei stehendem Spieler passiert es praktisch bei jedem Weichen, in der Sonde alle rund 160 Bilder. Dieselbe Zeile steht am Uhr-Ende (:341), dort 548 Einheiten. Die Zeile :329 kam mit 635d803e5, in 4ee345aaa blieb das Bit hier stehen.
- Beleg: Sonde p_weichen: 125 Austritte, davon 123 in den Hieb, Knochen-Sprung im Austritts-Tick Mittel 1230, höchstens 1259. Normal ist in Clip 5 höchstens 211 je Bild. Fensterlauf: RE15_ANIM_TRACE zeigt F1283 Slot 61 rückwärts, F1284 Slot 27 vorwärts. RE15_POSE_DUMP zeigt kf 278 -> 244, b13 (1604,-781,916) -> (1365,-1022,986). Bild: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/abnahme_r2/code2/weichen_austritt_zoom.jpg. Original: A[8] setzt 0x201 @0x80104158, B[2] läuft im selben Tick, die Richtung ist Argument je Aufruf (@0x8010427c-84). Kontrolle: beide Zeilen in einer Kopie gestrichen, Sprung Hieb höchstens 22, Gehen höchstens 55. Kein Riegel prüft diesen Austritt.
- Vorschlag: Beide Bit-Löschungen in bb_weichen entfernen. bb_anim des nächsten Zustands löscht das Bit im selben Zug wie die neue Pose. Riegel: Knochen-Sprung im Austritts-Tick Weichen -> Hieb und Weichen -> Gehen unter 300.

### N5 [mittel] G5 blutet bei keinem Treffer

- Ablauf: Schüsse auf den G5 zeigen kein Blut, während EM30 und Form davor bluten. Die Wache g->ph == 0 (enemy_ai_boss_g5.c:1727) liest die KI-Phase. Im Original ist es +6 des Trefferzustands, und das ist im ersten Treffer-Tick immer 0. Der Spieler kämpft den Endboss ohne sichtbare Trefferwirkung. Auch die Blut-Serie nach dem Biss (@0x801042d8 ff.) wird im Port nur heruntergezählt, gespawnt wird nichts.
- Beleg: Lauf b1: 27 G5-Treffer, birkin_dbg.log blut=27/0/0/0, fx.log 0 Zeilen Id 0. Original: `lbu v0,6(s3)` / `bnez` @0x801025e8-f0, Applier `sw v0,4(s1)` @0x80047288 schreibt das Wort und nullt +6. Das Dossier führt den Fehler als offen, in M1 bis M17 steht er nicht.
- Vorschlag: Wache auf den ersten Treffer-Tick stellen (treffer_offen), g->sub != 0xF behalten. Biss-Blut-Serie @0x801042d8 nachbauen. Riegel: Spawn-Zähler je Treffer.

### N6 [mittel] EM30-Griff wirft Leon durch die Diagonale in die Wand

- Ablauf: EM30 Sub 5 wirft Leon mit über 1000 Einheiten je Bild durch Box 30. Er steht danach in der Wand, unsichtbar, bis ihn der nächste Treffer herausschlägt. Die Meldung 6 ist damit nur fürs Rennen behoben. Ein Griff in Wandnähe ist die zweite Einstiegstür zum Blocker (z_w3).
- Beleg: f006 (Elza, freies Spiel): F1265 (3103,-24484), F1266 (3878,-25190), F1267 (4287,-26207), dann Box 30/29 bis F1416, 150 Bilder, Flucht-Eingaben ohne Ausweg, Treffer F1416 schlägt sie auf (5946,-24732). Bild: l2_f006_wand.jpg.
- Vorschlag: Wurf-Haken je Teilschritt gegen SCA klemmen, nicht nur am Bildende.

## Gering, nur wenn sie am Weg liegen

- Rennen tötet den Spieler, im Original nie: bb_rennen ruft bb_schlag und damit den tödlichen Schaden. Bei HP unter 5 stirbt Leon am Rennen. Der Kommentar an bb_schaden nennt a1 = 0, für das Rennen stimmt das nicht. Vorschlag: Im Rennen auf HP 0 klemmen, Kommentar berichtigen.
- Doppelschlag trifft nie zweimal: Nach dem ersten Treffer steht Leon in der Reaktion, der zweite Hieb läuft ins Leere. Das Original löst dafür beim Clip-Wechsel die Sperre des Spielers. Vorschlag: Sperre an dieser Stelle wie @0x80101c94-a8 lösen, Gate HP >= 0 @0x80101c8c behalten.
- Hieb setzt die Sperre +0x232 nicht zurück, das Hieb-Geräusch fehlt zeitweise: Nach Rennen oder Biss steht cd_lauf bis 90. Der Hieb spielt se(4) nur bei cd_lauf == 0 und bleibt bis zu 3 s stumm. Nach dem Griff tritt das nicht auf, weil beide Sperren gleich laufen. Vorschlag: s_bb.cd_lauf = 0 im Eintritt T0[10].
- G5-Schaden ist nicht auf die Port-HP umgerechnet, das fällt jetzt auf: Die Form rechnet RE2-Schaden halbiert auf 100 HP, der G5 nimmt Rohzahlen (Biss 40, Arme 15 und 10). Seit die Bisssperre abläuft, trifft das bei jedem Biss. Ein Leon mit HP 70 braucht zwei Bisse bis HP 0. Vorschlag: Dieselbe Regel wie beim Griff anwenden (RE2 halbiert) und im Dossier festhalten.
- Griff läuft nach dem Wurf 92 Bilder ins Leere: Seit M5 dauert Clip 27 korrekt 100 Bilder. Leon fliegt aber bei Bild 8 weg, die Form schüttelt danach 3 s nichts. In RE2 hält sie ihn in dieser Zeit. Vorschlag: Als Gestaltung entscheiden: Griff nach dem Wurf abkürzen oder Griff-Opfer-Sequenz bauen.
- Leon ist im Rückstoss und G5-Auftritt rund 14 s nicht im Bild: Steht Leon östlich, zeigen Szene und Auftritt Cut 12 und Cut 11. Er hat ab dem Morph wieder Kontrolle und sieht sich nicht. Das Dossier nennt rund 80 Bilder, gemessen sind es über 400. Vorschlag: Dossier berichtigen. Ob die Szene kürzer wird, ist eine Gestaltungsfrage, die Kamerafreigabe selbst stimmt.
- Riegel: 41 von 89 Mutanten überleben die Unit-Tests: Kein Spielfehler, aber Rückfälle bleiben hier unbemerkt. Ungeprüft sind unter anderem Rate Hieb, Biss, Rennen und Nachhieb, Rennen-Tempo +10, Bremse und Abdrehen, alle vier Tentakel-a1-Varianten, Griff-Schaden ab 86 HP, Devour aus dem Flinch, Timer 150 und Segment-Klemme unten, Blut-Anker und Blut-Zone, Rate und Richtung beim Morph. Vorschlag: Riegel für Tentakel-a1 ergänzen: a1 = 0 bei HP 5 -> 0, bei HP 0 -> tot, a1 = 1 nie tot. Dazu Rate je Zustand und ein Tempo-Riegel fürs Rennen.
- Arme versetzen den Spieler sprunghaft ohne Treffer: Leon steht zielend, Arme fegen vorbei, er springt in einem Bild seitlich, ohne Treffer-Reaktion. Der Schub ist laut Port-Kommentar byte-true (FUN_80034D0C), im engen Gang aber sichtbar. Vorschlag: Mit der Klemme des Blockers mitprüfen, sonst so lassen.
- SDL-Assertion beendet das Spiel bei Anzeigewechsel: Ändert sich die Anzeige (hier Sitzungswechsel), endet jedes gerade startende oder laufende Fenster mit Exit 42. Kein Bezug zu Spur R. Vorschlag: Für das Paket prüfen, ob SDL mit SDL_ASSERTIONS=disabled oder einem Release-Typ baut.