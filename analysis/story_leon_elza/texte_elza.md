# RE1.5 - alle RDT-Nachrichten, Elza-Variante (ROOMxxx1)

Erzeugt von `analysis/story_leon_elza/dump_texte.py` (Dekoder = `re15_port/tools/scd_walk_lib.py`, Glyphen = `engine/src/msg_common.c:175-199`).
Variante: Elza-RDT = Leon-RDT + 1 (Raumlader PSX.EXE @0x800397e4 `srl a0,a0,31` + @0x800397ec `addu a0,a0,v0`; Startraum Leon 0x17=ROOM1170 @0x8001d2a8, Elza 0x03=ROOM1031 @0x8001d324).
Spalten: ID = Nachrichten-Index (Message_on-Argument), Off = Datei-Byte-Offset des Textanfangs, Sp. = Sprecher aus dem Textpraefix "Name:", Farbe = Steuercode 05:xx (Sprecherfarbe), Modus = Steuercode 04:xx (00 = Dialog, 02 = Untersuchungstext). " / " = Zeilenumbruch (0x08).

## ROOM1001 (STAGE1, 177008 B, msg_sec=0xD08)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D0A |  |  | 02 | Nothing unusual. |

## ROOM1011 (STAGE1, 210440 B, msg_sec=0xE8C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0EBC | Officer | 02 | 00 | Officer: groans |
| 1 | 0x0ED7 | Elza | 01 | 00 | Elza: Hang in there! Are you the / only officer left in the building? |
| 2 | 0x0F24 | Officer | 02 | 00 | Officer: Who... who are you? |
| 3 | 0x0F4A | Elza | 01 | 00 | Elza: My name's Elza. / What's yours? |
| 4 | 0x0F77 | Officer | 02 | 00 | Officer: Elza, huh? I'm Roy. |
| 5 | 0x0F9D | Elza | 01 | 00 | Elza: That wound... did one of / those things get you? How horrible... |
| 6 | 0x0FEB | Roy | 02 | 00 | Roy: This precinct isn't safe anymore. |
| 7 | 0x101B | Roy | 02 | 00 | Roy: Those things broke through the / rear shutters and got inside. |
| 8 | 0x1066 | Roy | 02 | 00 | Roy: Those bastards at Umbrella / caused all of this... |
| 9 | 0x10A5 | Elza | 01 | 00 | Elza: ...What do you mean? |
| 10 | 0x10C9 | Roy | 02 | 00 | Roy: groans |
| 11 | 0x10E0 | Elza | 01 | 00 | Elza: Are you okay? |
| 12 | 0x10FD | Roy | 02 | 00 | Roy: We don't have time to talk / right now. |
| 13 | 0x1131 | Roy | 02 | 00 | Roy: You have to rescue the survivors / in the other rooms. |
| 14 | 0x1174 | Elza | 01 | 00 | Elza: But... |
| 15 | 0x118A | Roy | 02 | 00 | Roy: My friend John should still be / in the underground cell block. |
| 16 | 0x11D6 | Roy | 02 | 00 | Roy: Please, you have to help him... |
| 17 | 0x1204 | Elza | 01 | 00 | Elza: Roy, wake up! Please wake up! |
| 18 | 0x1231 |  | 01 | 02 | He has passed out from his injuries.He is holding a slip.The numbers 0513" are / printed on the slip. |
| 19 | 0x12A1 |  |  | 02 | He is holding something... |
| 20 | 0x12BF |  | 01 | 02 | You've taken the Prison key. |
| 21 | 0x12E3 |  |  | 02 | Several books of testimonies / and cases.Nothing useful. |
| 22 | 0x131F |  |  | 02 | An old typewriter.Why use it if there are / computers? |
| 23 | 0x1359 |  |  | 02 | A camera to record the / interrogations. |

## ROOM1021 (STAGE1, 274480 B, msg_sec=0x22A4)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x22C2 | John | 07 | 00 | John: No way, this can't be... Roy, this / is a joke, right? |
| 1 | 0x2306 | Roy | 02 | 00 | Roy: John... Help me... Please kill me. |
| 2 | 0x2337 | Elza | 01 | 00 | Elza: John, shoot! That thing isn't / Roy anymore! |
| 3 | 0x2371 | John | 07 | 00 | John: Roy, it's me... You do recognize / me, right? |
| 4 | 0x23AC | Elza | 01 | 00 | Elza: John, I couldn't call for help / using the radio. |
| 5 | 0x23EB | Elza | 01 | 00 | Elza: The building's electric power / is starting to fail... |
| 6 | 0x242F | John | 07 | 00 | John: What am I supposed to do alone? / I don't get it! |
| 7 | 0x246E | Elza | 01 | 00 | Elza: We're the only ones left. If we / don't do anything, we'll also be... |
| 8 | 0x24C1 | John | 07 | 00 | John: Please... I... I'd like to be / alone now... |
| 9 | 0x24FB |  |  | 02 | He's dead.Poor Roy... |
| 10 | 0x2516 |  |  | 02 | There are only reports among / other office objects.Nothing that can be of help. |
| 11 | 0x256A |  |  | 02 | It's empty. |
| 12 | 0x2579 |  |  | 02 | It's Lieutenant Branagh's desk. |
| 13 | 0x259C |  |  | 02 | There is something saved here. |
| 14 | 0x25BE |  |  | 02 | There are only reports of various / cases. |

## ROOM1031 (STAGE1, 219120 B, msg_sec=0x2AAC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2ADC |  |  | 02 | This should keep those things out...For now... |
| 1 | 0x2B12 |  |  | 02 | Guess it was only a matter of time... |
| 2 | 0x2B3B |  |  | 02 | The emergency shutters have / already been activated. |
| 3 | 0x2B72 |  |  | 02 | A payphone.The line appears to be down... |
| 4 | 0x2BA3 |  |  | 02 | Some sort of reference chart for / identifying emergency severity." |
| 5 | 0x2BE9 |  |  | 02 | This emergency severity" chart / gives more confusion than it prevents. |
| 6 | 0x2C31 |  |  | 02 | There's nothing useful on the counter. |
| 7 | 0x2C5B |  |  | 02 | Seems like the crash broke the axle.Poor thing... |
| 8 | 0x2C94 |  |  | 02 | Seems like whoever rode this crashed / it through the front gate.Wonder if they're ok... |
| 9 | 0x2CF2 |  |  | 02 | It's too dark to see if there's / anything inside.Would you like to reach inside? |
| 10 | 0x2D49 |  | 01 | 00 | Found Green Medicine. / Take it? |
| 11 | 0x2D6F |  |  | 02 | Better not dig through other's trash... |
| 12 | 0x2D9A |  |  | 02 | There's glass from the front door / all over the floor. |
| 13 | 0x2DD3 |  |  | 02 | Desks for filling out inquiry forms. |
| 14 | 0x2DFB |  |  | 02 | Seems like even the emergency / broadcast has gone offline... |
| 15 | 0x2E3A |  |  | 02 | A map of dowtown Raccoon City. / Population est. 100,000 or more... |
| 16 | 0x2E7F |  |  | 02 | An emergency alarm.The red light works, but the alarm / bell appears to be malfunctioning. |
| 17 | 0x2EDF |  |  | 02 | There are some words carved here... / I    C  A  N  '  T    E  S  C  A  P  E  !  !  !"The door is locked, I can't open it. |
| 18 | 0x2F60 | Elza | 01 | 00 | Elza: That was close, this must / contain them for some time... |
| 19 | 0x2FA7 | Elza | 01 | 00 | Elza: Hey, is anyone here?! |
| 20 | 0x2FCC |  |  | 00 | We barricaded ourselves / inside the police station... |
| 21 | 0x3006 |  |  | 00 | Where we thought it would / be safe. |
| 22 | 0x302E |  |  | 00 | It was no use, and so began a / life and death struggle... |
| 23 | 0x306C |  |  | 00 | To escape. |

