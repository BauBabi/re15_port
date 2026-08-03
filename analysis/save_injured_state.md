# Verletzungszustand im Savegame (HP / Injured-Anims / Blut-Decals) — Original vs. Port

**Datum:** 2026-08-03 · **Status:** Original byte-belegt (Disasm + Binary-Scans), Port GEMESSEN (`probe_save_injured`)
**Quellen:** `info/Re1.5/PSX.EXE` (t_addr 0x80010000, Datei-Offset = addr−0x80010000+0x800), `RE_15_Quellcode_V2/FUN_{80025c00,80026f48,80026e54,800271a8,80027368,8004ee38,800314b0}.c`, `ghidra1_V2.txt` (Xref-Zensus), `info/re2leon/COMMON/BIN/MEM_CARD.BIN`, `re15_port/` (savedata/memcard/damage/gameflow/main), `analysis/blood_decals.md`, `analysis/leon_injured_model.md`, `re15_port/tests/unit/probe_save_injured.c`.
**Nutzer-Frage:** „Speichert das Spiel die Schadensanimationen/-texturen der Charaktere (Caution/Danger) mit? Oder haben sie nach dem Laden wieder ihre unverletzten Texturen/Animationen? Wenn letzteres: falsch."

---

## 0. Executive Answer

**Es gibt zwei getrennte Mechanismen, und die Antwort ist für beide verschieden:**

1. **Injured-ANIMATIONEN (Caution/Danger-Idle):** werden in KEINEM Save als solche gespeichert — sie sind eine reine **Laufzeit-Funktion der HP** (Schwellen `slti 0x32 @0x800321a0` / `slti 0x1e @0x800322a4`). Der **Port speichert und restauriert HP** (`re15_savedata_t.player_hp`) → nach dem Laden sind die Verletzt-Anims **automatisch korrekt** (gemessen: hp=30 Roundtrip, `player_common.c:551-590` wählt pro Frame). **Hier ist nichts falsch.** Pikant: RE1.5s EIGENES (dormantes) Save speichert HP **nicht einmal** — der Spieler-Load setzt HP fix auf 100 (`@0x80031718`); das HP-Restore des Ports ist das RE2-Verhalten, das per Nutzer-Mandat übernommen wurde.
2. **Blut-DECALS auf der Spieler-Textur** (Wund-Tabelle `@0x800b10ec`, `analysis/blood_decals.md`): **RE1.5s Save-Blob ENTHÄLT die Wund-Tabelle** (sie liegt bei GSB+0x130, mitten im gespeicherten Block — Byte-Beleg §2). Der **Port speichert sie NICHT** (gemessen: Save-Block mit Blut == Save-Block ohne Blut, byte-identisch) → **nach dem Laden ist Leon unblutig, obwohl er blutig gespeichert wurde** (gemessen). Das ist die Lücke aus der Nutzer-Frage → Fix-Plan §6. Zusätzlich gefunden: ein echter **Port-BUG** — der CONTINUE-Pfad resetted die Wund-Tabelle nie, ein nach Tod geladener SAUBERER Save startet mit dem Blut des gestorbenen Runs (§5, gemessen).

