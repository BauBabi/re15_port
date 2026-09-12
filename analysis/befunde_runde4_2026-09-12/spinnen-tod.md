# BEFUND A — Gruppe „spinnen-tod": Todesanimation 2x + schwarze Dreiecke darueber

Nutzer 2026-09-12: „die Todesanimation der Spinne spielt irgendwie 2x ab, ausserdem gibt es
komische schwarze Dreiecke darueber."

Alle Disasm-Zitate frisch gezogen mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis … --bin EMS25.BIN`
(info/re2leon/COMMON/BIN/EMS25.BIN, Overlay laedt RAW @0x80100000); alle EMD-Koepfe selbst
per Python aus `re15_port/shared_assets/RE2/CDEMD0.EMS` (RE2-TOC gen/re2_ems_toc.inc) und
`re15_port/shared_assets/PSX/EMD/CDEMD0.EMS` (RE1.5, Blob-Walk wie re15_ems.c) gedumpt.

Beide Symptome haben je GENAU EINEN Mechanismus, beide sind gemessen und byte-belegt:

* **2x-Ablauf** = die Wrap-Naeherung des EDD-Frame-Flags: der Port schaltet DEATH→CORPSE erst
  am Clip-WRAP (Frame 206→0), das Original am markierten EDD-Frame (Schrot: Frame 12 von 206).
  Der Wrap NULLT den Frame, und CORPSE-Phase-0 spielt denselben Clip 12 danach KOMPLETT
  noch einmal durch (Port: 206+206 = 412 Frames; Original: 206 gesamt).
* **Schwarze Dreiecke** = die 6–9 Baby-Spinnen, die der Tod ausstoesst (@0x80104590-A4),
  werden vom WELLE-G-Hybrid mit der RE1.5-EM26-Geometrie gezeichnet — und RE1.5-EM26 ist
  KEINE Spinne, sondern der Feuer-Emitter-Anker von ROOM1090: **1 Mesh = 1 einziges Dreieck**
  (3 Verts, 1 Face), das das Original per Design VERSTECKT (@0x801165d0-e4). 6–9 nackte
  Dreiecke krabbeln ueber der sterbenden Adult.

Der heutige Mehr-Meshes-als-Bones-Zeichenpfad (Commit 149edbdb, main.c npc_zeichen_n) ist
fuer BEIDE Spinnen-Modelle GEMESSEN unschuldig — s. §3.

---

## 1. Der Doppelablauf — EIN Mechanismus: Wrap-Naeherung + CORPSE-Phase-0

### 1.1 Port-IST (enemy_ai_re2_spider.c)

Der Todes-Clip ist **Clip 12, Rate 7** (`re2s_clip(e, 0x0007000C)`), und er hat genau ZWEI
Setzer, beide einmalig pro Tod (Gate `sub_state_2 == 0`):

| Zeile | Setzer | Original |
|---|---|---|
| generisch (Handfeuer u.a.) | `re2s_death_row_generic` Modus 0 P0 (:2320) | @0x80103DC0-C8 |
| Sonderzeile 5,6,7,8,17 (**Schrot w8→7**, Magnum w7→5) | `re2s_death_row_special` Modus 0 P0 (:2129) | @0x80104578 |

Es gibt also KEINEN doppelten Clip-Setzer, kein doppeltes Death-Root-Betreten noetig.
Der zweite Durchlauf entsteht so (Kette am Code entlang):

1. `re2s_advance` = `re15_re2_advance_959c` (enemy_ai_re2_dog.c:197-207): 1 Frame/Tick,
   **am Wrap-Tick `anim_frame = 0` und return done** („sb zero,333 + done @0x80029B48-4C").
2. Beide Todeszeilen benutzen dieses `done` als Ersatz fuer das ORIGINAL-Frame-Flag
   (deklarierte Naeherung, Kommentar :2337-2339): der Uebergang nach CORPSE passiert
   im Port erst, wenn Clip 12 KOMPLETT durch ist und der Frame schon auf 0 steht.
3. `re2s_word(e, 7u)` (:2154/:2358) setzt Zustand 7 UND `sub_state_1/2/3 = 0`
   (re2s_word :156-162) → CORPSE startet in Phase 0.
4. CORPSE Phase 0 (`re2s_corpse` :2377-2382, Original @0x80104D10-3C) ruft **wieder
   `re2s_advance(512)` auf demselben, gerade auf Frame 0 gestellten Clip 12** — der
   Todes-Clip laeuft also ein ZWEITES Mal vollstaendig (206 Frames), erst dann kommt
   `sub_state_1 = 1`, der 30–61-Tick-Timer und der Zuck-Clip 13.

**Port: 206 (DEATH) + 206 (CORPSE) = 412 Frames Todesanimation. Original: 206 gesamt.**

### 1.2 Original — der Uebergang haengt am EDD-Frame-Flag, nicht am Clip-Ende

Generische Zeile, EMS25.BIN @0x80103E08-24 (selbst disassembliert):

```
80103e08: jal 0x8002959c          ; Anim-Advance laeuft IMMER
80103e0c: addiu a3,zero,512
80103e10: lw v0,376(s0)           ; +0x178 = Zeiger auf das EDD-Wort des AKTUELLEN Frames
80103e18: lw v0,0(v0)
80103e1c: lui v1,0x8              ; Maske 0x80000
80103e20: and v0,v0,v1
80103e24: beq v0,zero,0x80103ebc  ; Flag nicht gesetzt -> KEIN CORPSE-Uebergang
```

Sonderzeile (Schrot/Magnum), @0x801045B8-D4 — dieselbe Form mit Maske **0x40000**:

```
801045b8: jal 0x8002959c
801045c0: lw v0,376(s0)
801045c8: lw v0,0(v0)
801045cc: lui v1,0x4              ; Maske 0x40000
801045d0: and v0,v0,v1
801045d4: beq v0,zero,0x80104648
```

Auch der Modus-2-Abschluss der generischen Zeile ist FLAG-gegatet (@0x8010416C-80:
`lw v0,376(s0) / lw v0,0(v0) / lui v1,0x8 / and / beq → sonst sw 7,4(s0) @0x80104190`) —
der Port naehert auch ihn mit `re2s_advance`-done (:2333).

CORPSE dagegen ist WIRKLICH done-gegatet — @0x80104D18-20: `jal 0x8002959c` und
`beq v0,zero,…` prueft den RUECKGABEWERT. Der Port ist hier byte-true.

### 1.3 Die Messung: wo die Flags in Clip 12 wirklich sitzen

EDD des Haupt-Paars (dir[1] @0xC, 16 Clips, Laengen
{16,8,16,31,21,6,10,17,5,29,55,42,**206**,68,32,32}) aus dem RE2-EMD kind 0x25
(CDEMD0.EMS Sektor 1871, 0xD0C8 B), Clip 12 = 206 Frames, Frame-Woerter selbst gedumpt:

| Frames | Flag-Bits (>>12) | Bedeutung |
|---|---|---|
| 0–11 | 0x00 | keine |
| 12–179 | 0x40 (= Bit 0x40000) | **Sonderzeilen-Uebergang feuert ab Frame 12** |
| 180–205 | 0xC0 (= 0x40000\|0x80000) | **generischer Uebergang feuert ab Frame 180** |

(Clip 13 = 68 Frames, nur der letzte traegt 0xF0000 — der Zuck-Clip ist unbeteiligt.)

Damit ist das Original-Verhalten exakt: **Schrot-Tod = 12 Frames im DEATH-State, die
restlichen 194 Frames desselben Clips laufen im CORPSE-State weiter** (die Leiche sackt
zusammen, waehrend sie schon Leiche ist); generischer Tod = 180 + 26. In beiden Faellen
EIN Durchlauf. Der Port spielt stattdessen 206 im DEATH und dann — weil der Wrap den Frame
genullt hat und CORPSE-Phase-0 bis zum naechsten done wartet — 206 weitere im CORPSE.
Das ist woertlich „die Todesanimation spielt 2x ab".

Nebenbefund (byte-gleich, KEIN Bug): ein weiterer Treffer waehrend der Todesanimation
stempelt `state=3, sub_state_2=0` neu (re15_damage.c:1874-1904 bzw. re2-Stempel
`sw @0x80047288`) und startet P0 samt Clip 12 neu — das tut das Original identisch
(P0-Gate `+0x6 == 0` @0x80103DB8/@0x80104548). Nicht anfassen.

---

## 2. Die schwarzen Dreiecke — die Babys tragen das Feuer-Anker-Dreieck von RE1.5-EM26

### 2.1 Was der Spinnen-Tod zusaetzlich spawnt (Original, EMS25.BIN)

* **Gore-Partikel**: `FUN_801056DC(self,19,1)` @0x80104580 + Spray-Buendel `FUN_80104890`
  @0x80104588 (Sonderzeile P0); generische Zeile 1x `FUN_801056DC(self,0,0)` @0x80103DE4
  (nur wenn Zeile != 1). Keine Gift-Wolke im Todespfad.
* **BABY-SPINNEN (Typ 0x26)**: `FUN_80105D38(self, 0x2002, (rand&3)+6)` @0x80104590-A4 —
  **6 bis 9 Kinder** auf der Position der Adult (X/Y/Z-Kopie @0x80105E18-30), Kappe 18
  lebende (`sltiu 0x12` @0x80105DB0). Die Abschluss-Ausbrueche (+0x224) spawnen weitere
  1–4 (@0x80104470-78) bzw. 6–9 (@0x80104828-30).

Im Port sind die Gore-Emitter die deklarierte RE1.5-ESP-Blut-Naeherung am Aktor-Ursprung
(`re2s_gore` :1678-1683 → `re15_esp_fx_spawn_ex(room_bank, 0, 0, 0x1500, …)`) — das sind
rote ESP-QUADS aus der Raumbank, nicht die Dreiecke. Der Baby-Spawn ist byte-true portiert
(`re2s_spawn_babies` :1720-1756) und ERZEUGT wirklich 6–9 neue Typ-0x26-Aktoren.

### 2.2 Der Defekt: der Hybrid ersetzt das RE2-Baby-Modell durch das RE1.5-Feuer-Dreieck

Ladeweg der Babys (alles Port-IST, gelesen):

1. Render-Loop main.c:7763-7764 / Roster-Schleife :6027-6029 / Vorzug :6047:
   `pc_enemy_load_ex(0x26, allow_re2 = re2s_baby_spawned)` — fuer echte RE2-Babys also RE2-Zweig.
2. main.c:801 `allow_re2 && re15_ai_re2_for_type && re15_re2_owns_type(0x26)` (enemy_ai_re2_dog.c:74-97
   fuehrt 0x26) → `pc_enemy_load_re2(0x26)`: RE2-Bank **EM026 = 1 Mesh / 1 Bone / 3 Clips**
   + eigene Spinnen-TIM (TOC kind 0x26: EMD Sektor 1921/0x1A14, TIM Sektor 1904/0x8220 —
   selbst gedumpt: MD1 nObj=2 → 1 Mesh; EMR-Kopf {16,20,**1**,20} → 1 Bone; bester
   EDD = Paar 3, Clips {14,36,20}).
3. main.c:846 `pc_enemy_hybrid_re15_models(0x26, eb)` laeuft **BEDINGUNGSLOS** („der Hybrid
   laeuft BEDINGUNGSLOS", :840-845) → `re2_hybrid_apply` (re2_ems.c:322-375) mit
   `k_perm_ident, n = 1` (re2_ems.c:178) — alle Gates passen (1==1) → **`eb->md1 = *md15`**:
   die RE2-Spinnengeometrie wird durch das RE1.5-EM26-MD1 ersetzt, und die RE1.5-EM26-TIM
   ueberschreibt die RE2-Spinnen-Textur (main.c:626-629).
4. **RE1.5-EM26 ist aber keine Spinne.** Es ist der Feuer-Emitter-Anker von ROOM1090
   (Dispatch 0x80072bac[0x26] = 0x80116288, Registrierung @0x8011E8F4/@0x8011E8FC):
   selbst gedumpt **1 Mesh / 1 Bone** (MD1 nObj=2, EMR {16,20,1,20}), laut dem bereits
   RE'ten Zeichenmasken-Block main.c:8184-8199 „**EM26: 1 Bone, 3 Verts, 1 Face**" — ein
   einziges Dreieck, das der RE1.5-INIT absichtlich unsichtbar schaltet
   (`lw a0,392(v0) / and mit -2 / sw` @0x801165d0-e4 → Port-Feld `no_draw`).
5. Das RE2-Baby-Brain (`re2sb_init` :2503-2547) setzt `no_draw` NIE — das Dreieck ist
   sichtbar, posiert an der Wurzel, krabbelt mit dem Baby-Automaten umher. **6–9 solcher
   Dreiecke direkt ueber/auf der sterbenden Adult = exakt der Nutzer-Befund.**

Die Weiche existiert nur fuers BRAIN und den Schadensweg (re15_re2spider_baby_owns,
enemy_ai_re2_spider.c:2859-2862) — beim MODELL fehlt sie: der Hybrid behandelt 0x26 wie
einen Gegner mit RE1.5-Gegenstueck, den es nicht gibt (dieselbe Situation wie Alligator
0x23 und Endkampf-Birkin 0x36, die deshalb beide „REIN, ohne Hybrid" laufen —
main.c:788/905-914).

### 2.3 Warum die Dreiecke SCHWARZ/untexturiert wirken

Das Feuer-Anker-Dreieck war nie zum Zeichnen gedacht (im Original per Part-Bit versteckt,
s.o.) — seine UV/Face-Daten zeigen in die RE1.5-EM26-TIM (Flammen-Blatt), die zudem in
Schritt 3 die korrekte RE2-Spinnen-TIM im Slot ERSETZT hat. Gezeichnet wird also ein
Design-Platzhalter mit Anker-UVs: dunkle, „komische" Dreiecke.

---

## 3. Gegenprobe: der heutige Mehr-Meshes-als-Bones-Pfad ist unschuldig

Commit 149edbdb (heute) zeichnet ueberzaehlige Meshes an der Wurzelpose — aber nur
`if (!npc_remap && npc_md1->mesh_count > npc_bones)` (main.c:8221). GEMESSEN:

| Modell | Meshes | Bones | Quelle |
|---|---|---|---|
| RE2 EM025 (Adult) | 20 | 20 | CDEMD0.EMS(RE2) Sektor 1871: MD1 nObj=40, EMR {128,228,**20**,104} |
| RE1.5 EM25 | 20 | 20 | CDEMD0.EMS(RE1.5) Blob 10 @0x196000: MD1 nObj=40, EMR {128,228,**20**,104} |
| RE2 EM026 (Baby) | 1 | 1 | Sektor 1921: MD1 nObj=2, EMR {16,20,**1**,20} |
| RE1.5 EM26 (Feuer-Anker) | 1 | 1 | Blob 11 @0x1b5800: MD1 nObj=2, EMR {16,20,**1**,20} |

Mesh-Zahl == Bone-Zahl in allen vier Faellen, und beide Hybrid-Baenke tragen ausserdem
`remap_ok` (npc_remap != NULL, main.c:7973-7976) → `npc_zeichen_n == npc_bones`, der neue
Pfad ist fuer 0x25/0x26 inert. Die Dreiecke kommen NICHT von heute — die Babys waren als
Dreiecke schon immer da, nur hat sie vor dem Schrot-/Treffer-Fix von heute kaum jemand
erlebt, weil die Adult vorher praktisch nicht totzukriegen war („Spinnen scheinen immun",
re15_damage.c:1476).

---

## 4. PLAN — je Ursache die exakte Aenderung

### 4.1 Doppelablauf: Frame-Flag statt Wrap (enemy_ai_re2_spider.c)

Der Port hat alles an Bord: die Bank fuehrt die ECHTEN RE2-EDD-Frame-Woerter (der Hybrid
tauscht nur MD1/TIM/Bindlaengen, `eb->anim` bleibt das RE2-EDD), und der Hund hat den
Leser bereits vorgemacht (`re2d_frame_word`, enemy_ai_re2_dog.c:214-224).

1. **Helfer** `re2s_frame_word(e)` einziehen — woertlich wie `re2d_frame_word`
   (`re15_enemy_find(e->type)` ist im Modul schon eingebunden, :88):
   `A->frames[clip.first_frame + anim_frame]`.
2. **Generische Zeile Modus 0** (:2334-2337): aus
   `int wrapped = re2s_advance(e, 512); if (!wrapped) return;` wird
   `(void)re2s_advance(e, 512); if (!(re2s_frame_word(e) & 0x80000u)) return;`
   — Beleg @0x80103E08-24 (`lui v1,0x8`), Flag ab Frame 180/206 (§1.3).
3. **Sonderzeile Modus 0** (:2140): dieselbe Form mit Maske **0x40000u**
   — Beleg @0x801045B8-D4 (`lui v1,0x4`), Flag ab Frame 12/206.
4. **Generische Zeile Modus-2-Abschluss** (:2333-2335): ebenfalls Maske 0x80000u
   — Beleg @0x8010416C-80 (identische lw 376/lw 0/lui 0x8-Leiter).
5. CORPSE (:2379), Sonderzeile-Modus-1-Advance und alle uebrigen `re2s_advance`-done-Gates
   UNVERAENDERT lassen — CORPSE ist im Original returnwert-gegatet (@0x80104D18-20 `jal
   0x8002959c / beq v0,zero`), genau wie portiert.
6. Wirkung: DEATH endet am markierten Frame (12 bzw. 180), CORPSE-Phase-0 spielt NUR den
   Rest des Clips (194 bzw. 26 Frames) zu Ende → EIN Durchlauf, wie das Original.
   Danach wie gehabt Timer + Zuck-Clip 13.
7. **Pin**: unit-Test (Muster test_adult_spider_ai.c) mit geladener RE2-Bank: Schrot-Tod
   (Zeile 7) → Zustand wird 7, waehrend `anim_frame` im Fenster [12, 205] liegt (nicht 0),
   und `motion` bleibt 12 bis zum CORPSE-done; Gesamtticks im Clip 12 ueber DEATH+CORPSE
   == 206, nicht 412.

### 4.2 Schwarze Dreiecke: kein Hybrid fuer Typ 0x26 (platform/pc/main.c)

RE1.5 hat KEINE Baby-Spinnen-Geometrie — der Tausch ist ein Kategorienfehler. Dieselbe
Regel wie Alligator 0x23 / Endkampf-Birkin 0x36 („REIN, ohne Hybrid"):

1. In `pc_enemy_load_ex` (main.c:846) den Hybrid fuer 0x26 auslassen:
   `if (type != 0x26u) pc_enemy_hybrid_re15_models(type, eb);`
   mit Beleg-Kommentar (RE1.5-EM26 = Feuer-Anker-Dreieck 1 Mesh/3 Verts/1 Face, im
   Original versteckt @0x801165d0-e4; RE2-EM026 = echtes Baby-Modell mit eigener TIM,
   TOC Sektor 1921/1904). Die RE2-TIM ist dann bereits im Slot (pc_enemy_load_re2 :548-552)
   und wird nicht mehr ueberschrieben.
2. Zusaetzlich `case 0x26` aus `re2_hybrid_perm` (re2_ems.c:178) entfernen, damit kein
   anderer Aufrufer den Tausch wieder einfuehrt (kein Test pinnt die 0x26-Permutation —
   geprueft: grep ueber re15_port/tests, 0 Treffer). `re2_hybrid_apply` liefert dann -2
   und der dokumentierte Fallback „reine RE2-Bank bleibt" greift auch defensiv.
3. Die RDT-gesetzten 0x26er (ROOM1090-Feuer) sind unberuehrt: sie laden mit
   `allow_re2 = 0` (re2s_baby_spawned == 0) weiter die RE1.5-Bank und bleiben per
   `no_draw` unsichtbar (main.c:8199).
4. **Verifikation**: `RE15_ENEMY_DBG=1` → enemy_dbg.log muss fuer die Babys
   „RE2 EM026: 1 Meshes, 1 Bones, 3 Clips" OHNE nachfolgende Hybrid-Zeile zeigen;
   visuell nach Skill re15-port-visual-verify (gdigrab, echter Flow): Schrot-Tod der
   Adult in ROOM2050/2090 → kleine texturierte Spinnen statt schwarzer Dreiecke.

### 4.3 Bewusst NICHT anfassen

* Der P0-Retrigger bei erneutem Treffer in den Tod (byte-gleich mit Original, §1.3).
* `re2s_gore`-ESP-Naeherung (deklariert; Quads, nicht die gemeldeten Dreiecke) und der
  Bone-19-/Spray-Kanal — eigener, groesserer Baustein (Mesh-Teil-Matrizen +0x198).
* Der Mehr-Meshes-als-Bones-Pfad von heute (fuer 0x25/0x26 gemessen inert, §3).
