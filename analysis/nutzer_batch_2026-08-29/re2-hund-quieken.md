# RE2-AI-Modus: finales lautes Quieken beim Hunde-Tod fehlt — RECON 2026-08-29

Nutzer-Report: „Bei RE2 AI wenn der Hund stirbt fehlt das finale laute Quieken."

**Kurzbefund:** Die Original-Todes-SND ist **ENEMSE SE id 7** (Aufruf `addiu a0,zero,7` @0x801041C8 +
`jal 0x8005bd6c` @0x801041CC im Todeskern 0x80104178, Hunde-Modul `EMD0G_MOD0.BIN`). Der Port HAT den
Aufruf (enemy_ai_re2_dog.c:1944), die Ressource IST gemappt und laut (Bank 6, map[7] → VAG 13, Peak
100 % FS). Der Riss ist Fall **(a)-Variante**: der Port unterdrückt den Aufruf über ein Gate mit
**falscher Zeilen-Semantik** — er wählt die Todes-Variante nach dem *Herkunfts-Substate*
(`re2z_prev_sub`, Z. 1921), das Original wählt sie nach der **Waffen-/Attack-Row in +0x5**, die der
Damage-Applier beim Treffer stempelt (`sb s5,5(s1)` @0x80047324). Ein Hund, der im Port während
IDLE (0), FRESSEN (5), RETREAT/Kreisen (6) oder LOST (9) stirbt, nimmt dadurch die stumme
Gore-Lane (+0x231=1 → kein SE 7) — im Original schreit **jeder** Handfeuerwaffen-Kill.
Exakt dieselbe Fehlerklasse wurde bei der Krähe am 2026-08-23 bereits gefixt
(enemy_ai_re2_crow.c:1423-1438) — der Hund blieb ungefixt.

---

## 1) Original RE2-Retail — Todespfad + Sound-Aufruf

Modul: `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN` (Zombie-Dog kind 0x20, Basis 0x80100000).
Alle Disasm-Zitate via `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py … --bin EMD0G_MOD0.BIN`.

### 1a) Der SE-Aufruf im Todeskern 0x80104178

```
801041b8: lbu v0,561(s2)          ; +0x231 Einmal-Latch
801041c0: bne v0,zero,0x801041d8  ; schon geschrien -> skip
801041c8: addiu a0,zero,7         ; SND-ID 7  <== das finale laute Quieken
801041cc: jal 0x8005bd6c          ; FUN_8005bd6c = Gegner-SE-Trigger (ENEMSE)
801041d0: addu a1,s2,zero         ; a1 = Entity
```

**Die Original-Sound-ID des Todesschreis ist ENEMSE SE 7** (einmalig über den +0x231-Latch).

### 1b) Wie der Tod dorthin dispatcht — und wer den Schrei unterdrückt

State-3-Einstieg 0x801040DC (Zeilen-Dispatch über **+0x5**):
```
801040e4: lbu v0,5(a0)            ; +0x5 = Zeile
801040f8: lw v0,21964(at)         ; Tabelle @0x801055CC[+0x5]
80104100: jalr v0
```
Router 0x80104118 (frischer Tod, +0x6==0 → zweite +0x5-Tabelle):
```
80104120: lbu v0,6(a0)  /  80104128: bne v0,zero,0x80104150
80104130: lbu v0,5(a0)  /  80104144: lw v0,22040(at)   ; @0x80105618[+0x5]
80104158: lw v0,22120(at)                              ; sonst @0x80105668[+0x6]
```

Tabelle @0x80105618 (frischer Tod, pro **Zeile +0x5**), selbst decodiert:

| Zeile | Handler | Bedeutung |
|---|---|---|
| 1,2,3,4,12,13,15,18 | **0x80104178** | Kern → **SE 7 Todesschrei** |
| 0,5,6,9,17,19 | 0x80104610 → 0x80104694 | Gore-Lane (s.u.) |
| 7,8 | 0x801042B0 | eigene Variante (Gore-FX-Tail) |
| 10,16 | 0x80104774 | Variante (unportiert) |
| 11 | 0x8010481C | Variante (unportiert) |
| 14 | 0x801048B4 | Variante (unportiert) |

