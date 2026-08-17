# Save-Zähler bei NEW GAME — Dossier

Nutzer-Report (2026-08-03): „Bei New Game, wenn ich auf einen bestehenden Save
speichere/überschreibe, wird der Zähler vom aktuellen Save hochgezählt. Aber bei
einem neuen Spiel soll der Zähler natürlich von vorne losgehen."

Status: **Bug im Port reproduziert (Probe), Original-Mechanismus vollständig
disassembliert und zitiert, Fix-Plan unten. Engine-Code NICHT geändert** (nur
Diagnose-Probe `re15_port/tests/unit/probe_save_counter.c` + dieses Dossier).
→ Der Fix-Plan aus §3 ist inzwischen umgesetzt (main.c:657/2141/2919/3112/3129)
und seit 2026-08-17 durch `integration_save_counter_pin` eingefroren (§5).

---

## 0. NACHTRAG 2026-08-17 — Report „nach dem Laden wieder dieselbe Nummer"

Nutzer-Report: „Beim Laden eines Spielstandes und Speichern auf einen NEUEN Slot
schreibt er wieder die gleiche Nummer, statt hochzuzählen."

**Analyse-Ergebnis: KEIN Port-Fehler — das war byte-true der Original-Mechanismus.**
Der Zähler ist LIVE-Spielzustand, kein Karten-/Slot-Zustand; ein LOAD setzt ihn
auf den im Save gespeicherten Stand zurück, und gespeichert wird PRE-Inkrement.
Also trägt der erste Save nach einem Load nochmal dieselbe Nummer, der nächste
zählt wieder hoch.

**→ NUTZER-ENTSCHEID 2026-08-17: bewusste ABWEICHUNG, siehe §6.** Der Port zählt
ab jetzt monoton hoch. Die Original-Beleglage unten bleibt unverändert gültig und
ist der Maßstab dafür, was die Abweichung genau ändert (und was nicht).

### 0.1 MESSUNG (echte `re15_pc.exe`, zwei Läufe, Karten-Bytes gelesen)

Karte `re15_card.mcr`, Slot s → Block (s+1)*0x2000, Blob ab +0x100,
`save_count` (u16 LE) bei Blob+26 = Block+0x11a; Titel-Ziffern Block+0x29/+0x2b.

```
Lauf 1  NEW GAME, zwei Saves in EINER Sitzung (RE15_CARD_SLOT=0,1)
  [save] saved (room 1240) slot n=0 -> next=1
  [save] saved (room 1240) slot n=1 -> next=2
  slot0: save_count=0  Titel-Ziffern 0 0   (SJIS 82 4f 82 4f)
  slot1: save_count=1  Titel-Ziffern 0 1   (SJIS 82 4f 82 50)   ← zählt hoch

Lauf 2  CONTINUE aus Slot 1 (Nummer 1), zwei Saves (RE15_CARD_SLOT=1,2,3)
  [save] CONTINUE: resumed in room 1240
  [save] saved (room 1240) slot n=1 -> next=2      ← WIEDER 1  = der Report
  [save] saved (room 1240) slot n=2 -> next=3
  slot2: save_count=1   slot3: save_count=2  Titel-Ziffern 0 2
```

Der gemeldete Effekt tritt also **genau einmal** auf — beim ersten Save nach
einem Load; innerhalb einer Sitzung zählt der Port korrekt hoch.

### 0.2 BELEG, dass das Original genau das tut

Zusätzlich zu §2 (Store/Inkrement/Restore) diesmal explizit nachgezogen:

* **Die Karte bekommt den Pre-Inkrement-Stand, nicht den inkrementierten.**
  Der Karten-Write `jal FUN_800271a8` @0x80026224 bekommt als Quelle
  `a0 = 0x800b0dbc` (@0x800261e4-ec: `lui t0,0x800b` / `addiu t0,t0,4028`
  = 0x800b0fbc / `addiu s0,t0,-512`), also **Live-RAM**: der Zähler
  DAT_800b0fbd liegt bei Block+0x201. Das Inkrement @0x80026488-9c läuft
  erst danach im Erfolgszweig (`beq v0,zero,0x80026480` @0x80026230).
* **Kein Async-Schlupfloch:** FUN_800271a8 ist synchron —
  `open`/`write(fd,param_1,param_5)`/`close` + Verify-`read` 0x200
  (RE_15_Quellcode_V2/FUN_800271a8.c). Der inkrementierte Wert kann die
  Karte nicht mehr erreichen.
* **Der LOAD überschreibt den Zähler wholesale** (@0x80026290-a0,
  `memcpy(&DAT_800b0dbc, buf, 0x1430)`; 0x800b0dbc+0x201 = 0x800b0fbd) und
  springt am Inkrement **vorbei** (@0x800262a4 `j 0x800264a0`).
