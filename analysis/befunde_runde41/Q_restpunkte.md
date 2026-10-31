# Spur Q "restpunkte" - Dossier (Runde 41, Basis master 91cd9ec82)

Baum `.claude/worktrees/r41_restpunkte`, Zweig r41/restpunkte. Fortlaufend geschrieben, nach jedem Schritt committet.
Vorgaenger hinterliess keine Commits (`git log master..HEAD` leer beim Start, Baum sauber).

## Punkte
1. Masken: Restloch ROOM6010/6011 Cut 5 Teil 8 schliessen (Abnahme M 5 Mangel 1).
2. Teile-Bericht M_bilder/NB5/teile_nb5.txt auf den ausgelieferten Stand; 8 fehlende Stellen in die Freistellungs-Liste;
   4040 C12 HINTER-Beleg berichtigen (Mangel 2/3).
3. Riegel Elza-Kette 2031 -> 20B1 -> 2051; Ursache der stillen Enden von integration_r41_ventilkette_kette (UEBERGABE 3b).
4. Toter Alligator bleibt tot (RE2 ROOM40A0 @0x0054, sub02 @0x00AE); Leon und Elza; Raum verlassen + wieder betreten,
   Spielstand laden; Water Key bleibt erreichbar oder aufgehoben.
5. Spinnen-Filter in gatorszene_r41.c unter RE15_GATOR_ORIGINAL pruefen/abschalten; Riegel Leon und Elza.

## Stand
- [ ] Bau im Spurbaum
- [ ] Punkt 1
- [ ] Punkt 2
- [ ] Punkt 3
- [ ] Punkt 4
- [ ] Punkt 5

## Punkt 4 - Der tote Alligator bleibt tot

### Messung vorher (exe Basis 91cd9ec82, Lauf Q41/runs/v4L, natuerlicher R1-Tod, Tuer Slot 0, Leiter zurueck)
Rezept des Riegels `integration_r41_alligator2_schluessel` + `RE15_FIRE_AOT=0@720#2090`: `RE2 R1 ende (Leiche) pos=(-5828,-20967)`, Folge L F217,
`Kanister verbraucht (R1-Tod) -> Bit 116 = 0` F220, Tuer, 2080, Leiter. Im naechsten 2090-Abschnitt: `[enemy] ROOM2090: Alligator (0x23) in Wasserzone gesetzt, Slot 35`,
state.log letzte Zeile `t=23 st=1 ss1=0 ... @(-7200,-9200,r1089)] hp=25300`. Volle Energie, kein zweiter Kanister: nicht mehr toetbar. Reproduziert H3 der Abnahme P 1.

### RE-Belege (selbst gelesen)
- RE2 ROOM40A0.RDT SCD-init sub00 (Datei 0x2502, Offsets relativ zum SCD): @0x0050 `06 00 64 00` Ifel_ck, @0x0054 `21 04 33 00` Ck Bank 4 Bit 0x33 = 0, @0x0058 `21 05 1f 01`,
  @0x00A0 `44 00 00 23 00 00 01 16 00 00 4a 16 b8 0b 3c eb 00 04 00 00 00 00` Sce_em_set Typ 0x23 liegt im Block: Bit gesetzt = kein Alligator.
- RE2 SCD-main sub02 @0x00A4: `46 05 ...` Aot_reset, @0x00AE `22 04 33 01` Set Bit, dann Intro. RE2 setzt das Sperrbit also beim KAMPFBEGINN, nicht beim Tod.
- RE2 hat daneben ein Tot-Bit: EM23_OVL_0000.BIN R1 P3 `801021e0 addiu a0,s2,17240` (s2 = 0x800d051c -> a0 = 0x800d4874 = Bank 5), `80102654 lhu v0,270(s1)`, `80102658 addiu a1,zero,30`,
  `8010265c ori v0,v0,0x4000`, `80102660 jal 0x8007730c` (Bit setzen: `8007730c srl a2,a1,5` .. `80077328 or v1,v1,v0`), `80102664 sh v0,270(s1)` = (5,0x1E). Leser SCD @0x07D0 `21 05 1e 00`, @0x0D56/@0x0DAC `21 05 1e 01`.
