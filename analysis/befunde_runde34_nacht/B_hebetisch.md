# Spur B — Hebetisch Irons Office ROOM1150/1151: Cursor-Bedienung statt Direktoeffnung

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_hebetisch`, Zweig `r34n/hebetisch`.
Werkzeuge: `re15_port/tools/r34n_b/`, Belege: `analysis/befunde_runde34_nacht/B_belege/`, grosse Zwischenausgaben
`build/r34n_b/` (nicht versioniert). Alle Messungen an der echten exe dieses Baums (`re15_pc.exe`, Kopie `re15_r34nb.exe`,
beschleunigter Renderer, `RE15_FRAMEDUMP`, `RE15_WINDOW_SCALE=3`, kein AUTOSHOT/SOFTWARE_RENDER).

## 0 Kurzfassung

* **Ansicht:** Cut 4 (RDT-Kameratabelle @0x00E0) — die Kamera, in die sub04 schaltet (`29 04` @0x0FB2). Mit der
  Plattform auf y=-305 (sub04 `Pos_set` @0x0FB4) und geschlossener Kuppel liegt die Kuppel **unten rechts**:
  Deckel+Podest x 151..267, y 149..211 (von 320x240), Mitte (208.6, 176.2). Gemessen am Framedump F240 und
  geometrisch aus den Modellbytes (Deckung 99,53 %). Ohne Plattform zeigt Cut 4 im Hintergrund ein LOCH -> der
  Cursor-Modus braucht den Zustand NACH @0x0FB4.
* **Mechanismus (Plan):** sub04 laeuft wie heute an (Aktion am Tisch -> Slot 1 @0x00D7E -> sub04), wird aber an seiner
  eigenen Deckelfahrt **angehalten**: am `For 15` @0x0FC0 (ROOM1151 @0x0F9E; Signatur in 240 RDTs genau 2x). Waehrend
  des Halts zeigt/steuert der Port den 11F0-Cursor. Druck auf der Kuppel -> Klick (RE2-Panel-SE Gruppe 2 / 0x0A,
  derselbe Aufruf wie beim 11F0-Schalter) + Halt frei -> die Kuppel geht im naechsten Bild auf, sub04 laeuft
  **unveraendert** weiter (Items, Abfahrt, `Cut_old` zurueck in die Raumkamera). Druck daneben -> "Nothing happened."
  (neuer Untersuchungstext, Glyphen/Form aus RE1.5 belegt), KEIN Klick (11F0 toent ausserhalb einer Zelle nicht,
  gemessen).
* **Der Cursor ist der 11F0-Cursor 1:1:** MD1 @0x001928 + TIM @0x018DAC aus ROOM11F0.RDT, Zustand in
  11F0-Weltkoordinaten (Start (-19554,22684) @0x00E54, Schritt 200/Bild @0x012F6/0x01302/0x0130E/0x0131A), Bild ueber
  die Cut-10-Kamera von ROOM11F0 (@0x001A0) -> gleiche Pixel, gleiche Groesse (23,7 x 22,3 px), gleiche
  Geschwindigkeit (2,67 px/Bild), Start in der Bildmitte (160,119), keine Randbegrenzung (wie 11F0, gemessen).
* **Warum angehalten statt vorgeschaltet:** sub04 merkt sich beim eigenen `Cut_chg 4` den ANGEZEIGTEN Cut
  (`g_scd.cam_id_prev = work_vars[0x0A]`, @0x800402c0/@0x800402e4). Zeigte der Port Cut 4 schon vorher, laege dort 4,
  und `Cut_old` @0x10B2 liesse die Kamera nach der Fahrt auf dem Loch-Bild stehen.
* Ressourcen: Nachrichten-Id 20 (frei in 1150 [15 Texte] und 1151 [4]), TIM-Slot `RE15_TIM_SLOT_PROP(8)` = 28;
  KEIN Bank-9-Bit, KEIN AOT-Slot, KEIN g_scd.props-Eintrag noetig.
* Offen fuer den Nutzer (§8): Abbruch ohne Kuppel (RE1.5 hat in keinem Cursor-Raum eine Abbruchtaste).

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
| "nicht das das Modell einfach direkt aufgeht" | heute startet die Aktionstaste an der Westseite sofort sub04 (@0x0F96): 11 Bilder nach dem Druck gehen die Deckel auf (§2.1) | der Port armiert den Record Slot 1 @0x00D7E (sce 0 -> 3), scd_vm.c op_aot_set sce-0-Zweig |
| "unseren Cursor ... navigieren" | der WELT-Cursor der Cursor-Raetsel, konkret der von ROOM11F0 (Obj 0, gruene Eckwinkel + gelbes Kreuz, D-Pad bewegt ihn, Aktionstaste bestaetigt) | Auftrag des Orchestrators; §3.3 |
| "irgendwo hin klickt, wo nichts passieren soll" | Aktionstaste, waehrend der Cursor NICHT ueber der Kuppel steht | "klicken" = Bestaetigen mit dem Cursor (so benutzt der Nutzer das Wort auch bei 11F0: "nach den Klick eines Schalters", AUFTRAG.md dritter Punkt) |
| Text "Nothing happened" | neuer Untersuchungstext "Nothing happened." — der Satz existiert in keinem der beiden Spiele (§3.5); FORM und GLYPHEN aus RE1.5 belegt | Satzpunkt: 678 von 758 RE1.5-Untersuchungstexten enden auf 0x57 '.' (§3.5) |
| "unten rechts auf die Kuppel ..., die ja danach aufgeht" | die Kuppel = die zwei Deckelhaelften Prop 1/2 samt ihrem Podest (sie gehen in sub04 @0x0FC0 auf); "unten rechts" = ihre Lage in der Kamera, in der sub04 laeuft = Cut 4 | am Bild belegt (§2.2): Mitte (208.6,176.2) rechts unter der Bildmitte (160,120) |
| "dann soll der normale Ablauf geschehen. Die Kuppel geht auf, etc." | sub04 UNVERAENDERT, ab der Deckelfahrt @0x0FC0 (davor laeuft es schon, s. §4), inklusive Sicherung/Granate in der Ruhe oben | Runde 31/32 |
| "Dafür soll es das Klick Geräusch geben, wie ... bei den Generator in ROOM 11F0" | beim Bestaetigen AUF DER KUPPEL der RE2-Panel-Klick (Gruppe 2 / 0x0A = RE15_PANEL_SE_KLICK), derselbe Aufruf wie beim Schalterdruck in 11F0 | "Dafür" steht im Satz zur Kuppel; scd_vm.c op_sce_key_ck |

### 1.3 Mehrdeutigkeiten und Aufloesung

1. **Klick auch bei "Nothing happened"?** Der Satz mit dem Klick bezieht sich auf die Kuppel ("Dafür"). Vorbild 11F0,
   GEMESSEN (§2.4, Lauf m5): Druck auf einer Zelle -> Panel-Klick (`[se] Stimme: se=10`), Druck ausserhalb jeder Zelle
   -> kein Laut, kein Text. Lesart: **kein Klick bei "Nothing happened"**; der Text kommt ohne Laut wie jeder
   RE1.5-Untersuchungstext.
2. **"Nothing happened" mit oder ohne Punkt?** Die Anfuehrungszeichen stehen im deutschen Fliesstext; RE1.5 schliesst
   Untersuchungstexte zu 89,4 % mit '.' (Zensus §3.5). -> "Nothing happened." (0x57).
3. **Wie verlaesst man den Cursor, ohne die Kuppel zu druecken?** Der Nutzer sagt nichts dazu. RE1.5 hat in KEINEM der
   13 Cursor-Raeume eine Abbruchtaste (Zensus §3.4); drei Raeume betreten den Cursor sogar OHNE Frage direkt per
   Aktionstaste (1080, 4020, 30E0) und lassen nur ueber Ziel-Zellen hinaus, drei weitere haben genau EINE Zelle
   (2040, 5050, 3050) = einziger Ausgang ist das Ziel. -> Plan: die Kuppel ist der einzige Ausgang (wie 2040/5050).
   Die Frage nach einem Abbruch steht in §8 fuer den Nutzer.
4. **Erneutes Aktivieren** nach Oeffnung/Aufnahme: nicht erwaehnt -> §4.4 (bleibt wie heute wiederholbar).
5. **ROOM1150 und ROOM1151:** der Nutzer nennt nur 1150; 1151 ist Elzas Variante desselben Raums (sub04 identisch um
   -0x22 verschoben, Record @0x00D7E identisch, Cut 4 identisch, Kuppel-Geometrie identisch, §3.7) — beide bekommen
   dasselbe (Auftrag §5).

## 2 Ist-Zustand im Port (gemessen)

Werkzeug `re15_port/tools/r34n_b/lauf.sh` (Vorbild Runde-31-`lauf_fahrt.sh`: Debug-Sprung, Spieler (-21000,-18500),
Eingabeskript auf Spielbild-Zeitachse).

### 2.1 Heute: Aktion am Tisch -> Kuppel geht sofort auf (Lauf m1)

`POS=-21000,-18500,0 SKRIPT=W1,A0.2,W60`, Framedumps F226..F262/2. Aus `debug.log` / `hebetisch.log`:

| Bild | Ereignis | Beleg |
|---|---|---|
| F230 | Aktionstaste (`Tasten 0x8000`) | `[input-script] Tick 30 -> F230` |
| F235 | Plattform noch geparkt y=-20324 | hebetisch.log |
| F236 | `Cut_chg(4)`, Plattform y=-305 | `[scd F236] Cut_chg(4)`, hebetisch.log `F236 y=-305` |
| F238/F240 | Cut 4, Kuppel geschlossen | Bild `B_belege/cut4_cursorzustand_F240_320.png` |
| F242 | Deckel beginnen sich zu oeffnen (For @0x0FC0) | Kontaktbogen `B_belege/m1_cut4_start_bogen.png` |

Kein Cursor, keine Wahl: vom Druck bis zur Deckelfahrt vergehen 11 Bilder.

### 2.2 Die Ansicht: Cut 4, Kuppel unten rechts

* Cut 4 @0x00E0: flag 0, fov 32946 (H = fov>>7 = 257), pos (-21942,-2160,-18378), tgt (-19980,-1566,-18396) — in
  ROOM1150 und ROOM1151 identisch. Die Raumkameras Cut 0..3 haben fov 26684; Cut 4 ist die Nahaufnahme des Tischs.
* Bild F240 (Lauf m1, `B_belege/cut4_cursorzustand_F240_320.png`): Modell im Vordergrund, lavendelfarbene gestreifte
  Achteck-Kuppel unten rechts.
* **Ohne Plattform** (Lauf m4, `RE15_FORCE_CUT=4`, kein Druck): der Hintergrund von Cut 4 hat an der Stelle der
  Plattform ein schwarzes LOCH (gemaltes Modell nur ringsum). Differenz m4/F240 gegen m1/F240: 267 809 Punkte (960x720)
  = die ganze 3D-Plattform. Folgerung: ein Cursor ueber Cut 4 geht nur mit der Plattform an ihrem Platz — also nach
  sub04 `Pos_set` @0x0FB4.
* **Trefferflaeche** (§3.6): Deckel+Podest, konvexe Huelle 12 Ecken, bbox x 151..267 / y 149..211, Mitte (208.6,176.2).
  Bild mit eingezeichneter Flaeche: `B_belege/kuppel_trefferflaeche_cut4_320.png`, Lupe `kuppel_trefferflaeche_lupe.png`
  (rot = Deckel, gelb = Podest, rote Linie = Huelle).

### 2.3 Der 11F0-Cursor im Port (Laeufe m2, m3)

Lauf: Debug-Sprung 11F0, `RE15_FIRE_AOT=1@40#11F0` (Slot 1 = Bedienfeld -> sub16), Eingabe ab F320: 8x (A0.5,W0.1)
Texte + "Ja", danach Bewegung (Vorbild Runde-31-Generatorlauf). Messwerkzeug `cursor_messen.py` (Cursor-Pixel = die
beiden hellen CLUT-Farben der Cursor-Textur).

