# Typ 0x42 („Ada") — Eskorten-Animation: belegt / bestritten / offen

Adjudikation 2026-08-28. **[eigen]** = in dieser Sitzung selbst disassembliert bzw. selbst
aus den Bytes gelesen.

> Namens-Hinweis: das RE1.5-Skript nennt sie nur „Woman"; „Ada" ist eine Community-
> Etikettierung (siehe `analysis/nutzer_batch_2026-08-26/ada-folgt.md:46`). Belegt ist
> nur: **Typ 0x42, Root 0x8011cb70, CDEMD0.EMS Blob 19**.


> ⛔ **Stand 2026-08-25 14:37: ein paralleler Agent hat den Fix bereits committet
> (`f4010659`).** Dieser Bericht ist damit teilweise eine Nachpruefung eines gelieferten
> Stands, nicht nur eine Vorarbeit. Details in `ZUSAMMENFASSUNG.md`, Abschnitt
> „WAEHREND DIESER ADJUDIKATION".

---

## A. BELEGT

### A1 — Identitaet und Modell **[eigen]**

    8011e924: addiu v0,v0,-13456     ; v0 = 0x8011cb70
    8011e92c: sw    v0,11444(at)     ; 0x80072cb4 = dispatch[0x42]

LBA-Tabelle CDEMD0: `0x80072f38 + (0x42-0x10)*8 = 0x800730c8` **[eigen]**
`read 0x800730b8 --w 4` = `[1435, 133400, 0, 0, 1501, 162524]`
→ Typ 0x40 = LBA 1435, **Typ 0x42 = LBA 1501 / 162524 B** = CDEMD0.EMS @Datei 0x2EE800.

CDEMD1-Zweig **[eigen]**: Basis `0x80073178` (`addiu at,at,12664` @0x800223e0),
Eintrag fuer 0x42 = `0x80073178 + 0x190` = **`0x80073308`** = `[1505, 144032]`.
`read 0x800732f0 --w 4` = `[0, 0, 1439, 133400, 0, 0, 1505, 144032, 0, 0]`
→ die im Befund genannte Adresse `0x800732f8` traegt `[1439, 133400]` = **Typ 0x40**.
Die Werte stimmen, die Adresse ist um zwei Typ-Slots falsch.

Baenke (Blob 19, 9 dir-Eintraege, Verzeichnis am Blob-ENDE): Container-Bank
`dir[1]/dir[2]` = 24 Clips `[34,32,50,20,20,20,50,1,1,1,1,25,1,1,1,1,1,10,25,1,1,1,30,30]`;
BANK 1 `dir[3]/dir[4]` = 6 Clips `[22,16,52,1,105,30]`; 15 Bones, kf_size 80.
(Von Pruefer [belege] unabhaengig aus den Bytes nachgezaehlt; von mir nicht.)
⚠ Das Laengenwort IM Blob ist 162488; die 162524 ergeben sich erst mit der 36-Byte-
Verzeichnis-Fussleiste — der Befund praesentierte das als Direkt-Treffer.

### A2 — Zustands-Tabelle **[eigen]**

`table 0x80121668` = `{0x8011ccac (INIT), 0x8011ce54 (Eskorte), 0x8011d018, 0x8011d060, 0x80050be8}`

`[4] = 0x80050be8` ist die geteilte EXE-NPC-Bibliothek (Plc_motion-Executor).

### A3 — ⛔ Der INIT endet nicht immer in State 1 **[eigen — DAS ist der entscheidende Befund, und weder der Befund noch der Pruefer [messung] hatten ihn]**

    8011ccb8: ori  v0,zero,0x1
    8011ccc0: sb   v0,4(v1)        ; +0x4 = 1     (unbedingt, am Funktionsanfang)
    ...
    8011ce1c: lbu  v0,9(v1)        ; grid_id
    8011ce24: andi v0,v0,0x40
    8011ce28: beq  v0,zero,0x8011ce44
    8011ce2c: ori  v0,zero,0x4
    8011ce30: sb   v0,4(v1)        ; +0x4 = 4     <-- State 4 statt Eskorte
    8011ce3c: ori  v0,zero,0x6
    8011ce40: sb   v0,5(v1)        ; +0x5 = 6

