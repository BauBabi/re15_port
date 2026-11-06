# Spur S (spinnengore), Abnahme 3 (Runde 41, 2026-11-01)

Ich habe als unabhängiger Abnehmer geprüft und dem Bau nichts geglaubt. Geprüft wurde der Zweig `r41/spinnengore` mit Kopf
`491e2520d` (Nachbesserung 2 auf `7982d54bd`).

- Bau selbst über `local_build.sh configure` und `local_build.sh build` (LOCAL-BUILD-OK, `ninja: no work to do`, die exe
  von 21:49 entspricht also dem Kopf).
- Nach den Gegenproben (Abschnitt 3) habe ich den Stand mit `git checkout HEAD --` wiederhergestellt und neu gebaut
  (LOCAL-BUILD-OK). `git status` zeigt danach nur meine Belege. Die md5 der exe weicht ab, weil `main.c:6558`
  `__DATE__`/`__TIME__` einbaut. Die Riegel sind danach wieder grün (`unit_r41_spinnengore`, `unit_re2_spider_ai`,
  `unit_re2_baby_spider_dmg`).
- Alle Fensterläufe liefen über die eigene Messkopie `re15_pc_S41abn3.exe` mit dem beschleunigten Renderer. Ich habe weder
  `AUTOSHOT` noch `SOFTWARE_RENDER` noch `TITLE_SHOT` gesetzt. Titel und Spielerwahl liefen über die echten Menüs
  (`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`, für Elza zusätzlich `_SWITCH`), danach kam `RE15_DEBUG_JUMP`.
  Aufnahme per gdigrab am Fenster-Handle der eigenen PID, 10 Bilder/s. Beendet habe ich nur die eigene PID.
- Eigenes Feuerskript mit anderem Takt als Bauer, Riegel und Abnahme 2: `W0.3,M0.5,(MA0.1,M0.9)×9,MA0.1,M1.0,W12`,
  Skriptstart Bild 8.
- Eigene Werkzeuge (capstone, unabhängig vom Skill-Disassembler und vom Prüfer des Bauers): `scratchpad/S41abn3/`
  mit `cap.ps1`, `ana.py`, `gruen.py`, `sheet.py`, `vbytes.py` mit `stellen_nb2.txt`, `dump.py`, `gegen.py`.
  Rohbilder und Logs der Läufe `a`–`h` liegen dort ebenfalls.
- Belege: `S_belege/abn3/c01…c09`.

## Urteil: NICHT bestanden

Der Kern hält:

- Unter der Standard-KI platzt die Spinne bei einem Magnum-Treffer sichtbar, und zwar für Leon und Elza.
- Unter der Leiche wächst jetzt eine grüne Lache.
- Unter der RE1.5-KI platzt die Spinne ebenfalls, und der Boden wird dunkel und grünstichig.
- Der Schalter wirkt.
- Die Riegel des Spinnen-Gehirns und der Lache sind ohne den Bau rot. Das gilt auch für meine engeren Gegenproben und einen
  Takt-Fehler um eins.
- 300 Unit-Tests und 5 Integrationstests sind grün.
- Gegen die Bytes stimmen 117 von 118 zitierten Stellen aus NB2.3 und dem Kopf von `re15_spinnenlache_r41.h`. Bei RE1.5
  gibt es 0 Abweichungen zwischen Mod und Disc.

Es bleiben drei Mängel:

1. **Krähe 0x21: Die Federn beim Tod sind in RE2 belegt. Das Dossier behauptet dreimal, der Mechanismus fehle in beiden
   Quellen** (Abschnitt 5.1). Das ist dieselbe Fehlerklasse wie Mangel 1 der Abnahme 2.
2. **Zwei Teile des Baus haben keinen Riegel**: der Zeichner der Babyflecken in `main.c` und das Leeren der Flecken beim
   Raumwechsel. Ich habe beide ausgebaut, und alle Riegel bleiben grün (Abschnitt 3, Gegenprobe C).
