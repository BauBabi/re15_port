# „Schreibmaschinen"-Sounds — Dossier (Save-Punkt + Text-Tipp-Effekt)

Nutzer-Report (2026-08-04): „Bei der Schreibmaschine fehlen gefühlt ein paar
Sounds. Ist das korrekt so?"

Status: **Beide Deutungen vollständig RE'd und gemessen. Gegen das RE1.5-Original
fehlt NICHTS — jede Stufe der Kette ist dort nachweislich stumm.** Die einzige
reale „fehlende Sounds"-Quelle ist der **Save-SCREEN des Ports** (die
RE2-Adaption): RE1.5s eigener (dormanter) Card-Screen ist stumm, aber die
RE2-Vorlage MEM_CARD.BIN spielt 24 SEs, und JEDES andere Menü in RE1.5/im Port
beept — nur der Save-Screen nicht. Fix-Plan unten. **Kein Engine-Code geändert.**

Vorab-Klarstellung: RE1.5 hat **keine Schreibmaschine**. Der Save-Punkt ist ein
TELEFON/COMPUTER (Flavor-Message „It's a phone/computer. You can save your
progress with this. Save is not available in this preview", 16 Stellen; Save-BG
= `DATA/TYPE00.TIM` CRT-Terminal-Schreibtisch). „Schreibmaschine" im Report =
(a) der Save-Punkt (Retail-RE-Sprachgebrauch) und/oder (b) der
Schreibmaschinen-TEXT-EFFEKT (Typewriter-Reveal der Messages).

---

## 1. MESSUNG — Port-IST (`RE15_SE_DEBUG=1`, SDL-dummy-Audio, echte Fenster-Läufe)

Läufe aus `re15_port/build/platform/pc/` (Logs kopiert in den Session-Scratchpad
als `runA_save.log` / `runB_dialog.log`):

**Run A — Save-Flow** (`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_GIVE_CARD=1
RE15_FORCE_EVENT=6@700 RE15_INPUT_SCRIPT="S0.2,W90"`):
Boot → Title-CONTINUE → LOAD slot0 → ROOM1150-Resume → `[force-event]
scd_event_fire(6) at F700` → `[scd F701] Cut_chg(6)` → Save-Screen (auto) →
`[save] saved (room 1150); card=consumed`.
**`[se]`-Zeilen im GESAMTEN Lauf: 2 — beide sind der Title-Confirm-Announcer
(se=0, CORE-Bank) beim Boot.** Zwischen Telefon-Examine, Cut, Save-Screen-Fade,
Slot-Navigation, Confirm, Save, Schließen: **NULL SE-Calls.**

**Run B — Dialog-Typing** (`RE15_PSELECT_AUTO=1
RE15_INPUT_SCRIPT="S0.2,W3,X0.2,W2,X0.2,W2,X0.2,W60"`):
Boot → NEW GAME → ROOM1240-Pre-Intro → ROOM1170-Intro; Narrator-Messages
main00–main05 tippen per Typewriter-Effekt (dazu `[voice] clip loaded:
main0N.wav` = die Voice-Over-Spur, KEIN SE).
**`[se]`-Zeilen während des gesamten Text-Tippens: NULL** (wieder nur die 2
Announcer-Layer beim Title-Confirm).

Port-Code-Census (grep): `msg_common.c`, `item_prompt_common.c`,
`pc_run_memcard_screen` (main.c:568ff) enthalten **keinen einzigen
`re15_audio_*`-Call**. Der Port tippt stumm und save-screent stumm.

---

## 2. ORIGINAL (a) — Save-Punkt-Interaktion: JEDE Stufe stumm (byte-belegt)