- RE2 hat ausserdem ein Kill-Flag im Satz: Sce_em_set FUN_8005714c `800571c0 lbu a1,9(v0)`, `800571c4 addiu v0,zero,255`, `800571c8 beq a1,v0`, `800571d0 jal 0x80077360`, `800571d8 bne v0,zero,0x800576dc`: pc[9] = 0x00, also Zone 0x800d489c Bit 0. EM23 setzt dieses Bit nicht (0x8007730c nur @0x80101848 Bank 5 Bit 29 und @0x80102660 Bank 5 Bit 30): der Mechanismus des Alligators ist das SCD-Bit, nicht das Kill-Flag.
- Warum RE2 das Bit beim Kampfbeginn setzen darf: die Tueren sind im Kampf Ereignisse (sub12 @0x0828/@0x0832 Aot_reset 0/1 -> sub29/sub30, Text), die Schalttafel AOT 4 (@0x00FA -> sub26 @0x0CC8, Abfrage `21 0b 1f 00` @0x0CDE) gibt sie frei (sub27/sub28): wer flieht, dem fehlt in RE2 danach nur der Gegner.
- Im Port traegt der Alligator den Water Key (Nutzer-Auftrag; Tor 2090/20A0): ein Sperrbit beim Kampfbeginn nimmt nach einer Flucht ueber die Leiter den Schluessel weg, also Softlock. PORT-WAHL: die Form bleibt RE2 (Ck-Tor vor dem Setzen, Bit gespeichert), der Zeitpunkt ist RE2s Tot-Bit (R1 P3 @0x80102660).

### Umsetzung
- Kein neues Port-Bit: (9,111) `RE15_GSZ_BIT_LIEGT` ist genau "Alligator tot" (include/re15_gatorszene_r41.h:62; einziger Setzer gatorszene_r41.c key_tod_setzen, bleibt nach dem Aufheben gesetzt, Taken-Bit ist (9,112)). Bit 126 bleibt frei.
- engine/src/gatorszene_r41.c `re15_gatorszene_r41_gator_kommt`, include/re15_gatorszene_r41.h, Haken in platform/pc/main.c (Alligator-Pool, eine Bedingung).

### Messung nachher (Laeufe Q41/runs/n2L, n3L)
n2L (Leon): R1 F358-360, Folge O F450, Tuer F500, `[save] saved (room 2080)`, Leiter, 2090-Abschnitt 510 Bilder ohne eine einzige t=23-Zeile, genau eine Zeile `Alligator (0x23) in Wasserzone gesetzt`, Schluessel unveraendert an (-5700,-1725,-7500).
n3L: Karte aus n2L, `[save] CONTINUE: resumed in room 2080`, Leiter: 200 Bilder ohne t=23, `Schluessel-Ersatzlage (-170,-1702,-23629)`.

## Punkt 5 - Spinnen-Filter unter RE15_GATOR_ORIGINAL

### Messung vorher (Hauptbaum-exe, Kopie, Lauf Q41/runs/o0L)
`RE15_GATOR_ORIGINAL=1`: `[gatorszene] ROOM2090: Spinne 0x25 bei (-100,-8296) verworfen`, ebenso (-2300,-23896). Es laeuft nur der Alligator, der Raum ist nicht der ausgelieferte.

### Beleg
ROOM2090 main00 `@0x00AB0 44 00 25 ...` und `@0x00AC4 44 01 25 ...` (F_gatorszene.md Z. 77-78): zwei Spinnen sind Inhalt des ausgelieferten Raums. Der Filter ist Nutzer-Design ("Entferne die Spinnen aus dem ROOM 2090", AUFTRAG.md) und gehoert zu dem Paket, das der Schalter abstellt (Commit aa463a744: Ankunft, Montage, Water-Key-Tor bleiben aus).

### Umsetzung, Messung nachher (Lauf Q41/runs/o1L)
gatorszene_r41.c `re15_gatorszene_r41_spawn_weg`: `if (!s_spinnen_weg || re15_gator_original()) return 0;`. Nachher: keine Zeile `verworfen`, F500 `[1 t=25 ... @(-1027,-11432..)]`, `[2 t=25 ... @(-6995,-25679..)]`, `EM23 loaded: 15 meshes, 15 bones, 43 clips`, keine einzige `[gatorszene]`-Zeile.
