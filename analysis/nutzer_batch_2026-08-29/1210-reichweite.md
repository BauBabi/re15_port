# 1210-reichweite — "die Arme der Zombies erreichen nirgendwo Leon"

## URSACHE (belegt)

### A. Beide Koerper haengen am selben Gitter — ein URSPRUNGS-Tor kann hier nie feuern

- Spieler-Klemmer: Radius **450** @0x80073e9a, Solid-Maske **1**
  (`lhu a1,6(v0)` / `jal 0x8003b0a4` / `ori a2,zero,0x1` @0x80031d6c-74).
- Arm-Klemmer: Radius **300** @0x80120922, Solid-Maske **4**
  (`lw v0,120(a0)` / `ori a2,zero,0x4` / `lhu a1,6(v0)` / `jal 0x8003b0a4` @0x8010c308-24).
- Dazwischen die SCA-Waende: #6 x -22150..-21090 (**1060** dick), #11 x -17696..-16676 (**1020**).
- Klemm-Haut **18** = `0x12` aus `push_rect` FUN_8003bca8.

Zwei Pruefer haben diese Zitate unabhaengig woertlich am Binary bestaetigt; die SCA-Werte habe ich
selbst aus der RDT gelesen. **BELEGT.**

### B. Der Original-Writher hat weder Schadens- noch Griff-Pfad

Eigener jal-Zensus 0x8010c1ec..0x8010d800 (11 Ziele, s. `1210-geraeusch.md` §A): kein
Damage-Aufruf, kein Opfer-Latch. Der EINZIGE Spieler-Kontakt ist `jal 0x8002aec4` @0x8010c2f8 mit
`a0 = 0x800aca54` (Spieler) — ein Koerper-Schub mit Radius 750, der `arm+0x1c2 |= 1` setzt; der
Arm liest dieses Flag nie (Offset-Scan im Baum: 0 Treffer). **BELEGT.**

⛔ **Wichtige Methoden-Korrektur eines Pruefers, die ich uebernehme:** ein jal-Scan im KI-Baum ist
fuer die Schadensfrage nicht der richtige Test — Spieler-Schaden laeuft in dieser Engine ueber den
Model-Instance-Action-Driver `FUN_80019e20` (`jalr` ueber Tabelle @0x80071d40, Eintraege 0x18/0x19/
0x1F -> `jal 0x80012d60`). Der Pruefer hat die **transitive** jal-Huelle ab allen 11 Writher-Callees
gebildet (34 Funktionen) und weder 0x80012d60 noch 0x80017fa4 noch 0x80019d50 noch 0x80019e20
erreicht. **Die Schlussfolgerung ueberlebt, auf einem staerkeren Argument als dem gelieferten.**

### C. `re15_collision_on_floor` ist NICHT der begehbare Boden — von mir am Quelltext bestaetigt

