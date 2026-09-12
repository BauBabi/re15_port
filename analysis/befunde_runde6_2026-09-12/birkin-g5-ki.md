# G5-Endkampf-KI (RE2 EM036): VOLLZENSUS des Overlays + Umsetzungs-Spez (2026-09-12, Runde 6)

Nutzer: „Bei Birkin fehlt immer noch Alles — Animationen, Tentakel die ihn quasi nach
vorne ziehen und schlagen, Bewegung — einfach alles von Resident Evil 2."

Der Port fährt heute die RE1.5-0x30-Wurzel (`enemy_ai_common.c re15_birkin_ai_tick`,
Registrierung :13646, BKMAP main.c:761-771) auf die RE2-EM036-Clips. Dieses Dossier
ist der komplette Zensus der ECHTEN RE2-G5-KI als Grundlage für ein eigenes Modul
`enemy_ai_boss_g5.c` (Muster: `enemy_ai_boss_gator.c`).

## 0. Das G5-Overlay — welcher BIN?

**Es gibt KEINEN Disc-BIN.** `info/re2leon/COMMON/BIN/` enthält kein EM36-Overlay
(nur EMZ0/EMS25/EMS26/EM23_OVL/…, selbst gelistet). Das G5-KI-Overlay liegt
ausschließlich im Gegnerarchiv **CDEMD0.EMS** (Disc: `info/re2leon/PL0/PLD/CDEMD0.EMS`,
Kopie `shared_assets/RE2/CDEMD0.EMS`), TOC in der RE2-Leon-EXE @0x8009ADF4
(Datei-Offset 0x8B5F4, `re15_port/tools/re2_ems_cut.py:13/45`):

| TOC-Eintrag | EMS-Offset | Größe | md5 | Schnitt |
|---|---|---|---|---|
| 0x36 k=1 (KI, **@0x80100000 gelinkt**) | **0x006C0000** (Sektor 3456) | 23476 B | b57f8315dc8a | `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN` |
| 0x36 k=0 (derselbe Code @0x8010D000) | 0x006BA000 | 23476 B | 1fb659e34538 | `..._ai0.BIN` |
| 0x36 k=3 (EMD) | 0x006E6800 | 112340 B | 3a49b091f8ad | `..._emd.EMD` |
| 0x37 k=1 (Tentakel-KI) | 0x00708000 | 23024 B | 689abac9db42 | `..._EM37_ai1.BIN` |

Alle 0x8010xxxx-Adressen unten = ai1.BIN, Datei-Offset = Adresse − 0x80100000.
Disasm: `RE_OVERLAY_DIR=build/extracted/re2_ems python
.claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> --bin CDEMD0_EM36_ai1.BIN`.

## 1. Dispatch-Architektur — ALLE Tabellen (Vollzensus)

Overlay-Kopf: Wort0=0x1A, **+0x04: 16 Byte = 4×4 Tentakel-Reihenfolgen**
{0,2,1,3 / 1,2,3,0 / 3,0,2,1 / 3,1,2,0} (Bytes @0x80100004-13; Verbraucher lwl/lwr
@0x80100d38-78), +0x14: 27 Zeiger [T0..T26], +0x84: 14 Zeiger [U0..U13].

**Per-Frame-Main = 0x801000BC** (erste Funktion nach den Tabellen; EXE-Aufrufkonvention
wie EMZ0 — EXE-seitige Callsite nicht verfolgt, OFFEN). Ablauf:
1. Freeze-Gate: `*(0x800CFBDC) & 0x20000000` → alles überspringen (@0x801000cc-e4).
2. +0x1D3 &0x7F Countdown (@0x801000ec-100); Flinch-Zerfall: bei (+0x226&1) zählt
   +0x221 15→0, dann +0x222−1, +0x221=15; +0x222==0 → +0x226&=~1 (@0x80100104-15c).
3. **Routine-Dispatch: `Tabelle_0x801055CC[entity+0x04]`** (@0x80100160-84), Argumente
   (entity, *(+0x108)=EMR-Pool, *(+0x17C)=EDD):
   `[0]=0x801003CC Konstruktor · [1]=0x80100784 AKTIV · [2]=0x801025BC TREFFER ·
   [3]=0x80102BBC TOD · [4]=0x80103834 Script-Hook · [7]=0x80103878 Stub`.
4. +0x229 Devour-Cooldown −1 (@0x80100188-98).
5. wenn !(+0x226&8): alle 16 Frames Augen-Ziel-Neuwurf `0x8010503C(0, 15−rng%15,
   15−rng%15)` (@0x801001b0-230), bei Frame&0xF==7 dito Kanal 1 (@0x80100234-2b0);
   je Frame 2× Augen-Wanderer `0x80105064(e,0/1)` (@0x801002bc-cc), Schüttel-Treiber
   `0x80104EB4` (@0x801002d0: +0x225-Countdown → 0x80039514(3,100,0)+0x800395b8(20,
   100,0,3) @0x80104ee8-f1c; |Speed +0x144|≥121 → (3,150,0)/(30,150,0,3) @0x80104f20-5c),
   Ooze-Emitter `0x80105388` (nur (+0x226&6)==6, Partikel an Parts 0/4/5/6 alle 8
   Frames, @0x801053a0-…), Frame-Flag-SE `0x80016028` (EM36-EDD trägt KEINE Flags —
   alle 1360 Frame-Wörter >>12==0, selbst geparst → hier stumm),
   **Blob-Maschine `0x80103BB4`** (@0x801002f0, §4) mit Morph-Anwendung
   `0x8004BF90(e,&part2,part2+0x8,part2+0x88)` (@0x80103bf8),
   **Kopf-Tracking**: `0x80017FDC(e, yaw, &delta)` → delta auf ±2048 gewickelt →
   part1-Lokalwinkel +278 += delta, Matrix `0x8008E1F4(parts+276, parts+196)`,
   danach Winkel-Var wieder −delta (@0x801002f8-370) — **der Kopf (Bone 1) verfolgt
   den Spieler jeden Frame**, die Entity-Yaw NICHT (einzige Yaw-Schreiber: 0
   @0x8010044c, Script-Hook @0x80103a24),
   +0x0D=255 (@0x80100374-78), Part-Weltpositionen `0x80035408` (@0x8010036c),
   Kollisionsanmeldung `0x80034D0C(*(0x800CFE14), e)` (@0x80100384).

