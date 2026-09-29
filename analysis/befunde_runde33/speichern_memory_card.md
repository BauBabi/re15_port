# Runde 33 / Thema S — Speichern nur mit Memory Card (RE2-Farbband-Ablauf)

Nutzer-Auftrag (woertlich): "Ich möchte das du eine Message schreibst, wie in Resident Evil 2, wenn man
speichern möchte, aber kein Farbband besitzt. Nur statt Farbband eben "Memory Card". Speichern soll nur
möglich sein, wenn man eine Memory Card besitzt."

Einordnung: RE1.5-Auslieferungsstand kann nicht speichern (21 RDTs: "Save is not available in this
preview"). RE2 Retail ist das Ziel (Memory reai-v2-beta-zu-retail). Memory Card (RE1.5-Item 0x21) ersetzt
das Farbband. **PORT-WAHL auf Nutzerwunsch: die Karte wird beim Speichern NICHT verbraucht** (RE2 verbraucht
ein Farbband je Speicherung, Beleg §1.5).

Status: GEBAUT + ABGENOMMEN (Framedumps echte exe, §5); Riegel unit_r33_speichern +
integration_r33_speichern (§4). Commit ee17ca0a (Bau) auf Zweig r33/speichern.

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

## 2. Port heute (gemessen VOR dem Bau)

### 2.1 Die Speicherstellen

16 Stellen, Registry `engine/src/re15_savepoint.c` (Leon-Raum + Elza-Spiegel), alle mit einer
ausgelieferten RE1.5-Meldung derselben Form (`rdt_msgdump.py`; Steuercodes `04 02`, `02 00`, `02 00`, `01 00`):

| Raum | msg | Satz 1 | Weg zum Text |
|---|---|---|---|
| 1070/1071 | 20 | "It's an phone." (Tippfehler des Originals, ROOM1070 @0x1CA0 `3d 4a 00 4c 44 4b 4a 41`) | MESSAGE-AOT slot 12, Nutzlast `14 00 ff ff` |
| 1120/1121 | 6 | "It's a phone." | MESSAGE-AOT slot 3, `06 00 ff ff` |
| 1150/1151 | 1 | "It's a phone." | GENERIC-AOT slot 3 (Event 6) -> sub06 `Cut_chg(6)` + `2b 01 ff ff` |
| 2010/2011 | 3 | "It's a computer." | |
| 30A0/30A1, 30B0/30B1 | 1 | "It's a computer." | (30B0/30B1: Altbefund, AOT sce=0 erreicht den Text nicht — unveraendert) |
| 4010 / 4011 | 43 / 7 | "It's a computer." | |
| 5010 / 5011 | 6 / 45 | "It's a computer." | |

Seiten 2/3 ueberall: "You can save your progress / with this." / "Save is not available in / this preview."
**RE1.5s Meldung ist RE2s Meldung (0x100,1) mit anderem Gegenstand auf Seite 1 und anderer Schlussseite**
("It's an old typewriter." <-> "It's a phone."; "Will you use the Ink Ribbon?" <-> "Save is not available ...").
Pausen-Maske aller gemessenen Stellen: `ff ff` -> 0xffff0000 (Spieler, KI, Anim, Skript stehen).

### 2.2 Ablauf vor dem Bau

Lauf `build/r33_speichern/base_ohne` (Vorher-exe, Spielstand des Nutzers Platz 0 = ROOM1150 vor dem
Telefon, Inventar OHNE Memory Card, VIERECK in Bild 150), debug.log:
```
[save] CONTINUE: resumed in room 1150 (hp=100)
[msg] room=1150 id=1 SAVEPOINT (menu, message suppressed)
[save] saved (room 1150) slot n=1 -> next=2; card=none
```
= **Untersuchen oeffnete sofort den Speicherbildschirm, ohne jeden Text und ohne Karte**; eine
vorhandene Karte wurde je Speicherung verbraucht (main.c `--g_inv.slots[mc].qty`). Keine Abfrage.
Beleg: `speichern_belege/00_vorher_ohne_karte_kein_text_F160.jpg`.

## 3. Bau

### 3.1 Ablauf (RE2-Form, Memory Card statt Farbband)

1. Untersuchen einer Speicherstelle — beide Wege (`op_message_on` und `re15_scd_show_message` in
   `scd_vm.c`) — ruft `re15_savepoint_examine(raum, msg, maske)` statt des frueheren Sofort-Pendings.
2. Karte: `re15_inv_find_item(0x21)` = **nur Inventar** (RE2 FUN_800696CC / RE1.5 FUN_8004DFEC).
3. Der Text liegt im Port-Platz 0xFE der Nachrichtentabelle und oeffnet mit der **Maske der ausgelieferten
   Meldung** im normalen Dialog-FSM (nicht blockierend; die Maske 0xffff0000 haelt das Skript an, deshalb
   bleibt in ROOM1150 die Nahaufnahme Cut 6 aus sub06 waehrend des Textes stehen — wie bei der
   ausgelieferten Meldung).
4. **Ohne Karte**: Hinweis, Ende (RE2 @0x80051b68-b98). **Mit Karte**: Frage + Ja/Nein, Abfrage offen
   (RE2 Unterzustand 1). `re15_savepoint_poll()` (main.c, je Bild vor der Pending-Abfrage) wartet, bis der
   Text zu ist, und meldet **JA genau einmal** (Antwort = Flag (12,31) aus dem Dialog-FSM, 0 = JA)
   -> Pending -> Speicherbildschirm. NEIN -> nichts.
5. **Kein Verbrauch** (PORT-WAHL, Nutzer): main.c laesst Block und Live-Inventar unveraendert
   (`[save] saved ... card=kept (slot N qty Q)`).
6. Der Speicherbildschirm oeffnet erst NACH dem Schliessen des Textes — kein wartender Dialog hinter dem
   modalen Menue (die alte Freeze-/Flacker-Quelle, Memory reai-v2-save-load, bleibt zu).
7. `RE15_SAVE_TEST` (Testhaken) oeffnet den Bildschirm weiter direkt (Bypass, kein Spielverhalten).

### 3.2 Die Texte (Bytes, RE1.5-Zeichensatz)

Buchstabenbytes = 1:1 RE2. Steuercodes auf das RE1.5-Gegenstueck an derselben Stelle ausgelieferter
RE1.5-Texte:

| RE2 | Bedeutung | RE1.5 | RE1.5-Beleg |
|---|---|---|---|
| `fc` | Zeilenwechsel | `08` | ROOM1150 msg 1 "progress `08` with this" |
| `fd 00` | Seite, Taste | `02 00` | ROOM1150 msg 1 "phone. `02 00` You can" |
| `f9 01` / `f9 00` | Farbe 1 / zurueck | `05 01` / `05 00` | ROOM10D0 msg 9 "You've used the `05 01` Blue Keycard `05 00`" (Gegenstandsname) |
| `fb 00` | Ja/Nein | `03 02` | alle 90 RE1.5-Auswahltexte enden `03 02 01 00` (Census ueber alle RDTs) |
| `fe 00` | Ende | `01 00` | alle RE1.5-Texte |
| `01` | '.' | `57` | RE1.5: 0x01 = Ende; Punkt = 0x57 (ROOM1150 "phone`57`") |
| "Ink Ribbon" `25 4a 47 00 2e 45 3e 3e 4b 4a` | Name | "Memory Card" `29 41 49 4b 4e 55 00 1f 3d 4e 40` | RE1.5-Name von 0x21, DEBUG.BIN @0x800C4BEA (Tabelle @0x800C495C) |

**Mit Karte** = RE1.5-Meldung der Stelle bis einschl. 2. `02 00` (aus der RDT) + RE2-Schlussseite:
```
33 45 48 48 00 55 4b 51 00 51 4f 41 00 50 44 41 08 05 01 29 41 49 4b 4e 55 00 1f 3d 4e 40 05 00 1b 03 02 01 00
"Will you use the" / [gruen]"Memory Card"[weiss] "?" [Ja/Nein]
```
ROOM1150: **"It's a phone." / "You can save your progress with this." / "Will you use the Memory Card?" -> Yes / No**

**Ohne Karte** = RE1.5-Meldung bis einschl. 1. `02 00` + RE2-Schlussseite von Meldung (0x100,0):
```
25 42 00 25 00 44 3d 40 00 3d 00 05 01 29 41 49 4b 4e 55 00 1f 3d 4e 40 05 00 18 00 25 08
3f 4b 51 48 40 00 4f 3d 52 41 00 49 55 00 4c 4e 4b 43 4e 41 4f 4f 57 57 57 01 00
"If I had a " [gruen]"Memory Card"[weiss] ", I" / "could save my progress..."
```
ROOM1150: **"It's a phone." / "If I had a Memory Card, I could save my progress..."**

Entscheidungen, offen benannt:
* **Seite 1 = RE1.5s eigener Gegenstandssatz** ("It's a phone." / "It's a computer."), nicht RE2s
  "It's an old typewriter." — die RE1.5-Speicherstellen SIND Telefone/Computer, und RE1.5s Meldung hat
  genau RE2s Seitenbau (§2.1). ROOM1070/1071 zeigen den Original-Tippfehler "It's an phone." byte-true.
* **"an" -> "a"** vor "Memory Card" (Artikel vor Konsonant) — einzige Wortaenderung neben dem Namen;
  Zeilenlaenge bleibt 25 Zeichen wie RE2s "If I had an Ink Ribbon, I".
* **Gross/Klein**: "Memory Card" wie RE1.5s Itemname und wie RE2s "Ink Ribbon" (Titelschreibweise),
  Farbe 1 wie beide Originale fuer Gegenstandsnamen (im RE1.5-Schriftsatz gruen).
* **Kein Verbrauch** (Nutzer). **Ja vorbelegt** (RE2 @0x8002FE88; der Port-Dialog startet mit Wahl 0).
* **Toene**: nichts Neues (§1.6) — die vorhandene Ja/Nein-Auswahl spielt RE2s Cursor-/Bestaetigungstoene.

### 3.3 Dateien

* `re15_port/include/re15_savepoint.h`, `re15_port/engine/src/re15_savepoint.c` — Text-Bau, Examine, Poll
* `re15_port/engine/src/scd_vm.c` — beide Intercepts rufen `re15_savepoint_examine`
* `re15_port/platform/pc/main.c` — Poll vor der Pending-Abfrage, kein Verbrauch, Log `card=kept`
* `re15_port/tools/local_build.sh` — RE15_MIN_TESTS 416 -> 418

## 4. Riegel

* **unit_r33_speichern** (`tests/unit/probe_r33_speichern.c`, `tests/unit/probes/r33_speichern.cmake`):
  (A) alle 16 Stellen: Anfang byte-gleich zur RDT, dekodierter Text == Soll, Endform `1b 03 02 01 00` /
  `57 57 57 01 00`; (B) ROOM1150 ohne Karte ueber den echten AOT-Scan (Slot 3 -> Event 6): Hinweis, keine
  Frage, kein Pending; (C) Karte nur in der Item-Kiste -> Hinweis; (D) mit Karte + JA: Frage, Ja vorbelegt,
  poll == 1 genau einmal, Inventar unveraendert; (E) mit Karte + NEIN: kein Pending; (F) waehrend des
  Textes Cut 6 + Maske 0xffff0000, danach laeuft sub06 weiter.
* **integration_r33_speichern** (`tests/integration/test_r33_speichern.cmake`, Kartenwerkzeug
  `tests/unit/probe_r33_speichern_karte.c`): echte exe, Spielstand ROOM1150 vor dem Telefon, VIERECK per
  RE15_PAD_AT. A ohne Karte: Hinweis, kein `[save] saved`. B mit Karte + JA: `[save] saved`,
  `card=kept (slot 0 qty 1)`, geschriebener Block Platz 1 Inventar 0 = `21 01`. C mit Karte + NEIN:
  kein `[save] saved`.
  **Gegenprobe** gegen die Vorher-exe: ROT in A (speicherte ohne Karte dreimal, `card=none`).
  3/3 gruen mit eigener exe-Kopie.
  ⛔ Unter ctest mit `re15_pc.exe` starb der Prozess zweimal mitten im Lauf (Bild 60 bzw. 240, exit 1,
  Log bricht ohne Fehlerzeile ab), waehrend drei andere Agenten parallel liefen; mit eigener exe-Kopie
  0 Ausfaelle. Einzeln wiederholen (Memory reai-v2-gui-tests-flattern-bei-parallelen-agenten).

## 5. Abnahme im echten Spiel

Werkzeug: `speichern_werkzeug/r33_speichern_lauf.sh` (echte exe unter eigenem Namen, beschleunigter
Renderer, RE15_FRAMEDUMP, RE15_WINDOW_SCALE=3; Spielstand des Nutzers Platz 0 = ROOM1150 vor dem Telefon;
VIERECK in Bild 150 = echtes Untersuchen), Kontaktbogen `speichern_werkzeug/kontaktbogen.py`.
Alle Bilder angesehen:

| Bild | Lauf | Inhalt |
|---|---|---|
| `00_vorher_ohne_karte_kein_text_F160.jpg` | Vorher-exe | kein Text — sofort gespeichert (`card=none`) |
| `01_ohne_karte_seite1_F190.jpg` | ohne Karte | Nahaufnahme Schreibtisch (Cut 6), "It's a phone." + Pfeil |
| `02_ohne_karte_hinweis_F320.jpg` | ohne Karte | "If I had a **Memory Card**, I / could save my progress..." (Name gruen) |
| `03_ohne_karte_zurueck_F350.jpg` | ohne Karte | nach Taste zurueck im Spielbild, kein Speicherbildschirm |
| `04_mit_karte_seite2_F290.jpg` | mit Karte | "You can save your progress / with this." + Pfeil |
| `05_mit_karte_frage_ja_F360.jpg` | mit Karte | "Will you use the / **Memory Card**?  ▸Yes  No" |
| `06_mit_karte_nach_speichern_F400.jpg` | mit Karte + JA | zurueck im Spiel nach dem Speichern |
| `07_mit_karte_frage_nein_F370.jpg` | mit Karte + NEIN | Cursor auf "No" |

Logs: ohne Karte `card_slot=-1 -> HINWEIS`, kein `[save] saved`; mit Karte + JA
`FRAGE` / `Antwort: JA` / `[save] saved (room 1150) slot n=1 -> next=2; card=kept (slot 3 qty 2)`;
mit Karte + NEIN `Antwort: NEIN`, kein Speichern. Nach dem Speichern laeuft Leon frei (STATE_LOG
Bild 400-440 mit OBEN: Lage (-22689,-19693) -> (-24288,-21192)).
Der Speicherbildschirm selbst ist modal und liegt ausserhalb der Spielbild-Zaehlung (kein Framedump);
belegt ueber `[save] saved` und den geschriebenen Kartenblock (Riegel B).

## 6. Offen

* **PSX-Ziel**: die Nachrichtentabelle hat dort nur 32 Plaetze (msg_common.c `MSG_TABLE_N`), Platz 0xFE
  passt nicht — der Text bliebe leer. Die PSX-Plattform hat ohnehin keinen Speicherablauf (Memory
  reai-v2-psx-build-gap). Nicht angefasst.
* **ROOM30B0/30B1** erreichen den Speichertext weiterhin nicht (Altbefund, AOT sce=0; Memory reai-v2-save-load).
* Alte Spielstaende ohne Memory Card (z. B. die Karte des Nutzers vom 2026-09-27, alle vier Plaetze):
  erst die Karte am Schreibtisch in Irons' Buero aufnehmen (Runde 30 E2, ROOM1150/1151), dann speichern.
