# Runde 30 / Nachtrag K — Granate im Hebetisch von Irons' Office

Auftrag (AUFTRAG.md Abschnitt K, wörtlich):

> Außerdem möchte ich im hochfahrenden model in irons office eine granate mit hochfahren haben.

Vorbild ist die Sicherung aus Abschnitt H (`engine/src/sicherung_1150.c`, Dossiers `sicherung.md`
und `nachschliff-sicherung-nein.md`). Werkzeuge dieses Nachtrags: `nachtrag-granate_werkzeug/`,
Belege (Textausgaben, Bilder): `nachtrag-granate_belege/`.

## 1. Welche Granate — Item-Id 0x09 „Hand Grenade"

Die Namensliste `analysis/befunde_2026-09-21/belege/re15_item_names.txt` zählt ab 0, die Ids ab 1.
Die Id habe ich deshalb über den Namensleser selbst bestimmt, nicht über die Liste.

Namensleser FUN_80028840 (PSX.EXE, `re15_disasm.py dis 0x80028840`):

```
80028840: andi a0,a0,0xff
80028844: sll  a0,a0,1
80028848: lui  at,0x800c / 8002884c: addiu at,at,18780   ; 0x800c495c
80028854: lhu  v1,0(at)                                ; Offset = Tabelle[id]
80028858: lui  v0,0x800c / 8002885c: addiu v0,v0,18984  ; 0x800c4a28
80028864: addu v0,v1,v0                                ; Name = 0x800c4a28 + Offset
```

`sicherung_werkzeug/item_namen.py 0x07 0x0c` (DEBUG.BIN lädt @0x800c0000):

| Id | Tabelle | Offset | Name @ | Text |
|---|---|---|---|---|
| 0x08 | @0x800C496C | 0x004C | @0x800C4A74 | Remington M870 |
| **0x09** | **@0x800C496E** | **0x005B** | **@0x800C4A83** (Datei 0x04A83) | **Hand Grenade** |
| 0x0A | @0x800C4970 | 0x0068 | @0x800C4A90 | Acid Grenade |
| 0x0B | @0x800C4972 | 0x0075 | @0x800C4A9D | Incendiary Grenade |

Die Liste führt „Hand Grenade" bei Index 8 → Id 0x09. Das passt zu ihrer Zählweise.

**Warum die Hand Grenade und nicht Acid/Incendiary oder die Werfer-Munition.** Nur Id 9 hat im
Auslieferungsstand einen Wurf. Der Beleg ist die Entlade-Tabelle @0x80074100
(`re15_disasm.py table 0x80074100 22`) zusammen mit dem Waffen-FSM:

* [9] @0x80074124 → **0x80033B38**. Der Handler hat 8 Instruktionen und ruft nur den
  Munitionsabzug (`jal 0x8004eae4` @0x80033b40).
* [10]/[11] → 0x80033B58/0x80033B78. Diese Handler ziehen ebenfalls nur Munition ab. Einen
  Projektil-Spawn gibt es für diese Ids nirgends (`waffen_fsm_2026-09-12/SPEC.md` §2).
* [15]..[18] (die drei Granatwerfer-Ids 0x0F–0x11 und 0x12) sind NULL (@0x8007413c-48).
  Feuern hieße dort `jalr 0`, also ein Absturz. Diese Klasse ist unfertig.
* Der Projektil-Spawn steckt im FSM, hart auf Id 9:
  `80033680: lbu v1,-13731(v1)` (0x800aca5d = ausgerüstete Waffe), `80033688: ori v0,zero,0x9`,
  `8003368c: bne v1,v0,0x800337ac`. Danach wird bei Bild 0x13/0x16/0x18 (je nach Zielhöhe,
  Bits 0x8000/0x4000/0x2000 in 0x800acaec) `lui a0,0x40d / ori a0,a0,0x1000` gesetzt und
  `jal 0x80019700` gerufen (@0x800336bc-ec, @0x8003371c-4c, @0x80033778-a8).

Für „Acid/Incendiary Grenade" und die Werfer-Munition gibt es in RE1.5 keine Platzierung (§3). Die
Munition 0x19–0x1B hätte dazu keine funktionierende Waffe. Die Wahl fällt deshalb auf die **Hand Grenade, Id 0x09**.

## 2. Funktioniert die Waffe im Port? — Teilweise. Der WURF selbst fehlt.

### 2.1 Messung im echten Spiel

Werkzeug `nachtrag-granate_werkzeug/lauf_wurf.sh`: echte exe, beschleunigter Renderer,
Framedump, kein AUTOSHOT/SOFTWARE_RENDER. Sprung nach ROOM1140, `RE15_GIVE=9:5 RE15_EQUIP=9`
(Mess-Harness in main.c), dann R1 halten und Viereck drücken (Eingabeskript = echte Pad-Bits).

