# Runde 37 — Spur J "bissarm" (2026-10-12)

Zweig `r37/bissarm`, Basis master 419380dc. Dossier fortlaufend (Sitzungslimit-Schutz: nach jedem Schritt committet).

## Auftrag (Wortlaut, AUFTRAG.md Nachtrag 6)

> Ja, wie ich gerade gesehen habe fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird.

Lesart (AUFTRAG.md, beide pruefen, keine F9-Marke): (1) der Arm des Zombies, der beim Biss/Griff an Leon haengt
(Griff von vorn/hinten, Kriecher am Bein; Leon UND Elza), fehlt in manchen Faellen/Bildern; (2) der angeschossene,
noch "dranhaengende" (baumelnde) Arm eines Zombies fehlt beim Biss. Erst messen, welcher Fall auftritt, dann wie RE2.

## 0. Stand

- [ ] 1 Messung vorher (Port, alle Biss-Varianten, je Bild Teile-Sichtbarkeit)
- [ ] 2 RE-Belege (RE2 Griff/Biss, Teile-Flags)
- [ ] 3 Umsetzung
- [ ] 4 Messung nachher
- [ ] 5 Tests

## 1. Messung vorher (Bau 419380dc, eigene Messkopie `re15_pc_j37mess.exe`, RE2-Standardgehirn + RE2-Modelle)

Werkzeug: Spielstand per `test_r36_kleidung karte` (Slot 0) -> CONTINUE; `RE15_STATE_LOG`, `RE15_FRAMEDUMP`
(Serien), `RE15_FORCE_CUT` (nur zum Hinschauen, Kamera festnageln), `RE15_ENEMY_DBG`, `RE15_RE2_TRACE`.
Laeufe unter `%TEMP%/.../scratchpad/J37/` (unversioniert; Zahlen hier zitiert).

### 1.1 ROOM1140 (Briefing-Raum, 5 Zombies) — Leon (-1400,-19600) Cut 1
- `enemy_dbg.log`: `RE2 EM016/EM010/EM011: 17 Meshes, 15 Bones, 31 Clips`, gezeichnet je `15 Teile`.
- Griff Slot 5 (Typ 0x11) F58..F165 (state.log `ss1=3`, `gr=1`), danach Slot 2 F166..F271 (Tod F227).
  Leons Lage waehrend der Griffe plausibel neben dem Greifer (z.B. F79 PL(-791,-19733), Greifer (-297,-19708)).
- Bild (Skala 3): Raum zu voll (vier Zombies um Leon), Arm-Zuordnung im Bild nicht eindeutig.

### 1.2 ROOM1010 (zwei Zombies) — Leon (3700,0,2300) Cut 4, 0,3 s rueckwaerts -> Griff durch Slot 1 (Typ 0x10)
state.log (Auszug):
```
F57 PL(3392,3279,rot=0,hp=100)    gr=0 [1 t=10 st=1 ss1=1 ...  @(3998,3875,r1541)]
F58 PL(0,0,rot=3589,hp=100)       gr=1 [1 t=10 st=1 ss1=3 ss2=1 mo=11 ... @(3998,3875,r1541)]
F89 PL(0,0,rot=3589,hp=80)        gr=1 [1 ... ss1=3 ss2=3 mo=12 af=26 d=5585 @(3524,3437,r1541)]
F146 PL(0,0,rot=3589,hp=60)       gr=1 [1 ... ss1=3 ss2=7 ...]
F147 PL(3514,3178,rot=3589,hp=60) gr=1 [1 ... ss1=3 ss2=8 ...]
```
**Leon steht den GANZEN Griff (F58..F146) auf (0,0)** — der Greifer beisst an seinem Platz ins Leere
(Kamera 6: Zombie allein in Biss-Pose ab F66), Leon ringt in Kamera 5/7 allein vor dem Schreibtisch.
Ursache (Code, `enemy_ai_common.c` re15_victim_place): die Port-Wandklemme (5f9ec726, 2026-09-13, "gilt jetzt
fuer JEDEN Greifer") faellt auf `s_victim_ok_x/z` zurueck; der Startwert wird nur beim Gitterarm-Griff
(enemy_ai_common.c ~Z.12901) und beim Gorilla (affen_11c0.c:86) gesetzt — NIE beim RE2-Zombie-Griff
(`re15_re2z_victim_begin`). Startwert ist damit der Rest eines frueheren Griffs bzw. (0,0).
