# Spur AN: unabhaengige Abnahme 5 (Runde 41, Delta nach Abnahme 4 = Nachbesserung 6)

Gepruefter Stand: Arbeitsbaum `.claude/worktrees/r41_annette`, Zweig `r41/annette`, Kopf 94c0e506b (Code 99bbe97ba). Ich bin nicht
der Autor. Ein Vorgaenger-Abnehmer hatte vor dem Abbruch seiner Sitzung einen Entwurf dieser Datei und Belege unter `AN_belege/abn5/`
unversioniert hinterlassen (Integrationsgruppen offen, seine Gruppe 3 mit fuenf roten Tests ohne Nachfahren). Ich habe das als
Rohmaterial gelesen, aber jede Lesung, jeden Fensterlauf, jede Gegenprobe und jeden Test selbst neu gemacht; die Belege unter
`AN_belege/abn5/` sind die meinen (seine Werkzeuge `abstand5.py`, `vergleich5.py`, `schuss5.py`, `blatt5.py`, `claire_seg_abn5.py`
habe ich gelesen und weiterverwendet). Gebaut habe ich nur mit `local_build.sh build` (keine Arbeit). Geprueft habe ich mit einer
eigenen Messkopie der exe (`scratchpad/annb6/abn5/mein/exe/re15_pc_anabn5.exe`, je Lauf eine Kopie `re15_pc_anabn5_<lauf>_<pid>.exe`,
nur eigene PIDs per `timeout`, alle Kopien danach geloescht). Laufausgaben: `scratchpad/annb6/abn5/mein/laeufe/<lauf>/`.

