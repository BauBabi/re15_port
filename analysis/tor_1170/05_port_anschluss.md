# 05 — Port-Anschluss: wie das Tor von ROOM1170 und später die Türsequenz in den Port kommen

Stand 2026-09-28. Reine Untersuchung: **keine Zeile Port-Code geändert, nichts gebaut, kein git-Befehl
mit Schreibwirkung.** Geschrieben wurden nur diese Datei und Dateien unter `build/tor_1170/`
(`port_anschluss.json` sowie die Messwerkzeuge `_pa_*.py`).

Alle Zeilennummern unten sind **gemessen, nicht abgetippt**: `build/tor_1170/_pa_sammeln.py` sucht jede
Stelle per Muster und schreibt die Zeile nach `port_anschluss.json` (`zeilenanker_nicht_gefunden: []`).
Alle `@0x…`-Adressen habe ich selbst gelesen mit
`python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis <addr> <n>` gegen `info/Re1.5/PSX.EXE`.

Nachmessen (Repo-Wurzel, Git-Bash): `python build/tor_1170/_pa_sammeln.py`

---

## 0. Kurzfassung

| Frage | Antwort | Status |
|---|---|---|
| Wo läuft die Türsequenz im Original? | Im Raumlader `FUN_8001d600`, Tür-Zweig: DO2 laden `@0x8001d838`, Tür-Task starten `@0x8001d840-48`, **vor** dem Setzen des Spawns `@0x8001d87c`; der Lader wartet am Ende `@0x8001dab8-d4` auf das Task-Ende. | belegt |
| Wo wäre das im Port? | Zwischen `aot_fire_door` (`aot_common.c:526`) und dem Verbrauch: Kreuz-Raum `main.c:7054`, Selbst-Tür `game_step_common.c:2038`. Die Einblendung (`re15_room_transition_present`) bleibt, wie sie ist, **danach**. | Stellen belegt, Einfügung ist Entwurf |
| Kann der Port ein Tür-MD1 + TIM zeichnen? | Ja, das Format ist wortgleich mit den Raum-Props (UV-Wörter `0x7800`/`0x80`, TIM 128x256 8bpp). Es fehlt aber eine **aufrufbare** Zeichenfunktion: der starre MD1-Zeichner steht nur inline in der Prop-Schleife `main.c:9608-9741`. | belegt |
| Welche Werkzeuge der Sicherung sind wiederverwendbar? | `tim_bauen`, `carr`, `basis`, `projiziere`, `rendern` direkt; `md1_bauen`, `rueckprojekt`, `textur_bauen`, `silhouette_original` nur nach Verallgemeinerung. Zwei Dinge dürfen **nicht** übernommen werden: `HELLIGKEIT = 2.87` und die Schwarz-Behandlung. | belegt |
| Wählen die RE1.5-Daten ein Türmodell? | Nein. 649 von 653 `Door_aot_set` tragen `pc[26] = pc[27] = 0`; die 4 übrigen sind die bekannten Scan-Artefakte in ROOM4030/4031. Die Zuordnung Tür → Modell muss portseitig kommen. | belegt |
| Datei im Asset-Baum oder eingebacken? | Für den PC-Stand eingebacken wie die Sicherung; der Generator soll zusätzlich den DO2-Container schreiben (für den späteren PSX-Port). Begründung in Abschnitt 4. | Empfehlung |

---

## 1. Der heutige Weg eines Raumwechsels — und wo die Türsequenz hingehört

### 1.1 Original, selbst disassembliert

**Auslöser** — sce-2-Handler, Tabelle `0x8007469c[2] = 0x800430bc` (`table 0x8007469c 4`):

```
800430bc: ori  v0,zero,0x1
800430c4: sw   a0,-13912(at)      ; DAT_800ac9a8 = Zeiger auf die Tür-Nutzlast (= Door_aot_set pc+14)
800430d4: sb   v0,21337(at)       ; DAT_800b5359 = 1  (Transitions-FSM, State 1)
800430d8: lw   v0,0(a0)           ; g_pauseflags
800430dc: lui  v1,0xff00
800430e0: or   v0,v0,v1
800430e4: sw   v0,0(a0)           ; g_pauseflags |= 0xff000000
```

**FSM State 1**, Sprungtabelle `0x8001069c[0] = 0x8001c9c8`, Tür-Zweig:

```
8001c9e0: jal  0x80061fc0 ; a0 = -1          (Ton aus)
8001ca20-2c: aca38 |= 0x10000
8001ca30: jal  0x80021634 ; (1,0)
8001ca44: sw   zero,g_pauseflags
8001ca48-50: aca38 |= 0x4000
8001ca54: jal  0x8001d600                     (Raumlader, synchron)
8001ca5c: j    0x8001cbbc                     (State-3-Rumpf = Einblendung)
```

**Raumlader `FUN_8001d600`, Tür-Zweig** — hier sitzt die Türsequenz:

```
8001d604: lw   v0,-13912(v0)      ; DAT_800ac9a8
8001d618: bne  v0,zero,0x8001d82c ; Tür-Record vorhanden -> Tür-Zweig
8001d82c: ori  a0,zero,0x2
8001d830: jal  0x80021634         ; (2,0) = Schwarz
8001d838: jal  0x800171f4         ; DO2 laden
8001d840: lui  a1,0x8001
8001d844: addiu a1,a1,24968       ; a1 = 0x80016188 = Treiber des Tür-Tasks
8001d848: jal  0x80029a98         ; a0 = 1 -> Task 1 starten
8001d850-68: Schleife  jal 0x80029ac8(1)  solange  aca38 & 0x10000
8001d870: lw   a0,DAT_800ac9a8
8001d87c: lh   v0,0(a0)           ; ERST JETZT der Spawn (X), danach Y/Z/Yaw/Cut/Raum
...
8001dab8: lui  s0,0x200
8001dabc-d4: Schleife  jal 0x80029ac8(1)  solange  aca38 & 0x2000000
8001dadc: jal  0x80021634         ; (0,0) = Freigabe
```

Das Bit `0x2000000` ist der Tür-Task selbst: der Aufbau setzt es (`@0x80016210-18` `lui a0,0x200 / or / sw`),
der Abbau löscht es (`@0x80016678 lui a1,0xfdff`, `@0x8001668c ori a1,a1,0xffff`, `@0x800166a8 and`,
`@0x800166b0 sw`).

**Der Tür-Task** — Treiber `@0x80016188` läuft die Zeigerliste `0x80071d30` ab (`table 0x80071d2c 6`:
`0x800161e0`, `0x800164c8`, `0x80016664`, dann 0):

| Phase | Adresse | Was dort steht |
|---|---|---|
| Aufbau | `0x800161e0` | Objektpool, DO2 relozieren, TIM hochladen, Kamera setzen |
| Schleife | `0x800164c8-504` | solange `DAT_800b39ad != 0`: `jal 0x80016518` (Skripte), `jal 0x800166c4` (Zeichnen), `jal 0x80029ac8(1)` |
| Abbau | `0x80016664-b0` | wartet auf `0x8002178c(0) != 0` (Blende Kanal 0 fertig), löscht `0x2000000`, `DAT_800b5456 = 2` |

