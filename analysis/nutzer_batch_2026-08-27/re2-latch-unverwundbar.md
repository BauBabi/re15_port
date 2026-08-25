# FINDING C — RE2-AI: Zombies manchmal unverwundbar, bis sie einmal gebissen haben

STATUS: **GELOEST / reproduziert** — die vermutete Ursache (fehlende `re2z_hit_latch_release`-Aufrufe)
ist **MESSTECHNISCH WIDERLEGT**; die echte Ursache ist eine ZWEITE, unabhaengige Sperre:
das ELEVATIONS-BAND des geteilten RE1.5-Aufloesers, das auf dem RE2-KRIECHER haengen bleibt.

---

## 1. Symptom (Nutzer-Wortlaut)

> "Bei RE2-AI sind die Zombies immer noch manchmal unverwundbar, bis sie mich einmal
> gebissen haben."

Auftrags-Hypothese war: `re2z_hit_latch_release()` (`re15_port/engine/src/enemy_ai_re2_zombie.c:1088`)
wird nicht auf allen Pfaden erreicht.

---

## 2. Was der PORT heute tut

### 2.1 Die Latch-Hypothese ist TOT — der Port hat laengst eine ZENTRALE Freigabe

`re2z_hit_latch_release()` ist heute **funktional irrelevant**. Seit dem Vier-Gate-Spiegel wird
`hit_react` (+0x93) fuer JEDEN RE2-eigenen Zombie in JEDEM Frame komplett neu berechnet:

`re15_port/engine/src/enemy_ai_re2_zombie.c:7443-7479`
```c
void re15_re2z_hit_filter_apply(int slot)
{   ...
    int hittable = (e->active != 0)                            /* (1) +0x0 & 1   @0x80047124-30 */
                && (spawn_pose || e->re2z_self1d3 == 0u)       /* (2) +0x1D3     @0x80047138-40 */
                && (e->hp >= 0)                                /* (3) HP < 0     @0x80047148-50 */
                && (spawn_pose || !(e->re2z_f10e & 0xC000u));  /* (4) +0x10E     @0x80047158-64 */
    if (hittable) e->hit_react &= (uint8_t)~1u;                /* Kandidat freigegeben */
    else          e->hit_react |= (uint8_t)1u;                 /* Kandidat UEBERSPRUNGEN */
    e->hit_react &= (uint8_t)~2u;
}
```
Aufgerufen (a) am Ende von `re15_re2z_tick` (`:7417`) UND (b) tick-unabhaengig in
`re15_port/engine/src/game_step_common.c:1695-1699`:
```c
extern void re15_re2z_hit_filter_apply(int slot);
for (int s = 1; s < RE15_ACTOR_MAX; s++)
    if (g_actors[s].active && re15_ai_re2_for_type(g_actors[s].type)
        && re15_re2z_owns_type(g_actors[s].type))
        re15_re2z_hit_filter_apply(s);
```
`hit_react` ist damit ein **abgeleiteter Wert**, kein Latch mehr. Ein "vergessener"
`re2z_hit_latch_release`-Aufruf kann per Konstruktion nicht mehr haengen bleiben.

**GEMESSEN** (`re15_port/build/tests/unit/probe_re2z_abc.exe`, 64 Seeds x 900 Frames, echter
`game_step`-Weg, ROOM1140, geladene RE2-Baenke EM010/11/16, Pistole):
```
(C) Untreffbar >= 200 Frames bei hp>=0: 0 Vorfaelle in 0/64 Seeds
--- (C) nach Ort: EXEC[7] liegend=0  EXEC[8] fressend=0  EXEC[5] Sturz=0  HURT=0  SONST=0
```
und mit 8 Seeds x 6000 Frames ebenfalls `0 Vorfaelle in 0/8 Seeds`.
=> **Der +0x93-Pfad ist geschlossen.** Die geforderte Tabelle "State -> Latch geloescht?" ist
damit trivial: fuer JEDEN State/Substate lautet die Antwort "ja, zentral, jeden Frame".

### 2.2 Die Zustands-Tabelle (Vollstaendigkeit, wie beauftragt)

| Wurzel / Executor | +0x1D3-Setzer | +0x1D3-Loescher | +0x93 nach dem Zentralfilter |
|---|---|---|---|
| INIT (state 0) `re2z_init` :6937 | — (`=0` @0x801006C8) | — | frei |
| EXEC[0] STAND / EXEC[1] WALK / EXEC[2] BUMP | keiner | — | frei |
| EXEC[3] GRAB `re2z_exec_grab` :1404 | `=15` @0x8010276C-70 | Root-Dekrement :7133 (@0x80100484-98) | 15 Frames gesperrt (byte-true) |
| EXEC[5] KNOCKDOWN :1729 / :1824 / :1929 | `|=0x80` @0x80103304 | P2 `&0x7f` @0x80103484-90, P8 @0x80103718-28 | frei ab P2 |
| EXEC[6] DEVOUR :2036 | `|=0x80` @0x801039F8-A04 | EXEC[8] P3 `&0x7f` @0x80103CE4-FC | Spawn-Pose-Ausnahme |
| EXEC[7] LIEGEND :2190 / :2219 | `|=0x80` @0x80103804-14 | P4 @0x80103914-18 | Spawn-Pose-Ausnahme = treffbar |
| EXEC[8] FRESSEND :2259 / :2290 | `|=0x80` @0x80103C04-14 | P3 @0x80103CE4-FC | Spawn-Pose-Ausnahme = treffbar |
| EXEC[9] STOSS/TAUMEL :2437 | `|=0x80` @0x801040B4 (nur Sturz-Zweig) | ueber EXEC[5] P2/P8 | frei ab EXEC[5] P2 |
| EXEC[11] ZUSAMMENBRUCH :2630 | `|=0x80` @0x8010444C-5C | P2 -> EXEC[5] P2, **ODER Kriecher-Zweig** | s. §4 |
| HURT `re2z_hit_ragdoll` :5460/:5471 | `|=0x80` @0x801069A4-B0 | P2 `&0x7f` @0x80106A9C | frei |
| HURT `re2z_hit_knockdown` :5767/:5809 | `|=0x80` @0x801074F0-504 | P2 `&0x7f` @0x80107850-5C | frei |
| DEATH/CORPSE (:4783/:4911/:5097/:6365/:6430/:6537) | `|=0x80` | — | hp<0, Gate (3) |
| Nachlauf fuer verlassene Spawn-Posen :7465-7468 | — | `&0x7f` + `10E&=~0x4000` | frei |

### 2.3 Die ECHTE, heute wirksame Sperre: das ELEVATIONS-BAND

`re15_port/engine/src/re15_damage.c:1267-1275` (in der Kandidatenschleife von
`re15_player_weapon_fire`):
```c
{   int lying = (e->grid_id & 0x80) ||
                (re15_ai_re2_for_type(e->type) &&
                 re15_re2z_owns_type(e->type) &&
                 (e->re2z_flags21a & 0x2u));
    if (lying) {
        eband &= ~0x40000000u;            /* @0x80101624-3c LEVEL weg */
        if (bdist < 0x1388u) eband |= 0x20000000u;  /* @0x800129cc-f0 */
    }
}
...
if ((pband & eband) == 0) continue;
```
`pband` ist bei ungeneigtem Zielen `0x40000000` (LEVEL). Ein Gegner mit `lying` verliert genau
dieses Bit => **jeder LEVEL-Schuss ueberspringt ihn**, unabhaengig von `hit_react`.