## ROOM1041 (STAGE1, 213700 B, msg_sec=0x2248)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2250 |  |  | 02 | It's a shutter switch. / Will you push it?  |
| 1 | 0x227E |  |  | 02 | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. |
| 2 | 0x22DC | Elza | 01 | 00 | Elza: What was that!? |
| 3 | 0x22FB |  |  | 00 | gunshot |

## ROOM1051 (STAGE1, 243976 B, msg_sec=0xDE0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0DEC |  |  | 02 | It's a shutter switch. / Will you push it?  |
| 1 | 0x0E1A |  |  | 02 | Various posters of missing / persons.It is better not to think about / it... |
| 2 | 0x0E68 |  |  | 02 | I need a fuse to run the shutter. |
| 3 | 0x0E8D |  |  | 02 | Just scattered papers and books.Nothing relevant. |
| 4 | 0x0EC4 |  |  | 02 | A blackboard full of reports of riots / and cannibalism at points in the city. |
| 5 | 0x0F14 |  |  | 02 | He's got a big bite on his neck, he / didn't give him a chance to avoid it.The weapon can be useful.Sorry, I'll just borrow this... |

## ROOM1061 (STAGE1, 149428 B, msg_sec=0xE38, BYTEGLEICH mit ROOM1060)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0E3A |  |  | 02 | Only the lights of the / buildings are visible. |

## ROOM1071 (STAGE1, 153328 B, msg_sec=0x16DC, BYTEGLEICH mit ROOM1070)

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

## ROOM1081 (STAGE1, 177696 B, msg_sec=0x0, BYTEGLEICH mit ROOM1080)

_kein Nachrichtenblock_

## ROOM1091 (STAGE1, 282404 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM10A1 (STAGE1, 101164 B, msg_sec=0x0, BYTEGLEICH mit ROOM10A0)

_kein Nachrichtenblock_

## ROOM10B1 (STAGE1, 139080 B, msg_sec=0x1AB8)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1AC4 | Elza | 01 | 00 | Elza: John, wait! |
| 1 | 0x1ADF | Elza | 01 | 00 | Elza: There's still a little girl / trapped in there! |
| 2 | 0x1B1C | John | 07 | 00 | John: Say what!? |
| 3 | 0x1B36 | Elza | 01 | 00 | Elza: I found her in the basement / garage! We can't just leave her behind! |
| 4 | 0x1B89 | John | 07 | 00 | John: The garage in the basement, / huh...? |
| 5 | 0x1BBC | John | 07 | 00 | John: If that's the case, we can go / through the sewer tunnels. Follow me. |

## ROOM10C1 (STAGE1, 87376 B, msg_sec=0x1044, BYTEGLEICH mit ROOM10C0)

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

## ROOM10D1 (STAGE1, 240364 B, msg_sec=0x1AAC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1AC4 |  | 01 | 02 | Enter the first number. |
| 1 | 0x1AE3 |  | 01 | 02 | Enter the second number. |
| 2 | 0x1B03 |  | 01 | 02 | Enter the third number. |
| 3 | 0x1B22 |  | 01 | 02 | Enter the fourth number. |
| 4 | 0x1B42 |  |  | 02 | Wrong code, try again. |
| 5 | 0x1B5C |  |  | 02 | You've opened the lock. |
| 6 | 0x1B77 |  |  | 02 | Communication Room"It's electronically / locked.There's a card reader / on the right. |
| 7 | 0x1BD1 |  | 01 | 02 | A card reader.You have to use the Blue Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  |
| 8 | 0x1C5A |  | 01 | 02 | You have not the Blue Keycard / to operate it. |
| 9 | 0x1C8E |  | 01 | 02 | You've used the Blue Keycard. |
| 10 | 0x1CB3 | Left |  | 02 | Left: First Aid Room, Communication / Room. Right: Archive Room. |
| 11 | 0x1CF5 |  |  | 02 | It's a shutter switch. / Will you push it?         /   ,      3    |

## ROOM10E1 (STAGE1, 157632 B, msg_sec=0xE2C, BYTEGLEICH mit ROOM10E0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0E36 |  |  | 02 | It's the body of a doctor.He has no vital signs. |
| 1 | 0x0E6C |  |  | 02 | An old typewriter.Why use it if there are / computers? |
| 2 | 0x0EA6 |  |  | 02 | September 1998. |
| 3 | 0x0EB9 |  |  | 02 | Various medicines and bandages. |
| 4 | 0x0EDC |  |  | 02 | It's a photo of the Arklay / Mountains. |

## ROOM10F1 (STAGE1, 225092 B, msg_sec=0x1168, BYTEGLEICH mit ROOM10F0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1174 |  |  | 02 | A large communications device.It's out of commission.I can't send any messages from / here. |
| 1 | 0x11D5 |  |  | 02 | Nothing suspicious nor / out of the ordinary. |
| 2 | 0x1204 |  |  | 02 | There isnt anything / useful on this shelf. |
| 3 | 0x1231 |  |  | 02 | There are various devices.This must be where they / gathered information. |
| 4 | 0x127E |  |  | 02 | Several files from past / cases.Nothing useful here. |
| 5 | 0x12B6 |  |  | 02 | It's a small office.It's locked. |

## ROOM1101 (STAGE1, 181704 B, msg_sec=0xD9C, BYTEGLEICH mit ROOM1100)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0DA8 |  |  | 02 | It's electronically locked. |
| 1 | 0x0DC7 |  |  | 02 | It's a control panel.A type of voice device is required / to open the lock. |
| 2 | 0x0E16 |  |  | 02 | You've opened the lock. |
| 3 | 0x0E31 |  | 01 | 02 | Will you use the Minidisc Player w/ Disc?  |
| 4 | 0x0E64 |  | 01 | 02 | You've used the Minidisc Player w/ Disc. |
| 5 | 0x0E94 |  |  | 02 | I don't need to use this anymore. |

## ROOM1111 (STAGE1, 196360 B, msg_sec=0xD54, BYTEGLEICH mit ROOM1110)

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

## ROOM1121 (STAGE1, 86900 B, msg_sec=0xDB4, BYTEGLEICH mit ROOM1120)

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

## ROOM1131 (STAGE1, 198104 B, msg_sec=0xB08)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B14 |  |  | 02 | It's a shutter switch. / Will you push it?  |
| 1 | 0x0B42 |  |  | 02 | It's not necessary to go back. |
| 2 | 0x0B64 |  |  | 02 | A photo of the members of / S.T.A.R.S. |
| 3 | 0x0B8C |  |  | 02 | An emblem of the Raccoon / S.W.A.T Team. |
| 4 | 0x0BB6 |  |  | 02 | A painting of a man with a hook, / seeing it makes me uncomfortable. |
| 5 | 0x0BFC |  |  | 02 | A shelf with trophies.Among them is the award for best / shooter, Chris Redfield. |

## ROOM1141 (STAGE1, 194568 B, msg_sec=0x10E4)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1102 | Elza | 01 | 00 | Elza: John, wait! |
| 1 | 0x111D | John | 07 | 00 | John: Elza! sorry, I'm glad to see / you're still alive... |
| 2 | 0x115F | Elza | 01 | 00 | Elza: Same here... are you okay? / what's happened? |
| 3 | 0x119A | John | 07 | 00 | John: Somehow... Elza, listen to me! |
| 4 | 0x11C8 | Elza | 01 | 00 | Elza: What is wrong? |
| 5 | 0x11E6 | John | 07 | 00 | John: Don't go to the rooftop! / ...there's something...a somewhat / dangerous monster! |
| 6 | 0x1243 | Elza | 01 | 00 | Elza: What was that?! |
| 7 | 0x1262 | John | 07 | 00 | John: Shit! that is what I was / talking about! |
| 8 | 0x1299 | Elza | 01 | 00 | Elza: what the hell is that?! |
| 9 | 0x12C0 | John | 07 | 00 | John: Looks like some kind of gorilla, / that would explain how they managed / to enter through the windows! |
| 10 | 0x1332 | Elza | 01 | 00 | Elza: John! what will we do now?! |
| 11 | 0x135D | John | 07 | 00 | John: Elza, I'll try to distract them, / while you try to get out of here, hurry! |
| 12 | 0x13B6 |  |  | 02 | You've opened the lock. |
| 13 | 0x13D1 |  |  | 02 | A beautiful painting.There's a switch below. / Will you push it?  |
| 14 | 0x1417 |  |  | 02 | It's a big table. |

