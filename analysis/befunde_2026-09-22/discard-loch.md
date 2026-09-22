# Das Loch in der Wegwerf-Abfrage — und was RE2 an dieser Stelle wirklich tut

Runde 22, 2026-09-22. Vorgänger: `discard-umsetzung.md` (Bau), `discard-nacharbeit.md`
(zwei Sperrgründe). Dieses Dossier behandelt den Sperrgrund, der durch die zweite
Nachbesserung **entstanden** ist: die Abfrage konnte still verschwinden.

Suite **336/336** (`bash re15_port/tools/local_build.sh all`).

---

## 0. Kurzfassung

| | |
|---|---|
| **Der Fehler** | 81 Bilder mit freiem Pad und armierter, unsichtbarer Abfrage. Raumwechsel darin = Abfrage weg, Gegenstand für immer im Inventar. |
| **Die Antwort aus RE2** | Die Lage **kann in RE2 nicht entstehen**. „Vorgemerkt" ist dort *genau* die Spanne, in der die auslösende Nachricht die Freeze-Maske `0xFF000000` hält. Das Loch war eine **Port-Eigenschaft**. |
| **Der Fix** | Solange vorgemerkt ist, hält der Port denselben Pad-Riegel wie RE2 → der Spieler kann das Fenster nicht mehr nutzen. Für den einzigen Restfall (Raumwechsel durch das **Skript**) überlebt die Vormerkung — als benannte **Port-Entscheidung**, gestützt auf RE2s Datenlage. |
| **Riegel** | Teil G: 0 Löcher jetzt / **192** vorher, größtes 80 Bilder. Teil H: Raumwechsel im Wartefenster, Abfrage bleibt, Gegenprobe rot. |

---

## 1. ⛔ Was RE2 tut — die Frage hat dort keinen Fall

Gefragt war: *was passiert in RE2, wenn der Spieler nach dem Aufschließen den Raum
wechselt, bevor die Abfrage kam?* Die Antwort ist nicht „sie überlebt" und nicht „sie
geht verloren", sondern: **der Spieler kommt dort nicht dazu.** Sechs Schritte, jeder
disassembliert.

**(1) Vormerkung und Freeze entstehen in derselben Instruktion-Folge.** Die Routine, die
die Fortsetzung einhängt, öffnet unmittelbar davor die Nachricht „You have used the
\<X\>." mit der Freeze-Maske als viertem Argument:

    8005164c  jal  FUN_8002fe38
    80051650  _lui a3,0xff00          ; Delay-Slot = 4. Argument = 0xFF000000
    80051670  sw   v0,DAT_800d4498    ; LAB_80051718 einhaengen

**(2) Ab da ruft die Hauptschleife die Fortsetzung jedes Bild.**

    80026370  lw   v1,-0x7d50(at)     ; at = s3+0x10000, s3 = 0x800cc1e8 -> DAT_800d4498
    8002637c  beq  v1,zero,LAB_8002638c
    80026384  jalr v1

**(3) Ihr einziger Aufschub ist das Belegt-Bit `0x80` von `DAT_800e873c`.**

    800517e0  lbu  v0,-0x78c4(v0)     ; DAT_800e873c
    800517f0  andi v0,v0,0x80
    800517f4  bne  v0,zero,LAB_80051870   ; -> return, NICHTS angefasst

**(4) Genau dieses Bit hält auch den Freeze.** `FUN_8002fe38` setzt beim Öffnen
`DAT_800e873c = 0x80`, legt die Maske nach `DAT_800e8760` und sichert das Steuerwort nach
`DAT_800e875c` (Decompile `RE2_Quellcode_V2/FUN_8002fe38.c`, Modus-Zweig `0x100`).
`FUN_8003027c` legt sie an und nimmt sie **erst beim Schließen** zurück:

    800307e0  lbu  v0,0x5c8c(s1)      ; s1 = 0x800e2ab0 -> DAT_800e873c
    800307e8  andi v0,v0,0x7f         ; Belegt-Bit WEG
    800307ec  sb   v0,0x5c8c(s1)
    800307e4  lw   v1,0x5cac(s1)      ; DAT_800e875c (Steuerwort vorher)
    800307f4  sw   v1,DAT_800cfbdc    ; Freeze ZURUECK

