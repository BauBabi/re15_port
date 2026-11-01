# Runde 41 — Spur W: wasserblink (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_wasserblink`, Zweig `r41/wasserblink`, Basis master aa88a1b6a.

Wortlaut des Nutzers: "Das blinken im Wasser ist auf jeden Fall fehlerhaft und quatsch, siehe Marker."
Marke: ROOM2000 Cut 2, Bild F444, Leon (-10685,-1800,-25659), Hauptbaum-exe 2026-11-01 08:21,
Bild `W_belege/nutzer_marke1_2000_F444.png`, befund.log Zeile 30997 (Cut 2, 10 Masken, "Gegner im Raum: keine").

Werkzeuge (alle unter `W_werkzeug/`):
* `w_fenster.py` — Kopie von `befunde_runde30/titel-blinken_tools/r30_port_capture_hwnd.py`: startet eine exe-Messkopie,
  sucht das Fenster ueber die EIGENE PID, nimmt per ffmpeg gdigrab `hwnd=` auf (beschleunigter Renderer direct3d,
  kein AUTOSHOT, kein SOFTWARE_RENDER), beendet nur die eigene PID.
* `w_flacker.py` — zaehlt Bildwechsel mit Aenderung > 8 Graustufen ausserhalb der Leon-Spalte (x 130..210 von 640).
* `w_scd.py`, `w_scd_liste.py` — linearer SCD-Durchlauf (Laengen Skill scd-disassembly).
* `w_cuts.sh` — je Cut ein Lauf mit `RE15_RAUMLICHT_LOG`, zaehlt die projizierten Glanzpunkte.

Weg zur Marke: Debug-Menue-Sprung `RE15_DEBUG_JUMP=2000@250` (dieselbe Install-Sequenz wie eine Tuer),
`RE15_PLAYER_POS=-7000,-25659,1976,1`. Der Sprung setzt Cut 0 (Sprungtabelle); in Cut 0/1 liegt Leon in den RVD-Zonen
0->1 (@0x0028C, x -9097..3805) und 1->2 (@0x002C8, x -9497..-2499), die Kamera wechselt ueber die echten Zonen auf
Cut 2. Danach 1,6 s Laufen nach Westen (Leon bei (-10552,-1800,-26043), Cut 2). Ohne Zonenweg bleibt Cut 0
(gemessen: Spawn bei x -12300 liegt in keiner Zone mit Herkunft 0). `RE15_FORCE_CUT` nur fuer die Cut-Uebersicht
(Abschnitt 1.3), weil es jedes Bild den Aufbau ausloest.

## 1. Messung vorher (master aa88a1b6a, Commit 2026-11-01 08:20; eigener Bau 17:21, Messkopie `re15_w_master.exe`)

Die Nutzer-exe (Hauptbaum, 08:21) traegt den Raumlicht-Pfad ebenfalls (Zeichenkette `RE15_KEIN_RAUMLICHT` 1x in der exe),
die Release-exe v0.8.29 nicht (0x).

### 1.1 Was blinkt (echtes Fenster, 30 Bilder/s, 640x480)

Lauf `cap_m1` (14 s, 420 Bilder): Die grossen gelblich-fleckigen Flaechen im Kanal (die Flecken auf dem Nutzerbild)
stehen still: Mittelwert der Flaechen b1 (x 215..250, y 200..222) = 11,1 und b3 (x 240..360, y 360..440) = 34,3 in
ALLEN 371 Bildern. Was blinkt, sind kleine helle Ellipsen (grau, weisser Strich) im Kanal zwischen x 238..379 und
y 268..319 (640er Bild) = PSX x 119..190, y 134..160: jede leuchtet genau 4 Bilder auf (+43..+76 Graustufen) und
erlischt wieder, an wechselnden Orten, meist eine, selten zwei zugleich. Beispiele (Bild: Bildpunkte, Lage, Vorzeichen):
`55 262 x254..375 y280..319 +21,7` / `59 ... -40,6`; `75 160 x254..327 y280..297 +43,5` / `80 ... -43,5`;
`84 102 x360..375 y312..319 +43,4` / `88 ... -43,4`. Bilder mit Aenderung ausserhalb Leon: 46 von 250.
Zeitreihe (Mittelhelligkeit je Fenster, Bilder 50..130, 30 Bilder/s; `hell` = mehr als 10 ueber dem Minimum):