`re15_port/engine/src/re15_collision.c`, `re15_collision_on_floor` (FUN_8003b7f0):
```c
if ((unsigned)(x - e->x) < e->width && (unsigned)(z - e->z) < e->density) return 1;
```
Es liefert **1 INNERHALB** einer bandgleichen Zelle, und ROOM1210s bandgleiche Zellen (#6, #11)
sind die **Waende**. Der Datei-Kopf sagt es selbst: *"the player walks in the band-MATCH-FREE
complement"*.

⇒ Der Kommentarblock **"WAS DIE RAUM-GEOMETRIE ERLAUBT"** in `enemy_ai_common.c`
("acht der zehn Kreaturen stehen 1348..2839 Einheiten AUSSERHALB des begehbaren Bodens",
"Arm 3 mit Abstand 0 im Griff-Tor") misst die falsche Groesse und ist **falsch**. Er steht heute
noch im Produktivcode. **Loeschen.**

---

## BESTRITTEN — und von mir entschieden

| Behauptung | Entscheidung | Beleg |
|---|---|---|
| `450+1060+300+2*18 = 1828` | **FALSCH GERECHNET** | Ergibt **1846**. Der gemessene Wert 1828 = 450+1060+300+**18** (nur EINE Haut; der Spieler steht exakt auf -21090+450 = -20640 ohne Zuschlag). Reine Arithmetik. **Diese falsche Gleichung steht woertlich in `enemy_ai_common.c` (~11244).** (Ost analog: -17696-450 = -18146, -16676+300+18 = -16358 ⇒ **1788**, nicht 1789.) |
| "die Hand ragt nach der byte-true Lunge **293** (West) in den Flur" | **FALSCH** | 293 gehoert zur ANSCHLAG-Lage (-22468). Die byte-true Lunge (+2420 ab Spawn -25000) endet bei **-22580** und erreicht den Anschlag nie (112 zu kurz); der Port setzt danach auf die Heimat zurueck. Richtig: Spitze -22580+1671 = -20909 ⇒ **181**. Ost (geklemmt) stimmt mit 333. |
| "Spitzen-Abstand **157** (West), Tor feuert" | **FALSCH** | -20909 gegen Spieler-Grenze -20640 = **269**. Tor feuert weiterhin (269 < 1200), aber die als "GEMESSEN" etikettierte Zahl stammt aus einer nie eingenommenen Lage. **Steht heute in `enemy_ai_common.c:11256`.** |
| "Sichere Bahn x -19597..-19229 (368 breit)" | **FALSCH** | Mit den erreichbaren Spitzen (-20909 / -18029): **-19709..-19229 = 480 breit**. |
| "RE2 hat ein ZWEITES Tor bei 2000 (@0x801017c0 **und** @0x801017fc) -> Zustand 0xE01" | **WIDERLEGT, eigene Disasm** | ```801017a4: lw v1,500(s0)``` (= **+0x1F4**, nicht der Abstand) / ```801017ac: and v0,v1,v0``` (Maske 0xC0000000, Flag-Test) / ```801017bc: and v0,v1,v0``` (0x3FFFFFFF) / ```801017c0: sltiu v0,v0,0x7d0```. Nur **@0x801017fc** (`sltiu v0,s2,0x7d0`) misst +0x1F0, und sein Pass-Pfad ist **0x80101808**, nicht 0xE01 (0xE01 = `addiu v0,zero,3585` @0x801017dc haengt am 0x1F4-Zweig, zusaetzlich gegatet durch `jal 0x80015714` @0x801017d0). Der "zweite, moeglicherweise besser belegte Weg" faellt. |
| `RE15_WRITHER_MESH_REACH = 1671` | **falscher Clip** | 1671 ist das Maximum von **Clip 2**. Das Tor laeuft in `case 2`, wo der Port `e->motion = 1` setzt ⇒ **Clip 1**, Maximum **1521**. Die Konstante ueberzeichnet die Hand im Bewertungsmoment um ~150 und ist zusaetzlich ein Maximum ueber alle Bilder (Clip-2-Profil schwankt 1535..1671). |
| "Kein Produktivcode geaendert / Voller Build gruen" | **WIDERLEGT, gemessen** | `git diff --stat`: `enemy_ai_common.c` +104, `test_1210_gitterhaende.c` 54 Zeilen. `local_build.sh all` heute: **237/239**, rot sind `unit_writher_ai` und `unit_1210_gitterhaende`. |
| "0x8002b43c-54 / 0x8002b520 (FUN_8002aec4)" | **falsche Funktion** | FUN_8002aec4 endet @0x8002b490/94. 0x8002b520 liegt in **FUN_8002b498** — der Funktion, die der Writher-Tick eine Instruktion vorher ruft (`jal 0x8002b498` @0x8010c2e8). Sachlich sogar staerker (das Flag wird jedes Bild geloescht), Zitat falsch. |
| "EXE-weiter Scan auf Offset 450/428: KEIN Leser" | **fuer 428 falsch** | Ein Pruefer findet `lw v0,428(s1)` @0x80042874 (Bulk-Strukturkopie, kein semantischer Konsument). Fuer 450 bestaetigt. Schlussfolgerung haelt, die pauschale Formulierung nicht. |
| Test-Harness misst einen geklemmten Arm | **WIDERLEGT, gemessen** | `unit_1210_gitterhaende` gibt heute aus: `bester Arm: slot 3, Landepunkt (-16420,-5897)`. Byte-true waere **-16358**. `re15_writher_step` klemmt gegen `g_room_rdt`, das nur der Raum-Loader fuellt; im Sonden-/Test-Harness ist es leer ⇒ der Arm-Klemmer ist dort ein **No-op**. |

---

## OFFEN

1. ⛔ **Keine visuelle Verifikation.** Weder Finder noch Pruefer haben per gdigrab gesehen, wie
   weit die Hand tatsaechlich aus dem Gitter ragt (181 bzw. 333 Einheiten in einen 3394 breiten
   Flur). Bei einem visuellen Nutzer-Report ist das nach `reai-v2-visual-verify-gdigrab`
   **Pflicht**, bevor ein Fix als wirksam gilt.
2. **Hand-Y (-1729..-3116) nie gegen die Gitterhoehe im BSS abgeglichen.** Ragt die Hand auf
   Gitterhoehe durch massive Geometrie, ist ein feuerndes Tor optisch falsch, egal wie sauber die
   XZ-Rechnung ist.
3. **Erreichbarkeit der Taschen hinter dem Gitter** ruht auf einer 25er-Raster-Flutfuellung ab
   einem gesetzten Startpunkt. Kein Beleg ueber den echten Tuer-/Spawn-Pfad. Die gefaehrliche
   Richtung ist eine **Unter**fuellung: (-14025,-6464) ist besetzbar und nur **567** vom Ursprung
   des Arms 3 entfernt — dass er nicht zaehlt, haengt allein am Konnektivitaets-Urteil.
4. **Erreicht der ORIGINAL-Arm die Anschlaglage?** Der Port kehrt nach jedem Durchgang heim
   (`case 3`, ausdruecklich als Nachruestung markiert). Das Original geht nach B[1] per
   `+0x5 = (rand&1)+2` @0x8010c8e4 in B[2]/B[3]. Ob von dort je wieder B[1] betreten wird — dann
   wuerde die Lunge akkumulieren und der Anschlag WAERE erreichbar — ist nicht disassembliert.
5. **Was RE2 in +0x1F0 schreibt** (Ursprungs- oder Oberflaechen-Abstand). Ein Pruefer hat
   @0x800265a8-e0 in `info/re2leon/PSX.EXE` dekodiert und keine Radien-Subtraktion gefunden
   ⇒ Ursprungs-Abstand. **Ein-Pruefer-verifiziert, von mir nicht nachgemessen** (meine
   `--bin`-Aufloesung auf die RE2-EXE griff nicht).
6. **Arm-Yaw** nur bei Spawn+8 Bildern gemessen. Eine Laufzeit-Drehung verschiebt die gesamte
   Hand-Punkt-Rechnung.
7. **Ursache des roten `unit_1210_gitterhaende`** ("3 Zugriffe, 2 mal frei") ist nicht auf einen
   Mechanismus zurueckgefuehrt: Fensterlaenge der Messung vs. schnelleres Nachgreifen durch das
   neue Hand-Tor.
8. **Spielwirkung des Hand-Tors ueber einen echten Durchlauf** ist nicht gemessen. Bei zehn Armen
   im z-Abstand ~1400 und einem Griff-Band entlang **beider** Waende ist eine nahezu durchgehende
   Griff-Kette zu erwarten. Ob das noch das gewuenschte RE2-Bild ist — offen.

---

## NAECHSTER SCHRITT

**Implementierungsreif (mit `@0x`): NUR Korrekturen, kein neues Verhalten.**

1. Die drei falschen "GEMESSEN"-Zahlen im Kommentar korrigieren: `450+1060+300+2*18 = 1828`
   (⇒ 1846; 1828 = ...+18), "Spitzen-Abstand 157 (West)" (⇒ **269**), "die Hand ragt 293 raus"
   (⇒ **181**). Reine Arithmetik, kein RE noetig.
2. Den Block **"WAS DIE RAUM-GEOMETRIE ERLAUBT"** loeschen (on_floor-Fehldeutung, §C).
3. Den Test-Harness `g_room_rdt` fuellen lassen, damit `re15_writher_step` gegen den echten
   Arm-Klemmer laeuft — sonst misst `unit_1210_gitterhaende` einen Arm, den es nicht gibt
   (Landepunkt -16420 statt -16358). Das ist eine Harness-Reparatur, keine Schranken-Senkung.

**GESTRICHEN (Regel: Konstante ohne Herkunft):**

- **`RE15_WRITHER_MESH_REACH = 1671` in seiner jetzigen Verwendung.** Fuer den Moment, in dem das
  Tor geprueft wird, spielt Clip 1 (Maximum 1521), nicht Clip 2. Ehrlich waere die
  **pose-aktuelle** Vorwaerts-Auslenkung — die der Port ohnehin rechnen kann, weil er denselben
  Render-Transform fuehrt (`skeleton_common.c` / `platform/pc/main.c:7137-7143`, Mesh-Index ==
  Bone-Index, keine Remap). Zweitbeste, immer noch belegbare Fassung: das Maximum des
  **spielenden** Clips.
- **Der "zweite RE2-Weg" ueber 0x7d0 -> 0xE01** — beruht auf einer falsch gelesenen Instruktion
  (@0x801017c0 liest +0x1F4, nicht den Abstand). Kein Kandidat.
- **`REACH_X = 11000`** und sein vorgeschlagener Ersatz — s. `1210-ausloeser.md` §NAECHSTER SCHRITT.
- Die Anhebung der Schwelle 1200 auf 1900+ war vom Befund selbst korrekt verworfen. **Bleibt
  verworfen.**

**Zu RE'en, mit Weg:**

- **Erst gdigrab**, dann alles andere. Der Report ist visuell; die XZ-Arithmetik kann vollstaendig
  stimmen und das Bild trotzdem falsch sein (Punkt 2: Hand durch Gitter-Geometrie).
- **Ob B[1] je erneut betreten wird** (Punkt 4) — entscheidet, ob der Anschlag ueberhaupt eine
  physikalische Groesse dieses Raums ist. Weg: A[2] @0x8010c930 und A[3] @0x8010cb34 vollstaendig
  disassemblieren und alle `sb …,5(…)` im Baum (0x8010c628, c65c, c8e4, cacc, ce0c, cf60, cf94,
  d08c) auf ihre Zielwerte pruefen.
- **Den echten Laufweg** statt der Flutfuellung: `RE15_DEBUG_JUMP="1210@<frame>"` +
  `RE15_INPUT_SCRIPT` ab den fuenf Tuer-Ankunftspunkten, und pro Bild protokollieren, welcher Arm
  im Hand-Tor steht. Das beantwortet Punkt 3 und Punkt 8 in einem Lauf.

---

## ⛔ Rahmen-Pruefung

Der Vorschlag stoesst die Nutzer-Entscheidung **nicht** um — das Pro-Arm-Tor bleibt, es wird nur
sein Messpunkt verschoben. Aber die ehrliche Etikettierung fehlt: **im Original greift ueberhaupt
kein Arm** (kein Griff-Pfad im gesamten Baum, §B), und der Ausloeser schaltet alle zehn auf einen
Schlag. Der ROOM1210-Griff ist damit vollstaendig eine Nachruestung — das Hand-Tor ist eine
Nachruestung **auf** einer Nachruestung. Beides gehoert in den Code-Kommentar und in die
Commit-Message, nicht als "byte-belegt" verkauft.