**AKTIV (r0=1) 0x80100784**: bei (+0x226&6)==4 → +0x04=3 (TOD, @0x801007ec-fc);
sonst Varianten-Dispatch `(+0x10E&0x3F)` → `0x80105648[0]=0x801007C4` (@0x8010078c-a4;
nur Variante 0 existiert), dann ZWEI Sub-Dispatches pro Frame auf **+0x05**:
Phase A (Entscheiden) `0x8010564C[sub]`, Phase B (Ausführen) `0x80105660[sub]`
(@0x80100800-48) — die drei Tabellen liegen lückenlos gepackt (Zombie-Falle!):

| sub (+0x05) | A @0x8010564C | B @0x80105660 | Bedeutung |
|---|---|---|---|
| 0 | 0x80100868 | 0x80100960 | IDLE + Entscheiden + Devour-Check |
| 1 | 0x80100D08 (Stub) | 0x80100D10 | **TENTAKEL-SCHLAG + ZUG nach vorn** |
| 2 | 0x80101108 (Stub) | 0x80101110 → `0x80100014[ph]` [T0..T14] | INTRO-Choreo |
| 3 | 0x80101808 (Stub) | 0x80101810 | RÜCKZUGS-KRIECHEN (Zähler-Rampe) |
| 4 | 0x80101AA0 (Stub) | 0x80101AA8 | DEVOUR (Fress-Kill) |

**TOD (r0=3) 0x80102BBC→0x80102BDC**: Phasen-Dispatch +0x06∈[0..5] →
`0x80100054[ph]` = Header-Tabelle [T16..T21] (@0x80102c04-2c).
**Script-Hook (r0=4) 0x80103834**: +0x05 → `0x801056E8` {[0]=0x801038BC Stub,
[1]=0x80103908}; dieselben Handler hängen als EXE-Callbacks in
`0x800CE300[slot]=0x80103880` / `0x800CE400[slot]=0x801038CC` (@0x801005dc/0x801005f8)
— Event-/Cutscene-Kanal, im RE1.5-Port nicht benötigt.

**Blob-Maschine** (eigener FSM-Kanal **+0x218**, Phase **+0x219**): Dispatch
`0x80105ABC[+0x218]` (@0x80103bc8-e4): `[0]=0x80103C18 Puls+Biss-Trigger ·
[1]=0x80103EFC Massen-BISS (ph→0x8010006C=[T22..T26]) · [2]=0x80104440 Zitter
(kein Setzer — toter Code) · [3]=0x801044FC Vorkampf-Puls (ph→0x80100084=[U0..U7]) ·
[4]=0x80104848 Devour-Blob · [5]=0x80104B94 Todes-Blob (ph→0x801000A4=[U8..U13])`.

**Tentakel-Kommandos**: die 4 Typ-0x37-Entities stehen in `0x800CFE20/24/28/2C`;
`0x80104E9C(slot,wort)` schreibt das Wort in deren **+0x04** (Routine-Wort!)
(@0x80104e9c-b0), `0x80104E5C(wort)` an alle vier (@0x80104e5c-98). Verwendete
Wörter: 0x301/0xD01 (Angriff, Mustertabelle @0x80105674: 8 Muster × 2), 0x601,
0x901, 0xA01, 0x40A01, 0xB01, 0x70B01, 0x90B01, 0xE01 (Tod); im Intro zusätzlich
direkte +0x06-Schreiber (1/6/7/8/9). +0x228 = Tentakel-Masken-Byte (Kandidaten-Scan
liest Bit i UND Bit i der Nachbarmaske, Schleife z.B. @0x801009e8-a40).

## 2. Kampf-Zustandsmaschine — State→Clip→Frames→SE→Bewegung→Damage

Clip-Wort +0x14C = `flags<<16 | clip`; **+0x14E-Flag = Überblend-Stärke** (FUN_80029614:
IR0-Lerp `a3*flag/4096`; 0x1F ≈ harter Schnitt, 0x07 = weicher Crossfade).
`0x8002959C(e,pool,edd,128)` = Anim-Schritt (setzt +0x178 auf die Frame-Zeile,
FUN_80029614 schreibt Root-Translation in part0+0x2C/30/34, **Rückgabe 1 = Clip zu
Ende**). `0x80015E7C` = Root-Motion-Delta (kf-Feld +6/+8/+10, akkumulierte Spur,
differenziert) → +0x144/146/148; `0x800152C8` rotiert (vx,vz) um Yaw und addiert auf
X/Z. SE = `0x8005BD6C(id, e)` (ENEMSE; ROOM7040-Spawns tragen sound-id **0x27** →
Paar-Zeile **25** {0x27,0x00} @PSX.EXE-Datei 0x97C32, flag2000=0 — Mechanik wie
gator-se-korrektur.md §1).

**Root-Motion-Spuren (selbst aus dem EMD geparst, EDD @0x4AB0/Pool @0x6040):**

| Clip | Frames | rootY min..max | akkX gesamt | max Schritt | Nutzung |
|---|---|---|---|---|---|
| 0 | 100 | −4534..−4392 | 0 | 0 | Idle-Wippen |
| 1 | 180 | −4536..−3191 | **+7014** | 190 | Intro-Kriechen 1 |
| 2 | 150 | −4534..−4392 | 0 | 0 | Rückzug/Recover-Kriechen |
| 3 | 100 | −3259..−3200 | 0 | 5 | Intro-Aufrichten |
| 4 | 180 | −4534..−3105 | **+3946** | 61 | Intro-Kriechen 2 |
| 5 | 150 | −4534..−4159 | **+2700** | 78 | **Tentakel-Zug** (×1,5 = +4050) |
| 6 | 100 | −4517..−4385 | 0 | 0 | Tod: Einbruch |
| 7 | 50 | −4507..−4391 | 0 | 0 | Tod: Zucken |
| 8 | 50 | −4520..−4391 | 0 (±48) | 23/−48 | **FLINCH/Stagger** |
| 9 | 150 | −4504..−4313 | 0 | 82 | **DEVOUR** |
| 10 | 150 | −4518..−4391 | 0 | 0 | Todes-Kollaps (Setzer @0x801034C8!) |

