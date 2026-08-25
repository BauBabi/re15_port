# ROOM1210-Kontakt — Zusammenfuehrung der drei Befunde + Gegenpruefungen

Stand 2026-08-29. Alle strittigen Punkte hier sind von mir **selbst disassembliert** bzw.
**selbst gemessen**, nicht nach Mehrheit entschieden. Werkzeug:
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, eigene Python-Dumps auf
`re15_port/shared_assets/PSX/STAGE1/ROOM1210.RDT`, und ein voller
`bash re15_port/tools/local_build.sh all`.

Detail je Finding: `1210-ausloeser.md`, `1210-geraeusch.md`, `1210-reichweite.md`.

---

## 0. Der Befund, der alles andere ueberlagert: der Arbeitsbaum ist bereits geaendert — und ROT

`git status`: `re15_port/engine/src/enemy_ai_common.c` (+104), `tests/unit/CMakeLists.txt` (+21),
`tests/unit/test_1210_gitterhaende.c` (54 Zeilen) sind **uncommittet modifiziert**.
HEAD = `32f2e9e4`. Alle drei Befunde behaupten "Kein Produktivcode geaendert"; zwei behaupten
zusaetzlich "PORT HEUTE REACH_Z 1700" bzw. "Voller Build gruen". Beides trifft nicht zu.

**GEMESSEN, heute, voller Lauf — 237/239, ZWEI ROTE TESTS, beide durch diesen Batch verursacht:**

| Test | Meldung | Ursache (belegt) |
|---|---|---|
| `45 unit_writher_ai` | `FAIL(2): die Lunge muss exakt 2420 Einheiten weit reichen, gemessen 0` | `test_writher_ai.c:29` setzt `pl->z = 900`; der Baum hat `RE15_WRITHER_REACH_Z` von 1700 auf **850** gesenkt (`enemy_ai_common.c:11126`). 900 > 850 ⇒ Tor zu ⇒ Lunge 0. Der Test nennt in Zeile 107 selbst noch "REACH_Z = 1700 @0x1EAE". |
| `224 unit_1210_gitterhaende` | `FAIL: und jeder Griff endet wieder (3 Zugriffe, 2 mal frei)` | Der dritte Griff laeuft am Fensterende der Messung noch. Ursache **nicht** auf einen Mechanismus zurueckgefuehrt (Fensterlaenge vs. schnelleres Nachgreifen durch das neue Hand-Tor) — siehe OFFEN. |

`integration_relatch_pin`, `integration_save_counter_pin`, `integration_boot_bg_pin` sind
**gruen** — die entsprechenden Behauptungen aus zwei Gegenpruefungen sind damit erledigt.

**Naechste Handlung vor allem anderen:** die zwei roten Tests aufloesen. Solange sie rot sind,
ist keine der drei Empfehlungen abnahmefaehig.

---

## 1. Der Rahmen des Nutzers — Stand der Sache

Der Nutzer hat verworfen, dass **alle zehn Arme gleichzeitig** ausfahren. Genau das tut das
Original: `sub02` @Datei `0x1ec8` schreibt in **einem** Bild allen zehn Armen `+0x9 = 1`
(10x `2e 02 0N 00` + `34 0c 01 00`, @0x1ED6..0x1F27; Member 12 -> `sb a2,9(a0)` @0x800411f8).

**Keiner der drei Befunde schlaegt eine Rueckkehr dorthin vor.** Das ist korrekt.

⛔ Aber alle drei verschleiern die Konsequenz: **das gesamte Pro-Arm-Naeherungstor ist eine
dauerhafte, bewusste Abweichung vom byte-true Stand.** Ein Zitat fuer eine ZAHL (1700 als
Rechteck-Tiefe) ist kein Zitat fuer den MECHANISMUS (ein Raum-Rechteck, das alle zehn schaltet),
der sie verbraucht. Der Kommentarblock in `enemy_ai_common.c` verkauft 850 als aus dem Raum
abgeleitetes Mass ("der Raum sagt selbst, wie nah der Spieler sein muss") — das ist eine
Inferenz, keine Fundstelle. Es muss als Nachruestung etikettiert werden.