**Zu welchem Commit gehoert die exe?** (`AN_belege/abn5/delta_abn5.txt`)
- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build` (18:59): `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`.
- `re15_port/build/platform/pc/re15_pc.exe`: 8717260 Byte, sha256 0e9ece98360bd5b0.., 15:58:12. Die Messkopie ist bitgleich.
- Quellen und Objekte: `re15_damage.c` 15:56:59 -> `.obj` 15:57:18; `annette_r41.c` 15:58:02 -> `.obj` 15:58:10; `libre15_engine.a`
  15:58:11; exe 15:58:12; `test_r41_annette.c` 15:53:38 -> `test_r41_annette.exe` 15:58:48. Commit 99bbe97ba: 15:59:57.
- Keine Datei unter `re15_port/` (ausser `build/`) ist neuer als die exe. 94c0e506b aendert nur Doku und Belege. Der Baum war zu
  Beginn sauber bis auf das unversionierte Rohmaterial des Vorgaengers.
- **Die exe gehoert zu 99bbe97ba = Zweigkopf fuer `re15_port`.**
- Assets: `RE15_ASSET_SELFTEST=1` mit der Messkopie ergab `RESULT ok=26 missing=0`, CD-Wurzel
  `.../r41_annette/re15_port/shared_assets/PSX` (`selftest_abn5.txt`).

## 0. Urteil

**BESTANDEN.** Es gibt keinen blockierenden Mangel. M1 der Abnahme 4 ist behoben: Die RE1.5-Annette in 6000/6030 und in 5090
vor der Szene traegt wieder ihren byte-belegten RE1.5-Kasten 450/1530, das RE2-Segment 350/1350 gilt erst ab dem Szenenstart und fuer
die Leiche, die Nachbesserung 5 ist unveraendert. Vier Hinweise (H1-H4), keiner davon ein Mangel am Verhalten; M3 (Stimmen) bleibt
der bekannte Rest.

- **RE1.5-Kasten der 0x4D: selbst gelesen, Code richtig.** STAGE6 und STAGE5 registrieren fuer Typ 0x4D eine eigene Wurzel
  (`sw v0,11488(at)` @0x80101dfc / @0x8011ddb8 -> 0x80072ce0 = 0x80072bac + 0x4D*4); deren INIT setzt +0x78 auf den eigenen Kasten
  `00 00 06 fa 00 00 c2 01 fa 05 c2 01` = {0,-1530,0,450,1530,450} (@0x80102784 / @0x801207d0). Die echte Disc `Version_40%` traegt an
  allen Stellen dieselben Bytes. `re15_enemy_apply_hitbox` gibt der 0x4D wieder r 450 / h 1530 / Versatz -1530; alle Adressen im
  Code-Kommentar, im Commit und im Dossier N6.1 stimmen mit meiner Lesung ueberein.
- **RE2-Segment: selbst gelesen, Umschaltung richtig.** FUN_80034D0C liest +0x1E8 @0x80034d74, +0x84 @0x80034d78, +0x16 @0x80034dac,
  +0x1A @0x80034e5c. Mein eigener Lauf des Claire-RAM-Lesers (lauf3 e01..e30) zeigt fuer Annette in allen 30 Abzuegen r 350, Halbhoehe
  1350, +0x14 -1350. `annette_r41.c` setzt das Segment nur an zwei Stellen: beim Szenenstart (Evt_exec sub04 @0x01296 in 5090) und in
  `ruhe_setzen` (Leiche beim Wiedereintritt). Sonst schreibt kein Code den Kasten einer 0x4D um; ein Neuaufbau des Raums legt die
  Leiche ueber `re15_annette_r41_install` -> `ruhe_setzen` wieder mit dem RE2-Segment.
- **Im echten Fenster gemessen (eigene Messkopie, eigene Laeufe).**
  - RE1.5-Inhalt: Am Pult in 6030 haelt Leon bei 901..903 (Abnahme 4: 802). In 5090 vor der Szene haelt er vor der lebenden 0x4D bei
    901..903. In 6000 wird die laufende 0x4D bei 899..904 um den stehenden Leon herumgefuehrt (`an6000_bahn`); mit dem Messhaken
    haelt Leon 901..903 vom Hakenpunkt.
  - RE2-Inhalt: Die Leiche in derselben Szene (Phase FREI) schiebt Leon bei 750 auf 801. Beim Wiedereintritt geht Leon bei 802 ohne
    Schub an ihr vorbei.
  - Die uebrigen Laeufe (an6000, z6030, s5090, an_vorszene, an_durch, kampf_linie, kampf, elza5091) sind Zeile fuer Zeile gleich mit
    Abnahme 4; z6030 auch mit Abnahme 3. Alle Laeufe, die der Vorgaenger ebenfalls gemacht hat, sind mit seinen bitgleich.
- **Nachbesserung 5 ist unveraendert.** Das hp-Tor steht in `re15_player_weapon_fire` (Zeile 2073) und `re15_resolve_attack`
  (Zeile 3670); `git diff c974552f2 HEAD -- re15_damage.c` zeigt im Code nur die `case 0x4d`-Zeile. Die Schuesse gehen durch die
  lebende 0x4D (`an_durch`) und durch die Leiche auf die G5 (`kampf_linie`: dieselben 44 hp-Wechsel der G5 wie in `kampf`).
- **Elza:** In 5091 gibt es keine 0x4D und keine Szene (`elza5091`, gleich Abnahme 4).
- **Tests (eigene Laeufe):** `unit_r41_annette*` 4/4; Suite 1040/1040 gruen; Integration Gruppe 1 8/8 gruen, Gruppe 2 28/28 gruen;
  Gruppe 3 (Auswahl des Vorgaengers, darunter seine fuenf roten) 8/8 gruen. Beide Gegenproben ausserhalb des Baums selbst gebaut: rot (7 und 5 Fehler) wie
  beim Autor.
- **Bekannt und unveraendert:** Die Stimmen fehlen (M3 seit Abnahme 1).

## 1. Delta seit Abnahme 4 (f4cb69304, 2026-11-07 15:42:55)

| Commit | Zeit | Art | Inhalt |
|---|---|---|---|
| 99bbe97ba | 15:59:57 | Code + Test | `re15_damage.c` `re15_enemy_apply_hitbox`: `case 0x4d: r = 450; h = 1530;` mit den STAGE6/STAGE5-Belegen (vorher 350/1350). `annette_r41.c`: `re2_segment` (r 350, Halbhoehe 1350, Versatz -1350, je mit Adresse), aufgerufen in `re15_annette_r41_evt_exec` beim Szenenstart und in `ruhe_setzen`. `test_r41_annette.c`: `test_leiche_radius` erweitert (6000, 6030 Pult mit Schub-Riegel, 5090 vor/ab Szenenstart, Leiche), `test_szene` mit zwei Kasten-Pruefungen |
| 94c0e506b | 17:01:01 | Doku | Dossier N6 (N6.1..N6.7), Zeilen NB5/NB6 in Abschnitt 0, Vermerk in N5.3; Belege `AN_belege/n6/`, Werkzeug `AN_belege/werkzeug/n6/` |

- Unter `release/`, `pri/`, `synchro/`, `Version_80%/` und `info/` hat sich nichts geaendert (`git diff --stat f4cb69304 HEAD -- ...` leer).
- Gegen c974552f2 (Nachbesserung 5 vor cefd57402) ist `re15_damage.c` im Code gleich bis auf die eigene `case 0x4d`-Zeile mit
  denselben Werten 450/1530. Neu ist also allein das RE2-Segment in `annette_r41.c` (plus Kommentare und Tests).
- Die Commit-Texte sind deutsch, ohne Backticks (per Python gezaehlt: 0), nur ASCII, letzte Zeile `Co-Authored-By: Claude Opus 5.5
  <noreply@anthropic.com>` (die Attributionszeile des Auftrags zur Bauzeit; die neue Zeile "Claude Fable 5.1" gilt ab diesem Bericht).

## 2. Maengel und Hinweise

| # | Schwere | Was | Beleg |
|---|---|---|---|
| H1 | Hinweis (Test-Hygiene) | `test_schuss` ruft nach `ruhe_setzen` noch `re15_enemy_apply_hitbox(a, 0x4Du)` (`test_r41_annette.c` Zeile 726, seit NB4). In diesem Test traegt die Leiche damit 450/1530 statt 350/1350. Die Schussregel haengt nur an hp < 0, das Testergebnis aendert sich nicht; der Kasten-Riegel steht in `test_leiche_radius`. Der Autor nennt den Punkt selbst unter "offen". | Abschnitt 5 |
| H2 | Hinweis (Messung) | In 6000 steht die 0x4D nie still: Spawn @0x00F44 startet die Ankunftsszene (Spieler in pst 4), Spawn @0x00F68 laeuft in Zustand 1 ihre eigene Bahn. Ohne Messhaken zieht sie an einem seitlich stehenden Leon vorbei (`an6000_direkt`, kein Zusammenstoss). Stellt man Leon auf ihre Bahn (`an6000_bahn`), wird sie 32 Bilder lang bei 899..904 um ihn herumgefuehrt, Leon rueckt dabei nur um 12 Einheiten; das ist der RE1.5-Halteabstand 900, der RE2-Wert waere 800. Der 6000-Kasten ist zusaetzlich im Unit-Riegel direkt an den Feldern geprueft. | 6.1 |
| H3 | Hinweis (Umgebung) | Die Abnahme-4-Laeufe `z6030`/`s5090` liefen ohne `RE15_AUDIO_CAP_SYNC`, meine mit (wie `lauf4.sh` es vorsieht). Die state.log-Dateien sind trotzdem Zeile fuer Zeile gleich; der Ton-Mitschnitt aendert den Spielzustand nicht. | `auswertung_s5090.txt` |
| H4 | Hinweis (Tests, Umgebung) | Der Vorgaenger-Abnehmer hatte in seiner Integrationsgruppe 3 fuenf rote Tests hinterlassen (r41_masken36_teile_c2 Failed, c3/d2/g5_westen/g5_tod `Exit code 0xc0000142`, 18:13-18:49, waehrend die Sitzung ans Limit lief; das PPM-Werkzeug von c2 starb mit demselben Code). Meine Wiederholung derselben fuenf Tests nacheinander (`-j 1`, 20:24-20:56): **alle gruen** (c2 469,19 s, c3 447,99 s, d2 457,84 s, g5_westen 285,35 s, g5_tod 265,78 s). Die roten Ergebnisse waren Umgebung (Prozessstart unter Last, Memory "GUI-Tests flattern bei parallelen Agenten"), keine Regression. | 7 |
| M3 | bekannt | Stimmen fehlen (6030 msg 27-30, 5090 msg 40-43), unveraendert seit Abnahme 1 (Abnahme 4 M3) | debug.log |

Nicht Teil von NB6, nur zur Einordnung: Nach dem Laden eines Raums gibt es in Bild 1-5 keinen Schub (die 0x4D hat noch hp 0, der
NPC-INIT kommt bei F6). Am Pult steht Leon dort bei 810, also innerhalb von 900; ab Bild 6 wird er auf 901 geschoben. Mit dem Wert aus
Abnahme 4 war es genauso (siehe dort H2).

## 3. RE1.5-Kasten der 0x4D (selbst gelesen)

Eigene Lesung der Bytes (Python, Datei-Offset = Adresse - 0x80100000, Overlay ohne Kopf) und eigene Dekodierung der Instruktionen
(op/rs/rt/imm). Quelle `re15_port/shared_assets/PSX/BIN/STAGE6.BIN` (10324 Byte, sha256 3c9a9ace..) und `STAGE5.BIN` (133280 Byte,
sha256 5924508c..), beide bitgleich mit `info/Re1.5/PSX/BIN`. Beleg `AN_belege/abn5/re15_kasten_4d_abn5.txt`.

| Stelle | STAGE6.BIN (6000/6030) | STAGE5.BIN (5090) |
|---|---|---|
| Registrierung in 0x80072bac + 0x4D*4 = 0x80072ce0 | `3c028010` lui v0,0x8010 @0x80101df0; `244217a0` addiu v0,v0,6048 @0x80101df4; `ac222ce0` sw v0,11488(at) @0x80101dfc -> Wurzel 0x801017a0 | `3c028012` lui v0,0x8012 @0x8011ddac; `2442d6d4` addiu v0,v0,-10540 @0x8011ddb0; `ac222ce0` sw v0,11488(at) @0x8011ddb8 -> Wurzel 0x8011d6d4 |
| Zustandstabelle der Wurzel | `3c018010` / `24212794` @0x80101874/78 -> 0x80102794, [0] = INIT 0x80101918, [4] = 0x80101ac0 | `3c018012` / `242107e0` @0x8011d7a8/ac -> 0x801207e0, [0] = INIT 0x8011d84c, [4] = 0x8011d9f4 |
| INIT: Kasten-Zeiger nach +0x78 | `8c422790` lw v0,10128(v0) @0x80101958 ([0x80102790] = `84 27 10 80` = 0x80102784); `ac620078` sw v0,120(v1) @0x80101960 | `8c4207dc` lw v0,2012(v0) @0x8011d88c ([0x801207dc] = `d0 07 12 80` = 0x801207d0); `ac620078` sw v0,120(v1) @0x8011d894 |
| Kasten-Bytes | @0x80102784 `00 00 06 fa 00 00 c2 01 fa 05 c2 01` | @0x801207d0 `00 00 06 fa 00 00 c2 01 fa 05 c2 01` |
| = {ofs_x, ofs_y, ofs_z, r_min, h, r_max} | {0,-1530,0,450,1530,450} | {0,-1530,0,450,1530,450} |

Leser des Kastens (selbst dekodiert):
- Wurzel STAGE6: Bodenprobe `0c006f58` jal 0x8001bd60 @0x80101844; Schub `0c00abb1` jal 0x8002aec4 @0x801018a8; Wand-Klemme
  `8c820078` lw v0,120(a0) @0x801018c4 / `94450006` lhu a1,6(v0) @0x801018cc / `0c00ec29` jal 0x8003b0a4 @0x801018d0.
  Wurzel STAGE5 dasselbe Muster @0x8011d778 / @0x8011d7dc / @0x8011d7f8-804.
- PSX.EXE FUN_8001bd60: `8c430078` lw v1,120(v0) @0x8001bd94, `94650006` lhu a1,6(v1) @0x8001bd9c, `0c00edfc` jal 0x8003b7f0
  @0x8001bda4 mit `00052823` subu a1,zero,a1 @0x8001bda8.
- PSX.EXE FUN_8002aec4: `8e760078` lw s6,120(s3) @0x8002af38, `8e550078` lw s5,120(s2) @0x8002af3c, `96cc0006` lhu t4,6(s6)
  @0x8002af40, `96ac0006` lhu t4,6(s5) @0x8002af4c (ghidra1_V2.txt Zeilen 117479-117484 stimmen ueberein).
- Spieler-Kasten PSX.EXE Datei 0x64694 = dieselben 12 Bytes {0,-1530,0,450,1530,450}; damit ist der Halteabstand 450 + 450 = 900.

Gegenprobe an der echten Disc (`C:/workspace/git/reAi_v2/Version_40%/PSX/BIN`, Memory "info/Re1.5 ist Fan-Mod"): STAGE6.BIN
(sha256 50a6dd49..) und STAGE5.BIN (sha256 96924dd1..) sind als Dateien anders als im Mod-Baum, aber an allen 19 gelisteten Stellen je
Stage (Registrierung, Tabelle, INIT, Zeiger, 12 Kasten-Bytes, Leser) stehen dieselben Bytes. Der Kasten ist kein Mod-Inhalt.

Zensus (eigener Lauf von `abn4/werkzeug/typ4d_zensus.py` ueber alle 240 RDT): Typ 0x4D wird nur in 5090 @0x0122A, 6000 @0x00F44 und
@0x00F68 sowie 6030 @0x010D4 und @0x010F8 gesetzt. Die Skriptstellen habe ich mit `scd_dump_room.py` nachgesehen (im Beleg).

Alle Adressen, Woerter und Bytes aus Commit 99bbe97ba, dem Code-Kommentar in `re15_damage.c` und Dossier N6.1 stimmen mit meiner
Lesung ueberein. Der Wert `case 0x4d: r = 450; h = 1530;` (Zeile 3856) ist byte-genau belegt.

## 4. RE2-Koerpersegment (selbst nachgelesen)

Beleg `AN_belege/abn5/re2_segment_abn5.txt`.

- FUN_80034D0C (Leon-EXE, `ghidra_re2_Leon.txt` Zeilen 128869-128947): Tore @0x80034d48-6c; Segmentzahl `lw v0,0x1e8(s6)`
  @0x80034d74; Segmente `addiu s5,s6,0x84` @0x80034d78; Radius `lhu s7,0x16(s5)` @0x80034dac; Radiussumme `addu s0,s7,v1`
  @0x80034de8; Halbhoehe `lhu v0,0x1a(s5)` @0x80034e5c; Hoehen-Tor als Summe beider Halbhoehen `addu a0,a0,v0` / `subu t0,zero,a0`
  @0x80034e64/68. Das sind die vier Adressen aus Code und Commit.
- Claire-RAM ROOM6141 lauf3 e01..e30 (Savestates und SLES_109.72 aus der Sitzung des Autors, Pfade im Beleg), eigener Lauf des
  Lesers `werkzeug/claire_seg_abn5.py` (RAM-Basis per Suche des EXE-Textanfangs): Annette steht in allen 30 Abzuegen bei
  @0x80125d4c (em[0] @0x800c79b0, Id 0x49) mit hp -1, Segmentzahl 1, +0x14 = -1350, Radius 350, Halbhoehe 1350, Segmentmitte =
  Lage - 1350 (lebend e01..e22 Lage y 0 -> Mitte -1350, liegend e23..e30 Lage y 900 -> Mitte -450). 30 von 30.
- Port: `re2_segment` setzt `hit_radius_min = hit_radius_max = 350`, `hit_height = 1350`, `hit_offset = (0,-1350,0)`. Das ist
  dieselbe Konvention wie `re15_enemy_apply_hitbox` (Versatz = -h, h als Halbhoehe), also passt die Abbildung.

## 5. Umschaltung im Code (gelesen)

- `re15_annette_r41_evt_exec` (`annette_r41.c` ab Zeile 708): `re2_segment(a)` (Zeile 735) steht hinter allen Abbruchpfaden (falscher
  Raum oder falsche Sub, Phase LEICHE -> Kampfstart, Phase nicht AUS oder Annette steht nicht da, keine Form-Bank -> "keine Szene")
  und im selben Aufruf, der `k_prog` statt sub04 zurueckgibt. Faellt die Szene aus, bleibt der RE1.5-Kasten. ROOM5090 selbst
  nachgesehen: sub01 `Ck (3,42)=0` @0x01288, `Cmp(0x0A,12)` @0x01290, `Evt_exec sub04` @0x01296, `Set(3,42)=1` @0x0129A.
- `ruhe_setzen` (Zeile 873): `re2_segment(a)` (Zeile 880) direkt nach `hp = -1`, vor `liegen(a)`. Einziger Weg in die Phase RUHE.
- Neuaufbau desselben Raums: `re15_annette_r41_install` (Zeile 639, aus `scd_room_setup.c:527`) setzt das Modul bei
  `phase >= FREI` zurueck; der Spawn @0x0122A gibt dann zuerst 450/1530, und der naechste Takt legt mit `leiche_da()` ((3,89),
  (9,133), (3,42)) die Leiche per `ruhe_setzen` wieder mit dem RE2-Segment. Im Lauf `leiche_west` steht das Protokoll bei F6 und F100.
- Weitere Schreiber: `grep` auf `hit_radius_min =`, `hit_height =`, `hit_offset_y =` in `engine/src` und `platform/pc` findet fuer
  eine 0x4D keinen ausser `re2_segment`. `re15_enemy_apply_hitbox` laeuft fuer die 0x4D nur beim `Sce_em_set` (`scd_vm.c:3931`);
  `re15_enemy_ai_init` (`enemy_ai_common.c:194`) wird nur aus dem Zustandsdispatch der Gegner gerufen, die NPC-Familie 0x40..0x4D
  laeuft ueber `re15_npc_ai_tick` (Zeile 15450 ff.) und kommt dort nicht hin. Die Zugstart-Szene 6030 (`zug_evt_exec`/`zug_tick`)
  schreibt keinen Kasten.
- Nachbesserung 5 unveraendert: die hp-Tore `if (e->type >= 0x40u && e->type <= 0x4Du && e->hp < 0) continue;` stehen in
  `re15_player_weapon_fire` (`re15_damage.c:2073`) und `re15_resolve_attack` (Zeile 3670). `git diff c974552f2 HEAD --
  re15_damage.c` zeigt im Code nur die eigene `case 0x4d`-Zeile mit denselben Werten 450/1530 wie vor cefd57402.

## 6. Spiel: eigene Laeufe mit der Messkopie

Alle Laeufe liefen im echten Fenster (Renderer des Nutzers, ohne AUTOSHOT und ohne SOFTWARE_RENDER) und nacheinander (Ketten
`werkzeug/alle.sh` und `alle2.sh`, Lauf `werkzeug/lauf.sh`; die erste Kette habe ich nach vier Laeufen wegen eines falschen
Eingabepfads abgebrochen und den Rest als `alle2.sh` neu gestartet). Bilder per `RE15_FRAMEDUMP`, Ende per `RE15_EXIT_AT`, alle mit
exit 0. Die Umgebungen sind 1:1 die der Abnahme 4 (`abn4/werkzeug/alle4.sh`, `alle4b.sh`, immer mit `RE15_AUDIO_CAP_SYNC`), die
Eingabeketten die Originale der Abnahme 3 (`abnahme3/werkzeug/eingabe_kette*.txt`). Je Lauf eine Auswertung
`AN_belege/abn5/auswertung_<lauf>.txt` (`werkzeug/auswerten.sh`): env, Vergleich der state.log Zeile fuer Zeile mit Abnahme 4 (exe
cefd57402, 0x4D ueberall 350, `scratchpad/w2abn/an4/laeufe/`), mit dem Vorgaenger-Abnehmer (exe 99bbe97ba,
`scratchpad/annb6/abn5/laeufe/`) und, wo vorhanden, mit Abnahme 3 (exe b646c72a6, 0x4D ueberall 450, `re15_port/build/abn3/`); dazu
Abstand Spieler <-> 0x4D, Schuesse/hp-Wechsel und das Annette-Protokoll. Die Bildleisten `*_abn5.jpg` habe ich alle angesehen
(Feldzeile: Bild, Spielerlage, Abstand zur 0x4D).

### 6.1 RE1.5-Inhalt: lebende 0x4D mit dem RE1.5-Kasten (Soll: Halteabstand 450 + 450 = 900)

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `an6030_pult` | wie Abnahme 4: 6030 vor dem Tor ((4,23)=1, (3,89)=0, sub07 @0x010F8, sie steht am Pult (-1790,-22272)), Leon 810 westlich, geht ab F100 2 s auf sie zu | F1..F5 810 (vor dem NPC-INIT, hp 0), **F6 auf 901 geschoben, F100..F400 beim Gehen 901..903**. Gegen Abnahme 4 (802) sind 395 Zeilen anders, nur der Spielerteil, ab F6. Bitgleich mit dem Lauf des Vorgaengers (651/651). |
| `an5090_vor_schub` | 5090 VOR der Szene ((3,89)=1, (9,133)=1, (3,42)=0), Leon in Wagen C (24800,-23550), die lebende 0x4D (Zustand 4, hp -1) per Messhaken ab F120 auf (22000,-23550), Leon geht 3 s nach Westen | **ab F158 bei 901..903 gehalten** (x 22901..22903). Mit dem RE2-Wert waeren es 801. Die Szene startet nicht (keine `[annette]`-Zeile). Bitgleich mit dem Vorgaenger (581/581). |
| `an6000_bahn` (neu) | 6000 mit (3,100)=1 und (3,239)=0 -> Spawn @0x00F68 ohne Szene, kein Messhaken. Leon steht ohne Eingabe auf ihrer Bahn (5131,-38) (Bahnpunkt F21 aus `an6000_direkt`), Blick -x | Sie laeuft in ihn hinein: F16 984, **F17..F48 899..904** (Minimum 899 bei F39/F40), dabei wird sie um ihn herumgefuehrt ((4235,65) -> (4236,24) -> (4382,448) -> (5719,650)), Leon rueckt nur von 5131 auf 5143; ab F49 entfernt sie sich (934, 944, ...). Halteabstand 900 = RE1.5-Kasten; mit dem RE2-Segment waeren es 800. |
| `an6000_direkt` (neu) | wie `an6000_bahn`, aber Leon seitlich bei (6400,1600), ab F100 geht er 2 s nach Westen | Kein Zusammenstoss: sie zieht ihre eigene Bahn (Bogen nach Suedosten, dann Nordosten) an ihm vorbei, naechster Abstand 901 bei F31, Leon unverrueckt; danach folgt sie ihm mit ~2970. Als Messung des Kastens nicht verwertbar, nur Bahnbeleg fuer `an6000_bahn`. |
| `an6000_schub2` | wie der Vorgaenger: Leon (3150,1600) Blick +x, die 0x4D per Messhaken ab F20 auf (5000,1600), Leon geht ab F100 2 s | Leon haelt ab F112 bei x 4097..4099 = 901..903 vom Hakenpunkt (der Haken setzt sie vor dem Schub, ihr eigener Takt traegt sie danach auf (5198,1622), deshalb steht im state.log 1100). Bitgleich mit dem Vorgaenger (551/551). Stuetzmessung. |
| `an6000` | wie Abnahme 4: Gespraech sub02, sie geht in den Zug | **bitgleich mit Abnahme 4** (1751/1751). Kleinster Abstand 1030 (F20), keine Beruehrung, 350 und 450 unterscheiden sich hier nicht. |
| `z6030` | wie Abnahme 4: Kette 6000 -> 6030, Zugstart-Szene, Abgang (F1..F1700) | **bitgleich mit Abnahme 4 und mit Abnahme 3** (je 1968/1968). Die Zugstart-Szene (RE1.5-Raum) haengt nicht am Kasten. |

Bildleisten: `an6030_pult_abn5.jpg`, `an5090_vor_schub_abn5.jpg`, `an6000_bahn_abn5.jpg`, `an6000_direkt_abn5.jpg`,
`an6000_schub2_abn5.jpg`, `an6000_abn5.jpg`, `z6030_abgang_abn5.jpg`.

### 6.2 RE2-Inhalt: Szene und Leiche mit dem RE2-Segment (Soll: Schub erst unter 350 + 450 = 800)

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `s5090` | wie Abnahme 4: Kette bis 5090, Szene (F1..F1965) | **bitgleich mit Abnahme 4** (4329/4329). Szenenschritte: F1168 Szenenstart, F1350 Sturz, F1365 gelandet, F1598 Hieb, F1640 Treffer, F1674 Leiche, F1776 Kampfstart. Gegen Abnahme 3 (450) sind 54 Zeilen anders, nur F1912..F1965 (Weg der Form an der Leiche vorbei, Wirkung des Leichen-Radius 350, Abnahme 4 H6). |
| `s5090_leiche750` | wie `s5090`, ab F1860 (Kampfstart F1776, Leon frei, pst 1) die Leiche (Phase FREI, dieselbe Szene) per Messhaken 750 westlich von Leon auf (13025,-23550) | bis F1859 gleich mit `s5090`; **im Bild F1860 wird Leon von 13775 auf 13826 geschoben = Abstand 801** (350 + 450 + 1). Mit 450 waeren es 901. Bitgleich mit dem Vorgaenger (4269/4269). |
| `leiche_west` | wie Abnahme 4: Wiedereintritt (3,42)=1, Leon geht nach Westen an der Leiche vorbei | **bitgleich mit Abnahme 4** (1351/1351). Minimum 802 bei F398, kein Schub. Gegen Abnahme 3 (450, dort auf 900 geschoben) sind ab F393 708 Zeilen anders, nur der Spielerteil. Protokoll: `ruhe_setzen` bei F6 (Cut 0) und F100 (Neuaufbau, Cut 14), beide Male "Leiche liegt". |
| `kampf` | wie Abnahme 4: Kampf mit der Pistole (F1..F5400) | **bitgleich mit Abnahme 4** (7764/7764). 144 Schuesse, 44 hp-Wechsel (Form-Rueckstoss, G5 600 -> -1), Leon in allen 7764 Zeilen hp 100. Gegen Abnahme 3 sind 194 Zeilen anders, nur F1912..F2105 (Weg der Form). |

Bildleisten: `s5090_szene_abn5.jpg`, `s5090_leiche750_abn5.jpg`, `leiche_west_abn5.jpg`, `kampf_abn5.jpg`.

### 6.3 Schuesse durch Annette (Nachbesserung 5 unveraendert)

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `an_durch` | wie Abnahme 4: lebende 0x4D (jetzt Kasten 450) per Messhaken auf Leons Schusslinie (600,-23550), 6 Schuesse | **bitgleich mit Abnahme 4** (651/651). Schuesse F202, F224, F246, F268, F290, F312. Die 0x4D bleibt bei hp -1 / Zustand 4; die einzigen hp-Wechsel sind F6 (INIT) und F100 (Form 300 -> 150 beim Neuaufbau, wie in Abnahme 4). |
| `an_vorszene` | wie Abnahme 4: 4 Schuesse von der Wagentuer, danach die Szene | **bitgleich mit Abnahme 4** (1551/1551). Szene vollstaendig: F380 Szenenstart, F562 Sturz, F852 Treffer, F988 Kampfstart. |
| `kampf_linie` | wie Abnahme 4: die Leiche ab F1790 per Messhaken auf der Linie Leon -> G5 (9000,-23550) | **bitgleich mit Abnahme 4** (5764/5764). 68 Schuesse; die 44 hp-Wechsel der G5 sind dieselben wie in `kampf` und in Abnahme 4 (diff leer). Bei F2377 stehen Leon (13775,-23550), die Leiche (9000,-23550, hp -1, Zustand 0) und die G5 (-2006,-23400, 600 -> 585). **Die Schuesse gehen durch die Leiche auf die G5.** |

Bildleisten: `an_durch_abn5.jpg`, `an_vorszene_abn5.jpg`, `kampf_linie_abn5.jpg`.

### 6.4 Elza

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `elza5091` | wie Abnahme 4: 5091, Elza (PL04), 4 Schuesse, dann zum Ausloeser | **bitgleich mit Abnahme 4** (1551/1551). Keine `t=4d`-Zeile im state.log, keine `[annette]`-Zeile im debug.log. Ohne weitere Eingabe frisst die G5 Elza (hp -1), wie in Abnahme 4. |

Bildleiste: `elza5091_abn5.jpg`.

## 7. Tests (eigene Laeufe)

Git-Bash, `PATH=/c/msys64/mingw64/bin:$PATH`, `SDL_ASSERT=always_ignore`, `--test-dir <Baum>/re15_port/build` (Werkzeug
`werkzeug/tests.sh`). Die Summenzeilen stehen woertlich in `AN_belege/abn5/ctest_*.txt`, die Zeiten in `ctest_zeiten.txt`.
Waehrend der Laeufe arbeiteten andere Agenten mit eigenen exe-Kopien im selben Fenster-System (`re15_pc_r41g2_*`, `re15_pc_formabn3_*`).

| Lauf | Ergebnis |
|---|---|
| `unit_r41_annette`, `_se_bank`, `_se_toene`, `_se_lagen` (`-V`) | **`100% tests passed, 0 tests failed out of 4`**. test_leiche_radius woertlich: 6000 lebend 450/450/1530 Versatz (0,-1530,0); 6030 Pult ebenso, Leon bei 850 -> 901, bei 810 -> 901, bei 950 -> 950; 5090 vor der Szene 450/450/1530, ab Szenenstart 350/350/1350 Versatz (0,-1350,0); Leiche 350/350/1350, Schub bei 850: 850, bei 750: 801; `test_r41_annette: OK` |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|titelversion\|android\|paket\|r37_masken36_ausschluss` (1040 Tests, `-j 2`, 19:26-19:35, parallel zum Ende der Fensterlaeufe und zum Anfang der Gruppe 1) | **`100% tests passed, 0 tests failed out of 1040`** (549,58 s) |
| Integration Gruppe 1 (Liste des Autors, `-j 1`): r35_zgirl_5090, r36_story_leon, r36_story_elza, r41_annette, r41_begleiter_schusslinie, r41_kollision_npc3070, r41_kollision_npc3001, r41_birkin_pistole (19:30-19:55) | **`100% tests passed, 0 tests failed out of 8`** (1460,11 s; r35_zgirl_5090 153,71 s, r36_story_leon 375,99 s, r36_story_elza 215,50 s, r41_annette 249,80 s, r41_begleiter_schusslinie 52,98 s, r41_kollision_npc3070 83,71 s, r41_kollision_npc3001 85,07 s, r41_birkin_pistole 243,26 s) |
| Integration Gruppe 2 (Liste des Autors, `-j 1`): r32_tor_hell, r35_zgirl, r36_elza_* (5), r37_elzaems_* (12), r37_sperre1050, r37_szene11c0, r38_elzatoene_* (2), r38_tagebuch_* (3), r41_wasser20b0, r41_wasser20b0_npc (28; 19:55-20:24) | **`100% tests passed, 0 tests failed out of 28`** (1748,01 s; laengste: r35_zgirl 259,38 s, r37_sperre1050 212,11 s, r41_wasser20b0_npc 215,91 s, r37_szene11c0 125,17 s) |
| Integration Gruppe 3 (Auswahl des Vorgaengers: 5090/6000/6030-Tests ausserhalb der Autorliste, `-j 1`), zuerst seine fuenf roten (`ctest_integ3rot.txt`, 20:24-20:56), dann seine drei gruenen (`ctest_integ3rest.txt`, 20:56-21:13) | **`100% tests passed, 0 tests failed out of 5`** (1926,22 s: r41_masken36_teile_c2 469,19 s, teile_c3 447,99 s, teile_d2 457,84 s, r41_g5_westen 285,35 s, r41_g5_tod 265,78 s) und **`100% tests passed, 0 tests failed out of 3`** (1016,49 s: r35_granate 168,26 s, r41_masken36_zellen3d 391,14 s, teile_c1 457,01 s) = 8/8 gruen |

