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
*Nachtrag Nachbesserung 1 (Abschnitt 5):* Dieser FX_LOG-Lauf reichte nur 313 Bilder weit und sah deshalb nur die
Tropfpunkte 3 und 5. Punkt 7 kommt ab F545 dazu. Der Port zeichnet ihn als 1 bis 2 Bildpunkte grossen, pulsierenden Fleck
am oberen Bildrand bei (83,0..1). Effekt 6 Sub 0 ist ausserdem kein ruhendes Teilchen, sondern ein Deckentropfen, der im
Port haengen bleibt (Abschnitt 2.1).

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

~~Kein Skript setzt einen Effekt auf das Kanalwasser in Cut 2; die Effekt-6-Teilchen bei y -10000 projiziert der Port
in Cut 2 auf sy -128..-78 (ueber dem Bild, `RE15_FX_LOG`).~~ **Falsch, berichtigt in Nachbesserung 1 (Mangel M1 aus
`W_abnahme_1.md`).** "Ueber dem Bildrand" beschreibt nur das, was der Port tut, nicht das Original. Richtig ist:

**(a) Effekt 6 Sub 0 ist ein fallender Deckentropfen mit Aufschlag-Spritzer.** Die Bytes stehen in der Raum-ESP, gelesen
mit `W_werkzeug/abnahme1/wa_esp.py` und als Hexdump gegengeprueft. Repo-Bank @0x14608, Effekt 6 @0x147E0, ein Strom mit 4 Zeilen @0x14A08:

| Zeile | Bytes ab | Routine A | Wirkung (selbst disassembliert, Tabelle @0x80071d40) |
|---|---|---|---|
| 0 | @0x14A0C `0a 00 00 00 00 10 00 10` | 10 → @0x800176b0 | Flags := +0x0e (3), +0x30 \|= +0x16 (0x60), +0x6e := +0x26 (Repo 0x1C, 40% 0), sofort weiter (`jal 0x800174e4` @0x80017700) |
| 1 | @0x14A34 `03 00 …` | 3 → @0x80017334 | zaehlt +0x16 = **9** Bilder herunter (@0x8001735c), dann weiter, weil +0x26 = 1 (@0x80017390) |
| 2 | @0x14A5C `0d 00 00 00 00 10 00 10 00 00 0a 00` | 13 → @0x80017990 | **accel (0,10,0)** @0x14A64 fuer den Takt; Routine 13 liest die Weltlage +0x28/+0x2a/+0x2c, `jal 0x8001c6e8` (room_coll) @0x800179c4; liegt y unter dem Boden (`slt` @0x800179d8): Flags := +0x0e (@0x800179f8), +0x38 -= (y - Boden) (@0x80017a0c), +0x6e := +0x16 = **0x26** (@0x80017a28), weiter (`jal 0x800174e4` @0x80017a48) |
| 3 | @0x14A84 | 0 | leer |

* Version_40%: Bank @0x267C, Strom @0x2758, dieselben Zeilen. Zeile 2 @0x27AC mit accel (0,10,0) @0x27B4, die Aufschlag-Anim
  ist dort 0x0A statt 0x26.
* Routine 13 ist in beiden EXE gleich: `info/Re1.5/PSX.EXE` und `Version_40%/PSX.EXE` stimmen @0x80017990..@0x80017a5c
  Byte fuer Byte ueberein, die Tabelleneintraege [3], [10] und [13] @0x80071d40 ebenso.
* ROOM2001 im Repo: main00 @0x0177C `04 ff 18 08` startet sub08 @0x01D32. Die Bytes der Schleife sind gleich (0xAE Bytes),
  Zeile 2 steht @0x14E74. ROOM2001 der 40%-Disc ist Byte fuer Byte gleich ROOM2000 der 40%-Disc (`cmp`: 0 Unterschiede).

**(b) Wer die Schleife startet.** Ein Umlauf dauert 3·180 + 45 = 585 Bilder (19,5 s). Punkte 0-2 fallen bei t=0,
Punkte 3-4 bei t=180, Punkte 5-6 bei t=360, Punkt 7 bei t=540.