## ROOM1151 (STAGE1, 202392 B, msg_sec=0x10EC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x10F4 |  |  | 02 | There's a small sign in one corner. / It reads:Raccoon City 21st Century, / Hillman-Grayson Architects LLC." |
| 1 | 0x1163 |  |  | 02 | It's a phone.You can save your progress / with this.Save is not available in / this preview. |
| 2 | 0x11C3 | Irons | 02 | 00 | Irons: I'll be fine. Just worry about / yourself for now. |
| 3 | 0x1204 |  |  | 02 | Itembox is not available in / this preview. |

## ROOM1161 (STAGE1, 69944 B, msg_sec=0x0, BYTEGLEICH mit ROOM1160)

_kein Nachrichtenblock_

## ROOM1171 (STAGE1, 161300 B, msg_sec=0x17A4)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x17AE | John | 07 | 00 | John: Elza, hurry up! |
| 1 | 0x17CD | John | 07 | 00 | John: We need to get this thing / off of here! Help me out! |
| 2 | 0x1810 | John | 07 | 00 | John: C'mon, give me a hand! |
| 3 | 0x1836 | John | 07 | 00 | John: Alright, let's get down from here! |
| 4 | 0x1868 |  |  | 02 | It's locked from the other side. |

## ROOM1181 (STAGE1, 91848 B, msg_sec=0xB60, BYTEGLEICH mit ROOM1180)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B66 |  |  | 02 | Weapon Storage"It's electronically / locked.There's a card reader / on the right. |
| 1 | 0x0BBC |  |  | 02 | A card reader.It seems that it will not / work if power is not restored. |
| 2 | 0x0C08 |  |  | 02 |  / ... |

## ROOM1191 (STAGE1, 330532 B, msg_sec=0x2E08)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2E14 |  |  | 02 | There's a switch here. Push it?  |
| 1 | 0x2E39 |  |  | 02 | There's a driver here. Take it?  |
| 2 | 0x2E5E |  |  | 02 | I have nothing else to do / here. |
| 3 | 0x2E81 |  |  | 02 | No response...The power is not supplied. |
| 4 | 0x2EAF |  |  | 02 | There is one R.P.D. armor / that should fit you.Will you equip it?  |
| 5 | 0x2EF7 |  |  | 02 | Will you unequip the armor? e         /   4      4   / |

## ROOM11A1 (STAGE1, 253632 B, msg_sec=0x13FC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1402 |  |  | 02 | There's a shutter switch here. Press it?  |
| 1 | 0x1430 |  |  | 02 | It's locked. |
| 2 | 0x1440 |  |  | 02 | It's locked from the other side. |

## ROOM11B1 (STAGE1, 253312 B, msg_sec=0x1544)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1562 | Elza | 01 | 00 | Elza: Hey! What are you doing in a / place like this? |
| 1 | 0x159F | Elza | 01 | 00 | Elza: You got trapped in here, right? |
| 2 | 0x15CE | Elza | 01 | 00 | Elza: What's your name? |
| 3 | 0x15EF | Girl | 06 | 00 | Girl: ...Sherry. |
| 4 | 0x1609 | Elza | 01 | 00 | Elza: Sherry, huh? That's a cute name. / I'm Elza. |
| 5 | 0x1643 | Sherry | 06 | 00 | Sherry: Elza... |
| 6 | 0x165C | Elza | 01 | 00 | Elza: That's right! Okay Sherry, listen / up... |
| 7 | 0x1693 | Elza | 01 | 00 | Elza: Just wait there. / I'll come rescue you in a bit. |
| 8 | 0x16D2 | Elza | 01 | 00 | Elza: So just hold tight and wait for / me quietly in that car over there. Okay? |
| 9 | 0x172A |  |  | 02 | It doesn't seem like you could move / this on your own. |
| 10 | 0x1763 | Elza | 01 | 00 | Elza: Sherry!? |
| 11 | 0x177B | Elza | 01 | 00 | Elza: Where has she gone!? |
| 12 | 0x179F |  |  | 02 | The door is locked. |
| 13 | 0x17B6 |  |  | 02 | The shutter is tightly sealed in place. |
| 14 | 0x17E1 |  |  | 02 | It's too dark to see anything...I must find way to activate the energy. |

## ROOM11C1 (STAGE1, 108020 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM11D1 (STAGE1, 130140 B, msg_sec=0x18FC, BYTEGLEICH mit ROOM11D0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1906 |  |  | 02 | A panel to control the cells of / the dogs.It does not work. |
| 1 | 0x1946 |  |  | 02 | The road is blocked and the knob / is broken. |
| 2 | 0x1975 |  |  | 02 | Boxes and dog food.Nothing useful. |
| 3 | 0x199D |  |  | 02 | Raccoon Police wins the award for / the best K-9 unit in the Midwest. |
| 4 | 0x19E4 |  |  | 02 | -Warning-Sewer access area. / Authorized personal only. |

## ROOM11E1 (STAGE1, 209684 B, msg_sec=0x210C, BYTEGLEICH mit ROOM11E0)

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

## ROOM11F1 (STAGE1, 152588 B, msg_sec=0x1818, BYTEGLEICH mit ROOM11F0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1820 |  |  | 02 | -Reserve Power / Control Panel-Power can be routed to / the areas withinsufficient power using / these ten switches.Using all the power may / cause a short andcut the power supply. |
| 1 | 0x18D8 |  |  | 02 | Will you push them?  |
| 2 | 0x18F1 |  | 01 | 02 | Power supply OK. |
| 3 | 0x1909 |  | 02 | 02 | Power supply incorrect. |

## ROOM1201 (STAGE1, 167132 B, msg_sec=0x0, BYTEGLEICH mit ROOM1200)

_kein Nachrichtenblock_

## ROOM1211 (STAGE1, 254616 B, msg_sec=0x2348)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x235E | Elza | 01 | 00 | Elza: Hey! You must be John right? / Get up now! |
| 1 | 0x2396 | John | 07 | 00 | John: What do you want? / I'm trying to sleep here! |
| 2 | 0x23D1 | John | 07 | 00 | John: Ughh... my head is killing me. |
| 3 | 0x23FF | John | 07 | 00 | John: Who the hell are you anyway? |
| 4 | 0x242B | Elza | 01 | 00 | Elza: My name's Elza. / Roy sent me to get you out of here. |
| 5 | 0x246E | Elza | 01 | 00 | Elza: Listen. We're in serious trouble. / The whole station is crawling with / zombies! |
| 6 | 0x24CB | John | 07 | 00 | John: Say what!? |
| 7 | 0x24E5 | Elza | 01 | 00 | Elza: Roy was injured by one of them. / I left him in the Interrogation Room, but... |
| 8 | 0x2541 | John | 07 | 00 | John: The interrogation room? / Alright. I'm going there. |
| 9 | 0x2582 | Elza | 01 | 00 | Elza: John. Wait! |
| 10 | 0x259D |  |  | 02 | It's locked.It seems that there is someone / inside... |

## ROOM1221 (STAGE1, 140456 B, msg_sec=0x0, BYTEGLEICH mit ROOM1220)

_kein Nachrichtenblock_

## ROOM1231 (STAGE1, 166268 B, msg_sec=0x14F4, BYTEGLEICH mit ROOM1230)

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