---

## 3. Was das ORIGINAL tut (alles selbst disassembliert, Bytes im Zitat)

### 3.1 RE2: der Kandidatenfilter hat VIER Gates und KEIN Elevations-Band
`info/re2leon/PSX.EXE`, `t_addr=0x80010000`, FUN_800470C0 — selbst disassembliert:
```
80047124: 8e020000  lw   v0,0(s0)          ; Entity-Flagwort
8004712c: 30420001  andi v0,v0,0x1
80047130: 104000b6  beq  v0,zero,0x8004740c ; (1) inaktiv        -> uebersprungen
80047138: 920201d3  lbu  v0,467(s0)         ; +0x1D3
80047140: 144000b2  bne  v0,zero,0x8004740c ; (2) +0x1D3 != 0    -> uebersprungen
80047148: 86020156  lh   v0,342(s0)         ; HP
80047150: 044000ae  bltz v0,0x8004740c      ; (3) HP < 0         -> uebersprungen
80047158: 9602010e  lhu  v0,270(s0)         ; +0x10E
80047160: 3042c000  andi v0,v0,0xc000
80047164: 144000a9  bne  v0,zero,0x8004740c ; (4) +0x10E & 0xC000-> uebersprungen
8004716c: 960301d0  lhu  v1,464(s0)         ; erst danach die Trefferpruefung
```
**Kein einziger Test auf die Hoehen-Bits von word0.** RE2 trennt die Zielhoehe ueber die
Waffen-Geometrie-Tabelle @0x800A68E8 (drei Hoehen-Gruppen je Waffe), nicht ueber ein Band-Gate.
=> Ein RE2-Kriecher mit `+0x1D3 == 0` und `HP >= 0` ist im Original IMMER Kandidat.

### 3.2 RE1.5: das Band-Gate existiert und haengt an `+0x9 & 0x80`
`info/Re1.5/PSX/BIN/STAGE1.BIN`, Zombie-ACTIVE-Tail — selbst disassembliert:
```
801015c0: lbu v0,9(a0)                 ; +0x9 (grid_id)
801015c8: andi v0,v0,0x80
801015cc: bne v0,zero,0x80101600
...
80101600: jal 0x80012aa4               ; Band-Stempel
80101604: ori a0,zero,0xbb8
80101614: lbu v0,9(a1)                 ; +0x9 erneut
8010161c: andi v0,v0,0x80
80101620: beq v0,zero,0x80101640       ; NICHT liegend -> fertig
80101624: lui v1,0xbfff
80101628: ori v1,v1,0xffff             ; Maske ~0x40000000  = LEVEL WEG
80101630: ori a0,zero,0x1388
80101634: and v0,v0,v1
80101638: jal 0x80012974               ; DOWN-Band, wenn dist < 0x1388
8010163c: sw  v0,0(a1)
```

### 3.3 RE1.5: der eigene Kriecher traegt `grid_id = 0x81`
`STAGE1.BIN`, Kriech-Toggle FUN_80104f80, Phase 2 — selbst disassembliert:
```
801050cc: beq v0,zero,0x80105100
801050d0: ori v0,zero,0x81
801050d4: sb  v0,9(v1)                 ; +0x9 = 0x81  -> traegt Bit 0x80 = LIEGEND
801050f0: ori v0,zero,0x8
801050f4: sb  v0,471(v1)               ; +0x1D7 = 8 (Kriech-SCA-Zeile)
```
=> In RE1.5 ist ein Kriecher tatsaechlich "liegend" fuer das Band. RE1.5 erzeugt Kriecher aber
**nur** ueber das Raumskript (`+0x1C4 & 0x1000`, ROOM1030-Tor) — nicht spontan im Kampf.

### 3.4 RE2: der spontane Kriecher-Umbau im Kampf (EXEC[11] P1)
`info/re2leon/COMMON/BIN/EMOVL10_S0.BIN` (RAW @0x80100000) — selbst disassembliert:
```
80104530: 0c005585  jal   0x80015614            ; Peilung, a3=256
80104534: 24070100  addiu a3,zero,256
8010453c: 1440002c  bne   v0,zero,0x801045f0    ; nicht frontal -> raus
80104544: 8e0201f0  lw    v0,496(s0)            ; +0x1F0 = dist
8010454c: 2c420708  sltiu v0,v0,0x708
80104550: 10400027  beq   v0,zero,0x801045f0    ; >= 0x708 -> raus
80104558: 3c02800d  lui   v0,0x800d
8010455c: 9042fdcb  lbu   v0,-565(v0)           ; GLOBAL 0x800CFDCB
80104564: 30420080  andi  v0,v0,0x80
80104568: 14400021  bne   v0,zero,0x801045f0    ; Spieler bereits beansprucht -> raus
80104570: 92030106  lbu   v1,262(s0)            ; +0x106 Etage
80104578: 9042fcfe  lbu   v0,-770(v0)           ; 0x800CFCFE Spieler-Etage
80104580: 1462001b  bne   v1,v0,0x801045f0
80104584: 24030101  addiu v1,zero,257
8010458c: ae030004  sw    v1,4(s0)              ; +0x4 = 0x101
80104588: 9602010e  lhu   v0,270(s0)
80104590: 3042ffc0  andi  v0,v0,0xffc0
80104594: 34420001  ori   v0,v0,0x1
80104598: a602010e  sh    v0,270(s0)            ; +0x10E = (+0x10E & ~0x3F) | 1 = KRIECHER
801045a0: 9042fdcb  lbu   v0,-565(v0)
801045a8: 34420080  ori   v0,v0,0x80
801045b0: a022fdcb  sb    v0,-565(at)           ; 0x800CFDCB |= 0x80
```
Und EXEC[11] **P0**, das die beiden Liege-Latches setzt:
```
80104438: 3042fffb  andi v0,v0,0xfffb
8010443c: 34420002  ori  v0,v0,0x2
80104448: a602021a  sh   v0,538(s0)            ; +0x21A = (&~4) | 2   <== das LIEGE-Bit
8010444c: 920201d3  lbu  v0,467(s0)
80104458: 34420080  ori  v0,v0,0x80
8010445c: a20201d3  sb   v0,467(s0)            ; +0x1D3 |= 0x80
80104460: 9602010e  lhu  v0,270(s0)
80104468: 34422000  ori  v0,v0,0x2000
80104470: a602010e  sh   v0,270(s0)            ; +0x10E |= 0x2000
```
Der EINZIGE Loescher von `+0x21A` Bit 1 im ganzen Overlay ist EXEC[5] P7
(`andi 0xfffd` / `sh 538` @0x801036C8-CC). Der Kriecher-Zweig verlaesst EXEC[11] in P1 und
erreicht EXEC[5] P7 **nie** — das Bit steht danach fuer die gesamte Kriech-Lebenszeit.
Im Original ist das folgenlos: RE2s Aufloeser liest `+0x21A` gar nicht (er hat nur die vier
Gates aus §3.1), und `+0x21A & 2` dient dort ausschliesslich der HURT-ROUTUNG
(`lhu v1,538 / andi v0,v1,0x2` @0x80105168-70).

