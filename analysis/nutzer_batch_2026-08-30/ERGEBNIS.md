# Nutzer-Batch 2026-08-30 — Ergebnis (v0.3.30, Commits ea4ed344 + 3003a0b9)

## 1. Schwarze Dreiecke ueber dem Feuer — BEHOBEN

Ursache war NICHT die Bank-Wahl (v0.3.29 hat an der falschen Stelle repariert), sondern die
PART-ZEICHENMASKE, Bit 0 der Part-Flags (entity+0x188):

    Binder setzt:   8001e74c ori v0,zero,0x1 / 8001e758 sw v0,0(s2)
    Zeichner:       8001ecc4 andi v0,v1,0x1 / 8001ecc8 beq v0,zero,0x8001ee48
    Typ-0x26-INIT:  801165d0 lw a0,392(v0) / 801165d8 lw v0,0(a0)
                    801165dc addiu v1,zero,-2 / 801165e0 and v0,v0,v1 / 801165e4 sw v0,0(a0)

Die sieben ROOM1090-Emitter sind unsichtbare Traeger. Ihr Mesh ist 1 Bone / 3 Verts / 1 Face.
Fix: `re15_actor_t.no_draw`, im 0x26-INIT gesetzt, im Renderer vor der Mesh-Ausgabe geprueft.
PIN `unit_1090_nodraw_pin` (7/7 Emitter maskiert + Gegenprobe: 0x10-Zombie NICHT maskiert).

## 2. Ada laeuft durch die Kiste — BEHOBEN

Anderes System als die Wand (v0.3.29 hatte nur die Wand). FUN_8002bd44 schiebt JEDEN aktiven
Aktor aus den Objekten:

    8002be14 lbu s1,0(v0)  / 8002be20 addiu s0,v0,478 / 8002be2c andi v0,v0,0x1
    8002be40 jal 0x8002cabc / 8002be44 addu a2,zero,zero / 8002be4c addiu s0,s0,500

Kein Typ-Filter. Ausnahme nur ueber Wort-0-Bit 0x40 (8002cad8 andi v0,v0,0x40); die
NPC-Familie traegt es NICHT (INIT ODERt 0x40000000, 8011ccd4 lui v1,0x4000).
Radius: Original summiert Objekt-Box + Aktor-Box[+6] (8002cb3c lhu v0,6(t2)); NPC-Box
@0x80121658 = {0,-1530,0,450,1530,450}, hx = 450 = PR des Ports -> zahlengleich.
Fix: `re15_collision_objects_actor` + `re15_actor_prop_pushout`, aufgerufen HINTER der
Entitaeten-Schleife (Original-Reihenfolge: 8001ce04 Entitaeten -> 8001ce14 Objekt-Tick).
PIN `unit_1090_npc_prop_push` (Versatz 1350 gegen Halbmasse 900 + Gegenprobe fern + Box-Kontrolle).

⛔ BEGRENZT auf 0x40..0x4d. Fuer die uebrigen Gegner-Typen ist die +0x78-Box eine andere
(Hund 500, Kraehe 200, …) und `re15_collision_objects` kennt nur PR. OFFEN.

## 3. Arme kommen zu frueh raus — TOR AUSGETAUSCHT

### Der Original-Mechanismus, vollstaendig gelesen
- `ROOM1210.RDT` @0x1EAE: Aot_set aot=6 sce=3, x=-22100 z=-15000 w=5200 d=1700.
- sub02 @0x1EC8: Flag(3,0x2c):=1, Aot_reset Slot 6, dann 10x `2e 02 0N 00` + `34 0c 01 00`
  = Work_set(Gegner N) + Member_set(12,1). Member 12 -> `sb a2,9(a0)` @0x800411f8.
- Arm-Zustand 0 liest genau das:
    8010c614 lbu v0,9(a0) / 8010c61c andi v0,v0,0x1f / 8010c620 bne v0,v1(=1),0x8010c644
    8010c628 sb v0(=1),5(a0) / 8010c638 sb zero,6(v0)
- LOGIK-Tabelle @0x80120968[1] = 0x8010c70c = blankes `jr ra`. Die Arbeit macht die
  ANIM-Tabelle @0x80120984[1] = FUN_8010c714, und die laeuft OHNE Bedingung.
- Der Wurzel-Tick rechnet den Spielerabstand (`jal 0x80065f60` @0x8010c27c) und verwirft ihn.

