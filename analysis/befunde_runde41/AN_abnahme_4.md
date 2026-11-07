# Spur AN: unabhaengige Abnahme 4 (Runde 41, Delta nach Abnahme 3)

Gepruefter Stand: Arbeitsbaum `.claude/worktrees/r41_annette`, Zweig `r41/annette`, Kopf cefd57402, Baum sauber. Ich bin nicht der
Autor und habe nichts gebaut. Geprueft habe ich mit einer eigenen Kopie der exe des Arbeitsbaums (Messordner
`scratchpad/w2abn/`, Laeufe unter `w2abn/an4/`, exe-Namen `re15_pc_an4_*`, nur eigene PIDs per `timeout`).

**Zu welchem Commit gehoert die exe?**
- `re15_port/build/platform/pc/re15_pc.exe`: 8717212 Byte, sha256 5e058b2f7f26ac6d.., Zeitstempel 2026-11-03 10:29:54. Die Kopie
  ist bitgleich.
- Letzte Quellaenderung: `re15_damage.c` 10:29:43. Danach wurden `re15_damage.c.obj` (10:29:51), `libre15_engine.a` (10:29:52) und
  die exe (10:29:54) gebaut, auch laut `.ninja_log`. `annette_r41.c` (09:49:06) und der Kopf `re15_annette_r41.h` (09:48:57) sind
  aelter als ihre Objekte.
- Keine Datei unter `re15_port/` (ausser `build/`) ist neuer als die exe. Der Baum ist sauber, also gleich cefd57402.
- cefd57402 (Commit 10:36:13) ist der letzte Commit auf `re15_port`/`release` (`git log -1 -- re15_port release`).
- **Die exe gehoert zu cefd57402 = Zweigkopf fuer `re15_port`.** Die exe der Unit-Tests (`tests/unit/test_r41_annette.exe`, 10:30:39)
  ist ebenfalls juenger als ihre Quelle (10:28:55).
- Die Assets kommen aus dem Arbeitsbaum. Der Selbsttest `RE15_ASSET_SELFTEST=1` mit der Kopie ergab 26/26, CD-Wurzel
  `.../r41_annette/re15_port/shared_assets/PSX`.

Hinweis zur Umgebung: Waehrend meiner Fensterlaeufe lief im selben Messordner `w2abn/` die Abnahme der Form-Spur mit eigener exe
(`w2abn/exe/`, `w2abn/laeufe/`). Meine Laeufe liegen getrennt unter `w2abn/an4/`. Fremde Dateien habe ich nicht angefasst.

## 0. Urteil

**NICHT BESTANDEN.** Es gibt einen blockierenden Mangel (M1), alles andere traegt.

- **Nachbesserung 5, Schussregel (c974552f2): in Ordnung.**
  - Die RE2-Regel "hp < 0 ist kein Ziel" habe ich im Leon-Programm (FUN_800410CC @0x80041280/88) und im Claire-Programm, in dem
    die Annette-Szene liegt (@0x8003df34/3c, Liste ab em[0] = Annette), selbst nachgelesen.
  - Der Nutzerentscheid steht woertlich im Dossier.
  - Kein Begleiter nimmt in meinen Laeufen Schaden. Im Kampf gehen die Schuesse durch die auf die Linie gelegte Leiche auf die G5
    (`kampf_linie`). Im echten Raum 10D0 gehen sie durch Marvin auf den Zombie (`integration_r41_begleiter_schusslinie`).
  - Bei Leon laeuft die Szene unveraendert und ohne Haenger. Bei Elza gibt es wie vorgesehen keine Szene, ihr Ablauf in 5091
    laeuft ohne Haenger.
  - A1 und H1 der Abnahme 3 sind richtig berichtigt, A2 hat jetzt einen Riegel.
  - Alle 1040 Unit-Tests und alle 10 Integrationstests sind gruen.
- **Koerpersegment (cefd57402): Wert richtig, Umfang falsch (M1).**
  - Den RE2-Wert r 350 / Halbhoehe 1350 habe ich selbst im RE2-Code (FUN_80034D0C @0x80034d74/@0x80034dac/@0x80034e5c) und im
    Claire-RAM (lauf3 e01..e30) bestaetigt.
  - Der Code gibt ihn aber jeder 0x4D, also auch der lebenden Annette in 6000/6030 und in 5090 vor der Szene. Dort ist sie
    RE1.5-Inhalt.
  - RE1.5 hat fuer genau diese 0x4D einen eigenen, vollstaendigen Kasten 450/1530: STAGE6 INIT @0x80101958 -> 0x80102784,
    STAGE5 INIT @0x8011d88c -> 0x801207d0. Die Commits und das Dossier nennen ihn nicht.
  - Das Dossier selbst sagt: "Ihr Verhalten in 6000/6030 bleibt das alte."
  - Gemessen: Im Fuehrerstand 6030 kommt Leon jetzt bis 802 an Annette heran, mit dem RE1.5-Kasten waeren es 900.
  - Das ist ein Rueckschritt gegen das Original in RE1.5-Raeumen, ohne Beleg und ohne Nutzerentscheid.
- **Bekannt und unveraendert:** Die Stimmen fehlen (6030 msg 27-30, 5090 msg 40-43, wie Abnahme 1-3).

## 1. Delta seit Abnahme 3 (37f565a3c, 2026-11-03 09:01:43)

