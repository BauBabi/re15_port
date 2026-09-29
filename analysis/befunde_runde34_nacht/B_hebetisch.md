# Spur B — Hebetisch Irons Office ROOM1150/1151: Cursor-Bedienung statt Direktoeffnung

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_hebetisch`, Zweig `r34n/hebetisch`.
Stand: in Arbeit (Abschnitte 1 und 3 geschrieben, 2/4/5/6 folgen nach den Messungen an der exe).
Werkzeuge: `re15_port/tools/r34n_b/`, Belege: `analysis/befunde_runde34_nacht/B_belege/`.

## 0 Kurzfassung

(folgt am Schluss)

## 1 Nutzerwortlaut + Lesart

### 1.1 Wortlaut (AUFTRAG.md, zweiter Punkt)

> "Bei Irons Office - ROOM 1150 - möchte ich - beim Modell - nicht das das Modell einfach direkt aufgeht,
> sondern das wir unseren Cursor haben und mit den navigieren können. Wenn man irgendwo hin klickt, wo nichts
> passieren soll - soll der Text kommen "Nothing happened". Wenn man unten rechts auf die Kuppel klickt, die ja,
> danach aufgeht, dann soll der normale Ablauf geschehen. Die Kuppel geht auf, etc. Dafür soll es das Klick
> Geräusch geben, wie eben auch bei den Generator in ROOM 11F0"

### 1.2 Lesart, Satz fuer Satz

| Nutzerwort | Lesart | Grund |
|---|---|---|
| "beim Modell" | das Architekturmodell "Raccoon City 21st Century" auf dem Mitteltisch; im Port der Hebetisch = Prop 0 (Plattform mit Papierstapeln = Hochhaeuser) + Prop 1/2 (Deckelhaelften, Anhaengeform pc[5]=0xC0) | ROOM1150 msg 0 @0x01316 "There's a small sign in one corner. It reads: Raccoon City 21st Century, Hillman-Grayson Architects LLC." haengt am grossen Text-Platz Slot 4 @0x00DBA ueber dem Tisch; Runde 18/30-32 |
| "nicht das das Modell einfach direkt aufgeht" | heute startet die Aktionstaste an der Westseite sofort sub04 (@0x0F96): Deckel auf, Plattform hoch | der Port armiert den Record Slot 1 @0x00D7E (sce 0 -> 3), scd_vm.c op_aot_set sce-0-Zweig |
| "unseren Cursor ... navigieren" | der WELT-Cursor der Cursor-Raetsel, konkret der von ROOM11F0 (Obj 0, gruene Eckwinkel + gelbes Kreuz, D-Pad bewegt ihn, Aktionstaste bestaetigt) | Auftrag des Orchestrators; §3.3 |
| "irgendwo hin klickt, wo nichts passieren soll" | Aktionstaste, waehrend der Cursor NICHT ueber der Kuppel steht | "klicken" = Bestaetigen mit dem Cursor (so benutzt der Nutzer das Wort auch bei 11F0: "nach den Klick eines Schalters", AUFTRAG.md dritter Punkt) |
| Text "Nothing happened" | neuer Untersuchungstext "Nothing happened." — der Satz existiert in keinem der beiden Spiele (§3.5); FORM und GLYPHEN aus RE1.5 belegt | Satzpunkt: 678 von 758 RE1.5-Untersuchungstexten enden auf 0x57 '.' (§3.5) |
| "unten rechts auf die Kuppel ..., die ja danach aufgeht" | die Kuppel = die zwei Deckelhaelften Prop 1/2 (sie gehen in sub04 @0x0FC0 auf); "unten rechts" = ihre Lage in der Kamera, in der sub04 laeuft = Cut 4 | Nachweis am Bild in §2.3 |
| "dann soll der normale Ablauf geschehen. Die Kuppel geht auf, etc." | sub04 UNVERAENDERT ab seinem Anfang (@0x0F96), inklusive Sicherung/Granate in der Ruhe oben | Runde 31/32 |
| "Dafür soll es das Klick Geräusch geben, wie ... bei den Generator in ROOM 11F0" | beim Bestaetigen AUF DER KUPPEL der RE2-Panel-Klick (Gruppe 2 / 0x0A = RE15_PANEL_SE_KLICK), derselbe Aufruf wie beim Schalterdruck in 11F0 | "Dafür" steht im Satz zur Kuppel; scd_vm.c op_sce_key_ck |

### 1.3 Mehrdeutigkeiten und Aufloesung

1. **Klick auch bei "Nothing happened"?** Der Satz mit dem Klick bezieht sich auf die Kuppel ("Dafür"). Vorbild 11F0:
   ein Druck ausserhalb eines Schalters toent NICHT — der Laut haengt dort an drei Bedingungen (Maske 0x0040,
   Praedikat wahr, Cursor ueber einer Zelle), und die 0x0040-Abfragen stehen nur INNERHALB der Zellen-Bloecke
   (`Member_cmp(15 == k)` davor, ROOM11F0 sub01 @0x010FC ff.). Lesart: **kein Klick bei "Nothing happened"**, der Text
   kommt stumm (wie jeder Untersuchungstext). Messung an der exe in §2.
2. **"Nothing happened" mit oder ohne Punkt?** Die Anfuehrungszeichen stehen im deutschen Fliesstext; RE1.5 schliesst
   Untersuchungstexte zu 96 % mit Satzzeichen, zu 89 % mit '.' (Zensus §3.5). -> "Nothing happened." (0x57).
3. **Wie verlaesst man den Cursor, ohne die Kuppel zu druecken?** Der Nutzer sagt nichts dazu. RE1.5 hat in KEINEM der
   13 Cursor-Raeume eine Abbruchtaste (Zensus §3.4) — raus kommt man ueber eine AUSGANGS-Zelle (11F0: gemaltes EXIT,
   Zelle 12 -> sub17) oder ueber die ZIEL-Zelle selbst (2040 Zeitbombe, 5050 Fingerabdruck, 3050 Keycard: genau
   eine Zelle, sonst kein Weg hinaus). Aufloesung in §4/§5 (Bauplan: Ziel-Zelle = einziger Ausgang wie 2040/5050,
   Begruendung dort) und als offener Punkt fuer den Nutzer (§8).
4. **Erneutes Aktivieren** nach Oeffnung/Aufnahme: nicht erwaehnt -> §4.4.
5. **ROOM1150 und ROOM1151:** der Nutzer nennt nur 1150; 1151 ist Elzas Variante desselben Raums (sub04 identisch,
   Record @0x00D7E identisch) — beide bekommen dasselbe (Auftrag §5).

## 2 Ist-Zustand im Port (gemessen)

(folgt — Messungen an der echten exe dieses Baums)

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

Alle SCD-Stellen selbst mit `re15_port/tools/scd_dump_room.py` opcode-exakt gelaufen und an den Rohbytes gelesen.
Datei-Offsets = `re15_port/shared_assets/PSX/STAGE1/ROOM1150.RDT` (194080 B) bzw. ROOM11F0.RDT (152588 B).

### 3.1 Der Ausloeser am Tisch (heute)

`main00 @0x00D7E  2c 01 00 31 00 00 d8 aa e0 b1 dc 05 e4 0c ff 00 18 04 00 00`
= Aot_set Slot 1, **sce 0** (im Original inert, Handler[0] @0x8004305C; der ACTION-Scan ueberspringt sce 0
@0x80042f48-50), flags 0x31, Rechteck Ecke (-21800,-20000) Groesse (1500,3300), Nutzlast `ff 00 18 04` = sub 4.
Identisch in ROOM1151 @0x00D7E. Der Port retypt ihn auf sce 3 (scd_vm.c op_aot_set, sce-0-Zweig,
`re15_aot_retype(slot, 3, …)` = die Wirkung eines Aot_reset LAB_80040738). Davor liegt kein Frage-Text: der Tastendruck
startet sub04 sofort.

Direkt dahinter: `main00 @0x00DBA  2c 04 01 31 00 00 74 aa f8 ad 68 10 50 14 00 00 ff ff 00 00`
= Text-Platz Slot 4, msg 0 ("…Raccoon City 21st Century, Hillman-Grayson Architects LLC."), Rechteck
x -21900..-17700, z -21000..-15800 (umschliesst den ganzen Tisch; Slot 1 liegt in der Scan-Reihenfolge davor).

### 3.2 sub04 — der "normale Ablauf" (@0x0F96..@0x10B6, ROOM1151 um -0x22 verschoben, sonst gleich)

```
0x0F96 22 02 00 01        Set(2,0,1)   = RE15_PAUSE_PLAYER (Zone 2 = DAT_800aca40, game_state.c)
0x0F9A 22 02 02 01        Set(2,2,1)   = RE15_PAUSE_AI
0x0F9E 2e 03 00           Work_set(3,0)  Plattform
0x0FA2 36 02 0a 00 03 00 ..  Se_on(Bank 2 = Raum-snd0, 0x0A)   <- erster Laut der Szene (ROOM1150-EDT [0x0A] = 00 00 45 11)
0x0FAE 09 0a 05 00        Sleep 5
0x0FB2 29 04              Cut_chg 4    <- die Kamera der Szene
0x0FB4 32 00 24 af cf fe cc bb   Pos_set (-20700,-305,-17460)  Plattform aus der Parklage (-20324) in den Tisch
0x0FBC 09 0a 05 00        Sleep 5
0x0FC0 0d 00 18 00 0f 00  For 15 { Work_set(3,1) Speed_set(2,+10) Add_speed ; Work_set(3,2) Speed_set(2,-10) Add_speed ; Evt_next }
                          <- DIE KUPPEL GEHT AUF (je Haelfte 150)
