# Birkin G5 (ROOM5090) — "nicht solid" + Vollzensus des RE2-Endkampfs (Runde 17, 2026-09-20)

Nutzer, woertlich: *"Birkin ist nicht solid. Auch sonst ist das Verhalten noch immer nicht wie
in resident Evil 2, also beim Endboss. ich will hier eine 1 zu 1 Umsetzung!"*

Worktree `.claude/worktrees/wf_2d0f494e-255-4` (Branch `worktree-birkin-1zu1`), Build
`re15_port/build_r17`, Suite **322/322**.

Quellen, alles in dieser Runde selbst gelesen/disassembliert:
* RE2-Overlay `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN` (G5, gelinkt @0x80100000) und
  `CDEMD0_EM37_ai1.BIN` (Tentakel) — `re2_disasm.py` mit `RE_OVERLAY_DIR`.
* RE2-EXE `info/re2leon/PSX.EXE` (Datei = Adresse − 0x80010000 + 0x800), Decompilate
  `RE2_Quellcode_V2/FUN_80034d0c.c`, `FUN_80035408.c`.
* Sonde `re15_port/tests/unit/probe_r17_birkin_1zu1.c` (Registrierung
  `tests/unit/probes/r17_birkin-1zu1.cmake`), Protokoll
  `analysis/befunde_2026-09-20/birkin-1zu1/pin_r17.log`.
* Echte exe (`build_r17/platform/pc/re15_pc.exe`), Bilder in
  `analysis/befunde_2026-09-20/birkin-1zu1/`, Mess-Schiene `sicht_lauf_birkin_dbg.log`.
* Vorbefund: `analysis/befunde_2026-09-19/birkin-g5.md` (Runde 16, §1–§7).

---

## 0. Kurzfassung

1. **"Nicht solid" ist gefunden, belegt und gefixt.** RE2 gibt G5 **keinen `+0x78`-Kasten**,
   sondern **zwei Kollisionssegmente** (`+0x1E8 = 2` @0x8010052c-30): Segment 0 lokal X −2000 mit
   **Radius 6000** (@0x80100534-40), Segment 1 lokal X 2200 mit **Radius 1300** (@0x80100550-5c),
   beide lokal Y −1500 und Halbhoehe 1500. Geschoben wird der Spieler aus seinem eigenen Tick
   (`FUN_800355C4` @0x80026628 → `FUN_80034D0C` @0x80035630). Der Port hatte davon nichts: er
   benutzte den RE1.5-Zylinder r 1000 aus `re15_enemy_apply_hitbox`. **Gemessen: Standabstand
   1451 → 4451**, waehrend die sichtbare Masse bis Ursprung + 4494 reicht — der Spieler lief also
   drei Meter weit in den Koerper hinein und steht jetzt davor.
2. **Der Zensus ist vollstaendig** (§3): Routine-Tabelle @0x801055CC (8 Eintraege), die beiden
   Sub-Tabellen @0x8010564C/@0x80105660 (je 5), die Blob-Tabelle @0x80105ABC (6) und die
   Todes-/Opfer-Tabelle @0x801056E8 — je Eintrag Adresse, Funktion, Port-Status. Ergebnis:
   **von 25 lebenden Eintraegen hat der Port 22**; zwei sind beweisbar tot (`Routine 5/6/7`,
   `Blob-Eintrag 2`), und **drei echte Luecken** bleiben (§5).
3. **Zwei Zensus-Befunde, die im Vorbefund fehlten:**
   * **Blob-Eintrag 2 (@0x80104440) ist DATENTOT** — kein Schreiber setzt `+0x218 = 2`
     (Vollzensus aller 10 Schreiber in EM036 §3.4; EM037 schreibt nur sein eigenes `+0x218`).
     Die fehlende `case 2` im Port ist damit keine Luecke.
   * **Routine 2 ist der EXE-Trefferpfad**, nicht ein Overlay-Zustand: `@0x80047280-90` setzt
     `+0x04 = 2` bei `HP >= 0` und `= 3` bei `HP < 0`, nachdem es den alten Routine-Wert nach
     `+0x1FC` gerettet hat. Routine 2 ruft die normale Routine (`jal 0x80100784` @0x80102a08 /
     @0x80102a70) selbst wieder auf — der Boss agiert im Treffer weiter.