| Commit | Zeit | Art | Inhalt |
|---|---|---|---|
| c974552f2 | 10:22:11 | Code + Tests | `re15_damage.c` re15_player_weapon_fire: Kandidatenschleife verwirft NPC 0x40..0x4D mit hp < 0; NB4-Typpruefung des Gewinners entfernt. `annette_r41.c` und `re15_annette_r41.h`: nur Kommentare (A1, H1; eine Leerzeichen-Aenderung im Prototyp). `test_r41_annette.c` test_schuss (b) umgestellt. Neu: `integration_r41_begleiter_schusslinie` (+ Eintrag in `probes/r41_annette.cmake`). Belege `AN_belege/n5/` |
| 52e461188 | 10:22:12 | Doku | Dossier N5 |
| cefd57402 | 10:36:13 | Code + Test | `re15_enemy_apply_hitbox`: case 0x4d r 350 / h 1350 (vorher Familie 450/1530). `test_r41_annette.c` test_leiche_radius. Beleg `n5/gegenprobe_radius450.txt` |

- Unter `release/` hat sich nichts geaendert.
- Der Zweig enthaelt r41/form bis 7d79bcfae (Merge b0685df4f); das hat sich gegenueber Abnahme 3 nicht geaendert.
- Gepruefte Commits: c974552f2, 52e461188, cefd57402.

## 2. Maengel und Hinweise

| # | Schwere | Was | Beleg |
|---|---|---|---|
| **M1** | **mittel, blockierend** (Rueckschritt gegen RE1.5, Inhalt ohne Beleg) | `case 0x4d: r = 350; h = 1350` gilt fuer jede 0x4D, auch fuer die lebende Annette in 6000/6030 und in 5090 vor der Szene. Dort gilt der RE1.5-Kasten 450/1530 (STAGE6 @0x80101958/@0x80101960 -> 0x80102784, STAGE5 @0x8011d88c/@0x8011d894 -> 0x801207d0). RE1.5 liest ihn fuer Wand-Klemme (@0x801018c4-d0), Bodenprobe (FUN_8001bd60 @0x8001bd94-a8) und Spieler-Schub (FUN_8002aec4 @0x8002af38-4c). Gemessen: `an6030_pult`, Leon haelt bei 802 statt 900. Begruendet ist der RE2-Wert nur fuer die RE2-Szene und die Leiche ("die Annette-Szene folgt RE2, also bekommt die Leiche den RE2-Wert", cefd57402). | Abschnitt 4, `re15_kasten_4d_abn4.txt`, `auswertung_an6030_pult.txt` |
| H1 | Hinweis (Beleg) | `info/re2leon/PSX.EXE` und `SLUS_007.48` sind dieselbe Datei (sha256 gleich). "In beiden Programmen gleich" ist deshalb keine zweite Bestaetigung. Die echte zweite Bestaetigung ist das Claire-Programm SLES_109.72 (@0x8003df34/3c), das ich nachgelesen habe. | Abschnitt 3 |
| H2 | Hinweis | Die Begleiter-Regel haengt nur an hp < 0. Ein NPC mit hp >= 0 waere wie in RE2 ein Ziel und faellt im Port dann in Zustand 3 (Sonde: hp 0 -> -5, st 3). In 556 vorhandenen state.log-Dateien traegt ein NPC hp >= 0 nur in Bild 1-5 nach dem Laden (vor dem INIT). Im gemessenen Lauf fiel der erste Schuss 9 Bilder nach dem Druck. Das Fenster ist also nicht erreichbar. | 5.5, `npc_hp_zensus.txt`, `npc_schuss4.txt` |
| H3 | Hinweis (nicht im Delta) | Vor der Szene wird die Form (0x30, hp 150) von der Wagentuer aus nicht getroffen (10 Schuesse in `an_vorszene`/`an_durch`, hp bleibt 150). Die Ursache habe ich nicht untersucht. Damit gibt es in meinen Laeufen keinen Moment, in dem eine lebende Annette vor einem treffbaren Gegner steht. | 5.2 |
| H4 | Hinweis | Elza: Einen Schuss durch einen Begleiter auf einen treffbaren Gegner konnte ich in einem echten Elza-Raum nicht vorfuehren. 10D0 laedt fuer Elza 10D1 ohne Begleiter; in 5031 ist der Affe in Zustand 4 / g 0x61 kein Ziel. Der Resolver hat keinen Figurenzweig. Sherry blieb bei 8 Schuessen unberuehrt. | 5.4 |
| H5 | Hinweis (Umgebung) | Die Sitzung hat keinen WASAPI-Endpunkt. In `z6030`/`s5090` fehlte deshalb die Audio-Schicht ("Tuerbank DOOR2A NICHT geladen"). Ab `kampf` lief der Ton per `RE15_AUDIO_CAP_SYNC`. Die Zustaende haengen daran nicht (`z6030` ist bitgleich mit dem Abnahme-3-Lauf mit Ton). | 5.1 |
| H6 | Hinweis | Nach dem Kampfstart laeuft die Form F1912..F2105 70-150 naeher an der Leiche vorbei (Leiche jetzt r 350). Der Kampfausgang ist gleich: dieselben 44 hp-Wechsel, G5 tot F3235, Leon hp 100. | 5.1 |
| M3 | bekannt | Stimmen fehlen (`[voice] ... keine Aufnahme`), unveraendert seit Abnahme 1 | debug.log |

## 3. RE2-Regel "Schuss geht durch Begleiter" (selbst disassembliert)