## ROOM1241 (STAGE1, 163748 B, msg_sec=0x630)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x063C |  |  | 00 | A bizarre incident occurred in the / outskirts of an American suburb called / Raccoon City. |
| 1 | 0x0699 |  |  | 00 | It was later revealed that the terrible / disaster had been caused by the T-Virus, |
| 2 | 0x06EF |  |  | 00 | a mutagenic toxin created by the / international corporation Umbrella / incorporated. |
| 3 | 0x0746 |  |  | 00 | The Raccoon police department special / S.T.A.R.S Unit began an immediate / investigation. |
| 4 | 0x07A2 |  |  | 00 | The case was apparently closed thanks / to the efforts of S.T.A.R.S members / Chris Redfield and Jill Valentine. |
| 5 | 0x0814 |  |  | 00 | But the umbrella corporation's / experiments were far from finished. |

## ROOM1251 (STAGE1, 140056 B, msg_sec=0x0, BYTEGLEICH mit ROOM1250)

_kein Nachrichtenblock_

## ROOM1261 (STAGE1, 287712 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM1271 (STAGE1, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM1270)

_kein Nachrichtenblock_

## ROOM2001 (STAGE2, 360660 B, msg_sec=0x1DEC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1E0A | John | 07 | 00 | John: Elza, over here! |
| 1 | 0x1E2A | John | 07 | 00 | John: Through this staircase you can / access the basement garage. |
| 2 | 0x1E74 | Elza | 01 | 00 | Elza: Okay, I'll go for her. |
| 3 | 0x1E9A | John | 07 | 00 | John: Meanwhile I will take a look / around, maybe she is down here too. |
| 4 | 0x1EEA | Elza | 01 | 00 | Elza: Good idea, but be careful. |
| 5 | 0x1F14 | John | 07 | 00 | John: You too. |
| 6 | 0x1F2C | Elza | 01 | 00 | Elza: Sherry... thank god you're / fine! I was so worried! |
| 7 | 0x1F6E | Elza | 01 | 00 | Elza: What happened? why didn't you / wait for me? |
| 8 | 0x1FA8 | Sherry | 06 | 00 | Sherry: It was because of my daddy, / I've heard him call me! |
| 9 | 0x1FED | Sherry | 06 | 00 | Sherry: I went down here to find him, / but I couldn't... |
| 10 | 0x202E | Elza | 01 | 00 | Elza: I understand, don't worry honey, / if he's here we'll find him, okay? |
| 11 | 0x2081 | Elza | 01 | 00 | Elza: My, that's a nice pocket watch! |
| 12 | 0x20B0 | Sherry | 06 | 00 | Sherry: My mom gave it to me, she says / it's for good luck. |
| 13 | 0x20F4 | Elza | 01 | 00 | Elza: I see, I guess your mom is right. |
| 14 | 0x2125 | Elza | 01 | 00 | Elza: Come on Sherry, let's look for / John and get out of here! |

## ROOM2011 (STAGE2, 316856 B, msg_sec=0x91C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0926 |  | 01 | 02 | Will you take the Crank?  |
| 1 | 0x0948 |  | 01 | 02 | You've taken the Crank. |
| 2 | 0x0967 |  |  | 02 | Itembox is not available in / this preview. |
| 3 | 0x0994 |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |
| 4 | 0x09F7 |  |  | 02 | An old typewriter.Why use it if there are / computers? |

## ROOM2021 (STAGE2, 322276 B, msg_sec=0xB44)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B4C |  |  | 02 | It's a square hole. |
| 1 | 0x0B63 |  | 01 | 02 | Will you use the Crank?  |
| 2 | 0x0B84 |  |  | 02 | You've opened the lock. |
| 3 | 0x0B9F |  |  | 02 | It's locked. |

## ROOM2031 (STAGE2, 282076 B, msg_sec=0x200C, BYTEGLEICH mit ROOM2030)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2014 |  |  | 02 | It's locked from the other side. |
| 1 | 0x2038 |  |  | 02 | The elevator is not responding...Maybe it should be activated / from somewhere. |
| 2 | 0x208B |  | 01 | 02 | Will you take the Fuse?  |
| 3 | 0x20AC |  | 01 | 02 | You've taken the Fuse. |

## ROOM2041 (STAGE2, 255800 B, msg_sec=0x1750, BYTEGLEICH mit ROOM2040)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x175C |  | 01 | 02 | Notice of damage door:While doing maintenance and / construction work on the Raccoonsewage system we found a problem / with a door, it seems that itrusted due to water leaks, we tried / everything and nothing works!Unless we use explosives or something / heavy it doesn't seem I'm going togive in, I'll have to ask the manager / for options for this problem.Bill Johnson, / Knights Construction |
| 1 | 0x18F7 |  |  | 02 | An old typewriter, perhaps someone will / find it useful even in these times. |
| 2 | 0x1946 |  |  | 02 | This old console is useless. |
| 3 | 0x1966 |  |  | 02 | The body of a sewer worker... / He has something in his hands. |
| 4 | 0x19A6 |  | 02 | 02 | Bomb activated, I have 10 seconds / before it detonates! |
| 5 | 0x19E4 |  | 01 | 02 | Will you use the Time Bomb?    e         /            4   2  /           /      3  x  x                     |

## ROOM2051 (STAGE2, 342712 B, msg_sec=0x2440, BYTEGLEICH mit ROOM2050)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2446 |  |  | 02 | A lever to fill and drain water. / Will you move it?  |
| 1 | 0x247E |  |  | 02 | The water is blocking the / entrance. |
| 2 | 0x24A5 |  |  | 02 | I have nothing else to do / here. |

## ROOM2061 (STAGE2, 272112 B, msg_sec=0x1758, BYTEGLEICH mit ROOM2060)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1766 |  |  | 02 | The generator does not appear to / be on.There are four switches...Will you push them?  |
| 1 | 0x17C4 |  |  | 02 | I need to insert the missing / fuse before I can operate this. |
| 2 | 0x1804 |  |  | 02 | It seems that the generator is / working now. |
| 3 | 0x1833 |  |  | 02 | One of the fuses is missing... |
| 4 | 0x1855 |  | 01 | 02 | Will you use the Fuse?  |
| 5 | 0x1875 |  | 01 | 02 | You've used the Fuse. |
| 6 | 0x1892 |  |  | 02 | The fuse is in place. |

## ROOM2071 (STAGE2, 174036 B, msg_sec=0xD04, BYTEGLEICH mit ROOM2070)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D06 |  |  | 02 | You've opened the lock. |

## ROOM2081 (STAGE2, 173608 B, msg_sec=0xCF4, BYTEGLEICH mit ROOM2080)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0CF8 |  |  | 02 | From here maybe I can control the / water level.Will you operate the computer?  |
| 1 | 0x0D4C |  |  | 02 | The water level is very high...I can't get down to the floor below. |

## ROOM2091 (STAGE2, 311024 B, msg_sec=0x0, BYTEGLEICH mit ROOM2090)

_kein Nachrichtenblock_

## ROOM20A1 (STAGE2, 300176 B, msg_sec=0x2008, BYTEGLEICH mit ROOM20A0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x200E |  |  | 02 | There's a shutter switch here. Press it?  |
| 1 | 0x203C |  |  | 02 | It's locked. |
| 2 | 0x204C |  |  | 02 | You've opened the lock. |

## ROOM20B1 (STAGE2, 201460 B, msg_sec=0x0, BYTEGLEICH mit ROOM20B0)

_kein Nachrichtenblock_

## ROOM20C1 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20C0)

_kein Nachrichtenblock_

## ROOM20D1 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20D0)

_kein Nachrichtenblock_

## ROOM20E1 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20E0)

_kein Nachrichtenblock_

## ROOM20F1 (STAGE2, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM20F0)

_kein Nachrichtenblock_