| Fenster (640er Bild) | master: min / max, hell in Bildern | Fix: min / max |
|---|---|---|
| P1 x254..269 y280..285 | 0,1 / 45,9 — 55-58, 75-79, 100-103 | 0,1 / 0,1 |
| P2 x360..375 y312..319 | 0,0 / 35,2 — 55-58, 84-87, 91-99 | 0,0 / 0,0 |
| P3 x238..253 y268..275 | 42,1 / 80,9 — 96-99, 124-127 | 42,1 / 42,1 |
| Fleck b3 x240..360 y360..440 | 37,9 / 37,9 — nie | 37,9 / 37,9 |

Takt: Fenster von 4 Spielbildern (`frame / 4`), an wechselnden Punkten; P2 91-99 = zwei Fenster hintereinander.
Auf dem Nutzerbild F444 leuchtet gerade keine (Ausschnitt 4-fach geprueft) — die Marke zeigt den Kanal zwischen zwei Blitzen.

### 1.2 Quelle — einzeln abgeschaltet und gemessen (je 10 s, gleicher Weg)

| Lauf | Schalter | Bilder mit Aenderung ausserhalb Leon |
|---|---|---|
| cap_m1 | keiner (master) | 46 / 250 |
| cap_m1b | Wasserlauf Spur N aus (`RE15_SET_FLAG=9:100`, kein `[wasserlauf41]` im Log) | 63 / 370 (blinkt weiter) |
| cap_np | Raum-Objekte 0..2 (Wasserflaechen Typ 3) aus (Messhaken) | 46 / 250 |
| cap_o1 | alle ESP-Effekte aus (Messhaken) | 46 / 250 |
| cap_o2 | RE2-FX aus (Messhaken) | 46 / 250 |
| cap_opri | Masken aus (`RE15_NO_PRI=1`) | 46 / 250 |
| **cap_krl** | **`RE15_KEIN_RAUMLICHT=1`** | **0 / 250** |
| cap_v0829 | Release-exe v0.8.29 (`release/pkg-win/re15_port_v0.8.29`, Messkopie) | 0 / 250 |

`RE15_FX_LOG` im selben Lauf: im Cut-2-Bild zeichnet der ESP-Pfad nur Effekt 6 Sub 0 bei y -10000 (Bildlage sy -128..-78,
also ueber dem Bildrand); `RE15_TRILOG@115,130,195,165` (Wasserflaechen aus): im Fenster nur Leons Dreiecke. Die
Ellipsen sind also weder ESP noch Modell, sondern die Lichtvierecke des Raumlicht-Pfads (`re15_render_light_quad`,
ins 2D-Bild eingerechnet).

**Ursache:** `engine/src/raumlicht.c`, Fall `0x2000` (gilt fuer 2000 und 2001: `room & 0xFFFE`), Commit 4100d2aed
(2026-10-30, direkt auf master, nach dem Release v0.8.29 — `git merge-base --is-ancestor 4100d2aed v0.8.29` = nein; vor dem
Merge der Spur N c5d7ba18a). Kopf `include/re15_raumlicht.h`: "PORT-ERGAENZUNG, nicht byte-true ... Groesse/Blinken
Gestaltung". Sechs feste Weltpunkte im Kanal (`s_glanz`, (-6300,-21450) .. (-9900,-23875), y 0), je Punkt ein
4-Bild-Fenster, `rl_hash(fenster*31 + i*977) % 12 == 0` -> leuchtet (225,230,230) additiv, Halbmass 260. Takt, Zahl,
Groesse und Farbe sind ausgedacht, keine Adresse. Die Spur N (Wasserlauf) ist NICHT die Quelle (Lauf cap_m1b).

### 1.3 Wo noch (Lauf je Cut, `w_cuts.sh`, `RE15_FORCE_CUT`, 200 Bilder, je 128 Glanz-Vierecke angelegt)

