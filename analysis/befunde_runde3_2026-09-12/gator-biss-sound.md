# BEFUND B — Gruppe gator-biss-sound: "Ausserdem fehlt der Biss Sound beim Alligator"

Nutzer 2026-09-12 (ROOM2090-Bosskampf). Analyse 2026-09-12.

## Kurzfassung (Wurzelursache)

Der ROOM2090-Boss ist heute **komplett stumm — by omission, nicht by design**:

1. `enemy_ai_boss_gator.c` enthaelt **keinen einzigen Audio-Aufruf** (grep ueber die ganze
   Datei: 0 Treffer auf `re15_audio_*`/SE-Hooks; die Datei bindet auch keinen Audio-Header ein).
2. Der Lade-Pfad des Bosses (`platform/pc/main.c:887`, `type == 0x23u && pc_enemy_load_re2`)
   registriert — anders als Hund/Kraehe/Spinne/Zombie (main.c:796-816) — **keinen
   ENEMSE-Audio-Hook und waehlt keine ENEMSE-Bank**.
3. Der generische Frame-Flag-SFX-Pfad `re15_enemy_anim_sfx` wird im Typ-0x23-Dispatch-Zweig
   (enemy_ai_common.c:13679-13699) **nicht aufgerufen** — der Gator laeuft an ihm vorbei.
   (Das ist derzeit sogar ein Glueck: der RE1.5-Zweig von `re15_enemy_anim_sfx` liest
   `frames[] >> 22` als Room-SE-Maske (enemy_ai_common.c:13302-13304); auf die RE2-kodierten
   EM23-Frame-Worte angewandt ergaebe z.B. 0x48000178>>22 = 0x120 = Phantom-Room-SEs 5+8.)

Im RE2-Original traegt der Alligator (EM23) seine Sounds ueber **genau zwei Mechanismen**,
beide vollstaendig disassembliert (unten): 4 direkte ENEMSE-Aufrufe im KI-Overlay und
**Frame-Flag-SEs in den EDD-Animationsdaten** (der Biss-/Brüll-Laut = **SE 4** der
**ENEMSE-Bank 17**). Die komplette Infrastruktur (Bank-TOC, Lader, Mixer, Samples) liegt
im Port bereits fertig — es fehlt nur die Verdrahtung fuer Typ 0x23.

---

## 1. PORT-IST

### 1.1 Boss-KI ohne Audio

- `re15_port/engine/src/enemy_ai_boss_gator.c` — kein `re15_audio_*`, kein SE-Hook, kein
  `#include re15_audio.h`. Alle "hits"-Treffer im grep sind Geometrie (`gb_seg_hits_*`).
- Biss-Stellen (Einfuegepunkte fuer den Sound):
  * `gb_biss_abschluss()` :312-322 — Schaden 50 + Knockdown bzw. FRESSEN-Umschaltung.
  * GBP_LUNGE-Eintritt: :1177 und :1241 (`e->motion = 4; e->anim_frame = 0` = Schnapp-Clip).
  * GUARD-Hochbiss: :1575 (gleiches Muster).
  * FRESSEN Schnapp 1/2: :1751 / :1782; Zubeissen aus `gb_biss_abschluss` :315.
  * HURT-Intercept: :521-548 (Flinch-Clip 10 @ :548); DEATH-Umschaltung :526 → GBP_DIE,
    Todesrolle Clip 7 @ :1812.
- Der 0x23-Dispatch (enemy_ai_common.c:13679-13699) ruft `re15_gator_boss_tick` bzw.
  `re15_alligator_ai_tick` — **ohne** `re15_enemy_anim_sfx` (vgl. Zombie-Zweig :13425 und
  ZGirl-Zweig :13642, die ihn haben).

### 1.2 Wie die anderen RE2-Gegner ihre SEs spielen (fertige Infrastruktur)

- **Hook-Muster** (engine bleibt link-sauber fuers PSX-Target): je Brain ein
  `re15_re2<X>_audio_hook(se_fn, bank_fn)`; PC registriert
  `re15_audio_re2_enemy_se` + `re15_audio_re2_enemy_bank` in `pc_enemy_load_ex`
  (main.c:796-816). Hund Bank 6 (enemy_ai_re2_dog.c:132), Kraehe Bank 7
  (enemy_ai_re2_crow.c:111), Spinne Bank 11 (enemy_ai_re2_spider.c:133), Zombie Bank 0.
