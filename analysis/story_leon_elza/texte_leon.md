# RE1.5 - alle RDT-Nachrichten, Leon-Variante (ROOMxxx0)

Erzeugt von `analysis/story_leon_elza/dump_texte.py` (Dekoder = `re15_port/tools/scd_walk_lib.py`, Glyphen = `engine/src/msg_common.c:175-199`).
Variante: Elza-RDT = Leon-RDT + 1 (Raumlader PSX.EXE @0x800397e4 `srl a0,a0,31` + @0x800397ec `addu a0,a0,v0`; Startraum Leon 0x17=ROOM1170 @0x8001d2a8, Elza 0x03=ROOM1031 @0x8001d324).
Spalten: ID = Nachrichten-Index (Message_on-Argument), Off = Datei-Byte-Offset des Textanfangs, Sp. = Sprecher aus dem Textpraefix "Name:", Farbe = Steuercode 05:xx (Sprecherfarbe), Modus = Steuercode 04:xx (00 = Dialog, 02 = Untersuchungstext). " / " = Zeilenumbruch (0x08).

## ROOM1000 (STAGE1, 177032 B, msg_sec=0xD20)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D22 |  |  | 02 | Nothing unusual. |

## ROOM1010 (STAGE1, 168808 B, msg_sec=0xA4C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0A52 |  |  | 02 | Several books of testimonies / and cases.Nothing useful. |
| 1 | 0x0A8E |  |  | 02 | An old typewriter.Why use it if there are / computers? |
| 2 | 0x0AC8 |  |  | 02 | A camera to record the / interrogations. |

## ROOM1020 (STAGE1, 228992 B, msg_sec=0x2028)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2034 |  |  | 02 | It's the RPD officer, Roy.He appears to have died recently. |
| 1 | 0x2075 |  |  | 02 | There are only reports among / other office objects.Nothing that can be of help. |
| 2 | 0x20C9 |  |  | 02 | It's empty. |
| 3 | 0x20D8 |  |  | 02 | It's Lieutenant Branagh's desk. |
| 4 | 0x20FB |  |  | 02 | There is something saved here. |
| 5 | 0x211D |  |  | 02 | There are only reports of various / cases. |

## ROOM1030 (STAGE1, 191528 B, msg_sec=0x2894)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x28B8 |  |  | 02 | This should keep those things out...For now... |
| 1 | 0x28EE |  |  | 02 | Guess it was only a matter of time... |
| 2 | 0x2917 |  |  | 02 | The emergency shutters have / already been activated. |
| 3 | 0x294E |  |  | 02 | A payphone.The line appears to be down... |
| 4 | 0x297F |  |  | 02 | Some sort of reference chart for / identifying emergency severity." |
| 5 | 0x29C5 |  |  | 02 | This emergency severity" chart / gives more confusion than it prevents. |
| 6 | 0x2A0D |  |  | 02 | There's nothing useful on the counter. |
| 7 | 0x2A37 |  |  | 02 | Seems like the crash broke the axle.Poor thing... |
| 8 | 0x2A70 |  |  | 02 | Seems like whoever rode this crashed / it through the front gate.Wonder if they're ok... |
| 9 | 0x2ACE |  |  | 02 | It's too dark to see if there's / anything inside.Would you like to reach inside? |
| 10 | 0x2B25 |  | 01 | 00 | Found Green Medicine. / Take it? |
| 11 | 0x2B4B |  |  | 02 | Better not dig through other's trash... |
| 12 | 0x2B76 |  |  | 02 | There's glass from the front door / all over the floor. |
| 13 | 0x2BAF |  |  | 02 | Desks for filling out inquiry forms. |
| 14 | 0x2BD7 |  |  | 02 | Seems like even the emergency / broadcast has gone offline... |
| 15 | 0x2C16 |  |  | 02 | A map of dowtown Raccoon City. / Population est. 100,000 or more... |
| 16 | 0x2C5B |  |  | 02 | An emergency alarm.The red light works, but the alarm / bell appears to be malfunctioning. |
| 17 | 0x2CBB |  |  | 02 | There are some words carved here... / I    C  A  N  '  T    E  S  C  A  P  E  !  !  !"The door is locked, I can't open it. |

## ROOM1040 (STAGE1, 213520 B, msg_sec=0x21C8)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x21CC |  |  | 02 | It's a shutter switch. / Will you push it?  |
| 1 | 0x21FA |  |  | 02 | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. |

## ROOM1050 (STAGE1, 297324 B, msg_sec=0xE44)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0E56 |  |  | 02 | It's a shutter switch. / Will you push it?  |
| 1 | 0x0E84 |  |  | 02 | Various posters of missing / persons.It is better not to think about / it... |
| 2 | 0x0ED2 |  |  | 02 | I need a fuse to run the shutter. |
| 3 | 0x0EF7 |  |  | 02 | Just scattered papers and books.Nothing relevant. |
| 4 | 0x0F2E |  |  | 02 | A blackboard full of reports of riots / and cannibalism at points in the city. |
| 5 | 0x0F7E |  |  | 02 | He's got a big bite on his neck, he / didn't give him a chance to avoid it.The weapon can be useful.Sorry, I'll just borrow this... |
| 6 | 0x1007 | Ada | 02 | 00 | Ada: Give me a moment, I need a / break. |
| 7 | 0x1037 | Leon | 01 | 00 | Leon: Relax, I'll take you to the / medical room. |
| 8 | 0x1070 | Leon | 01 | 00 | Leon: Hey, wait! |

## ROOM1060 (STAGE1, 149428 B, msg_sec=0xE38, BYTEGLEICH mit ROOM1061)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0E3A |  |  | 02 | Only the lights of the / buildings are visible. |

## ROOM1070 (STAGE1, 153328 B, msg_sec=0x16DC, BYTEGLEICH mit ROOM1071)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1706 |  |  | 02 | The number of desks far outnumbers / the amount of office chairs...Were these their solution? |
| 1 | 0x1769 |  |  | 02 | The seats contra chairs almost makes  / musical chairs seem like the most fairmethod of ensuring everyone had / an even chance getting good seat for sits. |
| 2 | 0x1807 |  |  | 02 | A cabinet full of binders containing / police reports.Nothing useful. |
| 3 | 0x1852 |  |  | 02 | These boxes contain nothing of use. |
| 4 | 0x1879 |  |  | 02 | The lockers all appear to be locked. |
| 5 | 0x18A1 |  |  | 02 | A work schedule of some sort. / Guess "L.S.K." lucked out this week. |
| 6 | 0x18E7 |  |  | 02 | The work schedule for this week. / This really wasn't my week. |
| 7 | 0x1927 |  |  | 02 | A lot of notes and reminders regarding / ongoing cases.Most appears to be missing people / or random assault cases. |
| 8 | 0x199E |  |  | 02 | A messy desk. Nothing useful here. |
| 9 | 0x19C4 |  |  | 02 | I'd hate sitting this close to the door, / even if I had that computer at my desk. |
| 10 | 0x1A18 |  |  | 02 | During times like these, I almost miss / filling out paperwork at a desk like this. |
| 11 | 0x1A6D |  |  | 02 | A tidy desk with an electric fan. |
| 12 | 0x1A92 |  |  | 02 | These desks are tidy, but the fan on / the other desk is clearly a neusance. |
| 13 | 0x1AE0 |  |  | 02 | Going by the shelf, whoever used this / desk probably enjoyed privacy. |
| 14 | 0x1B28 |  |  | 02 | Like the other, this desk too contains / documents for ongoing investigations. |
| 15 | 0x1B78 |  |  | 02 | These desks all appear quite messy / and seem to contain nothing of use. |
| 16 | 0x1BC2 |  |  | 02 | The desks on this row all seem to / be filled with paperwork. |
| 17 | 0x1C01 |  |  | 02 | These desks look strangely empty. / Guess they were unoccupied. |
| 18 | 0x1C42 |  |  | 02 | You can get a good overview of the / lobby area from here. |
| 19 | 0x1C7E |  |  | 02 | A police officer. He's dead... |
| 20 | 0x1CA0 |  |  | 02 | It's an phone.You can save your progress / with this.Save is not available in / this preview. |

## ROOM1080 (STAGE1, 177696 B, msg_sec=0x0, BYTEGLEICH mit ROOM1081)

_kein Nachrichtenblock_

## ROOM1090 (STAGE1, 336440 B, msg_sec=0x2748)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x275C | Woman | 02 | 00 | Woman: Anyone! Can someone please / get me out of here!? |
| 1 | 0x279C | Leon | 01 | 00 | Leon: Hey! Who's that?! |
| 2 | 0x27BD | Woman | 02 | 00 | Woman: Someone, anyone! If out there, / open the door! |
| 3 | 0x27FB | Leon | 01 | 00 | Leon: There! Are you okay? |
| 4 | 0x281F | Woman | 02 | 00 | Woman: You saved me... Thank you, / you have my gratitude. |
| 5 | 0x2861 | Leon | 01 | 00 | Leon: We can talk later... It's not safe / here. |
| 6 | 0x2899 | Leon | 01 | 00 | Leon: Besides, you need to get that / wound treated, right? Let's go. |
| 7 | 0x28E6 |  |  | 02 | I must hurry up and get something to / put out this fire to save that woman! |
| 8 | 0x2934 |  | 01 | 02 | Will you use the Fire Extinguisher?  |
| 9 | 0x2961 |  | 01 | 02 | You've used the Fire Extinguisher. |