Sound-Layer des Spiels = `FUN_80045024` (SE, Operand `bank<<24|id<<16`;
Packer LAB_80041624) und `FUN_800453d0` (Raum-SE snd1). EXE-weiter
Decompile-Census: FUN_80045024 wird nur aus 7 EXE-Funktionen gerufen
(FUN_80011f50, FUN_80017fa4, FUN_8002c444, FUN_80035538, FUN_80036718,
FUN_8004a0cc + sich selbst); FUN_800453d0 nur aus FUN_80012d60. **Keine davon
liegt in der Message-/Examine-/Card-Kette.**

Die Kette Stufe für Stufe:

1. **ACTION-Scan** (SQUARE-Edge → AOT): `FUN_80042bac`. jal-Census aus den
   Bytes: einzige Call-Ziele `0x80014368` und `0x8004f008` — kein Sound.
2. **Save-Telefon-Subs** (GENERIC-AOT sce=3 → sub): ALLE Message_on-in-Sub-
   Telefone game-wide sind byte-identisch und **Se_on-frei** (Se_on = SCD-Op
   `0x36`, 12 B). ROOM1150 sub06 @RDT+0x10ea (14 Bytes, verifiziert):
   `29 06 2b 01 ff ff 02 00 2a 00 3c 01 01 00`
   = Cut_chg(6) · Message_on(1,0xffff,2) · Cut_old · Evt_next(1) · Evt_end.
   Identisches Muster (nur Cut/Msg-Id anders) in 1151 sub06, 2010 sub03,
   4010 sub08, 4011 sub03, 5010 sub03, 5011 sub08. 1070/1120 (+Mirrors) sind
   MESSAGE-AOTs ohne Sub → Stufe 3 direkt.
3. **sce-1-MESSAGE-Handler** `@0x80043084` (Dispatch-Tabelle @0x8007469c[1]):
   14 Instruktionen, **einziger Call @0x800430a0 `jal 0x80027e68`**
   (= Message_on-Entry mit a1=0x300). Kein Sound.
4. **SCD-Opcode 0x2B (Message_on)** = LAB_800404f4: lädt msg-Operanden,
   **einziger Call @0x80040518 `jal 0x80027e68`**. Kein Sound.
5. **Message-System** — siehe §3: komplett stumm.
6. **Card-Screen** — siehe §4: komplett stumm (und im Original ohnehin
   dormant/unerreichbar — der MZD-Save-Punkt zeigt NUR die Flavor-Message).

**Original-Soll am Telefon: SQUARE → Kamera-Cut → Message tippt STUMM →
CROSS-Dismiss → Cut zurück. Null Sound-Effekte.** (Die Cinematic-Räume
1170/1240 haben Voice-Over über den msg_show-Pfad — Voice, kein SE; die
Telefon-Messages haben keine Voice.)

---

## 3. ORIGINAL (b) — Typewriter-TEXT-EFFEKT: KEIN Per-Glyph-Sound (byte-belegt)

Message-Familie: `FUN_80027e68` (Open) → `FUN_80028134` (FSM/Glyph-Reveal) →
`FUN_80028868` (Per-Glyph-SPRT-Renderer) → `FUN_8002877c` (Close),
`FUN_800279c8` (Format-Text/Sprite-Print), `FUN_80028840` (Charset-Helfer).

**jal-Census von `FUN_80028134` aus den Bytes (420 Instr., vollständig):**
```
@0x800283f4  jal 0x80028840   (Charset/Pointer-Helfer im Reveal-Loop)
@0x800284f0  jal 0x800c69bc   (PAGE_WAIT: Pfeil-Positions-Packer, s.u.)
@0x800284f8  jal 0x800279c8   (PAGE_WAIT: ▼-Pfeil-Draw, Blink-Gate timer&0x18)
@0x800285a8  jal 0x8002877c   (YES/NO-Confirm: Message-Close)
@0x80028654  jal 0x800279c8   (YES/NO: Cursor-Draw)
@0x8002867c  jal 0x800279c8   (YES/NO: Text-Draw)
@0x80028750  jal 0x80028868   (Ausgang: Glyph-Renderer)
```
**Kein einziges Sound-Ziel.** Der Glyph-Reveal selbst (case 1,
@0x80028250–0x80028434) enthält außer dem Charset-Helfer GAR keinen Call.

