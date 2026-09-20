# Befund "waffen-loop" — Mehrfachschuss-Pistole: Kugel-Animation und Schuss-Sound hängen im Loop

Runde 17, 2026-09-20. Basis master (v0.8.6). Worktree `wf_2d0f494e-255-1`.

Nutzer-Report (zwei Punkte, **eine** Ursache):

1. „nehme ich die mehrfach schuss Pistole mit in stage 1, bleibt dort die Kugel Animation und
   Sound im loop hängen."
2. „Ausserdem bleibt genau in diesem Raum der Sound hängen."

---

## 0. Kurzfassung

Die „Mehrfachschuss-Pistole" ist die **Beretta M93R (Inventar-Id 5)** bzw. die **Glock 18 (Id 6)**.
Sie sind die einzigen zwei Waffen, deren Entlade-Handler `0x800338A8` seine drei Effekte mit
**FX-SUB 2** anstößt (`@0x800338ac/f4/28`). Genau diese drei Row-Ketten in `CORE00.ESP`
(id2 sub2 @0x1188, id3 sub2 @0x44c, id4 sub2 @0x1988) sind die **einzigen Routine-15-Ketten der
ganzen Datei** — der Drei-Schuss-Burst: drei Rows, jede spawnt ein Kind (Mündungsfeuer / Rauch /
Hülse), zwei Ticks auseinander.

Die letzte Row beendet sich im Original mit `80017b6c sb zero,108(v1)` — sie schreibt das
**Flags-Byte `slot+0x6c` auf 0**. Dieses Byte **ist** im Original zugleich die Belegung des
Effekt-Platzes: der Spawner vergibt genau so einen Platz neu (`800197a8 lbu` / `800197b0 beq`),
und beide Tick-Schleifen überspringen ihn (`80019e70-7c`, `80019f44-50`).

Der Port führt die Belegung in einem **eigenen** Feld (`re15_esp_fx_t.active`). `f->flags = 0`
ließ den Platz deshalb ewig im Dispatch stehen. Routine 15 setzte im nächsten Bild die Flags
wieder auf 0x65, fand den Zähler `row[0x0e]` weiterhin auf 0 — und spawnte **in jedem Bild ein
neues Kind**, bis der 96er-Pool voll war und voll blieb. Jedes Mündungs-Kind trägt in seiner
Row-Kette Routine 9 = den positionsgebundenen **KNALL** (ARMS-Record 0), und der liegt byte-true
auf einer **festen Stimme** — der Schuss-Sound wurde also mehrmals pro Bild neu gekeyt und
endete nie. Der Effekt-Pool wird nur beim **Raumladen** gewischt (`scd_room_setup.c:195`) —
deshalb „genau in diesem Raum".

Fix: `esp_fx_kill()` in `re15_esp.c` zieht die Port-Belegung beim byte-true `flags := 0` mit,
dazu das Lebend-Gate des Flags-Bytes im Tick (nur für Row-VM-Plätze). Gepinnt durch
`unit_r17_waffen_loop_pin`. Suite **322/322 grün**.

---

## 1. Reproduktion und Messung

Messlauf im echten Spiel (`re15_pc.exe` des Worktrees), Waffe 5 im Inventar, Sprung nach
ROOM1140, Abzug 4 s gehalten, danach 12 s nichts:

```
RE15_GIVE="5:15,21:50" RE15_EQUIP=5 RE15_DEBUG_JUMP=1140@250
RE15_INPUT_SCRIPT="W2,MA4,M2,W12" RE15_INPUT_SCRIPT_START=200
RE15_WAFFEN_LOG=wf.log RE15_FRAMEDUMP="40-360/80:vor_"
```

Neue Mess-Werkzeuge dieser Runde (reiner Debug-Harness, kein Spielverhalten):

* `RE15_GIVE="<item>:<menge>,…"` (`platform/pc/main.c`) — legt Gegenstände in freie
  Inventarplätze. `RE15_EQUIP` allein setzt nur `DAT_800aca5d`; ohne Platz ist das Magazin −1
  und der Feuer-Pfad gesperrt.
* `RE15_WAFFEN_LOG=<datei>` (`re15_waffen_log()` in `player_common.c`) — je Bild
  Waffe/Clip/Clip-Länge/Bild/Phase/Rückstoß/Feuertakt/Magazin/Effekt-Plätze, dazu **jeder**
  Effekt-Spawn (`re15_esp.c`) und **jeder** Waffen-SE (`audio_pc.c`).