4. **Ehrlich offen** (§5): die RE2-PL0-Opferanimation des Devour (`0x80103908`), der
   Blut-Spawn der Trefferroutine (@0x80102888 mit Korridor-Klemme Z −26000..−20500), und
   Bilder von Treffer-Reaktion und Boss-Tod aus dem laufenden Spiel — der gescriptete Spieler
   wird gefressen, bevor er 600 HP herunterschiessen kann (§4.2).

---

## 1. Reproduktion / Messung

### 1.1 Vorher (der gemeldete Defekt)

Sonde `probe_r17_birkin_1zu1` Abschnitt A: nach dem Intro steht der Boss auf (1960, −23400).
Der Spieler wird von X = Boss+8000 in 100er-Schritten herangesetzt, je Schritt laeuft der Push.
Der kleinste Abstand, den er behaelt:

| Modell | kleinster Abstand Spieler↔Boss-Ursprung |
|---|---|
| ALT: ein Zylinder r 1000 (`re15_enemy_apply_hitbox` case 0x36, RE1.5-Box @0x8011ee64) | **1451** |
| NEU: die zwei RE2-Segmente | **4451** |

Die Massenfront liegt bei Ursprung + 4494 (Runde 16, §1.2). Bei 1451 steht der Spieler also
**3043 Einheiten INNERHALB** der sichtbaren Masse — genau der Nutzer-Befund.

### 1.2 Nachher, im echten Spiel

Lauf mit dauerhaft gehaltenem "vorwaerts" GEGEN die Masse
(`RE15_INPUT_SCRIPT="W2,U40"`, Protokoll `sicht_lauf_birkin_dbg.log`):

```
kleinster Abstand ueber den ganzen Lauf, je Zustand:
   sub 0 (Idle/Entscheiden) : 5094
   sub 1 (Tentakel-Zug)     : 5016
   sub 2 (Intro)            : 4222
```
Der Spieler kommt mit voller Vorwaertstaste nie naeher als die Segmente erlauben. (Der
Intro-Wert 4222 liegt unter dem 4450er-Standabstand, weil dort die Root-Motion den Boss
BEWEGT und der Push erst im Folgebild greift; im Kampf tritt das nicht auf.)

---

## 2. Original-Mechanismus (@0x)

### 2.1 Die zwei Segmente (Ctor 0x801003CC)

```
8010052c: addiu v0,zero,2     / 80100530: sw v0,488(s0)   ; +0x1E8 = ANZAHL der Segmente = 2
80100534: addiu v0,zero,-2000 / 80100538: sh v0,148(s0)   ; Seg0 +0x10 = lokal X
8010053c: addiu v0,zero,6000  / 80100540: sh v0,154(s0)   ; Seg0 +0x16 = RADIUS 6000
                                80100544: sh v0,156(s0)   ; Seg0 +0x18 = zweiter Radius (gleich)
80100550: addiu v0,zero,2200  / 80100554: sh v0,180(s0)   ; Seg1 +0x10 = lokal X
80100558: addiu v0,zero,1300  / 8010055c: sh v0,186(s0)   ; Seg1 +0x16 = RADIUS 1300
80100470: addiu v1,zero,-1500 / 80100520: sh v1,152(s0)   ; Seg0 +0x14 = lokal Y
                                80100524: sh v1,184(s0)   ; Seg1 +0x14 = lokal Y
80100448: addiu t0,zero,1500  / 80100580: sh t0,158(s0)   ; Seg0 +0x1A = HALBHOEHE
                                80100588: sh t0,190(s0)   ; Seg1 +0x1A = HALBHOEHE
```
Segment-Block ab `+0x84`, Stride 32 (`param_1 + 0x21` bzw. `+ 8` Woerter je Segment,
Decompile `FUN_80034d0c.c` Z.28/Z.33/Z.135).

**Weltlage** rechnet `FUN_80035408(entity, &entity.pos)` (Decompile): je Segment
`RotMatrix(0, entity+0x76, 0)`, `ApplyMatrix` auf `(seg+0x10, seg+0x14, seg+0x12)`, dann
`seg.x/y/z = entity.x/y/z + Ergebnis`. G5s Yaw bleibt im ganzen Kampf 0 (der einzige
Yaw-Schreiber des Overlays ist @0x8010044c) — die Matrix ist die Identitaet auf +X.