| Groesse | Messung | Rechnung (Port-Projektion, Cut 10 von ROOM11F0) |
|---|---|---|
| Bbox am Start | x 148.3..172.0, y 107.7..130.0 (23,7 x 22,3 px), Mitte (160.2,118.8) | Oberseite y=-900 im Modell + Typ-4-Anhebung -900 (@0x8002c23c/@0x8002c24c) = Welt-y -1800, Tiefe 17442-1800 = 15642, n = gte_divide(208,15642) = 871: 1800 x 871/65536 = 23,9 px; Mitte x 160 + 74x871/65536 = 161,0, y 120 - 68x871/65536 = 119,1 |
| Schritt | 8 px je Bild bei 960 (Folge 9/6/9), = 2,667 px/Bild bei 320; rechts, runter, hoch, links gleich (m3, `B_belege/11f0_cursor_schritt_m3.txt`) | 200 x 871/65536 = 2,658 px/Bild |
| Grenze | keine: rechts gehalten verlaesst der Cursor ab F570 das Bild (m2, `B_belege/11f0_cursor_rechts_bogen.png`) | Add_speed LAB_80040f40 addiert nur (scd_vm.c op_add_speed) |
| Farben | nur (8,248,0) und (216,208,0) = CLUT[3]/[4] der Cursor-Textur, also NEUTRAL (Tint 128) | TIM @0x018DAC CLUT @VRAM(0,480) |
| D-Pad | UP -> nach oben, DOWN -> unten, RIGHT -> rechts, LEFT -> links | sub02..05 (§3.3) |

### 2.4 Klick in 11F0: nur auf einer Zelle (Lauf m5, Ton an, `RE15_SE_DEBUG=1`)

`B_belege/11f0_klick_m5.txt`:
* F500 Aktion auf der Startzelle (Slot 9) -> `[se] Stimme: se=10 layer=0 vag=9 note=66 ...` = der Panel-Klick
  (RE2-Bank, Satz 0x0A).
* F577 Aktion ausserhalb jeder Zelle (Cursor bei x 197 -> Welt-x -16844, alle Zellen enden bei -17650 bzw. EXIT beginnt
  bei -15300; Pad-Sperre aus: `panelsperre=0`, `msg=0`) -> KEIN `se=10`, kein Text. Im ganzen Lauf genau 1x `se=10`.

### 2.5 Klickton in ROOM1150 spielbar?

Ja, statisch belegt: `re15_audio_re2_panel_se` laedt die Bank beim ersten Aufruf ueber
`re15_pc_read_re2("PANEL2130.EDT/.VH/.VB")` (audio_pc.c load_re2_panel_se_pc) — ohne jede Raumbedingung; die drei
Dateien liegen unter `re15_port/shared_assets/RE2/` (192 / 3104 / 41744 B). Der erste Klick in ROOM1150 laedt die Bank
nach. (Dynamische Messung erst mit dem Bau moeglich: `RE15_SE_DEBUG`-Zeile `se=10` beim Kuppeldruck, §6.)

### 2.6 Folgerungen fuer den Bau aus den Messungen

1. Die Kuppel liegt unten rechts nur in Cut 4 und nur mit der Plattform bei -305 -> der Cursor gehoert in den Zustand
   zwischen sub04 @0x0FB4 und @0x0FC0.
2. Der 11F0-Cursor ist im Port neutral gefaerbt, 23,9 px gross, 2,66 px/Bild schnell, unbegrenzt — genau das ist
   "unser Cursor".
3. Ausserhalb eines Ziels: kein Laut (11F0).

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