Gore-Variante 0x80104694 — der EINZIGE Unterdrücker des Schreis:
```
801046a8: lbu v1,5(s0)            ; +0x5 (Zeile)
801046ac: addiu v0,zero,9
801046b0: bne v1,v0,0x801046dc    ; Zeile != 9      -> GORE (stumm)
801046b8: lbu v0,466(s0)          ; +0x1D2
801046c0: sltiu v0,v0,0x3
801046c4: bne v0,zero,0x801046dc  ; +0x1D2 < 3      -> GORE (stumm)
801046cc: jal 0x80104178          ; NUR Zeile 9 UND +0x1D2>=3 -> Kern MIT SE 7
…
801046e0: addiu s1,zero,1
801046e4: jal 0x80104178          ; GORE: Kern, aber…
801046e8: sb s1,561(s0)           ; …Delay-Slot: +0x231=1 VOR dem Kern -> KEIN SE 7
801046ec: jal 0x80104440          ; Part-Scatter (Gibs) statt Schrei
```

### 1c) Was ist die „Zeile" +0x5 beim Tod? — DIE WAFFEN-/ATTACK-ROW

Der RE2-Damage-Applier FUN_800470C0 (`RE2_Quellcode_V2/FUN_800470c0.c`, Anker selbst
re-disassembliert in `info/re2leon/PSX.EXE`):

```
80047278: sw v1,508(s1)           ; altes State-Wort -> +0x1FC (gesichert)
80047284: bgez v1,0x80047294
80047288: sw v0,4(s1)             ; GANZES 32-Bit-Wort = 2 (HURT)  -> +0x5/6/7 = 0
80047290: sw v0,4(s1)             ; bzw. = 3 (DEATH) wenn HP (+0x156) < 0
…
80047324: sb s5,5(s1)             ; DANACH: +0x5 = (char)param_4  (zweiter Zweig @0x80047574)
```
Decompilat (Z. 45-51, 63, 99-105, 120): `puVar5[1] = 2;` / `= 3;` dann
`*(char *)((int)puVar5 + 5) = (char)param_4;` — und `param_4 & 0xffff` ist genau der Index in die
per-kind-Schadens-Tabelle `PTR_DAT_800a6a88[kind] + row*0x14`, d.h. **die Waffen-/Attack-Row**.
Der alte Substate ist zu diesem Zeitpunkt zerstört (nur noch in +0x1FC archiviert).