### 2.2 Wer schiebt wen

* **G5 schiebt sich selbst aus fremden Koerpern** — jeden Tick am Ende des Hauptlaufs:
  `jal 0x80035408` @0x8010036c (Segmente nachfuehren), `sb v0=255,13(s3)` @0x80100378,
  `lw a0,-492(a0)` = `*(0x800CFE14)` = Listenkopf und `jal 0x80034d0c` @0x80100384.
* **Der SPIELER schiebt sich aus G5** — in seinem eigenen Tick: `jal 0x800355c4` @0x80026628
  (direkt nach der SCA-Klemme `jal 0x8003bfac` @0x80026620). `FUN_800355C4` laeuft die
  Entity-Liste `0x800CFE14 .. *(0x800CE334)` durch, ueberspringt sich selbst und alles ohne
  Wort0-Bit 0x1, ruft `FUN_80034D0C(entity, spieler)` @0x80035630 und stempelt je Treffer ein
  Bit in `Spieler+0x0E` (`sh s3,14(s1)` @0x80035658).
* **Der Spieler hat GENAU EIN Segment**, aus seinem Ctor:
  `addiu v1,zero,450 / sh v1,154(s2)` @0x8003bdc0-c4 (Radius 450),
  `sw v0=1,488(s2)` @0x8003bddc (ein Segment),
  `addiu v0,zero,-1530 / sh v0,152(s2)` @0x8003bde0-e4 (lokal Y),
  `addiu v0,zero,1530 / sh v0,158(s2)` @0x8003bde8-ec (Halbhoehe).

### 2.3 Das Scharfschalten

```
Ctor  80100594: ori v1,v1,0x2   / 80100598: sw v1,0(s0)    ; Bit 0x2 = "nicht kollidieren"
Ctor  80100604: ori v0,v0,0x4   / 8010060c: sw v0,0(s0)    ; Bit 0x4 = "nie GESCHOBEN werden"
[T0]  801011e4: addiu v1,zero,-3  (= ~0x2)
      801011ec: and v0,v0,v1
      801011f0: sw v0,0(s0)                                 ; Bit 0x2 WEG = ab jetzt solide
```
`FUN_80034D0C` steigt aus bei `((*param_1 | *param_2) & 2) != 0` (Decompile Z.24) und bei
`(*param_2 & 4) != 0` (Z.25). Bit 0x4 bleibt: G5 ist ein Schieber, nie ein Geschobener.
Im Port ist `[T0]` das `g->gestartet`-Gate.

### 2.4 Phasenabhaengigkeit

Nur **Segment 1** bewegt sich, und zwar im Massen-Biss:

```
801040b8: lhu v0,180(s2) / 801040c0: addiu v0,v0,500 / 801040c4: sh v0,180(s2)
801040d0: slti v0,v0,3701 / 801040dc: addiu v0,zero,3700 / 801040e0: sh v0,180(s2)   ; Kappe
801042b0: lhu v0,180(s2) / 801042b8: addiu v0,v0,-15 / 801042bc: sh v0,180(s2)
801042c8: slti v0,v0,2200 / 801042d0: addiu v0,zero,2200 / 801042d4: sh v0,180(s2)   ; Boden
```
Der Port fuehrt diesen Wert seit Runde 16 als `g->hitbox_b` (Biss-Trefferpunkt), nur hatte er
keinen Kollisions-Konsumenten. Segment 0 wird nach dem Ctor nie wieder geschrieben
(Vollzensus aller Zugriffe auf 144..190 in EM036).

---

## 3. VOLLZENSUS der Tabellen

### 3.1 Routine-Tabelle @0x801055CC (Entity `+0x04`)

Dispatch: die EXE ruft `table[+0x04]`. `+0x04` schreiben: der Ctor (`sw v0=1,4(s0)`
@0x801003e8), die Routine-1-Wache (`3` bei `+0x226 & 6 == 4`, @0x801007f4-fc), die
Zustandswoerter der Subs (0x101/0x301/0x401 — Byte 0 bleibt 1) und **die EXE-Waffenwirkung**
(`addiu v0,zero,2 / sw v0,4(s1)` @0x80047280-88 bzw. `3` @0x8004728c-90).