Alle SCD-Stellen selbst mit `re15_port/tools/scd_dump_room.py` opcode-exakt gelaufen und an den Rohbytes gelesen.
Datei-Offsets = `re15_port/shared_assets/PSX/STAGE1/ROOM1150.RDT` (194080 B) bzw. ROOM11F0.RDT (152588 B).

### 3.1 Der Ausloeser am Tisch (heute)

`main00 @0x00D7E  2c 01 00 31 00 00 d8 aa e0 b1 dc 05 e4 0c ff 00 18 04 00 00`
= Aot_set Slot 1, **sce 0** (im Original inert, Handler[0] @0x8004305C; der ACTION-Scan ueberspringt sce 0
@0x80042f48-50), flags 0x31, Rechteck Ecke (-21800,-20000) Groesse (1500,3300), Nutzlast `ff 00 18 04` = sub 4.
Identisch in ROOM1151 @0x00D7E. Der Port retypt ihn auf sce 3 (scd_vm.c op_aot_set, sce-0-Zweig,
`re15_aot_retype(slot, 3, …)` = die Wirkung eines Aot_reset LAB_80040738). Die Aktion landet ueber den ACTION-Scan
(aot_common.c, `default:` -> `g_aot.fired_event_id_this_frame = 4`) in game_step_common.c
(`scd_event_fire(g_aot.fired_event_id_this_frame)`). Die Mess-Haken `RE15_FIRE_AOT` gehen dagegen ueber
`re15_aot_fire_slot` -> `scd_event_fire(a->event_id)` direkt (aot_common.c, GENERIC-Zweig). `ff 00 18 04` kommt in
ROOM1150 nur an dieser einen Stelle vor -> sub04 hat genau einen Ausloeser.

Direkt dahinter: `main00 @0x00DBA  2c 04 01 31 00 00 74 aa f8 ad 68 10 50 14 00 00 ff ff 00 00`
= Text-Platz Slot 4, msg 0 (das Schild am Modell), Rechteck x -21900..-17700, z -21000..-15800.

### 3.2 sub04 — der "normale Ablauf" (@0x0F96..@0x10B6, ROOM1151 um -0x22 verschoben, sonst gleich)

```
0x0F96 22 02 00 01        Set(2,0,1)   = RE15_PAUSE_PLAYER (Zone 2 = DAT_800aca40, game_state.c)
0x0F9A 22 02 02 01        Set(2,2,1)   = RE15_PAUSE_AI
0x0F9E 2e 03 00           Work_set(3,0)  Plattform
0x0FA2 36 02 0a 00 03 00 ..  Se_on(Bank 2 = Raum-snd0, 0x0A)   <- erster Laut der Szene (ROOM1150-EDT [0x0A] = 00 00 45 11)
0x0FAE 09 0a 05 00        Sleep 5
0x0FB2 29 04              Cut_chg 4    <- merkt den ANGEZEIGTEN Cut fuer Cut_old (s.u.)
0x0FB4 32 00 24 af cf fe cc bb   Pos_set (-20700,-305,-17460)  Plattform aus der Parklage (-20324) in den Tisch
0x0FBC 09 0a 05 00        Sleep 5
0x0FC0 0d 00 18 00 0f 00  For 15 { Work_set(3,1) Speed_set(2,+10) Add_speed ; Work_set(3,2) Speed_set(2,-10) Add_speed ; Evt_next }
                          <- DIE KUPPEL GEHT AUF (je Haelfte 150)          == geplante HALTE-STELLE
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
0x10A6 22 05 00 00        Set(5,0,0)
0x10AA 22 02 00 00        Set(2,0,0)
0x10AE 22 02 02 00        Set(2,2,0)
0x10B2 2a                 Cut_old      <- zurueck in den bei @0x0FB2 gemerkten Cut
0x10B4 01 00              Evt_end
```

**Cut_chg / Cut_old (Port byte-true, scd_vm.c op_cut_chg / op_cut_old):** Cut_chg LAB_800402a0 merkt
`DAT_800b3f7b = DAT_800b0fe4` (@0x800402c0 `lbu` / @0x800402e4 `sb`) — den ANGEZEIGTEN Cut (work_vars[0x0A]); Cut_old
FUN_8004032c liest ihn zurueck (@0x8004033c). Laeuft sub04 aus der Raumkamera an, merkt @0x0FB2 die Raumkamera und
@0x10B2 kehrt dorthin zurueck. **Zeigt vorher schon etwas anderes Cut 4 an, merkt @0x0FB2 die 4 — und @0x10B2 bleibt
auf Cut 4 stehen, mit geparkter Plattform = das Loch-Bild (§2.2).** Das entscheidet den Bauweg (§5.1).

**Indiz fuer einen geplanten Cursor-Schritt:** `Set(5,0,0)` @0x010A6 ist in ROOM1150 der EINZIGE Zugriff auf Bank 5 Bit 0
(Zensus `cursor_zensus.py`: kein Ck(5,0), kein Set(5,0,1) im ganzen Raum; ROOM1151 dasselbe @0x01084). In ROOM11F0 ist
genau dieses Bit der Cursor-Schalter: gesetzt beim Einstieg (sub16 @0x015C2 `22 05 00 01`), abgefragt vor jeder
D-Pad-Abfrage (sub01 @0x01090/@0x010A8/@0x010C0/@0x010D8 `21 05 00 01`), geloescht beim Ausstieg (sub17 @0x0168C,
sub18 @0x016FA) — und der Ausstieg sub17 loescht im selben Zug auch (2,0)/(2,2) (@0x016E8/@0x016EC) und macht `Cut_old`
(@0x016F0), wie sub04 am Ende. ⛔ Nur ein Indiz: Bank 5 Bit 0 ist ein raumlokales Allzweckbit (der Raum-SCD-Init
FUN_8003ecec loescht Bank 5 Wort 0, @0x8003ed74 `sw zero,0x800b1028`, Kopf re15_tuer1120.h), ROOM1011/11B0/20A0 setzen
es ohne Cursor, und 10D0/1230/11E0 benutzen es in jedem Schalter-Sub als Bewegungssperre. Der Bauplan stuetzt sich
darauf NICHT.

### 3.3 Der Cursor von ROOM11F0 (Vorbild "unser Cursor")

(Vollstaendige Tabellen: analysis/befunde_2026-09-26/raum11f0-raetsel-cursor.md §2; hier die fuer B tragenden Stellen,
selbst nachgelesen, Dumps `B_belege/cursor_zensus.txt`, `cursor_einstieg.txt`.)

