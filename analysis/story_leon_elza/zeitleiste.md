# Kreuz-Zeitleiste Leon <-> Elza als Graph (erzeugt von zeitleiste.py)

Knoten = Ereignisse (L = Leon Original, E = Elza Original, P = Port-Ergaenzung Leon, Q = Port-Ergaenzung Elza). Kanten = 'a VOR b'. Arten: ablauf (Reihenfolge im eigenen Szenario), hart (Original-Bytes beider RDTs), port (Port-Mechanik), nutzer (Nutzer-Vorgabe Runde 36), vorschlag (Empfehlung in C_zeitleiste.md).

## 0. Ergebnis je Modell

| Modell | Beschreibung | Ergebnis |
|---|---|---|
| c | PARALLEL / raumweise versetzt (nur die Kreuz-Kanten) | WIDERSPRUCHSFREI |
| a | Elza GANZ NACH Leon (Leons Ende L37 vor Elzas Anfang E01) | WIDERSPRUCH (12 Zyklen gezeigt) |
| a1 | Elza beginnt nach Leons STAGE1 (P09 vor E01) | WIDERSPRUCH (6 Zyklen gezeigt) |
| b | Elza GANZ VOR Leon (Elzas Ende E44 vor Leons Anfang L01) | WIDERSPRUCH (9 Zyklen gezeigt) |
| b1 | Elza verlaesst das Revier, bevor Leon es betritt (E22 vor L01) | WIDERSPRUCH (6 Zyklen gezeigt) |

## 1. Knoten