**Orientierung:** Yaw bleibt den ganzen Kampf 0 (@0x8010044c einziger Kampf-Schreiber)
→ Vorwärts = **+X**, der Spieler steht auf der +X-Seite. Belege: Blob-Biss-Lunge
`X += speed>>3` bis Kappe **12000** (@0x80104060-94), Devour-Gate `X ≥ 9001`
(@0x801008a0-a8), Intro kriecht +7014/+3946 (Clips 1/4). ⇒ das „Advance"-Kriechen
`X −= Zähler` bis 4000 (@0x80101934-4c) ist der **RÜCKZUG**.

### sub0 — IDLE/ENTSCHEIDEN (A 0x80100868, B 0x80100960)

* **Phase A (Devour-Check aus dem Stand):** Arc `0x80015614(e,SpielerX@0x800CFC30,
  SpielerZ@0x800CFC38,128)==0` (±11,25°) UND X≥9001 (@0x801008a0) UND Dist
  +0x1F0<6000 (@0x801008ac) UND Spieler nicht gegriffen (0x800CFDCB==0 @0x801008c4)
  UND 0x800CFD4E≤0 UND !(+0x226&4) → **player_damage(500,0)** @0x80100904,
  Routine-Wort 0x401 (→sub4) @0x80100908, 0x800CFDCB|=0x80 @0x8010091c,
  +0x218=4 @0x80100928, 0x800CFB74|=0x100 @0x80100944.
* **B ph0** @0x801009a4: Clip-Wort 0x1F0000 (Clip 0), ph=1, Timer +0x158 =
  120+rng&0x1F @0x801009c4; wenn X≥8000 (@0x801009c8) ODER Dist≥11001 (@0x801009dc):
  Tentakel-Scan — **alle 4 frei → sofort Wort 257 (sub1)** @0x80100c44/0x80100ce0,
  sonst +0x16B=1, Timer=10 (@0x80100a48-50).
* **B ph1** @0x80100a8c: Anim-Schritt; bei Clip-Ende rng&3==0 → **SE 9** (Grollen)
  @0x80100ab8. Alle 16 Ticks (Timer&0xF==0, solange !(+0x16B&1)): Tentakel-Kandidat
  wählen, Dist<7000 → Muster +0x16A=0 (@0x80100b4c-54), Kommando
  `0x80105674[Muster*8 + rng-Bit*4]` (nur 0x301/0xD01) an Zufalls-Kandidaten
  @0x80100bc0-c8 — **die Tentakel schlagen den Spieler, während G5 steht.**
  Timer→0 (@0x80100bcc-e8): +0x16B|=1; sind noch Kandidaten frei → warten; sonst
  **Entscheidung** @0x80100c44-ce4:
  Wort=769 (sub3 Rückzug); (rng&3)≠0 && X<10000 → 257 (sub1) @0x80100c70;
  Dist<10001 → 257 @0x80100c88; X≥9001 && (rng&1)==0 → 769 @0x80100cb4;
  X<7000 && (rng&1)==0 → 257 @0x80100ce4.

### sub1 — TENTAKEL-SCHLAG + ZUG („Tentakel ziehen ihn nach vorne und schlagen")

B 0x80100D10; kopiert die 4×4-Reihen-Tabelle vom Header nach sp (@0x80100d30-78);
setzt +0x226|=2 (busy, @0x80100d7c-88). Reihe = +0x16B = (rng>>(rng&3))&3 @0x80100e0c-14.

| ph | Aktion | Beleg |
|---|---|---|
| 0 | Clip-Wort 0x1F0005 (**Clip 5**), Broadcast 0xB01 an ALLE Tentakel, **SE 10**, ph=1 | @0x80100dd8/0x80100de0/0x80100dec |
| 1 | Frame 0/7/10/15: Tentakel Reihe[0..3] ← 0xB01 (Schlag-Kaskade); Frame 55: Reihe[0]+[2] ← 0x70B01 + **SE 9**; Frame 61: Reihe[1]+[3] ← 0x70B01; Frame 90: **SE 11**; **jeden Frame Root-Motion (0x80015E7C) und +0x144 ×1,5** (`v0=(s16)v1>>1; v1+=v0` @0x80100fa8-c0) **→ +4050 über 150 F**, Anwendung 0x800152C8 nur solange **X<12000** (@0x80100fd0-e4); Clip-Ende → ph=2 (ph+=Anim-Ergebnis @0x80100fc4-d8) | @0x80100e18-fe8 |
| 2 | Clip-Wort 0x1F0002 (Clip 2), **SE 10**, ph=3 | @0x80101000-0c |
| 3 | Frame 25: Reihe[0]+[2] ← 0x90B01 + **SE 9** @0x80101070-98; Frame 15: Reihe[1]+[3] ← 0x90B01 @0x801010c4-e0; **Frame 85: Wort=1 (→sub0), +0x226&=~2** | @0x8010103c-44 |

### sub3 — RÜCKZUGS-KRIECHEN (B 0x80101810)

ph0 @0x8010185c: Clip-Wort 0x1F0002 (Clip 2), **SE 11**, ph=1, +0x158=0, +0x16A=0,
+0x15A=0. ph1 @0x801018a8: Dreiecks-Rampe +0x158: +1/Tick bis **50** (→ **SE 11**
@0x801018e4), dann −1/Tick bis 0 (→ **SE 10** @0x80101920, +0x16A=2 = stehen);
+0x144 = Zähler (@0x80101930, nur fürs Schütteln); **Bewegung: `if (X>4000)
X −= Zähler`** @0x80101934-4c → **exakt 2500 Einheiten rückwärts** (Σ1..50 + Σ49..0),
Kappe 4000. Clip-2-Ende → Wort=1 (sub0), +0x226&=~2 @0x80101958-74. Alle 32 Frames
Tentakel-Kommando wie sub0 (Dist<7000 → Muster 0) @0x80101978-a6c.