| Teil | Stelle | Bytes | Bedeutung |
|---|---|---|---|
| Einstieg | sub16 @0x015A2/@0x015A8 | `2b 00 ff ff` / `2b 01 ff ff` | zwei Texte, der zweite mit Ja/Nein; @0x015B2 `21 0c 1f 00` Ck(12,31)==0 = "Ja" |
| | @0x015B6 | `46 01 00 00 …` | Aot_reset Slot 1 -> sce 0 (Untersuchen-Zone aus) |
| | @0x015C0 | `29 0a` | Cut_chg 10 (Raetsel-Kamera, Nadir) |
| | @0x015C2..0x015F2 | 13 x `22 05 nn 01` | Bank 5 Bit 0 (Bewegen) + Bit 1..11 (Zellen) + Bit 12 (Pause-Halter) |
| Cursor | sub00 @0x00E54 | `2d 00 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 …` | Obj_model_set obj 0, **Typ 4**, Lage (-19554,0,22684) |
| Modell | Prop-Tabelle @0x0240 | MD1 @0x001928 (5556 B), TIM @0x018DAC (33312 B) | 1 Mesh: 132 Punkte, 98 Vierecke, 16 Dreiecke; Oberseite = 49 Vierecke bei y=-900, x -940..932, z -932..900, UV u 1..127 / v 0..82; TIM 8bpp 128x256, CLUT 256 @VRAM(0,480): [1] (0,88,0) [2] (64,56,0) [3] (8,248,0) [4] (216,208,0) [5] (240,248,136); Index 0 = Farbschluessel (transparent) |
| Kamera | Kameratabelle Eintrag 10 @0x001A0 | | flag 0, fov 26684 (H 208), pos (-19628,-17442,22616), tgt (-19628,15928,22617) = Nadir |
| Bewegen | sub01 @0x01098/@0x010B0/@0x010C8/@0x010E0 | `51 01 01 00` / `…04…` / `…02…` / `…08…` | Sce_key_ck UP/DOWN/RIGHT/LEFT (virtuelle Maske, GEHALTEN = DAT_800ac768, LAB_80042920) -> Evt_exec sub02..05 |
| | sub02..05 @0x012F6/0x01302/0x0130E/0x0131A | `2f 02 c8 00` / `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff` | Speed_set Achse 2 (+z) / 2 (-z) / 0 (+x) / 0 (-x), **200 je Bild**, dann Add_speed + Evt_next |
| Zellen | sub00 @0x00D78..@0x00E40 | `2c nn 05 44 00 00 …` | Aot_set sce 5, flags **0x44** (CENTRE + Objekt-Pool), Rechteck 2050 x 2050 |
| Stempel | EXE @0x80042f5c / Clear @0x80043788 | | Objekt+0x0B = Slot der Zelle unter der OBJEKTLAGE (x,z), sonst 0 |
| Druecken | sub01 @0x010F0.. | `21 05 01 01` `2e 03 00` `3e 00 0f 00 02 00` `51 01 40 00` `04 ff 18 06` | Zelle frei? Cursor-Objekt; Member_cmp(15 == 2); Sce_key_ck(0x0040 = Aktion); Evt_exec |
| Pause | sub01 @0x012A4..@0x012B0 | `21 05 0c 01` `22 02 00 01` `22 02 02 01` | solange Bit 12: Spieler + KI angehalten (jedes Bild neu) |
| Ausstieg | Zelle 12 (EXIT, gemalt) -> sub17 @0x015FA | `32 00 9e b3 00 00 9c 58` … `46 01 03 31 ff 00 18 10 …` … `2a` `3c 01` | Cursor zurueck auf Start, Untersuchen-Zone wieder an, Bank 5 geloescht, Pause aus, Cut_old + Cut_auto |

**Virtuelle Tasten (pad_common.c re15_pad_virtual_word, Preset-Tabelle @0x80073dbc):** 0x0001 UP, 0x0002 RIGHT,
0x0004 DOWN, 0x0008 LEFT, 0x0040/0x0080 <- SQUARE (Aktion), 0x4000 <- SQUARE (Bestaetigen), 0x8000 <- CROSS (Abbrechen,
@0x80073dbc[15]). Unter der Pad-Sperre (Bit 0x01000000, @0x800304f4-@0x8003051c) bleiben nur 0xf000 uebrig.

**Grenzen:** Das Skript kennt keine. Add_speed (LAB_80040f40, Port scd_vm.c op_add_speed) addiert nur; es gibt keine
Kollision fuer das Cursor-Objekt (gemessen §2.3).

**Klick (Port, NUTZER-ENTSCHEIDUNG 2026-09-20/26, RE2-Ergaenzung):** scd_vm.c op_sce_key_ck spielt
`re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK)` genau dann, wenn Maske == 0x0040, Praedikat wahr, Arbeits-Entitaet ein
Objekt mit member_0b != 0, und nur an der Flanke. RE2-Vorbild: ROOM2130.RDT sub04+0x0082 (@0x01192)
`36 02 0a 01 00 00 9b a0 00 fc f4 d3` im Schalt-Zweig. Bank: `shared_assets/RE2/PANEL2130.*` (§2.5).

### 3.4 Zensus der Cursor-Raeume (alle RDTs, `cursor_zensus.py` + `cursor_einstieg.py`)

Raeume mit D-Pad-Abfrage (0x01/0x02/0x04/0x08) + Aktion 0x0040 + Typ-4-Cursor (je + Variante):

| Raum | Aktions-Zellen | Einstieg (Sub, das Bank5-Bit0 setzt) | Ausstieg |
|---|---|---|---|
| 1080 | 4 | **direkt**, keine Frage: sub06 @0x006C6 `29 01` + @0x006C8 Set(5,0,1) (Aufzugspanel, Slot 3 sce 3) | nur Ziel-Zellen (Etagen sub07..09) |
| 4020 | 5 | **direkt**: sub06 @0x008D2 Cut 4 + @0x008D4 Set(5,0,1) | Ziel-Zellen sub07..10, 12 |
| 30E0 | 2 | **direkt**: sub09 @0x01038 Cut 2 + @0x0103A Set(5,0,1) | Zelle 5 -> sub04 (loescht) |
| 10D0 | 11 | nach Text msg0 @0x018E2, sub17 @0x018EC | EXIT-Zelle 13 -> sub16 |
| 11E0 | 12 | nach Text msg0 @0x01ECA, sub17 @0x01ED4 | Zelle 18 -> sub16 |
| 1230 | 11 | nach Text msg0 @0x013AA, sub17 @0x013B4 | Zelle 17 -> sub16 |
| 11F0 | 11 | nach Ja: sub16 Ck(12,31,0) @0x015B2 | EXIT-Zelle 12 -> sub17 |
| 1100 | 2 | nach Ja ("Will you use the Minidisc Player w/ Disc?"), sub07 @0x00D60 | Zelle 5 -> sub02 (Schloss auf) |
| 2060 | 6 | nach Ja, sub12 @0x015C4 | sub13 |
| 2040 | **1** | nach Ja ("Will you use the Time Bomb?" msg 5), sub07 @0x016F8 | NUR Ziel-Zelle Slot 9 -> sub02 |
| 5050 | **1** | nach Ja ("Will you operate the computer?" msg 0), sub02 @0x00878 | NUR Ziel-Zelle Slot 2 -> sub03 |
| 3050 | **1** | nach Ja ("Will you use the Red Master Keycard?" msg 4), sub10 @0x02588 | NUR Ziel-Zelle Slot 11 -> sub15 |

Ergebnis: **keine einzige andere Tastenmaske** in einem Cursor-Raum (die drei "0x4152"-Treffer in 2030/20A0/30C0 liegen
ausserhalb jedes Cursor-Raums). RE1.5 hat also KEINE Abbruchtaste im Cursor. **Kein Raum zeigt einen Text bei
Fehldruck.** Direkter Einstieg ohne Frage ist belegt (1080/4020/30E0), Ziel-Zelle als einziger Ausgang ebenfalls
(2040/5050/3050).

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

Form: Kopf `04 02` = Untersuchungstext, Ende `01 00`. Satzschluss-Zensus (`satzschluss_zensus.py`,
`B_belege/satzschluss_zensus.txt`): 758 Untersuchungstexte, 678 (89,4 %) mit 0x57, 24 '!', 13 '?', 16 '"'.
-> **`04 02 2a 4b 50 44 45 4a 43 00 44 3d 4c 4c 41 4a 41 40 57 01 00`** ("Nothing happened.", 17 Glyphen,
**123 px** nach der Vorschubtabelle include/font_width.h = DEBUG.BIN[0x4416+code]; "Nothing unusual." misst 111 px —
eine Zeile, kein Umbruch). Nachrichten-Id: ROOM1150 hat 15 (Sektion @0x012F8, off[0] = 0x1E), ROOM1151 hat 4
(@0x010EC, off[0] = 0x08) — Id 20 (Vertrag) ist in beiden frei und < 32 (MSG_TABLE_N). Sprachdatei (optional,
Nutzer): `synchro/STAGE1/room1150/main20.wav` und `synchro/STAGE1/room1151/main20.wav`; Untersuchungstexte sind in
RE1.5 unvertont (room1150 hat nur die Dialoge 02, 04..14), ohne Datei laeuft er stumm.