## ROOM10A0 (STAGE1, 101164 B, msg_sec=0x0, BYTEGLEICH mit ROOM10A1)

_kein Nachrichtenblock_

## ROOM10B0 (STAGE1, 93616 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM10C0 (STAGE1, 87376 B, msg_sec=0x1044, BYTEGLEICH mit ROOM10C1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1054 |  |  | 02 | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. |
| 1 | 0x10B2 |  |  | 02 | Only the lights of the / buildings are visible. |
| 2 | 0x10E3 |  |  | 02 | Wanted posters and reports, / nothing relevant. |
| 3 | 0x1114 |  |  | 02 | The emergency channel is / down... |
| 4 | 0x1138 |  |  | 02 | Nothing useful. |
| 5 | 0x114B |  |  | 02 | Best Midwest Winner K-9 Unit / for R.P.D. |
| 6 | 0x1176 |  |  | 02 | Papers of various cases and / forms, nothing important. |
| 7 | 0x11AF |  |  | 02 |  / ... |

## ROOM10D0 (STAGE1, 286320 B, msg_sec=0x1E50)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1E88 |  | 01 | 02 | Enter the first number. |
| 1 | 0x1EA7 |  | 01 | 02 | Enter the second number. |
| 2 | 0x1EC7 |  | 01 | 02 | Enter the third number. |
| 3 | 0x1EE6 |  | 01 | 02 | Enter the fourth number. |
| 4 | 0x1F06 |  |  | 02 | Wrong code, try again. |
| 5 | 0x1F20 |  |  | 02 | You've opened the lock. |
| 6 | 0x1F3B |  |  | 02 | Communication Room"It's electronically / locked.There's a card reader / on the right. |
| 7 | 0x1F95 |  | 01 | 02 | A card reader.You have to use the Blue Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  |
| 8 | 0x201E |  | 01 | 02 | You have not the Blue Keycard / to operate it. |
| 9 | 0x2052 |  | 01 | 02 | You've used the Blue Keycard. |
| 10 | 0x2077 | Left |  | 02 | Left: First Aid Room, Communication / Room. Right: Archive Room. |
| 11 | 0x20B9 |  |  | 02 | It's a shutter switch. / Will you push it?  |
| 12 | 0x20E7 |  | 07 | 00 | ???: Freeze! |
| 13 | 0x20FD | Leon | 01 | 00 | Leon: Wait don't shoot! |
| 14 | 0x211E | Marvin | 07 | 00 | Marvin: Leon... You're still alive! |
| 15 | 0x214B | Leon | 01 | 00 | Leon: I'm glad to see that you're / still alive Marvin. |
| 16 | 0x218A | Leon | 01 | 00 | Leon: Where are the rest of the / officers? |
| 17 | 0x21BD | Leon | 01 | 00 | Leon: Has everyone been eaten by / zombies? or have they turned into / zombies? |
| 18 | 0x2212 | Marvin | 07 | 00 | Marvin: You've already seen him, / I thought he was the only one left at |
| 19 | 0x2262 |  |  | 00 | the station, tell me, are there other / survivors? |
| 20 | 0x2298 | Leon | 01 | 00 | Leon: You are the only one who I found, / the chief is injured, one of those / things bit him. |
| 21 | 0x22FC | Marvin | 07 | 00 | Marvin: Damn, the situation is terrible, / we have to find a way out of here. |
| 22 | 0x2351 | Leon | 01 | 00 | Leon: The chief told me about using the / communication system and asking for / outside help. |
| 23 | 0x23B4 | Marvin | 07 | 00 | Marvin: But the system is damaged, if / the other officers could not repair it / as only we can? |
| 24 | 0x241A | Leon | 01 | 00 | Leon: I don't know, it remains to try, / the helicopter abandoned us, the phones |
| 25 | 0x2472 |  |  | 00 | are down and with the building besieged / by those things, I don't see any more / options. |
| 26 | 0x24CE | Marvin | 07 | 00 | Marvin: Okay, I'll meet with the chief / first and we'll start looking for a way / to fix it. |
| 27 | 0x2531 | Leon | 01 | 00 | Leon: I agree, he's in his office. |

## ROOM10E0 (STAGE1, 157632 B, msg_sec=0xE2C, BYTEGLEICH mit ROOM10E1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0E36 |  |  | 02 | It's the body of a doctor.He has no vital signs. |
| 1 | 0x0E6C |  |  | 02 | An old typewriter.Why use it if there are / computers? |
| 2 | 0x0EA6 |  |  | 02 | September 1998. |
| 3 | 0x0EB9 |  |  | 02 | Various medicines and bandages. |
| 4 | 0x0EDC |  |  | 02 | It's a photo of the Arklay / Mountains. |

## ROOM10F0 (STAGE1, 225092 B, msg_sec=0x1168, BYTEGLEICH mit ROOM10F1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1174 |  |  | 02 | A large communications device.It's out of commission.I can't send any messages from / here. |
| 1 | 0x11D5 |  |  | 02 | Nothing suspicious nor / out of the ordinary. |
| 2 | 0x1204 |  |  | 02 | There isnt anything / useful on this shelf. |
| 3 | 0x1231 |  |  | 02 | There are various devices.This must be where they / gathered information. |
| 4 | 0x127E |  |  | 02 | Several files from past / cases.Nothing useful here. |
| 5 | 0x12B6 |  |  | 02 | It's a small office.It's locked. |

## ROOM1100 (STAGE1, 181704 B, msg_sec=0xD9C, BYTEGLEICH mit ROOM1101)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0DA8 |  |  | 02 | It's electronically locked. |
| 1 | 0x0DC7 |  |  | 02 | It's a control panel.A type of voice device is required / to open the lock. |
| 2 | 0x0E16 |  |  | 02 | You've opened the lock. |
| 3 | 0x0E31 |  | 01 | 02 | Will you use the Minidisc Player w/ Disc?  |
| 4 | 0x0E64 |  | 01 | 02 | You've used the Minidisc Player w/ Disc. |
| 5 | 0x0E94 |  |  | 02 | I don't need to use this anymore. |

## ROOM1110 (STAGE1, 196360 B, msg_sec=0xD54, BYTEGLEICH mit ROOM1111)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D68 |  | 01 | 02 | It's a police officer, he's dead.He is holding a slip.The numbers 4312" are / printed on the slip. |
| 1 | 0x0DD5 |  |  | 02 | Files on various cases.None appear to be useful. |
| 2 | 0x0E0B |  |  | 02 | Visual materials. |
| 3 | 0x0E20 |  |  | 02 | A pile of investigation reports. |
| 4 | 0x0E44 |  |  | 02 | Weapons seized, those that remain / are unusable. |
| 5 | 0x0E77 |  |  | 02 | An unarmed carbine, I have no idea / how to fix it. |
| 6 | 0x0EAC |  |  | 02 | A rocket launcher, has something / written on it:Evidence of the Arklay incident, / do not manipulate. |
| 7 | 0x0F14 |  |  | 02 | Ammunition boxes seized, they've / all taken it. |
| 8 | 0x0F46 |  | 01 | 02 | Will you take the Time Bomb?  |
| 9 | 0x0F6C |  | 01 | 02 | You've taken the Time Bomb. |

## ROOM1120 (STAGE1, 86900 B, msg_sec=0xDB4, BYTEGLEICH mit ROOM1121)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0DC6 |  |  | 02 | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. |
| 1 | 0x0E24 |  |  | 02 | A painting, titled: / The Music Lesson" |
| 2 | 0x0E4E |  |  | 02 | A painting of 5 women's. |
| 3 | 0x0E6A |  |  | 02 | Only the lights of the / buildings are visible. |
| 4 | 0x0E9B |  |  | 02 | Various reports and case / information.Nothing useful. |
| 5 | 0x0ED5 |  |  | 02 | A strange Christmas toy. |
| 6 | 0x0EF1 |  |  | 02 | It's a phone.You can save your progress / with this.Save is not available in / this preview. |
| 7 | 0x0F51 |  |  | 02 | ... |
| 8 | 0x0F58 |  |  | 02 | An advertisement for an arm / wrestling contest.The winner will be named / Chief for a Day of theRaccoon Police Department!" |

## ROOM1130 (STAGE1, 198108 B, msg_sec=0xB0C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B18 |  |  | 02 | It's a shutter switch. / Will you push it?  |
| 1 | 0x0B46 |  |  | 02 | It's not necessary to go back. |
| 2 | 0x0B68 |  |  | 02 | A photo of the members of / S.T.A.R.S. |
| 3 | 0x0B90 |  |  | 02 | An emblem of the Raccoon / S.W.A.T Team. |
| 4 | 0x0BBA |  |  | 02 | A painting of a man with a hook, / seeing it makes me uncomfortable. |
| 5 | 0x0C00 |  |  | 02 | A shelf with trophies.Among them is the award for best / shooter, Chris Redfield. |

