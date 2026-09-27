# Elza im Port — Zustandsmessung (2026-09-27)

Auftrag: MESSEN, nicht bauen. Alles hier ist entweder ein LAUF (debug.log / Bilddump /
Zaehler) oder eine Adresse, die ich selbst disassembliert habe. Keine Schaetzwerte.

Bau: `local_build.sh` gruen. Exe: `re15_port/build/platform/pc/re15_pc.exe`.

---

## Kurzfassung

Die Charakterwahl funktioniert, ist erreichbar und liefert 0/1 korrekt ab — und dann
**verpufft sie**. Zwei Vollstarts (Leon / Elza) erzeugen **bitgleiche Bilder**. Die Ursache
ist nicht eine fehlende Abfrage, sondern dass der Port nur **eines von zwei** Original-Feldern
kennt, und das in einer um 2 Bit verschobenen Kodierung:

| Original | Wert Leon | Wert Elza | steuert | im Port |
|---|---|---|---|---|
| `DAT_800ACA5C` | 0 | **4** | Spielermodell, CORE-Bank, Gore-Zweige | `g_gameflow.character` **= 0/1**, also verschoben |
| `DAT_800ACA3C` Bit 31 | 0 | **1** | **RDT-Dateiwahl + Startraum** | **existiert nicht** |

---

## (2) Ist die Charakterwahl erreichbar? — JA, sie laeuft vollstaendig

Weg: Titel -> (Cursor steht auf NEW GAME) -> Bestaetigen -> `pc_run_player_select()`
(`re15_port/platform/pc/main.c:1953`, Aufruf `main.c:2974`) -> LINKS/RECHTS wechselt,
Kreuz bestaetigt.

Gemessener Lauf (Elza), aus `debug.log`:

    [input-script] RE15_INPUT_SCRIPT="W2,S1,W250" -> 7590 ticks @ 30 fps, start frame 30
    [flow] title confirm tblink=90 cursor=0
    [flow] pselect enter
    [flow] pselect done ch=1

Kommandozeile:

    RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 \
    RE15_FADE_LOG=1 RE15_BOOT_EXIT_AT=1 RE15_INPUT_SCRIPT="W2,S1,W250" \
    RE15_INPUT_SCRIPT_START=30 ./re15_pc.exe

Derselbe Lauf ohne `RE15_PSELECT_AUTO_SWITCH` liefert `pselect done ch=0` (Leon).
Die Wahl wird also getroffen und kommt als 0/1 an. **Der Auswahlschirm ist nicht der Defekt.**

Zwei Randnotizen, die man beim Testen kennen muss:
* `RE15_TITLE_SHOT` (der Auto-Vorlauf, den die vier GUI-Haken benutzen) ruft
  `main.c:3045` `re15_gameflow_new_game(0)` **direkt** — an `pc_run_player_select` vorbei.
  Jeder Test, der so startet, sieht immer Leon.
* `RE15_MODE_CHARSELECT` (`re15_gameflow.h:18`) ist im ganzen Port **nirgends** benutzt
  (Vollscan ueber engine/platform/include: 2 Treffer, beide sind die Deklaration selbst
  und ein Kommentar). Die Auswahl laeuft als Unterschleife *innerhalb* von `RE15_MODE_TITLE`,
  der Modus ist tote Deklaration.

## (1) Was passiert, wenn man Elza waehlt? — exakt dasselbe wie bei Leon

Zwei Vollstarts, identische Umgebung bis auf `RE15_PSELECT_AUTO_SWITCH`
(= RECHTS im Auswahlschirm), je bis in die Hubschrauber-Szene gefahren:

| Messgroesse | Leon (ch=0) | Elza (ch=1) | Quelle |
|---|---|---|---|
| Startraum-Id | 0x1240 | 0x1240 | `[boot] room RDT: STAGE1/ROOM1240.RDT (163744 bytes)` |
| RDT-Datei | STAGE1/ROOM1240.RDT | STAGE1/ROOM1240.RDT | dito |
| Spieler-Spawn | (-26214,0,-3861) yaw=0 | (-26214,0,-3861) yaw=0 | `[boot] player spawn for ROOM1240` |
| Spielermodell Mesh/Skelett | PL00 | **PL00** | `[skel] PL00: 15 bones, 24 clips, 712 keyframes` |
| Spieler-TIM | PLD/PL00.TIM | PLD/PL00.TIM | `main.c:3227` (fest verdrahtet) |
| Waffen-Handmeshes | PL00W** | **PL04W** | `[wpn] loaded 21/21 PL**W** in-hand meshes` |
| Waffen-Animbaenke | PL00W**.PLW | PL04W**.PLW | `[wpn-bank] 21/21 Animationsbaenke` |
| Folgeraum der Intro-Kette | ROOM1170 | ROOM1170 | `[bg-log] F750 blit room1170#02` |
| CORE-SE-Bank | CORE00 | CORE04 | `main.c:2985` `re15_audio_prime_core(ch ? 4 : 0)` |
| Kartentitel / Speicherort | keine Charakter-Abhaengigkeit, s. (4) | | |

`diff` der beiden kompletten `debug.log`: **genau drei Zeilen** unterscheiden sich —
`pselect done ch=`, `[wpn] … PL00W**/PL04W**`, `[wpn-bank] … PL00W**/PL04W**`. Sonst nichts.

### Bildbeweis: 0 von 691200 Pixeln Unterschied

Beide Laeufe mit `RE15_FRAMEDUMP="100-1400/100:<praefix>_"`, Bild 600/700/800/900/1000
paarweise pixelweise verglichen:

    600 abweichende pixel 0 von 691200
    700 abweichende pixel 0 von 691200
    800 abweichende pixel 0 von 691200
    900 abweichende pixel 0 von 691200
    1000 abweichende pixel 0 von 691200

Die Abzuege liegen in `analysis/befunde_2026-09-27/elza_shots/`:
`elza_gewaehlt_f700_room1170.png` und `leon_gewaehlt_f700_room1170.png` sind **bitgleich** —
auf beiden steht Leon in Polizeiuniform mit Elliot vor dem Hubschrauber auf dem Dach
(ROOM1170, Leons Helipad).

**Befund 1: Die Wahl ist folgenlos.** Wer Elza waehlt, spielt Leons Raumkette mit Leons
Modell. Der einzige Unterschied im ganzen Spiel ist die Waffen-Familie — und die erzeugt
sogar eine **Mischung**: Elzas PL04-Handmeshes an Leons PL00-Koerper
(`main.c:3389` `const char *wpn_fam = (g_gameflow.character == 0) ? "PL00" : "PL04";`),
waehrend Mesh/Textur/Skelett/Animation bei PL00 bleiben (`main.c:3227/3243/3453/3454`).

---

## (3) Der &4-Verdacht — bestaetigt, mit Zaehler

### Messung (Zaehler, nicht Codelektuere)

`probe_re2z_stomp` (ROOM1030, echte RE2-Baenke, 1800 Frames, 2 Kriecher) erreicht genau die
Charakter-Weiche `enemy_ai_re2_zombie.c:1808`. Fuer die Messung bekam die Sonde
**voruebergehend** ein `g_gameflow.character = atoi(getenv("MESS_CHAR"))` vor
`re15_ai_flavor_set` (danach wieder entfernt — der Arbeitsbaum ist sauber). Drei Laeufe:

| MESS_CHAR | `character & 4` | Kopf-ab-Ticks (Part8&0x40) | SE id2 "smash" | SE id8 | Zweig |
|---|---|---|---|---|---|
| 0 (Leon)                 | 0 | **2462** | 2 | 0 | Abriss / Kopf ab |
| 1 (Elza, Port-Kodierung) | 0 | **2462** | 2 | 0 | **derselbe** |
| 4 (Original-Byte ACA5C)  | 4 | **0**    | 0 | 2 | der andere |

Alle uebrigen Zaehler (Griff-Eintritte 2, P6-Ticks 2, CORPSE-Ticks 2424, Biss-SE 6) sind bei
0 und 1 identisch. Bei 4 schlagen genau die beiden Haken an, die den Kopf-ab-Zweig pruefen.

**Befund 2: Der zweite Zweig laeuft mit der Port-Kodierung nie.** Er ist erst ab Wert 4
erreichbar, und 4 kann `g_gameflow.character` nicht annehmen.

