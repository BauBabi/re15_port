# Affen (Typ 0x27): „treffen Leon so gut wie nie" + „Leon clippt in unerreichbare Bereiche"

Recon-Dossier 2026-08-29. Alle Overlay-Adressen selbst nachdisassembliert
(`re15_disasm.py --bin STAGE1.BIN`, laedt @0x80100000; EXE-Adressen aus `info/Re1.5/PSX.EXE`).
Alle Datei-Offsets selbst geparst (RDT/EMS, Parser gegen Port-Tabellen validiert).

## 0) Kurzfassung (Verdict)

- **„Die Affen" = Typ 0x27 (EM027), je ZWEI pro Raum in ROOM11C0 (Leon-Parkgarage) und
  ROOM1141 (Elza-Variante des Briefing-Raums).** Kein weiterer Affen-Typ im Spiel.
- **Symptom A (treffen nie):** Das Original prueft Treffer per **Quadrat-Box um einen
  ANGRIFFS-BONE** (Biss: Bone 9/Kiefer, r=1000; Heavy: Bones 6+10, r=800) — FUN_8001bff8,
  **ohne Winkeltest**. Der Port ersetzt das durch „Zentrum-Distanz < 2051 UND Yaw-Arc".
  Der Biss wird aber byte-true schon bei **dist < 3000** committet und der Maggot steht
  waehrend des Bisses fest ⇒ **jeder Biss im Band 2051..3000 ist im Port ein garantierter
  Whiff**; der zusaetzliche Arc (±33.75° Biss, ±16.9° Heavy) verwirft auch Kontakt-Treffer,
  die das Original-Quadrat (±1000 lateral) noch fasst.
- **Symptom B (Clipping):** Der Rear-up-**PIN** (sub 15) laeuft im Original ueber einen
  **EIGENEN Opfer-Handler 0x8011c118** (registriert `0x800ac758[0x27]` @0x8011ea38):
  EIN Wurf-Clip, Root-Platzierung NUR Frames < 0x25, Spieler-Wandklemme in JEDEM Tick,
  Unpin erst nach der Rueckkehr der Choreo. Der Port routet 0x27 durch die generische
  **Zombie**-Opfermaschine: der 83-Frame-Wurf-Clip (Root-Auslenkung **4364 Einheiten**,
  selbst gemessen) **LOOPT als „Hold"**, wird **absolut und UNGEKLEMMT** platziert, und der
  Release kommt auf der **Maggot**-Timeline = mitten im Ausschwung. Rechnung an der
  ROOM11C0-SCA: 4364 ueberschreitet die Mittellinie der Sued-/West-Wand fuer jeden Pin,
  der < ~1.8k vor der Wand beginnt ⇒ `push_rect` wirft Leon zur **Fernseite** aus
  (hinter die Wand, unerreichbar). Zusaetzlich fehlt im Port der Original-Pass
  „**Maggot wird aus dem Spieler geschoben**" (aec4 @0x80116e40) — stattdessen schiebt der
  Port **Leon** mit bis ~1950/Frame.

---

## 1) Identifikation: welche Typen, welche Raeume

Sce_em_set-Sweep (Opcode 0x44, 20 B) ueber alle 240 `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`
(Walker = Opcode-Laengen aus `re15_port/tools/aot_sce_census.py`; 1009 Records; die bekannten
Daten-Regionen-Desyncs betreffen die Zaehlung nicht — Gegenprobe unten):

