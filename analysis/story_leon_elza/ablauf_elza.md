# Ablauf Elza (ROOMxxx1) — Hauptweg aus Tuer-Sperren, Schluesseln und Flag-Ketten

Stand 2026-10-10. Beleg je Ereignis: Raum + Region@Datei-Offset (RDT unter `re15_port/shared_assets/PSX/STAGEn/`),
Nachrichten als `msg N` (Text in `texte_elza.md`), Flags `(Bank,Bit)`. Werkzeuge: `story_view.py`, `scd_story.py`,
`flag_xref.txt`. In RE1.5 gibt es keine Schluessel-Felder im Door_aot_set (scd_vm.c op_door_aot_set: Nutzlast =
Zielposition/Stage/Raum/Cut) — **alle Sperren sind SCD-Bedingungen** (Tuer `sce 0` = inert, Freischaltung per
Aot_reset nach Flag). Figuren-Details: `B_figuren.md`.

Start: PSX.EXE @0x8001d324 Raum 0x03 = ROOM1031 (Lobby), davor Vorspann ROOM1241 (Tuer @0x0051A -> ROOM1031).

## STAGE1 — Polizeirevier

| # | Raum | Ereignis | Beleg | Setzt / braucht |
|---|---|---|---|---|
| E1 | 1241 | Vorspann-Text (msg 0-5 "A bizarre incident...") | sub02 @0x0055A Set(3,0x6F), @0x0055E Set(3,0x70); Tuer Slot 0 -> 1031 | — |
| E2 | 1031 | Erster Eintritt: Tuer Slot 18 -> 1241 (Opening), Rueckkehr Slot 19; Prolog msg 20-23 "We barricaded ourselves..."; dann msg 18 "Elza: That was close, this must contain them for some time..." / msg 19 "Elza: Hey, is anyone here?!" | main00 @0x02052 (`Ck(3,0xC1)==0`), @0x02082 + sub15 @0x02A68 Set(3,0x7D)=1, (3,0xCF)=1; sub13 @0x02A08/@0x02A24 | (3,0xC1),(3,0x7D),(3,0xCF) |
| E3 | 1051 | Elza nimmt die SIG P228 von einem toten Polizisten ("He's got a big bite on his neck... The weapon can be useful. Sorry, I'll just borrow this...") | main00 @0x00C22 Item 0x04 x15 (Taken-Bit (9,0xA5)); sub03 msg 5 | Elzas Pistole (nur Elza; Leon: 1050 ohne Item) |
| E4 | 1041 | West Corridor: Schuss faellt, "Elza: What was that!?" (vor der Roy-Szene) | main00 @0x01178 AOT Slot 8 auto (`Ck(4,0xF1)==0 & Ck(3,0xAD)==0`) -> sub11 msg 3 "gunshot", msg 2 | Set(3,0xAD)=1 @0x021EA |
| E5 | 1021 | Main Office (noch ohne Roy; 2+2 Zombies 0x10/0x11 main00 @0x01E88ff solange (3,0xC8)==0/(3,0xC9)==0) | main00 @0x01E64-@0x01F2E | — |
| E6 | 1011 | **Roy** verwundet: Dialog msg 0-17 ("My friend John should still be in the underground cell block. Please, you have to help him"), Yellow Keycard liegt im Raum | sub00 @0x009F6 Spawn 0x47 (`Ck(4,0xF1)==0 & var0A==0`); sub02 @0x00AFE Set(4,0xF1)=1, @0x00B02 (4,0xFA)=1, @0x00B06 (3,0x74)=1; Item main00 @0x00954 0x39 (Taken (9,0x8A)) | (4,0xF1),(4,0xFA),(3,0x74); Yellow Keycard |
| E7 | 1011 | Roy untersuchen: "He is holding something..." / **"You've taken the Prison key."** -> danach "He has passed out... The numbers 0513 are printed on the slip." | sub00 @0x00A3E (`Ck(4,0xFA)==1`) AOT Slot 5 -> sub06 @0x00E2C Set(4,0xEC)=1, @0x00E30 (3,0x78)=1; sub07 msg 18 | **(4,0xEC) = "Prison key" (kein Item)**; Code 0513 |
| E8 | 1031 | Nach Roy: Lobby-Rolltor-Szene (sub08, "Guess it was only a matter of time..." msg 1) — 20 Zombies Typ 0x16 hinter dem Tor | sub01 @0x02284 (`Ck(3,0x74)==1 & Ck(4,0x0F)==0 & ...`) Set(4,0x0F)=1 + Evt_exec sub08; main00 20x 0x16 (`Ck(3,0x7D)==1`) | (4,0x0F) |
| E9 | 1051 -> 10A1 -> 1181 -> 11B1 | Garage: **Pliers** nehmen; danach erscheint **Sherry** ("Hey! What are you doing in a place like this?" ... "wait for me quietly in that car over there") | 11B1 Item @0x00FE4 0x30 (Taken (9,0x0A)); sub01 @0x0116E (`Ck(3,0x84)==0 & Ck(9,0x0A)==1`) -> sub02 msg 0-8, @0x011EC Set(3,0x84)=1; @0x01186 Set(3,0x7F)=1 (Pliers-Flag) | (9,0x0A),(3,0x7F),(3,0x84). Werkstatt-Tuer Slot 5 -> 1261 ist Text "The door is locked." msg 12 solange (3,0x83)==0 (sub00 @0x0112A/@0x0113E) |
| E10 | 10A1 -> 11E1 | B2-Korridor: Draht-Sperre mit der Pliers durchtrennen (msg 10/11/12 ueber eine Cursor-Minispiel-Sequenz, Cut 14), Gitter faehrt hoch | sub00 @0x01798 (`Ck(3,0x7F)==0` -> nur Text) / @0x017CC (`Ck(4,0x24)==0` -> Slot 6 sub26 @0x020AC-@0x020D4 -> Cursor sub21..25) -> sub21 @0x01FC6 Set(4,0x24)=1; danach Obj 0 y=-6550 @0x01836 | (4,0x24) |
| E11 | 11E1 | Tuer **"Prisons"**: Kartenleser mit Yellow Keycard + Code **0513** (Roys Zettel) | main00 @0x0157A msg 6, Slot 4 -> sub20 (msg 7/8/9 Yellow Keycard, Taken-Bit (9,0x8A)); sub01 @0x01A72-@0x01A8A `Ck(5,0x0D..0x10)` -> sub19 + Set(3,0x8B)=1; Ziffern: sub15="0", sub10="5", sub06="1", sub08="3" (Tastenzuordnung an 10D0 "4312" und 1230 "5632" verifiziert); Tuer Slot 5 inert -> Slot 3 scharf @0x016C4 (`!(Ck(3,0x8B)==0)`) | (3,0x8B) |
| E12 | 1211 | Zelle: solange (4,0xEC)==0 Text "It's locked. It seems that there is someone inside..."; mit Prison key: **John/Kendo** wird befreit (msg 0-9), rennt zum Verhoerraum | sub00 @0x01DEE / @0x01E36 Spawn 0x49 (-26225,0,-25100) (`(4,0xEC)==1 & (4,0xFC)==0`), AOT Slot 6 -> sub02: @0x01F76 Set(4,0xFC)=1, @0x01F7A (4,0xFA)=0, @0x01F7E (3,0x78)=0, **@0x01F82 (4,0x60)=1**, @0x01F86 (3,0xC9)=1, @0x01F8A (3,0x2C)=1, @0x01F8E (3,0xBA)=1 | (4,0xFC),(4,0x60),(3,0x2C) -> 10 Gefangenen-Zombies 0x1A (sub00 @0x01E86ff) erst jetzt; (3,0xBA) -> 2 Zombies in 1011 |
| E13 | 1021 | **John erschiesst den Zombie Roy** (msg 0-8), Leiche Obj 6 (-4488,0,-10684), "He's dead. Poor Roy..." | main00 @0x01DE2 AOT Slot 5 + @0x01DF6/@0x01E0A Spawns (`Ck(4,0x60)==1`), Evt_exec sub03 @0x01E1E; sub02 @0x01FD8 Set(4,0x60)=0, @0x01FDC (3,0xC9)=0, **@0x01FE0 (4,0xF0)=1**, @0x01FE4 (3,0x5F)=1; Leiche main00 @0x01E40 (`Ck(3,0x5F)==1`) + Slot 6 -> sub06 msg 9 | (4,0xF0),(3,0x5F) |
| E14 | 1041 -> 1061 -> 1121 -> 1131 | Treppe nach 3F (ohne Strom; Fahrstuhl 1081 braucht (4,0xF3)) | 1041 Slot 2 -> 1061 @0x010D8; 1061 Slot 0 -> 1121 @0x00D12; 1121 Slot 2 -> 1131 @0x00CD8 | — |
| E15 | 1151 | (optional) Chief Office: Irons auf der Couch; Examine -> "Irons: I'll be fine. Just worry about yourself for now." (einmalig). **Nutzer-Vorgabe Spur L: Irons tot, "He has already passed..."** — mechanisch haengt nichts daran | main00 @0x00E6A Spawn 0x45; Slot 2 sce5 @0x00D92; sub01 @0x00E9E-@0x00EAC (`Cmp(var00==2) & Ck(3,0x9D)==0`) -> sub03 msg 2 | (3,0x9D) (kein Leser) |
| E16 | 1141 | Briefing Room: **John warnt** "Don't go to the rooftop! ...dangerous monster!"; 2 Gorillas 0x27 brechen durch die Fenster; "John: Elza, I'll try to distract them, while you try to get out of here, hurry!" | sub00 @0x00B02/@0x00B06 (`Ck(4,0xF0)==1`) -> sub02 msg 0-11; @0x00C18 Set(4,0xF0)=0, @0x00C1C (4,0xF2)=1, **@0x00C20 (3,0xC2)=1**; Schloss zum Heliport: Slot 2 -> sub05 @0x00FD2 Set(4,0xC3)=1 "You've opened the lock." (Tuer Slot 1 -> 1171 scharf @0x00AB2) | (3,0xC2),(4,0xF2),(4,0xC3) |
| E17 | 1171 | Heliport: **John** "Elza, hurry up! We need to get this thing off of here! Help me out!" (gemeinsames Schieben sub03/sub04) / "Alright, let's get down from here!" -> Abstieg nach vorn frei | sub07 @0x016C8 Spawn 0x49 (`Ck(3,0xC2)==1`) + @0x01704 Evt_exec sub02: @0x01398 Set(3,0xC2)=0, @0x0139C (3,0x80)=1, **@0x013A0 (4,0xE9)=1** (Tuer Slot 2 -> 10B1 @0x0126C), @0x013A4 (3,0x82)=1 | (4,0xE9),(3,0x80),(3,0x82) |
| E18 | 10B1 | Vorhof: "Elza: John, wait! There's still a little girl trapped in there! I found her in the basement garage!" / "John: ...we can go through the sewer tunnels. Follow me." -> Kanaldeckel (Tuer Slot 1 -> 1261) | sub00 @0x017BA (`Ck(3,0x80)==1`) sub02; sub01 @0x01824 (`Ck(3,0x82)==1 & var0A==7`) -> sub04 msg 0-5: @0x0195A Set(3,0x82)=0, @0x0195E (3,0x83)=1, **@0x01962 (3,0x41)=1**; Tuer @0x016F2 | (3,0x83) (Garagen-Werkstatttuer 11B1 nun offen), (3,0x41) |
| E19 | 1261 | Sewer Passage: John je Eintrittscut | sub03 @0x01276/@0x01296/@0x012B2 (`Ck(3,0x41)==1`) | — |
| E20 | 2001 | L Tunnel: "John: Elza, over here!" / "Through this staircase you can access the basement garage." / "Elza: Okay, I'll go for her." / "John: Meanwhile I will take a look around" | sub00 @0x0179E (`Ck(3,0x41)==1`) -> sub02 @0x0185E Set(3,0x41)=0, @0x01862 (3,0x42)=1; sub04 @0x01954 (3,0x42)=0, **@0x01958 (3,0x46)=1** | (3,0x46) |
| E21 | 1261 -> 11B1 | Garage: Sherry ist weg — "Elza: Sherry!?" / "Where has she gone!?" | sub01 @0x01196 (`Ck(3,0x46)==1`) -> sub03 @0x01438 Set(3,0x46)=0, **@0x0143C (3,0x44)=1** | (3,0x44) |
| E22 | 2001 | Sherry gefunden: "Sherry... thank god you're fine!" / "Sherry: It was because of my daddy, I've heard him call me!" / Taschenuhr / "Come on Sherry, let's look for John and get out of here!" | sub00 @0x0180A Spawn 0x4B (`Ck(3,0x44)==1`) -> sub05 msg 6-14: @0x01A8A Set(3,0x44)=0, @0x01A8E (3,0x45)=1, **@0x01A92 (3,0x4A)=1** | (3,0x45),(3,0x4A) -> Sherry wartet in 2011 (sub00 @0x008C0) |