### 1.1 Gemessen — VORHER

```
F15  … clip=8  rec=0 fx=0     SPAWN id=2 sub=2 / id=3 sub=2 / id=4 sub=2   <- ein Schuss
F16  … clip=7  rec=1 fx=33    SE arms_rec=0
F18  …                        SPAWN id=2 sub=0 / id=3 sub=0 / id=4 sub=0
F19  … fx=37                  SE arms_rec=0
F22  …                        SPAWN …   SE arms_rec=0
F23  …                        SPAWN …   SE arms_rec=0
…  (ab hier JEDES Bild)
F428 … pad=0000 mag=0 fx=94
F432 … pad=0000 mag=0 fx=96   <- Pool VOLL, Abzug seit ~300 Bildern los, Magazin leer
```

* Kind-Spawns `id3 sub0` im Lauf: **100**
* Knall-SE (`ARMS`-Record 0): **946 Aufrufe**, bis zu **8 pro Bild**, letzter bei F432 — also
  bis zum Ende des Laufs, lange nach Abzug-Loslassen und leerem Magazin.
* `fx` (belegte Effekt-Plätze): steigt monoton auf **96 von 96** und bleibt dort.

### 1.2 Gemessen — NACHHER

```
F15  …  SPAWN id=2 sub=2 / id=3 sub=2 / id=4 sub=2
        SPAWN id=2 sub=0 / id=3 sub=0 / id=4 sub=0     <- Schuss 1 des Bursts
F18  …  SPAWN id=2 sub=0 / id=3 sub=0 / id=4 sub=0     <- Schuss 2
F21  …  SPAWN id=2 sub=0 / id=3 sub=0 / id=4 sub=0     <- Schuss 3, danach Schluss
…
F510 …  pad=0000 mag=0 fx=0
```

* Kind-Spawns `id3 sub0`: **15** = 5 Abzugszyklen × **3 Schuss Burst**.
* Knall-SE: **30 Aufrufe**, letzter bei **F140** — Ende mit dem letzten Schuss.
* `fx` fällt nach dem Feuern auf **0** und bleibt dort.

### 1.3 Sichtprüfung (selbst angesehen)

Bilder in `analysis/befunde_2026-09-20/waffen-loop/`, Ausschnitt x100..180 / y60..125,
5-fach vergrößert:

* `lupe_vor_000360.png` — eine Wolke eingefrorener heller Sprites hängt neben Leon in der Luft.
  Bild 360, Abzug seit ~220 Bildern los, Magazin leer.
* `lupe_nach_000360.png` — derselbe Ausschnitt, **leer**.

Pixel-Differenz vor/nach (`vor_*.png` vs. `nach_*.png`, volles Bild):
F40 183 px, F120 181 px, F200 121 px, F280 112 px, F360 119 px — die Abweichung verschwindet
nicht, weil die Wolke im „vor"-Bau dauerhaft steht.

---

## 2. Original-Mechanismus (alle Adressen selbst disassembliert, `info/Re1.5/PSX.EXE`)

### 2.1 Welche Waffe — die Zuordnung

`inv_name_bank.inc` (aus `BIN/DEBUG.BIN`, Namenstabelle `@0x800c495c`): Id 5 = *Beretta M93R*,
Id 6 = *Glock 18* — die beiden Mehrfachschuss-Handfeuerwaffen. Ihr Entlade-Handler ist
`0x800338A8` (Tabelle `@0x80074100`): 1× `FUN_80011f50` `@0x8003396c`, **3×** `FUN_8004eae4`
`@0x80033974/7c/84` (drei Patronen je Abzug) und drei Effekt-Spawns mit **Sub 2** statt 0
(`@0x800338ac/f4/28`). Keine andere Waffe benutzt Sub 2.

### 2.2 Die drei Sub-2-Ketten in CORE00.ESP (Datei selbst geparst)