## ROOM1140 (STAGE1, 147176 B, msg_sec=0xD88)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D94 |  |  | 02 | You've opened the lock. |
| 1 | 0x0DAF |  |  | 02 | A beautiful painting.There's a switch below. / Will you push it?  |
| 2 | 0x0DF5 |  |  | 02 | It's a big table. |
| 3 | 0x0E0A |  |  | 00 | The streets are infested with / zombies! |
| 4 | 0x0E36 |  |  | 02 | The streets are infested with / zombies! |
| 5 | 0x0E60 |  |  | 02 | The switch is not responding... |

## ROOM1150 (STAGE1, 194080 B, msg_sec=0x12F8)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1316 |  |  | 02 | There's a small sign in one corner. / It reads:Raccoon City 21st Century, / Hillman-Grayson Architects LLC." |
| 1 | 0x1385 |  |  | 02 | It's a phone.You can save your progress / with this.Save is not available in / this preview. |
| 2 | 0x13E5 | Irons | 02 | 00 | Irons: I'll be fine. Just worry about / yourself for now. |
| 3 | 0x1426 |  |  | 02 | Itembox is not available in / this preview. |
| 4 | 0x1453 | Leon | 01 | 00 | Leon: Sir... the helicopter is / gone... it was our only way out... |
| 5 | 0x149E | Irons | 02 | 00 | Irons: Calm down Rookie, there / must be a way. |
| 6 | 0x14D5 | Leon | 01 | 00 | Leon: The building is besieged by / those things, I don't see any option. |
| 7 | 0x1526 | Irons | 02 | 00 | Irons: Maybe using the communication / system... you can ask for outside help. |
| 8 | 0x157C | Leon | 01 | 00 | Leon: But the system is damaged. |
| 9 | 0x15A6 | Irons | 02 | 00 | Irons: I know, I know... but some / officers were trying to repair it, / you may be luckier than them. |
| 10 | 0x1612 | Leon | 01 | 00 | Leon: Ok, I'll go see if I can / achieve something with that. |
| 11 | 0x1657 | Irons | 02 | 00 | Irons: Take this card... it will allow / you to enter the communications room... / now go... |
| 12 | 0x16B9 | Irons | 02 | 00 | Irons: And remember this Leon, never / let your guard down... uniform or not, |
| 13 | 0x170E |  |  | 00 | those things... are no longer what / they were, don't make my mistake... |
| 14 | 0x175A | Leon | 01 | 00 | Leon: Yes sir... |

## ROOM1160 (STAGE1, 69944 B, msg_sec=0x0, BYTEGLEICH mit ROOM1161)

_kein Nachrichtenblock_

## ROOM1170 (STAGE1, 303564 B, msg_sec=0x1920)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1942 | Elliot | 07 | 00 | Elliot: We must go! There is no / time to lose! |
| 1 | 0x1979 | Pilot | 02 | 00 | Pilot: Is there no one else left / inside? |
| 2 | 0x19AB | Elliot | 07 | 00 | Elliot: There is no one left, we / must go! |
| 3 | 0x19DE | Elliot | 07 | 00 | Elliot: Those things are already / close, they almost caught me! |
| 4 | 0x1A26 | Pilot | 02 | 00 | Pilot: Alright get on board we're / off. |
| 5 | 0x1A56 | Leon | 01 | 00 | Leon: Hey! Hey wait! Down here! |
| 6 | 0x1A7F | Leon | 01 | 00 | Leon: Oh, that's just freaking great... |
| 7 | 0x1AB0 | Leon | 01 | 00 | Leon: Now what am I gonna do? |
| 8 | 0x1AD7 |  |  | 00 | We barricaded ourselves / inside the police station... |
| 9 | 0x1B11 |  |  | 00 | Where we thought it would / be safe. |
| 10 | 0x1B39 |  |  | 00 | It was no use, and so began a / life and death struggle... |
| 11 | 0x1B77 |  |  | 00 | To escape. |
| 12 | 0x1B87 |  |  | 02 | It's locked from the other side. |
| 13 | 0x1BAB | Leon | 01 | 00 | Leon: Damn it, there must be another / way out of here. |
| 14 | 0x1BEA | Leon | 01 | 00 | Leon: If only I could contact the / outside. |
| 15 | 0x1C1E | Leon | 01 | 00 | Leon: ... |
| 16 | 0x1C31 | Leon | 01 | 00 | Leon: That's right! I have to report / this to the chief. |

## ROOM1180 (STAGE1, 91848 B, msg_sec=0xB60, BYTEGLEICH mit ROOM1181)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B66 |  |  | 02 | Weapon Storage"It's electronically / locked.There's a card reader / on the right. |
| 1 | 0x0BBC |  |  | 02 | A card reader.It seems that it will not / work if power is not restored. |
| 2 | 0x0C08 |  |  | 02 |  / ... |

## ROOM1190 (STAGE1, 330532 B, msg_sec=0x2E08)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2E14 |  |  | 02 | There's a switch here. Push it?  |
| 1 | 0x2E39 |  |  | 02 | There's a driver here. Take it?  |
| 2 | 0x2E5E |  |  | 02 | I have nothing else to do / here. |
| 3 | 0x2E81 |  |  | 02 | No response...The power is not supplied. |
| 4 | 0x2EAF |  |  | 02 | There is one R.P.D. armor / that should fit you.Will you equip it?  |
| 5 | 0x2EF7 |  |  | 02 | Will you unequip the armor? e         /   4      4   / |

## ROOM11A0 (STAGE1, 253612 B, msg_sec=0x13E8)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x13EE |  |  | 02 | There's a shutter switch here. Press it?  |
| 1 | 0x141C |  |  | 02 | It's locked. |
| 2 | 0x142C |  |  | 02 | It's locked from the other side. |

## ROOM11B0 (STAGE1, 232492 B, msg_sec=0x18D8)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x18F8 | Leon | 01 | 00 | Leon: H-hey! |
| 1 | 0x190E | Marvin | 07 | 00 | Marvin: Leon, you're okay! What / was the commotion up there? |
| 2 | 0x1953 | Leon | 01 | 00 | Leon: Those things broke through the / shutters and started getting inside. |
| 3 | 0x19A6 | Marvin | 07 | 00 | Marvin: What about the chief? |
| 4 | 0x19CD | Marvin | 07 | 00 | Marvin: Oh, I see... Damn it! |
| 5 | 0x19F4 | Marvin | 07 | 00 | Marvin: Well you saw it, right? / We can't get out using the patrol / cars anymore. What should we do? |
| 6 | 0x1A60 | Leon | 01 | 00 | Leon: Marvin, please come here! |
| 7 | 0x1A89 | Leon | 01 | 00 | Leon: There's a manhole here. How / about an underground escape route? |
| 8 | 0x1AD7 | Marvin | 07 | 00 | Marvin: We don't really have any other / option than to try. Okay, let's see if / we can move this! |
| 9 | 0x1B40 | Marvin | 07 | 00 | Marvin: Leon! give me a hand! |
| 10 | 0x1B67 | Marvin | 07 | 00 | Marvin: Okay, let's hurry! |
| 11 | 0x1B8B | Leon | 01 | 00 | Leon: Marvin, I'll climb down first, / you help her get down! |
| 12 | 0x1BD0 | Marvin | 07 | 00 | Marvin: Okay! Be careful down there! |
| 13 | 0x1BFE |  |  | 02 | It doesn't seem like you could move / this on your own. |
| 14 | 0x1C37 |  |  | 02 | The shutter is tightly sealed in place. |
| 15 | 0x1C62 |  |  | 02 | It's too dark to see anything...I must find way to activate the energy. |

## ROOM11C0 (STAGE1, 155552 B, msg_sec=0x1C84)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1C98 | Leon | 01 | 00 | Leon: Ada! Where's Marvin? Wasn't / he with you? |
| 1 | 0x1CD0 | Ada | 02 | 00 | Ada: He turned around all of a sudden / to go look for you. |
| 2 | 0x1D13 | Ada | 02 | 00 | Ada: There was this really loud noise / from upstairs earlier. |
| 3 | 0x1D59 | Ada | 02 | 00 | Ada: You didn't run into him? |
| 4 | 0x1D80 | Leon | 01 | 00 | Leon: What!? |
| 5 | 0x1D96 | Leon | 01 | 00 | Leon: Ada, you hide inside that patrol / car. |
| 6 | 0x1DCB | Ada | 02 | 00 | Ada: O-okay, I'll do that... |
| 7 | 0x1DF1 | Ada | 02 | 00 | Ada: Okay, it's over now. |
| 8 | 0x1E14 | Ada | 02 | 00 | Ada: ...There are some outrageous / monsters out there. |
| 9 | 0x1E53 | Leon | 01 | 00 | Leon: Yeah... Anyway, let's get / out of here. |

## ROOM11D0 (STAGE1, 130140 B, msg_sec=0x18FC, BYTEGLEICH mit ROOM11D1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1906 |  |  | 02 | A panel to control the cells of / the dogs.It does not work. |
| 1 | 0x1946 |  |  | 02 | The road is blocked and the knob / is broken. |
| 2 | 0x1975 |  |  | 02 | Boxes and dog food.Nothing useful. |
| 3 | 0x199D |  |  | 02 | Raccoon Police wins the award for / the best K-9 unit in the Midwest. |
| 4 | 0x19E4 |  |  | 02 | -Warning-Sewer access area. / Authorized personal only. |