Die Skripte der Türszene laufen auf den **Ereignis-Slots 10..13 derselben SCD-Maschine** und über
**dieselbe Opcode-Tabelle**: `@0x80016520 ori s2,zero,0xa`, `@0x8001655c ori s1,zero,0xe60`,
`@0x80016568-70 Basis 0x800b2b4c + s1`, `@0x80016628 addiu s1,s1,368`, `@0x80016634 sltiu v0,s2,0xe`,
Tabelle `@0x80016528-2c = 0x800744a8`.

### 1.2 Port heute (Datei:Zeile, gemessen)

| Schritt | Stelle | Inhalt |
|---|---|---|
| Scan | `engine/src/game_step_common.c:2000` | `re15_aot_scan(...)` (eine von mehreren Aufrufstellen) |
| Tür erkannt | `engine/src/aot_common.c:1356` | `if (!aot_fire_door(i))` im `case RE15_AOT_TYPE_DOOR` |
| Tür per Skript | `engine/src/aot_common.c:700` | `re15_aot_fire_slot` → `aot_fire_door(slot)` (für `Aot_on`) |
| Feuer-Rumpf | `engine/src/aot_common.c:526` | `static int aot_fire_door(int i)` |
| Kreuz-Raum | `engine/src/aot_common.c:614` | `re15_room_request_change(dest_id, …)` → `g_room_change.pending = 1` (`room_common.c:186`) |
| Selbst-Tür | `engine/src/aot_common.c:620-647` | Teleport **sofort** (Zeilen 620-626), Band, `g_scd_pending_scenario = target_cut` (Zeile 647) |
| Verbrauch Selbst-Tür | `engine/src/game_step_common.c:2038` | im **selben Bild**: `scd_room_reenter` Zeile 2052, `re15_room_transition_present()` Zeile 2111 |
| Verbrauch Kreuz-Raum PC | `platform/pc/main.c:7054` | `re15_room_apply_pending(&rc)` (`room_common.c:196`), lädt die RDT synchron |
| Einblendung PC | `platform/pc/main.c:7105` | `re15_room_transition_present()` (`room_common.c:133`) |
| Props neu | `platform/pc/main.c:7157` | `pc_load_room_prop_set(...)` für den Zielraum |
| FSM-Tick | `platform/pc/main.c:4518` | `re15_room_transition_tick()` am Bildanfang (`room_common.c:155`) |
| Verbrauch PSX | `platform/psx/main.c:590` | `re15_room_apply_pending(&rc)` |

Der Port hat **keinen** Code für die Türszene: `grep -rn "DO2\|do2\|DOOR0" engine platform/pc include tests`
liefert 0 Treffer; Opcode `0x4F` steht nur als Länge 22 in `scd_vm.c:200` und fällt durch `op_unknown`
(`scd_vm.c:4140`), hat also keinen Handler. `main.c:7082` sagt es ausdrücklich: „RE1.5 has NO door-model
animation … in this path".

### 1.3 Die Einfügestelle

Im Original liegt die Sequenz **nach** Schwarz (`@0x8001d82c-34`) und **vor** der Einblendung
(`@0x8001ca5c → 0x8001cbbc`). Der Port lädt synchron, deshalb ist für das Bild gleichwertig, ob die
Sequenz vor oder nach `re15_room_apply_pending` läuft — solange die Welt währenddessen nicht gezeichnet
wird. Zwei Verbrauchsstellen müssen warten können:

1. **Kreuz-Raum:** vor `platform/pc/main.c:7054`. `g_room_change.pending` bleibt stehen, bis die Sequenz
   fertig ist. `re15_room_change_t` (`include/re15_room.h:185-191`) trägt heute nur
   `room_id, x, y, z, yaw_4096, target_cut` — **welche Tür** es war, geht auf dem Weg verloren und muss als
   neues Feld mit.
2. **Selbst-Tür (das Tor ist eine):** vor `engine/src/game_step_common.c:2038`. Der Teleport in
   `aot_common.c:620-626` ist dann schon geschehen; das ist unsichtbar, solange die Türszene das Bild trägt.

Vorbild für „eigene Szene mit eigener Kamera, eigener 30-Hz-Schleife, eigenen TIM-Slots" ist
`pc_run_player_select` (`platform/pc/main.c:2019`): eigene `re15_camera_view_t` (Zeilen 2088-2091),
`re15_render_begin_frame()`/`end_frame()` je Bild (2157/2177), Modelle in Slots 20..23. Das entspricht der
Form des Originals, dessen Türszene ebenfalls eine eigene Schleife ist (`0x800164c8`).

Unverändert bleiben muss `re15_room_transition_present()` samt `re15_room_transition_tick()`: die
6-Bild-Einblendung ist byte-true belegt (`room_common.c:68-124`) und folgt im Original **nach** der Tür.

### 1.4 Die Tür sagt dem Original, welches Archiv — die RE1.5-Daten nutzen das nicht

```
800171fc-80017200: lw   v0,DAT_800ac9a8
8001720c: lbu  v0,12(v0)          ; Nutzlast+0xC  = Door_aot_set pc[26]
80017214: sll  v0,v0,1
80017218-24: lhu a0, 0x80071d2c[v0]   ; Datei-Id
800171f8/80017204: a1 = 0x801a1000    ; Ladeadresse
800164a0: lw   v0,DAT_800ac9a8
800164a8: lbu  v0,13(v0)          ; Nutzlast+0xD  = Door_aot_set pc[27]
800164b4: sh   v0,4062(at)        ; DAT_800b0fde
```

Tabelle `@0x80071d2c`: Bytes `25 00 00 00` → Eintrag 0 = Datei-Id `0x25`, Eintrag 1 = 0.

**Zensus** (`python build/tor_1170/_pa_door_zensus.py`, Walker = `re15_port/tools/scd_walk_lib.py`):
240 RDT-Dateien, **653** `Door_aot_set`-Records.

| `pc[26]` / `pc[27]` | Anzahl | Fundstelle |
|---|---|---|
| `0x00` / `0x00` | 649 | alle echten Türen |
| `0x86` / `0x24` | 2 | ROOM4030/4031 main00 `@0x47E` |
| `0x34` / `0x08` | 2 | ROOM4030/4031 main00 `@0x4A6` |

Die vier Ausreißer sind dieselben, die `aot_common.c:561-562` schon als „non-door scan artifacts" führt.
Der Port liest `pc[26]` und `pc[27]` heute gar nicht (`scd_vm.c:3796-3857`, `re15_aot_door_params_t` in
`include/re15_aot.h:126-147` hat kein Feld dafür).

**Die Tor-Records** (Bytes aus den ausgelieferten RDTs):