STAGE1-Nebenwege (nicht auf dem Pflichtpfad, in beiden Varianten gleich): Minidisc Player (1201) -> Stimmschloss
1101 (3,0x67)/(4,0xE8) -> Beweisraum 1111 (Blue Keycard, Zettel 4312, Time Bomb) -> Kartenleser 10D1 (3,0x32) ->
Funkraum 10F1 ("A large communications device. It's out of commission."); Kesselraum 11F1 Reserve-Strom
(sub18 -> (4,0xF3)=1: Fahrstuhl 1081, Tueren 10A1/1181/1191/11B1/11D1/1231 wechseln auf die 1230/11D0-Ziele);
Schiessstand 1191 (Red Keycard) -> Waffenkammer 1231 (Zettel 5632, (3,0x89)). Elzas 10D1 hat KEINE Marvin-Szene
(main00 ohne Spawn; `zensus.md` ROOM10D).

## STAGE2 — Kanalisation (Raeume bis auf 2001/2011 bytegleich mit Leon)

| # | Raum | Ereignis | Beleg |
|---|---|---|---|
| E23 | 2011 | Break Room: Crank (sub02 (3,0x72)); Sherry wartet hier | sub00 @0x008C0 (`Ck(3,0x4A)==1`) |
| E24 | 2021-20B1 | Zombie Tunnel, Treatment Pool (SPAS-12 2031 @0x01BD6), Sealed Tunnel (Head of Akuma 2041 @0x01364), Septic Pool, Engine Room, Flood, Control Room, Alligator Pool (Pocket Watch 2091 @0x00A12; Boss), Service Tunnel | bytegleiche RDTs (`zensus.md`) |
| E25 | 11A1 | Sewer Exit: John + Sherry stehen am Ausgang | sub00 @0x0118C/@0x011A0 (`Ck(4,0xC6)==0`); Tuer Slot 1 -> 3001 @0x00F7C |

