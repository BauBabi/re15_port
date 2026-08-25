.EDH` und `CORE00.VB` sind zwischen RE1.5 und RE2 md5-IDENTISCH** (`9b0e0627500b50eaca5f8bc4124635d9` / `cdcb61fb`, 40464 B). Jeder RE2-Menü-SE mit Bank 4 / Record 0x00–0x0A ist damit **reiner Code-Import, null Asset**.
* **Record-Bedeutungen** aus RE1.5s EIGENEN Aufrufstellen: `0x04` Cursor (`8004A478/4A0/4C8/4F0`, `800C62D8/632C/63AC/6420`) · `0x05` Abbruch (`8004A660`, `800C6550`) · `0x06` Bestätigen (`8004A51C`, `800C6488`) · `0x07` fehlgeschlagen (in RE1.5 NIE gerufen, existiert als `CORE00.EDH@0x1C`) · `0x08` Seitenwechsel (`800C71D8/7264`) · `0x09` Menü-Öffnen (`8004A158`) · `0x0A` Equip (`800C64EC`) · `0x00–0x03` Spielerstimme.
* **⛔ Die RE1.5-Raumbank hat keinen freien Slot.** RDT-snd0-EDT = 0x80 B = 32 Records; Zensus über 206 RE1.5-Räume: 0x00–0x1F durchgehend belegt (0x1D–0x1F in 206/206). Schranke `sltiu s4,0x21` @`800450E4`. RE2 hat 48 Records. **Jeder RE2-Raum-SE mit Record ≥ 0x20 ist in RE1.5s Bankmodell nicht ausdrückbar** und braucht eine zusätzliche residente Port-Bank (Muster existiert: `audio_pc.c:1010-1015` `re15_pc_read_re2()` / `shared_assets/RE2/`).
* **⛔ Korrektur zu `analysis/typewriter_sounds.md` §5.2**: RE2-Overlay-Basis für CONFIG/MEM_CARD/SELECT/OPENING ist **`0x801BFA18`** (@`800107F4`), nicht 0x801C0000. Gegenprobe: `CONFIG.BIN`-Datei-Offset 0x110 = `addiu sp,sp,-24` = 0x801BFB28 = kleinster Selbstzeiger.
* **⛔ Die 5 Bank-0-Aufrufe in RE2s `TITLE.BIN` NICHT nachziehen** — Bank 0 zeigt in RE1.5 auf `0x801FDD00` und wird im gesamten Spiel nie benutzt (Voll-Scan EXE + DEBUG.BIN + alle STAGE-BINs + TITLE.BIN: nur Bänke 1,2,3,4).

## Gruppe A — reiner Code, Asset bereits resident (CORE00, Bank 4)

| # | Ereignis | RE2-Beleg (Adresse · Bank/Record) | RE1.5 | Port | Quelldatei | Import-Weg |
|---|---|---|---|---|---|---|
| A1 | **OPTIONS/Controller-CONFIG**, Cursorbewegung (22×), Bestätigen (5×), Abbruch (7×) | `CONFIG.BIN` @0x801BFA18. Rec 0x04: `801C0948 0A1C 0D48 0EC4 1038 11B4 16AC 1788 1864 1928 1B68 1C88 1D74 1FAC 2064 2180 23BC 2474 2A88 2B30 2BF4 2CA0`. Rec 0x05: `801C0444 0554 0C24 0CD8 1540 1A14 2934`. Rec 0x06: `801C0B0C 0B78 15B8 28C0 2950` | **stumm** (Task `8002DDE4` liegt in der Se_on-freien Lücke `8002C97C..8003338C`) | stumm | — (CORE00 resident) | `main.c` `pc_run_config` (dekl. Z.1692, gerufen Z.2485): `re15_audio_core_se(4)` bei Tab-/Zeilenwechsel, `(6)` Bestätigen, `(5)` Abbruch |
| A2 | **Inventar-Reiterleiste** ITEM/FILE/MAP/EXIT | `FUN_8006A7F0`: Cursorwechsel `8006A890/94` Rec 0x04 · Bestätigen Gate `8006A8A4 andi 0x1000`, Reiter 0..2 `8006A8C8` Rec 0x06, Reiter 3 `8006A8D4` Rec 0x05, Aufruf `8006A918` · Abbruch `8006A928 andi 0x2000` → `8006A930/34` Rec 0x05 · zweiter Abbruch `8006A94C andi 0x4000` + `[0x800D5C06]==1` → `8006A968/6C` | **stumm** (kein Se_on in `8004974C..80049A58`, keiner in `FUN_80046540`) | stumm | — | `engine/src/menu_common.c`, Reiter-Dispatch Z.1659: `se4(4)` Reiterwechsel, `se4(6)` Öffnen, `se4(5)` EXIT/Cancel. `se4()` existiert Z.110 |
| A3 | **Item-Get-Modal**, Aufnahme bestätigt | `80026528 beq v1,v0` (`[0x800D5C00]==2`), Delay `8002652C lui a0,0x406`, `80026530 jal 0x8005BA28` | **stumm** (`FUN_8001DB28`, jal-Zensus vollständig: kein Sound-Ziel) | stumm (`item_modal_common.c`: 0 Zeilen `re15_audio`) | — | `engine/src/item_modal_common.c`: Übergang nach state7 (Grant) → `re15_audio_core_se(6)`; NO/Abbruch → `(5)`; YES/NO-Toggle → `(4)` |
| A4 | **YES/NO-Box der Message-VM**, Toggle + Confirm | **kein RE2-Zwilling lokalisiert** — Konvention 4/5/6 ist game-weit belegt | stumm (`FUN_80028134`, 420 Instr., jal-Zensus: `80028840`, `800C69BC`, `800279C8`×3, `8002877C`, `80028868` — kein Sound) | stumm (`msg_common.c`: 0 Treffer) | — | `engine/src/msg_common.c`: Toggle (`DAT_800AC76C & 0x3000`-Kante) → `(4)`, Confirm (`&0x4000`) → `(6)`. **⛔ Als NACHRÜSTUNG kennzeichnen** (wie `menu_common.c:1090ff`) — Adressbeleg fehlt |
| A5 | **Game-Over-Schirm** | `DIEDEMO.BIN` @0x80190000, Datei 0x2A64 = `80192A64`: `80192A40 jal 0x80085DD0` / `80192A44 subu s0,s1,v0` (s1=100 @`80192A3C`) / `80192A5C bne v0,s1` / Delay `80192A60 lui a0,0x400` → **Bank 4 Rec 0x00** | stumm (`FUN_8001500C` in der Lücke `80018E3C..8002C70C`) | stumm (`re15_gameflow.c`: 0 Treffer) | — | `engine/src/re15_gameflow.c` beim Eintritt: `re15_audio_core_se(0)`. **⛔ VORBEHALT**: welche CORE-Bank während DIEDEMO resident ist, ist nicht ermittelt (Setter von `0x800A80B8[idx]` nicht verfolgt). Rec 0x00 = Charakterstimme in CORE00 — erst klären |
| A6 | **Memory-Card-Screen, Öffnungston** | RE1.5s eigener Beleg: `8004A154 lui a0,0x409` + `8004A158 jal 0x80045024` (Gate `pad&0x100`). RE2 `MEM_CARD.BIN` @0x801BFA18: Rec 0x04 ×5 (Datei 0x5E0 0x604 0x778 0x7BC 0x166C), Rec 0x05 ×8 (0x9C4 0xCD0 0x1568 0x1598 0x1768 0x18DC 0x1964 0x19AC), Rec 0x06 ×9 (0x8DC 0x938 0xC2C 0x1580 0x1728 0x1744 0x1850 0x1870 0x18B8) | Card-Screen `FUN_80025C00` stumm (jal-Zensus 680 Instr.) | **4/5/6 vorhanden** (`main.c:1167-1169`), Rec 0x09 fehlt | — | `main.c` `pc_run_memcard_screen` (ab Z.568): beim Eintritt `re15_audio_core_se(9)` |
| A7 | **Titel/LOAD-Screen**, Bestätigen + Zurück aus Unterschirmen | `TITLE.BIN` @0x80190000. Rec 0x04 ×22 (Datei 0x4FC 0x548 0x8F8 0x944 0x984 0x9C4 0x256C 0x25A4 0x2828 0x2860 0x28A4 0x28DC 0x2C00 0x2C38 0x2F1C 0x2F54 0x2FA8 0x2FEC 0x39C4 0x39F8 0x3C2C 0x3C5C) · Rec 0x05 ×7 (0xA78 0x21C0 0x2908 0x2C64 0x3018 0x38A4 0x3CD8) · Rec 0x06 ×10 (0x650 0x1F50 0x2360 0x2684 0x2698 0x3084 0x363C 0x367C 0x3CA0 0x3CB0) · Rec 0x00 ×3 (0x16D0 0x22B8 0x383C) | RE1.5 `TITLE.BIN` @0x80100000 ruft **4×**: `80102AC8` Rec 0x06, `80102B20` Rec 0x04, `80102B70` Rec 0x04, `80102C24` Rec 0x00 | teilweise: `main.c:2385` Rec 0x04, `main.c:2400` Rec 0x00 | — | `main.c` Titel-FSM: `se(6)` beim Bestätigen **zusätzlich** zum Ansager-Rec 0x00; `se(5)` beim Zurück aus jedem Unterschirm |

## Gruppe B — Asset-Import nötig (`CORE15`, eine einzige Datei-Paarung)

**Bank-Inventar** [B]: Von 13 gemeinsamen CORE-Nummern sind 4 byte-gleich (00, 0F, 12, 13). RE1.5-CORE10 == RE2-CORE11 und umgekehrt (**vertauscht**). RE1.5-CORE04..09 == RE2-CORE01 (`82f06386` / `ab90ba10`). RE2-only: **CORE14** (`d93bb3d4`, VB 199056 B, Tabelle 0x40, Recs 0x00/04/05/06 — dieselbe Rolle wie RE1.5-CORE12/13, **NICHT importieren**) und **CORE15** (`7009fa8d`, VB 63904 B, **Tabelle 0xC0 = 48 Records**, gültig 0x04, 0x06, 0x0A, 0x0B, 0x0C, **0x10**, **0x22, 0x23, 0x24**).

| # | Ereignis | RE2-Beleg | RE1.5 | Port | Quelldatei | Import-Weg |
|---|---|---|---|---|---|---|
| B1 | **Save-Screen: Schreibmaschine Start/Ende** | `MEM_CARD.BIN` Datei 0x2780 `a0=0x02220000` (Bank 2 Rec 0x22), 0x2794 `a0=0x02240000` (Rec 0x24) | **strukturell unmöglich** (Rec ≥ 0x20) | nein | `info/re2leon/COMMON/SOUND/CORE15.EDH` (3304 B) + `CORE15.VB` (63904 B) | nach `re15_port/shared_assets/RE2/`; neue residente Bank `RE15_SE_BANK_RE2SYS` in `include/re15_audio.h` neben `RE15_SE_BANK_CORE`; in `pc_run_memcard_screen` Recs 0x22/0x24 spielen |
| B2 | **Save-Raum-Trias** (Umgebungsgeräusch am Speicherpunkt) | 239-Raum-Zensus: Rec 0x22 in 30 Räumen (`00007316`), 0x23 in 30 (`00008317`), 0x24 in 30 (`00009316`) — exakt die Save-Räume. Alias in `CORE15.EDH`: 0x22=`00006316`, 0x23=`00007317`, 0x24=`00008316` (dupliziert auf 0x0A/0x0B/0x0C). Verbraucher `ENDING.BIN` Datei 0x36B0 (0x23), 0x374C (0x22) | strukturell unmöglich | nein | **dieselbe** CORE15-Paarung wie B1 | `engine/src/re15_savepoint.c` — **ein Import deckt B1 und B2** |
| B3 | **Player-Select: Tipp-/Scroll-Ton** | `SELECT.BIN` @0x801BFA18, Datei 0x2ADC = `801C24F4`: Zähler `0x2AB0/AB4`, `0x2AC0 sltiu v0,v0,0x14`, `0x2AC4 bne` mit Delay `0x2AC8 lui a0,0x410` → **Bank 4 Rec 0x10**. (Weitere SELECT-Töne 0x1038 Rec 0x06, 0x10CC/0x113C Rec 0x04 sind in CORE00 vorhanden) | **Record 0x10 existiert in KEINER RE1.5-CORE-Bank** (alle haben 0x40-Tabelle, gültig bis 0x0A) | nein | **dieselbe** CORE15-Paarung (Rec 0x10 = `00009317`) | Im RE15_PSELECT-Pfad (`platform/pc/main.c`) Rec 0x10 aus der RE2-Systembank. **Der einzige Menü-Ton, der wirklich ein Fremd-Sample braucht** |
| B4 | **Item aufnehmen / passt nicht** (5 Slots) | `FUN@0x80051514`: Item-Id 0xFE (`801515D4/D8`) → `801515F4/F8` **Bank 2 Rec 0x26**; Id 0xFF → `8005160C/10` **Rec 0x16**; sonst `80051628 jal 0x800696CC`, Erfolg (`80051634 bltz` nicht genommen) → `80051654/58` **Rec 0x25**, Fehlschlag → `801516A4` **Rec 0x16**; weiter `80051CF8` **Rec 0x15**, `801523A4` **Rec 0x27**. Verfügbarkeit im 239-Raum-Zensus: 0x15/28, 0x16/50, 0x25/27, 0x26/11, 0x27/10 | 0x25/0x26/0x27 strukturell unmöglich; 0x15/0x16 belegt | nein (`aot_common.c:1284`: „The pickup SE is the room's own SCD Se_on") | **NICHT CORE15** — die fünf Records aus einem RE2-Raum ziehen, der sie führt (RDT-Header: snd0-EDT @+0x08, VH @+0x0C, VB @+0x10) | Eigene VAB nach `shared_assets/RE2/`; in `engine/src/aot_common.c` am Item-AOT: Erfolg=0x25, voll/abgelehnt=0x16, Id 0xFE=0x26, Id 0xFF=0x16 |

## Gruppe C — offen / Architektur-Entscheidung, NICHT blind bauen

| # | Ereignis | RE2-Beleg | RE1.5 | Port | Quelle | Weg |
|---|---|---|---|---|---|---|
| C1 | **Tür-Öffnungsgeräusch aus dedizierter Tür-Bank** | `FUN_8001417C`: `801441AC jal 0x8002C1A0`, `801441C4 jal 0x8002C2B0`, `801441D0 lw v0,0x800C3A80`, `801441D8 lhu v0,584(v0)`, `801441E8/EC a1 = 0x800CFC30` (Position), Delay `801441F4 lui a0,0x1` → **Bank 0 Rec 0x01**, `801441F0 jal 0x8005BA28`. Bank-0-Lader `FUN_80014CD0`, Tag „DOOR SOUND" @0x8001084C, `0x800DBB78[0] = 0x801FB700` @`80014EB4` | Bank-0-Fall existiert (`800450BC` lädt `0x801FDD00`), wird aber **im ganzen Spiel nie gerufen** (Voll-Scan 41 EXE + 23 DEBUG.BIN + 66 Overlay-Callsites) | nein (`aot_common.c`: kein `re15_audio`-Aufruf) | **pro Tür geladene VAB**, kein Repo-Standalone | ⛔ **Architektur-Entscheidung.** Kurzweg: den vorhandenen Raum-SCD-`Se_on`-Pfad behalten und diesen Posten offen führen |
| C2 | **Schloss-Piepser auf/zu** | `FUN@0x80033D60` (Eintrag [15] der Tabelle @0x800A243C). Zweig A: `80034044 bne` Delay `80034048 lui a0,0x1` → `8003404C` (Bank 0 Rec 0x01), Zustand s0=6 @`80034058`; sonst `80034084 lui a0,0x2` → `80034088` (Rec 0x02), s0=5. Zweig B: `80104188/8C` und `801341D4/E4` | nein (keine Bank-0-Callsite) | nein | RE2-Tür-Bank | ⛔ **Erst den AUFRUFER der Tabelle @0x800A242C ermitteln** — welcher Bildschirm/welches Rätsel das ist, ist unbelegt. Bis dahin nur Kandidat |
| C3 | **Terminal-/Tastenfeld-Tippen** | `FUN@0x8003A9F4` (einziger Aufrufer `80039860`; Text-Xrefs `8003AB98`→"DOOR LOCK SERVICE]---" @0x80010BCC, `8003B4EC`→"Enter your user name.]>" @0x80010DB4). SE: `8003AF8C`, `8003AFE4`, `8003B050` — alle `lui a0,0x212` = **Bank 2 Rec 0x12**. Gruppen-Nachbarn: `80039CDC` 0x10, `80039E08` 0x09, `80039F10/FAC` 0x06, `80039F68/FD8` 0x07, `8003A504` 0x10, `8003B8AC` 0x06, `8003B934` 0x07 | kein Terminal | nein | RE2-Raum-snd0 Rec 0x12 | **Nur wenn ein Tastenfeld/Terminal im Port entsteht.** Dann über die residente RE2-Systembank (wie B4) |
| C4 | **EM37-Aufprall `Se_on(0x02290001)`** (aus dem Endkampf, TEIL 1) | EM37 `8010459C`, Bank 2 **Rec 0x29** | strukturell unmöglich | nein | RE2-Raum-snd0 des Endkampfraums | Über dieselbe residente Bank wie B4, oder ersatzlos streichen |

**Se_on-Gesamtzahlen** (eigener jal-Vollscan, Encoding `0x0C016E8A` bzw. `0x0C011409`) [B]:
RE2: PSX.EXE 138 · CONFIG.BIN 34 · MEM_CARD.BIN 24 · TITLE.BIN 47 · SELECT.BIN 4 · ENDING.BIN 2 · DIEDEMO.BIN 1 · RESULT.BIN 0 · OPENING.BIN 0.
RE1.5: PSX.EXE 41 · DEBUG.BIN 23 · TITLE.BIN 4 · STAGE1 11 · STAGE2 4 · STAGE3 16 · STAGE4 12 · STAGE5 19 · STAGE6 0.

**Abgrenzung — hier ist NICHTS zu importieren** [B]: Inventar-Grid-Navigation, Kommando-Cluster, CHECK, FILE-Seitenwechsel, Equip und Combine sind in RE1.5 vollständig vertont (`8004A158` Rec 0x09, `8004A47C/4A4/4CC/4F4` Rec 0x04, `8004A520` Rec 0x06, `8004A664` Rec 0x05; DEBUG.BIN 23 Aufrufe `800C62D8`…`800C7264`) und im Port umgesetzt (`menu_common.c` `se4()` Z.261-366, 767, 1509-1606; `re15_itembox.c:332`).

---

# TEIL 3 — Dokument-Vorlagen (FILES.TIM)

## 3.1 Container-Struktur [B]

`info/re2leon/COMMON/DATA/FILES.TIM` (5.257.216 B = 0x503800) ist **kein** einzelnes TIM, sondern **216 aneinandergehängte TIMs** in **191 sektor-ausgerichteten Slots**, gruppiert zu **25 Dokumenten**. Kein Container-Header (Datei-Offset 0 = `10 00 00 00 09 00 00 00` = TIM-Magic + Flags 8bpp/CLUT).

Ein Slot enthält 1 oder 2 TIMs, **byte-kontiguierlich ohne Padding**; der nächste Slot beginnt an der nächsten **0x800**-Grenze. 25 Slots führen 2 TIMs (Illustration + Titelseite), 166 führen 1 TIM (Folgeseite).
Beispiel Slot 0: TIM0 33312 B endet 0x8220, TIM1 startet unmittelbar 0x8220, endet 0xCA60 → nächster Slot @0xD000.

**Drei EXE-Tabellen** [B]:

```
Slot-Records      @0x800A94B4 (Datei-Offset 0x99CB4), 191 × 8 B:
                  { u32 groesse; u16 sektor_lo; u8 sektor_hi; u8 pad }
                  byteoffset = (sektor_lo | sektor_hi<<16) * 0x800
                  Leser: 8006D4D8 lw +0 · 8006D518 lhu +4 · 8006D50C lbu +6 · 8006D4F0 lbu +7