### sub4 — DEVOUR (B 0x80101AA8) — der Fress-Kill

| ph | Aktion | Beleg |
|---|---|---|
| 0 | Clip-Wort **0x70009** (Clip 9, weich); **Victim-Kanal: `0x80015B94(Spieler@0x800CFE4C, …)`** (Leon spielt die 150-F-Devour-Anim aus EDD-Paar 3, birkin-em36 §1.1 dir[5]/[6]); Grab-Globals: 0x800CFDAC=e, 0x800CFBFC=6, 0x800CFD80/84=+0x188/+0x18C, 0x800CFDCB|=0x80, Spieler-Flags|=0x1002, Spieler-Yaw 0x800CFD52 = eigene Yaw+2048; +0x1D3|=0x80, e-Flags|=0x1000; **SE 6** | @0x80101b14-bb8 |
| 1 | Frame 10: **SE 7**; Frames 16..29: Blut an Part-4-Weltmatrix (parts+0x2F8) via 0x8001BF10, Offsets ±256; je Frame Victim-Step `0x80015CB8` + Anim-Schritt; Clip-Ende → ph=2 | @0x80101bc0-c98 |
| 2 | Clip-Wort **0x70002** (Clip 2), **SE 10**, ph=3, Timer +0x15A=150 | @0x80101ca0-c4 |
| 3 | Timer→0 UND Morph-Gewicht[0] (part2+0x88,+12) < 6000 → +0x218=0 (Blob zurück auf Puls). Kein Routine-Exit — der Spieler ist tot (500 Schaden). | @0x80101ccc-d24 |

### r0=2 — TREFFER-REAKTION (0x801025BC; +0x05 = WAFFEN-ID des Treffers)