* **Nur 4 Referenzen** auf DAT_800b0fbd im ganzen Spiel (ghidra1_V2.txt:492892):
  R/W-Inkrement @0x8002648c/@0x8002649c + die zwei Titel-Reads
  @0x80026eac/@0x80026ecc. Kein Overlay referenziert die Adresse
  (grep `800b0fbd` über `RE_15_Quellcode_Overlays/`: leer). Es gibt also
  keinen zweiten Schreiber, der die Nummer aus der Karte ableiten könnte.
* **Die angezeigte Nummer ist der rohe Zähler** (kein +1): Kartentitel
  @0x80026ec8/@0x80026f00, In-Game-Slot-Zeile FUN_80026658 @0x80026834
  (`lbu t0,1(s0)`, s0 = GSB-Kopie, +1 = DAT_800b0fbd) + `divu` 10
  @0x8002683c-64.
* **Folge im Original** (unvermeidbar aus obigem): zwei Karten-Blöcke können
  dieselbe Nummer tragen; wer immer nur „letzten Stand laden → einmal
  speichern → beenden" spielt, sieht die Nummer nie steigen. Das Original
  zählt Saves **des laufenden Spielstrangs**, nicht Saves der Karte.

### 0.3 OFFEN (mit Adresse)

* RE2-Vergleichswert weiterhin **nicht lokalisiert**. Gesucht in
  `info/re2leon/COMMON/BIN/MEM_CARD.BIN` (Overlay-Basis ~0x801c0000): die
  vier `0xcccccccd`-Div-10-Stellen dort (f+0x22c8, f+0x263c, f+0x3648,
  f+0x3730) sind **Listen-/UI-Nummerierung**, nicht der Save-Zähler — die
  Ziffern bei @0x801c22c8-2318 stammen aus `s3+1` mit
  `s3 = <Stack-Basis> + lbu 21(s4)` (@0x801c22a4-b0), also einem
  Eintrags-Index; die Byte-Inkremente @0x801c1320/@0x801c133c sind
  Screen-Timer (`sb ...,2(s2)`-Nachbarn). Für die RE1.5-Semantik nicht
  nötig: RE1.5s eigener Karten-Code spezifiziert sie vollständig.

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
  Beleglage vollständig). Suchstand 2026-08-17 in §0.3.

---

## 5. PIN (2026-08-17) — `integration_save_counter_pin`

`re15_port/tests/integration/test_save_counter_pin.cmake` (+ CMakeLists-Eintrag)
fährt **zwei echte `re15_pc.exe`-Läufe** (~15 s) und prüft ZWEI unabhängige
Kanäle statt eines nachgebauten Modells:

* **Kanal A — die geschriebenen Nummern** aus der Prozess-Logzeile
  `[save] saved (room ....) slot n=<N> -> next=<M>`: Lauf 1 muss `0;1` liefern,
  Lauf 2 (CONTINUE aus Nummer 1) `1;2`. Das ist exakt der Nutzer-Report.
* **Kanal B — die Karten-Bytes** (das Produkt):

| Slot | erwartet | pinnt |
|------|----------|-------|
| 0 | 0 | neues Spiel startet bei 0 (EXE-Image @Datei 0xa17bd) + Store ist PRE-Inkrement |
| 1 | 1 | zweiter Save DERSELBEN Sitzung zählt hoch (@0x80026488-9c) |
| 2 | 1 | LOAD restauriert den Zähler wholesale (@0x80026290-a0) — nicht 0, und nicht 2 (kein Karten-Seed) |
| 3 | 2 | danach läuft die Zählung des geladenen Standes weiter |

Zusätzlich die Kartentitel-Ziffern von Slot 3 („0 2", Basis Vollbreiten-'0'
0x824f, @0x80026ec8/@0x80026f00).

**Gegenproben gemessen (Pin wird ROT):**

| Eingriff in `main.c` | Pin-Meldung |
|----------------------|-------------|
| Abweichung zurückgebaut (`+ 0` statt `+ 1` im LOAD-Restore) | „Lauf 2 … schrieb die Nummern '1;2', erwartet '2;3'" |
| Post-Inkrement entfernt (`scount + 0`) | „Lauf 1 … schrieb die Nummern '0;0', erwartet '0;1'" |
| LOAD-Restore ganz entfernt | „Lauf 2 … schrieb die Nummern '0;1', erwartet '2;3'" |

(Die letzten beiden auch über Kanal B gemessen, bevor Kanal A dazukam:
Slot1 = 0 statt 1 bzw. Slot2 = 0 statt 1.)

