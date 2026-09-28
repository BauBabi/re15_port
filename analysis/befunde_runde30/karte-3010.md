# Runde 30, Thema B — Karte oeffnet sich nach der Irons-Cutscene (RE2-Mechanismus aus ROOM3010)

Stand: master 437905cb (v0.8.15). Phase ERMITTLUNG, kein Bau. Fortsetzung eines am
Sitzungslimit abgebrochenen Vorgaengers; seine Messdateien liegen unter
`build/r30_karte-3010/`, seine Werkzeuge unter `analysis/befunde_runde30/tools/r30_karte3010_*`.
Uebernommen wurde nur, was eine Stichprobe bestanden hat (Abschnitt 8).

Alle RE2-Adressen: `info/re2leon/PSX.EXE` (t_addr 0x80010000, Kopf 0x800).
Alle RE1.5-Adressen: `info/Re1.5/PSX.EXE` bzw. `info/Re1.5/PSX/BIN/DEBUG.BIN` (Auslieferungsstand).

---

## 1. Auftrag (woertlich, AUFTRAG.md Abschnitt B)

> In ROOM 3010 in Resident Evil 2 gibt es nach der Cutscene eine Stelle wo die Map aufgeht
> und zeigt, wo der Spieler hin gehen muss. Auch mit einen bestimmten Sound. So einen
> aehnlichen Mechanismus moechte ich bitte nach der Irons Cutscene in Irons Office im ROOM
> 1150 ebenfalls haben, der mir dann den communication ROOM markiert. Beim Schliessen dieser
> Karte kann der Room, wenn man dann die Karte erneut aufruft auf aehnliche Weise wie bei
> Resident Evil 2 damit umgegangen wird, auch bei uns damit umgegangen werden. Also entweder
> markiert bleiben, oder als besucht markiert bleiben, oder keine Ahnung - ich weiss nicht
> genau wie Resident Evil 2 das macht.

Kurzantwort auf die offene Frage des Nutzers: **RE2 merkt sich NICHTS.** Der Hinweis ist ein
eigener Statusschirm-Modus (Modus 4) mit eigenem Zeichner; sein einziger Parameter ist ein
Arbeitsbyte, das nur dieser Modus liest. Es wird kein Flag gesetzt, nichts gespeichert, und
die normale Karte zeigt den Zielraum danach genau so wie vorher (Abschnitt 3.7).

---

## 2. MESSUNG im Port (Ist-Zustand)

### 2.1 Sonde: was passiert am Ende von ROOM1150 sub08

Sonde `re15_port/tests/unit/probe_r30_karte-3010.c` (+ `probes/r30_karte-3010.cmake`), gebaut in
`re15_port/build_r30_karte-3010`. Sie laedt die echte `ROOM1150.RDT`, startet sub08 als
SCD-Thread und tickt bis 120 Bilder nach dem Thread-Ende. Ausgabe
`build/r30_karte-3010/probe_messung.txt`; von mir erneut gefahren
(`probe_messung_wdh.txt`) — alle `[M*]`-Zeilen bitgleich zur Vorgaenger-Messung:

```
[M0] ROOM1150.RDT 194080 B; Schwanz @0x012E0: 42 00 3c 01 22 02 07 00 22 01 1b 00 01 00  -> wie zitiert
[M0] sub08-Zeiger der RDT: Datei 0x1106 (erwartet 0x1106), sub_scd_count=9
[M0] 14-Byte-Schwanz in ROOM1150.RDT: 1 Treffer
[M1] vor der Szene: flag(3,94)=0 flag(1,27)=0 flag(2,7)=0 menu_stage=0 offen=0
[M2] sub08-Thread beendet in Bild 1564; letzter pc im sub08-Bereich = 0x12E0 (Evt_end steht @0x12EC)
[M2] Szenen-Flags gefallen (cine==0) in Bild 1564; flag(3,94)=1
[M3] Statusschirm angefordert/geoeffnet waehrend Szene + 120 Bilder Nachlauf: NEIN (erstes Bild -1; menu_stage=0 offen=0 substate=0)
```

Befund: die Szene laeuft 1564 SCD-Ticks, faellt in EINEM Tick von `Plc_ret` @0x12E0 bis
`Evt_end` @0x12EC durch, und danach fordert nichts den Statusschirm an.

### 2.2 Echte exe (Framedump, kein AUTOSHOT, kein SOFTWARE_RENDER)

`analysis/befunde_runde30/tools/r30_karte3010_szene.sh` (Spielstand-Kopie, Debug-JUMP 1150,
`RE15_FORCE_EVENT=8@300`): `build/r30_karte-3010/szene_1150.debug.log`. Die `[walk]`-Zeilen
laufen nach dem Szenenende mit `frz=0` weiter (F1710 ... F2880, cut=5, Leerlauf-Clips
211/212); keine Zeile des Inventars (`[inv]`) im ganzen Lauf. Der Spieler steht frei im Raum,
die Karte geht nicht auf.

### 2.3 Wie die Karte im Port heute aussieht

`r30_karte3010_lauf.sh` (RE15_INV_FB_SHOT = der CPU-gerasterte Statusschirm):

| Abzug | Inhalt |
|---|---|
| `build/r30_karte-3010/ist_karte_im_1150.png` | Karte im ROOM1150, wie der Port sie von selbst zeigt: Blatt 4 "POLICE STATION 3F", Irons' Buero dunkelrot (aktueller Raum) |
| `build/r30_karte-3010/ist_blatt3_aufgedeckt.png` | Blatt 3 "POLICE STATION 2F", per Messschiene aufgedeckt |
| `build/r30_karte-3010/ist_blatt3_rect9_markiert.png` | dasselbe, Rechteck 9 gelb umrandet = die Kachel des Funkraums ROOM10F0 |

Der Zielraum liegt also auf einem ANDEREN Blatt (3 = 2F) als der Spieler (4 = 3F).

### 2.4 Die Skriptstelle (ROOM1150.RDT, selbst gelesen mit `tools/scd_dump_room.py`)

`build/r30_karte-3010/re15_room1150_scd.txt`:

```
main00 @0x00DE2  06 00 1a 00        Ifel_ck
       @0x00DE6  21 03 5e 00        Ck(3,94)==0
       @0x00DEA  2c 06 03 41 ... ff 00 18 08   Aot_set slot 6, AUTO (flags 0x41), Evt_exec sub 8
sub08  @0x01106  46 06 00 ...       Aot_reset(6)            ; Zone schaltet sich selbst ab
       @0x01110  22 03 5e 01        Set(3,94,1)             ; Szene gelaufen (am ANFANG gesetzt)
       @0x01114  22 02 07 01 / @0x01118 22 01 1b 01          ; Szenen-Flags an
       ...  Message_on msg 4..14, Dialog Leon/Irons ...
       @0x012E0  42                 Plc_ret
       @0x012E1  00                 Nop
       @0x012E2  3c 01              Cut_auto(1)
       @0x012E4  22 02 07 00        Set(2,7,0)
       @0x012E8  22 01 1b 00        Set(1,27,0)
       @0x012EC  01 00              Evt_end                  <<< hier gehoert der Hinweis hin
```

Der Dialog nennt das Ziel selbst (Textblock RDT+0x3C @0x012F8, dekodiert mit
`r30_karte3010_msg.py`): Irons: "Maybe using the communication system... you can ask for
outside help" ... "Take this card... it will allow you to enter the communications room...
now go".

ROOM1151 (Elza-Variante) hat diese Szene NICHT: `re15_room1151_scd.txt` zeigt 8 Subs
(sub00..sub07), kein sub08, keine AUTO-Zone Slot 6, kein `Ck(3,94)`.

---

## 3. ORIGINAL-MECHANISMUS (RE2 Retail, Leon)

RE1.5 hat diesen Mechanismus nicht: seine SCD-Tabelle endet bei Opcode 0x5E (95 Eintraege,
Memory `reai-v2-scd-opcode-tabelle-95`), RE2s Hinweis-Opcode ist 0x84. Nach der Regel
Beta->Retail ist deshalb RE2 das Ziel, und jeder Beleg unten kommt aus RE2.

### 3.1 Der Opcode: 0x84, Operand = Hinweis-Nummer

Zensus ueber alle 250 Leon-RDTs (`r30_karte3010_hint_zensus.py`,
`build/r30_karte-3010/re2_hint_zensus.txt`): 5 Records, davon 4 in sauber gelaufenen Bloecken.

