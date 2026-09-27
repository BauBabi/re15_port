# Elza-Zweig — Bau (Runde 35)

Vorarbeiten: `elza-original.md` (Ermittlung) und `elza-portzustand.md` (Messung),
beide aus den Schwesterbaeumen hierher uebernommen. Dieses Dossier baut darauf auf
und enthaelt nur, was in DIESER Runde gemessen oder gebaut wurde.

---

## 0. Die zwei Original-Groessen (aus der Vorarbeit, hier nur zitiert)

| Original | Leon | Elza | steuert |
|---|---|---|---|
| `DAT_800ACA5C` (byte) | 0 | **4** | PLD-Index, Waffenfamilie, CORE-Bank, Gore-Zweige |
| `DAT_800ACA3C` Bit 31 | 0 | 1 | **RDT-Dateivariante + Startraum** |

Schreiber in TITLE.BIN: `801024c0 sb zero` (Leon = 0) gegen
`801024cc ori v0,zero,0x4` + `801024d4 sb` (Elza = 4); zweiter Schreiber
`801016a4 sll v0,v0,2` + `801016ac sb`. Das Spiegel-Bit setzt derselbe Zweig:
`801024c8 and v0,v0,0x7fffffff` / `801024e4 or v0,v0,0x80000000`, Store
`801024ec sw v0,-13764(at)`.

Raumwahl, FUN_800396fc: `800397e0 lhu v0,0(v1)` (Basis-Dateiindex),
`800397e4 srl a0,a0,31` (Elza-Bit), `800397ec addu a0,a0,v0`.
Stage-0-Tabelle `0x8007429c` laeuft in Schritten von **3** — drei CD-Dateien je Raum,
Elza-RDT = Leon-RDT + 1. Im Port ist der Dateiindex die Raum-Id, die Variante also
deren niedrigste Hex-Ziffer.

---

## 1. NEUE MESSUNG DIESER RUNDE: ROOM1241 IST ELZAS VORSPANN — und er zeigt in die Lobby

