# BEFUND gator-schrei-schaden — "Beim Aligator gibt es in RE2 richtige Schreie als Sound, die fehlen uns hier. Die Schadensanimation ist in RE2 auch staerker."

Nutzer 2026-09-13. Analyse Runde 8. NUR statische Analyse.
Quellen: `info/re2leon/PSX.EXE` (t_addr 0x80010000 @0x18, Datei-Off = 0x800+(addr-0x80010000)),
`info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN` (= CDEMD0.EMS kind 0x23 rec 1, gelinkt @0x80100000,
Datei-Off = addr-0x80100000), `info/re2leon/PL0/RDT/ROOM40A0.RDT`, `RE2_Quellcode_V2/`,
`re15_port/shared_assets/RE2/{CDEMD0.EMS,ENEMSE.VBS}`.

Werkzeuge (neu, reproduzierbar) unter `analysis/befunde_runde8_2026-09-13/tools/`:
- `em23_dis.py <addr> [n]` — MIPS-R3000-Disasm des Overlays
- `em23_sound_census.py` — Vollzensus ALLER jal-Ziele + a0/a1-Rekonstruktion der Sound-Calls
- `em23_frameflags.py` — jedes EDD-Frame-Wort beider Paare, SE-Bits dekodiert
- `em23_rootmotion.py` — Netto-Root-Versatz je Clip (selbst nachgemessen)
- `re2_room40a0_se.py` — RE2-ROOM40A0-Raumbank dumpen + als WAV rendern

WAVs: `analysis/befunde_runde8_2026-09-13/re2_room40a0_se/*.wav` (RE2-Raumbank),
`analysis/befunde_runde4_2026-09-12/gator_se_wavs/*.wav` (ENEMSE Bank 17).

---

## 0. KURZFAZIT

**Die Schreie fehlen, weil der Port genau die zwei Rufstellen nicht hat, an denen RE2 bruellt,
und weil drei der sechs RE2-Gator-Soundaufrufe in Runde 6 als "Blutlache" fehletikettiert
wurden.** Der Vollzensus (jal-Scan, §1) findet **10 Soundquellen**: 4 direkte ENEMSE-Rufe
(`jal 0x8005BD6C`), **6 Se_on-Rufe der 3D-Variante `jal 0x8005BA28`** (Bank/Record gepackt,
a1 = Entity+0x38 = Position) und die Frame-Flags. Der Port spielt davon heute 3.

Konkret fehlen:

1. **SE 0 — der 1,32-s-Bruecller bei der GROSSREAKTION auf Treffer** (`addu a0,zero,zero`
   @0x80102C0C + `jal 0x8005BD6C` @0x80102C10). Der Port-FLINCH
   (`enemy_ai_boss_gator.c:1414-1422`) ist **komplett stumm**. Das ist der Schrei, den man
   beim Beschiessen hoert.
2. **SE 4 — der 2,40-s-Angriffsbrueller am LUNGE-START** (`addiu a0,zero,4` @0x80100D64,
   `jal 0x8005BD6C` @0x80100D84). Der Port hat keinen Lunge-State, also nie einen Angriffsruf.
3. **Leons SCHREI im Fress-Finisher**: `Se_on(0x04020001)` = CORE-Bank Record 2 @0x80102EF4,
   ausgeloest bei **Opfer-Frame 3** (`lbu v1,333(s0)` / `addiu v0,zero,3` / `bne`
   @0x80102EDC-E4). Der Port-Finisher ist auf Leons Seite tonlos.
4. **Der 2,40-s-Todesbrueller aus der RAUM-Bank** (`Se_on(0x020E0001)` @0x8010280C) — Runde 4
   hatte die ROOM40A0-Raumbank falsch als "leer" abgehakt; sie traegt 16 VAGs (§3).

**"Schadensanimation staerker" ist byte-belegt vierteilig** (§4): (a) RE2 spult bei JEDEM
Treffer den laufenden Clip **2 Frames zurueck** (@0x80102030-48) — ein sichtbarer Stotterer,
den der Port nicht hat; (b) der Flinch-Clip 10 traegt **-1025 Einheiten Rueckstoss** in seinen
Keyframes (selbst nachgemessen, §4.3) und richtet den Koerper auf (PoseY -2033 → -1053) —
der Port spielt Clip 10 ohne Root-Motion, also ohne Rueckstoss; (c) Clip 10 laeuft mit
**frac 3** (`addiu a2,zero,3` @0x80102C44) = 3-Schritt-Crossfade, der Port setzt `anim_frac`
nie; (d) der Treffer setzt **Unverwundbarkeit** (`+0x1D3 |= 0x80` @0x80102C20-28) und die
Waffen-Tabelle liefert pro Schuss **5-15 Frames i-Frames** (§5).

---

## 1. SOUND-VOLLZENSUS DES EM23-OVERLAYS (Aufgabe 1)

`em23_sound_census.py` scannt alle 4666 Instruktionen auf `jal` und rekonstruiert a0/a1.
Gefunden: zwei Sound-Einspruenge in die EXE.

### 1.1 `jal 0x8005BD6C` — ENEMSE-Gegner-SE (a0 = id, a1 = Entity)