| RDT | Block | Datei-Offset | Bytes | davor | danach |
|---|---|---|---|---|---|
| ROOM3010 | sub02 | 0x026EE | `84 02` | `29 01` Cut_chg(1), `18 0c` | `01 00` Evt_end |
| ROOM3040 | sub24 | 0x01BB0 | `84 04` | `22 02 07 00`, `22 01 1b 00`, 10 Set | `09` |
| ROOM30B0 | sub15 | 0x01A86 | `84 01` | `42` Plc_ret, `22 02 07 00`, `22 01 1b 00`, `3c 01` | `01 00` Evt_end |
| ROOM6030 | sub21 | 0x035C8 | `84 03` | `29 06` Cut_chg, Sce_bgm..., `18 18` | `01 00` Evt_end |
| ROOMB0B0 | sub02 | 0x03324 | `84 03` | — | Block desynchron (op 0xD8 @0x7A): NICHT BELEGT, ob das Code ist |

Selbst nachgelesen: `xxd -s 0x26EE -l 4 ROOM3010.RDT` = `84 02 01 00`.
ROOM3010 ist Bens Zelle (Raumtext msg 0 "He's not talking.", msg 2 "The body has been torn
apart. Something appears to have burst out from inside...", Textblock RDT+0x40 @0x03E60).

**Lage im Skript:** in drei von vier Faellen steht der Record UNMITTELBAR vor `Evt_end`, in
ROOM30B0 zusaetzlich direkt hinter dem Abbau der Szenen-Flags (2,7)/(1,27) — dieselbe
Schwanzform wie ROOM1150 sub08.

### 3.2 Der Handler @0x800591C4 (Tabelle 0x800A74C8 + 0x84*4 = @0x800A76D8 -> 0x800591C4)

`build/r30_karte-3010/re2_op84_handler_800591c4.dis.txt` (von mir disassembliert):

```
800591c4: lui a1,0x800d / lw a1,-1060(a1)   ; a1 = [0x800CFBDC]  (Pausen-Maske)
800591cc: lui v1,0x800d / lw v1,-1064(v1)   ; v1 = [0x800CFBD8]  (System-Flags)
800591d4: addiu v0,zero,4
800591dc: sb v0,23552(at)                   ; [0x800D5C00] = 4   STATUSSCHIRM-MODUS 4 = Hinweis
800591e0: addiu v0,zero,1
800591e8: sb v0,-3256(at)                   ; [0x800DF348] = 1   Anforderungs-Phase 1
800591ec: ori v1,v1,0x8000                  ; Bit 0x8000 = "Statusschirm angefordert"
800591f4: sw a1,-3188(at)                   ; [0x800DF38C] = alte Pausen-Maske (Sicherung)
800591fc: sw v1,-1064(at)                   ; [0x800CFBD8] |= 0x8000
80059200: lw v0,28(a0) / 80059208: lbu v0,1(v0)
80059210: sb v0,27122(at)                   ; [0x800D69F2] = pc[1] = HINWEIS-NUMMER
80059214: lw v0,28(a0) / addiu v0,v0,2 / sw v0,28(a0)   ; pc += 2  (Satzlaenge 2)
80059224: jr ra / addiu v0,zero,1
```

Der Handler setzt also genau vier Dinge: Modus (4), Hinweis-Nummer (Operand), die
Anforderung (Bit 0x8000 + Phase 1) und die Sicherung der Pausen-Maske. **Blatt, Zielraum,
Blinken und Ton setzt er NICHT** — die kommen erst aus dem Statusschirm (3.4/3.5).

### 3.3 Der Weg von der Anforderung zum Schirm (Spielschleife)

`build/r30_karte-3010/re2_gameloop_80025840.dis.txt`:

| Stelle | Instruktion | Bedeutung |
|---|---|---|
| @0x800263B0 | `jal 0x80053644` | der SCD-Laeufer (liest die Opcode-Tabelle 0x800A74C8 @0x8005366C) |
| @0x800263FC-0x80026404 | `lbu v0,[0x800DF348]` / `bne v0,zero,0x80026540` | Phase != 0 -> der Tasten-Oeffnen-Zweig wird UEBERSPRUNGEN |
| @0x8002652C-30 | `lui a0,0x406` / `jal 0x8005ba28` | Oeffnen-Ton Se(4,6) — liegt IM uebersprungenen Zweig |
| @0x8002594C-68 | `sltiu v0,v1,0x7` / Sprungtabelle @0x800109C4 | Phasen-Verteiler auf [0x800DF348] |
| @0x800259B0-B4 | `andi v1,v1,0x8000` / `bne -> 0x80025A80` | Phase 1: Anforderung steht |
| @0x80025A80-9C | `FUN_8002C1A0(0x200, 0x1800, 7, 1)`, Phase = 2 | Spielbild blendet AUS |
| @0x80025B8C-94 | `lui a1,0x8007 / addiu a1,a1,-30276` = 0x800689BC, `jal 0x80031F6C(1, ...)` | Status-Task wird gestartet |
| @0x80025BA8-E0 | `[0x800DF38C]` zurueck, `[0x800CFBD8] &= ~0x40 & ~0x8000` | **die Anforderung wird hier GELOESCHT** |

Folge: beim Hinweis faellt der normale Oeffnen-Ton Se(4,6) NICHT — der Handler hat die Phase
schon selbst auf 1 gesetzt, der Zweig mit dem Ton wird uebersprungen.

Status-Task `0x800689BC`: setzt den VSync-Teiler `[0x800DFC1A] = 0` (@0x80068A1C; gelesen
@0x8002B994 als Argument von `jal 0x80085EA0` = VSync, libetc, liest GPU_REG1 1F801814) — der
Statusschirm laeuft also mit **einem Durchgang je VBlank (NTSC 59,8 Hz)**, das Spiel sonst
mit Teiler 2 (`addiu v0,zero,2` @0x80068E6C -> `sb` @0x80068E74 beim Verlassen).
Verteiler @0x80068C70-A0: `lw [0x800A936C + Modus*12 + Zustand*4]` / `jalr`.

| Modus `[0x800D5C00]` | Init | Lauf | Ende |
|---|---|---|---|
| 0 Status | 0x8006A574 | 0x8006A774 | 0x80068CD4 |
| 1 | 0x8006F900 | 0x8006FB64 | 0x80068CD4 |
| 2 | 0x80071BA0 | 0x80071E14 | 0x80068CD4 |
| 3 Karte zum aktuellen Raum (Blatt aus `jal 0x8006e7f0` @0x8006F008) | 0x8006EFD8 | 0x8006F164 | 0x80068CD4 |
| **4 Hinweis** | **0x8006F6A8** | **0x8006F8B4** | 0x80068CD4 |

(Tabelle @0x800A936C von mir gelesen; Eintrag 5 ist schon Fremddaten `0x2, 0xf0f0f0f0`.
Schreiber des Modus-Bytes mit festem Wert: 2 @0x8003E25C, 2 @0x800518F8, 1 @0x80051DD8,
2 @0x8005254C, **4 @0x800591DC** — der Opcode 0x84 ist der EINZIGE, der Modus 4 anfordert.
Wer Modus 3 anfordert, ist NICHT BELEGT.)

### 3.4 Hinweis-Init @0x8006F6A8

`build/r30_karte-3010/re2_hintmode_8006f6a8.dis.txt`:

```
8006f6b4: addiu v1,zero,1
8006f6bc: sb v1,0(a1)            ; [0x800D5BF0] = 1   Zustand -> Lauf
8006f6c4: sb v1,23577(at)        ; [0x800D5C19] = 1   Blink-RICHTUNG
8006f6cc: lbu v1,27122(v1)       ; v1 = [0x800D69F2]  Hinweis-Nummer
8006f6d0: addiu v0,zero,2
8006f6d8: sb v0,23537(at)        ; [0x800D5BF1] = 2   Unterzustand "wartet auf Taste"
8006f6dc: addiu v0,zero,10
8006f6e4: sh zero,23624(at)      ; [0x800D5C48] = 0   Karten-Verschiebung x
8006f6ec: sh zero,23626(at)      ; [0x800D5C4A] = 0   Karten-Verschiebung y
8006f6f4: sb v0,23576(at)        ; [0x800D5C18] = 10  Blink-ZAEHLER
8006f6f8: sltiu v0,v1,0x5        ; Hinweis < 5
8006f70c: lw v0,7456(at)         ; Sprungtabelle @0x80011D20[Hinweis]
8006f71c: j 0x8006f738 / addiu v0,zero,5     ; Hinweis 0
8006f724: j 0x8006f738 / addiu v0,zero,5     ; Hinweis 2
8006f72c: j 0x8006f738 / addiu v0,zero,16    ; Hinweis 3
8006f734: addiu v0,zero,3                    ; Hinweis 1 und 4
8006f738: sb v0,26(a1)           ; [0x800D5C0A] = KARTENBLATT
8006f73c..0x8006f818             ; Blattgrafik laden (Datei 171, Blatt-Tabelle 0x800AAA38), LoadImage
8006f824..30: FUN_8002C1A0(0x200, -0x1800, 7, 1)      ; EINBLENDEN
8006f838: jal 0x80031f94(1)      ; ein Bild warten
8006f840: jal 0x8006f1c4         ; Hinweis-ZEICHNER
8006f848: jal 0x8007526c
8006f850: jal 0x8002c350(0) / 8006f858: beq v0,zero,0x8006f838   ; bis die Blende fertig ist
8006f85c: lui a0,0x409 / 8006f860: jal 0x8005ba28    ; Se(4,9)  = "Karte ist da"
```

Hinweis-Tabellen (selbst aus der EXE gelesen):

| Hinweis | Sprungziel @0x80011D20+4n | Blatt | Zielraum-Byte @0x800A9BA0+n | gesetzt in |
|---|---|---|---|---|
| 0 | 0x8006F71C | 5 | 0x16 | (kein Skript gefunden) |
| 1 | 0x8006F734 | 3 | 0x0A | ROOM30B0 |
| 2 | 0x8006F724 | 5 | 0x1A | **ROOM3010** |
| 3 | 0x8006F72C | 16 | 0x07 | ROOM6030 |
| 4 | 0x8006F734 | 3 | 0x08 | ROOM3040 |

Das Blatt ist also **fest je Hinweis** und unabhaengig davon, wo der Spieler steht.

### 3.5 Hinweis-Zeichner FUN_8006F1C4

`build/r30_karte-3010/re2_FUN_8006f1c4.dis.txt`. Aufrufer (Xref `jal`): nur @0x8006F840
(Init) und @0x8006F8E0 (Lauf) — der Zeichner gehoert ALLEIN dem Modus 4.

**(a) Blinkzaehler + Ton, je Durchgang:**

```
8006f20c: lbu v0,[0x800D5C19]            ; Richtung
8006f218: beq v0,zero,0x8006f258
8006f224: lbu v0,[0x800D5C18]            ; Zaehler
8006f22c: sltiu v0,v0,0xa                ; Zaehler < 10 ?
8006f230: beq v0,zero,0x8006f248
8006f234: lui a0,0x22b
8006f238: jal 0x8005ba28                 ; Se(2,0x2B)  = DER HINWEIS-TON
8006f244: sb zero,[0x800D5C19]           ; Richtung = 0
8006f24c: lbu v0,[0x800D5C18] / 8006f254: addiu v0,v0,-2     ; Zaehler -= 2
8006f258: lbu v1,[0x800D5C18]
8006f264: sltiu v0,v1,0x51               ; Zaehler < 81 ?
8006f268: bne v0,zero,0x8006f280 / 8006f26c: addiu v0,v1,2   ; Zaehler += 2
8006f270: addiu v0,zero,1 / 8006f278: sb v0,[0x800D5C19]     ; sonst Richtung = 1
8006f284: sb v0,[0x800D5C18]
```

Tick fuer Tick nachgebildet (`r30_karte3010_blink_sim.py`, `re2_blink_sim.txt`), Start
Zaehler 10 / Richtung 1:

```
Ton-Ticks (1-basiert ab Init): [2, 80, 158, 236, 314, 392]      Abstand 78
Zustandswechsel (Tick, CLUT-Y): (1,498) (2,502) (41,498) (80,502) (119,498) ...
Dauer der Phasen in Ticks: 39 / 39 / 39 ...     Zaehler min/max: 6 / 84
```

In Zeit (ein Tick = ein VBlank, 3.3): Phase 39 Ticks = **0,65 s**, Periode 78 Ticks =
**1,30 s**, der Ton faellt am Anfang jeder ROTEN Phase, erstmals im 2. Durchgang.

**(b) Zielraum bestimmen:**

```
8006f2ac: lbu v0,[0x800D69F2]            ; Hinweis-Nummer
8006f2b4: lbu a0,[0x800D5C0A]            ; Blatt
8006f2c4: lbu a1,-25696(at)              ; a1 = [0x800A9BA0 + Hinweis]  = Raumnummer
8006f2c8: jal 0x8006eae8                 ; (Blatt, Raum) -> Satzindex auf dem Blatt
8006f2dc: sw v0,16(sp)
```

`FUN_8006EAE8` (`re2_roomlookup_8006eae8.dis.txt`) ist ein fest verdrahteter Schalter je Blatt
(Sprungtabelle @0x80011CD0, 20 Blaetter); derselbe Aufruf steht im normalen Zeichner
@0x8006E1AC mit der AKTUELLEN Raumnummer `[0x800D481E]`.

**(c) Farbe des Zielraums** (Raumschleife @0x8006F34C-0x8006F628, Satz 16 Byte,
`+12` Besucht-Bit, `+13` Bit der Bank 0x800D4920, `+14/+15` Szenario-Bedingung):

```
8006f4c8: lbu a1,13(s0) / jal 0x80077360 (a0 = 0x800D4920) / addiu s2,zero,501
8006f4d4: beq v0,zero,... / 8006f4dc: addiu s2,zero,506
8006f4e0: lw a2,16(sp)
8006f4e8: bne s4,a2,0x8006f518           ; nur der ZIELRAUM:
8006f4f4: lbu v0,[0x800D5C19]
8006f4fc: beq v0,zero,0x8006f510
8006f504: lbu a1,13(s0) / j 0x8006f5d8   ;   Richtung != 0 -> @0x8006F5DC addiu s2,zero,498 (bzw. 503)
8006f510: j 0x8006f604 / addiu s2,s2,1   ;   Richtung == 0 -> 501+1 = 502 (bzw. 507)
8006f604: addiu a0,zero,256 / 8006f608: jal 0x8008f828 (GetClut(256,s2)) / ... jal 0x8008f918
```

Der Zielraum wechselt also zwischen **CLUT-Zeile 502** und **CLUT-Zeile 498**. Das sind
dieselben Zeilen, die der Port schon fuehrt (`analysis/karte_2026-09-27/umsetzung.md`,
`ST0.TIM` zweites TIM @0x10820):

| CLUT-Y | Eintrag 1 | Datei-Offset ST0.TIM | im normalen Zeichner | im Port |
|---|---|---|---|---|
| 502 | `0x842D` = 680808 + STP (dunkelrot) | 0x109B6 | aktueller Raum (@0x8006E648) | `RE15_INV_CLUT_MAP_AKTUELL` |
| 498 | `0x0000` durchsichtig, nur Wandlinie | 0x10936 | Karte da, Raum unbesucht (@0x8006E71C) | `RE15_INV_CLUT_MAP_UNBESUCHT` |

(Die Varianten 503/506/507 gelten fuer Raeume, deren Bit `+13` in der Bank 0x800D4920
gesetzt ist. Was diese Bank bedeutet, ist NICHT BELEGT; der Port fuehrt sie nicht.)

**Der Zielraum wird IMMER gezeichnet** — beide Zweige enden im Zeichnen (@0x8006F604/608),
ohne dass Besucht-Bit oder Kartenbesitz gefragt werden. Alle UEBRIGEN Raeume des Blatts
folgen der normalen Regel (@0x8006F518-0x8006F600). Der Zweig ist gleich AUFGEBAUT wie
@0x8006E658-0x8006E748 im normalen Zeichner — dieselben Pruefungen in derselben Reihenfolge
(Besitz-Bit `[0x800AAA3D + Blatt*8]` in Bank 0x800D4924, Besucht-Bit `+12` in Bank
0x800D490C, Szenario-Byte `+14/+15` gegen `[0x800CFBD8] & 0x40000000`, Bit `+13` in Bank
0x800D4920) —, aber NICHT wortgleich: der Vergleich der Instruktionsfolgen deckt 40 von 59
(die Basisadressen werden anders gebildet). Ergebnis: besucht -> 501, Karte im Besitz und
unbesucht -> 498, sonst nichts.

**(d) Was der Hinweis-Zeichner NICHT hat:**

| fehlt | Beleg |
|---|---|
| Spielerpfeil | die Spielerlage `[0x800CFC30]`/`[0x800CFC38]` wird im ganzen Kartencode nur @0x8006E1E4/@0x8006E1F8 gelesen (normaler Zeichner); im Bereich 0x8006F1C4-0x8006F900 kein Zugriff (`r30_karte3010_xref.py ref` + `memscan`) |
| Hervorhebung des EIGENEN Raums | im Zeichner gibt es genau EINEN Indexvergleich, `bne s4,a2` @0x8006F4E8 gegen den Zielraum; der normale hat `bne s2,a3` @0x8006E640 gegen den aktuellen |
| Flag-Schreiber | alle `jal` im Bereich 0x8006F1C4-0x8006F900 (`r30_karte3010_jals.py`, `re2_hint_jals.txt`): 13 x `0x80077360`, 0 x `0x8007730C`, 0 x `0x80077334`; ebenso 0 in den Gerufenen 0x8006DCC0-0x8006E120 und 0x8007526C |

`0x8007730C` = Bit SETZEN (`or` @0x80077328), `0x80077334` = Bit LOESCHEN (`nor`/`and`
@0x80077350-54), `0x80077360` = Bit PRUEFEN (`and` @0x80077380) — von mir disassembliert
(`re2_flagfuncs_8007730c.dis.txt`).

### 3.6 Lauf + Schliessen

```
8006f8b4: lbu v0,[0x800D5BF1] / sll 2 / lw v0,[0x800A9BA8 + ...] / jalr v0   ; Unterzustand
8006f8e0: jal 0x8006f1c4 / 8006f8e8: jal 0x8007526c                          ; zeichnen

Unterzustand 2  @0x8006F878:
8006f87c: lw v0,[0x800CE310]          ; Tasten-Flanken
8006f884: andi v0,v0,0x6000
8006f888: beq v0,zero,...
8006f890: lui a0,0x405 / jal 0x8005ba28     ; Se(4,5)  Abbruch-Ton
8006f8a0: sb zero,[0x800D5BF1]              ; -> Unterzustand 0
Unterzustand 0  @0x80068F08:  FUN_8002C1A0(0x200, 0x1800, 7, 1), [0x800D5BF1] = 1   ; ausblenden
Unterzustand 1  @0x80068F40:  warten bis FUN_8002C350(0), dann @0x80068F88 [0x800D5BF0] = 2
Ende            @0x80068CD4:  @0x80068E30 sb zero,[0x800D5C00]  (Modus zurueck auf 0),
                              @0x80068E74 [0x800DFC1A] = 2, Task beendet sich
```

Die Maske 0x6000: Bit 0x4000 ist die Statusschirm-Taste (dieselbe Flanke oeffnet ihn in der
Spielschleife, `andi v0,v0,0x4000` @0x800264A0), Bit 0x2000 ist Abbruch (im normalen Status
`andi v0,v0,0x2000` @0x8006A928 direkt vor Se(4,5) @0x8006A930). Es gibt im Hinweis KEINE
andere Eingabe: kein Blaettern, kein Verschieben, kein Wechsel in die normale Karte.

### 3.7 Nach dem Schliessen — Setzer, Loescher, Speichern

Alle Zugriffe auf die Hinweis-Nummer `[0x800D69F2]` in der ganzen EXE (Xref + memscan):

| Adresse | Zugriff | wer |
|---|---|---|
| @0x80059210 | `sb v0` | SETZER: Opcode 0x84 |
| @0x800687FC | `sb zero,3586(s3)` | LOESCHER: Grundstellung des Statusschirms (im selben Block @0x800687F0 `sb zero,16(s3)` = Modus 0) |
| @0x8006F2B0 | `lbu` | Hinweis-Zeichner |
| @0x8006F6CC | `lbu` | Hinweis-Init |

Der normale Kartenzeichner (0x8006E120) liest das Byte nicht. Die Anforderung (Bit 0x8000)
ist schon beim Start des Status-Tasks geloescht (3.3), der Modus beim Ende (3.6).
**Es bleibt nach dem Schliessen nichts zurueck**: kein Flag (3.5 d), keine Marke, kein
Besucht-Bit. Ruft der Spieler danach die Karte selbst auf, zeichnet der normale Zeichner den
Zielraum nach seiner gewoehnlichen Regel — unbesucht heisst: mit Karte als Umriss (498), ohne
Karte gar nicht.
Gespeichert wird folglich auch nichts; dass `[0x800D69F2]` ausserhalb des Speicherblocks
liegt, habe ich NICHT eigens nachgewiesen (Abschnitt 7) — es ist fuer das Ergebnis ohne
Belang, weil nach dem Schliessen ohnehin niemand mehr das Byte liest.

### 3.8 Die Toene

`FUN_8005BA28(a0,a1)`: `srl t1,a0,24` @0x8005BA30 = BANK, `srl v0,a0,16` @0x8005BA7C
(& 0xff) = SATZ; Bank 2 = Raumbank (EDT RDT+0x08, VH RDT+0x0C, VB RDT+0x10; Lader
FUN_80059E54, Dossier `tuer-verschlossen.md` §3.2). Tonhoehe = Tone-Byte +6
(`lbu v0,6(s0)` @0x8005BBEC), Feinstimmung +5 (@0x8005BBF8), Lautstaerke +2 (@0x8005BC04).

| Ton | wann | Aufrufstellen in der EXE | Quelle |
|---|---|---|---|
| Se(4,9) | Karte fertig eingeblendet | 2: @0x8006F148 (Modus 3) und @0x8006F85C (Modus 4) | CORE00, Satz 9 |
| Se(2,0x2B) | Anfang jeder roten Phase, alle 78 Ticks | 1: @0x8006F234 | Raumbank, Satz 0x2B |
| Se(4,5) | Taste, Schliessen | 16, u.a. @0x8006F890 | CORE00, Satz 5 |
| Se(4,6) | — faellt beim Hinweis NICHT (3.3) | 11 | CORE00, Satz 6 |

(Zaehlung: `r30_karte3010_se_sites.py`, `re2_se_sites.txt`.)

**CORE00 ist zwischen RE1.5 und RE2 byte-gleich** (md5 von mir gerechnet):
`CORE00.EDH` 9b0e0627500b50eaca5f8bc4124635d9, `CORE00.VB` cdcb61fb58d9ebfcf3352757674f7a6e —
gleich in `info/re2leon/COMMON/SOUND/`, `info/Re1.5/PSX/SOUND/` und
`re15_port/shared_assets/PSX/SOUND/`. Satz 9 @EDH 0x24 = `00 00 a3 01` -> Programm 0 Ton 10,
VAG 12 @VB 0x9AA0, 880 B (`r30_karte3010_core_satz.py`, `re2_core00_saetze.txt`). RE1.5
benutzt Satz 9 selbst einmal (@0x8004A154, `re15_se_sites.txt`). Fuer Se(4,9) und Se(4,5)
ist also NICHTS zu importieren.

**Der Hinweis-Ton Se(2,0x2B)** (`r30_karte3010_hint_se.py`, `re2_hint_se.txt`; die drei
Kopfworte, der Satz und der Hash von mir nachgelesen):

| | ROOM3010.RDT |
|---|---|
| Kopf +0x08/+0x0C/+0x10 | EDT 0x1F778, VH 0x1F838, VB 0x20658 |
| EDT[0x2B] @0x1F824 | `00 01 23 00` -> Programm 1, Ton 2, Prio 3 |
| Tone @0x20298 | `00 00 50 40 55 00 3d 3d ...` vol 80, pan 64, center 85, shift 0, min = max = 61, ADSR 0x80FF/0x1FC0, VAG 18 |
| Welle | VAG 18 @0x39788, 4480 B, sha1 eb386970f9a996369889d101b68314681138ff99 |
| Tonhoehe | Note 61 gegen center 85 = -24 Halbtoene -> 44100/4 = 11025 Hz, 7812 Samples = 0,709 s |

Dieselbe Welle (sha1 bitgleich) liegt in allen vier Hinweis-Raeumen auf Satz 0x2B
(ROOM3010 VAG 18, ROOM3040 VAG 12, ROOM30B0 VAG 15, ROOM6030 VAG 20), jeweils mit
center - min = 24. RE2 haelt den Ton also nicht global vor, sondern legt ihn in jede
Raumbank, deren Skript einen Hinweis ausloest. Abgehoert werden kann er als
`build/r30_karte-3010/hint_se_ROOM3010.wav`.

### 3.9 Gegenstandsmarken FUN_8006DCC0 (Umfeld des Zeichners)

Der Hinweis-Zeichner ruft am Ende `jal 0x8006dcc0` (@0x8006F66C), ausser wenn
`[0x800CFBD8] & 0x02000000` steht (@0x8006F654-64). `build/r30_karte-3010/re2_itemmarks_8006dcc0.dis.txt`:
14 Marken (`addiu s4,zero,14` @0x8006DCDC), Tabelle 0x800A9B10 (Felder gelesen als
0x800A9B1C/1D/1E + Index), gezeichnet nur mit Kartenbesitz (Bank 0x800D4924, @0x8006DD70-74)
und solange der Gegenstand nicht genommen ist (Bank 0x800D4A34 = 0x800D4924+272,
@0x8006DDB4-BC). Ihre Helligkeit folgt derselben Blink-Richtung `[0x800D5C19]`
(@0x8006DD3C: 128 bei Richtung != 0, sonst 80). Das sind GEGENSTAENDE, nicht das Ziel — der
Zielraum wird allein ueber die CLUT-Zeile markiert (3.5 c). Der Port hat keine
Gegenstandsmarken (`analysis/karte_2026-09-27/umsetzung.md`, "Was NICHT gebaut ist").

---

## 2b. MESSUNG im Port, zweiter Teil (Zielraum, Ton, Takt)

### 2.5 Welcher Raum ist der "communication room"

| Frage | Befund | Beleg |
|---|---|---|
| Raumname | **ROOM10F0 = "COMMUNIC. ROOM"** | DEBUG.BIN @Datei 0x027C0: `01 00 d0 20 a2 fe 00 00` + "COMMUNIC. ROOM" (`xxd`); Satzindex (637*Stage + 13*Raum)*2 @0x8001D39C-C8, Basis 0x800C263C @0x8001D3D0 (von mir disassembliert); ganze Tabelle `re15_raumnamen.txt` |
| ROOM10D0 | ist NICHT der Funkraum, sondern "2F CORRIDOR" mit der TUER zum Funkraum | DEBUG.BIN @0x0278C; `tuer-verschlossen.md` §2.2 meint die Tuer |
| die Tuer | ROOM10D0 main00 @0x01052 / @0x01174 `3b .. d0 20 00 00 a2 fe 00 08 00 0f` = Ziel (8400,0,-350), Stage 0, Raum 0x0F | `scd_dump_room.py ROOM10D0.RDT`; (8400,-350) ist zugleich der Debug-Spawn von ROOM10F0 |
| verschlossen solange | Flag (3,50) == 0 (`21 03 32 00` @0x01026); dann Text msg 6 "Communication Room / It's electronically locked / There's a card reader on the right" | ebd. |
| aufgeschlossen durch | Kartenleser sub20 @0x019AE: braucht Flag (9,52) (`21 09 34 01` @0x019C0) = Aufnahme-Bit des Items 0x38 "Blue Keycard" in ROOM1110 (`50 08 09 31 ... 38 00 01 00 34 00` @0x00B2E) | `re15_item_aot_zensus.txt` (83 Item_aot_set in allen Leon-Raeumen) |

⛔ Berichtigung zur Auftragsnotiz "Item 0x42 Communications Card": die Namenstabelle
(Offsettabelle @0x800C495C, Blob @0x800C4A28 in DEBUG.BIN) fuehrt unter 0x42 "Key Disc" und
unter **0x43 "Communications Card"** (Name @Datei 0x4DA7). Das Item 0x43 hat in keinem
Leon-Raum eine Platzierung (0 von 83 Item_aot_set), und sub08 uebergibt trotz Irons' Satz
"Take this card" nichts. Der Auslieferungsstand oeffnet den Funkraum ueber die Blue Keycard.
Fuer den Kartenhinweis ist das ohne Belang — markiert wird der RAUM.

### 2.6 Wo der Zielraum auf der Karte des Ports liegt

Zweite Sonde `re15_port/tests/unit/probe_r30_karte-3010_ziel.c`, gelesen ueber die
oeffentliche Karten-API; Spieler in ROOM1150 an der Stelle, an der sub08 ihn abstellt, kein
Raum sonst besucht. `build/r30_karte-3010/probe_ziel.txt`:

```
[Z] Spieler ROOM1150 idx 0: Blatt 4 Rechteck 2 zid 21 etage=0   Schirm (120,80) 32x40  uv (96,48)
[Z] Ziel    ROOM10F0 idx 0: Blatt 3 Rechteck 9 zid 15 etage=0   Schirm (156,76) 48x40  uv (208,80)  Teilbereiche 0
[Z] Ziel/E  ROOM10F1 idx 0: Blatt 3 Rechteck 9 zid 15 etage=0   (dieselbe Zeile)
[Z] Gast    ROOM10F0 idx 0: Blatt 2 Rechteck 8 zid 15 etage=1   Schirm (164,77) 24x24  uv (128,40)
[S] Blatt 4 Rechteck 2 (Irons' Buero): Zustand CURRENT
[S] Blatt 3 Rechteck 9 (Funkraum)    : Zustand UNVISITED
[S] Besitz Blatt 3: 0   Besitz Blatt 4: 0   Besitz-Bits 0x00000000
[S] Blatt 3 bekannt (blaetterbar): 0   Blatt 4 bekannt: 1
[S] vom normalen Zeichner gezeichnete Rechtecke auf Blatt 3: 0 von 10
```

Quellzeilen: `engine/src/re15_map_zones.h:142-143` (Hauptzeile), `:342-343` (Gastzeile auf
1F), `:667-670` (Etagen-Tabelle), Rechteck `s_map_rectfix` `{3, 208, 80, 156, 76, 48, 40}`;
im Generator `tools/gen_map_zones.py:1250` `(0x10F0, 0): (3, 9)  # Funkraum`.

Folge fuer den Plan: **der normale Zeichner wuerde den Zielraum gar nicht zeichnen**
(unbesucht + Blatt nicht im Besitz -> `continue`, `re15_inv_screen.c:2317-2318`), und das
Blatt 3 ist nicht einmal erblaetterbar. Der Hinweis muss beides umgehen — genau wie RE2, das
den Zielraum ohne Besuch und ohne Karte zeichnet (3.5 c).

### 2.7 Die Ton-Bank laeuft durch den echten VAB-Pfad des Ports

`re15_port/tools/re2_hint_cut.py` (neu, Muster `re2_elevator_cut.py`) schneidet den Satz;
in dieser Phase nach `build/r30_karte-3010/HINTSE.VBS` (8280 B, md5
c6e962522b696f8caabf0068abef7024) + `re2_hint_bank.inc`. Der Schnitt prueft sich selbst
gegen alle zitierten Bytes und gegen die drei anderen Hinweis-Raeume
(`re2_hint_cut.txt`). Dritte Sonde `probe_r30_karte-3010_ton.c` fuehrt die Datei durch
`re15_vab_parse` / `re15_edt_decode` / `re15_edt_resolve_layers_ex` /
`re15_vab_note2pitch2` / `re15_vag_adpcm_decode` — dieselben Funktionen, die
`se_play_layers` (audio_pc.c:685) ruft. `build/r30_karte-3010/probe_ton.txt`:

```
[T2] re15_vab_parse rc=0  programme=2 vag=1 tones_loaded=1  Welle 1: off=0 size=4480
[T3] re15_edt_decode(0x2B) rc=0: Bytes 00 01 23 00 -> prog=1 tone=2 prio=3 voice=-16 extra=0 empty=0
[T4]   Lage 0: vag=0 tone-index=18  vol=80 pan=64 center=85 shift=0 min=61 max=61 adsr=0x80FF/0x1FC0  pitch=0x0400 = 11025 Hz
[T4]   Welle: 4480 B -> 7812 Samples, Spitze 28672; Dauer bei diesem Pitch 0.709 s
[T5] unbelegter Satz 0x10: rc=0 empty=1
```

Die Welle liegt in KEINER RE1.5-Datei (`r30_karte3010_welle_suche.py`: 3193 Dateien,
300 522 652 Byte, 0 Treffer) — sie muss importiert werden.

### 2.8 Der Takt des Port-Menues

| | RE2 | Port |
|---|---|---|
| Statusschirm-Durchgang | 1 je VBlank: `[0x800DFC1A] = 0` @0x80068A1C | 1 je Host-Bild: `re15_menu_fsm_tick` aus `re15_game_step` (game_step_common.c:1187) |
| Rate | 59,8 Hz | `target_fps = 30` (platform/pc/main.c:3140), Logzeile "[fps] target=30 FPS, frame_budget=33ms" in jedem Lauf. GEMESSEN an der echten exe: zwei Laeufe mit 30,38 s und 60,29 s Wanduhr enden bei Bild F120 und F1020 (`takt_30s.debug.log`, `takt_60s.debug.log`) = 900 Bilder in 29,91 s = **30,1 Hz** (Aufloesung der Logzeilen 30 Bilder, also 29,1 bis 31,1 Hz) |

RE1.5 stellt seinen Statusschirm ebenfalls auf Teiler 0 (`DAT_800b5456 = 0` @0x800460DC,
im Port vermerkt in `menu_common.c` phase0_init Punkt 3: "the port has no divider"); der Port
faehrt das Menue also heute schon mit dem halben Takt des Originals. Fuer den Hinweis heisst
das (`r30_karte3010_blink_port.py`, `port_blink_takt.txt`):

| Bauart | Ton-Abstand | Phasen |
|---|---|---|
| RE2 selbst | 78 Ticks = 1,304 s | 39 / 39 Ticks = 0,652 s |
| Port, 1 Zaehlschritt je Menue-Tick | 78 Ticks = **2,600 s** | 1,300 s |
| Port, 2 Zaehlschritte je Menue-Tick | 39 Ticks = **1,300 s** | 20 / 19 Ticks = 0,667 / 0,633 s |

---

## 4. URSACHE

Es gibt kein Symptom im Sinne eines Fehlers — der Port tut, was das RE1.5-Skript sagt, und
das Skript enthaelt keinen Hinweis (2.1, 2.4). Es fehlen vier Dinge, die RE1.5 alle nicht hat:

1. **der Ausloeser**: RE1.5s SCD kennt keinen Opcode 0x84 (Tabelle endet bei 0x5E);
2. **der Statusschirm-Modus**: `menu_common.c` kennt als Fremd-Einstieg nur die Item-Box
   (`re15_menu_request_box`, Unterzustand 4), keinen Einstieg direkt in die Karte mit
   vorgegebenem Blatt;
3. **die Zeichenregel**: der Kartenzeichner kennt keinen Zustand "Ziel" und wuerde den
   unbesuchten Funkraum auf dem nicht besessenen Blatt 3 ueberspringen (2.6);
4. **der Ton**: Satz 0x2B der RE2-Raumbank liegt in keiner RE1.5-Datei (2.7).

Die Toene Se(4,9) und Se(4,5) sind dagegen schon da (CORE00 byte-gleich, 3.8), ebenso die
beiden CLUT-Zeilen 502/498 (`RE15_INV_CLUT_MAP_AKTUELL` / `_UNBESUCHT`).

---

## 5. UMSETZUNGSPLAN fuer den Bau-Agenten

Grundsatz: RE2s Modus 4 auf der vorhandenen RE1.5-Menue-Infrastruktur, nach dem Muster der
zwei bestehenden RE2-Ergaenzungen — Anker im Skript wie `scd_elev_se.c`, Fremd-Einstieg in
den Statusschirm wie `re15_menu_request_box`. Kein Asset-Patch an `ROOM1150.RDT`.

### 5.1 Konstanten (jede mit Beleg)

| Name (Vorschlag) | Wert | Beleg |
|---|---|---|
| `RE15_HINT_ZAEHLER_START` | 10 | RE2 `addiu v0,zero,10` @0x8006F6DC, `sb` @0x8006F6F4 |
| `RE15_HINT_RICHTUNG_START` | 1 | RE2 `addiu v1,zero,1` @0x8006F6B4, `sb` @0x8006F6C4 |
| `RE15_HINT_ZAEHLER_UNTEN` | 10 (`< 10` -> Ton + Umkehr) | RE2 `sltiu v0,v0,0xa` @0x8006F22C |
| `RE15_HINT_ZAEHLER_OBEN` | 0x51 (`>= 0x51` -> Umkehr) | RE2 `sltiu v0,v1,0x51` @0x8006F264 |
| `RE15_HINT_SCHRITT` | 2 | RE2 `addiu v0,v0,-2` @0x8006F254, `addiu v0,v1,2` @0x8006F26C |
| `RE15_HINT_VBLANKS_JE_TICK` | 2 | RE2 Teiler 0 `sb zero,[0x800DFC1A]` @0x80068A1C gegen Port `target_fps = 30` (main.c:3140); siehe 2.8 und Risiko R1 |
| `RE2_HINT_SE` | 0x2B, Bank 2 | RE2 `lui a0,0x22b` @0x8006F234 / `jal 0x8005ba28` @0x8006F238 |
| Ton "Karte da" | CORE-Satz 9 | RE2 `lui a0,0x409` @0x8006F85C / `jal` @0x8006F860 |
| Ton "Schliessen" | CORE-Satz 5 | RE2 `lui a0,0x405` @0x8006F890 / `jal` @0x8006F894 |
| KEIN Oeffnen-Ton (CORE-Satz 6) | — | RE2 `bne v0,zero,0x80026540` @0x80026404 ueberspringt @0x8002652C-30 |
| Schliess-Tasten | Statustaste ODER Abbruch, NICHT Bestaetigen | RE2 `andi v0,v0,0x6000` @0x8006F884; 0x4000 = Status (@0x800264A0), 0x2000 = Abbruch (@0x8006A928, @0x8006B59C), 0x1000 = Bestaetigen (@0x8006B594) |
| CLUT rote Phase | `RE15_INV_CLUT_MAP_AKTUELL` (= RE2 Zeile 502) | RE2 `addiu s2,zero,501` @0x8006F4D0 + `addiu s2,s2,1` @0x8006F514 |
| CLUT Umriss-Phase | `RE15_INV_CLUT_MAP_UNBESUCHT` (= RE2 Zeile 498) | RE2 `addiu s2,zero,498` @0x8006F5DC |
| Anker-Signatur | `42 00 3c 01 22 02 07 00 22 01 1b 00 01 00` | ROOM1150.RDT @Datei 0x012E0 (14 B, 1 Treffer in der Datei, Sonde [M0]) |
| Anker-PC | Signatur + 12 = @Datei 0x012EC (`01 00` Evt_end von sub08) | ebd.; RE2-Vorbild ROOM30B0.RDT @0x01A86 `84 01` direkt vor `01 00` |
| Quellraum | 0x1150 (NICHT 0x1151: dort gibt es sub08 nicht) | 2.4 |
| Zielraum / Zone | 0x10F0, idx 0; Blatt und Rechteck = die HAUPTZEILE dieses Ortes (`etage == 0`), gesucht ueber `re15_map_zone_count` / `re15_map_zone_by_index`; heute Blatt 3, Rechteck 9 | DEBUG.BIN @0x027C0; `re15_map_zones.h:142` (Hauptzeile) gegen `:342` (Gastzeile Blatt 2, `etage == 1`); Sonde 2. Blatt und Rechteck NICHT als Zahl in den Code — der Riegel haelt 3/9 fest und meldet, wenn die Zonen-Tabelle sich aendert. RE2 fuehrt das Blatt je Hinweis fest (@0x80011D20); der Port kann es ableiten, weil seine Zonen-Tabelle das Blatt schon traegt |
| Panel-Endlage der Karte | 0x19 Schritte der vorhandenen Gleit-Schritte | RE1.5 `sltiu 0x19` @0x8004C0BC und die sieben Schrittweiten @0x8004C0E0-0x8004C154 (stehen schon in `map_mode` case 0) |
| Blenden | unveraendert die der Menue-FSM | RE1.5 @0x8001CA64-88 / @0x800496C4-704 / @0x80046544-7C; RE2 benutzt dieselben Werte `FUN_8002C1A0(0x200, +-0x1800, 7, 1)` @0x80025A80, @0x8006F824, @0x80068F0C |

### 5.2 Dateien und Schritte

**S1 — Ton-Bank ausliefern.**
`python re15_port/tools/re2_hint_cut.py --install` -> `re15_port/shared_assets/RE2/HINTSE.VBS`
(8280 B, md5 c6e962522b696f8caabf0068abef7024) und
`re15_port/engine/src/gen/re2_hint_bank.inc` (`RE2_HINT_EDT_SIZE 3800`, `_VBD_OFF 3800`,
`_VBD_SIZE 4480`, `RE2_HINT_SE 0x2B`).

**S2 — Ton-Funktion.**
- `include/re15_audio.h`: `void re15_audio_re2_hint_se(int se_id);` mit Belegblock (3.8).
- `platform/pc/src/audio_pc.c`: `load_re2_hint_se_pc()` + `re15_audio_re2_hint_se()` als Kopie
  des Fahrstuhl-Slots (Z. 1129-1197), Datei "HINTSE.VBS", Groessen aus `re2_hint_bank.inc`.
- `platform/psx/src/audio_psx.c`: Folge-Stub neben Z. 858/865.
- `tests/test_support.c`: Spion `g_test_hint_se_last` / `g_test_hint_se_count` neben Z. 69/75.

**S3 — neues Modul `engine/src/map_hint_common.c` + `include/re15_map_hint.h`.**
- Tabelle der Hinweise, EIN Eintrag: `{ quelle 0x1150, signatur[14], pc_versatz 12,
  ziel_raum 0x10F0, ziel_idx 0 }` (Port-Gegenstueck zu RE2s Hinweis-Tabellen
  @0x80011D20 / @0x800A9BA0).
- `re15_map_hint_room_scan(raw, size, room_id)`: sucht die Signatur im geladenen RDT-Puffer,
  merkt den Anker-Zeiger; setzt `g_re15_map_hint_anchor_n` (0 in allen anderen Raeumen).
- `re15_map_hint_pc(pc)`: `pc == Anker` -> Anforderung merken (Hinweis-Nummer).
- `re15_map_hint_pending()` / `re15_map_hint_take()`.
- Blinker: `re15_map_hint_begin()` (Zaehler 10, Richtung 1), `re15_map_hint_tick()`
  (`RE15_HINT_VBLANKS_JE_TICK` Zaehlschritte nach 3.5 a; bei "Zaehler < 10 und Richtung != 0"
  `re15_audio_re2_hint_se(RE2_HINT_SE)`), `re15_map_hint_rot()` (= Richtung == 0).
- Ziel-Aufloesung: `re15_map_hint_ziel(&page, &rect)` = Hauptzeile (`etage == 0`) des
  Zielraums aus der Zonen-Tabelle (s. 5.1).

**S4 — `engine/src/scd_vm.c`, zwei Zeilen, beide neben den Fahrstuhl-Haken.**
- Z. 157 (in `scd_register_current_rdt`): `re15_map_hint_room_scan(...)`.
- Z. 693 (Verteiler, hinter `uint8_t op = *t->pc;`):
  `if (g_re15_map_hint_anchor_n) re15_map_hint_pc(t->pc);`
  `op_evt_end` selbst bleibt unberuehrt.

**S5 — `engine/src/game_step_common.c`, vor dem START-Poll (Z. 1177).**
`if (re15_map_hint_pending()) { re15_menu_request_map_hint(); }` — NICHT an
`s_inv_open_allowed` haengen: RE2s Handler setzt die Phase selbst und umgeht die Tastenabfrage
(3.2/3.3). Verbraucht wird die Anforderung erst, wenn `re15_menu_request_map_hint` sie
angenommen hat (s_stage == 0 && !s_alive), sonst bleibt sie stehen.

**S6 — `engine/src/menu_common.c` + `include/re15_menu.h`.**
- `static uint8_t s_hint_target;` und `void re15_menu_request_map_hint(void)` nach dem Muster
  von `re15_menu_request_box` (Z. 1835): `s_hint_target = 1; s_latch = 1; s_stage = 1;`.
- Phase 0 nach `phase0_init()` (neben `if (s_box_target)`, Z. 1699): `s_substate = 1;
  g_inv_screen.tab = 1;` dann `g_inv_screen.map_page = Zielblatt`. ⛔ NICHT `map_entry()`
  rufen: das schreibt ueber `re15_inv_map_stage_init` die PERSISTENTEN Register
  DAT_800b260d/260e (`s_map_room`/`s_map_page`, re15_inv_screen.c:111-116) und hinterliesse
  damit eine Spur. Die Blattgrafik folgt `g_inv_screen.map_page` von selbst
  (`map_page_check`, inv_render_pc.c:412; die Messschiene main.c:4442 setzt das Feld genauso).
  Dann die sieben Panel-Register 0x19-mal um ihre Schrittweite versetzen (vorhandener Code
  aus `map_mode` case 0 als Hilfsfunktion herausziehen, keine neue Zahl), `s_c3 = 0x19`,
  `g_inv_screen.item_state = 1`, `g_inv_screen.hint_aktiv = 1`, `hint_page`/`hint_rect`
  aus `re15_map_hint_ziel`, `re15_map_hint_begin()`. Kein Oeffnen-Ton.
  (`re15_inv_screen_open` loescht die ganze Struktur per `memset` — die Hinweis-Felder
  muessen also NACH `phase0_init()` gesetzt werden und sind beim naechsten Oeffnen von
  selbst wieder 0.)
- Uebergang Phase 0 -> 1 (Einblendung fertig, Z. 1704): im Hinweis `se4(9)`.
- `menu_task_step`: im Hinweis JEDEN Tick `re15_map_hint_tick()` — auch waehrend der
  Einblendung, denn RE2s Init-Schleife ruft den Zeichner samt Zaehler schon dort
  (@0x8006F838-58). Danach `g_inv_screen.hint_rot = re15_map_hint_rot()`.
- `map_mode` case 1 im Hinweis: KEIN Blaettern (HOCH/RUNTER), KEIN L1; nur
  `(pressed & RE15_PAD_BIT_START) || (re15_pad_virtual_word(pressed) & 0x8000)` ->
  `se4(5)`, `s_phase = 2`. Geschlossen wird der GANZE Schirm ueber `close_phase` (RE2 verlaesst
  den Status-Task, @0x80068F88 -> 0x80068CD4), nicht ueber das Rueck-Gleiten in die Reiter.
- Abbau in `close_phase` und `re15_menu_toggle`: `s_hint_target = 0;
  g_inv_screen.hint_aktiv = 0;`.

**S7 — `include/re15_inv_screen.h` + `engine/src/re15_inv_screen.c`.**
- Neue Felder ANS ENDE der Schirm-Struktur: `hint_aktiv`, `hint_rot`, `hint_page`,
  `hint_rect`.
- Kachel-Schleife (Z. 2294 ff.), nur wenn `hint_aktiv`:
  - Zielkachel (`map_page == hint_page && i == hint_rect`): wird IMMER gezeichnet, vor dem
    Besitz-Gatter Z. 2317 und dem UNMAPPED-Gatter Z. 2341; CLUT = `hint_rot ?
    RE15_INV_CLUT_MAP_AKTUELL : RE15_INV_CLUT_MAP_UNBESUCHT`; sie gehoert in den ERSTEN
    Durchgang (`durchgang_r == 0`), damit kein Nachbar sie ueberdeckt.
  - jede andere Kachel im Zustand CURRENT wird wie VISITED gezeichnet (RE2s Hinweis-Zeichner
    hebt den eigenen Raum nicht hervor, 3.5 d). Dasselbe im Schema-Durchgang (Z. 2553 ff.).
- Spielermarker (Z. 1917): im Hinweis nicht zeichnen (3.5 d).
- Tuer-/Treppenmarken bleiben an ihrem Besucht-Gatter — der Hinweis deckt nichts auf.

**S8 — Speichern: NICHTS.** Kein Feld in `re15_savedata.c`, kein Versionssprung. RE2 haelt
nach dem Schliessen nichts (3.7). Dass der Hinweis nur einmal kommt, leistet schon das
vorhandene Spiel-Flag (3,94): es wird am ANFANG von sub08 gesetzt (@0x01110), liegt in
`g_game.flags` und damit im Spielstand (`re15_savedata.c:166/212`), und main00 legt die
AUTO-Zone nur bei (3,94) == 0 an (@0x00DE6).

### 5.3 Riegel

| Riegel | prueft |
|---|---|
| `unit_r30_hinweis_anker` | ROOM1150.RDT: Signatur 1 Treffer @0x012E0; sub08 bis zum Ende ticken (Muster Sonde 1): Anforderung steht GENAU im Tick des Thread-Endes, vorher nie. ROOM1151.RDT und zwei beliebige andere Raeume: 0 Anker |
| `unit_r30_hinweis_fsm` | Anforderung -> Stufen 1/2 -> Schirm lebt mit `substate 1`, `item_state 1`, `map_page 3`; CORE-Spion: KEIN Satz 6, Satz 9 genau einmal (nach der Einblendung), Satz 5 beim Schliessen; Hinweis-Spion: Satz 0x2B im 1. Tick und dann alle 39 Ticks; `hint_rot` 20 Ticks an / 19 aus; Bestaetigen-Taste, HOCH/RUNTER und L1 aendern nichts; START und Abbruch schliessen |
| `unit_r30_hinweis_spurlos` | vor und nach dem ganzen Hinweis bitgleich: `re15_map_visited_export` (32 B), `g_game.flags`, `re15_map_owned_bits()`, `re15_inv_map_page()`; danach zeigt der normale Kartenaufruf in ROOM1150 Blatt 4, und Blatt 3 Rechteck 9 ist weiter UNVISITED |
| `unit_r30_hinweis_zeichner` | Op-Liste im Hinweis: Zielkachel (156,76) 48x40 uv (208,80) vorhanden, obwohl unbesucht und Blatt nicht im Besitz; CLUT wechselt mit `hint_rot`; kein Marker-Sprite (uv 224,128); keine Kachel mit `RE15_INV_CLUT_MAP_AKTUELL` ausser dem Ziel in der roten Phase |
| `unit_r30_hinweis_bank` | `HINTSE.VBS` 8280 B; Welle sha1 eb386970...; Durchlauf wie Sonde 3: prog 1, tone 2, Pitch 0x0400 |

### 5.4 Abnahme-Messung (echte exe, echter Ablauf)

1. Spielstand vor der Szene (flag (3,94) == 0), Raum 1150 betreten, Szene laufen lassen;
   KEIN `RE15_AUTOSHOT`, KEIN `RE15_SOFTWARE_RENDER`; Bilder ueber `RE15_FRAMEDUMP`.
2. Erwartet: im Bild nach dem Szenenende blendet das Spielbild aus, die Karte erscheint mit
   dem Titel "POLICE STATION 2F"; die Kachel (156,76) 48x40 wechselt alle 0,65 s zwischen
   dunkelrot gefuelltem und leerem Umriss; kein Spielermarker.
3. `debug.log` mit `RE15_SE_DEBUG=1`: eine Zeile `se=43` (0x2B) mit `pitch=0x400 (11025 Hz)`
   je 39 Menue-Ticks; Abstand der Zeilen in Wanduhr-Zeit 1,30 s +- ein Bild.
4. START druecken: Abbruch-Ton, Ausblenden, Spiel laeuft weiter. START erneut: normaler
   Statusschirm; L1 -> Karte zeigt Blatt 4 mit Irons' Buero rot; Blatt 3 ist nur
   erblaetterbar, wenn es das vorher schon war.
5. Speichern, laden, Raum 1150 erneut betreten: keine Szene, kein Hinweis.

---

## 6. Risiken und offene Fragen

**R1 — Takt (Entscheidung des Bau-Agenten, mit Messung).** Der Plan nimmt 2 Zaehlschritte je
Menue-Tick, damit Ton-Abstand und Blinkdauer in ZEIT stimmen (1,30 s statt 2,60 s). Das
setzt voraus, dass der Menue-Tick 30 Hz laeuft. Mit `RE15_FPS=60` waere der Faktor 1. Sauber
ist ein Faktor aus der Bildrate (`60 / target_fps`), nicht die feste 2 — dafuer muss die
Engine die Bildrate kennen; heute kennt sie nur `platform/pc/main.c`. Die uebrigen
Menue-Animationen des Ports laufen unveraendert mit halbem Originaltakt; das ist ein
eigener, aelterer Befund und nicht Teil dieses Auftrags.

**R2 — leeres Blatt.** Wer ROOM1150 vom Dach her erreicht, hat 2F noch nie betreten: auf
Blatt 3 stuende dann NUR die blinkende Zielkachel (Sonde 2: 0 von 10 Rechtecken sichtbar).
Das ist RE2s Regel (ohne Karte nur Besuchtes + Ziel) und so geplant. Eine Orientierung an
den Nachbarraeumen gibt es dann nicht.

**R3 — der Raum ist vorerst verschlossen.** Der Hinweis zeigt auf einen Raum, dessen Tuer die
Blue Keycard aus dem Evidence Room braucht (2.5). Das ist der Auslieferungsstand und aendert
am Hinweis nichts; es kann aber so wirken, als fuehre der Hinweis in eine Sackgasse.

**R4 — Letterbox-Rest.** sub08 baut die Szenen-Flags im selben Tick ab, in dem der Hinweis
angefordert wird. Das Menue friert das Spiel sofort ein; ein noch laufender
Letterbox-Abbau wuerde erst nach dem Schliessen zu Ende laufen. NICHT GEMESSEN — in der
Abnahme (5.4 Schritt 4) auf die ersten Bilder nach dem Schliessen achten.

**R5 — dieselben Dateien wie Thema F.** Thema F (Karten-Marken, Verlust beim Laden) fasst
ebenfalls `re15_inv_screen.c` und die Kartenzonen an. Reihenfolge absprechen; der Hinweis
aendert in der Kachel-Schleife nur einen `if (hint_aktiv)`-Zweig.

**R6 — Pakete.** `HINTSE.VBS` muss in alle drei Pakete; neue `engine/src/*.c` verlangen beim
Android-Bau ein frisches `app/.cxx` (Memory `reai-v2-android-glob-cache`).

**Offene Frage O1 — Elza.** ROOM1151 hat die Szene sub08 nicht; dort gibt es nur das kurze
Gespraech sub03 (Flag (3,157)). Der Auftrag nennt ROOM 1150; der Plan haengt den Hinweis
deshalb NUR an sub08 von ROOM1150. Soll Elza denselben Hinweis nach sub03 bekommen, ist das
ein zweiter Tabelleneintrag (Anker = Evt_end von sub03, ROOM1151.RDT @Datei 0x00F72) — das
ist eine Entscheidung des Nutzers, kein Befund.

**Offene Frage O2 — zweites Gespraech.** Auch ROOM1150 hat sub03 (zweites Gespraech mit
Irons, Flag (3,157)). RE2 zeigt seine Hinweise je Skriptstelle genau einmal; der Plan
wiederholt den Hinweis dort nicht.

---

## 7. Was NICHT belegt ist

| Aussage | Stand |
|---|---|
| Bedeutung der Bank 0x800D4920 (Satzbyte +13, CLUT-Varianten 503/506/507) | NICHT BELEGT; der Port fuehrt sie nicht, der Plan braucht sie nicht |
| `[0x800D69F2]` liegt ausserhalb des RE2-Speicherblocks | NICHT BELEGT (nicht gesucht); folgenlos, weil nach dem Schliessen niemand das Byte liest |
| ROOMB0B0 `84 03` @0x03324 ist ausgefuehrter Code | NICHT BELEGT — der Block desynchronisiert im Walker |
| wer Statusschirm-Modus 3 anfordert | NICHT BELEGT — kein Schreiber mit Wert 3 gefunden (Xref + memscan); fuer den Hinweis ohne Belang |
| welches Skript Hinweis 0 (Blatt 5, Raum 0x16) setzt | NICHT BELEGT — im Leon-Zensus kein Record `84 00`; Claire-RDTs (PL1) nicht durchsucht |
| welche RE2-Raeume die Zielraum-Bytes 0x0A/0x1A/0x07/0x08 sind | NICHT BELEGT (Raumnamen nicht aufgeloest); fuer den Port ohne Belang |
| Tick-Rate des Port-Menues bei OFFENEM Schirm, in Wanduhr-Zeit | nur mittelbar: gemessen ist die Bildrate im SPIEL (30,1 Hz, 2.8), belegt ist im Code ein Menue-Tick je `re15_game_step`; eine Zeitmessung bei offenem Menue steht aus (Abnahme 5.4 Schritt 3) |
| Verhalten des Letterbox-Abbaus unter dem Hinweis | NICHT GEMESSEN (R4) |
| dass RE2 den Ton auf echter Hardware mit 11025 Hz spielt | berechnet aus Tone-Bytes und `note2pitch2`, nicht an einer Aufnahme gemessen |
| Tastenbelegung 0x4000/0x2000/0x1000 in RE2 | aus der Verwendung geschlossen (Oeffnen-Zweig, Abbruch-Ton, Bestaetigen-Ton), nicht aus der Tastentabelle gelesen |

---

## 8. Stichprobe der uebernommenen Vorgaenger-Ergebnisse

Selbst nachgeschlagen, alle bestanden:

| Aussage des Vorgaengers | meine Pruefung |
|---|---|
| Opcode-Tabelle 0x800A74C8, Eintrag 0x84 | `[0x800A76D8]` = 0x800591C4 gelesen, Handler neu disassembliert |
| Modus-Tabelle / Hinweis-Init 0x8006F6A8 | `[0x800A936C + 4*12]` = {0x8006F6A8, 0x8006F8B4, 0x80068CD4} gelesen |
| Sprungtabelle @0x80011D20 und Bytes @0x800A9BA0 | beide aus der EXE gelesen: `16 0a 1a 07 08` |
| ROOM3010.RDT @0x26EE `84 02` | `xxd`: `84 02 01 00` |
| EDT[0x2B] @0x1F824, Tone @0x20298, VAG 18 sha1 | `xxd` + eigener sha1: bitgleich |
| VSync-Teiler 0 @0x80068A1C | Leser @0x8002B994 -> `jal 0x80085EA0`; dort GPU_REG1/TMR_HRETRACE = VSync |
| DEBUG.BIN-Raumnamen, Satzformel | `xxd -s 0x27C0`; Formel @0x8001D39C-D0 disassembliert |
| Sonde 1 | neu gefahren, `[M*]`-Zeilen bitgleich |
| Blink-Simulation | gegen das Disassemblat Zeile fuer Zeile gelesen; eigene Zweit-Simulation im Port-Takt |

Berichtigt gegenueber Vorarbeiten:
- `map-re2-system.md` Z. 33 nennt FUN_8006F1C4 die "CHECK/Karten-Item-Variante" mit
  "Raum-Cursor". Sie ist der Zeichner des SCD-Hinweises (Modus 4), ihr "Cursor" ist der
  Zielraum aus der Hinweis-Tabelle.
- Auftragsnotiz "Item 0x42 Communications Card": es ist 0x43 (2.5).
- Auftragsnotiz "ROOM10D0 Funkraum-Tuer": ROOM10D0 ist der Flur MIT der Tuer; der Funkraum
  ist ROOM10F0.

---

## 9. Artefakte

Werkzeuge (`analysis/befunde_runde30/tools/`): `r30_karte3010_hint_zensus.py`,
`_hint_se.py`, `_blink_sim.py`, `_blink_port.py`, `_xref.py`, `_memscan.py`, `_jals.py`,
`_se_sites.py`, `_core_satz.py`, `_welle_suche.py`, `_msg.py`, `_raumnamen.py`, `_lauf.sh`,
`_szene.sh`; dazu `re15_port/tools/re2_hint_cut.py`.
Sonden (`re15_port/tests/unit/`): `probe_r30_karte-3010.c`, `probe_r30_karte-3010_ziel.c`,
`probe_r30_karte-3010_ton.c`, `probes/r30_karte-3010.cmake`.
Ausgaben: `build/r30_karte-3010/` (Disassemblate `re2_*.dis.txt`, Zensus, Sonden-Ausgaben,
Abzuege, `HINTSE.VBS`, `hint_se_ROOM3010.wav`).
