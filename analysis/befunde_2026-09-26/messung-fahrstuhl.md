# Messung: Fahrstuhl 1F -> 2F/3F bleibt 1F — URSACHE GEFUNDEN UND REPRODUZIERT

Stand 2026-09-26, Arbeitsbaum `.claude/worktrees/wf_19e49a69-d81-2`.
Ergebnis dieses Laufs ist eine MESSUNG, kein Fix. Die Engine-Aenderungen in diesem Baum
sind ausschliesslich Messhaken (env-gegated, `RE15_ELEV_LOG`) — ausdruecklich
**Messaenderung, nicht der Fix**.

---

## 0. Kurzurteil in einem Satz

`Aot_on(1)` legt den richtigen Zielraum 0x10C0 an, und **im SELBEN Bild ueberschreibt ihn der
AOT-Scan mit Tuer-Slot 0 = 0x1040 (1F)** — ueber den Port-eigenen Sonderzweig
`is_auto_door` (`re15_port/engine/src/aot_common.c:943-956` + `:1260/1268`), der jede
Null-Rect-Tuer feuert, sobald Cinematic laeuft und alle SCD-Threads leer sind. Weil
`g_room_change` ein einziger Datensatz ist (`room_common.c:186-194`, letzter Schreiber
gewinnt), laedt `re15_room_apply_pending` 1F — egal welche Etage gedrueckt wurde.

---

## 1. Die Messung (Echtlauf, echter Renderpfad, 3 von 3 Faellen)

Aufbau: `re15_pc.exe` (SDL-GUI, kein AUTOSHOT, kein Software-Renderer), Debug-Menue-Sprung
`RE15_DEBUG_JUMP=1080@100`, Spieler per `RE15_PLAYER_POS=-12050,-670,3072` direkt vor das
Panel (Rect-Zentrum (-12050,-50), FORWARD-620 bei Yaw 3072 -> Testpunkt genau auf dem
Zentrum), Eingaben ueber `RE15_PRESS`, Messchiene ueber die NEUE Datei-Schiene
`RE15_ELEV_LOG` (die GUI-exe hat **kein** stderr — gemessen: `press2fb.err` = **0 Byte**).
Treiber: `analysis/befunde_2026-09-26/run_elev.sh`.

**Abdeckung: 3 Fahrten (1F/2F/3F), je ein vollstaendiger Lauf ueber ~3000 Protokollzeilen
(press1f 3041, press2fb 3038, press3f 3058), je ~600 Bilder bis zum Raumwechsel und danach
weiter bis Sekunde 120.** Keine Fehllaeufe: in allen drei Protokollen steht die volle Kette.

### 1F (Knopf 4, 4x DOWN) — `mess/press1f.log`
```
451: EVT_EXEC sub=7 cond=0xff -> slot 10  raum=0x1080
614: AOT_ON slot=0 typ=1
615: FIRE_DOOR slot=0 dest=st0 rm0x04 cut=5
616: REQ  room=0x1040 spawn=(-21936,0,-11000) yaw=1024 cut=5  (von=0x1080)
617: SCAN-FIRE-DOOR slot=0 action=0 inside=0        <-- Scan feuert Slot 0 ERNEUT
618: FIRE_DOOR slot=0 dest=st0 rm0x04 cut=5
619: REQ  room=0x1040 ...
622: APPLY-ENDE  raum=0x1040 cut=5 spieler=(-21936,0,-11000)
```

### 2F (Knopf 5, 4x UP) — `mess/press2fb.log`
```
451: EVT_EXEC sub=8 cond=0xff -> slot 10  raum=0x1080
614: AOT_ON slot=1 typ=1
615: FIRE_DOOR slot=1 dest=st0 rm0x0C cut=0
616: REQ  room=0x10C0 spawn=(1450,0,7300) yaw=3072 cut=0  (von=0x1080)   <-- RICHTIG
617: SCAN-FIRE-DOOR slot=0 action=0 inside=0                             <-- UEBERSCHREIBT
618: FIRE_DOOR slot=0 dest=st0 rm0x04 cut=5
619: REQ  room=0x1040 spawn=(-21936,0,-11000) yaw=1024 cut=5  (von=0x1080)
620: APPLY-START ziel=0x1040 aktuell=0x1080
621: APPLY-LOAD ok -> g_current_room_id=0x1040
622: APPLY-ENDE  raum=0x1040 cut=5 spieler=(-21936,0,-11000)
```