3. **Eine zitierte Adresse ist falsch**: `RE15_SPINNENLACHE_FLECKEN_MAX 50 @0x800164a4 (addiu v1,zero,50)`. Bei
   0x800164a4 steht `beqz v1`. Das `addiu v1,zero,0x32` steht @0x8001649c (Abschnitt 2).

## 1. Eigene Läufe im echten Fenster

Ausgewertet habe ich `RE15_SPINNEN_LOG` (Zustand, `{G..}`-Klammer), `RE15_FX_CLUT_LOG` (je gezeichnetem Partikel Id,
Sub, Skala und CLUT), `RE15_LACHE_LOG` (Maß, Tinte, sichtbar) und `RE15_STATE_LOG`. Die Auswerter sind meine eigenen.

| Lauf | Einstellung | Log | Bild |
|---|---|---|---|
| a Leon, ROOM2070, Redhawk w7 | Standard | Slot 2 F92: st 1→3, s=5.1.0, HP 110→-20, +0x93 = 0xc1, **ta=1 sh=80001 fl=7fff8**. Slot 1 F152 ebenso (+0x93 = 0x41). Je Tod Sub 8 / 0x7991, zusammen 110 Zeichnungen. **Lache Slot 2: F104 1308 x 1408 `bf10bf` → F193 2020 x 2120** (90 Schritte), sichtbar 511/521. Slot 1 bis 2020 x 2120, liegt aber außerhalb des Bildes (sichtbar 0). Rotes `0x1500` nur ab F105, das sind Leons Babybisse (HP -1) | Bild 278–280: Der Rumpf zerfällt in gelbgrüne Scherben, die Beine fliegen ab. Bild 300/350: **grüne Lache** unter den Resten (`c01`) |
| b Elza, ROOM2071, w7 | Standard | Slot 2 F32 und Slot 1 F62: ta=1, sh=80001. Lache Slot 2: F44 → F133 2020 x 2120 `bf10bf`, sichtbar 511/521 | Bild 257–259: Platzen mit grüner Wolke und großen Scherben. Ab Bild 262 eine **hellgrüne Lache** unter den Beinresten, bis Bild 400 unverändert (`c02`) |
| c Elza, ROOM2071, w7, `RE15_KEIN_80PROZENT_GORE=1` | Schalter aus | F32/F62 **ta=0 sh=00000**. Sub 8 nur 44 Zeichnungen (`re2s_blut`, hängt nicht am Schalter). Lache wächst wie in b | Bild 268–285: Die Spinne bäumt sich auf, dreht sich auf den Rücken und bleibt ganz, keine Scherben. Die grüne Lache wächst trotzdem, wie im Dossier unter NB2.4 als „keine PORT-WAHL“ festgelegt (`c03`) |
| d Leon, ROOM2070, w7, `RE15_AI_FLAVOR=re15` | RE1.5-KI | Kein Treffer: Die RE1.5-Spinnen bleiben bei x = -1100/-12000 stehen, Leon bei 4851, und keine Spinne verliert HP | kein Gore-Bild. Das ist kein Mangel des Baus. Die RE1.5-Spinne kommt auf diesem Weg nicht in Reichweite |
| e Elza, ROOM2071, w7, RE1.5-KI | RE1.5-KI | Slot 2 F32 s=7.1.1, +0x93 = 0x43, ta=1. Slot 1 F62 +0x93 = 0xc3. **Lache `9f80df` F41 → F131 2028 x 2128** (91 Schritte), Endzustand st=7/7/1/2, also Phase 2. **sichtbar 0/502** | Bild 346–349: Die Spinne direkt vor der Kamera platzt in große gelbgrüne Scherben (`c04`). Die Lache ist **nicht zu sehen**, weil das Viereck an der Nahebene wegfällt (Offen 24 des Bauers, bestätigt) |
| f Leon, ROOM2070, **Remington w8**, RE1.5-KI | RE1.5-KI | F92 Slot 2: Krit-Treffer, sh=06000 (Beinpaar). F152 Slot 2 und F212 Slot 1 Tod über den Platzweg (+0x5 = 8). Lache `9f80df` 1308 → 2028 x 2128, 91 Schritte, sichtbar 534/581 | Bild 437 und 464: Platzen. Ab Bild 475 wird der Boden unter den Resten dunkel und grünstichig: Mittel (29.6, 30.0, 22.8) in Bild 430 gegen (20.5, 20.5, 13.2) in Bild 490 (`c05`) |
| h Leon, ROOM1190, w7, `RE15_FORCE_EVENT=10@400` | Standard (Hund) | 3 Hunde sterben mit dem Redhawk (F639, F765, F909, ss1 = 7) | Bild 475–482 und 517–525: Beim Tod fliegen viele dunkelrote Fetzen weg, dazu rotes Blut. Bild 565–577: rote Fleischbrocken am Kadaver (`c09`). Damit gibt es den **Fensterbeleg zum Hund-Gore, der dem Bauer nicht gelang** (Dossier 5: „Bild aus dem Fenster nicht gelungen“). Ob die Fetzen die 7 Teile der Brücke oder die ESP-Effekte Id 7 (741 Zeichnungen, CLUT 0x7840) sind, lässt sich bei dieser Auflösung nicht trennen |

