# Runde 18 — „ada-eintritt-1050": Adas doppelte Animation beim Betreten von ROOM1050

Nutzer-Befund (wörtlich, 2026-09-20/21):

> „dann im Raum der cutscene wo man ada aus dem feuer rettet, und dann mit ihr den Raum
> wechselt, spielt ihre Animation am Anfang doppelt ab."

Auf Rückfrage präzisiert:

> „zu ada - nach Der feuer cutscene der raum danach direkt am Anfang. Ich glaube 1050."

---

## 0. Kurzfassung

**Der Befund ist ein echter Port-Defekt und er ist behoben.**

Der ECHTE Übergang (ROOM1090-Feuer-Cutscene → Tür-Slot 0 → ROOM1050) ist gefahren und
bildweise protokolliert worden — Zustandsspur, Opcode-Spur und **Pose-Strom**
(`RE15_ANIM_TRACE`, die einzige Quelle, die sagt, welcher Clip in welchem Bild wirklich
posiert wird). Ergebnis:

| Bild | Clip | Bild-im-Clip | Quelle |
|---|---|---|---|
| 1–5 | **0** | 0 | Tür-Blende: Pausemaske `0xFF000000` friert KI + Skript ein, der Renderer läuft weiter |
| 6–8 | 5 | 1..3 | `Plc_dest(mode 9)` → Sub-9-Turn (`+0x94 = 5` @0x80051d3c) |
| 9–24 | 1 | 1..15,0 | Sub 9 ausgerichtet → Sub 6 (@0x80051dac/@0x80051dbc) → Phase 0 setzt Clip 1 @0x80051854 |
| 25–34 | 2 | 1..10 | Sub-6-Phase 2 → Clip 2 = Ruhe-Schleife @0x800518c8 |
| **35+** | **0** | 1..21 | sub04 @0x0E04 `Plc_dest(mode 5)` → sie läuft WIRKLICH weg |

**Bild 1–5 und Bild 35+ sind DERSELBE Clip 0 — der 22-Bild-Laufzyklus von EM42.** Der Raum
blendet auf, während Ada bereits mitten im Weglauf-Schritt steht (Bein in der Luft), sie
schnappt in den Stand zurück und spielt die Weglauf-Animation ~30 Bilder später noch
einmal. Genau die gemeldete Doppelung — und genau „direkt am Anfang".