## STAGE3 — Fabrik

| # | Raum | Ereignis | Beleg |
|---|---|---|---|
| E26 | 3001 | Fact. Entrance: Leichen der RPD-Truppe ("John: These corpses are definitively police officers." / "This used to be a chemical plant") | sub00 @0x0156C/@0x01580; sub03 @0x016A6 Set(4,0xC6)=1, @0x016AA (4,0xC7)=1, @0x016AE (4,0xF4)=1, @0x016B2 (3,0xA0)=1 |
| E27 | 3011 / 3021 / 3061 | John + Sherry folgen (Download Area, Parking Area "It's an open window. I could boost Sherry in there." msg 2, Backyard: "Sherry: I'll unlock the back door from the other side!" msg 7/8) | 3011 @0x02212/@0x02226, 3021 @0x00BB8/@0x00BCC (`Ck(4,0xC7)==1`), 3061 @0x00F44/@0x00F58 (`Ck(4,0xF4)==1`), sub02 @0x01026 Set(4,0xF4)=0 |
| E28 | 3071 | Warehouse: Lift-Szene mit Birkin (0x30 @0x03434) — "Sherry: I know how to make it work!" / "John: Let's get this lift moving and go down there!" | sub00 @0x034AE/@0x034C2 (`(4,0x4B)==1 & (4,0xEF)==1 & (3,0xB4)==1`), sub11 msg 5-14 |
| E29 | 3081 | Warehouse Lift: Fahrt mit John + Sherry | sub00 @0x007A0/@0x007B4 (`Ck(4,0xFD)==0`), sub02 @0x007DA Set(4,0xFD)=1 |
| E30 | 3091 | T Corridor: Gorilla-Kampf, Latch Key (msg 31/32), dann msg 9-30 ("John: Sherry, huh? I'm John... I'll keep you safe"; "Sherry: This is... where my dad used to be..."; "Elza: Let me deal with it. John, take Sherry somewhere safe.") | sub00 @0x014E0/@0x014F4 (`Ck(4,0x10)==0`); sub07 @0x016D0 Set(4,0x10)=1, **@0x016D4 (3,0x39)=1** |
| E31 | 30C1 | Lobby: **Marvin** ("I'm Marvin Branagh, RPD. I'm here with Leon Kennedy." / "Elza: Leon?! How is he? We got separated back at the RPD." / "...helping me and Ada escape the city" / "Will you tell him that me and John and Sherry are here") | sub00 @0x00BEE (`Ck(3,0x39)==1`) -> sub03 msg 1-8; @0x00C2C Set(3,0x39)=0, @0x00C30 (3,0x47)=1 |
| E32 | 30E1 | Forklift-Halle: **John gebissen** ("one of those monsters bit me as I tried to get away!"), Birkin-Sichtung ("That can't be... human, can it...?") | sub03 @0x00EAC/@0x00EC0 (`Ck(3,0xB5)==1`), sub02 msg 0-17 |