| Adresse | a0 | Kontext | Port heute |
|---|---|---|---|
| **0x80100D84** | **4** (`addiu a0,zero,4` @0x80100D64) | ACTIVE **sub3 LUNGE Phase 0** — eine Instruktion vor `sb v0,6(s1)` Phase++ (@0x80100D94) | **FEHLT** (kein Lunge-State) |
| 0x801020C4 | **5** (`addiu a0,zero,5` @0x801020C0) | HURT-**Reaktion 0** (Standard); Gate `lw v0,560(s0)` / `lui v1,0xFFFF` / `and` / `bne` @0x801020AC-B8 = **+0x232 UND +0x233 muessen 0 sein** | vorhanden (`:617`), Gate nur auf +0x232 |
| 0x801021EC | **4** | HURT-**Reaktion 1** = Kanister-Explosionstod | n/a (kein Kanister) |
| **0x80102C10** | **0** (`addu a0,zero,zero` @0x80102C0C, Delay-Slot) | HURT-**Reaktion 4 Phase 0** (GROSSREAKTION); Gate `lbu v0,562(s0)` / `bne` @0x80102C00-08 = +0x232 == 0 | **FEHLT** |

### 1.2 `jal 0x8005BA28` — Se_on-Zwilling, 3D (a0 = Bank<<24 | Rec<<16 | 1, a1 = Entity+0x38)