## ROOM11E0 (STAGE1, 209684 B, msg_sec=0x210C, BYTEGLEICH mit ROOM11E1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2126 |  | 01 | 02 | Enter the first number. |
| 1 | 0x2145 |  | 01 | 02 | Enter the second number. |
| 2 | 0x2165 |  | 01 | 02 | Enter the third number. |
| 3 | 0x2184 |  | 01 | 02 | Enter the fourth number. |
| 4 | 0x21A4 |  |  | 02 | Wrong code, try again. |
| 5 | 0x21BE |  |  | 02 | You've opened the lock. |
| 6 | 0x21D9 |  |  | 02 | Prisons"It's electronically / locked.There's a card reader / on the right. |
| 7 | 0x2228 |  | 01 | 02 | A card reader.You have to use the Yellow Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  |
| 8 | 0x22B3 |  | 01 | 02 | You have not the Yellow Keycard / to operate it. |
| 9 | 0x22E9 |  | 01 | 02 | You've used the Yellow Keycard. |
| 10 | 0x2310 |  |  | 02 | If I had some kind of pliers maybe I / could cut that wire and disable thesafety lock. |
| 11 | 0x236A |  | 01 | 02 | Will you use the Pliers?  |
| 12 | 0x238C |  | 01 | 02 | You've used the Pliers. |

## ROOM11F0 (STAGE1, 152588 B, msg_sec=0x1818, BYTEGLEICH mit ROOM11F1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1820 |  |  | 02 | -Reserve Power / Control Panel-Power can be routed to / the areas withinsufficient power using / these ten switches.Using all the power may / cause a short andcut the power supply. |
| 1 | 0x18D8 |  |  | 02 | Will you push them?  |
| 2 | 0x18F1 |  | 01 | 02 | Power supply OK. |
| 3 | 0x1909 |  | 02 | 02 | Power supply incorrect. |

## ROOM1200 (STAGE1, 167132 B, msg_sec=0x0, BYTEGLEICH mit ROOM1201)

_kein Nachrichtenblock_

## ROOM1210 (STAGE1, 140596 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM1220 (STAGE1, 140456 B, msg_sec=0x0, BYTEGLEICH mit ROOM1221)

_kein Nachrichtenblock_

## ROOM1230 (STAGE1, 166268 B, msg_sec=0x14F4, BYTEGLEICH mit ROOM1231)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x150C |  | 01 | 02 | Enter the first number. |
| 1 | 0x152B |  | 01 | 02 | Enter the second number. |
| 2 | 0x154B |  | 01 | 02 | Enter the third number. |
| 3 | 0x156A |  | 01 | 02 | Enter the fourth number. |
| 4 | 0x158A |  |  | 02 | Wrong code, try again. |
| 5 | 0x15A4 |  |  | 02 | You've opened the lock. |
| 6 | 0x15BF |  |  | 02 | Weapon Storage"It's electronically / locked.There's a card reader / on the right. |
| 7 | 0x1615 |  | 01 | 02 | A card reader.You have to use the Red Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  |
| 8 | 0x169D |  | 01 | 02 | You have not the Red Keycard / to operate it. |
| 9 | 0x16D0 |  | 01 | 02 | You've used the Red Keycard. |
| 10 | 0x16F4 |  | 01 | 02 | A miserable death...He is holding a slip.The numbers 5632" are / printed on the slip. |
| 11 | 0x1754 |  |  | 02 |  / ... |

## ROOM1240 (STAGE1, 163744 B, msg_sec=0x62C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0638 |  |  | 00 | A bizarre incident occurred in the / outskirts of an American suburb called / Raccoon City. |
| 1 | 0x0695 |  |  | 00 | It was later revealed that the terrible / disaster had been caused by the T-Virus, |
| 2 | 0x06EB |  |  | 00 | a mutagenic toxin created by the / international corporation Umbrella / incorporated. |
| 3 | 0x0742 |  |  | 00 | The Raccoon police department special / S.T.A.R.S Unit began an immediate / investigation. |
| 4 | 0x079E |  |  | 00 | The case was apparently closed thanks / to the efforts of S.T.A.R.S members / Chris Redfield and Jill Valentine. |
| 5 | 0x0810 |  |  | 00 | But the umbrella corporation's / experiments were far from finished. |

## ROOM1250 (STAGE1, 140056 B, msg_sec=0x0, BYTEGLEICH mit ROOM1251)

_kein Nachrichtenblock_

## ROOM1260 (STAGE1, 287920 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM1270 (STAGE1, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM1271)

_kein Nachrichtenblock_

## ROOM2000 (STAGE2, 359612 B, msg_sec=0x1BC0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1BD8 | Leon | 01 | 00 | Leon: Ada, Marvin, hold on. |
| 1 | 0x1BFD | Marvin | 07 | 00 | Marvin: What's up Leon? |
| 2 | 0x1C1E | Leon | 01 | 00 | Leon: There is a little girl that / came in here, we have to find her! |
| 3 | 0x1C6C | Marvin | 07 | 00 | Marvin: A little girl, are you sure? |
| 4 | 0x1C9A | Leon | 01 | 00 | Leon: Yes, before meeting Ada I saw / her coming in this way, if I hadn't |
| 5 | 0x1CEB |  |  | 00 | seen her maybe we wouldn't have / found this way out. |
| 6 | 0x1D24 | Leon | 01 | 00 | Leon: She must have thought I was / a zombie. |
| 7 | 0x1D59 | Ada | 02 | 00 | Ada: So there is no time to lose, / we do not know what things are in |
| 8 | 0x1DA6 |  |  | 00 | the sewers, zombies are not the only / things we should worry about here. |
| 9 | 0x1DF3 | Leon | 01 | 00 | Leon: Okay, it will be better to / separate and cover more ground. |
| 10 | 0x1E3D | Leon | 01 | 00 | Leon: Marvin sees Ada up front. |
| 11 | 0x1E66 | Marvin | 07 | 00 | Marvin: Understood. If something / happens, this area will be to meet. |

## ROOM2010 (STAGE2, 316824 B, msg_sec=0x8FC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0906 |  | 01 | 02 | Will you take the Crank?  |
| 1 | 0x0928 |  | 01 | 02 | You've taken the Crank. |
| 2 | 0x0947 |  |  | 02 | Itembox is not available in / this preview. |
| 3 | 0x0974 |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |
| 4 | 0x09D7 |  |  | 02 | An old typewriter.Why use it if there are / computers? |

## ROOM2020 (STAGE2, 322276 B, msg_sec=0xB44)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B4C |  |  | 02 | It's a square hole. |
| 1 | 0x0B63 |  | 01 | 02 | Will you use the Crank?  |
| 2 | 0x0B84 |  |  | 02 | You've opened the lock. |
| 3 | 0x0B9F |  |  | 02 | It's locked. |

## ROOM2030 (STAGE2, 282076 B, msg_sec=0x200C, BYTEGLEICH mit ROOM2031)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2014 |  |  | 02 | It's locked from the other side. |
| 1 | 0x2038 |  |  | 02 | The elevator is not responding...Maybe it should be activated / from somewhere. |
| 2 | 0x208B |  | 01 | 02 | Will you take the Fuse?  |
| 3 | 0x20AC |  | 01 | 02 | You've taken the Fuse. |

## ROOM2040 (STAGE2, 255800 B, msg_sec=0x1750, BYTEGLEICH mit ROOM2041)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x175C |  | 01 | 02 | Notice of damage door:While doing maintenance and / construction work on the Raccoonsewage system we found a problem / with a door, it seems that itrusted due to water leaks, we tried / everything and nothing works!Unless we use explosives or something / heavy it doesn't seem I'm going togive in, I'll have to ask the manager / for options for this problem.Bill Johnson, / Knights Construction |
| 1 | 0x18F7 |  |  | 02 | An old typewriter, perhaps someone will / find it useful even in these times. |
| 2 | 0x1946 |  |  | 02 | This old console is useless. |
| 3 | 0x1966 |  |  | 02 | The body of a sewer worker... / He has something in his hands. |
| 4 | 0x19A6 |  | 02 | 02 | Bomb activated, I have 10 seconds / before it detonates! |
| 5 | 0x19E4 |  | 01 | 02 | Will you use the Time Bomb?    e         /            4   2  /           /      3  x  x                     |

## ROOM2050 (STAGE2, 342712 B, msg_sec=0x2440, BYTEGLEICH mit ROOM2051)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2446 |  |  | 02 | A lever to fill and drain water. / Will you move it?  |
| 1 | 0x247E |  |  | 02 | The water is blocking the / entrance. |
| 2 | 0x24A5 |  |  | 02 | I have nothing else to do / here. |

## ROOM2060 (STAGE2, 272112 B, msg_sec=0x1758, BYTEGLEICH mit ROOM2061)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1766 |  |  | 02 | The generator does not appear to / be on.There are four switches...Will you push them?  |
| 1 | 0x17C4 |  |  | 02 | I need to insert the missing / fuse before I can operate this. |
| 2 | 0x1804 |  |  | 02 | It seems that the generator is / working now. |
| 3 | 0x1833 |  |  | 02 | One of the fuses is missing... |
| 4 | 0x1855 |  | 01 | 02 | Will you use the Fuse?  |
| 5 | 0x1875 |  | 01 | 02 | You've used the Fuse. |
| 6 | 0x1892 |  |  | 02 | The fuse is in place. |

