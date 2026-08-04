# Confirm/Cancel-Belegung: Original-Zensus vs. Port (Nutzer-Report 2026-08-04)

**Report:** „Die Bestaetigen/Abbrechen-Tastatur-/Controller-Belegung entspricht noch nicht dem
Original — z.B. bei der Auswahl eines zu ladenden Spielstands gemerkt."

**Ergebnis vorab:** Die Confirm/Cancel-**Bits** des Ports sind in **jedem** untersuchten Screen
byte-identisch zum Original (Card-Screen: □/START bestaetigt, ✕ bricht ab — exakt RE1.5s eigene,
dormante Card-FSM `FUN_80025c00`). Der wahrgenommene Bruch ist original-treu: im Title-Menue
bestaetigt **jede** Face-Taste (`0x8f0`), einen Screen spaeter (Card) bricht ✕ ab. Ein echter
(config-gegateter) Port-Defekt wurde trotzdem gefunden: `pc_run_memcard_screen` umgeht die
OPTIONS-Type-B/C-Remap (D-1 unten).

---

## 1. Pad-Architektur des Originals (die Grundlage aller Masken)

`FUN_80030444` (EXE) liest `PadRead(0)` und produziert **zwei** Wortfamilien:

| Global | Inhalt | Beleg |
|---|---|---|
| `DAT_800ac760` / `_DAT_800ac758` | RAW **held** (libetc-Layout) | `sw` @0x80030444-Decompile Z.27 |
| `DAT_800ac75c` (32-bit) / `DAT_800ac762` (lo16) | RAW **edge** `(prev^new)&new` | Decompile Z.25/31 |
| `DAT_800ac768` | VIRTUAL **held** | Remap-Loop @0x800304b8-e4 |
| `DAT_800ac76c` | VIRTUAL **edge** | Decompile Z.28 |

RAW = **libetc-PadRead-Layout**: `0x10`=△ `0x20`=○ `0x40`=✕ `0x80`=□ `0x100`=SELECT
`0x800`=START `0x1000`=UP `0x2000`=RIGHT `0x4000`=DOWN `0x8000`=LEFT `0x04`=L1 `0x08`=R1.
(Sanity: Title-Maske `0x8f0` @0x80102c14 = START+alle 4 Face-Tasten; Card-Nav `0x1000`/`0x4100` =
UP / DOWN|SELECT; START-Confirm `0x800` — alles nur unter diesem Layout sinnvoll.)

VIRTUAL: pro Bit i (15..0) `lhu mask = table[i*2]; and raw; bne -> or (1<<i)`
(@0x800304b8-e4, laedt `PTR_DAT_80073e1c` @0x800304b8). Aktive Tabelle =
`PTR_DAT_80073e1c[DAT_800b0fcc]`; `DAT_800b0fcc` wird vom OPTIONS-Config-Picker geschrieben
(`sh` @0x8002ea88 / @0x8002eaec; Reset @0x80021ee0) → **die virtuellen Bits folgen der
gewaehlten Button-Config game-weit**.

Preset-Tabellen (PSX.EXE Datei-Offset 0x645bc = `0x80073dbc`, je 16×u16):

| Virtual-Bit | Type A @0x80073dbc | Type B @0x80073ddc | Type C @0x80073dfc |
|---|---|---|---|
| 0..3 (0x1/2/4/8) | UP RIGHT DOWN LEFT | UP RIGHT DOWN LEFT | UP **R1** DOWN **L1** |
| 4..5 | UP DOWN | UP DOWN | UP DOWN |
| 6..7 (Action) | □ □ | ○ ○ | ○ ○ |
| 8 (Aim) | R1 | R1 | **R2** |
| 9 (Run) | ✕ | ✕ | ✕ |
| 12..13 (Menue-L/R) | LEFT RIGHT | LEFT RIGHT | LEFT RIGHT |
| **14 = 0x4000 (Menue-Confirm)** | **□ (0x0080)** | **○ (0x0020)** | **○ (0x0020)** |
| **15 = 0x8000 (Menue-Cancel)** | **✕ (0x0040)** | **✕ (0x0040)** | **✕ (0x0040)** |

→ RE1.5s Menue-Konvention: **v0x4000 = Action-Taste (Default □), v0x8000 = ✕** — der
Port-Anker „0x4000=SQUARE-Confirm, 0x8000=CROSS-Cancel" ist damit fuer Type A byte-belegt.

## 2. ORIGINAL-ZENSUS: Confirm/Cancel pro Menue-Kontext