### 3.6 Trefferflaeche der Kuppel (`kuppel_flaeche.py`, `B_belege/kuppel_flaeche_1150.txt` / `_1151.txt`)

Projektion der ausgelieferten Geometrie mit der Port-Mathematik (r30_lib.py = camera_common.c + main.c-Prop-Projektion,
in Runde 31/32 gegen Framedumps auf < 1 px geprueft), Cursor-Zustand: Plattform (-20700,-305,-17460) rot_y 2048
(@0x0FB4, @0x0E00), Deckel geschlossen (lokal 0, @0x0E22/@0x0E44), Kamera Cut 4 @0x00E0.

| Teil | Quelle | Flaechen | bbox (320x240) |
|---|---|---|---|
| Deckel | Prop 1 MD1 @0x138D4 + Prop 2 MD1 @0x13B88 (1151: @0x1594C/@0x15C00) | 20 | x 151.0..267.3, y 149.0..188.3 |
| Podest | Prop 0 MD1 @0x11E40 (1151 @0x13EB8), Flaechen mit allen Punkten in y[-1036,-886], x[-490,-70], z[870,1650] (Podest-Masse Runde 31 §0/§1.1) | 11 | x 151.0..267.3, y 158.0..211.3 |

**Konvexe Huelle (320x240, 12 Ecken):** (151,171) (163,161) (180,151) (206,149) (234,151) (248,157) (267,171) (264,191)
(261,205) (214,211) (164,205) (151,191); Eckenmittel (208.6,176.2). ROOM1151 Zahl fuer Zahl gleich.
**Gegenprobe am gerenderten Bild:** von 53 020 projizierten Kuppel-Pixeln (960x720) liegen 52 771 = 99,53 % in der
Differenzmaske "Plattform da" (m1 F240) gegen "Plattform nicht da" (m4 F240); die 249 anderen sind Randpixel.
Sichtbild: `B_belege/kuppel_trefferflaeche_cut4_320.png` und `kuppel_trefferflaeche_lupe.png`.

### 3.7 Halte-Stelle in sub04 (`halt_signatur.py`, `B_belege/halt_signatur.txt`)

Signatur `29 04 | 32 00 24 af cf fe cc bb | 09 0a 05 00 | 0d 00 18 00 0f 00` (Cut_chg 4, Pos_set, Sleep 5, For 15) —
in allen 240 RDTs genau 2 Treffer: ROOM1150 @0x0FB2 -> For @0x0FC0, ROOM1151 @0x0F90 -> For @0x0F9E. Die
Ruhe-Signatur von hebetisch_1150.c (Fenster [0x101A,0x1042) bzw. [0x0FF8,0x1020)) liegt im selben Sub dahinter.
Port-Semantik op_for (scd_vm.c, LAB_8003f540): liest Blocklaenge/Zaehler, legt den Schleifenrahmen an, PC += 6 — ein
Halt VOR diesem Opcode (Rueckgabe Yield, PC unveraendert) laesst den Thread ohne jede Zustandsaenderung stehen.
Vorbild fuer einen solchen PC-Halt im Port: `re15_panel_zeiger_abnahme_haelt(t->pc, raw)` in op_evt_exec (Runde 31).

### 3.8 Laute in ROOM1150

ROOM1150-snd0-EDT @0x145E8 (RDT-Kopf +0x08; VH @0x14668, VB @0x15F08): [0x0A] `00 00 45 11`, [0x0B] `00 00 55 11`,
[0x0C] `00 00 66 16`, [0x0D] `00 00 77 16` — belegt (ROOM1151 dieselben Records @0x16660). sub04 spielt 0x0A als ersten
Laut (@0x0FA2) und vor der Abfahrt (@0x1022), 0x0C vor jeder Fahrt, 0x0D beim Anschlag. Zum Vergleich ROOM11F0-EDT
@0x3794: [0x0A]..[0x0D] = `00 00 00 00` (leer) — deshalb fuehrt der Port dort die RE2-Bank.

## 4 Soll-Verhalten (Zeitlinie)

Bildnummern relativ zum Druck am Tisch (F0), abgeleitet aus Lauf m1 (§2.1); alles bis F11 ist heutiges sub04.

| Bild | Was der Spieler sieht/hoert | Mechanik |
|---|---|---|
| F0 | Aktion an der Westseite des Tischs | ACTION-Scan Slot 1 -> Ereignis 4; **Haken:** das B-Modul merkt "Cursor verlangt" |
| F0..F5 | Raumkamera, Laut ROOM1150-0x0A | sub04 @0x0F96..@0x0FAE unveraendert (Pause Spieler+KI, Se_on, Sleep 5) |
| F6 | Schnitt auf Cut 4, Modell im Tisch, Kuppel zu | @0x0FB2 Cut_chg 4 (merkt die RAUMkamera), @0x0FB4 Pos_set |
| F6..F10 | Standbild | @0x0FBC Sleep 5 |
| F11.. | **der 11F0-Cursor erscheint in der Bildmitte (161,119)**; D-Pad bewegt ihn 200 11F0-Einheiten = 2,66 px je Bild, diagonal moeglich (vier Abfragen unabhaengig wie sub01), keine Randgrenze | sub04 steht VOR `For` @0x0FC0 (Halt), Spieler + KI bleiben angehalten (sub04 @0x0F96/@0x0F9A) |
| Druck daneben | "Nothing happened." (eine Zeile, kein Laut); Welt steht, Cursor steht; nach dem Wegdruecken wieder frei | Text-Id 20, Pausemaske 0xFFFF0000 wie alle 524 sce-1-Texte |
| Druck auf der Kuppel (Bild K) | Panel-Klick; Cursor weg | Klick = derselbe Aufruf wie 11F0; Halt frei |
| K+1.. | Kuppel geht auf (15 Bilder), Plattform faehrt hoch, Ruhe oben: Sicherung/Granate wie bisher, Abfahrt, Kuppel zu, Parken | sub04 ab @0x0FC0 unveraendert |
| Ende | zurueck in die Raumkamera, Spieler frei | @0x10AA/@0x10AE Pause aus, @0x10B2 Cut_old = gemerkte Raumkamera |

### 4.1 Laden/Speichern

Der Cursor-Modus ist fluechtig (kein Bank-9-Bit, kein Speicherstandfeld): ein Speichern ist waehrend des Modus
unmoeglich (Spieler angehalten, Speichern braucht die Memory-Card-Schreibmaschine/Telefon), ein Raumwechsel ebenso.
Nach dem Laden in ROOM1150/1151 ist der Modus aus (Modul-Zustand wird beim Raumaufbau zurueckgesetzt), die Tisch-Items
haengen weiter an (9,53)/(9,56)/(9,54)/(9,55) — unveraendert.

### 4.2 Sicherung und Granate

Unveraendert: sie liegen seit Runde 32 in den Faechern unter der Tischplatte (in Cut 4 im Cursor-Zustand vom gemalten
Tisch verdeckt, Runde 32 §4.2: 0 Punkte bis y=-515) und gehen in der Ruhe oben auf (Fenster @0x101A..@0x1042). Der Halt
liegt davor und aendert keinen ihrer Zeitpunkte relativ zu sub04.

### 4.3 Mess-Haken bleiben alt

`RE15_FIRE_AOT=1@…` (re15_aot_fire_slot) und direkte `scd_event_fire(4)` in Sonden starten sub04 OHNE "Cursor
verlangt" -> kein Halt, die Fahrt laeuft wie heute durch. Damit bleiben integration_r30_sicherung_laden,
integration_r30_granate_laden (beide `RE15_FIRE_AOT=1@90`) sowie unit_r31_hebetisch, unit_r30_granate,
unit_r30_sicherung_nein, unit_r32_hebetisch_faecher gueltig. Der Nutzerweg (Aktionstaste) ist der einzige, der den
Cursor bringt — und nur ihn gibt es im Spiel.

