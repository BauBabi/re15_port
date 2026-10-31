# Runde 41 Spur O "keyitems" - Abnahme 1

Stand: r41/keyitems 8271c2726 (Basis f5a5e2f7e, master ist 28 Commits weiter). Bau: `local_build.sh configure` + `build` = `no work to do`, exe md5 18978cc1...
Laufordner aller eigenen Messungen: `$TEMP/o41a1/run/<lauf>`, Bogen `$TEMP/o41a1/b_*.png`, Treiber `$TEMP/o41a1/lauf.sh` (Messkopie `re15_pc_o41a1_<lauf>.exe`, keine geerbten RE15_*, Bilder RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1).

(Bericht wird fortlaufend geschrieben; Ergebnis-Tabelle unten steht erst am Schluss fest.)

## Punkt 1: Kurbel (laufend)

Lauf k_stand (Leon, echte Tuer 2050 -> 2020, Handgun, START, ITEM, Kurbel, USE; Bogen b_k_stand_a.png, b_k_stand_zoom.png):
```
F255 USE (pad=8000)  F256..F262 Inventar blendet aus  F263 schwarz  PL(2914,-3388) rot 2016 cam=3
[scd F264] Cut_chg(3)  [scd] Plc_motion(entity=0, motion=0, flags=0x40)  F264 PL(3650,-3018,rot=1024) pst=4 mo=0
F265..F267 blendet ein, F268 erstes volles Bild: Leon kurbelt
[scd F372] Cut_chg(4)  [scd F442] Cut_chg(3)  [keyitems] Waffe 3 wieder in der Hand
```
Aenderung je Bild im Leon-Ausschnitt (40,70)-(140,200), Schwelle 24: F268 2648 (letzter Blendenschritt), F269..F330 11..160, keine Spitze. Szenenende: F443 6016 (Schnitt), F444 21, F445 14, ab F446 0.

Weitere Laeufe (alle echte exe, echtes Menue START, ITEM, Platz, USE):
- k_renn (Rennen nach Westen, START bei x 3617): Bild N nach Pos_set gegen k_stand, N = 4..32, Leon-Ausschnitt 13000 Pixel: 0 verschiedene Pixel in jedem Bild.
- k_halten (Aktionstaste 36 Bilder ueber das USE hinaus gehalten): dieselben Bildnummern wie k_stand, kein zweiter Start.
- k_ost (Stand x 1700, vorher Kamera 0): F334 Pos_set in Schwarz, F335..F337 Einblende schon Kamera 3, kein Bild der alten Kamera (b_k_ost.png).
- e_stand (Elza, RE15_TITLE_SHOT_CHAR=1, Tuer 2051 -> 2021): F264 wie Leon, Lage (3650,-3018), `[keyitems] Waffe 1 wieder in der Hand` (b_e_stand.png).
- Gegenprobe alt (selbe exe, Inventar voll, Frage + Ja, Altstand): Plc_motion F352 flags=0x00, Aenderung F352..F357 = 338 473 264 274 234 146, danach 58..92: das Mass sieht das Fliegen; im USE-Lauf gibt es diese Spitze nicht.

Sollwert der Lage selbst nachgelesen: Original-Lauf E41n2/o_stand1.txt Zeile 90 `n89 vs9535 ... pl=3650,-2925 ... spd=0`, Zeile 91 `n90 vs9569 ... pl=3650,-3018 ... spd=0`, Zeile 125 `n124 ... cmd=4/0/1/0 mo=0/15 pl=3650,-3018`. Der Riegel-Sollwert -3018 stimmt.

Vergleich mit RE2 (selbst gelesen, ROOM1110): USE-Rechteck @0x020A0 `2c 07 06 21 00 00 0a a2 ec bc 08 07 08 07 0a 00 43 00 01 00` = x -24054..-22254, z -17172..-15372; Spieler-Lage sub13 @0x02EDA `32 00 11 a0 6c 00 34 bd` (-24559,108,-17100), @0x02EE6 `32 00 0d a1 6c 00 62 bd` (-24307,108,-17054), Gier @0x02EF0 `34 0f 00 0c` = 3072. Mitte des Rechtecks -> Lage: 1393 Einheiten; RE1.5 ROOM2020 Mitte (2600,-3750) -> Pos_set (3650,-2925): 1335. Auch in RE2 steht man beim USE nicht auf der Dreh-Lage, die Lage kommt aus dem Skript; von der Dreh-Lage selbst liegt der Vorwaerts-Punkt (-24307,-16434) ausserhalb des Rechtecks. Der Hinweis H1 aus Abnahme 0 ist damit RE2-Form, kein Mangel.

RE2-Beleg Flag 0x40 selbst gefunden (im Dossier nur RE1.5 zitiert): RE2 @0x800669f4 `lhu v0,460(s0)`, @0x800669f8 `addiu v1,zero,7`, @0x800669fc `andi v0,v0,0x40`, @0x80066a04 `sb v1,334(s0)`, @0x80066a08 `sb zero,334(s0)`; gleicher Block @0x80066e30..@0x80066e44. +0x1CC ist das Flag-Halbwort (Handler @0x80056bbc `sh a2,460(v1)`). Bit 0x40 schaltet die Blende also auch in RE2 ab.

