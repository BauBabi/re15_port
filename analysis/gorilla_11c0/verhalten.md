# Gorilla-Boss (Typ 0x27, ROOM11C0) — Verhaltens-Divergenzen Original vs. Port

Datum: 2026-08-30. Nutzer-Report (im Spiel getestet): „die Gorillas verhalten sich überhaupt nicht original" — trotz „byte-true"-Etikett (RE15_MAGGOT_AI.md, „16 States").
Schwester-Dossiers: `analysis/gorilla_11c0/groesse.md` („zu klein", paralleler Agent — Scale-Fix in Arbeit, `render_scale_q12` seit heute im INIT), `analysis/nutzer_batch_2026-08-29/affen-treffer-clip.md` (Treffer/Clipping-Recon, Teil-Fixes am 2026-08-29 gelandet).

Alle Original-Adressen unten selbst nachdisassembliert (`re15_disasm.py --bin STAGE1.BIN`, Overlay lädt @0x80100000, kein Header-Offset) bzw. aus `RE_15_Quellcode_Overlays/STAGE1_full/` + `RE_15_Quellcode_V2/`.

---

## 1. Original — Root, Dispatch, State-Tabelle

### 1.1 Root FUN_80116db8 (disasm-verifiziert, alle 102 Instruktionen)
Gate `(DAT_800aca40 & 0x20000000)==0 && (ent+9 & 0x20)==0` @0x80116dcc-df8, dann:
- @0x80116e00-24: Dispatch `@0x801213c8[ent[+0x4]]` (jalr).
- @0x80116e30 `jal 0x8002b498(ent)` — Locator-Vektor drehen + **`ent+0x1c2 = 0`** (Kontakt-Clear).
- **@0x80116e40-44 `jal 0x8002aec4(a0=0x800aca54 /*SPIELER*/, a1=Gorilla)`** → Rückgabe `sh v0,0x1d2(ent)` @0x80116e54. `FUN_8002aec4(pusher, pushee)` VERSCHIEBT param_2 (+0x34/+0x3c, @0x8002af8c ff.) aus der Ellipse von param_1; `pushee+0x1c2 |= 1` wenn pusher==Spieler, sonst `|= 2` + Pusher-Zeiger → +0x1ac; Grab-Paar-Ausnahme both-0x1000 @0x8002af14. **⇒ Der GORILLA wird aus dem Spieler herausgeschoben.**
- @0x80116e50 `jal 0x8002b544()` — Schleife über ALLE lebenden Entities (Basis 0x800acc2c, Anzahl DAT_800aca4e, Stride 0x1f4; der Spieler @0x800aca54 ist NICHT in diesem Array) → `FUN_8002aec4(other, Gorilla)` = Gorilla-vs-Gorilla/Gegner-Separation.
- @0x80116e64-84 `jal 0x8003b0a4(ent+0x34, skel[6]=1600, 4)` → `+0x1d6` (SCA-Wand-Klemme + Kontakt-Flag).
- Gate `(ent+9 & 0x40)==0` @0x80116e9c: Schatten-OTZ `+0x1d8 = skel[6]+100+((+0x38 − s16 +0x1ba)>>3)` @0x80116ea8-ec8 (min 100 @0x80116ee0-e8); **`+0xb4 = +0x1d8` @0x80116f00, `+0xb6 = +0xb4+100` @0x80116f1c**; dann `jal 0x8001b064(ent+0xb0, +0x1ba)` Floor-Shadow @0x80116f34.

### 1.2 Dispatch-Tabelle @0x801213c8 — 40 Einträge (0..39)
Struktur: PRIMARY [0..7] auf `+0x4`; A-DECISION [8..23] = `@0x801213e8[+0x5]` und B-MOVEMENT [24..39] = `@0x80121428[+0x5]`, beide vom STATE-1-Brain gerufen. Nach Eintrag 39 folgen Byte-Daten (Fenster-Tabellen @0x8012146c/0x80121470/0x80121478).

| +0x4 | Handler | Rolle |
|---|---|---|
| 0 | 0x80116f50 | INIT: HP=180 (Tab @0x8011f034 Zeile 0x27 @0x8011f514), Clear +0x94/95/8f/93, Steer=Spieler, +0x1b9=2, +0x1e2=4, **flags = (flags&0x1fffffff) \| 0x800 + `+0x166=0x1b33` (1.7× Scale; grid&0x40-Override `((grid&0xf)+10)*0x1000/10`)**, Box +0x78=@0x80121350 {0,-1440,0,1600,1440,1600} → state 1 (sub 1 wenn grid&1) |
| 1 | 0x80117254 | ACTIVE-Brain (s. 1.3) |
| 2 | 0x8011af5c | HURT: Lanes @0x801214a8 — [0-6] Flinch clip 7 (0x8011b018), [7-8] Air-Hit clip 8/9 (0x8011b1ec), [9-11] Crash clip 0xa/0xb + Get-up 0x10/0x11 (0x8011b400); Exit → state 1 sub 7 (Retaliation-Leap; sub 9 wenn +0x1e3) |
| 3 | 0x8011b6fc | DEATH: Lanes @0x80121500 — Boden clip 0xe / Mid-Air 0xa/0xb / Crash; Kill-Flag 0x8004ef90(0x800b1038,+0x1c6) @0x8011b8e8; → state **7** |
| 4/5 | 0x8011bdec | Sub-Dispatch `@0x80121558[+0x5]`: sub 0-3 = `jr ra`-Stubs @0x8011be34/3c/44/4c; **sub 4/5 → 0x8011c158, sub 10/11 → 0x8011c414** = die Opfer-FSM-Routinen des Pin-Handlers (kein STAGE1-Writer von +0x4=4/5 gefunden — Legacy-Route auf denselben Code wie Hook A/B) |
| 6 | 0x8011c598 | `jr ra`-Stub (verifiziert @0x8011c598-9c) |
| 7 | 0x8011be54 | CORPSE: +0x9e=90-Tick-Fade, flags\|=0x42, kein +0x94-Write |
| 8..23 | A-Decision | A[0]=0x80117484 idle-decide, A[1]=0x80117668 dormant-decide, A[2]=0x80117858 stub, A[3]=0x80117a3c CHASE-decide, A[4]=0x80117e40 SELECTOR-decide, A[5..8]=Stubs (0x80118268/544/900/dd4), A[15]=0x8011a878 Airborne/Landing, … |
| 24..39 | B-Movement | B[0]=0x80117574 idle-wander clip 0x16, B[1]=0x80117764 dormant clip 0, B[2]=0x80117860 RISE clips {3,2,1}, B[3]=0x80117c90 crawl clip 4/5, B[4]=0x80118110 heavy-approach clip 6, B[5]=0x80118270 BITE clip 0x12 (−6), B[6]=0x8011854c HEAVY clip 0x13 (−12), B[7]=0x80118908 LEAP clip 0x14, B[8]=0x80118ddc FINISHER clip 0x15 (−600, Devour), B[15]=0x8011a960 REAR-UP-PIN clip 0x1c, B[9-14]=0x8011936c/971c/9a6c/9d0c/a1f8/a44c (Zone-Leaps/Spezial, +0x1e3-gated) |

### 1.3 Brain STATE[1] 0x80117254 — Kopf + Tail
- @0x8011725c `jal 0x8001bc08` (LOS) → +0x1d8; Sticky-LOS `if((+0x1d8>>1)==0){+0x1d0&=~1; |=+0x1d8&1}` @0x80117288-c0; Distanz `SquareRoot0` @0x80117300 → +0x1d4 @0x80117314.
- A-Dispatch `@0x801213e8[+0x5]` (jalr @0x80117344, Delay `lui s0,0x1fff` @0x80117348), B-Dispatch `@0x80121428[+0x5]` (jalr @0x80117378, Delay `ori s0,s0,0xffff` @0x8011737c) → **s0 = 0x1fffffff**.
- **AIM-BAND-Stempel:**
  - @0x80117380-98 `flags &= 0x1fffffff` (Bänder löschen)
  - @0x801173a8-b8 `flags |= 0x40000000` (LEVEL) + `jal FUN_80012974(0xfa0=4000)`: dist(Spieler)<4000 ⇒ `flags |= 0x20000000` (DOWN) @0x800129cc-f0
  - @0x801173c8-e8 wenn `+0x1e0` (airborne): `flags &= 0x1fffffff` NOCHMAL + `jal FUN_80012a0c(0x1388=5000)`: dist<5000 ⇒ `flags |= 0x80000000` (NUR UP) @0x80012a70-88
  - ⇒ **Am Boden: LEVEL + DOWN(dist<4000). Im Sprung: NUR UP (dist<5000).**
- Tail-Timer: +0x1dc−− @0x801173f8-40c, +0x1e1−− @0x8011741c-38, +0x1de Kontakt-Zähler @0x80117448-6c (ohne Verbraucher, tot).

### 1.4 Pin-Opfer-System (Hook A/B) — VOLLSTÄNDIG DISASSEMBLIERT
**Registrierung (STAGE1-Init):** Hook-Tabelle A Basis 0x800ac758, Index = Gegner-TYP: **[0x27] = 0x800ac7f4 ← 0x8011c118** @0x8011ea2c-38 (gleiches Muster wie Krähe [0x21]=0x800ac7dc←0x8011597c @0x8011ea1c-28). Hook-Tabelle B Basis 0x800ac858: **[0x27] = 0x800ac8f4 ← 0x8011c3d4** @0x8011eab4-ac8 (Devour-Lane aca58=6).

**Hook A 0x8011c118 (läuft im cmd-5-Spieler-Tick):** Entry `lbu 0x800aca59` (Variante) @0x8011c118-1c → Dispatch `@0x80121568[aca59]`: 0/1 → **0x8011c158**, 6/7 → **0x8011c414**, 2-5 → NULL.
- **0x8011c158:** Phase `aca5a` ≥8 → exit; Phasen-Tabelle **@0x80100404** = {c194, c228, c268, c2e8, c314, c31c, c348, c384}:
  - **P0 @0x8011c194:** aca5a=1; **acae8=1 (= Opfer-Bank-CLIP 1, der 83-Frame-WURF)**; Zwangs-Yaw `acabe = [acbfc](Gorilla)+0x6a`; acae9=0, acae3=0; `0x80045024(0x04000001, gorilla+0x34)`; `aca3c |= 0xc0`; **aca59≠0 (Rück-Variante) → aca5a=2 UND acae9=0xc** @0x8011c208-1c (Clip startet bei Frame 12).
  - **P1 @0x8011c228:** `anim_set 0x8001f314([acbcc],[acbd0], 0, 0x200)`; bei acae9==0xb → aca5a=2.
  - **P2 @0x8011c268:** **NUR solange acae9 < 0x25 (`sltiu` @0x8011c278): `0x8001ad68(player=0x800aca54, [acbcc], [acbd0], 0)` @0x8011c294 = Root-PLATZIERUNG Leons am Gorilla**; danach anim_set; **Clip-ENDE → aca5a=3 + `player.flags &= ~0x1000` (UNPIN) @0x8011c2c0-dc**. Leon fliegt ab Frame 0x25 frei; Unpin exakt am LEON-Clip-Ende.
  - **P3-P7:** P3 aca5a=4, acae8=0x10, acae3=7 @0x8011c2e8; P4 `anim_set([acad8],[acbc0], 0, 0x200)` (Leons EIGENE Aufsteh-Bank) @0x8011c34c-60; P5 aca5a=6, acae8=0xb, acae3=0xb @0x8011c31c; P6 anim_set(...,1,...) @0x8011c348; **P7 `aca58=2` @0x8011c38c, `player.+0x93=0` @0x8011c3a0, `aca3c &= ~0xc0` @0x8011c398-b4.**
- **Hook B 0x8011c3d4 (Devour):** Dispatch `@0x80121580[aca59]` → 0/1 → **0x8011c414**: P0 @0x8011c460: aca5a=1, acae3=7, Blut-FX `0x80019700(0x2000, gorilla+0x6a, [acbdc]+0x5a0, vec @0x80121570)` @0x8011c4a0, `0x80045024(0x04030001, …)` @0x8011c4b8, aca3c|=0xc0; P1: bei acae9==0x3c `0x80045630(2,0)` + Gore @0x8011c4ec-510; P2 @0x8011c55c…

### 1.5 Angriffs-Trefferprüfung (Referenz, affen-treffer-clip.md §2, disasm-verifiziert)
`FUN_8001bff8` = achsenparalleles QUADRAT um einen ANGRIFFS-BONE (Transform durch die POOL-Bone-Matrix `jal 0x80022da0` @0x8001c078): Biss Bone 9 r=1000 @0x801183c0-cc; Heavy dual Bone 6/10 r=800 @0x801186f0-714; Pin-Connect Bone 5 r=800 @0x8011ab24-78; Finisher dual r=800 @0x801190dc-9100. KEIN Winkeltest.

---

## 2. Divergenzen Port ↔ Original (nach Spieler-Sichtbarkeit priorisiert)

### D1 — TREFFER-HÖHENBAND fehlt: „nach unten zielen trifft den Gorilla NIE" (SEHR SICHTBAR, OFFEN)
- **Original:** §1.3 — LEVEL immer + DOWN bei dist<4000 (0xfa0 @0x801173a4); airborne NUR UP bei dist<5000 (0x1388 @0x801173d4). Bits: LEVEL 0x40000000 @0x801173ac, DOWN @0x800129d4, UP @0x80012a6c.
- **Port:** `re15_port/engine/src/re15_damage.c:1376-1386` — expliziter OFFEN-Block, Typ 0x27 fällt auf `eband = 0x40000000` (pauschal LEVEL; nur grid&0x80 → DOWN<5000). Der Kommentar nennt „Maggot @0x801173a8-b8" wörtlich als unerledigt.
- **Folge:** Nach-unten-Zielen auf das niedrige Krabbelvieh = stiller Whiff (identische Bug-Klasse wie der Hund-Fix 2026-08-29, re15_damage.c:1348-1364); der springende Gorilla ist fälschlich LEVEL-treffbar.
- **Fix:** 0x27-Zweig analog Hund: `eband = 0x40000000 | (bdist<0xfa0 ? 0x20000000 : 0)`; wenn `e->mag_airborne`: `eband = (bdist<0x1388) ? 0x80000000 : 0`. Sauberer: Brain-Tail stempelt `e->aim_band` (wie Hund/Krähe), Damage liest nur.

### D2 — Körper-Push `aec4(player, gorilla)` fehlt: Gorilla schiebt sich durch/über Leon (SICHTBAR, OFFEN)
- **Original:** §1.1 @0x80116e40-44 — der Gorilla wird pro Tick aus dem SPIELER herausgeschoben (und via b544 aus allen Gegnern). Der Spieler ist nicht im b544-Array — darum der separate Aufruf.
- **Port:** `re15_enemy_body_push_tail` (enemy_ai_common.c:12760-12770, Aufruf :13254) beginnt bei `RE15_ACTOR_SLOT_PLAYER + 1` — der Spieler-Pass fehlt. Nur der Spieler-Tick-Push (`re15_body_push_player`, game_step_common.c:1280/1744) schiebt LEON aus dem Gorilla — mit bis ~1950 Einheiten/Frame (affen-treffer-clip.md §3.1). Krähe/Spinne haben den Aufruf (enemy_ai_common.c:6409-6411 / 7990-7994), der Gorilla nicht.
- **Folge:** Landet der Boss-Leap auf Leon, fliegt LEON statt dass der Boss abgleitet; an der Wand rückt der Boss ungebremst in Leon hinein → Überlappungen, Mit-Verursacher von „Leon clippt in unerreichbare Bereiche". (+0x1d2-Rückgabe hat im Gorilla-Bereich KEINEN Leser — Voll-Scan lh/lhu imm=466 über 0x80116db8..0x8011c600 — nur der Positions-Effekt zählt.)
- **Fix:** Im 0x27-Zweig vor `re15_enemy_body_push_tail`: `re15_body_push(pl, RE15_BODY_R_PLAYER, e, e->hit_radius_min)` — exakt das Krähen-Muster.

### D3 — Dedizierter Pin-Opfer-Handler 0x8011c118 UNPORTIERT: Halte-Loop statt WURF (SEHR SICHTBAR, OFFEN)
- **Original:** §1.4 — EIN Wurf-Clip (Opfer-Bank Clip 1, 83 Frames; Rück-Variante ab Frame 0xc), Platzierung am Gorilla NUR Frames <0x25, Unpin exakt am LEON-Clip-Ende, danach Leons eigene Aufsteher (Clips 0x10/0xb aus [acad8]/[acbc0]), Freeze-Bits aca3c 0xc0 an/aus, Abschluss aca58=2.
- **Port:** generische Zombie-Opfermaschine `re15_victim_place`/`re15_player_victim_tick` (enemy_ai_common.c:1301-1369, 1374 ff): Hold-LOOP des 83-Frame-Wurf-Clips (Clip-Map Zombie-Default :892-988; Root-Auslenkung 4364 absolut platziert), Release auf der GORILLA-Timeline (sub-15 Phase 4/5, `s_victim_phase=4` :8578 ff), Variante 1 (hinten) = Clips 3/4/5 out-of-range → Leon friert (affen-treffer-clip.md §3.2). Seit 2026-08-29 mildert eine Wandklemme (:1336-1366) das Weg-Clippen; ihr eigener Kommentar (:1331-1333) benennt den Handler als offen.
- **Folge:** Komplett andere Choreographie als das Original (Wurf + eigener Aufsteher vs. endloser Halte-Loop) — Kernstück des Nutzer-Reports.
- **Fix:** 0x27-Zweig in der Opfermaschine nach §1.4: (1) Variante=a780 bei Latch; (2) Wurf-Clip EINMAL (Rück-Variante ab f12); (3) Platzierung nur f<0x25 (re15_clip_root_motion_abs + Wandklemme des Spieler-Ticks), ab f0x25 freie Clip-Ballistik; (4) Unpin am LEON-Clip-Ende; (5) Leons Recovery Clip 0x10→0xb aus SEINER Bank; (6) danach cmd-2; Gorilla-Release-Kopplung von der Gorilla- auf die Leon-Timeline umstellen.

### D4 — 1.7×-Scale wirkt im Original auch aufs VERHALTEN: Reichweite + Fortbewegung (SEHR SICHTBAR, IN ARBEIT nur render-seitig)
- **Original:** INIT `flags|=0x800` + `+0x166=0x1b33` (STAGE1_full/FUN_80116f50.c; groesse.md FUND 1). `FUN_8001e8c8` skaliert die Root-Matrix +0x20; die POOL-Bone-Records (Stride 172, Welt-t @+84/88/92) ketten daran → **skaliert**. Konsequenz: (a) `FUN_8001bff8`-Angriffspunkte (POOL-Matrix @0x8001c078) liegen 1.7× weiter draußen; (b) Foot-Plant `FUN_8011bf50`/`c024` (`+0x34 -= (m.tx - rec[84])` @0x8011bfd4-e8) liefert 1.7× der rohen Clip-Rootbewegung = Krabbel-/Lunge-TEMPO.
- **Port:** Stand JETZT (2026-08-30, Quelle in Bewegung durch den groesse-Agenten): `render_scale_q12` wird im INIT gesetzt (enemy_ai_common.c:8193-8195), hat aber noch KEINEN Konsumenten — weder Renderer noch `re15_maggot_footlock` (:8113-8146, rechnet aus rohem EMR) noch `re15_maggot_bone_square` (:8090-8098, `re15_enemy_bone_world_pos` unskaliert).
- **Folge:** Gorilla zu klein (groesse.md), schließt pro Locomotion-Clip nur ~59% der Distanz, Angriffe reichen ~59% — zu klein + zu träge + zu kurz = „überhaupt nicht original".
- **Fix:** Scale in die Pose-/Bone-Welt-Berechnung ziehen (footlock-Deltas und bone_square-Weltpunkte ×`render_scale_q12`/4096, Gate flags&0x800), zusätzlich zum Render-Pfad des groesse-Fixes.

### D5 — Angriffs-Connects: Proxy-Umbau seit 2026-08-29 gelandet, Reste offen (TEILGEFIXT)
- Der frühere Zentrum+Arc-Proxy (Whiff-Band 2051..3000, affen-treffer-clip.md §2) ist ersetzt durch `re15_maggot_bone_square` (:8340, :8371-8372, :8501-8502, :8550). Offen: der lokale a1-Offset-Vektor des Originals ist nicht modelliert (:8088-8089) und die Bone-Position ist unskaliert (D4). ⚠️ Ob der Fix im vom Nutzer getesteten PAKET war, ist unklar (Fix 2026-08-29, Test 2026-08-30 — Paketstand prüfen, Memory „Paket lügt").

### D6 — Kleinere verifizierte Punkte
- Staaten 4/5/6-Fold (enemy_ai_common.c:8773-8776): Original-Handler sind `jr ra`-Stubs bzw. die Opfer-FSM-Routen (§1.2) — der Fold verliert nichts, SOLANGE D3 (Hook) portiert wird; danach sind sub 4/5/10/11 die Hook-Codepfade.
- A/B-Lanes 9-14 (Zone-Leaps, +0x1e3): im Port OFFEN benannt (:8296-8305, :8604-8607) — braucht die `FUN_8003b93c`-SCA-Attr-Zonenabfrage; im Kampfbild sekundär.
- +0x1de-Zähler: toter Code im Original (nur INIT-Clear @0x8011706c + Inkrement @0x80117458-6c, kein Leser) — Port lässt ihn bewusst weg, OK.

---

## 3. Aktivierung / Kontext ROOM11C0

sub00 (ROOM11C0.RDT, SCD-Basis Datei 0x1758, sub00 @+0x10 = 0x1768; Bytes selbst dekodiert):
- `06 00 60 00` + `21 04 40 00` = If **Ck(Zone 4, Bit 0x40, ==0)** → **Cutscene-Layout:** `44 00 42 40 …` NPC 0x42 (grid 0x40) @(-8965,0,-14347) + **2× `44 0N 27 30 …` Gorillas grid 0x30** @(-1220,-20000,-21568)/(-554,-20000,-25423), em_flags 0x60/0x61; danach `2e 03 00 00` + `34 00..05 …` = Prop-Member-Sets (gemessen im Port: prop[0] m0..5 = -840/-2930/-19110/-72/-1400/48). **grid 0x30 ⇒ Bit 0x20 = Root-Skip (eingefroren, Cutscene-Props) + Bit 0x10.**
- `07 00 52 00` (Else) + `06 00 48 00` + `21 03 43 00` = If **Ck(Zone 3, Bit 0x43, ==0)** → **Kampf-Layout:** NPC 0x42 @(-18214,-20000,-7229) + **2× Gorillas grid 0x10** @(-9013,0,-15461)/(9434,0,2189). **Flag 3:0x43 = „beide 11C0-Bosse tot"** ⇒ danach spawnt nichts mehr.
- Es folgen `54 00 01 …` (Aot) und `06/21 Ck(4,0x40) / 04 0a / 18 02` (Event-GOSUB) — die Cutscene-/Unfreeze-Choreo (sub02-Route, hier nicht weiter verfolgt).

**Port-Parität (dynamisch gemessen, Probe Teil A/B):** beide Branches + Flag-Gates + Positionen/grid byte-genau im Port (`op_sce_em_set` dekodiert LE korrekt; Kill-Flag-Gate pc[7] + Spawn-Cap work_vars[0x11]/[0x12] vorhanden). Frozen-Gate grid&0x20 hält den Gorilla in state 0 (enemy_ai_common.c:8168 == @0x80116df4-f8). ⇒ Die Aktivierungssequenz ist NICHT die Divergenz.

---

## 4. Dynamische Probe (Port, Build-Lib-Stand 2026-08-29 23:25)

`scratchpad/probe_gorilla_11c0.c` (standalone gegen `re15_port/build/engine/libre15_engine.a` + `libre15_test_support.a`; NICHTS im Repo verändert):
- **Teil A/B (Spawn):** s. §3 — Parität bestätigt. (Ein anfängliches „Aktoren korrupt nach 120 Ticks" war ein Harness-Artefakt: `scd_register_current_rdt` fehlte, sub00s Event-GOSUB `18 02` lief ins Leere; mit sauberem Setup stabil korrekt.)
- **Teil C (Kampf, 3600 Frames, EM027-Bank geladen):** Sequenz idle 0x16 → (dist 5000+LOS) CHASE sub3 clip 5 → (dist<3000) BITE sub5 clip 0x12 → Rear-up sub15 clip 0x1c → Heavy −12 — Brain-Gates stimmen mit §1.3 überein. Gorilla bewegt sich (footlock aktiv), Bisse verbinden mit Bone-Quadraten (−6/−12-Ketten, 1 Pin-Connect −6).
  ⚠️ Zwei Harness-Fallen dokumentiert (für spätere Messungen): (1) Gegner-Bank braucht `bank->ok=1` + residenten Buffer, sonst footlock inert + bone_square-Fallback = „bewegt sich nie / trifft nie"; (2) `pl->hit_react` muss der Harness zurücksetzen (kein Spieler-FSM), sonst gaten alle Folgefenster.
- Die Probe misst die AI-Maschine ISOLIERT auf dem NEUESTEN Quellstand — sie widerlegt NICHT die Nutzer-Beobachtung am Paket: D1/D2/D3/D4 liegen außerhalb dieser Maschine (Damage-Band, game_step-Push, Opfermaschine, Render/Pose-Scale) bzw. der Paketstand kann älter sein (Memory „Paket lügt": sha256 + Symbol-Gegenprobe beim nächsten Paketbau).

---

## 5. Kurzfazit

Die A/B-State-Maschine selbst ist im aktuellen Quellstand weitgehend byte-true (Gates/Clips/Damage-Werte messen sich original). „Verhalten sich überhaupt nicht original" speist sich aus vier offenen, spieler-sichtbaren Mechanismen AUSSERHALB der State-Maschine:
1. **D4** Scale 1.7× fehlt in Reichweite+Tempo (und bis heute im Rendering) — Gorilla klein, träge, kurze Angriffe.
2. **D3** Pin = Halte-Loop statt Ein-Wurf-Choreo mit eigenem Aufsteher.
3. **D1** Höhenband: nach unten zielen trifft nie, springender Gorilla falsch treffbar.
4. **D2** Push-Asymmetrie: Leon wird geschoben statt der Gorilla.
Dazu D5-Restrisiko: der Connect-Fix vom 2026-08-29 war evtl. nicht im getesteten Paket.