| Raum | Slot | Datei | Rechteck (x,z,w,d) | Ziel | Band | `pc[26..27]` |
|---|---|---|---|---|---|---|
| ROOM1170 | 0 | `@0x1206` | (1500,14400,2100,1700) | Cut 11, (-11710,-7200,-26500), Yaw 2957 | 4 | `00 00` |
| ROOM1170 | 6 | `@0x135A` | (-11940,-28450,1750,1200) | Cut 0, (2300,-7200,14365), Yaw 1024 | 4 | `00 00` |
| ROOM1171 | 0 | `@0x11DC` | wie oben Slot 0 | wie oben | 4 | `00 00` |
| ROOM1171 | **5** | `@0x12F0` | wie oben Slot 6 | wie oben | 4 | `00 00` |
| ROOM1170 | 3 | `@0x12CC` | **(0,0,0,0)** | Cut 0, (2300,-7200,14365), Yaw 1024 | 0 | `00 00` |

Zwei Folgen für den Plan:

- Die Slot-Nummer taugt **nicht** als Schlüssel: die Rückrichtung ist in ROOM1170 Slot 6, in ROOM1171 Slot 5.
- ROOM1170 Slot 3 ist die Intro-Übergabe (`Aot_on(3)` aus sub11) mit **identischer Nutzlast** wie das Tor
  zurück — sie unterscheidet sich nur im Rechteck (0,0,0,0) und im Band (0). „Selbst-Tür in ROOM1170" allein
  würde die Torsequenz also mitten ins Intro legen. Ein Schlüssel muss das Rechteck oder das Band mitführen.

---

## 2. Wie der Port heute ein MD1 mit TIM zeichnet

### 2.1 Schnittstelle

| Schritt | Funktion | Stelle | Eigenschaft |
|---|---|---|---|
| MD1 lesen | `re15_md1_parse(data,size,&md1)` | `engine/src/md1_common.c:41` | kopiert nicht, Zeiger in den Quellpuffer; Kopf 12 B, Mesh-Kopf 56 B; höchstens `MD1_MAX_MESHES = 32` (`include/re15_md1.h:41`); Feld `length` wird nur gespeichert, nie geprüft |
| TIM lesen | `re15_tim_parse` | `engine/src/tim_common.c:24` | 4/8/16/24 bpp, CLUT optional |
| TIM hochladen | `re15_render_pc_upload_tim_slot(&tim, slot)` | `platform/pc/src/render_pc.c:2072` | je CLUT-Zeile eine Kopie, gestapelt; `n_cluts = clut_entries / 256` bei 8 bpp |
| Slot wählen | `re15_render_pc_bind_tim_slot(slot)` | `render_pc.c:286` | leerer Slot → laute Warnung, alter Slot bleibt |
| Dreieck | `re15_render_textured_tri_lit(...)` | `render_pc.c:2365` | UV in Texeln, Tiefe `z` = Sortierschlüssel, Farbe je Ecke |

**Dreiecke und Vierecke:** beide. Die Prop-Schleife zeichnet Dreiecke (`main.c:9620-9674`) und Vierecke
(`main.c:9675-9740`), ein Viereck als zwei Dreiecke (0,1,3) + (0,3,2) (`main.c:9728-9739`).

**Texturseite:** `page_off = (uv->page & 0x000F) * 128` (`main.c:9650` und `9706`). Bei einem 128 Texel
breiten TIM ist nur Seitenindex 0 gültig, also Seitenwort `0x80`.

**CLUT:** Zeile = `(clut >> 6) & 0x1FF` minus `clut_y` des TIM (`render_pc.c:2388-2391`, gleichlautend
`2292-2295`). UV-Wort `0x7800` = `(480 << 6) | 0` trifft bei `clut_y = 480` die Zeile 0.

**Farbschlüssel:** am **Wert**, nicht am Index — `re15_tim_texel_argb` (`include/re15_tim.h:127`): aufgelöster
Texel `0x0000` wird nicht gezeichnet, alles andere opak (Bit 15 wird ignoriert, `0x8000` ist also opakes
Schwarz).

**Beleuchtung:** je Ecke NCCT über `re15_light_shade_vertex` (`engine/src/light_common.c:244`) mit dem
Lichtsatz des **aktiven Raum-Cuts** (`main.c:9594-9606`). Normalen sind Pflicht; ohne sie fällt der Zeichner
auf `g_re15_light_tint` zurück. Primitiv-Farbe `0x80` = Textur unverändert (`psx_prim_to_sdl_vert`,
`render_pc.c:2356`). Nur Umgebungslicht `0xFF` ohne Lichtquelle ergibt gerechnet
`(255<<16)>>12 = 4080`, dann `(128*4080)>>12 = 127` (`light_common.c:283-304`) — also praktisch neutral.

**Was fehlt:** eine aufrufbare Funktion „starres MD1 mit Matrix zeichnen". Der Code steht nur **inline** in
der Prop-Schleife `main.c:9508-9764` (Mesh-Schleife ab 9608) und hängt an den lokalen Größen `cam_view`,
`cx`, `cy` und dem Raumlicht. `pselect_render_model` (`main.c:1822`) ist die Skelett-Variante mit eigener
Kamera. Für die Türszene muss der Rumpf 9608-9741 als Funktion herausgelöst werden.

**Slots:** `RE15_TIM_SLOT_MAX = 50` (`render_pc.c:179`), Belegung laut Tabelle `render_pc.c:179-205`
vollständig vergeben. Frei sind die Slots **24 und 25**: `main.c:141-142` definiert sie als
„RESERVED/unused", und `grep -rn "RE15_TIM_SLOT_WPN" platform engine include` findet nur diese beiden
Definitionen. Ein Raum-Prop-Slot taugt für das Tor **nicht**: bei der Selbst-Tür wird der Raum nicht neu
geladen, ROOM1170 belegt mit `nOmodel = 6` (RDT Byte `@0x02`) die Slots 4..9, und die Textur wäre danach
zerstört.

**Warteschlange:** `TEXTRI_QUEUE_MAX = 8192` Dreiecke je Bild (`render_pc.c:172`); darüber wird still
verworfen.

### 2.2 Vorbild Sicherung: von den Originalpixeln zum eingebackenen MD1/TIM

| Stufe | Datei | Funktion | Was sie tut |
|---|---|---|---|
| Pixel holen | `tools/sicherung_modell.py` | `bg_laden(name)` | lädt `build/bg_ppm/<name>.ppm` (Engine-Dekoder) |
| Textur | | `textur_bauen(breite,hoehe)` | Ausschnitt `QUELLE_BG08`, NEAREST, gespiegelt für die Rückseite |
| Geometrie | | `ring()`, `modell_bauen()` | Rotationskörper; liefert `(v, vt, faces)` mit 1-basierten OBJ-Indizes |
| Sichtmodell | | `schreiben()` | OBJ + MTL + PNG |
| TIM | `tools/sicherung_engine_export.py` | `tim_bauen()` | 128x256, 8 bpp, 255 Farben, Index 0 frei, CLUT `@VRAM(0,480)`, Bild `@VRAM(0,0)` |
| MD1 | | `md1_bauen()` | EIN Mesh, geteilte Listen, Y gespiegelt, Normalen auf 4096, UV-Wörter `0x7800`/`0x80` |
| Einbacken | | `carr()`, `main()` | C-Felder nach `engine/src/gen/sicherung_prop.inc` |
| Abnahme | `tools/sicherung_abnahme.py` | `basis()`, `projiziere()`, `rueckprojekt()`, `silhouette_original()` | Kamera aus der RDT, Silhouette zeilenweise gegen das Original |
| Sichtprobe | `tools/sicherung_probe.py` | `rendern()` | affiner Rasterer mit Z-Puffer, Bild neben das Original |
| Engine | `engine/src/sicherung_1150.c` | `re15_sicherung_md1_bytes/_tim_bytes/_install/_tick` | Bytes ausliefern, Prop anlegen |
| Lader | `platform/pc/main.c:1360-1375` | in `pc_load_room_prop_set` | hängt MD1/TIM in Slot `RE15_TIM_SLOT_PROP(4)` |
| Sonde | `tests/unit/probe_sicherung_1150.c` | | fährt den Ablauf und misst |