| # | Adresse | Was er tut | Port |
|---|---|---|---|
| 0 | 0x801003CC | **INIT**: HP 600 (@0x801003fc, easy 400), Kopf-/Augen-Ctor, **die zwei Segmente** (§2.1), Wort0-Bits 0x2/0x4/0x8/0xC000000, Part-Flag 0x4000 (@0x80100628), Registrierung der Spieler-Haken 0x800CE300/0x800CE400 (@0x801005dc/@0x801005f8) | **ja** (Segmente NEU in dieser Runde) |
| 1 | 0x80100784 | **HAUPT**: Dispatch `+0x10E & 0x3f` → @0x80105648; Wache `+0x226 & 6 == 4` → Routine 3 | ja |
| 2 | 0x801025BC | **TREFFER** (Eintritt aus der EXE, s.o.): Blut-Spawn an der Trefferstelle (`jal 0x800154ac` @0x80102888) mit Korridor-Klemme Z ∈ [−26000, −20500] (@0x80102760-84) und Versatz −1400/−700/−2000, Flinch-Zaehler `+0x222` (Schwelle 15, @0x80102a48/@0x80102a58), **ruft die Hauptroutine selbst wieder auf** (@0x80102a08/@0x80102a70), setzt `+0x04` zurueck auf 1 (@0x801029f0/@0x80102b88) | **teilweise** — Flinch-Akku ja, **Blut-Spawn + Klemme NEIN** (§5.2) |
| 3 | 0x80102BBC | **TOD**: Phasen [T16]..[T20] (Clip 6/7/10, Zuckungen, Absink-Rampe @0x80103668), Blob → 5 (@0x801030e4), Tracking aus (`+0x1C0 \|= 2` @0x801030f8), Rumble-Kaskade (13× `jal 0x8003947C` @0x801034e4ff) | ja (Runde 6/16), **Rumble-Kaskade nicht** (§5.3) |
| 4 | 0x80103834 | Dispatch `+0x05` → @0x801056E8. **Kein Schreiber setzt `+0x04 = 4`** (Vollzensus EXE + beide Overlays) → im Boss-Dispatch tot; dieselbe Tabelle bedient aber der Spieler-Haken (Zeile unten) | n/z (tot) |
| 5 | 0x00000000 | leer | n/z |
| 6 | 0x00000000 | leer | n/z |
| 7 | 0x80103878 | `jr ra` — inert | n/z |

**Spieler-Haken** (nicht Teil der Routine-Tabelle, aber Teil des Kampfes): der Ctor traegt
`0x80103880` in `0x800CE300[0x36]` (@0x801005dc) und `0x801038CC` in `0x800CE400[0x36]`
(@0x801005f8) ein. Beide faechern ueber `Spieler+0x05` auf: @0x801056E8 = {`0x801038BC`
(`jr ra`), `0x80103908`} bzw. @0x801056EC = {`0x80103908`, …}. **0x80103908 ist die
RE2-Opfermaschine des Devour auf dem PL0-Rig** — im Port DEFERRED (§5.1).

### 3.2 `+0x10E`-Tabelle @0x80105648

| # | Adresse | Was | Port |
|---|---|---|---|
| 0 | 0x801007C4 | Dual-Dispatch: erst A-Tabelle @0x8010564C[`+0x05`], dann B-Tabelle @0x80105660[`+0x05`] | ja |
| ≥1 | — | **existieren nicht**: ab 0x8010564C beginnt die A-Tabelle. In EM036 schreibt NICHTS `+0x10E`; der einzige Schreiber ist das Raumskript (`Member_set(0x07, 0)` in ROOM7040 sub06, Runde 16 §2.1). Die Tabelle hat genau einen lebenden Eintrag. | n/z |

### 3.3 Sub-Tabellen A @0x8010564C / B @0x80105660 (Entity `+0x05`)

