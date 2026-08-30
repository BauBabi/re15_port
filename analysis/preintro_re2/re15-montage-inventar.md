# RE1.5 Pre-Intro (ROOM1240-Montage) — Inventar für die RE2-Standbild-Übernahme

Status: FERTIG (alle 4 Liefergegenstände belegt)
Datum: 2026-08-30
Zweck: Exaktes Inventar der bestehenden Pre-Intro-Montage (Cuts, BSS-Bilder, Texte, Treiber, Port-Präsentation),
damit die RE2-Standbild-Präsentation 1:1 auf unseren Inhalten aufsetzen kann. KEINE Code-Änderungen.

## 0. Quellenlage (Vorarbeiten)

- Memory `reai-v2-cut-opcodes.md` — byte-true 1240-Montage (SUB2), Opcode 0x29/0x2A Cut-Freeze via `DAT_800aca3c|0x100`
- `re15_port/platform/pc/main.c` — PRE-INTRO-Blöcke
- `re15_port/engine/src/scd_vm.c` — op 0x29/0x2a
- `re15_port/engine/src/room_common.c` — Raum-/Cut-Verwaltung
- RDT: `re15_port/shared_assets/PSX/STAGE1/ROOM1240.RDT` (+ BSS)

## 1. Asset-Inventar (verifiziert am Dateisystem)

- RDT: `re15_port/shared_assets/PSX/STAGE1/ROOM1240.RDT` (Original-Quelle `info/Re1.5/PSX/STAGE1/ROOM1240.RDT`)
- BSS-Standbilder: `re15_port/shared_assets/PSX/BSS/ROOM1240/BG00.BSS … BG08.BSS` — **9 Stück, je exakt 65536 B**
  (= 1 MDEC-VLC-Chunk pro Kamera-Cut, 320×240, VLC-ID 0x3800; RE15_KNOWLEDGE §1.3)
- Voice: `re15_port/shared_assets/PSX/VOICE/r1240_m00..m05.vag` + `.xa` — **6 Erzähler-Takes**, Benennung `m<msg-id>`
  → 1:1-Kopplung Message-Id ↔ Voice-Take (Port-Hook op 0x2B Message_on, RE15_KNOWLEDGE §Voice).

### 1.1 Referenz-Captures (Original MZD, frame-by-frame verifiziert 2026-07-02)
`stage_saves/mzd_preintro_verify/`:
- `contact_ORIGINAL.png` — Kontaktbogen der Original-Sequenz (o00..o25, L1-Screenshot alle ~2,2 s)
- `COMPARE_port_vs_original.png` — Port vs. Original nebeneinander (black/zombie/T-Virus/S.T.A.R.S/Umbrella)

**Gesichtete Original-Sequenz (Kontaktbogen):**
| Phase | Bild | Text sichtbar |
|---|---|---|
| o00 | Player-Select (Leon S. Kennedy) | — |
| o01–o08 | SCHWARZ (langes Opening, ~15 s auf dem Bogen) | keiner |
| o09–o11 | Zombie-Gesicht (grau, Profil) | "A bizarre incident occurred in the outskirts of an American suburb called Raccoon City." |
| o12–o14 | T-Virus-Kulturen (gelblich, Klumpen) | "It was later revealed that the terrible disaster had been caused by the T-Virus," |
| o15–o17 | dieselben Kulturen | "a mutagenic toxin created by the international corporation Umbrella, incorporated." |
| o18 | S.T.A.R.S.-RACCOON-POLICE-DEP-Abzeichen | "The Raccoon police department special S.T.A.R.S Unit began an immediate investigation." |
| o19–o20 | dunkle Foto-Szene (Person an Tür/Anlage) | (derselbe S.T.A.R.S.-Text bleibt stehen) |
| o21 | Helikopter + Explosion/Feuerball | "The case was apparently closed thanks to the efforts of S.T.A.R.S members Chris Redfield and Jill Valentine." |
| o22–o23 | S.T.A.R.S.-Gruppe vor Helikopter | (Text bleibt stehen) |
| o24 | Industrie-/Labor-Anlage s/w | "But the umbrella corporation's experiments were far from finished." |
| o25 | Umbrella-Logo (rot/grau Schirm) | (Text bleibt stehen) |

→ Messages ÜBERLEBEN den Cut-Wechsel (Message-Anzeige läuft weiter, während Cut_chg das Bild tauscht).