### ⛔ NEU GEMESSEN — die Frage "hat das Original einen eigenen Staffel-Mechanismus?" ist entschieden

Eine Gegenpruefung behauptete, das Original habe einen **eigenen, pro Arm zufaellig gestaffelten
Weck-Pfad** ueber `+0x9 == 2` -> `+0x5 = 2` -> rng-Timer `+0x1D0` (Saat `(rand&0x1f)+30`), und der
sei nie geprueft worden. **Halb richtig, im entscheidenden Teil falsch — eigene Messung:**

1. Der `+0x9 == 2`-Zweig existiert wirklich (eigene Disasm @0x8010c644-66c, s.u.).
2. **Aber er wird in keinem Writher-Raum betreten.** Byte-Scan ueber alle STAGE-RDTs:
   `34 0c 01 00` (Member_set(12,1)) kommt in genau **drei** Dateien vor —
   `ROOM1210.RDT`, `ROOM1211.RDT`, `ROOM3071.RDT`, also exakt den Writher-Raeumen.
   `34 0c 02 00` kommt in diesen dreien **nicht** vor.
3. **Und der Writher-Baum schreibt `+0x9` niemals selbst**: eigener Store-Scan ueber
   0x8010c1ec..0x8010d800 auf `sb/sh/sw ...,9(rs)` ⇒ **null Treffer**. `+0x9` kommt
   ausschliesslich aus dem Skript.
4. Substate 2 wird trotzdem erreicht — aber **nach** der Lunge:
   `8010c8e0: addiu v0,v0,2` / `8010c8e4: sb v0,5(v1)` ⇒ `+0x5 = (rand&1) + 2`.

**Fazit:** `+0x1D0` ist der **Zuck-Timer nach dem Ausfahren**, kein Weck-Staffler. Er kann das
gleichzeitige Ausfahren konstruktionsbedingt nicht entzerren, weil alle zehn Arme `+0x9 = 1`
im selben Bild bekommen. Damit ist der einzige im Batch vorgeschlagene "byte-true Ausweg aus
dem erfundenen Tor" **widerlegt** — es gibt keinen. Das Pro-Arm-Tor bleibt eine Nachruestung
ohne Original-Entsprechung, und das ist die ehrliche Etikettierung.

*(Caveat zum Byte-Scan: ein Muster-Scan ueber ganze RDTs kann in Daten-Regionen Fehltreffer
liefern. Der tragende Teil — ROOM1210s sub02 enthaelt nur Wert 1 — ist unabhaengig davon durch
den expliziten Offset-Dump @0x1ec8..0x1f27 belegt.)*

---

## 2. Ampel je Finding

| Finding | Kern haelt? | Empfehlung abnahmefaehig? |
|---|---|---|
| **1210-ausloeser** | RE-Kern **JA** (selbst disassembliert). Wirkungs-Prognose **NEIN**. | **NEIN** — bereits angewandt, hat `unit_writher_ai` rot gemacht, Prognose "3->2" gemessen falsch. |
| **1210-geraeusch** | Sound-Belege **JA**. Headline-Aussage **NEIN**. | Teilweise — `re15_audio_room_se(4)` liegt schon im Baum, aber mit einer **falschen** Begruendung im Kommentar und ohne das belegte Cooldown-Tor. |
| **1210-reichweite** | Geometrie-Kern **JA**. Drei Zahlen und ein RE2-Zitat **NEIN**. | **NEIN** — drei als "GEMESSEN" etikettierte Zahlen im Produktivcode sind falsch, `MESH_REACH` stammt aus dem falschen Clip. |

---

## 3. Was ich GESTRICHEN habe (Regel: Konstante ohne Herkunft)