Babyspinnen sind in keinem meiner 6 Spinnenläufe gestorben (`st=3` bei Typ 0x26: 0). Das bestätigt Offen 18. Einen
Hinweis zur Ursache habe ich am Zertret-Tor gefunden (EMS26, selbst disassembliert). Der Bauer sucht sie am
Spieler-Substate. Das Tor verlangt aber außerdem einen Tastenzustand:

```
801003d4 lw v0,0x1f0(s0) / 801003dc sltiu v0,v0,0x1f4      ; Abstand < 500
801003ec lhu v0,-0x40a(v0) (0x800CFBF6) / 801003f4 andi v0,v0,6 / 801003f8 beqz   ; Pad-Bits 1|2 noetig
80100400 lbu v1,0x106(s0) / 80100408 lbu v0,-0x302 (0x800CFCFE) / 80100410 bne    ; gleiche Ebene
8010041c lhu v1,-0x404 (0x800CFBFC) == 0x101/0x201/0x301/0x401                    ; Spieler-Zustand
80100444 addiu v0,zero,3 / 80100448 sw v0,4(s0)                                    ; Tod
```

Nur Gehen (`pmo=105`) genügt also nicht. Gebraucht wird auch das Tastenbit. Das ist kein Gore und kein Mangel dieser Spur,
aber der nächste Weg für Offen 18.

### Beobachtung (kein Mangel): Die Lache liegt über Leons Schuh

In Lauf a steht Leon am Ende in der Lache (Spinne bei (3977,-5683), Leon bei (3665,-6402)). In den Bildern 404, 405 und
414 ist sein rechter Schuh **grün gefärbt**, ebenso die Beinstümpfe der Spinne über der Lache (`c06`).

Die Ursache ist die OT-Ordnung nach mittlerer Tiefe: Das große Viereck wird nach der Summe seiner vier Ecken-z einsortiert
(`re15_schatten_bucket`) und landet nach Leons Fußdreiecken. Das Original macht es genauso. RE2 FUN_8001699c hängt jeden
Schatten-Record per `AVSZ4` (`4b68002e` @0x80016b6c) und `srl a0,a0,3` (@0x80016b9c) in die OT, selbst gelesen. Der
Effekt wäre dort also auch zu sehen. Er wird aber durch die PORT-WAHL „Basis 1300 x 1400 statt 800 x 800“ größer, denn die
Lache wächst auf 2020 statt 1520 Halbmaß. Das gehört in die Release Notes oder in die Entscheidung über die PORT-WAHL.

## 2. Zitierte Adressen gegen die Bytes

Den Prüfer habe ich selbst mit capstone geschrieben. RE1.5 lese ich in `info/Re1.5` und in `Version_40%`, RE2 in
`info/re2leon`. Geprüft sind alle Stellen aus NB2.3 und aus dem Kopf von `re15_spinnenlache_r41.h`, dazu die
Tabellenworte und Records (`stellen_nb2.txt`, Ausgabe `vbytes_nb2.txt`).