0x0FDE 09 0a 1e 00        Sleep 30
0x0FE2 36 02 0c ..        Se_on(2,0x0C)
0x0FF2 2f 01 f6 ff / 0x0FF6 For 91   Hub 910
0x1000 36 02 0d ..        Se_on(2,0x0D)
0x100C .. 0x1018          Setzen (10 x +1)
0x101A 09 0a 1e 00        Sleep 30     <- Ruhe oben: hier Sicherung + Granate (Port, hebetisch_1150.c)
0x1022 36 02 0a ..  0x102E Sleep 10  0x1032 36 02 0c ..   0x1042 For 90   Abfahrt
0x104C .. 0x1074          Aufsetzen, Sleep 30
0x1078 For 15             Kuppel zu
0x1096 09 0a 3c 00        Sleep 60
0x109A 2e 03 00 / 0x109E 32 00 24 af 00 b1 cc bb   Pos_set (-20700,-20224,-17460) = wieder geparkt
0x10A6 22 05 00 00        Set(5,0,0)   <- !!
0x10AA 22 02 00 00        Set(2,0,0)
0x10AE 22 02 02 00        Set(2,2,0)
0x10B2 2a                 Cut_old
0x10B4 01 00              Evt_end
```

**Indiz fuer einen geplanten Cursor-Schritt:** `Set(5,0,0)` @0x010A6 ist in ROOM1150 der EINZIGE Zugriff auf Bank 5 Bit 0
(Zensus `cursor_zensus.py`: kein Ck(5,0), kein Set(5,0,1) im ganzen Raum; ROOM1151 dasselbe @0x01084). In ROOM11F0 ist
genau dieses Bit der Cursor-Schalter: gesetzt beim Einstieg (sub16 @0x015C2 `22 05 00 01`), abgefragt vor jeder
D-Pad-Abfrage (sub01 @0x01090/@0x010A8/@0x010C0/@0x010D8 `21 05 00 01`), geloescht beim Ausstieg (sub17 @0x0168C,
sub18 @0x016FA) — und der Ausstieg sub17 loescht im selben Zug auch (2,0)/(2,2) (@0x016E8/@0x016EC) und macht `Cut_old`
(@0x016F0), wie sub04 am Ende. Das Ende von sub04 hat damit die Gestalt eines Cursor-Ausstiegs, dessen Einstieg im
Auslieferungsstand fehlt. ⛔ Nur ein Indiz: Bank 5 Bit 0 ist ein raumlokales Allzweckbit (der Raum-SCD-Init
FUN_8003ecec loescht Bank 5 Wort 0, @0x8003ed74 `sw zero,0x800b1028`, Kopf re15_tuer1120.h), und ROOM1011/11B0/20A0
setzen es ebenfalls ohne Cursor. Der Bauplan stuetzt sich darauf NICHT, er benutzt es nur als Namensvorbild.

### 3.3 Der Cursor von ROOM11F0 (Vorbild "unser Cursor")

(Vollstaendige Tabellen: analysis/befunde_2026-09-26/raum11f0-raetsel-cursor.md §2; hier die fuer B tragenden Stellen,
selbst nachgelesen, Dump `B_belege/cursor_zensus.txt`.)

| Teil | Stelle | Bytes | Bedeutung |
|---|---|---|---|
| Einstieg | sub16 @0x015A2/@0x015A8 | `2b 00 ff ff` / `2b 01 ff ff` | zwei Texte, der zweite mit Ja/Nein; @0x015B2 `21 0c 1f 00` Ck(12,31)==0 = "Ja" |
| | @0x015B6 | `46 01 00 00 …` | Aot_reset Slot 1 -> sce 0 (Untersuchen-Zone aus) |
| | @0x015C0 | `29 0a` | Cut_chg 10 (Raetsel-Kamera, Nadir) |
| | @0x015C2..0x015F2 | 13 x `22 05 nn 01` | Bank 5 Bit 0 (Bewegen) + Bit 1..11 (Zellen) + Bit 12 (Pause-Halter) |
| Cursor | sub00 @0x00E54 | `2d 00 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 …` | Obj_model_set obj 0, **Typ 4** (Zeichen-y -900, @0x8002c23c/@0x8002c24c), Lage (-19554,0,22684) |
| Modell | Prop-Tabelle @0x0240 | MD1 @0x001928, TIM @0x018DAC | 1800 x 1800, nur die Oberseite (y=-900) gezeichnet: 4 gruene Eckwinkel + gelbes Kreuz, Schatten in die Textur gebacken |
| Bewegen | sub01 @0x01098/@0x010B0/@0x010C8/@0x010E0 | `51 01 01 00` / `…04…` / `…02…` / `…08…` | Sce_key_ck UP/DOWN/RIGHT/LEFT (virtuelle Maske, GEHALTEN) -> Evt_exec sub02..05 |
| | sub02..05 @0x012F6/0x01302/0x0130E/0x0131A | `2f 02 c8 00` / `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff` | Speed_set Achse 2 (+z) / 2 (-z) / 0 (+x) / 0 (-x), **200 je Bild**, dann Add_speed + Evt_next |
| Zellen | sub00 @0x00D78..@0x00E40 | `2c nn 05 44 00 00 …` | Aot_set sce 5, flags **0x44** (CENTRE + Objekt-Pool), Rechteck 2050 x 2050 |
| Stempel | EXE @0x80042f5c / Clear @0x80043788 | | Objekt+0x0B = Slot der Zelle unter dem Cursor, sonst 0 (aot_common.c re15_object_notch_update) |
| Druecken | sub01 @0x010F0.. | `21 05 01 01` `2e 03 00` `3e 00 0f 00 02 00` `51 01 40 00` `04 ff 18 06` | Zelle frei? Cursor-Objekt; Member_cmp(15 == 2); Sce_key_ck(0x0040 = Aktion); Evt_exec |
| Pause | sub01 @0x012A4..@0x012B0 | `21 05 0c 01` `22 02 00 01` `22 02 02 01` | solange Bit 12: Spieler + KI angehalten (jedes Bild neu) |
| Ausstieg | Zelle 12 (EXIT, gemalt) -> sub17 @0x015FA | `32 00 9e b3 00 00 9c 58` … `46 01 03 31 ff 00 18 10 …` … `2a` `3c 01` | Cursor zurueck auf Startzelle, Untersuchen-Zone wieder an, Bank 5 geloescht, Pause aus, Cut_old + Cut_auto |

**Grenzen:** Das Skript kennt keine. Add_speed (LAB_80040f40, Port scd_vm.c op_add_speed) addiert nur; es gibt keine
Kollision fuer das Cursor-Objekt. Der Cursor kann das Brett also verlassen (Messung an der exe in §2).

**Klick (Port, NUTZER-ENTSCHEIDUNG 2026-09-20/26, RE2-Ergaenzung):** scd_vm.c op_sce_key_ck spielt
`re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK)` genau dann, wenn Maske == 0x0040, Praedikat wahr, Arbeits-Entitaet ein
Objekt mit member_0b != 0, und nur an der Flanke. Die Bank ist `shared_assets/RE2/PANEL2130.EDT/.VH/.VB`
(bytegleicher snd0-Schnitt aus RE2 ROOM2130.RDT @0x0339C/@0x0345C/@0x0407C), geladen beim ersten Aufruf
(audio_pc.c load_re2_panel_se_pc) — **nicht raumgebunden, also auch in ROOM1150 spielbar** (Messung in §2).
RE2-Vorbild: ROOM2130.RDT sub04+0x0082 (@0x01192) `36 02 0a 01 00 00 9b a0 00 fc f4 d3` im Schalt-Zweig.

### 3.4 Zensus der Cursor-Raeume (alle 206 RDTs mit Inhalt, `cursor_zensus.py`, Ausgabe `B_belege/cursor_zensus.txt`)

Raeume mit D-Pad-Abfrage (0x01/0x02/0x04/0x08) + Aktion 0x0040 + Typ-4-Cursor + Raster-Zellen (sce 5, Objekt-Pool):

| Raum (je +Variante) | Zellen | Aktion-Bloecke | Einstieg | Ausstieg |
|---|---|---|---|---|
| 1080 | 4 | 4 | Frage | Ziel-/Ausgangszellen |
| 10D0 | 11 | 11 | Frage | EXIT-Zelle |
| 1100 | 2 | 2 | "Will you use the Minidisc Player w/ Disc?" (msg 3) | Ziel-Zelle (sub02 oeffnet das Schloss) |
| 11E0 | 12 | 12 | Frage | EXIT-Zelle |
| 11F0 | 11 | 11 | msg 0 + msg 1 (Ja/Nein) | EXIT-Zelle 12 -> sub17 |
| 1230 | 11 | 11 | Frage | EXIT-Zelle |
| 2040 | 1 | 1 | "Will you use the Time Bomb?" (msg 5, sub07 @0x016EE) | NUR die Ziel-Zelle Slot 9 -> sub02 |
| 2050 | 3 | 1 | | |
| 2060 | 7 | 6 | | |
| 3050 | 2 | 1 | "Will you use the Red Master Keycard?" (msg 4) | Ziel-Zelle |
| 30E0 | 2 | 2 | | |
| 4020 | 5 | 5 | | |
| 5050 | 1 | 1 | "Will you operate the computer?" (msg 0) | NUR die Ziel-Zelle Slot 2 -> sub03 |

Ergebnis: **keine einzige andere Tastenmaske** in einem Cursor-Raum (die drei "0x4152"-Treffer in 2030/20A0/30C0 liegen
ausserhalb jedes Cursor-Raums). RE1.5 hat also KEINE Abbruchtaste im Cursor; verlassen wird er ueber eine gemalte
Ausgangs-Zelle oder ueber die Ziel-Zelle. Jeder Cursor-Modus beginnt nach einem Ja in einer Frage.
**Kein Raum zeigt einen Text bei Fehldruck** — ausserhalb einer Zelle geschieht im Original nichts.

### 3.5 "Nothing happened" — gibt es den Satz? Glyphen und Form

`msg_suche.py` (RE1.5: 1229 RDT-Nachrichten; RE2: 2315 .msg aus `info/re2leon/PL0/RDT/room*/msg/`; dazu Rohbytes in
RE1.5 PSX.EXE, DEBUG.BIN und RE2 PSX.EXE), Ausgabe `B_belege/nothing_happened_suche.txt`:
**"Nothing happened" existiert in keinem der beiden Spiele.** Die Woerter liegen im RE1.5-Auslieferungsstand vor:

| Wort | Fundstelle (Datei-Offset) | Bytes |
|---|---|---|
| "Nothing" | ROOM1000.RDT msg 0 @0x00D24 ("Nothing unusual.") | `2a 4b 50 44 45 4a 43` |
| " happened" | ROOM3001.RDT msg 6 @0x0199D ("What happened here?") | `00 44 3d 4c 4c 41 4a 41 40` |
| "." | ROOM1000.RDT msg 0 @0x00D33 | `57` |
| Kopf / Ende | ROOM1000.RDT msg 0 @0x00D22 / @0x00D34 | `04 02` / `01 00` |

Form: Kopf `04 02` = Untersuchungstext (wie jede Examine-Meldung), Ende `01 00`. Satzschluss-Zensus
(`satzschluss_zensus.py`, `B_belege/satzschluss_zensus.txt`): 758 Untersuchungstexte, davon 678 (89,4 %) mit 0x57,
24 mit '!', 13 mit '?', 16 mit '"'. -> **`04 02 2a 4b 50 44 45 4a 43 00 44 3d 4c 4c 41 4a 41 40 57 01 00`**
("Nothing happened."). Nachrichten-Id: ROOM1150 hat 15 (Sektion @0x012F8, off[0] = 0x1E), ROOM1151 hat 4
(@0x010EC, off[0] = 0x08) — Id 20 (Vertrag) ist in beiden frei und < 32 (MSG_TABLE_N).
Sprachdatei (optional, Nutzer): `synchro/STAGE1/room1150/main20.wav` und `synchro/STAGE1/room1151/main20.wav`;
Untersuchungstexte sind in RE1.5 unvertont (room1150 hat nur die Dialoge 02, 04..14), ohne Datei laeuft er stumm.

### 3.6 Laute in ROOM1150

ROOM1150-snd0-EDT @0x145E8 (RDT-Kopf +0x08; VH @0x14668, VB @0x15F08): [0x0A] `00 00 45 11`, [0x0B] `00 00 55 11`,
[0x0C] `00 00 66 16`, [0x0D] `00 00 77 16` — belegt (ROOM1151 dieselben Records @0x16660). sub04 spielt 0x0A als ersten
Laut (@0x0FA2) und vor der Abfahrt (@0x1022), 0x0C vor jeder Fahrt, 0x0D beim Anschlag. Zum Vergleich ROOM11F0-EDT
@0x3794: [0x0A]..[0x0D] = `00 00 00 00` (leer) — deshalb fuehrt der Port dort die RE2-Bank.

## 4 Soll-Verhalten (Zeitlinie)

(folgt)

## 5 Bauplan

(folgt)

### 5.1 Konstanten-Tabelle

| Konstante | Wert | Beleg @0x… / Datei-Offset / NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|

## 6 Abnahmeplan

(folgt)

## 7 Risiken, Softlocks, Wechselwirkungen

(folgt)

## 8 Offene Punkte

(folgt)
