# ada-kiste — Schiedsspruch

Alle strittigen Punkte selbst nachdisassembliert (eigener jal-Vollscan ueber PSX.EXE +
alle PSX/BIN/*.BIN), Port-Seite selbst nachgelesen.

## URSACHE (belegt)

Das Original schiebt NPCs aus Props heraus — **von der Objekt-Seite**, nicht von der NPC-Seite.
Die Objekt-Schleife FUN_8002bd44 iteriert nach dem Typ-Handler das enemy_array und ruft pro
aktivem Entity FUN_8002cabc im Modus a2=0 ("schiebe das ENTITY"). Selbst verifiziert, woertlich:

```
8002be04: jalr  v0                 ; Typ-Handler des Objekts (Tabelle @0x80073c70)
8002be08: addu  a0,s2,zero
8002be0c: lui   v0,0x800b
8002be10: addiu v0,v0,-13746       ; 0x800aca4e  g_active_count
8002be14: lbu   s1,0(v0)
8002be1c: beq   s1,zero,0x8002be50
8002be20: addiu s0,v0,478          ; 0x800acc2c  enemy_array
8002be24: lw    v0,0(s0)
8002be2c: andi  v0,v0,0x1          ; nur "aktiv", KEIN Typ-Filter
8002be30: beq   v0,zero,0x8002be48
8002be34: addu  a0,s0,zero
8002be3c: addu  a1,s2,zero
8002be40: jal   0x8002cabc
8002be44: addu  a2,zero,zero       ; a2 = 0 -> das ENTITY wird bewegt
8002be48: bne   s1,zero,0x8002be24
8002be4c: addiu s0,s0,500          ; Stride 0x1F4
```

Die NPC-Wurzel selbst ist prop-frei — ihre einzige Klemme ist die SCA-Wand-Klemme
`jal 0x8003b0a4` @0x8011cc64 (Radius aus +0x78[6], Maske 4). Der Tail geht nach dem im Befund
zitierten Ende noch weiter: `lh a1,442(v0)` @0x8011cc8c / `jal 0x8001b064` @0x8011cc90
(Boden-Schatten) — prop-frei bleibt es, "komplett zitiert" war es nicht.

**Die Port-Luecke ist real und portseitig verifiziert:** `re15_collision_objects` wird an genau
vier Stellen gerufen (game_step_common.c:452, :1144, :1298, :1741), alle auf `pl`.
`re15_collision_push_prop` / `prop_contain` sind ebenfalls reine Spielerpfade
(aot_common.c:1480, climb_common.c:434/493/506). Es gibt **keinen einzigen** Nicht-Spieler-
Prop-Aufruf.

Der Deskriptor des Typ-0x42-NPC @0x80121658 = `00 00 06 fa 00 00 c2 01 fa 05 c2 01`
= s16[6] {0,-1530,0,450,1530,450} ist bestaetigt; Port-Gegenprobe hit_radius_min = 450.

**"Ada laeuft durch die Kiste" ist damit KEIN byte-true Verhalten, sondern eine echte
Port-Luecke.** Beide Pruefer haben das unabhaengig bestaetigt, einer davon mit einem
zwangfreien Lauf (Tuerspawn, SCD an, Pad-Eingabe): 62 Bilder lang liegt die 0x42-NPC
INNERHALB prop[0], waehrend der Spieler am Rand -9486 = -8136-900-450 sauber gestoppt wird.

## BESTRITTEN

1. **"die Objekt-Schleife FUN_8002bd44 (@0x8001ce14 im Main-Tick)" — es sind FUENF Aufrufer.**
   Eigener jal-Vollscan der PSX.EXE:
   `jal 0x8002bd44 -> 0x8001ce14, 0x800465d4, 0x8004c7d8, 0x8004c808, 0x8004cd4c`.
   Der Fix-Vorschlag verdrahtet nur einen davon.

2. **Die Einbaustelle ist falsch — der Port portiert FUN_8002bd44 BEREITS.**
   `re15_prop_push_tick` (aot_common.c:1453, gerufen game_step_common.c:1297) ist die
   Rueckwaerts-Prop-Schleife. Sie beginnt aber erst beim Typ-4-Schiebe-Zweig @0x8002bf14 und
   ueberspringt **beides**: den Typ-Handler-`jalr` @0x8002be04 und die Entity-Schleife
   @0x8002be0c. Der Aktor-Pass gehoert an den **Anfang des Prop-Rumpfes** in dieser Funktion,
   nicht in eine neue Schleife vor `re15_aot_stamp_entities`. Der Vorschlag haette die
   Original-Reihenfolge (Entity-Push VOR Objekt-gegen-Objekt, VOR dem 0x200-Zweig, VOR dem
   Spieler-Push @0x8002c0e4) verletzt und die Prop-Schleife dupliziert.

3. **ERFUNDENE KONSTANTE — Fix-Punkt 3 wird GESTRICHEN.** Der Vorschlag sagt, der Port habe kein
   Gegenstueck zum Deskriptor-Mittenoffset und schlaegt `cy = -hit_height` vor. Beides falsch:
   - `int16_t hit_offset_x, hit_offset_y, hit_offset_z` existiert (re15_actor.h:106), wird
     typweise gesetzt (re15_damage.c:2203-2208) und bereits byte-true als +0x7c-Y-Zentrum
     benutzt (enemy_ai_common.c:3893).
   - `cy == -hit_height` ist widerlegt: die Kraehe (0x21) hat cy=0 bei h=180, der Port fuehrt
     dafuer eine explizite Ausnahme (re15_damage.c:2210-2212).
   => `cy = -hit_height` ist eine erfundene Zahl fuer genau dieses Problem und darf nicht in den
   Code. Der belegte Wert liegt bereits vor: `hit_offset_y`.

4. **Zwei Zustands-Stores im Modus 0 wurden uebersehen — der Fix ist so nicht byte-true.**
   Zwischen Y-Gate und Achsenwahl schreibt FUN_8002cabc zwei Kontakt-Indizes:
   ```
   8002cc0c: bne   a3,v1,0x8002cc1c     ; v1 = 0x800aca54 (Spieler)
   8002cc10: sb    v0,10(a3)            ; ENTITY+0xA = Objekt-Index
   8002cc18: ori   v0,zero,0x80         ; Spieler-Sonderwert
   8002cc38: sb    v0,10(t0)            ; OBJEKT+0xA = Entity-Index
   ```
   Der Pfad dorthin ist im Modus 0 unbedingt. Weder der bestehende Spieler-Port noch der
   vorgeschlagene Aktor-Pass haben diese Indizes.

5. **a2 hat DREI Modi, nicht zwei.**
   ```
   8002cb78: ori v0,zero,0x2
   8002cb7c: beq t9,v0,0x8002cde4       ; a2 == 2 -> Rueckkehr ohne Aufloesung (Testmodus)
   ```

6. **"die drei uebrigen FUN_8002cabc-Stellen" — es sind fuenf uebrige.** Eigener Scan:
   `0x8002be40, 0x8002bed4, 0x8002bfa4, 0x8002c020, 0x8002c0e4, 0x8003852c`. (0x8002bfa4 und
   0x8003852c bleiben in der Abgrenzung unerwaehnt.)

7. **Die Erreichbarkeits-Aussage der Sonde ist widerlegt** (Pruefer, von mir nicht nachgefahren,
   aber methodisch schluessig und mit Zeilenverweisen belegt): Eskorte-State 1 wird im echten
   Bild-Ablauf bei F661 durch das Plc_ret des Skripts selbst erreicht (sub03 @Datei 0x26C4);
   240 Bilder konnten nicht reichen, weil allein die Sleeps in sub03 >= 577 Bilder summieren
   und `Message_on 0x09` @0x2502 ohne Confirm-Druck blockiert. Die massive Erzwingung in
   Messung (4) war unnoetig. Zusaetzlich ist der prop[1]-Teil von Messung (3) **vakuant**:
   prop[1] liegt auf Band 5 und ist vom Tuerspawn unerreichbar; der dortige Spielerlauf misst
   nur Waende (MIT objects == OHNE objects == x=-9668).

8. **Die Aufruferliste FUN_8002cabc/FUN_8002cfd4 ist bestaetigt** (mein eigener Scan, identisch;
   `jal 0x8002cfd4 -> 0x8002bf04, 0x800382dc, 0x80038604, 0x800390b0`; kein Overlay-Treffer).
   Der Satz "kein NPC-Overlay ruft sie" haelt.

## OFFEN

- **Objekt-Flag Bit 0x2** (@0x8002caec) ist als Ausschalter belegt, seine Semantik und sein
  Setzer nicht. Der Port benutzt statt dessen "alle drei Halbmassen == 0". Konsequenz fuer
  ROOM1090 prop[2] (flags 0x0009, Box 0/0/0) ist ungeklaert — und betrifft **auch den bereits
  ausgelieferten Spielerpfad**. Ungeprueft blieb, ob fuer solche Objekte obj+0x78 ueberhaupt
  auf einen gueltigen Deskriptor zeigt; genau das koennte die Port-Regel rechtfertigen.
- **Y-Gate @0x8002cbac und Hoehen-Zweig @0x8002cc3c-cd8 fehlen in der bestehenden
  Spieler-Fassung** `re15_collision_objects`. Ob das heute eine sichtbare Divergenz erzeugt,
  ist nicht gemessen.
- **Wurzel-Zuordnung Typ 0x42 -> 0x8011cb70 ist unbelegt.** In STAGE1.BIN gibt es weder ein
  `jal 0x8011cb70` noch ein Datenwort 0x8011cb70. Indizien: die per entity+4 dispatchte Tabelle
  @0x80121668 enthaelt 0x80050be8 (gemeinsame NPC-EXE-Lib), @0x80121664 zeigt auf den
  Deskriptor 0x80121658 mit Radius 450 = gemessener hit_radius_min.
- **Traegt der 0x42-NPC in seinen erreichbaren States das Entity-Flag 0x40?** (@0x8002cad8
  `andi v0,v0,0x40 / bne` = Ausschluss). Im Port als Leichen-Bit dokumentiert. Traegt er es,
  waere das Durchlaufen byte-true. Nicht am INIT/State-Pfad nachgelesen.
- Die vier weiteren FUN_8002bd44-Aufrufer (0x800465d4, 0x8004c7d8, 0x8004c808, 0x8004cd4c)
  sind nicht identifiziert.
- Der Typ-Handler-`jalr` @0x8002be04 (Tabelle @0x80073c70, Index obj+8) laeuft PRO OBJEKT VOR
  der Entity-Schleife und kann das Objekt bewegen — nicht disassembliert, im Port nicht
  abgebildet.
- Die Gegner-blockieren-die-Kiste-Schleife @0x8002bff4-c038 (a2=1) und die
  Objekt-gegen-Objekt-Schleifen @0x8002be90/@0x8002c078 (FUN_8002cdf4) fehlen im Port ebenfalls.
- **Keine dynamische ORIGINAL-Messung.** Dass die 0x42-NPC im echten PSX-Spiel tatsaechlich aus
  prop[0] herausgeschoben wird, ist rein statisch belegt.
- Welche Bewegung der Nutzer gesehen hat (Eskorte-State 1 vs. Plc_dest-Walk in State 4) und in
  welchem Raum / an welcher Kiste, ist nirgends belegt.
- **Konkurrierende Ursache nicht ausgeschlossen:** Commit 32f2e9e4 schloss denselben
  Nutzer-Report ("Ada kann durch alles durchlaufen") mit einer anderen Ursache (Typ 0x42 fiel in
  `default: return` von `re15_enemy_apply_hitbox`, hit_radius_min = 0, Wand-Klemme @0x8011cc60
  zahnlos). Der Befund misst 450 = post-Fix. Ob die Kisten-Beobachtung nach 32f2e9e4 entstand,
  ist unbelegt.

## NAECHSTER SCHRITT

**Implementierungsreif (nach Streichung von Punkt 3 und Korrektur der Einbaustelle):**

1. Gemeinsamen Rumpf aus `re15_collision_objects` ziehen und aktor-parametrisieren
   (FUN_8002cabc, Modus a2=0). Die Spieler-Fassung ruft ihn mit dem Spieler-Aktor.
2. Kasten-/Radius-Summen byte-true:
   - `t5 = obj.box_hx + e->hit_radius_min` (@0x8002cb3c-48)
   - `t4 = obj.box_hz + e->hit_radius_min` (@0x8002cb44-4c — derselbe `lhu v0,6(t2)`, der Aktor
     ist ein ZYLINDER)
   - `a1 = obj.box_hy + e->hit_height` (@0x8002cb9c-a8)
3. Gates byte-true statt Port-Abkuerzung:
   - `if (e->flags & 0x40) return;` (@0x8002cad8-dc)
   - `if (obj.flags & 0x2) continue;` (@0x8002caec-f0) — **erst nach Klaerung von Bit 0x2**
     (s. OFFEN), sonst aendert sich der ausgelieferte Spielerpfad unkontrolliert.
   - Y-Gate `|dy| < obj.box_hy + e->hit_height` (@0x8002cbac-bc), mit
     `dy = (obj.box_cy + obj.y) - (e->hit_offset_y + e->y)` (@0x8002cb84-a4).
     **`e->hit_offset_y` benutzen — NICHT `-hit_height`.**
4. Kontakt-Indizes mitschreiben: `entity+0xA = Objekt-Index` (@0x8002cc10),
   `obj+0xA = Entity-Index bzw. 0x80 fuer den Spieler` (@0x8002cc18/@0x8002cc38).
   Ohne sie ist es eine Teil-Portierung, keine byte-true.
5. Aufloesung wie vorhanden: Achsenwahl @0x8002cce8-d10, X @0x8002cd50-7c, Z @0x8002cdb8;
   Hoehen-Zweig @0x8002cc3c-cd8 (schreibt e->y).
6. **Einbaustelle: an den Anfang des Prop-Rumpfes in `re15_prop_push_tick`**
   (aot_common.c:1453), entsprechend @0x8002be0c — vor dem Typ-4-Schiebe-Zweig. NICHT als
   separate Schleife vor `re15_aot_stamp_entities`.
7. Nach dem Push laeuft im Original KEINE erneute Wand-Klemme (die lag im selben Bild vorher,
   @0x8011cc64). Also nichts "sicherheitshalber" nachschieben.

**Regressionsflaeche — vom Befund unterschaetzt:** Der Pass bewegt JEDEN aktiven Aktor in JEDEM
Raum aus JEDEM Prop (Zombie 400/1440, Hund 500/600, Kraehe 200/180, Writher 0x1a 300/1440).
Die exponierten Wachen sind die **Gegner-Positions-Pins in ROOM1090/1140/1210**, nicht die drei
genannten Spieler-Tests. Vgl. Memory `reai-v2-pin-fixture-verschiebung`: Fixture reparieren,
nicht die Schranke senken.

**Vor der Fertigmeldung noch zu RE'en:**

- Objekt-Flag Bit 0x2: Setzer suchen (Xref-Scan auf `ori ...,0x2` gegen obj+0, dann
  PCSX-Redux-Watchpoint auf das Flag-Wort von prop[2] in ROOM1090). Das entscheidet zugleich,
  ob die Port-Abkuerzung im Spielerpfad falsch ist.
- Wurzel-Zuordnung Typ 0x42 -> 0x8011cb70 belegen: Typ->Wurzel-Installer in STAGE1 suchen
  (Muster wie @0x8011e8f4/@0x8011e8fc fuer 0x26), oder per Savestate entity+0x0 vergleichen.
- Entity-Flag 0x40 fuer 0x42 im INIT/State-Pfad nachlesen — traegt es der NPC, faellt der
  ganze Befund.
- Ob der neue Push ueberhaupt Wirkung behaelt: im Port haengt das an der Reihenfolge relativ
  zum Nav-/Plc_dest-Executor. Im Original korrigiert der Pass nachtraeglich (@0x8001ce14 NACH
  der AI @0x8001ce04). Messen, nicht annehmen.