Beleg: `AN_belege/abn4/re2_disasm_abn4.txt` (Skill re15-psx-disasm, `re2_disasm.py`; Claire per `werkzeug/cdis4.py`).

**Leon, `info/re2leon/PSX.EXE`, FUN_800410CC (Schusswaffen):**

| Adresse | Befehl | Bedeutung |
|---|---|---|
| 0x80041208-0c | `lui fp,0x800d` / `addiu fp,fp,-484` | Kandidatenliste ab 0x800cfe1c |
| 0x80041254 | `lw s2,0(fp)` | Kandidat |
| 0x8004125c-68 | `lw v0,0(s2)` / `andi v0,v0,0x1` / `beq v0,zero,0x80041300` | Tor 1: aktiv |
| 0x80041270-78 | `lbu v0,467(s2)` / `bne v0,zero,0x80041300` | Tor 2: Trefferpause +0x1D3 |
| **0x80041280** | **`lh v0,342(s2)`** | **hp (+0x156)** |
| **0x80041288** | **`bltz v0,0x80041300`** | **Tor 3: hp < 0 -> kein Ziel** |
| 0x80041290-9c | `lhu v0,270(s2)` / `andi v0,v0,0xc000` / `bne` | Tor 4: +0x10E & 0xC000 |
| 0x800412f0 | `jal 0x80050858` | Sichtlinie, erst nach den Toren |
| 0x80041300-04 | `j 0x800417d0` / `addiu fp,fp,4` | naechster Kandidat |
| 0x800417d0-dc | `lw v0,-7372(v0)` (= [0x800ce334]) / `bne fp,v0,0x80041254` | Schleifenende |

Eine Typabfrage gibt es in der Schleife nicht. Messer, Bolzen und Explosion (FUN_800470C0) haben dasselbe Tor:
`lh v0,342(s0)` @0x80047148 / `bltz v0,0x8004740c` @0x80047150. Der Em-Setzer setzt hp -1: `addiu v0,zero,-1` / `sh v0,342(s0)`
@0x8005d7b4-b8.

**Zweites Programm.** `info/re2leon/PSX.EXE` und `info/re2leon/SLUS_007.48` sind dieselbe Datei (sha256 e576348f64d015ec..). Die
Angabe "Leon PSX.EXE und SLUS_007.48 gleich" in Dossier und Code ist also keine zweite Bestaetigung (Hinweis H1). Die zweite
Bestaetigung liefert das Claire-Programm SLES_109.72 (PAL, Scratch der Autor-Sitzung, unversioniert), in dem die Annette-Szene
liegt. Wortmuster-Suche (`werkzeug/claire_hp_tor.py`) und Disassembly:

| Claire-Adresse | Befehl |
|---|---|
| 0x8003deb0-b4 | `lui t0,0x800c` / `addiu t0,t0,31152` = Liste ab **0x800c79b0** |
| 0x8003df08 | `lw s2,0(t0)` |
| 0x8003df24-2c | `lbu v0,467(s2)` / `bne v0,zero,0x8003dfb8` |
| **0x8003df34 / 0x8003df3c** | **`lh v0,342(s2)` / `bltz v0,0x8003dfb8`** |

0x800c79b0 ist der Listenplatz em[0]. In den Claire-Savestates der Szene ROOM6141 (lauf3 e01..e30) steht dort Annettes Entity
0x80125d4c (Id 0x49) mit hp -1, lebend wie liegend (eigene Lesung, `werkzeug/claire_annette_ram.py`, `claire_annette_ram.txt`).
Die RE2-Annette steht also in der Schusskandidatenliste und faellt wegen hp -1 aus jeder Wahl. Die Regel ist belegt.

**Nutzerentscheid, woertlich im Dossier N5.2:** "Mache die RE 2 Variante, das der Schuss durch begleiter durch geht." Derselbe
Wortlaut steht im Commit c974552f2, in `re15_damage.c` und im Orchestrator-Gedaechtnis (`reai-v2-runde41-laufend.md`, Eintrag
2026-11-03). Den Nutzer selbst habe ich nicht gehoert, die Quellen stimmen ueberein.

**Port (`re15_damage.c`, re15_player_weapon_fire):** In der Kandidatenschleife steht
`if (e->type >= 0x40u && e->type <= 0x4Du && e->hp < 0) continue;`. Die NB4-Typpruefung des Gewinners ist entfernt. Die
Zuordnung "RE2-hp-Tor fuer die NPC-Familie 0x40..0x4D, RE1.5-Semantik fuer Gegner < 0x40" entspricht re15_resolve_attack (Runde 34
B1, Zeile 3670). Die Granatenwege haben das Tor schon: re15_re2_gl_apply Zeile 4488, re15_re2_gl_kandidat Zeile 4631.

**Plc_motion (A1 aus Abnahme 3), nachgelesen:** Der Dispatcher liest @0x80053f70-8c die Tabelle 0x800a74c8. Eintrag [0x3F]
@0x800a75c4 zeigt auf 0x80056b8c. Dort steht `sb zero,333(v1)` @0x80056ba0, der Clip folgt mit `sb a1,332(v1)` @0x80056bb8. An
0x80052ef0 steht `subu v0,v0,v1`. Die Berichtigung stimmt.

## 4. Koerpersegment r 350 / Halbhoehe 1350

### 4.1 RE2-Wert: belegt