| Effekt | Rowblock | Basis | Rows | A je Row | `row[0x0e]` | `row[0x16]` | `row[0x26]` |
|---|---|---|---|---|---|---|---|
| id2 Mündungsfeuer sub2 | 0x0fc8 | 0x1188 | 3 | 15,15,15 | 0, 2, 2 | 0x200 | 1, 1, 0 |
| id3 Rauch sub2 | 0x0384 | 0x044c | 3 | 15,15,15 | 0, 2, 2 | 0x300 | 1, 1, 0 |
| id4 Hülse sub2 | 0x18c0 | 0x1988 | 3 | 15,15,15 | 0, 2, 2 | 0x400 | 1, 1, 0 |

`row[0x16]` ist der Kind-Code: `row[0x17]<<24 | row[0x16]<<16 | scale16`, also id2/3/4 **sub 0** —
die normalen Einzelschuss-Effekte. Drei Rows = drei Kinder = der Drei-Schuss-Burst. Ein Scan über
alle Ids und Subs der Datei findet **keine weitere** Routine-15-Kette.

### 2.3 Routine 15 `@0x80017ac8` (Dispatch-Tabelle `@0x80071d40[15]`)

```
80017adc  sb   v1,108(v0)        ; slot+0x6c (Flags) := 0x65
80017aec  lhu  v0,14(a3)         ; row[0x0e]
80017af4  beq  v0,zero,80017b10  ; == 0 -> spawnen
80017b04  addiu v0,v0,-1         ; sonst herunterzaehlen
80017b0c  sh   v0,14(a3)         ;   und RETURN
80017b38  jal  0x800199d4        ; Kind spawnen (Code aus row[0x17]/row[0x16], scale slot+0x72)
80017b4c  lhu  v0,38(v1)         ; row[0x26]
80017b54  beq  v0,zero,80017b6c
80017b5c  jal  0x800174e4        ;   != 0 -> Row-Advance
80017b6c  sb   zero,108(v1)      ;   == 0 -> Flags-Byte := 0
```

### 2.4 Das Flags-Byte IST die Belegung

```
Spawner FUN_80019700:
  800197a8  lbu  v0,108(t0)
  800197b0  beq  v0,zero,0x800197d0   ; Flags==0  ->  dieser Platz wird NEU vergeben
  800197c4  addiu v1,t0,132           ; Schrittweite 132, 0x60=96 Plaetze (sltiu @0x8001978c)

Tick-Schleife 1 (Routine-A-Dispatch) FUN_80019e20:
  80019e70  lbu  v0,108(v1)
  80019e78  andi v0,v0,0x1
  80019e7c  beq  v0,zero,0x80019ea4   ; bit0==0 -> Dispatch uebersprungen

Tick-Hauptschleife:
  80019f44  lbu  v1,108(a3)
  80019f4c  andi v0,v1,0x1
  80019f50  beq  v0,zero,0x8001a480   ; bit0==0 -> ganzer Slot-Rumpf aus
```

Im Original ist der Platz nach `80017b6c` also tot **und** wieder frei.

### 2.5 Warum der Sound mithängt

Das Kind `id2 sub0` (Mündungsfeuer) ist eine Kette `A=8` → **`A=9`** → `A=0`; Routine 9 ist der
positionsgebundene KNALL = `FUN_80045024(0x01000001)` = ARMS-Record 0. Im Port hängt er an
`re15_esp_bang_hook` → `pc_bang` → `re15_audio_weapon_se(0)` (`audio_pc.c:1369`). Der Record legt
die **Stimme fest** (`voice = (byte3 & 0x1f) - 0x10`, `@0x80045478-7c`) — ein alle ein bis drei
Bilder neu gekeyter Schuss auf derselben Stimme ist genau das, was der Nutzer als „der Sound
bleibt hängen" hört. Eine zweite, unabhängige Ursache habe ich nicht gefunden.

---

## 3. Port-Ist (vor dem Fix)

* `re15_port/engine/src/re15_esp.c:568` (alt) — `else f->flags = 0;` in `case 15`.
  Kein Gegenstück zu `active`.
* `re15_port/engine/src/re15_esp.c:858` (alt) — `if (!f->active) continue;` ist die **einzige**
  Lebend-Prüfung der Tick-Schleife; das Flags-Byte wird dort nicht gelesen.
* `re15_port/engine/src/re15_esp.c:745` — `re15_esp_fx_spawn_rows` sucht den freien Platz über
  `re15_esp_fx_spawn_ex`, und der prüft `f->active` (`re15_esp.c:775`), nicht `f->flags`.