**117 von 118 Stellen stimmen.** Vier weitere Zeilen hat mein Regex als „XX“ markiert. Sie sind aber richtig: capstone
schreibt dort `(s0)` statt `0(s0)` (`sw v0,0(s0)` @0x80104ec4 und @0x80100cf0, `sw zero,0(a0)` @0x8001b258,
`lw v0,0(s0)` @0x800267cc). Bei RE1.5 gibt es 0 Abweichungen zwischen Mod und Disc.

Die Ablaufbehauptungen habe ich an den ganzen Funktionen nachgerechnet (`d_corpse15.txt`, `d_ems25_de8.txt`):

- **RE1.5 CORPSE 0x801153d4**: Phase 0 (0x80115414) fällt ohne Sprung in Phase 1 (0x8011546c). Phase 1 springt
  `j 0x8011559c` zum Zähler. Der Zähler speichert `a0-1` im Delay-Slot von `bnez a0` (@0x801155b4/b8) und prüft dabei den
  alten Wert. Daraus folgen **91** Wachstumsschritte, also 2028. Phase 3 fällt in Phase 4, Phase 4 fällt in den Zähler,
  daraus folgen **121** Schritte, also 2268. Am Ende kommt `ori a0,zero,9` / `jal 0x800453d0` / `sb v0(2),7(v1)`.
  Bestätigt.
- **Eintritt**: `sw v0(7),4(v1)` @0x80114580 schreibt Byte +4 = 7 und +5/+6/+7 = 0. `0x03000007` @0x80114d4c-60
  schreibt +7 = 3. Der Krit-Zweig `andi v0,v1,0x40` @0x801146b0 führt nach der Teile-Schleife (+0x1e0 @0x80114af8-0c) zu
  `sb 7,5(v1)` @0x80114b1c-20. Bestätigt.
  Nebenbei: Der Port setzt beim Übergang in die Leiche nur +7. Das Original nullt dabei auch +5 und +6, der Port lässt
  dort s=7.1 stehen (Lauf e). Das ist ohne Folgen, denn CORPSE liest nur +7 und +0x9E.
- **RE2 FUN_80104DE8**: Zustand [2] fällt ohne Sprung in [3] (@0x80104ec4 → 0x80104ec8). Zustand [3] wächst, zählt
  +0x16B herunter und prüft den neuen Wert (`sll v0,v0,0x18` / `bnez` @0x80104ef0-f4). Daraus folgen **90** Schritte,
  also 2020. Die Wurzel ruft den Zustands-Handler per `jalr` @0x80100138 (Tabelle @0x80106420) und danach
  `jal 0x80104de8` @0x80100180 im selben Takt. Die geänderte Erwartung in `unit_re2_spider_ai` (+0x225 = 3 nach dem ersten
  Leichentakt) ist also richtig.
- **Babyfleck**: FUN_8001699c zeichnet über `s1 = base+6` (@0x800169dc), also `lbu v1,8(s1)` = Record-Byte +0xE,
  Bit 4 ohne Bit 8 (@0x80016a04-18), 50 Records (`addiu s5,zero,0x32` @0x800169a4). FUN_8001633c leert mit `s3 = 0x32`
  (@0x80016350). Bestätigt.

**Die falsche Stelle (Mangel 3):** `include/re15_spinnenlache_r41.h:56`, `RE15_SPINNENLACHE_FLECKEN_MAX 50` mit dem
Zitat „50 Plätze @0x800164a4 (`addiu v1,zero,50`)“. Gelesen:

```
8001649c: 24030032  addiu v1, zero, 0x32     <- hier steht die 50
800164a0: 2463ffff  addiu v1, v1, -1
800164a4: 10600008  beqz v1, 0x800164c8      <- zitierte Adresse
```

Der Wert stimmt, die Adresse nicht. Nach CLAUDE.md muss jede Konstante ihre Adresse richtig tragen.

## 3. Sind die Riegel ohne den Bau rot?

Der Bauer hat ganze Dateien zurückgesetzt (Gegenprobe 1) und die Zeichner-Zeile entfernt (Gegenprobe 2). Ich habe
stattdessen einzelne Stellen ausgebaut, jeweils mit `local_build.sh build`, dann die Riegel, dann `gegen.py restore`.