| Was | gemessen | Beleg |
|---|---|---|
| Waffenbank | `[equip] W-bank -> W09 (Clips 13, Rueckstoss-Clip7 fc=35)`, Hand+Granate-Netz aus PL00W09.PLW | wurf1 debug.log |
| Wurfanimation | Clip 7 läuft 35 Bilder, Leon holt aus und wirft (Lupe F140..F176) | `nachtrag-granate_belege/wurf2_lupe.png` |
| Munition | 5 → 4 → 3, eine je Wurf (FUN_8004eae4 @0x80033b40) | wurf3 state.log `mg=` |
| Projektil-Spawn | `SPAWN id=4 sub=13 scale=0x1000 streams=1` bei Rückstoß-Bild 22 (MITTE) | wurf2 wf.log F142, wurf5 F69 |
| Projektil-Flug | **keiner**. Der Effekt-Platz wird nie wieder frei: `fx` steigt nach jedem Wurf um 1 und fällt nie (wurf2: 405× fx=0, 201× fx=1, 60× fx=2 bis Laufende) | wurf2 wf.log |
| Explosion | **keine** | Bilder wurf2/wurf5 |
| Schaden | sofort beim **Abzug** (F47), 22 Bilder BEVOR die Granate die Hand verlässt (F69). Zombie 2 in 1300 Abstand: F47 `st=3` (Treffer), F77 `st=7` (tot). Dazu Blut (id 0) und zweimal Feuer (id 8 sub 3) am Zombie, alles in F47 | `nachtrag-granate_belege/wurf5_zustand.txt`, `wurf5_bogen.png` |

Der Schaden kommt also nicht aus der Granate. Er stammt aus der **Port-Brücke** in
`game_step_common.c` (Entlade-Tabelle `ENT[9] = {1,1,1,0}`, `resolve=1`). Sie wertet beim Abzug
einen Sofort-Treffer aus (`re15_player_weapon_fire(9)` = Stellvertreter für FUN_80011f50).
Der Kommentar dort nennt das selbst eine Port-Brücke: „der Explosionsschaden des Projektils ist
noch un-RE'd".

### 2.2 Was im Original dahinter steht (selbst disassembliert)

Der Spawn liefert Effekt 4, sub 0x0D der globalen Bank CORE00.ESP. `esp_zeilen.c` (gegen
libre15_engine gelinkt, Ausgabe `esp_zeilen.txt`) zeigt Stream 0 mit **2 Zeilen @Datei 0x1AB8**,
Zeile 0: Routine A = **30**, Beschleunigung (0,10,0). Routinentabelle @0x80071d40
(`re15_disasm.py table 0x80071d40 48`): [29] → 0x80018320, [30] → **0x8001843c**, [31] → 0x8001854c.

* **Routine 30 @0x8001843c (Wurf-Init, 70 Instruktionen):** setzt `+0x6e := 0x17`, Flags `+0x6c := 3`,
  Routine B `+0x02 := 0x1d` (29), Routine A `+0x00 := 0`, Zünder `+0x1e := 0x2a` (42). Die
  Wurfgeschwindigkeit hängt an der Zielhöhe 0x800acaec: 0x8000 → (0x17c,-110,0x15), Beschleunigung x -2
  (@0x80018498-dc); 0x4000 → (0x118,-50,0x18), -1, dazu Abpraller `+0x26 = rng & 3 + 7`
  (`jal 0x8001af20` @0x800184d8, @0x800184ec-0x8001850c); 0x2000 → (0x50,0,1), -1, `+0x26 = 5`
  (@0x8001851c-38).
* **Routine 29 @0x80018320 (Flug und Abprall):** Bei jedem Bodenkontakt (`+0x2a` > 0) wird
  `vel_x -= vel_x/3` und `vel_y := -(vel_y/3)` gerechnet (0x55555556-Idiom @0x8001838c-f8),
  der Zähler `+0x26` sinkt um 1, und es kommt der SE `0x010A0001 | (n<<8)` (`jal 0x80045024`
  @0x80018424). Bei Zähler 0: SE 0x010A0001 (@0x80018358), Flags `0x63`, Routine A := **31**
  (@0x8001837c).