## 2. Treiber laut Vorarbeit (Memory `reai-v2-cut-opcodes`, RE'd 2026-06-28 — wird unten gegen die RDT-Bytes nachgeprüft)

- ROOM1240 Pre-Intro = **SUB2** (sub_scd-Tabelle @RDT+0x44). Kette: room-load → main00 (`Door_aot_set`+`Evt_end`) → SUB0 `04 FF 18 02` = Evt_exec(typ 0x18, event 2) → **SUB2**.
- SUB2 = 9-Cut-Montage mit 6 Messages:
  `Cut_chg(0)=schwarz sleep 0xA0` → `Cut_chg(1) sleep 0x14 Message_on(0) sleep 0xB4` → `Cut_chg(2) Message_on(1) Message_on(2)` → `Cut_chg(3) Message_on(3)` → `Cut_chg(4)` → `Cut_chg(5) Message_on(4)` → `Cut_chg(6)` → `Cut_chg(7) Message_on(5)` → `Cut_chg(8) Aot_on(0) Evt_end`.
- Vor jedem Cut `Set(2,0x07,0/1)`-Paare (Flag-Toggles, Letterbox/Cine-Flag (2,7)).
- **Cut-Freeze-Mechanik**: Opcode 0x29 Cut_chg → `FUN_800402a0` @0x800402a0 (Tabelleneintrag 0x8007454c = Index 0x29 der Dispatch-Basis `PTR_LAB_800744a8`): setzt `DAT_800aca3c |= 0x100` (BG-Renderer-Freeze), sichert alten Cut in `DAT_800b3f7b`, schreibt `DAT_800b0fe4 = cut`, ruft `FUN_800142f4()` (einmal zeichnen). Opcode 0x2A Cut_old → `FUN_8004032c`: Cut zurück + `DAT_800aca3c &= ~0x100`.
  Leser: disp-Hauptfunktion EXE @0x8001cce0 — Bit 0x100 gesetzt ⇒ Live-BG/MDEC-Renderer `FUN_80014230` wird NICHT aufgerufen ⇒ Standbild bleibt stehen.
- Dynamischer Beleg (DuckStation-Savestate im 1240-Briefing): `DAT_800aca3c=0x110`, `DAT_800b0fe4=0` (Cut 0 = schwarz), Fade offen `DAT_800b5568=0xf0`.

## 3. SCD-Walk ROOM1240.RDT — NACHGEPRÜFT an den Bytes (2026-08-30)

RDT `re15_port/shared_assets/PSX/STAGE1/ROOM1240.RDT`, 163744 B. Header: **nCut=9**, nDoor=0, nItem=0, nOmodel=0, nSprite=0.
Sektions-Offsets (Header +0x20…): camera=0x060, zone=0x180, light=0x238, collision=0x3EC, block=0x4F8, floor=0x508,
**mainScd=0x518, subScd=0x53C, extraScd=0x624, message=0x62C**, effect=0x858, espTim=0x24B00.

Disassembler: eigener Walk mit der byte-true Längen-Tabelle aus `re15_port/engine/src/scd_vm.c` `s_opcode_sizes`
(disasm-verifiziert gegen die Dispatch-Tabelle `PTR_LAB_800744a8`). Skript: Scratchpad `scd_walk_1240.py`.

### 3.1 main00 @file 0x51A — nur die Tür
```
main0+0x0000 (file 0x00051A): Door_aot_set  3B 00 02 31 00 00 00 00 00 00 00 00 00 00 9A 99 00 00 EB F0 00 00 00 17 00 00 00 00 00 00 00 00
main0+0x0020 (file 0x00053A): Evt_end       01 00
```
(Decodierung der Door-Felder in §3.4.)

### 3.2 sub_scd-Tabelle @0x53C — 4 Subs: ptrs 0008 000E 001E 00E0
- **sub0** @0x544: `Evt_exec 04 FF 18 02` → startet **SUB2** (Typ 0x18, Event 2); `Evt_end`.
- **sub1** @0x54A: `Ifel_ck(00,0A,00)` + `51 01 40 00` (Key-Check) → `Evt_exec 04 FF 18 03` (SUB3) → `Endif` → `Evt_end`. (Alternativpfad: Montage überspringen → sofort Tür scharf, siehe sub3.)
- **sub3** @0x61C: `Sleep(1)` → `Aot_on(0)` → `Evt_end` — der Skip-Pfad: Tür sofort aktivieren.