## ROOM2070 (STAGE2, 174036 B, msg_sec=0xD04, BYTEGLEICH mit ROOM2071)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D06 |  |  | 02 | You've opened the lock. |

## ROOM2080 (STAGE2, 173608 B, msg_sec=0xCF4, BYTEGLEICH mit ROOM2081)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0CF8 |  |  | 02 | From here maybe I can control the / water level.Will you operate the computer?  |
| 1 | 0x0D4C |  |  | 02 | The water level is very high...I can't get down to the floor below. |

## ROOM2090 (STAGE2, 311024 B, msg_sec=0x0, BYTEGLEICH mit ROOM2091)

_kein Nachrichtenblock_

## ROOM20A0 (STAGE2, 300176 B, msg_sec=0x2008, BYTEGLEICH mit ROOM20A1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x200E |  |  | 02 | There's a shutter switch here. Press it?  |
| 1 | 0x203C |  |  | 02 | It's locked. |
| 2 | 0x204C |  |  | 02 | You've opened the lock. |

## ROOM20B0 (STAGE2, 201460 B, msg_sec=0x0, BYTEGLEICH mit ROOM20B1)

_kein Nachrichtenblock_

## ROOM20C0 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20C1)

_kein Nachrichtenblock_

## ROOM20D0 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20D1)

_kein Nachrichtenblock_

## ROOM20E0 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20E1)

_kein Nachrichtenblock_

## ROOM20F0 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20F1)

_kein Nachrichtenblock_

## ROOM3000 (STAGE3, 158784 B, msg_sec=0x1828)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x183A |  |  | 02 | A ladder leads down the manhole. / Descend it?  |
| 1 | 0x186C |  |  | 02 | My strength is running out. / I've got to find the others! |
| 2 | 0x18A8 | Leon | 01 | 00 | Leon: Marvin! Aren't these... RPD / officers!? |
| 3 | 0x18DE | Marvin | 07 | 00 | Marvin: Oh, man... They must be from the / squad investigating on the Umbrella / incident. |
| 4 | 0x193E | Marvin | 07 | 00 | Marvin: We don't know what's up ahead... / Stay alert, Leon! |
| 5 | 0x1982 | Leon | 01 | 00 | Leon: Marvin, what about that woman? |
| 6 | 0x19B0 | Marvin | 07 | 00 | Marvin: Ada...? She went on ahead. |
| 7 | 0x19DC | Marvin | 07 | 00 | Marvin: She didn't seem to be at all / disturbed by all this massacre. |
| 8 | 0x1A2A | Leon | 01 | 00 | Leon: There's a ton of things I need / to ask her the next time I see her... |

## ROOM3010 (STAGE3, 166992 B, msg_sec=0x2318)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x231C |  |  | 02 | It's locked.An ID card is required to open it. |
| 1 | 0x2350 |  | 01 | 02 | You've used the Green Keycard. |

## ROOM3020 (STAGE3, 162568 B, msg_sec=0xE0C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0E1A | Leon | 01 | 00 | Leon: Dammit! the door does not open! |
| 1 | 0x0E49 | Marvin | 07 | 00 | Marvin: There's an open window, maybe / we could enter through there? |
| 2 | 0x0E96 | Marvin | 07 | 00 | Marvin: Leon give me an hand! |
| 3 | 0x0EBD |  |  | 02 | It's locked from the other side. |
| 4 | 0x0EE1 |  |  | 02 | The car is locked, I can't get inside. |
| 5 | 0x0F0B |  |  | 02 | The panel is smashed and doesn't work. |
| 6 | 0x0F35 |  |  | 02 |  / It's locked. |

## ROOM3030 (STAGE3, 81004 B, msg_sec=0x0, BYTEGLEICH mit ROOM3031)

_kein Nachrichtenblock_

## ROOM3040 (STAGE3, 152352 B, msg_sec=0x15CC, BYTEGLEICH mit ROOM3041)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x15D4 |  |  | 02 | You've opened the lock. |
| 1 | 0x15EF |  |  | 02 | There's a button here. Push it?  |
| 2 | 0x1614 |  |  | 02 | The boxes are blocking the way. / I must find a way to move them. |
| 3 | 0x1657 |  |  | 02 | The control panel is not / responding...Maybe it should be activated / from somewhere. |

## ROOM3050 (STAGE3, 262652 B, msg_sec=0x2638, BYTEGLEICH mit ROOM3051)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2648 |  |  | 02 | There's a button here. Push it?  |
| 1 | 0x266D |  |  | 02 | The control panel is not / responding...Maybe it should be activated / from somewhere. |
| 2 | 0x26C5 |  |  | 02 | The elevator is not responding...Maybe it should be activated / from somewhere. |
| 3 | 0x2718 |  | 01 | 02 | The computer seems to be working. / There is a note:In case the machinery stops / working, it will be necessary touse the Red Master Keycard to / restore the system." |
| 4 | 0x27C5 |  | 01 | 02 | Will you use the Red Master Keycard?  |
| 5 | 0x27F3 |  | 01 | 02 | You've used the Red Master Keycard. |
| 6 | 0x281E |  |  | 02 | The system has been successfully / restored. |
| 7 | 0x284C |  |  | 02 | I don't need to use this anymore. |

## ROOM3060 (STAGE3, 129900 B, msg_sec=0x10F0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1106 |  |  | 02 |  / It's locked. |
| 1 | 0x1117 | Elza |  | 00 | Elza: Sherry, come back! |
| 2 | 0x1135 | Elza |  | 00 | Elza: What's wrong? Why did you start / running...? |
| 3 | 0x116C | Sherry |  | 00 | Sherry: Elza... I know a secret / way in! |
| 4 | 0x1199 | Elza |  | 00 | Elza: A secret entrance? |
| 5 | 0x11B7 | Sherry |  | 00 | Sherry: It's here, I can get inside / through this hole! |
| 6 | 0x11F3 | Elza |  | 00 | Elza: Sherry, wait! |
| 7 | 0x120C | Sherry |  | 00 | Sherry: I'll unlock the back door from / the other side!Just wait by the door, okay? |
| 8 | 0x126A | John |  | 00 | John: H-hey, you brat! That's dangerous, / get back here! |
| 9 | 0x12A7 |  |  | 02 | For some reason the door isn't / locked anymore... |
| 10 | 0x12DB |  |  | 02 | It's locked from the other side. |

## ROOM3070 (STAGE3, 243040 B, msg_sec=0x3980)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x398C | Leon | 01 | 00 | Leon: Get on the lift! It's our only / option... |
| 1 | 0x39C4 | Marvin | 07 | 00 | Marvin: This thing...? / But where will it take us? |
| 2 | 0x39FF | Ada | 02 | 00 | Ada: Underground... Though we might / be better off staying here and get / eaten by those things... |
| 3 | 0x3A68 | Marvin | 07 | 00 | Marvin: What? |
| 4 | 0x3A7F | Leon | 01 | 00 | Leon: Hurry up! They're coming! |
| 5 | 0x3AA8 |  |  | 02 | The power seems to be off... / If the control box worked,the elevator might move, but... |

## ROOM3080 (STAGE3, 240472 B, msg_sec=0xC08)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0C0C | Ada | 02 | 00 | Ada: You just don't give up... |
| 1 | 0x0C34 | Ada | 02 | 00 | Ada: That's... got to be... |

## ROOM3090 (STAGE3, 184132 B, msg_sec=0x14FC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1504 |  |  | 02 |  / The power is already on... |
| 1 | 0x1523 |  |  | 02 |  / This breaker switch is flipped down... |
| 2 | 0x154E |  |  | 02 |  / This breaker switch is flipped down... |
| 3 | 0x1579 |  |  | 02 | The door is latched shut and won't open. |

## ROOM30A0 (STAGE3, 208760 B, msg_sec=0x88C, BYTEGLEICH mit ROOM30A1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x08A0 |  |  | 02 | Itembox is not available in / this preview. |
| 1 | 0x08CD |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |
| 2 | 0x0930 |  |  | 02 | A detailed map of product / distribution in the city. |
| 3 | 0x0967 |  |  | 02 | It's a disaster. |
| 4 | 0x097B |  |  | 02 | Wait, there's something here. |
| 5 | 0x099C |  |  | 02 | There is nothing useful. |
| 6 | 0x09B8 |  |  | 02 | There are only papers lying / around. |
| 7 | 0x09DF |  |  | 02 | It's empty. |
| 8 | 0x09EE |  |  | 02 | There is nothing more than / papers thrown everywhere.I do not see that it is useful. |
| 9 | 0x0A47 |  |  | 02 | A calendar and several sheets.It must be where they had part / of the control of the orders. |

## ROOM30B0 (STAGE3, 175620 B, msg_sec=0x9A0, BYTEGLEICH mit ROOM30B1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x09A4 |  |  | 02 | Itembox is not available in / this preview. |
| 1 | 0x09D1 |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |

## ROOM30C0 (STAGE3, 117896 B, msg_sec=0xBE0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0BE4 |  |  | 02 | You've opened the lock. |
| 1 | 0x0BFF |  |  | 02 |  / It's locked. |

## ROOM30D0 (STAGE3, 119948 B, msg_sec=0x13BC, BYTEGLEICH mit ROOM30D1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x13C0 |  |  | 02 | The car is locked, I can't get inside. |
| 1 | 0x13EA |  |  | 02 | The panel is smashed and doesn't work. |

## ROOM30E0 (STAGE3, 295380 B, msg_sec=0x10C8)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x10E8 | Ada | 02 | 00 | Ada: What was that noise? |
| 1 | 0x110B | Leon | 01 | 00 | Leon: Y-you...! |
| 2 | 0x1124 | Ada | 02 | 00 | Ada: ...? Where's that other guy? / Wasn't he with you? |
| 3 | 0x1163 | Leon | 01 | 00 | Leon: Y-yeah, Marvin is out in the / hall. He can't get out because of / all the monsters outside... |
| 4 | 0x11CD | Leon | 01 | 00 | Leon: Ada... What have you been doing, / anyway!? |
| 5 | 0x1206 | Leon | 01 | 00 | Leon: And what happened in that / tunnel, what's with all the police / officer corpses? |
| 6 | 0x1263 |  |  | 00 |  / Manhole...? |
| 7 | 0x1275 | Ada | 02 | 00 | Ada: That's really bad... Those things / should already be trying to bust in / there. |
| 8 | 0x12D0 | Ada | 02 | 00 | Ada: That glass won't hold up if they / keep pushing at it! |
| 9 | 0x1313 | Leon | 01 | 00 | Leon: Marvin!! Are you okay!? |
| 10 | 0x133A | Leon | 01 | 00 | Leon: That wound... Did those things / get you? How careless... |
| 11 | 0x1381 | Marvin | 07 | 00 | Marvin: The hall is no good...! Those / things broke through the main gate / and got inside! |
| 12 | 0x13E3 | Leon | 01 | 00 | Leon: Say what!? |
| 13 | 0x13FD | Marvin | 07 | 00 | Marvin: The entire place is overrun! / And if they're already all the way out / here... the surface might just be done for... |
| 14 | 0x1480 |  |  | 02 | It's too dangerous outside...We need to hurry and get that / elevator working. |
| 15 | 0x14D2 |  | 01 | 02 | There's a Flamethrower here.This weapon is not available / in this preview. |

## ROOM30F0 (STAGE3, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM30F1)

_kein Nachrichtenblock_

## ROOM4000 (STAGE4, 288632 B, msg_sec=0x1B8C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1BA0 |  |  | 02 | The door is locked. The text on the / panel reads:Door is under emergency lockdown. / An ID card is required to unlock it." |
| 1 | 0x1C1E |  | 01 | 02 | Will you use the Blue Master Keycard?  |
| 2 | 0x1C4D |  | 01 | 02 | You've used the Blue Master Keycard. |
| 3 | 0x1C79 | Leon | 01 | 00 | Leon: Ada, what is this place...? |
| 4 | 0x1CA4 | Ada | 02 | 00 | Ada: Just hurry up, okay?! / Let's get him to this room here! |
| 5 | 0x1CE9 | Leon | 01 | 00 | Leon: Go on ahead! / I'll take care of this! |
| 6 | 0x1D1D | Ada | 02 | 00 | Ada: Leon...! |
| 7 | 0x1D34 | Leon | 01 | 00 | Leon: Just get going already! |
| 8 | 0x1D5B | Leon | 01 | 00 | Leon: Take care of Marvin! |
| 9 | 0x1D7F |  |  | 02 | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. |

## ROOM4010 (STAGE4, 297112 B, msg_sec=0xFB8)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1010 | Ada | 02 | 00 | Ada: Leon, that wound... |
| 1 | 0x1032 | Leon | 01 | 00 | Leon: Yeah, we need to hurry. |
| 2 | 0x1059 | Leon | 01 | 00 | Leon: Ada... Just who are you, anyway? |
| 3 | 0x1089 | Leon | 01 | 00 | Leon: Don't tell me... |
| 4 | 0x10A9 | Ada | 02 | 00 | Ada: That's right. / I'm with Umbrella. |
| 5 | 0x10D8 | Ada | 02 | 00 | Ada: Leon... There's no time to talk / right now. |
| 6 | 0x1111 | Ada | 02 | 00 | Ada: If we don't hurry, he's going / to die. |
| 7 | 0x1145 | Leon | 01 | 00 | Leon: What should we do? |
| 8 | 0x1167 | Ada | 02 | 00 | Ada: It's a virus that makes people / turn into those things. If we can / get him the vaccine, it won't break out. |
| 9 | 0x11DF | Leon | 01 | 00 | Leon: And where's this vaccine? |
| 10 | 0x1208 | Ada | 02 | 00 | Ada: The vaccine itself isn't here, / but we can make one from a strain of / the virus. |
| 11 | 0x1265 | Ada | 02 | 00 | Ada: However... |
| 12 | 0x127E | Leon | 01 | 00 | Leon: Ada? What's wrong? |
| 13 | 0x12A0 | Ada | 02 | 00 | Ada: Leon, I can't help you. |
| 14 | 0x12C6 | Leon | 01 | 00 | Leon: Wait, what!? |
| 15 | 0x12E2 | Ada | 02 | 00 | Ada: There's something I have to do. |
| 16 | 0x1310 | Ada | 02 | 00 | Ada: You've helped me out a lot, but / I have to do this myself!  |
| 17 | 0x1359 | Leon | 01 | 00 | Leon: I'll... |
| 18 | 0x1370 | Leon | 01 | 00 | Leon: ...do it on my own if I have to! |
| 19 | 0x13A0 | Leon | 01 | 00 | Leon: So, please tell me! / What should I do!? |
| 20 | 0x13D6 | Ada | 02 | 00 | Ada: The viral strain should be stored / in this research center's ultra-low / temperature laboratory. |
| 21 | 0x1442 | Ada | 02 | 00 | Ada: As long as you can start up the / program for it, it should handle the / rest automatically. |
| 22 | 0x14A9 | Ada | 02 | 00 | Ada: However, the Virus Inhibitor is / very delicate. |
| 23 | 0x14E6 | Ada | 02 | 00 | Ada: If you don't handle it with / extreme care, it'll all be for naught. |
| 24 | 0x1537 | Leon | 01 | 00 | Leon: Well, I have to give it a go. |
| 25 | 0x1564 | Leon | 01 | 00 | Leon: Marvin... I'll save you! |
| 26 | 0x158C | Ada | 02 | 00 | Ada: Leon. When all this is over, / head to the underground shelter! |
| 27 | 0x15D8 | Ada | 02 | 00 | Ada: If you don't, you probably won't / survive. |
| 28 | 0x1610 | Ada | 02 | 00 | Ada: And keep in mind that this is an / Umbrella research facility. Be careful. |
| 29 | 0x1667 | Leon | 01 | 00 | Leon: Ada, wait! |
| 30 | 0x1681 | Leon | 01 | 00 | Leon: I probably won't survive...? |
| 31 | 0x16AD |  |  | 02 | I need to hurry up and get that / vaccine! |
| 32 | 0x16D9 | Leon | 01 | 00 | Leon: What's wrong, Marvin? Open your / eyes!! |
| 33 | 0x170F | Leon | 01 | 00 | Leon: Ada! |
| 34 | 0x1723 | Ada | 02 | 00 | Ada: Leon! What are you doing!? The / virus is on the loose here as well! |
| 35 | 0x1774 | Ada | 02 | 00 | Ada: Hurry to the shelter! |
| 36 | 0x1798 |  |  | 02 | It's a row of monitors.They're completely blank, maybe / they're not getting any power? |
| 37 | 0x17F3 |  |  | 02 |  / The desk is completely empty. |
| 38 | 0x1815 |  |  | 02 | This equipment seems to be unusable. |
| 39 | 0x183D |  |  | 02 | There's a memo here.To Michael: I left an ID card on the / right desk.Don't lose it again, okay? - Henry |
| 40 | 0x18AB |  |  | 02 |  / There's nothing useful in this locker. |
| 41 | 0x18D6 |  |  | 02 | This freezer isn't on. It smells pretty / bad. |
| 42 | 0x1906 |  |  | 02 | Itembox is not available in / this preview. |
| 43 | 0x1933 |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |

## ROOM4020 (STAGE4, 178440 B, msg_sec=0xB80, BYTEGLEICH mit ROOM4021)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B84 |  |  | 02 |  / There's a map of the entire area. |
| 1 | 0x0BAA |  |  | 02 |  / The button doesn't respond... |

## ROOM4030 (STAGE4, 142824 B, msg_sec=0x724)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x072E |  |  | 02 | Main Fuse" / It's a large circuit breaker system.It looks like someone has removed all / the fuses. |
| 1 | 0x0794 |  | 01 | 02 | Will you use the Spark Plug?  |
| 2 | 0x07BA |  |  | 02 | The fuses are all connected. I should / be able to get to some new places now. |
| 3 | 0x080A |  |  | 02 | Thanks to the footlights, I can just / barely see the ground. |
| 4 | 0x0849 |  |  | 02 | The lamps seem to flicker now and / then, but... |

