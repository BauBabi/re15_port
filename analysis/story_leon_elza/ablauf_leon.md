# Ablauf Leon (ROOMxxx0) — Hauptweg aus Tuer-Sperren, Schluesseln und Flag-Ketten

Stand 2026-10-10. Belegform wie `ablauf_elza.md` (Raum + Region@Datei-Offset, `msg N` aus `texte_leon.md`, Flags
`(Bank,Bit)`). **[PORT]** = Ergaenzung nur im Port (Runde 34 Nacht / 35 / 36; Dossiers `analysis/befunde_runde34_nacht/`,
`analysis/befunde_runde35/K_cut10f0.md`, `L_cut1150.md`, `analysis/befunde_runde36/AUFTRAG.md` + `VERTRAG.md`),
kein Original-Beleg. Figuren-Details: `B_figuren.md`.

Start: PSX.EXE @0x8001d2a8 Raum 0x17 = ROOM1170 (Heliport), davor Vorspann ROOM1240 (Tuer @0x0051A -> ROOM1170).

## STAGE1 — Polizeirevier (Original)

| # | Raum | Ereignis | Beleg | Setzt / braucht |
|---|---|---|---|---|
| L1 | 1240 | Vorspann-Text (msg 0-5); **setzt (3,0x8B)=1 = die B2-Tuer "Prisons" (11E0 Slot 3 -> 1210) ist fuer Leon von Anfang an offen** (Elzas Vorspann setzt stattdessen (3,0x6F)/(3,0x70) = Schiessstand-Zielscheiben) | sub02 @0x0055A `22 03 8b 01`; Leser ROOM11E0 main00 @0x01576/@0x016C4 | (3,0x8B) — Leon braucht keine Yellow Keycard; die Tuer wurde "schon aufgeschlossen" (Zeitleiste: Elza vor Leon in B2) |
| L2 | 1170 | Prolog msg 8-11 ("We barricaded ourselves...") beim ersten Eintritt (Cut 0) | main00 @0x012CC (`var0A==0 & (3,0xC1)==1 & (3,0x7D)==0`) -> sub11 @0x0169A Set(3,0x7D)=1, @0x0169E (4,0xF2)=1 | (3,0x7D),(4,0xF2) |
| L3 | 1170 | **Elliot + Pilot** fliegen ohne Leon ab (msg 0-7: "Elliot: There is no one left, we must go!" / "Leon: Hey! Hey wait! Down here!" / "Now what am I gonna do?") | sub15 @0x017D2 Spawn 0x47 (`Ck(4,0xF2)==1`) + @0x0186E Evt_exec sub02; sub02 @0x0142C Set(4,0xF2)=0 | — |
| L4 | 1170 | Leon in Cut 11: "Damn it, there must be another way out of here." / "If only I could contact the outside." / **"That's right! I have to report this to the chief."** Abstieg zum Vorhof (Slot 1 -> 10B0) ist **inert** (sce 0, nie scharf) | sub00 @0x0141E (`var0A==11 & Ck(3,0x3E)==0`) -> sub14 msg 13-16, @0x01738 Set(3,0x3E)=1; main00 @0x01226 `3b 01 00 ...` | Leon kann das Dach nur ueber 1130 verlassen |
| L5 | 1130 -> 1150 | **Irons** auf der Couch (gebissen): "Leon: Sir... the helicopter is gone..." / "Irons: Calm down Rookie, there must be a way." / "Maybe using the communication system... you can ask for outside help." / "Leon: But the system is damaged." / "Irons: ...some officers were trying to repair it" / "And remember this Leon, never let your guard down... don't make my mistake..." | ROOM1150 main00 @0x00E88 Spawn 0x45; Slot 6 auto @0x00DEA (`Ck(3,0x94)==0`) -> sub08 msg 4-14, @0x01110 Set(3,0x94)=1. msg 11 "Irons: Take this card..." UNBENUTZT | (3,0x94) |
| L6 | 1130 -> 1120 -> 1060 -> 1040 -> 1030 -> 1050 | Abstieg ueber die Treppen (Fahrstuhl 1080 ohne Strom) | 1130 Slot 1 -> 1120 @0x008AE; 1120 Slot 0 -> 1060 @0x00C76; 1060 Slot 2 -> 1040 @0x00D52; 1040 Slot 1 -> 1030 @0x010B8; 1030 Slot 0 -> 1050 @0x01C6A | — |
| L7 | 1050 | Tote Polizisten-Leiche vor dem Rolltor (msg 5, bei Leon ohne Waffe-Item); Rolltor-Schalter (3,0x79); "I need a fuse to run the shutter." (msg 2) | sub00 @0x00C22 Slot 7 -> sub02; Text msg 2 | (3,0x79) |
| L8 | 1050 -> 1090 | Rear Exterior: **Frau (Ada) ruft aus dem brennenden Schuppen** ("Woman: Anyone! Can someone please get me out of here!?" / "Leon: Hey! Who's that?!"); 7 Feuer (0x26); "I must hurry up and get something to put out this fire to save that woman!" | sub01 @0x02406 (`Ck(3,0x80)==0 & var0A==13`) -> sub02 msg 0-2, @0x0240A Set(3,0x80)=1; sub00 @0x02214-@0x0228C Feuer (`Ck(3,0x81)==0`); Slot 2 Text (`Ck(3,0x85)==0`) | braucht Feuerloescher |
| L9 | 1000 | Bath-Lockers: **Fire Extinguisher** (nur Leon; Elza: 30 Patronen) | sub00 @0x00C24 Item 0x31 (Taken (9,0x86)); sub01 @0x00D0C Set(3,0x85)=1 | (3,0x85) |
| L10 | 1090 | Feuer loeschen -> **Ada gerettet** ("Woman: You saved me... Thank you" / "Leon: Besides, you need to get that wound treated, right? Let's go.") | Slot 2 -> sub06 msg 8/9, @0x0271E Set(3,0x81)=1, @0x02722 (3,0x84)=1 -> sub00 @0x022CC Spawn 0x42 + sub03 msg 3-6: @0x024D2 **(3,0xBB)=1** (4 Zombies @0x023A0ff), @0x024D6 **(3,0x6E)=1** | (3,0xBB),(3,0x6E) |
| L11 | 1050 | Ada geht voraus: "Leon: Hey, wait!" (msg 6/7 UNBENUTZT: "Ada: Give me a moment, I need a break." / "Leon: Relax, I'll take you to the medical room.") | sub00 @0x00C8E Spawn 0x42 (`Ck(3,0x6E)==1`) -> sub03 @0x00D88 Set(3,0x6E)=0, msg 8 | — |
| L12 | 10A0 -> 11E0 -> 1200 | B2: Draht-Gitter mit der **Pliers** (aus 11B0 @0x00FF2, Taken (9,0x0A) -> (3,0x7F)) durchtrennen (11E0 sub00 (4,0x24)); Leichenhalle: **Minidisc Player w/ Disc** | 11E0 wie 11E1 (bytegleich bis auf nichts); 1200 sub00 @0x00932 Item 0x44 (Taken (9,0x68)), sub01 @0x0095A Set(3,0x67)=1 | (3,0x7F),(4,0x24),(3,0x67) |
| L13 | 10D0 -> 1100 -> 1110 | Beweisflur: Stimmschloss mit dem Minidisc Player ("You've used the Minidisc Player w/ Disc." / "You've opened the lock."); Beweisraum: **Blue Keycard**, Zettel "4312" an der Polizisten-Leiche, Time Bomb, MC51 | 1100 sub00 @0x00B14 Slot 2 -> sub07 (`(3,0x67)==1 & (4,0xE8)==0`), sub02 @0x00C68 Set(4,0xE8)=1 + Tuer Slot 1 scharf @0x00CC8; 1110 Item @0x00B2E 0x38 (Taken (9,0x34)), msg 0 | (4,0xE8), Blue Keycard, Code 4312 |
| L14 | 10D0 | Funkraum-Tuer: Kartenleser Blue Keycard + **4312** -> "You've opened the lock." | main00 @0x0103E Slot 1 -> sub20 (msg 7/9, `Ck(9,0x34)==1`); sub01 @0x0152A Set(3,0x32)=1 (`Ck(5,0x0D..0x10)`); Ziffern sub09="4", sub08="3", sub06="1", sub07="2"; Tuer Slot 0 -> 10F0 @0x01174 | (3,0x32) |
| L15 | 10D0 | **Marvin** am Rolltor-Schalter: "???: Freeze!" / "Leon: Wait don't shoot!" / "Marvin: Leon... You're still alive!" / "Leon: ...the chief is injured, one of those things bit him." / "Leon: The chief told me about using the communication system" / "Marvin: Okay, I'll meet with the chief first and we'll start looking for a way to fix it." / "Leon: I agree, he's in his office." | sub00 @0x01242 Slot 18 -> sub21 + @0x01256 Spawn 0x40 (`Ck(4,0xF7)==0`); sub21 msg 11-27, @0x019EC Set(4,0xF7)=1 | (4,0xF7) |
| L16 | 10F0 | Funkraum: "A large communications device. It's out of commission. I can't send any messages from here." (msg 0); kleines Buero "It's locked." (msg 5); Remington M870, Munition, Spray; Tuer Slot 1 -> 1090 | main00 @0x0103E Item 0x08; Tuer @0x00F52 | — |
| L17 | 11F0 | Kesselraum: Reserve-Strom-Schalter (10 Schalter, Loesung sub18) -> **Strom an** -> Fahrstuhl 1080, Tueren 10A0/1160/1180/1190/11B0/11D0/1230 wechseln auf die zweiten Ziele | sub01 @0x012E6 (`(5,0x0D)=1,(5,0x0E)=0,(5,0x0F)=1,(5,0x10)=0,(5,0x11)=1,(5,0x12)=0,(5,0x13)=1,(5,0x14)=0,(5,0x15)=1,(5,0x16)=0`) -> sub18 @0x016F6 Set(4,0xF3)=1, @0x012EA (4,0xEE)=1 | (4,0xF3) |
| L18 | 1180 -> 11B0 | Garage (mit Strom): **das Maedchen (Sherry) rennt vor Leon weg** in den Schacht ("Leon: H-hey!"); vorher "It's too dark to see anything... I must find way to activate the energy." (msg 15, `Ck(4,0xF3)==0`) | main00 @0x00F56 Set(4,0x06)=1 (`Ck(4,0xF3)==1`); sub01 @0x011BA (`Ck(3,0x82)==0 & Ck(4,0x06)==1`) -> sub04 @0x012A6 Set(3,0x82)=1, NPC-Slot 2 (0x4B) @0x01324 Pos_set, msg 0 | (3,0x82) |
| L19 | 11C0 | Parkplatz: **Ada** ("Leon: Ada! Where's Marvin? Wasn't he with you?" / "Ada: He turned around all of a sudden to go look for you." / "There was this really loud noise from upstairs earlier." / "Leon: Ada, you hide inside that patrol car."); 2 Gorillas 0x27; danach "Ada: Okay, it's over now." | sub00 @0x01770 Spawn 0x42 (`Ck(4,0x40)==0`), sub01 @0x01824 -> sub02 @0x0184E Set(4,0x40)=1; sub01 @0x01842 (`(3,0x43)==0 & (7,0x60)==1 & (7,0x61)==1`) -> sub03 @0x01B02 **Set(3,0x43)=1** | (4,0x40),(3,0x43) |
| L20 | 11B0 | **Marvin + Ada + Leon am Schacht**: "Marvin: Leon, you're okay! What was the commotion up there?" / "Leon: Those things broke through the shutters and started getting inside." / "Marvin: What about the chief?" / "Oh, I see... Damn it!" / "We can't get out using the patrol cars anymore." / "Leon: There's a manhole here." / "Marvin: Leon! give me a hand!" / "Leon: Marvin, I'll climb down first, you help her get down!" | sub01 @0x011D2 (`Ck(3,0x83)==0 & Ck(3,0x43)==1`) -> sub06 msg 1-12: @0x01478 Set(3,0x83)=1, **@0x0147C (4,0xCD)=1** (Schacht-Tuer Slot 5 scharf @0x0172E), @0x01480 (4,0xF4)=1 | (4,0xCD),(4,0xF4) -> STAGE2 |