| Typ | Raeume (Anzahl em_set-Records) |
|---|---|
| **0x27** | **ROOM1141 x4** (sub00: Slot 1+2, je beh 0x30 und 0x10), **ROOM11C0 x4** (sub00: Slot 1+2, je beh 0x30 „script-frozen" und 0x10) |

Raw-Records ROOM11C0 (sub00, Datei-Offsets im SCD): Slot 1 `44 01 27 30 02 00 00 60 3c fb e0 b1 c0 ab …`,
Slot 2 `44 02 27 30 02 00 00 61 …`; dazu je ein beh-0x10-Paar. ROOM1141 analog (hp-Feld 0 ⇒
INIT-HP 180, Port `enemy_ai_common.c:8139` = HP-Tabelle @0x8011f034-Zeile).

Gegenprobe: Raw-Byte-Scan `44 [00-07] 27` ueber alle RDTs — nur ROOM1141 und ROOM11C0 haben
4 Treffer (alle uebrigen Raeume 1 Zufallsbyte-Treffer). Es gibt **keinen** weiteren
Affen-/Gorilla-Typ: 0x28 spawnt nirgends, 0x22/0x24 sind andere Familien (Audit
`RE15_ENEMY_AI_AUDIT.md`, dormant-Welle 2d6e49a2). Der ROOM11C0-„Gorilla-BOSS" und die
„Affen" sind derselbe Typ — **zwei gleichzeitige EM027** (deshalb Plural).

---

## 2) Symptom A — „treffen Leon so gut wie nie"

### 2.1 Original-Kette (STAGE1.BIN, selbst disassembliert)

**Commit:** CHASE A[3] committet den Biss bei `a804(0xbb8, 0x180)` = dist < **3000**, Arc ±384
(@0x80117a60-74), `+0x1dc==0` (@0x80117a88-90). Rear-up-Override auf sub 15 bei 2500/±256
(@0x80117ab4-b3c).

**Hit-Check (Biss B[5], Fenster {0x0c..0x0f} @0x8012146c):**
```
801183c0: addiu a0,s2,1612          ; a0 = Part-Pool + 0x64c = 0xAC*9+0x40 -> BONE 9 (Kiefer)
801183c4: addu  a1,s1,zero          ; a1 = lokaler Offset-Vektor (sp+16)
801183c8: ori   a2,zero,0x3e8       ; r = 1000
801183cc: jal   0x8001bff8
801183d0: addiu a3,a3,-95           ; a3 = 0x800aca88 = SPIELER-X/Z
```
**FUN_8001bff8** (EXE @0x8001bff8, selbst disassembliert): transformiert den Offset durch die
Bone-Matrix (`jal 0x80022da0` @0x8001c078) und prueft dann ein **achsenparalleles QUADRAT**:
```
8001c080: lhu v0,36(sp)  ; Welt-X des Angriffspunkts     8001c0a8: lhu v0,44(sp) ; Welt-Z
8001c084: lhu v1,0(s1)   ; Spieler-X                     8001c0ac: lhu v1,8(s1)  ; Spieler-Z
8001c088: subu/subu/andi ; (plX - ptX + r) & 0xffff
8001c094: sll/sra a0     ; a0 = 2r
8001c09c: slt v0,a0,v0   ; > 2r ? -> MISS                ; Achse 2 identisch, xori -> Rueckgabe
```
⇒ Treffer ⟺ |plX−Bone9X| ≤ 1000 **UND** |plZ−Bone9Z| ≤ 1000. **KEIN Winkel-Test, kein
Zentrums-Radius.** Heavy B[6] prueft dual `bff8(+0x448=Bone 6 / +0x6f8=Bone 10, r=0x320)`
@0x801186f0-714; Pin-Connect (Frame 0x0f) `bff8(+0x39c=Bone 5, r=0x320)` @0x8011ab24-78;
Finisher B[8] dual r=0x320 @0x801190dc-9100.

**Damage-Apply (Biss):** `hp -= 6` @0x80118460-6c, `+0x1dc=0x2d` @0x80118470-78, Se(6)
(a0=6 @0x80118454, jal @0x80118474), Stagger `aca58=2` @0x8011847c-84, `aca59=a780()+2`
@0x80118488-9c, hp<0 → `aca58=3` @0x801184a8-b8, `hit_react|=1` @0x801184c0-cc.
Heavy: `hp -= 12` @0x801187b4-c0; Finisher: `hp -= 600` @0x80119198-ac.

### 2.2 Port-Gegenstueck (`re15_port/engine/src/enemy_ai_common.c`)

Die Damage-Applies existieren und stimmen (−6 `:8308`, −12 `:8339`, −600 `:8468`; Stagger/
Knockdown/Devour verdrahtet `:8311`, `:8348-8349`, `:8471-8473`). **Die Divergenz ist der
Hit-Check:** alle vier Connects laufen ueber
`re15_dog_arc(e, pl, re15_body_contact_reach(e), ±384/±0xc0)` (`:8307`, `:8338`, `:8467`,
`:8515`) mit `re15_body_contact_reach = hit_radius_min + 450 + 1 = 1600+450+1 = **2051**`
(`:4010-4013`; Radius-Quelle `re15_damage.c:2220-2221` = Box @0x80121350
{0,-1440,0,1600,1440,1600}; Spieler-r 0x1c2 @PSX.EXE Datei 0x64694). Der Umbau ist als
Proxy dokumentiert (Audit wf_555f18eb Part B, Kommentar `:4002-4009`).

### 2.3 Die Rechnung, warum das „so gut wie nie" ergibt

1. **Whiff-Band 2051..3000 (PROVEN):** Trigger byte-true bei dist<3000 (`:8211` ==
   @0x80117a60-74), Connect erst <2051. Waehrend des Bisses steht der Maggot fest
   (B[5] `bf50(0,0)` `:8303` == @0x8011832c-40 Foot-Lock; die Clip-0x12-Rootkurve liegt an
   den Fensterframes 12-15 nur 293..421 von f0 — selbst gemessen, CDEMD0.EMS Blob 12
   dir[1]/dir[2], Parser validiert gegen `s_maggot_clip_len` `:8032`). Leon backpedalt im
   Kampf typischerweise genau in diesem Band ⇒ Biss um Biss whifft. Im Original deckt das
   Kiefer-Quadrat das Band: Bone 9 sitzt an der Front des 1600er-Koerpers, ±1000 dazu
   (exakte Bone-9-Auslage an den Fensterframes statisch nicht beziffert — OPEN, s. §4).
2. **Arc-Gate erfunden (PROVEN):** Das Original-bff8 hat keinen Winkeltest. Port-Heavy
   verlangt ±0xc0=±16.9°: laterale Toleranz bei Kontakt (2050) ≈ 595 — das Original fasst
   ±800 um Bone 6 UND ±800 um Bone 10, richtungsunabhaengig. Ein seitlich stehender/
   strafender Leon faellt im Port durch, im Original nicht.
3. **Pin frisst Bisse:** Bei frontalem Spieler in 2500/±256 wird der committete Biss zum
   Rear-up-PIN eskaliert (`:8218-8222` == @0x80117ab4-b3c) — der macht **keinen HP-Schaden**
   (weder Original-Fenster noch Port-Case-15 schreiben hp), sondern nur den Wurf (§3).
   Subjektiv: viele Angriffe, kein Schaden.
4. Heavy kann im Port treffen, WENN der Locator die Lunge traegt: Clip-0x13-Rootkurve
   deckt 2145 bis zum Move-Gate-Ende f20 (Fenster f21-24 bei 2267..2632; selbst gemessen).
   Ob `re15_maggot_footlock` diese Strecke im Port real erbringt: dynamisch offen (§4).

---

## 3) Symptom B — „Leon clippt in unerreichbare Bereiche"

### 3.1 Push-Asymmetrie: der Port schiebt LEON, das Original den MAGGOT

Original-Maggot-Root-Tail (selbst disassembliert):
```
80116e38: lui a1,0x800b / lw a1,-14460(a1)   ; a1 = MAGGOT (g_entity cur)
80116e40: jal 0x8002aec4                     ; Body-Push: pushee = a1
80116e44: addiu a0,s0,20                     ; a0 = 0x800aca40+20 = 0x800aca54 = SPIELER
80116e50: jal 0x8002b544                     ; vs. andere Gegner
80116e64-74: lw +0x78 / ori a2,4 / lhu a1,6(v0) / jal 0x8003b0a4  ; SCA-Klemme r=1600
80116e84: sh v0,470(v1)                      ; +0x1d6 = Wand-Kontakt
```
⇒ **Der MAGGOT wird aus dem Spieler geschoben** und sofort wand-geklemmt. Da die
Entitaeten-Schleife @0x8001ce04 VOR dem Spieler-FSM @0x8001ce0c laeuft (belegt im Port
`game_step_common.c:1288-1289` + hund-erster-treffer.md), findet der Spieler-seitige
b544 danach pen≈0 — **Leon wird vom Boss-Koerper im Original praktisch nie verschoben.**

Port: der 0x27-Branch in run_all (`enemy_ai_common.c:13198-13201`) ruft
`re15_enemy_body_push_tail` (`:12706-12716`) — die Schleife beginnt bei
`RE15_ACTOR_SLOT_PLAYER + 1`, **der aec4(&player, maggot)-Pass fehlt**. Stattdessen schiebt
`re15_body_push_player` (`game_step_common.c:1280`) **LEON** mit voller Penetration aus dem
1600er-Zylinder: Verschiebung `d·(2050−d)/(d+1)` ⇒ bis ~**1950 Einheiten in einem Frame**
(Maximum bei d≈100-300). Landet der Boss-Leap auf Leon, fliegt Leon — nicht der Boss.

### 3.2 Der PIN: falsche Opfermaschine, ungeklemmte 4364er-Platzierung, falsches Release-Timing

**Original:** cmd-5 (@0x80036834) dispatcht pro Greifer-TYP `0x800ac758[type]`
(Port-Zitat `enemy_ai_common.c:498`). STAGE1 registriert fuer 0x27 einen **eigenen** Handler:
```
8011ea2c: lui v0,0x8012 / addiu v0,v0,-16104   ; = 0x8011C118
8011ea34: lui at,0x800b
8011ea38: sw  v0,-14348(at)                    ; 0x800ac7f4 = 0x800ac758[0x27]
```
(cmd-6/Fresser analog: `0x8011c3d4` → `0x800ac8f4` @0x8011eab4-c8).

**FUN_8011c118** (selbst disassembliert):
- Variant-Dispatch @0x8011c128-140 (Tabelle 0x80121568[aca59]); Phasen-Dispatch
  @0x8011c168-8c (Tabelle 0x80100404[aca5a], nur Phasen < 8).
- Phase 0 @0x8011c194-224: **Opfer-CLIP 1** (`ori v0,0x1` / `sb v0,-13592(at)` = acae8
  @0x8011c1a0-ac), Frame 0 (@0x8011c1c8), CORE-0-SE (@0x8011c194/@0x8011c1e4),
  `aca3c |= 0xc0` @0x8011c1fc-204; **Variante ≠ 0 startet in Phase 2 mit Frame 0xc**
  (@0x8011c210-1c).
- Phase 2 @0x8011c268-2e4: Root-Platzierung `jal 0x8001ad68(Spieler, acbcc, acbd0)`
  @0x8011c294 **NUR solange Frame < 0x25** (`sltiu v0,v0,0x25` @0x8011c278);
  f314-Advance @0x8011c2b0; Clip-Ende → Phase 3 + **UNPIN**
  (`addiu v1,zero,-4097 / and / sw` @0x8011c2d0-dc = Spieler-Flag &= ~0x1000).
- Phasen 3/5: Leons **EIGENE** Get-up-Clips 0x10 bzw. 0xb aus SEINEN Baenken
  (acae8=0x10 @0x8011c2f4-fc; 0xb @0x8011c328-30; Baenke acad8/acbc0 @0x8011c34c-58).
- Und im Original klemmt der cmd-5-**Spieler-Tick** jede Platzierung im SELBEN Tick an die
  Waende (FUN_80031c44-Reihenfolge Platzierung → Push → SCA; vom Port selbst zitiert:
  `game_step_common.c:1734-1740` „walls win").

**Die Opferbank EM027** (CDEMD0.EMS Blob 12 @Datei 0x1be000, dir @+0x32bac; dir[5]-EDD
@+0x17330, dir[6]-Pool @+0x175a0, 15 Bones / KF-Groesse 80 — selbst geparst, EDD-Layout ==
`re15_emd_parse_animation`, Haupt-Bank-Gegenprobe == `s_maggot_clip_len` {78,20,…,52}):
**genau 2 Clips** — Clip 0 (70 f, netto +1573 vorwaerts) und **Clip 1 (83 f) = der WURF**:
Root-Offset schwenkt ueber Frames 10..35 bis (−4119,−1443) = **4364 Einheiten** (Peak f29/30)
und kehrt bis f41 auf ~550 zurueck. Das Original platziert davon nur f0..f36 (Fenster <0x25;
letzter platzierter Offset f36 = 2128) — **einmal**, wand-geklemmt, und released erst am
Leon-Clip-Ende.

**Port** (`enemy_ai_common.c`): 0x27 faellt in `re15_victim_clip_map` in den **ZOMBIE-Default**
(`:892-988`: intro=v*3, hold=v*3+1, release=+2, collapse=v+6). Folgen bei einer 2-Clip-Bank:
- Variante 0: **Hold = Clip 1 ⇒ der 83-f-Wurf LOOPT ENDLOS** (`:1415-1422`), jede
  Platzierung **absolut** (`:1429` → `re15_victim_place` `:1312` →
  `re15_clip_root_motion_abs` `:622-641`, Anker @Latch `:584-616`), **ungeklemmt** — die
  Wandklemme in `re15_victim_place` gilt NUR fuer Greifer-Typ 0x1A (`:1326`; der Kommentar
  `:1313-1325` dokumentiert woertlich dieselbe Fehlerklasse am ROOM1210-Greifer:
  „Leon hinter der Kollisionsgrenze … im normalen Raum nicht mehr bewegen").
- Release-Clip 2 und Collapse 6/7 sind **out of range** (`re15_clip_root_motion_abs :626`
  kehrt still zurueck; fc-Lookup `:1405` ⇒ 1). Variante 1 (von hinten): Clips 3/4/5 OOB ⇒
  gar keine Choreo (Leon friert) — auch das ist eine sichtbare Divergenz.
- **Release-Timing:** der Port unpinnt auf der **MAGGOT**-Timeline — Case 3 RIDE endet mit
  Maggot-Clip 0x1c (52 f, `s_maggot_clip_len[28]`), `:8535` → Case 4 `:8543`
  (`s_victim_phase = 4`). Beim Frame-0x0f-Connect (Rueckseite) steht Leons Wurf-Clip beim
  Maggot-Clip-Ende bei ≈ f36 — **nahe dem 4364er-Maximum**. Victim-State 3 spielt danach den
  OOB-„Release" (fc=1) und gibt Leon **an Ort und Stelle** frei.

### 3.3 ROOM11C0-Rechnung: setzt der Wurf/Push Leon ueber eine Wand?

SCA aus `shared_assets/PSX/STAGE1/ROOM11C0.RDT` (Zeiger @0x20 → Sektion @0x1410; Header
24 B, 59 Zellen à 12 B, selbst geparst):

| Wand | Zelle | Ausdehnung | Dicke | inflatiert (+450 je Seite) | Mittellinie hinter Face |
|---|---|---|---|---|---|
| West | [11]/[43] | x −32200..−27400, z −30800.. | 4800 | 5700 | 2850 |
| Sued | [24]/[44] | z −31100..−27000, x −32200.. | 4100 | 5000 | **2500** |
| Nord | [1]/[12] | z 17736..25536 | 7800 | 8700 | 4350 |
| Ost | [0]/[25] | x 12440..21640 | 9200 | 10100 | 5050 |

Innen: Kreise [23]/[42] (⌀3500), Boxen [2]/[7]/[10] (3500..15600), Diagonalen (Typ 2),
Band-1-Zellen (floor 0x13, fuer Band-0-Leon inaktiv), [30]/[48] u0=0x04 (nur gegner-solid),
[4]/[19]/[36] u0=0 (nicht solid).

- **Body-Push allein tunnelt NICHT:** max Ein-Frame-Verschiebung ≈ 1950 < 2500 (kleinste
  Mittellinien-Distanz, Sued). `push_rect` (`re15_collision.c:53-98`, byte-true
  FUN_8003bca8-Modell) wirft zur naechsten Face aus; die Klemme `:1287` laeuft nach dem Push.
  Der Push-Fehler (§3.1) aeussert sich als „der Boss schiebt Leon durch den Raum", nicht als
  Ein-Frame-Wanddurchbruch.
- **Der Pin-Wurf tunnelt:** 4364 > 2500/2850. Beispiel Sued-Wand (inflatierte Face
  z=−26550, Mittellinie −29050): jeder Pin-Anker mit z < −24686 (= **weniger als ~1.86k vor
  der Wand**) und Wurf-Yaw suedwaerts platziert Leon jenseits der Mittellinie
  (Anker−4364 < −29050) ⇒ `push_rect` min-axis wirft zur **FERNSEITE** aus (z ≈ −31568 =
  hinter der Wand; dort greift keine Zelle mehr — unerreichbar). West-Wand analog
  (Anker < ~1.51k vor der Face). Innen-Kreise (⌀3500: Querung braucht 4400 — der Wurf
  endet knapp innen ⇒ Auswurf je nach Naehe auf der **falschen Seite**) und die
  Innen-Boxen werden vom Schwenk zur jenseitigen Haelfte durchstossen ⇒ Leon steht hinter
  Autos/Sperren. Da die Platzierung im Port **nie** geklemmt wird, zaehlt allein die
  Release-Position — und die liegt beim Frame-0x0f-Connect nahe dem Maximum.
- Knockdown ist NICHT die Ursache: `kd_move` klemmt (`game_step_common.c:444-456`),
  ebenso beide `re15_body_push_player`-Stellen (`:1280→:1287`, `:1744→:1748`).

---

## 4) OFFEN (ehrlich benannt, mit dem jeweils naechsten Messweg)

1. **Bone-9-Vorwaertsauslage** an den Biss-Fensterframes (bestimmt die exakte
   Original-Reichweite): braucht Pose-Rechnung (12-bit-EMR) oder Savestate-RAM
   (`re15-savestate-ghidra`, Part-Pool +0x64c-Matrix live lesen).
2. Ob der Port-Footlock (`re15_maggot_footlock`) die Heavy-Lunge-Strecke (Rootkurve 2145
   bis f20) real erbringt — `re15-room-probe` mit lebender KI messen.
3. `0x8011c3d4` (cmd-6-Fresser-Handler 0x27) nicht disassembliert; dass Opfer-Clip 0
   (70 f, +1573) dessen Choreo ist, ist **HYPOTHESE**. Der Port-Finisher (−600) nutzt die
   generische Devour-Maschine — vor einem Fix RE'en.
4. Phasen-Tabelle @0x80100404 und Variant-Tabelle @0x80121568 nur ueber den Dispatch belegt,
   Eintraege nicht einzeln gedumpt (Phasen 4/6/7 des Handlers nicht gelesen).
5. ROOM1141 (Elza) nicht separat geprueft — gleiches Overlay (STAGE1), gleiche Mechanik
   unterstellt.
6. Dynamische Bestaetigung des Clip-Wegs (gdigrab, `re15-port-visual-verify`): Pin nahe der
   Suedwand provozieren, Release-Position messen — heute jenseits, nach Fix diesseits.

## 5) Fix-Vorschlag (byte-true, jede Konstante mit Adresse)

**A — Treffer:** Die vier Connects auf das Original-Modell umbauen: Welt-Position des
Angriffs-Bones (Biss Bone 9 = Pool+0x64c, r=0x3e8 @0x801183c0-cc; Heavy Bones 6+10 =
+0x448/+0x6f8, r=0x320 @0x801186f0-714; Pin Bone 5 = +0x39c, r=0x320 @0x8011ab24-78;
Finisher dual r=0x320 @0x801190dc-9100) als **Quadrat-Test** |dX|≤r ∧ |dZ|≤r gegen
Spieler-X/Z (FUN_8001bff8-Semantik @0x8001c080-c0c0), **ohne Arc**. Engine-seitige
Bone-Weltposition existiert als Muster (`re15_player_victim_bone_pos`,
`enemy_ai_common.c:995`). Kein Zahlen-Tuning am 2051-Proxy — das waere Raten.
Pin/Wache: „Biss-Commit bei dist 2600 ⇒ Treffer" (heute 0 — Zaehler auf den Damage-Apply,
nicht aufs Ergebnis; Regel `reai-v2-pin-prueft-zustand-nicht-weg`).

**B — Clipping (drei Teile):**
1. 0x27-Pin auf die Original-Opfermaschine 0x8011c118 umbauen: EIN Clip (1), Variante-1-
   Start bei Frame 0xc (@0x8011c210-1c), Platzierungs-Fenster `< 0x25` (@0x8011c278),
   Unpin am **Leon**-Clip-Ende (@0x8011c2d0-dc), danach Leons eigene Clips 0x10/0xb
   (@0x8011c2f4-fc/@0x8011c328-30). Damit verschwinden Loop, OOB-Clips und das
   Maggot-Timeline-Release zugleich.
2. Spieler-SCA-Klemme in JEDEM Pin-/Platzierungs-Tick (Original-Reihenfolge Platzierung →
   Push → Waende im cmd-5-Spieler-Tick, FUN_80031c44 — vom Port bereits zitiert
   `game_step_common.c:1734-1740`), nicht nur fuer Greifer 0x1A. Das 0x1A-Sondergate
   (`:1326`) geht darin auf.
3. Fehlenden `aec4(&player, maggot)`-Pass in den 0x27-run_all-Branch (Original
   @0x80116e40-44: pusher=0x800aca54, pushee=Maggot) — VOR der SCA-Klemme des Maggots
   (@0x80116e64-74-Reihenfolge), damit der Boss ausweicht statt Leon.
   ⚠️ Frame-Ordnung beachten: im Original laeuft die Entitaeten-Schleife VOR dem
   Spieler-FSM; der Port tickt Gegner am Frame-ENDE — der Pass gehoert in den
   Maggot-Branch (wie beim Zombie `:13065`), NICHT zusaetzlich in den Spieler-Pfad.
   Regressionsflaeche: Gegner-Positions-Pins (vgl. ada-kiste-Schiedsspruch §2).