### Warum: die Port-Kodierung ist um 2 Bit verschoben — mit Beleg

Der Original-Test (STAGE1.BIN, RE1.5-Kriech-Grab), selbst disassembliert:

    80104000: lbu  v0,-13732(v0)      ; DAT_800ACA5C
    80104008: andi v0,v0,0x4
    8010400c: bne  v0,zero,0x801040c4

Und was in `DAT_800ACA5C` steht, schreibt der Auswahlschirm selbst (TITLE.BIN):

    801024a4: bne  a1,zero,0x801024cc  ; a1 = Cursor-Byte (0 = Leon, !=0 = Elza)
    801024c0: sb   zero,-13732(at)     ; LEON : DAT_800ACA5C = 0
    801024cc: ori  v0,zero,0x4
    801024d4: sb   v0,-13732(at)       ; ELZA : DAT_800ACA5C = 4

Zweiter, unabhaengiger Schreiber derselben Groesse (TITLE.BIN Szenen-Init):

    801016a4: sll  v0,v0,2             ; Auswahl-Index << 2
    801016ac: sb   v0,-13732(at)       ; DAT_800ACA5C = index*4

Also **`DAT_800ACA5C` ∈ {0, 4}**, nicht {0, 1}. `g_gameflow.character` traegt den INDEX (0/1),
der Port testet aber das Bit des ungeschobenen Bytes.

Vollzaehliger Xref-Scan ueber `info/Re1.5/PSX.EXE` + alle `PSX/BIN/*.BIN` auf `0x800aca5c`:
**47 Zugriffe** — PSX.EXE 34 (davon 1 Schreiber @0x8001d4f4), DEBUG.BIN 4, TITLE.BIN 4
(darunter die 3 Schreiber), und je genau **ein** Leser in STAGE1..5 mit identischer
Instruktionsfolge `lbu / andi 0x4 / bne`:
STAGE1 @0x80104000, STAGE2 @0x80103e94, STAGE3 @0x801040ec, STAGE4 @0x80103fb4,
STAGE5 @0x80104134.

Nebenbei gemessen (`@0x8003976c-8c`): das Original vergleicht `aca5c & 0x0F` mit dem
Charakter-Byte `DAT_800B0FF0` und laedt bei Abweichung das Spielermodell neu
(`jal 0x800314b0`) — die **untere Nibble** von ACA5C IST der Charakter.

### ⛔ Die Raumwahl haengt an einer ZWEITEN Groesse — DAT_800ACA3C Bit 31

Derselbe Auswahl-Zweig in TITLE.BIN setzt noch ein zweites Feld:

    801024a8: lui  v1,0x8000          ; Delay-Slot des Elza-Branches: v1 = 0x80000000
    801024ac: lui  v1,0x7fff          ; Leon-Pfad
    801024b8: ori  v1,v1,0xffff       ;   v1 = 0x7fffffff
    801024c8: and  v0,v0,v1           ; LEON : DAT_800ACA3C &= 0x7fffffff  (Bit 31 = 0)
    801024e4: or   v0,v0,v1           ; ELZA : DAT_800ACA3C |= 0x80000000  (Bit 31 = 1)
    801024ec: sw   v0,-13764(at)      ; -> DAT_800ACA3C

Und genau dieses Bit 31 **waehlt die RDT-Datei** (PSX.EXE, Raumlader):

    800397a8: lh   v0,4064(v0)        ; Stage      = DAT_800B0FE0
    800397b0: lh   v1,4066(v1)        ; Raumindex  = DAT_800B0FE2
    800397b8: lw   a0,-13764(a0)      ; a0 = DAT_800ACA3C
    800397cc: addiu at,at,17292       ; Stage-Tabelle @0x8007438C
    800397e0: lhu  v0,0(v1)           ; v0 = CD-Datei-Id des Raums
    800397e4: srl  a0,a0,31           ; a0 = 0 (Leon) / 1 (Elza)
    800397e8: jal  0x80013b60         ; CD-Load
    800397ec: addu a0,a0,v0           ; Datei-Id = Basis + Variante