⛔ **KORREKTUR zu Runde 6**: `befunde_runde6_2026-09-12/gator-vollausbau.md` tippt diese
sechs Aufrufe als "Blutlache". Sie sind **Sound**. Beleg: `RE2_Quellcode_V2/FUN_8005ba28.c`
schreibt dieselben Voice-Register (`DAT_800d4f18`-Familie) wie `FUN_8005bd6c`, und der Port
zitiert die Funktion selbst bereits als Se_on-Zwilling (`enemy_ai_common.c:745-748`:
"RE2 FUN_8005ba28: Bank @0x8005ba30, Record @0x8005ba7c-80 => 0x04000001 = CORE 0,
0x04020001 = CORE 2"). Das echte Blut sind die separaten Aufrufe daneben
(0x800395B8 / 0x8003947C / 0x8001BF10).

Bank-Slot-Aufloesung (RE2-Array @0x800DBB78, Setzer selbst nachgeschlagen):
slot 0 = `&DAT_801fb700`, slot 1 = `&DAT_801faa00` (`FUN_80059654.c:65-69`),
**slot 2 = `*(DAT_800ce324+8)` = RAUM-Bank** (`FUN_80059e54.c:30`, `FUN_8005a338.c:12`),
slot 3 = `&DAT_801f8e10` = ENEMSE (`FUN_8005a09c.c:25`),
**slot 4 = `&DAT_801f9d10` = CORE** (`FUN_80059a00.c:22`, Lade-Tag `"              CORE EDH"`),
slot 5 = slot 2. **Identisch zur RE1.5-Bank-Nummerierung** (`re15_audio.h:136-141`).

| Adresse | a0 | = Bank/Record | Kontext | Port heute |
|---|---|---|---|---|
| 0x80102384 | 0x020F0001 | **RAUM rec 15** | R1 Kanister-Explosionstod | n/a |
| 0x8010280C | 0x020E0001 | **RAUM rec 14** | R2 Maulschuss-Tod A, direkt nach Clip-7-Start (`beq v0,zero` @0x801027F0) | **FEHLT** |
| **0x80102EF4** | **0x04020001** | **CORE rec 2** | Opfer-Handler [0] **Phase 2, Frame 3** (`lbu v1,333(s0)` / `addiu v0,zero,3` / `bne` @0x80102EDC-E4) = **Leons Schrei beim Rumschleudern** | **FEHLT** |
| 0x80102F94 | 0x04030001 | **CORE rec 3** | Opfer-Handler [1] P0 = Proximity-Fang-Kill (danach Blitz `0x800395B8(53,240,40)` @0x80102FA8) | **FEHLT** |
| 0x801030C4 | 0x04010001 | **CORE rec 1** | Opfer-Handler [2] P0 = ueberlebbarer RUECKWAERTS-Chomp (Speed -800 @0x801030E0) | indirekt vorhanden (§6) |
| 0x80103240 | 0x04020001 | **CORE rec 2** | Opfer-Handler [3] P0 = ueberlebbarer VORWAERTS-Chomp (Speed +800 @0x8010325C) | **FEHLT** |

CORE-Record-Semantik im Port bereits byte-belegt (RE1.5-Zwillinge, gleiche Bank-Nummer):
CORE 1 = Treffer-/Sturzschrei (`Se_on(0x04010001)` @0x80035e0c/e2c/e80 und @0x80036184-88;
`game_step_common.c:328` / `:486`), CORE 2 = zweiter Treffer-Schrei (@0x80035f90 /
@0x80036004; `game_step_common.c:329`), CORE 3 = **Todesroecheln** (@0x800367a8 /
@0x8010a864 / @0x80111df4; `game_step_common.c:206`, `enemy_ai_common.c:1201`).
Das passt exakt: RE2 gibt dem Instant-Kill-Fang CORE 3 und dem ueberlebbaren Chomp CORE 1/2.

### 1.3 Frame-Flag-SEs — Vollzensus BEIDER Paare (`em23_frameflags.py`)

Dekodierung `FUN_80016028` @0x80016034-54: Bit 0x08000000 → SE = Wort>>28;
Top-5-Bits 0x28 → +0x227 = 1 (Splash-Event @0x801003FC-0C), **0x48 → +0x232 = 55**
(Bruell-Cooldown, `addiu v0,zero,55` @0x80100430 / `sb v0,562(s0)` @0x80100434).

| Clip | F | Frame-Woerter | Port nutzt Clip? |
|---|---|---|---|
| 0 | 163 | f76 `0x28000045` SE2, f162 `0x28000095` SE2 | JA (`:813` u.a.) |
| 1 | 26 | f12 SE2, f25 SE2 | nein |
| 2 | 150 | f50 SE2, f110 SE2 | nein |
| 3 | 150 | f50 `0x48000178` SE4, f77 `0x48000193` SE4 | nein |
| 4 | 45 | **keine** (datenstumm) | JA (`:368/:1287/:1351/:1685/:1865`) |
| 5 | 120 | f2 `0x1800020B` **SE1**, f80 `0x38000259` **SE3** | JA (`:1894`) |
| 6 | 125 | f6 SE4, f30 SE2, f34 SE2 | nein |
| 7 | 165 | f4 SE4, f25/f36/f44 SE2, f45 SE4, f115 SE4 | JA (`:1937`, Tod) |
| 8 | 97 | f8 SE4, f63 SE4, f90 SE2 | nein |
| 9 / 10 / 11 | 37 / 30 / 59 | **keine** | 10 JA (`:1415`), 11 JA (`:1920`) |
| **Opfer-Paar 3** | 30 / 120 | **beide Clips ohne jedes Frame-Wort** | JA |

**Ungenutzte SE-Ids: keine** — 0..5 sind alle belegt. **SE 0 und SE 5 kommen AUSSCHLIESSLICH
aus direkten Rufen** (§1.1), nie aus Frame-Flags. Und es gibt **keinen Schmerzens-/Todesschrei
in den Todes-Clips 7/8/9 ausser SE 4** (Clip 9 ist stumm, Clips 7/8 tragen nur SE4/SE2) —
der eigentliche Todesschrei ist der **Raum-SE 14** aus §1.2/§3.

### 1.4 Was der Port heute tatsaechlich hoert

`enemy_ai_boss_gator.c` ruft `gb_se()` an 6 Stellen: `:368/:1287/:1351/:1685` (id **3**),
`:617` (id **5**), `:854` (Frame-Flag-Weiterleitung `gb_se(w>>28)`).
Weil der Port nur die Clips 0/4/5/7/10/11 faehrt, liefern die Frame-Flags im Kampf
**nur SE 2** (Clip 0 f76/f162) und im Tod SE4/SE2 (Clip 7).

| Ereignis im Port | gespielt |
|---|---|
| Schwimmen | ENEMSE SE 2 (Wasserschub) |
| Zubeissen | `gb_se(3)` → **RAUM-SE 2 des RE1.5-Raums** (Mapper `pc_gator_se_re15`, `main.c:734-738`) |
| Treffer einstecken | ENEMSE SE 5 (0,81 s Bell-Grunzer) |
| **FLINCH / Schadensanimation** | **NICHTS** |
| Fress-Finisher | ENEMSE SE 1 (Clip 5 f2) + **SE 3, das der Mapper auf Raum-SE 2 umbiegt** |
| Tod (Clip 7) | ENEMSE SE 4 x3 + SE 2 x3 (Frame-Flags) |

⛔ **REGRESSION aus Runde 6**: Runde 5 begruendete den Mapper "id 3 → `re15_audio_room_se(2)`"
ausdruecklich damit, dass id 3 *"ausschliesslich von den sechs Zubeiss-Stellen"* komme —
*"kein `motion = 5` in der Datei"* (`gator-biss-se.md` §4.3). Seit Runde 6 gibt es
`e->motion = 5;` (`:1894`), also feuert **Clip 5 f80 = SE 3** und wird vom Mapper auf den
dumpfen Biss-Impakt umgebogen statt den hellen Wasser-Einschlag zu spielen.

---

## 2. DIE SCHREI-SAMPLES (ENEMSE Bank 17, Runde-4-Renders neu bewertet)

Bank 17 bleibt richtig (Paar-Zeile 17 `{0x16,0x00}` @Datei 0x97C22; Baenke 17/67/68 tragen
byte-identische Gator-Haelften — Runde 4 §1/§2, unveraendert gueltig).

| SE | Kanal/Prio | Dauer | Attack | ZCR | Charakter | RE2-Rufstelle |
|---|---|---|---|---|---|---|
| **0** | ch4 p2 | **1,32 s** | sofort | ~1000 | **grosses Jaulen/Bruellen** | **NUR** @0x80102C10 (Grossreaktion) |
| 1 | ch2 p3 | 0,87 s | 0,15 s | — | Knurr-Grunzer | Clip 5 f2 |
| 2 | ch3 p3 | 0,98 s | anschwellend | ~600 | Wasserschub | Clips 0/1/2/6/7/8 |
| 3 | ch2 p3 | 0,94 s | <30 ms | ~1600 | heller Klatsch | Clip 5 f80 |
| **4** | ch4 p2 | **2,40 s** | **1,43 s** | ~700 | **Angriffs-BRUELLER** | @0x80100D84 (Lunge-P0), @0x801021EC, Clips 3/6/7/8 |
| 5 | ch4 p2 | 0,81 s | 0,05 s | — | kurzer Bell-Grunzer | @0x801020C4 (Standard-Hurt) |

**SE 0, 4 und 5 liegen alle auf Kanal 4 mit Prio 2** — RE2 spielt sie deshalb nie gleichzeitig:
Reaktion 0 ruft SE 5, Reaktion 4 ruft SE 0, **nie beides** (getrennte Handler @0x80102090 /
@0x80102BD0, Dispatch ueber +0x1D2 und die Tabelle @0x8010467C). Der Port-Patch muss diese
Exklusivitaet nachbilden (heute spielt `:617` SE 5 *bevor* `:628-638` ueber den Flinch entscheidet).
Der Port hat das byte-true Kanal-Prio-Gate bereits (`audio_pc.c` `re15_se_prio_gate`,
@0x8005be08-14 / `FUN_8005c92c`).

**Es gibt in RE1.5 kein Alligator-Sample** (Runde 5 §2: die RE1.5-0x23-KI ruft in allen
23 Overlay-Funktionen keine einzige SE-Funktion, und EM023 fehlt im RE1.5-CDEMD0.EMS) —
die ENEMSE-Brueller SE 0/4 sind die einzige authentische Gator-Stimme und liegen **bereits
geladen** (`re15_gator_audio_hook(pc_gator_se_re15, re15_audio_re2_enemy_bank)` `main.c:945`,
`GATOR_ENEMSE_BANK 17` `:254`).

---

## 3. ⛔ KORREKTUR: die ROOM40A0-Raumbank ist NICHT leer

Runde 4 §2 schrieb: *"Die Room-SE-Bank von ROOM40A0 ist leer (snd0.edt 192 B ohne pBAV,
snd0.snd 12 B) — es gibt in RE2 also KEINEN raumseitigen Zusatz-Laut."* Das ist falsch.
Selbst geparst (`re2_room40a0_se.py`): ROOM40A0.RDT Sektionstabelle @0x08,
**sec0 = EDT @0x009DE0 (192 B, 48 Records), sec1 = VH @0x009EA0 (Magic `pBAV`, progs=2,
tones=25, vags=16), sec2 = VB @0x00ACC0 (95 536 B)**. Belegte Records: 8-11, 13-19, 22, 26-31.