| Gegenprobe | Ausgebaut | Ergebnis |
|---|---|---|
| A | (a) `re15_spinnenlache_re2_boden(e)` in der RE2-Wurzel; (b) Babytod wieder ohne Record-Tabelle (`re2s_gore`); (c) `e->sub_state_1 = 7` im Krit-Platzer | **rot**: M5 ×3 („+0x225 2, 0 x 0 nach 800 Takten“, Tinte 000000), `unit_re2_spider_ai` („ist 2“), **L8 ×6** („gruen 0 rot 1“ in den Zeilen 0/3/7/8/13/20), **M4** („w20 bits 41 Leichen-Phase 4, Soll 1“). Integration: **L7 `pruef_lache re2` Lache 0** |
| B | `RE15_SPINNENLACHE_RE2_TAKTE` 90 → 91 (Fehler um eins) | **rot**: M5 „2028 x 2128 nach 92 Takten (Soll 2020 x 2120)“, `unit_re2_spider_ai` („+0x16B 90“) |
| C | (e) der Aufruf `re15_render_shadow_quad_c` in der Flecken-Schleife von `main.c`; (f) `re15_spinnenlache_raum_reset()` in `re15_re2z_rng_reset` | **grün**: `unit_r41_spinnengore`, `unit_re2_spider_ai`, `unit_re2_baby_spider_dmg` und `integration_r41_spinnengore` (125 s) bestehen alle. **Kein Riegel bemerkt, dass der Fleck nie gezeichnet wird und beim Raumwechsel liegen bleibt** |

Zur Lücke aus Gegenprobe C:

- M7 ruft `re15_spinnenlache_raum_reset()` direkt auf (`test_r41_spinnengore.c:601`), nicht über
  `re15_re2z_rng_reset`.
- Kein Integrationslauf setzt `RE15_BABYTOD_TEST`, und keiner liest `lache.log.flecken`.
- Damit hängen der einzige Fensterbeleg des Babyflecks (Messhaken) und seine Löschung beim Raumwechsel an keinem Riegel.
  Fällt (f) weg, liegen grüne Flecken des alten Raums an denselben Koordinaten im neuen Raum.
- Der Rest des Baus hat Riegel, auch einzeln und gegen einen Takt-Fehler um eins.

Zur Integration E7R15: `pruef_lache r15` verlangt nur das Endmaß, nicht `sichtbar` (`test_r41_spinnengore.c:646`). In
2071 bleibt die RE1.5-Lache immer unsichtbar (Lauf e, Offen 24). Der Riegel prüft dort also das Modul, nicht das Bild.
Das ist offen benannt und kein eigener Mangel.

## 4. Die Gore-Inventur in Stichproben selbst gemessen

- **Platzen der Spinne**, unter beiden Gehirnen, für Leon und Elza, dazu der Schalter: Abschnitt 1, alles wie im
  Dossier.
- **Lache**:
  - RE2: 90 Schritte bis 2020 x 2120 mit `bf10bf`, in a und b. Im Bild grün.
  - RE1.5: 91 Schritte bis 2028 x 2128 mit `9f80df`, Endphase 2, in e und f. Im Bild dunkel und grünstichig.
  - Der Weg 0x8011461c (121 Schritte, 2268) kam in meinen Läufen nicht vor. Mit w7 und w8 platzt die RE1.5-Spinne immer
    über den Platzweg. Belegt ist er nur durch M3 und die Bytes.
- **Babytod grün**: L8 gegen Gegenprobe A (Abschnitt 3). Im Fenster stirbt kein Baby (Abschnitt 1).
- **Hund-Teilewurf** (Commit 9fb1c73e2, Riegel K): Bisher gab es dafür keinen Fensterbeleg, jetzt gibt es einen
  (Lauf h, `c09`).
- **Nebenwirkung auf lebende Babys** (beabsichtigt, belegt): Ihr Schatten ist jetzt 200 x 200 statt 500 x 600 (Lauf a,
  alle 17 Babyplätze `hx=200`, `tinte 000000`). Das ist RE2 @0x8010024c-50 und steht im Dossier nur in NB2.2 und M7. Für
  die Release Notes ist es eine sichtbare Änderung an jeder Babyspinne. Kein Mangel.