- Schub FUN_80034D0C (`info/re2leon/PSX.EXE`, selbst disassembliert, `re2_disasm_abn4.txt` Abschnitt 6):
  - Segmentzahl `lw v0,488(s6)` (+0x1E8) @0x80034d74, Segmente ab `addiu s5,s6,132` (+0x84) @0x80034d78
  - Radius `lhu s7,22(s5)` (+0x16) @0x80034dac
  - Halbhoehe `lhu v0,26(s5)` (+0x1A) @0x80034e5c, Hoehen-Tor @0x80034e60-78
- Claire-RAM ROOM6141 lauf3, eigene Lesung mit eigenem Leser (`werkzeug/claire_annette_ram.py`, Ergebnis `claire_annette_ram.txt`):

  | Bilder | Annette @0x80125d4c (Id 0x49) | Segment 0 |
  |---|---|---|
  | e01..e22 (lebend, Lage y 0) | hp -1, +0x1E8 = 1 | Mitte = Lage + (0,-1350,0), +0x14 = -1350, r 350, Halbhoehe 1350 |
  | e23..e30 (liegend, Lage (-12300,900,-15300)) | hp -1, +0x1E8 = 1 | Mitte (-12300,-450,-15300), r 350, Halbhoehe 1350 |

  Claire selbst traegt dasselbe Segment (r 350, Halbhoehe 1350).
- Port: `re15_enemy_apply_hitbox` case 0x4d: r 350, h 1350, Versatz -1350. `unit_r41_annette` test_leiche_radius in meinem Lauf:
  "Kasten: r 350/350 h 1350 Versatz -1350", Schub bei 850: keiner, bei 750: auf 801.

Der RE2-Wert und seine Uebertragung stimmen.

### 4.2 Der Umfang: RE1.5 hat fuer 0x4D einen eigenen, vollstaendigen Kasten

Der Commit cefd57402 begruendet: "die Annette-Szene folgt RE2, also bekommt die Leiche den RE2-Wert". Der Code aendert aber den
Kasten jeder 0x4D in jedem Raum, also auch die lebende Annette in ROOM6000 und ROOM6030 und in 5090 vor der Szene. Dort ist sie
RE1.5-Inhalt. Das Dossier sagt das selbst (8.3): "Ihr Verhalten in 6000/6030 bleibt das alte (gleiche Clips)."

RE1.5 gibt der 0x4D einen eigenen Kasten. Selbst disassembliert (`AN_belege/abn4/re15_kasten_4d_abn4.txt`, Overlay ohne Kopf
@0x80100000):

| Stelle | STAGE6.BIN (6000/6030) | STAGE5.BIN (5090) |
|---|---|---|
| Registrierung Typ 0x4D (0x80072bac + 0x4D*4 = 0x80072ce0) | `addiu v0,v0,6048` / `sw v0,11488(at)` @0x80101df4/@0x80101dfc -> Wurzel 0x801017a0 | `addiu v0,v0,-10540` / `sw v0,11488(at)` @0x8011ddb0/@0x8011ddb8 -> Wurzel 0x8011d6d4 |
| Zustandstabelle der Wurzel | `addiu at,at,10132` @0x80101878 -> 0x80102794, [0] = INIT 0x80101918 | `addiu at,at,2016` @0x8011d7ac -> 0x801207e0, [0] = INIT 0x8011d84c |
| INIT: +0x78 = Kasten | `lw v0,10128(v0)` @0x80101958 (= [0x80102790] = 0x80102784) / `sw v0,120(v1)` @0x80101960 | `lw v0,2012(v0)` @0x8011d88c (= [0x801207dc] = 0x801207d0) / `sw v0,120(v1)` @0x8011d894 |
| Kasten-Bytes | @0x80102784 `00 00 06 fa 00 00 c2 01 fa 05 c2 01` | @0x801207d0 `00 00 06 fa 00 00 c2 01 fa 05 c2 01` |
| = {ofs_x, ofs_y, ofs_z, r_min, h, r_max} | {0,-1530,0,450,1530,450} | {0,-1530,0,450,1530,450} |

Diesen Kasten liest RE1.5 an drei Stellen:
- Wand-Klemme im Wurzel-Schwanz der 0x4D: `lw v0,120(a0)` / `lhu a1,6(v0)` / `jal 0x8003b0a4` @0x801018c4-d0 (STAGE6).
- Bodenprobe FUN_8001bd60: `lw v1,120(v0)` / `lhu a1,6(v1)` / `jal 0x8003b7f0` mit `subu a1,zero,a1` @0x8001bd94-a8 (EXE),
  aufgerufen aus der Wurzel @0x80101844.
- Spieler-Schub FUN_8002aec4, aufgerufen @0x801018a8: `lw s6,120(s3)` / `lw s5,120(s2)` @0x8002af38-3c, Radien
  `lhu t4,6(s6)` / `lhu t4,6(s5)` @0x8002af40/@0x8002af4c (EXE), also Kasten der 0x4D gegen den Spieler-Kasten 450.

Der alte Port-Wert 450/1530 war fuer 0x4D also byte-genau richtig, auch wenn der Kommentar nur den STAGE1-Kasten @0x80121658
nannte. RE1.5 hat das System vollstaendig. Nach der Projektregel (Beta -> Retail, "Original oder nicht") bleibt RE1.5 dort
massgeblich. Der RE2-Wert gehoert zur RE2-Szene und zur Leiche (5090 ab dem Szenenstart). In 6000/6030 und in 5090 vor der Szene ist
350/1350 ohne gueltigen Beleg.

### 4.3 Gemessene Wirkung in RE1.5-Raeumen