| Screen (Original) | Wort | Confirm | Cancel | Beleg (@0x…) |
|---|---|---|---|---|
| **Title-Menue** (NEW GAME/LOAD GAME/OPTION) | RAW-Edge `DAT_800ac762` | `0x8f0` = △○✕□+START (jede) | — | `lhu 0x800ac762; andi 0x8f0` @0x80102c0c-14 (TITLE.BIN); SE @0x80102c20-24 |
| **Player-Select** | RAW-Edge | `0x8f0` (jede Face+START); Toggle L/R `0xa000` | — | `andi 0x8f0` @0x80101268; Toggle @0x80101258ff (TITLE.BIN) |
| **Card-/Save-Load-Screen** `FUN_80025c00` (dormant) | VIRTUAL-Edge `DAT_800ac76c` + RAW-Edge `DAT_800ac75c` | **v0x4000 ODER RAW-START 0x800** | **v0x8000** | Slot-Case: `andi 0x4000` @0x80025e84, START `andi 0x800` @0x80025e98, `andi 0x8000` @0x80025ea0 (fp=0x800ac76c @0x80025c58). Overwrite-Case 0xd: @0x800263f4/0x80026408/0x80026410. Result-Dismiss Case 0xc: `andi 0xc000` @0x80026378 ‖ START. Nav: RAW-held UP `0x1000` / DOWN‖SELECT `0x4100`, Repeat-Gate RAW-Edge `andi 0x5100` @0x80025c74, 0x13/0x05 Frames @0x80025c7c/8c |
| **Inventar Top-Menue** `FUN_8004a0cc` case 4 | VIRTUAL-Edge | v0x4000 (SE 0x0406 @0x8004a51c-20) | v0x8000 (SE 0x0405 @0x8004a660-64) | Decompile Z.105/112; Nav RAW-Edge 0x1000/2000/4000/8000 (Z.101-104, SE 0x0404) |
| **Item-Grid** (DEBUG.BIN-Modul 0x800c62a0) | VIRTUAL-Edge | v0x4000 @0x800c6470-78 | v0x8000 @0x800c6538-58; RAW-START `0x800` @0x800c6560 = Sofort-Close | dito |
| **Exchange-Select** @0x8004b37c | VIRTUAL-Edge | v0x4000 @0x8004b3a4 | v0x8000 @0x8004b398 (Cancel VOR Confirm) | dito |
| **MAP-Viewer** | VIRTUAL + RAW | — | v0x8000 @0x8004c1d0-e0 ODER RAW-L1 `0x4` @0x8004c1e8-f8 | dito |
| **MSG-Dialog** `FUN_80028134` (auch Item-Get-Prompt Yes/No) | VIRTUAL | Confirm/Yes-No: v0x4000-Edge `lw 0x800ac76c; andi 0x4000` @0x80028568-70; FF: v0x4000-**held** `lw 0x800ac768` @0x8002820c-14 | Toggle: v0x3000 @0x800285b8 (= RAW-D-Pad L/R); Page-/End-Wait-Dismiss: v0xc000 @0x80028458 / @0x80028698 (= □ ODER ✕) | **Korrektur des alten Merksatzes „MSG-Confirm=CROSS": byte-widerlegt — es ist die VIRTUELLE Confirm (physisch □ bei Type A)** |
| **OPTIONS/Config-Screen** | **RAW-Edge** (bewusst — der Screen editiert ja die Remap) | `0xa0` = □‖○ @0x8002e898 (Top) / @0x8002ea38 (Picker) | `0x40` = ✕ @0x8002ea30 | Nav LEFT/RIGHT @0x8002e80c/e830 |
| **Debug-/UTILITY-Menue** @0x80014444 | RAW held+edge | □ `0x80` = Raum laden @0x80014a38 | ✕ `0x40` = Exit @0x8001466c; △ `0x10` = Stage+1 @0x800148b4 | Nav RAW-held `0x800ac760` |
| **RE2-Retail MEM_CARD.BIN** (Vorbild des Port-Save-Systems, laedt @0x801c0000) | RE2-VIRTUAL-Edge `DAT_800ce310` + RE2-RAW-Edge `DAT_800ce300` | **v0x1000 ODER RAW-START 0x800** | **v0x2000** | `lw 800ce310; andi 0x1000` @0x801c08a8, START `andi 0x800` @0x801c08c0, Cancel `andi 0x2000` @0x801c08c8→Test @0x801c0964; Nav RAW-held `800ce304` 0x1000/0x4000 @0x801c05cc/05f0, Repeat-Gate RAW-Edge `andi 0x5000` @0x801c0590; SEs 0x0404/0x0406 via `jal 0x8005ba28` @0x801c05e0/0604/08dc |