Gemessen am eingebackenen Ergebnis (`_pa_sammeln.py`, Schlüssel `sicherung_eingebacken`):
MD1 2532 B, Kopf `(65, 0, 2)`, 50 Punkte, 16 Dreiecke, 40 Vierecke; TIM **33312 B**.

Die Abnahme läuft heute noch (`python re15_port/tools/sicherung_abnahme.py`, nur lesend): Breite Median
+0.0 px, Betrag-Mittel 0.58 px über 57 unverdeckte Zeilen, Versatz -0.43 px.

### 2.3 Das Zielformat ist dasselbe — gemessen an allen 56 Türmodellen

`python build/tor_1170/_pa_do2_parse.py re15_port/shared_assets/PSX/DOOR/DOOR00.DO2` und
`python build/tor_1170/_pa_re2_zensus.py`:

| Größe | RE1.5 DOOR00 | RE2, 55 Modelle |
|---|---|---|
| TIM | 128x256, 8 bpp, CLUT 256x1, **33312 B** | alle 55: 128x256, 8 bpp, CLUT 256x1, **33312 B** |
| TIM-Lage | Bild `@VRAM(0,0)`, CLUT `@VRAM(0,480)` | alle 55 gleich |
| UV-Wörter | clut `0x7800`, page `0x80` | 54 gleich; DOOR-Ausnahme: 1 Modell mit `0x7fc0`/`0x95` |
| Listen | `tv==qv`, `tn==qn` | alle geteilt |
| Normalen | Länge 4096 | 4095..4097, vereinzelt 0 |
| Dreiecke | 12 + 33 = 45 | min 12, Median 80, max 490 (DOOR26), Summe 8560 |
| Vierecke | 0 | 0 |
| Punkte | 8 + 24 = 32 | min 8, Median 50, max 324 |
| Meshes | 2 | min 1, Median 2, max 5 |
| MD1-Größe | 2444 B | 612..24244 B |

`DOOR00.md1` und `DOOR00.tim` aus RE2 sind byte-gleich mit den RE1.5-Bytes `@0x18..0x9A4` bzw. `@0x9A8`
(im selben Lauf geprüft).

RE1.5-Container `DOOR00.DO2` (57016 B, sha1 `5725afcb…ea6`): Kopf `0c 00 00 00 | c8 8b 00 00 | f8 97 00 00`,
Tabelle `@0x0C` = `0c 00 00 00 | 98 09 00 00 | 9c 09 00 00`, jeweils +0xC (`@0x800162f4 ori a0,a0,0x100c`)
→ MD1 `@0x18`, SCD `@0x9A4` (`02 00 01 00`, 4 B), TIM `@0x9A8`. **Der RE2-Container ist anders gebaut**
(`xxd -l 32 info/re2leon/COMMON/DOOR/DOOR00.DO2`: `00 00 14 16 | 00 00 24 17 | ff ff ff ff | ff ff ff ff`,
ab `@0x10` die Kennung `pBAV`); mein RE1.5-Parser läuft an allen 55 RE2-Archiven auf. Den Zensus der
RE2-Modelle habe ich deshalb an den bereits entpackten `DOORxx/DOORxx.md1` und `.tim` gefahren. Wer ein Archiv für den RE1.5-Lader schreibt, muss die RE1.5-Form nehmen.

### 2.4 Was wiederverwendbar ist — und was nicht

| Teil | Urteil | Grund |
|---|---|---|
| `tim_bauen()` | direkt, mit **einer** Korrektur (s. u.) | erzeugt exakt das Format aller 56 Tür-TIMs (33312 B) |
| `carr()` | direkt | reiner C-Feld-Schreiber |
| `basis()` | direkt | erzwingt `u[1] > 0` (PSX: +Y unten) statt es anzunehmen |
| `projiziere()` | direkt, Kamera als Parameter | heute fest auf ROOM1050 Cut 8 (`CAM`, `TGT`, `H` als Modulkonstanten) |
| `rendern()` | direkt | affiner Rasterer, unabhängig vom Gegenstand |
| `md1_bauen()` | nur nach Umbau | EIN Mesh; `uv()` fest auf das Feld 32x128 (`u*31`, `(1-v)*127`); Kopf fest `(65,0,2)`; schreibt Vierecke — alle 56 Türmodelle haben 0 |
| `rueckprojekt()` | nur nach Umbau | kennt nur Ebenen `x = const` |
| `textur_bauen()` | nur nach Umbau | spiegelt für einen Rotationskörper; das Tor hat zwei **verschiedene** Seiten (Cut 0 gegen Cut 11/12) |
| `silhouette_original()` | **nicht übertragbar** | lebt von der Differenz zweier Zustandsbilder; vom Tor gibt es kein Bild „ohne" |
| `bg_laden()` | Quelle prüfen | `build/bg_ppm/` ist im Hauptbaum **leer** (0 Dateien); die Sicherungswerkzeuge laufen nur, weil der zweite Eintrag `.claude/worktrees/wf_5ebaf1dc-c6e-1/build/bg_ppm` (1119 Dateien) noch existiert |
| `HELLIGKEIT = 2.87` | **nicht übernehmen** | gemessen für das Raumlicht von ROOM1150; s. 2.5 |

**Die Korrektur an `tim_bauen()` — Schwarz wird zum Loch.** Die Funktion rechnet jede Palettenfarbe nach
RGB555; reines Schwarz wird `0x0000`, und das ist der Farbschlüssel. Gemessen am eingebackenen Sicherungs-TIM:
CLUT-Einträge mit Wert `0x0000` sind `[0, 251, 252, 253, 254, 255]`, und **68 von 4096** Texeln des benutzten
Felds 32x128 zeigen darauf — das sind Löcher im ausgelieferten Modell. Beim Tor wäre das kein Randeffekt:

| Ausschnitt | Pixel | davon RGB555 = `0x0000` |
|---|---|---|
| gate_01 (30x28) | 840 | 6 |
| gate_02 (44x36) | 1584 | 97 |
| gate_03 (111x76) | 8436 | **1865** (22,1 %) |

(`python build/tor_1170/_pa_tor_schwarz.py`; Zählregel: alle drei Kanäle `>> 3 == 0`, dieselbe Kürzung wie
in `tim_bauen`.)

