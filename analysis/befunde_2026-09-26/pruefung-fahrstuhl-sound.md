# Prüfung: analysis/befunde_2026-09-26/fahrstuhl-sound.md

Rolle: Skeptiker. Keine Engine-Änderung, kein Build. Alle unten genannten Bytes/Adressen
selbst gelesen (38 Stellen), nicht aus dem Dossier übernommen.

## Urteil in einem Satz

Der harte Kern (16 bitgleiche Bytes, die zwei Se_on, die zwei Wellenformen) hält
byte-genau — aber das Dossier **behauptet mehr als es belegt**: eine zitierte Grundgesamtheit
ist falsch (495 statt 250 RE2-RDTs), und der Satz „RE1.5 hat KEINEN" ist nur skriptseitig
geprüft, obwohl RE1.5 nachweislich auch aus dem Stage-Overlay heraus SE auslöst.
**haltbar = false.**

---

## 1. Was ich BESTÄTIGEN kann (selbst gelesen)

### 1.1 Die „16 bitgleichen Bytes" — stimmen exakt

Rohsuche nach `22 01 1c 01 09 0a 08 00 22 01 1c 00 09 0a 5a 00` über **alle** RDTs beider
Bäume (240 RE1.5-RDTs unter `re15_port/shared_assets/PSX/**`, 250 RE2-RDTs unter
`info/re2leon/PL0/RDT/`). Vollständige Trefferliste, selbst erzeugt:

```
15 shared_assets/PSX/STAGE1/ROOM1080.RDT  0x746 0x7d8 0x86a
15 shared_assets/PSX/STAGE1/ROOM1081.RDT  0x746 0x7d8 0x86a
15 shared_assets/PSX/STAGE4/ROOM4020.RDT  0x95a 0xa00 0xa96
15 shared_assets/PSX/STAGE4/ROOM4021.RDT  0x95a 0xa00 0xa96
R2 info/re2leon/PL0/RDT/ROOM21B0.RDT      0x2762
R2 info/re2leon/PL0/RDT/ROOMB1B0.RDT      0x28b8
```
Kein weiterer Treffer. Rohbytes @ROOM1080.RDT:0x746 nachgelesen:
`22 01 1c 01 09 0a 08 00 22 01 1c 00 09 0a 5a 00` — identisch mit
ROOM21B0.RDT:0x2762. ✔

### 1.2 Die zwei Se_on in ROOM21B0 — Bytes und Bank/Id aufgelöst

Selbst gelesen:
```
ROOM21B0.RDT 0x2756 : 36 02 11 01 01 00 00 00 00 00 00 00
ROOM21B0.RDT 0x2784 : 36 02 12 01 01 00 00 00 00 00 00 00
```
Operanden-Layout **nicht** aus dem Decompilat, sondern aus dem Handler selbst
(`re15_disasm.py dis 0x80041624`):
```
80041640: lh   a1,4(s0)      ; +4/+5
80041644: lbu  a3,1(s0)      ; +1 = BANK
80041648: lh   a0,2(s0)      ; +2 = id (low) | flags (high)
8004164c: andi a2,a1,0xff    ; Ursprungs-Modus 0..5
80041650: sltiu v0,a2,0x6    ; Tabelle @0x80010d9c
```
→ bank=2, id=0x11 bzw. 0x12, flags=1, Ursprungs-Modus=1. bank 2 → SND0 laut
`re15_port/include/re15_audio.h:157`. ✔

**Wichtig — und vom Dossier nicht gesagt:** diese Lesart stützt sich auf den **RE1.5**-Handler,
angewandt auf **RE2**-Bytes. Die Gegenprobe habe ich nachgeholt: dieselbe Instruktionsfolge
(`lh a1,4(s0)` / `lbu a3,1(s0)` / `lh a0,2(s0)` / `andi a2,a1,0xff`) steht in
`info/re2leon/PSX.EXE` @Datei 0x46c44 = RAM **0x80056444** (0x80010000 + off − 0x800).
Das Layout ist in beiden Engines dasselbe. Damit trägt die Auflösung.