- **Wiedergabe**: `re15_audio_re2_enemy_se` (audio_pc.c:1104ff) = byte-true
  FUN_8005bd6c-Dekodierung (Map-Eintrag, Kanal b3&0x1f, Prio b2&0xf, flag2000 → +0x10);
  Bank-Lader `load_re2_enemy_se_pc` (audio_pc.c:1018) liest `shared_assets/RE2/ENEMSE.VBS`
  ueber den TOC `s_re2_enemse_toc` (gen/re2_ems_toc.inc, aus der RE2-EXE generiert).
- **Bank-Wahl-Mechanismus** (byte-true, re2_ems.c:392-455): Paar-Tabelle @0x800A7400
  fuehrt **SOUND-IDs aus dem Spawn-Record (+7 → entity+0x1FA)**, NICHT Gegner-kinds
  (Beleg-Kette in enemy_ai_re2_crow.c:86-111; `lbu v0,7(v0)` @0x80057274,
  Vergleich `lb v1,506(a0)` @0x80052C48).

---

## 2. RE2-ORIGINAL — EM23-Sound, vollstaendig

Quelle: `EM23_OVL_0000.BIN` (18664 B, gelinkt @0x80100000; Beschaffung + TOC-Beleg
analysis/re2_alligator_2026-09-09/DOSSIER.md; Arbeitskopie fuer `re2_disasm.py` liegt
jetzt auch in `info/re2leon/COMMON/BIN/`).

### 2.1 Dispatch-Geruest (fuer die Einordnung der Rufstellen)

- STATE-Tabelle @0x8010461C, Index = entity+0x4 (`lbu v0,4(s0)` @0x801001E0,
  `lw v0,0x461C(at)` @0x801001F8): [0] INIT 0x801004E0, [1] **ACTIVE 0x80100764**,
  [2] **HURT 0x80101FF0**, [3] DEATH 0x80102C84, [7] 0x80102D00.
- ACTIVE dispatcht sub-state (+0x5) doppelt: Pre-Pass Tabelle @0x8010463C
  (`lw @0x80100794`), Haupt-Pass Tabelle @0x8010465C (`lbu v0,5(a0)` @0x801007B8,
  `lw @0x801007D0`): [0]=0x80100804 … **[3]=0x80100D30** … [7]=0x8010122C.
- HURT waehlt zuerst die REAKTION: Selektor 0x80101EAC schreibt **+0x1D2**
  (`sb v0,466(s0)`):
  * 1 @0x80101F14-18 (wenn +0x226==7 && +0x230-Byte!=1),
  * 2/3 @0x80101F7C-88 (**Maul-offen-Treffer**: (+0x218&0x10) && Global 0x800D4820==8
    && dist(+0x156)<25000; 3 wenn 0x800CFBD8&0x40000000, sonst 2),
  * 4 @0x80101FBC-C4 (nur wenn +0x225==0 && +0x22E==0 && rng&3==0;
    Cooldown +0x22E=(rng&0x3f)+120 @0x80101FC8-D4),
  * sonst 0 @0x80101FD8.
  Dann Zonen-Dispatch @0x80102050-6C (`lbu v0,466(s0)`, Tabelle @0x8010467C):
  [0]=0x80102090, [1]=0x80102158, [2]=0x801026B8, [3]=0x80102924, [4]=0x80102BD0.
- Das Maul-offen-Bit +0x218|=0x10 produziert ACTIVE sub 3: Clip==3 && Frame>=0x72(114)
  @0x80100E74-A4 — sub 3 ist der **Lunge-Biss** (Clip 3, 150 F).

### 2.2 Die 4 direkten ENEMSE-Aufrufe (jal 0x8005bd6c — vollzaehlig, eigener Scan)