Die Originale lösen das über den Wert `0x8000` (`python build/tor_1170/_pa_tim_schluessel.py`):

| 56 Tür-TIMs | Anzahl |
|---|---|
| mit mindestens einem CLUT-Eintrag `0x8000` | 31 |
| mit Texeln, die auf einen `0x0000`-Eintrag zeigen (echte Aussparung) | 7 |
| Texel auf `0x8000`, Summe | 10485 |
| Texel auf `0x0000`, Summe | 34579 von 1835008 |

Also: **Schwarz = `0x8000`, Aussparung = `0x0000`.** Das Tor braucht beides — schwarze Streifen im
Warnschild und die offene Fläche zwischen Rohrrahmen und Schild.

Eine zweite Falle im selben Werkzeug: der Upload warnt, wenn kein Texel transparent ist **und** mindestens
ein Viertel opak schwarz dekodiert (`render_pc.c:2138`). Die ungenutzte Restfläche des 128x256-Blatts muss
deshalb auf den Schlüssel (`0x0000`) zeigen, nicht auf `0x8000`.

### 2.5 Helligkeit: die Tür-Texturen sind dunkel, die Ausschnitte nicht

`python build/tor_1170/_pa_tim_helligkeit.py` — Summe R+G+B je Texel (0..765), nur gezeichnete Texel:

| Quelle | Mittel | 99. Perzentil |
|---|---|---|
| RE1.5 DOOR00 | 60,0 | 192 |
| RE2, 55 Modelle | 32,1 … **116,5** (Median) … 269,4 | 128 … 360 (Median) … 704 |
| gate_01 | 286,2 | 669 |
| gate_02 | 150,4 | 503,5 |
| gate_03 | 91,8 | 356 |

Die Türszene setzt das Umgebungslicht auf Weiß (`@0x80016448-54`: `ori a0/a1/a2,zero,0xff`, `jal 0x80066420`;
dort `sll a0,a0,4 / ctc2 a0,$13` usw.) und die Objektfarbe auf `0x808080` (`@0x8001704c-50`, `@0x80017098
sw a2,112(s0)`). Eine Aufhellung wie bei der Sicherung ist danach weder nötig noch begründet. Welcher
Ausschnitt die Eigenfarbe am besten trägt, ist eine Frage des Modellbaus, nicht dieser Untersuchung.

### 2.6 Zwei Pixelstände desselben Hintergrunds

`python build/tor_1170/_pa_dekoder_vergleich.py`. Die PPM unter `build/tor_1170/_pa_bg_port/` stammen aus
der **vorhandenen** `re15_port/build/tests/unit/probe_bg_dump.exe` (2026-09-27 21:36, nicht neu gebaut, als
Kopie aus dem Scratchpad gestartet: `… probe_bg_dump.exe re15_port/shared_assets/PSX <ziel> 1`, 428 Bilder,
0 Fehlschläge).

| Ausschnitt | gegen Extraktor-BMP | gegen Engine-Dekoder | gleiche Pixel |
|---|---|---|---|
| gate_01, Cut 0 (133,81) | Mittel 0,000 / max 0 | Mittel 3,683 / max 39 | 21 von 840 |
| gate_02, Cut 11 (73,60) | Mittel 0,000 / max 0 | Mittel 1,734 / max 11 | 134 von 1584 |
| gate_03, Cut 12 (107,115) | Mittel 0,000 / max 0 | Mittel 1,502 / max 11 | 1589 von 8436 |

Die Verortung aus dem Auftrag ist damit gegengeprüft (Abweichung 0,000). Zugleich gilt: die Nutzer-Ausschnitte
stammen aus dem **Java-Extraktor**, der Port zeigt einen **anderen** Dekoderstand. Eine Abnahme, die das
gerenderte Tor gegen einen Hintergrund stellt, muss sagen, gegen welchen. Die BSS-Quelle ist dieselbe
(sha1 `3eb00ab9…dd2d2` für beide `ROOM117.BSS`).

---

## 3. Grenzen

### 3.1 Aus dem Original (selbst disassembliert)

| Größe | Wert | Beleg |
|---|---|---|
| Objekte je Türszene | **4** | `@0x800161e4 ori a3,zero,0x3`, Schleife `@0x8001621c-2c` bis `bgez a3` |
| Objektgröße | 144 B | `@0x8001622c addiu a1,a1,-144` |
| Objektpool | oberstes `@0x801bd1b0`, Zeiger `0x800b23f4..0x800b2400` | `@0x800161f0-f4 lui a1,0x801b / ori a1,a1,0xd1b0`, `@0x800161e8-ec` |
| Slot-Prüfung im Opcode `0x4F` | **keine** | `@0x80016f30 lbu t1,1(a2)`, `@0x80016f38 sll v0,t1,2`, `@0x80016f48 lw a1,0(at)` — kein Vergleich dazwischen |
| Elternteil | höchstens Slot 0..7 kodierbar | `@0x80016fe4 andi a3,v0,0x8`, `@0x80016ff4 andi v0,v0,0x7` |
| Ladeadresse DO2 | `0x801a1000` | `@0x800171f8 lui a1,0x801a`, `@0x80017204 ori a1,a1,0x1000` |
| Primitiv-Puffer | beginnt `0x801ab000` | `@0x8001635c-60`, `@0x80016378 sw v0,DAT_800b8550` |
| Mesh-Schritt | 56 B | `@0x8001705c sll v0,a1,3`, `@0x80017060 subu`, `@0x8001706c sll v0,v0,3` |
| Dreiecks-/Vierecksgruppe | `+0x00` / `+0x1C` | `@0x8001708c sw v0,16(s0)`, `@0x80017090 addiu v0,v0,28`, `@0x80017094 sw v0,24(s0)` |
| Skript-Slots | 10..13 | s. 1.1 |
| Kamera der Szene | Ort (30000,0,0), Ziel (22000,0,0) | `@0x80016460 ori v0,zero,0x7530` → `0x800b2210`, `@0x80016468 ori v0,zero,0x55f0` → `0x800b221c`, Nullen `@0x80016470-90`, `@0x80016494 jal 0x80053ca4` mit `a0 = 0x800b220c`; das Sprungziel liest Ziel `@0x80053d1c lw v0,16(s3)` und Ort `@0x80053d20 lw v1,4(s3)` — dieselbe Lage wie `re15_camera_cut_t` (`include/re15_camera.h:27-39`) |
| Bildmitte | (160,120) | `@0x8001643c ori a0,zero,0xa0`, `@0x80016444 ori a1,zero,0x78`, `jal 0x80066d60` |

Rechnerisch liegen zwischen Ladeadresse und Primitiv-Puffer `0x801ab000 - 0x801a1000 = 0xA000 = 40960 B`.
DOOR00.DO2 ist 57016 B groß; TIM und VAB sind zu dem Zeitpunkt aber schon hochgeladen. Ob daraus eine harte
Obergrenze für MD1 + Skript folgt, habe ich **nicht** belegt (s. OFFEN 3).

Opcode `0x4F` (22 B), Tabelle `0x800744a8 + 0x4F*4 = 0x800745e4 → 0x80016f20`:

| Byte | Ziel | Bedeutung nach dem Lesen |
|---|---|---|
| `pc[1]` | Index in `0x800b23f4` | Objekt-Slot |
| `pc[2]`, `pc[3]` | `+8`, `+9` | je 1 Byte |
| `pc[4]` | `+0` (als Wort) | |
| `pc[5]` | `+0x8E` | Mesh-Index (geht `@0x80017028` als `a1` an `0x80017048`) |
| u16 `pc[6]` | `+0x8C` | Bit `0x8` = hat Elternteil, `& 7` = Eltern-Slot, dann `+0x74 = Eltern + 0x48` |
| s16 `pc[8]` | `+0x0C` | |
| s16 `pc[10]`, `[12]`, `[14]` | `+0x34`, `+0x38`, `+0x3C` | Position |
| u16 `pc[16]`, `[18]`, `[20]` | `+0x68`, `+0x6A`, `+0x6C` | Drehung |

Das sind dieselben Objekt-Offsets, die der Port für Raum-Props schon abbildet (`+0x0C` Flags, `+0x34/38/3C`
Position, Elternmatrix `+0x48`; vgl. `main.c:1336-1342` und `pc_prop_world`, `main.c:594`). Der Port-Pool
`g_scd.props` (`RE15_SCD_MAX_PROPS = 17`, `include/re15_scd.h:36`) trägt `parent_obj`, Position und Drehung —
die Felder reichen, aber die Türszene braucht einen **eigenen** Pool, sonst überschreibt sie bei der
Selbst-Tür die Props des Raums.

### 3.2 Aus den Modellen

Für das Tor als Anhalt, nicht als Vorschrift: Median der 55 RE2-Türen 80 Dreiecke / 50 Punkte, das
RE1.5-Modell 45 / 32. Textur: **ein** Blatt 128x256 mit **einer** 256er-CLUT, UV in 0..127 x 0..255. Die
drei Ausschnitte passen zusammen hinein (30x28 + 44x36 + 111x76 = 10860 von 32768 Texeln).

Farben: eine CLUT hat 256 Einträge, Index 0 gehört dem Farbschlüssel, bleiben **255**. Die Ausschnitte
tragen 819 / 1380 / 2808 verschiedene Farben (`_pa_tor_schwarz.py`, Feld `farben`). Ohne Quantisierung
geht es also nicht; `tim_bauen()` macht sie mit `Image.ADAPTIVE, colors=255`
(`tools/sicherung_engine_export.py:63`).

### 3.3 PSX-Zielplattform — was im Code steht

| Größe | Wert | Stelle |
|---|---|---|
| OT-Länge | 1024 | `platform/psx/src/render.c:31` |
| Paketpuffer je Bild | `0x10000` = 65536 B, zweimal | `platform/psx/src/render.c:56`; Kommentar 50-55: 256 KB x 2 sprengte den Speicher, ~81 KB bleiben frei |
| VRAM-Pool | 8 Plätze zu 64 Spalten x 256 Zeilen (= 128x256 Texel bei 8 bpp) | `platform/psx/src/vram_psx.c:18-22` |
| CLUT-Zeilen dynamisch | 483..501 (19 Zeilen) | `platform/psx/src/vram_psx.c:33-34` |
| Prop-Lader | kappt bei `id >= 6`, nur 8 bpp | `platform/psx/src/asset_psx.c:334` und `337` |
| Nah-Ausschluss | OTZ < 64 | `platform/psx/src/mesh_psx.c:269`; Original `@0x80025654 sra v0,v1,6` / `@0x80025658 beq v0,zero,0x80025690`, OT-Index `@0x8002565c sra v1,v1,4` (selbst gelesen) |
| Kantenlänge GP0 | dx ≤ 1023, dy ≤ 511 | `platform/psx/src/mesh_psx.c:295-296` |

ROOM1170 belegt den VRAM-Pool laut `vram_psx.c:23-25` schon mit 8 von 8 Plätzen (5 Props + Elliot 2 +
em21 1). Ein Tür-TIM braucht einen Platz; im Original ist das kein Konflikt, weil die Türszene die Welt
nicht zeichnet. Für den Port heißt das: das Tür-TIM **zeitweise** laden und danach den Raum neu hochladen,
oder einen Platz freimachen.

Die Laufzeit-Regel des Projekts steht nur im Gedächtnis (`reai-v2-psx-laufzeitbudget`: Schweres in den
Generator, die Laufzeit schlägt nach) und als Verweis in `tools/sicherung_modell.py:77`. Eine in Zahlen
gefasste Dreiecks- oder Zeitgrenze für den PSX-Port habe ich im Code und in den Dossiers **nicht** gefunden
(`grep -rn "PSX-Budget\|Laufzeitbudget"` → 5 Treffer, keiner mit einer Modellgrenze).

PC-seitig: `TEXTRI_QUEUE_MAX = 8192`, `RE15_TIM_SLOT_MAX = 50`, `MD1_MAX_MESHES = 32`, `SCD_THREAD_COUNT = 24`
(`include/re15_scd.h:51`, Ereignis-Slots 10..23).

### 3.4 Kamera der drei Ansichten

`python build/tor_1170/_pa_kamera_1170.py` — Tabelle `@0x60` (Zeiger u32 `@RDT+0x24`), 32 B je Cut,
`nCut = 13` (Byte `@0x01`):

| Cut | Datei | flag | fov | H = fov>>7 | Ort | Ziel |
|---|---|---|---|---|---|---|
| 0 | `@0x60` | 0 | 26684 | 208 | (-1170,-11682,4050) | (4266,-5058,17892) |
| 11 | `@0x1C0` | 0 | 26684 | 208 | (-15606,-11556,-20988) | (-13266,-4644,-30204) |
| 12 | `@0x1E0` | **1** | 26684 | 208 | (-10756,-7256,-23388) | (-11916,-9444,-29804) |

`H = fov >> 7` steht in `engine/src/camera_common.c:38` (nicht in `render_pc.c:30-38`, wie
`sicherung_modell.py:24` zitiert — die Angabe dort ist veraltet).

Cut 12 liegt mit der Kamera auf y = -7256, also 56 Einheiten über dem Boden y = -7200, und schaut nach
oben: der Boden steht dort fast auf Kante. **Eine Rückprojektion auf die Bodenebene ist in Cut 12
unbrauchbar**; für diese Ansicht braucht es die senkrechte Torebene.

Plausibilitätsprobe (`python build/tor_1170/_pa_projektion.py`, Gleitkomma-Nachbau des LookAt, also auf
etwa 1 px, **kein** byte-true Nachbau): die Tür-Rechtecke auf Bodenhöhe y = -7200 fallen in den Cuts dorthin,
wo die Ausschnitte liegen.

| Cut | Rechteck | projiziert x | projiziert y | Tiefe z | Ausschnitt |
|---|---|---|---|---|---|
| 0 | Slot 0 | 129,2 … 171,2 | 102,1 … 116,0 | 11515 … 13662 | x 133..162, y 81..108 |
| 11 | Slot 6 | 69,8 … 120,9 | 86,8 … 105,4 | 8200 … 9490 | x 73..116, y 60..95 |
| 12 | Slot 6 | 85,8 … 186,3 | 192,3 … 193,3 | 3490 … 4904 | x 107..217, y 115..190 |