| Rec | Record-Bytes | Layer | VAG (B) | Rate | Profil | Bedeutung |
|---|---|---|---|---|---|---|
| **14** | `00 00 d3 20` | 2 (Tones 13+14) | 15 (11 232) | 8193 | **2,40 s**, Attack 0,21 s, Decay 1,11 s, **ZCR 952** (tief, getragen) | **TODESBRUELLER** — @0x8010280C, beim Start der Todesrolle (Clip 7) |
| **15** | `00 01 53 40` | 3 (Tones 5-7) | 14 (19 920) | 13781 | **2,53 s**, Attack 0,18 s, Decay 1,65 s | **Explosions-/Todesbrueller** @0x80102384 (Kanister) |
| 9 | `00 00 b3 01` | 1 | 12 (8 000) | 11025 | 1,27 s, Attack 0,48 s | Ambiente/Anschwellen |
| 18 | `00 01 23 01` | 1 | 9 (4 800) | 2917 | 2,87 s, ZCR 327 | tiefes Grollen (Ambiente) |

WAVs: `re2_room40a0_se/room40A0_se14_vag15.wav` (+ `_L1`) und `…_se15_vag14.wav`
(+ `_L1`/`_L2`). Diese beiden sind die grossen, mehrschichtigen "richtigen Schreie" des
RE2-Alligators — sie liegen aber in der RAUM-Bank von ROOM40A0 und sind im Port **nicht
ausgeliefert** (`re15_port/shared_assets/RE2/` enthaelt nur BOXPANEL/BOXTEXT/CDEMD0.EMS/
EM23.EMD/EM23.TIM/EM23_OVL_0000.BIN/ENEMSE.VBS/FILES).

---

## 4. "SCHADENSANIMATION IST IN RE2 STAERKER" — was das byte-genau heisst (Aufgabe 2)

### 4.1 Der HURT-Eintritt: 2-Frame-Ruecklauf bei JEDEM Treffer

`FUN @0x80101FF0` (Routine 2). Bei frischem Eintritt (+0x5 != 255):

```
80102024  sb    a1,560(s0)      ; +0x230 = laufender ACTIVE-Sub (gerettet)
80102028  jal   0x80101EAC      ; Reaktions-Selektor
8010202C  sb    v0,5(s0)        ; +0x5 = 255
80102030  lbu   v1,333(s0)      ; +0x14D = Anim-Frame
80102038  sltiu v0,v1,2
8010203C  bne   v0,zero,0x80102050
80102044  addiu v0,v1,-2
80102048  sb    v0,333(s0)      ; ***Frame -= 2***
```

Jeder Treffer zuckt den laufenden Clip also **zwei Frames zurueck** — auch wenn gar keine
Reaktion gewaehlt wird. Der Port hat das nicht (`gb_absorb_hit` `:611-639` fasst `anim_frame`
nie an).

### 4.2 Der Reaktions-Selektor `FUN @0x80101EAC` (vollstaendig disassembliert)

| # | Bedingung | Adressen |
|---|---|---|
| — | HP < 25000 → +0x218 \|= 0x20 ("geschwaecht") | `slti v0,v0,25000` @0x80101EC4; `ori 0x20`/`sh` @0x80101ED8-DC |
| **0** | Spieler-HP (0x800CFD4E) < 0 → +0x1D2 = 0 | @0x80101EE4-EEC, @0x80101FD8 |
| **1** | +0x226 == 7 (traegt Kanister) UND +0x230 != 1 | @0x80101EF4-F18 |
| **2/3** | (+0x218 & 0x10 Maul offen) UND **[0x800D4820] == 8** UND HP < 25000; 3 wenn 0x800CFBD8 & 0x40000000 | @0x80101F24-F88 |
| **4** | +0x225 == 0 (nicht im Lunge) UND **+0x22E == 0** UND **(rand & 3) == 0**; danach **+0x22E = (rand & 0x3F) + 120** | `lb v0,549` @0x80101F8C; `lbu v0,558` @0x80101F9C; `jal 0x80015FE8` / `andi 0x3` @0x80101FAC-B4; `andi 0x3F` / `addiu 120` / `sb` @0x80101FC8-D4 |
| sonst | 0 | @0x80101FD8 |