## ROOM3001 (STAGE3, 159180 B, msg_sec=0x1834)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1848 |  |  | 02 | A ladder leads down the manhole. / Descend it?  |
| 1 | 0x187A |  |  | 02 | My strength is running out. / I've got to find the others! |
| 2 | 0x18B6 | Elza | 01 | 00 | Elza: !! |
| 3 | 0x18C8 | Elza | 01 | 00 | Elza: Wh-what is this place...? |
| 4 | 0x18F1 | John | 07 | 00 | John: These corpses are definitively / police officers. |
| 5 | 0x1930 | John | 07 | 00 | John: This used to be a chemical plant, / but it was supposedly not in use anymore... |
| 6 | 0x198D | Elza | 01 | 00 | Elza: What happened here? |
| 7 | 0x19B0 | Sherry | 06 | 00 | Sherry: Elza... |
| 8 | 0x19C9 | Elza | 01 | 00 | Elza: It'll be okay, just stick close to / me. |
| 9 | 0x19FF | John | 07 | 00 | John: Let's go. We should find / somewhere to hide for now. |

## ROOM3011 (STAGE3, 167044 B, msg_sec=0x234C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2350 |  |  | 02 | It's locked.An ID card is required to open it. |
| 1 | 0x2384 |  | 01 | 02 | You've used the Green Keycard. |

## ROOM3021 (STAGE3, 118140 B, msg_sec=0xBFC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0C06 |  |  | 02 | It's locked from the other side. |
| 1 | 0x0C2A |  |  | 02 | The car is locked, I can't get inside. |
| 2 | 0x0C54 |  |  | 02 | It's an open window. / I could boost Sherry in there.Not a good idea.Too dark, and there may be zombies / inside. |
| 3 | 0x0CC9 |  |  | 02 | The panel is smashed and doesn't work. |
| 4 | 0x0CF3 |  |  | 02 |  / It's locked. |

## ROOM3031 (STAGE3, 81004 B, msg_sec=0x0, BYTEGLEICH mit ROOM3030)

_kein Nachrichtenblock_

## ROOM3041 (STAGE3, 152352 B, msg_sec=0x15CC, BYTEGLEICH mit ROOM3040)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x15D4 |  |  | 02 | You've opened the lock. |
| 1 | 0x15EF |  |  | 02 | There's a button here. Push it?  |
| 2 | 0x1614 |  |  | 02 | The boxes are blocking the way. / I must find a way to move them. |
| 3 | 0x1657 |  |  | 02 | The control panel is not / responding...Maybe it should be activated / from somewhere. |

## ROOM3051 (STAGE3, 262652 B, msg_sec=0x2638, BYTEGLEICH mit ROOM3050)

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

## ROOM3061 (STAGE3, 144536 B, msg_sec=0x13D0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x13E6 |  |  | 02 |  / It's locked. |
| 1 | 0x13F7 | Elza | 01 | 00 | Elza: Sherry, come back! |
| 2 | 0x1419 | Elza | 01 | 00 | Elza: What's wrong? Why did you start / running...? |
| 3 | 0x1454 | Sherry | 06 | 00 | Sherry: Elza... I know a secret / way in! |
| 4 | 0x1485 | Elza | 01 | 00 | Elza: A secret entrance? |
| 5 | 0x14A7 | Sherry | 06 | 00 | Sherry: It's here, I can get inside / through this hole! |
| 6 | 0x14E7 | Elza | 01 | 00 | Elza: Sherry, wait! |
| 7 | 0x1504 | Sherry | 06 | 00 | Sherry: I'll unlock the back door from / the other side! |
| 8 | 0x1544 | Sherry | 06 | 00 | Sherry: Just wait by the door, okay? |
| 9 | 0x1572 | John | 07 | 00 | John: H-hey, you brat! That's dangerous, / get back here! |
| 10 | 0x15B3 |  |  | 02 | It's locked from the other side. |

## ROOM3071 (STAGE3, 243908 B, msg_sec=0x3E2C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x3E4C | Elza | 01 | 00 | Elza: I know it has to be somewhere...! |
| 1 | 0x3E7D | John | 07 | 00 | John: Okay, Elza! I've got your back! |
| 2 | 0x3EAC | Elza | 01 | 00 | Elza: This lift, this is where it / came from. |
| 3 | 0x3EE2 | John | 07 | 00 | John: ...! That's a damn huge lift... |
| 4 | 0x3F11 | John | 07 | 00 | John: They probably used it to move / some heavy equipment. |
| 5 | 0x3F54 | John | 07 | 00 | John: ...Can we get it to work? |
| 6 | 0x3F7D | Elza | 01 | 00 | Elza: No... It looks like it can be / operated only from the inside... |
| 7 | 0x3FCB | Sherry | 06 | 00 | Sherry: I know how to make it work! |
| 8 | 0x3FF8 | Elza | 01 | 00 | Elza: Sherry? What are you talking / about...? |
| 9 | 0x402E | John | 07 | 00 | John: Save the discussion for later! / Those things are trying to get in here...! |
| 10 | 0x4087 | John | 07 | 00 | John: Let's get this lift moving and go / down there! |
| 11 | 0x40C4 | Elza | 01 | 00 | Elza: Are you insane!? |
| 12 | 0x40E4 | Elza | 01 | 00 | Elza: I told you that weird monster / came from here! |
| 13 | 0x4121 | John | 07 | 00 | John: If we stay here, we'll be eaten / for sure! |
| 14 | 0x415A | John | 07 | 00 | John: We've got no choice but to take / our chances down there! Let's go! |
| 15 | 0x41AB |  |  | 02 | The power seems to be off... / If the control box worked,the elevator might move, but... |

## ROOM3081 (STAGE3, 168560 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM3091 (STAGE3, 272500 B, msg_sec=0x1E74)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1EB6 |  |  | 02 |  / The power is already on... |
| 1 | 0x1ED5 |  |  | 02 | The power is off... Has a circuit breaker / tripped? |
| 2 | 0x1F0B |  |  | 02 |  / This breaker switch is flipped down... |
| 3 | 0x1F36 |  |  | 02 |  / I can't reach the breaker switch... |
| 4 | 0x1F5E |  |  | 02 |  / This breaker switch is flipped down... |
| 5 | 0x1F89 |  |  | 02 | This breaker switch is flipped up... Flip / it back down?  |
| 6 | 0x1FC6 |  |  | 02 | The door is latched shut and won't open.I'd need a key to open this... |
| 7 | 0x2012 |  |  | 02 | The door is latched shut and won't open. |
| 8 | 0x203E |  |  | 02 |  / I need to hurry up and save the others... |
| 9 | 0x206C | John | 07 | 00 | John: That was a close shave! / I almost got my ass handed to me! |
| 10 | 0x20B5 | John | 07 | 00 | John: Elza, are you okay? |
| 11 | 0x20D8 | Elza | 01 | 00 | Elza: Yes, somehow! I thought I was / done for! |
| 12 | 0x210F | John | 07 | 00 | John: Hey, you really saved my butt / there! |
| 13 | 0x2143 | Elza | 01 | 00 | Elza: You really did save us, / thank you! |
| 14 | 0x2175 | John | 07 | 00 | John: What's your name, kid? |
| 15 | 0x219B | Sherry | 06 | 00 | Sherry: It's Sherry... |
| 16 | 0x21BB | John | 07 | 00 | John: Sherry, huh? I'm John, and you / can rest assured that I'll keep you / safe from now on! |
| 17 | 0x221F | Elza | 01 | 00 | Elza: You seem to have taken a / liking to her. |
| 18 | 0x2256 | John | 07 | 00 | John: Well, of course, she saved my / life, you know? That's why I'll protect / her with my life! |
| 19 | 0x22BD | Elza | 01 | 00 | Elza: How simplistic... Anyway, Sherry, / how did you know about that entrance? |
| 20 | 0x2314 | Sherry | 06 | 00 | Sherry: This is... where my dad used to / be... |
| 21 | 0x234B | Elza | 01 | 00 | Elza: Your... dad...? |
| 22 | 0x236A | Sherry | 06 | 00 | Sherry: What was that? |
| 23 | 0x238A | John | 07 | 00 | John: I dunno. Sounds like somebody's / in there. |
| 24 | 0x23C3 | Sherry | 06 | 00 | Sherry: There wasn't anybody before. |
| 25 | 0x23F1 | Elza | 01 | 00 | Elza: Let me deal with it. / John, take Sherry somewhere safe. |
| 26 | 0x2437 | John | 07 | 00 | John: Will do! |
| 27 | 0x244F | John | 07 | 00 | John: You stay with me now, you hear? |
| 28 | 0x247E | Sherry | 06 | 00 | Sherry: Yes sir. |
| 29 | 0x2498 | Sherry | 06 | 00 | Sherry: Good luck! |
| 30 | 0x24B4 | Elza | 01 | 00 | Elza: Thanks! |
| 31 | 0x24CB |  | 01 | 02 | You've taken the Latch Key. |
| 32 | 0x24EE |  | 01 | 02 | You've used the Latch Key. |