### 3F (Knopf 6, 17x UP) — `mess/press3f.log`
```
464: EVT_EXEC sub=9 cond=0xff -> slot 10  raum=0x1080
627: AOT_ON slot=2 typ=1
628: FIRE_DOOR slot=2 dest=st0 rm0x12 cut=0
629: REQ  room=0x1120 spawn=(1300,0,7300) yaw=3072 cut=0  (von=0x1080)   <-- RICHTIG
630: SCAN-FIRE-DOOR slot=0 action=0 inside=0                             <-- UEBERSCHREIBT
631: FIRE_DOOR slot=0 dest=st0 rm0x04 cut=5
632: REQ  room=0x1040 ...
635: APPLY-ENDE  raum=0x1040 cut=5 spieler=(-21936,0,-11000)
```

Das ist wortwoertlich der Nutzer-Befund: **2F und 3F landen beide in ROOM1040 = 1F.**

Zustand im Feuer-Bild (Protokollzeile F502 unmittelbar davor):
```
F502 raum=1080 cut=0 pmode=2 pause=01000007 pad=0000 f5=000 f3_54=0 f3_55=0 f3_56=0
     thr=00000400 cursor=0(-19554,23484)notch=5 spieler=(-13650,0,-675) pend=0->1080
```
`notch=5` = der Cursor stand korrekt ueber Zelle 5 (2F). Die Zellenwahl, der Stempel +0x0B,
`Member_cmp(15==5)`, `Sce_key_ck(1,0x0040)`, `Evt_exec sub=8` und `Aot_on 1` sind ALLE
richtig. Der Defekt sitzt eine Schicht spaeter.

---

## 2. Der Mechanismus, Zeile fuer Zeile

`SCAN-FIRE-DOOR slot=0 action=0 inside=0` sagt es genau: der Scan feuert Tuer-Slot 0,
obwohl **kein Action-Druck** anliegt (`g_aot_action_pressed == 0`) und der Spieler
**nicht im Rechteck** steht (`door_inside == 0`). Die regulaere Tuer-Bedingung
(`aot_common.c:1274`, `door_inside && g_aot_action_pressed && !msg_block && !action_fired`)
wird also gar nicht erreicht — es greift der **erste** Zweig des Ternaer-Baums:

`re15_port/engine/src/aot_common.c:1260`
```c
int fire = is_auto_door
               ? (in_cinematic && scd_idle && scd_ran && !msg_block && !action_fired)   /* :1268 */
         : (a->type == RE15_AOT_TYPE_DOOR)
               ? (door_inside && g_aot_action_pressed && ...)                            /* :1274 */
```

`is_auto_door` entsteht in `aot_common.c:943-956`:
```c
int is_auto_door = 0;
if (a->type == RE15_AOT_TYPE_DOOR && a->half_w == 0 && a->half_h == 0) {
    unsigned dd = (((unsigned)dest_stage + 1u) << 12) | (dest_room << 4) | (g_current_room_id & 0xF);
    is_auto_door = (dd != g_current_room_id);
}
```
Alle drei Fahrstuhl-Tueren von ROOM1080 sind **Null-Rect** (`half_w == half_h == 0`,
`ROOM1080.RDT @Datei 0x0482/0x04A2/0x04C2`: `3b 00 02 31 00 00 00 00 00 00 00 00 00 00 …`)
und zeigen auf einen anderen Raum. Damit ist **jede der drei** eine „Auto-Advance-Tuer".

Die drei Bedingungen des Zweigs stehen in genau diesem Moment alle:
* `in_cinematic` (`aot_common.c:882`, `player_mode == 2 || letterbox_countdown != 0`) —
  gemessen `pmode=2` in jedem Bild der Fahrt; sub08 setzt dazu `Set(1,27,1)`
  (`ROOM1080.RDT @0x07C2`, Bytes `22 01 1b 01`) und `Set(2,7,1)` (`@0x07BE`, `22 02 07 01`).
* `scd_idle` (`aot_common.c:897-899`) — wird **exakt in dem Bild** wahr, in dem sub08 nach
  `47 01` (`@0x080E`) sein `01 00` Evt_end (`@0x0810`) erreicht: der letzte Thread stirbt.
* `scd_ran` (`aot_common.c:900`, `tick_count > 90`) — nach 600 Bildern laengst wahr.

Und weil die Schleife bei `i = 0` beginnt (`aot_common.c:913`), gewinnt immer der
**niedrigste** Null-Rect-Slot — in ROOM1080 ist das Slot 0 = ROOM1040 = 1F.