**Byte-true-Regel: `datei_id = stage_tabelle[stage][raumindex] + (ACA3C >> 31)`.**
Die Stage-1-Tabelle @0x8007429C hat 40 u16-Eintraege im Abstand 3
(`681, 684, 687, 690, 693, …`) — ein Raum belegt drei CD-Dateien, und die **Elza-RDT ist
die Leon-RDT + 1**. Das ist exakt die gerade/ungerade Paarung der Dateinamen, die der
Nutzer beschreibt.

Und der Neues-Spiel-Einstieg verzweigt auf dasselbe Bit (PSX.EXE, Zweig hinter
`aca38 & 0x20000`, der Gegenzweig @0x8001d49c ist der CONTINUE/Load-Pfad aus
DAT_800B0FC0..):

    8001d29c: lw   v0,-13764(v0)      ; DAT_800ACA3C
    8001d2a4: bltz v0,0x8001d324      ; Bit 31 gesetzt (= ELZA) -> anderer Zweig
    8001d2a8: ori  v0,zero,0x17       ; LEON : Raumindex 0x17 (= ROOM1170)
    8001d2b0: sh   v0,4066(at)        ;        + Pos (0x952, 0x35b1) rot -2904 floor 4
    8001d324: ori  v0,zero,0x3        ; ELZA : Raumindex 0x03 (-> Datei 690+1 = ROOM1031)
    8001d32c: sh   v0,4066(at)        ;        + Pos (-8888, -12989) floor 0

**Befund 3: Das Original hat die Elza-Weiche vollstaendig — zwei Felder, nicht eines.**
`DAT_800ACA5C` (0/4) steuert Modell, CORE-Bank und die Gore-Zweige; `DAT_800ACA3C` Bit 31
steuert Dateiwahl und Startraum. Der Port kennt nur das erste, und das in falscher
Kodierung. Fuer das zweite gibt es im Port **kein Feld**: Scan ueber `re15_port/engine` +
`platform` + `include` nach `aca3c` liefert 79 Fundstellen, darunter die Bits 0x1, 0x40,
0x4000, 0x8000 — aber **keine einzige** fuer Bit 31 / 0x80000000 / `>>31`.

⚠️ Offen (nicht geraten, nur nicht zu Ende verfolgt): wer `aca38 & 0x20000` setzt und ob
der Port-Startraum 0x1240 (die Montage) vor oder hinter diesem Zweig haengt. Der EXE-Zweig
nennt fuer Leon Raumindex 0x17 = ROOM1170, nicht 0x1240 — `RE15_NEWGAME_ROOM 0x1240`
(`re15_gameflow.c:16`) ist damit selbst eine Port-Entscheidung, keine gemessene Groesse.
Das gehoert in die naechste Runde, bevor jemand den Startraum anfasst.

---

## (4) Wie der Port aus einer Raum-Id einen Dateinamen macht — und wer Raum-Ids festhaelt

### Die Namensbildung: 8 Stellen, alle reines `%04X`, kein Charakter-Versatz

| Datei:Zeile | Muster |
|---|---|
| `platform/pc/src/room_pc.c:49` | `"STAGE%u/ROOM%04X.RDT"` (jeder Raumwechsel) |
| `platform/pc/main.c:3115` | `"STAGE%u/ROOM%04X.RDT"` (Boot-Raum) |
| `platform/pc/main.c:3481` | `"RBJ/ROOM%04X.RBJ"` |
| `platform/pc/main.c:4975` | `"MASKS/ROOM%04X.MSK"` |
| `platform/pc/src/bg_pc.c:187/203/211/341` | `"BSS/ROOM%04X/BG%02d.BSS"`, `"…/PRI%02d.TIM"`, `"MASKS/ROOM%04X_PRI%02d.TIM"` |
| `platform/psx/src/re15_room.c:31` | `"\\RDT\\ROOM%04X.RDT;1"` |
| `platform/psx/src/bg_psx.c:194`, `pri_psx.c:116`, `asset_psx.c:468`, `psx/main.c:505` | dieselbe Bauform |

Die Stage ergibt sich aus `(room_id >> 12)`, sonst steht die Id 1:1 im Namen. **Es gibt
keine Stelle, die eine Variante addiert oder abzieht.** Wer die Id auf ungerade setzt,
bekommt automatisch Elzas Dateien — Name, BSS, Maske, RBJ, alles.