| | Start der Schleife | Beleg |
|---|---|---|
| Repo-Baum (Mod) | **ja, unbedingt** | main00 @0x0177C `04 ff 18 07` (Evt_exec sub07), direkt nach den drei 0x0B |
| Version_40% (echte Disc) | **nein** | main00 endet @0x01598 mit `01 00`. Ein linearer Durchlauf von main00 und sub00..03 (Laengentabelle `scd_vm.c`) findet kein Evt_exec (0x04) und kein Gosub (0x18), die Bytesuche `04 ?? 18 ??` in 0x1424..0x1664 findet nichts. sub00 @0x015A4 und sub01 @0x015A6 sind `01 00`. Die drei Aot_set @0x014C6/@0x014DA/@0x014EE sind sce 8, also Wasserzonen mit p0 0xFC7C = -900, kein Ereignis. Von sich aus startet die EXE nur sub00 (Raum-Init, `jal 0x8003ee3c` @0x8003efd4 mit a1 = 0) und jedes Bild sub01 (@0x8003f080 mit a1 = 1). Beides ist in beiden EXE gleich. |

Fuer die echte Disc stimmt "kein Effekt auf dem Kanalwasser" also nur deshalb, weil die vorhandene Schleife sub03 nie
gestartet wird.

**(c) Was im Bild von Cut 2 faellt (Repo-Baum).** Kamera RID Cut 2 @0x000A0: Lage (1926,-4590,-25164), Ziel (-13392,234,-20556), H = 0x683C>>7 = 208.
Die Gegenprobe der Rechnung mit Leon (-10552,-1800,-26043) ergibt (83,108), `befund.log` sagt (84,108). Fuer Effekte gilt der
Regions-Test des Originals (`jal 0x80014368` @0x80053334, Viereck = erster RVD-Satz mit rec[2] = Cut, FUN_80014324).
Fuer Cut 2 ist das der Satz @0x00318 `01 ff 02 00`, x -28298..4001, z -28000..-18500. Masken: Cut 2 hat 10 (pri @0x0093C),
alle bei x 130..184.

| Punkt | Lage (x,z), Skript | Regions-Test | Bildlage y -10000 → y -1800 → y 0 | Ergebnis |
|---|---|---|---|---|
| 3 | (-5540,-21205) @0x01B40 | innen | (214,-127) → (199,127) → (197,165) | faellt durch die rechte Bildhaelfte auf den Sims bzw. die Kanalkante |
| 5 | (-14476,-23171) @0x01B6A | innen | (118,-26) → (124,94) → (125,116) | **faellt in den Kanal**, hinten |
| 7 | (-23020,-25755) @0x01B94 | innen | (84,2) → (91,83) → (93,99) | faellt hinten am linken Steg, vor der Stirnwand. Der Steg bei x -27000 projiziert auf y 80, dort liegt im Bild etwa der Fuss der Stirnwand (abgelesen). In der Marke faellt er genau hinter Leon |
| 0, 2, 4, 6 | z > -18500 | aussen | — | vom Regions-Test verworfen |
| 1 | (3635,-24307) | aussen | hinter der Kamera | — |

Keine der 10 Masken deckt die Fallwege von 3, 5 und 7. Die Bodenhoehe liefert room_coll, sie ist hier nicht nachgerechnet.
Die Tabelle nennt deshalb y -1800 (Steg) und y 0 (Kanalgrund). Bild: `W_belege/wb1_tropfen_cut2.png`. Gelb ist Punkt 5,
tuerkis Punkt 3, rot Punkt 7, Ringe bei y -1800, -900 und 0.
*Abweichung von `W_abnahme_1.md` §3.2 Punkt 3:* Dort steht, zwei Punkte fielen ins Bild und die uebrigen sechs laegen
hinter Kamera oder Stirnwand. Mit dem Regions-Test und dem FX_LOG des Ports sind es **drei**. Der Port verwendet denselben
Regions-Test und zeichnet in Cut 2 genau die Punkte 3, 5 und 7 (Abschnitt 5.1).