## ROOM30A1 (STAGE3, 208760 B, msg_sec=0x88C, BYTEGLEICH mit ROOM30A0)

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

## ROOM30B1 (STAGE3, 175620 B, msg_sec=0x9A0, BYTEGLEICH mit ROOM30B0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x09A4 |  |  | 02 | Itembox is not available in / this preview. |
| 1 | 0x09D1 |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |

## ROOM30C1 (STAGE3, 163416 B, msg_sec=0xEA0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0EB6 |  |  | 02 | You've opened the lock. |
| 1 | 0x0ED4 | Elza | 01 | 00 | Elza: Who are you? |
| 2 | 0x0EF0 | Officer | 07 | 00 | Officer: I'm Marvin Branagh, RPD. / I'm here with Leon Kennedy. |
| 3 | 0x0F37 | Elza | 01 | 00 | Elza: Leon?! How is he? / We got separated back at the RPD. |
| 4 | 0x0F7A | Marvin | 07 | 00 | Marvin: He's all right and he's helping / me and Ada escape the city. |
| 5 | 0x0FC7 | Elza | 01 | 00 | Elza: Ada? |
| 6 | 0x0FDB | Marvin | 07 | 00 | Marvin: Someone we rescued from the / RPD. |
| 7 | 0x100D | Elza | 01 | 00 | Elza: Will you tell him that me and John / and Sherry are here and we're all right? |
| 8 | 0x1068 | Marvin | 07 | 00 | Marvin: Will do. See you soon! |
| 9 | 0x1090 |  |  | 02 |  / I need to hurry up and save the others... |
| 10 | 0x10BE |  |  | 02 |  / It's locked. |

## ROOM30D1 (STAGE3, 119948 B, msg_sec=0x13BC, BYTEGLEICH mit ROOM30D0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x13C0 |  |  | 02 | The car is locked, I can't get inside. |
| 1 | 0x13EA |  |  | 02 | The panel is smashed and doesn't work. |

## ROOM30E1 (STAGE3, 263116 B, msg_sec=0xEE4)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0F0A | Elza | 01 | 00 | Elza: John! |
| 1 | 0x0F1F | John | 07 | 00 | John: Elza! So this is where you were? |
| 2 | 0x0F4F | John | 07 | 00 | John: This is bad... one of those monsters / bit me as I tried to get away! |
| 3 | 0x0FA2 | John | 07 | 00 | John: ...And there doesn't seem to be a / place with any medical supplies here... |
| 4 | 0x0FFB | John | 07 | 00 | John: Elza, how are things on your end? / Did you find any tasty food or something? |
| 5 | 0x1056 | Elza | 01 | 00 | Elza: No, nothing like that... |
| 6 | 0x107E | John | 07 | 00 | John: Wh-what's wrong? Are those / walking corpses over there as well... |
| 7 | 0x10CE | Elza | 01 | 00 | Elza: No... not at all... That's... |
| 8 | 0x10FB | John | 07 | 00 | John: What is that...? |
| 9 | 0x111B | John | 07 | 00 | John: What are you talking about? / Is there something in there? |
| 10 | 0x1163 | Elza | 01 | 00 | Elza: I'm not sure what it is, but... / That can't be... human, can it...? |
| 11 | 0x11B5 | John | 07 | 00 | John: Elza, is that thing still in there? |
| 12 | 0x11E8 | Elza | 01 | 00 | Elza: Yes, I think so... But I believe / it got scared and hid when it was / attacked... |
| 13 | 0x1246 | Elza | 01 | 00 | Elza: I don't know where it could've / gone, though. |
| 14 | 0x1282 | Elza | 01 | 00 | Elza: ...Anyway, there are too many / walking corpses outside. We can't / really turn back. |
| 15 | 0x12E3 | Elza | 01 | 00 | Elza: We have no choice but to go / inside... |
| 16 | 0x1318 | John | 07 | 00 | John: But... that beast might still be in / there, right? |
| 17 | 0x1359 | Elza | 01 | 00 | Elza: We can probably deal with it if / we stay alert... Let's go! |
| 18 | 0x13A3 |  |  | 02 | It's too dangerous outside...We need to hurry and get that / elevator working. |

## ROOM30F1 (STAGE3, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM30F0)

_kein Nachrichtenblock_

## ROOM4001 (STAGE4, 328240 B, msg_sec=0x1C84)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1CA2 |  |  | 02 | The door is locked. The text on the / panel reads:Door is under emergency lockdown. / An ID card is required to unlock it." |
| 1 | 0x1D20 |  | 01 | 02 | Will you use the Blue Master Keycard?  |
| 2 | 0x1D4F |  | 01 | 02 | You've used the Blue Master Keycard. |
| 3 | 0x1D7B | Elza | 01 | 00 | Elza: What? What is this place...!? |
| 4 | 0x1DA8 | John | 07 | 00 | John: Wow, this sure is something... |
| 5 | 0x1DD6 | Elza | 01 | 00 | Elza: Sherry, what is this place? |
| 6 | 0x1E01 | John | 07 | 00 | John: Do you know anything about / this place? |
| 7 | 0x1E37 | Sherry | 06 | 00 | Sherry: This is dad's research center. / He should be around here somewhere. |
| 8 | 0x1E8B | Sherry | 06 | 00 | Sherry: I'll go look for him! |
| 9 | 0x1EB2 | John | 07 | 00 | John: Sherry, hold on! |
| 10 | 0x1ED2 | John | 07 | 00 | John: Don't go alone! |
| 11 | 0x1EF1 | Elza | 01 | 00 | Elza: Sherry! |
| 12 | 0x1F08 | John | 07 | 00 | John: Elza, let's go after her! |
| 13 | 0x1F31 |  |  | 02 | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. |
| 14 | 0x1F8A |  |  | 02 | Security Room"It's locked. |

## ROOM4011 (STAGE4, 194208 B, msg_sec=0x658)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0668 |  |  | 02 | It's a row of monitors.They're completely blank, maybe / they're not getting any power? |
| 1 | 0x06C3 |  |  | 02 |  / The desk is completely empty. |
| 2 | 0x06E5 |  |  | 02 |  / This equipment seems to be unusable. |
| 3 | 0x070E |  |  | 02 | There's a memo here.To Michael: I left an ID card on the / right desk.Don't lose it again, okay? - Henry |
| 4 | 0x077C |  |  | 02 |  / There's nothing useful in this locker. |
| 5 | 0x07A7 |  |  | 02 | This freezer isn't on. It smells pretty / bad. |
| 6 | 0x07D7 |  |  | 02 | Itembox is not available in / this preview. |
| 7 | 0x0804 |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |

## ROOM4021 (STAGE4, 178440 B, msg_sec=0xB80, BYTEGLEICH mit ROOM4020)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B84 |  |  | 02 |  / There's a map of the entire area. |
| 1 | 0x0BAA |  |  | 02 |  / The button doesn't respond... |