| # | SE | Adresse | Kontext | Gate |
|---|----|---------|---------|------|
| 1 | **4** | @0x80100D84 (li a0=4 @0x80100D64, Delay-Slot von `beq v1,zero` @0x80100D60) | ACTIVE **sub 3 = Lunge-Biss**, Phase +0x6==0 (Brüller beim Angriffsbeginn) | einmal, danach +0x6++ @0x80100D94-98 |
| 2 | **5** | @0x801020C4 (li a0=5 @0x801020C0) | HURT Reaktion **0** (Standard-Treffer) 0x80102090 | `lw +0x230 & 0xFFFF0000 == 0` @0x801020AC-B8 = **Brüll-Cooldown +0x232 leer** |
| 3 | **4** | @0x801021EC (li a0=4 @0x801021C4, Delay von `beq v1,zero` @0x801021C0) | HURT Reaktion **1** 0x80102158, Phase 0 | einmal pro Reaktion |
| 4 | **0** | @0x80102C10 (`addu a0,zero,zero` @0x80102C0C) | HURT Reaktion **4** (seltene Grossreaktion) 0x80102BD0, Phase 0 | `lbu +0x232 == 0` @0x80102C00-08; danach +0x1D3\|=0x80 @0x80102C18-28 |

a1 ist immer der Entity-Zeiger (FUN_8005bd6c(byte se, uint *word0) — flag2000 kommt aus
`*word0 & 0x2000`, RE2_Quellcode_V2/FUN_8005bd6c.c Kopf).

### 2.3 Frame-Flag-SEs — hier lebt der eigentliche Biss-/Brüll-Sound

Overlay-Root-Tail @0x801003F0-50 (Maske `lui s1,0xf800` @0x801003F4):

```
801003f0: lw v0,376(s0)        ; +0x178 = Zeiger auf aktuelles EDD-Frame-Wort
801003f8: lw v0,0(v0)
801003fc: lui v1,0x2800        ; (w & 0xF8000000) == 0x28000000 -> +0x227 = 1 (Event-Flag)
8010040c: sb v0,551(s0)
80100424: lui v1,0x4800        ; (w & 0xF8000000) == 0x48000000 -> +0x232 = 55
80100434: sb v0,562(s0)        ;   = 55-Frame-BRUELL-COOLDOWN (unterdrueckt Hurt-SEs 5/0)
80100438: jal 0x80016028       ; EXE-Helfer: Frame-SE spielen
80100440: lbu v0,562(s0)       ; +0x232 je Frame -1 @0x80100440-50
```

EXE-Helfer FUN_80016028 (einziger Caller im ganzen Spiel ist @0x80100438 im EM23-Overlay):

```
80016034: lw v0,376(a1)        ; entity+0x178
8001603c: lw a0,0(v0)          ; Frame-Wort
80016040: lui v0,0x800         ; Bit 0x08000000 = "SE spielen"
80016050: jal 0x8005bd6c       ; ENEMSE
80016054: srl a0,a0,28         ; SE-Id = Wort >> 28 (Top-Nibble)
```

**EDD-Zensus EM23.EMD** (dir @0x2E2F8; Aktions-Bank dir[1], 12 Clips — selbst geparst):

| Clip (Laenge) | Bedeutung (Clip-Statistik Port) | Frame-SEs |
|---|---|---|
| 0 (163 F) | Loko/Schwimmen | f76 **SE2**, f162 **SE2** (Wasser-Schub) |
| 1 (26 F) | — | f12 SE2, f25 SE2 |
| 2 (150 F) | — | f50 SE2, f110 SE2 |
| 3 (150 F) | **Lunge-Biss** | **f50 SE4, f77 SE4** (0x48000178 / 0x48000193) |
| 4 (45 F) | Schnapp | **keine** (in RE2 datenseitig stumm) |
| 5 (120 F) | ? (Aufstieg?) | f2 SE1, f80 SE3 |
| 6 (125 F) | Wende | f6 SE4, f30 SE2, f34 SE2 |
| 7 (165 F) | Todesrolle | f4 SE4, f25/f36/f44 SE2, f45 SE4, f115 SE4 |
| 8 (97 F) | — | f8 SE4, f63 SE4, f90 SE2 |
| 9/10/11 | 10 = Flinch | keine |

