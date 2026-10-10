# B — Figuren, Dialoge und Ablauf beider Szenarien (RE1.5, Auslieferungsstand = MZD-Build)

Stand 2026-10-10 (Runde 36, Spur P). NUR ANALYSE, kein Port-Code geaendert. Fortsetzung von `A_zensus.md`
(dort: xxx0 = Leon, xxx1 = Elza, belegt an PSX.EXE @0x800397e4/@0x8001d2a8/@0x8001d324).

Quellen dieses Dossiers (alle im Ordner `analysis/story_leon_elza/`):
`zensus.md` (Raum-Unterschiede), `texte_leon.md` / `texte_elza.md` (alle Nachrichten mit Datei-Offset),
`scd_annot/ROOMxxxx.txt` (annotierte SCD-Dumps; weitere Raeume ad hoc mit `python scd_story.py <raum>`),
`story_view.py <raum>` (Bedingungen je Instruktion aus `census.json`), `flag_xref.txt` (wer setzt/liest ein Flag).
Belegform: `ROOMxxxx region@0xOFFSET` = Datei-Byte-Offset in `re15_port/shared_assets/PSX/STAGEn/ROOMxxxx.RDT`;
`msg N` = Nachrichten-Index (Message_on-Argument) mit Text aus `texte_*.md`; Flags als `(Bank,Bit)`.
Unsichere Schluesse sind als **VERMUTUNG** markiert.

Die Ablauf-Dateien: `ablauf_leon.md` und `ablauf_elza.md` (nummerierte Ereignisse, Port-Ergaenzungen fuer Leon
als eigene, gekennzeichnete Eintraege).

---

## 0. NPC-Typen (Sce_em_set-Typ) -> Figur

Die Zuordnung Typ -> Figur ist aus dem Dialog-Kontext der Szene abgeleitet, in der der Typ gespawnt und per
`Work_set NPC-Slot` animiert wird (nicht aus Modell-/EMD-Namen; die EXE-Namen EM040 "Irons" usw. aus
`RE15_NPC_AI.md` gelten fuer den Capcom-Prototyp, der MZD-Build nutzt dieselben Modelle fuer eigene Figuren).

| Typ | Figur (MZD-Text) | Projekt-Name | Beleg (Spawn + Dialog) |
|---|---|---|---|
| 0x40 | Marvin | Marvin | ROOM10D0 sub00@0x01256 + msg 14 "Marvin: Leon... You're still alive!"; ROOM30C1 sub00@0x00BEE + msg 2 "Officer: I'm Marvin Branagh" |
| 0x42 | Ada ("Woman") | Ada | ROOM1090 sub00@0x022CC + msg 0 "Woman: Anyone!..."; ROOM11C0 sub00@0x01770 + msg 1 "Ada: He turned around..." |
| 0x45 | Chief Irons | Irons | ROOM1150 main00@0x00E88 / ROOM1151 main00@0x00E6A + msg 2 "Irons: I'll be fine..." |
| 0x47 | Polizist-Modell: Elliot (Leon) / Roy (Elza) | Elliot / Roy | ROOM1170 sub15@0x017D2 + msg 0 "Elliot: We must go!"; ROOM1011 sub00@0x009F6 + msg 4 "Officer: Elza, huh? I'm Roy."; ROOM1021 main00@0x01E0A + msg 1 "Roy: John... Help me..." |
| 0x49 | John | **Kendo** (Nutzer-Name) | ROOM1211 sub00@0x01E36 + msg 0 "Elza: Hey! You must be John right?"; 15 Elza-Raeume + ROOM6030 (Leon) |
| 0x4B | Sherry ("Girl") | Sherry | ROOM11B1 main00@0x01044 (geparkt) + msg 3 "Girl: ...Sherry."; ROOM2001 sub00@0x0180A + msg 8 "Sherry: It was because of my daddy" |
| 0x4D | Annette | Annette | ROOM6000 sub00@0x00F44 + msg 0 "Annette: You must be that cop."; ROOM5090, ROOM6030 (nur Leon) |
| 0x30/0x36 | Birkin (Kreatur, Gegner-Typ) | Birkin | ROOM3070/3071, 3080, 50E0, 50F1 — kein Dialog |

Hinweis: Der MZD-Build nennt Elzas Begleiter durchgehend "John" (alle Nachrichten mit Sprecherfarbe 07). Der
Nutzer und der Port nennen ihn "Kendo"; in diesem Dossier steht **John/Kendo**. Ausnahme ROOM3091 (Elza):
dort traegt NPC-Slot 1 den Typ 0x42 (Ada-Modell, main00@0x014F4, gleiche Position wie John @0x014E0) und
wird in sub07 bei allen Sherry-Zeilen (msg 15/20/22/24/28/29) per `Work_set NPC-Slot 1` animiert ->
**VERMUTUNG**: Modell-Platzhalter des Modders fuer Sherry, keine Ada-Begegnung.

---

## 1. Figuren je Szenario

### 1.1 Roy (Typ 0x47, nur bei Elza lebend)

**Elza:**
- ROOM1041 (West Corridor) sub11 (AOT Slot 8 auto, main00@0x01178, Bedingung `Ck(4,0xF1)==0 & Ck(3,0xAD)==0`):
  msg 3 "gunshot" + msg 2 "Elza: What was that!?" -> setzt (3,0xAD)=1 @0x021EA. Ein Schuss faellt, BEVOR Elza
  Roy trifft ((4,0xF1) wird erst von der Roy-Szene gesetzt). **VERMUTUNG**: Roys Schuss auf einen Zombie.
- ROOM1011 (Interrogation Room) sub00@0x009F6: Roy Typ 0x47 Pos (-2000,0,7450), wenn `Ck(4,0xF1)==0 & var0A==0`
  (Eintrittscut 0); sub02 (Evt_exec @0x00A1E) = Dialog msg 0-17: Roy ist verwundet ("Officer: groans"), nennt
  seinen Namen (msg 4), "Those things broke through the rear shutters and got inside" (msg 7), "Those bastards
  at Umbrella caused all of this" (msg 8), "You have to rescue the survivors in the other rooms" (msg 13),
  **"My friend John should still be in the underground cell block. Please, you have to help him"** (msg 15/16),
  "Elza: Roy, wake up! Please wake up!" (msg 17). sub02 setzt (4,0xF1)=1 @0x00AFE, (4,0xFA)=1 @0x00B02,
  (3,0x74)=1 @0x00B06 (-> ROOM1031 sub01: Rolltor-Szene/20 Zombies in der Lobby, s. ablauf_elza.md).
