# Runde 32 — Hebetisch ROOM1150/1151: Granate + Sicherung in die unteren Fächer

Stand: 2026-09-29, Zweig `r32/hebetisch-fach`, Baum `.claude/worktrees/r32_hebetisch`.

## 0. Befund (wörtlich, AUFTRAG.md)

> "Die Granate und die Sicherung die im hochfahrenden Modell in ROOM 1170 rein soll, liegt aktuell
> oben drauf. Aber sie sollen unten, in den hochfahrenden Fach liegen - ein item links ein item rechts."

"ROOM 1170" = Hebetisch ROOM1150/1151 (Runde 30 bestätigt).

## 1. Ausgangslage (Runde 31)

- Sicherung obj 4 (Item 0x40): (-280,-1062,1280) rot_y 1440 — oben in der Kuppel.
- Granate obj 7 (Item 0x09): (-260,-1091,1140) rot_y 1792 — oben in der Kuppel.
- Ziel: Granate LINKES unteres Fach, Sicherung RECHTES unteres Fach (Bildschirm, Cut 4),
  auf dem Fachboden, fährt mit (parent_obj=0).

## 2. Geometrie der unteren Faecher (ausgelieferte Bytes, nur gelesen)

Werkzeug `hebetisch_faecher_werkzeug/faecher.py` (sucht in Prop 0 die achsparallelen Vierecke, die von
der Vorderfront x=2 bis zur Rueckwand x=-1258 durchgehen; Viereck-Face-Offset = MD1-Kopf Feld 11 `qf`),
`p0_dump.py` (volle Punkt-/Flaechenliste mit Datei-Offsets). ROOM1150.RDT == `info/Re1.5/PSX/STAGE1/ROOM1150.RDT`
(cmp). Plattform-Koordinaten, +Y nach unten.

Prop 0 (MD1 @0x11E40, 1 Mesh, 163 Punkte, 11 Dreiecke, 120 Vierecke) ist unten ein Kasten
x[-1258..2] y[-901..-1] z[5..1805] (Tischplatte y=-901, das Kuppel-Podest y -886..-1036 sitzt darauf).
Darin zwei durchgehende Faecher, Trennwand z 861..950:

| Fach | z | Boden | Decke | Seitenwaende | Rueckwand x=-1258 | vorn x=2 |
|---|---|---|---|---|---|---|
| A | 96..861 | y=-90, Viereck 89 @0x12EE0 (`6b00 8a00 6b00 8b00 6b00 7e00 6b00 7f00` = Punkte 138/139/126/127) | y=-810, Viereck 87 @0x12EC0 | z=96 Viereck 86 @0x12EB0, z=861 Viereck 88 @0x12ED0 | volle Tafel Viereck 119 @0x130C0 | offen (Rahmen um y -810..-90) |
| B | 950..1715 | y=-91, Viereck 93 @0x12F20 (`6f00 8e00 6f00 8f00 6f00 8200 6f00 8300` = Punkte 142/143/130/131) | y=-811, Viereck 91 @0x12F00 | z=950 Viereck 90 @0x12EF0, z=1715 Viereck 92 @0x12F10 | volle Tafel Viereck 82 @0x12E70 | offen (Rahmen um y -811..-91) |

Bodenpunkte (Datei-Offset, Bytes): 138 @0x122D4 `0200 a6ff 5d03` (2,-90,861), 139 @0x122DC `0200 a6ff 6000`
(2,-90,96), 126 @0x12274 (-1258,-90,861), 127 @0x1227C (-1258,-90,96); 142 @0x122F4 `0200 a5ff b306`
(2,-91,1715), 143 @0x122FC `0200 a5ff b603` (2,-91,950), 130 @0x12294 (-1258,-91,1715), 131 @0x1229C
(-1258,-91,950). Keine andere Flaeche von Prop 0 ragt ins Innere eines Fachs (Clip-Test, 0 von 131).
ROOM1151.RDT (Prop 0 @0x13EB8): dieselben Werte, Boden A Viereck 89 @0x14F58, Boden B Viereck 93 @0x14F98.

Kamera Cut 4 (@Datei 0xE0, pos (-21942,-2160,-18378), tgt (-19980,-1566,-18396)): Plattform rot_y 2048 bei
(-20700, y, -17460) -> Kamera in Plattform-Koordinaten x=+1242, z=+918 (genau vor der Trennwand z 861..950),
y = -2160 - Plattform-y (Ruhe oben -1205: y=-955, also UEBER der Tischplatte -901 -> Blick von oben
schraeg in beide Faecher). +z = rechts auf dem Schirm (Runde 31 gemessen) -> **Fach A (z 96..861) links,
Fach B (z 950..1715) rechts.** Planung (`fach_schirm.py`, Rechnung, Messung folgt): Ruhe oben Boden A
vorn Schirm-x 9..147 y 204, hinten 77..152 y 130; Boden B vorn 163..299 y 203, hinten 160..234 y 129.

## 3. Arbeitsprotokoll

(wird fortgeschrieben)