## ROOM4031 (STAGE4, 187740 B, msg_sec=0x8D0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x08E2 |  |  | 02 | Main Fuse" / It's a large circuit breaker system.It looks like someone has removed all / the fuses. |
| 1 | 0x0948 |  | 01 | 02 | Will you use the Spark Plug?  |
| 2 | 0x096E |  |  | 02 | The fuses are all connected. I should / be able to get to some new places now. |
| 3 | 0x09BE |  |  | 02 | Thanks to the footlights, I can just / barely see the ground. |
| 4 | 0x09FD |  |  | 02 | The lamps seem to flicker now and / then, but... |
| 5 | 0x0A2F | John | 07 | 00 | John: Sherry! |
| 6 | 0x0A46 | John | 07 | 00 | John: What the? / Where did she go!? |
| 7 | 0x0A72 | Elza | 01 | 00 | Elza: Let's split up and look for / her! |
| 8 | 0x0AA2 | John | 07 | 00 | John: Okay! I'll go this way, you / go that way, Elza! |

## ROOM4041 (STAGE4, 156844 B, msg_sec=0x1294, BYTEGLEICH mit ROOM4040)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1296 | Left |  | 02 | Left: Emergency Staircase / Right: Private Rooms |

## ROOM4051 (STAGE4, 221128 B, msg_sec=0x2040, BYTEGLEICH mit ROOM4050)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x2048 |  |  | 02 | There's nothing useful in this / locker. |
| 1 | 0x2072 |  |  | 02 | A researcher's bed. |
| 2 | 0x2089 |  |  | 02 | A cabinet full of books and / reference materials.Most appear to be about / chemistry and biology. |
| 3 | 0x20ED |  |  | 02 | There's a memo here.To David: I found your ID card / in the hallway.Please don't lose it again. / - Eric |

## ROOM4061 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM4060)

_kein Nachrichtenblock_

## ROOM4071 (STAGE4, 102320 B, msg_sec=0x135C, BYTEGLEICH mit ROOM4070)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x135E |  |  | 02 | The door won't open! |

## ROOM4081 (STAGE4, 145212 B, msg_sec=0xF00, BYTEGLEICH mit ROOM4080)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0F02 |  |  | 02 | Cell Cultivation Room"It's electronically locked. |

## ROOM4091 (STAGE4, 118896 B, msg_sec=0xB7C, BYTEGLEICH mit ROOM4090)

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

## ROOM40A1 (STAGE4, 97496 B, msg_sec=0x111C)

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

## ROOM40B1 (STAGE4, 196460 B, msg_sec=0x7CC, BYTEGLEICH mit ROOM40B0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x07D4 |  |  | 02 | It looks like a large vat used to / store research specimens butthere's nothing inside... |
| 1 | 0x0831 |  |  | 02 | It looks like the remains of / a failed experiment... |
| 2 | 0x0868 |  |  | 02 | It's an operating table.There's what seems to be blood / on it... |
| 3 | 0x08AD |  |  | 02 | It's been broken from the inside. |

## ROOM40C1 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40C0)

_kein Nachrichtenblock_

## ROOM40D1 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40D0)

_kein Nachrichtenblock_

## ROOM40E1 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40E0)

_kein Nachrichtenblock_

## ROOM40F1 (STAGE4, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM40F0)

_kein Nachrichtenblock_

## ROOM5001 (STAGE5, 128712 B, msg_sec=0x950)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0952 |  |  | 02 | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. |

## ROOM5011 (STAGE5, 360020 B, msg_sec=0xE4C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0EA8 | Elza | 01 | 00 | Elza: Sherry! Wake up! |
| 1 | 0x0EC8 | John | 07 | 00 | John: It's no good... / She's passed out. |
| 2 | 0x0EF9 | Elza | 01 | 00 | Elza: Ahh... Sherry... |
| 3 | 0x0F19 | Elza | 01 | 00 | Elza: John. I read some files in this / research center, and I found out / some terrifying things. |
| 4 | 0x0F81 | Elza | 01 | 00 | Elza: She's been infected by what / they were working on here. |
| 5 | 0x0FC7 | John | 07 | 00 | John: Elza, have a look at this. |
| 6 | 0x0FF1 | Elza | 01 | 00 | Elza: What is this...? |
| 7 | 0x1011 | John | 07 | 00 | John: If we can't get the vaccine, / Sherry will... |
| 8 | 0x104C | Elza | 01 | 00 | Elza: Okay. I'll go get it. |
| 9 | 0x1071 | John | 07 | 00 | John: What? |
| 10 | 0x1086 | Elza | 01 | 00 | Elza: While we're talking, the virus / is spreading inside Sherry's body. |
| 11 | 0x10D7 | Elza | 01 | 00 | Elza: Even if it's just a chance, / I have to take it. |
| 12 | 0x1115 | John | 07 | 00 | John: Elza... |
| 13 | 0x112C | Elza | 01 | 00 | Elza: John, keep a close eye on her. |
| 14 | 0x115A | Elza | 01 | 00 | Elza: Sherry needs you right now. |
| 15 | 0x1185 | John | 07 | 00 | John: Elza, please... |
| 16 | 0x11A4 | John | 07 | 00 | John: ...Please save this girl, even / if it's just her. |
| 17 | 0x11E4 | Elza | 01 | 00 | Elza: John, I've got the vaccine! |
| 18 | 0x120F | Elza | 01 | 00 | Elza: How's Sherry? |
| 19 | 0x122C | John | 07 | 00 | John: Quick! She doesn't have long! |
| 20 | 0x1259 | Elza | 01 | 00 | Elza: Sherry! Wake up! |
| 21 | 0x1279 | John | 07 | 00 | John: Sherry, what's wrong!? / Talk to me! |
| 22 | 0x12AB | Elza | 01 | 00 | Elza: Sherry, please... Wake up...! |
| 23 | 0x12D8 | John | 07 | 00 | John: What's wrong...? |
| 24 | 0x12F8 | Elza | 01 | 00 | Elza: Sherry! You're awake! |
| 25 | 0x131D | Elza | 01 | 00 | Elza: What a relief... |
| 26 | 0x133D | John | 07 | 00 | John: Yes, I was really worried! |
| 27 | 0x1367 | Elza | 01 | 00 | Elza: John, did you hear that / announcement? |
| 28 | 0x139C | John | 07 | 00 | John: Yeah. What's going on!? |
| 29 | 0x13C3 | Elza | 01 | 00 | Elza: It seems like that beast broke / the place's power core. |
| 30 | 0x1409 | Elza | 01 | 00 | Elza: And fire's starting to spread / all over the place. |
| 31 | 0x144A | John | 07 | 00 | John: What!? That means we're also / in danger! |
| 32 | 0x1481 | Elza | 01 | 00 | Elza: That's right! We can't stay / here, we need to hurry to the shelter! |
| 33 | 0x14D3 | John | 07 | 00 | John: Sherry, do you know where the / shelter is? |
| 34 | 0x150C | Sherry | 06 | 00 | Sherry: Y-yes. |
| 35 | 0x1524 | Sherry | 06 | 00 | Sherry: The second shelter should be / open. |
| 36 | 0x1558 | John | 07 | 00 | John: Alright, let's get going! |
| 37 | 0x1581 | Elza | 01 | 00 | Elza: Lead the way, Sherry! |
| 38 | 0x15A6 |  |  | 02 | There's a bunch of footprints going / in circles.There doesn't seem to be anything / useful here. |
| 39 | 0x1609 |  |  | 02 |  / This desk is very dusty.There's nothing useful on it. |
| 40 | 0x1645 |  |  | 02 | It looks like this was used a little / while ago. |
| 41 | 0x1678 |  |  | 02 | There's a memo board behind a bunch / of machinery.It seems to be mostly notes written / down by the research staff. |
| 42 | 0x16EE |  |  | 02 | The trashcan is full of crumpled / up papers. |
| 43 | 0x171D |  |  | 02 |  / I need to hurry up and get that vaccine! |
| 44 | 0x174A |  |  | 02 | Itembox is not available in / this preview. |
| 45 | 0x1777 |  |  | 02 | It's a computer.You can save your progress / with this.Save is not available in / this preview. |