**(d) Was der Port tut.** Die Tropfen haengen bei y -10018..-10000 und fallen nie (Abschnitt 5.1). Zwei Port-Stellen sind die Ursache:
* `re15_esp.c` `esp_fx_spawn_rows_one` setzt `f->floor_y = y`, also die Spawnhoehe. Diese Sammelklemme setzt den Tropfen im
  ersten Bild von Zeile 2 auf den "Boden" -10000 zurueck, danach zittert er mit halber Rueckgabe, siehe Log
  `xlat=(0,-5,0) drift=(0,5,0)`. Das Original hat im Takt keinen Boden. Selbst disassembliert: @0x8001a2e8-388 prueft
  Flags-Bit 5 (`andi v0,v0,0x20` @0x8001a2f0), rechnet xlat += vel (@0x8001a324-360) und vel += accel (@0x8001a354-388),
  ohne jeden Vergleich. Den Boden kennt nur Routine 13.
  Fuer Effekt 0x0B kennt `re15_esp.c` dasselbe Muster schon (Routine 41, `ESP_KEIN_BODEN`).
* `esp_fx_dispatch` hat fuer Routine 13 keinen Fall und landet im `default: break;`. Es gibt also keinen Bodentest und keine
  Aufschlag-Anim 0x26.

**(e) Nebenbefund `RE15_ESP_ROWMACHINE.md`.** In den Korrekturen, Punkt 6, steht "Routine 13 = NUR Room-Drips
(11A0/1260)". Die Raumliste ist unvollstaendig. Zensus ueber alle ESP-Zeilen mit A = 13 (`W_werkzeug/nb1/r13_zensus.py`):

* Repo-Baum: 28 RDTs, jeweils Effekt 6 Sub 0, Zeile 2, accel (0,10,0). Das sind 11A0/11A1, 1260/1261 und alle STAGE2-Raeume
  2000..20B1.
* Version_40%: 6 RDTs, naemlich 2000, 2001, 2030, 2070, 20B0 und 20B1.

Ob ein Skript den Effekt dort startet, sagt der Zensus nicht. Die Zeile ist dort berichtigt.

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
Skript beider Discs und das 80%-Video (2.3, nur Schritt-Spritzer). Zum Skript (2.1, berichtigt): Ortsfest blinkt nichts.
Der Repo-Baum laesst aber alle 19,5 s Deckentropfen fallen, und die 40%-Disc startet sie nicht.
Im 80%-Videoabschnitt Bild 432..516 (2,8 s ohne Figur im Wasser) faellt kein Tropfen. Das ist kein Gegenbeleg, denn dafuer
ist der Abschnitt kuerzer als ein Umlauf von 19,5 s (`W_abnahme_1.md` §3.1).

## 3. Behebung

`engine/src/raumlicht.c`: Fall `0x2000` (Tabelle `s_glanz`, `rl_hash`, 4-Bild-Fenster) entfernt; der Kopfkommentar an der
Stelle nennt Grund und Belege (SCD @0x0174C/@0x0175C/@0x0176C, sub07 @0x01B06..@0x01B94, 40% @0x01568.., sub03
@0x015B6..; Video). In Nachbesserung 1 ist der Kommentar zum Deckentropfen berichtigt (Abschnitt 2.1, 5). `frame` bleibt im Aufruf (Signatur unveraendert, `(void)frame`). `include/re15_raumlicht.h`: Kopf
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

## 5. Nachbesserung 1 (Maengel M1 und M2 aus `W_abnahme_1.md`)

Auftrag: Code bleibt. Berichtigt werden die Aussage zum Original in Kommentaren und Dossier (M1) sowie OFFEN 4 (M2).
Alle Belege habe ich selbst nachgemessen, Abschnitt 2.1 enthaelt die Einzelheiten.

### 5.1 Messung vorher und nachher

Werkzeuge liegen unter `W_werkzeug/nb1/`:
* `wb1_tropfen.py` gibt Schleife, Kamera, Region, Masken und Projektion aus und zeichnet das Bild.
* `wb1_fxlog.py` wertet FX_LOG aus.
* `r13_zensus.py` ist der Zensus der Routine-13-Zeilen.