**RE2-Virtual-Aufloesung** (Pad-Writer @0x80039340-84: `800ce304`=RAW-held-lo16,
`800ce306`=RAW-edge-lo16, `800ce300`=RAW-edge-32, `800ce310`=VIRTUAL-edge; Builder-Loop
@0x800391bc-d4, Tabelle `0x800a26a0 + DAT_800d46b2*0x20`, SLUS_007.48 Datei-Offset 0x92ea0):
alle 3 RE2-Presets mappen **v0x1000←✕(0x40), v0x2000←△(0x10; Type B: △‖□ 0x90),
v0x4000←○(0x20), v0x8000←leer**.
→ **RE2-Card: ✕/START bestaetigt, △ bricht ab** (RE2s ✕-Confirm-Konvention).
→ **RE1.5-Card: □/START bestaetigt, ✕ bricht ab** (RE1.5s □-Confirm-Konvention).
Beide FSMs sind strukturgleich (VIRTUAL-Confirm+RAW-START / VIRTUAL-Cancel); der Port folgt
korrekt der **RE1.5**-Belegung.

## 3. PORT-IST

### 3.1 Screens

| Port-Screen | Confirm | Cancel | Code | Match |
|---|---|---|---|---|
| `pc_run_memcard_screen` (Save+Load) | `vp&0x4000` ‖ `pp&START` | `vp&0x8000` | main.c:680-682 | Bits ✓ byte-true; **Config-Remap fehlt (D-1)** |
| Title-Menue | ✕‖□‖△‖○‖START | — | main.c:1808-09 | ✓ (0x8f0) |
| Player-Select | ✕‖□‖△‖○‖START | — | main.c:1191-92 | ✓ |
| Inventar/menu_common (Top/Grid/Exchange/MAP) | virt 0x4000 | virt 0x8000 (+RAW-START/L1 an den Original-Stellen) | menu_common.c:192/223/288/317/322/353/365/463/475/1064/1069/1350 | ✓ (laeuft ueber gctx = `pc_pad_config`-normalisiert, main.c:3640-41) |
| MSG-Dialog / Item-Get-Prompt | virt 0x4000 (Edge+Held-FF), Toggle 0x3000, Dismiss 0xc000 | — | msg_common.c:393-406, item_modal 2949 (mit `pc_pad_config`) | ✓ |
| Itembox | virt 0x4000 | virt 0x8000 | re15_itembox.c:376-426 | ✓ |
| OPTIONS/Config | RAW □‖○ | RAW ✕ | main.c:1555-56 | ✓ |
| Debug-Menue | □ Load, △ Stage | ✕ Exit | debug_menu_common.c (alle Konstanten mit @0x) | ✓ |

`re15_pad_virtual_word` (pad_common.c) == Type-A-Tabelle @0x80073dbc, 16/16 Eintraege
semantisch identisch (Port-Bit-Layout uebersetzt); `k_pad_remap[B]/[C]` (main.c:1224-26) ==
Dump @0x80073ddc/dfc, 16/16 ✓ (gegengeprueft in dieser Analyse).

### 3.2 Tastatur/Controller (input_pc.c:308-325 / 216-233)

| PSX-Taste | Tastatur | Controller (SDL) | Menue-Rolle |
|---|---|---|---|
| □ | **J** | X (links) | Confirm (Card/Inventar/MSG/Itembox), Action, Debug-Load |
| ✕ | **K**, L/R-Shift | A (unten) | Cancel (Card/Inventar/MSG-Dismiss-Teil), Run |
| ○ | L | B | Config-Confirm (mit □), Title/pselect-Confirm |
| △ | I | Y | Title/pselect-Confirm, Debug-Stage |
| START | Enter | Start | Card-Confirm (mit □), Title/pselect-Confirm, Inventar oeffnen |
| SELECT | Backspace | Back | Card-Nav-DOWN, Debug-Menue oeffnen |