| Id | Wer | Raum | Ereignis | Beleg |
|---|---|---|---|---|
| L01 | L | 1240 | Vorspann; setzt (3,0x8B)=1 = B2-Tuer 'Prisons' fuer Leon von Anfang an offen | ROOM1240 sub02@0x0055A `22 03 8b 01`; Leser ROOM11E0 main00@0x01576/@0x016C4 |
| L02 | L | 1170 | Prolog-Einblendung (3,0x7D),(4,0xF2) | ROOM1170 main00@0x012CC -> sub11@0x0169A/@0x0169E |
| L03 | L | 1170 | Elliot + Pilot fliegen ohne Leon ab; Hubschrauber (obj 2-5) verschwindet | ROOM1170 sub15@0x017D2 Spawn 0x47, obj@0x017E6..@0x0184C; sub02@0x0142C (4,0xF2)=0; msg 0-7 |
| L04 | L | 1170 | 'That's right! I have to report this to the chief.' | ROOM1170 sub14 msg 13-16, @0x01738 (3,0x3E)=1 |
| L05 | L | 1150 | Irons LEBEND auf der Couch, Dialog (Funkraum-Hinweis); (3,0x94) | ROOM1150 main00@0x00E88 Spawn 0x45; sub08 msg 4-14, @0x01110 (3,0x94)=1 |
| L06 | L | 1130..1050 | Abstieg 1130->1120->1060->1040->1030->1050 (Fahrstuhl ohne Strom) | Tueren 1130 Slot1@0x008AE, 1120 Slot0@0x00C76, 1060 Slot2@0x00D52, 1040 Slot1@0x010B8, 1030 Slot0@0x01C6A |
| L07 | L | 1050 | Rolltor/'I need a fuse' (Port P2: Sicherung aus 1150); Polizisten-Leiche OHNE Untersuch-AOT (kein Waffen-Item) | ROOM1050 msg 2; Slot 11 (Leiche, sce 3) nur in ROOM1051 main00@0x00C0E; rolltor_1050.c |
| L08 | L | 1090 | Frau (Ada) ruft aus dem brennenden Schuppen; 7 Feuer | ROOM1090 sub01@0x02406 -> sub02 msg 0-2, (3,0x80); sub00@0x02214..@0x0228C Typ 0x26 |
| P03 | P | 1050->10A0 | [PORT] Ada-Ruf an der Treppenhaus-Tuer; Tuer gesperrt bis Ada gerettet (3,0xBB) | adaruf_1050.c; Runde 34 Nacht D_adaruf.md |
| L09 | L | 1000 | Feuerloescher (3,0x85) | ROOM1000 sub00@0x00C24 Item 0x31; sub01@0x00D0C |
| L10 | L | 1090 | Feuer geloescht, Ada gerettet; 4 Zombies (3,0xBB); Feuer aus (3,0x81); (3,0x6E) | ROOM1090 Slot2->sub06 @0x0271E/@0x02722; sub03 @0x024D2/@0x024D6; Spawns @0x023A0..@0x023DC |
| L11 | L | 1050 | Ada geht voraus: 'Hey, wait!' | ROOM1050 sub00@0x00C8E Spawn 0x42 (3,0x6E)==1; sub03@0x00D88 |
| L11E | L | 11E0 | Erster Besuch B2-Korridor: 'Prisons'-Tuer bereits offen; Weg zum Kesselraum fuehrt am Drahtgitter VORBEI (Gitter sperrt nur Leichenhalle + Abkuerzung) | ROOM11E0 main00@0x01576 Ck(3,0x8B)==0 -> @0x016C4 Slot 3 scharf; Gitter = SCA Zelle 0 x-14100..-6800/z-12600..-10600 (sub00@0x017B0ff `37 0r 00 ff`), Kessel-Tuer Slot 0 @0x01512 (-27100,-10300) ueber Zellen 3-5-4-2-7-10 erreichbar |
| L17 | L | 11F0 | Reserve-Strom an (4,0xF3) | ROOM11F0 sub18@0x016F6 `22 04 f3 01` |
| L18 | L | 11B0 | Erster Eintritt mit Strom: Maedchen (Sherry) rennt von (-14886,-24048) zum Schacht/Container und verschwindet; 'Leon: H-hey!' (3,0x82) | ROOM11B0 main00@0x00F56 (4,0x06)=1 bei (4,0xF3)==1; sub01@0x011AE/@0x011B6 -> sub04: @0x01324 Pos_set NPC2, @0x01338 msg 0, @0x013AC Pos_set -30000 |
| L12a | L | 11B0 | Pliers nehmen (9,0x0A) -> (3,0x7F); NUR mit Strom (sonst 'It's too dark...') | ROOM11B0 main00@0x00FF2 Item 0x30; sub01@0x011F6 Ck(4,0xF3)==0 -> @0x01220 Aot_reset Slot 4 -> msg 15 |
| L12b | L | 11E0/1200 | Draht durchtrennen (4,0x24); Leichenhalle: Minidisc Player (3,0x67) | ROOM11E0 sub21@0x01FC6; ROOM1200 sub00@0x00932 Item 0x44, sub01@0x0095A |
| L13 | L | 1100/1110 | Stimmschloss (4,0xE8); Blue Keycard + Zettel 4312 | ROOM1100 sub02@0x00C68; ROOM1110 @0x00B2E Item 0x38, msg 0 |
| L14 | L | 10D0 | Kartenleser Funkraum: Blue Keycard + 4312 (3,0x32) | ROOM10D0 sub01@0x0152A; Tuer Slot 0 -> 10F0 @0x01174 |
| L15 | L | 10D0 | Marvin am Rolltor-Schalter: 'the chief is injured, one of those things bit him' (4,0xF7) | ROOM10D0 sub00@0x01256 Spawn 0x40; sub21 msg 12-27, @0x019EC |
| L16 | L | 10F0 | Funkraum: 'A large communications device. It's out of commission.' | ROOM10F0 msg 0; main00@0x00F72 |
| P05 | P | 10F0 | [PORT] Szene Ada/Leon/Marvin: 'Marvin, you go with Ada to the parking lot... I'm going to get Chief Irons' (9,71) | cut_10f0.c; K_cut10f0.md |
| P06 | P | 10F0 | [PORT] Parking-Lot-Schluessel im kleinen Buero (9,82) | Spur Q, VERTRAG §1.1 |
| L20a | L | 1020 | (optional) Roy-Leiche obj 6 (-4488,0,-10684): 'It's the RPD officer, Roy. He appears to have died recently.' | ROOM1020 main00@0x01DEE obj 6 unbedingt; Slot 5 @0x01DDA -> sub02 msg 0 |
| L21a | L | 1210 | (optional) Detention Area: Zelle leer/ohne Text, 10 Gefangenen-Zombies von Anfang an | ROOM1210 main00@0x01D86..@0x01E3A (Typ 0x1A ohne Flag); kein Slot-8-Text (vgl. ROOM1211 sub00@0x01DEE) |
| P04 | P | 1060->1040 | [PORT] Tuer gesperrt 'I have to get the Chief first...' solange (9,73)=0 & ((9,71)=1 / (3,0x94)=0) | tuer1060_1040.c; L_cut1150.md §2.1 |
| P07 | P | 1150 | [PORT] Irons stirbt (9,73); danach 'Oh, I see... Damn it!' (ROOM11B0 msg 3/4) passt | irons_tod_1150.c; L_cut1150.md §2.2 |
| P08 | P | 1130/1040/1030/11C0/1120 | [PORT] Knall-Montage: Rest aus 1140 -> 1130; 1040 Rolltor hoch + 5 Zombies; Rest aus 1070 kriecht in 1030 unterm Tor durch; 11C0 Cut 13 Marvin rennt zurueck, Ada bleibt; 1120 Fenster zerbricht + Kraehe (9,79) | L_cut1150.md §2.3/§8; fenster_1120.c; Runde 36 K |
| L19 | L | 11C0 | Ada + 2 Gorillas; 'Ada: He turned around all of a sudden to go look for you' (3,0x43) [Port: Tuer 11B0->11C0 nur mit P06] | ROOM11C0 sub00@0x01770, sub02@0x0184E (4,0x40), sub03@0x01B02 |
| L20 | L | 11B0 | Marvin+Ada+Leon: Container obj 0 nach z=-19368 geschoben, Schacht frei (4,0xCD),(4,0xF4); 'Marvin: What about the chief?' 'Oh, I see... Damn it!' | ROOM11B0 sub01@0x011CE -> sub06 @0x01478..@0x01480; main00@0x010EA obj 0 (-2168,0,-19368) bei (4,0xCD)==1 |
| P09 | P | 11B0 | [PORT] Marvin schliesst Parkplatz-Tuer (Slot 1) und Werkstatt-Tuer ab (9,84): 'unwanted followers' | Spur J; Werkstatt-Tuer = Elzas Slot-8-Platz ROOM11B1 sub00@0x0112A (-22400,-17200) msg 12 |
| L21 | L | 1260/2000 | Kanal: 'There is a little girl that came in here... before meeting Ada I saw her coming in this way'; Marvin/Ada voraus | ROOM2000 sub00@0x017A0 (4,0xF4)==1 -> sub02 msg 0-11, @0x017FA |
| L22 | L | 2010..20B0 | Kanalisation (bytegleich mit Elza): Crank, SPAS-12, Head of Akuma, Alligator | zensus.md ROOM201..20B |
| L23 | L | 11A0 | Marvin wartet am Kanalausgang | ROOM11A0 sub00@0x0118C (4,0xC6)==0 |
| L24 | L | 3000 | RPD-Leichen; 'Marvin: Ada...? She went on ahead.' | ROOM3000 sub00@0x0156C/@0x0158E; sub03@0x0166E |
| L25 | L | 3010/3020/3060 | Marvin folgt | ROOM3010@0x02212, 3020@0x00BF6, 3060@0x00F58 (4,0xC7)==1 |
| L26 | L | 3070/3080 | Warehouse-Lift mit Birkin: Leon + Ada + Marvin | ROOM3070 sub00@0x0343C/@0x03450; ROOM3080 @0x007AC/@0x007C0/@0x007D4 |
| L27 | L | 30E0 | Ada: 'Where's that other guy?'; Marvin kommt GEBISSEN: 'The hall is no good...! Those things broke through the main gate' | ROOM30E0 sub11@0x01068/@0x0108C/@0x010A0; msg 0-13 |
| L28 | L | 4000 | Marvin wird getragen (4,0x26),(4,0x14) | ROOM4000 sub05@0x01550/@0x01554 |
| L29 | L | 4010 | 'Ada: That's right. I'm with Umbrella.' (3,0x3F),(3,0x4C) | ROOM4010 main00@0x00664; sub04@0x00DA4/@0x00DAC |
| L30 | L | 5060 | T-Vaccine (4,0xFA) | ROOM5060 sub06@0x030E2 |
| L31 | L | 4010 | Marvin kollabiert; 'Hurry to the shelter!' | ROOM4010 sub00@0x006CE -> sub04 msg 32-35 |
| L32 | L | 5090/6000 | Annette: 'You must be that cop. Your friends are already here.' (3,0x64) | ROOM6000 sub00@0x00F44 |
| L33 | L | 50B0 | Ada stirbt (2 Affen); Leiche (-11800,0,-11600); (3,0x62) | ROOM50B0 sub00@0x00512, sub02 msg 0-18, @0x005A6; Leiche @0x00566 |
| L34 | L | 50E0 | Birkin | ROOM50E0 sub00@0x00ACE/@0x00AEE |
| L35 | L | 6020 | Shelter: Marvin + Ada (3,0x4C) | ROOM6020 sub00@0x00B96/@0x00BAA |
| L36 | L | 6030 | Zug: John, Marvin (liegend), Sherry, Annette; 'Leon: Where's Elza?' 'Sherry: She's out there fighting the monsters!' | ROOM6030 sub06@0x0101C/@0x01030/@0x01044; msg 1/2 |
| L37 | L | 6040 | Ending | - |
| E01 | E | 1241 | Vorspann; setzt (3,0x6F),(3,0x70) (Schiessstand-Zielscheiben) | ROOM1241 sub02@0x0055A/@0x0055E |
| E02 | E | 1031 | Start in der Lobby; Prolog; 'Elza: Hey, is anyone here?!' | ROOM1031 main00@0x02052; sub15@0x02A68; sub13@0x02A24 msg 19 |
| E03 | E | 1051 | SIG P228 vom toten Polizisten: 'Sorry, I'll just borrow this...' (9,0xA5) | ROOM1051 main00@0x00C22 Item 0x04; Slot 11@0x00C0E -> sub03 msg 5 |
| E04 | E | 1041 | Schuss faellt: 'Elza: What was that!?' (3,0xAD) - Schuetze unbenannt | ROOM1041 main00@0x01178 Slot 8 auto -> sub11 msg 3 'gunshot', msg 2, @0x021EA |
| E05 | E | 1021 | Main Office noch ohne Roy (2+2 Zombies) | ROOM1021 main00@0x01E88ff (3,0xC9)==0 |
| E06 | E | 1011 | Roy verwundet; Dialog ('Those things broke through the rear shutters', 'My friend John... underground cell block'); Yellow Keycard (9,0x8A); (4,0xF1),(4,0xFA),(3,0x74) | ROOM1011 sub00@0x009F6 Spawn 0x47; sub02 msg 0-17, @0x00AFE/@0x00B02/@0x00B06; main00@0x00954 Item 0x39 |
| E07 | E | 1011 | Prison key = Flag (4,0xEC) aus Roys Hand (kein Item) + Zettel 0513 | ROOM1011 sub06@0x00E2C `22 04 ec 01`, @0x00E30; sub07 msg 18 |
| E08 | E | 1031 | Lobby-Rolltor-Szene: 20 Zombies hinter dem Tor, 'this must contain them for some time' (4,0x0F) | ROOM1031 sub01@0x02284 (3,0x74)==1 -> sub08; main00 20x Typ 0x16; msg 18 |
| EPW | E | 11F1 | Reserve-Strom an (4,0xF3) - PFLICHT vor den Pliers | ROOM11F1 sub18@0x016F6; ROOM11B1 sub01@0x011A0 Ck(4,0xF3)==0 -> @0x011CA Aot_reset Slot 4 -> msg 14 'It's too dark...' |
| E09 | E | 11B1 | Pliers (9,0x0A),(3,0x7F); danach Sherry an (-14886,-24048): 'wait for me quietly in that car' (3,0x84) | ROOM11B1 main00@0x00FE4 Item 0x30; sub01@0x0116E -> sub02 @0x011EC; Sherry Pos_set @0x011FC |
| E10 | E | 11E1 | Draht durchtrennen (4,0x24), Gitter hoch | ROOM11E1 sub21@0x01FC6; Obj 0 y=-6550 @0x01836 |
| EPK | Q | 10F1 | [PORT Q] Prison Key als Item in 10F1 (9,83) statt (4,0xEC) in 1011 -> braucht Minidisc 1201 -> Stimmschloss 1101 (4,0xE8) -> Blue Keycard 1111 -> Kartenleser 10D1 (3,0x32) | Spur Q, VERTRAG §0; Nebenweg aus ablauf_elza.md (10D1 sub01@0x0152A wie Leon) |
| E11 | E | 11E1 | 'Prisons'-Tuer: Kartenleser Yellow Keycard + Code 0513 -> (3,0x8B) | ROOM11E1 sub01@0x01A72-@0x01A8A `22 03 8b 01`; Slot 3 scharf @0x016C4 |
| E12 | E | 1211 | John/Kendo aus der Zelle befreit: 'Roy sent me to get you out of here'; (4,0xFC),(4,0x60); erst jetzt 10 Gefangenen-Zombies (3,0x2C); (3,0xBA) | ROOM1211 sub00@0x01E36 Spawn 0x49 (4,0xEC)==1; sub02 @0x01F76..@0x01F8E |
| E13 | E | 1021 | John erschiesst Zombie-Roy; Leiche obj 6 (-4488,0,-10684); 'The building's electric power is starting to fail...'; (4,0xF0),(3,0x5F) | ROOM1021 main00@0x01E0A Spawn 0x47; sub02 @0x01FD8..@0x01FE4; Leiche main00@0x01E40; msg 5 |
| E14 | E | 1041->1061->1121->1131 | Weg nach 3F [Nutzer L: Fenster 1121 IMMER kaputt + 1 Kraehe mehr] | Tueren 1041 Slot2@0x010D8, 1061 Slot0@0x00D12, 1121 Slot2@0x00CD8; Spur L r36 |
| E15 | E | 1151 | Irons: Original LEBEND, Examine 'Irons: I'll be fine...' (3,0x9D) [NUTZER: tot, 'He has already passed...'] | ROOM1151 main00@0x00E6A Spawn 0x45; sub01@0x00E9E; sub03 msg 2; AUFTRAG r36 |
| E16 | E | 1141 | John warnt vor dem Dach; 2+2 Gorillas durch die Fenster; (3,0xC2); Schloss zum Heliport (4,0xC3) | ROOM1141 sub00@0x00B06 Spawn 0x49 (4,0xF0)==1; sub02 @0x00C18..@0x00C20; sub05@0x00FD2 |
| E17 | E | 1171 | John: 'get this thing off of here' (obj 0/1 weg); Abstieg (4,0xE9) Slot 2 -> 10B1; KEIN Hubschrauber (nur Leon obj 2-5) | ROOM1171 sub07@0x016C8 Spawn 0x49; sub02@0x013A0; main00@0x01220 Ck(4,0xE9)==0 obj 0/1, @0x0126C Slot 2 |
| E18 | E | 10B1 | 'There's still a little girl trapped in there! I found her in the basement garage!' -> Kanalschacht im Vorhof; (3,0x83),(3,0x41) | ROOM10B1 sub01@0x01824 -> sub04 msg 0-5, @0x0195E/@0x01962; Tuer Slot 1 -> 1261 @0x016F2 |
| E19 | E | 1261 | John im Kanalgang | ROOM1261 sub03@0x01276/@0x01296/@0x012B2 (3,0x41)==1 |
| E20 | E | 2001 | John: 'Through this staircase you can access the basement garage.' (3,0x46) | ROOM2001 sub00@0x0179E -> sub02/sub04 @0x01958 |
| E21 | E | 11B1 | Rueckkehr DURCH DEN SCHACHT: Container steht bereits bei z=-19368 (verschoben), Sherry weg: 'Where has she gone!?' (3,0x44) | ROOM11B1 sub00@0x010FA (3,0x83)==1 -> @0x01122 Member_set z=-19368; sub01@0x01192 -> sub03 @0x0143C |
| E22 | E | 2001 | Sherry gefunden: 'It was because of my daddy, I've heard him call me!' (3,0x4A) | ROOM2001 sub00@0x0180A Spawn 0x4B (3,0x44)==1 -> sub05 @0x01A92 |
| E23 | E | 2011 | Sherry wartet im Break Room; Crank | ROOM2011 sub00@0x008C0 (3,0x4A)==1 |
| E24 | E | 2021..20B1 | Kanalisation (bytegleich mit Leon), Alligator | zensus.md |
| E25 | E | 11A1 | John + Sherry am Kanalausgang | ROOM11A1 sub00@0x0118C/@0x011A0 |
| E26 | E | 3001 | RPD-Leichen ('These corpses are definitively police officers.') (4,0xC6),(4,0xC7) | ROOM3001 sub03@0x016A6/@0x016AA |
| E27 | E | 3011/3021/3061 | John + Sherry folgen; 'Sherry: I'll unlock the back door from the other side!' | ROOM3061 @0x00F44/@0x00F58, msg 7/8 |
| E28 | E | 3071 | Lift-Szene mit Birkin: 'Sherry: I know how to make it work!' | ROOM3071 sub00@0x034AE/@0x034C2; sub11 msg 5-14 |
| E29 | E | 3081 | Liftfahrt mit John + Sherry | ROOM3081 sub00@0x007A0/@0x007B4 |
| E30 | E | 3091 | Gorilla-Kampf; 'Elza: Let me deal with it. John, take Sherry somewhere safe.' (3,0x39) | ROOM3091 sub07@0x016D0/@0x016D4 |
| E31 | E | 30C1 | Marvin ALLEIN: 'I'm here with Leon Kennedy' / 'helping me and Ada escape the city' / 'Elza: We got separated back at the RPD.' (msg 3) | ROOM30C1 sub00@0x00BEE Spawn 0x40 (3,0x39)==1 -> sub03 msg 1-8; @0x00C2C/@0x00C30 |
| E32 | E | 30E1 | John gebissen; Birkin-Sichtung | ROOM30E1 sub03@0x00EAC (3,0xB5)==1; msg 2 |
| E33 | E | 4001 | Sherry rennt zum Labor ihres Vaters (4,0x14) | ROOM4001 sub10 @0x01866 |
| E34 | E | 4031 | 'Let's split up' (3,0x58) | ROOM4031 sub08@0x0075C |
| E35 | E | 5021 | Cargo: 'the hole is too small for me' (4,0xC0),(3,0x3A) | ROOM5021 sub02@0x01352..@0x0135E |
| E36 | E | 5031 | Monitor Room: Sherry + Affe | ROOM5031 sub00@0x010B6/@0x010CA |
| E37 | E | 5011 | Sherry infiziert; 'Please save this girl' (3,0x4F) | ROOM5011 sub02 @0x0055A/@0x0055E |
| E38 | E | 5061 | G-Vaccine (4,0xF8) | ROOM5061 sub06@0x030E2 |
| E39 | E | 5011 | Vaccine verabreicht; 'hurry to the shelter' (3,0x4C) | ROOM5011 sub04 @0x00948/@0x0094C |
| E40 | E | 50B1 | Adas Leiche obj 0 (-11800,0,-11600): 'It's Ada. She's dead. It looks like one of those gorillas killed her.' | ROOM50B1 sub00@0x004F2; Slot 2 -> sub02 msg 0 |
| E41 | E | 50F1 | Birkin | ROOM50F1 sub00@0x009AC/@0x009D4 |
| E42 | E | 6021 | Shelter: John + Sherry | ROOM6021 sub00@0x00B96/@0x00BAA |
| E43 | E | 6031 | Zug: Marvin (liegend), John, Sherry; 'Elza: Where's Leon?' 'Sherry: He's fighting off monsters.' | ROOM6031 sub06@0x00E42/@0x00E56/@0x00E6A; msg 0/1 |
| E44 | E | 6041 | Ending | - |
| E91 | E | 1091 | (optional) Hinterhof: KEIN Feuer, KEINE Schuppen-Tuer, 4 Zombies = Leons Nach-Rettungs-Zustand | ROOM1091 main00@0x0216E..@0x021AA == ROOM1090 sub00@0x023A0..@0x023DC (Kill-Bits 0xf8-0xfb); Tuer 1051 Slot 5 @0x00B7A frei |

## 2. Kreuz-Kanten (zwischen den Szenarien)

| a VOR b | Art | Grund | Beleg |
|---|---|---|---|
| E13 < L20a | hart | Roys Leiche liegt bei Leon unbedingt an EXAKT der Position, die Elzas Szene erzeugt; 'died recently' | ROOM1020 main00@0x01DEE obj 6 (-4488,0,-10684) == ROOM1021 main00@0x01E40 obj 6 (nur bei (3,0x5F)==1, gesetzt sub02@0x01FE4) |
| E12 < L21a | hart | Die 10 Gefangenen-Zombies stehen bei Leon ohne Flag, bei Elza erst nach Johns Befreiung; Leons Zelle hat keinen 'someone inside'-Text | ROOM1210 main00@0x01D86ff vs ROOM1211 sub00 Ck(3,0x2C)==1 (gesetzt sub02@0x01F8A); ROOM1211 sub00@0x01DEE Slot 8 msg 10 fehlt in 1210 |
| E11 < L11E | hart | 'Prisons'-Tuer ist fuer Leon beim ersten Erreichen von 11E0 bereits aufgeschlossen = der Zustand, den Elzas Kartenleser herstellt | ROOM1240 sub02@0x0055A (3,0x8B)=1; ROOM11E1 sub01@0x01A8A `22 03 8b 01`; Leser ROOM11E0 main00@0x01576 |
| L10 < E91 | hart | Elzas Hinterhof ist bytegleich Leons Zustand NACH der Rettung (4 Zombies, kein Feuer, keine Schuppen-Tuer) | ROOM1091 main00@0x0216E..@0x021AA == ROOM1090 sub00@0x023A0..@0x023DC; Feuer Typ 0x26 + Slot 3 @0x02352 nur in 1090 |
| E09 < L18 | hart | Sherry steht bei Elza an (-14886,-24048) und wartet im Auto; bei Leon rennt sie von genau dort weg | ROOM11B1 sub02@0x011FC Pos_set (-14886,0,-24048) vs ROOM11B0 sub04@0x01324 Pos_set (-14886,0,-24048) -> @0x013AC -30000 |
| L18 < E21 | hart | Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen | ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way' |
| L20 < E21 | hart | Elza kommt DURCH den Schacht und findet den Container bereits verschoben (z=-19368) = der Zustand nach Leons/Marvins Schieben | ROOM11B1 sub00@0x01122 Member_set m02=-19368 bei (3,0x83)==1 == ROOM11B0 main00@0x010EA obj 0 (-2168,0,-19368) bei (4,0xCD)==1 |
| L18 < E22 | hart | Sherry ist in den Kanal gelaufen (Leon) und wird dort von Elza gefunden | ROOM2001 sub00@0x0180A Spawn 0x4B (1797,-1800,-20962); ROOM2000 msg 2-6 (Leon sucht sie vergeblich) |
| L26 < E31 | hart | Marvin ist in Elzas 30C1 ALLEIN und noch nicht gebissen ('helping me and Ada escape') -> nach der Liftfahrt (Marvin mit Leon+Ada in 3080) ... | ROOM3080 @0x007AC/@0x007C0/@0x007D4 (3 Spawns); ROOM30C1 sub03 msg 2/4 |
| E31 < L27 | hart | ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') | ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11 |
| L33 < E40 | hart | Adas Leiche liegt bei Elza an Leons Todesposition | ROOM50B1 sub00@0x004F2 obj 0 (-11800,0,-11600) == ROOM50B0 @0x00566 |
| L03 < E17 | hart | Bei Elza steht kein Hubschrauber mehr auf dem Dach (Leons obj 2-5 existieren nur in 1170 sub15) | ROOM1170 sub15@0x017E6..@0x0184C; ROOM1171 ohne diese Objekte |
| L35 < E43 | hart | Zug-Ende gleichzeitig: 'Where's Elza?' / 'Where's Leon?' (beide 'out there fighting'); modelliert als Treffen am Zug | ROOM6030 msg 1/2; ROOM6031 msg 0/1 |
| E42 < L36 | hart | dito (Gegenrichtung des Treffens: John, Sherry, Marvin liegen im Zug, bevor Leon einsteigt) | ROOM6030 sub06@0x0101C/@0x01030/@0x01044 |
| P07 < E15 | nutzer | Irons liegt bei Elza TOT ('He has already passed...') -> Leons Port-Todesszene liegt davor | AUFTRAG r36 Z.36; Spur L |
| P08 < E14 | nutzer | Fenster 1121 bei Elza IMMER kaputt (+1 Kraehe) = der Zustand nach Leons Montage (1120 Fenster zerbricht) | AUFTRAG r36 Z.34; fenster_1120.c (9,79) |
| EPK < E12 | port | Port Q: der Prison Key (Item) oeffnet Johns Zelle | Spur Q; Original: ROOM1211 sub00@0x01DEA Ck(4,0xEC) |
| E13 < L17 | vorschlag | 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) | ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch |
| E03 < L07 | vorschlag | Die SIG P228 fehlt an Leons Leiche (kein Untersuch-AOT, kein Item): Elza hat sie vorher mitgenommen | ROOM1051 main00@0x00C22 Item 0x04 + Slot 11; ROOM1050 ohne Slot 11/Item |
| E04 < L06 | vorschlag | Der Schuss, den Elza in 1041 hoert, kann Leons Schuss im selben Korridor (1040, Abstieg) sein - alternativ Roy; Textbeleg fehlt | ROOM1041 sub11 msg 3 'gunshot'; Leon in 1040 (L06) mit 20 Zombies hinter dem Rolltor |
| E17 < E91 | vorschlag | Port: Elzas Hinterhof-Tuer 1051->1091 erst nach dem Dach-Abstieg (4,0xE9) freigeben, damit 1091 nach Leons Rettung liegt | ROOM1051 Slot 5 @0x00B7A; (4,0xE9) ROOM1171 sub02@0x013A0 |
| L17 < L20a | vorschlag | Port: Roys Leiche in Leons 1020 erst nach dem Strom (4,0xF3) zeigen -> mit E13<L17 liegt sie sicher nach Elzas Szene | ROOM1020 main00@0x01DEE (unbedingt) -> Port-Gate (4,0xF3) |