**(5) Und die Nachricht schließt nur auf die Bestätigungstaste — und stopft im selben
Bild das Pad-Vorwort**, es gibt dort also keine zweite Flanke:

    800307a0  lw   v0,DAT_800ce310
    800307a8  andi v0,v0,0x3000       ; Bestaetigen?
    800307ac  beq  v0,zero,default    ; nein -> Nachricht bleibt offen
    800307b0  _ori v0,zero,0xffff
    800307b8  sw   v0,DAT_800ce30c    ; Pad-Vorwort gestopft

**(6) Im nächsten Durchgang wird dekrementiert, gefragt und sofort wieder eingefroren.**

    80051810  addiu v0,v0,-1          ; Zaehler -= 1
    80051824  bne  v0,zero,LAB_8005185c
    80051834  jal  FUN_8002fe38       ; a1=0x100, a2=9 "This key is useless now / Discard?"
    80051844  lui  v1,0xff00
    80051850  sw   v0,DAT_800cfbdc    ; wieder eingefroren

> **Ergebnis:** In RE2 ist die Spanne „vorgemerkt" **identisch** mit der Spanne
> „Nachricht hält `0xFF000000`". Ein Bild mit vorgemerkter Abfrage **und** freiem Spieler
> gibt es nicht. Deshalb kann der Spieler dort in diesem Fenster keinen Raum wechseln —
> **das Loch ist keine RE2-Eigenschaft, sondern eine Port-Eigenschaft.** Es entsteht erst
> durch die längere Port-Wartezeit (Ende des auslösenden Unterprogramms statt Ende der
> Nachricht — die Port-Schranke mit RE2-Vorbild aus Runde 21, die bleibt).

### 1a. Was aus RE2 trotzdem messbar ist — und nur das

Der armierte Zustand sind **zwei** Zellen im raum-übergreifenden Arbeitsblock
(Basis `0x800cc1e8`, `@0x8005172c addiu s0,s0,-0x3e18`):

| Zelle | Offset | Bedeutung |
|---|---|---|
| `DAT_800d4498` | +0x82b0 | Fortsetzungs-Zeiger |
| `DAT_800d4249` | +0x8061 | Phase / Platz+1 (`@0x8005171c lbu`, `@0x800517e4 addiu v1,v1,-1`) |
| `DAT_800d4a3c` | +0x8854 | **das Inventar** (`@0x80051774`, `@0x80051808`) |

Vollzensus per Byte-Muster über den ganzen Ghidra-Dump nach beiden Adressierungsformen
des Zeigers (`b0 82 ?? ac` = +0x82b0 und `98 44 ?? ac` = `DAT_800d4498(at)`) — im ganzen
Programm **6 Schreib- und 2 Lesestellen**:

| Adresse | |
|---|---|
| `@0x80051670` | `sw v0` — `LAB_80051718` einhängen (Schlüssel benutzt) |
| `@0x800517d0` | `sw zero` — nach der Antwort aushängen |
| `@0x80051860` | `sw zero` — Zähler ≠ 0, keine Frage, aushängen |
| `@0x80052168` | `sw v0` — `DAT_800522cc` einhängen (andere AOT-Klasse) |
| `@0x80052294` | `sw v0` — dito |
| `@0x800524e8` | `sw zero` — deren eigenes Aushängen |
| `@0x80026370` | `lw v1` — **Hauptschleife** |
| `@0x80052048` | `lw v0` — „hängt schon eine Fortsetzung?" → dann nicht starten |

⛔ **Ghidras eigene XREF-Liste nennt nur fünf Schreibstellen.** `@0x800524e8` adressiert
über `s1` (`@0x800524e4 addu at,s1,at`) und ist dort unbeschriftet — wer der XREF-Liste
vertraut, fährt mit einer unvollständigen Liste. Die Byte-Muster-Suche findet sie.

Dazu zwei Punkte, die für das Überleben nötig wären:

* **Keine** der sechs liegt auf einem Raumwechsel-, Tür- oder Ladepfad.
* Die Fortsetzung ist **resident**: `LAB_80051718` liegt in der EXE (`0x8005xxxx`), nicht
  in einem Stage-Overlay (`0x80100000`). Läge sie im Overlay, wäre Überleben unmöglich.