### Die Stelle, an der die Etagenwahl verloren geht (Auftrag a)

`re15_port/engine/src/room_common.c:186-194` — `g_room_change` ist EIN Datensatz ohne
Warteschlange, `re15_room_request_change` ueberschreibt ihn bedingungslos. Der erste
Schreiber im Bild ist `aot_common.c:598` aus `Aot_on` (richtig, 0x10C0), der zweite
ist **derselbe** `aot_common.c:598`, diesmal aus dem Scan-Zweig `aot_common.c:1268`
(falsch, 0x1040). `re15_room_apply_pending` (`room_common.c:196`) sieht nur noch den
zweiten. Der Pruefer hat den Ort richtig benannt; der TAETER steht aber davor, in
`aot_common.c:943-956/1268`.

---

## 3. Warum die alte Sonde das nicht sehen konnte

`probe_elevator_1080.c:73-86` baut den Frame aus acht Aufrufen selbst und setzt
`g_scd.player_mode` nie auf 2 (das leitet im Spiel `platform/pc/main.c` jedes Bild aus
flag(2,7) ab). Damit ist `in_cinematic` in der Sonde **false**, der Zweig
`aot_common.c:1268` kann nicht feuern, und die Sonde misst eine Lage, die es im Spiel
nicht gibt. Zusaetzlich bricht sie bei `probe_elevator_1080.c:174` beim ersten
`g_room_change.pending` ab — das ist der Moment ZWISCHEN dem richtigen und dem falschen
Schreiber.
Der frisch gegen HEAD gebaute Lauf der unveraenderten Sonde meldet weiterhin
`===== BEFUND: OK =====` (1F/2F/3F alle „OK") — **die Staleness-Frage des Pruefers ist
damit beantwortet: nicht das Alter war das Problem, sondern der fehlende Zustand.**

---
## 4. Die neue Sonde: `probe_elevator_1080_full` (Auftrag b)

`re15_port/tests/unit/probe_elevator_1080_full.c` (+ `re15_port/tests/unit/probes/fahrstuhl.cmake`).
Sie faehrt denselben UI-Pfad wie die alte Sonde, aber:
* zieht den player_mode-Proxy des Spiels nach (`platform/pc/main.c:4566-4578`,
  `cine_active = flag(1,27) || flag(2,7)` -> `player_mode = 2`) — das ist genau der
  Zustand, ohne den der Defekt unerreichbar bleibt;
* ruft **`re15_room_apply_pending`** mit einem Datei-RDT-Lader in jedem Bild;
* pinnt den **geladenen Raum**, nicht die Warteschlange.

Lauf gegen HEAD:
```
== Knopf 1F (Zelle 4, 4x DOWN) ==  notch=4  -> GELADENER Raum nach 161 Bildern: 0x1040 OK
== Knopf 2F (Zelle 5, 4x UP)   ==  notch=5  -> GELADENER Raum nach 161 Bildern: 0x1040 (Soll 0x10C0) FALSCH
== Knopf 3F (Zelle 6, 17x UP)  ==  notch=6  -> GELADENER Raum nach 161 Bildern: 0x1040 (Soll 0x1120) FALSCH
ABDECKUNG: 550 Bilder getickt, 6 Raumwechsel angewandt, 438 Bilder im Cinematic.
===== BEFUND: FAIL =====
```
Deterministisch, ~1 Sekunde, und deckungsgleich mit dem Echtlauf. Bewusst **ohne
`add_test`**, solange der Defekt offen ist (sonst faerbt sie die Suite rot); die
`add_test`-Zeile gehoert zum Fix.

---

## 5. Wie weit die Fehlerklasse reicht

Roh-Zensus ueber alle ausgelieferten RDTs (Muster: `Door_aot_set` mit `sce==2`, Rechteck
`w==0 && h==0`, Ziel != aktueller Raum; Feldlagen nach `scd_vm.c:3780-3795`):
**27 Null-Rect-Cross-Room-Tueren in 19 Raeumen.** Genau VIER Raeume haben mehr als eine —
und nur dort kann „der niedrigste Slot gewinnt" eine falsche Etage liefern:

| Raum | Slots |
|---|---|
| ROOM1080 / ROOM1081 | slot0->0x1040 (1F), slot1->0x10C0 (2F), slot2->0x1120 (3F) |
| ROOM4020 / ROOM4021 | slot0->0x4000, slot1->0x5000, slot2->0x50C0 |

**Der ZWEITE Fahrstuhl des Spiels (ROOM4020/4021) hat exakt denselben Defekt** — dort ist er
nur noch nicht gemeldet worden. Die uebrigen 15 Raeume mit genau EINER Null-Rect-Tuer bekommen
keine falsche Etage, wohl aber einen Tuerauslöser ohne Tastendruck, sobald Cinematic laeuft und
die Threads leer sind (nicht Teil dieses Auftrags, nicht gemessen).

---

## 6. Auftrag (d): der Letterbox-Streit — der PRUEFER hat recht, der Dossier-Beifund faellt

Selbst disassembliert (`re15_disasm.py dis 0x800396fc 24`, `info/Re1.5/PSX.EXE`):
```
800396fc: addiu sp,sp,-48
8003970c: lw    v0,-13764(v0)     ; v0 = [0x800aca3c] = Flag-Bank 1
80039710: lui   v1,0xffff         ; v1 = 0xFFFF0000
80039728: and   v0,v0,v1
80039730: sw    v0,-13764(at)     ; [0x800aca3c] = v0
```
Bank-Basis selbst nachgelesen (`bytes 0x80074664 32`):
`38 ca 0a 80 | 3c ca 0a 80 | 40 ca 0a 80 | f8 0f 0b 80 | …`
=> [0]=0x800aca38, **[1]=0x800aca3c**, [2]=0x800aca40, [3]=0x800b0ff8, [5]=0x800b1028.

Bit-Reihenfolge selbst nachgelesen (Flag-Schreiber, `dis 0x8003fdc4` / `dis 0x8003fe4c`):
```
8003fdd4: lui  t0,0x8000        ; t0 = 0x80000000
8003fdf0: sra  v0,a1,5          ; Wortindex = idx>>5
8003fe18: andi a1,a1,0x1f       ; Bit im Wort
8003fe68: srlv v1,t0,a1         ; Maske = 0x80000000 >> idx   (MSB-FIRST)
8003fe6c: or   v1,v1,a0
8003fe74: sw   v1,0(a3)
```
=> idx 27 hat den Wert `0x80000000 >> 27 = 0x10`. `0xFFFF0000 & 0x10 == 0`, also **loescht
die Raum-Ladekette `FUN_800396fc` das Letterbox-Bit 27 bei JEDEM Raumladen**.

Die Dossier-Behauptung „loescht es nie" ist an der falschen Funktion gemessen
(`FUN_8003ecec` ist der Flag-/Thread-Reset, nicht der Raumlader). Der Port macht es bereits
byte-true: `re15_port/engine/src/scd_room_setup.c:241`
`for (int fi = 16; fi < 32; fi++) re15_game_flag_set(1, (uint8_t)fi, 0);` mit demselben
Zitat @0x80039710-30. **Hier ist kein zweiter Defekt.**

---

## 7. Umsetzungsplan (Auftrag e) — mit datei.c:zeile und @0x

**Kernaussage vorweg:** `is_auto_door` ist eine PORT-ERFINDUNG. Im Original kann der einzige
druckfreie, jedes Bild laufende Scan-Pass eine sce-2-Tuer gar nicht auswaehlen — das steht in
vier Instruktionen, selbst disassembliert:

```
; der AUTO-Pass FUN_800436a8 ruft den Scanner mit Pool=OBJEKT und ACTION=0
80043778: ori  a1,zero,0x4          ; Pool-Maske = 4 = OBJEKT-Pool
8004378c: jal  0x80042bac
80043790: addu a2,zero,zero         ; (Delay-Slot) a2 = 0 = AUTO, nicht ACTION

; und der Scanner filtert genau darauf, BEVOR er den sce-Typ ueberhaupt ansieht
80042c84: lbu  v0,1(s0)             ; rec[1] = sce_flags
80042c8c: and  v0,v0,a3             ; & Pool-Maske
80042c90: beq  v0,zero,0x80043018   ; kein Pool-Treffer -> Satz uebersprungen
80042ca0: andi v0,v0,0x10           ; ACTION-Bit
80042ca4: bne  v0,s6,0x8004301c     ; != a2 -> Satz uebersprungen
80042cac: lbu  v0,2(s0)             ; erst HIER wird der sce-Typ gelesen
```
Die drei Fahrstuhl-Tueren tragen `rec[1] = pc[3] = 0x31` (ROOM1080.RDT @0x0485/0x04A5/0x04C5).
`0x31 & 0x04 = 0` -> erstes Gate schlaegt zu; `0x31 & 0x10 = 0x10 != 0` -> zweites Gate
schlaegt zu. **Zwei unabhaengige Riegel, beide dicht.** Der Port-Zweig umgeht beide.
Er existiert allein fuer die Intro-Uebergabe ROOM1240 -> ROOM1170.

**Und die braucht ihn nicht:** in `ROOM1240.RDT` steht der Handoff als `Aot_on` im Skript —
selbst gelesen, `47 00 01 00` (Aot_on 0 direkt vor Evt_end) @Datei **0x0618** und **0x0620**
(sub02/sub03). Denselben Befund nennt bereits der Kommentar an `scd_vm.c:4139`.

Schritte, in dieser Reihenfolge:

1. **`re15_port/engine/src/aot_common.c:943-956` + `:1260/1268` — den Zweig `is_auto_door`
   entfernen**, sodass Tueren wieder ausschliesslich ueber
   `door_inside && g_aot_action_pressed` (`aot_common.c:1274`, byte-true @0x80042f48/50)
   oder ueber `Aot_on` (`scd_vm.c:4144-4153`, byte-true LAB_800407bc, `jalr` @0x8004082c)
   feuern. Der Cinematic-Riegel `aot_common.c:958` faellt damit wieder auf ALLE Nicht-RVD-AOTs
   zurueck — genau das, was der Zweig aushebelt.
2. **Verifikation A (der Grund, warum der Zweig da ist):** Durchlauf ab Spielstart bis
   ROOM1240 -> ROOM1170. Muss weiterhin uebergeben, jetzt ueber `Aot_on 0`
   (`ROOM1240.RDT @0x0618/0x0620`). Messhaken: dieselbe `RE15_ELEV_LOG`-Schiene, Zeile
   `AOT_ON slot=0` muss kommen und `SCAN-FIRE-DOOR` darf NICHT kommen.
3. **Verifikation B:** `probe_elevator_1080_full` muss OK melden (3/3 Etagen) —
   dann `add_test(NAME probe_elevator_1080_full …)` in
   `re15_port/tests/unit/probes/fahrstuhl.cmake` nachziehen.
4. **Verifikation C (zweiter Fahrstuhl):** dieselbe Sonde auf ROOM4020 spiegeln
   (slot0->0x4000, slot1->0x5000, slot2->0x50C0).
5. **Messhaken zurueckbauen:** die vier `re15_elev_log`-Einsprungstellen
   (`room_common.c` Helfer + 4 Aufrufe, `scd_vm.c:4147`, `aot_common.c` in `aot_fire_door`
   und im Scan-Zweig, `platform/pc/main.c` Frame-Zeile) sind reine MESSAENDERUNG und
   gehoeren nach der Verifikation wieder raus.

**Falls Schritt 2 scheitert** (die Intro-Uebergabe bleibt ohne den Zweig haengen), ist die
naechste Stufe NICHT, den Zweig zurueckzuholen, sondern zu messen, warum `Aot_on 0` in
ROOM1240 nicht durchkommt — das Skript hat es, und ein Skript-Aot_on ist byte-true, ein
geometrischer Auto-Trigger nicht.

---

## 8. Projekt-Einordnung (40%-Beta-Regel)

**RE1.5 hat dieses System VOLLSTAENDIG** — Panel-Cursor (sub01-sub06), drei Fahrt-Subs
(sub07/08/09), Tuer-Sub (sub10), Etagen-Flags Bank 3 Bit 54/55/56, drei Zieltueren, eigene
Panel-Kamera (Cut 1-4) und eigene RVD-Zone. Es ist kein Stub und kein Platzhalter.
=> **RE1.5 bleibt hier massgeblich, RE2-Retail ist NICHT das Ziel.** Der Defekt ist rein
port-seitig: eine Port-Erfindung (`is_auto_door`) ueberschreibt eine byte-true Skript-Wahl.

---

## 9. Was dieser Lauf NICHT geklaert hat

* Ob die Intro-Uebergabe ROOM1240 -> ROOM1170 ohne `is_auto_door` durchlaeuft. Das ist
  Verifikation A oben; dieser Lauf hat sie nicht gefahren (der Messlauf sprang per
  Debug-Menue direkt nach ROOM1080, wie beauftragt).
* Die 15 Raeume mit genau EINER Null-Rect-Tuer: sie bekommen keine falsche Etage, aber
  ebenfalls einen Auslöser ohne Tastendruck. Ungemessen.
* ROOM4020/4021 ist per Zensus betroffen, aber nicht laufend gemessen.