### 3.3 SUB2 @file 0x55A — die komplette Montage (0xC2 Bytes), Opcode für Opcode

| file-Offset | sub2+ | Opcode | Bytes | Bedeutung |
|---|---|---|---|---|
| 0x00055A | +0x000 | Set | `22 03 8B 01` | Flag (Bank 3, Id 0x8B) = 1 (Story-Flag „Pre-Intro läuft/gesehen") |
| 0x00055E | +0x004 | Set | `22 01 1B 01` | Flag **(1,27) = 1** — Cine-Modus AN (Player-Control aus; main.c:1236 keyt darauf) |
| 0x000562 | +0x008 | Set | `22 02 07 01` | Flag **(2,7) = 1** — Letterbox/Cine-Balken AN |
| 0x000566 | +0x00C | **Cut_chg(0)** | `29 00` | → BG00 (SCHWARZ), Freeze `DAT_800aca3c|=0x100` |
| 0x000568 | +0x00E | Sleep | `09 0A A0 00` | **160 Ticks** (~5,3 s @30fps) schwarzes Opening, KEIN Text |
| 0x00056C | +0x012 | Set | `22 02 07 00` | (2,7)=0 — 2-Tick-„Blink" des Cine-Flags zwischen den Bildgruppen |
| 0x000570 | +0x016 | Sleep | `09 0A 02 00` | 2 Ticks |
| 0x000574 | +0x01A | Set | `22 02 07 01` | (2,7)=1 |
| 0x000578 | +0x01E | **Cut_chg(1)** | `29 01` | → BG01 (Zombie-Gesicht) |
| 0x00057A | +0x020 | Sleep | `09 0A 14 00` | 20 Ticks Bild ohne Text |
| 0x00057E | +0x024 | **Message_on(0)** | `2B 00 00 00` | msg 0 „A bizarre incident …" (+ Voice r1240_m00) |
| 0x000582 | +0x028 | Sleep | `09 0A B4 00` | 180 Ticks |
| 0x000586 | +0x02C | Set/Sleep/Set | (2,7) 0→2 Ticks→1 | Blink |
| 0x000592 | +0x038 | **Cut_chg(2)** | `29 02` | → BG02 (T-Virus-Kulturen) |
| 0x000594 | +0x03A | Sleep | 20 Ticks | |
| 0x000598 | +0x03E | **Message_on(1)** | `2B 01 00 00` | msg 1 „It was later revealed … T-Virus," |
| 0x00059C | +0x042 | Sleep | 180 Ticks | |
| 0x0005A0 | +0x046 | **Message_on(2)** | `2B 02 00 00` | msg 2 „a mutagenic toxin …" — **GLEICHES Bild, zweiter Text** |
| 0x0005A4 | +0x04A | Sleep | 180 Ticks | |
| 0x0005A8 | +0x04E | Set/Sleep/Set | Blink (2,7) | |
| 0x0005B4 | +0x05A | **Cut_chg(3)** | `29 03` | → BG03 (S.T.A.R.S.-Abzeichen) |
| 0x0005B6 | +0x05C | Sleep | 20 Ticks | |
| 0x0005BA | +0x060 | **Message_on(3)** | `2B 03 00 00` | msg 3 „The Raccoon police department …" |
| 0x0005BE | +0x064 | Sleep | **90 Ticks** | |
| 0x0005C2 | +0x068 | **Cut_chg(4)** | `29 04` | → BG04 — **KEIN Blink, KEINE neue Message: Text von msg 3 bleibt stehen** |
| 0x0005C4 | +0x06A | Sleep | 90 Ticks | |
| 0x0005C8 | +0x06E | Set/Sleep/Set | Blink (2,7) | |
| 0x0005D4 | +0x07A | **Cut_chg(5)** | `29 05` | → BG05 (Heli + Explosion) |
| 0x0005D6 | +0x07C | Sleep | 20 Ticks | |
| 0x0005DA | +0x080 | **Message_on(4)** | `2B 04 00 00` | msg 4 „The case was apparently closed …" |
| 0x0005DE | +0x084 | Sleep | 90 Ticks | |
| 0x0005E2 | +0x088 | **Cut_chg(6)** | `29 06` | → BG06 — Text bleibt stehen |
| 0x0005E4 | +0x08A | Sleep | 90 Ticks | |
| 0x0005E8 | +0x08E | Set/Sleep/Set | Blink (2,7) | |
| 0x0005F4 | +0x09A | **Cut_chg(7)** | `29 07` | → BG07 (Labor/Anlage) |
| 0x0005F6 | +0x09C | Sleep | 20 Ticks | |
| 0x0005FA | +0x0A0 | **Message_on(5)** | `2B 05 00 00` | msg 5 „But the umbrella corporation's …" |
| 0x0005FE | +0x0A4 | Sleep | **80 Ticks** (0x50) | |
| 0x000602 | +0x0A8 | **Cut_chg(8)** | `29 08` | → BG08 (Umbrella-Logo) — Text bleibt stehen |
| 0x000604 | +0x0AA | Sleep | 80 Ticks | |
| 0x000608 | +0x0AE | Set/Sleep/Set | Blink (2,7); endet mit (2,7)=1 | |
| 0x000614 | +0x0BA | Sleep | 90 Ticks | |
| 0x000618 | +0x0BE | **Aot_on(0)** | `47 00` | Tür-AOT 0 scharfschalten → Übergang nach ROOM1170 |
| 0x00061A | +0x0C0 | Evt_end | `01 00` | |

**Sleep-Kodierung** (byte-true, `op_sleep` scd_vm.c:1051 / PSX @0x8003F3E0): `09 0A <lo> <hi>` — Byte 1 ist bereits das
`Sleeping`-Opcode 0x0A, die Dauer ist **LE u16** dahinter. `09 0A A0 00` = **160 Ticks**. `Sleeping` (LAB_8003f428) yieldet
und liefert exakt N Ticks. SCD-VM tickt mit **30 Hz** (main.c: bei 60-fps-Target jeder 2. Frame).

**Timing-Gerüst:** Cut 0 (schwarz) 160 T; Cuts mit neuer Message: 20 T Bild allein → Message_on → 180 T (Cuts 1–2) bzw. 90/80 T (Cuts 3,5,7); Message-lose Zwischen-Cuts (4,6,8) je 90/80 T mit STEHENDEM Text; die (2,7)-Blinks je 2 T. Summe = **1422 Ticks ≈ 47,4 s @30 Hz**. Warte-Mechanik ist ausschließlich `Sleep` (Timer) — **kein** Tasten-Confirm, kein PAGE_WAIT im Treiber selbst.

**Cut-Gruppen durch das (2,7)-Blink-Muster:** [0] [1] [2] [3+4] [5+6] [7+8] — das 2-Tick-Aus/An des Cine-Flags markiert jede BILDGRUPPEN-Grenze (= dort, wo im Original der Text wechselt).

## 4. Erzähler-Texte — WORTLAUT aus dem RDT-Message-Block @0x62C

Pointer-Tabelle: 6 Messages, ptrs 000C 0069 00BF 0116 0172 01E4. Kodierung: RE1.5-Charset (A=0x1D…, a=0x3D…, 0x00=Space, 0x08=Zeilenumbruch, 0x57='.', 0x18=',', 0x3B='-'; Steuer: 0x04=…, Ende `01 01`+Pad).

| Id | file-Offset | Wortlaut (Zeilenumbrüche = `\n`) | Voice |
|---|---|---|---|
| 0 | 0x000638 | "A bizarre incident occurred in the\noutskirts of an American suburb called\nRaccoon City." | r1240_m00 |
| 1 | 0x000695 | "It was later revealed that the terrible\ndisaster had been caused by the T-Virus," | r1240_m01 |
| 2 | 0x0006EB | "a mutagenic toxin created by the\ninternational corporation Umbrella\nincorporated." | r1240_m02 |
| 3 | 0x000742 | "The Raccoon police department special\nS.T.A.R.S Unit began an immediate\ninvestigation." | r1240_m03 |
| 4 | 0x00079E | "The case was apparently closed thanks\nto the efforts of S.T.A.R.S members\nChris Redfield and Jill Valentine." | r1240_m04 |
| 5 | 0x000810 | "But the umbrella corporation's\nexperiments were far from finished." | r1240_m05 |

Raw-Rahmung jeder Message: `04 00 <text> 57 04 01 01 AF` — führendes `04` + Space, `57`='.', Terminator `04 01 01` + Pad `AF`.
Die Wortlaute stimmen 1:1 mit dem Original-Kontaktbogen (`contact_ORIGINAL.png`) überein.

## 3.4 Door-AOT (der Übergang nach ROOM1170) — Byte-Decode

main00 @file 0x51A: `3B 00 02 31 | 0000 0000 0000 0000 | 9A99 0000 EBF0 0000 | 00 17 | 00 …`
Felder nach `op_door_aot_set` (scd_vm.c:3433, byte-true LAB_800405bc):
- slot=0, sce=2 (scharfe Tür), pc[3]=0x31 (Floor-Byte; Bit 0x80 clear → 32-B-Form)
- **Trigger-Rect x=0 z=0 w=0 d=0 — NULL-Rechteck am Ursprung** (die „Selbst-Tür" der Montage: unbetretbar per Lauf, wird per `Aot_on(0)` + Skript scharf)
- Spawn im Zielraum: next (x,y,z) = (**-26214**, 0, **-3861**), dir_y=0, target_cut pc[24]=0
- Ziel: pc[22]=stage 0 (STAGE1), pc[23]=**room 0x17 → ROOM1170**

## 3.5 Skip-Mechanik (byte-true) — SUB1 ist das per-Frame-Skript

Per-Frame-Modell (Memory `reai-v2-scd-per-frame-model`): Slot 1 wird in JEDEM Gameplay-Frame mit **sub01** neu geseedet (FUN_8003f038). sub01 @0x54A:
```
Ifel_ck 06 00 0A 00
Sce_key_ck 51 01 40 00      ; Prädikat LAB_80042920: (mask 0x0040 & DAT_800ac768[HELD, virtuell]) — param 1
Evt_exec  04 FF 18 03       ; → SUB3
Endif; Evt_end
```
**Virtuelles Pad-Bit 0x0040 = RAW SQUARE** (Preset-Tabelle @0x80073dbc, Index 6; Port `re15_pad_virtual_word`, pad_common.c).
SUB3 @0x61C: `Sleep(1) → Aot_on(0) → Evt_end`.
⇒ **Skip = □/SQUARE gehalten** → Tür-AOT sofort scharf → Raumwechsel nach 1170; die Montage (SUB2) läuft dabei nicht zu Ende, ihr Thread stirbt mit dem Raumwechsel. Kein „X drücken", kein Fade — Skip ist einfach „Tür feuert früher".

## 3.6 Port-Umsetzung der Cut-Opcodes (scd_vm.c — byte-true, Stand heute)

- `op_cut_chg` (scd_vm.c:1268): merkt `cam_id_prev = work_vars[0x0A]` (ANGEZEIGTER Cut, @0x800402c0/@0x800402e4), setzt `cam_id = pc[1]`, `cam_change_pending=1`, Work-Var-Umlauf `work_vars[0x0C]=work_vars[0x0A]; work_vars[0x0A]=neuer Cut` (@0x800402ec/@0x800402fc), und **`cut_auto_enabled = 0`** = Port-Inverse von `DAT_800aca3c |= 0x100` (@0x800402d4). Der Freeze ist damit „RVD-Auto-Kamera-Scan AUS", das Bild bleibt auf dem gesetzten Cut.
- `op_cut_old` (scd_vm.c:4043): stellt `cam_id_prev` wieder her, Work-Var-Umlauf (@0x8004035c/@0x80040364), **`cut_auto_enabled = 1`** (= Bit 0x100 clear @0x8004032c).

## 5. Die 9 BSS-Standbilder — DEKODIERT und GESICHTET (2026-08-30)

Dekodiert mit der bereits gebauten Engine-Probe `re15_port/build/tests/unit/probe_1090_bgdiff.exe`
(Kette `re15_bss_parse_chunk → re15_bss_vlc_decode → re15_bss_mdec_decode`, 320×240 RGB).
Ausgabe: `analysis/preintro_re2/bg/bg00..bg08.png` + Kontaktbogen `analysis/preintro_re2/bg/contact_BG.png`.

**Die 9 Kameras sind ALLE byte-identisch** (RDT camera-Block @0x60, je 0x20 B; nur der letzte u32 = sprite.pri-Offset zählt
0x3A0…0x3C0 hoch): pos(-12834,-3114,-9774) tgt(-7794,-2196,-22446) — die **VOID-Kamera**, byte-identisch zu ROOM1170 Cut 7
(main.c:4610). ⇒ Alle 9 Cuts unterscheiden sich AUSSCHLIESSLICH im Hintergrundbild; es gibt keine Kamerafahrt, keine Geometrie.
(cam8 hat als einziges Byte[0] = 01 statt 00 = Kamera-Flag.)

| Cut | Datei | Größe | VLC-Koeff. | Ø-Helligkeit | Bildinhalt (gesichtet) | Text auf dem Bild |
|---|---|---|---|---|---|---|
| 0 | `BSS/ROOM1240/BG00.BSS` | 65536 | 7304 | **1,0 (SCHWARZ)** | reines Schwarz (max. Pixelsumme 3) | keiner |
| 1 | `BG01.BSS` | 65536 | 56456 | 32,5 | **Zombie-Kopf im Profil**, s/w, Gitter/Geländer im Hintergrund | msg 0 |
| 2 | `BG02.BSS` | 65536 | 69000 | 88,4 | **T-Virus-Mikroskopaufnahme**: 3 grün-rote Zellklumpen auf gelb-olivem Grund | msg 1, dann msg 2 |
| 3 | `BG03.BSS` | 65536 | 53896 | 16,5 | **S.T.A.R.S.-Abzeichen** „RACCOON POLICE DEP" mit 3 gelben Sternen auf Schwarz | msg 3 |
| 4 | `BG04.BSS` | 65536 | 61960 | 9,8 | **Arklay-Villa-Szene**: 2–3 S.T.A.R.S.-Mitglieder mit Waffen vor einem Tor/Portal, dunkel | msg 3 bleibt stehen |
| 5 | `BG05.BSS` | 65536 | 60040 | 44,3 | **Helikopter über Explosion/Feuerball** (Sprengung des Herrenhauses), s/w Nacht | msg 4 |
| 6 | `BG06.BSS` | 65536 | 62984 | 58,5 | **S.T.A.R.S.-Gruppe vor Helikopter** bei Tageslicht, Presse/Zuschauer im Vordergrund | msg 4 bleibt stehen |
| 7 | `BG07.BSS` | 65536 | 79624 | 21,2 | **Umbrella-Laboranlage von innen**, Rohre/Konsolen/Maschinerie, sehr dunkel | msg 5 |
| 8 | `BG08.BSS` | 65536 | 31624 | 19,1 | **Umbrella-Logo** (rot-weißer Schirm) auf Schwarz | msg 5 bleibt stehen |

Die Zuordnung stimmt Bild für Bild mit dem Original-Kontaktbogen `stage_saves/mzd_preintro_verify/contact_ORIGINAL.png`
(o09→BG01, o12→BG02, o18→BG03, o19/o20→BG04, o21→BG05, o22/o23→BG06, o24→BG07, o25→BG08).

**Musik/Voice:** BGM kommt über den Raumlader (Port: `re15_audio_start_room_bgm` mit der ECHTEN Raum-ID, main.c:3326 —
das frühere hartkodierte ROOM1170-BGM war der 2026-08-01-Bug). Sprecher: `VOICE/r1240_m00..m05` (VAG + XA), gekoppelt an
die Message-Id über den Message_on-Voice-Hook. Im SCD selbst steht **kein** Se_on/Sce_bgm_control — die Montage ist rein
Bild + Text + Timer.

## 6. Wie der PORT es heute präsentiert

### 6.1 Bild-Tausch = HARTER CUT, kein Fade
- SCD `op_cut_chg` setzt nur `g_scd.cam_id` + `cam_change_pending` (scd_vm.c:1268) und schaltet `cut_auto_enabled=0`.
- Der PC-Loop liest das Pending und lädt den neuen Hintergrund synchron: `re15_bg_load_cut((int)g_scd.cam_id)`
  (main.c:3559 für den Boot-Cut; im Frame-Loop derselbe Weg im Cut-Wechsel-Block). Der BG-Cache wird ab Frame 0 geblittet.
- **Es gibt KEINEN Fade zwischen den Montage-Bildern.** `re15_render_pc_set_fade(0)` wird pro Frame genullt (main.c:4599-4602);
  Raum-Fades laufen ausschließlich über die Fade-Kanäle (`g_fade_ch`, `re15_fade_tick`) und werden hier von nichts getriggert.
  Der Bildwechsel ist also ein **1-Frame-Hartschnitt** — byte-true zum Original, wo `Cut_chg` `FUN_800142f4()` einmal zeichnet.
- Load-Fehlschlag ⇒ **SCHWARZ HALTEN** (`re15_bg_invalidate()`, main.c:3559-3564), byte-true zu @0x8001d620-28/@0x8001dadc-ec;
  der alte TEST.BSS-Fallback (= byte-identisch mit ROOM1170/BG00 = Helipad) ist ersatzlos gestrichen.

### 6.2 Text-Rendering
- `Message_on` (0x2B) blockiert den Thread NICHT: `msg_show` setzt `g_scd.message_id/arg2/arg3/message_active` (scd_vm.c:1330 ff.).
  Die Anzeigedauer kommt aus dem per-Message berechneten Provider (`re15_scd_set_msg_duration_provider`, PSX-Dialog-FSM @0x80028134),
  Fallback 90 Frames. Deshalb bleibt ein Text stehen, während `Cut_chg` das Bild darunter tauscht — genau das Original-Verhalten.
- Gerendert wird über das Text-Overlay des PC-Renderers (Layer-Reihenfolge siehe Skill `re15-pc-render-order`: Framebuffer → … →
  Letterbox → Text-Overlay → Fade). Volltext-Blöcke, nicht Typewriter (RE'd: der Volltext-Printer liegt im DEBUG.BIN-Overlay
  @0x800C0000; Original-Capture bestätigt volle Caption-Blöcke).
- **Letterbox-Balken** kommen aus dem Cine-Flag **(2,7)**, das SUB2 selbst setzt/löscht; der Port keyt Letterbox + player_mode
  auf die Cine-Flags (1,27)/(2,7) (main.c:1236) statt auf das Original-Player-ROUTINE-Byte `DAT_800aca58` — dokumentierte,
  beobachtbar identische Divergenz.

### 6.3 Skip
- Es gibt **keinen eigenen Port-Skip-Code**. Der Skip ist datengetrieben: sub01 wird jeden Frame neu geseedet, prüft
  `Sce_key_ck(1, 0x0040)` = □/SQUARE gehalten (`op_sce_key_ck`, scd_vm.c:4343, liest `g_scd_pad_held` = virtueller Pad-Word) und
  feuert dann SUB3 → `Aot_on(0)` → Tür → ROOM1170.

### 6.4 Übergang nach ROOM1170 (Handoff)
- SUB2 endet mit `Aot_on(0)`; die Tür-AOT (main00, dest STAGE1/room 0x17) lädt ROOM1170.
- In ROOM1170 läuft dann `main00 → Evt_exec(0x180B) → sub11` = der zweite Narrator-Block auf **Cut 7** (der VOID-Kamera,
  byte-identisch zu den 9 Montage-Kameras) mit Messages 8–11. Der Port erkennt das über den `s_preintro`-Latch
  (main.c:3485-3488: „ein laufender Event-Slot-Thread nach dem Boot-Tick = Pre-Intro") und hält sub00 (Helipad-Cinematic)
  zurück, bis sub11 endet (main.c:3836-3860, heute nur noch FALLBACK — der byte-true Weg ist sub11s eigenes `Aot_on(3)` →
  Selbst-Tür-Reenter, Latch `g_scd_self_reenter_fired`).
- Der Narrator-Schwarzhintergrund in 1170 entsteht NICHT durch einen Fill, sondern durch die VOID-Kamera + Near-Clip
  (`wz<0 → cull`) plus das schwarz dekodierende BG07; `re15_render_pc_set_scene_black` ist nur ein redundantes Sicherheitsnetz,
  exakt auf `room==0x1170 && cut==7` gegated (main.c:4604-4634).

## 7. Kurz-Fazit für die RE2-Übernahme

Unsere Montage ist bereits genau die Form, die RE2 später verwendet: **9 Standbilder auf einer Dummy-Kamera, harte Schnitte,
Timer-gesteuert, Texte überleben den Bildwechsel**. Was für eine RE2-Präsentation als Anknüpfungspunkte existiert:
- Inhalt: 9 BSS-Bilder (§5) + 6 Texte (§4) + 6 Voice-Takes — vollständig vorhanden, nichts muss neu erzeugt werden.
- Timing: reine `Sleep`-Kette (§3.3), 160/20/180/90/80-Ticks, ≈48 s gesamt.
- Präsentation heute: harter Cut ohne Fade (§6.1), Volltext über Letterbox (§6.2), Skip = □ halten (§6.3).
- Freie Stellschrauben für eine RE2-Angleichung wären damit ausschließlich die PRÄSENTATION (Fade/Überblendung zwischen den
  Stills, Text-Ein-/Ausblendung, Letterbox-Form) — Sequenz, Bilder, Texte und Timing sind byte-true festgelegt und dürfen
  sich nicht ändern.