* Der Gegenstand ist im Wartezustand **unversehrt**: `@0x800517f4` kehrt **vor** dem
  Dekrement `@0x80051810` um, ohne die Anzahl anzufassen.

> Die Zellen **würden** einen Raumwechsel überstehen. Dass RE2 das **nutzt**, ist damit
> **nicht** gezeigt — es braucht es nie.

### 1b. Grenze der Methode, benannt

Die Byte-Muster-Suche findet **direkte** Zugriffe mit diesen zwei Adressierungsformen.
Ein `memset` über den Block in einer Schleife würde sie nicht sehen. Gesucht und **nicht**
gefunden: kein Decompilat in `RE2_Quellcode_V2/` / `RE2_Quellcode_Overlays/` nennt
`800d4498`; die Stellen, die `&DAT_800cc1e8` als Basis führen (`FUN_8001b710`,
`FUN_80068f9c`, `FUN_800698b4`, `FUN_80069714`, `main.c`), sind Feld-Zugriffe, keine
Block-Löscher. Ein Block-Löscher über +0x82b0 läge außerdem nur 0x5a4 Byte unter dem
Inventar (+0x8854), das den Raumwechsel nachweislich übersteht. Das ist ein **Indiz**,
kein Beweis — und es ist als Indiz gekennzeichnet, weil die Port-Entscheidung ohnehin
nicht daran hängt (siehe §2).

---

## 2. Der Fix — zwei Teile, beide benannt

### Teil 1: der Pad-Riegel schließt das Fenster (RE2-Lage)

`re15_discard_pad_locked()` hält, solange vorgemerkt ist, das Pad-Bit `0x01000000` —
dieselbe Lage, in der RE2 während derselben Spanne ist. Die **Wirkung** des Bits ist in
RE1.5 wörtlich disassembliert:

    800304f4  lw   v0,DAT_800aca40
    800304f8  lui  v1,0x100
    80030500  beq  v0,zero,LAB_80030520
    80030514  andi v0,v0,0xf000      ; nur noch die vier Menuebits
    8003051c  sw   v0,DAT_800ac768

Die vier Menübits bleiben offen — die Abfrage selbst muss bedienbar sein.

⛔ **Nur** das Pad-Bit, **nicht** die ganze `0xFF000000`: `RE15_PAUSE_SCD` (`0x02000000`)
würde das auslösende Unterprogramm anhalten, auf dessen Ende der Port gerade wartet — das
wäre eine Verklemmung. RE2 hat das Problem nicht, weil seine Wartezeit die Nachricht
selbst ist.

### Teil 2: die Vormerkung überlebt den Raumwechsel — ⛔ PORT-ENTSCHEIDUNG

Nach Teil 1 kann der **Spieler** im Wartefenster keinen Raum mehr wechseln. Es bleibt
**ein** Fall: gerade das Unterprogramm, auf dessen Ende gewartet wird, darf selbst einen
Raumwechsel enthalten. Dafür fällt die Vormerkung nicht weg, weil das der einzige Ausgang
ohne stummen Verlust ist — Item `0x44` „Minidisc Player w/ Disc" hat genau **eine**
Benutzungsstelle (`discard_sites.inc`), es würde also nie wieder gefragt.

Die Wahl folgt RE2s Datenlage (§1a), ist aber **keine gemessene RE2-Verhaltensweise**. So
steht es im Code (`re15_discard_room_change`, `scd_room_setup.c`, `re15_savedata.c`,
`re15_savedata.h`) und so bleibt es.

Was **nicht** mitgenommen wird: der Faden-Index (die Fäden des alten Raums gibt es nicht
mehr) und der Inventar-Platz (das Inventar kann nachgerückt sein) — der Platz wird beim
Fragen neu gesucht.

**Save/Load:** der Spielstand führt die **Gegenstands-Id** mit (`discard_pending_item`, das
frühere `reserved1`, alte Stände tragen dort 0 — kein Versions-Bump). RE2 speichert seinen
Zustand **nicht**: es ist ein Code-Zeiger, und dort ist „vorgemerkt" nie ein Speicherpunkt.
Durch den Pad-Riegel ist das Feld im Auslieferungsstand praktisch immer 0.