- Danach (Ck(4,0xFA)==1) sub00@0x00A3E Roy erneut gespawnt (bewusstlos), AOT Slot 5 -> sub06: msg 19 "He is
  holding something...", msg 20 **"You've taken the Prison key."** -> (4,0xEC)=1 @0x00E2C, (3,0x78)=1 @0x00E30.
  **Der "Prison key" ist KEIN Inventar-Item** (kein Item_aot_set, kein Item-Index; `item_namen.json` kennt
  ihn nicht) — er ist nur das Flag (4,0xEC). Dann AOT Slot 5 -> sub07: msg 18 "He has passed out from his
  injuries. He is holding a slip. The numbers 0513 are printed on the slip." (nur Elza).
- Item in 1011 nur bei Elza: Yellow Keycard 0x39 (main00@0x00954, Taken-Bit (9,138)); Leon hat an dieser
  Stelle ein First Aid Spray (ROOM1010 sub00@0x00996).
- Code 0513 = Code des Kartenlesers "Prisons" in ROOM11E1 (sub01-Ziffernautomat: Ziffer1 sub15 -> Taste 9 = "0",
  Ziffer2 sub10 -> "5", Ziffer3 sub06 -> "1", Ziffer4 sub08 -> "3"; dieselbe Tastenzuordnung ergibt in ROOM10D0
  "4312" (Zettel ROOM1110 msg 0) und in ROOM1230 "5632" (Zettel ROOM1230 msg 10) — Zuordnung damit verifiziert).
- ROOM1021 (Main Office) main00@0x01E0A: Roy Typ 0x47 Pos (-4614,0,-16919) wenn `Ck(4,0x60)==1` (gesetzt von
  ROOM1211 sub02 @0x01F82 = John befreit). Szene sub02 (AOT Slot 5 @0x01DE2, Evt_exec sub03 @0x01E1E):
  Roy ist zum Zombie geworden: msg 0 "John: No way... Roy, this is a joke, right?", msg 1 "Roy: John... Help me...
  Please kill me.", msg 2 "Elza: John, shoot! That thing isn't Roy anymore!", msg 3 "John: Roy, it's me...";
  Roy (NPC-Slot 1) geht auf John zu (Plc_dest @0x020B2 -> (-4274,-6590)), John (Slot 0) Clip 2/6+2/7 @0x020C6ff
  (= Schuss-Pose, kein Se_on im Sub — der Nutzer hat die fehlenden Schuss-Toene bereits bemerkt, Spur L),
  Roy Clip 1/7 @0x020DE (= Fallen), dann msg 4-8 (Elza: Funk tot, Strom faellt aus; John: "I'd like to be alone
  now"). Setzt (4,0x60)=0, (3,0xC9)=0, **(4,0xF0)=1** @0x01FE0 (-> Briefing-Room-Szene ROOM1141), (3,0x5F)=1
  @0x01FE4 (-> Leiche: main00@0x01E40 Obj 6 Pos (-4488,0,-10684) + AOT Slot 6 -> sub06 msg 9 "He's dead. Poor Roy...").
- **Wer schiesst:** John (NPC-Slot 0 bekommt die Schuss-Clips @0x020C6/@0x020CE/@0x020EA, waehrend Roy Slot 1
  @0x020DE Clip 1/7 faellt). Elza fordert es (msg 2), schiesst aber nicht (Spieler bleibt in Plc_dest-Pose).

**Leon:**
- ROOM1020 main00@0x01DEE: Obj 6 Pos (-4488,0,-10684) **unbedingt** (= exakt die Position der Leiche aus Elzas
  1021-Szene) + AOT Slot 5 @0x01DDA -> sub02 msg 0 **"It's the RPD officer, Roy. He appears to have died
  recently."** Kein lebender Roy bei Leon (ROOM1010 hat keinen NPC-Spawn, kein Text ueber ihn).
- Leon erfaehrt Roys Namen also aus dem Untersuchungstext; eine Vorstellung gibt es nicht (Marvin nennt ihn in
  ROOM10D0 msg 18 indirekt: "You've already seen him, I thought he was the only one left at the station").

### 1.2 John/Kendo (Typ 0x49)

**Elza** (Begleiter durch alle Stages):
- ROOM1211 (Detention Area, B2): Zelle. sub00@0x01DEE: solange `Ck(4,0xEC)==0` (kein Prison key) ist Slot 8 ein
  Text-AOT msg 10 **"It's locked. It seems that there is someone inside..."**. Mit (4,0xEC)==1 und (4,0xFC)==0:
  Spawn John @0x01E36 Pos (-26225,0,-25100) (in der Zelle), AOT Slot 6 -> sub02 (Dialog), Evt_exec sub06.
  Dialog sub02 msg 0-9: "Elza: Hey! You must be John right? Get up now!" / "John: What do you want? I'm trying to
  sleep here!" / "John: Ughh... my head is killing me." / "John: Who the hell are you anyway?" / "Elza: My name's
  Elza. Roy sent me to get you out of here." / "Elza: ...The whole station is crawling with zombies!" / "John: Say
  what!?" / "Elza: Roy was injured by one of them. I left him in the Interrogation Room, but..." / "John: The
  interrogation room? Alright. I'm going there." / "Elza: John. Wait!". sub02 setzt (4,0xFC)=1 @0x01F76 (John
  befreit), (4,0xFA)=0 @0x01F7A + (3,0x78)=0 @0x01F7E (Roy in 1011 nicht mehr untersuchbar), **(4,0x60)=1**
  @0x01F82 (-> ROOM1021-Szene), (3,0xC9)=1, **(3,0x2C)=1** @0x01F8A (-> die 10 Gefangenen-Zombies Typ 0x1A in
  1211 werden erst jetzt gespawnt, sub00 `Ck(3,0x2C)==1`; Leon hat sie in ROOM1210 main00@0x01D86ff von Anfang an),
  (3,0xBA)=1 @0x01F8E (-> 2 Zombies Typ 0x10 in ROOM1011).
- **Warum sitzt er in der Zelle?** Der Text sagt es nicht. Hinweise: "I'm trying to sleep here!" (msg 1), "my head
  is killing me" (msg 2) -> **VERMUTUNG**: Ausnuechterungszelle. Roy nennt ihn "my friend John" (1011 msg 15),
  kein Vorwurf. Wer ihn eingesperrt hat, steht in keinem Text beider Szenarien.