ROOM2000 Leon: im Bild projiziert in **Cut 2 (128/128), Cut 7 (128/128), Cut 6 (60/128), Cut 15 (28/128)**; alle
anderen Cuts 0. In Cut 6/7 (Kamera bei x -24876 am Weststeg) liegen die Punkte 15000 Einheiten entfernt hinter Waenden:
der Raumlicht-Pfad hat keinen Regions-Test (anders als ESP @0x80053334), die Blitze erscheinen dort durch die Wand.
ROOM2001 (Elza): derselbe Fall (`room & 0xFFFE`); gemessen Cut 2 128/128, Cut 7 128/128 im Bild.
ROOM20B0: `re15_raumlicht_liste` hat keinen Fall 0x20B0 (nur 0x50B0 und 0x2000) — dort entsteht nichts aus dieser Quelle.
Gemessen (echtes Fenster, `RE15_DEBUG_JUMP=20B0@250`, `RE15_FORCE_CUT=2`, 12 s): master und Fix gleich — keine 4-Bild-
Blitze; die einzigen Aenderungen sind die Ab- und Aufblende der Ankunftsszene (Bilder 117..131 und 241..255, ganzes Bild
-15/+15). Der Kanal von 20B0 Cut 2 ist trocken (braun), Leon steht danach bei (-20657,-1800,-9629) (Plc_dest sub05).

## 2. Original

### 2.1 Skript ROOM2000 (Disc Version_40% und Repo-Baum)

`w_scd.py` / `w_scd_liste.py`. Sce_espr_on in ROOM2000:
* Repo-Baum (`shared_assets/PSX/STAGE2/ROOM2000.RDT`): main00 @0x0174C/@0x0175C/@0x0176C Effekt 0x0B Sub 0 Skala 0x1900
  bei (-23900/-23300/-22700, -5300, 11200) = die drei Strahlen am Gitter; sub07 @0x01B06..@0x01B94 achtmal Effekt 6 Sub 0
  Skala 0x1000 bei y -10000 in einer Schleife (For 0xB4 @0x01B36/@0x01B60/@0x01B8A, For 0x2D @0x01BA4, Goto @0x01BAE).
* Version_40% (`Version_40%/PSX/STAGE2/ROOM2000.RDT`): dieselben drei 0x0B @0x01568/@0x01578/@0x01588, dieselbe
  Schleife Effekt 6 Sub 0 y -10000 in sub03 @0x015B6..@0x01644 (andere x/z).
* Wasserflaechen: Obj_model_set obj 0/1/2 Typ 3 Attribut 0x001A (Bit 0x2 halbtransparent) @0x016E6/@0x01708/@0x0172A
  (40%: @0x01502/@0x01524/@0x01546, Bytes gleich bis auf pc[4]).
Kein Skript setzt einen Effekt auf das Kanalwasser in Cut 2; die Effekt-6-Teilchen bei y -10000 projiziert der Port
in Cut 2 auf sy -128..-78 (ueber dem Bild, `RE15_FX_LOG`).

### 2.2 Hintergrund und Wasserflaeche

Mit ausgeblendeten Wasserflaechen (Messhaken, Lauf cap_np) zeigt der Hintergrund ROOM200.BSS Cut 2 den hellen
Betonboden des Kanals mit den Lichtflecken unter den Deckenlampen. Die Wasserflaechen (Typ 3, halbtransparent)
dunkeln ihn ab; die hellen Flecken scheinen durch. Das sind die gelblichen Flecken des Nutzerbilds — sie stehen
still und sind in v0.8.29 identisch (Seitenvergleich `W_belege`).

### 2.3 80%-Videos

`documentation/Biohazard 1.5 Complete Disc Video 2 of 4 (360p).mp4`, 5..21 s (ROOM2000 Cut 2, Korrelation 0,72 gegen
unser Bild): der Kanal ist dunkel, die gelben Bodenflecken scheinen still durch (alle Bilder). Die "weissen,
leuchtenden Partikelwolken" (BEFUND G4) sind Spritzer an den Fuessen der Zombies, die durch den Kanal waten: sie
erscheinen nur unter einer gehenden Figur und wandern mit ihr (Videobilder 94..98, 114..121, 213..216, 227..233,
242..247, 257..262, 271..277 bei 30 fps, Abstand ~15 Bilder = Schritttakt). Ohne Figur im Wasser blitzt nichts.
Der Raumlicht-Fall 0x2000 hat diese Schritt-Spritzer als ortsfeste Glanzpunkte nachgebaut.