Der Pruefer [messung] las nur bis 0x8011cd90 und schloss daraus, es gebe kein
Deskriptor-Tor. Es gibt eines — am Funktionsende.

### A4 — Woher grid_id kommt **[eigen]**

`Sce_em_set` @0x800420a0:

    800420e8: lw   a1,28(s5)       ; a1 = Thread-pc  (Record-Basis)
    800420f0: lbu  v0,1(a1)        ; pc[1]: Slot = &0x7f, Flag = &0x80
    800420f4: addiu s2,a1,2        ; s2 = pc+2
    80042120: lbu  a1,7(a1)        ; pc[7] = Spawn-Flag-Byte
    80042128: beq  a1,v0,0x8004215c ;  == 0xFF -> kein Flag-Tor
    80042130: jal  0x8004efe4      ; sonst Flag pruefen
    80042138: beq  v0,zero,0x80042154 ;  gesetzt -> Spawn ABBRECHEN
    8004215c: lbu  v0,1(s2)        ; = pc[3]
    80042164: sb   v0,9(s0)        ; +0x9 = grid_id = pc[3]

**ROOM1090.RDT @0x22CC** (eigenes `xxd`):

    000022c0: 0000 0000 0000 0000 0000 0000 4400 4240
    000022d0: 0100 00ff d08a 0000 ...

→ `44 00 42 40 01 00 00 ff` = Slot 0, Typ 0x42, **grid_id = 0x40**, Flag-Byte 0xFF.
Bit 0x40 ist gesetzt → der INIT endet in **State 4 / Sub 6**, nicht in der Eskorte.

Von allen Typ-0x42-Rekorden, die ich in der SCD-Region gefunden habe, traegt genau einer
grid_id 0: **ROOM3070 @0x343c** (`44 00 42 00 ...`). Alle anderen tragen 0x40.
(Der Befunds-Zensus „20 Rekorde in 15 Raeumen" ist von mir NICHT nachgezaehlt — mein
Byte-Scan produziert Falschtreffer.)

### A5 — State 1 IST in ROOM1090 erreichbar — ueber sub03 **[eigen — hier widerspreche ich dem Pruefer [messung]]**

Aus dem repo-eigenen vollstaendigen SCD-Walk
`analysis/nutzer_batch_2026-08-27/room1090_scd_full.txt` (sub03 = 0x24CE..0x26E6):

    0x0026A4  Work_set  kind=2 idx=0     ; 2e 02 00  -> Gegner-Array Slot 0 = der 0x42-Aktor
    0x0026A8  Plc_neck  ...
    0x0026B2  Sleep     ...
    0x0026B6  Plc_neck  ...
    0x0026C0  Sleep     ...
    0x0026C4  Plc_ret                    ; 42        -> +0x4=1, +0x5..7=0  = ESKORTE