Verdächtiger Kandidat ausgeräumt: `FUN_800c69bc` (liegt im DEBUG.BIN-Modul
@0x800c0000, Datei-Offset 0x69bc) ist ein **Positions-Packer, kein Sound**:
```
800c69bc: lui  t0,0x800c / lhu t0,-31422(t0)   ; DAT_800b8542 (Box-Breite-Modus)
800c69c4: ori  t1,zero,0xff7f / beq t0,t1,+
800c69d0: addiu a0,a0,44 / sll a0,a0,16 / jr ra / ori a0,a0,0x6d   ; ((y+44)<<16)|0x6d
800c69e0: addiu a0,a0,28 / sll a0,a0,16 / jr ra / ori a0,a0,0x99   ; ((y+28)<<16)|0x99
```
→ packt die ▼-Pfeil-Bildschirmposition für den nachfolgenden
`FUN_800279c8(pos,0x80,&DAT_80010938,0)`-Draw.

Ausnahmen geprüft (alle stumm):
- **Item-Prompt/Item-Get-Modal** `FUN_8001db28` (ruft FUN_80027e68
  @0x8001df90/0x8001dfe0): Call-Census = FUN_8001e1c8 (Item-Spin-Render),
  FUN_80029a98 (Thread-Cmd), FUN_8004dc4c (Inventar-Insert), FUN_8004df2c
  (return -1-Stub), FUN_8004ee78 (TIM), FUN_8004ef90 (Bit-Set). Kein Sound —
  RE1.5 hat auch keinen Item-Jingle.
- **YES/NO-Dialog** (FSM case 4): Toggle/Confirm lesen DAT_800ac76c
  (0x3000/0x4000) — nur Draws, kein SE.
- **Narrator/Cinematic** (msg_show, Räume 1170/1240): Voice-Over (XA bzw.
  Port-`[voice]`-Clips), aber ebenfalls kein Tipp-SE.

**Antwort (b): Das Original hat KEINEN Typewriter-Klick — weder pro Glyph noch
pro Page noch beim Dismiss. Der stumme Typewriter des Ports ist byte-true.**

---

## 4. ORIGINAL (c) — der (dormante) Memory-Card-Screen: ebenfalls stumm

`FUN_80025c00` (14-State-Card-FSM) — vollständiger jal-Census aus den Bytes
(680 Instr.): 0x80013b60, 0x800216ec/0x8002178c/0x800217b0 (Fade-Engine),
0x800264e8/0x80026594 (Open-/Close-Fade), 0x80026658 (Renderer), 0x80026ca8/
0x80026dd4/0x80026e54/0x80026f48/0x800271a8/0x80027368/0x80027488/0x800275f0/
0x800276b0/0x80027724 (Card-I/O-Sub-Ops), 0x80028ec4/0x800c0158 (Text),
0x80029ac8 (ChangeTh-Thread-Yield), 0x80043850, 0x8004ee38 (memcpy),
0x8004ee60, 0x80061fc0 (VSync/GPU-Wait: GPU_REG1 + TMR_HRETRACE, verifiziert).
**Kein Sound-Call — auch nicht transitiv** (alle Sub-Ops per Decompile-Census
geprüft; Renderer FUN_80026658 ruft nur Draw/Text/memcpy).

Das passt zum Befund [[reai-v2-save-load]]: der RE1.5-Card-Screen ist
DORMANT/dev-only (ein Caller, nie erreichbar) — unfertiger Code, nie mit SEs
poliert worden.

---

## 5. Der Kontrast — warum es sich trotzdem „fehlend" ANFÜHLT