Ablauf-Kanten mit nicht-trivialem Beleg:

- L08 < P03: Port: Tuer 1050->10A0 gesperrt bis (3,0xBB) (adaruf_1050.c) -> Leon muss Ada retten, bevor er in den Keller kommt
- L11 < L11E: Port P03: erst nach (3,0xBB) nach 10A0 -> 11E0
- L11E < L17: Kesselraum nur ueber ROOM11E0 Slot 0 @0x01512 erreichbar
- L17 < L18: ROOM11B0 main00@0x00F56 (4,0x06)=1 nur bei (4,0xF3)==1; sub01@0x011B6 Ck(4,0x06)==1 -> sub04
- L18 < L12a: gleicher Raum: die Sherry-Szene (sub01@0x011AE, auto) laeuft beim ersten Eintritt mit Strom; Pliers danach (Slot 4 sonst msg 15 'too dark')
- L12a < L12b: ROOM11E0 sub00@0x01798 Ck(3,0x7F)==0 -> nur Text msg 10; Leichenhalle hinter dem Gitter (SCA Zelle 0)
- L12b < L13: ROOM1100 sub00@0x00B14 Slot 2 -> sub07 (3,0x67)==1
- L13 < L14: ROOM10D0 sub20 Ck(9,0x34)==1 (Blue Keycard)
- L14 < L15: ROOM10D0: Marvin-Szene am Rolltor-Schalter Slot 18 (sub00@0x01242); Reihenfolge zu L14 = PORT-ANNAHME (Szene 10F0 setzt 'Leon! You already made it!' voraus)
- L15 < L16: Tuer 10D0 Slot 0 -> 10F0 @0x01174 nach (3,0x32)
- L16 < P05: Port: Szene beim ersten Betreten 10F0 (cut_10f0.c)
- P05 < P06: Port Q: Schluessel im kleinen Buero (gleicher Raum)
- P06 < P04: Port: nach (9,71) ist 1060->1040 gesperrt bis (9,73) (tuer1060_1040.c) -> zurueck zu Irons
- P04 < P07: Port: Gate (9,71)=1 & (3,0x94)=1 -> Todesszene (irons_tod_1150.c)
- P07 < P08: Port: Montage unmittelbar nach der Todesszene (Ereignis 21)
- P08 < L19: Port: 11C0-Schnitt nur solange (4,0x40)=0 (L_cut1150.md §8.5); Tuer 11B0->11C0 erst mit Schluessel (9,82)
- L19 < L20: ROOM11B0 sub01@0x011CE Ck(3,0x43)==1 -> sub06 (Original zwingend)
- L20 < P09: Port: Verlaengerung derselben Szene (Spur J)
- P09 < L21: ROOM11B0 Slot 5 -> 1260 scharf @0x0172E nach (4,0xCD)
- E08 < EPW: ROOM11B1 sub01@0x011A0 Ck(4,0xF3)==0 -> @0x011CA Slot 4 (Pliers) = msg 14 'too dark' -> Strom VOR den Pliers (im Original-Ablauf ablauf_elza.md E9 fehlte das)
- EPW < E09: s.o.
- E09 < E10: ROOM11E1 sub00@0x01798 Ck(3,0x7F)==0 -> nur Text
- E10 < EPK: Port Q: Prison Key liegt in 10F1; der Funkraum ist nur ueber 10D1 (Blue Keycard 1111 <- Stimmschloss 1101 <- Minidisc 1201 hinter dem Gitter) erreichbar
- EPK < E11: Reihenfolge frei (beide noetig vor E12); hier EPK zuerst
- E11 < E12: ROOM1211 sub00@0x01DEA Ck(4,0xEC)==0 -> Text msg 10; Tuer 11E1 Slot 3 erst nach (3,0x8B)
- E12 < E13: ROOM1021 main00@0x01DDE Ck(4,0x60)==1 (gesetzt ROOM1211 sub02@0x01F82)
- E13 < E14: ROOM1141 sub00@0x00B02 Ck(4,0xF0)==1 (gesetzt ROOM1021 sub02@0x01FE0) -> Elzas Pflichtweg nach 3F erst nach Roy
- E14 < E15: 1151 liegt am 3F-Korridor (optional; Reihenfolge zu E16 frei)
- E15 < E16: s.o.
- E16 < E17: ROOM1171 sub07@0x016C4 Ck(3,0xC2)==1; Schloss (4,0xC3) aus 1141 sub05
- E17 < E18: ROOM10B1 sub01@0x01816 Ck(3,0x82)==1 (gesetzt 1171 sub02@0x013A4)
- E18 < E19: ROOM1261 sub03@0x01268 Ck(3,0x41)==1
- E19 < E20: ROOM2001 sub00@0x0179E Ck(3,0x41)==1
- E20 < E21: ROOM11B1 sub01@0x01192 Ck(3,0x46)==1 (gesetzt 2001 sub04@0x01958)
- E21 < E22: ROOM2001 sub00@0x0180A Ck(3,0x44)==1 (gesetzt 11B1 sub03@0x0143C)
- E22 < E23: ROOM2011 sub00@0x008C0 Ck(3,0x4A)==1
- E30 < E31: ROOM30C1 sub00@0x00BEE Ck(3,0x39)==1 (gesetzt 3091 sub07@0x016D4)