- **Wie kommt er heraus?** Allein ueber das Flag (4,0xEC) ("Prison key" aus 1011 sub06). Die Zellentuer ist kein
  Door_aot_set (Slots 1-5 in 1211 fuehren nach ROOM1221 = Detention Cells, bytegleich in beiden Szenarien);
  John verlaesst die Zelle in der Szene (Plc_dest) und rennt zum Verhoerraum.
- ROOM1021: s. 1.1 (John erschiesst Roy; danach "I'd like to be alone now").
- ROOM1141 (Briefing Room) sub00@0x00B06: John Pos (-6750,0,-13350) wenn `Ck(4,0xF0)==1`; Szene sub02: msg 0-11:
  "Elza: John, wait!" / "John: Elza! sorry, I'm glad to see you're still alive..." / **"John: Don't go to the
  rooftop! ...there's something...a somewhat dangerous monster!"** / Krach / "John: Looks like some kind of
  gorilla, that would explain how they managed to enter through the windows!" (2 Gorillas Typ 0x27 sub00@0x00B1A/
  @0x00B2E) / "John: Elza, I'll try to distract them, while you try to get out of here, hurry!". Setzt (4,0xF0)=0,
  (4,0xF2)=1 @0x00C1C (2 weitere Gorillas bleiben), **(3,0xC2)=1** @0x00C20 (-> Heliport-Szene).
- ROOM1171 (Heliport) sub07@0x016C8: John Pos (1250,-7200,-4900) wenn `Ck(3,0xC2)==1`, Szene sub02: msg 0 "John:
  Elza, hurry up!", msg 1 "John: We need to get this thing off of here! Help me out!" (Evt_exec sub03/sub04 =
  gemeinsames Wegschieben), msg 3 "John: Alright, let's get down from here!". Setzt (3,0xC2)=0, (3,0x80)=1,
  **(4,0xE9)=1** @0x013A0 (-> Tuer Slot 2 nach ROOM10B1 = Abstieg vom Dach), (3,0x82)=1. (Nur 2 Kraehen statt 7.)
- ROOM10B1 (Front Exterior): main00@0x01726 John (geparkt); sub01 `Ck(3,0x82)==1 & var0A==7` -> sub04: **"Elza: John,
  wait! There's still a little girl trapped in there! I found her in the basement garage! We can't just leave
  her behind!"** / "John: ...we can go through the sewer tunnels. Follow me." Setzt (3,0x83)=1, **(3,0x41)=1**
  @0x01962 (-> John in 1261/2001). Tuer Slot 1 @0x016F2 -> ROOM1261 (Kanalschacht im Vorhof).
- ROOM1261 (Sewer Passage) sub03: John je Eintrittscut (var0A 15/8/9) @0x01276/@0x01296/@0x012B2, `Ck(3,0x41)==1`.
- ROOM2001 (L Tunnel) sub00@0x0179E John wenn (3,0x41)==1 -> sub02 msg 0 "John: Elza, over here!" -> sub04:
  "John: Through this staircase you can access the basement garage." / "Elza: Okay, I'll go for her." / "John:
  Meanwhile I will take a look around, maybe she is down here too." Setzt (3,0x42)=0, **(3,0x46)=1** @0x01958
  (-> ROOM11B1 sub03 "Sherry!? Where has she gone!?").
- ROOM11A1 (Sewer Exit) sub00@0x0118C John + @0x011A0 Sherry wenn `Ck(4,0xC6)==0`; ROOM3001 (Fact. Entrance)
  sub00@0x0156C/@0x01580 (Leichen der RPD-Truppe: "John: These corpses are definitively police officers.",
  "This used to be a chemical plant"), setzt (4,0xC6)=1, (4,0xC7)=1 @0x016AA; ROOM3011/3021/3061 mit Sherry.