Der Ausschnitt liegt jeweils **über** der Bodenlinie des Rechtecks — wie es für ein hüfthohes Tor sein muss.

---

## 4. Assets: wie der Port sie findet, und wohin ein neues Türarchiv gehört

**Finden:** `pc_read_shared(rel,&size)` (`platform/pc/main.c:486`) → `re15_pc_read_any` mit der
gemeinsamen Wurzelliste aus `platform/pc/src/asset_root_pc.c`: Umgebungsvariablen, dann das Verzeichnis der
laufenden exe samt Vorfahren, dann das Arbeitsverzeichnis, zuletzt der einkompilierte Standard; je Wurzel
erst `shared_assets/PSX/`, dann der Geschwisterbaum (`extracted_fx/`, `RE2/`).

**DOOR00.DO2:** liegt in `re15_port/shared_assets/PSX/DOOR/` (57016 B, sha1-gleich mit
`info/Re1.5/PSX/DOOR/DOOR00.DO2`) und wird **nirgends gelesen**. Im PC-Port, in der Engine, in den Headern
und in den Tests gibt es 0 Treffer für `DO2`. Die einzigen Erwähnungen stehen auf der PSX-Seite und lesen die
Datei ebenfalls nicht: `platform/psx/src/audio_psx.c:6,39` (Kommentar zur gebündelten VAB),
`targets/psx/CMakeLists.txt:79` (dort wird ausdrücklich ein 4-Byte-Platzhalter eingebettet) und
`platform/psx/main.c:413-416` (ein alter Kommentar über die Ausdehnung von `DOOR00.md1`).

**Was das Repo zur Ablage schon sagt:**

| Aussage | Stelle |
|---|---|
| Neuer Inhalt kommt eingebacken, die ausgelieferten RDTs bleiben byte-true | `tools/sicherung_engine_export.py:5-10`, `include/re15_sicherung.h:26-29`, `engine/src/scd_room_setup.c:345-347` |
| Fremdes Material liegt **neben** dem PSX-Baum, „der RE1.5-Single-Asset-Root bleibt unangetastet" | `platform/pc/main.c:659-663` (`shared_assets/RE2/`) |
| Eine Datei kann im Paket liegen und zur Laufzeit trotzdem nicht gefunden werden | `platform/pc/src/asset_selftest_pc.h:10-21` (Bericht 0.3.19) |
| Das Paket kopiert den Baum ganz, prüft aber nur benannte Dateien | `release/make_package.sh:363-369` (Kopie), `176-187` (Prüfung) |
| Es gibt portseitig erzeugte Dateien im Baum | `shared_assets/extracted_fx/*.tim` (aus `tools/vram_png_to_tim.py`), `shared_assets/PSX/MASKS/` (8,3 MB) |

**Folgerung.** Beide Wege haben im Repo ein Vorbild. Für das Tor spricht mehr für das Einbacken:

1. Es ist neuer Inhalt, kein Originalbestand — dieselbe Lage wie bei der Sicherung.
2. Ein eingebackenes Feld kann nicht „im Paket liegen und nicht gefunden werden"; es braucht weder einen
   Eintrag im Paket-Gate noch im Asset-Selbsttest, und es ist auf Windows, Linux und Android ohne weiteres da.
3. Die Größe ist bekannt und klein: TIM 33312 B plus MD1 (DOOR00: 2444 B).

Dagegen steht die PSX-Zielplattform: dort bleibt ein eingebackenes TIM dauerhaft im Arbeitsspeicher, und
`render.c:50-55` beziffert den freien Rest auf rund 81 KB. Das Original lädt das Archiv deshalb bei Bedarf
von der CD nach `0x801a1000`. Deshalb die Empfehlung: **der Generator schreibt beides** — das eingebackene
Feld für den PC-Stand und zusätzlich einen RE1.5-förmigen DO2-Container nach `build/tor_1170/`, der erst
dann in den Asset-Baum wandert, wenn der PSX-Port ihn braucht. Die endgültige Asset-Strategie ist laut
Gedächtnis (`reai-v2-single-asset-root`) ohnehin bis zum Schluss zurückgestellt.

Ein Archiv unter `shared_assets/PSX/DOOR/DOOR01.DO2` hätte außerdem einen Haken: die Datei-Id-Tabelle
`@0x80071d2c` kennt nur Eintrag 0 (`0x25`), Eintrag 1 ist 0. Ein zweites Archiv ist im Original nicht
adressierbar; der Pfad wäre also Port-Erfindung, auch wenn er wie Originalbestand aussieht.

---

## 5. Sonden und Tests

| Baustein | Stelle | Regel |
|---|---|---|
| Sonde je Thema | `re15_port/tests/unit/probes/<thema>.cmake` | eingebunden über `file(GLOB _re15_probe_cmakes …)`, `tests/unit/CMakeLists.txt:4060`; heute 74 Dateien |
| Quelltext | `re15_port/tests/unit/probe_<thema>.c` | bindet `re15_engine` + `re15_test_support`, **nicht** SDL |
| Asset-Pfade | `target_compile_definitions(… RE15_ASSET_PSX_DIR=… RE15_ASSET_RE2_DIR=…)` | Vorbild `probes/r16_tuer-animation-1040.cmake` |
| Messsonde gegen Riegel | ohne bzw. mit `add_test` + `TIMEOUT` | Vorbild mit Test: `probes/r35_sicherung-1050.cmake`, `probes/r18_irons_mittelmodell.cmake` |
| Eigenes Bauverzeichnis | `re15_port/build_<thema>` | `RE15_BUILD_DIR=… bash re15_port/tools/local_build.sh` (`local_build.sh:207`); `-DFETCHCONTENT_SOURCE_DIR_SDL2=<repo>/re15_port/build/_deps/sdl2-src`, so steht es in `build_r30_sicherung/CMakeCache.txt:340` |
| Tests einschalten | `-DRE15_BUILD_TESTS=ON` | Standard ist OFF, `re15_port/CMakeLists.txt:111` (CLAUDE.md nennt noch Zeile 100) |
| Neue Engine-Datei | `engine/src/*.c` | wird über `file(GLOB ENGINE_SOURCES "src/*.c")` erfasst, `engine/CMakeLists.txt:8` |
| Dossier | `analysis/<runde>/<thema>.md` | hier `analysis/tor_1170/` |

**Vorbilder für das Tor:**

- Türablauf über den echten Pfad: `tests/unit/probe_r16_tuer1040.c` — eigene `re15_room_apply_ctx_t`-Rückrufe
  (Zeilen 106-133), `re15_room_transition_tick()` vor dem Spielschritt (Zeile 243), dann
  `re15_room_apply_pending` + `re15_room_transition_present` (Zeilen 540-541).
- Abnahme eines eingebackenen Modells: `tests/unit/probe_sicherung_1150.c`, registriert in
  `tests/unit/CMakeLists.txt:902`.