## 5. Das Maximum aus RE1.5, RE2 und den Videos je Typ

Ich habe selbst aus `Version_80%/Resident Evil 1.5 (All Footage).mp4` geschnitten:

- **S05, B 7:37.0–7:41.4 mit 10 Bildern/s** (`c07`): Bei **B 7:40.4** steht ein matter grüner Schein neben dem
  Vorderkörper der Spinne. Die Spinne bleibt ganz, bei 7:40.3 und 7:40.5 ist nur ihr rotbrauner Kopf zu sehen. Das
  bestätige ich: Der Treffer ist grün.
- **S04, B 5:42–6:02 mit 2 Bildern/s** (`c08`): Eine Spinne kommt von der Decke (5:42.5–5:45.5). Ab 5:50 liegt eine tote
  Spinne auf dem Rücken mit angezogenen Beinen. Ein Platzen gibt es nicht, und eine Lache ist im dunklen Moosgang nicht zu
  erkennen. Das bestätige ich.

### 5.1 MANGEL 1: Krähe 0x21, die Federn beim Tod sind in RE2 belegt, das Dossier sagt „fehlt in beiden Quellen“

Das Dossier schreibt an drei Stellen:

- `S_spinnengore.md:258`: „Feder-Sprite (Video K01) ohne Mechanismus in beiden“;
- `:337`: „Kraehe, Feder-Sprite (Video K01): Mechanismus fehlt in beiden Quellen“;
- `:575`, Offen 15: „Der Mechanismus fehlt in beiden Quellen (wie bisher Offen 6)“.

RE2 hat ihn. Gelesen in `info/re2leon/COMMON/BIN/EMOVL21_S0.BIN` (RE2-Krähe), capstone:

```
GIB-Tod 0x80102CA0:
80102e90 jal 0x8010459c   ; Federburst (3x, @0x80102e90/98/a0)
80102ea8 jal 0x80015fe8   ; rand
80102eb4 andi v0,v0,3
80102eb8 jal 0x80104678   ; FUN_80104678(self, (rand & 3) | 8)  -> 8..11 Federn
80102ebc ori a1,v0,8
FUN_80104678 (je Feder):
801046a4-e0               ; Offsets x/y/z = sll 24 / sra 22 (+-512) um self+0x38/0x3c/0x40
801046ec lui a0,0xc00 / 801046f0 ori a0,a0,0x800     ; FX-Wort 0x0C000800
801046f4 sll a1,v0,4                                 ; a1 = rand << 4
80104700 jal 0x8001bf10                              ; RE2-ESP-Spawner (Id = a0 >> 24 @0x8001bf14 = 0x0C)
```

Dazu kommen beim Treffer 2..3 Federn (`re2c_feather(e, (rand & 1) | 2)` @0x80102A08-1C). Der Port kennt das selbst:

- `enemy_ai_re2_crow.c:202-212` nennt Id 0x0C „das RE2-FEDER-FX“ und die Todesstelle „8..11 Federn @0x80102EA8-BC“.
- `re2c_feather` zieht aber nur die vier Zufallszahlen je Feder und zeichnet nichts („Das Feder-Sprite selbst bleibt
  OPEN“).

Das Video K01 (B 1:11.8–1:15.8) zeigt „schwarze Fetzen und Federn“. Das hat die Abnahme 2 bestätigt.

**Bewertung:** Nach der Nutzerregel gilt beim Gore je Typ das Maximum aus RE1.5, RE2 und den Videos. Die Federn beim Tod
und beim Treffer sind in RE2 mit Bytes belegt und im Video zu sehen. Der Port zeichnet sie nicht, und das Dossier ordnet
sie als „ohne Mechanismus in beiden Quellen“ ein. Das ist derselbe Fehler, den die Abnahme 2 bei der Spinnenlache als
Mangel 1 gewertet hat. Offen ist noch, ob die RE2-Federn am Boden liegen bleiben, wie im Video. Das hängt an der
Zeilenmaschine von Effekt 0x0C und ist nicht geprüft.

