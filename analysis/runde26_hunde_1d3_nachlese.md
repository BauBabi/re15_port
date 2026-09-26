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