## ROOM5021 (STAGE5, 164720 B, msg_sec=0x1420)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1424 |  |  | 02 | Sherry must have crawled through here, / but the hole is too small for me.There must be another way to / get back there. |
| 1 | 0x149E | Elza | 01 | 00 | Elza: Sherry... |

## ROOM5031 (STAGE5, 229300 B, msg_sec=0x124C)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1254 |  |  | 02 | A map of the lab is available here.Will you file the laboratory map?  |
| 1 | 0x12A0 |  |  | 02 | You've taken the laboratory map. |
| 2 | 0x12C4 |  |  | 02 | It's a row of monitors.They appear to be used for showing / security footage, but they're onlydisplaying static... |
| 3 | 0x133C | Elza | 01 | 00 | Elza: Sherry! |

## ROOM5041 (STAGE5, 209404 B, msg_sec=0x1B74, BYTEGLEICH mit ROOM5040)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1B76 | Left |  | 02 | Left: P-4 Level Laboratory / Right: Computer Room |

## ROOM5051 (STAGE5, 195312 B, msg_sec=0x938, BYTEGLEICH mit ROOM5050)

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

## ROOM5061 (STAGE5, 270976 B, msg_sec=0x3284)

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
| 13 | 0x35E1 |  | 01 | 02 | You've taken the G-Vaccine. |
| 14 | 0x3604 |  |  | 02 | There is nothing left for me to do / here. |

## ROOM5071 (STAGE5, 204548 B, msg_sec=0xD8C, BYTEGLEICH mit ROOM5070)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0D92 | Left |  | 02 | Left: Shelter 02 / Right: Shelter 01 |
| 1 | 0x0DB8 |  |  | 02 | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |
| 2 | 0x0E27 |  |  | 02 | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |

## ROOM5081 (STAGE5, 101088 B, msg_sec=0x870)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0878 |  |  | 02 | A high yield generator / that supplies power forthe inside facilities / and the train.Will you turn on the / switch?  |
| 1 | 0x08F0 |  |  | 02 | The emergency mode has / been activated.The power supply will be / cut temporarily.The emergency train will / be activated uponrestoration of the power." |
| 2 | 0x098E |  |  | 02 | The door won't open until / the power is restored! |
| 3 | 0x09C2 |  |  | 02 | The power already has been / supplied to the train! |

## ROOM5091 (STAGE5, 191696 B, msg_sec=0x1340)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1344 |  | 01 | 00 | THANK YOU FOR PLAYING !!! |
| 1 | 0x1367 |  |  | 02 | I can't put my friends / in danger.I have no choice but to / stop that creature here! |

## ROOM50A1 (STAGE5, 131832 B, msg_sec=0x19CC, BYTEGLEICH mit ROOM50A0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x19D4 |  |  | 02 | It's the shutter control panel.I can't use it without an ID Card. |
| 1 | 0x1A1B |  |  | 02 | It looks the shutter on the right / leads to a hangar. |
| 2 | 0x1A53 |  |  | 02 | There's a maintenance panel here, but / it doesn't look like it has seen any use. |
| 3 | 0x1AA6 |  |  | 02 | Emergency Staircase"They probably wrote this so that / the researchers wouldn't get lost. |

## ROOM50B1 (STAGE5, 225512 B, msg_sec=0x558)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x055A |  |  | 02 | It's Ada. She's dead.It looks like one of those / gorillas killed her. |

## ROOM50C1 (STAGE5, 105908 B, msg_sec=0xB7C, BYTEGLEICH mit ROOM50C0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0B7E |  |  | 02 | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. |

## ROOM50D1 (STAGE5, 207788 B, msg_sec=0x14A0, BYTEGLEICH mit ROOM50D0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x14A2 |  |  | 02 | There's a button here. Push it?  e         /            4   2  /           /      3  x  x                     |

## ROOM50E1 (STAGE5, 101280 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM50F1 (STAGE5, 101292 B, msg_sec=0x0)

_kein Nachrichtenblock_

## ROOM5101 (STAGE5, 117452 B, msg_sec=0x0, BYTEGLEICH mit ROOM5100)

_kein Nachrichtenblock_

## ROOM5111 (STAGE5, 220360 B, msg_sec=0x0, BYTEGLEICH mit ROOM5110)

_kein Nachrichtenblock_

## ROOM5121 (STAGE5, 165396 B, msg_sec=0x16F4, BYTEGLEICH mit ROOM5120)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x16F8 | Left |  | 02 | Left: P-4 Level Laboratory / Right: Computer Room |
| 1 | 0x172B |  |  | 02 | The door is locked because of / the fire! |

## ROOM5131 (STAGE5, 221440 B, msg_sec=0xBE0, BYTEGLEICH mit ROOM5130)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0BE6 | Left |  | 02 | Left: Shelter 02 / Right: Shelter 01 |
| 1 | 0x0C0C |  |  | 02 | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |
| 2 | 0x0C7B |  |  | 02 | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... |

## ROOM5141 (STAGE5, 188848 B, msg_sec=0x1C48, BYTEGLEICH mit ROOM5140)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1C4A |  |  | 00 |  / It's open on this end. |

## ROOM5151 (STAGE5, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM5150)

_kein Nachrichtenblock_

## ROOM5161 (STAGE5, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM5160)

_kein Nachrichtenblock_

## ROOM5171 (STAGE5, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM5170)

_kein Nachrichtenblock_

## ROOM6001 (STAGE6, 184724 B, msg_sec=0xFA0)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0FA6 |  |  | 02 | A device to open the / tunnel gate.Will you turn on the / switch?  |
| 1 | 0x0FEB |  |  | 02 | It's locked from the other side. |
| 2 | 0x100F |  |  | 02 | A device to open the / tunnel gate.The power is not supplied. |

## ROOM6011 (STAGE6, 145260 B, msg_sec=0x10C4, BYTEGLEICH mit ROOM6010)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x10C6 |  |  | 02 | You've opened the lock. |

## ROOM6021 (STAGE6, 201880 B, msg_sec=0xBDC)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x0BDE |  |  | 02 | Itembox is not available in / this preview. |

## ROOM6031 (STAGE6, 363876 B, msg_sec=0x1130)

| ID | Off | Sp. | Farbe | Modus | Text |
|---:|---:|---|---|---|---|
| 0 | 0x1148 | Elza | 01 | 00 | Elza: Where's Leon? |
| 1 | 0x1165 | Sherry | 06 | 00 | Sherry: He's fighting off monsters. / He wants you to start the train. |
| 2 | 0x11B3 | Elza | 01 | 00 | Elza: I'll do it! |
| 3 | 0x11CE |  |  | 02 | Itembox is not available in / this preview. |
| 4 | 0x11FB |  |  | 02 | I have to get this thing moving / first! |
| 5 | 0x1225 | Sherry | 06 | 00 | Sherry: I know how to start this! / I watched Mom and Dad do it. |
| 6 | 0x126D | Sherry | 06 | 00 | Sherry: Let's see, you push this / here and throw this switch there. |
| 7 | 0x12B9 | Elza | 01 | 00 | Elza: What's wrong? |
| 8 | 0x12D6 | Sherry | 06 | 00 | Sherry: It's doesn't have enough power! |
| 9 | 0x1307 | Sherry | 06 | 00 | Sherry: This says the train's main / batteries need to be fueled! |
| 10 | 0x1350 | Elza | 01 | 00 | Elza: Let me take care of that. / You start the train once it's ready. |
| 11 | 0x139E |  |  | 02 | You can't get off the / train right now. |

## ROOM6041 (STAGE6, 163048 B, msg_sec=0x0, BYTEGLEICH mit ROOM6040)

_kein Nachrichtenblock_

## ROOM6051 (STAGE6, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM6050)

_kein Nachrichtenblock_

## ROOM6061 (STAGE6, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM6060)

_kein Nachrichtenblock_

## ROOM6071 (STAGE6, 4 B, msg_sec=0x0, BYTEGLEICH mit ROOM6070)

_kein Nachrichtenblock_