Belege: `W_belege/w_hintergrund_room200_cut2.jpg` (BSS Cut 2 Repo-Baum und Version_40% ohne Objekte, daneben der Port mit
Wasserflaechen), `W_belege/w_80prozent_2000cut2_spritzer.jpg` (Videobilder 110..122 und 225..245: Spritzer unter der
watenden Figur), `W_belege/w_v0829_gegen_master_cut2.jpg` (Flecken in v0.8.29 und master gleich, Blinken nur master).

DuckStation-Lauf: nicht gemacht. Fuer die Frage "blinkt im Original etwas ortsfest auf dem Kanalwasser" reichen das
Skript beider Discs (2.1, kein Effekt aufs Wasser) und das 80%-Video (2.3, nur Schritt-Spritzer).

## 3. Behebung

`engine/src/raumlicht.c`: Fall `0x2000` (Tabelle `s_glanz`, `rl_hash`, 4-Bild-Fenster) entfernt; der Kopfkommentar an der
Stelle nennt Grund und Belege (SCD @0x0174C/@0x0175C/@0x0176C, sub07 @0x01B06..@0x01B94, 40% @0x01568.., sub03
@0x015B6..; Video). `frame` bleibt im Aufruf (Signatur unveraendert, `(void)frame`). `include/re15_raumlicht.h`: Kopf
angepasst. Unveraendert: der 50B0-Schein (eigene Port-Ergaenzung, nicht Teil dieses Befunds, siehe OFFEN), die
Wasserflaechen (Obj_model_set Typ 3), der Wasserlauf der Spur N (`wasserlauf_r41.c`), der ESP-Pfad.
Die Mess-Haken (`RE15_W_OHNE_PROP`, `RE15_W_OHNE` in main.c) waren nur im Messbau und sind zurueckgenommen.

## 4. Messung nachher (Bau 2026-11-01 18:00, Messkopie `re15_w_fix.exe`)

Echtes Fenster, gleicher Weg wie 1.1 (14 s, 30 Bilder/s):

| Lauf | Figur / Raum | vorher (master) | nachher (Fix) |
|---|---|---|---|
| cap_m1 / cap_fix | Leon ROOM2000 Cut 2 | 46 / 250 (cap_m1 bis Bild 300) | **0 / 370** |
| cap_elza_master / cap_elza_fix | Elza ROOM2001 Cut 2 | 64 / 370 | **0 / 370** |

Wasserlauf der Spur N weiter da: `[wasserlauf41] ROOM2000: 6 Strahlen, Lauf ueber den Weg Effekt 0x0F` (Leon) und
`ROOM2001` (Elza) in beiden Fix-Laeufen. Die Flecken (Hintergrund durch die Wasserflaechen) stehen unveraendert.
Bilder: `W_belege/w_vorher_bildfolge_cut2.jpg` / `w_nachher_bildfolge_cut2.jpg` (Bild 73..80, Glanzpunkt Bild 75..79),
`w_vorher_nachher_voll_cut2.jpg` (mit Differenzbild 76-72), `w_elza_vorher_nachher_cut2.jpg` (Bild 67..74).

Cut-Uebersicht nachher (`w_cuts.sh`, Messkopie Fix): Leon Cut 2/6/7/15 und Elza Cut 2/7 je **0** Glanz-Vierecke (vorher
128/60/128/28 bzw. 128/128).

### Riegel

* `unit_raumlicht` (geaendert): 2000/2001 in 6000 Bildern kein Licht, 20B0 und 1140 nichts, 50B0 drei Scheine.
* `integration_r41_wasserblink_leon` / `_elza` (neu, `tests/integration/test_r41_wasserblink.cmake`, Werkzeug
  `tests/unit/test_r41_wasserblink.c`): Bildserie F300..F419 (RE15_FRAMEDUMP) im Kanal-Fenster x 104..230, y 100..200:
  kein Wechsel > 8; F360 gleich dem Lauf mit RE15_KEIN_RAUMLICHT; Cut 2 ueber die Zonen; Wasserlauf-Zeile; Elza = PL04.
