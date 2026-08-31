# Nutzer-Batch 2026-08-30 — Zusammenfuehrung und Schiedsspruch

Drei Findings, je zwei bis drei Gegenpruefungen. Bei jedem Widerspruch zwischen Finder und
Pruefer habe ich **selbst disassembliert bzw. selbst geparst** und nicht nach Mehrheit
entschieden. Einzelberichte: `schwarze-dreiecke.md`, `ada-kiste.md`, `1210-re2-verhalten.md`.

## ⛔ Werkzeug-Defekt, der jede RE2-Nachpruefung im Batch betrifft

Die Auftrags-Anleitung *"RE2-EXE: info/re2leon/PSX.EXE (per --bin relativ zu
info/Re1.5/PSX/BIN/)"* funktioniert nicht. `re15_disasm.py` wertet `--bin` nur fuer Adressen
>= 0x80100000 aus; darunter wird hart `info/Re1.5/PSX.EXE` gelesen — **still, ohne Fehler**.
Korrektes Werkzeug fuer RE2-EXE-Adressen: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`.
Das gehoert in die Anleitung, sonst ist jede Nachpruefung ein Blindgang.

---

## 1. schwarze-dreiecke

**URSACHE (belegt):** Dem Port fehlt das Per-Part-Zeichenbit (`part.flags` Bit 0, Part-Array
`entity+0x188`, Stride 0xAC). Der Typ-0x26-INIT loescht es (`addiu v1,zero,-2` / `and` /
`sw v0,0(a0)` @0x801165d0-e4, selbst verifiziert); der Zeichner FUN_8001e9ec springt ohne das Bit
in den Epilog (@0x8001ecc4/@0x8001ecc8); der Binder setzt es normalerweise auf 1
(@0x8001e74c/58). EM26 ist **belegt** ein Modell aus einem Dreieck mit einer praktisch schwarzen
Textur: EMR-count = 1 (CDEMD0.EMS @0x1B5820, `10 00 14 00 01 00 14 00` — die im Befund unbelegte
Praemisse, von mir nachgemessen), MD1 @blob+0x6C mit tv=3/tf=1, Verts (900,0,0)/(-450,-779,0)/
(-450,779,0). Sieben Emitter in ROOM1090 (RDT @0x2214). Der Port kennt kein Part-Bit
(main.c:6580-6612, nur `active` + Region-Cull).

**BESTRITTEN:** (a) "Umfang exakt zwei Typen" ist **widerlegt** — es gibt einen dritten,
generischen Clear **im Zeichner selbst**: FUN_8001f024 @0x8001f12c-34 (`a2 = a0 = Part`,
@0x8001f028), gegated durch Part-Flag 0x20 @0x8001eb14-24. Das Bit ist ein lebendes Per-Frame-
Flag, kein Spawn-Attribut. (b) Der Beleg "0x80072bac[0x26]=0x80116288" zeigt in der Datei auf
eine **NULL** — der Wert wird zur Laufzeit geschrieben (@0x8011e8f4/@0x8011e8fc). (c) Das
STAGE2-Zitat ist nicht woertlich: dazwischen steht `sw a0,-27880(at)` = **Part-Zeiger nach
0x80119318 gecacht** — genau die Klasse Schreiber, fuer die das Suchmuster blind ist.
(d) Die dir-Index-Etiketten sind falsch (count = 8, Offsets [0xc,0x20,0x5c,0x60,0x64,0x68,0x6c,
0xe8] -> 0x6C = Index 6) — **und die Gegenrede des Pruefers ebenfalls**; im Code Datei-Offsets
zitieren. (e) Der Yaw-Einwand des Pruefers ist **falsch**, der Finder hatte recht: Record+16 =
`00 04` LE = 0x0400 = 1024. (f) Der Flavor-A/B-Schluss ist ungueltig (der Messbuild enthaelt
v0.3.29 schon); der Widerruf der v0.3.29-*Begruendung* traegt trotzdem, aber ueber
"EM26 = 1 Bone, keine Bank 1", nicht ueber das A/B.

**OFFEN:** Direktbeweis fehlt (kein Kill-Switch/Magenta-A/B) — der Kernsatz "die Dreiecke SIND
EM26" ist Hypothese. Zweiter Setzer FUN_8001ee68 (@0x8001ef04/@0x8001ef14, Stride 172) wird aus
JEDEM Stage-Overlay gerufen (mein Scan: STAGE1 @0x80100fe0, STAGE2, STAGE3 x2, STAGE4, STAGE5 x2);
in STAGE1 sitzt er im Tail von FUN_80100688, das **keinen jal-Aufrufer** und genau eine
Datenreferenz (@0x8011f7b4) hat — Index-Domaene nicht aufloesbar. Solange das offen ist, ist der
geloeschte Zustand nicht als Dauerzustand bewiesen. Typ 0x24/ROOM20B0 ungemessen; kein
PSX-Gegenbeweis; Slot 4/5 (3,1% opak-schwarz) ungeprueft; platform/psx nicht analysiert.

**NAECHSTER SCHRITT:** Generische Part-Zeichenmaske (Seed 1 @0x8001e74c/58; Schranke im
Mesh-Loop @0x8001ecc4/c8; Clear im INIT @0x801165d0-e4 bzw. @0x8010efc8-e4; Schatten
@0x80116740 und Kollision @0x80116368 bleiben AN) ist **implementierungsreif** — mit korrigierter
Umfangs-Aussage. Vorher/parallel: Magenta-Tint-A/B + gdigrab (`re15-port-visual-verify`) fuer den
Direktbeweis, und PCSX-Redux-Watchpoint auf das Flag-Wort von part[0] (`re15-pcsx-watchpoint`),
der Lebensdauer, FUN_8001ee68-Reichweite und Bit-0x20-Leser in einem Zug klaert.
Kommentar-Widerrufe: enemy_ai_common.c:4747-4762, main.c:6874ff., enemy_ai_common.c:11699.

**Gestrichene Konstanten: keine.** Gestrichen wird eine *Behauptung* ("Umfang exakt zwei Typen").

---

## 2. ada-kiste

**URSACHE (belegt):** Das Original schiebt NPCs aus Props — von der **Objekt-Seite**.
FUN_8002bd44 iteriert nach dem Typ-Handler-`jalr` @0x8002be04 das enemy_array
(count 0x800aca4e, Basis 0x800acc2c, Stride 500) und ruft `jal 0x8002cabc` @0x8002be40 mit
`a2 = 0` — selbst disassembliert, Filter ist nur `flags & 1`, kein Typ-Filter. Die NPC-Wurzel ist
prop-frei (einzige Klemme: SCA-Wand `jal 0x8003b0a4` @0x8011cc64). Portseitig verifiziert:
`re15_collision_objects` hat vier Aufrufer, alle auf `pl`; kein einziger Aktor-Pfad.
**"Ada laeuft durch die Kiste" ist eine echte Port-Luecke, nicht byte-true** — ein Pruefer hat es
zwangfrei nachgemessen (62 Bilder im Kasten, Spieler sauber bei -9486 = -8136-900-450 gestoppt).

**BESTRITTEN:** (a) FUN_8002bd44 hat **fuenf** Aufrufer (mein Vollscan: 0x8001ce14, 0x800465d4,
0x8004c7d8, 0x8004c808, 0x8004cd4c), nicht einen. (b) Die Einbaustelle ist falsch: der Port
portiert FUN_8002bd44 bereits als `re15_prop_push_tick` (aot_common.c:1453, gerufen
game_step_common.c:1297) — er beginnt aber erst beim Typ-4-Zweig @0x8002bf14 und ueberspringt
Typ-Handler und Entity-Schleife. Der Aktor-Pass gehoert **an den Anfang des Prop-Rumpfes dort**,
nicht in eine neue Schleife. (c) **Fix-Punkt 3 enthaelt eine erfundene Konstante und wird
gestrichen**: `cy = -hit_height` — der Port fuehrt `hit_offset_y` bereits (re15_actor.h:106,
re15_damage.c:2203-2208) und die Gleichsetzung ist durch die Kraehe widerlegt (cy=0 bei h=180,
Ausnahme re15_damage.c:2210-2212). (d) Zwei Zustands-Stores im Modus 0 uebersehen:
`sb v0,10(a3)` @0x8002cc10 (entity+0xA) und `sb v0,10(t0)` @0x8002cc38 (obj+0xA) — ohne sie ist
es eine Teil-Portierung. (e) a2 hat **drei** Modi (`beq t9,2 -> return` @0x8002cb7c).
(f) "die drei uebrigen FUN_8002cabc-Stellen" sind fuenf. (g) Die Erreichbarkeits-OPEN-Notiz ist
widerlegt (State 1 wird bei F661 durch Plc_ret selbst erreicht); der prop[1]-Teil von Messung (3)
ist **vakuant** (Band 5, unerreichbar, misst nur Waende).

**OFFEN:** Objekt-Flag Bit 0x2 (@0x8002caec) — Semantik/Setzer unbekannt; betrifft auch den
ausgelieferten Spielerpfad (prop[2], flags 0x0009, Box 0/0/0). Y-Gate @0x8002cbac und
Hoehen-Zweig @0x8002cc3c-cd8 fehlen in der Spieler-Fassung. **Wurzel-Zuordnung Typ 0x42 ->
0x8011cb70 ist unbelegt** (weder `jal` noch Datenwort in STAGE1.BIN). Traegt der 0x42 in seinen
States das Entity-Flag 0x40 (@0x8002cad8)? — dann waere das Durchlaufen byte-true. Vier weitere
FUN_8002bd44-Aufrufer unidentifiziert. Typ-Handler-`jalr` @0x8002be04 nicht disassembliert.
Gegner-blockieren-Kiste @0x8002bff4-c038 und Obj-gegen-Obj @0x8002be90/@0x8002c078 fehlen im Port.
Keine dynamische Original-Messung. Konkurrierende Ursache 32f2e9e4 nicht ausgeschlossen.

**NAECHSTER SCHRITT:** Aktor-parametrisierter FUN_8002cabc-Modus-0 ist **implementierungsreif**
mit den Summen @0x8002cb3c-4c / @0x8002cb9c-a8, dem Y-Gate @0x8002cbac-bc (mit `hit_offset_y`,
NICHT `-hit_height`), den Kontakt-Indizes @0x8002cc10/@0x8002cc38 und der Aufloesung
@0x8002cce8-d10 / @0x8002cd50 / @0x8002cdb8 — **eingebaut am Anfang des Prop-Rumpfes in
`re15_prop_push_tick`**. Das Bit-0x2-Gate erst nach RE des Setzers. Vorher zu klaeren:
Wurzel-Zuordnung 0x42, Entity-Flag 0x40 im 0x42-State-Pfad (kippt den ganzen Befund),
Bit-0x2-Semantik (PCSX-Watchpoint auf prop[2]-Flags). Regressionsflaeche sind die
**Gegner-Positions-Pins in 1090/1140/1210**, nicht die drei genannten Spieler-Tests.

**Gestrichene Konstante: `cy = -hit_height`.**

---

## 3. 1210-re2-verhalten

**URSACHE (belegt):** `RE15_WRITHER_REACH_Z = 850` und `RE15_WRITHER_REACH_X = 11000`
(enemy_ai_common.c:11135) tragen als einzige Konstanten ihres Enums **kein `@0x…`** — selbst
nachgelesen. REACH_X 11000 umfasst die ganze Flurbreite, deshalb reagiert immer auch die
Gegenreihe. RE2s verankerter Greifer misst statt dessen Radius (`sltiu s0,s0,0x514` @0x80102f3c,
+0x1F0 = euklidischer Ursprungs-Abstand @0x800265a4-e0) plus Sektorpaar (`jal 0x80015758`,
Halbwinkel 256, @0x80102f8c-98 / @0x80102fd4-e4) und greift im selben Bild
(`sw 257,4(s1)` @0x80102fa8). Die beiden `jal 0x80015614` (@0x80102f10/@0x80102f2c) sind
**tot** — ich habe FUN_80015614 gelesen: ausser Stack kein einziger Store.

**BESTRITTEN:** (a) "byte-gleich zu 0x80102ee4" ist **falsch** — RE2 misst Ursprung gegen
Ursprung, der Vorschlag eine um 4091 nach vorn verschobene Scheibe. (b) **Fix 1 ist mechanisch
falsch**: `re15_writher_step` bewegt `e->x` (enemy_ai_common.c:10958-10970), die 2420 stecken
nach der Lunge bereits im Ursprung; der Vorschlag addiert sie ein zweites Mal — und `reach` ist
zugleich das **Rueckzugs-Tor** (Zeile 11306). Gemessen kehrt sich das Tor um (naher Arm 2031 =
aus, Gegenreihe 497..535 = an). Das bestehende, funktionierende Griff-Tor addiert korrekt nur
`MESH_REACH 1671`. (c) **Fix 2** trifft Bild 0 statt Bild 4 (`case 0` setzt `sub_state_2 = 1` und
faellt durch, Zeilen 11176-11180; die 4-Bild-Marke waere `>= 2` @0x8010c7e8). (d) **Fix 3
gestrichen**: wirkungslos (alle 30 Sweep-Zeilen bitgleich), falsch begruendet (1300 = verankerter
Zombie @0x80102f3c, 1200 = freier @0x801018f4 — zwei Gegner), und kippt einen ⛔-Marker im
Produktivcode. (e) Die Tor-Beschreibung laesst zwei RE2-Bedingungen weg, die ich woertlich
disassembliert habe: Ein-Angreifer-Riegel `lbu [0x800cfdcb] & 0x80` @0x80102f4c-58 (**vor** dem
Winkel, und **im Tor selbst gesetzt** @0x80102fb8-c0) und Etagen-Gleichheit @0x80102f60-70.
(f) Die Sektor-Form traegt die Uebertragung nicht: die Vereinigung ist +-512 = **+-45 Grad**, also
identisch zum echten Kegel der Schwester-Variante (a3=512 @0x80103ab8, v0 dort geprueft); die
Zweiteilung waehlt nur den Arm. Auf eine einarmige Kreatur uebertragen verdoppelt die
ODER-Verknuepfung das Fenster — unbelegt, betrifft auch das bestehende Griff-Tor.
(g) "aus 0 Griffen werden 4 bzw. 6" ist ein **Modell-Artefakt**: die Sonde laeuft mit
`re15_enemy_ai_set_paused(1)`, ohne Riegel, ohne Grabbed-Schranke, mit `adv = 800*ext_f` — waehrend
der Port die Phase-2-**Rueckwaertsbewegung** byte-true fuehrt (`re15_writher_step(e, 0x800)`,
@0x8010c8b4) und die Hand in den Bildern 5..35 bis zu 800 kuerzer ist. Belegbar ist "aus 0 wird
mindestens 1". Die Rueckzugsregel der Sonde enthaelt zudem eine **erfundene 400er-Hysterese**.
(h) Kleinere Fehlaussagen: "alle 10 Arme in JEDER Spur" (West = 9), "2600/3200 kippt beide
Wand-Spuren" (nur West), "Ruhe dreht nur alle (rng&7)+7 Bilder" (ab Anim-Bild >= 21 in JEDEM
Bild, `slti v0,v0,21` @0x801030c8), "+0x1F0 wird pro Bild gerechnet" (uebersprungen bei
+0x10E & 0x8000, @0x80026590-9c).

**OFFEN:** MESH_REACH 1671 in dieser Sitzung von niemandem neu gemessen — traegt schon heute das
ausgelieferte Griff-Tor. "Fenster-/Gitter-Zombie" unbelegt (nur: ungerades `+0x10E & 0x3F` ->
Hirn 0x80101210); damit auch unentschieden, ob Zustand [0] oder [2] das Vorbild ist. Setzer von
+0x21A Bit 0x20/0x40 unbekannt. Welche Lesart von "zu weit weg" (lateral/zeitlich) der Nutzer
meint, unbelegt. Keine visuelle Verifikation. Etagen-Bedingung im Port nicht abgebildet.

**NAECHSTER SCHRITT: kein Fix ist implementierungsreif — alle drei werden gestrichen.**
⛔ Rahmen: REACH_Z/REACH_X ersatzlos zu streichen ergibt "alle zehn Arme" — den vom Nutzer
**verworfenen** Zustand; die Sweep-Zeilen T_out 2600/3200 (9-10 Arme) liegen in derselben
verbotenen Zone. Reihenfolge: (1) gdigrab-Aufnahme + Nutzer-Rueckfrage, welche der zwei
gemessenen Fehlbilder gemeint ist; (2) MESH_REACH 1671 neu messen (`probe_1210_reach` §5);
(3) Lauf mit **lebender** KI inkl. Phase-2-Rueckzug und echter `!reach`-Bedingung — vorher hat
keine Wirkungszahl Wert; (4) Anker-Frage: ein Reichweiten-Tor auf die Hand muss an
`s_writher_home_x/z` haengen, nicht an `e->x`, und ist als **Nachruestung** zu benennen (RE2 hat
gar kein Ausfahr-Tor); (5) Sektor-Form entscheiden (ein Sektor pro Arm, oder +0x21A-Setzer RE'en);
(6) `test_1210_gitterhaende` um einen Constrain-LAUF erweitern — heute parkt sie den Spieler
400 Bilder an der Wand und haelt den Kernfehler nicht fest.

**Gestrichen: Fix 1, Fix 2, Fix 3** (Fix 3 enthaelt keine erfundene Zahl, sondern verschiebt eine
belegte Zahl an den falschen Gegner — nach Regel 3 dieselbe Konsequenz). Zusaetzlich gestrichen:
der rechnerische Vorlauf `T_out = T_grab + 4*Tempo` (1600/2100) — vom Befund selbst korrekt
ausgeschlossen.

---

## Querschnitt

- **Drei Befunde, drei richtige Ursachen, drei falsch dimensionierte Fixes.** In allen drei Faellen
  war die RE-Arbeit am Original solide und die *Uebertragung in den Port* der schwache Teil —
  weil der Port-Bestand nicht gelesen wurde (`re15_prop_push_tick` existiert; `hit_offset_y`
  existiert; das Griff-Tor misst die Hand bereits richtig).
- **Zwei Muster wiederholen sich aus der Memory:** Modell-Sonden mit abgeschalteter KI
  (`re15_enemy_ai_set_paused(1)`) liefern Zahlen, die im lebenden Ablauf nicht gelten
  (vgl. `reai-v2-sonde-luegt-paket-luegt`); und Flag-Gates werden vorgeschlagen, ohne die
  **Lebensdauer** des Flags zu messen (vgl. `reai-v2-klebrige-bits`) — bei den schwarzen
  Dreiecken genau der Punkt, an dem der dritte Schreiber FUN_8001f024 uebersehen wurde.
- **Was heute in den Code darf:** die Part-Zeichenmaske (Finding 1) und der Aktor-Prop-Pass
  (Finding 2, ohne Punkt 3, an der richtigen Stelle). **Finding 3 gehoert zurueck ins RE.**