Dokument→Startslot @0x800A9AD0 (0x9A2D0), 25 × u8, danach 0x00:
                  { 0,7,14,17,21,32,43,54,62,68,76,79,83,88,92,97,
                    107,115,126,133,136,144,151,157,174 }
                  Leser: 8006D480 lbu a0,-25904(at)

Dokument-Record   @0x800AA144, 25 × 4 B: { u16 max_page; u8 y_off; u8 pad }
                  seitenzahl = max_page + 1 ;  seitenhoehe H = 256 - y_off
                  Leser: 80076224 lhu s4 (max_page) · 8007603C lhu s3 (y_off)
                         80076040 addiu s1,zero,256 · 80076044 subu s5,s1,s3
```

**Slot-Auswahl** [B]: `slot = first[doc]` wenn Flag `[0x800D5C05] == 0` (Titelseite), sonst `slot = first[doc] + 1 + page` mit `page = u8 [0x800D5C03]`. `docid = [0x800D4B68][sel_row*8 + sel_col]`, `sel_row = s8 [0x800D5C01]`, `sel_col = s8 [0x800D5C02]` (`8006D448`–`8006D498`).

**CD-Anbindung** (nur PSX-Target) [B]: FILES.TIM = Datei-Id **166 (0xA6)**, Basis-LBA **42049**. Es wird immer nur EIN Slot geladen (`8006D4B4 sh 166`, `8006D52C sw`, `8006D530 jal 0x80012FB8`); Zielpuffer über Debug-Tag „FILE ETC" @0x80011C7C. Im PC-Port entfällt das Streaming vollständig.

## 3.2 Inhaltstabelle — alle 25 Dokumente [B]

| doc | Titel (aus den Titelseiten-Pixeln gelesen) | Slots | Seiten | H | Datei-Offset |
|---|---|---|---|---|---|
| 00 | CHRIS'S DIARY | 0–6 | 6 | 144 | 0x000000 |
| 01 | FEDERAL POLICE DEPT. — INTERNAL INVESTIGATION REPORT | 7–13 | 6 | 176 | 0x02B000 |
| 02 | MEMO TO LEON | 14–16 | 2 | 176 | 0x05D000 |
| 03 | POLICE MEMORANDUM | 17–20 | 3 | 176 | 0x077000 |
| 04 | OPERATION REPORT | 21–31 | 10 | 176 | 0x097000 |
| 05 | MAIL TO THE CHIEF | 32–42 | 10 | 176 | 0x0E1000 |
| 06 | MAIL TO THE CHIEF | 43–53 | 10 | 176 | 0x12B000 |
| 07 | SECRETARY'S DIARY A | 54–61 | 7 | 144 | 0x175000 |
| 08 | SECRETARY'S DIARY B | 62–67 | 5 | 144 | 0x1A5000 |
| 09 | OPERATION REPORT 2 | 68–75 | 5 (**2 Slots Reserve**) | 176 | 0x1CB000 |
| 10 | USER REGISTRATION | 76–78 | 2 | 128 | 0x203000 |
| 11 | FILM A | 79–82 | 3 | 112 | 0x218800 |
| 12 | FILM B | 83–87 | 4 | 112 | 0x230800 |
| 13 | FILM C | 88–91 | 3 | 112 | 0x24C800 |
| 14 | PATROL REPORT | 92–96 | 4 | 144 | 0x264800 |
| 15 | WATCHMAN'S DIARY | 97–106 | 9 | 176 | 0x285800 |
| 16 | CHIEF'S DIARY | 107–114 | 7 | 176 | 0x2C9800 |
| 17 | SEWER MANAGER DIARY | 115–125 | 10 | 176 | 0x301800 |
| 18 | SEWER MANAGER FAX | 126–132 | 6 | 176 | 0x34B800 |
| 19 | RECRUIT | 133–135 | 2 | 112 | 0x37D800 |
| 20 | INSTRUCTIONS FOR SYNTHESIS OF THE G-VIRUS ANTIGEN: G-VACCINE. CODE NAMED "DEVIL." | 136–143 | 7 | 176 | 0x391800 |
| 21 | LABORATORY SECURITY MANUAL | 144–150 | 6 | 176 | 0x3C9800 |
| 22 | INVESTIGATIVE REPORT ON P-EPSILON GAS | 151–156 | 5 | 176 | 0x3FB800 |
| 23 | HINT FILES FOR THE ROOKIE MODE | 157–173 | 3 (**14 Slots Reserve**) | 176 | 0x427800 |
| 24 | HINT FILES FOR THE ROOKIE MODE | 174–190 | 3 (**14 Slots Reserve**) | 176 | 0x495800 |

**Reserve-Slots sind nicht leer** [B]: Pixel-Render von `FILES_0196/0200/0209/0211` zeigt echte Hinweistexte („A gas light in the press room.", „Elevator leading to the transport facility…"). doc23/doc24 liefern zusammen **34 fertig gesetzte 256×176-Textseiten mit identischer Palette** — die beste Vorlage für eigene Texte.

**Sonderfall doc23** [B]: wird abhängig von `[0x800CFBD8] & 0x04000000`, `[0x800CFB74] & 0x00200000` und `& 0x40000000` an Listenplatz 0 gesetzt UND sein `max_page` zur Laufzeit auf 1 **gepatcht** (`80068368 sh 1,-24160(at)` → 0x800AA1A0, `80068374 sb 23,19304(at)`). ⇒ **`max_page` ist ein RAM-Wert und darf wachsen** — im Port als veränderliche Variable führen, nicht als `const`.

**FILE-Liste zur Laufzeit** [B]: `u8 [0x800D4B68][24]`, `0xFF` = leer, Init-Schleife `800682DC`–`800682F8`, Anhängen `FUN_800692DC` (`sltiu v0,a1,0x18` = Grenze 24). **Es gibt keine Titel-Strings in der EXE** — der Titel steht als Pixel in der Titelseite.

## 3.3 Anzeigeverfahren [B]

`FUN_80075FD0` zeichnet **zwei SPRT-Prims** (GPU-Code 0x66, rgb 128/128/128, setlen 4) in eine 256×256-Fläche:

| Prim | Inhalt | u | v | w | h | CLUT | Adressen |
|---|---|---|---|---|---|---|---|
| 1 | Textseite 4bpp | 0 | 0 | 256 | **H** | `GetClut(0, 490)` | `80076068` (u0=0), `8007606C` (v0=0), `80076070` (w, s1=256), `80076078` (h, s5=256−y_off), `80076048/4C` |
| 2 | Illustration 8bpp | 0 | **H** | 128 | **256−H** | `GetClut(0, 489)` | `800760B8 subu v0,zero,s3` → v0-Byte = H, `800760D0` (v), `800760D4` (w=128), `800760DC` (h=s3), `800760A8/AC` |

Die Illustration ist eine 128×256-Leinwand, deren Motiv **exakt in den Zeilen H..255** liegt (visuell verifiziert an FILES_0014+0015 mit H=144 und FILES_0091+0092 mit H=176: Motiv beginnt bei Zeile H, unten bündig bei 255, kein Beschnitt).

**Endseiten-Marker** [B]: wenn `page == max_page` zusätzlicher Sprite x=280, y=110, u=56, v=12, w=42, h=14, `GetClut(256, 490)` (`800762C8`–`80076304`). *Optional* — der RE1.5-Port hat mit `emit_file_arrows`/`emit_file_footer` bereits einen eigenen Satz; nicht doppelt zeichnen.

**CLUT-Zielzeilen** [B]: Beide TIM-Köpfe tragen crect-Zeile **480**, das Spiel benutzt beim Zeichnen aber **489** (Illustration, 256 Einträge) und **490** (Textseite, 16 Einträge) — der Lader verschiebt sie beim Upload. Für ein PSX-Target zwingend beachten; für den PC-Port irrelevant.

## 3.4 Pflichtformate für eigene Dokumente [B]

**Textseite (4 bpp)**

```
Flags       0x08 (pmode 0 = 4bpp, CLUT-Bit)
CLUT-Block  len = 44,     rect (x=0, y=480, w=16,  h=1),  16 × BGR555
Bild-Block  len = 12 + 128*H, rect (x=0, y=0, w=64 Halbwörter, h=H)
Breite      IMMER 256 Pixel (= 64 Halbwörter)
Gesamt      8 + 44 + 12 + 128*H
Belegte H   112 → 14400 B · 128 → 16448 B · 144 → 18496 B · 176 → 22592 B
Pixelpackung  2 Pixel je Byte, niederes Nibble = linker Pixel
```

**Text-Palette (Eintrag 0..8 dokumentübergreifend identisch)** [B]:
`0 = 0x0000 (transparent)`, `1 = 0x677B`, `2 = 0x5F39`, `3 = 0x4EB5`, `4 = 0x4231`, `5 = 0x35CE`, `6 = 0x294A`, `7 = 0x8000`, `8 = 0x1063`; Rest `0x8000`-Füller (dokumentweise abweichend, z.B. doc23: `14 = 0x188C`, `15 = 0x34E6`).
⇒ Diese 9er-Rampe übernehmen, dann sieht das Antialiasing identisch aus. **Index 0 MUSS transparent bleiben** (der Port-Rasterizer verwirft CLUT-Wert 0, `inv_render_pc.c:653`).

**Illustration (8 bpp)** — bei allen 25 byte-identisch im Kopf:

```
Flags       0x09 (pmode 1 = 8bpp + CLUT)
CLUT-Block  len = 524,   rect (0, 480, 256, 1), 256 × BGR555
Bild-Block  len = 32780, rect (0, 0, 64, 256) = 128 Pixel breit, 256 hoch
Gesamt      IMMER 33312 B
Motiv       MUSS unten bündig liegen; obere (256−H) Zeilen = Index 0
```

## 3.5 Port-Einbau [B, Ist-Stand]

| Baustein | Ist | Soll |
|---|---|---|
| Prim-Schnittstelle | `re15_inv_op_t {kind, page, clut, abe, x,y,w,h(int16), u,v(uint8), r,g,b}`, `sprt(...)` @`re15_inv_screen.c:394-405`, `RE15_INV_MAX_OPS 768` | unverändert nutzbar |
| Seiten-Enum | `RE15_INV_PAGE_*` 0..6 (`re15_inv_screen.h:42-63`) | **+2**: `RE15_INV_PAGE_DOC4 = 7`, `RE15_INV_PAGE_DOCPAPER8 = 8` |
| CLUT-Selektoren | 0..16, `s_clut[17][256]` (`inv_render_pc.c:70`) | **+2**: 17 = Seiten-16er, 18 = Illustrations-256er; `s_clut[19][256]` |
| **Rasterizer-Falle** | `int v = (o->v + py) & 255; int u = (o->u + pxx) & 255;` (`inv_render_pc.c:642-644`) | Eine 256-px-breite Seite geht **nur mit u=0 und einem [256]-breiten Fenster**. Puffer: `uint8_t s_doc4[256][256]` (4bpp entpackt auf 1 B/Pixel) und `uint8_t s_docpaper8[256][128]`; Page-Selektor Z.645-651 um zwei Zweige ergänzen: `DOC4 → s_doc4[v][u]`, `DOCPAPER8 → (u<128 ? s_docpaper8[v][u] : 0)` |
| Transparenz | `if (c == 0) continue;` (Z.653) | deckt sich — beide RE2-CLUTs haben Eintrag 0 = `0x0000`. Keine Änderung |
| Leser | `emit_file_reader(e, st, x)` zeichnet nur Text (x, 0x20) + Fußzeile (`re15_inv_screen.c:656-661`, Aufrufkette Z.978-995, FILE-Zustand `25c1==2`) | Die zwei Bild-SPRTs **VOR** dem Text einreihen (Ops werden in Reihenfolge rasterisiert, später = oben): <br>`if (doc_has_image(doc)) { sprt(e, PAGE_DOCPAPER8, CLUT_DOCPAPER, ox, oy+H, 128, 256-H, 0, H, 128,128,128, 0); sprt(e, PAGE_DOC4, CLUT_DOCPAGE, ox, oy, 256, H, 0, 0, 128,128,128, 0); }` <br>Ursprung `ox/oy` **[O]** — neu festzulegen |

**RE1.5-Bestandsaufnahme** [B]:
* **RE1.5 hat keine Dokumentbilder.** Der gesamte CD-Baum `info/Re1.5/PSX/` enthält kein FILES-Äquivalent; `DATA/` führt nur AAA.TIM, CONFIG.TIM, CORE00.ESP, C_BACK2.TIM, ITEMALL.PIX, MAP01-0D.PIX, MIXITEM.PIX, ROOM115U/506U.SCD, SELECT*.TIM, ST_00.TIM, TEX.TIM, TITLEJ/U.TIM, TMOJI.TIM, TYPE00.TIM, YOUDIED.TIM. **Die Bild-Ebene ist ein NEUBAU, keine Reaktivierung.**
* **`ITPS.ITP` ist NICHT Dokument**: 884736 B = 72 Slots à 0x3000 B, je ein 8bpp-TIM 112×72, CLUT 256, prect (832,256,56,72), crect (0,489) — die CHECK-/Untersuchen-Fotos. Das **0x3000-Raster ist aber die beste Vorlage für einen eigenen Container mit fester Schrittweite.**
* **`STPIC_00..0F.TIM` ist NICHT Dokument**: 16 × 12492 B, 8bpp 116×103, crect (0,485), prect (640,406,58,103) — Status-Portraits.
* **RE1.5-FILE-Screen ist reiner Text**: Dokument-Blob @0x800CCD34 (DEBUG.BIN), 7 Seiten (`u16 @0x800CCD34 >> 1`), Leser `FUN@0x800C6CA0`, Glyphdrucker `FUN_80013160` / `0x80028F6C`, Ursprung x=0x28 y=0x20, Fußzeile zentriert `x = 0xA0 − breite/2`, y = 0xD2 (`re15_inv_screen.c:561-700`, `gen/inv_file_doc.inc`). **Textebene NICHT ersetzen** — sonst fällt die byte-true RE1.5-Darstellung weg; nur die Bild-Ebene ergänzen.
* **RE1.5-Listenumfang reicht**: 3 Listenseiten, Masken `u16[3] @0x800C6C98 = {0x0001, 0xFFFF, 0xFFFF}`, Zeilenbasen `u8[3] @0x800C7370 = {0x48, 0x52, 0x5C}`, **30 Zeilennamen** (Ids 0x48..0x65, Bänke @0x800C495C/0x800C4A28) — mehr als RE2s 24 Listenplätze und mehr als 25 Dokumente.

## 3.6 Werkzeuge

| # | Werkzeug | Stand | Inhalt |
|---|---|---|---|
| W1 | **Schneider** `re15_port/tools/re2_files_cut.py` | **GESCHRIEBEN + AUSGEFÜHRT** | Schneidet FILES.TIM anhand der drei EXE-Tabellen in 216 eigenständige TIMs + `toc.csv`. Ausgabe: `build/extracted/re2_files/` (5,3 MB, 216 `.TIM` + `toc.csv` mit 217 Zeilen). Namensschema `FILE<doc:02d>_<title\|pNN>_<page\|paper>.TIM`. Aufruf: `python re15_port/tools/re2_files_cut.py [zielverzeichnis]`. **Nichts mehr zu tun** |
| W2 | **PNG → Dokument-TIM** `doc_png_to_tim.py` | **FEHLT** | (a) 256×H RGB-PNG → 4bpp-TIM mit der 9-Farb-Rampe aus 3.4, Index 0 transparent; (b) 128×256 RGB-PNG → 8bpp-TIM mit 256er-Palette. ⛔ `tools/vram_png_to_tim.py` taugt **nicht**: es liest ein DuckStation-VRAM-PNG und rekonstruiert rohe VRAM-Halbwörter, es quantisiert kein beliebiges RGB-Bild (`--tpage/--clut`, Funktion `v15(x,y)`, keine Palettenbildung). Pflichtkonstanten s. 3.4 |
| W3 | **Packer + TOC-Generator** `doc_pack.py` | **FEHLT** | Einzel-TIMs wieder zu einem Container mit 0x800-Slot-Raster zusammenlegen UND die drei Tabellen als `.inc` erzeugen (Slot-Records, `doc→first_slot`, `doc-Record {max_page, 256−H}`). Muster: `re15_port/tools/gen_re2_ems_toc.py` → `engine/src/gen/re2_ems_toc.inc`. Ergebnis: `engine/src/gen/re2_filedoc_toc.inc` |
| W4 | **Textsatz** | optional | RE2-Seiten sind fertig gesetzter Text als Pixel (Schrift ca. 8×16, Großbuchstaben, Zeilenabstand 16, linksbündig ab x=0, Titel zentriert). Entweder eigene Bitmapschrift — oder **byte-true günstiger**: die RE1.5-Textebene weiterverwenden und nur die Illustration als Bild einspeisen |

**BMP-Querverweis** [B]: Die 216 bereits im Repo liegenden `FILES_NNNN.bmp` entsprechen 1:1 den 216 TIMs: `NNNN = 14 + laufender TIM-Index` (0-basiert). `FILES_0014.bmp` = doc0-Illustration, `FILES_0229.bmp` = doc24-Seite-15. Größenhistogramm: 16× 86070 (128×256), 28× 98358, 26× 110646 (256×144), 146× 135222 (256×176). Für Sichtprüfungen die vorhandenen BMPs nutzen, nicht neu dekodieren.

**Generierbare Konstanten** [B]:

```c
static const uint8_t re2_doc_first_slot[25] = {
    0,7,14,17,21,32,43,54,62,68,76,79,83,88,92,97,
    107,115,126,133,136,144,151,157,174 };          /* @0x800A9AD0 */
