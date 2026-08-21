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

### Dritter geteilter Helfer: der KEGEL-TEST

RE2 `FUN_80015614` == RE1.5 `FUN_8001A9CC` == Port `re15_ai_arc_test` (in `re15_damage.c`).

```
bearing = FUN_800154AC(self+0x38, self+0x40, tx, tz)
t       = (bearing - yaw@+0x76 + half) & 0xfff       @0x80015650/54/58
if (t <  2*half) return 0;      @0x80015664/68/6c (+ Delay-Slot v0=0)   -> INNERHALB
if (t <  0x801)  return +half;  @0x80015674/78     (+ Delay-Slot v0=half)
else             return -half;  @0x80015680
```
RE1.5 dreht nur die Reihenfolge der beiden Vorzeichen-Instruktionen um (`subu` im Delay-Slot
@0x8001AA40, `addu` am Sprungziel @0x8001AA50) — **gleiche** Semantik. Der Port-Code stimmt Zeile
für Zeile: `u < 2*cone -> 0`, sonst `±cone` mit dem Umschlag bei `u > 0x800`.

→ **Muster (dreimal in Folge bestätigt): bevor ich eine RE2-Hilfsfunktion nachbaue, prüfe ich, ob
der Port sie über die RE1.5-Seite schon byte-true hat.** Bisher: Steer-Helfer, Bearing-Helfer,
Kegel-Test — alle drei schon vorhanden.

## ✅ W2 — Angriffs-Arbitrierung `@0x80101714` — VOLLSTÄNDIG SPEZIFIZIERT, BEWUSST NICHT PORTIERT

