# RE2-Zombie-KI als wählbare Option (`OPTIONS → AI → RE1.5 | RE2`)

Auftrag (Nutzer, 2026-07-28): *„Ich möchte, das du mir im Hauptmenu unter Option auch eine Option
anbietest, zwischen RE1.5 KI oder RE2 KI auszuwählen … Wenn ich im Optionsmenu für KI RE2 auswähle,
möchte ich, das das Spiel die Resident Evil 2 KI für die Zombies nutzt anstelle der RE1.5 KI."*

**RE1.5 bleibt der Default und bleibt unangetastet.** Nur die Zombie-Typen
`0x10 / 0x11 / 0x12 / 0x13 / 0x16 / 0x18` verlassen überhaupt das RE1.5-Gehirn; Hund, Krähe, Spinne,
Gorilla-Boss und NPCs laufen in beiden Modi auf der byte-true RE1.5-KI (alles andere würde sechs
verifizierte Kampagnen brechen).

## Quelle und Werkzeug

| | |
|---|---|
| Overlay | `info/re2leon/COMMON/BIN/EMZ0.BIN`, lädt **RAW @0x80100000** (kein 0x800-Header) |
| EXE | `info/re2leon/PSX.EXE` (`~0x8001xxxx–0x800Fxxxx`) |
| Disassembler | `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py {dis\|read\|bytes\|table} <addr> <n> [--bin EMZ0.BIN]` |

## Zustandstabelle @0x8010C830 (selbst verifiziert)

| Idx | Adresse | Bedeutung |
|---|---|---|
| 0 | `0x8010065C` | INIT |
| 1 | `0x8010114C` | ACTIVE |
| 2 | `0x80104F40` | HURT |
| 3 | `0x80108250` | DEATH |
| 4 | `0x80065C88` | (EXE-Shared) |
| 5, 6 | `0` | NULL |
| 7 | `0x8010A440` | CORPSE (12 Substates) |
| 8 | `0x80109CFC` | **RE2-exklusiv** (11 Substates) — RE1.5 hat keinen Eintrag 8 |

ACTIVE dispatcht über `(+0x10E & 0x3F)` durch `0x8010C854` in **genau zwei** Varianten:
`0x8010118C` (aufrecht) / `0x80101210` (kriechend). Die aufrechte Variante dispatcht dann auf
`+0x5` (`sub_state_1`) durch **zwei** Tabellen:

* DECISION `@0x8010C88C` (16 Einträge)
* EXECUTOR `@0x8010C8CC` (16 Einträge)

`sub_state_1 = 1` ist WALK → Decision `0x80101714`, Executor `0x80101A40`.

## ⛔ Was „RE2-KI" NICHT ist