* **Routine 31 @0x8001854c (Explosion):** Der Zünder `+0x1e` zählt herunter. Bei 7 wird der
  Lärm-Latch gesetzt (`sb v0,21336(at)` = 0x800b5358 @0x8001857c) und
  **`jal 0x80012d60`** gerufen (@0x800185b8, a0 = 0x1f4 = 500, Punkt = Lage mit y−500, a2 = 2).
  Laut Katalog (`RE15_FUN_CATALOG.md` Zeile 57) ist FUN_80012d60 der gemeinsame
  Treffer-Resolver, der jeden Gegner UND den Spieler im Radius trifft; im Port ist er
  `re15_resolve_attack`. Dazu kommen der Kind-Effekt 0x03195000 (`jal 0x800199d4` @0x800185dc)
  und der SE 0x04080001 (@0x800185ec). Bei 2 folgen 0x03195000 und 0x030B5400, bei 0 kommt
  0x030B5800 und der Platz wird frei (`sb zero,108(a1)` @0x800186b0).

**Im Port fehlen alle drei Routinen.** `esp_fx_dispatch` in `re15_esp.c` kennt die Routinen A
0/3/4/5/8/9/10/11/15/16/17/18/38, `esp_fx_dispatch_b` kennt nur 12. Die Granate bleibt deshalb
ohne Wurfgeschwindigkeit an der Hand stehen, fällt mit der Zeilen-Beschleunigung (0,10,0) und
wird nie freigegeben.

### 2.3 Befund und Umfang

**Die Hand Grenade ist im Port NICHT wie im Original benutzbar.** Ausrüsten, Wurfanimation und
Munitionsabzug funktionieren. Der eigentliche Wurf fehlt (Flug, Abprall, Zünder, Explosion,
Flächenschaden). Stattdessen trifft die Port-Brücke sofort beim Abzug. Nach Auftrag baue ich den
Wurf in dieser Runde **nicht**. Er braucht:

1. Die Row-VM-Routinen 30, 29 und 31 in `re15_esp.c` (zusammen rund 250 MIPS-Instruktionen, alle
   oben mit Adresse). Dazu kommt Routine B 29 im Hauptlauf-Dispatch neben der 12.
2. Die Kind-Effekte 0x03195000 / 0x030B5400 / 0x030B5800 (CORE00 Effekt 3, sub 0x19/0x0B).
   Offen ist, ob der Port deren Zeilen schon trägt; das ist nicht geprüft.
3. Die SEs 0x010A0001 (Abprall, ARMS-Bank Satz 0x0A) und 0x04080001.
4. Den Flächenschaden über `re15_resolve_attack(500, Punkt, 2)` statt der Sofort-Brücke. Dafür
   muss `ENT[9].resolve` auf 0 gehen. Zu klären ist dabei, ob die Explosion auch den Spieler
   trifft; laut Katalog prüft FUN_80012d60 auch den Spielerblock.
5. Einen Riegel: Wurf → Flug → n Abpraller → Explosion bei Zünder 7 → Schaden im Radius 500 →
   Platz frei.

Das ist ein eigenes Thema im Umfang einer Spur. Die Granate wird trotzdem als Aufnahme gebaut.

## 3. Menge je Aufnahme — keine Platzierung im Original → Port-Wahl 1

`nachtrag-granate_werkzeug/zensus.py` (Walker `scd_walk_lib`, Satzlage wie `op_item_aot_set` /
RE1.5 @0x80040644) läuft über alle 206 RDTs mit Kopf und alle 164 `Item_aot_set`. Ausgabe:
`nachtrag-granate_belege/zensus.txt`.

| Id | Name | Platzierungen |
|---|---|---|
| 0x09 / 0x0A / 0x0B | Hand / Acid / Incendiary Grenade | **0 / 0 / 0** |
| 0x0F / 0x10 / 0x11 | Grenade Launcher | 0 / 0 / 0 |
| 0x19 / 0x1A / 0x1B | Explosive / Acid / Incendiary Rounds | 0 / 0 / 0 |
| 0x1D..0x20 | Empty Shells, Capsules | 0 |

Dazu gibt es null `Sce_key_ck` und null `Keep_Item_ck` mit diesen Ids. Die Menge ist damit
**Port-Wahl, keine Original-Adresse: 1**. Gründe:

* Der Nutzer schreibt „eine Granate".
* Das Weltmodell zeigt genau eine Granate.
* Ein Wurf verbraucht genau eine (Entlade-Handler 0x80033B38 → `jal 0x8004eae4` @0x80033b40,
  gemessen mg 5 → 4 → 3).

Die 250 in der Waffentabelle @0x80074da8 (`inventory_common.c` s_wpn_props[9]) sind die
Nachlade-Portion eines Stubs (Nachladen erst ab Id < 9, `sltiu` @0x80033368), keine Aufnahmemenge.

## 4. Weltmodell, Item-Bild, Icon, CHECK-Foto

**Weltmodell:** RE1.5 hat keine Platzierung (§3), also auch kein Prop-MD1 einer Granate. Das
einzige ausgelieferte Granatennetz ist die Granate IN DER HAND: `PLD/PL00W09.PLW` dir[2]
(MD1 @0x5544..0x5F78, 2612 B), Textur dir[3] (TIM @0x5F78, 56×32 8bpp, CLUT (256,480)).
PL00W0A/0B sind bytegleich mit W09 (`cmp`).