Fenster-Laeufe mit `W_werkzeug/abnahme1/wa_lauf.py` (eigene PID, gdigrab `hwnd=`, 30 Bilder/s, direct3d, kein AUTOSHOT,
kein SOFTWARE_RENDER). Messkopien `re15_wb1_vorher.exe` und `re15_wb1_nachher.exe`, nach den Laeufen geloescht. Einstellungen:
`RE15_DEBUG_JUMP=2000@90 RE15_FORCE_CUT=2 RE15_FX_LOG=fx.log`, Wartezeit 17 s, Aufnahme 26 s.

| Messung | vorher (HEAD 80826d68e, exe md5 5e4a0526…) | nachher (Kommentare und Doku, exe md5 87c33be9…) |
|---|---|---|
| FX_LOG Effekt 6 Sub 0 in Cut 2, nach dem Regions-Test | genau 3 Punkte: (-5540,-21205) ab F185, (-14476,-23171) ab F365, (-23020,-25755) ab F545 | dieselben 3, dieselben Startbilder |
| Weltlage y der drei, bis F715 / F714 | -10018..-10000, A 3→13, Flags 03, xlat.y -18..0, drift.y -14..19 | gleich |
| Fleck am Bildrand (83,1), Aufnahme 10 Bilder/s | ab Aufnahmebild 207 (64,72,56), danach pulsierend (112,120,104)/(64,72,56); vorher Hintergrund (40,34,27) | ab Aufnahmebild 207 (64,72,56) |
| `.text` von `raumlicht.c.obj` und `test_raumlicht.c.obj` (objdump -d) | — | **gleich** |
| exe `.text` `.data` `.pdata` `.xdata` `.idata` `.bss` | — | **gleich**. 9 Bytes weichen ab: PE-Zeitstempel und Pruefsumme sowie die `__TIME__`-Zeichenkette in `.rdata` (18:00:09 → 20:28:39) |
| ctest `-R "unit_raumlicht\|r41_wasserblink"`, `-j 3`, PowerShell mit `C:\msys64\mingw64\bin` vorn | **5/5 gruen** (88 s) | **5/5 gruen** (89 s) |
| Endstand nach Feinschliff der Kommentare (exe md5 2c451435…) | — | `.text` und `.data` wieder gleich, `.rdata` nur `__TIME__`; ctest 5/5 gruen (89 s) |

Die Code-Aenderung ist null, nur Kommentare haben sich geaendert. Die Messung zeigt das Port-Verhalten, auf das sich die
berichtigten Kommentare beziehen. Bilder:
* `W_belege/wb1_tropfen_cut2.png`: die drei Fallwege auf dem Bild der Marke.
* `W_belege/wb1_port_tropfen7_bildrand.png`: der Fleck des haengenden Punkts 7 im Port.
* `W_belege/wb1_80prozent_tropfenwege.jpg`: die Fallwege im 80%-Video, Ausschnitte.

### 5.2 Was berichtigt ist

* `engine/src/raumlicht.c`, Block ueber `re15_raumlicht_liste` (M1):
  * Effekt 6 Sub 0 ist als Deckentropfen beschrieben, mit Routinen und Adressen.
  * Repo-Baum startet die Schleife (@0x0177C), Version_40% nicht (@0x01598, kein Evt_exec).
  * In Cut 2 fallen die Punkte 3, 5 und 7.
  * Port: Klemme plus fehlende Routine 13, Entscheidung beim Nutzer.
* `include/re15_raumlicht.h:9-12`, `tests/unit/test_raumlicht.c:3-6`, `tests/integration/test_r41_wasserblink.cmake:6-9`:
  * Statt "das Original setzt keinen Effekt aufs Kanalwasser" steht dort jetzt "die Raumdaten setzen kein Licht".
  * Dazu der Verweis auf den Tropfen.
  * Im cmake-Riegel steht zusaetzlich, dass er bei Bau der Tropfen anzupassen ist.