**Nachbesserung:**

- Entweder die Federn bauen: Tod 8..11, Treffer 2..3, an der Stelle von FUN_80104678. Das RE2-ESP 0x0C bzw. ein Stand-in
  ist PORT-WAHL wie bei `re2s_blut`.
- Oder sie mindestens an allen drei Stellen als „in RE2 belegt (FUN_80104678 @0x80102EA8-BC, FX 0x0C000800
  @0x801046EC-F0), im Port ohne Zeichner, nicht gebaut“ führen und den Grund nennen.
- Die Einordnung „fehlt in beiden Quellen“ ist zu streichen.

### Was ich nicht als Mangel werte

- Die Wandflecken der Spinne (S01): Weder RE1.5 noch RE2 haben Wand-Decals. Das ist Gestaltung nach Bild und ist benannt.
- Die PORT-WAHL zur Basis der Lache (1300 statt 800): Sie ist benannt (Offen 21). Die Folge für den Schuh steht in
  Abschnitt 1.
- Offen 18 bis 24 der Nachbesserung 2: Sie sind benannt. Offen 18 und 24 habe ich selbst bestätigt.

## 6. Nebenwirkungen

Unit-Zielauswahl, 2 Läufe mit `-j 6`, zusammen **300/300 grün** (die Überlappung ist gering):

- **Lauf 1, 56 Tests:**
  - Spinnen und Babys: `unit_r41_spinnengore`, `unit_r41_spinnenanim`, `unit_r38_spinnen`, `unit_spinne_klettern`
    (Wandspinnen, Runde 42), `unit_re2_spider_ai`, `unit_adult_spider_ai`, `unit_spider_ai`, `unit_re2_baby_spider_dmg`,
    `unit_re2_spider_water_2090`, `unit_r41_gatorszene_spinnen`, `probe_spinne_elevation`, `unit_r38_spiderman`,
    `unit_r38_griff_spiderman_*`.
  - Filter, Trefferpause und Tod-Bits: `unit_re2_hit_repeat`, `unit_re2_hit_repeat_crow_spider`,
    `unit_re2_zombie_hitfilter`, `r29_1d3_doppelabzug`, `unit_r34n_f_leiche_latch`, `unit_writher_kill_flag`,
    `unit_r35_zgirl_killflag`, `unit_flag_gate`.
  - Schatten und Lachen: `unit_r36_pri_schatten`, `unit_re2z_corpse_pool`, `unit_re2_crow_shadow_release`,
    `unit_crow_death_corpse`.
  - Gore und FX: `unit_re2_gore*`, `unit_zombie_gore`, `unit_r34_re2fx*`, `unit_r34_plattform*`, `unit_esp_parse`,
    `unit_fx_emitter_ai`, `unit_montage_fx`.
  - Hund und Krähe: `unit_r30_hund_tod*`, `unit_re2_dog_ai`, `unit_dog_hit`, `unit_re2_dog_hitwindow`,
    `unit_re2_crow_ai`, `unit_r36_schuss_kraehe`.
  - Waffen: `unit_r35_redhawk`, `unit_r35_granate`, `unit_r36_granate`, `unit_re2_weapon_rows`, `unit_r34_reaktion`.
- **Lauf 2, 244 Tests** (397 s): alle `unit_re2z_*` (der Raumwechsel-Haken sitzt in `re15_re2z_rng_reset`), alle
  `unit_r41_*` (134), `unit_r35_affen_*`, `unit_r38_affen3_*`, `unit_r37_re2zombie_*`, `unit_r39_gegner_*`,
  `unit_re2_crow_*`, `unit_crow_*`, `unit_r37_masken36_*`, `unit_r38_griff_*`, `unit_r36_kleidung_*`,
  `unit_r36_zombieki_*`.

Integration einzeln nacheinander, **5/5 grün**:

- `integration_r41_spinnengore` 115 s;
- `integration_r38_spinnen_echtlauf` 127 s (Babys, Bisse, Gift);
- `integration_r41_spinnenanim` 147 s (Schachtspinne);
- `integration_r35_redhawk` 18 s;
- `integration_r36_pri_schatten` 34 s (Schatten in der OT-Ordnung, wegen der neuen großen Vierecke).