---

## 3. Der Riegel — `re15_port/tests/unit/r21_discard_wegwerfen.c`

### Teil G — „kein Bild mit vorgemerkt UND Pad frei UND unsichtbar"

Über **alle 16** erzeugten Benutzungsstellen, mit dem echten, registrierten Unterprogramm:

```
ABDECKUNG: 10 von 16 Benutzungsstellen gefahren, 6 mit Grund ausgelassen
Loecher JETZT: 0 — Loecher VORHER (nur g_re15_pauseflags): 192,
               groesstes ROOM1100 msg 4 mit 80 Bildern
```

Die 6 Auslassungen sind je benannt (ROOM10D0/10D1/11E0 sub20/11E1/1230/1231: die Stelle
hängt hinter der Ja/Nein-Verzweigung `Ck(12,31,0)` und wird ohne Spieler-Schritt nicht
erreicht) — für ROOM10D0 tritt der Live-Abzug an ihre Stelle (§4).

**Gegenprobe im selben Lauf** (nicht als zweiter Lauf behauptet): dieselbe Zählung mit der
**alten** Bedingung (nur `g_re15_pauseflags`) findet 192 Löcher. Fiele sie auf 0, würde der
Riegel nichts messen — zwei `PRUEFE` erzwingen das (`> 0` und `größtes ≥ 20`).

### Teil H — Raumwechsel im Wartefenster, über den echten Ladeweg

```
ROOM1100 sub02 msg 4 → Raumwechsel nach ROOM1110:
    Abfrage 1 Bild(er) spaeter offen, Gegenstand unversehrt
GEGENPROBE: mit dem alten, unbedingten re15_discard_reset() ist die
            Vormerkung nach dem Raumwechsel weg
```

Gefahren wird `scd_room_reenter` — die Stelle, an der der Aufruf sitzt
(`scd_room_setup.c`), nicht eine Nachbildung. Geprüft wird dabei auch: im Wartefenster ist
die **Anzahl noch 1** (RE2 dekrementiert erst `@0x80051810`) und der Pad **gesperrt**;
nach dem Wechsel ist die Vormerkung da, der Gegenstand unversehrt, und die Abfrage fragt
nach `0x44`, nicht nach etwas anderem.

---

## 4. Bild aus dem laufenden Spiel — selbst angesehen

Echter Renderpfad: `RE15_FRAMEDUMP` liest das komplett komponierte Bild **innerhalb** von
`re15_render_end_frame()` unmittelbar **vor** `SDL_RenderPresent` zurück. Kein
`RE15_AUTOSHOT`, kein Softwarerenderer. Skript:
`analysis/befunde_2026-09-22/discard-loch/abzug_discard.sh`, ROOM10D0-Kartenleser, Blaue
Keycard `0x38`.