* Dieses Dossier: §1.2 Nachtrag, §2.1 neu (a)..(e), §2.3, §3, "Fuer den Nutzer" und OFFEN 4 (M2).
* `RE15_ESP_ROWMACHINE.md`, Korrekturen Punkt 6: Die Raumliste fuer Routine 13 ist aus dem Zensus ergaenzt (Nebenbefund aus M2).
* Commit 3f565627f bleibt, wie er ist. Seine Aussage "das Original setzt keinen Effekt auf das Kanalwasser" berichtigt dieser Folge-Commit.
* Abweichungen von `W_abnahme_1.md`:
  * Es fallen drei Punkte ins Bild, nicht zwei (§2.1 (c)).
  * Der Port haelt die Tropfen auch durch die Sammelklemme `floor_y = y` fest, nicht nur, weil Routine 13 fehlt (§2.1 (d)).
  * Das 80%-Video werte ich als schwachen Gegenbeleg, nicht als "kein Gegenbeleg" (OFFEN 4).

## Fuer den Nutzer

Das Blinken im Kanal war ein ausgedachter Effekt des Ports: weisse Glanzpunkte, die an sechs festen Stellen des Kanals
fuer je vier Bilder aufleuchteten. Er stammte aus einer falschen Deutung des 80%-Videos; die weissen Wolken dort sind
Spritzer an den Fuessen von Zombies, die durch das Wasser waten. Im Kanal gibt es in den Raumdaten kein Licht.
Der Effekt ist entfernt (Leon und Elza, ROOM2000/2001, alle Kameras). Die hellen Flecken auf deinem Bild bleiben: das ist
der Kanalboden mit den Lampenflecken, der durch die halbdurchsichtige Wasserflaeche scheint, genau so im 80%-Video zu
sehen und unveraendert seit v0.8.29. Der Wasserlauf an der Leiter bleibt.

Berichtigung (Nachbesserung 1): Dass "das Original keinen Effekt auf das Kanalwasser legt", stimmt nur fuer die echte
40%-Disc. In den Raumdaten, die der Port laedt (Repo-Baum, Mod), faellt alle 19,5 s eine Runde Wassertropfen von der
Decke und spritzt am Boden auf. In der Kamera deiner Marke sind es drei Tropfen: einer in den Kanal, einer auf den rechten
Sims, einer hinten auf den linken Steg. Auf der 40%-Disc steht dieselbe Tropfen-Schleife im Skript, wird aber nie gestartet.
Der Port laesst die Tropfen derzeit nicht fallen, sie haengen an der Decke. Einer davon ist als winziger Punkt am oberen
Bildrand zu sehen. Ob die Tropfen kommen sollen, entscheidest du (OFFEN 4).

## OFFEN

1. **ROOM50B0 Cut 2, roter Bodenschein** (`raumlicht.c` Fall 0x50B0, derselbe Commit 4100d2aed): ebenfalls Port-Ergaenzung
   ohne Adresse ("Farben gemessen, Groesse Gestaltung"), blinkt aber nicht. Nicht Teil dieses Befunds, unveraendert; die
   Frage, ob er bleiben soll, gehoert dem Nutzer (Regel "Mod-Inhalt nur mit 80%-Beleg": das Video zeigt dort ein rotes Band).
2. **Zombie-Spritzer beim Waten** (80%-Video, ROOM2000 Cut 2): die echten "weissen Wolken" sind Schritt-Spritzer watender
   Zombies. Ob die Port-Zombies in 2000/2001 im Kanal waten und dabei spritzen (FUN_80045630-Zwilling `wasser_r38.c`,
   NPC-Weg Spur A Punkt 2), ist hier nicht gemessen — in der Marke steht "Gegner im Raum: keine".
3. **BEFUND.md G4** (`analysis/video_80prozent/BEFUND.md` Zeile 43) nennt fuer 2000 noch "zu pruefen"; Ergebnis dieser Spur:
   Schritt-Spritzer der Zombies (Abschnitt 2.3). Die Datei ist nicht angefasst (gemeinsame Datei).