Das Netz (`plw_baender.py`, `nachtrag-granate_belege/plw_baender.txt`) hat 40 Flächen auf page 0x81 /
clut 0x7840. Sie teilen sich SAUBER in zwei Bänder (Band-Regel: main.c, Kommentar WEAPON TEXTURE
COMPOSITE, byte-true FUN_80036b68 @0x80036c08):

* **Waffe** (v ≥ 224, das 56×32-Bild aus dir[3]): 19 Flächen (5 Dreiecke, 14 Vierecke),
  24 Punkte, Hülle x −77..77, y 135..245, z −23..87 = 154 lang, 110 Durchmesser
* **Hand** (v 108..159, Hautatlas): 21 Flächen, 22 Punkte
* gemeinsame Punkte: **0**

Das Weltmodell sind genau diese 19 Waffen-Flächen mit ihren Punkten, Normalen, UVs und der dir[3]-Textur.
Es wird nichts erfunden. Die Granate ist dort kein geschlossener Körper: die Seite zur
Handfläche (z = −23) und zwei untere Fasen fehlen, ebenso ein Übergang am +x-Ende. Diese Flächen
verdeckt im Original die Hand. Der Export legt die offene Handseite nach UNTEN auf den Fachboden
(§5).

**Item-Bild und Icon** (`bilder.py`): ITPS.ITP Block 0x09 @0x1B000 trägt crect (0,489) / prect
(832,256). Das ist das RE1.5-Format, das der CHECK-Foto-Lader erwartet (DEBUG.BIN @0x800c0258,
Prüfung in `inv_render_pc.c`). Es gehört also NICHT zu den 19 Blöcken im RE2-Format (davon ist 0x40 seit
Abschnitt H ersetzt), deren Foto leer bleibt. Das Bild zeigt eine olivgrüne Handgranate (`nachtrag-granate_belege/bilder_itps_09_x4.png`).
Das Icon ist Tile 0x09 in ITEMALL.PIX @0x2A30, alle 1200 Bytes belegt
(`bilder_icon_09_x8.png`). Beide sind nicht bytegleich mit einem RE2-Block bzw. -Tile, also
eigene RE1.5-Kunst. **Nichts einzusetzen**, anders als bei der Sicherung.

## 5. Sitz im Kuppelfach — Port-Wahl aus der Fachgeometrie

`nachtrag-granate_werkzeug/fach.py` und `sitz.py` (Ausgaben `sitz.txt`), Plattform-Koordinaten:

* Fachboden: acht Punkte y = −1036, x −485..−74, z 875..1645 (ROOM1150 Prop 0 @0x121AC..0x1221C,
  ROOM1151 bytegleich)
* Kuppel (Prop 1 MD1 @0x138D4, Prop 2 @0x13B88): Basis y −1036, Ring y −1138 (x −413/−147,
  z 1011..1510), Scheitel (−280, −1185, 1260). Geschlossen ist sie über dem Fach höchstens 149 hoch.
* Sicherung: (−280, −1062, 1260), rot_y 1024, Rohr 406 × Ø 52 → x −306..−254, z 1057..1463

Die Granate liegt flach auf dem Boden, Mitte y = −1036 − 55 = **−1091**, Längsachse entlang z wie
die Sicherung, Mitte z = **1260** (dort ist die Kuppel am höchsten). Gerechnet für x-Mitten
−400..−340 und beide Drehungen, in ROOM1150 und ROOM1151 gleich:

| x-Mitte | Spalt zur Sicherung | Luft unter der GESCHLOSSENEN Kuppel |
|---|---|---|
| −370 | 9 | −8,3 |
| **−365** | **4** | **−3,4** |
| −360 | −1 (durchdringt) | −1,6 |
| −355 | −6 (durchdringt) | +0,1 |

Beide Bedingungen zugleich erfüllt keine Lage: ohne Durchdringen der Sicherung ragt die Granate
bei geschlossener Kuppel mindestens 3,4 Einheiten durch die Schale. Die Kuppel ist nur
geschlossen, solange die Plattform am Schreibtisch steht (Szene F96–F101 und F411–F486,
`sicherung.md` §2.2). Offen gibt sie z 1110..1410 frei, und die Granate liegt mit z 1183..1337
ganz in dieser Öffnung.

**Gewählt: (−365, −1091, 1260)**, 4 Einheiten neben der Sicherung. Der Auftrag verlangt „dürfen sich
nicht durchdringen", das hat Vorrang. Ob die 3,4 Einheiten im Bild bei geschlossener Kuppel zu
sehen sind, entscheidet die Messung (§7).