Harness-Haken in `main.c` (reine Testhaken, kein Spielverhalten):
`RE15_CARD_SLOT="a,b,c"` (Ziel-Slot je Karten-Screen beim Auto-Drive),
`RE15_SAVE_TEST_AGAIN=<frame>` (zweiter Save in derselben Sitzung) und
`RE15_SAVE_TEST_EXIT_AFTER=<n>` (deterministisches Prozessende nach n Saves —
der zuvor benutzte Weg über `RE15_KILL_AT`+`RE15_BOOT_EXIT_AT` war flakey);
dazu trägt die `[save] saved`-Logzeile jetzt `n=<geschriebene Nummer> ->
next=<Zähler>`.

**Zwei Werkzeugfallen, die dabei Zeit gekostet haben** (für künftige Pins):
* `file(STRINGS)` auf `debug.log` verschluckt stumm alles ab dem ersten
  BINÄR-Byte — die späteren `[save]`-Zeilen fehlten, obwohl sie in der Datei
  stehen. Der Pin liest das Log darum binär-sicher als HEX.
* Der frisch überschriebene 128-KB-Kartenblob war direkt nach Prozess-Ende
  gelegentlich noch mit dem vorigen Inhalt sichtbar (Windows-Dateisicht);
  der Pin liest die Karte darum mit begrenzter Wiederholung.

---

## 6. NUTZER-ENTSCHEID 2026-08-17 — Abweichung: Save-Nummer zählt monoton

**Entscheidung:** Die Save-Nummer soll **monoton hochzählen** — auch der erste
Save nach einem Load steigt um 1. Das ist eine **bewusste PORT-ABWEICHUNG** vom
Original (Präzedenz: die Speicherort-Namen 2026-08-08, Muster `re15_savepoint.c`).

### 6.1 Original vs. Port — nebeneinander

| | Original (belegt) | Port ab 2026-08-17 |
|---|---|---|
| Neues Spiel | 0 (EXE-Image @Datei 0xa17bd = 00) | **0 — unverändert** |
| Save schreibt | Zähler PRE-Inkrement (Write-Quelle Live-RAM 0x800b0dbc, @0x800261ec/@0x80026224) | **unverändert** |
| Nach erfolgreichem Write | Zähler +1 (@0x8002648c `lbu` / @0x80026494 `addiu 1` / @0x8002649c `sb`, Erfolgszweig @0x80026230) | **unverändert** |
| LOAD | Zähler = gespeicherter Stand (memcpy @0x80026290-a0, Block+0x201) und **springt am Post-Inkrement vorbei** (@0x800262a4 `j 0x800264a0`) | **Zähler = gespeicherter Stand + 1** ← die Abweichung |
| Ziffernbau/Anzeige | roher Zähler, kein +1 (@0x80026ec8/@0x80026f00; In-Game @0x80026834) | **unverändert** |
| Karte/Ziel-Slot | geht in die Nummer NIE ein | **unverändert** |
| Folge | Laden(N) → Save schreibt N erneut | Laden(N) → Save schreibt **N+1** |

### 6.2 Umsetzung

**Eine** Stelle: `re15_port/platform/pc/main.c`, LOAD-Restore im CONTINUE-Zweig
(`s_save_counter = (uint16_t)(s_resume_sd.save_count + 1);`), Kommentar dort als
`[PORT-ABWEICHUNG, Nutzer-Entscheid 2026-08-17]` mit den Original-Adressen.

**Warum dort und nicht am Post-Inkrement** (der zunächst genannte Einzeiler): ein
`+2` am Post-Inkrement (`s_save_counter = scount + 2`) würde auch die Saves
*innerhalb* einer Sitzung um 2 springen lassen (0, 2, 4 …). Die Abweichung gehört
genau an die Stelle, an der das Original den Stand zurücksetzt — so bleibt jede
andere Semantik byte-true, und die Zählung ist lückenlos.

### 6.3 Gemessene Soll-Sequenz (drei exe-Läufe, eine Karte)

```
Lauf 1  NEUES SPIEL, zwei Saves      slot0 = 0  Titel 82 4f 82 4f  (/00/)
                                     slot1 = 1  Titel 82 4f 82 50  (/01/)
Lauf 2  CONTINUE aus slot1 (=1)      slot2 = 2  Titel 82 4f 82 51  (/02/)
                                     slot3 = 3  Titel 82 4f 82 52  (/03/)
Lauf 3  CONTINUE aus slot3 (=3)      slot4 = 4  Titel 82 4f 82 53  (/04/)
```

Randfälle damit abgedeckt: neues Spiel startet weiter bei der Original-Startnummer
(0 → `/00/`); Laden→Speichern→Laden→Speichern zählt durchgehend hoch (Lauf 3);
die Kartentitel-Ziffern folgen dem neuen Wert (der Ziffernbau selbst bleibt
byte-true, `re15_mc_title.c`).