- Selbst-Türen in ROOM1170: `tests/unit/probe_door_1170.c`.

**Lücke im Vorbild:** kein Test lässt die eingebackenen Bytes durch die Parser laufen
(`grep -rn "re15_sicherung_md1_bytes\|re15_sicherung_tim_bytes" re15_port/tests` → 0 Treffer). Für das Tor
gehört genau das an den Anfang: MD1 und TIM parsen, Dreieckszahl, 0 Vierecke, UV-Wörter, kein Texel auf
einem ungewollten `0x0000`-Eintrag.

**Was eine Sonde nicht sehen kann:** das gezeichnete Bild. Die Zeichenseite liegt in `platform/pc/main.c`
und hängt an SDL. Dafür gilt die Sichtprüfung am echten Fenster (Skill `re15-port-visual-verify`).

**Bestehende Riegel, die grün bleiben müssen:** `tests/unit/test_room_transition_cmd.c`,
`tests/integration/test_room_transition.c`, `tests/property/prop_door_roundtrip.c`,
`tests/unit/probe_door_1170.c` — sie nageln den heutigen Übergang fest.

---

## 6. Plan

Reihenfolge so, dass jede Stufe für sich abnehmbar ist. Neue Dateien sind als **neu** markiert; an
bestehenden Dateien steht die Stelle, an der später angesetzt würde.

### Stufe A — Modell und Textur, ohne den Port zu berühren

| Schritt | Ort | Inhalt |
|---|---|---|
| A1 | **neu** `re15_port/tools/tor/` (Verzeichnis ist von einer Nachbaruntersuchung schon angelegt) | Geometrie aus den drei Ansichten; Kamera als Parameter, Werte aus 3.4 |
| A2 | **neu**, Export | MD1 mit mehreren Meshes, **nur Dreiecke**, geteilte Listen, Normalen 4096, UV-Wörter `0x7800`/`0x80`; TIM über `tim_bauen()` mit Schwarz → `0x8000`, Aussparung und Restfläche → Index 0 |
| A3 | **neu**, Abnahme | Projektion mit `basis()`/`projiziere()`; Silhouette **nicht** über Differenzbild; Cut 12 über die Torebene, nicht über den Boden |
| A4 | Ausgabe nach `build/tor_1170/` | OBJ + PNG, MD1, TIM, RE1.5-förmiger DO2-Container |

### Stufe B — eingebackenes Modell und Parser-Riegel

| Schritt | Ort | Inhalt |
|---|---|---|
| B1 | **neu** `engine/src/gen/tor_1170.inc` | C-Felder über `carr()` |
| B2 | **neu** `engine/src/…c` + `include/…h` | Auslieferung der Bytes wie `re15_sicherung_md1_bytes` (`sicherung_1150.c:32-42`) |
| B3 | **neu** `tests/unit/probe_tor_1170*.c` + `tests/unit/probes/tor_1170.cmake` | Parser-Riegel aus Abschnitt 5 |

### Stufe C — Türsequenz

| Schritt | Stelle | Inhalt |
|---|---|---|
| C1 | `engine/src/scd_vm.c:3796-3857`, `include/re15_aot.h:126-147` | `pc[26]`, `pc[27]` mitlesen (Beleg `@0x8001720c`, `@0x800164a8`) |
| C2 | **neu**, Tabelle | Tür → Modell; Schlüssel mit Rechteck oder Band, nicht mit Slot (s. 1.4) |
| C3 | `include/re15_room.h:185-191` | Feld „welche Tür" in `re15_room_change_t` |
| C4 | `engine/src/aot_common.c:526` | Sequenz scharfmachen; im Original setzt der Handler an dieser Stelle State 1 und friert ein (`@0x800430bc-e4`) |
| C5 | `platform/pc/main.c:7054`, `engine/src/game_step_common.c:2038`, `platform/psx/main.c:590` | Verbrauch warten lassen |
| C6 | `platform/pc/main.c:9608-9741` | starren MD1-Zeichner als Funktion herauslösen; Textur in Slot 24 |
| C7 | **neu**, Szene | eigene Schleife nach dem Muster `pc_run_player_select` (`main.c:2019`); Kamera (30000,0,0) → (22000,0,0), Bildmitte (160,120), Umgebungslicht 0xFF |
| C8 | `engine/src/scd_vm.c:200` | Handler für Opcode `0x4F` mit eigenem Vier-Objekt-Pool; Felder laut Tabelle in 3.1 |
| C9 | unverändert | `re15_room_transition_present()` folgt nach der Sequenz |

Die Bewegung selbst (wie weit, wie schnell, welcher Ton) kommt aus RE2 und ist Gegenstand der
Nachbaruntersuchungen; RE1.5 liefert dafür nichts (SCD-Block der DO2 = `02 00 01 00`).

---

## 7. OFFEN

1. **Projektionsdistanz H der Türszene.** Im Aufbau `0x800161e0` steht kein Schreiber auf das GTE-Register
   H; `0x80066c40` (gerufen `@0x80016434`) führt über `0x80066cc0` in einen Vektor-Patch, nicht in eine
   H-Zuweisung. Woher H kommt, ist nicht gefunden. Nächster Weg: `0x800166c4` (Zeichnen) und `0x80053ca4`
   lesen, in RE2 dieselbe Stelle suchen.
2. **Lichtquellen der Türszene.** Belegt sind nur Umgebungslicht 0xFF und Objektfarbe `0x808080`. Ob die
   Szene eine Lichtmatrix setzt oder flach zeichnet, ist nicht gelesen. Nächster Weg: `0x800166c4`,
   `0x80025940`, `0x80025a98`.
3. **Obergrenze für MD1 + Skript.** Der Abstand Ladeadresse → Primitiv-Puffer ist 40960 B; ob das Original
   darüber hinaus schreibt, ist nicht geprüft.
4. **Ton und Musik des Zielraums.** `re15_room_apply_pending` startet die Raum-BGM (`room_common.c:416`).
   Ob sie im Original während oder nach der Türszene einsetzt, ist nicht belegt — und entscheidet, ob die
   Sequenz im Port vor oder nach `apply_pending` liegen muss.
5. **Zeigt der Port im Spiel wirklich den Engine-Dekoderstand?** `probe_bg_dump` benutzt laut eigenem Kopf
   „dieselbe Kette, die im Spiel läuft"; eine Aufnahme des Fensters habe ich nicht gemacht.
6. **Die Wartebedingung `aca38 & 0x10000`** (`@0x8001d850-68`). Gesetzt in State 1 `@0x8001ca20-2c`; wer
   das Bit löscht, habe ich nicht gesucht. Für den Port ohne Belang, solange synchron geladen wird.
7. **Wohin das Archiv endgültig gehört.** Abschnitt 4 ist eine Empfehlung mit den Gründen des Repos, keine
   Festlegung.
8. **Nebenbefund an der Sicherung:** 68 von 4096 Texeln des ausgelieferten Modells sind Löcher (2.4).
   Nicht Teil dieses Auftrags, nicht angefasst.