### Tuer-Ziele: kommen aus den DATEN und tragen die Variante schon

`aot_common.c:572-575`:

    dest_id = ((dest_stage + 1) << 12) | (dest_room << 4) | (g_current_room_id & 0x000F);

Die Variante wird also **aus dem aktuellen Raum mitgeschleppt**. Gemessen ueber alle
240 RDT (Walk + Opcode-Laengen aus `re15_port/tools/scd_dump_room.py`, das aus
`scd_vm.c s_opcode_sizes` stammt; 34 RDT sind <64-Byte-Stummel und entfallen):

| | Anzahl |
|---|---|
| `Door_aot_set` (0x3B) gesamt | **653** (Leon 328, Elza 325) |
| loesen auf einen existierenden Raum auf | **649** (Leon 326, Elza 323) |
| davon in DERSELBEN Variante | **649 / 649 = 100 %** |
| nicht aufloesbar | 4 (ROOM4030/4031 Slots 1+2 — dieselben 4 Scan-Artefakte, die `aot_common.c:562` schon nennt) |

Die 653/649 decken sich exakt mit der im Port dokumentierten Zahl. **Fuer Tueren ist
nichts zu tun**: sobald man in einem ungeraden Raum steht, bleibt die ganze Kette ungerade.

### Raum-Ids, die im Port-Code FEST stehen

Vollscan ueber `engine/`, `platform/`, `include/` (ohne `re15_room_list.h`), nur Literale,
die in der 240er-Raumliste vorkommen UND in einer Zeile mit Raumbezug stehen, Kommentare
abgezogen: **47 Ids in 20 Dateien, davon 3 ungerade.** Die echten Verdachtsflaechen:

| Ort | was |
|---|---|
| `engine/src/re15_gameflow.c:16` | `#define RE15_NEWGAME_ROOM 0x1240` — **der Startraum, unbedingt** |
| `engine/src/room_common.c:183` | `g_current_room_id = 0x1170` (Startwert) |
| `include/re15_room.h:172` | `#define RE15_BOOT_ROOM 0x1170` |
| `platform/pc/main.c:3738/3777/4429/4910/5781/6951` | Intro-Sonderfaelle `boot_room == 0x1170 / 0x1240` — **alle nur gerade** |
| `engine/src/scd_vm.c:1596`, `include/re15_scd.h:679` | Erzaehler-Raeume `{0x1170, 0x1240}` — **nur gerade** |
| `engine/src/scd_vm.c:2890` | `0x1150 || 0x1151` — **beide Varianten**, so soll es sein |
| `platform/pc/src/audio_pc.c:3317` | Kommentar ueber einen entfernten 0x1170-Hardcode |

**Befund 4: Genau sieben Stellen sind auf gerade Ids festgenagelt** — der Startraum und die
Intro-/Erzaehler-Sonderfaelle um ROOM1170/ROOM1240. Alles andere ist variantenblind oder
fuehrt beide Varianten.

### Die raumbezogenen TABELLEN fuehren Elza bereits

| Tabelle | Leon | Elza |
|---|---|---|
| `engine/src/re15_map_zones.h` (Kartenzonen) | 93 Raeume | **93 Raeume** |
| `engine/src/re15_savepoint.c` (Telefone) | 8 | **8** (`0x1071, 0x1121, 0x1151, 0x2011, 0x30A1, 0x30B1, 0x4011, 0x5011`) |
| `engine/src/re15_itembox.c` | 11 | **8** |
| `include/re15_room_list.h` | 120 | **120** |

Der Hinweis `re15_savepoint.c:21` `[PORT-S] 1071 = Elza-Spiegel des Telefons` ist damit
keine wacklige Annahme mehr, sondern deckt sich mit der Paarungsregel aus (3):
`datei_id(Elza) = datei_id(Leon) + 1`.

---