- `an6030_pult` (Abschnitt 5.3): Annette steht vor dem Tor am Pult in 6030 (sub07 @0x010F8). Leon geht auf sie zu und bleibt bei
  **802** stehen (Minimum 801 bei F102). Das ist 450 + 350. Mit ihrem RE1.5-Kasten waeren es 900.
- `z6030` (Zugstart, Abgang zur Tuer): bitgleich mit Abnahme 3. Ihr Weg beruehrt weder eine Wand noch Leon so, dass 350 gegen 450
  zaehlt.
- `an6000` (sie geht in den Zug): Der Ablauf ist vollstaendig, der kleinste Abstand ist 1030. Ein Vergleichslauf mit 450 fehlt mir,
  weil ich nicht bauen durfte.

Die Abweichung ist also klein, aber im Spiel erreichbar und messbar. Sie aendert RE1.5-Verhalten, fuer das es keinen Beleg und keinen
Nutzerentscheid gibt. Deshalb ist M1 blockierend.

## 5. Spiel: eigene Laeufe mit der exe-Kopie

Alle Laeufe im echten Fenster (Renderer des Nutzers, ohne AUTOSHOT und ohne SOFTWARE_RENDER), nacheinander, Bilder per
`RE15_FRAMEDUMP`, Ende per `RE15_EXIT_AT` oder `timeout` auf die eigene PID. Werkzeug: `AN_belege/abn4/werkzeug/` (`lauf4.sh`,
`alle4.sh`, `alle4b.sh`, `alle4c.sh`). Je Lauf eine Auswertung `AN_belege/abn4/auswertung_<lauf>.txt` (env, Ende, Vergleich mit dem
gleichnamigen Abnahme-3-Lauf, Abstand Spieler-Annette, Schuesse und Treffer, Szenenschritte).