### 3.5 Nebenbefund: `0x800CFDCB` ist ein GLOBAL, nicht `PL+0x1D3`
Voll-Scan beider RE2-Binaries nach Zugriffen mit Immediate `0xfdcb` (Basis `lui 0x800d`):
19 Treffer im Overlay, 1 in der EXE. Lesen: @0x80101efc, @0x80102568, **@0x80102f4c**,
@0x80102fb0, **@0x80103ad4**, @0x80103b1c, **@0x8010455c**, @0x801045a0, @0x8010464c,
@0x80104f94, @0x80105aa4, @0x80106074, @0x80108200, @0x801082dc, @0x80052070.
Schreiben: @0x80102fc0 (`ori 0x80`), @0x80103b2c, @0x801045b0, **@0x80104fac (`andi 0x7f`)**,
@0x801082f4.
Der Port bildet dieses Global auf `pl->re2z_self1d3` Bit 0x80 ab (`enemy_ai_re2_zombie.c:2609`,
`:2676`, `:2867`, `:2971`, `:3605`). Da alle Setzer/Loescher paarweise an denselben Stellen
sitzen wie die PL+0x1D3-Klaue (`sb v0,467(s3)` @0x80102760 mit s3 = Spieler), ist die
Alias-Abbildung **funktional korrekt** — die Kommentar-Zitate an :2676/:2867/:2971 sind aber
falsch etikettiert (dort steht "PL+0x1D3", gemeint ist `0x800CFDCB`). Reine Doku-Korrektur.

---

## 4. Die Divergenz (praezise)

**Ein RE2-Zombie, der im Kampf per EXEC[11] P1 zum KRIECHER umgebaut wird, traegt danach
dauerhaft `+0x21A & 2`. Der Port uebersetzt dieses Bit in `lying` und streicht damit das
LEVEL-Band — der Spieler kann ihn mit normal gehaltener Waffe NIE treffen.**

Diesen Zustand gibt es in KEINEM der beiden Originale:
* **RE2**: der Aufloeser hat gar kein Band-Gate (§3.1) — der Kriecher ist immer Kandidat.
* **RE1.5**: das Band-Gate existiert (§3.2) und ein Kriecher ist dort tatsaechlich "liegend"
  (`grid 0x81`, §3.3) — aber RE1.5 baut Kriecher NUR per Raumskript um, nie spontan im Kampf.
