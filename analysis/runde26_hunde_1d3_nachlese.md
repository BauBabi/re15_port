# Runde 26 — Nachlese der sechs "Port-Fehlstellen" (selbst nachgelesen)

Alle Adressen unten habe ich in dieser Runde SELBST disassembliert
(`re15_disasm.py dis … --bin <overlay>`, `RE_OVERLAY_DIR=info/re2leon/COMMON/BIN`,
Ladebasis 0x80100000, headerlos).

## Ergebnis: DREI der sechs sind falsch verortet

Das Zielregister des Stores entscheidet, WESSEN +0x1D3 geschrieben wird.
`s0` = der Gegner selbst, `s1`/`a0` = das OPFER.

| Behauptung (Dossier) | Store | Ziel | Urteil |
|---|---|---|---|
| S2 dog ACTIVE-Sub16 | `sb v0,467(s0)` @0x8010318C | **self** | stimmt |
| S5 dog Fress-Kette | `sb v1,467(s1)` @0x80104F34 | **OPFER** | falsch verortet |
| C1 dog Freigeber | `sb v0,467(s0)` @0x80100608 | **self** | stimmt |
| S7 dog HURT-Hilfe | `sb v1,467(a0)` @0x80105200 | **OPFER** | falsch verortet |
| Krähe Root-Setzer | `sb v1,467(s1)` @0x801004A8 (s1 = self im Root) | **self** | stimmt |
| Spinne 0x25/0x26-Hilfe | `sb v1,467(a0)` @0x80105F64 / @0x8010101C | **OPFER** | falsch verortet |

### Beleg S5 — die Fress-Kette schreibt auf das Opfer
`0x80104DF0` (Fress-Kette) legt die Register fest:
```
80104df8: addu  s0,a0,zero            ; s0 = der HUND
80104e00: addu  s2,a1,zero            ; s2 = Auswahl-Flag
80104e1c: addiu s1,s1,-1032           ; s1 = 0x800cfbf8  (Opfer = Spieler)
80104e20: beq   s2,zero,0x80104e30
80104e2c: lw    s1,-488(s1)           ; s1 = *(0x800cfe18) (Opfer = Entity-Zeiger)
...
80104f34: sb    v1,467(s1)            ; +0x1D3 = 0xFF  -> OPFER
80104f74: sb    v0,467(s0)            ; +0x1D3 |= 0x80 -> HUND  (Port hat das: dog.c:566)
```

### Beleg S7 — die "HURT-Eintritts-Hilfe" versetzt das OPFER in HURT
```
801051bc: beq   a0,zero,0x801051d4
801051c8: lw    a0,-488(a0)           ; a0 = *(0x800cfe18)
801051d8: addiu a0,a0,-1032           ; a0 = 0x800cfbf8  (Spieler)
801051dc: lbu   v1,467(a0)
801051e4: sb    v0,4(a0)   (v0=2)     ; state = 2 (HURT)
801051ec: sb    v0,5(a0)   (v0=a1+2)  ; sub  = Reaktion + 2
801051f8: ori   v1,v1,0x80
80105200: sb    v1,467(a0)            ; Pose-Riegel auf das OPFER
```
Aufrufer @0x801043F4 übergibt `a0 = 0` (`addu a0,zero,zero` @0x801043F0) ⇒ Spieler.
0x800cfbf8 = Spieler-Work (0x220 B), 0x800cfe18 = Entity-Zeiger-Feld — belegt über
`RE2_Quellcode_V2/FUN_80018e08.c:58` (`(&DAT_800cfe18)[uVar6 & 0xff]`) und
`FUN_80035f68.c:24` (`FUN_80038b84(&DAT_800cfbf8)`).

Die Spinnen-Hilfen sind baugleich: EMS25 @0x80105F20-64 identisch (Aufrufer @0x80105AA0,
`a0 = s2`), EMS26 @0x80100FFC-101C nimmt das Opfer direkt (`a0 = s1`, Aufrufer @0x80100EA0
mit `addu a0,s1,zero` @0x80100E9C; s1 = das Ziel von `jal 0x80015910`).

⇒ Der Port bildet die Opfer-Seite bereits ab: `enemy_ai_common.c:12363` liest
`pl->re2z_self1d3 & 0x80` (Greif-Sperre), `enemy_ai_common.c:1929` löscht sie.
Diese drei Stellen als GEGNER-Selbstschreiber zu bauen wäre ein Defekt.

## Die drei verbleibenden — Reichweite im Port