Das war die offene Frage der Vorarbeit ("`RE15_NEWGAME_ROOM 0x1240` ist selbst eine
Port-Entscheidung, das gehoert in die naechste Runde"). Sie ist **in den Daten
beantwortet**, nicht durch eine Wahl.

Beide Montage-RDT haben denselben Kopf (`nCut=9`, alles andere 0) und je **genau einen**
`Door_aot_set` in `main00` @Datei-0x051A. Die beiden Saetze unterscheiden sich in
**einem einzigen Byte**, dem Zielraum-Index bei +23 = Datei-0x0531:

```
ROOM1240.RDT @0x051A  3b 00 02 31 00 00 00 00 00 00 00 00 00 00 9a 99 00 00 eb f0 00 00 00 [17] …
ROOM1241.RDT @0x051A  3b 00 02 31 00 00 00 00 00 00 00 00 00 00 9a 99 00 00 eb f0 00 00 00 [03] …
                                                                                            ^^
ROOM1240 @0x0531 = 0x17  ->  Raum 0x17 = ROOM1170  "HELIPORT"
ROOM1241 @0x0531 = 0x03  ->  Raum 0x03 = ROOM1031  "LOBBY"
```

Das sind **Zeichen fuer Zeichen die beiden Raumindizes des Original-Einstiegs**
FUN_8001d22c:

```
8001d29c  lw   v0,-13764(v0)     ; DAT_800ACA3C
8001d2a4  bltz v0,0x8001d324     ; Bit 31 gesetzt = ELZA
8001d2a8  ori  v0,zero,0x17      ; LEON : Raumindex 0x17
8001d2b0  sh   v0,4066(at)
8001d324  ori  v0,zero,0x3       ; ELZA : Raumindex 0x03
8001d32c  sh   v0,4066(at)
```

Raumnamen aus DEBUG.BIN (Basis 0x800c0000, Tabelle 0x800c263c, 26-Byte-Satz je Raum,
Indexformel `(637*stage + 13*raum)*2` aus @0x8001d39c-3c8): 0x03 "LOBBY",
0x17 "HELIPORT", 0x24 "OPENING".

Der Rest der beiden Montagen ist eigenstaendig gebaut, nicht kopiert: `sub02` setzt in
ROOM1240 `Set(3,139,1)`, in ROOM1241 dagegen `Set(3,111,1)` **und** `Set(3,112,1)`
(198 statt 194 Byte, eigene Message-Reihe).

**Folgerung, und damit die Entscheidung dieser Runde:** Der Port darf seine
Vorspann-Montage behalten. Es genuegt, auf den Raum die Original-Variantenregel
anzuwenden — `start_room = 0x1240 | elza_bit` —, und Elza landet ueber ihre eigene
Montage in genau dem Raum, den die EXE-Weiche fuer sie nennt. Leon bleibt auf 0x1240
unveraendert.

---

## 2. WAS GEBAUT WURDE

### 2.1 Die Kodierung: `g_gameflow.character` traegt jetzt 0/4 (der &4-Defekt)

Entschieden wie in Auftrag (3) verlangt: **die Abbildung im Port ist berichtigt**, nicht
der Test. Das Feld ist der PLD-Index, nicht ein 0/1-Schalter — Beleg ist das `sll v0,v0,2`
@0x801016a4, das den Auswahl-Cursor erst zum Byte macht, und die Dateitabelle 0x80073f70
(16 u16 = CD-Index 60..75), in die FUN_800314b0 @0x800314d4 damit indiziert.

* `re15_gameflow.h` — Feldkommentar mit den vier Adressen.
* `re15_gameflow.c` — `re15_gameflow_new_game(int char_index)` rechnet
  `(char_index & 3) << 2` (@0x801016a4). Die Aufrufer uebergeben unveraendert den
  Cursor.
* `re15_char_variant()` = `(character & 4) ? 1 : 0` — der Port-Zwilling von
  `DAT_800ACA3C` Bit 31, abgeleitet statt als zweites Feld gefuehrt, weil TITLE.BIN
  genau diese Ableitung herstellt.
* `RE15_ROOM_BASE(id)` = `id & 0xFFF0` und `re15_room_for_char(base)` = `base | Variante`.

Damit sind `enemy_ai_common.c:4230` und `enemy_ai_re2_zombie.c:1849`
(`(g_gameflow.character & 4) == 0`, Original @0x80104008 `andi v0,v0,0x4`) **zum ersten
Mal erreichbar** — bisher war `character` 0 oder 1 und der Test immer 0.

### 2.2 Startraum

`re15_gameflow.c`: `start_room = re15_room_for_char(RE15_NEWGAME_ROOM)` —
Leon 0x1240, Elza 0x1241. Herleitung + Bytebeleg stehen im Code am Makro.

### 2.3 Spielermodell + Animations- und Waffenbaenke

`platform/pc/main.c` laedt den Spieler nicht mehr aus fest verdrahteten
`PLD/PL00.*`-Dateien, sondern aus der **Familie des Charakters**
(`pl_fam = "PL%02X"` mit `character & 0x0F`): TIM, MD1, Basis-EDD/EMR, W01- und
W03-Spur und die 21 Waffen-PLW.

Neuer Helfer `pc_read_pl_part(stem, teil, &size)`: erst die vorextrahierte Teildatei,
sonst der Schnitt aus dem Container (`re15_pld_part`, dieselbe Regel, die der
Westen-Wechsel schon benutzt). Das war noetig, weil Elzas Satz im entpackten Baum
**unvollstaendig** ist — `PL04.EDD`, `PL04W01.EDD` und `PL04W01.EMR` fehlen als Datei,
liegen aber in `PL04.PLD` bzw. `PL04W01.PLW`.

**Gegenprobe, dass der Schnitt derselbe Inhalt ist** (sha256, 13 von 13 vorhandenen
Teildateien byte-gleich mit dem Containerschnitt):

| Datei | Bytes | sha256[0:16] Datei | sha256[0:16] Schnitt |
|---|---|---|---|
| PL00.EDD | 3160 | 26198800428ae2ec | 26198800428ae2ec |
| PL00.EMR | 57136 | 38ae9ec1f0a7f134 | 38ae9ec1f0a7f134 |
| PL00.MD1 | 28916 | aed21adddd4f4ff2 | aed21adddd4f4ff2 |
| PL00.TIM | 99872 | 3bd14dc62dcfb3e2 | 3bd14dc62dcfb3e2 |
| PL00W01.EDD/.EMR | 1116 / 17768 | bf9f6480… / aa190c4d… | gleich |
| PL00W03.EDD/.EMR | 1208 / 19848 | dcd713b9… / 490debdb… | gleich |
| PL04.EMR/.MD1/.TIM | 56576 / 30428 / 99872 | 35963fd3… / 70bc7937… / 9ed9529c… | gleich |
| PL04W03.EDD/.EMR | 1428 / 19448 | d233153d… / c48046ce… | gleich |

Nur fuer Elza neu erschlossen: `PL04.PLD` dir[0] = EDD, +8, 3156 B, sha 3368cf3e1c732bdc;
`PL04W01.PLW` dir[0]/dir[1] = 1336 B / 17368 B.

Beleg fuer die Familienwahl: PLD-Tabelle 0x80073f70 (FUN_800314b0 @0x800314d4,
`sll v0,v0,1`), Waffen-Basistabelle 0x800741e8 = {76,76,76,76,97,97,97,97,…}
(FUN_80036b68, `lbu` @0x80036df8), Container-Verzeichnis dir[0]=EDD @0x80036be4 /
dir[1]=EMR @0x80036c04.

### 2.4 Der angeforderte Modell-Index (sonst haette der erste Raumwechsel Elza zurueckgetauscht)

`work_vars[0x10]` ist der Port-Zwilling von `DAT_800B0FF0`. Das Original fuellt es beim
Sitzungsstart aus dem Charakter-Byte (`lbu` @0x8001d51c -> `sh a0,4080(at)` @0x8001d558)
und vergleicht es danach in jedem Raumlader gegen die untere Nibble von `aca5c`
(@0x8003976c `andi v0,a0,0xf`, @0x80039770 `beq v0,v1`, sonst @0x80039788
`jal 0x800314b0`). Im Port stand es auf 0 — Elzas eben geladenes PL04 waere beim ersten
Raumwechsel gegen PL00 getauscht worden. Jetzt wird es (samt `s_player_model_idx` und
dem Westen-Spiegel) auf `character & 0x0F` gesetzt.

`re15_savedata.c` rekonstruiert den Index beim Laden aus Flag(3,0x75) (die R.P.D.-Weste,
Leons Variante PL01). Fuer Elza bleibt ihr eigener Index stehen — sie sonst in Leons rote
Uniform zu stecken waere geraten, und eine Elza-Westenvariante ist **nicht** ermittelt.

### 2.5 CORE-SE-Bank und die uebrigen Charakterstellen

* `re15_audio_prime_core(g_gameflow.character)` statt `ch ? 4 : 0` an beiden Stellen
  (New Game + Continue) — Original `lbu a0,aca5c` @0x800316dc, `jal 0x800440c4`
  @0x800316e8, also der Wert selbst, nicht eine Umrechnung.
* Wund-LUT: `(character & 4) ? 1 : 0` statt `== 0`.
* Equip-Pfad: Familie aus `character & 0x0F` statt `== 0 ? "PL00" : "PL04"`.
* Vorspann-Montage-Effekt (2 Stellen) und die Voll-Text-Erzaehlerraeume
  (`scd_vm.c re15_room_full_text`) vergleichen jetzt gegen `RE15_ROOM_BASE`, damit
  ROOM1241/ROOM1171 dieselbe Darstellung bekommen wie ihre geraden Zwillinge.

### 2.6 Was bewusst NICHT angefasst wurde

Der Flag-Vorlauf `if (boot_room == 0x1170 || boot_room == 0x1240) flag_set(3,193,1)`
bleibt **auf den geraden Ids**. Er ist ausdruecklich ROOM1170-spezifisch (der Kommentar
im Port sagt das), und Elzas Kette laeuft gar nicht ueber ROOM1170 — ROOM1241 setzt
seine eigenen Flags in seinem eigenen Bytecode (`Set(3,111,1)`, `Set(3,112,1)`).
Ihn auf die Basis-Id zu oeffnen haette Elza einen Leon-Flag untergeschoben.

---

## 3. MESSUNGEN

Werkzeug: `analysis/befunde_2026-09-27/elza_run.sh` (ein Vollstart, Auswahlschirm per
`RE15_PSELECT_AUTO`, mit `RE15_PSELECT_AUTO_SWITCH` = rechts = Elza) und
`elza_cmp.py` (Pixelvergleich der PPM-Reihen). Beide Staende wurden im SELBEN
Arbeitsbaum gebaut: erst der Stand VOR der Aenderung (Commit 617fef68 in re15_port/),
Lauf, dann der Stand danach (04e8694d), Lauf.

### 3.1 DER WICHTIGSTE RIEGEL: Leon vorher == Leon nachher

| Messgroesse | Leon VORHER | Leon NACHHER |
|---|---|---|
| Auswahl | `pselect done ch=0` | `pselect done ch=0` |
| Startraum | `STAGE1/ROOM1240.RDT (163744 bytes)` | `STAGE1/ROOM1240.RDT (163744 bytes)` |
| Spieler-Spawn | `(-26214,0,-3861) yaw=0` | `(-26214,0,-3861) yaw=0` |
| Koerper-TIM | `384x256 bpp=8 clut=1` | `384x256 bpp=8 clut=1` |
| Koerper-Mesh | `17 meshes` | `17 meshes` |
| Skelett | `PL00: 15 bones, 24 clips, 712 keyframes` | `PL00: 15 bones, 24 clips, 712 keyframes` |
| W01-Spur | `PL00W01: 15 bones, 14 clips, 222 kf` | `PL00W01: 15 bones, 14 clips, 222 kf` |
| W03-Spur | `PL00W03: 15 bones, 14 clips, 248 kf` | `PL00W03: 15 bones, 14 clips, 248 kf` |
| Waffenmeshes | `21/21 PL00W**` | `21/21 PL00W**` |
| Waffenbaenke | `21/21 aus PL00W**.PLW` | `21/21 aus PL00W**.PLW` |
| Raumkette im Lauf | room1240 -> room1170 | room1240 -> room1170 |
| Containerschnitte | — | **keine** (`[pl-part]` kommt im Log nicht vor) |

Die letzte Zeile ist der Grund, warum das bitgenau sein MUSS: fuer Leon liegen alle
Teildateien vor, der neue Helfer nimmt sie und kommt nie zum Schnitt — er liest
buchstaeblich dieselben Dateien wie vorher.

**Bildvergleich, 7 Bilder (Nr. 200/400/600/800/1000/1200/1400):**

```
000200  abweichende Pixel 0 von 691200
000400  abweichende Pixel 0 von 691200
000600  abweichende Pixel 0 von 691200
000800  abweichende Pixel 0 von 691200
001000  abweichende Pixel 0 von 691200
001200  abweichende Pixel 0 von 691200
001400  abweichende Pixel 0 von 691200
SUMME abweichende Pixel: 0
```

### 3.2 Die Wahl hat jetzt Folgen

| Messgroesse | Leon (ch=0) | Elza (ch=1) |
|---|---|---|
| Charakter-Byte | `character=0, Elza-Bit=0` | `character=4, Elza-Bit=1` |
| Startraum | `STAGE1/ROOM1240.RDT (163744 B)` | **`STAGE1/ROOM1241.RDT (163748 B)`** |
| Folgeraum (Tuer der Montage) | **room1170** | **room1031** |
| Koerper-Mesh | 17 meshes | **21 meshes** |
| Skelett | `PL00: 15 bones, 24 clips, 712 kf` | **`PL04: 15 bones, 24 clips, 705 kf`** |
| W01-Spur | `PL00W01: 14 clips, 222 kf` | **`PL04W01: 14 clips, 217 kf`** |
| W03-Spur | `PL00W03: 14 clips, 248 kf` | **`PL04W03: 14 clips, 243 kf`** |
| Waffenmeshes / -baenke | 21/21 PL00W\*\* | **21/21 PL04W\*\*** |
| Containerschnitte | keine | `PL04W01.EDD (+8, 1336 B)`, `PL04W01.EMR (+1344, 17368 B)`, `PL04.EDD (+8, 3156 B)` |

Die drei Schnitte sind genau die drei Teildateien, die im entpackten Baum fehlen —
gemessen, nicht angenommen.

**Bildvergleich Leon gegen Elza, dieselben 7 Bildnummern:**

```
000200  abweichende Pixel 677349 von 691200
000400  abweichende Pixel 687087 von 691200
000600  abweichende Pixel 691147 von 691200
000800  abweichende Pixel 691147 von 691200
001000  abweichende Pixel 690979 von 691200
001200  abweichende Pixel 549591 von 691200
001400  abweichende Pixel 554733 von 691200
SUMME abweichende Pixel: 4542033
```

**Vorher stand hier 0 von 691200 in allen fuenf verglichenen Bildern**
(elza-portzustand.md). Das ist die eigentliche Zahl dieser Runde.

### 3.3 Der Tuer-Uebergang bleibt ungerade

Der Riegel verlangt ausdruecklich, dass der Versatz nicht nur beim Start greift.
Gemessen im Elza-Vollstart:

```
[boot] room RDT: STAGE1/ROOM1241.RDT (163748 bytes)
[room] PC loaded room1031.rdt (219120 bytes)
```

Der Uebergang laeuft ueber den Door_aot_set der Montage, also ueber
`aot_common.c:572-575`, der die Variante aus `g_current_room_id & 0x000F`
mitschleppt. Leon im selben Aufbau: room1240 -> room1170, beide gerade.

### 3.4 Sichtpruefung

`analysis/befunde_2026-09-27/elza_zweig_bilder/`

| Bild | was darauf zu sehen ist |
|---|---|
| `nachher_leon_f1400.png` | Leon auf dem Helipad (ROOM1170), blaue R.P.D.-Uniform, PL00 |
| `elza_lobby_f4000.png` | **Elza in der Lobby (ROOM1031)** vor dem Drehkreuz, PL04 — anderes Modell (21 statt 17 Meshes), andere Haarfarbe, andere Silhouette |
| `nachher_elza_f1400.png` | Elzas Vorspann-Montage (ROOM1241) laeuft zu diesem Zeitpunkt noch (Umbrella-Blende) — ihre Montage ist laenger als Leons |

### 3.5 Testsuite

`bash re15_port/tools/local_build.sh all`:

```
100% tests passed, 0 tests failed out of 360
Total Test time (real) = 161.29 sec
=== LOCAL-BUILD-OK (all) — Tests 360/360
```

Die vier GUI-Haken liefen im selben Durchlauf durch: `integration_boot_bg_pin`
3.60 s, `integration_dark_start_pin` 2.71 s, `integration_relatch_pin` 24.65 s,
`integration_save_counter_pin` 27.78 s — alle *Passed*.

**359 -> 360 ist ein NEUER Haken, keine gesenkte Schranke.** `unit_elza_zweig`
(Test #340, `tests/unit/test_elza_zweig.c`, registriert in
`tests/unit/probes/r35_elza-zweig.cmake`) haelt die fuenf Stellen dieser Runde
fest; `RE15_MIN_TESTS` in `local_build.sh` ist mit angehoben, wie es der
Kommentar dort verlangt. Seine Ausgabe:

```
A  Leon: character=0 variante=0 start=1240
A  Elza: character=4 variante=1 start=1241
B  &4-Tor: Leon nimmt Zweig-A=1, Elza nimmt Zweig-A=0
C  0x1170 -> Leon 1170 / Elza 1171 ; 0x1030 -> Leon 1030 / Elza 1031
D  ROOM1240@0x0531=0x17 (Raum 0x17)  ROOM1241@0x0531=0x03 (Raum 0x03)
E1 Tuer aus 1031 nach Stage 0 Raum 17 -> 1171
E2 geladen: character=4 room=1031 work_vars[0x10]=4
```

Teil **B** traegt seine eigene Gegenprobe: der ALTE Port-Wert 1 muss auf Leons
Seite fallen (`(1 & 4) == 0`). Ohne sie wuerde der Haken nur bestaetigen, dass 4
das Bit hat. Teil **D** liest die beiden Bytes aus der echten ROOM124x.RDT —
faellt der Beleg weg, faellt der Haken. Teil **E2** prueft nicht nur, dass der
Charakter zurueckkommt, sondern auch `work_vars[0x10]`; ohne den waere der Load
eine Taeuschung, weil der naechste Raumwechsel Elza gegen PL00 getauscht haette.

Ein bestehender Haken wurde angepasst: `test_savedata.c` speicherte bisher
`character = 1`. Es bleibt derselbe Rundlauf-Pin, prueft ihn jetzt aber mit 4 —
einem Wert, den das Spiel wirklich annimmt.

---

## 4. WAS HINTER DEM ZWEIG NOCH FEHLT (gezaehlt, nicht geschaetzt)

Der Auftrag war DER ZWEIG, nicht Elzas ganzes Szenario. Ehrliche Restliste:

| # | offen | Zahl / Stelle |
|---|---|---|
| 1 | **Elzas Raum-Inhalte sind ungeprueft.** Die 120 ungeraden RDT existieren lueckenlos (120/120, elza-portzustand.md §5) und werden jetzt geladen — ob ihre Skripte, Gegner und Gegenstaende im Port durchlaufen, ist NICHT gemessen. Fuer Elzas Kette ist bisher genau ein Uebergang gefahren: ROOM1241 -> ROOM1031. | 119 von 120 Raeumen ungefahren |
| 2 | **Vier ROOM1170-Sonderfaelle bleiben auf der geraden Id.** `main.c:3878` (Spawn-Ueberschreibung), `main.c:5886` (Cut 7 schwarz), `main.c:7137`, und der Flag-Vorlauf `main.c:3839`. Alle vier gehoeren zu Leons Helipad-Vorspann; Elzas Kette laeuft nicht darueber, deshalb sind sie hier KEIN Defekt — aber ein Debug-Sprung nach ROOM1171 wuerde sie vermissen. | 4 Stellen |
| 3 | **Elzas Westen-Variante ist nicht ermittelt.** Leons R.P.D.-Weste ist PLD-Index 1 (Flag(3,0x75), ROOM1190/1191). Welchen Index Elza dort bekaeme, steht nirgends belegt; der Ladeweg laesst ihren Index deshalb unveraendert, statt zu raten. | 1 offener Index |
| 4 | **Die Startposition aus DEBUG.BIN.** FUN_8001d22c ueberschreibt die fest verdrahtete Startpose unbedingt aus der Tabelle @0x800c263c (26 Byte je Raum). Ob DEBUG.BIN im Auslieferungsstand zu diesem Zeitpunkt resident ist, ist nicht gemessen (elza-original.md §3). Der RAUM ist davon unberuehrt, die POSE moeglicherweise nicht. | 1 offene Messung |
| 5 | **0x800b0fbe Bit 0 (Elza-Kartentitel).** Im ORIGINAL setzt es keiner der fuenf Schreiber von 0x800aca5c — mit Werten 0/4 ist Bit 0 immer 0 und das Elza-Template @0x800107cc nie erreichbar. Der Port trifft mit `re15_mc_title.c:38` das offenbar Gemeinte; die Original-Luecke bleibt offen. | 1 Original-Luecke |
| 6 | **Das PSX-Target hat keine Charakterwahl.** `RE15_BOOT_ROOM 0x1170` wird dort noch fest benutzt (`platform/psx/main.c:375`, `asset_psx.c:677/751`). Nicht angefasst — dieser Auftrag ist der PC-Zweig. | 3 Stellen |