Der Port kombiniert RE2s spontanen Umbau mit RE1.5s Band-Gate. Genau dieselbe Fehlerklasse wie
der schon dokumentierte Spawn-Posen-Fall ("aus der Ferne unverwundbar, aus der Naehe verwundbar
— gibt es in KEINEM der beiden Originale", Blockkommentar an `re15_re2z_hit_filter_apply`).

### 4.1 MESSUNG (neue Sonde `probe_re2z_bandlock`, echter `game_step`-Weg, ROOM1140, RE2-Baenke)

Teil 1, Sweep 64 Seeds x 900 Frames, Pistole:
```
aufrechte Zombie-Frames gesamt : 66223
davon mit Liege-Bit            : 679  (1.03%) in 6/64 Seeds
   nur grid&0x80 : 0    nur 21A&2 : 679    beide : 0
[STALE] seed 16 slot 4 f569  st=1 s1=1 s2=0 grid=0x00 21A=8002 1D3=80 10E=2001 hp=74 clip=10
[STALE] seed 11 slot 5 f576  st=1 s1=1 s2=0 grid=0x00 21A=8042 1D3=80 10E=2001 hp=74 clip=10
[STALE] seed 24 slot 5 f617 / seed 31 slot 5 f171 / seed 38 slot 4 f629 / seed 59 slot 5 f179
```
Alle 679 Frames tragen `10E = 0x2001` = die KRIECHER-Wurzel. **Kein einziger Fall kommt von
`grid & 0x80`.**

Teil 2, Orakel (Spieler 1200 Einheiten frontal davor, 30 Schuss, `hit_react` je Schuss
neutralisiert, damit ausschliesslich das Band gemessen wird):
```
  Fall 0: seed 11 slot 5 f576  21A=8042 elev=0 -> LEVEL =  0 | DOWN-Aim = 30 | ohne Liege-Bit = 30
  Fall 1: seed 16 slot 4 f569  21A=8002 elev=0 -> LEVEL =  0 | DOWN-Aim = 30 | ohne Liege-Bit = 30
  Fall 2: seed 24 slot 5 f617  21A=8002 elev=0 -> LEVEL =  0 | DOWN-Aim = 30 | ohne Liege-Bit = 30
  Fall 3: seed 31 slot 5 f171  21A=8050 elev=0 -> LEVEL = 30 | DOWN-Aim = 30 | ohne Liege-Bit = 30
  Fall 4: seed 38 slot 4 f629  21A=8002 elev=0 -> LEVEL =  0 | DOWN-Aim = 30 | ohne Liege-Bit = 30
  Fall 5: seed 59 slot 5 f179  21A=8002 elev=0 -> LEVEL =  0 | DOWN-Aim = 30 | ohne Liege-Bit = 30
```
(Fall 3 traegt beim Nachfahren `21A=8050`, also OHNE Bit 1 — daher 30/30; die Detektion sah
dort `21A=8052`. Der Nachlauf reproduziert 5 von 6 Faellen bitgenau, Fall 3 driftet um einen
Tick. Als Beleg reichen die 5 exakten Faelle.)

**0 von 30 mit dem Bit, 30 von 30 ohne, 30 von 30 mit nach unten gerichtetem Zielen.**
Damit ist die Kausalkette geschlossen: das Band ist die Sperre, nicht `hit_react`.

### 4.2 Der Ablauf, Frame fuer Frame (Trace seed 16 slot 4, echter `game_step`)
```
f549  st=2 s1= 3 s2=3  10E=0004 21A=8004 1D3=00 93=00   HURT laeuft
f550  st=1 s1=11 s2=0  10E=0004 21A=8004                HURT committet 0xB01 = EXEC[11]
f551  st=1 s1=11 s2=1  10E=2004 21A=8002 1D3=80 93=01   EXEC[11] P0 @0x80104438-70
f569  st=1 s1= 1 s2=0  10E=2001 21A=8002 1D3=80 93=01   EXEC[11] P1 -> KRIECHER @0x80104590-98
f570  st=1 s1= 1 s2=1  10E=2001 21A=8002 1D3=0F 93=01   Kriecher-GRIFF P0, +0x1D3 = 15
f585  ...                                1D3=00 93=00   Latch frei — ab hier sperrt NUR das Band
f585..f683                               hp bleibt 74   98 Frames LEVEL-unverwundbar
f684  st=7 s1= 0                         hp=  -1        Griff-Ende toetet den Kriecher
```
Das erklaert den Nutzer-Wortlaut exakt: der Zombie ist ~113 Frames (~2 s) unverwundbar,
und die Phase endet mit dem BISS (dem Kriech-Griff), danach ist der Gegner tot bzw. wieder
normal — "unverwundbar, **bis** sie mich einmal gebissen haben".
Haeufigkeit gemessen: 6 von 64 Seeds (~9 %) in 900 Frames ROOM1140 = "manchmal".

---

## 5. Fix-Rezept fuer den Implementierer

### FIX 1 (Kern) — den KRIECH-Zustand aus dem Port-Liege-Term herausnehmen
`re15_port/engine/src/re15_damage.c:1267-1270`
```c
/* VORHER */
{   int lying = (e->grid_id & 0x80) ||
                (re15_ai_re2_for_type(e->type) && re15_re2z_owns_type(e->type) &&
                 (e->re2z_flags21a & 0x2u));
/* NACHHER */
{   int lying = (e->grid_id & 0x80) ||
                (re15_ai_re2_for_type(e->type) && re15_re2z_owns_type(e->type) &&
                 (e->re2z_flags21a & 0x2u) &&
                 !(e->re2z_f10e & 1u));   /* NICHT im RE2-Kriech-Root */
```
Begruendung, jede Haelfte belegt:
* `+0x21A` Bit 1 wird von EXEC[5] P0 (`|0x202`) und EXEC[11] P0 (`ori 0x2` / `sh v0,538`
  @0x8010443C-48) gesetzt und **ausschliesslich** von EXEC[5] P7 (`andi 0xfffd` / `sh 538`
  @0x801036C8-CC) geloescht. Der Port-Term war (2026-08-24) fuer den Sturz-Flop `0x00020501`
  eingefuehrt, der GENAU durch EXEC[5] laeuft. Der Kriecher-Zweig @0x80104584-98 verlaesst
  EXEC[11] vor P2 und erreicht EXEC[5] P7 nie — eine Lebensdauer, fuer die der Term nie
  gedacht war.
* `+0x10E` Bit 0 identifiziert den Kriech-Root eindeutig: Setzer sind nur
  `andi 0xffc0 / ori 1 / sh 270` @0x80104590-98 sowie die Wiederbelebungen
  `sh 0x2001,270` @0x80106B0C-10 und @0x80107820-24; Leser ist die Zustand-1-Wurzel
  `lhu v0,270(a0)` @0x80101154 / `andi 0x3f` @0x8010115C.
* Fuer genau diesen Zustand hat RE2s eigener Aufloeser **kein** Band-Gate
  (@0x80047124-64, vier Gates, keine word0-Hoehenbits) — der Kriecher ist dort immer Kandidat.
* Der RE1.5-Kriecher bleibt unveraendert gesperrt: der laeuft ueber `grid_id = 0x81`
  (`ori 0x81` / `sb v0,9(v1)` @0x801050D0-D4), also ueber den ERSTEN Term, den der Fix nicht
  anfasst.
* Der Fix ist eine **monotone Verengung**: gemessen tragen 679 von 679 betroffenen Frames
  `10E & 1`; kein anderer Fall aendert sich (Sweep §4.1).

### FIX 2 (Konsistenz, gleiche Datei-Familie) — Kommentar-Zitate korrigieren
`re15_port/engine/src/enemy_ai_re2_zombie.c:2676`, `:2867`, `:2971` (und der Setzer `:2609`)
kommentieren `pl->re2z_self1d3 & 0x80` mit "PL+0x1D3 @0x8010455C-68". Die Instruktionen dort
sind aber `lui v0,0x800d / lbu v0,-565(v0) / andi 0x80` = das **GLOBAL 0x800CFDCB**
(Voll-Scan §3.5). Die Abbildung selbst bleibt (alle Setzer/Loescher decken sich mit der
PL-Klaue), nur die Zitate muessen auf `0x800CFDCB` umgeschrieben werden — sonst sucht der
naechste Leser an der falschen Stelle.

### FIX 3 (gleiche Fehlerklasse, andere Brains — praeventiv)
Der Nachlauf-Filter in `game_step_common.c:1695-1699` laeuft NUR fuer
`re15_re2z_owns_type` (0x10/0x11/0x12/0x13/0x16/0x18). Hund, Kraehe und Spinne haben KEINEN
zentralen Spiegel; ihre Freigabe sitzt im Brain:
* `re15_port/engine/src/enemy_ai_re2_dog.c:1581` und `:1605` (HURT-P2-/P3-Ausgang nach ACTIVE)
* `re15_port/engine/src/enemy_ai_re2_crow.c:1829` — `if (e->state == 1) e->hit_react &= ~1;`
* `re15_port/engine/src/enemy_ai_re2_spider.c:2922` — identisch
Faellt deren Tick aus (globaler Freeze `lw g_pauseflags / lui 0x2000 / and / bne`
@0x8010042C-3C, oder Per-Entity `lbu +0x9 / andi 0x20 / bne` @0x80100450-5C), friert der
Latch — exakt der Defekt, der bei den Zombies zu eb841053 und zum Nachlauf-Filter gefuehrt hat.
Empfehlung: die Schleife auf alle RE2-eigenen Typen ausdehnen bzw. je Brain einen
gleichwertigen, tick-unabhaengigen Spiegel setzen. **NICHT GEMESSEN** (s. §7).

---

## 6. Wie man es verifiziert

Neue Sonde (bereits im Baum, ohne `add_test`, reine Diagnose):
* `re15_port/tests/unit/probe_re2z_bandlock.c`
* Registrierung am Ende von `re15_port/tests/unit/CMakeLists.txt`
* Bauen/Ausfuehren:
```bash
PATH="/c/msys64/mingw64/bin:/c/msys64/usr/bin:$PATH" \
  cmake --build re15_port/build --target probe_re2z_bandlock
cd re15_port && ./build/tests/unit/probe_re2z_bandlock.exe 64 900 3
```
Sie tut zwei Dinge:
1. **Sweep**: zaehlt Frames, in denen ein lebender Zombie im aufrechten Root laeuft und
   `grid&0x80 || 21A&2` traegt, und meldet `(seed, slot, frame)` + `10E/1D3/21A`.
2. **Orakel**: faehrt einen gemeldeten Fall erneut bis zum Frame, stellt den Spieler 1200
   Einheiten frontal davor (gleiche Y = LEVEL) und feuert 30 Schuss durch den ECHTEN
   `re15_player_weapon_fire` — je einmal (a) im Ist-Zustand, (b) mit
   `re15_player_set_aim_elevation_for_test(-1)` und (c) mit von Hand geloeschtem Liege-Bit.

**Soll nach dem Fix:** Spalte `LEVEL` muss in allen Faellen mit `10E & 1` von 0 auf 30 springen;
`Teil 1` darf weiterhin Frames melden (das Bit BLEIBT byte-true stehen), aber die
Treffbarkeit haengt nicht mehr daran. Regressionswache: `nur grid&0x80` muss 0 bleiben und
`test_re2_zombie_hitfilter` / `test_re2_hit_repeat` / `test_re2_zombie_teardeath` gruen
bleiben (`bash re15_port/tools/local_build.sh`, Sollzeile `Tests 224/224`).

Ergaenzende Sonde fuer FIX 3 (noch zu schreiben, Muster = `probe_re2z_bandlock`):
"Gegner X ueber N Frames beschiessen, ohne dass er je angreift -> Trefferzahl > 0", je Typ
{0x10 Zombie, 0x13 Girl, 0x20 Hund, 0x21 Kraehe, 0x25/0x26 Spinne} x
{`RE15_AI_FLAVOR=re15`, `=re2`}, mit erzwungenem Per-Entity-Freeze (`grid_id |= 0x20`) als
zweitem Durchgang — das ist genau die Bedingung, unter der der Zombie-Latch 2026-08-24
eingefroren war (ROOM1200 slot03 typ=0x10 grid=0xA1).

---

## 7. Offene Punkte / NICHT GEFUNDEN

1. **FIX 3 ist NICHT GEMESSEN.** Fuer Hund/Kraehe/Spinne fehlt der Zensus "welcher
   ausgelieferte Spawn traegt `+0x9 & 0x20`" (fuer Zombies existiert er: ROOM1200 slot03
   typ=0x10 grid=0xA1). Naechster Weg: `probe_re2_aim4 --part=8` auf die Nicht-Zombie-Typen
   erweitern, dann die oben skizzierte Sonde.