4. **Deckentropfen Effekt 6 Sub 0, ENTSCHEIDUNG BEIM NUTZER** (ersetzt "ob das Original sie fallen laesst, ist nicht
   geprueft"; Befund `W_abnahme_1.md` §3.2, hier nachgemessen in Abschnitt 2.1 und 5):
   * **Was es ist:** Ein fallender Deckentropfen mit Aufschlag-Spritzer, Routine 13 @0x80017990, accel (0,10,0) @0x14A64,
     Bodentest room_coll `jal 0x8001c6e8` @0x800179c4, Aufschlag-Anim 0x26 @0x80017a28.
     Acht Punkte bei y -10000, ein Umlauf dauert 585 Bilder (19,5 s).
   * **Wer ihn startet:** nur der Repo-Baum, also der Mod (main00 @0x0177C `04 ff 18 07`, ROOM2001 `04 ff 18 08`).
     Auf Version_40% steht die Schleife in sub03 @0x015B6, wird aber nicht gestartet (main00 endet @0x01598 `01 00`,
     kein Evt_exec, kein Gosub).
   * **Im Bild von Cut 2 (Mod-Daten):** Punkt 3 fiele auf den rechten Sims, Punkt 5 in den Kanal bei (125,116) und Punkt 7
     hinten auf den linken Steg bei (91,83). Keine Maske deckt die drei.
   * **80%-Video:** Im Abschnitt Bild 232..~496 (ROOM2000 Cut 2, etwa 9 s) ist kein fallender Tropfen zu sehen
     (`W_abnahme_1.md` §3.1). Eigene Sichtung der Fallwege, je 4 bzw. 5 Bilder: `W_belege/wb1_80prozent_tropfenwege.jpg`,
     ebenfalls nichts.
     *Abweichung von der Abnahme:* Ich halte das nicht fuer "kein Gegenbeleg wegen 19,5 s Takt". Die in Cut 2 sichtbaren
     Punkte starten bei t = 180, 360 und 540. Die groesste Luecke zwischen zwei sichtbaren Tropfen ist also 225 Bilder
     (7,5 s), und ein Fall dauert etwa 9 + 45 Bilder. Liefe dieselbe Schleife mit denselben Daten, haette in einem
     9-s-Abschnitt mindestens ein Tropfen zu fallen begonnen. Einzige Ausnahme waeren die ersten 180 Bilder nach dem
     Raumeintritt, und der liegt vor dem Schnitt des Videos. Der Gegenbeleg ist aber schwach: Das Video ist ein
     DivX-Mitschnitt, und wie gross der Tropfen im Bild ist, ist nicht gemessen. Der Port zeigt ihn am Bildrand nur
     1 bis 2 Bildpunkte gross, allerdings angeschnitten.
   * **Regel:** "Mod-Inhalt nur mit 80%-Beleg". Ein 80%-Beleg fehlt. Ohne Entscheidung des Nutzers baut der Port die Tropfen nicht.
   * **Was der Port heute tut** (Port-Fehler, unabhaengig von der Entscheidung): Die drei Tropfen in Cut 2 haengen bei
     y -10018..-10000 (Abschnitt 5.1). Ursache sind die Sammelklemme `floor_y = y` in `re15_esp.c`
     `esp_fx_spawn_rows_one` und das Fehlen von Routine 13 in `esp_fx_dispatch`. Punkt 7 zeichnet der Port ab F545 als
     1 bis 2 Bildpunkte grossen, pulsierenden Fleck bei (83,0..1) am oberen Bildrand
     (`W_belege/wb1_port_tropfen7_bildrand.png`). Punkte 3 und 5 liegen bei y -10000 ueber dem Bild.
   * **Zur Wahl:**
     * (A) Tropfen bauen wie im Repo-Baum. Dafuer braucht es Routine 13 mit room_coll statt der Sammelklemme fuer
       Effekt 6 und die Aufschlag-Anim 0x26. Der Riegel `integration_r41_wasserblink_leon/_elza` ist dann anzupassen,
       weil Punkt 5 in sein Kanal-Fenster faellt.
     * (B) Wie die 40%-Disc: Die Schleife sub07/sub08 in 2000/2001 laeuft nicht. Damit verschwindet auch der Fleck am Bildrand.
     * (C) Bleibt wie jetzt: Die Tropfen haengen, und der Fleck am Bildrand bleibt. Das ist kein Originalverhalten.
     Dieser Befund gilt auch fuer andere Raeume. Zeilen mit Routine 13 stehen in 28 Repo-RDTs (Abschnitt 2.1 (e)).
     Ob deren Skripte sie starten, ist nicht geprueft.