1. **`REACH_X = 11000`** (`enemy_ai_common.c:11126`) — keine `@0x...`. Der vorgeschlagene Ersatz
   (`(uint32_t)(pl->x - (-22100)) <= 5200u`) ist **kein** Beleg, sondern ein No-op: nach der
   eigenen Messung des Befundes umschliesst `x -22100..-16900` den erreichbaren Flur
   (`-20622..-18164`) vollstaendig, die x-Haelfte entscheidet also **nie**. Eine
   raumfeste Absolut-Koordinate in einer per-TYP globalen Funktion
   (`else if (t == 0x1a) re15_writher_ai_tick(s);`) ist zusaetzlich eine latente Falle.
   **Gestrichen.**
2. **`RE15_WRITHER_MESH_REACH = 1671` in seiner jetzigen Verwendung** — die Zahl ist das Maximum
   von **Clip 2**, das Tor laeuft aber in `case 2`, wo der Port `e->motion = 1` setzt, also
   **Clip 1** (Maximum 1521). Eine Konstante aus einem Clip, der im Bewertungsmoment gar nicht
   spielt, hat fuer diesen Moment keine Herkunft. **Gestrichen in dieser Form.**
3. **"RE2 hat ein zweites Tor bei 2000 -> Zustand 0xE01"** als Implementierungsweg — beruht auf
   einer falsch gelesenen Instruktion (eigene Disasm, s. `1210-reichweite.md`). **Gestrichen.**
4. **"Schritt 3": ein Laut beim Ausfahren** — der Befund benennt selbst, dass es dafuer weder in
   RE1.5 noch in RE2 einen Beleg gibt, und schlaegt korrekt keine Zahl vor. **Bleibt draussen.**
5. **`REACH_Z = 850` bleibt im Baum, aber die Etikettierung wird gestrichen.** 850 ist die halbe
   Tiefe eines **raumfesten** Rechtecks, umgehaengt auf jeden Arm. Das ist Rechnung + Port-Zutat,
   kein Zitat. Der Satz "der Raum sagt selbst, wie nah der Spieler sein muss" faellt.

---

## 4. Die kuerzeste Handlungsliste

1. `unit_writher_ai` und `unit_1210_gitterhaende` gruen bekommen (Ursache der 3. Griff-Lage
   messen, nicht die Schranke senken — siehe Memory `reai-v2-pin-fixture-verschiebung`).
2. Drei falsche "GEMESSEN"-Zahlen in `enemy_ai_common.c` korrigieren: die Gleichung
   `450+1060+300+2*18 = 1828` (ergibt 1846), "Spitzen-Abstand 157 (West)" (richtig **269**),
   "die Hand ragt 293 raus" (richtig **181**).
3. Den Block "WAS DIE RAUM-GEOMETRIE ERLAUBT" (1348..2839 "ausserhalb des begehbaren Bodens",
   "Arm 3 mit Abstand 0") loeschen — er beruht auf einer Fehldeutung von
   `re15_collision_on_floor` (liefert 1 **innerhalb** einer Zelle; ROOM1210s Zellen sind die
   **Waende**). Von mir am Quelltext bestaetigt.
4. Den falschen Satz "SE4 ... wird deshalb nie vom Prio-Tor verworfen" korrigieren
   (eigene Disasm: `andi a1,a1,0x7` @0x80045a30 + `sltiu v0,v0,0x8` @0x80045a54 ⇒ SE4s
   Nibble 0xb = 11 >= 8 verwirft bei Gleichstand gegen sich selbst).
5. **Vor jeder weiteren Sound-Aussage:** die Id-Verschiebung `+12` @0x80045418 RE'en
   (`andi v0,v0,0x2000` @0x8004540c). Ist das Bit im Griff-Moment gesetzt, trifft
   `re15_audio_room_se(4)` **Record 16**, nicht Record 4 — dann ist die ganze
   "SE4 = Tone 5 = Stimme 5 = Nibble 0xb"-Kette gegenstandslos. Keine der drei Analysen
   zitiert dieses Gate.
6. **gdigrab-Verifikation.** Der Ausloeser war ein VISUELLER Nutzer-Report; nach
   `reai-v2-visual-verify-gdigrab` ist das Pflicht und in keinem der sechs Dokumente passiert.
