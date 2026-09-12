# BEFUND A — Gruppe „kopf-wegschiessen": Schrot toetet nah, aber der Kopf platzt nicht

Nutzer 2026-09-12: „Man kann den Zombies mit der Shotgun noch nicht den Kopf wegschiessen."
Stand: der RE2-Nah-Kopfschuss TRIFFT (UP-Fenster-Fix, re15_damage.c) und toetet — die
Kopf-Explosion fehlt. Analyse 2026-09-12, alle Dumps/Disasms frisch gezogen
(`re2_disasm.py dis/table/bytes … --bin EMOVL10_S0.BIN|PSX.EXE`, info/re2leon;
`re15_disasm.py … --bin STAGE1.BIN`, info/Re1.5).

## Kurzfassung (Wurzelursache)

RE2 dispatcht den Schrot-Kopf-Tod **NICHT ueber die Kopf-Spalte** und **NICHT ueber eine
Zonen-Verzweigung in der DEATH-Wurzel**. Die Kette ist:

1. Der Zombie traegt **nie das Kopf-Bit**: INIT stempelt `word0 |= 0x0C000000` (nur
   Beine+Rumpf, @0x80100984-998); ein `lui 0x1c00`/Kopf-Bit-Setzer existiert im gesamten
   Zombie-Overlay nicht (vollstaendiger lui-Scan, §2.3). Der Hitscan-Zonenwuerfel
   (Maskentabelle 0x800A6DB4 gegen `word0>>26&7`) liefert fuer den Nah-UP-Schuss deshalb
   **Zone 1 = RUMPF**, nicht 2 — `+0x1D2 = 1`, die NULL-Spalte 2 ENTSTEHT nie (§2.4).
2. Zelle `death_tbl[7][1]` = **FUN_801066FC** (Ragdoll). Deren Phase 0 prueft ALS ERSTES:
   **`lw v0,0x800CFBF8 / bgez` @0x8010679C-A8 — das ist `player.word0 < 0` = Bit 31 =
   ZIELEN-HOCH.** Ist es gesetzt: `+0x231 = 2` (@0x801067B0) und `jal 0x801092C4`
   (@0x801067BC) = **die Kopf-Explosions-Routine der Magnum** — deterministisch, VOR dem
   rand-Wurf. Der beruehmte „Shotgun nah + hoch = Kopf ab" ist wortwoertlich:
   *Rumpf-Todeszelle + Spieler-zielt-hoch-Bit → Magnum-Kopfplatzer.*