## (5) Existieren Elzas Daten vollstaendig? — JA, lueckenlos

    STAGE1: gerade(Leon)= 40  ungerade(Elza)= 40   Leon ohne Zwilling: -   Elza ohne Zwilling: -
    STAGE2: gerade(Leon)= 16  ungerade(Elza)= 16   -                       -
    STAGE3: gerade(Leon)= 16  ungerade(Elza)= 16   -                       -
    STAGE4: gerade(Leon)= 16  ungerade(Elza)= 16   -                       -
    STAGE5: gerade(Leon)= 24  ungerade(Elza)= 24   -                       -
    STAGE6: gerade(Leon)=  8  ungerade(Elza)=  8   -                       -
    SUMME : 120 / 120  — kein einziger Raum ohne Gegenstueck
    BSS/  : 78 gerade / 78 ungerade Hintergrundordner

### Spielermodelle in `shared_assets/PSX/PLD/`

| Familie | Dateien | Bemerkung |
|---|---|---|
| PL00 (Leon) | 32 | `PL00.PLD` 189108 B + PLD/EDD/EMR/MD1/TIM + 21 `PL00W**.PLW` + `W01/W03` EDD+EMR |
| PL01 | 1 | nur `PL01.PLD` 190900 B (Leon im roten POLICE-Anzug, s. `main.c:1990`) |
| **PL04 (Elza)** | 28 | `PL04.PLD` 190056 B + `PL04.MD1/.EMR/.TIM` + 21 `PL04W**.PLW` + `W03` EDD+EMR |
| PL05 | 1 | (`ELLIOT.*` liegen separat entpackt) |
| PL08 | 5 | `PL08.PLD` 182928 B + MD1/EMR/EDD/TIM |
| PL02/03/06/07/09/0A..0F | je 1 | nur der `.PLD`-Container |

Was Elza gegenueber Leon **im entpackten Baum** fehlt (die Container selbst sind vollstaendig,
es sind Extraktions-Zwischenstaende):

* `PL04.EDD` — der **Basis-Animationssatz**. Leon hat `PL00.EDD` (24 Clips, 712 Keyframes,
  im Lauf als `[skel] PL00: 15 bones, 24 clips, 712 keyframes` gemessen). Fuer Elza ist nur
  `PL04W03.EDD` (die Pistolen-Variante fuer den Auswahlschirm) entpackt.
* `PL04W01.EDD` / `PL04W01.EMR` — die Messer-Spur. Leon hat beide.

Das sind **drei fehlende Splits aus einem vorhandenen Container**, keine fehlenden Assets.

---

## Was daraus folgt (Liste der Orte, die eine Charakter-Weiche braeuchten)

1. **`g_gameflow.character` traegt die falsche Kodierung.** Entweder das Feld auf die
   Original-Werte {0,4} umstellen (dann stimmen die beiden `& 4`-Tests sofort) oder die
   Tests auf `& 1` ziehen. Das ist eine Entscheidung ueber die Darstellung, keine ueber das
   Verhalten — beide Wege sind byte-true, solange das Ergebnis dem
   `andi v0,v0,0x4` @0x80104000 entspricht.
2. **Das zweite Feld fehlt ganz.** `DAT_800ACA3C` Bit 31 (@0x801024c8 / @0x801024e4) ist
   die Groesse, die im Original die Raumvariante bestimmt (@0x800397e4).
3. **Die Raumwahl braucht genau EINEN Eingriff**, nicht 240: die Variante muss in die
   Start-Raum-Id. Tueren, Dateinamen, Karten, Speicherpunkte und Hintergruende ziehen
   danach von selbst nach (100 % der 649 aufloesbaren Tueren bleiben in ihrer Variante).
4. **Sieben Intro-Sonderfaelle** sind auf `0x1170` / `0x1240` festgenagelt und wuerden in
   Elzas Kette ins Leere laufen.
5. **Das Spielermodell ist hart PL00** (`main.c:3227/3243/3453/3454`), waehrend die
   Waffen-Familie bereits umschaltet — heute eine Mischung, kein Charakter.

---

## Arbeitsspuren

* Laeufe: `re15_port/build/platform/pc/debug.log` (je Lauf ueberschrieben), Abzuege unter
  `analysis/befunde_2026-09-27/elza_shots/`.
* Der Zaehlerlauf aus (3) hat `re15_port/tests/unit/probe_re2z_stomp.c` nur voruebergehend
  angefasst; die Datei ist wieder im Auslieferungsstand.