* **C1 @0x80100600-08** — Phase 0 von `0x80100548` (Idle-Wander, Subs 0 und 10).
  Im Port `enemy_ai_re2_dog.c:re2d_idle_wander case 0` — die Zeile FEHLT.
  Erreichbar, reiner Freigeber ⇒ **wird gebaut**.
* **S2 @0x80103188-8C** — Phase 0 von Sub 16. Sub-16-Verteiler @0x801030FC liest +0x6
  gegen die Tabelle @0x80105530, die nur ZWEI Einträge hat
  (`[0]=0x80103138 [1]=0x80102d98`); Phase 1 schreibt aber `sh 2,6` @0x80102DE4,
  also Phase 2 — deren Tabellenwort @0x80105538 ist bereits der Kopf der HURT-Zeilen-Tabelle
  (`[0]=0x8010321C`). Der Ausstieg der Phasen-Maschine ist damit aus den Tabellen NICHT
  auflösbar, und der Eintritt in Sub 16 hat im Port ohnehin keinen Produzenten
  (`enemy_ai_re2_dog.c:1529` dokumentiert das). ⇒ **wird NICHT gebaut** (wäre geraten).
* **Krähe-Root-Setzer @0x801004A0-A8** — hängt an `+0x10E & 0x40`
  (`lhu v0,270(s1)` @0x80100490 / `andi v0,v0,0x40` @0x80100498). Für Typ 0x21 füllt der
  Port +0x10E nirgends (die einzige Zuweisung `re2z_f10e = grid_id`
  `enemy_ai_common.c:10701` gilt nur für Typ 0x25); `enemy_ai_re2_crow.c:1765-69`
  dokumentiert das bereits als ohne Produzenten. Zudem ist der Store eine ZUWEISUNG
  (`addiu v1,zero,128` @0x801004A0), die die low-7-Trefferpause jedes Bild löschen würde.
  ⇒ **wird NICHT gebaut** (toter Zweig + Risiko ohne Messnutzen).

## Zensus selbst nachgezählt (Teil 2 des Pakets)

`re15_port/shared_assets/PSX` enthält **240** RDTs (STAGE1 80, STAGE2 32, STAGE3 32,
STAGE4 32, STAGE5 48, STAGE6 16) — die 206 des Dossiers sind falsch, die 240 des Prüfers
stimmen.

Muster `44 [00-1f] 20 41` (Sce_em_set Typ 0x20 grid 0x41) und `34 0c 42 00`
(Member_set(0x0C,66)):

```
ROOM11D0 em41=5 ms42=5      ROOM2060 em41=0 ms42=2
ROOM11D1 em41=5 ms42=5      ROOM2061 em41=0 ms42=2
ROOM3060 em41=3 ms42=3      ROOM20A0 em41=0 ms42=2
ROOM3061 em41=1 ms42=1      ROOM20A1 em41=0 ms42=2
                            ROOM4001 em41=0 ms42=2
SUMME em41=14  ms42=32      ROOM5040/5041/5070/5071 je ms42=2
```
⇒ **vier** betroffene Räume, **14** Hunde. STAGE3 trägt dieselbe Freigabe:
`STAGE3.BIN` @0x801101EC-0x8011020C ist Instruktion für Instruktion dieselbe Folge wie
`STAGE1.BIN` @0x8011170C-0x8011172C (selbst disassembliert).

Die Blut-Behauptung des Dossiers ("spritzt pro Bild") ist falsch: der Schwanz von
0x80102608 ist EDD-gegatet — `lw v0,376(s0)` @0x801027DC / `lw s1,0(v0)` @0x801027E4 /
`lui v0,0x3` @0x801027E8 / `and v0,s1,v0` @0x801027EC / `beq v0,zero,0x8010284C`
@0x801027F0; der zweite Spritzer zusätzlich hinter @0x80102814 und dem 1/4-Würfel
@0x80102828. Der Kommentar im Code sagt das jetzt so.

## Nebenbefund (NICHT repariert)

Der Hund zieht seine Trefferpause **zweimal je Bild** ab: einmal in `enemy_ai_common.c`
(die Zeile vor der 4/5/6-Weiche) und noch einmal im RE2-Root
`enemy_ai_re2_dog.c:2224` (@0x80100028-3C). Gemessen im Riegel-Lauf: +0x1D3 geht in EINEM
Tick von 0x85 auf 0x83. Für die Zustände 4..6 ist die erste Zeile richtig (dort kehrt der
Port vor dem Root zurück), für alle anderen Zustände halbiert sie die Pause. Das Original
zieht genau einmal ab.