## STAGE4/5 — Labor

| # | Raum | Ereignis | Beleg |
|---|---|---|---|
| E33 | 4001 | Lab Entrance: "Sherry: This is dad's research center... I'll go look for him!" — Sherry rennt weg; 2+2 Spinnenmaenner 0x2B | sub00 @0x01194/@0x011A8 (`Ck(4,0x14)==0`); sub10 msg 9-12, @0x01866 Set(4,0x14)=1 |
| E34 | 4031 | Main Shaft: "John: Sherry! Where did she go!?" / "Elza: Let's split up" | main00 @0x00556 (`Ck(3,0x58)==0`); sub08 @0x0075C Set(3,0x58)=1 |
| E35 | 5021 | Cargo Storage: "Sherry must have crawled through here, but the hole is too small for me." | sub00 @0x01316 (`Ck(4,0xC0)==0`); sub02 @0x01352 Set(4,0xC0)=1, @0x01356 (4,0xED)=1, @0x0135A (3,0x3A)=1, @0x0135E (3,0x2F)=1 |
| E36 | 5031 | Monitor Room: Sherry + Affe 0x29 ("Elza: Sherry!") | sub00 @0x010B6/@0x010CA (`Ck(3,0x2F)==0`); sub03 @0x01134 Set(3,0x2F)=1 |
| E37 | 5011 | Security Room: Sherry infiziert ("She's been infected by what they were working on here" / "John: ...Please save this girl, even if it's just her.") | main00 @0x004E0/@0x004F4 (`Ck(3,0x3A)==1`); sub02 msg 0-16, @0x0055A Set(4,0xED)=0, @0x0055E (3,0x4F)=1 |
| E38 | 5061 | P-4 Laboratory: **G-Vaccine** (Leon: T-Vaccine) | sub06 @0x030E2 Set(4,0xF8)=1; msg 13 |
| E39 | 5011 | Vaccine verabreicht, Sherry erwacht; Durchsage: Kraftwerk zerstoert, Feuer -> "we need to hurry to the shelter!" / "Sherry: The second shelter should be open." | sub00 @0x0054A (`Ck(4,0xF8)==1`) -> sub04 msg 17-37; @0x00948 Set(4,0xF8)=0, @0x0094C **(3,0x4C)=1** |
| E40 | 50B1 | C-2 Hangar: **Adas Leiche** ("It's Ada. She's dead. It looks like one of those gorillas killed her.") | sub00 @0x004F2 Obj 0 (-11800,0,-11600); Slot 2 -> sub02 msg 0 |
| E41 | 50F1 | B4 Power Room: Birkin (Leon: 50E0) | sub00 @0x009AC/@0x009D4 |