### 4.4 Erneutes Aktivieren

Wie heute beliebig oft: sub04 setzt Slot 1 nie zurueck (kein Aot_reset im Sub), der Port armiert ihn bei jedem
Raumaufbau. Jede Aktion am Tisch bringt wieder den Cursor; nach der Aufnahme bleiben die Faecher leer (Flags 53/56).
Begruendung: das Original legt fuer sub04 keinen "erledigt"-Zustand an, und der Nutzer hat keinen verlangt.

## 5 Bauplan

### 5.1 Grundsatzentscheidungen (mit Grund)

1. **Halt in sub04 statt Vorschalt-Modus.** Ein vom Port vorgeschalteter Cursor muesste Cut 4 selbst zeigen -> sub04s
   `Cut_chg 4` @0x0FB2 merkte dann die 4 (LAB_800402a0 @0x800402c0/@0x800402e4), `Cut_old` @0x10B2 kaeme nie zurueck in
   die Raumkamera, und der Port muesste `cam_id_prev` nachtraeglich faelschen. Mit dem Halt laufen Kamera, Plattform,
   Pause und Laute aus den Original-Bytes; der Port fuegt nur eine Wartezeit vor @0x0FC0 ein.
2. **Der Cursor ist der 11F0-Cursor, abgebildet ueber die Cut-10-Kamera von ROOM11F0 und ueber das Cut-4-Bild gelegt.**
   Ein Welt-Cursor auf der Tischplatte (11F0-Mechanik woertlich, xz-Ebene) wuerde von der Cut-4-Kamera (Neigung
   atan(594/1962) = 16,8 Grad) auf ~29 % Hoehe gestaucht, seine Winkel waeren unlesbar, und seine Groesse (1800) waere in
   ~1800 Tiefe 260 px breit. Nur die 11F0-Abbildung liefert "unseren Cursor": Groesse 23,9 px, 2,66 px/Bild,
   D-Pad = Bildrichtung, neutrale Farben (§2.3).
3. **Treffertest im Bild**: Heisspunkt = Projektion der 11F0-Objektlage (x, -1800, z) = das Kreuz (Bezugspunkt wie der
   Zellstempel @0x80042f5c, der die OBJEKTLAGE prueft), gegen die konvexe Huelle der Kuppel (§3.6).
4. **Flanke statt Halten fuer den Druck.** 11F0 fragt 0x0040 GEHALTEN ab und verhindert Wiederholung ueber das
   Zellenbit (sub06 @0x01322 Set(5,1,0)). Hier gibt es zwei Ausgaenge (Text oder Fahrt); nach dem Text laege die Taste
   ggf. noch -> Flanke `g_scd_pad_edge & 0x0040` (DAT_800ac76c, wie der ACTION-Scan der Spielerbefehle: aot_common.c
   Kommentar @0x80073f90 "DAT_800ac76c & 0x80 = virtual edge"). Bewegen weiter GEHALTEN (`g_scd_pad_held`, DAT_800ac768,
   wie Sce_key_ck LAB_80042920).
5. **Nur der Spielerweg armiert.** Haken an der GENERIC-Ausgabe in game_step_common.c, nicht in scd_event_fire (§4.3).

### 5.2 Dateien

| Datei | neu/Haken | Inhalt |
|---|---|---|
| `re15_port/include/re15_hebetisch_cursor.h` | neu | Kopf mit allen Belegen (Abschnitte 3/5 dieses Dossiers), API unten |
| `re15_port/engine/src/hebetisch_cursor_1150.c` | neu | Zustand, Halt, Bewegung, Treffertest, Klick, Text |
| `re15_port/platform/pc/src/hebetisch_cursor_pc.c` | neu (B-eigen, PC) | Zeichnen: 11F0-MD1 mit Cut-10-Sicht projizieren, Dreiecke in die Tri-Queue, TIM-Slot 28 |
| `re15_port/engine/src/gen/hebetisch_cursor.inc` | neu, generiert | MD1 (5556 B) + TIM (33312 B) bytegleich aus ROOM11F0.RDT @0x001928/@0x018DAC |
| `re15_port/tools/r34n_b/cursor_export.py` | neu | erzeugt das .inc, prueft Bytegleichheit (Vorbild tools/irons_tisch_engine_export.py) |
| `re15_port/engine/src/scd_vm.c` | Haken 1 Zeile | op_for: `if (s_current_rdt && re15_hebetisch_cursor_haelt(t->pc, s_current_rdt->raw)) return SCD_R_YIELD;` vor jeder Zustandsaenderung |
| `re15_port/engine/src/game_step_common.c` | Haken 2 Zeilen | (a) GENERIC-Ausgabe: `re15_hebetisch_cursor_aktion(g_aot.fired_event_id_this_frame);` vor `scd_event_fire(...)`; (b) neben `re15_granate_tick()`: `if (c->rdt_ok) re15_hebetisch_cursor_tick();` |
| `re15_port/engine/src/scd_room_setup.c` | Haken 1 Zeile | neben `re15_granate_install(...)`: `re15_hebetisch_cursor_install((uint16_t)g_current_room_id);` (Zustand zuruecksetzen, Signatur suchen) |
| `re15_port/platform/pc/main.c` | Haken ~10 Zeilen | (a) in `pc_load_room_prop_set` nach dem Granaten-Block: Cursor-TIM in `RE15_TIM_SLOT_PROP(8)` laden (nur 1150/1151, nur wenn `nprops <= 8`); (b) nach der Prop-Zeichenschleife: `re15_hebetisch_cursor_zeichnen_pc();` |
| `re15_port/tests/unit/probe_r34n_b_*.c` + `probes/r34n_b_hebetisch.cmake` | neu | Riegel §6 |
| `re15_port/tests/integration/test_r34n_b_*.cmake` | neu | exe-Laeufe §6 |

PSX-Ziel: wie Sicherung/Granate kein Lader fuer Zusatz-Props; `hebetisch_cursor_pc.c` gibt es dort nicht -> auf PSX
halten wir sub04 NICHT an (Modul fragt `RE15_PLATFORM_PC`), sonst stuende die Fahrt ohne sichtbaren Cursor.

### 5.3 API und Ablauf im Modul

```c
void re15_hebetisch_cursor_install(uint16_t room);        /* Raumaufbau: Zustand 0, Halte-Signatur suchen (§3.7) */
void re15_hebetisch_cursor_aktion(uint8_t ereignis);      /* GENERIC-Ausgabe: 1150/1151 && ereignis == 4 -> VERLANGT */
int  re15_hebetisch_cursor_haelt(const uint8_t *pc, const uint8_t *raw); /* op_for: 1 = Yield */
void re15_hebetisch_cursor_tick(void);                    /* je Spielbild */
int  re15_hebetisch_cursor_sicht(int32_t *x11f0, int32_t *z11f0); /* fuers Zeichnen: 1 = zeigen */
```

Zustaende: `AUS` -> (aktion 4) `VERLANGT` -> (op_for am Halt-PC) `AKTIV` -> (Kuppeldruck) `FREI` -> (sub04-PC hinter
@0x0FC0 bzw. Thread weg) `AUS`. `haelt()` liefert 1 genau bei (VERLANGT|AKTIV) && pc == Halt-PC; der erste solche
Aufruf setzt AKTIV und den Cursor auf den Start. `tick()` nur in AKTIV und nur wenn weder Text offen noch SCD-Pause
(`g_re15_pauseflags & RE15_PAUSE_SCD`, dieselbe Schranke wie scd_vm_tick) — dann:
1. Bewegen aus `g_scd_pad_held` (virtuell; unter der Pad-Sperre 0xf000 automatisch tot): 0x0001 -> z += 200,
   0x0004 -> z -= 200, 0x0002 -> x += 200, 0x0008 -> x -= 200 (jede Richtung einzeln, wie sub01).