**Nutzersicht konsistent:** J bestaetigt ueberall dort, wo das Original v0x4000/□ prueft;
K bricht ueberall dort ab, wo das Original v0x8000/✕ prueft; im Title/Player-Select gilt
original-treu „jede Face-Taste". Der im Report empfundene Bruch — „im Title geht K, im
Load-Screen wirft K mich raus" — ist **exakt das Original-Verhalten** (Title `0x8f0`
@0x80102c14 vs. Card-FSM v0x8000-Cancel @0x80025ea0). Die RE2-Gewohnheit (✕ bestaetigt,
@0x801c08a8) gilt in RE1.5 nicht; RE1.5 ist durchgaengig □-Confirm (Inventar @0x8004a51c,
MSG @0x80028570, Card @0x80025e84, Tuer/Action in-game).

## 4. DIVERGENZEN + FIX-PLAN

### D-1 (echt, config-gegatet): Card-Screen umgeht die OPTIONS-Button-Config
- **Original:** Die Card-FSM liest `DAT_800ac76c`, das der Config `DAT_800b0fcc` folgt
  (Builder @0x800304b8, Picker-Write @0x8002ea88/eaec). Mit Type B/C bestaetigt der
  Card-Screen im Original also mit **○** (v0x4000←0x20 @0x80073ddc[14]/dfc[14]).
- **Port:** `pc_run_memcard_screen` baut `vp = re15_pad_virtual_word(pp)` direkt aus
  `g_engine.pad_pressed` (main.c:667/680) — **ohne** `pc_pad_config` (das nur ingame
  @main.c:3640-41 und im Item-Modal @2949 vorgeschaltet ist). Unter Type B/C/EDIT verlangt
  der Card-Screen weiterhin □.
- **Fix:** main.c:680 → `uint16_t vp = re15_pad_virtual_word(pc_pad_config(pp));`
  (Muster von main.c:2949). START-Confirm bleibt RAW (`pp&START`, Original liest
  `DAT_800ac75c` RAW @0x80025e98) ✓; Nav bleibt RAW-held (Original `DAT_800ac760`
  @Decompile Z.68/72 — D-Pad mappt in allen Presets auf sich selbst, keine Nav-Aenderung).
- **Folge-Aenderungen:** KEINE fuer die Test-Envs — `RE15_CARD_AUTO` injiziert □ vor dem
  vp-Bau (main.c:672) und laeuft mit Default Type A = Identity (`pc_pad_config`
  main.c:1231 `return p`), ebenso `RE15_CONTINUE_TEST` (✕ am Title, dort RAW `0x8f0`)
  und alle Input-Skripte (Token A=□). Einzig ein Lauf, der VOR dem Card-Test Type B/C
  waehlt, wuerde ○ statt □ brauchen — ein solcher Test existiert nicht.

### D-2 (kosmetisch): invertierter Doku-Kommentar
main.c:566-567 sagt „save_mode: 0 = SAVE, 1 = LOAD"; der Code ist andersherum
(main.c:650 `save_mode ? "Do not save" : "Do not load"`; Aufrufer: Title-LOAD mit 0
@main.c:1877, Telefon-SAVE mit 1 @main.c:2760). Kommentar drehen.

### Kein Fix (byte-true bestaetigt):
- Card-Screen-Bits □/START-Confirm, ✕-Cancel (RE1.5-FSM @0x80025e84/e98/ea0 ist der
  massgebliche Beleg; das RE2-Vorbild liefert nur die FSM-Struktur, nicht die Belegung).
- Title/pselect `0x8f0`, Inventar, MSG, Config `0xa0`/`0x40`, Debug-Menue — alle geprueft.
- **Merksatz-Korrektur fuers Memory:** „MSG-Dialoge: Confirm/FF=CROSS" ist veraltet —
  MSG-Confirm/FF ist die VIRTUELLE Confirm v0x4000 (`andi 0x4000` auf `0x800ac76c`
  @0x80028570 bzw. held `0x800ac768` @0x80028214), physisch □ bei Type A.

## 5. Offen
- DuckStation-Live-Gegenprobe der Title-Maske (□ vs ✕ vs ○ vs △) nicht gefahren — die
  Masken sind statisch eindeutig (einzelne `andi`-Instruktionen), der Card-Screen ist im
  Original ohnehin dormant (nicht live erreichbar). Optionale Bestaetigung.
- RE2-MEM_CARD Case-Zensus wurde fuer die Confirm/Cancel-Sites erhoben (@0x801c08a8ff,
  0x801c08f8ff, 0x801c1530ff, 0x801c1638ff gleiche Bitmuster); ein vollstaendiger
  State-Graph des RE2-FSM war nicht Ziel dieser Analyse.