⛔ **OFFENER PUNKT AUS RUNDE 6 GESCHLOSSEN**: `0x800D4820` ist **KEINE Waffen-Id**, sondern
der **aktive KAMERA-Cut**. Beleg: `FUN_8002bdf4.c:19-20` `DAT_800d4824 = DAT_800d4820;
DAT_800d4820 = (ushort)DAT_800cfbf2;` mit `DAT_800cfbf2` = dem von `FUN_8002c7ac` gesetzten
Cut (`FUN_8002c7ac.c:5`) und der Kamera-Array-Adressierung
`*(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20` (`FUN_80040350.c:30`, `FUN_8002bdf4.c:74-75`).
Der Maulschuss-Kill ist also **an Kamera-Cut 8 gebunden**, und die Nah-Cam-5-Bedingung
in R1 (@0x80102204-6C) verlangt Kamera 6 oder 13.

Die Grossreaktion hat damit **25 % Chance pro Treffer** und danach **120-183 Frames Sperre**
(= 4,0-6,1 s bei 30 fps; +0x22E wird je Tick dekrementiert, `lbu` @0x80100494 / `sb`
@0x801004A4). Der Port flincht dagegen **alle 6 Treffer ODER je 300 HP** (`:628-638`,
beides DESIGN).

### 4.3 Die Grossreaktion `FUN @0x80102BD0` (Reaktion 4) — der eigentliche "starke" Teil

```
80102C00  lbu   v0,562(s0)      ; +0x232 Bruell-Cooldown
80102C08  bne   v0,zero,0x80102C18
80102C0C  addu  a0,zero,zero    ; a0 = 0   (Delay-Slot!)
80102C10  jal   0x8005BD6C      ; ***SE 0 = der 1,32-s-Bruecller***
80102C14  addu  a1,s0,zero
80102C18  lbu   v0,467(s0)
80102C20  ori   v0,v0,0x80
80102C24  jal   0x801039EC      ; Treffer-VFX (Blut, Tabelle @0x80100144 nach +0x230)
80102C28  sb    v0,467(s0)      ; ***+0x1D3 |= 0x80 = UNVERWUNDBAR***
80102C38  sb    v0,6(s0)        ; Phase 1
...
80102C40  addiu a1,zero,10      ; ***Clip 10***
80102C44  addiu a2,zero,3       ; ***frac 3***  -> Crossfade-Gewicht 4096/(3+1) = 1024
80102C48  jal   0x8001A330      ; Clip-Treiber MIT Bewegung
80102C50  beq   v0,zero,0x80102C68
80102C54  addiu v1,zero,257     ; 0x101
80102C5C  sw    v1,4(s0)        ; Routine-Wort = ACTIVE sub1 (Schwimmen)
80102C60  andi  v0,v0,0x7F
80102C64  sb    v0,467(s0)      ; Unverwundbarkeit wieder weg
```

`frac` ist die **Crossfade-Laenge**, nicht Zeitlupe: `FUN_8001A330` rechnet
`a3 = 4096/(frac+1)` (`addiu v1,s1,1` / `addiu v0,zero,4096` / `divu` @0x8001A37C-84) und
`FUN_80029614` mischt damit `weight = rate * (+0x14E)` gegen `4096 - weight`
(`lbu t3,334(s2)` @0x800296A8, `andi v0,a3,0xffff` / `mult v0,t3` @0x800296B8-BC,
`addiu v0,zero,4096` / `subu` @0x800296E4-EC).

**Clip-10-Root-Motion selbst nachgemessen** (`em23_rootmotion.py`; EMR1 bones=22, kfsize=112,
kfo=0xFC; Keyframe-Bytes 6..11 = kumulativer Lauf-Versatz):

```
clip 10:  30 F  netto dx=-1025  dy=0  dz=0  (-34,2/F)  poseY -2033..-1053 (erst -2033 -> letzt -1053)
```

Der Gator **richtet sich also auf** (PoseY startet 980 Einheiten hoeher als er endet) und wird
**1025 Einheiten rueckwaerts geschoben**; der Treiber `FUN_8001A330` fuehrt diese Bahn ueber
`FUN_80015E7C` (Root-Versatz → Speed +0x144/146/148, `jal` @0x8001A374) und
`FUN_800152C8(em,0)` (`jal` @0x8001A3C0) wirklich als Bewegung aus.

Zum Vergleich die ganze Clip-Tabelle (selbst gemessen, bestaetigt Runde 6 §2.1):

```
clip  0: 163 F  dx +5990   clip  4:  45 F  dx +2491   clip  8:  97 F  dx +1243
clip  1:  26 F  dx +5798   clip  5: 120 F  dx  +329   clip  9:  37 F  dx +1243
clip  2: 150 F  dx +3535   clip  6: 125 F  dx +9498   clip 10:  30 F  dx -1025
clip  3: 150 F  dx +2884 / dz -4045   clip  7: 165 F  dx -6134 / dz +4072   clip 11: 59 F  dx 0
```

Der Port-FLINCH (`:1414-1422`) macht **nur** `e->motion = 10; e->anim_frame++;` — kein
Rueckstoss, kein Aufrichten ueber Root-Motion, kein Crossfade, kein SE, keine i-Frames.
**Das ist die ganze "staerkere Schadensanimation".**

### 4.4 Die Standardreaktion `FUN @0x80102090` (Reaktion 0)

```
801020AC  lw    v0,560(s0)      ; WORT bei +0x230
801020B0  lui   v1,0xFFFF
801020B4  and   v0,v0,v1        ; testet die OBEREN 16 Bit = +0x232 UND +0x233
801020B8  bne   v0,zero,0x801020CC
801020C0  addiu a0,zero,5
801020C4  jal   0x8005BD6C      ; SE 5
801020CC  jal   0x801039EC      ; Treffer-VFX
801020D4  lw    v0,508(s0)      ; +0x1FC gerettetes Routine-Wort
801020DC  sw    v0,4(s0)        ; ***Routine SOFORT zurueck*** — kein Anim-Abbruch
801020E0..80102138              ; und der ACTIVE-Sub laeuft im SELBEN Frame inline weiter
```