* Folge: `row[0x0e]` bleibt auf 0, `row[0x26] == 0` → jeder Tick spawnt ein weiteres Kind.
  Pool 96 (`RE15_ESP_FX_MAX`, `re15_esp.h:145`) läuft voll und bleibt voll, bis
  `re15_esp_fx_reset()` beim Raumladen greift (`scd_room_setup.c:195`, `room_pc.c:130`).

---

## 4. Umsetzung

`re15_port/engine/src/re15_esp.c`

* Neu `esp_fx_kill()`: setzt byte-true `f->flags = 0` (`@0x80017b6c`) **und** zieht die
  Port-Belegung `f->active = 0` mit — im Original ist das dasselbe Byte (`@0x800197a8-b0`).
* `case 15` ruft im Abschlusszweig `esp_fx_kill(f)` statt `f->flags = 0`.
* `re15_esp_fx_tick()`: Lebend-Gate `if (!(f->flags & 0x01)) { f->active = 0; continue; }`
  für Row-VM-Plätze (`@0x80019e70-7c` / `@0x80019f44-50`). Nur für Plätze mit `rows_base` —
  die tragen, wie im Original, immer ein datengetriebenes Flags-Byte; die row-losen
  Port-Spawns (z. B. der Speedloader-Auswurf) bleiben unberührt.

Mess-Werkzeug (kein Spielverhalten): `RE15_GIVE` in `platform/pc/main.c`, `re15_waffen_log()` +
`re15_player_aim_dbg()` in `engine/src/player_common.c`, je ein Log-Aufruf in
`re15_esp_fx_spawn_rows` (`re15_esp.c`), `re15_audio_weapon_se` (`platform/pc/src/audio_pc.c`)
und im Feuer-Pfad (`engine/src/game_step_common.c`).

### 4.1 Pin

`tests/unit/probe_r17_waffen_loop.c`, registriert in
`tests/unit/probes/r17_waffen-loop.cmake` als `unit_r17_waffen_loop_pin`:

1. jede der drei Sub-2-Ketten spawnt **genau 3** Kind-Auslösungen (× Ströme des Kind-Effekts:
   Mündungsfeuer führt 2, Rauch und Hülse je 1),
2. der Eltern-Platz wird wieder frei,
3. 300 Bilder nach einem Schuss ist der Effekt-Pool **leer** und lief nie voll.

Gegenprobe mit ausgebautem Fix (beide Zeilen auskommentiert, neu gebaut):

```
id2 sub2: Kind-Plaetze 14 (SOLL 6)
id3 sub2: Kind-Plaetze 25 (SOLL 3)
id4 sub2: Kind-Plaetze 56 (SOLL 3), Eltern-Platz wurde NIE frei
Ein Schuss, 300 Bilder Nachlauf: Rest 18 belegt (SOLL 0)
FEHLER (5 Befunde)
```

Mit Fix: `OK (0 Befunde)`.

Suite: `322/322 Tests grün` (321 vorher + dieser Pin), 196 s.

---

## 5. Offen

* **Kein zweiter Befund zum Sound.** Punkt 2 des Reports („genau in diesem Raum bleibt der Sound
  hängen") wird durch dieselbe Ursache vollständig erklärt: 946 Knall-Aufrufe auf einer festen
  Stimme, der Pool wird erst beim Raumwechsel gewischt. Ich habe daneben den Stimmen-/
  Prioritätspfad (`se_play_layers`, `audio_pc.c:684`) gelesen und keinen eigenen Fehler gefunden —
  wenn der Nutzer den hängenden Sound auch ohne Schuss hört, ist das ein **eigener, noch offener**
  Befund und braucht einen Raum-Namen.
* Der Messlauf hat den ARMS-Bank-Ladepfad nicht mit erfasst: im Harness stand
  `bank=-1 (geladen=0)`, weil `RE15_EQUIP`/`RE15_GIVE` `re15_audio_prime_weapon()` nicht rufen.
  Im Spiel lädt das der Menü-Equip. Für einen Messlauf **mit** Ton müsste der Harness die Bank
  mit anstoßen — nicht Teil dieses Befundes.
* Die Sichtprüfung lief in ROOM1140 Kamera 0; dort verdecken 104 Vordergrund-Masken den Spieler,
  die Effekt-Wolke steht rechts daneben frei. Ein Raum mit freiem Blick auf den Spieler wäre für
  künftige Effekt-Befunde die bessere Bühne.