**(1) RE1.5s EIGENE Menü-SE-Konvention (CORE-Bank 4) — überall sonst aktiv.**
Inventar-/Status-Screen-Input `FUN_8004a0cc`, byte-exakt:
```
@0x8004a154-58  lui a0,0x409 + jal 0x80045024   ; SE 0x0409: Menü-OPEN  (pad & 0x100)
@0x8004a478/a4a0/a4c8/a4f0 (+jal +4)  lui a0,0x404 ; SE 0x0404: CURSOR-Blip (4 Richtungen)
@0x8004a51c-20  lui a0,0x406 + jal 0x80045024   ; SE 0x0406: CONFIRM (virtual 0x4000 = SQUARE)
@0x8004a660-64  lui a0,0x405 + jal 0x80045024   ; SE 0x0405: CANCEL  (virtual 0x8000 = CROSS)
```
Auch der Title nutzt sie (Cursor 0x0404 @0x80102b1c-24, Announcer 0x0400
@0x80102c20-24 — beides im Port). Der Port spielt diese Beeps im Inventar
(`menu_common.c` se4(4)/se4(6)/se4(5)), in der Item-Box (`re15_itembox.c`)
und im Title — **nur der Save-Screen ist das einzige stumme Menü.**

**(2) Die RE2-VORLAGE des portierten Save-Screens beept.** RE2-Retail
`COMMON/BIN/MEM_CARD.BIN` (lädt @0x801c0000, 0x5d7c B) ruft den RE2-SE-Player
`FUN_8005ba28` (Struktur-identisch zu RE1.5s FUN_80045024: `param>>0x18`-Bank,
Slot-Tabelle DAT_800d4c48) an **24 Call-Sites**:
```
0x04040000 (Cursor-Blip) ×5: @0x801c05e0 0x801c0604 0x801c0778 0x801c07bc 0x801c166c
0x04050000            ×8: @0x801c09c4 0x801c0cd0 0x801c1568 0x801c1598 0x801c1768
                             0x801c18dc 0x801c1964 0x801c19ac
0x04060000            ×9: @0x801c08dc 0x801c0938 0x801c0c2c 0x801c1580 0x801c1728
                             0x801c1744 0x801c1850 0x801c1870 0x801c18b8
0x02220000 / 0x02240000 ×1: @0x801c2780 / 0x801c2794 (Spezial, Rolle OFFEN)
```
Dieselbe Bank-4-Triade 4/5/6 wie RE1.5s eigene Menüs. (Die Per-Site-Rollen
0x405↔0x406 = Cancel↔Confirm sind aus den Zählern + der RE1.5-Konvention
WAHRSCHEINLICH, nicht einzeln aus dem MEM_CARD-FSM gemappt — s. Offen.)

---

## 6. BEFUND — Divergenzen

| # | Punkt | Original | Port | Divergenz? |
|---|-------|----------|------|------------|
| TW-1 | Typewriter-Text-Effekt (per Glyph/Page/Dismiss) | stumm (§3, jal-Census FUN_80028134) | stumm (Run B: 0 `[se]`) | **NEIN — byte-true** |
| TW-2 | Save-Punkt-Interaktion (Examine→Cut→Message) | stumm (§2, Stufen 1–5) | stumm (Run A: 0 `[se]`) | **NEIN — byte-true** |
| TW-3 | Item-Prompt/YES-NO | stumm (§3 Ausnahmen) | stumm | **NEIN** |
| TW-4 | Save-SCREEN (Slot-Nav/Confirm/Cancel/Open) | RE1.5-FSM stumm (dormant, §4); RE2-Vorlage: 24 SEs (§5.2); RE1.5-Menü-Konvention: Bank-4 4/5/6/9 (§5.1) | stumm (`pc_run_memcard_screen` ohne Audio-Call) | **Gegen RE1.5: NEIN. Als RE2-Adaption + gegen die gameweite Menü-Konvention: JA — das ist die „gefühlt fehlenden Sounds"-Quelle** |