**Gegenproben selbst gebaut** (ausserhalb des Arbeitsbaums, `werkzeug/gp.sh` -> `scratchpad/annb6/abn5/mein/gp/`, Ergebnis
`gegenproben_abn5.txt`): `test_r41_annette.c.obj` des Baums mit Ersatz-Objekt und `libre15_engine.a` gelinkt, der Baum blieb unberuehrt.
- basis (ohne Ersatz): `test_r41_annette: OK`.
- gp1 (`re15_damage.c` aus cefd57402, 350/1350): **rot, 7 Fehler** (6000, 6030, 5090 vor der Szene und Wiedereintritt-Spawn 350 statt
  450; Pult "Leon bei 850 nicht auf 900 geschoben (Abstand 850)", "bei 810 ... (Abstand 810)").
- gp2 (`annette_r41.c` ohne die beiden `re2_segment`-Aufrufe, `werkzeug/gp2_ohne_umschaltung.diff`): **rot, 5 Fehler** (ab
  Szenenstart und als Leiche 450/1530; die Leiche schiebt bei 850 und bei 750 auf 901).
- Dieselben Ergebnisse wie beim Autor (`n6/gegenprobe_*.txt`). Der Riegel greift in beide Richtungen.

## 8. Belege

`analysis/befunde_runde41/AN_belege/abn5/` (eigene Dateien dieser Abnahme; das unversionierte Rohmaterial des Vorgaengers mit
denselben Namen ist ersetzt, seine Laeufe `an6000_schub`/`an6000_wandtest` sind durch `an6000_bahn`/`an6000_direkt` abgeloest):