static struct { uint16_t max_page; uint8_t page_h; } re2_doc[25];
    /* max_page = u16 @0x800AA144+doc*4 ; page_h = 256 - u8 @+2   */
    /* NICHT const: doc23.max_page wird zur Laufzeit auf 1 gepatcht @0x80068368 */
```

---

# TEIL 4 — Reihenfolge der Umsetzung

Aufwand in Personentagen (PT), grob. Reihenfolge ist eine echte Abhängigkeitskette: Schritte 2–4 müssen **vor** jeder Hirn-Arbeit stehen, sonst ist nichts verifizierbar.

## Strang I — Endkampf

| # | Schritt | Aufwand | Abhängig von | Abnahme |
|---|---|---|---|---|
| **1** | **RE-Blocker schließen** (O-1, O-2, O-3). (a) `+0x10E & 0xC000`: PCSX-Redux-Watchpoint auf `Boss+0x10E` (Skill `re15-pcsx-watchpoint`) plus Xref-Suche `sh …,270(…)` in der EXE und im `room7040`-SCD. (b) `TAB_ACTION`-Handler `0x80103C18`/`0x80104440`/`0x801044FC`/`0x80104B94` disassemblieren + Setter von `+0x218 = 1,2` finden. (c) EM37 `0x801046AC` und `0x801048F4` disassemblieren. | **2–3** | — | Für jede der drei Lücken: Adresse + Instruktionen im Chat, bevor Code entsteht |
| **2** | **Lade-Tor + Parser-Feinschliff**: P1 (eigenes Boss-Tor statt `re15_re2_owns_type`), P2 (Hybrid-Gate), P4 (Sektionsgröße statt Dateirest), P5 (Opfer-Rig) | **0,5** | — | `re2_ems_load_bank(…, 0x36/0x37)` liefert 0; keine stderr-Meldung; `skel.keyframe_count == 1360 / 1207` |
| **3** | **Renderer P3**: Mesh-Schleife über Meshes statt Bones, Einheitsmatrix + Bind-Offsets aus 1.4 für Mesh 2..6, 3-CLUT-Stapelung der EM36-Textur | **1** | 2 | gdigrab-Verifikation (Skill `re15-port-visual-verify`, **nicht** `RE15_AUTOSHOT`/`RE15_SOFTWARE_RENDER`): 635-Vertex-Körper sichtbar, Mesh0/Mesh3 richtig eingefärbt |
| **4** | **Sound-Anbindung P6/P7**: `entity+0x1FA` führen, Bank 25 latchen, `re15_audio_re2_enemy_se` verdrahten | **0,5** | 2 | Ladeausgabe `[re2se] ENEMSE Bank 25 geladen: 15 VAGs, Map 32 Eintraege`; Unit-Test: 14 gerufene Ids `silent == 0`, Id 13 lautlos |
| **5** | **EM36 z0 + z1 Sub 0/Sub 1** (INIT, Hitboxen, Part-Kette, Patrouille, Sturmlauf, Finisher, `TAB_COMP_CMD`, `TAB_PERM_4x4`). Korrekturen K1, K2 einarbeiten | **2** | 1a, 2, 3, 4 | `re15-room-probe`-ctest: HP 600/400, `+0x1E8==2`, Entscheidungskaskade trifft alle 5 Zweige, Frame-Marken 0/7/10/15/55/61/85 |
| **6** | **EM36 z1 Sub 2** (15 Phasen, `jr`-Tabelle, Y-Skalierungen 4415/8830, Begleiter-Choreographie). Korrekturen K3, K5 einarbeiten | **1,5** | 5 | Phasenfolge 0→14 durchläuft; Timer-Marken exakt; Tentakel starten mit `model+0x8C == 0` |
| **7** | **EM36 z1 Sub 3 + Sub 4 + Opfer-Handler `0x80103908`**. Korrekturen K4, K7, K13 einarbeiten | **1,5** | 5 | Rammlauf: Befehl alle 32 Frames (nicht jeden); Fresstötung bindet den Spieler und friert terminal ein; `0x800CFC6E` bleibt nach p2 unverändert |
| **8** | **EM36 z2 + z3** (Trefferreaktion, Stagger-Akku, aufgeschobener/echter Tod, Blutfontänen, Explosion, Ausbleichen, Absinken). Korrekturen K6, K9, K10, K11, K12 einarbeiten | **2** | 5, 1a | Stagger löst bei kumulativ ≥15 aus und zerfällt mit 1/16 Frames; einzelner Treffer kostet **genau einen** Frame; Quirk K6 reproduziert (beide Wund-Sprites am selben Punkt) |
| **9** | **EM37-Hirn** (16 Verhalten, 8 Phasentabellen, 4 Schadensaufrufe, Griff-Handshake, Rotation/Skalierung statt Translation, Kind-Entity Typ 52, tote Zustände 2/3 auslassen). Korrektur K8, K14 | **3–4** | 1c, 5 | Tentakel folgen der Boss-Bone-Matrix; Biss-Fenster aus `0x80105730`; HP == −1; Boss-Rückmeldung `+0x228` |
| **10** | **Raum-Einbau**: Raumwahl, Verschiebung `(dx, dz)` für alle X/Z-Konstanten (1.6.3), Spawn der 5+4 Entities, Zeigertabelle `0x800CFE1C/20/24/28/2C`, Kampfstart `+0x1D4 \|= 1` | **1** | 5–9 | Voller Durchlauf ab EXE-Start (Skill `re15-playthrough`, **kein** Raumsprung): Sequenz → Sturmlauf → Rammlauf → Treffer → Tod |
| **11** | **BGM** (MAIN23 + SUB_2C/2F/30, Trailer-basiertes Container-Layout) | **0,5** | 4 | VH/VB-Offsets aus dem Trailer, kein Rechnen; `nVAG` = 2/9/11/17 |
| | **Zwischensumme Strang I** | **~16** | | |

## Strang II — Sound-Import (unabhängig von Strang I, kann parallel laufen)

| # | Schritt | Aufwand | Inhalt |
|---|---|---|---|
| **12** | **Gruppe A, reiner Code** (A1–A3, A6, A7) — 5 Posten, alle über die bereits residente CORE00-Bank | **1** | `pc_run_config`, `menu_common.c`, `item_modal_common.c`, `pc_run_memcard_screen`, Titel-FSM |
| **13** | **A4 + A5** — die zwei mit Vorbehalt | **0,5** | A4 als NACHRÜSTUNG kennzeichnen (kein RE2-Adressbeleg); A5 erst nach Klärung der DIEDEMO-CORE-Bank |
| **14** | **Gruppe B1/B2/B3** — **ein** Asset-Import: `CORE15.EDH` + `CORE15.VB` nach `shared_assets/RE2/`, neue Bank `RE15_SE_BANK_RE2SYS` | **1** | Deckt Save-Schreibmaschine, Save-Raum-Trias und den Player-Select-Tick in einem Zug |
| **15** | **Gruppe B4** — Item-Pickup-Fünfersatz (0x15/0x16/0x25/0x26/0x27) aus einem RE2-Raum-RDT als eigene VAB | **1** | Zugleich der Träger für C4 (`0x02290001`) aus dem Endkampf |
| **16** | **Gruppe C** — nicht bauen, nur als RE-Aufträge führen (C1 Tür-Bank-Architektur, C2 Tabellen-Aufrufer @0x800A242C, C3 nur bei Terminal-Rätsel) | **0** (+ je 0,5 RE, wenn beauftragt) | — |
| | **Zwischensumme Strang II** | **~3,5** | |

## Strang III — Dokument-Vorlagen (unabhängig)

| # | Schritt | Aufwand | Inhalt |
|---|---|---|---|
| **17** | **TOC-Generator W3 (Teil 1)**: die drei EXE-Tabellen als `engine/src/gen/re2_filedoc_toc.inc` erzeugen (Quelle: `build/extracted/re2_files/toc.csv`, W1 ist fertig) | **0,5** | 191 Slot-Records, 25 first-slot, 25 doc-Records |
| **18** | **Renderer-Ebene**: 2 Page-Ids, 2 CLUT-Ids, `s_clut[19][256]`, zwei Fenster-Puffer, Page-Selektor-Zweige, `emit_file_reader`-Erweiterung. Rasterizer-Falle `&255` beachten | **1** | Abnahme: doc0 (H=144) und doc9 (H=176) sehen aus wie die Referenz-Komposits |
| **19** | **Autorenwerkzeug W2**: `doc_png_to_tim.py` (4bpp + 8bpp), feste Palette, Header-Konstanten aus 3.4 | **1** | Round-Trip: eigenes PNG → TIM → Renderer, pixelgleich zur Vorlage |
| **20** | **Packer W3 (Teil 2)**: 0x800-Slot-Raster, Container schreiben | **0,5** | Slot-Kette geht auf; Slot n+1 = nächste 0x800-Grenze |
| **21** | *(optional)* **W4 Textsatz** oder Beibehaltung der RE1.5-Textebene | **0–2** | Empfehlung: RE1.5-Textebene behalten, nur Illustration als Bild — spart W4 vollständig |
| | **Zwischensumme Strang III** | **~3 (+2 optional)** | |

## Gesamt

| Strang | PT |
|---|---|
| I — Endkampf 0x36/0x37 | ~16 |
| II — Sound-Import | ~3,5 |
| III — Dokument-Vorlagen | ~3 (+2 optional) |
| **Summe** | **~22,5 PT** (ohne die optionalen Posten und ohne die C-Gruppe) |

**Kritischer Pfad:** 1 → 2 → 3 → 5 → 6/7/8 → 9 → 10. Schritt 1 ist der einzige echte Blocker: ohne O-1 ist unklar, ob der Boss überhaupt Schaden nimmt, und damit sind die Schritte 8 (z2/z3) und die halbe Abnahme von 10 unverifizierbar.

**Nicht anfangen mit:** Schritt 5 vor Schritt 3 — ohne die Mesh-Schleife ist der Boss zu 74 % unsichtbar (Mesh 2 = 635 von 963 Faces), und jede visuelle Verifikation liefert ein falsches Ergebnis.