- ROOM3071 (Warehouse) sub00@0x034AE: Lift-Szene sub11 msg 5-14 ("John: Save the discussion for later! Those things
  are trying to get in here...!", "Sherry: I know how to make it work!"); ROOM3081 (Lift) @0x007B4.
- ROOM3091 (T Corridor) sub00@0x014E0: nach dem Gorilla-Kampf msg 9-30 ("John: That was a close shave!", "John:
  Sherry, huh? I'm John, and you can rest assured that I'll keep you safe from now on!", "Elza: Let me deal with
  it. John, take Sherry somewhere safe."). Setzt (4,0x10)=1, **(3,0x39)=1** @0x016D4 (-> Marvin in ROOM30C1).
- ROOM30E1 (Forklift) sub03@0x00EAC `Ck(3,0xB5)==1`: **"John: This is bad... one of those monsters bit me as I tried
  to get away!"** (msg 2) — Johns Biss ist die Parallele zu Marvins Biss in Leons ROOM30E0 msg 10/11.
- ROOM4001 (Lab Entrance) sub00@0x01194 `Ck(4,0x14)==0`: msg 3-12 ("Sherry: This is dad's research center... I'll go
  look for him!" / "John: Sherry, hold on!" / "John: Elza, let's go after her!"). ROOM4031 (Main Shaft) main00@0x00556
  `Ck(3,0x58)==0`: "John: Sherry! Where did she go!?" / "John: Okay! I'll go this way, you go that way, Elza!".
- ROOM5011 (Security Room B) main00@0x004E0 `Ck(3,0x3A)==1` (gesetzt ROOM5021 sub02): Sherry bewusstlos/infiziert;
  msg 0-16 ("John: If we can't get the vaccine, Sherry will..." / "Elza: Okay. I'll go get it." / "John: ...Please
  save this girl, even if it's just her."); nach dem G-Vaccine (ROOM5061 msg 13, (4,0xF8)) sub04 msg 17-37
  ("Elza: John, I've got the vaccine!" ... "Elza: That's right! We can't stay here, we need to hurry to the
  shelter!" / "Sherry: The second shelter should be open."), setzt (3,0x4C)=1 @0x0094C.
- ROOM6021 (Shelter) sub00@0x00B96 John + Sherry `Ck(3,0x4C)==1`; ROOM6031 (Train Main Car) main/sub06 @0x00E56
  John (-26055,0,-25784) + Marvin (0x40 @0x00E42, liegend y=-920) + Sherry; sub07/sub08 Sherry startet den Zug.

**Leon:** John/Kendo kommt in Leons STAGE1-6 NUR in ROOM6030 (Train Main Car, sub06@0x0101C, Pos (-26055,0,-25784))
vor: msg 3 "John: Yeah! She's trying to keep the outside clear so we can escape!", msg 5 "John: She told me to
stay and keep an eye on Marvin here.", msg 7 "John: Up front, trying to start the train." Leons ROOM1210 hat
keinen NPC, keinen Nachrichtenblock (`texte_leon.md` "ROOM1210 ... _kein Nachrichtenblock_") und die 10
Gefangenen-Zombies von Anfang an (main00@0x01D86..@0x01E3A). **Keine Spur von John in Leons STAGE1** (weder
lebend noch tot).

### 1.3 Chief Irons (Typ 0x45)

**Leon:** ROOM1150 main00@0x00E88 Pos (-21150,-720,-26131) auf der Couch (sub02 Clip 3 Loop). Szene sub08
(AOT Slot 6 auto @0x00DEA, `Ck(3,0x94)==0` @0x00DE6): msg 4-14 — "Leon: Sir... the helicopter is gone..." /
"Irons: Calm down Rookie, there must be a way." / "Irons: Maybe using the communication system... you can ask for
outside help." / "Leon: But the system is damaged." / "Irons: ...some officers were trying to repair it" /
msg 11 (UNBENUTZT) "Irons: Take this card... it will allow you to enter the communications room" / msg 12/13
"And remember this Leon, never let your guard down... uniform or not, those things... are no longer what they
were, don't make my mistake..." / "Leon: Yes sir...". Setzt (3,0x94)=1 @0x01110. (3,0x94) wird spaeter in
ROOM4000/4020/5000/50C0 (beide Varianten) als Teil eines BGM-/Szenengates gelesen (flag_xref 1566-1573), nicht
als Irons-Zustand. Leon nennt Irons' Biss in ROOM10D0 msg 20 ("the chief is injured, one of those things bit
him"); Marvin in ROOM11B0 msg 3/4 "What about the chief?" / "Oh, I see... Damn it!" (= Irons ist tot/verloren;
die Todesszene selbst existiert im Original NICHT, sie ist Port-Ergaenzung, s. ablauf_leon.md).
Zweiter AOT ueber Irons: Slot 2 sce=5 @0x00D92 (Examine -> work_vars[0]=2) -> sub01 `Cmp(var00==2) & Ck(3,0x9D)==0`
-> sub03 msg 2 "Irons: I'll be fine. Just worry about yourself for now." (einmalig, (3,0x9D)).

**Elza:** ROOM1151 main00@0x00E6A: derselbe Irons, dieselbe Couch, derselbe Loop (sub02 @0x00EB8 Clip 3). Es gibt
NUR den Examine-Weg: Slot 2 sce=5 @0x00D92 -> sub01 @0x00E9E `Cmp(var00==2) & Ck(3,0x9D)==0` -> Set (3,0x9D)=1
@0x00EA8 -> sub03 (@0x00EC2-0x00F72): Elza wird vor die Couch gesetzt (Pos_set (-20538,0,-25147)), kniet
(Clip 11), Cut 7, **msg 2 "Irons: I'll be fine. Just worry about yourself for now."**, Irons Clip 6 (2x) und
zurueck in Clip 3, Elza nickt und steht auf. Keine weitere Nachricht, kein Set ausser (3,0x9D) und den
Pausen-Bits (2,7)/(1,27). **(3,0x9D) wird nirgends sonst gelesen** (flag_xref Z.1629-1633: nur 1150/1151 sub01).
Elza erfaehrt im Original also: Irons lebt, ist verletzt ("I'll be fine"), mehr nicht. Keine Tuer, kein Item,
kein Flag eines anderen Raums haengt an dieser Szene.

**Folge der Nutzer-Entscheidung "bei Elza liegt Irons von Anfang an tot" (Spur L):** fuer Elzas Ablauf
aendert sich NICHTS Mechanisches — nur sub01/sub03 von ROOM1151 entfallen und msg 2 wird durch "He has already
passed..." ersetzt. Fuer die Zeitleiste bedeutet es: Elza betritt 1151 NACH Irons' Tod, der in Leons Szenario
(Port-Szene, ROOM1150 Ereignis 21, Bit (9,73)) am Ende von STAGE1 liegt -> Elza ist in 1151 **nach** Leon
(konsistent mit 1091, s. Abschnitt 3).
Port-Hinweis: Das Sicherungs-Versteck im Hebetisch (`irons_tisch_1150.c:20`, 1150 UND 1151) und das Rolltor
1050/1051 mit Sicherung (`rolltor_1050.c`) gelten im Port fuer beide Varianten -> Elza muss 1151 betreten
(toter Irons + Sicherung), die Reihenfolge "Leon vor Elza in 1150/1151" ist damit auch spielmechanisch
anzunehmen.

### 1.4 Ada (Typ 0x42)

**Leon:**
- ROOM1090 (Rear Exterior): sub01 `Ck(3,0x80)==0 & Cmp(var0A==13)` -> sub02 msg 0-2 ("Woman: Anyone! Can someone
  please get me out of here!?" / "Leon: Hey! Who's that?!" / "Woman: ...open the door!"); 7 Feuer-Objekte Typ 0x26
  sub00@0x02214-@0x0228C (`Ck(3,0x81)==0`); Slot 2 -> sub06 msg 7/8 "I must hurry up and get something to put out
  this fire to save that woman!" / "Will you use the Fire Extinguisher?" (Item 0x31 aus ROOM1000 sub00@0x00C24, nur
  Leon) -> (3,0x81)=1, (3,0x84)=1 -> sub03: Ada @0x022CC spawnt, msg 3-6 ("Leon: There! Are you okay?" / "Woman:
  You saved me..." / "Leon: Besides, you need to get that wound treated, right? Let's go."), setzt (3,0xBB)=1
  @0x024D2 (4 Zombies sub00@0x023A0ff) und **(3,0x6E)=1** @0x024D6.
- ROOM1050 (East Corridor) sub00@0x00C8E Ada Pos (18550,0,-22300) wenn `Ck(3,0x6E)==1` -> sub03: Ada geht weg,
  msg 8 "Leon: Hey, wait!" (msg 6/7 "Ada: Give me a moment, I need a break." / "Leon: Relax, I'll take you to the
  medical room." sind UNBENUTZT), setzt (3,0x6E)=0.
- ROOM11C0 (Parking Lot) sub00@0x01770 Ada (-8965,0,-14347) `Ck(4,0x40)==0` -> sub02 msg 0-6: "Leon: Ada! Where's
  Marvin? Wasn't he with you?" / "Ada: He turned around all of a sudden to go look for you." / "Ada: There was this
  really loud noise from upstairs earlier." / "Leon: Ada, you hide inside that patrol car." (2 Gorillas 0x27 @0x01784/
  @0x01798). Nach deren Tod ((7,0x60),(7,0x61)) sub03 msg 7-9 "Ada: Okay, it's over now." / "...outrageous monsters
  out there" / "Leon: Yeah... Anyway, let's get out of here.", setzt **(3,0x43)=1** @0x01B02 (-> ROOM11B0 sub06).
- ROOM11B0: Ada main00@0x01094 (geparkt), Szene sub06 (Marvin/Ada/Leon, Schacht) msg 11 "Leon: Marvin, I'll climb
  down first, you help her get down!".
- ROOM1260 sub02 @0x01254/@0x0128C, ROOM2000 sub00@0x017A0 (`Ck(4,0xF4)==1`): msg 7/8 "Ada: So there is no time to
  lose, we do not know what things are in the sewers..."; Leon: "Marvin sees Ada up front." (Trennung).
- ROOM3000 (Leon) msg 5-8: "Leon: Marvin, what about that woman?" / "Marvin: Ada...? She went on ahead." / "She
  didn't seem to be at all disturbed by all this massacre."
- ROOM3070 sub00@0x0343C, ROOM3080 @0x007AC (Lift mit Birkin: msg 0/1 "Ada: You just don't give up..." / "That's...
  got to be..."), ROOM30E0 sub11 @0x01068/@0x0108C (msg 0-8: "Ada: ...? Where's that other guy?" / "Leon: Ada... What
  have you been doing, anyway!?" / "Ada: That glass won't hold up if they keep pushing at it!").
- ROOM4000 sub00@0x012A2/@0x012E0 ("Ada: Just hurry up, okay?! Let's get him to this room here!" / "Leon: Take care of
  Marvin!"); ROOM4010 main00@0x00664 `Ck(3,0x48)==0`: msg 0-30 **"Ada: That's right. I'm with Umbrella."**, Virus/
  Vaccine-Erklaerung, "Ada: Leon, I can't help you. There's something I have to do." / "Ada: Leon. When all this is
  over, head to the underground shelter!"; sub04 setzt (3,0x3F)=1, (4,0x14)=0, (3,0x4C)=1 @0x00DAC; msg 32-35 ("Leon:
  What's wrong, Marvin? Open your eyes!!" / "Ada: Leon! What are you doing!? The virus is on the loose here as well!
  Hurry to the shelter!").
- ROOM50B0 (C-2 Hangar) sub00@0x00512 `Ck(3,0x62)==0`: msg 0-16 ("Ada: We're all set. Annette is waiting for us
  below, and I left Marvin with her." / "Ada: Dr. Birkin's wife and my boss." / "Ada: He's dead. Umbrella killed
  him." / "Leon: Ada, why did you set the reactor to blow up?" / "Ada: Aaaaaaaagggghhh!" / "Ada: I... I'm sorry,
  Leon. I'm not... going... to make it... am I?"), msg 18 "She's dead." -> (3,0x62)=1 @0x005A6; danach Ada-Leiche
  @0x00566 Pos **(-11800,0,-11600)**. Toeter: 2 Affen Typ 0x29 @0x00526/@0x0053A.
- ROOM6020 (Shelter) sub00@0x00BAA Ada + Marvin `Ck(3,0x4C)==1` (Vor-Zustand vor 50B0; Ada sagt in 50B0 msg 0 "I left
  Marvin with her [Annette]").

**Elza:** kein lebender Ada-Spawn. ROOM50B1 sub00@0x004F2 Obj 0 Pos **(-11800,0,-11600)** (= Adas Todesposition aus
50B0) + msg 0 **"It's Ada. She's dead. It looks like one of those gorillas killed her."** Marvin erwaehnt sie in
ROOM30C1 msg 4/6 ("he's helping me and Ada escape the city" / "Someone we rescued from the RPD").

### 1.5 Marvin (Typ 0x40)

**Leon:**
- ROOM10D0 (2F Corridor) sub00@0x01256 Marvin Pos (5775,0,23625) `Ck(4,0xF7)==0`; Szene sub21 (AOT Slot 18 =
  Rolltor-Schalter): msg 12-27 "???: Freeze!" / "Leon: Wait don't shoot!" / "Marvin: Leon... You're still alive!" /
  "Marvin: You've already seen him [Roy], I thought he was the only one left at the station" / "Leon: ...the chief is
  injured, one of those things bit him." / "Leon: The chief told me about using the communication system" / "Marvin:
  Okay, I'll meet with the chief first and we'll start looking for a way to fix it." / "Leon: I agree, he's in his
  office." Setzt (4,0xF7)=1 @0x019EC. (Leon kennt Marvin: "I'm glad to see that you're still alive Marvin".)
- ROOM11B0 (Garage): main00@0x01080 (geparkt); sub01 `Ck(3,0x83)==0 & Ck(3,0x43)==1` -> sub06 msg 1-12: "Marvin: Leon,
  you're okay! What was the commotion up there?" / "Leon: Those things broke through the shutters and started
  getting inside." / **"Marvin: What about the chief?" / "Marvin: Oh, I see... Damn it!"** / "We can't get out using
  the patrol cars anymore." / Schacht: "Marvin: Leon! give me a hand!" / "Leon: Marvin, I'll climb down first, you
  help her get down!". Setzt (3,0x83)=1, **(4,0xCD)=1** @0x0147C (Schachttuer Slot 5 scharf), **(4,0xF4)=1** @0x01480.
- ROOM1260/2000 (`Ck(4,0xF4)==1`), 3000 (Leichen der RPD-Truppe: "Marvin: They must be from the squad investigating
  on the Umbrella incident."), 3010/3020/3060 (`Ck(4,0xC7)==1`), 3070 (Lift: "Marvin: This thing...? But where will
  it take us?"), 3080, 30E0 sub11 (**"Marvin: The hall is no good...! Those things broke through the main gate and got
  inside!"**, Leon: "That wound... Did those things get you? How careless..."), 4000 ("Take care of Marvin!"),
  4010 main00@0x00678 (bewusstlos, y=-720; Vaccine-Handlung), 6020 (Shelter), 6030 sub06@0x01030 (Train, liegend
  y=-920: "Leon: Hang on, Marvin. We're getting out of here.").

**Elza:** ROOM30C1 (Lobby STAGE3) sub00@0x00BEE Marvin Pos (-12450,0,-1000) `Ck(3,0x39)==1` (gesetzt ROOM3091 sub07):
sub03 msg 1-8: "Elza: Who are you?" / "Officer: I'm Marvin Branagh, RPD. I'm here with Leon Kennedy." /
**"Elza: Leon?! How is he? We got separated back at the RPD."** / "Marvin: He's all right and he's helping me and
Ada escape the city." / "Elza: Ada?" / "Marvin: Someone we rescued from the RPD." / "Elza: Will you tell him that
me and John and Sherry are here and we're all right?" / "Marvin: Will do. See you soon!". Setzt (3,0x39)=0,
(3,0x47)=1. ROOM6031 sub06@0x00E42 Marvin liegend im Zug (Leons Zustand nach 4010/50B0).
**Widerspruch im MZD-Text:** "We got separated back at the RPD" (30C1 msg 3) setzt voraus, dass Elza und Leon sich
im RPD begegnet sind — eine solche Szene gibt es in keinem der 120 Raumpaare (kein Elza-Spawn bei Leon, kein
Leon-Spawn bei Elza; Zensus `zensus.md` Spalte EM). Das ist der einzige Satz, der eine Begegnung behauptet.

### 1.6 Sherry (Typ 0x4B, "Girl")

**Elza:** ROOM11B1 main00@0x01044 (geparkt); sub01 `Ck(3,0x84)==0 & Ck(9,0x0A)==1` ((9,0x0A) = Pliers genommen,
Item_aot_set @0x00FE4 flag=10) -> sub02 msg 0-8: "Elza: Hey! What are you doing in a place like this?" / "Girl:
...Sherry." / "Elza: Just wait there. I'll come rescue you in a bit." / "Elza: ...wait for me quietly in that car
over there." -> (3,0x84)=1 @0x011EC. Spaeter `Ck(3,0x46)==1` (aus ROOM2001 sub04) -> sub03 msg 10/11 "Elza: Sherry!?" /
"Where has she gone!?" -> (3,0x44)=1 @0x0143C -> ROOM2001 sub00@0x0180A Sherry Pos (1797,-1800,-20962) -> sub05
msg 6-14 ("Sherry: It was because of my daddy, I've heard him call me!" / "Elza: My, that's a nice pocket watch!" /
"Sherry: My mom gave it to me") -> (3,0x45)=1, (3,0x4A)=1 -> ROOM2011 (Break Room) sub00@0x008C0 Sherry wartet.
Weiter: 11A1, 3001-3061 (ROOM3061 msg 7/8 "Sherry: I'll unlock the back door from the other side!"), 3071 ("Sherry: I
know how to make it work!"), 3081, 3091 (Sherry rettet John: "she saved my life"; "Sherry: This is... where my dad
used to be..."), 30E1, 4001 ("This is dad's research center... I'll go look for him!" -> rennt weg), 4031, 5021
(Cargo: msg 0 "Sherry must have crawled through here, but the hole is too small for me."; setzt (4,0xC0),(4,0xED),
(3,0x3A),(3,0x2F)), 5031 (Monitor Room, mit Affe Typ 0x29; "Elza: Sherry!"), 5011 (infiziert; G-Vaccine), 6021, 6031
(startet den Zug: "Sherry: I know how to start this! I watched Mom and Dad do it.").

**Leon:** ROOM11B0 main00@0x010A8 (geparkt); sub01 `Ck(3,0x82)==0 & Ck(4,0x06)==1` ((4,0x06) = Strom an, main00@0x00F56
`Ck(4,0xF3)==1`) -> sub04 msg 0 "Leon: H-hey!" (das Maedchen rennt in den Schacht; Beleg fuer die Deutung: ROOM2000
msg 2-6 "Leon: There is a little girl that came in here, we have to find her!" / "before meeting Ada I saw her coming
in this way, if I hadn't seen her maybe we wouldn't have found this way out." / "She must have thought I was a
zombie."). Danach erst wieder ROOM6030 sub06@0x01044 (Zug): msg 2 "Sherry: She's out there fighting the monsters!".
(ROOM3060 Leon msg 1-8 "Elza: Sherry, come back!" ... "John: H-hey, you brat!" sind UNBENUTZTE Reste der Elza-Szene.)

### 1.7 Elliot + Pilot (Typ 0x47, nur Leon)

ROOM1170 (Heliport) sub15@0x017D2 Elliot Pos (1261,-7200,10091) `Ck(4,0xF2)==1` ((4,0xF2) gesetzt von sub11 =
Prolog-Einblendung msg 8-11) -> sub02 msg 0-7: "Elliot: We must go! There is no time to lose!" / "Pilot: Is there no
one else left inside?" / "Elliot: There is no one left, we must go!" / "Elliot: Those things are already close, they
almost caught me!" / "Pilot: Alright get on board we're off." / "Leon: Hey! Hey wait! Down here!" / "Leon: Oh, that's
just freaking great..." / "Leon: Now what am I gonna do?" -> (4,0xF2)=0. Danach sub14 (`var0A==11 & Ck(3,0x3E)==0`)
msg 13-16 "Leon: Damn it, there must be another way out of here." / "If only I could contact the outside." /
**"That's right! I have to report this to the chief."** Der Pilot hat keinen NPC-Spawn (nur Text).
Elza: ROOM1171 ohne Elliot/Pilot/Hubschrauber-Dialog; sub02 (John) "We need to get this thing off of here" —
**VERMUTUNG**: "this thing" = ein Hindernis auf dem Abstieg (Obj-Schieben sub03/sub04), nicht der Hubschrauber.

### 1.8 Annette (Typ 0x4D, nur Leon)

ROOM5090 sub00@0x0122A (Train Car ABC); ROOM6000 (Gate Platform) sub00@0x00F44 `Ck(3,0x64)==0`: msg 0-4 "Annette: You
must be that cop. Your friends are already here." / "Annette: Where's Ada?" / "Annette: I see..." / "Come on. Let's
join your friends and get out of here."; ROOM6030 sub07@0x010D4: msg 11-19 "Annette: Where's that biker girl?" /
"Leon: Keeping the monsters at bay. Can you start this thing?" / "Annette: Not enough power! The train's batteries
are too low!". Bei Elza uebernimmt Sherry Annettes Platz (ROOM6031 sub07@0x00EFA, dieselbe Position (-1790,0,-22272)).

### 1.9 Weitere Namen ohne NPC

"Ben" kommt in keinem Text vor (grep `texte_*.md`). "Dr. Birkin"/"Annette" nur in ROOM50B0 msg 3-6 (Leon).
Tote Polizisten mit Zetteln (beide Szenarien): ROOM1110 msg 0 (4312), ROOM1230 msg 10 (5632), ROOM1070 msg 19,
ROOM1050 msg 5 (Elza nimmt hier die SIG P228, ROOM1051 main00@0x00C22). Umbrella-Truppe: ROOM3000/3001.

---

## 2. Die drei Nutzer-Fragen

### 2.1 Kendo/John bei Elza — Zelle, Befreiung, Flucht

| Frage | Antwort (Beleg) |
|---|---|
| In welchem Raum? | **ROOM1211** (Detention Area, B2), nicht ROOM1221: Spawn sub00@0x01E36 Pos (-26225,0,-25100); ROOM1221 (Detention Cells) ist bytegleich in beiden Szenarien und hat keinen NPC/Text. |
| Sagt er, warum er einsitzt? | Nein. msg 1 "I'm trying to sleep here!", msg 2 "my head is killing me" -> VERMUTUNG Ausnuechterungszelle. Niemand nennt den Einsperrenden. |
| Wie kommt er heraus? | Flag (4,0xEC) "Prison key" aus ROOM1011 sub06 @0x00E2C (Roys Hand). Kein Item. Dazu braucht Elza fuer die B2-Tuer "Prisons" (ROOM11E1 main00@0x0157A, Door Slot 5 inert bis (3,0x8B)) die Yellow Keycard (ROOM1011 main00@0x00954, Taken-Bit (9,138)) und den Code 0513 (ROOM1011 msg 18). |
| Wohin fliehen beide? | John rennt nach 1211 zum Verhoerraum (msg 8), die naechste gemeinsame Szene ist ROOM1021 (Roy). Dann 1141 (Briefing; John lenkt die Gorillas ab), 1171 (Dach; gemeinsam "get down from here"), 10B1 (Vorhof; John fuehrt in die Kanalisation, Tuer Slot 1 -> ROOM1261), 1261, 2001 (Trennung: Elza holt Sherry, John sucht), 11A1 (Sewer Exit, zu dritt), 3001 (Fabrik) ... bis 6031 (Zug). |
| Stirbt er? | Nein. Er wird in ROOM30E1 gebissen (msg 2), lebt bis ROOM6031 (und in Leons ROOM6030). |
| Kendo bei Leon? | Nur ROOM6030 (Zug). In STAGE1 nirgends: ROOM1210 ohne NPC/Text, Zellen (1220) bytegleich. Spur: die "Prisons"-Tuer 11E0 ist bei Leon vom Vorspann an offen ((3,0x8B)=1, ROOM1240 sub02 @0x0055A) — das Flag, das Elza mit Yellow Keycard + 0513 setzt (11E1 sub01 @0x01A8A); und die 10 Gefangenen-Zombies in 1210 sind bei Leon von Anfang an da (bei Elza erst nach Johns Befreiung). Beides = Zustand NACH Elzas Besuch. |

### 2.2 Roy

| | Elza | Leon |
|---|---|---|
| Lebend | ROOM1011 (verwundet, Dialog sub02; danach bewusstlos, Prison key + Zettel 0513) | — |
| Tod | ROOM1021 sub02: als Zombie von John erschossen (Clips @0x020C6ff), Leiche Obj 6 (-4488,0,-10684) + msg 9 "He's dead. Poor Roy..." | ROOM1020 main00@0x01DEE Obj 6 **(-4488,0,-10684)** unbedingt + msg 0 "It's the RPD officer, Roy. He appears to have died recently." |
| Flags | (4,0xF1),(4,0xFA),(3,0x74) [1011]; (4,0x60)->(4,0xF0),(3,0x5F) [1021] | keine (ROOM1020 AOT Slot 5 unbedingt) |

Schluss: Leons 1020 ist byte-fuer-byte der Zustand NACH Elzas 1021-Szene (gleiche Leichenposition) ->
**Elza war vor Leon im Main Office**. Dass Leon ihn "recently" tot findet, passt zu einem kleinen Zeitversatz.

### 2.3 Irons

Original Elza: ROOM1151 lebend, ein Satz (msg 2) per Examine, Flag (3,0x9D) ohne Leser. Original Leon: ROOM1150
lebend, Dialog sub08 ((3,0x94)); sein Tod wird nur indirekt erzaehlt (ROOM11B0 msg 3/4). Nutzer-Entscheidung
"bei Elza tot": keine mechanische Abhaengigkeit (keine Tuer, kein Item, kein Flag-Leser) — nur die Zeitleiste
verlangt dann **Leon vor Elza in 1150/1151**.

---

## 3. Kreuz-Zeitleiste: Belege "wer war zuerst da" (raumweise)

| Raum | Beleg | Reihenfolge |
|---|---|---|
| 1020/1021 Main Office | Roy-Leiche Obj 6 gleiche Position (s. 2.2) | **Elza vor Leon** |
| 1210/1211 Detention | Leon: 10 Gefangenen-Zombies 0x1A von Anfang an (main00@0x01D86ff), Zelle leer; Elza: dieselben 10 Records erst nach Johns Befreiung (sub00 `Ck(3,0x2C)==1`, Kill-Bits 0x8a/0x18b..0x1eb identisch) | **Elza vor Leon** |
| 1090/1091 Rear Exterior | Elza 1091 main00@0x0216E..@0x021AA = Leons Nach-Rettungs-Records (ROOM1090 sub00@0x023A0..@0x023DC, `Ck(3,0xBB)==1`), bytegleich inkl. Kill-Bits 0xf8-0xfb; kein Feuer (Typ 0x26), keine Loesch-/Schuppen-Tuer (Leon sub00 Slot 3 @0x02352, nur mit Feuerloescher (3,0x85) solange (3,0x81)==0), kein Text | **Leon vor Elza** |
| 11E0/11E1 + 1240/1241 B2-Tuer "Prisons" | Leons Vorspann setzt **(3,0x8B)=1** (ROOM1240 sub02 @0x0055A `22 03 8b 01`) = genau das Flag, das in 11E0/11E1 der Kartenleser nach Yellow Keycard + Code 0513 setzt (sub01 @0x01A8A); Leon findet die Tuer also bereits aufgeschlossen und hat deshalb keine Yellow Keycard (Item 0x39 nur in ROOM1011). Elzas Vorspann setzt stattdessen (3,0x6F)/(3,0x70) (Schiessstand-Zielscheiben 1191 sub01 @0x02444/@0x02484) | **Elza vor Leon** (sie hat die Tuer aufgeschlossen) |
| 1150/1151 Chief Office | Original: beide lebend (kein Beleg fuer eine Reihenfolge). Mit Nutzer-Vorgabe "tot bei Elza" | **Leon vor Elza** (Vorgabe) |
| 1140/1141 Briefing | Leon: 5 Zombies durch die Fenster (sub00@0x00BAA ff, "The streets are infested with zombies!"); Elza: 2+2 Gorillas durch die Fenster (msg 9) | kein Zeitbeleg; verschiedene Eindringlinge (Widerspruch bleibt) |
| 11B0/11B1 Garage | Elza trifft Sherry (sub02), Sherry verschwindet (sub03); Leon sieht das Maedchen in den Schacht laufen (sub04 "H-hey!", ROOM2000 msg 4-6) | **Elza vor Leon** (Sherry wartet erst im Auto, dann laeuft sie — Leon sieht sie laufen) |
| 11C0 Parking | Elza 11C1: 5 Hunde Typ 0x20 (sub00@0x016F8ff), keine Szene; Leon: Ada + 2 Gorillas | kein Beleg |
| 30C1 Lobby STAGE3 | Marvin trifft Elza (`Ck(3,0x39)`), "helping me and Ada escape" -> Ada lebt noch, Marvin noch nicht gebissen (30E0) | Elzas 30C1 liegt zwischen Leons 2000 und 30E0 |
| 50B0/50B1 Hangar | Elza 50B1: Ada-Leiche Obj 0 (-11800,0,-11600) = Leons Todesposition 50B0 @0x00566 | **Leon vor Elza** |
| 6030/6031 Zug | Leon: "Where's Elza?" "She's out there fighting the monsters!" (msg 1/2); Elza: "Where's Leon?" "He's fighting off monsters. He wants you to start the train." (msg 0/1) | **gleichzeitig** (Zapping-Ende: beide Szenarien laufen parallel und treffen sich am Zug) |

Fazit zum Beratungsansatz: Die RDTs belegen den **parallelen** Ablauf mit raumweisem Zeitversatz (RE2-Zapping-
Muster), nicht "Elza nach Leon": Elza ist in 1020/1210/11E0/11B0 vor Leon, Leon in 1090/50B0 (und per Vorgabe
1150) vor Elza, und am Zug sind beide gleichzeitig. Einzige Textstelle gegen "nie begegnet": ROOM30C1 msg 3
("We got separated back at the RPD") — ein Modder-Satz ohne Szene.

### 3.1 Die Zeitleiste als Ungleichungskette (aus den Belegen oben)

Elza@1211 (John frei) < Elza@1021 (Roy erschossen) < Leon@1020 ("died recently") ... Leon@1090 (Rettung) <
Elza@1091 ... Leon@1150 (Tod, Port) < Elza@1151 (Vorgabe) ... Leon@2000 (Trennung von Marvin) < Elza@30C1
(Marvin allein) < Leon@30E0 (Marvin gebissen, wieder bei Leon) ... Leon@50B0 (Ada stirbt) < Elza@50B1 ...
Leon@6030 = Elza@6031. Diese Kette ist widerspruchsfrei, verlangt aber:
- Leon erreicht das Main Office 1020 erst, nachdem Elza den langen Weg 1011 -> 11B1 (Pliers) -> 11E1 -> 1211 ->
  1021 hinter sich hat. Im Original ist 1020 fuer Leon ab 1040 frei (ROOM1040 Slot 3 @0x010F8) und enthaelt nur
  Munition — er KANN frueh dort sein. Weicher Punkt 1 (Vorschlag: Roys Leiche/Text in 1020 erst nach einem
  Leon-Fortschritt installieren, z.B. (3,0x94) oder (3,0xBB); nur Port).
- Elza erreicht den Hinterhof 1091 (ab 1051 Slot 5 @0x00B7A frei, kein Pflichtraum) erst nach Leons Rettung,
  die bei Leon zwischen L8 und L10 liegt. Weicher Punkt 2 (Vorschlag: analog an einen Elza-Fortschritt koppeln,
  z.B. (3,0xC2) Dach-Szene; nur Port).
- Johns Zelle: wer ihn einsperrte, sagt kein Text (VERMUTUNG Ausnuechterungszelle; Roy nennt ihn "my friend").
  Vorschlag: ein Satz Johns in 1211 oder ein Dokument (wie die Port-Dokumente P1 in ablauf_leon.md).
- ROOM30C1 msg 3 "We got separated back at the RPD" passt nicht zu "nie begegnet" -> Vorschlag: Satz aendern
  ("I heard there was another survivor at the RPD" o.ae.); betrifft nur Elzas STAGE3-Text.
- ROOM1140/1141 (Briefing): Leon = Zombies durch die Fenster, Elza = Gorillas durch die Fenster; beide Raeume
  stehen im Zustand "gerade eingebrochen" -> kein gemeinsamer Zustand; wer zuerst da war, ist nicht belegbar.

---

## 4. Offene Punkte / VERMUTUNGEN

1. ROOM1171 "this thing" (John) — welches Objekt geschoben wird (sub03/sub04 Prop-Bewegung), nicht aus dem Text
   belegbar; fuer die Story reicht "Abstieg vom Dach freimachen".
2. ROOM1041 "gunshot" vor der Roy-Szene: Schuetze nicht benannt (VERMUTUNG Roy).
3. Warum Johns Zelle ohne Door_aot_set: Die Zelle oeffnet sich nur per Szene; ein "Unlock"-Ton fehlt (Nutzer-
   Befund, Spur L).
4. ROOM3091: NPC-Slot 1 Typ 0x42 als Sherry-Platzhalter (VERMUTUNG, s. Abschnitt 0).
5. ROOM11B0 Werkstatt-Tuer (Nutzer: "ohne Strom zu, mit Strom offen"): nicht Teil dieses Auftrags; der SCD-Beleg
   steht in `zensus.md` ROOM11B (Slot 1/4/7 Text msg 15 "It's too dark..." nur bei `Ck(4,0xF3)==0`, sub01@0x01216ff).