* Wund-Spray: Kamera-Yaw 0x800CFC6E → Richtungsprojektion; Spray-Punkt
  x = X+3183 (Massen-Front!), y aus **+0x1D2 % 3** ∈ {−700, −1400, −2000}
  (@0x801026dc-740 — die „+0x1D2-Zonen" sind drei WUND-HÖHEN auf der Masse),
  z geklemmt auf **[−26000, −20500]** (Korridor!) @0x80102758-84; Frontalband
  z ∈ [−23999, −22500] → Zusatz-Spray an +0xA4 + **SE 8** (@0x8010285c-944).
* Waffen-Extra-Effekte: sub 9/17 → 0x8010221C, 10 → 0x80101D9C, 11 → 0x80101FF0,
  14 → 0x801022D0 (@0x80102948-ac) — alles Partikel-Anker an der Blob-Weltmatrix.
* **Flinch-System:** Tabelle @0x801056B4 (Byte je Waffen-Id):
  `[0..3]=5, [4]=14, [5]=20, [6]=14, [7]=20, [8..10]=14, [11..12]=5, [13]=20,
  [14..15]=1, [16]=20, [17]=1`; Wert ≥ 11 → +0x225=7 (Schütteln) @0x801029b0-d0.
  Busy (+0x226&2): Wort aus **+0x1FC restaurieren** (Backup!), weiterlaufen
  (@0x801029d4-a14). Sonst: **+0x222 += Tabelle[waffe]**; unter 15 → restaurieren;
  **≥ 15 → STAGGER `0x80102AD0`**: Clip-Wort 0x1F0008 (Clip 8, 50 F), **SE 12**,
  Frame 15 **SE 9**, Root-Motion (Rückstoß-Wackeln ±48) + 0x800152C8; Clip-Ende →
  Wort=1 (sub0) (@0x80102b18-98). Zerfall: −1 je 15 Frames (Main-Tick §1.2).
* +0x04=2 wird EXE-seitig beim Treffer gesetzt (Schreiber nicht decompiliert,
  OFFEN — Standard wie EMZ0 [2]=HURT); +0x1FC = gesichertes Routine-Wort.

### r0=3 — TODES-SEQUENZ ([T16..T21]; Trigger (+0x226&6)==4, Bit 2 = EXE bei HP≤0)

| ph | Adresse | Aktion |
|---|---|---|
| 0 | [T16] 0x80102C34 | Wund-Spray-Kopie; busy → +0x225=240, HP=0, +0x226|=4, Wort restaurieren (Zug zu Ende spielen) @0x80102fe8-3048; sonst **HP=−1** @0x80103060, Clip-Wort 0x1F0006 (**Clip 6**), ph=1, +0x1D4|=2, **SE 10**, 7 Partikel-Bursts an ALLEN Part-Matrizen (Schleife parts+72, Schritt 172) @0x80103094-d0, **SE 13**, **+0x218=5** (Todes-Blob) @0x801030e4, Rumble-Salve 0x80039514(250,180,0)+0x800395b8(150,…)+8 verzögerte (t=20..240) @0x801030fc-1bc |
| 1 | [T17] 0x801031C0 | Clip 6; Wund-Risse: 16-Schritt-Lerp über die DREI Blob-Vektoren @0x801056C8 {(1903,−1046,−2310),(1703,−2646,−750),(2393,−546,1410)}, Anker part2+0x48 @0x801031f4-38c; Clip-Ende → Clip-Wort 0x1F0007 (**Clip 7**), ph=2, Timer 250 @0x801031d8-f0; alle 4 F Tentakel-Zufallskommando (Tail @0x801035d4: Wort **0xE01** + 0x8003947C(3,0) + Schütteln(15,250,0)) |
| 2 | [T18] 0x8010340C | Clip-Ende → abwechselnd Clip 6 / (rng&3==0) Clip 8 @0x80103424-44 (Zucken); Timer 250→0: ph=3, **+0x226|=8** (Kopf-Tracking/Kollision aus), Augen-Ziele (0,0,0)/(1,0,0), **Clip-Wort 0x1F000A (Clip 10!)** @0x801034c8, part2-Skala=4096 + Flag|=0x400 @0x801034b0-c0, Rumble-Kaskade 0x8003947C ids 3/3/2/3/3/2/3/3/2/2/2/1 @ t=0/5/15/60/65/75/90/95/105/110/115/120 @0x801034e0-56c |
| 3 | [T19] 0x80103634 | Clip 10 in **halber Geschwindigkeit** (Anim-Schritt nur bei Timer&1==0), ph+=Clip-Ende @0x8010363c-64 |
| 4 | [T20] 0x80103668 | **Absink-Rampe** (die frühere „Setz-Rampe"): t=Gewicht[2] += 16/12/8/4 (Stufen 1025/2049/3073) @0x8010373c-78; Farb-Fade part0/part2 → 0x403030 je 2. Tick @0x80103680-72c; `y=(t·2950)>>12` @0x801037c4-f4, part2-Bind-Y=4500−y @0x801037f8-804, Entity-Y=y @0x80103808 — **der Kriecher versinkt 2950 in die Masse, die Masse bleibt am Boden**; t≥4097 → ph=5 @0x801037a4-b0 |
| 5 | [T21] 0x8010380C | LEER (Dispatcher-Epilog) — Kadaver-Ruhe |

### r0=4 — Script-/Event-Hook (0x80103908, via 0x801056E8[1])

ph0: Clip-Wort 0x70000, 0x800CFBD8|=0x40, 3D-SE `0x8005BA28(0x4010001,&pos)`;
ph1: exponentielles Gleiten X/Z → Zielpunkt +0x164/+0x168 (Halbierung je Frame
@0x801039b0-e4), Drehung `0x8001569C(e,+0x15A,256)`; Frame 8: Yaw=+0x15A, ph=2;
ph2: Kamera-Yaw-Flip ±2048 um Victim-Step 0x80015CB8 @0x80103a30-70; Clip-Ende → ph3.
Kein Overlay-Schreiber von +0x04=4 → Raumscript/EXE-Event (ROOM7040-spezifisch,
für den Port ohne Belang).

## 3. INTRO-Choreo (sub2, Start-Zustand: Konstruktor setzt +0x05=2 @0x8010076C)

Scharf erst wenn **+0x1D4 Bit 0** gesetzt ist (Script; sonst bleibt [T0] stehen).
[T0] parkt dann X=−9000/Z=−23400 (@0x801011d0-dc), sichert die Bind-Y-Baseline
nach 0x80105BB0 (@0x80101200) und startet Blob-Zustand 3.

| ph | Adresse | Clip | Dauer | Aktion |
|---|---|---|---|---|
| 0 | [T0] 0x80101164 | 1 (0x1F0001 @0x8010116c) | — | Morph-Gewichte[0..3]=0; Armierung s.o.; SE 10 |
| 1 | [T1] 0x80101214 | 1 | 90 T | t=10/30/40: Tentakel 0/2/3 wecken (+0x06=1, part0-Skala 0) + SE 9; t=90: alle vier +0x06=6, ph=2, SE 11 |
| 2 | [T2] 0x801012EC | 1 läuft | 180 F | **Root-Motion +7014** (0x80015E7C+0x800152C8); Gewicht[1] = (part0BindY−Baseline)·4415/2048 @0x8010132c-68 (Masse bläht mit der Hebung); Clip-Ende → ph=3 |
| 3 | [T3] 0x8010136C | 3 (0x1F0003) | — | ph=4, SE 9 |
| 4 | [T4] 0x80101390 | 3 | 90 T | t=30: alle Tentakel +0x06=8 + SE 0; t=90: ph=5, SE 10 |
| 5 | [T5] 0x8010141C | 3 | 110 T | t=20/40/45/56: Tentakel 0/1/2/3 Wort=0x901 + SE 0; t=110: Clip 4 (0x1F0004), ph=6, Tentakel +0x06=7, SE 11 |
| 6 | [T6] 0x80101530 | 4 | 180 F | **Root-Motion +3946**; Gewicht[1] = diff·4415·2/4096 @0x80101584-b4; Ende → ph=7 |
| 7 | [T7] 0x801015B8 | 2 (0x1F0002) | — | Gewicht[1]=0, ph=8, Tentakel +0x06=9, SE 10 |
| 8 | [T8] 0x8010173C | 2 | 150 F | Anim; Ende → ph=9 |
| 9 | [T9] 0x8010160C | 0 (0x1F0000) | — | ph=10, SE 9 |
| 10 | [T10] 0x8010162C | 0 | 110 T | t=20/35/40/50: Tentakel 0/2/1/3 Wort=0xA01; t=110 → ph=11 |
| 11 | [T11] 0x80101700 | 2 | — | ph=12, Tentakel 0+2 Wort=0x40A01, SE 10 |
| 12 | [T12] = [T8] | 2 | 150 F | Ende → ph=13 |
| 13 | [T13] 0x80101764 | 0 | — | ph=14, **+0x218=0** (Puls an), Timer 60, SE 9, Broadcast 0x601 @0x80101790-94 |
| 14 | [T14] 0x80101798 | 0 | 60 T | Timer→0: **Wort=257 (→sub1)**, +0x1C0=0, SE 10 @0x801017c0-d4 |

## 4. Blob-Maschine — pulsiert und SCHLÄGT die Masse? JA (Frage 2)

Gewichte = part2-Morph-Controller (+0x88): +12=[0], +16=[1], +20=[2], +24=[3]
(dir[0]-Vertex-Morph, 4 Segmente, Anwendung 0x8004BF90 je Frame @0x80103bf8).

* **Zustand 0 (Kampf-Puls + Biss-Trigger) 0x80103C18**: ph0: Timer 180+rng&0x1F,
  Schritt 6; ph1: Gewicht[0] += Schritt (beschleunigt bis 162) bis >8000; ph2:
  −= Schritt (bis 192) bis <−548; Gewicht[3] halbiert je Frame — **die Masse
  ATMET dauerhaft** (@0x80103c6c-dfc). Trigger (jeder Frame, jede ph):
  Dist<6000 (@0x80103e08) UND +0x229==0 UND Arc ±128 (@0x80103e34) UND Spieler
  frei UND !(+0x226&4) → **+0x218=1 (BISS)** @0x80103e6c; zusätzlich 0x800CFD4E≤0
  UND HP≥0 → **DEVOUR: player_damage(500,0)** @0x80103ea0, Wort 0x401, +0x218=4
  @0x80103ec4 (zweiter Devour-Pfad — birkin-em36 §2.1 nannte ihn fälschlich
  „Damage-Fenster [21]").
* **Zustand 1 (MASSEN-BISS)** [T22..T26], ph=+0x219:
  ph0 @0x80103F44: ph=1, Speed=0. ph1 @0x80103F58 AUSHOLEN: Gewicht[0]−=Speed,
  Speed+=32, Gewicht[3]+=Speed/2; Gewicht[0]<−4048 → ph=2. ph2 @0x80103FE0
  **ZUSCHNAPPEN**: Gewicht[0]+=Speed (Kappe 0), Gewicht[3]+=Speed, ≥5573 → ph=3 +
  **SE 6** + +0x21E=15 @0x80104040-5c; **Lunge `X += Speed>>3`, Kappe 12000**
  @0x80104060-94 (≥11/F → +0x225=4 Schütteln @0x80104098-b4); Hitbox-B-Breite
  +0xB4 **+500/F bis 3700** @0x801040b8-e0; **Treffer: `0x800157D4(+0xA4-Punkt
  (X+900 vorgehalten @0x801040ec-f4), Spielerpos, Radius 1500)`** → frei:
  **player_damage(40,1)** @0x8010416c-70, **SE 14**, Grab-Modus 0x800CFBFC=514,
  0x800CFDAC=e, 0x800CFDCB|=0x80, +0x21B=10 (Blutserie), 0x8003947C(8,0),
  Schütteln(10,250,4) @0x80104174-c8; bereits gegriffen (Modus-Byte==5, Sub<2,
  0x800CFD4E≥0): Biss trifft den von einem TENTAKEL gehaltenen Spieler
  @0x80104108-64; Speed+=256/F @0x801041d8.
  ph3 @0x801041E8: 15 Ticks Nachwackeln Gewicht[3] ±(64/−192) → ph=4.
  ph4 @0x80104228: Gewicht[3]−=64 (Zusatz −32 solange `0x800157D4(±900,3000)`
  trifft @0x80104244-74), +0xB4 −15/F bis 2200; Gewicht[3]<0 → **+0x218=0,
  +0x229=5 (Cooldown)** @0x801042a0-ac. Blut-Tail: solange +0x21B>0 je Frame 2
  Bursts an Part 4 (Maul) + 1 am SPIELER-Part0 (*(0x800CFD90)+72) @0x801042d8-420.
* **Zustand 3 (Vorkampf)** 0x801044FC → [U0..U7] @0x80100084: Puls-Varianten
  (Schwellen 8501/−1048), Konvulsions-Zyklen (−2136/+712 je Frame, 7+rng&3 Zyklen
  @0x80104720-7c8) und eine 110-Schritt-Wellen-Tabelle (`Gewicht[0]=8000−24·s16
  @0x80105AD4[i]` @0x801047d8-828).
* **Zustand 4 (Devour-Blob)** 0x80104848: Schütteln(35,250,15)+0x8003947C(30,0);
  Kompress −1024·beschleunigt bis <−4048, dann += bis >10000; danach Zufalls-Kauen
  mit SE 14/6 (rng&1 @0x801049fc-a08) + Maul-Blut @0x80104a14-…
* **Zustand 5 (Todes-Blob)** 0x80104B94 → [U8..U13]: Deflate bis −4048, 300 Ticks
  Zuckungen (±32·rng, Klemme [−5000,8000], Blutstrahl an Part 4 mit fallender
  Intensität 8096→6096→4048 @0x80104d14-38), Aufblähen +64 bis 12000, Kollaps
  −128 bis <0 → ph5=Ende (@0x80104dc8-e34).
* Dazu die zwei „Augen-Wanderer" (Records @0x80105610/0x8010562C, Part 2,
  Grenzen ±15, Ziele je 16 Frames neu — Treiber 0x80105064/0x8010503C; Byte-Ströme
  @0x801055EC/F8/FC/0x80105608 — Detail OFFEN, rein dekorativ).

## 5. Spieler-Interaktion (Frage 3)

* **HP 600, easy 400** (Flag 0x20 in 0x800CFB74) @0x801003fc-418; Hitboxen A
  {6000,6000, y−2000, h1500} @0x80100534-48/0x8010057c-80, B {1300,1300, y0,
  h2200→im Biss 3700} @0x80100550-68/0x80100584-88; +0x1EE=5700 @0x8010058c.
  **Verwundbar überall** — das Overlay gate´t Treffer nicht nach Zonen; +0x1D2
  wählt nur die Wund-Optik (drei Höhenbänder, §2 r0=2).
* Schaden am Spieler: **Devour 500 (Kill)** @0x80100904/@0x80103ea0 (X≥9001 bzw.
  Blob-Idle, beide: Dist<6000, Arc ±128, Spieler frei) · **Massen-Biss 40**
  @0x8010416c (Reichweite: Anschnapp-Punkt +0xA4 mit +900 X-Vorhalt, Radius 1500,
  Lunge bis X=12000) · Tentakel-Schaden liegt im EM037-Overlay (nicht Teil dieses
  Zensus).
* Treffer-Rückmeldung: Flinch-Akkumulator (Schwelle 15, Waffentabelle @0x801056B4,
  Zerfall 1/15 F) → Stagger Clip 8 + SE 12; Tod: (+0x226&6)==4 → §2 r0=3.

## 6. Korrekturen an den Vorbefunden

1. **g5-optik §3.2 „Setz-Rampe [20] = Kampf-Einlauf" ist FALSCH**: [T20] ist
   TODES-ph4 (Dispatch @0x80102c24: `0x80100054[+0x06]`, ph4 nur aus ph3
   @0x80103658-64 erreichbar). Im Kampf reitet der Kriecher HOCH auf der Masse
   (rootY −4534…, §2-Tabelle); erst der Tod senkt ihn um 2950. ⇒ g5-optik §6
   Schritt 2 („uniform +2950 als Kampfhöhe") entfällt.
2. **„Advance @0x8010194c auf X=4000 zu" = RÜCKZUG** (Spieler auf +X, Belege §2).
3. **birkin-em36 §2.1: „Damage-Fenster [21] @0x80103ea0 / [24] @0x8010416c im
   Klauen-Cluster 6/7/8"**: [T21] ist leer; 0x80103ea0 = Devour-Trigger (Blob-
   Zustand 0), 0x8010416c = Massen-Biss (Blob-Zustand 1 ph2). **G5 hat KEINE
   Klauen-Angriffe** — Clips 6/7 = Tod, 8 = Flinch; die Angriffe sind Masse-Biss,
   Devour und Tentakel-Kommandos.
4. Clip 10 hat einen Setzer: @0x801034C8 (Todeskollaps, halbe Geschwindigkeit).
5. Tentakel-Liste = 0x800CFE20..2C (s3=0x800CC1E8+15416; g5-optik §3.6 Adressen
   waren Roh-Guesses des Annotators).
6. „0x8002959c = Treffer-Check" (birkin-bewegung §Referenzen) ist falsch:
   das ist der Anim-Schritt; die Zustands-Exits hängen am CLIP-ENDE.

## 7. UMSETZUNGS-SPEZIFIKATION — `enemy_ai_boss_g5.c` (neues Modul)

**Einhängung:** wie der Gator — eigener Tick `re15_g5_boss_tick(slot)`, Aufruf in
enemy_ai_common.c neben :13761 (`re15_gator_boss_active`-Muster), aktiv NUR für
`type==0x36 && (room==0x5090||0x5091)`; **ROOM3070/Typ 0x30 bleibt unangetastet auf
`re15_birkin_ai_tick`**; der 0x36-Zweig der Birkin-Wurzel (enemy_ai_common.c:11022,
:11041, :11123-11129 Masse-Sonderfälle) und die BKMAP-Näherung (main.c:761-771)
werden für 5090/5091 NICHT mehr betreten (BKMAP bleibt für nichts anderes nötig —
das Modul setzt `e->motion` direkt auf RE2-Clip-Indizes 0..10 der EM036-Bank).
Renderseite (Komposition/Parts, g5-optik §6 Schritte 1/3/4) bleibt bestehen;
NICHT übernehmen: g5-optik §6 Schritt 2 (§6.1 hier).

**Koordinaten-Anker (dokumentierte Port-Entscheidung):** ROOM5090-Streifen C ist
der um +25000 versetzte, auf 33730 (statt 57000) GEKÜRZTE room7040-Korridor
(g5-optik §5) — die absoluten RE2-X-Konstanten (4000..12000) lägen jenseits des
5090-Ostendes. Das Modul rechnet deshalb in `u` („RE2-X-Äquivalent"):
`u = 8000 + s·(x − X0)` mit X0 = sub04-`Pos_set`-X = **1200** (ROOM5090.RDT @0x12FE)
und s = Vorzeichen Richtung Spieler-Kampfmarke (300, @0x1320) = **−1**; alle
RE2-Schwellen gelten wörtlich auf u: Rückzugslinie u=4000 (@0x80101934), Lunge-
und Zug-Kappe u=12000 (@0x80104074/@0x80100fd0), Devour u≥9001 (@0x801008a0),
Entscheidungen 7000/10000/8000/11001 (§2 sub0). Yaw einmalig beim Kampfstart auf
die Spielermarke einrasten, danach KONSTANT (RE2: nie gedreht); Kopf-Tracking
(±2048-Wickel je Frame, @0x80100318-34) auf Bone 1 übernehmen.

**Kampfstart:** Freigabe = sub04-`Member_set(0x0c,0x13)` (grid==0x13, gemessen
birkin-bewegung §2) ≙ RE2 +0x1D4-Bit 0; davor Modul-Zustand PARK (Clip 0, keine
Bewegung — RE2-Beleg: [T0] ohne Armierung tut NICHTS). Danach INTRO sub2
(Tabelle §3; Tentakel-Kommandos im Port No-Ops, s.u.), dann Kampfschleife.

**Zustands-Tabelle (Kern; Ticks = Frames @30fps):**

| Port-State | RE2 | Clip | Dauer | SE (ENEMSE, sound-id 0x27 → Paar-Zeile 25 @EXE 0x97C32) | Bewegung | Schaden |
|---|---|---|---|---|---|---|
| PARK | [T0] unarmiert | 0 | ∞ | — | — | — |
| INTRO ph0..14 | §3-Tabelle | 1,3,4,2,0 | ~1120 T | 9/10/11/0 je §3 | Clip 1: +7014·f, Clip 4: +3946·f (Root-Spur, §2-Tabelle) | — |
| IDLE | sub0 | 0 | 120+rng&31 | SE 9 (Clip-Ende, 25 %) @0x80100ab8 | — | — |
| ZUG | sub1 | 5→2 | 150+85 F | 10 @0x80100dec, 9 @0x80100f18, 11 @0x80100f80, 9 @0x80101094, 10 @0x80101008 | Root-Spur Clip 5 ×1,5 = +4050·f, Kappe u<12000 | — |
| RÜCKZUG | sub3 | 2 | 150 F | 11 @0x8010187c, 11 @0x801018e4, 10 @0x80101920 | Rampe 0..50..0, −2500·f, Kappe u>4000 | — |
| FLINCH | 0x80102AD0 | 8 | 50 F | 12 @0x80102b30, 9 @0x80102b4c | Root-Spur (±48) | — |
| DEVOUR | sub4 | 9→2 | 150+150 T | 6 @0x80101bb8, 7 @0x80101bd0, 10 @0x80101cb8 | — | 500 @0x80100904/@0x80103ea0 |
| BISS (Blob) | +0x218=1 | (Morph) | ~60 T | 6 @0x80104058, 14 @0x801041a8 | Lunge +Speed>>3·f bis u=12000 | 40 @0x8010416c (Punkt +900·f, r=1500) |
| TOD ph0..5 | r0=3 | 6,7,6/8,10 | 250+300+534 T | 10 @0x8010307c, 13 @0x801030f4, Kaskade §2 | — | — |

Entscheidungslogik IDLE→{ZUG,RÜCKZUG,DEVOUR} und Biss-/Devour-Gates wörtlich aus
§2 (jede Schwelle steht dort mit @0x). Flinch: Akkumulator +Tabelle @0x801056B4,
RE1.5-Waffen-Mapping (Port-Entscheidung, an RE2-Semantik angelehnt): Handfeuer→5,
Schrot/Bogen→14, Magnum/Granate→20, Messer→1; Schwelle 15, Zerfall 1/15 F.

**Root-Motion im Port:** akkumulierte Spur = kf-Halbwörter +6/+8/+10 (EMR-Pool
@EMD 0x6040, Stride 24; Extraktor FUN_80015E7C, Anwendung FUN_800152C8 =
`RotMatrix(yaw)`·(vx,vz) auf X/Z). Falls `re15_emd_animation_t` das Feld nicht
führt: beim EM036-Laden je Clip die Spur aus dem Roh-EMD ziehen (Offsets oben)
und als Tabelle an das Modul geben — KEINE Näherungs-Konstanten.

**Tentakel:** RE1.5-ROOM5090 spawnt keine 0x37 (Randnotiz birkin-em36) — die
Kommandos (§1-Wortliste) werden im Modul zu No-Ops hinter einem Sender-Hook;
+0x228-Maske = „keine Kandidaten" (sub0/sub3-Scans laufen dann in den
Entscheidungszweig, byte-true erreichbar @0x80100c3c-44). EM037-Port = eigener
späterer Schritt (Overlay CDEMD0 @0x00708000 liegt bereit).

**Devour im Port:** Schaden 500 + Grab-Latch (Spieler einfrieren, Yaw = Boss-Yaw
+2048 @0x80101ba4-b4) + Clip 9; die Leon-Victim-Anim (EDD-Paar 3, 15-Bone-PL0-Rig)
passt nicht auf das RE1.5-Spieler-Rig → DEFERRED, dokumentieren (Kamera/Latch
genügen für den Kill). Blob-Morph-Gewichte: KI-seitig zählt nur das Devour-Ende-
Gate Gewicht[0]<6000 (@0x80101d04-14) — ohne Morph-Implementierung durch festen
Timer 150 T ersetzen (dokumentierte Abweichung); die Gewichts-Dynamik (§4) ist die
Vorlage, falls der dir[0]-Morph (g5-optik §3.4) später gebaut wird.

**Schütteln/Rumble:** 0x80039514/0x800395b8/0x8003947C-Aufrufe (Amplituden/Delays
in §2/§4) auf den Port-Screenshake mappen; Semantik der EXE-Queues OFFEN, die
Zahlenwerte sind zitiert.

## 8. OFFEN

* EXE-Schreiber von +0x04=2 (Treffer), +0x226 Bit 2 (HP≤0), +0x1D2, +0x05=Waffen-Id
  — nicht im Decompile-Satz; Overlay-Lesarten sind belegt, Portverhalten daraus ableitbar.
* 0x80015B94/0x80015CB8 (Victim-Kanal Start/Step), 0x80016480, 0x80016028-Details,
  0x8003947C-Queue — Decompiles fehlen.
* Augen-Wanderer-Bytestrom @0x801055EC.. (rein dekorativ).
* EM037-Tentakel-Overlay (eigener Zensus nötig, wenn Tentakel portiert werden).

## FAZIT (Kurz)

Das G5-Overlay ist identifiziert (CDEMD0.EMS @0x006C0000, 23476 B, @0x80100000)
und vollständig zensiert: Per-Frame-Main 0x801000BC, Routine-Tabelle @0x801055CC,
Sub-Tabellen @0x8010564C/0x80105660, Intro [T0..T14], Tod [T16..T21], Biss
[T22..T26], Blob-Maschine @0x80105ABC, Tentakel-Sender 0x80104E9C — jede Zahl mit
Adresse. Der Kern dessen, was der Nutzer vermisst, ist jetzt spezifiziert:
(1) Die BEWEGUNG ist Root-Motion der Clips — Intro +7014/+3946 (Clips 1/4), der
„Tentakel-Zug" sub1 = Clip 5 ×1,5 = +4050 nach vorn, gedeckelt bei u=12000; das
bisher als „Advance" portierte X−=Zähler-Kriechen ist in Wahrheit der RÜCKZUG auf
u=4000. (2) Die TENTAKEL sind vier eigene 0x37-Entities, die G5 per Routine-Wörter
(0xB01/0x70B01/… in deren +0x04) im Takt seiner Clip-Frames schlagen lässt — im
RE1.5-Raum existieren sie nicht und werden als No-Op-Hook dokumentiert. (3) Die
ANGRIFFE sind nicht Klauen-Clips, sondern der Massen-BISS der Blob-Maschine
(Morph-Ausholen → Zuschnappen mit Lunge, 40 Schaden @0x8010416c, wachsende Hitbox
2200→3700) und der DEVOUR-Kill (500 @0x80100904/@0x80103ea0, zwei Trigger,
Cooldown +0x229). (4) Treffer laufen über einen Flinch-Akkumulator mit Waffen-
Tabelle @0x801056B4 (Schwelle 15, Zerfall 1/15F) in den Stagger Clip 8; der Tod
ist eine sechsphasige Sequenz, deren „Setz-Rampe" [T20] — Korrektur an g5-optik —
das ABSINKEN des Kriechers in die Masse ist, nicht der Kampf-Einlauf. Damit ist
der byte-true Bauplan für `enemy_ai_boss_g5.c` komplett (Tabelle §7); ersetzt wird
ausschließlich der 0x36-Zweig in ROOM5090/5091, ROOM3070-Form-1 bleibt unberührt.