2. **Der Kriech-Grid-Stempel fehlt im RE2-Zweig.** RE1.5 setzt beim Kriech-Umbau
   `grid_id = 0x81` (@0x801050D0-D4); der Port-RE2-Kriecher hat gemessen `grid = 0x00`.
   Andere Konsumenten von `grid & 0x80` (Downed-Reroute `enemy_ai_common.c:2531`,
   Todes-Zeilenwahl) sehen den RE2-Kriecher deshalb NICHT als liegend. Ob das im RE2-Zweig
   erwuenscht ist, ist NICHT ENTSCHIEDEN — RE2 hat fuer `grid_id`/+0x9 ein anderes Layout und
   ich habe keinen RE2-Zwilling fuer den Stempel gefunden. Nicht geraten, sondern offen gelassen.
3. **Der Nachfahr-Determinismus der Sonde ist nicht 100 %**: Fall 3 (seed 31) driftet zwischen
   Detektionslauf und Orakellauf um einen Tick (`21A 8052` -> `8050`). Ursache nicht ermittelt
   (vermutlich Bank-/Global-Reste zwischen `setup()`-Laeufen). 5 von 6 Faellen reproduzieren
   bitgenau, der Befund haengt nicht daran. Naechster Weg: `setup()` zusaetzlich
   `re15_re2z_rng_reset()` + `re15_enemy_reset()` in fixer Reihenfolge kapseln und die
   Snapshot-Signatur beider Laeufe vergleichen.
4. **Nicht gemessen: andere Raeume.** Der Sweep laeuft nur auf ROOM1140. Der Kriech-Umbau
   haengt nur an Distanz/Peilung/Etage/Claim, ist also raumunabhaengig — ein Gegenbeweis
   waere aber ein Zensus ueber mehrere STAGE1-Raeume.
5. **`+0x21A` Bit 1 wird im Port bewusst NICHT geloescht** (byte-true: einziger Loescher ist
   EXEC[5] P7 @0x801036C8-CC). FIX 1 aendert nur die AUSWERTUNG, nicht das Bit. Falls spaeter
   ein anderer Konsument fuer das Bit dazukommt, muss diese Entscheidung erneut geprueft werden.

---

## 8. Verifikation (unabhaengig nachdisassembliert, 2026-08-25)

**GESAMTURTEIL: TEILWEISE.** Die *Kausalkette* des Dossiers ist bestaetigt und von mir
**A/B nachgemessen** (§8.9). Drei Einzelaussagen sind **falsch bzw. zu stark** und muessen
korrigiert werden: (a) "der EINZIGE Loescher von +0x21A Bit 1" (§8.4), (b) "im Original IMMER
Kandidat" (§8.1), (c) die Tabellenadresse `0x800A68E8` (§8.1). Zusaetzlich fehlt in §3.2 ein
Toter-Code-Hinweis, der genau die Falle ist, gegen die dieser Auftrag laeuft (§8.5).

Werkzeug: eigener MIPS-R3000-Disassembler auf `dis_one()` aus
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, mit explizitem File+Ladeadresse
(das Skill-Tool kennt nur die RE1.5-Binaries). RE2-EXE `t_addr` aus dem PS-X-EXE-Header
(`0x80010000`, text @ Datei `0x800`); `EMOVL10_S0.BIN` RAW @`0x80100000` (53068 B = 0xCF4C,
alle zitierten Adressen liegen im File).

### 8.1 `@0x80047124-64` RE2-Kandidatenfilter — **BESTAETIGT (byte-exakt), Fazit ueberdehnt**

Selbst disassembliert, `info/re2leon/PSX.EXE`, FUN_800470C0 (Prolog @0x800470C0, Schleifenkopf
@0x8004711C `lw s0,0(s2)`, Schleifenfuss @0x8004740C-18):

    8004711c: 8e500000  lw   s0,0(s2)            ; naechster Entity-Zeiger
    80047124: 8e020000  lw   v0,0(s0)
    8004712c: 30420001  andi v0,v0,0x1
    80047130: 104000b6  beq  v0,zero,0x8004740c  ; (1)
    80047138: 920201d3  lbu  v0,467(s0)
    80047140: 144000b2  bne  v0,zero,0x8004740c  ; (2)
    80047148: 86020156  lh   v0,342(s0)
    80047150: 044000ae  bltz v0,0x8004740c       ; (3)
    80047158: 9602010e  lhu  v0,270(s0)
    80047160: 3042c000  andi v0,v0,0xc000
    80047164: 144000a9  bne  v0,zero,0x8004740c  ; (4)

Jede Instruktion **identisch** zum Zitat. Ich habe die GANZE Funktion gelesen
(0x800470C0..0x8004763C): **kein Bitmasken-Test auf die Band-Bits von word0 in der
Kandidaten-Auswahl** — die Kernaussage traegt.

WIDERLEGT als Wortlaut: "ein Kriecher mit +0x1D3==0 und HP>=0 ist im Original **IMMER**
Kandidat". Direkt nach Gate (4) stehen zwei weitere Aussiebungen:

    80047170: 86040098  lh    a0,152(s0)         ; +0x98 Hoehe
    80047174: 8e02003c  lw    v0,60(s0)          ; +0x3C  Y
    80047180: 24420064  addiu v0,v0,100
    80047188: 9603009e  lhu   v1,158(s0)         ; +0x9E
    8004718c: 8e840004  lw    a0,4(s4)           ; Schuss-Hoehe aus arg0[+4]
    80047190: 00431021  addu  v0,v0,v1
    80047194: 00441023  subu  v0,v0,a0
    80047198: 24630064  addiu v1,v1,100
    8004719c: 00031840  sll   v1,v1,1
    800471a0: 0043102b  sltu  v0,v0,v1
    800471a4: 10400099  beq   v0,zero,0x8004740c ; (5) VERTIKALER Ueberlapp-Test
    800471e8: 0c0107be  jal   0x80041ef8         ; (6) Geometrie-/Trefferpruefung
    800471f0: 1040007a  beq   v0,zero,0x800473dc ;     0 -> kein Treffer