## 3. Modelle

### Modell c - PARALLEL / raumweise versetzt (nur die Kreuz-Kanten)

Ergebnis: **widerspruchsfrei** (keine Zyklen). Moegliche Gesamt-Reihenfolge = topologische Sortierung; Schicht = laengster Weg von den Quellen (frueheste Position). Ereignisse derselben Schicht sind untereinander unbestimmt.

| Schicht | Leon | Elza |
|---|---|---|
| 0 | L01 (1240) | E01 (1241) |
| 1 | L02 (1170) | E02 (1031) |
| 2 | L03 (1170) | E03 (1051) |
| 3 | L04 (1170) | E04 (1041) |
| 4 | L05 (1150) | E05 (1021) |
| 5 | L06 (1130..1050) | E06 (1011) |
| 6 | L07 (1050) | E07 (1011) |
| 7 | L08 (1090) | E08 (1031) |
| 8 | P03 (1050->10A0) | EPW (11F1) |
| 9 | L09 (1000) | E09 (11B1) |
| 10 | L10 (1090) | E10 (11E1) |
| 11 | L11 (1050) | EPK (10F1) |
| 12 |  | E11 (11E1) |
| 13 | L11E (11E0) | E12 (1211) |
| 14 | L21a (1210) | E13 (1021) |
| 15 | L17 (11F0) |  |
| 16 | L18 (11B0), L20a (1020) |  |
| 17 | L12a (11B0) |  |
| 18 | L12b (11E0/1200) |  |
| 19 | L13 (1100/1110) |  |
| 20 | L14 (10D0) |  |
| 21 | L15 (10D0) |  |
| 22 | L16 (10F0) |  |
| 23 | P05 (10F0) |  |
| 24 | P06 (10F0) |  |
| 25 | P04 (1060->1040) |  |
| 26 | P07 (1150) |  |
| 27 | P08 (1130/1040/1030/11C0/1120) |  |
| 28 | L19 (11C0) | E14 (1041->1061->1121->1131) |
| 29 | L20 (11B0) | E15 (1151) |
| 30 | P09 (11B0) | E16 (1141) |
| 31 | L21 (1260/2000) | E17 (1171) |
| 32 | L22 (2010..20B0) | E18 (10B1), E91 (1091) |
| 33 | L23 (11A0) | E19 (1261) |
| 34 | L24 (3000) | E20 (2001) |
| 35 | L25 (3010/3020/3060) | E21 (11B1) |
| 36 | L26 (3070/3080) | E22 (2001) |
| 37 |  | E23 (2011) |
| 38 |  | E24 (2021..20B1) |
| 39 |  | E25 (11A1) |
| 40 |  | E26 (3001) |
| 41 |  | E27 (3011/3021/3061) |
| 42 |  | E28 (3071) |
| 43 |  | E29 (3081) |
| 44 |  | E30 (3091) |
| 45 |  | E31 (30C1) |
| 46 | L27 (30E0) | E32 (30E1) |
| 47 | L28 (4000) | E33 (4001) |
| 48 | L29 (4010) | E34 (4031) |
| 49 | L30 (5060) | E35 (5021) |
| 50 | L31 (4010) | E36 (5031) |
| 51 | L32 (5090/6000) | E37 (5011) |
| 52 | L33 (50B0) | E38 (5061) |
| 53 | L34 (50E0) | E39 (5011) |
| 54 | L35 (6020) | E40 (50B1) |
| 55 |  | E41 (50F1) |
| 56 |  | E42 (6021) |
| 57 | L36 (6030) | E43 (6031) |
| 58 | L37 (6040) | E44 (6041) |