2. Druck `g_scd_pad_edge & 0x0040`: Heisspunkt projizieren; in der Huelle -> `re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK)`,
   Zustand FREI (naechster op_for laeuft durch); sonst `re15_msg_install_text(20, k_nichts, sizeof k_nichts)` +
   `re15_msg_install_durations(20, re15_msg_compute_duration(...))` + `re15_dialog_open_mask(20, 0, 0xFFFF0000u)`.
Protokoll (nur PC, `RE15_HEBETISCH_LOG` erweitern): je Bild `cursor=<zustand> x=<x> z=<z> sx=<px> sy=<px> treffer=<0|1>`
und Zeilen fuer Druck/Klick/Text — Grundlage der Abnahme.

### 5.4 KONSTANTEN-TABELLE

| Konstante | Wert | Beleg @0x… / Datei-Offset / NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|
| Raeume | 0x1150, 0x1151 | Auftrag §5; Record @0x00D7E in beiden identisch |
| Ausloeser-Ereignis | 4 | ROOM1150 main00 @0x00D7E Nutzlast `ff 00 18 04` (einziges `18 04` im Raum) |
| Halte-Signatur | `29 04 32 00 24 af cf fe cc bb 09 0a 05 00 0d 00 18 00 0f 00`, Halt = Signatur+14 | ROOM1150 @0x0FB2..@0x0FC5 (1151 @0x0F90), Zensus 2/240 (§3.7) |
| Cursor-Modell | MD1 5556 B | ROOM11F0.RDT @0x001928 (Prop 0, Tabelle @0x0240), bytegleich eingebacken |
| Cursor-Textur | TIM 33312 B, 8bpp 128x256, CLUT @VRAM(0,480) | ROOM11F0.RDT @0x018DAC |
| Cursor-Typ-Anhebung | -900 | EXE @0x8002c23c/@0x8002c24c (re15_prop_render_y), Obj_model_set ROOM11F0 @0x00E54 pc[2] = 4 |
| Cursor-Start (x,z) | (-19554, 22684) | ROOM11F0 sub00 @0x00E54 (`9e b3` / `9c 58`); Rueckstellung sub17 @0x01602 |
| Schritt je Bild | 200 | ROOM11F0 sub02..05 @0x012F6/0x01302/0x0130E/0x0131A (`c8 00` / `38 ff`) |
| Tastenmasken Bewegen | 0x0001 UP, 0x0004 DOWN, 0x0002 RIGHT, 0x0008 LEFT (gehalten) | ROOM11F0 sub01 @0x01098/@0x010B0/@0x010C8/@0x010E0; Richtung->Achse sub02..05 |
| Aktionsmaske | 0x0040 (Flanke) | ROOM11F0 sub01 @0x01106 `51 01 40 00`; Flanke: PORT-WAHL §5.1 Nr. 4 (DAT_800ac76c statt DAT_800ac768) |
| Abbildungskamera | Cut 10 von ROOM11F0: fov 26684, pos (-19628,-17442,22616), tgt (-19628,15928,22617) | ROOM11F0.RDT Kameratabelle @0x001A0 (RDT+0x24 -> 0x60, Eintrag 10) |
| Farbe | Tint 128 (neutral) | gemessen: 11F0 rendert CLUT[3]/[4] 1:1 (§2.3); PORT-WAHL "wie 11F0 im Port" |
| Tiefe in der Tri-Queue | 0 (vor allem) | PORT-WAHL: Zeiger ueber allem; Tris und PRI-Masken werden gemeinsam nach Tiefe sortiert (render_pc.c end_frame), Tiefe 0 liegt vor jeder Maske |
| Randgrenze | keine | wie 11F0 (gemessen §2.3, op_add_speed ohne Grenze) |
| Trefferflaeche | Huelle (151,171) (163,161) (180,151) (206,149) (234,151) (248,157) (267,171) (264,191) (261,205) (214,211) (164,205) (151,191) | Projektion Prop 1 @0x138D4, Prop 2 @0x13B88, Podest aus Prop 0 @0x11E40 unter Cut 4 @0x00E0 bei Plattform @0x0FB4; gerendert 99,53 % (§3.6). Besser: zur Laufzeit dieselbe Projektion (siehe unten) |
| Heisspunkt | Projektion von (x, -1800, z) unter Cut 10 | Objektlage wie der Zellstempel @0x80042f5c; -1800 = Oberseite -900 + Typ-4 -900 |
| Klick | `re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK)` = RE2 Gruppe 2 / 0x0A | NUTZER-VORGABE ("wie ... Generator in ROOM 11F0"); RE2 ROOM2130.RDT @0x01192; Port scd_vm.c op_sce_key_ck |
| Klick ausserhalb | keiner | 11F0 gemessen (m5, §2.4) |
| Text-Id | 20 | VERTRAG §1.3; frei: 1150 hat 15 Texte (off[0]=0x1E @0x012F8), 1151 hat 4 (off[0]=0x08 @0x010EC) |
| Text-Bytes | `04 02 2a 4b 50 44 45 4a 43 00 44 3d 4c 4c 41 4a 41 40 57 01 00` | NUTZER-VORGABE Wortlaut "Nothing happened"; Glyphen ROOM1000 @0x00D24, ROOM3001 @0x0199D, '.' @0x00D33, Kopf/Ende @0x00D22/@0x00D34; Punkt: Zensus 678/758 |
| Pausemaske Text | 0xFFFF0000 | sce-1-Handler @0x80043098/@0x800430a4 (u16@+2 << 16), alle 524 sce-1-Records 0xffff (scd_vm.c) |
| TIM-Slot | 28 = RE15_TIM_SLOT_PROP(8) | VERTRAG §1.5 obj_id 8; main.c RE15_TIM_SLOT_PROP (6..15 -> 26..35); in 1150/1151 frei (nOmodel 4, Port-Props 4..7 = Slots 8/9/26/27) |

Trefferflaeche zur Laufzeit statt als Zahlen (empfohlen): das Modul projiziert die 12 Huellen-Punkte nicht, sondern
die Weltpunkte der Kuppel (Deckel-Punkte aus den Prop-1/2-MD1, Podest-Punkte aus Prop 0) mit der AKTIVEN Kamera und der
aktuellen Plattformlage und bildet die Huelle je Druck neu — dann kann sie nicht von der gezeichneten Kuppel abweichen.
Die Zahlen oben sind dann nur der Riegel-Sollwert. Wer es einfacher will: Zahlen als Tabelle, Riegel vergleicht sie
mit der Engine-Projektion (§6 R2).

### 5.5 Gemeinsame Dateien — was genau wo (minimale Haken)

* scd_vm.c: 1 Zeile in `op_for` + 1 `#include "re15_hebetisch_cursor.h"`.
* game_step_common.c: 2 Zeilen (Aktion, Tick) + Include.
* scd_room_setup.c: 1 Zeile (Install) + Include.
* main.c: Upload-Block (~10 Zeilen, Muster Granaten-Block) + 1 Zeichen-Aufruf + Include.
* Keine Aenderung an aot_common.c, msg_common.c, panel_zeiger_common.c, re15_panel_zeiger.h (Spur C), audio_pc.c.

## 6 Abnahmeplan

### 6.1 Riegel (Unit, headless)