Es gibt also sehr wohl eine Hoehenpruefung, nur eben **keine Band-Bitmaske**, die ein
Liege-Flag streichen koennte. Formulierung im Dossier bitte auf "kein Band-Gate" verengen.

Adresse falsch: §3.1 nennt "Waffen-Geometrie-Tabelle @0x800A68E8". FUN_800470C0 liest an
beiden Stellen eine ANDERE Tabelle:

    80047224: 3c01800a  lui at,0x800a
    8004722c: 8c256a88  lw  a1,27272(at)     ; = 0x800A6A88  (identisch @0x80047450)

indiziert mit `+0x8` (Typ) und dem 4. Argument; die 10-Bit-Felder werden mit
`srlv v1,v1,v0` / `andi v1,v1,0x3ff` @0x80047254-5C gezogen, `s6 = s5>>28` @0x80047114.
`0x800A68E8` existiert zwar (Xrefs @0x80043A68/0x80043AF0/0x80047E04/…), kommt in FUN_800470C0
aber **nicht** vor.

Nebenbefund (nicht im Dossier): es GIBT einen word0-Bit-Test in der Funktion —
`800472dc: 3c031000 lui v1,0x1000 / 800472e0: 00431024 and v0,v0,v1` (und identisch
@0x80047500-04), aber **erst nach** der Trefferannahme und nur zur Wahl der Trefferzone
`+0x1D2` (`sb v0,466(s1)` @0x8004730C). Er kann keinen Kandidaten aussieben.

### 8.2 `@0x8010443C-70` EXEC[11] P0 — **BESTAETIGT (byte-exakt)**

    80104438: 3042fffb  andi v0,v0,0xfffb
    8010443c: 34420002  ori  v0,v0,0x2
    80104448: a602021a  sh   v0,538(s0)
    8010444c: 920201d3  lbu  v0,467(s0)
    80104458: 34420080  ori  v0,v0,0x80
    8010445c: a20201d3  sb   v0,467(s0)
    80104460: 9602010e  lhu  v0,270(s0)
    80104468: 34422000  ori  v0,v0,0x2000
    80104470: a602010e  sh   v0,270(s0)

Alle neun Woerter identisch. **Kein toter Code:** die Funktion beginnt @0x8010439C
(`27bdffe0 addiu sp,sp,-32`), dispatcht auf `+0x6` (`92030006 lbu v1,6(s0)` @0x801043B8;
0 -> 0x801043F8, 1 -> 0x8010449C, 2 -> 0x801045D4, sonst Epilog), und P0 committet `+0x6 = 1`
in `80104410: 24020001 addiu v0,zero,1` / `80104418: a2020006 sb v0,6(s0)`.
Ergaenzung, die im Dossier fehlt: P0 schreibt im selben Block auch
`word0 = (word0 & 0xF3FFFFFF) | 0x04000000` (@0x8010441C / 0x80104428 / 0x80104440-54).

### 8.3 `@0x80104530-B0` Kriecher-Umbau (EXEC[11] P1) — **BESTAETIGT (byte-exakt)**

Nachdisassembliert; alle zitierten Woerter stimmen. Zwei Praezisierungen:

* Die Reihenfolge im Dossier-Listing ist verdreht. Real:
  `80104584 24030101 addiu v1,zero,257` / `80104588 9602010e lhu v0,270(s0)` /
  `8010458c ae030004 sw v1,4(s0)` / `80104590 3042ffc0 andi v0,v0,0xffc0` /
  `80104594 34420001 ori v0,v0,0x1` / `80104598 a602010e sh v0,270(s0)`.
* Vorgeschaltet steht ein Gate, das das Dossier nicht nennt:
  `8010450c: 9202014d lbu v0,333(s0)` / `80104514: 2c420012 sltiu v0,v0,0x12` /
  `80104518: 14400028 bne v0,zero,0x801045bc` — der Kriecher-Test laeuft erst ab Clip-Frame 0x12.

**Der entscheidende Punkt haelt:** nach `801045b0 a022fdcb sb v0,-565(at)` folgt
`801045b4: 0804117c j 0x801045f0` = Epilog, und die Funktion endet
`80104604: 03e00008 jr ra`. P2 (@0x801045D4 `34420501 ori v0,v0,0x501`, mit `lui v0,0x2`
aus dem Delay-Slot @0x801043EC ⇒ `+0x4 = 0x00020501`) wird auf diesem Zweig **nie** betreten.

### 8.4 `@0x801036C8-CC` "der EINZIGE Loescher von +0x21A Bit 1" — WIDERLEGT

Vollscan **aller 75** `sh rX,538(rY)`-Stores in `EMOVL10_S0.BIN` mit Rueckverfolgung der
Definitionskette des gespeicherten Registers. **FUENF** Stellen loeschen Bit 1, nicht eine:

| Adresse | Instruktionen | Ort |
|---|---|---|
| `0x8010087C` | `a640021a  sh zero,538(s2)` | Spawn/Init (Kontext @0x80100858-8C) |
| `0x801036C8-CC` | `3042fffd andi v0,v0,0xfffd` / `a642021a sh v0,538(s2)` | EXEC[5] **P7** (die zitierte) |
| `0x80103748-50` | `3063ffed andi v1,v1,0xffed` / `a643021a sh v1,538(s2)` | EXEC[5] **P8** (loescht 0x10 und 0x2) |
| `0x801049DC` | `a620021a  sh zero,538(s1)` | Init (Kontext @0x801049C4-E4) |
| `0x80107EA0-A4` | `3063ffed andi v1,v1,0xffed` / `a623021a sh v1,538(s1)` | HURT-Wurzel 0x80107438, Tail @0x80107E58 |

Ebenso ungenau ist §5s "gesetzt von EXEC[5] P0 (|0x202) und EXEC[11] P0": Setzer sind **vier** —
`ori v0,v0,0x2` @0x80100C28 (Init) und @0x8010443C, `ori v0,v0,0x202` @0x801032E0 (EXEC[5] P0)
und @0x80104090 (EXEC[9]).

**Folgen fuer das Fazit: KEINE.** Alle fuenf Loescher liegen im Init-, Aufsteh- oder HURT-Pfad;
der laufende Kriecher erreicht keinen davon (er verlaesst EXEC[11] vor P2 und ist — das ist ja
der Defekt — nicht beschiessbar, also erreicht er auch die HURT-Wurzel nicht).
Bemerkenswert: der **Port-Kommentar** in `re15_damage.c` (Arbeitsstand, s. §8.8) listet diese
fuenf Stellen bereits korrekt — nur der Dossier-Text §3.4/§5 ist falsch geblieben.

**Neuer Nebenbefund (echte Port-Luecke, im Dossier nicht erwaehnt):** der Port implementiert
zwei dieser Original-Stores gar nicht.

* `enemy_ai_re2_zombie.c` (default-case = EXEC[5] P8) portiert `+0x1D3 &= 0x7f` (@0x80103718-28)
  und `+0x10E &= ~0x2000` (@0x8010373C-4C), **aber nicht** `+0x21A &= 0xffed` (@0x80103748-50).