| `+0x05` | A | B | Was | Port |
|---|---|---|---|---|
| 0 | 0x80100868 | 0x80100960 | **IDLE/ENTSCHEIDEN.** A: Devour-Test (`arc 128` @0x80100894, `X >= 9001` @0x801008a0, `dist < 6000` @0x801008ac → 500 Schaden `jal 0x800401d4` @0x80100904, Zustandswort 0x401, Blob 4). B: die Wuerfel-Kaskade der Tentakel-Muster (Schwellen 7000/8000/9001/10000/10001/11001, `+0x16A`/`+0x16B`, Sender `0x80104E9C`), Zustandswoerter 0x101/0x301 | ja |
| 1 | 0x80100D08 (`jr ra`) | 0x80100D10 | **TENTAKEL-ZUG**: Kommandos 0xB01/0x70B01/0x90B01 (`addiu 2817` @0x80100dd4ff), Zeitmarken 7/10/15/55/61/90, Root-Spur `jal 0x800152c8` @0x80100fe0 mit der **absoluten** Kappe X < 12001 (@0x80100fd0) | ja |
| 2 | 0x80101108 (`jr ra`) | 0x80101110 | **INTRO** [T0]..[T15]: Selbstplatzierung (−9000/−23400 @0x801011d0-dc), **Bit 0x2 loeschen** (@0x801011e4-f0), Phasenbytes an die Arme, Clips 1/3/4/2, Intro-Ende → sub 1 (@0x80101780-84, `+0x1C0 = 0` @0x801017d4) | ja |
| 3 | 0x80101808 (`jr ra`) | 0x80101810 | **RUECKZUG**: Rampen-Timer, `X > 4000`-Linie (`slti 4001` @0x80101934), Positions-Schreiber `sw v0,56(s2)` @0x8010194c, Rueckkehr auf `+0x04 = 1` (@0x8010196c) | ja |
| 4 | 0x80101AA0 (`jr ra`) | 0x80101AA8 | **DEVOUR**: Kommandowort 6 an den Spieler (`sw v0,-1028(at)` @0x80101b50), Opferbank in PL+0x188/0x18C (@0x80101b60/@0x80101b78), `+0x1D3 \|= 0x80` (@0x80101b8c), Freeze-Bit 0x1000 (@0x80101b90), Clip 2 mit Blend 512, Blob → 0 (@0x80101d20) | ja — bis auf die PL0-Opferanimation (§5.1) |

Die A-Eintraege 1..4 sind `jr ra`-Stummel (je 8 Byte) — kein Port-Bedarf.

### 3.4 Blob-Tabelle @0x80105ABC (Entity `+0x218`, Dispatch @0x80103bc8-e4)

| `+0x218` | Adresse | Was | Port |
|---|---|---|---|
| 0 | 0x80103C18 | **KAMPF-PULS** + Biss-/Devour-Trigger: Amplitudenrampen mit den Schwellen 162/192/8001/−548, Trigger `dist < 6000` (@0x80103e08) ∧ `arc 128` (@0x80103e38) ∧ `+0x229 == 0` → Blob 1 (@0x80103e6c) bzw. 500 Schaden + Blob 4 (@0x80103e94-ec8) | ja |
| 1 | 0x80103EFC | **MASSEN-BISS**: Ausholen/Zuschnappen, **Segment-1-Ausfahrt 2200→3700** (@0x801040b8-e0), Lunge mit der Kappe X < 12001 (@0x80104074), 40 Schaden (@0x8010416c), Nachwackeln, **Segment-1-Rueckzug** (@0x801042b0-d4), Blob → 0 (@0x801042a4) | ja (Segment-Konsum NEU) |
| 2 | 0x80104440 | Gedaempfte Schwingung auf Gewicht 1 (Startwerte 36/10/10/1, `amp -= rate<<2`, Vorzeichenwechsel je Durchlauf) | **DATENTOT** — kein Schreiber setzt `+0x218 = 2`. Die zehn Schreiber in EM036: 0x80100474 (0), 0x80100928 (4), 0x80101208 (3), 0x80101780 (0), 0x80101d20 (0), 0x801030e4 (5), 0x80103e6c (1), 0x80103ec4 (4), 0x801042a4 (0), 0x801044ec (0). EM037 schreibt nur sein eigenes `+0x218` (@0x80100664). |
| 3 | 0x801044FC | **VORKAMPF-PULS** (ab [T0] @0x80101208): Rampen mit 162/192/8501/−1048, Endphasen mit 2047/8000/110 | ja |
| 4 | 0x80104848 | **DEVOUR-BLOB**: Kompression (`slti -4048` @0x8010492c), Kauen mit SE 14/6 (@0x80104a04-08), Rumble 35/250/15 (@0x801048cc-d8), zweite Rumble-Runde 250 (@0x80104af8) | ja (SE + Rampen; Rumble nicht, §5.3) |
| 5 | 0x80104B94 | **TODES-BLOB**: Deflate, Zuckungen, Aufblaehen, Kollaps | ja |

