# BEFUND — „torso-schrot": Kann die Schrotflinte in RE2 den Oberkoerper abschiessen?

Nutzer 2026-09-12: „Es sieht so aus, als koennte die Schrotflinte den Oberkoerper noch
nicht abschiessen von Zombies, was im Original Resident Evil 2 geht."

Analyse 2026-09-12, alle Dumps frisch gezogen: `ghidra_re2_Leon.txt` (RE2 PSX.EXE),
`re2_disasm.py dis/read … --bin EMZ0.BIN` (info/re2leon/COMMON/BIN, Overlay @0x80100000),
Port `re15_port/engine/src/enemy_ai_re2_zombie.c` + `re15_damage.c`, RDT-Scan
`re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`. Vorbefunde gelesen:
schrot_befunde/abtrennen.md, befunde_runde4/gore-vollausbau.md, befunde_runde5/kopf-flug.md
(paralleles Dossier, gleiche Tabellen-Dumps — deckungsgleich).

## Kurzfassung

**Die Port-Zuordnung W8 → Zeile 7 ist byte-richtig, und Zeile 7 zerreisst auch im
RE2-Original NIE den Rumpf.** Leons NORMALE Shotgun (Remington M870) traegt in RE2 die
Waffen-Id **7**; die Todeszeile ist die Item-Id selbst (Beleg-Kette unten). DEATH-Zeile 7
enthaelt in keiner der 9 Spalten die Zerreiss-Zelle 0x80108BEC. Was der Nutzer aus RE2
erinnert („Shotgun zerreisst den Oberkoerper") ist die **CUSTOM Shotgun (Id 8)** — im
RE2-Spielverlauf dieselbe M870, mid-game per Shotgun-Parts aufgeruestet; ihre Zeile 8
Spalte 1 ist 0x80108BEC (voller Zerreiss mit Rumpf-Stumpf). Das RE1.5-Gegenstueck der
staerkeren Schrotflinte ist die **SPAS-12 (W13) → Zeile 8** — im Port fertig verdrahtet
und im Spiel erreichbar (Pickup in ROOM2030). Kein Mapping-Fix noetig; Liefergegenstand
ist die Spielanleitung + optional ein Hinweis-Kommentar und ein Fixture-Pin.

---

## 1. Frage 1 — Waffen-Id der normalen Shotgun + Produzent der Zeile

### 1.1 Die Zeile (+0x5 des Opfers) = die ITEM-ID der gefuehrten Waffe

Hitscan-Pfad (Spieler-Schuss), Aufrufkette selbst nachgelesen (ghidra_re2_Leon.txt):

```
80048444  lhu  a1,0x10e(s2)          ; Spieler +0x10E = Item-Id der ausgeruesteten Waffe
8004844c  andi a1,a1,0xfff           ;   (Erzeuger-Kette: 0x800D5BFA @0x8006b09c ->
80048450  addiu a1,a1,-0x1           ;    sh v0,270 @0x8003bd4c, s. re15_damage.c:1848ff)
80048454  sll  a1,a1,0x10            ; a1.hi = Item-Id - 1
80048458  or   a1,a1,s1
8004845c  jal  FUN_800410cc          ; der Gewehr-Applier
80048460  _ori a1,a1,0x8000
```

Im Applier `FUN_800410cc` (Damage `sw 2,4` @0x800418ec / HP<0 `sw 3,4` @0x800418f8-fc):

```
80041aa0  lw   t0,local_70(sp)       ; = a1
80041aa8  srl  v1,t0,0x10            ; Item-Id - 1
80041ab0  addiu v1,v1,0x1            ; + 1
80041ab4  sb   v1,0x5(t0=Opfer)      ; +0x5 (Zeile) = ITEM-ID
```

AoE-Applier `FUN_800470c0` analog: `sb s5,0x5(s1)` @0x80047324 (Zeile = Attack-Code&0xFF);
HURT/DEATH-Wort `sw 2,4` @0x80047288 / `sw 3,4` @0x8004728c-90; Spalte
`+0x1D2 = zone + 3*bracket` @0x80047310-30 bzw. @0x80041a8c-9c.

Nebenfund (stuetzt die Kette): der Applier remappt die Waffe mit `(a1>>16)==0x12`
(Item-Id 19) per RNG auf 0x50000 (→ Zeile 6, Magnum-Record -0x118) oder 0x20000
(→ Zeile 3, Pistolen-Record -0x154) @0x8004184c-b0 — Zeile und Schadensrecord laufen
beide ueber die Item-Id.

### 1.2 Die Id-Anker (Zeile ≙ RE2-Item-Id, selbst disassembliert)

Die Zeilen tragen ihre Waffen-Identitaet in den Zellen-Extras — alle in EMZ0.BIN geprueft:
Zeile 10 → Brand-Emitter `jal 0x80106128` @0x80108708, Zeile 11 → Saeure `jal 0x80106310`
@0x80108720, Zeile 14 → Spark `jal 0x80106510` @0x801087d4, Zeile 16 → Brand @0x801087ec-,
Zeilen 9/17 → Russ `jal 0x8010640c` @0x801087bc (alles in FUN_80108530); Zeile 12 wird in
66FC/9610 als Bowgun-Einzelbolzen gesondert behandelt (`beq v1,12` @0x801096e0);
Zeilen 5/6 → 0x801092C4 Kopf-Platzer (Magnum). Das ist exakt die kanonische RE2-Item-Folge
…5 Magnum, 6 Custom Magnum, **7 Shotgun, 8 Custom Shotgun**, 9/10/11 GL, 12 Bowgun,
13 Colt S.A.A., 14 Sparkshot, 15 SMG, 16 Flammenwerfer, 17 Rakete, 18 Gatling.
**Leons normale Shotgun = Id 7 → DEATH-Zeile 7. Die Custom Shotgun = Id 8 → Zeile 8.**

## 2. Frage 2 — DEATH-Tabelle @0x8010CC24, Zeilen 7/8 (eigener frischer Dump)

`EMZ0.BIN`, Basis 0x8010CC24, 36 B/Zeile, 9 Spalten (zone + 3*bracket):

| Zeile | col0 | col1 | col2 | col3 | col4 | col5 | col6 | col7 | col8 |
|---|---|---|---|---|---|---|---|---|---|
| 7 (Shotgun) @0x8010CD20 | 80107438 | **801066FC** | 0 | 80107438 | 80108530 | 0 | 80107438 | 80108530 | 0 |
| 8 (Custom) @0x8010CD44 | 80107438 | **80108BEC** | 0 | 80107438 | 80109610 | 0 | 80107438 | 80108530 | 0 |

* **Zeile 7 col1 = 0x801066FC bestaetigt** (Ragdoll-Zelle, kein Zerreiss). KEINE Zelle der
  Zeile 7 zeigt auf 0x80108BEC. (Der Vorbefund „col1 = 66FC" war korrekt dekodiert.)
* 0x80108BEC (Zerreiss) steht nur in Zeile 8 col1, Zeile 9 col1/col2, Zeile 17 col1
  (abtrennen §2.1, hier gegengeprueft).
* col4/col7 der Zeile 7 = **0x80108530** = schlichter Umfall-Tod (frisch disassembliert:
  Fall-Clip 1/2, Blut 8096/6096, SE 11/13, per-Zeile Brand/Saeure/Spark-Extras — KEINE
  Part-Stores). Mittel-/Fern-Bracket, im Port ohnehin unerreichbar (Bracket fest 0).

### 2.1 Was Zeile 7 col1 (66FC) beim TODES-Schuss wirklich tut (frisch disassembliert)

P0 @0x8010679C: `lw 0x800CFBF8 / bgez` — Spieler zielt HOCH → `+0x231=2` @0x801067b0 →
**FUN_801092C4 = Kopf-Explosion** @0x801067bc. Sonst `rand&3 != 0` UND `+0x4==3`
(@0x801067fc-810) → `+0x231=1` → **FUN_80109610 Burst** (3/4). Sonst (1/4) Ragdoll-Wurf
(Clip 3/4, Brocken-Spawns 0x08000000er/0x09020000er @0x801068c8-948) → P2 @0x80106A8C:
`HP=10` @0x80106ae4-e8, `+0x10E=0x2001` @0x80106b0c-10 = **Wiederbelebung als KRIECHER**.

**FUN_80109610 (Burst) ruehrt die PARTS NICHT an** — vollstaendig disassembliert
(0x80109610-0x801099E0): P0 setzt nur `+0x1C0|=1` / `+0x1D3|=0x80` @0x801096a0-ac,
spawnt Blut 8096/6096 (Part 0, ofs -300/-100) und `(rand&3)+1` Fleischbrocken
`0x09020000|(r*4+1536)` @0x8010973c-b8, waehlt Flug-Clip 1/2 ab Frame 15 (`0xF0F00+…`
@0x80109840-64), SE 11/13, Schub 400 @0x80109888. Es gibt **keinen einzigen part_mesh-
oder Part-Flag-Store** — der Koerper bleibt INTAKT und endet als CORPSE (`sw 7,4`
@0x801099b0). Auch der Kriecher-Uebergang @0x80106a8c-b58 stempelt keine Parts (nur
Hitbox 200/±350 @0x80106b14-34 + Clip 5). D. h.: **auch im RE2-Original zeigt die normale
Shotgun beim Rumpf-Kill keinen Torso-Abriss** — Burst-Explosion mit fliegenden Brocken
(3/4), Kriecher-Wiederbelebung (1/4) oder Kopf-Platzer (hoch gezielt). Der Befund
„Burst zerlegt den Torso nicht sichtbar" ist RE2-authentisch, kein Port-Defekt.

## 3. Frage 3 — Port-Vergleich (re2z_row_from_weapon :3648-3653)

* `/* 8*/ 7` — RE1.5-W8 (REMINGTON M870, Item 0x08, s_item_names inventory_common.c:250)
  → Zeile 7. **IDENTITAET**: RE2-Id 7 IST die Remington M870. Byte-richtig.
* `/*13*/ 8` — RE1.5-W13 (SPAS-12) → Zeile 8 (Custom Shotgun, Schaden 300/80/60).
  Klassen-Identitaet „staerkere Schrotflinte". Byte-sinnig.
* Der Zerreiss-Pfad ist im Port KOMPLETT: Zeile 8 col1 → `RE2ZD_8BEC` → `re2z_death_rip`
  (:6955ff Dispatch, :5321ff Zelle) inkl. Zielen-HOCH-Uebersprung des Burst-Ausstiegs
  (:5342-49, `lw 0x800CFBF8/bltz` @0x80108C94-A0) und Rumpf-Stumpf `part_mesh[0]=16`
  (@0x80108FB8-FDC, Gore-Seitenbank md1_gore seit gore-vollausbau). Spalten-Stempel seit
  2026-09-12 korrekt: HOCH/EBEN → col 1, TIEF/liegend → col 0 (re15_damage.c:1856-70) —
  der frühere HOCH→col2-NULL-Defekt (abtrennen §1.3) ist behoben.
* FX-Dekoder live (re2z_gore_fx_ex :1033-1054: 8→Raum-Id 5, 9→Raum-Id 7, 5→CORE00-8) —
  die Burst-Brocken der W8 sind sichtbar.
* SPAS-Erreichbarkeit im Spiel: RDT-Scan (Item-AOT op 0x50, Kurzform item=pc[14]):
  **ROOM2030.RDT/ROOM2031.RDT @0x1bd6: Item 0x0D (SPAS-12) x12**; M870: ROOM10F0/1 @0x103e
  Item 0x08 x7. Die SPAS ist also regulaer aufsammelbar (STAGE2).

## 4. FAZIT + Patch-Plan

**W8→7 ist byte-richtig; Zeile 7 war korrekt dekodiert; das RE2-Original kann mit der
NORMALEN Shotgun keinen Rumpf abreissen.** Der vom Nutzer erinnerte Torso-Abriss ist die
Custom Shotgun (Id 8) — deren RE1.5-Pendant W13/SPAS-12 im Port bereits auf Zeile 8 liegt.

1. **KEINE Aenderung** an `re2z_row_from_weapon` (enemy_ai_re2_zombie.c:3648-3653).
   Ein Umbiegen W8→8 waere ein Mod des RE2-Verhaltens (nur auf ausdrueckliche
   Nutzer-Entscheidung, dann :3650 `/* 8*/ 7` → `8` mit [PORT-ZUORDNUNG]-Marker).
2. **Nutzer-Rezept** (Antwort statt Code): Torso-Abriss = **SPAS-12 (W13, Pickup ROOM2030)
   fuehren, nah, Rumpf/eben oder hoch zielen, Todesschuss** — hoch = deterministisch
   (Gate :5342-49), eben = 1/4 (3/4 Burst wie im Original). Mit der M870 (W8): hoch = Kopf-
   Explosion, eben = Burst/Kriecher — exakt RE2.
3. **Doku-Kommentar** (eine Zeile, kein Verhalten): enemy_ai_re2_zombie.c:3612 (W8-Zeile
   im Mapping-Block) ergaenzen: „Zeile 7 zerreisst NIE (DEATH @0x8010CD20 ohne 8BEC-Zelle,
   eigener Dump 2026-09-12); Torso-Abriss = Zeile 8 = W13".
4. **Fixture-Pin** (falls noch nicht vorhanden, tests/unit, Stil test_re2_zombie_teardeath):
   W13 + aim-up + Rumpf-Kill → Dispatch landet in `re2z_death_rip` und stempelt
   `re2z_part_mesh[0]=16` + `part_flags[8]|=0x4A`; Gegenpin: W8 + aim-up + Rumpf-Kill →
   `re2z_death_magnum` (Kopf), NIE `re2z_death_rip`. Sichert die Zuordnung gegen kuenftige
   „Fixes" ab.
5. OFFEN (unabhaengig, kopf-flug.md): der Bit-0x08-Scherben-Zeichner — betrifft die
   OPTIK des 8BEC/92C4-Kopf-Flugs, nicht die Zeilen-Zuordnung.