* `@0x80107EA0-A4` hat ueberhaupt keinen Zwilling (`grep -n "0x80107ea\|~0x12"` = 0 Treffer).

Damit bleibt `+0x21A & 2` im Port auch nach einem **regulaeren** Aufsteh-Ausgang stehen, wo das
Original es raeumt. Das ist unabhaengig von FIX 1 und gehoert nachgezogen.

### 8.5 `@0x801015c0-0x8010163c` RE1.5-Band-Stempel — BESTAETIGT, aber Kontext fehlt (TOTER STORE)

Byte-exakt bestaetigt (STAGE1.BIN):

    801015c0: lbu v0,9(a0)   / 801015c8: andi v0,v0,0x80 / 801015cc: bne v0,zero,0x80101600
    80101600: jal 0x80012aa4 / 80101604: ori a0,zero,0xbb8
    80101614: lbu v0,9(a1)   / 8010161c: andi v0,v0,0x80 / 80101620: beq v0,zero,0x80101640
    80101624: lui v1,0xbfff  / 80101628: ori v1,v1,0xffff      ; ~0x40000000
    80101630: ori a0,zero,0x1388 / 80101634: and v0,v0,v1
    80101638: jal 0x80012974 / 8010163c: sw v0,0(a1)

DOWN-Band gegengeprueft: `800129cc: sltu s0,a1,s0` / `800129d0: beq s0,zero,0x800129f4` /
`800129d4: lui a0,0x2000` / `800129ec: or v1,v1,a0` / `800129f0: sw v1,0(v0)` ⇒ `|= 0x20000000`
wenn `dist < a0`. Passt zum Port-Zitat `@0x800129cc-f0`.

**Was das Dossier weglaesst** (und was genau die Falle dieses Auftrags ist): der
NICHT-liegend-Zweig @0x801015D4-FC

    801015d4: lw  v0,0(a0)   / 801015d0: lui v1,0x1fff / 801015d8: ori v1,v1,0xffff
    801015dc: and v0,v0,v1   / 801015e0: sw v0,0(a0)
    801015f0: lw  v0,0(a0)   / 801015f4: lui v1,0x4000
    801015f8: or  v0,v0,v1   / 801015fc: sw v0,0(a0)

ist ein **TOTER STORE**: `jal 0x80012aa4` @0x80101600 macht als erstes selbst

    80012aec: lui a1,0x1fff / 80012af0: lw v0,0(a0) / 80012af4: ori a1,a1,0xffff
    80012af8: and v0,v0,a1  / 80012afc: sw v0,0(a0)

und wischt das gerade gesetzte `0x40000000` wieder weg. Das ueberlebende LEVEL-Bit setzt
FUN_80012aa4 selbst: `80012b20: sltiu v0,s0,0x2` (Etagendifferenz < 2) ->
`80012b3c: lui a0,0x4000 / 80012b40: or v1,v1,a0 / 80012b44: sw v1,0(v0)`.
Das aendert das Fazit **nicht** (der Port faehrt ohnehin `re15_band_stamp_aa4`, dessen Kopf das
dokumentiert), aber "NICHT-downed ⇒ LEVEL" waere aus dem Dossier-Zitat allein falsch abgeleitet.

### 8.6 `@0x801050CC-F4` RE1.5-Kriecher `grid_id = 0x81` — **BESTAETIGT (byte-exakt)**

    801050b0: ori v0,zero,0x4  / 801050b4: sb v0,471(v1)    ; +0x1D7 = 4
    801050c4: lb  v0,159(v1)                                ; GATE: +0x9F != 0
    801050cc: beq v0,zero,0x80105100
    801050d0: ori v0,zero,0x81 / 801050d4: sb v0,9(v1)      ; grid_id = 0x81
    801050e4: sw  a1,4(v0)
    801050f0: ori v0,zero,0x8  / 801050f4: sb v0,471(v1)    ; +0x1D7 = 8
    8010510c: andi v0,v0,0xfe  / 80105110: sb v0,147(v1)    ; +0x93 &= 0xfe

Funktionswurzel FUN_80104F80 bestaetigt (`lbu v1,6(a0)` @0x80104F90, Phasen 0/1/2).
Das Dossier verschweigt das `+0x9F != 0`-Gate @0x801050C4-CC.

**Beleg nachgeliefert fuer die unbelegte Behauptung "RE1.5 erzeugt Kriecher nur ueber das
Raumskript (+0x1C4 & 0x1000)":** ich finde in STAGE1.BIN genau drei Stellen, die den
Kriech-Zustand `0x1001` committen, und alle drei haengen an diesem Spawn-Flag —

    80101ec4: lhu v0,452(v1) / 80101ecc: andi v0,v0,0x1000 / 80101ed0: beq v0,zero,0x80101edc
    80101ed4: ori v0,zero,0x1001            (ebenso @0x801021CC-DC und @0x80105790-A0)

Praezisierung: die Entscheidung faellt sehr wohl **in den KI-Decide-Leaves**, also im Kampf —
aber nur fuer Entities, die der Raum mit `+0x1C4 & 0x1000` dafuer freigegeben hat. "Nie spontan
im Kampf" ist damit als *Wirkung* richtig, als *Mechanismus-Beschreibung* zu grob.

### 8.7 `0x800CFDCB` ist ein GLOBAL (Doku-Fehler im Port) — **BESTAETIGT, exakt**

Eigener Vollscan (op in {lb,lbu,lh,lhu,lw,sb,sh,sw}, `imm == 0xfdcb`): **19 Treffer im Overlay,
1 in der EXE** — Adressliste deckungsgleich mit §3.5, inklusive der fuenf Stores
(@0x80102FC0, @0x80103B2C, @0x801045B0, @0x80104FAC, @0x801082F4) und des EXE-Treffers
@0x80052070. Basis stichprobenartig verifiziert:
`80102f48: 3c02800d lui v0,0x800d` / `80102f4c: 9042fdcb lbu v0,-565(v0)` (ebenso @0x80103AD0,
@0x80104F90). Loescher: `80104fa0: 3042007f andi v0,v0,0x7f` / `80104fac: a022fdcb sb v0,-565(at)`.

Die Port-Zeilen `enemy_ai_re2_zombie.c:2676/:2867/:2971` lesen tatsaechlich
`pl->re2z_self1d3 & 0x80u` und zitieren `@0x8010455C-68` / `@0x80102F4C-58` / `@0x80103AD4-E0`,
und die zugehoerigen Blockkommentare (:2669, :2860, :2965) etikettieren das als
"Spieler+0x1D3" ⇒ **Doku-Fehler bestaetigt**. Die Alias-Abbildung ist plausibel gestuetzt:
`80102754: lbu v0,467(s3) / 8010275c: ori v0,v0,0x80 / 80102760: sb v0,467(s3)` (s3 = Spieler)
liegt unmittelbar neben `80102770: sb v1,467(s1)` mit `v1 = 15` (s1 = Zombie).

### 8.8 Port-Zitate — **BESTAETIGT, mit einer wichtigen Einschraenkung**