Die Sitzung hat kein Audiogeraet (`SDL_OpenAudioDevice failed: WASAPI can't find requested audio endpoint`). Ab `kampf` lief deshalb
`RE15_AUDIO_CAP_SYNC` mit wie in Abnahme 3. In `z6030` und `s5090` fehlte die Audio-Schicht (daher dort "Tuerbank DOOR2A NICHT
geladen"). Die Spielzustaende haengen daran nicht: `z6030` ist trotzdem Zeile fuer Zeile gleich dem Abnahme-3-Lauf mit Ton (H5).

### 5.1 Wiederholung der Abnahme-3-Laeufe (exe b646c72a6 = NB4) mit der exe cefd57402 = NB5

Gleiche Umgebung und Eingaben wie `abnahme3/werkzeug/abn3_alle.sh` (Kette ab dem Torstand in 6000). Verglichen wurde das state.log
Zeile fuer Zeile (`werkzeug/logvergleich.py`) gegen `re15_port/build/abn3/<lauf>/state.log`.

| Lauf | Ergebnis | Bewertung |
|---|---|---|
| `z6030` (6000 -> 6030, Zugstart, Abgang durch die Tuer, F1..F1700) | **1968 von 1968 Zeilen gleich**, auch Annettes Weg zur Tuer und Leons Seitschritt. Kleinster Abstand Leon-Annette 1226 (F1502). Plc_dest (-3000,-23700), Se_on Bank 14 Satz 0 und 1 wie Abnahme 3 | unveraendert |
| `s5090` (Kette bis 5090, Szene, F1..F1965) | 4275 Zeilen gleich, ab F1912 54 Zeilen anders. Annettes Zeilen sind alle gleich. Die Szene laeuft in derselben Reihenfolge auf denselben Bildern: F1168 Szenenstart, F1350 Sturz, F1365 gelandet, F1598 Hieb, F1640 Treffer, F1674 Leiche, F1776 Kampfstart. Ab F1912 laeuft die Form 70-150 naeher an der Leiche vorbei ((3858,-23659) statt (3784,-23726)), weil die Leiche jetzt Radius 350 hat. | erwartete Folge des RE2-Segments der Leiche (H6) |
| `kampf` (Pistole, 144 Schuesse, F1..F5400) | 7570 Zeilen gleich, 194 anders, nur F1912..F2105 (Weg der Form, s.o.). Spielerlage nie anders. 44 hp-Wechsel wie in Abnahme 3: Form-Rueckstoss F1915, G5 von F2377 bis F3235 im 22er-Takt (600 -> -1), G5 tot F3235. Leon in allen 7764 Zeilen hp 100. Die Leiche liegt 3727 Bilder unveraendert bei (4856,-22748) r1170, Clip 24/0, hp -1 | Kampfausgang unveraendert |
| `leiche_west` (Wiedereintritt, Leon laeuft an der Leiche vorbei) | Abnahme 3: Leon wird bei F390..F404 auf 900 geschoben (z -23550 -> -23650). Jetzt: kleinster Abstand 802 bei F398, kein Schub, Leon bleibt auf z -23550. 708 Folgezeilen weichen deshalb um bis zu 100 ab | gewollte Folge (Abnahme 3 H2, Dossier N5.3) |

Bildleisten: `z6030_abgang_abn4.jpg` (F1460..F1600), `s5090_szene_abn4.jpg` (20 Felder F1165..F1960), `kampf_abn4.jpg` (F1750..F5400),
`leiche_west_abn4.jpg`.

### 5.2 Schuesse neben und durch Annette in 5090 (Leon)

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `an_vorszene` | Debug-Sprung 5090 (3,89)(9,133), Pistole. Leon tritt in Wagen C, schiesst 3 s von der Wagentuer (20300,-23550) nach Westen, geht dann zum Ausloeser | 4 Schuesse (F202..F268). Annette (600,-25300) hp -1 st 4 unveraendert. Die Form (0x30, (-14700,-23350), hp 150) wird nicht getroffen (H3). Danach laeuft die Szene vollstaendig: F380 Szenenstart ... F988 Kampfstart, alle 12 Schritte in Reihenfolge, Leiche gelegt. Kein Haenger |
| `an_durch` | wie oben, Annette per `RE15_GEGNER_POS` ab F120 genau auf Leons Schusslinie (600,-23550), 19700 vor ihm, die Form 35000 dahinter | 6 Schuesse (F202..F312). **Annette hp -1, st 4, Lage unveraendert.** Vor NB4 haette der erste Schuss sie getoetet (Abnahme 3: hp -1 -> -6, st 3), mit NB4 haette sie jeden Schuss geschluckt. Die Form bleibt wie in `an_vorszene` ungetroffen (H3) |
| `kampf_linie` | Lauf `kampf`, aber die Leiche ab F1790 per `RE15_GEGNER_POS` genau auf die Linie Leon (13775,-23550) -> G5 gelegt: (9000,-23550), 4775 vor Leon | 68 Schuesse bis F3400. **Die hp-Wechsel sind dieselben 44 wie in `kampf`** (G5 F2377 600 -> 585 ... F3235 15 -> -1). Bei F2377: Leon (13775,-23550), Leiche (9000,-23550), G5 (-2006,-23400). Die Leiche bleibt hp -1 st 0. **Die Schuesse gehen durch Annette auf die G5.** |

Bildleisten: `an_vorszene_abn4.jpg`, `an_durch_abn4.jpg` (Kamera 14 an der Wagentuer, Annette und Form ausserhalb des Bildes).

In meinen Laeufen stand in 5090 nie eine lebende Annette vor einem treffbaren Gegner: Vor der Szene wurde die Form von der Wagentuer
aus nicht getroffen (H3), nach der Szene ist Annette tot. Fuer den lebenden Begleiter vor einem Gegner stehen deshalb
`integration_r41_begleiter_schusslinie` (ROOM10D0, Marvin vor dem Zombie, Abschnitt 6) und die Sonde (5.5).

### 5.3 ROOM6000 und ROOM6030: lebende Annette (RE1.5-Inhalt) mit dem neuen Kasten

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `an6000` | Debug-Sprung 6000, (3,48)(3,49)(3,100), Leon in Aot Slot 4 (4550,-8200) | sub02 laeuft: msg 4 "Let's see if we can get this thing moving.", Annette geht (5593,-7854) -> (2583,-7854) -> (2073,-9148), Cut 4, versteckt bei (-10507,8646) ab F300, Leon frei. Kleinster Abstand 1030, kein Schub. Kein Haenger |
| `an6030_pult` | Debug-Sprung 6030 vor dem Tor ((4,23)=1, (3,89)=0: sub07 @0x010F8, sie steht am Pult (-1790,-22272)); Leon 810 westlich, geht 2 s auf sie zu | **Leon bleibt bei 802 (F102) stehen, Minimum 801.** Der Koerperschub haelt ihn bei 450 + 350 = 800. Mit dem RE1.5-Kasten der 0x4D (450, Abschnitt 4.2) laege die Grenze bei 900; Leon stand schon beim Start (810) innerhalb davon |

Bildleisten: `an6000_abn4.jpg`, `an6030_pult_abn4.jpg` (Leon dicht hinter Annette am Pult).

### 5.4 Elza

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| `integration_r41_annette` elza | 5091 (3,89)(9,133) | keine 0x4D, keine Szene (Abschnitt 6) |
| `elza5091` | 5091 wie oben, Pistole, 3 s Schuesse von der Wagentuer, dann zum Ausloeser | Spieler-Familie PL04 (Elza). Keine 0x4D, keine Szene, keine Annette-Zeile im state.log. 4 Schuesse, die Form wird nicht getroffen (wie bei Leon, H3). Danach Kampfstart wie im Riegel; ohne Eingabe stirbt Elza bei F1150 (Form-Angriffe), Todesablauf ohne Haenger |
| `elza5031` | echter Elza-Raum 5031, sub00 @0x010B2 Ck(3,47)=0: Sherry 0x4B (14100,17050) und der kleine Affe 0x29 (18200,17050) auf einer Linie; Elza (11000,17050) Blick +x, 6 s Zielen + Feuer | 8 Schuesse. **Sherry hp -1, st 4 unveraendert.** Der Affe (st 4, g 0x61, hp 87) ist in diesem Zustand kein Ziel und wird nicht getroffen (H4) |
| `elza_10d0` | Messaufbau wie der Riegel in 10D0, mit Elza | **verworfen:** Der Debug-Sprung laedt fuer Elza ROOM10D1 (kein Marvin, nur ein Zombie). Elza wurde gepackt und starb, das Spiel ging zurueck auf den Titel, `RE15_EXIT_AT` #10D0 griff nie -> `timeout` (exit 124). Kein Befund gegen die Spur |

Der Waffen-Resolver `re15_player_weapon_fire` hat keinen Zweig nach der Figur (Zeilen 2018-2900 ohne Elza- oder Charakter-Abfrage).
Die Regel gilt fuer Elza also unveraendert. Dass ein Schuss bei Elza durch einen Begleiter auf einen Gegner geht, konnte ich in einem
echten Elza-Raum nicht vorfuehren (H4). Die Sonde (5.5) ist figurunabhaengig.

### 5.5 Bibliotheks-Sonde `npc_schuss4.c` (gegen `libre15_engine.a` des Baums, 10:29:52 = cefd57402)

Aufbau wie die Sonde `npc_schuss.c` der Abnahme 3: Leon (0,0) Blick -x, NPC bei x -3000, Zombie 0x10 (hp 100) bei x -6000, 5 Schuesse
je Fall. Gebaut nur in meinem Scratch-Ordner, nichts im Arbeitsbaum. Ergebnis: `AN_belege/abn4/npc_schuss4.txt`.

| Fall | Waffen 3, 7, 8, 12, 13, 19 | Ergebnis |
|---|---|---|
| ohne NPC (Kontrolle) | alle | 5 Zombie-Treffer je Waffe |
| NPC 0x40/0x42/0x45/0x47/0x49/0x4B/0x4D mit hp -1 davor | alle | **0 NPC-Treffer, 5 Zombie-Treffer**, NPC hp -1 und st 0 unveraendert (vorher NB4: 0/0, 5 ins Leere) |
| NPC 0x40 mit hp 0 davor | Pistole | 1 NPC-Treffer (hp 0 -> -5, **st 3**), dann 4 Zombie-Treffer |
| Zombie 0x10 mit hp -1 davor | Pistole | 1 Treffer auf ihn, dann 4 auf den hinteren (RE1.5-Semantik fuer Typen < 0x40, unveraendert) |
| NPC 0x4D quer 1300 | Pistole | 0 NPC-Treffer, 5 Zombie-Treffer |

Die Regel haengt also allein an hp < 0. Ein NPC mit hp >= 0 ist wie in RE2 ein Ziel. Im Port faellt es dann aber in Zustand 3, den
schweren Fehler von vor NB4. Wann traegt ein NPC im Spiel hp >= 0? `werkzeug/npc_hp_zensus.py` ueber 556 vorhandene state.log-Dateien
(dieser Baum 170, r41_integration und Hauptbaum 386; alle sieben NPC-Typen kommen vor): Es gibt 205 Spannen mit hp >= 0. Alle
liegen in Bild 1 bis 5 nach dem Laden des Raums, also vor dem NPC-INIT (`+0x9a = -1` @0x8011c744, STAGE6 @0x80101980-84).
RE2 setzt hp -1 schon im Em-Setzer (@0x8005d7b4-b8). Im eigenen Lauf faellt der erste Schuss 9 Bilder nach dem Druck auf Zielen und Feuer
(eigener Lauf `integration_r41_begleiter_schusslinie` davor: Zielen + Feuer ab F280, erster Magazin-Abfall F289). Das Fenster F1..F5
ist also nicht erreichbar. Das ist ein Hinweis (H2), kein Mangel.

## 6. Tests (eigene Laeufe, Git-Bash mit `PATH=/c/msys64/mingw64/bin:$PATH`, `SDL_ASSERT=always_ignore`, `--test-dir <Baum>/re15_port/build`)

| Test | Ergebnis |
|---|---|
| unit_r41_annette, unit_r41_annette_se_bank, _se_toene, _se_lagen | **gruen**. test_schuss: (a) Leiche hp -1 st 0, G5 330; (b) lebendes NPC hp -1 st 0, Rueckgabe 3, G5 330. test_leiche_radius: Kasten 350/350/1350/-1350, Schub bei 850 keiner, bei 750 auf 801 |
| Auswahl Schuss/NPC (zusammen mit den vier Annette-Tests 16/16): unit_damage, unit_npc_ai, unit_re2_weapon_rows, unit_r16_aufstehen_schuss, unit_r17_waffen_loop_pin, unit_r35_affen_npcband, unit_r36_elza_schuss, unit_r36_schuss, unit_r36_schuss_kraehe, unit_r38_elzatoene_schuss1021, unit_r39_gegner_affen_schuss, unit_r41_kollision_npc | **12/12 gruen** |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|titelversion\|android\|paket\|r37_masken36_ausschluss`, `-j 2` | **1040/1040 gruen** (449 s) |
| integration_r41_begleiter_schusslinie (neu, NB5) | **gruen** (53 s). davor: 8 Schuesse, Zombie 98 -> 34 (Treffer F289/F311/F333/F355), Marvin st 4 hp -1 durchgehend |
| integration_r41_annette (5 Laeufe) | **gruen** (254 s). leon_tor: Schritte F280 Szenenstart ... F888 Kampfstart in Reihenfolge, Ende Leiche (4856,-22748) r1170 Clip 24/0 hp -1; elza/leon_ohne_zug: keine 0x4D |
| integration_r41_birkin_pistole | **gruen** (243 s) |
| integration_r35_zgirl_5090 | **gruen** (148 s) |
| integration_r36_story_leon / _elza (darin 6000/6030 mit Annette, msg 0-3 und 11/12) | **gruen** (374 s / 216 s) |
| integration_r36_schuss, integration_r36_elza_schuss | **gruen** (149 s / 49 s) |
| integration_r41_kollision_npc3070 / 3001 | **gruen** (83 s / 83 s) |

Alle Integrationstests liefen nacheinander (`-j 1`). Die Unit-Suite lief parallel dazu, beide waren gruen; kein roter Fenster-Riegel war
nachzufahren. Die ausgeschlossenen df-haengenden Skripttests sind dieselben wie in Abnahme 3.

## 7. Was fuer BESTANDEN fehlt (Vorschlag Nachbesserung 6)

1. In `re15_enemy_apply_hitbox` die 0x4D wieder in die NPC-Familie nehmen (r 450 / h 1530). Als Beleg gehoert der eigene 0x4D-Kasten
   dazu: STAGE6 `lw v0,10128(v0)` @0x80101958 / `sw v0,120(v1)` @0x80101960 -> 0x80102784, STAGE5 @0x8011d88c / @0x8011d894 ->
   0x801207d0, Bytes `00 00 06 fa 00 00 c2 01 fa 05 c2 01`.
2. Das RE2-Segment r 350 / h 1350 / Versatz -1350 nur dort setzen, wo die RE2-Szene Annette uebernimmt. Das ist in `annette_r41.c`
   ab dem Szenenstart in 5090, spaetestens beim Legen der Leiche, auch beim Wiedereintritt. Die Zitate sind FUN_80034D0C und
   Claire-RAM lauf3.
3. Zwei Riegel:
   - `test_leiche_radius` bleibt (Leiche 350).
   - Neu ist ein Riegel fuer die lebende 0x4D in 6030/6000 mit 450/1530: Abstand ueber 850 und Schub bis 900, Gegenprobe 350 rot.
     Ein Fensterlauf wie `an6030_pult` muss danach 900 statt 802 zeigen.
4. Will der Nutzer dagegen, dass Annette ueberall den RE2-Koerper hat (sie traegt ja auch das RE2-Netz), dann bleibt der Code.
   Dann muss aber der Nutzerentscheid woertlich ins Dossier, und im Code steht er als PORT-WAHL mit dem verdraengten RE1.5-Kasten.
   Das ist die einzige Frage in diesem Bericht.

Alles andere aus dem Delta kann so bleiben.

## 8. Belege

`analysis/befunde_runde41/AN_belege/abn4/`:

| Datei | Inhalt |
|---|---|
| `delta_abn4.txt` | Commits, Diffstat, Bauzeiten, sha256 der exe, "keine Quelle neuer als die exe", git status |
| `re2_disasm_abn4.txt` | RE2 Leon FUN_800410CC (Liste, vier Tore, Schleifenende), FUN_800470C0-Tor, Em-Setzer hp -1, Claire SLES_109.72 Schleife + hp-Tor, Plc_motion (A1), FUN_80034D0C Segmentfelder |
| `claire_annette_ram.txt`, `claire_hp_tor.txt` | eigene Lesung Claire-RAM lauf3 e01..e30 (Annette/Claire: hp, +0x1E8, Segment); Wortmuster-Suche hp-Tor in Leon- und Claire-Programm |
| `re15_kasten_4d_abn4.txt` | RE1.5 STAGE6/STAGE5: Registrierung Typ 0x4D, Wurzel, INIT, Kasten-Bytes; Wand-Klemme der 0x4D-Wurzel; FUN_8001bd60-Radius |
| `typ4d_zensus.txt` | 0x4D steht nur in 5090 (@0x0122A), 6000 (@0x00F44/@0x00F68), 6030 (@0x010D4/@0x010F8); NPC-Typen aller Skripte |
| `npc_schuss4.txt` | Sonde gegen `libre15_engine.a` (Quelle `werkzeug/npc_schuss4.c`) |
| `npc_hp_zensus.txt` | NPC-hp >= 0 nur in Bild 1-5 nach dem Laden (556 state.log) |
| `auswertung_<lauf>.txt` | je Fensterlauf: env, Ende, Vergleich mit Abnahme 3, Abstand, Schuesse/Treffer, Szenenschritte |
| `z6030_abgang_abn4.jpg`, `s5090_szene_abn4.jpg`, `kampf_abn4.jpg`, `leiche_west_abn4.jpg`, `an_vorszene_abn4.jpg`, `an_durch_abn4.jpg`, `an6000_abn4.jpg`, `an6030_pult_abn4.jpg`, `elza5091_abn4.jpg`, `elza5031_abn4.jpg` | Bildleisten aus den RE15_FRAMEDUMP-Serien |
| `ctest_abn4.txt` | Ergebniszeilen aller ctest-Laeufe |
| `werkzeug/` | `lauf4.sh`, `alle4.sh`, `alle4b.sh`, `alle4c.sh`, `auswerten4.sh`, `logvergleich.py`, `schuss4.py`, `szene4.py`, `abstand4.py`, `blatt4.py`, `claire_annette_ram.py`, `claire_hp_tor.py`, `cdis4.py`, `typ4d_zensus.py`, `npc_hp_zensus.py`, `npc_schuss4.c` |

Nicht versioniert:
- Rohdaten (state.log, debug.log, ppm, ton.raw) unter `scratchpad/w2abn/an4/laeufe/`.
- Die Claire-Savestates und SLES_109.72 im Scratch der Autor-Sitzung (Pfade in den Skripten).

Ich habe keinen Code und keine Datei unter `pri/`, `synchro/`, `Version_80%/` oder `info/` geaendert und im Arbeitsbaum nichts gebaut
(die Sonde `npc_schuss4.c` habe ich nur im Scratch-Ordner gegen die vorhandene Bibliothek uebersetzt). Die ctest-Laeufe
haben wie vorgesehen nur ihre Arbeitsordner unter `re15_port/build/` angelegt. Alle exe-Kopien `re15_pc_an4_*` liefen unter `timeout`
auf die eigene PID und sind geloescht.