Lesbare Gesamt-Reihenfolge (eine gueltige Linearisierung, Leon/Elza verschraenkt):

```
  L01 < E01 < L02 < E02 < L03 < E03 < L04 < E04 < L05 < E05 < L06 < E06
  L07 < E07 < L08 < E08 < P03 < EPW < L09 < E09 < L10 < E10 < L11 < EPK
  E11 < E12 < L11E < E13 < L21a < L17 < L18 < L20a < L12a < L12b < L13 < L14
  L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < E14 < L20 < E15 < P09
  E16 < L21 < E17 < L22 < E18 < E91 < L23 < E19 < L24 < E20 < L25 < E21
  L26 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < E32
  L27 < E33 < L28 < E34 < L29 < E35 < L30 < E36 < L31 < E37 < L32 < E38
  L33 < E39 < L34 < E40 < L35 < E41 < E42 < E43 < L36 < E44 < L37
```

### Modell a - Elza GANZ NACH Leon (Leons Ende L37 vor Elzas Anfang E01)

Ergebnis: **WIDERSPRUCH** - 1 Zyklus-Komponente(n).

- Komponente mit 85 Knoten: E01, E02, E03, E04, E05, E06, E07, E08, E09, E10, E11, E12, E13, E14, E15, E16, E17, E18, E19, E20, E21, E22, E23, E24, E25, E26, E27, E28, E29, E30, E31, E32, E33, E34, E35, E36, E37, E38, E39, E40, E41, E42, EPK, EPW, L06, L07, L08, L09, L10, L11, L11E, L12a, L12b, L13, L14, L15, L16, L17, L18, L19, L20, L21, L22, L23, L24, L25, L26, L27, L28, L29, L30, L31, L32, L33, L34, L35, L36, L37, P03, P04, P05, P06, P07, P08, P09
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < E12 < E13 < L17 < L18 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < L34 < L35 < L36 < L37 < E01
    - Kante E13 < L17 [vorschlag]: 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) (ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < E12 < E13 < L17 < L18 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < E40 < E41 < E42 < L36 < L37 < E01
    - Kante E13 < L17 [vorschlag]: 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) (ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L33 < E40 [hart]: Adas Leiche liegt bei Elza an Leons Todesposition (ROOM50B1 sub00@0x004F2 obj 0 (-11800,0,-11600) == ROOM50B0 @0x00566)
    - Kante E42 < L36 [hart]: dito (Gegenrichtung des Treffens: John, Sherry, Marvin liegen im Zug, bevor Leon einsteigt) (ROOM6030 sub06@0x0101C/@0x01030/@0x01044)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < E12 < E13 < L17 < L18 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < E32 < E33 < E34 < E35 < E36 < E37 < E38 < E39 < E40 < E41 < E42 < L36 < L37 < E01
    - Kante E13 < L17 [vorschlag]: 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) (ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch)
    - Kante L18 < E22 [hart]: Sherry ist in den Kanal gelaufen (Leon) und wird dort von Elza gefunden (ROOM2001 sub00@0x0180A Spawn 0x4B (1797,-1800,-20962); ROOM2000 msg 2-6 (Leon sucht sie vergeblich))
    - Kante E42 < L36 [hart]: dito (Gegenrichtung des Treffens: John, Sherry, Marvin liegen im Zug, bevor Leon einsteigt) (ROOM6030 sub06@0x0101C/@0x01030/@0x01044)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < E12 < E13 < L17 < L18 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < L34 < L35 < L36 < L37 < E01
    - Kante E13 < L17 [vorschlag]: 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) (ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch)
    - Kante L18 < E22 [hart]: Sherry ist in den Kanal gelaufen (Leon) und wird dort von Elza gefunden (ROOM2001 sub00@0x0180A Spawn 0x4B (1797,-1800,-20962); ROOM2000 msg 2-6 (Leon sucht sie vergeblich))
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < E12 < E13 < L17 < L18 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < E40 < E41 < E42 < L36 < L37 < E01
    - Kante E13 < L17 [vorschlag]: 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) (ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch)
    - Kante L18 < E22 [hart]: Sherry ist in den Kanal gelaufen (Leon) und wird dort von Elza gefunden (ROOM2001 sub00@0x0180A Spawn 0x4B (1797,-1800,-20962); ROOM2000 msg 2-6 (Leon sucht sie vergeblich))
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L33 < E40 [hart]: Adas Leiche liegt bei Elza an Leons Todesposition (ROOM50B1 sub00@0x004F2 obj 0 (-11800,0,-11600) == ROOM50B0 @0x00566)
    - Kante E42 < L36 [hart]: dito (Gegenrichtung des Treffens: John, Sherry, Marvin liegen im Zug, bevor Leon einsteigt) (ROOM6030 sub06@0x0101C/@0x01030/@0x01044)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < L11E < L17 < L18 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < E32 < E33 < E34 < E35 < E36 < E37 < E38 < E39 < E40 < E41 < E42 < L36 < L37 < E01
    - Kante E11 < L11E [hart]: 'Prisons'-Tuer ist fuer Leon beim ersten Erreichen von 11E0 bereits aufgeschlossen = der Zustand, den Elzas Kartenleser herstellt (ROOM1240 sub02@0x0055A (3,0x8B)=1; ROOM11E1 sub01@0x01A8A `22 03 8b 01`; Leser ROOM11E0 main00@0x01576)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
    - Kante E42 < L36 [hart]: dito (Gegenrichtung des Treffens: John, Sherry, Marvin liegen im Zug, bevor Leon einsteigt) (ROOM6030 sub06@0x0101C/@0x01030/@0x01044)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < L11E < L17 < L18 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < L34 < L35 < L36 < L37 < E01
    - Kante E11 < L11E [hart]: 'Prisons'-Tuer ist fuer Leon beim ersten Erreichen von 11E0 bereits aufgeschlossen = der Zustand, den Elzas Kartenleser herstellt (ROOM1240 sub02@0x0055A (3,0x8B)=1; ROOM11E1 sub01@0x01A8A `22 03 8b 01`; Leser ROOM11E0 main00@0x01576)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < L11E < L17 < L18 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < E40 < E41 < E42 < L36 < L37 < E01
    - Kante E11 < L11E [hart]: 'Prisons'-Tuer ist fuer Leon beim ersten Erreichen von 11E0 bereits aufgeschlossen = der Zustand, den Elzas Kartenleser herstellt (ROOM1240 sub02@0x0055A (3,0x8B)=1; ROOM11E1 sub01@0x01A8A `22 03 8b 01`; Leser ROOM11E0 main00@0x01576)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L33 < E40 [hart]: Adas Leiche liegt bei Elza an Leons Todesposition (ROOM50B1 sub00@0x004F2 obj 0 (-11800,0,-11600) == ROOM50B0 @0x00566)
    - Kante E42 < L36 [hart]: dito (Gegenrichtung des Treffens: John, Sherry, Marvin liegen im Zug, bevor Leon einsteigt) (ROOM6030 sub06@0x0101C/@0x01030/@0x01044)
    - Kante L37 < E01 [modell]: Modell a: Elzas Szenario beginnt nach Leons Ende (-)