Die Folklore („RE2-Zombies sehen dich, RE2-Zombies koordinieren sich") ist **widerlegt** — nicht
wieder einbauen:

* **Kein Line-of-Sight, kein Raycast.** Ein `jal`-Scan über alle 13267 Overlay-Worte findet **null**
  Aufrufe von `0x80050858` / `0x80065518` / `0x80065890`. Die gehören zur em/NPC-Familie.
* **Keine Schwarm-KI.** ACTIVE dispatcht **einmal**; die 14 Einträge von `0x8010C854` alternieren nur
  auf bit0 → exakt zwei Varianten, nicht elf „Modi".
* **`+0x1F4`** wird vom Zombie **gelesen** (`@0x801017A4`), aber nie geschrieben; Produzent ist
  `FUN_80065518` aus der NPC-Familie → in reinen Zombie-Räumen ein toter Zweig.

Der reale Unterschied ist **Lokomotion**, **Angriffs-Arbitrierung** und **Trefferreaktion**.

---

## ✅ W1 — Lokomotion (portiert, byte-true, getestet)

Commit `5f56df41`. Code: `re15_port/engine/src/enemy_ai_re2_zombie.c`,
Test: `re15_port/tests/unit/test_re2_zombie_ai.c` (`unit_re2_zombie_ai`).

### Walk-Turn-Gate `@0x80101BAC–0x80101CAC`

```
80101bb4: sltiu v0,v0,0x1389   ; +0x1F0 Spielerdistanz < 5001 ?
80101bbc: addiu v1,zero,8      ;   JA  -> Rate +8, MONOTON auf den Steuerpunkt
80101bc0: lb    v0,363(s1)     ;   NEIN-> Gait-Zeile +0x16B  (SIGNED lb)
80101bc8: sll   v0,v0,1        ;         FLACHER u16-Index
80101bd4: lhu   v0,[0x8010c924]
80101bdc: srl   v0,v0,15       ;         bit15
80101be8: subu  v1,v1,v0<<4    ;         Rate = 8 - (bit15?16:0) -> +8/-8 = WEAVE
80101bf8/fc: a1/a2 = Steuerpunkt +0x1C4/+0x1C6
80101c00: jal   0x80015558
80101c94: sltiu v0,v0,0xbb8    ; < 3000 ?
80101cac: addiu a3,zero,16     ;   -> ZUSÄTZLICHE Rate-16-Drehung
```

### Gait-Maschine `@0x80101A7C–AC` (Init) / `@0x80101B2C–90` (pro Tick)

```
80101a90/94/9c: Zeile = (rand & 0xf) * 2        -> Startzeile immer GERADE
80101ac0/c8:    Timer = (tbl[row] & 0x7fff) + rand        <- VOLLES Byte beim Init
80101b30/34:    Timer != 0 -> dekrementieren, fertig
80101b40/50/5c: sonst Zeile += 1, Wrap bei 32
80101b80/84/90: Timer = (tbl[row] & 0x7fff) + (rand & 0x1f)   <- MASKIERT beim Reload
```

Tabelle `@0x8010C924`, 32 × u16, byte-verifiziert. **Jede** Zeile packt beides:
`bits 0..14 = Dauer`, `bit15 = Weg-Dreh-Flag`. Ergibt 190/50, 150/64, 110/64, 180/52 … = langes
Zu-Segment im Wechsel mit kurzem Weg-Segment. **Das ist der RE2-Gang.**

### PRNG `@0x80015FE8`, Seed `0xD2706CA4`

```
s=state; h=(s>>7)&0xff; v=((h+s)&0xff)|(h<<8); state=v&0xffff; return v&0xff
```
Seed geschrieben an beiden Writern von `0x800CE318`: `@0x8002B908-1C` und `@0x8003BCB0-C4`
(`lui 0xd270 / ori 0x6ca4`). **Der Seed ist tragend** — aus State 0 wäre der Generator ein Fixpunkt.
Eigener Generator, *nicht* der RE1.5-RNG (der hasht das Argument des Aufrufers; sein State-Store
`@0x800AC774` ist ein Dead Store).

### Der Turn-Helfer ist in beiden Spielen DIESELBE Funktion

RE1.5 `FUN_8001AAC4` (a0=tx a1=tz a2=slew, globales Current-Entity, Pos `+0x34/+0x3c`, Yaw `+0x6a`)
vs. RE2 `FUN_80015558` (a0=actor* a1=tx a2=tz a3=rate, Pos `+0x38/+0x40`, Yaw `+0x76`).
Andere Signatur und Offsets, **identische Mathematik**:

```
bgez rate      -> rate<0: rate=-rate UND bearing+=0x800   (WEG-drehen!)
delta = (rate + bearing - yaw) & 0xfff
delta < 2*rate -> SNAP yaw = bearing              @0x800155cc
delta < 0x801  -> yaw += rate  sonst  yaw -= rate @0x800155d4
```

Die Bearing-Helfer sind ebenfalls byte-gleich (RE2 `FUN_800154AC` vs RE1.5 `FUN_8001A6D4`: gleiche
`0x400/0xc00`-Basis, gleiches `catan((dz<<12)/dx)`, und RE2s `(4096-at)&0xfff` ≡ `(0-at)&0xfff`).
→ Der Port benutzt für RE2 **denselben** `re15_enemy_steer_point` / `re15_atan2_q12`.

### Drei Rate-Defekte, die der eigene Test aufgedeckt hat

1. Eigener Shortest-Arc-Clamp geschrieben — `FUN_80015558` macht etwas anderes (negative Rate =
   180°-gedrehtes Ziel + Snap, siehe oben). Kopie entfernt, Original-Helfer exportiert.
2. Angenommen, die Bearing-Helfer *seien* gleich — nachträglich belegt statt geglaubt.
3. Gait-Zeile aus dem RE1.5-Wander-Index gespeist — spielefremde Annahme. RE2 hat seine **eigene**
   Gait-Maschine, die die Zeile jetzt besitzt.

### Wirkungsnachweis (headless ROOM1140, identisches Spieler-Skript `L14,U200`)

| | Dreh-Ticks | Vorzeichenwechsel | nächster Zombie |
|---|---|---|---|
| RE1.5 | 680 | 28 | **726** |
| RE2 | 599 | 55 | **122** |

RE2 zeigt 45× den Drehbetrag **24** (= 8 + 16, die Nah-Drehung); RE1.5 kein einziges Mal in dieser
Häufung, dafür 259×/238× die Dauer-Slews 10/15 = der „betrunkene" Gang.

Messen mit: `RE15_AI_FLAVOR=re2` (headless-Umschalter, umgeht das Menü).

### Noch nicht byte-true (markiert, nicht gefüllt)

* **Steuerpunkt `+0x1C4/+0x1C6`** kommt aus dem RE2-Navigator `FUN_8004A808` (`@0x80100354`, nicht
  portiert) → es wird die **Spielerposition** benutzt. Auf freier Fläche deckungsgleich, um
  Ecken/Hindernisse nicht.
* `+0x16A = (rand & 0x1f) + 30` (`@0x80101AD4-E8`) und `+0x14D = (rand & 0x1f)` (`@0x80101B04-0C`)
  werden gesetzt, ihre Konsumenten sind noch nicht identifiziert → nicht portiert.

---

## 🔜 W2 — Angriffs-Arbitrierung (Decision `@0x80101714`) — ANGEFANGEN, NICHT PORTIERT

Der Anfang der Entscheidungsleiter ist gelesen; der Rest läuft gerade als Audit-Workflow
(`wf_a22318c3-be8`). Was bisher aus den Bytes steht:

```
80101830: lh    v0,468(s0)      ; +0x1D4
80101838: andi  v0,v0,0xc000    ; bits 14|15
8010183c: beq   v0,zero,0x80101860
80101844: lw    v0,272(s0)      ; +0x110
8010184c: andi  v0,v0,0x1
80101854: addiu v0,zero,2561    ; 0x0A01 -> state=1 sub1=10
80101858: sw    v0,4(s0)
8010185c: sltiu v0,s2,0xdac     ; Distanz < 3500
80101874: lhu   v0,[0x800cfbf6] ; globaler Zustand, & 0x15  (muss != 0 sein)
80101888: jal   0x80015fe8      ; rand, verlangt (rand & 3) == 0   = 1-von-4
8010189c: addiu v0,zero,3073    ; 0x0C01 -> sub1=12
801018a0: sw    v0,4(s0)
801018a4: sltiu v0,s2,0x9c4     ; Distanz < 2500
801018c4: andi  v0,v0,0x17      ; weiterer globaler Filter
801018d0: jal   0x80015fe8      ; rand, & 1
```

Also: **gestaffelte Distanzbänder (3500 / 2500) mit Wahrscheinlichkeits-Gates**, die verschiedene
Angriffs-Substates wählen. Genau das liest sich im Spiel als „der greift entschlossener an".

Offen und **nicht** zu raten: was `s2`/`s4` beim Eintritt sind (s2 vermutlich die Distanz — beweisen),
was `0x800CFBF6` ist und was die Bits `0x15`/`0x17` bedeuten, und was die Substates 10/12 tun
(Executor `0x8010417C` bzw. `0x80104748`).

## 🔜 W3+ — Trefferreaktion (HURT `@0x80104F40`), Kriech-Variante (`@0x80101210`), State 8

Noch nicht angefasst. Die Vermutung „Flinch-Cooldown bei `+0x223`" ist ein **unbelegtes Gerücht** und
muss aus den Instruktionen bewiesen oder verworfen werden.

---

## UI

`OPTIONS`-Panel ist gegen die PSX **pixel-verifiziert** und wird nicht angefasst: es endet bei
x=296 (EXIT = 248+48), ein 4. Tab auf der Tab-Zeile hing halb außerhalb (`shots/_aitab_top2.png`).
Die Erweiterung liegt deshalb im freien Streifen **unter** der unteren Box (die bei y=220 endet):
`"AI  <flavor>"` bei (14,225), mit demselben blauen 50%-ABE-Cursor-Tile wie die byte-true Tabs.

Prüfen am **echten** Fenster (nicht Autoshot, siehe Skill `re15-port-visual-verify`):

```bash
RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_CONFIG_TEST=1 RE15_CONFIG_TAB=8 ./re15_pc.exe &
sleep 22
ffmpeg -y -f gdigrab -i "title=RE1.5 Rebuilt — PC" -frames:v 1 shot.png
```

`RE15_CONFIG_TAB` wirkt jetzt auch auf den **interaktiven** Options-Loop — SDL sieht kein SendKeys,
die Navigation wäre am echten Fenster sonst nicht prüfbar.

Belege: `shots/_ai_top.png`, `shots/_ai_aiscreen.png`, `shots/_ai_re2sel.png`.