Mechanisch zwingend fuer das Verlassen von STAGE1 (Original): nur L19 -> L20 ((3,0x43) -> (4,0xCD)); Ada steht in
11C0 unabhaengig von der Rettung in 1090 (sub00 @0x01770 prueft nur (4,0x40)). Alles andere (Irons, Funkraum,
Rettung) ist Story-Weg ohne Tuer-Sperre — darum setzt der Port die Sperren unten.

Nebenwege (beide Varianten): Schiessstand 1190 (Red Keycard (4,0xEA)) -> Waffenkammer 1230 (Zettel 5632,
(3,0x89)); 1140 Briefing Room (Leon: 5 Zombies durch die Fenster, sub00 @0x00BAA ff mit Verhalten 0x86/0x88, Ausloeser
Slot 6 (3,0xD2); Schloss zum Heliport (4,0xC3) @0x00C90; "The switch is not responding..." msg 5); 1210 Detention
Area (10 Gefangenen-Zombies, Zelle leer, kein Text).

## STAGE1 — Port-Ergaenzungen [PORT] (in Reihenfolge des Hauptwegs)

| # | Raum | Ergaenzung | Quelle |
|---|---|---|---|
| P1 | 1000 / 1020 / 1010 / 1050 | Dokumente: "Elliot's Diary" (1000, Bank: Elliot sabotierte den Funk und zog die Sicherung), "Marvin's Notes" (1020, Marvins Schreibtisch: Code 4312), "Armory Notice" (1010: Code 5632), "Police Officer's Final Diary Entry" (1050-Leiche: Sicherung beim Chief versteckt). Installiert in beiden Varianten (`dokumente_r34.c:57` room_id & 0xFFFE) | Runde 34 Nacht AUFTRAG.md Z.20-64, `dokumente_r34.c` |
| P2 | 1050 | Rolltor nur mit **Sicherung** (Nahansicht Cut 7/8), Sicherung im Hebetisch-Versteck in 1150 (Cursor-Bedienung) | Runde 34 Nacht A_rolltor.md / B_hebetisch.md, `rolltor_1050.c`, `irons_tisch_1150.c` (1150 und 1151) |
| P3 | 1050 -> 10A0 | **Ada-Ruf**: an der Tuer zum Treppenhaus Szene "Woman: Hello? Anyone? Please, get me out of here!" / "Leon: Another civilian survivor. I have to help her!"; Tuer gesperrt bis Ada gerettet ((3,0xBB)) -> Reihenfolge L8-L10 VOR dem Keller erzwungen | Runde 34 Nacht D_adaruf.md, `adaruf_1050.c` |
| P4 | 1060 -> 1040 | Tuer gesperrt mit Text "I have to get the Chief first..." solange (9,73)=0 und ((9,71)=1 oder (3,94)=0) -> L5 vor dem Abstieg erzwungen; nach der 10F0-Szene zurueck zum Chief erzwungen | Runde 35 L_cut1150.md §2.1, `tuer1060_1040.c` |
| P5 | 10F0 | **Neue Szene beim ersten Betreten**: Ada an den Monitoren ("Did you really think there was only one staff card for the Communication Room?" / "the communication system is completely destroyed"), Marvin kommt herein, Vorstellung ("...Ada, Ada Wong"), Leon: "The patrol car! We can use it to get out of here!" / "Okay, Marvin, you go with Ada to the parking lot and wait there. I'm going to get Chief Irons" -> (9,71)=1; Karte markiert 11C0 und 1150; MAIN01 bis 11C0 | Runde 35 K_cut10f0.md, `cut_10f0.c`; Runde 36 J: Kamera bleibt beim Abgang auf Leon |
| P6 | 10F0 | **Parking-Lot-Schluessel** (RE2-Modell) im kleinen Buero; Tuer 11B0 -> 11C0 bleibt verschlossen, bis er gefunden ist (Bit (9,82)); Elza findet an derselben Stelle in 10F1 den **Prison Key** (Bit (9,83)) statt in 1011 | Runde 36 VERTRAG Spur Q |
| P7 | 1150 | **Irons stirbt** (nach P5, Gate (9,71)=1 & (3,94)=1): Todesszene, Tuerknall, (9,73)=1; danach kein Kopf-Tracking mehr | Runde 35 L_cut1150.md §2.2, `irons_tod_1150.c`; Runde 36 K (RE2-Knall-Ton) + J |
| P8 | 1130 / 1040 / 1030 / 11C0 / 1120 | **Knall-Montage** nach dem Tod: Rest-Zombies aus 1140 in 1130 (Cut 0); Knall -> 1040 Rolltor faehrt hoch, 5 Zombies (Cut 1); Rest aus 1070 kriecht in 1030 unter dem Tor durch (Cut 6); Schnitt 11C0 Cut 13: Ada/Marvin "What was this noise?", Marvin rennt zurueck ins Gebaeude, Ada bleibt; 1120 Fenster zerbricht + Kraehe | Runde 35 L_cut1150.md §2.3/2.4, Spur M (Glas); Runde 36 K (erst Knall, dann Spawn, dann Kamera) |
| P9 | 11B0 | Szenen-Erweiterung nach L20: Marvin "Hold on", schliesst die Tuer zum Parkplatz (11C0) und die Werkstatt-Tuer ab (RE2-Kendo-Animation + Ton), "I just want to make sure we don't have any unwanted followers."; beide Tueren danach mit Elzas Text "The door is locked." (ROOM11B1 msg 12) | Runde 36 VERTRAG Spur J (Nachrichten 20-27) |

