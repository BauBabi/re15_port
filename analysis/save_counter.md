# Save-Zähler bei NEW GAME — Dossier

Nutzer-Report (2026-08-03): „Bei New Game, wenn ich auf einen bestehenden Save
speichere/überschreibe, wird der Zähler vom aktuellen Save hochgezählt. Aber bei
einem neuen Spiel soll der Zähler natürlich von vorne losgehen."

Status: **Bug im Port reproduziert (Probe), Original-Mechanismus vollständig
disassembliert und zitiert, Fix-Plan unten. Engine-Code NICHT geändert** (nur
Diagnose-Probe `re15_port/tests/unit/probe_save_counter.c` + dieses Dossier).

---

## 1. MESSUNG — Port-IST (Bug reproduziert)

Probe: `re15_port/tests/unit/probe_save_counter.c` (Build-Target
`probe_save_counter`, kein ctest). Sie repliziert die Save-Nummern-Logik aus
`re15_port/platform/pc/main.c` Zeile für Zeile gegen das echte
memcard-Backend (`re15_memcard.c`/`re15_savedata.c`). Lauf 2026-08-03:

```
[M1] NEW-GAME-Erst-Save: Port-scount = 8 (Karte: max=7 in slot0, 3 in slot2)
[M2] Neuer-Spielstand in LEEREM Slot 1 traegt save_count=8
[M3] CONTINUE(7) -> NEW GAME -> erster Save: Port-scount = 8 (Original: 0)
probe_save_counter: BUG REPRODUZIERT (fails=0)
```

### Warum der Port den Zähler erbt — zwei Pfade, beide in `main.c`

Der Save-Zähler des Ports ist ein **Datei-static in main.c, kein Teil des
Spielzustands**:

- `main.c:491` — `static uint16_t s_save_counter = 0;` (Kommentar: „0 = not yet
  seeded from a load").
- **Pfad A (frischer Prozess, NEW GAME):** beim ersten Save der Session ist
  `s_save_counter == 0`, und `main.c:2626` **liest das Maximum über ALLE
  Karten-Slots**:
  ```c
  if (s_save_counter == 0) s_save_counter = (uint16_t)re15_memcard_max_save_count(RE15_CARD_PATH);
  int scount = (int)s_save_counter + 1;                       /* main.c:2627 */
  ```
  `re15_memcard_max_save_count` (`re15_memcard.c:113-129`) iteriert alle 5
  Slots und nimmt das größte `save_count`-Feld. → Ein frisches Spiel erbt den
  Zähler der ALTEN Saves auf der Karte (Messung M1/M2: 8 statt Neu-Start),
  egal in welchen Slot gespeichert wird — auch in einen leeren.
- **Pfad B (in-Prozess, CONTINUE → NEW GAME):** `main.c:2526` setzt beim Laden
  `s_save_counter = s_resume_sd.save_count;` (korrekt, s.u.), aber es gibt
  **keinen Reset-Pfad bei NEW GAME**: weder der Title-NEW-GAME-Zweig
  (`main.c:1767-1781`, `re15_gameflow_new_game`) noch der Welt-Reset nach
  Game-Over (`main.c:4030-4032`, memset nur `g_inv`+`g_game`) fasst
  `s_save_counter` an (grep: einzige Vorkommen 491/2526/2626/2627/2644).
  → Messung M3.

Die Antwort auf die Diagnose-Frage „liest der Port den Zähler aus dem
Ziel-Slot statt aus dem laufenden Spielzustand?": **Ja, der Sache nach** — er
liest ihn aus der Karte (Max über alle Slots, was beim Überschreiben des
höchsten Slots dem Ziel-Slot entspricht) bzw. behält ihn prozess-lokal über
den New-Game-Übergang hinweg. Der Original-Mechanismus kennt beides nicht.

---

## 2. ORIGINAL — RE1.5 PSX.EXE, Memory-Card-Treiber `FUN_80025c00`

RE1.5s eigenes (retail dormantes, aber vollständig codiertes) Karten-System.
Einziger Aufrufer: `jal FUN_80025c00 @0x8001cba4` (in FUN_8001c958, dem
Save-Trigger-Konsumenten — deckungsgleich mit `psx_dev/re2_port/re15_addrs.h`,
das `G_SAVE_COUNT 0x800B0FBD` bereits so führt).

### 2.1 Wo der Save-Count lebt: im LIVE-Game-State-Block (GSB)

- Zähler = **`DAT_800b0fbd`**, das Byte direkt nach der
  Inventar-Kapazität `DAT_800b0fbc` (= 10, live; vgl. `menu_common.c:72`
  „lbu @0x800c63e0"). Beide liegen im Game-Work: `FUN_80030494` liest
  `lhu v0,0x4870(s0)` mit s0=0x800ac75c → 0x800b0fcc, d.h. Zähler =
  Work-Base 0x800ac75c + 0x4861.
- Startwert **0 aus dem EXE-Image selbst**: PSX.EXE t_addr/t_size
  `0x80010000/0xaf000` (Header @0x18), 0x800b0fbd → Datei-Offset
  **0xa17bd**, Byte dort = `00` (xxd-verifiziert).

### 2.2 SAVE (case 8, @0x800261b4 ff.): Quelle ist IMMER der laufende Spielzustand

```
800261b4 jal FUN_80026f48          ; GSB-Header aus LIVE-Globals auffrischen:
                                   ;   DAT_800b0fbe = DAT_800aca5c (Charakter)
                                   ;   DAT_800b0fc0/c2/c4 = DAT_800aca88/8c/90 (Spieler-XYZ)
                                   ;   (RE_15_Quellcode_V2/FUN_80026f48.c — fasst 0fbd NICHT an)
800261c4 lui  a1,0x800b
800261c8 addiu a1=>DAT_800b0fbc,a1,0xfbc
800261d0 ori  a2,zero,0x1230
800261d4 jal  FUN_8004ee38          ; memcpy(card_buf+slot*0x80+0x1430, &DAT_800b0fbc, 0x1230)
800261f0 jal  FUN_80026e54          ; Karten-Titel bauen (nutzt DAT_800b0fbd, s.u.)
80026224 jal  FUN_800271a8          ; Karten-Write
80026230 beq  v0,zero,LAB_80026480  ; nur bei ERFOLG:
80026488 lui  v0,0x800b
8002648c lbu  v0,offset DAT_800b0fbd(v0)
80026494 addiu v0,v0,0x1
8002649c sb   v0,offset DAT_800b0fbd(at)   ; DAT_800b0fbd++ NACH dem Write
```

(`FUN_8004ee38` = byte-memcpy, `RE_15_Quellcode_V2/FUN_8004ee38.c`.)

**Der Ziel-Slot/die Karte wird für die Nummer NIE gelesen.** In den Save-Block
wandert der PRE-Inkrement-Stand des laufenden Spiels; erst nach erfolgreichem
Write wird der RAM-Zähler erhöht. Titel-Digits ebenfalls pre-inkrement:
`FUN_80026e54` @0x80026eac/0x80026ecc addiert `DAT_800b0fbd / 10` und `% 10`
auf die „/NN/"-Zeichen (RE_15_Quellcode_V2/FUN_80026e54.c:14-15) — der
allererste Save eines Spiels heißt also **/00/**.

### 2.3 LOAD (case 9, @0x80026240 ff.): Zähler kommt wholesale aus dem Save zurück

```
80026270 jal  FUN_80027368          ; Karten-Read → Puffer
80026290 addiu a0=>DAT_800b0dbc,a0,0xdbc
800262a0 _ori a2,zero,0x1430
8002629c jal  FUN_8004ee38          ; memcpy(&DAT_800b0dbc, buf, 0x1430)
```

0x800b0dbc + 0x200 = 0x800b0fbc → der Restore überschreibt den GSB inklusive
`DAT_800b0fbd`. Ein Load-then-Save setzt die Zählung des geladenen Standes
fort — genau das, was `main.c:2526` im Port korrekt nachbildet.

### 2.4 NEW GAME im Original

`DAT_800b0fbd` hat laut Ghidra-Xref-Liste (ghidra1_V2.txt:492892) **genau
vier Referenzen**: Inkrement R/W @0x8002648c/0x8002649c und Titel-Reads
@0x80026eac/0x80026ecc; ein Writer über Work-Base+0x4861 existiert nicht
(grep `0x4860/0x4861` über Dump + Decompile-Korpus: leer). Initialisierung =
das EXE-Image-Byte 0 (Datei-Offset 0xa17bd). Einen dedizierten
NEW-GAME-Clear des Zählers gibt es nicht (das Karten-System ist retail
dormant; der raum-/session-scope Clear FUN_8003ecec @0x8003ed74 nullt nur das
Flag-Wort 0x800b1028 usw., nicht 0x800b0fbd) — **OFFEN** bleibt allein, ob
ein Dev-Build-Flow den GSB bei New Game zusätzlich wischte. Für die
Semantik ist das unerheblich: **der Zähler ist Zustand des laufenden Spiels
(frisch = 0), niemals Karten-/Slot-Zustand.**

RE2-Leon-Vergleich (Port nutzt das RE2-Save-MODELL): der eigene
RE2-Zähler wurde in begrenzter Suche nicht lokalisiert (kein `bu00:`-String,
`_bu_init` nur @0x8002b574; kein /NN/-Digit-Builder im Korpus) — **OFFEN,
aber nicht benötigt**: RE1.5s eigener Karten-Code spezifiziert den
Mechanismus vollständig, und `re15_savedata.h:50` dokumentiert das
`save_count`-Feld selbst gegen „RE1.5 DAT_800b0fbd".

---

## 3. FIX-PLAN (main.c, kein Engine-Code — Umsetzung ausstehend)

Modell: `s_save_counter` verhält sich wie `DAT_800b0fbd` — Teil des
laufenden Spielzustands.

1. **Karten-Seed streichen** — `main.c:2626`
   (`if (s_save_counter == 0) s_save_counter = re15_memcard_max_save_count(...)`)
   ersatzlos entfernen. Das Original liest die Nummer nie von der Karte
   (SAVE-Quelle = live-RAM memcpy @0x800261c4-d8). Damit wird
   `re15_memcard_max_save_count()` (re15_memcard.c:113-129, re15_memcard.h:46-48)
   toter Code → entfernen; sein Header-Kommentar („the next save is this + 1")
   beschreibt den falschen Mechanismus.
2. **Reset bei NEW GAME** — im Title-NEW-GAME-Zweig (`main.c:1767-1781`, vor
   `re15_gameflow_new_game(ch)`) `s_save_counter = 0;` setzen. Beleg:
   frischer Spielzustand hat Zähler 0 (EXE-Image-Byte @Datei-Offset 0xa17bd
   = 00; einziger Inkrement-Writer @0x8002649c läuft erst nach einem Save).
   CONTINUE bleibt unberührt: `main.c:2526` (= Original-Restore
   @0x80026290-a0) seed't danach ggf. wieder aus dem geladenen Save.
3. **Byte-true Inkrement-Semantik (Zweitbefund)** — Original speichert den
   PRE-Inkrement-Stand und inkrementiert NACH Erfolg (memcpy @0x800261c4-d8
   vor `sb` @0x8002649c; Titel /NN/ pre-inkrement @0x80026eac/ecc → erster
   Save = /00/). Der Port speichert derzeit `s_save_counter + 1`
   (`main.c:2627-2628`) — Anzeige um 1 verschoben (/01/ beim ersten Save).
   Byte-true: `sd.save_count = s_save_counter;` speichern, bei Erfolg
   (`main.c:2644`, entspricht dem beq-Erfolgspfad @0x80026230→0x80026480)
   `s_save_counter++`. Betrifft Anzeige `%02d` in `pc_do_save`
   (`main.c:505`) und die Slot-Liste (`main.c:594`) automatisch mit.
4. **Verifikation** — `probe_save_counter` erneut laufen lassen: M1 muss
   dann scount=0 (bzw. Anzeige /00/) UNABHÄNGIG vom Karteninhalt liefern,
   M3 ebenso 0 nach New Game; zusätzlich Load(7)→Save = 7 speichern,
   danach RAM 8 (Fortsetzungs-Fall, Original case 9 → case 8).

## 4. Offene Punkte

- OFFEN: ob ein (dev-only) Flow im Original den GSB bei in-Prozess-NEW-GAME
  zusätzlich nullt — kein Clear gefunden (alle 4 Xrefs enumeriert, kein
  base+0x4861-Writer); retail un-exercisable, für die Fix-Semantik irrelevant.
- OFFEN: Lage des RE2-Leon-eigenen Save-Zählers (nur Vergleichswert; RE1.5-
  Beleglage vollständig).