* `re15_damage.c:1267-1275`: das Dossier-Zitat stimmt **wortgleich** mit
  `git show HEAD:re15_port/engine/src/re15_damage.c` (Zeilen 1267-1275).
  **Der Arbeitsbaum traegt FIX 1 bereits (uncommitted, +33/-1).** Aktuell steht der Term in
  Zeile **1298-1307** und enthaelt `&& !(e->re2z_f10e & 1u)`. Wer das Dossier heute liest, findet
  den "VORHER"-Stand nur noch in HEAD.
* `enemy_ai_re2_zombie.c:7443-7479` (`re15_re2z_hit_filter_apply`, Vier-Gate-Spiegel) und
  `game_step_common.c:1695-1699` (tick-unabhaengige Schleife, in `if (c->rdt_ok)`): **verbatim
  vorhanden**.
* §6 ist veraltet: `probe_re2z_bandlock` ist inzwischen als Test registriert
  (`re15_port/tests/unit/CMakeLists.txt:2786  add_test(NAME unit_re2z_bandlock_pin ...)`), nicht
  mehr "ohne add_test".

**Zusatz-Beleg fuer die FIX-1-Praedikatswahl, den das Dossier nicht bringt:** `+0x10E & 1` ist
buchstaeblich der Wurzel-Selektor, nicht bloss ein Merkmal:

    80101154: 9482010e  lhu  v0,270(a0)
    8010115c: 3042003f  andi v0,v0,0x3f
    80101160: 00021080  sll  v0,v0,2
    8010116c: 8c22c854  lw   v0,-14252(at)      ; Tabelle 0x8010C854
    80101174: 0040f809  jalr v0

und die Tabelle alterniert strikt nach Bit 0:
`[0]=0x8010118C [1]=0x80101210 [2]=0x8010118C [3]=0x80101210 ...` — `0x8010118C` = aufrechte
Wurzel (`lbu v0,5(s0)` @0x801011A8), `0x80101210` = Kriech-Wurzel (`lbu v0,5(s0)` @0x8010122C ->
**andere** Tabelle `0x8010C90C` @0x80101240). `!(f10e & 1)` ist damit byte-true
"nicht im Kriech-Root".

### 8.9 Messung — **UNABHAENGIG REPRODUZIERT (A/B, echte Binaries)**

Ich habe nicht nur nachgerechnet, sondern den Beweis **gegenprobiert**: einmal mit dem
Arbeitsbaum (FIX 1 drin), einmal mit `re15_damage.c` auf HEAD zurueckgesetzt und neu gebaut
(danach wiederhergestellt, Baum unveraendert `+33/-1`).

`./build/tests/unit/probe_re2z_bandlock.exe 64 900 3` — Teil 1 **in beiden Laeufen identisch**
und Ziffer fuer Ziffer wie im Dossier:

    aufrechte Zombie-Frames gesamt : 66223
    davon mit Liege-Bit            : 679  (1.03%) in 6/64 Seeds
       nur grid&0x80 : 0    nur 21A&2 : 679    beide : 0
    [STALE] seed 11/16/24/31/38/59 ... 10E=2001 in ALLEN sechs Faellen

Teil 2 (Orakel), **Spalte LEVEL**:

| Fall | seed/slot/frame | 21A | HEAD (vor FIX 1) | Arbeitsbaum (mit FIX 1) |
|---|---|---|---|---|
| 0 | 11/5/f576 | 8042 | **0** | **30** |
| 1 | 16/4/f569 | 8002 | **0** | **30** |
| 2 | 24/5/f617 | 8002 | **0** | **30** |
| 3 | 31/5/f171 | 8050 | 30 | 30 |
| 4 | 38/4/f629 | 8002 | **0** | **30** |
| 5 | 59/5/f179 | 8002 | **0** | **30** |

`DOWN-Aim` und `ohne Liege-Bit` sind in beiden Laeufen durchgehend 30/30. Das ist exakt das im
Dossier §6 formulierte Soll — der Kausalnachweis (Band = Sperre, nicht `hit_react`) **haelt**,
und FIX 1 hebt sie auf, ohne einen anderen Fall zu bewegen. Auch Fall 3 driftet bei mir
reproduzierbar (`21A 8052` in der Detektion -> `8050` im Orakel), wie in §7.3 eingeraeumt.

`./build/tests/unit/probe_re2z_abc.exe 64 900 3` liefert wortgleich:

    (C) Untreffbar >= 200 Frames bei hp>=0: 0 Vorfaelle in 0/64 Seeds
    --- (C) nach Ort: EXEC[7] liegend=0  EXEC[8] fressend=0  EXEC[5] Sturz=0  HURT=0  SONST=0

⇒ die Widerlegung der Auftrags-Hypothese (`re2z_hit_latch_release` haengt) ist bestaetigt.

### 8.10 Was ich NICHT verifiziert habe

1. **Der Frame-Trace §4.2** (f549...f684, "98 Frames LEVEL-unverwundbar", Tod durch den
   Kriech-Griff). Die Sonde gibt in der von mir gefahrenen Betriebsart nur die `[STALE]`-Zeilen
   aus; die eine reproduzierte Zeile (seed 16 slot 4 f569, `21A=8002 1D3=80 10E=2001 hp=74
   clip=10`) stimmt exakt. Die Frame-Kette selbst ist **NICHT NACHGEPRUEFT**. Naechster Weg:
   Trace-Modus der Sonde bzw. ein Zaehler ueber `hp`-Konstanz zwischen erstem `10E&1` und
   `state==7`.
2. **§5 FIX 3** (Hund/Kraehe/Spinne) — vom Dossier selbst als "NICHT GEMESSEN" markiert; ich habe
   es ebenfalls nicht geprueft.
3. **Ob RE2 selbst je einen Kriecher mit `+0x21A & 2` in eine Situation bringt, in der das Bit
   stoert** — irrelevant, weil RE2s Aufloeser das Bit nachweislich nicht liest (§8.1), aber ich
   habe nicht alle Leser von `+0x21A` in `EMOVL10_S0.BIN` einzeln klassifiziert.
4. **Die zwei fehlenden Port-Loescher (§8.4)** habe ich statisch belegt, aber nicht gemessen,
   wie oft sie im Spiel greifen wuerden. Naechster Weg: `probe_re2z_bandlock` Teil 1 um einen
   Zaehler "verlaesst EXEC[5] ueber P8 mit `21A&2` noch gesetzt" erweitern.

> **Hinweis zum Zeitpunkt:** meine A/B-Messung (§8.9) lief gegen den Arbeitsbaum-Stand mit
> genau FIX 1 (`re15_damage.c` +33/-1 gegen HEAD). Waehrend ich dieses Kapitel schrieb, hat ein
> paralleler Lauf in denselben Block eine ZWEITE Ausnahme eingebaut (Boden-Aufsteher,
> `re2z_flags21a & 0x10`, jetzt `re15_damage.c:1315-1324`, Diff +57/-4). Der FIX-1-Term
> `!(e->re2z_f10e & 1u)` steht unveraendert drin (Zeile 1323); meine Zahlen sind also weiterhin
> die Aussage ueber FIX 1, aber ein Nachlauf der Sonde misst jetzt BEIDE Ausnahmen zusammen.