* `integration_r41_wasserblink_gegen` (neu): ROOM50B0 Cut 2 mit/ohne Raumlicht unterscheiden sich im Bodenband (gemessen
  28 Bildpunkte, groesster Unterschied 46) — der Framedump-Weg sieht den Raumlicht-Pfad.
* **Gegenprobe gegen den alten Stand:** derselbe Riegel mit der master-exe (`re15_w_master.exe`, aa88a1b6a) schlaegt fehl,
  Leon und Elza: `flacker: 120 Bilder, 16 von 119 Wechseln > 8 (groesster Sprung 131)` und `gleich: groesster Unterschied 47
  bei (171,153)`. Mit dem Fix: 0 von 119, groesster Sprung 0; gleich 0.

* Bestehende Riegel (ctest `-R "nachtrag30|wasser20b0|r38_wasser"`, -j 3, Fix-Bau 18:00): **35/35 gruen** (u.a.
  integration_r41_nachtrag30_wasser_leon/_elza, _laden_leon/_elza, integration_r41_wasser20b0, _npc, integration_r38_wasser,
  integration_r38_wasserzombie). Neue Riegel: unit_raumlicht, unit_r41_wasserblink_werkzeug,
  integration_r41_wasserblink_leon/_elza/_gegen: 5/5 gruen.

## Fuer den Nutzer

Das Blinken im Kanal war ein ausgedachter Effekt des Ports: weisse Glanzpunkte, die an sechs festen Stellen des Kanals
fuer je vier Bilder aufleuchteten. Er stammte aus einer falschen Deutung des 80%-Videos; die weissen Wolken dort sind
Spritzer an den Fuessen von Zombies, die durch das Wasser waten. Das Original legt keinen Effekt auf das Kanalwasser.
Der Effekt ist entfernt (Leon und Elza, ROOM2000/2001, alle Kameras). Die hellen Flecken auf deinem Bild bleiben: das ist
der Kanalboden mit den Lampenflecken, der durch die halbdurchsichtige Wasserflaeche scheint, genau so im 80%-Video zu
sehen und unveraendert seit v0.8.29. Der Wasserlauf an der Leiter bleibt.

## OFFEN

1. **ROOM50B0 Cut 2, roter Bodenschein** (`raumlicht.c` Fall 0x50B0, derselbe Commit 4100d2aed): ebenfalls Port-Ergaenzung
   ohne Adresse ("Farben gemessen, Groesse Gestaltung"), blinkt aber nicht. Nicht Teil dieses Befunds, unveraendert; die
   Frage, ob er bleiben soll, gehoert dem Nutzer (Regel "Mod-Inhalt nur mit 80%-Beleg": das Video zeigt dort ein rotes Band).
2. **Zombie-Spritzer beim Waten** (80%-Video, ROOM2000 Cut 2): die echten "weissen Wolken" sind Schritt-Spritzer watender
   Zombies. Ob die Port-Zombies in 2000/2001 im Kanal waten und dabei spritzen (FUN_80045630-Zwilling `wasser_r38.c`,
   NPC-Weg Spur A Punkt 2), ist hier nicht gemessen — in der Marke steht "Gegner im Raum: keine".
3. **BEFUND.md G4** (`analysis/video_80prozent/BEFUND.md` Zeile 43) nennt fuer 2000 noch "zu pruefen"; Ergebnis dieser Spur:
   Schritt-Spritzer der Zombies (Abschnitt 2.3). Die Datei ist nicht angefasst (gemeinsame Datei).
4. Effekt 6 Sub 0 bei y -10000 (sub07 @0x01B06..): der Port haelt die Teilchen an der Decke (wpos.y -10018..-10000 ueber
   313 Bilder, `RE15_FX_LOG`). Ob das Original sie fallen laesst, ist nicht geprueft; in Cut 2 liegen sie ausserhalb des Bilds.
