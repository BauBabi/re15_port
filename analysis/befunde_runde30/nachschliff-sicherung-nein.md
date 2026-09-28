# Runde 30 / Nachschliff „sicherung-nein" — „No" im Sicherungs-Modal sperrt den Raumaufenthalt

Stand: Integrationsstand cac33993 (alle zehn Spuren der Runde 30), Zweig `r30/n-sicherung-nein`.
Vorgänger-Dossier: `sicherung.md` (§6.3 „Antwort No — nicht gemessen").

## 1. Symptom

Gegenprüfer, Lauf `g2/taste_nein`: Wer im Sicherungs-Modal des Hebetischs (ROOM1150/1151)
mit „No" antwortet, bekommt die Sicherung im selben Raumaufenthalt nie wieder angeboten. Der
Tisch fährt beim erneuten Auslösen wieder hoch, das Rohr ist zu sehen, aber es kommt kein Modal
(`Cut_chg(4)` bei F253 und F1186, Modal nur beim ersten Mal, Bild 383).

## 2. Messung vorher (Stand cac33993, nichts geändert)

### 2.1 Sonde `probe_r30_sicherung_nein` (Engine, Bild für Bild wie das Spiel)

Die Bildschleife folgt der Reihenfolge des Spiels: SCD-Tick nur ohne Modal (`main.c`, Freeze),
dann `re15_sicherung_tick` (`game_step_common.c:1040`, vor dem Freeze-Gate), dann der Modal-Tick.
Volle Ausgabe: `nachschliff-sicherung-nein_abnahme/sonde_vorher.txt`.

| Raum | Fall | Fahrt | Modale | Modal-Bild / y | geparkt Bild / y | Ergebnis |
|---|---|---|---|---|---|---|
| 1150 | A | 1 („No") | 1 | 134 / -1105 | 484 / -20224 | Flag 0, Prop sichtbar, Item nicht im Inventar |
| 1150 | A | 2 („Yes") | **0** | — | 395 / -20224 | **kein Modal** → Flag 0, kein Item |
| 1150 | A | 3 | 0 | — | 395 / -20224 | — |
| 1150 | B | 1 („Yes") | 1 | 134 / -1105 | 466 / -20224 | Flag 1, Prop weg, Item im Inventar |
| 1150 | B | 2 | 0 | — | 395 / -20224 | richtig |
| 1151 | A/B | | | | | Zeile für Zeile gleich 1150 |

`FEHLGESCHLAGEN — 6 Pruefung(en) gerissen` (je Raum Prüfung 5, 6, 7). Die Fahrt im Raum reicht in
allen Fahrten von y = -301 bis -1215; die Parklage nach der Szene ist -20224.

### 2.2 Echtes Spiel mit Tasten (`nachschliff-sicherung-nein_werkzeug/lauf.sh vorher`)

`re15_pc.exe` (Bau cac33993), beschleunigter Renderer, Bilder aus `RE15_FRAMEDUMP`, kein
`RE15_AUTOSHOT`/`RE15_SOFTWARE_RENDER`. ROOM1150 per Debug-Sprung, Spieler (-21000,-18500),
Eingabeskript `W1,A0.2,W8,R0.2,W0.5,A0.2,W22,A0.2,W8,A0.2,W4,S0.2,W4` auf Spielbildern ab
Bild 200 (A = Viereck, R = Rechts, S = Start). `debug.log`:

```
[input-script] Tick 30 -> F230 Tasten 0x8000        Viereck: Fahrt 1
[scd F236] Cut_chg(4)
[sicherung] Modal Item 0x40 zeichnet in Bild 366 (Raum 1150, Hebetisch y=-1105): ...
[input-script] Tick 276 -> F476 Tasten 0x0020        Rechts: auf "No"
[input-script] Tick 297 -> F497 Tasten 0x8000        Viereck: "No" bestaetigt
[input-script] Tick 963 -> F1163 Tasten 0x8000       Viereck: Fahrt 2
[scd] thread start slot=10 first_op=0x22
[scd F1169] Cut_chg(4)
[input-script] Tick 1209 -> F1409 Tasten 0x8000      Viereck: ins Leere
[input-script] Tick 1335 -> F1535 Tasten 0x0008      Start: Statusschirm
```

Kontaktbogen `nachschliff-sicherung-nein_abnahme/vorher_bogen.png`: F420/F480 Modal mit
„Yes ▸No", F510 Modal schrumpft weg; F1300/F1370/F1420 Fahrt 2, Plattform oben, Rohr sichtbar,
**kein Modal**; F1560/F1600 Statusschirm: Messer, Pistole, Munition — **keine Sicherung**.

Befund reproduziert, im Spiel wie in der Sonde.

## 3. Original-Mechanismus: was geschieht nach „No"?

### 3.1 RE1.5 — die Aufnahme-FSM `FUN_8001db28`, Zustand 7 (PSX.EXE, selbst disassembliert)

```
8001e048: lui  v0,0x8009
8001e04c: lb   v0,-2516(v0)         ; 0x8008f62c  freier Platz (Vorpruefung), <0 = voll
8001e054: bltz v0,0x8001e0ec        ; voll -> Zustand 8
8001e058: ori  v0,zero,0x8
8001e060: lbu  v0,-31456(v0)        ; 0x800b8520  Auswahl-Byte der Ja/Nein-Box
8001e068: andi v0,v0,0x1            ; Bit 0 = "No"
8001e06c: bne  v0,zero,0x8001e0ec   ; "No" -> Zustand 8
8001e070: ori  v0,zero,0x8
--- JA-Zweig ---
8001e078: lw   v1,-13776(v1)        ; v1 = [0x800aca30] = der ausloesende Zonen-Datensatz
8001e080: lbu  v0,1(v1)
8001e088: andi v0,v0,0x80
8001e08c: beq  v0,zero,0x8001e09c
8001e090: sb   zero,0(v1)           ; (Verzoegerungsplatz, laeuft IMMER) sce-Byte der Zone = 0
8001e0c4: jal  0x8004dc4c           ; INSERT ins Inventar
8001e0d0: jal  0x8004ef90           ; Genommen-Flag
8001e0e0: sb   zero,11579(at)       ; 0x80072d3b = 0  (fertig)
--- NEIN-/VOLL-Zweig ---
8001e0ec: sh   zero,-2512(at)       ; 0x8008f630 = 0
8001e0f8: sh   zero,-2508(at)       ; 0x8008f634 = 0
8001e100: sb   v0,11579(at)         ; Zustand 8 (Wegschrumpfen)
```

Zustand 8 (`@0x8001e10c`) schrumpft das Bild 17 Bilder lang (`sltiu v0,v0,0x11` @0x8001e154)
und endet mit `sb zero,11579(at)` @0x8001e16c — Zustand 0. **Kein Schreiber auf den
Zonen-Datensatz** in diesem Zweig.

Wer ist `[0x800aca30]`? Der Item-Handler (Tabelle `@0x8007469c`, Eintrag 9 = `0x80043328`):

```
80043328: lui  v0,0x8007
8004332c: lbu  v0,11579(v0)         ; 0x80072d3b  Zustand der Aufnahme-FSM
80043334: bne  v0,zero,0x80043368   ; laeuft schon -> nichts tun
80043338: ori  v1,zero,0x1
80043344: lw   a0,-16996(a0)        ; a0 = [0x800bbd9c]
8004334c: sb   v1,11579(at)         ; Zustand 1 = Start
80043364: sw   a0,-13776(at)        ; [0x800aca30] = a0
```

und `[0x800bbd9c]` schreibt der Zonen-Scanner `FUN_80042bac` für jeden Datensatz:
`@0x80042c50 lw s0,0(s4)` / `@0x80042c5c sw s0,-16996(at)`. Derselbe Scanner überspringt
Datensätze mit sce = 0:

```
80042f48: lbu  v0,0(s0)             ; sce-Byte
80042f50: beq  v0,zero,0x8004301c   ; 0 -> naechster Datensatz
```

**Regel RE1.5:** Nur „Yes" macht die Zone inert (`sb zero,0(v1)` @0x8001e090). Nach „No"
bleibt das sce-Byte stehen; der nächste Aktionsdruck in der Zone findet den Datensatz wieder
(@0x80042f48) und der Handler startet das Modal neu — solange die FSM wieder in Zustand 0 ist
(@0x80043334). **Ein Auslösen = höchstens ein Modal.**

### 3.2 RE2 Retail — die Aufnahme-Abfrage im Statusschirm, Art 2 (info/re2leon/PSX.EXE)

Die Item-Zone (`FUN_80051884`, Tabelle `@0x800a73c4` Eintrag 2) merkt sich die auslösende
Zone: `@0x800518b0 sw v1,-6696(at)` = `[0x800ce5d8]`. Die Abfrage läuft in `FUN_80072050`,
Sprungtabelle `@0x80011d64` über den Zustand `0x800d5bf2`:

```
[1] 800720b4: lbu  v1,-30916(v1)    ; 0x800e873c  Status der Ja/Nein-Box
    800720c0: andi v0,v1,0x80        ; Box laeuft noch?
    800720c4: bne  v0,zero,0x8007249c
    800720c8: andi v0,v1,0x1         ; Cursor = 1 = "No"
    800720cc: bne  v0,zero,0x80072194
    800720d0: addiu v0,zero,4        ; (Verzoegerungsplatz)
    ...
    80072194: j    0x8007249c
    80072198: sb   v0,2(s1)          ; "No" -> Zustand 4
[4..17] 80072424: Zustand += 1       ; 14 Bilder
[18] 800723f0: lui  a0,0x405
     800723f4: jal  0x8005ba28       ; Se_on Bank 4 / Satz 5 (Abbruch)
     80072400: sb   zero,1(s1)       ; 0x800d5bf1 = 0  Schirm zu
--- JA-Weg ---
[3]  8007227c: lw   v1,-6696(v1)     ; v1 = [0x800ce5d8] = die ausloesende Zone
     80072294: beq  v0,zero,0x800722a4
     80072298: sb   zero,0(v1)       ; (Verzoegerungsplatz) sce-Byte der Zone = 0
```

`0x800e873c` ist das Status-Byte der Nachrichten-Box: der Öffner `@0x8002fe38` legt die Basis
`0x800e2ab0` an (`@0x8002fe3c`) und setzt `@0x8002fe88 sb t2(=0x80),0x800e873c`; die
Ja/Nein-Box `FUN_80030844` liest dasselbe Byte als `+23692` (0x800e2ab0 + 0x5C8C = 0x800e873c),
das untere Halbbyte ist der Cursor (0 → Se 0x406, 1 → Se 0x405, `@0x80030930-44`), Bit 0x80
wird beim Bestätigen gelöscht (`@0x800308f8 andi v0,v0,0x7f`). Der Zonen-Scan überspringt
sce-0-Datensätze wie in RE1.5: `@0x800513d8 lbu v0,0(s0)` / `@0x800513e0 beq v0,zero`.

**Regel RE2:** identisch. „No" fasst die Zone nicht an; nur der Ja-Weg nullt sie
(`@0x80072298`). Erneutes Untersuchen öffnet die Abfrage wieder.

### 3.3 Übertragen auf die Sicherung (Port-Ergänzung)

Die Sicherung hat keine Zone: ihr Auslöser ist die **Fahrt** des Hebetischs (sub04
@0x0F96-0x10B6, ausgelöst über den Datensatz slot 1 @0x0D7E). Die Entsprechung der Original-Regel:

| Original | Sicherung |
|---|---|
| Aktionsdruck in der Zone | eine Fahrt von sub04 |
| „No" lässt die Zone scharf | „No" lässt die Sicherung liegen, die **nächste Fahrt** bietet sie wieder an |
| ein Auslösen = höchstens ein Modal (@0x80043334) | eine Fahrt = höchstens ein Modal |
| „Yes" nullt die Zone (@0x8001e090 / @0x80072298) | „Yes" setzt Flag (9,53) und blendet das Prop aus — keine weitere Aufnahme |

## 4. Ursache

`re15_port/engine/src/sicherung_1150.c` (Stand cac33993):

* `:34` `static uint8_t s_modal_ausgeloest;  /* in diesem Raumaufenthalt schon aufgemacht? */`
* `:65` zurückgesetzt **nur** in `re15_sicherung_install` (Raumstart),
* `:107` geprüft, `:123` gesetzt beim Öffnen.

Die Sperre galt je **Raumaufenthalt**. Das ist die richtige Sperre für „nach Yes" (dort greift
ohnehin das Flag, `:109`), aber die falsche für „nach No": das Original lässt eine abgelehnte
Aufnahme scharf.