## ROOM4040 (STAGE4, 156844 B, msg_sec=0x1294, BYTEGLEICH mit ROOM4041)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1296 | Left |  | 02 | Left: Emergency Staircase / Right: Private Rooms |

## ROOM4050 (STAGE4, 221128 B, msg_sec=0x2040, BYTEGLEICH mit ROOM4051)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2048 |  |  | 02 | There's nothing useful in this / locker. |
| 1 | 0x2072 |  |  | 02 | A researcher's bed. |
| 2 | 0x2089 |  |  | 02 | A cabinet full of books and / reference materials.Most appear to be about / chemistry and biology. |
| 3 | 0x20ED |  |  | 02 | There's a memo here.To David: I found your ID card / in the hallway.Please don't lose it again. / - Eric |

## ROOM4060 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM4061)

_kein Nachrichtenblock_

## ROOM4070 (STAGE4, 102320 B, msg_sec=0x135C, BYTEGLEICH mit ROOM4071)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x135E |  |  | 02 | The door won't open! |

## ROOM4080 (STAGE4, 145212 B, msg_sec=0xF00, BYTEGLEICH mit ROOM4081)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0F02 |  |  | 02 | Cell Cultivation Room"It's electronically locked. |

## ROOM4090 (STAGE4, 118896 B, msg_sec=0xB7C, BYTEGLEICH mit ROOM4091)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B8C |  | 01 | 02 | An ultra low temperature freezer / used to store viral samples.There is a hole to set the Enzyme  / in. |
| 1 | 0x0BF9 |  | 01 | 02 | The enzyme is already in place. / I need to operate the machine tosynthesize a sample of the T-Virus. |
| 2 | 0x0C66 |  |  | 02 | A control box for operating the / robotic arm.Will you use it?  |
| 3 | 0x0CAA |  | 01 | 02 | There's a slot for swiping a keycard. |
| 4 | 0x0CD7 |  |  | 02 | The keypanel has already been / activated. |
| 5 | 0x0D03 |  |  | 02 | A numerical keypanel.An ID card is required to operate / it. |
| 6 | 0x0D43 |  |  | 02 | A numerical keypanel.If I had the codes, I could use the / machine, but... |
| 7 | 0x0D91 |  |  | 02 | It looks like a computer for organizing / data files. |

## ROOM40A0 (STAGE4, 97496 B, msg_sec=0x111C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1138 |  |  | 02 | There's a switch here... Press it? /   |
| 1 | 0x1161 |  |  | 02 | There's a Flammable" mark on it. / Looks dangerous. |
| 2 | 0x1197 |  |  | 02 | The infirmary bed shows no signs of / recent use. |
| 3 | 0x11CA |  |  | 02 | It's securely locked. An ID card is / required to open it. |
| 4 | 0x1206 |  |  | 02 | Use the Research Center ID Card?  |
| 5 | 0x122C |  |  | 02 |  / It's a CT Scanner.The researchers here must be rather / unhealthy, because it looks like it's / seen a lot of use. |
| 6 | 0x12A3 |  |  | 02 |  / It's too dark to see what's on the shelf. |
| 7 | 0x12D1 |  |  | 02 |  / It's too dangerous to dig around with / my bare hands. |
| 8 | 0x130A |  |  | 02 |  / It's too dark to tell what it is. |
| 9 | 0x1330 |  |  | 02 |  / It's too dark to see anything. |
| 10 | 0x1353 |  |  | 02 | An operating table? It's too dark to / tell for sure. |
| 11 | 0x138A |  |  | 02 |  / There's what seems to be blood on it... |
| 12 | 0x13B6 |  |  | 02 | There is one U.S.S. armor / that should fit you.Will you equip it?  |
| 13 | 0x13FE |  |  | 02 | Will you unequip the armor?   |

## ROOM40B0 (STAGE4, 196460 B, msg_sec=0x7CC, BYTEGLEICH mit ROOM40B1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x07D4 |  |  | 02 | It looks like a large vat used to / store research specimens butthere's nothing inside... |
| 1 | 0x0831 |  |  | 02 | It looks like the remains of / a failed experiment... |
| 2 | 0x0868 |  |  | 02 | It's an operating table.There's what seems to be blood / on it... |
| 3 | 0x08AD |  |  | 02 | It's been broken from the inside. |

## ROOM40C0 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40C1)

_kein Nachrichtenblock_

## ROOM40D0 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40D1)

_kein Nachrichtenblock_

## ROOM40E0 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40E1)

_kein Nachrichtenblock_

## ROOM40F0 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40F1)

_kein Nachrichtenblock_

## ROOM5000 (STAGE5, 128700 B, msg_sec=0x944)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0946 |  |  | 02 | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. |

## ROOM5010 (STAGE5, 173880 B, msg_sec=0x50C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x051A |  |  | 02 | There's a bunch of footprints going / in circles.There doesn't seem to be anything / useful here. |
| 1 | 0x057D |  |  | 02 |  / This desk is very dusty.There's nothing useful on it. |
| 2 | 0x05B9 |  |  | 02 | It looks like this was used a little / while ago. |
| 3 | 0x05EC |  |  | 02 | There's a memo board behind a bunch / of machinery.It seems to be mostly notes written / down by the research staff. |
| 4 | 0x0662 |  |  | 02 | The trashcan is full of crumpled / up papers. |
| 5 | 0x0691 |  |  | 02 | Itembox is not available in / this preview. |
| 6 | 0x06BE |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |

## ROOM5020 (STAGE5, 136380 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM5030 (STAGE5, 158772 B, msg_sec=0x10E0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x10E6 |  |  | 02 | A map of the lab is available here.Will you file the laboratory map?  |
| 1 | 0x1132 |  |  | 02 | You've taken the laboratory map. |
| 2 | 0x1156 |  |  | 02 | It's a row of monitors.They appear to be used for showing / security footage, but they're onlydisplaying static... |

## ROOM5040 (STAGE5, 209404 B, msg_sec=0x1B74, BYTEGLEICH mit ROOM5041)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1B76 | Left |  | 02 | Left: P-4 Level Laboratory / Right: Computer Room |

## ROOM5050 (STAGE5, 195312 B, msg_sec=0x938, BYTEGLEICH mit ROOM5051)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0948 |  |  | 02 | Will you operate the / computer?  |
| 1 | 0x096C |  |  | 02 | Culture Experiment / RoomStaff Registry. |
| 2 | 0x0996 |  |  | 02 | Commencing with temporary / registration. |
| 3 | 0x09C1 |  |  | 02 | Register your fingerprint. |
| 4 | 0x09DF |  |  | 02 | Will you register?  |
| 5 | 0x09F7 |  |  | 02 | Registration complete. |
| 6 | 0x0A11 |  |  | 02 | Guest registration is valid / for 24 hours. |
| 7 | 0x0A3E |  |  | 02 | I don't need to use this anymore. |

## ROOM5060 (STAGE5, 270976 B, msg_sec=0x3284)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x32A2 |  |  | 02 | A locker used to hold containment suits.It's broken, and can't be opened. |
| 1 | 0x32F1 |  |  | 02 | There's nothing inside.It looks like it was emptied in a hurry. |
| 2 | 0x3336 |  |  | 02 | Current Biohazard Level:It seems to be at a safe level right now. |
| 3 | 0x337D |  |  |  | Security CabinetIt's locked and airtight. |
| 4 | 0x33AA |  |  | 02 | The security bolt has been removed.I hope nothing dangerous was inside. |
| 5 | 0x33F7 |  |  | 02 | Something must've happened inside / this building. These footprints look / like those of an injured person. |
| 6 | 0x3462 |  |  | 02 | The sink is very clean. |
| 7 | 0x347D |  |  | 02 | It's a draft chamber, for use if a gas / outbreak occurs.It's very clean. I can't tell if it's ever / been used. |
| 8 | 0x34EF |  |  | 02 | It looks like a virus injector. I don't / really need to use it. |
| 9 | 0x3531 |  |  | 02 | I can not investigate" this ROM yet. |
| 10 | 0x355A |  |  | 02 | A vaccine synthesis machine.A sample of the virus must / go inside.Will you push the switch?  |
| 11 | 0x35BE |  |  | 00 | Processing... |
| 12 | 0x35D1 |  |  | 00 | Processed! |
| 13 | 0x35E1 |  | 01 | 02 | You've taken the T-Vaccine. |
| 14 | 0x3604 |  |  | 02 | There is nothing left for me to do / here. |

## ROOM5070 (STAGE5, 204548 B, msg_sec=0xD8C, BYTEGLEICH mit ROOM5071)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D92 | Left |  | 02 | Left: Shelter 02 / Right: Shelter 01 |
| 1 | 0x0DB8 |  |  | 02 | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |
| 2 | 0x0E27 |  |  | 02 | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |

## ROOM5080 (STAGE5, 101096 B, msg_sec=0x878)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0880 |  |  | 02 | A high yield generator / that supplies power forthe inside facilities / and the train.Will you turn on the / switch?  |
| 1 | 0x08F8 |  |  | 02 | The emergency mode has / been activated.The power supply will be / cut temporarily.The emergency train will / be activated uponrestoration of the power." |
| 2 | 0x0996 |  |  | 02 | The door won't open until / the power is restored! |
| 3 | 0x09CA |  |  | 02 | The power already has been / supplied to the train! |

## ROOM5090 (STAGE5, 187708 B, msg_sec=0x135C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1360 |  | 01 | 00 | THANK YOU FOR PLAYING !!! |
| 1 | 0x1383 |  |  | 02 | I can't put my friends / in danger.I have no choice but to / stop that creature here! |

## ROOM50A0 (STAGE5, 131832 B, msg_sec=0x19CC, BYTEGLEICH mit ROOM50A1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x19D4 |  |  | 02 | It's the shutter control panel.I can't use it without an ID Card. |
| 1 | 0x1A1B |  |  | 02 | It looks the shutter on the right / leads to a hangar. |
| 2 | 0x1A53 |  |  | 02 | There's a maintenance panel here, but / it doesn't look like it has seen any use. |
| 3 | 0x1AA6 |  |  | 02 | Emergency Staircase"They probably wrote this so that / the researchers wouldn't get lost. |

## ROOM50B0 (STAGE5, 177296 B, msg_sec=0xB60)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B86 | Ada | 02 | 00 | Ada: We're all set. Annette is waiting / for us below, and I left Marvin with her. |
| 1 | 0x0BE0 | Ada | 02 | 00 | Ada: Your other friends are down there / with her too. |
| 2 | 0x0C1E | Leon | 01 | 00 | Leon: That's good. Who's Annette? |
| 3 | 0x0C49 | Ada | 02 | 00 | Ada: Dr. Birkin's wife and my boss. / She's going to help us with the train. |
| 4 | 0x0C9D | Leon | 01 | 00 | Leon: Who's Dr. Birkin, and where is he? |
| 5 | 0x0CCF | Ada | 02 | 00 | Ada: He... |
| 6 | 0x0CE3 | Ada | 02 | 00 | Ada: He's dead. Umbrella killed him. / That's why Annette is helping us. |
| 7 | 0x0D33 | Leon | 01 | 00 | Leon: What's with those? |
| 8 | 0x0D55 | Ada | 02 | 00 | Ada: They're part of Umbrella's / experiments here. |
| 9 | 0x0D90 | Ada | 02 | 00 | Ada: They were to be shipped out for / use as biological weapons. |
| 10 | 0x0DD9 | Leon | 01 | 00 | Leon: Ada, why did you set the reactor / to blow up? |
| 11 | 0x0E15 | Ada | 02 | 00 | Ada: So I can destroy this place. / You've seen what's in here. |
| 12 | 0x0E5C | Ada | 02 | 00 | Ada: You've seen what they can do. / I had to do it, understand? It's... |
| 13 | 0x0EAC | Ada | 02 | 00 | Ada: Aaaaaaaagggghhh! |
| 14 | 0x0ECB | Leon | 01 | 00 | Leon: Ada... |
| 15 | 0x0EE1 | Ada | 02 | 00 | Ada: I... I'm sorry, Leon. I'm not... / going... to make it... am I? |
| 16 | 0x0F2D | Ada | 02 | 00 | Ada: I... I so wanted... to escape... / you... will have... to do that... for me... |
| 17 | 0x0F88 |  |  | 02 | I can't leave Ada alone! |
| 18 | 0x0FA4 |  |  | 02 | She's dead. |

## ROOM50C0 (STAGE5, 105908 B, msg_sec=0xB7C, BYTEGLEICH mit ROOM50C1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B7E |  |  | 02 | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. |

## ROOM50D0 (STAGE5, 207788 B, msg_sec=0x14A0, BYTEGLEICH mit ROOM50D1)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x14A2 |  |  | 02 | There's a button here. Push it?  e         /            4   2  /           /      3  x  x                     |

## ROOM50E0 (STAGE5, 101604 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM50F0 (STAGE5, 100988 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM5100 (STAGE5, 117452 B, msg_sec=0x0, BYTEGLEICH mit ROOM5101)

_kein Nachrichtenblock_

## ROOM5110 (STAGE5, 220360 B, msg_sec=0x0, BYTEGLEICH mit ROOM5111)

_kein Nachrichtenblock_

## ROOM5120 (STAGE5, 165396 B, msg_sec=0x16F4, BYTEGLEICH mit ROOM5121)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x16F8 | Left |  | 02 | Left: P-4 Level Laboratory / Right: Computer Room |
| 1 | 0x172B |  |  | 02 | The door is locked because of / the fire! |

## ROOM5130 (STAGE5, 221440 B, msg_sec=0xBE0, BYTEGLEICH mit ROOM5131)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0BE6 | Left |  | 02 | Left: Shelter 02 / Right: Shelter 01 |
| 1 | 0x0C0C |  |  | 02 | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |
| 2 | 0x0C7B |  |  | 02 | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |

## ROOM5140 (STAGE5, 188848 B, msg_sec=0x1C48, BYTEGLEICH mit ROOM5141)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1C4A |  |  | 00 |  / It's open on this end. |

## ROOM5150 (STAGE5, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM5151)

_kein Nachrichtenblock_

## ROOM5160 (STAGE5, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM5161)

_kein Nachrichtenblock_

## ROOM5170 (STAGE5, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM5171)

_kein Nachrichtenblock_

## ROOM6000 (STAGE6, 237836 B, msg_sec=0x1230)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1240 | Annette | 02 | 00 | Annette: You must be that cop. / Your friends are already here. |
| 1 | 0x1287 | Annette | 02 | 00 | Annette: Where's Ada? |
| 2 | 0x12A6 | Annette | 02 | 00 | Annette: I see... |
| 3 | 0x12C1 | Annette | 02 | 00 | Annette: Come on. Let's join your / friends and get out of here. |
| 4 | 0x1309 | Annette | 02 | 00 | Annette: Let's see if we can get / this thing moving. |
| 5 | 0x1346 |  |  | 02 | A device to open the / tunnel gate.Will you turn on the / switch?  |
| 6 | 0x138B |  |  | 02 | It's locked from the other side. |
| 7 | 0x13AF |  |  | 02 | A device to open the / tunnel gate.The power is not supplied. |

## ROOM6010 (STAGE6, 145260 B, msg_sec=0x10C4, BYTEGLEICH mit ROOM6011)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x10C6 |  |  | 02 | You've opened the lock. |

## ROOM6020 (STAGE6, 201880 B, msg_sec=0xBDC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0BDE |  |  | 02 | Itembox is not available in / this preview. |

## ROOM6030 (STAGE6, 368996 B, msg_sec=0x1394)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x13BE | Leon | 01 | 00 | Leon: Hang on, Marvin. / We're getting out of here. |
| 1 | 0x13F9 | Leon | 01 | 00 | Leon: Where's Elza? |
| 2 | 0x1416 | Sherry | 06 | 00 | Sherry: She's out there fighting / the monsters! |
| 3 | 0x144E | John | 07 | 00 | John: Yeah! She's trying to keep / the outside clear so we can escape! |
| 4 | 0x149C | Leon | 01 | 00 | Leon: Why aren't you out there? |
| 5 | 0x14C5 | John | 07 | 00 | John: She told me to stay and keep / an eye on Marvin here. |
| 6 | 0x1508 | Leon | 01 | 00 | Leon: Where's Annette? |
| 7 | 0x1528 | John | 07 | 00 | John: Up front, trying to start / the train. |
| 8 | 0x155C | Leon | 01 | 00 | Leon: I see. Thanks. |
| 9 | 0x157A |  |  | 02 | Itembox is not available in / this preview. |
| 10 | 0x15A7 |  |  | 02 | I have to get this thing moving / first! |
| 11 | 0x15D1 | Annette | 02 | 00 | Annette: Where's that biker girl? |
| 12 | 0x15FC | Leon | 01 | 00 | Leon: Keeping the monsters at bay. / Can you start this thing? |
| 13 | 0x1642 | Annette | 02 | 00 | Annette: Certainly. |
| 14 | 0x165F | Annette | 02 | 00 | Annette: If I remember the startup / sequence correctly... |
| 15 | 0x16A1 | Leon | 01 | 00 | Leon: What's wrong? |
| 16 | 0x16BE | Annette | 02 | 00 | Annette: Not enough power! / The train's batteries are too low! |
| 17 | 0x1705 | Leon | 01 | 00 | Leon: I'll take care of it! |
| 18 | 0x172A | Annette | 02 | 00 | Annette: I'll be ready to start the / train when you do! |
| 19 | 0x176A | Leon | 01 | 00 | Leon: Good luck to both of us! |
| 20 | 0x1792 |  |  | 02 | You can't get off the / train right now. |

## ROOM6040 (STAGE6, 163048 B, msg_sec=0x0, BYTEGLEICH mit ROOM6041)

_kein Nachrichtenblock_

## ROOM6050 (STAGE6, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM6051)

_kein Nachrichtenblock_

## ROOM6060 (STAGE6, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM6061)

_kein Nachrichtenblock_

## ROOM6070 (STAGE6, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM6071)

_kein Nachrichtenblock_