| Datei | Inhalt |
|---|---|
| `delta_abn5.txt` | Commits, Diffstat, unberuehrte Ordner, Bau (`no work to do`), Zeitstempel, sha256 von exe und Messkopie, Commit-Text-Pruefung |
| `re15_kasten_4d_abn5.txt` | STAGE6/STAGE5: Registrierung, Wurzel-Tabelle, INIT, Zeiger, Kasten-Bytes, Leser, je mit Disc-40%-Vergleich; PSX.EXE-Leser; Zensus; Skriptstellen |
| `re2_segment_abn5.txt` | FUN_80034D0C (Ghidra-Zeilen) und die eigene Lesung des Claire-RAM lauf3 e01..e30 (Zusammenfassung + Rohausgabe) |
| `gegenproben_abn5.txt` | Ausgaben von basis, gp1 und gp2 |
| `selftest_abn5.txt` | `RE15_ASSET_SELFTEST=1` der Messkopie (26/26) |
| `ctest_unit_annette.txt`, `ctest_suite.txt`, `ctest_integ1.txt`, `ctest_integ2.txt`, `ctest_integ3*.txt`, `ctest_zeiten.txt` | ctest-Ausgaben woertlich |
| `auswertung_<lauf>.txt` | je Fensterlauf: env, Ende, Vergleich mit Abnahme 4 / Vorgaenger / Abnahme 3, Abstand, Schuesse/hp, Annette-Protokoll |
| `ref_abn4_kampf_schuss.txt`, `ref_abn4_kampf_linie_schuss.txt`, `ref_vorgaenger_kampf_schuss.txt` | Schuss-/hp-Listen der Vergleichslaeufe |
| `*_abn5.jpg` | Bildleisten aus den RE15_FRAMEDUMP-Serien (Feldzeile: Bild, Spielerlage, Abstand zur 0x4D aus dem state.log) |
| `werkzeug/` | eigene: `lauf.sh`, `alle.sh`, `alle2.sh`, `nachlauf.sh`, `auswerten.sh`, `blaetter.sh`, `tests.sh`, `gp.sh`, `gp2_ohne_umschaltung.diff`; weiterverwendet vom Vorgaenger: `abstand5.py`, `vergleich5.py`, `schuss5.py`, `blatt5.py`, `claire_seg_abn5.py`, Eingaben `eingabe_kette*.txt` (Kopie aus `abnahme3/werkzeug/`) |

Nicht versioniert sind die Rohdaten (state.log, debug.log, ppm, ton.raw) unter `scratchpad/annb6/abn5/mein/laeufe/`, die
Gegenprobe-Bauten unter `scratchpad/annb6/abn5/mein/gp/` und die Claire-Savestates im Scratch der Autor-Sitzung.

Ich habe keinen Code und keine Datei unter `pri/`, `synchro/`, `Version_80%/` oder `info/` geaendert. Im Arbeitsbaum habe ich nur
`local_build.sh build` gerufen (nichts zu tun) und ctest laufen lassen (Arbeitsordner unter `re15_port/build/`). Alle exe-Kopien
`re15_pc_anabn5*` liefen unter `timeout` auf die eigene PID und sind geloescht; die abgebrochene erste Kette habe ich nur ueber eigene
PIDs beendet (`kill`, `Stop-Process -Id`, nie `taskkill /IM`). Kein Push, kein Merge.