## STAGE6 — Zug

| # | Raum | Ereignis | Beleg |
|---|---|---|---|
| E42 | 6021 | Shelter: John + Sherry | sub00 @0x00B96/@0x00BAA (`Ck(3,0x4C)==1`) |
| E43 | 6031 | Train Main Car: Marvin (liegend), John, Sherry; "Elza: Where's Leon?" / "Sherry: He's fighting off monsters. He wants you to start the train." / Sherry startet den Zug, Batterien leer ("Elza: Let me take care of that.") | sub06 @0x00E42/@0x00E56/@0x00E6A; sub07 @0x00EFA/@0x00F1E Sherry am Pult (`Ck(4,0x17)==0`), sub08 @0x00F5C Set(4,0x17)=1; msg 0-10 |
| E44 | 6041 | Ending (bytegleich) | — |

## Zusammenfassung der Elza-Flag-Kette (STAGE1)
(4,0xF1)/(4,0xFA)/(3,0x74) [Roy] -> (4,0xEC) [Prison key] + Yellow Keycard (9,0x8A) + Code 0513 -> (3,0x8B)
[Prisons-Tuer] -> (4,0xFC)/(4,0x60) [John frei] -> (4,0xF0) [Roy erschossen] -> (3,0xC2) [Briefing] -> (4,0xE9)/
(3,0x82) [Dach] -> (3,0x41)/(3,0x83) [Vorhof -> Kanal] -> (3,0x46) -> (3,0x44) -> (3,0x4A) [Sherry]. Parallel:
(9,0x0A)/(3,0x7F) [Pliers] -> (4,0x24) [Draht-Gitter 11E1] -> (3,0x84) [Sherry in der Garage].