Also: **ein Standardtreffer bricht nichts ab**, es gibt nur SE + Blut + den 2-Frame-Ruecklauf.
Nebenbefund: das Gate prueft **zwei** Bytes (+0x232 Bruell-CD und +0x233, gesetzt von
`FUN_80103C84` @0x80103CC0 = Blut-Cooldown; beide dekrementiert @0x80100480-90 /
@0x80100440-50); der Port gated nur auf +0x232 (`:617`).

### 4.5 Treffer-VFX `FUN @0x801039EC` (Blutmenge haengt am Sub)

`lbu v0,560(s0)` (+0x230 = Sub vor dem Treffer), `addiu v1,v0,-1`, `sltiu v0,v1,19`,
Sprungtabelle **@0x80100144** (19 Eintraege, selbst dekodiert):

| Sub (+0x230) | Tabellenziel | Wirkung |
|---|---|---|
| 1 (Schwimmen) | 0x80103B4C | **gar nichts** |
| 2, 3 (Paddeln, Lunge) | 0x80103A30 | `0x80103B60(em, 1, 0, 6, 10240, 0)` = **1** Partikel, Typ 6, Betrag 10240 |
| 4 (Fressen) | 0x80103A3C | `FUN_80103C84` (eigener Emitter, setzt +0x233) |
| 5, 6 | 0x80103A4C | 1 Partikel |
| 7 (Maul-offen-Jagd) | 0x80103A58 | `0x80104244(em, 4)` |

Der Emitter `FUN @0x80103B60` wuerfelt pro Partikel einen von 24 Ankerpunkten aus der Tabelle
**@0x801046A0** (12 B je Eintrag; `addiu s2,s2,18080` @0x80103BB0; RNG `jal 0x80015FE8`
@0x80103BBC/BF4/C0C) und spawnt ueber `0x801044E0`. Der Port spawnt statt dessen **einen
festen** 0x1500-Burst an Bone 1 (`:620-626`) — bewusstes Port-Mapping, aber weniger sichtbar.

---

## 5. WAFFEN → SCHADEN (Aufgabe 2, zweiter Teil)

### 5.1 RE2 — der Gator ist durch Beschuss NICHT toetbar

HP-Init @0x80100518-580 (selbst disassembliert):
`HP = 25000 + Tabelle[0x800CFB74 & 0x40 / & 0x20]` mit den u16
`[0x801045BC] = 300`, `[0x801045BE] = 300`, `[0x801045EC] = 240`, `[0x801045EE] = 240`
→ **HP = 25300** (Bit 0x40 aus) bzw. **25240** (Bit 0x40 gesetzt).

Schadensformel `FUN_800470c0.c:41-45` (Gegner-Trefferaufloeser):

```
tbl = (&PTR_DAT_800a6a88)[entity+0x8 /*kind*/];
hp -= ( *(u32*)(tbl + (weapon & 0xffff)*0x14 - 0x14) >> ((param>>0x1c)*10 & 0x1f) ) & 0x3ff;
+0x1D3 |= (*(u32*)(rec+4) >> 9) & 0x7f;      /* i-Frames */
```

`PTR[0x23] = 0x800A4A14` (selbst gelesen; `PTR[0x25] = 0x800A4B90` begrenzt die Zeile auf
19 Waffen a 0x14 B). Drei 10-Bit-Schadenswerte je Record, ausgewaehlt vom Nibble
`param>>0x1c` (0/1/2 → Shift 0/10/20 = Trefferzone):

| w | @ | z0 | z1 | z2 | i-Frames | Treffer bis HP<25000 (z0) |
|---|---|---|---|---|---|---|
| 1 Messer | 0x800A4A14 | 10 | 0 | 0 | 15 | 30 |
| **2 Handgun** | 0x800A4A28 | **17** | 16 | 15 | **5** | **18** |
| 3 | 0x800A4A3C | 17 | 16 | 15 | 5 | 18 |
| 4 | 0x800A4A50 | 17 | 16 | 15 | 2 | 18 |
| **5 Shotgun** | 0x800A4A64 | **31** | 31 | 31 | 15 | **10** |
| **6 Magnum** | 0x800A4A78 | **50** | 50 | 50 | 15 | **6** |
| 7 | 0x800A4A8C | 27 | 25 | 24 | 15 | 12 |
| 8 | 0x800A4AA0 | 50 | 45 | 40 | 15 | 6 |
| 9 | 0x800A4AB4 | 30 | 30 | 30 | 15 | 10 |
| 10 | 0x800A4AC8 | 31 | 31 | 5 | 15 | 10 |
| 11 | 0x800A4ADC | 25 | 25 | 25 | 15 | 12 |
| 12 | 0x800A4AF0 | 20 | 20 | 20 | 0 | 15 |
| 13 | 0x800A4B04 | 17 | 16 | 15 | 5 | 18 |
| 14 | 0x800A4B18 | 30 | 30 | 30 | 15 | 10 |
| 15 / 16 | 0x800A4B2C / 40 | 10 | 10 | 10 | 10 | 30 |
| **17** | 0x800A4B54 | **110** | 110 | 110 | 15 | 3 |
| 18 | 0x800A4B68 | 18 | 18 | 18 | 10 | 17 |
| 19 | 0x800A4B7C | 17 | 16 | 15 | 5 | 18 |

