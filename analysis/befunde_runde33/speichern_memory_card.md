# Runde 33 / Thema S — Speichern nur mit Memory Card (RE2-Farbband-Ablauf)

Nutzer-Auftrag (woertlich): "Ich möchte das du eine Message schreibst, wie in Resident Evil 2, wenn man
speichern möchte, aber kein Farbband besitzt. Nur statt Farbband eben "Memory Card". Speichern soll nur
möglich sein, wenn man eine Memory Card besitzt."

Einordnung: RE1.5-Auslieferungsstand kann nicht speichern (21 RDTs: "Save is not available in this
preview"). RE2 Retail ist das Ziel (Memory reai-v2-beta-zu-retail). Memory Card (RE1.5-Item 0x21) ersetzt
das Farbband. **PORT-WAHL auf Nutzerwunsch: die Karte wird beim Speichern NICHT verbraucht** (RE2 verbraucht
ein Farbband je Speicherung, Beleg §1.5).

Status: IN ARBEIT (Dossier wird laufend fortgeschrieben)

Quellen (nur gelesen): `info/re2leon/PSX.EXE` (SLUS_007.48, t_addr 0x80010000, RAM = 0x80010000 +
Datei-Offset - 0x800), `info/re2leon/COMMON/BIN/MEM_CARD.BIN` (laedt @0x801C0000), `info/Re1.5/PSX.EXE`,
`re15_port/shared_assets/PSX/` (RDTs, DEBUG.BIN @0x800C0000). Disassembliert mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` / `re15_disasm.py`.

---

## 1. RE2-Beleg — die Schreibmaschine

### 1.1 AOT-Typ und Handler

RE2s AOT-Handlertabelle liegt @0x800A73C4 (Index = AOT-Typbyte +0), gelesen von
`@0x8005522c lw v0,0x73c4(at)` / `@0x80055234 jalr v0` (und von der AOT-Schleife `@0x8005111c`).
Eintraege (selbst ausgelesen, Wort je 4 Byte):

| Typ | Handler | |
|---|---|---|
| 8 | 0x80051A48 | |
| **9 = SAVE (Schreibmaschine)** | **0x80051AB0** | Tabelle @0x800A73E8 |
| 10 = ITEMBOX | 0x80051C20 | Tabelle @0x800A73EC |

`0x80051AB0` (Eintritt beim Untersuchen):
```
80051ab8: lui v0,0x8005 / 80051abc: addiu v0,v0,6916   ; 0x80051B04 = Fortsetzung
80051ac4: sw v0,17564(at)                            ; -> DAT_800D449C (Fortsetzungszeiger)
80051ad4: sb zero,16970(at)                          ; DAT_800D424A = 0 (Unterzustand)
80051ae4: sw v0,-3188(at)                            ; DAT_800DF38C = Pause-Flags (Sicherung)
80051ae8: or v0,v0,v1 (v1 = 0xff000000)
80051af0: sw v0,-1060(at)                            ; DAT_800CFBDC |= 0xFF000000 (Freeze)
80051ad8: lhu a0,12(v1) / 80051af8: sb a0,18095(at)  ; AOT-Datenwort -> DAT_800D46AF
```

### 1.2 Die Farbband-Pruefung — NUR das Inventar

Fortsetzung `0x80051B04`, Unterzustand 0:
```
80051b34: jal 0x800696cc
80051b38: addiu a0,zero,30        ; Item 30 = 0x1E = Farbband (Ink Ribbon)
80051b3c: bltz v0,0x80051b68      ; nicht gefunden -> Meldung 0
```
`FUN_800696CC(id)` (vollstaendig, @0x800696cc-0x80069710): laeuft ueber die Inventarslots
@0x800D4A3C (Schritt 4, `lbu v0,19004(at)` @0x800696e4) bis zur Inventar-Kapazitaet
`lbu v0,0(a2)` a2 = 0x800D46AC (@0x800696f4); Rueckgabe Slot oder -1 (@0x80069708).
Die Item-Kiste liegt dahinter @0x800D4A68 (64 Slots, analysis/itembox_re2/re2-box-speicher.md §1)
und wird **nicht** durchsucht. **=> Es zaehlt nur, was man bei sich traegt.**

RE1.5-Gegenstueck derselben Form: `FUN_8004DFEC` (@0x8004dfec-0x8004e04c) laeuft ueber
DAT_800B10AC (Inventar, Schritt 4) bis DAT_800B0FBC (Slotzahl) — im Port `re15_inv_find_item`.

### 1.3 Die beiden Meldungen (EXE, keine Raumdaten)

Aufloeser `FUN_8002FE38(a0,a1,a2,a3)`: Gruppe `a1 = 0x100`, englischer Block = u16-Offsettabelle
@0x8009F368, Basis @0x8009EFCC (analysis/befunde_2026-09-21/discard-re2-mechanismus.md §1; Tabelle hier
nachgelesen: `00 00 53 00 ...` -> Eintrag 0 = +0x0000, Eintrag 1 = +0x0053).

**Ohne Farbband** — `@0x80051b68 addiu a1,zero,256` / `@0x80051b6c addu a2,zero,zero` /
`@0x80051b80 jal 0x8002fe38` / `@0x80051b84 lui a3,0xff00` -> Meldung (0x100, **0**),
RAM **0x8009EFCC** (Datei 0x8F7CC), roh:
```
fc 25 50 3a 4f 00 3d 4a 00 4b 48 40 00 50 55 4c 41 53 4e 45 50 41 4e 01   \n"It's an old typewriter."
fd 00                                                                     Seite (Taste)
25 42 00 25 00 44 3d 40 00 3d 4a 00                                       "If I had an "
f9 01 25 4a 47 00 2e 45 3e 3e 4b 4a f9 00                                 [Farbe 1]"Ink Ribbon"[Farbe 0]
18 00 25 fc                                                               ", I" \n
3f 4b 51 48 40 00 4f 3d 52 41 00 49 55 00 4c 4e 4b 43 4e 41 4f 4f 01 01 01 "could save my progress..."
fe 00                                                                     Ende
```
> **"It's an old typewriter."** / **"If I had an Ink Ribbon, I could save my progress..."**

Vor dem Aufruf werden die Pause-Flags zurueckgeschrieben (`@0x80051b74 lw DAT_800DF38C` /
`@0x80051b7c sw DAT_800CFBDC`), der Text friert selbst ein (a3 = 0xFF000000); danach Fortsetzung
geloescht (`@0x80051b8c sw zero,DAT_800D449C`). Kein Speichern, kein Ton im Handler.

**Mit Farbband** — `@0x80051b40 addu a0,zero,zero` / `@0x80051b44 addiu a1,zero,256` /
`@0x80051b48 addiu a2,zero,1` / `@0x80051b4c jal 0x8002fe38` / `@0x80051b50 lui a3,0xff00`
-> Meldung (0x100, **1**), RAM **0x8009F01F** (Datei 0x8F81F), roh:
```
fc 25 50 3a 4f 00 3d 4a 00 4b 48 40 00 50 55 4c 41 53 4e 45 50 41 4e 01   \n"It's an old typewriter."
fd 00                                                                     Seite
35 4b 51 00 3f 3d 4a 00 4f 3d 52 41 00 55 4b 51 4e 00 4c 4e 4b 43 4e 41 4f 4f "You can save your progress"
fc 53 45 50 44 00 50 44 45 4f 01                                          \n"with this."
fd 00                                                                     Seite
33 45 48 48 00 55 4b 51 00 51 4f 41 00 50 44 41 fc                        "Will you use the" \n
f9 01 25 4a 47 00 2e 45 3e 3e 4b 4a f9 00 1b                              [Farbe 1]"Ink Ribbon"[Farbe 0]"?"
fb 00                                                                     Ja/Nein-Auswahl
fe 00                                                                     Ende
```
> **"It's an old typewriter."** / **"You can save your progress with this."** /
> **"Will you use the Ink Ribbon?"**  -> Yes / No

Zeichensatz: Buchstaben identisch zu RE1.5 (A = 0x1D, a = 0x3D, '?' = 0x1B, ',' = 0x18, ''' = 0x3A);
in RE2 ist 0x01 der Punkt-Glyph (Beleg discard-re2-mechanismus.md §0), RE1.5 nimmt 0x57 und 0x01 = Ende.

### 1.4 Ja / Nein

Unterzustand 1 (`@0x80051ba0`): wartet, bis das Nachrichtensystem frei ist
(`lbu DAT_800E873C` / `andi 0x80` / `bne` @0x80051ba4-b0), dann Antwort-Bit 0:
* **Ja** (Bit 0 = 0): `@0x80051bc4 sb 1,DAT_800DF348`, `@0x80051bd0-dc DAT_800CFB74 |= 0x40000`
  = Anforderung Speicherbildschirm (die Pause-Flags bleiben gesetzt, der Bildschirm uebernimmt).
* **Nein** (Bit 0 = 1): `@0x80051be8-f4` Pause-Flags zurueck (DAT_800DF38C -> DAT_800CFBDC).
* beide: Fortsetzung + Unterzustand geloescht (`@0x80051c00` / `@0x80051c0c`).
Vorbelegung **Yes** (Auswahlzelle := 0x80 beim Nachrichtenstart @0x8002FE88, discard-Dossier §2).

### 1.5 Verbrauch (nur Dokumentation — der Port verbraucht NICHT)

Im Overlay MEM_CARD.BIN, direkt nach dem Kopieren des Inventars in den Speicherpuffer:
```
801c1180: jal 0x80076a00 / 801c1184: addiu a2,zero,44   ; memcpy Inventar (11 Slots) -> Puffer
801c118c: addiu a0,zero,30 / 801c1190: jal 0x800696cc  ; Farbband-Slot suchen
801c11a4: lbu v0,19005(at) / 801c11ac: addiu v0,v0,-1 / 801c11b8: sb v0,19005(at)   ; Anzahl - 1
801c11c0: bne v0,zero,...  ; 0 -> 801c11d0 sb zero,id / 801c11dc sb zero,+2 / 801c11e0 jal 0x80069714 (kompaktieren)
```
**PORT-WAHL (Nutzer):** dieser Block entfaellt; die Memory Card bleibt nach dem Speichern im Inventar.

### 1.6 Toene

Der Schreibmaschinen-Handler selbst hat **keinen** Ton-Aufruf (in 0x80051AB0-0x80051C1C nur
`jal 0x800696cc` und `jal 0x8002fe38`). Toene kommen allein aus der Ja/Nein-Auswahl des
Nachrichtensystems (@0x80030844): Cursor `@0x80030968`/`@0x8003099c lui a0,0x404` -> Se_on 0x8005BA28;
Bestaetigen Ja `@0x80030944`/`@0x80030950 lui a0,0x406`, Nein `@0x8003093c lui a0,0x405`
(Ja stumm nur bei Auswahlbyte & 0x20, hier `fb 00` -> Ton faellt). Genau diese drei Toene spielt
die Port-Auswahl bereits (msg_common.c Zustand 3, re15_audio_core_se 4/5/6, RE1.5 CORE00.EDH
Records 4/5/6) — die neue Abfrage laeuft durch dieselbe Auswahl, also **dieselben Toene, nichts Neues**.

## 2. Port heute

(offen)

## 3. Bau

(offen)

## 4. Riegel

(offen)

## 5. Abnahme

(offen)