Zwei Workflows (`wf_a22318c3-be8` Fund + adversariale Gegenprüfung, `wf_16a84529-3e0` Adjudikation).
**Von 65 Behauptungen der ersten Runde überlebten nur 12** — fast alle Widerlegungen betrafen nicht
das Transkript, sondern die *Deutung* (siehe „Widerlegte Etiketten" unten). Die folgende Leiter habe
ich danach **selbst** nachdisassembliert; die mit ✔ markierten Punkte sind von mir persönlich
gegengeprüft, nicht bloß von Agenten berichtet.

### Aufruf-Kontext

`0x8010118C`: `lbu +5` → DECISION[`0x8010C88C`] → **`lbu +5` erneut** → EXECUTOR[`0x8010C8CC`]
(@0x801011A8-EC). Ein Schreiber auf `+0x4` lässt seinen Executor also **im selben Tick** laufen.

### Die Leiter

```c
s32 s2 = self[0x1F0];                        // Distanz          ✔ lw s2,496(s0) @0x80101744
s32 s4 = arc(self, PL.x, PL.z, 1024);        // jal @0x80101748, a3 @0x8010174c, gefangen @0x8010176c
s32 s3 = arc(self, PL.x, PL.z,  512);        // jal @0x80101768, a3 @0x80101754, gefangen @0x8010178c
         arc(self, PL.x, PL.z, 1300);        // jal @0x80101788 — ERGEBNIS TOT (v0 zerstört @0x80101790)
// arc() == re15_ai_arc_test: 0 == INNERHALB des Kegels

if (self[0x23E] == 0) {                      // ✔ lbu v0,574(s0) @0x80101790 / bne @0x8010179c
  /* A */ u32 d = self[0x1F4];
  if ((d & 0xC0000000) && (d & 0x3FFFFFFF) < 2000        // @0x801017a0/ac/b0, sltiu 0x7d0 @0x801017c0
      && FUN_80015714(self, (s16)self[0x1F8], 256) == 0) // @0x801017cc-d8
      { self[4] = 0x00000E01; return; }      // ✔ EINZIGER Früh-Ausstieg: j 0x80101a1c @0x801017e0,
                                             //   Store im Delay-Slot @0x801017e4
  /* B */ if ((s16)s3 == 0 && (u32)s2 < 2000             // @0x801017f4/f8, sltiu @0x801017fc
           && self[0x106] != PL[0x106]                   // beq-weg @0x80101814
           && PL[0x1D3] == 0)                            // bne @0x80101824
      self[4] = 0x00000E01;                              // @0x80101828/2c  (KEIN return)
}
/* C */ if ((self[0x1D4] & 0xC000) && (self[0x110] & 1))
     self[4] = 0x00000A01;                               // @0x80101854/58
/* D */ if ((u32)s2 < 3500 && (s16)s4 != 0               // sltiu 0xdac @0x8010185c, beq-weg @0x80101868
         && (*(u16*)0x800CFBF6 & 0x15) && (rand() & 3) == 0)
     self[4] = 0x00000C01;                               // @0x8010189c/a0
/* E */ if ((u32)s2 < 2500 && (s16)s4 != 0               // sltiu 0x9c4 @0x801018a4
         && (*(u16*)0x800CFBF6 & 0x17) && (rand() & 1) == 0)
     self[4] = 0x00000C01;                               // @0x801018e0/e4
// ✔ D und E sind SEQUENZIELL, nicht gestaffelt: @0x80101860 springt bei Miss nach 0x801018a4 =
//   Es genau. Unter 2500 laufen BEIDE und ziehen BEIDE eine Zufallszahl.

if (PL[0x8] != 15) {                                     // @0x801018e8-f0
  /* G */ if ((u32)s2 < 1200 && !(PL[0x1D3] & 0x80) && self[0x106] == PL[0x106]) {
      if (!(self[0x21A] & 0x20) && FUN_80015758(self+0x38, PL+0x38, yaw+256, 256) == 0)
          { self[4] = 0x00000301; PL[0x1D3] |= 0x80; }   // @0x80101954/58, @0x80101964/68
      if (!(self[0x21A] & 0x40) && FUN_80015758(self+0x38, PL+0x38, yaw-256, 256) == 0)
          { self[4] = 0x00000301; PL[0x1D3] |= 0x80; }   // @0x80101998/9c, @0x801019a8/b0
  }                                                       // ✔ j 0x801019e8 @0x801019ac (überspringt J)
} else {
  /* J */ if (self[0x23E] == 0 && (s16)s3 == 0 && (u32)s2 < 2000 && PL[0x1D3] == 0)
      self[4] = 0x00000E01;                               // @0x801019e0/e4   (= B ohne den 0x106-Test)
}
/* K */ if (PL[0x156] == -32768 && (s16)s3 == 0 && (u32)s2 < 1000) {  // ✔ @0x801019e8-a00
     self[4]     = 0x00060801;                            // ✔ lui 0x6 / ori 0x801 / sw @0x80101a10
     self[0x10E] |= 0x4000;                               // ✔ lhu/ori/sh @0x80101a08/14/18
}
```

**✔ „Letzter Schreiber gewinnt" ist bewiesen, nicht angenommen:** im Bereich 0x80101714–0x80101A34
gibt es exakt **9** `sw …,4(s0)` (0x801017E4, 82C, 858, 8A0, 8E4, 958, 99C, 9E4, A10) und exakt
**zwei** Sprünge — `j 0x80101a1c` @0x801017E0 (Block A in den Epilog) und `j 0x801019e8` @0x801019AC
(G überspringt J). Keine Instruktion der Funktion *liest* `+0x4`, also ist sequenzielles C exakt.
Feuert kein Block, bleibt `+0x4` unberührt.

**Die Zahl der RNG-Ziehungen ist selbst Verhalten:** 0–2 pro Tick (D @0x80101888, E @0x801018D0),
und nur nachdem die jeweils ersten drei Gates passiert sind. Wer die Reihenfolge portiert, aber die
Ziehungen falsch zählt, desynchronisiert die gesamte Folge.

**Wort → Bytes (LE):** `0x0E01`→sub 14, `0x0A01`→10, `0x0C01`→12, `0x0301`→3, `0x00060801`→sub 8,
Phase `+0x6`=6. Jeder `sw` **nullt `+0x6`/`+0x7`** — tragend, weil die Executors auf `+0x6` dispatchen.

### Der globale Filter `0x800CFBF6` (selbst aufgelöst)

5 Schreiber, alle in der EXE: `@0x8003BFF0` löscht Bits 0..4 (`andi 0xffe0`, **gegated** auf
`0x800CFBDC >= 0` @0x8003BFC0 — die Behauptung „wird jeden Frame gelöscht" war deshalb falsch),
Bit `0x2` wird gesetzt @0x8003CC80 und @0x8003D6B4, Bit `0x4` @0x8003D18C.
Damit ist der Maskenunterschied **echt**: `0x15` (Bits 0,2,4) lässt Bit `0x2` aus, `0x17` schließt es
ein — Block E reagiert also auf eine Spieleraktion, auf die Block D nicht reagiert.
**Welche** Aktionen die Bits 0 und 4 setzen, ist noch offen.

### ✅ VERDRAHTET (Commit `c1ef8e8e`)

Die Leiter **ersetzt** bei RE2-Flavor `re15_ai_decide_engage` vollständig — sie *ist* das Gegenstück
zu `DECISION[1]`, nicht eine Ergänzung. Beleg, dass sie den Angriff auslöst
(`RE15_RE2_TRACE=1`, Ausgabe in `debug.log`):

```
[re2z] gates d=1137 flo=0/0 claimed=0 g1=0 g2=1 -> COMMIT 0x00000301
danach claimed=128  -> der Claim-Latch sperrt weitere Grabs, wie im Original
```

A/B ROOM1140, identisches Spieler-Skript `L14,U200`:

| | Grab-Frame | nächste Distanz | Spieler-HP |
|---|---|---|---|
| RE1.5 | 725 | 726 | 70 |
| RE2 | 685 | 284 | 75 |

**Drei Fehlschläge, die nur die Messung aufgedeckt hat** (und die als Muster wichtig sind):
1. Der erste Hook saß im **ANIMATE**-Teil statt im DECIDE-Teil → **0** Leiter-Commits; der Grab im
   RE2-Lauf kam weiterhin aus dem RE1.5-Pfad. Hätte ich nur „RE2 greift an" gemessen, wäre der
   Fehler unentdeckt geblieben.
2. `stderr` geht per `freopen` nach `debug.log` — mein `2>`-Redirect zeigte 0 Zeilen.
3. Der Verdacht „Distanz zu groß" war falsch: 43 von 75 Gate-Aufrufen lagen unter 1200.

**Gate-Belegung:** gemappt sind `dist`, beide Kegel, `floor`, HP, der Claim-Latch und die zwei
Sektor-Tests. Mit **belegter Null** gefüllt: `self+0x23E` (einziger Schreiber `@0x80104E2C` in
EXECUTOR[14], nie betreten), `self+0x1F4/+0x1F8` (null Schreiber in EMZ0; Erzeuger `FUN_80065518`
tickt nur Typen 64..91, Zombies sind `@0x8001B738-48` auf 16..31 geklemmt), `self+0x21A`
(INIT löscht `@0x8010087C`). Nur `0x0301` wird angewandt.

**Claim-Latch `PL+0x1D3` bit 0x80** — Mechanismus vollständig belegt: der Zombie **setzt** ihn an
neun Stellen (`ori 0x80`, u.a. `@0x80101968`/`@0x801019B0`) und löscht ihn nie; das **Löschen** liegt
auf der Spielerseite (`andi 0x7f` + `sb`, `@0x8003E844` und `@0x800630E0`). Port-Äquivalent:
`s_player_grabbed`. ⚠️ Zwei vermeintliche Löscher im Overlay (`@0x801006C8` INIT, `@0x8010499C`)
treffen das **eigene** Byte — in beiden Funktionen ist die Basis `a0 = self`; meine erste
Registersuche mit 300-Instruktions-Fenster war dort falsch positiv.

**Bekannte Abweichung:** das Original lässt den GRAB-Executor im **selben** Tick laufen
(`0x8010118C` liest `+0x5` nach der Entscheidung neu, `@0x801011D0`); der Port startet ihn einen
Frame später.

### ⛔ Was an W2 noch NICHT portiert ist

Nach der Blocker-Auflösung (2026-07-29) sieht die Leiter so aus:

| Block | Wort | Status | Beleg |
|---|---|---|---|
| **A** | `0x0E01` | **kann nie feuern** | `+0x1F4` hat **null** Schreiber in EMZ0; Erzeuger `FUN_80065518` tickt nur Typen 64..91, Zombies sind `@0x8001B738-48` auf 16..31 geklemmt |
| **B** | `0x0E01` | Gate gelöst, Executor fehlt | braucht `PL+0x1D3 == 0` als **ganzes** Byte; untere 7 Bit = 15-Frame-Countdown, den der Grab-Executor setzt (`addiu v1,zero,15` @0x8010276C, `sb` @0x80102770) und die Spieler-Seite herunterzählt (@0x8003BFF4-C008) |
| **C** | `0x0A01` | **inert mit RE1.5-Raumdaten** | `self+0x1D4` hat **keinen** Schreiber im Zombie-Overlay; alle EXE-Schreiber sind skript-/spawn-parametergetrieben (`lhu v0,2(a1)` @0x800570EC → `sh v0,468(v1)` @0x800570F4). RE1.5-Räume setzen RE2s Bits `0xC000` nie |
| **D** | `0x0C01` | ✅ **feuert** | Filter aufgelöst (s.u.); rennen, ab 3500, 25% |
| **E** | `0x0C01` | ✅ **feuert** | jede Bewegung, ab 2500, 50% |
| **G** | `0x0301` | ✅ **feuert + wirkt** | verdrahtet auf den byte-true Grab des Ports |
| **J** | `0x0E01` | wie B | zusätzlich `PL+0x8 == 15` |
| **K** | `0x00060801` | **kann nie feuern** | braucht Spieler-HP `== -32768`; der einzige `-32768 → +0x156`-Store im ganzen Spiel (`@0x8010B730/38`) schreibt die HP eines **Gegners** |

### Der Filter `0x800CFBF6` — AUFGELÖST

Kette vollständig selbst disassembliert:

1. **Vier** Schreiber im gesamten RE2-EXE. `@0x8003BFF0` löscht Bits 0..4 (`andi 0xffe0`), **gegated**
   auf `0x800CFBDC >= 0` `@0x8003BFC0`. Bits `0x1` und `0x10` werden **nirgends** gesetzt → Maske
   `0x15` reduziert sich auf Bit `0x4`, Maske `0x17` auf `0x2|0x4`.
2. Die drei Setzer sind **Spieler-Sub-State-Handler**: Dispatcher `@0x8003C5D4` indiziert die Basis
   `0x800A4084` mit `player+0x5` → sub 1 = `0x8003CBDC` (`ori 0x2` @0x8003CC80),
   sub 2 = `0x8003D0E8` (`ori 0x4` @0x8003D18C), sub 3 = `0x8003D5F4` (`ori 0x2` @0x8003D6B4).
   Der zweite Dispatcher `@0x8003C19C` indiziert dieselbe Tabelle ab `0x800A4030` mit `player+0x4`
   (dem State); die Sub-Tabelle überlappt sie ab Index 21.
3. Welcher Sub-State was ist, steht im **pad-getriebenen** Selektor `@0x8003C650-C6C8`:
   `0x1` vorwärts → sub 1, `0x200` rennen → sub 2, `0x4` rückwärts → sub 3, `0xa` drehen → sub 4
   (setzt **nichts**). Bit-Bedeutung aus der virtuellen Pad-Tabelle des Ports
   (`pad_common.c:27-35`, RE1.5 `@0x80073dbc`): bit0←UP, bit2←DOWN, bits1|3←RIGHT|LEFT, bit9←CROSS.

**→ Bit `0x2` heißt „der Spieler GEHT", Bit `0x4` heißt „der Spieler RENNT".** Block D reagiert nur
auf Rennen, Block E auf jede Bewegung. **Stillstehen = kein D/E-Angriff.** Genau das ist das
RE2-Verhalten, das die Folklore für Sehvermögen gehalten hat.

⚠️ **Port-Mapping, kein byte-true Port des Feldes:** der Erzeuger ist RE2s Spieler-Zustandsmaschine,
die der Port nicht hat. Abgebildet auf die Bewegungs-Sentinels (`player_common.c:65-67`: RUN=100,
WALK=105, BACK=WALK+reverse), pro Tick neu berechnet wie die Original-Bits.

**Gemessen** (`RE15_RE2_TRACE=1`, ROOM1140, `L14,U200`): **20 Commits** — 1× `0x0301` und
**19× `0x0C01`**. Vorher: 1.

### ~~Was jetzt noch fehlt~~ → in WELLE B geschlossen (s.u.)

## ✅ WELLE B — der KOMPLETTE RE2-Zombie (Verhalten + Präsentation), 2026-08-10

Code: `enemy_ai_re2_zombie.c` (`re15_re2z_tick` = das volle Brain), Shims in `enemy_ai_common.c`,
Hooks in `re15_enemy_ai_live_tick` / `re15_zgirl_ai_tick` (der RE1.5-Dispatch wird für RE2-Zombies
KOMPLETT ersetzt — die alten Teil-Hooks in `decide_engage`/Walk-Animate sind entfernt).
Tests: `test_re2_zombie_ai` (erweitert) + `test_re2_room1140_ab` (A/B im echten ROOM1140).
Alle Zitate erneut selbst disassembliert (`re2_disasm.py --bin EMZ0.BIN`); Stichproben der
Parameterblock-Bytes direkt aus EMZ0.BIN gedumpt.

### Struktur (byte-belegt)

* **Root-Prolog = Cooldown-Bank** vor dem State-Dispatch: `+0x239` (Moan-CD) `@0x8010045C-6C`,
  `+0x23E` (Biss-CD) `@0x80100470-80`, `self+0x1D3` low-7 `@0x80100484-98`.
* **ACTIVE = Decision-dann-Executor im selben Tick** (`lbu +5` → `0x8010C88C`, `lbu +5` erneut →
  `0x8010C8CC`, `@0x801011A8-EC`); die Tabellen selbst ausgelesen (16 Paare, s. Code-Kommentar).
* **Modell-Parameterblock @0x80100000** (Bytes selbst gedumpt): +0x04 Walk-Stile `{0,2}`
  (INIT-Pick `(r1>>(r2&3))&7` `@0x80100860-8C`), +0x0C Grab-Clips `0B 0B 0E 0E ×2` (P0 liest
  `[s5*2]` `@0x801026C4-CC`), **+0x14 Biss-Paare `(16,20),(1,5),(16,30),(1,10)`**, +0x1C die 10
  Grab-Phasen-Zeiger (== 0x801026C0…0x80102EB4, verifiziert), +0x44 Clip-Liste
  `01 02 17 16 08 09` (Fall/Leiche/Bodenliegen), +0xD8 Aufsteh-Clips `03 03 04 0D`.
* **Bank-Modell**: der WALK spielt/verfährt die **Pair-1-Bank** (sein `0x80015e7c` lädt explizit
  `a1=+0x108/a2=+0x17C` `@0x80101CB0-BC`; Pair-1-Clips 0..7 tragen die +X-Vorwärts-Root-Bewegung,
  byte-gelesen aus EM010) — alle anderen Subs die 31-Clip-**Pair-2**-Action-Bank (deren Clip 2 =
  Rückwärts-Fall, sx 0/−53/−186…). Im Port: `re15_actor_uses_loco_bank`-Zweig + `re15_re2z_move_root`.
* **Bewegung** = das Paar `0x80015e7c` (schreibt das Keyframe-Root-DELTA nach `+0x144/146/148`,
  `sh v1,324(t0)` `@0x80015FD8-E4`) + `0x800152C8` (wendet den Vektor yaw-rotiert an,
  `@0x80015314-34`) → im Port EINE Delta-Anwendung. Grab-P8-Recoil: `delta.x −= 30` zwischen den
  beiden (`@0x80102CA0-AC`). Die nackten `sh 11,+0x144`-Seeds sind Dead Stores (dokumentiert).

### Executors (alle mit @-Zitaten im Code)

| Sub | Inhalt |
|---|---|
| 0 | Idle Clip 0, Zufalls-Startframe, Moan-Timer (`@0x80101458-90`); DECISION[0] `@0x80101294`: dist<5000+Kegel→`0x101`, Bewegungs-Filter+50 %→`0xC01` (`@0x8010131C/74/3CC`) |
| 1 | WALK: Clip `+0x218` (`@0x80101A7C-8C`), W1-Turn-Gate, Moan-Block (`@0x80101C44-88`), Pair-1-Root-Bewegung; DECISION[1] = die W2-Leiter, jetzt LIVE für ALLE Worte |
| 3 | **GRAB, 10 Phasen** (P0 Latch/Claim/self+0x1D3=15/Moan `@0x801026C0-D0`, P2 Biss=Grab+1 **plain, Rate 0** + Budget 148 `@0x80102814-34`, P3 Biss-Frame/Schaden + Mash −(2+5·mash) `@0x80102838-FC` — der Budget-Ablauf-Tick **fällt in den Biss-Check durch** `@0x80102884-98`, P4 Abwurf `@0x80102968`, P5 1/16-Seiten-Latch 0x20/0x40 `@0x801029A4-A40`, P7/P8 Erholung + 15/16-Rückwärts-Fall (Commit `@0x80102D24-DEC`: `0x501`+Phase 1, Clip 2@Frame 20, self+0x1D3\|=0x80, +0x10E\|=0x2000, SE rand&1→13/12, t158=1); **P8-Frames 7..24 = PARTNER-DOMINO** `@0x80102DF0-EB0` (Gates kind&0x10/HP≥0/!Kriecher/+0x1D3==0/!(+0x21A&8)/!(+0x10E&0x2000) → Partner `0x901` + Richtungsbyte +0x16B + **SE 4** `@0x80102E90-A8`; Partner-Index +0xD → Tabelle `@0x800CFE14`, Produzent nicht RE'd → Port: Body-Push-Kontakt, MAPPING); **P9 = Wort `0x1` (STAND), KEIN SE** `@0x80102EB4-B8` — Spieler-Seite über die Port-Victim-Infra; Kill-Tick-Richtung explizit (`re15_re2z_victim_devour`, `@0x80102928-50`) |
| 5 | KNOCKDOWN-Fall (eigene Phasentabelle param+0x6C): P0 Seite+Clip 1/2 **ab Frame side·5+10** (`@0x80103310-38`) + self+0x1D3\|=0x80/+0x10E\|=0x2000 (`@0x80103300-20`) + **SE rand&1→13/12 cd150** (`@0x8010332C-60`) + t158=0 (`@0x80103368`), P6 **Kriech-Marker `+0x21A|=0x10` `@0x8010358C`** + Liege-Clip 8/9 + ¼-Moan, P7 = Liege-Clip läuft aus (advance `@0x801036A0-A4`, **kein Timer**) → bei done **Marker-Clear `@0x801036B8-BC`** + `&=~0x2` `@0x801036C8-CC` + Phase 8, P8 → `0x101` (`@0x801036F4-F8`) |
| 6 | Post-Kill (Grab committet `0x601` beim Spielertod `@0x80102924-38`): Clip-Wort 0xF0018=Clip 24 (`@0x801039B0/C8`), Anker `0x80015b94` (`@0x801039D0`, OPEN), SE rand&1→11/10 (`@0x801039D8-F0`), self+0x1D3\|=0x80 → `0x101` (`@0x80103B14`) |
| 7 | Liege-Spawn: P0 Clip 23/22 **plain (Rate 0)** (`@0x801037CC-E4`) + self+0x1D3\|=0x80; P1 = **Limpet-Halt auf `+0x10E&0x4000`** (`@0x8010381C-28`; Spawn schreibt 0x4002 `@0x80100A34-38`); P2 Boden-Idle 8/9 Rate 0xF Zufalls-Frame (`@0x80103838-8C`) + Moan ½ SE 12/¼ SE 10/¼ SE 11 cd150 (`@0x80103894-D4`); P3 advance; **P4 → `0x101`** + Claim-Clear (`@0x80103900-2C`). Wake-PRODUZENT = Limpet-Clear (einziger Overlay-Clear `@0x80104F0C` in EXEC[15], skript-/EXE-seitig) → Port: RE1.5-Nähe-Gate als MAPPING |
| 8 | Fressen: **rotiert Clips 18/19/20** per rand&7 aus param `@0x801000A8` (P0 `@0x80103BF4-C10` Rate 7; P2-Re-Draw Frame 3 Rate 0 `@0x80103CB0-D4`); Limpet wie sub 7 (Spawn 0x4004 `@0x80100A88-8C`, Loop-Gate `@0x80103C94-A0`); Wake-Kette P3 Aufsteh-Clip 0x15 (`@0x80103CD8-EC`) + Marker-Set/Clear → **P5 `0x101`** (`@0x80103D90-94`) |
| 9 | Aufstehen: Clips 3/4 (param+0xD8), `+0x21A = (~0x10)|0x8` (`@0x80103F7C-90`), SE 12 **rand&1- und cd-gegated + cd150** (`@0x80103F84-B4`) → `0x101` |
| 12 | **AUSFALL-BISS**: 0x19→0x1B Rate 7, ±190-Yaw-Korrektur, Kegel 320, Dash-Root-Motion, Ende Frame 25/`0x101` (`@0x801047B8-48FC`) — KEIN Direktschaden (einziger 401d4-Caller = Grab) |
| 14 | **SCHNAPP-BISS**: Clip 0x11 Rate 0xF, Frame 10 FX+SE 5, Ende `+0x4=1` + **`+0x23E=60`** (`@0x80104DB4-E2C`) |
| 13 | **Voller Re-Init** (`@0x80104928`): Wort `1` (`@0x80104988`), HP-Re-Roll aus `@0x8010C600` (rand&0xf, `@0x801049C8`), Stil = **EIN** Draw&7 aus **eigener** Tabelle `@0x801000F8` (`@0x801049C4-EC`), Flag-Clears + res223-Reseed (`@0x801049DC-A18`); Re-Bind/Schatten OPEN — heute toter Code (kein 0xD01-Produzent) |
| 2 | Anrempeln: Clip **4+walkclip** (Pair 1!) Rate 0xF (`@0x80102290-AC`), t158=(rand&0x3f)+180 (`@0x801022A8-C0`), SE cd-gegated rand&1→11/10 + cd150 (`@0x801022C4-E8`) — Entry OFFEN |
| 11 | Clip 0x0A Rate 0xF (`@0x801043D8/F8/440C`), Steer(+0x1C4, 128) (`@0x80104400-14`), `+0x21A=(~0x4)|0x2` (`@0x80104438-48`), self+0x1D3\|=0x80, +0x10E\|=0x2000, SE cd-gegated rand&1→11/10 (`@0x80104474-98`) — Entry OFFEN |
| 10/15 | inert (Block C / Bank-B; EXEC[15] trägt den einzigen Limpet-Clear `@0x80104F0C`) |

### HURT/DEATH/CORPSE/State 8

* **HURT `@0x80104F40`**: Grab-Abbruch-Prolog (PL-cmd==5 && PL+0x1B4==self → Claim-Clear
  `@0x80104FAC`, beidseitig `&=~0x1004`); Resistenz `+0x223` (INIT-Seed 16+(rand&15)
  `@0x80100888-9C`; Abschreibung = angewandter Schaden, **MAPPING** — der Dekrement-Produzent
  liegt im nicht disassemblierten RE2-EXE-Damage-Writer). Überlebt die Resistenz → Resist
  **ohne** `+0x222`-Write (`@0x80105078` springt an `0x80105164` vorbei). Erschöpft →
  **Eligibility-Gate `@0x80105080-98`**: `0x501` NUR wenn `+0x222==1` ODER `+0x5==1` (Port:
  `re2z_prev_sub`, weil das geteilte take_damage `+0x5` überschreibt); sonst Marke
  (`@0x80105164`) + Resist. Flinch: Re-Seed (`@0x801050A4-C8`) + jeder 3. Treffer Blut
  (`@0x801050B0-E4`); **KEIN SE auf dem Flinch-Pfad** (beide Zweige enden `j 0x80105418`;
  SE 9 `@0x801052B4-B8` liegt im nicht portierten Per-Sub-Bereich hinter `+0x21A&0x60==0`,
  `+0x152<0`, `+0x1D0&0xC0` — OPEN, stumm). Kriech-Konvertierung (`+0x21A&0x10` →
  `0x80107A78`) NICHT portiert (W5); der Marker lebt nur während der Bodenphase — P7-Exit
  löscht ihn (`@0x801036B8-BC`).
* **DEATH `@0x80108250`**: Kill-Latch `+0x21A|=0x4000` (`@0x80108294-98`), Grab-Abbruch
  (Claim-Clear `@0x801082F4`), Todes-Clip 7, Ende → `0x907` (`@0x801084DC`).
* **CORPSE `@0x8010A440`**: Clip 23, bei `+0x21A&0x4` 22 (`@0x8010A490-BC`), **HP=−1**
  (`@0x8010A4D4`), Timer 120/40; Leichen-Tint 0xBFBF10 = Render-seitig OFFEN.
* **State 8 `@0x80109CFC`**: Entry OFFEN (Helfer-Tails nicht RE'd); Auflösung implementiert:
  Aufsteh-Commit `0x901` (`@0x8010AE9C`).

### Präsentation

* **Clips**: RE2-nativ aus der echten EM01x-Bank (Welle-A-Loader); Clip-Wort
  `(rate<<16)|(frame<<8)|clip` auf `+0x14C` → Port-Felder (rate 0xF/7 = frac, Advance-a3
  0x100/0x200 = blend).
* **SEs** über `re15_audio_re2_enemy_se` (Hook `re15_re2z_audio_hook`, PC registriert in
  `pc_enemy_load`): 3 Grab-Biss `@0x801028E8`, **4 = Partner-Domino-Wake** `@0x80102E9C-A4`
  (NICHT „Grab-Ende" — P9 spielt nichts), 5 Schnapp-Biss `@0x80104DFC`, 10/11 Moans (Idle ½
  `@0x801014CC-EC`, Walk 2×1/32 `@0x80101C54-88`, Grab rand&1 `@0x8010279C-C0`, Boden ¼
  `@0x801035F0-610`), **12/13 Knockdown-Paar** (rand&1→13/12: Grab-Fall `@0x80102DC4-DC`,
  Knockdown-P0 `@0x8010333C-58`), 12 Getup (rand&1+cd `@0x80103F94-B4`), Liege-Idle-Mix
  ½ 12/¼ 10/¼ 11 (`@0x80103894-D4`); **SE 9 = OPEN** (Per-Sub-Bereich, s. HURT). Alle mit
  Cooldown `+0x239=150` wo zitiert. **Footsteps** = Frame-Flag-Mechanismus `0x801016c8`
  (Bit 0x08000000, id = Wort>>28 < 2 → ENEMSE-SE 0/1; EM010-Pair-1-Clip 0: Frames 20/62) —
  flavor-bewusst in `re15_enemy_anim_sfx`.
  **ENEMSE-Bank = 0**: die Paar-Tabelle `@0x800a7400` (file 0x97C00, Zeile 11 = `{0x10,0}`
  byte-verifiziert) ist zur kind-Basis nicht auflösbar; empirische Probe über alle 73 dekodierten
  EDT-Maps: Bank 0 ist die einzige Single-kind-Bank, deren Live-Einträge ALLE benutzten Zombie-IDs
  {3,4,5,8,9,10,11,12} abdecken (Bank 11 fehlt 11/12, Bänke 28/65 fehlen 4/5, Bank 72 nur in der
  +0x10-Hälfte). `RE15_RE2_SE_BANK` übersteuert zum Hör-A/B. Hörbarkeit im Agent-Env nicht prüfbar.
* **Treffer-FX bleiben RE1.5** (dokumentiert): RE2s FX-System (`FUN_8001bf10`, z.B. Schnapp-Biss
  `0x0A001000` `@0x80104DE0-F4`) ist nicht portiert (Row-Kompatibilität unbelegt, Lane I §3) —
  der Port spawnt das RE1.5-Room-Bank-Blut an denselben Stellen.
* **Schaden** (`FUN_800401d4`, Decompilat): Biss-Paare byte-true (20/5/30/10); **One-Save**
  (HP<0 aber ≥−14 und Latch frei → HP=0 + Latch; sonst Tod) portiert; die ×1.5/×5/×2-Skalierungen
  hängen an RE2-only-Globals (200-HP-Skala, Difficulty `@0x800D482A`) — nicht portiert, dokumentiert.

### Verifikation

* `test_re2_zombie_ai`: Gates↔Produzenten, `+0x23E`-Gating, Root-Prolog-Dekremente (bit7-Erhalt),
  Grab-Phasen P0–P5 inkl. Biss-Schaden 20 + Budget −2 + **Ablauf-Tick-Biss-Fallthrough**,
  **P9-Wort `0x1` ohne SE**, **Partner-Domino** (0x901+SE 4, 0x2000-Gate), One-Save→Tod
  (`0x601`), Schnapp-Biss-Ende `+0x23E=60`, Lunge-Clips 0x19→0x1B→`0x101`,
  **HURT-Eligibility** (Resist ohne 222-Write / Marke ohne Flinch / 222==1→`0x501`, kein SE),
  Knockdown-P0 (Frame 10/15, SE 12/13, cd150, Downed-Marker) + P7-Marker-Clear,
  DEATH→CORPSE (HP −1, Clip 23/22 nach Bit 0x4), State 8→`0x901` (Getup-Latch 0x8 + SE-Gate),
  Leiter-LIVE (D/E→EXEC[12] im selben Tick), INIT-Remap (Feeder→sub 8 + `+0x10E=0x4004`,
  **Fress-Rotation 18/19/20**, Wake-Kette + Grid-Clear), **Liege-Kette** (Limpet-Halt →
  Idle 8/9 → `0x101` + Grid-Clear), P5-Seiten-Latch.
* `test_re2_room1140_ab` (echter Raum, echtes SCD, voller run_all-Loop): RE1.5-Baseline
  first-attack F32; RE2: 1×lying+4×feeding-Spawns, Wake F12, first-attack F35, **Biss-Drop exakt
  20 HP**, Beschuss→Knockdown→CORPSE HP=−1+Kill-Latch, 600-Frame-No-Freeze-Lauf.
* **Live-Smoke** (echte EXE, echte RE2-Bänke, `RE15_AI_FLAVOR=re2` + Debug-JUMP ROOM1140):
  EM016/EM010/EM011 geladen (31 Clips), Zombies laufen an (Pair-1-Root-Bewegung), Leiter committet
  live (`0x0C01`-Lunge, `0x0301`-Grab), HP-Kette 100→70→40→20→**0 (One-Save)**→−20 (Tod) mit den
  byte-zitierten 30er- (Typ 0x11) und 20er-Bissen, Opfer-Collapse-Clip 6 aus der RE2-Victim-Bank,
  Seiten-Latch (21a=0x0020) und Knockdown-Flags (21a=0x0216) im Trace, kein Freeze.

### OFFEN (Welle-B-Ausgang, nach dem Review-Pass)

* Kriech-Variante (`0x80101210`, eigene Tabellen `0x8010C90C/18`, HURT-Tabelle `0x8010CBE8`,
  Konvertierung `0x80107A78`) — W5.
* State-8-EINTRITT (Helfer-Tails `@0x80107A58/@0x80107EB8`), CORPSE-Beschuss-Subs 1..11,
  Dismember-Helfer `0x80106128/0x80106310/0x8010640C/0x80106510`, EXEC[5]-Phasen P1–P5-Interna
  (FX/SE), HURTs Per-Sub-Reaktionsbereich `@0x80105168+` (inkl. SE-9-Gates `+0x1D0&0xC0`).
* Wake-PRODUZENT der Limpets: der einzige `+0x10E&0x4000`-Clear liegt in EXEC[15]
  `@0x80104F0C` (Bank-B-Kette); wer `0xF01` committet ist skript-/EXE-seitig nicht RE'd →
  Port ersetzt ihn durch das RE1.5-Nähe-Gate (MAPPING, dokumentiert im Code).
* Partner-Index `+0xD` (Tabelle `@0x800CFE14`): Produzent nicht RE'd → Port-MAPPING =
  Body-Push-Kontakt (`contact_slot`); Richtungsbyte +0x16B via facing-aligned-Kollaps.
* PL+0x1D3-low-7-Fenster (B/J bleiben über ihre übrigen Gates in RE1.5-Räumen stumm),
  `+0x154&0x800`-Gate (gemappt 1), FUN_80015910-Richtungs-Sub → front/behind-Kollaps
  (Grab-P0, Kill-Tick, P5-Latch, Domino), Rumble (`0x800395B8/0x80039514/0x8003947C`),
  Kill-Zähler-Global `0x800D46C0`, Leichen-Tint, ENEMSE-Zeilen-Semantik `FUN_80052b38`,
  EXEC[13]-Re-Bind `0x80028794` + `+0x151..153` + Schatten-Reset, EXEC[6]-Anker
  `0x80015b94`, Fress-Tropf-FX `0x13D0`, `+0x1C0`-Bits (kein Port-Feld),
  Downed-/Grid-Mappings (Review #16/#18) = Port-Infrastruktur, als MAPPING kommentiert.

## ✅ Spieler-Opfer-Seite (Fress-Kollaps) — GELÖST 2026-08-21

Nutzer-Report (RE2-Modus): *„wenn Leon gefressen wird, steht er noch komisch."* Der oben als
OFFEN geführte „EXE-seitige Grab-Art-6-Spielerhandler" ist **nicht** EXE-seitig — das
**Zombie-Overlay installiert ihn selbst**, exakt wie die RE1.5-Hooks `0x800ac758`/`0x800ac858`:

```
801010ec: lbu v0,8(s2)                       ; kind
801010f0-f4: lui v1,0x8011 / addiu v1,-22236 ; = 0x8010A924
801010fc-104: lui at,0x800d / addu at,v0 / sw v1,-7424(at)  ; 0x800CE300[kind]  = cmd 5
80101108-120: dito 0x8010B3C0 -> 0x800CE400[kind]           ;                    = cmd 6
```

* **cmd 5 (Griff/Struggle)** `0x8010A924` → Tab `@0x8010CF2C[PL+0x5]` = {`0x8010A9B8`×2,
  `0x8010AF58`×2}; Maschine `0x8010A9B8`, Phasen `@0x801001DC`. Basis-Paar **{0,3}**
  (`sb zero,24(sp)` `@0x8010A9E4` / `sb 3,25(sp)` `@0x8010A9E8`); P0 Clip = Basis
  (`@0x8010AA5C`, Rate 0x0F), P2 Clip = Basis+1 (`@0x8010AB88`), P4 Clip = Basis+2
  (`lui a0,0x7 / ori 2` `@0x8010ACA0-A8`, Rate 7). ⇒ intro/hold/release **identisch** zur
  RE1.5-Belegung `v*3 / +1 / +2` — der Port war hier schon richtig.
* **cmd 6 (Gefressen/Kollaps)** `0x8010B3C0` → Tab `@0x8010CF3C[PL+0x5]` = {`0x8010B464`,
  `0x8010B464`}; Maschine `0x8010B464`, Phasen `@0x8010022C`. Basis-Paar **{13,15}**
  (`addiu v0,zero,13` `@0x8010B484` + `sb v0,24(sp)` `@0x8010B488`; `addiu v0,zero,15`
  `@0x8010B48C` + `sb v0,25(sp)` `@0x8010B490`); P0 `@0x8010B4C4` Clip = Paar[Variante]
  (`lbu v1,24(v0)` `@0x8010B4D0`, Rate 0x0F `lui a0,0xf` `@0x8010B4C8`, Clip-Wort
  `sw v1,332(s2)` `@0x8010B4F4`). **Tod erst in P2** `sh -32768,342(s2)` `@0x8010B738`
  (= PL+0x156 = 0x800CFD4E), also im Tick NACH dem Clip-Ende.

**Der Bug:** der Port fuhr für die Zombie-Familie auch unter RE2 die RE1.5-Belegung
`collapse = Variante + 6`. In der RE2-Victim-Bank (EM010 EMD-Paar 3, **17** Clips) sind 6/7 aber
**stehende Struggle-Clips**. Gemessen über den Renderpfad (`platform/pc/main.c:5460-5478`,
PL00-Rig + Keyframe-Pool der Greifer-Bank, Weltposition Kopf-Bone 8; PSX-Y nach oben negativ):

| RE2-Victim-Clip | Frames | Kopf-y am Clip-Ende | |
|---|---|---|---|
| 6 / 7 (was der Port spielte) | 10 / 30 | −2340 / −2350 | **STEHEND** (Referenz stehend: −2513) |
| **13 / 15** (Original) | 116 / 116 | −138 / −353 | **am Boden** |

Fix: `re15_victim_clip_map` (`enemy_ai_common.c`) bekommt für `re15_re2z_owns_type` unter
RE2-Flavor `collapse = 13/15`, und der Tod fällt am letzten Clip-Frame statt bei der
RE1.5-Konstante 0x23. PIN: `unit_re2_victim_collapse` (inkl. Negativ-Kontrolle „6/7 sind
stehend" und RE1.5-Regressionswache). Messsonde: `probe_re2_victim_pose`.

**Weiterhin OFFEN (mit Adressen, bewusst nicht halb gebaut):** die Nachlauf-Phasen des Kollapses
— P2 `@0x8010B724` schaltet `PL+0x14C += 1` (`@0x8010B73C-40`) auf den 6-Frame-Zuck-Clip 14/16,
P3 `@0x8010B744` spielt ihn, P4 `@0x8010B774` zieht `PL+0x16B = (rng&0x1f)+3`, P5 `@0x8010B78C`
springt bei 0 zurück auf P3 (Zuck-Schleife), P6 `@0x8010B7AC` schreibt `PL+0x4 = 7`. Die Endpose
von Clip 13 ist bit-identisch zur Startpose von 14 (Kopf (−703,−138,97)), 15→16 analog
((661,−353,−181)) — der gehaltene Endframe zeigt also dieselbe Pose. Ebenso OFFEN: die
Blut-Kadenz in P1 (`jal 0x8001bf10` `@0x8010B668` auf jedem GERADEN Frame in [41,98],
`@0x8010B620-34`, a0 = 3024+(rng<<3) `@0x8010B650-5C`; zwei weitere Spawns Frames 100..104
`@0x8010B678-80` mit a0 = 6096 / 7120) — die RE2-Gore-Familie `0x8001bf10` hat im Port kein
Gegenstück.

## ✅ W5 — DER RE2-KRIECHER (gebaut 2026-08-21, Nutzer-Auftrag „Na dann baue das Kriechen nach")

Der Abschnitt darunter (`🔜 W5 (Stand VOR dem Bau)`) beschreibt den Stand VOR dieser Welle und
bleibt als Herleitung stehen. Was jetzt gebaut ist:

### Die Zustand-1-Wurzel hat zwei Hälften

```
80101154: lhu v0,270(a0)      ; +0x10E
8010115c: andi v0,v0,0x3f
80101160: sll  v0,v0,2
8010116c: lw   v0,-14252(at)  ; Tabelle 0x8010C854
80101174: jalr v0
```
`table 0x8010c854 14` (eigener Dump): **alle 14 Einträge alternieren strikt auf Bit 0** —
gerade → `0x8010118C` (aufrecht), **ungerade → `0x80101210` (KRIECHER)**.

Kriecher-Wurzel `0x80101210`: DECIDE `0x8010C90C[+0x5]` (`@0x80101240`), danach `+0x5` FRISCH
gelesen (`@0x80101254`), EXEC `0x8010C918[+0x5]` (`@0x80101268`). **Je drei Worte**; ab
`0x8010C924` folgt die `(u16,u16)`-Datentabelle.

| +0x5 | DECIDE | EXEC | Rolle |
|---|---|---|---|
| 0 | `0x80102EE4` | `0x80103024` | Kriech-**Lokomotion** + Angriffs-Entscheid |
| 1 | `0x801025E4` (`jr ra`) | `0x801025EC` | **GRIFF** (dieselbe Funktion wie aufrecht [3]) |
| 2 | `0x80103A70` | `0x80103B48` | **Warten** (Clip 0x17) + Angriffs-Entscheid |

HURT: `+0x10E & 1` → 1D-Tabelle `@0x8010CBE8` → `FUN_80107888` (`@0x80104FE0-500C`,
`j 0x8010540C` = jalr + sofortiger Epilog, die normale Reaktion läuft **nicht** mehr).
DEATH: `+0x10E & 1` → 1D-Tabelle `@0x8010CECC` → `FUN_80108A14` (war schon portiert).

Die Kriech-Variante des GRIFFS wählt der Griff selbst:
`8010266c: lhu v0,270(s1)` / `80102674: andi s5,v0,0x1` / `80102690: addiu s5,s5,2` (Typ 0x17/0x11).
Der Zweig war früher als „OPEN → Bit 0" geführt; mit dem Kriecher-Brain ist er scharf.

### EXEC[0] `0x80103024` — die Kriech-Lokomotion, Instruktion für Instruktion

```
P0 @0x80103064  lui 0xf / ori 5 / sw 332      +0x14C = 0x000F0005 (Clip 5, Frame 0, Rate 15)
   @0x80103074/78                              +0x6 = 1 (Delay-Slot des RNG-jal)
   @0x8010308C-98 andi 0x7 / addiu 7 / sh 344  +0x158 = (rand & 7) + 7
   @0x80103094    jal 0x80015E7C               BARER Aufruf (füllt nur +0x144)
   @0x8010309C-BC andi 0xf / sb 333            +0x14D = rand & 0xF
   @0x801030B8    jal 0x80015E7C               zweiter barer Aufruf  -> +0x144 == 0
   KEIN Sprung — P0 FÄLLT DURCH nach P1
P1 @0x801030C0  lh 324 / slti 21 / bne         **nur wenn +0x144 >= 21** steuert er
   @0x801030D4-E0 jal 0x80015558(+0x1C4,+0x1C6, a3=24)
   @0x801030E4-F4 lhu 344 / addiu -1 / bne / sh 344   ALT==0-Test, Store im Delay-Slot
   @0x801030F8-108 (nur bei ALT==0) zweites 0x80015558(…, 24)
   @0x8010310C-1C  +0x158 = (rand & 7) + 7
   IMMER: @0x8010312C e7c, @0x80103140 959c(a3=256), @0x8010314C 152c8
```

⛔ **Clip 5 ist ein ZYKLUS, nicht play-once** — byte-abgeleitet, nicht geraten: P1 ruft
`0x8002959C` jeden Tick und **liest die Rückgabe nicht** (kein `+0x6 +=`; `@0x80103148` lädt
schon a0 fürs `152c8`). Der Executor kann P1 nie verlassen. Wo das Original play-once meint,
VERBRAUCHT es die Rückgabe (EXEC[11] P1 `@0x801044F8-508`, Kriecher-HURT P1 `@0x80107A40-50`).
Gemessen, warum das zählt: mit play-once pinnte der globale Advancer den Kriecher auf
`clip=5 fr=49` — `+0x144` blieb 0 und der Kriecher stand 2100 Frames bewegungslos.

### EXEC[2] `0x80103B48` — das Warten, komplett (neun Instruktionen)

```
80103b48: lbu v0,6(a0)      80103b50: bne v0,zero,0x80103b64   ; +0x6 != 0 -> nichts
80103b54: addiu v0,zero,1   80103b58: sb v0,6(a0)              ; +0x6 = 1
80103b5c: addiu v0,zero,23  80103b60: sw v0,332(a0)            ; +0x14C = Clip 0x17, Rate 0
```
Kein Ausgang — nur DECIDE[2] holt den Kriecher hier wieder heraus.

### Kriecher-HURT `FUN_80107888`

P0 `@0x801078F4`: `+0x14C = 0x00030006 | ((rand & 7) << 8)` (Clip 6, Zufalls-Startframe, Rate 3),
Blut-Id 6000 an Part 0 (`@0x80107938`), SE 12 mit `+0x239`-Sperre, dann die Zeilen-Gore-Leiter
10 / 11 / 14 / 16 — **ohne** den Ruß-Zweig 9/17, den die Liege-/Death-Leiter hat (deshalb ist
`re2z_dismember_row` hier NICHT wiederverwendet). Fällt durch nach P1.
P1 `@0x80107A2C`: `+0x6 += 0x8002959C(a3=1024)`.
P2 `@0x80107A54-58`: `sh 1,270` **und** `sw 1,4` = Zustand 1 / Sub 0 / Kriecher — zurück in die
Lokomotion. Der `sh` ist nackt und wischt alle anderen `+0x10E`-Bits weg.

### `+0x144` ist ein LESBARES FELD

`FUN_80015E7C` legt den Wurzel-Delta-Vektor in der Entity ab, BEVOR `FUN_800152C8` ihn anwendet:
```
80015fcc: subu v1,t1,v1     ; dx = sx(kf_now) - sx(kf_prev)
80015fd8: sh   v1,324(t0)   ; +0x144 = dx   (UNROTIERT)
80015fdc: sh   a0,326(t0)   ; +0x146        80015fe4: sh a1,328(t0)  ; +0x148
```
Ein Port, der `e7c+152c8` zu EINER Delta-Anwendung fusioniert, muss den Zwischenwert trotzdem
ablegen — sonst hat der Kriecher gar kein Steuer-Kriterium. Neues Aktor-Feld `re2z_root144`.

### *** 0x800CFDCB IST KEIN RAUM-GLOBAL ***

Die alte Notiz nannte es „Einmal-Riegel PRO RAUM ohne Port-Produzenten". Falsch — es ist ein
**Feld des Spielers**. Dieselbe Adresse, einmal absolut und einmal basisrelativ:
```
@0x80102F34-38  lui s2,0x800d / addiu s2,s2,-1032   ; s2 = 0x800CFBF8 = Spieler-Entity
@0x80102FB0     lbu v0,-565(lui 0x800d)             ; = 0x800CFDCB
@0x80102FF8     lbu v0,467(s2)                      ; = 0x800CFBF8 + 0x1D3 = 0x800CFDCB
```
Dass `0x800CFBF8` die Spieler-Entity ist, belegt dieselbe Funktion doppelt: `+56 = 0x800CFC30`
(Spieler-X, `@0x80102EF4` vs `addiu a1,s2,56`) und `+0x106 = 0x800CFCFE` (Etage, `@0x80102F68`).

⇒ **`0x800CFDCB` = Spieler+0x1D3, Bit 0x80 = der globale EIN-ANGREIFER-RIEGEL.**
Setzer: jeder Kriech-/Lunge-Commit (`@0x80102FB8`, `@0x80103000`, `@0x80103B24`, `@0x801045A8`).
Löscher: `andi 0x7f` `@0x80104FA0/AC` (HURT-Grab-Abbruch) und `@0x801082E8/F4` (DEATH-Abbruch).
Der Port hat das Feld pro Aktor (`re2z_self1d3`) — der Riegel ist schlicht das Feld des
SPIELER-Aktors. Kein erfundenes Global.

### Der Kampf-Eintritt in EXEC[11] P1 ist jetzt scharf

```
80104530: jal 0x80015614(PL.x,PL.z,a3=256)   80104538/3c: ret16 != 0 -> raus
80104544/4c/50: +0x1F0 < 0x708               8010455c/64/68: Spieler+0x1D3 & 0x80 -> raus
80104570-80: +0x106 != Spieler+0x106 -> raus
80104584/8c: +0x4 = 0x101
80104590-98: andi 0xffc0 / ori 1 / sh 270    = die KRIECHER-WURZEL
8010459c-b0: Spieler+0x1D3 |= 0x80
```
`andi 0xffc0` ist die **einzige** `0xffc0`-Maskierung im ganzen Overlay (eigener Voll-Scan).
Früher stand hier „wird NICHT genommen, weil der Port weder einen Kriecher-Brain noch einen
Produzenten für 0x800CFDCB hat" — beides ist jetzt da.

### Die zwei Brücken (bewusst, benannt, nicht erfunden)

**(1) Auslöser.** ROOM1030 fordert das Kriechen über das RE1.5-Protokoll an (sub07 `@Datei 0x2754`
setzt `+0x1C4 |= 0x1000`). Das RE2-Modul hat dafür **keinen Konsumenten**: alle 36 `+0x1C4/+0x1C6`-
Zugriffe in `EMOVL10_S0.BIN` sind das Paar `lh a1,452 / lh a2,454` = das Steuer-Ziel; die acht
`andi 0x1000` (`@0x80101DDC`, `@0x80101E5C`, `@0x80101E9C`, `@0x80102490`, `@0x801024E8`,
`@0x80102528`, `@0x80105E18`, `@0x80108B44`) lesen **alle** `+0x21A` (`lhu v0,538`), nicht
`+0x1C4`. Die Brücke (`D15.3` in `re15_re2z_tick`, wie der D15.2-Wecker daneben) übersetzt
**nur den Anforderungskanal**; die Zielwerte sind byte-gelesen:
* HIN (`+0x1C4 & 0x1000`) → `re15_re2z_enter_crawler(sub 0)` = `sh 1,270` `@0x80107A54` +
  `sw 1,4` `@0x80107A58` (die Kombination, mit der der Kriecher-HURT selbst in seine
  **Lokomotion** zurückkehrt) — nicht der Kampf-Griff `sw 0x101` `@0x8010458C`, denn das Skript
  will Fortbewegung, keinen Angriff. Der Spieler-Riegel wird dabei NICHT gesetzt: er hängt im
  Original hinter dem Kampf-Test.
* ZURÜCK (`+0x1C4 & 0x2000`, das zweite Skript-Bit; RE1.5-Gate `@0x8010369C-A4` mit
  `!(+0x1D8 & 0x80)` `@0x801036B0-BC` → Wort `0x601`) → Kriecher-Bit aus, `sw 0x901` =
  EXEC[9] Aufstehen `@0x80103E48`, SCA zurück auf 4 (`@0x801050B4`).

**(2) Tor-Zelle.** Voll-Scan von `EMOVL10_S0.BIN` nach jedem `+0x1D7`-Zugriff
(`(sb|lb|lbu) rt,471(rs)`): **NULL Treffer** — das RE2-Zombie-Overlay kennt das Feld nicht. Die
SCA-Maske ist **RE1.5-Raumdaten-Eigentum**: der Port lädt unter beiden Flavors dieselbe
RE1.5-`ROOM1030.RDT`, und deren Torzelle lässt nur die Maske 8 durch. Der Wert 8 stammt aus der
RE1.5-Kette (`@0x801050F4` / `@0x8010374C`), 4 ist die aufrechte Zeile (`@0x801050B4`).
Die Zelle selbst ist **nicht statisch**: alle 35 SCA-Zellen von ROOM1030 stehen auf `u0 = 0xFF`;
das Skript öffnet sie mit `Sca_id_set` —
`37 02 06 f7` `@Datei 0x2000`/`0x278E` und `37 03 06 f7` `@Datei 0x2004`/`0x2792` (eigener
Byte-Scan). `0xF7` = `0b1111_0111`, Bit 3 (Wert 8) als einziges frei ⇒ Maske 4 fest, Maske 8 frei.
Die beiden Torzellen sind rgn2/idx6 == rgn3/idx6, Rechteck `(-20144, -24420, 24264 x 1649)`
⇒ `z[-24420 … -22771]` — exakt der Riegel des Dossiers. `0x278E/0x2792` liegen unmittelbar an
sub07 `@0x2754`: das Skript **öffnet das Tor und befiehlt das Kriechen im selben Atemzug**.

### Gemessen — vorher / nachher

`probe_re2_crawl_gate` (gleicher Aktor, gleiche Vorbedingung, echter Tick, geladene Bänke):

| | vorher | nachher |
|---|---|---|
| RE1.5 | Sub 0x10 @T1 → Commit @T97, `grid 0x81 sca 8 Clip 0x1A` | **unverändert** |
| RE2 | `s1` bleibt 2, `grid 0`, `sca 4`, Clip 4 → kriecht nicht | `s1=0`, Clip 5, `sca 8`, `+0x10E&1` → **kriecht** |

`probe_1030_crawl_live` (echte ROOM1030.RDT + SCD-VM + `re15_game_step` + geladene Bänke,
2400 Frames, sechs gespawnte Zombies):

| Phase | RE1.5 | RE2 |
|---|---|---|
| A — reiner Skript-Weg | **5/6 geflaggt, 5/6 kriechen, 5/6 unter dem Tor durch** | **6/6 geflaggt, 6/6 kriechen, 5/6 unter dem Tor durch** |
| B — ab `Member_set(16)` | 6/6 kriechen, 6/6 `sca 8`, **6/6 unter dem Tor durch** | 6/6 kriechen, 6/6 `sca 8`, **6/6 unter dem Tor durch** |

### ✅ ERLEDIGT — der „Skript-Auslöser feuert nicht"-Blocker war ein SONDEN-Defekt (2026-08-21)

Die alte Phase-A-Zeile („0/6 geflaggt, vorbestehender Blocker") war **falsch gemessen**. Beide
damals benannten Blocker existieren nicht:

* **`work_slot == -1`** war ein Artefakt der **Handauslösung**. Der Torwächter
  `3e 00 0f 00 05 00` (`Member_cmp(15,==,5)`) und der `2e 02 i` `Work_set(kind 2, Slot i)` liegen
  **nicht in sub07**, sondern im AUFRUFER **sub06 `@0x24c8`** (20 ausgerollte 32-Byte-Blöcke, der
  letzte endet bei `@0x2740`) bzw. **sub09 `@0x27e0`**. `scd_event_fire(7)` startet sub07 als
  EIGENEN Thread und überspringt damit genau die Bindung. sub07 `@0x2754` ist nur
  `3d 04 10 / 26 00 05 04 00 10 / 35 10 04 / 01 00` — 16 Byte ohne jeden Test.
* **„`member_0b` einen Frame später gewischt"** war derselbe Fehler von der anderen Seite: ein
  **von Hand** gesetzter Stempel ohne echten Zonen-Treffer fällt im byte-true Aktiv-Clear
  (`@0x80043704 andi v0,v0,0x1` / `@0x8004371c sb zero,0(at)`) auf 0 zurück. Der Aktor-Stempel
  IST implementiert (`re15_aot_stamp_entities`, `aot_common.c:702`) und vergibt live `+0x0B = 5`.

Die eigentlichen Ursachen lagen alle drei in `probe_1030_crawl_live` selbst:
1. Die Sonde tickte die **SCD-VM nie**. `scd_vm_tick()` hängt NICHT an `re15_game_step`; die
   Hauptschleife ruft sie selbst und VORHER (`platform/pc/main.c:3578`) — byte-true die
   Reihenfolge des Originals `@0x8001cdec jal 0x8003f038` (VM) vor `@0x8001ce04` (Gegner-AI),
   `@0x8001ce0c` (Spieler), `@0x8001ce1c` (AOT-Scan). Beleg: `RE15_SCD_TRACE=1` über 2900
   Sonden-Frames = 474 Zeilen, alle aus dem Raum-Init.
2. Die **LOCO-Bank** fehlte (`re15_emd_parse_loco_bank`). `re15_enemy_footlock_step` steigt bei
   `!bank->loco_ok` sofort aus (`enemy_ai_common.c:2567`) → die Zombies animierten auf der Stelle
   und erreichten die AOT-Zone 5 nie → `flag(5,0x22)` nie → sub01 nie.
3. **`g_room_rdt`** war leer → `re15_collision_constrain_enemy(&g_room_rdt, …)` klemmte nicht →
   das Rolltor war offen.

Gemessen über den echten Weg (Spieler steht im Auslöse-Rechteck AOT-3, sonst kein Eingriff),
RE1.5: `f5` Stempel `+0x0B = 5` → `f7` `flag(4,0x0f)` (sub01 `@0x2198`) + `Evt_exec` sub08 →
`f7` `+0x1C4 |= 0x1000` → `f103` Kriech-Commit (`grid 0x81`, `sca 8`) → `f457` `flag(5,0x14)`
(sub08-Ende; Hardware-Soll t+450, `tools/redux/crawl_cycle_out.txt`). RE2: `f21/f23/f23/f473`.
Pin + vier Negativproben: **`unit_1030_trigger_chain`**.

### ⛔ OFFEN (benannt, nicht gefüllt)

1. **Kriecher mit `+0x5 > 2`.** Erreichbar über den Wurf-Ausgang des Griffs
   (`addiu v0,zero,1281` `@0x80102D24` / `sw v0,4(s1)` `@0x80102D2C`, Grab-Phase 8), der NICHT
   auf `+0x10E` gegated ist. Das Original würde `lw 0x8010C918[5]` = `0x8010C92C` = **`0x8040006E`**
   laden und `jalr` darauf ausführen — ein Datenwort. Der Zustand kann im Original also nicht
   vorkommen; welche Vorbedingung ihn dort verhindert, ist noch nicht gefunden. Der Port fährt
   für `+0x5 > 2` weiter die **aufrechte** Tabelle (= exakt das Verhalten vor dieser Welle) —
   kleinstmögliche Abweichung, kein Freeze, kein erfundener Zweig. Ohne diesen Rückfall blieben
   in `test_re2_zombie_abc` 16 Aktoren mit `s1=5 s2=1 10E=0x2001` unsterblich stehen (gemessen).
2. **Der zweite Umbauweg** `+0x21A & 0x10` → `FUN_80107A78` (`@0x80105014-38`): eigene Welle,
   im Port ohne Produzenten für Bit 0x10 unerreichbar.
3. **EXEC[2] (Warten, Clip 0x17)** hat im Original gar keinen `0x8002959C`-Aufruf; der globale
   Port-Advancer spielt den Clip trotzdem einmal durch und pinnt den letzten Frame. Ein Halt-Pin
   wie beim Schläfer-Clip 0x2A wäre nötig — heute folgenlos, weil kein Produzent Kriecher-Sub 2
   schreibt.

**Tests:** `unit_re2_crawler` (Eintritt / Kriech-Bewegung / Tor-Transit / Ausstieg /
Grab-Entscheid + vier Negativproben + RE1.5-Regressionswache), `unit_1030_crawl_live` (die echte
Raum-Messung oben), `unit_1030_trigger_chain` (die volle Auslösekette über den echten Weg in
beiden Flavors + vier Negativproben für die beiden Hälften des Gatters `@0x2180`).

---

## 🔜 W5 (Stand VOR dem Bau) — Kriecher, State-8-Eintritt, Dismemberment

Belegt bisher: die Kriech-Variante nutzt **eigene** Tabellen (`0x8010C90C` Decision /
`0x8010C918` Executor, nur drei Einträge), und `sub_state_1 == 1` teilt sich den Handler mit der
aufrechten Variante. Korrekturen aus Lane Z: der Grab-15er geht auf **SELF**+0x1D3
(`@0x80102770`), und es GIBT Overlay-Claim-Löscher (HURT `@0x80104FAC`, DEATH `@0x801082F4`) —
die alte „nur Spieler-seitig"-Aussage oben ist damit präzisiert.

### W5 ist der Blocker für den ROOM1030-Kriechtor-Report (2026-08-21)

Nutzer-Report (RE2-Modus): *„in der Lobby kriechen die Zombies in der Cutscene nicht unter das
Tor (ROOM1030)."* **Gemessen** (`probe_re2_crawl_gate`, gleicher Aktor, gleiche Vorbedingung
`anim_flags |= 0x1000`, echter Tick-Einstieg `re15_enemy_ai_live_tick`, echte Bänke):

| Flavor | Ergebnis |
|---|---|
| RE1.5 (EM016, 43 Clips) | Tick 1 → `+0x5 = 0x10`, Clip 0x12 · Tick 97 → `grid = 0x81`, `sca = 8`, Clip 0x1A → **kriecht** |
| RE2 (EM016, 31 Clips) | bleibt `+0x5 = 2`, `grid = 0`, `sca = 4` → **kriecht nicht** |

Ursache: die ROOM1030-Kette ist ein **RE1.5-Protokoll** — AOT-Stempel `entity+0x0B` → SCD sub07
(`@Datei 0x2754`) setzt `entity+0x1C4 |= 0x1000` → die drei RE1.5-Steer-Funktionen
(`@0x80101ecc` / `@0x801021d4` / `@0x80105798`) machen daraus `+0x4 = 0x1001` → Sub-Modus 0x10
(`FUN_80104f80`) → Grid-Wurzel 1. Unter RE2 übernimmt `re15_re2z_tick` den **ganzen** Dispatch
(`enemy_ai_common.c:4232` / `:8131`), und das RE2-Modul hat für dieses Protokoll **keinen
Konsumenten**: eigener Byte-Scan über `EMOVL10_S0.BIN` findet 36 Zugriffe auf `+0x1C4/+0x1C6`,
**alle** als Paar `lh a1,452(r) / lh a2,454(r)` = **Steer-Ziel (x,z)** (z.B. `@0x80104400/04`) —
RE2 belegt dieses Feld völlig anders.

**Die RE1.5-Maschine ist unter RE2 NICHT wiederverwendbar** (zwei unabhängige Blocker):
1. **Clip-Indizes.** Sie adressiert Bank-1-Clips 0x12/0x1A. Gemessen: RE1.5 EM016 0x12 = 98
   (Hinlegen), 0x1A = **99** (Kriechen); RE2 EM016 0x12 = 67, 0x1A = **1**. Unter RE2 trägt der
   Aktor die RE2-Bank (die Animation bleibt im RE2-Modus RE2, `re15_ai_flavor.h`) —
   es liefe ein 1-Frame-„Kriechclip".
2. **Advance-Ordnung.** Der globale Advancer (`player_common.c`, `game_step:1304`) läuft VOR der
   KI (`:1324`) und lässt die Kriech-Zustände **nur unter RE1.5-Flavor** aus
   (`player_common.c:743`). Die RE1.5-Kriechkette advanct selbst → unter RE2 doppelte Cliprate.

**Der RE2-eigene Kriecher taugt für dieses Tor ebenfalls nicht als Fertigteil:** DECIDE[0]
`0x80102EE4` ist byte-gelesen ein reiner **Kampf**-Entscheider (`sltiu s0,s0,0x514`
`@0x80102F3C`, `0x800CFDCB & 0x80` `@0x80102F4C`, Etage `+0x106 == 0x800CFCFE`
`@0x80102F60-70`, Sektor `jal 0x80015758` `@0x80102F98` → `+0x4 = 0x101` `@0x80102FA8` = GRAB,
danach `0x800CFDCB |= 0x80` `@0x80102FB8`); EXEC[0] `0x80103024` steuert auf den **Spieler**
(`jal 0x80015558` `@0x801030DC` / `@0x80103104`). Kein Tor-Transit, kein Skript-Eintritt, kein
Aufsteh-Exit. Ein tragfähiger Fix braucht also **W5 komplett** (Wurzel `0x80101210`, DECIDE
`@0x8010C90C` = {`0x80102EE4`, `0x801025E4`, `0x80103A70`}, EXEC `@0x8010C918` =
{`0x80103024`, `0x801025EC` = Grab, schon portiert, `0x80103B48`}, Konvertierung `0x80107A78`,
HURT-Tabelle `@0x8010CBE8`) **plus** ein deklariertes MAPPING für den Auslöser
(RE1.5-Member16-Bit 0x1000 → `+0x10E |= 1`, im Original nur EXEC[11] P1 `@0x80104590-98`)
**plus** `+0x1D7 = 8`, damit die Tor-Zelle (`Sca_id_set 0xF7`) passierbar wird.

---

## ✅ WELLE C — der RE2-HUND (Cerberus, kind 0x20), 2026-08-16

Quelle: `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN` == `CDEMD0.EMS[0x25B000..+22266]` (EXE-TOC
`@0x8009ABF4` Typ 0x20 ModB = Sektor **1206**, 22266 B; Modul lädt RAW `@0x80100000`, ID-Wort 6).
Vollständige EIGENE Disassemblierung 2026-08-16 (`re2_disasm.py --bin EMD0G_MOD0.BIN`, Dump
`scratchpad/dog_self.asm`, 5326 Instr) — jede Konstante im Port trägt ihre Adresse aus diesem Dump.
Port: `engine/src/enemy_ai_re2_dog.c` (`re15_re2dog_tick`), Hook am Kopf von `re15_dog_ai_tick`
(enemy_ai_common.c), Ownership `re15_re2_owns_type` (= Zombie-Familie + 0x20). Der RE1.5-Hund
(Root 0x8010d7f8) bleibt byte-identisch Default.

### Struktur (byte-belegt)

* Root `@0x80100004`: Freeze-Gate `0x800CFBDC&0x20000000` (`@0x80100010-20`), `+0x1D3`-low7-Dec
  (`@0x80100028-3C`), `+0x232`-Dec (`@0x80100040-50`), Dispatch `+0x4` → Tabelle **@0x80105438**
  {INIT `0x801000F4`, ACTIVE `0x801004DC`, HURT `0x801032A8`, DEATH `0x801040DC`, [4] jr-ra-Stub
  `0x801049E4`, CORPSE `0x801049EC`}. Epilog: Stuck-Zähler `+0x230` (`@0x80100090-D8` —
  Pos unverändert → ++; bewegt UND `+0xD==255` → 0; der Report-Wortlaut war hier ungenau,
  selbst nachdisassembliert).
* ACTIVE `@0x801004DC`: Dispatch `+0x5` → **17 Substates @0x80105464**; Tail: EDD-Frame-Wort
  (`+0x178`) Bit `0x40000` → **SE 8** (`@0x80100510-30`).
* INIT `@0x801000F4`: `state=1` ZUERST (`sw 1,4 @0x8010012C`); **HP-Roll** = 2 Draws
  (`rand&3` + `rand&0xf`) über 4 Tabellen je `0x800CFB74`-Bits 0x40/0x20: `@0x80105340`
  {119,85,…,59} (Default — die Bits haben keinen Port-Produzenten), `@0x80105360`, `@0x801053B0`,
  `@0x801053D0`; Hitbox 500/−1000/1000 + 600er (`@0x80100284-C4`), Part-Metadaten-Loop läuft
  **a0=2..3** (2 Parts, nicht 4 — Lane-D korrigiert, `@0x801002D0-F8`); Spawn-Param `+0x10E&0x7F`
  → Start-Sub-Tabelle `@0x80105458` {0,1,2,4,10,12,14,13,15}; Start-Clips je Raw
  (`@0x8010032C-480`); Callback-Install `0x80104ACC→0x800CE480` (`@0x801004A8-B4`, Zweck OFFEN).
  RE1.5-Räume tragen kein `+0x10E` → Port-MAPPING: alle Spawns Raw 0 (Sub 0, Clip 1 Zufalls-Frame).

### Executor-Tabelle (alle mit @-Zitaten im Code)

| Sub | Adresse | Kern (Kurz) |
|---|---|---|
| 0 IDLE | 0x80100784 | Wander-Helfer 0x80100548 (Speed 30, Kontakt→Pause 90f/Roam); Wake: gleiche Etage + dist<4000 / Noise 0xFBF4&0x20 → Sub 1 |
| 1 STALK | 0x80100824 | Speed 30; Ziel=Spieler+Offset tbl@0x801054A8 (2 Draws); **Yaw += Arc(16)** direkt; Aggro `+0x16A` +1/Tick (<3000), **+21 rennend**, −1 fern, +20 fern+rennend+im Kegel; **≥65→RUN**; Angriff: Kegel 128 UND **dist≥601** (sltiu 0x259 SKIPPT bei kleiner — Lane-D "<601" war falsch herum) → Sub 3; Drool FX(4,6) Budget tbl@0x80105420 |
| 2 RUN | 0x80100B20 | P0: Clip 2, Turn 96 bzw. tbl@0x801054D8 wenn Rudel (0x8001A5C0 = zählt andere kind-0x20 — selbst RE'd); P1: +40→240, Arc(+0x224), Unstick ≥16, Decision 0x80104CD8 (>0→Sub 3, −2→Sub 8+Fat+10, −4→Abort-Flag); **Fatigue ≥451 → Abort IMMER** (`sb v0,4(s1)` @0x80100E10 = Branch-Ziel UND Fallthrough, Delay-Slot `addiu v0,1` @0x80100E08), **Fatigue-Clear NUR bei Spieler-HP≥21** (`sh zero,2(s1)` @0x80100E0C, Review-Fix #1); P2: Halbkreis (Akku ±2048); P3: Kreisen (Route OFFEN→Arc-Steuerung), Attack-CD tbl@0x80105430+30 |
| 3 ANGRIFF | 0x801011EC | P0: Launch 0x80101380(65,−280) → 8f Windup → **Speed 280, vy −280, +0x219=1, SE 3, Noise 0xFBF4|=0x20**; P1 Flug: **Gravity +40** (0x80015350-Paar), Kontakt-Probe; P2 Landung: Boden `+0x1C2`, SE 10, Ramp-down(64,240); P3 Boden-Biss: Clip 22, Dreh-Biss Yaw+=128 bis 2048, Budget 4 |
| 4 HEULEN | 0x80101830 | Gate 3500/**5000 wenn Ziel-Zustand 0x0201 = RENNT** (513 ≡ state1/sub2-RUN — Lane-D-Etikett „gepackt" korrigiert); **Claim-Bit 0xFBF4&0x80** (einer heult, Rest wartet tbl@0x80105430); Heul-SE = EDD-Event-Bit → SE 8 |
| 5 Knurren/Fressen | 0x80101B14 | Clips 3→4 Rate 3; SE 4 beim Übergang (**ENEMSE-Map-Eintrag 20 = 0xFFFFFFFF = byte-true STUMM**); Ende → 0x201 |
| 6 Rückzug | 0x80101BDC | Clip 9, Speed 50, Bewegung Yaw+2048, Steer 64; Ende → 0x201 |
| 7 LATCH | 0x80101CDC | s. unten |
| 8 Break-off | 0x8010224C | tbl@0x80105428+30 Frames Wegrennen, **Fatigue +300**; → Sub 2 |
| 9 Etage verloren | 0x8010237C | Clip 0, 60f → Sub 0 + Clip 1; Eintritt 0x80105234 (cleart Noise-Bit) |
| 10 Aggro-Idle | 0x80102404 | wie 0, weckt in Sub 2 |
| 11 Rudel-Hilfe | 0x80102490 | Eintritt 0xFBF4&0x40 (0x80105274); rennt zum Latch-Ziel (Port: Spieler), Steer 16, Stop <2500 |
| 12 Post-Latch | 0x80102608 | Clip 14 Schütteln + Heul-Logik (P3-Reuse 0x80101A58 → Sub 1); Blut über **EDD-Bits 0x30000**, Frame-SE 0x80016028 |
| 13 Zickzack-Flucht | 0x801028AC | verwundeter Spawn: Kauern aus tbl@0x8010550C = **{11,26,11,26,26,26,11,26}, beide Leser `andi 0x7`** (@0x80102968/@0x80102A0C — Review-Fix #2, P(26)=5/8) ×(rand&3+1), Flucht Speed 240 Yaw±127, Band-1920-Roll → Sub 15 |
| 14/15 Fenster-Sprünge | 0x80102C28/0x80102E58 | Launch(65,−320)→280+SE 3; 15: Wand-Abpraller (Speed −½, Spin 256) → Sub 13 |
| 16 | 0x801030FC | **Eintritt OFFEN** (Skript-Kette) — Port: recover → Sub 2 |

### Biss/Latch/Schaden (der Lane-D-„EXE-Gap" ist GESCHLOSSEN)

* Kontakt `0x80104DF0`: Gates `+0x21E==0`, Ziel unclaimt, **Jaw-Nähe 0x800157D4(Part 4, Radius
  700 wenn PL-HP<21 sonst 1000 — `@0x80104E34-48`)**, Y>Boden−1800, gleiche Etage. Dann
  Fatigue=0 (`@0x80104EB4`) + **`FUN_800401D4(20, 0)` `@0x80104EB8-C0` — der Biss-Schaden 20
  STEHT IM MODUL** (a0 = Schaden; die Funktion ist voll RE'd: Rückgabe 0=überlebt /
  1=One-Save (HP→0 + Latch `0x800CFD4C|0x1000`) / **2=Tod (HP<−14)**; Skalierungen ×1.5/×5/×2
  hängen an RE2-only-Globals — wie Welle B nicht übernommen).
* **Rückgabe 2 → LATCH** (`@0x80104EE0-F74`): der Wurf-Biss ist der FINISHER. `+0x21E=2`,
  PL`+0x1D3`=255, PL-Flags|=0xA, **PL-Yaw = Hund+2048**, self claim. Touchdown → **Sub 7**.
* Sub 7 `@0x80101D1C`: Clip **23** (145 F), P0 ankert das Opfer BEREITS (`jal 0x80015B94`
  @0x80101DA0 — Review-Fix #12); **Mash-Fenster `+0x16A`=12** — der Zähler **SATURIERT bei 0**
  (Review-Fix #4: `beq v0,zero → Release-Block OHNE Dekrement` @0x80102008-24; Dekrement nur
  auf den Pfaden ctr≠0 @0x80102020/@0x801020C4 oder Mash-Bit @0x801020A4-C4), der Release-/
  Gravity-Block (@0x80102070-A0) läuft ab ctr==0 JEDEN Tick → der Hund kommt während des
  Bisses auf den Boden; PL`+0x8`-Bit0 = Spieler-Struggle-Signal → Port-MAPPING
  `re15_mash_pressed`; Latch-Globals `0x800CFDAC/0x800CFBFC=6/0x800CFD80/84/0x800CFC6E`;
  Frame-98-Event: **Budget 18** + Part-4-Blend 96/0x101010 (Render-OFFEN); Frame 102:
  Part-Liste `@0x80105508` {8,0,9,12}; Release-Flags `0x800CFB74|=0x4000100` +
  **Rudel-Signal 0xFBF4|=0x40** (room-persistent: einziger EXE-Clear = Room-Init FUN_80052f3c
  → das Rudel PARKT danach byte-true in Sub 11, der keinen Exit-Commit hat);
  Clip 23→**24** (Hang-Loop)→**25** (+0x158=2) → **Sub 12**. Die „SE-Stapel"-Kette `@0x80101E18-EB4`
  (0x8003947C/0x80039514/0x800395B8) ist **PAD-RUMBLE** (beide Funktionen decompiliert:
  Aktuator-Ringpuffer `DAT_800EAAD8/800EAC28`, Envelope `(p3−p2)*0x80/p1`) — kein Port-Rumble,
  dokumentiert; die hörbaren Latch-SEs kommen über `0x80016028` (EDD-Bit `0x8000000` →
  SE = Wort>>28).
* Spieler-Seite (Port): Victim-Shim (Welle-B-Infra); RE2-Victim-Bank = EMD-Paar 3 (dir[5]/[6]:
  **1 Clip, 145 F, 15-Bone-Spieler-Pool** — synchron zum Hunde-Clip 23); Mash im Fenster =
  One-Save-Analog (HP→0 + Throw-off), sonst Devour-Kollaps (MAPPING — der EXE-seitige
  Grab-Art-6-Spielerhandler ist nicht RE'd, s. OFFEN).

### HURT/DEATH/CORPSE

* HURT-Router `@0x801032A8`: `+0x223&0x80` → Knockdown-Maschine (`0x801038C0`, Phasen
  `@0x801055AC`, Tick-Tail 3×FX); sonst Zeile `+0x5` → Tabelle `@0x80105538` (Sonder-Zeilen [0]
  Schlaf-Landung, [4]/[15] Luft-Kick vy=−140/Speed 240 `@0x80104030`, [9]/[10]/[16] Gore-Budget
  2−hits/3 bzw. 4−hits/3 + FX 8, [11] FX 9, [14] Glas-FX 11 — die FX-Kinds ohne RE1.5-Pendant
  sind stumm-OFFEN, die Budgets byte-true). Generisch `@0x80103344`: Knock-Richtung =
  **Spieler-Yaw − self-Yaw + Jitter `@0x8010559C`** {128,−192,−128,192,−256,256,256,−256},
  SE 1, Clip **17**, Knockback **240** (Luft: **halbiert** + P1-Fall), `+0x15A=512`,
  `+0x223=1`; **zweiter Treffer bei `+0x223≠0` → `|0x80` + Knockdown-Entry 0x80103950 im
  SELBEN Tick** (`jal @0x8010340C`, via `j 0x801034A8` am Router-Tail-FX vorbei —
  Review-Fix #6; Herkunfts-Zeile aus `+0x5` = prev_sub-Schnappschuss).
  Erholung P2 → frontal `0x501` (Knurren) / sonst `0x601` (Rückzug) / verwundet `0x10D01`
  (Flucht); P3 (2/16 sanft) → Clip 22 ab F15 → `0x201`.
* DEATH `@0x801040DC` (Zeilen `@0x801055CC`): Kern `0x80104178` = EXE `0x80018FB0` +
  P0-Reuse + **SE 7 einmalig über `+0x231`** (`@0x801041B8-D4`); Gore-Variante `0x80104694`
  ([0]/[5]/[6]/[9]) SETZT `+0x231` (unterdrückt den Schrei) + Part-Scatter `0x80104440`
  (7 Parts `@0x80105680` {2,3,4,7,8,9,10}, Flags|0x4A, Vel 800/−150/10/−100 = GIB —
  Render-OFFEN) + FX 7 Budget 18; Rutsch-Phase `0x80104200` → **`sw 7,4` = CORPSE**
  (`@0x80104290`); Phasen-Zeile [3] `0x801037E8` = HP-Re-Roll („spielt tot") byte-true
  mitgeführt, Port sichert nach Corpse ab (kein Wiedergänger-Produzent).
* CORPSE `@0x801049EC`: `+0x5=1` + word0|=2; Blut-Lache `0x80104A2C`: Modell-Farbwörter
  `(&0xFF000000)|0x00BFBF10`, **Textur-U/V += 8/Frame, 90 Frames** — Farb-/Scroll-Kanal ist
  Render-seitig (Modell-Pool `+0x16C`) → OFFEN, die Timer-Maschine läuft byte-true.

### SE-Map + ENEMSE-Bank (selbst bestimmt)

* SEs via `0x8005BD6C(id, self)`: 1 Jaulen (Hurt/Knockdown), 3 Angriffs-Bark, 4 Fress-Biss
  (**Map-stumm**), 7 Todesschrei, 8 EDD-Event (Heulen u.a.), 10 Lande-Thud, 11 (Luft-Landung),
  EDD-Top-Nibble-SEs via `0x80016028`.
* **Bank-Bestimmung**: Raum-Paar-Tabelle `@0x800A7400` (EXE-Datei 0x97C00, 73×2, selbst
  gelesen): kind 0x20 steht in **13 Zeilen immer als ZWEITER kind** → Hunde-SEs =
  **zweite Map-Hälfte (flag2000, se_id+0x10)**. Zeile **31 = {0x00, 0x20}** = reine Hunde-Bank.
  EDT-Map-Probe über alle 73 Bänke: jede Hunde-Paar-Bank trägt die IDENTISCHE zweite Hälfte
  (live ids {0..3,6..14}; **4/5 = 0xFFFFFFFF stumm**). → Default **Bank 31 + flag2000=1**;
  `RE15_RE2_DOG_SE_BANK` übersteuert (A/B-Hören).

### Assets

* EXE-TOC Typ 0x20: ModA=(1195,22266), ModB=(1206,22266), TIM=(1217,66592), EMD=(1250,121144) —
  deckt sich mit `gen/re2_ems_toc.inc` (verifiziert). **Der Welle-A-Splitter passt UNVERÄNDERT**:
  `EM_TYPE20.EMD` dir@0x1D918, count 8; Paar 1 (dir[1]@0xC/dir[2]@0xEBC) = **27 Clips,
  17 Bones, 0x5C B/Frame, 912 kf == EDD-Summe** (Haupt- UND Loco-Bank); Paar 2 (dir[3]/[4])
  **leer** (EDD 0 Clips → own_ok=0, bricht die Bank nicht); Paar 3 (dir[5]@0x15748/dir[6]) =
  **1 Clip à 145 F, Pool 15 Bones/0x50** = die Spieler-Victim-Choreo; dir[7] MD1 nObj=34 =
  17 Parts. Live-Log: `[enemy] RE2 EM020 loaded: 17 meshes, 17 bones, 27 clips`.

### Verifikation

* `test_re2_dog_ai` (Unit, 9 Blöcke): INIT/HP-Tabelle/Start-Clip, Wake-Gates + Sub-9-Kette,
  STALK Speed 30/Offsets/Aggro(+21→RUN in ≤4 Ticks), **Angriffs-Gate dist≥601** (beide
  Richtungen), RUN-Beschleunigung 40→240 + Turn 96/Rudel-Tabelle, Fatigue 451 + HP<21-Reset,
  Launch (65/−280→280, Windup 8, SE 3, Noise weckt Partner, Gravity +40, Landung Clip 21 +
  Fatigue+20 + SE 10), **Biss 20 HP** + Boden-Biss vs. **Tod-Latch** (Yaw+2048, Mash 12,
  Clip 23), HURT (Clip 17/240/SE 1/Doppel-Treffer-0x80), DEATH (SE 7 einmal → CORPSE HP −1),
  Heul-Claim (einer heult, zweiter wartet), Bank 31 + flag2000 an jedem SE.
* `test_re2_room1190_ab` (echter Raum; die Hunde-Welle lebt in **sub13** — die drei
  `Sce_em_set` type 0x20 behavior 0x40 @RDT 0x2900/14/28, selbst gescannt; der B-Lauf lädt
  die ECHTE EM020-Bank → Original-Clip-Längen): RE1.5-Baseline unverändert (3 Hunde,
  grid-0x40-Pounce-Halter); RE2: INIT-Remap, Wake F0→RUN F4→Sub 3 F17→Absprung 280 F25,
  **Biss-Drop exakt 20**, Latch + Yaw+2048 + **echte Auflösung in Sub 12 + Boden-Kontakt am
  Latch-Ende** (Gravity-Block, Review-Fix #8 — die alte Assertion war vacuous),
  Flinch→**Knockdown im selben Tick**→CORPSE HP −1, 600-Frame-Lauf (Rudel parkt danach
  byte-true in Sub 11, als gültiger Zustand gewertet).
* **ctest komplett: 130/130** (Basis 128 + 2 neue).
* **Review-Pass (16 bestätigte Funde, alle gefixt oder als OPEN belegt)**: Fatigue-Zweig
  entinvertiert (#1), Cower-Tabelle 8/&7 (#2), Anim-Advance = echte 959c-Semantik (Wrap auf 0
  + done nur am Wrap-Tick @0x80029B28-4C, frac-Decay @0x800299C0-CC — vorher fror jede
  Loop-Anim ein und die Lean-Maschine war tot, #3), Mash-Saturierung bei 0 (#4), Lean-Wechsel
  unbedingt / nur Reseed gegated (Delay-Slot `sb v1,332` @0x80104CC4, #5), Knockdown-Entry im
  selben Tick + prev_sub-Zeilen (#6), +0x1D2-Produzent als OPEN belegt (#7), A/B-Assertion
  gehärtet (#8), Ownership-Kommentar korrigiert (#9), +0x22D-Clear @0x801005E0 (#10/#14),
  Launch-Zitate 0x8010139x (#11), P0-Anker @0x80101DA0 (#12), Kopf-Tracking 0x80104B98 als
  OPEN belegt (#13), FX-Yaw-Spread pro Tabellen-Eintrag zentriert (`lhu 2(s1)` @0x80105100,
  Maske/Zentrierung @0x8010510C-120, TARGET-Yaw `lh 118(s0)` @0x80105104, #16).
* **Live-Smoke** (echte EXE, `RE15_AI_FLAVOR=RE2`, `RE15_DEBUG_JUMP=1190@gp`,
  `RE15_FORCE_EVENT=13@2000` = die sub13-Hunde-Welle): `[enemy] RE2 EM020 loaded: 17 meshes,
  17 bones, 27 clips`, 3 Hunde ticken das RE2-Brain (`[re2dog] slot 1..3 state=1 sub=0/0→0/1
  clip=1 spd=30`), HP-Rolls 71/59/86 = Tabellenwerte 70+1/59+0/85+1. (Anfahrt/Kampf live =
  A/B-Test-Pfad; Hör-/Sichtprüfung braucht eine Display-Session.)

### OFFEN (Welle-C-Ausgang)

* Navigator `0x8004A808`/Route `0x8004AA50` (`[0x800CE324]`) — Steuerziel = Spieler (MAPPING,
  wie Welle B W1); RUN-P3-Wegpunkt-Advance (sqrt `0x8008D2F4`, <1000) folgt mit dem Navigator.
* Spieler-seitiger Grab-Art-6-Handler (EXE): Mash-Semantik von PL`+0x8`-Bit0 und die Wirkung
  von `0x800CFB74|0x100` — Port-MAPPING (Mash=One-Save, sonst Devour); RE2-Victim-Paar-3-Clip
  wird über die Port-Victim-FSM gefahren (Hold/Release/Collapse der RE2-Spieler-Seite EXE-OFFEN).
* Partner-/Zweitcharakter-Zweige: `+0x227`-Ziel `[0x800CFE18]`, Partner-Biss `0x80065B9C`,
  `+0xD`-Partner-Index, `0x800CFBD8&0x10000000`-Gate — kein Port-Produzent, Zweige tot dokumentiert.
* Sub-16-Eintritt (Skript-Kette), Callback `0x800CE480`-Konsument, 24×u16-Tabellen
  `@0x80105380/0x801053F0` (keine Modul-Xrefs), zweite EXE-TOC `@0x8009CFA0`.
* ~~`+0x1D2`-Produzent OPEN~~ → **GESCHLOSSEN (Welle-D-Nachtrag, fix_1d2_spec)**: `+0x1D2` =
  **zone + 3·bracket, pro Treffer GESETZT** — beide Applier selbst disassembliert (Projektil
  FUN_800470C0: Basis 1 @0x80047294-98/@0x800474B8-BC, Zone 0 @0x800472D4/@0x800474F8, Zone 2
  @0x8004730C/@0x80047530, `+3·s6` @0x80047310-30/@0x80047564-80, s6 = Hitcode>>28 @0x80047114;
  Hitscan-Applier `3·s1+zone → sb 466` @0x80041A8C-9C). **Port-Produzent**:
  `re15_re2_stamp_1d2` (re15_damage.c) — Zone über die Aim-Elevation (deklarierte Näherung
  der Schuss-Y-Regeln), **Bracket = 0 (OPEN)**: die Hitcode-Produzenten sind waffen-/pfad-
  spezifisch (Bowgun `0xC` @FUN_80046304:153, GL `(+0x1b)+0x30009` @FUN_8001ed9c:44/47,
  Sparkshot `0x2002000A` @FUN_80020758:28; Hitscan-s1 nicht RE'd). Das Welle-B-Zombie-
  `hits1d2++` ist ENTFERNT (Blut-Gate `%3==0` = ZONE 0 = Bein-Treffer); Hunde-Gore-Zeile 9
  bleibt mit Bracket 0 unterdrückt (= RE2-Nah-Treffer-Verhalten; Fern-Bracket = Folge-Lane).
* Kopf-/Kiefer-Tracking `0x80104B98` (~13 Sub-Tails, u.a. Feed @0x80101BB0): Zwei-Part-
  Aufruf von EXE `0x80017D98` (+0x1C1=2/3, Gate `+0x1C0&1` @0x80017DC4-CC, Ziel
  `[self+0x1B4]`, schreibt Part-Euler) — kein Gegner-Part-Winkel-Kanal im Port → OPEN;
  die portierten `+0x1C0`-Bit-1-Schreiber gaten im Original genau dieses Tracking.
* Rudel-Signal-Clear: `0xFBF4&0x40` ist room-persistent (einziger EXE-Clear FUN_80052f3c) —
  nach einem Latch parkt das Rudel dauerhaft in Sub 11 (kein Exit-Commit im Executor);
  byte-true so übernommen.
* Render-Seite: RE2-FX-System `0x8001BF10` (Tabelle `@0x801056AC` — Blut-Kinds als RE1.5-
  Stand-in, Sabber/0x84/Glas stumm), Part-Blend-Wörter (Latch F98/102), GIB-Part-Scatter
  `0x80104440`, Corpse-Farbtint/UV-Scroll, Hitbox-Squash `0x80104088` (Port führt Hitbox über
  atk_pt). Spawn-Raws 6/7/8/9/0x2003 (Fenster-/Skript-Varianten) haben keinen RE1.5-Produzenten.

## ✅ WELLE D — die RE2-KRÄHE (kind 0x21), 2026-08-16

Quelle: `info/re2leon/COMMON/BIN/EMOVL21_S0.BIN` == `CDEMD0.EMS`-Sektor 0x528 (byte-identisch,
Lane K; TOC in der EXE `@0x8009ADF4`: Typ 0x21 = Code 0x4A88 @0x51E/0x528, TIM 0x8220 @0x532,
EMD 0x7E28 @0x543 — alle Werte für Welle D selbst nachgelesen). Slot-0-Reloc `@0x80100000`.
Voll-Disasm 2026-08-16: Scratchpad `crow_self.asm` (4770 Instr., eigener Dump; jede Konstante
im Code trägt ihr `@0x…`-Zitat daraus). Port: `engine/src/enemy_ai_re2_crow.c`, Hook am Kopf
von `re15_crow_ai_tick` (enemy_ai_common.c). Der RE1.5-Default (Flug-Brain FUN_80112020)
bleibt byte-identisch.

### Struktur (byte-belegt)

* Zustandstabelle `@0x80104908`: [0] INIT `0x801002FC` · [1] ACTIVE `0x801004E4` ·
  [2]=[3] HURT `0x801028BC` · [4] SKRIPT-PERCH `0x801034DC` (eigene Tabelle `@0x80104A64`) ·
  [5][6] NULL · [7] CORPSE `0x80103950` (Tabelle `@0x80104A70`).
* ACTIVE = DEC `@0x80104928` → EXEC `@0x80104964` auf `+0x5` (15 Subs); die ALT-Tabellen
  `@0x801049A0/DC` (für `+0x10E&0x80`) sind **inhaltsgleich** (selbst gedumpt). Der EXEC-Index
  wird NACH dem DEC **neu gelesen** (`lbu v0,5` @0x80100654 UND @0x8010067C) — ein DEC-Commit
  läuft noch im selben Tick in den neuen Executor (Port-Bug der ersten Fassung, gefixt).
* Root `@0x8010013C`: Pause-Gate `0x800CFBDC&0x20000000`; `+0x1D3`-low-7-Dec; **LOS-Ray
  `0x80050858(self,PL,0x8400,0)` JEDEN Tick — POLARITÄT SELBST BELEGT: ret==0 → `+0x22A|=2`
  („Sicht frei", @0x801001C8-E8)**; dist<900 → `+0x98=−350` (Render-Pitch, doc); Post-Pass
  `0x801044B0` (Mutex-Release wenn `+0x5∉[11..14]`, Pacify-Bit 0x80 → `+0x221=120`); Schatten-
  Helfer `0x8010026C`: state≠7 → Schatten w=h = **360000/((Höhe+1550)>>1)**, am Boden 400
  (@0x80100294-2F0); Wand-Pass EXE `FUN_8003567C` → `+0x110/+0x114`.
* INIT `@0x801002FC`: **HP=10** (`sh 10,342` @0x80100324/348); Hitbox-Record {96,96,512,312}
  (@0x801003A0-B4); `+0x98=−350`, `+0x9E=530`, `+0x1EE=300` (Leser offen); Boden-Probe
  `0x8004FBA0(pos,250,1024,0)` → `+0x1C2`; Schatten-Init `0x80016480(+0x16C,0,0xC800C8,0)` =
  200×200; `+0x10E&0x4000` → State 4 (Sub=`+0x10E&0xf`, `+0x22A|=1`); Victim-Hook-Install
  `0x80104740` → `0x800CE384`.

### Substate-/Executor-Tabelle (alle mit @-Zitaten im Code)

| Sub | Rolle | Kern (selbst disasm'd) |
|---|---|---|
| 0 | Sitzen/Idle | DEC: Wake dist≤1800 / RENNT≤5401 / `+0x22A&1`-Timer `+0x158`; **Spieler tot + Pacifier abgelaufen + dist<1800 → NIE wecken** (@0x801006D0-F4). EXEC: 15-Phasen-Idle `@0x80100004` (P7 würfelt `rand%7` als Nächstes in `+0x218`, Phase 6 = Hop nur 1/16; P10: SE 6 50%, gesperrt von `+0x22A&0x20`; P13: Reverse-Trick `(39−f)<<8\|0x7000A` @0x80100B1C; P14: Teleport zurück zum `+0x224/226`-Snapshot) |
| 1 | Alarm | `rand&7` Ticks → Sub 2 (bzw. 3 bei `+0x10E&0x40`) @0x80100BCC-C14 |
| 2 | Abheben | 6 Phasen `@0x80100044`: Clip 7, Speed `100+rand&0x1f`, **vy-Folge −10→−80→−60→−100→−80 (Lane-K-Reihenfolge korrigiert)**, SE 1 50%, Feder 1/8, Steer 32 (50% auf den DELTA-Punkt = authored Jink @0x80100DF4-E2C), Accel +1 Cap 300, done → Sub 4 |
| 3 | Skript-Abflug | Clip 6, Speed 60/80+`rand&7`, Steer-Rate 0→32, done → Sub 4 |
| 4 | Kreisen | DEC: dist≥7201 → Sub 7/8 (`rand&1`); 1/16: `+0x158=rand&0x3f` + Höhenprobe gegen PL-BodenY−3600 → Sub 5/6; Wand-Prober + Arbitrierung/Listener. EXEC: Clip 3/4 (`rand&1`), Steer 32 auf `+0x1C4/6`, **Accel = `+0x218` ∈ {2,3} je Clip**, Cap 300 |
| 5/6 | Steigen/Sinken | vy −40/+40, Bänder PL-BodenY−4800/−1800, Accel +2/+1, Flap-Reload 9/15 |
| 7 | Landen | 7 Phasen `@0x8010005C`: Sturz vy 120, Flare-Gate y−(floor−250)≥−300, Flare Clip 0xA, Snap y=floor−250, Falten Clip 8; **Ende: Spieler lebt ODER Pacifier läuft → `+0x158=rand·2` + SOFORT Sub 2; nur tot+pacified → Sub 0 (Lane-K-„Spieler ruhig→Sub 0" korrigiert, @0x801018DC-924)** |
| 8 | Trudeln | Yaw += `(s8)+0x218`, Steer 96, Kegel 96 auf den Steuerpunkt → Sub 4; Timer `rand&0x3f` |
| 9 | Setzen | Clip 8, Steer 96, done → `+0x158=rand&0xf` + Sub 0 |
| 10 | Wand-Crash | via Prober `0x80104094`: Kontakt `+0x110&1` + `+0x21D`-Streak frisch + Speed≥231 + **Ebenen-Fenster `(plane<<10)+1920..2176` (`0x80050110`; plane 2 wrap-identisch @0x80104120-28)**; Streak≥11 → 1/128 Sub 7 sonst Sub 8; Grav-Akku `+0x224` +20/Tick, Thud SE 5, Clip 0xB mit Frame-Dekrement-Hold, Aufstehen → (1,9) |
| 11 | Angriffs-Anlauf | DEC: Kegel(Spieler,96) raus ODER LOS weg → Sub 4. EXEC: Zielhöhe **PL-BodenY−2700**, Steer SPIELER 96, Accel +5, y±40-Klemme; **Commit-Gate = Spieler-Body-Kontakt (`+0xD==0`)**: Fenster ±359 → Speed≥231 → Sub 12, sonst Nav-Reseed + Sub 8; ohne Kontakt nur 3D-Move (@0x80101EE8-F74) |
| 12 | STRIKE | P0: `0x80015910(self,PL)` ≠0 → **Spieler-State (2,1)** sonst (2,0) (@0x8010201C-34); Shake(4,0)+Rumble(10,150,0) (Pad-Aktuator, doc), SE 4, Clip 9; Grav-Fall, Thud SE 5, Clip 0xB; Exit-Kegel 1024: innen → Sub 9, außen → Sub 0 + `+0x158=rand&0xf` |
| 13 | Hover-Harass | DEC: Kegel 192 / LOS → Vol 350 + Sub 4. EXEC: Ziel PL-BodenY−2880, ±60-Klemme; claimed (`+0x22A&4`) → Steer 96/Accel +4, sonst 384/+12; dist<900 → Vol 100; **dist<650 stoppt + Höhenfenster ±99 → Sub 14** |
| 14 | GRAB | P0 (claimed): Broadcast 8+64, **PL → (5,0)** via `0x80104078`, Anker `0x800CFDAC/0x800CFD80/84`, y-Snap auf `+0x224`, Steer 2048, Clip 5, **Timeout `+0x219=100`**, Speed 300. P1-Peck: Shake(6,0)/Rumble(4,150,1) (doc), **Blut-FX `0x11000` @ y+500**, Feder 1/4, SE 2, **`0x800401d4(5, aliveflag)`** — ret 1 → Release, ret 2 → **Broadcast 128** + Release. P2-4: **Mash `+0x219 −= 3·mash+1`** (@0x801026DC-F8), Lift/Drop y∓(150∓15k). Release `@0x80102848`: Nav-Reseed, Sub 4, Vol 350, **Broadcast 16, `PL+0x6=3`** (Victim-Release-Phase) |

### Flock-Protokoll (Mutex + Broadcast, selbst belegt)

* **Mutex = `0x800CFBF4` Bit 0** — DASSELBE Wort wie die Hunde-Bits (Port: geteiltes
  `g_re2_room_gflags`, einziger EXE-Clear = Room-Init FUN_80052f3c). Claim in der Arbitrierung
  `0x801041F0`: Fenster **dist ∈ [901, 7195]** (`addiu −901; sltiu 0x189B` @0x80104254-60) +
  Mutex frei + LOS-Bit 2 + Kegel(Spieler,256) → `0xFBF4|=1`, `+0x22A|=4`; `rand&0xf==0` →
  Sub 11 sonst Sub 13. **Es greift immer nur EINE Krähe an.** Release im Post-Pass, sobald
  `+0x5∉[11..14]` (@0x801044C4-F0).
* **Direkt-STRIKE** (Lane-K-Korrektur): das Gate ist KEIN „dist<901", sondern der
  **Body-Push-Stempel `+0xD==0`** (EXE `FUN_800355c4` setzt `+0xD=0xFF`/Frame, der Part-Push
  `FUN_80034d0c` stempelt die Pusher-Id; Spieler = 0 — beide selbst decompile-gelesen) +
  Kontakt-Streak `+0x21E==0` + Speed≥231 + Mutex frei + Kegel 256 → Sub 12 OHNE Claim
  (@0x80104310-80).
* **Broadcast `0x8010452C(mask)`**: alle Typ-0x21 der Aktivliste AB Eintrag 1 (Spieler-Kopf
  übersprungen), Claimer ausgenommen (`andi 4; bne` @0x8010456C-70): `+0x22A|=mask` +
  Nav-Reset. Masken: **8+64** Grab-Start (Konsum „Join-Harass": Bits 8+2 zusammen → Sub 13,
  Clear 0x48 @0x8010420C-34; Bit 64 = Konvergenz im ACTIVE-Nav @0x80100584-90), **16**
  Abbruch (Listener `0x80104400`: Vol 350, Nav-Reseed `rand&0x9D` (!), Sub 4, Cooldown
  `+0x21C=30`, Clear 0x58), **128** Pacify (Post-Pass → `+0x221=120`; ACTIVE-Prolog
  dekrementiert bei totem Spieler @0x80100558-80; abgelaufen + dist<1800 → Landen
  `0x8010447C`, Sitzen wacht nie mehr @0x801006D0-F4).

### HURT/GIB/CORPSE (Zeilen-Dispatch `@0x80104A18` auf `+0x5`)

* Zeilen **0-4/13-16/18 → Flug-Treffer `0x80102934`**: SE 3, Clip 9, Knock-Richtung
  `atan2(PL→self)−Yaw` → `+0x226`, **Speed=100 über `[0x800CE330]`=CURRENT-ENTITY (=self —
  Lane-K-OPEN „Attacker-Write `+0x144`" damit AUFGELÖST)**, Spiral-Spin ±200 auf `+0x78`
  (Pitch, Clamp ±2047 — Render-doc), Federburst + 2..3 Federn; Boden: SE 5, Clip 0xB;
  **HP≥0 → ERHOLUNG: Kegel 1024 außen → (1,9), innen → (1,0)+`+0x158=rand&0xf` — die Krähe
  fliegt wieder**; HP<0 → 11 Ticks Liegen → (7,0).
* Zeilen **5/6/9/10/11/17 → GIB `0x80102CA0`**: 13 Part-Zeilen (`[+0x198]`+i·172) Wurf
  `0x80102EF4` (Speed 800/vy −100) / Wirbel `0x80102F34` nach rand-Maske (Zeilen 9/10 immer
  Wurf), 3× Federburst, Federn `(rand&3)|8` → **(7,1) ohne Lache**. Port: Mesh-Wipe
  (`crow_hide`) + ESP-Stand-in (Part-Mover Render-OPEN).
* Zeile **12 → Boden-Treffer `0x80102FB4`**: **Sofort-Kill (`sh −1,342`)**, Roh-Vektor-
  Rutscher (byte-true Skalar/Vektor-Aliasing von `+0x144`), Wand → Splat-Pose → **(7,2)**.
* Zeilen **7/8 → Launch-Treffer `0x8010327C`**: Sofort-Kill, **Speed 600 in
  SPIELER-Blickrichtung `0x800CFC6E`**, Wand-Bounce Yaw+2048 → **(7,3)**.
* CORPSE `@0x80104A70`: **Sub 0** = Record-Recolor `0x80016FE4([+0x16C], 0x00BFBF10)`
  `@0x80103A20` = **BLUTLACHE** (⛔ 2026-08-21 korrigiert; das war als „Modell-Tint,
  Render-OPEN“ fehlgelesen — s. Block „Schatten-/Prim-Record“ unten),
  **Lache = Schatten-Record `+4/+6` := 400, dann +10/Tick solange `+0x15A`(=60)>0 — NUR in
  Phase 1**; Zucken: Frame 11↔0/12-Toggle, Timer `+0x219=(rand&0x1f)+25` = `+0x158`-Seed,
  Eskalation `+0x158 += 10+rand&0xf` bis ≥151, SE 3 nur solange `+0x158<90`, ≥91 nur jeden
  2. Tick. **Sub 1** (GIB) = Record-FREIGABE `sb zero,14([+0x16C])` `@0x80103C34`. **Sub 2** (Wand-Splat) =
  Clip-4-Flattern (Doppel-Rate unter `+0x158<100`) + Absturz. **Sub 3** = Lache ohne Zucken.
  Sub 4 (`@0x80104A80` → `0x8010477C`) = tote Zeile (kein Schreiber).

#### ⛔ Schatten-/Prim-Record `[+0x16C]` — Nutzer-Report 2026-08-21 („platzende Krähen hinterlassen Schatten“)

Der Boden-Schatten ist ein **Slot aus einem 50er-Pool**. Allokator `0x80016480` (`PSX.EXE`,
selbst gelesen): Basis `0x800CE698` `@0x80016488`, Stride 104 `@0x800164A8`, Freisuche
`lbu v0,14(t0)` `@0x800164AC-B4`, Belegen `sb 5,14(t0)` `@0x800164D4`, Zeiger nach `ent+0x16C`
`@0x800164D0`, `sw a2,4(t0)` `@0x80016530` (= die zwei Halb-Ausdehnungen `rec+0x04/+0x06`),
Farbe bei `a3==0` = **0x00808080 Neutralgrau** `@0x80016500-04`. Recolor `0x80016FE4`:
`lw v0,28(a0)` / `lw v1,68(a0)` → `(alt & 0xFF000000) | a1` → `sw` zurück
(`@0x80016FE8-0x80017008`) = die Farbwörter `rec+0x1C` / `rec+0x44`.

| Corpse-Sub | Original | Port (gefixt) |
|---|---|---|
| 0 Normal | `jal 0x80016FE4(rec, 0x00BFBF10)` `@0x80103A20`, `rec+4/+6 = 400` `@0x80103A2C-3C`, +10/Tick × 60 | `crow_pool = 1`, Farbe **0x10/0xBF/0xBF** |
| 3 Launch | dieselben Zeilen `@0x80103EF4-F18` / `@0x80103F1C-38` | `crow_pool = 1` |
| 1 GIB | `lw v1,364(a0)` `@0x80103C24` + **`sb zero,14(v1)`** `@0x80103C34` = **Slot FREIGEGEBEN** | `crow_shadow_w/h = 0` → Renderer zeichnet **nichts** |
| 2 Wandsplat | `lw v1,364(s1)` `@0x80103CA8` + `sb zero,14(v1)` `@0x80103CBC` | dito |

**Der Fehler:** `platform/pc/main.c` testete `crow_shadow_w != 0` und fiel bei 0 auf seinen
**500×600-Standardschatten** zurück — unter der geplatzten Krähe blieb also ein ganz normaler
Charakter-Schatten liegen. Zusätzlich blieb die **normale** Leiche auf dem grauen Alloc-Default
stehen statt zur Lache zu werden. Beides gefixt; Pin: `tests/unit/test_re2_crow_shadow_release.c`,
Messung am echten Weg: `tests/unit/probe_re2_crow_shadow.c`. Die **RE1.5**-Krähe ist unberührt —
ihr GIB-Wipe schreibt `sh 1` `@0x80115938-3C` (1×1-Quad), nicht 0, und ihre Lachenfarbe bleibt
`0x00FFFF38` `@0x80115880-C8`.

### SE-/FX-Map + ENEMSE-Bank (selbst bestimmt)

* **ENEMSE-Bank = 7** (korrigiert 2026-08-20, Nutzer-Report „bei RE2-AI haben die Krähen den
  falschen Sound"). ⛔ **WIDERLEGT: die frühere Zuordnung „Bank 21, weil kind 0x21 in Zeile 21
  = {0x21,0x00} steht".** Die Paar-Tabelle `@0x800A7400` führt **keine Gegner-kinds**, sondern
  die **SOUND-ID aus dem Sce_em_set-Record (+7)**:
  * Sce_em_set = **Opcode 0x44** (Dispatch-Tabelle `@0x800A74C8`, Eintrag `[0x44] @0x800A75D8`
    = `0x8005714C`; einzige Wort-Referenz dieser Adresse in der EXE).
  * Im Handler: `lbu a0,3(v0)` `@0x800571EC` → `jal 0x8001b710` = **Record+3 IST der kind**;
    `lbu v0,7(v0)` `@0x80057274` → `sb v0,506(s0)` `@0x80057280` = **Record+7 → entity+0x1FA**.
    Verglichen wird gegen die Tabelle **nur +0x1FA** (`lb v1,506(a0)` `@0x80052C48`).
  * Struktur-Beleg: 28 Zeilen führen Werte 0x01..0x0F — im kind-Raum (ab 0x10) unmöglich.
  * Zensus über alle 495 RE2-RDTs (`info/re2leon/PL0/RDT`): **kind 0x21 → Sound-Id 0x0D**,
    37/37 Records (ROOM1090 ×28, ROOM2110 ×9). 0x0D steht in genau einer Zeile:
    **Zeile 7 = {0x0D, 0x00} → Bank 7, erste Hälfte (flag2000 = 0)**.
  * Daten-Gegenprobe (TOC `@0x800A7B1C`): Bank 7 = EDT `@0x86800`, VBD 0x0BCF0, **8 VAGs,
    ids 0..6 live** — deckt exakt die 6 SEs des Moduls. Bank 21 = EDT `@0x189000`, VBD
    0x1FE00, 16 VAGs, ids 0..14 = eine fremde, doppelt so große Bank.
  * Der env-Override `RE15_RE2_CROW_SE_BANK` ist **entfernt** (Wert ist belegt, kein Schalter).
  * Herleitung als Test: `re15_port/tests/unit/test_re2_crow_se_bank.c` (liest EXE, die beiden
    Krähen-RDTs und ENEMSE.VBS selbst nach).
  * ⚠️ **Gleiche Kategorien-Verwechslung noch offen bei Hund und Spinne**: kind 0x20 → Sound-Id
    0x0C (Zeile 6), kind 0x25/0x26 → Sound-Id 0x10 (Zeile 11); der Port fährt dort 31+flag2000
    bzw. 53/24. Der Zombie-Wert 0 = Zeile {0x03,0x00} ist dagegen zufällig richtig
    (kind 0x10/0x12 tragen Sound-Id 0x03/0x05/0x06).
* **Kanal-/Prioritäts-Maschine** (2026-08-20 nachgezogen): `FUN_8005BD6C` legt den SPU-Kanal
  FEST (`andi s1,v0,0x1f` `@0x8005BDFC` auf EDT-Byte3) und lässt pro Kanal nur EINEN Laut zu —
  `jal 0x8005C92C` `@0x8005BE08` + `bne v0,zero` `@0x8005BE14` verwerfen den SE komplett;
  `sb (prio&7),DAT_800D4CA0[chan*2]` `@0x8005BE98` latcht, `sb zero,…` `@0x8005C870` gibt frei;
  Key-On wird nur VORGEMERKT (`@0x8005BEAC/B0`), Extra-Layer laufen auf chan+1/tone+1
  (`@0x8005BF3C/44`). Bank-7-Kanäle: id0→4 id1→5 id2→7 id3→5 id4→7 id5→6 id6→4.
* SEs (`0x8005BD6C(id,self)`): 1 Flügelschlag · 2 Picken (Grab-Peck) · 3 Kreischen
  (Wand-Crash/Hurt/Zucken) · 4 Strike-Impact · 5 Thud · 6 Idle-Krächzen (50%,
  `+0x22A&0x20`-gesperrt). Volume-Helfer `0x8010472C` (350/100/50/10 → `+0x90/92/9A/9C`) =
  doc-NOP (kein Port-Kanal; `+0x90` ist zugleich der Wand-Radius von FUN_8003567C).
* FX (`0x8001BF10`, Tabelle `0x8009DB44`): Federn `0xC000800` ±512 (`0x80104678`, a1=ANZAHL),
  Federburst `0x12800` ±128 (`0x8010459C`), Grab-Blut `0x11000` @ y+500 → Port: ESP-Splatter-/
  Room-Bank-Blut-Stand-ins (RE1.5-GIB-Präzedenz), RNG-Draw-Zählung erhalten.

### Schaden (byte-belegt vs. OFFEN)

* **GRAB-Peck = 5 HP**: `0x800401d4(5, aliveflag)` @0x8010265C-64; `aliveflag` = `+0x218` =
  (PL-HP>0) aus dem Sub-14-Prolog. **MODE-Semantik selbst decompile-gelesen** (FUN_800401d4):
  mode 1 = NICHT-tödlich (HP<0 → One-Save-Schwanz, ret 1 → Abschütteln); mode 0 (HP≤0-Spieler)
  = voller Pfad → mit gesetztem One-Save-Latch ret 2 = Tod → Broadcast 128. Port:
  `re15_re2_player_damage_mode` (geteilter One-Save-Latch mit Zombie/Hund).
* **STRIKE**: schreibt NUR den Spieler-State (2, Sub 1/0) — die Schadenshöhe liegt im
  EXE-State-2-Handler des Spielers (kein `0x800401d4`-Caller; einziger weiterer EXE-Caller
  `FUN_80047664` ist ein AoE-Helfer, selbst gelesen). **OFFEN** — Port: Hit-Guard/Flinch ohne
  HP-Verlust, KEIN erfundener Wert.

### Assets

`EM021.EMD` @Sektor 0x543 (selbst geparst): dir @0x7E08, 8 Sektionen; **Paar 1 = 12 Clips**
(EDD @0xC; Frame-Zahlen 15/4/21/16/10/8/24/33/16/6/40/40 — exakt der KI-Bereich 0..0xB),
EMR 13 Knochen kf 72; Paar 2 leer (own_ok=0); **Paar 3 = Victim {14, 36, 20}F auf
15-Bone-Player-Pool** (Intro/Hold-Loop/Release = die Clips 0→1→2 des Victim-Hooks
`0x80104740`, Tabelle `@0x80100124` selbst gelesen) → der Welle-A-Splitter
(`re2_emd_parse_bank`) trägt alles unverändert. TIM 0x8220 @0x532.

### Verifikation

* `test_re2_crow_ai` (Unit): INIT/HP10/Hitbox96, Wake→Alarm→Takeoff→Kreisen, Takeoff-Werte,
  Arbitrierung (NIE zwei Claims), GRAB-Peck exakt 5 HP + Timeout-Release + One-Save (mode 1)
  + Kill (mode 0 + Latch) + Pacifier 120, Direkt-STRIKE am Body-Kontakt, HURT-RECOVERY
  (fliegt wieder!), Kill→(7,0) mit Lache 400→+10/Tick×60 + Zuck-SE, GIB→(7,1)+Schatten-Disable,
  Launch-Zeile (Speed 600, Spieler-Yaw, (7,3)), 600F-No-Freeze.
* `test_re2_room10c0_ab` (A/B, echter RE1.5-Krähen-Raum): RE1.5-Baseline unverändert; RE2:
  EM021-Bank parst (12 Clips/13 Knochen/victim_ok), Spawn HP=10, Takeoff+Höhengewinn,
  Attack-Commit ohne Doppel-Claim, GRAB mit exakt 5-HP-Pecks, Anschuss-Recovery, Kill→Corpse
  mit Lache-Wachstum, 600F-No-Freeze. ctest gesamt: **132/132**.
* Live-Smoke (echte EXE, `RE15_AI_FLAVOR=RE2` + `RE15_DEBUG_JUMP=10c0@gp` + Boot-Input-Skript;
  stderr = `re15_port/debug.log`): `[room] PC loaded room10c0.rdt`, `[enemy] RE2 EM021 loaded:
  13 meshes, 13 bones, 12 clips`, `[re2se] ENEMSE Bank 21 geladen: 16 VAGs, Map 32 Eintraege`,
  3 Krähen ticken das RE2-Brain (Idle-Phasen-Würfel P7, Clips 0/1/2; kein Wake bei dist>5401 —
  byte-true).

### Review-Pass (16 bestätigte Funde, 2026-08-16) — alle umgesetzt

Adversarieller Review über den Welle-D-Diff (`welle_d_review_confirmed.json`): 4×HIGH
(dec4-Fern-Exit fällt in den 1/16-Roll @0x801010F0→F8; exec4-Re-Roll ALT==0 @0x801012B4-C8;
Alarm ohne sub2-Latch = geometrische Dauer, 1 Draw/Tick @0x80100BCC-D8; GRAB-Eintritts-Tick =
P0+P1+P2 @0x801025C8→CC/@0x801026BC→C0), 9 weitere Setup-Fallthroughs (Landen/Wand-Crash/
Strike/Flug-Treffer/Boden/Launch/3×Corpse-Ph0), exec8-ALT==0 @0x80101A78-94, exec11 ohne
y-Klemme @0x80101EAC-E4, GIB-Wirbel-Draws (3/Zeile, 0x80102F34 @0x80102F54/68/88),
Post-Pass-Release ohne State-Zusatz @0x801044C4-D4, dec5/6-Timer unbedingt @0x8010131C/@0x801014BC,
exec3-Timer/Overwrite @0x80101068-84, Reenter-Clear des 0x800CFBF4-Analogs (FUN_80052f3c
`sh zero` @0x80052fe4 → `scd_room_reenter` ruft jetzt `re15_re2z_rng_reset`), Test-OOB-Härtung.
Muster-Sweep über ALLE übrigen Phasen-/Timer-Stellen: exec2-Tail/exec7-P5 sind byte-true
NEU==0-Muster (`addiu −1; sb; andi; bne`), alle übrigen ALT==0-Stellen folgen dem
`t/t−1/if(t==0)`-Muster (je gegen crow_self.asm geprüft).

### OFFEN (Welle-D-Ausgang)

* **STRIKE-Spielerschaden** (EXE-State-2-Handler des Spielers): nicht RE'd — Port ohne
  HP-Verlust (s.o.). Nächster Weg: RE2-Spieler-Dispatch von `FUN_80035f68`/`FUN_80038bbc`
  weiter verfolgen (Handler-Tabelle nicht gefunden; `DAT_800cfbfc`-Xrefs erschöpft).
* ⚠️ **HURT-Row-Semantik = TREFFER-CODE, nicht Herkunfts-Sub** (Welle-D-Nachtrag, selbst
  belegt): BEIDE RE2-Waffen-Applier überschreiben `+0x5` vor dem HURT-Tick — Projektil
  `sb s5,5(s1)` = RE2-**Waffen**-Low-Byte (@0x80047324/@0x80047574), Hitscan
  `sb (hi16(param)+1),5` (@0x80041AB4). Die HURT-Zeilen der Krähe (@0x80104A18, 19 Zeilen)
  und des Hundes (@0x80105538) dispatchen also auf Waffen-/Trefferklassen (Krähen-GIB bei den
  Projektil-Ids 9/10/11/17 = GL-Runden/Rocket — deckt sich mit dem RE2-Spielverhalten), NICHT
  auf dem Herkunfts-Substate. Port (Welle C+D): `re2z_prev_sub` als deklariertes MAPPING;
  der byte-true Umbau braucht ein RE1.5-Waffe→RE2-Zeilen-Mapping + den Hitscan-Code-Producer
  (`[sp+64]>>16`-Quelle) = eigene Folge-Lane, betrifft Welle C mit. Der Post-Pass-Mutex-
  Release der Krähe nutzt bereits die wörtliche +0x5-Form (s. Code @0x801044C4-D4-Kommentar).
* **Bracket-Producer für `+0x1D2`** (s. Welle-C-OFFEN-Update): Zone gestempelt, Bracket 0 —
  Hunde-Gore-Zeile 9 wird erst mit einem Fern-Bracket-Producer erreichbar.
* Navigator `0x8004A808`/Route `0x8004AA50` — Steuerziel = Spieler (MAPPING, Welle-B/C-Muster);
  `+0x21F`-Routen-Byte bleibt 0, `+0x220`-Buchhaltung byte-true.
* LOS `0x80050858` Mode 0x8400: Ret-Semantik der EXE-Funktion nicht disassembliert; Polarität
  über den Root-Writer belegt (ret==0 = frei) → Port-MAPPING RE1.5-Ray `FUN_8003dcc4`
  (alle 4 Regionen/Tick, ohne FOV-Kegel).
* Wand-Ebene `0x80050110` + Fenster: Port-MAPPING = SCA-Klemme + Frontal-Metrik
  (Restweg²·4 < Sollweg²); Radius-Quelle `+0x90` (350/100/50/10, volume-gekoppelt) nicht
  modelliert (fix 200 wie die RE1.5-Krähe).
* State 4 (Skript-Perch, Subs 0/1/2 mit Wegpunkten (−12500,−12852)→(−3919,−12288) Rate 96
  @0x801036FC-704/@0x8010376C-70, Versteck via `word0|0x80008` bis `+0x1D4&4`): kein
  RE1.5-Spawn-Produzent für `+0x10E&0x4000` → Port-Stub (Spec hier dokumentiert).
* Render-Seite: Modell-Tint `0xBFBF10`, Spiral-Pitch `+0x78` ±200, Roll/Pitch-Zufall des
  Wand-Splats, Part-Scatter des GIB, Lache-FARBE (EXE-Schattenrenderer), `+0x98`-Pitch −350.
* Leser offen: `+0x9E=530`, `+0x1EE=300`, `+0x1E8=1` (INIT), Paar-Tabelle `@0x801048A8`
  (2×12 s16, {300,300},{250,450},… — kein Modul-Konsument), Advancer-a3-Bit `0x10000`
  (Idle-P9/P13), Feder-Einzel-FX `0x80104628` (kind `0x50C00`, kein gefundener Caller).
* SE-Volume-Kanal (`0x8010472C`) und Pad-Rumble/Screenshake (`0x8003947C/0x80039514`,
  Welle-C-Decompile) — dokumentiert übersprungen.

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

### ⛔ 2026-08-21 — die dritte Stufe „AI RE2 MODELS" ist ENTFERNT

Nutzer-Entscheidung, wörtlich: *„Mittlerweile ist RE 2 AI schon so gut, dass wir diese Option im
Optionsmenü sowie seine eigenen Models entfernen können. Wir konzentrieren uns jetzt nur noch auf
RE 1.5 AI und RE 2 AI."*

Der Schalter hat wieder **zwei** Zustände (`RE1.5` / `RE2`), UP == DOWN togglet wie auf der
SOUND-Seite (kein Modulo-Zyklus, kein toter dritter Index). Unter dem RE2-Flavor läuft **immer**
das Hybrid-Rig: `platform/pc/main.c pc_enemy_load` ruft `pc_enemy_hybrid_re15_models`
bedingungslos; der reine RE2-Modell-Zweig ist weg.

* `pc_ai_mode()` liefert nur noch 0/1; `pc_ai_mode_set()` **klemmt** jeden Fremdwert (>1 → 1).
  Ein Alt-Wert `2` (z. B. `RE15_CONFIG_AI=2`) landet damit auf „AI RE2" — dem Zustand, den er
  gemeint hat. Der Wert wird **nirgends persistiert** (weder Savedata noch Memory-Card noch eine
  Konfigdatei), es gibt also keinen Speicherstand mit einer 2 darin.
* Das orthogonale Flag `re15_ai_models()` (`re15_ai_flavor.h`, Speicher in
  `enemy_ai_re2_zombie.c`) wird von `main.c` **nicht mehr gelesen** und lebt nur noch als
  Test-Haken für den Rig-Umbau (`tests/unit/test_re2_hybrid_rig.c`). Kein Spiel-Codepfad darf
  neu darauf keyen.
* **Assets:** `shared_assets/RE2/{CDEMD0.EMS, ENEMSE.VBS}` bleiben Pflicht — Stufe 2 braucht
  CDEMD0.EMS für die RE2-**Skelette/EDDs/Clips** (der Hybrid tauscht nur Mesh + Textur) und
  ENEMSE.VBS für die RE2-SEs. `release/make_package.sh` kopiert also unverändert weiter; es gab
  dort **keinen** Kopierschritt nur für die Modell-Stufe.