**Der hörbare "Biss" des RE2-Alligators ist also SE 4** — als Brüller beim Angriffs-Start
(ENEMSE-Call ACTIVE sub 3 Phase 0) UND als Frame-Flag mitten im Lunge-Biss-Clip 3 (f50/f77).
Der reine Schnapp-Clip 4 traegt in RE2 keine Frame-SEs.

### 2.4 Korrektur zum Alt-Dossier: FUN_8001bf10 ist KEIN Sound

analysis/re2_alligator_2026-09-09/DOSSIER.md fuehrte "Sound `0x8001BF10` x24".
RE2_Quellcode_V2/FUN_8001bf10.c zeigt: die Funktion allokiert einen 0x7C-Stride-Record im
Pool @0x800D8D08 (Typ-Wort 0xA003, Positions-Parameter param_4) — das ist der
**Effekt-Spawner** (Wasserspritzer etc.), kein SE-Player. Sound = ausschliesslich
FUN_8005bd6c (2.2) + FUN_80016028 (2.3).

### 2.5 ENEMSE-Bank des Alligators = **Bank 17**, flag2000 = 0

1. Sce_em_set-Zensus (Opcode 0x44, +3=kind, +7=sound-id) ueber alle 495 RDTs in
   info/re2leon/PL0/RDT: kind **0x23** kommt in genau **einem** authored Record vor:
   **ROOM40A0.RDT scd16 @0x025A0** = `44 00 00 23 00 00 01 16 …` → **sound-id 0x16**
   (der zweite Roh-Treffer ROOM21B0 @0x212DE liegt in `77 77 …`-Fuelldaten weit hinter
   der SCD-Sektion = Falsch-Positiv, wie der ROOM5040-Ausreisser beim Hund).
2. Paar-Tabelle @0x800A7400 (Datei info/re2leon/PSX.EXE 0x97C00): 0x16 als kindA in
   Zeile **17** = {0x16,0x00} (Datei 0x97C22) sowie 67={0x16,0x1F}/68={0x16,0x20}
   (NPC-Kombis). ROOM40A0 traegt nur 0x16 (+ NPC-Record kind 0x45 mit sound 0x00, der
   das Raum-Paar nicht belegt) → FUN_80052b38 liefert **Bank 17, erste Haelfte** →
   **flag2000 = 0** (die Entity-Schleife @0x80052C4C-88 setzt 0x2000 nur fuer die zweite
   Haelfte).