Aus dem Code:

- `re15_spinnenlache_render` gibt für den RE1.5-Feuer-Emitter 0x26 0 zurück, weil er kein RE2-Baby ist (M9).
- Lebende Spinnen ohne Tinte behalten das Spur-L-Maß (M10).
- `re2z_gaitrow` (+0x16B) und `re2s_gs225` haben außerhalb des Zombie- bzw. Spinnenmoduls keine Leser, und
  `grab_kill_ctr` (+0x9E) nutzt sonst nur `spiderman_r38.c` am lebenden Kletterer.
- Tod-Bits, `em_flag_id`, Trefferpause und Latch fasst der Commit nicht an.

## 7. Der Schalter `RE15_KEIN_80PROZENT_GORE`

Er wirkt (Lauf c). Es gilt ta=0 und sh=0, die Spinne bleibt im Bild ganz, und es fliegen keine Scherben. Das grüne
`re2s_blut` und die Lache hängen bewusst nicht am Schalter. Beide stehen in den Originalen.

## Mängel (für die Nachbesserung)

1. **Krähe 0x21, Federn beim Tod und beim Treffer: in RE2 belegt, im Port nicht gezeichnet, im Dossier dreimal als „ohne
   Mechanismus in beiden Quellen“ eingeordnet** (`S_spinnengore.md:258`, `:337`, `:575`).
   - RE2 EMOVL21_S0: GIB-Tod `jal 0x80104678` mit `ori a1,v0,8` @0x80102eb8-bc (8..11 Federn), Treffer 2..3 Federn
     @0x80102A08-1C. FUN_80104678 `lui a0,0xc00` / `ori a0,a0,0x800` @0x801046ec-f0, `jal 0x8001bf10` @0x80104700.
   - Der Port-Kommentar `enemy_ai_re2_crow.c:202-212` benennt es selbst. Video K01.
   - Nachbesserung: bauen (PORT-WAHL Stand-in) oder mit diesen Adressen als „belegt, nicht gebaut“ führen und die
     Einordnung streichen.
2. **Riegel-Lücke Babyfleck.** Der Fleck-Zeichner in `main.c` (Flecken-Schleife) und `re15_spinnenlache_raum_reset()` in
   `re15_re2z_rng_reset` sind ohne Riegel. Gegenprobe C (beide ausgebaut) lässt `unit_r41_spinnengore`,
   `unit_re2_spider_ai`, `unit_re2_baby_spider_dmg` und `integration_r41_spinnengore` grün.
   - Nachbesserung: ein Integrationslauf mit `RE15_BABYTOD_TEST`, der `lache.log.flecken` liest (`sichtbar=1`,
     `bf10bf`, 200 x 200).
   - Dazu ein Unit-Schritt, der den Raumwechsel über `re15_re2z_rng_reset` (bzw. `re15_actor_init`) auslöst statt über
     `re15_spinnenlache_raum_reset` direkt.
3. **Falsches Adresszitat**: `include/re15_spinnenlache_r41.h:56` `RE15_SPINNENLACHE_FLECKEN_MAX 50 @0x800164a4`. Dort
   steht `beqz v1`, die 50 steht @0x8001649c (`addiu v1,zero,0x32`, ebenso @0x80016350 und @0x800169a4). Das Zitat ist
   zu berichtigen.

Hinweise ohne Mängelrang:

- Der grüne Schuh über der Lache (Abschnitt 1, `c06`) folgt der OT-Ordnung wie im Original (@0x80016b6c/9c). Durch die
  PORT-WAHL der Basis wird er größer.
- Die RE1.5-Lache ist in der Elza-Kamera von 2071 nie sichtbar (Offen 24).
- Der Schatten lebender Babys ist jetzt 200 x 200. Das gehört in die Release Notes.
- Für Offen 18 verlangt das Zertret-Tor zusätzlich die Pad-Bits `0x800CFBF6 & 6` (@0x801003ec-f8).