**Lesart: die 25000 sind ein Sockel, der den Attrition-Tod verbietet** (25300 / 17 =
1488 Handgun-Treffer). Die einzigen 300 "echten" HP schalten bei HP < 25000 nur den Zustand
+0x218 & 0x20 frei — der **halbiert den Chomp-Schaden am Spieler von 60 auf 30**
(`lhu v0,536(s1)` / `andi v0,v0,0x20` @0x8010168C-94; `addiu a0,zero,60` @0x8010169C bzw.
`addiu a0,zero,30` @0x801016A0; dann `jal 0x801038E4` → `FUN_800401D4`) und **oeffnet den
Maulschuss-Kill** (Reaktion 2/3, §4.2). RE2s Alligator stirbt also **nur** durch Maulschuss
bei Kamera-Cut 8 oder durch den Gaskanister.

### 5.2 Port-Stand

`GB_HP 3000` (`:75`, DESIGN — "10x die byte-true 300er-HP-Zeile @0x801175dc"),
`GB_FLINCH_STEP 300` (`:76`, DESIGN). Schaden aus der byte-true RE1.5-Zeile
`@0x8006e0d0 + 0x23*0x58` (`re15_damage.c` `s_wpn_dmg_gator[22]`):
`{0,7,30,7,7,21,21,50,30,70,30,30,8,50,8,70,30,30,400,20,0,100}` — Handgun (w2) = **30**,
also 100 Handgun-Treffer bis 0 und 10 je Flinch-Schwelle. Das ist eine bewusste
Design-Entscheidung (der Port-Boss IST toetbar) und wird hier **nicht** angetastet.
Spielerschaden: `GB_BITE_TYPE 5` → `re15_damage_table[5] = 50` (RE2: 60 bzw. 30 geschwaecht).

---

## 6. WAS DER PORT AUF LEONS SEITE SCHON HAT

Ueberlebter Biss (`:371` `re15_player_knockdown_begin`) → `game_step_common.c:486`
`re15_audio_core_se(1)` (SE 0x04010001 @0x80036184-88). Das **deckt RE2s Opfer-Handler [2]
(CORE 1 @0x801030C4) ab**; RE2s Handler [3] nimmt dort CORE 2 (@0x80103240), der Port spielt
in beiden Sturzrichtungen CORE 1 (byte-true RE1.5: Phase 0 ist richtungsgemeinsam) —
akzeptable RE1.5-Treue, als Divergenz notieren.

**Toedlicher Biss (`:350-370`)**: `re15_player_take_damage(pl, 5, …)` spielt bei `type >= 2`
**keinen** SE (`re15_damage.c:222` — Raum-SE 10 nur fuer `type < 2`), der Stagger-Detektor
(`game_step_common.c:981` verlangt `pl->hp >= 0`) greift nicht, und
`re15_player_victim_force` (`enemy_ai_common.c:806-813`) spielt nichts. **Der gesamte
Fress-Finisher ist auf Leons Seite stumm.**

---

## 7. PATCH-PLAN (Aufgabe 3)

Alle Werte mit Adresse; kein Wert geraten. Reihenfolge = Wirkung pro Aufwand.

### P1 — GROSSREAKTIONS-BRUELLER SE 0 (`enemy_ai_boss_gator.c:611-639`)

RE2 spielt in Reaktion 0 **SE 5** und in Reaktion 4 **SE 0**, nie beides (§2). Heute steht
`gb_se(5)` (`:617`) **vor** der Flinch-Entscheidung (`:628`). Umbauen: Flinch-Entscheidung
nach vorn ziehen, dann

```c
if (flinch) { if (s_gb_brull_cd == 0) gb_se(0); }   /* @0x80102C00-14: Gate +0x232==0, a0=0 */
else        { if (s_gb_brull_cd == 0) gb_se(5); }   /* @0x801020AC-C4 */
```

Kommentar-Adressen: SE 0 `addu a0,zero,zero` @0x80102C0C + `jal 0x8005BD6C` @0x80102C10,
Gate `lbu v0,562(s0)` @0x80102C00. Bank bleibt 17 (`:254`), kein neues Asset.

### P2 — FLINCH MIT RUECKSTOSS + CROSSFADE (`:1414-1422`, Eintritt `:636-637`)

1. Beim Eintritt zusaetzlich `e->anim_frac = 3;` — `addiu a2,zero,3` @0x80102C44
   (frac = Crossfade-Laenge, Gewicht 4096/(frac+1): `divu` @0x8001A384 /
   `andi a3,0xffff` + `mult v0,t3` @0x800296B8-BC).
2. Root-Motion des Clips fahren statt ihn nur abzuspielen. Die Helfer existieren und werden
   im Finisher-Pfad schon benutzt (`:1873` / `:1898`): beim Eintritt
   `re15_clip_anchor_set_pub(e, &bk->skel, &bk->anim, 10, 0)`, danach je Tick
   `re15_clip_root_motion_abs_pub(e, &bk->skel, &bk->anim, 10, frame)`.
   Sollwert zum Gegenpruefen: **netto dx = -1025 ueber 30 F (-34,2/F), PoseY -2033 → -1053**
   (`em23_rootmotion.py`; Treiber `FUN_8001A330`, `jal 0x80015E7C` @0x8001A374 /
   `jal 0x800152C8` @0x8001A3C0).
3. Optional byte-true: Unverwundbarkeit waehrend des Flinch (`+0x1D3 |= 0x80` @0x80102C20-28,
   Freigabe `andi 0x7F` @0x80102C60-64). Weil die Port-Flinch-Kadenz abweicht (alle
   6 Treffer statt 25 %), **als Divergenz dokumentieren statt ungefragt zu uebernehmen**.

### P3 — TREFFER-STOTTERER: 2 Frames zurueck (`gb_absorb_hit`, nach `:616`)

```c
if (e->anim_frame >= 2) e->anim_frame -= 2;   /* @0x80102030-48 */
```

Gilt in RE2 bei JEDEM Treffer, auch ohne gewaehlte Reaktion.