Konsequenz: **beim Todes-Dispatch ist +0x5 die Waffe, nie der Verhaltens-Substate.** Mit der
RE1.5→RE2-Waffenkarte des Ports (`re2z_row_from_weapon`, enemy_ai_re2_zombie.c:3418:
Pistole w3 → RE2-Row 3) läuft **jeder Handfeuerwaffen-Kill** über Row 1-4 → Kern 0x80104178 →
**SE 7 spielt immer**. Die stummen Gore-Zeilen {5,6,9(nah),17,19} sind Magnum-/Projektil-Klassen
(Interpretation der Labels = HYPOTHESE; die Struktur Row=Waffe ist byte-belegt, und die
Port-eigene Krähen-Analyse enemy_ai_re2_crow.c:1423-1424 bestätigt dieselbe Semantik:
„die 19 Zeilen … sind TREFFER-CODES …, NICHT der Herkunfts-Sub").

## 2) Port — was heute passiert

### 2a) Der SE-Aufruf existiert

`re15_port/engine/src/enemy_ai_re2_dog.c:1944`:
```c
if (e->re2d_se231 == 0) { re2d_se(7); }            /* SE 7 Todesschrei @0x801041C8-CC */
```
Kette: `re2d_se` (:142) → Hook `re15_audio_re2_enemy_se` (main.c:704 — der Hund nutzt
**nicht** den RE1.5-Mapper `pc_re2z_se_re15`; der ist Zombie-only, main.c:723) → ENEMSE-Bank 6
(`RE2DOG_ENEMSE_BANK`, dog.c:130). Unit-Test pinnt den Schrei (tests/unit/test_re2_dog_ai.c:371,
Fixture mit prev_sub=1).

### 2b) Die Ressource ist gemappt, vorhanden und laut

`re15_port/shared_assets/RE2/ENEMSE.VBS` existiert (0x662800 B). Bank 6 (TOC @0x800A7B1C, EXE-Datei
0x9831C): EDT @0x75800 (0xCA8), VBD @0x76800 (0xF8F0). Selbst decodiert:
**map[7] = 0x04820000 → prog 0, tone 8, chan 4, prio 2 → tone: vol=100, vag=13; VAG 13 = 2928 B
= 5124 Samples, Peak 32606 (100 % FS).** Kanal 4 < MIXER_RE2SE_CH_COUNT=8 (audio_pc.c:75).
Kanal-4-Konkurrenz ist minimal: EDD-Frame-SEs des Hundes (CDEMD0_EM20 EDD, selbst gescannt) feuern
id 5 nur in Clip 23 (Latch) und id 6 nur in Clip 14 — die Kill-Standardfälle haben Kanal 4 frei.
→ Fall (b) und (c) scheiden aus.

### 2c) DER RISS — falsche Zeilen-Semantik im Todes-Dispatch

`enemy_ai_re2_dog.c:1921/1925/1927/1935`:
```c
uint8_t row = e->re2z_prev_sub;                              /* :1921  <== RISS */
…
int gore = (row == 0 || row == 5 || row == 6 || row == 9);   /* :1925 */
if (gore && !(row == 9 && e->re2z_hits1d2 < 3)) {            /* :1927 */
    e->re2d_se231 = 1;                                       /* :1935 -> kein SE 7 */
```
`re2z_prev_sub` ist der **letzte ACTIVE-Substate** (:2106, nur in state 1 aktualisiert) — also die
Verhaltensphase, nicht die Waffe. Folgen:

- Kill während **IDLE (0)** (z. B. Distanzschuss auf ruhenden Hund), **FRESSEN (5)**,
  **RETREAT/Kreisen (6)** (die dominante Phase zwischen den Sprüngen) oder **LOST (9)**
  → Gore-Lane → `re2d_se231=1` → **SE 7 stumm**. Der Gib-Scatter ist zudem „Render-seitig OPEN"
  (:1937) — man sieht UND hört nichts. Das ist das fehlende Quieken.
- Kill während RUN (2)/ATTACK (3)/STALK (1) → Schrei spielt auch heute (deckt sich mit dem Pin).
- Zweitfehler in derselben Zeile: das `!(row==9 && hits<3)` **invertiert** das Original-Gate
  (@0x801046B0-C4: NUR row 9 UND +0x1D2>=3 erreicht den Schrei-Kern; der Port spielt den Schrei im
  Komplementärfall).
- Drittfehler gleicher Familie: `:1922-1924` deutet Row 7/8 als „stirbt im Latch"
  (victim_throwoff) — im Original sind [7]/[8] Waffen-Rows mit eigener Todes-Variante 0x801042B0.
- `re2d_hurt` (:1838) dispatcht die HURT-Zeilen ebenso mit `prev_sub` — dieselbe Baustelle
  (dort mit eigenen Folge-Divergenzen, hier nicht weiter verfolgt).

### 2d) Präzedenzfall: die Krähe hatte EXAKT diesen Bug — und ist gefixt

`enemy_ai_re2_crow.c:1425-1431` (⛔ FIX 2026-08-23, Nutzer-Report „Gib bei Pistolentreffern im
RE2-Modus"): „das alte MAPPING nahm den Herkunfts-Sub (re2z_prev_sub) als Zeile … Jetzt: Zeile =
RE1.5-Waffe (+0x5, Resolver-Stempel `sb s8,0x5` @0x800124bc) übersetzt … via
s_re2c_row_from_weapon". Der Hund wurde damals nicht mitgezogen.

Alle Bausteine für den Fix existieren schon:
- `re15_damage.c:1413`: `e->sub_state_1 = (uint8_t)weapon_id;` — der Port stempelt die
  RE1.5-Waffe byte-true in +0x5 (@0x800124bc).
- `re2z_row_from_weapon[22]` (enemy_ai_re2_zombie.c:3418, identisch crow:1433, spider:1556):
  RE1.5-Waffe → RE2-Row (Pistole 3 → 3).

## 3) Fix-Vorschlag

In `re2d_death` (enemy_ai_re2_dog.c:1918-1953):
1. `row` aus der Waffe ableiten: `row = re2z_row_from_weapon[e->sub_state_1]` (Karte als
   geteilten Helfer exportieren oder Hunde-Kopie wie bei der Krähe). Der Stempel liegt beim
   Todeseintritt garantiert in `sub_state_1` (re15_damage.c:1413; der RE2-Stempel setzt nur +0x6=0).
2. Lanes nach Tabelle @0x80105618: Rows {5,6,17,19} → Gore-Lane (+0x231=1, Scatter);
   Row 9 → **Original-Richtung wiederherstellen**: `+0x1D2 >= 3` → Kern mit SE 7, sonst Gore
   (mit Port-Bracket 0 heißt das: GL-Row 9 gibbt immer — wie der RE2-Nah-Treffer);
   Rows {7,8} → 0x801042B0-Lane; Rows {10,11,14,16} → unportierte Varianten: bis zum RE
   dokumentiert auf den Kern (Schrei) fallen lassen, NICHT stumm; alles andere → Kern → SE 7.
3. Grab-Abbruch (victim_throwoff) von der Row lösen — am tatsächlichen Latch-Zustand festmachen
   (`re15_player_victim_state()==1`, ggf. && `re2z_prev_sub==7/12`), denn Row 7 ist jetzt eine Waffe.
4. Pin: Handfeuerwaffen-Kill mit `re2z_prev_sub==6` (Retreat-Fixture) → `se_count(7)==1`
   (heute: 0). Bestands-Pin :371 bleibt grün.

## 4) Offenes

- **HURT-Router gleiche Familie** (:1838): Zeilen [0]/[4]/[9]/[10]/[11]/[14]/[15]/[16] sind
  Waffen-Rows, nicht Substates — die dortigen Deutungen („Schlaf-Treffer", „Glas-Schauer",
  „Heulen/Sprung-Kick") müssen gegen die Waffen-Semantik neu ge-RE'd werden (eigener Batch).
- Todes-Varianten 0x80104774 / 0x8010481C / 0x801048B4 / 0x801042B0 (Flamme/Säure/Spark/Shotgun-
  Klasse — Waffen-Labels HYPOTHESE) sind unportiert; mit RE1.5-GL-Runden/Flammenwerfer erreichbar.
- Exakte RE2-Waffen-Namen pro Row: nicht byte-belegt (nur die Struktur Row=Waffen-Index ist es).
- Nutzer-Mandat „RE2-AI = RE1.5-Sounds" (2026-08-23) wurde nur für die Zombie-Familie umgesetzt
  (main.c:723); der Hund spielt bewusst ENEMSE-Samples (main.c:704). Ob das Todes-Quieken nach dem
  Fix das RE2-Sample (Bank 6 SE 7) oder ein RE1.5-Raum-SE sein soll → Nutzer-Entscheidung.

## 5) Werkzeug-/Datenpfade

- Disasm: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (EXE + `--bin EMD0G_MOD0.BIN`).
- Bank-6-Decoder + EDD-Scan: Scratchpad `enemse_bank6.py` / `enemse_bank6_tones.py` /
  `em20_edd_scan.py` (Session 25fdb723…, reproduzierbar aus diesem Dossier).
- ENEMSE-TOC: `info/re2leon/PSX.EXE` @0x800A7B1C (Datei 0x9831C), Bank 6 = Zeile {0x0C,0x00}
  (Zensus-Beleg im Kopf von enemy_ai_re2_dog.c:96-129).