### Modell a1 - Elza beginnt nach Leons STAGE1 (P09 vor E01)

Ergebnis: **WIDERSPRUCH** - 1 Zyklus-Komponente(n).

- Komponente mit 39 Knoten: E01, E02, E03, E04, E05, E06, E07, E08, E09, E10, E11, E12, E13, EPK, EPW, L06, L07, L08, L09, L10, L11, L11E, L12a, L12b, L13, L14, L15, L16, L17, L18, L19, L20, P03, P04, P05, P06, P07, P08, P09
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < E12 < E13 < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < L20 < P09 < E01
    - Kante E13 < L17 [vorschlag]: 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) (ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch)
    - Kante P09 < E01 [modell]: Modell a1: Elza betritt das Revier, nachdem Leon es verlassen hat (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E11 < L11E < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < L20 < P09 < E01
    - Kante E11 < L11E [hart]: 'Prisons'-Tuer ist fuer Leon beim ersten Erreichen von 11E0 bereits aufgeschlossen = der Zustand, den Elzas Kartenleser herstellt (ROOM1240 sub02@0x0055A (3,0x8B)=1; ROOM11E1 sub01@0x01A8A `22 03 8b 01`; Leser ROOM11E0 main00@0x01576)
    - Kante P09 < E01 [modell]: Modell a1: Elza betritt das Revier, nachdem Leon es verlassen hat (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < E10 < EPK < E12 < E13 < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < L20 < P09 < E01
    - Kante EPK < E12 [port]: Port Q: der Prison Key (Item) oeffnet Johns Zelle (Spur Q; Original: ROOM1211 sub00@0x01DEA Ck(4,0xEC))
    - Kante E13 < L17 [vorschlag]: 'The building's electric power is starting to fail...' (E13 msg 5) -> der Generator faellt nach Elzas Start wieder aus; Leon startet ihn danach neu (erklaert, warum BEIDE das Generator-Raetsel loesen) (ROOM1021 msg 5; ROOM11F0/11F1 sub18@0x016F6 identisch)
    - Kante P09 < E01 [modell]: Modell a1: Elza betritt das Revier, nachdem Leon es verlassen hat (-)
  - Zyklus: E01 < E02 < E03 < E04 < E05 < E06 < E07 < E08 < EPW < E09 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < L20 < P09 < E01
    - Kante E09 < L18 [hart]: Sherry steht bei Elza an (-14886,-24048) und wartet im Auto; bei Leon rennt sie von genau dort weg (ROOM11B1 sub02@0x011FC Pos_set (-14886,0,-24048) vs ROOM11B0 sub04@0x01324 Pos_set (-14886,0,-24048) -> @0x013AC -30000)
    - Kante P09 < E01 [modell]: Modell a1: Elza betritt das Revier, nachdem Leon es verlassen hat (-)
  - Zyklus: E01 < E02 < E03 < E04 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < L20 < P09 < E01
    - Kante E04 < L06 [vorschlag]: Der Schuss, den Elza in 1041 hoert, kann Leons Schuss im selben Korridor (1040, Abstieg) sein - alternativ Roy; Textbeleg fehlt (ROOM1041 sub11 msg 3 'gunshot'; Leon in 1040 (L06) mit 20 Zombies hinter dem Rolltor)
    - Kante P09 < E01 [modell]: Modell a1: Elza betritt das Revier, nachdem Leon es verlassen hat (-)
  - Zyklus: E01 < E02 < E03 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < L20 < P09 < E01
    - Kante E03 < L07 [vorschlag]: Die SIG P228 fehlt an Leons Leiche (kein Untersuch-AOT, kein Item): Elza hat sie vorher mitgenommen (ROOM1051 main00@0x00C22 Item 0x04 + Slot 11; ROOM1050 ohne Slot 11/Item)
    - Kante P09 < E01 [modell]: Modell a1: Elza betritt das Revier, nachdem Leon es verlassen hat (-)

### Modell b - Elza GANZ VOR Leon (Elzas Ende E44 vor Leons Anfang L01)

Ergebnis: **WIDERSPRUCH** - 1 Zyklus-Komponente(n).

- Komponente mit 75 Knoten: E14, E15, E16, E17, E18, E19, E20, E21, E22, E23, E24, E25, E26, E27, E28, E29, E30, E31, E32, E33, E34, E35, E36, E37, E38, E39, E40, E41, E42, E43, E44, L01, L02, L03, L04, L05, L06, L07, L08, L09, L10, L11, L11E, L12a, L12b, L13, L14, L15, L16, L17, L18, L19, L20, L21, L22, L23, L24, L25, L26, L27, L28, L29, L30, L31, L32, L33, L34, L35, P03, P04, P05, P06, P07, P08, P09
  - Zyklus: E17 < E18 < E19 < E20 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < E32 < E33 < E34 < E35 < E36 < E37 < E38 < E39 < E40 < E41 < E42 < E43 < E44 < L01 < L02 < L03 < E17
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L03 < E17 [hart]: Bei Elza steht kein Hubschrauber mehr auf dem Dach (Leons obj 2-5 existieren nur in 1170 sub15) (ROOM1170 sub15@0x017E6..@0x0184C; ROOM1171 ohne diese Objekte)
  - Zyklus: E17 < E18 < E19 < E20 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < L34 < L35 < E43 < E44 < L01 < L02 < L03 < E17
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L35 < E43 [hart]: Zug-Ende gleichzeitig: 'Where's Elza?' / 'Where's Leon?' (beide 'out there fighting'); modelliert als Treffen am Zug (ROOM6030 msg 1/2; ROOM6031 msg 0/1)
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L03 < E17 [hart]: Bei Elza steht kein Hubschrauber mehr auf dem Dach (Leons obj 2-5 existieren nur in 1170 sub15) (ROOM1170 sub15@0x017E6..@0x0184C; ROOM1171 ohne diese Objekte)
  - Zyklus: E17 < E18 < E19 < E20 < E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < E40 < E41 < E42 < E43 < E44 < L01 < L02 < L03 < E17
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L33 < E40 [hart]: Adas Leiche liegt bei Elza an Leons Todesposition (ROOM50B1 sub00@0x004F2 obj 0 (-11800,0,-11600) == ROOM50B0 @0x00566)
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L03 < E17 [hart]: Bei Elza steht kein Hubschrauber mehr auf dem Dach (Leons obj 2-5 existieren nur in 1170 sub15) (ROOM1170 sub15@0x017E6..@0x0184C; ROOM1171 ohne diese Objekte)
  - Zyklus: E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < E32 < E33 < E34 < E35 < E36 < E37 < E38 < E39 < E40 < E41 < E42 < E43 < E44 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < E21
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
  - Zyklus: E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < L34 < L35 < E43 < E44 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < E21
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L35 < E43 [hart]: Zug-Ende gleichzeitig: 'Where's Elza?' / 'Where's Leon?' (beide 'out there fighting'); modelliert als Treffen am Zug (ROOM6030 msg 1/2; ROOM6031 msg 0/1)
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
  - Zyklus: E21 < E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < E40 < E41 < E42 < E43 < E44 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < E21
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L33 < E40 [hart]: Adas Leiche liegt bei Elza an Leons Todesposition (ROOM50B1 sub00@0x004F2 obj 0 (-11800,0,-11600) == ROOM50B0 @0x00566)
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
  - Zyklus: E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < E32 < E33 < E34 < E35 < E36 < E37 < E38 < E39 < E40 < E41 < E42 < E43 < E44 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < E22
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L18 < E22 [hart]: Sherry ist in den Kanal gelaufen (Leon) und wird dort von Elza gefunden (ROOM2001 sub00@0x0180A Spawn 0x4B (1797,-1800,-20962); ROOM2000 msg 2-6 (Leon sucht sie vergeblich))
  - Zyklus: E22 < E23 < E24 < E25 < E26 < E27 < E28 < E29 < E30 < E31 < L27 < L28 < L29 < L30 < L31 < L32 < L33 < L34 < L35 < E43 < E44 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < E22
    - Kante E31 < L27 [hart]: ... und vor 30E0, wo Marvin gebissen zu Leon zurueckkommt ('The hall is no good...!') (ROOM30E0 sub11@0x0108C/@0x010A0; msg 10/11)
    - Kante L35 < E43 [hart]: Zug-Ende gleichzeitig: 'Where's Elza?' / 'Where's Leon?' (beide 'out there fighting'); modelliert als Treffen am Zug (ROOM6030 msg 1/2; ROOM6031 msg 0/1)
    - Kante E44 < L01 [modell]: Modell b: Leons Szenario beginnt nach Elzas Ende (-)
    - Kante L18 < E22 [hart]: Sherry ist in den Kanal gelaufen (Leon) und wird dort von Elza gefunden (ROOM2001 sub00@0x0180A Spawn 0x4B (1797,-1800,-20962); ROOM2000 msg 2-6 (Leon sucht sie vergeblich))

### Modell b1 - Elza verlaesst das Revier, bevor Leon es betritt (E22 vor L01)

Ergebnis: **WIDERSPRUCH** - 1 Zyklus-Komponente(n).

- Komponente mit 37 Knoten: E14, E15, E16, E17, E18, E19, E20, E21, E22, L01, L02, L03, L04, L05, L06, L07, L08, L09, L10, L11, L11E, L12a, L12b, L13, L14, L15, L16, L17, L18, L19, L20, P03, P04, P05, P06, P07, P08
  - Zyklus: E14 < E15 < E16 < E17 < E18 < E19 < E20 < E21 < E22 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < E14
    - Kante E22 < L01 [modell]: Modell b1: Elzas STAGE1 komplett vor Leons STAGE1 (-)
    - Kante P08 < E14 [nutzer]: Fenster 1121 bei Elza IMMER kaputt (+1 Kraehe) = der Zustand nach Leons Montage (1120 Fenster zerbricht) (AUFTRAG r36 Z.34; fenster_1120.c (9,79))
  - Zyklus: E15 < E16 < E17 < E18 < E19 < E20 < E21 < E22 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < E15
    - Kante E22 < L01 [modell]: Modell b1: Elzas STAGE1 komplett vor Leons STAGE1 (-)
    - Kante P07 < E15 [nutzer]: Irons liegt bei Elza TOT ('He has already passed...') -> Leons Port-Todesszene liegt davor (AUFTRAG r36 Z.36; Spur L)
  - Zyklus: E17 < E18 < E19 < E20 < E21 < E22 < L01 < L02 < L03 < E17
    - Kante E22 < L01 [modell]: Modell b1: Elzas STAGE1 komplett vor Leons STAGE1 (-)
    - Kante L03 < E17 [hart]: Bei Elza steht kein Hubschrauber mehr auf dem Dach (Leons obj 2-5 existieren nur in 1170 sub15) (ROOM1170 sub15@0x017E6..@0x0184C; ROOM1171 ohne diese Objekte)
  - Zyklus: E21 < E22 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < L12a < L12b < L13 < L14 < L15 < L16 < P05 < P06 < P04 < P07 < P08 < L19 < L20 < E21
    - Kante E22 < L01 [modell]: Modell b1: Elzas STAGE1 komplett vor Leons STAGE1 (-)
    - Kante L20 < E21 [hart]: Elza kommt DURCH den Schacht und findet den Container bereits verschoben (z=-19368) = der Zustand nach Leons/Marvins Schieben (ROOM11B1 sub00@0x01122 Member_set m02=-19368 bei (3,0x83)==1 == ROOM11B0 main00@0x010EA obj 0 (-2168,0,-19368) bei (4,0xCD)==1)
  - Zyklus: E21 < E22 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < E21
    - Kante E22 < L01 [modell]: Modell b1: Elzas STAGE1 komplett vor Leons STAGE1 (-)
    - Kante L18 < E21 [hart]: Bei Elzas Rueckkehr ist Sherry weg ('Where has she gone!?'); Leon sah sie in den Schacht laufen (ROOM11B1 sub03 msg 10/11; ROOM2000 msg 4 'before meeting Ada I saw her coming in this way')
  - Zyklus: E22 < L01 < L02 < L03 < L04 < L05 < L06 < L07 < L08 < P03 < L09 < L10 < L11 < L11E < L17 < L18 < E22
    - Kante E22 < L01 [modell]: Modell b1: Elzas STAGE1 komplett vor Leons STAGE1 (-)
    - Kante L18 < E22 [hart]: Sherry ist in den Kanal gelaufen (Leon) und wird dort von Elza gefunden (ROOM2001 sub00@0x0180A Spawn 0x4B (1797,-1800,-20962); ROOM2000 msg 2-6 (Leon sucht sie vergeblich))