**Ursache:** der Spawn-Saat-Wert für `entity+0x94`. `re15_enemy_spawn_action` (scd_vm.c)
lieferte für Typ 0x42 den ausdrücklich als ungeprüft markierten Platzhalter **0**
(„other types: their overlay decoders not yet RE'd → spawn at idle action 0"). Für Ada ist
Clip 0 aber kein Idle, sondern der Laufzyklus.

**Fix:** der Saat-Wert trägt jetzt den Literal aus dem State-0-INIT der TYP-EIGENEN
Overlay-Wurzel — für Typ 0x42 die Wurzel 0x8011cb70, INIT @0x8011ccac, dort
verzweigungsfrei `ori v0,zero,0x2` @0x8011cd8c + `sb v0,148(v1)` @0x8011cd90, also
**+0x94 = 2**. Dieselbe 2 tragen die INITs aller sechs STAGE1-NPC-Wurzeln.

**Riegel:** `unit_ada_eintritt_1050` (tests/unit, eigene Probe-CMake). Gegenprobe gefahren:
mit dem alten Platzhalter fällt er rot (`+0x94 = 0`, Exit 1).

---

## 1. Reproduktion — der ECHTE Übergang, nicht der Raum-Sprung

Die Vorrunde (`analysis/befunde_2026-09-20/ada-und-weste.md` §1.3/§5) hatte ROOM1050
**einzeln per Raum-Sprung** gefahren und deshalb keinen Defekt festmachen können. Hier ist
zuerst der echte Weg gefahren worden.

### 1.1 Der Lauf

Echte exe aus diesem Arbeitsbaum (`re15_port/build_ada/platform/pc/re15_pc.exe`), echter
Renderpfad (kein `AUTOSHOT`, kein Softwarerenderer):

    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp
    RE15_DEBUG_JUMP=1090@gp RE15_SET_FLAG=3:0x84
    RE15_AUTOPILOT=aot:0 RE15_AP_CUTSCENE_CLICK=1
    RE15_FIRE_AOT="0@760#1090"
    RE15_STATE_LOG=... RE15_SCD_TRACE=1 / RE15_ANIM_TRACE=...

Der Sprung geht nach ROOM1090 (Feuer-Raum), `Ck(3,0x84)==1` startet dort sub00's
`Sce_em_set` Typ 0x42 @0x22CC + `Evt_exec sub03` = die Rettungs-Cutscene. Der Regler
klickt die Dialoge durch; die Cutscene endet mit `Plc_ret` @0x26C4/@0x26D8, danach ist Ada
**Begleiterin** (gemessen ROOM1090 F698: `[1 t=42 st=1 ...]` = NPC-State 1 = Eskorte,
F700 pm=1, F714 pm=0 = Spieler frei). Bei F760 wird Tür-Slot 0 gefeuert — derselbe Pfad,
den das Hineinlaufen nimmt (`re15_aot_fire_slot` → `aot_fire_door`, aot_common.c:676):

    [fire-aot] slot=0 at F760 (Raum 1090)
    [aot] DOOR FIRE slot=0 rect=(-10080,3060,hw=2350,hh=800) target_cut=6 spawn=(19850,0,-22300)
    [room] PC loaded room1050.rdt (297324 bytes)

Der Raumwechsel selbst läuft also echt durch. Das Tür-Ziel (19850,0,-22300) yaw 3072 aus
`Door_aot_set` @0x211A ist exakt das, was ROOM1050s sub03 @0x0D9A-0x0DA2 per `Member_set`
noch einmal auf den Spieler schreibt — die Zuordnung „Tür 1090/0 → ROOM1050" ist damit
doppelt belegt.

### 1.2 Die Opcode-Spur des Eintritts (RE15_SCD_TRACE)

    [scd] F760 slot=1 @0x0C86 op=0x06      Ifel_ck
    [scd] F760 slot=1 @0x0C8A op=0x21      Ck(3,110,1)      <- das Tor aus ROOM1090 sub03 @0x24D6
    [scd] F760 slot=1 @0x0C8E op=0x44      Sce_em_set  Typ 0x42, grid 0x40, (18550,0,-22300) yaw 3072
    [scd] F760 slot=1 @0x0CA2 op=0x04      Evt_exec sub03
    [scd] thread start slot=10
    [scd] F760 slot=10 @0x0D94 op=0x29     Cut_chg(6)
    [scd] F760 slot=10 @0x0D96 op=0x2E     Work_set 1,0   (Spieler)
    [scd] F760 slot=10 @0x0D9A/9E/A2 0x34  Member_set 0/2/4 = 19850 / -22300 / 3072
    [scd] F760 slot=10 @0x0DA6 op=0x2E     Work_set 2,0   (Ada = Aktor-Slot 1)
    [scd] F760 slot=10 @0x0DAA op=0x40     Plc_dest(slot=1 mode=0x09 dest=(16250,-16650))

`sub00` und `sub03` laufen im SELBEN VM-Tick. Der Spawn wird also noch im Ladeframe auf
State 4 / Sub 9 umgesetzt — **der State-0-INIT des Aktors läuft nie** (dazu §2.3). Danach
steht der Zähler auf 0 und die Tür-Blende hält fünf Bilder (`pf=FF000007` in F1..F5,
ab F6 `pf=00000007`): keine einzige `[scd]`-Zeile, kein KI-Tick.

### 1.3 Adas Zustandsspur in ROOM1050 (RE15_STATE_LOG)

| Bild | st | ss1 | ss2 | mo | Position |
|---|---|---|---|---|---|
| 1   | 4 | 9 | 0 | 0 | (18550,-22300,r3072) |
| 6   | 4 | 9 | 1 | 5 | r2976 |
| 8   | 4 | 6 | 0 | 5 | r2821 |
| 9   | 4 | 6 | 1 | 1 | |
| 24  | 4 | 6 | 2 | 1 | |
| 25  | 4 | 6 | 3 | 2 | |
| 35  | 4 | 5 | 2 | 0 | |
| 64  | 4 | 6 | 0 | 0 | (16361,-16920) — Ankunft Ziel A |
| 65  | 4 | 5 | 2 | 0 | zweites `Plc_dest` (sub04 @0x0E18) |
| 78  | 4 | 6 | 0 | 0 | (16251,-14322) — Ankunft Ziel B |
| 79  | 4 | 6 | 1 | 1 | (850,-14200) — `Member_set` @0x0E2C/0x0E30 parkt sie aus dem Bild |
| 95  | 4 | 6 | 3 | 2 | bleibt bis Laufende |

### 1.4 Die Mess-Abkürzung ist VALIDIERT, nicht angenommen

Für die langen Bildreihen (jede `RE15_FRAMEDUMP`-Rückleseoperation kostet Laufzeit; der
volle Weg ab EXE-Start braucht ~2500 Bilder) ist ein schnellerer Einstieg benutzt worden:
`RE15_DEBUG_JUMP=1050@100 RE15_SET_FLAG=3:0x6e`. Dass er dasselbe zeigt, ist **gemessen,
nicht angenommen** — Adas Moduswechsel sind bild-identisch zum echten Übergang:

    echter Uebergang : F1 F6 F8 F9 F24 F25 F35 F64 F65 F78 F79 F94 F95
    Sprung-Harness   : F1 F6 F8 F9 F24 F25 F35 F64 F65 F78 F79 F94 F95

Alle Zahlen dieses Dossiers sind aus dem ECHTEN Übergang; die Bildbelege stammen aus dem
validierten Harness.

### 1.5 Der Pose-Strom — das entscheidende Maß (RE15_ANIM_TRACE)

Die Zustandsspur allein kann „welcher Clip ist gerade auf dem Schirm" nicht beantworten.
`re15_compute_actor_kf` ist die EINZIGE Stelle, an der eine Pose aufgelöst wird; ihr
Trace ist die Bodenwahrheit:

    F1..F5   type=66 mo=0  clip=0  fc=22  cur=0     slot=0    rev=0
    F6       type=66 mo=5  clip=5  fc=30  cur=1     slot=1
    F7,F8    type=66 mo=5  clip=5  fc=30  cur=2,3   slot=2,3
    F9..F23  type=66 mo=1  clip=1  fc=16  cur=1..15 slot=1..15
    F24      type=66 mo=1  clip=1  fc=16  cur=0     slot=0
    F25..F34 type=66 mo=2  clip=2  fc=52  cur=1..10 slot=1..10
    F35..F55 type=66 mo=0  clip=0  fc=22  cur=1..21 slot=1..21
    F56..     ... Clip 0 laeuft weiter (Schleife)

`type=66` = 0x42 = Ada. **Clip 0 (22 Bilder) in F1–F5 und wieder ab F35.**

### 1.6 Bildbeleg (echter Renderpfad)

`ada-eintritt-1050/vorher_eintritt.png` — F5/F6/F7 (Bein in der Luft, Laufschritt),
F9/F12/F20 (Stand), F37/F40 (der echte Weglauf). Die ersten sichtbaren Bilder des Raums
zeigen Ada mitten im Schritt; F1–F4 sind noch schwarz (Blende).

`ada-eintritt-1050/nachher_eintritt.png` — dieselben Bildnummern nach dem Fix: F5/F6/F7
stehend, beide Füße am Boden. F37/F40 (der echte Weglauf) sind in beiden Streifen
identisch — die Animation läuft jetzt genau EINMAL.

Pose-Strom nach dem Fix, dieselbe Messung:

    F1..F5   mo=2  clip=2  fc=52  cur=0  slot=0     <- Steh-Clip statt Laufzyklus
    F6..F8   mo=5  clip=5  fc=30  cur=1..3
    F9..     mo=1  clip=1  fc=16  cur=1..

Ab Bild 6 ist alles unverändert (byte-true, §2.1).

---

## 2. Original-Mechanismus — alle Adressen selbst disassembliert

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` gegen
`info/Re1.5/PSX.EXE` bzw. `info/Re1.5/PSX/BIN/STAGE1.BIN`.

### 2.1 Was die Kette macht, die Ada in ROOM1050 animiert — sie ist BYTE-TRUE

Damit kein Zweifel bleibt, welche Teile der Szene KEIN Defekt sind:

**`Plc_dest` (0x40) @0x80041be4** — Idempotenz-Wächter und Re-Init:

    80041bec  lhu  v0,452(a1)          ; +0x1c4
    80041bf8  andi v0,v0,0x4
    80041bfc  beq  v0,zero,0x80041c14  ; Bit 0x04 aus -> IMMER re-initialisieren
    80041c04  lbu  v0,5(a1)            ; +0x5
    80041c0c  beq  v0,a2,0x80041c24    ; gleicher Sub -> Re-Init ueberspringen
    80041c14  sb   v0,4(a1)            ; +0x4 = 4
    80041c18  sb   a2,5(a1)            ; +0x5 = Modus
    80041c1c  sb   zero,6(a1)          ; +0x6 = 0
    80041c20  sb   zero,7(a1)          ; +0x7 = 0
    80041c24  sb   v1,451(a1)          ; +0x1c3 = pc[3]
    80041c38  sh   v0,444(a1)          ; +0x1bc = Ziel-X
    80041c4c  sh   zero,452(a1)        ; +0x1c4 = 0
    80041c50  sltiu v0,v0,0x10         ; Typ < 0x10 ?
    80041c54  beq  v0,zero,0x80041d40  ; Typ >= 0x10 -> NICHT-Spieler-Zweig
    80041c58  sh   v1,446(a1)          ; +0x1be = Ziel-Z

Ada ist Typ 0x42, nimmt also @0x80041d40: `v1 = modus-4`, `sltiu v1,0x6` (Modi 4..9),
Sprungtabelle @0x80010de4[6]; Modus 9 → Eintrag [5] = 0x80041e10 → `+0x1c8` aus
@0x80076c41+(typ-0x40)*2, dann @0x80041e38 `sh zero,458(a1)` (+0x1ca = 0). **Kein
Schreiben auf +0x94.**

**Sub 9 = TURN @0x80051cf8** (Tabelle @0x80076ca0[9], selbst gedumpt):

    80051d2c  sb v0,6(a0)      ; +0x6 = 1 (INIT)
    80051d3c  sb v0,148(v1)    ; +0x94 = 5   <- der Dreh-/Geh-Clip
    80051d4c  sb zero,149(v0)  ; +0x95 = 0
    80051d5c  sb v0,143(v1)    ; +0x8f = 7
    80051d90  jal 0x8001ab9c   ; arc_test(ziel, cone[typ] @0x80076c41)
    80051d98  bne v0,zero,0x80051e14        ; nicht ausgerichtet -> nur steuern
    80051dac  sb v0,5(v1)      ; +0x5 = 6   <- AUSGERICHTET: an Sub 6 uebergeben
    80051dbc  sb zero,6(v0)    ; +0x6 = 0
    80051dd8  jal 0x8004ef90   ; Flag(bank 0x800b1028, +0x1c3) = 1
    80051e98  addu a2,zero,zero / 80051ea4 jal 0x8001f314  ; anim_set(+0x170,+0x174, RICHTUNG 0)

**Sub 6 = EVENT-REACH @0x800517f0** (Tabelle @0x80076ca0[6]):

    80051844  sb v1,6(a0)      ; Phase 0 -> +0x6 = 1
    80051854  sb v1,148(v0)    ; +0x94 = 1   <- CLIP 1
    80051864  sb zero,149(v0)  ; +0x95 = 0
    80051874  sb v0,143(v1)    ; +0x8f = 7
    80051878  jal 0x8001f314   ; spielen (a2 = 0 = vorwaerts, @0x80051880)
    800518a8  addu v1,v1,v0 / 800518b0 sb v1,6(a0)   ; Clip zu Ende -> Phase 2
    800518b8  sb v0,6(a0)      ; Phase 2 -> +0x6 = 3
    800518c8  sb v0,148(v1)    ; +0x94 = 2   <- CLIP 2 = Ruhe-Schleife
    800518dc  jal 0x8001f314

Die Kette Modus 9 → ausgerichtet → Sub 6 → Clip 1 → Clip 2 ist damit **byte-true**; die
Clip-1-Wiederholung, die die Vorrunde in beiden Kandidatenräumen gesehen hat, ist
tatsächlich das, was das Originalskript tut (zwei `Plc_dest`, jedes mit eigener Ankunft),
und **nicht** der gemeldete Fehler.

Gegengeprüft wurde auch Leons Geste in derselben Szene (`Plc_motion(0,15)` @0x0DDA,
`Plc_motion(0,15)` + `Plc_flg(0,0x80)` @0x0DE2/@0x0DE6). Der Pose-Strom zeigt sie
korrekt als **vorwärts + rückwärts**, nicht zweimal vorwärts:

    F59..F78  clip=15 slot=0..19  rev=0     (vorwaerts)
    F79..F88  clip=15 slot=19     rev=0     (Halten)
    F89..F107 clip=15 slot=18..0  rev=1     (RUECKWAERTS)

Das entspricht @0x80050d44/@0x80050d50 (`lbu a2,452(v0)` / `srl a2,a2,7` = Bit 0x80 =
Richtung) und @0x8001f338-54 (Spiegelung des Bildindex). Auch hier: kein Defekt.
`Plc_motion` @0x80041b90 schreibt `+0x1c4 = pc[3]` (`lhu a1,2(v0)` @0x80041b9c,
`srl a1,a1,8` @0x80041bac, `sh a1,452(v0)` @0x80041bc8) — der Port tut dasselbe.

### 2.2 `Sce_em_set` schreibt +0x94 NIE

Vollständiger Handler-Scan FUN_800420a0, 0x800420a0..0x8004262c: **kein einziger Store auf
`148(s0)`**. Was er anfasst:

    8004216c  sh zero,452(s0)   ; +0x1c4 = 0
    800421d0  sb v0,130(s0)     ; +0x82 = Etage/Band aus pc[4]
    800421e0  sw zero,4(s0)     ; +0x4..+0x7 = 0 als WORT (State/Sub/Phase)
    800421e4  sb zero,146(s0)   ; +0x92
    800421e8  sb zero,147(s0)   ; +0x93
    800421ec  sh zero,152(s0)   ; +0x98
    800421f0  sb zero,142(s0)   ; +0x8e
    800421f4  sh zero,448(s0)   ; +0x1c0
    80042210  sh v0,442(s0)     ; +0x1ba = -(pc[4]*1800)
    800422bc  sb v0,8(s0)       ; +0x8 = Typ
    8004262c  addiu v1,v1,20    ; PC += 20

### 2.3 Der Raumwechsel räumt am Entity nur +0x00

`FUN_8001a4c0` — 20 Slots (0x13..0), Basis 0x800acc2c, Stride
`((n<<5)-n)<<2 + n)<<2` = 500 = 0x1f4:

    8001a4c0  ori   a0,zero,0x13
    8001a4e0  addiu at,at,-13268      ; 0x800acc2c
    8001a4e8  sw    zero,0(at)        ; NUR Feld +0x00
    8001a4f4  bne   v0,zero,0x8001a4c4

**+0x94 überlebt also den Raumwechsel.** Im Original steht dort beim Aufblenden der
Wert aus dem Vorraum (für Ada: ihr letzter ROOM1090-`Plc_motion`-Clip 23) — in keinem Fall
der Laufzyklus 0.

### 2.4 Die Pose kommt aus dem State-0-INIT der TYP-EIGENEN Overlay-Wurzel

Typ → Wurzel, belegt am Installer `FUN_8011e864` (STAGE1.BIN):

    8011e914  addiu v0,v0,-14944   ; 0x8011c5a0   -> 8011e91c sw v0,11436(at) = 0x80072cac (Typ 0x40)
    8011e924  addiu v0,v0,-13456   ; 0x8011cb70   -> 8011e92c sw v0,11444(at) = 0x80072cb4 (Typ 0x42)
    8011e934  addiu v0,v0,-11968   ; 0x8011d140   -> 8011e93c sw v0,11456(at) = 0x80072cc0 (Typ 0x45)
    8011e944  addiu v0,v0,-10540   ; 0x8011d6d4   -> 8011e94c sw v0,11464(at) = 0x80072cc8 (Typ 0x47)
    8011e954  addiu v0,v0,-9112    ; 0x8011dc68   -> 8011e95c sw v0,11472(at) = 0x80072cd0 (Typ 0x49)
    8011e964  addiu v0,v0,-7636    ; 0x8011e22c   -> 8011e96c sw v0,11480(at) = 0x80072cd8 (Typ 0x4b)

0x80072cb4 = Dispatch-Tabelle 0x80072bac + 0x42·4 ✓ (die Tabelle steht statisch auf 0 und
wird beim Overlay-Laden gefüllt — deshalb ist sie in der EXE „leer").

Adas Wurzel 0x8011cb70 dispatcht auf `+0x4` über @0x80121668; Eintrag [0] = **0x8011ccac =
der State-0-INIT**. Von seinem Eintritt bis zur Pose gibt es **keine Verzweigung**:

    8011ccc0  sb v0,4(v1)      ; +0x4 = 1
    8011cd18  sh v0,154(v1)    ; +0x9a = -1  (unverwundbar)
    8011cd8c  ori v0,zero,0x2
    8011cd90  sb v0,148(v1)    ; +0x94 = 2      <---- DIE SPAWN-POSE
    8011cda0  sb zero,149(v0)  ; +0x95 = 0
    8011cdb0  sb zero,143(v0)  ; +0x8f = 0
    8011cdc0  sb zero,147(v0)  ; +0x93 = 0
    8011cdd8  jal 0x8001f314   ; anim_set(+0x84, +0x16c, 0, 0x200)
    8011ce24  andi v0,v0,0x40  ; grid & 0x40 ...
    8011ce30  sb v0,4(v1)      ;   -> +0x4 = 4
    8011ce40  sb v0,5(v1)      ;   -> +0x5 = 6

Ein Voll-Scan von STAGE1.BIN nach `sb <reg>,148(<reg>)` mit unmittelbar vorangehendem
`ori <reg>,zero,<imm>` zeigt, dass **alle sechs NPC-Wurzeln denselben Wert setzen**:

| Typ | Wurzel | INIT-Store | Wert |
|---|---|---|---|
| 0x40 | 0x8011c5a0 | @0x8011c7c0 | 2 |
| 0x42 | 0x8011cb70 | @0x8011cd90 | 2 |
| 0x45 | 0x8011d140 | @0x8011d39c | 2 |
| 0x47 | 0x8011d6d4 | @0x8011d930 | 2 |
| 0x49 | 0x8011dc68 | @0x8011de7c | 2 |
| 0x4b | 0x8011e22c | @0x8011e454 | 2 |

---

## 3. Port-Ist und Fix

### 3.1 Was der Port tat

1. `re15_room_apply_pending` (room_common.c:257) ruft `re15_actor_init()` — ein
   **flächiger memset** aller Aktoren. Das Original räumt am Entity nur +0x00 (§2.3);
   +0x94 ist im Port danach 0, im Original nicht.
2. `op_sce_em_set` setzt `a->motion = re15_enemy_spawn_action(type, behavior)`
   (scd_vm.c). Diese Funktion endete mit
   `return 0;  /* other types: their overlay decoders not yet RE'd → spawn at idle
   action 0 */` — ein ausdrücklich als ungeprüft markierter Platzhalter. Für Typ 0x42 ist
   Clip 0 der **Laufzyklus** (22 Bilder), nicht ein Idle.
3. Der State-0-INIT, der im Port denselben Wert setzen würde
   (`enemy_ai_common.c` `case 0: ... e->motion = 2;`), läuft in ROOM1050 **nie**: sub00
   spawnt Ada und startet im selben VM-Tick `Evt_exec sub03`, dessen `Plc_dest` +0x4 sofort
   auf 4 setzt (@0x80041c14). Es gibt also nichts, was den Saat-Wert nachträglich
   korrigiert.

Ergebnis: fünf Bilder Laufzyklus beim Aufblenden, dann derselbe Clip ab Bild 35 noch
einmal — die gemeldete Doppelung.

### 3.2 Der Fix

`re15_port/engine/src/scd_vm.c`, `re15_enemy_spawn_action`: die NPC-Familie bekommt den
INIT-Literal ihrer eigenen Wurzel.

```c
if (type == 0x40 || type == 0x42 || type == 0x47 || type == 0x49 || type == 0x4b)
    return 2;