| # | Riegel | prueft | Gegenprobe (Mutation -> rot) |
|---|---|---|---|
| R1 | `unit_r34n_b_halt` | ROOM1150+1151: Signatur gefunden (Halt @0x0FC0 / @0x0F9E); Aktion 4 + sub04 -> Thread steht nach 11 Bildern auf dem Halt-PC, Plattform y=-305, angefordert Cut 4, Deckel lokal z=0; 60 weitere Bilder: PC und Deckel unveraendert | Haken in op_for entfernt -> Deckel bewegen sich |
| R2 | `unit_r34n_b_kuppel` | Engine-Projektion (camera_common.c) der Kuppel unter Cut 4 = Huelle §3.6 (+-1 px); Heisspunkt am Start (161,119) liegt NICHT drin, (209,176) liegt drin; 1151 gleich | Huelle aus Prop 0 statt 1/2 -> rot |
| R3 | `unit_r34n_b_ablauf` | Kuppeldruck: Klickzaehler +1, naechstes Bild For ausgefuehrt, nach 15 Bildern Deckel offen (+-150), Ruhe-Fenster und Items wie unit_r31_hebetisch; Fehldruck: Text 20 offen, Bytes = §5.4, Klickzaehler unveraendert, Halt bleibt | Treffertest invertiert -> rot |
| R4 | `unit_r34n_b_cut_old` | nach komplettem Durchlauf mit Cursor: `Cut_old` stellt die Raumkamera her (cam_id_prev != 4) | Vorschalt-Variante (Port zeigt Cut 4 selbst) -> cam_id_prev == 4 -> rot |
| R5 | `unit_r34n_b_harness` | `re15_aot_fire_slot(1)` / `scd_event_fire(4)` ohne Aktion -> KEIN Halt (Fahrt wie heute) | Armieren im scd_event_fire -> rot |
| R6 | `unit_r34n_b_cursor_bytes` | eingebackenes MD1/TIM == ROOM11F0.RDT @0x001928/@0x018DAC | ein Byte geaendert -> rot |

### 6.2 An der echten exe (Framedumps + Log; Laeufe mit `lauf.sh`)

| Nutzerpunkt | Messung | Soll |
|---|---|---|
| "nicht direkt aufgeht" | Tuerweg wie m1 (A bei F230), Serie F230..F330 | Deckel bleiben zu; Log `cursor=AKTIV` ab dem Haltbild; Bild ab F241: Cursor sichtbar |
| "unseren Cursor" | Cursor-Pixel am Start in ROOM1150 gegen 11F0 F480 (m2): nur CLUT-Farben vergleichen | gleiche Pixel (bbox 148.3..172.0 x 107.7..130.0, 524 Punkte bei 960x720) |
| "navigieren" | R0.5 / D1 / U0.5 / L0.5 wie m3 | 8 px je Bild @960, Richtungen wie 11F0 |
| "Nothing happened" | Druck bei Start (161,119) | `[msg]` Text 20 offen, Wortlaut im Bild, KEIN `se=10` (`AUDIO=1 RE15_SE_DEBUG=1`), Deckel zu |
| "unten rechts auf die Kuppel" | R 18 Bilder + D 22 Bilder (Heisspunkt ~(209,177)), A | `se=10` im selben Bild, Deckel oeffnen im naechsten, Ruhe-Protokoll wie Runde 31 (`ruht=1 pc=0x101B`), Items-Dialoge |
| "normaler Ablauf" | bis Parken + 30 Bilder | `Cut_chg`/Cut_old-Log: zurueck in die Raumkamera (nicht 4), Bild ohne Loch |
| Rand | Cursor hinaus und zurueck | verlaesst das Bild wie 11F0, kommt zurueck |
| Unterdecken / PRI | Heisspunkt auf y 230 | Cursor sichtbar (vor der Tischkanten-Maske) |
| ROOM1151 | Lade-Weg (probe_r30_granate_karte ... 1151, CONTINUE), dann zu Fuss an den Tisch, A | wie 1150 |
| Laden/Speichern | Karte mit (9,53)/(9,56) gesetzt, Durchlauf | Cursor, Kuppel, leere Faecher, kein Dialog |

Integration: `test_r34n_b_cursor.cmake` (echte exe, Eingabeskript wie oben, prueft Log-Zeilen: Halt, Fehldruck-Text,
Kuppeldruck-Klick, Deckelfahrt, Cut_old-Ziel). Bestehende Riegel unveraendert gruen (§4.3), RE15_MIN_TESTS +Anzahl.

## 7 Risiken, Softlocks, Wechselwirkungen

1. **Kein Ausgang ausser der Kuppel** (bewusst, §1.3/§3.4). Softlock nur, wenn der Halt nie frei wird: Treffertest
   fehlerhaft oder Huelle nicht erreichbar. Riegel R2/R3 + exe-Abnahme decken das; der Cursor ist unbegrenzt, die
   Kuppel also immer erreichbar.
2. **Cut_old-Falle** (§3.2) — durch den Halt-Weg vermieden, Riegel R4 haelt es fest.
3. **Pad-Sperre / Text:** waehrend "Nothing happened." maskiert das Pausewort die virtuellen Woerter auf 0xf000 —
   Bewegung und 0x0040 sind tot, der Text schliesst mit 0x4000. Nach dem Schliessen verhindert die Flanke eine
   Wiederholung.
4. **Inventar (START) waehrend des Cursors:** wie in 11F0 nicht gesperrt (dort nur (2,0)/(2,2), kein Bit 0x01000000);
   das Inventar verwirft die Tri-Queue (re15_render_pc_clear_textris) -> Cursor unsichtbar solange, danach wieder da.
5. **Spur C (ROOM11F0)** nutzt denselben Klick-Aufruf und die Pad-Woerter; B aendert weder op_sce_key_ck noch
   panel_zeiger_common.c. Keine gemeinsamen Zeilen ausser Includes.
6. **Andere Spuren in 1150:** keine (A/D/E in 1050, F in 1110/1230, G in 1170). Text-Id 20 wird unmittelbar vor dem
   Oeffnen installiert, damit eine gleichnamige Id einer anderen Spur in einem anderen Raum nicht nachwirkt.
7. **Mess-Haken:** `RE15_FIRE_AOT` bringt keinen Cursor (§4.3) — wer den Cursor messen will, muss die Aktionstaste
   am Tisch druecken (Tuerweg m1).
8. **PSX-Ziel:** kein Halt (5.2). Android: neue .c-Dateien -> frischer Configure (Memory android-glob-cache).
9. **Irons (Typ 0x45) im Raum:** KI steht (sub04 @0x0F9A), wie heute waehrend der Fahrt.

## 8 Offene Punkte

1. **Abbruch ohne Kuppel?** RE1.5 kennt keinen (§3.4). Falls der Nutzer einen will: virtuelle Abbruchtaste 0x8000
   (<- CROSS, @0x80073dbc[15], die Menue-Abbruchtaste) -> Halt frei UND sub04 bis zum Parken ueberspringen waere ein
   Eingriff in sub04; sauberer: vor dem Halt gar nicht erst Cut 4 zeigen geht nicht (Kamera kommt aus sub04). Vorschlag,
   falls gewuenscht: bei Abbruch sub04 per Evt_kill beenden und die drei Aufraeum-Opcodes von @0x109A..@0x10B2
   nachstellen — erst nach Nutzerentscheid.
2. **Trefferflaeche nur Deckel oder Deckel+Podest?** Geplant ist beides (sichtbar ein Objekt). Nur Deckel waere
   y 149..188 statt 149..211.
3. **Sprachdatei** `synchro/STAGE1/room1150/main20.wav` (+ `room1151/main20.wav`) fuer "Nothing happened." — optional,
   Untersuchungstexte sind in RE1.5 unvertont.
4. **Klick-Stimme parallel zu sub04-Laut:** beim Kuppeldruck spielt nur der Klick; der ROOM1150-Laut 0x0A kam schon beim
   Druck am Tisch (@0x0FA2). Falls der Nutzer beides beim Kuppeldruck hoeren will, muesste der Halt VOR @0x0FA2 liegen
   (dann aber ohne Cut 4/Plattform — nicht moeglich, §2.2). Bleibt so.