---

## 4. Umsetzung

### 4.1 Code

* `engine/src/enemy_ai_boss_g5.c` — neuer Abschnitt "KOERPER-KOLLISION" mit dem vollen
  Belegblock: `re15_g5_body_segment(idx, …)` liefert die Weltlage eines Segments
  (`FUN_80035408` mit Yaw 0 = der eingefrorenen Vorwaertsachse `vor` des Moduls, wie schon
  beim Biss-Trefferpunkt), gegated auf `g->gestartet` = das geloeschte Wort0-Bit 0x2.
  `re15_g5_body_push_player(pl)` ist `FUN_80034D0C(G5, Spieler)`: Broadphase, `SquareRoot0`,
  Hoehenband `1500 + 1530`, radialer Push `p = d * over / (dist + 1)`, dazu der
  Vorzeichen-Dreh-Zweig (@0x80034ec4-0x80035044).
* `engine/src/enemy_ai_common.c` — `re15_body_push_player` leitet Typ 0x36 auf den
  Segment-Push um, statt den RE1.5-Zylinder zu nehmen. Der 1000er-Kasten aus
  `re15_enemy_apply_hitbox` bleibt unveraendert der **Schadens**-Kasten (Waffenwirkung /
  Auto-Aim) — das ist eine andere Funktion desselben Feldes und wurde nicht angefasst.
* `tests/unit/probe_r17_birkin_1zu1.c` + `tests/unit/probes/r17_birkin-1zu1.cmake`.

**Eine Korrektur am Vorbefund:** Runde 16 §7.3 notiert, das Lokal-Y des Spieler-Segments sei
"fuer das einzige Spieler-Segment nie gesetzt", also 0. Das ist **falsch**: der Spieler-Ctor
schreibt `sh v0=-1530,152(s2)` @0x8003bde4. Der neue Code benutzt den richtigen Wert;
`enemy_ai_tentakel_g5.c` steht weiter auf 0 und ist hier als offener Punkt vermerkt, statt
ihn ohne eigene Messung mitzuaendern (§5.4).

### 4.2 Messwerte vorher/nachher

| Groesse | vorher | nachher |
|---|---|---|
| Standabstand Spieler↔Boss-Ursprung (Sweep 8000→100) | **1451** | **4451** |
| Punkte quer durch die Masse, die im Koerper stecken bleiben | — | **0 von 57** |
| kleinster Abstand im echten Spiel bei gehaltenem "vorwaerts" (sub 0 / sub 1) | (Spieler lief hindurch) | **5094 / 5016** |
| Segment 0 lokal X ueber 6000 Kampfbilder | — | **−2000..−2000** (fest) |
| Segment 1 lokal X ueber 6000 Kampfbilder | — | **2200..3700** |
| Armierungs-Gate: Segmente vor `Member_set(0x0c,0x13)` | — | **keine** |
| ctest (`re15_port/build_r17`) | 321/321 | **322/322** |

---

## 5. Offen (ehrlich)

1. **Devour-Opferanimation auf dem RE2-PL0-Rig** (`0x80103908`, erreicht ueber
   `0x800CE300[0x36] = 0x80103880` @0x801005dc). Der Port spielt seine eigene, byte-true
   cmd-6-Reaktion. Das ist dieselbe Lage wie bei den Zug-Varianten 0/1 der Tentakel
   (Runde 16 §7.9 Punkt 1) und waere nur durch einen Clip-fuer-Clip-Abgleich der
   RE2-Leon-Bank mit der RE1.5-Bank zu schliessen — eigene Arbeit, eigenes Thema.
2. **Blut-Spawn der Trefferroutine** (`jal 0x800154ac` @0x80102888) samt seiner
   Korridor-Klemme Z ∈ [−26000, −20500] (@0x80102760-84) und den Versaetzen −1400/−700/−2000
   (@0x801026cc/@0x80102708/@0x8010273c) sowie der Schwelle `sltiu 1499` @0x80102860. Der
   Port fuehrt nur den Flinch-Akkumulator; der sichtbare Blutauswurf an der Trefferstelle
   fehlt. Der Spawner `0x800154ac` selbst ist NICHT disassembliert — ohne ihn waeren die
   Argumente geraten.