```

mit der vollen Belegkette (§2.2/§2.3/§2.4) im Kommentar. **Typ 0x45 bleibt bei 3** — das
ist die dokumentierte, bewusste Abweichung für die ROOM1150-Irons-Cutscene und wird nicht
angefasst.

**Warum genau dieser Wert und keine geschätzte Zahl:** das Original hat an dieser Stelle
keinen Spawn-Schreibvorgang auf +0x94 (§2.2); der Wert kommt dort aus dem Vorraum, weil
der Raumwechsel das Entity nicht räumt (§2.3). Der Port KANN das nicht nachbilden, solange
`re15_room_apply_pending` alle Aktoren memset-t (offener Punkt 2). Der byte-nächste
belegbare Wert ist deshalb der Literal, den die typ-eigene Wurzel in ihrem State-0-INIT
selbst schreibt (§2.4) — derselbe Wert, den auch die Port-Seite des INIT eine Tick später
setzen würde (`enemy_ai_common.c` `case 0: e->motion = 2;`), und derselbe bei allen sechs
NPC-Wurzeln. Es ist damit keine gewählte Zahl, sondern ein zitierter Store.

### 3.3 Riegel + Gegenprobe

`re15_port/tests/unit/test_r18_ada_eintritt_1050.c`, registriert aus der EIGENEN Datei
`re15_port/tests/unit/probes/r18_ada-eintritt-1050.cmake` (GLOB-eingebunden, keine
Kollision mit parallel laufenden Agenten).

Der Riegel fährt genau den Eintritt: ROOM1050.RDT laden, `Flag(3,0x6e)` setzen (das Tor,
das ROOM1090s sub03 @0x24D6 stellt), `scd_room_reenter` → main00+sub00 →
`Sce_em_set` @0x0C8E + `Evt_exec sub03`, und prüft die Pose **vor dem ersten KI-Tick** —
also genau den Zustand, den die Tür-Blende fünf Bilder lang zeigt. Zusätzlich wird
geprüft, dass Ada überhaupt gespawnt ist und dass sie in State 4 / Sub 9 steht (Beleg,
dass der INIT wirklich übersprungen wird).

MIT Fix:

    [M1] Slot 1: active=1 type=0x42 grid=0x40 motion=2 anim_frame=0 pos=(18550,-22300)
    OK: Ada betritt ROOM1050 in Clip 2 (Steh-Pose), nicht im Laufzyklus
    EXIT=0

**Gegenprobe** (Fix per `if (0 && ...)` ausgeschaltet, neu gebaut, gefahren):

    [M1] Slot 1: active=1 type=0x42 grid=0x40 motion=0 anim_frame=0 pos=(18550,-22300)
    FAIL: Ada-Spawn-Pose +0x94 = 0, erwartet 2 (State-0-INIT der Wurzel 0x8011cb70: ...)
    FAIL: 1 Pruefung(en)
    Gegenprobe EXIT=1

---

## 4. Was ausdrücklich KEIN Defekt ist (gegen den Vorrunden-Verdacht)

* **Ada wird beim Eintritt nicht doppelt aufgesetzt.** Die Opcode-Spur zeigt genau EIN
  `Sce_em_set` @0x0C8E und genau EIN `Evt_exec sub03` (`thread start slot=10`), ebenso
  genau ein `thread start slot=11` für sub04. Über den ganzen Lauf (bis F2697) gibt es
  keinen zweiten Spawn und keinen zweiten Szenenstart.
* **Der Eskorten-Aktor aus ROOM1090 wird nicht mitgeschleppt.** Der Port löscht beim
  Raumwechsel alle Aktoren (`re15_actor_init`, room_common.c:257) und ROOM1050 spawnt
  Ada frisch. Die Hypothese „Begleiter-Aktor existiert schon und wird ein zweites Mal
  aufgesetzt" ist damit **widerlegt** — der Defekt liegt im Gegenteil genau darin, dass
  dieser flächige Reset im Original nicht existiert (§2.3) und der Ersatz-Saat-Wert falsch
  war.
* **Die beiden Clip-1-Durchläufe (Bild 9 und Bild 79) sind byte-true.** Beide kommen aus
  Sub 6, beide Male ausgelöst von einem eigenen `Plc_dest` des Raumskripts; der zweite
  läuft ausserdem bei (850,-14200), also ausserhalb des Kamerabilds (`Member_set` @0x0E2C/
  @0x0E30).
* **Leons „Hey, wait!"-Geste läuft korrekt vorwärts + rückwärts** (§2.1), nicht zweimal
  vorwärts.

## 5. Offen / bewusst nicht angefasst

1. **Typ 0x4d (STAGE6-NPC).** Der Port routet ihn in dieselbe NPC-Familie; sein INIT
   liegt laut Port-Kommentar @0x80101918 in STAGE6.BIN. Ich habe nur STAGE1 gescannt und
   ihn deshalb NICHT in den Fix aufgenommen — kein `@0x…`-Beleg, also keine Zahl.
2. **Der flächige `re15_actor_init()` beim Raumwechsel** bleibt (das Original räumt nur
   +0x00, §2.3). Das ist eine bekannte, davon unabhängige Port-Abweichung; sie wird hier
   nicht umgebaut, sondern an der einen Stelle korrekt gesättigt, an der sie sichtbar
   wurde.
3. **`Cut_chg 6` in sub03 @0x0D94 setzt `cut_auto` im Original auf 0**; weil der
   Tür-Eintrittscut ebenfalls 6 ist, greift in `re15_room_apply_pending` Schritt (8) der
   Zweig `scd_queued_cut < 0` und Schritt (12) schaltet die RVD-Autokamera wieder ein.
   Gemessen hat das in dieser Szene keine sichtbare Folge (cam bleibt 6 über die ganze
   Cutscene), es ist aber eine echte Abweichung und gehört in eine eigene Runde.
4. **`+0x1c8`/`+0x1ca`** (die Modus-Tabellenwerte, die `Plc_dest` @0x80041d70-e38 schreibt)
   hat der Port nicht als Felder; die Sub-INITs holen ihre Rate stattdessen direkt aus den
   Tabellen. Verhaltensgleich, aber nicht feldgleich.

---

## 6. Schlussbericht

**Was gemessen wurde.** Der echte Übergang ist gefahren: Sprung nach ROOM1090,
Flag(3,0x84) gesetzt, die Feuer-Rettungs-Cutscene per Regler durchgeklickt bis `Plc_ret`
(Ada = Eskorte, NPC-State 1), dann Tür-Slot 0 gefeuert und ab Bild 0 von ROOM1050
protokolliert: Zustandsspur (`RE15_STATE_LOG`: State +0x4, Sub +0x5, Phase +0x6,
Clip +0x94, Clip-Bild +0x95, Flags, Position), Opcode-Spur mit RDT-Offset
(`RE15_SCD_TRACE`) und Pose-Strom (`RE15_ANIM_TRACE`). Dazu Bildreihen aus dem echten
Renderpfad (`RE15_FRAMEDUMP`, Rücklesung vor `SDL_RenderPresent`; kein AUTOSHOT, kein
Softwarerenderer). Die schnellere Sprung-Variante des Harness ist gegen den echten
Übergang validiert (bild-identische Moduswechsel, §1.4).

**Welche Adressen.**
`Sce_em_set` FUN_800420a0 (kein +0x94-Store im ganzen Handler 0x800420a0..0x8004262c;
+0x4..+0x7 @0x800421e0, +0x1c4 @0x8004216c) · Raumwechsel-Entity-Clear FUN_8001a4c0
@0x8001a4e8 (nur +0x00) · Typ→Wurzel-Installer FUN_8011e864 @0x8011e924/@0x8011e92c →
0x80072cb4 = 0x80072bac + 0x42·4 · Ada-Wurzel 0x8011cb70, State-Tabelle @0x80121668,
State-0-INIT @0x8011ccac mit `ori v0,zero,0x2` @0x8011cd8c + `sb v0,148(v1)` @0x8011cd90 ·
Geschwister-INITs @0x8011c7c0 (0x40), @0x8011d39c (0x45), @0x8011d930 (0x47),
@0x8011de7c (0x49), @0x8011e454 (0x4b) · byte-true gegengeprüft und NICHT verändert:
`Plc_dest` @0x80041be4 (Nicht-Spieler-Zweig @0x80041d40, Modus-Tabelle @0x80010de4),
`Plc_motion` @0x80041b90, Sub-9-Turn @0x80051cf8 (@0x80051d3c/@0x80051dac/@0x80051dbc),
Sub-6-Event-Reach @0x800517f0 (@0x80051854/@0x800518c8), Motion-FSM @0x80050cb8
(@0x80050d44/@0x80050d50), `anim_set` @0x8001f314 (@0x8001f338-54), Sub-Tabelle
@0x80076ca0.

**Was geändert wurde.** Eine Stelle: `re15_enemy_spawn_action` in
`re15_port/engine/src/scd_vm.c` gibt für die NPC-Familie 0x40/0x42/0x47/0x49/0x4b den
INIT-Literal 2 statt des Platzhalters 0 zurück (0x45 bleibt bei 3). Neu:
`re15_port/tests/unit/test_r18_ada_eintritt_1050.c` und
`re15_port/tests/unit/probes/r18_ada-eintritt-1050.cmake` (Test `unit_ada_eintritt_1050`),
dieses Dossier und zwei Bildbelege.

**Testzahl.** `bash re15_port/tools/local_build.sh all` — 329/329 grün (328 vorher plus
der neue Riegel). Gegenprobe des Riegels: ohne den Fix Exit 1 (`+0x94 = 0`).
(Anmerkung: im ersten Suite-Lauf fiel `integration_relatch_pin` mit `exit=-1` nach 18,6 s
aus und lief allein sofort wieder grün — das ist der bekannte flatterhafte exe-Testhaken,
nicht diese Änderung.)

**Was offen blieb.** §5: Typ 0x4d (STAGE6, kein STAGE1-Beleg, deshalb nicht angefasst),
der flächige `re15_actor_init()` beim Raumwechsel (Original räumt nur +0x00), die
`Cut_chg`-/`cut_auto`-Abweichung bei gleichem Skript- und Tür-Cut, sowie die nicht
abgebildeten Felder +0x1c8/+0x1ca von `Plc_dest`.