**Antwort auf „Ist das korrekt so?": Ja — gemessen am RE1.5-Original ist die
Stummheit an Schreibmaschine/Save-Punkt und beim Text-Tippen korrekt.** Der
einzige inkonsistente Fleck ist der Save-Screen selbst: er ist eine
RE2-Portierung (Nutzer-Direktive, RE1.5s Save ist dormant), hat aber die
SE-Calls seiner Vorlage nicht mitportiert und ist damit das einzige Menü im
Spiel ohne die Bank-4-Beeps.

---

## 7. FIX-PLAN (nur falls gewünscht — Design-Entscheidung, kein byte-true-Defekt)

In `pc_run_memcard_screen` (main.c:568ff) die CORE-Bank-4-SEs der gameweiten
Menü-Konvention einhängen (Infrastruktur `re15_audio_core_se` existiert und ist
im Inventar/Title bereits verifiziert):
- **Cursor-Step** (nav_up/nav_down, ST_LIST + ST_OVERWRITE): `core_se(4)`
  — Beleg: RE1.5 @0x8004a478ff (0x0404) + RE2-Card ×5 (0x0404).
- **Confirm** (`ok`-Kante, Slot-Wahl/Overwrite-Yes/Exit): `core_se(6)`
  — Beleg: RE1.5 @0x8004a51c (0x0406, virtual 0x4000) + RE2-Card ×9.
- **Cancel/Back** (`cancel`-Kante): `core_se(5)`
  — Beleg: RE1.5 @0x8004a660 (0x0405, virtual 0x8000) + RE2-Card ×8.
- Optional **Screen-Open**: `core_se(9)` (RE1.5-Menü-Open @0x8004a154) —
  RE2s Card-Screen hat stattdessen die Bank-2-Specials 0x22/0x24 (Rolle offen).
Danach Messlauf Run A wiederholen: erwartete `[se]`-Zeilen an Nav/Confirm/
Cancel, weiter NULL beim Text-Tippen.

Alternative (streng byte-true zu RE1.5): alles stumm lassen — dann ist dieses
Dossier der Beleg, dass nichts fehlt.

---

## 8. OFFEN

1. **Per-Site-Rollen der 24 RE2-MEM_CARD-SEs** (welcher 0x405/0x406 an welchem
   FSM-Event; nur nötig, falls die RE2-Rollen statt der RE1.5-Konvention
   gewünscht werden) — erfordert RE des MEM_CARD.BIN-FSM (Overlay nicht
   decompiliert; Roh-Disasm möglich).
2. **Zweck der RE2-Bank-2-Specials 0x0222/0x0224** (@0x801c2780/94, vermutlich
   Save-Erfolg/Karten-Zugriff).
3. **Design-Entscheidung des Nutzers**: Save-Screen stumm (RE1.5-byte-true)
   oder mit Bank-4-Beeps (konsistent mit Vorlage + restlichem Spiel)?

---

## Quellen/Methodik

- Disasm: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (EXE +
  DEBUG.BIN via `--bin DEBUG.BIN` + Adress-Trick base 0x80100000);
  MEM_CARD.BIN-jal-Scan per Python (Datei `info/re2leon/COMMON/BIN/MEM_CARD.BIN`).
- Decompiles: `RE_15_Quellcode_V2/FUN_{80028134,80027e68,80028868,800279c8,
  8002877c,80025c00,80026658,8004a0cc,8001db28,...}.c`; `RE2_Quellcode_V2/
  FUN_8005ba28.c`.
- SCD: `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT` (subScd @+0x44,
  Se_on=0x36, Message_on=0x2B).
- Port-Messung: `re15_pc.exe` (Fenster-Lauf, SDL_AUDIODRIVER=dummy,
  RE15_SE_DEBUG=1), Logs `debug.log` → Scratchpad runA_save.log/runB_dialog.log.