3. **Rumble/Shake-Kaskaden**: Routine 3 feuert 13× `FUN_8003947C` (@0x801034e4-…) und 10×
   `FUN_800395B8` (@0x80103118-…), der Devour-Blob 2× `FUN_8003947C` + 2× `FUN_80039514`
   (@0x801048c4-d8, @0x80104ad8-afc). Die Argument-Semantik dieser Funktionen ist weiterhin
   nicht disassembliert (schon Runde 16 §5); die Werte sind zitiert, nicht umgesetzt.
4. **`enemy_ai_tentakel_g5.c`** setzt im Dreh-Zweig das Lokal-Y des Spieler-Segments auf 0,
   obwohl es −1530 ist (§4.1). Der Zweig greift nur in den Lagen des Kletter-FSM; ich habe
   ihn NICHT nebenbei geaendert, weil dafuer eine eigene Messung an den Tentakel-Pins
   gehoerte.
5. **Bilder von Treffer-Reaktion und Boss-Tod fehlen** (§6): der gescriptete Spieler wird
   vom Devour gefressen (gemessen: sub 4 ab Bild ~1700), bevor er 600 HP herunterschiessen
   kann; drei Laeufe mit Pistole (`RE15_EQUIP=2`) und Ziel-/Feuer-Skript brachten keinen
   einzigen Treffer (Boss-HP blieb 600). Das ist eine Luecke der Sichtpruefung, kein
   gemessener Defekt — die Todes-Maschine haelt ihre Pins aus Runde 6/16.
6. **Kein Parity-Vergleich gegen einen DuckStation-Savestate** dieses Kampfes (wie Runde 16
   §6.5/§7.9): alle Zahlen sind gegen die disassemblierten RE2-Bytes und die eigene Sonde
   geprueft, nicht gegen RAM einer laufenden Konsole.

---

## 6. Sichtpruefung (echte exe, selbst angesehen)

Lauf: `RE15_TITLE_SHOT=… RE15_NO_INTRO=1 RE15_DEBUG_JUMP=5090@30
RE15_PLAYER_POS="13600,-23350,<yaw>" RE15_FORCE_EVENT=4@120 RE15_SOFTWARE_RENDER=1
RE15_BIRKIN_DBG=1 RE15_FRAMEDUMP=…` in `re15_port/build_r17/platform/pc`.

| Bild | Urteil |
|---|---|
| `sicht_solid000200.png` | noch das Vorspann-Standbild — der Sprung greift erst danach. Kein Befund. |
| `sicht_solid000590.png` | Auftritt: die Masse steht am WESTENDE im Wagen, Leon im Korridor davor. Richtig. |
| `sicht_solid000980.png` | die Masse ist herangekrochen, der Kriecherkopf sitzt oben auf, ein Arm quert das Bild. Richtig. |
| `sicht_solid001110.png` | zwei Tentakel greifen nach Leon; zwischen Masse und Leon bleibt eine Luecke. Das ist der Zustand, der vorher nicht existierte. |
| `sicht_solid001240.png` / `001370.png` | Leon ist so weit nach Osten geschoben, dass die Kamera in den Nachbar-Cut umschaltet — der Push traegt ihn vor der Masse her, wie es das Original beschreibt (Runde 16 §2.1). |
| `sicht_druck000880.png` | Druckprobe mit gehaltenem "vorwaerts": Leon steht rechts NEBEN der Masse, nicht darin. |
| `sicht_druck001040.png` | die Masse schiebt sich vor ihn (Zeichenreihenfolge), der Abstand im Protokoll bleibt ≥ 4222. Kein Durchlaufen. |
| `sicht_tod001700.png` | Devour: die Kamera sitzt im geoeffneten Maul, Leon wird verschlungen. |
| `sicht_tod001900.png` | derselbe Finisher mit "YOU DIED" — die Kill-Kette laeuft durch. |

⛔ Nicht abgedeckt: Treffer-Reaktion und Boss-Tod (§5.5).
