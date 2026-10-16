# Runde 38 — Spur N "wasser" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_wasser`, Zweig `r38/wasser`, Basis d70c234e (2026-10-16).

## Nutzer-Punkt (woertlich, AUFTRAG.md Z. 29)

> In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza im flachen Wasser bewegen.

## Stand

- [ ] Messung vorher (Port: was passiert heute in 1260 beim Gehen im Wasser? Bild + Ton + FX-Log)
- [ ] RE-Beleg Original (Mechanismus: Bodentyp -> Effekt + Schritt-Ton)
- [ ] Raumliste (Zensus)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

## Messung vorher

Gebauter Stand d70c234e (Baum r38_wasser, `local_build.sh configure` + `build` OK), Messkopie
`re15_pc_r38w_mess.exe`, Lauf: `RE15_DEBUG_JUMP=1260@240 RE15_PLAYER_POS=-8600,-7900,0,1`
(Band 1 = Laufsteg y -1800, Sprung-Spawn des Debug-Menues ist (-8750,-1800,-6900)),
`RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT="W0.5,U1.5,W1" START=60`, `RE15_FOOT_DEBUG=1`,
`RE15_STATE_LOG`, `RE15_AUDIO_CAP_SYNC`, Framedump 90-150/6.

- Weg: F72 (-8404,-7246) -> F92 (-7054,-7246), Wand bei x -7068 (Kasten 12 endet bei -6600, Radius 450).
- Schritte (debug.log): `[foot] SE foot=7 type=29 vag=1`, dann `foot=4 type=151 vag=7`,
  `foot=7 type=151 vag=7` = zwei Schritte im FLR-Fleck P0 (x -7800..-6600, z -8700..-7100, Typ 0x97).
  **Ton: vorhanden** (0x97 & 0x7f = Satz 0x17 -> VAG 7, vab_common.c maskiert Bit 0x80 seit Audit wf_1db9c802).
- Effekt-Pool (state.log Feld `fx=` = re15_esp_fx_count): **0 in jedem Bild F72-F100** — kein Spritzer.
- Code-Befund: `re15_audio_footstep(foot, re15_rdt_floor_sound(...))` (game_step_common.c:2159) ist
  der EINZIGE Verbraucher; der FX-Zweig von FUN_80045630 fehlt im Port vollstaendig, `water_y` (+0x88)
  wird von aot_common.c:1363 gestempelt, aber von NIEMANDEM gelesen und NIE geloescht.

### Raum-Zensus (Datei-Bytes, alle 206 nichtleeren RDTs, Skript flr_census.py / sce8_scan.py)

FLR-Wasserboden (Typ-Byte +8 mit Bit 0x80, alle 0x97):
| Raum | Flecken 0x97 | ESP-Ids (RDT+0x4C) |
|---|---|---|
| ROOM1260/1261 | 6 | 05 06 07 0b 0c 0d 0f |
| ROOM2000/2001 | 6 (+1x 0x17 ohne Bit) | dito |
| ROOM2050/2051 | 7 | dito |
| ROOM20B0/20B1 | 6 (+1x 0x17) | dito |

sce-8-Wasserzonen (Aot_set 0x2C, sce 8, Flags 0x47; Nutzlast u16 = Wasserspiegel):
11A0/11A1 (2, -900), 1260/1261 (2, -900), 2000/2001 (2, -900), 2020/2021 (1, -900),
2070/2071 (2 Saetze in zwei Zweigen, -200), 2090/2091 (3, -1620), 20A0/20A1 (2, -540, eine Band 1),
20B0/20B1 (2, -900) = 32 Saetze (wie aot_common/scd_vm "32 zones").
In 1260/2000/2020/20B0 decken sich die Zonen mit SCA-Kaesten u0=0x01 u1=0x02 Ebene 0x10 (Rinnen);
in 2070, 2090, 20A0 (Zone 2) und 11A0 liegt KEIN SCA-Kasten im Zonenmittelpunkt.
Effekt-Id 6 ist in allen Raeumen mit Wasserboden in der Raumbank (CORE00 traegt nur 0,2,3,4,8).

## RE-Belege

(folgt)

## Umsetzung

(folgt)

## Messung nachher

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