Der `Work_set kind=2 idx=0` steht unmittelbar davor und ist bis zum `Plc_ret` nicht
ueberschrieben. Das ist der Weg, auf dem die Eskorte im Original startet.
Die Messung des Pruefers [messung] („State 1 in 0 von 8 Subs erreicht") misst den **PORT**,
nicht das Original — und ist damit ein Hinweis auf einen Port-Defekt, kein Beweis der
Unerreichbarkeit.

### A6 — Die Eskorten-Subs posieren aus BANK 1 **[Pruefer-verifiziert, woertlich]**

Alle fuenf EXECs laden `a0 = +0x170` / `a1 = +0x174`:
@0x8004f384/88 (Sub 0), @0x8004f5c0/c4 (Sub 1), @0x8004f7bc/c0 (Sub 2),
@0x8004fb14/18 (Sub 3), @0x8004ff68/6c (Sub 5).
`FUN_80022300`: `dir[3] -> +0x174` @0x800224b8, `dir[4] -> +0x170` @0x800224c8;
Gegenprobe im selben Block: `dir[1] -> +0x16c` @0x80022498, `dir[2] -> +0x84` @0x800224a8.

⚠ **NICHT** „ausschliesslich": der INIT selbst posiert aus BANK 0
(`lw a0,132(v0)` @0x8011cdd0 = +0x84, `lw a1,364(v0)` @0x8011cdd4 = +0x16c,
`jal 0x8001f314` @0x8011cdd8). Korrekt ist: **nur die Subs des State-1-Wrappers
0x8011cf20 posieren aus +0x170/+0x174.**

### A7 — Phasen-Tor `+0x6` **[eigen fuer EXEC[3], Pruefer-verifiziert fuer EXEC[0]/[1]]**

EXEC[1] @0x8004f4e0:

    8004f4f0: lbu v1,6(a0)
    8004f4f8: beq v1,zero,0x8004f510    ; Phase 0 -> Seed
    8004f500: beq v1,v0,0x8004f548      ; Phase 1 -> nur Rumpf
    8004f508: j   0x8004f5d8            ; sonst gar nichts

Phase-0-exklusiv sind NUR die vier Seed-Schreiber: `+0x6=1` @0x8004f514,
`+0x94=5` @0x8004f524, `+0x95=0` @0x8004f534, `+0x8f=7` @0x8004f544.
Der Rumpf ab 0x8004f548 laeuft in BEIDEN Phasen und enthaelt Tempo-Tabelle,
Yaw-Slew (`jal 0x8001aac4` @0x8004f5ac) und `jal 0x800245d8` @0x8004f5d0.
→ Die Befunds-Formulierung „ab Phase 1 ruft der Sub nur noch anim_set" ist falsch,
die tragende Folgerung (kein Clip-Reseed ab Phase 1) bleibt.

### A8 — Sub 3 IST Halb-Tempo-Gehen, auf BEIDEN Eintritts-Pfaden **[eigen — hier widerspreche ich dem Pruefer [mechanismus]]**

    8004f9fc: lbu   v1,6(a0)
    8004fa04: beq   v1,zero,0x8004fa1c   ; Phase 0
    8004fa0c: beq   v1,v0,0x8004fa54     ; Phase 1
    8004fa14: j     0x8004fb2c
    ; --- Phase 0 (Seed) ---
    8004fa20: sb    v0,6(a0)             ; +0x6 = 1
    8004fa30: sb    v0,148(v1)           ; +0x94 = 5
    8004fa40: sb    zero,149(v0)         ; +0x95 = 0
    8004fa50: sb    v0,143(v1)           ; +0x8f = 7
    ; --- FAELLT DURCH in den gemeinsamen Rumpf ---
    8004fa54: ...
    8004fa60: lbu   v0,8(v1)             ; Typ
    8004fa74: addiu at,at,27648          ; Tabelle 0x80076c00
    8004fa7c: lbu   v0,0(at)             ; = 75
    8004fa84: srl   v0,v0,1              ; HALBES Tempo
    8004fa88: sh    v0,140(v1)           ; +0x8c = 37

Der `+0x8c`-Schreiber liegt im **gemeinsamen Rumpf**, nicht im Phase-0-Block.
DECIDE[1] setzt beim Uebergang `+0x5=3` @0x8004f3c8 **und `+0x6=1`** @0x8004f3d8
(bzw. @0x8004f40c/@0x8004f41c) — der Aktor betritt EXEC[3] also mit Phase 1,
behaelt Clip 5 aus Sub 1 (kein Reseed noetig) und laeuft **mit Tempo 37**.
Der Pruefer [mechanismus] hielt beide Schreiber fuer unerreichbar; nur `@0x8004fa30` ist es.

### A9 — DECIDE-Schwellen lesen `+0x1d0`, nicht die Spieler-Distanz **[eigen]**

    8004f3b4: lw    v0,464(v1)           ; +0x1d0 = Folgepunkt-Distanz
    8004f3bc: sltiu v0,v0,0x1f4          ; < 500  -> +0x5=3 @0x8004f3c8, +0x6=1 @0x8004f3d8
    8004f3e8: lh    a0,476(v0)           ; +0x1dc  (Folgepunkt, NICHT Spieler)
    8004f3ec: lh    a1,478(v0)           ; +0x1de
    8004f3f0: jal   0x8001ab9c           ; arc_test, a2 = 0x400
    8004f40c/41c:                        ; -> +0x5=3, +0x6=1
    8004f42c: lw    v0,464(v1)
    8004f434: sltiu v0,v0,0xbb9          ; >= 3001 -> +0x5=5 @0x8004f440, +0x6=0 @0x8004f450

`+0x1d0` wird in 0x8011ce54 gesetzt (`jal 0x8005070c` @0x8011ce80,
`sw v0,464(v1)` @0x8011ce94) **[eigen]**.
Sub 5 DECIDE: `sltiu v0,v0,0x3e8` @0x8004fd54 → `+0x5=3` @0x8004fd60,
`+0x6=0` @0x8004fd70 **[eigen]** — nur DIESER Pfad erreicht den Phase-0-Block von EXEC[3].

Achtung beim Nachbau: Sub 0 (@0x8004f148) und Sub 2 (@0x8004f78c) testen gegen
**playerX/playerZ**, Sub 1 (@0x8004f3f0) und Sub 3 (@0x8004f7f4) gegen den
**Folgepunkt +0x1dc/+0x1de**. Wer das vertauscht, baut ein anderes Verhalten.

### A10 — Port-Seite **[eigen]**

* `re15_npc_channel_anim` (enemy_ai_common.c:8660-8695) endet mit einem **UNGEGATETEN**
  `if (bank && bank->own_ok) return &bank->anim_own;` → in State 1 liefert die Funktion
  **bereits BANK 1**. Fix-Punkt 1 des Befunds beschreibt fuer diese Funktion den Ist-Zustand.
* Container-gebunden sind nur der **Renderer** (`platform/pc/main.c:6836-6900`, alle
  own-Zweige an `state == 4` / `walk_active`) und `re15_actor_clip_len`
  (enemy_ai_common.c:4926-4932).
* Daraus folgt ein vom Befund uebersehener Zustand: die **AI-Uhr** taktet in State 1
  bereits gegen BANK 1 (Laenge 30), waehrend der **Renderer** den Container-Clip
  (Laenge 20) posiert — Uhr und Pose divergieren schon jetzt.
* Globaler Advancer `player_common.c:941-953 / 975-976`: Typ 0x42 wird nur bei
  `state == 4` uebersprungen.
* `op_plc_ret` (scd_vm.c:2370-2376) routet ueber `t->work_slot`.

---

## B. BESTRITTEN

| Behauptung des Befunds | Nachpruefung |
|---|---|
| Fix 1: „`re15_npc_channel_anim` muss den own/BANK1-Kanal auch fuer state==1 waehlen" | **Falsch** — tut sie bereits (A10). Nur Renderer + `re15_actor_clip_len` sind betroffen. |
| „Der Port RENDERT aus der CONTAINER-Bank" (als Laufzeit-Messung) | Die Spalte `Bank=CONTAINER` stammt aus `render_bank_of()`, einem Hand-Nachbau IN der Sonde. Er laesst `re15_actor_uses_loco_bank()` aus und modelliert `clip_override = (anim_flags & 0x04) ? motion : -1` (anim_select_common.c:256) nicht — im natuerlichen Raumablauf setzt der SCD dieses Bit. Die Tabelle „was der Port stattdessen zeichnen wuerde" wurde also ueber einen Aufloesungspfad gerechnet, den der Renderer dort nicht nimmt. |
| „laesst den Spieler 60 Bilder im Gehtempo 75 weglaufen" | Pruefer [messung] hat gemessen, dass `pl->x/pl->z` von `re15_game_step` jedes Bild ueberschrieben werden: der Spieler steht in ALLEN 120 Bildern auf (0,−6752). Die gesamte Distanzkurve entsteht aus der NPC-Bewegung. |
| „Ziel-Abstand 5200..1400 → das Original haette in Sub 5 gewechselt" | Falsche Groesse (Spieler-Distanz statt `+0x1d0`, A9). Die Schlussfolgerung ueberlebt (Pruefer [messung] mass `+0x1d0` 6926 → 430, 66/92 Bilder ≥ 3001), die zitierte Zahl gehoert nicht zur Schwelle. |
| „posiert AUSSCHLIESSLICH aus BANK 1" | Der INIT posiert aus BANK 0 (A6). |
| „Ab Phase 1 ruft der Sub nur noch anim_set" | Falsch (A7). |
| „Sub 3 … Der Port setzt dort Clip 2 — falscher Clip UND falsche Semantik" | Der Port setzt Clip 2 nicht im Sub-3-EXEC, sondern im DECIDE-Zweig (`case 1: if (ai_dist < 0x1f4) { sub=3; re15_npc_clip(e,2); }`). Das Original schreibt in DECIDE[1] **ueberhaupt keinen Clip** (@0x8004f3a4-f3dc kein `+0x94`-Store). Der Write muss **ersatzlos gestrichen** werden, nicht auf 5 umgestellt. |
| CDEMD1-Eintrag „@0x800732f8 = LBA 1505/144032" | Adresse falsch — korrekt `0x80073308` (A1). |
| „Blob 19 mit exakt dieser Laenge [162524]" | Laengenwort im Blob = 162488; 162524 erst mit der 36-B-Verzeichnis-Fussleiste. |
| Messwert (4) „ALLE Subs erreicht → die KI laeuft korrekt" | Der Port-`case 1` ist eine andere Maschine (RE2-Nachruestung mit eigener Sub-Semantik). Der Messwert prueft den Port gegen sich selbst. |
| ⛔ Die **Grundgesamtheit** der Messung | In ROOM1090 spawnt der Aktor wegen grid_id 0x40 in **State 4 / Sub 6** (A3/A4), nicht in der Eskorte. Die Sonde setzt `state=1` per Hand UND schaltet den SCD ab. Keiner der vier gemeldeten Port-Fehler ist damit fuer den gemeldeten Fall nachgewiesen. |

---

## C. OFFEN

1. **Erreicht der PORT in ROOM1090 State 1?** Das Original tut es ueber sub03 @0x26C4
   (A5). Pruefer [messung] mass im Port 0 von 8 Subs / 400 Bilder. **Das ist die erste
   Frage** — wenn der Port nie ankommt, ist das Nutzer-Symptom „Ada folgt mir nicht" ein
   Work_set/Plc_ret-Routing-Defekt und **kein** Animations-Bank-Defekt.
2. **In welchem Raum der Nutzer-Report spielt** ist unbelegt.
3. **Sub 6** (EXEC 0x8004ffc0) schreibt `+0x94 = 6` @0x8005001c, BANK 1 von Typ 0x42 hat
   aber nur 6 Clips (0..5) → ausserhalb. Die 6-Phasen-Sprungtabelle @0x800112bc ist nicht
   zu Ende verfolgt. **Das ist jetzt wichtiger als vorher, weil ROOM1090 direkt in Sub 6 spawnt.**
4. **`DAT_800aca3c` Bit 31** (CDEMD0 vs. CDEMD1, `bltz v1,0x800223ac` @0x80022334) —
   kein Schreiber lokalisiert. Solange das offen ist, gelten ALLE Clip-Laengen
   (22/16/52/1/105/30) nur fuer den CDEMD0-Zweig.
5. **Kein Laufzeit-Beleg aus dem Original.** Kein DuckStation-Savestate mit einer
   laufenden Typ-0x42-Eskorte, an dem `+0x94/+0x95/+0x8f/+0x170/+0x174` live gegengelesen
   waeren. Die BANK-1-Zuordnung ruht auf Disasm + der Typ-0x40-Gegenprobe
   (`analysis/marvin_spawn_anim.md`).
6. **Pose-Amplituden** (Fuss-Bone 4 = 897, Container-Clip-2 dY ≈ 1768, „hoechstens 154")
   sind reine Sondenrechnungen, von mir nicht nachgerechnet. Die zugrundeliegenden
   Clip-Laengen sind dagegen aus den Bytes bestaetigt.
7. **`entity+0x8 == Typ`** ist angenommen, nicht belegt. Der Index in die Tempo-Tabellen
   (`lbu v0,8(v1)` / `addiu v0,v0,-64` @0x8004f55c, @0x8004fa60, @0x8004feb8) haengt daran.
8. **Zensus** „20 Rekorde in 15 Raeumen" nicht nachgezaehlt (mein Byte-Scan liefert
   Falschtreffer). Bestaetigt sind nur ROOM1090 @0x22CC (grid 0x40) und
   ROOM3070 @0x343c (grid 0x00).
9. **PSX-Renderpfad** (`platform/psx`) von keiner Seite geprueft.
10. **Kollision/Geometrie:** weder die Befunds-Sonde noch die Gegensonde prueft, ob die
    NPC-Bahn auf begehbarem Boden liegt; 91 Bilder ohne jede Kollisionspruefung.

---

## D. NAECHSTER SCHRITT

### D1 — Zuerst messen, VOR jedem Fix

**Erreicht der Port in ROOM1090 State 1 fuer den 0x42-Slot?**
Weg: Raum mit vollem SCD ticken, `op_work_set`/`op_plc_ret` tracen, `t->work_slot` an
Datei-0x26C4 in sub03 pruefen, und feststellen, was sub03 ueberhaupt startet.
Das entscheidet, ob der Nutzer-Report ein Routing- oder ein Animations-Defekt ist.
Zweitens: **Sub 6** aufloesen (Phasen-Tabelle @0x800112bc) — dort spawnt sie in ROOM1090.

### D2 — Implementierungsreif (unabhaengig von D1), jede Konstante mit Adresse

1. **Bank fuer state == 1**: Renderer `platform/pc/main.c:6836-6900` und
   `re15_actor_clip_len` (enemy_ai_common.c:4926-4932) muessen den own/BANK-1-Kanal
   auch fuer `state == 1` waehlen.
   Beleg: alle fuenf Eskorte-EXECs laden `+0x170`/`+0x174` (@0x8004f384/88, @0x8004f5c0/c4,
   @0x8004f7bc/c0, @0x8004fb14/18, @0x8004ff68/6c); `FUN_80022300` legt
   `dir[3] -> +0x174` @0x800224b8, `dir[4] -> +0x170` @0x800224c8.
   **NICHT** `re15_npc_channel_anim` — die ist bereits korrekt (A10).
2. **Phasen-Tor `+0x6`** (dreiwertig) nachbilden: @0x8004f4f0/f4f8/f500/f508 (EXEC 1),
   @0x8004f320ff (EXEC 0), @0x8004f9fc/fa04/fa0c/fa14 (EXEC 3).
   Nur Phase 0 schreibt `+0x94`, `+0x95 = 0`, `+0x8f = 7` und setzt `+0x6 = 1`;
   Phase ≥ 2 tut gar nichts.
3. **Ein einziger Advancer**: `anim_set` ist der einzige Stepper (`+0x95` Post-Inkrement
   @0x8001f618, Wrap an der Clip-Laenge aus `+0x174`; `+0x8f--` @0x8001f5b4).
   Typ 0x42 muss im globalen Advancer (`player_common.c:941-953` bzw. der NPC-Skip
   :975-976) auch in State 1 uebersprungen werden.
4. **DECIDE[1]**: den Port-`re15_npc_clip(e,2)` auf der Kante Sub1→Sub3 **ersatzlos
   streichen** — das Original schreibt dort keinen Clip (@0x8004f3a4-f3dc) — und
   `+0x6 = 1` setzen (@0x8004f3d8), nicht 0.
5. **Sub 3 = Halb-Tempo-Gehen** auf BEIDEN Pfaden: `+0x8c = 0x80076c00[(typ-0x40)*2] >> 1`
   = 75>>1 = 37 (@0x8004fa7c/@0x8004fa84/@0x8004fa88); Clip bleibt 5
   (Phase-0-Reseed nur @0x8004fa30).
6. **Sub 5 (Laufen)**: `+0x94 = 0` @0x8004fe88, `+0x95 = 0` @0x8004fe98, `+0x8f = 7`
   @0x8004fea8, `+0x8c = 0x80076c80[(typ-0x40)*2] = 200` @0x8004fedc (Dreh-Rate 72);
   Eintritt aus DECIDE[1] bei `+0x1d0 >= 3001` (@0x8004f434) mit `+0x6 = 0` (@0x8004f450);
   Austritt bei `< 1000` (@0x8004fd54) nach Sub 3 mit `+0x6 = 0` (@0x8004fd70).
7. **Sub 2 (Drehen)**: `+0x94 = 5` @0x8004f734, `Yaw += arc_test(Spieler, 0x80076c41[4]=96)`
   @0x8004f780-a8, **kein** `pos_advance` (@0x8004f7c4 nur `anim_set`).
8. **`+0x9 == 1`-Zweig** (0x8004f100) endet ohne `anim_set` (`jr ra` @0x8004f1fc) —
   dort darf der Port nicht advancen.
9. **Neu und im Befund nicht enthalten — muss jede Eskorten-Arbeit respektieren:**
   grid_id-Bit **0x40** @0x8011ce24 → `+0x4 = 4` @0x8011ce30 / `+0x5 = 6` @0x8011ce40.
   ROOM1090 traegt 0x40 (@Datei 0x22CF), ROOM3070 traegt 0x00 (@Datei 0x343F).

### D3 — Gestrichen (unbelegt oder falsch)

* **Fix-Punkt 1, Teil `re15_npc_channel_anim`** — beschreibt den Ist-Zustand (A10).
* **Die Typen-Liste `0x45/0x47/0x49/0x4b/0x4d`** in Fix-Punkt 3. Nur fuer 0x47 gibt es
  einen Beleg (Root 0x8011dc68, Tabelle @0x80121808 mit derselben Form, vom Pruefer
  [belege] nachgeliefert). Fuer 0x45/0x49/0x4b/0x4d **kein Beleg** — ein pauschaler Skip
  im globalen Advancer waere ein Rate-Risiko.
* **`@0x800732f8` fuer den CDEMD1-Eintrag** — falsche Adresse (korrekt `0x80073308`);
  und der ganze CDEMD1-Zweig ist ohnehin unbelegt erreichbar (OFFEN 4).
* **„Sub 3 setzt Clip 2 im Port — falscher Clip"** als EXEC-Aussage — es ist ein
  DECIDE-Write, der geloescht gehoert (B-Tabelle).
* **Die Perzeptions-Aussagen** („20-Bilder-Animation ist kein Gangzyklus", „50-Bilder-
  Sturz", „feiner Atem-Idle") — Interpretationen von Amplituden, kein Disasm-Beleg;
  keine Original-Funktion benennt diese Clips.