3. Der Port hat den 92C4-Eingang in `re2z_hit_ragdoll` laengst (`re2z_rag231 == 2`,
   enemy_ai_re2_zombie.c:5615-5617), aber **sein Setzer ist als OPEN unverdrahtet**
   (:5619-5623, „0x800CFBF8 ohne Port-Produzent"). Der Nutzer bekommt darum 3/4 Burst
   (Fleischbrocken) und 1/4 Sturz-mit-Kriecher-Wiederbelebung — nie den Kopf.

Der Fix ist eine handvoll Zeilen: den Zielen-HOCH-Zweig in `re2z_hit_ragdoll` Phase 0
verdrahten (§5.1) und den heutigen +0x1D2-Stempel von „UP→2" auf das byte-hergeleitete
„UP→1" korrigieren (§5.2). Die heutige Spalten-Klemme landet zufaellig schon auf der
richtigen Zelle; sie bleibt als Sicherung, ist aber nicht der Mechanismus (§5.3).

---

## 1. Antwort auf die Kernfrage: WIE dispatcht RE2 den Kopf-Tod?

**Die DEATH-Wurzel liest +0x1D2 NUR als Spaltenindex — es gibt KEINE Zonen-Verzweigung
vor der Tabelle.** FUN_80108250 vollstaendig disassembliert (EMOVL10_S0.BIN raw
@0x80100000):

```
80108260-98  Kill-Latch +0x21A|=0x4000, Zaehler 0x800D46C0++
801082a8-ac  +0x21A &= ~0x200
801082b0-324 Grab-Claim-Abbruch (0x800CFBFC==5 && 0x800CFDAC==self)
80108328-ac  nur Phase 0: fuenf Modell-Flagwoerter |= 0x8000
801083b0-dc  ZWEIG 1  (+0x10E & 1)  -> 1D-Tabelle 0x8010CECC[+0x5]
801083e4-404 ZWEIG 2  (+0x21A & 0x10) -> jal 0x801099E4 (Liegend-Tod)
80108408-dc  ZWEIG 3  (+0x21A & 0x2)  -> Blut/SE/Zerleger-Leiter, +0x4=0x907
801084e0-518 ZWEIG 4  der 2D-Dispatch:
  801084e4  lbu v1,5(a0)        ; ZEILE = Angriffs-Id (+0x5)
  801084ec  addiu a2,a2,-13276  ; Basis 0x8010CC24
  801084fc  lbu v1,466(a0)      ; SPALTE = +0x1D2   <- der EINZIGE +0x1D2-Lesezugriff
  8010850c  lw v0,0(v1)
  80108514  jalr v0             ; KEIN beq v0,zero -> NULL-Zelle waere Absturz
```

Zwischen 0x80108250 und 0x80108514 gibt es **keinen weiteren `lbu …,466`** und keinen
Kopf-Sonderzweig. Die Aussage des kopfschuss-nah-Dossiers „+0x1D2=2 erreicht nur die
DEATH-Wurzel (Kopf-Tod/Enthauptung)" ist damit **widerlegt**: +0x1D2=2 wuerde in Zeile 7
`jalr 0` ausfuehren. Sie entsteht nur deshalb nie, weil die Zone 2 fuer Zombies gar nicht
produziert wird (§2.3/§2.4).

## 2. Der Erzeuger: was der Hitscan wirklich stempelt

### 2.1 +0x1D2 und +0x5 (Tail von FUN_800410CC, info/re2leon/PSX.EXE)

```
80041a6c  sll v1,s1,1          ; s1 = BRACKET (0 nah / 1 mitte / 2 fern)
80041a88  lw  t0,112(sp)       ; = ZONE (s7 des Maskentreffers)
80041a8c  addu v1,v1,s1        ; v1 = 3*bracket
80041a90  addu v1,v1,t0        ; + zone
80041a9c  sb  v1,466(t0')      ; +0x1D2 = 3*bracket + zone
80041aa8  srl v1,t0,16         ; Waffen-Record-Index (param_2 >> 16)
80041ab0  addiu v1,v1,1
80041ab4  sb  v1,5(t0')        ; +0x5 (ZEILE) = Index + 1  (Schrot: 6+1 = 7)
```

### 2.2 Die Zone kommt aus der Maskentabelle gegen die TEILE-MASKE des Gegners

```
80041478/7c  Basis 0x800A6DB4 (+ satz*9 + drittel)
80041488  lbu v1,2(a0)   ; Prioritaet [0] > [1] > [2] (letzter Schreiber gewinnt)
80041490  and v0,v1,s6   ; s6 = enemy.word0 >> 26 & 7  (Teile-Maske!)
8004149c  srl s7,v1,1    ; Teil = Maskenbyte >> 1  (4->2 Kopf, 2->1 Rumpf, 1->0 Beine)
```

Maskentabelle 0x800A6DB4 (frisch gedumpt): Satz0 `[04 02 01][02 01 04][01 02 04]`,
Satz1(+9) `[04 02 00][02 00 00][01 02 00]` (Zeilen = unteres/mittleres/oberes
Fenster-Drittel). Satz-Wahl = Flag-Byte der Sub-Box `&8`; Schrot-Records (Dump
@0x800A6724/40/5C): DOWN-Flags `04 02 01` (Satz0), LEVEL `08 08 08` (Satz1),
UP `10 20 40` (Satz0).

### 2.3 ⛔ Der Zombie hat KEIN Kopf-Bit — der Produzent der Teile-Maske (bisher OPEN)

Zombie-Overlay EMOVL10_S0.BIN (== EMZ0.BIN, byte-identische Trefferliste):

- **INIT (Zelle 0 der Zustands-Tabelle @0x8010C830 → 0x8010065C):**
  `lw v0,0(s2) / lui v1,0xc00 / or / sw v0,0(s2)` **@0x80100968-998** —
  `word0 |= 0x0C000000` = **Beine + Rumpf. Kein 0x10000000.**
- Kriecher-/Liege-Varianten setzen nur BEINE: `lui v0,0x400 / or / sw` @0x80100B38-44
  und @0x80100C0C-1C.
- Der Zombie-Root pflegt die Maske pro Frame: `(+0x10E&1 || +0x21A&2) -> word0|=0x04000000`
  @0x8010037C-A8; und nimmt Liegenden das Bein-Bit, wenn der Spieler nah dran steht
  (`player.+0x154 & 0x8000: playerY-1500 < zombieY -> word0 &= ~0x04000000` @0x801003E8-41C,
  Zwilling mit 0x4000/1000 @0x80100420-58).
- **Vollstaendiger Scan aller `lui` im Overlay: kein einziges `lui 0x1c00` und kein
  `lui 0x1000`-OR auf word0** (die vier 0x1000-Treffer @0x801002E8/0x801067E4/0x801077C0/
  0x8010895C sind alle `0x800CFBD8 & 0x10000000`-Reads eines Globals, kein word0-Store).

Das Bit 0x10000000 („Kopftreffer erlaubt", Konsument des AoE-Appliers @0x800472E0) ist
also eine Eigenschaft ANDERER Gegnertypen — beim Zombie kommt Zone 2 im Hitscan **nie** an.

### 2.4 Byte-hergeleitete Zonen fuer den stehenden Zombie (Maske = 3)

| Aim | Fenster (Schrot, dy=0) | Drittel | Maskenzeile | Ergebnis |
|---|---|---|---|---|
| HOCH | UP [-5000,+500], nur Nah-Box | unteres | Satz0 `[4,2,1]`: 4&3=0, **2&3 → 1** | **RUMPF (1)** |
| EBEN | LEVEL [-3000,+2000] | mittleres | Satz1 `[2,0,0]`: **2&3 → 1** | **RUMPF (1)** |
| TIEF | DOWN [-500,+3000] | oberes | Satz0 `[1,2,4]`: 4&3=0, 2&3→1, **1&3 → 0** (letzter gewinnt) | **BEINE (0)** |

Kriecher/Liegender (Maske = 1): alle erreichbaren Zeilen liefern BEINE (0); die
Satz1-Mitte `[2,0,0]` matcht gegen Maske 1 GAR NICHT → uVar7 bleibt 0 → **kein Treffer**
(die bekannte „Kriecher nur mit TIEF-Zielen treffbar"-RE2-Mechanik, gehoert zur
Fenster-Kandidatur des kopfschuss-nah-Fixes).

## 3. Der Kopf-Explosions-Dispatch: FUN_801066FC Phase 0

Zelle `DEATH[7][1] = 0x801066FC` (Tabelle @0x8010CD20, abtrennen-Dossier §2.1). Der Kopf
des Handlers, frisch disassembliert:

```
80106738  lbu v0,561(s2)      ; +0x231-Router:
80106740  bne v0,1 -> …       ;   1 -> jal 0x80109610 (Burst)      @0x80106748
8010675c  beq v0,2 -> 801067b4;   2 -> jal 0x801092C4 (KOPF AB)    @0x801067BC
; Phase 0 (+0x231 == 0):
8010679c  lui v0,0x800d
801067a0  lw  v0,-1032(v0)    ; 0x800CFBF8 = word0 der SPIELER-Entity
801067a8  bgez v0,0x801067cc  ; Bit 31 NICHT gesetzt -> weiter
801067b0  sb  v0(=2),561(s2)  ; ZIELEN-HOCH -> +0x231 = 2
801067b4  …
801067bc  jal 0x801092c4      ; -> KOPF-EXPLOSION (deterministisch, KEIN rand-Wurf!)
; sonst:
801067cc  lh v0,268(s2); bne  ; +0x10C != 0            -> +0x231=1 -> Burst
801067dc  lw 0x800CFBD8 & 0x10000000                    -> +0x231=1 -> Burst
801067f4  jal rand; andi 3; beq zero -> Ragdoll-Sturz
80106808  lbu v1,4(s2); bne v1,3 -> Ragdoll-Sturz       ; nur DEATH wuerfelt in den Burst
8010681c  sb 1,561 -> jal 0x80109610                    ; 3/4 -> Burst
```

**Beweis, dass 0x800CFBF8 die Spieler-Entity ist:** `DAT_800ce330 = &DAT_800cfbf8`
(RE2_Quellcode_V2/FUN_80049e48.c:133, Spieler-Slot-Registrierung); der Zombie-Root liest
`0x800CFC30/0x800CFC38` = +0x38/+0x40 (X/Z) derselben Basis als Spielerposition
(@0x801002D0-DC mit @0x801096B0-BC).
**Beweis, dass Bit 31 = ZIELEN-HOCH:** Waffen-Geometrie-Selektor FUN_80047C6C:
`s2 = player.word0 >> 0x1d` @0x80047D18, Gruppentabelle DAT_800A6F8C
`[FF,0,1,FF,2,…]` → DOWN(1)→0, LEVEL(2)→1, **UP(4)→2** (kopfschuss-nah §3.1, byte-
verifiziert); Band-Kodierung identisch RE1.5 (One-Hot 0x20000000/0x40000000/0x80000000,
RE2-Stamper FUN_80042368: `*w &= 0x1fffffff; … |= 0x80000000`).

**Warum das GENAU der gemeldete Move ist:** Schrot UP trifft bodengleiche Zombies NUR in
der Nah-Box (UP-Fenster [-5000,+500], kopfschuss-nah §3.2), Schaden 200 ≥ max. HP → immer
DEATH → Zeile 7 / Spalte 1 → 66FC → Zielen-HOCH-Bit gesetzt → **Kopf-Explosion, 100%
deterministisch**. Level gezielt (Bit 30) → derselbe Handler wuerfelt 3/4 Burst, 1/4
Ragdoll-Sturz mit Kriecher-Wiederbelebung.

Zur Vollstaendigkeit — **was die Nachbar-Zellen tun** (Selbst-Disasm):
- `FUN_80109610` (Burst) reisst **keine Teile ab**: Blut 8096+6096 an Part 0, Schleife
  `(rand&3)+1` Fleischbrocken-Emitter `0x0902xxxx` (@0x80109780-88), Rueckstoss
  +0x144=400/-50er-Decay, Clip via 0x000F0F00-Wort. Kopf bleibt dran.
- `FUN_801092C4` (die Zelle der Magnum-Zeilen 5/6, Spalte 0/1) ist die einzige
  Kopf-Abriss-Routine dieser Tabelle: Part 8 `flags|=0x4A` + Drift-Physik + Tinte
  0x00101040 + Stumpf-Marker Part 0 (@0x80109460-C8), 50% laeuft der Kopflose weiter
  (@0x801094D8-F0). Magnum enthauptet also IMMER (auch level gezielt), Schrot nur bei
  Zielen-HOCH, Custom-Schrot (Zeile 8, Spalte 1) zerreisst komplett (0x80108BEC).
- Auch der HURT-Pfad nutzt 66FC (HURT[7][1]): die Zielen-HOCH-Pruefung laeuft dort
  VOR der `+0x4==3`-Abfrage — ein UEBERLEBTER Rumpf-Treffer mit gehaltenem HOCH-Zielen
  (nur bei erhoehtem Gegner moeglich) enthauptet im Original ebenfalls. Byte-true
  uebernehmen, nicht weg-gaten.

## 4. RE1.5-Original: hat der Auslieferungsstand eine Enthauptung? — NEIN

- Der RE1.5-Nah-Crit setzt nur `hp = 0xffff` (`(w==8 && DAT_8008f5e0<3000)||w==7` →
  Bit 0x40 → hp=-1, @0x800124FC-2C, kopfschuss-nah §2.5). Keine Gore-Feld-Writes.
- Der RE1.5-Zombie-Death ist ein reiner Clip-Dispatcher: `FUN_8011DB88` (STAGE1-Overlay,
  Zustand 3 der Tabelle PTR_FUN_801217A0) = `DAT_801217D0[+0x6]()` mit +0x6 = vertikale
  Trefferrichtung; die Tabelle @0x801217D0 zeigt auf die **generischen EXE-Anim-Executor**
  0x80050CB8 / 0x80050DDC / 0x80050F00 / 0x80051024 / 0x80051148 / 0x80051484
  (frisch gedumpt aus STAGE1.BIN; 0x80050DDC ist katalogisiert als „Generic anim-phase
  advance", RE15_FUN_CATALOG.md:73). Kein Part-Record-Block, keine Mesh-Verstecke.
- RE1.5-Zombie-EMDs tragen keine Stumpf-Reserve-Meshes (mesh_count == bone_count == 15,
  Messung EM10/11/12/13/16/18, enemy_ai_re2_zombie.c:6138-6141).

Der vom Nutzer gewuenschte Kopf-Abriss ist also eine reine RE2-Mechanik — im Port richtig
im RE2-/MIXED-Flavor (Default RE2, enemy_ai_re2_zombie.c:102) bzw. per Import-Bruecke
im RE1.5-Flavor (Nutzer-Mandat 2026-08-20, abtrennen-Dossier Blocker 1).

## 5. PORT-IST und PLAN

### 5.1 IST — warum der Nutzer keinen Kopf-Abriss sieht

Kette heute (RE2-Flavor, w8, nah, Zielen HOCH):
1. `re15_player_weapon_fire` trifft (UP-Fenster-Fix), Crit `hp=-1`, `state=3`
   (re15_damage.c:1641-1644).
2. `re15_re2_stamp_hit` stempelt `+0x1D2 = 2` (KOPF) aus der Elevation
   (re15_damage.c:1856-1858) — **Divergenz**: das Original stempelt 1 (§2.4).
3. `re2z_death` ZWEIG 4: `death_tbl[7][2] = NULL` → heutige Spalten-Klemme → `col=1` →
   `RE2ZD_66FC` → `re2z_hit_ragdoll(e, pl, 1)` (enemy_ai_re2_zombie.c:6935-6948) —
   zufaellig die ORIGINAL-Zelle.
4. **In `re2z_hit_ragdoll` Phase 0 fehlt der Zielen-HOCH-Zweig** (:5619-5623, als OPEN
   dokumentiert: „`0x800CFBF8 < 0` … Global ohne Port-Produzent"). Der 92C4-Eingang
   `re2z_rag231 == 2` (:5615-5617) ist tot. Ergebnis: `(rand&3)!=0 && death` → 75%
   `re2z_death_burst` (Brocken, kein Kopf), sonst Ragdoll-Sturz mit `hp=10`-
   Kriecher-Wiederbelebung in P2. **Nie eine Kopf-Explosion.**
5. `re2z_death_magnum` selbst ist vollstaendig portiert (:5124-5250: Part-8-Flug 0x4A,
   Drift-Felder, Tinte 0x00101040, Stumpf-Marker Part 0, 50% kopfloses Weiterlaufen)
   und der Renderer konsumiert die Part-Stempel (platform/pc/main.c:8166-8290). Der
   Magnum-Tod braucht KEIN Reserve-Mesh 15/16 (kein Stumpf-Mesh-Swap, nur der
   0x80-Marker, Konsument OPEN) — **er ist auch im RE1.5-Modell-Hybrid sichtbar**
   (Kopf = Mesh 8 < 15, die Klammer main.c:8283 greift nicht). Die Blut-FX sind bis
   abtrennen-Schritt 3 generisch (packed_ids 8000 / 0x08001B58 / 0x00021F40 werden in
   `re2z_gore_fx` verworfen, :3916-3921).

### 5.2 PLAN (exakte Aenderungen, in Reihenfolge)

**Schritt 1 — der Kopf-Zweig (der eigentliche Fix).**
enemy_ai_re2_zombie.c, `re2z_hit_ragdoll`, `case 0:` — VOR dem `(re2z_rand()&3)`-Wurf
(:5624), damit die RNG-Reihenfolge dem Original entspricht (bei Zielen-HOCH zieht das
Original in 66FC-P0 KEINEN Wurf; 92C4 zieht seine eigenen @0x80109330/@0x801094D8):

```c
/* @0x8010679C-A8: `lw v0,0x800CFBF8 / bgez` — 0x800CFBF8 = word0 der SPIELER-Entity
 * (FUN_80049e48.c:133), Bit 31 = Aim-Band HOCH (FUN_80047C6C @0x80047D18,
 * DAT_800A6F8C: UP(4)->Gruppe 2; Kodierung wie RE1.5 FUN_80012aa4). Zielt der
 * Spieler HOCH, waehlt der Ragdoll-Tod die KOPF-EXPLOSION — deterministisch. */
{
    extern int re15_player_aim_elevation(void);   /* player_common.c:206, +1 = HOCH */
    if (re15_player_aim_elevation() > 0) {
        e->re2z_rag231 = 2u;                      /* sb v0(=2),561 @0x801067B0 */
        re2z_death_magnum(e, pl);                 /* jal 0x801092C4 @0x801067BC */
        return;                                   /* j 0x80106F14 */
    }
}
```
KEIN `death`-Gate: das Original prueft +0x4 hier nicht (§3, HURT-Fall erhoehter Gegner).
Die zwei uebrigen Seiteneingaenge (`+0x10C != 0`, `0x800CFBD8 & 0x10000000`) bleiben
OPEN wie dokumentiert.

**Schritt 2 — den +0x1D2-Stempel byte-true machen.**
re15_damage.c:1856-1858 (`re15_re2_stamp_hit`), NUR fuer die Zombie-Familie
(`re15_re2z_owns_type`): Zone aus Teile-Maske statt roher Elevation —
stehend: `elev>0 → 1, elev==0 → 1, elev<0 → 0`; Kriecher/Liegend
(`re2z_f10e&1 || re2z_flags21a&2`): `0`. Belege §2.3/§2.4 als @0x-Kommentar
(INIT 0x0C000000 @0x80100984-998; Maskentabelle 0x800A6DB4; Records 0x800A6724/40/5C).
Nicht-Zombie-RE2-Typen behalten die bisherige Bruecke (deren Masken sind nicht RE'd).
Damit ist Spalte 2 fuer Zombies wieder original-unerreichbar — wie im Auslieferungsstand.

**Schritt 3 — die Spalten-Klemme bleibt, wird aber Sicherung statt Mechanismus.**
Die vier Klemmen (:6203-6207, :6373-6377, :6598-6603, :6936-6941) NICHT entfernen
(Port-Sicherung gegen kuenftige fremde +0x1D2-Schreiber), aber die Kommentare
umschreiben: nach Schritt 2 ist die NULL-Spalte fuer Zombie-Hitscan unerreichbar;
die Klemme ersetzt KEINEN Zonen-Zweig — die DEATH-Wurzel HAT keinen (§1).

**Schritt 4 — Wache/Pins (Fixture-Verschiebung einpreisen).**
- Neuer/erweiterter Probe-Fall (Muster probe_re2z_bandlock/probe_schrot_up_sweep,
  kopfschuss-nah §6): RE2-Flavor, w8, nah, `elev=+1` → nach dem Kill:
  `e->re2z_hits1d2 == 1` (NICHT mehr 2!), `re2z_rag231 == 2`,
  `re2z_part_flags[8] & 0x4A`, `re2z_part_tint[8] == 0x00101040`, Ende CORPSE (state 7).
  `elev=0` → rand-Pfad (Burst ODER Sturz), `re2z_part_flags[8]` unveraendert.
  `elev=-1` → `hits1d2 == 0`, Zelle 7438 (Knockdown-Tod).
- ⛔ kopfschuss-nah §4 Punkt 4 erwartete `re2z_hits1d2==2` — diese Erwartung ist mit
  diesem Dossier ueberholt (Fixture neu verankern, Memory reai-v2-pin-fixture-verschiebung).
- Negativ-Pin: RE1.5-Flavor unveraendert (kein Kopf-Abriss ohne Import-Bruecke; PIN-8-
  Umbau gehoert zu abtrennen-Schritt 2, nicht hierher).

**Schritt 5 — Sichtbarkeit (Querverweis, kein Blocker fuer den Kopf-Abriss).**
Der Kopf FLIEGT nach Schritt 1 auch im Hybrid (§5.1 Punkt 5). Damit er wie RE2 AUSSIEHT,
braucht es abtrennen-Schritt 3 (packed_id-FX: Fontaene 8000, Brocken 0x08001B58,
Spray 0x00021F40 @0x801093C0-0x80109438) und fuer RE1.5-Flavor abtrennen-Schritt 2
(Todes-Gore durch die Import-Bruecke, :6149).

### 5.3 Korrekturen an den Vorbefunden (dieses Dossier ueberschreibt)

- kopfschuss-nah §3.3/§3.4 + §1.2: „UP-nah => Teil 2 = KOPF" und „+0x1D2 = 2 erreicht
  nur die DEATH-Wurzel (Kopf-Tod/Enthauptung)" sind FALSCH — die Herleitung unterschlug
  die Teile-Maske `word0>>26&7` (Zombie = 3, kein Kopf-Bit, §2.3). Richtig: UP-nah
  stempelt RUMPF (1); der Kopf-Tod haengt am Zielen-HOCH-Bit in 66FC-P0.
- kopfschuss-nah §4 Punkt 3 („Stempel bleibt, UP->2 byte-hergeleitet") ist zurueckzunehmen
  (ersetzt durch Schritt 2 oben).
- abtrennen §2.1 („Kopf-Gore laeuft bei Schusswaffen ausschliesslich ueber die
  Rumpf-Spalte der DEATH-Tabelle") ist BESTAETIGT und um den Mechanismus ergaenzt:
  Zeilen 5/6 → 92C4 direkt, Zeile 8/17 → 8BEC direkt, Zeile 7 → 66FC + Zielen-HOCH → 92C4.
- abtrennen §1.2 Blocker-Liste: der 66FC-Seiteneingang `0x800CFBF8 < 0` (dort OPEN
  :5616-5618) ist hiermit aufgeloest: Spieler-word0 Bit 31 = Zielen-HOCH.

## 6. Unklar / Restposten (kein Blocker)

1. Produzent der Spieler-word0-Band-Bits in RE2 (PL00-Overlay, nicht decompiliert) —
   Konsumenten-Beweis (FUN_80047C6C + FUN_80042368-Kodierung) traegt den Fix; das
   Port-Aequivalent `s_aim_elev` wird beim Aim-Ende auf 0 (LEVEL) gesetzt
   (player_common.c:277) — im Kill-Frame identisch, ein 1-Frame-Release-Fenster ist
   theoretisch, nicht messbar.
2. `+0x10C != 0`- und `0x800CFBD8 & 0x10000000`-Seiteneingaenge zum Burst: weiterhin OPEN
   (kein Produzent gefunden; dokumentiert :5619-5623).
3. Stumpf-Marker-Konsument (+0x98/+0x9C der 0x80-Parts): weiterhin OPEN (abtrennen §Unklar 3).
4. AoE-Zonen-Pfad (Granaten, Zeilen 9-11): nutzt die echte Schuss-Y-Regel + word0-Bit28
   (@0x800472D8-30C) — dort IST Spalte 2 erreichbar; der Port hat keinen AoE-Werfer,
   bleibt wie gehabt.
