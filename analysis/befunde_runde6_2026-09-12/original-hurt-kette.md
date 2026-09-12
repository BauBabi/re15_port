# BEFUND — original-hurt-kette: Die KOMPLETTE RE2-Kette des NICHT-toedlichen Schrot-Treffers (RUMPF/TIEF) am stehenden Zombie

Nutzer 2026-09-12: „Beim Schuss mit Schrotflinte Richtung Bauch der Zombies taumeln sie
manchmal merkwuerdig, gehen dann mit Animation unterhalb des Bodens taumeln weiter und
stehen dann wieder." — nicht-toedlich, RE2-Flavor; W8 (M870) -> RE2-Zeile 7.

Statische Analyse 2026-09-12. **Alle zentralen Y-Instruktionen in dieser Runde FRISCH aus
den Binaries verifiziert** (nicht nur Port-Kommentare gegengelesen):
`info/re2leon/COMMON/BIN/EMZ0.BIN` (Overlay @0x80100000) via
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`, `info/re2leon/PSX.EXE`
(t_addr 0x80010000), `RE2_Quellcode_V2/FUN_80015e7c.c` / `FUN_800152c8.c`,
`info/re2leon/PL0/PLD/CDEMD0.EMS` (EM010-Keyframes selbst geparst).
Schwester-Dossier (Port-Seite, parallel): `port-y-audit.md` — dessen Kernbefund wird hier
von der Binaer-Seite UNABHAENGIG bestaetigt.

---

## 1. Frage 1 — Zeile, Spalte, HURT-Dispatch

### 1.1 Die Erzeuger (EXE-Applier, laufen VOR der HURT-Wurzel)

* Zeile `+0x5` = **Item-Id der Waffe**: `lw t0,local_70(sp)` / `srl v1,t0,0x10` /
  `addiu v1,v1,1` / `sb v1,0x5(t0)` @0x80041AA0-B4 (FUN_800410CC; M870 = Id 7,
  Beleg-Kette torso-schrot.md §1). HURT/DEATH-Wort: `sw 2,4` @0x80047288 bzw. `sw 3,4`
  @0x8004728C-90 — der Stempel nullt zugleich `+0x6`.
* Spalte `+0x1D2` = **zone + 3*bracket** @0x80041A88-9C (AoE-Zwilling @0x80047310-30).
  Zonen-Rechnung @0x80047294-0x80047334: Vorgabe RUMPF (1); Ziel-Y unter halber
  Koerperhoehe -> BEINE (0); Kopf (2) nur mit word0-Bit 0x10000000 — **der Zombie traegt
  es NIE** (INIT `word0 |= 0x0C000000` @0x80100984-998, kopf-wegschiessen.md).
  Bauch/EBEN => Spalte 1, TIEF/liegend => Spalte 0 (Port-Stempel re15_damage.c:1866-70).

### 1.2 Die HURT-Wurzel FUN_80104F40 — Stationen IN DIESER REIHENFOLGE

1. Grab-Abbruch @0x80104F68-FDC.
2. **KRIECHER-Weiche** `+0x10E & 1` @0x80104FE0-500C -> 1D-Tabelle
   `0x8010CBE8[+0x5]` ([0]=NULL, [1..18] alle = FUN_80107888 = Kriecher-HURT).
3. `+0x21A & 0x10` -> Kriecher-Umbau FUN_80107A78 @0x80105014-38.
4. **FLINCH-Tor** @0x8010503C-AC: nur Phase 0 (`lbu v0,6` @0x8010505C), Schwelle
   `+0x10E&0x40 ? (+0x5!=1?23:0) : 0`, `res223 <= thr` und (`+0x222==1 || +0x5==1`)
   -> `sw 0x501,4` @0x801050A4-AC = EXEC[5]-Sturz, **return** (Dispatch laeuft NICHT).
5. Liege-Route `+0x21A & 2` -> `sw 0x60501` @0x8010517C-84 (EXEC[5] Phase 6).
6. Zerleger @0x80105288-3D8 (faellt durch).
7. **2D-DISPATCH — frisch disassembliert:**
   ```
   801053e0: lbu v1,5(a0)          ; Zeile = +0x5
   801053e4: lui a2,0x8011
   801053e8: addiu a2,a2,-14016    ; Basis = 0x8010C940  (die HURT-Tabelle)
   801053ec-f4: sll 3/addu/sll 2   ; Zeile * 36
   801053f8: lbu v1,466(a0)        ; Spalte = +0x1D2, * 4
   80105408: lw v0,0(v1)
   80105410: jalr v0               ; KEIN NULL-Waechter
   ```

### 1.3 Die HURT-Tabelle @0x8010C940 — eigener Dump (19 Zeilen x 9 Spalten)

`re2_disasm.py read 0x8010c940 9 --w 4 --rows 19 --rowstride 36 --bin EMZ0.BIN`.
Zeile 0 = Datenwoerter (nie dispatcht). Legende der 6 vorkommenden Zeiger:

| Zelle | Funktion | Port |
|---|---|---|
| 0x80105438 | Haupt-Treffer (MAIN) | re2z_hit_main |
| 0x80105BC0 | STAGGER (Rueckwaerts-Wanken) | re2z_hit_stagger |
| 0x801066FC | RAGDOLL-Wurf (HURT: `+0x4==2`) | re2z_hit_ragdoll |
| 0x8010703C | RUTSCH-Rueckstoss (nur Zeile 12 Bowgun, Spalten 1/4/7) | re2z_hit_slide |
| 0x80107438 | KNOCKDOWN mit Arm-Abriss | re2z_hit_knockdown |
| 0x80107EF0 | leichter Taumel (Zeilen 15/18 SMG/Gatling) | re2z_hit_light |

**Die Port-Liste „4 = 0x8010703C, 5 = 0x80107438, 6 = 0x80107EF0"
(enemy_ai_re2_zombie.c:3700) ist schlicht das Handler-Id-Enum von `re2z_hit_tbl` —
Zelle fuer Zelle identisch mit diesem Dump verifiziert.**

Zeile 7 (Schrotflinte M870) @0x8010CA3C:
`[7438, 66FC, 0 | 7438, 5BC0, 0 | 7438, 5438, 0]`
=> Bracket 0 (Port immer): **RUMPF (Spalte 1) -> 0x801066FC Ragdoll-HURT;
TIEF (Spalte 0) -> 0x80107438 Knockdown-HURT**; HOCH (Spalte 2) = NULL (Port klemmt -> 1).
Zeile 8 (SPAS) identisch bis auf col7 = 5BC0. `re2z_hit_stagger` liegt bei **0x80105BC0**
— „@0x80106098" aus dem Auftrag ist KEIN Handler-Einstieg, sondern der 0xB01-Commit in
Stagger-P2 (`sw 2817` @0x80106098/@0x801060F8).

---

## 2. Frage 2 — Y-Behandlung je Executor

### 2.1 Das Wurzel-Bewegungs-Paar (EXE): Y wird VERWORFEN

* **FUN_80015E7C** (fuellt): Delta der Keyframe-Felder +6/+8/+10 zwischen prev/now
  in `+0x144/146/148` (`sh v1,324(t0)` @0x80015FD8-E4); prev-Cache +0x20C-210.
* **FUN_800152C8** (wendet an): RotMatrix(yaw) + ApplyMatrixSV auf +0x144 — dann
  **NUR `+0x38 += out.x` und `+0x40 += out.z`** (@0x80015314-34; Decompile
  RE2_Quellcode_V2/FUN_800152c8.c: kein +0x3C-Store). **Die Y-Komponente des Root-Deltas
  erreicht das Aktor-Y NIE.**
* Doppelt abgesichert von den DATEN: EM010 Paar-2 (Action-Bank) traegt **sy == 0 in
  ALLEN 31 Clips** (eigener CDEMD0.EMS-Dump, §2.4).

### 2.2 Der Vollscan: WER schreibt +0x3C ueberhaupt?

Eigener Scan aller `sw rt,60(rs)` in EMZ0.BIN (17 Treffer, 10 davon `(sp)` =
Stack-Saves): **Nur DREI Entity-Y-Stores im gesamten Reaktions-/Bewegungs-Code, alle im
RAGDOLL 0x801066FC** — @0x80106AC8 (P2-Restore), @0x80106E88 (Gravitation),
@0x80106F10 (Boden-Klemme) — plus vier im CORPSE-Zustand 7 (@0x8010A8A0/B4/D0/E0,
Leichen-Senken/Heben in FUN_8010A440, y±2/−5 mit Klemme +0x10C+300).
**Knockdown, Stagger, Slide, Light, EXEC[5], EXEC[9], Kriecher-HURT schreiben Y NIE.**

### 2.3 Die Executoren im Einzelnen

* **0x80107438 KNOCKDOWN (TIEF, Spalte 0):** P0 Clip-Wort `0x000F0001` (Clip 1, Rate 15)
  @0x80107490-C8, **Zufallsstartframe `+0x14D = rand&7`** @0x801074CC-EC (Clip-1-rootY der
  Frames 0..7 liegt konstant bei ~−1976..−1979 — jeder Start ist aufrecht), zwei bare
  FUN_80015E7C @0x801074C4/E8 (fuellen nur). P1 = e7c + Advance(256) + 152C8
  @0x80107740-6C — **X/Z-only**. KEINE Y-Zeile, KEINE Klemme — nicht noetig, weil der
  gesamte Fall im CLIP-rootY liegt (−1976 -> −222, §2.4); das Aktor-Y bleibt die ganze
  Zeit auf Bodenhoehe. P2 @0x8010777C: Kriecher-Exit (§3).
* **0x801066FC RAGDOLL (RUMPF, Spalte 1) — der EINZIGE Y-Executor:**
  - P0: **`+0x232 = +0x1C2`** (`lhu v0,450(s2)` / `sh v0,562(s2)` @0x80106994-A0) — das
    ist das BODEN-Y aus dem EXE-Boden-Query (Produzent-Paar `sw v0,60(s0)` /
    `sh v0,450(s0)` @0x8003EE04/EE18), NICHT das aktuelle +0x3C. Schub −250 (Ruecken
    +100) via 152C8 @0x8010686C-9C; Clips 4/3 Rate 3; `+0x1D3 |= 0x80` @0x801069A4-B0
    (unbeschiessbar bis P2).
  - Frame 20 (P1): `+0x15A = 10`, `+0x16A = 2` (Bounce-Budget) @0x80106D50-5C.
  - Frames >= 20: **Gravitation** `lw v0,60/addu/sw v0,60` @0x80106E7C-88
    (y += +0x15A), `+0x15A += 5`, ab Frame 35 `+= 55` @0x80106E90-A4.
  - **BODEN-KLEMME** @0x80106EA8-C0: `lh v0,562` (gy) / `lw v1,48(s3)` (**gerenderte
    Part-0-Translation Y** = posiertes Wurzel-Y) / `addiu −200` / `subu −rooty` /
    `slt < y`; feuert -> @0x80106EE4-F10: `+0x15A = −(+0x15A>>3)`, `+0x16A −= 1`,
    **`y = gy − 300 − rooty`** (`sw v1,60(a1)` @0x80106F10), SE 4. Die Klemme pinnt also
    die GERENDERTE Wurzel auf 300 Einheiten ueber dem Boden; tiefster Moment pro Tick =
    Wurzel auf gy−200.
  - P2 @0x80106A8C: **`y = +0x232`** (`lh v1,562` / `sw v1,60` @0x80106AC0-C8) — die
    unbedingte Gegenbuchung — dann Kriecher-Exit (§3).
  - Wichtig fuers Bild: die Clips 3/4 halten rootY bei −1529..−1917 (§2.4) — der Sturz
    zu Boden kommt hier NICHT aus dem Clip, sondern aus der Aktor-Y-Physik. Das Aktor-Y
    taucht dabei GEWOLLT ~1300..1700 Einheiten unter die Bodenebene (Schwelle
    `gy−200−rooty` mit rooty≈−1550..−1850) — sichtbar bleibt der Koerper trotzdem ueber
    dem Boden, weil der Renderer `y + rootY(Pose)` zeichnet. **Jeder Port-Konsument, der
    e->y als „Koerperhoehe" liest, oder jede Klemmen-Messung, die vom gerenderten rootY
    abweicht, wird hier sichtbar.**
* **0x80105BC0 STAGGER:** Clips 4/3 Rate 3, Schub −450/0 via 152C8 @0x80105C64-88 — kein Y.
* **0x80107EF0 LIGHT:** Clips 0x1C-0x1E (rootY −1950..−2038), Rueckstoss via 152C8 — kein Y.
* **0x8010703C SLIDE:** P2 `+0x15A = Wurzel-Bone-Y` (`lw 48(s2)` @0x80107270), P3/P5
  Bone-BLEND (FUN_80028F48) auf die Part-0-MATRIX — Praesentation, kein +0x3C-Store.
* **EXEC[9] @0x80103E48 (Stoss-Taumel):** Schub `+0x144=400` @0x80103F60-64, 152C8
  @0x80103FBC (X/Z), Abbau −30 @0x80103FCC; am Auslauf (+0x15A==1 @0x80103FF0-F8)
  **7/8-Sturz** `s=(r1>>(r2&3))&7; s!=0 -> 0x501` @0x80104000-28. Kein Y.
* **EXEC[5] @0x80103188 (Sturzkette):** Fall-Clips 1/2 (Startframes 10/15), Liege 0x17/0x16,
  Boden 8/9, P8 -> 0x101. Advance ueberall a3=256. **Kein einziger Y-Store** — Fall UND
  Aufstehen liegen komplett im Clip-rootY.
* **Kriecher-HURT 0x80107888:** Clip 6 (Start rand&7, Rate 3), P2 `+0x10E = 1` (nackter
  Halbwort-Store) + `+0x4 = 1` @0x80107A54-58 -> zurueck in die Kriech-Lokomotion. Kein Y.

### 2.4 Clip-rootY aus CDEMD0.EMS EM010 (eigener Dump, Paar 2 = Action-Bank)

EMD @EMS+0x2A800 (TOC kind 0x10 rec 3), EDD @+0x506C (31 Clips/1263 Frames), EMR-Pool
@+0x64A8 (kf_size 0x50). PSX-Y: negativ = ueber dem Aktor-Ursprung. Auszug (min/max):

| Clip | Rolle | frames | rootY min..max | Verlauf |
|---|---|---|---|---|
| 1 | Fall rueckw. (7438-P0, EXEC[5]) | 60 | −1979..−183 | −1976 -> −222 (Stand->Boden IM CLIP) |
| 2 | Fall vorw. | 60 | −1985..−99 | −1976 -> −227 |
| 3 | Ragdoll/Stagger (Ruecken) | 40 | −1847..−1529 | bleibt HOCH (Taumeln; endet −1847) |
| 4 | Ragdoll/Stagger (Front) | 44 | −1917..−1593 | bleibt HOCH |
| 5 | Kriech-Lokomotion / Kriecher-Exit | 50 | −175 konstant | am Boden |
| 6 | Kriecher-HURT | 20 | −461..−175 | am Boden |
| 8 | Boden-AUFSTEHER L (EXEC[5]-P6/7, EXEC[7]-P2) | 80 | −1957..−152 | −222 -> **−1957 = Aufstehen IM CLIP** |
| 9 | Boden-AUFSTEHER R | 80 | −1974..−227 | −227 -> −1974 |
| 0x16/0x17 | Liege-Halten | 10 | ~−221..−229 | konstant am Boden |
| 0x1C/0x1D/0x1E | Light-Zuckungen | 3 | −1952..−2038 | aufrecht |

**Kein einziger Clip traegt positives rootY** (nichts unter der Bodenebene), und
**sy == 0 in allen 31 Clips** — Root-Delta-Y existiert in den Daten nicht.

---

## 3. Frage 3 — Der Weg danach: KRIECHER, kein Wiederaufstehen

* **Knockdown-P2 @0x8010777C** (HURT): `+0x10E = 0x2001` @0x80107820-24
  (Bit 0 = Kriecher-Wurzel), Zustandswort `rand&1 ? 1 : 0x201` @0x80107834-4C,
  `+0x1D3 &= 0x7F` @0x80107850-5C. (`+0x4==3`-Zweig/HP=10 ist im HURT strukturell tot.)
* **Ragdoll-P2 @0x80106A8C** (HURT): identischer Exit — `+0x10E=0x2001` @0x80106B0C-10,
  Clip 5 (`0xF0005` @0x80106B40), Wort 1 @0x80106B3C, plus **y-Restore** @0x80106AC8.
* Danach dispatcht die Zustand-1-Wurzel ueber `+0x10E & 0x3F` in 0x8010C854 — ungerade
  -> **Kriecher-Root 0x80101210** (DECIDE @0x8010C90C / EXEC @0x8010C918): Lokomotion
  Clip 5 (@0x80103064-6C), Warten Clip 0x17 (@0x80103B5C-60), Griff.
  **Der Kriecher steht im Original NIE wieder auf**: sein HURT kehrt in die Lokomotion
  zurueck (@0x80107A54-58), einen Aufsteh-Ausgang gibt es in keiner der drei
  Kriecher-Zellen. Die Auftrags-These „steht ueber EXEC[7] wieder auf" ist **falsch**:
  EXEC[7] @0x80103780 ist der LIEGE-SPAWN (P2 Boden-Aufsteher Clip 8/9 @0x80103840-80,
  P4 `0x101` @0x80103900-0C) — er haengt am Limpet-Latch `+0x10E&0x4000`, nicht am
  Kriecher-Bit. Aufrecht-Aufstehen nach einem Sturz gibt es nur in **EXEC[5]** selbst
  (P6/P7 Clips 8/9 -> P8 `0x101` @0x801036F4-F8) — also nur fuer den FLINCH-/Stoss-Sturz
  (0x501/0x60501), der das Kriecher-Bit NICHT setzt (nur +0x10E|=0x2000 @0x80103308).
* **Y beim Aufstehen: unberuehrt.** Clips 8/9 heben die Wurzel im CLIP von −222 auf
  −1957; das Aktor-Y steht dabei (und die ganze Liegezeit) auf Bodenhoehe.

---

## 4. FAZIT

**Was das Original zeigt (Schrot, Bauch, Ueberlebender):** Wurf-Taumeln (Clips 3/4),
der Koerper wird ab Frame 20 von der AKTOR-Y-Gravitation abgesenkt, die Boden-Klemme
pinnt die GERENDERTE Wurzel auf 200..300 ueber dem Boden (bis 3 Bounces, SE 4), P2
bucht y hart auf das Boden-Y zurueck und der Zombie wird KRIECHER. Bei TIEF (Knockdown)
bewegt sich das Aktor-Y ueberhaupt nie. **Unter den Boden kommt der Zombie nie — durch
VIER Riegel:** (a) 152C8 verwirft Root-Y (@0x80015314-34) und die Daten tragen sy==0;
(b) kein Clip hat positives rootY; (c) die Ragdoll-Klemme misst die GERENDERTE Wurzel
(`lw 48(s3)` @0x80106E64/EAC) gegen das Boden-Y; (d) das Boden-Y kommt aus +0x1C2
(Boden-Query @0x8003EE18), nicht aus dem aktuellen +0x3C — und Re-Hits waehrend der
Reaktion blockt +0x1D3=0x80 (@0x80047138-40). **Und: er steht danach NICHT wieder auf
(Kriecher)** — „steht dann wieder" ist selbst schon ein Divergenz-Signal.

**Kandidaten (Prioritaet, Port):**
1. **`re2z_root_py` ohne Hybrid-Rootfix/Tween** (enemy_ai_re2_zombie.c:5742-5759): Klemme
   rechnet mit rohem kf_py, der Renderer zeichnet `kf_py + K` (skeleton_common.c:256,
   K≈+108..185) -> Wurzel klemmt K zu tief, das taumelnde Modell (Clips 3/4!) taucht in
   den Boden. == Kernbefund von port-y-audit.md, hier binaerseitig bestaetigt.
   Patch: `py + re15_skel_root_y_fix(sk,kf)` zurueckgeben (+0x8000-Skip wie
   enemy_ai_common.c:633-635).
2. **gy232-Stand-in** `(int16_t)e->y` (:5853) statt +0x1C2-Zwilling: jede P0-Re-Anker
   waehrend verschobenem y verewigt die Tieflage (Original doppelt geschuetzt: 1D3-Filter
   + Boden-Query). Haerten: Boden-Y-Produzent spiegeln.
3. **Exits ohne Gegenbuchung**: Flinch 0x501 (:6507) / Grab-Abbruch verlassen Zustand 2
   ohne den P2-Restore — nur mit 1D3-Leck erreichbar; erst MESSEN (Trace-Zeile aus
   port-y-audit.md §4.3), nicht raten.
4. **„Steht wieder auf"**: (a) Port-only Kriecher-s1>2-Rueckfall in die AUFRECHT-Tabelle
   (:3284, OPEN; erreichbar via Kriech-Griff-Abwurf `sw 0x501` @0x80102D24) — der
   Kriecher steht dann ueber EXEC[5]-P6/7 auf, was das Original nie tut; (b) der
   byte-true EXEC[5]-Aufsteher nach Flinch — dann war es kein Ragdoll-Ende.
   Beobachtung des Nutzers gegen beide Pfade messen (s2/s1-Trace).
Reichweiten-Hinweis: die HURT-Zellen der Zeile 7 sind Ueberlebenden-only — mit RE2-Schaden
toetet Schrot nah fast immer; der Port erreicht sie „manchmal", weil Schaden/HP aus der
RE1.5-/Import-Welt kommen.