| Bild | Was zu sehen ist |
|---|---|
| `bild/bild000304.png` | Wartefenster: vorgemerkt, **unsichtbar**, Folgenachricht läuft |
| `bild/bild000336.png` | Abfrage geht auf, Schreibmaschine bei 5/44 Glyphen („You d…") |
| `bild/bild000388.png` | **„You don't need this key any more. Discard it?"** ▶ Yes    No |

`bild000388.png` zeigt den RE1.5-Wortlaut `@0x800C508B` mit **Yes vorbelegt** — der
Cursor steht links von „Yes".

Der **Pad-Riegel** ist am Protokoll ablesbar, nicht behauptet: das Abzug-Skript drückt
nach den Quadrat-Stößen 6 s **vorwärts**, während die Abfrage vorgemerkt und unsichtbar
ist. `bild/discard.log` (2185 Zeilen, jede Zeile ein Bild):

```
F303  abfrage=1 frage=0 ... padsperre=1 pausepad=0 px=1278 pz=-5750
F333  abfrage=1 frage=8 gegenstand=0x38 wahl=0 text=0/44 ... px=1278 pz=-5750
```

* Bilder mit **vorgemerkt + unsichtbar + Pad frei**: **0** (die Loch-Bedingung).
* `px/pz` bleiben über das ganze Fenster auf `1278/-5750` — der Spieler läuft nicht,
  obwohl vorwärts gedrückt wird.

In ROOM10D0 ist das Fenster mit *ausgeredeter* Nachricht nur **ein** Bild breit (F303),
weil das Skript danach weitere Nachrichten hält (`pausepad=1`). Das große Fenster steckt in
ROOM1100 — dort messen es Teil G (80 Bilder) und Teil H.

---

## 5. Die vier Berichtigungen aus dem Urteil

### (1) Bildzahlen neu erhoben — mit registriertem RDT

Teil F rief **kein** `scd_register_current_rdt`. Ohne das hat der VM keine `sub_scd[]`-
Tabelle, `Gosub`/`Evt_exec` sind stille No-Ops, und gemessen wurde ein **verkürztes**
Skript. Jetzt registriert:

| Stelle | vorgemerkt | alter Öffnungszeitpunkt | jetzt |
|---|---|---|---|
| ROOM1100 sub02 msg 4 — Minidisc Player `0x44` | F0 | F84 | **F245** (+161) |
| ROOM11E0 sub21 msg 12 — Pliers `0x30` | F30 | F80 | **F262** (+182) |
| ROOM1090 sub03 msg 9 — Fire Extinguisher `0x31` | F2 | F72 | **endet nicht** |

⛔ **ROOM1090 ist mit registriertem RDT gar nicht mehr messbar** — und das ist der
eigentliche Befund: mit echtem `Gosub` läuft `@0x261C Plc_dest(mode 9, flag 33)` +
`@0x2624 Gosub 5`, und sub05 ist `@0x26F4 Do` / `@0x26FA Edwhile` / `@0x26FC Ck(5,33,0)`,
also eine **Warteschleife auf das Ankunftsflag des Spieler-Weges**. Dieser Riegel fährt
keinen Spieler-Schritt, das Flag fällt nie, der Faden bleibt in sub05 @Datei `0x26F9`
stehen. Die Stelle wird deshalb **mit gemessenem Grund** ausgelassen (der Riegel prüft
dabei, dass der Faden wirklich in einer `Do`/`Edwhile`-Schleife steht — sonst rot) und von
Teil G gefahren, der kein Faden-Ende braucht.

**Die alte 677 ist damit hinfällig — und 684 ist es auch.** Die im Auftrag genannte
Korrekturzahl 684 lässt sich nicht bestätigen: mit registriertem RDT endet das
Unterprogramm in diesem Prüfstand überhaupt nicht. Keine Zahl ist hier besser als eine
geratene.

Abdeckung Teil F: **9 von 16** gefahren, **7** mit Grund ausgelassen, in **9 von 9** geht
die Abfrage jetzt später auf als vorher.

### (2) Header-Begründung in `include/re15_msg_select.h` — falsch, berichtigt

Dort stand, die Skript-Abfrage öffne ebenfalls mit `0x100` und „eigene Koordinaten kann es
gar nicht geben". **Beides falsch.** Der Modus ist `a1 & 0x300` (`@0x80027eb8 andi
a1,a1,0x300`), und es gibt **drei**:

| Modus | Box | Beleg |
|---|---|---|
| `0x100` | (34, 180) | `@0x80027eec ori v0,zero,0x22` / `@0x80027f14 ori v0,zero,0xb4` — Aufnahme-Prompt, Tabelle `@0x800C4FC6` |
| `0x300` | (34, 180) | `@0x80027f00` / `@0x80027f9c` — **die Skript-Abfrage**, `op_message_on` setzt `@0x80040500 ori a1,zero,0x300` |
| `0x200` | (34, **186**) | `@0x80027f4c` / `@0x80027f58 ori v0,zero,0xba` — **hat sehr wohl eigene Koordinaten** |

Richtig ist nur das **Ergebnis**, und zwar gemessen statt gefolgert: `0x100` und `0x300`
schreiben dieselben zwei Konstanten 34/180, alle drei Abfragen landen deshalb auf derselben
Box.

Zusätzlich ist `s1 = 0` (Blink-Schiebeweite) jetzt als **Messung** ausgewiesen, nicht als
Herleitung: `s1` kommt aus `@0x80028148 lbu v0,DAT_800b5456` + `@0x80028168 sltiu
s1,v0,0x1`, ist also 1 genau dann, wenn `DAT_800b5456 == 0`. `DAT_800b5456 = 0x02` in fünf
sauberen Savestates (`lamp_1170_stage1`, `doorA_square`, `fx_R60`, `doorB_walkin`,
`lamp_near`) → `s1 = 0`. Eine Herleitung gibt es dafür nicht; der Wert kommt aus den
Optionen.

### (3) `{0x4001, 2, 0x47}` war tot — Eintrag raus, Begründung war falsch

Die alte Begründung „liegt im mainScd" war falsch: in ROOM4001 gibt es `Message_on 2`
**weder** im mainScd **noch** in einem sub — der Raum sagt nur die Nachrichten 3…12. Der
Text steht im Nachrichtenblock, aber kein Opcode macht ihn auf.

Statt den Eintrag von Hand zu streichen, hat der **Generator** eine vierte
Aufnahmebedingung bekommen (`tools/gen_discard_sites.py`, Regel **D**): der Raum muss die
Nachricht überhaupt ausgeben, also mindestens ein `Message_on <mid>` (Opcode `0x2B`,
`pc[1] == mid`; Handler `@0x800404f4`) enthalten. Damit fällt der Eintrag **abgeleitet**
heraus: **17 → 16 Stellen**, protokolliert in `discard_sites.inc`
(„VERWORFEN ROOM4001 msg 2 … D: kein Message_on 2 im SCD (nur [3…12])").

Alle Zählungen in Riegel, Sonden-`cmake` und Dossier sind auf 16 nachgezogen.

### (4) `bild000391.png` war byte-gleich zu `bild000389.png`

Geprüft und entfernt: `analysis/befunde_2026-09-22/discard-nacharbeit/bild/bild000391.png`
ist gelöscht.

---

## 6. Was steht, unverändert (nicht neu hergeleitet)

* **Wortlaut RE1.5** `@0x800C508B` (DEBUG.BIN, Skript [6] von 8 der Tabelle `@0x800C4FC6`),
  Ja-Aktion `0x02` = wegwerfen `@0x800C50BB`, Ja vorbelegt.
* **Auswahl-Darstellung byte-true:** Zeile 196 (`@0x80027f14` + `@0x80028630`/`@0x80028674`),
  Yes 174 (`@0x80028680`), No 244, Cursor 160/230 (`@0x80028650`), Blink-Maske `0x18`
  `@0x80028600`, Zähler −1/Bild `@0x800285f0` — alle vier Zeichen-Stellen holen aus
  `engine/src/msg_select_common.c`.
* **Mechanismus RE2:** Verbrauchszähler im Inventarplatz (`@0x800D4A3C`, 4 Byte je Platz),
  Dekrement `@0x80051810` hinter der Warte-Schranke `@0x800517f4`, Frage bei 0
  `@0x80051834`; Ja: Platz genullt `@0x80051774`/`@0x80051794`; Nein: Anzahl := 1
  `@0x800517C4`.
* **Sackgasse strukturell ausgeschlossen:** `Keep_Item_ck` (`0x5E`) kommt in 40674 Opcodes
  über 206 RDTs **0 mal** vor.
* ⛔ Die **Szenen-Schranke** (Warten auf das Ende des auslösenden Unterprogramms) ist eine
  **Port-Schranke mit RE2-Vorbild**, keine nachgewiesene RE1.5-Konstante.

---

## 7. Offen, benannt

* **Abbrechen mit CROSS = „No"** bleibt Port-Konvention; RE2s Bestätigen-Taste
  (`FUN_80030844` / `DAT_800ce310 & 0x1000`, vgl. `@0x800307a8` mit Maske `0x3000`) ist
  nicht bis zur Tastenzuordnung aufgelöst.
* **Das Blinken** ist im Live-Bild nicht abgelichtet (Aus-Phase 8 von 32 Takten) — nur im
  Riegel gepinnt (Teil D, über alle 256 Zählerstände).
* **Block-Löscher in RE2** ist als Indiz geführt, nicht als Beweis (§1b). Die
  Port-Entscheidung hängt nicht daran.
* **Die 81 Bilder** aus dem Auftragstext sind am Live-Lauf 80 (Teil G, ROOM1100 msg 4);
  der Unterschied ist die Zählweise der Abbruchkante, nicht ein anderer Befund.