⇒ **Das Original hat pro Arm KEIN Abstands-Tor.** Jedes Tor hier ist eine Nachruestung.

### RE2 hat auch keins
Verankerter Greifer EMZ0.BIN, LOGIK-Tabelle @0x8010c90c = {0x80102ee4, 0x801025e4, 0x80103a70},
ANIM-Tabelle @0x8010c918. Zustand 0 = Ruhe-Winden (Anim 0x80103024) + GRIFF-Tor:
    80102f0c lw s0,496(s1)   ; entity+0x1F0, Ursprung gegen Ursprung
    80102f3c sltiu s0,s0,0x514 ; < 1300
    80102f40 beq s0,zero,0x80103008
    80102f4c lbu v0,-565(v0) / 80102f54 andi v0,v0,0x80   ; Ein-Angreifer-Riegel
    80102f60 lbu v1,262(s1)                                ; Etagen-Gleichheit
    80102f98 jal 0x80015758                                ; Sektor, Halbwinkel 256
    80102fa8 sw v0(=257),4(s1)
Der freie Zombie: `sltiu v0,s2,0x4b0` = 1200 @0x801018f4.

### Was jetzt im Port steht
- RAUS: `REACH_Z 850` / `REACH_X 11000` — die einzigen Konstanten dieses Enums ohne `@0x…`.
  11000 war die Flurbreite und liess jeden Arm reagieren, egal wie weit der Spieler von SEINER
  Wand entfernt stand. Das ist der gemeldete Fehler.
- REIN: `RE15_WRITHER_REACH_R = 0x514` @0x80102f3c — ein RADIUS statt eines Rechtecks.
- BEZUGSPUNKT (benannte Nachruestung): die ausgefahrene HAND ab der HEIMAT-Position,
  Versatz LUNGE_NET 2420 + MESH_REACH 1671 = 4091. Begruendung: die Arme sitzen mit dem
  Ursprung in der Wand (Reihen x=-25000/-14000), der begehbare Flur liegt rund 4200 daneben —
  ein 1300er-Radius um den Ursprung ginge NIE auf. An `e->x` gehaengt kippte der Ausdruck
  mitten in der Lunge (er ist zugleich das Rueckzugs-Tor), deshalb die Heimat-Position.

### Gemessen (unit_1210_gitterhaende, Lauf an Ost- und Westwand)
    kleinster Abstand Spieler->Hand: 72 / 77 / 79 / 81 (Ost), 286 / 288 (West)
    10 von 10 Armen reagieren, 10 kommen zur Ruhe
    hoechstens 2 gleichzeitig ausgefahren (v0.3.29: 3)
    Gegenreihe: 0 Reaktionen   <-- unter dem alten Tor unmoeglich
    Ausschlag: 2420 (byte-true)

⛔ GEMESSENE GRENZE, ehrlich benannt: auf der Mittellinie zwischen den Armreihen (x = -19500)
ist der Abstand zu BEIDEN Handpunkten je 1409 — dort geht kein Tor auf. Es gibt also einen
schmalen Streifen in der Flurmitte, in dem kein Arm reagiert. Das ist RE2-konform (ein
verankerter Greifer, der nicht heranreicht, greift auch nicht), aber es ist eine Aenderung
gegenueber v0.3.29, wo JEDER Arm im ganzen Flur reagierte.

## 4. Leon verschwindet beim Treffer — BEHOBEN (schon in ea4ed344 enthalten, Commit 9661e369)

Gemessen: Sprung von (-18164,-5897) auf (-892,0) = 16508 Einheiten. Ursache: fehlender
gemeinsamer Anker. Die Opfer-Animation wird ABSOLUT vom Anker +0xa0/+0xa2 platziert, den der
Zombie-Griff @0x801025f0 setzt und @0x8001ad28-48 auf den Spieler kopiert; beim Arm wurde er
nie gesetzt. Fix: Anker beim Griff-Eintritt kopieren. Gemessen jetzt 3710 statt 16508.

## OFFEN
- MESH_REACH 1671 ist seit seiner Ersterhebung nicht neu gemessen worden. Es traegt die
  Haelfte des 4091er-Versatzes UND das bestehende Griff-Tor.
- Der Aktor-Prop-Push laeuft nur fuer 0x40..0x4d (s.o.).
- Keine visuelle Verifikation moeglich: in dieser Sitzung liefern sowohl RE15_AUTOSHOT als
  auch ffmpeg-gdigrab am echten Fenster schwarze Bilder (kein echter Desktop).