**Ehrliche Einordnung des Originals:** Im ausgelieferten MZD-Build ist das komplette Save-System dormant („Save is not available in this preview") — es existiert kein erreichbarer Save/Load. Und selbst der dormante Load-Pfad WÜRDE das Blut netto verlieren, weil das Ende nie verdrahtet wurde: die Tabelle wird zwar mitgeschrieben und beim Load nach RAM restauriert, aber der Re-Apply-Pfad `LAB_80037d1c` hat **0 Caller** (Binary-Scan §3.3) und jeder Spieler-Load nullt die Tabelle (`FUN_80037c1c`, exakt 2 jal-Sites `@0x800316c8/@0x800318cc`) + lädt das TIM frisch von CD. Der Blob-Inhalt belegt die **Intention** (Wunden persistieren); die Verdrahtung fehlt im Dev-Build — wie beim Save-System selbst.

---

## 1. Das Original-Save-System (dormant) — Blob-Layout, byte-belegt

Card-FSM = `FUN_80025c00` (14 States). Einziger statischer Caller: `@0x8001cba4 jal 0x80025c00` mit `a0=0` (=Save-Modus; `a1 = 0x80200000` Devkit-Buffer, `@0x8001cb9c lui a1,0x8020`) in der Dev-FSM `FUN_8001c958` (nur via Runtime-Pointer erreichbar — dormant, s. Memory `reai-v2-save-load`).

### 1.1 SAVE (case 8, `@0x800261b4-0x80026228`)

```
800261b4 jal FUN_80026f48              ; CAPTURE (s. 1.2)
800261bc sll   a0,s3,0x7               ; slot*0x80
800261c0 addiu a0,a0,0x1430            ; +0x1430
800261c4/c8 a1 = 0x800b0fbc            ; GSB (Game-State-Block)
800261d0 ori   a2,zero,0x1230          ; Laenge
800261d4 jal FUN_8004ee38              ; memcpy(workbuf+slot*0x80+0x1430, GSB, 0x1230)
                                       ;   (Slot-Cache-Update; Stride-Jank im Dev-Code)
800261f8 a0 = 0x800b0dbc               ; Datei-Quelle = GSB-0x200 (Header-Vorspann)
80026220 ori   v0,zero,0x800           ; 5. Arg (Stack): WRITE-LAENGE
80026224 jal FUN_800271a8              ; BIOS open/write/close auf "bu00:"
```

`FUN_8004ee38` = memcpy(dst,src,len) (Byte-Loop). `FUN_800271a8` (decompiliert): `write(fd, param_1, param_5)` mit param_1=`0x800b0dbc`, param_5=`0x800`; Blockzahl `(0x800>>13)+1 = 1` → Header-Byte `param_1+3`, Titel per strcpy → `param_1+4` (Titel gebaut von `FUN_80026e54`: „BIO HAZARD <char> /NN/", Zähler `DAT_800b0fbd`). Bei Erfolg `DAT_800b0fbd++` (Decompile FUN_80025c00 Zeile 276).

**Save-Blob = 0x800 Bytes ab `0x800b0dbc`:**

| File-Offset | RAM | Inhalt |
|---|---|---|
| +0x000..+0x1FF | 0x800b0dbc..0x800b0fbb | Header-Region (+3 Blockzahl, +4 Titel-String; Rest Header/Icon-Vorspann) |
| +0x200..+0x7FF | 0x800b0fbc..0x800b15bb | **die ersten 0x600 Bytes des GSB** `@0x800b0fbc` (Gesamt-GSB 0x1230, es wird nur der Anfang geschrieben) |
| ↳ +0x202 | 0x800b0fbe | PLD-Slot (Charakter/Kostüm) |
| ↳ +0x204/206/208 | 0x800b0fc0/c2/c4 | Spieler X / Yaw / Z |
| ↳ +0x234 | 0x800b0ff0 | Pending-PLD-Variante (`leon_injured_model.md` §2) |
| ↳ **+0x330..+0x40F** | **0x800b10ec..0x800b11cb** | **WUND-TABELLE (8 Panels × 0x1c: Level, Akku, DR_MOVE-Prim)** |
| ↳ +0x688..+0x727 | 0x800b1444/1484/14a4/14c4 | ITEM-BOX-Arrays (bereits als v4-Begründung im Port-Header dokumentiert) |

### 1.2 CAPTURE `FUN_80026f48` — was VOR dem Write in den GSB gespiegelt wird

```
80026f4c lbu v0, DAT_800aca5c   ; PLD-Slot
80026f54 lhu v1, DAT_800aca88   ; Spieler X
80026f5c lhu a0, DAT_800aca8c   ; Yaw
80026f64 lhu a1, DAT_800aca90   ; Z
80026f6c sb  v0, 0x800b0fbe
80026f74 sh  v1, 0x800b0fc0
80026f7c sh  a0, 0x800b0fc2
80026f84 sh  a1, 0x800b0fc4
80026f88 jr  ra                 ; — SONST NICHTS. Kein HP, kein Status.
```

Alles andere im Blob ist der **live gepflegte** GSB-Inhalt (Flags, Item-Box, … und eben die Wund-Tabelle, die `FUN_80037edc` direkt in-place `@0x800b10ec` fortschreibt).

### 1.3 LOAD (case 9, `@0x80026240-0x800262a0`)

```
80026240 ori a2,zero,0x800              ; READ-Laenge
80026270 jal FUN_80027368               ; BIOS open/lseek(0)/read(fd, buf, 0x800)
80026290 a0 = 0x800b0dbc
80026298 a1 = buf
8002629c jal FUN_8004ee38
800262a0 ori a2,zero,0x1430             ; memcpy(0x800b0dbc, buf, 0x1430)  ⚠ 0xC30 mehr
                                        ;   als gelesen (stale Buffer-Bytes) — Dev-Jank
```

→ Der Load restauriert die 0x800 echten Datei-Bytes nach `0x800b0dbc..0x800b15bc` — **inklusive der Wund-Tabelle** `@0x800b10ec`.

---

## 2. HP ist im Original-Save NICHT enthalten (Zensus geschlossen)

HP = `DAT_800acaee`, liegt AUSSERHALB des GSB (0x800b0dbc..0x800b21ec). Vollständiger Xref-Zensus (`ghidra1_V2.txt`, 21 Xrefs):

| Site | Bedeutung |
|---|---|
| `@0x80012e44/e64/edc` | Schadens-Subtraktion `FUN_80012d60` |
| `@0x80031718` | **Spieler-Load `FUN_800314b0`: `HP := 100`** (+ `DAT_800acaec := 0` Poison-Reset) |
| `@0x80031c4c` | Read (Elza-Physik-Gate) |
| `@0x80031e9c-ec8` | Poison-Drain: `HP -= 2`, Clamp auf 1 |
| `@0x8004ae98-aea0` | Event/Item: `HP := 0x4d` Fixwert |
| `@0x8004af70-0x8004b030` | Heil-Item-Pfad (lhu/sh-Paare) |
| `@0x8004aea0`, `@0x8004edb8` | SCD-Write / Status-Classifier-Read |
| `@0x800321a0/0x80032294` | Injured-Idle-Schwellen (Reads) |

**Keine einzige Kopie von/zu 0x800b0dbc..0x800b21ec.** Capture (`FUN_80026f48`, §1.2) schreibt kein HP. → RE1.5s dormantes Save speichert **weder HP noch Condition**; nach einem Load + Raum-Boot hat der Spieler HP=100.

---

## 3. Was der Original-LOAD wiederherstellt — und was nicht

1. **RAM-Restore:** die 0x800 Datei-Bytes → GSB-Anfang inkl. Wund-Tabelle (§1.3). ✔
2. **Kein Raum-Reboot durch die FSM:** nach dem Card-Screen (`@0x8001cbac-cbb4 state:=3`) machen die Handler 3/4/5 (Dispatch `@0x8001069c` → `0x8001cbb8/0x8001cc34/0x8001cc70`) nur Fade + Pause-Flag-Restore. Der geladene GSB wirkt erst beim nächsten Raum-/Spieler-Load.
3. **Wund-Blits werden NIE re-appliziert:** der einzige Re-Insert-Pfad `LAB_80037d1c` hat **0 jal-Sites und 0 Datenpointer** in PSX.EXE + allen BINs (eigener Binary-Scan 2026-08-03, bestätigt `blood_decals.md` §2.5).
4. **Der Spieler-Load ZERSTÖRT den restaurierten Zustand:** `FUN_800314b0` (jal-Sites von `FUN_80037c1c` exakt `@0x800316c8/@0x800318cc`, eigener Scan) → Wund-Tabelle Level+Akku := 0, TIM frisch von CD, `HP := 100 @0x80031718`.

**Netto-Original:** Selbst wenn man das dormante Save/Load fährt, verliert Leon nach dem Load Blut UND Verletzung. Der Blob-Inhalt (Wund-Tabelle wird geschrieben) belegt aber die **Persistenz-Intention** — die letzte Meile (Re-Apply nach Load) wurde im Dev-Build nie verdrahtet, genau wie das Save-System selbst nie freigeschaltet wurde.

**Korrektur zu `analysis/blood_decals.md` §5:** Der Satz „die Wund-Tabelle steht in keinem Save-Record" ist **falsch** — sie steht bei GSB+0x130 im Record und im 0x800-Blob (§1.1). Richtig bleibt: der Load macht sie wirkungslos (Reset + kein Re-Apply).

---

## 4. RE2-Leon-Vergleich (Architektur-Spender des Port-Saves)

- Retail-RE2 speichert den Spielzustand inkl. **HP/Condition** und stellt ihn beim Laden wieder her; das Limpen kommt (wie in RE1.5) zur Laufzeit aus der HP → nach Load automatisch korrekt. RE2 hat **kein** Spieler-Textur-Blut-Decal-System (das ist RE1.5-spezifisch).
- **OFFEN (ehrlich):** Das byte-genaue RE2-Blob-Layout (HP-Offset im Save) wurde hier NICHT nachgewiesen — RE2s Card-I/O liegt im Overlay `info/re2leon/COMMON/BIN/MEM_CARD.BIN` (@0x801c0000, 0x5d7c Bytes; „bu00" @0x801c02f4, „BASLUS" @0x801c0008); ein Kurz-Scan fand die Save-Quellbuffer-Bildung nicht sofort (0x800ce310 = Flags-Word, nicht der Buffer). Eigenes RE-Ticket, falls das RE2-Layout gebraucht wird. Für die Verhaltens-Aussage (HP wird restauriert) ist das Retail-Verhalten hinreichend bekannt.

---

## 5. PORT-IST — GEMESSEN (`probe_save_injured`, tests/unit, kein ctest)

Save-Block = `re15_savedata_t` (`re15_port/include/re15_savedata.h`, v4): magic/version/playtime, Position, room, save_count, `player_hp`, `player_status`, character, equipped_slot, weapon_id, camera_cut, inv[11], flags[16][8], box[32], checksum. **Kein Wund-Feld.** Wund-Zustand = `s_wounds[8]{level,acc}` (re15_damage.c:106-141) + Wound-Sync (main.c:5217-5251, re-appliziert Blits bei Generation-/Slot-Textur-Wechsel — der Port HAT also, anders als das Original, einen funktionierenden Re-Apply-Pfad).

Messungen (Probe baut das Ground-Truth-Wundbild aus `mzd_blood_decals_hp30.sav` nach: 3× Hurt-Substate-0 → p5=1, p7=1, p0-Akku 30; hp=30):

| # | Messung | Ergebnis |
|---|---|---|
| M1 | `capture` mit Blut vs. ohne Blut | **byte-identisch** → Wund-Tabelle wird nicht serialisiert |
| M2 | Voll-Roundtrip über echte `.mcr` (save→load→restore), frischer Boot | **hp=30 restauriert** (→ Injured-Idle automatisch, player_common.c:551-590), **Wund-Level alle 0** → Leon lädt unblutig, obwohl blutig gespeichert |
| M3 | Tod-mit-Blut → Title → CONTINUE eines SAUBEREN Saves | **Wund-Level p5/p7 = 1 bleiben stehen** → geladenes sauberes Spiel startet blutig (**BUG**) |

M3-Ursache (statisch): der CONTINUE/LOAD-Pfad `main.c:1786-1793` setzt `g_gameflow` direkt und ruft `re15_gameflow_new_game()` — den einzigen `re15_wound_reset()`-Caller (re15_gameflow.c:47-48) — NICHT; der Gameover→Title-Pfad (main.c:4030-4033) memset't nur `g_inv`/`g_game`. Der Wound-Sync stempelt die stale Level dann auf die frisch geladene Textur. Genau die Lücke, vor der der Kommentar in re15_gameflow.c warnt („ein separater Load-Pfad muss diesen Reset mitrufen").

---

## 6. Antwort auf die Nutzer-Frage + Fix-Plan

**Animationen (Caution/Danger):** Der Port lädt sie KORREKT verletzt — HP wird gespeichert/restauriert, die Anims folgen pro Frame. Kein Fix. (Dass RE1.5s dormantes Save nicht mal HP sicherte, ist Dev-Build-Realität; das HP-Restore ist das per Nutzer-Mandat übernommene RE2-Verhalten.)

**Blut-Texturen:** Der Port lädt sie FALSCH (unblutig). RE1.5s eigener Blob serialisiert die Wund-Tabelle (§1.1) — der Port soll das ebenfalls tun, konsistent mit der v4-Item-Box-Begründung („liegt im 0x1230-GSB-memcpy") und dem Nutzer-Mandat „Load = gespeicherter Zustand". Dass das Original die letzte Meile (Re-Apply) nie verdrahtet hat, ist derselbe Dormant-Zustand wie das Save-System selbst — der Port hat den Re-Apply-Pfad (Wound-Sync) bereits.

**Fix-Plan (2 Schritte, klein):**
- **F1 — BUG-Fix (unabhängig):** `re15_wound_reset()` in den Load-Pfad — sauberster Ort: am Anfang von `re15_savedata_restore()` (engine-seitig, wirkt für CONTINUE + jeden künftigen Load-Pfad) ODER in main.c:1788 vor `s_resume_pending=1`. Behebt M3 (stale carry-over).
- **F2 — Wund-Persistenz (v5):** `uint8_t wounds[8][2]` (level, acc — das GSB+0x130-Analog, nur die 2 Zustands-Bytes; die DR_MOVE-Koordinaten sind im Port ableitbar) in `re15_savedata_t` VOR `checksum` einfügen; `capture`: aus `re15_wound_level()`/neuem acc-Getter (oder ein `re15_wound_export/import`-Paar in re15_damage.c); `restore`: nach F1-Reset Tabelle setzen + Generation bump → der vorhandene Wound-Sync (main.c:5235) stempelt die Panels nach dem nächsten Slot-0-TIM-Upload automatisch. Versions-Upgrade-Pfad wie v2/v3→v4 (ältere Saves laden mit Wund-Tabelle 0). Verifikation: `probe_save_injured` M2-Erwartung invertieren (Level p5/p7 == 1 nach Load) + gdigrab-Sichtprüfung (Skill `re15-port-visual-verify`).

---

## 7. Offene Punkte (ehrlich OFFEN)

1. **RE2-Save-Blob-Layout** (HP-Feld-Offset, Blockgröße) byte-genau aus `MEM_CARD.BIN` @0x801c0000 — eigenes RE-Ticket; hier nur Verhaltens-Referenz.
2. **GSB+0-Semantik:** `probe_save_counter.c` nennt 0x800b0fbc „Inventar-Kapazität", `leon_injured_model.md` „Stage" — widersprüchlich, für dieses Dossier irrelevant (beide bestätigen GSB-Basis + memcpy-Layout). Bei Gelegenheit klären.
3. **Header-Region 0x800b0dbc..0x800b0fbb** vollständig kartieren (nur +3 Blockzahl und +4 Titel sind belegt).
4. Ob im 0x600-GSB-Fenster weitere verhaltensrelevante Zustände liegen, die der Port-Save noch nicht abdeckt (systematischer Feld-für-Feld-Census 0x800b0fbc..0x800b15bb) — Kandidat für einen Save-Vollständigkeits-Audit.