Port-Zeitleiste STAGE1 damit: L2-L5 (Irons lebt) -> L6-L11 (Ada gerettet, P3) -> L12-L14 (Blue Keycard) -> L15
(Marvin) -> P5 (10F0: Marvin+Ada zum Parkplatz) -> P4/P7 (zurueck zu Irons, er stirbt) -> P8 (Montage: das Revier
faellt) -> L17 (Strom) -> P6 (Parking-Lot-Schluessel) -> L18-L20 + P9 (Schacht).

## STAGE2-6 (Original, Kurzfassung)

| # | Raum | Ereignis | Beleg |
|---|---|---|---|
| L21 | 1260 / 2000 | Ada + Marvin im Kanal; "Leon: There is a little girl that came in here, we have to find her!" / "before meeting Ada I saw her coming in this way" / "Leon: Marvin sees Ada up front." (Trennung) | 1260 sub02 @0x01254ff (`Ck(4,0xF4)==1`); 2000 sub00 @0x017A0/@0x017B4 -> sub02 msg 0-11, @0x017FA Set(4,0xF4)=0 |
| L22 | 2010-20B0 | Kanalisation (bytegleich mit Elza): Crank, SPAS-12, Head of Akuma, Alligator (Pocket Watch) | `zensus.md` |
| L23 | 11A0 | Sewer Exit: Marvin wartet | sub00 @0x0118C (`Ck(4,0xC6)==0`) |
| L24 | 3000 | Fabrik-Eingang: RPD-Leichen ("Marvin: They must be from the squad investigating on the Umbrella incident." / "Leon: Marvin, what about that woman?" / "Marvin: Ada...? She went on ahead.") | sub00 @0x0156C/@0x0158E; sub03 @0x0166E Set(4,0xC6)=1, @0x01672 (4,0xC7)=1 |
| L25 | 3010 / 3020 / 3060 | Marvin folgt ("Marvin: There's an open window, maybe we could enter through there?" 3020 msg 1); Hunde im Backyard | 3010 @0x02212, 3020 @0x00BF6, 3060 @0x00F58 (`Ck(4,0xC7)==1`) |
| L26 | 3070 / 3080 | Warehouse-Lift mit Birkin (0x30 @0x033CE): "Leon: Get on the lift! It's our only option..." / "Ada: Underground..."; Lift: Birkin-Oberkoerper 0x36 ("Ada: You just don't give up...") | 3070 sub00 @0x0343C/@0x03450 (`(4,0xD7)==1 & (4,0xEC)==1 & (3,0xB4)==1`); 3080 @0x007AC/@0x007C0/@0x007D4 (`Ck(4,0x60)==0`) |
| L27 | 30E0 | Forklift-Halle: Ada ("Where's that other guy?" / "That glass won't hold up"); **Marvin gebissen** ("The hall is no good...! Those things broke through the main gate") | sub11 @0x01068/@0x0108C/@0x010A0; msg 0-13 |
| L28 | 4000 | Lab Entrance: Marvin wird getragen ("Ada: Let's get him to this room here!" / "Leon: Go on ahead! I'll take care of this!" / "Take care of Marvin!"); Affen 0x29 | sub00 @0x012A2ff (`Ck(4,0x26)==0`); sub05 @0x01550 Set(4,0x26)=1, @0x01554 (4,0x14)=1 |
| L29 | 4010 | Security Room: **"Ada: That's right. I'm with Umbrella."**; Virus/Vaccine; "Ada: Leon, I can't help you. There's something I have to do." / "When all this is over, head to the underground shelter!"; Blue Master Keycard | main00 @0x00664/@0x00678 (`Ck(3,0x48)==0`); sub03 msg 0-30; sub04 @0x00DA4 Set(3,0x3F)=1, @0x00DAC (3,0x4C)=1 |
| L30 | 5060 | **T-Vaccine** (Elza: G-Vaccine) | sub06 @0x030E2 Set(4,0xFA)=1; msg 13 |
| L31 | 4010 | Marvin kollabiert / Ada: "The virus is on the loose here as well! Hurry to the shelter!" | sub00 @0x006CE (`Ck(4,0xFA)==1`) -> sub04 msg 32-35, @0x00DB8 |
| L32 | 5090 / 6000 | Annette (Train Car ABC; Gate Platform: "You must be that cop. Your friends are already here." / "Where's Ada?") | 5090 sub00 @0x0122A; 6000 sub00 @0x00F44 (`Ck(3,0x64)==0`) |
| L33 | 50B0 | C-2 Hangar: Ada ("Annette is waiting for us below, and I left Marvin with her" / "Dr. Birkin's wife and my boss" / "He's dead. Umbrella killed him." / Reaktor), **Ada stirbt** (2 Affen) -> "She's dead." | sub00 @0x00512 (`Ck(3,0x62)==0`), sub02 msg 0-16, @0x005A6 Set(3,0x62)=1; Leiche @0x00566 (-11800,0,-11600) |
| L34 | 50E0 | B3 Power Room: Birkin | sub00 @0x00ACE/@0x00AEE |
| L35 | 6020 | Shelter: Marvin + Ada | sub00 @0x00B96/@0x00BAA (`Ck(3,0x4C)==1`) |
| L36 | 6030 | Train Main Car: **John/Kendo, Marvin (liegend), Sherry**; "Leon: Where's Elza?" / "Sherry: She's out there fighting the monsters!" / "John: She told me to stay and keep an eye on Marvin here." / Annette: "Where's that biker girl?" / "Not enough power!" | sub06 @0x0101C/@0x01030/@0x01044; sub07 @0x010D4/@0x010F8 Annette (`Ck(4,0x17)==0`); msg 0-19 |
| L37 | 6040 | Ending | — |

## Zusammenfassung der Leon-Flag-Kette (STAGE1, Original)
(3,0x7D)/(4,0xF2) [Prolog/Elliot] -> (3,0x94) [Irons] -> (3,0x85) [Feuerloescher] -> (3,0x81)/(3,0x84) -> (3,0xBB)/
(3,0x6E) [Ada gerettet] -> (9,0x0A)/(3,0x7F)/(4,0x24) [Pliers/Gitter] -> (3,0x67)/(4,0xE8) [Minidisc] -> (9,0x34)+4312 ->
(3,0x32) [Funkraum] -> (4,0xF7) [Marvin] -> (4,0xEE)/(4,0xF3) [Strom] -> (4,0x06)/(3,0x82) [Sherry im Schacht] ->
(4,0x40) -> (7,0x60)/(7,0x61) -> (3,0x43) [Ada/Gorillas] -> (3,0x83)/(4,0xCD)/(4,0xF4) [Schacht]. Vom Vorspann:
(3,0x8B) [Prisons-Tuer offen].