### 1.3 Ausrichtung der Se_on hängt NICHT an der empirischen RE2-Opcode-Tabelle

Zwei unabhängige Anker:
* Walk mit der **RE1.5**-Tabelle (`re15_port/tools/scd_dump_room.py:19-33`) ab 0x26F8 läuft
  sauber bis 0x2744 (dort RE2-Opcode 0x8A, in RE1.5 unbekannt → Abbruch).
* Selbstverankerung: 0x2762 − 12 = **0x2756**, 0x2790 − 12 = **0x2784**. Se_on ist 12 B, und
  unmittelbar hinter jedem der beiden Kandidaten steht die bereits rohgefundene
  Set/Sleep/Set/Sleep-Signatur. Beide Male dieselbe 18-B-Lücke (`8a 00 04 00 00 00 8b c8 …`).

→ Antwort auf die Frage des Auftrags: **für diese Fundstelle reicht die empirische Tabelle,
weil sie gar nicht gebraucht wird.** Das Dossier sagt das in §6.3 selbst („zusätzlich über die
16-Byte-ROHSUCHE gefunden") — dieser Satz hält.

### 1.4 EDT / Tone / VAG — alle Zahlen stimmen bitgenau

```
ROOM21B0  EDT=0x5bc4 VH=0x5c84 VB=0x6aa4  nprog=2 ntone=18 nvag=18
  EDT+0x11*4 @0x5c08 : 00 00 e3 00   -> prog0 tone14 prio3 voice-16 (re15_edt_decode, vab_common.c:252-265)
  EDT+0x12*4 @0x5c0c : 00 00 f3 00   -> prog0 tone15
  tone14 @0x6664 : 00 00 5a 40 67 39 49 49 … ff 80 c0 1f 00 00 10 00   (vol90 pan64 cen103 shift57 min=max73 vag16)
  tone15 @0x6684 : 00 00 6e 40 65 00 4a 4a … ff 80 c0 1f 00 00 11 00   (vol110 cen101 shift0 min=max74 vag17)
  vag16 @0x1ba34  16400 B  sha1 ac0b9a13a8806802575e60f37e9691ef7b1ef846
  vag17 @0x1fa44   6336 B  sha1 5a9aca0d211de60a78871e7bff63371333ab4a3c
ROOMB1B0  vag6 @0x7d68 / vag7 @0xbd78 — SHA-1 identisch, EDT 0x434c/0x4350 identisch
```
Auch die Pitch-Rechnung aus §2.7 ist reproduzierbar: `s_pitch_lut[6][14]=0x17CC`,
`s_pitch_lut[9][0]=0x1AE8` (`re15_port/engine/src/vab_common.c:298/301`). ✔

### 1.5 Weiteres Bestätigtes

* ROOM1080 snd0 = 3 VAGs (48/1552/3840 B), snd1 = 15 VAGs (max 15392 B) — §2.2 stimmt.
* Se_on-Zähler: ROOM1080 = 0, ROOM1081 = 0, ROOM4020 = 0, ROOM4021 = 0, ROOM3080 = 3
  (`36 02 0a 00 02 00 …` @0x95C/0x970/0x984, selbst gelesen). Xa_on = 0.
* „An elevator" @ROOM21B0.RDT:0x304A: `1d 4a 00 41 48 41 52 3d 50 4b 4e`, mit a=0x3D /
  A=0x1D dekodiert = **An elevator**. ✔
* Nadelsuche §2.9 nachgestellt: dieselben zwei 512-B-Nadeln (`00 00 bd c2 f1 ed 0c ff` /
  `12 00 0f 2d 0c e2 4d 11`), 3193 Dateien unter `shared_assets/PSX`, **0 Treffer**;
  zusätzlich `info/Re1.5` (759 Dateien) und `shared_assets` gesamt (3430) — ebenfalls 0. ✔

---

## 2. Was FALSCH ist

### F1 — „495 RE2-Räume" / „alle 495 RE2-RDTs" (§2.4 und §6.3)

> „Die Byte-Folge kommt in **keinem** anderen der 495 RE2-Raeume … vor."
> „… walken 2334 von 2568 init+main-Regionen aller 495 RE2-RDTs sauber durch (91 %)"

`info/re2leon/PL0/RDT/` hat **495 Verzeichniseinträge**, davon **250 `*.RDT`-Dateien** und
**245 entpackte Raum-ORDNER** (z.B. `room21B0/` mit `scd`, `snd0.vb`, `msg`, …). Die 495 ist
eine Verzeichniszählung, keine Raumzahl. Das Ergebnis der Rohsuche kippt dadurch nicht
(ich habe alle 250 RDTs selbst durchsucht, Treffer wie oben), aber die Grundgesamtheit der
Opcode-Abdeckung in §6.3 ist damit nicht nachvollziehbar.

### F2 — „RE1.5 hat KEINEN" (Titel) ist nur SKRIPT-seitig geprüft

Der gesamte Negativ-Nachweis ist ein SCD-Zensus (0x36/0x37/0x38) plus eine Nadelsuche.
RE1.5 löst SE aber auch **direkt aus dem Stage-Overlay** aus, am SCD vorbei. Selbst gezählt
(`jal 0x80045024` = `0c 01 14 09`, der SE-Emitter):

```
STAGE1.BIN : 11 Aufrufe   z.B. @8010a7b8  3c040401 34840001 0c011409  = bank4 id1 flags1
STAGE4.BIN : 12 Aufrufe   z.B. @801115bc  3c040400 34840001 0c011409  = bank4 id0
STAGE3.BIN : 16, STAGE5.BIN : 19
```
(bank 2 = snd0 kommt dort ebenfalls vor: @8010a804 `3c040207` = bank2 id7.)
Diesen Pfad hat das Dossier **nie angesehen und auch nicht als Lücke genannt**. Belegt ist
„die Fahrt ist skript-stumm"; behauptet wird „RE1.5 hat KEINEN Fahrstuhl-Sound".

### F3 — „kein 0x38 (Xa_on)" (§2.1)

In der verwendeten RE1.5-Tabelle ist `0x38 = Flr_set` (12 B) und `Xa_on = 0x59`
(`re15_port/tools/scd_dump_room.py:33` bzw. `:57`). Die Aussage stimmt im Ergebnis
(Xa_on-Zähler = 0, selbst geprüft), die Opcode-Zuordnung ist falsch.

### F4 — das §2.1-Listing ist stillschweigend gekürzt

Es liest sich als lückenloser Walk mit „+offset"-Spalte, lässt aber weg:
* 0x6FA..0x722: **13 weitere `Set`** (Bank 5 bit0..8, Bank 2 bit0/bit2) und
  0x6F2 `Ifel_ck`, 0x736 `Work_set`, 0x739 `Nop`.
* zwischen `Plc_dest` @0x766 und `Aot_on` @0x77C: `Do` @0x76E, `Evt_next` @0x772,
  `Edwhile` @0x774, `Ck` @0x776, `Endif` @0x77A.

Die Offsets selbst stimmen; die Vollständigkeits-Anmutung nicht.

---

## 3. Was ÜBERZOGEN ist (Belege tragen weniger als der Satz)

### Ü1 — die negative Aussage ist BIT-EXAKT, wird aber allgemein formuliert

> „RE1.5 fuehrt diese Wellenformen also nirgends — sie muessen tatsaechlich aus RE2 kommen."

Gesucht wurde nach **zwei 512-Byte-Rohnadeln aus dem ADPCM-Rumpf**. Das Merkmal ist
**nicht stabil** gegen Re-Encoding, andere Sample-Rate, anderes Master oder anderen
ADPCM-Encoder — genau der plausibelste Fall bei einem RE2-**Prototyp**. Bewiesen ist
„keine bitgleiche Kopie", nicht „diese Wellenform existiert in RE1.5 nicht".

Das Merkmal **Größe** wurde offenbar gar nicht geprüft. Ich habe es nachgeholt (Inventar aller
VAB-Bänke aller 240 RE1.5-RDTs):
* keine RE1.5-VAG mit exakt 16400 oder 6336 B,
* aber **58 VAGs ≥ 16000 B**, größte **20832 B**.

RE1.5 hat also sehr wohl Samples dieser Länge — das Argument „so lange Wellen gibt es da
nicht" gilt nur für die zwei Bänke von ROOM1080 (§2.8), wo es korrekt formuliert ist, und
darf nicht auf den Baum verallgemeinert werden.

### Ü2 — die „Dauer-Gegenprobe" (§2.7) hat keine Trennschärfe, und sie passt nicht

| SE | Welle | Skriptfenster | Abweichung | Dossier |
|---|---|---|---|---|
| 0x11 | 3,50 s | 8+90 = 98 F = 3,27 s | **+7 %** | „Passt." |
| 0x12 | 1,20 s | 8+20 = 28 F = 0,93 s | **+29 %** | „Passt." |

Beide Wellen sind **länger** als ihr Fenster. Das widerlegt nichts (ein SE läuft über das
Skriptende weiter), bestätigt aber auch nichts: ein Test, den fast jedes Sample passender
Größenordnung besteht, ist kein Beleg. „Passt" ist hier ein Guess-Tell.

### Ü3 — „Es ist genau das Fahrstuhl-Fahrskript" (§2.4)

Die Rohsuche belegt: **diese 16 Bytes** stehen nur in diesen sechs Dateien. Dass die Bytes
das *Fahrskript* sind, folgt aus dem Kontext (Cut_chg, Member_set-Kabinenposition,
„An elevator"), nicht aus der Rohsuche. Das ist plausibel und gut gestützt — aber es ist eine
Interpretation, keine Messung, und wird als Messung präsentiert.

---

## 4. Risiko des Umsetzungsplans

**Der Haken sitzt im heißesten Handler des Ports.**
`op_set` = `re15_port/engine/src/scd_vm.c:2028-2056` ist der **einzige** Schreibpfad für
**alle** Spiel-Flags (Bank 0..7, OR/CLEAR/XOR) in allen 240 RDTs. Selbst gezählt: `Set` ist
mit **716 statischen Vorkommen allein in 40 STAGE1-Räumen** der dritthäufigste Opcode
(nach `Nop` 977 und `Ifel_ck` 746). Wer dort danebengreift, trifft Tür-, Event- und
Save-Latches, nicht den Fahrstuhl.

**Die bevorzugte Variante steht auf einem Feld, das es nicht gibt.**
Der Plan will `t->pc - rdt_base` gegen eine Ankertabelle vergleichen. `scd_thread_t` führt
aber nur `const uint8_t *pc` (`re15_port/include/re15_scd.h:68`); eine RDT-Dateibasis gibt es
im Thread nicht. Die einzige Raum-Handhabe ist die datei-statische
`s_current_rdt` (`re15_port/engine/src/scd_vm.c:149-150`), die **Sektionszeiger** hält
(`s_current_rdt->sub_scd[event_id]`, `scd_vm.c:583`). Wird die Basis versehentlich aus
`sub_scd[0]` statt aus dem Dateianfang gebildet, verschieben sich **alle sechs Anker** und der
Ton zündet an einem fremden `Set` — still, ohne Fehlermeldung. Das Dossier ahnt das
(„falls der rdt_base im VM-Thread nicht verfuegbar ist"), zieht daraus aber nicht den Schluss.

**Schritt 3 und Schritt 5 widersprechen sich.**
Schritt 3 legt die Welle in einen **eigenen** Ein-Bank-Slot (`ELEVSE.VBS`). Schritt 5 verlangt
als Nachweis die Zeilen `[se] Se_on bank=2 id=17` / `id=18`. Diese Zeile druckt
**ausschließlich** `op_se_on` (`re15_port/engine/src/scd_vm.c:1886`) — ein Haken in `op_set`
erzeugt sie nie. Die Abnahme kann so nicht scheitern und nicht bestehen.
Und ginge das Ereignis doch als `bank=2` durch die Normalkette, landete es in ROOM1080s
**eigener** snd0-EDT: die Records 0x11/0x12 dort sind `00 00 00 00` (EDT @0x2338, selbst
gelesen; belegt sind nur 0,1,2,5,6,26..31) → **stumm, ohne Fehlermeldung**.

**Entlastung: die „einfachere Variante" ist die bessere.**
`22 01 1c 01` kommt in ROOM1080 **genau 6×** (0x746/0x756/0x7d8/0x7e8/0x86a/0x87a) und in
ROOM4020 **genau 6×** (0x95a/0x96a/0xa00/0xa10/0xa96/0xaa6) vor — alle innerhalb der
SCD-Sektion (0x480..0x8dc bzw. 0x638..0xb78), alle an den Signaturstellen. Ein Raum-Gate
{0x1080,0x1081,0x4020,0x4021} plus Flankenzähler hat dort **nachweislich null Fehlauslöser**.
Die generierte Ankertabelle kauft nichts dazu und bringt die `rdt_base`-Falle mit.

---

## 5. Fehlende Messung

Ein Lauf des **echten** RE1.5 in ROOM1080 über die drei Fahrten mit mitgeschriebenem
SPU-Key-On — PCSX-Redux-Watchpoint auf den SE-Emitter `FUN_80045024` bzw. auf die
SPU-Voice-Register, oder DuckStation-Savestate-Diff über die Fahrt (Skill
`re15-pcsx-watchpoint` / `re15-room-capture`). Das ist die **einzige** Messung, die
„RE1.5 ist während der Fahrt tatsächlich stumm" belegt, statt nur „das Skript ruft kein
Se_on". Solange sie fehlt, ist der Titelsatz des Dossiers eine Vermutung mit sehr guten
Indizien — und der Import bleibt eine Ergänzung, die als Rekonstruktion deklariert werden
muss, nicht als byte-true Wiederherstellung.

## 6. Ergebnis

| Punkt | Urteil |
|---|---|
| 16 bitgleiche Bytes | **bestätigt**, selbst gelesen, Trefferliste vollständig reproduziert |
| Se_on bank=2 id=0x11/0x12 | **bestätigt**, Layout gegen RE1.5- **und** RE2-Handler (0x80056444) geprüft |
| EDT/Tone/VAG/SHA-1 | **bestätigt**, bitgenau, inkl. Doppelbeleg in ROOMB1B0 |
| Opcode-Tabelle reicht? | **ja** — die Fundstelle ist selbstverankert, die Tabelle wird nicht gebraucht |
| „495 RE2-RDTs" | **falsch** — 250 RDTs (495 = Verzeichniseinträge inkl. 245 Ordner) |
| „RE1.5 hat KEINEN" | **nicht getragen** — Overlay-Emitter-Pfad (11 bzw. 12 `jal 0x80045024`) nie geprüft |
| „Welle kommt im Baum nicht vor" | **nur bit-exakt** — Nadelmerkmal instabil gegen Re-Encoding; Größenmerkmal ungeprüft (58 RE1.5-VAGs ≥16 kB) |
| „Passt" (Dauerprobe) | **überzogen** — +7 % / +29 % Abweichung, kein trennscharfer Test |
| Plan-Risiko | Haken im hottesten Handler; `rdt_base` existiert nicht im Thread; Schritt 3 ⇄ Schritt 5 widersprüchlich |

**haltbar = false.**