Waffe nach der Szene (Lauf k_schuss): Plc_ret F512, `Waffe 3 wieder in der Hand`, R1 + Feuer ab F570: `mg=15` -> `mg=14` in F570. Die Pistole schiesst wieder.

## Punkt 2: Key Items ueber das Inventar (laufend)

Eigener Zensus, unabhaengig von k_stellen (alle 206 RDTs):
- Texte: `zensus_used.py` alle Nachrichten mit 0x03-Code; Treffer mit Gegenstand: 1090, 10D0/1, 1100/1, 11E0/1 (2x), 1230/1, 2020/1, 2040/1, 2060/1, 3050/1, 4000 (4001 ohne Message_on), 4030/1, dazu 40A0 msg 4 ohne Message_on. Das sind die 12 Skript-Stellen der Tabelle.
- Fragen ohne Gegenstand bleiben Fragen: Schalter, Hebel, Computer, Ruestung (1040, 1190, 2050, 2080, 5030, 5050, 5060, 5080, 6000 ...).
- Key-Item-Texte OHNE Frage, einzeln im Skript nachgesehen: ROOM4090 msg 0/1 ("hole to set the Enzyme", Enzyme 0x3C nie vergeben, Slot 1 sce 1 reiner Text), ROOM50A0 msg 0 (reiner Text, Slot sce 1), ROOM1190 msg 1 "There's a driver here" (kein Message_on), ROOM3010 msg 1 (Karte als Tuerschloss, siehe Punkt 3), ROOM3091 msg 32 (Latch Key = Schluessel). Keine weitere Stelle.
- Besitz-Abfragen `Ck(9,..)` aller Skripte (zensus_ck9.py): Ziele 10D0/10D1 @0x019C0/@0x019AA, 11E0/1 @0x01FA8, 1230/1 @0x01488 (Tabelle), 3091 Latch Key; der Rest ist keine Gegenstands-Benutzung. Op 0x5E Keep_Item_ck: 0 in allen RDTs.
- Gegenstaende (Item_aot_set + Namenstabelle, items.py): Head of Akuma 0x32 (2040) und Pocket Watch 0x3F (2090) haben in keinem Skript eine Benutzung. 0x33 und 0x35: 0 Platzierungen.
- Flag-Bits: alle `Set` auf (3,114), (3,113), (3,146), (3,53), (3,108), (3,144), (3,78) in zensus_set.py: je genau die zitierte Stelle, kein Set auf 0.

57 zitierte Bytefolgen (belege.py: alle Message_on, Aot_set, Set der Tabellen in keyitems_r41.c, Sleep, Plc_motion, Pos_set, Cut_chg, Plc_ret ROOM2020; RE2 ROOM1080/2110/6140): 0 Abweichungen.

Echtes Menue an der exe (RE15_PRESS START, ITEM, Platz, Auswahl, USE; Laufname in Klammern):
| Stelle | Ergebnis |
|---|---|
| 1090 Loescher (s1090) | `USE 0x31 ROOM1090 Slot 2 Ereignis 6`, Frage 8 entfaellt, Cut 5, msg 9 (b_s1090.png) |
| 11E0 Zange (s11e0) | Frage 11 entfaellt, Cut 14 |
| 11E1 Yellow Keycard Elza (e11e1) | Frage 7 entfaellt, msg 9 |
| 1100 Minidisc (s1100) | Frage 3 entfaellt, Cut 12 |
| 1050 Sicherung (s1050) | Frage 20 entfaellt, msg 21 |
| 2010 Sicherung Kasten (s2010) | Frage 41 entfaellt, Eintrag weg |
| 2020 / 2021 Kurbel (k_stand, e_stand) | siehe Punkt 1 |
| 2060 Sicherung (s2060) | Frage 4 entfaellt |
| 3050 / 3051 Red Master Keycard (s3050, e3051) | Frage 4 entfaellt, Cut 15 |
| 4000 Blue Master Keycard (s4000) | Frage 1 entfaellt, msg 2 (b_s4000.png) |
| 4030 Zuendkerze (s4030) | Frage 1 entfaellt, Cut 5 |
| 10D0, 1230-Form, 2040 | Riegel integration_r41_keyitems_karte, _bombe gruen |

Aktionstaste (a1090, a11e0, a2060, a4000, a3050): nur Beschreibung, kein Ja/Nein, kein Ereignis. a2060: `Frage 4 entfaellt: Nein (Aktionstaste)`.
Falsche Stelle (k_falsch, b_k_falsch.png): Text "You can't use it here", Inventar bleibt offen, Kurbel bleibt.
Aufnahme (auf_tuer): 2010 Frage, Ja, `Gegenstand 0x33 ins Inventar ((3,114) gesetzt)`, msg 1, echte Tuer -> ROOM2000, Inventar dort zeigt die Kurbel (b_auf_tuer.png). Elza e_auf ROOM2011: dasselbe. Volles Inventar (voll2010, b_voll.png): Bild der Kurbel, "You can't carry any more items", Flag bleibt 0.
CHECK (chk_kurbel, chk_bombe): Bild Kurbel, "The end is square-shaped"; Bild Bombe, "Time Bomb".