3. Daten-Gegenprobe Bank 17 (ENEMSE.VBS, TOC-Eintrag = gen/re2_ems_toc.inc Zeile Bank 17):
   EDT @0x128000 (0xCA8), VBD @0x129000 (0xF430); Map 32 Eintraege, **live ids {0..5}**
   (zweite Haelfte 16..31 komplett 0xFFFFFFFF — passt zu flag2000=0); VH: 1 prog,
   7 tones, 7 VAGs:

   | SE | Map-Eintrag | Kanal/Prio | Tone→VAG | Groesse (~ms @22kHz) | Kontext-Label |
   |----|-------------|-----------|----------|----------------------|----------------|
   | 0 | 0x04120000 | ch4 p2 | t1→vag5 | 9328 B (~740) | Hurt-Grossreaktion (2.2 #4) |
   | 1 | 0x02230000 | ch2 p3 | t2→vag4 | 7728 B (~613) | Clip-5-Frame-SE |
   | 2 | 0x03330000 | ch3 p3 | t3→vag2 | 8224 B (~652) | Wasser/Schwimm-Schub |
   | 3 | 0x02430000 | ch2 p3 | t4→vag3 | 11888 B (~943) | Clip-5-Frame-SE |
   | 4 | 0x04520000 | ch4 p2 | t5→vag6 | **18032 B (~1431, laengstes Sample)** | **Biss-/Angriffs-Brüller** |
   | 5 | 0x04620000 | ch4 p2 | t6→vag7 | 7264 B (~576) | Standard-Hurt |

   (Labels aus dem Ruf-Kontext hergeleitet, nicht angehoert — s. OFFEN.)
4. Samples liegen im Port-Assetbaum: `re15_port/shared_assets/RE2/ENEMSE.VBS`
   (0x662800 B, enthaelt Bank 17 vollstaendig); der Lader `load_re2_enemy_se_pc`
   kann sie unveraendert laden (Bank-Nummer 17 < RE2_ENEMSE_BANK_COUNT 73).

---

## 3. PLAN

Alles PC-seitig ueber das etablierte Hook-Muster (engine bleibt PSX-link-sauber):

1. **Hook + Bank** — `enemy_ai_boss_gator.c`: `re15_gator_audio_hook(se_fn, bank_fn)`
   nach dem Muster enemy_ai_re2_dog.c:127-142, `#define GATOR_ENEMSE_BANK 17`
   (Beleg §2.5; flag2000 fest 0). Registrierung in `pc_enemy_load_ex` im 0x23-Zweig
   (main.c:885-888) — analog :796-816.
2. **Frame-Flag-Spieler (byte-true Mechanismus)** — im Boss-Tick nach jedem
   anim_frame-Fortschritt das EDD-Frame-Wort der laufenden (motion, anim_frame)-Position
   lesen (Bank via `re15_enemy_find(0x23)->anim`, Frame-Worte liegen roh vor — derselbe
   Zugriff wie re15_enemy_anim_sfx:13301-13302, aber mit **RE2-Dekodierung**):
   * Bit 0x08000000 → SE (Wort>>28) spielen (FUN_80016028 @0x80016034-54),
   * (Wort & 0xF8000000)==0x48000000 → Brüll-Cooldown-Feld = 55 (@0x80100424-34),
     je Tick -1 (@0x80100440-50).
   Deckt ab: Schwimmen Clip 0 (SE2 f76/f162), Todesrolle Clip 7 (SE4 f4/f45/f115 +
   SE2 f25/f36/f44). ⛔ NICHT den RE1.5-Pfad `>>22` benutzen (§Kurzfassung Punkt 3).
3. **Biss-SE** — der Boss nutzt (Design) Clip 4, der in RE2 datenseitig stumm ist; der
   RE2-Biss-Laut ist SE 4 beim Angriffs-Start (ENEMSE @0x80100D60-84) bzw. im
   Lunge-Clip 3 (f50/f77). Port-Abbildung (MAPPING, als solches kommentieren):
   **SE 4 einmal pro Biss beim Schnapp-Start** — Stellen :1177, :1241 (LUNGE),
   :1575 (GUARD-Hochbiss), :1751/:1782 (FRESSEN Schnapp 1/2), :315 (Zubeissen);
   der bestehende `bite_done`-Latch verhindert Doppel-Trigger im Fenster.
4. **Hurt-SEs** — im HURT-Intercept (:521-548): Standard-Treffer → **SE 5**, gegated auf
   Brüll-Cooldown==0 (@0x801020AC-B8); optional die seltene Grossreaktion **SE 0**
   (rng&3==0 + Cooldown 120+(rng&0x3f), @0x80101FAC-D4 / @0x80102C00-10) — passt zum
   vorhandenen 10%-Schwellen-Flinch.
5. **Test** — Unit-Pin ueber den Hook (Muster test_re2_enemse_bank / Gore-SE-Tests):
   Bank-Registrierung == 17; Todesrolle-Sequenz liefert SE-Folge 4@f4 … 2@f25 …;
   SE 5 unterdrueckt, solange der 55er-Cooldown laeuft.

## OFFEN (benannt, nicht geraten)

- Die SE-Klang-Labels (Brüller/Wasser/Hurt) sind aus Ruf-Kontext + Sample-Laengen
  hergeleitet, nicht angehoert. Vor "byte-true"-Etikett: Bank 17 dekodieren und anhoeren
  (RE15_BGM_DUMP-Muster) oder im Emulator gegenpruefen.
- Clip 5 (SE1 f2, SE3 f80) — Bedeutung unklar (Aufstieg/An-Land?); erst relevant, wenn
  der Boss den Clip nutzt.
- Global 0x800D4820==8 im Maul-offen-Gate (Reaktion 2/3) nicht aufgeloest (Waffen-Id?);
  fuer den Sound-Befund nicht noetig.
- Das 0x28-Frame-Event (+0x227=1, @0x801003FC-0C) triggert vermutlich den
  Wasserspritzer-Effekt (FUN_8001bf10-Familie, §2.4) — Effekt-Seite separat.