### P4 — LEONS SCHREI IM FINISHER (`:1894-1906`, P2-Block)

RE2: `Se_on(0x04020001)` = **CORE 2** an Leons Position, ausgeloest bei **Opfer-Frame 3**
(`lbu v1,333(s0)` / `addiu v0,zero,3` / `bne` @0x80102EDC-E4; Ruf @0x80102EF4).
Im Port ist `sf` genau dieser Frame (`pl->anim_frame = (uint32_t)sf`, `:1902`):

```c
if (sf == 3) re15_audio_core_se(2);   /* Se_on(0x04020001) @0x80102EF4, Frame-Gate @0x80102EDC-E4 */
```

`re15_audio_core_se` ist im Port als CORE-Bank-Pfad byte-belegt (`re15_audio.h:174`,
`audio_pc.c:957`); Record 2 = Leons zweiter Treffer-/Schmerzensschrei (@0x80035f90 /
@0x80036004). Zusaetzlich fuer die **Proximity-Variante**, falls Runde-6-§7b.5 nachgezogen
wird: `re15_audio_core_se(3)` = CORE 3 (@0x80102F94) am Fang-Kill.

### P5 — MAPPER-REGRESSION: id 3 nur am ZUBEISSEN (`platform/pc/main.c:734-738`)

Der Mapper biegt heute JEDE id 3 um, auch die aus Clip 5 f80 (`0x38000259`). Die
Frame-Flag-Weiterleitung (`enemy_ai_boss_gator.c:854`) muss den Mapper umgehen — z. B. ein
`gb_se_raw()`, das `s_gb_se_fn` mit einem Kennbit "aus Frame-Flag" ruft, oder im
Frame-Flag-Zweig direkt `re15_audio_re2_enemy_se(id, 0)`. Belege: die Runde-5-Begruendung
*"kein `motion = 5` in der Datei"* ist seit `:1894` ungueltig; RE2-Frame-Wort Clip 5 f80
= `0x38000259` (`em23_frameflags.py`).

### P6 — ANGRIFFS-BRUELLER SE 4 (setzt den LUNGE-State voraus)

RE2 ruft SE 4 am **Lunge-Phase-0** (@0x80100D64 / @0x80100D84), also **~1,8 s vor dem
Maulkontakt** (Clip 2 Anlauf frac 16 @0x80100DAC-B4, dann Clip 3 mit Mauloeffnung ab f54) —
der 1,43-s-Attack des Samples faellt damit MIT dem Biss zusammen. Ohne Anlauf **darf SE 4
nicht an den 45-F-Schnapp** (das war exakt der Runde-4-Fehler). Also: erst Runde-6-§7a.2
(Lunge = Clip 2 frac 16 → Clip 3 frac 0, danach `Yaw += 0x400` @0x80100EB4-C0) umsetzen,
dann `gb_se(4)` in dessen Phase 0. **Und dann** das Cooldown-Arming byte-true verschieben:
`+0x232 = 55` wird in RE2 **nur** von 0x48-Frame-Woertern gesetzt (`addiu v0,zero,55`
@0x80100430 / `sb v0,562(s0)` @0x80100434), **nicht** vom direkten Ruf — der Port armt heute
in `gb_se()` `:270` bei jedem `id == 4`.

### P7 — TODESBRUELLER AUS DER RAUMBANK (optional, braucht ein neues Asset)

RE2 spielt beim Maulschuss-Tod `Se_on(0x020E0001)` = **ROOM40A0-Raumbank Record 14**
(@0x8010280C), 2,40 s, zweilagig (§3). Der Port liefert diese Bank nicht aus. Zwei Wege:
(a) EDT/VH/VB aus `info/re2leon/PL0/RDT/ROOM40A0.RDT` (**0x009DE0 / 0x009EA0 / 0x00ACC0**;
192 B / VH / 95 536 B) als Asset nach `shared_assets/RE2/` schneiden und einen kleinen Lader
analog `load_re2_enemy_se_pc` ergaenzen; (b) **darauf verzichten** — der Port-Tod (Clip 7,
`:1937`) feuert ueber die Frame-Flags f4/f45/f115 bereits **3x ENEMSE SE 4** (2,40-s-Brueller),
was klanglich dieselbe Rolle fuellt. Empfehlung: (b); (a) nur, wenn der Nutzer den
RE2-Todesschrei woertlich will.

### Reihenfolge

P1 + P4 bringen die Schreie (keine neuen Assets, keine neuen States).
P2 + P3 bringen die "staerkere Schadensanimation".
P5 ist eine Drei-Zeilen-Regressionsbehebung.
P6 haengt am Lunge-Nachbau (Runde 6 §7a.2). P7 ist optional.

---

## 8. OFFEN / NICHT GERATEN

- Welcher CORE-Bank-Index (CORE00…CORE15) in ROOM40A0 geladen ist:
  `FUN_80059a00((short)DAT_800d482c, 1)` (`FUN_80049e48.c:79`), Datei-Id-Tabellen
  `&DAT_800a80b8` / `&DAT_800a80e8`; der Schreiber von `DAT_800d482c` ist statisch nicht
  eindeutig. Fuer den Port irrelevant (Sound-Mandat = RE1.5-CORE), uebernommen wird nur die
  Record-NUMMER 1/2/3.
- `0x800CFB74` Bits 0x40/0x20 (HP-Zeilenwahl 300/240) — Szenario-/Schwierigkeitsbits, ungeklaert.
- Der Blut-Emitter-Ankerkatalog @0x801046A0 (24 x 12 B) ist dekodierbar, aber fuer den Port
  nur relevant, wenn der Ein-Burst-Ersatz (`:620-626`) ausgebaut werden soll.
