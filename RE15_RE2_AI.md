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

### Was jetzt noch fehlt

Nicht mehr die Entscheidung, sondern die **Ausführung**: `0x0C01` ist RE2-Substate 12, dessen
Executor `0x80104748` nicht portiert ist, und `0x0E01` ist Substate 14 (`0x80104D74`). RE2-Clip-
Indizes zeigen nicht auf RE1.5-Clips, und ein Mapping auf einen RE1.5-Substate wäre wieder eine
Chimäre. Deshalb werden diese Worte verworfen statt falsch angewandt.

## 🔜 W3+ — Trefferreaktion (HURT `@0x80104F40`), Kriech-Variante (`@0x80101210`), State 8

Nur Leads, **alle** Erstbefunde widerlegt. Belegt ist bisher: die Kriech-Variante nutzt **eigene**
Tabellen (`0x8010C90C` Decision / `0x8010C918` Executor) mit nur **drei** Einträgen, und
`sub_state_1 == 1` teilt sich den Handler mit der aufrechten Variante.

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
